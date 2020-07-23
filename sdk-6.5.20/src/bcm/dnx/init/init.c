/** \file src/bcm/dnx/init/init.c
 * DNX init seq functions.
 *
 * The DNX init seq should be constructed of a series of well defined steps.
 * each step should have a seperate init and deinit functions, where the deinit function
 * should not access the HW but just free allocated resources.
 *
 *
 * each step should have a built in mechanism to test for freeing all allocated resources.
 * each step should have a built in time stamping mechanism to provide an option to test for deviation from time frames.
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
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/init.h>
#include <bcm_int/control.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/port/port_dyn.h>
#include <soc/sand/sand_defs.h>

#include <soc/types.h>
#include <soc/memory.h>
#include <soc/cm.h>
#include <soc/feature.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <shared/bsl.h>
#include <shared/utilex/utilex_seq.h>
#include <shared/utilex/utilex_time_analyzer.h>
#include <sal/appl/config.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#ifdef BCM_WARM_BOOT_API_TEST
#include <soc/dnxc/dnxc_wb_test_utils.h>
#endif /* BCM_WARM_BOOT_API_TEST */
#include <sal/appl/sal.h>

/*
 * }
 */

/*
 * Externs objects:
 * {
 */
extern const dnx_init_step_t dnx_init_deinit_seq[];
extern const dnx_init_time_thresh_info_t dnx_init_deinit_time_thresh_expl[];
/*
 * }
 */

/*
 * Static objects:
 * {
 */
static utilex_seq_t *seq_ptr_arr[SOC_MAX_NUM_DEVICES];
/*
 * }
 */

/*
 * Functions.
 * {
 */
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
 *   step id == DNX_INIT_STEP_LAST_STEP.
 * \see
 *   * None
 */
static shr_error_e
dnx_init_step_list_count_steps(
    int unit,
    const dnx_init_step_t * step_list,
    int *nof_steps)
{
    int step_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * all lists must have at least one member - the last one - with step_id == DNX_INIT_STEP_LAST_STEP
     */
    for (step_index = 0; step_list[step_index].step_id != DNX_INIT_STEP_LAST_STEP; ++step_index)
    {
        /*
         * Do nothing
         */
    }

    *nof_steps = step_index;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Internal Init input verification function.
 *   returns error if contredicting flags or input data is used.
 *   Additional details about the input can be found in function 'dnx_init_advanced()'
 */
static shr_error_e
dnx_init_advanced_verify(
    int unit,
    int flags,
    dnx_init_info_t * dnx_init_info,
    int forward)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_init_info, _SHR_E_PARAM, "dnx_init_info");

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_init_step_list_destory(
    int unit,
    utilex_seq_step_t * step_list)
{
    int step_index;
    SHR_FUNC_INIT_VARS(unit);

    /** recursive destroy */
    for (step_index = 0; step_list[step_index].step_id != UTILEX_SEQ_STEP_LAST; step_index++)
    {
        if (step_list[step_index].step_sub_array != NULL)
        {
            SHR_IF_ERR_CONT(dnx_init_step_list_destory(unit, step_list[step_index].step_sub_array));
        }
    }

    /** destroy current */
    sal_free(step_list);

    SHR_FUNC_EXIT;
}
/**
 * \brief See in init.h
 */
shr_error_e
dnx_init_step_list_convert(
    int unit,
    const dnx_init_step_t * dnx_step_list,
    utilex_seq_step_t ** step_list)
{
    int list_size;
    int step_index;
    utilex_seq_step_t *step;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get list size
     */
    SHR_IF_ERR_EXIT(dnx_init_step_list_count_steps(unit, dnx_step_list, &list_size));
    list_size++; /** Count the last step too */

    /*
     * Allocate memory for list
     */
    *step_list = (utilex_seq_step_t *) sal_alloc(sizeof(utilex_seq_step_t) * list_size, "dnx init sequence list");
    SHR_NULL_CHECK(*step_list, _SHR_E_MEMORY, "failed to allocate memory for step list");

    /*
     * Convert each step
     */
    for (step_index = 0; step_index < list_size; step_index++)
    {
        step = &((*step_list)[step_index]);

        SHR_IF_ERR_EXIT(utilex_seq_step_t_init(unit, step));

        step->step_id = dnx_step_list[step_index].step_id;
        step->step_name = dnx_step_list[step_index].step_name;
        step->forward = dnx_step_list[step_index].init_function;
        step->backward = dnx_step_list[step_index].deinit_function;
        step->dyn_flags = dnx_step_list[step_index].flag_function;
        step->static_flags = dnx_step_list[step_index].step_flags;
        step->depends_on_step_id = dnx_step_list[step_index].depends_on_step_id;
        if (dnx_step_list[step_index].step_sub_array != NULL)
        {
            SHR_IF_ERR_EXIT(dnx_init_step_list_convert
                            (unit, dnx_step_list[step_index].step_sub_array, &(step->step_sub_array)));
        }

    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_init_seq_flags_get(
    int unit,
    int *flags)
{
    char *time_analyze_mode_str;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "time_test_en", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_TIME_STAMP;
    }

    if (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "time_log_en", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_TIME_LOG;
    }

    if (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mem_test_en", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_MEM_LEAK_DETECT;
    }

    if (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mem_log_en", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_MEM_LOG;
    }

    if (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "swstate_log_en", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_SWSTATE_LOG;
    }
    if (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "access_only", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_ACCESS_ONLY;
    }
    if (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "heat_up", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_HEAT_UP;
    }

    if (!((*flags & BCM_INIT_ADVANCED_F_ACCESS_ONLY) || (*flags & BCM_INIT_ADVANCED_F_HEAT_UP)))
    {
        if (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multithread_en", 0))
        {
            *flags |= BCM_INIT_ADVANCED_F_MULTITHREAD;
        }

        if (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "kbp_multithread_en", 0))
        {
            *flags |= BCM_INIT_ADVANCED_F_KBP_MULTITHREAD;
        }
    }

    time_analyze_mode_str = dnx_drv_soc_property_suffix_num_str_get(unit, -1, spn_CUSTOM_FEATURE, "time_analyze");
    /*
     * Make sure at least one of the strings is NULL terminated.
     */
    if (time_analyze_mode_str != NULL && (!sal_strcasecmp("PER_STEP", time_analyze_mode_str)))
    {
        *flags |= BCM_INIT_ADVANCED_F_TIME_ANALYZER_LOG;
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief See in init.h
 */
shr_error_e
dnx_time_thresh_cb(
    int unit,
    int step_id,
    uint32 flags,
    sal_usecs_t * time_thresh)
{
    int time_thresh_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (step_id == UTILEX_SEQ_STEP_INVALID)
    {
        /** Take the total threshold for the full sequence from DNX Data */
        *time_thresh = dnx_data_dev_init.time.init_total_thresh_get(unit);
    }
    else
    {
        if (flags & DNX_INIT_STEP_F_TIME_THRESH_EXPLICIT)
        {
            /** Take the threshold for the step from a user-defined array (dnx_init_deinit_time_thresh_expl)
              * of step IDs and thresholds */
            for (time_thresh_index = 0;
                 dnx_init_deinit_time_thresh_expl[time_thresh_index].step_id != DNX_INIT_STEP_LAST_STEP;
                 ++time_thresh_index)
            {
                if (dnx_init_deinit_time_thresh_expl[time_thresh_index].step_id == step_id)
                {

                    *time_thresh = dnx_init_deinit_time_thresh_expl[time_thresh_index].time_thresh;
                }
            }
        }
        else if (flags & DNX_INIT_STEP_F_TIME_THRESH_PER_DEVICE)
        {
            /** Take the threshold for the step from DNX Data */
            *time_thresh = dnx_data_dev_init.time.step_thresh_get(unit, step_id)->value;
        }
        else
        {
            /** Step used the default threshold */
            *time_thresh = BCM_INIT_STEP_TIME_THRESH_DEFAULT;
        }
    }

    SHR_FUNC_EXIT;
}

int
dnx_init_skip_print_decider_cb(
    int unit)
{
    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_production_ready))
    {
        return 0;
    }
    return 1;
}

/**
 * \brief - Runnig bcm init \ deinit sequnce (according to forward parameter)
 *          Additional info about the input can be found in 'dnx_init_advanced()'
 */
static shr_error_e
dnx_init_seq_convert(
    int unit,
    int flags,
    dnx_init_info_t * dnx_init_info,
    const dnx_init_step_t * step_list,
    utilex_seq_t * seq,
    int forward)
{
    SHR_FUNC_INIT_VARS(unit);

    if (forward == 1)
    {
                /** Init seq structure */
        SHR_IF_ERR_EXIT(utilex_seq_t_init(unit, seq));
    }

    
    dnx_init_seq_flags_get(unit, &flags);

    /** Configure sequence */
    /** Logging */
    seq->bsl_flags = BSL_LOG_MODULE;
    if (forward == 1)
    {
        sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "   %d: BCM Init:", unit);
    }
    else
    {
        sal_snprintf(seq->log_prefix, UTILEX_SEQ_LOG_PREFIX_SIZE, "   %d: BCM Deinit:", unit);
    }
    seq->log_severity = bslSeverityInfo;

    /** Parital seq */
    seq->first_step = dnx_init_info->first_step;
    seq->last_step = dnx_init_info->last_step;

    /** Seq utils */
    seq->time_log_en = (flags & BCM_INIT_ADVANCED_F_TIME_LOG) ? 1 : 0;
    seq->time_test_en = (flags & BCM_INIT_ADVANCED_F_TIME_STAMP) ? 1 : 0;
    seq->time_thresh_cb = dnx_time_thresh_cb;
    seq->mem_log_en = (flags & BCM_INIT_ADVANCED_F_MEM_LOG) ? 1 : 0;
    seq->mem_test_en = (flags & BCM_INIT_ADVANCED_F_MEM_LEAK_DETECT) ? 1 : 0;
    seq->swstate_log_en = (flags & BCM_INIT_ADVANCED_F_SWSTATE_LOG) ? 1 : 0;
    seq->access_only = (flags & BCM_INIT_ADVANCED_F_ACCESS_ONLY) ? 1 : 0;
    seq->heat_up = (flags & BCM_INIT_ADVANCED_F_HEAT_UP) ? 1 : 0;
    seq->time_analyzer_mode =
        (flags & BCM_INIT_ADVANCED_F_TIME_ANALYZER_LOG) ? UTILEX_TIME_ANALYZER_MODE_PER_INIT_STEP :
        UTILEX_TIME_ANALYZER_MODE_OFF;
    seq->skip_print_decider = dnx_init_skip_print_decider_cb;
    seq->tmp_allow_access_enable = dnxc_ha_tmp_allow_access_enable;
    seq->tmp_allow_access_disable = dnxc_ha_tmp_allow_access_disable;
    seq->kbp_multithread_en = (flags & BCM_INIT_ADVANCED_F_KBP_MULTITHREAD) ? 1 : 0;
#if 1
    seq->multithread_en = 0;
#endif

    if (seq->multithread_en || seq->kbp_multithread_en)
    {
        int advanced_feature_sum =
            seq->time_log_en + seq->time_test_en + seq->mem_log_en + seq->mem_test_en + seq->swstate_log_en +
            seq->access_only + seq->heat_up + seq->time_analyzer_mode;
        /** in order to use multi-threading, all other advanced features should be off */
        if (advanced_feature_sum)
        {
            SHR_ERR_EXIT(_SHR_E_INIT,
                         "In order to use advanced init features, multi-threading should be turned off (use custom_feature_multithread_en=0 and custom_feature_kbp_multithread_en=0)");
        }
    }

    if (forward == 1)
    {
        /** Convert sequence (only on init, deinit used the same data)*/
        SHR_IF_ERR_EXIT(dnx_init_step_list_convert(unit, step_list, &seq->step_list));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Runnig bcm init \ deinit sequnce (according to forward parameter)
 *          Additional info about the input can be found in 'dnx_init_advanced()'
 */
static shr_error_e
dnx_init_run(
    int unit,
    int flags,
    dnx_init_info_t * dnx_init_info,
    int forward)
{
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    SHR_IF_ERR_EXIT(dnx_init_advanced_verify(unit, flags, dnx_init_info, forward));

    if (forward == 1)
    {
        /*
         * Allocate memory for Init sequence only if it's init
         */
        seq_ptr_arr[unit] = (utilex_seq_t *) sal_alloc(sizeof(utilex_seq_t), "DNX Init sequence");
        SHR_NULL_CHECK(seq_ptr_arr[unit], _SHR_E_MEMORY, "Failed to allocate memory for Init sequence");
    }

    /*
     * Convert step list to general utilex_seq list
     */
    {
        SHR_IF_ERR_EXIT(dnx_init_seq_convert
                        (unit, flags, dnx_init_info, dnx_init_deinit_seq, seq_ptr_arr[unit], forward));
    }
    /*
     * Run list and check error just after utilex seq list destroy and the pointer is freed
     * This is done so that in case of an error the resources will be freed before exiting the function
     */
    rv = utilex_seq_run(unit, seq_ptr_arr[unit], forward);

    if (!forward)
    {
        /*
         * Only if it is deinit destroy step list (allocated in 'dnx_init_seq_convert()')
         */
        SHR_IF_ERR_EXIT(dnx_init_step_list_destory(unit, seq_ptr_arr[unit]->step_list));
        SHR_FREE(seq_ptr_arr[unit]);
    }

    /*
     * Check error
     */
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_rerun(
    int unit,
    int flags,
    dnx_init_info_t * dnx_init_info)
{
    const int forward = 1;
    utilex_seq_t seq;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_init_info, _SHR_E_PARAM, "dnx_init_info");

    /** tell SW state to ignore allocations **/
    SHR_IF_ERR_EXIT(dnxc_sw_state_skip_alloc_mode_set(unit, 1));

    /** Convert step list to general utilex_seq list, first in the forward direction, afterwards in backwords direction to align prints */
    SHR_IF_ERR_EXIT(dnx_init_seq_convert(unit, flags, dnx_init_info, dnx_init_deinit_seq, &seq, forward));
    SHR_IF_ERR_EXIT(dnx_init_seq_convert(unit, flags, dnx_init_info, dnx_init_deinit_seq, &seq, !forward));

    /** Run list backward */
    SHR_IF_ERR_EXIT(utilex_seq_run(unit, &seq, !forward));
    /** Run list forward */
    SHR_IF_ERR_EXIT(dnx_init_seq_convert(unit, flags, dnx_init_info, dnx_init_deinit_seq, &seq, forward));
    SHR_IF_ERR_EXIT(utilex_seq_run(unit, &seq, forward));

    /** destroy step list */
    SHR_IF_ERR_EXIT(dnx_init_step_list_destory(unit, seq.step_list));

    /** tell SW state not to ignore allocations anymore **/
    SHR_IF_ERR_EXIT(dnxc_sw_state_skip_alloc_mode_set(unit, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Init \ref dnx_init_info_t with default values.
 */
static shr_error_e
dnx_init_info_t_init(
    int unit,
    dnx_init_info_t * dnx_init_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_init_info, _SHR_E_MEMORY, "dnx_init_info");

    dnx_init_info->first_step = DNX_INIT_STEP_INVALID_STEP;
    dnx_init_info->last_step = DNX_INIT_STEP_INVALID_STEP;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Running full DNX BCM init sequence (without any debug or test flags).
 *          Runnig partial init or enable debug or test flags cen be done via 'dnx_init_advanced()'
 *
 * \param [in] unit - Unit #
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_init(
    int unit)
{
    int flags = 0;
    dnx_init_info_t dnx_init_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * set defaults for init info and flags.
     * currently no flags used by default.
     */
    SHR_IF_ERR_EXIT(dnx_init_info_t_init(unit, &dnx_init_info));

    /*
     * run internal init which is capable of getting arguments and flags.
     * we keep the bcm_dnx_init just for backward compitability, the new
     * function can use init info and flags.
     */
    SHR_IF_ERR_EXIT(dnx_init_run(unit, flags, &dnx_init_info, 1));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
dnx_init_is_init_done_get(
    int unit)
{
    int ret;
    shr_error_e err;
    uint32 *soc_flags_p;

    ret = FALSE;
    err = soc_control_element_address_get(unit, SOC_FLAGS, (void **) (&soc_flags_p));
    if (err != _SHR_E_NONE)
    {
        goto exit;
    }
    ret = (*soc_flags_p & dnx_drv_soc_f_all_modules_inited())? TRUE : FALSE;
exit:
    return (ret);
}

/**
 * \brief - new init function. wrapper to \ref dnx_init_run.
 * caller can decide himself on flags and init info.
 *
 * \param [in] unit - unit #
 * \param [in] init_advanced_info - additional user information for init seq, should be kept minimal.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_init_advanced(
    int unit,
    bcm_init_advanced_info_t * init_advanced_info)
{
    int flags = 0;
    dnx_init_info_t dnx_init_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * set defaults for init info.
     */
    SHR_IF_ERR_EXIT(dnx_init_info_t_init(unit, &dnx_init_info));

    SHR_IF_ERR_EXIT(dnx_init_run(unit, flags, &dnx_init_info, 1));

exit:
    SHR_FUNC_EXIT;
}

/*
 * IMPORTANT:
 *   The code here is empty and meaningless.
 *   It will probably need to be rewritten!
 *   See (_bcm_petra_attach) as example.
 *
 *   those are probably the minimal steps needed, this should be done, need to verify
 */
int
_bcm_dnx_attach(
    int unit,
    char *subtype)
{
    int dunit;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL;
    dunit = BCM_CONTROL(unit)->unit;
    if (dnx_drv_soc_unit_valid(dunit))
    {
        BCM_CONTROL(unit)->chip_vendor = dnx_drv_soc_pci_vendor(dunit);
        BCM_CONTROL(unit)->chip_device = dnx_drv_soc_pci_device(dunit);
        BCM_CONTROL(unit)->chip_revision = dnx_drv_soc_pci_revision(dunit);
        BCM_CONTROL(unit)->capability |= BCM_CAPA_SWITCH;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnx_info_get_verify(
    int unit,
    bcm_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_info_get(
    int unit,
    bcm_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_info_get_verify(unit, info));

    info->vendor = dnx_drv_soc_pci_vendor(unit);
    info->device = CMDEV(unit).dev.dev_id;
    info->revision = CMDEV(unit).dev.rev_id;
    info->capability = 0;

exit:
#ifdef BCM_WARM_BOOT_API_TEST
    dnxc_wb_test_mode_skip_wb_sequence(unit);
#endif /* BCM_WARM_BOOT_API_TEST */
    SHR_FUNC_EXIT;
}

int
_bcm_dnx_detach(
    int unit)
{
    int flags = 0;
    dnx_init_info_t dnx_init_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * set defaults for init info and flags.
     * currently no flags used by default.
     */
    SHR_IF_ERR_EXIT(dnx_init_info_t_init(unit, &dnx_init_info));

    /*
     * run internal init which is capable of getting arguments and flags.
     * we keep the bcm_dnx_init just for backward compitability, the new
     * function can use init info and flags.
     */
    SHR_IF_ERR_EXIT(dnx_init_run(unit, flags, &dnx_init_info, 0));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_init_only_enough_for_access(
    int unit,
    int action)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    sal_config_set("custom_feature_access_only", "1");
    sal_config_set("custom_feature_multithread_en*", NULL);
    sal_config_set("custom_feature_kbp_multithread_en*", NULL);
    SHR_IF_ERR_EXIT(bcm_init(unit));
    sal_config_set("custom_feature_access_only", NULL);
exit:
    SHR_FUNC_EXIT;
}

int
_bcm_dnx_match(
    int unit,
    char *subtype_a,
    char *subtype_b)
{
    int r_rv;
    r_rv = _SHR_E_UNAVAIL;
    return r_rv;
}

/**
 * \brief - Check if device is member in a group 'member_type'.
 *          Allows to write generic c-int and applications.
 *
 * \param [in] unit - unit #
 * \param [in] flags - set to 0
 * \param [in] member_type - group
 * \param [in] is_member - 1 if device is a member in the group (otherwise 0)
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_device_member_get(
    int unit,
    uint32 flags,
    bcm_device_member_t member_type,
    int *is_member)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify */
    SHR_NULL_CHECK(is_member, _SHR_E_PARAM, "is_member");
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "flags must be 0\n");

    /** get is_member */
    switch (member_type)
    {
        case bcmDeviceMemberDNX:
        case bcmDeviceMemberDNXC:
            *is_member = 1;
            break;

        default:
            *is_member = 0;
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get whether DNX BCM init sequence finished.
 *
 * \param [in] unit - Unit #
 * \return
 *   int - if init done - BCM_E_NONE, otherwise - BCM_E_UNIT
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_init_check(
    int unit)
{
    if (dnx_init_is_init_done_get(unit))
    {
        return BCM_E_NONE;
    }
    else
    {
        return BCM_E_UNIT;
    }
}
