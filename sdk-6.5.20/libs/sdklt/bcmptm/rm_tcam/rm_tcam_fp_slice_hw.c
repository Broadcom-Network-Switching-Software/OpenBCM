/*! \file rm_tcam_fp_slice_hw.c
 *
 * Low Level Functions for FP Slice hw install
 * This file contains low level functions for FP based TCAMs
 * to do write slice related info to HW
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
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include "rm_tcam_fp_utils.h"
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM


/*******************************************************************************
 * Private variables
 */
static bcmptm_rm_tcam_sid_info_t sid_info[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */
/*
 * Function:
 *     bcmptm_rm_tcam_fp_slice_priority_compare
 * Purpose:
 *     Compare two slice priorities.
 * Parameters:
 *     a - (IN) First slice priority.
 *     b - (IN) Second slice priority.
 * Returns:
 *     -1 if prio_first <  prio_second
 *     1 if prio_first >=  prio_second
 */
static int
bcmptm_rm_tcam_fp_slice_priority_compare(const void *a, const void *b)
{
    bcmptm_slice_prio_map_t *first;  /* First slice.   */
    bcmptm_slice_prio_map_t *second; /* Second slice.  */

    first = (bcmptm_slice_prio_map_t *)a;
    second = (bcmptm_slice_prio_map_t *)b;

    if (first->slice_priority < second->slice_priority) {
        return (-1);
    }

    return (1);
}

/*******************************************************************************
 * Public Functions
 */
/*
 * Function:
 *     bcmptm_rm_tcam_fp_lt_group_priority_compare
 * Purpose:
 *     Compare two group priorities linked to logical tables.
 * Parameters:
 *     a - (IN) First logical table group priority.
 *     b - (IN) Second logical table group priority.
 * Returns:
 *     -1 if prio_first <  prio_second
 *     0 if prio_first == prio_second
 *     1 if prio_first >  prio_second
 */
int
bcmptm_rm_tcam_fp_lt_group_priority_compare(const void *a, const void *b)
{
    bcmptm_rm_tcam_lt_config_t *first;  /* First Logical table configuration.   */
    bcmptm_rm_tcam_lt_config_t *second; /* Second Logical table configuration.  */

    first = (bcmptm_rm_tcam_lt_config_t *)a;
    second = (bcmptm_rm_tcam_lt_config_t *)b;

    if (first->priority < second->priority) {
        return (-1);
    } else if (first->priority > second->priority) {
        return (1);
    }

    return (0);
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_group_lt_action_prio_install
 * Purpose:
 *     Write logical table action priortiy information to hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     ltid     - (IN) Logical Table enum that is accessing the table
 *     pipe_id  - (IN) Pipe number
 *     stage_fc - (IN) Field stage structure.
 * Returns:
 *     BCM_E_XXX
 */
int
bcmptm_rm_tcam_fp_group_lt_action_prio_install(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                int pipe_id,
                                bcmptm_rm_tcam_fp_stage_t *stage_fc)
{
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    uint32_t num_groups_per_pipe;
    uint16_t field_idx;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Input parameter check. */
    SHR_NULL_CHECK(stage_fc, SHR_E_PARAM);

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
        SHR_EXIT();
    }

    sal_memset(&sid_info[unit], 0, sizeof(bcmptm_rm_tcam_sid_info_t));

    /* Initialize LT config pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_lt_config_info_get(unit, ltid,
                 ltid_info,
                 pipe_id, stage_fc->stage_id,
                 &lt_config));
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(
                          unit, ltid, ltid_info, pipe_id,
                          &num_groups_per_pipe));

    sid_info[unit].lt_action_pri_count = num_groups_per_pipe;

    SHR_ALLOC(sid_info[unit].lt_action_pri,
              sizeof(uint8_t) * num_groups_per_pipe,
              "bcmptmRmtcamLtActionPri");

    /* Modify the contents of the sid. */
    for (field_idx = 0;
         field_idx < num_groups_per_pipe;
         field_idx++) {
         /* update the action priority in entry buffer */
         sid_info[unit].lt_action_pri[field_idx] = lt_config->lt_action_pri;
         lt_config = lt_config + 1;
    }

    /* set tbl Instance depending on pipe number*/
    sid_info[unit].tbl_inst = pipe_id;

    sid_info[unit].stage_flags = stage_fc->flags;
    sid_info[unit].oper_flags |= BCMPTM_RM_TCAM_OPER_ACTION_PRIO_UPDATE;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                    BCMPTM_RM_TCAM_IFP_LOGICAL_TABLE_ACTION_PRIORITY,
                    BCMFP_RM_TCAM_SID_OPER_UPDATE,
                    &sid_info[unit]));

exit:
    SHR_FREE(sid_info[unit].lt_action_pri);
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_fp_group_lt_partition_prio_install(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                bcmptm_rm_tcam_fp_group_t *fg,
                                int pipe_id,
                                bcmptm_rm_tcam_fp_stage_id_t stage_id)
{
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    uint8_t num_slices;
    uint8_t field_idx;
    bcmptm_rm_tcam_fp_stage_t  *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    sal_memset(&sid_info[unit], 0, sizeof(bcmptm_rm_tcam_sid_info_t));

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          pipe_id,
                                          stage_id,
                                          &stage_fc));

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_virtual_slice_map_install(unit,
                                                         ltid,
                                                         ltid_info,
                                                         pipe_id,
                                                         stage_id));
        SHR_EXIT();
    }

    /* Initialize LT config pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_lt_config_info_get(unit, ltid, ltid_info,
                         pipe_id, stage_id,
                         &lt_config));
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_slices_count_get(unit,
                         ltid, ltid_info, pipe_id,
                         &num_slices));

    SHR_ALLOC(sid_info[unit].group_ltid,
              sizeof(uint8_t),
              "bcmptmRmtcamGrpLtId");

    sid_info[unit].lt_part_map = (lt_config[fg->group_ltid].lt_part_map);
    sid_info[unit].num_slices = num_slices;
    sid_info[unit].group_ltid[0] = fg->group_ltid;

    SHR_ALLOC(sid_info[unit].lt_part_pri,
              sizeof(uint8_t) * num_slices,
              "bcmptmRmtcamLtPartitionpriority");

    /* Modify the contents of the sid. */
    for (field_idx = 0;
         field_idx < num_slices;
         field_idx++) {
         /* update the partition priority in entry buffer */
         sid_info[unit].lt_part_pri[field_idx]
           = lt_config[fg->group_ltid].lt_part_pri[field_idx];
    }

    /* set tbl Instance depending on pipe number*/
    sid_info[unit].tbl_inst = pipe_id;
    sid_info[unit].ltid_info = (void *)ltid_info;
    sid_info[unit].stage_id = stage_id;
    sid_info[unit].stage_flags = stage_fc->flags;
    sid_info[unit].oper_flags |= BCMPTM_RM_TCAM_OPER_PARTITION_PRIO_UPDATE;
    sid_info[unit].group_mode = fg->group_mode;

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                    BCMPTM_RM_TCAM_IFP_LOGICAL_TABLE_CONFIG,
                    BCMFP_RM_TCAM_SID_OPER_UPDATE,
                    &sid_info[unit]));
exit:
    SHR_FREE(sid_info[unit].lt_part_pri);
    SHR_FREE(sid_info[unit].group_ltid);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_slice_selcodes_install
 * Purpose:
 *     Write Slice selcodes information to hardware.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     entry_attr - (IN) entry attrs structure.
 *     slice_id   - (IN) Slice id.
 * Returns:
 *     BCM_E_XXX
 */

int
bcmptm_rm_tcam_fp_slice_selcodes_install(int unit, bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                          uint8_t slice_id)
{
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;
    int parts_count = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    sal_memset(&sid_info[unit], 0, sizeof(bcmptm_rm_tcam_sid_info_t));

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attr->pipe_id,
                                          entry_attr->stage_id,
                                          &stage_fc));

    /* Get number of entry parts. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                   entry_attr->group_mode,
                                   stage_fc->flags,
                                   &parts_count));

    sid_info[unit].parts_count = parts_count;

    if ((BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode) &&
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE)) {
        sid_info[unit].intraslice = 1;
    }

    if (BCMPTM_RM_TCAM_GRP_MODE_DBLINTER == entry_attr->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode) {
        sid_info[unit].pairing = 1;
    }

    /* set tbl Instance depending on pipe number*/
    sid_info[unit].tbl_inst = entry_attr->pipe_id;
    sid_info[unit].slice_id= slice_id;

    sid_info[unit].stage_flags = stage_fc->flags;
    sid_info[unit].oper_flags |= BCMPTM_RM_TCAM_OPER_SELCODE_INSTALL;
    sid_info[unit].selcode = &(entry_attr->selcode);

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_FIXED) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_SLICE_CONTROL,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR_2,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_KEY4_DVP_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_KEY8_DVP_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_KEY4_MDL_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_VFP_KEY_CONTROL_1,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_VFP_KEY_CONTROL_2,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_slice_selcodes_copy
 * Purpose:
 *     Copy Slice selcodes information from primary to new slice in hardware.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     entry_attr - (IN) entry attrs structure.
 *     slice_id   - (IN) Slice id.
 * Returns:
 *     BCM_E_XXX
 */

int
bcmptm_rm_tcam_fp_slice_selcodes_copy(int unit, bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                          uint8_t pri_slice_id,
                          uint8_t slice_id)
{
    bcmptm_rm_tcam_fp_stage_t           *stage_fc = NULL;
    int parts_count = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    sal_memset(&sid_info[unit], 0, sizeof(bcmptm_rm_tcam_sid_info_t));

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attr->pipe_id,
                                          entry_attr->stage_id,
                                          &stage_fc));

    /* Get number of entry parts. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                   entry_attr->group_mode,
                                   stage_fc->flags,
                                   &parts_count));

    sid_info[unit].parts_count = parts_count;
    sid_info[unit].selcode = &(entry_attr->selcode);

    if ((BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode) &&
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE)) {
        sid_info[unit].intraslice = 1;
    }

    /* set tbl Instance depending on pipe number*/
    sid_info[unit].tbl_inst = entry_attr->pipe_id;
    sid_info[unit].pri_slice_id= pri_slice_id;
    sid_info[unit].slice_id= slice_id;

    sid_info[unit].stage_flags = stage_fc->flags;
    sid_info[unit].oper_flags |= BCMPTM_RM_TCAM_OPER_SELCODE_COPY;

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_FIXED) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_SLICE_CONTROL,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR_2,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_KEY4_DVP_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_KEY8_DVP_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_KEY4_MDL_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_VFP_KEY_CONTROL_1,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_VFP_KEY_CONTROL_2,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_slice_selcodes_reset
 * Purpose:
 *     Reset Slice selcodes information in hardware.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     entry_attr - (IN) entry attrs structure.
 *     slice_id   - (IN) Slice id.
 * Returns:
 *     BCM_E_XXX
 */

int
bcmptm_rm_tcam_fp_slice_selcodes_reset(int unit, bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                          uint8_t slice_id)
{
    bcmptm_rm_tcam_fp_stage_t           *stage_fc = NULL;
    int parts_count = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    sal_memset(&sid_info[unit], 0, sizeof(bcmptm_rm_tcam_sid_info_t));

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          entry_attr->pipe_id,
                                          entry_attr->stage_id,
                                          &stage_fc));

    /* Get number of entry parts. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(unit,
                                   entry_attr->group_mode,
                                   stage_fc->flags,
                                   &parts_count));

    sid_info[unit].parts_count = parts_count;
    sid_info[unit].selcode = &(entry_attr->selcode);


    if ((BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA == entry_attr->group_mode
        || BCMPTM_RM_TCAM_GRP_MODE_QUAD == entry_attr->group_mode) &&
        (stage_fc->flags & BCMPTM_STAGE_INTRASLICE_CAPABLE)) {
        sid_info[unit].intraslice = 1;
    }

    /* set tbl Instance depending on pipe number*/
    sid_info[unit].tbl_inst = entry_attr->pipe_id;
    sid_info[unit].slice_id= slice_id;

    sid_info[unit].stage_flags = stage_fc->flags;
    sid_info[unit].oper_flags |= BCMPTM_RM_TCAM_OPER_SELCODE_RESET;

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_FIXED) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_SLICE_CONTROL,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_CLASSID_SELECTOR_2,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_KEY4_DVP_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_KEY8_DVP_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_KEY4_MDL_SELECTOR,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_VFP_KEY_CONTROL_1,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_VFP_KEY_CONTROL_2,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_tcam_fp_slice_enable_set
 * Purpose:
 *     Write Slice information to hardware.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     ltid     - (IN) Logical Table enum that is accessing the table
 *     pipe_id  - (IN) Pipe number
 *     stage_fc - (IN) Field stage structure.
 * Returns:
 *     BCM_E_XXX
 */

int
bcmptm_rm_tcam_fp_slice_enable_set(int unit, bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          int pipe_id,
                          bcmptm_rm_tcam_fp_stage_id_t stage_id,
                          uint32_t group_mode,
                          uint8_t slice_idx, int enable)
{
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          pipe_id,
                                          stage_id,
                                          &stage_fc));

    sal_memset(&sid_info[unit], 0, sizeof(bcmptm_rm_tcam_sid_info_t));

    /* set tbl Instance depending on pipe number*/

    sid_info[unit].tbl_inst = pipe_id;
    sid_info[unit].slice_id = slice_idx;
    sid_info[unit].group_mode = group_mode;
    sid_info[unit].stage_flags = stage_fc->flags;
    sid_info[unit].slice_group_id = stage_fc->slice_group_id;

    if (enable) {
        sid_info[unit].oper_flags |= BCMPTM_RM_TCAM_OPER_SLICE_ENABLE;
    } else {
        sid_info[unit].oper_flags |= BCMPTM_RM_TCAM_OPER_SLICE_DISABLE;
    }

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_SELCODE) {
        if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_FIXED) {
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_SLICE_CONTROL,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
         } else {
            SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_VFP_SLICE_CONTROL,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
         }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_IFP_CONFIG,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *    bcmptm_rm_tcam_fp_virtual_slice_map_install
 * Purpose:
 *     Map the physical slices with the virtual slice ID.
 *     Map the virtual slices with the Virtual group ID.
 * Paramters:
 *     unit          - (IN) BCM device number
 *     ltid          - (IN) Logical Table enum that is accessing the table
 *     pipe_id       - (IN) Pipe number
 *     stage_id      - (IN) Field stage id.
 *
 * Returns:
 *     SHR_E_XXX
 */
int bcmptm_rm_tcam_fp_virtual_slice_map_install(int unit,
                                          bcmltd_sid_t ltid,
                                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                                          int pipe_id,
                                          bcmptm_rm_tcam_fp_stage_id_t stage_id)
{
    bcmptm_rm_tcam_lt_config_t *lt_config = NULL;
    bcmptm_rm_tcam_lt_config_t *lt_info = NULL;
    bcmptm_rm_tcam_lt_config_t *hw_lt_info = NULL;
    bcmptm_slice_prio_map_t *slice_prio_map = NULL;
    int8_t virtual_slice_count = 0;
    uint8_t group_id = 0;
    uint8_t slice_id = 0;
    int8_t slice_count = 0;
    uint8_t updated_slice = 0;
    int8_t virtual_slice_no = 0;
    uint32_t num_groups_per_pipe = 0;
    uint32_t num_groups = 0;
    uint8_t num_pipes = 0;
    uint8_t num_slices_per_pipe = 0;
    uint8_t field_idx;
    bcmptm_rm_tcam_fp_stage_t *stage_fc = NULL;

    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    sal_memset(&sid_info[unit], 0, sizeof(bcmptm_rm_tcam_sid_info_t));

    /* Initialize field stage pointer. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_fp_stage_info_get(unit,
                                          ltid,
                                          ltid_info,
                                          pipe_id,
                                          stage_id,
                                          &stage_fc));

    /* Initialize LT config pointer. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_fp_lt_config_info_get(unit,
                                                         ltid, ltid_info,
                                                         pipe_id,
                                                         stage_id,
                                                         &lt_config));

    /* Get number of slices per pipe */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_slices_count_get(unit,
                                                         ltid, ltid_info,
                                                         pipe_id,
                                                         &num_slices_per_pipe));
    /* Get number of pipes */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_pipes_count_get(unit,
                                                         ltid, ltid_info,
                                                         &num_pipes));
    /* Get number of groups per pipe */
    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_stage_attr_groups_per_pipe_get(unit,
                                                         ltid, ltid_info,
                                                         pipe_id,
                                                         &num_groups_per_pipe));

    /* Make a copy of the lt_config structure to sort */
    SHR_ALLOC(lt_info, num_slices_per_pipe * sizeof(bcmptm_rm_tcam_lt_config_t),
              "bcmptmRmtcamLtInfo");

    sal_memset(lt_info, 0,
              num_slices_per_pipe * sizeof(bcmptm_rm_tcam_lt_config_t));


    SHR_ALLOC(hw_lt_info,
              num_slices_per_pipe * sizeof(bcmptm_rm_tcam_lt_config_t),
              "bcmptmRmtcamHwLtConfigInfo");

    sal_memset(hw_lt_info, 0,
              num_slices_per_pipe * sizeof(bcmptm_rm_tcam_lt_config_t));

    SHR_ALLOC(slice_prio_map,
              num_slices_per_pipe * sizeof(bcmptm_slice_prio_map_t),
              "bcmptmRmtcamLtSlicePrioMap");

    num_groups = num_groups_per_pipe * num_pipes;

    /*To make a copy of LT_config array without elements with same priority*/
    for (group_id = 0; group_id < num_groups;  group_id++) {
        if ((lt_config[group_id].valid == TRUE)
                && (lt_config[group_id].pipe_id == pipe_id)) {
            for (updated_slice = 0; updated_slice < num_slices_per_pipe;
                    updated_slice++) {
                if (lt_info[updated_slice].valid == TRUE) {
                    if (lt_info[updated_slice].priority ==
                                        lt_config[group_id].priority) {
                        break;
                    }
                } else {
                    break;
                }
            }

            /*No more LT config with different priority to be updated*/
            if (updated_slice == num_slices_per_pipe) {
                break;
            }

            if (lt_info[updated_slice].valid == FALSE) {
                sal_memcpy(&lt_info[updated_slice],
                           &lt_config[group_id],
                           sizeof(bcmptm_rm_tcam_lt_config_t));
            }
        }
    }

    /*Sort the lt_info array based on the priority*/
    sal_qsort(lt_info, num_slices_per_pipe,
              sizeof(bcmptm_rm_tcam_lt_config_t),
              bcmptm_rm_tcam_fp_lt_group_priority_compare);

    /*After sorting the number of valid elements in the lt_info is limited to
    * <= num_slices_per_pipe and in order of increasing priority*/
    virtual_slice_no = num_slices_per_pipe - 1;

    for (virtual_slice_count = num_slices_per_pipe - 1;
         virtual_slice_count >= 0; virtual_slice_count--) {
        if (lt_info[virtual_slice_count].valid == TRUE) {
            sal_memset(slice_prio_map,0 ,
                       num_slices_per_pipe * sizeof(bcmptm_slice_prio_map_t));
            slice_count = 0;
            /*Update the slice_priority array with the sliceid and priority*/
            for (slice_id = 0; slice_id < num_slices_per_pipe; slice_id++) {
                if (lt_info[virtual_slice_count].lt_part_map & (1 << slice_id)){
                    slice_prio_map[slice_count].slice_id = slice_id;
                    slice_prio_map[slice_count].slice_priority =
                        lt_info[virtual_slice_count].lt_part_pri[slice_id];
                    slice_count++;
                }
            }

            /*Sort the slice_prio_map array based on the priority*/
            sal_qsort(slice_prio_map, slice_count,
                    sizeof(bcmptm_slice_prio_map_t),
                    bcmptm_rm_tcam_fp_slice_priority_compare);

            /*Update the hw_lt_info with the sorted slice_prio_map*/
            while (slice_count-- > 0) {
                /*virtual_slice_no cannot be less than 0*/
                if (virtual_slice_no < 0) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
                /*Update the virtual slice No.*/
                hw_lt_info[virtual_slice_no].lt_action_pri =
                                           slice_prio_map[slice_count].slice_id;
                /*Update the virtual group No.*/
                hw_lt_info[virtual_slice_no].group_ltid =
                                        lt_info[virtual_slice_count].group_ltid;
                virtual_slice_no--;
            }
        }
    }

    sid_info[unit].num_slices = num_slices_per_pipe;

    SHR_ALLOC(sid_info[unit].lt_action_pri,
              sizeof(uint8_t) * num_slices_per_pipe,
              "bcmptmRmtcamLtActionPri");
    SHR_ALLOC(sid_info[unit].group_ltid,
              sizeof(uint8_t) * num_slices_per_pipe,
              "bcmptmRmtcamGroupLtId");

    /* Modify the contents of the sid. */
    for (field_idx = 0; field_idx < num_slices_per_pipe; field_idx++) {

        /*Update the Virtual Group No. in entry buffer*/
        sid_info[unit].group_ltid[field_idx]
           = hw_lt_info[field_idx].group_ltid;

        /*Update the physical slice No. in entry buffer*/
        sid_info[unit].lt_action_pri[field_idx]
           = hw_lt_info[field_idx].lt_action_pri;
    }

    /* set tbl Instance depending on pipe number*/
    sid_info[unit].tbl_inst = pipe_id;
    sid_info[unit].stage_flags = stage_fc->flags;
    sid_info[unit].oper_flags |= BCMPTM_RM_TCAM_OPER_SLICE_MAP_UPDATE;

    if (stage_fc->flags & BCMPTM_STAGE_KEY_TYPE_FIXED) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_EFP_SLICE_MAP,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
    } else {

        SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_memreg_oper(unit, ltid,
                        BCMPTM_RM_TCAM_VFP_SLICE_MAP,
                        BCMFP_RM_TCAM_SID_OPER_UPDATE,
                        &sid_info[unit]));
    }

exit:
    SHR_FREE(sid_info[unit].lt_action_pri);
    SHR_FREE(sid_info[unit].group_ltid);
    SHR_FREE(lt_info);
    SHR_FREE(hw_lt_info);
    SHR_FREE(slice_prio_map);
    SHR_FUNC_EXIT();
}

