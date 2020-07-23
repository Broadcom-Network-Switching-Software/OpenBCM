/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file lif_table_mngr_lib.h
 * 
 *
 * This file contains the public APIs required for lif table allocations HW configuration.
 */

#ifndef  INCLUDE_LIF_TABLE_MNGR_LIB_H_INCLUDED
#define  INCLUDE_LIF_TABLE_MNGR_LIB_H_INCLUDED

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
/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */
#define LIF_TABLE_MANAGER_INVALID_RESULT_TYPE   (-1)

/*
 * Shortcuts for DBAL action ENUMS for clarity
 *
 *  Set action is the commit expected behavior for fields that were supplied in the handle.
 *    SET_ACTION_NONE: Setting the field is irrelevant, action ignored
 *    SET_ACTION_USE_VALUE: The field's value is set to the HW
 *    SET_ACTION_DO_NOT_USE_VALUE: The field is used for result type selection, but its value
 *                                 isn't set to the HW (Another API is expected to set the value).
 *                                 The user is expected to enter the field's default value.
 *
 *  Unset action is the commit expected behavior for fields that weren't supplied in the handle.
 *    UNSET_ACTION_NONE: Fields that shouldn't be set by the user such as result_type
 *    UNSET_ACTION_FORBIDDEN: Must be present in the handle, otherwise an error is issued.
 *    UNSET_ACTION_IGNORE: If not supplied, the field isn't set to the HW
 *    UNSET_ACTION_SET_TO_DEFAULT: If not supplied, the field is set to its default value
 */
#define SET_ACTION_NONE                  DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_SET_ACTION_NONE
#define SET_ACTION_USE_VALUE             DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_SET_ACTION_USE_VALUE
#define SET_ACTION_DO_NOT_USE_VALUE      DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_SET_ACTION_DO_NOT_USE_VALUE
#define UNSET_ACTION_NONE                DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_UNSET_ACTION_NONE
#define UNSET_ACTION_FORBIDDEN           DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_UNSET_ACTION_FORBIDDEN
#define UNSET_ACTION_IGNORE              DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_UNSET_ACTION_IGNORE
#define UNSET_ACTION_SET_TO_DEFAULT      DBAL_ENUM_FVAL_LIF_TABLE_MANAGER_UNSET_ACTION_SET_TO_DEFAULT
#define NONE                             FALSE

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
 * \brief - Manage local lif creation and lif table management:
 *          1. Decide on result type according to set of fields that were set on the handle.
 *          2. If a result type with a parent field was chosen - If one of the parent's sub-field was set - get() API needs to get the sub-field id
 *          3. Allocate local LIF
 *          4. Fill HW LIF table
 *          5. Write to GLEM if required
 *          6. Add global to local SW mapping
 *
 *      For replace:
 *          1. Find the new result type if changed
 *          2. Allocate new local lif
 *          3. Create new lif entry in HW Lif table and copy old content into the new entry
 *          4. Change all EEDB pointers to point on the new lif
 *          5. Update GLEM if required
 *          6. Write the new content to the new entry
 *          7. Free old local lif entry
 *          8. Update global to local SW mapping
 *
 *   \param [in] unit - unit id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [out] local_lif_id - local outlif allocated
 *   \param [in] lif_info - struct containing the requirements for the new LIf and all the fields
 * \return DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_allocate_local_outlif_and_set_hw(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_outlif_info_t * lif_info);

/**
 * \brief - Manage local lif creation and lif table management:
 *      For new LIF:
 *          1. Decide on result type according to set of fields required
 *          2. If a result type with a parent field was chosen - If one of the parent's sub-field was set - get() API needs to get the sub-field id
 *          3. Allocate local LIF
 *          4. Fill HW LIF table
 *          5. Add global to local SW mapping
 *
 *      For replace:
 *          1. Find the new result type if changed
 *          2. Allocate new local lif
 *          3. For AC: Create new lif entry in HW Lif table and copy old content into the new entry
 *          4. For AC: change all ISEM pointers to point on the new lif
 *          5. Write the new content to the new entry
 *          6. Free old local lif entry
 *          7. Update global to local SW mapping
 *
 *   \param [in] unit - unit id
 *   \param [in] core_id - the id of the core
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [out] local_lif_id - local inlif allocated
 *   \param [in] lif_info - struct containing the requirements for the new LIf and all the fields
 * \return DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_allocate_local_inlif_and_set_hw(
    int unit,
    int core_id,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_inlif_info_t * lif_info);

/**
 * \brief - Get all fields
 *
 *   \param [in] unit - unit id
 *   \param [in] local_lif_id - local outlif to get
 *   \param [in] entry_handle_id - DBAL entry handle to fill.
 *                                Filled handle is of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [out] lif_info - struct containing lif information
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_get_outlif_info(
    int unit,
    int local_lif_id,
    uint32 entry_handle_id,
    lif_table_mngr_outlif_info_t * lif_info);

/**
 * \brief - Get all fields
 *
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [in] local_lif_id - local inlif to get
 *   \param [in] entry_handle_id - DBAL entry handle to fill.
 *                                Filled handle is of type DBAL_SUPERSET_RESULT_TYPE
 *   \param [out] lif_info - struct containing lif information
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_get_inlif_info(
    int unit,
    int core_id,
    int local_lif_id,
    uint32 entry_handle_id,
    lif_table_mngr_inlif_info_t * lif_info);

/**
 * \brief - Clear local LIF SW and HW allocation, and GLEM
 *
 *   \param [in] unit - unit id
 *   \param [in] local_lif_id - local outlif to clear
 *   \param [in] flags - Flags LIF_TABLE_MNGR_LIF_INFO_XXX
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_outlif_info_clear(
    int unit,
    int local_lif_id,
    uint32 flags);

/**
 * \brief - Clear local LIF SW and HW allocation
 *
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [in] local_lif_id - local inlif to clear
 *   \param [in] dbal_table_id - dbal table id to clear from
 *   \param [in] flags - Flags LIF_TABLE_MNGR_LIF_INFO_XXX
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_inlif_info_clear(
    int unit,
    int core_id,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint32 flags);

/**
 * \brief - Check if a given field exists on a given dabl handle.
 *
 *   \param [in] unit - unit id
 *   \param [in] entry_handle_id - dbal handle
 *   \param [in] field_id - required field
 *   \param [out] is_field_on_handle - indication if field is available
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 */
shr_error_e dnx_lif_table_mngr_is_field_exist_on_dbal_handle(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *is_field_on_handle);

/**
 * \brief - Check if a given field exists on a given dabl handle  and return it assuming field is uint32.
 *
 *   \param [in] unit - unit id
 *   \param [in] entry_handle_id - dbal handle
 *   \param [in] field_id - required field
 *   \param [out] is_field_on_handle - indication if field is available
 *   \param [out] field_val - field value
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 */
shr_error_e dnx_lif_table_mngr_uint32_field_on_dbal_handle_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *is_field_on_handle,
    uint32 *field_val);

/**
 * \brief - Check if a given field is valid for in/out lif entry.
 *          This is according to user specification on entry allocation.
 *          Field can be present in the result type but not enabled by the user.
 *
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for inlif)
 *   \param [in] local_lif_id - local inlif or outlif
 *   \param [in] dbal_table_id - dbal table id of the lif entry
 *   \param [in] is_ingress - indication if ingress
 *   \param [in] field_id - the field that will be checked
 *   \param [out] is_valid - indication if field is available
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 */
shr_error_e dnx_lif_table_mngr_is_valid_field(
    int unit,
    int core_id,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint8 is_ingress,
    dbal_fields_e field_id,
    uint8 *is_valid);

/**
 * \brief - Return for each table and field what action to perform in case of setting & unsetting the field.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] field_id - relevant field id
 *   \param [out] field_set_action - action to perform if field is set
 *   \param [out] field_unset_action - action to perform if field is unset
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
shr_error_e dnx_lif_table_mngr_field_set_actions_get(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 field_id,
    dbal_enum_value_field_lif_table_manager_set_action_e * field_set_action,
    dbal_enum_value_field_lif_table_manager_unset_action_e * field_unset_action);


/*
 * }
 */

#endif /* INCLUDE_LIF_MNGR_LIB_H_INCLUDED */
