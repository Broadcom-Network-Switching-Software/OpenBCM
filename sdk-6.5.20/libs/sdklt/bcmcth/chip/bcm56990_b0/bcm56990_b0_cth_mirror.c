/*! \file bcm56990_b0_cth_mirror.c
 *
 * Chip stub for BCMCTH MIRROR.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <shr/shr_util.h>
#include <bcmcth/bcmcth_mirror_drv.h>
#include <bcmcth/bcmcth_mirror_util.h>
#include <bcmcth/bcmcth_imm_util.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56990_b0_defs.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_cci.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MIRROR


/*******************************************************************************
 * Private variables
 */
/* Flex sFlow control structure. */
static bcmcth_mirror_flex_sflow_control_t *flex_sflow_control[BCMDRD_CONFIG_MAX_UNITS];

/*! Write flex sFlow stats */
static int
bcm56990_b0_cth_mirror_ctr_flex_sflow_write(
    int unit,
    bcmcth_mirror_flex_sflow_stat_t *info)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    bcmdrd_sid_t sid_inst[2] = {SFLOW_ING_FLEX_DATA_SOURCE_INST0m,
                                SFLOW_ING_FLEX_DATA_SOURCE_INST1m};
    uint32_t entry_sz = 0;
    uint32_t *pt_entry_buff = NULL;
    uint32_t rsp_flags = 0, rsp_entry_wsize = 0;
    uint32_t inst_idx, num_insts = 2;
    uint32_t pipe_idx, pipe_start, pipe_end;
    uint32_t tmp_val[2] = {0};
    ctr_sflow_field_t *lt_field = NULL;
    uint16_t field_width = 0;
    uint64_t req_flags = 0;
    bool sampled_pkt_updated = 0;
    bool sampling_eligible_pkt_updated = 0;
    bool eligible_pkt_snapshot_updated = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    entry_sz = flex_sflow_control[unit]->entry_wsize;
    rsp_entry_wsize = entry_sz;

    /* Allocate PT entry buffer */
    SHR_ALLOC(pt_entry_buff, (entry_sz * sizeof(uint32_t)),
              "bcmcthirrorCtrSflowStats");
    SHR_NULL_CHECK(pt_entry_buff, SHR_E_MEMORY);

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(info->attrib);
    /* Write flex Sflow stats for all applicable pipes */
    if (info->pipe_idx == -1) {
        pipe_start = 0;
        pipe_end = 4;
    } else {
        pipe_start = info->pipe_idx;
        pipe_end = pipe_start + 1;
    }

    sampled_pkt_updated = 0;
    sampling_eligible_pkt_updated = 0;
    eligible_pkt_snapshot_updated = 0;
    for (pipe_idx = pipe_start; pipe_idx < pipe_end; pipe_idx++) {
        sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
        pt_dyn_info.index = info->flex_sflow_id;
        pt_dyn_info.tbl_inst = pipe_idx;

        /*
         * Write counter values to one instance of one pipe, zero for all
         * other instance, pipe.
         */
        for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
            /* Ignore disabled pipes */
            if (!bcmdrd_pt_index_valid(unit, sid_inst[inst_idx],
                                       pipe_idx, -1)) {
                continue;
            }
            sal_memset(pt_entry_buff, 0, (entry_sz * sizeof(uint32_t)));
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_ltm_ireq_read(unit,
                                                         req_flags,
                                                         sid_inst[inst_idx],
                                                         &pt_dyn_info,
                                                         NULL,
                                                         rsp_entry_wsize,
                                                         req_ltid,
                                                         pt_entry_buff,
                                                         &rsp_ltid,
                                                         &rsp_flags));
            /* Modify PT entry field */
            if (info->upd_sampled_pkt) {
                lt_field = &(flex_sflow_control[unit]->lt_field[0]);
                field_width = lt_field->maxbit - lt_field->minbit + 1;
                if (sampled_pkt_updated) {
                    tmp_val[0] = 0;
                    tmp_val[1] = 0;
                } else {
                    tmp_val[0] = (uint32_t)(info->sampled_pkt);
                    tmp_val[1] = (uint32_t)(info->sampled_pkt >> 32);
                    sampled_pkt_updated = 1;
                }
                bcmptm_cci_buff32_field_copy(pt_entry_buff, lt_field->minbit,
                                             tmp_val, 0, field_width);
            }
            if (info->upd_sampling_eligible_pkt) {
                lt_field = &(flex_sflow_control[unit]->lt_field[1]);
                field_width = lt_field->maxbit - lt_field->minbit + 1;
                if (sampling_eligible_pkt_updated) {
                    tmp_val[0] = 0;
                    tmp_val[1] = 0;
                } else {
                    tmp_val[0] = (uint32_t)(info->sampling_eligible_pkt);
                    tmp_val[1] = (uint32_t)(info->sampling_eligible_pkt >> 32);
                    sampling_eligible_pkt_updated = 1;
                }
                bcmptm_cci_buff32_field_copy(pt_entry_buff, lt_field->minbit,
                                             tmp_val, 0, field_width);
            }
            if (info->upd_eligible_pkt_snapshot) {
                lt_field = &(flex_sflow_control[unit]->lt_field[2]);
                field_width = lt_field->maxbit - lt_field->minbit + 1;
                if (eligible_pkt_snapshot_updated) {
                    tmp_val[0] = 0;
                    tmp_val[1] = 0;
                } else {
                    tmp_val[0] = (uint32_t)(info->eligible_pkt_snapshot);
                    tmp_val[1] = (uint32_t)(info->eligible_pkt_snapshot >> 32);
                    eligible_pkt_snapshot_updated = 1;
                }
                bcmptm_cci_buff32_field_copy(pt_entry_buff, lt_field->minbit,
                                             tmp_val, 0, field_width);
            }

            /* Write PT entry */
            SHR_IF_ERR_EXIT
                (bcmptm_ltm_ireq_write(unit,
                                       req_flags,
                                       sid_inst[inst_idx],
                                       &pt_dyn_info,
                                       NULL,
                                       pt_entry_buff,
                                       req_ltid,
                                       &rsp_ltid,
                                       &rsp_flags));
        }/* end FOR inst */
    } /* end FOR pipe */

exit:
    if (pt_entry_buff) {
        SHR_FREE(pt_entry_buff);
    }
    SHR_FUNC_EXIT();
}

/*! Read flex sFlow stats */
static int
bcm56990_b0_cth_mirror_ctr_flex_sflow_read(
    int unit,
    bcmcth_mirror_flex_sflow_stat_t *info)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t req_ltid = 0, rsp_ltid = 0;
    bcmdrd_sid_t sid_inst[2] = {SFLOW_ING_FLEX_DATA_SOURCE_INST0m,
                                SFLOW_ING_FLEX_DATA_SOURCE_INST1m};
    uint32_t entry_sz = 0;
    uint32_t *pt_entry_buff = NULL;
    uint32_t rsp_flags = 0, rsp_entry_wsize = 0;
    uint32_t inst_idx, num_insts = 2;
    uint32_t pipe_idx, pipe_start, pipe_end;
    uint32_t tmp_val[2] = {0};
    ctr_sflow_field_t *lt_field = NULL;
    uint16_t field_width = 0;
    uint64_t req_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Entry size in words */
    entry_sz = flex_sflow_control[unit]->entry_wsize;
    rsp_entry_wsize = entry_sz;

    /* Allocate PT entry buffer */
    SHR_ALLOC(pt_entry_buff, (entry_sz * sizeof(uint32_t)),
              "bcmcthirrorCtrSflowStats");
    SHR_NULL_CHECK(pt_entry_buff, SHR_E_MEMORY);

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(info->attrib);
    /* Read flex sFlow stats from all applicable pipes */
    if (info->pipe_idx == -1) {
        pipe_start = 0;
        pipe_end = 4;
    } else {
        pipe_start = info->pipe_idx;
        pipe_end = pipe_start + 1;
    }
    for (pipe_idx = pipe_start; pipe_idx < pipe_end; pipe_idx++) {
        sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));
        pt_dyn_info.index = info->flex_sflow_id;
        pt_dyn_info.tbl_inst = pipe_idx;

        /* Get counter values from all instances in pipe */
        for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
            /* Ignore disabled pipes */
            if (!bcmdrd_pt_index_valid(unit, sid_inst[inst_idx],
                                       pipe_idx, -1)) {
                continue;
            }
            sal_memset(pt_entry_buff, 0, (entry_sz * sizeof(uint32_t)));
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_ltm_ireq_read(unit,
                                                         req_flags,
                                                         sid_inst[inst_idx],
                                                         &pt_dyn_info,
                                                         NULL,
                                                         rsp_entry_wsize,
                                                         req_ltid,
                                                         pt_entry_buff,
                                                         &rsp_ltid,
                                                         &rsp_flags));
            /* Get the counter value for sampled_pkt */
            lt_field = &(flex_sflow_control[unit]->lt_field[0]);
            field_width = lt_field->maxbit - lt_field->minbit + 1;
            bcmptm_cci_buff32_field_copy(tmp_val, 0, pt_entry_buff,
                                         lt_field->minbit, field_width);
            info->sampled_pkt += (((uint64_t)tmp_val[1] << 32) | tmp_val[0]);

            /* Get the counter value for sampling_eligible_pkt */
            lt_field = &(flex_sflow_control[unit]->lt_field[1]);
            field_width = lt_field->maxbit - lt_field->minbit + 1;
            bcmptm_cci_buff32_field_copy(tmp_val, 0, pt_entry_buff,
                                         lt_field->minbit, field_width);
            info->sampling_eligible_pkt +=
                (((uint64_t)tmp_val[1] << 32) | tmp_val[0]);

            /* Get the counter value for eligible_pkt_snapshot */
            lt_field = &(flex_sflow_control[unit]->lt_field[2]);
            field_width = lt_field->maxbit - lt_field->minbit + 1;
            bcmptm_cci_buff32_field_copy(tmp_val, 0, pt_entry_buff,
                                         lt_field->minbit, field_width);
            info->eligible_pkt_snapshot +=
                (((uint64_t)tmp_val[1] << 32) | tmp_val[0]);
        }/* end FOR inst */
    } /* end FOR pipe */

exit:
    if (pt_entry_buff) {
        SHR_FREE(pt_entry_buff);
    }
    SHR_FUNC_EXIT();
}

/* Initialize the PT and LT counter field information */
static int
bcm56990_b0_cth_mirror_ctr_flex_field_init(int unit)
{
    size_t i, field_num, num;
    bcmdrd_sym_field_info_t finfo = {0};
    bcmdrd_fid_t *fid_list = NULL;
    bcmdrd_sid_t sid = SFLOW_ING_FLEX_DATA_SOURCE_INST0m;
    uint16_t lt_shift = 0, lt_max = 0, total_bits = 0;
    ctr_sflow_field_t *lt_field = NULL;
    uint32_t *ctr_field_num, *entry_wsize;

    SHR_FUNC_ENTER(unit);

    ctr_field_num = &flex_sflow_control[unit]->lt_info_num;
    entry_wsize   = &flex_sflow_control[unit]->entry_wsize;

    *ctr_field_num = 0;
    *entry_wsize = 0;

    /* Get number of fields */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_fid_list_get(unit, sid, 0, NULL, &field_num));
    SHR_ALLOC(fid_list,
              field_num * sizeof(bcmdrd_fid_t),
              "bcmcthMirrorSflowCountFieldId");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);

    /* Get the field list and identify counter fields only */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_fid_list_get(unit, sid, field_num, fid_list, &field_num));

    for (num = 0, i = 0; i < field_num; i++) {
        if (SHR_FAILURE(
                bcmdrd_pt_field_info_get(unit, sid, fid_list[i], &finfo))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                 "Error Field Info SID =%d Name = %s\n"),
                                 sid,
                                 bcmdrd_pt_sid_to_name(unit, sid)));
        } else {
            /* Check if this is a counter field */
            if (bcmdrd_pt_field_is_counter(unit, sid, fid_list[i])) {
                lt_field = &(flex_sflow_control[unit]->lt_field[num]);

                /*
                 * lt_shift determines the shift in counter lt
                 * bits due to shift
                 * Reset the shift if pt min bit >= lt_max
                 */
                if (finfo.minbit >= lt_max) {
                    lt_shift = 0;
                }

                lt_field->minbit = lt_shift + finfo.minbit;
                /* support 64 bit fields */
                lt_field->maxbit = lt_field->minbit + 63;
                lt_max = lt_field->maxbit;
                /* Increment the shift */
                lt_shift += 64 - (finfo.maxbit - finfo.minbit + 1);

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "SID =%d Name = %s, \
                             lt-minbit =%u, lt-maxbit = %u\n"),
                             sid,
                             bcmdrd_pt_sid_to_name(unit, sid),
                             lt_field->minbit,
                             lt_field->maxbit));

                (*ctr_field_num)++;
                num++;

                /* Each counter field is 64 bit wide */
                total_bits += 64;
            } else {
                /* Non counter fields will be same width as HW */
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                 "Field Info SID =%d Field Name = %s, \
                                  field bits = %d\n"),
                                 sid, finfo.name,
                                 (finfo.maxbit -finfo.minbit + 1)));
            }
        }
    }

    /* store entry size in words */
    *entry_wsize = (total_bits % 32) ? (total_bits / 32) :
                                       ((total_bits / 32) + 1);

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

static int
bcm56990_b0_cth_mirror_deinit(int unit)
{
    SHR_FREE(flex_sflow_control[unit]);

    return SHR_E_NONE;
}

static int
bcm56990_b0_cth_mirror_init(
    int unit, int warm, const bcmcth_mirror_data_t *data)
{
    SHR_FUNC_ENTER(unit);

    /* Allocate control structure */
    flex_sflow_control[unit] = NULL;
    SHR_ALLOC(flex_sflow_control[unit],
              sizeof(bcmcth_mirror_flex_sflow_control_t),
              "bcmcthMirrorFlexSflowControl");
    SHR_NULL_CHECK(flex_sflow_control[unit], SHR_E_MEMORY);
    sal_memset(flex_sflow_control[unit], 0,
               sizeof(bcmcth_mirror_flex_sflow_control_t));

    /* Initialize field info. */
    SHR_IF_ERR_EXIT
        (bcm56990_b0_cth_mirror_ctr_flex_field_init(unit));


exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(flex_sflow_control[unit]);
    }

    SHR_FUNC_EXIT();
}

static bcmcth_mirror_drv_t bcm56990_b0_cth_mirror_drv = {
    .init = bcm56990_b0_cth_mirror_init,
    .deinit = bcm56990_b0_cth_mirror_deinit,
    .ctr_flex_sflow_write = &bcm56990_b0_cth_mirror_ctr_flex_sflow_write,
    .ctr_flex_sflow_read = &bcm56990_b0_cth_mirror_ctr_flex_sflow_read,
};

/*******************************************************************************
 * Public functions
 */
bcmcth_mirror_drv_t *
bcm56990_b0_cth_mirror_drv_get(int unit)
{
    return &bcm56990_b0_cth_mirror_drv;
}
