

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ


#include <bcm/cosq.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/types.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/cosq/latency/cosq_latency.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/cosq_latency_access.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/init/init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_latency.h>
#include <shared/utilex/utilex_u64.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/port.h>



#define DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION (4)
#define DNX_COSQ_LATENCY_LFSR_MACHINE_SIZE (32)

#define DNX_COSQ_LATENCY_END_TO_END_AQM_PKT_IS_CLASSIC_LQ (1)
#define DNX_COSQ_LATENCY_END_TO_END_AQM_PKT_IS_COUPLED_CLASSIC (2)
#define DNX_COSQ_LATENCY_END_TO_END_AQM_PKT_IS_COUPLED_LQ (4)

#define DNX_COSQ_LATENCY_DEFAULT_PROFILE (0)



void
dnx_cosq_latency_queue_template_print_cb(
    int unit,
    const void *data)
{
    dnx_cosq_latency_queue_template_t *profile_info = (dnx_cosq_latency_queue_template_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_STRING, NULL, "latency queue profile info:", NULL, NULL);
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "latency_map", profile_info->latency_map, NULL,
                                   "%d");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT32, "tc_map", profile_info->tc_map, NULL, "%d");
    SW_STATE_PRETTY_PRINT_FINISH();
    return;
}


static shr_error_e
dnx_cosq_latency_profile_create_verify(
    int unit,
    uint32 flags,
    int *gport_profile)
{
    uint32 legal_flags =
        BCM_COSQ_LATENCY_PROFILE_WITH_ID | BCM_COSQ_LATENCY_PROFILE_ID_EXTENSION | BCM_COSQ_LATENCY_END_TO_END;
    SHR_FUNC_INIT_VARS(unit);
    legal_flags = dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_valid_flow_profile_flag) ?
        (legal_flags | BCM_COSQ_LATENCY_PROFILE_BY_FLOW) : legal_flags;
    legal_flags = dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_valid_end_to_end_aqm_profile) ?
        (legal_flags | BCM_COSQ_LATENCY_END_TO_END_AQM) : legal_flags;

    SHR_NULL_CHECK(gport_profile, _SHR_E_PARAM, "gport_profile");
    SHR_MASK_VERIFY(flags, legal_flags, _SHR_E_PARAM, "some of the flags are not supported.\n");

    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_PROFILE_WITH_ID))
    {
        if ((BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(*gport_profile) == FALSE) &&
            (BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(*gport_profile) == FALSE) &&
            (BCM_GPORT_IS_END_TO_END_AQM_LATENCY_PROFILE(*gport_profile) == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport_profile %d", *gport_profile);
        }
        if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END) &&
            (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(*gport_profile)
             || BCM_GPORT_IS_END_TO_END_AQM_LATENCY_PROFILE(*gport_profile)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "flag BCM_COSQ_LATENCY_END_TO_END, but gport (=%d) is decoded as ingress/aqm latency",
                         *gport_profile);
        }
        if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END_AQM) &&
            (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(*gport_profile)
             || BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(*gport_profile)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "flag BCM_COSQ_LATENCY_END_TO_END_AQM, but gport (=%d) is decoded as ingress/e2e latency",
                         *gport_profile);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_flow_profile_valid_set(
    int unit,
    int profile_gport,
    int profile_id,
    int valid)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(profile_gport) == TRUE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_PROFILE, &entry_handle_id));
    }
    else if (BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(profile_gport) == TRUE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_PROFILE, &entry_handle_id));
    }
    else if (BCM_GPORT_IS_END_TO_END_AQM_LATENCY_PROFILE(profile_gport) == TRUE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, &entry_handle_id));
    }
    else
    {
        SHR_EXIT();
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_FLOW_PROFILE_VALID, INST_SINGLE, valid);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_aqm_packet_configuration(
    int unit,
    int clear_hw)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_PACKET_CLASSIFICATION, &entry_handle_id));
    
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_ECN_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_DE, INST_SINGLE, clear_hw ? 0 : 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_ECN_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_TYP, INST_SINGLE, clear_hw ? 0 : 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_PROFILE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_CQ, INST_SINGLE, clear_hw ? 0 : 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_LQ, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_PROFILE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_CQ, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_LQ, INST_SINGLE, clear_hw ? 0 : 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_PROFILE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_CQ, INST_SINGLE, clear_hw ? 0 : 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_LQ, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_PROFILE, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_CQ, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_LQ, INST_SINGLE, clear_hw ? 0 : 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

     
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L4_AQM_TYP_ENABLE, INST_SINGLE, clear_hw ? 0 : 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_aqm_cfg_maintain_average_latency(
    int unit,
    int clear_hw)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_MAINTAIN_AVERAGE_LATENCY, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PKT_TYPE_KEY,
                               DNX_COSQ_LATENCY_END_TO_END_AQM_PKT_IS_CLASSIC_LQ);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_NEW_AVG, INST_SINGLE, clear_hw ? FALSE : TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_NEW_INST, INST_SINGLE, clear_hw ? FALSE : TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PKT_TYPE_KEY,
                               DNX_COSQ_LATENCY_END_TO_END_AQM_PKT_IS_COUPLED_CLASSIC);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_NEW_AVG, INST_SINGLE, clear_hw ? FALSE : TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_NEW_INST, INST_SINGLE, clear_hw ? FALSE : TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PKT_TYPE_KEY,
                               DNX_COSQ_LATENCY_END_TO_END_AQM_PKT_IS_COUPLED_LQ);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_NEW_AVG, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_NEW_INST, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_aqm_ecn_probability_map_dbal_set(
    int unit,
    int mantissa,
    int exponent,
    int mantissa_new,
    int exponent_new)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_PROBABILITY_MAPPING, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MANTISSA, mantissa);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EXPONENT, exponent);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_MANTISSA, INST_SINGLE, mantissa_new);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_EXPONENT, INST_SINGLE, exponent_new);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_aqm_ecn_probability_map_calculation(
    int unit,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 mantissa_output = 0, exponent_output = 0, mantissa_input = 0, exponent_input = 0;
    uint32 max_mantissa = 0, max_exponent = 0, max_exponent_output = 0, coefficient_a = 0, exponent_b =
        0, mantissa_size = 0;
    int numerator_b = 0, denumerator_exp = 0;
    uint32 nomilize32_CQ;
    uint64 fnew_nomilize32_CQ64;
    uint32 fnew_nomilize32_CQ;
    uint32 temp_shift;
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);

    
    if (type == bcmCosqControlLatencyEcnProbConvertExponent)
    {
        exponent_b = arg;
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.probability_convert.exponent_b.set(unit, exponent_b));
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.probability_convert.coefficient_a.get(unit, &coefficient_a));
    }
    else if (type == bcmCosqControlLatencyEcnProbConvertCoeff)
    {
        coefficient_a = arg;
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.probability_convert.coefficient_a.set(unit, coefficient_a));
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.probability_convert.exponent_b.get(unit, &exponent_b));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
    }

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_LATENCY_EGRESS_PROBABILITY_MAPPING, DBAL_FIELD_MANTISSA, TRUE, 0, 0,
                     &field_info));
    max_mantissa = field_info.max_value;
    mantissa_size = field_info.field_nof_bits;
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_LATENCY_EGRESS_PROBABILITY_MAPPING, DBAL_FIELD_EXPONENT, TRUE, 0, 0,
                     &field_info));
    max_exponent = field_info.max_value;

    
    
    for (mantissa_input = 0; mantissa_input <= max_mantissa; mantissa_input++)
    {
        
        for (exponent_input = 0; exponent_input <= max_exponent; exponent_input++)
        {
             
            temp_shift = (exponent_input > mantissa_size) ? (exponent_input - mantissa_size) : 0;
            nomilize32_CQ = (1 << exponent_input) | (mantissa_input << temp_shift);

            COMPILER_64_SET(fnew_nomilize32_CQ64, 0, coefficient_a);
            numerator_b = exponent_b;
            denumerator_exp = (exponent_b - 1) * 32;
            if (exponent_b > 0)
            {
                while (denumerator_exp >= 0)
                {
                    while ((COMPILER_64_HI(fnew_nomilize32_CQ64) == 0) && (numerator_b > 0))
                    {
                        COMPILER_64_UMUL_32(fnew_nomilize32_CQ64, nomilize32_CQ);
                        numerator_b--;
                    }
                    if (denumerator_exp > 0)
                    {
                        COMPILER_64_UDIV_32(fnew_nomilize32_CQ64, 2);
                    }
                    denumerator_exp--;
                }
            }
            if (COMPILER_64_HI(fnew_nomilize32_CQ64) == 0)
            {
                fnew_nomilize32_CQ = COMPILER_64_LO(fnew_nomilize32_CQ64);
            }
            else
            {
                fnew_nomilize32_CQ = 0xFFFFFFFF;
            }
             
            max_exponent_output = (max_exponent + mantissa_size) < 32 ? max_exponent : (max_exponent - mantissa_size);
            SHR_IF_ERR_EXIT(utilex_break_complex_to_mnt_exp_round_down
                            (fnew_nomilize32_CQ, max_mantissa, max_exponent_output, 1,
                             utilex_power_of_2(mantissa_size), &mantissa_output, &exponent_output));

            SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_ecn_probability_map_dbal_set
                            (unit, mantissa_input, exponent_input, mantissa_output, exponent_output));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_aqm_cq_probability_default(
    int unit)
{
    uint32 exponent_b = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.probability_convert.exponent_b.get(unit, &exponent_b));
    SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_ecn_probability_map_calculation
                    (unit, bcmCosqControlLatencyEcnProbConvertExponent, exponent_b));

exit:
    SHR_FUNC_EXIT;
}


int
bcm_dnx_cosq_latency_profile_create(
    int unit,
    int flags,
    bcm_gport_t * profile_gport)
{
    uint32 res_flags = 0;
    int profile_id = 0;
    resource_tag_bitmap_extra_arguments_alloc_info_t alloc_info;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_profile_create_verify(unit, flags, profile_gport));

    if (!_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END)
        && !_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END_AQM))
    {
        sal_memset(&alloc_info, 0x0, sizeof(resource_tag_bitmap_extra_arguments_alloc_info_t));
        if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_PROFILE_WITH_ID))
        {
            profile_id = BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(*profile_gport);
            res_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
            res_flags |= RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG;
        }
        else
        {
            res_flags |= RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG;
        }

        alloc_info.tag = 0;

        
        if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_PROFILE_ID_EXTENSION)
            && !_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_PROFILE_WITH_ID))
        {

            alloc_info.tag = 1;
            rv = dnx_cosq_latency_db.ingress_latency_res.allocate_single(unit,
                                                                         res_flags | DNX_ALGO_RES_ALLOCATE_SIMULATION,
                                                                         (void *) &alloc_info, &profile_id);

            alloc_info.tag = (rv != _SHR_E_RESOURCE) ? 1 : 0;
        }

        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.ingress_latency_res.allocate_single
                        (unit, res_flags, (void *) &alloc_info, &profile_id));
        BCM_GPORT_INGRESS_LATENCY_PROFILE_SET((*profile_gport), profile_id);

    }
    else if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END))
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_PROFILE_WITH_ID))
        {
            profile_id = BCM_GPORT_END_TO_END_LATENCY_PROFILE_GET(*profile_gport);
            res_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
        }
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.end_to_end_latency_res.allocate_single(unit, res_flags, NULL, &profile_id));
        BCM_GPORT_END_TO_END_LATENCY_PROFILE_SET((*profile_gport), profile_id);
    }
    else if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END_AQM))
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_PROFILE_WITH_ID))
        {
            profile_id = BCM_GPORT_LATENCY_END_TO_END_AQM_LATENCY_PROFILE_GET(*profile_gport);
            res_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
        }
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.
                        end_to_end_aqm_latency_res.allocate_single(unit, res_flags, NULL, &profile_id));
        BCM_GPORT_LATENCY_END_TO_END_AQM_LATENCY_PROFILE_SET((*profile_gport), profile_id);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags given %d", flags);
    }
    
    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_PROFILE_BY_FLOW))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_flow_profile_valid_set(unit, *profile_gport, profile_id, 1));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}


int
bcm_dnx_cosq_latency_profile_destroy(
    int unit,
    int flags,
    bcm_gport_t profile_gport)
{
    int profile_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    if (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(profile_gport))
    {
        profile_id = BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(profile_gport);
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.ingress_latency_res.free_single(unit, profile_id));
    }
    else if (BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(profile_gport))
    {
        profile_id = BCM_GPORT_END_TO_END_LATENCY_PROFILE_GET(profile_gport);
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.end_to_end_latency_res.free_single(unit, profile_id));
    }
    else if (BCM_GPORT_IS_END_TO_END_AQM_LATENCY_PROFILE(profile_gport))
    {
        profile_id = BCM_GPORT_LATENCY_END_TO_END_AQM_LATENCY_PROFILE_GET(profile_gport);
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.end_to_end_aqm_latency_res.free_single(unit, profile_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport_profile %d", profile_gport);
    }
    
    if (dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_valid_flow_profile_flag))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_flow_profile_valid_set(unit, profile_gport, profile_id, 0));
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_res_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    resource_tag_bitmap_extra_arguments_create_info_t extra_info;
    resource_tag_bitmap_tag_info_t tag_info;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&extra_info, 0x0, sizeof(resource_tag_bitmap_extra_arguments_create_info_t));
    sal_memset(&tag_info, 0x0, sizeof(resource_tag_bitmap_tag_info_t));

    
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = dnx_data_latency.profile.ingress_nof_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_COSQ_LATENCY_INGRESS_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    
    
    extra_info.grain_size = data.nof_elements / 2;
    extra_info.max_tag_value = 1;
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.ingress_latency_res.create(unit, &data, (void *) &extra_info));
    tag_info.element = 0;
    tag_info.nof_elements = extra_info.grain_size;
    tag_info.tag = 0;
    tag_info.force_tag = FALSE;
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.ingress_latency_res.advanced_algorithm_info_set(unit, (void *) &tag_info));
    tag_info.element = extra_info.grain_size;
    tag_info.nof_elements = extra_info.grain_size;
    tag_info.tag = 1;
    tag_info.force_tag = FALSE;
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.ingress_latency_res.advanced_algorithm_info_set(unit, (void *) &tag_info));

     
    BCM_GPORT_INGRESS_LATENCY_PROFILE_SET(gport, DNX_COSQ_LATENCY_DEFAULT_PROFILE);
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_latency_profile_create(unit, BCM_COSQ_LATENCY_PROFILE_WITH_ID, &gport));

    
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = dnx_data_latency.profile.egress_nof_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_COSQ_LATENCY_END_TO_END_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.end_to_end_latency_res.create(unit, &data, NULL));
    
    BCM_GPORT_END_TO_END_LATENCY_PROFILE_SET(gport, DNX_COSQ_LATENCY_DEFAULT_PROFILE);
    SHR_IF_ERR_EXIT(bcm_dnx_cosq_latency_profile_create
                    (unit, (BCM_COSQ_LATENCY_PROFILE_WITH_ID | BCM_COSQ_LATENCY_END_TO_END), &gport));

    if (dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_valid_end_to_end_aqm_profile))
    {
        
        sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
        data.first_element = 0;
        data.nof_elements = dnx_data_latency.profile.egress_nof_get(unit);
        data.flags = 0;
        sal_strncpy(data.name, DNX_COSQ_LATENCY_END_TO_END_AQM_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.end_to_end_aqm_latency_res.create(unit, &data, NULL));
        
        BCM_GPORT_LATENCY_END_TO_END_AQM_LATENCY_PROFILE_SET(gport, DNX_COSQ_LATENCY_DEFAULT_PROFILE);
        SHR_IF_ERR_EXIT(bcm_dnx_cosq_latency_profile_create
                        (unit, (BCM_COSQ_LATENCY_PROFILE_WITH_ID | BCM_COSQ_LATENCY_END_TO_END_AQM), &gport));

        
        sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
        data.first_element = 0;
        data.nof_elements = dnx_data_latency.aqm.flows_nof_get(unit);
        data.flags = 0;
        sal_strncpy(data.name, DNX_COSQ_LATENCY_AQM_FLOW_ID_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.aqm_flow_id_res.create(unit, &data, NULL));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_latency_queue_tm_init(
    int unit)
{
    dnx_algo_template_create_data_t data;
    int nof_profiles;
    dbal_table_field_info_t field_info;
    dnx_cosq_latency_queue_template_t default_data = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_VOQ_QUARTET_PROFILE, DBAL_FIELD_PROFILE_ID,
                                               FALSE, 0, 0, &field_info));
    nof_profiles = field_info.max_value + 1;

    
    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE | DNX_ALGO_TEMPLATE_CREATE_ALLOW_DEFAULT_PROFILE_OVERRIDE;
    data.first_profile = 0;
    data.nof_profiles = nof_profiles;
    data.max_references = dnx_data_ipq.queues.nof_queues_get(unit) / DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION; 
    data.default_profile = 0;
    data.data_size = sizeof(dnx_cosq_latency_queue_template_t);
    data.default_data = (void *) &default_data;

    
    sal_strncpy(data.name, DNX_COSQ_LATENCY_QUARTET_QUEUES_TEMPLATE_NAME, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.queue_template.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_based_admission_extended_profiles_enable(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_BASED_ADMISSION_EXTENDED_PROFILES, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECO_ENABLE, INST_SINGLE,
                                 dnx_data_ingr_congestion.
                                 info.latency_based_admission_cgm_extended_profiles_enable_value_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_based_admission_reject_flow_profile(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_RJCT_SETTINGS, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RJCT_FLOW_PROFILE_NULL, INST_SINGLE,
                                 dnx_data_ingr_congestion.
                                 info.latency_based_admission_reject_flow_profile_value_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_ingress_timestamp_resolution_config_jr_mode(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_TIMESTAMP_RESOLUTION_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_JR_MODE_LEFT_SHIFT, INST_SINGLE,
                                 dnx_data_latency.ingress.jr_mode_latency_timestamp_left_shift_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.init(unit));

    
    SHR_IF_ERR_EXIT(dnx_cosq_latency_res_init(unit));
    
    SHR_IF_ERR_EXIT(dnx_cosq_latency_queue_tm_init(unit));

    if (dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_valid_end_to_end_aqm_profile))
    {
        
        SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_packet_configuration(unit, 0));
        
        SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_cfg_maintain_average_latency(unit, 0));
        
        SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_cq_probability_default(unit));
    }

    if (dnx_data_ingr_congestion.info.feature_get(unit, dnx_data_ingr_congestion_info_latency_based_admission))
    {
        if (dnx_data_ingr_congestion.
            info.feature_get(unit, dnx_data_ingr_congestion_info_latency_based_admission_cgm_extended_profiles_enable))
        {
            SHR_IF_ERR_EXIT(dnx_cosq_latency_based_admission_extended_profiles_enable(unit));
        }
        else
        {
            
            SHR_IF_ERR_EXIT(dnx_cosq_latency_based_admission_reject_flow_profile(unit));
        }
    }

    if (dnx_data_headers.system_headers.system_headers_mode_get(unit) !=
        DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        
        SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_timestamp_resolution_config_jr_mode(unit));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    

    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_ingress_max_get(
    int unit,
    bcm_gport_t gport,
    int max_count,
    bcm_cosq_max_latency_pkts_t * max_latency_pkts,
    int *actual_count)
{
    int index;
    uint32 entry_handle_id;
    dbal_table_field_info_t field_info;
    uint32 tc;
    uint32 ftmh_pp_dsp;
    uint32 modid;
    uint32 lat_val;
    uint32 flow_id;
    bcm_core_t core_id = _SHR_COSQ_GPORT_CORE_GET(gport);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_INGRESS_MAX_REPORT, DBAL_FIELD_INDEX,
                                               TRUE, 0, 0, &field_info));
    *actual_count = 0;

    
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_MAX_REPORT, &entry_handle_id));

    
    for (index = 0; index <= field_info.max_value; index++)
    {
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_VALUE, INST_SINGLE, &lat_val);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_FLOW_ID, INST_SINGLE, &flow_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TRF_CLS, INST_SINGLE, &tc);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_PORT_ID, INST_SINGLE, &ftmh_pp_dsp);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_MODID, INST_SINGLE, &modid);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if ((*actual_count) < max_count)
        {
            uint64 work_reg_64;
            COMPILER_64_SET(max_latency_pkts[(*actual_count)].latency, 0, lat_val);
            max_latency_pkts[(*actual_count)].latency_flow = flow_id;
            max_latency_pkts[(*actual_count)].cosq = (int) tc;
            BCM_GPORT_MODPORT_SET(max_latency_pkts[(*actual_count)].dest_gport, modid, ftmh_pp_dsp);
            
            COMPILER_64_ZERO(work_reg_64);
            if (COMPILER_64_GT(max_latency_pkts[(*actual_count)].latency, work_reg_64))
            {
                (*actual_count)++;
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_egress_max_get(
    int unit,
    bcm_gport_t gport,
    int max_count,
    bcm_cosq_max_latency_pkts_t * max_latency_pkts,
    int *actual_count)
{
    int index;
    uint32 entry_handle_id;
    dbal_table_field_info_t field_info;
    uint32 lat_val;
    uint32 tc;
    uint32 tm_port;
    uint32 mc;
    uint32 flow_id;
    bcm_core_t core_id = _SHR_COSQ_GPORT_CORE_GET(gport);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_EGRESS_MAX_REPORT, DBAL_FIELD_INDEX,
                                               TRUE, 0, 0, &field_info));
    *actual_count = 0;

    
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_MAX_REPORT, &entry_handle_id));

    
    for (index = 0; index <= field_info.max_value; index++)
    {
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_VALUE, INST_SINGLE, &lat_val);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_FLOW_ID, INST_SINGLE, &flow_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TC, INST_SINGLE, &tc);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TM_PORT_ID, INST_SINGLE, &tm_port);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_MC, INST_SINGLE, &mc);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if ((*actual_count) < max_count)
        {
            uint64 work_reg_64;
            COMPILER_64_SET(max_latency_pkts[(*actual_count)].latency, 0, lat_val);
            max_latency_pkts[(*actual_count)].latency_flow = flow_id;
            max_latency_pkts[(*actual_count)].cosq = (int) tc;
            
            SHR_IF_ERR_EXIT(dnx_stk_sys_tm_port_to_modport_gport_convert(unit, core_id, tm_port,
                                                                         &max_latency_pkts[*actual_count].dest_gport));

            max_latency_pkts[(*actual_count)].is_mc = (int) mc;
            
            COMPILER_64_ZERO(work_reg_64);
            if (COMPILER_64_GT(max_latency_pkts[(*actual_count)].latency, work_reg_64))
            {
                (*actual_count)++;
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_max_pkts_get_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    int max_count,
    bcm_cosq_max_latency_pkts_t * max_latency_pkts,
    int *actual_count)
{
    uint32 legal_flags = BCM_COSQ_LATENCY_END_TO_END;
    int core_id;
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(actual_count, _SHR_E_PARAM, "actual_count");
    SHR_NULL_CHECK(max_latency_pkts, _SHR_E_PARAM, "max_latency_pkts");

    SHR_MASK_VERIFY(flags, legal_flags, _SHR_E_PARAM, "some of the flags are not supported.\n");

    if (_SHR_COSQ_GPORT_IS_CORE(gport))
    {
        core_id = _SHR_COSQ_GPORT_CORE_GET(gport);
        
        DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport %d", gport);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END))
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_EGRESS_MAX_REPORT, DBAL_FIELD_INDEX,
                                                   TRUE, 0, 0, &field_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, DBAL_TABLE_LATENCY_INGRESS_MAX_REPORT, DBAL_FIELD_INDEX,
                                                   TRUE, 0, 0, &field_info));
    }
    if (max_count > (field_info.max_value + 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "max_count=%d, allow up to %d", max_count, (field_info.max_value + 1));
    }
exit:
    SHR_FUNC_EXIT;
}


int
bcm_dnx_cosq_max_latency_pkts_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    int max_count,
    bcm_cosq_max_latency_pkts_t * max_latency_pkts,
    int *actual_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_max_pkts_get_verify
                          (unit, gport, flags, max_count, max_latency_pkts, actual_count));
    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_LATENCY_END_TO_END))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_egress_max_get(unit, gport, max_count, max_latency_pkts, actual_count));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_max_get(unit, gport, max_count, max_latency_pkts, actual_count));
    }

exit:
    SHR_FUNC_EXIT;
}


static void
dnx_cosq_latency_probability_marking_alpha_value_calculation(
    int unit,
    uint64 delta_value,
    uint32 *shift_value)
{
    uint64 lfsr_calc;
    uint64 work_reg_64;
    UTILEX_U64 shift_value_log2_calc;
    
    COMPILER_64_ZERO(work_reg_64);
    if (COMPILER_64_GT(delta_value, work_reg_64))
    {
        COMPILER_64_UPOW(lfsr_calc, 2, DNX_COSQ_LATENCY_LFSR_MACHINE_SIZE);
        COMPILER_64_UDIV_64(lfsr_calc, delta_value);
        utilex_u64_clear(&shift_value_log2_calc);
        shift_value_log2_calc.arr[0] = COMPILER_64_LO(lfsr_calc);
        shift_value_log2_calc.arr[1] = COMPILER_64_HI(lfsr_calc);
        *shift_value = utilex_u64_log2_round_down(&shift_value_log2_calc);
    }
}


static shr_error_e
dnx_cosq_latency_ingress_probability_marking_update_alpha_value(
    int unit,
    int profile_id,
    int is_cni,
    int is_min,
    int threshold_value,
    uint32 *shift_value)
{
    uint32 entry_handle_id, threshold_value_get = 0;
    uint64 delta_value = COMPILER_64_INIT(0, 0);
    dbal_fields_e min_max_field_id;
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    if (is_min)
    {
        
        min_max_field_id = is_cni ? DBAL_FIELD_LAT_CNI_THRESHOLD : DBAL_FIELD_LAT_DROP_THRESHOLD;
        table_id = DBAL_TABLE_LATENCY_INGRESS_PROFILE;
    }
    else
    {
         
        min_max_field_id = is_cni ? DBAL_FIELD_MARK_CNI_MIN_THRESH : DBAL_FIELD_DROP_MIN_THRESH;
        table_id = DBAL_TABLE_LATENCY_INGRESS_PROBABILITY_MARK;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
    dbal_value_field32_request(unit, entry_handle_id, min_max_field_id, INST_SINGLE, &threshold_value_get);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    
    if (is_min)
    {
        COMPILER_64_SET(delta_value, 0, (threshold_value_get - threshold_value));
    }
    else
    {
        COMPILER_64_SET(delta_value, 0, (threshold_value - threshold_value_get));
    }
    
    dnx_cosq_latency_probability_marking_alpha_value_calculation(unit, delta_value, shift_value);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_ingress_probability_marking_set(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 entry_handle_id, is_enabled = 0, shift_value = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    if (!dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support)
        && (type != bcmCosqControlLatencyDropThreshold && type != bcmCosqControlLatencyEcnThreshold))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for ingress latency profile \n", type);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_PROBABILITY_MARK, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyDropThreshold:
            if (dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support))
            {
                
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DROP_EN, INST_SINGLE, &is_enabled);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
                if (is_enabled)
                {
                     
                    SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_probability_marking_update_alpha_value
                                    (unit, profile_id, 0, 0, arg, &shift_value));
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_SHIFT, INST_SINGLE,
                                                 shift_value);
                }
            }
            break;
        case bcmCosqControlLatencyEcnThreshold:
            if (dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support))
            {
                
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MARK_CNI_EN, INST_SINGLE, &is_enabled);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
                if (is_enabled)
                {
                     
                    SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_probability_marking_update_alpha_value
                                    (unit, profile_id, 1, 0, arg, &shift_value));
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MARK_CNI_SHIFT, INST_SINGLE,
                                                 shift_value);
                }
            }
            break;
        case bcmCosqControlLatencyDropProbEnable:
            if (!dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_EN, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyDropProbBaseThreshold:
            if (!dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            }
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DROP_EN, INST_SINGLE, &is_enabled);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            if (is_enabled)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_MIN_THRESH, INST_SINGLE, arg);
                
                SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_probability_marking_update_alpha_value
                                (unit, profile_id, 0, 1, arg, &shift_value));
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_SHIFT, INST_SINGLE, shift_value);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Latency drop probability not enabled, call API first with type=bcmCosqControlLatencyDropProbEnable \n");
            }
            break;
        case bcmCosqControlLatencyEcnProbEnable:
            if (!dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MARK_CNI_EN, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyEcnProbBaseThreshold:
            if (!dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            }
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MARK_CNI_EN, INST_SINGLE, &is_enabled);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            if (is_enabled)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MARK_CNI_MIN_THRESH, INST_SINGLE, arg);
                
                SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_probability_marking_update_alpha_value
                                (unit, profile_id, 1, 1, arg, &shift_value));
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MARK_CNI_SHIFT, INST_SINGLE,
                                             shift_value);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Latency mark ECN probability not enabled, call API first with type=bcmCosqControlLatencyEcnProbEnable \n");
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for ingress latency profile \n", type);
            break;
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_ingress_profile_set(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (type == bcmCosqControlLatencyBinThreshold)
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_PROFILE_BINS, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_ID, cosq);
    }
    else
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_PROFILE, &entry_handle_id));
    }
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyCounterEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_ENABLE, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyDropEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LATENCY_DROP_ENABLE, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEcnEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_ENABLE, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyDropThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_DROP_THRESHOLD, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            
            if (dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support))
            {
                SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_probability_marking_set(unit, profile_id, cosq, type, arg));
            }
            break;
        case bcmCosqControlLatencyEcnThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_THRESHOLD, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            
            if (dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support))
            {
                SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_probability_marking_set(unit, profile_id, cosq, type, arg));
            }
            break;
        case bcmCosqControlLatencyTrackDropPacket:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_DROP_ENABLE, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyTrack:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_ENABLE, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyBinThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_BIN_THRESHOLD, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyDropProbEnable:
        case bcmCosqControlLatencyDropProbBaseThreshold:
        case bcmCosqControlLatencyEcnProbEnable:
        case bcmCosqControlLatencyEcnProbBaseThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_probability_marking_set(unit, profile_id, cosq, type, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for ingress latency profile \n", type);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_latency_end_to_end_profile_set_arg_validate(
    int unit,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    if (type == bcmCosqControlLatencyTrack)
    {
        if (arg != 0 && arg != 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid mode %d for type %d \n", arg, type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_profile_set(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_end_to_end_profile_set_arg_validate(unit, type, arg));

    if (type == bcmCosqControlLatencyBinThreshold)
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_PROFILE_BINS, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_ID, cosq);
    }
    else
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_PROFILE, &entry_handle_id));
    }
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    
    if (dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_valid_end_to_end_aqm_profile))
    {
        if (type == bcmCosqControlLatencyDropEnable || type == bcmCosqControlLatencyEcnEnable
            || type == bcmCosqControlLatencyDropThreshold || type == bcmCosqControlLatencyEcnThreshold)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "type=%d not supported for end to end latency profile, use end to end AQM profile \n", type);
        }
    }

    switch (type)
    {
        case bcmCosqControlLatencyCounterEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyDropEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LATENCY_DROP_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyEcnEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyDropThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_DROP_THRESHOLD, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyEcnThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_THRESHOLD, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyEgressCounterCommand:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_COMMAND, INST_SINGLE,
                                         STAT_PP_PROFILE_ID_GET(arg));
            break;
        case bcmCosqControlLatencyTrack:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyBinThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_BIN_THRESHOLD, INST_SINGLE, arg);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end latency profile \n", type);
            break;
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_aqm_probability_marking_update_alpha_value(
    int unit,
    int profile_id,
    int is_cni,
    int is_min_changed,
    int pkt_is_typ,
    int threshold_value,
    uint32 *shift_value)
{
    uint32 entry_handle_id, threshold_value_get = 0;
    uint64 delta_value = COMPILER_64_INIT(0, 0);
    dbal_fields_e min_max_field_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    
    if (pkt_is_typ)
    {
        if (is_min_changed)
        {
            
            min_max_field_id = is_cni ? DBAL_FIELD_CNI_MAX_THRESH : DBAL_FIELD_DROP_MAX_THRESH;
        }
        else
        {
            
            min_max_field_id = is_cni ? DBAL_FIELD_CNI_MIN_THRESH : DBAL_FIELD_DROP_MIN_THRESH;
        }
    }
    else
    {
        if (is_min_changed)
        {
            
            min_max_field_id = is_cni ? DBAL_FIELD_LQ_CNI_MAX_THRESH : DBAL_FIELD_DROP_MAX_THRESH;
        }
        else
        {
             
            min_max_field_id = is_cni ? DBAL_FIELD_LQ_CNI_PROB_MIN_THRESH : DBAL_FIELD_DROP_MIN_THRESH;
        }
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
    dbal_value_field32_request(unit, entry_handle_id, min_max_field_id, INST_SINGLE, &threshold_value_get);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    if (is_min_changed)
    {
        COMPILER_64_SET(delta_value, 0, (threshold_value_get - threshold_value));
    }
    else
    {
        COMPILER_64_SET(delta_value, 0, (threshold_value - threshold_value_get));
    }
    
    dnx_cosq_latency_probability_marking_alpha_value_calculation(unit, delta_value, shift_value);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_aqm_probability_marking_set(
    int unit,
    int profile_id,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 entry_handle_id, is_enabled = 0, pkt_is_typ = 0, shift_value = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_L4_AQM_TYP_ENABLE, INST_SINGLE, &pkt_is_typ);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyDropEnable:
            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_MAX_ENABLE, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEcnEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id,
                                         (pkt_is_typ) ? DBAL_FIELD_CNI_MAX_ENABLE : DBAL_FIELD_LQ_CNI_MAX_ENABLE,
                                         INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyDropThreshold:
            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_MAX_THRESH, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, entry_handle_id));
            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DROP_PROB_ENABLE, INST_SINGLE, &is_enabled);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, entry_handle_id));
            if (is_enabled)
            {
                SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_probability_marking_update_alpha_value
                                (unit, profile_id, 0, 0, pkt_is_typ, arg, &shift_value));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_MIN_THRESH_SHIFT, INST_SINGLE,
                                             shift_value);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            break;
        case bcmCosqControlLatencyEcnThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNI_MAX_THRESH, INST_SINGLE, arg);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LQ_CNI_MAX_THRESH, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, entry_handle_id));
            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
            dbal_value_field32_request(unit, entry_handle_id,
                                       (pkt_is_typ) ? DBAL_FIELD_CNI_PROB_ENABLE : DBAL_FIELD_LQ_CNI_PROB_ENABLE,
                                       INST_SINGLE, &is_enabled);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, entry_handle_id));
            if (is_enabled)
            {
                
                SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_probability_marking_update_alpha_value
                                (unit, profile_id, 1, 0, 0, arg, &shift_value));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
                dbal_entry_value_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_LQ_CNI_PROB_MIN_SHIFT, INST_SINGLE, shift_value);
                SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_probability_marking_update_alpha_value
                                (unit, profile_id, 1, 0, 1, arg, &shift_value));
                dbal_entry_value_field32_set
                    (unit, entry_handle_id, DBAL_FIELD_CNI_MIN_THRESH_SHIFT, INST_SINGLE, shift_value);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            break;
        case bcmCosqControlLatencyEcnProbEnable:
            if (!dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            }
            dbal_entry_value_field32_set(unit, entry_handle_id,
                                         (pkt_is_typ) ? DBAL_FIELD_CNI_PROB_ENABLE : DBAL_FIELD_LQ_CNI_PROB_ENABLE,
                                         INST_SINGLE, arg);
            if (!pkt_is_typ)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CQ_CNI_PROB_ENABLE, INST_SINGLE, arg);
            }
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEcnProbBaseThreshold:
            if (!dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CNI_MIN_THRESH, INST_SINGLE, arg);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LQ_CNI_PROB_MIN_THRESH, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, entry_handle_id));
            
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
            dbal_value_field32_request(unit, entry_handle_id,
                                       (pkt_is_typ) ? DBAL_FIELD_CNI_PROB_ENABLE : DBAL_FIELD_LQ_CNI_PROB_ENABLE,
                                       INST_SINGLE, &is_enabled);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, entry_handle_id));
            if (is_enabled)
            {
                
                SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_probability_marking_update_alpha_value
                                (unit, profile_id, 1, 1, 0, arg, &shift_value));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);
                dbal_entry_value_field32_set(unit, entry_handle_id,
                                             DBAL_FIELD_LQ_CNI_PROB_MIN_SHIFT, INST_SINGLE, shift_value);
                SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_probability_marking_update_alpha_value
                                (unit, profile_id, 1, 1, 1, arg, &shift_value));
                dbal_entry_value_field32_set(unit, entry_handle_id,
                                             DBAL_FIELD_CNI_MIN_THRESH_SHIFT, INST_SINGLE, shift_value);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "type=%d not supported - cni marking probability mechanism is not enabled for this profile \n",
                             type);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_aqm_profile_set(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyDropEnable:
        case bcmCosqControlLatencyEcnEnable:
        case bcmCosqControlLatencyDropThreshold:
        case bcmCosqControlLatencyEcnThreshold:
        case bcmCosqControlLatencyEcnProbEnable:
        case bcmCosqControlLatencyEcnProbBaseThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_probability_marking_set(unit, profile_id, type, arg));
            break;
        case bcmCosqControlLatencyEcnProbConvertExponent:
        case bcmCosqControlLatencyEcnProbConvertCoeff:
            SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_ecn_probability_map_calculation(unit, type, arg));
            break;
        case bcmCosqControlLatencyEgressCoupledAqmMode:
            if (arg == TRUE)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L4_AQM_LQ_ENABLE, INST_SINGLE, 1);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L4_AQM_CQ_ENABLE, INST_SINGLE, 1);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L4_AQM_TYP_ENABLE, INST_SINGLE, 0);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L4_AQM_LQ_ENABLE, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L4_AQM_CQ_ENABLE, INST_SINGLE, 0);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L4_AQM_TYP_ENABLE, INST_SINGLE, 1);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            break;
        case bcmCosqControlLatencyEgressAqmL4sEcnClassificationThresholdMax:
            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LQ_CNI_CLASSIFICATION_THRESH, INST_SINGLE,
                                         arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEgressAqmAverageWeight:
            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AVERAGE_WEIGHT, INST_SINGLE, arg);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_profile_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_profile_set
                        (unit, BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
    else if (BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_profile_set
                        (unit, BCM_GPORT_END_TO_END_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
    else if (BCM_GPORT_IS_END_TO_END_AQM_LATENCY_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_profile_set
                        (unit, BCM_GPORT_LATENCY_END_TO_END_AQM_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Given gport is not latency related, configure latency profile in order to use cosq control type %d",
                     type);
    }

exit:
    SHR_FUNC_EXIT;

}


static shr_error_e
dnx_cosq_latency_ingress_probability_marking_get(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    if (!dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_probabilistic_mechanism_support))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for ingress latency profile \n", type);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_PROBABILITY_MARK, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyDropProbEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DROP_EN, INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyDropProbBaseThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DROP_MIN_THRESH, INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyEcnProbEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MARK_CNI_EN, INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyEcnProbBaseThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MARK_CNI_MIN_THRESH, INST_SINGLE,
                                       (uint32 *) arg);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for ingress latency profile \n", type);
            break;
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_ingress_profile_get(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (type == bcmCosqControlLatencyBinThreshold)
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_PROFILE_BINS, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_ID, cosq);
    }
    else
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_INGRESS_PROFILE, &entry_handle_id));
    }
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyCounterEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_ENABLE, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyDropEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LATENCY_DROP_ENABLE, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEcnEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_ENABLE, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyDropThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_DROP_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEcnThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyTrackDropPacket:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_DROP_ENABLE, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyTrack:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_ENABLE, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyBinThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_BIN_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyDropProbEnable:
        case bcmCosqControlLatencyDropProbBaseThreshold:
        case bcmCosqControlLatencyEcnProbEnable:
        case bcmCosqControlLatencyEcnProbBaseThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_probability_marking_get(unit, profile_id, cosq, type, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for ingress latency profile \n", type);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_profile_get(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (type == bcmCosqControlLatencyBinThreshold)
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_PROFILE_BINS, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_ID, cosq);
    }
    else
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_PROFILE, &entry_handle_id));
    }
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    
    if (dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_valid_end_to_end_aqm_profile))
    {
        if (type == bcmCosqControlLatencyDropEnable || type == bcmCosqControlLatencyEcnEnable
            || type == bcmCosqControlLatencyDropThreshold || type == bcmCosqControlLatencyEcnThreshold)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "type=%d not supported for end to end latency profile, use end to end AQM profile \n", type);
        }
    }
    switch (type)
    {
        case bcmCosqControlLatencyCounterEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_ENABLE, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyDropEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LATENCY_DROP_ENABLE, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEcnEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_ENABLE, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyDropThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_DROP_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEcnThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_CNI_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEgressCounterCommand:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_STAT_COMMAND, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            STAT_PP_ENGINE_PROFILE_SET(*arg, *arg, bcmStatCounterInterfaceEgressTransmitPp);
            break;
        case bcmCosqControlLatencyTrack:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_TRACK_ENABLE, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyBinThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_BIN_THRESHOLD, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end latency profile \n", type);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_aqm_probability_marking_get(
    int unit,
    int profile_id,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32 entry_handle_id, pkt_is_typ = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_L4_AQM_TYP_ENABLE, INST_SINGLE, &pkt_is_typ);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyDropEnable:
            
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DROP_MAX_ENABLE, INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyEcnEnable:
            dbal_value_field32_request(unit, entry_handle_id,
                                       (pkt_is_typ) ? DBAL_FIELD_CNI_MAX_ENABLE : DBAL_FIELD_LQ_CNI_MAX_ENABLE,
                                       INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyDropThreshold:
            
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DROP_MAX_THRESH, INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyEcnThreshold:
            dbal_value_field32_request(unit, entry_handle_id,
                                       (pkt_is_typ) ? DBAL_FIELD_CNI_MAX_THRESH : DBAL_FIELD_LQ_CNI_MAX_THRESH,
                                       INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyEcnProbEnable:
            dbal_value_field32_request(unit, entry_handle_id,
                                       (pkt_is_typ) ? DBAL_FIELD_CNI_PROB_ENABLE : DBAL_FIELD_LQ_CNI_PROB_ENABLE,
                                       INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyEcnProbBaseThreshold:
            dbal_value_field32_request(unit, entry_handle_id,
                                       (pkt_is_typ) ? DBAL_FIELD_CNI_MIN_THRESH : DBAL_FIELD_LQ_CNI_PROB_MIN_THRESH,
                                       INST_SINGLE, (uint32 *) arg);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            break;
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_end_to_end_aqm_profile_get(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32 entry_handle_id, field_get;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_EGRESS_DROP_CNI_PROFILE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_id);

    switch (type)
    {
        case bcmCosqControlLatencyDropEnable:
        case bcmCosqControlLatencyEcnEnable:
        case bcmCosqControlLatencyDropThreshold:
        case bcmCosqControlLatencyEcnThreshold:
        case bcmCosqControlLatencyEcnProbEnable:
        case bcmCosqControlLatencyEcnProbBaseThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_probability_marking_get(unit, profile_id, type, arg));
            break;
        case bcmCosqControlLatencyEcnProbConvertExponent:
            SHR_IF_ERR_EXIT(dnx_cosq_latency_db.probability_convert.exponent_b.get(unit, (uint32 *) arg));
            break;
        case bcmCosqControlLatencyEcnProbConvertCoeff:
            SHR_IF_ERR_EXIT(dnx_cosq_latency_db.probability_convert.coefficient_a.get(unit, (uint32 *) arg));
            break;
        case bcmCosqControlLatencyEgressCoupledAqmMode:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_L4_AQM_TYP_ENABLE, INST_SINGLE, &field_get);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            *arg = !field_get; 
            break;
        case bcmCosqControlLatencyEgressAqmL4sEcnClassificationThresholdMax:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LQ_CNI_CLASSIFICATION_THRESH, INST_SINGLE,
                                       (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        case bcmCosqControlLatencyEgressAqmAverageWeight:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_AVERAGE_WEIGHT, INST_SINGLE, (uint32 *) arg);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end latency profile \n", type);
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_profile_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_profile_get
                        (unit, BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
    else if (BCM_GPORT_IS_END_TO_END_LATENCY_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_profile_get
                        (unit, BCM_GPORT_END_TO_END_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
    else if (BCM_GPORT_IS_END_TO_END_AQM_LATENCY_PROFILE(port))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_end_to_end_aqm_profile_get
                        (unit, BCM_GPORT_LATENCY_END_TO_END_AQM_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Given gport is not latency related, configure latency profile in order to use cosq control type %d",
                     type);
    }

exit:
    SHR_FUNC_EXIT;

}


static shr_error_e
dnx_cosq_latency_queue_quartet_profile_id_get(
    int unit,
    int core,
    int qid,
    uint32 *profile_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_QUARTET_PROFILE, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUARTET_QUEUE,
                               (qid / DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_latency_ingress_queue_map_verify(
    int unit,
    int core,
    int flags,
    int lat_profile_id,
    int qid)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (lat_profile_id % DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid lat_profile_id (=%d)", lat_profile_id);
    }
    
    if (qid % DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid queue_id (=%d)", qid);
    }

    DNXCMN_CORE_VALIDATE(unit, core, TRUE);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_latency_ingress_queue_profile_set(
    int unit,
    int profile,
    dnx_cosq_latency_queue_template_t * info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_QUARTET_PROFILE_MAP, &entry_handle_id));

    

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUARTET_PROFILE, profile);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_FLOW_PROFILE, INST_SINGLE, info->latency_map);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_MAP, INST_SINGLE, info->tc_map);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_latency_ingress_queue_profile_info_get(
    int unit,
    int profile,
    dnx_cosq_latency_queue_template_t * info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_QUARTET_PROFILE_MAP, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUARTET_PROFILE, profile);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_FLOW_PROFILE, INST_SINGLE,
                               (uint32 *) &info->latency_map);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TC_MAP, INST_SINGLE, (uint32 *) &info->tc_map);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_latency_ingress_queue_profile_map(
    int unit,
    int core,
    int qid,
    int profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_QUARTET_PROFILE, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QUARTET_QUEUE,
                               (qid / DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION));

    if (core == BCM_CORE_ALL)
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    else
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, INST_SINGLE, profile);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_ingress_queue_map_set(
    int unit,
    int core,
    int flags,
    int qid,
    int lat_profile_id)
{
    uint32 quartet_profile_id;
    dnx_cosq_latency_queue_template_t profile_info;
    int new_profile;
    uint8 first_reference, last_reference;
    int curr_core;
    int nof_cores;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_ingress_queue_map_verify(unit, core, flags, lat_profile_id, qid));

    curr_core = (core == BCM_CORE_ALL) ? 0 : core;
    nof_cores = (core == BCM_CORE_ALL) ? dnx_data_device.general.nof_cores_get(unit) : curr_core + 1;

    for (; curr_core < nof_cores; curr_core++)
    {
        
        SHR_IF_ERR_EXIT(dnx_cosq_latency_queue_quartet_profile_id_get(unit, curr_core, qid, &quartet_profile_id));

        
        SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_queue_profile_info_get(unit, quartet_profile_id, &profile_info));

        
        profile_info.latency_map = lat_profile_id / DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION;

        
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.queue_template.exchange(unit, 0, &profile_info,
                                                                    quartet_profile_id, NULL, &new_profile,
                                                                    &first_reference, &last_reference));

        
        if (first_reference == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_queue_profile_set(unit, new_profile, &profile_info));
        }

        
        SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_queue_profile_map(unit, curr_core, qid, new_profile));

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_symmetric_profile_cheack(
    int unit,
    int qid,
    int *is_symmetric_profile)
{
    uint32 quartet_profile_id;
    uint32 tmp_quartet_profile_id;
    int nof_cores;
    int core;
    SHR_FUNC_INIT_VARS(unit);

    core = 0;
    SHR_IF_ERR_EXIT(dnx_cosq_latency_queue_quartet_profile_id_get(unit, core, qid, &quartet_profile_id));

    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    *is_symmetric_profile = 1;

    for (core = 1; core < nof_cores; core++)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_queue_quartet_profile_id_get(unit, core, qid, &tmp_quartet_profile_id));

        if (tmp_quartet_profile_id != quartet_profile_id)
        {
            *is_symmetric_profile = 0;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_latency_ingress_queue_map_get_verify(
    int unit,
    int core,
    int qid)
{
    int is_symmetric_profile;
    SHR_FUNC_INIT_VARS(unit);

    if (core == BCM_CORE_ALL)
    {
        dnx_cosq_symmetric_profile_cheack(unit, qid, &is_symmetric_profile);

        if (!is_symmetric_profile)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "get profile for asymetric configuration requier a specific core, got BCM_CORE_ALL");
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_ingress_queue_map_get(
    int unit,
    int core,
    int flags,
    int qid,
    int *lat_profile_id)
{
    uint32 quartet_profile_id;
    dnx_cosq_latency_queue_template_t info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_ingress_queue_map_get_verify(unit, core, qid));

    core = (core == BCM_CORE_ALL) ? 0 : core;
    
    SHR_IF_ERR_EXIT(dnx_cosq_latency_queue_quartet_profile_id_get(unit, core, qid, &quartet_profile_id));

    SHR_IF_ERR_EXIT(dnx_cosq_latency_ingress_queue_profile_info_get(unit, quartet_profile_id, &info));
    *lat_profile_id = info.latency_map * DNX_COSQ_LATENCY_QUEUE_QUARTET_RESOLUTION;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_end_to_end_per_port_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_END_TO_END_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_PER_FLOW_ENABLE, INST_SINGLE, (enable == FALSE));
    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_end_to_end_per_port_get(
    int unit,
    int *enable)
{
    uint32 entry_handle_id;
    uint32 flow_enable;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_END_TO_END_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAT_PER_FLOW_ENABLE, INST_SINGLE, &flow_enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *enable = (flow_enable == FALSE);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_latency_end_to_end_port_map_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport_to_map,
    int profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flags (=%d)", flags);
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_end_to_end_port_map_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport_to_map,
    int profile_id)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_end_to_end_port_map_verify(unit, flags, gport_to_map, profile_id));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport_to_map, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
        
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LATENCY_FLOW_PROFILE, INST_SINGLE, profile_id);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_end_to_end_port_map_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport_to_map,
    int *profile_id)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport_to_map, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);
    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LATENCY_FLOW_PROFILE, INST_SINGLE,
                               (uint32 *) profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_voq_settings_set(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    if (type == bcmCosqControlLatencyVoqRejectBinThreshold)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_RANGES, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_FLOW_PROFILE, profile_id);
    }
    
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_RJCT_SETTINGS, &entry_handle_id));
    }

    switch (type)
    {
        case bcmCosqControlLatencyVoqRejectBinThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_THRESHOLD, cosq, arg);
            break;
        case bcmCosqControlLatencyVoqRejectFilterMulticast:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RJCT_FILTER_MC, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyVoqRejectFilterSniff:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RJCT_FILTER_SNIFF, INST_SINGLE, arg);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for ingress latency profile \n", type);
            break;
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_voq_settings_get(
    int unit,
    int profile_id,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    if (type == bcmCosqControlLatencyVoqRejectBinThreshold)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_RANGES, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAT_FLOW_PROFILE, profile_id);
    }
    
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_RJCT_SETTINGS, &entry_handle_id));
    }
    switch (type)
    {
        case bcmCosqControlLatencyVoqRejectBinThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RANGE_THRESHOLD, cosq, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyVoqRejectFilterMulticast:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RJCT_FILTER_MC, INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyVoqRejectFilterSniff:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RJCT_FILTER_SNIFF, INST_SINGLE,
                                       (uint32 *) arg);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for ingress latency profile \n", type);
            break;
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_config_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(port)
        && (type == bcmCosqControlLatencyVoqRejectBinThreshold
            || type == bcmCosqControlLatencyVoqRejectFilterMulticast
            || type == bcmCosqControlLatencyVoqRejectFilterSniff))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_voq_settings_set
                        (unit, BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
    
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_profile_set(unit, port, cosq, type, arg));
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_config_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (BCM_GPORT_IS_INGRESS_LATENCY_PROFILE(port)
        && (type == bcmCosqControlLatencyVoqRejectBinThreshold
            || type == bcmCosqControlLatencyVoqRejectFilterMulticast
            || type == bcmCosqControlLatencyVoqRejectFilterSniff))
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_voq_settings_get
                        (unit, BCM_GPORT_INGRESS_LATENCY_PROFILE_GET(port), cosq, type, arg));
    }
    
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_latency_profile_get(unit, port, cosq, type, arg));
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_cosq_param_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    uint32 key,
    bcm_cosq_generic_control_t type,
    uint32 arg)
{
    uint32 entry_handle_id, rate_class = 0, granularity = 0;
    int bin = 0, dp = 0, reject_prob = 0;
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    granularity = dnx_data_ingr_congestion.info.bytes_threshold_granularity_get(unit);
    rate_class = BCM_GPORT_PROFILE_GET(gport);
    dp = (type ==
          bcmCosqGenericControlLatencyVoqRejectProb) ? BCM_COSQ_GENERIC_KEY_LAT_BIN_COLOR_COLOR_GET(key) :
        BCM_COSQ_GENERIC_KEY_COLOR_GET(key);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_DISCARD_PARAMS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    switch (type)
    {
        case bcmCosqGenericControlLatencyVoqRejectEnable:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, arg);
            break;
        case bcmCosqGenericControlLatencyVoqRejectProb:
            bin = BCM_COSQ_GENERIC_KEY_LAT_BIN_COLOR_BIN_GET(key);
            
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                            (unit, DBAL_TABLE_LATENCY_VOQ_DISCARD_PARAMS, DBAL_FIELD_RJCT_PROB, FALSE, 0, bin,
                             &field_info));
            reject_prob = field_info.max_value - (arg * UTILEX_DIV_ROUND_DOWN(field_info.max_value, 100));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RJCT_PROB, bin, reject_prob);
            break;
        case bcmCosqGenericControlLatencyVoqRejectMinThreshold:
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_THRESHOLD, INST_SINGLE,
                                         UTILEX_DIV_ROUND_DOWN(arg, granularity));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for cosq generic latency settings \n", type);
            break;
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_cosq_param_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    uint32 key,
    bcm_cosq_generic_control_t type,
    uint32 *arg)
{
    uint32 entry_handle_id, rate_class = 0, granularity = 0;
    int bin = 0, dp = 0;
    uint32 reject_probability_hw = 0;
    dbal_table_field_info_t field_info;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

     
    granularity = dnx_data_ingr_congestion.info.bytes_threshold_granularity_get(unit);
    rate_class = BCM_GPORT_PROFILE_GET(gport);
    dp = (type ==
          bcmCosqGenericControlLatencyVoqRejectProb) ? BCM_COSQ_GENERIC_KEY_LAT_BIN_COLOR_COLOR_GET(key) :
        BCM_COSQ_GENERIC_KEY_COLOR_GET(key);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LATENCY_VOQ_DISCARD_PARAMS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    switch (type)
    {
        case bcmCosqGenericControlLatencyVoqRejectEnable:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqGenericControlLatencyVoqRejectProb:
            bin = BCM_COSQ_GENERIC_KEY_LAT_BIN_COLOR_BIN_GET(key);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RJCT_PROB, bin, &reject_probability_hw);
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                            (unit, DBAL_TABLE_LATENCY_VOQ_DISCARD_PARAMS, DBAL_FIELD_RJCT_PROB, FALSE, 0, bin,
                             &field_info));
            break;
        case bcmCosqGenericControlLatencyVoqRejectMinThreshold:
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ADMIT_THRESHOLD, INST_SINGLE, (uint32 *) arg);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for cosq generic latency settings \n", type);
            break;
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (type == bcmCosqGenericControlLatencyVoqRejectProb)
    {
        
        *arg =
            UTILEX_DIV_ROUND_DOWN((field_info.max_value - reject_probability_hw),
                                  UTILEX_DIV_ROUND_DOWN(field_info.max_value, 100));
    }
    if (type == bcmCosqGenericControlLatencyVoqRejectMinThreshold)
    {
        *arg = *arg * granularity;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_aqm_flow_profile_mapping_verify(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg,
    uint8 is_set)
{
    int flow_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_valid_end_to_end_aqm_profile))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "AQM profile not supported for this device.\n");
    }

    if (type == bcmCosqControlLatencyEgressAqmFlowIdToProfile)
    {
        
        flow_id = BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(port);
        SHR_RANGE_VERIFY(flow_id, 0, dnx_data_latency.aqm.flows_nof_get(unit) - 1, _SHR_E_PARAM,
                         "AQM Flow ID %d is out of range 0-%d\n", flow_id,
                         dnx_data_latency.aqm.flows_nof_get(unit) - 1);
        if (is_set == TRUE)
        {
            
            SHR_RANGE_VERIFY(arg, 0, dnx_data_latency.aqm.profiles_nof_get(unit) - 1, _SHR_E_PARAM,
                             "AQM profile %d is out of range 0-%d\n", arg,
                             dnx_data_latency.aqm.profiles_nof_get(unit) - 1);
        }
    }

    if (type == bcmCosqControlLatencyEgressAqmFlowId)
    {
        
        SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, port));
        
        SHR_RANGE_VERIFY(cosq, BCM_COS_MIN, BCM_COS_MAX, _SHR_E_PARAM, "COSQ %d is out of range %d-%d\n", cosq,
                         BCM_COS_MIN, BCM_COS_MAX);
        if (is_set == TRUE)
        {
            
            SHR_RANGE_VERIFY(arg, 0, dnx_data_latency.aqm.flows_nof_get(unit) - 1, _SHR_E_PARAM,
                             "AQM Flow ID %d is out of range 0-%d\n", arg,
                             dnx_data_latency.aqm.flows_nof_get(unit) - 1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_aqm_flow_profile_mapping_set(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 entry_handle_id, tm_port;
    int core_id;
    bcm_port_t logical_port = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_aqm_flow_profile_mapping_verify(unit, port, cosq, type, arg, TRUE));

    switch (type)
    {
        case bcmCosqControlLatencyEgressAqmFlowId:
            
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_LATENCY_EGRESS_L4Q_MAP_AQM_FLOW_ID_PORT_TC, &entry_handle_id));
            
            SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core_id, &tm_port));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, cosq);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_ID, INST_SINGLE, arg);
            break;
        case bcmCosqControlLatencyEgressAqmFlowIdToProfile:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_LATENCY_EGRESS_L4Q_MAP_AQM_FLOW_ID_TO_PROFILE, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_ID,
                                       BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(port));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_PROFILE, INST_SINGLE, arg);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            break;
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_aqm_flow_profile_mapping_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    int core_id;
    uint32 entry_handle_id, tm_port;
    bcm_port_t logical_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_aqm_flow_profile_mapping_verify(unit, port, cosq, type, *arg, FALSE));

    switch (type)
    {
        case bcmCosqControlLatencyEgressAqmFlowId:
            
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_LATENCY_EGRESS_L4Q_MAP_AQM_FLOW_ID_PORT_TC, &entry_handle_id));
            
            SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, port, &logical_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core_id, &tm_port));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, tm_port);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, cosq);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_ID, INST_SINGLE, (uint32 *) arg);
            break;
        case bcmCosqControlLatencyEgressAqmFlowIdToProfile:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_LATENCY_EGRESS_L4Q_MAP_AQM_FLOW_ID_TO_PROFILE, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_ID,
                                       BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(port));
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_L4Q_FLOW_PROFILE, INST_SINGLE, (uint32 *) arg);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for end to end AQM latency profile \n", type);
            break;
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_cosq_latency_aqm_flow_id_gport_verify(
    int unit,
    uint32 flags,
    bcm_gport_t * gport)
{
    uint32 legal_flags = BCM_COSQ_GPORT_WITH_ID | BCM_COSQ_GPORT_LATENCY_AQM_FLOW_ID;
    int flow_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_latency.features.feature_get(unit, dnx_data_latency_features_valid_end_to_end_aqm_profile))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport AQM flow id not supported for this device.\n");
    }
    SHR_NULL_CHECK(gport, _SHR_E_PARAM, "gport");
    SHR_MASK_VERIFY(flags, legal_flags, _SHR_E_PARAM, "some of the flags are not supported.\n");
    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_GPORT_WITH_ID))
    {
        flow_id = BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(*gport);
        SHR_RANGE_VERIFY(flow_id, 0, dnx_data_latency.aqm.flows_nof_get(unit) - 1, _SHR_E_PARAM,
                         "AQM Flow ID %d is out of range 0-%d\n",
                         flow_id, dnx_data_latency.aqm.flows_nof_get(unit) - 1);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_aqm_flow_id_gport_add(
    int unit,
    uint32 flags,
    bcm_gport_t * gport)
{
    uint32 res_flags = 0;
    int flow_id = 0;
    resource_tag_bitmap_extra_arguments_alloc_info_t alloc_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    res_flags = (flags & BCM_COSQ_GPORT_WITH_ID) ? DNX_ALGO_RES_ALLOCATE_WITH_ID : 0;
    
    SHR_INVOKE_VERIFY_DNX(dnx_cosq_latency_aqm_flow_id_gport_verify(unit, flags, gport));

    sal_memset(&alloc_info, 0x0, sizeof(resource_tag_bitmap_extra_arguments_alloc_info_t));
    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_GPORT_WITH_ID))
    {
        flow_id = BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(*gport);
        res_flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }
    res_flags |= RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG;
    alloc_info.tag = 0;
    SHR_IF_ERR_EXIT(dnx_cosq_latency_db.
                    aqm_flow_id_res.allocate_single(unit, res_flags, (void *) &alloc_info, &flow_id));
    BCM_GPORT_LATENCY_AQM_FLOW_ID_SET((*gport), flow_id);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_cosq_latency_aqm_flow_id_gport_delete(
    int unit,
    bcm_gport_t gport)
{
    int flow_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (BCM_GPORT_IS_LATENCY_AQM_FLOW_ID(gport))
    {
        flow_id = BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(gport);
         
        SHR_IF_ERR_EXIT(dnx_cosq_latency_db.aqm_flow_id_res.free_single(unit, flow_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport AQM flow id %d\n", gport);
    }
exit:
    SHR_FUNC_EXIT;
}
