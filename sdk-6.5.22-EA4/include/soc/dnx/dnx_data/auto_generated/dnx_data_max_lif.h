/** \file dnx_data_max_lif.h
 * 
 * MODULE DATA MAX - 
 * Maintain max value (of all devices) per define
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
#ifndef _DNX_DATA_MAX_LIF_H_
/*{*/
#define _DNX_DATA_MAX_LIF_H_
/*
 * INCLUDE FILES:
 * {
 */
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
 * MAX DEFINES:
 * {
 */
/**
 * \brief
 * Some lif sizes are not taken from dnx data, but from the mdb. Use this flag to indicate if it should be used, or just use default minimal value.
 */
#define DNX_DATA_MAX_LIF_GLOBAL_LIF_USE_MDB_SIZE (1)

/**
 * \brief
 * Amount of out RIFs one GLEM entry can hold for optimized GLEM usage is 4.
 */
#define DNX_DATA_MAX_LIF_GLOBAL_LIF_GLEM_RIF_OPTIMIZATION_ENABLED (1)

/**
 * \brief
 * ERPP GLEM trap requires NULL LIF to be added to GLEM by default.
 */
#define DNX_DATA_MAX_LIF_GLOBAL_LIF_NULL_LIF (1)

/**
 * \brief
 * This is the size of a single index in the inlif table. Divide the entry bit size by this to get its size in indexes.
 */
#define DNX_DATA_MAX_LIF_IN_LIF_INLIF_INDEX_BIT_SIZE (60)

/**
 * \brief
 * The minimum size in indexes of an inlif entry
 */
#define DNX_DATA_MAX_LIF_IN_LIF_INLIF_MINIMUM_INDEX_SIZE (2)

/**
 * \brief
 * The size of the inlif resource's n-packs. The packs are used to prevent fragmentation by assigning each pack a fixed entry size. The pack should be the least common denominator of every possible entry side. See the SDD for full description.
 */
#define DNX_DATA_MAX_LIF_IN_LIF_INLIF_RESOURCE_TAG_SIZE (6)

/**
 * \brief
 * Number of possible in_lif profiles.
 */
#define DNX_DATA_MAX_LIF_IN_LIF_NOF_IN_LIF_PROFILES (256)

/**
 * \brief
 * Number of used in_lif_profile bits out of 8.
 */
#define DNX_DATA_MAX_LIF_IN_LIF_NOF_USED_IN_LIF_PROFILE_BITS (7)

/**
 * \brief
 * Number of used eth_rif_profile bits.
 */
#define DNX_DATA_MAX_LIF_IN_LIF_NOF_USED_ETH_RIF_PROFILE_BITS (7)

/**
 * \brief
 * Number of possible in_lif profile incoming orientation groups.
 */
#define DNX_DATA_MAX_LIF_IN_LIF_NOF_IN_LIF_INCOMING_ORIENTATION (8)

/**
 * \brief
 * Number of possible eth_rif profiles.
 */
#define DNX_DATA_MAX_LIF_IN_LIF_NOF_ETH_RIF_PROFILES (128)

/**
 * \brief
 * Number of da_not_found_destination profiles.
 */
#define DNX_DATA_MAX_LIF_IN_LIF_NOF_DA_NOT_FOUND_DESTINATION_PROFILES (4)

/**
 * \brief
 * Default da_not_found_destination profile.
 */
#define DNX_DATA_MAX_LIF_IN_LIF_DEFAULT_DA_NOT_FOUND_DESTINATION_PROFILE (0)

/**
 * \brief
 * local in_lif id performing trap with drop action.
 */
#define DNX_DATA_MAX_LIF_IN_LIF_DROP_IN_LIF (0)

/**
 * \brief
 * Enable/Disable the MC bridge fallback functionality based on LIF profile
 */
#define DNX_DATA_MAX_LIF_IN_LIF_MC_BRIDGE_FALLBACK_PER_LIF_PROFILE (1)

/**
 * \brief
 * Max number of orientation bits in IOP mode
 */
#define DNX_DATA_MAX_LIF_IN_LIF_MAX_NUMBER_OF_ORIENTATION_BITS_IOP_MODE (2)

/**
 * \brief
 * Max number of egress lif_profile bits in IOP mode
 */
#define DNX_DATA_MAX_LIF_IN_LIF_MAX_NUMBER_OF_EGRESS_LIF_PROFILE_BITS_IOP_MODE (2)

/**
 * \brief
 * Number of possible out_lif profiles.
 */
#define DNX_DATA_MAX_LIF_OUT_LIF_NOF_OUT_LIF_PROFILES (64)

/**
 * \brief
 * Number of possible erpp_out_lif profiles.
 */
#define DNX_DATA_MAX_LIF_OUT_LIF_NOF_ERPP_OUT_LIF_PROFILES (8)

/**
 * \brief
 * Number of possible out_lif profile outgoing orientation groups.
 */
#define DNX_DATA_MAX_LIF_OUT_LIF_NOF_OUT_LIF_OUTGOING_ORIENTATION (8)

/**
 * \brief
 * The number of bits in the outrif profile from GLEM.
 */
#define DNX_DATA_MAX_LIF_OUT_LIF_OUTRIF_PROFILE_WIDTH (4)

/**
 * \brief
 * The number of bits in GLEM result.
 */
#define DNX_DATA_MAX_LIF_OUT_LIF_GLEM_RESULT (20)

/**
 * \brief
 * How many bits from the outlif pointer are used to indicate the physical bank.
 */
#define DNX_DATA_MAX_LIF_OUT_LIF_PHYSICAL_BANK_POINTER_SIZE (7)

/**
 * \brief
 * pointer size used to access eedb banks.
 */
#define DNX_DATA_MAX_LIF_OUT_LIF_OUTLIF_EEDB_BANKS_POINTER_SIZE (19)

/**
 * \brief
 * global out lif msb bits (by the given resolution), used as key to bytes to add sop table
 */
#define DNX_DATA_MAX_LIF_OUT_LIF_GLOBAL_OUTLIF_BTA_SOP_RESOLUTION (1024)

/**
 * \brief
 * The number of bits in the pmf outlif profile
 */
#define DNX_DATA_MAX_LIF_OUT_LIF_PMF_OUTLIF_PROFILE_WIDTH (1)

/**
 * \brief
 * The number of bits in the pmf outrif profile
 */
#define DNX_DATA_MAX_LIF_OUT_LIF_PMF_OUTRIF_PROFILE_WIDTH (1)

/**
 * \brief
 * Maximal number of logical lif tables (inlif + outlif)
 */
#define DNX_DATA_MAX_LIF_LIF_TABLE_MANAGER_MAX_NOF_LIF_TABLES (100)

/**
 * \brief
 * Maximal number of result types in lif table
 */
#define DNX_DATA_MAX_LIF_LIF_TABLE_MANAGER_MAX_NOF_RESULT_TYPES (30)

/**
 * \brief
 * Maximal number of fields for one result types in lif table
 */
#define DNX_DATA_MAX_LIF_LIF_TABLE_MANAGER_MAX_FIELDS_PER_RESULT_TYPE (64)

/**
 * \brief
 * Maximal number of fields in a lif table
 */
#define DNX_DATA_MAX_LIF_LIF_TABLE_MANAGER_MAX_FIELDS_PER_TABLE (64)

/*
 * }
 */

/*}*/
#endif /*_DNX_DATA_MAX_LIF_H_*/
/* *INDENT-ON* */
