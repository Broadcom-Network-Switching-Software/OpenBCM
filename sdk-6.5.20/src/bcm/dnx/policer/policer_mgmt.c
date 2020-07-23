/** \file policer_mgmt.c
 * 
 *  policer module gather all functionality that related to the policer/meter.
 *  policer_mgmt.c gather the managements policer functionality for DNX.
 *  It is external policer file . (it have interfaces with other modules)
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
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/policer.h>
#include <shared/policer.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include "meter_global.h"
#include <bcm_int/dnx/policer/policer_mgmt.h>
#include "meter_utils.h"
#include "meter_generic.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/port_ingr_reassembly.h>
#include <include/bcm_int/dnx/cosq/ingress/cosq_ingress.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/meter_access.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */
/** convert defines*/
    /** policer utilities */
#define DNX_POLICER_TRANSLATE_KBITS_TO_PACKETS(__kbits) \
        (__kbits*DNX_METER_UTILS_KBITS_TO_BYTES/DNX_METER_UTILS_PACKETS_TO_BYTES_FACTOR)

#define DNX_POLICER_TRANSLATE_PACKETS_TO_KBITS(__packets) \
        (__packets*DNX_METER_UTILS_PACKETS_TO_BYTES_FACTOR/DNX_METER_UTILS_KBITS_TO_BYTES)

#define DNX_POLICER_TRANSLATE_PACKETS_TO_BYTES(__packets) \
        (__packets*DNX_METER_UTILS_PACKETS_TO_BYTES_FACTOR)

#define DNX_POLICER_TRANSLATE_BYTES_TO_PACKETS(__bytes) \
        (__bytes/DNX_METER_UTILS_PACKETS_TO_BYTES_FACTOR)

#define DNX_POLICER_MGMT_PROFILE_MAX_NUMBER_OF_RATES (24)

#define DNX_POLICER_INGRESS_COMPENSATION_DELTA_VALUE_MIN (-128)
#define DNX_POLICER_INGRESS_COMPENSATION_DELTA_VALUE_MAX (127)
#define DNX_POLICER_EIGHT_BITS (8)
/*
 * }
 */

/** see .h file */
shr_error_e
dnx_policer_default(
    int unit,
    bcm_policer_config_t * pol_cfg,
    uint32 is_single_bucket)
{
    dnx_policer_profile_limits_info_t profile_limit_info;
    SHR_FUNC_INIT_VARS(unit);
    bcm_policer_config_t_init(pol_cfg);
    pol_cfg->flags = 0;

    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.get(unit, &profile_limit_info));
    /** this mode and configuration is not valid for global meter, because global meter is bcmPolicerModeCommitted.*/
    /** but we use it anyway also for global meter,  in order to have one default configuration.*/
    /** global meter do not refer to the parameters that are not relevant, so it is not a problem */
    pol_cfg->ckbits_sec = profile_limit_info.max_rate;
    pol_cfg->max_ckbits_sec = profile_limit_info.max_rate;
    pol_cfg->ckbits_burst = DNX_POLICER_BYTES_TO_KBITS(profile_limit_info.max_burst_normal_mode);
    if (is_single_bucket == TRUE)
    {
        pol_cfg->mode = bcmPolicerModeCommitted;
        pol_cfg->pkbits_sec = 0;
        pol_cfg->max_pkbits_sec = 0;
        pol_cfg->pkbits_burst = 0;
    }
    else
    {
        pol_cfg->mode = bcmPolicerModeCoupledTrTcmDs;
        pol_cfg->pkbits_sec = profile_limit_info.max_rate;
        pol_cfg->max_pkbits_sec = profile_limit_info.max_rate;
        pol_cfg->pkbits_burst = DNX_POLICER_BYTES_TO_KBITS(profile_limit_info.max_burst_normal_mode);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_correct_config_rates(
    int unit,
    bcm_policer_config_t * pol_cfg,
    int is_packets_to_kbits)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_packets_to_kbits)
    {
        pol_cfg->ckbits_burst = DNX_POLICER_TRANSLATE_PACKETS_TO_KBITS(pol_cfg->ckbits_burst);
        pol_cfg->ckbits_sec = DNX_POLICER_TRANSLATE_PACKETS_TO_KBITS(pol_cfg->ckbits_sec);
        pol_cfg->pkbits_sec = DNX_POLICER_TRANSLATE_PACKETS_TO_KBITS(pol_cfg->pkbits_sec);
        pol_cfg->pkbits_burst = DNX_POLICER_TRANSLATE_PACKETS_TO_KBITS(pol_cfg->pkbits_burst);
        /** for the max_rates, 0xFFFFFFFF is an 'unlimited' value so it cannot be changed*/
        pol_cfg->max_ckbits_sec = (pol_cfg->max_ckbits_sec == DNX_METER_UTILS_MAX_RATE_UNLIMITED) ?
            pol_cfg->max_ckbits_sec : DNX_POLICER_TRANSLATE_PACKETS_TO_KBITS(pol_cfg->max_ckbits_sec);

        pol_cfg->max_pkbits_sec = (pol_cfg->max_pkbits_sec == DNX_METER_UTILS_MAX_RATE_UNLIMITED) ?
            pol_cfg->max_pkbits_sec : DNX_POLICER_TRANSLATE_PACKETS_TO_KBITS(pol_cfg->max_pkbits_sec);
    }
    else
    {
        pol_cfg->ckbits_burst = DNX_POLICER_TRANSLATE_KBITS_TO_PACKETS(pol_cfg->ckbits_burst);
        pol_cfg->ckbits_sec = DNX_POLICER_TRANSLATE_KBITS_TO_PACKETS(pol_cfg->ckbits_sec);
        pol_cfg->pkbits_sec = DNX_POLICER_TRANSLATE_KBITS_TO_PACKETS(pol_cfg->pkbits_sec);
        pol_cfg->pkbits_burst = DNX_POLICER_TRANSLATE_KBITS_TO_PACKETS(pol_cfg->pkbits_burst);
        /** for the max_rates, 0xFFFFFFFF is an 'unlimited' value so it cannot be changed*/
        pol_cfg->max_ckbits_sec = (pol_cfg->max_ckbits_sec == DNX_METER_UTILS_MAX_RATE_UNLIMITED) ?
            pol_cfg->max_ckbits_sec : DNX_POLICER_TRANSLATE_KBITS_TO_PACKETS(pol_cfg->max_ckbits_sec);

        pol_cfg->max_pkbits_sec = (pol_cfg->max_pkbits_sec == DNX_METER_UTILS_MAX_RATE_UNLIMITED) ?
            pol_cfg->max_pkbits_sec : DNX_POLICER_TRANSLATE_KBITS_TO_PACKETS(pol_cfg->max_pkbits_sec);
    }

/**exit:*/
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      convert internal meter profile info to API policer configuration structure
 * \param [in] unit -unit id
 * \param [in] core_id -core_id
 * \param [in] meter_id -meter_id
 * \param [in] is_single_bucket -is_single_bucket (no yellow bucket)
 * \param [in] meter_profile_info -strucutre which hold the internal meter configuration
 * \param [in] global_sharing -if true, cascade mode
 * \param [in] profile_id -profile_id of the meter
 * \param [out] pol_cfg -strucutre which hold the policer configuration
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_policer_from_meter_inter_info_convert(
    int unit,
    int core_id,
    int meter_id,
    int is_single_bucket,
    dnx_meter_profile_info_t * meter_profile_info,
    int global_sharing,
    int profile_id,
    bcm_policer_config_t * pol_cfg)
{
    dnx_policer_profile_limits_info_t profile_limit_info;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.get(unit, &profile_limit_info));

    flags = pol_cfg->flags;
    bcm_policer_config_t_init(pol_cfg);
    pol_cfg->flags = flags;

    /** commited rate set */
    /** zero, means disable credits, yet set to minimum cir */
    if (meter_profile_info->disable_cir == TRUE)
    {
        pol_cfg->ckbits_sec = 0;
    }
    else
    {
        pol_cfg->ckbits_sec = meter_profile_info->cir;
    }
    if (meter_profile_info->is_sharing_enabled)
    {
        pol_cfg->max_ckbits_sec = meter_profile_info->max_cir;
    }

    /** excess/peak rate set */
    /** zero, means disable credits, yet set to minimum cir */
    if (meter_profile_info->disable_eir == TRUE)
    {
        pol_cfg->pkbits_sec = 0;
    }
    else
    {
        pol_cfg->pkbits_sec = meter_profile_info->eir;
    }

    pol_cfg->pkbits_burst = DNX_POLICER_BYTES_TO_KBITS(meter_profile_info->ebs); /** bytes to kbits */
    pol_cfg->ckbits_burst = DNX_POLICER_BYTES_TO_KBITS(meter_profile_info->cbs); /** bytes to kbits */
    if (meter_profile_info->color_mode == DNX_MTR_COLOR_MODE_BLIND)
    {
        pol_cfg->flags |= BCM_POLICER_COLOR_BLIND;
    }

    /** max_eir is valid either when eir is enabled or when coupling is set or when coupleCascade mode */
    if ((meter_profile_info->disable_eir == FALSE) || (meter_profile_info->is_coupling_enabled == TRUE) ||
        ((meter_profile_info->is_sharing_enabled == TRUE)))
    {
        pol_cfg->max_pkbits_sec = meter_profile_info->max_eir;
    }

    /*
     * get the mode of the policer
     */
    if (is_single_bucket)
    {
        pol_cfg->mode = bcmPolicerModeCommitted;
    }
    else
    {
        /** if no yellow rate and no yellow burst*/
        if (pol_cfg->pkbits_sec == 0 && meter_profile_info->ebs <= profile_limit_info.min_burst)
        {
            pol_cfg->mode = bcmPolicerModeCommitted;
        }
        /** if no yellow rate but coupling enabled */
        else if (pol_cfg->pkbits_sec == 0 && meter_profile_info->is_coupling_enabled == TRUE)
        {
            pol_cfg->mode = bcmPolicerModeSrTcm;
        }
        /** if coupling enabled (and yellow rate)*/
        else if (meter_profile_info->is_coupling_enabled == TRUE)
        {
            pol_cfg->mode = bcmPolicerModeCoupledTrTcmDs;
        }
        /** if yellow rate but coupling disabled */
        else if (pol_cfg->pkbits_sec >= profile_limit_info.min_rate)
        {
            pol_cfg->mode = bcmPolicerModeTrTcmDs;
        }
        else
        {
            /** similar to the first case: this can be if pkbits_sec=0, but burst mode is in large bucket mode.
                            so, the  ebs is not min_burst (althought it should have been), but, large bucket mode force it to be bigger */
            pol_cfg->mode = bcmPolicerModeCommitted;
        }
        if ((meter_profile_info->is_sharing_enabled == TRUE))
        { /** cascade */
            if (global_sharing)
            {
                pol_cfg->mode = bcmPolicerModeCoupledCascade;
            }
            else
            {
                pol_cfg->mode = bcmPolicerModeCascade;
            }
        }
    }

    if (meter_profile_info->is_pkt_truncate)
    {
        pol_cfg->flags |= BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE;
    }

    /*
     * In packet mode the values are artificially divided by _DPP_POLICER_PACKET_MODE_CORRECTION and so must be scaled
     * back to user values
     */
    if (meter_profile_info->is_packet_mode)
    {
        pol_cfg->flags |= BCM_POLICER_MODE_PACKETS;
        SHR_IF_ERR_EXIT(dnx_policer_correct_config_rates(unit, pol_cfg, FALSE));
    }
    if (meter_profile_info->is_large_bucket_mode)
    {
        pol_cfg->flags |= BCM_POLICER_LARGE_BUCKET_MODE;
    }
    pol_cfg->color_resolve_profile = meter_profile_info->profile_data;

    if (pol_cfg->flags & BCM_POLICER_REPLACE_SHARED)
    {
        pol_cfg->entropy_id = profile_id;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*      convert policer profile  from api strucutre to internal meter structure
* \param [in] unit -unit id
* \param [in] pol_cfg -strucutre which hold the api configuration
* \param [out] meter_profile_info -strucutre which hold the internal meter configuration
* \return
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_policer_to_meter_inter_info(
    int unit,
    bcm_policer_config_t * pol_cfg,
    dnx_meter_profile_info_t * meter_profile_info)
{
    /** Copy the config to a temp variable since it may change (times 64) if PACKET_MODE is set. */
    bcm_policer_config_t pol_cfg_internal = *pol_cfg;
    uint32 min_rate, min_burst;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.min_rate.get(unit, &min_rate));
    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.min_burst.get(unit, &min_burst));

    /** clear the internal structure */
    sal_memset(meter_profile_info, 0x0, sizeof(dnx_meter_profile_info_t));

     /** Set the packet mode and change the rates from kbits per sec to packets (with the defined resolution) per sec */
    if (pol_cfg_internal.flags & BCM_POLICER_MODE_PACKETS)
    {
        /** convert the parameters to kbits, (user called it in packet per second). */
        SHR_IF_ERR_EXIT(dnx_policer_correct_config_rates(unit, &pol_cfg_internal, TRUE));
    }

    if (pol_cfg_internal.mode == bcmPolicerModeCascade || pol_cfg_internal.mode == bcmPolicerModeCoupledCascade)
    {
        meter_profile_info->is_sharing_enabled = TRUE;
    }

    /** commited rate set */
    /** zero, means disable credits, yet set to minimum cir */
    if (pol_cfg_internal.ckbits_sec == 0)
    {
        meter_profile_info->cir = 0;
        meter_profile_info->disable_cir = TRUE;
    }
    else
    {
        meter_profile_info->cir = pol_cfg_internal.ckbits_sec;
        meter_profile_info->disable_cir = FALSE;
    }

    /** commited max rate, applicable only in hierarchical modes*/
    /** max_cir can exist also when disable_cir = TRUE since it will get extra credits from higher buckets */
    if (meter_profile_info->is_sharing_enabled)
    {
        meter_profile_info->max_cir = pol_cfg_internal.max_ckbits_sec;
    }
    else
    {
        meter_profile_info->max_cir = meter_profile_info->cir;
    }

    /** coupling depends on policer mode */
    if (pol_cfg_internal.mode == bcmPolicerModeSrTcm || pol_cfg_internal.mode == bcmPolicerModeCoupledTrTcmDs)
    {
        meter_profile_info->is_coupling_enabled = TRUE;
    }
    else
    {
        meter_profile_info->is_coupling_enabled = FALSE;
    }

    /** excess/peak rate set */
    /** zero, means disable credits, yet set to minimum cir */
    if (pol_cfg_internal.pkbits_sec == 0)
    {
        meter_profile_info->eir = 0;
        meter_profile_info->disable_eir = TRUE;
    }
    else
    {
        meter_profile_info->disable_eir = FALSE;
        meter_profile_info->eir = pol_cfg_internal.pkbits_sec;
    }

    /** max_eir can exist also when disable_eir = TRUE, */
    /** because if coupling/sharing is enabled the bucket gets extra credits from other buckets */
    if ((meter_profile_info->disable_eir == FALSE) || (meter_profile_info->is_coupling_enabled == TRUE) ||
        (pol_cfg->mode == bcmPolicerModeCoupledCascade) || (pol_cfg->mode == bcmPolicerModeCascade))
    {
        meter_profile_info->max_eir = pol_cfg_internal.max_pkbits_sec;
    }
    else
    {
        meter_profile_info->max_eir = meter_profile_info->eir;
    }

    meter_profile_info->ebs = DNX_POLICER_KBITS_TO_BYTES(pol_cfg_internal.pkbits_burst); /** kbits to bytes*/
    meter_profile_info->cbs = DNX_POLICER_KBITS_TO_BYTES(pol_cfg_internal.ckbits_burst); /** kbits to bytes*/
    meter_profile_info->color_mode = (pol_cfg_internal.flags & BCM_POLICER_COLOR_BLIND) ?
        DNX_MTR_COLOR_MODE_BLIND : DNX_MTR_COLOR_MODE_AWARE;
    meter_profile_info->is_pkt_truncate = (pol_cfg_internal.flags & BCM_POLICER_PKT_ADJ_HEADER_TRUNCATE) ? TRUE : FALSE;

    meter_profile_info->is_packet_mode = (pol_cfg_internal.flags & BCM_POLICER_MODE_PACKETS) ? TRUE : FALSE;

    /** set by default the rev_exp to be invalid. For cascade mode, it will be modify later on.
            For all other caes it will stay invalid and will be calculated in the profile set low level functions  */
    meter_profile_info->ir_rev_exp = DNX_METER_UTILS_INVALID_IR_REV_EXP;

    meter_profile_info->profile_data = pol_cfg_internal.color_resolve_profile;
    /** if force flag set, force large bucket mode, otherwise, calc if the bucket mode */
    if ((pol_cfg_internal.flags & BCM_POLICER_LARGE_BUCKET_MODE))
    {
        meter_profile_info->is_large_bucket_mode = TRUE;
    }
    else
    {
        SHR_IF_ERR_EXIT(meter_utils_large_bucket_mode_get(unit, meter_profile_info->cbs, meter_profile_info->ebs,
                                                          &meter_profile_info->is_large_bucket_mode));
    }

    /** Profile ID, used in order to allow two profiles with the same data */
    meter_profile_info->entropy_id = pol_cfg_internal.entropy_id;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_policer_tm_profile_init(
    int unit,
    dnx_meter_profile_info_t * profile_info)
{
    dnx_algo_template_create_data_t data;
    dbal_table_field_info_t field_info;
    int nof_profiles, nof_banks;
    int bank_size;
    uint32 core_id;
    SHR_FUNC_INIT_VARS(unit);

    /** get data on the profiles size etc.. from HW dbal tables */
    /** note - ingress and egress has the same hw configuration in HW. we take the data from ingress, but it belong also to egress */
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_PROFILE_CONFIG, DBAL_FIELD_PROFILE_INDEX,
                                               TRUE, 0, 0, &field_info));
    nof_profiles = field_info.max_value + 1;
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_CRPS_METER_BIG_ENGINE_SHARED_MEM,
                                               DBAL_FIELD_BANK_ID, TRUE, 0, 0, &field_info));
    nof_banks = field_info.max_value + 1;
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_PROFILE_SET_FOR_BIG_ENGINE,
                                               DBAL_FIELD_METER_ID, TRUE, 0, 0, &field_info));
    bank_size = field_info.max_value + 1;

    /*
     * Add all entities to template init id with given data
     */
    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.first_profile = 0;
    data.nof_profiles = nof_profiles;
    data.max_references = nof_banks * bank_size;
    data.default_profile = DNX_METER_UTILS_DEFAULT_PROFILE_ID;
    data.data_size = sizeof(dnx_meter_profile_info_t);
    data.default_data = profile_info;

    /** create 3 ingress template managers. one for each database */
    sal_strncpy(data.name, DNX_POLICER_INGRESS_DB0_TEMPLATE_NAME, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_ingress_db0.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_ingress_db0.create(unit, core_id, &data, NULL));
    }

    sal_strncpy(data.name, DNX_POLICER_INGRESS_DB1_TEMPLATE_NAME, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_ingress_db1.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_ingress_db1.create(unit, core_id, &data, NULL));
    }

    sal_strncpy(data.name, DNX_POLICER_INGRESS_DB2_TEMPLATE_NAME, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_ingress_db2.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_ingress_db2.create(unit, core_id, &data, NULL));
    }

    /** create 2 egress template managers. one for each database */
    sal_strncpy(data.name, DNX_POLICER_EGRESS_DB0_TEMPLATE_NAME, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_egress_db0.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_egress_db0.create(unit, core_id, &data, NULL));
    }

    sal_strncpy(data.name, DNX_POLICER_EGRESS_DB1_TEMPLATE_NAME, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_egress_db1.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_egress_db1.create(unit, core_id, &data, NULL));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  init sw state database
*
* \par DIRECT_INPUT:
*   \param [in] unit - Unit ID
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
static shr_error_e
dnx_policer_mgmt_db_init(
    int unit)
{
    uint8 is_init;
    int nof_cores, core_idx;
    SHR_FUNC_INIT_VARS(unit);

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    SHR_IF_ERR_EXIT(dnx_meter_db.is_init(unit, &is_init));
     /** if init wasn't made => init */
    if (is_init == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.init(unit));
        SHR_IF_ERR_EXIT(dnx_meter_db.global_meter_enabled.alloc(unit, nof_cores));
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.alloc(unit, nof_cores));
        for (core_idx = 0; core_idx < nof_cores; core_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.alloc
                            (unit, core_idx, 0, dnx_data_meter.meter_db.nof_egress_db_get(unit)));
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.alloc
                            (unit, core_idx, 1, dnx_data_meter.meter_db.nof_ingress_db_get(unit)));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "dnx_meter_db.init=TRUE \n");
    }

    SHR_IF_ERR_EXIT(dnx_utils_limits_profile_info_init(unit));

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dnx_meter_dynamic_access_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id1;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_DYNAMIC_ACCESS_ENABLE, &entry_handle_id1));
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id1, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id1, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_meter_dynamic_access_enable_get(
    int unit,
    int *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_DYNAMIC_ACCESS_ENABLE, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *) (enable));
     /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_policer_mgmt_tc_metering_enable_set(
    int unit,
    bcm_core_t core_id,
    int database_id,
    int tc,
    uint32 enable)
{
    uint32 entry_handle_id;
    uint32 bit_val;
    uint32 tc_bitmap;
    bcm_core_t core_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG, &entry_handle_id));

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_idx);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);

        /** First get the current bitmap from HW */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TC_ENABLE_BITMAP, INST_SINGLE, &tc_bitmap);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /** For the specified TC set the corresponding bit in the bitmap based on the "enable" parameter*/
        bit_val = (enable == FALSE) ? 0 : 1;
        SHR_IF_ERR_EXIT(utilex_bitstream_set(&tc_bitmap, tc, bit_val));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_ENABLE_BITMAP, INST_SINGLE,
                                     (tc_bitmap) & 0xFF);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

 /** See .h file */
shr_error_e
dnx_policer_init(
    int unit)
{
    bcm_policer_config_t pol_cfg, global_pol_cfg;
    dnx_meter_profile_info_t meter_profile_info, global_meter_profile_info;
    uint32 tc_idx;
    uint32 database_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_db_init(unit));

    /** convert API parameters to internal hw/sw structures */
    SHR_IF_ERR_EXIT(dnx_policer_default(unit, &global_pol_cfg, TRUE));
    SHR_IF_ERR_EXIT(dnx_policer_default(unit, &pol_cfg, FALSE));
    SHR_IF_ERR_EXIT(dnx_policer_to_meter_inter_info(unit, &pol_cfg, &meter_profile_info));
    SHR_IF_ERR_EXIT(dnx_policer_to_meter_inter_info(unit, &global_pol_cfg, &global_meter_profile_info));

    SHR_IF_ERR_EXIT(dnx_meter_dynamic_access_enable_set(unit, TRUE));
    SHR_IF_ERR_EXIT(dnx_meter_global_init(unit, &global_meter_profile_info));
    SHR_IF_ERR_EXIT(dnx_meter_generic_init(unit));
    /** init template manager - one for each meter database (ingress and egress) */
    SHR_IF_ERR_EXIT(dnx_policer_tm_profile_init(unit, &meter_profile_info));

    if (dnx_data_meter.meter_db.feature_get(unit, dnx_data_meter_meter_db_tcsm_support) == TRUE)
    {
        /** Enable meering for all TCs on all Ingress databases */
        for (database_id = 0; database_id < dnx_data_meter.meter_db.nof_ingress_db_get(unit); database_id++)
        {
            /** For each TC set the enable bit in the bitmap */
            for (tc_idx = 0; tc_idx < BCM_COS_COUNT; tc_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_policer_mgmt_tc_metering_enable_set(unit, BCM_CORE_ALL, database_id, tc_idx, TRUE));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_policer_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit
     */

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_destroy_all(
    int unit)
{
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    /** instead of destroying each meter, one by one, we reset all sw template manager for all databases.
        In addition, we clear the entire HW tables */
    /** after this process, all meters, from all databases point to profile 0, whic hold the default profile */

    /** reset template manager for each database */
    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_ingress_db0.clear(unit, core_idx));
        SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_ingress_db1.clear(unit, core_idx));
        SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_ingress_db2.clear(unit, core_idx));
        SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_egress_db0.clear(unit, core_idx));
        SHR_IF_ERR_EXIT(dnx_meter_db.policer_templates.policer_egress_db1.clear(unit, core_idx));
    }
    /** hw - clear the entire hw tables (ext_a and ext_c memories) that are used for meters*/
    SHR_IF_ERR_EXIT(dnx_meter_generic_clear_all(unit));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
void
dnx_policer_mgmt_tm_meter_profile_print_cb(
    int unit,
    const void *data)
{
    dnx_meter_profile_info_t *profile_info = (dnx_meter_profile_info_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "meter profile info:", NULL, NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "disable_cir", profile_info->disable_cir, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "disable_eir", profile_info->disable_eir, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "cir", profile_info->cir, NULL, "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "eir", profile_info->eir, NULL, "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "max_cir", profile_info->max_cir, NULL, "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "max_eir", profile_info->max_eir, NULL, "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "cbs", profile_info->cbs, NULL, "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "ebs", profile_info->ebs, NULL, "0x%x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "color_mode", profile_info->color_mode, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "is_coupling_enabled",
                                   profile_info->is_coupling_enabled, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "is_sharing_enabled",
                                   profile_info->is_sharing_enabled, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "is_packet_mode",
                                   profile_info->is_packet_mode, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "is_pkt_truncate",
                                   profile_info->is_pkt_truncate, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "ir_rev_exp",
                                   profile_info->ir_rev_exp, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "profile_data",
                                   profile_info->profile_data, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "is_large_bucket_mode",
                                   profile_info->is_large_bucket_mode, NULL, "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "entropy_id",
                                   profile_info->entropy_id, NULL, "%d");

    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_expansion_groups_set(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_policer_expansion_group_t * expansion_group)
{
    uint32 entry_handle_id1;
    int group_idx;
    uint32 filter_bitmap = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle before we start iterations */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_FWD_TYPE_GROUP_MAP, &entry_handle_id1));

    if ((flags == 0) || _SHR_IS_FLAG_SET(flags, BCM_POLICER_EXPANSION_GROUPS_FWD))
    {
        /** Configure the L2-FWD expansion groups */
        for (group_idx = bcmPolicerFwdTypeUc; group_idx < bcmPolicerFwdTypeMax; group_idx++)
        {
            /** Taking a handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_METER_FWD_TYPE_GROUP_MAP, entry_handle_id1));
            /** Setting key fields */
            dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_FWD_TYPE, group_idx);

            /** Setting value fields */
            dbal_entry_value_field32_set(unit, entry_handle_id1, DBAL_FIELD_OFFSET, INST_SINGLE,
                                         expansion_group->config[group_idx].offset);

            if (expansion_group->config[group_idx].valid)
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set(&filter_bitmap, group_idx, 1));
            }

            /** Performing the action */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id1, DBAL_COMMIT));
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_GENERAL_CONFIG, &entry_handle_id1));
        dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_CORE_ID, core_id);
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id1, DBAL_FIELD_FWD_TYPE_FILTER_BITMAP, INST_SINGLE,
                                     (~filter_bitmap) & 0xFF);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id1, DBAL_COMMIT));
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_POLICER_EXPANSION_GROUPS_TC))
    {
        /** Configure the TC expansion groups */
        for (group_idx = 0; group_idx < BCM_COS_COUNT; group_idx++)
        {
            /** Taking a handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_METER_TC_GROUP_MAP, entry_handle_id1));
            /** Setting key fields */
            dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_TC, group_idx);

            /** Setting value fields */
            dbal_entry_value_field32_set(unit, entry_handle_id1, DBAL_FIELD_OFFSET, INST_SINGLE,
                                         expansion_group->tc_offset[group_idx]);

            /** Performing the action */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id1, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_policer_mgmt_expansion_groups_get(
    int unit,
    int flags,
    bcm_core_t core_id,
    bcm_policer_expansion_group_t * expansion_group)
{
    uint32 entry_handle_id1;
    int group_idx;
    uint32 filter_bitmap = 0, offset, valid;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle before we start iterations */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_FWD_TYPE_GROUP_MAP, &entry_handle_id1));

    if ((flags == 0) || _SHR_IS_FLAG_SET(flags, BCM_POLICER_EXPANSION_GROUPS_FWD))
    {

        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_METER_GLOBAL_GENERAL_CONFIG, entry_handle_id1));
        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_CORE_ID, core_id);
        /** Setting pointers value to receive the fields */
        dbal_value_field32_request(unit, entry_handle_id1, DBAL_FIELD_FWD_TYPE_FILTER_BITMAP, INST_SINGLE,
                                   &filter_bitmap);
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id1, DBAL_COMMIT));

        filter_bitmap = (~filter_bitmap);

        /** get the goffset of each type and update the returned structure */
        for (group_idx = bcmPolicerFwdTypeUc; group_idx < bcmPolicerFwdTypeMax; group_idx++)
        {
            /** Taking a handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_METER_FWD_TYPE_GROUP_MAP, entry_handle_id1));
            /** Setting key fields */
            dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_FWD_TYPE, group_idx);

            /** Setting pointers value to receive the fields */
            dbal_value_field32_request(unit, entry_handle_id1, DBAL_FIELD_OFFSET, INST_SINGLE, &offset);

            /** Performing the action */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id1, DBAL_COMMIT));

            expansion_group->config[group_idx].offset = offset;

            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(&filter_bitmap, group_idx, 1, &valid));
            expansion_group->config[group_idx].valid = valid;
        }
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_POLICER_EXPANSION_GROUPS_TC))
    {
        /** Configure the TC expansion groups */
        for (group_idx = 0; group_idx < BCM_COS_COUNT; group_idx++)
        {
            /** Taking a handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_METER_TC_GROUP_MAP, entry_handle_id1));
            /** Setting key fields */
            dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_TC, group_idx);

            /** Getting value fields */
            dbal_value_field32_request(unit, entry_handle_id1, DBAL_FIELD_OFFSET, INST_SINGLE, &offset);

            /** Performing the action */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id1, DBAL_COMMIT));

            expansion_group->tc_offset[group_idx] = offset;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_database_enable_set(
    int unit,
    int flags,
    int core_id,
    int database_id,
    int is_ingress,
    int is_global,
    int enable)
{
    uint32 entry_handle_id, core_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    if (is_global)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_GENERAL_CONFIG, &entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                          (is_ingress ? DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG :
                                           DBAL_TABLE_METER_EGRESS_DB_GENERAL_CONFIG), &entry_handle_id));
    }

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    if (is_global == FALSE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    }
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_INIT, INST_SINGLE, (enable ? FALSE : TRUE));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** update sw state */
    DNXCMN_CORES_ITER(unit, core_id, core_index)
    {
        if (is_global)
        {
            SHR_IF_ERR_EXIT(dnx_meter_db.global_meter_enabled.set(unit, core_index, enable));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.enable.set
                            (unit, core_index, is_ingress, database_id, enable));

        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_database_enable_get(
    int unit,
    int flags,
    int core_id,
    int database_id,
    int is_ingress,
    int is_global,
    int *enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    if (is_global)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_GENERAL_CONFIG, &entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                          (is_ingress ? DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG :
                                           DBAL_TABLE_METER_EGRESS_DB_GENERAL_CONFIG), &entry_handle_id));
    }

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    if (is_global == FALSE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    }

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *) enable);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * calculate the ir_rev_exp for sharing mode. Since the rev exp must be the same for all buckets,
 * it should be calculate according to all rates of the MEF (4 meters)
 * \param [in] unit -unit id
 * \param [in] nof_profiles -how many configurations to consider when calc the rev exp. (size of array)
 * \param [in] meter_profile_info -array of meter profiles configurations
 * \param [out] ir_reverse_exponent_2 -calculated reverse exponent
 * \return
 *      shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_policer_mgmt_rev_exp_for_sharing_enable_mode_calc(
    int unit,
    int nof_profiles,
    dnx_meter_profile_info_t * meter_profile_info,
    uint32 *ir_reverse_exponent_2)
{
    int i;
    uint32 rates[DNX_POLICER_MGMT_PROFILE_MAX_NUMBER_OF_RATES];
    uint32 rates_idx, nof_rates = -1;
    uint32 max_cir_for_calc, max_eir_for_calc;
    uint32 min_rate;
    int is_large_bucket_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_db.profile_limits_info.min_rate.get(unit, &min_rate));

    rates_idx = 0;
    for (i = 0; i < nof_profiles; i++)
    {
        is_large_bucket_mode = meter_profile_info[i].is_large_bucket_mode;

        /** at least one is used*/
        if (FALSE == meter_profile_info[i].disable_cir)
        {
            /** CIR used*/
            /** if large_bucket_mode_exp_add_get>32,  */
            /** utilex_power_of_2 will return zero and will cause zero division, but this is not the case */
             /* coverity[Division or modulo by zero :FALSE]  */
            rates[rates_idx] = (is_large_bucket_mode == TRUE) ? UTILEX_DIV_ROUND_UP
                (meter_profile_info[i].cir,
                 utilex_power_of_2(dnx_data_meter.profile.
                                   large_bucket_mode_exp_add_get(unit))) : meter_profile_info[i].cir;
            ++rates_idx;
        }
        if (FALSE == meter_profile_info[i].disable_eir)
        {
            /** EIR used*/
            /** if large_bucket_mode_exp_add_get>32,  */
            /** utilex_power_of_2 will return zero and will cause zero division, but this is not the case */
             /* coverity[Division or modulo by zero :FALSE]  */
            rates[rates_idx] = (is_large_bucket_mode == TRUE) ? UTILEX_DIV_ROUND_UP
                (meter_profile_info[i].eir,
                 utilex_power_of_2(dnx_data_meter.profile.
                                   large_bucket_mode_exp_add_get(unit))) : meter_profile_info[i].eir;
            ++rates_idx;
        }
        if (meter_profile_info[i].max_cir == DNX_METER_UTILS_PROFILE_IR_MAX_UNLIMITED)
        {
            /** max cir unlimited */
            /** max_cir is not used in calculations */
            /** if large_bucket_mode_exp_add_get>32,  */
            /** utilex_power_of_2 will return zero and will cause zero division, but this is not the case */
             /* coverity[Division or modulo by zero :FALSE]  */
            max_cir_for_calc = (is_large_bucket_mode == TRUE) ? UTILEX_DIV_ROUND_UP
                (meter_profile_info[i].cir,
                 utilex_power_of_2(dnx_data_meter.profile.
                                   large_bucket_mode_exp_add_get(unit))) : meter_profile_info[i].cir;
        }
        else
        {
            /** if large_bucket_mode_exp_add_get>32,  */
            /** utilex_power_of_2 will return zero and will cause zero division, but this is not the case */
             /* coverity[Division or modulo by zero :FALSE]  */
            max_cir_for_calc = (is_large_bucket_mode == TRUE) ? UTILEX_DIV_ROUND_UP
                (meter_profile_info[i].max_cir,
                 utilex_power_of_2(dnx_data_meter.profile.
                                   large_bucket_mode_exp_add_get(unit))) : meter_profile_info[i].max_cir;
        }
        if (max_cir_for_calc >= min_rate)
        {
            /** Max-CIR used */
            rates[rates_idx] = max_cir_for_calc;
            ++rates_idx;
        }
        if (meter_profile_info[i].max_eir == DNX_METER_UTILS_PROFILE_IR_MAX_UNLIMITED)
        {
            /** max eir unlimited */
            /** max_eir is not used in calculations */
            /** if large_bucket_mode_exp_add_get>32,  */
            /** utilex_power_of_2 will return zero and will cause zero division, but this is not the case */
             /* coverity[Division or modulo by zero :FALSE]  */
            max_eir_for_calc = (is_large_bucket_mode == TRUE) ? UTILEX_DIV_ROUND_UP
                (meter_profile_info[i].eir,
                 utilex_power_of_2(dnx_data_meter.profile.
                                   large_bucket_mode_exp_add_get(unit))) : meter_profile_info[i].eir;
        }
        else
        {
            /** if large_bucket_mode_exp_add_get>32,  */
            /** utilex_power_of_2 will return zero and will cause zero division, but this is not the case */
             /* coverity[Division or modulo by zero :FALSE]  */
            max_eir_for_calc = (is_large_bucket_mode == TRUE) ? UTILEX_DIV_ROUND_UP
                (meter_profile_info[i].max_eir,
                 utilex_power_of_2(dnx_data_meter.profile.
                                   large_bucket_mode_exp_add_get(unit))) : meter_profile_info[i].max_eir;
        }
        if (max_eir_for_calc >= min_rate)
        {
            /** Max-EIR used */
            rates[rates_idx] = max_eir_for_calc;
            ++rates_idx;
        }
        if (rates_idx == DNX_POLICER_MGMT_PROFILE_MAX_NUMBER_OF_RATES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "rates_idx=%d, out of range \n", rates_idx);
        }
    }
    nof_rates = rates_idx;
    if (nof_rates == 0)
    {
        *ir_reverse_exponent_2 = 0;
    }
    else
    {
        /** Calculate the best rev_exp (with respect to both rate accuracy and bucket update) for: CIR, EIR and max EIR */
        SHR_IF_ERR_EXIT(dnx_meter_utils_rev_exp_optimized_for_bucket_rate
                        (unit, rates, nof_rates, ir_reverse_exponent_2));
    }
exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 * get the min/max rates of the 3 other meters
 * since changing the rate of one of the meters, affects all 4 meters (rev-exp is shared)
 * the function will recalculate the rates of the 3 other meters.
 * \param [in] unit -unit id
 * \param [in] core_id -core_id
 * \param [in] is_ingress -is_ingress
 * \param [in] database_id -database_id
 * \param [in] meter_id -one of the 4 hierarchical_meters. min/max rates will be of the 3 other meters
 * \param [in] meter_profile_info -configuration array of 4 meters. function might modify the rev_exp value for each one of the meters
 * \param [out] reconfigure_hierarchical_meters - indicated if reconfiguration of rev exponent was made.
 * \return
 *      shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_policer_mgmt_hierarchical_meters_rev_exp_set(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_id,
    dnx_meter_profile_info_t * meter_profile_info,
    int *reconfigure_hierarchical_meters)
{
    int i;
    int profile;
    int other_meter_id;
    uint32 other_ir_rev_exp = DNX_METER_UTILS_INVALID_IR_REV_EXP, current_ir_rev_exp;
    dnx_meter_profile_info_t meter_profile_info_array[DNX_POLICER_CASCADE_GROUP_SIZE];
    int nof_ref;
    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < DNX_POLICER_CASCADE_GROUP_SIZE; i++)
    {
        other_meter_id = meter_id - (meter_id % DNX_POLICER_CASCADE_GROUP_SIZE) + i;
        if (other_meter_id == meter_id)
        {
            memcpy(&meter_profile_info_array[i], &meter_profile_info[0], sizeof(dnx_meter_profile_info_t));
        }
        else
        {
            /** get the meters profile id from HW */
            SHR_IF_ERR_EXIT(dnx_meter_generic_profile_id_get
                            (unit, core_id, is_ingress, database_id, other_meter_id, &profile));
            /** get the data from tm */
            SHR_IF_ERR_EXIT(POLICER_MNGR_TM_CB(is_ingress, database_id,
                                               profile_data_get(unit, core_id, profile, &nof_ref,
                                                                &meter_profile_info_array[i])));
            /** it doesn't matter which one of the other profiles we hold the rev_exp, because they all should be the same */
            other_ir_rev_exp = meter_profile_info_array[i].ir_rev_exp;
        }
    }
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_rev_exp_for_sharing_enable_mode_calc
                    (unit, DNX_POLICER_CASCADE_GROUP_SIZE, meter_profile_info_array, &current_ir_rev_exp));
    /** case that rev_exp is different for profiles belong to the same group,
           need to exchange the profile in order to have same rev_exp.
           Here, we just update a flag and the relevant structures. the exchange will be made later*/
    if (other_ir_rev_exp != current_ir_rev_exp)
    {
        *reconfigure_hierarchical_meters = TRUE;

        for (i = 0; i < DNX_POLICER_CASCADE_GROUP_SIZE; i++)
        {
            other_meter_id = meter_id - (meter_id % DNX_POLICER_CASCADE_GROUP_SIZE) + i;
            if (other_meter_id == meter_id)
            {
                /** at this point all 4 meters are configure according tothe original one. */
                /** therefore, need to update only the rev exp */
                /** update the original meter with the rev_exp */
                meter_profile_info[i].ir_rev_exp = current_ir_rev_exp;
            }
            else
            {
                memcpy(&meter_profile_info[i], &meter_profile_info_array[i], sizeof(dnx_meter_profile_info_t));
                meter_profile_info[i].ir_rev_exp = current_ir_rev_exp;
            }
        }
    }
    else
    {
        *reconfigure_hierarchical_meters = FALSE;
        meter_profile_info[0].ir_rev_exp = current_ir_rev_exp;
    }
exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*   Function handle the profile configuration:
*   mode1: change_profile=FALSE, perform the following steps:
*       a. get the old profile id for the given meter from HW.
*       b. exchange the profile using template manager and get new profile_id
*       c. write to hw the new profile configuration
*       d. write to hw new profile id for the given meter id
*
*   mode2: change_profile=TRUE, perform the following steps:
*       a. replace template manage data for the given profile_id
*       b. write to hw the new profile configuration
*
* \param [in] unit -unit id
* \param [in] core -core_id
* \param [in] is_ingress -is_ingress
* \param [in] database_id -database_id
* \param [in] meter_idx -meter id
* \param [in] meter_profile_info -pointer to meter profile structure
* \param [in] new_profile -pointer index to update
* \param [in] change_profile -if true, need to change the profile configuration, but not to change the meter pointer to different profile_id
* \return
*      shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_policer_mgmt_profile_alloc(
    int unit,
    int core,
    int is_ingress,
    int database_id,
    int meter_idx,
    dnx_meter_profile_info_t * meter_profile_info,
    int new_profile,
    int change_profile)
{
    int old_profile;
    dnx_meter_profile_info_t temp_meter_profile_info;
    uint8 last_reference, first_reference;
    uint8 data_was_replaced = FALSE;
    int nof_references = 0;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** change_profile only valid with new_profile !=0. user can't change profile 0 because it is the default one */
    if (change_profile && (new_profile == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "change_profile is only valid in combination with new_profile\n");
    }

    /** Get the old profile ID from HW that is used by the given meter ID */
    SHR_IF_ERR_EXIT(dnx_meter_generic_profile_id_get(unit, core, is_ingress, database_id, meter_idx, &old_profile));

    if (new_profile != 0)
    {
       /** 
        * If the user has provided a specific profile ID, get the profile data if profile is already allocated 
        */
        SHR_IF_ERR_EXIT_EXCEPT_IF(POLICER_MNGR_TM_CB(is_ingress, database_id,
                                                     profile_data_get(unit, core, new_profile, &nof_references,
                                                                      &temp_meter_profile_info)), _SHR_E_NOT_FOUND);

        /**
         * If the profile is not allocated, user wants to allocate it.
         * If the profile is allocated and change_profile == TRUE, user wants to replace the data and add another reference to the profile.
         * If the profile is allocated and change_profile == FALSE, user wants to just add another refference to it without changing the data.
         * These actions must be done with the user's specified ID.
         */
        flags = DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;

        if (nof_references != 0)
        {
            if (change_profile == TRUE)
            {
                 /**
                  * User wants to replace the data and add another reference to the profile.
                  */
                SHR_IF_ERR_EXIT(POLICER_MNGR_TM_CB(is_ingress, database_id,
                                                   replace_data(unit, core, new_profile, (void *) meter_profile_info)));
                data_was_replaced = TRUE;
            }
            else
            {
                /**
                 * User wants to add another refference to the profile without changing the data.
                 */
                flags |= DNX_ALGO_TEMPLATE_EXCHANGE_IGNORE_DATA;
            }
        }
    }
    /**
     * If the user did not specify meter profile no flags will be used. The 'exchange' function will
     * automatically allocate a new profile if the data does not exist, or will return the ID of an existing profile if it has the same data.
     */
    SHR_IF_ERR_EXIT(POLICER_MNGR_TM_CB(is_ingress, database_id,
                                       exchange(unit, core, flags, meter_profile_info, old_profile, NULL, &new_profile,
                                                &first_reference, &last_reference)));

    /** Update profile in HW if is is the first time it is set, 
     * or if the data of an existing profile was changed
     */
    if ((first_reference == TRUE) || (data_was_replaced == TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_meter_generic_profile_set(unit, core, is_ingress, database_id,
                                                      new_profile, meter_profile_info));
    }

    /** Map meter to new profile */
    SHR_IF_ERR_EXIT(dnx_meter_generic_profile_id_set(unit, core, is_ingress, database_id, meter_idx, new_profile));

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*   function handle the generic meter configuration: template manager setting, profile id setting and profile configuration.
*   In case of cascade, it will allocate 4 meters.
*   in case of flag=BCM_POLICER_REPLACE_SHARED- it will replace the data of the meter profile, but not the meter pointers
*   in case of flag=BCM_POLICER_REPLACE- it replace one meter, but in cascade mode, it might affect other 3 meters.
* \param [in] unit -unit id
* \param [in] flags -flags
* \param [in] core_id -core_id
* \param [in] is_ingress -is_ingress
* \param [in] database_id -database_id
* \param [in] meter_idx -meter id
* \param [in] pol_cfg -pointer to meter configuration
* \return
*      shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
static shr_error_e
dnx_policer_mgmt_generic_policer_set(
    int unit,
    uint32 flags,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_idx,
    bcm_policer_config_t * pol_cfg)
{
    int nof_meters = 1;
    int reconfigure_hierarchical_meters = FALSE;
    uint32 global_sharing = FALSE;
    int change_profile = (flags & BCM_POLICER_REPLACE_SHARED) ? TRUE : FALSE;
    dnx_meter_profile_info_t meter_profile_info[DNX_POLICER_CASCADE_GROUP_SIZE];
    dnx_meter_profile_info_t meter_profile_old;
    int i, meter_incr, profile, nof_ref = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** If the user just wants to change the profile that the meter uses - read the profile property from HW */
    if ((pol_cfg->entropy_id != 0) && ((pol_cfg->flags & BCM_POLICER_REPLACE_SHARED) == 0))
    {
        SHR_IF_ERR_EXIT(POLICER_MNGR_TM_CB(is_ingress, database_id,
                                           profile_data_get(unit, core_id, pol_cfg->entropy_id, &nof_ref,
                                                            &meter_profile_info[0])));
        if (nof_ref == 0)
        {
            /** If profile has not been allocated user wants to add a refference to a non-existing profile */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Trying to add a refference to profile %d that is not allocated! \n",
                         pol_cfg->entropy_id);
        }
    }
    else
    {
        /** Convert API parameters to internal HW/SW structures */
        SHR_IF_ERR_EXIT(dnx_policer_to_meter_inter_info(unit, pol_cfg, &meter_profile_info[0]));
    }

    if (pol_cfg->mode == bcmPolicerModeCascade || pol_cfg->mode == bcmPolicerModeCoupledCascade)
    {
        /** for cascade mode, the bucket_mode was determine in the create API. "replace" cannot change it */
        /** Therefore, need to keep using the same mode for all meters belong to the cascade*/
        if (flags & BCM_POLICER_REPLACE)
        {
            /** get the meters profile id from HW */
            SHR_IF_ERR_EXIT(dnx_meter_generic_profile_id_get
                            (unit, core_id, is_ingress, database_id, meter_idx, &profile));
            /** get the data from tm */
            SHR_IF_ERR_EXIT(POLICER_MNGR_TM_CB
                            (is_ingress, database_id,
                             profile_data_get(unit, core_id, profile, &nof_ref, &meter_profile_old)));
            if (meter_profile_info[0].is_large_bucket_mode == TRUE && meter_profile_old.is_large_bucket_mode == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "unable to configure buckets. current cascade configuration is noraml bucket mode. New configuration require large bucket mode (ckbits_burst=%d, pkbits_burst=%d) \n",
                             pol_cfg->ckbits_burst, pol_cfg->pkbits_burst);
            }
            meter_profile_info[0].is_large_bucket_mode = meter_profile_old.is_large_bucket_mode;
        }
        nof_meters = DNX_POLICER_CASCADE_GROUP_SIZE; /** allocate 4 meters, first one % 4 = 0 */
        if (pol_cfg->mode == bcmPolicerModeCoupledCascade)
        {
            global_sharing = 1; /** 4 meters are in hierarchical mode */
        }

        /** configure all 4 meters the same */
        for (i = 1; i < nof_meters; i++)
        {
            memcpy(&meter_profile_info[i], &meter_profile_info[0], sizeof(dnx_meter_profile_info_t));
        }
        if (flags & BCM_POLICER_REPLACE)
        {
            /** get 3 other members to determine the rev exp that is most accurate for all meters in the group.
                         HW limitation - all buckets must have the same rev_exp in cascade mode */
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_hierarchical_meters_rev_exp_set
                            (unit, core_id, is_ingress, database_id, meter_idx, meter_profile_info,
                             &reconfigure_hierarchical_meters));
        }
        else
        {
            /** calc the rev exp */
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_rev_exp_for_sharing_enable_mode_calc
                            (unit, 1, &meter_profile_info[0], &meter_profile_info[0].ir_rev_exp));
            /** update the DB */
            for (i = 1; i < nof_meters; i++)
            {
                meter_profile_info[i].ir_rev_exp = meter_profile_info[0].ir_rev_exp;
            }
        }
    }

    /** set profile id to store this information */
    if (flags & BCM_POLICER_REPLACE)
    {
        /** in case of replace, only change the rates of one of the 4 meters group */
        nof_meters = 1;
        if (reconfigure_hierarchical_meters == TRUE)
        {
            nof_meters = DNX_POLICER_CASCADE_GROUP_SIZE;
            /** align to the first meter in the group */
            meter_idx = meter_idx - (meter_idx % DNX_POLICER_CASCADE_GROUP_SIZE);
        }
    }
    for (meter_incr = 0; meter_incr < nof_meters; ++meter_incr)
    {
        meter_profile_info[meter_incr].entropy_id = pol_cfg->entropy_id;
        SHR_IF_ERR_EXIT(dnx_policer_mgmt_profile_alloc(unit, core_id, is_ingress, database_id,
                                                       meter_idx + meter_incr, &meter_profile_info[meter_incr],
                                                       meter_profile_info[meter_incr].entropy_id, change_profile));
    }

    /** set the global sharing bit */
    SHR_IF_ERR_EXIT(dnx_meter_generic_global_sharing_set
                    (unit, core_id, is_ingress, database_id, meter_idx, global_sharing));

exit:
    SHR_FUNC_EXIT;

}
/** see .h file */
shr_error_e
dnx_policer_mgmt_policer_set(
    int unit,
    uint32 flags,
    int core_id,
    int is_global,
    int is_ingress,
    int database_id,
    int meter_idx,
    bcm_policer_config_t * pol_cfg)
{
    dnx_meter_profile_info_t meter_profile_info;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    if (is_global)
    {
        SHR_IF_ERR_EXIT(dnx_policer_to_meter_inter_info(unit, pol_cfg, &meter_profile_info));
        SHR_IF_ERR_EXIT(dnx_meter_global_profile_set(unit, core_id, meter_idx, &meter_profile_info));
    }
    else
    {
        DNXCMN_CORES_ITER(unit, core_id, core_idx)
        {
            SHR_IF_ERR_EXIT
                (dnx_policer_mgmt_generic_policer_set
                 (unit, flags, core_idx, is_ingress, database_id, meter_idx, pol_cfg));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_policer_get(
    int unit,
    int core_id,
    int is_global,
    int is_ingress,
    int database_id,
    int meter_idx,
    bcm_policer_config_t * pol_cfg)
{
    dnx_meter_profile_info_t meter_profile_info = { 0 };
    int is_single_bucket = 0;
    int profile = 0;
    int global_sharing = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (is_global)
    {
        SHR_IF_ERR_EXIT(dnx_meter_global_profile_get(unit, core_id, meter_idx, &meter_profile_info));
    }
    else
    {
        /** get the profile id */
        SHR_IF_ERR_EXIT(dnx_meter_generic_profile_id_get(unit, core_id, is_ingress, database_id, meter_idx, &profile));

        /** get the profile configuration */
        SHR_IF_ERR_EXIT(dnx_meter_generic_profile_get
                        (unit, core_id, is_ingress, database_id, profile, &meter_profile_info));

        /** get if glbal sharing enable (meaning cascade mode)*/
        SHR_IF_ERR_EXIT(dnx_meter_generic_global_sharing_get
                        (unit, core_id, is_ingress, database_id, meter_idx, &global_sharing));
    }

    /** convert the internal structure to API data */
    SHR_IF_ERR_EXIT(dnx_policer_from_meter_inter_info_convert(unit, core_id, meter_idx, is_single_bucket,
                                                              &meter_profile_info, global_sharing, profile, pol_cfg));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_database_create(
    int unit,
    int flags,
    int core_id,
    int is_ingress,
    int database_id,
    bcm_policer_database_config_t * config)
{
    uint32 entry_handle_id, entry_handle_id1, filter_bitmap = 0;
    int expansion_size;
    int nof_expansion_groups, expansion_flags;
    int core_idx;
    bcm_policer_config_t pol_cfg;
    dnx_meter_profile_info_t meter_profile_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** if expansion disable, the expansion size is zero, if enable, take it from the expansion group mapping */
    if (config->expansion_enable == FALSE)
    {
        expansion_size = 0;
    }
    else
    {
        expansion_flags = (config->expand_per_tc == TRUE) ? BCM_POLICER_EXPANSION_GROUPS_TC : 0;
        SHR_IF_ERR_EXIT(dnx_policer_mgmt_nof_expansion_groups_get
                        (unit, expansion_flags, core_id, &nof_expansion_groups));
        if (nof_expansion_groups == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "nof_expansion_groups=0\n");
        }
        else
        {
            expansion_size = nof_expansion_groups - 1;
        }

        if (config->expand_per_tc == FALSE)
        {
            /** in case expansion enabled, the expansion size and L2-Fwd_type filter should be the same as the global meter. */
            /** therefore, we get it from global and meter, and set the same in ingress generic meter */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_GENERAL_CONFIG, &entry_handle_id1));
            dbal_entry_key_field32_set(unit, entry_handle_id1, DBAL_FIELD_CORE_ID, core_id);
            dbal_value_field32_request(unit, entry_handle_id1, DBAL_FIELD_FWD_TYPE_FILTER_BITMAP, INST_SINGLE,
                                       &filter_bitmap);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id1, DBAL_COMMIT));
        }
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      ((is_ingress) ? DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG :
                                       DBAL_TABLE_METER_EGRESS_DB_GENERAL_CONFIG), &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SINGLE_RATE_ENABLE, INST_SINGLE,
                                 config->is_single_bucket);
    if (is_ingress)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXPANSION_SIZE, INST_SINGLE, expansion_size);
        if (dnx_data_meter.expansion.feature_get(unit, dnx_data_meter_expansion_expansion_based_on_tc_support) == TRUE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXPANSION_IS_GLBL_POINTER, INST_SINGLE,
                                         ((config->expand_per_tc == TRUE) ? FALSE : TRUE));
        }

        if (config->expand_per_tc == FALSE)
        {
            /** Type Filtering bitmap is relevant only when expanding per L2-Fwd type */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_TYPE_FILTER_BITMAP, INST_SINGLE,
                                         filter_bitmap);
        }
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** get default profile according to the bucket mode */
    /** set default meters to the max rate */
    SHR_IF_ERR_EXIT(dnx_policer_default(unit, &pol_cfg, config->is_single_bucket));
    SHR_IF_ERR_EXIT(dnx_policer_to_meter_inter_info(unit, &pol_cfg, &meter_profile_info));

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        /** update the tm default profile according to the database mode */
        SHR_IF_ERR_EXIT(POLICER_MNGR_TM_CB(is_ingress, database_id,
                                           replace_data(unit, core_idx, DNX_METER_UTILS_DEFAULT_PROFILE_ID,
                                                        (void *) &meter_profile_info)));
        /** set it to HW */
        SHR_IF_ERR_EXIT(dnx_meter_generic_profile_set(unit, core_idx, TRUE, database_id,
                                                      DNX_METER_UTILS_DEFAULT_PROFILE_ID, &meter_profile_info));

        /** update sw state */
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.created.set
                        (unit, core_idx, is_ingress, database_id, TRUE));
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.expansion_enable.set
                        (unit, core_idx, is_ingress, database_id, config->expansion_enable));
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.expansion_is_per_tc.set
                        (unit, core_idx, is_ingress, database_id, config->expand_per_tc));
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.is_single_bucket.set
                        (unit, core_idx, is_ingress, database_id, config->is_single_bucket));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_policer_mgmt_nof_expansion_groups_get(
    int unit,
    int flags,
    int core_id,
    int *nof_expansion_groups)
{
    bcm_policer_expansion_group_t expansion_groups;
    int offset[(BCM_COS_COUNT > bcmPolicerFwdTypeMax) ? BCM_COS_COUNT : bcmPolicerFwdTypeMax] = { FALSE };
    int idx;
    int nof_groups = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_expansion_groups_get(unit, flags, core_id, &expansion_groups));
    if (flags == 0)
    {
        /** count for each meter offset, how many valid fwd-types use it */
        for (idx = bcmPolicerFwdTypeUc; idx < bcmPolicerFwdTypeMax; idx++)
        {
            if (expansion_groups.config[idx].valid == TRUE)
            {
                if (offset[expansion_groups.config[idx].offset] == FALSE)
                {
                    nof_groups++;
                }
                offset[expansion_groups.config[idx].offset] = TRUE;
            }
        }
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_POLICER_EXPANSION_GROUPS_TC))
    {
        /** Count the TC expansion groups */
        for (idx = 0; idx < BCM_COS_COUNT; idx++)
        {
            if (offset[expansion_groups.tc_offset[idx]] == FALSE)
            {
                nof_groups++;
            }
            offset[expansion_groups.tc_offset[idx]] = TRUE;
        }
    }

    *nof_expansion_groups = nof_groups;

exit:
    SHR_FUNC_EXIT;
}

/** see .h file*/
shr_error_e
dnx_policer_mgmt_engine_database_attach(
    int unit,
    int flags,
    bcm_policer_engine_t * engine,
    bcm_policer_database_attach_config_t * config)
{
    dnx_crps_mgmt_shmem_section_t section;
    int core_idx;
    int is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(config->policer_database_handle);
    int database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(config->policer_database_handle);
    int database_core = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(config->policer_database_handle);
    int single_bucket_bank, bank_id = -1;
    int nof_meters;
    uint32 nof_generic_banks = 0, address_map_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** convert API enum to internal enum. the internal enum holds the api enum values, so they are the same */
    section = (dnx_crps_mgmt_shmem_section_t) engine->section;

    /** engine 0 and engine 1 are small engines and cannot be concatanated to other engines and must be used as single bucket meter */
    single_bucket_bank = DNX_CRPS_METER_SHMEM_IS_METER_SINGLE_BUCKET_ENGINE(unit, engine->engine_id);

    /** update sw state */
    DNXCMN_CORES_ITER(unit, database_core, core_idx)
    {

        /** update the sw state that the engine is used by meter (or crps in case of detach) */
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_meter_engine_attach_detach
                        (unit, core_idx, engine->engine_id, section, is_ingress, TRUE));

        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.use_single_bucket_bank.set
                        (unit, core_idx, is_ingress, database_id, single_bucket_bank));
        /** for genenric banks,  update the new banks added to the database and the nof_banks */
        if (single_bucket_bank == FALSE)
        {
            /** for dual bucket mode, the sw state holds the base bank. for example: engine=16 (banks0&6) > bank_id=0 */
            SHR_IF_ERR_EXIT(dnx_crps_mgmt_meter_shmem_bank_id_get(unit, engine->engine_id, section, &bank_id));

            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.nof_generic_banks.get
                            (unit, core_idx, is_ingress, database_id, &nof_generic_banks));
            address_map_index = nof_generic_banks;
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.banks_arr.bank_id.set
                            (unit, core_idx, is_ingress, database_id, nof_generic_banks, bank_id));
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.banks_arr.base_meter_index.set
                            (unit, core_idx, is_ingress, database_id, nof_generic_banks,
                             config->object_stat_pointer_base));
            nof_generic_banks++;
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.nof_generic_banks.set
                            (unit, core_idx, is_ingress, database_id, nof_generic_banks));
        }
        /** get nof meters for this database */
        SHR_IF_ERR_EXIT(dnx_policer_mgmt_nof_meter_in_database_get
                        (unit, core_idx, is_ingress, database_id, &nof_meters));

        /** set the HW tables */
        SHR_IF_ERR_EXIT(dnx_meter_generic_address_range_hw_set(unit, core_idx, is_ingress, database_id, nof_meters));
        SHR_IF_ERR_EXIT(dnx_meter_generic_address_map_hw_set
                        (unit, core_idx, is_ingress, database_id, address_map_index, bank_id));

        /** update the table the map between the pointer and the bank id that represent it */
        if (single_bucket_bank == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_set
                            (unit, core_idx, is_ingress, database_id, config->object_stat_pointer_base,
                             (uint32) bank_id));
        }

        /** clear the entire memory space */
        SHR_IF_ERR_EXIT(dnx_meter_generic_engine_clear
                        (unit, core_idx, engine, is_ingress, database_id, single_bucket_bank));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_policer_mgmt_engine_database_find(
    int unit,
    int core_id,
    bcm_policer_engine_t * engine,
    int *is_ingress,
    int *database_id,
    int *bank_index,
    int *found)
{
    int is_ingress_idx;
    int database_idx, nof_databases;
    int bank_idx = 0, bank_id, calc_bank_id;
    uint32 nof_banks;
    int engine_found = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    for (is_ingress_idx = FALSE; is_ingress_idx <= TRUE; is_ingress_idx++)
    {
        nof_databases = (is_ingress_idx == TRUE) ?
            dnx_data_meter.meter_db.nof_ingress_db_get(unit) : dnx_data_meter.meter_db.nof_egress_db_get(unit);
        for (database_idx = 0; database_idx < nof_databases; database_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.nof_generic_banks.get
                            (unit, core_id, is_ingress_idx, database_idx, &nof_banks));
            for (bank_idx = 0; bank_idx < nof_banks; bank_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.banks_arr.bank_id.get
                                (unit, core_id, is_ingress_idx, database_idx, bank_idx, &bank_id));
                SHR_IF_ERR_EXIT(dnx_crps_mgmt_meter_shmem_bank_id_get(unit, engine->engine_id,
                                                                      (dnx_crps_mgmt_shmem_section_t) engine->section,
                                                                      &calc_bank_id));
                if (bank_id == calc_bank_id)
                {
                    engine_found = TRUE;
                    break;
                }
            }
            if (engine_found)
            {
                break;
            }
        }
        if (engine_found)
        {
            break;
        }
    }

    *found = (engine_found == FALSE) ? 0 : 1;
    *is_ingress = is_ingress_idx;
    *database_id = database_idx;
    *bank_index = bank_idx;
exit:
    SHR_FUNC_EXIT;
}

/** see .h file*/
shr_error_e
dnx_policer_mgmt_engine_database_detach(
    int unit,
    int flags,
    bcm_policer_engine_t * engine)
{
    dnx_crps_mgmt_shmem_section_t section;
    int is_ingress = 0;
    int database_id;
    int core_idx;
    int bank_id_to_remove, bank_idx_to_remove, bank_id, bank_idx, base_meter_index;
    uint32 nof_banks, hw_bank_id;
    int nof_meters = 0;
    dbal_table_field_info_t index_field_info;
    int entry_ptr_map_table;
    int found;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, engine->core_id, core_idx)
    {
        /** engine 0 and engine 1 are special engines, small that do not have mapping. no need to update the mapping for them, \
                     just need to reduce the range */

        if (DNX_CRPS_METER_SHMEM_IS_METER_SINGLE_BUCKET_ENGINE(unit, engine->engine_id) == FALSE)
        {
            /** find the bank that the engine belong to */
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_engine_database_find
                            (unit, core_idx, engine, &is_ingress, &database_id, &bank_idx_to_remove, &found));
            if (found == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "engine=%d, core=%d was not found in any meter database\n",
                             engine->engine_id, core_idx);
            }

            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.banks_arr.bank_id.get
                            (unit, core_idx, is_ingress, database_id, bank_idx_to_remove, &bank_id_to_remove));

            /** verification parameters: it is made here because it is the first time that we know on the database that the engine belong to.
                         if database is dual bucket, must use section all. if single bucket, can't use section all */
            SHR_INVOKE_VERIFY_DNX(dnx_policer_mgmt_section_verify
                                  (unit, core_idx, is_ingress, database_id, engine->section));

            /** get from dbal the size of the table index*/
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_PTR_MAP, DBAL_FIELD_INDEX,
                                                       TRUE, 0, 0, &index_field_info));

            /** if big engines, go over ptr_map and find the first index that holds the bank_idx_to_remove */
            for (entry_ptr_map_table = 0; entry_ptr_map_table <= index_field_info.max_value; entry_ptr_map_table++)
            {
                SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_get
                                (unit, core_idx, is_ingress, database_id, entry_ptr_map_table, &hw_bank_id));
                if (bank_id_to_remove == hw_bank_id)
                {
                    /** set invalid bank instead of the bank is now removed in the ptr_map table*/
                    SHR_IF_ERR_EXIT(dnx_meter_generic_ptr_map_hw_set(unit, core_idx, is_ingress, database_id,
                                                                     (entry_ptr_map_table *
                                                                      dnx_data_meter.
                                                                      mem_mgmt.ptr_map_table_resolution_get(unit)),
                                                                     dnx_data_meter.
                                                                     mem_mgmt.invalid_bank_id_get(unit)));
                    break;
                }
            }

            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.nof_generic_banks.get
                            (unit, core_idx, is_ingress, database_id, &nof_banks));
            nof_banks--;
            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.nof_generic_banks.set
                            (unit, core_idx, is_ingress, database_id, nof_banks));

            /** remove from sw state array the engine bank and reorder the array */
            for (bank_idx = 0; bank_idx < nof_banks; bank_idx++)
            {
                if (bank_idx >= bank_idx_to_remove)
                {
                    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.banks_arr.bank_id.get
                                    (unit, core_idx, is_ingress, database_id, (bank_idx + 1), &bank_id));
                    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.banks_arr.base_meter_index.get
                                    (unit, core_idx, is_ingress, database_id, (bank_idx + 1), &base_meter_index));

                    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.banks_arr.bank_id.set
                                    (unit, core_idx, is_ingress, database_id, bank_idx, bank_id));
                    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.banks_arr.base_meter_index.set
                                    (unit, core_idx, is_ingress, database_id, bank_idx, base_meter_index));
                    /** set the HW tables */
                    /** since the array was updated, and reorder, we need to update all banks from the first change */
                    SHR_IF_ERR_EXIT(dnx_meter_generic_address_map_hw_set
                                    (unit, core_idx, is_ingress, database_id, bank_idx, bank_id));
                }
            }
        }
        else
        {
            is_ingress =
                (engine->engine_id == dnx_data_meter.mem_mgmt.ingress_single_bucket_engine_get(unit)) ? TRUE : FALSE;
            /** special (single_buckets) engines can be connected only to database_id-0*/
            database_id = 0;
        }
        /** get nof meters for this database */
        SHR_IF_ERR_EXIT(dnx_policer_mgmt_nof_meter_in_database_get
                        (unit, core_idx, is_ingress, database_id, &nof_meters));
        /** set HW address range */
        SHR_IF_ERR_EXIT(dnx_meter_generic_address_range_hw_set(unit, core_idx, is_ingress, database_id, nof_meters));

        /** convert API enum to internal enum */
        section = (dnx_crps_mgmt_shmem_section_t) engine->section;

        /** update the sw state that the engine is no longer used by meter. */
        SHR_IF_ERR_EXIT(dnx_crps_mgmt_meter_engine_attach_detach
                        (unit, engine->core_id, engine->engine_id, section, is_ingress, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file*/
shr_error_e
dnx_policer_mgmt_engine_database_get(
    int unit,
    int flags,
    bcm_policer_engine_t * engine,
    bcm_policer_database_attach_config_t * config)
{
    int is_ingress, database_id;
    int bank_idx;
    int core_idx;
    int found;
    dbal_table_field_info_t index_field_info;
    SHR_FUNC_INIT_VARS(unit);

    /** get from dbal the size of the table index*/
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_METER_ING_PTR_MAP, DBAL_FIELD_INDEX,
                                               TRUE, 0, 0, &index_field_info));

    core_idx = (engine->core_id == BCM_CORE_ALL) ? 0 : engine->core_id;

    /** if special (small and single_bucket) engines, base=0 */
    if (DNX_CRPS_METER_SHMEM_IS_METER_SINGLE_BUCKET_ENGINE(unit, engine->engine_id) == TRUE)
    {
        config->object_stat_pointer_base = 0;
        is_ingress = dnx_data_meter.mem_mgmt.ingress_single_bucket_engine_get(unit) == engine->engine_id ? TRUE : FALSE;
        /** update the database_handle. single_bucket engine is used only for database id 0 */
        _SHR_POLICER_DATABASE_HANDLE_SET(config->policer_database_handle, is_ingress, 0, core_idx, 0);
    }
    else
    {
        /** find the bank that the engine belong to */
        SHR_IF_ERR_EXIT(dnx_policer_mgmt_engine_database_find
                        (unit, core_idx, engine, &is_ingress, &database_id, &bank_idx, &found));
        if (found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "engine=%d, core=%d was not found in any meter database\n", engine->engine_id,
                         core_idx);
        }
        /** update the database_handle */
        _SHR_POLICER_DATABASE_HANDLE_SET(config->policer_database_handle, is_ingress, 0, core_idx, database_id);

        /** verify that section is valid and match to the database configuration */
        SHR_INVOKE_VERIFY_DNX(dnx_policer_mgmt_section_verify
                              (unit, core_idx, is_ingress, database_id, engine->section));
        /** get from sw state the bank_id that was found */
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.banks_arr.base_meter_index.get
                        (unit, core_idx, is_ingress, database_id, bank_idx, &config->object_stat_pointer_base));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file*/
shr_error_e
dnx_policer_mgmt_section_verify(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    bcm_policer_engine_section_t section)
{
    uint32 is_single_bucket;
    SHR_FUNC_INIT_VARS(unit);

    /** verification parameters: it is made here because it is the first time that we know on the database that the engine belong to.
                 if database is dual bucket, must use section all. if single bucket, can't use section all */
    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.is_single_bucket.get
                    (unit, core_id, is_ingress, database_id, &is_single_bucket));
    if ((is_single_bucket == FALSE) && (section != bcmPolicerEngineSectionAll))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "for is_single_bucket=0, EngineSection must be 'ALL'\n");
    }
    if ((is_single_bucket == TRUE) && (section == bcmPolicerEngineSectionAll))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "for is_single_bucket=TRUE, EngineSection='ALL' is not allowed\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file*/
shr_error_e
dnx_policer_mgmt_nof_meter_in_database_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int *nof_meters)
{
    int use_single_bucket_bank;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    int engine_id;
    uint32 nof_banks;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.use_single_bucket_bank.get
                    (unit, core_id, is_ingress, database_id, &use_single_bucket_bank));

    if (use_single_bucket_bank == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.nof_generic_banks.get
                        (unit, core_id, is_ingress, database_id, &nof_banks));
        /** nof meters is not nof buckets. each bank contain 16K buckets (which is the size of crps engine).
                      so nof_meters is nof_banks*nof_counters.
                      For Dual bucket mode,  2 banks contain 16K meters, but the parameter nof_generic_banks do not reflect that it use 2 banks,
                      so also in this case the calculation is the same */
        engine_info = dnx_data_crps.engine.engines_info_get
            (unit, dnx_data_meter.mem_mgmt.base_big_engine_for_meter_get(unit));
        *nof_meters = nof_banks * engine_info->nof_counters;
    }
    else
    {
        engine_id = (is_ingress == TRUE) ? dnx_data_meter.mem_mgmt.ingress_single_bucket_engine_get(unit) :
            dnx_data_meter.mem_mgmt.egress_single_bucket_engine_get(unit);
        engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);

        /** for each engine, there are X counters or 2*X meter buckets. so nof_meters are calculated base of that.*/
        *nof_meters = (engine_info->nof_counters * dnx_data_meter.mem_mgmt.counters_buckets_ratio_per_engine_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      convert the color from bcm_policer_color_t enum to DBAL enum
 * \param [in] unit -unit id
 * \param [in] internal_color -3 color mode
 * \param [out] dbal_color - dbal enum color
 * \return
 *   shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_policer_mgmt_int_color_to_dbal(
    int unit,
    bcm_policer_color_t internal_color,
    dbal_enum_value_field_meter_internal_color_mapping_e * dbal_color)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (internal_color)
    {
        case bcmPolicerColorGreen:
            *dbal_color = DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_GREEN;
            break;
        case bcmPolicerColorYellow:
            *dbal_color = DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_YELLOW;
            break;
        case bcmPolicerColorRed:
            *dbal_color = DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_RED;
            break;
        case bcmPolicerColorInvalid:
            *dbal_color = DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_INVALID;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid policer color: %d \n", internal_color);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      convert the color from DBAL enum to bcm_policer_color_t enum
 * \param [in] unit -unit id
 * \param [in] dbal_color - dbal enum color
 * \param [out] internal_color -3 color mode
 * \return
 *   shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_policer_mgmt_int_color_from_dbal(
    int unit,
    dbal_enum_value_field_meter_internal_color_mapping_e dbal_color,
    bcm_policer_color_t * internal_color)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (dbal_color)
    {
        case DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_GREEN:
            *internal_color = bcmPolicerColorGreen;
            break;
        case DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_YELLOW:
            *internal_color = bcmPolicerColorYellow;
            break;
        case DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_RED:
            *internal_color = bcmPolicerColorRed;
            break;
        case DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_INVALID:
            *internal_color = bcmPolicerColorInvalid;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid policer dbal color: %d \n", dbal_color);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_primary_color_resolution_set(
    int unit,
    int flags,
    bcm_policer_primary_color_resolution_key_t * key,
    bcm_policer_primary_color_resolution_config_t * config)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_meter_internal_color_mapping_e dbal_internal_color;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (key->is_ingress == TRUE)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_RESOLVE_COLOR_TABLE, &entry_handle_id));
    }
    else
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGR_RESOLVE_COLOR_TABLE, &entry_handle_id));

    }

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, key->core_id);
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_to_dbal(unit, key->color_data[0].policer_color, &dbal_internal_color));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_0_STATE, dbal_internal_color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_0_OTHER_BUCKET,
                               key->color_data[0].policer_other_bucket_has_credits);
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_to_dbal(unit, key->color_data[1].policer_color, &dbal_internal_color));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_1_STATE, dbal_internal_color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_1_OTHER_BUCKET,
                               key->color_data[1].policer_other_bucket_has_credits);
    if (key->is_ingress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_to_dbal
                        (unit, key->color_data[2].policer_color, &dbal_internal_color));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_2_STATE, dbal_internal_color);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_2_OTHER_BUCKET,
                                   key->color_data[2].policer_other_bucket_has_credits);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION, key->action);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESOLVE_DP, INST_SINGLE, config->resolved_color);
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_to_dbal(unit,
                                                       ((config->policer_update_bucket[0] ==
                                                         bcmPolicerColorRed) ? bcmPolicerColorInvalid :
                                                        config->policer_update_bucket[0]), &dbal_internal_color));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_0_UPDATE_BUCKET, INST_SINGLE,
                                 dbal_internal_color);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_0_COUNTER_VALID, INST_SINGLE,
                                 config->policer_count[0]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_0_COUNTER_COLOR, INST_SINGLE,
                                 config->policer_color[0]);
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_to_dbal
                    (unit,
                     ((config->policer_update_bucket[1] ==
                       bcmPolicerColorRed) ? bcmPolicerColorInvalid : config->policer_update_bucket[1]),
                     &dbal_internal_color));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_1_UPDATE_BUCKET, INST_SINGLE,
                                 dbal_internal_color);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_1_COUNTER_VALID, INST_SINGLE,
                                 config->policer_count[1]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_1_COUNTER_COLOR, INST_SINGLE,
                                 config->policer_color[1]);
    if (key->is_ingress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_to_dbal(unit,
                                                           ((config->policer_update_bucket[2] ==
                                                             bcmPolicerColorRed) ? bcmPolicerColorInvalid :
                                                            config->policer_update_bucket[2]), &dbal_internal_color));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_2_UPDATE_BUCKET, INST_SINGLE,
                                     dbal_internal_color);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_2_COUNTER_VALID, INST_SINGLE,
                                     config->policer_count[2]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_2_COUNTER_COLOR, INST_SINGLE,
                                     config->policer_color[2]);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT | DBAL_COMMIT_OVERRUN));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_primary_color_resolution_get(
    int unit,
    int flags,
    bcm_policer_primary_color_resolution_key_t * key,
    bcm_policer_primary_color_resolution_config_t * config)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_meter_internal_color_mapping_e dbal_internal_color;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (key->is_ingress == TRUE)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_RESOLVE_COLOR_TABLE, &entry_handle_id));
    }
    else
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGR_RESOLVE_COLOR_TABLE, &entry_handle_id));

    }

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, key->core_id);
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_to_dbal(unit, key->color_data[0].policer_color, &dbal_internal_color));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_0_STATE, dbal_internal_color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_0_OTHER_BUCKET,
                               key->color_data[0].policer_other_bucket_has_credits);
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_to_dbal(unit, key->color_data[1].policer_color, &dbal_internal_color));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_1_STATE, dbal_internal_color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_1_OTHER_BUCKET,
                               key->color_data[1].policer_other_bucket_has_credits);
    if (key->is_ingress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_to_dbal
                        (unit, key->color_data[2].policer_color, &dbal_internal_color));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_2_STATE, dbal_internal_color);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_METER_DB_2_OTHER_BUCKET,
                                   key->color_data[2].policer_other_bucket_has_credits);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTION, key->action);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RESOLVE_DP, INST_SINGLE,
                               (uint32 *) (&config->resolved_color));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_METER_0_UPDATE_BUCKET, INST_SINGLE,
                               (uint32 *) (&config->policer_update_bucket[0]));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_METER_0_COUNTER_VALID, INST_SINGLE,
                               (uint32 *) (&config->policer_count[0]));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_METER_0_COUNTER_COLOR, INST_SINGLE,
                               (uint32 *) (&config->policer_color[0]));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_METER_1_UPDATE_BUCKET, INST_SINGLE,
                               (uint32 *) (&config->policer_update_bucket[1]));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_METER_1_COUNTER_VALID, INST_SINGLE,
                               (uint32 *) (&config->policer_count[1]));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_METER_1_COUNTER_COLOR, INST_SINGLE,
                               (uint32 *) (&config->policer_color[1]));
    if (key->is_ingress == TRUE)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_METER_2_UPDATE_BUCKET, INST_SINGLE,
                                   (uint32 *) (&config->policer_update_bucket[2]));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_METER_2_COUNTER_VALID, INST_SINGLE,
                                   (uint32 *) (&config->policer_count[2]));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_METER_2_COUNTER_COLOR, INST_SINGLE,
                                   (uint32 *) (&config->policer_color[2]));
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_from_dbal(unit,
                                                         (dbal_enum_value_field_meter_internal_color_mapping_e)
                                                         config->policer_update_bucket[0],
                                                         &config->policer_update_bucket[0]));
    SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_from_dbal
                    (unit, (dbal_enum_value_field_meter_internal_color_mapping_e) config->policer_update_bucket[1],
                     &config->policer_update_bucket[1]));
    if (key->is_ingress == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_policer_mgmt_int_color_from_dbal(unit,
                                                             (dbal_enum_value_field_meter_internal_color_mapping_e)
                                                             config->policer_update_bucket[2],
                                                             &config->policer_update_bucket[2]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_color_final_resolution_set(
    int unit,
    bcm_policer_color_resolution_t * policer_color_resolution)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((policer_color_resolution->flags & BCM_COLOR_RESOLUTION_EGRESS) == FALSE)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_FINAL_RESOLVE_COLOR_TABLE, &entry_handle_id));
    }
    else
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGR_FINAL_RESOLVE_COLOR_TABLE, &entry_handle_id));

    }

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_CMD, policer_color_resolution->policer_action);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_COLOR, policer_color_resolution->incoming_color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RESOLVE_DP, policer_color_resolution->policer_color);
    /** global meter is relevant only for ingress and the value are different */
    if ((policer_color_resolution->flags & BCM_COLOR_RESOLUTION_EGRESS) == FALSE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_DECISION_DROP,
                                   (policer_color_resolution->ethernet_policer_color == bcmColorRed));
        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_COLOR, INST_SINGLE,
                                     policer_color_resolution->egress_color);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_COLOR, INST_SINGLE,
                                     policer_color_resolution->ingress_color);

    }
    else
    {
        /** Setting value fields for egress table*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_COLOR, INST_SINGLE,
                                     policer_color_resolution->egress_color);
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_color_final_resolution_get(
    int unit,
    bcm_policer_color_resolution_t * policer_color_resolution)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((policer_color_resolution->flags & BCM_COLOR_RESOLUTION_EGRESS) == FALSE)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_FINAL_RESOLVE_COLOR_TABLE, &entry_handle_id));
    }
    else
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGR_FINAL_RESOLVE_COLOR_TABLE, &entry_handle_id));

    }

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_CMD, policer_color_resolution->policer_action);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_COLOR, policer_color_resolution->incoming_color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RESOLVE_DP, policer_color_resolution->policer_color);
    /** global meter is relevant only for ingress */
    if ((policer_color_resolution->flags & BCM_COLOR_RESOLUTION_EGRESS) == FALSE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_DECISION_DROP,
                                   (policer_color_resolution->ethernet_policer_color == bcmColorRed));
        /** Setting value fields */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_EGRESS_COLOR, INST_SINGLE,
                                   (uint32 *) (&policer_color_resolution->egress_color));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INGRESS_COLOR, INST_SINGLE,
                                   (uint32 *) (&policer_color_resolution->ingress_color));
    }
    else
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_COLOR, INST_SINGLE,
                                   (uint32 *) (&policer_color_resolution->egress_color));
        policer_color_resolution->ingress_color = bcmColorPreserve;
    }

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      verify ingress port compensation delta and logical port
 * \param [in] unit -unit id
 * \param [in] logical_port - logical_port
 * \param [in] delta -compensation delta
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_policer_mgmt_ingress_port_compensation_delta_and_port_verify(
    int unit,
    bcm_port_t logical_port,
    int delta)
{
    const dnx_data_meter_compensation_ingress_port_delta_value_t *delta_boundries =
        dnx_data_meter.compensation.ingress_port_delta_value_get(unit);
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect port=%d\n, ILKN ELK port compensation not supported", logical_port);
    }

    if (delta < delta_boundries->min || delta > delta_boundries->max)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Incorrect delta=%d\n, must be in range min=%d max=%d", delta,
                     delta_boundries->min, delta_boundries->max);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_ingress_port_compensation_delta_set(
    int unit,
    bcm_port_t logical_port,
    int delta)
{
    uint32 entry_handle_id, fld_delta;
    uint32 cgm_input_port[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF], temp_port_indx;
    int nof_cgm_entries;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** verify delta and port value */
    SHR_INVOKE_VERIFY_DNX(dnx_policer_mgmt_ingress_port_compensation_delta_and_port_verify(unit, logical_port, delta));

    /** Get the relevant core ID from the src_port */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core_id));
    SHR_IF_ERR_EXIT(dnx_cosq_cgm_in_port_get(unit, logical_port, &nof_cgm_entries, cgm_input_port));

    fld_delta = UTILEX_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, DNX_POLICER_EIGHT_BITS);

    for (temp_port_indx = 0; temp_port_indx < nof_cgm_entries; temp_port_indx++)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_INGRESS_PORT_HEADER_COMPENSATION, &entry_handle_id));

        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, cgm_input_port[temp_port_indx]);

        /** Setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADER_DELTA, INST_SINGLE, fld_delta);

        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Last -- mark the port to have unchangable priority */
    SHR_IF_ERR_EXIT(dnx_cosq_port_fixed_priority_set(unit, logical_port, 1));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_ingress_port_compensation_delta_get(
    int unit,
    int logical_port,
    int *delta)
{
    uint32 fld_delta;
    uint32 entry_handle_id;
    uint32 cgm_input_port[DNX_DATA_MAX_INGR_REASSEMBLY_PRIORITY_CGM_PRIORITIES_NOF];
    int nof_cgm_entries;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the relevant core ID from the src_port */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core_id));

    SHR_IF_ERR_EXIT(dnx_cosq_cgm_in_port_get(unit, logical_port, &nof_cgm_entries, cgm_input_port));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_INGRESS_PORT_HEADER_COMPENSATION, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, cgm_input_port[0]);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HEADER_DELTA, INST_SINGLE, &fld_delta);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** calculate the size of delta parameter */
    *delta = UTILEX_TWO_COMPLEMENT_INTO_SIGNED_NUM(fld_delta, DNX_POLICER_EIGHT_BITS);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_mef_10_3_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** INGRESS  */
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_GENERAL_SETTINGS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEF_10_DOT_3_ENABLE, INST_SINGLE, enable);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** EGRESS  */
   /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGR_GENERAL_SETTINGS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEF_10_DOT_3_ENABLE, INST_SINGLE, enable);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_mef_10_3_get(
    int unit,
    int *enable)
{
    uint32 enable_get;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_GENERAL_SETTINGS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MEF_10_DOT_3_ENABLE, INST_SINGLE, &enable_get);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *enable = enable_get;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_filter_by_dp_command_set(
    int unit,
    int policer_database_handle,
    uint32 filter_bitmap)
{
    int is_global, is_ingress, database_id;
    uint32 entry_handle_id;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);
    database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(policer_database_handle);
    is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(policer_database_handle);
    is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(policer_database_handle);

    if (is_global)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_GENERAL_CONFIG, &entry_handle_id));
    }
    else if (is_ingress)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    }
    /** egress meter */
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_CMD_FILTER_BITMAP, INST_SINGLE, filter_bitmap);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_filter_by_dp_command_get(
    int unit,
    int policer_database_handle,
    uint32 *filter_bitmap)
{
    int is_global, is_ingress, database_id;
    uint32 entry_handle_id;
    uint32 fld_value;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);
    database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(policer_database_handle);
    is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(policer_database_handle);
    is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(policer_database_handle);

    if (is_global)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_GLOBAL_GENERAL_CONFIG, &entry_handle_id));
    }
    else if (is_ingress)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);

    }
    /** egress meter */
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DP_CMD_FILTER_BITMAP, INST_SINGLE, &fld_value);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *filter_bitmap = fld_value;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_dp_resolution_by_dp_command_set(
    int unit,
    int policer_database_handle,
    uint32 enable)
{
    int enable_profile = TRUE, disable_profile = FALSE;
    int is_ingress;
    uint32 entry_handle_id;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);
    is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(policer_database_handle);

    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_GENERAL_SETTINGS, &entry_handle_id));
    }
    /** egress meter */
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGR_GENERAL_SETTINGS, &entry_handle_id));
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    if (enable == TRUE)
    {
        /** user wants to select DP data instead of Profile */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESOLVE_TABLE_MODE_SELECT_PROFILE, INST_SINGLE,
                                     disable_profile);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESOLVE_TABLE_MODE_SELECT_PROFILE, INST_SINGLE,
                                     enable_profile);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_dp_resolution_by_dp_command_get(
    int unit,
    int policer_database_handle,
    uint32 *enable)
{
    int enable_dp = TRUE, disable_dp = FALSE;
    int is_ingress;
    uint32 entry_handle_id;
    uint32 fld_value;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);
    is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(policer_database_handle);

    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_ING_GENERAL_SETTINGS, &entry_handle_id));
    }
    /** egress meter */
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_EGR_GENERAL_SETTINGS, &entry_handle_id));
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RESOLVE_TABLE_MODE_SELECT_PROFILE, INST_SINGLE,
                               &fld_value);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (fld_value == TRUE)
    {
        /** resolution by Profile - enabled, by DP - disabled */
        *enable = disable_dp;
    }
    else
    {
        *enable = enable_dp;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_traverse(
    int unit,
    bcm_policer_traverse_cb cb,
    void *user_data)
{
    int policer_id;
    bcm_policer_config_t pol_cfg;
    int core_idx, is_ingress_idx, is_global;
    int database_idx, nof_databases;
    int bank_idx = 0, object_stat_pointer_base;
    int profile_idx, default_profile = 0;
    uint32 is_created, nof_banks;
    int is_enabled, meter_idx, single_bucket_bank;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    int engine_id, nof_meters, database_handle;
    int nof_expansion_groups;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        /** Generic  */
        is_global = 0;
        /** egress / ingress */
        for (is_ingress_idx = FALSE; is_ingress_idx <= TRUE; is_ingress_idx++)
        {
            nof_databases = (is_ingress_idx == TRUE) ?
                dnx_data_meter.meter_db.nof_ingress_db_get(unit) : dnx_data_meter.meter_db.nof_egress_db_get(unit);
            for (database_idx = 0; database_idx < nof_databases; database_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.
                                created.get(unit, core_idx, is_ingress_idx, database_idx, &is_created));
                if (is_created == TRUE)
                {
                    /** if database is created check if special bank or regular
                     *  bank */
                    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.use_single_bucket_bank.get
                                    (unit, core_idx, is_ingress_idx, database_idx, &single_bucket_bank));
                    if (single_bucket_bank == FALSE)
                    {
                        /** for regular bank iterate the bank array */
                        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.
                                        nof_generic_banks.get(unit, core_idx, is_ingress_idx, database_idx,
                                                              &nof_banks));
                        for (bank_idx = 0; bank_idx < nof_banks; bank_idx++)
                        {
                            /** get the base pointer */
                            SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.banks_arr.
                                            base_meter_index.get(unit, core_idx, is_ingress_idx, database_idx, bank_idx,
                                                                 &object_stat_pointer_base));
                            for (meter_idx = object_stat_pointer_base;
                                 meter_idx <
                                 (object_stat_pointer_base +
                                  dnx_data_meter.mem_mgmt.object_stat_pointer_base_resolution_get(unit)); meter_idx++)
                            {
                                /** check if meter id is configured */
                                SHR_IF_ERR_EXIT(dnx_meter_generic_profile_id_get
                                                (unit, core_idx, is_ingress_idx, database_idx, meter_idx,
                                                 &profile_idx));
                                if (profile_idx != default_profile)
                                {
                                    SHR_IF_ERR_EXIT(dnx_policer_mgmt_policer_get
                                                    (unit, core_idx, is_global, is_ingress_idx, database_idx, meter_idx,
                                                     &pol_cfg));
                                    _SHR_POLICER_DATABASE_HANDLE_SET(database_handle, is_ingress_idx, is_global,
                                                                     core_idx, database_idx);
                                    _SHR_POLICER_ID_SET(policer_id, database_handle, meter_idx);
                                    SHR_IF_ERR_EXIT((*cb) (unit, policer_id, &pol_cfg, user_data));
                                }
                            }
                        }
                    }
                    /** single bucket (special) bank*/
                    else
                    {
                        engine_id = (is_ingress_idx == TRUE) ?
                            dnx_data_meter.mem_mgmt.ingress_single_bucket_engine_get(unit) :
                            dnx_data_meter.mem_mgmt.egress_single_bucket_engine_get(unit);
                        engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);
                        nof_meters =
                            (engine_info->nof_counters *
                             dnx_data_meter.mem_mgmt.counters_buckets_ratio_per_engine_get(unit));
                        for (meter_idx = 0; meter_idx < nof_meters; meter_idx++)
                        {
                            /** check if meter id is configured */
                            SHR_IF_ERR_EXIT(dnx_meter_generic_profile_id_get
                                            (unit, core_idx, is_ingress_idx, database_idx, meter_idx, &profile_idx));
                            if (profile_idx != default_profile)
                            {
                                SHR_IF_ERR_EXIT(dnx_policer_mgmt_policer_get
                                                (unit, core_idx, is_global, is_ingress_idx, database_idx, meter_idx,
                                                 &pol_cfg));
                                _SHR_POLICER_DATABASE_HANDLE_SET(database_handle, is_ingress_idx, is_global, core_idx,
                                                                 database_idx);
                                _SHR_POLICER_ID_SET(policer_id, database_handle, meter_idx);
                                SHR_IF_ERR_EXIT((*cb) (unit, policer_id, &pol_cfg, user_data));
                            }
                        }
                    }
                }
            }
        }
        /** Global*/
        is_global = 1;
        is_ingress_idx = 1;
        database_idx = 0;
        /** check if global DB is enabled */
        SHR_IF_ERR_EXIT(dnx_meter_db.global_meter_enabled.get(unit, core_idx, &is_enabled));
        if (is_enabled == TRUE)
        {
            /**
             * Get nof of valid configured expansion groups
             * Global meter only supports expansion per L2-FWD Type
             */
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_nof_expansion_groups_get(unit, 0, core_idx, &nof_expansion_groups));
            for (meter_idx = bcmPolicerFwdTypeUc; meter_idx < nof_expansion_groups; meter_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_policer_mgmt_policer_get
                                (unit, core_idx, is_global, is_ingress_idx, database_idx, meter_idx, &pol_cfg));
                _SHR_POLICER_DATABASE_HANDLE_SET(database_handle, is_ingress_idx, is_global, core_idx, database_idx);
                _SHR_POLICER_ID_SET(policer_id, database_handle, meter_idx);
                SHR_IF_ERR_EXIT((*cb) (unit, policer_id, &pol_cfg, user_data));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_fairness_set(
    int unit,
    int policer_database_handle,
    int enable)
{
    int nof_databases;
    int database_idx;
    int is_global, is_ingress;
    bcm_core_t core_id;
    SHR_FUNC_INIT_VARS(unit);

    /** if specific database is given - set it. */
    if (_SHR_POLICER_IS_DATABASE_HANDLE(policer_database_handle) == TRUE)
    {
        core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);
        database_idx = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(policer_database_handle);
        is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(policer_database_handle);
        is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(policer_database_handle);
        if (is_global == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_meter_global_fairness_set(unit, core_id, enable));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_meter_generic_fairness_set(unit, core_id, is_ingress, database_idx, enable));
        }
    }
    /** if no specific database is given - set it all. */
    else
    {
        for (is_ingress = FALSE; is_ingress <= TRUE; is_ingress++)
        {
            nof_databases = (is_ingress == TRUE) ?
                dnx_data_meter.meter_db.nof_ingress_db_get(unit) : dnx_data_meter.meter_db.nof_egress_db_get(unit);
            for (database_idx = 0; database_idx < nof_databases; database_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_meter_generic_fairness_set(unit, BCM_CORE_ALL, is_ingress, database_idx, enable));
            }
        }
        SHR_IF_ERR_EXIT(dnx_meter_global_fairness_set(unit, BCM_CORE_ALL, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_fairness_get(
    int unit,
    int policer_database_handle,
    uint32 *enable)
{
    int database_idx;
    int is_global, is_ingress;
    bcm_core_t core_id;
    SHR_FUNC_INIT_VARS(unit);

    /** if specific database is given - get it. */
    if (_SHR_POLICER_IS_DATABASE_HANDLE(policer_database_handle) == TRUE)
    {
        core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);
        database_idx = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(policer_database_handle);
        is_global = _SHR_POLICER_DATABASE_HANDLE_IS_GLOBAL_GET(policer_database_handle);
        is_ingress = _SHR_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(policer_database_handle);
        if (is_global == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_meter_global_fairness_get(unit, core_id, enable));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_meter_generic_fairness_get(unit, core_id, is_ingress, database_idx, enable));
        }
    }
    /** if no specific database is given - get it from core-0 generic ingress database zero. */
    else
    {
        SHR_IF_ERR_EXIT(dnx_meter_generic_fairness_get(unit, 0, 0, 0, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_mgmt_meter_index_get(
    int unit,
    int core_id,
    int is_ingress,
    int database_id,
    int meter_pointer,
    int *meter_valid,
    int *meter_idx_base,
    int *meter_idx_size)
{
    uint32 db_created, expansion_enable, expansion_per_tc;
    int i, valid = FALSE, flags;
    SHR_FUNC_INIT_VARS(unit);

    /** verification stage */
    SHR_NULL_CHECK(meter_valid, _SHR_E_PARAM, "meter_valid");
    SHR_NULL_CHECK(meter_idx_base, _SHR_E_PARAM, "meter_idx_base");
    SHR_NULL_CHECK(meter_idx_size, _SHR_E_PARAM, "meter_idx_size");
    /** check for valid core - BCM_CORE_ALL not allowed */
    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.created.get
                    (unit, core_id, is_ingress, database_id, &db_created));

    if (db_created == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.expansion_enable.get
                        (unit, core_id, is_ingress, database_id, &expansion_enable));
        SHR_IF_ERR_EXIT(dnx_meter_db.generic_meter_info.db_info.expansion_is_per_tc.get
                        (unit, core_id, is_ingress, database_id, &expansion_per_tc));
        /** if expansion disable, there is only one meter */
        if (expansion_enable == FALSE)
        {
            *meter_idx_size = 1;
        }
        else
        {
            flags = (expansion_per_tc == TRUE) ? BCM_POLICER_EXPANSION_GROUPS_TC : 0;
            /** Get how many expansion groups are defined. the function can be called only if expansion is enabled */
            SHR_IF_ERR_EXIT(dnx_policer_mgmt_nof_expansion_groups_get(unit, flags, core_id, meter_idx_size));
        }

        /** all meters indexes corrsponding to the meter_pointer are consecutives. therefore, the base=mtr_ptr*size. */
        *meter_idx_base = meter_pointer * (*meter_idx_size);
        /** run over all meter indexes. If found one index that does not has corresponding entry in the HW memory,
            break the loop and return "invalid" */
        for (i = 0; i < (*meter_idx_size); i++)
        {
            SHR_IF_ERR_EXIT(dnx_meter_generic_index_verify
                            (unit, core_id, is_ingress, database_id, ((*meter_idx_base) + i), &valid));

            if (valid == FALSE)
            {
                break;
            }
        }
        *meter_valid = valid;
    }
    else
    {
        *meter_valid = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_policer_mgmt_tcsm_threshold_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int tcsm_threshold)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    uint32 per_tc_mantissa = 0;
    int per_tc_mantissa_max = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get relevant CORE_ID */
    core = (gport != 0) ? BCM_COSQ_GPORT_CORE_GET(gport) : DBAL_CORE_ALL;

    /** Get the maximum valie for the per TC mantissa from DBAL */
    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                    (unit, DBAL_TABLE_METER_TCSM_MANT_CONFIG, DBAL_FIELD_MANTISSA, FALSE, 0, 0, &per_tc_mantissa_max));

    /** The user threshold is in percentage. Calculate the mantissa value that is corresponding to it */
    per_tc_mantissa = (per_tc_mantissa_max * tcsm_threshold) / 100;

    /** Take a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_TCSM_MANT_CONFIG, &entry_handle_id));

    /** Set the key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, cosq);

    /** Set the result field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MANTISSA, INST_SINGLE, per_tc_mantissa);

    /** Preform the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_policer_mgmt_tcsm_threshold_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    int *tcsm_threshold)
{
    uint32 entry_handle_id;
    bcm_core_t core;
    uint32 per_tc_mantissa = 0;
    int per_tc_mantissa_max = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get relevant CORE_ID */
    core = (gport != 0) ? BCM_COSQ_GPORT_CORE_GET(gport) : DBAL_CORE_ALL;

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_TCSM_MANT_CONFIG, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, cosq);

    /** Request the result field */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MANTISSA, INST_SINGLE, &per_tc_mantissa);

    /** Preform the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /** Get the maximum valie for the per TC mantissa from DBAL */
    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                    (unit, DBAL_TABLE_METER_TCSM_MANT_CONFIG, DBAL_FIELD_MANTISSA, FALSE, 0, 0, &per_tc_mantissa_max));

    /** The user threshold is in percentage, calculate it */
    *tcsm_threshold = (per_tc_mantissa * 100) / per_tc_mantissa_max;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_policer_mgmt_tcsm_enable_set(
    int unit,
    int policer_database_handle,
    uint32 tcsm_enable)
{
    uint32 entry_handle_id;
    int database_id;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);
    database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(policer_database_handle);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    /** Setting value field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCSM_ENABLE, INST_SINGLE, tcsm_enable);
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_policer_mgmt_tcsm_enable_get(
    int unit,
    int policer_database_handle,
    uint32 *tcsm_enable)
{
    uint32 entry_handle_id;
    int database_id;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);
    database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(policer_database_handle);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    /** Getting value field */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TCSM_ENABLE, INST_SINGLE, tcsm_enable);
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_policer_mgmt_tcsm_use_pp_tc_set(
    int unit,
    int policer_database_handle,
    uint32 use_pp_tc)
{
    uint32 entry_handle_id;
    int database_id;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);
    database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(policer_database_handle);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    /** Setting value field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCSM_MANT_MAP_SEL, INST_SINGLE, use_pp_tc);
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_policer_mgmt_tcsm_use_pp_tc_get(
    int unit,
    int policer_database_handle,
    uint32 *use_pp_tc)
{
    uint32 entry_handle_id;
    int database_id;
    bcm_core_t core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    core_id = _SHR_POLICER_DATABASE_HANDLE_CORE_ID_GET(policer_database_handle);
    database_id = _SHR_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(policer_database_handle);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_METER_INGRESS_DB_GENERAL_CONFIG, &entry_handle_id));
    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DATABASE_ID, database_id);
    /** Getting value field */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TCSM_MANT_MAP_SEL, INST_SINGLE, use_pp_tc);
    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
