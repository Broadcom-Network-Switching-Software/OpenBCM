/** \file src/bcm/dnxf/init.c
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *   This module calls the initialization routine of each BCM module.
 *
 * Initial System Configuration
 *
 *   Each module should initialize itself without reference to other BCM
 *   library modules to avoid a chicken-and-the-egg problem.  To do
 *   this, each module should initialize its respective internal state
 *   and hardware tables to match the Initial System Configuration.  The
 *   Initial System Configuration is:
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT
#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/counter.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_warm_boot.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_dev_init.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/dnxc_ha.h>

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <bcm/init.h>

#include <bcm_int/control.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/common/family.h>
#include <bcm_int/common/debug.h>

#include <bcm_int/dnxf_dispatch.h>
#include <bcm_int/dnxf/init.h>
#include <bcm_int/dnxf/stat.h>
#include <bcm_int/dnxf/stack.h>
#include <bcm_int/dnxf/port.h>
#include <bcm_int/dnxf/multicast.h>
#include <bcm_int/dnxf/link.h>
#include <bcm_int/dnxf/fabric.h>
#include <bcm_int/dnxf/rx.h>

#include <bcm_int/dnxf/tune/tune.h>

#ifdef BCM_WARM_BOOT_API_TEST
#include <soc/dnxc/dnxc_wb_test_utils.h>
#endif
int _bcm_dnxf_detach(
    int unit);

static int _bcm_dnxf_init_finished_ok[BCM_MAX_NUM_UNITS] = { 0 };

#define BCM_DNXF_DRV_INIT_LOG(_unit_, _msg_str_)\
                LOG_INFO(BSL_LS_BCM_INIT,\
                         (BSL_META_U(_unit_,\
                                     "    + %d: %s\n"), _unit_ , _msg_str_))

shr_error_e dnxf_soc_flags_init(int unit);
shr_error_e dnxf_soc_flags_deinit(int unit);
shr_error_e _dnxf_data_init(int unit);
shr_error_e _dnxf_sw_state_init(int unit);
shr_error_e _dnxf_sw_state_deinit(int unit);
shr_error_e dnxf_soc_init(int unit);
shr_error_e dnxf_soc_deinit(int unit);
shr_error_e _dnxf_tune_init(int unit);
shr_error_e dnxf_init_done_init(int unit);
shr_error_e dnxf_init_done_deinit(int unit);
/**
 * \brief
 * Time threshold flags - used for testing how much time each step in the Init takes.
 * If flag is set to PER_DEVICE the value will be taken from DNX data. THIS FLAG CAN NOT BE USED FOR STEPS BEFORE DNX DATA IS INITIALIZED
 * If no flag is specified, default value will be assumed.
 */
#define TIME_PER_DEV   DNXF_INIT_STEP_F_TIME_THRESH_PER_DEVICE

/**
 * \brief step should be skipped in warm reboot
 */
#define WB_SKIP     DNXF_INIT_STEP_F_WB_SKIP

/**
 * \brief Init-Deinit High level seq.
 *
 * For specific info on steps look for each function description
 */
const dnxf_init_step_t dnxf_init_deinit_seq[] = {
  /* STEP_ID,                          STEP_NAME,              STEP_INIT,                    STEP_DEINIT,                         STEP_FLAGS */
  /*                                                              |                             ^                                            */
  /*                                                             \ /                           / \                                           */
  /*                                                              V                             |                                            */
    {DNXF_INIT_STEP_MARK_NOT_INITED,   "Soc Flags",            dnxf_soc_flags_init,           dnxf_soc_flags_deinit,              0},
    {DNXF_INIT_STEP_DNX_DATA,          "DNXF_Data",            _dnxf_data_init,               dnxf_data_deinit,                   0},
    {DNXF_INIT_STEP_SW_STATE,          "SW_State",             _dnxf_sw_state_init,           _dnxf_sw_state_deinit,              0},
    {DNXF_INIT_STEP_SOC,               "SOC Init",             dnxf_soc_init,                 dnxf_soc_deinit,                    TIME_PER_DEV},
    {DNXF_INIT_STEP_PORT,              "Port",                 dnxf_port_init,                dnxf_port_deinit,                   TIME_PER_DEV},
    {DNXF_INIT_STEP_LINKSCAN,          "Linkscan",             dnxf_linkscan_init,            dnxf_linkscan_deinit,               0},
    {DNXF_INIT_STEP_STAT,              "Stat",                 dnxf_stat_init,                dnxf_stat_deinit,                   0},
    {DNXF_INIT_STEP_STACK,             "Stack",                dnxf_stk_init,                 dnxf_stk_deinit,                    0},
    {DNXF_INIT_STEP_MULTICAST,         "Multicast",            dnxf_multicast_init,           dnxf_multicast_deinit,              0},
    {DNXF_INIT_STEP_FABRIC,            "Fabric",               dnxf_fabric_init,              dnxf_fabric_deinit,                 TIME_PER_DEV},
    {DNXF_INIT_STEP_RX,                "RX",                   dnxf_rx_init,                  dnxf_rx_deinit,                     0},
    {DNXF_INIT_STEP_TUNE,              "Tune",                 dnxf_tune_init,                NULL,                               WB_SKIP},
    {DNXF_INIT_STEP_INIT_DONE,         "Init Done",            dnxf_init_done_init,           dnxf_init_done_deinit,              0},
    {DNXF_INIT_STEP_LAST_STEP,         "Last Step",            NULL,                          NULL,                               0}
};

static utilex_seq_t *seq_ptr_arr[SOC_MAX_NUM_DEVICES];



static shr_error_e
dnxf_init_seq_flags_get(
    int unit,
    int *flags)
{
    char *time_analyze_mode_str;
    SHR_FUNC_INIT_VARS(unit);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "time_test_en", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_TIME_STAMP;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "time_log_en", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_TIME_LOG;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mem_test_en", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_MEM_LEAK_DETECT;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mem_log_en", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_MEM_LOG;
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "swstate_log_en", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_SWSTATE_LOG;
    }
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "access_only", 0))
    {
        *flags |= BCM_INIT_ADVANCED_F_ACCESS_ONLY;
    }
    time_analyze_mode_str = soc_property_suffix_num_str_get(unit, -1, spn_CUSTOM_FEATURE, "time_analyze");
    if (time_analyze_mode_str != NULL && (!sal_strcasecmp("PER_STEP", time_analyze_mode_str)))
    {
        *flags |= BCM_INIT_ADVANCED_F_TIME_ANALYZER_LOG;
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
 *   step id == DNXF_INIT_STEP_LAST_STEP.
 * \see
 *   * None
 */
static shr_error_e
dnxf_init_step_list_count_steps(
    int unit,
    const dnxf_init_step_t * step_list,
    int *nof_steps)
{
    int step_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * all lists must have at least one member - the last one - with step_id == DNXF_INIT_STEP_LAST_STEP
     */
    for (step_index = 0; step_list[step_index].step_id != DNXF_INIT_STEP_LAST_STEP; ++step_index)
    {
        /*
         * Do nothing
         */
    }

    *nof_steps = step_index;

    SHR_FUNC_EXIT;
}

/**
 * \brief Init \ref dnxf_init_info_t with default values.
 */
static shr_error_e
dnxf_init_info_t_init(
    int unit,
    dnxf_init_info_t * dnxf_init_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnxf_init_info, _SHR_E_MEMORY, "dnxf_init_info");

    dnxf_init_info->first_step = DNXF_INIT_STEP_INVALID_STEP;
    dnxf_init_info->last_step = DNXF_INIT_STEP_INVALID_STEP;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Internal Init input verification function.
 *   returns error if contradicting flags or input data is used.
 *   Additional details about the input can be found in function 'dnx_init_advanced().'
 *   Not yet implemented for DNXF!
 */
static shr_error_e
dnxf_init_advanced_verify(
    int unit,
    int flags,
    dnxf_init_info_t * dnxf_init_info,
    int forward)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnxf_init_info, _SHR_E_PARAM, "dnxf_init_info");

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnxf_init_step_list_destory(
    int unit,
    utilex_seq_step_t * step_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /** destroy current */
    sal_free(step_list);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief See in init.h
 */
shr_error_e
dnxf_init_step_list_convert(
    int unit,
    const dnxf_init_step_t * dnxf_step_list,
    utilex_seq_step_t ** step_list)
{
    int list_size;
    int step_index;
    utilex_seq_step_t *step;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get list size
     */
    SHR_IF_ERR_EXIT(dnxf_init_step_list_count_steps(unit, dnxf_step_list, &list_size));
    list_size++; /** Count the last step too */

    /*
     * Allocate memory for list
     */
    *step_list = (utilex_seq_step_t *) sal_alloc(sizeof(utilex_seq_step_t) * list_size, "dnxf init sequence list");
    SHR_NULL_CHECK(*step_list, _SHR_E_MEMORY, "failed to allocate memory for step list");

    /*
     * Convert each step
     */
    for (step_index = 0; step_index < list_size; step_index++)
    {
        step = &((*step_list)[step_index]);

        SHR_IF_ERR_EXIT(utilex_seq_step_t_init(unit, step));

        step->step_id =         dnxf_step_list[step_index].step_id;
        step->step_name =       dnxf_step_list[step_index].step_name;
        step->forward =         dnxf_step_list[step_index].init_function;
        step->backward =        dnxf_step_list[step_index].deinit_function;
        step->dyn_flags =       NULL;
        step->static_flags =    dnxf_step_list[step_index].step_flags;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief See in init.h
 */
shr_error_e dnxf_time_thresh_cb(
    int unit,
    int step_id,
    uint32 flags,
    sal_usecs_t *time_thresh)
{
    SHR_FUNC_INIT_VARS(unit);

    if (step_id == UTILEX_SEQ_STEP_INVALID)
    {
        /** Take the total threshold for the full sequence from DNXF Data */
        *time_thresh = dnxf_data_dev_init.time.init_total_thresh_get(unit);
    }
    else
    {
        if (flags & DNXF_INIT_STEP_F_TIME_THRESH_PER_DEVICE)
        {
            /** Take the threshold for the step from DNX Data */
            *time_thresh = dnxf_data_dev_init.time.step_thresh_get(unit, step_id)->value;
        }
        else
        {
            /** Step used the default threshold */
            *time_thresh = BCM_DNXF_INIT_STEP_TIME_THRESH_DEFAULT;
        }
    }

    SHR_FUNC_EXIT;
}

int
dnxf_init_skip_print_decider_cb(int unit){
	if (dnxf_data_device.general.feature_get(unit, dnxf_data_device_general_production_ready)) {
		return 0;
	}
	return 1;
}

static shr_error_e
dnxf_init_seq_convert(
    int unit,
    int flags,
    dnxf_init_info_t * dnxf_init_info,
    const dnxf_init_step_t * step_list,
    utilex_seq_t * seq,
    int forward)
{
    SHR_FUNC_INIT_VARS(unit);

    if (forward == 1)
    {
        /** Init seq structure */
        SHR_IF_ERR_EXIT(utilex_seq_t_init(unit, seq));
    }

    
    dnxf_init_seq_flags_get(unit, &flags);

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

    /** Partial seq */
    seq->first_step = dnxf_init_info->first_step;;
    seq->last_step = dnxf_init_info->last_step;

    /** Seq utils */
    seq->time_log_en =      (flags & BCM_INIT_ADVANCED_F_TIME_LOG) ? 1 : 0;
    seq->time_test_en =     (flags & BCM_INIT_ADVANCED_F_TIME_STAMP) ? 1 : 0;
    seq->time_thresh_cb =   dnxf_time_thresh_cb;
    seq->mem_log_en =       (flags & BCM_INIT_ADVANCED_F_MEM_LOG) ? 1 : 0;
    seq->mem_test_en =      (flags & BCM_INIT_ADVANCED_F_MEM_LEAK_DETECT) ? 1 : 0;
    seq->swstate_log_en =   (flags & BCM_INIT_ADVANCED_F_SWSTATE_LOG) ? 1 : 0;
    seq->access_only =      (flags & BCM_INIT_ADVANCED_F_ACCESS_ONLY) ? 1 : 0;
    seq->time_analyzer_mode =
        (flags & BCM_INIT_ADVANCED_F_TIME_ANALYZER_LOG) ? UTILEX_TIME_ANALYZER_MODE_PER_INIT_STEP :
        UTILEX_TIME_ANALYZER_MODE_OFF;
    seq->tmp_allow_access_enable =      dnxc_ha_tmp_allow_access_enable;
    seq->tmp_allow_access_disable =     dnxc_ha_tmp_allow_access_disable;
    seq->skip_print_decider = dnxf_init_skip_print_decider_cb;

    if (forward == 1)
    {
        /** Convert sequence (only on init, deinit used the same data)*/
        SHR_IF_ERR_EXIT(dnxf_init_step_list_convert(unit, step_list, &seq->step_list));
    }

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief - Runnig bcm init \ deinit sequnce (according to forward parameter)
 *          Additional info about the input can be found in 'dnxf_init_advanced()'
 *          Advanced init not yet implemented for DNXF
 */
shr_error_e
dnxf_init_run(
    int unit,
    int flags,
    dnxf_init_info_t * dnxf_init_info,
    int forward)
{
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify
     */
    SHR_IF_ERR_EXIT(dnxf_init_advanced_verify(unit, flags, dnxf_init_info, forward));

    if (forward == 1)
    {
        /*
         * Allocate memory for Init sequence only if it's init
         */
        seq_ptr_arr[unit] = (utilex_seq_t *) sal_alloc(sizeof(utilex_seq_t), "DNXF Init sequence");
        SHR_NULL_CHECK(seq_ptr_arr[unit], _SHR_E_MEMORY, "Failed to allocate memory for Init sequence");
    }

    /*
     * Convert step list to general utilex_seq list
     */
    SHR_IF_ERR_EXIT(dnxf_init_seq_convert(unit, flags, dnxf_init_info, dnxf_init_deinit_seq, seq_ptr_arr[unit], forward));

    /*
     * Run list and check error just after utilex seq list destroy and the pointer is freed
     * This is done so that in case of an error the resources will be freed before exiting the function
     */
    rv = utilex_seq_run(unit, seq_ptr_arr[unit], forward);

    if (!forward)
    {
        /*
         * Only if it is deinit destroy step list (allocated in 'dnxf_init_seq_convert()')
         */
        SHR_IF_ERR_EXIT(dnxf_init_step_list_destory(unit, seq_ptr_arr[unit]->step_list));
        SHR_FREE(seq_ptr_arr[unit]);
    }

    /*
     * Check error
     */
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_init_check(
    int unit)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    rc = (_bcm_dnxf_init_finished_ok[unit] == 1 ? _SHR_E_NONE : _SHR_E_UNIT);
    if (rc == _SHR_E_UNIT)
    {
        /*
         * bcm init not finished - return _SHR_E_UNIT withot error printing
         */
        SHR_SET_CURRENT_ERR(rc);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rc);
    }
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_init_selective(
    int unit,
    uint32 module_number)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}

int dnxf_sw_state_init(int unit)
{
    uint32 warmboot = 0;
    uint32 sw_state_max_size = 0;

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    /* check in DNX Data if we are enabling wb mode for this unit */
    warmboot = dnxf_data_device.ha.warmboot_support_get(unit);
#endif /* BCM_WARM_BOOT_SUPPORT */
    dnxc_sw_state_is_warmboot_supported_set(unit, warmboot);
    sw_state_max_size = dnxf_data_device.ha.sw_state_max_size_get(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_init(unit, warmboot, sw_state_max_size));

exit:
    SHR_FUNC_EXIT; 
}

int
bcm_dnxf_info_get(
    int unit,
    bcm_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    info->vendor = SOC_PCI_VENDOR(unit);
    info->device = SOC_PCI_DEVICE(unit);
    info->revision = SOC_PCI_REVISION(unit);
    info->capability = BCM_CAPA_LOCAL | BCM_CAPA_FABRIC;
exit:
    SHR_FUNC_EXIT;
}

int dnxf_sw_state_deinit(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_deinit(unit));

exit:
    SHR_FUNC_EXIT; 

}


int
_bcm_dnxf_attach(
    int unit,
    char *subtype)
{
    int dunit;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(subtype);

    BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL;

    dunit = BCM_CONTROL(unit)->unit;

    if (SOC_UNIT_VALID(dunit))
    {
        BCM_CONTROL(unit)->chip_vendor = SOC_PCI_VENDOR(dunit);
        BCM_CONTROL(unit)->chip_device = SOC_PCI_DEVICE(dunit);
        BCM_CONTROL(unit)->chip_revision = SOC_PCI_REVISION(dunit);
        BCM_CONTROL(unit)->capability |= BCM_CAPA_FABRIC;
    }

    SHR_IF_ERR_EXIT(bcm_dnxf_init(unit));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_init(
    int unit)
{

    int flags = 0, forward = 1;
    dnxf_init_info_t dnxf_init_info;

    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    DNXF_UNIT_LOCK_TAKE(unit);

    /*
     * set defaults for init info and flags.
     * currently no flags used by default.
     */
    SHR_IF_ERR_EXIT(dnxf_init_info_t_init(unit, &dnxf_init_info));

    SHR_IF_ERR_EXIT(dnxf_init_run(unit, flags, &dnxf_init_info, forward));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    if (SHR_FAILURE(_func_rv))
    {
        _bcm_dnxf_detach(unit);
    }
    SHR_FUNC_EXIT;
}

int
_bcm_dnxf_detach(
    int unit)
{
    int flags = 0, forward = 0;
    dnxf_init_info_t dnxf_init_info;

    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_CONT(bcm_dnxf_linkscan_enable_set(unit, 0));

    SHR_IF_ERR_CONT(soc_counter_stop(unit));
    /*
     * Holding the unit lock or disabling hw operations
     * might prevent threads of clean exit.
     * That's why lock is taken after the threads exit 
     */
    DNXF_UNIT_LOCK_TAKE(unit);

    /*
     * set defaults for init info and flags.
     * currently no flags used by default.
     */
    SHR_IF_ERR_EXIT(dnxf_init_info_t_init(unit, &dnxf_init_info));

    SHR_IF_ERR_CONT(dnxf_init_run(unit, flags, &dnxf_init_info, forward));

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

int
_bcm_dnxf_match(
    int unit,
    char *subtype_a,
    char *subtype_b)
{
    SHR_FUNC_INIT_VARS(unit);
    COMPILER_REFERENCE(unit);
    SHR_IF_ERR_EXIT(sal_strcmp(subtype_a, subtype_b));
exit:
    SHR_FUNC_EXIT;
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
bcm_dnxf_device_member_get(
    int unit,
    uint32 flags,
    bcm_device_member_t member_type,
    int *is_member)
{
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    /** verify */
    SHR_NULL_CHECK(is_member, _SHR_E_PARAM, "is_member");
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "flags must be 0\n");

    /** get is_member */
    switch (member_type)
    {
        case bcmDeviceMemberDNXF:
        case bcmDeviceMemberDNXC:
            *is_member = 1;
            break;

        default:
            *is_member = 0;
            break;
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}


/**
 * \brief - First and last step for the init/deinit step
 *
 * \param [in] unit - unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
soc_dnxf_mark_not_inited(
    int unit)
{
    soc_control_t *soc;
    SHR_FUNC_INIT_VARS(unit);

    soc = SOC_CONTROL(unit);
    soc->soc_flags &= ~SOC_F_INITED;

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - First step for the init sequence
 *
 * \param [in] unit - unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnxf_soc_flags_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_mark_not_inited(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Last step for the deinit sequence
 *
 * \param [in] unit - unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnxf_soc_flags_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_mark_not_inited(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init DNX data
 *
 * \param [in] unit - unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e _dnxf_data_init
(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
    * Update dnxf_data values after the rc_load
    * DNX data init was once called in soc_dnxf_attach so we have to deinit it first before we refresh it.
    * The call in soc_dnxf_attach is not something we want but we call dnxf data defines in 
    * soc_ramon_drv_block_valid_get called from soc_dnxf_info_config
    */
    SHR_IF_ERR_EXIT(dnxf_data_deinit(unit));
    SHR_IF_ERR_EXIT(dnxf_data_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init SW state modules
 *
 * \param [in] unit - unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e _dnxf_sw_state_init
(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_ha_init(unit));

    SHR_IF_ERR_EXIT(dnxf_sw_state_init(unit));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Deinit Infrastructure modules
 *
 * \param [in] unit - unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e _dnxf_sw_state_deinit
(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxf_sw_state_deinit(unit));

    SHR_IF_ERR_EXIT(soc_dnxf_ha_deinit(unit));

#ifdef BCM_WARM_BOOT_SUPPORT
    SHR_IF_ERR_EXIT(soc_scache_detach(unit));
#endif
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init soc
 *
 * \param [in] unit - unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxf_soc_init
(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_init(unit, 1));

    SHR_IF_ERR_EXIT(bcm_chip_family_set(unit, BCM_FAMILY_DNXF));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init soc
 *
 * \param [in] unit - unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxf_soc_deinit
(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * declare that driver is not detaching
     */
    SOC_DETACH(unit, 0);

    SHR_IF_ERR_EXIT(soc_dnxf_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Mark INIT done
 *
 * \param [in] unit - unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxf_init_done_init
(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Update dnxf data that init done
     */
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_set(unit, DNXC_DATA_STATE_F_BCM_INIT_DONE));

    _bcm_dnxf_init_finished_ok[unit] = 1;

    /*
     * Mark soc control as done
     */
    SOC_CONTROL(unit)->soc_flags |= SOC_F_INITED;
    SOC_CONTROL(unit)->soc_flags |= SOC_F_ALL_MODULES_INITED;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Mark INIT done
 *
 * \param [in] unit - unit
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxf_init_done_deinit
(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

     /*
     * declare that driver is detaching
     */
    SOC_DETACH(unit, 1);

    /*
     * Mark soc control as not done
     */
    SOC_CONTROL(unit)->soc_flags &= ~SOC_F_INITED;
    SOC_CONTROL(unit)->soc_flags &= ~SOC_F_ALL_MODULES_INITED;

    _bcm_dnxf_init_finished_ok[unit] = 0;

    /* Update dnxf data that deinit begin */
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_reset(unit, DNXC_DATA_STATE_F_BCM_INIT_DONE));

exit:
    SHR_FUNC_EXIT;
}


#undef BSL_LOG_MODULE
