/*! \file bcmptm_bcm56780_a0_rm_tcam.c
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
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>
#include "./bcmptm_bcm56780_a0_rm_tcam.h"
#include <bcmptm/generated/bcmptm_rm_tcam_prio_only_ha.h>
#include "../../rm_tcam/rm_tcam_prio_only.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_MISC


/*******************************************************************************
 * Private variables
 */

/*******************************************************************************
 * Public Functions
 */
static int
bcm56780_a0_rm_tcam_part_prio_sid_get(int unit,
                    bcmptm_rm_tcam_fp_stage_id_t stage_id,
                    uint8_t hw_tile_id,
                    uint8_t slice_id,
                    bcmdrd_sid_t *sid)
{
    bcmdrd_sid_t fp_ing_part_prio_sid[12] =
          {
              IFTA100_T4T_00_LTPR_PROFILE_TABLE_0m,
              IFTA100_T4T_00_LTPR_PROFILE_TABLE_1m,
              IFTA100_T4T_00_LTPR_PROFILE_TABLE_2m,
              IFTA100_T4T_00_LTPR_PROFILE_TABLE_3m,
              IFTA100_T4T_01_LTPR_PROFILE_TABLE_0m,
              IFTA100_T4T_01_LTPR_PROFILE_TABLE_1m,
              IFTA100_T4T_01_LTPR_PROFILE_TABLE_2m,
              IFTA100_T4T_01_LTPR_PROFILE_TABLE_3m,
              IFTA100_T4T_02_LTPR_PROFILE_TABLE_0m,
              IFTA100_T4T_02_LTPR_PROFILE_TABLE_1m,
              IFTA100_T4T_02_LTPR_PROFILE_TABLE_2m,
              IFTA100_T4T_02_LTPR_PROFILE_TABLE_3m,
          };

    bcmdrd_sid_t fp_vfp_part_prio_sid[8] =
          {
              IFTA40_T4T_00_LTPR_PROFILE_TABLE_0m,
              IFTA40_T4T_00_LTPR_PROFILE_TABLE_1m,
              IFTA40_T4T_00_LTPR_PROFILE_TABLE_2m,
              IFTA40_T4T_00_LTPR_PROFILE_TABLE_3m,
              IFTA40_T4T_01_LTPR_PROFILE_TABLE_0m,
              IFTA40_T4T_01_LTPR_PROFILE_TABLE_1m,
              IFTA40_T4T_01_LTPR_PROFILE_TABLE_2m,
              IFTA40_T4T_01_LTPR_PROFILE_TABLE_3m,
          };

    bcmdrd_sid_t fp_efp_part_prio_sid[4] =
          {
              EFTA30_T4T_00_LTPR_PROFILE_TABLE_0m,
              EFTA30_T4T_00_LTPR_PROFILE_TABLE_1m,
              EFTA30_T4T_00_LTPR_PROFILE_TABLE_2m,
              EFTA30_T4T_00_LTPR_PROFILE_TABLE_3m,
          };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sid, SHR_E_PARAM);

    switch (stage_id) {
        case BCMPTM_RM_TCAM_FP_STAGE_ID_INGRESS:
            *sid = fp_ing_part_prio_sid[slice_id];
            break;
        case BCMPTM_RM_TCAM_FP_STAGE_ID_EGRESS:
            *sid = fp_efp_part_prio_sid[slice_id];
            break;
        case BCMPTM_RM_TCAM_FP_STAGE_ID_LOOKUP:
            *sid = fp_vfp_part_prio_sid[(hw_tile_id * 4) + slice_id];
            break;
        case BCMPTM_RM_TCAM_FP_STAGE_ID_EXACT_MATCH:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

/* Set the partition map for each group ltid in each slice. */
static int
bcm56780_a0_rm_tcam_part_prio_update(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_sid_type_t sid_type,
                              bcmptm_rm_tcam_sid_info_t *sid_info)
{
    bcmdrd_sid_t sid = BCM56780_A0_ENUM_COUNT;
    uint32_t e_buffer[BCMPTM_MAX_PT_FIELD_WORDS] = { 0 };
    bcmbd_pt_dyn_info_t dyn_info;
    int tbl_inst = -1;
    uint8_t num_slices = 0;
    uint32_t rsp_flags = 0;
    bcmptm_rm_op_t op_type = BCMPTM_RM_OP_NORMAL;
    bcmptm_rm_tcam_slice_t *slice_fc = NULL;
    bcmptm_rm_tcam_lt_info_t *ltid_info = NULL;
    bcmptm_rm_tcam_fp_stage_id_t stage_id;
    int prev_slice_id_offset = 0;
    uint8_t slice_id = 0;
    uint8_t tslice_id = 0;
    uint8_t prev_slice_id = 0;
    uint32_t slice_prio = 0;
    uint32_t tslice_prio = 0;
    uint32_t enable = 1;
    uint8_t tile_id = 0;
    uint8_t hw_tile_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sid_info, SHR_E_PARAM);

    tbl_inst = sid_info->tbl_inst;
    ltid_info = sid_info->ltid_info;
    stage_id = sid_info->stage_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_slice_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          &num_slices,
                                          tbl_inst,
                                          stage_id,
                                          &slice_fc));

    for (slice_id = 0; slice_id < sid_info->num_slices; slice_id++) {

        /* Ignore if slice is not in the part map of ltid. */
        if (!(sid_info->lt_part_map & (1 << slice_id))) {
            continue;
        }
        sal_memset(e_buffer, 0, sizeof(e_buffer));
        slice_prio = sid_info->lt_part_pri[slice_id];
        tile_id = ltid_info->rm_more_info->slice_info[slice_id].tile_id;
        hw_tile_id = ltid_info->rm_more_info->tile_info[tile_id].hw_tile_id;
        bcmdrd_field_set(e_buffer,
                         ((hw_tile_id * 4) + (slice_id % 4)),
                         ((hw_tile_id * 4) + (slice_id % 4)),
                         &enable);
        prev_slice_id_offset =
                      slice_fc[slice_id].prev_slice_id_offset[sid_info->group_mode];
        prev_slice_id = slice_id;
        while (prev_slice_id_offset != 0) {
            tslice_id = prev_slice_id + prev_slice_id_offset;
            prev_slice_id = tslice_id;
             prev_slice_id_offset =
                slice_fc[tslice_id].prev_slice_id_offset[sid_info->group_mode];
            if (!(sid_info->lt_part_map & (1 << tslice_id))) {
                continue;
            }
            tslice_prio = sid_info->lt_part_pri[tslice_id];
            tile_id =
                ltid_info->rm_more_info->slice_info[tslice_id].tile_id;
            hw_tile_id =
                ltid_info->rm_more_info->tile_info[tile_id].hw_tile_id;
            if (slice_prio < tslice_prio) {
                 bcmdrd_field_set(e_buffer,
                                  ((hw_tile_id * 4) + (tslice_id % 4)),
                                  ((hw_tile_id * 4) + (tslice_id % 4)),
                                  &enable);
            }
        }
        dyn_info.tbl_inst = tbl_inst;
        dyn_info.index = sid_info->group_ltid[0];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_rm_tcam_part_prio_sid_get(unit,
                                                   stage_id,
                                                   hw_tile_id,
                                                   slice_id,
                                                   &sid));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_cmdproc_write(unit,
                                  0,
                                  0, 
                                  sid,
                                  &dyn_info,
                                  NULL, /* misc_info */
                                  e_buffer,
                                  1,
                                  1,
                                  1,
                                  op_type,
                                  NULL,
                                  ltid,
                                  0,
                                  &rsp_flags));
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcm56780_a0_rm_tcam_memreg_oper(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_sid_type_t sid_type,
                              bcmptm_rm_tcam_sid_oper_t sid_oper,
                              bcmptm_rm_tcam_sid_info_t *sid_info)
{

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(sid_info, SHR_E_PARAM);

    if (sid_info->oper_flags &
        BCMPTM_RM_TCAM_OPER_ACTION_PRIO_UPDATE) {
        SHR_EXIT();
    } else if (sid_info->oper_flags &
        BCMPTM_RM_TCAM_OPER_PARTITION_PRIO_UPDATE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_rm_tcam_part_prio_update(unit,
                                                  ltid,
                                                  sid_type,
                                                  sid_info));
    } else if ((sid_info->oper_flags &
                BCMPTM_RM_TCAM_OPER_SLICE_DISABLE) ||
               (sid_info->oper_flags &
                BCMPTM_RM_TCAM_OPER_SLICE_ENABLE)) {
        SHR_EXIT();
    } else if (sid_info->oper_flags &
               BCMPTM_RM_TCAM_IFP_CONFIG) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_rm_tcam_prio_only_shared_sid_compare(int unit,
                                     bcmdrd_sid_t sid,
                                     bcmdrd_sid_t shr_sid,
                                     uint32_t *neg_shr_flags,
                                     bool *compare)
{
    bcmdrd_sid_t sid_a, sid_b;
    bool check = FALSE;
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(compare, SHR_E_PARAM);

    if (bcmdrd_pt_entry_wsize(unit, sid) >
        bcmdrd_pt_entry_wsize(unit, shr_sid)) {
        sid_a = sid;
        sid_b = shr_sid;
        check = TRUE;
    } else {
        sid_a = shr_sid;
        sid_b = sid;
    }

    if (neg_shr_flags) {
        *neg_shr_flags = 0;
    }

    switch (sid_a) {
    case MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m:
        if (sid_b == MEMDB_TCAM_IFTA80_MEM0_0m ||
            sid_b == MEMDB_TCAM_IFTA80_MEM0_1m) {
            *compare = TRUE;

            if (neg_shr_flags &&
                sid_b == MEMDB_TCAM_IFTA80_MEM0_1m) {
                if (check == TRUE) {
                    *neg_shr_flags = BCMPTM_RM_TCAM_KEY0;
                } else {
                    *neg_shr_flags = BCMPTM_RM_TCAM_SHR_KEY0;
                }
            }
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM1_0_MEM1_1m:
        if (sid_b == MEMDB_TCAM_IFTA80_MEM1_0m ||
            sid_b == MEMDB_TCAM_IFTA80_MEM1_1m) {
            *compare = TRUE;

            if (neg_shr_flags &&
                sid_b == MEMDB_TCAM_IFTA80_MEM1_1m) {
                if (check == TRUE) {
                    *neg_shr_flags = BCMPTM_RM_TCAM_KEY0;
                } else {
                    *neg_shr_flags = BCMPTM_RM_TCAM_SHR_KEY0;
                }
            }
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM2_0_MEM2_1m:
        if (sid_b == MEMDB_TCAM_IFTA80_MEM2_0m ||
            sid_b == MEMDB_TCAM_IFTA80_MEM2_1m) {
            *compare = TRUE;

            if (neg_shr_flags &&
                sid_b == MEMDB_TCAM_IFTA80_MEM2_1m) {
                if (check == TRUE) {
                    *neg_shr_flags = BCMPTM_RM_TCAM_KEY0;
                } else {
                    *neg_shr_flags = BCMPTM_RM_TCAM_SHR_KEY0;
                }
            }
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM3_0_MEM3_1m:
        if (sid_b == MEMDB_TCAM_IFTA80_MEM3_0m ||
            sid_b == MEMDB_TCAM_IFTA80_MEM3_1m) {
            *compare = TRUE;

            if (neg_shr_flags &&
                sid_b == MEMDB_TCAM_IFTA80_MEM3_1m) {
                if (check == TRUE) {
                    *neg_shr_flags = BCMPTM_RM_TCAM_KEY0;
                } else {
                    *neg_shr_flags = BCMPTM_RM_TCAM_SHR_KEY0;
                }
            }
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM4_0_MEM4_1m:
        if (sid_b == MEMDB_TCAM_IFTA80_MEM4_0m ||
            sid_b == MEMDB_TCAM_IFTA80_MEM4_1m) {
            *compare = TRUE;

            if (neg_shr_flags &&
                sid_b == MEMDB_TCAM_IFTA80_MEM4_1m) {
                if (check == TRUE) {
                    *neg_shr_flags = BCMPTM_RM_TCAM_KEY0;
                } else {
                    *neg_shr_flags = BCMPTM_RM_TCAM_SHR_KEY0;
                }
            }
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM5_0_MEM5_1m:
        if (sid_b == MEMDB_TCAM_IFTA80_MEM5_0m ||
            sid_b == MEMDB_TCAM_IFTA80_MEM5_1m) {
            *compare = TRUE;

            if (neg_shr_flags &&
                sid_b == MEMDB_TCAM_IFTA80_MEM5_1m) {
                if (check == TRUE) {
                    *neg_shr_flags = BCMPTM_RM_TCAM_KEY0;
                } else {
                    *neg_shr_flags = BCMPTM_RM_TCAM_SHR_KEY0;
                }
            }
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM6_0_MEM6_1m:
        if (sid_b == MEMDB_TCAM_IFTA80_MEM6_0m ||
            sid_b == MEMDB_TCAM_IFTA80_MEM6_1m) {
            *compare = TRUE;

            if (neg_shr_flags &&
                sid_b == MEMDB_TCAM_IFTA80_MEM6_1m) {
                if (check == TRUE) {
                    *neg_shr_flags = BCMPTM_RM_TCAM_KEY0;
                } else {
                    *neg_shr_flags = BCMPTM_RM_TCAM_SHR_KEY0;
                }
            }
        }
        break;
    case MEMDB_TCAM_IFTA80_MEM7_0_MEM7_1m:
        if (sid_b == MEMDB_TCAM_IFTA80_MEM7_0m ||
            sid_b == MEMDB_TCAM_IFTA80_MEM7_1m) {
            *compare = TRUE;

            if (neg_shr_flags &&
                sid_b == MEMDB_TCAM_IFTA80_MEM7_1m) {
                if (check == TRUE) {
                    *neg_shr_flags = BCMPTM_RM_TCAM_KEY0;
                } else {
                    *neg_shr_flags = BCMPTM_RM_TCAM_SHR_KEY0;
                }
            }
        }
        break;
    default:
        break;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_rm_tcam_prio_only_hw_key_info_init(int unit,
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
    hw_field_list[0].field_width[1] = 50;
    hw_field_list[0].field_start_bit[1] = 2;
    hw_field_list[0].field_width[2] = 50;
    hw_field_list[0].field_start_bit[2] = 102;
    hw_field_list[0].field_width[3] = 155;
    hw_field_list[0].field_start_bit[3] = 210;

    hw_field_list[1].num_fields = 4;
    hw_field_list[1].field_width[0] = 1;
    hw_field_list[1].field_start_bit[0] = 1;
    hw_field_list[1].field_width[1] = 50;
    hw_field_list[1].field_start_bit[1] = 52;
    hw_field_list[1].field_width[2] = 50;
    hw_field_list[1].field_start_bit[2] = 152;
    hw_field_list[1].field_width[3] = 155;
    hw_field_list[1].field_start_bit[3] = 379;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_rm_tcam_ser_corr_sid_info_get(int unit,
                                 bcmdrd_sid_t sid,
                                 bcmdrd_sid_t *tcam_only_sid,
                                 bcmdrd_sid_t *data_only_sid,
                                 bcmdrd_sid_t *tcam_data_sid,
                                 bcmdrd_sid_t *paired_sid,
                                 uint8_t *sid_count)
{
    SHR_FUNC_ENTER(unit);

    switch (sid) {
        case MEMDB_TCAM_IFTA80_MEM0_0m:
        case MEMDB_TCAM_IFTA80_MEM0_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM0_1m:
        case MEMDB_TCAM_IFTA80_MEM0_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM0_1_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM0_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM0_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM0_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM1_0m:
        case MEMDB_TCAM_IFTA80_MEM1_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM1_0_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0_MEM1_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM1_1m:
        case MEMDB_TCAM_IFTA80_MEM1_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM1_1_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM1_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM1_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM1_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0_MEM1_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM2_0m:
        case MEMDB_TCAM_IFTA80_MEM2_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM2_0_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM2_0_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM2_0_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM2_0m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM2_0_MEM2_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM2_1m:
        case MEMDB_TCAM_IFTA80_MEM2_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM2_1_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM2_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM2_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM2_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM2_0_MEM2_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM3_0m:
        case MEMDB_TCAM_IFTA80_MEM3_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM3_0_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM3_0_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM3_0_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM3_0m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM3_0_MEM3_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM3_1m:
        case MEMDB_TCAM_IFTA80_MEM3_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM3_1_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM3_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM3_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM3_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM3_0_MEM3_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM4_0m:
        case MEMDB_TCAM_IFTA80_MEM4_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM4_0_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_MEM4_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM4_1m:
        case MEMDB_TCAM_IFTA80_MEM4_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM4_1_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM4_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM4_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM4_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_MEM4_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM5_0m:
        case MEMDB_TCAM_IFTA80_MEM5_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM5_0_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_MEM5_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM5_1m:
        case MEMDB_TCAM_IFTA80_MEM5_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM5_1_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM5_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM5_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM5_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_MEM5_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM6_0m:
        case MEMDB_TCAM_IFTA80_MEM6_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM6_0_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM6_0_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM6_0_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM6_0m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM6_0_MEM6_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM6_1m:
        case MEMDB_TCAM_IFTA80_MEM6_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM6_1_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM6_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM6_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM6_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM6_0_MEM6_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM7_0m:
        case MEMDB_TCAM_IFTA80_MEM7_0_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM7_0_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0_MEM7_1m;
            *sid_count = 1;
            break;
        case MEMDB_TCAM_IFTA80_MEM7_1m:
        case MEMDB_TCAM_IFTA80_MEM7_1_ONLYm:
        case MEMDB_TCAM_IFTA80_MEM7_1_DATA_ONLYm:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM7_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM7_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0_MEM7_1m;
            *sid_count = 1;
            break;

        case MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_ONLYm;
            tcam_only_sid[1] = MEMDB_TCAM_IFTA80_MEM0_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM0_1_DATA_ONLYm;
            data_only_sid[1] = MEMDB_TCAM_IFTA80_MEM0_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0m;
            tcam_data_sid[1] = MEMDB_TCAM_IFTA80_MEM0_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m;
            *sid_count = 2;
            break;
        case MEMDB_TCAM_IFTA80_MEM1_0_MEM1_1m:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0_ONLYm;
            tcam_only_sid[1] = MEMDB_TCAM_IFTA80_MEM1_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM1_1_DATA_ONLYm;
            data_only_sid[1] = MEMDB_TCAM_IFTA80_MEM1_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM1_0m;
            tcam_data_sid[1] = MEMDB_TCAM_IFTA80_MEM1_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM0_0_MEM0_1m;
            *sid_count = 2;
            break;
        case MEMDB_TCAM_IFTA80_MEM2_0_MEM2_1m:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM2_0_ONLYm;
            tcam_only_sid[1] = MEMDB_TCAM_IFTA80_MEM2_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM2_1_DATA_ONLYm;
            data_only_sid[1] = MEMDB_TCAM_IFTA80_MEM2_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM2_0m;
            tcam_data_sid[1] = MEMDB_TCAM_IFTA80_MEM2_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM2_0_MEM2_1m;
            *sid_count = 2;
            break;
        case MEMDB_TCAM_IFTA80_MEM3_0_MEM3_1m:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM3_0_ONLYm;
            tcam_only_sid[1] = MEMDB_TCAM_IFTA80_MEM3_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM3_1_DATA_ONLYm;
            data_only_sid[1] = MEMDB_TCAM_IFTA80_MEM3_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM3_0m;
            tcam_data_sid[1] = MEMDB_TCAM_IFTA80_MEM3_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM3_0_MEM3_1m;
            *sid_count = 2;
            break;
        case MEMDB_TCAM_IFTA80_MEM4_0_MEM4_1m:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_ONLYm;
            tcam_only_sid[1] = MEMDB_TCAM_IFTA80_MEM4_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM4_1_DATA_ONLYm;
            data_only_sid[1] = MEMDB_TCAM_IFTA80_MEM4_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0m;
            tcam_data_sid[1] = MEMDB_TCAM_IFTA80_MEM4_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM4_0_MEM4_1m;
            *sid_count = 2;
            break;
        case MEMDB_TCAM_IFTA80_MEM5_0_MEM5_1m:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_ONLYm;
            tcam_only_sid[1] = MEMDB_TCAM_IFTA80_MEM5_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM5_1_DATA_ONLYm;
            data_only_sid[1] = MEMDB_TCAM_IFTA80_MEM5_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0m;
            tcam_data_sid[1] = MEMDB_TCAM_IFTA80_MEM5_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM5_0_MEM5_1m;
            *sid_count = 2;
            break;
        case MEMDB_TCAM_IFTA80_MEM6_0_MEM6_1m:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM6_0_ONLYm;
            tcam_only_sid[1] = MEMDB_TCAM_IFTA80_MEM6_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM6_1_DATA_ONLYm;
            data_only_sid[1] = MEMDB_TCAM_IFTA80_MEM6_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM6_0m;
            tcam_data_sid[1] = MEMDB_TCAM_IFTA80_MEM6_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM6_0_MEM6_1m;
            *sid_count = 2;
            break;
        case MEMDB_TCAM_IFTA80_MEM7_0_MEM7_1m:
            tcam_only_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0_ONLYm;
            tcam_only_sid[1] = MEMDB_TCAM_IFTA80_MEM7_1_ONLYm;
            data_only_sid[0] = MEMDB_TCAM_IFTA80_MEM7_1_DATA_ONLYm;
            data_only_sid[1] = MEMDB_TCAM_IFTA80_MEM7_1_DATA_ONLYm;
            tcam_data_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0m;
            tcam_data_sid[1] = MEMDB_TCAM_IFTA80_MEM7_1m;
            paired_sid[0] = MEMDB_TCAM_IFTA80_MEM7_0_MEM7_1m;
            *sid_count = 2;
            break;
        default:
            *sid_count = 0;
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}
