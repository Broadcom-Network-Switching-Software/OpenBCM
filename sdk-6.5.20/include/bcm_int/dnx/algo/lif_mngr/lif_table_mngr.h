/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file lif_table_mngr.h
 * 
 *
 * This file contains the public APIs required for lif table algorithms.
 */

#ifndef  INCLUDE_LIF_TABLE_MNGR_H_INCLUDED
#define  INCLUDE_LIF_TABLE_MNGR_H_INCLUDED

/*************
 * INCLUDES  *
 */
/*
 * {
 */

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_table_mngr_access.h>

/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */
#define MAX_NUM_OF_LOOKUP_TABLES_PER_MODULE (20)
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
  * Enum defining which type of module is handled by LIF-Table-Mngr operation.
  * Should be mainly used to perform module specific operations that are to be
  * handled within the LIF-Table-Mngr operation sequence
  */
typedef enum
{
    LIF_TBL_MNGR_MODULE_INVALID = -1,
    LIF_TBL_MNGR_MODULE_UNRESOLVED,
    LIF_TBL_MNGR_MODULE_FIRST,
    LIF_TBL_MNGR_MODULE_VLAN_PORT = LIF_TBL_MNGR_MODULE_FIRST,
    LIF_TBL_MNGR_MODULE_PON,
    LIF_TBL_MNGR_MODULE_MPLS,
    LIF_TBL_MNGR_MODULE_L3_TUNNELS,
    LIF_TBL_MNGR_MODULE_EXTENDER,
    LIF_TBL_MNGR_MODULE_PPP,
    LIF_TBL_MNGR_MODULE_GTP,
    LIF_TBL_MNGR_MODULE_BIER,
    LIF_TBL_MNGR_MODULE_LAST = LIF_TBL_MNGR_MODULE_BIER,
    LIF_TBL_MNGR_MODULE_COUNT
} lif_table_mngr_module_e;
/*
 * {
 */
/**
 * \brief Specific rules per table.
 *
 * /see
 * dnx_lif_mngr_result_type_deduce
 */
typedef struct
{
    /**
     * Bitmap of forbidden field indices (in the order they appear on the SUPERSET result type) that cannot appear in the result type.
     * Array size should be equal to _SHR_BITDCLSIZE(dnx_data_lif.lif_table_manager.max_fields_per_table_get(0))
     */
    uint32 forbidden_fields_idx_bitmap[2];
    /**
     * Flags that are used for tables where advanced rules should be applied
     */
    uint32 allowed_result_types_bmp;
} lif_table_mngr_table_specific_rules_t;

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
 * \brief - Set specific rules for result type filtering per table.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - table id relevant to the current lif
 *   \param [in] result_type - Result type of the current lif
 *   \param [in] outlif_phase - Lif mngr outlif phase of the current lif, invalid for inlif
 *   \param [out] table_specific_flags - filled in this function
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_lif_table_mngr_table_specific_flags_fill(
    int unit,
    dbal_tables_e dbal_table_id,
    int result_type,
    uint32 outlif_phase,
    uint32 *table_specific_flags);

/**
 * \brief - Find best result type for given set of fields.
 * - Create a bitmap of available result types according to table_specific_flags
 * - For each result type X (starting from smaller and increasing)
 *   For each input field Y
 *     check if field Y exists in result type X
 *     If no - continue to next result type
 *     If yes - check the size of the field
 *        If the size is big enough continue to the next field
 *        Else continue to next result type
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] table_specific_flags - lif table specific information
 *   \param [in] outlif_phase - outlif logical phase
 *   \param [in] entry_handle_id - DBAL handle id containing all the fields
 *   \param [out] result_type_decision - selected result type of the current lif
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
shr_error_e dnx_algo_lif_table_mngr_decide_result_type(
    int unit,
    uint32 table_specific_flags,
    lif_mngr_outlif_phase_e outlif_phase,
    uint32 entry_handle_id,
    uint32 *result_type_decision);

/**
 * \brief - Perform DNX Algo initializations for the LIF table
 * manager module, like SW State table creations.
 * Should be done once on device init.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
shr_error_e dnx_algo_lif_table_mngr_init(
    int unit);

/*
 * }
 */

#endif /* INCLUDE_LIF_MNGR_H_INCLUDED */
