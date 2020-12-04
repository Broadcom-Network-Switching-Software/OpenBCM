/**
 * \file algo_oamp_v1.h Internal DNX OAMP Template Management
ent This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
ent 
ent Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_OAMP_V1_H_INCLUDED
/*
 * { 
 */
#define ALGO_OAMP_V1_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/oam.h>

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/**
 * Resource name defines for algo oamp
 * \see
 * dnx_oam_init_templates
 * {
 */

/**
 * \brief - Printing callback for the OAM/BFD MPLS/PWE push 
 *        profile templates.  The push profiles are used to
 *        construct the packets transmitted from MPLS/PWE
 *        OAM/BFD endpoints.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  The 
 *        OAM/BFD MPLS/PWE PUSH profile structure
 *        dnx_oam_push_profile_t is defined and described above.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Callback function for printing OAM MEP profile template data
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - MEP profile template data to be printed.
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_algo_oamp_v1_mep_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the MAC DA MSB templates,
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  T
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oam_oamp_v1_da_mac_msb_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the MAC DA LSB templates,
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  T
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_oam_oamp_v1_da_mac_lsb_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - initialize templates for all oamp profile types for JR2 family.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_oamp_v1_init(
    int unit);

/*
 * } 
 */
#endif /* ALGO_OAMP_V1_H_INCLUDED */
