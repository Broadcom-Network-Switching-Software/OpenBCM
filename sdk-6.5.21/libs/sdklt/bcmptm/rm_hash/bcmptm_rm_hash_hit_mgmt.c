/*! \file bcmptm_rm_hash_hit_mgmt.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the resource manager for physical hash tables
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
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmevm/bcmevm_api.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_hit_mgmt.h"

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
void
bcmptm_rm_hash_entry_hit_bit_retrieve(int unit,
                                      uint32_t *ent_words,
                                      const bcmptm_rm_hash_hit_info_t *hit_info,
                                      uint32_t *hit_list,
                                      uint8_t *hit_cnt)
{
    uint8_t idx, hit_map_count = hit_info->hit_map_count;
    uint32_t hit_value = 0;
    const bcmptm_rm_hash_hit_map_t *hit_map = NULL;

    for (idx = 0; idx < hit_map_count; idx++) {
        hit_map = hit_info->hit_map + idx;
        /*
         * Record the hit bit value provided in reserved fields in physical
         * entry.
         */
        bcmdrd_field_get(ent_words, hit_map->bit, hit_map->bit, &hit_list[idx]);
        /* Clear the hit bit in physical table entry. */
        bcmdrd_field_set(ent_words, hit_map->bit, hit_map->bit, &hit_value);
    }
    *hit_cnt = hit_map_count;
}

int
bcmptm_rm_hash_hit_table_update(int unit,
                                rm_hash_req_context_t *req_ctxt,
                                bcmbd_pt_dyn_info_t *dyn_info,
                                const bcmptm_rm_hash_hit_info_t *hit_info,
                                uint32_t *hit_list,
                                uint8_t hit_cnt)
{
    uint8_t idx, hit_map_count = hit_info->hit_map_count;
    const bcmptm_rm_hash_hit_map_t *hit_map = NULL;
    uint32_t rsp_flags = BCMPTM_RSP_FLAGS_NO_FLAGS;

    SHR_FUNC_ENTER(unit);

    if (hit_cnt != hit_map_count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    for (idx = 0; idx < hit_map_count; idx++) {
        hit_map = hit_info->hit_map + idx;
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_write(unit,
                                  req_ctxt->req_flags,
                                  0,
                                  hit_map->sid[0],
                                  dyn_info,
                                  NULL,
                                  &hit_list[idx],
                                  TRUE,
                                  TRUE,
                                  FALSE,
                                  BCMPTM_RM_OP_NORMAL, NULL,
                                  0,
                                  0,
                                  &rsp_flags));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_hit_bit_get(int unit,
                           rm_hash_req_context_t *req_ctxt,
                           bcmbd_pt_dyn_info_t *dyn_info,
                           const bcmptm_rm_hash_hit_info_t *hit_info,
                           uint32_t *ent_words)
{
    uint8_t idx, hit_map_count = hit_info->hit_map_count;
    uint32_t hit_value;
    const bcmptm_rm_hash_hit_map_t *hit_map = NULL;
    bool cache_valid;
    bcmltd_sid_t ltid;
    uint16_t dft_id;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < hit_map_count; idx++) {
        hit_map = hit_info->hit_map + idx;
        hit_value = 0;
        SHR_IF_ERR_EXIT
            (bcmptm_cmdproc_read(unit,
                                 req_ctxt->req_flags,
                                 0,
                                 hit_map->sid[0],
                                 dyn_info,
                                 NULL,
                                 1,
                                 &hit_value,
                                 &cache_valid,
                                 &ltid,
                                 &dft_id,
                                 &rsp_flags));
        bcmdrd_field_set(ent_words, hit_map->bit, hit_map->bit, &hit_value);
    }

exit:
    SHR_FUNC_EXIT();
}
