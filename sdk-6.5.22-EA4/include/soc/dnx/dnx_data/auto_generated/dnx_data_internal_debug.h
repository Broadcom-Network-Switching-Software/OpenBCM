/** \file dnx_data_internal_debug.h
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
#ifndef _DNX_DATA_INTERNAL_DEBUG_H_
/*{*/
#define _DNX_DATA_INTERNAL_DEBUG_H_
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
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
    dnx_data_debug_submodule_mem,
    dnx_data_debug_submodule_feature,
    dnx_data_debug_submodule_kleap,
    dnx_data_debug_submodule_general,

    /**
     * Must be last one!
     */
    _dnx_data_debug_submodule_nof
} dnx_data_debug_submodule_e;

/*
 * }
 */

/*
 * SUBMODULE MEM:
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
int dnx_data_debug_mem_feature_get(
    int unit,
    dnx_data_debug_mem_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{

    /**
     * Must be last one!
     */
    _dnx_data_debug_mem_define_nof
} dnx_data_debug_mem_define_e;

/* Get Data */
/*
 * Tables
 */
/**
 * \brief 
 * Table enum
 */
typedef enum
{
    dnx_data_debug_mem_table_params,

    /**
     * Must be last one!
     */
    _dnx_data_debug_mem_table_nof
} dnx_data_debug_mem_table_e;

/* Get Data */
/**
 * \brief get table params entry 
 * Per ASIC block properties and characteristics
 * 
 * \param [in] unit - unit #
 * \param [in] block - ASIC Block ID
 * 
 * \return
 *     params - returns the relevant entry values grouped in struct - see dnx_data_debug_mem_params_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnx_data_debug_mem_params_t * dnx_data_debug_mem_params_get(
    int unit,
    int block);

/* Get Value Str */
/**
 * \brief Get table value as string
 * Module - 'debug', Submodule - 'mem', table - 'params'
 * Per ASIC block properties and characteristics
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
shr_error_e dnx_data_debug_mem_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/* Get Table info */
/**
 * \brief get general info table about table (for example key size)params info
 * Per ASIC block properties and characteristics
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     params - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnx_data_debug_mem_params_info_get(
    int unit);

/*
 * SUBMODULE FEATURE:
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
int dnx_data_debug_feature_feature_get(
    int unit,
    dnx_data_debug_feature_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{

    /**
     * Must be last one!
     */
    _dnx_data_debug_feature_define_nof
} dnx_data_debug_feature_define_e;

/* Get Data */
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
    _dnx_data_debug_feature_table_nof
} dnx_data_debug_feature_table_e;

/* Get Data */
/* Get Value Str */
/* Get Table info */
/*
 * SUBMODULE KLEAP:
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
int dnx_data_debug_kleap_feature_get(
    int unit,
    dnx_data_debug_kleap_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_debug_kleap_define_fwd1_gen_data_size,
    dnx_data_debug_kleap_define_fwd2_gen_data_size,

    /**
     * Must be last one!
     */
    _dnx_data_debug_kleap_define_nof
} dnx_data_debug_kleap_define_e;

/* Get Data */
/**
 * \brief returns define data of fwd1_gen_data_size
 * Module - 'debug', Submodule - 'kleap', data - 'fwd1_gen_data_size'
 * FWD1 general data size, used to find the const values
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     fwd1_gen_data_size - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_debug_kleap_fwd1_gen_data_size_get(
    int unit);

/**
 * \brief returns define data of fwd2_gen_data_size
 * Module - 'debug', Submodule - 'kleap', data - 'fwd2_gen_data_size'
 * FWD2 general data size, used to find the const values
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     fwd2_gen_data_size - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_debug_kleap_fwd2_gen_data_size_get(
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
    _dnx_data_debug_kleap_table_nof
} dnx_data_debug_kleap_table_e;

/* Get Data */
/* Get Value Str */
/* Get Table info */
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
int dnx_data_debug_general_feature_get(
    int unit,
    dnx_data_debug_general_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_debug_general_define_global_visibility,
    dnx_data_debug_general_define_etpp_eop_and_sop_dec_above_threshold,
    dnx_data_debug_general_define_egress_port_mc_visibility,
    dnx_data_debug_general_define_mdb_debug_signals,
    dnx_data_debug_general_define_lbp_debug_signals_register_based,
    dnx_data_debug_general_define_mem_array_index,

    /**
     * Must be last one!
     */
    _dnx_data_debug_general_define_nof
} dnx_data_debug_general_define_e;

/* Get Data */
/**
 * \brief returns define data of global_visibility
 * Module - 'debug', Submodule - 'general', data - 'global_visibility'
 *  visibility global enabler
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     global_visibility - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_debug_general_global_visibility_get(
    int unit);

/**
 * \brief returns define data of etpp_eop_and_sop_dec_above_threshold
 * Module - 'debug', Submodule - 'general', data - 'etpp_eop_and_sop_dec_above_threshold'
 * eop/sop decrement above threshold signal
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     etpp_eop_and_sop_dec_above_threshold - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_debug_general_etpp_eop_and_sop_dec_above_threshold_get(
    int unit);

/**
 * \brief returns define data of egress_port_mc_visibility
 * Module - 'debug', Submodule - 'general', data - 'egress_port_mc_visibility'
 * port-visibility control at egress MC
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     egress_port_mc_visibility - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_debug_general_egress_port_mc_visibility_get(
    int unit);

/**
 * \brief returns define data of mdb_debug_signals
 * Module - 'debug', Submodule - 'general', data - 'mdb_debug_signals'
 * Indicates whether debug signals are accessible.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     mdb_debug_signals - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_debug_general_mdb_debug_signals_get(
    int unit);

/**
 * \brief returns define data of lbp_debug_signals_register_based
 * Module - 'debug', Submodule - 'general', data - 'lbp_debug_signals_register_based'
 * Indicates LBP debug signals are register based, LBP signals are always show the last packet when it's register based.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     lbp_debug_signals_register_based - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_debug_general_lbp_debug_signals_register_based_get(
    int unit);

/**
 * \brief returns numeric data of mem_array_index
 * Module - 'debug', Submodule - 'general', data - 'mem_array_index'
 * used debug mem array index according to device
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     mem_array_index - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_debug_general_mem_array_index_get(
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
    _dnx_data_debug_general_table_nof
} dnx_data_debug_general_table_e;

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
shr_error_e dnx_data_debug_init(
    int unit,
    dnxc_data_module_t *module_data);

/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_INTERNAL_DEBUG_H_*/
/* *INDENT-ON* */
