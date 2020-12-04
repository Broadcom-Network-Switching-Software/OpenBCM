/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Driver utility routines
 */

#include <assert.h>
#include <soc/enet.h>
#include <soc/util.h>
#include <soc/error.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>

static fw_desc_t fw_desc[MAX_FW_TYPES];
static misc_desc_t misc_desc[_MAX_PHYS];

int (*soc_phy_fw_acquire)(const char *dev_name, uint8 **fw, int *fw_len) = NULL;
int (*soc_phy_fw_release)(const char *dev_name, uint8 *fw, int fw_len) = NULL;
int (*soc_phy_misc_launch)(const char *dev_name, void *arg) = NULL;


/*
 * soc_timeout
 *
 *   These routines implement a polling timer that, in the normal case,
 *   has low overhead, but provides reasonably accurate timeouts for
 *   intervals longer than a millisecond.
 *
 *   min_polls should be chosen so the operation is expected to complete
 *   within min_polls, if possible.  If the operation completes within
 *   min_polls, there is very little overhead.  Otherwise, the routine
 *   starts making O/S calls to check the real time clock and uses an
 *   exponential timeout to avoid hogging the CPU.
 *
 *   Example usage:
 *
 *	soc_timeout_t		to;
 *	sal_usecs_t		timeout_usec = 100000;
 *	int			min_polls = 100;
 *
 *	soc_timeout_init(&to, timeout_usec, min_polls);
 *
 *	while (check_status(thing) != DONE)
 *		if (soc_timeout_check(&to)) {
 *              if (check_status(thing) == DONE) {
 *                  break;
 *              }
 *			printf("Operation timed out\n");
 *			return ERROR;
 *		}
 *
 *   Note that even after timeout the status should be checked
 *   one more time.  Otherwise there is a race condition where an
 *   ill-placed O/S task reschedule could cause a false timeout.
 */

void
soc_timeout_init(soc_timeout_t *to, sal_usecs_t usec, int min_polls)
{
    to->min_polls = min_polls;
    to->usec = usec;
    to->polls = 1;
    to->exp_delay = 1;   /* In case caller sets min_polls < 0 */
}

int
soc_timeout_check(soc_timeout_t *to)
{
    if (++to->polls >= to->min_polls) {
	if (to->min_polls >= 0) {
	    /*
	     * Just exceeded min_polls; calculate expiration time by
	     * consulting O/S real time clock.
	     */

	    to->min_polls = -1;
	    to->expire = SAL_USECS_ADD(sal_time_usecs(), to->usec);
	    to->exp_delay = 1;
	} else {
	    /*
	     * Exceeded min_polls in a previous call.
	     * Consult O/S real time clock to check for expiration.
	     */

	    if (SAL_USECS_SUB(sal_time_usecs(), to->expire) >= 0) {
		return 1;
	    }

	    sal_usleep(to->exp_delay);

	    /* Exponential backoff with 10% maximum latency */

	    if ((to->exp_delay *= 2) > to->usec / 10) {
		to->exp_delay = to->usec / 10;
	    }
	}
    }

    return 0;
}
int
soc_tightdelay_timeout_check(soc_timeout_t *to)
{
    if (++to->polls >= to->min_polls) {
	if (to->min_polls >= 0) {
	    /*
	     * Just exceeded min_polls; calculate expiration time by
	     * consulting O/S real time clock.
	     */

	    to->min_polls = -1;
	    to->expire = SAL_USECS_ADD(sal_time_usecs(), to->usec);
	    to->exp_delay = 1;
	}
        else if (to->expire < SOC_TIGHTLOOP_DELAY_LIMIT_USECS) {
	    /*
	     * Exceeded min_polls in a previous call.
	     * Consult O/S real time clock to check for expiration.
	     */

	    if (SAL_USECS_SUB(sal_time_usecs(), to->expire) >= 0) {
		return 1;
	    }

	    sal_udelay(to->exp_delay);

	    /* Exponential backoff with 10% maximum latency */

	    if ((to->exp_delay *= 2) > to->usec / 10) {
		to->exp_delay = to->usec / 10;
	    }
	}
        else {
	    /*
	     * Exceeded min_polls in a previous call.
	     * Consult O/S real time clock to check for expiration.
	     */

	    if (SAL_USECS_SUB(sal_time_usecs(), to->expire) >= 0) {
		return 1;
	    }

	    sal_usleep(to->exp_delay);

	    /* Exponential backoff with 10% maximum latency */

	    if ((to->exp_delay *= 2) > to->usec / 10) {
		to->exp_delay = to->usec / 10;
	    }
	}
    }

    return 0;
}

sal_usecs_t
soc_timeout_elapsed(soc_timeout_t *to)
{
    sal_usecs_t		start_time;

    start_time = SAL_USECS_SUB(to->expire, to->usec);

    return SAL_USECS_SUB(sal_time_usecs(), start_time);
}


/*
 * Function:
 *	soc_ntohl_load
 * Purpose:
 *	Load a 32-bit value and convert from network to host byte order.
 * Parameters:
 *	a - Address to load from
 * Returns:
 *	32-bit value.
 * Notes:
 *	Unaligned addresses are handled (even if no swap is needed).
 */
uint32
soc_ntohl_load(const void *a)
{
    uint32	v;

    v =  ((uint8 *)a)[0] << 24;
    v |= ((uint8 *)a)[1] << 16;
    v |= ((uint8 *)a)[2] << 8;
    v |= ((uint8 *)a)[3] << 0;

    return(v);
}

/*
 * Function:
 *	soc_ntohs_load
 * Purpose:
 *	Load a 16-bit value and convert from network to host byte order.
 * Parameters:
 *	a - Address to load from
 * Returns:
 *	16-bit value.
 * Notes:
 *	Unaligned addresses are handled (even if no swap is needed).
 */
uint16
soc_ntohs_load(const void *a)
{
    uint16	v;

    v =  ((uint8 *)a)[0] << 8;
    v |= ((uint8 *)a)[1] << 0;

    return(v);
}

/*
 * Function:
 *	soc_htonl_store
 * Purpose:
 *	Convert a 32-bit value from host to network byte order and store.
 * Parameters:
 *	a - Address to store to
 *	v - 32-bit value to store
 * Returns:
 *	Original value of v
 * Notes:
 *	Unaligned addresses are handled (even if no swap is needed).
 */
uint32
soc_htonl_store(void *a, uint32 v)
{
    ((uint8 *)a)[0] = v >> 24;
    ((uint8 *)a)[1] = v >> 16;
    ((uint8 *)a)[2] = v >> 8;
    ((uint8 *)a)[3] = v >> 0;

    return(v);
}

/*
 * Function:
 *	soc_htons_store
 * Purpose:
 *	Convert a 16-bit value from host to network byte order and store.
 * Parameters:
 *	a - Address to store to
 *	v - 16-bit value to store
 * Returns:
 *	Original value of v
 * Notes:
 *	Unaligned addresses are handled (even if no swap is needed).
 */
uint16
soc_htons_store(void *a, uint16 v)
{
    ((uint8 *)a)[0] = v >> 8;
    ((uint8 *)a)[1] = v >> 0;

    return(v);
}

/*
 * Function:
 *	soc_letohl_load
 * Purpose:
 *	Load a 32-bit value and convert from little-endian to host byte order.
 * Parameters:
 *	a - Address to load from
 * Returns:
 *	32-bit value.
 * Notes:
 *	Unaligned addresses are handled (even if no swap is needed).
 */
uint32
soc_letohl_load(const void *a)
{
    uint32	v;

    v =  ((uint8 *)a)[3] << 24;
    v |= ((uint8 *)a)[2] << 16;
    v |= ((uint8 *)a)[1] << 8;
    v |= ((uint8 *)a)[0] << 0;

    return(v);
}

/*
 * Function:
 *	soc_letohs_load
 * Purpose:
 *	Load a 16-bit value and convert from little-endian to host byte order.
 * Parameters:
 *	a - Address to load from
 * Returns:
 *	16-bit value.
 * Notes:
 *	Unaligned addresses are handled (even if no swap is needed).
 */
uint16
soc_letohs_load(const void *a)
{
    uint16	v;

    v =  ((uint8 *)a)[1] << 8;
    v |= ((uint8 *)a)[0] << 0;

    return(v);
}

/*
 * Function:
 *	soc_htolel_store
 * Purpose:
 *	Convert a 32-bit value from host to little-endian byte order and store.
 * Parameters:
 *	a - Address to store to
 *	v - 32-bit value to store
 * Returns:
 *	Original value of v
 * Notes:
 *	Unaligned addresses are handled (even if no swap is needed).
 */
uint32
soc_htolel_store(void *a, uint32 v)
{
    ((uint8 *)a)[3] = v >> 24;
    ((uint8 *)a)[2] = v >> 16;
    ((uint8 *)a)[1] = v >> 8;
    ((uint8 *)a)[0] = v >> 0;

    return(v);
}

/*
 * Function:
 *	soc_htoles_store
 * Purpose:
 *	Convert a 16-bit value from host to little-endian byte order and store.
 * Parameters:
 *	a - Address to store to
 *	v - 16-bit value to store
 * Returns:
 *	Original value of v
 * Notes:
 *	Unaligned addresses are handled (even if no swap is needed).
 */
uint16
soc_htoles_store(void *a, uint16 v)
{
    ((uint8 *)a)[1] = v >> 8;
    ((uint8 *)a)[0] = v >> 0;

    return(v);
}

/* Helper routine to retreive particular bits from data */
void
soc_bits_get(uint32 *str, uint32 minbit, uint32 maxbit, void *data_vp)
{
    int len, str_index, data_index, right_shift_count, left_shift_count;
    uint32 *data = data_vp;

    len = maxbit - minbit + 1;
    str_index = minbit >> 5;
    data_index = 0;
    right_shift_count = minbit & 0x1f;
    left_shift_count = 32 - right_shift_count;

    if (right_shift_count) {
        for (; len > 0; len -= 32) {
            data[data_index] = str[str_index++] >> right_shift_count;
            data[data_index++] |= str[str_index] << left_shift_count;
        }
    } else {
        for (; len > 0; len -= 32) {
            data[data_index++] = str[str_index++];
        }
    }
    if (len & 0x1f) {
        data[data_index - 1] &= (1 << (len & 0x1f)) - 1;
    }
}

void soc_phy_fw_init(void)
{
    int i = 0;

    for (i = 0; i < MAX_FW_TYPES; i++) {
        fw_desc[i].dev_name = NULL;
        fw_desc[i].fw = NULL;
        fw_desc[i].fw_len = 0;
    }

}

void soc_phy_misc_init(void)
{
    int i = 0;

    for (i = 0; i < _MAX_PHYS; i++) {
        misc_desc[i].dev_name = NULL;
        misc_desc[i].arg = NULL;
    }

}

int soc_phy_fw_get(char *dev_name, uint8 **fw, int *fw_len)
{
    int i = 0;

    while(i < MAX_FW_TYPES) {
        if (fw_desc[i].fw == NULL) {
            /* empty slot */
            break;
        }
        if ( !sal_strcmp(dev_name, fw_desc[i].dev_name) ) {
            if (fw_desc[i].fw == NO_FW) {
                /* f/w unavailable */
                return SOC_E_UNAVAIL;
            }
            /* matching f/w found */
            *fw = fw_desc[i].fw;
            *fw_len = fw_desc[i].fw_len;
            return SOC_E_NONE;
        }
        i++;
    }
    if (i == MAX_FW_TYPES) {
        /* no more room */
        return SOC_E_UNAVAIL;
    }

    fw_desc[i].dev_name = dev_name;

    if (soc_phy_fw_acquire && ((*soc_phy_fw_acquire)(dev_name, fw, fw_len) == SOC_E_NONE)) {
        fw_desc[i].fw = *fw;
        fw_desc[i].fw_len = *fw_len;
        return SOC_E_NONE;
    } else {
        /* This type of f/w is not found. So add a blacklist entry. */ 
        fw_desc[i].fw = NO_FW;
    }

    return SOC_E_UNAVAIL;
}

void soc_phy_fw_put_all(void)
{
    int i = 0;

    while(i < MAX_FW_TYPES) {
        if (fw_desc[i].dev_name == NULL) {
            /* empty slot */
            break;
        }
        if ((fw_desc[i].fw == NO_FW) || (soc_phy_fw_release && 
            ((*soc_phy_fw_release)(fw_desc[i].dev_name, fw_desc[i].fw, fw_desc[i].fw_len) == SOC_E_NONE))) {
            fw_desc[i].dev_name = NULL;
            fw_desc[i].fw = NULL;
            fw_desc[i].fw_len = 0;
        } 
        i++;
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
soc_format_long_integer(char *buf, uint32 *val, int nval)
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

/*
 * Format uint64
 * Endian handling is taken into account.
 */

void
soc_format_uint64(char *buf, uint64 n)
{
    uint32              val[2];

    val[0] = COMPILER_64_LO(n);
    val[1] = COMPILER_64_HI(n);

    soc_format_long_integer(buf, val, 2);
}


/*
 * Convert hex character to digit
 */

int
soc_xdigit2i(int digit)
{
    if (digit >= '0' && digit <= '9') return (digit - '0'     );
    if (digit >= 'a' && digit <= 'f') return (digit - 'a' + 10);
    if (digit >= 'A' && digit <= 'F') return (digit - 'A' + 10);
    return 0;
}

/*
 * Return true if a constant is a well-formed integer of the type
 * supported by parse_integer.
 */

int
soc_isint(char *s)
{
    int base;

    if (s == NULL) {
        return 0;
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
        if (!isxdigit((unsigned) *s) || soc_xdigit2i(*s) >= base) {
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
soc_parse_integer(char *str)
{

    if (!soc_isint(str)) {
        cli_out("WARNING: truncated malformed integer \"%s\"\n", str);
    }

    return _shr_ctoi(str);
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
soc_parse_long_integer(uint32 *val, int nval, char *str)
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


    if (str[0] != '0' || (str[1] != 'x' && str[1] != 'X')) {
        val[0] = soc_parse_integer(str);
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
        while (t > eight + 2 && *s != 'x') {
            *--t = *s--;
        }
        *--t = 'x';
        *--t = '0';

        val[i++] = soc_parse_integer(t);
    } while (*s != 'x' && i < nval);

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
 * Parse uint64
 * Endian handling is taken into account.
 */

uint64
soc_parse_uint64(char *str)
{
    uint32 tmpval[2];
    uint64 rval;

    soc_parse_long_integer(tmpval, 2, str);
    COMPILER_64_SET(rval, tmpval[1], tmpval[0]);

    return rval;
}

int soc_phy_misc(const char *dev_name, void *arg)
{
    int i = 0;

    while(i < _MAX_PHYS) {
        if (misc_desc[i].dev_name == NULL) {
            /* empty slot */
            break;
        }
        if ( !sal_strcmp(dev_name, misc_desc[i].dev_name) && (misc_desc[i].arg == arg)) {
            /* Matching entry found. Already invoked. */

            return SOC_E_NONE;
        }
        i++;
    }
    if (i == _MAX_PHYS) {
        /* no more room */
        return SOC_E_UNAVAIL;
    }

    misc_desc[i].dev_name = dev_name;
    misc_desc[i].arg = arg;

    if ( !soc_phy_misc_launch ) {
        return SOC_E_UNAVAIL;
    }

    if ((*soc_phy_misc_launch)(dev_name, arg) == SOC_E_NONE) {
        return SOC_E_NONE;
    }

    misc_desc[i].dev_name = NULL;
    misc_desc[i].arg = NULL;

    return SOC_E_FAIL;
}

