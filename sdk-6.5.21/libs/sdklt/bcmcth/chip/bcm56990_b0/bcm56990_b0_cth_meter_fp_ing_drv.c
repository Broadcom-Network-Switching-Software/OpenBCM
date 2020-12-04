/*! \file bcm56990_b0_cth_meter_fp_ing_drv.c
 *
 * This file defines bcm56990_b0 chip specific attributes for IFP Meter.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcmdrd_config.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_meter_fp_ing.h>
#include <bcmcth/bcmcth_meter_fp_common.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_METER

/*! Number of ingress meters per pipe for BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_METERS_PER_PIPE         512
/*! Number of pipes for BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_NUM_PIPES               4
/*! Number of ingress meters per pool for BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_METERS_PER_POOL         256
/*! Number of ingress meter pools for BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_NUM_POOLS               2
/*! Max granularity level supported on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_MAX_GRANULARITY         8
/*! Max refresh count supported on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_MAX_REFRESH_COUNT       0x3FFFFF
/*! Max bucket size supported on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_MAX_BUCKET_SIZE         0xFFF
/*! Bucket size granularity in tokens on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_BUCKET_SIZE_GRAN        65536
/*! Refresh count adjustment required for IFP meters on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_REFRESH_COUNT_ADJUST    false
/*! Meter pool Global status for IFP meters on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_POOL_GLOBAL             true
/*! Ingress meter byte mode field name on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_BYTEMODE_FID            PKTS_BYTESf
/*! ingress meter granularity field name on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_GRAN_FID                METER_GRANf
/*! Ingress meter refresh mode field name on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_REFRESHMODE_FID         REFRESH_MODEf
/*! Ingress meter refresh count field name on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_REFRESHCOUNT_FID        REFRESHCOUNTf
/*! Ingress meter bucket size field name on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_BUCKETSIZE_FID          BUCKETSIZEf
/*! Ingress meter bucket count field name on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_BUCKETCOUNT_FID         BUCKETCOUNTf
/*! IFP meter refresh enable register on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_REFRESH_CONFIG_SID      ING_DII_AUX_ARB_CONTROLr
/*! IFP meter refresh enable field on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_REFRESH_EN_FID          FP_REFRESH_ENABLEf
/*! IFP meter refresh mode field on BCM56990_B0. */
#define BCM56990_B0_METER_FP_ING_REFRESH_MODE_FID        FP_REFRESH_MODEf

/*!
 * \brief Min-Max definitions in byte mode per granularity for BCM56990_B0.
 *
 * Min/Max rate in kbps, min burst size in bits for IFP Meter.
 */
static bcmcth_meter_fp_gran_info_t
bcm56990_b0_meter_fp_ing_gran_info_bytes[BCM56990_B0_METER_FP_ING_MAX_GRANULARITY] = {
    {8, 33554424, 4096},
    {16, 67108848, 8192},
    {32, 134217696, 16384},
    {64, 268435392, 32768},
    {128, 536870784, 65536},
    {256, 1073741568, 131072},
    {512, 2147483136, 262144},
    {1024, 4294966272UL, 524288},
};

/*!
 * \brief Min-Max definitions in packet mode per granularity for BCM56990_B0.
 *
 * Min/Max rate in pps, min burst size in (pps / 1000) for IFP Meter.
 */
static bcmcth_meter_fp_gran_info_t
bcm56990_b0_meter_fp_ing_gran_info_pkts[BCM56990_B0_METER_FP_ING_MAX_GRANULARITY] = {
    /* min rate in pps, max rate in pps, min burst in (packet / 1000) */
    {1, 4194303, 512},
    {2, 8388606, 1024},
    {4, 16777212, 2048},
    {8, 33554424, 4096},
    {16, 67108848, 8192},
    {64, 268435392, 32768},
    {256, 1073741568, 131072},
    {1024, 4294966272UL, 524288},
};

static bcmcth_meter_fp_fid_info_t bcm56990_b0_meter_fp_ing_fid_info = {
    .pipe_fid = METER_ING_FP_TEMPLATEt_PIPEf,
    .pool_fid = METER_ING_FP_TEMPLATEt_POOL_INSTANCEf,
    .pkt_mode_fid = METER_ING_FP_TEMPLATEt_PKT_MODEf,
    .meter_mode_fid = METER_ING_FP_TEMPLATEt_METER_MODEf,
    .color_mode_fid = METER_ING_FP_TEMPLATEt_COLOR_MODEf,
    .min_rate_kbps_fid = METER_ING_FP_TEMPLATEt_MIN_RATE_KBPSf,
    .max_rate_kbps_fid = METER_ING_FP_TEMPLATEt_MAX_RATE_KBPSf,
    .min_burst_kbits_fid = METER_ING_FP_TEMPLATEt_MIN_BURST_SIZE_KBITSf,
    .max_burst_kbits_fid = METER_ING_FP_TEMPLATEt_MAX_BURST_SIZE_KBITSf,
    .min_rate_pps_fid = METER_ING_FP_TEMPLATEt_MIN_RATE_PPSf,
    .max_rate_pps_fid = METER_ING_FP_TEMPLATEt_MAX_RATE_PPSf,
    .min_burst_pkts_fid = METER_ING_FP_TEMPLATEt_MIN_BURST_SIZE_PKTSf,
    .max_burst_pkts_fid = METER_ING_FP_TEMPLATEt_MAX_BURST_SIZE_PKTSf,
    .min_rate_kbps_oper_fid = METER_ING_FP_TEMPLATEt_MIN_RATE_KBPS_OPERf,
    .max_rate_kbps_oper_fid = METER_ING_FP_TEMPLATEt_MAX_RATE_KBPS_OPERf,
    .min_burst_kbits_oper_fid = METER_ING_FP_TEMPLATEt_MIN_BURST_SIZE_KBITS_OPERf,
    .max_burst_kbits_oper_fid = METER_ING_FP_TEMPLATEt_MAX_BURST_SIZE_KBITS_OPERf,
    .min_rate_pps_oper_fid = METER_ING_FP_TEMPLATEt_MIN_RATE_PPS_OPERf,
    .max_rate_pps_oper_fid = METER_ING_FP_TEMPLATEt_MAX_RATE_PPS_OPERf,
    .min_burst_pkts_oper_fid = METER_ING_FP_TEMPLATEt_MIN_BURST_SIZE_PKTS_OPERf,
    .max_burst_pkts_oper_fid = METER_ING_FP_TEMPLATEt_MAX_BURST_SIZE_PKTS_OPERf,
    .meter_action_set_fid = METER_ING_FP_TEMPLATEt_FP_METER_ACTION_SETf,
};

static uint32_t bcm56990_b0_meter_fp_ing_sid[] = {
    IFP_METER_TABLE_INST0m,
    IFP_METER_TABLE_INST1m,
};

/*! BCM56990_B0 specific attribute definitions for IFP meter. */
bcmcth_meter_fp_attrib_t bcm56990_b0_meter_fp_ing_attrib = {
    .ltid = METER_ING_FP_TEMPLATEt,
    .key_fid = METER_ING_FP_TEMPLATEt_METER_ING_FP_TEMPLATE_IDf,
    .fld_count = METER_ING_FP_TEMPLATEt_FIELD_COUNT,
    .num_meters_per_pipe  = BCM56990_B0_METER_FP_ING_METERS_PER_PIPE,
    .num_pipes = BCM56990_B0_METER_FP_ING_NUM_PIPES,
    .num_meters_per_pool = BCM56990_B0_METER_FP_ING_METERS_PER_POOL,
    .num_pools = BCM56990_B0_METER_FP_ING_NUM_POOLS,
    .max_granularity = BCM56990_B0_METER_FP_ING_MAX_GRANULARITY,
    .max_refresh_cnt = BCM56990_B0_METER_FP_ING_MAX_REFRESH_COUNT,
    .max_bucket_size = BCM56990_B0_METER_FP_ING_MAX_BUCKET_SIZE,
    .bucket_size_gran = BCM56990_B0_METER_FP_ING_BUCKET_SIZE_GRAN,
    .refreshcount_adjust = BCM56990_B0_METER_FP_ING_REFRESH_COUNT_ADJUST,
    .meter_pool_global = BCM56990_B0_METER_FP_ING_POOL_GLOBAL,
    .num_meter_sids = 2,
    .meter_sid = bcm56990_b0_meter_fp_ing_sid,
    .byte_mode_fid = BCM56990_B0_METER_FP_ING_BYTEMODE_FID,
    .meter_gran_fid = BCM56990_B0_METER_FP_ING_GRAN_FID,
    .refresh_mode_fid = BCM56990_B0_METER_FP_ING_REFRESHMODE_FID,
    .refresh_count_fid = BCM56990_B0_METER_FP_ING_REFRESHCOUNT_FID,
    .bucket_size_fid = BCM56990_B0_METER_FP_ING_BUCKETSIZE_FID,
    .bucket_count_fid = BCM56990_B0_METER_FP_ING_BUCKETCOUNT_FID,
    .refresh_cfg_sid = BCM56990_B0_METER_FP_ING_REFRESH_CONFIG_SID,
    .refresh_en_fid = BCM56990_B0_METER_FP_ING_REFRESH_EN_FID,
    .fp_refresh_mode_fid = BCM56990_B0_METER_FP_ING_REFRESH_MODE_FID,
    .fid_info = &bcm56990_b0_meter_fp_ing_fid_info,
    .gran_info_bytes = bcm56990_b0_meter_fp_ing_gran_info_bytes,
    .gran_info_pkts = bcm56990_b0_meter_fp_ing_gran_info_pkts,
    .slice_ctrl = NULL,
    .meter_format_info = NULL,
};

static int
bcm56990_b0_meter_fp_ing_hw_config(int unit,
                                   uint32_t refresh_en)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56990_b0_meter_fp_ing_attrib;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_util_hw_config(unit, attr, refresh_en));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_meter_fp_ing_hw_param_compute(int unit,
                                          bcmcth_meter_sw_params_t *lt_entry)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56990_b0_meter_fp_ing_attrib;
    bcmcth_meter_fp_sw_state_t *state;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_ing_sw_state_get(unit, &state));

    SHR_IF_ERR_EXIT
        (bcmcth_meter_util_convert_to_hw_params(unit, attr, state, lt_entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_meter_fp_ing_hw_oper_get(int unit,
                                     bcmcth_meter_sw_params_t *lt_entry)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56990_b0_meter_fp_ing_attrib;
    bcmcth_meter_fp_sw_state_t *state;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_ing_sw_state_get(unit, &state));

    SHR_IF_ERR_EXIT
        (bcmcth_meter_util_hw_oper_get(unit, attr, state, lt_entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_meter_fp_ing_hw_idx_alloc(int unit,
                                      int pool,
                                      uint8_t oper_mode,
                                      bcmcth_meter_sw_params_t *lt_entry,
                                      uint32_t *hw_idx)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56990_b0_meter_fp_ing_attrib;
    bcmcth_meter_fp_sw_state_t *state;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_ing_sw_state_get(unit, &state));

    SHR_IF_ERR_EXIT
        (bcmcth_meter_util_hw_index_alloc(unit, attr, state,
                                          pool, oper_mode,
                                          lt_entry, hw_idx));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_meter_fp_ing_hw_idx_free(int unit,
                                     uint32_t hw_idx,
                                     bcmcth_meter_sw_params_t *lt_entry)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56990_b0_meter_fp_ing_attrib;
    bcmcth_meter_fp_sw_state_t *state;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_ing_sw_state_get(unit, &state));

    SHR_IF_ERR_EXIT
        (bcmcth_meter_util_hw_index_free(unit, attr, state, hw_idx, lt_entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_meter_fp_ing_hw_write(int unit,
                                  uint8_t oper_mode,
                                  uint32_t trans_id,
                                  bcmcth_meter_sw_params_t *lt_entry)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56990_b0_meter_fp_ing_attrib;
    bcmcth_meter_fp_sw_state_t *state;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_ing_sw_state_get(unit, &state));

    SHR_IF_ERR_EXIT
        (bcmcth_meter_util_entry_write(unit, trans_id, attr, state,
                                       oper_mode, lt_entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_meter_fp_ing_hw_clear(int unit,
                                  uint8_t oper_mode,
                                  uint32_t trans_id,
                                  int pool,
                                  uint32_t hw_index,
                                  bcmcth_meter_sw_params_t *lt_entry)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56990_b0_meter_fp_ing_attrib;
    bcmcth_meter_fp_sw_state_t *state;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_ing_sw_state_get(unit, &state));

    SHR_IF_ERR_EXIT
        (bcmcth_meter_util_entry_clear(unit, attr, state, oper_mode, trans_id,
                                       pool, hw_index, lt_entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_meter_fp_ing_attrib_get(int unit,
                                        bcmcth_meter_fp_attrib_t **attr)
{
    *attr = &bcm56990_b0_meter_fp_ing_attrib;
    return SHR_E_NONE;
}

bcmcth_meter_fp_drv_t bcm56990_b0_meter_fp_ing_drv = {
    .hw_config = &bcm56990_b0_meter_fp_ing_hw_config,
    .hw_param_compute = &bcm56990_b0_meter_fp_ing_hw_param_compute,
    .hw_oper_get = &bcm56990_b0_meter_fp_ing_hw_oper_get,
    .hw_idx_alloc = &bcm56990_b0_meter_fp_ing_hw_idx_alloc,
    .hw_idx_free = &bcm56990_b0_meter_fp_ing_hw_idx_free,
    .hw_write = &bcm56990_b0_meter_fp_ing_hw_write,
    .hw_clear = &bcm56990_b0_meter_fp_ing_hw_clear,
    .attrib_get = bcm56990_b0_cth_meter_fp_ing_attrib_get
};

bcmcth_meter_fp_drv_t *
bcm56990_b0_cth_meter_fp_ing_drv_get(int unit)
{
    return &bcm56990_b0_meter_fp_ing_drv;
}

