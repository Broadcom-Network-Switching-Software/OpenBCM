/*! \file rm_tcam_init.c
 *
 * Low Level Functions for TCAM resource manager init.
 * This file contains low level functions to initialize the
 * TCAMs software structure.
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
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"
#include "rm_tcam_prio_eid.h"
#include "rm_tcam_fp_entry_mgmt.h"
#include "rm_tcam_traverse.h"
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmevm/bcmevm_api.h>
#include <shr/shr_ha.h>
#include <bcmptm/bcmptm_rm_tcam_fp_internal.h>
#include <bcmissu/issu_api.h>
#include <bcmcfg/comp/bcmcfg_feature_ctl.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmcfg/bcmcfg_lt.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/* To get the pointer to TCAM Resource Manager SW state for the
 * given Logical device id
 */
#define RM_TCAM_TABLE_VERSION 1

#define RM_TCAM_TABLE_STRUCT_SIGN 0x7654321

#define RM_TCAM_MAX_NUM_LTID_PER_EVENT_GROUP 2

/*! Global data structure to hold pointers to TCAM resource manager.*/
#define RM_TCAM_MD(unit) rm_tcam_md[unit]

#define RM_TCAM_INFO_GLOBAL(unit) rm_tcam_global_info[unit]

#define RM_TCAM_HA_HEADER_PREVIOUS(unit) rm_tcam_header_previous[unit]

#define RM_TCAM_DYN_PTRM_TBL(unit)  rm_tcam_dyn_ptrm_table[unit]
/*******************************************************************************
 * Global variables
 */
/* To know the next free sub_component_id for HA */
bool sub_component_id[BCMDRD_CONFIG_MAX_UNITS][BCMPTM_RM_TCAM_HA_SUBID_MAX];

/* To pass any info for future purpose */
uint32_t user_info = 0;

/*
 * Metadata generated for each tcam LT which doesnot
 * change in runtime.
 */
bcmptm_rm_tcam_md_t *rm_tcam_md[BCMDRD_CONFIG_MAX_UNITS];

/*
 * Metadata generated for the whole RM-TCAM which doesnot
 * change in runtime.
 */
bcmptm_rm_tcam_info_global_t *rm_tcam_global_info[BCMDRD_CONFIG_MAX_UNITS];

bcmptm_rm_tcam_trans_info_t *rm_tcam_header_previous[BCMDRD_CONFIG_MAX_UNITS];

bcmptm_rm_tcam_dyn_ptrm_table_t *rm_tcam_dyn_ptrm_table[BCMDRD_CONFIG_MAX_UNITS];

/*
 * Atomic transactions are enabled or not using enable_atomic_trans
 * feature control.
 */
static bool rm_tcam_static_atomic_trans[BCMDRD_CONFIG_MAX_UNITS];

/*
 * Atomic transactions are enabled or not dynamically using bcmptm API
 * bcmptm_mreq_atomic_trans_enable.
 */
static bool rm_tcam_dynamic_atomic_trans[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
int
bcmptm_rm_tcam_pipe_config_get(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint8_t num_pipes,
                                uint8_t *pipe_count)
{
    bcmltd_sid_t config_ltid;
    bcmltd_fid_t config_lfid;
    uint64_t pipe_unique = 0;

     /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    *pipe_count = 1;
    if (ltid_info->rm_more_info[0].config_ltid != 0) {
        config_ltid = ltid_info->rm_more_info->config_ltid;
        config_lfid = ltid_info->rm_more_info->pipe_config_lfid;
        SHR_IF_ERR_EXIT
            (bcmcfg_field_get(unit, config_ltid, config_lfid, &pipe_unique));
        if (pipe_unique == ltid_info->rm_more_info->per_pipe_config_value) {
            *pipe_count = num_pipes;
        }
    } else if (ltid_info->rm_more_info[0].is_per_block) {
        /* LT is per block, pipe count shouldn't be adjusted */
        *pipe_count = num_pipes;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_dyn_lt_hw_info_cleanup(int unit, lt_mreq_info_t *mreq_info)
{
    uint8_t num_rows = 0;
    uint8_t num_hw_info_count = 1;
    uint32_t idx = 0;
    uint8_t iter= 0;
    bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *more_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mreq_info, SHR_E_FAIL);

    hw_entry_info = (bcmptm_rm_tcam_hw_entry_info_t *)mreq_info->hw_entry_info;
    more_info = (bcmptm_rm_tcam_more_info_t *)mreq_info->rm_more_info;
    SHR_NULL_CHECK(hw_entry_info, SHR_E_FAIL);
    SHR_NULL_CHECK(more_info, SHR_E_FAIL);

    if (mreq_info->pt_type == LT_PT_TYPE_FP) {
        if (more_info->flags & BCMPTM_RM_TCAM_UFT_HASH_BANKS) {
             num_hw_info_count = 1;
        } else {
            /*
             * IFP chip variants banks and
             * VFP dynamic banks
             */
            num_hw_info_count = mreq_info->hw_entry_info_count;
        }
    }

    for (iter = 0; iter < num_hw_info_count; iter++) {

        num_rows = hw_entry_info[iter].num_depth_inst;

        /* Free previously allocated memory. */
        if (hw_entry_info[iter].sid != NULL) {
            for (idx = 0; idx < num_rows; idx++) {
                if (hw_entry_info[iter].sid[idx] != NULL) {
                    SHR_FREE(hw_entry_info[iter].sid[idx]);
                }
            }
            SHR_FREE(hw_entry_info[iter].sid);
        }

        if (hw_entry_info[iter].sid_data_only != NULL) {
            for (idx = 0; idx < num_rows; idx++) {
                if (hw_entry_info[iter].sid_data_only[idx]) {
                    SHR_FREE(hw_entry_info[iter].sid_data_only[idx]);
                }
            }
            SHR_FREE(hw_entry_info[iter].sid_data_only);
        }
        hw_entry_info[iter].num_depth_inst = 0;
    }
    if (mreq_info->pt_type == LT_PT_TYPE_FP) {
        if (more_info->entry_count != NULL) {
            for (idx = 0; idx < more_info->pipe_count; idx++) {
                SHR_FREE(more_info->entry_count[idx]);
            }
            SHR_FREE(more_info->entry_count);
        }
        SHR_FREE(more_info->slice_info);
        SHR_FREE(more_info->tile_info);
    }
    SHR_FREE(more_info->slice_count);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_dyn_lt_mreq_info_cleanup(int unit, bcmltd_sid_t ltid)
{
    int rv = 0;
    lt_mreq_info_t *lrd_info = NULL;
    bcmptm_rm_tcam_hw_entry_info_t *hw_entry_info = NULL;
    bcmptm_rm_tcam_more_info_t *more_info = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_rm_tcam_dyn_ptrm_tbl_get(unit,
                                         ltid,
                                         &lrd_info);
    if (rv != SHR_E_NONE) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_dyn_lt_hw_info_cleanup(unit, lrd_info));

    hw_entry_info = (bcmptm_rm_tcam_hw_entry_info_t *)lrd_info->hw_entry_info;
    more_info = (bcmptm_rm_tcam_more_info_t *)lrd_info->rm_more_info;
    SHR_FREE(hw_entry_info);
    SHR_FREE(more_info->shared_lt_info);
    SHR_FREE(more_info->main_lt_info);
    SHR_FREE(more_info);
    SHR_FREE(lrd_info);
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_issu_ha_struct_id_info_clear(int unit, uint8_t sub_id)
{
    uint32_t issu_offset = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* BCMPTM_RM_TCAM_TRANS_INFO_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_TRANS_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_TRANS_INFO_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_FP_ENTRY_SEG_BMP_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_FP_ENTRY_SEG_BMP_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_FP_ENTRY_SEG_BMP_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_RM_TCAM_LT_CONFIG_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_LT_CONFIG_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_LT_CONFIG_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_RM_TCAM_SLICE_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_SLICE_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_SLICE_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_RM_TCAM_FP_GROUP_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_FP_GROUP_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_FP_GROUP_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_RM_TCAM_FP_STAGE_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_FP_STAGE_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_FP_STAGE_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_RM_TCAM_PRIO_EID_INFO_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_EID_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_PRIO_EID_INFO_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_RM_TCAM_PRIO_EID_ENTRY_INFO_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_EID_ENTRY_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_PRIO_EID_ENTRY_INFO_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_RM_TCAM_PRIO_EID_SLICE_INFO_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_EID_SLICE_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_PRIO_EID_SLICE_INFO_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_RM_TCAM_HW_FIELD_LIST_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_HW_FIELD_LIST_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_HW_FIELD_LIST_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_RM_TCAM_PRIO_ONLY_INFO_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_ONLY_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_PRIO_ONLY_INFO_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_RM_TCAM_PRIO_ONLY_ENTRY_INFO_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_ONLY_ENTRY_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_PRIO_ONLY_ENTRY_INFO_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

    /* BCMPTM_RM_TCAM_PRIO_ONLY_SHR_INFO_T_ID */
    rv = bcmissu_struct_info_get(unit,
                            BCMPTM_RM_TCAM_PRIO_ONLY_SHR_INFO_T_ID,
                            BCMMGMT_RM_TCAM_COMP_ID,
                            sub_id,
                            &issu_offset);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_clear(unit,
                                   BCMPTM_RM_TCAM_PRIO_ONLY_SHR_INFO_T_ID,
                                   BCMMGMT_RM_TCAM_COMP_ID,
                                   sub_id));
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * Calculate the total memory required for SW state of a TCAM LTID.
 */
static int
bcmptm_rm_tcam_sub_component_allocate(int unit, uint8_t *sub_id) {
    int16_t iter;
    for (iter = 0; iter < BCMPTM_RM_TCAM_HA_SUBID_MAX; iter++) {
        if (sub_component_id[unit][iter] == FALSE) {
            *sub_id = iter;
            sub_component_id[unit][iter] = TRUE;
            return SHR_E_NONE;
        }
    }
    return SHR_E_INTERNAL;
}

static int
bcmptm_rm_tcam_sub_component_free(int unit, uint8_t sub_id) {

    if (sub_component_id[unit][sub_id] == TRUE) {
        sub_component_id[unit][sub_id] = FALSE;
        return SHR_E_NONE;
    } else {
        return SHR_E_INTERNAL;
    }
}

static int
bcmptm_rm_tcam_sub_component_set(int unit, uint8_t sub_id) {
    if (sub_component_id[unit][sub_id] == FALSE) {
        sub_component_id[unit][sub_id] = TRUE;
        return SHR_E_NONE;
    } else {
        return SHR_E_INTERNAL;
    }
}

static int
bcmptm_rm_tcam_sub_component_init(int unit) {
    int16_t iter;
    for (iter = 0; iter < BCMPTM_RM_TCAM_HA_SUBID_MAX; iter++) {
        sub_component_id[unit][iter] = FALSE;
    }
    return SHR_E_NONE;
}


static int
bcmptm_rm_tcam_calc_mem_required(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_calc_mem_required(unit,
                                                       ltid,
                                                       ltid_info,
                                                       user_data));
    } else if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_calc_mem_required(unit,
                                                        ltid,
                                                        ltid_info,
                                                        user_data));
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_ha_header_init(int unit,
                              bcmltd_sid_t ltid,
                              uint8_t sub_id,
                              void *ha_ptr,
                              int tbl_inst,
                              bool active)
{
    bcmptm_rm_tcam_trans_info_t *ha_header = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ha_ptr, SHR_E_PARAM);

    ha_header = (bcmptm_rm_tcam_trans_info_t *)ha_ptr;

    ha_header->tbl_id = ltid;
    ha_header->tbl_inst = tbl_inst;
    ha_header->active = active;
    ha_header->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;
    ha_header->sign_word = BCMPTM_HA_SIGN_LT_OFFSET_INFO;

    if (RM_TCAM_HA_HEADER_PREVIOUS(unit)) {
        RM_TCAM_HA_HEADER_PREVIOUS(unit)->next_sub_id = sub_id;
    }
    RM_TCAM_HA_HEADER_PREVIOUS(unit) = ha_header;
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_ha_header_update(int unit,
                                uint8_t old_sub_id,
                                uint8_t sub_id)
{
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    bcmptm_rm_tcam_md_t *md = NULL;
    bool found = FALSE;
    uint8_t pipe = 0;

    SHR_FUNC_ENTER(unit);

    md = RM_TCAM_MD(unit);
    while (md != NULL) {
        for (pipe = 0; pipe < md->num_pipes; pipe++) {
            if (md->active_ptrs[pipe] != NULL) {
                trans_info = (bcmptm_rm_tcam_trans_info_t *)md->active_ptrs[pipe];
                if (trans_info != NULL &&
                    trans_info->next_sub_id == old_sub_id) {
                    trans_info->next_sub_id = sub_id;
                    found = TRUE;
                    break;
                }
            }
            if (md->backup_ptrs[pipe] != NULL) {
                trans_info = (bcmptm_rm_tcam_trans_info_t *)md->backup_ptrs[pipe];
                if (trans_info != NULL &&
                    trans_info->next_sub_id == old_sub_id) {
                    trans_info->next_sub_id = sub_id;
                    found = TRUE;
                    break;
                }
            }
        }
        if (found == TRUE) {
            break;
        }
        md = md->next;
    }

    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_ha_header_delete(int unit,
                                uint8_t old_sub_id,
                                bcmptm_rm_tcam_trans_info_t *old_trans_info)
{
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    bcmptm_rm_tcam_md_t *md = NULL;
    bool found = FALSE;
    uint8_t pipe = 0;

    SHR_FUNC_ENTER(unit);

    md = RM_TCAM_MD(unit);
    while (md != NULL) {
        for (pipe = 0; pipe <md->num_pipes; pipe++) {
            if (md->active_ptrs[pipe] != NULL) {
                trans_info = (bcmptm_rm_tcam_trans_info_t *)md->active_ptrs[pipe];
                if (trans_info != NULL &&
                    trans_info->next_sub_id == old_sub_id) {
                    trans_info->next_sub_id = old_trans_info->next_sub_id;
                    found = TRUE;
                    break;
                }
            }
            if (md->backup_ptrs[pipe] != NULL) {
                trans_info = (bcmptm_rm_tcam_trans_info_t *)md->backup_ptrs[pipe];
                if (trans_info != NULL &&
                    trans_info->next_sub_id == old_sub_id) {
                    trans_info->next_sub_id = old_trans_info->next_sub_id;
                    found = TRUE;
                    break;
                }
            }
        }
        if (found == TRUE) {
            break;
        }
        md = md->next;
    }

    if (RM_TCAM_HA_HEADER_PREVIOUS(unit) == old_trans_info) {
        RM_TCAM_HA_HEADER_PREVIOUS(unit) = trans_info;
    }

    SHR_FUNC_EXIT();
}

static int
bcmptm_rm_tcam_trans_state_init(int unit)
{
    bcmcfg_feature_ctl_config_t feature_conf;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmcfg_feature_ctl_config_get(&feature_conf);
    if (SHR_FAILURE(rv) ||
        (feature_conf.enable_atomic_trans == 0)) {
        rm_tcam_static_atomic_trans[unit] = FALSE;
        /*
         * Record whether RM is in dynamic atomic transaction mode.
         * Default value is FALSE.
         */
        rm_tcam_dynamic_atomic_trans[unit] = FALSE;
    } else {
        rm_tcam_static_atomic_trans[unit] = TRUE;
    }

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_undo_uc_state_get(int unit, bool *undo_uc_state)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(undo_uc_state, SHR_E_PARAM);

    *undo_uc_state = (rm_tcam_dynamic_atomic_trans[unit] ||
                      rm_tcam_static_atomic_trans[unit]);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_dynamic_trans_set(int unit, bool dynamic_trans_state)
{
    SHR_FUNC_ENTER(unit);

    rm_tcam_dynamic_atomic_trans[unit] = dynamic_trans_state;

    SHR_FUNC_EXIT();
}

/*
 * Update the SW metadata required for all LTIDs in memory created
 * in pass one.
 */
static int
bcmptm_rm_tcam_init_metadata(int unit,
                             bool warm,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);

    if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID) {
        /* priority eid based tcam initialization. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_eid_init_metadata(unit,
                                                   warm,
                                                   ltid,
                                                   ltid_info,
                                                   user_data));
    } else if (ltid_info->pt_type == LT_PT_TYPE_TCAM_PRI_ONLY) {
        /* priority key based tcam initialization. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_init_metadata(unit,
                                                    warm,
                                                    ltid,
                                                    ltid_info,
                                                    user_data));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_shared_lt_depth_check (int unit,
                                      bcmltd_sid_t ltid,
                                      bcmptm_rm_tcam_lt_info_t *ltid_info,
                                      bool *depth_differs)
{
    bcmltd_sid_t shr_ltid = BCMLTD_SID_INVALID;
    bcmptm_rm_tcam_lt_info_t shared_ltid_info;
    uint32_t shared_count;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    shared_count = ltid_info->rm_more_info->shared_lt_count;
    *depth_differs = FALSE;
    for (idx = 0; idx < shared_count ; idx++) {
        shr_ltid = ltid_info->rm_more_info->shared_lt_info[idx];
        if (ltid == shr_ltid) {
            continue;
        }
        sal_memset(&shared_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ptrm_info_get(unit,
                                          shr_ltid,
                                          0,
                                          0,
                                          ((void *)&shared_ltid_info),
                                          NULL,
                                          NULL));
        if (shared_ltid_info.hw_entry_info->num_depth_inst !=
            ltid_info->hw_entry_info->num_depth_inst) {
            *depth_differs = TRUE;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT();

}

/*
 * If more than two LTs are eligible to share the same RM TCAM state,
 * then allocate the state for the first LT and assign the same state
 * the remaining shared LTS.
 */
STATIC int
bcmptm_rm_tcam_shared_lt_init(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              bool warm,
                              bcmptm_sub_phase_t phase,
                              uint32_t *lt_state_offset,
                              bool *tcam_info_required)
{
    bcmptm_rm_tcam_lt_info_t shared_ltid_info;
    uint8_t shared_count = 0;
    uint8_t idx = 0;
    bcmltd_sid_t shr_ltid = BCMLTD_SID_INVALID;
    bool shared = TRUE;
    bcmltd_sid_t lowest_ltid = BCMLTD_SID_INVALID;
    bcmptm_rm_tcam_prio_only_info_t *tcam_info = NULL;
    bool depth_differ = FALSE;
    bool sid_rows_differ;
    int pipe;
    uint8_t num_pipes = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_state_offset, SHR_E_PARAM);
    SHR_NULL_CHECK(tcam_info_required, SHR_E_PARAM);

    (*tcam_info_required) = FALSE;

    /* if it is dynamically Managed LT, set tcam_info_required to true,
     * irrespective of tcam sharing details.
     */
    if (ltid_info->rm_more_info->flags & BCMPTM_RM_TCAM_DYNAMIC_MANAGEMENT ||
        ltid_info->hw_entry_info[0].sid == NULL) {
        (*tcam_info_required) = TRUE;
        SHR_EXIT();
    }
    /*
     * For LT's sharing same TCAM, mem required and metadata init needs to
     * be done for the first LT. Other LT's will use the LT same offset.
     */

    shared_count = ltid_info->rm_more_info->shared_lt_count;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_shared_lt_depth_check(unit, ltid, ltid_info, &depth_differ));

    for (idx = 0; idx < shared_count ; idx++) {
        shr_ltid = ltid_info->rm_more_info->shared_lt_info[idx];
        if (ltid == shr_ltid) {
            continue;
        }
        shared = FALSE;
        sid_rows_differ = FALSE;
        sal_memset(&shared_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ptrm_info_get(unit,
                                          shr_ltid,
                                          0,
                                          0,
                                          ((void *)&shared_ltid_info),
                                          NULL,
                                          NULL));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ltid_tcam_info_shared_check(unit,
                                                        ltid_info,
                                                        &shared_ltid_info,
                                                        &shared,
                                                        &sid_rows_differ));
        if (shared == TRUE) {
            if (sid_rows_differ == TRUE && depth_differ == TRUE) {
                shared = FALSE;
                continue;
            }
            if (shared_ltid_info.hw_entry_info[0].view_type) {
                shared = FALSE;
                continue;
            }
            if (shr_ltid < lowest_ltid) {
                lowest_ltid = shr_ltid;
            }
        }
    }

    if (lowest_ltid == BCMLTD_SID_INVALID) {
        /*
         * If lowest LTID is not set it means that the LT's
         * cannnot be shared. if it is set it is set to the lowest LTID
         * among the shared LT's
        */
        (*tcam_info_required) = TRUE;
        SHR_EXIT();
    }

    /* This LT is sharing tcam info with other LT but the first
     * LTID in the list, So need to create tcam info.
     */
    if (ltid < lowest_ltid) {
        (*tcam_info_required) = TRUE;
        SHR_EXIT();
    }

    if (phase == BCMPTM_SUB_PHASE_1) {
        SHR_FUNC_EXIT();
    } else if (phase == BCMPTM_SUB_PHASE_2) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_md_copy(unit, ltid, lowest_ltid));
        if (warm == TRUE) {
            SHR_FUNC_EXIT();
        }
    } else if (phase == BCMPTM_SUB_PHASE_3) {
        if (warm == TRUE) {
            SHR_FUNC_EXIT();
        }
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmptm_rm_tcam_pipe_config_get(unit,
                                           ltid,
                                           ltid_info,
                                           ltid_info->rm_more_info->pipe_count,
                                           &num_pipes));
        for (pipe = 0; pipe < num_pipes; pipe++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                        ltid,
                                                        ltid_info,
                                                        pipe,
                                                        &tcam_info));
                tcam_info->num_lt_sharing++;
        }
        SHR_FUNC_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
rm_tcam_fp_dynamic_lt_update(int unit,
                             uint32_t trans_id,
                             uint32_t ltid)
{
    int rv = 0;
    uint32_t ptrm_size = 0, alloc_sz = 0;
    bcmptm_rm_tcam_md_t *ptr = NULL;
    bcmptm_rm_tcam_md_t *old_ptr = NULL;
    bcmptm_rm_tcam_md_t new_ptr;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    bcmptm_rm_tcam_trans_info_t *new_trans_info = NULL;
    uint8_t sub_id = 0;
    bcmltd_sid_t shr_ltid;
    uint8_t idx = 0;
    uint8_t pipe_count = 0;
    uint8_t pipe = 0;
    uint8_t num_pipes = 0;
    uint32_t size = 0;

    SHR_FUNC_ENTER(unit);
    user_info = 0;

    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit, ltid, 0, 0,
                                      ((void *)&ltid_info),
                                      NULL, NULL));

    /* For IFP/VFP/EFP Presel LT's, no need to update metadata. */
    if (ltid_info.rm_more_info != NULL) {
        if (!((ltid_info.rm_more_info[0].flags & BCMPTM_RM_TCAM_UFT_HASH_BANKS)
             || (ltid_info.dynamic_banks == TRUE &&
             ltid_info.rm_more_info[0].flags & BCMPTM_RM_TCAM_UFT_T4T_BANKS))) {
            SHR_EXIT();
        }
    } else {
        SHR_EXIT();
    }

    if (!((ltid_info.rm_more_info[0].flags & BCMPTM_RM_TCAM_UFT_HASH_BANKS) ||
        (ltid_info.dynamic_banks == TRUE &&
             ltid_info.rm_more_info[0].flags & BCMPTM_RM_TCAM_UFT_T4T_BANKS))) {
        SHR_EXIT();
    }

    /* Initialize lrd mreq_info of this LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_dyn_lt_mreq_info_init(unit,
                                              trans_id,
                                              ltid,
                                              FALSE));

    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit, ltid, 0, 0,
                                      ((void *)&ltid_info),
                                      NULL, NULL));

    if (ltid_info.rm_more_info[0].entry_count == NULL) {
        /* No banks allocated to this LT. */
        rv = bcmptm_rm_tcam_md_get(unit, ltid, &old_ptr);
        if (rv == SHR_E_NOT_FOUND) {
            SHR_EXIT();
        } else {
            /* Free previously allocated memory. */
            for (pipe = 0; pipe < old_ptr->num_pipes; pipe++) {
                trans_info =
                    (bcmptm_rm_tcam_trans_info_t *)old_ptr->backup_ptrs[pipe];
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_ha_header_delete(unit,
                                         old_ptr->backup_sub_component_id[pipe],
                                         trans_info));

                trans_info =
                    (bcmptm_rm_tcam_trans_info_t *)old_ptr->active_ptrs[pipe];
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_ha_header_delete(unit,
                                        old_ptr->active_sub_component_id[pipe],
                                        trans_info));
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_tcam_issu_ha_struct_id_info_clear(
                                      unit,
                                      old_ptr->active_sub_component_id[pipe]));
                SHR_IF_ERR_EXIT
                    (shr_ha_mem_free(unit, old_ptr->active_ptrs[pipe]));
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_tcam_issu_ha_struct_id_info_clear(
                                      unit,
                                      old_ptr->backup_sub_component_id[pipe]));
                SHR_IF_ERR_EXIT
                    (shr_ha_mem_free(unit, old_ptr->backup_ptrs[pipe]));

                old_ptr->active_ptrs[pipe] = NULL;
                old_ptr->backup_ptrs[pipe] = NULL;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_sub_component_free(unit,
                                    old_ptr->backup_sub_component_id[pipe]));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_sub_component_free(unit,
                                    old_ptr->active_sub_component_id[pipe]));
            }
            old_ptr->ptrm_size = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_md_delete(unit, ltid));

            if (ltid_info.rm_more_info->shared_lt_count &&
                ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_UFT_T4T_BANKS) {
                for (idx = 0 ;
                     idx < ltid_info.rm_more_info->shared_lt_count; idx++) {
                    shr_ltid =
                        ltid_info.rm_more_info->shared_lt_info[idx];
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_md_delete(unit, shr_ltid));
                }
            }
        }
    } else {
        /* Calculate HA size required. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_fp_calc_mem_required(unit,
                                                 ltid,
                                                 &ltid_info,
                                                 (void *)&ptrm_size));

        rv = bcmptm_rm_tcam_md_get(unit, ltid, &old_ptr);
        if (rv == SHR_E_NOT_FOUND) {
            /* metadata for this LT has not been initialized yet. */
            num_pipes = ltid_info.rm_more_info[0].pipe_count;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_pipe_config_get(unit,
                                              ltid,
                                              &ltid_info,
                                              num_pipes,
                                              &pipe_count));
            ptr =
                sal_alloc(sizeof(bcmptm_rm_tcam_md_t), "bcmptmRmTcamLtid");
            sal_memset(ptr, 0, sizeof(bcmptm_rm_tcam_md_t));

            /* set LTID */
            ptr->ltid = ltid;
            ptr->num_pipes = pipe_count;
            for (pipe = 0; pipe < ptr->num_pipes; pipe++) {
                /* Allocate the Memory */
                alloc_sz = ptrm_size;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));

                ptr->active_ptrs[pipe] = shr_ha_mem_alloc(unit,
                                                 BCMMGMT_RM_TCAM_COMP_ID,
                                                 sub_id,
                                                 "bcmptmRmTcamLtid",
                                                 &alloc_sz);
                SHR_NULL_CHECK(ptr->active_ptrs[pipe], SHR_E_MEMORY);

                sal_memset(ptr->active_ptrs[pipe], 0, alloc_sz);
                ptr->active_sub_component_id[pipe] = sub_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_ha_header_init(unit,
                                                   ltid,
                                                   sub_id,
                                                   ptr->active_ptrs[pipe],
                                                   pipe,
                                                   1));

                /* Allocate sub component for back up pointers */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));

                /* Allocate the Memory for backup*/
                alloc_sz = ptrm_size;
                ptr->backup_ptrs[pipe] = shr_ha_mem_alloc(unit,
                                                      BCMMGMT_RM_TCAM_COMP_ID,
                                                      sub_id,
                                                      "bcmptmRmTcamLtidBkup",
                                                      &alloc_sz);
                SHR_NULL_CHECK(ptr->backup_ptrs[pipe], SHR_E_MEMORY);
                sal_memset(ptr->backup_ptrs[pipe], 0, alloc_sz);
                ptr->backup_sub_component_id[pipe] = sub_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_ha_header_init(unit,
                                                   ltid,
                                                   sub_id,
                                                   ptr->backup_ptrs[pipe],
                                                   pipe,
                                                   0));
            }
            ptr->ptrm_size = ptrm_size;
            ptr->next = RM_TCAM_MD(unit);
            RM_TCAM_MD(unit) = ptr;

            /*
             * shared_lt_info is initailzed in only main TCAM ltid.
             * this code required to copy main tcam md pointer to
             * presel md pointer.
             */
            if (ltid_info.rm_more_info->shared_lt_count &&
                ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_UFT_T4T_BANKS) {
                for (idx = 0 ;
                     idx < ltid_info.rm_more_info->shared_lt_count; idx++) {
                    shr_ltid =
                        ltid_info.rm_more_info->shared_lt_info[idx];
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_md_copy(unit, shr_ltid, ltid));
                }
            }

            /* Initialize meta data */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_init_metadata(unit,
                                             false,
                                             ltid,
                                             &ltid_info));
        } else {
            /*
             * Metadata already initialized for this LT.
             * Realloc and copy existing data.
             */
            if (ptrm_size == old_ptr->ptrm_size) {
                /* No change to the banks. Return. */
                SHR_EXIT();
            } else {
                num_pipes = ltid_info.rm_more_info[0].pipe_count;

                /* Allocate new HA memory. */
                sal_memset(&new_ptr, 0, sizeof(bcmptm_rm_tcam_md_t));
                /* set LTID */
                new_ptr.ltid = ltid;
                new_ptr.ptrm_size = ptrm_size;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_pipe_config_get(unit,
                                              ltid,
                                              &ltid_info,
                                              num_pipes,
                                              &pipe_count));
                new_ptr.num_pipes = pipe_count;
                for (pipe = 0; pipe < new_ptr.num_pipes; pipe++) {

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
                    /* Allocate the Memory */
                    alloc_sz = ptrm_size;
                    new_ptr.active_ptrs[pipe] = shr_ha_mem_alloc(unit,
                                                        BCMMGMT_RM_TCAM_COMP_ID,
                                                        sub_id,
                                                        "bcmptmRmTcamLtid",
                                                        &alloc_sz);
                    SHR_NULL_CHECK(new_ptr.active_ptrs[pipe], SHR_E_MEMORY);

                    sal_memset(new_ptr.active_ptrs[pipe], 0, alloc_sz);
                    new_ptr.active_sub_component_id[pipe] = sub_id;

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_ha_header_update(unit,
                                         old_ptr->active_sub_component_id[pipe],
                                         sub_id));

                    /* Allocate the Memory for backup*/
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
                    alloc_sz = ptrm_size;
                    new_ptr.backup_ptrs[pipe] = shr_ha_mem_alloc(unit,
                                                        BCMMGMT_RM_TCAM_COMP_ID,
                                                        sub_id,
                                                        "bcmptmRmTcamLtidBkup",
                                                        &alloc_sz);
                    SHR_NULL_CHECK(new_ptr.backup_ptrs[pipe], SHR_E_MEMORY);

                    sal_memset(new_ptr.backup_ptrs[pipe], 0, alloc_sz);
                    new_ptr.backup_sub_component_id[pipe] = sub_id;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_ha_header_update(unit,
                                         old_ptr->backup_sub_component_id[pipe],
                                         sub_id));


                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_fp_init_metadata_cb(unit,
                                       ltid,
                                       &ltid_info,
                                       pipe,
                                       new_ptr.active_ptrs[pipe],
                                       new_ptr.active_sub_component_id[pipe]));

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_fp_init_metadata_cb(unit,
                                        ltid,
                                        &ltid_info,
                                        pipe,
                                        new_ptr.backup_ptrs[pipe],
                                        new_ptr.backup_sub_component_id[pipe]));
                }

                for (pipe = 0; pipe < new_ptr.num_pipes; pipe++) {
                    trans_info =
                      (bcmptm_rm_tcam_trans_info_t *)old_ptr->active_ptrs[pipe];
                    new_trans_info =
                       (bcmptm_rm_tcam_trans_info_t *)new_ptr.active_ptrs[pipe];
                    size = new_trans_info->state_size;
                    sal_memcpy(new_trans_info, trans_info,
                                        sizeof(bcmptm_rm_tcam_trans_info_t));
                    new_trans_info->state_size = size;
                    if (RM_TCAM_HA_HEADER_PREVIOUS(unit) == trans_info) {
                        RM_TCAM_HA_HEADER_PREVIOUS(unit) = new_trans_info;
                    }
                    trans_info =
                      (bcmptm_rm_tcam_trans_info_t *)old_ptr->backup_ptrs[pipe];
                    new_trans_info =
                       (bcmptm_rm_tcam_trans_info_t *)new_ptr.backup_ptrs[pipe];
                    size = new_trans_info->state_size;
                    sal_memcpy(new_trans_info, trans_info,
                                         sizeof(bcmptm_rm_tcam_trans_info_t));
                    new_trans_info->state_size = size;
                    if (RM_TCAM_HA_HEADER_PREVIOUS(unit) == trans_info) {
                        RM_TCAM_HA_HEADER_PREVIOUS(unit) = new_trans_info;
                    }
                }
                for (pipe = 0; pipe < new_ptr.num_pipes; pipe++) {
                    if (old_ptr->ptrm_size != 0) {
                        if (old_ptr->ptrm_size < ptrm_size) {
                            /* Copy data from old HA block to new HA block. */
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmptm_rm_tcam_fp_copy_metadata(unit,
                                                    old_ptr->active_ptrs[pipe],
                                                    new_ptr.active_ptrs[pipe]));
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmptm_rm_tcam_fp_copy_metadata(unit,
                                                    old_ptr->backup_ptrs[pipe],
                                                    new_ptr.backup_ptrs[pipe]));
                        }

                        SHR_IF_ERR_EXIT
                            (bcmptm_rm_tcam_issu_ha_struct_id_info_clear(
                                       unit,
                                       old_ptr->active_sub_component_id[pipe]));
                        SHR_IF_ERR_EXIT
                            (shr_ha_mem_free(unit, old_ptr->active_ptrs[pipe]));
                        SHR_IF_ERR_EXIT
                            (bcmptm_rm_tcam_issu_ha_struct_id_info_clear(
                                       unit,
                                       old_ptr->backup_sub_component_id[pipe]));
                        SHR_IF_ERR_EXIT(shr_ha_mem_free(unit,
                                        old_ptr->backup_ptrs[pipe]));
                        bcmptm_rm_tcam_sub_component_free(unit,
                                old_ptr->active_sub_component_id[pipe]);
                        bcmptm_rm_tcam_sub_component_free(unit,
                                old_ptr->backup_sub_component_id[pipe]);
                    }

                    /*
                     * Update metadata to use the new ptr and free previously
                     * allocated HA memory.
                     */
                    old_ptr->active_ptrs[pipe] = new_ptr.active_ptrs[pipe];
                    old_ptr->backup_ptrs[pipe] = new_ptr.backup_ptrs[pipe];
                    old_ptr->active_sub_component_id[pipe] =
                                new_ptr.active_sub_component_id[pipe];
                    old_ptr->backup_sub_component_id[pipe] =
                                new_ptr.backup_sub_component_id[pipe];
                }
                if (old_ptr->ptrm_size != 0 &&
                    ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_UFT_HASH_BANKS) {
                    /*
                     * Update slice information in expanded slices
                     * for HASH LTs alone
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_rm_tcam_fp_em_slice_info_update(unit,
                                                              ltid,
                                                              &ltid_info));
                }

                old_ptr->ptrm_size = new_ptr.ptrm_size;
                if (ltid_info.rm_more_info->shared_lt_count &&
                    ltid_info.rm_more_info->flags & BCMPTM_RM_TCAM_UFT_T4T_BANKS) {
                    for (idx = 0 ; idx < ltid_info.rm_more_info->shared_lt_count; idx++) {
                        shr_ltid =
                            ltid_info.rm_more_info->shared_lt_info[idx];
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmptm_rm_tcam_md_delete(unit, shr_ltid));
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmptm_rm_tcam_md_copy(unit, shr_ltid, ltid));
                    }
                }

            }
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        if (ptr != NULL) {
            sal_free(ptr);
        }
        LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                 "Dynamic LT info update failed for LTID %d\n"),ltid));
    }
    SHR_FUNC_EXIT();
}

STATIC int
rm_tcam_prio_dynamic_lt_update(int unit,
                               uint32_t ltid,
                               uint8_t *ltid_info_updated)
{
    int rv = 0;
    uint32_t ptrm_size = 0, alloc_sz = 0;
    bcmptm_rm_tcam_md_t *ptr = NULL;
    bcmptm_rm_tcam_md_t *old_ptr = NULL;
    bcmptm_rm_tcam_md_t new_ptr;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    bcmptm_rm_tcam_prio_only_info_t *old_tcam_info = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    bcmptm_rm_tcam_trans_info_t *new_trans_info = NULL;
    uint8_t sub_id = 0;
    uint8_t alloc_needed = 1;
    SHR_FUNC_ENTER(unit);

    user_info = 0;
    /* Initialize lrd mreq_info of this LT. */
    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit, ltid, 0, 0,
                                      ((void *)&ltid_info),
                                      NULL, NULL));
    *ltid_info_updated = 0;
    /* DEFIP Tables */
    /* Calculate HA size required. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_prio_only_calc_mem_required(unit,
                                                    ltid,
                                                    &ltid_info,
                                                    (void *)&ptrm_size));
    rv = bcmptm_rm_tcam_md_get(unit, ltid, &old_ptr);
    if (rv == SHR_E_NOT_FOUND) {
        /* metadata for this LT has not been initialized yet. */
        ptr = sal_alloc(sizeof(bcmptm_rm_tcam_md_t), "bcmptmRmTcamLtid");
        sal_memset(ptr, 0, sizeof(bcmptm_rm_tcam_md_t));
        /* set LTID */
        ptr->ltid = ltid;
        ptr->num_pipes = 1;
        /* Allocate the Memory */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
        alloc_sz = ptrm_size;
        ptr->active_ptrs[0] = shr_ha_mem_alloc(unit,
                                               BCMMGMT_RM_TCAM_COMP_ID,
                                               sub_id,
                                               "bcmptmRmTcamLtidAcPtr",
                                               &alloc_sz);
        SHR_NULL_CHECK(ptr->active_ptrs[0], SHR_E_MEMORY);
        if (alloc_sz < ptrm_size) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        sal_memset(ptr->active_ptrs[0], 0, alloc_sz);
        ptr->active_sub_component_id[0] = sub_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ha_header_init(unit,
                                           ltid,
                                           sub_id,
                                           ptr->active_ptrs[0],
                                           0,
                                           1));
        /* Allocate the Memory for backup*/
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
        alloc_sz = ptrm_size;
        ptr->backup_ptrs[0] = shr_ha_mem_alloc(unit,
                                               BCMMGMT_RM_TCAM_COMP_ID,
                                               sub_id,
                                               "bcmptmRmTcamLtidBuPtr",
                                               &alloc_sz);
        SHR_NULL_CHECK(ptr->backup_ptrs[0], SHR_E_MEMORY);
        if (alloc_sz < ptrm_size) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        sal_memset(ptr->backup_ptrs[0], 0, alloc_sz);
        ptr->backup_sub_component_id[0] = sub_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ha_header_init(unit,
                                           ltid,
                                           sub_id,
                                           ptr->backup_ptrs[0],
                                           0,
                                           0));
        ptr->ptrm_size = ptrm_size;
        ptr->next = RM_TCAM_MD(unit);
        RM_TCAM_MD(unit) = ptr;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_metadata_cb_process(unit,
                                             ltid,
                                             &ltid_info,
                                             ptr->active_ptrs[0],
                                             ptr->active_sub_component_id[0]));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_metadata_cb_process(unit,
                                             ltid,
                                             &ltid_info,
                                             ptr->backup_ptrs[0],
                                             ptr->backup_sub_component_id[0]));
        *ltid_info_updated = 1;
    } else {
        /*
         * Metadata already initialized for this LT.
         * Realloc and copy existing data.
         */
        if (ptrm_size == old_ptr->ptrm_size) {
            SHR_EXIT();
        }
        if (ptrm_size < old_ptr->ptrm_size) {
            /* No change to the banks. Return. */
            alloc_needed = 0;
        }
        /* get number of entries in old tcam info */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                    ltid,
                                                    &ltid_info,
                                                    0,
                                                    &old_tcam_info));
        if (old_tcam_info->num_entries_ltid > 0 &&
            (ptrm_size < old_ptr->ptrm_size)) {
            /* One or more banks deallocated, when entries are present.
             * log error and return. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                            "Banks being deallocated while in use.\n")));
            SHR_EXIT();
        } else {
            /* Allocate new HA memory. */
            sal_memset(&new_ptr, 0, sizeof(bcmptm_rm_tcam_md_t));
            /* set LTID */
            new_ptr.ltid = ltid;
            new_ptr.num_pipes = 1;
            /* Allocate the Memory */
            if (alloc_needed == 1) {
                alloc_sz = ptrm_size;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
                new_ptr.active_ptrs[0] = shr_ha_mem_alloc(unit,
                                                          BCMMGMT_RM_TCAM_COMP_ID,
                                                          sub_id,
                                                          "bcmptmRmTcamLtidAcPtr",
                                                          &alloc_sz);

                SHR_NULL_CHECK(new_ptr.active_ptrs[0], SHR_E_MEMORY);
                if (alloc_sz < ptrm_size) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
                sal_memset(new_ptr.active_ptrs[0], 0, alloc_sz);
                new_ptr.active_sub_component_id[0] = sub_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_ha_header_update(unit,
                                              old_ptr->active_sub_component_id[0],
                                              sub_id));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
                /* Allocate the Memory for backup*/
                alloc_sz = ptrm_size;
                new_ptr.backup_ptrs[0] = shr_ha_mem_alloc(unit,
                                                          BCMMGMT_RM_TCAM_COMP_ID,
                                                          sub_id,
                                                          "bcmptmRmTcamLtidBuPtr",
                                                          &alloc_sz);

                SHR_NULL_CHECK(new_ptr.backup_ptrs[0], SHR_E_MEMORY);
                if (alloc_sz < ptrm_size) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
                sal_memset(new_ptr.backup_ptrs[0], 0, alloc_sz);
                new_ptr.backup_sub_component_id[0] = sub_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_ha_header_update(unit,
                                               old_ptr->backup_sub_component_id[0],
                                               sub_id));
                new_ptr.ptrm_size = ptrm_size;

                trans_info =
                    (bcmptm_rm_tcam_trans_info_t *)old_ptr->active_ptrs[0];
                new_trans_info =
                    (bcmptm_rm_tcam_trans_info_t *)new_ptr.active_ptrs[0];
                sal_memcpy(new_trans_info, trans_info,
                                       sizeof(bcmptm_rm_tcam_trans_info_t));
                if (RM_TCAM_HA_HEADER_PREVIOUS(unit) == trans_info) {
                    RM_TCAM_HA_HEADER_PREVIOUS(unit) = new_trans_info;
                }

                trans_info =
                    (bcmptm_rm_tcam_trans_info_t *)old_ptr->backup_ptrs[0];
                new_trans_info =
                    (bcmptm_rm_tcam_trans_info_t *)new_ptr.backup_ptrs[0];
                sal_memcpy(new_trans_info, trans_info,
                                        sizeof(bcmptm_rm_tcam_trans_info_t));
                if (RM_TCAM_HA_HEADER_PREVIOUS(unit) == trans_info) {
                    RM_TCAM_HA_HEADER_PREVIOUS(unit) = new_trans_info;
                }

            } else {
                new_ptr.active_ptrs[0] = old_ptr->active_ptrs[0];
                new_ptr.active_sub_component_id[0] =
                              old_ptr->active_sub_component_id[0];
                new_ptr.backup_ptrs[0] = old_ptr->backup_ptrs[0];
                new_ptr.backup_sub_component_id[0]=
                              old_ptr->backup_sub_component_id[0];
                new_ptr.ptrm_size = ptrm_size;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_metadata_cb_process(unit,
                                          ltid,
                                          &ltid_info,
                                          new_ptr.active_ptrs[0],
                                          new_ptr.active_sub_component_id[0]));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_metadata_cb_process(unit,
                                          ltid,
                                          &ltid_info,
                                          new_ptr.backup_ptrs[0],
                                          new_ptr.backup_sub_component_id[0]));
            if (alloc_needed == 1) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_tcam_issu_ha_struct_id_info_clear(
                                         unit,
                                         old_ptr->active_sub_component_id[0]));
                shr_ha_mem_free(unit, old_ptr->active_ptrs[0]);
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_tcam_issu_ha_struct_id_info_clear(
                                         unit,
                                         old_ptr->backup_sub_component_id[0]));
                shr_ha_mem_free(unit, old_ptr->backup_ptrs[0]);
                bcmptm_rm_tcam_sub_component_free(unit,
                        old_ptr->active_sub_component_id[0]);
                bcmptm_rm_tcam_sub_component_free(unit,
                        old_ptr->backup_sub_component_id[0]);
            }
            /*
             * Update metadata to use the new ptr and free previously
             * allocated HA memory.
             */
            old_ptr->active_ptrs[0] = new_ptr.active_ptrs[0];
            old_ptr->backup_ptrs[0] = new_ptr.backup_ptrs[0];
            old_ptr->ptrm_size = new_ptr.ptrm_size;
            old_ptr->active_sub_component_id[0] =
                new_ptr.active_sub_component_id[0];
            old_ptr->backup_sub_component_id[0] =
                new_ptr.backup_sub_component_id[0];
            *ltid_info_updated = 1;
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        if (ptr != NULL) {
            sal_free(ptr);
        }
    }
    SHR_FUNC_EXIT();
}

STATIC int
rm_tcam_prio_shared_lt_update(int unit, uint32_t main_ltid)
{
    int rv = 0;
    uint32_t ptrm_size = 0, alloc_sz = 0;
    bcmptm_rm_tcam_md_t *old_ptr = NULL;
    bcmptm_rm_tcam_md_t new_ptr;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    bcmptm_rm_tcam_lt_info_t main_ltid_info;
    bcmptm_rm_tcam_prio_only_info_t *old_tcam_info = NULL;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    bcmptm_rm_tcam_trans_info_t *new_trans_info = NULL;
    uint8_t sub_id = 0;
    uint8_t alloc_needed = 1;
    bcmltd_sid_t ltid = 0;
    uint8_t idx = 0;

    SHR_FUNC_ENTER(unit);

    user_info = 0;
    /* Initialize lrd mreq_info of this LT. */
    sal_memset(&main_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit, main_ltid, 0, 0,
                                      ((void *)&main_ltid_info),
                                      NULL, NULL));

    for (idx = 0; idx < main_ltid_info.rm_more_info->shared_lt_count; idx++) {
        ltid = main_ltid_info.rm_more_info->shared_lt_info[idx];

        if (main_ltid == ltid) {
            continue;
        }

        ptrm_size = 0;
        sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_ptrm_info_get(unit, ltid, 0, 0,
                                      ((void *)&ltid_info),
                                      NULL, NULL));
        /* DEFIP Tables */
        /* Calculate HA size required. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_prio_only_calc_mem_required(unit,
                                                        ltid,
                                                        &ltid_info,
                                                        (void *)&ptrm_size));
        rv = bcmptm_rm_tcam_md_get(unit, ltid, &old_ptr);
        if (rv == SHR_E_NOT_FOUND) {
            /*
             * Metadata for this LT has not been initialized yet.
             * Shr_info will also be  initialized during meta data init.
             * skip this LT.
             */
             continue;
        } else {
            alloc_needed = 1;
            /*
             * Metadata already initialized for this LT.
             * Realloc and copy existing data.
             */
            if (ptrm_size <= old_ptr->ptrm_size) {
                /*
                 * In case of ptrm_size == old_ptr->ptrm_size,
                 * No change to the banks. But shared info of this LT
                 * might have changed because of other shared LT is assigned
                 * with new banks. Need to regenerate full tcam info
                 */
                alloc_needed = 0;
            }

            /* get number of entries in old tcam info */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_tcam_info_get(unit,
                                                        ltid,
                                                        &ltid_info,
                                                        0,
                                                        &old_tcam_info));
            if (old_tcam_info->num_entries_ltid > 0) {
                /*
                 * Some entries are present in shared LT,
                 * while shared info of this LT with other LT's is changed.
                 * log verbose for info. */
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                    "shared LT info is updated when some entries are present\n")));
            }

            /* Allocate new HA memory. */
            sal_memset(&new_ptr, 0, sizeof(bcmptm_rm_tcam_md_t));
            /* set LTID */
            new_ptr.ltid = ltid;
            new_ptr.num_pipes = 1;
            /* Allocate the Memory */
            if (alloc_needed == 1) {
                alloc_sz = ptrm_size;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
                new_ptr.active_ptrs[0] = shr_ha_mem_alloc(unit,
                                                          BCMMGMT_RM_TCAM_COMP_ID,
                                                          sub_id,
                                                          "bcmptmRmTcamLtidAcPtr",
                                                          &alloc_sz);

                SHR_NULL_CHECK(new_ptr.active_ptrs[0], SHR_E_MEMORY);
                if (alloc_sz < ptrm_size) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
                sal_memset(new_ptr.active_ptrs[0], 0, alloc_sz);
                new_ptr.active_sub_component_id[0] = sub_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_ha_header_update(unit,
                                              old_ptr->active_sub_component_id[0],
                                              sub_id));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
                /* Allocate the Memory for backup*/
                alloc_sz = ptrm_size;
                new_ptr.backup_ptrs[0] = shr_ha_mem_alloc(unit,
                                                          BCMMGMT_RM_TCAM_COMP_ID,
                                                          sub_id,
                                                          "bcmptmRmTcamLtidBuPtr",
                                                          &alloc_sz);

                SHR_NULL_CHECK(new_ptr.backup_ptrs[0], SHR_E_MEMORY);
                if (alloc_sz < ptrm_size) {
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
                sal_memset(new_ptr.backup_ptrs[0], 0, alloc_sz);
                new_ptr.backup_sub_component_id[0] = sub_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_ha_header_update(unit,
                                               old_ptr->backup_sub_component_id[0],
                                               sub_id));
                new_ptr.ptrm_size = ptrm_size;

                trans_info =
                    (bcmptm_rm_tcam_trans_info_t *)old_ptr->active_ptrs[0];
                new_trans_info =
                    (bcmptm_rm_tcam_trans_info_t *)new_ptr.active_ptrs[0];
                sal_memcpy(new_trans_info, trans_info,
                                       sizeof(bcmptm_rm_tcam_trans_info_t));
                if (RM_TCAM_HA_HEADER_PREVIOUS(unit) == trans_info) {
                    RM_TCAM_HA_HEADER_PREVIOUS(unit) = new_trans_info;
                }

                trans_info =
                    (bcmptm_rm_tcam_trans_info_t *)old_ptr->backup_ptrs[0];
                new_trans_info =
                    (bcmptm_rm_tcam_trans_info_t *)new_ptr.backup_ptrs[0];
                sal_memcpy(new_trans_info, trans_info,
                                        sizeof(bcmptm_rm_tcam_trans_info_t));
                if (RM_TCAM_HA_HEADER_PREVIOUS(unit) == trans_info) {
                    RM_TCAM_HA_HEADER_PREVIOUS(unit) = new_trans_info;
                }

            } else {
                new_ptr.active_ptrs[0] = old_ptr->active_ptrs[0];
                new_ptr.active_sub_component_id[0] =
                              old_ptr->active_sub_component_id[0];
                new_ptr.backup_ptrs[0] = old_ptr->backup_ptrs[0];
                new_ptr.backup_sub_component_id[0]=
                              old_ptr->backup_sub_component_id[0];
                new_ptr.ptrm_size = ptrm_size;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_metadata_cb_process(unit,
                                          ltid,
                                          &ltid_info,
                                          new_ptr.active_ptrs[0],
                                          new_ptr.active_sub_component_id[0]));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_prio_only_metadata_cb_process(unit,
                                          ltid,
                                          &ltid_info,
                                          new_ptr.backup_ptrs[0],
                                          new_ptr.backup_sub_component_id[0]));
            if (alloc_needed == 1) {
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_tcam_issu_ha_struct_id_info_clear(
                                         unit,
                                         old_ptr->active_sub_component_id[0]));
                shr_ha_mem_free(unit, old_ptr->active_ptrs[0]);
                SHR_IF_ERR_EXIT
                    (bcmptm_rm_tcam_issu_ha_struct_id_info_clear(
                                         unit,
                                         old_ptr->backup_sub_component_id[0]));
                shr_ha_mem_free(unit, old_ptr->backup_ptrs[0]);
                bcmptm_rm_tcam_sub_component_free(unit,
                        old_ptr->active_sub_component_id[0]);
                bcmptm_rm_tcam_sub_component_free(unit,
                        old_ptr->backup_sub_component_id[0]);
            }
            /*
             * Update metadata to use the new ptr and free previously
             * allocated HA memory.
             */
            old_ptr->active_ptrs[0] = new_ptr.active_ptrs[0];
            old_ptr->backup_ptrs[0] = new_ptr.backup_ptrs[0];
            old_ptr->ptrm_size = new_ptr.ptrm_size;
            old_ptr->active_sub_component_id[0] =
                new_ptr.active_sub_component_id[0];
            old_ptr->backup_sub_component_id[0] =
                new_ptr.backup_sub_component_id[0];
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static void
rm_tcam_grp_change_event_handler(int unit, const char *event, uint64_t ev_data)
{
    int rv = 0;
    const lt_mreq_info_t *lrd_info = NULL;
    bcmptm_rm_tcam_dyn_ptrm_table_t *dyn_tbl = NULL;
    uint8_t ltid_info_updated = 0;

    dyn_tbl = RM_TCAM_DYN_PTRM_TBL(unit);
    /* Validate banks info */
    while (dyn_tbl != NULL) {
        lrd_info = dyn_tbl->handler;
        if (lrd_info == NULL) {
            dyn_tbl = dyn_tbl->next;
            continue;
        }

        if (lrd_info->pt_type == LT_PT_TYPE_FP) {
            dyn_tbl = dyn_tbl->next;
            continue;
        }

        rv = bcmptm_rm_tcam_dynamic_lt_validate(unit, dyn_tbl->sid);
        if (rv != 0) {
            SHR_EXIT();
        }
        dyn_tbl = dyn_tbl->next;
    }
    /* update */
    /* Initialize lrd mreq_info of this LT. */
    dyn_tbl = RM_TCAM_DYN_PTRM_TBL(unit);
    while (dyn_tbl != NULL) {
        lrd_info = dyn_tbl->handler;
        if (lrd_info == NULL) {
            dyn_tbl = dyn_tbl->next;
            continue;
        }
        if (lrd_info->pt_type != LT_PT_TYPE_FP) {
            rv = bcmptm_rm_tcam_dyn_lt_mreq_info_init(unit,
                                    BCMPTM_UFT_EV_INFO_TRANS_ID_GET(ev_data),
                                                      dyn_tbl->sid,
                                                      FALSE);
        }
        if (rv != 0) {
            SHR_EXIT();
        }
        dyn_tbl = dyn_tbl->next;
    }
    dyn_tbl = RM_TCAM_DYN_PTRM_TBL(unit);
    while (dyn_tbl != NULL) {
        lrd_info = dyn_tbl->handler;
        if (lrd_info == NULL) {
            dyn_tbl = dyn_tbl->next;
            continue;
        }
        if (lrd_info->pt_type == LT_PT_TYPE_FP) {
            if (lrd_info->dynamic_banks == 1) {
                rv = rm_tcam_fp_dynamic_lt_update(unit,
                                   BCMPTM_UFT_EV_INFO_TRANS_ID_GET(ev_data),
                                                  dyn_tbl->sid);
            } else {
                dyn_tbl = dyn_tbl->next;
                continue;
            }
        } else {
            ltid_info_updated = 0;
            rv = rm_tcam_prio_dynamic_lt_update(unit,
                                                dyn_tbl->sid,
                                                &ltid_info_updated);
            if ((rv == 0) && (ltid_info_updated == 1)) {
                /*
                 * Update all the shared LT info's too as it will
                 * regenerate the shared info information per LT for all
                 * the shared LT's.
                 */
                rv = rm_tcam_prio_shared_lt_update(unit,
                                                   dyn_tbl->sid);
            }
        }
        if (rv != 0) {
            SHR_EXIT();
        }
        dyn_tbl = dyn_tbl->next;
    }

exit:
    return;
}

static int
bcmptm_rm_tcam_event_handle_init(int unit)
{

    SHR_FUNC_ENTER(unit);
    /*
     * Register handler to handle bank count, bank offset and
     * robust configuration change.
     */
    SHR_IF_ERR_EXIT(
        bcmevm_register_published_event(unit,
                                        BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                        rm_tcam_grp_change_event_handler));
exit:
    SHR_FUNC_EXIT();
}


static int
bcmptm_rm_tcam_event_handle_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmevm_unregister_published_event(unit,
                                          BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                          rm_tcam_grp_change_event_handler));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
/*
 * Get the HA pointer to tcam information of given TCAM LTID.
 */
int
bcmptm_rm_tcam_info_get(int unit,
                        bcmltd_sid_t ltid,
                        int pipe,
                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                        void **tcam_info)
{
    bcmptm_rm_tcam_md_t *lt_md = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ltid_info, SHR_E_PARAM);
    SHR_NULL_CHECK(tcam_info, SHR_E_PARAM);

    *tcam_info = NULL;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_md_get(unit, ltid, &lt_md));

    if (pipe == -1) {
        *tcam_info = lt_md->active_ptrs[0];
    } else {
        *tcam_info = lt_md->active_ptrs[pipe];
    }

    if (*tcam_info == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit, "tcam info for req_ltid=%0d, pipe= %0d is NULL!\n"),
             ltid, pipe));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_stop(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmptm_rm_tcam_event_handle_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Cleanup the TCAM resource manager. Free memory created
 * for traverse. RM shouldn't free HA memory. It is applications
 * resposniblity to free HA memory.
 */
int
bcmptm_rm_tcam_cleanup(int unit)
{
    bcmptm_rm_tcam_md_t *md = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmptm_rm_tcam_traverse_info_delete_all(unit));

    md = RM_TCAM_MD(unit);
    while (md != NULL) {
         md = RM_TCAM_MD(unit);
         RM_TCAM_MD(unit) = md->next;
         sal_free(md);
         md = RM_TCAM_MD(unit);
    }
    RM_TCAM_MD(unit) = NULL;
    RM_TCAM_INFO_GLOBAL(unit) = NULL;
    RM_TCAM_HA_HEADER_PREVIOUS(unit) = NULL;

    bcmptm_rm_tcam_sub_component_init(unit);
    user_info = 0;
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_dyn_ptrm_tbl_cleanup(int unit)
{
    bcmptm_rm_tcam_dyn_ptrm_table_t *dyn_tbl = NULL;

    SHR_FUNC_ENTER(unit);

    dyn_tbl = RM_TCAM_DYN_PTRM_TBL(unit);
    while (dyn_tbl != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_dyn_lt_mreq_info_cleanup(unit,
                                                     dyn_tbl->sid));
         RM_TCAM_DYN_PTRM_TBL(unit) = dyn_tbl->next;
         dyn_tbl->sid = 0;
         dyn_tbl->handler = NULL;
         dyn_tbl->next = NULL;
         sal_free(dyn_tbl);
         dyn_tbl = RM_TCAM_DYN_PTRM_TBL(unit);
    }
    RM_TCAM_DYN_PTRM_TBL(unit) = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_init0(int unit)
{
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);
    rm_tcam_md[unit] = NULL;
    rm_tcam_global_info[unit] = NULL;
    SHR_FUNC_EXIT();
}

/*
 * Initialize the TCAM Resource Manager. It involves
 * 1. Calculating the total size required to store SW state of all TCAM LTIDs
 * 2. All memory.
 * 3. Intialize the SW state of all TCAM LTIDs.
 */
int
bcmptm_rm_tcam_init(int unit)
{
    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_event_handle_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT(bcmptm_rm_tcam_trans_state_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_comp_config(int unit,
                           bool warm,
                           bcmptm_sub_phase_t phase)
{
    return SHR_E_NONE;
}

STATIC int
bcmptm_rm_tcam_global_init_cb(int unit,
                              bcmptm_sub_phase_t phase)
{
    uint32_t size = 0, req_size = 0;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    uint8_t sub_id = 0;

    SHR_FUNC_ENTER(unit);

    if (phase == BCMPTM_SUB_PHASE_1) {
        SHR_IF_ERR_EXIT
            (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
        req_size = size = sizeof(bcmptm_rm_tcam_info_global_t);
        RM_TCAM_INFO_GLOBAL(unit) =
                shr_ha_mem_alloc(unit,
                                 BCMMGMT_RM_TCAM_COMP_ID,
                                 sub_id,
                                 "bcmptmRmTcamGlobInfo",
                                 &size);
        SHR_NULL_CHECK(RM_TCAM_INFO_GLOBAL(unit), SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT
            ((size < req_size) ? SHR_E_MEMORY : SHR_E_NONE);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit,
                                        BCMMGMT_RM_TCAM_COMP_ID,
                                        sub_id,
                                        0,
                                        sizeof(bcmptm_rm_tcam_trans_info_t), 1,
                                        BCMPTM_RM_TCAM_TRANS_INFO_T_ID));
        trans_info = &(RM_TCAM_INFO_GLOBAL(unit)->trans_info);
        trans_info->sign_word = BCMPTM_RM_TCAM_HA_SIGN_GLOBAL_INFO;
        trans_info->state_size = size;
        trans_info->trans_state = BCMPTM_RM_TCAM_STATE_IDLE;
    } else if (phase == BCMPTM_SUB_PHASE_2 ||
               phase == BCMPTM_SUB_PHASE_3) {
        trans_info = &(RM_TCAM_INFO_GLOBAL(unit)->trans_info);
        if (trans_info->sign_word !=
            BCMPTM_RM_TCAM_HA_SIGN_GLOBAL_INFO) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        size = sizeof(bcmptm_rm_tcam_info_global_t);
        if (trans_info->state_size < size) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        if (trans_info->trans_state != BCMPTM_RM_TCAM_STATE_IDLE) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmptm_rm_tcam_global_init_wb(int unit,
                              bcmptm_sub_phase_t phase)
{
    uint32_t size = 0;
    uint32_t ptrm_size = 0;
    void *ha_ptr = NULL;
    uint8_t sub_comp_id = 0;
    int pipe = -1;
    bcmptm_rm_tcam_trans_info_t *trans_info = NULL;
    bcmptm_rm_tcam_trans_state_t trans_state;
    bcmptm_rm_tcam_md_t *tcam_md = NULL;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    uint8_t num_pipes = 0;
    uint8_t pipe_count = 0;

    SHR_FUNC_ENTER(unit);

    if (phase == BCMPTM_SUB_PHASE_1) {

        size = sizeof(bcmptm_rm_tcam_info_global_t);
        RM_TCAM_INFO_GLOBAL(unit) =
                    shr_ha_mem_alloc(unit,
                                     BCMMGMT_RM_TCAM_COMP_ID,
                                     sub_comp_id,
                                     "bcmptmRmTcamGlobInfo",
                                     &size);
        trans_info = &(RM_TCAM_INFO_GLOBAL(unit)->trans_info);
        if (trans_info->sign_word !=
            BCMPTM_RM_TCAM_HA_SIGN_GLOBAL_INFO) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        trans_state = trans_info->trans_state;
        if (!((trans_state == BCMPTM_RM_TCAM_STATE_IDLE) ||
             (trans_state == BCMPTM_RM_TCAM_STATE_UC_A) ||
             (trans_state == BCMPTM_RM_TCAM_STATE_COPY_A_TO_B) ||
             (trans_state == BCMPTM_RM_TCAM_STATE_COPY_B_TO_A))) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_sub_component_set(unit,sub_comp_id));

        sub_comp_id = 1;
        while (sub_comp_id) {
            size = 0;
            ha_ptr = shr_ha_mem_alloc(unit,
                                      BCMMGMT_RM_TCAM_COMP_ID,
                                      sub_comp_id,
                                      "bcmptmRmTcamTransInfo",
                                      &size);
            trans_info = ha_ptr;
            tcam_md = NULL;
            _func_rv = bcmptm_rm_tcam_md_get(unit,
                                             trans_info->tbl_id,
                                             &tcam_md);
            SHR_IF_ERR_EXIT_EXCEPT_IF(_func_rv, SHR_E_NOT_FOUND);
            if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
                tcam_md = sal_alloc(sizeof(bcmptm_rm_tcam_md_t),
                                    "bcmptmRmTcamMd");
                sal_memset(tcam_md, 0, sizeof(bcmptm_rm_tcam_md_t));
                tcam_md->next = RM_TCAM_MD(unit);
                RM_TCAM_MD(unit) = tcam_md;
            }
            tcam_md->ltid = trans_info->tbl_id;
            sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_ptrm_info_get(unit,
                                              tcam_md->ltid,
                                              0,
                                              0,
                                              &ltid_info,
                                              NULL,
                                              NULL));
            pipe = trans_info->tbl_inst;
            if (trans_info->active == TRUE) {
                tcam_md->active_ptrs[pipe] = ha_ptr;
                tcam_md->active_sub_component_id[pipe] = sub_comp_id;
            } else {
                tcam_md->backup_ptrs[pipe] = ha_ptr;
                tcam_md->backup_sub_component_id[pipe] = sub_comp_id;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_sub_component_set(unit,sub_comp_id));
            RM_TCAM_HA_HEADER_PREVIOUS(unit) = trans_info;
            sub_comp_id = trans_info->next_sub_id;
        }
    } else if (phase == BCMPTM_SUB_PHASE_2) {
        tcam_md = RM_TCAM_MD(unit);
        while (tcam_md != NULL) {
            sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_ptrm_info_get(unit,
                                      tcam_md->ltid,
                                      0,
                                      0,
                                      ((void *)&ltid_info),
                                      NULL,
                                      NULL));
            num_pipes = ltid_info.rm_more_info[0].pipe_count;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_pipe_config_get(unit,
                                              tcam_md->ltid,
                                              &ltid_info,
                                              num_pipes,
                                              &pipe_count));
            tcam_md->num_pipes = pipe_count;

            ptrm_size = 0;
            if (ltid_info.pt_type == LT_PT_TYPE_FP) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_fp_calc_mem_required(unit,
                                                       tcam_md->ltid,
                                                       &ltid_info,
                                                       (void *)&ptrm_size));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_tcam_calc_mem_required(unit,
                                                      tcam_md->ltid,
                                                      &ltid_info,
                                                      (void *)&ptrm_size));
            }
            tcam_md->ptrm_size = ptrm_size;
            tcam_md = tcam_md->next;
        }
    } else {
        SHR_EXIT();
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_um_comp_config(int unit,
                              bool warm,
                              bcmptm_sub_phase_t phase)
{
    SHR_FUNC_ENTER(unit);

    if (warm == FALSE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_global_init_cb(unit, phase));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_global_init_wb(unit, phase));
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmptm_rm_tcam_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_ltid_tcam_info_shared_check(int unit,
                              bcmptm_rm_tcam_lt_info_t *ltid_info1,
                              bcmptm_rm_tcam_lt_info_t *ltid_info2,
                              bool *shared,
                              bool *sid_rows_diff)
{
    uint8_t num_key_rows = 0;
    uint8_t num_sid_rows1 = 0;
    uint8_t num_sid_rows2 = 0;
    uint8_t depth = 0;
    uint8_t key_row = 0;
    bcmdrd_sid_t sid;
    bcmdrd_sid_t shr_sid;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(ltid_info1, SHR_E_PARAM);
    SHR_NULL_CHECK(ltid_info2, SHR_E_PARAM);
    SHR_NULL_CHECK(shared, SHR_E_PARAM);

    (*shared) = TRUE;

    /* If shared LT and this LT has different depth instances
     * then create new TCAM info.
     */
    if (ltid_info2->hw_entry_info[0].num_depth_inst !=
        ltid_info1->hw_entry_info[0].num_depth_inst) {
        (*shared) = FALSE;
        SHR_EXIT();
    }

    /* Depth instances are same but physical memories are
     * different. LTs sharing the same Tile/TileArray as the
     * incoming LT are in shared LT list of incoming LT. LTs
     * sharing the same Tile/TileArray may or may not share
     * the TCAMs in the Tile/TileArray.
     */
    if (((ltid_info1->rm_more_info->flags & BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) ||
        (ltid_info2->rm_more_info->flags & BCMPTM_RM_TCAM_F_DEPTH_EXPANSION)) &&
        (ltid_info1->hw_entry_info->num_key_rows !=
         ltid_info2->hw_entry_info->num_key_rows)){
        /*
         * In depth expanded case we can keep same entry info For this we need to
         * remove the logic based on reserved index in tcam info.
         * Currently keeping it simple by having seperate TCAM info and entry info
         * Can revist this later if needed
         */
        (*shared) = FALSE;
        SHR_EXIT();
    }

    if (ltid_info1->rm_more_info->flags &
        BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
        num_sid_rows1 = 1;
    } else {
        num_sid_rows1 = ltid_info1->hw_entry_info->num_key_rows;
    }

    if (ltid_info2->rm_more_info->flags &
        BCMPTM_RM_TCAM_F_DEPTH_EXPANSION) {
        num_sid_rows2 = 1;
    } else {
        num_sid_rows2 = ltid_info2->hw_entry_info->num_key_rows;
    }

    if (num_sid_rows1 < num_sid_rows2) {
        num_key_rows = num_sid_rows1;
    } else {
        num_key_rows = num_sid_rows2;
    }
    if (num_sid_rows1 != num_sid_rows2) {
        *sid_rows_diff = TRUE;
    }
    /*
     * View type is set when LTs share same hardware resource, but with
     * different views(sids are different for the sharing LTs). So skipping the
     * below check as it fails.
     */

    for (depth = 0;
        depth < ltid_info1->hw_entry_info[0].num_depth_inst;
        depth++) {
        for (key_row = 0; key_row < num_key_rows; key_row++) {
            sid = ltid_info1->hw_entry_info[0].sid[depth][key_row];
            shr_sid = ltid_info2->hw_entry_info[0].sid[depth][key_row];
            if (sid != shr_sid) {
                (*shared) = FALSE;
                SHR_EXIT();
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_lt_reg(int unit,
                      bcmltd_sid_t ltid,
                      bool warm,
                      bcmptm_sub_phase_t phase,
                      uint32_t *lt_state_offset)
{
    bcmptm_rm_tcam_lt_info_t ltid_info;
    bool tcam_info_required = FALSE;
    uint32_t ptrm_size = 0;
    bcmptm_rm_tcam_md_t *tcam_md = NULL;
    uint32_t alloc_sz = 0;
    uint8_t sub_id = 0;
    uint8_t pipe_count = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lt_state_offset, SHR_E_PARAM);

    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit,
                                      ltid,
                                      0,
                                      0,
                                      ((void *)&ltid_info),
                                      NULL,
                                      NULL));

    *lt_state_offset = 0;
    user_info = 0;

    /*
     * For LT's sharing same TCAM, mem required and metadata
     * init needs to be done for the first LT. Other LT's will
     * use the LT same offset.
     */
    if (ltid_info.rm_more_info->shared_lt_count != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_shared_lt_init(unit,
                                           ltid,
                                           &ltid_info,
                                           warm,
                                           phase,
                                           lt_state_offset,
                                           &tcam_info_required));
        if (tcam_info_required == FALSE) {
            SHR_FUNC_EXIT();
        }
    }

    if (phase == BCMPTM_SUB_PHASE_1 && warm == FALSE) {

        uint8_t pipe = 0;
        uint8_t num_pipes = 0;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_calc_mem_required(unit,
                                              ltid,
                                              &ltid_info,
                                              (void *)&ptrm_size));

        num_pipes = ltid_info.rm_more_info[0].pipe_count;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_pipe_config_get(unit,
                                              ltid,
                                              &ltid_info,
                                              num_pipes,
                                              &pipe_count));
        tcam_md = sal_alloc(sizeof(bcmptm_rm_tcam_md_t),
                            "bcmptmRmTcamMd");
        sal_memset(tcam_md, 0, sizeof(bcmptm_rm_tcam_md_t));

        tcam_md->ltid = ltid;
        tcam_md->num_pipes = pipe_count;
        alloc_sz = ptrm_size;
        for (pipe = 0; pipe < tcam_md->num_pipes; pipe++) {
            /* Allocate the Memory for active state. */
            alloc_sz = ptrm_size;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
            tcam_md->active_ptrs[pipe] =
                               shr_ha_mem_alloc(unit,
                                                BCMMGMT_RM_TCAM_COMP_ID,
                                                sub_id,
                                                "bcmptmRmTcamMdAcPtr",
                                                &alloc_sz);
            SHR_NULL_CHECK(tcam_md->active_ptrs[pipe], SHR_E_MEMORY);
            tcam_md->active_sub_component_id[pipe] = sub_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_ha_header_init(unit,
                                               ltid,
                                               sub_id,
                                               tcam_md->active_ptrs[pipe],
                                               pipe,
                                               1));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
            /* Allocate the Memory for backup state. */
            alloc_sz = ptrm_size;
            tcam_md->backup_ptrs[pipe] =
                               shr_ha_mem_alloc(unit,
                                                BCMMGMT_RM_TCAM_COMP_ID,
                                                sub_id,
                                                "bcmptmRmTcamMdBuPtr",
                                                &alloc_sz);
            SHR_NULL_CHECK(tcam_md->backup_ptrs[pipe], SHR_E_MEMORY);
            tcam_md->backup_sub_component_id[pipe] = sub_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_ha_header_init(unit,
                                               ltid,
                                               sub_id,
                                               tcam_md->backup_ptrs[pipe],
                                               pipe,
                                               0));
        }
        tcam_md->ptrm_size = ptrm_size;
        tcam_md->next = RM_TCAM_MD(unit);
        RM_TCAM_MD(unit) = tcam_md;
    } else if (phase == BCMPTM_SUB_PHASE_2) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_rm_tcam_init_metadata(unit,
                                          warm,
                                          ltid,
                                          &ltid_info,
                                          (void *)&user_info));
    }

    SHR_FUNC_EXIT();
exit:
    (void)bcmptm_rm_tcam_cleanup(unit);
    /* coverity[leaked_storage] */
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_um_lt_reg(int unit,
                         bcmltd_sid_t ltid,
                         bool warm,
                         bcmptm_sub_phase_t phase,
                         uint32_t *lt_state_offset)
{

    int rv = 0;
    uint8_t idx;
    uint8_t shared_count = 0;
    bcmptm_rm_tcam_lt_info_t ltid_info;
    uint32_t ptrm_size = 0;
    uint32_t alloc_sz = 0;
    bcmptm_rm_tcam_md_t *ptr = NULL;
    bcmltd_sid_t shr_ltid;
    uint8_t sub_id = 0;
    uint8_t pipe_count = 0;

    /* Log the  function entry. */
    SHR_FUNC_ENTER(unit);

    /* Input parameter check. */
    SHR_NULL_CHECK(lt_state_offset, SHR_E_PARAM);

    sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit,
                                      ltid,
                                      0,
                                      0,
                                      ((void *)&ltid_info),
                                      NULL,
                                      NULL));
    *lt_state_offset = 0;
    user_info = 0;

    if (ltid_info.pt_type == LT_PT_TYPE_FP) {
        /*
         * The offset for Presel and IFP stage are the same,
         * hence we need to avoid re initialization
         */
        if ((ltid_info.rm_more_info[0].fp_flags)
            & BCMPTM_STAGE_KEY_TYPE_PRESEL
            && !((ltid_info.rm_more_info[0].fp_flags)
                 & BCMPTM_STAGE_ENTRY_TYPE_HASH)) {
            /*
             * shared_lt_info is initailzed in only main TCAM ltid.
             * this code required when presel ltid value is smaller than
             * main tcam ltid.
             */
            if (ltid_info.rm_more_info->shared_lt_count) {
                shared_count = ltid_info.rm_more_info->shared_lt_count;
                for (idx = 0 ; idx < shared_count ; idx++) {
                    if (ltid_info.rm_more_info->shared_lt_info[idx] < ltid) {
                        if (phase == BCMPTM_SUB_PHASE_1) {
                            SHR_FUNC_EXIT();
                        } else if (phase == BCMPTM_SUB_PHASE_2) {
                            shr_ltid =
                                ltid_info.rm_more_info->shared_lt_info[idx];
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmptm_rm_tcam_md_copy(unit, ltid, shr_ltid));
                            SHR_FUNC_EXIT();
                        }
                    }
                }
            }

            /*
             * For presel consider main tcam lt only.
             * if presel ltid value is smaller than main TCAM ltid,
             * init or calculate memory based on main TCAM ltid.
             */
            if (ltid_info.rm_more_info->main_lt_info) {
                if (ltid_info.rm_more_info->main_lt_info[0] < ltid) {
                    if (phase == BCMPTM_SUB_PHASE_1) {
                        SHR_FUNC_EXIT();
                    } else if (phase == BCMPTM_SUB_PHASE_2) {
                        shr_ltid =
                            ltid_info.rm_more_info->main_lt_info[0];

                       /*
                        * For dynamic LT's tcam_md_get wil fail. both presel and
                        * main LT are initialized when resources are allocated.
                        */
                        rv = bcmptm_rm_tcam_md_get(unit, shr_ltid, &ptr);
                        if (rv == SHR_E_NOT_FOUND) {
                            SHR_FUNC_EXIT();
                        }
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmptm_rm_tcam_md_copy(unit, ltid, shr_ltid));
                        SHR_FUNC_EXIT();
                    }
                }
                /*
                 * Main TCAM LT was not initialized, initalize it now.
                 */
                ltid = ltid_info.rm_more_info->main_lt_info[0];
                sal_memset(&ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmptm_rm_tcam_ptrm_info_get(unit, ltid, 0, 0,
                                                 ((void *)&ltid_info),
                                                 NULL, NULL));
            }
        }

        if (phase == BCMPTM_SUB_PHASE_1 && warm == FALSE) {

            uint8_t pipe = 0;
            uint8_t num_pipes = 0;

            num_pipes = ltid_info.rm_more_info[0].pipe_count;
            rv = bcmptm_rm_tcam_fp_calc_mem_required(unit,
                                           ltid, &ltid_info,
                                           (void *)&ptrm_size);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_pipe_config_get(unit,
                                              ltid,
                                              &ltid_info,
                                              num_pipes,
                                              &pipe_count));
            ptr = sal_alloc(sizeof(bcmptm_rm_tcam_md_t), "bcmptmRmTcamLtid");
            sal_memset(ptr, 0, sizeof(bcmptm_rm_tcam_md_t));
            ptr->next = RM_TCAM_MD(unit);
            RM_TCAM_MD(unit) = ptr;

            /* set LTID */
            ptr->ltid = ltid;
            ptr->num_pipes = pipe_count;
            /* Allocate the Memory */
            alloc_sz = ptrm_size;
            for (pipe = 0; pipe < ptr->num_pipes; pipe++) {
            alloc_sz = ptrm_size;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
            ptr->active_ptrs[pipe] = shr_ha_mem_alloc(unit,
                                             BCMMGMT_RM_TCAM_COMP_ID,
                                             sub_id,
                                             "bcmptmRmTcamLtid",
                                             &alloc_sz);

            SHR_NULL_CHECK(ptr->active_ptrs[pipe], SHR_E_MEMORY);
            ptr->active_sub_component_id[pipe] = sub_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_ha_header_init(unit,
                                               ltid,
                                               sub_id,
                                               ptr->active_ptrs[pipe],
                                               pipe,
                                               1));
            /* Allocate the Memory for backup*/
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_sub_component_allocate(unit,&sub_id));
            alloc_sz = ptrm_size;
            ptr->backup_ptrs[pipe] = shr_ha_mem_alloc(unit,
                                                 BCMMGMT_RM_TCAM_COMP_ID,
                                                 sub_id,
                                                 "bcmptmRmTcamLtidkup",
                                                 &alloc_sz);

            SHR_NULL_CHECK(ptr->backup_ptrs[pipe], SHR_E_MEMORY);
            ptr->backup_sub_component_id[pipe] = sub_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_ha_header_init(unit,
                                               ltid,
                                               sub_id,
                                               ptr->backup_ptrs[pipe],
                                               pipe,
                                               0));
            }
            ptr->ptrm_size = ptrm_size;
            ptr = NULL;
        } else if (phase == BCMPTM_SUB_PHASE_2) {
           /*
            * For dynamic LT's tcam_md_get wil fail. both presel and
            * main LT are initialized when resources are allocated.
            */
            rv = bcmptm_rm_tcam_md_get(unit, ltid, &ptr);
            if (rv == SHR_E_NOT_FOUND) {
                SHR_FUNC_EXIT();
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_rm_tcam_fp_init_metadata(unit,
                                                 warm,
                                                 ltid,
                                                &ltid_info));
        }
    }

    SHR_FUNC_EXIT();

exit:
    (void)bcmptm_rm_tcam_cleanup(unit);
    SHR_FUNC_EXIT();
}

/* Copy HA info from one LT to another for sharing LT */
int
bcmptm_rm_tcam_md_copy(int unit,
                       bcmltd_sid_t ltid,
                       bcmltd_sid_t shr_ltid)
{
    bcmptm_rm_tcam_md_t *md = RM_TCAM_MD(unit);
    bcmptm_rm_tcam_md_t *md_new = NULL;

    SHR_FUNC_ENTER(unit);

    md_new = sal_alloc(sizeof(bcmptm_rm_tcam_md_t), "bcmptmRmTcamMd");
    sal_memset(md_new, 0, sizeof(bcmptm_rm_tcam_md_t));

    while (NULL != md) {
        if (md->ltid == shr_ltid) {
            sal_memcpy(md_new, md, sizeof(bcmptm_rm_tcam_md_t));
            break;
        }
        md = md->next;
    }

    md_new->ltid = ltid;
    md_new->next = RM_TCAM_MD(unit);
    RM_TCAM_MD(unit) = md_new;

    SHR_FUNC_EXIT();
}

/* Fetch pointer to corresponding LT info HA block. */
int
bcmptm_rm_tcam_md_get(int unit,
                      bcmltd_sid_t ltid,
                      bcmptm_rm_tcam_md_t **tcam_md)
{
    bcmptm_rm_tcam_md_t *md = RM_TCAM_MD(unit);

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tcam_md, SHR_E_PARAM);

    while (NULL != md) {
        if (md->ltid == ltid) {
            *tcam_md = md;
            break;
        }
        md = md->next;
   }

   if (*tcam_md == NULL) {
       SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
   }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_md_delete(int unit,
                         bcmltd_sid_t ltid)
{
    bcmptm_rm_tcam_md_t *md = RM_TCAM_MD(unit);
    bcmptm_rm_tcam_md_t *prev = RM_TCAM_MD(unit);

    SHR_FUNC_ENTER(unit);

    while (NULL != md) {
        if (md->ltid == ltid) {
            if (md == RM_TCAM_MD(unit)) {
                RM_TCAM_MD(unit) = md->next;
            } else {
                prev->next = md->next;
            }
            md->next = NULL;
            sal_free(md);
            break;
        }
        prev = md;
        md = md->next;
   }

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_dyn_ptrm_tbl_global_get(int unit,
                               bcmptm_rm_tcam_dyn_ptrm_table_t **dyn_tbl)
{

    *dyn_tbl = RM_TCAM_DYN_PTRM_TBL(unit);
    return SHR_E_NONE;
}

int
bcmptm_rm_tcam_dyn_ptrm_tbl_get(int unit,
                                bcmltd_sid_t ltid,
                                lt_mreq_info_t **mreq_info)
{
    bcmptm_rm_tcam_dyn_ptrm_table_t *dyn_tbl = RM_TCAM_DYN_PTRM_TBL(unit);

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mreq_info, SHR_E_PARAM);

    while (NULL != dyn_tbl) {
        if (dyn_tbl->sid == ltid) {
            *mreq_info = dyn_tbl->handler;
            break;
        }
        dyn_tbl = dyn_tbl->next;
    }

    if (*mreq_info == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    } else {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_ptrm_info_get(int unit,
                             bcmltd_sid_t req_ltid,
                             uint16_t dfield_format_id,
                             bool dfield_format_id_valid,
                             void *rsp_ptrm_info,
                             int *rsp_index_hw_entry_info,
                             uint16_t **rsp_hw_data_type_val)
{
    int rv = 0;
    bcmptm_rm_tcam_lt_info_t *rm_tcam_ptrm_info;
    const lt_mreq_info_t *lrd_info = NULL;
    lt_mreq_info_t *dyn_lrd_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Param check */
    SHR_NULL_CHECK(rsp_ptrm_info, SHR_E_PARAM);
    if (dfield_format_id_valid) {
        SHR_NULL_CHECK(rsp_index_hw_entry_info, SHR_E_PARAM);
        SHR_NULL_CHECK(rsp_hw_data_type_val, SHR_E_PARAM);
        SHR_NULL_CHECK(*rsp_hw_data_type_val, SHR_E_PARAM);
    }

    rv = bcmptm_rm_tcam_dyn_ptrm_tbl_get(unit,
                                         req_ltid,
                                         &dyn_lrd_info);
    if (rv == SHR_E_NONE) {
        lrd_info = dyn_lrd_info;
    } else {
        /* Query for lrd_info */
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmlrd_lt_mreq_info_get(unit,
                                    req_ltid,
                                    &lrd_info));
    }

    SHR_NULL_CHECK(lrd_info, SHR_E_FAIL);

    switch (lrd_info->pt_type) {
        case LT_PT_TYPE_TCAM_PRI_ONLY:
        case LT_PT_TYPE_TCAM_PRI_AND_ENTRY_ID:
        case LT_PT_TYPE_LPM:
        case LT_PT_TYPE_FP:
            rm_tcam_ptrm_info = (bcmptm_rm_tcam_lt_info_t *)rsp_ptrm_info;
            rm_tcam_ptrm_info->num_ekw = lrd_info->num_ekw;
            rm_tcam_ptrm_info->num_edw = lrd_info->num_edw;
            rm_tcam_ptrm_info->non_aggr = lrd_info->aggr_ekw_edw? FALSE : TRUE;
            rm_tcam_ptrm_info->pt_type = lrd_info->pt_type;
            rm_tcam_ptrm_info->dynamic_banks = lrd_info->dynamic_banks;
            rm_tcam_ptrm_info->hw_entry_info = lrd_info->hw_entry_info;
            rm_tcam_ptrm_info->hw_entry_info_count = lrd_info->hw_entry_info_count;
            rm_tcam_ptrm_info->rm_more_info = lrd_info->rm_more_info;

            if (dfield_format_id_valid) { /* for INSERT, or entry in cache */
                *rsp_index_hw_entry_info = 0; /* hw_entry_info_count is 1 */
                *rsp_hw_data_type_val = NULL; /* no data_type field */
            }
            break; /* TCAM_PRI_ONLY */

        default:
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "req_ltid=%0d, pt_type = %0d is unsupported !\n"),
                 req_ltid, lrd_info->pt_type));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            break;
    } /* pt_type */

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_tcam_dyn_ptrm_tbl_update(int unit,
                                   bcmltd_sid_t ltid,
                                   lt_mreq_info_t *mreq_info)
{
    bcmptm_rm_tcam_dyn_ptrm_table_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mreq_info, SHR_E_PARAM);

    ptr = sal_alloc(sizeof(bcmptm_rm_tcam_dyn_ptrm_table_t), "bcmptmRmTcamDyn");
    sal_memset(ptr, 0, sizeof(bcmptm_rm_tcam_dyn_ptrm_table_t));
    ptr->sid = ltid;
    ptr->handler = mreq_info;
    ptr->next = RM_TCAM_DYN_PTRM_TBL(unit);
    RM_TCAM_DYN_PTRM_TBL(unit) = ptr;

exit:
    SHR_FUNC_EXIT();
}

/* Set the RM-TCAM(not specific to any LT) transaction state. */
int
bcmptm_rm_tcam_md_get_first(int unit,
                            bcmptm_rm_tcam_md_t **tcam_md)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tcam_md, SHR_E_PARAM);

    *tcam_md = RM_TCAM_MD(unit);
exit:
    SHR_FUNC_EXIT();
}

/* Get the RM-TCAM(not specific to any LT) transaction info. */
int
bcmptm_rm_tcam_trans_info_global_get(int unit,
               bcmptm_rm_tcam_trans_info_t **trans_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(trans_info, SHR_E_PARAM);

    if (RM_TCAM_INFO_GLOBAL(unit) == NULL) {
        *trans_info = NULL;
        SHR_FUNC_EXIT();
    }

    *trans_info = &(RM_TCAM_INFO_GLOBAL(unit)->trans_info);
exit:
    SHR_FUNC_EXIT();
}
