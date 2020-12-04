/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file flow_lif_mgmt.h
 * 
 *
 * This file contains the public APIs required for lif table allocations HW configuration.
 */

#ifndef  INCLUDE_FLOW_LIF_MGMT_H_INCLUDED
#define  INCLUDE_FLOW_LIF_MGMT_H_INCLUDED

/*************
 * INCLUDES  *
 */
/*
 * {
 */

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm/flow.h>

/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */
#define FLOW_LIF_MGMT_INVALID_RESULT_TYPE   (-1)

/*
 * }
 */
/*************
 * MACROS    *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 */
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

/*
 * }
 */
/*************
 * GLOBALS   *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */

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
 *   \param [out] local_lif_id - local outlif allocated
 *   \param [in] lif_info - struct containing the requirements for the new LIf and all the fields
 * \return DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_flow_lif_mgmt_initiator_info_allocate_and_set_hw(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_outlif_info_t * lif_info);

/**
 * \brief - Manage FLOW Terminator local lif creation:
 *      For new LIF:
 *          1. Decide on result type according to the set of fields that were set on the handle.
 *          2. If a result type with a parent field was chosen - If one of the parent's sub-field was set - get() API needs to get the sub-field id
 *          3. Allocate a local LIF
 *          4. Fill the relevant HW LIF table
 *          5. Add global to local SW mapping
 *
 *      For replace:
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
 *   \param [in] core_id - the id of the core
 *   \param [in] flow_handle_info - FLOW handle that identifies the FLOW application
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [out] local_lif_id - local inlif allocated
 *   \param [in] lif_info - struct containing the requirements for the new LIf and all the fields
 * \return DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_flow_lif_mgmt_terminator_info_allocate_and_set_hw(
    int unit,
    int core_id,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_inlif_info_t * lif_info);

/*
 * }
 */

#endif /* INCLUDE_FLOW_LIF_MGMT_H_INCLUDED */
