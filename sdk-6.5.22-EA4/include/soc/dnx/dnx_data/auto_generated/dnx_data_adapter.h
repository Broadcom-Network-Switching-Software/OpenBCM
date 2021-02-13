/** \file dnx_data_adapter.h
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
#ifndef _DNX_DATA_ADAPTER_H_
/*{*/
#define _DNX_DATA_ADAPTER_H_
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_adapter.h>
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
 * \brief Init default data structure - dnx_data_if_adapter
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
shr_error_e dnx_data_if_adapter_init(
    int unit);

/*
 * }
 */

/*
 * SUBMODULE  - TX:
 * adapter tx
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
    _dnx_data_adapter_tx_feature_nof
} dnx_data_adapter_tx_feature_e;

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
    *dnx_data_adapter_tx_feature_get_f) (
    int unit,
    dnx_data_adapter_tx_feature_e feature);

/**
 * \brief returns numeric data of loopback_enable
 * Module - 'adapter', Submodule - 'tx', data - 'loopback_enable'
 * Use loopback in the adapter server instead of go through the pipe when receiving tx packets
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     loopback_enable - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_adapter_tx_loopback_enable_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_ADAPTER - TX:
 * {
 */
/**
 * \brief Interface for adapter tx data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_adapter_tx_feature_get_f feature_get;
    /**
     * returns numeric data of loopback_enable
     */
    dnx_data_adapter_tx_loopback_enable_get_f loopback_enable_get;
} dnx_data_if_adapter_tx_t;

/*
 * }
 */

/*
 * SUBMODULE  - RX:
 * adapter rx
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
    _dnx_data_adapter_rx_feature_nof
} dnx_data_adapter_rx_feature_e;

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
    *dnx_data_adapter_rx_feature_get_f) (
    int unit,
    dnx_data_adapter_rx_feature_e feature);

/**
 * \brief returns numeric data of packet_header_signal_id
 * Module - 'adapter', Submodule - 'rx', data - 'packet_header_signal_id'
 * The adater signal id to identify the packet header
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     packet_header_signal_id - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_adapter_rx_packet_header_signal_id_get_f) (
    int unit);

/**
 * \brief returns numeric data of constant_header_size
 * Module - 'adapter', Submodule - 'rx', data - 'constant_header_size'
 * Number of bytes in the constant header of a rx packet
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     constant_header_size - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_adapter_rx_constant_header_size_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_ADAPTER - RX:
 * {
 */
/**
 * \brief Interface for adapter rx data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_adapter_rx_feature_get_f feature_get;
    /**
     * returns numeric data of packet_header_signal_id
     */
    dnx_data_adapter_rx_packet_header_signal_id_get_f packet_header_signal_id_get;
    /**
     * returns numeric data of constant_header_size
     */
    dnx_data_adapter_rx_constant_header_size_get_f constant_header_size_get;
} dnx_data_if_adapter_rx_t;

/*
 * }
 */

/*
 * SUBMODULE  - GENERAL:
 * adapter general configurations
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
    _dnx_data_adapter_general_feature_nof
} dnx_data_adapter_general_feature_e;

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
    *dnx_data_adapter_general_feature_get_f) (
    int unit,
    dnx_data_adapter_general_feature_e feature);

/**
 * \brief returns numeric data of lib_ver
 * Module - 'adapter', Submodule - 'general', data - 'lib_ver'
 * Adapter library version
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     lib_ver - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_adapter_general_lib_ver_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_ADAPTER - GENERAL:
 * {
 */
/**
 * \brief Interface for adapter general data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_adapter_general_feature_get_f feature_get;
    /**
     * returns numeric data of lib_ver
     */
    dnx_data_adapter_general_lib_ver_get_f lib_ver_get;
} dnx_data_if_adapter_general_t;

/*
 * }
 */

/*
 * SUBMODULE  - REG_MEM_ACCESS:
 * adapter reg/mem access
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
    _dnx_data_adapter_reg_mem_access_feature_nof
} dnx_data_adapter_reg_mem_access_feature_e;

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
    *dnx_data_adapter_reg_mem_access_feature_get_f) (
    int unit,
    dnx_data_adapter_reg_mem_access_feature_e feature);

/**
 * \brief returns numeric data of do_collect_enable
 * Module - 'adapter', Submodule - 'reg_mem_access', data - 'do_collect_enable'
 * Use do_collect in adapter in order to collect writes into one bulk write.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     do_collect_enable - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_adapter_reg_mem_access_do_collect_enable_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_ADAPTER - REG_MEM_ACCESS:
 * {
 */
/**
 * \brief Interface for adapter reg_mem_access data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_adapter_reg_mem_access_feature_get_f feature_get;
    /**
     * returns numeric data of do_collect_enable
     */
    dnx_data_adapter_reg_mem_access_do_collect_enable_get_f do_collect_enable_get;
} dnx_data_if_adapter_reg_mem_access_t;

/*
 * }
 */

/*
 * MODULE INTERFACE DNX_DATA_IF_ADAPTER:
 * {
 */
/**
 * \brief Interface for adapter data
 */
typedef struct
{
    /**
     * Interface for adapter tx data
     */
    dnx_data_if_adapter_tx_t tx;
    /**
     * Interface for adapter rx data
     */
    dnx_data_if_adapter_rx_t rx;
    /**
     * Interface for adapter general data
     */
    dnx_data_if_adapter_general_t general;
    /**
     * Interface for adapter reg_mem_access data
     */
    dnx_data_if_adapter_reg_mem_access_t reg_mem_access;
} dnx_data_if_adapter_t;

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
extern dnx_data_if_adapter_t dnx_data_adapter;
/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_ADAPTER_H_*/
/* *INDENT-ON* */
