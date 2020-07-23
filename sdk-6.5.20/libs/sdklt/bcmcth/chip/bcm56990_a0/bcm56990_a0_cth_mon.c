/*! \file bcm56990_a0_cth_mon.c
 *
 * Chip stub for BCMCTH MON.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmcth/bcmcth_mon_drv.h>
#include <bcmcth/bcmcth_imm_util.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmcth/bcmcth_mon_telemetry_drv.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56990_a0_defs.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/* MTPP numbers */
#define MTPP_NUMBER (2)

/*******************************************************************************
 * Private variables
 */

/* Valid counters */
static const bool pt_id_crt_vld[BCMCTH_MON_CTR_COUNT] = {
    TRUE, /* BCMCTH_MON_CTR_ETRAP_CANDIDATE_FILTER_EXCEEDED */
    TRUE, /* BCMCTH_MON_CTR_ETRAP_FLOW_INSERT_SUCCESS */
    TRUE, /* BCMCTH_MON_CTR_ETRAP_FLOW_INSERT_FAILURE */
    TRUE, /* BCMCTH_MON_CTR_ETRAP_FLOW_ELEPHANT */
};

/* Counter H/W register mapping */
static const bcmdrd_sid_t pt_id_ctr[BCMCTH_MON_CTR_COUNT][MTPP_NUMBER] = {
    {ETRAP_FILT_EXCEED_CTR_INST0r, ETRAP_FILT_EXCEED_CTR_INST1r},
    {ETRAP_FLOW_INS_SUCCESS_CTR_INST0r, ETRAP_FLOW_INS_SUCCESS_CTR_INST1r},
    {ETRAP_FLOW_INS_FAIL_CTR_INST0r, ETRAP_FLOW_INS_FAIL_CTR_INST1r},
    {ETRAP_FLOW_DETECT_CTR_INST0r, ETRAP_FLOW_DETECT_CTR_INST1r},
};

/*
 * Device specified!
 * Inspected based on EGR_MD_HDR_CONST_INPUT_BUS, this bus can be logicaly
 * split as 16 bits or 32 bits chunks.
 */
static bcmcth_mon_inband_telemetry_metadata_field_t field_info_tbl[] =
{
    /* PAD_ONES. */
    {0, 32, 0, 0, 0, 0},
    /* PAD_ZEROES. */
    {1, 32, 1, 0, 2, 0},
    /* OPAQUE_MD_FIELD_3. */
    {2, 32, 2, 0, 4, 0},
    /* OPAQUE_MD_FIELD_2. */
    {3, 32, 3, 0, 6, 0},
    /* OPAQUE_MD_FIELD_1. */
    {4, 32, 4, 0, 8, 0},
    /* SWITCH_ID. */
    {5, 32, 5, 0, 10, 0},
    /* TRANSIT_DELAY_SUBSECONDS. */
    {6, 32, 6, 0, 12, 0},
    /* MMU_STAT_1. */
    {7, 32, 7, 0, 14, 0},
    /* MMU_STAT_0. */
    {8, 32, 8, 0, 16, 0},
    /* INGRESS_TIMESTAMP_SUB_SECONDS. */
    {9, 32, 9, 0, 18, 0},
    /* EGRESS_TIMESTAMP_SUB_SECONDS. */
    {10, 32, 10, 0, 20, 0},
    /* INGRESS_TIMESTAMP_SECONDS_LOWER. */
    {11, 32, 11, 0, 22, 0},
    /* EGRESS_TIMESTAMP_SECONDS_LOWER. */
    {12, 32, 12, 0, 24, 0},
    /* INGRESS_TIMESTAMP_SECONDS_UPPER. */
    {13, 16, 13, 0, 26, 0},
    /* EGRESS_TIMESTAMP_SECONDS_UPPER. */
    {14, 16, 13, 16, 27, 0},
    /* TRANSIT_DELAY_SECONDS. */
    {15, 16, 14, 0, 28, 0},
    /* L3_OIF. */
    {16, 16, 14, 16, 29, 0},
    /* L3_IIF. */
    {17, 16, 15, 0, 30, 0},
    /* INGRESS_INTERFACE_ID. */
    {18, 16, 15, 16, 31, 0},
    /* EGRESS_INTERFACE_ID. */
    {19, 16, 16, 0, 32, 0},
    /* HOP_LIMIT, not supported. */
    {20, 8, 16, 16, 33, 0},
    /* OUTGOING_IP_TTL, not supported. */
    {21, 8, 16, 24, 33, 8},
    /* NODELEN, not supported. */
    {22, 8, 17, 0, 34, 0},
    /* TEMPLATE_ID, not supported. */
    {23, 4, 17, 8, 34, 8},
    /* CONGESTION. */
    {24, 4, 17, 12, 34, 12},
    /* RESERVED_1, not supported. */
    {25, 6, 17, 16, 35, 0},
    /* QUEUE_ID. */
    {26, 6, 17, 22, 35, 6},
    /* MD_HDR_TYPE. */
    {27, 3, 17, 28, 35, 12},
    /* CUT_THROUGH. */
    {28, 1, 17, 31, 35, 15},
};

/*
 * Device specified!
 * Index mapping in case some fields listed in HW are actually not supported.
 * index: enum in INBAND_TELEMETRY_METADATA_FIELD_T
 * mapped index: fld_enum in field_info_tbl
 */
static uint32_t field_info_index_map[] =
{
    /* PAD_ONES. */
    0,
    /* PAD_ZEROES. */
    1,
    /* OPAQUE_MD_FIELD_3. */
    2,
    /* OPAQUE_MD_FIELD_2. */
    3,
    /* OPAQUE_MD_FIELD_1. */
    4,
    /* SWITCH_ID. */
    5,
    /* TRANSIT_DELAY_SUBSECONDS. */
    6,
    /* MMU_STAT_1. */
    7,
    /* MMU_STAT_0. */
    8,
    /* INGRESS_TIMESTAMP_SUB_SECONDS. */
    9,
    /* EGRESS_TIMESTAMP_SUB_SECONDS. */
    10,
    /* INGRESS_TIMESTAMP_SECONDS_LOWER. */
    11,
    /* EGRESS_TIMESTAMP_SECONDS_LOWER. */
    12,
    /* INGRESS_TIMESTAMP_SECONDS_UPPER. */
    13,
    /* EGRESS_TIMESTAMP_SECONDS_UPPER. */
    14,
    /* TRANSIT_DELAY_SECONDS. */
    15,
    /* L3_OIF. */
    16,
    /* L3_IIF. */
    17,
    /* INGRESS_INTERFACE_ID. */
    18,
    /* EGRESS_INTERFACE_ID. */
    19,
    /* CONGESTION. */
    24,
    /* QUEUE_ID. */
    26,
    /* MD_HDR_TYPE. */
    27,
    /* CUT_THROUGH. */
    28,
};

/*******************************************************************************
 * Local functions
 */

static int
bcm56990_a0_cth_mon_ctr_clr(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    bcmcth_mon_ctr_type_t ctype;
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    int idx, mtpp, pipe, pipe_num;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;
    uint32_t sub_pipe_map;
    uint32_t fbuf[2] = {0, 0};

    SHR_FUNC_ENTER(unit);

    for (ctype = BCMCTH_MON_CTR_FIRST;
         ctype < BCMCTH_MON_CTR_COUNT;
         ctype++) {
        if (pt_id_crt_vld[ctype] == FALSE) {
            continue;
        }

        /* Allocate buffer */
        pt_id = pt_id_ctr[ctype][0];
        entry_sz = bcmdrd_pt_entry_wsize(unit, pt_id) * 4;
        SHR_ALLOC(entry_buf, entry_sz, "bcmcthMonCtrEntry");
        SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

        /* pipe number */
        idx = 0;
        pt_fid = COUNTf;
        pipe_num = bcmdrd_pt_num_tbl_inst(unit, pt_id);

        /* pipe */
        for (pipe = 0; pipe < pipe_num; pipe++) {
            /* pipe instance (MTPP) */
            for (mtpp = 0; mtpp < MTPP_NUMBER; mtpp++) {
                /* validate pipe and pie instance */
                sal_memset(pi, 0, sizeof(*pi));
                pi->pipe_inst = pipe;
                pi->sub_pipe_inst = mtpp;
                sub_pipe_map =
                    bcmdrd_dev_pipe_info(unit, pi,
                                         BCMDRD_PIPE_INFO_TYPE_SUB_PIPE_MAP);
                if (sub_pipe_map == 0) {
                    continue;
                }

                pt_id = pt_id_ctr[ctype][mtpp];
                sal_memset(entry_buf, 0, entry_sz);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_uint64_to_uint32_array((uint64_t)0, fbuf));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmdrd_pt_field_set(
                        unit, pt_id, entry_buf, pt_fid, fbuf));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_imm_pipe_ireq_write(
                        unit, lt_id, pt_id, pipe, idx, entry_buf));
            }
        }

        /* Free buffer */
        if (entry_buf) {
            SHR_FREE(entry_buf);
        }
    }
exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_mon_ctr_get(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_ctr_t *entry)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    bcmcth_mon_ctr_type_t ctype;
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    int idx, mtpp, pipe, pipe_num;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;
    uint32_t sub_pipe_map;
    uint32_t fbuf[2] = {0, 0};
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    for (ctype = BCMCTH_MON_CTR_FIRST;
         ctype < BCMCTH_MON_CTR_COUNT;
         ctype++) {
        if (pt_id_crt_vld[ctype] == FALSE) {
            SHR_BITCLR(entry->fbmp, ctype);
            continue;
        }
        SHR_BITSET(entry->fbmp, ctype);
        entry->cnt[ctype] = 0;

        /* Allocate buffer */
        pt_id = pt_id_ctr[ctype][0];
        entry_sz = bcmdrd_pt_entry_wsize(unit, pt_id) * 4;
        SHR_ALLOC(entry_buf, entry_sz, "bcmcthMonCtrEntry");
        SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

        /* pipe number */
        idx = 0;
        pt_fid = COUNTf;
        pipe_num = bcmdrd_pt_num_tbl_inst(unit, pt_id);

        /* pipe */
        for (pipe = 0; pipe < pipe_num; pipe++) {
            /* pipe instance (MTPP) */
            for (mtpp = 0; mtpp < MTPP_NUMBER; mtpp++) {
                /* validate pipe and pie instance */
                sal_memset(pi, 0, sizeof(*pi));
                pi->pipe_inst = pipe;
                pi->sub_pipe_inst = mtpp;
                sub_pipe_map =
                    bcmdrd_dev_pipe_info(unit, pi,
                                         BCMDRD_PIPE_INFO_TYPE_SUB_PIPE_MAP);
                if (sub_pipe_map == 0) {
                    continue;
                }

                pt_id = pt_id_ctr[ctype][mtpp];
                sal_memset(entry_buf, 0, entry_sz);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_imm_pipe_ireq_read(
                        unit, lt_id, pt_id, pipe, idx, entry_buf));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmdrd_pt_field_get(
                        unit, pt_id, entry_buf, pt_fid, fbuf));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_uint32_array_to_uint64(fbuf, &fval));

                entry->cnt[ctype] += fval;
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "pipe(%d), mtpp(%d) fval=%"PRIu64"\n"),
                           pipe, mtpp, fval));
            }
        }

        /* Free buffer */
        if (entry_buf) {
            SHR_FREE(entry_buf);
        }
    }
exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_mon_ctr_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_ctr_op_t op,
    bcmcth_mon_ctr_t *entry)
{
    SHR_FUNC_ENTER(unit);

    switch (op) {
        case BCMCTH_MON_CTR_OP_CLR:
            SHR_IF_ERR_EXIT
                (bcm56990_a0_cth_mon_ctr_clr(unit, trans_id, lt_id));
            break;
        case BCMCTH_MON_CTR_OP_GET:
            SHR_NULL_CHECK(entry, SHR_E_PARAM);
            SHR_IF_ERR_EXIT
                (bcm56990_a0_cth_mon_ctr_get(unit, trans_id, lt_id, entry));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56990_a0_mon_telemetry_attrs_info_get(
    int unit,
    bcmcth_mon_inband_telemetry_metadata_attrs_t *attrs_info)
{
    attrs_info->narrow_size = 16;
    attrs_info->wide_size = 32;
    attrs_info->field_num = COUNTOF(field_info_index_map);;

    return SHR_E_NONE;
}

static int
bcm56990_a0_mon_telemetry_field_info_index_map(int unit, uint32_t fld_enum,
                                               uint32_t *fld_index)
{
    uint32_t field_num = 0;

    SHR_FUNC_ENTER(unit);

    *fld_index = 0;
    field_num = COUNTOF(field_info_index_map);
    if (fld_enum >= field_num) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else {
        *fld_index = field_info_index_map[fld_enum];
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_mon_telemetry_field_info_get(
    int unit,
    bcmcth_mon_inband_telemetry_metadata_field_t *field_info)
{
    uint32_t index = 0;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcm56990_a0_mon_telemetry_field_info_index_map(unit,
                                                        field_info->fld_enum,
                                                        &index));
    field_info->fld_size = field_info_tbl[index].fld_size;
    field_info->wide_index = field_info_tbl[index].wide_index;
    field_info->wide_start = field_info_tbl[index].wide_start;
    field_info->narrow_index = field_info_tbl[index].narrow_index;
    field_info->narrow_start = field_info_tbl[index].narrow_start;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_mon_deinit(int unit)
{
    return SHR_E_NONE;
}

static int
bcm56990_a0_cth_mon_init(int unit, bool warm)
{
    return SHR_E_NONE;
}

static bcmcth_mon_drv_t bcm56990_a0_cth_mon_drv = {
   .mon_init = &bcm56990_a0_cth_mon_init,
   .mon_deinit = &bcm56990_a0_cth_mon_deinit,
   .ctr_op = &bcm56990_a0_cth_mon_ctr_op,
   .telemetry_attrs_info = &bcm56990_a0_mon_telemetry_attrs_info_get,
   .telemetry_field_info = &bcm56990_a0_mon_telemetry_field_info_get,
};

/*******************************************************************************
 * Public functions
 */

bcmcth_mon_drv_t *
bcm56990_a0_cth_mon_drv_get(int unit)
{
    return &bcm56990_a0_cth_mon_drv;
}

