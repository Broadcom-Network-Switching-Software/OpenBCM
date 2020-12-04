/*! \file bcmcfg_variant_load.c
 *
 * Load variant data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmltd/chip/bcmltd_id.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_INIT

/*******************************************************************************
 * Public functions
 */
int
bcmcfg_variant_load(int unit, bool warm)
{
    uint64_t ltl_variant;
    bcmlrd_variant_t lrd_variant;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmcfg_field_get(unit,
                                     DEVICE_CONFIGt,
                                     DEVICE_CONFIGt_VARIANTf,
                                     &ltl_variant));

    lrd_variant = bcmlrd_variant_from_device_variant_t(ltl_variant);

    if (lrd_variant != BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(bcmlrd_variant_set(unit, lrd_variant));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                  "Unavailable variant\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}
