/** \file dnx_data_ingr_reassembly.h
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
#ifndef _DNX_DATA_INGR_REASSEMBLY_H_
/*{*/
#define _DNX_DATA_INGR_REASSEMBLY_H_
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
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_ingr_reassembly.h>
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
 * \brief Init default data structure - dnx_data_if_ingr_reassembly
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
shr_error_e dnx_data_if_ingr_reassembly_init(
    int unit);

/*
 * }
 */

/*
 * SUBMODULE  - CONTEXT:
 * Ingress context data
 * {
 */
/*
 * Table value structure
 */
/**
 * \brief Holds values of submodule context table context_map
 * Table info:
 * Context Map configuration information per port type 
 */
typedef struct
{
    /**
     * index of start of specific interface type in reassembly table
     */
    uint32 start_index;
} dnx_data_ingr_reassembly_context_context_map_t;

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
     * RCY interleaving between priorities
     */
    dnx_data_ingr_reassembly_context_rcy_interleaving_between_priorities,
    /**
     * is context map table accessed directly, without base table
     */
    dnx_data_ingr_reassembly_context_direct_context_table,
    /**
     * is port termination configured in context table
     */
    dnx_data_ingr_reassembly_context_port_termination_in_context_table,
    /**
     * Is PP/TM context passed to IRPP/CGM
     */
    dnx_data_ingr_reassembly_context_pp_tm_context,

    /**
     * Must be last one!
     */
    _dnx_data_ingr_reassembly_context_feature_nof
} dnx_data_ingr_reassembly_context_feature_e;

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
    *dnx_data_ingr_reassembly_context_feature_get_f) (
    int unit,
    dnx_data_ingr_reassembly_context_feature_e feature);

/**
 * \brief returns define data of nof_contexts
 * Module - 'ingr_reassembly', Submodule - 'context', data - 'nof_contexts'
 * Number of ingress reassembly contexts per core
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_contexts - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_context_nof_contexts_get_f) (
    int unit);

/**
 * \brief returns define data of invalid_context
 * Module - 'ingr_reassembly', Submodule - 'context', data - 'invalid_context'
 * Invalid reassembly context
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     invalid_context - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_context_invalid_context_get_f) (
    int unit);

/**
 * \brief returns define data of nof_cpu_channels_per_core
 * Module - 'ingr_reassembly', Submodule - 'context', data - 'nof_cpu_channels_per_core'
 * Number of CPU channels per core
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_cpu_channels_per_core - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_context_nof_cpu_channels_per_core_get_f) (
    int unit);

/**
 * \brief get table context_map entry 
 * Context Map configuration information per port type 
 * 
 * \param [in] unit - unit #
 * \param [in] port_type - interface type
 * 
 * \return
 *     context_map - returns the relevant entry values grouped in struct - see dnx_data_ingr_reassembly_context_context_map_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef const dnx_data_ingr_reassembly_context_context_map_t *(
    *dnx_data_ingr_reassembly_context_context_map_get_f) (
    int unit,
    int port_type);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_INGR_REASSEMBLY - CONTEXT:
 * {
 */
/**
 * \brief Interface for ingr_reassembly context data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_ingr_reassembly_context_feature_get_f feature_get;
    /**
     * returns define data of nof_contexts
     */
    dnx_data_ingr_reassembly_context_nof_contexts_get_f nof_contexts_get;
    /**
     * returns define data of invalid_context
     */
    dnx_data_ingr_reassembly_context_invalid_context_get_f invalid_context_get;
    /**
     * returns define data of nof_cpu_channels_per_core
     */
    dnx_data_ingr_reassembly_context_nof_cpu_channels_per_core_get_f nof_cpu_channels_per_core_get;
    /**
     * get table context_map entry 
     */
    dnx_data_ingr_reassembly_context_context_map_get_f context_map_get;
    /**
     * get general info table about table (for example key size)context_map info
     */
    dnxc_data_table_info_get_f context_map_info_get;
} dnx_data_if_ingr_reassembly_context_t;

/*
 * }
 */

/*
 * SUBMODULE  - ILKN:
 * Interlaken data
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
     * set if device supports ILKN ports (not including ELK)
     */
    dnx_data_ingr_reassembly_ilkn_is_supported,

    /**
     * Must be last one!
     */
    _dnx_data_ingr_reassembly_ilkn_feature_nof
} dnx_data_ingr_reassembly_ilkn_feature_e;

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
    *dnx_data_ingr_reassembly_ilkn_feature_get_f) (
    int unit,
    dnx_data_ingr_reassembly_ilkn_feature_e feature);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_INGR_REASSEMBLY - ILKN:
 * {
 */
/**
 * \brief Interface for ingr_reassembly ilkn data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_ingr_reassembly_ilkn_feature_get_f feature_get;
} dnx_data_if_ingr_reassembly_ilkn_t;

/*
 * }
 */

/*
 * SUBMODULE  - PRIORITY:
 * information about priorities
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
    _dnx_data_ingr_reassembly_priority_feature_nof
} dnx_data_ingr_reassembly_priority_feature_e;

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
    *dnx_data_ingr_reassembly_priority_feature_get_f) (
    int unit,
    dnx_data_ingr_reassembly_priority_feature_e feature);

/**
 * \brief returns define data of nif_eth_priorities_nof
 * Module - 'ingr_reassembly', Submodule - 'priority', data - 'nif_eth_priorities_nof'
 * Number of priorities for NIF ETH port
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nif_eth_priorities_nof - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_priority_nif_eth_priorities_nof_get_f) (
    int unit);

/**
 * \brief returns define data of mirror_priorities_nof
 * Module - 'ingr_reassembly', Submodule - 'priority', data - 'mirror_priorities_nof'
 * Number of priorities for mirror channel
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     mirror_priorities_nof - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_priority_mirror_priorities_nof_get_f) (
    int unit);

/**
 * \brief returns define data of rcy_priorities_nof
 * Module - 'ingr_reassembly', Submodule - 'priority', data - 'rcy_priorities_nof'
 * Number of priorities (groups) for RCY interface
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     rcy_priorities_nof - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_priority_rcy_priorities_nof_get_f) (
    int unit);

/**
 * \brief returns define data of priorities_nof
 * Module - 'ingr_reassembly', Submodule - 'priority', data - 'priorities_nof'
 * Number of priorities (groups) for any interface type
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     priorities_nof - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_priority_priorities_nof_get_f) (
    int unit);

/**
 * \brief returns define data of cgm_priorities_nof
 * Module - 'ingr_reassembly', Submodule - 'priority', data - 'cgm_priorities_nof'
 * Number of priorities (groups) for any interface type as considered by CGM module
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     cgm_priorities_nof - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_priority_cgm_priorities_nof_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_INGR_REASSEMBLY - PRIORITY:
 * {
 */
/**
 * \brief Interface for ingr_reassembly priority data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_ingr_reassembly_priority_feature_get_f feature_get;
    /**
     * returns define data of nif_eth_priorities_nof
     */
    dnx_data_ingr_reassembly_priority_nif_eth_priorities_nof_get_f nif_eth_priorities_nof_get;
    /**
     * returns define data of mirror_priorities_nof
     */
    dnx_data_ingr_reassembly_priority_mirror_priorities_nof_get_f mirror_priorities_nof_get;
    /**
     * returns define data of rcy_priorities_nof
     */
    dnx_data_ingr_reassembly_priority_rcy_priorities_nof_get_f rcy_priorities_nof_get;
    /**
     * returns define data of priorities_nof
     */
    dnx_data_ingr_reassembly_priority_priorities_nof_get_f priorities_nof_get;
    /**
     * returns define data of cgm_priorities_nof
     */
    dnx_data_ingr_reassembly_priority_cgm_priorities_nof_get_f cgm_priorities_nof_get;
} dnx_data_if_ingr_reassembly_priority_t;

/*
 * }
 */

/*
 * SUBMODULE  - DBAL:
 * information required for dbal
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
    _dnx_data_ingr_reassembly_dbal_feature_nof
} dnx_data_ingr_reassembly_dbal_feature_e;

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
    *dnx_data_ingr_reassembly_dbal_feature_get_f) (
    int unit,
    dnx_data_ingr_reassembly_dbal_feature_e feature);

/**
 * \brief returns define data of reassembly_context_bits
 * Module - 'ingr_reassembly', Submodule - 'dbal', data - 'reassembly_context_bits'
 * number of bits in reassembly context
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     reassembly_context_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_dbal_reassembly_context_bits_get_f) (
    int unit);

/**
 * \brief returns define data of interface_bits
 * Module - 'ingr_reassembly', Submodule - 'dbal', data - 'interface_bits'
 * number of bits in ingress reassembly interface ID
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     interface_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_dbal_interface_bits_get_f) (
    int unit);

/**
 * \brief returns define data of interface_max
 * Module - 'ingr_reassembly', Submodule - 'dbal', data - 'interface_max'
 * maximal ingress reassembly interface ID
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     interface_max - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_dbal_interface_max_get_f) (
    int unit);

/**
 * \brief returns define data of context_map_base_address_bits
 * Module - 'ingr_reassembly', Submodule - 'dbal', data - 'context_map_base_address_bits'
 * number of bits in context_map_base_address
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     context_map_base_address_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_dbal_context_map_base_address_bits_get_f) (
    int unit);

/**
 * \brief returns define data of context_map_base_address_max
 * Module - 'ingr_reassembly', Submodule - 'dbal', data - 'context_map_base_address_max'
 * maximal value of context_map_base_address
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     context_map_base_address_max - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_dbal_context_map_base_address_max_get_f) (
    int unit);

/**
 * \brief returns define data of context_map_base_offest_ext_max
 * Module - 'ingr_reassembly', Submodule - 'dbal', data - 'context_map_base_offest_ext_max'
 * maximal value of offset ext field
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     context_map_base_offest_ext_max - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_dbal_context_map_base_offest_ext_max_get_f) (
    int unit);

/**
 * \brief returns define data of interleaved_interface_max
 * Module - 'ingr_reassembly', Submodule - 'dbal', data - 'interleaved_interface_max'
 * maximal interleaved interface ID
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     interleaved_interface_max - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_dbal_interleaved_interface_max_get_f) (
    int unit);

/**
 * \brief returns numeric data of interleaved_interface_bits
 * Module - 'ingr_reassembly', Submodule - 'dbal', data - 'interleaved_interface_bits'
 * number of bits in interleaved interface ID
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     interleaved_interface_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_ingr_reassembly_dbal_interleaved_interface_bits_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_INGR_REASSEMBLY - DBAL:
 * {
 */
/**
 * \brief Interface for ingr_reassembly dbal data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_ingr_reassembly_dbal_feature_get_f feature_get;
    /**
     * returns define data of reassembly_context_bits
     */
    dnx_data_ingr_reassembly_dbal_reassembly_context_bits_get_f reassembly_context_bits_get;
    /**
     * returns define data of interface_bits
     */
    dnx_data_ingr_reassembly_dbal_interface_bits_get_f interface_bits_get;
    /**
     * returns define data of interface_max
     */
    dnx_data_ingr_reassembly_dbal_interface_max_get_f interface_max_get;
    /**
     * returns define data of context_map_base_address_bits
     */
    dnx_data_ingr_reassembly_dbal_context_map_base_address_bits_get_f context_map_base_address_bits_get;
    /**
     * returns define data of context_map_base_address_max
     */
    dnx_data_ingr_reassembly_dbal_context_map_base_address_max_get_f context_map_base_address_max_get;
    /**
     * returns define data of context_map_base_offest_ext_max
     */
    dnx_data_ingr_reassembly_dbal_context_map_base_offest_ext_max_get_f context_map_base_offest_ext_max_get;
    /**
     * returns define data of interleaved_interface_max
     */
    dnx_data_ingr_reassembly_dbal_interleaved_interface_max_get_f interleaved_interface_max_get;
    /**
     * returns numeric data of interleaved_interface_bits
     */
    dnx_data_ingr_reassembly_dbal_interleaved_interface_bits_get_f interleaved_interface_bits_get;
} dnx_data_if_ingr_reassembly_dbal_t;

/*
 * }
 */

/*
 * MODULE INTERFACE DNX_DATA_IF_INGR_REASSEMBLY:
 * {
 */
/**
 * \brief Interface for ingr_reassembly data
 */
typedef struct
{
    /**
     * Interface for ingr_reassembly context data
     */
    dnx_data_if_ingr_reassembly_context_t context;
    /**
     * Interface for ingr_reassembly ilkn data
     */
    dnx_data_if_ingr_reassembly_ilkn_t ilkn;
    /**
     * Interface for ingr_reassembly priority data
     */
    dnx_data_if_ingr_reassembly_priority_t priority;
    /**
     * Interface for ingr_reassembly dbal data
     */
    dnx_data_if_ingr_reassembly_dbal_t dbal;
} dnx_data_if_ingr_reassembly_t;

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
extern dnx_data_if_ingr_reassembly_t dnx_data_ingr_reassembly;
/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_INGR_REASSEMBLY_H_*/
/* *INDENT-ON* */
