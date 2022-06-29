/** \file l3_wcmp.c
 *
 *  L3 WCMP procedures for DNX
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

/*
 * Include files
 * {
 */
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
/*
 * }
 */

/**
 * \brief
 *  Verifies the user input for the WCMP create function.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_info - The WCMP group information.
 * \param [in] ecmp_member_count - The number of members in the WCMP group.
 * \param [in] ecmp_member_array - An array of the WCMP group members.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_ecmp_create
 */
static shr_error_e
dnx_l3_wcmp_group_create_verify(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t * ecmp_member_array)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_wcmp_support))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "WCMP group create is not supported on this device.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Creates WCMP forwarding object
 * \param [in] unit - The unit number.
 * \param [in] options - Not relevant, should be set to 0.
 * \param [in,out] ecmp_info - The WCMP group information.
 *   The following fields are relevant of the bcm_l3_egress_ecmp_t structure:
 *  - flags            -> BCM_L3_* flags (supported flags are BCM_L3_REPLACE and BCM_L3_WITH_ID).
 *  - ecmp_intf        -> If BCM_L3_WITH_ID flag is set, this field will hold the WCMP ID value.
                          In case it is omitted, this field will return the allocated WCMP ID value.
 *  - rpf_mode         -> Indicates the URPF mode for the WMCP group - loose, strict or disabled.
 * \param [in] ecmp_member_count - The number of members in the WCMP group.
 * \param [in] ecmp_member_array - An array of the WCMP group members.
    The following fields are relevant for the bcm_l3_ecmp_member_t structure:
    - egress_if - WCMP member interface ID.
    - weight - WCMP member weight.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
bcm_dnx_l3_ecmp_create(
    int unit,
    uint32 options,
    bcm_l3_egress_ecmp_t * ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t * ecmp_member_array)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_l3_egress_ecmp_t), ecmp_info);

    /*
     * General WCMP group create verification
     */
    DNX_SWITCH_VERIFY_IF_ENABLE_DNX(unit,
                                    dnx_l3_wcmp_group_create_verify(unit, ecmp_info, ecmp_member_count,
                                                                    ecmp_member_array), bcmModuleL3EgressEcmp);

exit:
    DNX_ERR_RECOVERY_END_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function verifies that the WCMP group is allocated and in range.
 *   If it isn't, an error is returned.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_group_id -  The WCMP group ID.
 * \return
 *   \retval Zero if no error was detected and the group is allocated
 *   \retval _SHR_E_NOT_FOUND if the group is not allocated
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_l3_ecmp_destroy
 */
static shr_error_e
dnx_l3_wcmp_group_destroy_verify(
    int unit,
    bcm_if_t ecmp_group_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_wcmp_support))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "WCMP group destroy is not supported on this device.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Destroys WCMP group.
 * \param [in] unit - The unit number.
 * \param [in] ecmp_group_id - The WCMP group ID.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
bcm_dnx_l3_ecmp_destroy(
    int unit,
    bcm_if_t ecmp_group_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START_MODULE_ID(unit, bcmModuleL3EgressEcmp);

    DNX_SWITCH_VERIFY_IF_ENABLE_DNX(unit, dnx_l3_wcmp_group_destroy_verify(unit, ecmp_group_id), bcmModuleL3EgressEcmp);

exit:
    DNX_ERR_RECOVERY_END_MODULE_ID(unit, bcmModuleL3EgressEcmp);
    SHR_FUNC_EXIT;
}
