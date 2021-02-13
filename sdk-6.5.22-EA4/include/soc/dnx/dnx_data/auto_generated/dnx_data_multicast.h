/** \file dnx_data_multicast.h
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
#ifndef _DNX_DATA_MULTICAST_H_
/*{*/
#define _DNX_DATA_MULTICAST_H_
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_multicast.h>
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
 * \brief Init default data structure - dnx_data_if_multicast
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
shr_error_e dnx_data_if_multicast_init(
    int unit);

/*
 * }
 */

/*
 * SUBMODULE  - PARAMS:
 * values for different multicast groups
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
     * Must be last one!
     */
    _dnx_data_multicast_params_feature_nof
} dnx_data_multicast_params_feature_e;

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
    *dnx_data_multicast_params_feature_get_f) (
    int unit,
    dnx_data_multicast_params_feature_e feature);

/**
 * \brief returns numeric data of max_ing_mc_groups
 * Module - 'multicast', Submodule - 'params', data - 'max_ing_mc_groups'
 * maximum ingress multicast groups id
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_ing_mc_groups - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef int(
    *dnx_data_multicast_params_max_ing_mc_groups_get_f) (
    int unit);

/**
 * \brief returns numeric data of max_egr_mc_groups
 * Module - 'multicast', Submodule - 'params', data - 'max_egr_mc_groups'
 * maximum egress multicast groups id
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_egr_mc_groups - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef int(
    *dnx_data_multicast_params_max_egr_mc_groups_get_f) (
    int unit);

/**
 * \brief returns numeric data of nof_ing_mc_bitmaps
 * Module - 'multicast', Submodule - 'params', data - 'nof_ing_mc_bitmaps'
 * number of multicast bitmap groups ids
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_ing_mc_bitmaps - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_multicast_params_nof_ing_mc_bitmaps_get_f) (
    int unit);

/**
 * \brief returns numeric data of nof_egr_mc_bitmaps
 * Module - 'multicast', Submodule - 'params', data - 'nof_egr_mc_bitmaps'
 * number of multicast bitmap groups ids
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_egr_mc_bitmaps - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_multicast_params_nof_egr_mc_bitmaps_get_f) (
    int unit);

/**
 * \brief returns numeric data of mcdb_allocation_method
 * Module - 'multicast', Submodule - 'params', data - 'mcdb_allocation_method'
 * The method of mcdb allocation
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     mcdb_allocation_method - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_multicast_params_mcdb_allocation_method_get_f) (
    int unit);

/**
 * \brief returns numeric data of nof_mcdb_entries_bits
 * Module - 'multicast', Submodule - 'params', data - 'nof_mcdb_entries_bits'
 * number of multicast entries in bits
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_mcdb_entries_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_multicast_params_nof_mcdb_entries_bits_get_f) (
    int unit);

/**
 * \brief returns define data of mcdb_block_size
 * Module - 'multicast', Submodule - 'params', data - 'mcdb_block_size'
 * number of multicast entries in block
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     mcdb_block_size - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_multicast_params_mcdb_block_size_get_f) (
    int unit);

/**
 * \brief returns define data of nof_mcdb_entries
 * Module - 'multicast', Submodule - 'params', data - 'nof_mcdb_entries'
 * number of multicast entries
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_mcdb_entries - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_multicast_params_nof_mcdb_entries_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_MULTICAST - PARAMS:
 * {
 */
/**
 * \brief Interface for multicast params data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_multicast_params_feature_get_f feature_get;
    /**
     * returns numeric data of max_ing_mc_groups
     */
    dnx_data_multicast_params_max_ing_mc_groups_get_f max_ing_mc_groups_get;
    /**
     * returns numeric data of max_egr_mc_groups
     */
    dnx_data_multicast_params_max_egr_mc_groups_get_f max_egr_mc_groups_get;
    /**
     * returns numeric data of nof_ing_mc_bitmaps
     */
    dnx_data_multicast_params_nof_ing_mc_bitmaps_get_f nof_ing_mc_bitmaps_get;
    /**
     * returns numeric data of nof_egr_mc_bitmaps
     */
    dnx_data_multicast_params_nof_egr_mc_bitmaps_get_f nof_egr_mc_bitmaps_get;
    /**
     * returns numeric data of mcdb_allocation_method
     */
    dnx_data_multicast_params_mcdb_allocation_method_get_f mcdb_allocation_method_get;
    /**
     * returns numeric data of nof_mcdb_entries_bits
     */
    dnx_data_multicast_params_nof_mcdb_entries_bits_get_f nof_mcdb_entries_bits_get;
    /**
     * returns define data of mcdb_block_size
     */
    dnx_data_multicast_params_mcdb_block_size_get_f mcdb_block_size_get;
    /**
     * returns define data of nof_mcdb_entries
     */
    dnx_data_multicast_params_nof_mcdb_entries_get_f nof_mcdb_entries_get;
} dnx_data_if_multicast_params_t;

/*
 * }
 */

/*
 * MODULE INTERFACE DNX_DATA_IF_MULTICAST:
 * {
 */
/**
 * \brief Interface for multicast data
 */
typedef struct
{
    /**
     * Interface for multicast params data
     */
    dnx_data_if_multicast_params_t params;
} dnx_data_if_multicast_t;

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
extern dnx_data_if_multicast_t dnx_data_multicast;
/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_MULTICAST_H_*/
/* *INDENT-ON* */
