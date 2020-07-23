/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        dfe_mbcm.c
 * Purpose:     Implementation of bcm multiplexing - For fabric element functions
 *
 * Different chip families require such different implementations
 * of some basic BCM layer functionality that the functions are
 * multiplexed to allow a fast runtime decision as to which function
 * to call.  This file contains the basic declarations for this
 * process.
 *
 * This code allows to use the same MBCM_DFE_DRIVER_CALL API independently of the chip type
 */
/*
 * 
 * $Copyright: (c) 2005 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File:        mbcm.h
 * Purpose:     Multiplexing of the bcm layer - For fabric element functions
 *
 * Different chip families require such different implementations
 * of some basic BCM layer functionality that the functions are
 * multiplexed to allow a fast runtime decision as to which function
 * to call.  This file contains the basic declarations for this
 * process.
 *
 * This code allows to use the same MBCM_DFE_DRIVER_CALL API independently of the chip type
 *
 * See internal/design/soft_arch/xgs_plan.txt for more info.
 *
 * Conventions:
 *    MBCM is the multiplexed bcm prefix
 *    _f is the function type declaration postfix
 */


#include <shared/bsl.h>

#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/cmn/dfe_drv.h>

mbcm_dfe_functions_t    *mbcm_dfe_driver[BCM_MAX_NUM_UNITS]={0};
soc_dfe_chip_family_t    mbcm_dfe_family[BCM_MAX_NUM_UNITS];


int
mbcm_dfe_init(int unit)
{
#ifdef BCM_88750_A0
    if (SOC_IS_FE1600(unit)) {
        mbcm_dfe_driver[unit] = &mbcm_fe1600_driver;
        mbcm_dfe_family[unit] = BCM_FAMILY_FE1600;
        return SOC_E_NONE;
    }
#endif  
#ifdef BCM_88950_A0
	if (SOC_IS_FE3200(unit)) {
		mbcm_dfe_driver[unit] = &mbcm_fe3200_driver;
		mbcm_dfe_family[unit] = BCM_FAMILY_FE3200;
		return SOC_E_NONE;
    }
#endif	

    LOG_CLI((BSL_META_U(unit,
                        "ERROR: mbcm_dfe_init unit %d: unsupported chip type\n"), unit));
    return SOC_E_INTERNAL;
}
