/*! \file rm_tcam_prio_only_compress.c
 *
 *  Utility functions to compress entries when multiple prio only
 *  TCAM LTs are sharing the same physical resources.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include "rm_tcam.h"
#include "rm_tcam_prio_only.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMTCAM

/*******************************************************************************
 * Public Functions
 */
/*
 * Function:
 *     bcmptm_rm_tcam_prio_only_compress
 * Purpose:
 *     Compress entries in order to free entries
 *     for 2x,3x and 4x type
 *     entries.
 * Paramters:
 *     unit - (IN) BCM device number
 *     iomd - (IN) Input and Output metadata
 * Returns:
 *     SHR_E_XXX
 */
int
bcmptm_rm_tcam_prio_only_compress(int unit,
                                  bcmptm_rm_tcam_entry_iomd_t *iomd)
{
    int eidx;
    uint32_t idx;
    bcmptm_rm_tcam_prio_only_entry_info_t *entry_info = NULL;
    uint32_t num_entries;
    bcmltd_sid_t other_ltid = 0;
    bcmptm_rm_tcam_lt_info_t other_ltid_info;
    uint8_t shared_count = 0;
    uint8_t key_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(iomd, SHR_E_PARAM);

    entry_info = iomd->entry_info_arr.entry_info[0][0];
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_depth_get(unit,
                                  iomd->ltid,
                                  iomd->ltid_info,
                                  &num_entries));

    /* Get the ltid and ltid_info of shared ltid */
    if (iomd->ltid_info->rm_more_info->shared_lt_count) {
        shared_count =
            iomd->ltid_info->rm_more_info->shared_lt_count;
        for (idx = 0 ; idx < shared_count ; idx++) {
            if (iomd->ltid_info->rm_more_info->shared_lt_info[idx] !=
                iomd->ltid) {
                other_ltid =
                    iomd->ltid_info->rm_more_info->shared_lt_info[idx];
            }
        }
    }
    if (other_ltid == 0) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(&other_ltid_info, 0, sizeof(bcmptm_rm_tcam_lt_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_rm_tcam_ptrm_info_get(unit,
                                      other_ltid,
                                      0,
                                      0,
                                      ((void *)&other_ltid_info),
                                      NULL,
                                      NULL));


    eidx = -1;
    key_type = other_ltid_info.hw_entry_info->key_type_val[0][0];
    for (idx = 0; idx < num_entries; idx++) {
        /* Find an empty slot. */
        if (entry_info[idx].entry_pri == BCMPTM_RM_TCAM_EID_INVALID) {
            if (eidx == -1) {
                /* Check whether it is at correct start boundary */
                if (entry_info[idx].entry_type == key_type) {
                    eidx = idx;
                }
            }
            continue;
        }
        if (eidx != -1 &&
            entry_info[idx].entry_type == key_type) {
            /*
             * Move the current entry to empty index found
             * Move the entry in hardware.
             */
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmptm_rm_tcam_entry_move(unit,
                                            other_ltid,
                                            &other_ltid_info,
                                            0,
                                            idx, eidx,
                                            iomd->entry_attrs,
                                            iomd->pt_dyn_info,
                                            NULL));
             idx = eidx;
             eidx = -1;
        }
    }
exit:
    SHR_FUNC_EXIT();
}
