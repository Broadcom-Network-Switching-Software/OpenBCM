/** \file dnx_data_internal_ecgm.h
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
#ifndef _DNX_DATA_INTERNAL_ECGM_H_
/*{*/
#define _DNX_DATA_INTERNAL_ECGM_H_
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>
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
    dnx_data_ecgm_submodule_general,
    dnx_data_ecgm_submodule_core_resources,
    dnx_data_ecgm_submodule_port_resources,
    dnx_data_ecgm_submodule_delete_fifo,
    dnx_data_ecgm_submodule_info,

    /**
     * Must be last one!
     */
    _dnx_data_ecgm_submodule_nof
} dnx_data_ecgm_submodule_e;

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
int dnx_data_ecgm_general_feature_get(
    int unit,
    dnx_data_ecgm_general_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_ecgm_general_define_ecgm_is_supported,

    /**
     * Must be last one!
     */
    _dnx_data_ecgm_general_define_nof
} dnx_data_ecgm_general_define_e;

/* Get Data */
/**
 * \brief returns define data of ecgm_is_supported
 * Module - 'ecgm', Submodule - 'general', data - 'ecgm_is_supported'
 * indicate if egress congestion management block is supported
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     ecgm_is_supported - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_general_ecgm_is_supported_get(
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
    _dnx_data_ecgm_general_table_nof
} dnx_data_ecgm_general_table_e;

/* Get Data */
/* Get Value Str */
/* Get Table info */
/*
 * SUBMODULE CORE_RESOURCES:
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
int dnx_data_ecgm_core_resources_feature_get(
    int unit,
    dnx_data_ecgm_core_resources_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_ecgm_core_resources_define_nof_sp,
    dnx_data_ecgm_core_resources_define_nof_interface_profiles,
    dnx_data_ecgm_core_resources_define_nof_port_profiles,
    dnx_data_ecgm_core_resources_define_total_pds,
    dnx_data_ecgm_core_resources_define_total_pds_nof_bits,
    dnx_data_ecgm_core_resources_define_total_dbs,
    dnx_data_ecgm_core_resources_define_total_dbs_nof_bits,
    dnx_data_ecgm_core_resources_define_reserved_dbs_per_sp_nof_bits,
    dnx_data_ecgm_core_resources_define_max_alpha_value,
    dnx_data_ecgm_core_resources_define_min_alpha_value,
    dnx_data_ecgm_core_resources_define_max_core_bandwidth_Mbps,

    /**
     * Must be last one!
     */
    _dnx_data_ecgm_core_resources_define_nof
} dnx_data_ecgm_core_resources_define_e;

/* Get Data */
/**
 * \brief returns define data of nof_sp
 * Module - 'ecgm', Submodule - 'core_resources', data - 'nof_sp'
 * number of service pools
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_sp - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_core_resources_nof_sp_get(
    int unit);

/**
 * \brief returns define data of nof_interface_profiles
 * Module - 'ecgm', Submodule - 'core_resources', data - 'nof_interface_profiles'
 * total interface profiles per core
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_interface_profiles - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_core_resources_nof_interface_profiles_get(
    int unit);

/**
 * \brief returns define data of nof_port_profiles
 * Module - 'ecgm', Submodule - 'core_resources', data - 'nof_port_profiles'
 * total port profiles per core
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_port_profiles - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_core_resources_nof_port_profiles_get(
    int unit);

/**
 * \brief returns define data of total_pds
 * Module - 'ecgm', Submodule - 'core_resources', data - 'total_pds'
 * total packet descriptors per core
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     total_pds - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_core_resources_total_pds_get(
    int unit);

/**
 * \brief returns define data of total_pds_nof_bits
 * Module - 'ecgm', Submodule - 'core_resources', data - 'total_pds_nof_bits'
 * number of bits for total packet descriptors
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     total_pds_nof_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_core_resources_total_pds_nof_bits_get(
    int unit);

/**
 * \brief returns define data of total_dbs
 * Module - 'ecgm', Submodule - 'core_resources', data - 'total_dbs'
 * total data buffers per core
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     total_dbs - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_core_resources_total_dbs_get(
    int unit);

/**
 * \brief returns define data of total_dbs_nof_bits
 * Module - 'ecgm', Submodule - 'core_resources', data - 'total_dbs_nof_bits'
 * number of bits for total data buffers
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     total_dbs_nof_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_core_resources_total_dbs_nof_bits_get(
    int unit);

/**
 * \brief returns define data of reserved_dbs_per_sp_nof_bits
 * Module - 'ecgm', Submodule - 'core_resources', data - 'reserved_dbs_per_sp_nof_bits'
 * Number of bits for total reserved data buffers per Service Pool
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     reserved_dbs_per_sp_nof_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_core_resources_reserved_dbs_per_sp_nof_bits_get(
    int unit);

/**
 * \brief returns define data of max_alpha_value
 * Module - 'ecgm', Submodule - 'core_resources', data - 'max_alpha_value'
 * max value for FADT alpha
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_alpha_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_core_resources_max_alpha_value_get(
    int unit);

/**
 * \brief returns define data of min_alpha_value
 * Module - 'ecgm', Submodule - 'core_resources', data - 'min_alpha_value'
 * min value for FADT alpha
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     min_alpha_value - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_core_resources_min_alpha_value_get(
    int unit);

/**
 * \brief returns define data of max_core_bandwidth_Mbps
 * Module - 'ecgm', Submodule - 'core_resources', data - 'max_core_bandwidth_Mbps'
 * maximum bandwidth pec core in Mbps for ecgm calculation
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_core_bandwidth_Mbps - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_core_resources_max_core_bandwidth_Mbps_get(
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
    _dnx_data_ecgm_core_resources_table_nof
} dnx_data_ecgm_core_resources_table_e;

/* Get Data */
/* Get Value Str */
/* Get Table info */
/*
 * SUBMODULE PORT_RESOURCES:
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
int dnx_data_ecgm_port_resources_feature_get(
    int unit,
    dnx_data_ecgm_port_resources_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_ecgm_port_resources_define_max_nof_ports,

    /**
     * Must be last one!
     */
    _dnx_data_ecgm_port_resources_define_nof
} dnx_data_ecgm_port_resources_define_e;

/* Get Data */
/**
 * \brief returns define data of max_nof_ports
 * Module - 'ecgm', Submodule - 'port_resources', data - 'max_nof_ports'
 * maximum num of ports per core for ecgm calculation
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_nof_ports - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_port_resources_max_nof_ports_get(
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
    _dnx_data_ecgm_port_resources_table_nof
} dnx_data_ecgm_port_resources_table_e;

/* Get Data */
/* Get Value Str */
/* Get Table info */
/*
 * SUBMODULE DELETE_FIFO:
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
int dnx_data_ecgm_delete_fifo_feature_get(
    int unit,
    dnx_data_ecgm_delete_fifo_feature_e feature);

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
    _dnx_data_ecgm_delete_fifo_define_nof
} dnx_data_ecgm_delete_fifo_define_e;

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
    dnx_data_ecgm_delete_fifo_table_thresholds,

    /**
     * Must be last one!
     */
    _dnx_data_ecgm_delete_fifo_table_nof
} dnx_data_ecgm_delete_fifo_table_e;

/* Get Data */
/**
 * \brief get table thresholds entry 
 * Delete FIFO thresholds
 * 
 * \param [in] unit - unit #
 * \param [in] fifo_type - fifo type to get thresholds to.
 * 
 * \return
 *     thresholds - returns the relevant entry values grouped in struct - see dnx_data_ecgm_delete_fifo_thresholds_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnx_data_ecgm_delete_fifo_thresholds_t * dnx_data_ecgm_delete_fifo_thresholds_get(
    int unit,
    int fifo_type);

/* Get Value Str */
/**
 * \brief Get table value as string
 * Module - 'ecgm', Submodule - 'delete_fifo', table - 'thresholds'
 * Delete FIFO thresholds
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
shr_error_e dnx_data_ecgm_delete_fifo_thresholds_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/* Get Table info */
/**
 * \brief get general info table about table (for example key size)thresholds info
 * Delete FIFO thresholds
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     thresholds - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnx_data_ecgm_delete_fifo_thresholds_info_get(
    int unit);

/*
 * SUBMODULE INFO:
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
int dnx_data_ecgm_info_feature_get(
    int unit,
    dnx_data_ecgm_info_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_ecgm_info_define_fc_q_pair_vector_select_nof_bits,
    dnx_data_ecgm_info_define_fc_q_pair_vector_nof_presented,
    dnx_data_ecgm_info_define_fc_egr_if_vector_select_nof_bits,
    dnx_data_ecgm_info_define_fc_egr_if_vector_nof_presented,
    dnx_data_ecgm_info_define_nof_dropped_reasons_rqp,
    dnx_data_ecgm_info_define_nof_dropped_reasons_pqp,
    dnx_data_ecgm_info_define_nof_bits_in_pd_count,

    /**
     * Must be last one!
     */
    _dnx_data_ecgm_info_define_nof
} dnx_data_ecgm_info_define_e;

/* Get Data */
/**
 * \brief returns define data of fc_q_pair_vector_select_nof_bits
 * Module - 'ecgm', Submodule - 'info', data - 'fc_q_pair_vector_select_nof_bits'
 * Number of bits of the Q-pair debug vector selector for EGQ to SCH per Q-pair FC
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     fc_q_pair_vector_select_nof_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_info_fc_q_pair_vector_select_nof_bits_get(
    int unit);

/**
 * \brief returns define data of fc_q_pair_vector_nof_presented
 * Module - 'ecgm', Submodule - 'info', data - 'fc_q_pair_vector_nof_presented'
 * Number of presented indications of the Q-pair debug vector for EGQ to SCH per Q-pair FC
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     fc_q_pair_vector_nof_presented - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_info_fc_q_pair_vector_nof_presented_get(
    int unit);

/**
 * \brief returns define data of fc_egr_if_vector_select_nof_bits
 * Module - 'ecgm', Submodule - 'info', data - 'fc_egr_if_vector_select_nof_bits'
 * Number of bits of the Egr IF debug vector selector for ECGM to EGQ per Egr IF FC
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     fc_egr_if_vector_select_nof_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_info_fc_egr_if_vector_select_nof_bits_get(
    int unit);

/**
 * \brief returns define data of fc_egr_if_vector_nof_presented
 * Module - 'ecgm', Submodule - 'info', data - 'fc_egr_if_vector_nof_presented'
 * Number of presented indications of the Egr IF debug vector for EGQ to SCH per Egr IF FC
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     fc_egr_if_vector_nof_presented - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_info_fc_egr_if_vector_nof_presented_get(
    int unit);

/**
 * \brief returns define data of nof_dropped_reasons_rqp
 * Module - 'ecgm', Submodule - 'info', data - 'nof_dropped_reasons_rqp'
 * number of reasons for the dropped packets in RQP
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_dropped_reasons_rqp - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_info_nof_dropped_reasons_rqp_get(
    int unit);

/**
 * \brief returns define data of nof_dropped_reasons_pqp
 * Module - 'ecgm', Submodule - 'info', data - 'nof_dropped_reasons_pqp'
 * number of reasons for the dropped packets in PQP
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_dropped_reasons_pqp - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_info_nof_dropped_reasons_pqp_get(
    int unit);

/**
 * \brief returns define data of nof_bits_in_pd_count
 * Module - 'ecgm', Submodule - 'info', data - 'nof_bits_in_pd_count'
 * number of bits in PD COUNT
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_bits_in_pd_count - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_ecgm_info_nof_bits_in_pd_count_get(
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
    dnx_data_ecgm_info_table_dropped_reason_rqp,
    dnx_data_ecgm_info_table_dropped_reason_pqp,

    /**
     * Must be last one!
     */
    _dnx_data_ecgm_info_table_nof
} dnx_data_ecgm_info_table_e;

/* Get Data */
/**
 * \brief get table dropped_reason_rqp entry 
 * reasons for the dropped and rejected packets in RQP
 * 
 * \param [in] unit - unit #
 * \param [in] index - this index mapped to the bit in RQP_DISCARD_REASONS
 * 
 * \return
 *     dropped_reason_rqp - returns the relevant entry values grouped in struct - see dnx_data_ecgm_info_dropped_reason_rqp_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnx_data_ecgm_info_dropped_reason_rqp_t * dnx_data_ecgm_info_dropped_reason_rqp_get(
    int unit,
    int index);

/**
 * \brief get table dropped_reason_pqp entry 
 * reasons for the dropped and rejected packets in PQP
 * 
 * \param [in] unit - unit #
 * \param [in] index - this index mapped to the bit in PQP_DISCARD_REASONS
 * 
 * \return
 *     dropped_reason_pqp - returns the relevant entry values grouped in struct - see dnx_data_ecgm_info_dropped_reason_pqp_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnx_data_ecgm_info_dropped_reason_pqp_t * dnx_data_ecgm_info_dropped_reason_pqp_get(
    int unit,
    int index);

/* Get Value Str */
/**
 * \brief Get table value as string
 * Module - 'ecgm', Submodule - 'info', table - 'dropped_reason_rqp'
 * reasons for the dropped and rejected packets in RQP
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
shr_error_e dnx_data_ecgm_info_dropped_reason_rqp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/**
 * \brief Get table value as string
 * Module - 'ecgm', Submodule - 'info', table - 'dropped_reason_pqp'
 * reasons for the dropped and rejected packets in PQP
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
shr_error_e dnx_data_ecgm_info_dropped_reason_pqp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/* Get Table info */
/**
 * \brief get general info table about table (for example key size)dropped_reason_rqp info
 * reasons for the dropped and rejected packets in RQP
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     dropped_reason_rqp - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnx_data_ecgm_info_dropped_reason_rqp_info_get(
    int unit);

/**
 * \brief get general info table about table (for example key size)dropped_reason_pqp info
 * reasons for the dropped and rejected packets in PQP
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     dropped_reason_pqp - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnx_data_ecgm_info_dropped_reason_pqp_info_get(
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
shr_error_e dnx_data_ecgm_init(
    int unit,
    dnxc_data_module_t *module_data);

/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_INTERNAL_ECGM_H_*/
/* *INDENT-ON* */
