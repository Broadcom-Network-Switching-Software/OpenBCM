/*! \file bcm56780_a0_dna_2_4_13_mpls.c
 *
 * BCM56780_A0 DNA_2_4_13 MPLS driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mpls_int.h>
#include <bcm_int/ltsw/mbcm/mpls.h>
#include <bcm_int/ltsw/xfs/types.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_MPLS

/******************************************************************************
 * Private functions
 */

static mbcm_ltsw_mpls_drv_t bcm56780_a0_dna_2_4_13_ltsw_mpls_drv = {
    NULL
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_dna_2_4_13_ltsw_mpls_drv_attach(int unit)
{
    return mbcm_ltsw_mpls_drv_set(unit,
                                  &bcm56780_a0_dna_2_4_13_ltsw_mpls_drv);
}
