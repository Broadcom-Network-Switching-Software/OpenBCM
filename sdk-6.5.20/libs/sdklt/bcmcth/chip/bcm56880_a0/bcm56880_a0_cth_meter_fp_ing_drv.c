/*! \file bcm56880_a0_cth_meter_fp_ing_drv.c
 *
 * This file defines bcm56880_a0 chip specific attributes for IFP Meter.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcmdrd_config.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmcth/bcmcth_meter_fp_ing.h>
#include <bcmcth/bcmcth_meter_fp_common.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_METER

/*! Number of ingress meters per pipe for BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_METERS_PER_PIPE         512
/*! Number of pipes for BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_NUM_PIPES               4
/*! Number of ingress meters per pool for BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_METERS_PER_POOL         128
/*! Number of ingress meter pools for BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_NUM_POOLS               4
/*! Max granularity level supported on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_MAX_GRANULARITY         8
/*! Max refresh count supported on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_MAX_REFRESH_COUNT       0x3FFFFF
/*! Max bucket size supported on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_MAX_BUCKET_SIZE         0xFFF
/*! Bucket size granularity in tokens on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_BUCKET_SIZE_GRAN        65536
/*! Refresh count adjustment required for IFP meters on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_REFRESH_COUNT_ADJUST    false
/*! Meter pool Global status for IFP meters on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_POOL_GLOBAL             true
/*! Ingress meter hardware table name on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_METER_SID               IFP_METER_METER_TABLEm
/*! Ingress meter byte mode field name on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_BYTEMODE_FID            PKTS_BYTESf
/*! ingress meter granularity field name on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_GRAN_FID                METER_GRANf
/*! Ingress meter refresh mode field name on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_REFRESHMODE_FID         REFRESH_MODEf
/*! Ingress meter refresh count field name on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_REFRESHCOUNT_FID        REFRESHCOUNTf
/*! Ingress meter bucket size field name on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_BUCKETSIZE_FID          BUCKETSIZEf
/*! Ingress meter bucket count field name on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_BUCKETCOUNT_FID         BUCKETCOUNTf
/*! IFP meter refresh enable register on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_REFRESH_CONFIG_SID      ING_DII_AUX_ARB_CONTROLr
/*! IFP meter refresh enable field on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_REFRESH_EN_FID          FP_REFRESH_ENABLEf
/*! IFP meter refresh mode field on BCM56880_A0. */
#define BCM56880_A0_METER_FP_ING_REFRESH_MODE_FID        FP_REFRESH_MODEf

/*!
 * \brief Min-Max definitions in byte mode per granularity for BCM56880_A0.
 *
 * Min/Max rate in kbps, min burst size in bits for IFP Meter.
 */
static bcmcth_meter_fp_gran_info_t
bcm56880_a0_meter_fp_ing_gran_info_bytes
[BCM56880_A0_METER_FP_ING_MAX_GRANULARITY] = {
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
 * \brief Min-Max definitions in packet mode per granularity for BCM56880_A0.
 *
 * Min/Max rate in pps, min burst size in (pps / 1000) for IFP Meter.
 */
static bcmcth_meter_fp_gran_info_t
bcm56880_a0_meter_fp_ing_gran_info_pkts
[BCM56880_A0_METER_FP_ING_MAX_GRANULARITY] = {
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

static bcmcth_meter_format_set_t
bcm56880_a0_meter_ing_format_info[METER_FIELD_COUNT] = {
    {METER_PAIR_INDEX, 0, 7},
    {METER_PAIR_MODE, 8, 10},
    {METER_PAIR_MODE_MODIFIER, 11, 11},
    {METER_TEST_EVEN, 12, 12},
    {METER_TEST_ODD, 13, 13},
    {METER_UPDATE_EVEN, 14, 14},
    {METER_UPDATE_ODD, 15, 15}
};

static bcmcth_meter_fp_fid_info_t bcm56880_a0_meter_fp_ing_fid_info = {
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

static uint32_t bcm56880_a0_meter_fp_ing_sid[] = {
    IFP_METER_METER_TABLEm,
};

/*! BCM56880_A0 specific attribute definitions for IFP meter. */
bcmcth_meter_fp_attrib_t bcm56880_a0_meter_fp_ing_attrib = {
    .ltid = METER_ING_FP_TEMPLATEt,
    .key_fid = METER_ING_FP_TEMPLATEt_METER_ING_FP_TEMPLATE_IDf,
    .fld_count = METER_ING_FP_TEMPLATEt_FIELD_COUNT,
    .num_meters_per_pipe = BCM56880_A0_METER_FP_ING_METERS_PER_PIPE,
    .num_pipes = BCM56880_A0_METER_FP_ING_NUM_PIPES,
    .num_meters_per_pool = BCM56880_A0_METER_FP_ING_METERS_PER_POOL,
    .num_pools = BCM56880_A0_METER_FP_ING_NUM_POOLS,
    .max_granularity = BCM56880_A0_METER_FP_ING_MAX_GRANULARITY,
    .max_refresh_cnt = BCM56880_A0_METER_FP_ING_MAX_REFRESH_COUNT,
    .max_bucket_size = BCM56880_A0_METER_FP_ING_MAX_BUCKET_SIZE,
    .bucket_size_gran = BCM56880_A0_METER_FP_ING_BUCKET_SIZE_GRAN,
    .refreshcount_adjust =  BCM56880_A0_METER_FP_ING_REFRESH_COUNT_ADJUST,
    .meter_pool_global = BCM56880_A0_METER_FP_ING_POOL_GLOBAL,
    .num_meter_sids = 1,
    .meter_sid = bcm56880_a0_meter_fp_ing_sid,
    .byte_mode_fid = BCM56880_A0_METER_FP_ING_BYTEMODE_FID,
    .meter_gran_fid = BCM56880_A0_METER_FP_ING_GRAN_FID,
    .refresh_mode_fid = BCM56880_A0_METER_FP_ING_REFRESHMODE_FID,
    .refresh_count_fid = BCM56880_A0_METER_FP_ING_REFRESHCOUNT_FID,
    .bucket_size_fid = BCM56880_A0_METER_FP_ING_BUCKETSIZE_FID,
    .bucket_count_fid = BCM56880_A0_METER_FP_ING_BUCKETCOUNT_FID,
    .refresh_cfg_sid = BCM56880_A0_METER_FP_ING_REFRESH_CONFIG_SID,
    .refresh_en_fid = BCM56880_A0_METER_FP_ING_REFRESH_EN_FID,
    .fp_refresh_mode_fid = BCM56880_A0_METER_FP_ING_REFRESH_MODE_FID,
    .fid_info = &bcm56880_a0_meter_fp_ing_fid_info,
    .gran_info_bytes = bcm56880_a0_meter_fp_ing_gran_info_bytes,
    .gran_info_pkts = bcm56880_a0_meter_fp_ing_gran_info_pkts,
    .slice_ctrl = NULL,
    .meter_format_info = bcm56880_a0_meter_ing_format_info,
};

static int
bcm56880_a0_meter_fp_profile_init (int unit)
{
    int i = 0;
    uint32_t entry_sz = 0;
    uint32_t rsp_flags, val = 0;
    uint8_t *read_buf = NULL;
    bcmltd_sid_t req_ltid, rsp_ltid;
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    sid = IFP_METER_BITP_PROFILEm;

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(read_buf, entry_sz, "bcmcthMeterFpHwConfig");
    SHR_NULL_CHECK(read_buf, SHR_E_MEMORY);

    req_ltid = METER_ING_FP_TEMPLATEt;

    for (i = 0; i < 4; i++) {

        pt_dyn_info.index = i;
        pt_dyn_info.tbl_inst = -1;
        entry_sz = bcmdrd_pt_entry_wsize(unit, sid);

        /* Read the Meter BITP profile table. */
        sal_memset(read_buf, 0, entry_sz);
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_read(unit,
                              0,
                              sid,
                              &pt_dyn_info,
                              NULL,
                              entry_sz,
                              req_ltid,
                              (uint32_t *)read_buf,
                              &rsp_ltid,
                              &rsp_flags));

        val = 0xf;
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)read_buf, METER_MAP_EN_SET_VALIDf,
                            &val);

        val = 0;
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)read_buf, METER_MAP_EN_DEFAULTf,
                            &val);

        val = 0xf;
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)read_buf, METER_ENf,
                            &val);

        /* Write back refresh config table. */
        SHR_IF_ERR_EXIT
            (bcmptm_ireq_write(unit,
                               0,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               (uint32_t *)read_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
    }
exit:
    SHR_FREE(read_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure FP meters for BCM56880_A0.
 *
 * Initialize FP meter related device configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] stage FP stage.
 * \param [in] refresh_en Refresh enable status.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56880_a0_meter_fp_ing_hw_config(int unit,
                                   uint32_t refresh_en)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56880_a0_meter_fp_ing_attrib;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_util_hw_config(unit, attr, refresh_en));

    SHR_IF_ERR_EXIT
        (bcm56880_a0_meter_fp_profile_init(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_meter_fp_ing_hw_param_compute(int unit,
                                          bcmcth_meter_sw_params_t *lt_entry)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56880_a0_meter_fp_ing_attrib;
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
bcm56880_a0_meter_fp_ing_hw_oper_get(int unit,
                                     bcmcth_meter_sw_params_t *lt_entry)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56880_a0_meter_fp_ing_attrib;
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
bcm56880_a0_meter_fp_ing_hw_idx_alloc(int unit,
                                      int pool,
                                      uint8_t oper_mode,
                                      bcmcth_meter_sw_params_t *lt_entry,
                                      uint32_t *hw_idx)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56880_a0_meter_fp_ing_attrib;
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
bcm56880_a0_meter_fp_ing_hw_idx_free(int unit,
                                     uint32_t hw_idx,
                                     bcmcth_meter_sw_params_t *lt_entry)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56880_a0_meter_fp_ing_attrib;
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
bcm56880_a0_meter_fp_ing_hw_write(int unit,
                                  uint8_t oper_mode,
                                  uint32_t trans_id,
                                  bcmcth_meter_sw_params_t *lt_entry)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56880_a0_meter_fp_ing_attrib;
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
bcm56880_a0_meter_fp_ing_hw_clear(int unit,
                                  uint8_t oper_mode,
                                  uint32_t trans_id,
                                  int pool,
                                  uint32_t hw_index,
                                  bcmcth_meter_sw_params_t *lt_entry)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56880_a0_meter_fp_ing_attrib;
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
bcm56880_a0_meter_fp_ing_format_get(int unit,
                                    bcmcth_meter_sw_params_t *lt_entry)
{
    bcmcth_meter_fp_attrib_t *attr = &bcm56880_a0_meter_fp_ing_attrib;
    bcmcth_meter_fp_sw_state_t *state;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_ing_sw_state_get(unit, &state));

    SHR_IF_ERR_EXIT
        (bcmcth_meter_util_policy_format_get(unit, attr, state, lt_entry));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_meter_fp_ing_attrib_get(int unit,
                                        bcmcth_meter_fp_attrib_t **attr)
{
    *attr = &bcm56880_a0_meter_fp_ing_attrib;
    return SHR_E_NONE;
}

bcmcth_meter_fp_drv_t bcm56880_a0_meter_fp_ing_drv = {
    .hw_config = &bcm56880_a0_meter_fp_ing_hw_config,
    .hw_param_compute = &bcm56880_a0_meter_fp_ing_hw_param_compute,
    .hw_oper_get = &bcm56880_a0_meter_fp_ing_hw_oper_get,
    .hw_idx_alloc = &bcm56880_a0_meter_fp_ing_hw_idx_alloc,
    .hw_idx_free = &bcm56880_a0_meter_fp_ing_hw_idx_free,
    .hw_write = &bcm56880_a0_meter_fp_ing_hw_write,
    .hw_clear = &bcm56880_a0_meter_fp_ing_hw_clear,
    .meter_format_get = &bcm56880_a0_meter_fp_ing_format_get,
    .attrib_get = bcm56880_a0_cth_meter_fp_ing_attrib_get
};

bcmcth_meter_fp_drv_t *
bcm56880_a0_cth_meter_fp_ing_drv_get(int unit)
{
    return &bcm56880_a0_meter_fp_ing_drv;
}
