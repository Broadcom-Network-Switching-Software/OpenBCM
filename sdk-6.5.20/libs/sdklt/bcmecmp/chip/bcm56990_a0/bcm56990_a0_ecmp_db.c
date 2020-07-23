/*! \file bcm56990_a0_ecmp_db.c
 *
 * Chip stub for BCMECMP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmecmp/bcmecmp.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

int
bcm56990_a0_ecmp_drv_var_attach(int unit,
                                bcmecmp_drv_var_t *drv_var)
{
    SHR_FUNC_ENTER(unit);

    drv_var->flds_mem0 = NULL;
    drv_var->flds_mem1 = NULL;
    drv_var->wflds_mem0 = NULL;
    drv_var->wflds_mem1 = NULL;
    drv_var->dest_flds_mem0 = NULL;

    drv_var->member0_info = NULL;
    drv_var->member1_info = NULL;
    drv_var->wmember0_info = NULL;
    drv_var->wmember1_info = NULL;
    drv_var->dest_member0_info = NULL;

    drv_var->ecmp_mem0 = NULL;
    drv_var->ecmp_mem1 = NULL;
    drv_var->dest_ecmp_mem0 = NULL;
    drv_var->ids = NULL;

    SHR_FUNC_EXIT();
}
