/** \file dnx_data_esb.h
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
#ifndef _DNX_DATA_ESB_H_
/*{*/
#define _DNX_DATA_ESB_H_
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_esb.h>
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
 * \brief Init default data structure - dnx_data_if_esb
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
shr_error_e dnx_data_if_esb_init(
    int unit);

/*
 * }
 */

/*
 * SUBMODULE  - GENERAL:
 * General parameters of Egress Shared Buffers
 * {
 */
/*
 * Table value structure
 */
/**
 * \brief Holds values of submodule general table if_esb_params
 * Table info:
 * Configuration parameters for ESB
 */
typedef struct
{
    /**
     * Port speed in MB
     */
    int speed;
    /**
     * Egress interface ID
     */
    int if_id;
    /**
     * Dedicated ESB queue ID
     */
    int queue_id;
    /**
     * Dedicated ESB queue ID for interleave
     */
    int interleave_queue_id;
    /**
     * Value of min gap for MUX access
     */
    int min_gap;
} dnx_data_esb_general_if_esb_params_t;

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
     * Indication if Egress Shared Buffers are supported for device
     */
    dnx_data_esb_general_esb_support,
    /**
     * Support for dedicated identifier for Slow Ports queues to agreggate them  in the MUX
     */
    dnx_data_esb_general_slow_port_dedicated_queue_support,

    /**
     * Must be last one!
     */
    _dnx_data_esb_general_feature_nof
} dnx_data_esb_general_feature_e;

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
    *dnx_data_esb_general_feature_get_f) (
    int unit,
    dnx_data_esb_general_feature_e feature);

/**
 * \brief returns define data of total_nof_esb_queues
 * Module - 'esb', Submodule - 'general', data - 'total_nof_esb_queues'
 * Number of ESB queue IDs, each queue represents an interface (NIFs or special)
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     total_nof_esb_queues - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_total_nof_esb_queues_get_f) (
    int unit);

/**
 * \brief returns define data of nof_nif_eth_queues
 * Module - 'esb', Submodule - 'general', data - 'nof_nif_eth_queues'
 * Number of ESB queue IDs that represent NIF ETH interfaces
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_nif_eth_queues - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_nof_nif_eth_queues_get_f) (
    int unit);

/**
 * \brief returns define data of nof_nif_ilkn_queues
 * Module - 'esb', Submodule - 'general', data - 'nof_nif_ilkn_queues'
 * Number of ESB queue IDs that represent NIF ILKN interfaces
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_nif_ilkn_queues - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_nof_nif_ilkn_queues_get_f) (
    int unit);

/**
 * \brief returns define data of nof_ilkn_tdm_queues
 * Module - 'esb', Submodule - 'general', data - 'nof_ilkn_tdm_queues'
 * Number of ESB queue IDs dedicated for ILKN TDM interleave
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_ilkn_tdm_queues - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_nof_ilkn_tdm_queues_get_f) (
    int unit);

/**
 * \brief returns define data of nof_flexe_queues
 * Module - 'esb', Submodule - 'general', data - 'nof_flexe_queues'
 * Number of ESB queue IDs that represent FlexE interfaces
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_flexe_queues - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_nof_flexe_queues_get_f) (
    int unit);

/**
 * \brief returns define data of nof_special_queues
 * Module - 'esb', Submodule - 'general', data - 'nof_special_queues'
 * Number of ESB queue IDs that represent special interfaces
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_special_queues - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_nof_special_queues_get_f) (
    int unit);

/**
 * \brief returns define data of nof_reserved_queues
 * Module - 'esb', Submodule - 'general', data - 'nof_reserved_queues'
 * Number of ESB queue IDs that are reserved(not used)
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_reserved_queues - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_nof_reserved_queues_get_f) (
    int unit);

/**
 * \brief returns define data of nof_fpc
 * Module - 'esb', Submodule - 'general', data - 'nof_fpc'
 * Number of Free Pointer Controlers
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_fpc - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_nof_fpc_get_f) (
    int unit);

/**
 * \brief returns define data of total_nof_buffers
 * Module - 'esb', Submodule - 'general', data - 'total_nof_buffers'
 * Number of buffers
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     total_nof_buffers - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_total_nof_buffers_get_f) (
    int unit);

/**
 * \brief returns define data of mux_cal_len
 * Module - 'esb', Submodule - 'general', data - 'mux_cal_len'
 * Calendar length for the ESB MUX calendar
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     mux_cal_len - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_mux_cal_len_get_f) (
    int unit);

/**
 * \brief returns define data of ilkn_credits_resolution
 * Module - 'esb', Submodule - 'general', data - 'ilkn_credits_resolution'
 * number of credits to add for each speed resolution
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     ilkn_credits_resolution - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_ilkn_credits_resolution_get_f) (
    int unit);

/**
 * \brief returns define data of ilkn_credits_rate_resolution
 * Module - 'esb', Submodule - 'general', data - 'ilkn_credits_rate_resolution'
 * ilkn rate resolution (units: Mbps), for calculate the number of credits
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     ilkn_credits_rate_resolution - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_ilkn_credits_rate_resolution_get_f) (
    int unit);

/**
 * \brief returns define data of fpc_ptr_size
 * Module - 'esb', Submodule - 'general', data - 'fpc_ptr_size'
 * size of fpc ptr
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     fpc_ptr_size - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_fpc_ptr_size_get_f) (
    int unit);

/**
 * \brief returns define data of nof_buffers_per_fpc
 * Module - 'esb', Submodule - 'general', data - 'nof_buffers_per_fpc'
 * Number of buffers per Free Pointer Controller
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_buffers_per_fpc - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_nof_buffers_per_fpc_get_f) (
    int unit);

/**
 * \brief returns define data of l1_mux_cal_res
 * Module - 'esb', Submodule - 'general', data - 'l1_mux_cal_res'
 * Resolution of the MUX calendar used for L1 ports. units: Mbps
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     l1_mux_cal_res - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_l1_mux_cal_res_get_f) (
    int unit);

/**
 * \brief returns define data of tm_egr_queuing_mux_cal_res
 * Module - 'esb', Submodule - 'general', data - 'tm_egr_queuing_mux_cal_res'
 * Resolution of the MUX calendar used for TM Egr Queuing ports. units: Mbps
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     tm_egr_queuing_mux_cal_res - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_tm_egr_queuing_mux_cal_res_get_f) (
    int unit);

/**
 * \brief returns numeric data of txi_irdy_th_factor
 * Module - 'esb', Submodule - 'general', data - 'txi_irdy_th_factor'
 * Value of a per-device parameter used to calculate the IRDY threshold
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     txi_irdy_th_factor - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_txi_irdy_th_factor_get_f) (
    int unit);

/**
 * \brief returns numeric data of txi_irdy_th_divident
 * Module - 'esb', Submodule - 'general', data - 'txi_irdy_th_divident'
 * Value of a per-device parameter used to calculate the IRDY threshold
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     txi_irdy_th_divident - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_txi_irdy_th_divident_get_f) (
    int unit);

/**
 * \brief returns numeric data of slow_port_dedicated_queue
 * Module - 'esb', Submodule - 'general', data - 'slow_port_dedicated_queue'
 * Dedicated ESB queue ID for Slow Ports (under 10G) to be used in the MUX calendar
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     slow_port_dedicated_queue - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_slow_port_dedicated_queue_get_f) (
    int unit);

/**
 * \brief returns numeric data of flexe_l1_dest_dedicated_queue
 * Module - 'esb', Submodule - 'general', data - 'flexe_l1_dest_dedicated_queue'
 * Dedicated ESB queue ID for FlexE L1 destination ports to be used in the MUX calendar
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     flexe_l1_dest_dedicated_queue - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_general_flexe_l1_dest_dedicated_queue_get_f) (
    int unit);

/**
 * \brief get table if_esb_params entry 
 * Configuration parameters for ESB
 * 
 * \param [in] unit - unit #
 * \param [in] idx - Table index
 * 
 * \return
 *     if_esb_params - returns the relevant entry values grouped in struct - see dnx_data_esb_general_if_esb_params_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef const dnx_data_esb_general_if_esb_params_t *(
    *dnx_data_esb_general_if_esb_params_get_f) (
    int unit,
    int idx);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_ESB - GENERAL:
 * {
 */
/**
 * \brief Interface for esb general data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_esb_general_feature_get_f feature_get;
    /**
     * returns define data of total_nof_esb_queues
     */
    dnx_data_esb_general_total_nof_esb_queues_get_f total_nof_esb_queues_get;
    /**
     * returns define data of nof_nif_eth_queues
     */
    dnx_data_esb_general_nof_nif_eth_queues_get_f nof_nif_eth_queues_get;
    /**
     * returns define data of nof_nif_ilkn_queues
     */
    dnx_data_esb_general_nof_nif_ilkn_queues_get_f nof_nif_ilkn_queues_get;
    /**
     * returns define data of nof_ilkn_tdm_queues
     */
    dnx_data_esb_general_nof_ilkn_tdm_queues_get_f nof_ilkn_tdm_queues_get;
    /**
     * returns define data of nof_flexe_queues
     */
    dnx_data_esb_general_nof_flexe_queues_get_f nof_flexe_queues_get;
    /**
     * returns define data of nof_special_queues
     */
    dnx_data_esb_general_nof_special_queues_get_f nof_special_queues_get;
    /**
     * returns define data of nof_reserved_queues
     */
    dnx_data_esb_general_nof_reserved_queues_get_f nof_reserved_queues_get;
    /**
     * returns define data of nof_fpc
     */
    dnx_data_esb_general_nof_fpc_get_f nof_fpc_get;
    /**
     * returns define data of total_nof_buffers
     */
    dnx_data_esb_general_total_nof_buffers_get_f total_nof_buffers_get;
    /**
     * returns define data of mux_cal_len
     */
    dnx_data_esb_general_mux_cal_len_get_f mux_cal_len_get;
    /**
     * returns define data of ilkn_credits_resolution
     */
    dnx_data_esb_general_ilkn_credits_resolution_get_f ilkn_credits_resolution_get;
    /**
     * returns define data of ilkn_credits_rate_resolution
     */
    dnx_data_esb_general_ilkn_credits_rate_resolution_get_f ilkn_credits_rate_resolution_get;
    /**
     * returns define data of fpc_ptr_size
     */
    dnx_data_esb_general_fpc_ptr_size_get_f fpc_ptr_size_get;
    /**
     * returns define data of nof_buffers_per_fpc
     */
    dnx_data_esb_general_nof_buffers_per_fpc_get_f nof_buffers_per_fpc_get;
    /**
     * returns define data of l1_mux_cal_res
     */
    dnx_data_esb_general_l1_mux_cal_res_get_f l1_mux_cal_res_get;
    /**
     * returns define data of tm_egr_queuing_mux_cal_res
     */
    dnx_data_esb_general_tm_egr_queuing_mux_cal_res_get_f tm_egr_queuing_mux_cal_res_get;
    /**
     * returns numeric data of txi_irdy_th_factor
     */
    dnx_data_esb_general_txi_irdy_th_factor_get_f txi_irdy_th_factor_get;
    /**
     * returns numeric data of txi_irdy_th_divident
     */
    dnx_data_esb_general_txi_irdy_th_divident_get_f txi_irdy_th_divident_get;
    /**
     * returns numeric data of slow_port_dedicated_queue
     */
    dnx_data_esb_general_slow_port_dedicated_queue_get_f slow_port_dedicated_queue_get;
    /**
     * returns numeric data of flexe_l1_dest_dedicated_queue
     */
    dnx_data_esb_general_flexe_l1_dest_dedicated_queue_get_f flexe_l1_dest_dedicated_queue_get;
    /**
     * get table if_esb_params entry 
     */
    dnx_data_esb_general_if_esb_params_get_f if_esb_params_get;
    /**
     * get general info table about table (for example key size)if_esb_params info
     */
    dnxc_data_table_info_get_f if_esb_params_info_get;
} dnx_data_if_esb_general_t;

/*
 * }
 */

/*
 * SUBMODULE  - DBAL:
 * ESB DBAL defines
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
    _dnx_data_esb_dbal_feature_nof
} dnx_data_esb_dbal_feature_e;

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
    *dnx_data_esb_dbal_feature_get_f) (
    int unit,
    dnx_data_esb_dbal_feature_e feature);

/**
 * \brief returns define data of pm_internal_port_size
 * Module - 'esb', Submodule - 'dbal', data - 'pm_internal_port_size'
 * Size in bits of the port number internal to the port macro field in table for ESB queue to EGQ interface mapping
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     pm_internal_port_size - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_dbal_pm_internal_port_size_get_f) (
    int unit);

/**
 * \brief returns define data of counter_config_mask_size
 * Module - 'esb', Submodule - 'dbal', data - 'counter_config_mask_size'
 * Size in bits of the mask for ESB queues to display in the ESB debug counter
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     counter_config_mask_size - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
typedef uint32(
    *dnx_data_esb_dbal_counter_config_mask_size_get_f) (
    int unit);

/*
 * SUBMODULE INTERFACE DNX_DATA_IF_ESB - DBAL:
 * {
 */
/**
 * \brief Interface for esb dbal data
 */
typedef struct
{
    /**
     * get submodule feature - disabled/enabled
     */
    dnx_data_esb_dbal_feature_get_f feature_get;
    /**
     * returns define data of pm_internal_port_size
     */
    dnx_data_esb_dbal_pm_internal_port_size_get_f pm_internal_port_size_get;
    /**
     * returns define data of counter_config_mask_size
     */
    dnx_data_esb_dbal_counter_config_mask_size_get_f counter_config_mask_size_get;
} dnx_data_if_esb_dbal_t;

/*
 * }
 */

/*
 * MODULE INTERFACE DNX_DATA_IF_ESB:
 * {
 */
/**
 * \brief Interface for esb data
 */
typedef struct
{
    /**
     * Interface for esb general data
     */
    dnx_data_if_esb_general_t general;
    /**
     * Interface for esb dbal data
     */
    dnx_data_if_esb_dbal_t dbal;
} dnx_data_if_esb_t;

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
extern dnx_data_if_esb_t dnx_data_esb;
/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_ESB_H_*/
/* *INDENT-ON* */
