/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2.c
 * Purpose:     Trident2 L2 function implementations
 */

#include <shared/bsl.h>
#include <soc/defs.h>

#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/drv.h>
#include <soc/l2x.h>

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trident2.h>

/*
 * Function:
 *      bcm_td2_l2_init
 * Description:
 *      Initialize chip-dependent parts of L2 module
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

int
bcm_td2_l2_init(int unit)
{
    int sw_learning_support = 0;

    BCM_IF_ERROR_RETURN(bcm_tr_l2_init(unit));

    if (SOC_CONTROL(unit)->l2x_age_interval != 0) {
        SOC_IF_ERROR_RETURN(soc_td2_l2_bulk_age_stop(unit));
    }

    sw_learning_support = soc_feature(unit, soc_feature_l2_hw_aging_bug) ||
                          soc_feature(unit, soc_feature_no_l2_remote_trunk);
    
    if (soc_property_get(unit, spn_RUN_L2_SW_AGING, sw_learning_support)) {
        if (!SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN(soc_td2_l2_bulk_age_start(unit, 0));
        }
        SOC_CONTROL(unit)->l2x_sw_aging = 1;
    }

    return BCM_E_NONE;
}

#endif /* BCM_TRIDENT2_SUPORT */
