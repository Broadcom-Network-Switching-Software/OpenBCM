/*! \file bcm56990_b0_cth_mon.c
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
#include <bcmdrd/chip/bcm56990_b0_defs.h>
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

/*******************************************************************************
 * Local functions
 */

static int
bcm56990_b0_cth_mon_ctr_clr(
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
bcm56990_b0_cth_mon_ctr_get(
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
bcm56990_b0_cth_mon_ctr_op(
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
                (bcm56990_b0_cth_mon_ctr_clr(unit, trans_id, lt_id));
            break;
        case BCMCTH_MON_CTR_OP_GET:
            SHR_NULL_CHECK(entry, SHR_E_PARAM);
            SHR_IF_ERR_EXIT
                (bcm56990_b0_cth_mon_ctr_get(unit, trans_id, lt_id, entry));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56990_b0_cth_mon_deinit(int unit)
{
    return SHR_E_NONE;
}

static int
bcm56990_b0_cth_mon_init(int unit, bool warm)
{
    return SHR_E_NONE;
}

static bcmcth_mon_drv_t bcm56990_b0_cth_mon_drv = {
   .mon_init = &bcm56990_b0_cth_mon_init,
   .mon_deinit = &bcm56990_b0_cth_mon_deinit,
   .ctr_op = &bcm56990_b0_cth_mon_ctr_op,
};

/*******************************************************************************
 * Public functions
 */

bcmcth_mon_drv_t *
bcm56990_b0_cth_mon_drv_get(int unit)
{
    return &bcm56990_b0_cth_mon_drv;
}

