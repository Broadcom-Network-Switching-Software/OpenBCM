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

#include <soc/dpp/mbcm_pp.h>

CONST mbcm_pp_functions_t    *mbcm_pp_driver[BCM_MAX_NUM_UNITS]={0};
soc_pp_chip_family_t    mbcm_pp_family[BCM_MAX_NUM_UNITS];


int
mbcm_dpp_pp_init(int unit)
{
#ifdef  BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
#ifdef  BCM_QAX_SUPPORT
        if (SOC_IS_QAX(unit)) {
            mbcm_pp_driver[unit] = &mbcm_pp_qax_driver;
            mbcm_pp_family[unit] = BCM_PP_FAMILY_QAX;
            return SOC_E_NONE;
        }
#endif  
#ifdef BCM_JERICHO_PLUS_SUPPORT
        if (SOC_IS_JERICHO_PLUS(unit)) {
            mbcm_pp_driver[unit] = &mbcm_pp_jericho_plus_driver;
            mbcm_pp_family[unit] = BCM_PP_FAMILY_JERICHO_PLUS;
            return SOC_E_NONE;
        }
#endif 
#ifdef BCM_JERICHO_SUPPORT
        if (SOC_IS_JERICHO(unit)) {
            mbcm_pp_driver[unit] = &mbcm_pp_jericho_driver;
            mbcm_pp_family[unit] = BCM_PP_FAMILY_JERICHO;
            return SOC_E_NONE;
        }
#endif                               
        
        mbcm_pp_driver[unit] = &mbcm_pp_arad_driver;
        mbcm_pp_family[unit] = BCM_PP_FAMILY_ARAD;
        return SOC_E_NONE;
    }
#endif  

    LOG_INFO(BSL_LS_BCM_INIT,
             (BSL_META_U(unit,
                         "ERROR: mbcm_pp_init unit %d: unsupported chip type\n"), unit));
    return SOC_E_INTERNAL;
}

