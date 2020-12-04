/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mbcm.c
 * Purpose:     Implementation of bcm multiplexing
 */


#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/mbcm.h>

CONST mbcm_dpp_functions_t    *mbcm_dpp_driver[BCM_MAX_NUM_UNITS]={0};
soc_dpp_chip_family_t    mbcm_dpp_family[BCM_MAX_NUM_UNITS];


int
mbcm_dpp_init(int unit)
{
#ifdef  BCM_ARAD_SUPPORT 
    if (SOC_IS_ARAD(unit)) {        
#ifdef BCM_QAX_SUPPORT
        if (SOC_IS_QAX(unit)) {
            mbcm_dpp_driver[unit] = &mbcm_qax_driver;
            mbcm_dpp_family[unit] = BCM_FAMILY_QAX;
        } else
#endif
#ifdef BCM_JERICHO_PLUS_SUPPORT
        if (SOC_IS_JERICHO_PLUS(unit)) {
            mbcm_dpp_driver[unit] = &mbcm_jericho_driver;
            mbcm_dpp_family[unit] = BCM_FAMILY_JERICHO;
        } else
#endif 
#ifdef BCM_JERICHO_SUPPORT
        if (SOC_IS_JERICHO(unit)) {
            mbcm_dpp_driver[unit] = &mbcm_jericho_driver;
            mbcm_dpp_family[unit] = BCM_FAMILY_JERICHO;
        } else
#endif 
        {
            mbcm_dpp_driver[unit] = &mbcm_arad_driver;
            mbcm_dpp_family[unit] = BCM_FAMILY_ARAD;
        }
        return SOC_E_NONE;
    }
#endif  

    LOG_INFO(BSL_LS_BCM_INIT,
             (BSL_META_U(unit,
                         "ERROR: mbcm_dpp_init unit %d: unsupported chip type\n"), unit));
    return SOC_E_INTERNAL;
}
