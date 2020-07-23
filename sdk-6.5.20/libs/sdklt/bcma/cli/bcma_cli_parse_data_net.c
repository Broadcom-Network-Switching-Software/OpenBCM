/*! \file bcma_cli_parse_data_net.c
 *
 * Parse data drivers for net objets
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <shr/shr_util.h>

#include <bcma/cli/bcma_cli_parse.h>

/*!
 * \brief Convert a string to an integer
 *
 * The _strict_strtoul function converts the initial part of the stirng
 * in 's' to an integer value according to the give 'base'.
 *
 * Unlike the strtoul function, the _strict_strtoul function doesn't
 * accept the sign character '+' or '-', and the prefix notation, for example,
 * '0x' for hex is also not accepted.
 *
 * This function  only accepts digit value which is smaller than the 'base' only.
 * The supported base value is a positive value less or euqal to 16.
 *
 * If 'end' is not NULL, _strict_strtoul stores the address of the first
 * invald character in '*end'. Note that when valid digits reach to end of the
 * string, '*end' would be assigned with '\0'.
 *
 * \param [in] s String to conter to interger
 * \param [out] end The first invalid character of the conviersion if not NULL
 * \param [in] base The base for numeric conversion
 *
 * \retval Non-negtive value on success
 * \retval -1 No characters are converted
 */
static int
_strict_strtoul(const char *s, char **end, int base)
{
    int x, val = 0;
    const char *ptr = s;

    if (!ptr) {
        return -1;
    }

    do {
        x = shr_util_xch2i(*ptr);
        if (x < 0 || x >= base) {
            if (end) {
                /* Assigned the first invalid digit */
                *end = (char *)ptr;
            }
            if (ptr == s) {
                /* No valid digits */
                val = -1;
            }
            break;
        }
        val = val * base + x;
        ptr++;
    } while (1);

    return val;
}

/* Parse data driver for type "mac" */

/*
 * parse_macaddr will take a string of the form H:H:H:H:H:H where each
 * H is one or two hex digits, or a string of the form 0xN where N may
 * consist of up to 12 hex digits.
 */

static int
mac_parse_arg(const char *arg, void *data, void **option)
{
    const char *ptr;
    char *endptr;
    int val, len, idx = 0;
    uint8_t macaddr[6] = {0};

    if ((len = sal_strlen(arg)) < 3) {
        return -1;
    }

    if (arg[0] == '0' && (arg[1] == 'x' || arg[1] == 'X')) {
        /* Parse as large hex value for 0xN format */
        if (len > 14) {
            return -1;
        }
        ptr = arg + len - 1;
        arg += 2;
        do {
            val = shr_util_xch2i(*ptr--);
            if (val < 0) {
                return -1;
            }
            macaddr[5 - idx] = val;

            if (ptr >= arg) {
                val = shr_util_xch2i(*ptr--);
                if (val < 0) {
                    return -1;
                }
                macaddr[5 - idx] |= val << 4;
            }
            idx++;
        } while (ptr >= arg);
    } else {
        /* Colon format */
        ptr = arg;
        do {
            val = _strict_strtoul(ptr, &endptr, 16);
            if (val < 0 || val > 0xff) {
                return -1;
            }
            macaddr[idx] = val;

            /* Check separator */
            if (*endptr != ':' && idx < 5) {
                return -1;
            }
            ptr = endptr + 1;
        } while (++idx < 6);

        /* Check for excess characters */
        if (*endptr != '\0') {
            return -1;
        }
    }

    /* Save temp buffer to destination */
    sal_memcpy(data, macaddr, 6);

    return 0;
}

static char *
mac_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    uint8_t *macaddr = (uint8_t *)data;
    int len;

    len = sal_snprintf(buf, bufsz, "%02x:%02x:%02x:%02x:%02x:%02x",
                       macaddr[0], macaddr[1], macaddr[2],
                       macaddr[3], macaddr[4], macaddr[5]);

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_mac = {
    "mac",
    mac_parse_arg,
    mac_format_arg,
    NULL
};

/* Parse data driver for type "ipv4" */

static int
ipv4_parse_arg(const char *arg, void *data, void **option)
{
    const char *ptr;
    char *endptr;
    int val, idx = 0;
    uint32_t ipaddr = 0;

    if (sal_strchr(arg, '.')) {
        /* Dotted format */
        ptr = arg;
        do {
            val = _strict_strtoul(ptr, &endptr, 10);
            if (val > 0xff || val < 0) {
                return -1;
            }
            ipaddr = (ipaddr << 8) | val;

            if (*endptr != '.') {
                break;
            }
            ptr = endptr + 1;
        } while (++idx < 4);

        if (idx != 3 || *endptr != '\0') {
            return -1;
        }
    } else if (bcma_cli_parse_is_int(arg)) {
        ipaddr = (uint32_t)sal_ctoi(arg, NULL);
    } else {
        return -1;
    }

    *(uint32_t *)data = ipaddr;

    return 0;
}

static char *
ipv4_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    uint32_t ipaddr = *((uint32_t *)data);
    int len;

    len = sal_snprintf(buf, bufsz, "%d.%d.%d.%d",
                       (ipaddr >> 24) & 0xff, (ipaddr >> 16) & 0xff,
                       (ipaddr >> 8) & 0xff, ipaddr & 0xff);

    return (len < bufsz) ? buf : "<overrun>";
}

static bcma_cli_parse_data_t parse_data_ipv4 = {
    "ipv4",
    ipv4_parse_arg,
    ipv4_format_arg,
    NULL
};

/* Parse data driver for type "ipv6" */

static int
ipv6_parse_arg(const char *arg, void *data, void **option)
{
    const char *ptr;
    char *endptr;
    int val, tmp, cnt, idx = 0;
    int cmp_start = -1;
    int x[8];

    for (idx = 0; idx < 8; idx++) {
        x[idx] = -1;
    }

    idx = 0;

    ptr = arg;
    do {
        val = _strict_strtoul(ptr, &endptr, 16);

        /* At most 4 digits per separator */
        if (val > 0xffff) {
            return -1;
        } else if (val >= 0) {
            x[idx++] = val;
        }

        if (idx == 8) {
            break;
        }

        /* Check for non-hexadecimal digits */
        if (*endptr == ':') {
            /* Check for compressed format */
            if (*(endptr + 1) == ':') {
                /* Consume the colon */
                endptr++;

                /* Allow compressed format only once */
                if (cmp_start != -1) {
                    return -1;
                }
                cmp_start = idx;

                if (!sal_isxdigit(*(endptr + 1)) && *(endptr + 1) != '\0') {
                    return -1;
                }
            } else {
                /* Accept hex digits only after single colon for the next round */
                if (!sal_isxdigit(*(endptr + 1))) {
                    return -1;
                }
            }
        } else if (*endptr == '.') {
            uint32_t ipaddr = 0;
            void *opt;

            /* Check for IPv4 address */

            /* Need to reparse and overwrite the hex-parsed value previously in decimal */
            idx--;

            /* Check if we have enough room for IPv4 format */
            if (idx > 6) {
                return -1;
            }

            if (ipv4_parse_arg(ptr, &ipaddr, &opt) != 0) {
                return -1;
            }

            x[idx++] = ipaddr >> 16;
            x[idx++] = ipaddr & 0xffff;

            endptr = (char *)ptr + sal_strlen(ptr);

            break;
        } else if (*endptr == '\0'){
            break;
        } else {
            return -1;
        }

        ptr = endptr + 1;
    } while (idx < 8);

    /* Check for excess characters */
    if (*endptr != '\0') {
        return -1;
    }

    if (cmp_start == -1) {
        /* No compress format included, check if all values are assigned properly */
        for (idx = 0; idx < 8; idx++) {
            if (x[idx] == -1) {
                return -1;
            }
        }
    } else {
        /* Expand the compress format */
        cnt = 0;
        for (tmp = idx - 1; tmp >= cmp_start; tmp--) {
            x[7 - cnt] = x[tmp];
            cnt++;
        }

        cnt = cmp_start + cnt;
        cnt = 8 - cnt;

        /* Compress format with no space for 0's */
        if (cnt <= 0) {
            return -1;
        }

        for (idx = 0; idx < cnt; idx++) {
            x[cmp_start + idx] = 0;
        }
    }

    /* Save temp buffer to destination */
    for (idx = 0; idx < 8; idx++) {
        *((uint8_t *)data + idx * 2) = x[idx] >> 8;
        *((uint8_t *)data + idx * 2 + 1) = x[idx] & 0xff;
    }

    return 0;
}

static char *
ipv6_format_arg(const void *data, const void *option, char *buf, int bufsz)
{
    uint8_t *addr = (uint8_t *)data;
    char *zeros = "0:0:0:0:0:0:0:0";
    char *zp, *cp;
    unsigned int v16, idx, len = 0;

    /* Created uncompressed string */
    for (idx = 0; idx < 16; idx += 2) {
        v16 = (addr[idx] << 8) + addr[idx+1];
        sal_snprintf(&buf[len], bufsz-len, "%x:", v16);
        len = sal_strlen(buf);
    }
    buf[len-1] = 0;

    /* Avoid the IPv6 loopback address ::1 */
    if (addr[12] != 0) {
        /* Check for IPv4 address */
        cp = NULL;
        if (sal_memcmp(buf, "0:0:0:0:0:0", 11) == 0) {
            /* IPv4 compatible address */
            cp = ":";
        } else if (sal_memcmp(buf, "0:0:0:0:0:ffff", 14) == 0) {
            /* Mapped IPv4 address */
            cp = "::ffff";
        } else if (sal_memcmp(buf, "0:0:0:0:ffff:0", 14) == 0) {
            /* Translated IPv4 address */
            cp = "::ffff:0";
        }
        if (cp) {
            sal_snprintf(buf, bufsz, "%s:%d.%d.%d.%d",
                         cp, addr[12], addr[13], addr[14], addr[15]);
            return buf;
        }
    }

    /* Replace longest group of two or more zeros with :: */
    zp = zeros;
    for (idx = 0; idx < 7; idx++) {
        if ((cp = sal_strstr(buf, zp)) != NULL) {
            len = sal_strlen(zp);
            /* Check for leading zeros match */
            if (cp == buf) {
                *cp++ = ':';
                len--;
            }
            /* Check for trailing zeros match */
            if (cp[len] == 0) {
                *cp++ = ':';
                len--;
            }
            /* Compress */
            sal_strcpy(cp, &cp[len]);
            break;
        }
        zp += 2;
    }
    return buf;
}

static bcma_cli_parse_data_t parse_data_ipv6 = {
    "ipv6",
    ipv6_parse_arg,
    ipv6_format_arg,
    NULL
};

int
bcma_cli_parse_data_add_net(void)
{
    bcma_cli_parse_data_add(&parse_data_mac);
    bcma_cli_parse_data_add(&parse_data_ipv4);
    bcma_cli_parse_data_add(&parse_data_ipv6);

    return 0;
}

char *
bcma_cli_format_mac_addr(shr_mac_t mac_addr, char *buf, int bufsize)
{
    return mac_format_arg(mac_addr, NULL, buf, bufsize);
}

int
bcma_cli_parse_mac_addr(char *str, shr_mac_t mac_addr)
{
    return mac_parse_arg(str, mac_addr, NULL);
}

char *
bcma_cli_format_ipv4_addr(shr_ip_t ipv4_addr, char *buf, int bufsize)
{
    return ipv4_format_arg(&ipv4_addr, NULL, buf, bufsize);
}

int
bcma_cli_parse_ipv4_addr(char *str, shr_ip_t *ipv4_addr)
{
    return ipv4_parse_arg(str, ipv4_addr, NULL);
}

char *
bcma_cli_format_ipv6_addr(shr_ip6_t ipv6_addr, char *buf, int bufsize)
{
    return ipv6_format_arg(ipv6_addr, NULL, buf, bufsize);
}

int
bcma_cli_parse_ipv6_addr(char *str, shr_ip6_t ipv6_addr)
{
    return ipv6_parse_arg(str, ipv6_addr, NULL);
}
