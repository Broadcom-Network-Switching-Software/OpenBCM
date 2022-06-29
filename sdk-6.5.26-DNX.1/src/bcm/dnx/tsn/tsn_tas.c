/*
 * tsn_tas.c
 *
 *  Created on: Apr 12, 2022
 *      Author: nt893888
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */

#include <bcm/port.h>
#include <bcm/cosq.h>
#include <bcm/types.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_tsn_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_tsn_types.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>
#include <bcm_int/dnx/tsn/tsn_tas.h>

#include "tsn_tas.h"
#include "tsn_dbal.h"
#include "tsn_common.h"
#include "tsn_counter.h"
#include "tsn_thread.h"

#define DNX_TSN_TAS_GATE_ALLOC "TSN TAS GATE ALLOC"
#define DNX_TSN_TAS_PID_ALLOC "TSN TAS PID ALLOC"
#define TAS_PORT_UNUSED (-1)

/**
 * \brief - Init ports allocation managers
 *
 * \param [in] unit -HW identifier of unit.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_alloc_mng_init(
    int unit)
{
    dnx_algo_res_create_data_t gate_alloc_create_data;
    dnx_algo_res_create_data_t pid_alloc_create_data;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Allocate gate allocation entities **/
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.gate_alloc.alloc(unit));

    sal_memset(&gate_alloc_create_data, 0, sizeof(gate_alloc_create_data));
    gate_alloc_create_data.first_element = 0;
    gate_alloc_create_data.nof_elements = dnx_data_tsn.tas.nof_tas_ports_get(unit);
    sal_strncpy(gate_alloc_create_data.name, DNX_TSN_TAS_GATE_ALLOC, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /** Allocate pid allocation entities **/
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.pid_alloc.alloc(unit));

    sal_memset(&pid_alloc_create_data, 0, sizeof(pid_alloc_create_data));
    pid_alloc_create_data.first_element = 0;
    pid_alloc_create_data.nof_elements =
        dnx_data_tsn.tas.nof_tas_ports_get(unit) * dnx_data_tsn.tas.max_profiles_per_port_get(unit);
    sal_strncpy(pid_alloc_create_data.name, DNX_TSN_TAS_PID_ALLOC, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tas.gate_alloc.create(unit, core_id, &gate_alloc_create_data, NULL));
        SHR_IF_ERR_EXIT(dnx_tsn_db.tas.pid_alloc.create(unit, core_id, &pid_alloc_create_data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init tas sw
 *
 * \param [in] unit -HW identifier of unit.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_sw_init(
    int unit)
{
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Init allocation managers */
    SHR_IF_ERR_EXIT(dnx_tsn_tas_alloc_mng_init(unit));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.egq_if_to_gate_mapping.alloc(unit));

    /** set all mapping id to UNUSED state*/
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        for (int egq_if = 0; egq_if < dnx_data_tsn.tas.nof_tas_ports_get(unit); egq_if++)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_db.tas.egq_if_to_gate_mapping.set(unit, core_id, egq_if, TAS_PORT_UNUSED));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify profile destroy function.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] gate_id -gate id vaule.
 * \param [out] pid_min - pid minimum vale.
 * \param [out] pid_max - pid maximum vale.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_pid_range_get(
    int unit,
    int gate_id,
    int *pid_min,
    int *pid_max)
{
    SHR_FUNC_INIT_VARS(unit);

    *pid_min = gate_id * dnx_data_tsn.tas.max_profiles_per_port_get(unit);
    *pid_max = *pid_min + dnx_data_tsn.tas.max_profiles_per_port_get(unit);

    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify tas profile.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port - logical port.
 * \param [in] profile - TAS profile.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_profile_verify(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_t * profile)
{
    uint64 cycle_time;
    uint64 cycle_time_nanosec, cycle_time_ticks;
    uint64 ticks_sum;
    uint32 reminder;
    uint32 allowed_flags = 0;
    uint32 state_mask = 0;
    uint64 mask = 0;
    int nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_SET(cycle_time, profile->ptp_cycle_time_upper, profile->ptp_cycle_time);
    COMPILER_64_UDIV_32_WITH_REMAINDER(cycle_time, dnx_data_tsn.time.tick_period_ns_get(unit), reminder);
    if (reminder != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN TAS profile cycle time needs to be a microseconds multiplier");
    }
    else if (profile->num_entries > BCM_COSQ_MAX_TAS_CALENDAR_TABLE_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN TAS profile num_entries too big");
    }
    else if ((profile->ptp_base_time.nanoseconds % dnx_data_tsn.time.tick_period_ns_get(unit)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN TAS profile base time needs to be a microseconds multiplier");
    }
    else if (profile->entries[profile->num_entries - 1].ticks != BCM_COSQ_TAS_ENTRY_TICKS_STAY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN TAS last entry must be BCM_COSQ_TAS_ENTRY_TICKS_STAY");
    }

    COMPILER_64_ZERO(ticks_sum);
    allowed_flags |= BCM_COSQ_TAS_ENTRY_F_PREEMPT;

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));
    SHR_BITSET_RANGE(&state_mask, nof_priorities, sizeof(state_mask) - nof_priorities);

    for (int entry = 0; entry < profile->num_entries; entry++)
    {
        if ((profile->entries[entry].state & state_mask) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Entry state: %d is not valid", entry);
        }
        if (entry < profile->num_entries - 1)
        {
            COMPILER_64_ADD_32(ticks_sum, profile->entries[entry].ticks);
        }
        SHR_MASK_VERIFY(profile->entries[entry].flags, allowed_flags, _SHR_E_PARAM, "Unsupported flag provided.\n");
    }

    COMPILER_64_SET(cycle_time_nanosec, profile->ptp_cycle_time_upper, profile->ptp_cycle_time);
    SHR_IF_ERR_EXIT(dnx_tsn_counter_nanoseconds_to_ticks_convert(unit, cycle_time_nanosec, &cycle_time_ticks));

    if (COMPILER_64_GE(ticks_sum, cycle_time_ticks))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cycle time smaller then ticks sum");
    }

    COMPILER_64_SUB_64(cycle_time_ticks, ticks_sum);
    COMPILER_64_MASK_CREATE(mask, dnx_data_tsn.time.tsn_counter_nof_bits_get(unit), 0);
    COMPILER_64_NOT(mask);
    COMPILER_64_AND(cycle_time_ticks, mask);

    if (cycle_time_ticks != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cycle time too high");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify logical port.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_port_verify(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** verify logical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!(DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TAN TAS support only NIF ports, port - %d", port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify create function.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] profile - TAS profile.
 * \param [in] pid - profile id.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_create_verify(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_t * profile,
    bcm_cosq_tas_profile_id_t * pid)
{
    int is_channelized;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pid, _SHR_E_PARAM, "pid");
    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");

    SHR_IF_ERR_EXIT(dnx_tsn_tas_profile_verify(unit, port, profile));
    SHR_IF_ERR_EXIT(dnx_tsn_tas_port_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &is_channelized));
    if (is_channelized)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN TAS supported only not channelized ports, port- %d", port);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify pid matched to port.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] pid - profile id.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_pid_verify(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid)
{
    int core_id;
    uint8 is_allocated;
    bcm_port_t pid_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.pid_alloc.is_allocated(unit, core_id, pid, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "pid not in used");
    }
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.port.get(unit, core_id, pid, &pid_port));
    if (pid_port != port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TAS PID and port are not matched");
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Verify status get function.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] pid - profile id.
 * \param [in] status - profile status.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_status_get_verify(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_status_t * status)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(status, _SHR_E_PARAM, "status");
    SHR_IF_ERR_EXIT(dnx_tsn_tas_port_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_tsn_tas_pid_verify(unit, port, pid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify profile get function.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] pid - profile id.
 * \param [in] profile - tas profile.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_profile_get_verify(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t * profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");
    SHR_IF_ERR_EXIT(dnx_tsn_tas_port_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_tsn_tas_pid_verify(unit, port, pid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify profile set function.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] pid - profile id.
 * \param [in] profile - tas profile.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_profile_set_verify(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t * profile)
{
    bcm_core_t core_id;
    bcm_cosq_tas_profile_state_t profile_state;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");
    SHR_IF_ERR_EXIT(dnx_tsn_tas_port_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_tsn_tas_pid_verify(unit, port, pid));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_tsn_tas_profile_verify(unit, port, profile));

    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_state.get(unit, core_id, pid, &profile_state));
    if (profile_state != bcmCosqTASProfileInit)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "profile set can be done only for Init profiles, port- %d", port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify TAS port reset function.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] arg - expire all arguments
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_port_reset_verify(
    int unit,
    bcm_port_t port,
    int arg)
{
    bcm_core_t core_id;
    int gate_id;
    int egr_tsn_if;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_tas_port_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_tsn_common_egr_tsn_interface_get(unit, port, &egr_tsn_if));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.egq_if_to_gate_mapping.get(unit, core_id, egr_tsn_if, &gate_id));
    if (gate_id == TAS_PORT_UNUSED)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Port has no TAS profile");
    }
    else if (arg != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Arg should be 1");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify profile commit function.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] pid - profile id.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_profile_commit_verify(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid)
{
    bcm_core_t core_id;
    bcm_cosq_tas_profile_state_t profile_state;
    int gate_id;
    int egr_tsn_if;
    int pid_min, pid_max;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_tas_port_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_state.get(unit, core_id, pid, &profile_state));
    if (profile_state != bcmCosqTASProfileInit)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Commit profile can be done only for Init profiles, pid- %d", pid);
    }
    SHR_IF_ERR_EXIT(dnx_tsn_tas_pid_verify(unit, port, pid));
    SHR_IF_ERR_EXIT(dnx_tsn_common_egr_tsn_interface_get(unit, port, &egr_tsn_if));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.egq_if_to_gate_mapping.get(unit, core_id, egr_tsn_if, &gate_id));
    SHR_IF_ERR_EXIT(dnx_tsn_tas_pid_range_get(unit, gate_id, &pid_min, &pid_max));
    /*
     * only 1 port profile can be pending 
     */
    for (int pid_i = pid_min; pid_i < pid_max; pid_i++)
    {
        uint8 is_allocated;
        SHR_IF_ERR_EXIT(dnx_tsn_db.tas.pid_alloc.is_allocated(unit, core_id, pid_i, &is_allocated));
        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_state.get(unit, core_id, pid_i, &profile_state));
            if (profile_state == bcmCosqTASProfilePending)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Only one port profile can be commited at the same time");

            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify profile destroy function.
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] pid - profile id.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_profile_destroy_verify(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid)
{
    bcm_core_t core_id;
    bcm_cosq_tas_profile_state_t profile_state;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_tas_port_verify(unit, port));
    SHR_IF_ERR_EXIT(dnx_tsn_tas_pid_verify(unit, port, pid));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));

    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_state.get(unit, core_id, pid, &profile_state));

    if (profile_state != bcmCosqTASProfileInit)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Destroy profile can be done only for Init profiles, pid- %d", pid);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Alloc PID for new profile
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] core_id -core id.
 * \param [in] egr_tsn_if - egr tsn interface.
 * \param [out] pid - profile id.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_pid_alloc(
    int unit,
    bcm_core_t core_id,
    int egr_tsn_if,
    bcm_cosq_tas_profile_id_t * pid)
{
    int gate_id;
    int alloc_flags = 0;
    int pid_min, pid_max;
    resource_tag_bitmap_utils_extra_arg_alloc_info_t alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    /** get gate value*/
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.egq_if_to_gate_mapping.get(unit, core_id, egr_tsn_if, &gate_id));
    if (gate_id == TAS_PORT_UNUSED)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tas.gate_alloc.allocate_single(unit, core_id, 0, NULL, &gate_id));
        SHR_IF_ERR_EXIT(dnx_tsn_db.tas.egq_if_to_gate_mapping.set(unit, core_id, egr_tsn_if, gate_id));
    }

    /** get free pid from range*/
    SHR_IF_ERR_EXIT(dnx_tsn_tas_pid_range_get(unit, gate_id, &pid_min, &pid_max));
    sal_memset(&alloc_info, 0x0, sizeof(resource_tag_bitmap_utils_extra_arg_alloc_info_t));
    alloc_flags |= RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
    alloc_info.range_start = pid_min;
    alloc_info.range_end = pid_max;

    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.pid_alloc.allocate_single(unit, core_id, alloc_flags, (void *) &alloc_info, pid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get number of profile of specific port
 *
 * \param [in] unit -HW identifier of unit.
 * \param [in] port -logical port.
 * \param [in] egr_tsn_if - egr tsn interface value.
 * \param [in] nof_profiles - number of port's profiles.
 * \return
 *   shr_error_e
 * \remark
 */
static shr_error_e
dnx_tsn_tas_nof_port_profile_get(
    int unit,
    bcm_port_t port,
    int egr_tsn_if,
    int *nof_profiles)
{
    bcm_core_t core_id;

    int gate_id;
    int pid_min, pid_max;
    int nof_active_profiles = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** get gate value*/
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.egq_if_to_gate_mapping.get(unit, core_id, egr_tsn_if, &gate_id));
    SHR_IF_ERR_EXIT(dnx_tsn_tas_pid_range_get(unit, gate_id, &pid_min, &pid_max));

    for (int pid_i = pid_min; pid_i < pid_max; pid_i++)
    {
        uint8 is_allocated;
        SHR_IF_ERR_EXIT(dnx_tsn_db.tas.pid_alloc.is_allocated(unit, core_id, pid_i, &is_allocated));
        if (is_allocated)
        {
            nof_active_profiles++;
        }
    }

    *nof_profiles = nof_active_profiles;

exit:
    SHR_FUNC_EXIT;
}

/** see header file **/
shr_error_e
dnx_tsn_tas_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /**Init TAS hw*/
    SHR_IF_ERR_EXIT(dnx_tsn_tas_hw_init(unit));
    /**Init TAS SW*/
    SHR_IF_ERR_EXIT(dnx_tsn_tas_sw_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/** see header file **/
shr_error_e
dnx_tsn_tas_profile_create(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_t * profile,
    bcm_cosq_tas_profile_id_t * pid)
{
    int egr_tsn_if;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_tas_create_verify(unit, port, profile, pid));
    /** get egq interface */
    SHR_IF_ERR_EXIT(dnx_tsn_common_egr_tsn_interface_get(unit, port, &egr_tsn_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    /** get PID */
    SHR_IF_ERR_EXIT(dnx_tsn_tas_pid_alloc(unit, core_id, egr_tsn_if, pid));

    /** set profile data to sw */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_state.set(unit, core_id, *pid, bcmCosqTASProfileInit));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.port.set(unit, core_id, *pid, port));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_data.set(unit, core_id, *pid, profile));

exit:
    SHR_FUNC_EXIT;
}

/** see header file **/
shr_error_e
dnx_tsn_tas_status_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_status_t * status)
{
    int core_id;
    const bcm_cosq_tas_profile_t *profile_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_tas_status_get_verify(unit, port, pid, status));

    bcm_cosq_tas_profile_status_t_init(status);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_state.get(unit, core_id, pid, &status->profile_state));

    if (status->profile_state == bcmCosqTASProfileActive)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.
                        config_change_time.get(unit, core_id, pid, &status->config_change_time));
    }

    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_data.get(unit, core_id, pid, &profile_data));

    status->num_entries = profile_data->num_entries;

    sal_memcpy(&(status->entries), &(profile_data->entries),
               sizeof(bcm_cosq_tas_entry_t) * BCM_COSQ_MAX_TAS_CALENDAR_TABLE_SIZE);

exit:
    SHR_FUNC_EXIT;
}

/** see header file **/
shr_error_e
dnx_tsn_tas_profile_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t * profile)
{
    bcm_core_t core_id;
    const bcm_cosq_tas_profile_t *profile_ptr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_tas_profile_get_verify(unit, port, pid, profile));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_data.get(unit, core_id, pid, &profile_ptr));
    sal_memcpy(profile, profile_ptr, sizeof(bcm_cosq_tas_profile_t));

exit:
    SHR_FUNC_EXIT;
}

/** see header file **/
shr_error_e
dnx_tsn_tas_profile_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_cosq_tas_profile_t * profile)
{
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_tas_profile_set_verify(unit, port, pid, profile));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_data.set(unit, core_id, pid, profile));

exit:
    SHR_FUNC_EXIT;
}

/** see header file **/
shr_error_e
dnx_tsn_tas_profile_commit(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid)
{
    const bcm_cosq_tas_profile_t *profile_ptr;
    bcm_ptp_timestamp_t ptp_base_time;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_tas_profile_commit_verify(unit, port, pid));

    /** change state to pending */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_state.set(unit, core_id, pid, bcmCosqTASProfilePending));

    /** get profile info */
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_data.get(unit, core_id, pid, &profile_ptr));
    sal_memcpy(&ptp_base_time, &profile_ptr->ptp_base_time, sizeof(bcm_ptp_timestamp_t));

    if (dnx_data_tsn.general.feature_get(unit, dnx_data_tsn_general_tsn_thread))
    {
        /** commit to thread profile manager */
        SHR_IF_ERR_EXIT(dnx_tsn_thread_profile_commit(unit, &ptp_base_time, DNX_TSN_GATE_TYPE_TAS, pid));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Profile commit not supported on this device\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file **/
shr_error_e
dnx_tsn_tas_profile_destroy(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid)
{
    bcm_core_t core_id;
    int nof_profiles = 0;
    int egr_tsn_if;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_tas_profile_destroy_verify(unit, port, pid));

    /** free pid */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.pid_alloc.free_single(unit, core_id, pid, NULL));

    /** if last profile of the port, free gate */
    SHR_IF_ERR_EXIT(dnx_tsn_common_egr_tsn_interface_get(unit, port, &egr_tsn_if));
    SHR_IF_ERR_EXIT(dnx_tsn_tas_nof_port_profile_get(unit, port, egr_tsn_if, &nof_profiles));
    if (nof_profiles == 0)
    {
        int gate_id;
        SHR_IF_ERR_EXIT(dnx_tsn_db.tas.egq_if_to_gate_mapping.get(unit, core_id, egr_tsn_if, &gate_id));
        SHR_IF_ERR_EXIT(dnx_tsn_db.tas.gate_alloc.free_single(unit, core_id, gate_id, NULL));
        SHR_IF_ERR_EXIT(dnx_tsn_db.tas.egq_if_to_gate_mapping.set(unit, core_id, egr_tsn_if, TAS_PORT_UNUSED));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file **/
shr_error_e
dnx_tsn_tas_profile_hw_set(
    int unit,
    bcm_core_t core_id,
    int table_set,
    bcm_cosq_tas_profile_id_t pid)
{
    const bcm_cosq_tas_profile_t *profile_ptr;
    int egr_tsn_if;
    int gate_id;
    bcm_port_t port;
    int base_q;
    dbal_enum_value_field_egq_ps_mode_e ps_mode = 0;
    bcm_ptp_timestamp_t ptp_base_time;
    int global_nif_if;
    int local_nif_if;
    int esb_port_num = 0;
    uint64 cycle_time_nanosec, cycle_time_ticks;
    uint64 ticks_sum;
    uint64 base_time_in_tsn_counter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.profile_data.get(unit, core_id, pid, &profile_ptr));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.port.get(unit, core_id, pid, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q));
    SHR_IF_ERR_EXIT(dnx_tsn_common_port_to_ps_mode_get(unit, port, &ps_mode));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &global_nif_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_global_to_local_phy_get(unit, global_nif_if, &local_nif_if));
    esb_port_num = local_nif_if;
    SHR_IF_ERR_EXIT(dnx_tsn_common_egr_tsn_interface_get(unit, port, &egr_tsn_if));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.egq_if_to_gate_mapping.get(unit, core_id, egr_tsn_if, &gate_id));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.tas_profiles_data.config_change_time.get(unit, core_id, pid, &ptp_base_time));
    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_ptp_time_to_tsn_counter(unit, &ptp_base_time, &base_time_in_tsn_counter));

    SHR_IF_ERR_EXIT(dnx_tsn_tas_port_params_hw_set(unit, core_id, table_set, gate_id, profile_ptr->num_entries,
                                                   TRUE, base_time_in_tsn_counter, profile_ptr->entries[0].state,
                                                   profile_ptr->entries[0].flags & BCM_COSQ_TAS_ENTRY_F_PREEMPT, base_q,
                                                   ps_mode, esb_port_num));

    COMPILER_64_ZERO(ticks_sum);
    for (int entry = 0; entry < profile_ptr->num_entries - 1; entry++)
    {
        COMPILER_64_ADD_32(ticks_sum, profile_ptr->entries[entry].ticks);
        SHR_IF_ERR_EXIT(dnx_tsn_tas_port_control_list_hw_set(unit, core_id, table_set, gate_id, entry,
                                                             profile_ptr->entries[entry].ticks,
                                                             profile_ptr->entries[entry + 1].state,
                                                             profile_ptr->entries[entry +
                                                                                  1].flags &
                                                             BCM_COSQ_TAS_ENTRY_F_PREEMPT));
    }

    /*
     * set last entry - convert cycle time to ticks and subtract total time intervals ticks
     */
    COMPILER_64_SET(cycle_time_nanosec, profile_ptr->ptp_cycle_time_upper, profile_ptr->ptp_cycle_time);
    SHR_IF_ERR_EXIT(dnx_tsn_counter_nanoseconds_to_ticks_convert(unit, cycle_time_nanosec, &cycle_time_ticks));
    COMPILER_64_SUB_64(cycle_time_ticks, ticks_sum);

    SHR_IF_ERR_EXIT(dnx_tsn_tas_port_control_list_hw_set
                    (unit, core_id, table_set, gate_id, profile_ptr->num_entries - 1, cycle_time_ticks,
                     profile_ptr->entries[0].state, profile_ptr->entries[0].flags));

exit:
    SHR_FUNC_EXIT;
}

/** see header file **/
shr_error_e
dnx_tsn_tas_port_reset(
    int unit,
    bcm_port_t port,
    int arg)
{
    bcm_core_t core_id;
    int gate_id;
    int egr_tsn_if;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_tas_port_reset_verify(unit, port, arg));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core_id));
    SHR_IF_ERR_EXIT(dnx_tsn_common_egr_tsn_interface_get(unit, port, &egr_tsn_if));
    SHR_IF_ERR_EXIT(dnx_tsn_db.tas.egq_if_to_gate_mapping.get(unit, core_id, egr_tsn_if, &gate_id));

    /*
     * remove all port profiles from thread 
     */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_profile_reset_all(unit, gate_id, DNX_TSN_GATE_TYPE_TAS));

exit:
    SHR_FUNC_EXIT;
}
