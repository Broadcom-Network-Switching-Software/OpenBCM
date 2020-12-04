/** \file src/bcm/dnx/bier/bier_imp.h
 * Internal DNX MIRROR PROFILE APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef _DNX_MULTICAST_IMP_INCLUDED__
/*
 * { 
 */
#define _DNX_MULTICAST_IMP_INCLUDED__

#ifndef BCM_DNX_SUPPORT

#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/multicast.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_bier.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */
#define DNX_BIER_ALGO_RES_STR                     "BIER_OCCUPANCY"
/*
 * }
 */

/**
 * holds the MCDB entry values for BIER.
 */
typedef struct multicast_mcdb_dbal_fields_s
{
    uint32 bmp[_SHR_BITDCLSIZE(DNX_DATA_MAX_BIER_PARAMS_BITSTRING_SIZE)];
    uint32 cud;
    uint32 dest;
    uint32 tcm;
    uint32 valid;
    uint32 format;
} dnx_bier_mcdb_dbal_field_t;

/**
 * \brief - write a BIER entry to MCDB through DBAL interface
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] is_ingress - indicates whether ingress or egress table should be updated
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] bfr_id - determines the BFR-ID index to be updates
 *   \param [in] entry_val - structure filled with the entry values
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bier_entry_set(
    int unit,
    uint32 is_ingress,
    int core_id,
    uint32 bfr_id,
    dnx_bier_mcdb_dbal_field_t * entry_val);

/**
 * \brief - reads a BIER entry from MCDB through DBAL interface
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] is_ingress - indicates whether ingress or egress table should be updated
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] bfr_id - determines the BFR-ID index to be updates
 *   \param [in] entry_val - structure filled with the entry values
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bier_entry_get(
    int unit,
    uint32 is_ingress,
    int core_id,
    uint32 bfr_id,
    dnx_bier_mcdb_dbal_field_t * entry_val);

/**
 * \brief - given MCDB entry returns the destination gport & CUD information according
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] is_ingress - indicates whether ingress or egress table should be updated
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] entry_val - structure filled with the entry values
 *   \param [out] dest - gport representing the first destination
 *   \param [out] cud - encaps id of the replication
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bier_convert_hw_to_gport(
    int unit,
    uint32 is_ingress,
    int core_id,
    dnx_bier_mcdb_dbal_field_t entry_val,
    bcm_gport_t * dest,
    bcm_if_t * cud);

/**
 * \brief - convert gport to MCDB HW values. According to the given gport, the entry values and core information
 *          are returned.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] is_ingress - indicates whether ingress or egress table should be updated
 *   \param [out] dest - gport representing the first destination
 *   \param [out] encap - encaps id of the replication
 *   \param [in] core_id - core_id related to the gport
 *   \param [in] entry_val - structure filled with the entry values by this function
 *   \param [in] trunk_create - indicates weather to create or destroy the egress trunk profile.
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bier_convert_gport_to_hw(
    int unit,
    uint32 is_ingress,
    bcm_gport_t dest,
    bcm_if_t encap,
    int *core_id,
    dnx_bier_mcdb_dbal_field_t * entry_val,
    uint8 trunk_create);

/**
 * \brief - sets invalid to all members of the group
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - bier multicast group
 *   \param [in] set_size - set nof members
 *
  * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bier_group_invalid_set(
    int unit,
    bcm_multicast_t group,
    int set_size);

/**
 * \brief - update bier dbal predefined field(WB only)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] region_size - The size of each region (ingress/egress 0/egress 1)

 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e bier_dbal_update_predef_values(
    int unit,
    uint32 region_size);

/**
 * \brief - bier offset configuration
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] region_size - The size of each region (ingress/egress 0/egress 1)
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_bier_dbal_init(
    int unit,
    uint32 region_size);

/*
 * } 
 */
#endif/*_DNX_MULTICAST_IMP_INCLUDED__*/
