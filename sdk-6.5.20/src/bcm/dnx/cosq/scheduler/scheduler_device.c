

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_error.h>
#include "scheduler_dbal.h"
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>


static shr_error_e
dnx_scheduler_device_rate_to_internal(
    int unit,
    uint32 rate,
    uint32 *internal)
{
    uint32 calc;
    uint32 credit_worth;
    uint32 rate_max_value;
    uint64 u64_calc;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    

    if (0 == rate)
    {
        *internal = 0;
    }
    else
    {
        COMPILER_64_ZERO(u64_calc);
        COMPILER_64_SET(u64_calc, 0,
                        (credit_worth * UTILEX_NOF_BITS_IN_CHAR * dnx_data_sch.device.drm_resolution_get(unit)));
        COMPILER_64_UMUL_32(u64_calc, DNXCMN_CORE_CLOCK_KHZ_GET(unit));
        
        COMPILER_64_UDIV_32(u64_calc, 1000);
        calc = COMPILER_64_LO(u64_calc);

        *internal = UTILEX_DIV_ROUND(calc, rate);
    }

    

    if (*internal != 0)
    {
        
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, DBAL_TABLE_SCH_CORE_DRM,
                                                              DBAL_FIELD_RATE, 0, 0, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, &rate_max_value));
        

        *internal = UTILEX_RANGE_BOUND(*internal, dnx_data_sch.device.drm_resolution_get(unit), rate_max_value);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_scheduler_device_internal_to_rate(
    int unit,
    uint32 internal,
    uint32 *rate)
{
    uint32 calc;
    uint32 credit_worth;
    uint64 u64_calc;

    SHR_FUNC_INIT_VARS(unit);

    
    if (0 == internal)
    {
        *rate = 0;
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        COMPILER_64_ZERO(u64_calc);
        COMPILER_64_SET(u64_calc, 0,
                        (credit_worth * UTILEX_NOF_BITS_IN_CHAR * dnx_data_sch.device.drm_resolution_get(unit)));
        COMPILER_64_UMUL_32(u64_calc, DNXCMN_CORE_CLOCK_KHZ_GET(unit));
        
        COMPILER_64_UDIV_32(u64_calc, 1000);
        calc = COMPILER_64_LO(u64_calc);

        *rate = UTILEX_DIV_ROUND(calc, internal);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_scheduler_device_shared_rate_entry_set(
    int unit,
    int rci_level,
    int num_links,
    uint32 rate)
{

    uint32 internal_rate;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_device_rate_to_internal(unit, rate, &internal_rate));

    SHR_IF_ERR_EXIT(dnx_sch_global_drm_set(unit, rci_level, num_links, internal_rate));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_scheduler_device_shared_rate_entry_get(
    int unit,
    int rci_level,
    int num_links,
    uint32 *rate)
{
    uint32 internal_rate;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_global_drm_get(unit, rci_level, num_links, &internal_rate));

    SHR_IF_ERR_EXIT(dnx_scheduler_device_internal_to_rate(unit, internal_rate, rate));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_scheduler_device_core_rate_entry_set(
    int unit,
    int core,
    int rci_level,
    int num_links,
    uint32 rate)
{
    uint32 internal_rate;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_device_rate_to_internal(unit, rate, &internal_rate));

    SHR_IF_ERR_EXIT(dnx_sch_core_drm_set(unit, core, rci_level, num_links, internal_rate));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_scheduler_device_core_rate_entry_get(
    int unit,
    int core,
    int rci_level,
    int num_links,
    uint32 *rate)
{
    uint32 internal_rate;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_core_drm_get(unit, core, rci_level, num_links, &internal_rate));

    SHR_IF_ERR_EXIT(dnx_scheduler_device_internal_to_rate(unit, internal_rate, rate));

exit:
    SHR_FUNC_EXIT;
}
