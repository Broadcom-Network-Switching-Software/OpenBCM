/** \file init_time_analyzer.c
 *
 * Define the modules running time to be analyzed in order to get better decisions when working on time optimization.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_time_analyzer.h>
#include <bcm_int/dnx/init/init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
/*
 * }
 */

static dnx_init_time_analyzer_operations_t dnx_init_time_analyzer_ops[] = DNX_INIT_TIME_ANALYZER_OPS_INIT;

/*
 * See .h file
 */
shr_error_e
dnx_init_time_analyzer_init(
    int unit)
{
    int index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_time_analyzer_init(unit));

    for (index = 0; index < COUNTOF(dnx_init_time_analyzer_ops); index++)
    {
        if (dnx_init_time_analyzer_ops[index].enable_flag)
        {
            SHR_IF_ERR_EXIT(utilex_time_analyzer_module_add
                            (unit, dnx_init_time_analyzer_ops[index].module_id,
                             dnx_init_time_analyzer_ops[index].name));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_init_time_analyzer_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_time_analyzer_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_time_analyzer_flag_cb(
    int unit,
    int *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    *flags = 0;
    if (dnx_data_dev_init.time.analyze_get(unit) == UTILEX_TIME_ANALYZER_MODE_OFF)
    {
        *flags = DNX_INIT_STEP_F_SKIP | DNX_INIT_STEP_F_VERBOSE;
    }

    SHR_FUNC_EXIT;
}
