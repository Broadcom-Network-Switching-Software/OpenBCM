/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        avs.c
 */

#include <shared/bsl.h>

#ifdef INCLUDE_AVS
#include <soc/avs.h>
#include <soc/tomahawk3.h>
#include <soc/bondoptions.h>

extern soc_error_t soc_tomahawk3_avs_vcore_init(int unit);
extern soc_error_t soc_tomahawk3_avs_vcore_deinit(int unit);
extern soc_error_t soc_tomahawk3_avs_vcore_get(int unit, uint32 *voltage);
extern soc_error_t soc_tomahawk3_avs_vcore_set(int unit, uint32 voltage);

soc_error_t _soc_tomahawk3_avs_vcore_init(int unit)
{
    /* defined in avs_vctl.c */
    return soc_tomahawk3_avs_vcore_init(unit);
}

soc_error_t _soc_tomahawk3_avs_vcore_deinit(int unit)
{
    /* defined in avs_vctl.c */
    return soc_tomahawk3_avs_vcore_deinit(unit);
}

soc_error_t _soc_tomahawk3_avs_vcore_get(int unit, uint32 *voltage)
{
    /* defined in avs_vctl.c */
    return soc_tomahawk3_avs_vcore_get(unit, voltage);
}

soc_error_t _soc_tomahawk3_avs_vcore_set(int unit, uint32 voltage)
{
    /* defined in avs_vctl.c */
    return soc_tomahawk3_avs_vcore_set(unit, voltage);
}

soc_error_t _soc_tomahawk3_openloop_voltage_get(int unit, uint32 *voltage)
{
    /* unit of voltage is mV */
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        *voltage = SOC_BOND_INFO(unit)->avs_status;
    } else {
        *voltage = 8750;
    }
    return SOC_E_NONE;
}

soc_avs_functions_t _tomahawk3_avs_functions;
soc_avs_vrm_access_t _tomahawk3_avs_vrm_access;

soc_error_t
soc_tomahawk3_avs_init(int unit)
{
    /* register avs functions*/
    sal_memset(&_tomahawk3_avs_functions, 0, sizeof(soc_avs_functions_t));
    _tomahawk3_avs_functions.openloop_voltage_get = &_soc_tomahawk3_openloop_voltage_get;
    soc_avs_function_register(unit, &_tomahawk3_avs_functions);

    sal_memset(&_tomahawk3_avs_vrm_access, 0, sizeof(soc_avs_vrm_access_t));
    _tomahawk3_avs_vrm_access.vrm_init = &_soc_tomahawk3_avs_vcore_init;
    _tomahawk3_avs_vrm_access.voltage_set = &_soc_tomahawk3_avs_vcore_set;
    _tomahawk3_avs_vrm_access.voltage_get = &_soc_tomahawk3_avs_vcore_get;
    _tomahawk3_avs_vrm_access.vrm_deinit = &_soc_tomahawk3_avs_vcore_deinit;
    soc_avs_vrm_access_func_register(unit, &_tomahawk3_avs_vrm_access);

    return SOC_E_NONE;
}

#endif /* INCLUDE_AVS */
