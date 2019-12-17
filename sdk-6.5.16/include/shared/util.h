/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * General utility routines
 */

#ifndef _SHR_UTIL_H
#define _SHR_UTIL_H

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/dpc.h>
#include <sal/core/sync.h>

extern unsigned short _shr_swap16(unsigned short val);
extern unsigned int _shr_swap32(unsigned int val);

extern int _shr_popcount(unsigned int);

extern uint8 _shr_bit_rev8(uint8 n);
extern uint16 _shr_bit_rev16(uint16 n);
extern uint32 _shr_bit_rev32(uint32 n);
extern uint32 _shr_bit_rev_by_byte_word32(uint32 n);

extern unsigned short _shr_crc16(int start, unsigned char *p, int n);
extern unsigned short _shr_crc16b(int crc, unsigned char *data, int nbits);
extern unsigned short _shr_crc16bd15(int crc, unsigned char *data, int nbits);

#define _SHR_CRC32_CORRECT 0xe320bbde
extern unsigned int _shr_crc32(unsigned int accum, unsigned char *data, 
                               int len);
unsigned int _shr_crc32b(unsigned int crc, unsigned char *data, int nbits);
unsigned int _shr_crc32bd15(unsigned int crc, unsigned char *data, int nbits);

uint16       _shr_crc16_ccitt(int crc, unsigned char *data, int len);
uint32       _shr_crc32_castagnoli(unsigned int crc, unsigned char *data, int len);
uint32       _shr_crc32_ethernet(unsigned int crc, unsigned char *data, int len);
uint32       _shr_crc32_koopman(unsigned int crc, unsigned char *data, int len);

uint16       _shr_crc16_draco_array(uint32 *hash_words, int n);
uint16       _shr_crc16_ccitt_array(uint32 *hash_words, int n);
uint32       _shr_crc32_castagnoli_array(uint32 *hash_words, int n);
uint32       _shr_crc32_ethernet_array(uint32 *hash_words, int n);
uint32       _shr_crc32_koopman_array(uint32 *hash_words, int n);

void _shr_sort(void *base, int count, int size,
	       int (*compar)(void *, void *));
int _shr_bsearch(void *base, int count, int size, 
                 void *target, int (*compar)(void *, void *));

void _shr_format_integer(char *buf, unsigned int n, int min_digits, int base);
void _shr_format_long_integer(char *buf, unsigned int *val, int nval);

void _shr_format_uint64_hexa_string(uint64 value, char* uint64_hexa_string);

int  _shr_isxdigit(char c);

extern unsigned int _shr_ctoi(const char *s);
extern sal_vaddr_t _shr_ctoa(const char *s);

extern int _shr_div_exp10(int d1, int d2, int exp10);
extern int _shr_atof_exp10(const char *s, int exp10);

extern uint32 _shr_div32r(uint32 d1, uint32 d2);
extern char *_shr_scale_uint64(uint64 d64, int base, int prec, uint32 *d32);

extern unsigned short _shr_ip_chksum(unsigned int length, unsigned char *data);

/* WARNING:  SIDE EFFECTS; AVOID EXPRESSIONS IN THESE MACROS */


/* unsigned char *buf, unsigned short val */
#define _SHR_PACK_SHORT(buf, val) \
    do {                                               \
        (buf)[0] = (val) >> 8;                         \
        (buf)[1] = (val);                              \
    } while (0)

/* unsigned char *buf, unsigned long val */
#define _SHR_PACK_LONG(buf, val) \
    do {                                               \
        (buf)[0] = (val) >> 24;                        \
        (buf)[1] = (val) >> 16;                        \
        (buf)[2] = (val) >> 8;                         \
        (buf)[3] = (val);                              \
    } while (0)

/* unsigned char *buf, unsigned short val */
#define _SHR_UNPACK_SHORT(buf, val) (val) = (((buf)[0] << 8) | (buf)[1])

/* unsigned char *buf, unsigned long val */
#define _SHR_UNPACK_LONG(buf, val) \
    (val) = (((buf)[0] << 24) | ((buf)[1] << 16) | ((buf)[2] << 8) | (buf)[3])

int _shr_parse_macaddr(char *str, uint8 *macaddr);
int _shr_parse_ipaddr(char *s, sal_ip_addr_t *ipaddr);

uint16 _shr_uint16_read(uint8* buffer);
void   _shr_uint16_write(uint8* buffer, const uint16 value);

uint32 _shr_uint32_read(uint8* buffer);
void _shr_uint32_write(uint8* buffer, const uint32 value);

uint64 _shr_uint64_read(uint8* buffer);
void _shr_uint64_write(uint8* buffer, const uint64 value);

int64 _shr_int64_read(uint8* buffer);
void _shr_int64_write(uint8* buffer, const int64 value);

/* To check if particular flag is set */
#define _SHR_IS_FLAG_SET(flags, flag_bit)        (((flags) & (flag_bit)) ? 1 : 0)

/*
 * XOR two booleans.
 */
#define _SHR_XOR_BOOL(a, b) ((a && !b) || (!a && b))
/* Repeating Delayed Procedure Call:
 *   Wrapper on top of SAL DPC layer to provide a safe mechanism for
 *   scheduling / unscheduling repeated calls to a function.
 */

/* shr_rdpc_fn_t should return the time interval until the next call, or 0 to indicate it should stop */
typedef sal_usecs_t (*shr_rdpc_fn_t)(void **, void **, void **, void **);

typedef struct shr_rdpc_s {
    shr_rdpc_fn_t func;
    sal_mutex_t call_count_lock;
    int run_count;
    int running;
} shr_rdpc_t;

extern int shr_rdpc_callback_create(shr_rdpc_t *rdpc, shr_rdpc_fn_t func);
extern int shr_rdpc_callback_created(shr_rdpc_t *rdpc);
extern int shr_rdpc_callback_start(shr_rdpc_t *rdpc, sal_usecs_t first_interval, void *, void *, void *, void *);
extern int shr_rdpc_callback_stop(shr_rdpc_t *rdpc);
extern int shr_rdpc_callback_finished(shr_rdpc_t *rdpc);
extern int shr_rdpc_callback_destroy(shr_rdpc_t *rdpc);


#endif	/* !_SHR_UTIL_H */
