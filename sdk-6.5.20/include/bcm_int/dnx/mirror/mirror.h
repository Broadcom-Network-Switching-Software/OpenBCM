/*
 * ! \file bcm_int/dnx/mirror/mirror.h Internal DNX MIRROR APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef _DNX_MIRROR_API_INCLUDED__
/*
 * { 
 */
#define _DNX_MIRROR_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/*
 * MACROs
 * {
 */

#define DNX_MIRROR_EGRESS_DISABLE_PROFILE_IDX                   7

/**
 * \brief - res mngr for egress vlan mirror application
 */
#define DNX_ALGO_SNIF_TEMP_MNGR_EGRESS_VLAN_PROFILES_MIRROR         "SNIF_EGRESS_VLAN_PROFILES_MIRROR"
/**
 * \brief - res mngr for ingress vlan mirror application
 */
#define DNX_ALGO_SNIF_TEMP_MNGR_INGRESS_VLAN_PROFILES_MIRROR         "SNIF_INGRESS_VLAN_PROFILES_MIRROR"

/*
 * \brief - egress to ingress profile index mapping is not 1:1. therefore this macro should be used
 * to convert bwtween the profile created and the value which needs to be configured at egress tables
 */
#define DNX_MIRROR_EGRESS_PROFILE_GET(profile_id) ((profile_id == DNX_MIRROR_EGRESS_DISABLE_PROFILE_IDX) ? 0 : \
            ((profile_id == 0) ? DNX_MIRROR_EGRESS_DISABLE_PROFILE_IDX : profile_id))

#define DNX_MIRROR_ON_DROP_INVALID_PROFILE                   (-1)
/*
 * }
 */

/**
 * \brief - Printing callback for the vlan profile template manager.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data - Pointer to template profile data.  This
 *        data represents profile id.
 * \return
 * \remark
 *   * A print callback function is required to create a
 *     template.  It is later used for BCM shell diagnostics.
 * \see
 *   * None
 */
void dnx_mirror_vlan_profile_print_cb(
    int unit,
    const void *data);
/**
 * \brief - Initialize dnx mirror module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * DNX data related SNIF module information
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * Mirror HW related regs/mems
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mirror_init(
    int unit);

/**
 * \brief - Deinitialize dnx mirror module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
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
shr_error_e dnx_mirror_deinit(
    int unit);

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * } 
 */
#endif/*_DNX_MIRROR_API_INCLUDED__*/
