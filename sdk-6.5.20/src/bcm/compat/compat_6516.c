/*
 * $Id: compat_6516.c,v 1.0 2019/04/03
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.16 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <bcm/flowtracker.h>
#include <bcm/l2.h>
#include <bcm/l2gre.h>
#include <bcm/vxlan.h>
#include <bcm/wlan.h>
#include <bcm/mirror.h>
#include <bcm/qos.h>
#include <bcm/nat.h>
#include <bcm_int/compat/compat_6516.h>
#include <bcm_int/compat/compat_6518.h>

/*
 * Function:
 *   _bcm_compat6516in_flowtracker_collector_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_collector_info_t datatype from <=6.5.16 to
 *   SDK 6.5.16+
 * Parameters:
 *   from        - (IN) The <=6.5.16 version of the datatype
 *   to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516in_flowtracker_collector_info_t(
        bcm_compat6516_flowtracker_collector_info_t *from,
        bcm_flowtracker_collector_info_t *to)
{
    bcm_flowtracker_collector_info_t_init(to);
    sal_memcpy(&to->collector_type, &from->collector_type, sizeof(bcm_flowtracker_collector_type_t));
    sal_memcpy(&to->transport_type, &from->transport_type, sizeof(bcm_flowtracker_collector_transport_type_t));
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_flowtracker_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_flowtracker_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_flowtracker_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_flowtracker_collector_udp_header_t));
    to->max_packet_length = from->max_packet_length;
}

/*
 * Function:
 *   _bcm_compat6516out_flowtracker_collector_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_collector_info_t datatype from 6.5.16+ to
 *   <=6.5.16
 * Parameters:
 *   from     - (IN) The 6.5.16+ version of the datatype
 *   to       - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516out_flowtracker_collector_info_t(
        bcm_flowtracker_collector_info_t *from,
        bcm_compat6516_flowtracker_collector_info_t *to)
{
    sal_memcpy(&to->collector_type, &from->collector_type, sizeof(bcm_flowtracker_collector_type_t));
    sal_memcpy(&to->transport_type, &from->transport_type, sizeof(bcm_flowtracker_collector_transport_type_t));
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_flowtracker_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_flowtracker_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_flowtracker_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_flowtracker_collector_udp_header_t));
    to->max_packet_length = from->max_packet_length;
}

/*
 * Function:
 *   bcm_compat6516_flowtracker_collector_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_collector_create
 * Parameters:
 *   unit    - (IN) BCM device number.
 *   options - (IN) Collector create options
 *   id      - (INOUT) Collector Id.
 *   info    - (IN) Collector info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_flowtracker_collector_create(
        int unit,
        uint32 options,
        bcm_flowtracker_collector_t *collector_id,
        bcm_compat6516_flowtracker_collector_info_t *info)
{
    int rv = 0;
    bcm_flowtracker_collector_info_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flowtracker_collector_info_t*)
           sal_alloc(sizeof(bcm_flowtracker_collector_info_t), "New flowtracker collector info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_flowtracker_collector_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_collector_create(unit, options, collector_id, new_info);

    if (rv >= 0) {
        _bcm_compat6516out_flowtracker_collector_info_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_flowtracker_collector_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_collector_get
 * Parameters:
 *   unit    - (IN) BCM device number.
 *   id      - (IN) Collector Id.
 *   info    - (OUT) Collector info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_flowtracker_collector_get(
        int unit,
        bcm_flowtracker_collector_t collector_id,
        bcm_compat6516_flowtracker_collector_info_t *info)
{
    int rv = 0;
    bcm_flowtracker_collector_info_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flowtracker_collector_info_t*)
            sal_alloc(sizeof(bcm_flowtracker_collector_info_t), "New flowtracker collector info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_flowtracker_collector_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_collector_get(unit, collector_id, new_info);

    if (rv >= 0) {
        _bcm_compat6516out_flowtracker_collector_info_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6516in_flowtracker_check_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_info_t datatype from <=6.5.16 to
 *   SDK 6.5.16+
 * Parameters:
 *   from        - (IN) The <=6.5.16 version of the datatype
 *   to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516in_flowtracker_check_info_t(
        bcm_compat6516_flowtracker_check_info_t *from,
        bcm_flowtracker_check_info_t *to)
{
    bcm_flowtracker_check_info_t_init(to);
    to->flags = from->flags;
    to->param = from->param;
    to->min_value = from->min_value;
    to->max_value = from->max_value;
    to->operation = from->operation;
    to->primary_check_id = from->primary_check_id;
}

/*
 * Function:
 *   _bcm_compat6516out_flowtracker_check_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_info_t datatype from 6.5.16+ to
 *   <=6.5.16
 * Parameters:
 *   from        - (IN) The 6.5.16+ version of the datatype
 *   to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516out_flowtracker_check_info_t(
        bcm_flowtracker_check_info_t *from,
        bcm_compat6516_flowtracker_check_info_t *to)
{
    to->flags = from->flags;
    to->param = from->param;
    to->min_value = from->min_value;
    to->max_value = from->max_value;
    to->operation = from->operation;
    to->primary_check_id = from->primary_check_id;
}

/*
 * Function:
 *   bcm_compat6516_flowtracker_check_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_create
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   options    - (IN) Options to create check.
 *   check_info - (IN) check info.
 *   check_id   - (INOUT) Software id of check.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_flowtracker_check_create(
        int unit,
        uint32 options,
        bcm_compat6516_flowtracker_check_info_t check_info,
        bcm_flowtracker_check_t *check_id)
{
    int rv = 0;
    bcm_flowtracker_check_info_t new_check_info;

    _bcm_compat6516in_flowtracker_check_info_t(&check_info, &new_check_info);

    rv = bcm_flowtracker_check_create(unit, options, new_check_info, check_id);

    if (rv >= 0) {
        _bcm_compat6516out_flowtracker_check_info_t(&new_check_info, &check_info);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6516_flowtracker_check_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_get
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   check_id   - (IN) Software id of check.
 *   check_info - (OUT) check info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_flowtracker_check_get(
        int unit,
        bcm_flowtracker_check_t check_id,
        bcm_compat6516_flowtracker_check_info_t *check_info)
{
    int rv = 0;
    bcm_flowtracker_check_info_t *new_check_info = NULL;

    if (check_info != NULL) {
        new_check_info = (bcm_flowtracker_check_info_t*)
            sal_alloc(sizeof(bcm_flowtracker_check_info_t), "New check info");
        if (new_check_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_flowtracker_check_info_t(check_info, new_check_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_check_get(unit, check_id, new_check_info);

    if (rv >= 0) {
        _bcm_compat6516out_flowtracker_check_info_t(new_check_info, check_info);
    }

    sal_free(new_check_info);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6516in_flowtracker_check_action_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_action_info_t datatype from <=6.5.16 to
 *   SDK 6.5.16+
 * Parameters:
 *   from        - (IN) The <=6.5.16 version of the datatype
 *   to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516in_flowtracker_check_action_info_t(
        bcm_compat6516_flowtracker_check_action_info_t *from,
        bcm_flowtracker_check_action_info_t *to)
{
    bcm_flowtracker_check_action_info_t_init(to);
    to->flags = from->flags;
    to->param = from->param;
    to->action = from->action;
}

/*
 * Function:
 *   _bcm_compat6516out_flowtracker_check_action_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_check_action_info_t datatype from 6.5.16+ to
 *   <=6.5.16
 * Parameters:
 *   from        - (IN) The 6.5.16+ version of the datatype
 *   to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516out_flowtracker_check_action_info_t(
        bcm_flowtracker_check_action_info_t *from,
        bcm_compat6516_flowtracker_check_action_info_t *to)
{
    to->flags = from->flags;
    to->param = from->param;
    to->action = from->action;
}

/*
 * Function:
 *   bcm_compat6516_flowtracker_check_action_info_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_action_info_set
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   check_id   - (IN) Software id of check.
 *   info       - (IN) check action info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_flowtracker_check_action_info_set(
        int unit,
        bcm_flowtracker_check_t check_id,
        bcm_compat6516_flowtracker_check_action_info_t info)
{
    int rv = 0;
    bcm_flowtracker_check_action_info_t new_info;

    _bcm_compat6516in_flowtracker_check_action_info_t(&info, &new_info);

    rv = bcm_flowtracker_check_action_info_set(unit, check_id, new_info);

    if (rv >= 0) {
        _bcm_compat6516out_flowtracker_check_action_info_t(&new_info, &info);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6516_flowtracker_check_action_info_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_check_action_info_get
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   check_id   - (IN) Software id of check.
 *   info       - (OUT) check action info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_flowtracker_check_action_info_get(
        int unit,
        bcm_flowtracker_check_t check_id,
        bcm_compat6516_flowtracker_check_action_info_t *info)
{
    int rv = 0;
    bcm_flowtracker_check_action_info_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flowtracker_check_action_info_t*)
            sal_alloc(sizeof(bcm_flowtracker_check_action_info_t), "New check action info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_flowtracker_check_action_info_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_check_action_info_get(unit, check_id, new_info);

    if (rv >= 0) {
        _bcm_compat6516out_flowtracker_check_action_info_t(new_info, info);
    }

    sal_free(new_info);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6516in_flowtracker_tracking_param_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_tracking_param_info_t datatype from <=6.5.16 to
 *   SDK 6.5.16+
 * Parameters:
 *   from        - (IN) The <=6.5.16 version of the datatype
 *   to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516in_flowtracker_tracking_param_info_t(
        bcm_compat6516_flowtracker_tracking_param_info_t *from,
        bcm_flowtracker_tracking_param_info_t *to)
{
    bcm_flowtracker_tracking_param_info_t_init(to);
    to->flags = from->flags;
    to->param = from->param;
    sal_memcpy(&to->tracking_data, &from->tracking_data, sizeof(bcm_flowtracker_tracking_param_user_data_t));
    sal_memcpy(&to->mask, &from->mask, sizeof(bcm_flowtracker_tracking_param_mask_t));
    to->check_id = from->check_id;
    to->udf_id = from->udf_id;
}

/*
 * Function:
 *   _bcm_compat6516out_flowtracker_tracking_param_info_t
 * Purpose:
 *   Convert the bcm_flowtracker_tracking_param_info_t datatype from 6.5.16+ to
 *   <=6.5.16+
 * Parameters:
 *   from        - (IN) The 6.5.16+ version of the datatype
 *   to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516out_flowtracker_tracking_param_info_t(
        bcm_flowtracker_tracking_param_info_t *from,
        bcm_compat6516_flowtracker_tracking_param_info_t *to)
{
    to->flags = from->flags;
    to->param = from->param;
    sal_memcpy(&to->tracking_data, &from->tracking_data, sizeof(bcm_flowtracker_tracking_param_user_data_t));
    sal_memcpy(&to->mask, &from->mask, sizeof(bcm_flowtracker_tracking_param_mask_t));
    to->check_id = from->check_id;
    to->udf_id = from->udf_id;
}

/*
 * Function:
 *   bcm_compat6516_flowtracker_group_tracking_params_set
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_tracking_params_set
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   id                      - (IN) Flowtracker group Id.
 *   num_tracking_params     - (IN) Number of tracking params.
 *   list_of_tracking_params - (IN) List of tracking params.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_flowtracker_group_tracking_params_set(
        int unit,
        bcm_flowtracker_group_t id,
        int num_tracking_params,
        bcm_compat6516_flowtracker_tracking_param_info_t *list_of_tracking_params)
{
    int rv = 0;
    int iter = 0;
    bcm_flowtracker_tracking_param_info_t *new_list_of_tracking_params = NULL;

    if ((list_of_tracking_params != NULL) && (num_tracking_params > 0)) {
        new_list_of_tracking_params = (bcm_flowtracker_tracking_param_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                              num_tracking_params), "New tracking param info");
        if (new_list_of_tracking_params == NULL) {
            return BCM_E_MEMORY;
        }

        for (iter = 0; iter < num_tracking_params; iter++) {
            _bcm_compat6516in_flowtracker_tracking_param_info_t
                                        (&list_of_tracking_params[iter],
                                         &new_list_of_tracking_params[iter]);
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_group_tracking_params_set(unit, id,
                                num_tracking_params, new_list_of_tracking_params);

    sal_free(new_list_of_tracking_params);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_flowtracker_group_tracking_params_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_tracking_params_get
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   id                      - (IN) Flowtracker group Id.
 *   max_size                - (IN) Maximum Size of tracking param list.
 *   list_of_tracking_params - (OUT) List of tracking params.
 *   list_size               - (OUT) Number of tracking params actually filled.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_flowtracker_group_tracking_params_get(
        int unit,
        bcm_flowtracker_group_t id,
        int max_size,
        bcm_compat6516_flowtracker_tracking_param_info_t *list_of_tracking_params,
        int *list_size)
{
    int rv = 0;
    int iter = 0;
    bcm_flowtracker_tracking_param_info_t *new_list_of_tracking_params = NULL;

    if ((list_of_tracking_params != NULL) && (list_size != NULL) && (max_size > 0)) {
        new_list_of_tracking_params = (bcm_flowtracker_tracking_param_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                                        max_size), "New tracking param info");
        if (new_list_of_tracking_params == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_group_tracking_params_get(unit, id, max_size,
                                new_list_of_tracking_params, list_size);

    if (rv >= 0) {
        for (iter = 0; iter < max_size; iter++) {
            _bcm_compat6516out_flowtracker_tracking_param_info_t
                                    (&new_list_of_tracking_params[iter],
                                     &list_of_tracking_params[iter]);
        }
    }

    sal_free(new_list_of_tracking_params);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_flowtracker_user_entry_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_user_entry_add
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   flow_group_id           - (IN) Flowtracker group Id.
 *   options                 - (IN) Options to create user entry.
 *   num_user_entry_params   - (IN) Number of user entry params.
 *   user_entry_param_list   - (IN) List of user entry params.
 *   entry_handle            - (IN) User entry handle.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_flowtracker_user_entry_add(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        uint32 options,
        int num_user_entry_params,
        bcm_compat6516_flowtracker_tracking_param_info_t *user_entry_param_list,
        bcm_flowtracker_user_entry_handle_t *entry_handle)
{
    int rv = 0;
    int iter = 0;
    bcm_flowtracker_tracking_param_info_t *new_user_entry_param_list = NULL;

    if ((user_entry_param_list != NULL) &&
        (entry_handle != NULL) && (num_user_entry_params > 0)) {
        new_user_entry_param_list =  (bcm_flowtracker_tracking_param_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                            num_user_entry_params), "New tracking param info");
        if (new_user_entry_param_list == NULL) {
            return BCM_E_MEMORY;
        }

        for (iter = 0; iter < num_user_entry_params; iter++) {
            _bcm_compat6516in_flowtracker_tracking_param_info_t
                                        (&user_entry_param_list[iter],
                                         &new_user_entry_param_list[iter]);
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_user_entry_add(unit, flow_group_id, options,
                                        num_user_entry_params,
                                        new_user_entry_param_list,
                                        entry_handle);

    sal_free(new_user_entry_param_list);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_flowtracker_user_entry_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_user_entry_get
 * Parameters:
 *   unit                    - (IN) BCM device number.
 *   entry_handle            - (IN) User entry handle.
 *   num_user_entry_params   - (IN) Maximum Number of user entry params.
 *   user_entry_param_list   - (OUT) List of user entry params.
 *   actual_user_entry_params   - (OUT) Number of user entry params.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_flowtracker_user_entry_get(
        int unit,
        bcm_flowtracker_user_entry_handle_t *entry_handle,
        int num_user_entry_params,
        bcm_compat6516_flowtracker_tracking_param_info_t *user_entry_param_list,
        int *actual_user_entry_params)
{
    int rv = 0;
    int iter = 0;
    bcm_flowtracker_tracking_param_info_t *new_user_entry_param_list = NULL;

    if ((user_entry_param_list != NULL) &&
        (actual_user_entry_params != NULL) && (num_user_entry_params > 0)) {
        new_user_entry_param_list =  (bcm_flowtracker_tracking_param_info_t*)
            sal_alloc((sizeof(bcm_flowtracker_tracking_param_info_t) *
                        num_user_entry_params), "New tracking param info");
        if (new_user_entry_param_list == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_flowtracker_user_entry_get(unit, entry_handle, num_user_entry_params,
                                        new_user_entry_param_list,
                                        actual_user_entry_params);

    if (rv >= 0) {
        for (iter = 0; iter < num_user_entry_params; iter++) {
            _bcm_compat6516out_flowtracker_tracking_param_info_t
                                         (&new_user_entry_param_list[iter],
                                          &user_entry_param_list[iter]);
        }
    }

    sal_free(new_user_entry_param_list);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6516in_collector_info_t
 * Purpose:
 *   Convert the bcm_collector_info_t datatype from <=6.5.16 to
 *   SDK 6.5.16+
 * Parameters:
 *   from        - (IN) The <=6.5.16 version of the datatype
 *   to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516in_collector_info_t(
        bcm_compat6516_collector_info_t *from,
        bcm_collector_info_t *to)
{
    bcm_collector_info_t_init(to);
    sal_memcpy(&to->collector_type, &from->collector_type, sizeof(bcm_collector_type_t));
    sal_memcpy(&to->transport_type, &from->transport_type, sizeof(bcm_collector_transport_type_t));
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_collector_udp_header_t));
    sal_memcpy(&to->ipfix, &from->ipfix, sizeof(bcm_collector_ipfix_header_t));
    sal_memcpy(&to->protobuf, &from->protobuf, sizeof(bcm_collector_protobuf_header_t));
}

/*
 * Function:
 *   _bcm_compat6516out_collector_info_t
 * Purpose:
 *   Convert the bcm_collector_info_t datatype from 6.5.16+ to
 *   <=6.5.16
 * Parameters:
 *   from     - (IN) The 6.5.16+ version of the datatype
 *   to       - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516out_collector_info_t(
        bcm_collector_info_t *from,
        bcm_compat6516_collector_info_t *to)
{
    sal_memcpy(&to->collector_type, &from->collector_type, sizeof(bcm_collector_type_t));
    sal_memcpy(&to->transport_type, &from->transport_type, sizeof(bcm_collector_transport_type_t));
    sal_memcpy(&to->eth, &from->eth, sizeof(bcm_collector_eth_header_t));
    sal_memcpy(&to->ipv4, &from->ipv4, sizeof(bcm_collector_ipv4_header_t));
    sal_memcpy(&to->ipv6, &from->ipv6, sizeof(bcm_collector_ipv6_header_t));
    sal_memcpy(&to->udp, &from->udp, sizeof(bcm_collector_udp_header_t));
    sal_memcpy(&to->ipfix, &from->ipfix, sizeof(bcm_collector_ipfix_header_t));
    sal_memcpy(&to->protobuf, &from->protobuf, sizeof(bcm_collector_protobuf_header_t));
}

/*
 * Function:
 *   bcm_compat6516_collector_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_collector_create
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   options        - (IN) Collector create options
 *   id             - (INOUT) Collector Id.
 *   collector_info - (IN) Collector info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_collector_create(
        int unit,
        uint32 options,
        bcm_collector_t *collector_id,
        bcm_compat6516_collector_info_t *collector_info)
{
    int rv = 0;
    bcm_collector_info_t *new_collector_info = NULL;

    if (collector_info != NULL) {
        new_collector_info = (bcm_collector_info_t*)
            sal_alloc(sizeof(bcm_collector_info_t), "New collector info");
        if (new_collector_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_collector_info_t(collector_info, new_collector_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_collector_create(unit, options, collector_id, new_collector_info);

    if (rv >= 0) {
        _bcm_compat6516out_collector_info_t(new_collector_info, collector_info);
    }

    sal_free(new_collector_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_collector_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_collector_get
 * Parameters:
 *   unit    - (IN) BCM device number.
 *   id      - (IN) Collector Id.
 *   info    - (OUT) Collector info.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_collector_get(
        int unit,
        bcm_collector_t collector_id,
        bcm_compat6516_collector_info_t *collector_info)
{
    int rv = 0;
    bcm_collector_info_t *new_collector_info = NULL;

    if (collector_info != NULL) {
        new_collector_info = (bcm_collector_info_t*)
            sal_alloc(sizeof(bcm_collector_info_t), "New collector info");
        if (new_collector_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_collector_info_t(collector_info, new_collector_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_collector_get(unit, collector_id, new_collector_info);

    if (rv >= 0) {
        _bcm_compat6516out_collector_info_t(new_collector_info, collector_info);
    }

    sal_free(new_collector_info);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6516in_l2_station_t
 * Purpose:
 *   Convert the bcm_l2_station_t datatype from <=6.5.16 to
 *   SDK 6.5.16+
 * Parameters:
 *   from        - (IN) The <=6.5.16 version of the datatype
 *   to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516in_l2_station_t(
        bcm_compat6516_l2_station_t *from,
        bcm_l2_station_t *to)
{
    bcm_l2_station_t_init(to);
    to->flags = from->flags;
    to->priority = from->priority;
    sal_memcpy(to->dst_mac, from->dst_mac_mask, sizeof(bcm_mac_t));
    sal_memcpy(to->dst_mac_mask, from->dst_mac_mask, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->vlan_mask = from->vlan_mask;
    to->src_port = from->src_port;
    to->src_port_mask = from->src_port_mask;
    to->taf_gate_primap = from->taf_gate_primap;
}

/*
 * Function:
 *   _bcm_compat6516out_l2_station_t
 * Purpose:
 *   Convert the bcm_l2_station_t datatype from 6.5.16+ to
 *   <=6.5.16
 * Parameters:
 *   from     - (IN) The 6.5.16+ version of the datatype
 *   to       - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516out_l2_station_t(
        bcm_l2_station_t *from,
        bcm_compat6516_l2_station_t *to)
{
    to->flags = from->flags;
    to->priority = from->priority;
    sal_memcpy(to->dst_mac, from->dst_mac_mask, sizeof(bcm_mac_t));
    sal_memcpy(to->dst_mac_mask, from->dst_mac_mask, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->vlan_mask = from->vlan_mask;
    to->src_port = from->src_port;
    to->src_port_mask = from->src_port_mask;
    to->taf_gate_primap = from->taf_gate_primap;
}

/*
 * Function:
 *   bcm_compat6516_l2_station_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_l2_station_add
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   id             - (INOUT) Station Id.
 *   station        - (IN) Pointer to station address information.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_l2_station_add(
        int unit,
        int *station_id,
        bcm_compat6516_l2_station_t *station)
{
    int rv = 0;
    bcm_l2_station_t *new_station = NULL;

    if (station != NULL) {
        new_station = (bcm_l2_station_t*)
            sal_alloc(sizeof(bcm_l2_station_t), "New l2 station");
        if (new_station == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_l2_station_t(station, new_station);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l2_station_add(unit, station_id, new_station);

    if (rv >= 0) {
        _bcm_compat6516out_l2_station_t(new_station, station);
    }

    sal_free(new_station);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_l2_station_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_l2_station_get
 * Parameters:
 *   unit         - (IN) BCM device number
 *   station_id   - (IN) Station ID
 *   station      - (OUT) Pointer to station address information.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_l2_station_get(
        int unit,
        int station_id,
        bcm_compat6516_l2_station_t *station)
{
    int rv = 0;
    bcm_l2_station_t *new_station = NULL;

    if (station != NULL) {
        new_station = (bcm_l2_station_t*)
            sal_alloc(sizeof(bcm_l2_station_t), "New l2 station");
        if (new_station == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_l2_station_t(station, new_station);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l2_station_get(unit, station_id, new_station);

    if (rv >= 0) {
        _bcm_compat6516out_l2_station_t(new_station, station);
    }

    sal_free(new_station);
    return rv;
}

/*
 * Function:
 *  _bcm_compat6516in_l2_addr_t
 * Purpose:
 *  Convert the bcm_l2_addr_t datatype from <=6.5.16 to
 *  SDK 6.5.16+
 * Parameters:
 *  from        - (IN) The <=6.5.16 version of the datatype
 *  to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_l2_addr_t(bcm_compat6516_l2_addr_t *from,
                                         bcm_l2_addr_t *to)
{
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    sal_memcpy(to->mac, from->mac, sizeof(bcm_mac_t));
    to->vid = from->vid;
    to->port = from->port;
    to->modid = from->modid;
    to->tgid = from->tgid;
    to->cos_dst = from->cos_dst;
    to->cos_src = from->cos_src;
    to->l2mc_group = from->l2mc_group;
    to->egress_if = from->egress_if;
    to->l3mc_group = from->l3mc_group;
    to->block_bitmap = from->block_bitmap;
    to->auth = from->auth;
    to->group = from->group;
    to->distribution_class = from->distribution_class;
    to->encap_id = from->encap_id;
    to->age_state = from->age_state;
    to->flow_handle = from-> flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    sal_memcpy(to->logical_fields, from->logical_fields,
            BCM_FLOW_MAX_NOF_LOGICAL_FIELDS * sizeof(bcm_flow_logical_field_t));
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
}

/*
 * Function:
 *  _bcm_compat6516out_l2_addr_t
 * Purpose:
 *  Convert the bcm_l2_addr_t datatype from 6.5.16+ to
 *  <=6.5.16
 * Parameters:
 *  from        - (IN) The SDK 6.5.16+ version of the datatype
 *  to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *  Nothing
 */
STATIC void
_bcm_compat6516out_l2_addr_t(bcm_l2_addr_t *from,
                             bcm_compat6516_l2_addr_t *to)
{
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    sal_memcpy(to->mac, from->mac, sizeof(bcm_mac_t));
    to->vid = from->vid;
    to->port = from->port;
    to->modid = from->modid;
    to->tgid = from->tgid;
    to->cos_dst = from->cos_dst;
    to->cos_src = from->cos_src;
    to->l2mc_group = from->l2mc_group;
    to->egress_if = from->egress_if;
    to->l3mc_group = from->l3mc_group;
    to->block_bitmap = from->block_bitmap;
    to->auth = from->auth;
    to->group = from->group;
    to->distribution_class = from->distribution_class;
    to->encap_id = from->encap_id;
    to->age_state = from->age_state;
    to->flow_handle = from-> flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    sal_memcpy(to->logical_fields, from->logical_fields,
            BCM_FLOW_MAX_NOF_LOGICAL_FIELDS * sizeof(bcm_flow_logical_field_t));
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
}

/*
 * Function: bcm_compat6516_l2_addr_add
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_add.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat6516_l2_addr_add(int unit,
                           bcm_compat6516_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_add(unit, newl2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6516out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6516_l2_addr_multi_add
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_multi_add.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  count  - (IN) count of l2 address to add.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6516_l2_addr_multi_add(int unit,
                                 bcm_compat6516_l2_addr_t *l2addr,
                                 int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_add(unit, newl2addr,count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6516out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6516_l2_addr_multi_delete
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_multi_delete.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) L2 address entry with old format.
 *  count  - (IN) count of l2 address to delete.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6516_l2_addr_multi_delete(int unit,
                                    bcm_compat6516_l2_addr_t *l2addr,
                                    int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_delete(unit, newl2addr,count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6516out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6516_l2_addr_get
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_get.
 *
 * Parameters:
 *  unit     - (IN)  BCM device number.
 *  mac_addr - (IN)  Input mac address entry to search.
 *  vid      - (IN)  Input VLAN ID to search.
 *  l2addr   - (OUT) l2 address entry with old format.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6516_l2_addr_get(int unit,
                           bcm_mac_t mac_addr, bcm_vlan_t vid,
                           bcm_compat6516_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_get(unit, mac_addr, vid, newl2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6516out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6516_l2_conflict_get
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_conflict_get.
 *
 * Parameters:
 *  unit     - (IN)  BCM device number.
 *  addr     - (IN)  l2 address entry with old format.
 *  cf_array - (OUT) conflicting address array.
 *  cf_max   - (IN)  maximum conflicting address.
 *  cf_count - (OUT) actual count of conflicting address found.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6516_l2_conflict_get(int unit, bcm_compat6516_l2_addr_t *addr,
                               bcm_compat6516_l2_addr_t *cf_array, int cf_max,
                               int *cf_count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t naddr;
    bcm_l2_addr_t *ncf_array = NULL;
    int i = 0;

    if ((NULL != addr) && (NULL != cf_array) && (0 < cf_max)) {
        /* Create from heap to avoid the stack to bloat */
        ncf_array = (bcm_l2_addr_t *)
                     sal_alloc(cf_max * sizeof(bcm_l2_addr_t),
                    "compat6516_l2_conflict_get");
        if (NULL == ncf_array) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Transform the entry from the old format to new one */
    _bcm_compat6516in_l2_addr_t(addr, &naddr);

    /* Call the BCM API with new format */
    rv = bcm_l2_conflict_get(unit, &naddr, ncf_array, cf_max, cf_count);

    if (rv >= 0) {
        for(i = 0; i < *cf_count; i++) {
            _bcm_compat6516out_l2_addr_t(&ncf_array[i], &cf_array[i]);
        }
    }

    /* Deallocate memory*/
    sal_free(ncf_array);
    return rv;
}

/*
 * Function: bcm_compat6516_l2_replace
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_replace.
 *
 * Parameters:
 *  unit       - (IN) BCM device number.
 *  flags      - (IN) L2 replace flags.
 *  match_addr - (IN) L2 address entry with old format.
 *  new_module - (IN) module id.
 *  new_port   - (IN) port.
 *  new_trunk  - (IN) trunk group id.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6516_l2_replace(int unit, uint32 flags,
                          bcm_compat6516_l2_addr_t *match_addr,
                          bcm_module_t new_module,
                          bcm_port_t new_port, bcm_trunk_t new_trunk)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newmatch_addr = NULL;

    if (NULL != match_addr) {
        /* Create from heap to avoid the stack to bloat */
        newmatch_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newmatch_addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l2_addr_t(match_addr, newmatch_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace(unit, flags, newmatch_addr, new_module, new_port, new_trunk);

    /* Transform the entry from the new format to old one */
    _bcm_compat6516out_l2_addr_t(newmatch_addr, match_addr);

    /* Deallocate memory*/
    sal_free(newmatch_addr);

    return rv;
}

/*
 * Function: bcm_compat6516_l2_replace_match
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_replace_match.
 *
 * Parameters:
 *  unit              - (IN) BCM device number.
 *  match_addr        - (IN) l2 address entry with old format.
 *  mask_addr         - (IN) mask l2 address entry with old format.
 *  replace_addr      - (IN) replace l2 address entry in old format.
 *  replace_mask_addr - (IN) replace l2 mask address entry in old format.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6516_l2_replace_match(int unit, uint32 flags,
                                bcm_compat6516_l2_addr_t *match_addr,
                                bcm_compat6516_l2_addr_t *mask_addr,
                                bcm_compat6516_l2_addr_t *replace_addr,
                                bcm_compat6516_l2_addr_t *replace_mask_addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newmatch_addr = NULL;
    bcm_l2_addr_t *newmask_addr = NULL;
    bcm_l2_addr_t *newreplace_addr = NULL;
    bcm_l2_addr_t *newreplace_mask_addr = NULL;

    if ((NULL != match_addr) && (NULL != mask_addr) &&
            (NULL != replace_addr) && (NULL != replace_mask_addr)) {
        /* Create from heap to avoid the stack to bloat */
        newmatch_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newmatch_addr) {
            return BCM_E_MEMORY;
        }

        newmask_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr mask");
        if (NULL == newmask_addr) {
            sal_free(newmatch_addr);
            return BCM_E_MEMORY;
        }

        newreplace_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "Replace l2 addr");
        if (NULL == newreplace_addr) {
            sal_free(newmatch_addr);
            sal_free(newmask_addr);
            return BCM_E_MEMORY;
        }

        newreplace_mask_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "Replace l2 addr mask");
        if (NULL == newreplace_mask_addr) {
            sal_free(newmatch_addr);
            sal_free(newmask_addr);
            sal_free(newreplace_addr);
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l2_addr_t(match_addr, newmatch_addr);
        _bcm_compat6516in_l2_addr_t(mask_addr, newmask_addr);
        _bcm_compat6516in_l2_addr_t(replace_addr, newreplace_addr);
        _bcm_compat6516in_l2_addr_t(replace_mask_addr, newreplace_mask_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace_match(unit, flags, newmatch_addr, newmask_addr,
            newreplace_addr, newreplace_mask_addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6516out_l2_addr_t(newmatch_addr, match_addr);
    _bcm_compat6516out_l2_addr_t(newmask_addr, mask_addr);
    _bcm_compat6516out_l2_addr_t(newreplace_addr, replace_addr);
    _bcm_compat6516out_l2_addr_t(newreplace_mask_addr, replace_mask_addr);

    /* Deallocate memory*/
    sal_free(newmatch_addr);
    sal_free(newmask_addr);
    sal_free(newreplace_addr);
    sal_free(newreplace_mask_addr);

    return rv;
}

/*
 * Function: bcm_compat6516_l2_stat_get
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_get.
 *
 * Parameters:
 *  unit   - (IN)  BCM device number.
 *  l2addr - (IN)  l2 address entry with old format.
 *  stat   - (IN)  stat to collect.
 *  val    - (OUT) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6516_l2_stat_get(int unit,
                           bcm_compat6516_l2_addr_t *l2addr,
                           bcm_l2_stat_t stat, uint64 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6516out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6516_l2_stat_get32
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_get32.
 *
 * Parameters:
 *  unit   - (IN)  BCM device number.
 *  l2addr - (IN)  l2 address entry with old format.
 *  stat   - (IN)  stat to collect.
 *  val    - (OUT) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6516_l2_stat_get32(int unit,
                             bcm_compat6516_l2_addr_t *l2addr,
                             bcm_l2_stat_t stat, uint32 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get32(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6516out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6516_l2_stat_set
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_set.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  stat   - (IN) stat to set.
 *  val    - (IN) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6516_l2_stat_set(int unit,
                           bcm_compat6516_l2_addr_t *l2addr,
                           bcm_l2_stat_t stat, uint64 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6516out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6516_l2_stat_set32
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_set32.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  stat   - (IN) stat to set.
 *  val    - (IN) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6516_l2_stat_set32(int unit,
                             bcm_compat6516_l2_addr_t *l2addr,
                             bcm_l2_stat_t stat, uint32 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set32(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6516out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6516_l2_stat_enable_set
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_enable_set.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  enable - (IN) enable value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6516_l2_stat_enable_set(int unit,
                                  bcm_compat6516_l2_addr_t *l2addr,
                                  int enable)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_enable_set(unit, newl2addr, enable);

    /* Transform the entry from the new format to old one */
    _bcm_compat6516out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_trunk_info_t
 * Purpose:
 *      Convert the bcm_compat6516_trunk_info_t datatype from <=6.5.16 to
 *      SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_trunk_info_t(bcm_compat6516_trunk_info_t *from,
                           bcm_trunk_info_t *to)
{
    bcm_trunk_info_t_init(to);
    to->flags = from->flags;
    to->psc = from->psc;
    to->psc_info = from->psc_info;
    to->ipmc_psc = from->ipmc_psc;
    to->dlf_index = from->dlf_index;
    to->mc_index = from->mc_index;
    to->ipmc_index = from->ipmc_index;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    to->master_tid = from->master_tid;
 }

/*
 * Function:
 *      _bcm_compat6516out_trunk_info_t
 * Purpose:
 *      Convert the bcm_trunk_info_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_trunk_info_t(bcm_trunk_info_t *from,
                           bcm_compat6516_trunk_info_t *to)
{
    to->flags = from->flags;
    to->psc = from->psc;
    to->psc_info = from->psc_info;
    to->ipmc_psc = from->ipmc_psc;
    to->dlf_index = from->dlf_index;
    to->mc_index = from->mc_index;
    to->ipmc_index = from->ipmc_index;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    to->master_tid = from->master_tid;
 }

/*
 * Function: bcm_compat6516_trunk_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_trunk_get.
 *
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      tid    - Trunk ID.
 *      trunk_info   - (OUT) Place to store returned trunk info.
 *      member_max   - (IN) Size of member_array.
 *      member_array - (OUT) Place to store returned trunk members.
 *      member_count - (OUT) Place to store returned number of trunk members.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6516_trunk_get(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6516_trunk_info_t *trunk_info,
    int member_max,
    bcm_trunk_member_t *member_array,
    int *member_count)
{
    int rv;
    bcm_trunk_info_t *new_info = NULL;

    if (NULL != trunk_info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_trunk_info_t *)
            sal_alloc(sizeof(bcm_trunk_info_t), "New trunk info");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_trunk_info_t(trunk_info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_get(unit, tid, new_info, member_max, member_array, member_count);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_trunk_info_t(new_info, trunk_info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function: bcm_compat6516_trunk_set
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_trunk_set.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID to be affected.
 *      trunk_info - Information on the trunk group.
 *      member_count - Number of trunk members.
 *      member_array - Array of trunk members.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6516_trunk_set(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6516_trunk_info_t *trunk_info,
    int member_count,
    bcm_trunk_member_t *member_array)
{
    int rv;
    bcm_trunk_info_t *new_info = NULL;

    if (NULL != trunk_info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_trunk_info_t *)
            sal_alloc(sizeof(bcm_trunk_info_t), "New trunk info");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_trunk_info_t(trunk_info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_set(unit, tid, new_info, member_count, member_array);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_trunk_info_t(new_info, trunk_info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_ecn_traffic_map_info_t
 * Purpose:
 *      Convert the bcm_compat6516_ecn_traffic_map_info_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_ecn_traffic_map_info_t(
    bcm_compat6516_ecn_traffic_map_info_t *from,
    bcm_ecn_traffic_map_info_t *to)
{
    bcm_ecn_traffic_map_info_t_init(to);

    to->flags = from->flags;
    to->ecn = from->ecn;
    to->int_cn = from->int_cn;
}

/*
 * Function:
 *      _bcm_compat6516out_ecn_traffic_map_info_t
 * Purpose:
 *      Convert the bcm_ecn_traffic_map_info_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_ecn_traffic_map_info_t(
    bcm_ecn_traffic_map_info_t *from,
    bcm_compat6516_ecn_traffic_map_info_t *to)
{
    to->flags = from->flags;
    to->ecn = from->ecn;
    to->int_cn = from->int_cn;
}

/*
 * Function: bcm_compat6516_ecn_traffic_map_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_ecn_traffic_map_get.
 *
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      map - (OUT) Place to store returned map.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6516_ecn_traffic_map_get(
    int unit,
    bcm_compat6516_ecn_traffic_map_info_t *map)
{
    int rv;
    bcm_ecn_traffic_map_info_t *new_map = NULL;

    if (NULL != map) {
        /* Create from heap to avoid the stack to bloat */
        new_map = (bcm_ecn_traffic_map_info_t *)
            sal_alloc(sizeof(bcm_ecn_traffic_map_info_t), "New ECN traffic map");
        if (NULL == new_map) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ecn_traffic_map_info_t(map, new_map);
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_traffic_map_get(unit, new_map);

    if (NULL != new_map) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ecn_traffic_map_info_t(new_map, map);
        /* Deallocate memory*/
        sal_free(new_map);
    }

    return rv;
}

/*
 * Function: bcm_compat6516_ecn_traffic_map_set
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_ecn_traffic_map_set.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      map - Information on ECN traffic map.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6516_ecn_traffic_map_set(
    int unit,
    bcm_compat6516_ecn_traffic_map_info_t *map)
{
    int rv;
    bcm_ecn_traffic_map_info_t *new_map = NULL;

    if (NULL != map) {
        /* Create from heap to avoid the stack to bloat */
        new_map = (bcm_ecn_traffic_map_info_t*)
            sal_alloc(sizeof(bcm_ecn_traffic_map_info_t), "New ECN traffic map");
        if (NULL == new_map) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ecn_traffic_map_info_t(map, new_map);
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_traffic_map_set(unit, new_map);

    if (NULL != new_map) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ecn_traffic_map_info_t(new_map, map);
        /* Deallocate memory*/
        sal_free(new_map);
    }

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6516in_tunnel_initiator_t
 * Purpose:
 *      Convert the _bcm_compat6516in_tunnel_initiator_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_tunnel_initiator_t(
    bcm_compat6516_tunnel_initiator_t *from,
    bcm_compat6517_tunnel_initiator_t *to)
{
    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    sal_memcpy(to->dmac, from->dmac, sizeof(bcm_mac_t));
    to->dip = from->dip;
    to->sip = from->sip;
    sal_memcpy(to->sip6, from->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6, from->dip6, sizeof(bcm_ip6_t));
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    sal_memcpy(to->smac, from->smac, sizeof(bcm_mac_t));
    to->mtu = from->mtu;
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->span_id = from->span_id;
    to->aux_data = from->aux_data;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->encap_access = from->encap_access;
    to->hw_id = from->hw_id;
    to->switch_id = from->switch_id;
    to->class_id = from->class_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model,sizeof(to->egress_qos_model));
    to->qos_map_id = from->qos_map_id;
}

/*
 * Function:
 *      _bcm_compat6516out_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_tunnel_initiator_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_tunnel_initiator_t(
    bcm_compat6517_tunnel_initiator_t *from,
    bcm_compat6516_tunnel_initiator_t *to)
{
    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    sal_memcpy(to->dmac, from->dmac, sizeof(bcm_mac_t));
    to->dip = from->dip;
    to->sip = from->sip;
    sal_memcpy(to->sip6, from->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6, from->dip6, sizeof(bcm_ip6_t));
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    sal_memcpy(to->smac, from->smac, sizeof(bcm_mac_t));
    to->mtu = from->mtu;
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->span_id = from->span_id;
    to->aux_data = from->aux_data;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->encap_access = from->encap_access;
    to->hw_id = from->hw_id;
    to->switch_id = from->switch_id;
    to->class_id = from->class_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model,sizeof(to->egress_qos_model));
    to->qos_map_id = from->qos_map_id;
    /* Ignore from->ecn. */
    /* Ignore from->flow_label_sel. */
}

int
bcm_compat6516_l2gre_tunnel_initiator_create(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info)
{
    int rv;
    bcm_compat6517_tunnel_initiator_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6517_tunnel_initiator_t*)
            sal_alloc(sizeof(bcm_compat6517_tunnel_initiator_t), "New tunnel initiator");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_tunnel_initiator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6517_l2gre_tunnel_initiator_create(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_tunnel_initiator_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_l2gre_tunnel_initiator_get(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info)
{
    int rv;
    bcm_compat6517_tunnel_initiator_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6517_tunnel_initiator_t*)
            sal_alloc(sizeof(bcm_compat6517_tunnel_initiator_t), "New tunnel initiator");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_tunnel_initiator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6517_l2gre_tunnel_initiator_get(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_tunnel_initiator_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_tunnel_initiator_set(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6516_tunnel_initiator_t *tunnel)
{
    int rv;
    bcm_compat6517_tunnel_initiator_t *new_info = NULL;

    if (NULL != tunnel) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6517_tunnel_initiator_t*)
            sal_alloc(sizeof(bcm_compat6517_tunnel_initiator_t), "New tunnel initiator");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_tunnel_initiator_t(tunnel, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6517_tunnel_initiator_set(unit, intf, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_tunnel_initiator_t(new_info, tunnel);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_tunnel_initiator_create(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6516_tunnel_initiator_t *tunnel)
{
    int rv;
    bcm_compat6517_tunnel_initiator_t *new_info = NULL;

    if (NULL != tunnel) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6517_tunnel_initiator_t*)
            sal_alloc(sizeof(bcm_compat6519_tunnel_initiator_t), "New tunnel initiator");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_tunnel_initiator_t(tunnel, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6517_tunnel_initiator_create(unit, intf, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_tunnel_initiator_t(new_info, tunnel);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_tunnel_initiator_get(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6516_tunnel_initiator_t *tunnel)
{
    int rv;
    bcm_compat6517_tunnel_initiator_t *new_info = NULL;

    if (NULL != tunnel) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6517_tunnel_initiator_t*)
            sal_alloc(sizeof(bcm_compat6517_tunnel_initiator_t), "New tunnel initiator");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_tunnel_initiator_t(tunnel, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6517_tunnel_initiator_get(unit, intf, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_tunnel_initiator_t(new_info, tunnel);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_vxlan_tunnel_initiator_create(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info)
{
    int rv;
    bcm_compat6517_tunnel_initiator_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6517_tunnel_initiator_t*)
            sal_alloc(sizeof(bcm_compat6517_tunnel_initiator_t), "New tunnel initiator");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_tunnel_initiator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6517_vxlan_tunnel_initiator_create(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_tunnel_initiator_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_vxlan_tunnel_initiator_get(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info)
{
    int rv;
    bcm_compat6517_tunnel_initiator_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6517_tunnel_initiator_t*)
            sal_alloc(sizeof(bcm_compat6517_tunnel_initiator_t), "New tunnel initiator");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_tunnel_initiator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6517_vxlan_tunnel_initiator_get(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_tunnel_initiator_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_wlan_tunnel_initiator_create(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info)
{
    int rv;
    bcm_compat6517_tunnel_initiator_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6517_tunnel_initiator_t*)
            sal_alloc(sizeof(bcm_compat6517_tunnel_initiator_t), "New tunnel initiator");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_tunnel_initiator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6517_wlan_tunnel_initiator_create(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_tunnel_initiator_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_wlan_tunnel_initiator_get(
    int unit,
    bcm_compat6516_tunnel_initiator_t *info)
{
    int rv;
    bcm_compat6517_tunnel_initiator_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6517_tunnel_initiator_t*)
            sal_alloc(sizeof(bcm_compat6517_tunnel_initiator_t), "New tunnel initiator");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_tunnel_initiator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6517_wlan_tunnel_initiator_get(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_tunnel_initiator_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_flow_port_encap_t
 * Purpose:
 *      Convert the bcm_flow_port_encap_t datatype from <=6.5.16 to 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_flow_port_encap_t(
    bcm_compat6516_flow_port_encap_t *from,
    bcm_compat6518_flow_port_encap_t *to)
{
    sal_memset(to, 0, sizeof(*to));
    to->flow_port      = from->flow_port;
    to->options        = from->options;
    to->flags          = from->flags;
    to->class_id       = from->class_id;
    to->mtu            = from->mtu;
    to->network_group  = from->network_group;
    to->dvp_group      = from->dvp_group;
    to->pri            = from->pri;
    to->cfi            = from->cfi;
    to->tpid           = from->tpid;
    to->vlan           = from->vlan;
    to->egress_if      = from->egress_if;
    to->valid_elements = from->valid_elements;
    to->flow_handle    = from->flow_handle;
    to->flow_option    = from->flow_option;
}

/*
 * Function:
 *      _bcm_compat6516out_flow_port_encap_t
 * Purpose:
 *      Convert the bcm_flow_port_encap_t datatype f from 6.5.16+ to <=6.5.16.
 * Parameters:
 *      from        - (IN) The 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_flow_port_encap_t(
    bcm_compat6518_flow_port_encap_t *from,
    bcm_compat6516_flow_port_encap_t *to)
{
    to->flow_port      = from->flow_port;
    to->options        = from->options;
    to->flags          = from->flags;
    to->class_id       = from->class_id;
    to->mtu            = from->mtu;
    to->network_group  = from->network_group;
    to->dvp_group      = from->dvp_group;
    to->pri            = from->pri;
    to->cfi            = from->cfi;
    to->tpid           = from->tpid;
    to->vlan           = from->vlan;
    to->egress_if      = from->egress_if;
    to->valid_elements = from->valid_elements;
    to->flow_handle    = from->flow_handle;
    to->flow_option    = from->flow_option;
}

int
bcm_compat6516_flow_port_encap_set(
    int unit,
    bcm_compat6516_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_compat6518_flow_port_encap_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6518_flow_port_encap_t *)
            sal_alloc(sizeof(bcm_compat6518_flow_port_encap_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_flow_port_encap_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6518_flow_port_encap_set(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_flow_port_encap_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_flow_port_encap_get(
    int unit,
    bcm_compat6516_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_compat6518_flow_port_encap_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6518_flow_port_encap_t *)
            sal_alloc(sizeof(bcm_compat6518_flow_port_encap_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_flow_port_encap_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6518_flow_port_encap_get(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_flow_port_encap_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_flow_tunnel_initiator_t
 * Purpose:
 *      Convert the _bcm_compat6516in_flow_tunnel_initiator_t datatype
 *      from <=6.5.16 to 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_flow_tunnel_initiator_t(
    bcm_compat6516_flow_tunnel_initiator_t *from,
    bcm_flow_tunnel_initiator_t *to)
{
    bcm_flow_tunnel_initiator_t_init(to);
    to->flags           = from->flags;
    to->type            = from->type;
    to->ttl             = from->ttl;
    to->flow_label      = from->flow_label;
    to->dscp_sel        = from->dscp_sel;
    to->dscp            = from->dscp;
    to->dscp_map        = from->dscp_map;
    to->tunnel_id       = from->tunnel_id;
    to->udp_dst_port    = from->udp_dst_port;
    to->udp_src_port    = from->udp_src_port;
    to->vlan            = from->vlan;
    to->tpid            = from->tpid;
    to->pkt_pri         = from->pkt_pri;
    to->pkt_cfi         = from->pkt_cfi;
    to->ip4_id          = from->ip4_id;
    to->l3_intf_id      = from->l3_intf_id;
    to->flow_port       = from->flow_port;
    to->valid_elements  = from->valid_elements;
    to->flow_handle     = from->flow_handle;
    to->flow_option     = from->flow_option;
    sal_memcpy(to->dmac,  from->dmac, sizeof(bcm_mac_t));
    sal_memcpy(&to->dip, &from->dip,  sizeof(bcm_ip_t));
    sal_memcpy(&to->sip, &from->sip,  sizeof(bcm_ip_t));
    sal_memcpy(to->sip6,  from->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6,  from->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->smac,  from->smac, sizeof(bcm_mac_t));
}

/*
 * Function:
 *      _bcm_compat6516in_flow_tunnel_initiator_t
 * Purpose:
 *      Convert the _bcm_compat6516in_flow_tunnel_initiator_t datatype
 *      from <=6.5.16 to 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_flow_tunnel_initiator_t(
    bcm_flow_tunnel_initiator_t*from,
    bcm_compat6516_flow_tunnel_initiator_t *to)
{
    to->flags           = from->flags;
    to->type            = from->type;
    to->ttl             = from->ttl;
    to->flow_label      = from->flow_label;
    to->dscp_sel        = from->dscp_sel;
    to->dscp            = from->dscp;
    to->dscp_map        = from->dscp_map;
    to->tunnel_id       = from->tunnel_id;
    to->udp_dst_port    = from->udp_dst_port;
    to->udp_src_port    = from->udp_src_port;
    to->vlan            = from->vlan;
    to->tpid            = from->tpid;
    to->pkt_pri         = from->pkt_pri;
    to->pkt_cfi         = from->pkt_cfi;
    to->ip4_id          = from->ip4_id;
    to->l3_intf_id      = from->l3_intf_id;
    to->flow_port       = from->flow_port;
    to->valid_elements  = from->valid_elements;
    to->flow_handle     = from->flow_handle;
    to->flow_option     = from->flow_option;
    sal_memcpy(to->dmac,  from->dmac, sizeof(bcm_mac_t));
    sal_memcpy(&to->dip, &from->dip,  sizeof(bcm_ip_t));
    sal_memcpy(&to->sip, &from->sip,  sizeof(bcm_ip_t));
    sal_memcpy(to->sip6,  from->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6,  from->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->smac,  from->smac, sizeof(bcm_mac_t));
}

int
bcm_compat6516_flow_tunnel_initiator_create(
    int unit,
    bcm_compat6516_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_tunnel_initiator_t *)
            sal_alloc(sizeof(bcm_flow_tunnel_initiator_t), "New flow tunnel");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_flow_tunnel_initiator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_initiator_create(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_flow_tunnel_initiator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_flow_tunnel_initiator_get(
    int unit,
    bcm_compat6516_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_tunnel_initiator_t *)
            sal_alloc(sizeof(bcm_flow_tunnel_initiator_t), "New flow tunnel");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_flow_tunnel_initiator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_initiator_get(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_flow_tunnel_initiator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_flow_match_config_t
 * Purpose:
 *      Convert the bcm_flow_port_t datatype from <=6.5.16 to 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_flow_match_config_t(
    bcm_compat6516_flow_match_config_t *from,
    bcm_flow_match_config_t *to)
{
    bcm_flow_match_config_t_init(to);
    to->vnid                = from->vnid;
    to->vlan                = from->vlan;
    to->inner_vlan          = from->inner_vlan;
    to->sip                 = from->sip;
    to->sip_mask            = from->sip_mask;
    to->dip                 = from->dip;
    to->dip_mask            = from->dip_mask;
    to->udp_dst_port        = from->udp_dst_port;
    to->udp_src_port        = from->udp_src_port;
    to->protocol            = from->protocol;
    to->mpls_label          = from->mpls_label;
    to->flow_port           = from->flow_port;
    to->vpn                 = from->vpn;
    to->intf_id             = from->intf_id;
    to->options             = from->options;
    to->criteria            = from->criteria;
    to->valid_elements      = from->valid_elements;
    to->flow_handle         = from->flow_handle;
    to->flow_option         = from->flow_option;
    sal_memcpy(to->sip6,      from->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->sip6_mask, from->sip6_mask, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6,      from->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6_mask, from->dip6_mask, sizeof(bcm_ip6_t));
}

/*
 * Function:
 *      _bcm_compat6516out_flow_match_config_t
 * Purpose:
 *      Convert the bcm_flow_port_t datatype from  6.5.16+ to <=6.5.16
 * Parameters:
 *      from        - (IN) The 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_flow_match_config_t(
    bcm_flow_match_config_t *from,
    bcm_compat6516_flow_match_config_t *to)
{
    to->vnid                = from->vnid;
    to->vlan                = from->vlan;
    to->inner_vlan          = from->inner_vlan;
    to->sip                 = from->sip;
    to->sip_mask            = from->sip_mask;
    to->dip                 = from->dip;
    to->dip_mask            = from->dip_mask;
    to->udp_dst_port        = from->udp_dst_port;
    to->udp_src_port        = from->udp_src_port;
    to->protocol            = from->protocol;
    to->mpls_label          = from->mpls_label;
    to->flow_port           = from->flow_port;
    to->vpn                 = from->vpn;
    to->intf_id             = from->intf_id;
    to->options             = from->options;
    to->criteria            = from->criteria;
    to->valid_elements      = from->valid_elements;
    to->flow_handle         = from->flow_handle;
    to->flow_option         = from->flow_option;
    sal_memcpy(to->sip6,      from->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->sip6_mask, from->sip6_mask, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6,      from->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6_mask, from->dip6_mask, sizeof(bcm_ip6_t));
}

int
bcm_compat6516_flow_match_add(
    int unit,
    bcm_compat6516_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_add(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_flow_match_delete(
    int unit,
    bcm_compat6516_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_delete(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6516_flow_match_get(
    int unit,
    bcm_compat6516_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_get(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from <=6.5.16 to
 *      SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_l3_egress_t(
    bcm_compat6516_l3_egress_t *from,
    bcm_l3_egress_t *to)
{
    int i = 0;

    bcm_l3_egress_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->intf = from->intf;
    sal_memcpy(to->mac_addr, from->mac_addr, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->module = from->module;
    to->port = from->port;
    to->trunk = from->trunk;
    to->mpls_flags = from->mpls_flags;
    to->mpls_label = from->mpls_label;
    to->mpls_action = from->mpls_action;
    to->mpls_qos_map_id = from->mpls_qos_map_id;
    to->mpls_ttl = from->mpls_ttl;
    to->mpls_pkt_pri = from->mpls_pkt_pri;
    to->mpls_pkt_cfi = from->mpls_pkt_cfi;
    to->mpls_exp = from->mpls_exp;
    to->qos_map_id = from->qos_map_id;
    to->encap_id = from->encap_id;
    to->failover_id = from->failover_id;
    to->failover_if_id = from->failover_if_id;
    to->failover_mc_group = from->failover_mc_group;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->intf_class = from->intf_class;
    to->multicast_flags = from->multicast_flags;
    to->oam_global_context_id = from->oam_global_context_id;
    to->vntag = from->vntag;
    to->vntag_action = from->vntag_action;
    to->etag = from->etag;
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
    to->mc_group = from->mc_group;
    sal_memcpy(to->src_mac_addr, from->src_mac_addr, sizeof(bcm_mac_t));
    to->hierarchical_gport = from->hierarchical_gport;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->vlan_port_id = from->vlan_port_id;
    to->replication_id = from->replication_id;
}

/*
 * Function:
 *      _bcm_compat6516out_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from 6.5.16+ to
 *      <=6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_l3_egress_t(
    bcm_l3_egress_t *from,
    bcm_compat6516_l3_egress_t *to)
{
    int i = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->intf = from->intf;
    sal_memcpy(to->mac_addr, from->mac_addr, sizeof(bcm_mac_t));
    to->vlan = from->vlan;
    to->module = from->module;
    to->port = from->port;
    to->trunk = from->trunk;
    to->mpls_flags = from->mpls_flags;
    to->mpls_label = from->mpls_label;
    to->mpls_action = from->mpls_action;
    to->mpls_qos_map_id = from->mpls_qos_map_id;
    to->mpls_ttl = from->mpls_ttl;
    to->mpls_pkt_pri = from->mpls_pkt_pri;
    to->mpls_pkt_cfi = from->mpls_pkt_cfi;
    to->mpls_exp = from->mpls_exp;
    to->qos_map_id = from->qos_map_id;
    to->encap_id = from->encap_id;
    to->failover_id = from->failover_id;
    to->failover_if_id = from->failover_if_id;
    to->failover_mc_group = from->failover_mc_group;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->intf_class = from->intf_class;
    to->multicast_flags = from->multicast_flags;
    to->oam_global_context_id = from->oam_global_context_id;
    to->vntag = from->vntag;
    to->vntag_action = from->vntag_action;
    to->etag = from->etag;
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
    to->mc_group = from->mc_group;
    sal_memcpy(to->src_mac_addr, from->src_mac_addr, sizeof(bcm_mac_t));
    to->hierarchical_gport = from->hierarchical_gport;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->vlan_port_id = from->vlan_port_id;
    to->replication_id = from->replication_id;
}

int
bcm_compat6516_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6516_l3_egress_t *egr,
    bcm_if_t *intf)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_create(unit, flags, new_egr, intf);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6516_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6516_l3_egress_t *egr)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_get(unit, intf, new_egr);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6516_l3_egress_find(
    int unit,
    bcm_compat6516_l3_egress_t *egr,
    bcm_if_t *intf)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_find(unit, new_egr, intf);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6516_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6516_l3_egress_t *egr)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_set(unit, intf, new_egr);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6516_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6516_l3_egress_t *egr)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_get(unit, intf, new_egr);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_l3_host_t
 * Purpose:
 *      Convert the bcm_l3_host_t datatype from <=6.5.16 to 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_l3_host_t(
    bcm_compat6516_l3_host_t *from,
    bcm_l3_host_t *to)
{
    int i = 0;

    bcm_l3_host_t_init(to);
    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_ip_addr = from->l3a_ip_addr;
    sal_memcpy(to->l3a_ip6_addr, from->l3a_ip6_addr, sizeof(bcm_ip6_t));
    to->l3a_pri = from->l3a_pri;
    to->l3a_intf = from->l3a_intf;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_ipmc_ptr = from->l3a_ipmc_ptr;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->encap_id = from->encap_id;
    to->native_intf = from->native_intf;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->l3a_ipmc_ptr_l2 = from->l3a_ipmc_ptr_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
}

/*
 * Function:
 *      _bcm_compat6516out_l3_host_t
 * Purpose:
 *      Convert the bcm_l3_host_t datatype from 6.5.16+ to <=6.5.16
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_l3_host_t(
    bcm_l3_host_t *from,
    bcm_compat6516_l3_host_t *to)
{
    int i = 0;

    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_ip_addr = from->l3a_ip_addr;
    sal_memcpy(to->l3a_ip6_addr, from->l3a_ip6_addr, sizeof(bcm_ip6_t));
    to->l3a_pri = from->l3a_pri;
    to->l3a_intf = from->l3a_intf;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_ipmc_ptr = from->l3a_ipmc_ptr;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->encap_id = from->encap_id;
    to->native_intf = from->native_intf;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->l3a_ipmc_ptr_l2 = from->l3a_ipmc_ptr_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
}

int bcm_compat6516_l3_host_find(
    int unit,
    bcm_compat6516_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_find(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_host_add(
    int unit,
    bcm_compat6516_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_add(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_host_delete(
    int unit,
    bcm_compat6516_l3_host_t *ip_addr)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != ip_addr) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(ip_addr, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_host_delete_by_interface(
    int unit,
    bcm_compat6516_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_by_interface(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_host_delete_all(
    int unit,
    bcm_compat6516_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_all(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_host_stat_attach(
    int unit,
    bcm_compat6516_l3_host_t *info,
    uint32 stat_counter_id)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_attach(unit, new_info, stat_counter_id);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_host_stat_detach(
    int unit,
    bcm_compat6516_l3_host_t *info)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_detach(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_host_stat_counter_get(
    int unit,
    bcm_compat6516_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_get(unit, new_info, stat, num_entries,
                                      counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_host_stat_counter_sync_get(
    int unit,
    bcm_compat6516_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_sync_get(unit, new_info, stat, num_entries,
                                           counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_host_stat_counter_set(
    int unit,
    bcm_compat6516_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_counter_set(unit, new_info, stat, num_entries,
                                      counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_host_stat_id_get(
    int unit,
    bcm_compat6516_l3_host_t *info,
    bcm_l3_stat_t stat,
    uint32 *stat_counter_id)
{
    int rv;
    bcm_l3_host_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_stat_id_get(unit, new_info, stat, stat_counter_id);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_host_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_l3_route_t
 * Purpose:
 *      Convert the bcm_l3_route_t datatype from <=6.5.16 to 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_l3_route_t(
    bcm_compat6516_l3_route_t *from,
    bcm_l3_route_t *to)
{
    int i = 0;

    bcm_l3_route_t_init(to);
    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_ipmc_flags = from->l3a_ipmc_flags;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_subnet = from->l3a_subnet;
    sal_memcpy(to->l3a_ip6_net, from->l3a_ip6_net, sizeof(bcm_ip6_t));
    to->l3a_ip_mask = from->l3a_ip_mask;
    sal_memcpy(to->l3a_ip6_mask, from->l3a_ip6_mask, sizeof(bcm_ip6_t));
    to->l3a_intf = from->l3a_intf;
    to->l3a_nexthop_ip = from->l3a_nexthop_ip;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_vid = from->l3a_vid;
    to->l3a_pri = from->l3a_pri;
    to->l3a_tunnel_option = from->l3a_tunnel_option;
    to->l3a_mpls_label = from->l3a_mpls_label;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->l3a_expected_intf = from->l3a_expected_intf;
    to->l3a_rp = from->l3a_rp;
    to->l3a_mc_group = from->l3a_mc_group;
    to->l3a_expected_src_gport = from->l3a_expected_src_gport;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
}

/*
 * Function:
 *      _bcm_compat6516out_l3_route_t
 * Purpose:
 *      Convert the bcm_l3_route_t datatype from 6.5.16+ to <=6.5.16
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_l3_route_t(
    bcm_l3_route_t *from,
    bcm_compat6516_l3_route_t *to)
{
    int i = 0;

    to->l3a_flags = from->l3a_flags;
    to->l3a_flags2 = from->l3a_flags2;
    to->l3a_ipmc_flags = from->l3a_ipmc_flags;
    to->l3a_vrf = from->l3a_vrf;
    to->l3a_subnet = from->l3a_subnet;
    sal_memcpy(to->l3a_ip6_net, from->l3a_ip6_net, sizeof(bcm_ip6_t));
    to->l3a_ip_mask = from->l3a_ip_mask;
    sal_memcpy(to->l3a_ip6_mask, from->l3a_ip6_mask, sizeof(bcm_ip6_t));
    to->l3a_intf = from->l3a_intf;
    to->l3a_nexthop_ip = from->l3a_nexthop_ip;
    sal_memcpy(to->l3a_nexthop_mac, from->l3a_nexthop_mac, sizeof(bcm_mac_t));
    to->l3a_modid = from->l3a_modid;
    to->l3a_port_tgid = from->l3a_port_tgid;
    to->l3a_stack_port = from->l3a_stack_port;
    to->l3a_vid = from->l3a_vid;
    to->l3a_pri = from->l3a_pri;
    to->l3a_tunnel_option = from->l3a_tunnel_option;
    to->l3a_mpls_label = from->l3a_mpls_label;
    to->l3a_lookup_class = from->l3a_lookup_class;
    to->l3a_expected_intf = from->l3a_expected_intf;
    to->l3a_rp = from->l3a_rp;
    to->l3a_mc_group = from->l3a_mc_group;
    to->l3a_expected_src_gport = from->l3a_expected_src_gport;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
}

int bcm_compat6516_l3_host_delete_by_network(
    int unit,
    bcm_compat6516_l3_route_t *ip_addr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != ip_addr) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(ip_addr, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_host_delete_by_network(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_add(
    int unit,
    bcm_compat6516_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_add(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_delete(
    int unit,
    bcm_compat6516_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_delete(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_delete_by_interface(
    int unit,
    bcm_compat6516_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_delete_by_interface(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_delete_all(
    int unit,
    bcm_compat6516_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_delete_all(unit, new_info);

    if (NULL != new_info) {
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_get(
    int unit,
    bcm_compat6516_l3_route_t *info)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_get(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_multipath_get(
    int unit,
    bcm_compat6516_l3_route_t *the_route,
    bcm_compat6516_l3_route_t *path_array,
    int max_path,
    int *path_count)
{
    int rv, i;
    bcm_l3_route_t *new_info = NULL;
    bcm_l3_route_t *new_paths = NULL;

    if (NULL != the_route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(the_route, new_info);
    }

    if (NULL != path_array) {
        /* Create from heap to avoid the stack to bloat */
        new_paths = sal_alloc((sizeof(bcm_l3_route_t) * max_path), "New Route");
        if (NULL == new_paths) {
            if (NULL != new_info) {
                sal_free(new_info);
            }
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_multipath_get(unit, new_info, new_paths, max_path,
                                    path_count);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, the_route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    if (NULL != new_paths) {
        for (i = 0; i < *path_count && i < max_path; i++) {
            /* Transform the entry from the new format to old one */
            _bcm_compat6516out_l3_route_t(&new_paths[i], &path_array[i]);
        }
        /* Deallocate memory*/
        sal_free(new_paths);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_get(
    int unit,
    bcm_compat6516_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 *val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_get(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_get32(
    int unit,
    bcm_compat6516_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 *val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_get32(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_set(
    int unit,
    bcm_compat6516_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint64 val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_set(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_set32(
    int unit,
    bcm_compat6516_l3_route_t *route,
    bcm_l3_stat_t stat,
    uint32 val)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_set32(unit, new_info, stat, val);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_enable_set(
    int unit,
    bcm_compat6516_l3_route_t *route,
    int enable)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_enable_set(unit, new_info, enable);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_attach(
    int unit,
    bcm_compat6516_l3_route_t *route,
    uint32 stat_counter_id)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_attach(unit, new_info, stat_counter_id);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_detach(
    int unit,
    bcm_compat6516_l3_route_t *route)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(route, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_detach(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, route);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_counter_get(
    int unit,
    bcm_compat6516_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_counter_get(unit, new_info, stat, num_entries,
                                       counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_counter_sync_get(
    int unit,
    bcm_compat6516_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_counter_sync_get(unit, new_info, stat, num_entries,
                                            counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_counter_set(
    int unit,
    bcm_compat6516_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_counter_set(unit, new_info, stat, num_entries,
                                       counter_indexes, counter_values);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_multi_get(
    int unit,
    bcm_compat6516_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_get(unit, new_info, nstat, stat_arr,
                                     value_arr);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_multi_get32(
    int unit,
    bcm_compat6516_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_get32(unit, new_info, nstat, stat_arr,
                                       value_arr);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_multi_set(
    int unit,
    bcm_compat6516_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_set(unit, new_info, nstat, stat_arr,
                                     value_arr);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_multi_set32(
    int unit,
    bcm_compat6516_l3_route_t *info,
    int nstat,
    bcm_l3_route_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_multi_set32(unit, new_info, nstat, stat_arr,
                                       value_arr);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_stat_id_get(
    int unit,
    bcm_compat6516_l3_route_t *info,
    bcm_l3_route_stat_t stat,
    uint32 *stat_counter_id)
{
    int rv;
    bcm_l3_route_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_stat_id_get(unit, new_info, stat, stat_counter_id);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

int bcm_compat6516_l3_route_find(
    int unit,
    bcm_compat6516_l3_host_t *host,
    bcm_compat6516_l3_route_t *route)
{
    int rv;
    bcm_l3_host_t *new_host = NULL;
    bcm_l3_route_t *new_route = NULL;

    if (NULL != host) {
        /* Create from heap to avoid the stack to bloat */
        new_host = sal_alloc(sizeof(bcm_l3_host_t), "New Host");
        if (NULL == new_host) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_host_t(host, new_host);
    }

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_route = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_route) {
            if (NULL != new_host) {
                sal_free(new_host);
            }
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_route_find(unit, new_host, new_route);

    if (NULL != new_host) {
        /* Deallocate memory*/
        sal_free(new_host);
    }

    if (NULL != new_route) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_route, route);
        /* Deallocate memory*/
        sal_free(new_route);
    }

    return rv;
}

int bcm_compat6516_l3_subnet_route_find(
    int unit,
    bcm_compat6516_l3_route_t *input,
    bcm_compat6516_l3_route_t *route)
{
    int rv;
    bcm_l3_route_t *new_input = NULL;
    bcm_l3_route_t *new_route = NULL;

    if (NULL != input) {
        /* Create from heap to avoid the stack to bloat */
        new_input = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_input) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_l3_route_t(input, new_input);
    }

    if (NULL != route) {
        /* Create from heap to avoid the stack to bloat */
        new_route = sal_alloc(sizeof(bcm_l3_route_t), "New Route");
        if (NULL == new_route) {
            if (NULL != new_input) {
                sal_free(new_input);
            }
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_subnet_route_find(unit, new_input, new_route);

    if (NULL != new_input) {
        /* Deallocate memory*/
        sal_free(new_input);
    }

    if (NULL != new_route) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_l3_route_t(new_route, route);
        /* Deallocate memory*/
        sal_free(new_route);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_l3_route_t
 * Purpose:
 *      Convert the bcm_l3_route_t datatype from <=6.5.16 to 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_ipmc_addr_t(
    bcm_compat6516_ipmc_addr_t *from,
    bcm_ipmc_addr_t *to)
{
    int i = 0;

    bcm_ipmc_addr_t_init(to);
    to->s_ip_addr = from->s_ip_addr;
    to->mc_ip_addr = from->mc_ip_addr;
    sal_memcpy(to->s_ip6_addr, from->s_ip6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->mc_ip6_addr, from->mc_ip6_addr, sizeof(bcm_ip6_t));
    to->vid = from->vid;
    to->vrf = from->vrf;
    to->cos = from->cos;
    to->ts = from->ts;
    to->port_tgid = from->port_tgid;
    to->v = from->v;
    to->mod_id = from->mod_id;
    to->group = from->group;
    to->flags = from->flags;
    to->lookup_class = from->lookup_class;
    to->distribution_class = from->distribution_class;
    to->l3a_intf = from->l3a_intf;
    to->rp_id = from->rp_id;
    to->s_ip_mask = from->s_ip_mask;
    to->ing_intf = from->ing_intf;
    to->mc_ip_mask = from->mc_ip_mask;
    sal_memcpy(to->mc_ip6_mask, from->mc_ip6_mask, sizeof(bcm_ip6_t));
    to->group_l2 = from->group_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->priority = from->priority;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_logical_fields = from->num_logical_fields;
}

/*
 * Function:
 *      _bcm_compat6516out_ipmc_addr_t
 * Purpose:
 *      Convert the bcm_ipmc_addr_t datatype from 6.5.16+ to <=6.5.16
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_ipmc_addr_t(
    bcm_ipmc_addr_t *from,
    bcm_compat6516_ipmc_addr_t *to)
{
    int i = 0;

    to->s_ip_addr = from->s_ip_addr;
    to->mc_ip_addr = from->mc_ip_addr;
    sal_memcpy(to->s_ip6_addr, from->s_ip6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->mc_ip6_addr, from->mc_ip6_addr, sizeof(bcm_ip6_t));
    to->vid = from->vid;
    to->vrf = from->vrf;
    to->cos = from->cos;
    to->ts = from->ts;
    to->port_tgid = from->port_tgid;
    to->v = from->v;
    to->mod_id = from->mod_id;
    to->group = from->group;
    to->flags = from->flags;
    to->lookup_class = from->lookup_class;
    to->distribution_class = from->distribution_class;
    to->l3a_intf = from->l3a_intf;
    to->rp_id = from->rp_id;
    to->s_ip_mask = from->s_ip_mask;
    to->ing_intf = from->ing_intf;
    to->mc_ip_mask = from->mc_ip_mask;
    sal_memcpy(to->mc_ip6_mask, from->mc_ip6_mask, sizeof(bcm_ip6_t));
    to->group_l2 = from->group_l2;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->priority = from->priority;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_logical_fields = from->num_logical_fields;
}

int bcm_compat6516_ipmc_add(
    int unit,
    bcm_compat6516_ipmc_addr_t *data)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != data) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_add(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, data);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_find(
    int unit,
    bcm_compat6516_ipmc_addr_t *data)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != data) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_find(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, data);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_remove(
    int unit,
    bcm_compat6516_ipmc_addr_t *data)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != data) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(data, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_remove(unit, new_data);

    if (NULL != new_data) {
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_stat_attach(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    uint32 stat_counter_id)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_attach(unit, new_data, stat_counter_id);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_stat_detach(
    int unit,
    bcm_compat6516_ipmc_addr_t *info)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_detach(unit, new_data);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_stat_counter_get(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_get(unit, new_data, stat, num_entries,
                                   counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_stat_counter_sync_get(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_sync_get(unit, new_data, stat, num_entries,
                                        counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_stat_counter_set(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t *counter_values)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_counter_set(unit, new_data, stat, num_entries,
                                   counter_indexes, counter_values);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_stat_multi_get(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_get(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_stat_multi_get32(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_get32(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_stat_multi_set(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint64 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_set(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_stat_multi_set32(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    int nstat,
    bcm_ipmc_stat_t *stat_arr,
    uint32 *value_arr)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_multi_set32(unit, new_data, nstat, stat_arr, value_arr);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

int bcm_compat6516_ipmc_stat_id_get(
    int unit,
    bcm_compat6516_ipmc_addr_t *info,
    bcm_ipmc_stat_t stat,
    uint32 *stat_counter_id)
{
    int rv;
    bcm_ipmc_addr_t *new_data = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_data = sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC");
        if (NULL == new_data) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_ipmc_addr_t(info, new_data);
    }

    /* Call the BCM API with new format */
    rv = bcm_ipmc_stat_id_get(unit, new_data, stat, stat_counter_id);

    if (NULL != new_data) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_ipmc_addr_t(new_data, info);
        /* Deallocate memory*/
        sal_free(new_data);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_compat6516in_l3_nat_egress_t
 * Purpose:
 *   Convert the bcm_l3_nat_egress_t datatype from <=6.5.16 to
 *   SDK 6.5.16+
 * Parameters:
 *   from        - (IN) The <=6.5.16 version of the datatype
 *   to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516in_l3_nat_egress_t(
        bcm_compat6516_l3_nat_egress_t *from,
        bcm_l3_nat_egress_t *to)
{
    bcm_l3_nat_egress_t_init(to);
    to->flags = from->flags;
    to->nat_id = from->nat_id;
    to->sip_addr = from->sip_addr;
    to->sip_addr_mask = from->sip_addr_mask;
    to->src_port = from->src_port;
    to->dip_addr = from->dip_addr;
    to->dip_addr_mask = from->dip_addr_mask;
    to->dst_port = from->dst_port;
}

/*
 * Function:
 *   _bcm_compat6516out_l3_nat_egress_t
 * Purpose:
 *   Convert the bcm_l3_nat_egress_t datatype from 6.5.16+ to
 *   <=6.5.16
 * Parameters:
 *   from     - (IN) The 6.5.16+ version of the datatype
 *   to       - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516out_l3_nat_egress_t(
        bcm_l3_nat_egress_t *from,
        bcm_compat6516_l3_nat_egress_t *to)
{
    to->flags = from->flags;
    to->nat_id = from->nat_id;
    to->sip_addr = from->sip_addr;
    to->sip_addr_mask = from->sip_addr_mask;
    to->src_port = from->src_port;
    to->dip_addr = from->dip_addr;
    to->dip_addr_mask = from->dip_addr_mask;
    to->dst_port = from->dst_port;
}

/*
 * Function:
 *   bcm_compat6516_l3_nat_egress_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_egress_add
 * Parameters:
 *   unit           - (IN) BCM device unit number.
 *   nat_info       - (IN) Egress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_l3_nat_egress_add(
        int unit,
        bcm_compat6516_l3_nat_egress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_l3_nat_egress_t(nat_info, new_nat_info );
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_add(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6516out_l3_nat_egress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_l3_nat_egress_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_egress_get
 * Parameters:
 *   unit         - (IN) BCM device unit number.
 *   nat_info     - (OUT) Egress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_l3_nat_egress_get(
        int unit,
        bcm_compat6516_l3_nat_egress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_egress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_egress_t*)
            sal_alloc(sizeof(bcm_l3_nat_egress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_l3_nat_egress_t(nat_info, new_nat_info);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_egress_get(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6516out_l3_nat_egress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6516in_l3_nat_ingress_t
 * Purpose:
 *   Convert the bcm_l3_nat_ingress_t datatype from <=6.5.16 to
 *   SDK 6.5.16+
 * Parameters:
 *   from        - (IN) The <=6.5.16 version of the datatype
 *   to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516in_l3_nat_ingress_t(
        bcm_compat6516_l3_nat_ingress_t *from,
        bcm_l3_nat_ingress_t *to)
{
    bcm_l3_nat_ingress_t_init(to);
    to->flags = from->flags;
    to->ip_addr = from->ip_addr;
    to->vrf = from->vrf;
    to->l4_port = from->l4_port;
    to->ip_proto = from->ip_proto;
    to->nat_id = from->nat_id;
    to->pri = from->pri;
    to->class_id = from->class_id;
    to->nexthop = from->nexthop;
}

/*
 * Function:
 *   _bcm_compat6516out_l3_nat_ingress_t
 * Purpose:
 *   Convert the bcm_l3_nat_ingress_t datatype from 6.5.16+ to
 *   <=6.5.16
 * Parameters:
 *   from     - (IN) The 6.5.16+ version of the datatype
 *   to       - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516out_l3_nat_ingress_t(
        bcm_l3_nat_ingress_t *from,
        bcm_compat6516_l3_nat_ingress_t *to)
{
    to->flags = from->flags;
    to->ip_addr = from->ip_addr;
    to->vrf = from->vrf;
    to->l4_port = from->l4_port;
    to->ip_proto = from->ip_proto;
    to->nat_id = from->nat_id;
    to->pri = from->pri;
    to->class_id = from->class_id;
    to->nexthop = from->nexthop;
}

/*
 * Function:
 *   bcm_compat6516_l3_nat_ingress_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_ingress_add
 * Parameters:
 *   unit           - (IN) BCM device unit number.
 *   nat_info       - (IN) Ingress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_l3_nat_ingress_add(
        int unit,
        bcm_compat6516_l3_nat_ingress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_ingress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_ingress_t*)
            sal_alloc(sizeof(bcm_l3_nat_ingress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_l3_nat_ingress_t(nat_info, new_nat_info );
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_ingress_add(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6516out_l3_nat_ingress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_l3_nat_ingress_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_ingress_delete
 * Parameters:
 *   unit           - (IN) BCM device unit number.
 *   nat_info       - (IN) Ingress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_l3_nat_ingress_delete(
        int unit,
        bcm_compat6516_l3_nat_ingress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_ingress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_ingress_t*)
            sal_alloc(sizeof(bcm_l3_nat_ingress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_l3_nat_ingress_t(nat_info, new_nat_info );
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_ingress_delete(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6516out_l3_nat_ingress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_l3_nat_ingress_find
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_ingress_find
 * Parameters:
 *   unit           - (IN) BCM device unit number.
 *   nat_info       - (IN) Ingress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_l3_nat_ingress_find(
        int unit,
        bcm_compat6516_l3_nat_ingress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_ingress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_ingress_t*)
            sal_alloc(sizeof(bcm_l3_nat_ingress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_l3_nat_ingress_t(nat_info, new_nat_info );
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_ingress_find(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6516out_l3_nat_ingress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_l3_nat_ingress_delete_all
 * Purpose:
 *   Compatibility function for RPC call to bcm_l3_nat_ingress_delete_all
 * Parameters:
 *   unit           - (IN) BCM device unit number.
 *   nat_info       - (IN) Ingress nat object pointer.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6516_l3_nat_ingress_delete_all(
        int unit,
        bcm_compat6516_l3_nat_ingress_t *nat_info)
{
    int rv = 0;
    bcm_l3_nat_ingress_t *new_nat_info = NULL;

    if (nat_info != NULL) {
        new_nat_info = (bcm_l3_nat_ingress_t*)
            sal_alloc(sizeof(bcm_l3_nat_ingress_t), "New nat_info");
        if (new_nat_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_l3_nat_ingress_t(nat_info, new_nat_info );
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l3_nat_ingress_delete_all(unit, new_nat_info);

    if (rv >= 0) {
        _bcm_compat6516out_l3_nat_ingress_t(new_nat_info, nat_info);
    }

    sal_free(new_nat_info);
    return rv;
}

#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_compat6516in_vlan_action_set_t
 * Purpose:
 *      Convert the _bcm_compat6516in_vlan_action_set_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_vlan_action_set_t(
    bcm_compat6516_vlan_action_set_t *from,
    bcm_vlan_action_set_t *to)
{
    bcm_vlan_action_set_t_init(to);

    to->new_outer_vlan = from->new_outer_vlan;
    to->new_inner_vlan = from->new_inner_vlan;
    to->new_inner_pkt_prio = from->new_inner_pkt_prio;
    to->new_outer_cfi = from->new_outer_cfi;
    to->new_inner_cfi = from->new_inner_cfi;
    to->ingress_if = from->ingress_if;
    to->priority = from->priority;
    to->dt_outer = from->dt_outer;
    to->dt_outer_prio = from->dt_outer_prio;
    to->dt_outer_pkt_prio = from->dt_outer_pkt_prio;
    to->dt_outer_cfi = from->dt_outer_cfi;
    to->dt_inner = from->dt_inner;
    to->dt_inner_prio = from->dt_inner_prio;
    to->dt_inner_pkt_prio = from->dt_inner_pkt_prio;
    to->dt_inner_cfi = from->dt_inner_cfi;
    to->ot_outer = from->ot_outer;
    to->ot_outer_prio = from->ot_outer_prio;
    to->ot_outer_pkt_prio = from->ot_outer_pkt_prio;
    to->ot_outer_cfi = from->ot_outer_cfi;
    to->ot_inner = from->ot_inner;
    to->ot_inner_pkt_prio = from->ot_inner_pkt_prio;
    to->ot_inner_cfi = from->ot_inner_cfi;
    to->it_outer = from->it_outer;
    to->it_outer_pkt_prio = from->it_outer_pkt_prio;
    to->it_outer_cfi = from->it_outer_cfi;
    to->it_inner = from->it_inner;
    to->it_inner_prio = from->it_inner_prio;
    to->it_inner_pkt_prio = from->it_inner_pkt_prio;
    to->it_inner_cfi = from->it_inner_cfi;
    to->ut_outer = from->ut_outer;
    to->ut_outer_pkt_prio = from->ut_outer_pkt_prio;
    to->ut_outer_cfi = from->ut_outer_cfi;
    to->ut_inner = from->ut_inner;
    to->ut_inner_pkt_prio = from->ut_inner_pkt_prio;
    to->ut_inner_cfi = from->ut_inner_cfi;
    to->outer_pcp = from->outer_pcp;
    to->inner_pcp = from->inner_pcp;
    to->policer_id = from->policer_id;
    to->outer_tpid = from->outer_tpid;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid_action = from->outer_tpid_action;
    to->inner_tpid_action = from->inner_tpid_action;
    to->action_id = from->action_id;
    to->class_id = from->class_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->flags = from->flags;
}

/*
 * Function:
 *      _bcm_compat6516out_vlan_action_set_t
 * Purpose:
 *      Convert the bcm_vlan_action_set_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_vlan_action_set_t(
    bcm_vlan_action_set_t *from,
    bcm_compat6516_vlan_action_set_t *to)
{
    to->new_outer_vlan = from->new_outer_vlan;
    to->new_inner_vlan = from->new_inner_vlan;
    to->new_inner_pkt_prio = from->new_inner_pkt_prio;
    to->new_outer_cfi = from->new_outer_cfi;
    to->new_inner_cfi = from->new_inner_cfi;
    to->ingress_if = from->ingress_if;
    to->priority = from->priority;
    to->dt_outer = from->dt_outer;
    to->dt_outer_prio = from->dt_outer_prio;
    to->dt_outer_pkt_prio = from->dt_outer_pkt_prio;
    to->dt_outer_cfi = from->dt_outer_cfi;
    to->dt_inner = from->dt_inner;
    to->dt_inner_prio = from->dt_inner_prio;
    to->dt_inner_pkt_prio = from->dt_inner_pkt_prio;
    to->dt_inner_cfi = from->dt_inner_cfi;
    to->ot_outer = from->ot_outer;
    to->ot_outer_prio = from->ot_outer_prio;
    to->ot_outer_pkt_prio = from->ot_outer_pkt_prio;
    to->ot_outer_cfi = from->ot_outer_cfi;
    to->ot_inner = from->ot_inner;
    to->ot_inner_pkt_prio = from->ot_inner_pkt_prio;
    to->ot_inner_cfi = from->ot_inner_cfi;
    to->it_outer = from->it_outer;
    to->it_outer_pkt_prio = from->it_outer_pkt_prio;
    to->it_outer_cfi = from->it_outer_cfi;
    to->it_inner = from->it_inner;
    to->it_inner_prio = from->it_inner_prio;
    to->it_inner_pkt_prio = from->it_inner_pkt_prio;
    to->it_inner_cfi = from->it_inner_cfi;
    to->ut_outer = from->ut_outer;
    to->ut_outer_pkt_prio = from->ut_outer_pkt_prio;
    to->ut_outer_cfi = from->ut_outer_cfi;
    to->ut_inner = from->ut_inner;
    to->ut_inner_pkt_prio = from->ut_inner_pkt_prio;
    to->ut_inner_cfi = from->ut_inner_cfi;
    to->outer_pcp = from->outer_pcp;
    to->inner_pcp = from->inner_pcp;
    to->policer_id = from->policer_id;
    to->outer_tpid = from->outer_tpid;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid_action = from->outer_tpid_action;
    to->inner_tpid_action = from->inner_tpid_action;
    to->action_id = from->action_id;
    to->class_id = from->class_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->flags = from->flags;
}

/*
 * Function:
 *      _bcm_compat6516in_vlan_control_vlan_t
 * Purpose:
 *      Convert the _bcm_compat6516in_vlan_control_vlan_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_vlan_control_vlan_t(
    bcm_compat6516_vlan_control_vlan_t *from,
    bcm_compat6518_vlan_control_vlan_t *to)
{
    
    to->vrf = from->vrf;
    to->forwarding_vlan = from->forwarding_vlan;
    to->ingress_if = from->ingress_if;
    to->outer_tpid = from->outer_tpid;
    to->flags = from->flags;
    to->ip6_mcast_flood_mode = from->ip6_mcast_flood_mode;
    to->ip4_mcast_flood_mode = from->ip4_mcast_flood_mode;
    to->l2_mcast_flood_mode = from->l2_mcast_flood_mode;
    to->if_class = from->if_class;
    to->forwarding_mode = from->forwarding_mode;
    to->urpf_mode = from->urpf_mode;
    to->cosq = from->cosq;
    to->qos_map_id = from->qos_map_id;
    to->distribution_class = from->distribution_class;
    to->broadcast_group = from->broadcast_group;
    to->unknown_multicast_group = from->unknown_multicast_group;
    to->unknown_unicast_group = from->unknown_unicast_group;
    to->trill_nonunicast_group = from->trill_nonunicast_group;
    to->source_trill_name = from->source_trill_name;
    to->trunk_index = from->trunk_index;
    sal_memcpy(&to->protocol_pkt, &from->protocol_pkt,
               sizeof(bcm_vlan_protocol_packet_ctrl_t));
    to->nat_realm_id = from->nat_realm_id;
    to->l3_if_class = from->l3_if_class;
    to->vp_mc_ctrl = from->vp_mc_ctrl;
    to->aging_cycles = from->aging_cycles;
    to->entropy_id = from->entropy_id;
    to->vpn = from->vpn;
    to->egress_vlan = from->egress_vlan;
    to->learn_flags = from->learn_flags;
    to->sr_flags = from->sr_flags;
    to->flags2 = from->flags2;
    to->ingress_stat_id = from->ingress_stat_id;
    to->ingress_stat_pp_profile = from->ingress_stat_pp_profile;
    to->egress_stat_id = from->egress_stat_id;
    to->egress_stat_pp_profile = from->egress_stat_pp_profile;
}

/*
 * Function:
 *      _bcm_compat6516out_vlan_control_vlan_t
 * Purpose:
 *      Convert the bcm_vlan_control_vlan_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_vlan_control_vlan_t(
    bcm_compat6518_vlan_control_vlan_t *from,
    bcm_compat6516_vlan_control_vlan_t *to)
{
    to->vrf = from->vrf;
    to->forwarding_vlan = from->forwarding_vlan;
    to->ingress_if = from->ingress_if;
    to->outer_tpid = from->outer_tpid;
    to->flags = from->flags;
    to->ip6_mcast_flood_mode = from->ip6_mcast_flood_mode;
    to->ip4_mcast_flood_mode = from->ip4_mcast_flood_mode;
    to->l2_mcast_flood_mode = from->l2_mcast_flood_mode;
    to->if_class = from->if_class;
    to->forwarding_mode = from->forwarding_mode;
    to->urpf_mode = from->urpf_mode;
    to->cosq = from->cosq;
    to->qos_map_id = from->qos_map_id;
    to->distribution_class = from->distribution_class;
    to->broadcast_group = from->broadcast_group;
    to->unknown_multicast_group = from->unknown_multicast_group;
    to->unknown_unicast_group = from->unknown_unicast_group;
    to->trill_nonunicast_group = from->trill_nonunicast_group;
    to->source_trill_name = from->source_trill_name;
    to->trunk_index = from->trunk_index;
    sal_memcpy(&to->protocol_pkt, &from->protocol_pkt,
               sizeof(bcm_vlan_protocol_packet_ctrl_t));
    to->nat_realm_id = from->nat_realm_id;
    to->l3_if_class = from->l3_if_class;
    to->vp_mc_ctrl = from->vp_mc_ctrl;
    to->aging_cycles = from->aging_cycles;
    to->entropy_id = from->entropy_id;
    to->vpn = from->vpn;
    to->egress_vlan = from->egress_vlan;
    to->learn_flags = from->learn_flags;
    to->sr_flags = from->sr_flags;
    to->flags2 = from->flags2;
    to->ingress_stat_id = from->ingress_stat_id;
    to->ingress_stat_pp_profile = from->ingress_stat_pp_profile;
    to->egress_stat_id = from->egress_stat_id;
    to->egress_stat_pp_profile = from->egress_stat_pp_profile;
}

int
bcm_compat6516_vlan_port_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_default_action_get(unit, port, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_port_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_default_action_set(unit, port, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_port_egress_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_egress_default_action_get(unit, port, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_port_egress_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_egress_default_action_set(unit, port, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_port_protocol_action_add(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_protocol_action_add(unit, port, frame, ether, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_port_protocol_action_get(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_protocol_action_get(unit, port, frame, ether, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_mac_action_add(
    int unit,
    bcm_mac_t mac,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_mac_action_add(unit, mac, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}


int
bcm_compat6516_vlan_mac_action_get(
    int unit,
    bcm_mac_t mac,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_mac_action_get(unit, mac, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}


int
bcm_compat6516_vlan_translate_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_add(unit, port, key_type,
                                       outer_vlan, inner_vlan, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_translate_action_create(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_create(unit, port, key_type,
                                          outer_vlan, inner_vlan, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_translate_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_get(unit, port, key_type,
                                       outer_vlan, inner_vlan, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}


int
bcm_compat6516_vlan_translate_egress_action_add(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_add(unit, port_class,
                                              outer_vlan, inner_vlan,
                                              new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_translate_egress_action_get(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_get(unit, port_class, outer_vlan,
                                              inner_vlan, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_translate_egress_gport_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_gport_action_add(unit, port,
                                                    outer_vlan, inner_vlan,
                                                    new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_translate_egress_gport_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_gport_action_get(unit, port, outer_vlan,
                                                    inner_vlan, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}


int
bcm_compat6516_vlan_translate_action_range_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_range_add(unit, port,
                                             outer_vlan_low, outer_vlan_high,
                                             inner_vlan_low, inner_vlan_high,
                                             new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_translate_action_range_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_range_get(unit, port,
                                             outer_vlan_low, outer_vlan_high,
                                             inner_vlan_low, inner_vlan_high,
                                             new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}


int
bcm_compat6516_vlan_translate_action_id_set(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_translate_action_id_get(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_id_get(unit, flags, action_id, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_ip_action_add(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_ip_action_add(unit, vlan_ip, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_ip_action_get(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6516_vlan_action_set_t *action)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_ip_action_get(unit, vlan_ip, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, action);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_control_vlan_selective_get(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6516_vlan_control_vlan_t *control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_ctrl = NULL;

    if (NULL != control) {
        /* Create from heap to avoid the stack to bloat */
        new_ctrl = (bcm_compat6518_vlan_control_vlan_t *)
            sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan ctrl");
        if (NULL == new_ctrl) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_control_vlan_t(control, new_ctrl);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_selective_get(unit, vlan,
                                             valid_fields, new_ctrl);

    if (NULL != new_ctrl) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_control_vlan_t(new_ctrl, control);
        /* Deallocate memory*/
        sal_free(new_ctrl);
    }

    return rv;
}

int
bcm_compat6516_vlan_control_vlan_selective_set(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6516_vlan_control_vlan_t *control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_ctrl = NULL;

    if (NULL != control) {
        /* Create from heap to avoid the stack to bloat */
        new_ctrl = (bcm_compat6518_vlan_control_vlan_t *)
            sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan ctrl");
        if (NULL == new_ctrl) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_control_vlan_t(control, new_ctrl);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_selective_set(unit, vlan,
                                             valid_fields, new_ctrl);

    if (NULL != new_ctrl) {
        /* Deallocate memory*/
        sal_free(new_ctrl);
    }

    return rv;
}

int
bcm_compat6516_vlan_control_vlan_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6516_vlan_control_vlan_t *control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_ctrl = NULL;

    if (NULL != control) {
        /* Create from heap to avoid the stack to bloat */
        new_ctrl = (bcm_compat6518_vlan_control_vlan_t *)
            sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan ctrl");
        if (NULL == new_ctrl) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_control_vlan_t(control, new_ctrl);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_get(unit, vlan, new_ctrl);

    if (NULL != new_ctrl) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_control_vlan_t(new_ctrl, control);
        /* Deallocate memory*/
        sal_free(new_ctrl);
    }

    return rv;
}

int
bcm_compat6516_vlan_control_vlan_set(
    int unit,
    bcm_vlan_t vlan,
/* coverity[pass_by_value] */
    bcm_compat6516_vlan_control_vlan_t control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_ctrl = NULL;

    /* Create from heap to avoid the stack to bloat */
    new_ctrl = (bcm_compat6518_vlan_control_vlan_t *)
        sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan ctrl");
    if (NULL == new_ctrl) {
        return BCM_E_MEMORY;
    }

    /* Transform the entry from the old format to new one */
    _bcm_compat6516in_vlan_control_vlan_t(&control, new_ctrl);

    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_set(unit, vlan, *new_ctrl);

    if (NULL != new_ctrl) {
        /* Deallocate memory*/
        sal_free(new_ctrl);
    }

    return rv;
}

int
bcm_compat6516_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_vlan_match_info_t *match_info,
    bcm_compat6516_vlan_action_set_t *action_set)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action_set) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action_set, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_add(unit, options, match_info, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_match_action_get(
    int unit,
    bcm_vlan_match_info_t *match_info,
    bcm_compat6516_vlan_action_set_t *action_set)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != action_set) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(action_set, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_get(unit, match_info, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, action_set);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_vlan_match_action_multi_get(
    int unit,
    bcm_vlan_match_t match,
    int size,
    bcm_vlan_match_info_t *match_info_array,
    bcm_compat6516_vlan_action_set_t *action_set_array,
    int *count)
{
    int rv, i;
    bcm_vlan_action_set_t *new_action_set_array = NULL;

    if (NULL != action_set_array) {
        /* Create from heap to avoid the stack to bloat */
        new_action_set_array = (bcm_vlan_action_set_t *)
            sal_alloc(size * sizeof(bcm_vlan_action_set_t),
                      "New vlan action set array");
        if (NULL == new_action_set_array) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0 ; i < size; i++) {
            _bcm_compat6516in_vlan_action_set_t(&action_set_array[i],
                                                &new_action_set_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_multi_get(unit, match, size, match_info_array,
                                         new_action_set_array, count);

    if (NULL != new_action_set_array) {
        /* Transform the entry from the new format to old one */
        for (i = 0 ; i < size; i++) {
            _bcm_compat6516out_vlan_action_set_t(&new_action_set_array[i],
                                                 &action_set_array[i]);
        }
        /* Deallocate memory*/
        sal_free(new_action_set_array);
    }

    return rv;
}

int
bcm_compat6516_field_action_vlan_actions_add(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6516_vlan_action_set_t *vlan_action_set)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != vlan_action_set) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(vlan_action_set, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_field_action_vlan_actions_add(unit, entry, action, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

int
bcm_compat6516_field_action_vlan_actions_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6516_vlan_action_set_t *vlan_action_set)
{
    int rv;
    bcm_vlan_action_set_t *new_action = NULL;

    if (NULL != vlan_action_set) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_vlan_action_set_t *)
            sal_alloc(sizeof(bcm_vlan_action_set_t), "New vlan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vlan_action_set_t(vlan_action_set, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_field_action_vlan_actions_get(unit, entry, action, new_action);

    if (NULL != new_action) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vlan_action_set_t(new_action, vlan_action_set);
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_fcoe_vsan_action_set_t
 * Purpose:
 *      Convert the _bcm_compat6516in_fcoe_vsan_action_set_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_fcoe_vsan_action_set_t(
    bcm_compat6516_fcoe_vsan_action_set_t *from,
    bcm_fcoe_vsan_action_set_t *to)
{
    bcm_fcoe_vsan_action_set_t_init(to);

    to->flags = from->flags;
    to->new_vsan = from->new_vsan;
    to->new_vsan_pri = from->new_vsan_pri;
    sal_memcpy(&to->vft, &from->vft,
               sizeof(bcm_fcoe_vsan_vft_t));
    to->ingress_if = from->ingress_if;
    to->dt_vsan = from->dt_vsan;
    to->dt_vsan_pri = from->dt_vsan_pri;
    to->it_vsan = from->it_vsan;
    to->it_vsan_pri = from->it_vsan_pri;
    to->ot_vsan = from->ot_vsan;
    to->ot_vsan_pri = from->ot_vsan_pri;
    to->ut_vsan = from->ut_vsan;
    to->ut_vsan_pri = from->ut_vsan_pri;

    to->vlan_action.new_outer_vlan = from->vlan_action.new_outer_vlan;
    to->vlan_action.new_inner_vlan = from->vlan_action.new_inner_vlan;
    to->vlan_action.new_inner_pkt_prio = from->vlan_action.new_inner_pkt_prio;
    to->vlan_action.new_outer_cfi = from->vlan_action.new_outer_cfi;
    to->vlan_action.new_inner_cfi = from->vlan_action.new_inner_cfi;
    to->vlan_action.ingress_if = from->vlan_action.ingress_if;
    to->vlan_action.priority = from->vlan_action.priority;
    to->vlan_action.dt_outer = from->vlan_action.dt_outer;
    to->vlan_action.dt_outer_prio = from->vlan_action.dt_outer_prio;
    to->vlan_action.dt_outer_pkt_prio = from->vlan_action.dt_outer_pkt_prio;
    to->vlan_action.dt_outer_cfi = from->vlan_action.dt_outer_cfi;
    to->vlan_action.dt_inner = from->vlan_action.dt_inner;
    to->vlan_action.dt_inner_prio = from->vlan_action.dt_inner_prio;
    to->vlan_action.dt_inner_pkt_prio = from->vlan_action.dt_inner_pkt_prio;
    to->vlan_action.dt_inner_cfi = from->vlan_action.dt_inner_cfi;
    to->vlan_action.ot_outer = from->vlan_action.ot_outer;
    to->vlan_action.ot_outer_prio = from->vlan_action.ot_outer_prio;
    to->vlan_action.ot_outer_pkt_prio = from->vlan_action.ot_outer_pkt_prio;
    to->vlan_action.ot_outer_cfi = from->vlan_action.ot_outer_cfi;
    to->vlan_action.ot_inner = from->vlan_action.ot_inner;
    to->vlan_action.ot_inner_pkt_prio = from->vlan_action.ot_inner_pkt_prio;
    to->vlan_action.ot_inner_cfi = from->vlan_action.ot_inner_cfi;
    to->vlan_action.it_outer = from->vlan_action.it_outer;
    to->vlan_action.it_outer_pkt_prio = from->vlan_action.it_outer_pkt_prio;
    to->vlan_action.it_outer_cfi = from->vlan_action.it_outer_cfi;
    to->vlan_action.it_inner = from->vlan_action.it_inner;
    to->vlan_action.it_inner_prio = from->vlan_action.it_inner_prio;
    to->vlan_action.it_inner_pkt_prio = from->vlan_action.it_inner_pkt_prio;
    to->vlan_action.it_inner_cfi = from->vlan_action.it_inner_cfi;
    to->vlan_action.ut_outer = from->vlan_action.ut_outer;
    to->vlan_action.ut_outer_pkt_prio = from->vlan_action.ut_outer_pkt_prio;
    to->vlan_action.ut_outer_cfi = from->vlan_action.ut_outer_cfi;
    to->vlan_action.ut_inner = from->vlan_action.ut_inner;
    to->vlan_action.ut_inner_pkt_prio = from->vlan_action.ut_inner_pkt_prio;
    to->vlan_action.ut_inner_cfi = from->vlan_action.ut_inner_cfi;
    to->vlan_action.outer_pcp = from->vlan_action.outer_pcp;
    to->vlan_action.inner_pcp = from->vlan_action.inner_pcp;
    to->vlan_action.policer_id = from->vlan_action.policer_id;
    to->vlan_action.outer_tpid = from->vlan_action.outer_tpid;
    to->vlan_action.inner_tpid = from->vlan_action.inner_tpid;
    to->vlan_action.outer_tpid_action = from->vlan_action.outer_tpid_action;
    to->vlan_action.inner_tpid_action = from->vlan_action.inner_tpid_action;
    to->vlan_action.action_id = from->vlan_action.action_id;
    to->vlan_action.class_id = from->vlan_action.class_id;
    to->vlan_action.taf_gate_primap = from->vlan_action.taf_gate_primap;
    to->vlan_action.flags = from->vlan_action.flags;
}

int
bcm_compat6516_fcoe_vsan_translate_action_add(
    int unit,
    bcm_fcoe_vsan_translate_key_config_t *key,
    bcm_compat6516_fcoe_vsan_action_set_t *action)
{
    int rv;
    bcm_fcoe_vsan_action_set_t *new_action = NULL;

    if (NULL != action) {
        /* Create from heap to avoid the stack to bloat */
        new_action = (bcm_fcoe_vsan_action_set_t *)
            sal_alloc(sizeof(bcm_fcoe_vsan_action_set_t), "New fcoe vsan action");
        if (NULL == new_action) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_fcoe_vsan_action_set_t(action, new_action);
    }

    /* Call the BCM API with new format */
    rv = bcm_fcoe_vsan_translate_action_add(unit, key, new_action);

    if (NULL != new_action) {
        /* Deallocate memory*/
        sal_free(new_action);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_info_t
 * Purpose:
 *      Convert the _bcm_compat6516in_info_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_info_t(
    bcm_compat6516_info_t *from,
    bcm_info_t *to)
{
    bcm_info_t_init(to);

    to->vendor = from->vendor;
    to->device = from->device;
    to->revision = from->revision;
    to->capability = from->capability;
    to->num_pipes = from->num_pipes;
}

/*
 * Function:
 *      _bcm_compat6516out_info_t
 * Purpose:
 *      Convert the bcm_info_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_info_t(
    bcm_info_t *from,
    bcm_compat6516_info_t *to)
{
    to->vendor = from->vendor;
    to->device = from->device;
    to->revision = from->revision;
    to->capability = from->capability;
    to->num_pipes = from->num_pipes;
}

int
bcm_compat6516_info_get(
    int unit,
    bcm_compat6516_info_t *info)
{
    int rv;
    bcm_info_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_info_t *)
            sal_alloc(sizeof(bcm_info_t), "New bcm info");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_info_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_info_get(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_info_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_port_match_info_t
 * Purpose:
 *      Convert the bcm_compat6516_port_match_info_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_port_match_info_t(
    bcm_compat6516_port_match_info_t *from,
    bcm_port_match_info_t *to)
{
    to->match = from->match;
    to->port = from->port;
    to->match_vlan = from->match_vlan;
    to->match_vlan_max = from->match_vlan_max;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_inner_vlan_max = from->match_inner_vlan_max;
    to->match_tunnel_vlan = from->match_tunnel_vlan;
    sal_memcpy(to->match_tunnel_srcmac, from->match_tunnel_srcmac,
               sizeof(bcm_mac_t));
    to->match_label = from->match_label;
    to->flags = from->flags;
    to->match_pon_tunnel = from->match_pon_tunnel;
    to->match_ethertype = from->match_ethertype;
    to->match_pcp = from->match_pcp;

    to->action->new_outer_vlan = from->action->new_outer_vlan;
    to->action->new_inner_vlan = from->action->new_inner_vlan;
    to->action->new_inner_pkt_prio = from->action->new_inner_pkt_prio;
    to->action->new_outer_cfi = from->action->new_outer_cfi;
    to->action->new_inner_cfi = from->action->new_inner_cfi;
    to->action->ingress_if = from->action->ingress_if;
    to->action->priority = from->action->priority;
    to->action->dt_outer = from->action->dt_outer;
    to->action->dt_outer_prio = from->action->dt_outer_prio;
    to->action->dt_outer_pkt_prio = from->action->dt_outer_pkt_prio;
    to->action->dt_outer_cfi = from->action->dt_outer_cfi;
    to->action->dt_inner = from->action->dt_inner;
    to->action->dt_inner_prio = from->action->dt_inner_prio;
    to->action->dt_inner_pkt_prio = from->action->dt_inner_pkt_prio;
    to->action->dt_inner_cfi = from->action->dt_inner_cfi;
    to->action->ot_outer = from->action->ot_outer;
    to->action->ot_outer_prio = from->action->ot_outer_prio;
    to->action->ot_outer_pkt_prio = from->action->ot_outer_pkt_prio;
    to->action->ot_outer_cfi = from->action->ot_outer_cfi;
    to->action->ot_inner = from->action->ot_inner;
    to->action->ot_inner_pkt_prio = from->action->ot_inner_pkt_prio;
    to->action->ot_inner_cfi = from->action->ot_inner_cfi;
    to->action->it_outer = from->action->it_outer;
    to->action->it_outer_pkt_prio = from->action->it_outer_pkt_prio;
    to->action->it_outer_cfi = from->action->it_outer_cfi;
    to->action->it_inner = from->action->it_inner;
    to->action->it_inner_prio = from->action->it_inner_prio;
    to->action->it_inner_pkt_prio = from->action->it_inner_pkt_prio;
    to->action->it_inner_cfi = from->action->it_inner_cfi;
    to->action->ut_outer = from->action->ut_outer;
    to->action->ut_outer_pkt_prio = from->action->ut_outer_pkt_prio;
    to->action->ut_outer_cfi = from->action->ut_outer_cfi;
    to->action->ut_inner = from->action->ut_inner;
    to->action->ut_inner_pkt_prio = from->action->ut_inner_pkt_prio;
    to->action->ut_inner_cfi = from->action->ut_inner_cfi;
    to->action->outer_pcp = from->action->outer_pcp;
    to->action->inner_pcp = from->action->inner_pcp;
    to->action->policer_id = from->action->policer_id;
    to->action->outer_tpid = from->action->outer_tpid;
    to->action->inner_tpid = from->action->inner_tpid;
    to->action->outer_tpid_action = from->action->outer_tpid_action;
    to->action->inner_tpid_action = from->action->inner_tpid_action;
    to->action->action_id = from->action->action_id;
    to->action->class_id = from->action->class_id;
    to->action->taf_gate_primap = from->action->taf_gate_primap;
    to->action->flags = from->action->flags;

    to->extended_port_vid = from->extended_port_vid;
    to->vpn = from->vpn;
    to->niv_port_vif = from->niv_port_vif;
    to->isid = from->isid;
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
    to->port_class = from->port_class;
}

/*
 * Function:
 *      _bcm_compat6516out_port_match_info_t
 * Purpose:
 *      Convert the bcm_port_match_info_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_port_match_info_t(
    bcm_port_match_info_t *from,
    bcm_compat6516_port_match_info_t *to)
{
    to->match = from->match;
    to->port = from->port;
    to->match_vlan = from->match_vlan;
    to->match_vlan_max = from->match_vlan_max;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_inner_vlan_max = from->match_inner_vlan_max;
    to->match_tunnel_vlan = from->match_tunnel_vlan;
    sal_memcpy(to->match_tunnel_srcmac, from->match_tunnel_srcmac,
               sizeof(bcm_mac_t));
    to->match_label = from->match_label;
    to->flags = from->flags;
    to->match_pon_tunnel = from->match_pon_tunnel;
    to->match_ethertype = from->match_ethertype;
    to->match_pcp = from->match_pcp;

    to->action->new_outer_vlan = from->action->new_outer_vlan;
    to->action->new_inner_vlan = from->action->new_inner_vlan;
    to->action->new_inner_pkt_prio = from->action->new_inner_pkt_prio;
    to->action->new_outer_cfi = from->action->new_outer_cfi;
    to->action->new_inner_cfi = from->action->new_inner_cfi;
    to->action->ingress_if = from->action->ingress_if;
    to->action->priority = from->action->priority;
    to->action->dt_outer = from->action->dt_outer;
    to->action->dt_outer_prio = from->action->dt_outer_prio;
    to->action->dt_outer_pkt_prio = from->action->dt_outer_pkt_prio;
    to->action->dt_outer_cfi = from->action->dt_outer_cfi;
    to->action->dt_inner = from->action->dt_inner;
    to->action->dt_inner_prio = from->action->dt_inner_prio;
    to->action->dt_inner_pkt_prio = from->action->dt_inner_pkt_prio;
    to->action->dt_inner_cfi = from->action->dt_inner_cfi;
    to->action->ot_outer = from->action->ot_outer;
    to->action->ot_outer_prio = from->action->ot_outer_prio;
    to->action->ot_outer_pkt_prio = from->action->ot_outer_pkt_prio;
    to->action->ot_outer_cfi = from->action->ot_outer_cfi;
    to->action->ot_inner = from->action->ot_inner;
    to->action->ot_inner_pkt_prio = from->action->ot_inner_pkt_prio;
    to->action->ot_inner_cfi = from->action->ot_inner_cfi;
    to->action->it_outer = from->action->it_outer;
    to->action->it_outer_pkt_prio = from->action->it_outer_pkt_prio;
    to->action->it_outer_cfi = from->action->it_outer_cfi;
    to->action->it_inner = from->action->it_inner;
    to->action->it_inner_prio = from->action->it_inner_prio;
    to->action->it_inner_pkt_prio = from->action->it_inner_pkt_prio;
    to->action->it_inner_cfi = from->action->it_inner_cfi;
    to->action->ut_outer = from->action->ut_outer;
    to->action->ut_outer_pkt_prio = from->action->ut_outer_pkt_prio;
    to->action->ut_outer_cfi = from->action->ut_outer_cfi;
    to->action->ut_inner = from->action->ut_inner;
    to->action->ut_inner_pkt_prio = from->action->ut_inner_pkt_prio;
    to->action->ut_inner_cfi = from->action->ut_inner_cfi;
    to->action->outer_pcp = from->action->outer_pcp;
    to->action->inner_pcp = from->action->inner_pcp;
    to->action->policer_id = from->action->policer_id;
    to->action->outer_tpid = from->action->outer_tpid;
    to->action->inner_tpid = from->action->inner_tpid;
    to->action->outer_tpid_action = from->action->outer_tpid_action;
    to->action->inner_tpid_action = from->action->inner_tpid_action;
    to->action->action_id = from->action->action_id;
    to->action->class_id = from->action->class_id;
    to->action->taf_gate_primap = from->action->taf_gate_primap;
    to->action->flags = from->action->flags;

    to->extended_port_vid = from->extended_port_vid;
    to->vpn = from->vpn;
    to->niv_port_vif = from->niv_port_vif;
    to->isid = from->isid;
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
    to->port_class = from->port_class;
 }

int
bcm_compat6516_port_match_add(
    int unit,
    bcm_gport_t port,
    bcm_compat6516_port_match_info_t *match)
{
    int rv;
    bcm_port_match_info_t *new_match = NULL;
    bcm_vlan_action_set_t action;

    if (NULL != match) {
        /* Create from heap to avoid the stack to bloat */
        new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (NULL == new_match) {
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_match);
        bcm_vlan_action_set_t_init(&action);
        new_match->action = &action;

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_port_match_info_t(match, new_match);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_match_add(unit, port, new_match);

    if (NULL != new_match) {
        /* Deallocate memory*/
        sal_free(new_match);
    }

    return rv;
}

int
bcm_compat6516_port_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_compat6516_port_match_info_t *match)
{
    int rv;
    bcm_port_match_info_t *new_match = NULL;
    bcm_vlan_action_set_t action;

    if (NULL != match) {
        /* Create from heap to avoid the stack to bloat */
        new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (NULL == new_match) {
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_match);
        bcm_vlan_action_set_t_init(&action);
        new_match->action = &action;

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_port_match_info_t(match, new_match);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_match_delete(unit, port, new_match);

    if (NULL != new_match) {
        /* Deallocate memory*/
        sal_free(new_match);
    }

    return rv;
}

int
bcm_compat6516_port_match_replace(
    int unit,
    bcm_gport_t port,
    bcm_compat6516_port_match_info_t *old_match,
    bcm_compat6516_port_match_info_t *new_match)
{
    int rv;
    bcm_port_match_info_t *new_old_match = NULL;
    bcm_port_match_info_t *new_new_match = NULL;
    bcm_vlan_action_set_t old_action;
    bcm_vlan_action_set_t new_action;

    if (NULL != old_match) {
        /* Create from heap to avoid the stack to bloat */
        new_old_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New old_match info");
        if (NULL == new_old_match) {
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_old_match);
        bcm_vlan_action_set_t_init(&old_action);
        new_old_match->action = &old_action;

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_port_match_info_t(old_match, new_old_match);
    }

    if (NULL != new_match) {
        /* Create from heap to avoid the stack to bloat */
        new_new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New new_match info");
        if (NULL == new_new_match) {
            sal_free(new_old_match);
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_new_match);
        bcm_vlan_action_set_t_init(&new_action);
        new_new_match->action = &new_action;

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_port_match_info_t(new_match, new_new_match);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_match_replace(unit, port, new_old_match, new_new_match);

    if (NULL != new_old_match) {
        /* Deallocate memory*/
        sal_free(new_old_match);
    }

    if (NULL != new_new_match) {
        /* Deallocate memory*/
        sal_free(new_new_match);
    }

    return rv;
}

int
bcm_compat6516_port_match_multi_get(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6516_port_match_info_t *match_array,
    int *count)
{
    int rv, i;
    bcm_port_match_info_t *new_match_array = NULL;
    bcm_vlan_action_set_t action[size];

    if (NULL != match_array) {
        /* Create from heap to avoid the stack to bloat */
        new_match_array = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t) * size,
            "New port match info");
        if (NULL == new_match_array) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < size; i++) {
            bcm_port_match_info_t_init(&new_match_array[i]);
            bcm_vlan_action_set_t_init(&action[i]);
            new_match_array[i].action = &action[i];
            _bcm_compat6516in_port_match_info_t(&match_array[i],
                                                &new_match_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_port_match_multi_get(unit, port, size,
                                  new_match_array, count);

    if (NULL != new_match_array) {

        /* Transform the entry from the new format to old one */
        for (i = 0; i < size; i++) {
            _bcm_compat6516out_port_match_info_t(&new_match_array[i],
                                                 &match_array[i]);
        }

        /* Deallocate memory*/
        sal_free(new_match_array);
    }

    return rv;
}

int
bcm_compat6516_port_match_set(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6516_port_match_info_t *match_array)
{
    int rv, i;
    bcm_port_match_info_t *new_match_array = NULL;
    bcm_vlan_action_set_t action[size];

    if (NULL != match_array) {
        /* Create from heap to avoid the stack to bloat */
        new_match_array = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t) * size,
            "New port match info");
        if (NULL == new_match_array) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < size; i++) {
            bcm_port_match_info_t_init(&new_match_array[i]);
            bcm_vlan_action_set_t_init(&action[i]);
            new_match_array[i].action = &action[i];
            _bcm_compat6516in_port_match_info_t(&match_array[i],
                                                &new_match_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_port_match_set(unit, port, size, new_match_array);

    if (NULL != new_match_array) {
        /* Deallocate memory*/
        sal_free(new_match_array);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_port_speed_ability_t
 * Purpose:
 *      Convert the bcm_compat6516_port_speed_ability_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_port_speed_ability_t(
    bcm_compat6516_port_speed_ability_t *from,
    bcm_port_speed_ability_t *to)
{
    to->an_mode = from->an_mode;
    to->channel = from->channel;
    to->fec_type = from->fec_type;
    to->medium = from->medium;
    to->pause = from->pause;
    to->resolved_num_lanes = from->resolved_num_lanes;
    to->speed = from->speed;
    to->link_training = 0;
    to->parallel_detect_en = 0;
}

/*
 * Function:
 *      _bcm_compat6516out_port_speed_ability_t
 * Purpose:
 *      Convert the bcm_port_speed_ability_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_port_speed_ability_t(
    bcm_port_speed_ability_t *from,
    bcm_compat6516_port_speed_ability_t *to)
{
    to->an_mode = from->an_mode;
    to->channel = from->channel;
    to->fec_type = from->fec_type;
    to->medium = from->medium;
    to->pause = from->pause;
    to->resolved_num_lanes = from->resolved_num_lanes;
    to->speed = from->speed;
}

int
bcm_compat6516_port_autoneg_ability_advert_set(int unit,
                                            bcm_port_t port,
                                            int num_ability,
                                            bcm_compat6516_port_speed_ability_t *match_array)
{
    int rv, i;
    bcm_port_speed_ability_t *new_match_array = NULL;

    if (NULL != match_array) {
        /* Create from heap to avoid the stack to bloat */
        new_match_array = (bcm_port_speed_ability_t *)
            sal_alloc(sizeof(bcm_port_speed_ability_t) * num_ability, "New port speed ability array");
        if (NULL == new_match_array) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < num_ability; i++) {
            _bcm_compat6516in_port_speed_ability_t(&match_array[i], &new_match_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_port_autoneg_ability_advert_set(unit, port, num_ability, new_match_array);

    if (NULL != new_match_array) {
        /* Deallocate memory*/
        sal_free(new_match_array);
    }

    return rv;
}

int
bcm_compat6516_port_autoneg_ability_advert_get(int unit,
                                     bcm_port_t port,
                                     int max_num_ability,
                                     bcm_compat6516_port_speed_ability_t *match_array,
                                     int *actual_num_ability)
{
    int rv, i;
    bcm_port_speed_ability_t *new_match_array = NULL;

    if (NULL != match_array) {
        /* Create from heap to avoid the stack to bloat */
        new_match_array = (bcm_port_speed_ability_t *)
            sal_alloc(sizeof(bcm_port_speed_ability_t) * max_num_ability, "New port speed ability array");
        if (NULL == new_match_array) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < max_num_ability; i++) {
            _bcm_compat6516in_port_speed_ability_t(&match_array[i], &new_match_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_port_autoneg_ability_advert_get(unit, port, max_num_ability, new_match_array, actual_num_ability);

    if (NULL != new_match_array) {
        /* Transform the entry from the new format to old one */
        for (i = 0; i < max_num_ability; i++) {
            _bcm_compat6516out_port_speed_ability_t(&new_match_array[i], &match_array[i]);
        }
        /* Deallocate memory*/
        sal_free(new_match_array);
    }

    return rv;
}

int
bcm_compat6516_port_speed_ability_local_get(int unit, bcm_port_t port,
                                     int max_num_ability, bcm_compat6516_port_speed_ability_t *match_array,
                                     int *actual_num_ability)
{
    int rv, i;
    bcm_port_speed_ability_t *new_match_array = NULL;

    if (NULL != match_array) {
        /* Create from heap to avoid the stack to bloat */
        new_match_array = (bcm_port_speed_ability_t *)
            sal_alloc(sizeof(bcm_port_speed_ability_t) * max_num_ability, "New port speed ability array");
        if (NULL == new_match_array) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < max_num_ability; i++) {
            _bcm_compat6516in_port_speed_ability_t(&match_array[i], &new_match_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_port_speed_ability_local_get(unit, port, max_num_ability, new_match_array, actual_num_ability);

    if (NULL != new_match_array) {
        /* Transform the entry from the new format to old one */
        for (i = 0; i < max_num_ability; i++) {
            _bcm_compat6516out_port_speed_ability_t(&new_match_array[i], &match_array[i]);
        }
        /* Deallocate memory*/
        sal_free(new_match_array);
    }

    return rv;
}

int
bcm_compat6516_port_autoneg_ability_remote_get(int unit, bcm_port_t port,
                                     int max_num_ability, bcm_compat6516_port_speed_ability_t *match_array,
                                     int *actual_num_ability)
{
    int rv, i;
    bcm_port_speed_ability_t *new_match_array = NULL;

    if (NULL != match_array) {
        /* Create from heap to avoid the stack to bloat */
        new_match_array = (bcm_port_speed_ability_t *)
            sal_alloc(sizeof(bcm_port_speed_ability_t) * max_num_ability, "New port speed ability array");
        if (NULL == new_match_array) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < max_num_ability; i++) {
            _bcm_compat6516in_port_speed_ability_t(&match_array[i], &new_match_array[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_port_autoneg_ability_remote_get(unit, port, max_num_ability, new_match_array, actual_num_ability);

    if (NULL != new_match_array) {
        /* Transform the entry from the new format to old one */
        for (i = 0; i < max_num_ability; i++) {
            _bcm_compat6516out_port_speed_ability_t(&new_match_array[i], &match_array[i]);
        }
        /* Deallocate memory*/
        sal_free(new_match_array);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_port_phy_tx_t
 * Purpose:
 *      Convert the bcm_compat6516_port_phy_tx_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_port_phy_tx_t(
    bcm_compat6516_port_phy_tx_t *from,
    bcm_port_phy_tx_t *to)
{
    to->main = from->main;
    to->post = from->post;
    to->post2 = from->post2;
    to->post3 = from->post3;
    to->pre = from->pre;
    to->pre2 = from->pre2;
    to->signalling_mode = from->signalling_mode;
    to->tx_tap_mode = from->tx_tap_mode;
    to->amp = 0;
    to->drv_mode = 0;
}

/*
 * Function:
 *      _bcm_compat6516out_port_phy_tx_t
 * Purpose:
 *      Convert the bcm_compat6516_port_phy_tx_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_port_phy_tx_t(
    bcm_port_phy_tx_t *from,
    bcm_compat6516_port_phy_tx_t *to)

{
    to->main = from->main;
    to->post = from->post;
    to->post2 = from->post2;
    to->post3 = from->post3;
    to->pre = from->pre;
    to->pre2 = from->pre2;
    to->signalling_mode = from->signalling_mode;
    to->tx_tap_mode = from->tx_tap_mode;
}

int
bcm_compat6516_port_phy_tx_set(
    int unit,
    bcm_port_t port,
    bcm_compat6516_port_phy_tx_t *match)
{
    int rv;
    bcm_port_phy_tx_t *new_match = NULL;

    if (NULL != match) {
        /* Create from heap to avoid the stack to bloat */
        new_match = (bcm_port_phy_tx_t *)
            sal_alloc(sizeof(bcm_port_phy_tx_t), "New port phy tx");
        if (NULL == new_match) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_port_phy_tx_t(match, new_match);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_phy_tx_set(unit, port, new_match);

    if (NULL != new_match) {
        /* Deallocate memory*/
        sal_free(new_match);
    }

    return rv;
}

int
bcm_compat6516_port_phy_tx_get(
    int unit,
    bcm_port_t port,
    bcm_compat6516_port_phy_tx_t *match)
{
    int rv;
    bcm_port_phy_tx_t *new_match = NULL;

    if (NULL != match) {
        /* Create from heap to avoid the stack to bloat */
        new_match = (bcm_port_phy_tx_t *)
            sal_alloc(sizeof(bcm_port_phy_tx_t), "New port phy tx");
        if (NULL == new_match) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_port_phy_tx_t(match, new_match);
    }

    /* Call the BCM API with new format */
    rv = bcm_port_phy_tx_get(unit, port, new_match);

    if (NULL != new_match) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_port_phy_tx_t(new_match, match);

        /* Deallocate memory*/
        sal_free(new_match);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_rx_cosq_mapping_t
 * Purpose:
 *      Convert the bcm_compat6516_rx_cosq_mapping_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_rx_cosq_mapping_t(
    bcm_compat6516_rx_cosq_mapping_t *from,
    bcm_rx_cosq_mapping_t *to)
{
    bcm_rx_cosq_mapping_t_init(to);

    to->flags = from->flags;
    to->index = from->index;
    to->reasons = from->reasons;
    to->reasons_mask = from->reasons_mask;
    to->int_prio = from->int_prio;
    to->int_prio_mask = from->int_prio_mask;
    to->packet_type = from->packet_type;
    to->packet_type_mask = from->packet_type_mask;
    to->cosq = from->cosq;
}

/*
 * Function:
 *      _bcm_compat6516out_rx_cosq_mapping_t
 * Purpose:
 *      Convert the bcm_compat6516_rx_cosq_mapping_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_rx_cosq_mapping_t(
    bcm_rx_cosq_mapping_t *from,
    bcm_compat6516_rx_cosq_mapping_t *to)
{
    to->flags = from->flags;
    to->index = from->index;
    to->reasons = from->reasons;
    to->reasons_mask = from->reasons_mask;
    to->int_prio = from->int_prio;
    to->int_prio_mask = from->int_prio_mask;
    to->packet_type = from->packet_type;
    to->packet_type_mask = from->packet_type_mask;
    to->cosq = from->cosq;
}

/*
 * Function: bcm_compat6516_rx_cosq_mapping_extended_set
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_set.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      options - options.
 *      rx_cosq_mapping - bcm_compat6516_rx_cosq_mapping_t.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6516_rx_cosq_mapping_extended_set(
    int unit,
    uint32 options,
    bcm_compat6516_rx_cosq_mapping_t *rx_cosq_mapping)
{
    int rv;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (NULL != rx_cosq_mapping) {
        /* Create from heap to avoid the stack to bloat */
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
            sal_alloc(sizeof(bcm_rx_cosq_mapping_t), "New rx cos mapping");
        if (NULL == new_rx_cosq_mapping) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_set(unit, options, new_rx_cosq_mapping);

    if (NULL != new_rx_cosq_mapping) {
        /* Deallocate memory*/
        sal_free(new_rx_cosq_mapping);
    }

    return rv;
}

/*
 * Function: bcm_compat6516_rx_cosq_mapping_extended_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_rx_cosq_mapping_extended_get.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      rx_cosq_mapping - bcm_compat6516_rx_cosq_mapping_t.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6516_rx_cosq_mapping_extended_get(
    int unit,
    bcm_compat6516_rx_cosq_mapping_t *rx_cosq_mapping)

{
    int rv;
    bcm_rx_cosq_mapping_t *new_rx_cosq_mapping = NULL;

    if (NULL != rx_cosq_mapping) {
        /* Create from heap to avoid the stack to bloat */
        new_rx_cosq_mapping = (bcm_rx_cosq_mapping_t *)
            sal_alloc(sizeof(bcm_rx_cosq_mapping_t), "New rx cos mapping");
        if (NULL == new_rx_cosq_mapping) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_rx_cosq_mapping_t(rx_cosq_mapping, new_rx_cosq_mapping);
    }

    /* Call the BCM API with new format */
    rv = bcm_rx_cosq_mapping_extended_get(unit, new_rx_cosq_mapping);

    if (NULL != new_rx_cosq_mapping) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_rx_cosq_mapping_t(new_rx_cosq_mapping, rx_cosq_mapping);
        /* Deallocate memory*/
        sal_free(new_rx_cosq_mapping);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6516in_field_group_config_t
 * Purpose:
 *      Convert the bcm_compat6516_field_group_config_t datatype from <=6.5.16
 *      to SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_field_group_config_t(
    bcm_compat6516_field_group_config_t *from,
    bcm_field_group_config_t *to)
{
    bcm_field_group_config_t_init(to);

    to->flags = from->flags;
    to->qset = from->qset;
    to->priority = from->priority;
    to->mode = from->mode;
    to->ports = from->ports;
    to->group = from->group;
    to->aset = from->aset;
    to->preselset = from->preselset;
    to->group_ref = from->group_ref;
    to->max_entry_priorities = from->max_entry_priorities;
    to->hintid  = from->hintid;
    to->action_res_id = from->action_res_id;
    sal_memcpy(to->name, from->name,
               (sizeof(uint8) *  BCM_FIELD_MAX_NAME_LEN));
    to->cycle = from->cycle;
    to->pgm_bmp = from->pgm_bmp;
    to->share_id = from->share_id;
}

/*
 * Function:
 *      _bcm_compat6516out_field_group_config_t
 * Purpose:
 *      Convert the bcm_compat6516_field_group_config_t datatype from 6.5.16+ to
 *      <=SDK 6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_field_group_config_t(
    bcm_field_group_config_t *from,
    bcm_compat6516_field_group_config_t *to)
{
    to->flags = from->flags;
    to->qset = from->qset;
    to->priority = from->priority;
    to->mode = from->mode;
    to->ports = from->ports;
    to->group = from->group;
    to->aset = from->aset;
    to->preselset = from->preselset;
    to->group_ref = from->group_ref;
    to->max_entry_priorities = from->max_entry_priorities;
    to->hintid  = from->hintid;
    to->action_res_id = from->action_res_id;
    sal_memcpy(to->name, from->name,
               (sizeof(uint8) *  BCM_FIELD_MAX_NAME_LEN));
    to->cycle = from->cycle;
    to->pgm_bmp = from->pgm_bmp;
    to->share_id = from->share_id;

}

/*
 * Function: bcm_compat6516_field_group_config_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_group_config_create.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      group_config - bcm_compat6516_field_group_config_t.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6516_field_group_config_create(
    int unit,
    bcm_compat6516_field_group_config_t *group_config)
{
    int rv;
    bcm_field_group_config_t *new_group_config = NULL;

    if (NULL != group_config) {
        /* Create from heap to avoid the stack to bloat */
        new_group_config  = (bcm_field_group_config_t *)
            sal_alloc(sizeof(bcm_field_group_config_t), "New field group config");
        if (NULL == new_group_config) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_field_group_config_t(group_config, new_group_config);
    }

    /* Call the BCM API with new format */
    rv = bcm_field_group_config_create(unit, new_group_config);

    if (BCM_SUCCESS(rv) && new_group_config != NULL && group_config != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_field_group_config_t(new_group_config, group_config);
    }

    if (NULL != new_group_config) {
        /* Deallocate memory*/
        sal_free(new_group_config);
    }

    return rv;
}

/*
 * Function: bcm_compat6516_field_group_config_validate
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_group_config_validate.
 *
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      group_config - bcm_compat6516_field_group_config_t.
 *      mode - Reference to group mode.
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6516_field_group_config_validate(
    int unit,
    bcm_compat6516_field_group_config_t *group_config,
    bcm_field_group_mode_t *mode)
{
    int rv;
    bcm_field_group_config_t *new_group_config = NULL;

    if (NULL != group_config) {
        /* Create from heap to avoid the stack to bloat */
        new_group_config  = (bcm_field_group_config_t *)
            sal_alloc(sizeof(bcm_field_group_config_t), "New field group config");
        if (NULL == new_group_config) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_field_group_config_t(group_config, new_group_config);
    }

    /* Call the BCM API with new format */
    rv = bcm_field_group_config_validate(unit, new_group_config, mode);

    if (NULL != new_group_config) {
        /* Deallocate memory*/
        sal_free(new_group_config);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_compat6516in_mirror_destination_t
 * Purpose:
 *   Convert the bcm_mirror_destination_t datatype from <=6.5.16 to
 *   SDK 6.5.16+.
 * Parameters:
 *   from        - (IN) The <=6.5.16 version of the datatype.
 *   to          - (OUT) The SDK 6.5.16+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516in_mirror_destination_t(
    bcm_compat6516_mirror_destination_t *from,
    bcm_mirror_destination_t *to)
{
    bcm_mirror_destination_t_init(to);
    to->mirror_dest_id = from->mirror_dest_id;
    to->flags = from->flags;
    to->gport = from->gport;
    to->version = from->version;
    to->tos = from->tos;
    to->ttl = from->ttl;
    to->src_addr = from->src_addr;
    to->dst_addr = from->dst_addr;
    sal_memcpy(to->src6_addr, from->src6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->dst6_addr, from->dst6_addr, sizeof(bcm_ip6_t));
    to->flow_label = from->flow_label;
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
    sal_memcpy(to->dst_mac, from->dst_mac, sizeof(bcm_mac_t));
    to->tpid = from->tpid;
    to->vlan_id = from->vlan_id;
    to->trill_src_name = from->trill_src_name;
    to->trill_dst_name = from->trill_dst_name;
    to->trill_hopcount = from->trill_hopcount;
    to->niv_src_vif = from->niv_src_vif;
    to->niv_dst_vif = from->niv_dst_vif;
    to->niv_flags = from->niv_flags;
    to->gre_protocol = from->gre_protocol;
    to->policer_id = from->policer_id;
    to->stat_id = from->stat_id;
    to->stat_id2 = from->stat_id2;
    to->encap_id = from->encap_id;
    to->tunnel_id = from->tunnel_id;
    to->span_id = from->span_id;
    to->pkt_prio = from->pkt_prio;
    to->sample_rate_dividend = from->sample_rate_dividend;
    to->sample_rate_divisor = from->sample_rate_divisor;
    to->int_pri = from->int_pri;
    to->etag_src_vid = from->etag_src_vid;
    to->etag_dst_vid = from->etag_dst_vid;
    to->udp_src_port = from->udp_src_port;
    to->udp_dst_port = from->udp_dst_port;
    to->egress_sample_rate_dividend = from->egress_sample_rate_dividend;
    to->egress_sample_rate_divisor = from->egress_sample_rate_divisor;
    to->recycle_context = from->recycle_context;
    to->packet_copy_size = from->packet_copy_size;
    to->egress_packet_copy_size = from->egress_packet_copy_size;
    to->packet_control_updates = from->packet_control_updates;
    to->rtag = from->rtag;
    to->df = from->df;
    to->truncate = from->truncate;
    to->template_id = from->template_id;
    to->observation_domain = from->observation_domain;
    to->is_recycle_strict_priority = from->is_recycle_strict_priority;
    sal_memcpy(to->ext_stat_id, from->ext_stat_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
    to->flags2 = from->flags2;
    to->vni = from->vni;
    to->gre_seq_number = from->gre_seq_number;
    to->erspan_header = from->erspan_header;
    to->initial_seq_number = from->initial_seq_number;
    to->meta_data_type = from->meta_data_type;
    to->meta_data = from->meta_data;
    to->ext_stat_valid = from->ext_stat_valid;
    sal_memcpy(to->ext_stat_type_id, from->ext_stat_type_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
}

/*
 * Function:
 *   _bcm_compat6516out_mirror_destination_t
 * Purpose:
 *   Convert the bcm_mirror_destination_t datatype from 6.5.16+ to
 *   <=6.5.16.
 * Parameters:
 *   from     - (IN) The 6.5.16+ version of the datatype.
 *   to       - (OUT) The <=6.5.16 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516out_mirror_destination_t(
    bcm_mirror_destination_t *from,
    bcm_compat6516_mirror_destination_t *to)
{
    to->mirror_dest_id = from->mirror_dest_id;
    to->flags = from->flags;
    to->gport = from->gport;
    to->version = from->version;
    to->tos = from->tos;
    to->ttl = from->ttl;
    to->src_addr = from->src_addr;
    to->dst_addr = from->dst_addr;
    sal_memcpy(to->src6_addr, from->src6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(to->dst6_addr, from->dst6_addr, sizeof(bcm_ip6_t));
    to->flow_label = from->flow_label;
    sal_memcpy(to->src_mac, from->src_mac, sizeof(bcm_mac_t));
    sal_memcpy(to->dst_mac, from->dst_mac, sizeof(bcm_mac_t));
    to->tpid = from->tpid;
    to->vlan_id = from->vlan_id;
    to->trill_src_name = from->trill_src_name;
    to->trill_dst_name = from->trill_dst_name;
    to->trill_hopcount = from->trill_hopcount;
    to->niv_src_vif = from->niv_src_vif;
    to->niv_dst_vif = from->niv_dst_vif;
    to->niv_flags = from->niv_flags;
    to->gre_protocol = from->gre_protocol;
    to->policer_id = from->policer_id;
    to->stat_id = from->stat_id;
    to->stat_id2 = from->stat_id2;
    to->encap_id = from->encap_id;
    to->tunnel_id = from->tunnel_id;
    to->span_id = from->span_id;
    to->pkt_prio = from->pkt_prio;
    to->sample_rate_dividend = from->sample_rate_dividend;
    to->sample_rate_divisor = from->sample_rate_divisor;
    to->int_pri = from->int_pri;
    to->etag_src_vid = from->etag_src_vid;
    to->etag_dst_vid = from->etag_dst_vid;
    to->udp_src_port = from->udp_src_port;
    to->udp_dst_port = from->udp_dst_port;
    to->egress_sample_rate_dividend = from->egress_sample_rate_dividend;
    to->egress_sample_rate_divisor = from->egress_sample_rate_divisor;
    to->recycle_context = from->recycle_context;
    to->packet_copy_size = from->packet_copy_size;
    to->egress_packet_copy_size = from->egress_packet_copy_size;
    to->packet_control_updates = from->packet_control_updates;
    to->rtag = from->rtag;
    to->df = from->df;
    to->truncate = from->truncate;
    to->template_id = from->template_id;
    to->observation_domain = from->observation_domain;
    to->is_recycle_strict_priority = from->is_recycle_strict_priority;
    sal_memcpy(to->ext_stat_id, from->ext_stat_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
    to->flags2 = from->flags2;
    to->vni = from->vni;
    to->gre_seq_number = from->gre_seq_number;
    to->erspan_header = from->erspan_header;
    to->initial_seq_number = from->initial_seq_number;
    to->meta_data_type = from->meta_data_type;
    to->meta_data = from->meta_data;
    to->ext_stat_valid = from->ext_stat_valid;
    sal_memcpy(to->ext_stat_type_id, from->ext_stat_type_id,
               sizeof(int) * BCM_MIRROR_EXT_STAT_ID_COUNT);
}

/*
 * Function:
 *   bcm_compat6516_mirror_destination_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_mirror_destination_create.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   mirror_dest    - (INOUT) Mirrored destination and encapsulation.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6516_mirror_destination_create(
    int unit,
    bcm_compat6516_mirror_destination_t *mirror_dest)
{
    int rv = 0;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (mirror_dest != NULL) {
        new_mirror_dest = (bcm_mirror_destination_t*)
            sal_alloc(sizeof(bcm_mirror_destination_t), "New destination");
        if (new_mirror_dest == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_mirror_destination_t(mirror_dest, new_mirror_dest);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_destination_create(unit, new_mirror_dest);

    if (rv >= 0) {
        _bcm_compat6516out_mirror_destination_t(new_mirror_dest, mirror_dest);
    }

    sal_free(new_mirror_dest);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_mirror_destination_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_mirror_destination_get.
 * Parameters:
 *   unit             - (IN) BCM device number.
 *   mirror_dest_id   - (IN) Mirrored destination ID.
 *   mirror_dest      - (INOUT) Matching Mirrored destination descriptor.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6516_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6516_mirror_destination_t *mirror_dest)
{
    int rv = 0;
    bcm_mirror_destination_t *new_mirror_dest = NULL;

    if (mirror_dest != NULL) {
        new_mirror_dest = (bcm_mirror_destination_t*)
            sal_alloc(sizeof(bcm_mirror_destination_t), "New destination");
        if (new_mirror_dest == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_mirror_destination_t(mirror_dest, new_mirror_dest);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_mirror_destination_get(unit, mirror_dest_id, new_mirror_dest);

    if (rv >= 0) {
        _bcm_compat6516out_mirror_destination_t(new_mirror_dest, mirror_dest);
    }

    sal_free(new_mirror_dest);
    return rv;
}

/*
 * Function:
 *   _bcm_compat6516in_qos_map_t
 * Purpose:
 *   Convert the bcm_qos_map_t datatype from <=6.5.16 to
 *   SDK 6.5.16+.
 * Parameters:
 *   from        - (IN) The <=6.5.16 version of the datatype.
 *   to          - (OUT) The SDK 6.5.16+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516in_qos_map_t(
    bcm_compat6516_qos_map_t *from,
    bcm_qos_map_t *to)
{
    bcm_qos_map_t_init(to);
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->dscp = from->dscp;
    to->exp = from->exp;
    to->int_pri = from->int_pri;
    to->color = from->color;
    to->remark_int_pri = from->remark_int_pri;
    to->remark_color = from->remark_color;
    to->policer_offset = from->policer_offset;
    to->queue_offset = from->queue_offset;
    to->port_offset = from->port_offset;
    to->etag_pcp = from->etag_pcp;
    to->etag_de = from->etag_de;
    to->counter_offset = from->counter_offset;
    to->inherited_dscp_exp = from->inherited_dscp_exp;
    to->opcode = from->opcode;
}

/*
 * Function:
 *   _bcm_compat6516out_qos_map_t
 * Purpose:
 *   Convert the bcm_qos_map_t datatype from 6.5.16+ to
 *   <=6.5.16.
 * Parameters:
 *   from     - (IN) The 6.5.16+ version of the datatype.
 *   to       - (OUT) The <=6.5.16 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516out_qos_map_t(
    bcm_qos_map_t *from,
    bcm_compat6516_qos_map_t *to)
{
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->dscp = from->dscp;
    to->exp = from->exp;
    to->int_pri = from->int_pri;
    to->color = from->color;
    to->remark_int_pri = from->remark_int_pri;
    to->remark_color = from->remark_color;
    to->policer_offset = from->policer_offset;
    to->queue_offset = from->queue_offset;
    to->port_offset = from->port_offset;
    to->etag_pcp = from->etag_pcp;
    to->etag_de = from->etag_de;
    to->counter_offset = from->counter_offset;
    to->inherited_dscp_exp = from->inherited_dscp_exp;
    to->opcode = from->opcode;
}

/*
 * Function:
 *   bcm_compat6516_qos_map_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_qos_map_add.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   flags          - (IN) Configuration flags.
 *   map            - (IN) Pointer to a qos map structure.
 *   map_id         - (IN) QoS map ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6516_qos_map_add(
    int unit,
    uint32 flags,
    bcm_compat6516_qos_map_t *map,
    int map_id)
{
    int rv = 0;
    bcm_qos_map_t *new_map = NULL;

    if (map != NULL) {
        new_map = (bcm_qos_map_t*)
            sal_alloc(sizeof(bcm_qos_map_t), "New map");
        if (new_map == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_qos_map_t(map, new_map);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_qos_map_add(unit, flags, new_map, map_id);

    if (rv >= 0) {
        _bcm_compat6516out_qos_map_t(new_map, map);
    }

    sal_free(new_map);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_qos_map_multi_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_qos_map_multi_get.
 * Parameters:
 *   unit             - (IN) BCM device number.
 *   flags            - (IN) Configuration flags.
 *   map_id           - (IN) QoS map ID.
 *   array_size       - (IN) Number of elements in array parameter; 0 to query.
 *   array            - (OUT) Storage location for QoS mapping.
 *   array_count      - (OUT) Number of mappings retrieved.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6516_qos_map_multi_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_compat6516_qos_map_t *array,
    int *array_count)
{
    int rv = 0, i;
    bcm_qos_map_t *new_array = NULL;

    if (array_size <= 0) {
        return bcm_qos_map_multi_get(unit, flags, map_id, array_size,
                                     (bcm_qos_map_t *)array, array_count);
    }

    if (array != NULL) {
        new_array = (bcm_qos_map_t*)
            sal_alloc(sizeof(bcm_qos_map_t) * array_size, "New array");
        if (new_array == NULL) {
            return BCM_E_MEMORY;
        }

        for (i = 0; i < array_size; i++) {
            _bcm_compat6516in_qos_map_t(&array[i], &new_array[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size,
                               new_array, array_count);

    if (rv >= 0) {
        for (i = 0; i < array_size; i++) {
            _bcm_compat6516out_qos_map_t(&new_array[i], &array[i]);
        }
    }

    sal_free(new_array);
    return rv;
}

/*
 * Function:
 *   bcm_compat6516_qos_map_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_qos_map_delete.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   flags          - (IN) Configuration flags.
 *   map            - (IN) Pointer to a qos map structure.
 *   map_id         - (IN) QoS map ID.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6516_qos_map_delete(
    int unit,
    uint32 flags,
    bcm_compat6516_qos_map_t *map,
    int map_id)
{
    int rv = 0;
    bcm_qos_map_t *new_map = NULL;

    if (map != NULL) {
        new_map = (bcm_qos_map_t*)
            sal_alloc(sizeof(bcm_qos_map_t), "New map");
        if (new_map == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6516in_qos_map_t(map, new_map);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_qos_map_delete(unit, flags, new_map, map_id);

    if (rv >= 0) {
        _bcm_compat6516out_qos_map_t(new_map, map);
    }

    sal_free(new_map);
    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6516in_mim_vpn_config_t
 * Purpose:
 *      Convert the bcm_mim_vpn_config_t datatype from <=6.5.16 to
 *      SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_mim_vpn_config_t(bcm_compat6516_mim_vpn_config_t *from,
                           bcm_mim_vpn_config_t *to)
{
    bcm_mim_vpn_config_t_init(to);
    to->flags = from->flags;
    to->vpn = from->vpn;
    to->lookup_id = from->lookup_id;
    to->match_service_tpid = from->match_service_tpid;
    to->broadcast_group = from->broadcast_group;
    to->unknown_unicast_group = from->unknown_unicast_group;
    to->unknown_multicast_group = from->unknown_multicast_group;
    to->policer_id = from->policer_id;
    to->service_encap_id = from->service_encap_id;
    to->int_pri = from->int_pri;
    to->qos_map_id = from->qos_map_id;
    to->tunnel_vlan = from->tunnel_vlan;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->protocol_pkt = from->protocol_pkt;
    to->egress_service_tpid = from->egress_service_tpid;
    sal_memcpy(to->match_service_smac, from->match_service_smac, sizeof(bcm_mac_t));
}

/*
 * Function:
 *      _bcm_compat6516out_mim_vpn_config_t
 * Purpose:
 *      Convert the bcm_mim_vpn_config_t datatype from 6.5.16+ to
 *      <=6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_mim_vpn_config_t(bcm_mim_vpn_config_t *from,
                            bcm_compat6516_mim_vpn_config_t *to)
{
    to->flags = from->flags;
    to->vpn = from->vpn;
    to->lookup_id = from->lookup_id;
    to->match_service_tpid = from->match_service_tpid;
    to->broadcast_group = from->broadcast_group;
    to->unknown_unicast_group = from->unknown_unicast_group;
    to->unknown_multicast_group = from->unknown_multicast_group;
    to->policer_id = from->policer_id;
    to->service_encap_id = from->service_encap_id;
    to->int_pri = from->int_pri;
    to->qos_map_id = from->qos_map_id;
    to->tunnel_vlan = from->tunnel_vlan;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->protocol_pkt = from->protocol_pkt;
    to->egress_service_tpid = from->egress_service_tpid;
    sal_memcpy(to->match_service_smac, from->match_service_smac, sizeof(bcm_mac_t));
}

/*
 * Function: bcm_compat6516_mim_vpn_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_mim_vpn_get.
 *
 * Parameters:
 *      unit     - (IN)Device Number
 *      l2vpn   - (IN)MIM VPN
 *      info     - (IN/OUT)MIM VPN Config
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6516_mim_vpn_get(int unit, bcm_vpn_t l2vpn,
                                     bcm_compat6516_mim_vpn_config_t *info)
{
    int rv;
    bcm_mim_vpn_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_mim_vpn_config_t *)
            sal_alloc(sizeof(bcm_mim_vpn_config_t), "New mim vpn config");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_mim_vpn_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_mim_vpn_get(unit, l2vpn, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_mim_vpn_config_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }
    return rv;
}

/*
 * Function: bcm_compat6516_mim_vpn_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_mim_vpn_create.
 *
 * Parameters:
 *      unit     - (IN)Device Number
 *      info     - (IN/OUT)MIM VPN Config
 * Returns:
 *      BCM_E_XXXX
 */
int
bcm_compat6516_mim_vpn_create(int unit,
                                         bcm_compat6516_mim_vpn_config_t *info)
{
    int rv;
    bcm_mim_vpn_config_t *new_info = NULL;

    if (NULL != info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_mim_vpn_config_t *)
            sal_alloc(sizeof(bcm_mim_vpn_config_t), "New mim vpn config");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_mim_vpn_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_mim_vpn_create(unit, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_mim_vpn_config_t(new_info, info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    return rv;
}
#endif  /* defined(INCLUDE_L3) */

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6516in_vxlan_port_t
 * Purpose:
 *      Convert the bcm_vxlan_port_t datatype from <=6.5.16 to
 *      SDK 6.5.16+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516in_vxlan_port_t(bcm_compat6516_vxlan_port_t *from,
                               bcm_vxlan_port_t *to)
{
    bcm_vxlan_port_t_init(to);

    to->vxlan_port_id = from->vxlan_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_service_vlan = from->egress_service_vlan;
    to->mtu = from->mtu;
    to->match_port = from->match_port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->egress_tunnel_id = from->egress_tunnel_id;
    to->match_tunnel_id = from->match_tunnel_id;
    to->egress_if = from->egress_if;
    to->network_group_id = from->network_group_id;
    to->vnid = from->vnid;
    to->qos_map_id = from->qos_map_id;
    to->tunnel_pkt_pri = from->tunnel_pkt_pri;
    to->tunnel_pkt_cfi = from->tunnel_pkt_cfi;
}

/*
 * Function:
 *      _bcm_compat6516out_vxlan_port_t
 * Purpose:
 *      Convert the bcm_vxlan_port_t datatype from 6.5.16+ to
 *      <=6.5.16
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6516out_vxlan_port_t(bcm_vxlan_port_t *from,
                                bcm_compat6516_vxlan_port_t *to)
{
    to->vxlan_port_id = from->vxlan_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_service_vlan = from->egress_service_vlan;
    to->mtu = from->mtu;
    to->match_port = from->match_port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->egress_tunnel_id = from->egress_tunnel_id;
    to->match_tunnel_id = from->match_tunnel_id;
    to->egress_if = from->egress_if;
    to->network_group_id = from->network_group_id;
    to->vnid = from->vnid;
    to->qos_map_id = from->qos_map_id;
    to->tunnel_pkt_pri = from->tunnel_pkt_pri;
    to->tunnel_pkt_cfi = from->tunnel_pkt_cfi;
}

/*
 * Function: bcm_compat6516_vxlan_port_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_port_add.
 *
 * Parameters:
 *      unit           - (IN) Device Number
 *      l2vpn          - (IN) VxLAN VPN
 *      vxlan_port     - (IN/OUT) VxLAN port info
 * Returns:
 *      BCM_E_XXXX
 */
int bcm_compat6516_vxlan_port_add(int unit, bcm_vpn_t l2vpn,
                                  bcm_compat6516_vxlan_port_t *vxlan_port)
{
    int rv;
    bcm_vxlan_port_t *new_info = NULL;

    if (NULL != vxlan_port) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_vxlan_port_t *)
            sal_alloc(sizeof(bcm_vxlan_port_t), "New VxLAN Port");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vxlan_port_t(vxlan_port, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_port_add(unit, l2vpn, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vxlan_port_t(new_info, vxlan_port);
        /* Deallocate memory*/
        sal_free(new_info);
    }
    return rv;
}

/*
 * Function: bcm_compat6516_vxlan_port_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_port_get.
 *
 * Parameters:
 *      unit     - Device Number
 *      l2vpn   - VXLAN VPN
 *      vxlan_port     - VXLAN Gport
 * Returns:
 *      BCM_E_XXXX
 */
int bcm_compat6516_vxlan_port_get(int unit, bcm_vpn_t l2vpn,
                                  bcm_compat6516_vxlan_port_t *vxlan_port)
{
    int rv;
    bcm_vxlan_port_t *new_info = NULL;

    if (NULL != vxlan_port) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_vxlan_port_t*)
            sal_alloc(sizeof(bcm_vxlan_port_t), "New VxLAN Port Info");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6516in_vxlan_port_t(vxlan_port, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_port_get(unit, l2vpn, new_info);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6516out_vxlan_port_t(new_info, vxlan_port);
        /* Deallocate memory*/
        sal_free(new_info);
    }
    return rv;
}

/*
 * Function: bcm_compat6516_vxlan_port_get_all
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_port_get_all.
 *
 * Parameters:
 *      unit     - (IN) Device Number
 *      l2vpn   - VXLAN VPN
 *      port_max   - (IN) Maximum number of VXLAN ports in array
 *      port_array - (OUT) Array of VXLAN ports
 *      port_count - (OUT) Number of VXLAN ports returned in array
 * Returns:
 *      BCM_E_XXXX
 */
int bcm_compat6516_vxlan_port_get_all(int unit, bcm_vpn_t l2vpn, int port_max,
                                      bcm_compat6516_vxlan_port_t *port_array,
                                      int *port_count)
{
    int rv;
    bcm_vxlan_port_t *new_info = NULL;
    int i;

    if ((port_max > 0) && (port_array != NULL)) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_vxlan_port_t *)
            sal_alloc(sizeof(bcm_vxlan_port_t) * port_max, "New VxLAN Port");
        if (NULL == new_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < port_max; i++) {
            _bcm_compat6516in_vxlan_port_t(&port_array[i], &new_info[i]);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_port_get_all(unit, l2vpn, port_max, new_info, port_count);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        for (i = 0; i < port_max; i++) {
            _bcm_compat6516out_vxlan_port_t(&new_info[i], &port_array[i]);
        }
        /* Deallocate memory*/
        sal_free(new_info);
    }
    return rv;
}

#endif  /* defined(INCLUDE_L3) */

/*
 * Function:
 *   _bcm_compat6516in_l2_ring_t
 * Purpose:
 *   Convert the bcm_l2_ring_t datatype from <=6.5.16 to
 *   SDK 6.5.16+
 * Parameters:
 *   from        - (IN) The <=6.5.16 version of the datatype
 *   to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516in_l2_ring_t(bcm_compat6516_l2_ring_t *from,
                            bcm_l2_ring_t *to)
{
    bcm_l2_ring_t_init(to);
    to->flags = from->flags;
    sal_memcpy(&to->vlan_vector, &from->vlan_vector, sizeof(bcm_vlan_vector_t));
    to->port0 = from->port0;
    to->port1 = from->port1;
    sal_memcpy(&to->vpn_vector, &from->vpn_vector, sizeof(bcm_vpn_vector_t));
}

/*
 * Function:
 *   _bcm_compat6516out_l2_ring_t
 * Purpose:
 *   Convert the bcm_l2_ring_t datatype from 6.5.16+ to
 *   <=6.5.16
 * Parameters:
 *   from        - (IN) The 6.5.16+ version of the datatype
 *   to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6516out_l2_ring_t(bcm_l2_ring_t *from,
                             bcm_compat6516_l2_ring_t *to)
{
    to->flags = from->flags;
    sal_memcpy(&to->vlan_vector, &from->vlan_vector, sizeof(bcm_vlan_vector_t));
    to->port0 = from->port0;
    to->port1 = from->port1;
    sal_memcpy(&to->vpn_vector, &from->vpn_vector, sizeof(bcm_vpn_vector_t));
}

/*
 * Function: bcm_compat6516_l2_ring_replace
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_ring_replace.
 *
 * Parameters:
 *      unit     - (IN) Device Number
 *      l2_ring  - (IN) l2_ring info
 * Returns:
 *      BCM_E_XXXX
 */
int bcm_compat6516_l2_ring_replace(int unit,
                                   bcm_compat6516_l2_ring_t *l2_ring)
{
    int rv = 0;
    bcm_l2_ring_t new_l2_ring;

    if (l2_ring == NULL) {
        return BCM_E_PARAM;
    }

    _bcm_compat6516in_l2_ring_t(l2_ring, &new_l2_ring);

    rv = bcm_l2_ring_replace(unit, &new_l2_ring);

    if (rv >= 0) {
        _bcm_compat6516out_l2_ring_t(&new_l2_ring, l2_ring);
    }

    return rv;
 }

#endif /* BCM_RPC_SUPPORT*/
