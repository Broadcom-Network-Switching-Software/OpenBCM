/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ft_user.h
 * Purpose:     Function declarations for flowtracker user entry management.
 */

#ifndef _BCM_INT_FT_USER_H_
#define _BCM_INT_FT_USER_H_

#include <soc/defs.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)
#include <bcm_int/esw/flowtracker/ft_group.h>

/*|*****************************************************|
 *|*****************************************************|
 *|           START  -  Software Insert Methods.        |
 *|*****************************************************|
 *|*****************************************************|
 */

extern int
bcmi_ft_user_init(int unit);

extern void
bcmi_ft_user_cleanup(int unit);

/*
 * Function:
 *      bcmi_ftk_user_entry_add
 * Purpose:
 *      Add a user flow entry basis user input key elements. API
 *      expects that all tracking parametrs of type = 'KEY' in the
 *      group are specified.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 *      options - (IN) Options for creting the flowtracker user entry.
 *      num_user_entry_params - (IN) Number of user entry params.
 *      user_entry_param_list - (IN) List of user entry params to add flow entry.
 *      entry_handle - (OUT) User entry handle corresponding to the entry added.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
extern int
bcmi_ftk_user_entry_add(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    uint32 options,
    int num_user_entry_params,
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list,
    bcm_flowtracker_user_entry_handle_t *entry_handle);

/*
 * Function:
 *      bcmi_ftk_user_entry_get
 * Purpose:
 *      Fetch user entry info given the entry handle.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry_handle - (IN) Flowtracker user entry handle.
 *      num_user_entry_params - (IN) Number of user entry params.
 *      user_entry_param_list - (IN/OUT) List of user entry params added in the flow entry.
 *      actual_user_entry_params - (OUT) Actual number of params in the user entry.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
extern int
bcmi_ftk_user_entry_get(
    int unit,
    bcm_flowtracker_user_entry_handle_t *entry_handle,
    int num_user_entry_params,
    bcm_flowtracker_tracking_param_info_t *user_entry_param_list,
    int *actual_user_entry_params);

/*
 * Function:
 *      bcmi_ftk_user_entry_get_all
 * Purpose:
 *      Fetch all user entries added in a against a given flow group.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 *      num_user_entry_handles - (IN) Number of export elements intended in the tempate.
 *      user_entry_handle_list - (IN/OUT) List of user entry handles corresponding to the user entries added.
 *      actual_user_entry_handles - (OUT) Actual number of user entry handles corresponding to the entries added.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
extern int
bcmi_ftk_user_entry_get_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int num_user_entry_handles,
    bcm_flowtracker_user_entry_handle_t *user_entry_handle_list,
    int *actual_user_entry_handles);

/*
 * Function:
 *      bcmi_ftk_user_entry_delete
 * Purpose:
 *      Delete user flow entry that is added earlier.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 *      entry_handle - (IN) User entry handle corresponding to the entry added.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
extern int
bcmi_ftk_user_entry_delete(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_user_entry_handle_t *entry_handle);

/*
 * Function:
 *      bcmi_ftk_user_entry_delete_all
 * Purpose:
 *      Delete user flow entry that is added earlier.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group ID.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
extern int
bcmi_ftk_user_entry_delete_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id);

/*|*****************************************************|
 *|*****************************************************|
 *|           END  -  Software Insert Methods.          |
 *|*****************************************************|
 *|*****************************************************|
 */

#endif /* BCM_FLOWTRACKER_SUPPORT */

#endif /* _BCM_INT_FT_USER_H_ */


