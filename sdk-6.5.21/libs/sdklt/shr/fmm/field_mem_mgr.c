/*! \file field_mem_mgr.c
 *
 * Field memory manager API implementation
 *
 * Interface to the field memory manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_fmm.h>

#define FIELD_ALLOC_CHUNKS  256
static bool module_initialized = false;

static shr_lmm_hdl_t field_mem_hdl;
int shr_fmm_init(void)
{
    int rv;

    LMEM_MGR_INIT(shr_fmm_t,
                  field_mem_hdl,
                  FIELD_ALLOC_CHUNKS,
                  true,
                  rv);
    if (0 != rv) {
        return SHR_E_MEMORY;
    }
    module_initialized = true;
    return SHR_E_NONE;
}

void shr_fmm_delete(void)
{
    if (module_initialized) {
        shr_lmm_delete(field_mem_hdl);
        module_initialized = false;
    }
}

shr_fmm_t* shr_fmm_alloc(void)
{
    if (!module_initialized) {
        return NULL;
    }
    return shr_lmm_alloc(field_mem_hdl);
}

void shr_fmm_free(shr_fmm_t *field)
{
    if (module_initialized) {
        shr_lmm_free(field_mem_hdl, field);
    }
}
