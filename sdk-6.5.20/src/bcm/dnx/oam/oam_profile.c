/** \file oam_profile.c
 * 
 *
 * This file contains all OAM profile related APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx_dispatch.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * OAM count enable / disable readable values
 */
#define DNX_OAM_COUNT_ENABLE 0
#define DNX_OAM_COUNT_DISABLE 1

/*
 * OAM SECTION Indication
 */
#define     OAM_NORMAL DBAL_ENUM_FVAL_IS_OAM_SECTION_OAM_NORMAL
#define     OAM_SECTION DBAL_ENUM_FVAL_IS_OAM_SECTION_OAM_SECTION

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * Get allocation manager resource name according to OAM
 * profile type
 */
#define DNX_OAM_RESOURCE_NAME_PER_PROFILE_TYPE(profile_type) (                                          \
            profile_type == bcmOAMProfileIngressLIF ? DNX_ALGO_OAM_PROFILE_ID_ING :                     \
            profile_type == bcmOAMProfileEgressLIF ? DNX_ALGO_OAM_PROFILE_ID_EG :                       \
            profile_type == bcmOAMProfileIngressAcceleratedEndpoint ? DNX_ALGO_OAM_PROFILE_ID_ING_ACC : \
            DNX_ALGO_OAM_PROFILE_ID_EG_ACC)
/*
 * Get the sw state resource callback.
 */
#define DNX_OAM_RESOURCE_CB_PER_PROFILE_TYPE(profile_type, exec) (                                          \
            profile_type == bcmOAMProfileIngressLIF ? SW_STATE_ALGO_OAM_PROFILE_ID_ING.exec :                     \
            profile_type == bcmOAMProfileEgressLIF ? SW_STATE_ALGO_OAM_PROFILE_ID_EG.exec :                       \
            profile_type == bcmOAMProfileIngressAcceleratedEndpoint ? SW_STATE_ALGO_OAM_PROFILE_ID_ING_ACC.exec : \
            SW_STATE_ALGO_OAM_PROFILE_ID_EG_ACC.exec)

/*
 * Get internal OAM mp_type based on endpoint type (set by user).
 * Assuming action for high/middle/low active and passive matches are the same accordingly.
 */
#define DNX_OAM_MP_TYPE_PER_ENDPOINT_TYPE(ep_type) (                                        \
            ep_type == bcmOAMMatchTypeMIP ? DBAL_ENUM_FVAL_MP_TYPE_MIP_MATCH :              \
            ep_type == bcmOAMMatchTypeMEP ? DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_HIGH :      \
            ep_type == bcmOAMMatchTypePassive ? DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH : \
            DBAL_ENUM_FVAL_MP_TYPE_BELOW_LOWER_MEP)
#define DNX_OAM_ENDPOINT_TYPE_PER_MP_TYPE(mp_type) (                                                \
            (mp_type == DBAL_ENUM_FVAL_MP_TYPE_BELOW_LOWER_MEP ||                                   \
             mp_type == DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIDDLE_AND_LOW ||                            \
             mp_type == DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_HIGH_AND_MIDDLE ||                           \
             mp_type == DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIP_AND_MEP) ? bcmOAMMatchTypeLowMDL :       \
            (mp_type == DBAL_ENUM_FVAL_MP_TYPE_MIP_MATCH) ? bcmOAMMatchTypeMIP :                    \
            (mp_type == DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_LOW ||                                  \
             mp_type == DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_MIDDLE ||                               \
             mp_type == DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_HIGH ||                                 \
             mp_type == DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_NO_COUNTER) ? bcmOAMMatchTypeMEP :      \
            (mp_type == DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_LOW ||                                 \
             mp_type == DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_MIDDLE ||                              \
             mp_type == DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH ||                                \
             mp_type == DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_NO_COUNTER) ? bcmOAMMatchTypePassive : \
            bcmOAMMatchTypeCount)

#define DNX_OAM_ENDPOINT_PASSIVE_TYPE_OF(mp_type)(                                                        \
 mp_type == DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_LOW ? DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_LOW :          \
 mp_type == DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_MIDDLE ? DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_MIDDLE :    \
 mp_type == DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_HIGH ? DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH :        \
 mp_type                                                                                                  \
)

#define DNX_OAM_MP_TYPE_IS_RFC_6374(flag) ((flag & BCM_OAM_ACTION_KEY_MPLS_LM_DM_OPCODE )==BCM_OAM_ACTION_KEY_MPLS_LM_DM_OPCODE)

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
 * \brief - Updates MP Profile on OAM LIF SW State.
 *   If entry doesn't exist - add it. If it is - update it.
 *   Indicate if MEPs exist on the LIFs for later HW changes.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_lif - The lif to update the profile for.
 * \param [in] mp_profile - The profile id to be set.
 * \param [in] is_ingress - Ing/Eg indication (ingress=1,
 *        egress=0)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   bcm_dnx_oam_lif_profile_set
 */
static shr_error_e
dnx_oam_lif_profile_sw_update(
    int unit,
    int oam_lif,
    bcm_oam_profile_t mp_profile,
    uint8 is_ingress)
{
    dnx_oam_lif_info_t oam_lif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Write profile to SW_STATE ing/eg */
    if (oam_lif != DNX_OAM_INVALID_LIF && mp_profile != BCM_OAM_PROFILE_INVALID)
    {
        /** Get current settings */
        SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_oam_sw_state_oam_lif_info_get(unit, oam_lif, is_ingress, &oam_lif_info),
                                  _SHR_E_NOT_FOUND);

        /** Add / Update LIF OAM profile on SW_STATE */
        oam_lif_info.mp_profile = mp_profile;

        SHR_IF_ERR_EXIT(dnx_oam_sw_state_oam_lif_info_set(unit, DBAL_COMMIT_FORCE, oam_lif, is_ingress, &oam_lif_info));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Check if profile id is allocated on resource manager
 *        based on profile type.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile_type - Profile type is a combination
 *        of ingress / egress and non-acc / acc which provides 4
 *        different sets of profiles. Following types are
 *        available:
 *          bcmOAMProfileIngressLIF
 *          bcmOAMProfileEgressLIF
 *          bcmOAMProfileIngressAcceleratedEndpoint
 *          bcmOAMProfileEgressAcceleratedEndpoint
 * \param [in] profile_index - Profile index decoded from
 *        pofile_id
 * \param [out] is_allocated - Indicates if the provided profile
 *        is allocated
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_oam_profile_create_verify
 *   dnx_oam_lif_profile_set_verify
 */
static shr_error_e
dnx_oam_profile_is_allocated(
    int unit,
    bcm_oam_profile_type_t profile_type,
    int profile_index,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_allocated = 0;

    SHR_IF_ERR_EXIT(DNX_OAM_RESOURCE_CB_PER_PROFILE_TYPE
                    (profile_type, is_allocated(unit, profile_index, is_allocated)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Validity checks for bcm_dnx_oam_lif_profile_get
 */
static shr_error_e
dnx_oam_lif_profile_get_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_oam_profile_t * ingress_oam_action_profile,
    bcm_oam_profile_t * egress_oam_action_profile)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Validate pointers are provided */
    SHR_NULL_CHECK(ingress_oam_action_profile, _SHR_E_PARAM, "ingress oam action profile");
    SHR_NULL_CHECK(egress_oam_action_profile, _SHR_E_PARAM, "egress oam action profile");

    /** Validate flags (currently nothing is supported) */
    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags combination is not supported\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Validity checks for bcm_dnx_oam_lif_profile_set
 */
static shr_error_e
dnx_oam_lif_profile_set_verify(
    int unit,
    uint32 flags,
    int inlif,
    int outlif,
    const bcm_oam_profile_t ingress_oam_action_profile,
    const bcm_oam_profile_t egress_oam_action_profile)
{
    bcm_oam_profile_type_t profile_type;
    int profile_index;
    uint8 is_profile_allocated;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate flags (currently nothing is supported) */
    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags combination is not supported\r\n");
    }

    if (inlif == BCM_GPORT_INVALID && ingress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress profile set (%d) but inlif is not valid\r\n", ingress_oam_action_profile);
    }
    if (outlif == BCM_GPORT_INVALID && egress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress profile set (%d) but outlif is not valid\r\n", egress_oam_action_profile);
    }

    if (ingress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
    {
        if (!DNX_OAM_PROFILE_VALID_GET(ingress_oam_action_profile))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: OAM profile id %d is not valid\n", ingress_oam_action_profile);
        }
        else
        {
            /** Validate profile direction matches */
            profile_type = DNX_OAM_PROFILE_TYPE_GET(ingress_oam_action_profile);
            if (profile_type != bcmOAMProfileIngressLIF)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress OAM LIF profile must be of type bcmOAMProfileIngressLIF\r\n");
            }

            /** Validate profile is allocated */
            profile_index = DNX_OAM_PROFILE_IDX_GET(ingress_oam_action_profile);

            SHR_IF_ERR_EXIT(dnx_oam_profile_is_allocated(unit, profile_type, profile_index, &is_profile_allocated));

            if (!is_profile_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Ingress OAM profile (%d) not found\r\n", ingress_oam_action_profile);
            }
        }
    }
    if (egress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
    {
        if (!DNX_OAM_PROFILE_VALID_GET(egress_oam_action_profile))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: OAM profile id %d is not valid\n", egress_oam_action_profile);
        }
        else
        {
            /** Validate profile direction matches */
            profile_type = DNX_OAM_PROFILE_TYPE_GET(egress_oam_action_profile);
            if (profile_type != bcmOAMProfileEgressLIF)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Egress OAM LIF profile must be of type bcmOAMProfileEgressLIF\r\n");
            }

            /** Validate profile is allocated */
            profile_index = DNX_OAM_PROFILE_IDX_GET(egress_oam_action_profile);

            SHR_IF_ERR_EXIT(dnx_oam_profile_is_allocated(unit, profile_type, profile_index, &is_profile_allocated));

            if (!is_profile_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Egress OAM profile (%d) not found\r\n", egress_oam_action_profile);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validity checks for bcm_oam_profile_create
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - Flags specified by user.
 * \param [in] oam_profile_type - Profile type is a combination
 *        of ingress / egress and non-acc / acc which provides 4
 *        different sets of profiles.
 * \param [in] profile_index - Profile index decoded from
 *        pofile_id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   bcm_oam_profile_create
*/
static shr_error_e
dnx_oam_profile_create_verify(
    int unit,
    uint32 flags,
    bcm_oam_profile_type_t oam_profile_type,
    bcm_oam_profile_t * profile_index)
{
    uint32 legal_flags = 0;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate pointers are provided */
    SHR_NULL_CHECK(profile_index, _SHR_E_PARAM, "profile id");

    /*
     * Check for supported flags
     */
    legal_flags |=
        BCM_OAM_PROFILE_WITH_ID | BCM_OAM_PROFILE_LM_TYPE_DUAL_ENDED | BCM_OAM_PROFILE_DM_TYPE_NTP |
        BCM_OAM_PROFILE_LM_TYPE_NONE | BCM_OAM_PROFILE_TYPE_SECTION | BCM_OAM_PROFILE_HIERARCHICAL_LM_DISABLE;
    if (flags & ~legal_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: unsupported flag combination\n");
    }

    /*
     * Check for invalid flag combinations
     */
    if ((flags & BCM_OAM_PROFILE_LM_TYPE_DUAL_ENDED) && (flags & BCM_OAM_PROFILE_LM_TYPE_NONE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: unsupported flag combination of LM type 0x%x\n", flags);
    }

    /*
     * Validate profile type
     */
    if (oam_profile_type < 0 || oam_profile_type >= bcmOAMProfileCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Profile type is out of range (0 - %d)\n", bcmOAMProfileCount - 1);
    }

    /*
     * Validate profile is not already allocated (according to profile type)
     * if id is specified explicitly
     */
    if (flags & BCM_OAM_PROFILE_WITH_ID)
    {
        SHR_IF_ERR_EXIT(dnx_oam_profile_is_allocated(unit, oam_profile_type, *profile_index, &is_allocated));

        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "Error: OAM profile with id %d is already exist\n", *profile_index);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Create new profile by allocating new profile id. Different
 *  types are supported. Each type will have it's on pool as the
 *  HW resources are seperated.
 *  Following types are available:
 *      bcmOAMProfileIngressLIF
 *      bcmOAMProfileEgressLIF
 *      bcmOAMProfileIngressAcceleratedEndpoint
 *      bcmOAMProfileEgressAcceleratedEndpoint
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - Flags specified by user.
 *        BCM_OAM_PROFILE_WITH_ID can be used to explicitly
 *        specify the profile ID.
 * \param [in] oam_profile_type - Profile type is a combination
 *        of ingress / egress and non-acc / acc which provides 4
 *        different sets of profiles.
 * \param [in,out] profile_id - Profile identifier.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   bcm_oam_profile_create
*/
int
bcm_dnx_oam_profile_create(
    int unit,
    uint32 flags,
    bcm_oam_profile_type_t oam_profile_type,
    bcm_oam_profile_t * profile_id)
{
    int alloc_flags = 0;
    int profile_index;
    uint32 is_ingress;
    uint8 profile_is_section;
    uint8 is_acc;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_profile_t), profile_id);

    SHR_INVOKE_VERIFY_DNX(dnx_oam_profile_create_verify(unit, flags, oam_profile_type, profile_id));

    /*
     * The profile_id (if provided) reflects only the profile index which will later be encoded
     * with the profile type.
     */
    profile_index = *profile_id;

    /*
     * In case BCM_OAM_PROFILE_WITH_ID flag is specified, allocate profile with profile_id
     */
    if (flags & BCM_OAM_PROFILE_WITH_ID)
    {
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }
    profile_is_section = OAM_NORMAL;
    if (flags & BCM_OAM_PROFILE_TYPE_SECTION)
    {
        profile_is_section = OAM_SECTION;
    }

    /** Allocate the profile ID */

    SHR_IF_ERR_EXIT(DNX_OAM_RESOURCE_CB_PER_PROFILE_TYPE
                    (oam_profile_type, allocate_single(unit, alloc_flags, NULL, &profile_index)));

    /** Determine direction based on profile type */
    is_ingress = DNX_OAM_PROFILE_IS_INGRESS(oam_profile_type);

    is_acc = !(DNX_OAM_PROFILE_IS_LIF_DB_ASSOCIATED(oam_profile_type));

    /*
     * Single ended LM/Dual ended LM/SLM action set
     * Set PROFILE_EN_MAP table
     * No replace of LM_TYPE is allowed currently.
     * So simply set the value and store it in SW.
     */

    if (flags & BCM_OAM_PROFILE_LM_TYPE_DUAL_ENDED)
    {
        if (!is_acc)
        {
            /*
             * Dual ended LM case: Enable counting of data packets and store in oam_sw_db.
             */
            SHR_IF_ERR_EXIT(dnx_oam_enable_data_counting(unit, profile_index, is_ingress, DNX_OAM_ENABLE_COUNTING));
        }

        SHR_IF_ERR_EXIT(dnx_oam_sw_db_lm_mode_set
                        (unit, profile_index, is_ingress, is_acc, DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_DUAL_ENDED,
                         profile_is_section));
    }
    else if (flags & BCM_OAM_PROFILE_LM_TYPE_NONE)
    {
        if (!is_acc)
        {
            /*
             * NONE (No counter stamping) case: Disable counting of data packets and store in oam_sw_db.
             */
            SHR_IF_ERR_EXIT(dnx_oam_enable_data_counting(unit, profile_index, is_ingress, DNX_OAM_DISABLE_COUNTING));
        }

        SHR_IF_ERR_EXIT(dnx_oam_sw_db_lm_mode_set
                        (unit, profile_index, is_ingress, is_acc, DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_NONE,
                         profile_is_section));
    }
    else
    {
        if (!is_acc)
        {
            /*
             * Default (Single ended LM case) : Enable counting of data packets and store in oam_sw_db.
             */
            SHR_IF_ERR_EXIT(dnx_oam_enable_data_counting(unit, profile_index, is_ingress, DNX_OAM_ENABLE_COUNTING));
        }

        SHR_IF_ERR_EXIT(dnx_oam_sw_db_lm_mode_set
                        (unit, profile_index, is_ingress, is_acc, DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_LM,
                         profile_is_section));
    }

    if (flags & BCM_OAM_PROFILE_DM_TYPE_NTP)
    {
        /*
         * DM NTP time format: Store in oam_sw_db.
         */
        SHR_IF_ERR_EXIT(dnx_oam_sw_db_dm_mode_set
                        (unit, profile_index, is_ingress, is_acc, DBAL_ENUM_FVAL_DM_MODE_DM_MODE_IS_NTP,
                         profile_is_section));
    }
    else
    {
        /*
         * Default (DM 1588 time format): Store in oam_sw_db.
         */
        SHR_IF_ERR_EXIT(dnx_oam_sw_db_dm_mode_set
                        (unit, profile_index, is_ingress, is_acc, DBAL_ENUM_FVAL_DM_MODE_DM_MODE_IS_1588,
                         profile_is_section));
    }

    if (flags & BCM_OAM_PROFILE_HIERARCHICAL_LM_DISABLE)
    {
        /*
         * Disable HLM by MD-Level
         */
        SHR_IF_ERR_EXIT(dnx_oam_sw_db_hlm_mode_set(unit, profile_index, is_ingress, is_acc, 1));
    }
    else
    {
        /*
         * Enable HLM by MD-Level
         */
        SHR_IF_ERR_EXIT(dnx_oam_sw_db_hlm_mode_set(unit, profile_index, is_ingress, is_acc, 0));
    }

    /** Encode the profile index and type into profile_id */
    *profile_id = DNX_OAM_PROFILE_SET(profile_index, oam_profile_type);

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete profile by profile id. Different types are supported.
 *  Each type will have it's on pool as the HW resources are
 *  seperated. Following types are available:
 *      bcmOAMProfileIngressLIF
 *      bcmOAMProfileEgressLIF
 *      bcmOAMProfileIngressAcceleratedEndpoint
 *      bcmOAMProfileEgressAcceleratedEndpoint
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - Currently not used.
 * \param [in] profile_id - Profile identifier.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * All action changes (done using
 *     bcm_oam_profile_action_set) for the provided profile will
 *     be cleared.
 *
 * \see
 *   bcm_oam_profile_delete
*/
int
bcm_dnx_oam_profile_delete(
    int unit,
    uint32 flags,
    bcm_oam_profile_t profile_id)
{
    bcm_oam_profile_type_t profile_type;
    int profile_index;
    oam_action_key_t action_key;
    uint32 oam_action_del_flags = 0;
    int is_ingress;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (!DNX_OAM_PROFILE_VALID_GET(profile_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: OAM profile id %d is not valid\n", profile_id);
    }

    /** Resolve profile type and index */
    profile_type = DNX_OAM_PROFILE_TYPE_GET(profile_id);
    profile_index = DNX_OAM_PROFILE_IDX_GET(profile_id);

    /** Determine direction based on profile type */
    is_ingress = DNX_OAM_PROFILE_IS_INGRESS(profile_type);

    /** Set flags */
    oam_action_del_flags |= is_ingress ? DNX_OAM_CLASSIFIER_INGRESS : DNX_OAM_CLASSIFIER_EGRESS;
    oam_action_del_flags |= DNX_OAM_PROFILE_IS_LIF_DB_ASSOCIATED(profile_type) ? DNX_OAM_CLASSIFIER_LIF_ACTION :
        DNX_OAM_CLASSIFIER_ACC_MEP_ACTION;

    /** Clear all corresponding OAM action entries */
    action_key.mp_profile = profile_index;
    for (action_key.da_is_mc = 0; action_key.da_is_mc <= 1; action_key.da_is_mc++)
    {
        for (action_key.is_inject = 0; action_key.is_inject <= 1; action_key.is_inject++)
        {
            for (action_key.is_my_mac = 0; action_key.is_my_mac <= 1; action_key.is_my_mac++)
            {
                for (action_key.mp_type = 0; action_key.mp_type < DBAL_NOF_ENUM_MP_TYPE_VALUES; action_key.mp_type++)
                {
                    /*
                     * Skip entries with mp_type value that cannot be configured by the user
                     */
                    if (DNX_OAM_ENDPOINT_TYPE_PER_MP_TYPE(action_key.mp_type) != bcmOAMMatchTypeCount)
                    {
                        for (action_key.oam_internal_opcode = 0;
                             action_key.oam_internal_opcode < DBAL_NOF_ENUM_OAM_INTERNAL_OPCODE_VALUES;
                             action_key.oam_internal_opcode++)
                        {
                            SHR_IF_ERR_EXIT(dnx_oam_action_clear(unit, oam_action_del_flags, &action_key));
                        }
                    }
                }
            }
        }
    }

    /** Free profile from allocation manager according to type */
    SHR_IF_ERR_EXIT(DNX_OAM_RESOURCE_CB_PER_PROFILE_TYPE(profile_type, free_single(unit, profile_index)));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Validity checks for bcm_oam_profile_id_get_by_type
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile_id - profile id.
 * \param [in] oam_profile_type - Profile type is a combination
 *        of ingress / egress and non-acc / acc which provides 4
 *        different sets of profiles.
 * \param [in] enc_profile_index - Profile index decoded from
 *        profile_id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   dnx_oam_profile_id_get_by_type_verify
*/
static shr_error_e
dnx_oam_profile_id_get_by_type_verify(
    int unit,
    uint8 profile_id,
    bcm_oam_profile_type_t oam_profile_type,
    bcm_oam_profile_t * enc_profile_index)
{
    uint8 nof_max_prof_id_per_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate pointers are provided */
    SHR_NULL_CHECK(enc_profile_index, _SHR_E_PARAM, "profile id");

    /*
     * Validate profile type
     */
    if (oam_profile_type < 0 || oam_profile_type >= bcmOAMProfileCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Profile type is out of range (0 - %d)\n", bcmOAMProfileCount - 1);
    }

    if ((bcmOAMProfileIngressLIF == oam_profile_type) || (bcmOAMProfileEgressLIF == oam_profile_type))
    {
        nof_max_prof_id_per_type = dnx_data_oam.general.oam_nof_non_acc_action_profiles_get(unit) - 1;
    }
    else if ((bcmOAMProfileIngressAcceleratedEndpoint == oam_profile_type) ||
             (bcmOAMProfileEgressAcceleratedEndpoint == oam_profile_type))
    {
        nof_max_prof_id_per_type = dnx_data_oam.general.oam_nof_acc_action_profiles_get(unit) - 1;
    }

    /*
     * Validate profile id range (0-15)
     */
    if (profile_id > nof_max_prof_id_per_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Profile id is out of range (0 - %d)\n", nof_max_prof_id_per_type);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Function to return encoded profile id from raw profile id
 *  and profile type.
 *  Following types are available:
 *      bcmOAMProfileIngressLIF
 *      bcmOAMProfileEgressLIF
 *      bcmOAMProfileIngressAcceleratedEndpoint
 *      bcmOAMProfileEgressAcceleratedEndpoint
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile_id - Raw profile index having value (0-15).
 * \param [in] oam_profile_type - Type of the OAM profile to be encoded.
 * \param [out] flags - Flags specified by user during profile creation.
 * \param [out] enc_profile_id - Encoded profile index value.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   bcm_dnx_oam_profile_id_get_by_type
*/
int
bcm_dnx_oam_profile_id_get_by_type(
    int unit,
    uint8 profile_id,
    bcm_oam_profile_type_t oam_profile_type,
    uint32 *flags,
    bcm_oam_profile_t * enc_profile_id)
{
    uint8 is_ingress;
    uint8 is_acc;
    uint8 lm_type;
    uint8 dm_type;
    uint8 is_section;
    uint8 is_hlm_disable;
    uint32 profile_index;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_oam_profile_id_get_by_type_verify(unit, profile_id, oam_profile_type, enc_profile_id));

    /*
     * Encode the profile index and type into profile_id
     */
    *enc_profile_id = DNX_OAM_PROFILE_SET(profile_id, oam_profile_type);

    /** get lm-type */
    profile_index = DNX_OAM_PROFILE_IDX_GET(*enc_profile_id);
    /** Determine direction based on profile type */
    is_ingress = DNX_OAM_PROFILE_IS_INGRESS(oam_profile_type);

    is_acc = !(DNX_OAM_PROFILE_IS_LIF_DB_ASSOCIATED(oam_profile_type));

    SHR_IF_ERR_EXIT(dnx_oam_sw_db_lm_mode_get(unit, profile_index, is_ingress, is_acc, &lm_type, &is_section));

    *flags = 0;
    if (lm_type == DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_DUAL_ENDED)
    {
        *flags |= BCM_OAM_PROFILE_LM_TYPE_DUAL_ENDED;
    }
    else if (lm_type == DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_NONE)
    {
        *flags |= BCM_OAM_PROFILE_LM_TYPE_NONE;
    }
    if (is_section)
    {
        *flags |= BCM_OAM_PROFILE_TYPE_SECTION;
    }

    SHR_IF_ERR_EXIT(dnx_oam_sw_db_dm_mode_get(unit, profile_index, is_ingress, is_acc, &dm_type, &is_section));

    if (dm_type == DBAL_ENUM_FVAL_DM_MODE_DM_MODE_IS_NTP)
    {
        *flags |= BCM_OAM_PROFILE_DM_TYPE_NTP;
    }

    SHR_IF_ERR_EXIT(dnx_oam_sw_db_hlm_mode_get(unit, profile_index, is_ingress, is_acc, &is_hlm_disable));

    if (is_hlm_disable)
    {
        *flags |= BCM_OAM_PROFILE_HIERARCHICAL_LM_DISABLE;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - This function returns OAM lif profile/s for a
 *        specific gport from the SW State. gport may contain
 *        global_in_lif, local_out_lif or both.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - Currently not used.
 * \param [in] gport - The gport to search by.
 * \param [out] ingress_oam_action_profile - The inlif profile
 *        to be returned.
 * \param [out] egress_oam_action_profile - The outlif profile
 *        to be returned.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * If entry does'nt exist in SW state, no error will be
 *     raised. profiles will be set to BCM_OAM_PROFILE_INVALID.
 *     Following memories are related:
 *     DBAL_TABLE_INGRESS_OAM_LIF_DB,
 *     DBAL_TABLE_EGRESS_OAM_LIF_DB, DBAL_TABLE_OAM_LIF_INFO_SW
 *
 * \see
 *   bcm_oam_lif_profile_get (oam.h)
 */
int
bcm_dnx_oam_lif_profile_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_oam_profile_t * ingress_oam_action_profile,
    bcm_oam_profile_t * egress_oam_action_profile)
{
    int is_ingress;
    int rv;
    dnx_oam_lif_info_t oam_lif_info;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 gpm_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Validity checks */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_lif_profile_get_verify(unit, flags, gport, ingress_oam_action_profile,
                                                         egress_oam_action_profile));

    /** Get Gport information */
    gpm_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, gpm_flags, &gport_hw_resources));

    *ingress_oam_action_profile = BCM_OAM_PROFILE_INVALID;
    *egress_oam_action_profile = BCM_OAM_PROFILE_INVALID;

    /** Get profile from SW_STATE ing/eg */
    if (gport_hw_resources.global_in_lif != BCM_GPORT_INVALID)
    {
        is_ingress = 1;
        rv = dnx_oam_sw_state_oam_lif_info_get(unit, gport_hw_resources.global_in_lif, is_ingress, &oam_lif_info);
        /** Entry might never been set */
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

        /** In case the entry is set, return the correct profile ID*/
        if (rv != _SHR_E_NOT_FOUND)
        {
            *ingress_oam_action_profile = DNX_OAM_PROFILE_SET(oam_lif_info.mp_profile, bcmOAMProfileIngressLIF);
        }
    }
    if (gport_hw_resources.local_out_lif != BCM_GPORT_INVALID)
    {
        is_ingress = 0;
        rv = dnx_oam_sw_state_oam_lif_info_get(unit, gport_hw_resources.local_out_lif, is_ingress, &oam_lif_info);
        /** Entry might never been set */
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

        /** In case the entry is set, return the correct profile ID*/
        if (rv != _SHR_E_NOT_FOUND)
        {
            *egress_oam_action_profile = DNX_OAM_PROFILE_SET(oam_lif_info.mp_profile, bcmOAMProfileEgressLIF);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function sets mp_profile for a default mep
 *        profile. If profile should be updated in one
 *        side only, set the other side profile to
 *        BCM_OAM_PROFILE_INVALID.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - should indicate the default MEP profile
 *        id: BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_0/1/2/3
 * \param [in] ingress_oam_action_profile - The new profile to
 *        be set for the ingress oam-lif (global inlif
 *        reterieved from gport)
 * \param [in] egress_oam_action_profile - The new profile to be
 *        set for the egress oam-lif (local outlif reterieved
 *        from gport)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 */
static shr_error_e
dnx_oam_default_mep_action_profile_set(
    int unit,
    uint32 flags,
    bcm_oam_profile_t ingress_oam_action_profile,
    bcm_oam_profile_t egress_oam_action_profile)
{
    bcm_oam_endpoint_info_t endpoint_info;
    oam_default_mep_info_t default_mep_info;
    uint8 oam_default_profile;
    uint8 mp_profile;

    SHR_FUNC_INIT_VARS(unit);

    oam_default_profile = BCM_OAM_PROFILE_INVALID;
    bcm_oam_endpoint_info_t_init(&endpoint_info);

    if (ingress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
    {
        switch (flags)
        {
            case BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_0:
                endpoint_info.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS0;
                oam_default_profile = DNX_OAM_DEFAULT_PROFILE_0;
                break;
            case BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_1:
                endpoint_info.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS1;
                oam_default_profile = DNX_OAM_DEFAULT_PROFILE_1;
                break;
            case BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_2:
                endpoint_info.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS2;
                oam_default_profile = DNX_OAM_DEFAULT_PROFILE_2;
                break;
            case BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_3:
                endpoint_info.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS3;
                oam_default_profile = DNX_OAM_DEFAULT_PROFILE_3;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Invalid default MEP.\r\n");
        }
        SHR_IF_ERR_EXIT(dnx_oam_default_mep_profile_get(unit, &endpoint_info, &default_mep_info));

        mp_profile = DNX_OAM_PROFILE_IDX_GET(ingress_oam_action_profile);

        SHR_IF_ERR_EXIT(dnx_oam_default_endpoint_update
                        (unit, DNX_OAM_CLASSIFIER_INGRESS, oam_default_profile, mp_profile,
                         default_mep_info.mdl_mp_type));
    }

    if (egress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress Default Profile is not supported\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function set OAM lif profile for a specific
 *        gport on the SW State. gport may contain
 *        global_in_lif, local_out_lif or both. If the specified
 *        LIF/s have MEPs configured, The relevant OAM-LIF-DB
 *        entries will be updated as well. If profile should be
 *        updated in one side only, set the other side profile
 *        to BCM_OAM_PROFILE_INVALID.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - Currently not used.
 * \param [in] gport - The gport to be updated.
 * \param [in] ingress_oam_action_profile - The new profile to
 *        be set for the ingress oam-lif (global inlif
 *        reterieved from gport)
 * \param [in] egress_oam_action_profile - The new profile to be
 *        set for the egress oam-lif (local outlif reterieved
 *        from gport)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Following memories may be changed:
 *     DBAL_TABLE_INGRESS_OAM_LIF_DB,
 *     DBAL_TABLE_EGRESS_OAM_LIF_DB,
 *     DBAL_TABLE_OAM_LIF_INFO_SW
 *
 * \see
 *   bcm_oam_lif_profile_set (oam.h)
 */
int
bcm_dnx_oam_lif_profile_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_oam_profile_t ingress_oam_action_profile,
    bcm_oam_profile_t egress_oam_action_profile)
{
    uint8 is_ingress;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    dnx_oam_lif_entry_key_t oam_lif_db_key;
    oam_lif_resolution_t lif_resolution;
    oam_lif_configured_lif_t configured_lif;
    oam_lif_analysis_results_t lif_analysis;
    int prefix;
    int profile_index;
    uint32 algo_gpm_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Setting mp_profile for default mep profile
     */
    if (((flags & BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_0) == BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_0) ||
        ((flags & BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_1) == BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_1) ||
        ((flags & BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_2) == BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_2) ||
        ((flags & BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_3) == BCM_OAM_LIF_PROFILE_FLAGS_DEFAULT_MEP_3))
    {
        SHR_IF_ERR_EXIT(dnx_oam_default_mep_action_profile_set
                        (unit, flags, ingress_oam_action_profile, egress_oam_action_profile));
    }
    else
    {
        algo_gpm_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
       /** Validate profile */
        if (ingress_oam_action_profile == BCM_OAM_PROFILE_INVALID
            && egress_oam_action_profile == BCM_OAM_PROFILE_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Both profiles are invalid\r\n");
        }
        if (ingress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
        {
            algo_gpm_flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
        }
        if (egress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
        {
            algo_gpm_flags |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        }
       /** Get Gport information */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, algo_gpm_flags, &gport_hw_resources));

       /** Validity checks */
        SHR_INVOKE_VERIFY_DNX(dnx_oam_lif_profile_set_verify(unit, flags, gport_hw_resources.global_in_lif,
                                                             gport_hw_resources.local_out_lif,
                                                             ingress_oam_action_profile, egress_oam_action_profile));

       /** Write profile to SW_STATE ing/eg */
        if (ingress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
        {
            is_ingress = 1;
            SHR_IF_ERR_EXIT(dnx_oam_lif_profile_sw_update(unit, gport_hw_resources.global_in_lif,
                                                          DNX_OAM_PROFILE_IDX_GET(ingress_oam_action_profile),
                                                          is_ingress));
        }
        if (egress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
        {
            is_ingress = 0;
            SHR_IF_ERR_EXIT(dnx_oam_lif_profile_sw_update(unit, gport_hw_resources.local_out_lif,
                                                          DNX_OAM_PROFILE_IDX_GET(egress_oam_action_profile),
                                                          is_ingress));
        }

       /** HW related changes */

        /*
         * Determine number of OAM_LIF_DB entries associated with lif (0-3 for different prefixes),
         * for each existing entry, mp_profile field should be updated.
         */
        lif_resolution.global_in_lif = gport_hw_resources.global_in_lif;
        lif_resolution.local_out_lif = gport_hw_resources.local_out_lif;
        SHR_IF_ERR_EXIT(dnx_oam_lif_configured_lif_get(unit, &lif_resolution, &configured_lif));
        SHR_IF_ERR_EXIT(dnx_oam_analyze_configured_lif(unit, &configured_lif, &lif_analysis));

        oam_lif_db_key.core_id = _SHR_CORE_ALL;

        if (ingress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
        {

            for (prefix = PREFIX_HIGH; prefix < lif_analysis.lif_db.occupied_prefixes[DNX_OAM_CLASSIFIER_INGRESS];
                 prefix++)
            {
                oam_lif_db_key.oam_key_prefix = prefix;

             /** Ingress OAM LIF DB */
                oam_lif_db_key.flags = DNX_OAM_CLASSIFIER_INGRESS;
                oam_lif_db_key.oam_lif_id = gport_hw_resources.global_in_lif;
                profile_index = DNX_OAM_PROFILE_IDX_GET(ingress_oam_action_profile);

                SHR_IF_ERR_EXIT(dnx_oam_lif_modify_mp_profile(unit, &oam_lif_db_key, profile_index));
            }
        }

        if (egress_oam_action_profile != BCM_OAM_PROFILE_INVALID)
        {

            for (prefix = PREFIX_HIGH; prefix < lif_analysis.lif_db.occupied_prefixes[DNX_OAM_CLASSIFIER_EGRESS];
                 prefix++)
            {
                oam_lif_db_key.oam_key_prefix = prefix;

             /** Egress OAM LIF DB */
                oam_lif_db_key.flags = DNX_OAM_CLASSIFIER_EGRESS;
                oam_lif_db_key.oam_lif_id = gport_hw_resources.local_out_lif;
                profile_index = DNX_OAM_PROFILE_IDX_GET(egress_oam_action_profile);

                SHR_IF_ERR_EXIT(dnx_oam_lif_modify_mp_profile(unit, &oam_lif_db_key, profile_index));
            }
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function calculates the counter disable bits
 *        for OAM action tables based on the count_enable
 *        argument of bcm_oam_profile_action_set API provided by
 *        the user.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile_id - Profile identifier.
 * \param [in] mp_type - OAM mp type. Affects counter disable
 *        bits result.
 * \param [in] count_enable - argument of
 *        bcm_oam_profile_action_set API provided by the user.
 * \param [in] is_section - argument of lm/slm
 * \param [in] is_hlm_disable - argument of hierarchical LM enable(0)/disable(1)
 * \param [out] action_content - OAM action data entry contains
 *        all counter disable bits. results will be set on it.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * This function is the reversed function of
 *     dnx_oam_profile_counter_enable_calc
 *
 * \see
 *   bcm_oam_profile_action_set (oam.h)
 */
static shr_error_e
dnx_oam_profile_counter_disable_calc(
    int unit,
    bcm_oam_profile_t profile_id,
    dbal_enum_value_field_mp_type_e mp_type,
    uint8 count_enable,
    uint8 is_section,
    uint8 is_hlm_disable,
    oam_action_content_t * action_content)
{
    bcm_oam_profile_type_t profile_type;
    SHR_FUNC_INIT_VARS(unit);

    /** Resolve profile type and index */
    profile_type = DNX_OAM_PROFILE_TYPE_GET(profile_id);

    /** Initialize all to enable counting */
    action_content->oama_counter_disable_access_0 = DNX_OAM_COUNT_ENABLE;
    action_content->oama_counter_disable_access_1 = DNX_OAM_COUNT_ENABLE;
    action_content->oama_counter_disable_access_2 = DNX_OAM_COUNT_ENABLE;
    action_content->oamb_counter_disable = DNX_OAM_COUNT_ENABLE;

    if (DNX_OAM_PROFILE_IS_LIF_DB_ASSOCIATED(profile_type))
    {
        /** LIF action table (OAMA) */
        switch (mp_type)
        {
            case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_LOW:
                if (is_hlm_disable)
                {
                    action_content->oama_counter_disable_access_0 = !count_enable;
                    action_content->oama_counter_disable_access_1 = !count_enable;
                }
                else
                {
                    action_content->oama_counter_disable_access_0 = DNX_OAM_COUNT_DISABLE;
                    action_content->oama_counter_disable_access_1 = DNX_OAM_COUNT_DISABLE;
                }
                action_content->oama_counter_disable_access_2 = !count_enable;
                break;
            case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_MIDDLE:
                if (is_hlm_disable)
                {
                    action_content->oama_counter_disable_access_0 = !count_enable;
                    action_content->oama_counter_disable_access_2 = !count_enable;
                }
                else
                {
                    action_content->oama_counter_disable_access_0 = DNX_OAM_COUNT_DISABLE;
                }
                action_content->oama_counter_disable_access_1 = !count_enable;
                break;
            case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_HIGH:
                if (is_section || is_hlm_disable)
                {
                    action_content->oama_counter_disable_access_0 = !count_enable;
                    action_content->oama_counter_disable_access_1 = !count_enable;
                    action_content->oama_counter_disable_access_2 = !count_enable;
                }
                else
                {
                    action_content->oama_counter_disable_access_0 = !count_enable;
                }
                break;
            case DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_NO_COUNTER:
                action_content->oama_counter_disable_access_0 = DNX_OAM_COUNT_DISABLE;
                break;
            case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_LOW:
            case DBAL_ENUM_FVAL_MP_TYPE_BELOW_LOWER_MEP:
                action_content->oama_counter_disable_access_2 = DNX_OAM_COUNT_DISABLE;
                action_content->oama_counter_disable_access_1 = DNX_OAM_COUNT_DISABLE;
                action_content->oama_counter_disable_access_0 = DNX_OAM_COUNT_DISABLE;
                break;
            case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_MIDDLE:
                action_content->oama_counter_disable_access_1 = DNX_OAM_COUNT_DISABLE;
                action_content->oama_counter_disable_access_0 = DNX_OAM_COUNT_DISABLE;
                break;
            case DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIDDLE_AND_LOW:
                if (is_hlm_disable)
                {
                    action_content->oama_counter_disable_access_2 = DNX_OAM_COUNT_DISABLE;
                }
                action_content->oama_counter_disable_access_1 = DNX_OAM_COUNT_DISABLE;
                action_content->oama_counter_disable_access_0 = DNX_OAM_COUNT_DISABLE;
                break;
            case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH:
                action_content->oama_counter_disable_access_0 = !count_enable;
                break;
            case DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_NO_COUNTER:
                action_content->oama_counter_disable_access_0 = DNX_OAM_COUNT_DISABLE;
                break;
            case DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_HIGH_AND_MIDDLE:
                if (is_hlm_disable)
                {
                    action_content->oama_counter_disable_access_2 = DNX_OAM_COUNT_DISABLE;
                    action_content->oama_counter_disable_access_1 = DNX_OAM_COUNT_DISABLE;
                }
                action_content->oama_counter_disable_access_0 = DNX_OAM_COUNT_DISABLE;
                break;
            default:
                break;
        }
    }
    else
    {
        /** Accelerated action table (OAMB) */
        action_content->oamb_counter_disable = !count_enable;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function calculates the counter enable field
 *        for bcm_oam_profile_action_get based on OAM action
 *        table data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile_type - Ing/eg, acc/non-acc. Affects
 *        counter enable value.
 * \param [in] endpoint_type - Affects counter enable bits
 *        value.
 * \param [in] oama_active_high_count_disable - Counter disable
 *        value of ACTIVE_HIGH mp type from OAMA action table.
 * \param [in] oamb_count_disable - Counter disable
 *        value of OAMB action table.
 * \param [out] oam_action_result - OAM action result. Set only
 *        counter_enable field.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * This function is the reversed function of
 *     dnx_oam_profile_counter_disable_calc
 *
 * \see
 *   bcm_oam_profile_action_get (oam.h)
 */
static shr_error_e
dnx_oam_profile_counter_enable_calc(
    int unit,
    bcm_oam_profile_type_t profile_type,
    bcm_oam_match_type_t endpoint_type,
    uint32 oama_active_high_count_disable,
    uint32 oamb_count_disable,
    bcm_oam_action_result_t * oam_action_result)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_OAM_PROFILE_IS_LIF_DB_ASSOCIATED(profile_type))
    {
        /** LIF action table (OAMA) */
        oam_action_result->counter_increment = 0;

        if (endpoint_type == bcmOAMMatchTypeMEP)
        {
            oam_action_result->counter_increment = !oama_active_high_count_disable;
        }
    }
    else
    {
        /** Accelerated action table (OAMB) */
        oam_action_result->counter_increment = !oamb_count_disable;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function calculates the sub_type value to be
 *        set on OAM action table entry based on loss measurement type
 *        (single / double ended, synthetic) and the delay
 *        measurement time format (NTP / 1588) provided by the
 *        user.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] is_ingress - is_ingress - Ing/Eg indication (ingress=1,
 *        egress=0)
 * \param [in] profile_id - Profile identifier.
 * \param [in] internal_opcode - Translation of the opcode
 *        provided by user to be set in OAM action table.
 * \param [in] destination - classifier destination
 * \param [in] is_level_or_passive_trap - Indicates if level or
          passive trap is being configured.
 * \param [out] action_content - OAM action table entry data.
 *        This function only affects the oam_sub_type field.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 *
 * \see
 *   bcm_oam_profile_action_set (oam.h)
 */
static shr_error_e
dnx_oam_profile_sub_type_calc(
    int unit,
    uint32 is_ingress,
    bcm_oam_profile_t profile_id,
    dbal_enum_value_field_oam_internal_opcode_e internal_opcode,
    bcm_gport_t destination,
    uint8 *is_level_or_passive_trap,
    oam_action_content_t * action_content)
{
    uint8 is_acc;
    uint8 lm_type;
    uint8 dm_type;
    uint8 is_section;
    int profile_index;
    bcm_oam_profile_type_t profile_type;
    int trap_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * CCM is the default OAM sub type
     */
    action_content->oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_CCM;

    /** Resolve profile type and index */
    profile_type = DNX_OAM_PROFILE_TYPE_GET(profile_id);
    profile_index = DNX_OAM_PROFILE_IDX_GET(profile_id);

    /** Check if the given profile is accelerated */
    is_acc = !(DNX_OAM_PROFILE_IS_LIF_DB_ASSOCIATED(profile_type));

    /*
     * Resolve trap code from action's destination.
     * Will be set to -1 in case destination is not trap
     */
    trap_id = BCM_GPORT_TRAP_GET_ID(destination);

    if (internal_opcode == DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMM ||
        internal_opcode == DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMR ||
        internal_opcode == DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_1DM)
    {
        SHR_IF_ERR_EXIT(dnx_oam_sw_db_dm_mode_get(unit, profile_index, is_ingress, is_acc, &dm_type, &is_section));
        /** Opcode is relevant for delay measurement. */
        if (dm_type == DBAL_ENUM_FVAL_DM_MODE_DM_MODE_IS_NTP)
        {
            action_content->oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP;
        }
        else
        {
            action_content->oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588;
        }
    }
    if (internal_opcode == DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBM)
    {
        int reflector_trap_id;
        /** set subtype to loopback for upmep reflector*/
        SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamReflector, &reflector_trap_id));
        if (reflector_trap_id == trap_id)
        {
            action_content->oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOOPBACK;
        }
    }
    else if (DNX_OAM_OPCODE_IS_LM_RELATED(opcode))
    {
        /** Get profile's LM type */
        SHR_IF_ERR_EXIT(dnx_oam_sw_db_lm_mode_get(unit, profile_index, is_ingress, is_acc, &lm_type, &is_section));

        if (internal_opcode == DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_CCM)
        {
            /** CCM might also be relevant for LM (Piggy-back) */
            if (lm_type == DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_DUAL_ENDED)
            {
                action_content->oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT;
            }
            else
            {
                action_content->oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_CCM;
            }
        }
        else
        {
            /*
             * To prevent stamping of invalid counter, in case of LM_MODE_NONE
             * subtype CCM is used
             */
            if (lm_type == DBAL_ENUM_FVAL_LM_MODE_LM_MODE_IS_NONE)
            {
                action_content->oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_CCM;
            }
            else
            {
                action_content->oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT;
            }
        }
    }

    if (action_content->forwarding_strength > 0)
    {
        /** Set subtype for OAM level and passive trap */
        int oam_level_trap_id, oam_passive_trap_id;
        if (is_ingress)
        {
            /** At the ingress no processing required based on subtype.
                Using CCM (default OAM) subtype */
            SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, 0, bcmRxTrapOamLevel, &oam_level_trap_id));
            SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, 0, bcmRxTrapOamPassive, &oam_passive_trap_id));

            if (trap_id == oam_level_trap_id || trap_id == oam_passive_trap_id)
            {
                action_content->oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_CCM;
                *is_level_or_passive_trap = TRUE;
            }
        }
        else
        {
            /**At the egress the subtype is used as context selection for trap block and mirror profile.
               For OAM level/passive handling set the respective subtype.*/
            SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamLevel, &oam_level_trap_id));
            SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamPassive, &oam_passive_trap_id));

            if (trap_id == oam_level_trap_id)
            {
                action_content->oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_LEVEL_ERROR;
                *is_level_or_passive_trap = TRUE;
            }
            else if (trap_id == oam_passive_trap_id)
            {
                action_content->oam_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_PASSIVE_ERROR;
                *is_level_or_passive_trap = TRUE;
            }
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Translate rfc6374 opcode taken from mpls_lm_dm_opcode
 *          into internal opcode for profile action purposes
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mpls_lm_dm_opcode - mpls opcode
 * \param [out] internal_opcode - Internal opcode
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
dnx_rfc6374_opcode_map_get(
    int unit,
    bcm_oam_mpls_lm_dm_opcode_type_t mpls_lm_dm_opcode,
    dbal_enum_value_field_oam_internal_opcode_e * internal_opcode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (mpls_lm_dm_opcode)
    {
        case bcmOamMplsLmDmOpcodeTypeLm:
            *internal_opcode = DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMM;
            break;
        case bcmOamMplsLmDmOpcodeTypeDm:
            *internal_opcode = DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMM;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Unsupported mpls_lm_dm opcode 0x%x\n", mpls_lm_dm_opcode);

    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This utility function fills the key
 *        (internal_action_key) for OAM action table based on
 *        user provided information regarding the key
 *        (oam_action_key).
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile_id - The profile to change the action
 *        for. Direction (ing/eg) is based on profile type.
 * \param [in] oam_action_key - Combination of opcode,
 *        destination MAC address type (UC/MC/MyCFM) and MEP
 *        type (MEP/MIP/Passive/LowMDL) to change the action
 *        for. Provided by the user.
 * \param [out] internal_action_key - Actual OAM action table
 *        key to use with dnx_oam_action_set.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Several mp_type values can match one endpoint_type. Using
 *     _HIGH (see DNX_OAM_MP_TYPE_PER_ENDPOINT_TYPE)
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_profile_action_table_key_fill(
    int unit,
    bcm_oam_profile_t profile_id,
    bcm_oam_action_key_t * oam_action_key,
    oam_action_key_t * internal_action_key)
{
    bcm_oam_profile_type_t profile_type;
    int profile_index;
    uint32 is_ingress;

    SHR_FUNC_INIT_VARS(unit);

    /** Resolve profile type and index */
    profile_type = DNX_OAM_PROFILE_TYPE_GET(profile_id);
    profile_index = DNX_OAM_PROFILE_IDX_GET(profile_id);

    /** Determine direction based on profile type */
    is_ingress = DNX_OAM_PROFILE_IS_INGRESS(profile_type);

    if (DNX_OAM_MP_TYPE_IS_RFC_6374(oam_action_key->flags))
    {
        /** Translate mpls_lm_dm_opcode to internal-opcode */
        SHR_IF_ERR_EXIT(dnx_rfc6374_opcode_map_get
                        (unit, oam_action_key->mpls_lm_dm_opcode, &internal_action_key->oam_internal_opcode));
    }
    else
    {
      /** Translate opcode to internal-opcode */
        SHR_IF_ERR_EXIT(dnx_oam_opcode_map_get(unit, is_ingress, oam_action_key->opcode,
                                               &internal_action_key->oam_internal_opcode));
    }

    /** Fill key for OAM action table */
    internal_action_key->da_is_mc = (oam_action_key->dest_mac_type == bcmOAMDestMacTypeMcast);
    internal_action_key->is_inject = oam_action_key->inject;
    internal_action_key->is_my_mac = (oam_action_key->dest_mac_type == bcmOAMDestMacTypeMyCfmMac);
    internal_action_key->mp_profile = profile_index;
    if (DNX_OAM_MP_TYPE_IS_RFC_6374(oam_action_key->flags))
    {
        internal_action_key->mp_type = DBAL_ENUM_FVAL_MP_TYPE_BFD;
    }
    else
    {
        internal_action_key->mp_type = DNX_OAM_MP_TYPE_PER_ENDPOINT_TYPE(oam_action_key->endpoint_type);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validity checks for both bcm_oam_profile_action_get and
 *   bcm_oam_profile_action_set. Common parts only.
*/
static shr_error_e
dnx_oam_profile_action_common_verify(
    int unit,
    uint32 flags,
    bcm_oam_profile_t profile_id,
    bcm_oam_action_key_t * oam_action_key,
    bcm_oam_action_result_t * oam_action_result)
{
    bcm_oam_profile_type_t profile_type;
    int profile_index;
    uint8 is_profile_allocated;
    uint32 legal_flags = 0;
    int reserved_acc_egr_profile_id = dnx_data_oam.general.oam_egr_acc_action_profile_id_for_inject_mpls_get(unit);
    int is_reserved_acc_egr_profile = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate flags (currently nothing is supported) */
    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags combination is not supported\r\n");
    }

    /** Validate profile */
    if (profile_id == BCM_OAM_PROFILE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "OAM profile id %d is not valid\n", profile_id);
    }
    else
    {
        if (!DNX_OAM_PROFILE_VALID_GET(profile_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "OAM profile id %d is not valid\n", profile_id);
        }
        else
        {
            /** Validate profile is allocated */
            profile_type = DNX_OAM_PROFILE_TYPE_GET(profile_id);
            profile_index = DNX_OAM_PROFILE_IDX_GET(profile_id);
            is_reserved_acc_egr_profile = (profile_index == reserved_acc_egr_profile_id)
                && (profile_type == bcmOAMProfileEgressAcceleratedEndpoint);
            if (!is_reserved_acc_egr_profile)
            {
                SHR_IF_ERR_EXIT(dnx_oam_profile_is_allocated(unit, profile_type, profile_index, &is_profile_allocated));

                if (!is_profile_allocated)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "OAM profile (%d) not found\r\n", profile_id);
                }
            }
        }
    }

    /** Validate OAM action key fields */
    legal_flags |= BCM_OAM_ACTION_KEY_MPLS_LM_DM_OPCODE;
    if (oam_action_key->flags & ~legal_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "OAM action key flags combination is not supported\r\n");
    }

    if (oam_action_key->inject != 0 && oam_action_key->inject != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "OAM action key injection is out of range (0-1)\r\n");
    }
    if (oam_action_key->endpoint_type >= bcmOAMMatchTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "OAM action key endpoint type is out of range (0-%d)\r\n", bcmOAMMatchTypeCount - 1);
    }
    if (oam_action_key->dest_mac_type >= bcmOAMDestMacTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "OAM action key destination MAC type is out of range (0-%d)\r\n",
                     bcmOAMDestMacTypeCount - 1);
    }
    if (DNX_OAM_MP_TYPE_IS_RFC_6374(oam_action_key->flags))
    {
        if (oam_action_key->opcode != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "OAM opcode should be invalid for RFX-6374 profile. Please use mpls_lm_dm_opcode field instead.\n");
        }
    }
    else
    {
        if (oam_action_key->mpls_lm_dm_opcode != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "MPLS opcode should be invalid for OAM profile. Please use opcode field instead.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validity checks for bcm_oam_profile_action_get
*/
static shr_error_e
dnx_oam_profile_action_get_verify(
    int unit,
    uint32 flags,
    bcm_oam_profile_t profile_id,
    bcm_oam_action_key_t * oam_action_key,
    bcm_oam_action_result_t * oam_action_result)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Validity checks */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_profile_action_common_verify(unit, flags, profile_id, oam_action_key,
                                                               oam_action_result));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validity checks for bcm_oam_profile_action_set
*/
static shr_error_e
dnx_oam_profile_action_set_verify(
    int unit,
    uint32 is_ingress,
    uint32 flags,
    bcm_oam_profile_t profile_id,
    bcm_oam_action_key_t * oam_action_key,
    bcm_oam_action_result_t * oam_action_result)
{
    bcm_oam_profile_type_t profile_type;
    int reflector_trap_id, trap_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate OAM action result fields */
    if (oam_action_result->flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "OAM action result flags combination is not supported\r\n");
    }

    if (oam_action_result->counter_increment != 0 && oam_action_result->counter_increment != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "OAM action result count enable is out of range (0-1)\r\n");
    }
    else if (((oam_action_key->endpoint_type != bcmOAMMatchTypeMEP)
              && (oam_action_key->endpoint_type != bcmOAMMatchTypePassive))
             && oam_action_result->counter_increment != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "OAM action result count enable may only be set for bcmOAMMatchTypeMEP\r\n");
    }

    if (oam_action_result->meter_enable != 0 && oam_action_result->meter_enable != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "OAM action result meter enable is out of range (0-1)\r\n");
    }

    if (oam_action_result->destination != BCM_GPORT_INVALID && !BCM_GPORT_IS_TRAP(oam_action_result->destination))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "OAM action result destination must be set to a trap gport\r\n");
    }

    /** Resolve profile type */
    profile_type = DNX_OAM_PROFILE_TYPE_GET(profile_id);

    /** Validate OAM action key endpoint_type field */
    if ((profile_type == bcmOAMProfileIngressAcceleratedEndpoint
         || profile_type == bcmOAMProfileEgressAcceleratedEndpoint)
        && oam_action_key->endpoint_type != bcmOAMMatchTypeMEP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint type for accelerated profile is not valid\n");
    }

    /** Validate upmep reflector action - validate reflector only used for LBM*/
    SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamReflector, &reflector_trap_id));
    trap_id = BCM_GPORT_TRAP_GET_ID(oam_action_result->destination);    /* will set to -1 in case destination is not
                                                                         * trap */
    if (reflector_trap_id == trap_id)
    {
        if (oam_action_key->opcode != bcmOamOpcodeLBM)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Got OAM reflector trap, expected opcode to be LBM\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reterives the action to be taken in
 *        different cases of OAM packet classification. For
 *        example, destination of OAM CCM messages with lower
 *        level than the lowest existing MEP on the LIF.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - optional flag - BCM_OAM_ACTION_KEY_MPLS_LM_DM_OPCODE for RFC-6374 actions
 * \param [in] profile_id - The profile id required. Direction
 *        (ing/eg) is based on profile type.
 * \param [in] oam_action_key - Combination of opcode,
 *        destination MAC address type (UC/MC/MyCFM) and MEP
 *        type (MEP/MIP/Passive/LowMDL) to change the action
 *        for.
 *          flags - Currently not in use.
 *          opcode - OAM opcode to be affected by the action.
 *          dest_mac_type - Destination MAC type (UC/MC/MyCFM)
 *                          to be affected by the action.
 *          endpoint_type - Type of endpoint
 *                          (MEP/MIP/Passive/LowMDL) to be
 *                          affected by the action.
 *          inject - 1 for affecting transmitted packets by the
 *                   device. 0 for received packets.
 * \param [out] oam_action_result - The actual new action (new
 *        destination, counter / meter enabling, LM type and
 *        time format).
 *          flags - Currently not in use.
 *                  count_enable - 1 for enable counting for
 *                  packets matches the entry. 0 for no
 *                  counting.
 *          meter_enable - 1 for enable metering for packets
 *                         matches the entry. 0 for no
 *                         metering.
 *          destination - Detination (must be a trap
 *                        gport) to redirect packets matches the
 *                        entry.
 *          time_format - Controls the time format (NTP / 1588)
 *                        to use for delay measurement.
 *
 * \return
 *   int
 *
 * \remark
 *   * Following memories are accessed:
 *     DBAL_TABLE_INGRESS_OAM_LIF_ACTION_DB,
 *     DBAL_TABLE_EGRESS_OAM_LIF_ACTION_DB,
 *     DBAL_TABLE_INGRESS_OAM_ACC_MEP_ACTION_DB,
 *     DBAL_TABLE_EGRESS_OAM_ACC_MEP_ACTION_DB
 *
 * \see
 *   bcm_oam_profile_action_get (oam.h)
 */
int
bcm_dnx_oam_profile_action_get(
    int unit,
    uint32 flags,
    bcm_oam_profile_t profile_id,
    bcm_oam_action_key_t * oam_action_key,
    bcm_oam_action_result_t * oam_action_result)
{
    oam_action_key_t action_key;
    oam_action_content_t action_content;
    bcm_oam_profile_type_t profile_type;
    uint32 is_ingress;
    uint32 oam_action_get_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Validity checks */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_profile_action_get_verify(unit, flags, profile_id, oam_action_key,
                                                            oam_action_result));

    /** Resolve profile type */
    profile_type = DNX_OAM_PROFILE_TYPE_GET(profile_id);

    /** Determine direction based on profile type */
    is_ingress = DNX_OAM_PROFILE_IS_INGRESS(profile_type);

    /*
     * Fill OAM action key
     */

    SHR_IF_ERR_EXIT(dnx_oam_profile_action_table_key_fill(unit, profile_id, oam_action_key, &action_key));

    /*
     * Fill OAM action result based on OAM action table data
     */

    /** Set flags */
    oam_action_get_flags |= is_ingress ? DNX_OAM_CLASSIFIER_INGRESS : DNX_OAM_CLASSIFIER_EGRESS;
    oam_action_get_flags |= DNX_OAM_PROFILE_IS_LIF_DB_ASSOCIATED(profile_type) ? DNX_OAM_CLASSIFIER_LIF_ACTION :
        DNX_OAM_CLASSIFIER_ACC_MEP_ACTION;

    /** Get result from OAM action table */
    SHR_IF_ERR_EXIT(dnx_oam_action_get(unit, oam_action_get_flags, &action_key, &action_content));

    /** Counter enbale depends on profile and endpoint_type */
    SHR_IF_ERR_EXIT(dnx_oam_profile_counter_enable_calc(unit, profile_type, oam_action_key->endpoint_type,
                                                        action_content.oama_counter_disable_access_0,
                                                        action_content.oamb_counter_disable, oam_action_result));

    /** Fields depend on direction */
    if (is_ingress)
    {
        oam_action_result->destination = BCM_GPORT_INVALID;
        if (action_content.table_specific_fields.ingress_only_fields.ingress_trap_code)
        {
            BCM_GPORT_TRAP_SET(oam_action_result->destination,
                               action_content.table_specific_fields.ingress_only_fields.ingress_trap_code,
                               action_content.forwarding_strength, action_content.snoop_strength);
        }
        oam_action_result->meter_enable = !action_content.table_specific_fields.ingress_only_fields.meter_disable;
    }
    else
    {
        int trap_id;

        if (action_content.forwarding_strength > 0)
        {
            int trap_type = DNX_RX_TRAP_ETPP_OAM_INVALID;
            switch (action_content.table_specific_fields.egress_only_fields.egress_action_profile)
            {
                case DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_1:
                    trap_type = DNX_RX_TRAP_ETPP_OAM_REFLECTOR;
                    break;
                case DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_2:
                    trap_type = DNX_RX_TRAP_ETPP_OAM_UP_MEP_OAMP;
                    break;
                case DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_3:
                {
                    if (action_content.oam_sub_type == DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_LEVEL_ERROR)
                    {
                        trap_type = DNX_RX_TRAP_ETPP_OAM_LEVEL_ERR;
                    }
                    else
                    {
                        if (action_content.oam_sub_type == DBAL_ENUM_FVAL_OAM_SUB_TYPE_OAM_PASSIVE_ERROR)
                        {
                            trap_type = DNX_RX_TRAP_ETPP_OAM_PASSIVE_ERR;
                        }
                        else
                        {
                            trap_type = DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST1;
                        }
                    }
                }
                    break;
                case DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_4:
                    trap_type = DNX_RX_TRAP_ETPP_OAM_UP_MEP_DEST2;
                    break;
                default:
                    break;
            }

            trap_type =
                DNX_RX_TRAP_ETPP_OAM_TRAP_TYPE_SET(action_content.table_specific_fields.
                                                   egress_only_fields.egress_action_profile, trap_type);
            trap_id = DNX_RX_TRAP_ID_SET(DNX_RX_TRAP_BLOCK_ETPP, DNX_RX_TRAP_CLASS_OAM, trap_type);
        }
        else
        {
            trap_id = oam_action_result->destination =
                action_content.table_specific_fields.egress_only_fields.egress_action_profile;
        }

        BCM_GPORT_TRAP_SET(oam_action_result->destination,
                           trap_id, action_content.forwarding_strength, action_content.snoop_strength);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function changes the deafult action to be taken
 *        in different cases of OAM packet classification. For
 *        example, redirect OAM CCM messages which hit a MIP to
 *        a new destination.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - optional flag - BCM_OAM_ACTION_KEY_MPLS_LM_DM_OPCODE for RFC-6374 actions
 * \param [in] profile_id - The profile to change the action
 *        for. Direction (ing/eg) is based on profile type.
 * \param [in] oam_action_key - Combination of opcode,
 *        destination MAC address type (UC/MC/MyCFM) and MEP
 *        type (MEP/MIP/Passive/LowMDL) to change the action
 *        for.
 *          flags - Currently not in use.
 *          opcode - OAM opcode to be affected by the action.
 *          dest_mac_type - Destination MAC type (UC/MC/MyCFM)
 *                          to be affected by the action.
 *          endpoint_type - Type of endpoint
 *                          (MEP/MIP/Passive/LowMDL) to be
 *                          affected by the action.
 *          inject - 1 for affecting transmitted packets by the
 *                   device. 0 for received packets.
 * \param [in] oam_action_result - The actual new action (new
 *        destination, counter / meter enabling, LM type and
 *        time format).
 *          flags - Currently not in use.
 *                  count_enable - 1 for enable counting for
 *                  packets matches the entry. 0 for no
 *                  counting.
 *          meter_enable - 1 for enable metering for packets
 *                         matches the entry. 0 for no
 *                         metering.
 *          destination - Detination (must be a trap
 *                        gport) to redirect packets matches the
 *                        entry.
 *          time_format - Controls the time format (NTP / 1588)
 *                        to use for delay measurement.
 *
 * \return
 *   int
 *
 * \remark
 *   * Following memories may be changed:
 *     DBAL_TABLE_INGRESS_OAM_LIF_ACTION_DB,
 *     DBAL_TABLE_EGRESS_OAM_LIF_ACTION_DB,
 *     DBAL_TABLE_INGRESS_OAM_ACC_MEP_ACTION_DB,
 *     DBAL_TABLE_EGRESS_OAM_ACC_MEP_ACTION_DB
 *
 * \see
 *   bcm_oam_profile_action_set (oam.h)
 */
int
bcm_dnx_oam_profile_action_set(
    int unit,
    uint32 flags,
    bcm_oam_profile_t profile_id,
    bcm_oam_action_key_t * oam_action_key,
    bcm_oam_action_result_t * oam_action_result)
{
    oam_action_key_t action_key;
    oam_action_content_t action_content;
    bcm_oam_profile_type_t profile_type;
    uint8 is_acc;
    int profile_index;
    uint32 is_ingress;
    dbal_enum_value_field_mp_type_e mp_type;
    dbal_enum_value_field_oam_sub_type_e local_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_NULL;
    dbal_enum_value_field_oam_sub_type_e inject_sub_type = DBAL_ENUM_FVAL_OAM_SUB_TYPE_NULL;
    uint32 local_forwarding_strength;
    uint32 local_snoop_strength;
    uint32 local_trap_code;
    uint32 oam_action_set_flags = 0;
    uint8 is_level_or_passive_trap = 0, dummy = 0;
    uint8 lm_type = 0, is_section = 0, is_hlm_disable = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Validity checks */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_profile_action_common_verify(unit, flags, profile_id, oam_action_key,
                                                               oam_action_result));
    /** Resolve profile type */
    profile_type = DNX_OAM_PROFILE_TYPE_GET(profile_id);

    /** Determine direction based on profile type */
    is_ingress = DNX_OAM_PROFILE_IS_INGRESS(profile_type);

    /** Validity checks */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_profile_action_set_verify(unit, is_ingress, flags, profile_id, oam_action_key,
                                                            oam_action_result));

    /*
     * Fill OAM action key
     */

    SHR_IF_ERR_EXIT(dnx_oam_profile_action_table_key_fill(unit, profile_id, oam_action_key, &action_key));

    /*
     * Regarding action_key.mp_type,
     * Several mp types can match one endpoint type and all should be updated.
     * For example, if endpoint type is bcmOAMMatchTypePassive following mp types should be set with same action:
     * DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_(HIGH,MIDDLE,LOW,NO_COUNTER)
     */

    /*
     * Fill OAM action result
     */

    /** Fields depend on direction */
    if (is_ingress)
    {
        action_content.table_specific_fields.ingress_only_fields.ingress_trap_code = 0;
        action_content.forwarding_strength = 0;
        action_content.snoop_strength = 0;
        if (oam_action_result->destination != BCM_GPORT_INVALID)
        {
            action_content.table_specific_fields.ingress_only_fields.ingress_trap_code =
                BCM_GPORT_TRAP_GET_ID(oam_action_result->destination);
            action_content.forwarding_strength = BCM_GPORT_TRAP_GET_STRENGTH(oam_action_result->destination);
            action_content.snoop_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(oam_action_result->destination);
        }
        action_content.table_specific_fields.ingress_only_fields.meter_disable = !oam_action_result->meter_enable;
        action_content.table_specific_fields.ingress_only_fields.is_up_mep = oam_action_key->inject;
    }
    else
    {
        /**
         * Check gport type
         */
        if (BCM_GPORT_IS_TRAP(oam_action_result->destination))
        {
            action_content.forwarding_strength = BCM_GPORT_TRAP_GET_STRENGTH(oam_action_result->destination);
            action_content.snoop_strength = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(oam_action_result->destination);

            if (action_content.snoop_strength == 0)
            {
                action_content.table_specific_fields.egress_only_fields.egress_action_profile =
                    DNX_RX_TRAP_ETPP_OAM_ACTION_PROFILE_GET(BCM_GPORT_TRAP_GET_ID(oam_action_result->destination));
            }
            else
            {
                action_content.table_specific_fields.egress_only_fields.egress_action_profile =
                    BCM_GPORT_TRAP_GET_ID(oam_action_result->destination);
            }

            if ((action_content.forwarding_strength == 0) && (action_content.snoop_strength == 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "OAM action forward strength or snoop strength should be bigger than 0\r\n");
            }

            if ((action_content.forwarding_strength != 0) && (action_content.snoop_strength != 0))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "OAM action forward strength can't be set together with snoop strength. One of them should be 0.\r\n");
            }
        }
        else
        {
            action_content.snoop_strength = 7;
            action_content.forwarding_strength = 7;
            action_content.table_specific_fields.egress_only_fields.egress_action_profile = 0;
            if (oam_action_result->destination != BCM_GPORT_INVALID)
            {

                SHR_ERR_EXIT(_SHR_E_PARAM, "OAM action destination type should be trap\r\n");
            }
        }
    }

    /** Calculate sub_type based on Opcode, LM type, time format */
    SHR_IF_ERR_EXIT(dnx_oam_profile_sub_type_calc
                    (unit, is_ingress, profile_id, action_key.oam_internal_opcode, oam_action_result->destination,
                     &is_level_or_passive_trap, &action_content));
    local_sub_type = action_content.oam_sub_type;

    if (is_level_or_passive_trap)
    {
        /** Calculate sub_type for the inject entry */
        local_forwarding_strength = action_content.forwarding_strength;
        /** Trap code is not relevant for inject entry */
        action_content.forwarding_strength = 0;
        SHR_IF_ERR_EXIT(dnx_oam_profile_sub_type_calc
                        (unit, is_ingress, profile_id, action_key.oam_internal_opcode, oam_action_result->destination,
                         &dummy, &action_content));

        inject_sub_type = action_content.oam_sub_type;
        /** Restore the sub_type and forwarding strength */
        action_content.oam_sub_type = local_sub_type;
        action_content.forwarding_strength = local_forwarding_strength;
    }

    /*
     * Set result on OAM action table for all relevant entries (different mp_types)
     */

    /** Set flags */
    oam_action_set_flags |= is_ingress ? DNX_OAM_CLASSIFIER_INGRESS : DNX_OAM_CLASSIFIER_EGRESS;
    oam_action_set_flags |= DNX_OAM_PROFILE_IS_LIF_DB_ASSOCIATED(profile_type) ? DNX_OAM_CLASSIFIER_LIF_ACTION :
        DNX_OAM_CLASSIFIER_ACC_MEP_ACTION;

    /** Check if the given profile is accelerated */
    is_acc = !(DNX_OAM_PROFILE_IS_LIF_DB_ASSOCIATED(profile_type));

    profile_index = DNX_OAM_PROFILE_IDX_GET(profile_id);

    /** Get profile's OAM type */
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_lm_mode_get(unit, profile_index, is_ingress, is_acc, &lm_type, &is_section));

    SHR_IF_ERR_EXIT(dnx_oam_sw_db_hlm_mode_get(unit, profile_index, is_ingress, is_acc, &is_hlm_disable));

    if (!DNX_OAM_MP_TYPE_IS_RFC_6374(oam_action_key->flags))
    {
        for (mp_type = 0; mp_type < DBAL_NOF_ENUM_MP_TYPE_VALUES; mp_type++)
        {
            if (DNX_OAM_ENDPOINT_TYPE_PER_MP_TYPE(mp_type) == oam_action_key->endpoint_type)
            {
            /** mp_type matches endpoint_type. Set the entry */
                action_key.mp_type = mp_type;

            /** Calculate counter disable values based on mp_type */
                SHR_IF_ERR_EXIT(dnx_oam_profile_counter_disable_calc(unit, profile_id, mp_type,
                                                                     oam_action_result->counter_increment,
                                                                     is_section, is_hlm_disable, &action_content));

            /** Set the entry on OAM action table */
                SHR_IF_ERR_EXIT(dnx_oam_action_set(unit, oam_action_set_flags, &action_key, &action_content));

            /** Set the inject entry */
                if ((DNX_OAM_ENDPOINT_TYPE_PER_MP_TYPE(mp_type) == bcmOAMMatchTypeMEP) && (action_key.is_inject == 0))
                {
                    action_key.is_inject = 1;
                    action_content.table_specific_fields.ingress_only_fields.is_up_mep = action_key.is_inject;
                    local_forwarding_strength = action_content.forwarding_strength;
                    local_snoop_strength = action_content.snoop_strength;
                    local_sub_type = action_content.oam_sub_type;
                    if (is_ingress)
                    {
                        local_trap_code = action_content.table_specific_fields.ingress_only_fields.ingress_trap_code;
                    }
                    else
                    {
                        local_trap_code = action_content.table_specific_fields.egress_only_fields.egress_action_profile;
                    }

                    /*
                     * Configure forwarding_strength to 0 so trap code won't be relevant for inject=1.
                     */
                    action_content.forwarding_strength = 0;
                    action_content.snoop_strength = 0;
                    action_content.table_specific_fields.ingress_only_fields.ingress_trap_code = 0;

                    if (is_level_or_passive_trap)
                    {
                        action_content.oam_sub_type = inject_sub_type;
                    }

               /** Set the entry on OAM action table */
                    SHR_IF_ERR_EXIT(dnx_oam_action_set(unit, oam_action_set_flags, &action_key, &action_content));

               /** Configure inject=1, passive side */
                    action_key.mp_type = DNX_OAM_ENDPOINT_PASSIVE_TYPE_OF(mp_type);
                    SHR_IF_ERR_EXIT(dnx_oam_action_set(unit, oam_action_set_flags, &action_key, &action_content));

                    action_content.forwarding_strength = local_forwarding_strength;
                    action_content.snoop_strength = local_snoop_strength;
                    action_content.oam_sub_type = local_sub_type;
                    if (is_ingress)
                    {
                        action_content.table_specific_fields.ingress_only_fields.ingress_trap_code = local_trap_code;
                    }
                    else
                    {
                        action_content.table_specific_fields.egress_only_fields.egress_action_profile = local_trap_code;
                    }
                    action_key.is_inject = 0;
                    action_content.table_specific_fields.ingress_only_fields.is_up_mep = oam_action_key->inject;
                }
            }
        }
    }
    else
    {
        /** For rfc 6374 endpoint_type is bcmOAMMatchTypeMEP and the flag is on */
        action_key.mp_type = DBAL_ENUM_FVAL_MP_TYPE_BFD;

        /** Initialize all to enable counting */
        action_content.oama_counter_disable_access_0 = DNX_OAM_COUNT_ENABLE;
        action_content.oama_counter_disable_access_1 = DNX_OAM_COUNT_ENABLE;
        action_content.oama_counter_disable_access_2 = DNX_OAM_COUNT_ENABLE;
        action_content.oamb_counter_disable = DNX_OAM_COUNT_ENABLE;

        if (DNX_OAM_PROFILE_IS_LIF_DB_ASSOCIATED(profile_type))
        {
            /** Get profile's OAM type */
            SHR_IF_ERR_EXIT(dnx_oam_sw_db_lm_mode_get(unit, profile_index, is_ingress, is_acc, &lm_type, &is_section));

            if (is_section)
            {
                action_content.oama_counter_disable_access_0 = !oam_action_result->counter_increment;
                action_content.oama_counter_disable_access_1 = !oam_action_result->counter_increment;
                action_content.oama_counter_disable_access_2 = !oam_action_result->counter_increment;
            }
            else
            {
                action_content.oama_counter_disable_access_0 = !oam_action_result->counter_increment;
            }
            /** OAMB not accessed. Must be 0 */
            action_content.oamb_counter_disable = DNX_OAM_COUNT_ENABLE;
        }

        SHR_IF_ERR_EXIT(dnx_oam_action_set(unit, oam_action_set_flags, &action_key, &action_content));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
