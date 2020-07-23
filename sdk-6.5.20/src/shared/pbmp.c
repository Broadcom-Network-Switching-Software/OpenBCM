/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Port Bitmap helper functions
 * Only used if _SHR_DEFINE_PBMP_FUNCTIONS is set in <shared/pbmp.h>
 * (everything is done inline in <shared/pbmp.h> otherwise)
 *
 * Well, actually _shr_pbmp_format() is always available
 */

#include <sal/types.h>
#include <sal/core/libc.h>
#include <shared/util.h>
#include <shared/pbmp.h>
#include <sal/appl/sal.h>



/*
 * Size of a single range buffer in shr_pbmp_range_format()
 */
#define SHR_PBMP_SINGLE_RANGR_FMT_LEN (20)
#ifdef _SHR_DEFINE_PBMP_FUNCTIONS



/* returns 1 is the bitmap is empty */
int
_shr_pbmp_bmnull(_shr_pbmp_t *bmp)
{
    int	i;

    for (i = 0; i < _SHR_PBMP_WORD_MAX; i++) {
	if (_SHR_PBMP_WORD_GET(*bmp, i) != 0) {
	    return 0;
	}
    }
    return 1;
}

/* returns 1 is the two bitmaps are equal */
int
_shr_pbmp_bmeq(_shr_pbmp_t *bmp1, _shr_pbmp_t *bmp2)
{
    int	i;

    for (i = 0; i < _SHR_PBMP_WORD_MAX; i++) {
	if (_SHR_PBMP_WORD_GET(*bmp1, i) != _SHR_PBMP_WORD_GET(*bmp2, i)) {
	    return 0;
	}
    }
    return 1;
}

#endif /* _SHR_DEFINE_PBMP_FUNCTIONS */

/* format a bitmap into a static buffer suitable for printing */
char *
_shr_pbmp_format(_shr_pbmp_t bmp, char *buf)
{
    int		i;
    char	*bp;

    if (buf == NULL) {
	return buf;
    }
    buf[0] = '0';
    buf[1] = 'x';
    bp = &buf[2];
    for (i = _SHR_PBMP_WORD_MAX-1; i >= 0; i--) {
	_shr_format_integer(bp, _SHR_PBMP_WORD_GET(bmp, i), 8, 16);
	bp += 8;
    }
    return buf;
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
 *              ;
 *      STEP    : ':' INTEGER           { return $2 };
 *              ;

 *
 * Returns 0 on success, -1 on syntax error.
 */

int
_shr_pbmp_parse(char *s, _shr_pbmp_t *pbmp, uint32 *arr, uint8 *is_hex)
{
    _shr_pbmp_t  bmall;
    int          plast, pfirst, pstep, port, p = 0, i =0 ,rv = 0;
    char        *sn, *se;

    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {

        * is_hex = TRUE;
        rv = _shr_pbmp_decode(s, pbmp);
        if (rv == 0)
        {
            _SHR_PBMP_ITER(*pbmp, p)
            {
               arr[i++] = p;
            }
        }
        return rv;
    }

    _SHR_PBMP_CLEAR(bmall);
    _SHR_PBMP_CLEAR(*pbmp);

    pfirst = -1;
    pstep = 1;


    while (*s) {


        for (sn = s; *sn && *sn != '-' && *sn != ','  &&
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

        if (sn == s) {                          /* unprefixed number */
            plast = port;
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
            /* fall through */
        case ',':
        case '\0':
            if (plast < 0) {                    /* complete bitmap op */
                pfirst = 999;
                plast = -1;
               _SHR_PBMP_ITER(bmall, p) {
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
                p =  port ;
                /* coverity[overrun-local] */
                if ((p >= 0) && _SHR_PBMP_NOT_NULL(bmall) && !_SHR_PBMP_MEMBER(bmall, p)) {
                    continue;   /* skip gaps in range */
                }
                _SHR_PBMP_PORT_ADD(*pbmp, p );
                arr[i++] = p;
                }
            if (*s == '\0') {
                return 0;
            }
            pfirst = -1;
            pstep = 1;
            _SHR_PBMP_CLEAR(bmall);
            break;
        default:
            return -1;                          /* error: unexpected char */
        }
        s += 1;
    }
    return -1;                                  /* error: unexpected end */
}

/*
 * decode a string in hex format into a bitmap
 * returns 0 on success, -1 on error
 */
int
_shr_pbmp_decode(char *s, _shr_pbmp_t *bmp)
{
    char	*e;
    uint32	v;
    int		p;

    _SHR_PBMP_CLEAR(*bmp);

    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
	/* get end of string */
	s += 2;
	for (e = s; *e; e++)
	    ;
	e -= 1;
	/* back up to beginning of string, setting ports as we go */
	p = 0;
	while (e >= s) {
	    if (*e >= '0' && *e <= '9') {
		v = *e - '0';
	    } else if (*e >= 'a' && *e <= 'f') {
		v = *e - 'a' + 10;
	    } else if (*e >= 'A' && *e <= 'F') {
		v = *e - 'A' + 10;
	    } else {
		return -1;		/* error: invalid hex digits */
	    }
	    e -= 1;
	    /* now set a nibble's worth of ports */
	    if ((v & 1) && p < _SHR_PBMP_PORT_MAX) {
		_SHR_PBMP_PORT_ADD(*bmp, p);
	    }
	    p += 1;
	    if ((v & 2) && p < _SHR_PBMP_PORT_MAX) {
		_SHR_PBMP_PORT_ADD(*bmp, p);
	    }
	    p += 1;
	    if ((v & 4) && p < _SHR_PBMP_PORT_MAX) {
		_SHR_PBMP_PORT_ADD(*bmp, p);
	    }
	    p += 1;
	    if ((v & 8) && p < _SHR_PBMP_PORT_MAX) {
		_SHR_PBMP_PORT_ADD(*bmp, p);
	    }
	    p += 1;
	}
    } else {
	v = 0;
	while (*s >= '0' && *s <= '9') {
	    v = v * 10 + (*s++ - '0');
	}
	if (*s != '\0') {
	    return -1;			/* error: invalid decimal digits */
	}
	p = 0;
	while (v) {
	    if ((v & 1) && p < _SHR_PBMP_PORT_MAX) {
		_SHR_PBMP_PORT_ADD(*bmp, p);
	    }
	    v >>= 1;
	    p += 1;
	}
    }
    return 0;
}

/* 
 * format a bitmap into a static buffer suitable for printing - range format 
 * If the the string is too long - fallback to printing bitmap 
 */
char *
shr_pbmp_range_format(_shr_pbmp_t bmp, char *buf, int buf_size)
{
    int port;
    int cur_buf_size = 0;
    int temp_buf_size = 0;
    char temp_buffer[SHR_PBMP_SINGLE_RANGR_FMT_LEN];
    int range_start, range_end;
    int first_range = 1;

    *buf = 0;

    _SHR_PBMP_ITER(bmp, port)
    {
        range_start = port;
        range_end = port;

        /* iterate until the end of the range */ 
        while (port + 1 < _SHR_PBMP_PORT_MAX && _SHR_PBMP_MEMBER(bmp, port + 1))
        {
            port++;
            range_end++;
        }

        /* print to temp buffer */
        if (range_start == range_end)
        {
            sal_snprintf(temp_buffer, SHR_PBMP_SINGLE_RANGR_FMT_LEN, "%s%d", first_range ? "" : ", ", range_start);
        }
        else
        {
            sal_snprintf(temp_buffer, SHR_PBMP_SINGLE_RANGR_FMT_LEN, "%s%d-%d", first_range ? "" : ", ", range_start, range_end);
        } 

        /* check buffer size */
        temp_buf_size = sal_strlen(temp_buffer);
        cur_buf_size += temp_buf_size;

        /* print to buffer */
        if ( cur_buf_size < buf_size - 1)
        {
            sal_snprintf(buf, buf_size, "%s%s", buf, temp_buffer);
        }
        else
        {
            /* fallback to print bitmap */
            return _shr_pbmp_format(bmp, buf);
        }

        /* unmark first range */
        first_range = 0;
        /* continue to next range*/
    }

    return buf;
}

