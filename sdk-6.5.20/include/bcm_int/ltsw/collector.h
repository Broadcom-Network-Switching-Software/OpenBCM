/*! \file collector.h
 *
 * Collector module internal header file.
 * This file contains collector definitions internal
 * to the collector module itself.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMI_LTSW_COLLECTOR_H
#define BCMI_LTSW_COLLECTOR_H

#include <bcm/collector.h>

/******************************************************************************
 * Defines
 */

/*! Enum defines the transport types of collectors */
typedef enum bcm_ltsw_collector_transport_type_e{
    LTSW_COLLECTOR_TYPE_IPV4_UDP = 0,
    LTSW_COLLECTOR_TYPE_IPV6_UDP = 1,
} bcm_ltsw_collector_transport_type_t;

/* Internal enumeration for the users of common collector
 * infrastructure.
 */
typedef enum bcm_ltsw_collector_export_app_e {
    LTSW_COLLECTOR_EXPORT_APP_NONE = 0,
    LTSW_COLLECTOR_EXPORT_APP_FT_IPFIX = 1,
    LTSW_COLLECTOR_EXPORT_APP_FT_PROTOBUF = 2,
    LTSW_COLLECTOR_EXPORT_APP_ST_PROTOBUF = 3,
    LTSW_COLLECTOR_EXPORT_APP_IFA_IPFIX = 4,
} bcm_ltsw_collector_export_app_t;

/******************************************************************************
 * Function definitions
 */
/*
 * Function:
 *     bcmi_ltsw_collector_export_profile_ref_count_update
 * Purpose:
 *     Increment/Decrement reference count for an export profile
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Export profile Id
 *     update        - (IN) Reference count update (+/-)
 * Returns:
 *     BCM_E_XXX
 */
extern int 
bcmi_ltsw_collector_export_profile_ref_count_update(
    int unit, 
    int id, 
    int update);

/*
 * Function:
 *     bcmi_ltsw_collector_ref_count_update
 * Purpose:
 *     Increment/Decrement reference count for an export profile
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     update        - (IN) Reference count update (+/-)
 * Returns:
 *     BCM_E_XXX
 */
extern int 
bcmi_ltsw_collector_ref_count_update(
    int unit, 
    int id, 
    int update);


/*
 * Function:
 *     bcmi_ltsw_collector_user_update
 * Purpose:
 *     Add user for a collector.
 *     Currently one user is expected per collector
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     user          - (IN) The user type of calling app
 * Returns:
 *     BCM_E_XXX
 */
extern int
bcmi_ltsw_collector_user_update(
    int unit,
    bcm_collector_t id,
    bcm_ltsw_collector_export_app_t user);

/*
 * Function:
 *     bcmi_ltsw_collector_check_user_is_other
 * Purpose:
 *     Check if collector ID is used by other user.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     id            - (IN) Collector Id
 *     my_user       - (IN) The user type of calling app
 * Returns:
 *     BCM_E_XXX
 */
extern uint8
bcmi_ltsw_collector_check_user_is_other(
    int unit,
    bcm_collector_t id,
    bcm_ltsw_collector_export_app_t my_user);

/*
 * Function:
 *     bcmi_ltsw_collector_internal_id_get
 * Purpose:
 *     Get the internal ID (the ID that is programmed in LT) and the type of a
 *     collector.
 * Parameters:
 *     unit          - (IN)  BCM device number
 *     id            - (IN)  Collector Id
 *     internal_id   - (OUT) Internal ID of the collector.
 *     type          - (OUT) Type of the collector.
 * Returns:
 *     SHR_E_XXX
 */
extern int
bcmi_ltsw_collector_internal_id_get(int unit, bcm_collector_t id,
                                    uint32_t *internal_id,
                                    bcm_ltsw_collector_transport_type_t *type);

/*
 * Function:
 *     bcmi_ltsw_collector_external_id_get
 * Purpose:
 *     Get the external ID of a collector.
 * Parameters:
 *     unit          - (IN)  BCM device number
 *     internal_id   - (IN)  Collector internal ID.
 *     type          - (IN)  Collector type.
 *     id            - (OUT) External ID of the collector.
 * Returns:
 *     SHR_E_XXX
 */
extern int
bcmi_ltsw_collector_external_id_get(int unit, uint32_t internal_id,
                                    bcm_ltsw_collector_transport_type_t type,
                                    bcm_collector_t *id);
#endif






