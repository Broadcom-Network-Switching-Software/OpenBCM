/** \file algo_oamp_v1.c
 * 
 *
 * Resource and templates needed for the oam feature Ver1.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR

#include <bcm/types.h>

#ifdef BCM_DNX2_SUPPORT
/* { */

/*
* Include files.
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp_v1.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>

/*
 * }
 */

/*
 * See prototype definition for function description in algo_oam.h
 */
void
dnx_oam_oamp_v1_da_mac_lsb_profile_print_cb(
    int unit,
    const void *data)
{
    bcm_mac_t *da_mac_lsb = (bcm_mac_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_MAC, "Destination MAC MSB", *da_mac_lsb, NULL, NULL);

    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

/*
 * See prototype definition for function description in algo_oam.h
 */
void
dnx_oam_oamp_v1_da_mac_msb_profile_print_cb(
    int unit,
    const void *data)
{
    bcm_mac_t *da_mac_msb = (bcm_mac_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_MAC, "Destination MAC MSB", *da_mac_msb, NULL, NULL);

    SW_STATE_PRETTY_PRINT_FINISH();

    return;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_algo_oamp_v1_mep_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_oam_mep_profile_t *profile = (dnx_oam_mep_profile_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "piggy_back_lm", profile->piggy_back_lm, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "slm_lm", profile->slm_lm, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "slm_measurement_period",
                                   profile->slm_measurement_period_index, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "maid_check_dis", profile->maid_check_dis, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "report_mode_lm", profile->report_mode_lm, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "report_mode_dm", profile->report_mode_dm, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "rdi_gen_method", profile->rdi_gen_method, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "dmm_rate", profile->dmm_rate, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "lmm_rate", profile->lmm_rate, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "opcode_0_rate", profile->opcode_0_rate, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "opcode_1_rate", profile->opcode_1_rate, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "dmm_offset", profile->dmm_offset, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "dmr_offset", profile->dmr_offset, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "lmm_offset", profile->lmm_offset, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "lmr_offset", profile->lmr_offset, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "ccm_count", profile->ccm_count, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "dmm_count", profile->dmm_count, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "lmm_count", profile->lmm_count, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "opcode_0_count", profile->opcode_0_count, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "opcode_1_count", profile->opcode_1_count, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "dm_measurement_type",
                                   profile->dm_measurement_type, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "mep_id_shift", profile->mep_id_shift, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "opcode_mask", profile->opcode_mask, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "opcode_tx_statistics_enable",
                                   profile->opcode_tx_statistics_enable, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "opcode_rx_statistics_enable",
                                   profile->opcode_rx_statistics_enable, NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * See prototype definition for function description in algo_oamp.h
 */
void
dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_oam_ttl_exp_profile_t *profile = (dnx_oam_ttl_exp_profile_t *) data;
    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "ttl", profile->ttl, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT8, "exp", profile->exp, NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/*
 * DEFINEs
 * {
 */
#define NOF_MEP_PROFILES              128
#define NOF_MPLS_PWE_TTL_EXP_PROFILES 16
/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * Global and Static
 * {
 */

/*
 * }
 */

/**
 * \brief - This function creates the template for the OAM/BFD
 *        MPLS/PWE TTL/EXP profiles.  This resource is used by OAM
 *        and BFD MPLS/PWE accelerated endpoints to construct
 *        transmitted packets.
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
static shr_error_e
dnx_algo_oamp_v1_mpls_pwe_ttl_exp_profile_id_init(
    int unit)
{
    dnx_algo_template_create_data_t mpls_pwe_exp_ttl_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&mpls_pwe_exp_ttl_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    mpls_pwe_exp_ttl_profile_data.flags = 0;
    mpls_pwe_exp_ttl_profile_data.first_profile = 0;
    mpls_pwe_exp_ttl_profile_data.nof_profiles = NOF_MPLS_PWE_TTL_EXP_PROFILES;
    mpls_pwe_exp_ttl_profile_data.max_references = dnx_data_oam.general.oam_nof_oamp_meps_get(unit);
    mpls_pwe_exp_ttl_profile_data.default_profile = 0;
    mpls_pwe_exp_ttl_profile_data.data_size = sizeof(dnx_oam_ttl_exp_profile_t);
    mpls_pwe_exp_ttl_profile_data.default_data = NULL;
    sal_strncpy(mpls_pwe_exp_ttl_profile_data.name, DNX_OAM_TEMPLATE_MPLS_PWE_EXP_TTL_PROFILE,
                DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.create(unit, &mpls_pwe_exp_ttl_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the template needed for da mac msb profiles
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oamp_v1_da_mac_msb_template_init(
    int unit)
{
    dnx_algo_template_create_data_t oam_da_mac_msb_profile_data;
    uint32 max_nof_endpoint_id;
    uint32 nof_da_mac_msb_profiles;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    nof_da_mac_msb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_msb_profiles_get(unit);
    max_nof_endpoint_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);

    /** Create template for DA MAC msb */
    sal_memset(&oam_da_mac_msb_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    oam_da_mac_msb_profile_data.flags = 0;
    oam_da_mac_msb_profile_data.first_profile = 0;
    oam_da_mac_msb_profile_data.nof_profiles = nof_da_mac_msb_profiles;
    oam_da_mac_msb_profile_data.max_references = max_nof_endpoint_id;
    oam_da_mac_msb_profile_data.default_profile = 0;
    oam_da_mac_msb_profile_data.data_size = sizeof(bcm_mac_t);
    oam_da_mac_msb_profile_data.default_data = NULL;
    sal_strncpy(oam_da_mac_msb_profile_data.name, DNX_ALGO_OAM_OAMP_DA_MAC_MSB_PROFILE_TEMPLATE,
                DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.create(unit, &oam_da_mac_msb_profile_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the template needed for da mac lsb profiles
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_oamp_v1_da_mac_lsb_template_init(
    int unit)
{
    dnx_algo_template_create_data_t oam_da_mac_lsb_profile_data;
    uint32 max_nof_endpoint_id;
    uint32 nof_da_mac_lsb_profiles;
    SHR_FUNC_INIT_VARS(unit);

    /** Get values from DNX data   */
    nof_da_mac_lsb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_lsb_profiles_get(unit);
    max_nof_endpoint_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);

    /** Create template for DA MAC lsb */
    sal_memset(&oam_da_mac_lsb_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    oam_da_mac_lsb_profile_data.flags = 0;
    oam_da_mac_lsb_profile_data.first_profile = 0;
    oam_da_mac_lsb_profile_data.nof_profiles = nof_da_mac_lsb_profiles;
    oam_da_mac_lsb_profile_data.max_references = max_nof_endpoint_id;
    oam_da_mac_lsb_profile_data.default_profile = 0;
    oam_da_mac_lsb_profile_data.data_size = sizeof(bcm_mac_t);
    oam_da_mac_lsb_profile_data.default_data = NULL;
    sal_strncpy(oam_da_mac_lsb_profile_data.name, DNX_ALGO_OAM_OAMP_DA_MAC_LSB_PROFILE_TEMPLATE,
                DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.create(unit, &oam_da_mac_lsb_profile_data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the template needed for the
 *          OAMP MEP profile management
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
static shr_error_e
dnx_algo_oamp_v1_mep_profile_id_init(
    int unit)
{
    dnx_algo_template_create_data_t mep_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&mep_profile_data, 0, sizeof(dnx_algo_template_create_data_t));
    mep_profile_data.flags = 0;
    mep_profile_data.first_profile = 0;
    mep_profile_data.nof_profiles = NOF_MEP_PROFILES;
    mep_profile_data.max_references = dnx_data_oam.general.oam_nof_oamp_meps_get(unit);
    mep_profile_data.default_profile = 0;
    mep_profile_data.data_size = sizeof(dnx_oam_mep_profile_t);
    mep_profile_data.default_data = NULL;
    sal_strncpy(mep_profile_data.name, DNX_OAM_TEMPLATE_MEP_PROFILE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.create(unit, &mep_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates the resource configuration
 *        needed  OAM default MEP profiles.
 *        Used for creating OAM and BFD default MEPs.
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
static shr_error_e
dnx_algo_oamp_v1_default_mep_profile_init(
    int unit)
{
    dnx_algo_res_create_data_t oam_default_mep_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&oam_default_mep_profile_data, 0, sizeof(dnx_algo_res_create_data_t));
    oam_default_mep_profile_data.first_element = 0;
    oam_default_mep_profile_data.nof_elements = 4;
    oam_default_mep_profile_data.flags = 0;
    sal_strncpy(oam_default_mep_profile_data.name, DNX_ALGO_OAM_DEFAULT_MEP_PROFILE,
                DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_oam_db.oam_default_mep_profiles.create(unit, &oam_default_mep_profile_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_algo_oamp_v1_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_oamp_v1_da_mac_msb_template_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oamp_v1_da_mac_lsb_template_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oamp_v1_default_mep_profile_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oamp_v1_mpls_pwe_ttl_exp_profile_id_init(unit));
    SHR_IF_ERR_EXIT(dnx_algo_oamp_v1_mep_profile_id_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
#endif
