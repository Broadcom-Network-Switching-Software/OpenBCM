/** \file dnx_data_internal_synce.h
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
#ifndef _DNX_DATA_INTERNAL_SYNCE_H_
/*{*/
#define _DNX_DATA_INTERNAL_SYNCE_H_
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_synce.h>
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
    dnx_data_synce_submodule_general,

    /**
     * Must be last one!
     */
    _dnx_data_synce_submodule_nof
} dnx_data_synce_submodule_e;

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
int dnx_data_synce_general_feature_get(
    int unit,
    dnx_data_synce_general_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_synce_general_define_nof_plls,
    dnx_data_synce_general_define_nif_div_min,
    dnx_data_synce_general_define_nif_div_max,
    dnx_data_synce_general_define_fabric_div_min,
    dnx_data_synce_general_define_fabric_div_max,
    dnx_data_synce_general_define_nof_clk_blocks,
    dnx_data_synce_general_define_nof_fabric_clk_blocks,
    dnx_data_synce_general_define_output_clk_mode,

    /**
     * Must be last one!
     */
    _dnx_data_synce_general_define_nof
} dnx_data_synce_general_define_e;

/* Get Data */
/**
 * \brief returns define data of nof_plls
 * Module - 'synce', Submodule - 'general', data - 'nof_plls'
 * Number of synce plls in the device.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_plls - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_synce_general_nof_plls_get(
    int unit);

/**
 * \brief returns define data of nif_div_min
 * Module - 'synce', Submodule - 'general', data - 'nif_div_min'
 * Min value for the nif synce output clk.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nif_div_min - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_synce_general_nif_div_min_get(
    int unit);

/**
 * \brief returns define data of nif_div_max
 * Module - 'synce', Submodule - 'general', data - 'nif_div_max'
 * Max value for the nif synce output clk.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nif_div_max - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_synce_general_nif_div_max_get(
    int unit);

/**
 * \brief returns define data of fabric_div_min
 * Module - 'synce', Submodule - 'general', data - 'fabric_div_min'
 * Min value for the fabric synce output clk.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     fabric_div_min - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_synce_general_fabric_div_min_get(
    int unit);

/**
 * \brief returns define data of fabric_div_max
 * Module - 'synce', Submodule - 'general', data - 'fabric_div_max'
 * Max value for the fabric synce output clk.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     fabric_div_max - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_synce_general_fabric_div_max_get(
    int unit);

/**
 * \brief returns define data of nof_clk_blocks
 * Module - 'synce', Submodule - 'general', data - 'nof_clk_blocks'
 * Max value for the nif synce clock blocks.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_clk_blocks - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_synce_general_nof_clk_blocks_get(
    int unit);

/**
 * \brief returns define data of nof_fabric_clk_blocks
 * Module - 'synce', Submodule - 'general', data - 'nof_fabric_clk_blocks'
 * Max value for the fabric synce clock blocks.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_fabric_clk_blocks - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_synce_general_nof_fabric_clk_blocks_get(
    int unit);

/**
 * \brief returns numeric data of output_clk_mode
 * Module - 'synce', Submodule - 'general', data - 'output_clk_mode'
 * SyncE output clock mode.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     output_clk_mode - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_synce_general_output_clk_mode_get(
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
    dnx_data_synce_general_table_cfg,
    dnx_data_synce_general_table_nmg_chain_map,

    /**
     * Must be last one!
     */
    _dnx_data_synce_general_table_nof
} dnx_data_synce_general_table_e;

/* Get Data */
/**
 * \brief get table cfg entry 
 * SyncE common configuration.
 * 
 * \param [in] unit - unit #
 * \param [in] synce_index - Master or Slave SyncE.
 * 
 * \return
 *     cfg - returns the relevant entry values grouped in struct - see dnx_data_synce_general_cfg_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnx_data_synce_general_cfg_t * dnx_data_synce_general_cfg_get(
    int unit,
    int synce_index);

/**
 * \brief get table nmg_chain_map entry 
 * ethu_id mapped to NMG chain related to SYNCE
 * 
 * \param [in] unit - unit #
 * \param [in] ethu_index - ethu instance number.
 * 
 * \return
 *     nmg_chain_map - returns the relevant entry values grouped in struct - see dnx_data_synce_general_nmg_chain_map_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnx_data_synce_general_nmg_chain_map_t * dnx_data_synce_general_nmg_chain_map_get(
    int unit,
    int ethu_index);

/* Get Value Str */
/**
 * \brief Get table value as string
 * Module - 'synce', Submodule - 'general', table - 'cfg'
 * SyncE common configuration.
 * 
 * \param [in] unit - Unit #
 * \param [out] buffer - buffer for the value
 * \param [in] key0 - key #0 index - 0 if not used
 * \param [in] key1 - key #1 index - 0 if not used
 * \param [in] value_index - value index to get - the index is alligned with values array in table structure
 * 
 * \return
 *     err - See shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnx_data_synce_general_cfg_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/**
 * \brief Get table value as string
 * Module - 'synce', Submodule - 'general', table - 'nmg_chain_map'
 * ethu_id mapped to NMG chain related to SYNCE
 * 
 * \param [in] unit - Unit #
 * \param [out] buffer - buffer for the value
 * \param [in] key0 - key #0 index - 0 if not used
 * \param [in] key1 - key #1 index - 0 if not used
 * \param [in] value_index - value index to get - the index is alligned with values array in table structure
 * 
 * \return
 *     err - See shr_error_e
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
shr_error_e dnx_data_synce_general_nmg_chain_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/* Get Table info */
/**
 * \brief get general info table about table (for example key size)cfg info
 * SyncE common configuration.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     cfg - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnx_data_synce_general_cfg_info_get(
    int unit);

/**
 * \brief get general info table about table (for example key size)nmg_chain_map info
 * ethu_id mapped to NMG chain related to SYNCE
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nmg_chain_map - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnx_data_synce_general_nmg_chain_map_info_get(
    int unit);

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
shr_error_e dnx_data_synce_init(
    int unit,
    dnxc_data_module_t *module_data);

/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_INTERNAL_SYNCE_H_*/
/* *INDENT-ON* */
