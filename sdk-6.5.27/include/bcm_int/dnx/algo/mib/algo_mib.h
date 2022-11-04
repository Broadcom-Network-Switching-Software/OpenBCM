/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file algo_mib.h
 *
 *  Include:
 *      MIB DB initialization and deinitialization.
 *      MIB DB set and get functions.
 *
 */

#ifndef ALGO_MIB_H_INCLUDED
/*
 * {
 */
#define ALGO_MIB_H_INCLUDED

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
 * Init algo mib module for cool boot mode.
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
shr_error_e dnx_algo_mib_init(
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
 *  Set MIB port bitmap in SW DB.
 * 
 * \param [in] unit - unit #.
 * \param [in] counter_pbmp - counter port bitmap
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_mib_counter_pbmp_set(
    int unit,
    bcm_pbmp_t counter_pbmp);

/**
 * \brief - 
 *  Get MIB port bitmap in SW DB.
 * 
 * \param [in] unit - unit #.
 * \param [out] counter_pbmp - counter port bitmap.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_mib_counter_pbmp_get(
    int unit,
    bcm_pbmp_t * counter_pbmp);

/**
 * \brief - 
 *  Get MIB port bitmap in SW DB.
 * 
 * \param [in] unit - unit #.
 * \param [in] logical_port - logic port.
 * \param [in] enable - enable.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_mib_counter_pbmp_port_enable(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief - 
 *  Set MIB interval in SW DB.
 * 
 * \param [in] unit - unit #.
 * \param [in] interval - mib interval.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_mib_interval_set(
    int unit,
    int interval);

/**
 * \brief - 
 *  Get MIB interval in SW DB.
 * 
 * \param [in] unit - unit #.
 * \param [out] interval - mib interval.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_mib_interval_get(
    int unit,
    int *interval);
/*
 * }
 */

/*
 * }
 */
#endif /*_ALGO_SYNCE_H_INCLUDED_*/
