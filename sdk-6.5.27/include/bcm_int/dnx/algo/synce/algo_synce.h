/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file algo_synce.h
 *
 *  Include:
 *      SyncE DB initialization and deinitialization.
 *      SyncE DB set and get functions.
 *
 */

#ifndef ALGO_SYNCE_H_INCLUDED
/*
 * {
 */
#define ALGO_SYNCE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Includes.
 * {
 */
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/*
 * }
 */
/*
 * Macros.
 * {
 */
/*
 * }
 */
/*
 * TypeDefs.
 * {
 */

/*
 * Module Init / Deinit 
 * { 
 */
/**
 * \brief - 
 * Init algo synce module for cool boot mode.
 *
 * \param [in] unit - Unit #.
 * 
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_synce_init(
    int unit);
/*
 * }
 */

/*
 * }
 */

/*
 * synce Set/Get functions
 * {
 */
/**
 * \brief - 
 *  Set Synce type src port belong to which core_group in SW DB.
 * 
 * \param [in] unit - unit #.
 * \param [in] synce_type - Master or slave synce.
 * \param [out] core_group - core_group.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_synce_type_core_group_id_set(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    uint32 core_group);

/**
 * \brief - 
 *  Get Synce type src port belong to which core_group in SW DB.
 * 
 * \param [in] unit - unit #.
 * \param [in] synce_type - Master or slave synce.
 * \param [out] core_group - core_group.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_synce_type_core_group_id_get(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    uint32 *core_group);

/**
 * \brief - 
 *  Set Fabric synce enable status in SW DB.
 * 
 * \param [in] unit - unit #.
 * \param [in] synce_type - Master or slave synce.
 * \param [in] enable - Enable or disable.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_fabric_synce_enable_set(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    int enable);
/**
 * \brief - 
 *  Get Fabric synce enable status in SW DB.
 * 
 * \param [in] unit - unit #.
 * \param [in] synce_type - Master or slave synce.
 * \param [out] enable - Enable or disable.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_fabric_synce_enable_get(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    int *enable);

/**
 * \brief - 
 *  Set NIF synce enable status in SW DB.
 * 
 * \param [in] unit - unit #.
 * \param [in] synce_type - Master or slave synce.
 * \param [in] enable - Enable or disable.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_nif_synce_enable_set(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    int enable);
/**
 * \brief - 
 *  Get NIF synce enable status in SW DB.
 * 
 * \param [in] unit - unit #.
 * \param [in] synce_type - Master or slave synce.
 * \param [out] enable - Enable or disable.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_nif_synce_enable_get(
    int unit,
    dbal_enum_value_field_synce_type_e synce_type,
    int *enable);
/*
 * }
 */

/*
 * }
 */
#endif /*_ALGO_SYNCE_H_INCLUDED_*/
