/** 
 * \file algo/consistent_hashing/consistent_hashing_calendar.h
 *
 * header of consistent hashing calendar related 
 * functions 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_CONSISTENTHASHINGCALENDAR_H_INCLUDED
/*
 * { 
 */
#define _BCM_DNX_CONSISTENTHASHINGCALENDAR_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/consistent_hashing/consistent_hashing_manager.h>

/**
 * \brief - calculate a calendar from scratch. if calendr is not 
 *        full the first current_nof_members_in_calendar members
 *        are used. if in addition specific_members_array is not
 *        NULL, specific members are taken from there.
 * 
 * \param [in] unit - unit number
 * \param [in] max_nof_members_in_calendar - max number of 
 *        members in calendar as defined in profile associated
 *        to this calendar
 * \param [in] nof_entries_in_calendar - number of entries in 
 *        the calendar
 * \param [in] current_nof_members_in_calendar - current number 
 *        of members in the calendar - the number of members the
 *        calendar will be initialized with!
 * \param [in] specific_members_array - optional - if not NULL, 
 *        specifys that only specific members are found, it is
 *        expected that the number of members mentioned is equal
 *        to current_nof_members_in_calendar. if this it is
 *        NULL, the members taken are the first ones
 * \param [out] calendar - returned populated calendar
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_consistent_hashing_calendar_calculate(
    int unit,
    uint32 max_nof_members_in_calendar,
    uint32 nof_entries_in_calendar,
    uint32 current_nof_members_in_calendar,
    uint32 *specific_members_array,
    consistent_hashing_calendar_t * calendar);

/**
 * \brief - add a member to calendar. returns the new calendar. 
 *        it is expected that the member to add will not be part
 *        of the calendar. it is expected that member to add is
 *        smaller than max_nof_members_in_calendar.
 * 
 * \param [in] unit - unit number
 * \param [in] current_calendar - current calendar before 
 *        addition
 * \param [in] member_to_add - the member to add to calendar
 * \param [in] max_nof_members_in_calendar - max number of 
 *        members in calendar as defined in its associated
 *        profile.
 * \param [in] nof_entries_in_calendar - number of entries in 
 *        the calendar
 * \param [in] first_member_in_calendar - indication that this 
 *        is the first member to add to this calendar
 * \param [out] new_calendar - returned calendar after adding 
 *        the member
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_consistent_hashing_calendar_member_add(
    int unit,
    consistent_hashing_calendar_t * current_calendar,
    uint32 member_to_add,
    uint32 max_nof_members_in_calendar,
    uint32 nof_entries_in_calendar,
    uint8 first_member_in_calendar,
    consistent_hashing_calendar_t * new_calendar);

/**
 * \brief - remove a member from calendar. returns the new 
 *        calendar. it is expected that the removed member will
 *        be part of the calendar.
 * 
 * \param [in] unit - unit number
 * \param [in] current_calendar - current calendar before 
 *        removal
 * \param [in] member_to_remove - the member to remove from the 
 *        calendar
 * \param [in] max_nof_members_in_calendar - max number of 
 *        members in calendar as defined in its associated
 *        profile
 * \param [in] nof_entries_in_calendar - number of entries in 
 *        the calendar
 * \param [in] new_calendar - returned calendar after removing 
 *        the member
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_consistent_hashing_calendar_member_remove(
    int unit,
    consistent_hashing_calendar_t * current_calendar,
    uint32 member_to_remove,
    uint32 max_nof_members_in_calendar,
    uint32 nof_entries_in_calendar,
    consistent_hashing_calendar_t * new_calendar);

#endif /* _BCM_DNX_CONSISTENTHASHINGCALENDAR_H_INCLUDED */
