/** \file policer_apis.c
 * 
 *  policer module gather all functionality that related to the policer/meter.
 *  policer_apis.c holds all the policer APIs implemantation for DNX
 *  It's matching h file is bcm/policer.h
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_POLICER

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/policer.h>
#include <shared/policer.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/policer/policer_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/meter_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_access.h>
#include "meter_utils.h"
#include "meter_generic.h"
#include "meter_global.h"
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>

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

/**
* \brief
*      check that database id is in the valid limits 
* \param [in] unit -unit id
* \param [in] is_ingress -ingress/egress
* \param [in] database_id -database_id
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_policer_database_id_verify(
    int unit,
    int is_ingress,
    int database_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_ingress == TRUE && database_id >= dnx_data_meter.meter_db.nof_ingress_db_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid database_id (=%d). max allowed for ingress is %d\n",
                     database_id, dnx_data_meter.meter_db.nof_ingress_db_get(unit));
    }
    if (is_ingress == FALSE && database_id >= dnx_data_meter.meter_db.nof_egress_db_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid database_id (=%d). max allowed for egress is %d\n",
                     database_id, dnx_data_meter.meter_db.nof_egress_db_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*      verify database handle macro is correct
* \param [in] unit -unit id
* \param [in] database_handle -macro which hold the database key
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_policer_database_handle_verify(
    int unit,
    int database_handle)
{
    int core_id, is_global, is_ingress, database_id;
    SHR_FUNC_INIT_VARS(unit);

    if ((database_handle >> _SHR_POLICER_DATABASE_HANDLE_MACRO_IND_POSITION) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  database_handle (=%d)\n", database_handle);
    }

    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle);
    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(database_handle);

    if (is_global == FALSE)
    {
        is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(database_handle);
        database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(database_handle);
        SHR_IF_ERR_EXIT(dnx_policer_database_id_verify(unit, is_ingress, database_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*      verify policer id macro is correct
* \param [in] unit -unit id
* \param [in] policer_id -macro which hold the meter idx and database handle
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_policer_id_verify(
    int unit,
    bcm_policer_t policer_id)
{
    int database_handle;
    int meter_idx, is_global;
    int core_id;
    int is_ingress, database_id;
    int valid;
    SHR_FUNC_INIT_VARS(unit);

    if ((policer_id >> _SHR_POLICER_ID_MACRO_IND_POSITION) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  policer_id (=%d)\n", policer_id);
    }
    /** verify database_handle macro */
    database_handle = _SHR_POLICER_ID_DATABASE_HANDLE_GET(policer_id);
    SHR_IF_ERR_EXIT(dnx_policer_database_handle_verify(unit, database_handle));

    meter_idx = _SHR_POLICER_ID_METER_INDEX_GET(policer_id);
    is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(database_handle);
    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle);
    is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(database_handle);
    database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(database_handle);

    if (is_global)
    {
        /** meter_idx should be 0-4 */
        if (meter_idx >= dnx_data_meter.profile.nof_valid_global_meters_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "meter_idx (=%d) is not valid\n", meter_idx);
        }
    }
    /** generic */
    else
    {
        SHR_IF_ERR_EXIT(dnx_meter_generic_index_verify(unit, core_id, is_ingress, database_id, meter_idx, &valid));
        if (valid == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "meter_idx (=%d) does not belong to the given database (is_ingress=%d, database_id=%d)\n",
                         meter_idx, is_ingress, database_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      verify that the rate (cir,eir,max_cir, max_eir) and burst is above max values allowed
 * \param [in] unit -unit id
 * \param [in] pol_cfg -configuration of meter profile
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_policer_min_max_info_check(
    int unit,
    bcm_policer_config_t * pol_cfg)
{
    bcm_policer_config_t pol_cfg_rates;
    dnx_policer_profile_limits_info_t profile_limit_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_policer_config_t_init(&pol_cfg_rates);
    pol_cfg_rates.ckbits_sec = pol_cfg->ckbits_sec;
    pol_cfg_rates.ckbits_burst = pol_cfg->ckbits_burst;
    pol_cfg_rates.pkbits_sec = pol_cfg->pkbits_sec;
    pol_cfg_rates.pkbits_burst = pol_cfg->pkbits_burst;
    pol_cfg_rates.max_pkbits_sec = pol_cfg->max_pkbits_sec;
    pol_cfg_rates.max_ckbits_sec = pol_cfg->max_ckbits_sec;

    if (pol_cfg->flags & BCM_POLICER_MODE_PACKETS)
    {
        SHR_IF_ERR_EXIT(dnx_policer_correct_config_rates(unit, &pol_cfg_rates, TRUE));
    }

    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.get(unit, &profile_limit_info));

    /** verify configuration is not above max values */
    if (pol_cfg_rates.ckbits_sec > profile_limit_info.max_rate ||
        pol_cfg_rates.pkbits_sec > profile_limit_info.max_rate ||
        pol_cfg_rates.ckbits_burst > DNX_POLICER_BYTES_TO_KBITS(profile_limit_info.max_burst_large_mode) ||
        pol_cfg_rates.pkbits_burst > DNX_POLICER_BYTES_TO_KBITS(profile_limit_info.max_burst_large_mode))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "one or more of the rates/bursts is above max \n");
    }
    if (pol_cfg_rates.max_pkbits_sec != DNX_METER_UTILS_MAX_RATE_UNLIMITED &&
        (pol_cfg_rates.max_pkbits_sec > profile_limit_info.max_rate ||
         pol_cfg_rates.max_pkbits_sec < pol_cfg_rates.pkbits_sec))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "max_pkbits_sec is above max or < pkbits_sec");
    }
    /** verify rate is not below min values */
    if ((pol_cfg_rates.ckbits_sec < profile_limit_info.min_rate && pol_cfg_rates.ckbits_sec != 0) ||
        (pol_cfg_rates.pkbits_sec < profile_limit_info.min_rate && pol_cfg_rates.pkbits_sec != 0) ||
        (pol_cfg_rates.max_ckbits_sec < profile_limit_info.min_rate && pol_cfg_rates.max_ckbits_sec != 0) ||
        (pol_cfg_rates.max_pkbits_sec < profile_limit_info.min_rate && pol_cfg_rates.max_pkbits_sec != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "one or more of the rates/bursts is below min \n");
    }

    if (pol_cfg->mode == bcmPolicerModeCascade || pol_cfg->mode == bcmPolicerModeCoupledCascade)
    {
        if (pol_cfg_rates.max_ckbits_sec != DNX_METER_UTILS_MAX_RATE_UNLIMITED &&
            (pol_cfg_rates.max_ckbits_sec > profile_limit_info.max_rate ||
             pol_cfg_rates.max_ckbits_sec < pol_cfg_rates.ckbits_sec))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "max_ckbits_sec is above max or < ckbits_sec");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify function for global meter configuration
* \param [in] unit       - unit id
* \param [in] pol_cfg    - config struct
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_global_config_verify(
    int unit,
    bcm_policer_config_t * pol_cfg)
{
    uint32 legal_flags;

    SHR_FUNC_INIT_VARS(unit);

    legal_flags = BCM_POLICER_COLOR_BLIND | BCM_POLICER_MODE_PACKETS | BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE;

    /*
     * only these flags are supported for meters 
     */
    if ((pol_cfg->flags & ~legal_flags) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal flags specified");
    }

    /** ethernet policer, has one bucket (commited) */
    if (pol_cfg->mode != bcmPolicerModeCommitted)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "only bcmPolicerModeCommitted mode is supported for global meter");
    }
    /** data profile is 1 bit */
    if (pol_cfg->color_resolve_profile != 0 && pol_cfg->color_resolve_profile != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "color_resolve_profile valid values are: 0 or 1");
    }

    /** verify profile rate and burst is not above max allowed */
    SHR_IF_ERR_EXIT(dnx_policer_min_max_info_check(unit, pol_cfg));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify function for generic meter configuration
* \param [in] unit       - unit id
* \param [in] pol_cfg    - config struct
* \param [in] database_handle    - database_handle macro
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_generic_config_verify(
    int unit,
    bcm_policer_config_t * pol_cfg,
    int database_handle)
{
    uint32 legal_flags;
    uint32 is_single_bucket;
    int core_id, database_id, is_ingress;
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);

    legal_flags = BCM_POLICER_REPLACE | BCM_POLICER_COLOR_BLIND | BCM_POLICER_MODE_PACKETS |
        BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE | BCM_POLICER_REPLACE_SHARED | BCM_POLICER_LARGE_BUCKET_MODE;

    /*
     * only these flags are supported for generic meters 
     */
    if ((pol_cfg->flags & ~legal_flags) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal flags specified");
    }
    /** replace_shared, means to change the configuration of the profile, so it must be used with specific profile_id (=entropy_id) */
    if ((pol_cfg->flags & BCM_POLICER_REPLACE_SHARED) && (pol_cfg->entropy_id == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_POLICER_REPLACE_SHARED is valid only with non-zero entropy_id\n");
    }

    /** 
     * Get data on the number of profiles from HW dbal tables. Ingress and Egress has the same configuration in HW. 
     * we take the data from ingress, but it belong also to egress. Check if the entropy ID is within valid range.
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_METER_ING_PROFILE_CONFIG, DBAL_FIELD_PROFILE_INDEX, TRUE, 0, 0, &field_info));
    SHR_MAX_VERIFY(pol_cfg->entropy_id, field_info.max_value, _SHR_E_PARAM, "Entropy ID out of bounds!");

    if ((pol_cfg->entropy_id != 0) && (pol_cfg->flags & BCM_POLICER_REPLACE) &&
        (pol_cfg->mode == bcmPolicerModeCascade || pol_cfg->mode == bcmPolicerModeCoupledCascade))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "entropy_id != 0 is not allowed with mode Cascade or CoupledCascade\n");
    }

    if (pol_cfg->ckbits_sec != 0 && pol_cfg->ckbits_burst == 0)
    {
        /** if cir isn't zero, cbs can't be zero */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Zero burst size isn't supported, 64 bytes is the minimal value \n");
    }
    if (pol_cfg->pkbits_sec != 0 && pol_cfg->pkbits_burst == 0)
    {
        /** if eir isn't zero, ebs can't be zero */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Zero burst size isn't supported, 64 bytes is the minimal value \n");
    }

    database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(database_handle);
    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle);
    if (core_id == BCM_CORE_ALL)
    {
        core_id = 0;
    }
    is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(database_handle);
    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.is_single_bucket.get
                    (unit, core_id, is_ingress, database_id, &is_single_bucket));

    switch (pol_cfg->mode)
    {
        case bcmPolicerModeSrTcm:
            if (pol_cfg->pkbits_sec != 0)
            {
                 /** eir has to be zero in single rate */
                SHR_ERR_EXIT(_SHR_E_PARAM, "In single rate, EIR have to be equal to 0 \n");
            }
            if (pol_cfg->max_pkbits_sec == 0)
            {
                /** max eir shouldn't be zero in SrTcm  */
                SHR_ERR_EXIT(_SHR_E_PARAM, "In single-rate two-colors, Max EIR shouldn't be equal to zero \n");
            }
            if (pol_cfg->ckbits_sec != 0 && pol_cfg->ckbits_burst == 0)
            {
                /** if cir isn't zero, cbs can't be zero */
                SHR_ERR_EXIT(_SHR_E_PARAM, "Zero burst size isn't supported, 64 bytes is the minimal value \n");
            }
            if (is_single_bucket == TRUE)
            {
                /** Tcm are not supported if the database is single bucket */
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "mode=%d is not supported when the meter database (database_id=%d) working in single bucket mode\n",
                             bcmPolicerModeSrTcm, database_id);
            }
            break;
        /** two rate three colors */
        case bcmPolicerModeTrTcmDs:
        /** two rates three colors with coupling */
        case bcmPolicerModeCoupledTrTcmDs:
            if (is_single_bucket == TRUE)
            {
                /** Tcm are not supported if the database is single bucket */
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "mode=%d is not supported when the meter database (database_id=%d) working in single bucket mode\n",
                             pol_cfg->mode, database_id);
            }
            break;
        case bcmPolicerModeCommitted:
            /** single rate single burst */
            if (pol_cfg->pkbits_sec != 0 || pol_cfg->pkbits_burst != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "In single rate, EIR and Excess Burst size have to be equal to 0 \n");
            }
            break;
            /** 4 meters work in hierarchical mode */
        case bcmPolicerModeCascade:
            /** 4 meters work in hierarchical mode with coupling */
        case bcmPolicerModeCoupledCascade:
            /** replace share flag is used to replace the profile data. 
                             In cascade, one change of profile may cause modification in other 3 profiles. 
                             So the complexity is high and currently, it is not supported. */
            if (pol_cfg->flags & BCM_POLICER_REPLACE_SHARED)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_POLICER_REPLACE_SHARED is not supported for hierarchical mode \n");
            }
            if (is_single_bucket == TRUE)
            {
                /** Tcm are not supported if the database is single bucket */
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "mode=%d is not supported when the meter database (database_id=%d) working in single bucket mode\n",
                             pol_cfg->mode, database_id);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid policer mode: %d \n", pol_cfg->mode);
    }

    /** data profile is 1 bit */
    if (pol_cfg->color_resolve_profile != 0 && pol_cfg->color_resolve_profile != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "color_resolve_profile valid values are: 0 or 1");
    }

    /** verify profile rate and burst is not above max allowed */
    SHR_IF_ERR_EXIT(dnx_policer_min_max_info_check(unit, pol_cfg));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify the API bcm_dnx_policer_expansion_groups_set
* \param [in] unit - 
* \param [in] flags - 
* \param [in] core_id - 
* \param [in] expansion_group  - 
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_expansion_groups_set_verify(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_policer_expansion_group_t * expansion_group)
{
    int group_idx;
    SHR_FUNC_INIT_VARS(unit);

    /** Check for valid core - BCM_CORE_ALL allowed */
    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);
    SHR_NULL_CHECK(expansion_group, _SHR_E_PARAM, "expansion_group");

    /** Check valid flags */
    if ((flags != 0) && (flags & ~(BCM_POLICER_EXPANSION_GROUPS_TC | BCM_POLICER_EXPANSION_GROUPS_FWD)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Unsupported flags=%d\n", flags);
    }

    /** If user is trying to set also TC expansion groups, check if feature is supported */
    if ((dnx_data_meter.expansion.feature_get(unit, dnx_data_meter_expansion_expansion_based_on_tc_support) == FALSE) &&
        (_SHR_IS_FLAG_SET(flags, BCM_POLICER_EXPANSION_GROUPS_TC)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Expansion based on TC is not supported for current device!\n");
    }

    /** Parameters verifications */
    if ((flags == 0) || _SHR_IS_FLAG_SET(flags, BCM_POLICER_EXPANSION_GROUPS_FWD))
    {
        /** If user wants to configure L2-FWD groups, check valid offset */
        for (group_idx = 0; group_idx < bcmPolicerFwdTypeMax; group_idx++)
        {
            if (expansion_group->config[group_idx].offset >= bcmPolicerFwdTypeMax)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid  offset(=%d)\n", expansion_group->config[group_idx].offset);
            }
        }
    }
    if (_SHR_IS_FLAG_SET(flags, BCM_POLICER_EXPANSION_GROUPS_TC))
    {
        /** If user wants to configure TS groups, check valid offset */
        for (group_idx = 0; group_idx < BCM_COS_COUNT; group_idx++)
        {
            if (expansion_group->tc_offset[group_idx] >= BCM_COS_COUNT)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid  offset(=%d)\n", expansion_group->tc_offset[group_idx]);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   handle the API bcm_dnx_policer_expansion_groups_set
* \param [in] unit - unit id
* \param [in] flags - 
* \param [in] core_id - allow core all
* \param [in] expansion_group  - config structure
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_policer_expansion_groups_set(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_policer_expansion_group_t * expansion_group)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_expansion_groups_set_verify(unit, flags, core_id, expansion_group));
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_expansion_groups_set(unit, flags, core_id, expansion_group));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify the API bcm_dnx_policer_expansion_groups_get
* \param [in] unit - 
* \param [in] flags - 
* \param [in] core_id - 
* \param [in] expansion_group  - 
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_expansion_groups_get_verify(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_policer_expansion_group_t * expansion_group)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check for valid core - BCM_CORE_ALL allowed */
    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);
    SHR_NULL_CHECK(expansion_group, _SHR_E_PARAM, "expansion_group");

    /** Check valid flags */
    if ((flags != 0) && (flags & ~(BCM_POLICER_EXPANSION_GROUPS_TC | BCM_POLICER_EXPANSION_GROUPS_FWD)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Unsupported flags=%d\n", flags);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_POLICER_EXPANSION_GROUPS_TC)
        && (dnx_data_meter.expansion.feature_get(unit, dnx_data_meter_expansion_expansion_based_on_tc_support) ==
            FALSE))
    {
        /** Feature is not supported */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Expansion based on TC is not supported for current device!\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   get from HW the fwd_type expansion groups
* \param [in] unit - unit id
* \param [in] flags - 
* \param [in] core_id - allow core all
* \param [out] expansion_group  - config structure
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_policer_expansion_groups_get(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_policer_expansion_group_t * expansion_group)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_expansion_groups_get_verify(unit, flags, core_id, expansion_group));
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_expansion_groups_get(unit, flags, core_id, expansion_group));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify the API bcm_dnx_policer_engine_database_attach
* \param [in] unit - unit id
* \param [in] flags - none
* \param [in] policer_database_handle - macro which holds the database id parameters
* \param [in] config  - attach configuration - which database to attach and for which pbjects stat ids.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_database_create_verify(
    int unit,
    int flags,
    int policer_database_handle,
    bcm_policer_database_config_t * config)
{
    int is_global, is_ingress;
    int nof_exp_groups;
    uint32 is_created;
    int core_idx;
    int expansion_flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");
    SHR_BOOL_VERIFY(config->expansion_enable, _SHR_E_PARAM, "expansion_enable must be TRUE or FALSE");
    SHR_BOOL_VERIFY(config->expand_per_tc, _SHR_E_PARAM, "expand_per_tc must be TRUE or FALSE");

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags=%d, expected 0\n", flags);
    }

    SHR_IF_ERR_EXIT(dnx_policer_database_handle_verify(unit, policer_database_handle));

    is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(policer_database_handle);
    is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(policer_database_handle);

    if (is_global == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "policer_database_handle indicates on global meter. not valid for this API\n");
    }
    if ((is_ingress == FALSE) && (config->expansion_enable == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "expansion_enable is not valid for egress policer database\n");
    }
    /** user must configure expansion groups before set expansion_enable=TRUE */
    if (config->expansion_enable == TRUE)
    {
        if ((dnx_data_meter.expansion.feature_get(unit, dnx_data_meter_expansion_expansion_based_on_tc_support) ==
             FALSE) && (config->expand_per_tc == TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Expansion based on TC is not supported! \n");
        }

        expansion_flags = (config->expand_per_tc == TRUE) ? BCM_POLICER_EXPANSION_GROUPS_TC : 0;
        SHR_IF_ERR_EXIT(dnx_policer_mgmt_nof_expansion_groups_get(unit, expansion_flags,
                                                                  _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET
                                                                  (policer_database_handle), &nof_exp_groups));
        if (nof_exp_groups == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "No expansion groups configured. Please configure expansion groups before creating a database with expansion enabled!\n");
        }
    }
    else
    {
        if ((dnx_data_meter.expansion.feature_get(unit, dnx_data_meter_expansion_expansion_based_on_tc_support) == TRUE)
            && (config->expand_per_tc == TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Parameter expansion_enable needs to be TRUE in order to set expansion based on TC! \n");
        }
    }

    DNXCMN_CORES_ITER(unit, _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle), core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.created.get(unit,
                                                                            core_idx,
                                                                            _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET
                                                                            (policer_database_handle),
                                                                            _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET
                                                                            (policer_database_handle), &is_created));
        if (is_created == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "database_id=%d already created\n", _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET
                         (policer_database_handle));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_database_create(
    int unit,
    int flags,
    int policer_database_handle,
    bcm_policer_database_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_database_create_verify(unit, flags, policer_database_handle, config));
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_database_create(unit, flags,
                                                     _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle),
                                                     _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET
                                                     (policer_database_handle),
                                                     _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET
                                                     (policer_database_handle), config));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_database_destroy(
    int unit,
    int flags,
    int policer_database_handle)
{
    int core_idx, is_global, is_ingress, database_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_policer_database_handle_verify(unit, policer_database_handle));
    is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(policer_database_handle);
    database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(policer_database_handle);
    is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(policer_database_handle);
    /** create and destroy database does not relevant for global meter, therefore, we skip it */
    if (is_global == FALSE)
    {
        DNXCMN_CORES_ITER(unit, _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle), core_idx)
        {

            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.created.set
                            (unit, core_idx, is_ingress, database_id, FALSE));

            /** init hw database parameters */
            if (is_ingress)
            {
                SHR_IF_ERR_EXIT(dnx_meter_generic_ingress_db_hw_init(unit, database_id, FALSE));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_meter_generic_egress_db_hw_init(unit, database_id, FALSE));
            }
                /** reset the template manager for this database. 
            Hw will not be cleared. In each engine attach, we clear it and then it is aligned. */
            SHR_IF_ERR_EXIT(POLICER_MNGR_TM_CB(is_ingress, database_id, clear(unit, core_idx)));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify the API bcm_policer_control_set
* \param [in] unit - unit id
* \param [in] flags - none
* \param [in] policer_database_handle - macro which holds the database id parameters
* \param [in] type  - policer control type 
* \param [in] arg - argument 
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_control_set_verify(
    int unit,
    int flags,
    int policer_database_handle,
    bcm_policer_control_type_t type,
    uint32 arg)
{
    int is_global, is_ingress;
    SHR_FUNC_INIT_VARS(unit);

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags=%d, expected 0\n", flags);
    }

    SHR_IF_ERR_EXIT(dnx_policer_database_handle_verify(unit, policer_database_handle));

    switch (type)
    {
        case bcmPolicerControlTypeFilterByDpCommand:
            /** arg - 16b filter bitmap */
            if (arg >= utilex_power_of_2(dnx_data_meter.profile.nof_dp_command_max_get(unit)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " invalid arg (=%d)\n", arg);
            }
            break;
        case bcmPolicerControlTypeDpResolutionByDpCommand:
            /** arg - true/false */
            if (arg != TRUE && arg != FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " invalid arg (=%d)\n", arg);
            }
            is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(policer_database_handle);
            if (is_global)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "bcm_policer_control_set with control bcmPolicerControlTypeDpResolutionByDpCommand - expects ING/EGR meter policer_database_id=%d\n",
                             policer_database_handle);
            }
            break;
        case bcmPolicerControlTypeCountBytes:
            /** arg - true/false */
            if (arg != TRUE && arg != FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " invalid arg (=%d)\n", arg);
            }
            is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(policer_database_handle);
            if (!is_global)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "bcm_policer_control_set with control bcmPolicerControlTypeCountBytes - expects GLOBAL meter policer_database_id=%d\n",
                             policer_database_handle);
            }
            break;
        case bcmPolicerFairness:
            /** arg - true/false */
            if (arg != TRUE && arg != FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, " invalid arg (=%d)\n", arg);
            }
            break;
        case bcmPolicerControlTypeTCSMEnable:
        case bcmPolicerControlTypeTCSMUsePPTC:
            is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(policer_database_handle);
            is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(policer_database_handle);
            if ((is_ingress == FALSE) || (is_global == TRUE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Traffic Class Sensitive Metering is not is not supported for Global or Egress policer database! \n");
            }

            if (dnx_data_meter.meter_db.feature_get(unit, dnx_data_meter_meter_db_tcsm_support) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Traffic Class Sensitive Metering is not is not supported! \n");
            }
            SHR_BOOL_VERIFY(arg, _SHR_E_PARAM, "Argument must be TRUE or FALSE");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid control type (=%d)\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_control_set(
    int unit,
    int flags,
    int policer_database_handle,
    bcm_policer_control_type_t type,
    uint32 arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_control_set_verify(unit, flags, policer_database_handle, type, arg));

    switch (type)
    {
        case bcmPolicerControlTypeFilterByDpCommand:
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_filter_by_dp_command_set(unit, policer_database_handle, arg));
            break;
        case bcmPolicerControlTypeDpResolutionByDpCommand:
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_dp_resolution_by_dp_command_set(unit, policer_database_handle, arg));
            break;
        case bcmPolicerControlTypeCountBytes:
            SHR_IF_ERR_EXIT(dnx_meter_global_ctr_mode_set(unit, policer_database_handle, arg));
            break;
        case bcmPolicerFairness:
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_fairness_set(unit, policer_database_handle, arg));
            break;
        case bcmPolicerControlTypeTCSMEnable:
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_tcsm_enable_set(unit, policer_database_handle, arg));
            break;
        case bcmPolicerControlTypeTCSMUsePPTC:
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_tcsm_use_pp_tc_set(unit, policer_database_handle, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid control type (=%d)\n", type);
            break;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify the API bcm_policer_control_get
* \param [in] unit - unit id
* \param [in] flags - none
* \param [in] policer_database_handle - macro which holds the database id parameters
* \param [in] type  - policer control type 
* \param [in] arg - argument 
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_control_get_verify(
    int unit,
    int flags,
    int policer_database_handle,
    bcm_policer_control_type_t type,
    uint32 *arg)
{
    int is_global, is_ingress;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags=%d, expected 0\n", flags);
    }

    SHR_IF_ERR_EXIT(dnx_policer_database_handle_verify(unit, policer_database_handle));
    is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(policer_database_handle);
    switch (type)
    {
        case bcmPolicerControlTypeFilterByDpCommand:
            break;
        case bcmPolicerControlTypeDpResolutionByDpCommand:
            if (is_global)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "bcm_policer_control_set with control bcmPolicerControlTypeDpResolutionByDpCommand - expects ING/EGR meter policer_database_id=%d\n",
                             policer_database_handle);
            }
            break;
        case bcmPolicerControlTypeCountBytes:
            if (!is_global)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "bcm_policer_control_set with control bcmPolicerControlTypeCountBytes - expects GLOBAL meter policer_database_id=%d\n",
                             policer_database_handle);
            }
            break;
        case bcmPolicerFairness:
            break;
        case bcmPolicerControlTypeTCSMEnable:
        case bcmPolicerControlTypeTCSMUsePPTC:
            is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(policer_database_handle);
            is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(policer_database_handle);
            if ((is_ingress == FALSE) || (is_global == TRUE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Traffic Class Sensitive Metering is not is not supported for Global or Egress policer database! \n");
            }

            if (dnx_data_meter.meter_db.feature_get(unit, dnx_data_meter_meter_db_tcsm_support) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Traffic Class Sensitive Metering is not is not supported! \n");
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid control type (=%d)\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_control_get(
    int unit,
    int flags,
    int policer_database_handle,
    bcm_policer_control_type_t type,
    uint32 *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_control_get_verify(unit, flags, policer_database_handle, type, arg));

    switch (type)
    {
        case bcmPolicerControlTypeFilterByDpCommand:
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_filter_by_dp_command_get(unit, policer_database_handle, arg));
            break;
        case bcmPolicerControlTypeDpResolutionByDpCommand:
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_dp_resolution_by_dp_command_get(unit, policer_database_handle, arg));
            break;
        case bcmPolicerControlTypeCountBytes:
            SHR_IF_ERR_EXIT(dnx_meter_global_ctr_mode_get(unit, policer_database_handle, arg));
            break;
        case bcmPolicerFairness:
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_fairness_get(unit, policer_database_handle, arg));
            break;
        case bcmPolicerControlTypeTCSMEnable:
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_tcsm_enable_get(unit, policer_database_handle, arg));
            break;
        case bcmPolicerControlTypeTCSMUsePPTC:
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_tcsm_use_pp_tc_get(unit, policer_database_handle, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, " invalid control type (=%d)\n", type);
            break;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify the API bcm_dnx_policer_engine_database_attach
*   API connect an engine to a meter database. this function verify its parameters
* \param [in] unit - unit id
* \param [in] flags - none
* \param [in] engine - holds the core_id, engine_id and section of memory to attach
* \param [in] config  - attach configuration - which database to attach and for which pbjects stat ids.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_engine_database_attach_verify(
    int unit,
    int flags,
    bcm_policer_engine_t * engine,
    bcm_policer_database_attach_config_t * config)
{
    int core_idx;
    int single_bucket_engine;
    int is_ingress, database_id;
    uint32 hw_bank_id, is_single_bucket;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(engine, _SHR_E_PARAM, "engine");
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags=%d, expected 0\n", flags);
    }

    /** check for valid core - BCM_CORE_ALL allowed */
    DNXCMN_CORE_VALIDATE(unit, engine->core_id, TRUE);

    /** verify the engine id is valid */
    if (engine->engine_id >= dnx_data_crps.engine.nof_engines_get(unit) || engine->engine_id < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect engine_id=%d, possible values 0 till %d  \n", engine->engine_id,
                     (dnx_data_crps.engine.nof_engines_get(unit) - 1));
    }

    /** check policer_handle is a valid macro and verify its paramters */
    SHR_IF_ERR_EXIT(dnx_policer_database_handle_verify(unit, config->policer_database_handle));

    /** check that section=ALL if double mode */
    DNXCMN_CORES_ITER(unit, _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(config->policer_database_handle), core_idx)
    {
        if (DNX_CRPS_METER_SHMEM_IS_METER_SINGLE_BUCKET_ENGINE(unit, engine->engine_id) == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_section_verify(unit, core_idx,
                                                            _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET
                                                            (config->policer_database_handle),
                                                            _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET
                                                            (config->policer_database_handle), engine->section));
        }
    }

    /** check that engine core and database core is the same */
    if (_SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(config->policer_database_handle) != engine->core_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "missmatch between core value. policer_database_handle[core]=%d, engine.core=%d \n",
                     _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(config->policer_database_handle), engine->core_id);
    }
    /** check object_stat_pointer_base resolution */
    if (config->object_stat_pointer_base % dnx_data_meter.mem_mgmt.object_stat_pointer_base_resolution_get(unit) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "object_stat_pointer_base=%d, must be 16K resolution \n",
                     config->object_stat_pointer_base);
    }

    /** for small engines, base must be zero */
    single_bucket_engine = DNX_CRPS_METER_SHMEM_IS_METER_SINGLE_BUCKET_ENGINE(unit, engine->engine_id);
    if (single_bucket_engine == TRUE && config->object_stat_pointer_base != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "For engine_id=%d, object_stat_pointer_base must be zero\n", engine->engine_id);
    }
    /** only database 0 can be used for single bucket engine. HW limitation only */
    if ((single_bucket_engine == TRUE)
        && (_SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(config->policer_database_handle) != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "For engine_id=%d, database_id must be zero\n", engine->engine_id);
    }
    DNXCMN_CORES_ITER(unit, _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(config->policer_database_handle), core_idx)
    {
        /** small engines can be used only for single bucket and not
         *  for dual */
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.is_single_bucket.get
                        (unit, core_idx,
                         _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(config->policer_database_handle),
                         _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(config->policer_database_handle),
                         &is_single_bucket));
        if ((single_bucket_engine == TRUE) && (is_single_bucket == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "engine_id=%d cannot be configured in dual bucket mode\n", engine->engine_id);
        }
    }

    DNXCMN_CORES_ITER(unit, engine->core_id, core_idx)
    {
        /** if it is attached already for meter, can't use it again */
        SHR_IF_ERR_EXIT(dnx_crps_meter_engine_available_verify(unit, core_idx, engine->engine_id,
                                                               (dnx_crps_mgmt_shmem_section_t) engine->section));

        /** verify that the engine range is not overlapping with another engine range */
        if (single_bucket_engine == FALSE)
        {
            is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(config->policer_database_handle);
            database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(config->policer_database_handle);
            SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_get(unit, core_idx, is_ingress, database_id,
                                                             (config->object_stat_pointer_base /
                                                              dnx_data_meter.
                                                              mem_mgmt.ptr_map_table_resolution_get(unit)),
                                                             &hw_bank_id));
            if (hw_bank_id != dnx_data_meter.mem_mgmt.invalid_bank_id_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "overlapping range: object_stat_pointer_base=%d is already in used for other bank (band_id=%d).\n",
                             config->object_stat_pointer_base, hw_bank_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_engine_database_attach(
    int unit,
    int flags,
    bcm_policer_engine_t * engine,
    bcm_policer_database_attach_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_engine_database_attach_verify(unit, flags, engine, config));
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_engine_database_attach(unit, flags, engine, config));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify the API bcm_dnx_policer_engine_database_get
* \param [in] unit - 
* \param [in] flags - 
* \param [in] engine - holds the core_id, engine_id and section of memory to attach
* \param [out] config - configuration to update in the API
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_engine_database_get_verify(
    int unit,
    int flags,
    bcm_policer_engine_t * engine,
    bcm_policer_database_attach_config_t * config)
{
    int used_for_meter, core_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(engine, _SHR_E_PARAM, "engine");
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

   /** check for valid core - BCM_CORE_ALL allowed */
    DNXCMN_CORE_VALIDATE(unit, engine->core_id, TRUE);

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags=%d, expected 0\n", flags);
    }
    core_idx = (engine->core_id == BCM_CORE_ALL) ? 0 : engine->core_id;
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.get(unit, core_idx, engine->engine_id, &used_for_meter));
    /** if it is not attached first, can't detach it */
    if (used_for_meter == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "engine_id=%d, core=%d is not used for meter\n", engine->engine_id, core_idx);
    }
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_engine_database_get(
    int unit,
    int flags,
    bcm_policer_engine_t * engine,
    bcm_policer_database_attach_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_engine_database_get_verify(unit, flags, engine, config));

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_engine_database_get(unit, flags, engine, config));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify the API bcm_dnx_policer_engine_database_attach
* \param [in] unit - 
* \param [in] flags - 
* \param [in] engine - holds the core_id, engine_id and section of memory to attach
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_engine_database_detach_verify(
    int unit,
    int flags,
    bcm_policer_engine_t * engine)
{
    int core_idx;
    int used_for_meter;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(engine, _SHR_E_PARAM, "engine");

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags=%d, expected 0\n", flags);
    }

    /** check for valid core - BCM_CORE_ALL allowed */
    DNXCMN_CORE_VALIDATE(unit, engine->core_id, TRUE);

    DNXCMN_CORES_ITER(unit, engine->core_id, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.get(unit, core_idx, engine->engine_id, &used_for_meter));
        /** if it is not attached first, can't detach it */
        if (used_for_meter == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "detach failed: engine_id=%d, core=%d is not used for meter\n",
                         engine->engine_id, core_idx);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_engine_database_detach(
    int unit,
    int flags,
    bcm_policer_engine_t * engine)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_engine_database_detach_verify(unit, flags, engine));
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_engine_database_detach(unit, flags, engine));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/**
* \brief
*   verify the API bcm_dnx_policer_database_enable_set/get
* \param [in] unit - unit_id
* \param [in] flags - 
* \param [in] policer_database_handle - handler which holds the database parameters (see macro BCM_POLICER_DATABASE_HANDLE_SET)
* \param [in] enable  - enable/disable policer database
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_database_enable_set_get_verify(
    int unit,
    int flags,
    int policer_database_handle,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags=%d, expected 0\n", flags);
    }
    SHR_IF_ERR_EXIT(dnx_policer_database_handle_verify(unit, policer_database_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   handle API bcm_dnx_policer_database_enable_set
* \param [in] unit - unit_id
* \param [in] flags - 
* \param [in] policer_database_handle - handler which holds the database parameters (see macro BCM_POLICER_DATABASE_HANDLE_SET)
* \param [in] enable  - enable/disable policer database
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_policer_database_enable_set(
    int unit,
    int flags,
    int policer_database_handle,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_database_enable_set_get_verify(unit, flags, policer_database_handle, &enable));

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_database_enable_set(unit, flags,
                                                         _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET
                                                         (policer_database_handle),
                                                         _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET
                                                         (policer_database_handle),
                                                         _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET
                                                         (policer_database_handle),
                                                         _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET
                                                         (policer_database_handle), enable));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   handle API bcm_dnx_policer_database_enable_get
* \param [in] unit - unit_id
* \param [in] flags - 
* \param [in] policer_database_handle - handler which holds the database parameters (see macro BCM_POLICER_DATABASE_HANDLE_SET)
* \param [out] enable  - policer database is enabled/disabled
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_policer_database_enable_get(
    int unit,
    int flags,
    int policer_database_handle,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_database_enable_set_get_verify(unit, flags, policer_database_handle, enable));

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_database_enable_get(unit, flags,
                                                         _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET
                                                         (policer_database_handle),
                                                         _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET
                                                         (policer_database_handle),
                                                         _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET
                                                         (policer_database_handle),
                                                         _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET
                                                         (policer_database_handle), enable));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  
* \param [in] unit        - unit id
* \param [in] policer_id  - macro which holds the policer id and the database_handle
* \param [in] generic_meter_flags  - the input flags+indication for relace for generic meter that was called from bcm_policer_set
* \param [in] pol_cfg     - structure with all policer configuration
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_set_verify(
    int unit,
    bcm_policer_t policer_id,
    uint32 generic_meter_flags,
    bcm_policer_config_t * pol_cfg)
{
    int database_handle;
    int is_global;
    int meter_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pol_cfg, _SHR_E_PARAM, "pol_cfg");

    SHR_IF_ERR_EXIT(dnx_policer_id_verify(unit, policer_id));

    database_handle = _SHR_POLICER_ID_DATABASE_HANDLE_GET(policer_id);
    is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(database_handle);

    if (is_global)
    {
        SHR_IF_ERR_EXIT(dnx_policer_global_config_verify(unit, pol_cfg));
    }
    else
    {
        meter_idx = _SHR_POLICER_ID_METER_INDEX_GET(policer_id);
        /** for create API, cascade mode, need to configure 4 meters toghther, therefore, the meter_idx should be %4 */
        if ((meter_idx % DNX_POLICER_CASCADE_GROUP_SIZE != 0) && (!(generic_meter_flags & BCM_POLICER_REPLACE)) &&
            (pol_cfg->mode == bcmPolicerModeCascade || pol_cfg->mode == bcmPolicerModeCoupledCascade))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "meter_idx=%d is invalid for mode=%d\n", meter_idx, pol_cfg->mode);
        }
        if (generic_meter_flags & BCM_POLICER_LARGE_BUCKET_MODE)
        {
            if (generic_meter_flags & BCM_POLICER_REPLACE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "flag BCM_POLICER_LARGE_BUCKET_MODE allowed just when creating new meter \n");
            }
            if (pol_cfg->mode != bcmPolicerModeCascade && pol_cfg->mode != bcmPolicerModeCoupledCascade)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "flag BCM_POLICER_LARGE_BUCKET_MODE allowed just for cascade pol_cfg->mode \n");
            }
        }
        SHR_IF_ERR_EXIT(dnx_policer_generic_config_verify(unit, pol_cfg, database_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_set(
    int unit,
    bcm_policer_t policer_id,
    bcm_policer_config_t * pol_cfg)
{
    int database_handle;
    int meter_idx;
    uint32 flags;
    int is_global;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_set_verify(unit, policer_id, (pol_cfg->flags | BCM_POLICER_REPLACE), pol_cfg));

    database_handle = _SHR_POLICER_ID_DATABASE_HANDLE_GET(policer_id);
    meter_idx = _SHR_POLICER_ID_METER_INDEX_GET(policer_id);
    is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(database_handle);
    flags = pol_cfg->flags;
    /** set is always used as replace. it is used just for generic meter */
    if (is_global == FALSE)
    {
        flags |= BCM_POLICER_REPLACE;
    }

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_policer_set(unit, flags,
                                                 _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle),
                                                 is_global,
                                                 _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(database_handle),
                                                 _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(database_handle),
                                                 meter_idx, pol_cfg));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify bcm_policer_get API
* \param [in] unit        - unit id
* \param [in] policer_id  - macro which holds the policer id and the database_handle
* \param [out] pol_cfg     - structure with all policer configuration
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_get_verify(
    int unit,
    bcm_policer_t policer_id,
    bcm_policer_config_t * pol_cfg)
{
    int database_handle, core_id, is_ingress;
    int engine_id, database_id;
    int used_for_meter;
    uint32 is_single_bucket;
    uint32 nof_banks;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pol_cfg, _SHR_E_PARAM, "pol_cfg");

    SHR_IF_ERR_EXIT(dnx_policer_id_verify(unit, policer_id));

    database_handle = _SHR_POLICER_ID_DATABASE_HANDLE_GET(policer_id);
    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle);
    /** check for valid core - BCM_CORE_ALL allowed */
    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);
    core_id = (core_id == BCM_CORE_ALL) ? 0 : core_id;
    is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(database_handle);
    database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(database_handle);
    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.is_single_bucket.get
                    (unit, core_id, is_ingress, database_id, &is_single_bucket));
    /** for single bucket mode, need to check that the engine is available for meter */
    /** if not single bucket mode, it is verified later */
    if (is_single_bucket)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.nof_generic_banks.get
                        (unit, core_id, is_ingress, database_id, &nof_banks));
        engine_id = is_ingress ? dnx_data_meter.mem_mgmt.ingress_single_bucket_engine_get(unit) :
            dnx_data_meter.mem_mgmt.egress_single_bucket_engine_get(unit);
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.used_for_meter.get(unit, core_id, engine_id, &used_for_meter));
        if ((used_for_meter == FALSE) && (nof_banks == 0))
        {
            SHR_ERR_EXIT
                (_SHR_E_PARAM, "No Engine is attached for the given database, can't get policer configuration \n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_get(
    int unit,
    bcm_policer_t policer_id,
    bcm_policer_config_t * pol_cfg)
{
    int database_handle;
    int meter_idx, core_idx;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_get_verify(unit, policer_id, pol_cfg));

    database_handle = _SHR_POLICER_ID_DATABASE_HANDLE_GET(policer_id);
    meter_idx = _SHR_POLICER_ID_METER_INDEX_GET(policer_id);
    core_idx = (_SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle) == BCM_CORE_ALL) ? 0 :
        _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle);
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_policer_get(unit,
                                                 core_idx,
                                                 _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(database_handle),
                                                 _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(database_handle),
                                                 _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(database_handle),
                                                 meter_idx, pol_cfg));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_create(
    int unit,
    bcm_policer_config_t * pol_cfg,
    bcm_policer_t * policer_id)
{
    int database_handle;
    int meter_idx;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_set_verify(unit, *policer_id, pol_cfg->flags, pol_cfg));

    database_handle = _SHR_POLICER_ID_DATABASE_HANDLE_GET(*policer_id);
    meter_idx = _SHR_POLICER_ID_METER_INDEX_GET(*policer_id);

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_policer_set(unit, pol_cfg->flags,
                                                 _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle),
                                                 _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(database_handle),
                                                 _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(database_handle),
                                                 _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(database_handle),
                                                 meter_idx, pol_cfg));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_destroy(
    int unit,
    bcm_policer_t policer_id)
{
    bcm_policer_config_t pol_cfg;
    int nof_meters = 1, i;
    int meter_idx;
    int database_handle, temp_database_handle;
    int current_policer_id;
    int core_id;
    int database_id, is_ingress;
    uint32 is_single_bucket;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    database_handle = _SHR_POLICER_ID_DATABASE_HANDLE_GET(policer_id);
    meter_idx = _SHR_POLICER_ID_METER_INDEX_GET(policer_id);

    /** get the mode to see how many meter_ids to change */
    /** if core_all, change it to core 0 */
    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle);
    if (core_id == BCM_CORE_ALL)
    {
        _SHR_POLICER_DATABASE_HANDLE_SET(temp_database_handle,
                                         _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(database_handle),
                                         _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(database_handle),
                                         0, _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(database_handle));

        _SHR_POLICER_ID_SET(current_policer_id, temp_database_handle, meter_idx);
    }
    else
    {
        current_policer_id = policer_id;
    }
    SHR_IF_ERR_EXIT(bcm_dnx_policer_get(unit, current_policer_id, &pol_cfg));
    if (pol_cfg.mode == bcmPolicerModeCascade || pol_cfg.mode == bcmPolicerModeCoupledCascade)
    {
         /** 4 meters, first one % 4 = 0 */
        nof_meters = DNX_POLICER_CASCADE_GROUP_SIZE;
        if ((meter_idx % DNX_POLICER_CASCADE_GROUP_SIZE) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "meter_idx=%d is invalid for mode=%d (must be devided by 4)\n",
                         meter_idx, pol_cfg.mode);
        }
    }

    database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(database_handle);
    if (core_id == BCM_CORE_ALL)
    {
        core_id = 0;
    }
    is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(database_handle);
    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.is_single_bucket.get
                    (unit, core_id, is_ingress, database_id, &is_single_bucket));
    SHR_IF_ERR_EXIT(dnx_policer_default(unit, &pol_cfg, is_single_bucket));

    /** modify all relevant meters */
    for (i = 0; i < nof_meters; i++)
    {
        _SHR_POLICER_ID_SET(current_policer_id, database_handle, (meter_idx + i));
        SHR_IF_ERR_EXIT(bcm_dnx_policer_set(unit, current_policer_id, &pol_cfg));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_destroy_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /** DBAL tale clear not support error recovery */
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_destroy_all(unit));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_policer_primary_color_resolution_key_verify(
    int unit,
    bcm_policer_primary_color_resolution_key_t * key)
{
    int i;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, key->core_id, TRUE);

    for (i = 0; i < dnx_data_meter.meter_db.nof_ingress_db_get(unit); i++)
    {
        if (key->color_data[i].policer_color > bcmPolicerColorRed)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  key.color_data[%d].policer_color (=%d)\n",
                         i, key->color_data[i].policer_color);
        }
        if (key->color_data[i].policer_other_bucket_has_credits != FALSE &&
            key->color_data[i].policer_other_bucket_has_credits != TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  key->color_data[%d].policer_other_bucket_has_credits (=%d)\n",
                         i, key->color_data[i].policer_other_bucket_has_credits);
        }
    }
    if (key->action < 0 || key->action > DNX_POLICER_MGMT_DP_CMD_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  key.action (=%d)\n", key->action);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify bcm_policer_primary_color_resolution_set API
* \param [in] unit        - unit id
* \param [in] flags        - flags (none)
* \param [in] key  - key for the table to configure
* \param [in] config     - configured table
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_primary_color_resolution_set_verify(
    int unit,
    int flags,
    bcm_policer_primary_color_resolution_key_t * key,
    bcm_policer_primary_color_resolution_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Color decision flags are not supported\n");
    }

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    SHR_IF_ERR_EXIT(dnx_policer_primary_color_resolution_key_verify(unit, key));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify bcm_policer_primary_color_resolution_get API
* \param [in] unit        - unit id
* \param [in] flags        - flags (none)
* \param [in] key  - key for the table to configure
* \param [in] config     - configured table
* \return
*   shr_error_e - Error Type
* \remark
*   * config is not being check here, if wrong values given - dbal will return an error
* \see
*   * None
*/
static shr_error_e
dnx_policer_primary_color_resolution_get_verify(
    int unit,
    int flags,
    bcm_policer_primary_color_resolution_key_t * key,
    bcm_policer_primary_color_resolution_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Color decision flags are not supported\n");
    }

    SHR_NULL_CHECK(key, _SHR_E_PARAM, "key");
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    SHR_IF_ERR_EXIT(dnx_policer_primary_color_resolution_key_verify(unit, key));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_primary_color_resolution_set(
    int unit,
    int flags,
    bcm_policer_primary_color_resolution_key_t * key,
    bcm_policer_primary_color_resolution_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_policer_primary_color_resolution_set_verify(unit, flags, key, config));

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_primary_color_resolution_set(unit, flags, key, config));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_primary_color_resolution_get(
    int unit,
    int flags,
    bcm_policer_primary_color_resolution_key_t * key,
    bcm_policer_primary_color_resolution_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_policer_primary_color_resolution_get_verify(unit, flags, key, config));

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_primary_color_resolution_get(unit, flags, key, config));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify bcm_dnx_policer_color_resolution_set API
* \param [in] unit        - unit id
* \param [in] policer_color_resolution     - configured table
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_color_resolution_set_verify(
    int unit,
    bcm_policer_color_resolution_t * policer_color_resolution)
{
    int legal_flags = BCM_COLOR_RESOLUTION_EGRESS;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(policer_color_resolution, _SHR_E_PARAM, "policer_color_resolution");

    SHR_MASK_VERIFY(policer_color_resolution->flags, legal_flags, _SHR_E_PARAM,
                    "some of the flags are not supported.\n");

    if (policer_color_resolution->policer_action > DNX_POLICER_MGMT_DP_CMD_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  policer_action (=%d)\n", policer_color_resolution->policer_action);
    }
    if (policer_color_resolution->ethernet_policer_color != bcmColorGreen &&
        policer_color_resolution->ethernet_policer_color != bcmColorRed)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  ethernet_policer_color (=%d)\n",
                     policer_color_resolution->ethernet_policer_color);
    }

    /** for egress table, the ingress_color is not relevant */
    if (policer_color_resolution->flags & BCM_COLOR_RESOLUTION_EGRESS)
    {
        if (policer_color_resolution->ingress_color != bcmColorPreserve)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  ingress_color (=%d) for flag=BCM_COLOR_RESOLUTION_EGRESS\n",
                         policer_color_resolution->ingress_color);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   verify bcm_dnx_policer_color_resolution_get API
* \param [in] unit        - unit id
* \param [in] policer_color_resolution     - configured table
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_color_resolution_get_verify(
    int unit,
    bcm_policer_color_resolution_t * policer_color_resolution)
{
    int legal_flags = BCM_COLOR_RESOLUTION_EGRESS;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(policer_color_resolution, _SHR_E_PARAM, "policer_color_resolution");

    SHR_MASK_VERIFY(policer_color_resolution->flags, legal_flags, _SHR_E_PARAM,
                    "some of the flags are not supported.\n");

    if (policer_color_resolution->policer_action > DNX_POLICER_MGMT_DP_CMD_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  policer_action (=%d)\n", policer_color_resolution->policer_action);
    }
    if (policer_color_resolution->ethernet_policer_color != bcmColorGreen &&
        policer_color_resolution->ethernet_policer_color != bcmColorRed)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  ethernet_policer_color (=%d)\n",
                     policer_color_resolution->ethernet_policer_color);
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_color_resolution_set(
    int unit,
    bcm_policer_color_resolution_t * policer_color_resolution)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_policer_color_resolution_set_verify(unit, policer_color_resolution));

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_color_final_resolution_set(unit, policer_color_resolution));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

int
bcm_dnx_policer_color_resolution_get(
    int unit,
    bcm_policer_color_resolution_t * policer_color_resolution)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_policer_color_resolution_get_verify(unit, policer_color_resolution));

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_color_final_resolution_get(unit, policer_color_resolution));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/**
* \brief
*   verify bcm_dnx_policer_stat_get API
* \param [in] unit        - unit id
* \param [in] policer_id     - policer id 
* \param [in] stat - statistic type 
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_policer_stat_get_verify(
    int unit,
    bcm_policer_t policer_id,
    bcm_policer_stat_t stat)
{
    int database_handle, core_id, is_global;
    uint32 is_bytes;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_policer_id_verify(unit, policer_id));

    database_handle = _SHR_POLICER_ID_DATABASE_HANDLE_GET(policer_id);
    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(database_handle);

    /** check for valid core - BCM_CORE_ALL not allowed */
    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    /** check for global */
    is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(database_handle);
    if (is_global == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  database_handle (=%d) - API supports global meter \n", database_handle);
    }

    /** get configuration - counting packets or bytes */
    SHR_IF_ERR_EXIT(dnx_meter_global_ctr_mode_get(unit, database_handle, &is_bytes));

    /** if user selected to get statistic for bytes but the MRPS
     *  is in mode to count packets - error is printed - the mode
     *  can be changed with API bcm_control_set and control type
     *  bcmPolicerControlTypeCountBytes */
    switch (stat)
    {
        case bcmPolicerStatGlobalInRedColorPackets:
        case bcmPolicerStatGlobalInBlackColorPackets:
        case bcmPolicerStatGlobalRejectPackets:
            if (is_bytes == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid stat(=%d) for packets - configured to count bytes \n", stat);
            }
            break;
        case bcmPolicerStatGlobalInRedColorBytes:
        case bcmPolicerStatGlobalInBlackColorBytes:
        case bcmPolicerStatGlobalRejectBytes:
            if (is_bytes == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid stat(=%d) for bytes - configured to count packets \n", stat);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "invalid  stat (=%d) - API supports global meter bcmPolicerStatGlobalInRedColorPackets/Bytes, bcmPolicerStatGlobalInBlackColorPackets/Bytes, bcmPolicerStatGlobalRejectPackets/Bytes \n",
                         stat);
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_stat_get(
    int unit,
    bcm_policer_t policer_id,
    bcm_cos_t int_pri,
    bcm_policer_stat_t stat,
    uint64 *value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_policer_stat_get_verify(unit, policer_id, stat));
    SHR_IF_ERR_EXIT(dnx_meter_global_stat_get(unit, policer_id, stat, value));
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_traverse(
    int unit,
    bcm_policer_traverse_cb cb,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb");
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_traverse(unit, cb, user_data));
exit:
    SHR_FUNC_EXIT;
}
