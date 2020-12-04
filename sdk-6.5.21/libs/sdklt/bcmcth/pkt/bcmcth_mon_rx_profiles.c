/*! \file bcmcth_mon_rx_profiles.c
 *
 * Obtain valid RX flow profiles.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bsl/bsl.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmcth/bcmcth_mon_rx_flex.h>
#include <bcmcth/bcmcth_mon_rx_flex_internal.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/*******************************************************************************
 * Private functions
 */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    &_bd##_vu##_va##_mon_rx_flex_profiles_get,
static void (*profiles_get[])(SHR_BITDCL *) = {
    NULL,
#include <bcmlrd/chip/bcmlrd_variant.h>
    NULL
};

int
bcmcth_mon_rx_flex_profiles_get(bcmlrd_variant_t variant,
                                SHR_BITDCL *profiles)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(profiles, SHR_E_PARAM);

    if (variant <= BCMLRD_VARIANT_T_NONE ||
            variant >= BCMLRD_VARIANT_T_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    profiles_get[variant](profiles);
exit:
    SHR_FUNC_EXIT();

}
