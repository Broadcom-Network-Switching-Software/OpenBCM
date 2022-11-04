/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file flow_lif_mgmt.h
 * 
 *
 * This file contains the public APIs required for lif table allocations HW configuration.
 *
 */

#ifndef  INCLUDE_FLOW_LIF_MGMT_H_INCLUDED
#define  INCLUDE_FLOW_LIF_MGMT_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/flow/flow.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */
#define FLOW_LIF_MGMT_INVALID_RESULT_TYPE   (-1)
#define FIELD_NOT_EXISTS_IN_RT              (-1)

/** learn info defines */
#define DNX_FLOW_LEARN_MGMT_INFO_UIN32_ARR_MAX_SIZE (2)

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
  * Enum defining which type of module is handled by FLOW-LIF-Mgmt operation.
  * Should be mainly used to perform module specific operations that are to be
  * handled within the FLOW-LIF-Mgmt operation sequence
  */
typedef enum
{
    FLOW_LIF_MGMT_MODULE_INVALID = -1,
    FLOW_LIF_MGMT_MODULE_UNRESOLVED,
    FLOW_LIF_MGMT_MODULE_FIRST,
    FLOW_LIF_MGMT_MODULE_VLAN_PORT = FLOW_LIF_MGMT_MODULE_FIRST,
    FLOW_LIF_MGMT_MODULE_PON,
    FLOW_LIF_MGMT_MODULE_MPLS,
    FLOW_LIF_MGMT_MODULE_L3_TUNNELS,
    FLOW_LIF_MGMT_MODULE_EXTENDER,
    FLOW_LIF_MGMT_MODULE_PPP,
    FLOW_LIF_MGMT_MODULE_GTP,
    FLOW_LIF_MGMT_MODULE_BIER,
    FLOW_LIF_MGMT_MODULE_LAST = FLOW_LIF_MGMT_MODULE_BIER,
    FLOW_LIF_MGMT_MODULE_COUNT
} flow_lif_mgmt_module_e;

/** learn internal flow structure. Utilized only for applications with full learn support */
typedef struct
{
    int is_extended_learn;
    int is_large_group;
    int is_symmetric_learn;
    int is_optimized_learn;
    dnx_algo_gpm_forward_info_t forward_info;
    dbal_enum_value_field_learn_payload_context_e learn_context;
} flow_learn_mgmt_l2_learn_data_t;

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
/** for a given gport, validate if it is allocated. gport can be translated to either global_lif or virtual ID */
shr_error_e flow_lif_mgmt_is_gport_allocated(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    uint8 *is_allocated);

/**
 * \brief - Perform DNX related FLOW LIF Mgmt initializations:
 * Initialize a mapping table between the field index in a
 * superset to field index in all the result types where this
 * field or its subfields participate.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_flow_init
 */
shr_error_e dnx_flow_lif_mgmt_init(
    int unit);

/**
 * \brief - Perform DNX related FLOW LIF Mgmt
 * de-initializations: Free the allocation of a mapping table
 * between the field index in a superset to field index in all
 * the result types where this field or its subfields
 * participate.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_flow_deinit
 */
shr_error_e dnx_flow_lif_mgmt_deinit(
    int unit);

/**
 * \brief - Manage FLOW Initiator local lif creation:
 *          1. Decide on result type according to the set of fields that were set on the handle.
 *          2. If a result type with a parent field was chosen - If one of the parent's sub-field was set - get() API needs to get the sub-field id
 *          3. Allocate a local LIF
 *          4. Fill the relevant HW LIF table
 *          5. Write to GLEM if required
 *          6. Add global to local SW mapping
 *
 *      For replace:
 *          1. Find a new result type if required
 *          2. Allocate the new local lif
 *          3. Create a new lif entry in HW Lif table and copy old content into the new entry
 *          4. Change all EEDB pointers to point to the new lif
 *          5. Update the GLEM if required
 *          6. Write the new content to the new entry
 *          7. Free old local lif entry
 *          8. Update global to local SW mapping
 *
 *   \param [in] unit - unit id
 *   \param [in] flow_handle_info - FLOW handle that identifies the FLOW application
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [in] logical_phase - LIF phase (egress)
 *   \param [in] selectable_result_types - A bitmap of the
 *          selectable result types or zero when all the result
 *          types are selectable.
 * \return DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_flow_lif_mgmt_initiator_info_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    uint32 selectable_result_types);

shr_error_e dnx_flow_lif_mgmt_initiator_info_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id,
    uint32 prev_entry_handle_id,
    uint32 selectable_result_types);

shr_error_e dnx_flow_lif_mgmt_terminator_info_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id);

/**
 * \brief - Manage FLOW Terminator local lif replace:
 *          1. Find a new result type if required
 *          2. Allocate the new local lif
 *          3. For AC: Create new lif entry in HW Lif table and copy old content into the new entry
 *          4. For AC: change all ISEM pointers to point to the 
 *             new lif
 *          5. Write the new content to the new entry
 *          6. Free old local lif entry
 *          7. Update global to local SW mapping
 *
 *   \param [in] unit - unit id
 *   \param [in] flow_handle_info - FLOW handle that identifies the FLOW application
 *   \param [in] terminator_info - terminator coomon fields info
 *          including l2_learn_info
 *   \param [in] gport_hw_resources - Lif related SW info
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [in] prev_entry_handle_id - DBAL entry handle for a
 *          Get operation that preceeds the set operation.
 *          The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [in] selectable_result_types - A bitmap of the
 *          selectable result types or zero when all the result
 *          types are selectable.
 * \return DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_flow_lif_mgmt_terminator_info_replace(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id,
    uint32 prev_entry_handle_id,
    uint32 selectable_result_types);

/**
 * \brief - Manage FLOW Terminator local lif create:
 *          1. Decide on result type according to the set of fields that were set on the handle.
 *          2. If a result type with a parent field was chosen - If one of the parent's sub-field was set - get() API needs to get the sub-field id
 *          3. Allocate a local LIF
 *          4. Fill the relevant HW LIF table
 *          5. Add global to local SW mapping
 *   \param [in] unit - unit id
 *   \param [in] flow_handle_info - FLOW handle that identifies the FLOW application
 *   \param [in] terminator_info - terminator coomon fields info
 *          including l2_learn_info
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [in] selectable_result_types - A bitmap of the
 *          selectable result types or zero when all the result
 *          types are selectable.
 * \return DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_flow_lif_mgmt_terminator_info_create(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    bcm_flow_terminator_info_t * terminator_info,
    uint32 entry_handle_id,
    uint32 selectable_result_types);

shr_error_e dnx_flow_lif_mgmt_terminator_info_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources);

shr_error_e dnx_flow_lif_mgmt_initiator_info_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info);

shr_error_e dnx_flow_lif_mgmt_initiator_info_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id);

/** Check whether a specific field was set for a local LIF / Virtual gport. A field was set == true if it was previously
 *  set into HW for this LIF/gport using lif tbl mngr. */
shr_error_e dnx_flow_lif_mgmt_field_is_set_in_hw(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dbal_fields_e field_id,
    uint8 *is_valid);

shr_error_e dnx_flow_lif_mgmt_valid_fields_clear(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info);

/**
 * \brief - Get the values of valid field from the src_entry_handle_id and copy them to the dst_entry_handle_id
 *   \param [in] unit - unit id
 *   \param [in] flow_app_info - FLOW per application
 *          definitions
 *   \param [in] flow_handle_info - FLOW handle that identifies the FLOW application
 *   \param [in] gport_hw_resources - gport DBAL HW resources
 *   \param [in] src_entry_handle_id - DBAL entry handle to get
 *          the fields from.
 *   \param [in] dst_entry_handle_id - DBAL entry handle to fill
 *          only with flow valid fields.
 * \return DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e dnx_flow_lif_mgmt_info_from_entry_handle_get(
    int unit,
    dnx_flow_app_config_t * flow_app_info,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 src_entry_handle_id,
    uint32 dst_entry_handle_id);

/*
 * }
 */

#endif /* INCLUDE_FLOW_LIF_MGMT_H_INCLUDED */
