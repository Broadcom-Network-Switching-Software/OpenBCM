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
#include <soc/helix5.h>
#include <soc/bondoptions.h>

#define SOC_AVS_HELIX5_VMIN_AVS (8000)  /* 0.8000 V */
#define SOC_AVS_HELIX5_VMAX_AVS (8750)  /* 0.8750 V */

STATIC soc_error_t
_soc_hx5_openloop_voltage_get(int unit, uint32 *voltage)
{
    /* unit of voltage is mV */
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        *voltage = SOC_BOND_INFO(unit)->avs_status;
    }

    return SOC_E_NONE;
}

STATIC soc_error_t
_soc_hx5_openloop_min_max_voltage_get(int unit, int *min_volt, int *max_volt)
{
    /* unit of voltage is mV */
    if ((min_volt == NULL) || (max_volt == NULL) ){
        return SOC_E_PARAM;
    }

    *min_volt = SOC_AVS_HELIX5_VMIN_AVS;
    *max_volt = SOC_AVS_HELIX5_VMAX_AVS;

    return SOC_E_NONE;
}

static soc_avs_functions_t _hx5_avs_functions;

soc_error_t
soc_hx5_avs_init(int unit)
{
    /* register avs functions*/
    sal_memset(&_hx5_avs_functions, 0, sizeof(soc_avs_functions_t));
    _hx5_avs_functions.openloop_voltage_get =
                  &_soc_hx5_openloop_voltage_get;
    _hx5_avs_functions.openloop_min_max_voltage_get =
                  &_soc_hx5_openloop_min_max_voltage_get;
    soc_avs_function_register(unit, &_hx5_avs_functions);

    return SOC_E_NONE;
}

#endif /* INCLUDE_AVS */

