/** \file dnx_data_ingress_cs.h
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
#ifndef _DNX_DATA_INGRESS_CS_H_
/*{*/
#define _DNX_DATA_INGRESS_CS_H_
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ingress_cs.h>
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
 * \brief Init default data structure - dnx_data_if_ingress_cs
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
shr_error_e dnx_data_if_ingress_cs_init(
    int unit);

/*
 * }
 */

/*
 * SUBMODULE  - PROPERTIES:
 * Information about ingress CS TCAM, plus PRT TCAM and  ePMF CS TCAM.
 * {
 */
/*
 * Table value structure
 */
/**
 * \brief Holds values of submodule properties table per_stage
 * Table info:
 * Per stage (CS DB) information, including whether the CS_TCAM supports half entries.
 */
typedef struct
{
    /**
     * If the stage has information in this table.
     */
    int valid;
    /**
     * If the CS TCAM can use half entries.
     */
    int supports_half_entries;
} dnx_data_ingress_cs_properties_per_stage_t;

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
    _dnx_data_ingress_cs_properties_feature_nof
} dnx_data_ingress_cs_properties_feature_e;

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
    *dnx_data_ingress_cs_properties_feature_get_f) (
    int unit,
    dnx_data_ingress_cs_properties_feature_e feature);

/**
 * \brief returns define data of cs_buffer_nof_bits_max
 * Module - 'ingress_cs', Submodule - 'properties', data - 'cs_buffer_nof_bits_max'
 * Length in bits of the Context Selection TCAM buffer for qualifiers or masks, the maximum of all ingress CS TCAMs.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     cs_buffer_nof_bits_max - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingress_cs_properties_cs_buffer_nof_bits_max_get_f) (
    int unit);

/**
 * \brief returns define data of nof_bits_entry_size_prefix
 * Module - 'ingress_cs', Submodule - 'properties', data - 'nof_bits_entry_size_prefix'
 * Number of bits used in the prefix indicating half key or full key. Expected to be used for each half key at the lsb for tables that support half entries.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_bits_entry_size_prefix - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingress_cs_properties_nof_bits_entry_size_prefix_get_f) (
    int unit);

/**
 * \brief get table per_stage entry 
 * Per stage (CS DB) information, including whether the CS_TCAM supports half entries.
 * 
 * \param [in] unit - unit #
 * \param [in] stage - CS DB, which CS TCAM to provide information for.
 * 
 * \return
 *     per_stage - returns the relevant entry values grouped in struct - see dnx_data_ingress_cs_properties_per_stage_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef const dnx_data_ingress_cs_properties_per_stage_t *(
    *dnx_data_ingress_cs_properties_per_stage_get_f) (
    int unit,
    int stage);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_INGRESS_CS - PROPERTIES:
 * {
 */
/**
 * \brief Interface for ingress_cs properties data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_ingress_cs_properties_feature_get_f feature_get;
    /**
     * returns define data of cs_buffer_nof_bits_max
     */
    dnx_data_ingress_cs_properties_cs_buffer_nof_bits_max_get_f cs_buffer_nof_bits_max_get;
    /**
     * returns define data of nof_bits_entry_size_prefix
     */
    dnx_data_ingress_cs_properties_nof_bits_entry_size_prefix_get_f nof_bits_entry_size_prefix_get;
    /**
     * get table per_stage entry 
     */
    dnx_data_ingress_cs_properties_per_stage_get_f per_stage_get;
    /**
     * get general info table about table (for example key size)per_stage info
     */
    dnxc_data_table_info_get_f per_stage_info_get;
} dnx_data_if_ingress_cs_properties_t;

/*
 * }
 */

/*
 * SUBMODULE  - FEATURES:
 * Per devices features
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
    dnx_data_ingress_cs_features_disable_ecc_fix_en_before_read_write,

    /**
     * Must be last one!
     */
    _dnx_data_ingress_cs_features_feature_nof
} dnx_data_ingress_cs_features_feature_e;

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
    *dnx_data_ingress_cs_features_feature_get_f) (
    int unit,
    dnx_data_ingress_cs_features_feature_e feature);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_INGRESS_CS - FEATURES:
 * {
 */
/**
 * \brief Interface for ingress_cs features data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_ingress_cs_features_feature_get_f feature_get;
} dnx_data_if_ingress_cs_features_t;

/*
 * }
 */

/*
 * SUBMODULE  - PARSER:
 * Ingress parser data
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
     * data path control valid flag
     */
    dnx_data_ingress_cs_parser_parsing_context_map_enable,

    /**
     * Must be last one!
     */
    _dnx_data_ingress_cs_parser_feature_nof
} dnx_data_ingress_cs_parser_feature_e;

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
    *dnx_data_ingress_cs_parser_feature_get_f) (
    int unit,
    dnx_data_ingress_cs_parser_feature_e feature);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_INGRESS_CS - PARSER:
 * {
 */
/**
 * \brief Interface for ingress_cs parser data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_ingress_cs_parser_feature_get_f feature_get;
} dnx_data_if_ingress_cs_parser_t;

/*
 * }
 */

/*
 * MODULE INTERFACE DNX_DATA_IF_INGRESS_CS:
 * {
 */
/**
 * \brief Interface for ingress_cs data
 */
typedef struct
{
    /**
     * Interface for ingress_cs properties data
     */
    dnx_data_if_ingress_cs_properties_t properties;
    /**
     * Interface for ingress_cs features data
     */
    dnx_data_if_ingress_cs_features_t features;
    /**
     * Interface for ingress_cs parser data
     */
    dnx_data_if_ingress_cs_parser_t parser;
} dnx_data_if_ingress_cs_t;

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
extern dnx_data_if_ingress_cs_t dnx_data_ingress_cs;
/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_INGRESS_CS_H_*/
/* *INDENT-ON* */
