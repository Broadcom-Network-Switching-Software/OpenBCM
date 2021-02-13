/** \file dnx_data_internal_utilex.h
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
#ifndef _DNX_DATA_INTERNAL_UTILEX_H_
/*{*/
#define _DNX_DATA_INTERNAL_UTILEX_H_
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_utilex.h>
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
    dnx_data_utilex_submodule_common,

    /**
     * Must be last one!
     */
    _dnx_data_utilex_submodule_nof
} dnx_data_utilex_submodule_e;

/*
 * }
 */

/*
 * SUBMODULE COMMON:
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
int dnx_data_utilex_common_feature_get(
    int unit,
    dnx_data_utilex_common_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_utilex_common_define_max_nof_dss_per_core,
    dnx_data_utilex_common_define_max_nof_hashs_per_core,
    dnx_data_utilex_common_define_max_nof_lists_per_core,
    dnx_data_utilex_common_define_max_nof_multis_per_core,
    dnx_data_utilex_common_define_max_nof_res_tag_bitmaps_per_core,
    dnx_data_utilex_common_define_max_nof_defragmented_chunks_per_core,

    /**
     * Must be last one!
     */
    _dnx_data_utilex_common_define_nof
} dnx_data_utilex_common_define_e;

/* Get Data */
/**
 * \brief returns define data of max_nof_dss_per_core
 * Module - 'utilex', Submodule - 'common', data - 'max_nof_dss_per_core'
 * Maximal number of bit map arrays to assign at init.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_nof_dss_per_core - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_utilex_common_max_nof_dss_per_core_get(
    int unit);

/**
 * \brief returns define data of max_nof_hashs_per_core
 * Module - 'utilex', Submodule - 'common', data - 'max_nof_hashs_per_core'
 * Maximal number of hash table entries to assign at init.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_nof_hashs_per_core - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_utilex_common_max_nof_hashs_per_core_get(
    int unit);

/**
 * \brief returns define data of max_nof_lists_per_core
 * Module - 'utilex', Submodule - 'common', data - 'max_nof_lists_per_core'
 * Maximal number of sorted list entries to assign at init.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_nof_lists_per_core - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_utilex_common_max_nof_lists_per_core_get(
    int unit);

/**
 * \brief returns define data of max_nof_multis_per_core
 * Module - 'utilex', Submodule - 'common', data - 'max_nof_multis_per_core'
 * Maximal number of multi set entries to assign at init.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_nof_multis_per_core - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_utilex_common_max_nof_multis_per_core_get(
    int unit);

/**
 * \brief returns define data of max_nof_res_tag_bitmaps_per_core
 * Module - 'utilex', Submodule - 'common', data - 'max_nof_res_tag_bitmaps_per_core'
 * Maximal number of res tag bitmaps to assign at init.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_nof_res_tag_bitmaps_per_core - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_utilex_common_max_nof_res_tag_bitmaps_per_core_get(
    int unit);

/**
 * \brief returns define data of max_nof_defragmented_chunks_per_core
 * Module - 'utilex', Submodule - 'common', data - 'max_nof_defragmented_chunks_per_core'
 * Maximal number of defragmented chunks to assign at init.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_nof_defragmented_chunks_per_core - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_utilex_common_max_nof_defragmented_chunks_per_core_get(
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
    _dnx_data_utilex_common_table_nof
} dnx_data_utilex_common_table_e;

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
shr_error_e dnx_data_utilex_init(
    int unit,
    dnxc_data_module_t *module_data);

/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_INTERNAL_UTILEX_H_*/
/* *INDENT-ON* */
