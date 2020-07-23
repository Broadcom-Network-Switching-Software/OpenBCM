/*! \file bcmptm_rm_hash_narrow_mode_mgmt.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the routines that implement the basic operations of hash
 * resource manager. More specifically, it includes the routines for
 * GET_TABLE_INFO opcode.
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_state_mgmt.h"
#include "bcmptm_rm_hash_narrow_mode_mgmt.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */


/*******************************************************************************
 * Public Functions
 */
int
bcmptm_rm_hash_nm_entry_extract(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint8_t rbank,
                                rm_hash_entry_narrow_mode_t enm,
                                uint8_t nme_loc,
                                uint32_t *src_ent_buf,
                                uint32_t *dst_ent_buf)
{
    const bcmptm_rm_hash_narrow_mode_info_t *nm_info = NULL;
    const bcmlrd_hw_rfield_info_t *field_info = NULL;
    uint16_t width, start_bit, end_bit;
    uint32_t base_valid = 0;
    uint32_t ent[RM_HASH_MAX_NARROW_ENT_WORDS_SIZE] = {0};

    SHR_FUNC_ENTER(unit);

    nm_info = pt_info->nm_info[rbank];
    if ((nm_info == NULL) || (nm_info->nme_field_list == NULL)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * For narrow mode entry, extract the base valid field value.
     * The start and end bit for base valid is chip wide constant.
     */
    bcmdrd_field_get(src_ent_buf, 0, 2, &base_valid);
    bcmdrd_field_set(dst_ent_buf, 0, 2, &base_valid);

    /*
     * Extract the entry portion from the entry and set into narrow mode
     * entry according to the 1st format.
     */
    field_info = nm_info->nme_field_list[enm];
    width = field_info->field_width;
    start_bit = field_info->field_start_bit[nme_loc];
    end_bit = start_bit + width - 1;
    bcmdrd_field_get(src_ent_buf, start_bit, end_bit, ent);

    start_bit = field_info->field_start_bit[0];
    end_bit = start_bit + width - 1;
    bcmdrd_field_set(dst_ent_buf, start_bit, end_bit, ent);

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_matched_ltid_get(int unit,
                                bcmltd_sid_t ltid,
                                rm_hash_pt_info_t *pt_info,
                                uint8_t rbank,
                                uint8_t pipe,
                                uint32_t be_offset,
                                uint8_t *ltid_loc_list,
                                uint8_t *num_ltids,
                                rm_hash_entry_narrow_mode_t *e_nm)
{
    rm_hash_nme_bmp_t *tmp_ent_bmp = NULL;
    uint8_t idx, loc = 0;

    SHR_FUNC_ENTER(unit);

    *e_nm = BCMPTM_RM_HASH_ENTRY_NARROW_MODE_NONE;
    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_nme_bmp_get(unit, pt_info, rbank, pipe,
                                    be_offset, &tmp_ent_bmp));
    for (idx = 0; idx < RM_HASH_NARROW_ENTRY_LITD_NUM; idx++) {
        if (tmp_ent_bmp->ltid[idx] == ltid) {
            ltid_loc_list[loc] = idx;
            *e_nm = tmp_ent_bmp->nm[idx];
            loc++;
        }
    }
    *num_ltids = loc;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_nme_ltid_get(int unit,
                            rm_hash_pt_info_t *pt_info,
                            uint8_t rbank,
                            uint8_t pipe,
                            uint32_t be_offset,
                            uint8_t nme_loc,
                            bcmltd_sid_t *ltid)
{
    rm_hash_nme_bmp_t *tmp_ent_bmp = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_rm_hash_nme_bmp_get(unit, pt_info, rbank, pipe,
                                    be_offset, &tmp_ent_bmp));
    *ltid = tmp_ent_bmp->ltid[nme_loc];

exit:
    SHR_FUNC_EXIT();
}
