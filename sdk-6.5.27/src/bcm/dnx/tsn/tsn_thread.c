
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
#include <bcm/tsn.h>
#include <bcm_int/dnx/tsn/tsn.h>
#include <bcm_int/dnx/auto_generated/dnx_switch_dispatch.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_tsn_access.h>
#include <shared/utilex/utilex_ptp_time.h>
#include <shared/utilex/utilex_u64.h>

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>

#include "tsn_thread.h"
#include "tsn_counter.h"
#include "tsn_taf.h"
#include "tsn_tas.h"

#define DNX_TSN_THREAD_NAME "TSN profile manager thread"

#define DNX_TSN_THREAD_INTERVAL_MAX_USEC  (1800000000)
#define DNX_TSN_THREAD_MAX_NOF_PIDS (DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE + DNX_DATA_MAX_TSN_TAS_NOF_TAS_PORTS * DNX_DATA_MAX_TSN_TAS_MAX_PROFILES_PER_PORT)

#define DNX_TSN_THREAD_TAS_CORE_ID (0)

typedef struct
{
    SHR_BITDCL bitmap[_SHR_BITDCLSIZE(DNX_TSN_THREAD_MAX_NOF_PIDS)];
} dnx_tsn_thread_pid_bmp_t;

#define DNX_TSN_THREAD_PID_BMP_INIT(pid_bmp) \
    SHR_BITCLR_RANGE((pid_bmp)->bitmap, 0, DNX_TSN_THREAD_MAX_NOF_PIDS)

#define DNX_TSN_THREAD_PID_BMP_IS_EMPTY(pid_bmp) \
    SHR_BITNULL_RANGE((pid_bmp)->bitmap, 0, DNX_TSN_THREAD_MAX_NOF_PIDS)

#define DNX_TSN_THREAD_PID_BMP_BITSET(pid_bmp, type, pid)  \
    SHR_BITSET((pid_bmp)->bitmap, ((type == DNX_TSN_GATE_TYPE_TAF) ? 0 : DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE) + pid)

#define DNX_TSN_THREAD_PID_BMP_IS_BITSET(pid_bmp, type, pid)  \
    SHR_IS_BITSET((pid_bmp)->bitmap, ((type == DNX_TSN_GATE_TYPE_TAF) ? 0 : DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE) + pid)

#define DNX_TSN_THREAD_PID_BMP_BIT_ITER(pid_bmp, iter) \
    SHR_BIT_ITER((pid_bmp)->bitmap, DNX_TSN_THREAD_MAX_NOF_PIDS, iter)

#define DNX_TSN_THREAD_PID_BMP_ITER_PID_GET(iter) \
    (iter < DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE ? iter : iter - DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE)

#define DNX_TSN_THREAD_PID_BMP_ITER_TYPE_GET(iter) \
    (iter < DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE ? DNX_TSN_GATE_TYPE_TAF : DNX_TSN_GATE_TYPE_TAS)

#define DNX_TSN_THREAD_PID_BMP_TAF_BMP_CONVERT(pid_bmp, taf_bmp) \
    SHR_BITCOPY_RANGE((taf_bmp)->bitmap, 0,(pid_bmp)->bitmap, 0, DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE)

#define DNX_TSN_THREAD_PID_BMP_TAS_BMP_CONVERT(pid_bmp, tas_bmp) \
    SHR_BITCOPY_RANGE((tas_bmp)->bitmap, 0,(pid_bmp)->bitmap, DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE, DNX_DATA_MAX_TSN_TAS_NOF_TAS_PORTS * DNX_DATA_MAX_TSN_TAS_MAX_PROFILES_PER_PORT)

#define DNX_TSN_THREAD_TAF_BMP_PID_BMP_CONVERT(taf_bmp, pid_bmp) \
    SHR_BITCOPY_RANGE((pid_bmp)->bitmap, 0,(taf_bmp)->bitmap, 0, DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE)

#define DNX_TSN_THREAD_TAS_BMP_PID_BMP_CONVERT(tas_bmp, pid_bmp) \
    SHR_BITCOPY_RANGE((pid_bmp)->bitmap, DNX_DATA_MAX_TSN_TAF_NOF_GATES * DNX_DATA_MAX_TSN_TAF_MAX_PROFILES_PER_GATE,(tas_bmp)->bitmap, 0, DNX_DATA_MAX_TSN_TAS_NOF_TAS_PORTS * DNX_DATA_MAX_TSN_TAS_MAX_PROFILES_PER_PORT)

shr_error_e dnx_tsn_thread_main(
    int unit,
    void *user_data);

shr_error_e
dnx_tsn_thread_db_init(
    int unit)
{
    sw_state_ll_init_info_t init_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.create(unit));

    sal_memset(&init_info, 0x0, sizeof(init_info));

    init_info.max_nof_elements = dnx_data_tsn.taf.nof_gates_get(unit) + dnx_data_tsn.tas.nof_tas_ports_get(unit);
    init_info.key_cmp_cb_name = "sw_state_sorted_list_cmp_uint64";

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.create_empty(unit, &init_info));

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.taf_gates_to_reset.alloc_bitmap(unit));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tas_ports_to_reset.alloc_bitmap(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_thread_create(
    int unit)
{
    shr_thread_manager_config_t tsn_thread_config_info;
    shr_thread_manager_handler_t tsn_thread_handler;
    SHR_FUNC_INIT_VARS(unit);

    shr_thread_manager_config_t_init(&tsn_thread_config_info);
    tsn_thread_config_info.name = DNX_TSN_THREAD_NAME;
    tsn_thread_config_info.bsl_module = BSL_LOG_MODULE;
    tsn_thread_config_info.type = SHR_THREAD_MANAGER_TYPE_EVENT;
    tsn_thread_config_info.interval = sal_sem_FOREVER;
    tsn_thread_config_info.callback = dnx_tsn_thread_main;
    SHR_IF_ERR_EXIT(shr_thread_manager_create(unit, &tsn_thread_config_info, &tsn_thread_handler));

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tsn_thread_handler.set(unit, tsn_thread_handler));

    SHR_IF_ERR_EXIT(shr_thread_manager_trigger(tsn_thread_handler));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_thread_destory(
    int unit)
{

    shr_thread_manager_handler_t tsn_thread_handler;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tsn_thread_handler.get(unit, &tsn_thread_handler));

    if (tsn_thread_handler != NULL)
    {
        SHR_IF_ERR_EXIT(shr_thread_manager_destroy(&tsn_thread_handler));
    }

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tsn_thread_handler.set(unit, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_thread_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_tsn.general.feature_get(unit, dnx_data_tsn_general_power_down))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_thread_create(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_thread_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_tsn.general.feature_get(unit, dnx_data_tsn_general_power_down))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_thread_destory(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_thread_trigger(
    int unit)
{
    shr_thread_manager_handler_t tsn_thread_handler;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tsn_thread_handler.get(unit, &tsn_thread_handler));
    SHR_IF_ERR_EXIT(shr_thread_manager_trigger(tsn_thread_handler));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_thread_tas_tod_update_handle(
    int unit)
{
    uint32 value;
    bcm_switch_event_control_t type;

    SHR_FUNC_INIT_VARS(unit);

    type.index = DNX_TSN_THREAD_TAS_CORE_ID;
    type.action = bcmSwitchEventRead;
    type.event_id = dnx_data_tsn.time.tas_tod_update_interrupt_id_get(unit);
    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, &value));

    if (value)
    {

        SHR_IF_ERR_EXIT(dnx_tsn_tas_tod_update_handle(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_thread_gates_reset(
    int unit)
{
    int is_mutex_locked = FALSE;
    CONST SHR_BITDCL *gate_bitmap;
    int gate_iter = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.taf_gates_to_reset.get(unit, &gate_bitmap));

    SHR_BIT_ITER(gate_bitmap, dnx_data_tsn.taf.nof_gates_get(unit), gate_iter)
    {

        SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_reset(unit, gate_iter));

        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.taf_gates_to_reset.bit_clear(unit, gate_iter));

        /* *INDENT-OFF* */
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, " TAF gate reset = %d\n"), gate_iter));
        
    }

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tas_ports_to_reset.get(unit, &gate_bitmap));

    SHR_BIT_ITER(gate_bitmap, dnx_data_tsn.tas.nof_tas_ports_get(unit), gate_iter)
    {
        
        SHR_IF_ERR_EXIT(dnx_tsn_tas_port_reset(unit, DNX_TSN_THREAD_TAS_CORE_ID, gate_iter));

        
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tas_ports_to_reset.bit_clear(unit, gate_iter));

        /* *INDENT-OFF* */
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, " TAS port reset = %d\n"), gate_iter));
        
    }

exit:
    if (is_mutex_locked)
    {
        
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_tsn_thread_config_change_time_set(
    int unit,
    dnx_tsn_gate_type_e type,
    int pid,
    bcm_ptp_timestamp_t * ptp_base_time)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_TSN_GATE_TYPE_TAF:
            SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.config_change_time.set(unit, pid, *ptp_base_time));
            break;
        case DNX_TSN_GATE_TYPE_TAS:
            SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.
                            config_change_time.set(unit, DNX_TSN_THREAD_TAS_CORE_ID, pid, *ptp_base_time));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "gate type not supported %d\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_tsn_thread_config_change_time_get(
    int unit,
    dnx_tsn_gate_type_e type,
    int pid,
    bcm_ptp_timestamp_t * ptp_base_time)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_TSN_GATE_TYPE_TAF:
            SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.config_change_time.get(unit, pid, ptp_base_time));
            break;
        case DNX_TSN_GATE_TYPE_TAS:
            SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.
                            config_change_time.get(unit, DNX_TSN_THREAD_TAS_CORE_ID, pid, ptp_base_time));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "gate type not supported %d\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_tsn_thread_config_change_time_update(
    int unit,
    dnx_tsn_gate_type_e type,
    int pid)
{
    const bcm_cosq_tas_profile_t *profile_tas_ptr;
    const bcm_tsn_taf_profile_t *profile_taf_ptr;
    uint64 cycle_time_nsec;
    uint32 time_margin;
    bcm_ptp_timestamp_t config_change_time_prev, config_change_time;
    dnx_tsn_thread_pending_pid_t pending_pid;
    uint64 config_change_time_nsec;

    SHR_FUNC_INIT_VARS(unit);

    
    pending_pid.type = type;
    pending_pid.pid = pid;

    switch (type)
    {
        case DNX_TSN_GATE_TYPE_TAS:
            SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.
                            profile_data.get(unit, DNX_TSN_THREAD_TAS_CORE_ID, pid, &profile_tas_ptr));
            COMPILER_64_SET(cycle_time_nsec, profile_tas_ptr->ptp_cycle_time_upper, profile_tas_ptr->ptp_cycle_time);
            break;
        case DNX_TSN_GATE_TYPE_TAF:
            SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.get(unit, pid, &profile_taf_ptr));
            COMPILER_64_SET(cycle_time_nsec, profile_taf_ptr->ptp_cycle_time_upper, profile_taf_ptr->ptp_cycle_time);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "gate type not supported %d\n", type);
    }

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_config_change_time_get(unit, type, pid, &config_change_time_prev));

    
    time_margin =
        dnx_data_tsn.tsn_thread.wakeup_interval_nsec_get(unit) +
        dnx_data_tsn.tsn_thread.group_window_length_nsec_get(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_counter_config_change_time_get
                    (unit, &config_change_time_prev, cycle_time_nsec, time_margin, &config_change_time));

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_config_change_time_set(unit, type, pid, &config_change_time));

    
    SHR_IF_ERR_EXIT(utilex_ptp_time_to_nanoseconds_convert(unit, &config_change_time, &config_change_time_nsec));

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.add(unit, &config_change_time_nsec, &pending_pid));

    /* *INDENT-OFF* */
    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, " Update Base Time for : Type %u, Pid %u [Base time =  " COMPILER_64_PRINTF_FORMAT "]\n"),
                               type, pid, COMPILER_64_PRINTF_VALUE(config_change_time_nsec)));
    

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_tsn_thread_base_time_reschedule(
    int unit)
{
    int is_mutex_locked = FALSE;
    sw_state_ll_node_t node, next_node;
    uint64 curr_time_nsec, iter_time_nsec;
    dnx_tsn_thread_pending_pid_t pending_pid;
    dnx_tsn_thread_pid_bmp_t pid_bmp;
    dnx_tsn_gate_type_e type;
    int iter, pid;

    SHR_FUNC_INIT_VARS(unit);

    
    DNX_TSN_THREAD_PID_BMP_INIT(&pid_bmp);

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    
    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_get(unit, &curr_time_nsec));
    COMPILER_64_ADD_32(curr_time_nsec, dnx_data_tsn.tsn_thread.max_process_time_nsec_get(unit));

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.get_first(unit, &node));

    
    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.next_node(unit, node, &next_node));

        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.node_key(unit, node, (uint64 *) &iter_time_nsec));

        if (COMPILER_64_GE(iter_time_nsec, curr_time_nsec))
        {
            break;
        }

        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.
                        pending_list.node_value(unit, node, (dnx_tsn_thread_pending_pid_t *) & pending_pid));

        DNX_TSN_THREAD_PID_BMP_BITSET(&pid_bmp, pending_pid.type, pending_pid.pid);

        
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.remove_node(unit, node));

        node = next_node;
    }

    
    DNX_TSN_THREAD_PID_BMP_BIT_ITER(&pid_bmp, iter)
    {
        type = DNX_TSN_THREAD_PID_BMP_ITER_TYPE_GET(iter);
        pid = DNX_TSN_THREAD_PID_BMP_ITER_PID_GET(iter);

        
        SHR_IF_ERR_EXIT(dnx_tsn_thread_config_change_time_update(unit, type, pid));
    }

exit:
    if (is_mutex_locked)
    {
       
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_tsn_thread_pid_group_get(
    int unit,
    dnx_tsn_thread_pid_bmp_t * group_bmp)
{
    int is_mutex_locked = FALSE;
    sw_state_ll_node_t node;
    uint64 curr_time_nsec, iter_time_nsec, max_process_time_nsec, wakeup_time_nsec, group_window_nsec;
    dnx_tsn_thread_pending_pid_t pending_pid;

    SHR_FUNC_INIT_VARS(unit);

    
    DNX_TSN_THREAD_PID_BMP_INIT(group_bmp);

    
    COMPILER_64_ZERO(group_window_nsec);

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    
    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_get(unit, &curr_time_nsec));

    
    COMPILER_64_COPY(max_process_time_nsec, curr_time_nsec);
    COMPILER_64_ADD_32(max_process_time_nsec, dnx_data_tsn.tsn_thread.max_process_time_nsec_get(unit));

    
    COMPILER_64_COPY(wakeup_time_nsec, curr_time_nsec);
    COMPILER_64_ADD_32(wakeup_time_nsec, dnx_data_tsn.tsn_thread.wakeup_interval_nsec_get(unit));

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.get_first(unit, &node));

    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.node_key(unit, node, (uint64 *) &iter_time_nsec));

        if (COMPILER_64_LT(iter_time_nsec, max_process_time_nsec))
        {
            
            SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.next_node(unit, node, &node));
            continue;
        }

        
        if (DNX_TSN_THREAD_PID_BMP_IS_EMPTY(group_bmp))
        {
            if (COMPILER_64_LE(iter_time_nsec, wakeup_time_nsec))
            {
                
                COMPILER_64_COPY(group_window_nsec, iter_time_nsec);
                COMPILER_64_ADD_32(group_window_nsec, dnx_data_tsn.tsn_thread.group_window_length_nsec_get(unit));

                /* *INDENT-OFF* */
                LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, " New Group [Current Time = " COMPILER_64_PRINTF_FORMAT ", First time = " COMPILER_64_PRINTF_FORMAT "]\n"),
                                           COMPILER_64_PRINTF_VALUE(curr_time_nsec), COMPILER_64_PRINTF_VALUE(iter_time_nsec)));
                
            }
            else
            {
                
                break;
            }
        }

        
        if (COMPILER_64_LE(iter_time_nsec, group_window_nsec))
        {
            SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.
                            pending_list.node_value(unit, node, (dnx_tsn_thread_pending_pid_t *) & pending_pid));

            DNX_TSN_THREAD_PID_BMP_BITSET(group_bmp, pending_pid.type, pending_pid.pid);

            /* *INDENT-OFF* */
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit, " Add to Group - Type %u, Pid %u\n"), pending_pid.type, pending_pid.pid));
            
        }
        else
        {
            
            break;
        }

        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.next_node(unit, node, &node));
    }

exit:
    if (is_mutex_locked)
    {
        
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_tsn_thread_pid_group_remove(
    int unit,
    dnx_tsn_thread_pid_bmp_t * group_bmp)
{
    int is_mutex_locked = FALSE;
    sw_state_ll_node_t node, next_node;
    dnx_tsn_thread_pending_pid_t pending_pid;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.get_first(unit, &node));

    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.next_node(unit, node, &next_node));

        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.
                        pending_list.node_value(unit, node, (dnx_tsn_thread_pending_pid_t *) & pending_pid));

        if (DNX_TSN_THREAD_PID_BMP_IS_BITSET(group_bmp, pending_pid.type, pending_pid.pid))
        {
            
            SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.remove_node(unit, node));
        }

        node = next_node;
    }

exit:
    if (is_mutex_locked)
    {
        
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_tsn_thread_tas_profile_hw_update(
    int unit,
    dnx_tsn_thread_pid_bmp_t * group_bmp)
{
    dnx_tsn_tas_pid_bmp_t requested_tas_bmp;
    dnx_tsn_tas_pid_bmp_t updated_tas_bmp;
    dnx_tsn_thread_pid_bmp_t updated_pid_bmp;
    int pid_i;

    SHR_FUNC_INIT_VARS(unit);

    
    DNX_TSN_THREAD_PID_BMP_TAS_BMP_CONVERT(group_bmp, &requested_tas_bmp);

    if (!DNX_TSN_TAS_PID_BMP_IS_EMPTY(&requested_tas_bmp))
    {
        
        SHR_IF_ERR_EXIT(dnx_tsn_tas_profile_multi_hw_update
                        (unit, DNX_TSN_THREAD_TAS_CORE_ID, &requested_tas_bmp, &updated_tas_bmp));

        
        DNX_TSN_THREAD_PID_BMP_INIT(&updated_pid_bmp);
        DNX_TSN_THREAD_TAS_BMP_PID_BMP_CONVERT(&updated_tas_bmp, &updated_pid_bmp);

        
        SHR_IF_ERR_EXIT(dnx_tsn_thread_pid_group_remove(unit, &updated_pid_bmp));

        
        DNX_TSN_TAS_PID_BMP_BIT_ITER(&updated_tas_bmp, pid_i)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_tas_profile_state_active_set(unit, DNX_TSN_THREAD_TAS_CORE_ID, pid_i));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_tsn_thread_taf_profile_hw_update(
    int unit,
    dnx_tsn_thread_pid_bmp_t * group_bmp)
{
    dnx_tsn_taf_pid_bmp_t requested_taf_bmp;
    dnx_tsn_taf_pid_bmp_t updated_taf_bmp;
    dnx_tsn_thread_pid_bmp_t updated_pid_bmp;
    int pid_i;

    SHR_FUNC_INIT_VARS(unit);

    
    DNX_TSN_THREAD_PID_BMP_TAF_BMP_CONVERT(group_bmp, &requested_taf_bmp);

    if (!DNX_TSN_TAF_PID_BMP_IS_EMPTY(&requested_taf_bmp))
    {
        
        SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_multi_hw_update(unit, &requested_taf_bmp, &updated_taf_bmp));

        
        DNX_TSN_THREAD_PID_BMP_INIT(&updated_pid_bmp);
        DNX_TSN_THREAD_TAF_BMP_PID_BMP_CONVERT(&updated_taf_bmp, &updated_pid_bmp);

        
        SHR_IF_ERR_EXIT(dnx_tsn_thread_pid_group_remove(unit, &updated_pid_bmp));

        
        DNX_TSN_TAF_PID_BMP_BIT_ITER(&updated_taf_bmp, pid_i)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_state_active_set(unit, pid_i));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_tsn_thread_profile_hw_update(
    int unit,
    dnx_tsn_thread_pid_bmp_t * group_bmp)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_thread_tas_profile_hw_update(unit, group_bmp));
    SHR_IF_ERR_EXIT(dnx_tsn_thread_taf_profile_hw_update(unit, group_bmp));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_tsn_thread_interval_update(
    int unit)
{
    sw_state_ll_node_t node;
    int is_mutex_locked = FALSE;
    uint64 diff, first_time_nsec, curr_time_nsec;
    shr_thread_manager_handler_t tsn_thread_handler;
    int interval = sal_sem_FOREVER;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.get_first(unit, &node));

    if (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.node_key(unit, node, (uint64 *) &first_time_nsec));

        
        SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_get(unit, &curr_time_nsec));

        
        COMPILER_64_ADD_32(curr_time_nsec,
                           dnx_data_tsn.tsn_thread.wakeup_interval_nsec_get(unit) -
                           (dnx_data_tsn.tsn_thread.wakeup_interval_nsec_get(unit) / 100));

        if (COMPILER_64_LE(first_time_nsec, curr_time_nsec))
        {
            
            SHR_IF_ERR_EXIT(dnx_tsn_thread_trigger(unit));
        }
        else
        {
           
            COMPILER_64_COPY(diff, first_time_nsec);
            COMPILER_64_SUB_64(diff, curr_time_nsec);

            
            COMPILER_64_UDIV_32(diff, 1000);

            if ((COMPILER_64_HI(diff) != 0) || (COMPILER_64_LO(diff) >= DNX_TSN_THREAD_INTERVAL_MAX_USEC))
            {
                
                interval = DNX_TSN_THREAD_INTERVAL_MAX_USEC;
            }
            else
            {
                interval = COMPILER_64_LO(diff);
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tsn_thread_handler.get(unit, &tsn_thread_handler));
    SHR_IF_ERR_EXIT(shr_thread_manager_interval_set(tsn_thread_handler, 1, interval));

    if (interval != sal_sem_FOREVER)
    {
        /* *INDENT-OFF* */
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, " Interval = %d (usec) [Current Time = " COMPILER_64_PRINTF_FORMAT ", First time = " COMPILER_64_PRINTF_FORMAT "] \n"),
                                   interval, COMPILER_64_PRINTF_VALUE(curr_time_nsec), COMPILER_64_PRINTF_VALUE(first_time_nsec)));
        
    }
    else
    {
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, " Interval = FOREVER (Empty List)\n")));
    }

exit:
    if (is_mutex_locked)
    {
        
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_tsn_thread_main(
    int unit,
    void *user_data)
{
    dnx_tsn_thread_pid_bmp_t group_bmp;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_tas_tod_update_handle(unit));

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_gates_reset(unit));

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_base_time_reschedule(unit));

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_pid_group_get(unit, &group_bmp));

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_profile_hw_update(unit, &group_bmp));

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_interval_update(unit));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_tsn_thread_pending_list_add(
    int unit,
    bcm_ptp_timestamp_t * ptp_base_time,
    dnx_tsn_gate_type_e type,
    int pid)
{
    int is_mutex_locked = FALSE;
    dnx_tsn_thread_pending_pid_t pending_pid;
    uint64 base_time_nsec;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(utilex_ptp_time_to_nanoseconds_convert(unit, ptp_base_time, &base_time_nsec));

    
    pending_pid.type = type;
    pending_pid.pid = pid;

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.add(unit, &base_time_nsec, &pending_pid));

exit:
    if (is_mutex_locked)
    {
        
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_tsn_thread_pending_list_remove_all(
    int unit,
    dnx_tsn_gate_type_e type,
    int gate_id)
{
    int is_mutex_locked = FALSE;
    dnx_tsn_thread_pending_pid_t pending_pid;
    int pid_gate_id;
    sw_state_ll_node_t node, next_node;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.get_first(unit, &node));

    while (DNX_SW_STATE_LL_IS_NODE_VALID(node))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.next_node(unit, node, &next_node));

        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.
                        pending_list.node_value(unit, node, (dnx_tsn_thread_pending_pid_t *) & pending_pid));

        pid_gate_id = pending_pid.pid / dnx_data_tsn.taf.max_profiles_per_gate_get(unit);

        if (pending_pid.type == type && pid_gate_id == gate_id)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.pending_list.remove_node(unit, node));
            break;
        }

        node = next_node;
    }

exit:
    if (is_mutex_locked)
    {
        
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_tsn_thread_gate_reset_set(
    int unit,
    dnx_tsn_gate_type_e type,
    int gate_id)
{
    int is_mutex_locked = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    if (type == DNX_TSN_GATE_TYPE_TAF)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.taf_gates_to_reset.bit_set(unit, gate_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tsn_thread.tas_ports_to_reset.bit_set(unit, gate_id));
    }

exit:
    if (is_mutex_locked)
    {
        
        SHR_IF_ERR_CONT(dnx_tsn_db.tsn_thread.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_tsn_thread_profile_commit(
    int unit,
    bcm_ptp_timestamp_t * ptp_base_time,
    dnx_tsn_gate_type_e type,
    int pid)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_config_change_time_set(unit, type, pid, ptp_base_time));

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_pending_list_add(unit, ptp_base_time, type, pid));

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_trigger(unit));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_tsn_thread_control_reset(
    int unit,
    dnx_tsn_gate_type_e type,
    int gate_id)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_pending_list_remove_all(unit, type, gate_id));

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_gate_reset_set(unit, type, gate_id));

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_trigger(unit));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_tsn_thread_tas_tod_update_interrupt_handler(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_tsn_thread_trigger(unit));

exit:
    SHR_FUNC_EXIT;
}
