/*! \file bcmcth_meter_fp_egr_info.c
 *
 * This file contains EFP Meter device info handler functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_meter_fp_egr.h>

#include <bcmcth/bcmcth_meter_fp_egr_info.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Private functions
 */

static int
meter_fp_egr_device_info(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    size_t num_fid = 0, count = 0;
    uint8_t oper_mode = 0;
    uint32_t idx = 0, fid = 0;
    uint32_t table_sz = 0;
    uint32_t entries_in_use = 0;
    bcmlrd_fid_t *fid_list = NULL;
    bcmltd_field_t *foutlist = NULL;
    bcmcth_meter_fp_attrib_t *attr = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_fp_oper_mode_get(unit,
                                       METER_FP_STAGE_ID_EGRESS,
                                       &oper_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_util_used_entries_calc(unit,
                                             METER_FP_STAGE_ID_EGRESS,
                                             &entries_in_use));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, METER_EGR_FP_DEVICE_INFOt, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "bcmcthMeterFldList");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               METER_EGR_FP_DEVICE_INFOt,
                               num_fid,
                               fid_list,
                               &count));

    foutlist = out->field[0];

    for (idx = 0; idx < count; idx++) {
        fid = fid_list[idx];
        foutlist[idx].id = fid;

        switch (fid) {
        case METER_EGR_FP_DEVICE_INFOt_NUM_METER_POOLSf:
            foutlist[idx].data = attr->num_pools;
            break;
        case METER_EGR_FP_DEVICE_INFOt_NUM_METERS_PER_POOLf:
            foutlist[idx].data = attr->num_meters_per_pool;
            break;
        case METER_EGR_FP_DEVICE_INFOt_NUM_METERS_PER_PIPEf:
            foutlist[idx].data = attr->num_meters_per_pipe;
            break;
        case METER_EGR_FP_DEVICE_INFOt_NUM_METERSf:
            if (oper_mode == METER_FP_OPER_MODE_PIPE_UNIQUE) {
                foutlist[idx].data = (attr->num_meters_per_pipe * attr->num_pipes);
            } else {
                foutlist[idx].data = (attr->num_meters_per_pipe);
            }
            break;
        case METER_EGR_FP_DEVICE_INFOt_NUM_METERS_IN_USEf:
            foutlist[idx].data = entries_in_use;
            break;
        case METER_EGR_FP_DEVICE_INFOt_NUM_GRANULARITYf:
            foutlist[idx].data = attr->max_granularity;
            break;
        default:
            break;
        }
    }

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

static int
meter_fp_egr_gran_info(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       uint32_t gran,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    size_t num_fid = 0, count = 0;
    uint32_t i = 0, fid = 0;
    uint32_t table_sz = 0;
    uint32_t max_burst = 0;
    bcmlrd_fid_t *fid_list = NULL;
    bcmltd_field_t *foutlist = NULL;
    bcmcth_meter_fp_gran_info_t *gran_info_bytes = NULL;
    bcmcth_meter_fp_gran_info_t *gran_info_pkts = NULL;
    bcmcth_meter_fp_attrib_t *attr = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    gran_info_bytes = attr->gran_info_bytes;
    gran_info_pkts = attr->gran_info_pkts;
    if ((gran_info_bytes == NULL) || (gran_info_pkts == NULL)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                METER_EGR_FP_GRANULARITY_INFOt,
                                &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "bcmcthMeterFldList");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               METER_EGR_FP_GRANULARITY_INFOt,
                               num_fid,
                               fid_list,
                               &count));

    foutlist = out->field[0];
    for (i = 0; i < count; i++) {
        fid = fid_list[i];
        foutlist[i].id = fid;
        switch (i) {
        case METER_EGR_FP_GRANULARITY_INFOt_METER_EGR_FP_GRANULARITY_INFO_IDf:
            foutlist[i].data = gran;
            break;
        case METER_EGR_FP_GRANULARITY_INFOt_MIN_RATE_KBPSf:
            foutlist[i].data = gran_info_bytes[gran].min_rate;
            if (attr->refreshcount_adjust) {
                foutlist[i].data /= 2;
            }
            if (foutlist[i].data == 0) {
                foutlist[i].data = 1;
            }
            break;
        case METER_EGR_FP_GRANULARITY_INFOt_MAX_RATE_KBPSf:
            foutlist[i].data = gran_info_bytes[gran].max_rate;
            if (attr->refreshcount_adjust) {
                foutlist[i].data /= 2;
            }
            break;
        case METER_EGR_FP_GRANULARITY_INFOt_MIN_BURST_KBITSf:
            foutlist[i].data = (gran_info_bytes[gran].min_burst / 1000);
            if (foutlist[i].data == 0) {
                foutlist[i].data = 1;
            }
            break;
        case METER_EGR_FP_GRANULARITY_INFOt_MAX_BURST_KBITSf:
            max_burst = ((gran_info_bytes[gran].min_burst *
                          attr->max_bucket_size) / 1000);
            foutlist[i].data = max_burst;
            break;
        case METER_EGR_FP_GRANULARITY_INFOt_MIN_RATE_PPSf:
            foutlist[i].data = gran_info_pkts[gran].min_rate;
            if (attr->refreshcount_adjust) {
                foutlist[i].data /= 2;
            }
            if (foutlist[i].data == 0) {
                foutlist[i].data = 1;
            }
            break;
        case METER_EGR_FP_GRANULARITY_INFOt_MAX_RATE_PPSf:
            foutlist[i].data = gran_info_pkts[gran].max_rate;
            if (attr->refreshcount_adjust) {
                foutlist[i].data /= 2;
            }
            break;
        case METER_EGR_FP_GRANULARITY_INFOt_MIN_BURST_PKTSf:
            foutlist[i].data = (gran_info_pkts[gran].min_burst / 1000);
            if (foutlist[i].data == 0) {
                foutlist[i].data = 1;
            }
            break;
        case METER_EGR_FP_GRANULARITY_INFOt_MAX_BURST_PKTSf:
            max_burst = ((gran_info_pkts[gran].min_burst *
                          attr->max_bucket_size) / 1000);
            foutlist[i].data = max_burst;
        default:
            break;
        }
    }
exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmcth_meter_fp_egr_info_validate(int unit,
                                  bcmlt_opcode_t opcode,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    if ((opcode != BCMLT_OPCODE_LOOKUP) &&
        (opcode != BCMLT_OPCODE_TRAVERSE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_info_insert(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_info_lookup(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    uint32_t        sid = 0, fid = 0, gran = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    sid = arg->sid;

    switch (sid) {
    case METER_EGR_FP_DEVICE_INFOt:
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_fp_egr_device_info(unit, op_arg, out, arg));
        break;
    case METER_EGR_FP_GRANULARITY_INFOt:
        fid = METER_EGR_FP_GRANULARITY_INFOt_METER_EGR_FP_GRANULARITY_INFO_IDf;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_entry_field_value_get(unit, in, fid, &gran));

        SHR_IF_ERR_VERBOSE_EXIT
            (meter_fp_egr_gran_info(unit, op_arg, gran, out, arg));
        break;

    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_info_delete(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_info_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_info_first(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg)
{
    uint32_t            sid = 0, fid = 0, gran = 0;
    bcmlrd_field_def_t  field_def;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    sid = arg->sid;

    switch (sid) {
    case METER_EGR_FP_DEVICE_INFOt:
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_fp_egr_device_info(unit, op_arg, out, arg));
        break;
    case METER_EGR_FP_GRANULARITY_INFOt:
        sid = METER_EGR_FP_GRANULARITY_INFOt;
        fid = METER_EGR_FP_GRANULARITY_INFOt_METER_EGR_FP_GRANULARITY_INFO_IDf;
        sal_memset(&field_def, 0, sizeof(field_def));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_field_def_get(unit, sid, fid, &field_def));
        gran = METER_FP_FIELD_MIN(field_def);
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_fp_egr_gran_info(unit, op_arg, gran, out, arg));
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_info_next(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    uint32_t sid = 0, fid = 0, gran = 0;
    bcmcth_meter_fp_attrib_t *attr = NULL;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    sid = arg->sid;

    switch (sid) {
    case METER_EGR_FP_DEVICE_INFOt:
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_fp_egr_device_info(unit, op_arg, out, arg));
        break;
    case METER_EGR_FP_GRANULARITY_INFOt:
        fid = METER_EGR_FP_GRANULARITY_INFOt_METER_EGR_FP_GRANULARITY_INFO_IDf;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_meter_entry_field_value_get(unit, in, fid, &gran));
        if ((gran >= attr->max_granularity) ||
            ((gran + 1) >= attr->max_granularity)) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        gran = (gran + 1);

        SHR_IF_ERR_VERBOSE_EXIT
            (meter_fp_egr_gran_info(unit, op_arg, gran, out, arg));
        break;

    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();

}
