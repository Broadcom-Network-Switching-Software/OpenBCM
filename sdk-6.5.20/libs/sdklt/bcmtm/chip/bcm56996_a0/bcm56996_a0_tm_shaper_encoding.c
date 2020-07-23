/*! \file bcm56996_a0_tm_shaper_encoding.c
 *
 * TM Shaper rate encoding.
 * This converts the user readable values for bandwidth and burst size to
 * physical table readable format.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/generated/bcmtm_ha.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/sched_shaper/bcmtm_shaper_internal.h>
#include <bcmtm/bcmtm_shaper_encoding_internal.h>
#include "bcm56996_a0_tm_scheduler_shaper.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP

#define TH4G_METER_GRANULARITY_DEFAULT       3
#define TH4G_METER_GRANULARITY_NUM           8
/*! 32000 bits/second */
#define TH4G_METER_KBITS_SEC_QUANTUM_MIN     32
/*! 256 bytes */
#define TH4G_METER_BITS_BURST_MIN            (256 * 8)
#define TH4G_METER_PACKET_SEC_QUANTUM_MIN    2
/*! 0.256 packet, use 1000x */
#define TH4G_METER_MMU_PACKET_BURST_MIN      128
/*! 0.512 packet, use 1000x */
#define TH4G_METER_FP_PACKET_BURST_MIN       512
/*! use 1000x for calculation */
#define TH4G_METER_PACKET_BURST_DIVISOR      1000
#define TH4G_METER_NL_SEGMENT_GRANULARITY    256
#define TH4G_METER_NL_BUCKET_POWER_SHIFT     8
#define TH4G_METER_NL_BUCKET_RANGE_NUM       16
#define TH4G_METER_NL_BUCKET_SEGMENT_MASK    0xff
#define TH4G_METER_NL_BUCKET_POWER_MASK      0xf
/* 2 ** 0xf * (1 + 0xff/256) */
#define TH4G_METER_NL_BUCKET_MAC_ENCODE_MAX  0xff80
#define TH4G_REFRESH_MASK_SZ   15
#define TH4G_BUCKET_MASK_SZ    20

/* Select the minimum */
#define TH4G_MIN_SELECT(a, b) (a > b ? (a = b) : (a = a))

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief TM  rate unit size and burst unit size based on shaping mode.
 *
 * \param [in] unit           Unit number.
 * \param [in] granularity    Granularity.
 * \param [in] shaping_mode   Shaping mode(packet mode/ byte mode).
 * \param [out] rate_unit_sz  Rate unit size.
 * \param [out] burst_unit_sz Burst unit size.
 */
static void
bcm56996_a0_tm_granularity_params(int unit,
                                  int granularity,
                                  uint32_t shaping_mode,
                                  uint32_t *rate_unit_sz,
                                  uint32_t *burst_unit_sz)
{
    uint32_t gran_multiple;
    static const uint32_t bcmtm_pkt_mode_gran_multiple[] = {
        1, 2, 4, 16, 64, 256, 1024, 4096
    };

    if (shaping_mode != 0) {
        /* packet mode */
        gran_multiple = bcmtm_pkt_mode_gran_multiple[granularity];
        *burst_unit_sz = TH4G_METER_MMU_PACKET_BURST_MIN * gran_multiple;
        *rate_unit_sz = TH4G_METER_PACKET_SEC_QUANTUM_MIN * gran_multiple;
    } else {
        /* Byte mode */
        gran_multiple = 1 << granularity;
        *rate_unit_sz =  TH4G_METER_KBITS_SEC_QUANTUM_MIN * gran_multiple;
        *burst_unit_sz = TH4G_METER_BITS_BURST_MIN * gran_multiple;
    }
}

/*!
 * \brief TM calculate default burst size based on bandwidth.
 *
 * \param [in] unit       Unit number.
 * \param [in] lport      Logical port number.
 * \param [in] bandwidth  Bandwidth in Kbps.
 *
 * return Burst size in kbits.
 */
static int
bcm56996_a0_tm_default_burst_size(int unit,
                                  bcmtm_lport_t lport,
                                  uint32_t bandwidth)
{
#define TH4G_PORT_MTU    9216
    uint32_t max_port_speed;
    bcmtm_port_map_info_t *port_map;
    int proposed1, proposed2, proposed;
    int numq;

    bcmtm_port_map_get(unit, &port_map);
    max_port_speed = port_map->lport_map[lport].max_speed * 1000;

    numq = TH4G_TM_NUM_Q;

    /* Adjust loopback port max speed to 400G */
    if (bcmtm_lport_is_lb(unit, lport)) {
        max_port_speed = 400000 * 1000;
    }

    if (max_port_speed == 0)
        return 0;
    if (max_port_speed > bandwidth) {
        proposed1 = (TH4G_PORT_MTU * numq)/(max_port_speed/bandwidth);
    } else {
        if (((max_port_speed + bandwidth - 1) / bandwidth) == 0){
            return 0;
        }
        proposed1 = (TH4G_PORT_MTU * numq)/
                    ((max_port_speed + bandwidth - 1)/bandwidth);
    }
    proposed2 = bandwidth / (8 * 256);

    proposed = (proposed1 > proposed2) ? proposed1 : proposed2;
    proposed = (proposed * 8)/1000;

    return (proposed > 0) ? proposed : 1;
}

/*******************************************************************************
 * Public functions
 */
int
bcm56996_a0_tm_shaper_bucket_to_rate(int unit,
                                     uint32_t shaping_mode,
                                     bcmtm_shaper_bucket_encode_t *bucket_encode)
{
    uint32_t rate_unit_sz = 0, burst_unit_sz = 0;
    uint32_t power, segment, bucket_sz;
    uint64_t itu_mode;

    SHR_FUNC_ENTER(unit);
    bcm56996_a0_tm_granularity_params(unit, bucket_encode->granularity,
                                   shaping_mode, &rate_unit_sz, &burst_unit_sz);


    bucket_encode->bandwidth = (bucket_encode->refresh_rate) * rate_unit_sz;
    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, TM_SHAPER_CONFIGt,
                          TM_SHAPER_CONFIGt_ITU_MODEf, &itu_mode));
    if (itu_mode) {
        if (bucket_encode->bucket_sz == 0) {
            bucket_sz = 0;
        } else {
            segment =
                bucket_encode->bucket_sz & TH4G_METER_NL_BUCKET_SEGMENT_MASK;
            power =
                (bucket_encode->bucket_sz >> TH4G_METER_NL_BUCKET_POWER_SHIFT)
                & TH4G_METER_NL_BUCKET_POWER_MASK;
            /* Calculate raw bits */
            bucket_sz = (1 << power) *
                (burst_unit_sz / TH4G_METER_NL_SEGMENT_GRANULARITY) *
                (TH4G_METER_NL_SEGMENT_GRANULARITY + segment);
        }
    } else {
        bucket_sz = bucket_encode->bucket_sz * burst_unit_sz;
    }
    if (shaping_mode != 0) {
        /* packet mode */
        bucket_encode->burst = bucket_sz / TH4G_METER_PACKET_BURST_DIVISOR;
    } else {
        bucket_encode->burst = bucket_sz / 1000;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_shaper_rate_to_bucket(int unit,
                                     bcmtm_lport_t lport,
                                     uint32_t shaping_mode,
                                     uint8_t burst_auto,
                                     bcmtm_shaper_bucket_encode_t *bucket_encode)
{
    uint32_t refresh_mask = 0, bucket_mask = 0;
    uint32_t refresh_max = 0, bucket_max = 0;
    uint32_t refresh_unit_sz = 0, burst_unit_sz = 0;
    uint64_t itu_mode;
    uint32_t burst, gran, bucket_top, encoding;
    int bucket_segment_size, i = 0;
    uint32_t bandwidth = bucket_encode->bandwidth;
    uint32_t burst_size = bucket_encode->burst;

    SHR_FUNC_ENTER(unit);
    refresh_mask = 0xffffffff >> TH4G_REFRESH_MASK_SZ;
    bucket_mask = 0xffffffff >> TH4G_BUCKET_MASK_SZ;

    /* calculate burst rate based on bandwidth allocated */
    if (burst_auto) {
        burst_size = (bandwidth > 0) ?
            bcm56996_a0_tm_default_burst_size(unit, lport, bandwidth) : 0 ;
    }

    if ((burst_size  == 0) && (bandwidth == 0)) {
        bucket_encode->refresh_rate = 0;
        bucket_encode->bucket_sz = 0;
        bucket_encode->granularity = TH4G_METER_GRANULARITY_DEFAULT;
    }

    /* Packet mode */
    if (shaping_mode != 0) {
        burst = burst_size * TH4G_METER_PACKET_BURST_DIVISOR;
    } else {
        /* Byte mode */
        if (burst_size > (0xffffffff / 1000))
            burst = 0xffffffff;
        else
            burst = burst_size * 1000;
    }

    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, TM_SHAPER_CONFIGt,
                          TM_SHAPER_CONFIGt_ITU_MODEf, &itu_mode));
    /* granularity */
    for (gran = 0; gran <= TH4G_METER_GRANULARITY_NUM - 1; gran++) {
        bcm56996_a0_tm_granularity_params(unit, gran, shaping_mode,
                &refresh_unit_sz, &burst_unit_sz);
        refresh_max = refresh_mask * refresh_unit_sz;
        if (itu_mode)
            bucket_max = TH4G_METER_NL_BUCKET_MAC_ENCODE_MAX * burst_unit_sz;
        else
            bucket_max = (bucket_mask + 1) * burst_unit_sz;

        if ((bandwidth <= refresh_max) && (burst <= bucket_max)) {
            break;
        }
    }
    if (gran > TH4G_METER_GRANULARITY_NUM - 1) {
        /* Saturate */
        gran = TH4G_METER_GRANULARITY_NUM - 1;
        TH4G_MIN_SELECT(bandwidth, refresh_max);
        TH4G_MIN_SELECT(burst, bucket_max);
    }
    bucket_encode->granularity = gran;

    if (bandwidth > (0xffffffff - (refresh_unit_sz - 1)))
        bandwidth = 0xffffffff - (refresh_unit_sz - 1);
    bucket_encode->refresh_rate =
            (bandwidth + refresh_unit_sz - 1) / refresh_unit_sz;
    TH4G_MIN_SELECT(bucket_encode->refresh_rate, refresh_mask);

    if (itu_mode) {
        /* ITU mode enabled. (Non-Linear mode) */
        if (burst <= burst_unit_sz) {
            bucket_encode->bucket_sz = burst ? 1 : 0; /* 0 kb means disable */
        } else {
            encoding = bucket_mask;
            bucket_top = burst_unit_sz;
            for (i = 0; i < TH4G_METER_NL_BUCKET_RANGE_NUM;
                    i++, bucket_top *= 2) {
                 bucket_segment_size =
                    bucket_top / TH4G_METER_NL_SEGMENT_GRANULARITY;
                if (burst <= (bucket_top * 2 - bucket_segment_size)) {
                    encoding =
                        (burst - bucket_top + (bucket_segment_size - 1)) /
                        bucket_segment_size;
                    encoding |= i << TH4G_METER_NL_BUCKET_POWER_SHIFT;
                    break;
                }
            }
            bucket_encode->bucket_sz =
                (i < TH4G_METER_NL_BUCKET_RANGE_NUM) ? encoding : bucket_mask;
        }
    } else {
        encoding = (burst + (burst_unit_sz - 1)) / burst_unit_sz;
        if (encoding > bucket_mask) {
            encoding = bucket_mask;
        }
        bucket_encode->bucket_sz = encoding;
    }
exit:
    SHR_FUNC_EXIT();
}
