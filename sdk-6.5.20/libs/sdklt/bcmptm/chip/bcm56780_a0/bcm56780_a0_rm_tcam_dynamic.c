/*! \file bcm56780_a0_tcam_fp_dynamic.c
 *
 * Chip specific functions.
 *
 * This file contains the chip specific
 * functions for initializing mreq_info for dynamic LTs.
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include "../../rm_tcam/rm_tcam_prio_only.h"
#include "../../rm_tcam/rm_tcam_fp_utils.h"
#include "bcm56780_a0_rm_tcam_dynamic.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Local variables
 */

/*******************************************************************************
 * Private Functions
 */

static int
bcmptm_check_ifta80_bank(int unit,
                         bcmdrd_sid_t sid,
                         uint8_t *is_ifta80_bank)
{
    *is_ifta80_bank = 0;

    switch (sid) {
        case MEMDB_TCAM_IFTA80_MEM0_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM1_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM0_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM1_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM2_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM3_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM2_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM3_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM4_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM5_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM4_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM5_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM6_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM7_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM6_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM7_1_ONLYm:
            *is_ifta80_bank = 1;
            break;
    }
    return SHR_E_NONE;
}

static int
bcm56780_a0_rm_tcam_get_ifta80_bank_usage(int unit,
                                          bcmltd_sid_t ltid,
                                          uint8_t *t2t_0_banks,
                                          uint8_t *t2t_1_banks)
{
    bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    uint8_t is_ifta80_bank = 0;
    uint8_t i = 0, j = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit, ltid, 0, 0,
                                      ((void *)&ltid_info),
                                      NULL, NULL));

    hw_entry_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                   (ltid_info.hw_entry_info);

    if (hw_entry_info != NULL &&
        hw_entry_info->num_depth_inst != 0 &&
        hw_entry_info->sid[0] != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_check_ifta80_bank(unit,
                                  hw_entry_info->sid[0][0],
                                  &is_ifta80_bank));
    }

    if (is_ifta80_bank == 0) {
        SHR_EXIT();
    }

    for (j = 0; j < hw_entry_info->num_depth_inst; j++) {
        for (i = 0; i < hw_entry_info->num_key_rows; i++) {
            switch (hw_entry_info->sid[j][i]) {
                case MEMDB_TCAM_IFTA80_MEM0_0_ONLYm:
                    t2t_0_banks[0] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM0_1_ONLYm:
                    t2t_0_banks[1] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM1_0_ONLYm:
                    t2t_0_banks[2] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM1_1_ONLYm:
                    t2t_0_banks[3] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM2_0_ONLYm:
                    t2t_0_banks[4] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM2_1_ONLYm:
                    t2t_0_banks[5] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM3_0_ONLYm:
                    t2t_0_banks[6] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM3_1_ONLYm:
                    t2t_0_banks[7] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM4_0_ONLYm:
                    t2t_1_banks[0] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM4_1_ONLYm:
                    t2t_1_banks[1] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM5_0_ONLYm:
                    t2t_1_banks[2] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM5_1_ONLYm:
                    t2t_1_banks[3] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM6_0_ONLYm:
                    t2t_1_banks[4] = 1;
                    break;
                case MEMDB_TCAM_IFTA80_MEM6_1_ONLYm:
                    t2t_1_banks[5] = 1;
                    break;
               case MEMDB_TCAM_IFTA80_MEM7_0_ONLYm:
                    t2t_1_banks[6] = 1;
                    break;
               case MEMDB_TCAM_IFTA80_MEM7_1_ONLYm:
                    t2t_1_banks[7] = 1;
                    break;
               default:
                    /*
                     * There can be INVALIDm sid's because of
                     * misconfiguration of URPF/wide mode banks.
                     * Ignore non ifta80 banks
                     */
                    break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_rm_tcam_defip_acc_modes_write(int unit,
                                        uint32_t trans_id,
                                        bcmltd_sid_t ltid,
                                        uint8_t *t2t_0_banks,
                                        uint8_t *t2t_1_banks)
{
    MEMDB_TCAM_IFTA80_ACC_MODESr_t ifta80_acc_mode;
    bcmbd_pt_dyn_info_t dyn_info;
    int tbl_inst = -1;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid = 0;
    bcmdrd_sid_t sid = MEMDB_TCAM_IFTA80_ACC_MODESr;
    uint32_t e_buffer[BCMPTM_MAX_PT_FIELD_WORDS] = { 0 };

    SHR_FUNC_ENTER(unit);


    sal_memset(&ifta80_acc_mode, 0, sizeof(ifta80_acc_mode));

    /* Get the access modes current values from HW */
    dyn_info.tbl_inst = tbl_inst;
    dyn_info.index = 0;
    sal_memset(e_buffer, 0, sizeof(e_buffer));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_mreq_indexed_lt(unit,
                                0,
                                sid,
                                &dyn_info,
                                NULL,
                                NULL,
                                BCMPTM_OP_READ,
                                NULL,
                                1,
                                ltid,
                                trans_id,
                                NULL,
                                NULL,
                                e_buffer,
                                &rsp_ltid,
                                &rsp_flags));
    sal_memcpy(&ifta80_acc_mode, e_buffer, sizeof(ifta80_acc_mode));

    /* Reset RM TCAM ACC Modes 8f,9f,10f,11f,12f,13f,14f,15f to 0 */
    MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE8f_SET(ifta80_acc_mode, 0);
    MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE9f_SET(ifta80_acc_mode, 0);
    MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE10f_SET(ifta80_acc_mode, 0);
    MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE11f_SET(ifta80_acc_mode, 0);
    MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE12f_SET(ifta80_acc_mode, 0);
    MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE13f_SET(ifta80_acc_mode, 0);
    MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE14f_SET(ifta80_acc_mode, 0);
    MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE15f_SET(ifta80_acc_mode, 0);

    /* mem0_0|mem1_0 mode8f */
    if (t2t_0_banks[0] == 1 || t2t_0_banks[2] == 1) {
        MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE8f_SET(ifta80_acc_mode, 1);
    }

    /* mem0_1|mem1_1 mode9f */
    if (t2t_0_banks[1] == 1 || t2t_0_banks[3] == 1) {
        MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE9f_SET(ifta80_acc_mode, 1);
    }

    /* mem2_0|mem3_0 mode10f */
    if (t2t_0_banks[4] == 1 || t2t_0_banks[6] == 1) {
        MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE10f_SET(ifta80_acc_mode, 1);
    }

    /* mem2_1|mem3_1 mode11f */
    if (t2t_0_banks[5] == 1 || t2t_0_banks[7] == 1) {
        MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE11f_SET(ifta80_acc_mode, 1);
    }

    /* mem4_0|mem5_0 mode12f */
    if (t2t_1_banks[0] == 1 || t2t_1_banks[2] == 1) {
        MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE12f_SET(ifta80_acc_mode, 1);
    }

    /* mem4_1|mem5_1 mode13f */
    if (t2t_1_banks[1] == 1 || t2t_1_banks[3] == 1) {
        MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE13f_SET(ifta80_acc_mode, 1);
    }

    /* mem6_0|mem7_0 mode14f */
    if (t2t_1_banks[4] == 1 || t2t_1_banks[6] == 1) {
        MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE14f_SET(ifta80_acc_mode, 1);
    }

    /* mem6_1|mem7_1 mode15f */
    if (t2t_1_banks[5] == 1 || t2t_1_banks[7] == 1) {
        MEMDB_TCAM_IFTA80_ACC_MODESr_ACC_MODE15f_SET(ifta80_acc_mode, 1);
    }

    sal_memset(e_buffer, 0, sizeof(e_buffer));
    sal_memcpy(e_buffer, &ifta80_acc_mode, sizeof(ifta80_acc_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_mreq_indexed_lt(unit,
                                0,
                                sid,
                                &dyn_info,
                                NULL,
                                NULL,
                                BCMPTM_OP_WRITE,
                                e_buffer,
                                1,
                                ltid,
                                trans_id,
                                NULL,
                                NULL,
                                NULL,
                                &rsp_ltid,
                                &rsp_flags));

    if (trans_id == BCMPTM_DIRECT_TRANS_ID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_mreq_indexed_lt(unit,
                                BCMPTM_REQ_FLAGS_COMMIT,
                                INVALIDm,
                                NULL,
                                NULL,
                                NULL,
                                BCMPTM_OP_NOP,
                                NULL,
                                0,
                                0,
                                BCMPTM_DIRECT_TRANS_ID,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                &rsp_flags));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_rm_tcam_defip_acc_modes_update(int unit,
                                        uint32_t trans_id,
                                        bcmltd_sid_t ltid)
{
    const lt_mreq_info_t *lrd_info = NULL;
    bcmptm_rm_tcam_dyn_ptrm_table_t *dyn_tbl = NULL;
    uint8_t ifta80_t2t_00_banks_bitmap[8] = {0};
    uint8_t ifta80_t2t_01_banks_bitmap[8] = {0};
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_dyn_ptrm_tbl_global_get(unit, &dyn_tbl));

    while (dyn_tbl != NULL) {
        lrd_info = dyn_tbl->handler;
        if (lrd_info == NULL) {
            dyn_tbl = dyn_tbl->next;
            continue;
        }

        if (lrd_info->pt_type != LT_PT_TYPE_FP) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_rm_tcam_get_ifta80_bank_usage(unit,
                                               dyn_tbl->sid,
                                               ifta80_t2t_00_banks_bitmap,
                                               ifta80_t2t_01_banks_bitmap));
        }
        dyn_tbl = dyn_tbl->next;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_rm_tcam_defip_acc_modes_write(unit,
                                                trans_id,
                                                ltid,
                                                ifta80_t2t_00_banks_bitmap,
                                                ifta80_t2t_01_banks_bitmap));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_rm_tcam_slice_sid_info_get(int unit,
                                 bcmdrd_sid_t bank_sid,
                                 bcmdrd_sid_t *bank_sids,
                                 uint8_t *slice_id,
                                 uint8_t *ltpr_value,
                                 bcmdrd_sid_t *ltpr_slice_sid)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bank_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(slice_id, SHR_E_PARAM);
    SHR_NULL_CHECK(ltpr_slice_sid, SHR_E_PARAM);

    switch (bank_sid) {
    case MEMDB_TCAM_IFTA80_MEM0_0m:
    case MEMDB_TCAM_IFTA80_MEM0_1m:
    case MEMDB_TCAM_IFTA80_MEM1_0m:
    case MEMDB_TCAM_IFTA80_MEM1_1m:

        *slice_id = 0;
        *ltpr_slice_sid = IFTA80_T2T_00_LTPR_PROFILE_TABLE_0m;
        *ltpr_value = 0x1;
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM0_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM0_1m;
        bank_sids[2] = MEMDB_TCAM_IFTA80_MEM1_0m;
        bank_sids[3] = MEMDB_TCAM_IFTA80_MEM1_1m;

        break;
    case MEMDB_TCAM_IFTA80_MEM2_0m:
    case MEMDB_TCAM_IFTA80_MEM2_1m:
    case MEMDB_TCAM_IFTA80_MEM3_0m:
    case MEMDB_TCAM_IFTA80_MEM3_1m:

        *slice_id = 1;
        *ltpr_slice_sid = IFTA80_T2T_00_LTPR_PROFILE_TABLE_1m;
        *ltpr_value = 0x2;
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM2_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM2_1m;
        bank_sids[2] = MEMDB_TCAM_IFTA80_MEM3_0m;
        bank_sids[3] = MEMDB_TCAM_IFTA80_MEM3_1m;

        break;
    case MEMDB_TCAM_IFTA80_MEM4_0m:
    case MEMDB_TCAM_IFTA80_MEM4_1m:
    case MEMDB_TCAM_IFTA80_MEM5_0m:
    case MEMDB_TCAM_IFTA80_MEM5_1m:

        *slice_id = 2;
        *ltpr_slice_sid = IFTA80_T2T_01_LTPR_PROFILE_TABLE_0m;
        *ltpr_value = 0x4;
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM4_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM4_1m;
        bank_sids[2] = MEMDB_TCAM_IFTA80_MEM5_0m;
        bank_sids[3] = MEMDB_TCAM_IFTA80_MEM5_1m;

        break;
    case MEMDB_TCAM_IFTA80_MEM6_0m:
    case MEMDB_TCAM_IFTA80_MEM6_1m:
    case MEMDB_TCAM_IFTA80_MEM7_0m:
    case MEMDB_TCAM_IFTA80_MEM7_1m:

        *slice_id = 3;
        *ltpr_slice_sid = IFTA80_T2T_01_LTPR_PROFILE_TABLE_1m;
        *ltpr_value = 0x8;
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM6_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM6_1m;
        bank_sids[2] = MEMDB_TCAM_IFTA80_MEM7_0m;
        bank_sids[3] = MEMDB_TCAM_IFTA80_MEM7_1m;

        break;
    case MEMDB_TCAM_IFTA40_MEM0_0_ONLYm:
        *slice_id = 0;
        *ltpr_slice_sid = IFTA40_T4T_00_LTPR_PROFILE_TABLE_0m;
        *ltpr_value = 0x1;
        bank_sids[0] = MEMDB_TCAM_IFTA40_MEM0_0_ONLYm;
        break;
    case MEMDB_TCAM_IFTA40_MEM0_1_ONLYm:
        *slice_id = 1;
        *ltpr_slice_sid = IFTA40_T4T_00_LTPR_PROFILE_TABLE_1m;
        *ltpr_value = 0x2;
        bank_sids[0] = MEMDB_TCAM_IFTA40_MEM0_1_ONLYm;
        break;
    case MEMDB_TCAM_IFTA40_MEM0_2_ONLYm:
        *slice_id = 2;
        *ltpr_slice_sid = IFTA40_T4T_00_LTPR_PROFILE_TABLE_2m;
        *ltpr_value = 0x4;
        bank_sids[0] = MEMDB_TCAM_IFTA40_MEM0_2_ONLYm;
        break;
    case MEMDB_TCAM_IFTA40_MEM0_3_ONLYm:
        *slice_id = 3;
        *ltpr_slice_sid = IFTA40_T4T_00_LTPR_PROFILE_TABLE_3m;
        *ltpr_value = 0x8;
        bank_sids[0] = MEMDB_TCAM_IFTA40_MEM0_3_ONLYm;
        break;
    case MEMDB_TCAM_IFTA40_MEM1_0_ONLYm:
        *slice_id = 4;
        *ltpr_slice_sid = IFTA40_T4T_01_LTPR_PROFILE_TABLE_0m;
        *ltpr_value = 0x10;
        bank_sids[0] = MEMDB_TCAM_IFTA40_MEM1_0_ONLYm;
        break;
    case MEMDB_TCAM_IFTA40_MEM1_1_ONLYm:
        *slice_id = 5;
        *ltpr_slice_sid = IFTA40_T4T_01_LTPR_PROFILE_TABLE_1m;
        *ltpr_value = 0x20;
        bank_sids[0] = MEMDB_TCAM_IFTA40_MEM1_1_ONLYm;
        break;
    case MEMDB_TCAM_IFTA40_MEM1_2_ONLYm:
        *slice_id = 6;
        *ltpr_slice_sid = IFTA40_T4T_01_LTPR_PROFILE_TABLE_2m;
        *ltpr_value = 0x40;
        bank_sids[0] = MEMDB_TCAM_IFTA40_MEM1_2_ONLYm;
        break;
    case MEMDB_TCAM_IFTA40_MEM1_3_ONLYm:
        *slice_id = 7;
        *ltpr_slice_sid = IFTA40_T4T_01_LTPR_PROFILE_TABLE_3m;
        *ltpr_value = 0x80;
        bank_sids[0] = MEMDB_TCAM_IFTA40_MEM1_3_ONLYm;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_rm_tcam_sid_info_get(int unit,
                                 bcmdrd_sid_t bank_sid,
                                 bcmdrd_sid_t *bank_sids,
                                 bcmdrd_sid_t *tcam_sid,
                                 bcmdrd_sid_t *data_sid,
                                 uint8_t multi_aggr_no_data_sid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(bank_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(tcam_sid, SHR_E_PARAM);
    SHR_NULL_CHECK(data_sid, SHR_E_PARAM);

    bank_sids[0] = bank_sid;

    switch (bank_sid) {
    case MEMDB_TCAM_IFTA80_MEM0_0m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM0_1m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM0_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM0_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM1_0m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM1_1m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM1_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM1_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM2_0m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM2_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM2_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM2_1m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM2_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM2_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM3_0m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM3_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM3_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM3_1m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM3_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM3_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM4_0m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM4_1m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM4_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM4_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM5_0m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM5_1m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM5_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM5_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM6_0m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM6_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM6_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM6_1m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM6_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM6_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM7_0m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM7_1m:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM7_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM7_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA40_MEM1_0_ONLYm:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA40_MEM1_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA40_MEM1_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA40_MEM1_1_ONLYm:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA40_MEM1_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA40_MEM1_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA40_MEM1_2_ONLYm:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA40_MEM1_2_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA40_MEM1_2_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA40_MEM1_3_ONLYm:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA40_MEM1_3_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA40_MEM1_3_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA40_MEM0_0_ONLYm:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA40_MEM0_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA40_MEM0_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA40_MEM0_1_ONLYm:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA40_MEM0_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA40_MEM0_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA40_MEM0_2_ONLYm:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA40_MEM0_2_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA40_MEM0_2_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA40_MEM0_3_ONLYm:
        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA40_MEM0_3_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA40_MEM0_3_DATA_ONLYm;
        }
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}
static int
bcm56780_a0_rm_tcam_dbl_sid_info_get(int unit,
                                 bcmdrd_sid_t bank_sid,
                                 bcmdrd_sid_t *bank_sids,
                                 bcmdrd_sid_t *tcam_sid,
                                 bcmdrd_sid_t *data_sid,
                                 uint8_t multi_aggr_no_data_sid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(bank_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(tcam_sid, SHR_E_PARAM);
    SHR_NULL_CHECK(data_sid, SHR_E_PARAM);

    switch (bank_sid) {
    case MEMDB_TCAM_IFTA80_MEM0_0m:
    case MEMDB_TCAM_IFTA80_MEM0_1m:
        /* Bank sids */
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM0_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM0_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM0_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM0_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM1_0m:
    case MEMDB_TCAM_IFTA80_MEM1_1m:
        /* Bank sids */
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM1_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM1_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM1_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM1_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM2_0m:
    case MEMDB_TCAM_IFTA80_MEM2_1m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM2_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM2_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM2_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM2_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM2_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM3_0m:
    case MEMDB_TCAM_IFTA80_MEM3_1m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM3_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM3_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM3_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM3_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM3_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM4_0m:
    case MEMDB_TCAM_IFTA80_MEM4_1m:
        /* Bank sids */
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM4_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM4_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM4_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM4_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM5_0m:
    case MEMDB_TCAM_IFTA80_MEM5_1m:
        /* Bank sids */
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM5_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM5_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM5_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM5_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM6_0m:
    case MEMDB_TCAM_IFTA80_MEM6_1m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM6_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM6_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM6_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM6_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM6_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM7_0m:
    case MEMDB_TCAM_IFTA80_MEM7_1m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM7_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM7_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM7_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM7_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA40_MEM0_2_ONLYm:
    case MEMDB_TCAM_IFTA40_MEM0_3_ONLYm:

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            bank_sids[0] = MEMDB_TCAM_IFTA40_MEM0_2_ONLYm;
            bank_sids[1] = MEMDB_TCAM_IFTA40_MEM0_3_ONLYm;

            tcam_sid[0] = MEMDB_TCAM_IFTA40_MEM0_2_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA40_MEM0_3_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA40_MEM0_3_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA40_MEM1_2_ONLYm:
    case MEMDB_TCAM_IFTA40_MEM1_3_ONLYm:

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            bank_sids[0] = MEMDB_TCAM_IFTA40_MEM1_2_ONLYm;
            bank_sids[1] = MEMDB_TCAM_IFTA40_MEM1_3_ONLYm;

            tcam_sid[0] = MEMDB_TCAM_IFTA40_MEM1_2_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA40_MEM1_3_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA40_MEM1_3_DATA_ONLYm;
        }
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}
static int
bcm56780_a0_rm_tcam_dbl_urpf_sid_info_get(int unit,
                                 bcmdrd_sid_t bank_sid,
                                 bcmdrd_sid_t *bank_sids,
                                 bcmdrd_sid_t *tcam_sid,
                                 bcmdrd_sid_t *data_sid,
                                 uint8_t multi_aggr_no_data_sid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(bank_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(tcam_sid, SHR_E_PARAM);
    SHR_NULL_CHECK(data_sid, SHR_E_PARAM);

    switch (bank_sid) {
    case MEMDB_TCAM_IFTA80_MEM0_0m:
    case MEMDB_TCAM_IFTA80_MEM0_1m:
    case MEMDB_TCAM_IFTA80_MEM2_0m:
    case MEMDB_TCAM_IFTA80_MEM2_1m:
        /* Bank sids */
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM0_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM0_1m;
        bank_sids[2] = MEMDB_TCAM_IFTA80_MEM2_0m;
        bank_sids[3] = MEMDB_TCAM_IFTA80_MEM2_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
            tcam_sid[2] = bank_sids[2];
            tcam_sid[3] = bank_sids[3];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM0_1_ONLYm;
            tcam_sid[2] = MEMDB_TCAM_IFTA80_MEM2_0_ONLYm;
            tcam_sid[3] = MEMDB_TCAM_IFTA80_MEM2_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM0_1_DATA_ONLYm;
            data_sid[1] = MEMDB_TCAM_IFTA80_MEM2_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM1_0m:
    case MEMDB_TCAM_IFTA80_MEM1_1m:
    case MEMDB_TCAM_IFTA80_MEM3_0m:
    case MEMDB_TCAM_IFTA80_MEM3_1m:
        /* Bank sids */
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM1_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM1_1m;
        bank_sids[2] = MEMDB_TCAM_IFTA80_MEM3_0m;
        bank_sids[3] = MEMDB_TCAM_IFTA80_MEM3_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
            tcam_sid[2] = bank_sids[2];
            tcam_sid[3] = bank_sids[3];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM1_1_ONLYm;
            tcam_sid[2] = MEMDB_TCAM_IFTA80_MEM3_0_ONLYm;
            tcam_sid[3] = MEMDB_TCAM_IFTA80_MEM3_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM1_1_DATA_ONLYm;
            data_sid[1] = MEMDB_TCAM_IFTA80_MEM3_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM4_0m:
    case MEMDB_TCAM_IFTA80_MEM4_1m:
    case MEMDB_TCAM_IFTA80_MEM6_0m:
    case MEMDB_TCAM_IFTA80_MEM6_1m:
        /* Bank sids */
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM4_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM4_1m;
        bank_sids[2] = MEMDB_TCAM_IFTA80_MEM6_0m;
        bank_sids[3] = MEMDB_TCAM_IFTA80_MEM6_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
            tcam_sid[2] = bank_sids[2];
            tcam_sid[3] = bank_sids[3];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM4_1_ONLYm;
            tcam_sid[2] = MEMDB_TCAM_IFTA80_MEM6_0_ONLYm;
            tcam_sid[3] = MEMDB_TCAM_IFTA80_MEM6_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM4_1_DATA_ONLYm;
            data_sid[1] = MEMDB_TCAM_IFTA80_MEM6_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM5_0m:
    case MEMDB_TCAM_IFTA80_MEM5_1m:
    case MEMDB_TCAM_IFTA80_MEM7_0m:
    case MEMDB_TCAM_IFTA80_MEM7_1m:
        /* Bank sids */
        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM5_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM5_1m;
        bank_sids[2] = MEMDB_TCAM_IFTA80_MEM7_0m;
        bank_sids[3] = MEMDB_TCAM_IFTA80_MEM7_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
            tcam_sid[2] = bank_sids[2];
            tcam_sid[3] = bank_sids[3];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM5_1_ONLYm;
            tcam_sid[2] = MEMDB_TCAM_IFTA80_MEM7_0_ONLYm;
            tcam_sid[3] = MEMDB_TCAM_IFTA80_MEM7_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM5_1_DATA_ONLYm;
            data_sid[1] = MEMDB_TCAM_IFTA80_MEM7_1_DATA_ONLYm;
        }
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}
static int
bcm56780_a0_rm_tcam_urpf_paired_bank_get(int unit,
                                 bcmdrd_sid_t bank_sid,
                                 bcmdrd_sid_t *bank_sids,
                                 bcmdrd_sid_t *tcam_sid,
                                 bcmdrd_sid_t *data_sid,
                                 uint8_t multi_aggr_no_data_sid)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(bank_sids, SHR_E_PARAM);
    SHR_NULL_CHECK(tcam_sid, SHR_E_PARAM);
    SHR_NULL_CHECK(data_sid, SHR_E_PARAM);

    switch (bank_sid) {
    case MEMDB_TCAM_IFTA80_MEM0_0m:
    case MEMDB_TCAM_IFTA80_MEM2_0m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM0_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM2_0m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM2_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm;
            data_sid[1] = MEMDB_TCAM_IFTA80_MEM2_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM0_1m:
    case MEMDB_TCAM_IFTA80_MEM2_1m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM0_1m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM2_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM0_1_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM2_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM0_1_DATA_ONLYm;
            data_sid[1] = MEMDB_TCAM_IFTA80_MEM2_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM1_0m:
    case MEMDB_TCAM_IFTA80_MEM3_0m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM1_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM3_0m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM3_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0_DATA_ONLYm;
            data_sid[1] = MEMDB_TCAM_IFTA80_MEM3_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM1_1m:
    case MEMDB_TCAM_IFTA80_MEM3_1m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM1_1m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM3_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM1_1_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM3_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM1_1_DATA_ONLYm;
            data_sid[1] = MEMDB_TCAM_IFTA80_MEM3_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM4_0m:
    case MEMDB_TCAM_IFTA80_MEM6_0m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM4_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM6_0m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM6_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_DATA_ONLYm;
            data_sid[1] = MEMDB_TCAM_IFTA80_MEM6_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM4_1m:
    case MEMDB_TCAM_IFTA80_MEM6_1m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM4_1m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM6_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM4_1_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM6_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM4_1_DATA_ONLYm;
            data_sid[1] = MEMDB_TCAM_IFTA80_MEM6_1_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM5_0m:
    case MEMDB_TCAM_IFTA80_MEM7_0m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM5_0m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM7_0m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM7_0_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_DATA_ONLYm;
            data_sid[1] = MEMDB_TCAM_IFTA80_MEM7_0_DATA_ONLYm;
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM5_1m:
    case MEMDB_TCAM_IFTA80_MEM7_1m:

        bank_sids[0] = MEMDB_TCAM_IFTA80_MEM5_1m;
        bank_sids[1] = MEMDB_TCAM_IFTA80_MEM7_1m;

        if (multi_aggr_no_data_sid) {
            tcam_sid[0] = bank_sids[0];
            tcam_sid[1] = bank_sids[1];
        } else {
            tcam_sid[0] = MEMDB_TCAM_IFTA80_MEM5_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM5_1_DATA_ONLYm;
            tcam_sid[1] = MEMDB_TCAM_IFTA80_MEM7_1_ONLYm;
            data_sid[0] = MEMDB_TCAM_IFTA80_MEM7_1_DATA_ONLYm;
        }
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 *  \brief Check whether the bank_sid is in pt_banks_info
 *  \n recevied from UFT manager. If it is present, return the index
 *  \n at which bank_sid is present in pt_banks_info. If bank_sid is
 *  \n present in pt_banks_info->lookup1_banks, set lookup1_bank to 1.
 *
 *  \param [in] unit Logical device id
 *  \param [in] pt_banks_info banks info received from UFT
 *  \param [in] bank_sid
 *  \param [out] lookup1_bank
 *  \param [out] bank_idx
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Failure
 */
static int
check_bank_avaliability(int unit,
                         bcmptm_pt_banks_info_t *pt_banks_info,
                         bcmdrd_sid_t bank_sid,
                         bool *lookup1_bank,
                         uint8_t *bank_idx)
{
    int i;
    bcmdrd_sid_t bank_id;
    bool found = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pt_banks_info, SHR_E_FAIL);

    for (i = 0; i < pt_banks_info->bank_cnt; i++) {
        bank_id = pt_banks_info->bank[i].bank_sid;
        if (bank_id == bank_sid) {
            found = true;
            *bank_idx = i;
            break;
        }
    }
    for (i = 0; i < pt_banks_info->lookup1_bank_cnt; i++) {
        bank_id = pt_banks_info->lookup1_bank_sid[i];
        if (bank_id == bank_sid) {
            found = true;
            *bank_idx = i;
            *lookup1_bank = true;
            break;
        }
    }
    if (found == false) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
check_banks_and_mark_ltpr_used(int unit,
                         bcmptm_pt_banks_info_t *pt_banks_info,
                         bcmdrd_sid_t *banks_info,
                         uint8_t *is_used_bank)
{
    int rv;
    int idx;
    uint8_t bank_index = 0;
    bool lookup1_bank = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pt_banks_info, SHR_E_FAIL);
    SHR_NULL_CHECK(is_used_bank, SHR_E_FAIL);

    for (idx = 0; idx < BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES; idx++) {
        if (banks_info[idx] != 0) {
            rv = check_bank_avaliability(unit,
                                     pt_banks_info,
                                     banks_info[idx],
                                     &lookup1_bank,
                                     &bank_index);
            if (rv == SHR_E_NONE) {
                is_used_bank[bank_index] = 1;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
banks_sid_to_hw_ltid_get(int unit,
                         bcmdrd_sid_t bank_sid,
                         bcmptm_lt_lookup_banks_info_t *banks_lookup_info,
                         int32_t *hw_ltid)
{
    int idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(banks_lookup_info, SHR_E_PARAM);

    *hw_ltid = -1;

    for (idx = 0; idx < banks_lookup_info->bank_cnt; idx++) {
        if (banks_lookup_info->bank_attr[idx].bank_sid == bank_sid) {
            *hw_ltid = banks_lookup_info->bank_attr[idx].hw_ltid;
        }
    }

    if (*hw_ltid == -1) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 *  \brief Iterate over banks_info sid's.check whether sid is present
 *  \n in pt_banks_info recevied from UFT manager. If it is present,
 *  \n set is_used_bank[index] at which bank_sid is found in pt_banks_info.
 *  \n If bank_sid is not present in pt_banks_info,
 *  \n set bank_not_present[idx] at which bank_sid is present in banks_info.
 *
 *  \param [in] unit Logical device id
 *  \param [in] pt_banks_info banks info received from UFT
 *  \param [in] banks_info array of sid's
 *  \param [out] bank_not_present array
 *  \param [out] is_used_bank array
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Failure
 */
static int
check_banks_and_mark_used(int unit,
                         bcmptm_pt_banks_info_t *pt_banks_info,
                         bcmdrd_sid_t *banks_info,
                         uint8_t *bank_not_present,
                         uint8_t *is_used_bank)
{
    int rv;
    int idx;
    uint8_t bank_index = 0;
    bool lookup1_bank = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pt_banks_info, SHR_E_FAIL);
    SHR_NULL_CHECK(is_used_bank, SHR_E_FAIL);

    for (idx = 0; idx < BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES; idx++) {
        if (banks_info[idx] != 0) {
            rv = check_bank_avaliability(unit,
                                     pt_banks_info,
                                     banks_info[idx],
                                     &lookup1_bank,
                                     &bank_index);
            if (rv != SHR_E_NONE) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                    "URPF/WIDE mode Banks not allocated properly.\n")));
                bank_not_present[idx] = 1;
            } else {
                if (!lookup1_bank) {
                    is_used_bank[bank_index] = 1;
                } else {
                    lookup1_bank = false;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_rm_tcam_defip_program_ltpr_table(int unit,
                                        uint32_t trans_id,
                                        bcmltd_sid_t ltid,
                                        bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info,
                                        bcmptm_pt_banks_info_t *pt_banks_info,
                                        bool warm)
{
    uint32_t idx = 0;
    bcmdrd_sid_t bank_sid = 0;
    bcmdrd_sid_t banks_info[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES] = {0};
    uint8_t is_bank_used[UFT_VAL_BANKS_MAX] = {0};
    uint8_t bank_cnt = 0;
    bcmptm_lt_lookup_banks_info_t banks_lookup_info;
    uint32_t e_buffer[BCMPTM_MAX_PT_FIELD_WORDS] = { 0 };
    bcmbd_pt_dyn_info_t dyn_info;
    int tbl_inst = -1;
    uint32_t rsp_flags = 0;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t full_ltpr_value[4] = {0};
    uint8_t ltpr_value = 0;
    uint32_t lookup_id = 0;
    bcmdrd_sid_t ltpr_slice_sid = INVALIDm;
    uint8_t slice_id = 0;
    int32_t hw_ltid = -1;
    uint8_t ltpr_idx = 0;
    uint8_t i = 0, j = 0;
    uint8_t ltpr_idx_found = 0;


    SHR_FUNC_ENTER(unit);

    if (warm == true) {
        SHR_EXIT();
    }

    for (lookup_id = 0; lookup_id < 2; lookup_id++) {

        if (lookup_id == 0) {
            bank_cnt = pt_banks_info->bank_cnt;
        } else {
            bank_cnt = pt_banks_info->lookup1_bank_cnt;
        }

        if (bank_cnt == 0) {
            continue;
        }

        sal_memset(&banks_lookup_info, 0, sizeof(bcmptm_lt_lookup_banks_info_t));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_uft_bank_hw_attr_get_from_lt(unit,
                                                 ltid,
                                                 lookup_id,
                                                 &banks_lookup_info));

        sal_memset(&is_bank_used, 0, sizeof(uint8_t) * UFT_VAL_BANKS_MAX);

        sal_memset(full_ltpr_value, 0, sizeof(uint32_t) * 4);

        for (idx = 0; idx < bank_cnt; idx++) {

            if (is_bank_used[idx] == 1) {
                continue;
            }

            if (lookup_id == 0) {
                bank_sid = pt_banks_info->bank[idx].bank_sid;
            } else {
                bank_sid = pt_banks_info->lookup1_bank_sid[idx];
            }

            SHR_IF_ERR_VERBOSE_EXIT
               (bcm56780_a0_rm_tcam_slice_sid_info_get(unit,
                                     bank_sid,
                                     banks_info,
                                     &slice_id,
                                     &ltpr_value,
                                     &ltpr_slice_sid));

            ltpr_idx = 0;
            ltpr_idx_found = 0;
            if (lookup_id != 1) {
                for (j = 0; j < hw_entry_info->num_depth_inst; j++) {
                    for (i = 0; i < hw_entry_info->num_key_rows; i++) {
                        if (hw_entry_info->sid[j][i] == banks_info[0]) {
                            ltpr_idx = i;
                            ltpr_idx_found = 1;
                            break;
                        }
                    }
                    if (ltpr_idx_found == 1) {
                        break;
                    }
                }
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (check_banks_and_mark_ltpr_used(unit,
                                         pt_banks_info,
                                         banks_info,
                                         is_bank_used));

            SHR_IF_ERR_VERBOSE_EXIT
               (banks_sid_to_hw_ltid_get(unit,
                                         bank_sid,
                                         &banks_lookup_info,
                                         &hw_ltid));

           /* write ltpr programming for this tile, slice */
           /* get the hw_ltid for this bank_sid */
            dyn_info.tbl_inst = tbl_inst;
            dyn_info.index = hw_ltid;
            full_ltpr_value[ltpr_idx] |= ltpr_value;
            sal_memset(e_buffer, 0, sizeof(e_buffer));
            bcmdrd_field_set(e_buffer,
                             0,
                             11,
                             &full_ltpr_value[ltpr_idx]);
             SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_mreq_indexed_lt(unit,
                                        0,
                                        ltpr_slice_sid,
                                        &dyn_info,
                                        NULL,
                                        NULL,
                                        BCMPTM_OP_WRITE,
                                        e_buffer,
                                        1,
                                        ltid,
                                        trans_id,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &rsp_ltid,
                                        &rsp_flags));
             if (trans_id == BCMPTM_DIRECT_TRANS_ID) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_mreq_indexed_lt(unit,
                                        BCMPTM_REQ_FLAGS_COMMIT,
                                        INVALIDm,
                                        NULL,
                                        NULL,
                                        NULL,
                                        BCMPTM_OP_NOP,
                                        NULL,
                                        0,
                                        0,
                                        BCMPTM_DIRECT_TRANS_ID,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &rsp_flags));
            }
        }
    }
exit:
     SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_fp_dyn_lt_hw_info_init(int unit,
                                      const lt_mreq_info_t *orig_lrd_info,
                                      lt_mreq_info_t *new_lrd_info,
                                      bcmptm_pt_banks_info_t *pt_banks_info)
{
    uint32_t idx = 0, alloc_sz = 0, bank_id = 0;
    uint8_t i =0;
    const bcmdrd_sid_t **sid_arr = NULL;
    const bcmdrd_sid_t **sid_data_arr = NULL;
    const bcmdrd_sid_t **presel_sid_arr = NULL;
    const uint32_t **entry_count_arr = NULL;
    uint32_t *entry_cnt = NULL;
    uint32_t entry_count_array[BCMPTM_RM_TCAM_PIPE_COUNT_MAX]
                              [BCMPTM_RM_TCAM_MAX_SLICES];
    const uint32_t **shared_entry_count_arr = NULL;
    uint32_t shared_entry_count_array[BCMPTM_RM_TCAM_PIPE_COUNT_MAX]
                              [BCMPTM_RM_TCAM_MAX_SLICES];
    uint8_t *slice_cnt_arr = NULL;
    uint8_t *shared_slice_cnt_arr = NULL;
    bcmptm_rm_tcam_slice_info_t *slice_info = NULL;
    bcmptm_rm_tcam_slice_info_t *shared_slice_info = NULL;
    bcmptm_rm_tcam_tile_info_t *tile_info = NULL;
    bcmdrd_sid_t *sid = NULL;
    uint8_t num_key_rows = 0;
    uint8_t new_depth_inst = 0;
    uint8_t num_data_rows = 0;
    uint8_t presel_num_key_rows = 0;
    bcmdrd_sid_t **tcam_sid = NULL;
    bcmdrd_sid_t **data_sid = NULL;
    bcmdrd_sid_t **presel_tcam_sid = NULL;
    uint32_t num_slices = 0;
    uint8_t num_sids_per_slice = 0;
    uint8_t slice_idx = 0;
    uint8_t num_rows = 0;
    uint32_t next_start_idx = 0;
    uint32_t num_entries = 0;
    uint32_t presel_num_entries = 0;
    uint8_t iter = 0;
    uint8_t pipe = 0;
    lt_mreq_info_t *new_shared_lrd_info = NULL;
    const lt_mreq_info_t *shared_lrd_info = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *more_info = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *new_hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *new_more_info = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *shared_hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *shared_more_info = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *new_shared_hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *new_shared_more_info = NULL;
    uint8_t orig_slice_id = 0;
    uint8_t bank_processed[UFT_VAL_BANKS_MAX] = {0};

    SHR_FUNC_ENTER(unit);

    /* Original lrd info */
    more_info = (bcmptm_rm_tcam_more_info_t *)
                            orig_lrd_info->rm_more_info;
    hw_entry_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                            orig_lrd_info->hw_entry_info;
    SHR_NULL_CHECK(hw_entry_info, SHR_E_FAIL);
    SHR_NULL_CHECK(more_info, SHR_E_FAIL);

    /* New Lrd info */
    new_more_info = (bcmptm_rm_tcam_more_info_t *)new_lrd_info->rm_more_info;
    new_hw_entry_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                            new_lrd_info->hw_entry_info;
    SHR_NULL_CHECK(new_hw_entry_info, SHR_E_FAIL);
    SHR_NULL_CHECK(new_more_info, SHR_E_FAIL);

    /* Clean up the previous configured shared lt's lrd info */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_dyn_ptrm_tbl_get(unit,
                                         more_info->shared_lt_info[0],
                                         &new_shared_lrd_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_dyn_lt_hw_info_cleanup(unit, new_shared_lrd_info));

    /* Original shared lrd info */
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_lt_mreq_info_get(unit,
                                     more_info->shared_lt_info[0],
                                     &shared_lrd_info));

    shared_more_info = (bcmptm_rm_tcam_more_info_t *)
                                    shared_lrd_info->rm_more_info;
    shared_hw_entry_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                                    shared_lrd_info->hw_entry_info;
    SHR_NULL_CHECK(shared_hw_entry_info, SHR_E_FAIL);
    SHR_NULL_CHECK(shared_more_info, SHR_E_FAIL);

    /* New Shared Lrd info */
    new_shared_more_info = (bcmptm_rm_tcam_more_info_t *)
                                new_shared_lrd_info->rm_more_info;
    new_shared_hw_entry_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                                new_shared_lrd_info->hw_entry_info;
    SHR_NULL_CHECK(new_shared_hw_entry_info, SHR_E_FAIL);
    SHR_NULL_CHECK(new_shared_more_info, SHR_E_FAIL);

    /* Construct new hw_info and slice_info for both main LT and shred LT */

    /* Allocations for SID Arrays */
    alloc_sz = BCMPTM_RM_TCAM_MAX_DEPTH_INSTANCES * sizeof(bcmdrd_sid_t *);
    SHR_ALLOC(tcam_sid, alloc_sz, "bcmptmRmTcamSidPtr");
    SHR_ALLOC(data_sid, alloc_sz, "bcmptmRmDataSidPtr");
    SHR_ALLOC(presel_tcam_sid, alloc_sz, "bcmptmRmPreselTcamSidPtr");

    sal_memset(tcam_sid, 0, alloc_sz);
    sal_memset(data_sid, 0, alloc_sz);
    sal_memset(presel_tcam_sid, 0, alloc_sz);

    alloc_sz = BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES *  sizeof(bcmdrd_sid_t);
    for (i = 0; i < BCMPTM_RM_TCAM_MAX_DEPTH_INSTANCES; i++) {
        SHR_ALLOC(tcam_sid[i], alloc_sz, "bcmptmRmTcamSidArr");
        SHR_ALLOC(data_sid[i], alloc_sz, "bcmptmRmDataSidArr");
        SHR_ALLOC(presel_tcam_sid[i], alloc_sz, "bcmptmRmPreselTcamSidArr");
    }

    /* Main Slice info */
    num_slices = pt_banks_info->bank_cnt;
    alloc_sz = (num_slices * sizeof(bcmptm_rm_tcam_slice_info_t));
    SHR_ALLOC(slice_info, alloc_sz, "bcmptmRmSliceInfoArr");
    sal_memset(slice_info, 0, alloc_sz);

    /* Shared Slice info */
    SHR_ALLOC(shared_slice_info, alloc_sz, "bcmptmRmSliceInfoArr");
    sal_memset(shared_slice_info, 0, alloc_sz);

    /* Tile Info */
    alloc_sz = (more_info->tile_count * sizeof(bcmptm_rm_tcam_tile_info_t));
    SHR_ALLOC(tile_info, alloc_sz, "bcmptmRmTileInfoArr");
    sal_memset(tile_info, 0, alloc_sz);

    /* Update Hw entry info */
    for (idx = 0; idx < orig_lrd_info->hw_entry_info_count; idx++) {


        for (i = 0; i < BCMPTM_RM_TCAM_MAX_DEPTH_INSTANCES; i++) {
            sal_memset(tcam_sid[i], 0,
                       (BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES *
                        sizeof(bcmdrd_sid_t)));
            sal_memset(data_sid[i], 0,
                       (BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES *
                        sizeof(bcmdrd_sid_t)));
            sal_memset(presel_tcam_sid[i], 0,
                       (BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES *
                        sizeof(bcmdrd_sid_t)));
        }
        num_rows = hw_entry_info[idx].num_depth_inst;
        new_depth_inst = 0;

        num_key_rows = hw_entry_info[idx].num_key_rows;
        num_data_rows =  hw_entry_info[idx].num_data_rows;

        presel_num_key_rows = shared_hw_entry_info[idx].num_key_rows;
        bank_id = 0;
        sal_memset(&bank_processed, 0, sizeof(uint8_t) * UFT_VAL_BANKS_MAX);
        for (iter = 0; iter < num_rows; iter++) {
            for (bank_id = 0; bank_id < pt_banks_info->bank_cnt; bank_id++) {
                if (bank_processed[bank_id] == 1) {
                    continue;
                }
                if (pt_banks_info->bank[bank_id].bank_sid ==
                      hw_entry_info[idx].sid[iter][0]) {
                    sal_memcpy((tcam_sid[new_depth_inst]),
                               hw_entry_info[idx].sid[iter],
                               num_key_rows * sizeof(bcmdrd_sid_t));
                    sal_memcpy((data_sid[new_depth_inst]),
                               hw_entry_info[idx].sid_data_only[iter],
                               num_data_rows * sizeof(bcmdrd_sid_t));
                    sal_memcpy((presel_tcam_sid[new_depth_inst]),
                               shared_hw_entry_info[idx].sid[iter],
                               presel_num_key_rows * sizeof(bcmdrd_sid_t));
                    if (idx == 0) {
                        slice_info[new_depth_inst].slice_id = iter;
                        shared_slice_info[new_depth_inst].slice_id = iter;
                    }
                    bank_processed[bank_id] = 1;
                    new_depth_inst++;
                    bank_id++;
                    if (bank_id == pt_banks_info->bank_cnt) {
                        break;
                    }
                }
            }
        }

        if (new_depth_inst == 0) {
            continue;
        }
        sid_arr = NULL;
        alloc_sz = (new_depth_inst * sizeof(bcmdrd_sid_t *));
        SHR_ALLOC(sid_arr, alloc_sz, "bcmptmRmTcamSidArr");
        sal_memset(sid_arr, 0, alloc_sz);

        presel_sid_arr = NULL;
        SHR_ALLOC(presel_sid_arr, alloc_sz, "bcmptmRmTcamPreselSidArr");
        sal_memset(sid_arr, 0, alloc_sz);

        sid_data_arr = NULL;
        alloc_sz = (new_depth_inst * sizeof(bcmdrd_sid_t *));
        SHR_ALLOC(sid_data_arr, alloc_sz, "bcmptmRmTcamSidArr");
        sal_memset(sid_data_arr, 0, alloc_sz);

        for (iter = 0; iter < new_depth_inst; iter++) {
            sid = NULL;
            alloc_sz = (num_key_rows * sizeof(bcmdrd_sid_t));
            SHR_ALLOC(sid, alloc_sz, "bcmptmRmTcamSid");
            sal_memset(sid, 0, alloc_sz);
            sal_memcpy(sid, tcam_sid[iter], alloc_sz);
            sid_arr[iter] = sid;

            sid = NULL;
            alloc_sz = (num_data_rows * sizeof(bcmdrd_sid_t));
            SHR_ALLOC(sid, alloc_sz, "bcmptmRmDataSid");
            sal_memset(sid, 0, alloc_sz);
            sal_memcpy(sid, data_sid[iter], alloc_sz);
            sid_data_arr[iter] = sid;

            /* Presel sid's */
            sid = NULL;
            alloc_sz = (presel_num_key_rows * sizeof(bcmdrd_sid_t));
            SHR_ALLOC(sid, alloc_sz, "bcmptmRmPreselTcamSid");
            sal_memset(sid, 0, alloc_sz);
            sal_memcpy(sid, presel_tcam_sid[iter], alloc_sz);
            presel_sid_arr[iter] = sid;
        }
        new_hw_entry_info[idx].sid = sid_arr;
        new_hw_entry_info[idx].sid_data_only = sid_data_arr;
        new_hw_entry_info[idx].num_depth_inst = new_depth_inst;

        new_shared_hw_entry_info[idx].sid = presel_sid_arr;
        new_shared_hw_entry_info[idx].sid_data_only = NULL;
        new_shared_hw_entry_info[idx].num_depth_inst = new_depth_inst;
    }

    /* Construct more_info for both main and shared Lt's */

    /* Slice count */
    alloc_sz = (more_info->pipe_count * sizeof(uint8_t));
    SHR_ALLOC(slice_cnt_arr, alloc_sz, "bcmptmRmSliceCountArr");
    sal_memset(slice_cnt_arr, 0, alloc_sz);

    /* Shared slice count */
    SHR_ALLOC(shared_slice_cnt_arr, alloc_sz, "bcmptmRmSharedSliceCountArr");
    sal_memset(shared_slice_cnt_arr, 0, alloc_sz);

    /* Entry count */
    alloc_sz = (more_info->pipe_count * sizeof(uint32_t *));
    SHR_ALLOC(entry_count_arr, alloc_sz, "bcmptmRmEntryCountArr");
    sal_memset(entry_count_arr, 0, alloc_sz);

    /* Shared entry count */
    SHR_ALLOC(shared_entry_count_arr, alloc_sz, "bcmptmRmSharedEntryCountArr");
    sal_memset(shared_entry_count_arr, 0, alloc_sz);

    sal_memset(entry_count_array, 0,
               more_info->pipe_count * num_slices * sizeof(uint32_t));
    sal_memset(shared_entry_count_array, 0,
               more_info->pipe_count * num_slices * sizeof(uint32_t));


    /* Update slice_info for both main and shared LT's */
    num_sids_per_slice = more_info->tile_info[0].num_tcam_sids_per_slice;
    num_rows = new_hw_entry_info[0].num_depth_inst;
    slice_idx = 0;
    next_start_idx = 0;
    num_entries = 0;
    for (iter = 0; iter <= num_rows; iter++) {
        if (iter != 0 && iter % num_sids_per_slice == 0) {

            orig_slice_id =  slice_info[slice_idx].slice_id;
            sal_memcpy(&(slice_info[slice_idx]), &(more_info->slice_info[orig_slice_id]),
                    sizeof(bcmptm_rm_tcam_slice_info_t));
            sal_memcpy(&(shared_slice_info[slice_idx]), &(shared_more_info->slice_info[orig_slice_id]),
                    sizeof(bcmptm_rm_tcam_slice_info_t));
            slice_info[slice_idx].slice_id = slice_idx;
            shared_slice_info[slice_idx].slice_id = slice_idx;
            slice_info[slice_idx].start_index = next_start_idx;
            if (num_entries > 0) {
                slice_info[slice_idx].end_index =
                    slice_info[slice_idx].start_index + num_entries - 1;
            } else {
                slice_info[slice_idx].end_index = 0;
            }
            for (pipe = 0; pipe < more_info->pipe_count; pipe++) {
                entry_count_array[pipe][slice_idx] = num_entries;
                shared_entry_count_array[pipe][slice_idx] = presel_num_entries;
            }
            next_start_idx = slice_info[slice_idx].start_index + num_entries;
            slice_idx++;
            num_entries = 0;
            presel_num_entries = 0;
        }

        if (iter != num_rows && new_hw_entry_info[0].sid[iter] != NULL) {
            num_entries += bcmdrd_pt_index_max(unit,
                                  *(new_hw_entry_info[0].sid[iter])) + 1;
            presel_num_entries += bcmdrd_pt_index_max(unit,
                                  *(new_shared_hw_entry_info[0].sid[iter])) + 1;

        }
    }

    /* Update entry_count, slice_count in more_info */
    for (pipe = 0; pipe < more_info->pipe_count; pipe++) {
        entry_cnt = NULL;
        alloc_sz = (num_slices * sizeof(uint32_t));
        SHR_ALLOC(entry_cnt, alloc_sz, "bcmptmRmEntryCount");
        sal_memcpy(entry_cnt, entry_count_array[pipe], alloc_sz);
        entry_count_arr[pipe] = entry_cnt;

        /* Shared entry count */
        entry_cnt = NULL;
        alloc_sz = (num_slices * sizeof(uint32_t));
        SHR_ALLOC(entry_cnt, alloc_sz, "bcmptmRmSharedEntryCount");
        sal_memcpy(entry_cnt, shared_entry_count_array[pipe], alloc_sz);
        shared_entry_count_arr[pipe] = entry_cnt;

        slice_cnt_arr[pipe] = num_slices;
        shared_slice_cnt_arr[pipe] = num_slices;
    }

    new_more_info->entry_count = entry_count_arr;
    new_more_info->slice_count = slice_cnt_arr;
    new_more_info->slice_info = slice_info;
    new_more_info->tile_count = more_info->tile_count;

    new_shared_more_info->entry_count = shared_entry_count_arr;
    new_shared_more_info->slice_count = shared_slice_cnt_arr;
    new_shared_more_info->slice_info = shared_slice_info;

    sal_memcpy(tile_info, more_info->tile_info,
                    more_info->tile_count * sizeof(bcmptm_rm_tcam_tile_info_t));

    new_more_info->tile_info = tile_info;

exit:
    for (i = 0; i <  BCMPTM_RM_TCAM_MAX_DEPTH_INSTANCES; i++) {
        if (tcam_sid != NULL && tcam_sid[i] != NULL) {
            SHR_FREE(tcam_sid[i]);
        }
        if (data_sid != NULL && data_sid[i] != NULL) {
            SHR_FREE(data_sid[i]);
        }
        if (presel_tcam_sid != NULL && presel_tcam_sid[i] != NULL) {
            SHR_FREE(presel_tcam_sid[i]);
        }
    }
    SHR_FREE(tcam_sid);
    SHR_FREE(data_sid);
    SHR_FREE(presel_tcam_sid);

    if (SHR_FUNC_ERR()) {
        for (iter = 0; iter < orig_lrd_info->hw_entry_info_count; iter++) {
            if (new_hw_entry_info != NULL) {
                for (num_rows = 0;
                     num_rows < new_hw_entry_info[iter].num_depth_inst;
                     num_rows++) {
                    SHR_FREE(new_hw_entry_info[iter].sid[num_rows]);
                    SHR_FREE(new_hw_entry_info[iter].sid_data_only[num_rows]);
                    SHR_FREE(new_shared_hw_entry_info[iter].sid[num_rows]);
                    SHR_FREE(new_shared_hw_entry_info[iter].sid_data_only[num_rows]);
                }
                new_hw_entry_info[iter].sid = NULL;
                new_hw_entry_info[iter].sid_data_only = NULL;
                new_hw_entry_info[iter].num_depth_inst = 0;
            }
        }
        for (i = 0; i < more_info->pipe_count; i++) {
            if (entry_count_arr != NULL) {
                SHR_FREE(entry_count_arr[i]);
                SHR_FREE(shared_entry_count_arr[i]);
            }
        }
        if (new_more_info != NULL) {
            new_more_info->entry_count = NULL;
            new_more_info->presel_count = NULL;
            new_more_info->slice_count = NULL;
            new_more_info->slice_info = NULL;
            new_more_info->tile_info = NULL;
        }
        SHR_FREE(sid_arr);
        SHR_FREE(sid_data_arr);
        SHR_FREE(entry_count_arr);
        SHR_FREE(slice_cnt_arr);
        SHR_FREE(slice_info);
        SHR_FREE(tile_info);
        SHR_FREE(presel_sid_arr);
        SHR_FREE(shared_entry_count_arr);
        SHR_FREE(shared_slice_cnt_arr);
        SHR_FREE(shared_slice_info);
    }
    /*
     * Memory allocated by this function is freed when banks are deallocated.
     * Freeing is done by bcmptm_rm_tcam_dyn_lt_hw_info_cleanup().
     */
    /* coverity[leaked_storage] */
    SHR_FUNC_EXIT();

}

static int
bcm56780_a0_rm_tcam_dyn_lt_hw_info_init(int unit,
                                        uint32_t trans_id,
                                        bcmltd_sid_t ltid,
                                        lt_mreq_info_t *lrd_info,
                                        bool warm,
                                        uint8_t *is_change)
{
    int rv = 0;
    bcmltd_sid_t main_ltid = 0;
    uint32_t idx = 0, i = 0, bnk_idx = 0, alloc_sz = 0, num_banks = 0;
    bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *more_info = NULL;
    const bcmdrd_sid_t **sid_arr = NULL;
    const bcmdrd_sid_t **sid_data_arr = NULL;
    const uint32_t **entry_count_arr = NULL;
    uint32_t *entry_cnt = NULL;
    uint8_t *slice_cnt_arr = NULL;
    bcmptm_rm_tcam_slice_info_t *slice_info = NULL;
    bcmdrd_sid_t bank_sid = 0;
    bcmdrd_sid_t banks_info[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES] = {0};
    bcmdrd_sid_t *sid = NULL;
    bcmdrd_sid_t lts_tcam_sid = 0;
    bcmdrd_sid_t tcam_sid[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES] = {0};
    bcmdrd_sid_t data_sid[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES] = {0};
    bcmptm_pt_banks_info_t  pt_banks_info;
    uint8_t is_bank_used[UFT_VAL_BANKS_MAX] = {0};
    uint8_t bank_not_present[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES] = {0};
    uint8_t banks_cnt = 0;
    uint8_t total_banks_cnt = 0;
    uint8_t index = 0;
    uint8_t num_key_rows = 0;
    uint8_t num_data_rows = 0;
    uint8_t entries_present = 0;
    uint8_t is_ifta80_bank = 0;
    bcmptm_lt_lookup_banks_info_t banks_lookup0_info;
    bcmptm_lt_lookup_banks_info_t banks_lookup1_info;
    const lt_mreq_info_t *orig_lrd_info = NULL;
    bcmdrd_sid_t *ptsid_arr = NULL;
    uint32_t ptsid_arr_cnt = 0, r_cnt = 0;
    bcmptm_rm_tcam_hw_entry_info_t *orig_hw_info = NULL;
    uint8_t multi_aggr_no_data_sid = 0;


    SHR_FUNC_ENTER(unit);

    hw_entry_info = (bcmptm_rm_tcam_hw_entry_info_t *)lrd_info->hw_entry_info;
    more_info = (bcmptm_rm_tcam_more_info_t *)lrd_info->rm_more_info;
    SHR_NULL_CHECK(hw_entry_info, SHR_E_FAIL);
    SHR_NULL_CHECK(more_info, SHR_E_FAIL);

    main_ltid = more_info->main_lt_info[0];

    sal_memset(&pt_banks_info, 0, sizeof(pt_banks_info));
    sal_memset(&banks_lookup0_info, 0, sizeof(bcmptm_lt_lookup_banks_info_t));
    sal_memset(&banks_lookup1_info, 0, sizeof(bcmptm_lt_lookup_banks_info_t));

    rv = bcmptm_cth_uft_bank_info_get_from_lt(unit,
                                              main_ltid,
                                              0,
                                              &pt_banks_info);
    if (rv != SHR_E_NONE || pt_banks_info.bank_cnt == 0) {

        if (lrd_info->pt_type != LT_PT_TYPE_FP &&
            hw_entry_info->num_depth_inst != 0 &&
            hw_entry_info->sid[0] != NULL) {
            /*
             * There are non zero banks previously and in this update,
             * 0 banks are received for this LT.If it is ifta80 tile bank,
             * update acc_modes
             */
             /* check if any of the bank is ifta80 bank */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_check_ifta80_bank(unit,
                                          hw_entry_info->sid[0][0],
                                          &is_ifta80_bank));
        }
        /* Free previously allocated memory. */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmptm_rm_tcam_dyn_lt_hw_info_cleanup(unit, lrd_info));

        /* Do not update ACC_MODESr during warm boot */
        if (warm == false && is_ifta80_bank == 1) {
            /* Need to update acc modes as per new/updates banks */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_rm_tcam_defip_acc_modes_update(unit,
                                                    trans_id,
                                                    main_ltid));
        }
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_lt_mreq_info_get(unit,
                                 ltid,
                                 &orig_lrd_info));

    SHR_NULL_CHECK(orig_lrd_info, SHR_E_FAIL);

    orig_hw_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                   (orig_lrd_info->hw_entry_info);

    total_banks_cnt = pt_banks_info.bank_cnt + pt_banks_info.lookup1_bank_cnt;
    if (lrd_info->pt_type == LT_PT_TYPE_FP) {

        /* Validate the new banks against the existing banks. */
        if (more_info->slice_count != NULL &&
            more_info->slice_info != NULL) {

            /* All EM banks are paired banks */
            if (total_banks_cnt < more_info->slice_count[0]) {

                /* if number of entires is 0, we can allow reduction of banks. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_entries_exist(unit,
                                                     ltid,
                                                     &entries_present));
                if (entries_present != 0) {
                /* Banks cannot be deallocatd while in use. */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Banks being deallocated while in use.\n"
                                     )));
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }

            if (total_banks_cnt == more_info->slice_count[0]) {
                /* no change in bank assignment. */
                SHR_EXIT();
            }
        }
    } else {
        /* Validate the new banks against the existing banks. */
        if (more_info->slice_count != NULL &&
            total_banks_cnt == more_info->slice_count[0]) {
            /* no change in bank assignment. */
            SHR_EXIT();
        }
    }

    /* Free previously allocated memory. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_dyn_lt_hw_info_cleanup(unit, lrd_info));

    if (lrd_info->pt_type == LT_PT_TYPE_FP &&
        lrd_info->dynamic_banks == 1 &&
        more_info->flags & BCMPTM_RM_TCAM_UFT_T4T_BANKS) {
        /* This is t4t tile banks for IFP/VFP/EFP */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_dyn_lt_hw_info_init(unit,
                                              orig_lrd_info,
                                              lrd_info,
                                              &pt_banks_info));
        SHR_EXIT();
    }

    num_banks = pt_banks_info.bank_cnt;

    if (lrd_info->pt_type == LT_PT_TYPE_FP) {
        SHR_IF_ERR_EXIT
            (bcmptm_cth_uft_lt_lts_tcam_get(unit, main_ltid,
                                            NULL, 0, &r_cnt));
        if (r_cnt == 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        ptsid_arr_cnt = r_cnt;
        SHR_ALLOC(ptsid_arr,
                  ptsid_arr_cnt * sizeof(bcmdrd_sid_t),
                  "bcmptmLtsArray");
        SHR_NULL_CHECK(ptsid_arr, SHR_E_MEMORY);
        sal_memset(ptsid_arr, 0, (ptsid_arr_cnt * sizeof(bcmdrd_sid_t)));

        SHR_IF_ERR_EXIT
            (bcmptm_cth_uft_lt_lts_tcam_get(unit, main_ltid,
                                            ptsid_arr, ptsid_arr_cnt,
                                            &r_cnt));
        num_banks = r_cnt;
    }

    if (more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
        alloc_sz = (2 * num_banks * sizeof(bcmdrd_sid_t *));
    } else {
        alloc_sz = (num_banks * sizeof(bcmdrd_sid_t *));
    }
    SHR_ALLOC(sid_arr, alloc_sz, "bcmptmRmTcamSidArr");
    sal_memset(sid_arr, 0, alloc_sz);

    alloc_sz = (more_info->pipe_count * sizeof(uint8_t));
    SHR_ALLOC(slice_cnt_arr, alloc_sz, "bcmptmRmSliceCountArr");
    sal_memset(slice_cnt_arr, 0, alloc_sz);

    /* LTS info for EM_FT & EM_FT. LTS is TLB so no data_only */
    if (lrd_info->pt_type == LT_PT_TYPE_FP) {
        alloc_sz = (more_info->pipe_count * sizeof(uint32_t *));
        SHR_ALLOC(entry_count_arr, alloc_sz, "bcmptmRmPreselCountArr");
        sal_memset(entry_count_arr, 0, alloc_sz);

        alloc_sz = (num_banks * sizeof(bcmptm_rm_tcam_slice_info_t));
        SHR_ALLOC(slice_info, alloc_sz, "bcmptmRmSliceInfoArr");
        sal_memset(slice_info, 0, alloc_sz);

        for (idx = 0; idx < num_banks; idx++) {
            lts_tcam_sid = ptsid_arr[idx];
            sid = NULL;
            alloc_sz = (1 * sizeof(bcmdrd_sid_t));
            SHR_ALLOC(sid, alloc_sz, "bcmptmRmTcamSid");
            sal_memset(sid, 0, alloc_sz);
            sid[0] = lts_tcam_sid;
            sid_arr[idx] = sid;

            slice_info[idx].slice_id = idx;
            slice_info[idx].valid = 1;
            slice_info[idx].start_index =
                bcmdrd_pt_index_min(unit, lts_tcam_sid);
            slice_info[idx].end_index =
                bcmdrd_pt_index_max(unit, lts_tcam_sid);
        }

        for (idx = 0; idx < more_info->pipe_count; idx++) {
            entry_cnt = NULL;
            alloc_sz = (num_banks * sizeof(uint32_t));
            SHR_ALLOC(entry_cnt, alloc_sz, "bcmptmRmPreselCount");
            sal_memset(entry_cnt, 0, alloc_sz);
            for (bnk_idx = 0; bnk_idx < num_banks; bnk_idx++) {
                lts_tcam_sid = ptsid_arr[bnk_idx];
                entry_cnt[bnk_idx] =
                    (bcmdrd_pt_index_max(unit, lts_tcam_sid) -
                     bcmdrd_pt_index_min(unit, lts_tcam_sid) + 1);
            }
            entry_count_arr[idx] = entry_cnt;
            slice_cnt_arr[idx] = num_banks;
        }
        hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_SINGLE].sid = sid_arr;
        hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_SINGLE].sid_data_only = sid_data_arr;
        hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_DBLINTER].sid = sid_arr;
        hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_DBLINTER].sid_data_only = sid_data_arr;
        hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_QUAD].sid = sid_arr;
        hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_QUAD].sid_data_only = sid_data_arr;
        more_info->entry_count = entry_count_arr;
        more_info->presel_count = entry_count_arr;
        more_info->slice_count = slice_cnt_arr;
        more_info->slice_info = slice_info;

        hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_SINGLE].num_depth_inst =
            num_banks;
        hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_DBLINTER].num_depth_inst =
            num_banks;
        hw_entry_info[BCMPTM_RM_TCAM_GRP_MODE_QUAD].num_depth_inst =
            num_banks;

    } else {
        *is_change = 1;
        banks_cnt = 0;
        slice_cnt_arr[0] = total_banks_cnt;
        num_key_rows = orig_hw_info[0].num_key_rows;
        num_data_rows =  orig_hw_info[0].num_data_rows;
        if (more_info->urpf_enabled == 1) {
            num_key_rows = num_key_rows / 2;
            num_data_rows =  num_data_rows / 2;
        }

        /* Aggr LT */
        if (lrd_info->aggr_ekw_edw > 0) {
            multi_aggr_no_data_sid = 1;
        } else {
            /* Non aggr View LT has both KEY and DATA only */

            if (more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
                alloc_sz = (2 * num_banks * sizeof(bcmdrd_sid_t *));
            } else {
                alloc_sz = (num_banks * sizeof(bcmdrd_sid_t *));
            }

            SHR_ALLOC(sid_data_arr, alloc_sz, "bcmptmRmDataSidArr");
            sal_memset(sid_data_arr, 0, alloc_sz);
        }

        index = 0;
        for (idx = 0; idx < num_banks; idx++) {

            if (is_bank_used[idx] == 1) {
                continue;
            }
            bank_sid = pt_banks_info.bank[idx].bank_sid;
            if (num_key_rows == 2) {
                if (pt_banks_info.lookup1_bank_cnt != 0) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcm56780_a0_rm_tcam_dbl_urpf_sid_info_get(unit,
                                                  bank_sid,
                                                  banks_info,
                                                  tcam_sid,
                                                  data_sid,
                                                  multi_aggr_no_data_sid));
                } else {
                    /* double wide mode */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcm56780_a0_rm_tcam_dbl_sid_info_get(unit,
                                                  bank_sid,
                                                  banks_info,
                                                  tcam_sid,
                                                  data_sid,
                                                  multi_aggr_no_data_sid));
                }
            } else if (pt_banks_info.lookup1_bank_cnt != 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm56780_a0_rm_tcam_urpf_paired_bank_get(unit,
                                                  bank_sid,
                                                  banks_info,
                                                  tcam_sid,
                                                  data_sid,
                                                  multi_aggr_no_data_sid));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm56780_a0_rm_tcam_sid_info_get(unit,
                                                  bank_sid,
                                                  banks_info,
                                                  tcam_sid,
                                                  data_sid,
                                                  multi_aggr_no_data_sid));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (check_banks_and_mark_used(unit,
                                         &pt_banks_info,
                                         banks_info,
                                         bank_not_present,
                                         is_bank_used));

            for (i = 0; i < BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES; i++) {
                if (bank_not_present[i] == 1) {
                    /* Remove corresponding tcam_sid and data_sid */
                    tcam_sid[i] = INVALIDm;
                    data_sid[i] = INVALIDm;
                }
            }
            sal_memset(&bank_not_present, 0,
                        sizeof(uint8_t) * BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES);

            sid = NULL;
            alloc_sz = (orig_hw_info[0].num_key_rows * sizeof(bcmdrd_sid_t));
            SHR_ALLOC(sid, alloc_sz, "bcmptmRmTcamSid");
            sal_memset(sid, 0, alloc_sz);
            sal_memcpy(sid, tcam_sid, alloc_sz);
            sid_arr[index] = sid;

            if (more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
                sid = NULL;
                SHR_ALLOC(sid, alloc_sz, "bcmptmRmTcamSid2");
                sal_memset(sid, 0, alloc_sz);
                sal_memcpy(sid, tcam_sid, alloc_sz);
                sid_arr[index+1] = sid;
            }

            if (multi_aggr_no_data_sid == 0) {
                sid = NULL;
                alloc_sz = (orig_hw_info[0].num_data_rows * sizeof(bcmdrd_sid_t));
                SHR_ALLOC(sid, alloc_sz, "bcmptmRmDataSid");
                sal_memset(sid, 0, alloc_sz);
                sal_memcpy(sid, data_sid, alloc_sz);
                sid_data_arr[index] = sid;
            }

            if (more_info->flags & BCMPTM_RM_TCAM_HALF_MODE) {
                if (multi_aggr_no_data_sid == 0) {
                    sid = NULL;
                    SHR_ALLOC(sid, alloc_sz, "bcmptmRmDataSid2");
                    sal_memset(sid, 0, alloc_sz);
                    sal_memcpy(sid, data_sid, alloc_sz);
                    sid_data_arr[index+1] = sid;
                }
                banks_cnt += 2;
                index += 2;
            } else {
                banks_cnt++;
                index++;
            }
        }

        hw_entry_info[0].sid = sid_arr;
        hw_entry_info[0].sid_data_only = sid_data_arr;
        hw_entry_info[0].num_depth_inst = banks_cnt;
        more_info->slice_count = slice_cnt_arr;
        if (pt_banks_info.lookup1_bank_cnt != 0) {
            more_info->flags |= BCMPTM_RM_TCAM_URPF_ENABLED_LT;
            hw_entry_info[0].num_key_rows = orig_hw_info[0].num_key_rows;
            hw_entry_info[0].num_data_rows = orig_hw_info[0].num_data_rows;
            lrd_info->num_ekw = orig_lrd_info->num_ekw;
            lrd_info->num_edw = orig_lrd_info->num_edw;
        } else {
            if (more_info->urpf_enabled == 1) {
                hw_entry_info[0].num_key_rows = orig_hw_info[0].num_key_rows/2;
                hw_entry_info[0].num_data_rows = orig_hw_info[0].num_data_rows/2;
                lrd_info->num_ekw = orig_lrd_info->num_ekw/2;
                lrd_info->num_edw = orig_lrd_info->num_edw/2;
            }
            more_info->flags &= ~BCMPTM_RM_TCAM_URPF_ENABLED_LT;
        }

        /* LTPR programming */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_rm_tcam_defip_program_ltpr_table(unit,
                                                    trans_id,
                                                    main_ltid,
                                                    &hw_entry_info[0],
                                                    &pt_banks_info,
                                                    warm));
    }
exit:
    if (SHR_FUNC_ERR()) {
        if (sid_arr != NULL) {
            for (idx = 0; idx < num_banks; idx++) {
                SHR_FREE(sid_arr[idx]);
            }
        }
        if (sid_data_arr != NULL) {
            for (idx = 0; idx < num_banks; idx++) {
                SHR_FREE(sid_data_arr[idx]);
            }
        }
        if (hw_entry_info != NULL) {
            hw_entry_info[0].sid = NULL;
            hw_entry_info[0].sid_data_only = NULL;
            hw_entry_info[0].num_depth_inst = 0;
        }
        if (more_info != NULL) {
            for (idx = 0; idx < more_info->pipe_count; idx++) {
                if (entry_count_arr != NULL) {
                    SHR_FREE(entry_count_arr[idx]);
                }
            }
            more_info->entry_count = NULL;
            more_info->presel_count = NULL;
            more_info->slice_count = NULL;
            more_info->slice_info = NULL;
        }
        SHR_FREE(sid_arr);
        SHR_FREE(sid_data_arr);
        SHR_FREE(entry_count_arr);
        SHR_FREE(slice_cnt_arr);
        SHR_FREE(slice_info);
    }
    SHR_FREE(ptsid_arr);
    /*
     * Memory allocated by this function is freed when banks are deallocated.
     * Freeing is done by bcmptm_rm_tcam_dyn_lt_hw_info_cleanup().
     */
    /* coverity[leaked_storage] */
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_rm_tcam_dyn_shared_lt_mreq_info_init(int unit,
                                           bcmltd_sid_t ltid)
{
    uint32_t alloc_sz = 0, idx = 0;
    const lt_mreq_info_t *orig_lrd_info = NULL;
    lt_mreq_info_t *new_lrd_info = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *more_info = NULL;
    bcmdrd_sid_t *main_lt_arr = NULL;
    bcmdrd_sid_t *shared_lt_arr = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *orig_hw_info = NULL;
    bcmptm_rm_tcam_more_info_t *orig_more_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_lt_mreq_info_get(unit,
                                 ltid,
                                 &orig_lrd_info));
    SHR_NULL_CHECK(orig_lrd_info, SHR_E_FAIL);

    orig_hw_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                   (orig_lrd_info->hw_entry_info);
    orig_more_info = (bcmptm_rm_tcam_more_info_t *)
                     (orig_lrd_info->rm_more_info);

    alloc_sz = sizeof(lt_mreq_info_t);
    SHR_ALLOC(new_lrd_info, alloc_sz, "bcmptmLtMreqInfo");
    sal_memset(new_lrd_info, 0, alloc_sz);
    sal_memcpy(new_lrd_info, orig_lrd_info, alloc_sz);

    alloc_sz = (orig_lrd_info->hw_entry_info_count *
                sizeof(bcmptm_rm_tcam_hw_entry_info_t));
    SHR_ALLOC(hw_entry_info, alloc_sz, "bcmptmLtHwEntryInfo");
    sal_memset(hw_entry_info, 0, alloc_sz);
    sal_memcpy(hw_entry_info, orig_hw_info, alloc_sz);

    for (idx = 0; idx < orig_lrd_info->hw_entry_info_count; idx++) {
        hw_entry_info[idx].sid = NULL;
        hw_entry_info[idx].sid_data_only = NULL;
        hw_entry_info[idx].num_depth_inst = 0;
    }

    alloc_sz = (1 * sizeof(bcmdrd_sid_t));
    SHR_ALLOC(main_lt_arr, alloc_sz, "bcmptmMainLtArr");
    sal_memset(main_lt_arr, 0, alloc_sz);
    if (orig_more_info->main_lt_info != NULL) {
        sal_memcpy(main_lt_arr, orig_more_info->main_lt_info, alloc_sz);
    } else {
        sal_memcpy(main_lt_arr, &ltid, alloc_sz);
    }

    if (orig_more_info->shared_lt_count) {
        alloc_sz = (orig_more_info->shared_lt_count * sizeof(bcmdrd_sid_t));
        SHR_ALLOC(shared_lt_arr, alloc_sz, "bcmptmSharedLtArr");
        sal_memset(shared_lt_arr, 0, alloc_sz);
        sal_memcpy(shared_lt_arr, orig_more_info->shared_lt_info, alloc_sz);
    }

    alloc_sz = (orig_lrd_info->rm_more_info_count * sizeof(bcmptm_rm_tcam_more_info_t));
    SHR_ALLOC(more_info, alloc_sz, "bcmptmLtMoreInfo");
    sal_memset(more_info, 0, alloc_sz);
    sal_memcpy(more_info, orig_lrd_info->rm_more_info, alloc_sz);

    for (idx = 0; idx < orig_lrd_info->rm_more_info_count; idx++) {
        if (orig_lrd_info->pt_type == LT_PT_TYPE_FP) {
            more_info[idx].slice_info = NULL;
            more_info[idx].tile_info = NULL;
            more_info[idx].entry_count = NULL;
            more_info[idx].slice_count = NULL;
        }
        more_info[idx].main_lt_info = main_lt_arr;
        if (orig_more_info->shared_lt_count) {
            more_info[idx].shared_lt_info = shared_lt_arr;

        }
    }

    more_info->flags |= BCMPTM_RM_TCAM_DYNAMIC_MANAGEMENT;
    new_lrd_info->hw_entry_info = hw_entry_info;
    new_lrd_info->rm_more_info = more_info;

    /* Update the LT handler in dynamic PTRM table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_dyn_ptrm_tbl_update(unit,
                                            ltid,
                                            new_lrd_info));

exit:
    if (SHR_FUNC_ERR()) {
            SHR_FREE(shared_lt_arr);
            SHR_FREE(main_lt_arr);
            SHR_FREE(hw_entry_info);
            SHR_FREE(more_info);
            SHR_FREE(new_lrd_info);
    }
    /*
     * Memory allocated by this function is freed during shutdown
     * by bcmptm_rm_tcam_dyn_lt_mreq_info_cleanup()
     */
    /* coverity[leaked_storage] */
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
int
bcm56780_a0_rm_tcam_dynamic_lt_validate(int unit,
                                           bcmltd_sid_t ltid)
{
    int rv = 0;
    uint32_t idx = 0, num_banks = 0;
    bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *more_info = NULL;
    bcmdrd_sid_t bank_sid = 0;
    bcmdrd_sid_t banks_info[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES] = {0};
    bcmdrd_sid_t tcam_sid[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES] = {0};
    bcmdrd_sid_t data_sid[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES] = {0};
    uint8_t is_bank_used[UFT_VAL_BANKS_MAX] = {0};
    uint8_t bank_not_present[BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES] = {0};
    uint8_t num_key_rows = 0;
    uint8_t num_data_rows = 0;
    lt_mreq_info_t *lrd_info = NULL;
    bcmptm_pt_banks_info_t  pt_banks_info;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    uint8_t total_banks_cnt = 0;
    const lt_mreq_info_t *orig_lrd_info = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *orig_hw_info = NULL;
    uint8_t multi_aggr_no_data_sid = 0;

    SHR_FUNC_ENTER(unit);


    sal_memset(&pt_banks_info, 0, sizeof(pt_banks_info));

    rv = bcmptm_cth_uft_bank_info_get_from_lt(unit,
                                              ltid,
                                              0,
                                              &pt_banks_info);
    if (rv != SHR_E_NONE) {
        SHR_EXIT();
    }

    total_banks_cnt = pt_banks_info.bank_cnt + pt_banks_info.lookup1_bank_cnt;
    num_banks = pt_banks_info.bank_cnt;

    rv = bcmptm_rm_tcam_dyn_ptrm_tbl_get(unit,
                                         ltid,
                                         &lrd_info);
    if (rv != SHR_E_NONE) {
        SHR_EXIT();
    }

    hw_entry_info = (bcmptm_rm_tcam_hw_entry_info_t *)lrd_info->hw_entry_info;
    more_info = (bcmptm_rm_tcam_more_info_t *)lrd_info->rm_more_info;

    SHR_NULL_CHECK(hw_entry_info, SHR_E_FAIL);
    SHR_NULL_CHECK(more_info, SHR_E_FAIL);

    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit, ltid, 0, 0,
                                      ((void *)&ltid_info),
                                      NULL, NULL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_lt_mreq_info_get(unit,
                                 ltid,
                                 &orig_lrd_info));

    SHR_NULL_CHECK(orig_lrd_info, SHR_E_FAIL);

    orig_hw_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                   (orig_lrd_info->hw_entry_info);


    if ((more_info->slice_count != NULL
            && total_banks_cnt < more_info->slice_count[0])
        || (pt_banks_info.bank_cnt == 0)) {
        /* check if there are no valid entires in the tcam */
        SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                        ltid,
                                                        &ltid_info,
                                                        0,
                                                        &tcam_info));
        if (tcam_info->num_entries_ltid != 0) {
            /* One or more banks deallocated, log error and return. */
            LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                 "Banks being deallocated while in use.\n"
                                     )));
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

    num_key_rows = orig_hw_info[0].num_key_rows;
    num_data_rows = orig_hw_info[0].num_data_rows;

    if (more_info->urpf_enabled == 1) {
        num_key_rows = num_key_rows / 2;
        num_data_rows =  num_data_rows / 2;
    }

    /* Aggr LT */
    if (ltid_info.non_aggr == 0) {
        multi_aggr_no_data_sid = 1;
    }

    for (idx = 0; idx < num_banks; idx++) {

        if (is_bank_used[idx] == 1) {
            continue;
        }
        bank_sid = pt_banks_info.bank[idx].bank_sid;

        if (num_key_rows == 2) {
            if (pt_banks_info.lookup1_bank_cnt != 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm56780_a0_rm_tcam_dbl_urpf_sid_info_get(unit,
                                              bank_sid,
                                              banks_info,
                                              tcam_sid,
                                              data_sid,
                                              multi_aggr_no_data_sid));
            } else {
                /* double wide mode */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm56780_a0_rm_tcam_dbl_sid_info_get(unit,
                                              bank_sid,
                                              banks_info,
                                              tcam_sid,
                                              data_sid,
                                              multi_aggr_no_data_sid));
            }
        } else if (pt_banks_info.lookup1_bank_cnt != 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_rm_tcam_urpf_paired_bank_get(unit,
                                              bank_sid,
                                              banks_info,
                                              tcam_sid,
                                              data_sid,
                                              multi_aggr_no_data_sid));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_rm_tcam_sid_info_get(unit,
                                              bank_sid,
                                              banks_info,
                                              tcam_sid,
                                              data_sid,
                                              multi_aggr_no_data_sid));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (check_banks_and_mark_used(unit,
                                     &pt_banks_info,
                                     banks_info,
                                     bank_not_present,
                                     is_bank_used));

    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_rm_tcam_dyn_lt_mreq_info_init(int unit,
                                          uint32_t trans_id,
                                          bcmltd_sid_t ltid,
                                          bool warm)
{
    int rv = 0;
    uint32_t alloc_sz = 0, idx = 0;
    const lt_mreq_info_t *orig_lrd_info = NULL;
    lt_mreq_info_t *lrd_info = NULL;
    lt_mreq_info_t *new_lrd_info = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *more_info = NULL;
    bool lt_first_init = false;
    bcmdrd_sid_t *main_lt_arr = NULL;
    bcmdrd_sid_t *shared_lt_arr = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *orig_hw_info = NULL;
    bcmptm_rm_tcam_more_info_t *orig_more_info = NULL;
    uint8_t is_ifta80_bank = 0;
    uint8_t is_change = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_rm_tcam_dyn_ptrm_tbl_get(unit,
                                         ltid,
                                         &lrd_info);
    if (rv == SHR_E_NONE) {
        lt_first_init = false;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_rm_tcam_dyn_lt_hw_info_init(unit,
                                                     trans_id,
                                                     ltid,
                                                     lrd_info,
                                                     warm,
                                                     &is_change));
        hw_entry_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                       (lrd_info->hw_entry_info);
    } else {
        lt_first_init = true;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_lt_mreq_info_get(unit,
                                     ltid,
                                     &orig_lrd_info));
        SHR_NULL_CHECK(orig_lrd_info, SHR_E_FAIL);

        orig_hw_info = (bcmptm_rm_tcam_hw_entry_info_t *)
                       (orig_lrd_info->hw_entry_info);
        orig_more_info = (bcmptm_rm_tcam_more_info_t *)
                         (orig_lrd_info->rm_more_info);

        alloc_sz = sizeof(lt_mreq_info_t);
        SHR_ALLOC(new_lrd_info, alloc_sz, "bcmptmLtMreqInfo");
        sal_memset(new_lrd_info, 0, alloc_sz);
        sal_memcpy(new_lrd_info, orig_lrd_info, alloc_sz);

        alloc_sz = (orig_lrd_info->hw_entry_info_count *
                    sizeof(bcmptm_rm_tcam_hw_entry_info_t));
        SHR_ALLOC(hw_entry_info, alloc_sz, "bcmptmLtHwEntryInfo");
        sal_memset(hw_entry_info, 0, alloc_sz);
        sal_memcpy(hw_entry_info, orig_hw_info, alloc_sz);

        for (idx = 0; idx < orig_lrd_info->hw_entry_info_count; idx++) {
            hw_entry_info[idx].sid = NULL;
            hw_entry_info[idx].sid_data_only = NULL;
            hw_entry_info[idx].num_depth_inst = 0;
        }

        alloc_sz = (1 * sizeof(bcmdrd_sid_t));
        SHR_ALLOC(main_lt_arr, alloc_sz, "bcmptmMainLtArr");
        sal_memset(main_lt_arr, 0, alloc_sz);
        if (orig_more_info->main_lt_info != NULL) {
            sal_memcpy(main_lt_arr, orig_more_info->main_lt_info, alloc_sz);
        } else {
            sal_memcpy(main_lt_arr, &ltid, alloc_sz);
        }

        if (orig_more_info->shared_lt_count) {
            alloc_sz = (orig_more_info->shared_lt_count * sizeof(bcmdrd_sid_t));
            SHR_ALLOC(shared_lt_arr, alloc_sz, "bcmptmSharedLtArr");
            sal_memset(shared_lt_arr, 0, alloc_sz);
            sal_memcpy(shared_lt_arr, orig_more_info->shared_lt_info, alloc_sz);
        }

        alloc_sz = (orig_lrd_info->rm_more_info_count * sizeof(bcmptm_rm_tcam_more_info_t));
        SHR_ALLOC(more_info, alloc_sz, "bcmptmLtMoreInfo");
        sal_memset(more_info, 0, alloc_sz);
        sal_memcpy(more_info, orig_lrd_info->rm_more_info, alloc_sz);

        for (idx = 0; idx < orig_lrd_info->rm_more_info_count; idx++) {
            if (orig_lrd_info->pt_type == LT_PT_TYPE_FP) {
                more_info[idx].slice_info = NULL;
                more_info[idx].tile_info = NULL;
                more_info[idx].entry_count = NULL;
                more_info[idx].slice_count = NULL;
            }
            more_info[idx].main_lt_info = main_lt_arr;
            if (orig_more_info->shared_lt_count) {
                more_info[idx].shared_lt_info = shared_lt_arr;

            }
        }

        more_info->flags |= BCMPTM_RM_TCAM_DYNAMIC_MANAGEMENT;
        new_lrd_info->hw_entry_info = hw_entry_info;
        new_lrd_info->rm_more_info = more_info;

        /*
         * Check whether it has a shared lt. Only for
         * IFP/VFP/EFP, update shared LT lrd_info.
         */
        if (orig_lrd_info->pt_type == LT_PT_TYPE_FP
           && orig_lrd_info->dynamic_banks == 1
           && orig_more_info->shared_lt_count >= 1
           && more_info->flags & BCMPTM_RM_TCAM_UFT_T4T_BANKS) {

            /* Initialize shared mreq info */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_rm_tcam_dyn_shared_lt_mreq_info_init(unit,
                                           orig_more_info->shared_lt_info[0]));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_rm_tcam_dyn_lt_hw_info_init(unit,
                                                     trans_id,
                                                     ltid,
                                                     new_lrd_info,
                                                     warm,
                                                     &is_change));

        /* Update the LT handler in dynamic PTRM table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_dyn_ptrm_tbl_update(unit,
                                                ltid,
                                                new_lrd_info));
       /*
        * For first lt init, check if original hw info has ifta80 banks.
        * if yes, call ACC_MODESr update. during init, no banks are assigned
        * to rm_tcam, we need to set the ACC_MODESr to 0 for tcam part.
        */
        if (warm == false &&
            orig_hw_info != NULL &&
            orig_hw_info->num_depth_inst != 0 &&
            orig_hw_info->sid[0] != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_check_ifta80_bank(unit,
                                      orig_hw_info->sid[0][0],
                                      &is_ifta80_bank));
            if (is_ifta80_bank == 1) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm56780_a0_rm_tcam_defip_acc_modes_update(unit,
                                                    trans_id,
                                                    ltid));
            }
        }
    }
    /* Do ACC_MODESr update only in cold boot case. */
    if (warm == false && is_change == 1) {
        /*Check if any of the bank is ifta80 bank */
        if (hw_entry_info != NULL &&
            hw_entry_info->num_depth_inst != 0 &&
            hw_entry_info->sid[0] != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_check_ifta80_bank(unit,
                                      hw_entry_info->sid[0][0],
                                      &is_ifta80_bank));
        }
        /* Update the ACC_MODES if it is ifta80 tile */
        if (is_ifta80_bank == 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_rm_tcam_defip_acc_modes_update(unit,
                                                trans_id,
                                                ltid));
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        if (lt_first_init == true) {
            SHR_FREE(shared_lt_arr);
            SHR_FREE(main_lt_arr);
            SHR_FREE(hw_entry_info);
            SHR_FREE(more_info);
            SHR_FREE(new_lrd_info);
        }
    }
    /*
     * Memory allocated by this function is freed during shutdown
     * by bcmptm_rm_tcam_dyn_lt_mreq_info_cleanup()
     */
    /* coverity[leaked_storage] */
    SHR_FUNC_EXIT();
}

