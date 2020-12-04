/*! \file bcmptm_rm_hash_imm.c
 *
 * IMM table for Hash Table
 *
 * This file contains the manager for TABLE_HASH_STORE_INFO table.
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
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_table_constants.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmimm/bcmimm_int_comp.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_store.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

#define RM_HASH_STORE_ENT_INITIAL_SIZE 32768

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */

static rm_hash_store_hdr_t *fp_hash_store_hdr[BCMDRD_CONFIG_MAX_UNITS];
static rm_hash_store_ent_t *fp_hash_store[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Private Functions
 */

static int
rm_hash_store_ent_alloc(int unit)
{
    uint32_t req_size, alloc_size;

    SHR_FUNC_ENTER(unit);

    req_size = RM_HASH_STORE_ENT_INITIAL_SIZE * sizeof(rm_hash_store_ent_t);
    alloc_size = req_size;
    fp_hash_store[unit] =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_RM_HASH_COMP_ID,
                         BCMPTM_HA_SUBID_RMHASH_FP_HASH_STORE_ENT_ID,
                         "bcmptmRmhashStoreEnt",
                         &alloc_size);
    SHR_NULL_CHECK(fp_hash_store[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    sal_memset(fp_hash_store[unit], 0, alloc_size);
    SHR_IF_ERR_EXIT
        (bcmissu_struct_info_report
            (unit,
             BCMMGMT_RM_HASH_COMP_ID,
             BCMPTM_HA_SUBID_RMHASH_FP_HASH_STORE_ENT_ID,
             0,
             sizeof(rm_hash_store_ent_t),
             RM_HASH_STORE_ENT_INITIAL_SIZE,
             RM_HASH_STORE_ENT_T_ID));

exit:
    if (SHR_FUNC_ERR()) {
        if (fp_hash_store[unit]) {
            shr_ha_mem_free(unit, fp_hash_store[unit]);
            fp_hash_store[unit] = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

static int
rm_hash_store_ent_realloc(int unit)
{
    uint32_t req_size;
    uint32_t old_size, new_size;
    rm_hash_store_ent_t *ptr = NULL;

    SHR_FUNC_ENTER(unit);

    old_size = fp_hash_store_hdr[unit]->size;
    new_size = old_size * 2;
    req_size = new_size * sizeof(rm_hash_store_ent_t);
    fp_hash_store[unit] =
        shr_ha_mem_realloc(unit, fp_hash_store[unit], req_size);
    SHR_NULL_CHECK(fp_hash_store[unit], SHR_E_MEMORY);
    ptr = fp_hash_store[unit];
    sal_memset(ptr + old_size, 0, old_size * sizeof(rm_hash_store_ent_t));

    fp_hash_store_hdr[unit]->size = req_size;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_hash_store_init(int unit, bool warm)
{
    uint32_t req_size, alloc_size;

    SHR_FUNC_ENTER(unit);

    req_size = sizeof(rm_hash_store_hdr_t);
    alloc_size = req_size;
    fp_hash_store_hdr[unit]=
        shr_ha_mem_alloc(unit,
                         BCMMGMT_RM_HASH_COMP_ID,
                         BCMPTM_HA_SUBID_RMHASH_FP_HASH_STORE_HDR_ID,
                         "bcmptmRmhashStoreHdr",
                         &alloc_size);
    SHR_NULL_CHECK(fp_hash_store_hdr[unit], SHR_E_MEMORY);
    SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    if (warm == FALSE) {
        sal_memset(fp_hash_store_hdr[unit], 0, alloc_size);
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report
                (unit,
                 BCMMGMT_RM_HASH_COMP_ID,
                 BCMPTM_HA_SUBID_RMHASH_FP_HASH_STORE_HDR_ID,
                 0, req_size, 1,
                 RM_HASH_STORE_HDR_T_ID));
        fp_hash_store_hdr[unit]->size = 0;
        fp_hash_store_hdr[unit]->count = 0;
    } else {
        req_size = fp_hash_store_hdr[unit]->size * sizeof(rm_hash_store_ent_t);
        if (req_size == 0) {
            SHR_EXIT();
        }
        alloc_size = req_size;
        fp_hash_store[unit] =
            shr_ha_mem_alloc(unit,
                             BCMMGMT_RM_HASH_COMP_ID,
                             BCMPTM_HA_SUBID_RMHASH_FP_HASH_STORE_ENT_ID,
                             "bcmptmRmhashStoreEnt",
                             &alloc_size);
        SHR_NULL_CHECK(fp_hash_store[unit], SHR_E_MEMORY);
        SHR_IF_ERR_EXIT(alloc_size < req_size ? SHR_E_MEMORY : SHR_E_NONE);
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (fp_hash_store[unit]) {
            shr_ha_mem_free(unit, fp_hash_store[unit]);
            fp_hash_store[unit] = NULL;
        }
        if (fp_hash_store_hdr[unit]) {
            shr_ha_mem_free(unit, fp_hash_store_hdr[unit]);
            fp_hash_store_hdr[unit] = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_hash_store_cleanup(int unit)
{
    fp_hash_store_hdr[unit] = NULL;
    fp_hash_store[unit] = NULL;
}

int
bcmptm_rm_hash_collision_check(int unit,
                               bcmltd_sid_t ltid,
                               uint32_t hash_store,
                               uint32_t *full_key,
                               bool *collision)
{
    bool found = FALSE;
    uint32_t found_idx = 0;
    rm_hash_store_ent_t *ptr = NULL;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    *collision = FALSE;
    bcmptm_rm_hash_store_bin_search(unit,
                                    DT_EM_FP_ENTRYt,
                                    hash_store,
                                    &found,
                                    &found_idx,
                                    NULL);
    if (found) {
        ptr = fp_hash_store[unit] + found_idx;
        rv = sal_memcmp(ptr->full_key, full_key, RM_HASH_MAX_KEY_WORDS * 4);
        if (rv != 0) {
            *collision = TRUE;
        }
    }

    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_store_full_key_same(int unit,
                                   bcmltd_sid_t ltid,
                                   uint32_t hash_store,
                                   uint32_t *full_key,
                                   bool *same)
{
    bool found = FALSE;
    uint32_t found_idx = 0;
    uint32_t *key_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    *same = FALSE;
    bcmptm_rm_hash_store_bin_search(unit,
                                    DT_EM_FP_ENTRYt,
                                    hash_store,
                                    &found,
                                    &found_idx,
                                    NULL);
    if (!found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        key_ptr = (fp_hash_store[unit] + found_idx)->full_key;
        if (sal_memcmp(key_ptr, full_key, RM_HASH_MAX_KEY_WORDS * 4) == 0) {
            *same = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_store_full_key_get(int unit,
                                  bcmltd_sid_t ltid,
                                  uint32_t hash_store,
                                  uint32_t **full_key)
{
    bool found = FALSE;
    uint32_t found_idx = 0;

    SHR_FUNC_ENTER(unit);

    bcmptm_rm_hash_store_bin_search(unit,
                                    DT_EM_FP_ENTRYt,
                                    hash_store,
                                    &found,
                                    &found_idx,
                                    NULL);
    if (!found) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        *full_key = (fp_hash_store[unit] + found_idx)->full_key;
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_hash_store_bin_search(int unit,
                                bcmltd_sid_t ltid,
                                uint32_t search_val,
                                bool *found,
                                uint32_t *found_idx,
                                uint32_t *ins_idx)
{
    int start = 0;
    int end;
    int middle;
    rm_hash_store_ent_t *ent_ptr = NULL;

    COMPILER_REFERENCE(ltid);

    *found = FALSE;
    if (fp_hash_store_hdr[unit]->count == 0) {
        *found = FALSE;
        if (ins_idx) {
            *ins_idx = 0;
        }
    }

    ent_ptr = fp_hash_store[unit];
    end = fp_hash_store_hdr[unit]->count - 1;
    *found = FALSE;
    while (end >= start) {
        middle = (start + end) / 2;
        if (ent_ptr[middle].store_val == search_val) {
            *found = TRUE;
            if (found_idx) {
                *found_idx = middle;
            }
            break;
        } else if (ent_ptr[middle].store_val < search_val) {
            start = middle + 1;
        } else {
            end = middle - 1;
        }
    }

    if (ins_idx) {
        if (!(*found)) {
            *ins_idx = start;
        }
    }
}

int
bcmptm_rm_hash_store_entry_insert(int unit,
                                  bcmltd_sid_t ltid,
                                  uint32_t hash_store,
                                  uint32_t *full_key,
                                  uint32_t ins_idx)
{
    rm_hash_store_ent_t *ptr = NULL;
    uint32_t end = 0;
    uint32_t move_times;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(ltid);

    if (fp_hash_store_hdr[unit]->size == 0) {
        SHR_IF_ERR_EXIT
            (rm_hash_store_ent_alloc(unit));
        fp_hash_store_hdr[unit]->size = RM_HASH_STORE_ENT_INITIAL_SIZE;
    } else if ((fp_hash_store_hdr[unit]->count + 1) >
         fp_hash_store_hdr[unit]->size) {
        SHR_IF_ERR_EXIT
            (rm_hash_store_ent_realloc(unit));
    }

    if (fp_hash_store_hdr[unit]->count != 0) {
        end = fp_hash_store_hdr[unit]->count - 1;
        move_times = end + 1 - ins_idx;
        ptr = fp_hash_store[unit] + end;
        while (move_times--) {
            *(ptr + 1) = *ptr;
            ptr--;
        }
        ptr = fp_hash_store[unit] + ins_idx;
        ptr->store_val = hash_store;
        sal_memcpy(ptr->full_key, full_key, RM_HASH_MAX_KEY_WORDS * 4);
    } else {
        ptr = fp_hash_store[unit];
        ptr->store_val = hash_store;
        sal_memcpy(ptr->full_key, full_key, RM_HASH_MAX_KEY_WORDS * 4);
    }

    fp_hash_store_hdr[unit]->count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_hash_store_entry_delete(int unit,
                                  bcmltd_sid_t ltid,
                                  uint32_t del_idx)
{
    rm_hash_store_ent_t *ptr = NULL;
    uint32_t end;
    int move_times;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(ltid);

    end = fp_hash_store_hdr[unit]->count - 1;
    move_times = end - del_idx;
    if (move_times < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    ptr = fp_hash_store[unit] + del_idx;
    while (move_times--) {
        *ptr = *(ptr + 1);
        ptr++;
    }
    sal_memset(ptr, 0, sizeof(rm_hash_store_ent_t));

    fp_hash_store_hdr[unit]->count--;

exit:
    SHR_FUNC_EXIT();
}

