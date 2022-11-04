
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

#include <bcm/tsn.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_tsn_access.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <sal/core/boot.h>
#include <shared/utilex/utilex_ptp_time.h>

#include "tsn_taf.h"
#include "tsn_dbal.h"
#include "tsn_counter.h"
#include "tsn_thread.h"

#define TSN_TAF_GATE_RSRC_MNGR_ID "TSN_TAF_GATE_ID"
#define TSN_TAF_PID_RSRC_MNGR_ID "TSN_TAF_PID"

#define DNX_TSN_TAF_INCATIVE_SET(set) (1 - set)

static shr_error_e
dnx_tsn_taf_res_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));

    data.first_element = 0;
    data.nof_elements = dnx_data_tsn.taf.nof_gates_get(unit);

    sal_strncpy(data.name, TSN_TAF_GATE_RSRC_MNGR_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.gate_alloc.create(unit, &data, NULL));

    data.first_element = 0;
    data.nof_elements = dnx_data_tsn.taf.nof_gates_get(unit) * dnx_data_tsn.taf.max_profiles_per_gate_get(unit);

    sal_strncpy(data.name, TSN_TAF_PID_RSRC_MNGR_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.pid_alloc.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_sw_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_res_mngr_init(unit));

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.alloc(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_hw_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_global_config_hw_init(unit));

    SHR_IF_ERR_EXIT(dnx_tsn_taf_reject_settings_hw_init(unit));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_hw_init(unit));

    SHR_IF_ERR_EXIT(dnx_tsn_taf_sw_init(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_profile_id_range_get(
    int unit,
    int taf_gate,
    int *pid_min,
    int *pid_max)
{
    SHR_FUNC_INIT_VARS(unit);

    *pid_min = taf_gate * dnx_data_tsn.taf.max_profiles_per_gate_get(unit);
    *pid_max = *pid_min + dnx_data_tsn.taf.max_profiles_per_gate_get(unit);

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_gate_from_profile_get(
    int unit,
    int pid,
    int *taf_gate)
{
    SHR_FUNC_INIT_VARS(unit);

    *taf_gate = pid / dnx_data_tsn.taf.max_profiles_per_gate_get(unit);

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_gate_id_verify(
    int unit,
    int taf_gate)
{
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(taf_gate, 0, dnx_data_tsn.taf.nof_gates_get(unit) - 1,
                     _SHR_E_PARAM, "Invalid TAF gate id %d", taf_gate);

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.gate_alloc.is_allocated(unit, taf_gate, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gate was not allocated %d\n", taf_gate);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_pid_verify(
    int unit,
    int taf_gate,
    int pid)
{
    uint8 is_allocated = 0;
    int pid_min, pid_max;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_id_range_get(unit, taf_gate, &pid_min, &pid_max));
    SHR_RANGE_VERIFY(pid, pid_min, pid_max,
                     _SHR_E_PARAM, "profile id %d is not associated with gate %d", pid, taf_gate);

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.pid_alloc.is_allocated(unit, pid, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PID was not allocated %d\n", pid);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_profile_is_busy(
    int unit,
    int pid,
    uint8 *is_busy)
{
    dnx_tsn_profile_state_e profile_state;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.get(unit, pid, &profile_state));
    if (profile_state != DNX_TSN_PROFILE_STATE_INIT)
    {
        *is_busy = TRUE;
    }
    else
    {
        *is_busy = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_gate_is_pending(
    int unit,
    int taf_gate,
    uint8 *is_pending)
{
    uint8 is_allocated = 0;
    int pid_i, pid_min, pid_max;
    dnx_tsn_profile_state_e profile_state;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_id_range_get(unit, taf_gate, &pid_min, &pid_max));

    *is_pending = FALSE;
    for (pid_i = pid_min; pid_i < pid_max; pid_i++)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.pid_alloc.is_allocated(unit, pid_i, &is_allocated));
        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.get(unit, pid_i, &profile_state));
            if (profile_state == DNX_TSN_PROFILE_STATE_PENDING)
            {
                *is_pending = TRUE;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_profile_verify(
    int unit,
    bcm_tsn_taf_profile_t * profile)
{
    uint64 cycle_time;
    uint64 cycle_time_ticks;
    uint64 ticks_sum;
    uint32 reminder;
    uint64 mask = 0;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_SET(cycle_time, profile->ptp_cycle_time_upper, profile->ptp_cycle_time);
    COMPILER_64_UDIV_32_WITH_REMAINDER(cycle_time, dnx_data_tsn.time.tick_period_ns_get(unit), reminder);
    if (reminder != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN TAF profile cycle time needs to be a microseconds multiplier");
    }

    SHR_RANGE_VERIFY(profile->num_entries, 1, DNX_DATA_MAX_TSN_TAF_MAX_TIME_INTERVALS_ENTRIES, _SHR_E_PARAM,
                     "Illegal number of intervals.\n");

    if ((profile->ptp_base_time.nanoseconds % dnx_data_tsn.time.tick_period_ns_get(unit)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN TAF profile base time needs to be a microseconds multiplier");
    }

    if (profile->entries[profile->num_entries - 1].ticks != BCM_TSN_TAF_ENTRY_TICKS_STAY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TSN TAF last entry must be BCM_TSN_TAF_ENTRY_TICKS_STAY");
    }

    COMPILER_64_ZERO(ticks_sum);
    for (int entry = 0; entry < profile->num_entries; entry++)
    {
        SHR_BOOL_VERIFY(profile->entries[entry].state, _SHR_E_PARAM, "Only values 0 and 1 are supported for state\r\n");

        if (entry < profile->num_entries - 1)
        {
            COMPILER_64_ADD_32(ticks_sum, profile->entries[entry].ticks);
        }
    }

    COMPILER_64_SET(cycle_time, profile->ptp_cycle_time_upper, profile->ptp_cycle_time);
    SHR_IF_ERR_EXIT(dnx_tsn_counter_nanoseconds_to_ticks_convert(unit, cycle_time, &cycle_time_ticks));

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

static shr_error_e
dnx_tsn_taf_gate_create_verify(
    int unit,
    int flags,
    int *taf_gate_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(taf_gate_id, _SHR_E_PARAM, "arg");
    SHR_MASK_VERIFY(flags, BCM_TSN_TAF_WITH_ID, _SHR_E_PARAM, "unexpected flags.\n");

    if (flags & BCM_TSN_TAF_WITH_ID)
    {
        SHR_RANGE_VERIFY(*taf_gate_id, 0, dnx_data_tsn.taf.nof_gates_get(unit) - 1,
                         _SHR_E_PARAM, "Invalid TAF gate id %d", *taf_gate_id);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_gate_create(
    int unit,
    int flags,
    int *taf_gate_id)
{
    int alloc_flags = 0;
    resource_tag_bitmap_utils_extra_arg_alloc_info_t alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_gate_create_verify(unit, flags, taf_gate_id));

    if (flags & BCM_TSN_TAF_WITH_ID)
    {
        alloc_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    sal_memset(&alloc_info, 0x0, sizeof(resource_tag_bitmap_utils_extra_arg_alloc_info_t));
    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.gate_alloc.allocate_single(unit, alloc_flags, (void *) &alloc_info, taf_gate_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_gate_destroy_verify(
    int unit,
    int taf_gate_id)
{
    int pid_i, pid_min, pid_max;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_id_verify(unit, taf_gate_id));

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_id_range_get(unit, taf_gate_id, &pid_min, &pid_max));
    for (pid_i = pid_min; pid_i < pid_max; pid_i++)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.pid_alloc.is_allocated(unit, pid_i, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "destroy profile %d before destroying gate\n", pid_i);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_gate_destroy(
    int unit,
    int taf_gate_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_gate_destroy_verify(unit, taf_gate_id));

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.gate_alloc.free_single(unit, taf_gate_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_profile_create_verify(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_t * profile,
    bcm_tsn_taf_profile_id_t * pid)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_id_verify(unit, taf_gate));

    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");
    SHR_NULL_CHECK(pid, _SHR_E_PARAM, "pid");

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_verify(unit, profile));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_create(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_t * profile,
    bcm_tsn_taf_profile_id_t * pid)
{
    int alloc_flags = 0;
    resource_tag_bitmap_utils_extra_arg_alloc_info_t alloc_info;
    int pid_min, pid_max;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_profile_create_verify(unit, taf_gate, profile, pid));

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_id_range_get(unit, taf_gate, &pid_min, &pid_max));

    sal_memset(&alloc_info, 0x0, sizeof(resource_tag_bitmap_utils_extra_arg_alloc_info_t));
    alloc_flags |= RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
    alloc_info.range_start = pid_min;
    alloc_info.range_end = pid_max;

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.pid_alloc.allocate_single(unit, alloc_flags, (void *) &alloc_info, pid));

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.set(unit, *pid, DNX_TSN_PROFILE_STATE_INIT));

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.set(unit, *pid, profile));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_profile_destroy_verify(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    uint8 is_busy = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_id_verify(unit, taf_gate));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_pid_verify(unit, taf_gate, pid));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_is_busy(unit, pid, &is_busy));
    if (is_busy)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "profile is busy, cannot destroy");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_destroy(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_profile_destroy_verify(unit, taf_gate, pid));

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.pid_alloc.free_single(unit, pid, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_set_verify(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    uint8 is_busy = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_id_verify(unit, taf_gate));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_pid_verify(unit, taf_gate, pid));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_verify(unit, profile));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_is_busy(unit, pid, &is_busy));
    if (is_busy)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "profile is busy, cannot set");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_profile_set_verify(unit, taf_gate, pid, profile));

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.set(unit, pid, profile));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_get_verify(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_id_verify(unit, taf_gate));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_pid_verify(unit, taf_gate, pid));

    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    const bcm_tsn_taf_profile_t *profile_ptr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_profile_set_verify(unit, taf_gate, pid, profile));

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.get(unit, pid, &profile_ptr));
    sal_memcpy(profile, profile_ptr, sizeof(bcm_tsn_taf_profile_t));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_commit_verify(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    uint8 is_pending = 0, is_busy = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_id_verify(unit, taf_gate));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_pid_verify(unit, taf_gate, pid));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_is_busy(unit, pid, &is_busy));
    if (is_busy)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cannot commit, profile is busy");
    }

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_is_pending(unit, taf_gate, &is_pending));
    if (is_pending)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "cannot commit, gate has pending request\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_commit(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    const bcm_tsn_taf_profile_t *profile_ptr;
    bcm_ptp_timestamp_t ptp_base_time;
    int is_mutex_locked = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_profile_commit_verify(unit, taf_gate, pid));

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.set(unit, pid, DNX_TSN_PROFILE_STATE_PENDING));

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.get(unit, pid, &profile_ptr));
    sal_memcpy(&ptp_base_time, &profile_ptr->ptp_base_time, sizeof(bcm_ptp_timestamp_t));

    if (dnx_data_tsn.general.feature_get(unit, dnx_data_tsn_general_tsn_thread))
    {

        SHR_IF_ERR_EXIT(dnx_tsn_thread_profile_commit(unit, &ptp_base_time, DNX_TSN_GATE_TYPE_TAF, pid));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Profile commit not supported on this device\n");
    }

exit:
    if (is_mutex_locked)
    {

        SHR_IF_ERR_CONT(dnx_tsn_db.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_status_get_verify(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_status_t * status)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_id_verify(unit, taf_gate));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_pid_verify(unit, taf_gate, pid));

    SHR_NULL_CHECK(status, _SHR_E_PARAM, "status");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_status_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_status_t * status)
{
    const bcm_tsn_taf_profile_t *profile_ptr;
    dnx_tsn_profile_state_e profile_state;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_profile_status_get_verify(unit, taf_gate, pid, status));

    bcm_tsn_taf_profile_status_t_init(status);

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.get(unit, pid, &profile_state));

    switch (profile_state)
    {
        case DNX_TSN_PROFILE_STATE_INIT:
            status->profile_state = bcmTsnTafProfileInit;
            break;
        case DNX_TSN_PROFILE_STATE_PENDING:
            status->profile_state = bcmTsnTafProfilePending;
            break;
        case DNX_TSN_PROFILE_STATE_ACTIVATED:

            if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_hw_support))
            {
                uint32 is_running = 0;

                SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_is_running_hw_get(unit, taf_gate, &is_running));
                status->profile_state = (is_running) ? bcmTsnTafProfileActive : bcmTsnTafProfilePending;
            }
            else
            {
                status->profile_state = bcmTsnTafProfileActive;
            }

            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "state not supported\n");
    }

    if (status->profile_state == bcmTsnTafProfileActive)
    {

        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.
                        config_change_time.get(unit, pid, &status->config_change_time));
    }

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.get(unit, pid, &profile_ptr));
    status->num_entries = profile_ptr->num_entries;
    sal_memcpy(&(status->entries), &(profile_ptr->entries),
               sizeof(bcm_tsn_taf_entry_t) * BCM_TSN_TAF_CALENDAR_TABLE_SIZE);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_control_reset_verify(
    int unit,
    int taf_gate,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_id_verify(unit, taf_gate));

    if (arg != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Arg should be 1");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_control_reset(
    int unit,
    int taf_gate,
    int arg)
{
    int is_mutex_locked = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_control_reset_verify(unit, taf_gate, arg));

    if (dnx_data_tsn.general.feature_get(unit, dnx_data_tsn_general_tsn_thread))
    {
        SHR_IF_ERR_EXIT(dnx_tsn_thread_control_reset(unit, DNX_TSN_GATE_TYPE_TAF, taf_gate));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "gate reset not supported on this device\n");
    }

exit:
    if (is_mutex_locked)
    {

        SHR_IF_ERR_CONT(dnx_tsn_db.mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_state_active_set(
    int unit,
    bcm_tsn_taf_profile_id_t pid)
{
    int taf_gate;
    int pid_i, pid_min, pid_max;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_from_profile_get(unit, pid, &taf_gate));

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_id_range_get(unit, taf_gate, &pid_min, &pid_max));

    for (pid_i = pid_min; pid_i < pid_max; pid_i++)
    {
        if (pid != pid_i)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.
                            profile_state.set(unit, pid_i, DNX_TSN_PROFILE_STATE_INIT));
        }
    }

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.set(unit, pid, DNX_TSN_PROFILE_STATE_ACTIVATED));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_hw_update(
    int unit,
    uint32 set,
    dnx_tsn_taf_pid_bmp_t * active_bmp)
{
    const bcm_tsn_taf_profile_t *profile_ptr;
    int entry_idx = 0;
    bcm_ptp_timestamp_t ptp_base_time;
    uint32 last_ti_entry;
    uint64 cycle_time_nanosec, cycle_time_ticks;
    uint64 ticks_sum;
    uint64 tsn_counter_base_time;
    int taf_gate;
    int pid_i;
    uint32 ti_index_start = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_TSN_TAF_PID_BMP_BIT_ITER(active_bmp, pid_i)
    {

        SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_from_profile_get(unit, pid_i, &taf_gate));

        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.get(unit, pid_i, &profile_ptr));
        last_ti_entry = profile_ptr->num_entries - 1;

        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.config_change_time.get(unit, pid_i, &ptp_base_time));
        SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_ptp_time_to_tsn_counter(unit, &ptp_base_time, &tsn_counter_base_time));

        SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_params_hw_set
                        (unit, set, taf_gate, last_ti_entry, TRUE, tsn_counter_base_time,
                         profile_ptr->entries[0].state, ti_index_start));

        COMPILER_64_ZERO(ticks_sum);
        for (entry_idx = 0; entry_idx < profile_ptr->num_entries - 1; entry_idx++)
        {
            COMPILER_64_ADD_32(ticks_sum, profile_ptr->entries[entry_idx].ticks);

            SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_control_list_hw_set
                            (unit, set, ti_index_start + entry_idx, profile_ptr->entries[entry_idx].ticks,
                             profile_ptr->entries[entry_idx + 1].state));
        }

        COMPILER_64_SET(cycle_time_nanosec, profile_ptr->ptp_cycle_time_upper, profile_ptr->ptp_cycle_time);
        SHR_IF_ERR_EXIT(dnx_tsn_counter_nanoseconds_to_ticks_convert(unit, cycle_time_nanosec, &cycle_time_ticks));
        COMPILER_64_SUB_64(cycle_time_ticks, ticks_sum);

        SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_control_list_hw_set
                        (unit, set, ti_index_start + profile_ptr->num_entries - 1, cycle_time_ticks,
                         profile_ptr->entries[0].state));

        ti_index_start += profile_ptr->num_entries;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_profile_multi_hw_update(
    int unit,
    dnx_tsn_taf_pid_bmp_t * requested_bmp,
    dnx_tsn_taf_pid_bmp_t * updated_bmp)
{
    int max_profiles = dnx_data_tsn.taf.nof_gates_get(unit) * dnx_data_tsn.taf.max_profiles_per_gate_get(unit);
    int pid_i, pid_min, pid_max;
    dnx_tsn_profile_state_e profile_state;
    dnx_tsn_taf_pid_bmp_t active_bmp;
    uint32 set;
    int taf_gate;
    bcm_ptp_timestamp_t config_change_time;
    uint64 curr_time_nsec, change_time_nsec;

    SHR_FUNC_INIT_VARS(unit);

    DNX_TSN_TAF_PID_BMP_INIT(&active_bmp);

    for (pid_i = 0; pid_i < max_profiles; pid_i++)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.get(unit, pid_i, &profile_state));
        if (profile_state == DNX_TSN_PROFILE_STATE_ACTIVATED)
        {
            DNX_TSN_TAF_PID_BMP_BITSET(&active_bmp, pid_i);
        }
    }

    DNX_TSN_TAF_PID_BMP_BIT_ITER(requested_bmp, pid_i)
    {

        SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_from_profile_get(unit, pid_i, &taf_gate));
        SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_id_range_get(unit, taf_gate, &pid_min, &pid_max));
        SHR_BITCLR_RANGE(active_bmp.bitmap, pid_min, dnx_data_tsn.taf.max_profiles_per_gate_get(unit));

        DNX_TSN_TAF_PID_BMP_BITSET(&active_bmp, pid_i);
    }

    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_get(unit, &set));

    DNX_TSN_TAF_PID_BMP_BIT_ITER(requested_bmp, pid_i)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_from_profile_get(unit, pid_i, &taf_gate));
        SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_params_active_hw_set(unit, DNX_TSN_TAF_INCATIVE_SET(set), taf_gate, 0));
    }

    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_set(unit, DNX_TSN_TAF_INCATIVE_SET(set)));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_set(unit, DNX_TSN_TAF_INCATIVE_SET(set)));

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_hw_update(unit, set, &active_bmp));

    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_set(unit, set));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_set(unit, set));

    SHR_IF_ERR_EXIT(dnx_tsn_counter_curr_time_get(unit, &curr_time_nsec));

    DNX_TSN_TAF_PID_BMP_INIT(updated_bmp);
    DNX_TSN_TAF_PID_BMP_BIT_ITER(requested_bmp, pid_i)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.config_change_time.get(unit, pid_i, &config_change_time));
        SHR_IF_ERR_EXIT(utilex_ptp_time_to_nanoseconds_convert(unit, &config_change_time, &change_time_nsec));
        if (COMPILER_64_LT(curr_time_nsec, change_time_nsec))
        {
            DNX_TSN_TAF_PID_BMP_BITSET(updated_bmp, pid_i);
        }
    }

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_hw_update(unit, DNX_TSN_TAF_INCATIVE_SET(set), &active_bmp));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tsn_taf_gate_hw_clear(
    int unit,
    int taf_gate)
{
    uint32 set;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_get(unit, &set));

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_params_hw_clear(unit, DNX_TSN_TAF_INCATIVE_SET(set), taf_gate));

    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_set(unit, DNX_TSN_TAF_INCATIVE_SET(set)));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_set(unit, DNX_TSN_TAF_INCATIVE_SET(set)));

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_params_hw_clear(unit, set, taf_gate));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_gate_reset(
    int unit,
    int taf_gate)
{
    int pid_i, pid_min, pid_max;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_hw_clear(unit, taf_gate));

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_id_range_get(unit, taf_gate, &pid_min, &pid_max));

    for (pid_i = pid_min; pid_i < pid_max; pid_i++)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.set(unit, pid_i, DNX_TSN_PROFILE_STATE_INIT));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_gate_deactivate_all(
    int unit)
{
    int max_profiles = dnx_data_tsn.taf.nof_gates_get(unit) * dnx_data_tsn.taf.max_profiles_per_gate_get(unit);
    dnx_tsn_profile_state_e profile_state;
    int pid_i;
    uint32 set;
    int taf_gate;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_get(unit, &set));

    for (pid_i = 0; pid_i < max_profiles; pid_i++)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.get(unit, pid_i, &profile_state));
        if (profile_state == DNX_TSN_PROFILE_STATE_ACTIVATED)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_from_profile_get(unit, pid_i, &taf_gate));
            SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_params_active_hw_set(unit, DNX_TSN_TAF_INCATIVE_SET(set), taf_gate, 0));
        }
    }

    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_set(unit, DNX_TSN_TAF_INCATIVE_SET(set)));
    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_set(unit, DNX_TSN_TAF_INCATIVE_SET(set)));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tsn_taf_gate_base_time_restart(
    int unit)
{
    int max_profiles = dnx_data_tsn.taf.nof_gates_get(unit) * dnx_data_tsn.taf.max_profiles_per_gate_get(unit);
    const bcm_tsn_taf_profile_t *profile_ptr;
    dnx_tsn_profile_state_e profile_state;
    bcm_ptp_timestamp_t ptp_base_time, config_change_time;
    uint64 tsn_counter_base_time;
    uint64 cycle_time_nsec;
    int pid_i;
    uint32 set;
    int taf_gate;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_get(unit, &set));

    for (pid_i = 0; pid_i < max_profiles; pid_i++)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.get(unit, pid_i, &profile_state));
        if (profile_state == DNX_TSN_PROFILE_STATE_ACTIVATED)
        {

            SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.get(unit, pid_i, &profile_ptr));
            SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_from_profile_get(unit, pid_i, &taf_gate));

            COMPILER_64_SET(cycle_time_nsec, profile_ptr->ptp_cycle_time_upper, profile_ptr->ptp_cycle_time);
            sal_memcpy(&ptp_base_time, &profile_ptr->ptp_base_time, sizeof(bcm_ptp_timestamp_t));
            SHR_IF_ERR_EXIT(dnx_tsn_counter_config_change_time_get
                            (unit, &ptp_base_time, cycle_time_nsec, 1000000, &config_change_time));

            SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_ptp_time_to_tsn_counter
                            (unit, &config_change_time, &tsn_counter_base_time));

            SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_params_base_time_hw_set
                            (unit, DNX_TSN_TAF_INCATIVE_SET(set), taf_gate, tsn_counter_base_time, TRUE));

            SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_set(unit, DNX_TSN_TAF_INCATIVE_SET(set)));
            SHR_IF_ERR_EXIT(dnx_tsn_taf_active_set_hw_set(unit, DNX_TSN_TAF_INCATIVE_SET(set)));

            SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_params_base_time_hw_set(unit, set, taf_gate, tsn_counter_base_time, TRUE));

            SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.config_change_time.set(unit, pid_i, config_change_time));

            set = DNX_TSN_TAF_INCATIVE_SET(set);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
