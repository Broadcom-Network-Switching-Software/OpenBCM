/** \file dnx_data_pvt.h
 * 
 * MODULE DATA INTERFACE - 
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
#ifndef _DNX_DATA_PVT_H_
/*{*/
#define _DNX_DATA_PVT_H_
/*
 * INCLUDE FILES:
 * {
 */
#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_pvt.h>
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
 * MODULE FUNCTIONS:
 * {
 */
/**
 * \brief Init default data structure - dnx_data_if_pvt
 * 
 * \param [in] unit - Unit #
 * 
 * \return
 *     rv - see 'shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnx_data_if_pvt_init(
    int unit);

/*
 * }
 */

/*
 * SUBMODULE  - GENERAL:
 * PVT general attributes
 * {
 */
/*
 * Table value structure
 */
/*
 * Feature enum
 */
/**
 * \brief 
 * Submodule features
 */
typedef enum
{
    /**
     * Indicate if PVT temperature monitors exist
     */
    dnx_data_pvt_general_pvt_temp_monitors_hw_support,
    /**
     * Indicate if support PVT peak clear
     */
    dnx_data_pvt_general_pvt_peak_clear_support,
    /**
     * Indicate if PVT peak have faulty
     */
    dnx_data_pvt_general_pvt_peak_faulty,

    /**
     * Must be last one!
     */
    _dnx_data_pvt_general_feature_nof
} dnx_data_pvt_general_feature_e;

/*
 * Submodule functions typedefs
 */
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
typedef int(
    *dnx_data_pvt_general_feature_get_f) (
    int unit,
    dnx_data_pvt_general_feature_e feature);

/**
 * \brief returns define data of nof_pvt_monitors
 * Module - 'pvt', Submodule - 'general', data - 'nof_pvt_monitors'
 * Number of device PVT monitors
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_pvt_monitors - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_pvt_general_nof_pvt_monitors_get_f) (
    int unit);

/**
 * \brief returns define data of pvt_base
 * Module - 'pvt', Submodule - 'general', data - 'pvt_base'
 * PVT base. Used for correct calculation of the monitor readings
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     pvt_base - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_pvt_general_pvt_base_get_f) (
    int unit);

/**
 * \brief returns define data of pvt_factor
 * Module - 'pvt', Submodule - 'general', data - 'pvt_factor'
 * PVT factor Used for correct calculation of the monitor readings
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     pvt_factor - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_pvt_general_pvt_factor_get_f) (
    int unit);

/**
 * \brief returns define data of pvt_mon_control_sel_nof_bits
 * Module - 'pvt', Submodule - 'general', data - 'pvt_mon_control_sel_nof_bits'
 * Number of bits in the field PVT_MON_CONTROL_SEL
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     pvt_mon_control_sel_nof_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_pvt_general_pvt_mon_control_sel_nof_bits_get_f) (
    int unit);

/**
 * \brief returns define data of thermal_data_nof_bits
 * Module - 'pvt', Submodule - 'general', data - 'thermal_data_nof_bits'
 * Number of bits in the field THERMAL_DATA
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     thermal_data_nof_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_pvt_general_thermal_data_nof_bits_get_f) (
    int unit);

/**
 * \brief returns define data of vol_sel
 * Module - 'pvt', Submodule - 'general', data - 'vol_sel'
 * select voltage value
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     vol_sel - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_pvt_general_vol_sel_get_f) (
    int unit);

/**
 * \brief returns define data of vol_factor
 * Module - 'pvt', Submodule - 'general', data - 'vol_factor'
 * voltage calculation base value
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     vol_factor - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_pvt_general_vol_factor_get_f) (
    int unit);

/**
 * \brief returns define data of vol_multiple
 * Module - 'pvt', Submodule - 'general', data - 'vol_multiple'
 * voltage calculation multiple value
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     vol_multiple - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_pvt_general_vol_multiple_get_f) (
    int unit);

/**
 * \brief returns define data of vol_divisor
 * Module - 'pvt', Submodule - 'general', data - 'vol_divisor'
 * voltage calculation divisor
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     vol_divisor - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_pvt_general_vol_divisor_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_PVT - GENERAL:
 * {
 */
/**
 * \brief Interface for pvt general data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_pvt_general_feature_get_f feature_get;
    /**
     * returns define data of nof_pvt_monitors
     */
    dnx_data_pvt_general_nof_pvt_monitors_get_f nof_pvt_monitors_get;
    /**
     * returns define data of pvt_base
     */
    dnx_data_pvt_general_pvt_base_get_f pvt_base_get;
    /**
     * returns define data of pvt_factor
     */
    dnx_data_pvt_general_pvt_factor_get_f pvt_factor_get;
    /**
     * returns define data of pvt_mon_control_sel_nof_bits
     */
    dnx_data_pvt_general_pvt_mon_control_sel_nof_bits_get_f pvt_mon_control_sel_nof_bits_get;
    /**
     * returns define data of thermal_data_nof_bits
     */
    dnx_data_pvt_general_thermal_data_nof_bits_get_f thermal_data_nof_bits_get;
    /**
     * returns define data of vol_sel
     */
    dnx_data_pvt_general_vol_sel_get_f vol_sel_get;
    /**
     * returns define data of vol_factor
     */
    dnx_data_pvt_general_vol_factor_get_f vol_factor_get;
    /**
     * returns define data of vol_multiple
     */
    dnx_data_pvt_general_vol_multiple_get_f vol_multiple_get;
    /**
     * returns define data of vol_divisor
     */
    dnx_data_pvt_general_vol_divisor_get_f vol_divisor_get;
} dnx_data_if_pvt_general_t;

/*
 * }
 */

/*
 * MODULE INTERFACE DNX_DATA_IF_PVT:
 * {
 */
/**
 * \brief Interface for pvt data
 */
typedef struct
{
    /**
     * Interface for pvt general data
     */
    dnx_data_if_pvt_general_t general;
} dnx_data_if_pvt_t;

/*
 * }
 */
/*
 * Extern:
 * {
 */
/**
 * \brief
 * Module structured interface - used to get the required data stored in dnx data
 */
extern dnx_data_if_pvt_t dnx_data_pvt;
/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_PVT_H_*/
/* *INDENT-ON* */
