/*! \file bcm56990_a0_cth_lb.c
 *
 * Load balance hashing chip driver
 *
 * The library functions definitions for bcm56990_a0 device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmcth/bcmcth_lb_drv.h>
#include <bcmcth/bcmcth_lb_hash_info.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_LB

/*******************************************************************************
* Private functions
 */

static int
bcm56990_a0_cth_lb_hash_info_get(int unit, bcmcth_lb_hash_info_t *hash_info)
{
    bcmcth_lb_hash_info_t bcm56990_a0_hash_info = {
        .nonconcat_sub_field_width = { 16, 16, 6, 16, 0, 0, 16, 16 },
        .num_nonconcat_sub_field = 8,
        .concat_sub_field_width = { 64, 0, 6, 64, 0, 0, 0, 0 },
        .num_concat_sub_field = 8
    };

    if (hash_info) {
        sal_memcpy(hash_info, &bcm56990_a0_hash_info, sizeof(*hash_info));
    }
    return SHR_E_NONE;
}

static bcmcth_lb_drv_t bcm56990_a0_cth_lb_drv = {
    .hash_info_get = bcm56990_a0_cth_lb_hash_info_get
};

/*******************************************************************************
* Public functions
 */

bcmcth_lb_drv_t *
bcm56990_a0_cth_lb_drv_get(int unit)
{
    return &bcm56990_a0_cth_lb_drv;
}
