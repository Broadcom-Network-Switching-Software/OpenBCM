/** \file appl_ref_init_utils.c
 * Common init and deinit functions to be used by the INIT_DNX command.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

/*
* INCLUDE FILES:
* {
*/
#include <sal/appl/sal.h>

#include <soc/drv.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_seq.h>

#include <soc/property.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#endif

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#endif

#include <appl/reference/dnxc/appl_ref_init_utils.h>
#include <appl/reference/dnxc/appl_ref_init_deinit.h>

/*
 * }
 */

/*
 * Typedefs.
 * {
 */

/*
 * }
 */

/*
 * Globals.
 * {
 */

/*
 * }
 */

/*
 * Static objects
 * {
 */

static utilex_seq_t *seq_ptr_arr[SOC_MAX_NUM_DEVICES];

/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
appl_dnxc_init_step_list_destory(
    int unit,
    utilex_seq_step_t * step_list)
{
    int step_index;
    SHR_FUNC_INIT_VARS(unit);

    if (step_list != NULL)
    {
        /** recursive destroy */
        for (step_index = 0; step_list[step_index].step_id != UTILEX_SEQ_STEP_LAST; step_index++)
        {
            if (step_list[step_index].step_sub_array != NULL)
            {
                SHR_IF_ERR_CONT(appl_dnxc_init_step_list_destory(unit, step_list[step_index].step_sub_array));
            }
        }

        /** destroy current */
        sal_free(step_list);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - counts NOF members in step list
 *
 * \param [in] unit - unit #
 * \param [in] step_list - pointer to step list
 * \param [out] nof_steps - returned result
 * \return
 *   See shr_error_e
 * \remark
 *   list MUST contain at least one member (last member) with
 *   name  == NULL.
 * \see
 *   * None
 */
static shr_error_e
appl_dnxc_init_step_list_count_steps(
    int unit,
    const appl_dnxc_init_step_t * step_list,
    int *nof_steps)
{
    int step_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * all lists must have at least one member - the last one - with name = NULL
     */
    for (step_index = 0; step_list[step_index].name != NULL; ++step_index)
    {
        /*
         * Do nothing
         */
    }

    *nof_steps = step_index;

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxc_init_step_list_convert(
    int unit,
    const appl_dnxc_init_step_t * dnx_step_list,
    utilex_seq_step_t ** step_list)
{
    int list_size;
    int step_index;
    utilex_seq_step_t *step;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get list size
     */
    SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_count_steps(unit, dnx_step_list, &list_size));
    list_size++; /** Count the last step too */

    /*
     * Allocate memory for list
     */
    *step_list =
        (utilex_seq_step_t *) sal_alloc(sizeof(utilex_seq_step_t) * list_size, "dnx port add remove sequence list");
    SHR_NULL_CHECK(*step_list, _SHR_E_MEMORY, "failed to allocate memory for step list");

    /*
     * Convert each step
     */
    for (step_index = 0; step_index < list_size; step_index++)
    {
        step = &((*step_list)[step_index]);

        SHR_IF_ERR_EXIT(utilex_seq_step_t_init(unit, step));

        step->step_id = dnx_step_list[step_index].step_id;
        step->step_name = dnx_step_list[step_index].name;
        step->soc_prop_suffix = dnx_step_list[step_index].suffix;
        step->forward = dnx_step_list[step_index].init_func;
        step->backward = dnx_step_list[step_index].deinit_func;
        step->dyn_flags = dnx_step_list[step_index].flag_function;
        step->static_flags = dnx_step_list[step_index].step_flags;

        if (dnx_step_list[step_index].sub_list != NULL)
        {
            SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_convert
                            (unit, dnx_step_list[step_index].sub_list, &(step->step_sub_array)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
appl_init_seq_flags_get(
    int unit,
    int *flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "time_test_en", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_TIME_STAMP;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "time_log_en", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_TIME_LOG;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mem_test_en", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_MEM_LEAK_DETECT;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mem_log_en", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_MEM_LOG;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "swstate_log_en", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_SWSTATE_LOG;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "access_only", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_ACCESS_ONLY;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "heat_up", 0))
    {
        *flags |= APPL_INIT_ADVANCED_F_HEAT_UP;
    }

    if (!((*flags & APPL_INIT_ADVANCED_F_ACCESS_ONLY) || (*flags & APPL_INIT_ADVANCED_F_HEAT_UP)))
    {
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multithread_en", 0))
        {
            *flags |= APPL_INIT_ADVANCED_F_MULTITHREAD;
        }

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_multithread_en", 0))
        {
            *flags |= APPL_INIT_ADVANCED_F_KBP_MULTITHREAD;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the time threshold of a given step according to the
 *          step ID and the step fags. If called with flag UTILEX_SEQ_STEP_INVALID
 *          will return the overall threshold for the full sequence. In this case
 *          flags are not relevant.
 *
 * \param [in] unit - Unit #
 * \param [in] step_id - Step ID
 * \param [in] flags - Step flags
 * \param [out] time_thresh - Time threshold for the specified step
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
appl_init_time_thresh_cb(
    int unit,
    int step_id,
    uint32 flags,
    sal_usecs_t * time_thresh)
{
    int time_thresh_index = 0;
    const appl_init_time_thresh_info_t *time_thresh_list;
    SHR_FUNC_INIT_VARS(unit);

    if (step_id == UTILEX_SEQ_STEP_INVALID)
    {
        /** Get the total threshold for the full sequence per device */
        SHR_IF_ERR_EXIT(appl_dnxc_per_dev_full_time_threshold_get(unit, time_thresh));
    }
    else
    {
        if (flags & APPL_INIT_ADVANCED_F_TIME_THRESH_EXPLICIT)
        {
            /** Take the threshold for the step from a user-defined array (appl_init_time_thresh_expl)
              * of step IDs and thresholds */
            SHR_IF_ERR_EXIT(appl_dnxc_step_time_thresh_list_get(unit, &time_thresh_list));
            for (time_thresh_index = 0; time_thresh_list[time_thresh_index].step_id != UTILEX_SEQ_STEP_LAST;
                 ++time_thresh_index)
            {
                if (time_thresh_list[time_thresh_index].step_id == step_id)
                {

                    *time_thresh = time_thresh_list[time_thresh_index].time_thresh;
                }
            }
        }
        else if (flags & APPL_INIT_ADVANCED_F_TIME_THRESH_PER_DEVICE)
        {
            /** The thresholds are device-dependent */
            SHR_IF_ERR_EXIT(appl_dnxc_per_dev_step_time_threshold_get(unit, step_id, time_thresh));
        }
        else
        {
            /** Step uses the default threshold */
            *time_thresh = APPL_INIT_STEP_TIME_THRESH_DEFAULT;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
appl_init_skip_print_decider_cb(
    int unit)
{
    if (SOC_IS_DNXF(unit))
    {
#ifdef BCM_DNXF_SUPPORT
        if (dnxf_data_device.general.feature_get(unit, dnxf_data_device_general_production_ready))
        {
            return 0;
        }
#endif
    }
    else if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT
        if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_production_ready))
        {
            return 0;
        }
#endif
    }

    return 1;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxc_init_seq_convert(
    int unit,
    const appl_dnxc_init_step_t * step_list,
    int forward,
    utilex_seq_t * seq)
{
    int flags = 0;
    char *time_analyze_mode_str;
    SHR_FUNC_INIT_VARS(unit);

    if (forward)
    {
        /*
         * Init seq structure
         */
        SHR_IF_ERR_EXIT(utilex_seq_t_init(unit, seq));

        /** read directly soc property to know whether to enable time analyzer */
        time_analyze_mode_str = soc_property_suffix_num_str_get(unit, -1, spn_CUSTOM_FEATURE, "time_analyze");
        if (time_analyze_mode_str != NULL && (!sal_strcasecmp("PER_STEP", time_analyze_mode_str)))
        {
            seq->time_analyzer_mode = UTILEX_TIME_ANALYZER_MODE_PER_INIT_STEP;
        }
    }

    
    appl_init_seq_flags_get(unit, &flags);

    /*
     * Configure sequence
     */
    /** Logging */

    seq->bsl_flags = BSL_LOG_MODULE;
    if (forward)
    {
        sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "%d: APPL Init:", unit);
    }
    else
    {
        sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "%d: APPL Deinit:", unit);
    }

    seq->control_prefix = spn_APPL_ENABLE;
    seq->log_severity = bslSeverityInfo;

    seq->time_log_en = (flags & APPL_INIT_ADVANCED_F_TIME_LOG) ? 1 : 0;
    seq->time_test_en = (flags & APPL_INIT_ADVANCED_F_TIME_STAMP) ? 1 : 0;
    seq->time_thresh_cb = appl_init_time_thresh_cb;
    seq->mem_log_en = (flags & APPL_INIT_ADVANCED_F_MEM_LOG) ? 1 : 0;
    seq->mem_test_en = (flags & APPL_INIT_ADVANCED_F_MEM_LEAK_DETECT) ? 1 : 0;
    seq->swstate_log_en = (flags & APPL_INIT_ADVANCED_F_SWSTATE_LOG) ? 1 : 0;
    seq->access_only = (flags & APPL_INIT_ADVANCED_F_ACCESS_ONLY) ? 1 : 0;
    seq->heat_up = (flags & APPL_INIT_ADVANCED_F_HEAT_UP) ? 1 : 0;
#if 1
    seq->multithread_en = 0;
#endif
    seq->kbp_multithread_en = (flags & APPL_INIT_ADVANCED_F_KBP_MULTITHREAD) ? 1 : 0;
    seq->skip_print_decider = appl_init_skip_print_decider_cb;

    if (forward == 1)
    {
        /** Convert sequence (only on init, deinit used the same data)*/
        SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_convert(unit, step_list, &seq->step_list));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnxc_steps_convert_and_run(
    int unit,
    const appl_dnxc_init_step_t * step_list,
    int forward)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    if (forward)
    {
        /*
         * Allocate memory for APPL Init sequence only if it's init
         */
        seq_ptr_arr[unit] = (utilex_seq_t *) sal_alloc(sizeof(utilex_seq_t), "DNX Init sequence");
        SHR_NULL_CHECK(seq_ptr_arr[unit], _SHR_E_MEMORY, "Failed to allocate memory for Init sequence");
    }

        /** Convert step list to general utilex_seq list */
    SHR_IF_ERR_EXIT(appl_dnxc_init_seq_convert(unit, step_list, forward, seq_ptr_arr[unit]));

    /*
     * Run list and check error just after appl_dnxc_init_step_list_destory and the pointer is freed
     * This is done so that in case of an error the resources will be freed before exiting the function
     */
    rv = utilex_seq_run(unit, seq_ptr_arr[unit], forward);

    if (!forward)
    {
                /** Destory step list (allocated in 'dnx_port_add_remove_seq_convert()') */
        SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_destory(unit, seq_ptr_arr[unit]->step_list));
        SHR_FREE(seq_ptr_arr[unit]);
    }

    /** Check error */
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}
