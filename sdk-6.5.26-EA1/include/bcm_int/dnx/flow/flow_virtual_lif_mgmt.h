/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file flow_virtual_lif_mgmt.h
 * 
 *
 * This file contains the public APIs required for virtual lif configuration.
 *
 */

#ifndef  INCLUDE_FLOW_VIRTUAL_LIF_MGMT_H_INCLUDED
#define  INCLUDE_FLOW_VIRTUAL_LIF_MGMT_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dbal/dbal.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

#define DNX_FLOW_VIRTUAL_ID_INVALID             (-1)

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */
shr_error_e dnx_flow_egress_virtual_gport_allocate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    int is_local_lif);

shr_error_e dnx_flow_egress_virtual_gport_free(
    int unit,
    bcm_gport_t virtual_lif_gport);

/**
 * \brief - Add an entry to a SW table that maps a virtual gport 
 *        to the match keys of the entry and the selected
 *        result-type. The match keys and the result-type are
 *        taken from a DBAL handler.
 *  
 *   \param [in] unit - unit id
 *   \param [in] virtual_lif_gport - The gport of the added
 *          virtual LIF
 *   \param [in] entry_handle_id - The DBAL handler from which
 *          the match keys and the result-type are retrieved.
 *   \param [in] result_type - The result type selected for this entry.
 *   \param [in] flow_app_info - Pointer to the application definition info to determine the type of match.
 *  
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_flow_egress_virtual_sw_info_add(
    int unit,
    bcm_gport_t virtual_lif_gport,
    uint32 entry_handle_id,
    uint32 result_type,
    dnx_flow_app_config_t * flow_app_info);

/**
 * \brief - Retrieve the actual result-type of a virtual gport. 
 *        The value is taken from the SW table per virtual
 *        gport.
 *  
 *   \param [in] unit - unit id
 *   \param [in] virtual_lif_gport - The gport of the virtual LIF for which the result-type is retrieved.
 *   \param [out] table_id - The returned dbal_table.
 *   \param [out] result_type - The returned result-type.
 *  
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_flow_egress_virtual_sw_info_get(
    int unit,
    bcm_gport_t virtual_lif_gport,
    dbal_tables_e * table_id,
    uint32 *result_type);

shr_error_e dnx_flow_egress_virtual_sw_info_update(
    int unit,
    bcm_gport_t virtual_lif_gport,
    uint32 table_id,
    uint32 result_type);
/**
 * \brief - Retrieve the match keys of a virtual gport, from the
 *        SW table to the DBAL handler and return the retrieved
 *        result type.
 *  
 *   \param [in] unit - unit id
 *   \param [in] virtual_lif_gport - The gport of the retrieved
 *          virtual LIF
 *   \param [in] entry_handle_id - The DBAL handler that stores
 *          the match keys and the result-type.
   
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_flow_egress_virtual_key_fields_set(
    int unit,
    bcm_gport_t virtual_lif_gport,
    uint32 entry_handle_id);

/**
 * \brief - Clear an entry from a SW table that maps a virtual 
 *        gport to the match keys of the entry and the selected
 *        result-type.
 *  
 *   \param [in] unit - unit id
 *   \param [in] virtual_lif_gport - The gport of the virtual
 *          LIF for which the mapping is cleared.
 *  
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_flow_egress_virtual_match_clear(
    int unit,
    bcm_gport_t virtual_lif_gport);

shr_error_e dnx_flow_egress_virtual_gport_by_match_lookup(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_gport_t * virtual_lif_gport);

shr_error_e dnx_flow_egr_pointed_virtual_gport_get(
    int unit,
    int local_lif,
    uint32 *virtual_id);
shr_error_e dnx_flow_egr_pointed_virtual_mapping_remove(
    int unit,
    uint32 virtual_id);

shr_error_e dnx_flow_egr_pointed_virtual_mapping_add(
    int unit,
    uint32 virtual_id,
    int local_lif);

shr_error_e dnx_flow_egr_pointed_virtual_local_lif_get(
    int unit,
    uint32 virtual_id,
    int *local_lif_p);

shr_error_e dnx_flow_ingress_virtual_gport_allocate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info);

shr_error_e dnx_flow_ingress_virtual_gport_free(
    int unit,
    bcm_gport_t virtual_lif_gport);

shr_error_e dnx_flow_ingress_virtual_mapping_add(
    int unit,
    bcm_gport_t virtual_id,
    int nof_in_lifs,
    int *local_lifs);

shr_error_e dnx_flow_ingress_virtual_mapping_clear(
    int unit,
    bcm_gport_t virtual_id);

/**
 * \brief - Retrieve a terminator Virtual local LIF from a virtual gport and core-id using SW mapping
 *  
 *   \param [in] unit - unit id
 *   \param [in] virtual_id - The id from the virtual gport
 *   \param [in] core_idx - The core_id of the request local
 *          In-LIF
 *   \param [out] local_in_lif - The returned local In-LIF
 *  
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_flow_ingress_virtual_local_lif_get(
    int unit,
    int virtual_id,
    int core_idx,
    int *local_in_lif);

/**
 * \brief - Retrieve for a terminator Virtual an array of local LIFs from a specified virtual gport. 
 *          This API is intended for use when the local In-LIFs are configured per core.
 *          If the Virtual-ID isn't associated with a core, a value of LIF_MNGR_INLIF_INVALID is expected.
 *  
 *   \param [in] unit - unit id
 *   \param [in] virtual_id - The id from the virtual gport
 *   \param [out] local_in_lifs - The returned array of local In-LIFs
 *  
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_flow_ingress_virtual_local_lif_get_all(
    int unit,
    int virtual_id,
    inlif_array_t * local_in_lifs);

/**
 * \brief - Retrieve a terminator Virtual local gport from a virtual local LIF and core-id using SW mapping
 *  
 *   \param [in] unit - unit id
 *   \param [in] local_in_lif - The local In-LIF
 *   \param [in] core_idx - The core_id of the local In-LIF
 *   \param [out] virtual_id - The returned id from the virtual
 *          gport
 *  
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_flow_ingress_virtual_gport_get(
    int unit,
    int local_in_lif,
    int core_idx,
    int *virtual_id);

/*
 * }
 */

#endif /* INCLUDE_FLOW_VIRTUAL_LIF_MGMT_H_INCLUDED */
