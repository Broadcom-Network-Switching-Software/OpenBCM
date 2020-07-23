/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * General utility routines
 */

#include <assert.h>

#include <shared/util.h>
#include <shared/l3.h>
#include <shared/error.h>

#include <sal/types.h>
#include <sal/core/libc.h>

/*
 * Swap the bytes in a 32-bit word
 */

STATIC INLINE unsigned int
__shr_swap32(unsigned int i)
{
    i = (i << 16) | (i >> 16);

    return (i & 0xff00ffff) >> 8 | (i & 0xffff00ff) << 8;
}

unsigned int
_shr_swap32(unsigned int i)
{
    return __shr_swap32(i);
}

/*
 * Swap the bytes in a 16-bit half-word
 */

STATIC INLINE unsigned short
__shr_swap16(unsigned short i)
{
    return i << 8 | i >> 8;
}

unsigned short
_shr_swap16(unsigned short i)
{
    return __shr_swap16(i);
}

/*
 * Return the number of bits set in a unsigned int
 */

int
_shr_popcount(unsigned int n)
{
    n = (n & 0x55555555) + ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    n = (n + (n >> 4)) & 0x0f0f0f0f;
    n = n + (n >> 8);

    return (n + (n >> 16)) & 0xff;
}

/*
 * A few bit twiddling routines, initially for hashing
 */

/* reverse the bits in an 8 bit byte */
uint8
_shr_bit_rev8(uint8 n)
{
    n = (((n & 0xaa) >> 1) | ((n & 0x55) << 1));
    n = (((n & 0xcc) >> 2) | ((n & 0x33) << 2));
    n = (((n & 0xf0) >> 4) | ((n & 0x0f) << 4));
    return n;
}


/* reverse the bits in an 16 bit short */
uint16
_shr_bit_rev16(uint16 n)
{
    n = (((n & 0xaaaa) >> 1) | ((n & 0x5555) << 1));
    n = (((n & 0xcccc) >> 2) | ((n & 0x3333) << 2));
    n = (((n & 0xf0f0) >> 4) | ((n & 0x0f0f) << 4));
    n = (((n & 0xff00) >> 8) | ((n & 0x00ff) << 8));
    return n;
}

/* reverse the bits in an 32 bit long */
uint32
_shr_bit_rev32(uint32 n)
{
    n = (((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1));
    n = (((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2));
    n = (((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4));
    n = (((n & 0xff00ff00) >> 8) | ((n & 0x00ff00ff) << 8));
    return (n >> 16) | (n << 16);
}

/* reverse the bits in each byte of a 32 bit long */
uint32
_shr_bit_rev_by_byte_word32(uint32 n)
{
    n = (((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1));
    n = (((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2));
    n = (((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4));
    return n;
}

/*
 * Ethernet CRC Algorithm
 *
 * To generate CRC, do not include CRC field in data:
 *    unsigned int crc = ~_shr_crc32(~0, data, len)
 *
 * To check CRC, include CRC field in data:
 *    unsigned int check = _shr_crc32(~0, data, len)
 *    If CRC is correct, result will be _SHR_CRC32_CORRECT.
 *
 * NOTE: This routine generates the same 32-bit value whether the
 * platform is big- or little-endian.  The value must be stored into a
 * network packet in big-endian order, i.e. using htonl() or equivalent.
 * (Polynomial x ^ 32 + x ^ 28 + x ^ 23 + x ^ 22 + x ^ 16 + x ^ 12 + x ^ 11 +
 *             x ^ 10 + x ^ 8 + x ^ 7 + x ^ 5 + x ^ 4 + x ^ 2  + x ^ 1 + 1)
 */

static int		_shr_crc_table_inited;
static unsigned int	_shr_crc_table[256];

unsigned int
_shr_crc32(unsigned int crc, unsigned char *data, int len)
{
    int			i;

    if (!_shr_crc_table_inited) {
	int		j;
	unsigned int		accum;

	for (i = 0; i < 256; i++) {
	    accum = i;

	    for (j = 0; j < 8; j++) {
		if (accum & 1) {
		    accum = accum >> 1 ^ 0xedb88320UL;
		} else {
		    accum = accum >> 1;
		}
	    }

	    _shr_crc_table[i] = __shr_swap32(accum);
	}

	_shr_crc_table_inited = 1;
    }

    for (i = 0; i < len; i++) {
	crc = crc << 8 ^ _shr_crc_table[crc >> 24 ^ data[i]];
    }

    return crc;
}

/* Matches _shr_crc32 above */
unsigned int
_shr_crc32b(unsigned int crc, unsigned char *data, int nbits)
{
    int			i;
    int		        j;
    unsigned int	accum;
    unsigned int        poly = 0xedb88320UL;
    int                 last_nbits;

    if (!_shr_crc_table_inited) {
        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
            }
            _shr_crc_table[i] = __shr_swap32(accum);
        }
	_shr_crc_table_inited = 1;
    }

    for (i = 0; i < (nbits / 8); i++) {
	crc = (crc << 8) ^ _shr_crc_table[data[i] ^ ((crc >> 24) & 0x000FF)];
    }

    last_nbits =  nbits % 8;
    if (last_nbits) {
        accum = ((crc >> (32 - last_nbits)) & ((1 << last_nbits) - 1)) ^
                (data[i]  & ((1 << last_nbits) - 1));
        for (j = 0; j < last_nbits; j++) {
            accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
        }
        crc = (crc << last_nbits) ^ __shr_swap32(accum);
    }

    return crc;
}

static int		_shr_crc32bd15_table_inited;
static unsigned int	_shr_crc32bd15_table[256];

/* Matches Draco 1.5 CRC32 key76 */

unsigned int
_shr_crc32bd15(unsigned int crc, unsigned char *data, int nbits)
{
    int			i;
    int		        j;
    unsigned int	accum;
    unsigned int        poly = 0xedb88320UL;
    int                 last_nbits;

    if (!_shr_crc32bd15_table_inited) {
        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
            }
            _shr_crc32bd15_table[i] = accum;
        }
	_shr_crc32bd15_table_inited = 1;
    }

    for (i = 0; i < (nbits / 8); i++) {
	crc = (crc >> 8) ^ ((data[i] & 0x000000FF) << 24) ^
               _shr_crc32bd15_table[crc & 0x000000FF];
    }

    last_nbits =  nbits % 8;
    if (last_nbits) {
        accum = crc & ((1 << last_nbits) - 1);
        for (j = 0; j < last_nbits; j++) {
            accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
        }
        crc = (crc >> last_nbits) ^ accum ^
               ((data[i]  & ((1 << last_nbits) - 1)) << (32 - last_nbits));
    }

    return crc;
}


/*
 * CRC16 for Draco (Polynomial x ^ 16 + x ^ 15 + x ^ 2 + 1)
 */

static unsigned short _shr_crc_16_table[16] = {
    0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
    0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
};

unsigned short
_shr_crc16(int start, unsigned char *p, int n)
{
    unsigned short int crc = start;
    int r;

    /* while there is more data to process */
    while (n-- > 0) {

        /* compute checksum of lower four bits of *p */
        r = _shr_crc_16_table[crc & 0xF];
        crc = (crc >> 4) & 0x0FFF;
        crc = crc ^ r ^ _shr_crc_16_table[*p & 0xF];

        /* now compute checksum of upper four bits of *p */
        r = _shr_crc_16_table[crc & 0xF];
        crc = (crc >> 4) & 0x0FFF;
        crc = crc ^ r ^ _shr_crc_16_table[(*p >> 4) & 0xF];

        /* next... */
        p++;
    }

    return(crc);
}

static int		_shr_crc16b_table_inited;
static unsigned int	_shr_crc_16btable[256];

/* Matches _shr_crc16 above */

unsigned short
_shr_crc16b(int crc, unsigned char *data, int nbits)
{
    int			i;
    int		        j;
    unsigned int	accum;
    unsigned int        poly = 0xa001;
    int                 last_nbits;

    if (!_shr_crc16b_table_inited) {
        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
            }
            _shr_crc_16btable[i] = accum;
        }
        _shr_crc16b_table_inited = 1;
    }

    for (i = 0; i < (nbits / 8); i++) {
	crc = (crc >> 8) ^ _shr_crc_16btable[((data[i] & 0x000000FF) << 0) ^
                                             (crc & 0x000000FF)];
    }

    last_nbits =  nbits % 8;
    if (last_nbits) {
        accum = (crc & ((1 << last_nbits) - 1)) ^
                (data[i]  & ((1 << last_nbits) - 1));
        for (j = 0; j < last_nbits; j++) {
            accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
        }
        crc = (crc >> last_nbits) ^ accum;
    }

    return crc;
}

/* Matches Draco 1.5 CRC16 key76 */

unsigned short
_shr_crc16bd15(int crc, unsigned char *data, int nbits)
{
    int			i;
    int		        j;
    unsigned int	accum;
    unsigned int        poly = 0xa001;
    int                 last_nbits;

    if (!_shr_crc16b_table_inited) {
        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
            }
            _shr_crc_16btable[i] = accum;
        }
        _shr_crc16b_table_inited = 1;
    }

    for (i = 0; i < (nbits / 8); i++) {
	crc = (crc >> 8) ^ ((data[i] & 0x000000FF) << 8) ^
               _shr_crc_16btable[crc & 0x000000FF];
    }

    last_nbits =  nbits % 8;
    if (last_nbits) {
        accum = crc & ((1 << last_nbits) - 1);
        for (j = 0; j < last_nbits; j++) {
            accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
        }
        crc = (crc >> last_nbits) ^ accum ^
               ((data[i]  & ((1 << last_nbits) - 1)) << (16 - last_nbits));
    }

    return crc;
}

uint16
_shr_crc16_draco_array(uint32 *hash_words, int n)
{
    uint8 data[30];
    int i;
    uint16 crc;

    sal_memset(data, 0, 30);
    
    for (i = 0; i < n; i++) {
        data[i] = (hash_words[i/4] >> ((i % 4) * 8)) & 0xff;
    }

    crc = _shr_crc16(0, data, n);
    crc = _shr_bit_rev16(crc);

    return crc;
}

/* CRC16 CCITT: Polynomial x^16 + x^12 + x^5 + 1 */

static int	        _shr_crc16_ccitt_table_inited;
static uint16           _shr_crc16_ccitt_table[256];

uint16
_shr_crc16_ccitt(int crc, unsigned char *data, int len)
{
    int			i;
    int		        j;
    uint32	        accum;
    uint32              poly = 0x8408;

    if (!_shr_crc16_ccitt_table_inited) {
        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                accum = (accum & 1) ? (accum >> 1 ^ poly) : (accum >> 1);
            }
            _shr_crc16_ccitt_table[i] = accum;
        }
        _shr_crc16_ccitt_table_inited = 1;
    }

    for (i = 0; i < len; i++) {
	crc = (crc >> 8) ^
            _shr_crc16_ccitt_table[((data[i] & 0x000000FF) << 0) ^
                                   (crc & 0x000000FF)];
    }

    return crc;
}

uint16
_shr_crc16_ccitt_array(uint32 *hash_words, int n)
{
    uint8 data[30];
    int i;
    uint16 crc;

    sal_memset(data, 0, 30);
    
    for (i = 0; i < n; i++) {
        data[i] = (hash_words[i/4] >> ((i % 4) * 8)) & 0xff;
    }

    crc = _shr_crc16_ccitt(0, data, n);
    crc = _shr_bit_rev16(crc);
    return crc;
}

static int    _shr_crc32_cast_table_inited;
static uint32 _shr_crc32_cast_table[256];

/*
 * The following function implements polynomial:
 *  x^32 + x^28 + x^27 + x^26 + x^25 + x^23 + x^22 + x^20 + 
 *  x^19 + x^18 + x^14 + x^13 + x^11 + x^10 + x^9 + x^8 + x^6 + 1
 */
uint32
_shr_crc32_castagnoli(unsigned int crc, unsigned char *data, int len)
{
    int			i;

    if (!_shr_crc32_cast_table_inited) {
	int		j;
	unsigned int		accum;

	for (i = 0; i < 256; i++) {
	    accum = i;

	    for (j = 0; j < 8; j++) {
		if (accum & 1) {
		    accum = accum >> 1 ^ 0x82F63B78UL;
		} else {
		    accum = accum >> 1;
		}
	    }

	    _shr_crc32_cast_table[i] = _shr_swap32(accum);
	}

	_shr_crc32_cast_table_inited = 1;
    }

    for (i = 0; i < len; i++) {
	crc = crc << 8 ^ _shr_crc32_cast_table[crc >> 24 ^ data[i]];
    }

    return crc;
}

uint32
_shr_crc32_castagnoli_array(uint32 *hash_words, int n)
{
    uint8 data[30];
    int i;
    uint32 crc;

    sal_memset(data, 0, 30);
    
    for (i = 0; i < n; i++) {
        data[i] = (hash_words[i/4] >> ((i % 4) * 8)) & 0xff;
    }

    crc = _shr_crc32_castagnoli(0, data, n);
    crc = _shr_bit_rev_by_byte_word32(crc);
    return crc;
}

static int    _shr_crc32_ethernet_table_inited;
static uint32 _shr_crc32_ethernet_table[256];

/*
 * The following function implements polynomial:
 *  x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 +
 *  x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
 */
uint32
_shr_crc32_ethernet(unsigned int crc, unsigned char *data, int len)
{
    int i;

    if (!_shr_crc32_ethernet_table_inited) {
        int j;
        unsigned int accum;

        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                if (accum & 1) {
                    accum = accum >> 1 ^ 0xEDB88320UL;
                } else {
                    accum = accum >> 1;
                }
            }

            _shr_crc32_ethernet_table[i] = _shr_swap32(accum);
        }

        _shr_crc32_ethernet_table_inited = 1;
    }

    for (i = 0; i < len; i++) {
        crc = crc << 8 ^ _shr_crc32_ethernet_table[crc >> 24 ^ data[i]];
    }

    return crc;
}

uint32
_shr_crc32_ethernet_array(uint32 *hash_words, int n)
{
    uint8 data[30];
    int i;
    uint32 crc;

    sal_memset(data, 0, 30);

    for (i = 0; i < n; i++) {
        data[i] = (hash_words[i/4] >> ((i % 4) * 8)) & 0xff;
    }

    crc = _shr_crc32_ethernet(0, data, n);
    crc = _shr_bit_rev_by_byte_word32(crc);
    return crc;
}

static int    _shr_crc32_koopman_table_inited;
static uint32 _shr_crc32_koopman_table[256];

/*
 * The following function implements polynomial:
 *  x^32 + x^30 + x^29 + x^28 + x^26 + x^20 + x^19 + x^17 +
 *  x^16 + x^15 + x^11 + x^10 + x^7 + x^6 + x^4 + x^2 + x + 1
 */
uint32
_shr_crc32_koopman(unsigned int crc, unsigned char *data, int len)
{
    int i;

    if (!_shr_crc32_koopman_table_inited) {
        int j;
        unsigned int accum;

        for (i = 0; i < 256; i++) {
            accum = i;

            for (j = 0; j < 8; j++) {
                if (accum & 1) {
                    accum = accum >> 1 ^ 0xEB31D82EUL;
                } else {
                    accum = accum >> 1;
                }
            }

            _shr_crc32_koopman_table[i] = _shr_swap32(accum);
        }

        _shr_crc32_koopman_table_inited = 1;
    }

    for (i = 0; i < len; i++) {
        crc = crc << 8 ^ _shr_crc32_koopman_table[crc >> 24 ^ data[i]];
    }

    return crc;
}

uint32
_shr_crc32_koopman_array(uint32 *hash_words, int n)
{
    uint8 data[30];
    int i;
    uint32 crc;

    sal_memset(data, 0, 30);

    for (i = 0; i < n; i++) {
        data[i] = (hash_words[i/4] >> ((i % 4) * 8)) & 0xff;
    }

    crc = _shr_crc32_koopman(0, data, n);
    crc = _shr_bit_rev_by_byte_word32(crc);
    return crc;
}

/*
 * Function:
 *	_shr_sort
 * Purpose:
 *	Simple general purpose Shell sort with decent performance O(N log N)
 */

#define A(i)	((void *) &((char *)(base))[(i) * (size)])

void
_shr_sort(void *base, int count, int size, int (*compar)(void *, void *))
{
    int		h = 1, i, j;
    char	tmp[256];

    assert(size < (int)sizeof(tmp));

    while (h * 3 + 1 < count) {
	h = 3 * h + 1;
    }

    while (h > 0) {
	for (i = h - 1; i < count; i++) {
	    sal_memcpy(tmp, A(i), size);

	    for (j = i; j >= h && (*compar)(A(j - h), tmp) > 0; j -= h) {
		sal_memcpy(A(j), A(j - h), size);
	    }

	    sal_memcpy(A(j), tmp, size);
	}

	h /= 3;
    }
}

/*
 * Function:
 *	_shr_bsearch
 * Purpose:
 *	Simple general purpose binary search in a (low to high) sorted array.
 * Returns: 
 *    (negative insertion index) or positive exact match index.
 *
 *    Note that the negative insertion index is one greater than the
 *    actual insertion index to allow for an indication of the
 *    insertion index at the beginning of the array (index 0).
 */
int
_shr_bsearch(void *base, int count, int size, 
             void *target, int (*compar)(void *, void *))
{
    int     start = 0;
    int     end = count - 1;
    int     midpoint;
    int     compare_result;

    /* Keep going as long as interval of possible matches not empty. */
    while (end >= start) {
        midpoint = (end + start) / 2;
        compare_result = (*compar)(A(midpoint), target);
        if (0 == compare_result) {
            return midpoint;
        }
        if (compare_result < 0) {
            start = midpoint + 1;
        } else if (compare_result > 0) {
            end = midpoint - 1;
        }
    }

    return (-1) * (start + 1);
}

#undef A

/*
 * _shr_format_integer
 *
 *   Format an integer as a string of ASCII digits.
 *   Used for debugging printf's in the driver.
 */

void
_shr_format_integer(char *buf, unsigned int n, int min_digits, int base)
{
    static char		*digit_char = "0123456789abcdef";
    unsigned int	tmp;
    int			digit, needed_digits = 0;

    for (tmp = n, needed_digits = 0; tmp; needed_digits++) {
	tmp /= base;
    }

    if (needed_digits > min_digits)
	min_digits = needed_digits;

    buf[min_digits] = 0;

    for (digit = min_digits - 1; digit >= 0; digit--) {
	buf[digit] = digit_char[n % base];
	n /= base;
    }
}

/*
 * _shr_format_long_integer
 *
 * Format an arbitrary precision long integer.
 *
 * If the value is less than 10, prints one decimal digit;
 * otherwise output is in hex format with 0x prefix.
 *
 * val[0] is the least significant word.
 * nval is the number of bytes in the value.
 */

void
_shr_format_long_integer(char *buf, unsigned int *val, int nval)
{
    int i = BYTES2WORDS(nval) - 1;

    /*
    Don't actually skip leading 0's, as this makes packet buffers
    and other buffer memories really hard to read and to determine
    what the contents are.
    */
#if 0
    for (i = BYTES2WORDS(nval) - 1; i > 0; i--) {	/* Skip leading zeroes */
	if (val[i]) {
	    break;
	}
    }
#endif /* 0 */

    if (i == 0 && val[i] < 10) {	/* Only a single word < 10 */
	buf[0] = '0' + val[i];
	buf[1] = 0;
    } else {
	buf[0] = '0';			/* Print first word */
	buf[1] = 'x';

        /*
        Don't use the previous value of 1 min digit for multiple
        integer values, as that makes a long integer of all zeroes
        print as just '0', which makes memory contents hard to read.
        Only use mindigit of 1 if only one integer to print
        */
        if (i == 0) {
	_shr_format_integer(buf + 2, val[i], 1, 16);
        }
        else
        {
            if ((nval % 4) == 0) {
                _shr_format_integer(buf + 2, val[i], 8, 16);
            }
            else
            {
                _shr_format_integer(buf + 2, val[i], (2 * (nval % 4)), 16);
            }
        }

	while (--i >= 0) {		/* Print rest of words, if any */
	    while (*buf) {
		buf++;
	    }

	    _shr_format_integer(buf, val[i], 8, 16);
	}
    }
}

/* 
 * _shr_format_uint64_hexa_string 
 *  
 * Converts uint64 to hexa string
 */
void 
_shr_format_uint64_hexa_string( uint64 value, char* uint64_hexa_string)
{
    if(COMPILER_64_HI(value)) {
        sal_sprintf(uint64_hexa_string, "0x%x", COMPILER_64_HI(value));
        sal_sprintf(uint64_hexa_string + sal_strlen(uint64_hexa_string), "%08x", COMPILER_64_LO(value));
    } else {
        sal_sprintf(uint64_hexa_string, "0x%x", COMPILER_64_LO(value));
    }
}




/*
 * _shr_ctoi
 *
 *   Converts a C-style constant integer to unsigned int
 */

unsigned int
_shr_ctoi(const char *s)
{
    unsigned int	n, neg, base = 10;

    s += (neg = (*s == '-'));

    if (*s == '0') {
	s++;

	if (*s == 'x' || *s == 'X') {
	    base = 16;
	    s++;
	} else if (*s == 'b' || *s == 'B') {
	    base = 2;
	    s++;
	} else {
	    base = 8;
	}
    }

    for (n = 0; ((*s >= 'a' && *s <= 'z' && base > 10) ||
		 (*s >= 'A' && *s <= 'Z' && base > 10) ||
		 (*s >= '0' && *s <= '9')); s++) {
	n = n * base +
	    (*s >= 'a' ? *s - 'a' + 10 :
	     *s >= 'A' ? *s - 'A' + 10 :
	     *s - '0');
    }

    return (neg ? -n : n);
}

/*
 * _shr_ctoa
 *
 *   Converts a C-style constant string to an address
 */

sal_vaddr_t
_shr_ctoa(const char *s)
{
    unsigned int	base = 10;
    sal_vaddr_t		n;

    if (*s == '0') {
	s++;

	if (*s == 'x' || *s == 'X') {
	    base = 16;
	    s++;
	} else if (*s == 'b' || *s == 'B') {
	    base = 2;
	    s++;
	} else {
	    base = 8;
	}
    }

    for (n = 0; ((*s >= 'a' && *s <= 'z' && base > 10) ||
		 (*s >= 'A' && *s <= 'Z' && base > 10) ||
		 (*s >= '0' && *s <= '9')); s++) {
	n = n * base +
	    (*s >= 'a' ? *s - 'a' + 10 :
	     *s >= 'A' ? *s - 'A' + 10 :
	     *s - '0');
    }

    return (n);
}

/*
 * Functions to get around the lack of floating point support 
 * in Linux kernel mode.
 */

/*
 * Function:
 *	_shr_div_exp10
 * Purpose:
 *	Do uint32 division while preserving precision and 
 *	avoiding overflow.
 * Parameters:
 *	d1 - dividend
 *	d2 - divisor
 *	exp10 - factor of 10 by which to multiply the quotient
 * Returns:
 *	Result of division.
 * Notes:
 *	Example:
 *	5000 * 100000000 / 12345
 *	=> d1 = 5000, d2 = 12345, exp10 = 8
 */
int
_shr_div_exp10(int d1, int d2, int exp10)
{
    int rv;
    int e;
    int sign = 1;

    if (d2 < 0) {
        sign = -1;
    }

    while (exp10 && d1 < (0x7FFFFFFF / 10)) {
        d1 = d1 * 10;
        exp10--;
    }
    e = 1;
    while (exp10) {
        e *= 10;
        exp10--;
    }
    rv = (d1 / d2) * e;
    if (e > 1) {
        rv += ((d1 % d2) * e) / (d2 * sign);
    }
    return rv;
}

/*
 * Function:
 *	_shr_atof_exp10
 * Purpose:
 *	Parse floating point input string and transform the result 
 *	into a 32-bit integer, optionally multiplying with an
 *	exponent of 10.
 * Parameters:		
 *	s - string to parse
 * Returns:
 *	Binary value of input string.
 * Notes:
 *	This is a helper function to transform floating point input
 *	into an integer value without loosing any information.
 *
 *	Examples:
 *	_shr_atof_exp10("1.33", 6)   =>   1.33 * 10^6 => 1330000
 *	_shr_atof_exp10("0.1234", 3) => 0.1234 * 10^3 =>     123
 */

int
_shr_atof_exp10(const char *s, int exp10)
{
    int rv = 0;
    int dec_pt = 0;
    int sign = 10;

    if (exp10 > 9 || exp10 < 0) {
        return 0;
    }

    exp10++;

    if (*s == '-') {
        sign = -10;
        s++;
    }

    while (*s && exp10) {
        if (*s >= '0' && *s <= '9') {
            if (exp10) {
                rv *= 10;
            }
            rv += *s - '0';
            if (dec_pt && exp10) {
                exp10--;
            }
        } else if (*s == '.' && !dec_pt) {
            dec_pt = 1;
        } else {
            break;
        }
        s++;
    }
    while (exp10-- > 0) {
        rv *= 10;
    }

    return (rv + 5) / sign;
}

/*
 * Function:
 *	_shr_div32r
 * Purpose:
 *	Do 32 bit integer division with rounding.
 * Parameters:		
 *	d1 - dividend
 *	d2 - divisor
 * Returns:
 *	Result of division.
 */

uint32
_shr_div32r(uint32 d1, uint32 d2)
{
    uint32 rv;

    rv = d1 / d2;
    if ((d1 % d2) >= (d2 / 2)) {
        rv += 1;
    }
    return rv;
}

/*
 * Function:
 *	_shr_scale_uint64
 * Purpose:
 *	Transform 64 bit integer into a 32 bit integer
 *	and a prefix multiplier (kilo, mega, giga etc.)
 * Parameters:		
 *	d64 - 64 bit integer to scale
 *	base - value of 1k (must be 1000 or 1024)
 *	prec - precision by which result is multiplied
 *	d32 - pointer to 32 bit integer (OUT)
 * Returns:
 *	Prefix multiplier string ("", "K", "M", "G", "T").
 * Notes:
 *	This is helper function for displaying 64 bit
 *	integers without using floating point or 64 bit
 *	mul/div/mod.
 *
 *	Example:
 *	We want to display 20*10^12 (0x000012309ce54000)
 *	as 20.00 T (tera):
 *
 *	d64 is 0x000012309ce54000
 *	base is 1000 (i.e. 1k = 1000)
 *	precision is 100 (2 decimals)
 *
 *	s = _shr_scale_uint64(d64, 1000, 100, &d32);
 *	printf("%d.%02d %s", d32 / 100, d32 % 100, s);
 */

char *
_shr_scale_uint64(uint64 d64, int base, int prec, uint32 *d32)
{
    uint32 value;
    uint64 value64;
    int t, e;

    *d32 = 0;

    switch (base) {
    case 1000:
    case 1024:
        break;
    default:
        return 0;
    }

    switch (prec) {
    case 1:
    case 10:
    case 100:
    case 1000:
        break;
    default:
        return 0;
    }

    COMPILER_64_SET(value64, COMPILER_64_HI(d64), COMPILER_64_LO(d64));

    e = 0;
    while (COMPILER_64_HI(value64)) {
        /* Shift down to 32 bits */
        t = 0;
        while (COMPILER_64_HI(value64)) {
            COMPILER_64_SHR(value64, 1);
            t++;
        }
        /* Perform 32 bit division with rounding */
        value = _shr_div32r(COMPILER_64_LO(value64), base);
        /* Shift back - precision is still adequate */
        COMPILER_64_SET(value64, 0, value);
        COMPILER_64_SHL(value64, t);
        e++;
    }
    value = COMPILER_64_LO(value64);

    t = 1;
    while ((value / t) > (uint32)base) {
        t *= base;
        e++;
    }

    if (e > 0) {
    /*
     * COVERITY
     * As per switch statement on line 955, The function proceeds
     * only if value of prec  is 1,10,100,1000 only for rest of the
     * values function returns so the stated condition will not occur.
     */
    /* coverity[divide_by_zero : FALSE] */
	*d32 = _shr_div32r(value, t / prec);
        if (e == 1) {
            return (base == 1000) ? "k" : "K";
        }
        if (e == 2) {
            return "M";
        }
        if (e == 3) {
            return "G";
        }
        return "T";
    }
    *d32 = prec * value;
    return "";
}

/*
 * Function:
 *      _shr_ip6_mask_create
 * Purpose:
 *      Create IPv6 network address from prefix length
 * Parameters:
 *      ip6 - (OUT) IPv6 address holder
 *      len - the prefix/mask length
 * Returns:
 *      none
 */

int
_shr_ip6_mask_create(uint8 *ip6, int len)
{
    int num_bytes, bits_left;
    int i;

    sal_memset(ip6, 0, _SHR_L3_IP6_ADDRLEN);
    if (len == 0) {
        return _SHR_E_PARAM;
    }

    if (len > _SHR_L3_IP6_MAX_NETLEN) {
        len = _SHR_L3_IP6_MAX_NETLEN;
    }

    num_bytes = len / 8;
    bits_left = len % 8;

    for (i = 0; i < num_bytes; i++) {
        ip6[i] = 0xff;
    }

    if (bits_left) {
        ip6[i] = (0xff << (8 - bits_left));
    }

    return _SHR_E_NONE;
}


/*
 * Function:
 *      _shr_ip6_mask_length
 * Purpose:
 *      Return the mask length from IPv6 network address
 * Parameters: 
 *      mask - IPv6 address
 * Returns:
 *      The prefix/mask length
 */

int
_shr_ip6_mask_length(uint8 *mask)
{
    int masklen, i, j;
    uint8 temp;
            
    /* Convert netmask to number of bits */
    masklen = 0;
    
    for (i = 0; i < _SHR_L3_IP6_ADDRLEN; i++) {
        temp = mask[i];
        for (j = 0; j < 8; j++) {
            if (temp & 0x80) {
                masklen++;
                temp = temp << 1;
            } else {
                break;
            }
        }
    }
    return masklen;
}

/*
 * Function
 *      _shr_ip6_addr_compare
 * Purpose
 *      Compare IPv6 address from LSB to MSB
 * Parameters
 *      addr1 - (IN) IPv6 address
 *      addr2 - (IN) IPv6 address
 * Returns
 *      Compare result
 */
int
_shr_ip6_addr_compare(uint8 *addr1, uint8 *addr2)
{
    int i = 0;
    for (i = _SHR_L3_IP6_ADDRLEN - 1; i >= 0; i--) {
        if (addr1[i] != addr2[i]) {
            return -1;
        }
    }

    return 0;

}

/*
 * Function:
 *      _shr_ip_mask_create
 * Purpose:
 *      Create IPv4 network address from prefix length
 * Parameters:
 *      len - the prefix/mask length
 * Returns:
 *      The IPv4 mask
 */
uint32
_shr_ip_mask_create(int len)
{
    return ((len) ? (~((0x1 << (32 - (len))) - 1)) : 0);
}

/*
 * Function:
 *      _shr_ip_mask_length
 * Purpose:
 *      Return the mask length from IPv4 network address
 * Parameters:
 *      mask - The IPv4 mask as IP address
 * Returns:
 *      The IPv4 mask length
 */
int
_shr_ip_mask_length(uint32 mask)
{
    int masklen, i;

    /* Convert netmask to number of bits */
    masklen = 0;

    for (i = 0; i < _SHR_L3_IP_ADDRLEN * 8; i++) {
        if (mask & 0x80000000) {
            masklen++;
            mask = mask << 1;
        } else {
            break;
        }
    }

    return (masklen);
}

/*
 * Function:
 *      _shr_ip_chksum
 * Purpose:
 *      Calculate IP style checksum
 * Parameters:
 *      - length - length of data in bytes
 *      - data - pointer to data
 * Returns:
 *      -checksum
 */
unsigned short
_shr_ip_chksum(unsigned int length, unsigned char *data)
{
    unsigned int chksum = 0;
    unsigned short w16;
    int i=0;

    while (length > 1) {
        w16 = (((unsigned int)data[i]) << 8) + data[i+1];
        chksum += w16;
        i+=2;
        length -= 2;
    }
    if (length) {
        w16 = (((unsigned int)data[i]) << 8) + 0;
        chksum += w16;
    }

    while (chksum >> 16) {
        chksum = (chksum & 0xFFFF) + (chksum >> 16);
    }

    return (~chksum);
}


/*
 * Function:
 *      _shr_tolower
 * Purpose:
 *      Converts uppercase char to lowercase char
 * Parameters:
 *      - c - Char to convert
 * Returns:
 *      Lowercase char
 */
#define _shr_tolower(c) ((c >= 'A' && c <= 'Z') ? (c + 37):c)

/*
 * Function:
 *      _shr_isxdigit
 * Purpose:
 *      Returns true if char is a valid hex digit
 * Parameters:
 *      - c - Char to evaluate
 * Returns:
 *      True if valid hex digit, else false
 */
int  _shr_isxdigit(char c) {
    if ((c >= '0' && c <= '9') || 
	(c >= 'a' && c <= 'f') ||
	(c >= 'A' && c <= 'F'))
	return 1; 
    return 0;
}

/*
 * Function:
 *      _shr_xdigit2i
 * Purpose:
 *      Converts ASCII char to integer
 * Parameters:
 *      - c - Char to evaluate
 * Returns:
 *      Integer value of hex digit
 */
int _shr_xdigit2i(char c) {
    if (c >= '0' && c <= '9')
	return (int)(c - '0');
    else if (c >= 'a' && c <= 'f')
	return (int)(c - 'a') + 10;
    else if (c >= 'A' && c <= 'F')
	return (int)(c - 'A') + 10;
    return 0;
}

/*
 * Function:
 *      _shr_strchr
 * Purpose:
 *      Finds first occurance of char in string
 * Parameters:
 *      - str - String to evaluate
 *      - c - Char to find in string
 * Returns:
 *      Pointer to first occurance of c in string, else NULL
 */
char *_shr_strchr(const char *str, int c) {
    const char *ptr = str;

    while(*ptr != '\0') {
	if (*ptr == (char)c)
	    return (char*)ptr;
	ptr++;
    }

    return NULL;
}


/*
 * Function:
 *      _shr_isint
 * Purpose:
 *      Identify well-formed int
 * Parameters:
 *      - s - String to evaluate
 * Returns:
 *      Return true if a constant is a well-formed integer of the type
 *      supported by parse_integer.
 */

int
_shr_isint(char *s)
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
        if (!_shr_isxdigit((unsigned) *s) || _shr_xdigit2i(*s) >= base) {
            return(0);
        }
    } while (*++s);

    return(1);
}

/*
 * Function:
 *      _shr_parse_macaddr
 * Purpose:
 *      Convert mac address string to six bytes of data
 * Parameters:
 *      - str - MAC string to evaluate
 *      - macaddr - Place to store the result
 * Returns:
 */
int _shr_parse_macaddr(char *str, uint8 *macaddr)
{
    char *s;
    int	colon = FALSE;
    int	i, c1, c2;

    if (_shr_strchr(str, ':')) {		/* Colon format */
	colon = TRUE;
    } else if (*str++ != '0' || _shr_tolower(*str++) != 'x') {
	return -1;
    } else {
	sal_memset(macaddr, 0, 6);
    }
    /* Start at end and work back */
    s = str + sal_strlen(str);
    for (i = 5; (i >= 0) && (s >= str); i--) {
	c2 = (s > str && _shr_isxdigit((unsigned) s[-1])) ? _shr_xdigit2i((unsigned) *--s) : 0;
	c1 = (s > str && _shr_isxdigit((unsigned) s[-1])) ? _shr_xdigit2i((unsigned) *--s) : 0;
	macaddr[i] = c1 * 16 + c2;
	if (colon && (s >= str) && (':' != *--s))
	    break;
    }
    return(((s <= str) && (!colon || (i == 0))) ? 0 : -1);
}

/*
 * Function:
 *      _shr_parse_ipaddr
 * Purpose:
 *      Convert ip address string to four bytes of data
 * Parameters:
 *      - str - IP address string to evaluate
 *      - ipaddr - Place to store the result
 * Returns:
 */
int
_shr_parse_ipaddr(char *s, sal_ip_addr_t *ipaddr)
{
    char *ts;
    int i, x;
    sal_ip_addr_t ip = 0;

    if (strchr(s, '.')) {               /* dotted notation */
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
    } else if (_shr_isint(s)){
        *ipaddr = _shr_ctoi(s);
        return(0);
    } else {
        return(-1);
    }
}

/* routines for moving uint[16|32|64] and int64 to/from network order buffers */

/*                                                                                                            
 * Function:                                                                                                  
 *      _shr_uint16_read                                                                                  
 * Purpose:                                                                                                   
 *      Read a 16-bit unsigned integer from a buffer in network byte order.                                   
 * Parameters:                                                                                                
 *      buffer - (IN) Data buffer.                                                                            
 * Returns:                                                                                                   
 *      Result.                                                                                               
 * Notes:                                                                                                     
 */
uint16
_shr_uint16_read(
    uint8* buffer)
{
    return ((((uint16)(buffer[0])) << 8) + (((uint16)(buffer[1]))));
}


/*                                                                                                            
 * Function:                                                                                                  
 *      _shr_uint16_write                                                                                 
 * Purpose:                                                                                                   
 *      Write a 16-bit unsigned integer to a buffer in network byte order.                                    
 * Parameters:                                                                                                
 *      value  - (IN)  Data.                                                                                  
 *      buffer - (OUT) Data buffer.                                                                           
 * Returns:                                                                                                   
 *      None.                                                                                                 
 * Notes:                                                                                                     
 */
void
_shr_uint16_write(
    uint8* buffer,
    const uint16 value)
{
    buffer[0] = ((value >> 8) & 0xff);
    buffer[1] = ((value) & 0xff);
}


/*                                                                                                            
 * Function:                                                                                                  
 *      _shr_uint32_read                                                                                  
 * Purpose:                                                                                                   
 *      Read a 32-bit unsigned integer from a buffer in network byte order.                                   
 * Parameters:                                                                                                
 *      buffer - (IN) Data buffer.                                                                            
 * Returns:                                                                                                   
 *      Result.                                                                                               
 * Notes:                                                                                                     
 */
uint32
_shr_uint32_read(
    uint8* buffer)
{
    return ((((uint32)(buffer[0])) << 24) +
            (((uint32)(buffer[1])) << 16) +
            (((uint32)(buffer[2])) << 8)  +
            (((uint32)(buffer[3]))));
}


/*                                                                                                            
 * Function:                                                                                                  
 *      _shr_uint32_write                                                                                 
 * Purpose:                                                                                                   
 *      Write a 32-bit unsigned integer to a buffer in network byte order.                                    
 * Parameters:                                                                                                
 *      value  - (IN)  Data.                                                                                  
 *      buffer - (OUT) Data buffer.                                                                           
 * Returns:                                                                                                   
 *      None.                                                                                                 
 * Notes:                                                                                                     
 */
void
_shr_uint32_write(
    uint8* buffer,
    const uint32 value)
{
    buffer[0] = ((value >> 24) & 0xff);
    buffer[1] = ((value >> 16) & 0xff);
    buffer[2] = ((value >> 8) & 0xff);
    buffer[3] = ((value) & 0xff);
}


/*                                                                                                            
 * Function:                                                                                                  
 *      _shr_uint64_read                                                                                  
 * Purpose:                                                                                                   
 *      Read a 64-bit unsigned integer from a buffer in network byte order.                                   
 * Parameters:                                                                                                
 *      buffer - (IN) Data buffer.                                                                            
 * Returns:                                                                                                   
 *      Result.                                                                                               
 * Notes:                                                                                                     
 */
uint64
_shr_uint64_read(uint8* buffer)
{
    uint64 val64;

    COMPILER_64_SET(val64,
                    (buffer[0] << 24) + (buffer[1] << 16) +
                    (buffer[2] << 8)  + buffer[3],
                    (buffer[4] << 24) + (buffer[5] << 16) +
                    (buffer[6] << 8)  + buffer[7]);
    return val64;
}


/*                                                                                                            
 * Function:                                                                                                  
 *      _shr_uint64_write                                                                                 
 * Purpose:                                                                                                   
 *      Write a 64-bit unsigned integer to a buffer in network byte order.                                    
 * Parameters:                                                                                                
 *      value  - (IN)  Data.                                                                                  
 *      buffer - (OUT) Data buffer.                                                                           
 * Returns:                                                                                                   
 *      None.                                                                                                 
 * Notes:                                                                                                     
 */
void
_shr_uint64_write(
    uint8* buffer,
    const uint64 value)
{
    uint32 low;
    uint32 high;

    COMPILER_64_TO_32_LO(low,value);
    COMPILER_64_TO_32_HI(high,value);

    buffer[0] = ((high >> 24) & 0xff);
    buffer[1] = ((high >> 16) & 0xff);
    buffer[2] = ((high >> 8) & 0xff);
    buffer[3] = ((high) & 0xff);
    buffer[4] = ((low >> 24) & 0xff);
    buffer[5] = ((low >> 16) & 0xff);
    buffer[6] = ((low >> 8) & 0xff);
    buffer[7] = ((low) & 0xff);
}


/*                                                                                                            
 * Function:                                                                                                  
 *      _shr_int64_read                                                                                   
 * Purpose:                                                                                                   
 *      Read a 64-bit signed integer from a buffer in network byte order.                                     
 * Parameters:                                                                                                
 *      buffer - (IN) Data buffer.                                                                            
 * Returns:                                                                                                   
 *      Result.                                                                                               
 * Notes:                                                                                                     
 *      Casting to and from int64_t is not available on some platforms                                        
 */
int64
_shr_int64_read(uint8* buffer)
{
    int64 val;
    int64 temp;

    COMPILER_64_SET(temp, buffer[0] << 24, 0);
    val = temp;
    COMPILER_64_SET(temp, buffer[1] << 16, 0);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, buffer[2] << 8, 0);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, buffer[3], 0);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[4] << 24);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[5] << 16);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[6] << 8);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[7]);
    COMPILER_64_ADD_64(val, temp);

    return val;
}


/*                                                                                                            
 * Function:                                                                                                  
 *      _shr_int64_write                                                                                  
 * Purpose:                                                                                                   
 *      Write a 64-bit signed integer to a buffer in network byte order.                                      
 * Parameters:                                                                                                
 *      value  - (IN)  Data.                                                                                  
 *      buffer - (OUT) Data buffer.                                                                           
 * Returns:                                                                                                   
 *      None.                                                                                                 
 * Notes:                                                                                                     
 *      Casting to and from int64_t is not available on some platforms                                        
 */
void
_shr_int64_write(
    uint8* buffer,
    const int64 value)
{
    buffer[0] = (COMPILER_64_HI(value) >> 24) & 0xff;
    buffer[1] = (COMPILER_64_HI(value) >> 16) & 0xff;
    buffer[2] = (COMPILER_64_HI(value) >> 8) & 0xff;
    buffer[3] = (COMPILER_64_HI(value) & 0xff);
    buffer[4] = (COMPILER_64_LO(value) >> 24) & 0xff;
    buffer[5] = (COMPILER_64_LO(value) >> 16) & 0xff;
    buffer[6] = (COMPILER_64_LO(value) >> 8) & 0xff;
    buffer[7] = (COMPILER_64_LO(value) & 0xff);
}
