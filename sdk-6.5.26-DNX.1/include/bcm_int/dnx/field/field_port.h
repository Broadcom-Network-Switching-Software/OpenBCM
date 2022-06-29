/**
 * \file bcm_int/dnx/field/field_port.h
 *
 * Field Processor definitions related to port profiles of different types.
 *
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef FIELD_FIELD_PORT_H_INCLUDED
/* { */
#define FIELD_FIELD_PORT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files
 * {
 */
#include <shared/utilex/utilex_rhlist.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <include/bcm_int/dnx/lif/in_lif_profile.h>
#include <include/bcm_int/dnx/lif/out_lif_profile.h>
#include <bcm/field.h>
/*
 * }
 */

/*
 * Defines
 * {
 */

/*
 * } Defines
 */

/**
* \brief        Set the bit range inside misc. profile qualifiers
* \param [in] unit - Device Id
* \param [in] dnx_profile_type - what kind of profile we are dealing with
* \param [in] start_bit - first bit dedicated to PMF
* \param [in] nof_bits - number of bits in the profile dedicated to PMF
* \return
*   shr_error_e - Error Type
* \remark
*   * Used by In Lif manager to set PMF dedicated bits for FP usage
* \see
*   * None
*/
shr_error_e dnx_field_port_profile_bits_set(
    int unit,
    dnx_field_port_porfile_type_e dnx_profile_type,
    int start_bit,
    int nof_bits);

/**
* \brief        Get the bit range inside misc. profile qualifiers
* \param [in] unit - Device Id
* \param [in] dnx_profile_type - what kind of profile we are dealing with
* \param [out] start_bit_p - first bit dedicated to PMF
* \param [out] nof_bits_p - number of bits in the profile dedicated to PMF
* \return
*   shr_error_e - Error Type
* \remark
*   * Used by In Lif manager to set PMF dedicated bits for FP usage
* \see
*   * None
*/
shr_error_e dnx_field_port_profile_bits_get(
    int unit,
    dnx_field_port_porfile_type_e dnx_profile_type,
    unsigned int *start_bit_p,
    unsigned int *nof_bits_p);

/**
* \brief        Set the PMF data for port profile according to the profile type.
* \param [in] unit - Device Id
* \param [in] port_class - what kind of profile we are dealing with
* \param [in] port - Gport indicating the port
* \param [in] field_profile - PMF data to set in the port profile
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
shr_error_e dnx_field_port_profile_gport_set(
    int unit,
    bcm_port_class_t port_class,
    bcm_gport_t port,
    uint32 field_profile);

/**
* \brief        Get the PMF data for port profile according to the profile type.
* \param [in] unit - Device Id
* \param [in] port_class - what kind of profile we are dealing with
* \param [in] port - Gport indicating the port
* \param [out] field_profile_p - PMF data of the port profile
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
shr_error_e dnx_field_port_profile_gport_get(
    int unit,
    bcm_port_class_t port_class,
    bcm_gport_t port,
    uint32 *field_profile_p);

/**
* \brief        Set the PMF data for VSI profile
* \param [in] unit - Device Id
* \param [in] field_profile - PMF data to set in the VSI profile
* \param [in] vlan - Vlan
* \param [in] in_lif_profile_info - Structure holding in_lif_profile information
* \param [in] old_vsi_profile - old VSI profile
* \param [in,out] vsi_profile_p - new VSI Profile
* \param [out] last_profile - Structure holding  in_lif last profile that should be cleared from DBAL
*
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
shr_error_e dnx_field_port_profile_vsi_set(
    int unit,
    int field_profile,
    bcm_vlan_t vlan,
    in_lif_profile_info_t * in_lif_profile_info,
    int old_vsi_profile,
    int *vsi_profile_p,
    dnx_in_lif_profile_last_info_t * last_profile);

/**
* \brief        Get the PMF data for VSI profile
* \param [in] unit - Device Id
* \param [in] vsi_profile - VSI profile
* \param [out] field_profile_p - PMF data for the VSI profile
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
shr_error_e dnx_field_port_profile_vsi_get(
    int unit,
    int vsi_profile,
    int *field_profile_p);

/* } */
#endif
