/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * \file:    utilex_str.c
 * Purpose:    Misc. routines used by export/import/show facilities
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/appl/sal.h>
#include <sal/types.h>

#include <shared/util.h>
#include <shared/error.h>
#include <shared/utilex/utilex_str.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

static const char utilex_special_full[] = {
    ',',
    ' ',
    '#',
    '&',
    '$',
    '(',
    ')',
    '[',
    ']',
    '/',
    ':',
    '=',
    '\\',
    '\'',
    '\n',
    '\t',
    '\r',
    0
};

void
utilex_escape_quote(
    char *source,
    char *target,
    int target_size)
{
    int i_src, i_target = 0;
    int loc_len;

    loc_len = sal_strlen(source);
    for (i_src = 0; i_src < loc_len; i_src++)
    {
        if (source[i_src] == '"')
        {
            target[i_target++] = '\\';
            target[i_target++] = '"';
            target[i_target++] = ' ';
        }
        else
        {
            target[i_target++] = source[i_src];
        }
        if (i_target + 2 >= target_size)
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Target size if not enough for:%s\n"), source));
            break;
        }
    }
    /*
     * NULL terminate the string
     */
    target[i_target] = 0;
}

void
utilex_str_append(
    char *base,
    char *add)
{
    if (strlen(base) != 0)
        sal_strcpy(base + strlen(base), " ");
    sal_strcpy(base + strlen(base), add);
}

void
utilex_str_fill(
    char *str,
    int num,
    char ch)
{
    int i;
    for (i = 0; i < num; i++)
        str[i] = ch;

    str[num] = 0;
}

char **
utilex_str_split(
    char *string,
    char *delim,
    uint32 maxtokens,
    uint32 *realtokens)
{
    char **tokens = NULL;
    int i = 0;
    char *next;

    if (ISEMPTY(string))
        goto exit;

    tokens = sal_alloc(sizeof(char *) * maxtokens, "tokens");
    if (tokens == NULL)
        goto exit;
    /*
     * Assign all pointers NULL
     */
    for (i = 0; i < maxtokens; i++)
        tokens[i] = NULL;

    i = 0;
    while (((next = sal_strstr(string, delim)) != NULL) && (i < (maxtokens - 1)))
    {
        if ((tokens[i] = sal_alloc(sal_strlen(string) + 1, "token")) == NULL)
            goto exit;
        sal_memset(tokens[i], 0, sal_strlen(string) + 1);
        /*
         * Copy strictly number of characters until next
         */
        sal_memcpy(tokens[i++], string, next - string);
        /*
         * move string to after the delimiter
         */
        string = next + sal_strlen(delim);
    }
    /*
     * Once end of line or maxtokens achieved - string will keep the last part
     */
    if ((tokens[i] = sal_alloc(sal_strlen(string) + 1, "token")) == NULL)
        goto exit;
    sal_memset(tokens[i], 0, sal_strlen(string) + 1);
    sal_strncpy(tokens[i++], string, sal_strlen(string));

exit:
    *realtokens = i;
    return tokens;
}

void
utilex_str_split_free(
    char **tokens,
    uint32 token_num)
{
    int i = 0;

    if (tokens == NULL)
    {
        return;
    }

    for (i = 0; i < token_num; i++)
    {
        if (tokens[i] != NULL)
            sal_free(tokens[i]);
    }

    sal_free(tokens);
    return;
}

void
utilex_str_swap_long(
    uint8 *buffer,
    int uint32_num)
{
    uint32 tmp_value;
    uint8 *tmp_array = (uint8 *) (&tmp_value);
    int i;

    for (i = 0; i < uint32_num; i++)
    {
        tmp_value = *((uint32 *) buffer);
        buffer[0] = tmp_array[3];
        buffer[1] = tmp_array[2];
        buffer[2] = tmp_array[1];
        buffer[3] = tmp_array[0];
        buffer += 4;
    }
    return;
}

void
utilex_str_replace(
    char *str,
    char from,
    char to)
{
    int ind;
    int loc_len;

    loc_len = sal_strlen(str);
    for (ind = 0; ind < loc_len; ind++)
    {
        if (str[ind] == from)
        {
            str[ind] = to;
        }
    }
    return;
}

void
utilex_str_escape(
    char *str,
    char replacement)
{
    int ind;
    int loc_len;

    loc_len = sal_strlen(str);
    for (ind = 0; ind < loc_len; ind++)
    {
        if ((str[ind] == ',') || (str[ind] == ' ') || (str[ind] == '#') || (str[ind] == '&') || (str[ind] == '$')
            || (str[ind] == '(') || (str[ind] == ')'))
        {
            str[ind] = replacement;
        }
    }
    return;
}

void
utilex_str_escape_xml(
    char *str,
    char replacement)
{
    int ind;
    int loc_len;

    loc_len = sal_strlen(str);
    for (ind = 0; ind < loc_len; ind++)
    {
        int i_char = 0;
        while (utilex_special_full[i_char])
        {
            if (str[ind] == utilex_special_full[i_char])
            {
                str[ind] = replacement;
                break;
            }
            i_char++;
        }
    }
    return;
}

shr_error_e
utilex_str_verify(
    char *str)
{
    int ind;
    int loc_len;

    loc_len = sal_strlen(str);
    for (ind = 0; ind < loc_len; ind++)
    {
        int i_char = 0;
        while (utilex_special_full[i_char])
        {
            if (str[ind] == utilex_special_full[i_char])
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META("String '%s' contains illegal character '%c'\n"), str, str[ind]));
                return _SHR_E_PARAM;
            }
            i_char++;
        }
    }
    return _SHR_E_NONE;
}

void
utilex_str_replace_whitespace(
    char *dest,
    char *source)
{
    int i;
    int loc_len;

    loc_len = sal_strlen(source);
    for (i = 0; i < loc_len; i++)
    {
        if (source[i] == ' ')
            dest[i] = '_';
        else
            dest[i] = source[i];
    }
    dest[i] = 0;
    return;
}

void
utilex_str_replace_eol(
    char *str,
    char replacement)
{
    int i;
    int loc_len;

    loc_len = sal_strlen(str);
    for (i = 0; i < loc_len; i++)
    {
        if (str[i] == '\n')
        {
            str[i] = replacement;
        }
    }
    return;
}

void
utilex_str_to_lower(
    char *str)
{
    int i;
    int loc_len;

    loc_len = sal_strlen(str);
    for (i = 0; i < loc_len; i++)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] += 'a' - 'A';
        }
    }
    return;
}

void
utilex_str_to_upper(
    char *str)
{
    int i;
    int loc_len;

    loc_len = sal_strlen(str);
    for (i = 0; i < loc_len; i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] -= 'a' - 'A';
        }
    }
    return;
}

int
utilex_str_get_shift(
    char *string,
    int token_size)
{
    int i_char;
    int actual_size;

    actual_size = (token_size < strlen(string)) ? token_size : strlen(string);
    /*
     * First check if we have new line inside
     */
    for (i_char = 0; i_char < actual_size; i_char++)
    {
        if ((string[i_char] == '\t') || (string[i_char] == '\n'))
        {       /* return the offset */
            return i_char + 1;
        }
    }
    /*
     * If what is left from string is less then token, just return the leftover
     */
    if (actual_size < token_size)
    {
        return actual_size;
    }
    /*
     * No eol, look for white space
     */
    for (i_char = actual_size - 1; i_char > 0; i_char--)
    {
        if (string[i_char] == ' ')
        {
            return i_char + 1;
        }
    }
    /*
     * We reached this place means that there is no white space inside token_size
     */
    return actual_size;
}

void
utilex_str_shrink(
    char *str)
{
    int i_char;
    int shift = 0;
    char last_char = 0;
    int str_size = sal_strlen(str);
    /*
     * Step 1:shrink all white spaces to one
     */
    for (i_char = 0; i_char < str_size; i_char++)
    {
        if ((str[i_char] == ' ') && (last_char == ' '))
            shift++;
        else
        {
            if (shift != 0)
                str[i_char - shift] = str[i_char];
        }
        last_char = str[i_char];
    }
    /*
     * Step 2:zero all the dirty characters left at the end of string
     */
    for (i_char = str_size - 1; i_char > str_size - 1 - shift; i_char--)
    {
        str[i_char] = 0;
    }

    /*
     * Step 3:zero all trailing white spaces, dots and tabs
     */
    for (i_char = sal_strlen(str) - 1; i_char > 0; i_char--)
    {
        if ((str[i_char] == ' ') || (str[i_char] == '.') || (str[i_char] == '\t') || (str[i_char] == '\n'))
            str[i_char] = 0;
        else
            break;
    }

    return;
}

void
utilex_str_white_spaces_remove(
    char *str)
{
    char *i = str;
    char *j = str;

    while (*j != 0)
    {
        *i = *j++;
        if (*i != ' ')
            i++;
    }
    *i = 0;
}

void
utilex_str_clean(
    char **str_p)
{
    char *str = *str_p;
    int current = sal_strlen(str);

    /*
     * First cleaning from the end
     */
    while (current)
    {
        current--;
        if ((str[current] == ' ') || (str[current] == ',') || (str[current] == '\t') || (str[current] == '\r') ||
            (str[current] == '\n'))
        {
            str[current] = 0;
        }
        else
            break;
    }
    /*
     * Now from the beginning, same check only we move start of the string to none of empty characters
     */
    current = 0;
    while (current < sal_strlen(str))
    {
        if ((str[current] != ' ') && (str[current] != ',') && (str[current] != '\t') && (str[current] != '\r') &&
            (str[current] != '\n'))
            break;
        current++;
    }
    str += current;
    *str_p = str;
}

int
utilex_char_is_digit(
    unsigned char mychar)
{
    if ((mychar >= '0') && (mychar <= '9'))
        return TRUE;
    else
        return FALSE;
}

unsigned char
utilex_char_to_upper(
    unsigned char mychar)
{
    if ((mychar >= 97) && (mychar <= 122))
        return mychar - 32;
    else
        return mychar;
}

unsigned char
utilex_char_to_lower(
    unsigned char mychar)
{
    if ((mychar >= 65) && (mychar <= 90))
        return mychar + 32;
    else
        return mychar;
}

int
utilex_str_is_decimal(
    char *str)
{
    int i;
    char ch;
    int loc_len;

    loc_len = sal_strlen(str);
    for (i = 0; i < loc_len; i++)
    {
        ch = str[i];
        if ((ch < '0') || (ch > '9'))
            return FALSE;
    }
    return TRUE;
}

int
utilex_str_is_number(
    char *str)
{
    int i, is_hex = FALSE, i_start = 0;
    char ch;
    int loc_len;

    loc_len = sal_strlen(str);
    if ((str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X')))
    {
        is_hex = TRUE;
        i_start = 2;
    }

    for (i = i_start; i < loc_len; i++)
    {
        ch = str[i];

        if (((ch >= '0') && (ch <= '9')) ||
            ((is_hex == TRUE) && (((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))))
        {
            continue;
        }
        else
        {
            return FALSE;
        }
    }
    return TRUE;
}

uint32
utilex_str_stoul(
    char *str,
    uint32 *value_p)
{
    int i;
    char ch;
    int loc_len;

    loc_len = sal_strlen(str);
    for (i = 0; i < loc_len; i++)
    {
        ch = str[i];
        if ((i == 1) && ((ch == 'x') || (ch == 'X')))
        {
            continue;
        }
        if (((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))
        {
            continue;
        }
        else
        {
            return _SHR_E_PARAM;
        }
    }
    if (value_p != NULL)
    {
        *value_p = _shr_ctoi(str);
    }

    return _SHR_E_NONE;
}

uint32
utilex_str_xstoul(
    char *str,
    uint32 *value_p)
{

    int val_size;
    int nibble_count;
    int ptr_delta;

    /** In case the hex string is empty, return 0 which will be interpreted in the calling function as error */
    if (sal_strlen(str) == 0)
    {
        return _SHR_E_PARAM;
    }

    /** trim padding zeros - because they'll interfere with actual number bytes count (string length without padding zeros)
     *  trim only if there's not the case of a single '0' in string
     *  do this by moving the str pointer up until no '0' is observed
     */

    while ((*str == '0') && (sal_strlen(str) > 1))
    {
        str++;
    }

    /** Each string char represents a byte, so we want to calculate number of uint32 in the hex string */
    val_size = (sal_strlen(str) + 7) / 8;

    /** For MSB uint32, 'nible_count' counts how many nibbles are already present and when to skip to next uint32
     *  nibble_count initial value would determine after how many bit shifts to jump to next word in MSB word
     *  max uint32 value is FFFF FFFF, i.e 8 bytes
     */
    nibble_count = 8 - sal_strlen(str) % 8;

    /** value should not be 8, it's out of the margins */
    if (nibble_count == 8)
    {
        nibble_count = 0;
    }

    /** The MSB word location in value_p array, would be the number of words - 1 */
    ptr_delta = val_size - 1;

    while (*str)
    {
        /** get current character then increment */
        uint8 byte = *str++;
        /** transform hex character to the 4bit equivalent number, using the ascii table indexes */
        if (byte >= '0' && byte <= '9')
        {
            byte = byte - '0';
        }
        else if (byte >= 'a' && byte <= 'f')
        {
            byte = byte - 'a' + 10;
        }
        else if (byte >= 'A' && byte <= 'F')
        {
            byte = byte - 'A' + 10;
        }
        else
        {

            return _SHR_E_PARAM;
        }

        /** shift 4 to make space for new digit, and add the 4 bits of the new digit */
        value_p[ptr_delta] = (value_p[ptr_delta] << 4) | (byte & 0xF);

        /** each 8 nibbles we will go to lower significant word - the shift operation opearates only on uint32 scope */
        nibble_count++;
        if (nibble_count == 8)
        {
            nibble_count = 0;
            ptr_delta--;
        }
    }

    return _SHR_E_NONE;
}

/*
 * ---------------------------------------------------------------------------
 * Long Integer Support
 * ---------------------------------------------------------------------------
 *
 * Long integers consist of a variable length array of uint32.
 *
 * Within the array, the least significant word comes first.  This is
 * true on all platforms.  However, each word itself is stored in the
 * platform native byte order.
 *
 * Read a long integer: can read a long hex integer, or a regular
 * integer in any base supported by parse_integer.
 *
 * val[0] receives the least significant word (little-endian).
 * nval is the number size (count of uint32's).
 */

shr_error_e
utilex_str_long_stoul(
    char *str,
    uint32 *val,
    int nval)
{
    char eight[11], *s, *t;
    int i;
    char ch;
    int loc_len;

    loc_len = sal_strlen(str);
    for (i = 0; i < loc_len; i++)
    {
        ch = str[i];
        if ((i == 1) && ((ch == 'x') || (ch == 'X')))
            continue;
        if (((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))
            continue;
        else
            return _SHR_E_PARAM;
    }

    sal_memset(val, 0, nval * sizeof(*val));

    /*
 */
    if (str[0] != '0' || (str[1] != 'x' && str[1] != 'X'))
    {
        val[0] = _shr_ctoi(str);
        goto exit;
    }

    /*
     * Skip to the last hex digit in the string 
     */

    for (s = str + 1; isxdigit((unsigned) s[1]); s++)
    {
        ;
    }

    /*
     * Parse backward in groups of 8 digits 
     */

    i = 0;

    do
    {
        /*
         * Copy 8 digits backward to form a string "0xdddddddd\0" 
         */
        t = eight + 11;
        *--t = 0;
        while (t > eight + 2 && (*s != 'x' && *s != 'X'))
        {
            *--t = *s--;
        }
        *--t = 'x';
        *--t = '0';

        val[i++] = _shr_ctoi(t);
    }
    while ((*s != 'x' && *s != 'X') && i < nval);

exit:
    return _SHR_E_NONE;
}

void *
utilex_alloc(
    unsigned int size)
{
    void *mem;
    if ((mem = sal_alloc(size, "utilex")) != NULL)
        sal_memset(mem, 0, size);

    return mem;
}

void
utilex_free(
    void *mem)
{
    if (mem != NULL)
        sal_free(mem);
}

int
utilex_pow(
    int n,
    int k)
{
    int i, result = 1;
    for (i = 0; i < k; i++)
        result *= n;

    return result;
}

uint8
utilex_char_to_num(
    uint8 digit)
{
    if (digit >= '0' && digit <= '9')
        return (digit - '0');
    if (digit >= 'a' && digit <= 'f')
        return (digit - 'a' + 10);
    if (digit >= 'A' && digit <= 'F')
        return (digit - 'A' + 10);
    return 0;
}

shr_error_e
utilex_hex_char_array_to_num(
    int unit,
    uint8 *array_of_hex_digits_p,
    uint32 max_sizeof_array_of_hex_digits,
    uint8 *array_of_binary_values_p)
{
    int ii;
    uint8 ca, cb;
    uint32 num_pairs;
    uint8 *bin_values_p;
    uint8 *hex_digits_p;
    uint32 sizeof_array_of_hex_digits;

    SHR_FUNC_INIT_VARS(unit);
    if (max_sizeof_array_of_hex_digits < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n" "Input array is empty. Must have at least 'ending NULL'.Quit.\r\n");
    }
    else if (max_sizeof_array_of_hex_digits == 1)
    {
        /*
         * If there are no ASCII chars on input array, just quit. Do nothing.
         */
        SHR_EXIT();
    }
    else if (((max_sizeof_array_of_hex_digits - 1) % 2) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n"
                     "Input array has an odd number of ASCII characters (%d)\r\n"
                     "==> It must be even. Quit.\r\n", max_sizeof_array_of_hex_digits);
    }
    num_pairs = (max_sizeof_array_of_hex_digits - 1) / 2;
    sal_memset(array_of_binary_values_p, 0, num_pairs);
    sizeof_array_of_hex_digits = sal_strnlen((char *) array_of_hex_digits_p, max_sizeof_array_of_hex_digits - 1);
    if (sizeof_array_of_hex_digits == 0)
    {
        /*
         * If there are no ASCII chars on input array, just quit. Do nothing.
         */
        SHR_EXIT();
    }
    array_of_hex_digits_p[max_sizeof_array_of_hex_digits - 1] = 0;
    hex_digits_p = &array_of_hex_digits_p[sizeof_array_of_hex_digits - 1];
    bin_values_p = &array_of_binary_values_p[num_pairs - 1];
    for (ii = 0; ii < sizeof_array_of_hex_digits; ii++)
    {
        ca = utilex_char_to_num(*hex_digits_p);
        if (ca > 0xF)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n"
                         "1. Input array has a non-hex-digit ASCII char ('%c') on hex char index %d. Quit.\r\n",
                         *hex_digits_p, ii);
        }
        hex_digits_p--;
        ii++;
        if (ii >= sizeof_array_of_hex_digits)
        {
            *bin_values_p = ca;
            break;
        }
        cb = utilex_char_to_num(*hex_digits_p);
        if (cb > 0xF)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "\r\n"
                         "2. Input array has a non-hex-digit ASCII char ('%c') on hex char index %d. Quit.\r\n",
                         *hex_digits_p, ii);
        }
        hex_digits_p--;
        *bin_values_p = (cb << SAL_NIBBLE_NOF_BITS) | ca;
        bin_values_p--;
    }
exit:
    SHR_FUNC_EXIT;
}

void
utilex_sub_string_replace(
    char *str,
    const char *in_sub_str,
    const char *out_sub_str)
{
    int ii, jj;
    int in_sub_str_location, str_len;
    char temp_str[RHFILE_MAX_SIZE] = { 0 };

    str_len = sal_strlen(str);
    in_sub_str_location = ((int) (sal_strstr(str, in_sub_str) - str)) / (sizeof(char));

    for (jj = 0; jj < in_sub_str_location; jj++)
    {
        temp_str[jj] = str[jj];
    }
    ii = jj;

    sal_strncat_s(temp_str, out_sub_str, sizeof(temp_str));
    ii += sal_strlen(out_sub_str);
    jj += sal_strlen(in_sub_str);

    for (; jj < str_len; ii++, jj++)
    {
        temp_str[ii] = str[jj];
    }
    sal_memcpy(str, temp_str, RHFILE_MAX_SIZE);
}
