/*! \file bcma_cli_parse_array.c
 *
 * This module parses a long string into array data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bcma/cli/bcma_cli_parse.h>

/*
 * Remove a specified character from the given string.
 */
static void
str_rm_chr(char *src, char c)
{
    if (src) {
        char *dst = src;

        do {
            if (*src != c) {
                *dst++ = *src;
            }
        } while (*src++ != '\0');
    }
}

/*!
 * \brief Evaluate the string data width in bits. Only support in HEX format.
 *
 * \param [in] str String to Evaluate.
 * \param [in] skip_lead_zeros Ignore the digits of the leading 0s.
 *
 * \return The data width in bits or -1 when the input string is malformatted.
 */
static int
str_data_width_get(char *str, bool skip_lead_zeros)
{
    if (str[0] =='0' && (str[1] == 'x' || str[1] == 'X')) {
        char *ptr = str + 2;

        if (skip_lead_zeros) {
            while (*ptr == '0') {
                ptr++;
            }
        }

        if (*ptr) {
            return 4 * sal_strlen(ptr);
        }
        return 0;
    }

    return -1;
}

/*
 * Callback function of parse_data_array() to parse uint8_t data.
 */
static void
parse_uint8(const char *str, void *el)
{
    uint8_t *val = el;

    *val = (uint8_t)(sal_strtoul(str, NULL, 16) & 0xff);
}

/*
 * Callback function of parse_data_array() to parse uint32_t data.
 */
static void
parse_uint32(const char *str, void *el)
{
    uint32_t *val = el;

    *val = (uint32_t)sal_strtoul(str, NULL, 16);
}

/*
 * Callback function of parse_data_array() to parse uint64_t data.
 */
static void
parse_uint64(const char *str, void *el)
{
    uint64_t *val = el;

    *val = (uint64_t)sal_strtoull(str, NULL, 16);
}

/*
 * Parse a hex format string starting with 0x into data array.
 */
static int
parse_array_data(const char *str, void *data, int data_max, int el_size,
                 void (*parse_fn)(const char *el_str, void *el_val))
{
    int cnt = 0;
    char *p, *s;
    uint8_t *el = data;
    int el_strlen = 2 * el_size;

    /* Ignore the leading 0x */
    s = sal_strdup(str + 2);
    if (s == NULL) {
        return 0;
    }

    /* Get the position of the first data element */
    p = s + (sal_strlen(s) - el_strlen);
    if (s > p) {
        p = s;
    }
    while (p >= s) {
        parse_fn(p, el);
        el += el_size;
        cnt++;
        if (p == s || cnt == data_max) {
            break;
        }
        *p = '\0';
        p = (p < s + el_strlen) ? s : p - el_strlen;
    }

    /* Check if the remaining digits are all zero's */
    while (p > s) {
        if (*--p != '0') {
            /*
             * Return failure for the field value width
             * is greater than 64-bit * array size.
             */
            cnt = 0;
            break;
        }
    }

    sal_free(s);

    return cnt;
}

int
bcma_cli_parse_array_uint8(char *str, int data_max, uint8_t *data)
{
    int bit_width, numel = 1;
    uint32_t val = 0;

    if (str == NULL) {
        return -1;
    }

    /* Remove underscore in the string if any. */
    str_rm_chr(str, '_');

    if (!bcma_cli_parse_is_int(str)) {
        return -1;
    }

    bit_width = str_data_width_get(str, false);
    if (bit_width > 0) {
        numel = (bit_width + 7) / 8;
    }

    /* Query for actual data size */
    if (data_max <= 0 || data == NULL) {
        return numel;
    }
    /* Insufficient array size */
    if (data_max < numel) {
        return -1;
    }

    sal_memset(data, 0, sizeof(uint8_t) * data_max);

    /* Check for the hex string > 8-bit */
    if (bit_width > 8) {
        numel = parse_array_data(str, data, data_max,
                                 sizeof(uint8_t), parse_uint8);
        return numel > 0 ? numel : -1;
    }

    /* Use uint32 parser by default */
    if (bcma_cli_parse_uint32(str, &val) < 0) {
        return -1;
    }
    data[0] = (uint8_t)(val & 0xff);
    return 1;
}

int
bcma_cli_parse_array_uint32(char *str, int data_max, uint32_t *data)
{
    int bit_width, numel = 1;

    if (str == NULL) {
        return -1;
    }

    /* Remove underscore in the string if any. */
    str_rm_chr(str, '_');

    if (!bcma_cli_parse_is_int(str)) {
        return -1;
    }

    bit_width = str_data_width_get(str, false);
    if (bit_width > 0) {
        numel = (bit_width + 31) / 32;
    }

    /* Query for actual data size */
    if (data_max <= 0 || data == NULL) {
        return numel;
    }
    /* Insufficient array size */
    if (data_max < numel) {
        return -1;
    }

    sal_memset(data, 0, sizeof(uint32_t) * data_max);

    /* Check for the hex string > 32-bit */
    if (bit_width > 32) {
        numel = parse_array_data(str, data, data_max,
                                 sizeof(uint32_t), parse_uint32);
        return numel > 0 ? numel : -1;
    }

    /* Use uint32 parser by default */
    if (bcma_cli_parse_uint32(str, &data[0]) < 0) {
        return -1;
    }
    return 1;
}

int
bcma_cli_parse_array_uint64(char *str, int data_max, uint64_t *data)
{
    int bit_width, numel = 1;

    if (str == NULL) {
        return -1;
    }

    /* Remove underscore in the string if any. */
    str_rm_chr(str, '_');

    if (!bcma_cli_parse_is_int(str)) {
        return -1;
    }

    bit_width = str_data_width_get(str, false);
    if (bit_width > 0) {
        numel = (bit_width + 63) / 64;
    }

    /* Query for actual data size */
    if (data_max <= 0 || data == NULL) {
        return numel;
    }
    /* Insufficient array size */
    if (data_max < numel) {
        return -1;
    }

    sal_memset(data, 0, sizeof(uint64_t) * data_max);

    /* Check for the hex string > 64-bit */
    if (bit_width > 64) {
        numel = parse_array_data(str, data, data_max,
                                 sizeof(uint64_t), parse_uint64);
        return numel > 0 ? numel : -1;
    }

    /* Use uint64 parser by default */
    if (bcma_cli_parse_uint64(str, &data[0]) < 0) {
        return -1;
    }
    return 1;
}
