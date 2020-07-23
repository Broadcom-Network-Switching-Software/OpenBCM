/*! \file bcm56780_a0_cth_meter_l2_iif_sc_drv.c
 *
 * bcm56780_a0 driver for storm control meter.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmcth/bcmcth_meter_l2_iif_sc.h>
#include <bcmcth/bcmcth_meter_l2_iif_sc_drv.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_METER

/* Number of offsets per storm control meters. */
#define BCM56780_A0_METER_MAX_OFFSET            (4)
/* Minimum metering rate in kbps */
#define BCM56780_A0_METER_SC_RATE_KBPS_MIN      (64) /* 64kbps */
/* Maximum value of BUCKETSIZE */
#define BCM56780_A0_METER_SC_MAX_BUCKET_SZ      (8)
/* Number of tokens refreshed in a sec when REFRESHCOUNT=1 */
#define BCM56780_A0_METER_SC_TPS_MIN            (128000)
/* Number of quantum levels, SW defined. PKT_QUANTUM range is [1 - 128000]. */
#define BCM56780_A0_METER_SC_PKT_QUANTUM_LEVELS (18)
/* Maximum REFRESHCOUNT value supported on this device. */
#define BCM56780_A0_METER_SC_MAX_REFRESH_COUNT  (0x1fffff)

/* Byte mode macros */
/* Calculate refresh count from metering rate. */
#define BCM56780_A0_METER_SC_COMPUTE_BYTE_REFRESH_COUNT(rps)        \
    ((rps + (BCM56780_A0_METER_SC_RATE_KBPS_MIN - 1)) /             \
     BCM56780_A0_METER_SC_RATE_KBPS_MIN)

/* Calculate metering rate from refresh count. */
#define BCM56780_A0_METER_SC_COMPUTE_BYTE_METER_RATE(rc)            \
    ((uint64_t)rc * (uint64_t)BCM56780_A0_METER_SC_RATE_KBPS_MIN)

/* Calculate burst size from bucket size. */
#define BCM56780_A0_METER_SC_GET_BURST_SZ_KBITS_FROM_BKT_SZ(bktsz)  \
    (bcm56780_a0_bkt_sz_to_kbit_map[bktsz])

/* Packet mode macros */
/* Calculate packets per sec from tokens per sec based on packet quantum. */
#define BCM56780_A0_METER_SC_PPS_MIN(quantum)                       \
    (BCM56780_A0_METER_SC_TPS_MIN / quantum)

/* Calcualte refresh count from metering rate and quantum. */
#define BCM56780_A0_METER_SC_COMPUTE_PKT_REFRESH_COUNT(rps, quantum)        \
    ((rps + (BCM56780_A0_METER_SC_PPS_MIN(quantum) - 1)) /                  \
     BCM56780_A0_METER_SC_PPS_MIN(quantum))

/* Calculate metering rate from refreshcount and quantum .*/
#define BCM56780_A0_METER_SC_COMPUTE_PKT_METER_RATE(rc, quantum)            \
    ((uint64_t)rc * (uint64_t)BCM56780_A0_METER_SC_PPS_MIN(quantum))

/* Calculate burst size from bucket size. */
#define BCM56780_A0_METER_SC_GET_BURST_SZ_PKTS_FROM_BKT_SZ(bktsz, quantum)  \
    (bcm56780_a0_bkt_sz_to_token_map[bktsz] / quantum)

/* Get the quantum value corresponding to the quantum level .*/
#define BCM56780_A0_METER_SC_QUANTUM_VALUE(level)                           \
    (bcm56780_a0_pkt_quantum_info[level].quantum)

/* Get the maximum metering rate supported at each quantum level. */
#define BCM56780_A0_METER_SC_QUANTUM_MAX_RATE(level)                        \
    ((uint64_t)bcm56780_a0_pkt_quantum_info[level].max_rate *               \
     (uint64_t)1000000)

/*! Convert kilobytes to bytes. */
#define KBYTES          1024
/*! Convert megabytes to bytes. */
#define MBYTES          (1024 * 1024)
/*! Number of bits in a byte. */
#define BITSINBYTE      8
/*! Kilobits in API. */
#define KBITS_IN_API    1000

/*!
 * \brief Token-wise bucket size definition for BCM56780_A0.
 *
 * Each bucket size translates to a specific bucket count.
 *
 * 8 options for Bucket Size.
 * In packet mode, each packet is PQ tokens
 * 0 - 64K tokens
 * 1 - 256K tokens
 * 2 - 1M tokens
 * 3 - 4M tokens
 * 4 - 16M tokens
 * 5 - 64M tokens
 * 6 - 128M tokens
 * 7 - 256M tokens.
 */
static uint32_t
bcm56780_a0_bkt_sz_to_token_map[BCM56780_A0_METER_SC_MAX_BUCKET_SZ] = {
    (64  * KBYTES),
    (256 * KBYTES),
    (1   * MBYTES),
    (4   * MBYTES),
    (16  * MBYTES),
    (64  * MBYTES),
    (128 * MBYTES),
    (256 * MBYTES)
};

/*!
 * \brief Kbit-wise bucket size definition for BCM56780_A0.
 *
 * Each bucket size translates to a specific bucket count.
 *
 * In byte mode, each token is 0.5 bit
 * So the available bucket sizes translate to
 * 0 - 4KB ([64K * token_size / bits_in_bytes] => [64K * 0.5 / 8])
 * 1 - 16KB
 * 2 - 64KB
 * 3 - 256KB
 * 4 - 1MB
 * 5 - 4MB
 * 6 - 8MB
 * 7 - 16MB.
 */
static uint32_t
bcm56780_a0_bkt_sz_to_kbit_map[BCM56780_A0_METER_SC_MAX_BUCKET_SZ] = {
    ((4   * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((16  * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((64  * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((256 * KBYTES * BITSINBYTE) / KBITS_IN_API),
    ((1   * MBYTES * BITSINBYTE) / KBITS_IN_API),
    ((4   * MBYTES * BITSINBYTE) / KBITS_IN_API),
    ((8   * MBYTES * BITSINBYTE) / KBITS_IN_API),
    ((16  * MBYTES * BITSINBYTE) / KBITS_IN_API)
};

/*!
 * \brief Min-Max definitions per quantum level for BCM56780_A0.
 *
 * Quantum value, min and max meter rates for each Quantum level.
 * Quantum in tokens, min rate in pps, max rate in million pps.
 */
static bcmcth_meter_l2_iif_sc_pkt_quantum_info_t
bcm56780_a0_pkt_quantum_info[BCM56780_A0_METER_SC_PKT_QUANTUM_LEVELS] = {
    {1, 128000, 268435},
    {2, 64000, 134217},
    {4, 32000, 67108},
    {8, 16000, 33554},
    {16, 8000, 16777},
    {32, 4000, 8388},
    {64, 2000, 4194},
    {128, 1000, 2097},
    {256, 500, 1048},
    {512, 250, 524},
    {1024, 125, 262},
    {2000, 64, 134},
    {4000, 32, 67},
    {8000, 16, 33},
    {16000, 8, 16},
    {32000, 4, 8},
    {64000, 2, 4},
    {128000, 1, 2}
};

/*
 * PACKETQUANTUM value is per storm control meter and is shared by the
 * 4 meter offsets.
 * If PACKETQUANTUM value changes, recalculate the refreshcount and
 * bucket size of the other offsets in this meter so that their
 * metering rate and burst size is not changed much.
 */
static int
bcm56780_a0_meter_sc_recalc_hw_param(int unit,
                                     uint32_t meter_idx,
                                     bcmcth_meter_l2_iif_sc_entry_t *entry)
{
    uint32_t    i, j;
    uint32_t    burst_pkts = 0;

    SHR_FUNC_ENTER(unit);

    if (meter_idx == 0) {
        SHR_EXIT();
    }

    for (j = 0; j < meter_idx; j++) {
        /*
         * Recalculate refresh count for the other in-use meters of this port
         */
        if (entry->meter_en[j] != 0) {
            entry->refresh_count[j] =
                BCM56780_A0_METER_SC_COMPUTE_PKT_REFRESH_COUNT(entry->rate_pps[j],
                                                               entry->quantum);
            if (entry->refresh_count[j] == 0) {
                entry->refresh_count[j] = 1;
            } else if (entry->refresh_count[j] >
                       BCM56780_A0_METER_SC_MAX_REFRESH_COUNT) {
                entry->refresh_count[j] = BCM56780_A0_METER_SC_MAX_REFRESH_COUNT;
            }

            /*
             * Recalculate bucket size for the other 3 meters of this port
             * Check if the bucket sizes are valid
             */
            /*
             * Iterate over all bucket sizes to find the one
             * which gives the required burst size
             */
            for (i = 0; i < BCM56780_A0_METER_SC_MAX_BUCKET_SZ; i++) {
                burst_pkts =
                    BCM56780_A0_METER_SC_GET_BURST_SZ_PKTS_FROM_BKT_SZ(
                        i,
                        entry->quantum);
                if (burst_pkts >= entry->burst_pkts[j]) {
                    break;
                }
            }
            if (i == BCM56780_A0_METER_SC_MAX_BUCKET_SZ) {
                i--;
            }
            entry->bucket_size[j] = i;
            entry->bucket_count[j] = bcm56780_a0_bkt_sz_to_token_map[i];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Calculate PACKETQUANTUM value based on the metering rate and
 * burst size.
 */
static int
bcm56780_a0_meter_sc_compute_pkt_quantum(int unit,
                                         uint64_t meter_rate,
                                         uint32_t burst_pkts,
                                         uint32_t *quantum)
{
    int      i = 0, j = 0;
    bool     found = false;
    uint64_t max_rate = 0;
    uint32_t max_burst;

    SHR_FUNC_ENTER(unit);

    /*
     * Iterate over different quantum values (factor of 8000)
     */
    for (i = (BCM56780_A0_METER_SC_PKT_QUANTUM_LEVELS - 1); i >= 0; i--) {
        max_rate = BCM56780_A0_METER_SC_QUANTUM_MAX_RATE(i);
        if (max_rate >= meter_rate) {
            /*
             * If the metering rate at this level is sufficient, calculate if
             * it is enough for the specified burst size.
             */
            for (j = 0; j < BCM56780_A0_METER_SC_MAX_BUCKET_SZ; j++) {
                max_burst =
                    BCM56780_A0_METER_SC_GET_BURST_SZ_PKTS_FROM_BKT_SZ(
                        j,
                        BCM56780_A0_METER_SC_QUANTUM_VALUE(i));
                if (max_burst >= burst_pkts) {
                    found = true;
                    break;
                }
            }
            if (found == true) {
                break;
            }
        }
    }

    if (i < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *quantum = BCM56780_A0_METER_SC_QUANTUM_VALUE(i);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Convert the LT entry fields into corresponding H/W params
 * PACKETQUANTUM
 * REFRESHCOUNT
 * BUCKETSIZE
 * BUCKETCOUNT
 */
static int
bcm56780_a0_meter_sc_compute_hw_param (int unit,
                                       bcmcth_meter_l2_iif_sc_entry_t *entry)
{
    uint32_t *map_ptr;
    uint32_t burst_val = 0;
    uint32_t i, idx = 0;
    uint64_t max_rate = 0;
    uint32_t max_meter_offset = 4;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < max_meter_offset; idx++) {
        /* Meter at this offset is not enabled, reset fields. */
        if (entry->meter_en[idx] == 0) {
            entry->refresh_count[idx] = 0;
            entry->bucket_count[idx] = 0;
            entry->bucket_size[idx] = 0;
            entry->rate_kbps_oper[idx] = 0;
            entry->rate_pps_oper[idx] = 0;
            entry->burst_kbits_oper[idx] = 0;
            entry->burst_pkts_oper[idx] = 0;
            continue;
        }

        if (entry->byte_mode) {
            /*
             * No PACKETQUANTUM in byte mode. Direct calculation of
             * refreshcount from metering rate.
             */
            entry->refresh_count[idx] =
                BCM56780_A0_METER_SC_COMPUTE_BYTE_REFRESH_COUNT(
                    entry->rate_kbps[idx]);
        } else {
            /*
             * If current PACKETQUANTUM is zero, compute it.
             */
            if (entry->quantum == 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm56780_a0_meter_sc_compute_pkt_quantum(
                            unit,
                            entry->rate_pps[idx],
                            entry->burst_pkts[idx],
                            &(entry->quantum)));
            } else {
                /*
                 * If current quantum is not zero, which means there are other
                 * active offsets in this meter, check if the same value will suffice
                 * for this new offset. If not, compute a nwe PACKETQUANTUM value.
                */
                max_rate = BCM56780_A0_METER_SC_COMPUTE_PKT_METER_RATE(
                    BCM56780_A0_METER_SC_MAX_REFRESH_COUNT,
                    entry->quantum);
                if (max_rate < entry->rate_pps[idx]) {
                    /* Compute PACKETQUANTUM. */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcm56780_a0_meter_sc_compute_pkt_quantum(
                                unit,
                                entry->rate_pps[idx],
                                entry->burst_pkts[idx],
                                &(entry->quantum)));
                    /*
                     * Recalculate the refresh count and bucket size of
                     * the other offsets.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcm56780_a0_meter_sc_recalc_hw_param(
                                unit,
                                idx,
                                entry));
                }
            }

            /*
             * Once quantum is determined, calculate refresh count
             */
            entry->refresh_count[idx] =
                BCM56780_A0_METER_SC_COMPUTE_PKT_REFRESH_COUNT(
                    entry->rate_pps[idx],
                    entry->quantum);
        }

        /*
         * Compute bucket size and bucket count.
         */
        if (entry->byte_mode) {
            map_ptr = bcm56780_a0_bkt_sz_to_kbit_map;
            burst_val = entry->burst_kbits[idx];
        } else {
            map_ptr = bcm56780_a0_bkt_sz_to_token_map;
            burst_val = (entry->burst_pkts[idx] * (entry->quantum));
        }

        /*
         * Find the bucket size that can accommodate this burst size
         */
        for (i = 0; i < BCM56780_A0_METER_SC_MAX_BUCKET_SZ; i++) {
            if (map_ptr[i] >= burst_val) {
                break;
            }
        }

        if (i == BCM56780_A0_METER_SC_MAX_BUCKET_SZ) {
            i--;
        }
        entry->bucket_size[idx] = i;
        entry->bucket_count[idx] = bcm56780_a0_bkt_sz_to_token_map[i];

        /* Calculate the operating values. */
        if (entry->byte_mode) {
            entry->rate_kbps_oper[idx] =
                BCM56780_A0_METER_SC_COMPUTE_BYTE_METER_RATE(
                    entry->refresh_count[idx]);
            entry->burst_kbits_oper[idx] =
                BCM56780_A0_METER_SC_GET_BURST_SZ_KBITS_FROM_BKT_SZ(
                    entry->bucket_size[idx]);
        } else {
            entry->rate_pps_oper[idx] =
                BCM56780_A0_METER_SC_COMPUTE_PKT_METER_RATE(
                    entry->refresh_count[idx],
                    entry->quantum);
            entry->burst_pkts_oper[idx] =
                BCM56780_A0_METER_SC_GET_BURST_SZ_PKTS_FROM_BKT_SZ(
                    entry->bucket_size[idx],
                    entry->quantum);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* Storm control meter driver for bcm56780_a0. */
bcmcth_meter_l2_iif_sc_drv_t bcm56780_a0_meter_l2_iif_sc_drv = {
    .config_ptid = IFP_METER_STORM_CONTROL_METER_CONFIGm,
    .byte_fid = BYTE_MODEf,
    .quantum_fid = PACKET_QUANTUMf,
    .meter_ptid = IFP_METER_STORM_CONTROL_METER_TABLEm,
    .rc_fid = REFRESHCOUNTf,
    .bc_fid = BUCKETCOUNTf,
    .bs_fid = BUCKETSIZEf,
    .num_offset = BCM56780_A0_METER_MAX_OFFSET,
    .compute_hw_param = &bcm56780_a0_meter_sc_compute_hw_param,
};

/* Return driver for bcm56780_a0 storm control meter. */
bcmcth_meter_l2_iif_sc_drv_t *
bcm56780_a0_cth_meter_l2_iif_sc_drv_get(int unit)
{
    return &bcm56780_a0_meter_l2_iif_sc_drv;
}
