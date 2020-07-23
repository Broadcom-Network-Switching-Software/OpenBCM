/*! \file bcmptm_bcm56990_b0_rm_tcam.c
 *
 * Miscellaneous utilities for PTRM
 * This file contains the misc utils for PTRM
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include "./bcmptm_bcm56990_b0_rm_tcam.h"
#include <bcmptm/generated/bcmptm_rm_tcam_prio_only_ha.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC

/*******************************************************************************
 * Private variables
 */
/* List of PTs which are directly configured by PTRM */
static const bcmdrd_sid_t bcm56990_b0_list[BCMPTM_RM_TCAM_SID_COUNT] = {
    IFP_LOGICAL_TABLE_ACTION_PRIORITYm,
    IFP_LOGICAL_TABLE_CONFIGr,
    IFP_CONFIGr,
    EFP_SLICE_MAPr,
    EFP_SLICE_CONTROLr,
    EFP_CLASSID_SELECTORr,
    EFP_CLASSID_SELECTOR_2r,
    EFP_KEY4_L3_CLASSID_SELECTORr,
    EFP_KEY8_L3_CLASSID_SELECTORr,
    EFP_KEY4_MDL_SELECTORr,
    VFP_SLICE_MAPr,
    VFP_SLICE_CONTROLr,
    VFP_KEY_CONTROL_1r,
    VFP_KEY_CONTROL_2r
};

/*******************************************************************************
 * Internal Functions
 */
static int
bcm56990_b0_rm_tcam_memreg_read(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_sid_type_t sid_type,
                              bcmptm_rm_tcam_sid_info_t *sid_info)
{
    bcmdrd_sid_t sid;
    uint32_t entry_buffer[BCMPTM_MAX_PT_FIELD_WORDS] = { 0 };
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t rsp_flags = 0;
    bool rsp_entry_cache_vbit = 0;
    uint16_t rsp_dfield_format_id = 0;
    bcmltd_sid_t rsp_ltid;
    uint8_t start_bit, width, vg_offset;
    uint32_t field_val;
    int field_idx = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(sid_info, SHR_E_PARAM);

    sid = bcm56990_b0_list[sid_type];

    if (sid == 0) {
        SHR_EXIT();
    }

    dyn_info.tbl_inst = sid_info->tbl_inst;

    if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SLICE_MAP_UPDATE) {
        /* Execute the Instruction. */
        dyn_info.index = 0;

        /* Read the contents of the sid from h/w. */
        SHR_IF_ERR_EXIT(bcmptm_cmdproc_read(unit,
                                            0,
                                            0, 
                                            sid,
                                            &dyn_info,
                                            NULL, /* misc_info */
                                            BCMPTM_MAX_PT_FIELD_WORDS,
                                            entry_buffer,
                                            &rsp_entry_cache_vbit,
                                            &rsp_ltid,
                                            &rsp_dfield_format_id,
                                            &rsp_flags));

        start_bit = 0;
        width = 4;
        vg_offset = 2;

        for (field_idx = 0; field_idx < sid_info->num_slices; field_idx++) {
            /*Get the physical slice No. in entry buffer*/
            bcmdrd_field_get(entry_buffer,
                             start_bit,
                             start_bit + vg_offset - 1,
                             &field_val);
            if (sid_info[unit].slice_id == field_idx) {
                *sid_info[unit].lt_action_pri = field_val;
                break;
            }
            start_bit = start_bit + width;
        }

    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcm56990_b0_rm_tcam_memreg_oper(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_sid_type_t sid_type,
                              bcmptm_rm_tcam_sid_oper_t sid_oper,
                              bcmptm_rm_tcam_sid_info_t *sid_info)
{
    bcmdrd_sid_t sid;
    uint32_t entry_buffer[BCMPTM_MAX_PT_FIELD_WORDS] = { 0 };
    bcmbd_pt_dyn_info_t dyn_info;
    uint32_t rsp_flags = 0;
    bool rsp_entry_cache_vbit= 0;
    uint16_t rsp_dfield_format_id= 0;
    bcmltd_sid_t rsp_ltid;
    uint8_t field_idx, start_bit, width, vg_offset;
    uint32_t field_val;
    bcmptm_rm_tcam_fp_group_selcode_t   *selcode = NULL;
    uint8_t slice_idx, pri_slice_idx = 0;
    uint32_t data = 0;
    uint32_t data_arr[2] = {0,1};
    int  slice_en = 1;
    int slice_lookup_en = 2;
    int part = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(sid_info, SHR_E_PARAM);

    if (sid_oper == BCMFP_RM_TCAM_SID_OPER_READ) {
        SHR_ERR_EXIT(bcm56990_b0_rm_tcam_memreg_read(unit,
                                                        ltid,
                                                        sid_type,
                                                        sid_info));
    }
    sid = bcm56990_b0_list[sid_type];

    if (sid == 0) {
        SHR_EXIT();
    }

    dyn_info.tbl_inst = sid_info->tbl_inst;

    if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_ACTION_PRIO_UPDATE) {
        start_bit = 0;
        width = 5;
        /* Modify the contents of the sid. */
        for (field_idx = 0;
             field_idx < sid_info->lt_action_pri_count;
             field_idx++) {
             /* update the action priority in entry buffer */
             field_val = sid_info->lt_action_pri[field_idx];
             bcmdrd_field_set(entry_buffer,
                             start_bit,
                             start_bit + width - 1,
                             &field_val);
             start_bit = start_bit + width;
        }
        /* Execute the Instruction. */
        dyn_info.index = 0;
    } else if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_PARTITION_PRIO_UPDATE) {

        /* Set lt partition map */
        bcmdrd_field_set(entry_buffer,
                    0,
                    8,
                    (uint32_t *)&(sid_info->lt_part_map));

        start_bit = 9;
        width = 4;
        /* Modify the contents of the sid. */
        for (field_idx = 0;
             field_idx < sid_info->num_slices;
             field_idx++) {
             /* update the partition priority in entry buffer */
             field_val = sid_info->lt_part_pri[field_idx];
             bcmdrd_field_set(entry_buffer,
                    start_bit,
                    start_bit + width - 1,
                    &field_val);
             start_bit = start_bit + width;
        }
        /* Execute the Instruction. */
        dyn_info.index = sid_info->group_ltid[0];

    } else if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SELCODE_INSTALL) {

        /* Execute the Instruction. */
        dyn_info.index = 0;

        /* Read the contents of the sid from h/w. */
        SHR_IF_ERR_EXIT(bcmptm_cmdproc_read(unit,
                                            0,
                                            0, 
                                            sid,
                                            &dyn_info,
                                            NULL, /* misc_info */
                                            BCMPTM_MAX_PT_FIELD_WORDS,
                                            entry_buffer,
                                            &rsp_entry_cache_vbit,
                                            &rsp_ltid,
                                            &rsp_dfield_format_id,
                                            &rsp_flags));

        for (part = 0; part < sid_info->parts_count; part++) {
            slice_idx = sid_info->slice_id + (part >> sid_info->intraslice);
            selcode
              = (bcmptm_rm_tcam_fp_group_selcode_t *)
                ((uint8_t *)sid_info->selcode
                  + (sizeof(bcmptm_rm_tcam_fp_group_selcode_t) * part));

            if (sid_type == BCMPTM_RM_TCAM_EFP_SLICE_CONTROL) {
                /* IPV6 mode */
                if (selcode->egr_ip_addr_sel >= 0) {
                    field_val = selcode->egr_ip_addr_sel;
                    bcmdrd_field_set(entry_buffer,
                                     (5 * slice_idx) + 4, (5 * slice_idx) + 5,
                                     &field_val);
                }
                /* SLICE MODE */
                if (selcode->fpf2 >= 0) {
                    field_val = selcode->fpf2;
                    bcmdrd_field_set(entry_buffer,
                                     (5 * slice_idx) + 6, (5 * slice_idx) + 8,
                                     &field_val);
                }
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR) {
                /* KEY1 class id Sel */
                if (selcode->egr_key1_classid_sel >= 0) {
                    field_val = selcode->egr_key1_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                                     11 * slice_idx, (11 * slice_idx) + 2,
                                     &field_val);
                }
                /* KEY2 class id Sel */
                if (selcode->egr_key2_classid_sel >= 0) {
                    field_val = selcode->egr_key2_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                                     (11 * slice_idx) + 3, (11 * slice_idx) + 5,
                                     &field_val);
                }
                /* KEY3 class id Sel */
                if (selcode->egr_key3_classid_sel >= 0) {
                    field_val = selcode->egr_key3_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                                     (11 * slice_idx) + 6, (11 * slice_idx) + 8,
                                     &field_val);
                }
                /* KEY4 class id Sel */
                if (selcode->egr_key4_classid_sel >= 0) {
                    field_val = selcode->egr_key4_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                                     (11 * slice_idx) + 9, (11 * slice_idx) + 10,
                                     &field_val);
                }
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR_2) {
                /* KEY6 class id Sel */
                if (selcode->egr_key6_classid_sel >= 0) {
                    field_val = selcode->egr_key6_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                                     7 * slice_idx, (7 * slice_idx) + 1,
                                     &field_val);
                }
                /* KEY7 class id Sel */
                if (selcode->egr_key7_classid_sel >= 0) {
                    field_val = selcode->egr_key7_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                                     (7 * slice_idx) + 2, (7 * slice_idx) + 4,
                                     &field_val);
                }
                /* KEY8 class id Sel */
                if (selcode->egr_key8_classid_sel >= 0) {
                    field_val = selcode->egr_key8_classid_sel;
                    bcmdrd_field_set(entry_buffer,
                                     (7 * slice_idx) + 5, (7 * slice_idx) + 6,
                                     &field_val);
                }
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_KEY4_DVP_SELECTOR) {
                /* KEY4 DVP Sel */
                if (selcode->egr_key4_dvp_sel >= 0) {
                    field_val = selcode->egr_key4_dvp_sel;
                    bcmdrd_field_set(entry_buffer,
                                     (3 * slice_idx), (3 * slice_idx) + 2,
                                     &field_val);
                }
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_KEY4_MDL_SELECTOR) {
                /* KEY4 MDL Sel */
                if (selcode->egr_key4_mdl_sel >= 0) {
                    field_val = selcode->egr_key4_mdl_sel;
                    bcmdrd_field_set(entry_buffer,
                                     slice_idx, slice_idx,
                                     &field_val);
                }
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_KEY8_DVP_SELECTOR) {
                /* KEY8 DVP Sel */
                if (selcode->egr_key8_dvp_sel >= 0) {
                    field_val = selcode->egr_key8_dvp_sel;
                    bcmdrd_field_set(entry_buffer,
                                     (3 * slice_idx), (3 * slice_idx) + 2,
                                     &field_val);
                }
            } else if (sid_type == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_1) {
                /* Paring mode */
                if (sid_info->pairing && (part % 2 == 0)) {
                    bcmdrd_field_set(entry_buffer,
                                     (slice_idx / 2), (slice_idx / 2),
                                     &data_arr[1]);
                }
                /* Double wide mode */
                if (sid_info->intraslice && (part % 2 == 0)) {
                    bcmdrd_field_set(entry_buffer,
                                     (13 * slice_idx) + 4, (13 * slice_idx) + 4,
                                     &data_arr[1]);
                }
                /* F2  or DWF2*/
                if (selcode->fpf2 >= 0) {
                    field_val = selcode->fpf2;
                    if (sid_info->intraslice && (part % 2)) {
                        bcmdrd_field_set(entry_buffer,
                                         (13 * slice_idx) + 2, (13 * slice_idx) + 3,
                                         &field_val);
                    } else {
                        bcmdrd_field_set(entry_buffer,
                                         (13 * slice_idx) + 5, (13 * slice_idx) + 8,
                                         &field_val);
                    }
                }
                /* F3 */
                if (selcode->fpf3 >= 0) {
                    if (!(sid_info->intraslice && (part % 2))) {
                        field_val = selcode->fpf3;
                        bcmdrd_field_set(entry_buffer,
                                (13 * slice_idx) + 9, (13 * slice_idx) + 11,
                                &field_val);
                    }
                }
                /* Src Sel mode */
                if (selcode->src_sel >= 0) {
                    field_val = selcode->src_sel;
                    bcmdrd_field_set(entry_buffer,
                                     (13 * slice_idx) + 12, (13 * slice_idx) + 14,
                                     &field_val);
                }
            } else if (sid_type == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_2) {
                /* IP header Sel mode */
                if (selcode->ip_header_sel >= 0) {
                    field_val = selcode->ip_header_sel;
                    bcmdrd_field_set(entry_buffer,
                                     slice_idx, slice_idx,
                                     &field_val);
                }
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    } else if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SELCODE_RESET) {

        /* Execute the Instruction. */
        dyn_info.index = 0;

        /* Read the contents of the sid from h/w. */
        SHR_IF_ERR_EXIT(bcmptm_cmdproc_read(unit,
                                            0,
                                            0, 
                                            sid,
                                            &dyn_info,
                                            NULL, /* misc_info */
                                            BCMPTM_MAX_PT_FIELD_WORDS,
                                            entry_buffer,
                                            &rsp_entry_cache_vbit,
                                            &rsp_ltid,
                                            &rsp_dfield_format_id,
                                            &rsp_flags));

        for (part = 0; part < sid_info->parts_count; part++) {
            slice_idx = sid_info->slice_id + (part >> sid_info->intraslice);
            if (sid_type == BCMPTM_RM_TCAM_EFP_SLICE_CONTROL) {
                /* IPV6 mode */
                bcmdrd_field_set(entry_buffer,
                                 (5 * slice_idx) + 4, (5 * slice_idx) + 5,
                                 &data);
                /* SLICE MODE */
                bcmdrd_field_set(entry_buffer,
                                 (5 * slice_idx) + 6, (5 * slice_idx) + 8,
                                 &data);
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR) {
                /* KEY1 class id Sel */
                bcmdrd_field_set(entry_buffer,
                        11 * slice_idx, (11 * slice_idx) + 2,
                        &data);
                /* KEY2 class id Sel */
                bcmdrd_field_set(entry_buffer,
                        (11 * slice_idx) + 3, (11 * slice_idx) + 5,
                        &data);
                /* KEY3 class id Sel */
                bcmdrd_field_set(entry_buffer,
                        (11 * slice_idx) + 6, (11 * slice_idx) + 8,
                        &data);
                /* KEY4 class id Sel */
                bcmdrd_field_set(entry_buffer,
                        (11 * slice_idx) + 9, (11 * slice_idx) + 10,
                        &data);
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR_2) {
                /* KEY6 class id Sel */
                bcmdrd_field_set(entry_buffer,
                        7 * slice_idx, (7 * slice_idx) + 1,
                        &data);
                /* KEY7 class id Sel */
                bcmdrd_field_set(entry_buffer,
                        (7 * slice_idx) + 2, (7 * slice_idx) + 4,
                        &data);
                /* KEY8 class id Sel */
                bcmdrd_field_set(entry_buffer,
                        (7 * slice_idx) + 5, (7 * slice_idx) + 6,
                        &data);
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_KEY4_DVP_SELECTOR) {
                /* KEY4 DVP Sel */
                bcmdrd_field_set(entry_buffer, (3 * slice_idx),
                                 (3 * slice_idx) + 2, &data);
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_KEY4_MDL_SELECTOR) {
                /* KEY4 MDL Sel */
                bcmdrd_field_set(entry_buffer, slice_idx, slice_idx,
                                 &data);
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_KEY8_DVP_SELECTOR) {
                /* KEY8 DVP Sel */
                bcmdrd_field_set(entry_buffer, (3 * slice_idx),
                                 (3 * slice_idx) + 2, &data);
            } else if (sid_type == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_1) {
                /* Paring mode */
                if (part % 2 == 0) {
                    bcmdrd_field_set(entry_buffer,
                                     (slice_idx / 2), (slice_idx / 2),
                                     &data);
                }
                /* DWF2*/
                bcmdrd_field_set(entry_buffer,
                                 (13 * slice_idx) + 2, (13 * slice_idx) + 3,
                                 &data);
                /* Double wide mode */
                bcmdrd_field_set(entry_buffer,
                                 (13 * slice_idx) + 4, (13 * slice_idx) + 4,
                                 &data);
                /* F2 */
                bcmdrd_field_set(entry_buffer,
                                 (13 * slice_idx) + 5, (13 * slice_idx) + 8,
                                 &data);
                /* F3 */
                bcmdrd_field_set(entry_buffer,
                                 (13 * slice_idx) + 9, (13 * slice_idx) + 11,
                                 &data);
                /* Src Sel mode */
                bcmdrd_field_set(entry_buffer,
                                 (13 * slice_idx) + 12, (13 * slice_idx) + 14,
                                 &data);
            } else if (sid_type == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_2) {

                /* IP header Sel mode */
                bcmdrd_field_set(entry_buffer, slice_idx, slice_idx, &data);
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    } else if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SELCODE_COPY) {

        /* Execute the Instruction. */
        dyn_info.index = 0;

        /* Read the contents of the sid from h/w. */
        SHR_IF_ERR_EXIT(bcmptm_cmdproc_read(unit,
                                            0,
                                            0, 
                                            sid,
                                            &dyn_info,
                                            NULL, /* misc_info */
                                            BCMPTM_MAX_PT_FIELD_WORDS,
                                            entry_buffer,
                                            &rsp_entry_cache_vbit,
                                            &rsp_ltid,
                                            &rsp_dfield_format_id,
                                            &rsp_flags));

        for (part = 0; part < sid_info->parts_count; part++) {
            slice_idx = sid_info->slice_id + (part >> sid_info->intraslice);
            pri_slice_idx = sid_info-> pri_slice_id
                            + (part >> sid_info->intraslice);
            if (sid_type == BCMPTM_RM_TCAM_EFP_SLICE_CONTROL) {
                /* IPV6 mode */
                bcmdrd_field_get(entry_buffer, (5 * pri_slice_idx) + 4,
                                 (5 * pri_slice_idx) + 5, &data);
                bcmdrd_field_set(entry_buffer, (5 * slice_idx) + 4,
                                 (5 * slice_idx) + 5, &data);
                /* SLICE MODE */
                bcmdrd_field_get(entry_buffer, (5 * pri_slice_idx) + 6,
                                 (5 * pri_slice_idx) + 8, &data);
                bcmdrd_field_set(entry_buffer, (5 * slice_idx) + 6,
                                 (5 * slice_idx) + 8, &data);
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR) {
                /* KEY1 class id Sel */
                bcmdrd_field_get(entry_buffer,
                        11 * pri_slice_idx, (11 * pri_slice_idx) + 2,
                        &data);
                bcmdrd_field_set(entry_buffer,
                        11 * slice_idx, (11 * slice_idx) + 2,
                        &data);
                /* KEY2 class id Sel */
                bcmdrd_field_get(entry_buffer,
                        (11 * pri_slice_idx) + 3, (11 * pri_slice_idx) + 5,
                        &data);
                bcmdrd_field_set(entry_buffer,
                        (11 * slice_idx) + 3, (11 * slice_idx) + 5,
                        &data);
                /* KEY3 class id Sel */
                bcmdrd_field_get(entry_buffer,
                        (11 * pri_slice_idx) + 6, (11 * pri_slice_idx) + 8,
                        &data);
                bcmdrd_field_set(entry_buffer,
                        (11 * slice_idx) + 6, (11 * slice_idx) + 8,
                        &data);
                /* KEY4 class id Sel */
                bcmdrd_field_get(entry_buffer,
                        (11 * pri_slice_idx) + 9, (11 * pri_slice_idx) + 10,
                        &data);
                bcmdrd_field_set(entry_buffer,
                        (11 * slice_idx) + 9, (11 * slice_idx) + 10,
                        &data);
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR_2) {
                /* KEY6 class id Sel */
                bcmdrd_field_get(entry_buffer,
                        7 * pri_slice_idx, (7 * pri_slice_idx) + 1,
                        &data);
                bcmdrd_field_set(entry_buffer,
                        7 * slice_idx, (7 * slice_idx) + 1,
                        &data);
                /* KEY7 class id Sel */
                bcmdrd_field_get(entry_buffer,
                        (7 * pri_slice_idx) + 2, (7 * pri_slice_idx) + 4,
                        &data);
                bcmdrd_field_set(entry_buffer,
                        (7 * slice_idx) + 2, (7 * slice_idx) + 4,
                        &data);
                /* KEY8 class id Sel */
                bcmdrd_field_get(entry_buffer,
                        (7 * pri_slice_idx) + 5, (7 * pri_slice_idx) + 6,
                        &data);
                bcmdrd_field_set(entry_buffer,
                        (7 * slice_idx) + 5, (7 * slice_idx) + 6,
                        &data);
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_KEY4_DVP_SELECTOR) {
                /* KEY4 DVP Sel */
                bcmdrd_field_get(entry_buffer, (3 * pri_slice_idx),
                                 (3 * pri_slice_idx) + 2, &data);
                bcmdrd_field_set(entry_buffer, (3 * slice_idx),
                                 (3 * slice_idx) + 2, &data);
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_KEY4_MDL_SELECTOR) {
                /* KEY4 MDL Sel */
                bcmdrd_field_get(entry_buffer, pri_slice_idx,
                                 slice_idx, &data);
                bcmdrd_field_set(entry_buffer, slice_idx, slice_idx,
                                 &data);
            } else if (sid_type == BCMPTM_RM_TCAM_EFP_KEY8_DVP_SELECTOR) {
                /* KEY8 DVP Sel */
                bcmdrd_field_get(entry_buffer, (3 * pri_slice_idx),
                                 (3 * pri_slice_idx) + 2, &data);
                bcmdrd_field_set(entry_buffer, (3 * slice_idx),
                                 (3 * slice_idx) + 2, &data);
            } else if (sid_type == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_1) {
                /* Paring mode */
                if (part % 2 == 0) {
                    bcmdrd_field_get(entry_buffer, (pri_slice_idx / 2),
                                     (pri_slice_idx / 2), &data);
                    bcmdrd_field_set(entry_buffer, (slice_idx / 2),
                                     (slice_idx / 2), &data);
                }
                /* DWF2*/
                bcmdrd_field_get(entry_buffer, (13 * pri_slice_idx) + 2,
                                 (13 * pri_slice_idx) + 3, &data);
                bcmdrd_field_set(entry_buffer, (13 * slice_idx) + 2,
                                 (13 * slice_idx) + 3, &data);
                /* Double wide mode */
                bcmdrd_field_get(entry_buffer, (13 * pri_slice_idx) + 4,
                                 (13 * pri_slice_idx) + 4, &data);
                bcmdrd_field_set(entry_buffer, (13 * slice_idx) + 4,
                                 (13 * slice_idx) + 4, &data);
                /* F2 */
                bcmdrd_field_get(entry_buffer, (13 * pri_slice_idx) + 5,
                                 (13 * pri_slice_idx) + 8, &data);
                bcmdrd_field_set(entry_buffer, (13 * slice_idx) + 5,
                                 (13 * slice_idx) + 8, &data);
                /* F3 */
                bcmdrd_field_get(entry_buffer, (13 * pri_slice_idx) + 9,
                                 (13 * pri_slice_idx) + 11, &data);
                bcmdrd_field_set(entry_buffer, (13 * slice_idx) + 9,
                                 (13 * slice_idx) + 11, &data);
                /* Src Sel mode */
                bcmdrd_field_get(entry_buffer, (13 * pri_slice_idx) + 12,
                                 (13 * pri_slice_idx) + 14, &data);
                bcmdrd_field_set(entry_buffer, (13 * slice_idx) + 12,
                                 (13 * slice_idx) + 14, &data);
            } else if (sid_type == BCMPTM_RM_TCAM_VFP_KEY_CONTROL_2) {
                /* IP header Sel mode */
                bcmdrd_field_get(entry_buffer, pri_slice_idx,
                                 pri_slice_idx, &data);
                bcmdrd_field_set(entry_buffer, slice_idx, slice_idx, &data);
            } else {
                SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    } else if ((sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SLICE_DISABLE) ||
               (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SLICE_ENABLE)) {

        /* Execute the Instruction. */
        if (sid_info->stage_flags & BCMPTM_STAGE_KEY_TYPE_PRESEL) {
            dyn_info.index = sid_info->slice_id;
        } else {
            dyn_info.index = 0;
        }

        if (sid_info->stage_flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
            slice_lookup_en = sid_info->slice_id;
            if (sid_info->stage_flags & BCMPTM_STAGE_KEY_TYPE_FIXED) {
                slice_en = sid_info->slice_id + 24;
            } else {
                slice_en = sid_info->slice_id + 4;
            }
            /* Read the contents of the sid from h/w. */
            SHR_IF_ERR_EXIT(bcmptm_cmdproc_read(unit,
                                                0,
                                                0, 
                                                sid,
                                                &dyn_info,
                                                NULL, /* misc_info */
                                                BCMPTM_MAX_PT_FIELD_WORDS,
                                                entry_buffer,
                                                &rsp_entry_cache_vbit,
                                                &rsp_ltid,
                                                &rsp_dfield_format_id,
                                                &rsp_flags));
        }

        if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SLICE_ENABLE) {
            /* SLICE_ENABLE */
            bcmdrd_field_set(entry_buffer, slice_en, slice_en, &data_arr[1]);

            /* SLICE_LOOKUP_ENABLE */
            bcmdrd_field_set(entry_buffer, slice_lookup_en, slice_lookup_en,
                             &data_arr[1]);
        } else {
            /* RESET SLICE_ENABLE */
            bcmdrd_field_set(entry_buffer, slice_en, slice_en, &data_arr[0]);

            /* RESET SLICE_LOOKUP_ENABLE */
            bcmdrd_field_set(entry_buffer, slice_lookup_en, slice_lookup_en,
                             &data_arr[0]);
        }
    } else if (sid_info->oper_flags & BCMPTM_RM_TCAM_OPER_SLICE_MAP_UPDATE) {
             start_bit = 0;
             width = 4;
             vg_offset = 2;
             /* Modify the contents of the sid. */
             for (field_idx = 0; field_idx < sid_info->num_slices; field_idx++) {
                 /*Update the physical slice No. in entry buffer*/
                 field_val = sid_info->lt_action_pri[field_idx];
                 bcmdrd_field_set(entry_buffer,
                                  start_bit,
                                  start_bit + vg_offset - 1,
                                  &field_val);

                 /*Update the Virtual Group No. in entry buffer*/
                 field_val = sid_info->group_ltid[field_idx];
                 bcmdrd_field_set(entry_buffer,
                                  start_bit + vg_offset,
                                  start_bit + width - 1,
                                  &field_val);

                 start_bit = start_bit + width;
             }
             /* Execute the Instruction. */
             dyn_info.index = 0;

    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* write the contents of the sid back into h/w */
    SHR_IF_ERR_EXIT(bcmptm_cmdproc_write(unit,
                                         0,
                                         0, 
                                         sid,
                                         &dyn_info,
                                         NULL, /* misc_info */
                                         entry_buffer,
                                         1,
                                         1,
                                         1,
                                         BCMPTM_RM_OP_NORMAL, NULL,
                                         ltid,
                                         0,
                                         &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_rm_tcam_prio_only_hw_key_info_init(int unit,
                            bcmltd_sid_t ltid,
                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                            uint8_t num_entries_per_index,
                            void *hw_field_list1)
{
    bcmptm_rm_tcam_hw_field_list_t *hw_field_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    SHR_NULL_CHECK(hw_field_list1, SHR_E_PARAM);

    hw_field_list = (bcmptm_rm_tcam_hw_field_list_t *) hw_field_list1;

    hw_field_list[0].num_fields = 4;
    hw_field_list[0].field_width[0] = 1;
    hw_field_list[0].field_start_bit[0] = 0;
    hw_field_list[0].field_width[1] = 76;
    hw_field_list[0].field_start_bit[1] = 2;
    hw_field_list[0].field_width[2] = 76;
    hw_field_list[0].field_start_bit[2] = 154;
    hw_field_list[0].field_width[3] = 68;
    hw_field_list[0].field_start_bit[3] = 324;

    hw_field_list[1].num_fields = 4;
    hw_field_list[1].field_width[0] = 1;
    hw_field_list[1].field_start_bit[0] = 1;
    hw_field_list[1].field_width[1] = 76;
    hw_field_list[1].field_start_bit[1] = 78;
    hw_field_list[1].field_width[2] = 76;
    hw_field_list[1].field_start_bit[2] = 230;
    hw_field_list[1].field_width[3] = 68;
    hw_field_list[1].field_start_bit[3] = 392;

exit:
    SHR_FUNC_EXIT();
}
