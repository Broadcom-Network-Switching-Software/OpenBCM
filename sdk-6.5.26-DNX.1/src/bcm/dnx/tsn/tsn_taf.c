/**
 * \file src/bcm/dnx/tsn/tsn_taf.c
 * 
 *
 * TAF (Time aware filtering) functionality
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

/*
 * Include files.
 * {
 */
#include <bcm/tsn.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_tsn_access.h>

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>
#include <sal/core/boot.h>

#include "tsn_counter.h"
#include "tsn_dbal.h"
#include "tsn_thread.h"

/*
 * }
 */

/*
 * Defines:
 * {
 */

#define TSN_TAF_GATE_RSRC_MNGR_ID "TSN_TAF_GATE_ID"
#define TSN_TAF_PID_RSRC_MNGR_ID "TSN_TAF_PID"

 /*
  * }
  */

/**
 * \brief - initiate all allocation managers for this module
 */
static shr_error_e
dnx_tsn_taf_res_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    /*
     * TAF gate resource manager
     */
    data.first_element = 0;
    data.nof_elements = dnx_data_tsn.taf.nof_gates_get(unit);

    sal_strncpy(data.name, TSN_TAF_GATE_RSRC_MNGR_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.gate_alloc.create(unit, &data, NULL));

    /*
     * Profile ID resource manager
     */
    data.first_element = 0;
    data.nof_elements = dnx_data_tsn.taf.nof_gates_get(unit) * dnx_data_tsn.taf.max_profiles_per_gate_get(unit);

    sal_strncpy(data.name, TSN_TAF_PID_RSRC_MNGR_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.pid_alloc.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initiate TAF SW
 */
static shr_error_e
dnx_tsn_taf_sw_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_res_mngr_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initiate TAF HW
 */
static shr_error_e
dnx_tsn_taf_hw_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_global_config_hw_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_tsn_taf_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** initiate TAF HW */
    SHR_IF_ERR_EXIT(dnx_tsn_taf_hw_init(unit));

    /** initiate TAF SW */
    SHR_IF_ERR_EXIT(dnx_tsn_taf_sw_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Get the profile id range per gate
 */
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

/**
 * \brief -
 *   Verify TAF gate ID
 */
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

/**
 * \brief -
 *   Verify TAF gate ID
 */
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

/**
 * \brief -
 *   Indicates if profile is busy (active or pending)
 */
static shr_error_e
dnx_tsn_taf_profile_is_busy(
    int unit,
    int pid,
    uint8 *is_busy)
{
    bcm_tsn_taf_profile_state_t profile_state;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.get(unit, pid, &profile_state));
    if (profile_state != bcmTsnTafProfileInit)
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

/**
 * \brief -
 *   Indicates if gate has pending requests.
 */
static shr_error_e
dnx_tsn_taf_gate_is_pending(
    int unit,
    int taf_gate,
    uint8 *is_pending)
{
    uint8 is_allocated = 0;
    int pid_i, pid_min, pid_max;
    bcm_tsn_taf_profile_state_t profile_state;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_id_range_get(unit, taf_gate, &pid_min, &pid_max));

    *is_pending = FALSE;
    for (pid_i = pid_min; pid_i < pid_max; pid_i++)
    {
        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.pid_alloc.is_allocated(unit, pid_i, &is_allocated));
        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.get(unit, pid_i, &profile_state));
            if (profile_state == bcmTsnTafProfilePending)
            {
                *is_pending = TRUE;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify TAF profile.
 */
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

/**
 * \brief -
 *   Verify inputs for bcm_dnx_tsn_taf_gate_create
 */
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

/*
 * see .h file
 */
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

/**
 * \brief -
 *   Verify inputs for bcm_dnx_tsn_taf_gate_destroy
 */
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

/*
 * see .h file
 */
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

/**
 * \brief -
 *   Verify inputs for bcm_dnx_tsn_taf_profile_create
 */
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

/*
 * see .h file
 */
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

    /*
     * allocate pid 
     */
    SHR_IF_ERR_EXIT(dnx_tsn_taf_profile_id_range_get(unit, taf_gate, &pid_min, &pid_max));

    sal_memset(&alloc_info, 0x0, sizeof(resource_tag_bitmap_utils_extra_arg_alloc_info_t));
    alloc_flags |= RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
    alloc_info.range_start = pid_min;
    alloc_info.range_end = pid_max;

    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.pid_alloc.allocate_single(unit, alloc_flags, (void *) &alloc_info, pid));

    /*
     * init profile state 
     */
    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.set(unit, *pid, bcmTsnTafProfileInit));

    /*
     * update pid database 
     */
    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.set(unit, *pid, profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Verify inputs for bcm_dnx_tsn_taf_gate_destroy
 */
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

/*
 * see .h file
 */
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

/**
 * \brief -
 *   Verify inputs for bcm_dnx_tsn_taf_profile_set
 */
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

/*
 * see .h file
 */
shr_error_e
dnx_tsn_taf_profile_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify API inputs 
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_profile_set_verify(unit, taf_gate, pid, profile));

    /*
     * update pid database 
     */
    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.set(unit, pid, profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Verify inputs for bcm_dnx_tsn_taf_profile_get
 */
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

/*
 * see .h file
 */
shr_error_e
dnx_tsn_taf_profile_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_t * profile)
{
    const bcm_tsn_taf_profile_t *profile_ptr;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify API inputs 
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_profile_set_verify(unit, taf_gate, pid, profile));

    /*
     * get profile data from database 
     */
    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.get(unit, pid, &profile_ptr));
    sal_memcpy(profile, profile_ptr, sizeof(bcm_tsn_taf_profile_t));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Verify inputs for bcm_dnx_tsn_taf_profile_commit
 */
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

/*
 * see .h file
 */
shr_error_e
dnx_tsn_taf_profile_commit(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid)
{
    const bcm_tsn_taf_profile_t *profile_ptr;
    bcm_ptp_timestamp_t ptp_base_time;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify API inputs 
     */
    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_profile_commit_verify(unit, taf_gate, pid));

    /** change state to pending */
    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.set(unit, pid, bcmTsnTafProfilePending));

    /** get profile info */
    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.get(unit, pid, &profile_ptr));
    sal_memcpy(&ptp_base_time, &profile_ptr->ptp_base_time, sizeof(bcm_ptp_timestamp_t));

    if (dnx_data_tsn.general.feature_get(unit, dnx_data_tsn_general_tsn_thread))
    {
        /** commit to thread profile manager */
        SHR_IF_ERR_EXIT(dnx_tsn_thread_profile_commit(unit, &ptp_base_time, DNX_TSN_GATE_TYPE_TAF, pid));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Profile commit not supported on this device\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Verify inputs for bcm_dnx_tsn_taf_profile_status_get
 */
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

/*
 * see .h file
 */
shr_error_e
dnx_tsn_taf_profile_status_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_tsn_taf_profile_status_t * status)
{
    const bcm_tsn_taf_profile_t *profile_ptr;

    SHR_FUNC_INIT_VARS(unit);

    /** verify API inputs */
    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_profile_status_get_verify(unit, taf_gate, pid, status));

    /** reset status struct */
    bcm_tsn_taf_profile_status_t_init(status);

    /** get profile status */
    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_state.get(unit, pid, &status->profile_state));

    if (status->profile_state == bcmTsnTafProfileActive)
    {
        /** if profile is active - get actual change time */
        SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.
                        config_change_time.get(unit, pid, &status->config_change_time));
    }

    /*
     * get calendar info 
     */
    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.get(unit, pid, &profile_ptr));
    status->num_entries = profile_ptr->num_entries;
    sal_memcpy(&(status->entries), &(profile_ptr->entries),
               sizeof(bcm_tsn_taf_entry_t) * BCM_TSN_TAF_CALENDAR_TABLE_SIZE);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Verify inputs for bcmTsnTafProfileExpired
 */
static shr_error_e
dnx_tsn_taf_gate_reset_verify(
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

/*
 * see .h file
 */
shr_error_e
dnx_tsn_taf_gate_reset(
    int unit,
    int taf_gate,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify API inputs */
    SHR_INVOKE_VERIFY_DNXC(dnx_tsn_taf_gate_reset_verify(unit, taf_gate, arg));

    /*
     * 1. remove all pending profiles from thread.
     * 2. send indication to thread to reset TAF gate
     */
    SHR_IF_ERR_EXIT(dnx_tsn_thread_profile_reset_all(unit, taf_gate, DNX_TSN_GATE_TYPE_TAF));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Update TAF HW tables for one profile/set
 */
shr_error_e
dnx_tsn_taf_profile_hw_update(
    int unit,
    uint32 set,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    uint32 ti_index_start)
{
    const bcm_tsn_taf_profile_t *profile_ptr;
    int entry_idx = 0;
    bcm_ptp_timestamp_t ptp_base_time;
    uint32 last_ti_entry;
    uint64 cycle_time_nanosec, cycle_time_ticks;
    uint64 ticks_sum;
    uint64 tsn_counter_base_time;

    SHR_FUNC_INIT_VARS(unit);

    /** get profile info */
    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.profile_data.get(unit, pid, &profile_ptr));
    last_ti_entry = profile_ptr->num_entries - 1;

    /** Get TSN counter base time */
    SHR_IF_ERR_EXIT(dnx_tsn_db.taf.taf_profiles_data.config_change_time.get(unit, pid, &ptp_base_time));
    SHR_IF_ERR_EXIT(dnx_tsn_counter_convert_ptp_time_to_tsn_counter(unit, &ptp_base_time, &tsn_counter_base_time));

    /** Set TAF gate params attributes */
    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_params_hw_set
                    (unit, set, taf_gate, last_ti_entry, TRUE, tsn_counter_base_time,
                     profile_ptr->entries[0].state, ti_index_start));

    COMPILER_64_ZERO(ticks_sum);
    for (entry_idx = 0; entry_idx < profile_ptr->num_entries - 1; entry_idx++)
    {
        COMPILER_64_ADD_32(ticks_sum, profile_ptr->entries[entry_idx].ticks);

        /** Set TAF gate control list to hw */
        SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_control_list_hw_set
                        (unit, set, ti_index_start + entry_idx, profile_ptr->entries[entry_idx].ticks,
                         profile_ptr->entries[entry_idx + 1].state));
    }

    /*
     * set last entry - convert cycle time to ticks and subtract total time intervals ticks
     */
    COMPILER_64_SET(cycle_time_nanosec, profile_ptr->ptp_cycle_time_upper, profile_ptr->ptp_cycle_time);
    SHR_IF_ERR_EXIT(dnx_tsn_counter_nanoseconds_to_ticks_convert(unit, cycle_time_nanosec, &cycle_time_ticks));
    COMPILER_64_SUB_64(cycle_time_ticks, ticks_sum);

    SHR_IF_ERR_EXIT(dnx_tsn_taf_gate_control_list_hw_set
                    (unit, set, ti_index_start + profile_ptr->num_entries - 1, cycle_time_ticks,
                     profile_ptr->entries[0].state));

exit:
    SHR_FUNC_EXIT;
}
