/** 
 * \file algo/consistent_hashing/consistent_hashing_calendar.c
 *
 * Implementation of consistent hashing calendar related 
 * functions 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_UTILS

/** Include files: */
/** {  */
#include <bcm_int/dnx/algo/consistent_hashing/consistent_hashing_manager.h>
#include "consistent_hashing_calendar.h"
#include <shared/shrextend/shrextend_debug.h>
/** } */

/** Defines */
/** { */
/** } */

/** Structs */
/** { */
/** } */

/** Functions: */
/** { */

/**
 * \brief - returns the number of entries a member should 
 *        recieve if the profile is in full state.
 * 
 * \param [in] member - the checked member
 * \param [in] total_nof_entries - total number of entries in 
 *        this calander
 * \param [in] max_nof_members - max number of members in this 
 *        calander
 *   
 * \return
 *   int 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static inline int
dnx_algo_chc_nof_entries_per_member_when_full_get(
    uint32 member,
    uint32 total_nof_entries,
    uint32 max_nof_members)
{
    return (total_nof_entries / max_nof_members) + ((member < (total_nof_entries % max_nof_members)) ? 1 : 0);
}

/**
 * \brief - get first entry index for member when group is full
 * 
 * \param [in] member - the checked member
 * \param [in] total_nof_entries - total number of entries in 
 *        this calendar
 * \param [in] max_nof_members - max number of members in this 
 *        calendar
 *   
 * \return
 *   int 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static inline int
dnx_algo_chc_first_entry_index_per_member_get(
    uint32 member,
    uint32 total_nof_entries,
    uint32 max_nof_members)
{
    /** assume each member recieve the same amount of entries,
     *  add the remaining in the amount needed, if the remianing
     *  is bigger than the member we need to add all the
     *  remaining, if not we need to add the number of the member
     *  which is actually the amount of remaining needed to
     *  acount for all the previous members */
    return (total_nof_entries / max_nof_members * member) +
        ((member < (total_nof_entries % max_nof_members)) ? member : (total_nof_entries % max_nof_members));
}

/**
 * \brief - return a fully populated calendar and an array with 
 *        the number of entries each member has in the calendar.
 *        the order in which the members are populated are as
 *        equally as possible and the remaining are going to the
 *        first members
 * 
 * \param [in] unit - unit number  
 * \param [in] nof_entries_in_calendar - number of entries in 
 *        the calendar
 * \param [in] nof_members_in_calendar - number of members in 
 *        the calendar
 * \param [out] calendar - returned populated calendar
 * \param [out] member_array - returned array with number of 
 *        entries per member
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_chc_fully_populate(
    int unit,
    uint32 nof_entries_in_calendar,
    uint32 nof_members_in_calendar,
    consistent_hashing_calendar_t * calendar,
    int *member_array)
{
    int min_entries_per_member = nof_entries_in_calendar / nof_members_in_calendar;
    int remaining = nof_entries_in_calendar % nof_members_in_calendar;
    int max_entries_per_member = min_entries_per_member + (remaining ? 1 : 0);
    int entry = 0;

    SHR_FUNC_INIT_VARS(unit);
    /** assign members with max_entries_per_member - those are
     *  the first remaining members */
    for (int member = 0; member < remaining; ++member)
    {
        for (int entry_counter = 0; entry_counter < max_entries_per_member; ++entry_counter)
        {
            calendar->lb_key_member_array[entry] = member;
            ++entry;
        }
    }
    /** assign members with min_entries_per_member - those are
     *  always the last members */
    for (int member = remaining; member < nof_members_in_calendar; ++member)
    {
        for (int entry_counter = 0; entry_counter < min_entries_per_member; ++entry_counter)
        {
            calendar->lb_key_member_array[entry] = member;
            ++entry;
        }
    }
    /** set member_array with number of entries each member has, quicker than counting it in the for loop above.. */
    for (int member = 0; member < nof_members_in_calendar; ++member)
    {
        member_array[member] = dnx_algo_chc_nof_entries_per_member_when_full_get
            (member, nof_entries_in_calendar, nof_members_in_calendar);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - equilize a calendar according to provided delta 
 *        array. 
 * 
 * \param [in] unit - unit number
 * \param [in] nof_changes - total number of changes required to 
 *        be done - used to short circuit for loops
 * \param [in] max_nof_members_in_calendar - max number of 
 *        members a calendar was configured to contain
 * \param [in] nof_entries_in_calendar - number of entries in 
 *        the calendar
 * \param [in] wanted_members_array - each index which is not 0 
 *        represents a wanted member
 * \param [in] delta_array - needed changes are found in this 
 *        array, it has a cell for each member. if the value is
 *        positive it means that this member needs to give the
 *        mentioned number entries, if it is negative it means
 *        that this member needs to take the mentioned number of
 *        entries
 * \param [in,out] calendar - the calendar to change according 
 *        to the delta_array
 *   
 * \return
 *   shr_error_e
 *  
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_chc_delta_equilize(
    int unit,
    int nof_changes,
    uint32 max_nof_members_in_calendar,
    uint32 nof_entries_in_calendar,
    int *wanted_members_array,
    int *delta_array,
    consistent_hashing_calendar_t * calendar)
{
    SHR_FUNC_INIT_VARS(unit);
    /** equilize calendar according to delta array */
    for (int next_taker = 0, member = 0; (member < max_nof_members_in_calendar) && (nof_changes > 0); ++member)
    {
        /** search for members that are not wanted - thier original
         *  entries (when calendar is full) are candidates for
         *  changes */
        if (wanted_members_array[member] == 0)
        {
            /** get entries coresponding to this member */
            int first_entry_to_check = dnx_algo_chc_first_entry_index_per_member_get
                (member, nof_entries_in_calendar, max_nof_members_in_calendar);
            int nof_entries_to_check = dnx_algo_chc_nof_entries_per_member_when_full_get
                (member, nof_entries_in_calendar, max_nof_members_in_calendar);
            for (int entry = first_entry_to_check; entry < first_entry_to_check + nof_entries_to_check; ++entry)
            {
                int next_giver = calendar->lb_key_member_array[entry];
                /** check if entry contain a giver */
                if (delta_array[next_giver] > 0)
                {
                    /** giver was found */
                    /** find next taker */
                    int search_action_counter;
                    for (search_action_counter = 0;
                         search_action_counter < max_nof_members_in_calendar; ++search_action_counter)
                    {
                        if (delta_array[next_taker] < 0)
                        {
                            /** taker was found */
                            /** change entry from giver to taker, align delta array accordingly */
                            calendar->lb_key_member_array[entry] = next_taker;
                            ++delta_array[next_taker];
                            --delta_array[next_giver];
                            --nof_changes;
                            next_taker = (next_taker + 1) % max_nof_members_in_calendar;
                            break;
                        }
                        next_taker = (next_taker + 1) % max_nof_members_in_calendar;
                    }
                    if (search_action_counter >= max_nof_members_in_calendar)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unable to find taker, something went wrong\n");
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the delta of current destribution from 
 *        ideal destribution
 * 
 * \param [in] unit - uint number
 * \param [in] nof_unique_members - wanted number of unique 
 *        members in ideal destribution
 * \param [in] current_members_array - each index contains the 
 *        current number of entries a member populates in
 *        calendar
 * \param [in] wanted_members_array - each index which is not 0 
 *        represents a wanted member
 * \param [in] nof_entries_in_calendar - number of entries in 
 *        the calendar
 * \param [in] max_nof_members_in_calendar - max number of 
 *        members a calendar was configured to contain
 * \param [out] delta_array - resulted delta for each member 
 *        between current number of entries to ideal number of
 *        entries.
 * \param [out] nof_needed_changes - number of needed changes 
 *        between current state to ideal state
 *  
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_chc_delta_from_ideal_destribution_get(
    int unit,
    uint32 nof_unique_members,
    int *current_members_array,
    int *wanted_members_array,
    uint32 nof_entries_in_calendar,
    uint32 max_nof_members_in_calendar,
    int *delta_array,
    int *nof_needed_changes)
{
    int sum_takers = 0;
    int sum_givers = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** calculate deltas between current to ideal number of entries
     *  per member, negative means need to take entries, positive
     *  means need to give entries */
    for (int wanted_member_counter = 0, member = 0; member < max_nof_members_in_calendar; ++member)
    {
        /** ideal number of entries is either 0 for unwanted members,
         *  or calculated for wanted members */
        int ideal_nof_entries = 0;
        if (wanted_members_array[member] != 0)
        {
            ideal_nof_entries = dnx_algo_chc_nof_entries_per_member_when_full_get
                (wanted_member_counter, nof_entries_in_calendar, nof_unique_members);
            ++wanted_member_counter;
        }
        /** set delta array with result */
        delta_array[member] = current_members_array[member] - ideal_nof_entries;
        /** sum number of entries to take and to give */
        if (delta_array[member] < 0)
        {
            sum_takers += delta_array[member];
        }
        else
        {
            sum_givers += delta_array[member];
        }
    }
    /** make sure that need to give and take the same amount of entries */
    if (sum_takers + sum_givers != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "not the same number of givers and takers, something is wrong\n");
    }
    *nof_needed_changes = sum_givers;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - counts the number of entries dedicated for each 
 *        member in the calendar, also returns the number of
 *        unique members
 * 
 * \param [in] unit - unit number
 * \param [in] max_nof_members - max number of members possible 
 *        for this calendar, this is also the size of the
 *        members_array
 * \param [in] calendar - input calendar to count
 * \param [in] nof_entries_in_calendar - number of relevant 
 *        entries in the calendar
 * \param [out] members_array - returned counted member array
 * \param [out] nof_unique_members - returned number of unique 
 *        members
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_chc_count_entries_per_member(
    int unit,
    uint32 max_nof_members,
    consistent_hashing_calendar_t * calendar,
    uint32 nof_entries_in_calendar,
    int *members_array,
    uint32 *nof_unique_members)
{
    SHR_FUNC_INIT_VARS(unit);
    /** clear nof_unique_members */
    *nof_unique_members = 0;
    /** clear members array */
    sal_memset(members_array, 0, max_nof_members * sizeof(members_array[0]));
    /** iterate calendar and count entries per member */
    for (int calendar_index = 0; calendar_index < nof_entries_in_calendar; ++calendar_index)
    {
        int current_member;
        /** get current member */
        current_member = calendar->lb_key_member_array[calendar_index];
        /** verify that current_member is valid */
        if (current_member >= max_nof_members)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "calendar is corrupted, invalid member was found in it\n");
        }
        /** add current_member to count */
        ++members_array[current_member];
    }

    /** count unique members */
    /** this is done in a seperate loop because in the common
     *  case we will have much more calendar indexes than
     *   max_nof_members */
    for (int current_member = 0; current_member < max_nof_members; ++current_member)
    {
        if (members_array[current_member] != 0)
        {
            ++(*nof_unique_members);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_algo_consistent_hashing_calendar_calculate(
    int unit,
    uint32 max_nof_members_in_calendar,
    uint32 nof_entries_in_calendar,
    uint32 current_nof_members_in_calendar,
    uint32 *specific_members_array,
    consistent_hashing_calendar_t * calendar)
{
    int *current_members_array_p = NULL;
    int *wanted_members_array_p = NULL;
    int *delta_array_p = NULL;

    int nof_needed_changes;
    SHR_FUNC_INIT_VARS(unit);

    /** set calendar entries to 0 */
    sal_memset(calendar->lb_key_member_array, 0, sizeof(calendar->lb_key_member_array));

    SHR_ALLOC(current_members_array_p, sizeof(*current_members_array_p) * max_nof_members_in_calendar,
              "current_members_array_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    /** init to 0 arrays that will not be initialized completely by functions */
    SHR_ALLOC_SET_ZERO(wanted_members_array_p, sizeof(*wanted_members_array_p) * max_nof_members_in_calendar,
                       "wanted_members_array_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(delta_array_p, sizeof(*delta_array_p) * max_nof_members_in_calendar, "delta_array_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    /** fill calendar completely */
    SHR_IF_ERR_EXIT(dnx_algo_chc_fully_populate
                    (unit, nof_entries_in_calendar, max_nof_members_in_calendar, calendar, current_members_array_p));
    /** if not full calendar some additional steps are requiered */
    if (max_nof_members_in_calendar > current_nof_members_in_calendar)
    {
        /** set wanted members array */
        if (specific_members_array == NULL)
        {
            /** if specific members were not provided, the default is the
             *  first ones */
            for (int member = 0; member < current_nof_members_in_calendar; ++member)
            {
                wanted_members_array_p[member] = 1;
            }
        }
        else
        {
            /** if specific members were provided, they are used */
            for (int member = 0; member < current_nof_members_in_calendar; ++member)
            {
                int wanted_member = specific_members_array[member];
                if (wanted_member >= max_nof_members_in_calendar)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid member revieved\n");
                }
                wanted_members_array_p[wanted_member] = 1;
            }
        }
        /** get delta from ideal destribution */
        SHR_IF_ERR_EXIT(dnx_algo_chc_delta_from_ideal_destribution_get
                        (unit, current_nof_members_in_calendar, current_members_array_p, wanted_members_array_p,
                         nof_entries_in_calendar, max_nof_members_in_calendar, delta_array_p, &nof_needed_changes));

        /** equilize calander */
        SHR_IF_ERR_EXIT(dnx_algo_chc_delta_equilize
                        (unit, nof_needed_changes, max_nof_members_in_calendar, nof_entries_in_calendar,
                         wanted_members_array_p, delta_array_p, calendar));
    }
exit:
    SHR_FREE(current_members_array_p);
    SHR_FREE(wanted_members_array_p);
    SHR_FREE(delta_array_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief - populate a calendar with a single member
 * 
 * \param [in] unit - unit number
 * \param [in] member_to_add - member to add to the calendar
 * \param [in] nof_entries_in_calendar - the number of entries 
 *        in the calendar
 * \param [in] new_calendar - pointer to new calendar
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_chc_populate_first_member(
    int unit,
    uint32 member_to_add,
    uint32 nof_entries_in_calendar,
    consistent_hashing_calendar_t * new_calendar)
{

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(new_calendar->lb_key_member_array, 0, sizeof(new_calendar->lb_key_member_array));
    for (int i = 0; i < nof_entries_in_calendar; ++i)
    {
        new_calendar->lb_key_member_array[i] = member_to_add;
    }

    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_algo_consistent_hashing_calendar_member_add(
    int unit,
    consistent_hashing_calendar_t * current_calendar,
    uint32 member_to_add,
    uint32 max_nof_members_in_calendar,
    uint32 nof_entries_in_calendar,
    uint8 first_member_in_calendar,
    consistent_hashing_calendar_t * new_calendar)
{
    int *members_array_p = NULL;
    int *delta_array_p = NULL;

    int nof_changes;
    uint32 nof_unique_members;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(members_array_p, sizeof(*members_array_p) * max_nof_members_in_calendar, "members_array_p", "%s%s%s",
              EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(delta_array_p, sizeof(*delta_array_p) * max_nof_members_in_calendar, "delta_array_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    /** verify member to add is smaller than max_nof_members_in_calendar */
    if (member_to_add >= max_nof_members_in_calendar)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "member to add doesn't fit this calendar\n");
    }
    if (first_member_in_calendar)
    {
        SHR_IF_ERR_EXIT(dnx_algo_chc_populate_first_member(unit, member_to_add, nof_entries_in_calendar, new_calendar));
        new_calendar->profile_type = current_calendar->profile_type;
    }
    else
    {
        /** count entries per member and get unique nof members */
        SHR_IF_ERR_EXIT(dnx_algo_chc_count_entries_per_member
                        (unit, max_nof_members_in_calendar, current_calendar, nof_entries_in_calendar,
                         members_array_p, &nof_unique_members));
        /** verify member to add is not already found in calander */
        if (members_array_p[member_to_add] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "member to is already found in calendar\n");
        }
        /** copy current_calander to new_calander */
        *new_calendar = *current_calendar;

        /** overrun entries in original place with member_to_add */
        {
            int first_overrun_entry = dnx_algo_chc_first_entry_index_per_member_get
                (member_to_add, nof_entries_in_calendar, max_nof_members_in_calendar);
            int nof_entries_to_overrun = dnx_algo_chc_nof_entries_per_member_when_full_get
                (member_to_add, nof_entries_in_calendar, max_nof_members_in_calendar);
            /** fill entries with new member */
            for (int entry = first_overrun_entry; entry < first_overrun_entry + nof_entries_to_overrun; ++entry)
            {
                /** decrease count of entries in members_array_p */
                --members_array_p[new_calendar->lb_key_member_array[entry]];
                /** change in calendar to new member */
                new_calendar->lb_key_member_array[entry] = member_to_add;
            }
            /** compensate number of unique members for the member that is currently added */
            ++nof_unique_members;
            /** update entries in entries per member array */
            members_array_p[member_to_add] = nof_entries_to_overrun;
        }
        /** get delta from ideal entries to members destribution */
        SHR_IF_ERR_EXIT(dnx_algo_chc_delta_from_ideal_destribution_get
                        (unit, nof_unique_members, members_array_p, members_array_p, nof_entries_in_calendar,
                         max_nof_members_in_calendar, delta_array_p, &nof_changes));

        /** equilize calander */
        SHR_IF_ERR_EXIT(dnx_algo_chc_delta_equilize
                        (unit, nof_changes, max_nof_members_in_calendar, nof_entries_in_calendar,
                         members_array_p, delta_array_p, new_calendar));
    }
exit:
    SHR_FREE(members_array_p);
    SHR_FREE(delta_array_p);
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_algo_consistent_hashing_calendar_member_remove(
    int unit,
    consistent_hashing_calendar_t * current_calendar,
    uint32 member_to_remove,
    uint32 max_nof_members_in_calendar,
    uint32 nof_entries_in_calendar,
    consistent_hashing_calendar_t * new_calendar)
{
    int *members_array_p = NULL;
    int *wanted_members_array_p = NULL;
    int *delta_array_p = NULL;

    int nof_changes;
    uint32 nof_unique_members;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(members_array_p, sizeof(*members_array_p) * max_nof_members_in_calendar, "members_array_p", "%s%s%s",
              EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(wanted_members_array_p, sizeof(*wanted_members_array_p) * max_nof_members_in_calendar,
              "wanted_members_array_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(delta_array_p, sizeof(*delta_array_p) * max_nof_members_in_calendar, "delta_array_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    /** verify member to remove is smaller than max_nof_members_in_calendar */
    if (member_to_remove >= max_nof_members_in_calendar)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "member to remove doesn't fit this calendar\n");
    }
    /** count entries per member and get unique nof members */
    SHR_IF_ERR_EXIT(dnx_algo_chc_count_entries_per_member
                    (unit, max_nof_members_in_calendar, current_calendar, nof_entries_in_calendar,
                     members_array_p, &nof_unique_members));
    /** verify member to remove is found in calander */
    if (members_array_p[member_to_remove] == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "member is not found in calendar\n");
    }
    /** copy current_calander to new_calander */
    *new_calendar = *current_calendar;
    /** set wanted member array */
    sal_memcpy(wanted_members_array_p, members_array_p, sizeof(*members_array_p) * max_nof_members_in_calendar);
    wanted_members_array_p[member_to_remove] = 0;
    /** compensate number of unique members for the member that is currently removed */
    --nof_unique_members;
    /** need to do something only if there are members left, if
     *  removed all of the members from the calendar, it no
     *  longer has any meaning and the members there can stay
     *  whatever */
    if (nof_unique_members != 0)
    {
        /** get delta from ideal entries to members destribution */
        SHR_IF_ERR_EXIT(dnx_algo_chc_delta_from_ideal_destribution_get
                        (unit, nof_unique_members, members_array_p, wanted_members_array_p, nof_entries_in_calendar,
                         max_nof_members_in_calendar, delta_array_p, &nof_changes));
        /** equilize calendar */
        SHR_IF_ERR_EXIT(dnx_algo_chc_delta_equilize
                        (unit, nof_changes, max_nof_members_in_calendar, nof_entries_in_calendar,
                         wanted_members_array_p, delta_array_p, new_calendar));
    }
exit:
    SHR_FREE(members_array_p);
    SHR_FREE(wanted_members_array_p);
    SHR_FREE(delta_array_p);
    SHR_FUNC_EXIT;
}

/** } */
