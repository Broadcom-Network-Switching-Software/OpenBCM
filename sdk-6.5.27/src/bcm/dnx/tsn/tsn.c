
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

#include <bcm/switch.h>
#include <bcm_int/dnx/tsn/tsn.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_tsn_access.h>
#include <bcm_int/dnx/auto_generated/dnx_switch_dispatch.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>

#include "tsn_counter.h"
#include "tsn_taf.h"
#include "tsn_tas.h"
#include "tsn_cqf.h"
#include "tsn_thread.h"
#include "tsn_dbal.h"

int
bcm_dnx_tsn_taf_gate_create(
    int unit,
    int flags,
    int *taf_gate_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_create(unit, flags, taf_gate_id));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_tsn_taf_gate_destroy(
    int unit,
    int taf_gate_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_destroy(unit, taf_gate_id));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_tsn_taf_profile_create(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_t * profile,
    bcm_tsn_taf_profile_id_t * pid)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_create(unit, taf_gate, profile, pid));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_tsn_taf_profile_destroy(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_destroy(unit, taf_gate, pid));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_tsn_taf_profile_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_set(unit, taf_gate, pid, profile));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_tsn_taf_profile_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_get(unit, taf_gate, pid, profile));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_tsn_taf_profile_commit(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_commit(unit, taf_gate, pid));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_tsn_taf_profile_status_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_status_t * status)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_status_get(unit, taf_gate, pid, status));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_tsn_taf_control_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_control_t type,
    uint32 arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmTsnTafControlReset:
            SHR_IF_ERR_EXIT(dnx_tsn_taf_control_reset(unit, taf_gate, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "type not supported %d", type);
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_tsn_taf_control_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_control_t type,
    uint32 *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmTsnTafControlReset:
            SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");
            *arg = 0;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "type not supported %d", type);
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_tsn_cqf_config_set(
    int unit,
    uint32 flags,
    bcm_gport_t port,
    bcm_tsn_cqf_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_set(unit, flags, port, config));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_tsn_cqf_config_get(
    int unit,
    uint32 flags,
    bcm_gport_t port,
    bcm_tsn_cqf_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_cqf_config_get(unit, flags, port, config));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_interrupt_init(
    int unit)
{
    bcm_switch_event_control_t type;

    SHR_FUNC_INIT_VARS(unit);

    type.index = BCM_CORE_ALL;
    type.action = bcmSwitchEventUnmaskAndClearDisable;
    type.event_id = dnx_data_tsn.time.tas_tod_update_interrupt_id_get(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, TRUE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_tas_tod_update_clear(
    int unit)
{
    bcm_switch_event_control_t type;

    SHR_FUNC_INIT_VARS(unit);

    type.index = BCM_CORE_ALL;
    type.action = bcmSwitchEventClear;
    type.event_id = dnx_data_tsn.time.tas_tod_update_interrupt_id_get(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, TRUE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_tas_tod_update_unmask(
    int unit)
{
    bcm_switch_event_control_t type;

    SHR_FUNC_INIT_VARS(unit);

    type.index = BCM_CORE_ALL;
    type.action = bcmSwitchEventMask;
    type.event_id = dnx_data_tsn.time.tas_tod_update_interrupt_id_get(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, FALSE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_tas_tod_update_handle(
    int unit)
{
    int is_restart_required;
    int is_mutex_locked = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    SHR_IF_ERR_EXIT(dnx_tsn_tas_tod_update_clear(unit));

    SHR_IF_ERR_EXIT(dnx_tsn_counter_restart_required_get(unit, &is_restart_required));
    if (is_restart_required)
    {
        /* *INDENT-OFF* */
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "TSN Counter Restart !!!!!!!!!!! \n")));
        

        
        SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_deactivate_all(unit));

        
        SHR_IF_ERR_EXIT(dnx_tsn_tas_port_deactivate_all(unit));

        
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_deactivate_all(unit));

        
        SHR_IF_ERR_EXIT(dnx_tsn_counter_restart(unit));

        
        SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_base_time_restart(unit));

        
        SHR_IF_ERR_EXIT(dnx_tsn_tas_port_base_time_restart(unit));

        
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_port_base_time_restart(unit));
    }

exit:

    SHR_IF_ERR_CONT(dnx_tsn_tas_tod_update_unmask(unit));

    if (is_mutex_locked)
    {
        
        SHR_IF_ERR_CONT(dnx_tsn_db.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_tsn_tas_tod_update_interrupt_handler(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_tsn.general.feature_get(unit, dnx_data_tsn_general_tsn_thread))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_thread_tas_tod_update_interrupt_handler(unit));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TAS ToD interrupt not support for this device\n");
    }

exit:
    SHR_FUNC_EXIT;
}




static shr_error_e
dnx_tsn_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.init(unit));

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.mutex.create(unit));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_tsn_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_tsn.general.feature_get(unit, dnx_data_tsn_general_power_down))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_power_down_set(unit, 1));
    }
    else
    {

        
        SHR_IF_ERR_EXIT(dnx_tsn_sw_state_init(unit));

        
        SHR_IF_ERR_EXIT(dnx_tsn_counter_init(unit));

        
        SHR_IF_ERR_EXIT(dnx_tsn_taf_init(unit));

        
        SHR_IF_ERR_EXIT(dnx_tsn_tas_init(unit));

        
        SHR_IF_ERR_EXIT(dnx_tsn_cqf_init(unit));

        
        if (dnx_data_tsn.general.feature_get(unit, dnx_data_tsn_general_tsn_thread))
        {
            SHR_IF_ERR_EXIT(dnx_tsn_thread_db_init(unit));
        }

        
        SHR_IF_ERR_EXIT(dnx_tsn_interrupt_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}
