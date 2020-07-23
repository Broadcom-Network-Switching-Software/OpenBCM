/** \file bcm_int/dnx/trunk/trunk.h
 * 
 * Internal DNX TRUNK  APIs to be used from other modules 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_TRUNK_TRUNK_H_INCLUDED
/*
 * { 
 */
#define _BCMINT_DNX_TRUNK_TRUNK_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/trunk.h>

typedef struct dnx_trunk_unique_member_s
{
    uint32 system_port;
    int nof_replications;
    int is_enabled;
} dnx_trunk_unique_member_t;

/** prototype of trunk update CB function */
typedef shr_error_e(
    *dnx_trunk_update_cb) (
    int unit,
    bcm_trunk_t trunk_id,
    uint32 core,
    uint32 tm_port);

/** struct definition that contains the different CBs that are called when an action is performed. */
typedef struct dnx_trunk_feature_update_s
{
    /** CB that is called when creating a trunk */
    dnx_trunk_update_cb create_cb;
    /** CB that is called when destroying a trunk */
    dnx_trunk_update_cb destroy_cb;
    /** CB that is called when adding a member to a trunk */
    dnx_trunk_update_cb add_cb;
    /** CB that is called when deleting a member from a trunk */
    dnx_trunk_update_cb delete_cb;
} dnx_trunk_feature_update_t;

/** enum to represent the actions in the struct dnx_trunk_feature_update_t above */
typedef enum
{
    DNX_TRUNK_FEATURE_UPDATE_CB_CREATE,
    DNX_TRUNK_FEATURE_UPDATE_CB_DESTROY,
    DNX_TRUNK_FEATURE_UPDATE_CB_ADD,
    DNX_TRUNK_FEATURE_UPDATE_CB_DELETE,
} dnx_trunk_feature_update_cb_types_e;

/**
 * \brief - create an egress trunk
 * 
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id for which to create an egress 
 *        trunk
 * \param [in] egress_trunk_id - returned egress trunk that was 
 *        created
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_egress_create(
    int unit,
    bcm_trunk_t trunk_id,
    int *egress_trunk_id);

/**
 * \brief - destroy an egress trunk profile
 * 
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id whose profile to destroy
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_egress_destroy(
    int unit,
    bcm_trunk_t trunk_id);

/**
 * \brief - get the trunk id coresponding to the egress trunk id 
 *        received
 * 
 * \param [in] unit - unit number
 * \param [in] egress_trunk_id - source egress_trunk_id. find 
 *        the trunk ID coresponding to this egress trunk ID
 * \param [in] trunk_id - returned trunk id
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_containing_egress_trunk_get(
    int unit,
    int egress_trunk_id,
    bcm_trunk_t * trunk_id);

/**
 * \brief - get a trunk's pp port according to core
 * 
 * \param [in] unit - unit nubmer
 * \param [in] trunk_id - trunk id
 * \param [in] core - core index
 * \param [in] pp_port - returned pp port
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_pp_port_get(
    int unit,
    bcm_trunk_t trunk_id,
    int core,
    uint32 *pp_port);

/**
 * \brief - get local ports bitmap with trunk's members for 
 *        given core
 * 
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] core - core index
 * \param [out] local_port_bmp - result local port bit map
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_local_port_bmp_get(
    int unit,
    bcm_trunk_t trunk_id,
    int core,
    bcm_pbmp_t * local_port_bmp);

/**
 * \brief - when working in graceful lag modification mode, this 
 *        function flips between configurations.
 * 
 * \param [in] unit - unit nubmer
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_flip_active_configuration_selector(
    int unit);

/**
 * \brief - when working with graceful lag modification mode, 
 *        this function copies the active configuration to the
 *        standby configuration
 * 
 * \param [in] unit - unit nubmer
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_active_configuration_to_stand_by_configuration_copy(
    int unit);

/**
 * \brief - Function to check whether a system port is spa or just a physical system port.
 *
 * \param [in] unit - unit number
 * \param [in] system_port - system port to check.
 * \param [out] is_spa - will be filled with boolean indication of the system port
 *                          being a SPA
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_system_port_is_spa(
    int unit,
    int system_port,
    int *is_spa);

/**
 * \brief - function to return the system port aggregate id from trunk gport
 *
 * \param [in] unit - unit number
 * \param [in] trunk_gport - trunk gport
 * \param [in] member - trunk member
 * \param [out] spa - pointer for result spa
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_gport_to_spa(
    int unit,
    bcm_gport_t trunk_gport,
    int member,
    uint32 *spa);

/**
 * \brief - function to return the trunk gport id from system port aggregate
 *
 * \param [in] unit - unit number
 * \param [in] spa - spa, the system port aggregate
 * \param [out] member - trunk member (not available in gport)
 * \param [out] trunk_gport - pointer for result trunk gport
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_spa_to_gport(
    int unit,
    uint32 spa,
    int *member,
    bcm_gport_t * trunk_gport);

/**
 * \brief - mask the member part of given SPA (remaining parts are the pool and the group)
 *
 * \param [in] unit - unit number
 * \param [in] spa - spa
 * \param [out] spa_with_masked_member - spa with masked member
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_spa_member_mask(
    int unit,
    uint32 spa,
    uint32 *spa_with_masked_member);

/**
 * \brief - function to return the trunk gport id from system port aggregate
 *
 * \param [in] unit - unit number
 * \param [in] spa_id - spa_id, the system port aggregate id is defined as spa
 *                       without the bit responsible for defining the system port as spa.
 * \param [out] member - trunk member (not available in gport)
 * \param [out] trunk_gport - pointer for result trunk gport
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_spa_id_to_gport(
    int unit,
    uint32 spa_id,
    int *member,
    bcm_gport_t * trunk_gport);

/**
 * \brief - get from trunk's SW DB the header types configured to it
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [out] in_header_type - in direction header type
 * \param [out] out_header_type -  out direction header type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_header_types_get(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 *in_header_type,
    uint32 *out_header_type);

/**
 * \brief - set to trunk SW DB its header types
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] in_header_type - in direction header type
 * \param [in] out_header_type -  out direction header type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_header_types_set(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 in_header_type,
    uint32 out_header_type);

/**
 * \brief - check if trunk group is used
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool id
 * \param [in] group - group id
 * \param [out] is_used -  returned indication if trunk group is used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_is_used_get(
    int unit,
    uint32 pool,
    uint32 group,
    int *is_used);

/**
 * \brief - get an entry from trunk system port sw db
 * 
 * \param [in] unit - unit number
 * \param [in] system_port - (valid) system port
 * \param [in] trunk_id - ptr to returned trunk ID
 * \param [in] flags - ptr to returned flags
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_sw_db_trunk_system_port_db_get(
    int unit,
    uint32 system_port,
    bcm_trunk_t * trunk_id,
    uint32 *flags);

/**
 * \brief - Get the lsb and msb bits of the trunk members from trunk spa
 *
 * \param [in] unit - unit number
 * \param [in] spa - system port aggegrate
 * \param [in] members_lsb - least significant bit of trunk members bits encoded in spa
 * \param [in] members_msb - most significant bit of trunk members bits encoded in spa
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_trunk_utils_members_bits_get(
    int unit,
    uint32 spa,
    uint32 *members_lsb,
    uint32 *members_msb);
/*
 * }
 */
#endif /* _BCMINT_DNX_TRUNK_TRUNK_H_INCLUDED */
