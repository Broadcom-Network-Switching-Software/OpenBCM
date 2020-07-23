/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	meter.c
 * Purpose: 	Firebolt placeholder for metering, FP metering.
 */

#include <sal/types.h>
#include <sal/core/libc.h>

#include <shared/bitop.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/counter.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>

#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>


#define FB_NUM_BUCKET_SIZES       16
#define FB_NUM_VALID_BUCKET_SIZES 14

/* For Firebolt, every token in the bucket is worth one bit. */
/* For metering purposes, kbits means 1000 bits.             */
static uint32 fb_bucket_settings[FB_NUM_BUCKET_SIZES] = {
  0,                /*  Disabled   */
  0x00008000/1000,  /*  32K tokens */
  0x00010000/1000,  /*  64K tokens */
  0x00020000/1000,  /* 128K tokens */
  0x00040000/1000,  /* 256K tokens */
  0x00080000/1000,  /* 512K tokens */
  0x00100000/1000,  /*   1M tokens */
  0x00200000/1000,  /*   2M tokens */
  0x00400000/1000,  /*   4M tokens */
  0x00800000/1000,  /*   8M tokens */
  0x01000000/1000,  /*  16M tokens */
  0x02000000/1000,  /*  32M tokens */
  0x04000000/1000,  /*  64M tokens */
  0x08000000/1000,  /* 128M tokens */
  0,                /*     N/A     */
  0                 /*     N/A     */
};

/*
 * Function:
 *	_bcm_fb_kbits_to_bucketsize
 * Description:
 *      Routine that converts desired burst size into Firebolt style
 *      leaky bucketsize register value.
 * Parameters:
 *      kbits_burst - Desired size of maximum burst traffic
 * Returns:
 *	Register bits for BUCKETSIZEf field. 
 * Notes:
 *	None.
 */

uint8
_bcm_fb_kbits_to_bucketsize(uint32 kbits_burst)
{
    uint8 i;
    for (i=0; i< FB_NUM_VALID_BUCKET_SIZES; i++) {
        if (kbits_burst <= fb_bucket_settings[i])
	    break; 
    }
    if (i == FB_NUM_VALID_BUCKET_SIZES)
        i--;
    return i;
}

/*
 * Function:
 *	_bcm_fb_bucketsize_to_kbits
 * Description:
 *      Routine that converts Firebolt style leaky bucketsize 
 *      register value into its associated burst size.
 * Parameters:
 *      reg_val - BUCKETSIZEf field value
 * Returns:
 *	Max size burst traffic in kbits. 
 * Notes:
 *	None.
 */

uint32
_bcm_fb_bucketsize_to_kbits(uint8 reg_val)
{
    return fb_bucket_settings[reg_val & 0x0F];
}

#if defined (BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
    defined (BCM_TRX_SUPPORT)
#define FB2_NUM_BUCKET_RANGES           16
#define FB2_BUCKET_MIN_BITS             (4 * 1024 * 8) /* 4K bytes */
#define FB2_BUCKET_SEGMENT_GRANULARITY  256
#define FB2_BUCKET_SEGMENT_MASK         0xff
#define FB2_BUCKET_POWER_SHIFT          8
#define FB2_BUCKET_POWER_MASK           0xf
#define FB2_BUCKET_ENCODED_MAX          0xfff

/*
 * Function:
 *	_bcm_fb2_kbits_to_bucketsize
 * Description:
 *      Routine that converts desired burst size into Firebolt2 style
 *      leaky bucketsize register value.
 * Parameters:
 *      kbits_burst - Desired size of maximum burst traffic
 * Returns:
 *	Register bits for BUCKETSIZEf field. 
 * Notes:
 *	None.
 */

uint32
_bcm_fb2_kbits_to_bucket_encoding(uint32 kbits_burst, int linear_mode)
{
    uint32 i;
    uint32 bits, encoding, bucketsize, bucket_segment_size;

    if (linear_mode) {
        encoding = (((kbits_burst * 1000) + (FB2_BUCKET_MIN_BITS - 1)) /
                FB2_BUCKET_MIN_BITS);
        if (encoding > FB2_BUCKET_ENCODED_MAX) {
            encoding = FB2_BUCKET_ENCODED_MAX;
        }
        return encoding;
    } else {
        encoding = kbits_burst ? 1 : 0; /* 0 kbits means disable */
        bucketsize = FB2_BUCKET_MIN_BITS;
        bits = kbits_burst * 1000;

        for (i=0; i<= FB2_NUM_BUCKET_RANGES; i++, bucketsize *= 2) {
            if (bits <= bucketsize) {
                if (i > 0) {
                    bucket_segment_size =
                        (1 << (i - 1)) * FB2_BUCKET_MIN_BITS /
                        FB2_BUCKET_SEGMENT_GRANULARITY;
                    encoding =
                        (bits - (bucketsize / 2) +
                         (bucket_segment_size - 1)) / bucket_segment_size;
                    encoding |= (i - 1) << FB2_BUCKET_POWER_SHIFT;
                }
                break;
            }
        }

        return (i < FB2_NUM_BUCKET_RANGES) ? encoding :
            FB2_BUCKET_ENCODED_MAX;
    }
}

/*
 * Function:
 *	_bcm_fb2_bucketsize_to_kbits
 * Description:
 *      Routine that converts Firebolt2 style leaky bucketsize 
 *      register value into its associated burst size.
 * Parameters:
 *      reg_val - BUCKETSIZEf field value
 * Returns:
 *	Max size burst traffic in kbits. 
 * Notes:
 *	None.
 */

uint32
_bcm_fb2_bucket_encoding_to_kbits(uint32 reg_val, int linear_mode)
{
    uint32 power, segment, bucketsize;

    if (linear_mode) {
        return ((reg_val * FB2_BUCKET_MIN_BITS) / 1000);
    } else {
        segment = reg_val & FB2_BUCKET_SEGMENT_MASK;
        power = (reg_val >> FB2_BUCKET_POWER_SHIFT) & FB2_BUCKET_POWER_MASK;

        /* Calculate raw bits */
        bucketsize = (1 << power) *
            (FB2_BUCKET_MIN_BITS / FB2_BUCKET_SEGMENT_GRANULARITY) *
            (FB2_BUCKET_SEGMENT_GRANULARITY + segment);

    
        /* bits to kbits */
        return (bucketsize + (1000 - 1)) / 1000;
    }
}

#define METER_GRANULARITY_DEFAULT       3
#define METER_GRANULARITY_NUM           8
#define METER_KBITS_SEC_QUANTUM_MIN     8       /* 8000 bits/second */
#define METER_BITS_BURST_MIN           (1024 * 8 / 2) /* 1/2 Kbytes */
#define METER_PACKET_SEC_QUANTUM_MIN    1
#define METER_PACKET_MAX_GRAN_FP_EXTRA_MULTIPLE   4 /* 2 extra steps */
#define METER_PACKET_MAX_GRAN_MMU_EXTRA_MULTIPLE  2 /* 1 extra step */
#define METER_PACKET_BURST_MIN          512     /* 0.512 packet, use 1000x */
#define METER_PACKET_BURST_DIVISOR      1000    /* use 1000x for calculation */
#define METER_NL_SEGMENT_GRANULARITY    256
#define METER_NL_BUCKET_POWER_SHIFT     8
#define METER_NL_BUCKET_RANGE_NUM       16
#define METER_NL_BUCKET_SEGMENT_MASK    0xff
#define METER_NL_BUCKET_POWER_MASK      0xf
#define METER_NL_BUCKET_MAC_ENCODE_MAX  0xff80

STATIC void
_bcm_xgs_granularity_params(int granularity, uint32 flags,
                             uint32 *rate_quantum, uint32 *burst_quantum)
{
    int ix;
    uint32 gran_multiple = 1;

    for (ix = 0; ix < granularity; ix++) {
        gran_multiple *= 2;
    }

    if (flags & _BCM_XGS_METER_FLAG_PACKET_MODE) {
        if (granularity == (METER_GRANULARITY_NUM - 1)) {
            if (flags &_BCM_XGS_METER_FLAG_FP_POLICER) {
                if (!(flags & _BCM_XGS_METER_FLAG_FP_TD2_POLICER)) {
                   gran_multiple *= METER_PACKET_MAX_GRAN_FP_EXTRA_MULTIPLE;
                }
            } else {
                gran_multiple *= METER_PACKET_MAX_GRAN_MMU_EXTRA_MULTIPLE;
            }
        }
        if ((flags & _BCM_XGS_METER_FLAG_FP_TD2_POLICER)
             && (flags & _BCM_XGS_METER_FLAG_FP_POLICER)) {
            if ((granularity >= 5)  && (granularity <=7)) {
                for (ix = 0; ix <= (granularity - 5); ix++) {
                     gran_multiple = gran_multiple * 2;
                }
            }
        }
        *rate_quantum = METER_PACKET_SEC_QUANTUM_MIN * gran_multiple;
        *burst_quantum = METER_PACKET_BURST_MIN * gran_multiple;

    } else {
        *rate_quantum = METER_KBITS_SEC_QUANTUM_MIN * gran_multiple;
        *burst_quantum = METER_BITS_BURST_MIN * gran_multiple;
    }

    /*
     * On device with double refresh frequency (double the number of refresh
     * per second), granularity is half for burst and (per refresh) rate.
     */
    if (flags & _BCM_XGS_METER_FLAG_DOUBLE_REFRESH_FREQ) {
        *burst_quantum /= 2;
    }

    if (flags & _BCM_XGS_METER_FLAG_BUCKET_IN_8KB) { 
        if (granularity == 5) {
            *burst_quantum /= 2;
        }
    }
    /* With Refresh rate of 15.625us,
     * Minimum Quantum (g=0) | Byte Mode |   Pkt Mode |
     *   Rate                |  4 kbps   |    0.5 pps |
     *   Burst               | 512 bytes | 0.512 pkts |
     * So, calculated rate and burst quantum needs to be adjusted
     */
    if (flags & _BCM_XGS_METER_FLAG_REFRESH_RATE_15p625) {
        *rate_quantum /= 2;
        *burst_quantum /= 8;
    }
}

/*
 * Function:
 *	_bcm_xgs_kbits_to_bucket_encoding
 * Description:
 *      Routine that converts desired burst size into Firebolt2 style
 *      leaky bucketsize register value.
 * Parameters:
 *      kbits_sec - Desired rate of ongoing traffic
 *      kbits_burst - Desired size of maximum burst traffic
 *      flags - paramters indicating capabilities and configuration of meter
 *      refresh_bitsize - number of bits for the refresh rate field
 *      bucket_max_bitsize - number of bits for the bucket max field
 *      refresh_rate - (OUT) calculated value for the refresh rate field
 *      bucket_max - (OUT) calculated value for the bucket max field
 *      granularity - (OUT) calculated value for the granularity field
 * Returns:
 *	BCM_E_*
 * Notes:
 *
 * bandwidth-shaping mode
 * gran  refresh      refresh            bucket-size-granularity
 *       byte/refresh kbit/second        bits
 *                     min <-> max         base <-> max
 *                                                  (linear)   / (ITU mode)
 * 0     1/128           8 <-> 0x1ffff8    4096 <-> 0xfff000   / 0xff80000
 * 1     1/64           16 <-> 0x3ffff0    8192 <-> 0x1ffe000  / 0x1ff00000
 * 2     1/32           32 <-> 0x7fffe0   16384 <-> 0x3ffc000  / 0x3fe00000
 * 3     1/16           64 <-> 0xffffc0   32768 <-> 0x7ff8000  / 0x7fc00000
 * 4     1/8           128 <-> 0x1ffff80  65536 <-> 0xfff0000  / 0xff800000
 * 5     1/4           256 <-> 0x3ffff00 131072 <-> 0x1ffe0000 / 0x1ff000000
 * 6     1/2           512 <-> 0x7fffe00 262144 <-> 0x3ffc0000 / 0x3fe000000
 * 7     1            1024 <-> 0xffffc00 524288 <-> 0x7ff80000 / 0x7fc000000
 * (max value is based on 18-bit refresh setting and 12-bit burst setting)
 *
 * packet-shaping mode (for mmu)
 * gran  refresh        refresh           bucket-size-granularity
 *       packet/refresh packet/second     packet
 *                      min <-> max          base <-> max
 *                                                    (linear) / (ITU mode)
 * 0     1/128000         1 <-> 0x3ffff     0.512 <-> 0x830    / 0x82d0
 * 1     1/64000          2 <-> 0x7fffe     1.024 <-> 0x1061   / 0x105a1
 * 2     1/32000          4 <-> 0xffffc     2.048 <-> 0x20c2   / 0x20b43
 * 3     1/16000          8 <-> 0x1ffff8    4.096 <-> 0x4185   / 0x41687
 * 4     1/8000          16 <-> 0x3ffff0    8.192 <-> 0x830a   / 0x82d0e
 * 5     1/4000          32 <-> 0x7fffe0   16.384 <-> 0x10614  / 0x105a1c
 * 6     1/2000          64 <-> 0xffffc0   32.768 <-> 0x20c28  / 0x20b439
 * 7     1/500          256 <-> 0x3ffff00 131.072 <-> 0x830a3  / 0x82d0e5
 *
 * packet-shaping mode (for fp)
 * gran  refresh        refresh           bucket-size-granularity
 *       packet/refresh packet/second     packet
 *                      min <-> max          base <-> max
 *                                                    (linear) / (ITU mode)
 * 0     1/128000         1 <-> 0x7ffff     0.512 <-> 0x830    / 0x82d0
 * 1     1/64000          2 <-> 0xffffe     1.024 <-> 0x1061   / 0x105a1
 * 2     1/32000          4 <-> 0x1ffffc    2.048 <-> 0x20c2   / 0x20b43
 * 3     1/16000          8 <-> 0x3ffff8    4.096 <-> 0x4185   / 0x41687
 * 4     1/8000          16 <-> 0x7ffff0    8.192 <-> 0x830a   / 0x82d0e
 * 5     1/4000          32 <-> 0xffffe0   16.384 <-> 0x10614  / 0x105a1c
 * 6     1/2000          64 <-> 0x1ffffc0  32.768 <-> 0x20c28  / 0x20b439
 * 7     1/250          512 <-> 0xffffe00 262.144 <-> 0x106147 / 0x105a1ca
 */

int
_bcm_xgs_kbits_to_bucket_encoding(uint32 kbits_sec, uint32 kbits_burst,
                                  uint32 flags,
                                  int refresh_bitsize,
                                  int bucket_max_bitsize,
                                  uint32 *refresh_rate, uint32 *bucketsize,
                                  uint32 *granularity)
{
    int gran, gran_min, gran_max;
    uint32 i;
    uint32 burst, encoding, buckettop, bucket_segment_size;
    uint32 refresh_mask = 0;
    uint32 bucket_mask = 0;
    uint32 refresh_max = 0; 
    uint32 bucket_max = 0;
    uint32 rate_quantum = 0;
    uint32 burst_quantum = 0;
    uint32 gran_select = 0;

    if ((refresh_rate == NULL) || (bucketsize == NULL)) {
        return BCM_E_INTERNAL;
    }

    if ((kbits_burst == 0) &&
        (!(flags & _BCM_XGS_METER_FLAG_FP_POLICER) && (kbits_sec == 0))) {
        /* Disabled */
        *refresh_rate = 0;
        *bucketsize = 0;
        *granularity = METER_GRANULARITY_DEFAULT;
        return BCM_E_NONE;
    }

    refresh_mask = 0xffffffff >> (32 - refresh_bitsize);
    bucket_mask = 0xffffffff >> (32 - bucket_max_bitsize);
    if (flags & _BCM_XGS_METER_FLAG_PACKET_MODE) {
        /* Minimum packet burst is .512 packet.  We'll work in integers. */
        burst = kbits_burst * METER_PACKET_BURST_DIVISOR;
    } else {
        if (kbits_burst > (0xffffffff / 1000)) {
            burst = 0xffffffff;
        } else {
            burst = kbits_burst * 1000;
        }
    }

    if (flags & _BCM_XGS_METER_FLAG_GRANULARITY) {
        gran_min = 0;
        gran_max = METER_GRANULARITY_NUM - 1;
    } else if  (flags & _BCM_XGS_METER_FLAG_GRANULARITY_SELECTIVE) {
        gran_select = (flags & _BCM_XGS_METER_FLAG_GRANULARITY_SELECT_MASK);
        gran_select >>= _BCM_XGS_METER_FLAG_GRANULARITY_SELECT_SHIFT;
        
        gran_min = gran_max = -1;
        for (i = 0; i < METER_GRANULARITY_NUM; i++) {
            if ((gran_select & 0x1) && (gran_min < 0)) {
                gran_min = i;
            }
            if (gran_select & 0x1) {
                gran_max = i;
            }
            gran_select >>= 1;
        }
        
        /* Can't get min ang max gran, set to default */
        if ((gran_min < 0) || (gran_max < 0)) {
            gran_min = gran_max = METER_GRANULARITY_DEFAULT;
        }
    } else {
        gran_min = gran_max = METER_GRANULARITY_DEFAULT;
    }

    for (gran = gran_min; gran <= gran_max; gran++) {
        if	(flags & _BCM_XGS_METER_FLAG_GRANULARITY_SELECTIVE) {
            gran_select = (flags & _BCM_XGS_METER_FLAG_GRANULARITY_SELECT_MASK);
            gran_select >>= _BCM_XGS_METER_FLAG_GRANULARITY_SELECT_SHIFT;
            if (!((1 << gran) & gran_select)) {
                continue;
            }
        }

        _bcm_xgs_granularity_params(gran, flags,
                                     &rate_quantum, &burst_quantum);
        refresh_max = refresh_mask * rate_quantum;
        if (flags & _BCM_XGS_METER_FLAG_NON_LINEAR) {
            bucket_max = METER_NL_BUCKET_MAC_ENCODE_MAX * burst_quantum;
        } else {
            bucket_max = bucket_mask * burst_quantum;
        }

        if ((kbits_sec <= refresh_max) && (burst <= bucket_max)) {
            break;
        }
    }

    if (gran > gran_max) {
        /* Saturate! */
        gran = gran_max;
        if (kbits_sec > refresh_max) {
            kbits_sec = refresh_max;
        }
        if (burst > bucket_max) {
            burst = bucket_max;
        }
    }

    *granularity = gran;

    if (kbits_sec > (0xffffffff - (rate_quantum - 1))) {
        kbits_sec = 0xffffffff - (rate_quantum - 1);
    }

    if (!rate_quantum) {
        /* avoid un-expected rate_quantum */
        return BCM_E_INTERNAL;
    }

    *refresh_rate = (kbits_sec + (rate_quantum - 1)) / rate_quantum;
    if (*refresh_rate > refresh_mask) {
        *refresh_rate = refresh_mask;
    }        

    if (flags & _BCM_XGS_METER_FLAG_NON_LINEAR) {
        if (burst <= burst_quantum) {
            *bucketsize = burst ? 1 : 0; /* 0 kbits means disable */
        } else {
            encoding = bucket_mask;
            buckettop = burst_quantum;
            for (i = 0; i < METER_NL_BUCKET_RANGE_NUM; i++, buckettop *= 2) {
                bucket_segment_size =
                    buckettop / METER_NL_SEGMENT_GRANULARITY;
                if (burst <= (buckettop * 2 - bucket_segment_size) ||
                    (buckettop == 0)) {
                    if (buckettop == 0) {
                        /* Saturate */
                        buckettop = 0xffffffff;
                    }
                    encoding =
                        (burst - buckettop + (bucket_segment_size - 1)) /
                        bucket_segment_size;
                    encoding |= i << METER_NL_BUCKET_POWER_SHIFT;
                    break;
                }
            }
            *bucketsize = (i < METER_NL_BUCKET_RANGE_NUM) ? encoding :
                bucket_mask;
        }
    } else {
        encoding = (burst + (burst_quantum - 1)) / burst_quantum;
        if (encoding > bucket_mask) {
            encoding = bucket_mask;
        }
        *bucketsize = encoding;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_xgs_bucket_encoding_to_kbits
 * Description:
 *      Routine that converts Firebolt2 style leaky bucketsize 
 *      register value into its associated burst size.
 * Parameters:
 *      refresh_rate - refresh rate field
 *      bucket_max - bucket max field
 *      granularity - granularity field
 *      flags - paramters indicating capabilities and configuration of meter
 *      kbits_sec - (OUT) Configured rate of ongoing traffic
 *      kbits_burst - (OUT) Configured size of maximum burst traffic
 * Returns:
 *	Max size burst traffic in kbits. 
 * Notes:
 *      None.
 */

uint32
_bcm_xgs_bucket_encoding_to_kbits(uint32 refresh_rate, uint32 bucket_max,
                                  uint32 granularity, uint32 flags,
                                  uint32 *kbits_sec, uint32 *kbits_burst)
{
    int gran;
    uint32 power, segment, bucketsize;
    uint32 rate_quantum, burst_quantum;

    if ((kbits_sec == NULL) || (kbits_burst == NULL)) {
        return BCM_E_INTERNAL;
    }

    gran = (flags & _BCM_XGS_METER_FLAG_GRANULARITY) ? granularity :
        METER_GRANULARITY_DEFAULT;
    _bcm_xgs_granularity_params(gran, flags,
                                 &rate_quantum, &burst_quantum);

    *kbits_sec = refresh_rate * rate_quantum;

    if (flags & _BCM_XGS_METER_FLAG_NON_LINEAR) {
        if (bucket_max == 0) { /* 0 means disabled */
            bucketsize = 0;
        } else {
            segment = bucket_max & METER_NL_BUCKET_SEGMENT_MASK;
            power = (bucket_max >> METER_NL_BUCKET_POWER_SHIFT) &
                METER_NL_BUCKET_POWER_MASK;

            /* Calculate raw bits */
            bucketsize = (1 << power) *
                (burst_quantum / METER_NL_SEGMENT_GRANULARITY) *
                (METER_NL_SEGMENT_GRANULARITY + segment);
        }
    } else {
        bucketsize = bucket_max * burst_quantum;
    }

    if (flags & _BCM_XGS_METER_FLAG_PACKET_MODE) {
        *kbits_burst = bucketsize / METER_PACKET_BURST_DIVISOR;
    } else {
        *kbits_burst = bucketsize / 1000;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_xgs_kbits_to_dual_bucket_encoding
 * Description:
 *      Routine that converts desired burst size into TRX style
 *      leaky bucketsize register value for dual token bucket policer.
 * Parameters:
 *      kbits_sec - Desired rate of ongoing traffic
 *      kbits_burst - Desired size of maximum burst traffic
 *      flags - paramters indicating capabilities and configuration of meter
 *      refresh_bitsize - number of bits for the refresh rate field
 *      bucket_max_bitsize - number of bits for the bucket max field
 *      granularity_start - granularity start value to be used for computation
 *      refresh_rate - (OUT) calculated value for the refresh rate field
 *      bucket_max - (OUT) calculated value for the bucket max field
 *      granularity - (OUT) calculated value for the granularity field
 * Returns:
 *	BCM_E_*
 */
int
_bcm_xgs_kbits_to_dual_bucket_encoding(uint32 kbits_sec, uint32 kbits_burst,
                                       uint32 flags,
                                       int refresh_bitsize,
                                       int bucket_max_bitsize,
                                       int granularity_start,
                                       uint32 *refresh_rate, uint32 *bucketsize,
                                       uint32 *granularity)
{
    int gran, gran_min = 0, gran_max;
    uint32 burst, encoding;
    uint32 refresh_mask = 0;
    uint32 bucket_mask = 0;
    uint32 refresh_max = 0; 
    uint32 bucket_max = 0;
    uint32 rate_quantum = 0;
    uint32 burst_quantum = METER_PACKET_BURST_MIN;

    if ((refresh_rate == NULL) || (bucketsize == NULL)) {
        return BCM_E_INTERNAL;
    }

    if ((kbits_burst == 0) &&
        (!(flags & _BCM_XGS_METER_FLAG_FP_POLICER) && (kbits_sec == 0))) {
        /* Disabled */
        *refresh_rate = 0;
        *bucketsize = 0;
        *granularity = METER_GRANULARITY_DEFAULT;
        return BCM_E_NONE;
    }

    refresh_mask = 0xffffffff >> (32 - refresh_bitsize);
    bucket_mask = 0xffffffff >> (32 - bucket_max_bitsize);
    if (flags & _BCM_XGS_METER_FLAG_PACKET_MODE) {
        /* Minimum packet burst is .512 packet.  We'll work in integers. */
        burst = kbits_burst * METER_PACKET_BURST_DIVISOR;
    } else {
        if (kbits_burst > (0xffffffff / 1000)) {
            burst = 0xffffffff;
        } else {
            burst = kbits_burst * 1000;
        }
    }

    gran_min = granularity_start;
    gran_max = METER_GRANULARITY_NUM - 1;

    for (gran = gran_min; gran <= gran_max; gran++) {
        _bcm_xgs_granularity_params(gran, flags,
                                     &rate_quantum, &burst_quantum);
        refresh_max = refresh_mask * rate_quantum;
        bucket_max = bucket_mask * burst_quantum;

        if ((kbits_sec <= refresh_max) && (burst <= bucket_max)) {
            break;
        }
    }

    if (gran > gran_max) {
        /* Saturate! */
        gran = gran_max;
        if (kbits_sec > refresh_max) {
            kbits_sec = refresh_max;
        }
        if (burst > bucket_max) {
            burst = bucket_max;
        }
    }

    *granularity = gran;

    if (kbits_sec > (0xffffffff - (rate_quantum - 1))) {
        kbits_sec = 0xffffffff - (rate_quantum - 1);
    }

    /* Coverity fix : Making sure rate_quantum is above zero in order
    to avoid divide by zero coverity error */
    if (rate_quantum > 0) {
        *refresh_rate = (kbits_sec + (rate_quantum - 1)) / rate_quantum;
    }
    if (*refresh_rate > refresh_mask) {
        *refresh_rate = refresh_mask;
    }        

    encoding = (burst + (burst_quantum - 1)) / burst_quantum;
    if (encoding > bucket_mask) {
        encoding = bucket_mask;
    }
    *bucketsize = encoding;
    return BCM_E_NONE;
}
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRIUMPH_SUPPORT */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT)
int
_bcm_xgs_kbits_to_bucket_encoding_with_granularity(uint32 kbits_sec, 
                                  uint32 kbits_burst,
                                  uint32 flags,
                                  int refresh_bitsize,
                                  int bucket_max_bitsize,
                                  uint32 *refresh_rate, uint32 *bucketsize,
                                  uint32 *granularity)
{
    uint32 i;
    uint32 burst, encoding, buckettop, bucket_segment_size;
    uint32 refresh_mask = 0;
    uint32 bucket_mask = 0;
    uint32 rate_quantum = 0;
    uint32 burst_quantum = 0;

    if ((refresh_rate == NULL) || (bucketsize == NULL)) {
        return BCM_E_INTERNAL;
    }

    if ((kbits_burst == 0) &&
        (!(flags & _BCM_XGS_METER_FLAG_FP_POLICER) && (kbits_sec == 0))) {
        /* Disabled */
        *refresh_rate = 0;
        *bucketsize = 0;
        *granularity = METER_GRANULARITY_DEFAULT;
        return BCM_E_NONE;
    }

    refresh_mask = 0xffffffff >> (32 - refresh_bitsize);
    bucket_mask = 0xffffffff >> (32 - bucket_max_bitsize);
    if (flags & _BCM_XGS_METER_FLAG_PACKET_MODE) {
        /* Minimum packet burst is .512 packet.  We'll work in integers. */
        burst = kbits_burst * METER_PACKET_BURST_DIVISOR;
    } else {
        if (kbits_burst > (0xffffffff / 1000)) {
            burst = 0xffffffff;
        } else {
            burst = kbits_burst * 1000;
        }
    }

    _bcm_xgs_granularity_params(*granularity, flags,
                                     &rate_quantum, &burst_quantum);

    if (kbits_sec > (0xffffffff - (rate_quantum - 1))) {
        kbits_sec = 0xffffffff - (rate_quantum - 1);
    }

    *refresh_rate = (kbits_sec + (rate_quantum - 1)) / rate_quantum;
    if (*refresh_rate > refresh_mask) {
        *refresh_rate = refresh_mask;
    }        

    if (flags & _BCM_XGS_METER_FLAG_NON_LINEAR) {
        if (burst <= burst_quantum) {
            *bucketsize = burst ? 1 : 0; /* 0 kbits means disable */
        } else {
            encoding = bucket_mask;
            buckettop = burst_quantum;
            for (i = 0; i < METER_NL_BUCKET_RANGE_NUM; i++, buckettop *= 2) {
                bucket_segment_size =
                    buckettop / METER_NL_SEGMENT_GRANULARITY;
                if (burst <= (buckettop * 2 - bucket_segment_size) ||
                    (buckettop == 0)) {
                    if (buckettop == 0) {
                        /* Saturate */
                        buckettop = 0xffffffff;
                    }
                    encoding =
                        (burst - buckettop + (bucket_segment_size - 1)) /
                        bucket_segment_size;
                    encoding |= i << METER_NL_BUCKET_POWER_SHIFT;
                    break;
                }
            }
            *bucketsize = (i < METER_NL_BUCKET_RANGE_NUM) ? encoding :
                bucket_mask;
        }
    } else {
        encoding = (burst + (burst_quantum - 1)) / burst_quantum;
        if (encoding > bucket_mask) {
            encoding = bucket_mask;
        }
        *bucketsize = encoding;
    }
    return BCM_E_NONE;
}
#endif /* BCM_KATANA_SUPPORT || BCM_TRIUMPH3_SUPPORT ||
          BCM_GREYHOUND2_SUPPORT */
