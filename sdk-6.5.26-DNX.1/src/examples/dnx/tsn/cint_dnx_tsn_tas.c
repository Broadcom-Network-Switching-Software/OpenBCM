/*
 * cint_dnx_tsn_tas.c
 *
 *  Created on: May 16, 2022
 *      Author: nt893888
 */


/**
 * \brief - Get new TAs profile:
 * Parameters:
 * unit [INPUT] - unit
 * port [INPUT] - logical port on which to create TAS profile
 * total_cycle_time_ns [INPUT] - total time of all intervals (time intervals) in ns
 * nof_intervals [INPUT] - number of states intervals in the cycle
 * tc [INPUT] - tc value of time critical traffic
 * tc_percentage [INPUT] - percentage from total_intervals_time on which time critical tc will be open
 * add_to_curr_time_sec [INPUT] - time in seconds added to current time for profile start time
 * profile [OUTPUT] - new profile
 * pid [OUTPUT] - profile ID value
 */
int cint_tsn_tas_new_profile_get(int unit, int port, int total_cycle_time_ns, int nof_intervals, int tc, int tc_percentage, int add_to_curr_time_sec, bcm_cosq_tas_profile_t *profile, int* pid)
{
    int rv = BCM_E_NONE;
    int div_factor;
    int entry = 0;
    int nof_priorities;
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 open_tc_state;
    uint32 close_tc_state;
    float open_tc_ticks;
    float close_tc_ticks;
    uint32 ns_to_ticks = 1000;

    profile->num_entries = nof_intervals * 2;
    div_factor = nof_intervals;

    /** get current time and add seconds*/
    rv = dnx_tsn_counter_ptp_curr_time_get(unit, &(profile->ptp_base_time));
    if (BCM_FAILURE(rv))
    {
        printf("Error, dnx_tsn_counter_curr_time_get \n");
        return rv;
    }

    COMPILER_64_ADD_32(profile->ptp_base_time.seconds, add_to_curr_time_sec);

    profile->ptp_cycle_time = total_cycle_time_ns;
    profile->ptp_cycle_time_upper = 0;

    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_get\n");
        return rv;
    }
    nof_priorities = mapping_info.num_priorities;

    /** calculate states and time intervals for each tc state*/
    open_tc_state = 1 << tc;
    close_tc_state = open_tc_state ^ ((1 << nof_priorities) - 1);

    open_tc_ticks = ((total_cycle_time_ns/ div_factor) * (tc_percentage / 100)) / ns_to_ticks;
    close_tc_ticks = ((total_cycle_time_ns /div_factor)  * ((100 - tc_percentage) / 100 )) / ns_to_ticks;

    for (entry = 0; entry < profile->num_entries; entry++)
    {
        if(entry % 2 == 0)
        {
            profile->entries[entry].ticks = open_tc_ticks;
            profile->entries[entry].state = open_tc_state;
        }
        else
        {
            profile->entries[entry].ticks = close_tc_ticks;
            profile->entries[entry].state = close_tc_state;
        }

        profile->entries[entry].flags = 0;
    }

    /** set last entry ticks*/
    profile->entries[profile->num_entries - 1].ticks = BCM_COSQ_TAS_ENTRY_TICKS_STAY;

    return BCM_E_NONE;
}

/**
 * \brief - Create and commit TAS profile:
 *
 * Profile example:
 * parameters:  total_cycle_time_ns = 100000
 *              nof_intervals = 4
 *              tc = 1
 *              tc_precentage = 20
 *
 * num_of_entries : 2 * (nof_cycles) = 8
 * each even entry the configuration will be:
 *              state bits: 01000000
 *              state ticks: (100000 * 0.2 / 4) / 1000
 *
 * each odd entry the configuration will be:
 *              state bits: 10111111
 *              state ticks: (100000 * 0.8 / 4) / 1000
 *
 * Tc traffic behavior graph will be as:
 *
 * open tc __          __          __          __
 *           |        |  |        |  |        |  |
 *           |        |  |        |  |        |  |
 *           |        |  |        |  |        |  |
 * close tc  |________|  |________|  |________|  |________
 *
 * Parameters:
 * unit [INPUT] - unit
 * port [INPUT] - logical port on which to create TAS profile
 * total_cycle_time_ns [INPUT] - total time of all intervals (time intervals) in ns
 * nof_intervals [INPUT] - number of states intervals in the cycle
 * tc [INPUT] - tc value of time critical traffic
 * tc_percentage [INPUT] - percentage from total_intervals_time on which time critical tc will be open
 * add_to_curr_time_sec [INPUT] - time in seconds added to current time for profile start time
 * pid [OUTPUT] - profile ID value
 */
int cint_tsn_tas_profile_create_and_commit(int unit, int port, int total_cycle_time_ns, int nof_intervals, int tc, int tc_percentage, int add_to_curr_time_sec, int* pid)
{
    int rv = BCM_E_FAIL;
    bcm_cosq_tas_profile_t profile;

    rv = cint_tsn_tas_new_profile_get(unit, port, total_cycle_time_ns, nof_intervals, tc, tc_percentage, add_to_curr_time_sec, &profile, pid);
    if (BCM_FAILURE(rv))
    {
        printf("Error, cint_tsn_tas_new_profile_get \n");
        return rv;
    }

    rv = bcm_cosq_tas_profile_create(unit, port, &profile, pid);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_cosq_tas_profile_create \n");
        return rv;
    }

    rv = bcm_cosq_tas_profile_commit(unit, port, *pid);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_cosq_tas_profile_commit \n");
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief - Polling function for active state
 * Parameters:
 * unit [INPUT] - unit
 * port [INPUT] - local port on which TAS profile exist
 * pid [INPUT] - profile ID value
 * time_out [INPUT] - time in seconds to wait for active status
 */
int cint_tsn_tas_profile_active_state_poll(int unit, int port, int pid, int time_out)
{
    int rv = BCM_E_FAIL;
    bcm_cosq_tas_profile_status_t status;

    if (time_out < 0)
    {
        printf("Error,time_out can't be negative \n");
        return rv;
    }

    while (time_out >= 0)
    {
        rv = bcm_cosq_tas_profile_status_get(unit, port, pid, &status);
        if (BCM_FAILURE(rv))
        {
            printf("Error, bcm_cosq_tas_profile_status_get \n");
            return rv;
        }

        if (status.profile_state == bcmCosqTASProfileActive)
        {
            rv = BCM_E_NONE;
            break;
        }
        else
        {
            sal_sleep(1);
            time_out -= 1;
        }
    }

    return rv;
}

/**
 * \brief - Set new TAS profile to existing pid and commit:
 *
 * Parameters:
 * unit [INPUT] - unit
 * port [INPUT] - logical port on which to create TAS profile
 * total_cycle_time_ns [INPUT] - total time of all intervals (time intervals) in ns
 * nof_intervals [INPUT] - number of states intervals in the cycle
 * tc [INPUT] - tc value of time critical traffic
 * tc_percentage [INPUT] - percentage from total_intervals_time on which time critical tc will be open
 * add_to_curr_time_sec [INPUT] - time in seconds added to current time for profile start time
 * pid [input] - profile ID to be set
 */
int cint_tsn_tas_profile_set_and_commit(int unit, int port, int total_cycle_time_ns, int nof_intervals, int tc, int tc_percentage, int add_to_curr_time_sec, int pid)
{
    int rv = BCM_E_FAIL;
    bcm_cosq_tas_profile_t profile;

    rv = cint_tsn_tas_new_profile_get(unit, port, total_cycle_time_ns, nof_intervals, tc, tc_percentage, add_to_curr_time_sec, &profile, pid);
    if (BCM_FAILURE(rv))
    {
        printf("Error, cint_tsn_tas_new_profile_get \n");
        return rv;
    }

    rv = bcm_cosq_tas_profile_set(unit, port, pid, &profile);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_cosq_tas_profile_set \n");
        return rv;
    }

    rv = bcm_cosq_tas_profile_commit(unit, port, pid);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_cosq_tas_profile_commit \n");
        return rv;
    }

    return BCM_E_NONE;
}
