/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        dnxf_mbcm.c
 * Purpose:     Implementation of bcm multiplexing - For fabric element functions
 *
 * Different chip families require such different implementations
 * of some basic BCM layer functionality that the functions are
 * multiplexed to allow a fast runtime decision as to which function
 * to call.  This file contains the basic declarations for this
 * process.
 *
 * This code allows to use the same MBCM_DNXF_DRIVER_CALL API independently of the chip type
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
 * This code allows to use the same MBCM_DNXF_DRIVER_CALL API independently of the chip type
 *
 * See internal/design/soft_arch/xgs_plan.txt for more info.
 *
 * Conventions:
 *    MBCM is the multiplexed bcm prefix
 *    _f is the function type declaration postfix
 */


#include <shared/bsl.h>

#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>

mbcm_dnxf_functions_t    *mbcm_dnxf_driver[BCM_MAX_NUM_UNITS]={0};
soc_dnxf_chip_family_t    mbcm_dnxf_family[BCM_MAX_NUM_UNITS];


int
mbcm_dnxf_init(int unit)
{

	if (SOC_IS_RAMON(unit)) {
		mbcm_dnxf_driver[unit] = &mbcm_ramon_driver;
		mbcm_dnxf_family[unit] = BCM_FAMILY_RAMON;
		return _SHR_E_NONE;
    }	

    LOG_CLI((BSL_META_U(unit,
                        "ERROR: mbcm_dnxf_init unit %d: unsupported chip type\n"), unit));
    return _SHR_E_INTERNAL;
}
