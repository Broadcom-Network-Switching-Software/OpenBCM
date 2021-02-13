/** \file dnx_data_internal_trunk.h
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
#ifndef _DNX_DATA_INTERNAL_TRUNK_H_
/*{*/
#define _DNX_DATA_INTERNAL_TRUNK_H_
/*
 * INCLUDE FILES:
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
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
    dnx_data_trunk_submodule_parameters,
    dnx_data_trunk_submodule_psc,
    dnx_data_trunk_submodule_egress_trunk,
    dnx_data_trunk_submodule_graceful,

    /**
     * Must be last one!
     */
    _dnx_data_trunk_submodule_nof
} dnx_data_trunk_submodule_e;

/*
 * }
 */

/*
 * SUBMODULE PARAMETERS:
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
int dnx_data_trunk_parameters_feature_get(
    int unit,
    dnx_data_trunk_parameters_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_trunk_parameters_define_nof_pools,
    dnx_data_trunk_parameters_define_max_nof_members_in_pool,
    dnx_data_trunk_parameters_define_max_nof_groups_in_pool,
    dnx_data_trunk_parameters_define_spa_pool_shift,
    dnx_data_trunk_parameters_define_spa_type_shift,

    /**
     * Must be last one!
     */
    _dnx_data_trunk_parameters_define_nof
} dnx_data_trunk_parameters_define_e;

/* Get Data */
/**
 * \brief returns define data of nof_pools
 * Module - 'trunk', Submodule - 'parameters', data - 'nof_pools'
 * trunk module might be represented with pools in certain architectures, this number represents the number of pools currently supported for this device. each pool has a constant amount of members.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_pools - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_parameters_nof_pools_get(
    int unit);

/**
 * \brief returns define data of max_nof_members_in_pool
 * Module - 'trunk', Submodule - 'parameters', data - 'max_nof_members_in_pool'
 * the maximal amount of members that a pool can contain.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_nof_members_in_pool - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_parameters_max_nof_members_in_pool_get(
    int unit);

/**
 * \brief returns define data of max_nof_groups_in_pool
 * Module - 'trunk', Submodule - 'parameters', data - 'max_nof_groups_in_pool'
 * the maximal amount of groups that a pool can contain.
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     max_nof_groups_in_pool - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_parameters_max_nof_groups_in_pool_get(
    int unit);

/**
 * \brief returns define data of spa_pool_shift
 * Module - 'trunk', Submodule - 'parameters', data - 'spa_pool_shift'
 * bit shifts to describe the pool of a trunk in a System Port Aggregate
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     spa_pool_shift - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_parameters_spa_pool_shift_get(
    int unit);

/**
 * \brief returns define data of spa_type_shift
 * Module - 'trunk', Submodule - 'parameters', data - 'spa_type_shift'
 * bit shifts to describe trunk type in a System Port Aggregate
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     spa_type_shift - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_parameters_spa_type_shift_get(
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
    dnx_data_trunk_parameters_table_pool_info,

    /**
     * Must be last one!
     */
    _dnx_data_trunk_parameters_table_nof
} dnx_data_trunk_parameters_table_e;

/* Get Data */
/**
 * \brief get table pool_info entry 
 * this table contains the information per pool of trunks
 * 
 * \param [in] unit - unit #
 * \param [in] pool_index - pool index
 * 
 * \return
 *     pool_info - returns the relevant entry values grouped in struct - see dnx_data_trunk_parameters_pool_info_t
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnx_data_trunk_parameters_pool_info_t * dnx_data_trunk_parameters_pool_info_get(
    int unit,
    int pool_index);

/* Get Value Str */
/**
 * \brief Get table value as string
 * Module - 'trunk', Submodule - 'parameters', table - 'pool_info'
 * this table contains the information per pool of trunks
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
shr_error_e dnx_data_trunk_parameters_pool_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);

/* Get Table info */
/**
 * \brief get general info table about table (for example key size)pool_info info
 * this table contains the information per pool of trunks
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     pool_info - returns table info
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
const dnxc_data_table_info_t * dnx_data_trunk_parameters_pool_info_info_get(
    int unit);

/*
 * SUBMODULE PSC:
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
int dnx_data_trunk_psc_feature_get(
    int unit,
    dnx_data_trunk_psc_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_trunk_psc_define_consistant_hashing_nof_resources,
    dnx_data_trunk_psc_define_consistant_hashing_resource_to_entries_ratio,
    dnx_data_trunk_psc_define_consistant_hashing_small_group_size_in_bits,
    dnx_data_trunk_psc_define_smooth_division_max_nof_member,
    dnx_data_trunk_psc_define_smooth_division_entries_per_profile,

    /**
     * Must be last one!
     */
    _dnx_data_trunk_psc_define_nof
} dnx_data_trunk_psc_define_e;

/* Get Data */
/**
 * \brief returns numeric data of consistant_hashing_nof_resources
 * Module - 'trunk', Submodule - 'psc', data - 'consistant_hashing_nof_resources'
 * nof resources that CHM needs to manage - those are the reduced resources
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     consistant_hashing_nof_resources - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_psc_consistant_hashing_nof_resources_get(
    int unit);

/**
 * \brief returns numeric data of consistant_hashing_resource_to_entries_ratio
 * Module - 'trunk', Submodule - 'psc', data - 'consistant_hashing_resource_to_entries_ratio'
 * defines how many HW entries each resource represents
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     consistant_hashing_resource_to_entries_ratio - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_psc_consistant_hashing_resource_to_entries_ratio_get(
    int unit);

/**
 * \brief returns define data of consistant_hashing_small_group_size_in_bits
 * Module - 'trunk', Submodule - 'psc', data - 'consistant_hashing_small_group_size_in_bits'
 * size in bits of small groups in c-lag
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     consistant_hashing_small_group_size_in_bits - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_psc_consistant_hashing_small_group_size_in_bits_get(
    int unit);

/**
 * \brief returns define data of smooth_division_max_nof_member
 * Module - 'trunk', Submodule - 'psc', data - 'smooth_division_max_nof_member'
 * max number of member is trunk group with smooth division as PSC
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     smooth_division_max_nof_member - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_psc_smooth_division_max_nof_member_get(
    int unit);

/**
 * \brief returns define data of smooth_division_entries_per_profile
 * Module - 'trunk', Submodule - 'psc', data - 'smooth_division_entries_per_profile'
 * logical number of entries dedicated in the HW per SMD profile
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     smooth_division_entries_per_profile - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_psc_smooth_division_entries_per_profile_get(
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
    _dnx_data_trunk_psc_table_nof
} dnx_data_trunk_psc_table_e;

/* Get Data */
/* Get Value Str */
/* Get Table info */
/*
 * SUBMODULE EGRESS_TRUNK:
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
int dnx_data_trunk_egress_trunk_feature_get(
    int unit,
    dnx_data_trunk_egress_trunk_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_trunk_egress_trunk_define_nof,
    dnx_data_trunk_egress_trunk_define_nof_lb_keys_per_profile,
    dnx_data_trunk_egress_trunk_define_dbal_egress_trunk_index_multiplier,
    dnx_data_trunk_egress_trunk_define_dbal_entry_index_divider,
    dnx_data_trunk_egress_trunk_define_dbal_entry_index_msb_multiplier,
    dnx_data_trunk_egress_trunk_define_size_mode,
    dnx_data_trunk_egress_trunk_define_invalid_pp_dsp,

    /**
     * Must be last one!
     */
    _dnx_data_trunk_egress_trunk_define_nof
} dnx_data_trunk_egress_trunk_define_e;

/* Get Data */
/**
 * \brief returns numeric data of nof
 * Module - 'trunk', Submodule - 'egress_trunk', data - 'nof'
 * number of egress trunks
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_egress_trunk_nof_get(
    int unit);

/**
 * \brief returns numeric data of nof_lb_keys_per_profile
 * Module - 'trunk', Submodule - 'egress_trunk', data - 'nof_lb_keys_per_profile'
 * number of lb keys per Egress trunk
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     nof_lb_keys_per_profile - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_egress_trunk_nof_lb_keys_per_profile_get(
    int unit);

/**
 * \brief returns numeric data of dbal_egress_trunk_index_multiplier
 * Module - 'trunk', Submodule - 'egress_trunk', data - 'dbal_egress_trunk_index_multiplier'
 * multiplier size for mapping dbal key EGRESS_TRUNK_INDEX in dbal table TRUNK_EGRESS_MC_RESOLUTION
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     dbal_egress_trunk_index_multiplier - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_egress_trunk_dbal_egress_trunk_index_multiplier_get(
    int unit);

/**
 * \brief returns numeric data of dbal_entry_index_divider
 * Module - 'trunk', Submodule - 'egress_trunk', data - 'dbal_entry_index_divider'
 * divider size for mapping dbal key ENTRY_INDEX in dbal table TRUNK_EGRESS_MC_RESOLUTION
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     dbal_entry_index_divider - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_egress_trunk_dbal_entry_index_divider_get(
    int unit);

/**
 * \brief returns numeric data of dbal_entry_index_msb_multiplier
 * Module - 'trunk', Submodule - 'egress_trunk', data - 'dbal_entry_index_msb_multiplier'
 * multiplier size for mapping dbal key ENTRY_INDEX msb in dbal table TRUNK_EGRESS_MC_RESOLUTION
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     dbal_entry_index_msb_multiplier - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_egress_trunk_dbal_entry_index_msb_multiplier_get(
    int unit);

/**
 * \brief returns numeric data of size_mode
 * Module - 'trunk', Submodule - 'egress_trunk', data - 'size_mode'
 * HW mode relevant for group size
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     size_mode - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_egress_trunk_size_mode_get(
    int unit);

/**
 * \brief returns numeric data of invalid_pp_dsp
 * Module - 'trunk', Submodule - 'egress_trunk', data - 'invalid_pp_dsp'
 * invalid pp dsp that should be connected to invalid out port for egress trunk
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     invalid_pp_dsp - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_egress_trunk_invalid_pp_dsp_get(
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
    _dnx_data_trunk_egress_trunk_table_nof
} dnx_data_trunk_egress_trunk_table_e;

/* Get Data */
/* Get Value Str */
/* Get Table info */
/*
 * SUBMODULE GRACEFUL:
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
int dnx_data_trunk_graceful_feature_get(
    int unit,
    dnx_data_trunk_graceful_feature_e feature);

/*
 * Defines
 */
/**
 * \brief 
 * Define enum
 */
typedef enum
{
    dnx_data_trunk_graceful_define_first_hw_configuration,
    dnx_data_trunk_graceful_define_second_hw_configuration,

    /**
     * Must be last one!
     */
    _dnx_data_trunk_graceful_define_nof
} dnx_data_trunk_graceful_define_e;

/* Get Data */
/**
 * \brief returns define data of first_hw_configuration
 * Module - 'trunk', Submodule - 'graceful', data - 'first_hw_configuration'
 * hw value to indicate first configuration is currently used
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     first_hw_configuration - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_graceful_first_hw_configuration_get(
    int unit);

/**
 * \brief returns define data of second_hw_configuration
 * Module - 'trunk', Submodule - 'graceful', data - 'second_hw_configuration'
 * hw value to indicate second configuration is currently used
 * 
 * \param [in] unit - unit #
 * 
 * \return
 *     second_hw_configuration - the required data value
 * \remark
 *   * Auto-generated using device data autocoder
 * \see
 *   * None
 */
uint32 dnx_data_trunk_graceful_second_hw_configuration_get(
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
    _dnx_data_trunk_graceful_table_nof
} dnx_data_trunk_graceful_table_e;

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
shr_error_e dnx_data_trunk_init(
    int unit,
    dnxc_data_module_t *module_data);

/*
 * }
 */
/*}*/
#endif /*_DNX_DATA_INTERNAL_TRUNK_H_*/
/* *INDENT-ON* */
