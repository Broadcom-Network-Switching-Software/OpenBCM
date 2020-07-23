/*! \file bcm56880_a0_sec_drv_attach.c
 *
 * Chip stub for BCMSEC.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef UNFINISHED_DRIVER_CHECK
#error DRIVER UNFINISHED
#endif

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmsec/bcmsec_sysm.h>
#include <bcmsec/bcmsec_drv.h>

/*******************************************************************************
 * Local definitions
 */
static bcmsec_drv_t bcm56880_a0_sec_drv;
extern int
bcmsec_bcm56880_a0_drv_attach(int unit);

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_INIT
/******************************************************************************
 * Public functions
 */
int
bcmsec_bcm56880_a0_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmsec_drv_set(unit, &bcm56880_a0_sec_drv));
exit:
    SHR_FUNC_EXIT();
}

