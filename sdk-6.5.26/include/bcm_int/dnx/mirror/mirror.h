/*
 * ! \file bcm_int/dnx/mirror/mirror.h Internal DNX MIRROR APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */

#ifndef _DNX_MIRROR_API_INCLUDED__
/*
 * { 
 */
#define _DNX_MIRROR_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/*
 * MACROs
 * {
 */

/**
 * \brief - egress sniff disabled profile ID
 */
#define DNX_MIRROR_EGRESS_DISABLE_PROFILE_IDX(unit)  (dnx_data_snif.egress.feature_get(unit, dnx_data_snif_egress_egress_null_profile) ? 7 : 0)

/**
 * \brief - egress sniff nof profiles
 * The amount of egress snoop vs mirror profiles is different on some devices
 */
#define DNX_MIRROR_EGRESS_NOF_PROFILES_GET(unit, is_snoop) (is_snoop ? \
                                                            dnx_data_snif.egress.snoop_nof_profiles_get(unit) : \
                                                            dnx_data_snif.egress.mirror_nof_profiles_get(unit))

/*
 * \brief - egress to ingress profile index mapping is not 1:1. therefore this macro should be used
 * to convert bwtween the profile created and the value which needs to be configured at egress tables
 */
#define DNX_MIRROR_EGRESS_PROFILE_GET(unit, profile_id) ((profile_id == DNX_MIRROR_EGRESS_DISABLE_PROFILE_IDX(unit)) ? 0 : \
            ((profile_id == 0) ? DNX_MIRROR_EGRESS_DISABLE_PROFILE_IDX(unit) : profile_id))

/**
 * \brief - mirror on drop invalid profile ID
 */
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
 * \brief - Printing callback for the egress probability profiles template manager.
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
void dnx_mirror_egr_probability_profiles_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Printing callback for the egress crop size profiles template manager.
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
void dnx_mirror_egr_crop_size_profiles_print_cb(
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
 * \brief - Return the amount of supported admit profiles for mirror on drop profile configurations
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] nof_admit_profiles - The amount of supported admit profiles
 * \return
 * \remark
 *   * In case device doesn't support admit profiles, return 1 so iterations will iterate once
 * \see
 *   * None
 */
shr_error_e dnx_mirror_on_drop_nof_admit_profiles_get(
    int unit,
    int *nof_admit_profiles);
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
