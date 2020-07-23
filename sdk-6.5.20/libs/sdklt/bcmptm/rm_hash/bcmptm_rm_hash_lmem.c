/*! \file bcmptm_rm_hash_lmem.c
 *
 * Resource Manager for Hash Table
 *
 * This file contains the routines that implement the basic operations of hash
 * resource manager. More specifically, it includes the routines for LOOKUP,
 * INSERT, DELETE operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <bcmdrd/bcmdrd_field.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_lmem.h"

#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMHASH

/*******************************************************************************
 * Defines
 */

#define RM_HASH_ENTRY_BUFF_CHUNK_SIZE  128

#define RM_HASH_SRCH_INFO_CHUNK_SIZE   32

/*******************************************************************************
 * Private variables
 */

static shr_lmm_hdl_t rm_hash_entry_lmem_hdl;

static shr_lmm_hdl_t rm_hash_glb_srch_info_lmem_hdl;

static shr_lmm_hdl_t rm_hash_map_srch_result_lmem_hdl;

/*******************************************************************************
 * Public Functions
 */

rm_hash_entry_buf_t *
bcmptm_rm_hash_entry_buf_alloc(void)
{
    return shr_lmm_alloc(rm_hash_entry_lmem_hdl);
}

void
bcmptm_rm_hash_entry_buf_free(rm_hash_entry_buf_t *ent_buf)
{
    shr_lmm_free(rm_hash_entry_lmem_hdl, ent_buf);
}


rm_hash_glb_srch_info_t *
bcmptm_rm_hash_glb_srch_info_alloc(void)
{
    return shr_lmm_alloc(rm_hash_glb_srch_info_lmem_hdl);
}

void
bcmptm_rm_hash_glb_srch_info_free(rm_hash_glb_srch_info_t *glb_srch_info)
{
    shr_lmm_free(rm_hash_glb_srch_info_lmem_hdl, glb_srch_info);
}

rm_hash_map_srch_result_t *
bcmptm_rm_hash_map_srch_result_alloc(void)
{
    return shr_lmm_alloc(rm_hash_map_srch_result_lmem_hdl);
}

void
bcmptm_rm_hash_map_srch_result_free(rm_hash_map_srch_result_t *grp_srch_result)
{
    shr_lmm_free(rm_hash_map_srch_result_lmem_hdl, grp_srch_result);
}

void
bcmptm_rm_hash_lmem_cleanup(int unit)
{
    if (rm_hash_entry_lmem_hdl) {
        shr_lmm_delete(rm_hash_entry_lmem_hdl);
        rm_hash_entry_lmem_hdl = NULL;
    }

    if (rm_hash_glb_srch_info_lmem_hdl) {
        shr_lmm_delete(rm_hash_glb_srch_info_lmem_hdl);
        rm_hash_glb_srch_info_lmem_hdl = NULL;
    }

    if (rm_hash_map_srch_result_lmem_hdl) {
        shr_lmm_delete(rm_hash_map_srch_result_lmem_hdl);
        rm_hash_map_srch_result_lmem_hdl = NULL;
    }
}

int
bcmptm_rm_hash_lmem_init(int unit)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    if (!rm_hash_entry_lmem_hdl) {
        LMEM_MGR_INIT(rm_hash_entry_buf_t,
                      rm_hash_entry_lmem_hdl,
                      RM_HASH_ENTRY_BUFF_CHUNK_SIZE,
                      false,
                      rv);
        if (rv != SHR_E_NONE) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    if (!rm_hash_glb_srch_info_lmem_hdl) {
        LMEM_MGR_INIT(rm_hash_glb_srch_info_t,
                      rm_hash_glb_srch_info_lmem_hdl,
                      RM_HASH_SRCH_INFO_CHUNK_SIZE,
                      false,
                      rv);
        if (rv != SHR_E_NONE) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    if (!rm_hash_map_srch_result_lmem_hdl) {
        LMEM_MGR_INIT(rm_hash_map_srch_result_t,
                      rm_hash_map_srch_result_lmem_hdl,
                      RM_HASH_SRCH_INFO_CHUNK_SIZE,
                      false,
                      rv);
        if (rv != SHR_E_NONE) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmptm_rm_hash_lmem_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}
