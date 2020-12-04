/*! \file bcmcth_meter_util.c
 *
 * This file defines meter helper functions.
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
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_meter_fp_egr.h>
#include <bcmcth/bcmcth_meter_fp_ing.h>

#include <bcmcth/bcmcth_meter_fp_common.h>
#include <bcmcth/bcmcth_meter_util.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Calculate meter granularity params.
 *
 * Calculate meter granularity, refresh count and
 * bucket size from meter rate and burst size.
 *
 * \param [in] unit Unit number.
 * \param [in] attr FP attributes.
 * \param [in] meter_rate Metering rate.
 * \param [in] burst_size Meter burst size.
 * \param [in] byte_mode Meter mode
 * \param [out] gran Meter granularity.
 * \param [out] refresh_count Refresh count.
 * \param [out] bkt_sz Bucket size.
 * \param [out] bkt_cnt Bucket count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Unable to find a granularity level.
 */
static int
meter_util_compute_gran_params(int unit,
                               bcmcth_meter_fp_attrib_t *attr,
                               uint64_t meter_rate,
                               uint32_t burst_size,
                               int byte_mode,
                               int *gran,
                               uint32_t *refresh_count,
                               uint32_t *bkt_sz,
                               uint32_t *bkt_cnt)
{
    uint64_t min = 1, max = 0;
    uint32_t min_burst = 1, max_burst = 0;
    uint32_t bucket_size = 0, bucket_count = 0;
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < attr->max_granularity; i++) {
        /*
         * Get the max rate supported at each granularity level
         */
        if (byte_mode == METER_FP_MODE_BYTES) {
            min = attr->gran_info_bytes[i].min_rate;
            max = attr->gran_info_bytes[i].max_rate;
            min_burst = attr->gran_info_bytes[i].min_burst;
            max_burst = (min_burst * attr->max_bucket_size);
        } else {
            min = attr->gran_info_pkts[i].min_rate;
            max = attr->gran_info_pkts[i].max_rate;
            min_burst = attr->gran_info_pkts[i].min_burst;
            max_burst = (min_burst * attr->max_bucket_size);
        }

        /*
         *  Check if refresh count needs to be adjusted.
         */
        if (attr->refreshcount_adjust) {
            max /= 2;
        }

        if ((max >= meter_rate) && (max_burst >= burst_size)) {
            *gran = i;
            break;
        }
    }
    if (i == attr->max_granularity) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *refresh_count = ((meter_rate + (min - 1)) / min);
    /*
     * Multiply by 2 to adjust the refresh count
     */
    if (attr->refreshcount_adjust) {
        *refresh_count *= 2;
    }

    if (*refresh_count > attr->max_refresh_cnt) {
        *refresh_count = attr->max_refresh_cnt;
    }

    if (burst_size) {
        bucket_size = ((burst_size + (min_burst - 1)) / min_burst);
        if (bucket_size > attr->max_bucket_size) {
            bucket_size = attr->max_bucket_size;
        }
        if (bucket_size == 0) {
            bucket_size = 1;
        }

        *bkt_sz = bucket_size;

        bucket_count = (bucket_size * attr->bucket_size_gran);
        *bkt_cnt = bucket_count;
    } else {
        *bkt_sz = *bkt_cnt = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable slice status.
 *
 * Enable/disable the slice for access.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] attr FP attributes.
 * \param [in] slice FP slice number.
 * \param [in] status Enable/Disable flag.
 * \param [out] orig_status Slice status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
meter_util_slice_status_set(int unit,
                            uint32_t trans_id,
                            bcmcth_meter_fp_attrib_t *attr,
                            int slice,
                            uint32_t status,
                            uint32_t *orig_status)
{
    uint32_t entry_sz = 0;
    uint64_t req_flags = 0;
    uint32_t rsp_flags;
    uint8_t *read_buf = NULL;
    uint8_t fid_count = 0;
    bcmltd_sid_t req_ltid, rsp_ltid;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    if (attr->slice_ctrl == NULL) {
        SHR_EXIT();
    }

    sid = attr->slice_ctrl->sid;
    req_ltid = attr->ltid;

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(read_buf, entry_sz, "bcmcthMeterFpSliceStatus");
    SHR_NULL_CHECK(read_buf, SHR_E_MEMORY);
    sal_memset(read_buf, 0, entry_sz);

    fid_count = attr->slice_ctrl->fid_count;
    /*
     * Select the field name corresponding to the slice.
     */
    if (slice < fid_count) {
        fid = attr->slice_ctrl->fids[slice];
    } else {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /*
     * Global register. Index is 0 and instance is -1.
     */
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = -1;

    /*
     * Read slice control register to check if slice is enabled.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    0,
                                    entry_sz,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    (uint32_t *)read_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

    bcmdrd_pt_field_get(unit, sid, (uint32_t *)read_buf,
                        fid, orig_status);

    /*
     * If slice is already in desired state, do nothing.
     */
    if (status == *orig_status) {
        SHR_EXIT();
    }

    bcmdrd_pt_field_set(unit, sid, (uint32_t *)read_buf, fid,
                        &status);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)read_buf,
                                    0,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FREE(read_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore slice status.
 *
 * Restore the slice state.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] attr FP attributes.
 * \param [in] slice FP slice number.
 * \param [in] status Enable/Disable flag.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
meter_util_slice_status_reset(int unit,
                              uint32_t trans_id,
                              bcmcth_meter_fp_attrib_t *attr,
                              int slice,
                              uint32_t status)
{
    uint32_t entry_sz = 0;
    uint64_t req_flags = 0;
    uint32_t rsp_flags;
    uint8_t *read_buf = NULL;
    uint8_t fid_count = 0;
    bcmltd_sid_t req_ltid, rsp_ltid;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    if (attr->slice_ctrl == NULL) {
        SHR_EXIT();
    }

    sid = attr->slice_ctrl->sid;
    req_ltid = attr->ltid;

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(read_buf, entry_sz, "bcmcthMeterFpSliceStatus");
    SHR_NULL_CHECK(read_buf, SHR_E_MEMORY);
    sal_memset(read_buf, 0, entry_sz);

    fid_count = attr->slice_ctrl->fid_count;
    /*
     * Select the field name corresponding to the slice.
     */
    if (slice < fid_count) {
        fid = attr->slice_ctrl->fids[slice];
    } else {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /*
     * Global register. Index is 0 and instance is -1.
     */
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = -1;

    /*
     * Read slice control register to check if slice is enabled.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    0,
                                    entry_sz,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    (uint32_t *)read_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

    bcmdrd_pt_field_set(unit, sid, (uint32_t *)read_buf, fid,
                        &status);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    req_flags,
                                    sid,
                                    &pt_dyn_info,
                                    NULL,
                                    NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)read_buf,
                                    0,
                                    req_ltid,
                                    trans_id,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FREE(read_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable meter refresh on the device.
 *
 * Initialize FP meter related device configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] attr FP attributes.
 * \param [in] refresh_en Refresh enable status.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
meter_util_refresh_status_set(int unit,
                              bcmcth_meter_fp_attrib_t *attr,
                              uint32_t refresh_en)
{
    uint32_t entry_sz = 0;
    uint32_t rsp_flags;
    uint8_t *read_buf = NULL;
    uint32_t refresh_mode = 0;
    bcmltd_sid_t req_ltid, rsp_ltid;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmdrd_fid_t mode_fid;
    bcmbd_pt_dyn_info_t pt_dyn_info;

    SHR_FUNC_ENTER(unit);

    req_ltid = attr->ltid;
    sid = attr->refresh_cfg_sid;
    fid = attr->refresh_en_fid;

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    entry_sz *= 4;
    SHR_ALLOC(read_buf, entry_sz, "bcmcthMeterFpHwConfig");
    SHR_NULL_CHECK(read_buf, SHR_E_MEMORY);
    sal_memset(read_buf, 0, entry_sz);

    /*
     * Global register. Index is 0 and instance is -1.
     */
    pt_dyn_info.index = 0;
    pt_dyn_info.tbl_inst = -1;
    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);

    /* Read the refresh config table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_ireq_read(unit,
                              0,
                              sid,
                              &pt_dyn_info,
                              NULL,
                              entry_sz,
                              req_ltid,
                              (uint32_t *)read_buf,
                              &rsp_ltid,
                              &rsp_flags));

    bcmdrd_pt_field_set(unit, sid, (uint32_t *)read_buf, fid,
                        &refresh_en);
    mode_fid = attr->fp_refresh_mode_fid;
    if (mode_fid != 0) {
        bcmdrd_pt_field_set(unit, sid, (uint32_t *)read_buf, mode_fid,
                            &refresh_mode);
    }

    /* Write back refresh config table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_ireq_write(unit,
                               0,
                               sid,
                               &pt_dyn_info,
                               NULL,
                               (uint32_t *)read_buf,
                               req_ltid,
                               &rsp_ltid,
                               &rsp_flags));
exit:
    SHR_FREE(read_buf);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmcth_meter_util_hw_config(int unit,
                            bcmcth_meter_fp_attrib_t *attr,
                            uint32_t refresh_en)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (meter_util_refresh_status_set(unit, attr, refresh_en));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_util_entry_clear(int unit,
                              bcmcth_meter_fp_attrib_t *attr,
                              bcmcth_meter_fp_sw_state_t *state,
                              uint8_t oper_mode,
                              uint32_t trans_id,
                              int pool,
                              uint32_t meter_index,
                              bcmcth_meter_sw_params_t *lt_entry)
{
    int pipe_id = 0, i = 0;
    bool meter_pairing = false;
    uint8_t  idx = 0;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0;
    uint32_t entry_sz = 0;
    uint32_t orig_status = 0;
    uint32_t clear_index = 0;
    uint8_t *meter_buf = NULL;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t rsp_ltid, req_ltid;
    bcmdrd_sid_t sid;

    SHR_FUNC_ENTER(unit);

    pipe_id = lt_entry->pipe;

    if (attr->meter_pool_global == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_util_slice_status_set(unit,
                                         trans_id,
                                         attr,
                                         pool,
                                         TRUE,
                                         &orig_status));
    }

    for (idx = 0; idx < attr->num_meter_sids; idx++) {
        sid = attr->meter_sid[idx];
        req_ltid = attr->ltid;

        entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
        entry_sz *= 4;
        SHR_ALLOC(meter_buf, entry_sz, "bcmcthMeterFpHwEntry");
        SHR_NULL_CHECK(meter_buf, SHR_E_MEMORY);

        METER_FP_PAIRING_CHECK_ASSIGN(lt_entry->meter_mode,
                                      meter_pairing);

        clear_index = meter_index;
        for (i = 0; i < METER_FP_PAIR_NUM; i++) {
            /*
             * For non-paired meter modes, like DEFAULT and FLOW,
             * hw fields are stored at odd index, "1". Skip index "0".
             */
            if ((meter_pairing == false) && (i == METER_FP_INDEX_EVEN)) {
                continue;
            }

            sal_memset(meter_buf, 0, entry_sz);

            pt_dyn_info.index = clear_index;
            pt_dyn_info.tbl_inst =
                (oper_mode == METER_FP_OPER_MODE_PIPE_UNIQUE) ? pipe_id : -1;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_ltm_mreq_indexed_lt(unit,
                                            req_flags,
                                            sid,
                                            &pt_dyn_info,
                                            NULL,
                                            NULL, /* misc_info */
                                            BCMPTM_OP_WRITE,
                                            (uint32_t *)meter_buf,
                                            0,
                                            req_ltid,
                                            trans_id,
                                            NULL,
                                            NULL,
                                            NULL,
                                            &rsp_ltid,
                                            &rsp_flags));

            clear_index++;
        }
        SHR_FREE(meter_buf);
    }

    if (attr->meter_pool_global == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_util_slice_status_reset(unit,
                                           trans_id,
                                           attr,
                                           pool,
                                           orig_status));
    }

exit:
    SHR_FREE(meter_buf);
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_util_entry_write(int unit,
                              uint32_t trans_id,
                              bcmcth_meter_fp_attrib_t *attr,
                              bcmcth_meter_fp_sw_state_t *state,
                              uint8_t oper_mode,
                              bcmcth_meter_sw_params_t *lt_entry)
{
    int pipe_id = 0, i = 0, pool = 0;
    bool meter_pairing = false;
    uint8_t idx = 0;
    uint32_t entry_sz = 0, meter_index = 0;
    uint64_t req_flags = 0;
    uint32_t rsp_flags = 0, refresh_mode = 0, refresh_count = 0;
    uint32_t bucket_size = 0, gran = 0, byte_mode = 0, bucket_count = 0;
    uint32_t orig_status = 0, meter_id = 0;
    uint8_t *meter_buf = NULL;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t rsp_ltid, req_ltid;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmcth_meter_fp_entry_t *meter_entry = NULL;

    SHR_FUNC_ENTER(unit);

    meter_id = lt_entry->meter_id;
    meter_entry = &state->list[meter_id];
    pipe_id = lt_entry->pipe;
    pool = meter_entry->pool;

    if (attr->meter_pool_global == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_util_slice_status_set(unit,
                                         trans_id,
                                         attr,
                                         pool,
                                         TRUE,
                                         &orig_status));
    }

    for (idx = 0; idx < attr->num_meter_sids; idx++) {
        sid = attr->meter_sid[idx];
        req_ltid = attr->ltid;

        entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
        entry_sz *= 4;
        SHR_ALLOC(meter_buf, entry_sz, "bcmcthMeterFpHwEntry");
        SHR_NULL_CHECK(meter_buf, SHR_E_MEMORY);

        METER_FP_PAIRING_CHECK_ASSIGN(lt_entry->meter_mode,
                                      meter_pairing);

        meter_index = meter_entry->hw_fields.meter_hw_index;
        for (i = 0; i < METER_FP_PAIR_NUM; i++) {
            /*
             * For non-paired meter modes, like DEFAULT and FLOW,
             * hw fields are stored at odd index, "1". Skip index "0".
             */
            if ((meter_pairing == false) && (i == METER_FP_INDEX_EVEN)) {
                continue;
            }

            byte_mode = meter_entry->hw_fields.byte_mode;
            refresh_mode = meter_entry->hw_fields.refresh_mode;
            refresh_count = meter_entry->hw_fields.refresh_count[i];
            bucket_size = meter_entry->hw_fields.bucket_size[i];
            bucket_count = meter_entry->hw_fields.bucket_count[i];
            gran = meter_entry->hw_fields.gran[i];

            sal_memset(meter_buf, 0, entry_sz);
            fid = attr->bucket_size_fid;
            bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                                &bucket_size);

            fid = attr->bucket_count_fid;
            bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                                &bucket_count);

            fid = attr->refresh_count_fid;
            bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                                &refresh_count);

            fid = attr->meter_gran_fid;
            bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                                &gran);

            fid = attr->refresh_mode_fid;
            bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                                &refresh_mode);

            fid = attr->byte_mode_fid;
            bcmdrd_pt_field_set(unit, sid, (uint32_t *)meter_buf, fid,
                                &byte_mode);

            pt_dyn_info.index = meter_index;
            pt_dyn_info.tbl_inst =
                (oper_mode == METER_FP_OPER_MODE_PIPE_UNIQUE) ? pipe_id : -1;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_ltm_mreq_indexed_lt(unit,
                                            req_flags,
                                            sid,
                                            &pt_dyn_info,
                                            NULL,
                                            NULL, /* misc_info */
                                            BCMPTM_OP_WRITE,
                                            (uint32_t *)meter_buf,
                                            0,
                                            req_ltid,
                                            trans_id,
                                            NULL,
                                            NULL,
                                            NULL,
                                            &rsp_ltid,
                                            &rsp_flags));

            meter_index++;
        }
        SHR_FREE(meter_buf);
    }

    if (attr->meter_pool_global == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_util_slice_status_reset(unit,
                                           trans_id,
                                           attr,
                                           pool,
                                           orig_status));
    }

exit:
    SHR_FREE(meter_buf);
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_util_hw_index_free(int unit,
                                bcmcth_meter_fp_attrib_t *attr,
                                bcmcth_meter_fp_sw_state_t *state,
                                uint32_t index,
                                bcmcth_meter_sw_params_t *lt_entry)
{
    int pipe_id = 0;
    bool meter_pairing = false;
    uint32_t meter_mode = 0, actual_hw_idx = 0;
    bool *hw_idx_state = NULL;

    SHR_FUNC_ENTER(unit);

    meter_mode = lt_entry->meter_mode;
    pipe_id = lt_entry->pipe;

    METER_FP_PAIRING_CHECK_ASSIGN(meter_mode, meter_pairing);

    hw_idx_state = state->hw_idx;
    actual_hw_idx = ((attr->num_meters_per_pipe * pipe_id) + index);
    hw_idx_state[actual_hw_idx] = false;

    /*
     * If meter is paired, then free the next index also
     */
    if (meter_pairing == true) {
        hw_idx_state[actual_hw_idx + 1] = false;
    }

    SHR_FUNC_EXIT();
}

int
bcmcth_meter_util_hw_index_alloc(int unit,
                                 bcmcth_meter_fp_attrib_t *attr,
                                 bcmcth_meter_fp_sw_state_t *state,
                                 int pool,
                                 uint8_t oper_mode,
                                 bcmcth_meter_sw_params_t *lt_entry,
                                 uint32_t *hw_idx)
{
    int i = 0;
    int max = 0, min = 0;
    int pipe_id = 0;
    int max_meters = 0;
    bool meter_pairing = false;
    uint32_t meter_mode = 0, actual_hw_idx = 0;
    bool *hw_idx_state = NULL;

    SHR_FUNC_ENTER(unit);

    meter_mode = lt_entry->meter_mode;
    pipe_id = lt_entry->pipe;

    METER_FP_PAIRING_CHECK_ASSIGN(meter_mode, meter_pairing);

    hw_idx_state = state->hw_idx;
    /*
     * Get the min and max index of the particular pool
     */
    min = (attr->num_meters_per_pool * pool);
    max = (min + attr->num_meters_per_pool);

    /*
     * Iterate over the pool of the particular pipe and find a
     * free index
     */
    for (i = min; i < max; i++) {
        actual_hw_idx = ((attr->num_meters_per_pipe * pipe_id) + i);
        if (hw_idx_state[actual_hw_idx] == false) {
            if (meter_pairing == true) {
                /*
                 * Get even index for meter pair
                 */
                if (!(i & 0x1) &&
                    (hw_idx_state[actual_hw_idx + 1] == false)) {
                    *hw_idx = i;
                    break;
                }
            } else {
                *hw_idx = i;
                break;
            }
        }
    }

    if (i == max) {
        if (oper_mode != METER_FP_OPER_MODE_PIPE_UNIQUE) {
            max_meters = attr->num_meters_per_pipe;
        } else {
            max_meters = (attr->num_meters_per_pipe * attr->num_pipes);
        }
        for (i = 0; i < max_meters; i++) {
            if (hw_idx_state[i] == false) {
                break;
            }
        }
        if (i == max_meters) {
            SHR_ERR_EXIT(SHR_E_FULL);
        } else {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_util_hw_oper_get(int unit,
                              bcmcth_meter_fp_attrib_t *attr,
                              bcmcth_meter_fp_sw_state_t *state,
                              bcmcth_meter_sw_params_t *lt_entry)
{
    bool meter_pairing = false;
    int  meter_gran = 0;
    uint32_t burst_size = 0, bucket_size = 0;
    uint32_t refresh_count = 0, meter_id = 0;
    uint64_t meter_rate = 0;
    bcmcth_meter_hw_params_t  *hw_fields;
    bcmcth_meter_fp_entry_t *meter_entry = NULL;

    SHR_FUNC_ENTER(unit);

    meter_id = lt_entry->meter_id;
    meter_entry = &state->list[meter_id];
    hw_fields = &meter_entry->hw_fields;

    METER_FP_PAIRING_CHECK_ASSIGN(lt_entry->meter_mode, meter_pairing);

    meter_gran    = hw_fields->gran[METER_FP_INDEX_ODD];
    bucket_size   = hw_fields->bucket_size[METER_FP_INDEX_ODD];
    refresh_count = hw_fields->refresh_count[METER_FP_INDEX_ODD];

    /*
     * Determine meter rate and burst size (operational values) from
     * refresh count and bucket size
     */
    if (lt_entry->byte_mode == METER_FP_MODE_BYTES) {
        meter_rate = (attr->gran_info_bytes[meter_gran].min_rate *
                      refresh_count);
        burst_size = ((attr->gran_info_bytes[meter_gran].min_burst *
                       bucket_size) / 1000);
    } else {
        meter_rate = (attr->gran_info_pkts[meter_gran].min_rate *
                      refresh_count);
        burst_size = ((attr->gran_info_pkts[meter_gran].min_burst *
                       bucket_size) / 1000);
    }

    /*
     * Check if REFRESHCOUNT needs to be adjusted.
     */
    if (attr->refreshcount_adjust) {
        meter_rate /= 2;
    }

    lt_entry->min_rate_oper  = meter_rate;
    lt_entry->min_burst_oper = burst_size;

    /*
     * If meter is in pairing mode, get the max rates
     */
    if (meter_pairing == false) {
        SHR_EXIT();
    }

    /*
     * Determine meter rate and burst size (operational values) from
     * refresh count and bucket size
     */
    meter_gran    = hw_fields->gran[METER_FP_INDEX_EVEN];
    bucket_size   = hw_fields->bucket_size[METER_FP_INDEX_EVEN];
    refresh_count = hw_fields->refresh_count[METER_FP_INDEX_EVEN];

    if (lt_entry->byte_mode == METER_FP_MODE_BYTES) {
        meter_rate = (attr->gran_info_bytes[meter_gran].min_rate *
                      refresh_count);
        burst_size = ((attr->gran_info_bytes[meter_gran].min_burst *
                       bucket_size) / 1000);
    } else {
        meter_rate = (attr->gran_info_pkts[meter_gran].min_rate *
                      refresh_count);
        burst_size = ((attr->gran_info_pkts[meter_gran].min_burst *
                       bucket_size) / 1000);
    }

    if (attr->refreshcount_adjust) {
        meter_rate /= 2;
    }

    lt_entry->max_rate_oper  = meter_rate;
    lt_entry->max_burst_oper = burst_size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_util_convert_to_hw_params(int unit,
                                       bcmcth_meter_fp_attrib_t *attr,
                                       bcmcth_meter_fp_sw_state_t *state,
                                       bcmcth_meter_sw_params_t *lt_entry)
{
    int gran = 0;
    bool meter_pairing = false;
    uint32_t meter_id = 0;
    uint32_t refresh_count = 0, burst_size = 0;
    uint32_t bucket_size = 0, bucket_count = 0;
    uint64_t min_rate, max_rate;
    uint32_t min_burst, max_burst;
    bcmcth_meter_hw_params_t *hw_fields;
    bcmcth_meter_fp_entry_t *meter_entry = NULL;

    SHR_FUNC_ENTER(unit);

    meter_id = lt_entry->meter_id;
    meter_entry = &state->list[meter_id];
    hw_fields = &meter_entry->hw_fields;

    if (lt_entry->byte_mode == METER_FP_MODE_PKTS) {
        min_rate = lt_entry->min_rate_pps;
        max_rate = lt_entry->max_rate_pps;
        min_burst = lt_entry->min_burst_pkts;
        max_burst = lt_entry->max_burst_pkts;
    } else {
        min_rate = lt_entry->min_rate_kbps;
        max_rate = lt_entry->max_rate_kbps;
        min_burst = lt_entry->min_burst_kbits;
        max_burst = lt_entry->max_burst_kbits;
    }

    hw_fields->refresh_mode = 0;
    switch (lt_entry->meter_mode) {
    case METER_FP_MODE_DEFAULT:
        hw_fields->meter_pair_mode = 0;
        if (lt_entry->color_mode == METER_FP_COLOR_BLIND) {
            hw_fields->mode_modifier = 0;
        } else {
            hw_fields->mode_modifier = 1;
        }
        break;
    case METER_FP_MODE_FLOW:
        hw_fields->meter_pair_mode = 1;
        if (hw_fields->meter_hw_index & 0x1) {
            hw_fields->meter_update_odd = 1;
            hw_fields->meter_test_odd = 1;
            hw_fields->meter_update_even = 0;
            hw_fields->meter_test_even = 0;
        } else {
            hw_fields->meter_update_odd = 0;
            hw_fields->meter_test_odd = 0;
            hw_fields->meter_update_even = 1;
            hw_fields->meter_test_even = 1;
        }
        break;
    case METER_FP_MODE_SRTCM:
        hw_fields->refresh_mode = 1;
        hw_fields->mode_modifier = 0;
        if (lt_entry->color_mode == METER_FP_COLOR_BLIND) {
            hw_fields->meter_pair_mode = 6;
        } else {
            hw_fields->meter_pair_mode = 7;
        }
        break;
    case METER_FP_MODE_MODSRTCM:
        hw_fields->refresh_mode = 1;
        hw_fields->mode_modifier = 1;
        if (lt_entry->color_mode == METER_FP_COLOR_BLIND) {
            hw_fields->meter_pair_mode = 6;
        } else {
            hw_fields->meter_pair_mode = 7;
        }
        break;
    case METER_FP_MODE_TRTCM:
        hw_fields->mode_modifier = 0;
        if (lt_entry->color_mode == METER_FP_COLOR_BLIND) {
            hw_fields->meter_pair_mode = 2;
        } else {
            hw_fields->meter_pair_mode = 3;
        }
        break;
    case METER_FP_MODE_MODTRTCM:
        hw_fields->mode_modifier = 0;
        if (lt_entry->color_mode == METER_FP_COLOR_BLIND) {
            hw_fields->meter_pair_mode = 4;
        } else {
            hw_fields->meter_pair_mode = 5;
        }
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
        break;
    }

    hw_fields->byte_mode = lt_entry->byte_mode;

    /*
     * Calculate granularity, refresh count,
     * bucket size for min rate, max rate
     */
    if (min_rate != 0) {
        burst_size = (min_burst * 1000);
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_util_compute_gran_params(unit,
                                            attr,
                                            min_rate,
                                            burst_size,
                                            hw_fields->byte_mode,
                                            &gran,
                                            &refresh_count,
                                            &bucket_size,
                                            &bucket_count));

        hw_fields->gran[METER_FP_INDEX_ODD] = gran;
        hw_fields->refresh_count[METER_FP_INDEX_ODD] = refresh_count;
        hw_fields->bucket_size[METER_FP_INDEX_ODD] = bucket_size;
        hw_fields->bucket_count[METER_FP_INDEX_ODD] = bucket_count;
    } else {
        hw_fields->gran[METER_FP_INDEX_ODD] = 0;
        hw_fields->refresh_count[METER_FP_INDEX_ODD] = 0;
        hw_fields->bucket_size[METER_FP_INDEX_ODD] = 0;
        hw_fields->bucket_count[METER_FP_INDEX_ODD] = 0;
    }

    METER_FP_PAIRING_CHECK_ASSIGN(lt_entry->meter_mode, meter_pairing);
    if (meter_pairing == true) {
        burst_size = (max_burst * 1000);
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_util_compute_gran_params(unit,
                                            attr,
                                            max_rate,
                                            burst_size,
                                            hw_fields->byte_mode,
                                            &gran,
                                            &refresh_count,
                                            &bucket_size,
                                            &bucket_count));

        hw_fields->gran[METER_FP_INDEX_EVEN] = gran;
        hw_fields->refresh_count[METER_FP_INDEX_EVEN] = refresh_count;
        hw_fields->bucket_size[METER_FP_INDEX_EVEN] = bucket_size;
        hw_fields->bucket_count[METER_FP_INDEX_EVEN] = bucket_count;
    } else {
        hw_fields->gran[METER_FP_INDEX_EVEN] = 0;
        hw_fields->refresh_count[METER_FP_INDEX_EVEN] = 0;
        hw_fields->bucket_size[METER_FP_INDEX_EVEN] = 0;
        hw_fields->bucket_count[METER_FP_INDEX_EVEN] = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_util_policy_format_get(int unit,
                                    bcmcth_meter_fp_attrib_t *attr,
                                    bcmcth_meter_fp_sw_state_t *state,
                                    bcmcth_meter_sw_params_t *lt_entry)
{
    uint32_t i = 0, meter_id = 0, hw_val = 0;
    bcmcth_meter_hw_params_t  *hw_fields;
    bcmcth_meter_fp_entry_t *meter_entry = NULL;
    bcmcth_meter_format_set_t *format_info = NULL;

    SHR_FUNC_ENTER(unit);

    meter_id = lt_entry->meter_id;
    meter_entry = &state->list[meter_id];
    hw_fields = &meter_entry->hw_fields;

    lt_entry->meter_format = 0;
    hw_val = 0;
    for (i = 0; i < METER_FIELD_COUNT; i++) {
        format_info = &attr->meter_format_info[i];
        switch (format_info->field) {
        case METER_PAIR_INDEX:
            hw_val = hw_fields->meter_pair_index;
            break;
        case METER_PAIR_MODE:
            hw_val = hw_fields->meter_pair_mode;
            break;
        case METER_PAIR_MODE_MODIFIER:
            hw_val = hw_fields->mode_modifier;
            break;
        case METER_TEST_EVEN:
            hw_val = hw_fields->meter_test_even;
            break;
        case METER_TEST_ODD:
            hw_val = hw_fields->meter_test_odd;
            break;
        case METER_UPDATE_EVEN:
            hw_val = hw_fields->meter_update_even;
            break;
        case METER_UPDATE_ODD:
            hw_val = hw_fields->meter_update_odd;
            break;
        default:
            break;
        }
        lt_entry->meter_format |= (hw_val << format_info->s_bit);
    }

    SHR_FUNC_EXIT();
}

int
bcmcth_meter_entry_field_value_get(int unit,
                                   const bcmltd_fields_t *in,
                                   uint32_t fid,
                                   uint32_t *rval)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < in->count; i++) {
        if (in->field[i] == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (in->field[i]->id == fid) {
            *rval = in->field[i]->data;
            break;
        }
    }
    if (i == in->count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

void
meter_field_def_val_get(bcmlrd_field_def_t *fld_def,
                        uint64_t *data)
{
    if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U8) {
        *data = fld_def->def.u8;
    } else if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U16) {
        *data = fld_def->def.u16;
    } else if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U32) {
        *data = fld_def->def.u32;
    } else if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U64) {
        *data = fld_def->def.u64;
    } else {
        *data = fld_def->def.is_true;
    }
}

void
meter_field_min_val_get(bcmlrd_field_def_t *fld_def,
                        uint64_t *data)
{
    if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U8) {
        *data = fld_def->min.u8;
    } else if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U16) {
        *data = fld_def->min.u16;
    } else if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U32) {
        *data = fld_def->min.u32;
    } else if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U64) {
        *data = fld_def->min.u64;
    } else {
        *data = fld_def->min.is_true;
    }
}

void
meter_field_max_val_get(bcmlrd_field_def_t *fld_def,
                        uint64_t *data)
{
    if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U8) {
        *data = fld_def->max.u8;
    } else if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U16) {
        *data = fld_def->max.u16;
    } else if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U32) {
        *data = fld_def->max.u32;
    } else if (fld_def->dtag == BCMLT_FIELD_DATA_TAG_U64) {
        *data = fld_def->max.u64;
    } else {
        *data = fld_def->max.is_true;
    }
}
