/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Diagnostic shell support utilities.
 * These routines aren't necessary for the driver itself.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <soc/mem.h>

#include <bcm/pkt.h>
#include <bcm/field.h>
#include <bcm/stack.h>
#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/rx.h>
#if defined(BCM_ESW_SUPPORT)
#include <bcm_int/esw/l2.h>
#endif

#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#include <sal/appl/io.h>

#if defined(BCM_LTSW_SUPPORT)
#include <appl/diag/ltsw/util.h>
#endif

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#if defined(BCM_ESW_SUPPORT)
#include <bcm_int/esw/port.h>
#endif

#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#endif
#if defined(BCM_DFE_SUPPORT)
#include <soc/dfe/cmn/dfe_drv.h>
#endif

#if defined(BCM_ESW_SUPPORT)
#include <bcm_int/esw/field.h>
#endif

#include <bcm_int/api_xlate_port.h>

typedef struct _bcm_util_gport_dest_s {
    bcm_port_t      port;
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             mpls_id;
    int             mim_id;
    int             wlan_id;
    int             subport_id;
    int             subport_group_id;
    int             vlan_port_id;
    int             vxlan_id;
    int             trill_id;
    int             niv_id;
    int             l2gre_id;
    int             extender_id;
    int             scheduker_id;
    uint32          gport_type;
} _bcm_util_gport_dest_t;

/*
 * strcaseindex: case insensitive substring search
 *   Find substring 'sub' within string 's'.
 *   Return pointer to substring within 's', NULL if not found.
 */

const char *
strcaseindex(const char *s, const char *sub)
{
    int len;

    for (len = sal_strlen(sub); *s; s++) {
        if (sal_strncasecmp(s, sub, len) == 0) {
            return s;
        }
    }

    return 0;
}

/*
 * strrepl: string replace
 *   Delete 'len' characters from position 'start' in string 's',
 *   and insert 'repstr' at the deletion point.
 */

char *
strrepl(char *s, int start, int len, const char *repstr)
{
    memmove(s + start + sal_strlen(repstr),
            s + start + len,
            sal_strlen(s + start + len) + 1);
    memmove(s + start, repstr, sal_strlen(repstr));

    return s;
}

/*
 * Convert hex digit to hex character
 */

int
i2xdigit(int digit)
{
    digit &= 0xf;

    return (digit > 9) ? digit - 10 + 'a' : digit + '0';
}

/*
 * Convert hex character to digit
 */

int
xdigit2i(int digit)
{
    if (digit >= '0' && digit <= '9') return (digit - '0'     );
    if (digit >= 'a' && digit <= 'f') return (digit - 'a' + 10);
    if (digit >= 'A' && digit <= 'F') return (digit - 'A' + 10);
    return 0;
}

/*
 * tabify_line
 *
 *      Tabifies a string assuming a given tab_width.
 *
 *      Output buffer should have at least as many characters as input
 *      string plus the NUL terminator.
 */

void
tabify_line(char *out, char *in, int tab_width)
{
    int         col = 0;

    if (tab_width < 2) {
        tab_width = 1;
    }

    while (*in) {
        int     ts;     /* Spaces to next tab stop */
        int     ns;     /* Spaces that can be eaten from input */

        ts = col + tab_width - (col % tab_width) - col;

        for (ns = 0; ns < ts && in[ns] == ' '; ns++)
            ;

        if (ts > 1 && ns >= ts) {
            *out++ = '\t';
            col += ts;
            in += ts;
        } else {
            *out++ = *in++;
            col++;
        }
    }

    *out = 0;
}

/*
 * parse_rand
 *
 *      Returns non-zero if the string is of the form
 *              rand(x)
 *      or
 *              rand(x,y)
 *      and zero otherwise.
 *
 *      In the case of rand(x), val receives a random value between 0
 *      and x-1, inclusive.
 *
 *      In the case of rand(x,y), val receives a random value between x
 *      and y, inclusive.
 */

int
parse_rand(char *str, uint32 *val)
{
    uint32              min, max, rnd32;

    if (sal_strncasecmp(str, "rand(", 5) != 0) {
        return 0;
    }

    min = sal_ctoi(str + 5, &str);

    if (*str == ',') {
        max = sal_ctoi(str + 1, &str);
    } else {
        max = min - 1;
        min = 0;
    }

    if (*str != ')') {
        return 0;
    }

    rnd32 = (sal_rand() << 17 ^
             sal_rand() << 13 ^
             sal_rand() << 5 ^
             sal_rand() >> 3);

    if (max - min + 1 == 0) {
        *val = 0;
    } else {
        *val = min + rnd32 % (max - min + 1);
    }

    return 1;
}

/*
 * Return true if a constant is a well-formed integer of the type
 * supported by parse_integer.
 */

int
isint(char *s)
{
    int base;
    uint32 dummy;

    if (s == NULL) {
        return 0;
    }

    if (parse_rand(s, &dummy)) {
        return 1;
    }

    if (*s == '-') {
        s++;
    }

    if (*s == '0') {
        if (s[1] == 'b' || s[1] == 'B') {
            base = 2;
            s += 2;
        } else if (s[1] == 'x' || s[1] == 'X') {
            base = 16;
            s += 2;
        } else
            base = 8;
    } else {
        base = 10;
    }

    do {
        if (!isxdigit((unsigned) *s) || xdigit2i(*s) >= base) {
            return(0);
        }
    } while (*++s);

    return(1);
}

/*
 * Read an integer: return unsigned representation.
 * Number format explained below.
 *
 * Expects:
 * [-]0x[0-9|A-F|a-f]+   -hexadecimal if the string begins with "0x"
 * [-][0-9]+             -decimal integer
 * [-]0[0-7]+            -octal integer
 * [-]0b[0-1]+           -binary if the string begins with "0b"
 */

uint32
parse_integer(char *str)
{
    uint32      rnd;

    if (parse_rand(str, &rnd)) {
        return rnd;
    }

    if (!isint(str)) {
        cli_out("WARNING: truncated malformed integer \"%s\"\n", str);
    }

    return _shr_ctoi(str);
}

/*
 * Read string of integer
 * argument s is a string with integers smaller than 32, sepereted by some char that is not digit
 * the return pbmp is a pbmp of this numbers.
 * if one of the numbers on the string is in hex form "0x" its taken as bit map.
 * if "-1" set one to all bit map.
 * example from: "0xf,7" - it will fill the bit pbmp : 1 1 1 1 0 0 0 1
 */
int
parse_small_integers(int unit, char *s, soc_pbmp_t *pbmp)
{
    int integer, i;

    BCM_PBMP_CLEAR(*pbmp);

    while (*s != 0) {

        if ((*s <= '9') && (*s >= '0')) {
            integer = _shr_ctoi(s);

            /* if the number is in hex form use it as bit map */
            if ((*s=='0' && ((*(s+1)=='x') || (*(s+1) == 'X'))) ) {
                for(i=0; i<32; ++i) {

                    if (integer & (1 << i)) {
                         BCM_PBMP_PORT_ADD(*pbmp, i);
                    }
                }
                s = s+2;

            } /* alse -  block index */
             else if ((integer >= 0) && (integer < 32)) {
                    BCM_PBMP_PORT_ADD(*pbmp, integer);
            }

            while ((*s <= '9') && (*s >= '0')) {
                ++s;
            }

        }

        if (*s == '-' && *(s+1) == '1') {
            for(i=0; i<32; ++i) {
                BCM_PBMP_PORT_ADD(*pbmp, i);
            }

            break;

        }

        if (*s != 0) {
            ++s;
        }
    }

    return 0;
}

sal_vaddr_t
parse_address(char *str)
{
    if (!isint(str)) {
        cli_out("WARNING: truncated malformed address \"%s\"\n", str);
    }

    return _shr_ctoa(str);
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
 */

/*
 * Read a long integer: can read a long hex integer, or a regular
 * integer in any base supported by parse_integer.
 *
 * val[0] receives the least significant word (little-endian).
 * nbuf is the number size (count of uint32's).
 */

void
parse_long_integer(uint32 *val, int nval, char *str)
{
    char eight[11], *s, *t;
    int i, neg;

    if (*str == '-') {
        neg = 1;
        str++;
    } else {
        neg = 0;
    }

    sal_memset(val, 0, nval * sizeof (*val));

    if (parse_rand(str, &val[0])) {
        goto done;
    }

    if (str[0] != '0' || (str[1] != 'x' && str[1] != 'X')) {
        val[0] = parse_integer(str);
        goto done;
    }

    /* Skip to the last hex digit in the string */

    for (s = str + 1; isxdigit((unsigned) s[1]); s++) {
        ;
    }

    /* Parse backward in groups of 8 digits */

    i = 0;

    do {
        /* Copy 8 digits backward to form a string "0xdddddddd\0" */
        t = eight + 11;
        *--t = 0;
        while (t > eight + 2 && (*s != 'x' && *s != 'X')) {
            *--t = *s--;
        }
        *--t = 'x';
        *--t = '0';

        val[i++] = parse_integer(t);
    } while ((*s != 'x' && *s != 'X') && i < nval);

 done:
    if (neg) {
        uint32 cy = 1;
        for (i = 0; i < nval; i++) {
            if ((val[i] = (~val[i]) + cy) != 0) {
                cy = 0;
            }
        }
    }
}

/*
 * Format a long integer.  If the value is less than 10, generates
 * decimal, otherwise generates hex.
 *
 * val[0] is the least significant word.
 * nval is the number of uint32's in the value.
 */

void
format_long_integer(char *buf, uint32 *val, int nval)
{
    int i;

    for (i = nval - 1; i > 0; i--) {    /* Skip leading zeroes */
        if (val[i]) {
            break;
        }
    }

    if (i == 0 && val[i] < 10) {        /* Only a single word < 10? */
        sal_sprintf(buf, "%d", val[i]);
    } else {
        sal_sprintf(buf, "0x%x", val[i]);   /* Print first word */
    }

    while (--i >= 0) {                  /* Print rest of words, if any */
        sal_sprintf(buf + sal_strlen(buf), "%08x", val[i]);
    }
}

void
format_spec_long_integer(char *buf, uint32 *val, int nval)
{
    int i = nval - 1;

    sal_sprintf(buf, "0x%08X", val[i]);

    while (--i >= 0) {                  /* Print rest of words, if any */
        sal_sprintf(buf + sal_strlen(buf), " ");
        sal_sprintf(buf + sal_strlen(buf), "0x%08X", val[i]);
    }
}

/*
 * Add long integers.
 */

void
add_long_integer(uint32 *dst, uint32 *src, int nval)
{
    int         i;
    uint32      tmp, new_cy, cy = 0;

    for (i = 0; i < nval; i++, cy = new_cy) {
        tmp = dst[i];           /* Add digits, detect overflow */
        dst[i] += src[i];
        new_cy = (dst[i] < tmp);
        tmp = dst[i];           /* Add carry, detect overflow */
        dst[i] += cy;
        new_cy += (dst[i] < tmp);
    }
}

/*
 * Subtract long integers.
 */

void
sub_long_integer(uint32 *dst, uint32 *src, int nval)
{
    int         i;
    uint32      tmp, new_br, br = 0;

    for (i = 0; i < nval; i++, br = new_br) {
        tmp = dst[i];           /* Sub digits, detect underflow */
        dst[i] -= src[i];
        new_br = (dst[i] > tmp);
        tmp = dst[i];           /* Sub carry, detect underflow */
        dst[i] -= br;
        new_br += (dst[i] > tmp);
    }
}

/*
 * Negate long integer.
 */

void
neg_long_integer(uint32 *dst, int nval)
{
    int         i;
    uint32      tmp, new_br, br = 0;

    for (i = 0; i < nval; i++, br = new_br) {
        dst[i] = -dst[i];
        new_br = (dst[i] > 0);
        tmp = dst[i];           /* Sub carry, detect overflow */
        dst[i] -= br;
        new_br += (dst[i] > tmp);
    }
}

/*
 * Parse uint64
 * Endian handling is taken into account.
 */

uint64
parse_uint64(char *str)
{
    uint32 tmpval[2];
    uint64 rval;

    parse_long_integer(tmpval, 2, str);
    COMPILER_64_SET(rval, tmpval[1], tmpval[0]);

    return rval;
}

/*
 * Format uint64
 * Endian handling is taken into account.
 */

void
format_uint64(char *buf, uint64 n)
{
    uint32              val[2];

    val[0] = COMPILER_64_LO(n);
    val[1] = COMPILER_64_HI(n);

    format_long_integer(buf, val, 2);
}

/*
 * Format uint64 as unsigned decimal.
 *
 * Comma should be one of:
 *      ','     to use commas every third digit
 *      '.'     to use periods every third digit
 *      '\0'    no commas or periods
 *
 * The buffer must have room for up to 21 bytes (if no commas or
 * periods) or 27 bytes (commas or periods).
 */

void
format_uint64_decimal(char *buf, uint64 n, int comma)
{
    int                 pw;
    char                *obuf;

    static CONST struct pow10_s {
        uint32          hi, lo;
    } pow10[] = {
        { 0U,           1U              },
        { 0U,           10U             },
        { 0U,           100U            },
        { 0U,           1000U           },
        { 0U,           10000U          },
        { 0U,           100000U         },
        { 0U,           1000000U        },
        { 0U,           10000000U       },
        { 0U,           100000000U      },
        { 0U,           1000000000U     },
        { 2U,           1410065408U     },
        { 23U,          1215752192U     },
        { 232U,         3567587328U     },
        { 2328U,        1316134912U     },
        { 23283U,       276447232U      },
        { 232830U,      2764472320U     },
        { 2328306U,     1874919424U     },
        { 23283064U,    1569325056U     },
        { 232830643U,   2808348672U     },
        { 2328306436U,  2313682944U     },
    };

    obuf = buf;

    for (pw = 19; pw >= 0; pw--) {
        uint64          p10;
        int             digit;

        COMPILER_64_SET(p10, pow10[pw].hi, pow10[pw].lo);

        for (digit = '0'; COMPILER_64_GE(n, p10); digit++) {
            COMPILER_64_SUB_64(n, p10);
        }

        if (buf > obuf || digit != '0') {
            *buf++ = digit;
            if (comma != 0 && (pw % 3) == 0 && pw > 0) {
                *buf++ = comma;
            }
        }
    }

    if (buf == obuf) {
        *buf++ = '0';
    }

    *buf = 0;
}

/*
 * Parse a mac address that comes from a pure hex number.
 * The leading 0x is assumed to have been stripped off.
 * String can be up to 12 characters long.
 *
 * Process string from end.
 */

int
parse_macaddr_hex(char *str, sal_mac_addr_t macaddr)
{
    char *s;
    int i;

    sal_memset(macaddr, 0, 6);
    if ((sal_strlen(str) > 12) || (sal_strlen(str) == 0)) {
        return -1;
    }
    s = str + sal_strlen(str) - 1;
    for (i = 0; i < 6; i++) {
        if (s < str) {  /* done w/ string; fill w/ 0. */
            macaddr[5-i] = 0;
        } else {
            if (!isxdigit((unsigned) *s)) {  /* bad character */
                return -1;
            }
            macaddr[5-i] = xdigit2i((unsigned) *(s--));
            if (isxdigit((unsigned) *s)) {
                macaddr[5-i] += 16 * xdigit2i((unsigned) *(s--));
            }
        }
    }

    return 0;
}

/*
 * parse_macaddr will take a string of the form H:H:H:H:H:H where each
 * H is one or two hex digits, or a string of the form 0xN where N may
 * consist of up to 12 hex digits.  The result is returned in a byte
 * array to avoid endian confusion.
 */

int
parse_macaddr(char *str, sal_mac_addr_t macaddr)
{
    char *s;
    int i;

    if (sal_strchr(str, ':')) {             /* Colon format */
        s = str;
        for (i = 0; i < 6; i++) {
            if (!isxdigit((unsigned)*s)) {  /* bad character */
                return -1;
            }
            macaddr[i] = xdigit2i((unsigned)*(s++));
            if (isxdigit((unsigned)*s)) {
                macaddr[i] *= 16;
                macaddr[i] += xdigit2i((unsigned)*(s++));
            }
            if ((i < 5) && (*(s++) != ':')) {  /* bad character */
                return -1;
            }
        }
        if (*s) {
            return -1;
        }
    } else if (str[0] == '0' && sal_tolower((int)str[1]) == 'x') {
        return parse_macaddr_hex(&(str[2]), macaddr);
    } else {  /* Handle 0x... format */
        return -1;
    }

    return 0;
}

/*
 * format_macaddr requires a buffer of 18 bytes minimum.
 * It does not use sal_sprintf so it can be called from an interrupt context.
 */

void
format_macaddr(char buf[SAL_MACADDR_STR_LEN], sal_mac_addr_t macaddr)
{
    int i;

    for (i = 0; i <= 5; i++) {
        *buf++ = i2xdigit(macaddr[i] >> 4);
        *buf++ = i2xdigit(macaddr[i]);
        *buf++ = ':';
    }

    *--buf = 0;
}

/*
 * increment_macaddr
 */

void
increment_macaddr(sal_mac_addr_t macaddr, int amount)
{
    int i, v;
    for (i = 5; i >= 0; i--) {
        v = (int) macaddr[i] + amount;
        macaddr[i] = v;
        if (v >= 0 && v <= 255)
            break;
        amount = v >> 8;        /* carry or borrow (signed shift) */
    }
}

int
parse_ipaddr(char *s, ip_addr_t *ipaddr)
{
    char *ts;
    int i, x;
    ip_addr_t ip = 0;

    if (sal_strchr(s, '.')) {               /* dotted notation */
        for (i = 0; i < 4; i++) {
            x = sal_ctoi(s, &ts);
            if ((x > 0xff) || (x < 0)) {
                return(-1);
            }
            ip = (ip << 8) | x;
            if (*ts != '.') {   /* End of string */
                break;
            }
            s = ts + 1;
        }
        if (((i != 3) || (*ts != '\0'))) {
            return(-1);
        } else {
            *ipaddr = ip;
            return(0);
        }
    } else if (isint(s)){
        *ipaddr = parse_integer(s);
        return(0);
    } else {
        return(-1);
    }
}

static INLINE int
colon_count(char *s)
{
    int cc = 0;
    char *ts = s;

    while ((ts = sal_strchr(ts, ':')) != NULL) {
        ts++;
        cc++;
    }

    return cc;
}

int
parse_ip6addr(char *s, ip6_addr_t ipaddr)
{
    char *ts;
    int i, x;
    int has_dcolon = 0; /* double colon */
    char *dstr = s;
    char str_buf[50];
    char *tmps = &str_buf[0];
    uint8      *ip = (uint8 *)ipaddr;

    while ((dstr = sal_strstr(dstr, "::")) != NULL) {
        dstr += 2;
        has_dcolon++;
    }
    if (has_dcolon > 1) {
        return(-1);
    } else if (has_dcolon == 1) {
        dstr -= 2;
    }

    sal_memset(ipaddr, 0, sizeof (ip6_addr_t));
    if (sal_strchr(s, ':')) {               /* colon notation */
        sal_strncpy(&tmps[2], s, 47);
        tmps[49] = '\0';
        for (i = 0; i < 8; i++) {
            tmps[0] = '0';
            tmps[1] = 'x';
            x = sal_ctoi(tmps, &ts);
            if ((x > 0xffff) || (x < 0)) {
                return(-1);
            }

            /* double colon, heading double colon not supported */
            if (has_dcolon && *ts == ':' && ts == &tmps[2]) {
                int end;
                int cc = colon_count(ts);
                if (cc > 8) {
                    return (-1);
                }
                end = 8 - cc;
                for (;i < end; i++) {
                    ip[2 * i] = 0;
                    ip[2 * i + 1] = 0;
                }
                i--;
                tmps = ts - 1;
                /* :: will be replaced by 0x */
                continue;
            }

            /* double colon, heading double colon not supported */
            if (has_dcolon && *ts == ':' && ts == &tmps[2]) {
                int end;
                int cc = colon_count(ts);
                if (cc > 8) {
                    return (-1);
                }
                end = 8 - cc;
                for (;i < end; i++) {
                    ip[2 * i] = 0;
                    ip[2 * i + 1] = 0;
                }
                i--;
                tmps = ts - 1;
                /* :: will be replaced by 0x */
                continue;
            }

            ip[2 * i] = (x >> 8) & 0xFF;
            ip[2 * i + 1] = x & 0xFF;
            if (*ts != ':') {   /* End of string */
                break;
            }
            tmps = ts - 1;
        }
        if (((i != 7) || (*ts != '\0'))) {
            return(-1);
        } else {
            return(0);
        }
    } else {
        return(-1);
    }
}

/*
 * To be useful the incremnent position should also be specified
 */
void
increment_ip6addr(ip6_addr_t ipaddr, uint32 byte_pos, int amount)
{
    int i, v;
    if (byte_pos >= sizeof(ip6_addr_t)) {
        return;
    }
    for (i = byte_pos; i >= 0; i--) {
        v = (int) ipaddr[i] + amount;
        ipaddr[i] = v;
        if (v >= 0 && v <= 255)
            break;
        amount = v >> 8;        /* carry or borrow (signed shift) */
    }
}

void
format_ipaddr(char buf[SAL_IPADDR_STR_LEN], ip_addr_t ipaddr)
{
    sal_sprintf(buf, "%d.%d.%d.%d",
            (ipaddr >> 24) & 0xff, (ipaddr >> 16) & 0xff,
            (ipaddr >> 8) & 0xff, ipaddr & 0xff);
}

void
format_ipaddr_mask(char *buf, ip_addr_t ipaddr, ip_addr_t mask)
{
    int masklen, i;

    /* Convert netmask to number of bits */

    masklen = 0;
    for (i = 0; i < 32; i++) {
        if (mask & 0x80000000) {
            masklen++;
            mask = mask << 1;
        } else {
            break;
        }
    }
    sal_sprintf(buf, "%d.%d.%d.%d/%d",
            (ipaddr >> 24) & 0xff, (ipaddr >> 16) & 0xff,
            (ipaddr >> 8) & 0xff, ipaddr & 0xff, masklen);
}

void
format_ip6addr(char buf[IP6ADDR_STR_LEN], ip6_addr_t ipaddr)
{
    sal_sprintf(buf, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
            (((uint16)ipaddr[0] << 8) | ipaddr[1]),
            (((uint16)ipaddr[2] << 8) | ipaddr[3]),
            (((uint16)ipaddr[4] << 8) | ipaddr[5]),
            (((uint16)ipaddr[6] << 8) | ipaddr[7]),
            (((uint16)ipaddr[8] << 8) | ipaddr[9]),
            (((uint16)ipaddr[10] << 8) | ipaddr[11]),
            (((uint16)ipaddr[12] << 8) | ipaddr[13]),
            (((uint16)ipaddr[14] << 8) | ipaddr[15]));
}

/*
 * Parse a string containing a list of ports into a bitmap string.
 * The input format is:
 *
 *      PBMP    : '0x' {HEXDIGIT}+      { return BITS_SET($2); }
 *              | LIST                  { return $1; }
 *              ;
 *      LIST    : [LIST ','] RANGE STEP         { return UNION($1, $2, $3); }
 *              | [LIST ','] '~' RANGE STEP     { return REMOVE($1, $2, $3); }
 *              ;
 *      RANGE   : PORT '-' PORT         { return $1 .. $2; }
 *              | PORT                  { return $1 .. $1; }
 *              | 'FE'                  { return FE ports; }
 *              | 'GE'                  { return GE ports; }
 *              | 'XE'                  { return XE ports; }
 *              | 'HG'                  { return HG ports; }
 *              | 'E'                   { return ether ports; }
 *              | '*'                   { return ether, HG ports; }
 *              | 'ALL'                 { return ether, HG, CPU ports; }
 *              ;
 *      STEP    : ':' INTEGER           { return $2 };
 *              ;
 *      PORT    : INTEGER{NUM_PORT}             { return $1 }
 *              | 'E' INTEGER{NUM_E_PORT}       { return E port $2 }
 *              | 'FE' INTEGER{NUM_FE_PORT}     { return FE port $2 }
 *              | 'GE' INTEGER{NUM_GE_PORT}     { return GE port $2 }
 *              | 'XE' INTEGER{NUM_GE_PORT}     { return XE port $2 }
 *              | 'HG' INTEGER{NUM_HG_PORT}     { return HG port $2 }
 *              | 'CPU'                         { return CMIC_PORT }
 *              | 'NONE'                        { return NIL }
 *
 * Returns 0 on success, -1 on syntax error.
 */

int
parse_pbmp(int unit, char *s, soc_pbmp_t *pbmp)
{
    soc_pbmp_t  bmall;
    int         complement, plast, pfirst, pstep, port, p, tp, i;
    int         matched, blk, blktype, is_ci = 0, is_phy = 0;
    char        *sn, *se;
    soc_block_name_t *blk_names;
    soc_pbmp_t  pbmp_temp;

#if defined(BCM_LTSW_SUPPORT)
    if (SOC_IS_LTSW(unit)) {
        return ltsw_parse_pbmp(unit, s, pbmp);
    }
#endif

    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        return _shr_pbmp_decode(s, pbmp);
    }

    SOC_PBMP_CLEAR(bmall);
    SOC_PBMP_CLEAR(pbmp_temp);
    SOC_PBMP_CLEAR(*pbmp);

    blk_names = soc_block_names;
    pfirst = -1;
    pstep = 1;
    complement = 0;

    while (*s) {
        if (s[0] == '~' || s[0] == '!') {
            complement = 1;
            s += 1;
        }

        /* To cover for Apache XLPORTB0 and CLG2PORT blocks  */
        if ((sal_strncasecmp("XLB0PORT", s, (sizeof("XLB0PORT") - 1)) == 0) ||
            (sal_strncasecmp("CLG2PORT", s, (sizeof("CLG2PORT") - 1)) == 0)) {
            sn = s + 8;
        } else {


            for (sn = s; *sn && *sn != '-' && *sn != ',' && *sn != ':' &&
                     !isdigit((unsigned)*sn); sn++) {
                ;
            }
        }


        se = sn;
        if (isdigit((unsigned)*sn)) {
            port = 0;
            do {
                port = port * 10 + (*se++ - '0');
            } while (isdigit((unsigned) *se));
        } else {
            port = -1;
        }
        plast = -1;
        BCM_PBMP_ASSIGN(pbmp_temp, PBMP_PORT_ALL(unit));
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
        if (SOC_IS_ENDURO(unit) || SOC_IS_TD_TT(unit)) {
            /* Since the LB port in Enduro is port 1, not the last port.
                 We need to add this to support <reg_name>.number
                 register access method with correct port mapping in AV tests. */
            BCM_PBMP_OR(pbmp_temp, PBMP_LB(unit));
        }
#endif /* BCM_ENDURO_SUPPORT || BCM_TRIDENT_SUPPORT */

        if (soc_feature(unit, soc_feature_ep_redirect_v2)) {
            BCM_PBMP_OR(pbmp_temp, PBMP_RDB_ALL(unit));
        }
        if (SOC_IS_MONTEREY(unit) || SOC_IS_HURRICANE4(unit)) {
            BCM_PBMP_OR(pbmp_temp, PBMP_MACSEC_ALL(unit));
        }
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            /* Add AXP ports.
             * We need to add this to support <reg_name>.number
             * register access method with correct port mapping in AV tests.
             */
            BCM_PBMP_OR(pbmp_temp, PBMP_AXP_ALL(unit));
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
        if (sn == s) {                          /* unprefixed number */
            if (soc_property_get(unit, spn_DPORT_MAP_DIRECT, FALSE)) {
                /* Treat port as internal port number */
                plast = DPORT_FROM_PORT(unit, port);
            } else {
                plast = -1;

                /* Iterate through the dport map and find the corresponding
                 * logical port. If the logical port is valid and is the last
                 * one passed in as the arg, mark it as the last port
                 */
                for (p = 0; p < SOC_DPORT_MAX; p++) {
                    tp = soc_dport_to_port(unit, p);

                    /* If the logical port is valid and mapped, and if it's
                     * the last port from the string passed above, mark it
                     * as the last port and break out of the loop
                     */
                    if (tp >= 0 && SOC_PBMP_MEMBER(pbmp_temp, tp)) {
                        if (p == port) {
                            plast = p;
                            break;
                        }
                    }
                }
            }
            if (plast < 0) {
                return -1;                      /* error: port out of range */
            }
        } else {                                /* prefix number */
            tp = *sn;
            *sn = '\0';
            if (sal_strcasecmp(s, "fe") == 0) {
                bmall = PBMP_FE_ALL(unit);
            } else if (sal_strcasecmp(s, "ge") == 0) {
                bmall = PBMP_GE_ALL(unit);
            } else if (sal_strcasecmp(s, "xe") == 0) {
                bmall = PBMP_XE_ALL(unit);
            } else if (sal_strcasecmp(s, "le") == 0) {
                bmall = PBMP_LE_ALL(unit);
            } else if (sal_strcasecmp(s, "ce") == 0) {
                bmall = PBMP_CE_ALL(unit);
            } else if (sal_strcasecmp(s, "cd") == 0) {
                bmall = PBMP_CDE_ALL(unit);
            } else if (sal_strcasecmp(s, "cc") == 0) {
                bmall = PBMP_CCE_ALL(unit);
            } else if (sal_strcasecmp(s, "hg") == 0) {
                bmall = PBMP_HG_ALL(unit);
            } else if (sal_strcasecmp(s, "hl") == 0) {
                bmall = PBMP_HL_ALL(unit);
            } else if (sal_strcasecmp(s, "hg_s") == 0) {
                bmall = PBMP_HG_SUBPORT_ALL(unit);
            } else if (sal_strcasecmp(s, "st") == 0) {
                bmall = PBMP_ST_ALL(unit);
            } else if (sal_strcasecmp(s, "il") == 0) {
                bmall = PBMP_IL_ALL(unit);
            } else if (SOC_IS_SAND(unit) && (sal_strcasecmp(s, "xl") == 0)) {
                bmall = PBMP_XL_ALL(unit);
            } else if (sal_strcasecmp(s, "axp") == 0) {
                bmall = PBMP_AXP_ALL(unit);
            } else if (sal_strcasecmp(s, "sch") == 0) {
                bmall = PBMP_SCH_ALL(unit);
            } else if (sal_strcasecmp(s, "sci") == 0) {
                bmall = PBMP_SCI_ALL(unit);
            } else if (sal_strcasecmp(s, "qsgmii") == 0) {
                bmall = PBMP_QSGMII_ALL(unit);
            } else if ((sal_strcasecmp(s, "sfi") == 0 ||
                        sal_strcasecmp(s, "if") == 0)) {
                bmall = PBMP_SFI_ALL(unit);
            } else if (sal_strcasecmp(s, "fabric") == 0) {
                bmall = PBMP_SFI_ALL(unit);
                port += SOC_INFO(unit).fabric_logical_port_base; /* fabric links starting from
                                                                configurable logical_port_base */
            } else if (sal_strcasecmp(s, "sci_sfi") == 0) {
                bmall = PBMP_SCI_ALL(unit);
		BCM_PBMP_AND(bmall, PBMP_SFI_ALL(unit));
            } else if (sal_strcasecmp(s, "spi") == 0) {
                bmall = PBMP_SPI_ALL(unit);
            } else if (sal_strcasecmp(s, "spi_s") == 0) {
                bmall = PBMP_SPI_SUBPORT_ALL(unit);
            } else if (sal_strcasecmp(s, "e") == 0) {
#ifdef BCM_DNX_SUPPORT
                if (SOC_IS_DNX(unit)) {
                    cli_out("e is not supported in DNX devices. Use 'eth' instead.\n");
                    return -1;
                }
#endif
                bmall = PBMP_E_ALL(unit);
#ifdef BCM_DNX_SUPPORT
            } else if ((sal_strcasecmp(s, "eth") == 0) && SOC_IS_DNX(unit)) {
                bmall = PBMP_PORT_ALL(unit);
                SOC_PBMP_REMOVE(bmall, PBMP_SFI_ALL(unit));
                SOC_PBMP_REMOVE(bmall, PBMP_IL_ALL(unit));
                SOC_PBMP_REMOVE(bmall, PBMP_FLEXE_PHY_ALL(unit));
#endif
            } else if (sal_strcasecmp(s, "cpu") == 0) {
                bmall = PBMP_CMIC(unit);
            } else if (sal_strcasecmp(s, "lb") == 0) {
                bmall = PBMP_LB(unit);
            } else if (sal_strcasecmp(s, "rdb") == 0) {
                bmall = PBMP_RDB_ALL(unit);
            } else if (sal_strcasecmp(s, "fae") == 0) {
                bmall = PBMP_FAE_ALL(unit);
            } else if (sal_strcasecmp(s, "msec") == 0) {
                bmall = PBMP_MACSEC_ALL(unit);
            } else if (sal_strcasecmp(s, "roe") == 0) {
                bmall = PBMP_ROE_ALL(unit);
            } else if (sal_strcasecmp(s, "mmu") == 0) {
                bmall = PBMP_MMU(unit);
            } else if (sal_strcasecmp(s, "mgmt") == 0) {
                bmall = PBMP_MANAGEMENT(unit);
            } else if (sal_strcasecmp(s, "all") == 0) {
                bmall = PBMP_ALL(unit);
                SOC_PBMP_REMOVE(bmall, PBMP_MACSEC_ALL(unit));
                SOC_PBMP_REMOVE(bmall, PBMP_LB(unit));
                SOC_PBMP_REMOVE(bmall, PBMP_RDB_ALL(unit));
                SOC_PBMP_REMOVE(bmall, PBMP_FAE_ALL(unit));
                BCM_PBMP_OR(bmall, PBMP_SPI_ALL(unit));
            } else if (sal_strcasecmp(s, "*") == 0) {
                bmall = PBMP_PORT_ALL(unit);
                BCM_PBMP_OR(bmall, PBMP_SPI_ALL(unit));
            } else if (sal_strcasecmp(s, "none") == 0) {
                SOC_PBMP_CLEAR(bmall);
            } else if (sal_strcasecmp(s, "phy") == 0) {
                SOC_PBMP_ASSIGN(bmall,SOC_INFO(unit).physical_pbm);
                is_phy = 1;
            } else if (sal_strcasecmp(s, "ci") == 0) {
                SOC_PBMP_CLEAR(bmall);
                for (i = 0; blk_names[i].blk != SOC_BLK_NONE; i++) {
                    if (sal_strcasecmp(blk_names[i].name, "ci") == 0) {
                        blktype = blk_names[i].blk;
                        SOC_BLOCK_ITER(unit, blk, blktype) {
                            SOC_PBMP_OR(bmall, SOC_BLOCK_BITMAP(unit, blk));
                            plast = port;
                            is_ci = 1;
                        }
                    }
                }
            } else {
                /* parse block names */
                matched = 0;
                SOC_PBMP_CLEAR(bmall);
                for (i = 0; blk_names[i].blk != SOC_BLK_NONE; i++) {
                    if (sal_strcasecmp(blk_names[i].name, s) == 0) {
                        blktype = blk_names[i].blk;
                        matched += 1;
                        SOC_BLOCK_ITER(unit, blk, blktype) {
                            if (port >= 0 &&
                                SOC_BLOCK_INFO(unit, blk).number != port) {
                                continue;
                            }
                            SOC_PBMP_OR(bmall, SOC_BLOCK_BITMAP(unit, blk));
                        }
                    }
                }
                if (matched == 0) {             /* was not a block name */
                    *sn = tp;                   /* restore parsed string */
                    return -1;                  /* error: unexpected char */
                }
                port = -1;                      /* don't use the number */
            }
            *sn = tp;
            if (port >= 0) {
                i = -1;
                if(is_phy || is_ci) {
                  SOC_PBMP_ITER(bmall,p){
                    if (p == port) {
            			    plast = p;
            			    break;
                    }
                  }
                } else {
                  DPORT_SOC_PBMP_ITER(unit, bmall, p, tp) {
                    if (SOC_IS_SAND(unit)) {
                      if (p == port) {
                          plast = p;
                          break;
                      }
                    } else {
                      i += 1;
                      if (DPORT_FROM_DPORT_INDEX(unit, p, i) == port) {
                          plast = p;
                          break;
                      }
                    }
                  }
                }
                if (plast < 0) {
                    return -1;                  /* error: index out of range */
                }
            }
        }
        s = se;

        switch (*s) {
        case '-':
            pstep = 1;
            if (plast < 0) {
                return -1;                      /* error: range without port */
            } else if (pfirst < 0) {
                pfirst = plast;
            } else {
                return -1;                      /* error: x-y-z */
            }
            break;
        case ':':
            s++;
            if (!isdigit((unsigned)*s)) {
                return -1;
            }
            for (pstep = 0; isdigit((unsigned)*s); s++) {
                pstep = pstep * 10 + (*s - '0');
            }
            if (pstep == 0 || (*s != ',' && *s != '\0')) {
                return -1;
            }
            /* fall through */
        case ',':
        case '\0':
            if (plast < 0) {                    /* complete bitmap op */
                pfirst = 999;
                plast = -1;
                DPORT_SOC_PBMP_ITER(unit, bmall, p, tp) {
                    if (p < pfirst) {
                        pfirst = p;
                    }
                    if (p > plast) {
                        plast = p;
                    }
                }
            }
            if (pfirst < 0) {           /* just one port */
                pfirst = plast;
            }
            for (port = pfirst; port <= plast; port += pstep) {  /* a range */
                p = is_ci ? port : DPORT_TO_PORT(unit, port);
                p = is_phy ? port : p;
                /* coverity[overrun-local] */
                if ((p >= 0) && SOC_PBMP_NOT_NULL(bmall) && !SOC_PBMP_MEMBER(bmall, p)) {
                    continue;   /* skip gaps in range */
                }
                if ((is_ci) || (SOC_PORT_VALID(unit, p)) || is_phy) {
                    if (complement) {
                        SOC_PBMP_PORT_REMOVE(*pbmp, p + is_phy*(SOC_INFO(unit).physical_port_offset-1));
                    } else {
                        SOC_PBMP_PORT_ADD(*pbmp, p + is_phy*(SOC_INFO(unit).physical_port_offset-1));
                    }
                }
            }
            if (*s == '\0') {
                return 0;
            }
            pfirst = -1;
            pstep = 1;
            complement = 0;
            SOC_PBMP_CLEAR(bmall);
            is_ci = 0;
            break;
        default:
            return -1;                          /* error: unexpected char */
        }
        s += 1;
    }
    return -1;                                  /* error: unexpected end */
}

int
parse_pp_pbmp(int unit, char *s, soc_pbmp_t *pbmp)
    {
        soc_pbmp_t  bmall;
        int         complement, plast, pfirst, pstep, port, p, i;
        char        *sn, *se;
        soc_pbmp_t  pbmp_temp;

        if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
            return _shr_pbmp_decode(s, pbmp);
        }

        SOC_PBMP_CLEAR(bmall);
        SOC_PBMP_CLEAR(pbmp_temp);
        SOC_PBMP_CLEAR(*pbmp);

        pfirst = -1;
        pstep = 1;
        complement = 0;

        while (*s) {
            if (s[0] == '~' || s[0] == '!') {
                complement = 1;
                s += 1;
            }

            for (sn = s; *sn && *sn != '-' && *sn != ',' && *sn != ':' &&
                     !isdigit((unsigned)*sn); sn++) {
                ;
            }
            se = sn;
            if (isdigit((unsigned)*sn)) {
                port = 0;
                do {
                    port = port * 10 + (*se++ - '0');
                } while (isdigit((unsigned) *se));
            } else {
                port = -1;
            }

            plast = -1;
            BCM_PBMP_ASSIGN(pbmp_temp, PBMP_PP_ALL(unit));
            if (sn == s) {                          /* unprefixed number */
                i = 0;
                plast = -1;
                PBMP_ITER(pbmp_temp, p) {
                    if (i == port) {
                        plast = p;
                        break;
                    }
                    i += 1;
                }
                if (plast < 0) {
                    return -1;                      /* error: port out of range */
                }
            } else {                                /* prefix number */
                p = *sn;
                *sn = '\0';
                if (sal_strcasecmp(s, "pp") == 0) {
                    bmall = PBMP_PP_ALL(unit);
                } else {
                    /* Not Handled now */
                }
                SOC_PBMP_ITER(bmall,p){
                    if (p == port) {
                        plast = p;
                        break;
                    }
                }
                if (plast < 0) {
                    return -1;                  /* error: index out of range */
                }
            }
            s = se;

            switch (*s) {
            case '-':
                pstep = 1;
                if (pfirst < 0) {
                    pfirst = plast;
                } else {
                    return -1;                      /* error: x-y-z */
                }
                break;
            case ':':
                s++;
                if (!isdigit((unsigned)*s)) {
                    return -1;
                }
                for (pstep = 0; isdigit((unsigned)*s); s++) {
                    pstep = pstep * 10 + (*s - '0');
                }
                if (pstep == 0 || (*s != ',' && *s != '\0')) {
                    return -1;
                }
                /* fall through */
            case ',':
            case '\0':
                if (pfirst < 0) {           /* just one port */
                    pfirst = plast;
                }
                for (port = pfirst; port <= plast; port += pstep) {  /* a range */
                    if (SOC_PBMP_NOT_NULL(bmall) && !SOC_PBMP_MEMBER(bmall, port)) {
                        continue;   /* skip gaps in range */
                    }
                    if (1) { 
                        if (complement) {
                            SOC_PBMP_PORT_REMOVE(*pbmp, port);
                        } else {
                            SOC_PBMP_PORT_ADD(*pbmp, port);
                        }
                    }
                }
                if (*s == '\0') {
                    return 0;
                }
                pfirst = -1;
                pstep = 1;
                complement = 0;
                SOC_PBMP_CLEAR(bmall);
                break;
            default:
                return -1;                          /* error: unexpected char */
            }
            s += 1;
        }
        return -1;                                  /* error: unexpected end */
    }


int
parse_bcm_pbmp(int unit, char *s, bcm_pbmp_t *pbmp)
{
    int rv = parse_pbmp(unit, s, pbmp);

    BCM_API_XLATE_PORT_PBMP_P2A(unit, pbmp);

    return rv;
}

int
parse_pbmp_no_error(int unit, char *s, pbmp_t *pbmp)
{
    pbmp_t      bmall;
    int         complement, plast, pfirst, pstep, port, p, tp, i;
    int         matched, blk, blktype;
    char        *sn, *se;
    uint16      dev_id;
    uint8       rev_id;
    soc_block_name_t *blk_names;
    pbmp_t      pbmp_temp;

    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        return _shr_pbmp_decode(s, pbmp);
    }

    SOC_PBMP_CLEAR(bmall);
    SOC_PBMP_CLEAR(pbmp_temp);
    SOC_PBMP_CLEAR(*pbmp);

    blk_names = soc_block_names;
    soc_cm_get_id(unit, &dev_id, &rev_id);

    pfirst = -1;
    pstep = 1;
    complement = 0;

    while (*s) {
        if (s[0] == '~' || s[0] == '!') {
            complement = 1;
            s += 1;
        }

        for (sn = s; *sn && *sn != '-' && *sn != ',' && *sn != ':' &&
                 !isdigit((unsigned)*sn); sn++) {
            ;
        }
        se = sn;
        if (isdigit((unsigned)*sn)) {
            port = 0;
            do {
                port = port * 10 + (*se++ - '0');
            } while (isdigit((unsigned) *se));
        } else {
            port = -1;
        }

        plast = -1;
        BCM_PBMP_ASSIGN(pbmp_temp, PBMP_PORT_ALL(unit));
#if defined(BCM_ENDURO_SUPPORT)
        if(SOC_IS_ENDURO(unit)) {
            /* Since the LB port in Enduro is port 1, not the last port.
                 We need to add this to support <reg_name>.number
                 register access method with correct port mapping in AV tests. */
            BCM_PBMP_OR(pbmp_temp, PBMP_LB(unit));
        }
#endif /* BCM_ENDURO_SUPPORT */
        if (sn == s) {                          /* unprefixed number */
            if (soc_property_get(unit, spn_DPORT_MAP_DIRECT, FALSE)) {
                /* Treat port as internal port number */
                plast = DPORT_FROM_PORT(unit, port);
            } else {
                /* Port number is port index + 1 */
                i = 0;
                plast = -1;
                /* coverity[overrun-local] */
                DPORT_SOC_PBMP_ITER(unit, pbmp_temp, p, tp) {
                    i += 1;
                    if (i == port) {
                        plast = p;
                        break;
                    }
                }
            }
            if (plast < 0) {
                return -1;                      /* error: port out of range */
            }
        } else {                                /* prefix number */
            tp = *sn;
            *sn = '\0';
            if (sal_strcasecmp(s, "fe") == 0) {
                switch (dev_id) {
                case 0xb018:
                    SOC_PBMP_WORD_SET(bmall, 0, 0xffffffc0);
                    SOC_PBMP_WORD_SET(bmall, 1, 0x3fffff);
                    break;
                case 0xb014:
                case 0xb024:
                case 0xb025:
                    SOC_PBMP_WORD_SET(bmall, 0, 0x3fffffc0);
                    break;
                default:
                    break;
                }
            } else if (sal_strcasecmp(s, "ge") == 0) {
                switch (dev_id) {
                case 0xb218:
                    SOC_PBMP_WORD_SET(bmall, 0, 0xfffffffe);
                    SOC_PBMP_WORD_SET(bmall, 1, 0x3fffff);
                    break;
                case 0xb214:
                case 0xb215:
                case 0xb224:
                case 0xb225:
                    SOC_PBMP_WORD_SET(bmall, 0, 0x3ffffffe);
                    break;
                case 0xb226:
                case 0xb227:
                    SOC_PBMP_WORD_SET(bmall, 0, 0x3ffffe);
                    break;
                case 0xb212:
                case 0xb213:
                case 0xb228:
                case 0xb229:
                    SOC_PBMP_WORD_SET(bmall, 0, 0x3ffe);
                    break;
                case 0xc71a:
                    SOC_PBMP_WORD_SET(bmall, 0, 0xfffffff8);
                    SOC_PBMP_WORD_SET(bmall, 1, 0x3fffff);
                    break;
                case 0xc71b:
                case 0xc724:
                    SOC_PBMP_WORD_SET(bmall, 0, 0x3ffffff8);
                    break;
                case 0xc716:
                    SOC_PBMP_WORD_SET(bmall, 0, 0x3fffc8);
                    break;
                case 0xc726:
                    SOC_PBMP_WORD_SET(bmall, 0, 0x3ffff8);
                    break;
                case 0xb018:
                case 0xb014:
                case 0xb024:
                case 0xb025:
                    SOC_PBMP_WORD_SET(bmall, 0, 0x3e);
                    break;
                default:
                    break;
                }
            } else if (sal_strcasecmp(s, "st") == 0) {
                switch (dev_id) {
                case 0xb218:
                case 0xb214:
                case 0xb215:
                case 0xb212:
                case 0xb213:
                case 0xb018:
                case 0xb014:
                    SOC_PBMP_WORD_SET(bmall, 0, 0x6);
                    break;
                case 0xb024:
                case 0xb025:
                case 0xb224:
                case 0xb225:
                case 0xb226:
                case 0xb227:
                case 0xb228:
                case 0xb229:
                    SOC_PBMP_WORD_SET(bmall, 0, 0x36);
                default:
                    break;
                }
            } else if (sal_strcasecmp(s, "e") == 0) {
                bmall = PBMP_E_ALL(unit);
            } else if (sal_strcasecmp(s, "cpu") == 0) {
                bmall = PBMP_CMIC(unit);
            } else if (sal_strcasecmp(s, "all") == 0) {
                bmall = PBMP_ALL(unit);
                BCM_PBMP_OR(bmall, PBMP_SPI_ALL(unit));
            } else if (sal_strcasecmp(s, "*") == 0) {
                bmall = PBMP_PORT_ALL(unit);
                BCM_PBMP_OR(bmall, PBMP_SPI_ALL(unit));
            } else if (sal_strcasecmp(s, "none") == 0) {
                SOC_PBMP_CLEAR(bmall);
            } else {
                /* parse block names */
                matched = 0;
                SOC_PBMP_CLEAR(bmall);
                for (i = 0; blk_names[i].blk != SOC_BLK_NONE; i++) {
                    if (sal_strcasecmp(blk_names[i].name, s) == 0) {
                        blktype = blk_names[i].blk;
                        matched += 1;
                        SOC_BLOCK_ITER(unit, blk, blktype) {
                            if (port >= 0 &&
                                SOC_BLOCK_INFO(unit, blk).number != port) {
                                continue;
                            }
                            SOC_PBMP_OR(bmall, SOC_BLOCK_BITMAP(unit, blk));
                        }
                    }
                }
                if (matched == 0) {             /* was not a block name */
                    *sn = tp;                   /* restore parsed string */
                    return -1;                  /* error: unexpected char */
                }
                port = -1;                      /* don't use the number */
            }
            *sn = tp;
            if (port >= 0) {
                p = -1;
                SOC_PBMP_ITER(bmall, tp) {      /* map port from bitmap */
                    p += 1;
                    if (p == port) {
                        plast = tp;
                    }
                }
                if (plast < 0) {
/*                    return -1;                   error: index out of range */
                }

            }
        }
        s = se;

        switch (*s) {
        case '-':
            pstep = 1;
            if (plast < 0) {
/*                return -1;                       error: range without port */
            } else if (pfirst < 0) {
                pfirst = plast;
            } else {
                return -1;                      /* error: x-y-z */
            }
            break;
        case ':':
            s++;
            if (!isdigit((unsigned)*s)) {
                return -1;
            }
            for (pstep = 0; isdigit((unsigned)*s); s++) {
                pstep = pstep * 10 + (*s - '0');
            }
            if (pstep == 0 || (*s != ',' && *s != '\0')) {
                return -1;
            }
            /* fall through */
        case ',':
        case '\0':
            if (plast < 0) {                    /* complete bitmap op */
                pfirst = 999;
                plast = -1;
                PBMP_ITER(bmall, p) {
                    if (p < pfirst) {
                        pfirst = p;
                    }
                    if (p > plast) {
                        plast = p;
                    }
                }
            }
            if (pfirst < 0) {           /* just one port */
                pfirst = plast;
            }
            for (p = pfirst; p <= plast; p += pstep) {  /* a range */
    /*    coverity[overrun-local : FALSE]    */
                if (SOC_PBMP_NOT_NULL(bmall) && !SOC_PBMP_MEMBER(bmall, p)) {
                    continue;   /* skip gaps in range */
                }
                if (SOC_PORT_VALID(unit, p)) {
                    if (complement) {
                        SOC_PBMP_PORT_REMOVE(*pbmp, p);
                    } else {
                        SOC_PBMP_PORT_ADD(*pbmp, p);
                    }
                }
            }
            if (*s == '\0') {
                return 0;
            }
            pfirst = -1;
            pstep = 1;
            complement = 0;
            SOC_PBMP_CLEAR(bmall);
            break;
        default:
            return -1;                          /* error: unexpected char */
        }
        s += 1;
    }
    return -1;                                  /* error: unexpected end */
}


STATIC int
_parse_port(int unit, char *s, soc_port_t *portp)
{
    int         port, max_port;
    int         i, p, tp;
    soc_pbmp_t  pbmp_temp;

#if defined(BCM_LTSW_SUPPORT)
    if (SOC_IS_LTSW(unit)) {
        return ltsw_parse_port(unit, s, portp);
    }
#endif

    /* Check if only port number was passed. */
    if (isint(s)) {
        port = parse_integer(s);
        *portp = port;
        if (soc_property_get(unit, spn_DPORT_MAP_DIRECT, FALSE) == FALSE) {
            /* Port number is port index + 1 */
            i = 1;
            *portp = -1;
            SOC_PBMP_ASSIGN(pbmp_temp, PBMP_PORT_ALL(unit));
            /* coverity[overrun-local] */
            DPORT_SOC_PBMP_ITER(unit, pbmp_temp, p, tp) {
                if (i == port) {
                    *portp = tp;
                    break;
                }
                i += 1;
            }
        }
        if (*portp < 0) {
            return -1;                          /* error: invalid port */
        }
        return 0;
    }

    {
        max_port = SOC_MAX_NUM_PORTS;
    }

    for (port = 0; port < max_port; port++) {
        if (!SOC_PORT_VALID(unit, port)) {
            continue;
        }
        if (sal_strcasecmp(s, "any") == 0) {
            *portp = port;
            return 0;
        }
        if (sal_strcasecmp(s, SOC_PORT_NAME(unit, port)) == 0) {
            *portp = port;
            return 0;
        }
    }

    return -1;                                  /* error: no matching name */
}

char *
format_gport(char *buf, bcm_gport_t gport)
{
    int gport_type;

    assert(buf != NULL);

#define GPORT_TYPE_SPRINTF(_name) \
        sal_sprintf(buf, "0x%x(%s)", gport, _name)

    if (BCM_GPORT_IS_SET(gport)) {
        gport_type = (gport >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK;
        switch(gport_type) {
        case _SHR_GPORT_TYPE_LOCAL:
            GPORT_TYPE_SPRINTF("local");
            break;
        case _SHR_GPORT_TYPE_LOCAL_CPU:
            GPORT_TYPE_SPRINTF("local_cpu");
            break;
        case _SHR_GPORT_TYPE_MODPORT:
            GPORT_TYPE_SPRINTF("modport");
            break;
        case _SHR_GPORT_TYPE_TRUNK:
            GPORT_TYPE_SPRINTF("trunk");
            break;
        case _SHR_GPORT_TYPE_DEVPORT:
            GPORT_TYPE_SPRINTF("devport");
            break;
        case _SHR_GPORT_TYPE_BLACK_HOLE:
            GPORT_TYPE_SPRINTF("black_hole");
            break;
        case _SHR_GPORT_TYPE_MPLS_PORT:
            GPORT_TYPE_SPRINTF("mpls");
            break;
        case _SHR_GPORT_TYPE_MIM_PORT:
            GPORT_TYPE_SPRINTF("mim");
            break;
        case _SHR_GPORT_TYPE_WLAN_PORT:
            GPORT_TYPE_SPRINTF("wlan");
            break;
        case _SHR_GPORT_TYPE_TRILL_PORT:
            GPORT_TYPE_SPRINTF("trill");
            break;
        case _SHR_GPORT_TYPE_SUBPORT_GROUP:
            GPORT_TYPE_SPRINTF("subport_group");
            break;
        case _SHR_GPORT_TYPE_SUBPORT_PORT:
            GPORT_TYPE_SPRINTF("subport_port");
            break;
        case _SHR_GPORT_TYPE_VLAN_PORT:
            GPORT_TYPE_SPRINTF("vlan");
            break;
        case _SHR_GPORT_TYPE_VXLAN_PORT:
            GPORT_TYPE_SPRINTF("vxlan");
            break;
        case _SHR_GPORT_TYPE_FLOW_PORT:
            GPORT_TYPE_SPRINTF("flow");
            break;
        case _SHR_GPORT_TYPE_NIV_PORT:
            GPORT_TYPE_SPRINTF("niv");
            break;
        case _SHR_GPORT_TYPE_L2GRE_PORT:
            GPORT_TYPE_SPRINTF("l2gre");
            break;
        case _SHR_GPORT_TYPE_EXTENDER_PORT:
            GPORT_TYPE_SPRINTF("extender");
            break;
        default:
            GPORT_TYPE_SPRINTF("unknown");
            break;
        }
    } else {
        sal_sprintf(buf, "0x%x", gport);
    }

#undef GPORT_TYPE_SPRINTF

    return buf;
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_LTSW_SUPPORT)
STATIC int stack_len = 0;

STATIC int parse_gport_push(void)
{
    stack_len++;
    return 0;
}

STATIC int parse_gport_pop(void)
{
    if (stack_len) {
        stack_len--;
        return 0;
    }

    return 1;
}

/*
 * Function    : _bcm_util_gport_construct
 * Description : Internal function to construct a gport from
 *                given parameters
 * Parameters  : (IN)  unit       - BCM device number
 *               (IN)  gport_dest - Structure that contains destination
 *                                   to encode into a gport
 *               (OUT) gport      - Global port identifier
 * Returns     : BCM_E_XXX
 * Notes       : The modid and port are translated from the
 *               local modid/port space to application space
 */
static int
_bcm_util_gport_construct(int unit, _bcm_util_gport_dest_t *gport_dest, bcm_gport_t *gport)
{

    bcm_gport_t     l_gport = 0;
#if defined(BCM_ESW_SUPPORT)
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
#endif

    if ((NULL == gport_dest) || (NULL == gport) ){
        return BCM_E_PARAM;
    }

    switch (gport_dest->gport_type) {
        case _SHR_GPORT_TYPE_TRUNK:
            SOC_GPORT_TRUNK_SET(l_gport, gport_dest->tgid);
            break;
        case _SHR_GPORT_TYPE_LOCAL_CPU:
            l_gport = BCM_GPORT_LOCAL_CPU;
            break;
        case _SHR_GPORT_TYPE_LOCAL:
            SOC_GPORT_LOCAL_SET(l_gport, gport_dest->port);
            break;
        case _SHR_GPORT_TYPE_SUBPORT_GROUP:
            SOC_GPORT_SUBPORT_GROUP_SET(l_gport, gport_dest->subport_id);
            break;
        case _SHR_GPORT_TYPE_MPLS_PORT:
            BCM_GPORT_MPLS_PORT_ID_SET(l_gport, gport_dest->mpls_id);
            break;
        case _SHR_GPORT_TYPE_MIM_PORT:
            BCM_GPORT_MIM_PORT_ID_SET(l_gport, gport_dest->mim_id);
            break;
        case _SHR_GPORT_TYPE_SUBPORT_PORT:
            BCM_GPORT_SUBPORT_PORT_SET(l_gport, gport_dest->subport_id);
            break;
        case _SHR_GPORT_TYPE_WLAN_PORT:
            BCM_GPORT_WLAN_PORT_ID_SET(l_gport, gport_dest->wlan_id);
            break;
        case _SHR_GPORT_TYPE_DEVPORT:
            BCM_GPORT_DEVPORT_SET(l_gport, unit, gport_dest->port);
            break;
        case _SHR_GPORT_TYPE_MODPORT:
#if defined(BCM_ESW_SUPPORT)
            if (SOC_IS_ESW(unit)) {
                BCM_IF_ERROR_RETURN (
                    _bcm_gport_modport_hw2api_map(unit, gport_dest->modid,
                                                  gport_dest->port, &mod_out,
                                                  &port_out));
                SOC_GPORT_MODPORT_SET(l_gport, mod_out, port_out);
            }
#endif
            break;
        case _SHR_GPORT_TYPE_EGRESS_MODPORT:
#if defined(BCM_ESW_SUPPORT)
            if (SOC_IS_ESW(unit)) {
                BCM_IF_ERROR_RETURN (
                    _bcm_gport_modport_hw2api_map(unit, gport_dest->modid,
                                                  gport_dest->port, &mod_out,
                                                  &port_out));
                BCM_GPORT_EGRESS_MODPORT_SET(l_gport, mod_out, port_out);
            }
#endif
            break;
        case _SHR_GPORT_TYPE_CHILD:
            BCM_GPORT_CHILD_SET(l_gport,
                                gport_dest->modid,
                                gport_dest->port);
            break;
        case _SHR_GPORT_TYPE_EGRESS_CHILD:
            BCM_GPORT_EGRESS_CHILD_SET(l_gport,
                                       gport_dest->modid,
                                       gport_dest->port);
            break;
        case _SHR_GPORT_TYPE_VLAN_PORT:
            SOC_GPORT_VLAN_PORT_ID_SET(l_gport, gport_dest->vlan_port_id);
            break;
        case _SHR_GPORT_TYPE_VXLAN_PORT:
            SOC_GPORT_VXLAN_PORT_ID_SET(l_gport, gport_dest->vxlan_id);
            break;
        case _SHR_GPORT_TYPE_TRILL_PORT:
            SOC_GPORT_TRILL_PORT_ID_SET(l_gport, gport_dest->trill_id);
            break;
        case _SHR_GPORT_TYPE_NIV_PORT:
            SOC_GPORT_NIV_PORT_ID_SET(l_gport, gport_dest->niv_id);
            break;
        case _SHR_GPORT_TYPE_L2GRE_PORT:
            SOC_GPORT_L2GRE_PORT_ID_SET(l_gport, gport_dest->l2gre_id);
            break;
        case _SHR_GPORT_TYPE_EXTENDER_PORT:
            SOC_GPORT_EXTENDER_PORT_ID_SET(l_gport, gport_dest->extender_id);
            break;
        default:
            return BCM_E_PARAM;
    }

    *gport = l_gport;
    return BCM_E_NONE;
}


/*
 * Function:
 *      parse_gport
 * Description:
 *     Parses string that identifies a GPORT and constructs an appropriate GPORT
 * Parameters:
 *      unit - Device unit number
 *      s - input string to parse
 *      portp - output port number
 */
int parse_gport(int unit, char *s, soc_port_t *portp)
{
    int port_val = -1, mod_val = -1;
    int i = 0, start_pos = 0, str_len, modid_len = 0;
    char *gport_type_str = NULL;
    char *gport_val_str = NULL;
    char *mod_str = NULL;
    char *port_str = NULL;
    char *parse_port_str;
    char *start = s;
    int rv = CMD_OK;
    _bcm_util_gport_dest_t   gport_dest;

    if (NULL == s) {
        return CMD_FAIL;
    }

    str_len = sal_strlen(s) + 1;

    if (!str_len) {
        return CMD_FAIL;
    }
    while (s[i] != '\0') {
        if ((s[i] == '(') || (s[i] == '{') || (s[i] == '[')){
            parse_gport_push();
            /* Allocate only once - defensive check against wrong input */
            if (!gport_type_str) {
            gport_type_str = sal_alloc(str_len, "GPORT type string");
            }
            sal_strncpy(gport_type_str, start, i - start_pos);
            gport_type_str[i] = '\0';
            start = (s + i + 1);
            start_pos = i + 1;
        } else if ((s[i] == ')') || (s[i] == '}') || (s[i] == ']')){
            if (parse_gport_pop()) {
                rv = CMD_FAIL;
                goto done;
            }
            /* Allocate only once - defensive check against wrong input */
            if (!gport_val_str) {
            gport_val_str = sal_alloc(str_len, "GPORT val string");
            }
            sal_strncpy(gport_val_str, start, i - start_pos);
            gport_val_str[i - start_pos] = '\0';
        } else if ((s[i] == ',') || (s[i] ==':') || (s[i] == '.') ) {
            /* Allocate only once - defensive check against wrong input */
            if (!mod_str) {
            mod_str = sal_alloc(str_len, "Modid str");
            }
            sal_strncpy(mod_str, start , i - start_pos);
            mod_str[i - start_pos] = '\0';
            modid_len = i - start_pos;

            /* Allocate only once - defensive check against wrong input */
            if (!port_str) {
            port_str = sal_alloc(str_len, "Port str");
            }
            start_pos = i + 1;
            if (start_pos >= (str_len - 2)) {
                rv = CMD_FAIL;
                goto done;
            }
            sal_strncpy(port_str, start + modid_len + 1, str_len - 2 - start_pos);
            port_str[str_len - 2 - start_pos] = '\0';
        }
        i++;
    }

    /* Check for redundant parentheses */
    if (!parse_gport_pop()) {
        rv = CMD_FAIL;
        goto done;
    }

    /* if not gport - don't parse */
    if (NULL == gport_type_str) {
        if (isint(s)) {
            port_val = parse_integer(s);
            if (BCM_GPORT_IS_SET(port_val)) {
                *portp = port_val;
                rv = CMD_OK;
                goto done;
            } else {
                rv = CMD_FAIL;
                goto done;
            }
        } else {
        rv = CMD_FAIL;
        goto done;
    }
    }

    /* Initialized gport_dest structure */
    sal_memset(&gport_dest, 0, sizeof(_bcm_util_gport_dest_t));

    /* Take care of GPORTS that do not need input */
    if (!sal_strcasecmp(gport_type_str, "localcpu") ||
        !sal_strcasecmp(gport_type_str, "lc")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_LOCAL;
        goto gport_done;
    }
    if (!sal_strcasecmp(gport_type_str, "invalid")) {
        *portp = BCM_GPORT_INVALID;
        goto done;
    }
    if (!sal_strcasecmp(gport_type_str, "blackhole") ||
        !sal_strcasecmp(gport_type_str, "bh")) {
        *portp = _SHR_GPORT_BLACK_HOLE;
        goto done;
    }

    /* Parse the input */
    if (NULL == port_str) {
        mod_val = -1;
        parse_port_str = gport_val_str;
    } else {
        parse_port_str = port_str;
    }

    if (mod_str && isint(mod_str)) {
    mod_val = parse_integer(mod_str);
    }

    if (isint(parse_port_str)) {
        port_val = parse_integer(parse_port_str);
    } else {
        if (_parse_port(unit, parse_port_str, &port_val) < 0) {
            rv = CMD_FAIL;
            goto done;
        }
    }

    /* Take care of GPORTS that need input */
    if (!sal_strcasecmp(gport_type_str, "modport") ||
        !sal_strcasecmp(gport_type_str, "mp") ||
        !sal_strcasecmp(gport_type_str, "egress_modport") ||
        !sal_strcasecmp(gport_type_str, "emp")) {
        if (mod_val < 0) {
            /* If modid was not specified - use mymodid */
            if (bcm_stk_my_modid_get(unit, &mod_val) < 0) {
                rv = CMD_FAIL;
                goto done;
            }
        }
        if (port_val < 0) {
            rv = CMD_FAIL;
            goto done;
        }
        if (('e' == gport_type_str[0]) ||
            ('E' == gport_type_str[0])) {
            gport_dest.gport_type = _SHR_GPORT_TYPE_EGRESS_MODPORT;
        } else {
            gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        }
        gport_dest.modid = mod_val;
        gport_dest.port = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "child") ||
        !sal_strcasecmp(gport_type_str, "egress_child")) {
        if (mod_val < 0) {
            /* If modid was not specified, use mymodid */
            if (bcm_stk_my_modid_get(unit, &mod_val) < 0) {
                rv = CMD_FAIL;
                goto done;
            }
        }
        if (port_val < 0) {
            rv = CMD_FAIL;
            goto done;
        }
        if (('e' == gport_type_str[0]) ||
            ('E' == gport_type_str[0])) {
            gport_dest.gport_type = _SHR_GPORT_TYPE_EGRESS_CHILD;
        } else {
            gport_dest.gport_type = _SHR_GPORT_TYPE_CHILD;
        }
        gport_dest.gport_type = _SHR_GPORT_TYPE_CHILD;
        gport_dest.modid = mod_val;
        gport_dest.port = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "trunk")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
        gport_dest.tgid = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "local")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_LOCAL;
        gport_dest.port = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "devport") ||
        !sal_strcasecmp(gport_type_str, "dp")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
        gport_dest.port = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "mpls")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_MPLS_PORT;
        gport_dest.mpls_id = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "mim")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_MIM_PORT;
        gport_dest.mim_id = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "wlan")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_WLAN_PORT;
        gport_dest.wlan_id = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "sub")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_PORT;
        gport_dest.subport_id = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "subg")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
        gport_dest.subport_group_id = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "vlan")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_VLAN_PORT;
        gport_dest.vlan_port_id = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "vxlan")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_VXLAN_PORT;
        gport_dest.vxlan_id = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "trill")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_TRILL_PORT;
        gport_dest.trill_id = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "niv")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_NIV_PORT;
        gport_dest.niv_id = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "l2gre")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_L2GRE_PORT;
        gport_dest.l2gre_id = port_val;
    }
    if (!sal_strcasecmp(gport_type_str, "extender")) {
        gport_dest.gport_type = _SHR_GPORT_TYPE_EXTENDER_PORT;
        gport_dest.extender_id = port_val;
    }

gport_done:
    rv = _bcm_util_gport_construct(unit, &gport_dest, portp);
done:
    if (mod_str) {
        sal_free(mod_str);
    }
    if (port_str) {
        sal_free(port_str);
    }
    if (gport_type_str) {
    sal_free(gport_type_str);
    }
    if (gport_val_str) {
        sal_free(gport_val_str);
    }

    return rv;
}
#endif

int
parse_port(int unit, char *s, soc_port_t *portp)
{
#if defined(BCM_ESW_SUPPORT) || defined(BCM_LTSW_SUPPORT)
    /* First try to parse a string as a GPORT identifier */
    if (!parse_gport(unit, s, portp)) {
        /* If Success we done */
        return 0;
    }
#endif
    /* If parse_gport failed, try to parse the old way */
    return _parse_port(unit, s, portp);
}

int
parse_bcm_port(int unit, char *s, bcm_port_t *portp)
{
    int rv = parse_port(unit, s, (soc_port_t *)portp);

    BCM_API_XLATE_PORT_P2A(unit, portp);

    return rv;
}

/* PQ_MOD_PORT parsing:  PQ_MOD_PORT |= <mod>.<port> | PQ_PORT */

int
parse_mod_port(int unit, char *str, bcm_mod_port_t *mp)
{
    int mod, port;
    char *cur_ptr = str;

    if (isdigit((unsigned)*cur_ptr)) {
        mod = 0;
        do {
            mod = mod * 10 + (*cur_ptr++ - '0');
        } while (isdigit((unsigned) *cur_ptr));
        if (*cur_ptr == '.') {  /* Parse port portion; else call parse_port */
            cur_ptr++;
            if (isdigit((unsigned)*cur_ptr)) {
                port = 0;
                do {
                    port = port * 10 + (*cur_ptr++ - '0');
                } while (isdigit((unsigned) *cur_ptr));
                if (*cur_ptr == '\0') {
                    mp->mod = mod;
                    mp->port = port;
                    return 0;  /* Looks good */
                }
            }
        }
    }

    /* If we get here, wasn't of form <mod>.<port> */
    mp->mod = -1;
    return parse_port(unit, str, &mp->port);
}

void
dump_l2_addr(int unit, char *pfx, bcm_l2_addr_t *l2addr)
{
    char                pfmt[SOC_PBMP_FMT_LEN];
    char                bmstr[FORMAT_PBMP_MAX];
    bcm_module_t        local_modid;
#ifdef BCM_ESW_SUPPORT
    _bcm_l2_gport_params_t  gport_params;
    char                gpfmt[FORMAT_GPORT_MAX];
#endif
#if defined(BCM_LTSW_SUPPORT)
    char                gport_buf[FORMAT_GPORT_MAX];
#endif
    int                 islocal = 1;

    if (bcm_stk_my_modid_get(unit, &local_modid) != BCM_E_NONE) {
        local_modid = -1;
    }

    cli_out("%smac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d GPORT=0x%x",
            pfx,
            l2addr->mac[0], l2addr->mac[1], l2addr->mac[2],
            l2addr->mac[3], l2addr->mac[4], l2addr->mac[5],
            l2addr->vid, l2addr->port);

#if defined(BCM_LTSW_SUPPORT)
    if (SOC_IS_LTSW(unit)) {
        if (BCM_GPORT_IS_TRUNK(l2addr->port)) {
            l2addr->tgid = BCM_GPORT_TRUNK_GET(l2addr->port);
            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
        } else if (BCM_GPORT_IS_MODPORT(l2addr->port)) {
            l2addr->modid = BCM_GPORT_MODPORT_MODID_GET(l2addr->port);
            l2addr->port = BCM_GPORT_MODPORT_PORT_GET(l2addr->port);
        } else if (BCM_GPORT_IS_LOCAL_CPU(l2addr->port)) {
            l2addr->modid = BCM_GPORT_MODPORT_MODID_GET(l2addr->port);
            l2addr->port = BCM_GPORT_MODPORT_PORT_GET(l2addr->port);
        } else if (BCM_GPORT_IS_SET(l2addr->port)) {
            cli_out(" port=%s", format_gport(gport_buf, l2addr->port));
        }
    } else
#endif
#ifdef BCM_ESW_SUPPORT
    if (!SOC_IS_ARAD(unit) && BCM_GPORT_IS_SET(l2addr->port)) {
        if (BCM_FAILURE(_bcm_esw_l2_gport_parse(unit, l2addr, &gport_params))) {
            cli_out("Unknown GPORT 0x%x \n", l2addr->port);
            return;
        }
#define L2_ADDR_GPORT_FORMAT_PRINT(_fmt, _gport) \
        cli_out(" port=%s", format_gport(_fmt, _gport))
        switch (gport_params.type) {
        case _SHR_GPORT_TYPE_MPLS_PORT:
        case _SHR_GPORT_TYPE_WLAN_PORT:
        case _SHR_GPORT_TYPE_MIM_PORT:
        case _SHR_GPORT_TYPE_TRILL_PORT:
        case _SHR_GPORT_TYPE_SUBPORT_GROUP:
        case _SHR_GPORT_TYPE_SUBPORT_PORT:
        case _SHR_GPORT_TYPE_VLAN_PORT:
        case _SHR_GPORT_TYPE_NIV_PORT:
        case _SHR_GPORT_TYPE_L2GRE_PORT:
        case _SHR_GPORT_TYPE_VXLAN_PORT:
        case _SHR_GPORT_TYPE_FLOW_PORT:
        case _SHR_GPORT_TYPE_EXTENDER_PORT:
        case _SHR_GPORT_TYPE_BLACK_HOLE:
            L2_ADDR_GPORT_FORMAT_PRINT(gpfmt, l2addr->port);
            break;
        case _SHR_GPORT_TYPE_TRUNK:
            l2addr->tgid = gport_params.param0;
            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
            break;
        case _SHR_GPORT_TYPE_MODPORT:
            l2addr->port = gport_params.param0;
            l2addr->modid = gport_params.param1;
            break;
        case _SHR_GPORT_TYPE_LOCAL_CPU:
            l2addr->port = gport_params.param0;
            l2addr->modid = local_modid;
            break;
        default:
            cli_out("Unknown GPORT format \n");
            return;
        }
#undef L2_ADDR_GPORT_FORMAT_PRINT
    }
    if (SOC_IS_ESW(unit)) {
        (void)_bcm_esw_modid_is_local(unit, l2addr->modid, &islocal);
    } else
#endif /* BCM_ESW_SUPPORT */
    {
        islocal = (l2addr->modid == local_modid) ? TRUE : FALSE;
    }

    if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
        cli_out(" Trunk=%d", l2addr->tgid);
    } else if (!BCM_GPORT_IS_SET(l2addr->port)) {
        cli_out(" modid=%d port=%d%s%s", l2addr->modid, l2addr->port,
                (islocal == TRUE) ? "/" : " ",
                (islocal == TRUE) ?
                mod_port_name(unit, l2addr->modid, l2addr->port) : " ");
    }

    if (l2addr->flags & BCM_L2_STATIC) {
        cli_out(" Static");
    }

    if (l2addr->flags & BCM_L2_HIT) {
        cli_out(" Hit");
    }

    if (l2addr->cos_src != 0 || l2addr->cos_dst != 0) {
            cli_out(" COS(src=%d,dst=%d)", l2addr->cos_src, l2addr->cos_dst);
    }

    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
        cli_out(" SCP");
    }

    if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
        cli_out(" CPU");
    } else if((l2addr->port == CMIC_PORT(unit)) && islocal) {
        if(!(l2addr->flags & (BCM_L2_L3LOOKUP | BCM_L2_TRUNK_MEMBER))) {
            cli_out(" CPU");
        }
    }

    if (l2addr->flags & BCM_L2_MIRROR) {
        cli_out(" Mirror");
    }

    if (l2addr->flags & BCM_L2_L3LOOKUP) {
        cli_out(" L3");
    }

    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
        cli_out(" DiscardSrc");
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        cli_out(" DiscardDest");
    }

    if (l2addr->flags & BCM_L2_PENDING) {
        cli_out(" Pending");
    }

    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (l2addr->flags & BCM_L2_SETPRI) {
            cli_out(" ReplacePriority");
        }
    }

    if (l2addr->flags & BCM_L2_MCAST) {
        cli_out(" MCast=%d", l2addr->l2mc_group);
    }

    if (SOC_PBMP_NOT_NULL(l2addr->block_bitmap)) {
        format_pbmp(unit, bmstr, sizeof (bmstr), l2addr->block_bitmap);
        cli_out(" MAC blocked port bitmap=%s: %s",
                SOC_PBMP_FMT(l2addr->block_bitmap, pfmt), bmstr);
    }

    if (l2addr->group) {
        cli_out(" Group=%d", l2addr->group);
    }
#ifdef BCM_PETRA_SUPPORT
        if(SOC_IS_ARAD(unit)) {
			cli_out(" encap_id=0x%x", l2addr->encap_id);
		}
#endif
    if (l2addr->flags2 & BCM_L2_FLAGS2_MESH) {
        cli_out(" Mesh");
    }

    cli_out("\n");
}

void
dump_l2_cache_addr(int unit, char *pfx, bcm_l2_cache_addr_t *l2caddr)
{
    cli_out("%smac=%02x:%02x:%02x:%02x:%02x:%02x",
            pfx,
            l2caddr->mac[0], l2caddr->mac[1], l2caddr->mac[2],
            l2caddr->mac[3], l2caddr->mac[4], l2caddr->mac[5]);

    if (!ENET_MACADDR_BROADCAST(l2caddr->mac_mask)) {
        cli_out("/%02x:%02x:%02x:%02x:%02x:%02x",
                l2caddr->mac_mask[0], l2caddr->mac_mask[1], l2caddr->mac_mask[2],
                l2caddr->mac_mask[3], l2caddr->mac_mask[4], l2caddr->mac_mask[5]);
    }

    cli_out(" vlan=%d", l2caddr->vlan);
    if (l2caddr->vlan_mask != BCM_L2_VID_MASK_ALL) {
        cli_out("/0x%03x", l2caddr->vlan_mask);
    }

    if (l2caddr->src_port_mask != 0) {
        cli_out(" srcport=%d", l2caddr->src_port);
        if (l2caddr->src_port_mask != BCM_L2_SRCPORT_MASK_ALL) {
            cli_out("/0x%02x", l2caddr->src_port_mask);
        }
    }

    if (l2caddr->flags & BCM_L2_CACHE_TRUNK) {
        cli_out(" trunk=%d", l2caddr->dest_trunk);
    } else {
        cli_out(" modid=%d port=%d/%s",
                l2caddr->dest_modid,
                l2caddr->dest_port,
                mod_port_name(unit, l2caddr->dest_modid,  l2caddr->dest_port));
    }

    if (l2caddr->flags & BCM_L2_CACHE_SETPRI) {
        cli_out(" prio=%d", l2caddr->prio);
    }

    if (l2caddr->flags & BCM_L2_CACHE_BPDU) {
        cli_out(" BPDU");
    }

    if ((l2caddr->flags & BCM_L2_CACHE_CPU) ||
        (l2caddr->dest_port == CMIC_PORT(unit))) {
        cli_out(" CPU");
    }

    if (l2caddr->flags & BCM_L2_CACHE_L3) {
        cli_out(" L3");
    }

    if (l2caddr->flags & BCM_L2_CACHE_DISCARD) {
        cli_out(" Discard");
    }

    if (l2caddr->flags & BCM_L2_CACHE_MIRROR) {
        cli_out(" Mirror");
    }

    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (l2caddr->flags & BCM_L2_CACHE_SETPRI) {
            cli_out(" ReplacePriority");
        }
    }
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit)) {
        if (l2caddr->flags & BCM_L2_CACHE_LEARN_DISABLE) {
            cli_out(" LearnDisable");
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT */

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        cli_out(" lookup_class =%d", l2caddr->lookup_class);
    }
#endif /* BCM_TRX_SUPPORT */
    if (l2caddr->flags & BCM_L2_CACHE_PROTO_PKT) {
        cli_out(" ProtocolPkt");
    }

     cli_out("\n");
}

/*
 * Format a pbmp into a string, the opposite of what parse_pbmp does.
 * For example, 0x00ffffff ==> "fe"; 0x0b000007 ==> "cpu,ge,fe0-fe2".
 * For clarity, returns "ge,fe" instead of "e" (or on GSL, just "ge"),
 * and returns "cpu,ge,fe" instead of "all" (or on GSL, just "cpu,ge").
 */

STATIC void
_format_pbmp_type(int unit,
                  char *dest,
                  int bufsize,
                  soc_pbmp_t pbmp,
                  char *pname,
                  soc_pbmp_t pall)
{
    soc_pbmp_t  bm;
    soc_port_t  tport, port, dport, pfirst, plast;
    int size = 0;

    if (SOC_PBMP_IS_NULL(pall)) {       /* no such ports */
        return;
    }

    SOC_PBMP_ASSIGN(bm, pbmp);
    SOC_PBMP_AND(bm, pall);
    if (SOC_PBMP_EQ(bm, pall)) {        /* all ports of this type */
        size = sal_strlen(dest);
        assert((bufsize - size) >= 0);
        sal_snprintf(dest + size,(bufsize - size), ",%s", pname);
        return;
    }
    /*
     * The slightly tricky thing here is handling sparse sets of
     * bits in pall (on Tucana or Herc4 for example).
     */
    pfirst = plast = -1;
    port = -1;
    /* coverity[overrun-local] */
    DPORT_SOC_PBMP_ITER(unit, pall, dport, tport) {        /* a subset of ports */
        port += 1;
        /* coverity[overrun-local] */
        if (SOC_PBMP_MEMBER(pbmp, tport)) {
            if (pfirst < 0) {
                pfirst = plast = (SOC_IS_DPP(unit)
#ifdef BCM_DNX_SUPPORT
                                || SOC_IS_DNX(unit)
#endif
#ifdef BCM_DNF_SUPPORT
                                || SOC_IS_DNXF(unit)
#endif
                                 )?dport:DPORT_FROM_DPORT_INDEX(unit, dport, port);
            } else {
                plast = (SOC_IS_DPP(unit)
#ifdef BCM_DNX_SUPPORT
                        || SOC_IS_DNX(unit)
#endif
#ifdef BCM_DNF_SUPPORT
                        || SOC_IS_DNXF(unit)
#endif
                        )?dport:DPORT_FROM_DPORT_INDEX(unit, dport, port);
            }
            continue;
        }
        if (pfirst >= 0) {
            size = sal_strlen(dest);
            assert((bufsize - size) >= 0);
            sal_snprintf(dest + sal_strlen(dest), (bufsize - size), ",%s%d", pname, pfirst);
            if (pfirst != plast) {
                size = sal_strlen(dest);
                assert((bufsize - size) >= 0);
                sal_snprintf(dest + sal_strlen(dest), (bufsize - size), "-%s%d", pname, plast);
            }
            pfirst = plast = -1;
        }
    }

    if (pfirst >= 0) {
        size = sal_strlen(dest);
        assert((bufsize - size) >= 0);
        sal_snprintf(dest + sal_strlen(dest), (bufsize - size), ",%s%d", pname, pfirst);
        if (pfirst != plast) {
            size = sal_strlen(dest);
            assert((bufsize - size) >= 0);
            sal_snprintf(dest + sal_strlen(dest), (bufsize - size), "-%s%d", pname, plast);
        }
    }
}

char *
format_pbmp(int unit, char *buf, int bufsize, soc_pbmp_t pbmp)
{
    char        tmp[FORMAT_PBMP_MAX];
    soc_pbmp_t  tpbm;

#if defined(BCM_LTSW_SUPPORT)
    if (SOC_IS_LTSW(unit)) {
        return ltsw_format_pbmp(unit, buf, bufsize, pbmp);
    }
#endif

    SOC_PBMP_ASSIGN(tpbm, pbmp);
    SOC_PBMP_REMOVE(tpbm, PBMP_ALL(unit));
    if (SOC_PBMP_NOT_NULL(tpbm)) {
        sal_strncpy(buf, SOC_PBMP_FMT(pbmp, tmp), bufsize);
    } else if (SOC_PBMP_IS_NULL(pbmp)) {
        /*
         * COVERITY
         *
         * Handled below
         */
        /*    coverity[buffer_size]    */
        sal_strncpy(buf, "none", bufsize);
        if (bufsize < 5) {
            /* Guarantee null termination of buf */
            buf[bufsize - 1] = 0;
        }
    } else {
        if (SOC_PBMP_MEMBER(pbmp, CMIC_PORT(unit)) != 0) {
            /* Coverity[secure_coding] */
            sal_strcpy(tmp, ",cpu");
        } else {
            tmp[0] = tmp[1] = 0;
        }

        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "cd", PBMP_CDE_ALL(unit));

        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "ce", PBMP_CE_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "le", PBMP_LE_ALL(unit));
        if(SOC_IS_SAND(unit)) {
            _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "xl", PBMP_XL_ALL(unit));
        }
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "fe", PBMP_FE_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "ge", PBMP_GE_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "xe", PBMP_XE_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "hg", PBMP_HG_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "spi", PBMP_SPI_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "sci", PBMP_SCI_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "sfi", PBMP_SFI_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "spi_s", PBMP_SPI_SUBPORT_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "hg_s", PBMP_HG_SUBPORT_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "il", PBMP_IL_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "qsgmii", PBMP_QSGMII_ALL(unit));
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "roe", PBMP_ROE_ALL(unit));
        sal_strncpy(buf, tmp + 1, bufsize);
    }
    return buf;
}

char *
format_bcm_pbmp(int unit, char *buf, int bufsize, bcm_pbmp_t pbmp)
{
    pbmp_t      tpbm;

    SOC_PBMP_ASSIGN(tpbm, pbmp);
    BCM_API_XLATE_PORT_PBMP_A2P(unit, &tpbm);
    return format_pbmp(unit, buf, bufsize, tpbm);
}

char *
bcm_port_name(int unit, int port)
{
    char *name = NULL;

#if defined(BCM_LTSW_SUPPORT)
    if (SOC_IS_LTSW(unit)) {
        return ltsw_port_name(unit, port);
    }
#endif

    BCM_API_XLATE_PORT_A2P(unit, &port);
    if (SOC_PORT_VALID_RANGE(unit, port)) {
        name = SOC_PORT_NAME(unit, port);
    }
    if (name == NULL) {
        name = "<unknown>";
    }
    return name;
}

char *
mod_port_name(int unit, int modid, int port)
{
    bcm_gport_t gport;
    bcm_port_t local_port;

    if (BCM_GPORT_IS_SET(port)) {
	/* If port is already a gport, no conversion necessary */
        gport = port;
    } else {
	/* else convert modid/port to gport */
        BCM_GPORT_MODPORT_SET(gport, modid, port);
    }
    if (BCM_SUCCESS(bcm_port_local_get(unit, gport, &local_port))) {
        return bcm_port_name(unit, local_port);
    }
    return "";
}

/*
 * Parse a string containing a range of COS's.
 * The input format is:
 *
 *      COS_RANGE       : COS '-' COS           { return $1 .. $2; }
 *                      | COS                   { return $1 .. $1; }
 *                      | {NULL} | '' | 'ALL'   { return 0 .. NUM_COS - 1; }
 *                      ;
 *              COS     : INTEGER{NUM_COS}      { return $1; }
 *                      ;
 */

int
parse_num_range(int unit, char *s, int *min, int *max, int legal_min,
                int legal_max)
{
    if (s == 0 || *s == 0 || sal_strcasecmp(s, "all") == 0) {
        *min = legal_min;
        *max = legal_max;
        return 0;
    }

    if ((*min = *s++ - '0') < legal_min || *min > legal_max) {
        return -1;
    }

    if (*s == 0) {
        *max = *min;
        return 0;
    }

    if (*s++ != '-') {
        return -1;
    }

    if ((*max = *s++ - '0') > legal_max) {
        return -1;
    }

    return (*s == 0 && *max >= *min) ? 0 : -1;
}

int
diag_parse_range(char *low, char *high,
                 int *min, int *max,
                 int legal_min, int legal_max)
{
    if (low == 0 || *low == 0 || sal_strcasecmp(low, "all") == 0) {
        *min = legal_min;
        *max = legal_max;
        return 0;
    }

    *min = sal_strtoul(low, NULL, 0);

    if ((*min < legal_min) || *min > legal_max) {
        return -1;
    }

    if (high == 0 || *high == 0) {
        *max = *min;
        return 0;
    }

    *max = sal_strtoul(high, NULL, 0);

    if (*max < *min || *max > legal_max) {
        return -1;
    }

    return 0;
}

/*
 * Utility routine for parse_block_range
 */

STATIC int
_parse_block(int unit, soc_block_types_t block_types, char **s, int *nump)
{
    char        *sblk, *snum;
    int         blk, num;
    int         i, phy_port;
    soc_block_t blktype;
    soc_block_name_t *blk_names;
    int         port_num_blktype;

    {
#if defined(BCM_SAND_SUPPORT)
        if (SOC_IS_SAND(unit)) {
            blk_names = soc_dpp_block_names;
        }
        else
#endif
        {
            blk_names = soc_block_names;
        }
    }

    if (sal_strncasecmp("PORT_GROUP", *s, sizeof("PORT_GROUP") - 1) == 0) {
        switch ((*s)[sizeof("PORT_GROUP") - 1]) {
        case '4':
            blktype = SOC_BLK_PORT_GROUP4;
            break;
        case '5':
            blktype = SOC_BLK_PORT_GROUP5;
            break;
        default:
            return -1;
        }
        switch ((*s)[sizeof("PORT_GROUP*_") - 1]) {
        case 'X':
        case 'x':
            num = 0;
            break;
        case 'Y':
        case 'y':
            num = 1;
            break;
        default:
            return -1;
        }
        SOC_BLOCK_ITER(unit, blk, blktype) {
            if (SOC_BLOCK_INFO(unit, blk).number == num) {
                *s += sizeof("PORT_GROUP*_*") - 1;
                return blk;
            }
        }
        return -1;
    }

    /* To cover for Apache XLPORTB0 and CLG2PORT blocks  */
    if ((sal_strncasecmp("XLB0PORT", *s, (sizeof("XLB0PORT") - 1)) == 0) ||
        (sal_strncasecmp("CLG2PORT", *s, (sizeof("CLG2PORT") - 1)) == 0)) {
        sblk = *s + 8;
    } else {
        /* find the last "number" during string "s"
           ex. s = "pgw_ge8p2" ==> sblk = "2"
         */
        int index = sal_strlen(*s);

        sblk = &((*s)[index]);
        while ((index-1) >= 0 &&
               ((*s)[index-1] >= '0' && (*s)[index-1] <= '9')) {
            sblk--;
            index--;
        }
    }

    /* skip past prefix */
    num = 0;
    snum = sblk;
    while (*snum && *snum >= '0' && *snum <= '9') {
        num = (num * 10) + (*snum - '0');
        snum++;
    }

    /* try to match a port name before we split the prefix */
    PBMP_ALL_ITER(unit, i) {
        if (sal_strncasecmp(SOC_PORT_NAME(unit, i), *s, snum-*s) == 0) {
            *s = snum;
            if (soc_feature(unit, soc_feature_logical_port_num)) {
                phy_port = SOC_INFO(unit).port_l2p_mapping[i];
            } else {
                phy_port = i;
            }

            port_num_blktype = SOC_DRIVER(unit)->port_num_blktype > 1 ?
                SOC_DRIVER(unit)->port_num_blktype : 1;
            if (port_num_blktype > 1) {
                for (i = 0; i <  port_num_blktype; i++) {
                    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
                    if (blk < 0) {
                        break;
                    }
                    if (SOC_BLOCK_IN_LIST(unit, block_types,
                                          SOC_BLOCK_TYPE(unit, blk))) {
                        return blk;
                    }
                }
                return -1;
            } else {
                return SOC_PORT_BLOCK(unit, phy_port);
            }
        }
    }

    *sblk = '\0';
    sblk = *s;
    *s = snum;

    if (*sblk == '\0') {        /* just digits */
        if (nump == NULL) {
            return -1;
        }
        *nump = num;
        return 0;
    }

    /* try to match a block name */
    for (i = 0; blk_names[i].blk != SOC_BLK_NONE; i++) {
        if (sal_strcasecmp(blk_names[i].name, sblk) == 0) {
            blktype = blk_names[i].blk;
            SOC_BLOCK_ITER(unit, blk, blktype) {
                if (SOC_BLOCK_INFO(unit, blk).number == num) {
                    return blk;
                }
            }
            return -1;
        }
    }

    return -1;
}

/*
 * Parse a string containing a range of StrataSwitch internal blocks.
 * The input format is:
 *
 *      BLK_RANGE   : BLOCK '-' BLOCK   { return $1 .. $2; }
 *                  | BLOCK             { return $1 .. $1; }
 *                  | 'E'               { return MIN_EPIC .. MAX_EPIC; }
 *                  | 'G'               { return MIN_GPIC .. MAX_GPIC; }
 *                  | 'H'               { return MIN_HPIC .. MAX_HPIC; }
 *                  | 'ALL'             { return MIN_EPIC .. MAX_GPIC; }
 *                  ;
 *      BLOCK       : INTEGER{NUM_PIC}          { return $1; }
 *                  | 'E' INTEGER{NUM_EPIC}     { return MIN_EPIC + $1; }
 *                  | 'G' INTEGER{NUM_GPIC}     { return MIN_GPIC + $1; }
 *                  | 'H' INTEGER{NUM_GPIC}     { return MIN_HPIC + $1; }
 *                  ;
 */

int
parse_block_range(int unit,
                  soc_block_types_t regtype,
                  char *s,
                  int *min,
                  int *max,
                  soc_block_types_t mask)
{
    int i, nummin, nummax;
    int matched, blk, idx = 0;
    soc_block_name_t *blk_port_names;
    soc_block_name_t *blk_names;

    {
#if defined(BCM_SAND_SUPPORT)
        if (SOC_IS_SAND(unit)) {
            blk_names = soc_dpp_block_names;
            blk_port_names = soc_dpp_block_port_names;
        }
        else
#endif
        {
            blk_names = soc_block_names;
            blk_port_names = soc_block_port_names;
        }
    }

    *min = 0;
    *max = SOC_INFO(unit).block_num;

    /* empty string: all blocks */
    if (s == NULL || *s == '\0') {
        return 0;
    }

    /* all blocks */
    if (sal_strcasecmp(s, "all") == 0 || sal_strcasecmp(s, "*") == 0) {
        return 0;
    }

    /* all blocks of a certain type */
    for (i = 0; blk_names[i].blk != SOC_BLK_NONE; i++) {
        if (sal_strcasecmp(blk_names[i].name, s) == 0) {
            mask[0] = blk_names[i].blk;
            return 0;
        }
    }

    if (SOC_IS_ESW(unit)) {
        /* all blocks of a certain port type */
        /* (remember, hg matches hpic and ipic) */
        matched = 0;
        for (i = 0; blk_port_names[i].blk != SOC_BLK_NONE; i++) {
            if (sal_strcasecmp(blk_port_names[i].name, s) == 0) {
                mask[idx] = blk_port_names[i].blk;
                matched += 1;
                idx++;
            }
        }
        if (matched) {
            return 0;
        }
    }

    nummin = -1;
    if ((*min = _parse_block(unit, regtype, &s, &nummin)) < 0) {
        return -1;
    }

    if (*s == '\0') {
        if (nummin < 0) {
            *max = *min;
            return 0;
        }
        /* find the nummin'th port block */
        matched = -1;
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_PORT) {
            matched += 1;
            if (matched == nummin) {
                *max = *min = blk;
                return 0;
            }
        }
        return -1;
    }

    if (*s++ != '-') {
        return -1;
    }

    nummax = -1;
    if ((*max = _parse_block(unit, regtype, &s, &nummax)) < 0) {
        return -1;
    }
    if (*s != '\0') {
        return -1;
    }
    if ((nummin >= 0 && nummax < 0) || (nummin < 0 && nummax >= 0)) {
        return -1;
    }
    if (nummin < 0 && nummax < 0) {
        if (*min > *max) {
            return -1;
        }
        return 0;
    }
    if (nummin > nummax) {
        return -1;
    }
    /* find the nummin'th through nummax'th blocks */
    matched = -1;
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_PORT) {
        matched += 1;
        if (matched == nummin) {
            *min = blk;
        }
        if (matched == nummax) {
            *max = blk;
            return 0;
        }
    }
    return -1;
}

/*
 * Parse a CMIC register name and return offset.  Matching is
 * case-insensitive and works with or without the "CMIC_" prefix.  Also
 * accepts address offset as an integer in hex or decimal.
 */

int
parse_cmic_regname(int unit, char *name, uint32 *offset_ptr)
{

#if defined(BCM_ESW_SUPPORT) || defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
    if (isint(name)) {
        *offset_ptr = parse_integer(name);
        return 0;       /* Success */
    }
#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        char *s;
        int i;
        soc_cmicm_reg_t *creg;
        for (i = 0; i < NUM_CMICM_REGS; i++) {
            creg = soc_cmicm_reg_get(i);
            s = creg->name;
            if (sal_strcasecmp(name, s) == 0 || sal_strcasecmp(name, s + 5) == 0) {
                *offset_ptr = creg->addr;
                return 0;
            }
        }
        return -1;
    }
#endif

#ifdef BCM_CMICX_SUPPORT
    if(soc_feature(unit, soc_feature_cmicx)) {
        return(soc_parse_cmicx_regname(unit, name, offset_ptr));
    }
#endif

    for (*offset_ptr = 0; *offset_ptr <= 0x10000; (*offset_ptr) += 4) {
        char *s = soc_pci_off2name(unit, *offset_ptr);
        if (sal_strcasecmp(name, s) == 0 || sal_strcasecmp(name, s + 5) == 0)
            return 0;   /* Success */
    }
#endif
    return -1;
}

/*
 * Look up a table memory by user-friendly name.
 * On success, stores result and returns 0.  On failure, returns -1.
 *
 * A memory array index may be added after the memory name in square brackets like: mem[3].
 * The function will fail for memory arrays for which an array index
 * is not specified when array_index is non-NULL.
 * The function will fail for non array memories for which an array index is specified.
 * For non array memories, array_index may be NULL; If not Null 0 will be returned in it.
 *
 * A table copy number may be appended to the table name following a
 * period, e.g. "mcast.2".  If so, this value is returned in the copyno
 * parameter.  If not, COPYNO_ALL is returned in the copyno parameter.
 * If copyno is NULL, an appended copy number causes a syntax error.
 *
 * Also accepts suffixes .E0-E2 and .G0-G1 for StrataSwitch
 * Also accepts suffixes .G0-.G11 and .H0 for Draco
 * Also accepts suffixes .H0-H7 for Hercules
 */

int
parse_memory_name(int unit, soc_mem_t *result, char *str, int *copyno, unsigned *array_index)
{
    soc_mem_t   mem;
    int         blk, numblk, matched;
    char        *cp, *aip, tmp[80];
    uint64      blkbits;
    unsigned    arr_ind;
    int         block_types[2];

    if (str == NULL || *str == '\0') {
        return -1;
    }

    /* Do not destroy input string */
    sal_strncpy(tmp, str, sizeof (tmp));
    tmp[sizeof (tmp) - 1] = '\0';

    blk = COPYNO_ALL;
    numblk = -1;
    if (( ((aip = sal_strchr(tmp, '[')) != NULL) &&
                ((cp = sal_strchr(aip+1, ']')) != NULL) && (cp-aip > 1) )  ||
            ( ((aip = sal_strchr(tmp, '(')) != NULL) &&
              ((cp = sal_strchr(aip+1, ')')) != NULL) && (cp-aip > 1) ) )   {
        /* handle memory array */



        /* handle initial spaces after '[' */
        for (cp = aip+1; *cp == ' '; ++cp) {
        }
        if (*cp < '0' || *cp >'9') {
            return -1;
        }
        /* read array index */
        arr_ind = 0;
        for (; *cp >= '0' && *cp <='9'; ++cp) {
            arr_ind = arr_ind * 10 + (*cp - '0');
        }
        /* handle trailing spaces before ']' */
        for (; *cp == ' '; ++cp) {
        }
        if ((*cp != ']') && (*cp != ')')) { /* verify closing ']' */
            return -1;
        }
        if (array_index == NULL) {
            return -1;
        } else {
            *array_index = arr_ind;
        }
        cp = sal_strchr(cp+1, '.');
        *aip = '\0';
    } else {
        arr_ind = 0xffffffff;
        cp = sal_strchr(tmp, '.');
        if (array_index != NULL) {
            *array_index = 0;
        }
    }

    if (cp != NULL) {
        if (copyno == NULL) {
            return -1;
        }
        *cp++ = '\0';
    }

    mem = -1;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if (sal_strcasecmp(tmp, "EXACT_MATCH_ONLY_DOUBLE") == 0) {
            sal_strncpy(tmp, "EXACT_MATCH_2", sizeof (tmp));
        }
        if (sal_strcasecmp(tmp, "EXACT_MATCH_ONLY_QUAD") == 0) {
            sal_strncpy(tmp, "EXACT_MATCH_4", sizeof (tmp));
        }
    }
#endif

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }

        if (sal_strcasecmp(tmp, SOC_MEM_NAME(unit, mem)) == 0) {
            break;
        }

        if (sal_strcasecmp(tmp, SOC_MEM_UFNAME(unit, mem)) == 0) {
            break;
        }

        if (SOC_MEM_UFALIAS(unit, mem) != NULL &&
            sal_strcasecmp(tmp, SOC_MEM_UFALIAS(unit, mem)) == 0) {
            break;
        }
    }
    if (mem == NUM_SOC_MEM) {
        return -1;
    }
#endif

    if (cp != NULL) {
#if  defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
        blk = SOC_MEM_BLOCK_ANY(unit, mem);
        block_types[0] = SOC_BLOCK_TYPE(unit, blk);
#else
        block_types[0] = SOC_BLOCK_TYPE_INVALID;
#endif
        block_types[1] = SOC_BLOCK_TYPE_INVALID;
        blk = _parse_block(unit, block_types, &cp, &numblk);
        if (blk < 0 || *cp != '\0') {
            return -1;
        }
    }

    /* memory array related validation */
    if (arr_ind != 0xffffffff) {
        /* an array index was specified, verify that this is a memory array and that the index is in range */
        if (!SOC_MEM_IS_ARRAY(unit, mem)) {
            cli_out("ERROR: an array index was specified for a memory which is not an array\n");
            return -1;
        } else if  (arr_ind >= (SOC_MEM_NUMELS(unit, mem) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem)) || arr_ind < SOC_MEM_FIRST_ARRAY_INDEX(unit, mem)) {
            cli_out("ERROR: array index %u is out of range, should be under %u and above %u \n ",
                    arr_ind, (SOC_MEM_FIRST_ARRAY_INDEX(unit, mem) + SOC_MEM_NUMELS(unit, mem) - 1), SOC_MEM_FIRST_ARRAY_INDEX(unit, mem));
            return -1;
        }
    } else if (array_index != NULL && SOC_MEM_IS_ARRAY(unit, mem)) {
        /* no array index specified, but this is a memory array */
        cli_out("ERROR: an array index was not specified for a memory array\n");
        return -1;
    }

    if (numblk >= 0) {
        if (SOC_IS_XGS12_FABRIC(unit)) {
            /*
             * numblk is the index into a list of SOC_BLK_PORT blocks
             * well, thats intuitive, isn't it?
             * This makes numblk==0 be the cpic in all cases.
             * AAAAAAAARRRRRRRRRRRGGGGGGGGGGGHHHHHHHHHHHH!!!!!!!
             */
            matched = -1;
            SOC_BLOCK_ITER(unit, blk, SOC_BLK_PORT) {
                matched += 1;
                if (matched == numblk) {
                    break;
                }
            }
        } else {
            /*
             * numblk is the index into the list of blocks valid
             * for this memory (ie: it's a copy number)
             */
            COMPILER_64_SET(blkbits, SOC_MEM_INFO(unit, mem).blocks_hi,
                            SOC_MEM_INFO(unit, mem).blocks);
            matched = -1;
            for (blk = 0; !COMPILER_64_IS_ZERO(blkbits);
                 blk++) {
                if (COMPILER_64_BITTEST(blkbits, 1)) {
                    matched += 1;
                    if (matched == numblk) {
                        break;
                    }
                }
                COMPILER_64_SHR(blkbits, 1);
            }
        }
        if (matched != numblk) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
            cli_out("ERROR: memory %s.%d is not valid (not enough blocks)\n",
                    SOC_MEM_UFNAME(unit, mem), numblk);
#endif
            return -1;
        }
    }

    *result = mem;
    if (copyno != NULL) {
        *copyno = blk;
    }

    if (blk != COPYNO_ALL && !SOC_MEM_BLOCK_VALID(unit, mem, blk)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
        cli_out("ERROR: memory %s.%s (block %d) is not valid\n",
                SOC_MEM_UFNAME(unit, mem),
                SOC_BLOCK_NAME(unit, blk),
                blk);
#endif
        return -1;
    }
    return 0;
}

#if defined(BCM_ESW_SUPPORT)
/*
 * parse_format_name
 *
 *   Parses a string containing format name.
 *   and return format ID.
 */
int
parse_format_name(int unit, char *str, soc_format_t *format)
{

    int f_idx = 0;
    if (str == NULL || *str == '\0') {
        return -1;
    }
    for (f_idx = 0; f_idx < NUM_SOC_FORMAT; f_idx++) {
        if (!SOC_FORMAT_IS_VALID(unit, f_idx)) {
            continue;
        }
        if (sal_strcmp(str,
            SOC_FORMAT_NAME(unit, f_idx)) == 0) {
            *format = f_idx;
            return 0;
        }
    }
    return -1;
}
#endif /* BCM_ESW_SUPPORT */
/*
 * parse_memory_index
 *
 *   Parses a string containing an integer memory index, allowing
 *   the two special values "min" and "max" to indicate the minimum
 *   and maximum indices.
 */

int
parse_memory_index(int unit, soc_mem_t mem, char *val_str)
{
    int index;

    index = 0;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
    if (!sal_strcasecmp(val_str, "min")) {
        return soc_mem_index_min(unit, mem);
    }

    if (!sal_strcasecmp(val_str, "max")) {
        return soc_mem_index_max(unit, mem);
    }

    index = parse_integer(val_str);

    if (!soc_mem_index_valid(unit, mem, index)) {
        if (!(SOC_DEFIP_HOLE_RANGE_CHECK(unit, mem, index))) {
            cli_out("WARNING: index %d out of range for memory %s\n",
                index, SOC_MEM_UFNAME(unit, mem));
        }
    }
#endif

    return index;
}

/*
 * parse_memory_index
 *
 *   Parses a string containing an integer memory array index, allowing
 *   the two special values "min" and "max" to indicate the minimum
 *   and maximum indices.
 */

unsigned
parse_memory_array_index(int unit, soc_mem_t mem, char *val_str)
{
    unsigned array_index = 0;

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
    if (SOC_MEM_IS_ARRAY(unit, mem)) {
        soc_mem_array_info_t *maip = SOC_MEM_ARRAY_INFOP(unit, mem);
        if (maip) {
            if (!sal_strcasecmp(val_str, "min")) {
                return maip->first_array_index;
            } else if (!sal_strcasecmp(val_str, "max")) {
                return (maip->numels - 1 + maip->first_array_index);
            }
            array_index = parse_integer(val_str);
            if (array_index >= maip->numels + maip->first_array_index) {
                cli_out("WARNING: array index %u out of range for memory %s, changing it to %u\n",
                        array_index, SOC_MEM_UFNAME(unit, mem), maip->numels - 1);
                array_index = (maip->numels - 1) + maip->first_array_index;
            } else if (array_index < maip->first_array_index) {
                cli_out("WARNING: array index %u out of range for memory %s, changing it to %u\n",
                        array_index, SOC_MEM_UFNAME(unit, mem), maip->first_array_index);
                array_index = maip->first_array_index;
            }
        }
    }
#endif

    return array_index;
}

/*
 * parse_port_mode
 *
 *   Parses a port mode string consisting of a comma-separated
 *   list of modes.  Examples of valid input are:
 *
 *      1000full,100full,100half,10full,10half
 *      1000full,100,10    (equivalent to above)
 *      1000full,100,10,pause
 */

static struct {
    char                *str;
    soc_port_mode_t     pm;
} pm_map[] = {
    /* Multi-bit masks should come before their comprised 1-bit masks */
    { "full",      SOC_PM_FD },
    { "half",      SOC_PM_HD },
    { "16G",       SOC_PM_16GB },
    { "16Gfull",   SOC_PM_16GB_FD },
    { "16Ghalf",   SOC_PM_16GB_HD },
    { "13G",       SOC_PM_13GB },
    { "13Gfull",   SOC_PM_13GB_FD},
    { "13Ghalf",   SOC_PM_13GB_HD},
    { "12G",       SOC_PM_12GB },
    { "12Gfull",   SOC_PM_12GB_FD},
    { "12Ghalf",   SOC_PM_12GB_HD},
    { "10G",       SOC_PM_10GB },
    { "10Gfull",   SOC_PM_10GB_FD },
    { "10Ghalf",   SOC_PM_10GB_HD },
    { "2500",      SOC_PM_2500MB },
    { "2500full",  SOC_PM_2500MB_FD },
    { "2500half",  SOC_PM_2500MB_HD },
    { "1000",      SOC_PM_1000MB },
    { "1000full",  SOC_PM_1000MB_FD },
    { "1000half",  SOC_PM_1000MB_HD },
    { "100",       SOC_PM_100MB },
    { "100full",   SOC_PM_100MB_FD },
    { "100half",   SOC_PM_100MB_HD },
    { "10",        SOC_PM_10MB },
    { "10full",    SOC_PM_10MB_FD },
    { "10half",    SOC_PM_10MB_HD },
    { "pause",     SOC_PM_PAUSE },
    { "pause_tx",  SOC_PM_PAUSE_TX },
    { "pause_rx",  SOC_PM_PAUSE_RX },
    { "TBI",       SOC_PM_TBI },
    { "MII",       SOC_PM_MII },
    { "GMII",      SOC_PM_GMII },
    { "SGMII",     SOC_PM_SGMII },
    { "XGMII",     SOC_PM_XGMII },
    { "combo",     SOC_PM_COMBO },
    { NULL, 0 },
};

int
parse_port_mode(char *str, soc_port_mode_t *mode)
{
    int                 i;
    char                *s = str;

    *mode = 0;

    while (*s != 0) {
        for (i = 0; pm_map[i].str != NULL; i++) {
            int len = sal_strlen(pm_map[i].str);

            if (sal_strncasecmp(s, pm_map[i].str, len) == 0 &&
                (s[len] == 0 || s[len] == ',')) {

                *mode |= pm_map[i].pm;

                if (*(s += len) == ',') {
                    s++;
                }

                break;
            }
        }

        if (pm_map[i].str == NULL) {
            return -1;
        }
    }

    return 0;
}

#define TYPE_SP_FD  1
#define TYPE_SP_HD  2
#define TYPE_PAUSE  3
#define TYPE_INTF   4
#define TYPE_FLAGS  5
#define TYPE_DPLX   6
#define TYPE_CHANNEL 7
#define TYPE_FEC    8
#define TYPE_IGNORE 0
#define FULL_DUPLEX 1
#define HALF_DUPLEX 2
static struct {
    char *str;
    int  type;
    soc_port_mode_t     pm;
} pa_map[] = {
    /* Multi-bit masks should come before their comprised 1-bit masks */
    { "full",      TYPE_DPLX,       FULL_DUPLEX },
    { "half",      TYPE_DPLX,       HALF_DUPLEX },
    { "127G",      TYPE_SP_FD,      SOC_PA_SPEED_127GB },
    { "120G",      TYPE_SP_FD,      SOC_PA_SPEED_120GB },
    { "106G",      TYPE_SP_FD,      SOC_PA_SPEED_106GB },
    { "100G",      TYPE_SP_FD,      SOC_PA_SPEED_100GB },
    { "53G",       TYPE_SP_FD,      SOC_PA_SPEED_53GB },
    { "50G",       TYPE_SP_FD,      SOC_PA_SPEED_50GB },
    { "42G",       TYPE_SP_FD,      SOC_PA_SPEED_42GB },
    { "40G",       TYPE_SP_FD,      SOC_PA_SPEED_40GB },
    { "32G",       TYPE_SP_FD,      SOC_PA_SPEED_32GB },
    { "30G",       TYPE_SP_FD,      SOC_PA_SPEED_30GB },
    { "27G",       TYPE_SP_FD,      SOC_PA_SPEED_27GB },
    { "25G",       TYPE_SP_FD,      SOC_PA_SPEED_25GB },
    { "24G",       TYPE_SP_FD,      SOC_PA_SPEED_24GB },
    { "21G",       TYPE_SP_FD,      SOC_PA_SPEED_21GB },
    { "20G",       TYPE_SP_FD,      SOC_PA_SPEED_20GB },
    { "16G",       TYPE_SP_FD,      SOC_PA_SPEED_16GB },
    { "16Gfull",   TYPE_SP_FD,      SOC_PA_SPEED_16GB},
    { "16Ghalf",   TYPE_SP_HD,      SOC_PA_SPEED_16GB},
    { "15G",       TYPE_SP_FD,      SOC_PA_SPEED_15GB},
    { "13G",       TYPE_SP_FD,      SOC_PA_SPEED_13GB},
    { "13Gfull",   TYPE_SP_FD,      SOC_PA_SPEED_13GB},
    { "13Ghalf",   TYPE_SP_HD,      SOC_PA_SPEED_13GB},
    { "12G",       TYPE_SP_FD,      SOC_PA_SPEED_12GB},
    { "12Gfull",   TYPE_SP_FD,      SOC_PA_SPEED_12GB},
    { "12Ghalf",   TYPE_SP_HD,      SOC_PA_SPEED_12GB},
    { "10G",       TYPE_SP_FD,      SOC_PA_SPEED_10GB},
    { "10Gfull",   TYPE_SP_FD,      SOC_PA_SPEED_10GB},
    { "10Ghalf",   TYPE_SP_HD,      SOC_PA_SPEED_10GB},
    { "5000",      TYPE_SP_FD,      SOC_PA_SPEED_5000MB},
    { "5G",        TYPE_SP_FD,      SOC_PA_SPEED_5000MB},
    { "2500",      TYPE_SP_FD,      SOC_PA_SPEED_2500MB},
    { "2500full",  TYPE_SP_FD,      SOC_PA_SPEED_2500MB},
    { "2500half",  TYPE_SP_HD,      SOC_PA_SPEED_2500MB},
    { "1000",      TYPE_SP_FD,      SOC_PA_SPEED_1000MB},
    { "1000full",  TYPE_SP_FD,      SOC_PA_SPEED_1000MB},
    { "1000half",  TYPE_SP_HD,      SOC_PA_SPEED_1000MB},
    { "100",       TYPE_SP_FD,      SOC_PA_SPEED_100MB},
    { "100full",   TYPE_SP_FD,      SOC_PA_SPEED_100MB},
    { "100half",   TYPE_SP_HD,      SOC_PA_SPEED_100MB},
    { "10",        TYPE_SP_FD,      SOC_PA_SPEED_10MB},
    { "10full",    TYPE_SP_FD,      SOC_PA_SPEED_10MB},
    { "10half",    TYPE_SP_HD,      SOC_PA_SPEED_10MB},
    { "pause",     TYPE_PAUSE,      SOC_PA_PAUSE},
    { "pause_tx",  TYPE_PAUSE,      SOC_PA_PAUSE_TX},
    { "pause_rx",  TYPE_PAUSE,      SOC_PA_PAUSE_RX},
    { "TBI",       TYPE_INTF,       SOC_PA_INTF_TBI},
    { "MII",       TYPE_INTF,       SOC_PA_INTF_MII},
    { "GMII",      TYPE_INTF,       SOC_PA_INTF_GMII},
    { "SGMII",     TYPE_INTF,       SOC_PA_INTF_SGMII},
    { "XGMII",     TYPE_INTF,       SOC_PA_INTF_XGMII},
    { "combo",     TYPE_FLAGS,      SOC_PA_COMBO},
    { "short",     TYPE_CHANNEL,    SOC_PA_CHANNEL_SHORT},
    { "long",      TYPE_CHANNEL,    SOC_PA_CHANNEL_LONG},
    { "fec_none",  TYPE_FEC,        SOC_PA_FEC_NONE},
    { "cl74",      TYPE_FEC,        SOC_PA_FEC_CL74},
    { "cl91",      TYPE_FEC,        SOC_PA_FEC_CL91},
    { NULL, 0,0 },
};

int
parse_port_ability(char *str, soc_port_ability_t *abil)
{
    int                 i;
    int dplx = 0;
    char                *s = str;

    sal_memset((char *)abil, 0, sizeof(soc_port_ability_t));
    while (*s != 0) {
        for (i = 0; pa_map[i].str != NULL; i++) {
            int len = sal_strlen(pa_map[i].str);
            if (sal_strncasecmp(s, pa_map[i].str, len) == 0 &&
                (s[len] == 0 || s[len] == ',')) {
                switch (pa_map[i].type) {
                    case TYPE_DPLX:
                        dplx = pa_map[i].pm;
                        break;
                    case TYPE_SP_FD:
                        if (dplx == HALF_DUPLEX) {
                            abil->speed_half_duplex |= pa_map[i].pm;
                        } else {
                            abil->speed_full_duplex |= pa_map[i].pm;
                        }
                        break;
                    case TYPE_SP_HD:
                        if (dplx == FULL_DUPLEX) {
                            abil->speed_full_duplex |= pa_map[i].pm;
                        } else {
                            abil->speed_half_duplex |= pa_map[i].pm;
                        }
                        break;
                    case TYPE_PAUSE:
                        abil->pause |= pa_map[i].pm;
                        break;
                    case TYPE_INTF:
                        abil->interface |= pa_map[i].pm;
                        break;
                    case TYPE_FLAGS:
                        abil->flags |= pa_map[i].pm;
                        break;
                    case TYPE_CHANNEL:
                        abil->channel |= pa_map[i].pm;
                        break;
                    case TYPE_FEC:
                        abil->fec |= pa_map[i].pm;
                        break;
                    default:
                        break;
                }

                if (*(s += len) == ',') {
                    s++;
                }

                break;
            }
        }

        if (pa_map[i].str == NULL) {
            return -1;
        }
    }

    return 0;
}


/*
 * format_port_mode
 *
 *   Formats a port mode into a comma-separated list of port mode
 *   strings.  This is the reverse of parse_port_mode.
 *
 *   If abbrev is TRUE, then "1000full,1000half" will be abbreviated
 *   as "1000".
 *
 *   Output buffer must be at least 80 characters.
 */

void
format_port_mode(char *buf, int bufsize, soc_port_mode_t mode, int abbrev)
{
    int                 i, first = 1;

    assert(bufsize >= 80);

    buf[0] = 0;

    for (i = 0; mode != 0 && pm_map[i].str != NULL; i++) {
        if (!abbrev && _shr_popcount(pm_map[i].pm) != 1) {
            continue;
        }

        if ((mode & pm_map[i].pm) == pm_map[i].pm) {
            if (first) {
                first = 0;
            } else {
                *buf++ = ',';
            }

            /* Coverity[secure_coding] */
            sal_strcpy(buf, pm_map[i].str);

            while (*buf != 0) {
                buf++;
            }

            mode &= ~pm_map[i].pm;
        }
    }
}


/* Port Ability structure */

typedef struct {
    char                *str;
    soc_port_mode_t  pa;
} _port_ability_map_t;


/* Port Ability speed mapping */
_port_ability_map_t pa_speed_map[] = {
    /* Multi-bit masks should come before their comprised 1-bit masks */
    { "10MB",           SOC_PA_SPEED_10MB },
    { "100MB",          SOC_PA_SPEED_100MB },
    { "1000MB",         SOC_PA_SPEED_1000MB },
    { "2500MB",         SOC_PA_SPEED_2500MB },
    { "3000MB",         SOC_PA_SPEED_3000MB },
    { "5000MB",         SOC_PA_SPEED_5000MB },
    { "6000MB",         SOC_PA_SPEED_6000MB },
    { "10GB",           SOC_PA_SPEED_10GB},
    { "11GB",           SOC_PA_SPEED_11GB},
    { "12GB",           SOC_PA_SPEED_12GB},
    { "12GP5",          SOC_PA_SPEED_12P5GB},
    { "13GB",           SOC_PA_SPEED_13GB},
    { "15GB",           SOC_PA_SPEED_15GB},
    { "16GB",           SOC_PA_SPEED_16GB},
    { "20GB",           SOC_PA_SPEED_20GB},

    { "21GB",           SOC_PA_SPEED_21GB},
    { "24GB",           SOC_PA_SPEED_24GB},

    { "25GB",           SOC_PA_SPEED_25GB},
    { "27GB",           SOC_PA_SPEED_27GB},

    { "30GB",           SOC_PA_SPEED_30GB},
    { "32GB",           SOC_PA_SPEED_32GB},
    { "40GB",           SOC_PA_SPEED_40GB},

    { "42GB",           SOC_PA_SPEED_42GB},
    { "50GB",           SOC_PA_SPEED_50GB},
    { "53GB",           SOC_PA_SPEED_53GB},
    { "100GB",          SOC_PA_SPEED_100GB},
    { "106GB",          SOC_PA_SPEED_106GB},
    { NULL, 0 },
};

/* Port Ability interface mapping */
_port_ability_map_t pa_intf_map[] = {
    /* Multi-bit masks should come before their comprised 1-bit masks */
    { "tbi",            SOC_PA_INTF_TBI},
    { "mii",            SOC_PA_INTF_MII},
    { "gmii",           SOC_PA_INTF_GMII},
    { "rgmii",          SOC_PA_INTF_RGMII},
    { "sgmii",          SOC_PA_INTF_SGMII},
    { "xgmii",          SOC_PA_INTF_XGMII},
    { NULL, 0 },
};

/* Port Ability pause mapping */
_port_ability_map_t pa_pause_map[] = {
    /* Multi-bit masks should come before their comprised 1-bit masks */
    { "pause_tx",       SOC_PA_PAUSE_TX},
    { "pause_rx",       SOC_PA_PAUSE_RX},
    { "pause_asymm",    SOC_PA_PAUSE_ASYMM},
    { NULL, 0 },
};

_port_ability_map_t pa_medium_map[] = {
    /* Multi-bit masks should come before their comprised 1-bit masks */
    { "copper",         SOC_PA_MEDIUM_COPPER},
    { "fiber",          SOC_PA_MEDIUM_FIBER},
    { NULL, 0 },
};

_port_ability_map_t pa_lb_map[] = {
    /* Multi-bit masks should come before their comprised 1-bit masks */
    { "none",           SOC_PA_LB_NONE},
    { "MAC",            SOC_PA_LB_MAC},
    { "PHY",            SOC_PA_LB_PHY},
    { "LINE",           SOC_PA_LB_LINE},
    { NULL, 0 },
};

_port_ability_map_t pa_flags_map[] = {
    /* Multi-bit masks should come before their comprised 1-bit masks */
    { "autoneg",        SOC_PA_AUTONEG},
    { "combo",          SOC_PA_COMBO},
    { NULL, 0 },
};


STATIC void
format_port_ability(char *buf, int bufsize, soc_port_mode_t ability,
                    _port_ability_map_t *pa_map)
{
    int                 i, first = 1;

    assert(bufsize >= 80);
    memset(buf, 0, bufsize);

    buf[0] = 0;

    for (i = 0; ability != 0 && pa_map[i].str != NULL; i++) {
        if ((ability & pa_map[i].pa) == pa_map[i].pa) {
            /* Account for final termination null byte + plus one extra ',' */
            assert(strlen(buf) + strlen(pa_map[i].str) + 2 <= bufsize);

            if (first) {
                first = 0;
            } else {
                *buf++ = ',';
            }

            /* Coverity[secure_coding] */
            sal_strcpy(buf, pa_map[i].str);

            while (*buf != 0) {
                buf++;
            }

            ability &= ~pa_map[i].pa;
        }
    }
}

/*
 * format_port_speed_ability
 *
 *   Formats a port speed ability into a comma-separated list of port speed
 *   strings.
 *   Output buffer must be at least 80 characters.
 */

void
format_port_speed_ability(char *buf, int bufsize, soc_port_mode_t ability)
{
    format_port_ability(buf, bufsize, ability, pa_speed_map);
}

/*
 * format_port_intf_ability
 *
 *   Formats a port interface ability into a comma-separated list of port
 *   interface strings.
 *   Output buffer must be at least 80 characters.
 */
void
format_port_intf_ability(char *buf, int bufsize, soc_port_mode_t ability)
{
    format_port_ability(buf, bufsize, ability, pa_intf_map);
}

/*
 * format_port_medium_ability
 *
 *   Formats a port medium ability into a comma-separated list of port
 *   medium strings.
 *   Output buffer must be at least 80 characters.
 */
void
format_port_medium_ability(char *buf, int bufsize, soc_port_mode_t ability)
{
    format_port_ability(buf, bufsize, ability, pa_medium_map);
}

/*
 * format_port_pause_ability
 *
 *   Formats a port pause ability into a comma-separated list of port
 *   pause strings.
 *   Output buffer must be at least 80 characters.
 */
void
format_port_pause_ability(char *buf, int bufsize, soc_port_mode_t ability)
{
    format_port_ability(buf, bufsize, ability, pa_pause_map);
}
/*
 * format_port_lb_ability
 *
 *   Formats a port loopback ability into a comma-separated list of port
 *   loopback strings.
 *   Output buffer must be at least 80 characters.
 */
void
format_port_lb_ability(char *buf, int bufsize, soc_port_mode_t ability)
{
    format_port_ability(buf, bufsize, ability, pa_lb_map);
}
/*
 * format_port_flags_ability
 *
 *   Formats a port flags ability into a comma-separated list of port
 *   flags strings.
 *   Output buffer must be at least 80 characters.
 */
void
format_port_flags_ability(char *buf, int bufsize, soc_port_mode_t ability)
{
    format_port_ability(buf, bufsize, ability, pa_flags_map);
}

/*
 * Function:
 *     parse_field_qualifier
 * Purpose:
 *     Turn text into a bcm_field_qualifier_t for the Field Processor code.
 *
 * Returns:
 *     bcm_field_qualifier_t value that matches the input string
 *        or
 *     BCM_FIELD_ENTRY_INVALID if badly formed string
 */
bcm_field_qualify_t
parse_field_qualifier(char *qual_str)
{
    char                cannonical_str[64];
    char tos1[] = "TOS";
    char tos2[] = "bcmFieldQualifyTos";
    bcm_field_qualify_t qual;

    assert(qual_str != NULL);
    if (sal_strlen(qual_str) >=  64 - 1) {
        return BCM_FIELD_ENTRY_INVALID;
    }

    if (!sal_strcasecmp(tos1, qual_str) ||
        !sal_strcasecmp(tos2, qual_str)) {
        return bcmFieldQualifyDSCP;
    }

    for (qual = 0; qual < bcmFieldQualifyCount; qual++) {
        /* Test for whole name of the qualifier */
        format_field_qualifier(cannonical_str, qual, 0);
        if (!sal_strcasecmp(cannonical_str, qual_str)) {
            break;
        }
        /* Test for the suffix only */
        if(!sal_strcasecmp(cannonical_str + sal_strlen("bcmFieldQualify"),
                           qual_str)) {
            break;
        }
    }

    /* If not found, result is BCM_FIELD_ENTRY_INVALID */
    return qual;
}

/*
 * Function:
 *     format_field_qualifier
 * Purpose:
 *    Format a bcm_field_qualify_t variable for output
 * Parameters:
 *    buf       - character buffer where string is written
 *    qualifier - Field Qualifier value
 *    brief     - if 0, use cannonical form, otherwise use short format
 * Returns:
 *    pointer to buffer for printf-style usage
 */
char *
format_field_qualifier(char *buf, bcm_field_qualify_t qualifier, int brief)
{
    char *qualifier_text[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;

    assert(buf != NULL);

    if (0 <= qualifier && qualifier < bcmFieldQualifyCount) {
        if (brief) {
            sal_sprintf(buf, "%s", qualifier_text[qualifier]);
        } else {
            sal_sprintf(buf, "bcmFieldQualify%s", qualifier_text[qualifier]);
        }
    } else {
        sal_sprintf(buf, "invalid qualifier value=%#x", qualifier);
    }

    return buf;
}

/*
 * Function:
 *     parse_field_qset
 * Purpose:
 *     Turn CLI input into a qset for the Field Processor code.
 */
int
parse_field_qset(char *str, bcm_field_qset_t *qset) {
    char                 *buf;
    char                 *str_copy;
    int                  input_len;
    bcm_field_qualify_t  qual;
    int                  retval = 0;
    char                 delimiters[] = " \t,\n\0{}";
    int                  nval = _SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX);
    int                  idx;
    char                 eight[11], *src_str, *temp_str;
    char                 *tokstr;

    assert(str != NULL);

    BCM_FIELD_QSET_INIT(*qset);

    str_copy = (char *) sal_alloc((BCM_FIELD_QSET_WIDTH_MAX * sizeof(char)),
                                   "qset string");
    if (NULL == str_copy) {
        return (BCM_E_MEMORY);
    }

    /* Parse bitmap input in the form of an Integer. */
    if (isint(str)) {
        /* Skip to the last hex digit in the string */
        for (src_str = str + 1; isxdigit((unsigned) src_str[1]); src_str++) {
            ;
        }

        /* Parse backward in groups of 8 digits */
        idx = 0;
        do {
            /* Copy 8 digits backward to form a string "0xdddddddd\0" */
            temp_str = eight + 11;
            *--temp_str = 0;
            while (temp_str > eight + 2 && *src_str != 'x') {
                *--temp_str = *src_str--;
            }
            *--temp_str = 'x';
            *--temp_str = '0';

            qset->w[idx++] = parse_integer(temp_str);
        } while (*src_str != 'x' && idx < nval);
        sal_free(str_copy);
        return 1;
    }

    /* Parse list of qualifiers. */
    input_len = sal_strlen(str);
    if (input_len >= BCM_FIELD_QSET_WIDTH_MAX) {
        sal_free(str_copy);
        return 0;
    }
    sal_strncpy(str_copy, str, input_len + 1);

    buf = sal_strtok_r(str_copy, delimiters, &tokstr);

    do {
        qual = parse_field_qualifier(buf);
        if (qual != BCM_FIELD_ENTRY_INVALID) {
            BCM_FIELD_QSET_ADD(*qset, qual);
            retval++;
        }
        buf = sal_strtok_r(NULL, delimiters, &tokstr);
    } while (buf);

    sal_free(str_copy);
    return retval;
}

/*
 * Function:
 *     format_field_qset
 * Purpose:
 *    Format a bcm_field_qset_t for output
 * Parameters:
 *    buf       - character buffer where string is written
 *    qset      - Field Qset value
 * Returns:
 *    pointer to buffer (for printf-style usage)
 */
char *
format_field_qset(char *buf, bcm_field_qset_t qset, char *separator)
{
    bcm_field_qualify_t  qual;
    char                 buf_local[30];
    int                  first_print = 1;

    assert(buf != NULL);

    buf[0] = '{';
    buf[1] = '\0';

    /* Iterate over Qset, looking for bits that are set. */
    for (qual = 0; qual < bcmFieldQualifyCount; qual++) {
        if(BCM_FIELD_QSET_TEST(qset, qual)) {
            if (first_print == 1) {
                first_print = 0;
            } else {
                sal_strncat(buf, separator, sal_strlen(separator));
            }
            format_field_qualifier(buf_local, qual, 1);
            sal_strncat(buf, buf_local, sal_strlen(buf_local)+1 );
        }
    }
    sal_strncat(buf, "}", sal_strlen("}")+1);

    return buf;
}

/*
 * Function:
 *     parse_field_group_mode
 * Purpose:
 *     Turn text into a bcm_field_group_mode_t for the Field Processor code.
 *
 * Returns:
 *     bcm_field_group_mode_t value that matches the input string
 *        or
 *     bcmFieldGroupModeCount if badly formed string
 */
bcm_field_group_mode_t
parse_field_group_mode(char *act_str)
{
    char                    cannonical_str[64];
    bcm_field_group_mode_t  mode;

    assert(act_str != NULL);
    if (sal_strlen(act_str) >=  64 - 1) {
        return bcmFieldGroupModeCount;
    }

    for (mode = 0; mode < bcmFieldGroupModeCount; mode++) {
        /* Test for whole name of the mode */
        format_field_group_mode(cannonical_str, mode, 0);
        if (!sal_strcasecmp(cannonical_str, act_str)) {
            break;
        }
        /* Test for the suffix only */
        if(!sal_strcasecmp(cannonical_str + sal_strlen("bcmFieldGroupMode"),
                           act_str)) {
            break;
        }
    }

    /* If not found, result is bcmFieldGroupModeCount */
    return mode;
}

/*
 * Function:
 *     format_field_group_mode
 * Purpose:
 *    Format a bcm_field_group_mode_t variable for output
 * Parameters:
 *    buf       - character buffer where string is written
 *    mode      - Field group_mode value
 *    brief     - if 0, use cannonical form, otherwise use short format
 * Returns:
 *    pointer to buffer for printf-style usage
 */
char *
format_field_group_mode(char *buf, bcm_field_group_mode_t mode, int brief)
{
    char *mode_text[bcmFieldGroupModeCount] = BCM_FIELD_GROUP_MODE_STRINGS;

    assert(buf != NULL);

    if (0 <= mode && mode < bcmFieldGroupModeCount) {
        if (brief) {
            sal_sprintf(buf, "%s", mode_text[mode]);
        } else {
            sal_sprintf(buf, "bcmFieldGroupMode%s", mode_text[mode]);
        }
    } else {
        sal_sprintf(buf, "invalid group mode value=%#x", mode);
    }

    return buf;
}

/*
 * Function:
 *     parse_field_action
 * Purpose:
 *     Turn text into a bcm_field_action_t for the Field Processor code.
 *
 * Returns:
 *     bcm_field_action_t value that matches the input string
 *        or
 *     BCM_FIELD_ENTRY_INVALID if badly formed string
 */
bcm_field_action_t
parse_field_action(char *act_str)
{
    char                cannonical_str[64];
    bcm_field_action_t  action;

    assert(act_str != NULL);
    if (sal_strlen(act_str) >=  64 - 1) {
        return BCM_FIELD_ENTRY_INVALID;
    }

    for (action = 0; action < bcmFieldActionCount; action++) {
        /* Test for whole name of the action */
        format_field_action(cannonical_str, action, 0);
        if (!sal_strcasecmp(cannonical_str, act_str)) {
            break;
        }
        /* Test for the suffix only */
        if(!sal_strcasecmp(cannonical_str + sal_strlen("bcmFieldAction"),
                           act_str)) {
            break;
        }
    }

    /* If not found, result is BCM_FIELD_ENTRY_INVALID */
    return action;
}

/*
 * Function:
 *     format_field_action
 * Purpose:
 *    Format a bcm_field_action_t variable for output
 * Parameters:
 *    buf       - character buffer where string is written
 *    action    - Field action value
 *    brief     - if 0, use cannonical form, otherwise use short format
 * Returns:
 *    pointer to buffer for printf-style usage
 */
static const char *action_text[bcmFieldActionCount] = BCM_FIELD_ACTION_STRINGS;
char *
format_field_action(char *buf, bcm_field_action_t action, int brief)
{
    assert(buf != NULL);

    if (0 <= action && action < bcmFieldActionCount) {
        if (brief) {
            sal_sprintf(buf, "%s", action_text[action]);
        } else {
            sal_sprintf(buf, "bcmFieldAction%s", action_text[action]);
        }
    } else {
        sal_sprintf(buf, "invalid action value=%#x", action);
    }

    return buf;
}

/*
 * Function:
 *     parse_field_aset
 * Purpose:
 *     Turn CLI input into a aset for the Field Processor code.
 */
int
parse_field_aset(char *str, bcm_field_aset_t *aset, uint8 add) {
    char                 *buf;
    char                 *str_copy;
    int                  input_len;
    bcm_field_action_t   action;
    int                  retval = 0;
    char                 delimiters[] = " \t,\n\0{}";
    char                 *tokstr;

    assert(str != NULL);

    str_copy = (char *) sal_alloc((BCM_FIELD_ASET_WIDTH_MAX * sizeof(char)),
                                   "aset string");
    if (NULL == str_copy) {
        return (BCM_E_MEMORY);
    }

    /* Parse list of actions. */
    input_len = sal_strlen(str);
    if (input_len >= BCM_FIELD_ASET_WIDTH_MAX) {
        sal_free(str_copy);
        return 0;
    }
    sal_strncpy(str_copy, str, input_len + 1);

    buf = sal_strtok_r(str_copy, delimiters, &tokstr);

    do {
        action = parse_field_action(buf);
        if (action != BCM_FIELD_ENTRY_INVALID) {
            if (add) {
                BCM_FIELD_ASET_ADD(*aset, action);
            } else {
                BCM_FIELD_ASET_REMOVE(*aset, action);
            }
            retval++;
        }
        buf = sal_strtok_r(NULL, delimiters, &tokstr);
    } while (buf);

    sal_free(str_copy);
    return retval;
}

/*
 * Function:
 *     format_field_aset
 * Purpose:
 *    Format a bcm_field_aset_t for output
 * Parameters:
 *    buf       - character buffer where string is written
 *    aset      - Field Aset value
 * Returns:
 *    pointer to buffer (for printf-style usage)
 */
char *
format_field_aset(char *buf, bcm_field_aset_t aset, char *separator)
{
    bcm_field_action_t   action;
    char                 buf_local[30];
    int                  first_print = 1;

    assert(buf != NULL);

    buf[0] = '{';
    buf[1] = '\0';

    /* Iterate over Aset, looking for bits that are set. */
    for (action = 0; action < bcmFieldActionCount; action++) {
        if(BCM_FIELD_ASET_TEST(aset, action)) {
            if (first_print == 1) {
                first_print = 0;
            } else {
                sal_strncat(buf, separator, sal_strlen(separator));
            }
            format_field_action(buf_local, action, 1);
            sal_strncat(buf, buf_local, sal_strlen(buf_local));
        }
    }
    sal_strncat(buf, "}", sal_strlen("}")+1);

    return buf;
}


#define FP_OLP_HDR_TYPE_STR_SZ 50
/*
 * Function:
 *    parse_field_olp_hdr_type
 * Purpose:
 *     Turn CLI input into a bcm_field_oam_type_t for the Field Processor code.
 *
 * Returns:
 *     bcm_field_olp_header_type_t value that matches the input string
 *        or
 *     bcmFieldOlpHeaderTypeCount if badly formed string
 */
bcm_field_olp_header_type_t parse_field_olp_hdr_type(char *str)
{
    bcm_field_olp_header_type_t olp_hdr_type;
    char *olp_hdr_type_text[bcmFieldOlpHeaderTypeCount] = BCM_FIELD_OLP_HEADER_TYPE_STRINGS;
    char                tbl_str[FP_OLP_HDR_TYPE_STR_SZ];
    char                lng_str[FP_OLP_HDR_TYPE_STR_SZ];

    if (str == NULL) {
        return bcmFieldOlpHeaderTypeCount;
    }

    if (isint(str)) {
        return parse_integer(str);
    }

    for (olp_hdr_type = 0; olp_hdr_type < bcmFieldOlpHeaderTypeCount; olp_hdr_type++) {
        sal_memset(tbl_str, 0, sizeof(char) * FP_OLP_HDR_TYPE_STR_SZ);
        sal_memset(lng_str, 0, sizeof(char) * FP_OLP_HDR_TYPE_STR_SZ);
        /* Test for the suffix only */
        sal_strncpy(tbl_str, olp_hdr_type_text[olp_hdr_type], FP_OLP_HDR_TYPE_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, str)) {
            break;
        }
        /* Test for whole name of the action */
        sal_strncpy(lng_str, "bcmFieldOlpHeaderType", sizeof (lng_str));
        sal_strncat_s(lng_str, tbl_str, sizeof(lng_str));
        if (!sal_strcasecmp(lng_str, str)) {
            break;
        }
    }
    return olp_hdr_type;
}

#define FP_OAMTYPE_STR_SZ 50
/*
 * Function:
 *    parse_field_oam_type
 * Purpose:
 *     Turn CLI input into a bcm_field_oam_type_t for the Field Processor code.
 *
 * Returns:
 *     bcm_field_oam_type_t value that matches the input string
 *        or
 *     bcmFieldOamTypeCount if badly formed string
 */
bcm_field_oam_type_t
parse_field_oam_type(char *str)
{
    bcm_field_oam_type_t   oamtype;
    char *oamtype_text[bcmFieldOamTypeCount] = BCM_FIELD_OAM_TYPE;
    char                tbl_str[FP_OAMTYPE_STR_SZ];
    char                lng_str[FP_OAMTYPE_STR_SZ];

    if (str == NULL) {
        return bcmFieldOamTypeCount;
    }

    if (isint(str)) {
        return parse_integer(str);
    }

    for (oamtype = 0; oamtype < bcmFieldOamTypeCount; oamtype++) {
        sal_memset(tbl_str, 0, sizeof(char) * FP_OAMTYPE_STR_SZ);
        sal_memset(lng_str, 0, sizeof(char) * FP_OAMTYPE_STR_SZ);
        /* Test for the suffix only */
        sal_strncpy(tbl_str, oamtype_text[oamtype], FP_OAMTYPE_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, str)) {
            break;
        }
        /* Test for whole name of the action */
        sal_strncpy(lng_str, "bcmFieldOamType", sizeof (lng_str));
        sal_strncat_s(lng_str, tbl_str, sizeof (lng_str));
        if (!sal_strcasecmp(lng_str, str)) {
            break;
        }
    }
    return oamtype;
}


#define FP_DECAP_STR_SZ 50
/*
 * Function:
 *     parse_field_decap
 * Purpose:
 *     Turn CLI input into a bcm_field_decap_t for the Field Processor code.
 *
 * Returns:
 *     bcm_field_decap_t value that matches the input string
 *        or
 *     bcmFieldDecapCount if badly formed string
 */
bcm_field_decap_t
parse_field_decap(char *str)
{
    bcm_field_decap_t   decap;
    char *decap_text[bcmFieldDecapCount] = BCM_FIELD_DECAP_STRINGS;
    char                tbl_str[FP_DECAP_STR_SZ];
    char                lng_str[FP_DECAP_STR_SZ];

    if (str == NULL) {
        return bcmFieldDecapCount;
    }

    if (isint(str)) {
        return parse_integer(str);
    }

    for (decap = 0; decap < bcmFieldDecapCount; decap++) {
        sal_memset(tbl_str, 0, sizeof(char) * FP_DECAP_STR_SZ);
        sal_memset(lng_str, 0, sizeof(char) * FP_DECAP_STR_SZ);
        /* Test for the suffix only */
        sal_strncpy(tbl_str, decap_text[decap], FP_DECAP_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, str)) {
            break;
        }
        /* Test for whole name of the action */
        /* Coverity[secure_coding] */
        sal_strcpy(lng_str, "bcmFieldDecap");
        sal_strncat_s(lng_str, tbl_str, sizeof(lng_str));
        if (!sal_strcasecmp(lng_str, str)) {
            break;
        }
    }
    return decap;
}


#define FP_MPLS_OAM_CTRL_PKT_TYPE_STR_SZ 50
/*
 * Function:
 *    parse_field_MplsOamCtrlPktType
 * Purpose:
 *     Turn CLI input into a bcm_field_MplsOam_Control_pktType_t
       for the Field Processor code.
 *
 * Returns:
 *     bcm_field_MplsOam_Control_pktType_t value that matches the input string
 *        or
 *      bcmFieldMplsOamControlPktTypeCount if badly formed string
 */
bcm_field_MplsOam_Control_pktType_t
parse_field_MplsOamCtrlPktType (char *str)
{
    bcm_field_MplsOam_Control_pktType_t pktType;
    char *MplsOamCtrlPktType[bcmFieldMplsOamControlPktTypeCount]
                       = BCM_FIELD_MPLS_OAM_CONTROL_PKT_TYPE;
    char                tbl_str[FP_MPLS_OAM_CTRL_PKT_TYPE_STR_SZ];
    char                lng_str[FP_MPLS_OAM_CTRL_PKT_TYPE_STR_SZ];

    if (str == NULL) {
        return bcmFieldMplsOamControlPktTypeCount;
    }

    if (isint(str)) {
        return parse_integer(str);
    }

    for (pktType = 0; pktType < bcmFieldMplsOamControlPktTypeCount; pktType++) {
        sal_memset(tbl_str, 0, sizeof(char) * FP_MPLS_OAM_CTRL_PKT_TYPE_STR_SZ);
        sal_memset(lng_str, 0, sizeof(char) * FP_MPLS_OAM_CTRL_PKT_TYPE_STR_SZ);
        /* Test for the suffix only */
        sal_strncpy(tbl_str, MplsOamCtrlPktType[pktType],
                FP_MPLS_OAM_CTRL_PKT_TYPE_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, str)) {
            break;
        }
        /* Test for whole name of the action */
        sal_strncpy(lng_str, "bcmFieldMplsOamControlPktType", sizeof (lng_str));
        sal_strncat_s(lng_str, tbl_str, sizeof (lng_str));
        if (!sal_strcasecmp(lng_str, str)) {
            break;
        }
    }
    return pktType;
}

#define FP_ROE_FRAME_TYPE_STR_SZ 40
/*
 * Function:
 *    parse_field_roe_frame_type
 * Purpose:
 *     Turn CLI input into a bcm_field_roe_frame_type_t for the Field Processor code.
 *
 * Returns:
 *     bcm_field_roe_frame_type_t value that matches the input string
 *        or
 *     bcmFieldRoeFrameTypeCount if badly formed string
 */
bcm_field_roe_frame_type_t parse_field_roe_frame_type(char *str)
{
    bcm_field_roe_frame_type_t roe_frame_type;
    char *roe_frame_type_text[bcmFieldRoeFrameTypeCount] = BCM_FIELD_ROE_FRAME_TYPE_STRINGS;
    char                tbl_str[FP_ROE_FRAME_TYPE_STR_SZ];
    char                lng_str[FP_ROE_FRAME_TYPE_STR_SZ];

    if (str == NULL) {
        return bcmFieldRoeFrameTypeCount;
    }

    if (isint(str)) {
        return parse_integer(str);
    }

    for (roe_frame_type = 0; roe_frame_type < bcmFieldRoeFrameTypeCount; roe_frame_type++) {
        sal_memset(tbl_str, 0, sizeof(char) * FP_ROE_FRAME_TYPE_STR_SZ);
        sal_memset(lng_str, 0, sizeof(char) * FP_ROE_FRAME_TYPE_STR_SZ);
        /* Test for the suffix only */
        sal_strncpy(tbl_str, roe_frame_type_text[roe_frame_type],
                    FP_ROE_FRAME_TYPE_STR_SZ - 1);
        if (!sal_strcasecmp(tbl_str, str)) {
            break;
        }
        /* Test for whole name of the action */
        sal_strncpy(lng_str, "bcmFieldRoeFrameType", sizeof (lng_str));
        sal_strncat_s(lng_str, tbl_str, sizeof (lng_str));
        if (!sal_strcasecmp(lng_str, str)) {
            break;
        }
    }
    return roe_frame_type;
}

/*
 * Function:
 *     format_field_decap
 * Purpose:
 *    Format a bcm_field_decap_t variable for output.
 * Parameters:
 *    buf       - character buffer where string is written
 *    decap     - Field decap value
 * Returns:
 *    pointer to buffer for printf-style usage
 */
char *
format_field_decap(char *buf, bcm_field_decap_t decap)
{
    char *decap_text[bcmFieldDecapCount] = BCM_FIELD_DECAP_STRINGS;

    assert(buf != NULL);

    if (0 <= decap && decap < bcmFieldDecapCount) {
        sal_sprintf(buf, "bcmFieldDecap%s", decap_text[decap]);
    } else {
        sal_sprintf(buf, "invalid decap value=%#x", decap);
    }

    return buf;
}

/*
 * Routine to load a big-endian value from a packet with no alignment
 * restrictions.  Size is the length of the value in bytes (0-4).
 */

uint32
packet_load(uint8 *addr, int size)
{
    uint32              val = 0;

    switch (size) {
    default:
        val |= (uint32) *addr++ << 24;
        /* Fall through */
    case 3:
        val |= (uint32) *addr++ << 16;
    case 2:
        val |= (uint32) *addr++ << 8;
    case 1:
        val |= (uint32) *addr++ << 0;
    case 0:
        ;
    }

    return val;
}
/*
 * Random Pattern Packet Filler
 *
 * Fills a buffer of bytes with a random pattern.
 * There are no alignment restrictions on the buffer and length.
 */

void
packet_random_store(uint8 *buf, int size)
{
    int                 align_off = ((sal_vaddr_t) buf & 3);
    int                 pat_off = 24;
    uint32              pat;

    pat = sal_rand();

    if (align_off) {
        /* Store bytes from pat (MSB ==> LSB) until buf is aligned. */

        for (; ((sal_vaddr_t) buf & 3) != 0 && size > 0; pat_off -= 8, size--) {
            *buf++ = pat >> pat_off;
        }
    }

    /* Store whole words. */

    for (; size > 3; buf += 4, size -= 4) {
        pat = sal_rand();
        *(uint32 *) buf = bcm_htonl(pat);
    }
    /* Store up to 3 residual bytes. */

    pat = sal_rand();
    for (; size > 0; size--, pat_off -= 8) {
        *buf++ = pat >> pat_off;
        if (pat_off == 0) {
            pat = sal_rand();
            pat_off = 24;
        }
    }
}

/*
 * Optimized Packet Filler
 *
 * Fills a buffer of bytes with a 32-bit incrementing pattern in
 * big-endian order.  There are no alignment restrictions on the buffer
 * and length.
 *
 * The increment value is added to the pattern each time it is stored
 * (every 4 bytes).  The final value of the pattern is returned (after
 * all the incrementing).
 */

uint32
packet_store(uint8 *buf, int size, uint32 pat, uint32 inc)
{
    int                 align_off = ((sal_vaddr_t) buf & 3);
    int                 pat_off = 24;

    if (align_off) {
        int             align_shr = align_off * 8;
        int             align_shl = 32 - align_shr;

        /* Store bytes from pat (MSB ==> LSB) until buf is aligned. */

        for (; ((sal_vaddr_t)buf & 3) != 0 && size > 0; pat_off -= 8, size--) {
            *buf++ = pat >> pat_off;
        }

        /* Store whole words, incrementing pat in the middle of words. */

        for (; size > 3; buf += 4, size -= 4) {
            uint32 x = pat << align_shl;
            pat += inc;
            x |= pat >> align_shr;
            *(uint32 *)buf = bcm_htonl(x);
        }
    } else {
        for (; size > 3; buf += 4, size -= 4, pat += inc) {
            *(uint32 *)buf = bcm_htonl(pat);
        }
    }

    /* Store up to 3 residual bytes. */

    for (; size > 0; size--, pat_off -= 8) {
        *buf++ = pat >> pat_off;
        if (pat_off == 0) {
            pat += inc;         /* Add increment and wrap around to MSB */
            pat_off = 32;
        }
    }

    return pat;
}

/*
 * Optimized packet compare
 *
 * Returns byte offset of first mismatch, -1 for compare equal.
 */

int
packet_compare(uint8 *p1, uint8 *p2, int size)
{
    int                 i = 0;

    if ((((sal_vaddr_t) p1 ^ (sal_vaddr_t) p2) & 3) != 0) {
        goto residual;
    }

    for (; ((sal_vaddr_t) p1 & 3) != 0 && i < size; i++) {
        if (p1[i] != p2[i]) {
            return i;
        }
    }

    for (; i < size - 3; i += 4) {
        if (*(uint32 *) &p1[i] != *(uint32 *) &p2[i]) {
            break;
        }
    }

 residual:

    for (; i < size; i++) {
        if (p1[i] != p2[i]) {
            return i;
        }
    }

    return -1;
}

static struct {
    char                *str;
    soc_phy_control_longreach_ability_t     pa;
} lr_pa_map[] = {
    /* Multi-bit masks should come before their comprised 1-bit masks */
    { "10x1",      SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_1PAIR },
    { "10x2",      SOC_PHY_CONTROL_LONGREACH_ABILITY_10M_2PAIR },
    { "20x1",      SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_1PAIR },
    { "20x2",      SOC_PHY_CONTROL_LONGREACH_ABILITY_20M_2PAIR },
    { "25x1",      SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_1PAIR },
    { "25x2",      SOC_PHY_CONTROL_LONGREACH_ABILITY_25M_2PAIR },
    { "33x1",      SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_1PAIR },
    { "33x2",      SOC_PHY_CONTROL_LONGREACH_ABILITY_33M_2PAIR },
    { "50x1",      SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_1PAIR },
    { "50x2",      SOC_PHY_CONTROL_LONGREACH_ABILITY_50M_2PAIR },
    { "100x1",     SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_1PAIR },
    { "100x2",     SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_2PAIR },
    { "100x4",     SOC_PHY_CONTROL_LONGREACH_ABILITY_100M_4PAIR },
    { "pause_rx",  SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_RX },
    { "pause_tx",  SOC_PHY_CONTROL_LONGREACH_ABILITY_PAUSE_TX },
    { NULL, 0 },
};

int
parse_phy_control_longreach_ability(char *str, soc_phy_control_longreach_ability_t *ability)
{
    int                 i;
    char                *s = str;

    *ability = 0;

    while (*s != 0) {
        for (i = 0; lr_pa_map[i].str != NULL; i++) {
            int len = sal_strlen(lr_pa_map[i].str);

            if (sal_strncasecmp(s, lr_pa_map[i].str, len) == 0 &&
                (s[len] == 0 || s[len] == ',')) {

                *ability |= lr_pa_map[i].pa;

                if (*(s += len) == ',') {
                    s++;
                }

                break;
            }
        }

        if (lr_pa_map[i].str == NULL) {
            return -1;
        }
    }

    return 0;
}

void
format_phy_control_longreach_ability(char *buf, int bufsize, soc_phy_control_longreach_ability_t ability)
{
    int                 i, first = 1;

    assert(bufsize >= 80);

    buf[0] = 0;

    for (i = 0; ability != 0 && lr_pa_map[i].str != NULL; i++) {
        if ((ability & lr_pa_map[i].pa) == lr_pa_map[i].pa) {
            if (first) {
                first = 0;
            } else {
                *buf++ = ',';
            }

            sal_strncpy(buf, lr_pa_map[i].str, bufsize);

            while (*buf != 0) {
                buf++;
            }

            ability &= ~lr_pa_map[i].pa;
        }
    }

}

pbmp_t
soc_property_get_bcm_pbmp(int unit, const char *name, int defneg)
{
    pbmp_t pbmp = soc_property_get_pbmp(unit, name, defneg);

    BCM_API_XLATE_PORT_PBMP_P2A(unit, &pbmp);

    return pbmp;
}

uint32
soc_property_bcm_port_get(int unit, bcm_port_t aport,
                          const char *name, uint32 defl)
{
    soc_port_t port = aport;

    BCM_API_XLATE_PORT_A2P(unit, &port);

    return soc_property_port_get(unit, port, name, defl);
}


int soc_phy_fw_acquire_default(const char *file_name, uint8 **fw, int *fw_len)
{
#ifndef  NO_FILEIO
    FILE *fp;
    uint8 *buffer;
    int buffer_size, offset;

    if ((fp = sal_fopen((char *)file_name, "rb")) == NULL) {
        return SOC_E_FAIL;
    }

    buffer_size = sal_fsize(fp);

    /* some implementations of sal_fsize cannot move the file pointer back to the begining */
    sal_fclose(fp);
    if ((fp = sal_fopen((char *)file_name, "rb")) == NULL) {
        return SOC_E_FAIL;
    }

    if (buffer_size == -1) {
    /* cannot determine the f/w file size */
        buffer_size = MAX_FW_BUF_LEN;
    }

    buffer = sal_alloc(buffer_size,"firmware_buffer");
    if (buffer == NULL) {
        sal_fclose(fp);
        cli_out("ERROR: Can't allocate enough buffer : 0x%x\n",
                buffer_size);
        return SOC_E_FAIL;
    }

    /* coverity[tainted_data_argument] */
    offset = sal_fread(buffer, 1, buffer_size, fp);
    if (sal_ferror(fp)) {
        cli_out("ERROR: Can't read from file : %s\n", file_name);
        /* coverity[tainted_data] */
        sal_free(buffer);
        sal_fclose(fp);
        return SOC_E_FAIL;
    }

    sal_fclose(fp);

    *fw = buffer;
    *fw_len = offset;

    return SOC_E_NONE;
#else
    return SOC_E_FAIL;
#endif

}

int soc_phy_fw_release_default(const char *dev_name, uint8 *fw, int fw_len)
{
    if (fw) {
        sal_free(fw);
    }
    return SOC_E_NONE;
}


#if defined(BROADCOM_DEBUG)

#define CHK_FLAG(pkt, flag, offset, line, desc) \
    do { \
        if ((pkt)->flags & (flag)) { \
            sal_sprintf(&(line)[offset], desc); \
            (offset) = sal_strlen(line); \
        } \
    } while (0)

STATIC char *_rx_pkt_dump_reason_names[bcmRxReasonCount] =
                                        BCM_RX_REASON_NAMES_INITIALIZER;

void
bcm_pkt_dump(int unit, bcm_pkt_t *pkt, int dump_data)
{
    char pbm_str[SOC_PBMP_FMT_LEN],
        upbm_str[SOC_PBMP_FMT_LEN],
        l3pbm_str[SOC_PBMP_FMT_LEN];
    char line[80];
    int i;
    int nl = 1;
    int offset = 0;

    COMPILER_REFERENCE(unit);

    cli_out("Packet dump:  %p.  Data in %d blocks.\n", (void *)pkt,
            pkt->blk_count);
    for (i = 0; i < pkt->blk_count; i++) {
        sal_sprintf(&line[offset], "%s(0x%p, %d)", nl ? "" : ", ",
                    (void *)pkt->pkt_data[i].data, pkt->pkt_data[i].len);
        offset = sal_strlen(line);
        if (!((i+1) % 4)) {
            cli_out("    %s\n", line);
            nl = 1;
            offset = 0;
        } else {
            nl = 0;
        }
    }

    if (!nl) {
        cli_out("    %s\n", line);
    }
    line[0] = '\0';

    cli_out("  unit %d. %s %d. sm %d. dp %d. dm %d. cos %d. "
            "prio_int %d.\n",
            pkt->unit,  (pkt->flags & BCM_PKT_F_TRUNK) ? "st" : "sp",
            (pkt->flags & BCM_PKT_F_TRUNK) ? pkt->src_trunk : pkt->src_port,
            pkt->src_mod, pkt->dest_port, pkt->dest_mod, pkt->cos, pkt->prio_int);
    cli_out("  opcode 0x%x. base_len %d. tot_len %d.\n",
            pkt->opcode, pkt->pkt_len, pkt->tot_len);
    cli_out("  tx pbm %s. upbm %s. l3pbm %s\n",
            SOC_PBMP_FMT(pkt->tx_pbmp, pbm_str),
            SOC_PBMP_FMT(pkt->tx_upbmp, upbm_str),
            SOC_PBMP_FMT(pkt->tx_l3pbmp, l3pbm_str));
    cli_out("  rx_reason 0x%x. rx_unit %d."
            " rx_port %d. rx_cpu_cos %d. %s.\n",
            pkt->rx_reason, pkt->rx_unit, pkt->rx_port, pkt->rx_cpu_cos,
            (pkt->rx_untagged & BCM_PKT_OUTER_UNTAGGED) ?
            ((pkt->rx_untagged & BCM_PKT_INNER_UNTAGGED) ?
            "Untagged" : "Inner tagged") :
            ((pkt->rx_untagged & BCM_PKT_INNER_UNTAGGED) ?
            "Outer tagged" : "Double tagged"));

    cli_out("  matched %d. classification-tag %d. timestamp %d.\n",
            pkt->rx_matched,
            pkt->rx_classification_tag,
            pkt->rx_timestamp);
    for (i = 0; i < bcmRxReasonCount; i++) {
        if (BCM_RX_REASON_GET(pkt->rx_reasons, i)) {
            cli_out("  reasons: %s\n", _rx_pkt_dump_reason_names[i]);
        }
    }

    offset = 0;
    CHK_FLAG(pkt, BCM_PKT_F_HGHDR, offset, line, "hg_hdr ");
    CHK_FLAG(pkt, BCM_PKT_F_SLTAG, offset, line, "sl_tag ");
    CHK_FLAG(pkt, BCM_PKT_F_NO_VTAG, offset, line, "no_vtag ");
    cli_out("  flags-->%s\n", line);

    offset = 0;
    CHK_FLAG(pkt, BCM_TX_CRC_ALLOC, offset, line, "crc_alloc ");
    CHK_FLAG(pkt, BCM_TX_CRC_REGEN, offset, line, "crc_regen ");
    CHK_FLAG(pkt, BCM_TX_CRC_FORCE_ERROR, offset, line,
             "crc_force_error ");
    cli_out("  tx flags-->%s\n", line);

    offset = 0;
    CHK_FLAG(pkt, BCM_RX_CRC_STRIP, offset, line, "crc_strip ");
    CHK_FLAG(pkt, BCM_PKT_F_NO_VTAG, offset, line, "vtag_strip ");
    cli_out("  rx flags-->%s\n", line);

    cli_out("  dma chan %d. adr pkt_data %p. idx %d. dv %p\n",
            pkt->dma_channel, (void *)&pkt->_pkt_data, pkt->_idx, pkt->_dv);
    {
        uint32 *pkt_higig_ptr;
        uint32 *pkt_sltag_ptr;
        uint32 *pkt_vtag_ptr;

        pkt_higig_ptr = (uint32 *)((void *)(pkt->_higig));
        pkt_sltag_ptr = (uint32 *)((void *)(pkt->_sltag));
        pkt_vtag_ptr = (uint32 *)((void *)(pkt->_vtag));
        cli_out("  hghdr: 0x%08x%08x%08x. sltag 0x%x. vtag 0x%x\n",
            pkt_higig_ptr[0], pkt_higig_ptr[1],
            pkt_higig_ptr[2], pkt_sltag_ptr[0],
            pkt_vtag_ptr[0]);
    }
    if (dump_data) {
        int blk, byte = 0, tot_byte = 0;

        offset = 0;
        for (blk = 0; blk < pkt->blk_count; blk++) {
            for (byte = 0; byte < pkt->pkt_data[blk].len; byte++) {
                ++tot_byte;
                sal_sprintf(&line[offset], "%02x%s",
                            pkt->pkt_data[blk].data[byte],
                            !(tot_byte % 4) ? " " : "");
                offset = sal_strlen(line);
                if (!(tot_byte % 16)) {
                    cli_out("data[%04d]:  %s\n", tot_byte - 16, line);
                    offset = 0;
                }
                if (tot_byte >= pkt->pkt_len) { /* Displayed all of packet */
                    goto loop_exit;
                }
            }
        }

    loop_exit:
        if (tot_byte % 16) {
            cli_out("data[%04d]:  %s\n", 16 * (tot_byte / 16), line);
        }
    }
}

#endif /* BROADCOM_DEBUG */
