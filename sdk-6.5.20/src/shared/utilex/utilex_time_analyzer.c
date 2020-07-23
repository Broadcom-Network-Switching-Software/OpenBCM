/** \file utilex_time_analyzer.c
 *
 * Provide the a utility to analyze the running time to get better decisions when working on time optimization.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/drv.h>

#include <shared/utilex/utilex_time_analyzer.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
/*
 * }
 */
/**
 * \brief - collected statistics per module / operation
 */
typedef struct
{
    /** name of module / operation */
    char *name;
    /** the current start interval or (-1) if interval wasn't started */
    int last_start_time;
    /** number of active intervals for module */
    int start_count;
    /** total time of all measured intervals*/
    int total_time;
    /** number of intervals measured */
    int nof_occurences;
} utilex_time_analyzer_module_info_t;
/**
 * \brief - collected statistics per unit
 */
typedef struct
{
    /** thread id of primary thread */
    int master_thread_id;
    /** statistics per module / operation */
    utilex_time_analyzer_module_info_t modules_info[UTILEX_TIME_ANALYZER_MAX_NOF_MODULES];
} utilex_time_analyzer_info_t;

/** pointer to DB per unit */
static utilex_time_analyzer_info_t *utilex_time_analyzer_info[SOC_MAX_NUM_DEVICES] = { NULL };

/*
 * See .h file
 */
shr_error_e
utilex_time_analyzer_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(utilex_time_analyzer_info[unit], sizeof(utilex_time_analyzer_info_t), "utilex_time_analyzer",
                       "failed to allocate memory for utilex_time_analyzer.\n%s%s%s", EMPTY, EMPTY, EMPTY);

    utilex_time_analyzer_info[unit]->master_thread_id = sal_thread_id_get();

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_time_analyzer_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (utilex_time_analyzer_info[unit] != NULL)
    {
        SHR_FREE(utilex_time_analyzer_info[unit]);
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_time_analyzer_module_add(
    int unit,
    int module_id,
    char *module_name)
{
    SHR_FUNC_INIT_VARS(unit);

    if (utilex_time_analyzer_info[unit] != NULL)
    {
        if (utilex_time_analyzer_info[unit]->modules_info[module_id].name != NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "module %d already added\n", module_id);
        }

        utilex_time_analyzer_info[unit]->modules_info[module_id].name = module_name;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_time_analyzer_module_remove(
    int unit,
    int module_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (utilex_time_analyzer_info[unit] != NULL)
    {
        sal_memset(&utilex_time_analyzer_info[unit]->modules_info[module_id], 0,
                   sizeof(utilex_time_analyzer_module_info_t));
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
void
utilex_time_analyzer_module_start(
    int unit,
    int module_id)
{
    /** update only in case DB allocated and for primary thread */
    if (utilex_time_analyzer_info[unit] != NULL
        && utilex_time_analyzer_info[unit]->master_thread_id == sal_thread_id_get())
    {
        /** track the number of active intervals*/
        utilex_time_analyzer_info[unit]->modules_info[module_id].start_count++;
        /** store start time for the first active interval only */
        if (utilex_time_analyzer_info[unit]->modules_info[module_id].start_count == 1)
        {
            utilex_time_analyzer_info[unit]->modules_info[module_id].last_start_time = sal_time_usecs();
        }
    }
}

/*
 * See .h file
 */
void
utilex_time_analyzer_module_stop(
    int unit,
    int module_id)
{

    /** update only in case DB allocated and for primary thread */
    if (utilex_time_analyzer_info[unit] != NULL
        && utilex_time_analyzer_info[unit]->master_thread_id == sal_thread_id_get())
    {
        /** add interval for the first active interval only */
        if (utilex_time_analyzer_info[unit]->modules_info[module_id].start_count == 1)
        {
            int interval = sal_time_usecs() - utilex_time_analyzer_info[unit]->modules_info[module_id].last_start_time;
            utilex_time_analyzer_info[unit]->modules_info[module_id].total_time += interval;
            utilex_time_analyzer_info[unit]->modules_info[module_id].nof_occurences++;
        }
        /** track the number of active intervals*/
        utilex_time_analyzer_info[unit]->modules_info[module_id].start_count--;
    }
}

/*
 * See .h file
 */
shr_error_e
utilex_time_analyzer_clear(
    int unit)
{
    int module_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (utilex_time_analyzer_info[unit] != NULL)
    {
        for (module_id = 0; module_id < UTILEX_TIME_ANALYZER_MAX_NOF_MODULES; module_id++)
        {
            /** clear and restore module name only */
            char *name = utilex_time_analyzer_info[unit]->modules_info[module_id].name;
            SHR_IF_ERR_EXIT(utilex_time_analyzer_module_remove(unit, module_id));
            SHR_IF_ERR_EXIT(utilex_time_analyzer_module_add(unit, module_id, name));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
utilex_time_analyzer_diag_info_get(
    int unit,
    int max_nof_modules,
    utilex_time_analyzer_diag_info_t * diag_info,
    int *nof_modules)
{
    int module_index = 0, module_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (utilex_time_analyzer_info[unit] != NULL)
    {
        for (module_id = 0; module_id < UTILEX_TIME_ANALYZER_MAX_NOF_MODULES; module_id++)
        {
            if (utilex_time_analyzer_info[unit]->modules_info[module_id].name != NULL)
            {
                diag_info[module_index].name = utilex_time_analyzer_info[unit]->modules_info[module_id].name;
                diag_info[module_index].time = utilex_time_analyzer_info[unit]->modules_info[module_id].total_time;
                diag_info[module_index].occurences =
                    utilex_time_analyzer_info[unit]->modules_info[module_id].nof_occurences;
                module_index++;
            }
        }
    }

    *nof_modules = module_index;

    SHR_FUNC_EXIT;
}
