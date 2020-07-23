/**
 * \file algo_l3_egr_pointed.h
 *
 * This file contains the public APIs required for the tables
 * that map 'virtual egress pointed' objects to 'local out lif'
 * and vice versa.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef  INCLUDE_ALGO_L3_EGR_POINTED_H_INCLUDED
/*
 * {
 */
#define  INCLUDE_ALGO_L3_EGR_POINTED_H_INCLUDED

/*************
 * INCLUDES  *
 */
/*
 * {
 */
#include <shared/error.h>
/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */
/**
 *  \brief Illegal 'egr_pointed' id.
 *
 *  \see
 *    * dnx_algo_l3_local_lif_to_egr_pointed_get
 *    * dnx_algo_l3_egr_pointed_to_local_lif_get
 */
#define DNX_ALGO_EGR_POINTED_INVALID             (-1)
/**
 * }
 */

/*************
 * MACROS    *
 */
/*
 * {
 */
/**
 * \brief
 * Condition-only on whether egr_pointed_id is within range (1 - 2^19-1, including edges).
 */
#define DNX_EGR_POINTED_VALID(unit, egr_pointed_id)                   \
    (((unsigned int)egr_pointed_id <                                  \
     dnx_data_l3.egr_pointed.first_egr_pointed_id_get(unit)) ||       \
        ((unsigned int)egr_pointed_id >                               \
        (dnx_data_l3.egr_pointed.nof_egr_pointed_ids_get(unit) - 1)))
/**
 * \brief
 * Verify egr_pointed_id is within range (1 - 2^19-1, including edges).
 * Eject error if not.
 */
#define DNX_EGR_POINTED_CHK_ID(unit, egr_pointed_id)        \
    {                                                       \
        if (DNX_EGR_POINTED_VALID(unit, egr_pointed_id))    \
        {                                                                                         \
            SHR_ERR_EXIT(_SHR_E_PARAM,                                                            \
                "Invalid ID for EGR_POINTED object (%d): Must be between %d and %d\r\n",          \
                egr_pointed_id,                                                                   \
                dnx_data_l3.egr_pointed.first_egr_pointed_id_get(unit),                 \
                (dnx_data_l3.egr_pointed.nof_egr_pointed_ids_get(unit) - 1));           \
        }                                                                                         \
    }
/*
 * }
 */
/*************
 * TYPE DEFS *
 */
/*
 * {
 */
/**
 * \brief
 *   Internal type for EGR_POINTED object
 */
typedef uint32 dnx_egr_pointed_t;
/*
 * }
 */

/*************
 * GLOBALS
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

/******************************************************************
 * FUNCTIONS and definitions  related to EGR_POINTED objects      *
 */
/*
 * {
 */
/**
 * \brief
 *   Create mapping between the given local lif and egr_pointed object.
 *
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in] flags -
 *   Currently, not used
 * \param [in] egr_pointed_id -
 *   Pointer to memory containing egr_pointed object to be mapped.
 * \param [in] local_lif -
 *   Pointer to memory containing info of the local lif to be mapped.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 * \remark
 *   * Using egress egr_pointed to local and local to egr_pointed maps
 *   in the lif mapping sw state submodule.
 *   * As an exception, we use '_shr_error_t' instead of 'shr_error_e'
 *   since this file is included in gport.h which is included but
 *   many files and not all can digest shrextend_debug.h which
 *   includes the definition of 'shr_error_e'.
 */
_shr_error_t dnx_algo_l3_egr_pointed_local_lif_mapping_add(
    int unit,
    uint32 flags,
    dnx_egr_pointed_t egr_pointed_id,
    int local_lif);

/**
 * \brief
 *   Remove the mapping between the given local lif and EGR_POINTED object.
 *
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in] flags -
 *   Currently, not used.
 * \param [in] egr_pointed_id -
 *   EGR_POINTED object to be unmapped.
 * \return
 *   * retval Zero if no error was detected
 *   * retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * Using egress EGR_POINTED to local and local to EGR_POINTED maps
 *   in the lif mapping sw state submodule.
 *   * As an exception, we use '_shr_error_t' instead of 'shr_error_e'
 *   since this file is included in gport.h which is included but
 *   many files and not all can digest shrextend_debug.h which
 *   includes the definition of 'shr_error_e'.
 */
_shr_error_t dnx_algo_l3_egr_pointed_local_lif_mapping_remove(
    int unit,
    uint32 flags,
    dnx_egr_pointed_t egr_pointed_id);
/**
 * \brief
 *   Get the EGR_POINTED object mapped to this local lif.
 *  
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in] flags -
 *   Currently, not used.
 * \param [in] local_lif -
 *   The local lif whose mapping we require.
 * \param [out] egr_pointed_id_p -
 *   dnx_egr_pointed_t pointer to memory to write output into. \n
 *   \b As \b output - \n
 *     This procedure loads pointed memory with the EGR_POINTED object mapped
 *     from the given local lif.
 * \return
 *   * retval Zero if no error was detected
 *   * retval Negative if error was detected. See \ref shr_error_e
 *   * retval \ref BCM_E_NOT_FOUND if the global lif's mapping doesn't exist. 
 *      And, in that case, '*egr_pointed_id_p' is loaded by DNX_ALGO_EGR_POINTED_INVALID.
 * \remark
 *   * Using egress local to EGR_POINTED object maps in sw state.
 *   * As an exception, we use '_shr_error_t' instead of 'shr_error_e'
 *   since this file is included in gport.h which is included but
 *   many files and not all can digest shrextend_debug.h which
 *   includes the definition of 'shr_error_e'.
 */
_shr_error_t dnx_algo_l3_local_lif_to_egr_pointed_get(
    int unit,
    uint32 flags,
    int local_lif,
    dnx_egr_pointed_t * egr_pointed_id_p);
/**
 * \brief
 *   Get the local lif mapped to this EGR_POINTED object.
 *  
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in] flags -
 *   Currently, not used.
 * \param [in] egr_pointed_id -
 *   The egr_pointed object whose mapping we require.
 * \param [out] local_lif_p -
 *   int pointer to memory to write output into. \n
 *   \b As \b output - \n
 *   This procedure loads pointed memory with the local lif mapped
 *   from the given 'egress pointed' object.
 * \return
 *   * retval Zero if no error was detected
 *   * retval Negative if error was detected. See \ref shr_error_e
 *   * retval \ref BCM_E_NOT_FOUND if the global lif's mapping doesn't exist.
 *      And, in that case, '*local_lif_p' is loaded by DNX_ALGO_GPM_LIF_INVALID.
 * \remark
 *   * Using egress egr_pointed objects to local lif maps in sw state.
 *   * As an exception, we use '_shr_error_t' instead of 'shr_error_e'
 *   since this file is included in gport.h which is included but
 *   many files and not all can digest shrextend_debug.h which
 *   includes the definition of 'shr_error_e'.
 */
_shr_error_t dnx_algo_l3_egr_pointed_to_local_lif_get(
    int unit,
    int flags,
    dnx_egr_pointed_t egr_pointed_id,
    int *local_lif_p);
/**
 * \brief
 *   Initialization procedure for tables related to EGR_POINTED objects.
 *   Note that, currently, there is nothing to do.
 *  
 * \param [in] unit -
 *   Identifier of the device to access.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 */
_shr_error_t dnx_algo_l3_egr_pointed_tables_init(
    int unit);
/**
 * \brief
 *   De-initialization procedure for tables related to EGR_POINTED objects.
 *   Note that, currently, there is nothing to do.
 *  
 * \param [in] unit -
 *   Identifier of the device to access.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \remark
 *   * None
 */
_shr_error_t dnx_algo_l3_egr_pointed_tables_deinit(
    int unit);
/*
 * }
 */

/*
 * }
 */

/*
 * }
 */
#endif /* INCLUDE_ALGO_L3_EGR_POINTED_H_INCLUDED */
