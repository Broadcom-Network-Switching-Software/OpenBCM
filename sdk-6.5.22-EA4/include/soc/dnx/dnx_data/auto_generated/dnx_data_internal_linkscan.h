/** \file dnx_data_internal_linkscan.h
 * 
 * MODULE DEVICE DATA - DNX_DATA
 * 
 * Device Data
 * SW component that maintains per device data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 *        
 *     
 * 
 * AUTO-GENERATED BY AUTOCODER!
 * DO NOT EDIT THIS FILE!
 */
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_LINKSCAN_H_
/*{*/
#define _DNX_DATA_INTERNAL_LINKSCAN_H_
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_linkscan.h>
/*
 * }
 */

/*!
* \brief This file is only used by DNX (JR2 family). Including it by
* software that is not specific to DNX is an error.
*/
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * TYPEDEFS:
 * {
 */
/**
 * \brief 
 * Submodule enum
 */
typedef enum
{
    dnx_data_linkscan_submodule_general,

    /**
     * Must be last one!
     */
    _dnx_data_linkscan_submodule_nof
} dnx_data_linkscan_submodule_e;

/*
 * }
 */

/*
 * SUBMODULE GENERAL:
 * {
 */
/*
 * Features
 */
/* Feature enum - placed in if header (should be exposed to component user */
/* Get Data */
/**
 * \brief get submodule feature - disabled/enabled
 * 
 * \param [in] unit - Unit #
 * \param [in] feature - feature to get (see enum definition for more details
 * 
 * \return
 *     is_supported - returns 1 if and only if supported and 0 otherwise
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
int dnx_data_linkscan_general_feature_get(
    int unit,
    dnx_data_linkscan_general_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_linkscan_general_define_error_delay,
    dnx_data_linkscan_general_define_max_error,
    dnx_data_linkscan_general_define_thread_priority,
    dnx_data_linkscan_general_define_m0_pause_enable,

    /**
     * Must be last one!
     */
    _dnx_data_linkscan_general_define_nof
} dnx_data_linkscan_general_define_e;

/* Get Data */
/**
 * \brief returns numeric data of error_delay
 * Module - 'linkscan', Submodule - 'general', data - 'error_delay'
 * the amount of time in microseconds for which the bcm_linkscan module will suspend a port from further update processing after 'max_error' errors are detected.  After this delay, the error state for the port is cleared and normal linkscan processing resumes on the port
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     error_delay - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_linkscan_general_error_delay_get(
    int unit);

/**
 * \brief returns numeric data of max_error
 * Module - 'linkscan', Submodule - 'general', data - 'max_error'
 * The number of port update errors which will cause the bcm_linkscan module to suspend a port from update processing for the period of time set in 'error_delay'
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_error - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_linkscan_general_max_error_get(
    int unit);

/**
 * \brief returns numeric data of thread_priority
 * Module - 'linkscan', Submodule - 'general', data - 'thread_priority'
 * specifies the priority of the BCM Linkscan thread
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     thread_priority - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_linkscan_general_thread_priority_get(
    int unit);

/**
 * \brief returns numeric data of m0_pause_enable
 * Module - 'linkscan', Submodule - 'general', data - 'm0_pause_enable'
 * specifies the pause m0 firmware at Linkscan thread
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     m0_pause_enable - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_linkscan_general_m0_pause_enable_get(
    int unit);

/*
 * Tables
 */
/**
 * \brief 
 * Table enum
 */
typedef enum
{

    /**
     * Must be last one!
     */
    _dnx_data_linkscan_general_table_nof
} dnx_data_linkscan_general_table_e;

/* Get Data */
/* Get Value Str */
/* Get Table info */
/*
 * FUNCTIONS:
 * {
 */
/**
 * \brief Init module
 * 
 * \param [in] unit - Unit #
 * \param [out] module_data - pointer to module data
 * 
 * \return
 *     rv - see 'shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnx_data_linkscan_init(
    int unit,
    dnxc_data_module_t *module_data);

/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_INTERNAL_LINKSCAN_H_*/
/* *INDENT-ON* */
