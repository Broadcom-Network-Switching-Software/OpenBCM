/** \file ingress_congestion_dbal.c
 * 
 *
 * Low level functions accessing DBAL tables
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_error.h>
#include <bcm/cosq.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stat.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include "ingress_congestion.h"
#include "ingress_congestion_convert.h"
#include "ingress_congestion_dbal.h"
#include <bcm_int/dnx/cosq/cosq_dbal_utils.h>

/**
 * \brief - translate threshold to its harware representation
 */
shr_error_e
dnx_ingress_congestion_dbal_threshold_encode(
    int unit,
    uint32 threshold,
    uint32 *encoded_threshold)
{

    SHR_FUNC_INIT_VARS(unit);

    (*encoded_threshold) = threshold / dnx_data_ingr_congestion.info.threshold_granularity_get(unit);

/** exit:*/
    SHR_FUNC_EXIT;
}

/**
 * \brief - translate hardware representation of threshold to the actual number
 */
shr_error_e
dnx_ingress_congestion_dbal_threshold_decode(
    int unit,
    uint32 encoded_threshold,
    uint32 *decoded_threshold)
{

    SHR_FUNC_INIT_VARS(unit);

    (*decoded_threshold) = encoded_threshold * dnx_data_ingr_congestion.info.threshold_granularity_get(unit);

/** exit:*/
    SHR_FUNC_EXIT;
}

/**
 * \brief - translate byte threshold to its harware representation
 */
shr_error_e
dnx_ingress_congestion_dbal_bytes_threshold_encode(
    int unit,
    uint32 threshold,
    uint32 *encoded_threshold)
{

    SHR_FUNC_INIT_VARS(unit);

    (*encoded_threshold) = threshold / dnx_data_ingr_congestion.info.bytes_threshold_granularity_get(unit);

/** exit:*/
    SHR_FUNC_EXIT;
}

/**
 * \brief - translate harware representation of byte threshold to the actual number
 */
shr_error_e
dnx_ingress_congestion_dbal_bytes_threshold_decode(
    int unit,
    uint32 encoded_threshold,
    uint32 *decoded_threshold)
{

    SHR_FUNC_INIT_VARS(unit);

    (*decoded_threshold) = encoded_threshold * dnx_data_ingr_congestion.info.bytes_threshold_granularity_get(unit);

/** exit:*/
    SHR_FUNC_EXIT;
}

/**
 * \brief - write VOQ FADT drop to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_voq_fadt_drop_hw_set(
    int unit,
    dbal_tables_e table_id,
    int rate_class,
    int dp,
    bcm_cosq_fadt_threshold_t * fadt)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_TH, INST_SINGLE, fadt->thresh_max);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TH, INST_SINGLE, fadt->thresh_min);
    /*
     * If AdjustFactor[4] is set,
     *   Dynamic-Max-Th = Free-Resource << AdjustFactor[3:0]
     *   Otherwise,     Dynamic-Max-Th = Free-Resource >> AdjustFactor[3:0]
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALPHA_SIGN, INST_SINGLE,
                                 !utilex_is_neg(fadt->alpha));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALPHA_ABS, INST_SINGLE, utilex_abs(fadt->alpha));

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read VOQ FADT drop from HW
 */
shr_error_e
dnx_ingress_congestion_dbal_voq_fadt_drop_hw_get(
    int unit,
    dbal_tables_e table_id,
    int rate_class,
    int dp,
    bcm_cosq_fadt_threshold_t * fadt)
{
    uint32 entry_handle_id;
    uint32 alpha_sign, alpha_abs;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_TH, INST_SINGLE, &fadt->thresh_max);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MIN_TH, INST_SINGLE, &fadt->thresh_min);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ALPHA_SIGN, INST_SINGLE, &alpha_sign);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ALPHA_ABS, INST_SINGLE, &alpha_abs);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * If AdjustFactor[4] is set,
     *   Dynamic-Max-Th = Free-Resource << AdjustFactor[3:0]
     *   Otherwise,     Dynamic-Max-Th = Free-Resource >> AdjustFactor[3:0]
     */
    fadt->alpha = (alpha_sign ? alpha_abs : -alpha_abs);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - write WRED to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_wred_hw_set(
    int unit,
    dbal_tables_e table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    dnx_ingress_congestion_wred_hw_params_t * params)
{
    uint32 entry_handle_id;
    int is_ecn;
    int exists;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_ecn = (table_id == DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, key->rate_class);
    if (!is_ecn)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, key->dp);
    }

    /** set core key if exists */
    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_CORE_ID, &exists));
    if (exists)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, key->core_id);
    }

    /** set pool id for VSQ-E */
    if (table_id == DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_DROP)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL_ID, key->pool_id);
    }

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, params->wred_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_C1, INST_SINGLE, params->c1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_C2, INST_SINGLE, params->c2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_C3, INST_SINGLE, params->c3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TH, INST_SINGLE, params->min_thresh);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_TH, INST_SINGLE, params->max_thresh);
    if (!is_ecn)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_PACKET_SIZE, INST_SINGLE,
                                     params->ignore_packet_size);
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read WRED from HW
 */
shr_error_e
dnx_ingress_congestion_dbal_wred_hw_get(
    int unit,
    dbal_tables_e table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    dnx_ingress_congestion_wred_hw_params_t * params)
{
    uint32 entry_handle_id;
    int is_ecn;
    int exists;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_ecn = (table_id == DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, key->rate_class);
    if (!is_ecn)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, key->dp);
    }

    /** set core key if exists */
    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_CORE_ID, &exists));
    if (exists)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, key->core_id);
    }

    /** set pool id for VSQ-E */
    if (table_id == DBAL_TABLE_INGRESS_CONG_VSQ_E_RATE_CLASS_WRED_DROP)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL_ID, key->pool_id);
    }

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *) &params->wred_en);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_C1, INST_SINGLE, &params->c1);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_C2, INST_SINGLE, &params->c2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_C3, INST_SINGLE, &params->c3);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MIN_TH, INST_SINGLE, &params->min_thresh);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_TH, INST_SINGLE, &params->max_thresh);
    if (!is_ecn)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_PACKET_SIZE, INST_SINGLE,
                                   (uint32 *) &params->ignore_packet_size);
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read WRED enable for all DPs of the rate class (and ECN for VOQ) from HW
 */
shr_error_e
dnx_ingress_congestion_dbal_wred_enable_hw_get(
    int unit,
    dbal_tables_e table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    int *enable)
{
    uint32 entry_handle_id;
    int is_voq;
    uint32 curr_enable = 0;
    dbal_tables_e wred_table_id;
    dbal_tables_e ecn_table_id;

    int is_end;
    int exists;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *enable = 0;

    wred_table_id = table_id;

    is_voq = ((table_id == DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_DROP) ||
              (table_id == DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN));

    if (is_voq)
    {
        wred_table_id = DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_DROP;
        ecn_table_id = DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN;
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, wred_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /** Add KEY rule */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS,
                                                           DBAL_CONDITION_EQUAL_TO, (uint32 *) &key->rate_class, NULL));
    /** set core key if exists */
    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_CORE_ID, &exists));
    if (exists)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                               DBAL_CONDITION_EQUAL_TO, (uint32 *) &key->core_id,
                                                               NULL));
    }

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE,
                                                                &curr_enable));

        *enable |= curr_enable;
        if (curr_enable)
        {
            break;
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    }

    if (is_voq && !(*enable))
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, ecn_table_id, entry_handle_id));

        /** Setting key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, key->rate_class);

       /** Getting value fields */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &curr_enable);

        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        *enable |= curr_enable;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - write WRED average weight to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_wred_weight_hw_set(
    int unit,
    dbal_tables_e table_id,
    int core_id,
    int rate_class,
    dnx_ingress_congestion_wred_avrg_params_t * avrg_params)
{
    uint32 entry_handle_id;
    int exists;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, rate_class);

    /** set core key if exists */
    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_CORE_ID, &exists));
    if (exists)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, avrg_params->avrg_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AVRG_WEIGHT, INST_SINGLE, avrg_params->avrg_weight);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read WRED average weight to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_wred_weight_hw_get(
    int unit,
    dbal_tables_e table_id,
    int core_id,
    int rate_class,
    dnx_ingress_congestion_wred_avrg_params_t * avrg_params)
{
    uint32 entry_handle_id;
    int exists;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, rate_class);

    /** set core key if exists */
    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_CORE_ID, &exists));
    if (exists)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *) &avrg_params->avrg_en);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_AVRG_WEIGHT, INST_SINGLE, &avrg_params->avrg_weight);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - write hysteresis FC threshold to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_hyst_fc_hw_set(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int pool,
    int rate_class,
    dnx_cosq_hyst_threshold_t * fc)
{
    uint32 entry_handle_id;
    int exists;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, rate_class);

    /** set core key if exists */
    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_CORE_ID, &exists));
    if (exists)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }

    /** set pool key if exists */
    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_POOL, &exists));
    if (exists)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL, pool);
    }

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_TH, INST_SINGLE, fc->set);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLEAR_TH, INST_SINGLE, fc->clear);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read hysteresis FC threshold from HW
 */
shr_error_e
dnx_ingress_congestion_dbal_hyst_fc_hw_get(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int pool,
    int rate_class,
    dnx_cosq_hyst_threshold_t * fc)
{
    uint32 entry_handle_id;
    int exists;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, rate_class);

    /** set core key if exists */
    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_CORE_ID, &exists));
    if (exists)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }

    /** set pool key if exists */
    SHR_IF_ERR_EXIT(dnx_cosq_dbal_key_exists_in_table(unit, table_id, DBAL_FIELD_POOL, &exists));
    if (exists)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL, pool);
    }

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SET_TH, INST_SINGLE, &fc->set);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CLEAR_TH, INST_SINGLE, &fc->clear);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - write FADT FC threshold (VSQ-F) to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_vsq_fadt_fc_hw_set(
    int unit,
    int core,
    dbal_tables_e table_id,
    int rate_class,
    dnx_cosq_fadt_hyst_threshold_t * fadt_fc)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, rate_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_TH, INST_SINGLE, fadt_fc->set.max_threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TH, INST_SINGLE, fadt_fc->set.min_threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, fadt_fc->clear_offset);
    /*
     * If AdjustFactor[4] is set,
     *   Dynamic-Max-Th = Free-Resource << AdjustFactor[3:0]
     *   Otherwise,     Dynamic-Max-Th = Free-Resource >> AdjustFactor[3:0]
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALPHA_SIGN, INST_SINGLE,
                                 !utilex_is_neg(fadt_fc->set.alpha));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALPHA_ABS, INST_SINGLE,
                                 utilex_abs(fadt_fc->set.alpha));

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read  FADT FC (VSQ-f) from HW
 */
shr_error_e
dnx_ingress_congestion_dbal_vsq_fadt_fc_hw_get(
    int unit,
    int core,
    dbal_tables_e table_id,
    int rate_class,
    dnx_cosq_fadt_hyst_threshold_t * fadt_fc)
{
    uint32 entry_handle_id;
    uint32 alpha_sign, alpha_abs;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, rate_class);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_TH, INST_SINGLE, &fadt_fc->set.max_threshold);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MIN_TH, INST_SINGLE, &fadt_fc->set.min_threshold);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, &fadt_fc->clear_offset);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ALPHA_SIGN, INST_SINGLE, &alpha_sign);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ALPHA_ABS, INST_SINGLE, &alpha_abs);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * If AdjustFactor[4] is set,
     *   Dynamic-Max-Th = Free-Resource << AdjustFactor[3:0]
     *   Otherwise,     Dynamic-Max-Th = Free-Resource >> AdjustFactor[3:0]
     */
    fadt_fc->set.alpha = (alpha_sign ? alpha_abs : -alpha_abs);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - write port to VSQ-E and PG base mapping
 *
 * \note in_port is a PP port equivalent
 */
shr_error_e
dnx_ingress_congestion_dbal_port_to_src_vsq_map_set(
    int unit,
    int core,
    int in_port,
    int vsq_e,
    int pg_base)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (pg_base == -1)
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ,
                                                              DBAL_FIELD_PG_BASE, 0, 0, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, (uint32 *) &pg_base));
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, in_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSQ_E, INST_SINGLE, vsq_e);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PG_BASE, INST_SINGLE, pg_base);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - clear port to VSQ-E and PG base mapping
 *
 * \note in_port is a PP port equivalent
 */
shr_error_e
dnx_ingress_congestion_dbal_port_to_src_vsq_map_clear(
    int unit,
    int core,
    int in_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, in_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read port to VSQ-E and PG base mapping
 *
 * \note in_port is a PP port equivalent
 */
shr_error_e
dnx_ingress_congestion_dbal_port_to_src_vsq_map_get(
    int unit,
    int core,
    int in_port,
    int *vsq_e,
    int *pg_base)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, in_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VSQ_E, INST_SINGLE, (uint32 *) vsq_e);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PG_BASE, INST_SINGLE, (uint32 *) pg_base);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - return whether vsq-e/f for specified port enabled
 *
 * \note in_port is a PP port equivalent
 */
shr_error_e
dnx_ingress_congestion_dbal_port_to_src_vsq_enable_get(
    int unit,
    int core,
    int in_port,
    int *enable)
{
    uint32 entry_handle_id;
    uint32 vsq_e, invalid_vsq_e;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, in_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** get default value */
    SHR_IF_ERR_EXIT(dbal_entry_default_get(unit, entry_handle_id, 0));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VSQ_E,
                                                        INST_SINGLE, &invalid_vsq_e));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ, entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, in_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VSQ_E, INST_SINGLE, &vsq_e);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *enable = (vsq_e != invalid_vsq_e);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * map port to TC-PG profile
 */
shr_error_e
dnx_ingress_congestion_dbal_port_tc_pg_profile_set(
    int unit,
    int core,
    int port,
    int tc_pg_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_PG_PROFILE, INST_SINGLE, tc_pg_profile);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get port to TC-PG profile mapping
 */
shr_error_e
dnx_ingress_congestion_dbal_port_tc_pg_profile_get(
    int unit,
    int core,
    int port,
    int *tc_pg_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PORT_TO_SRC_VSQ, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TC_PG_PROFILE, INST_SINGLE, (uint32 *) tc_pg_profile);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * configure TC->PG mapping per profile
 */
shr_error_e
dnx_ingress_congestion_dbal_tc_pg_map_set(
    int unit,
    int core,
    int tc_pg_profile,
    int tc,
    int pg)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_TC_PG_MAPPING, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_PG_PROFILE, tc_pg_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PG, INST_SINGLE, pg);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get TC->PG mapping per profile
 */
shr_error_e
dnx_ingress_congestion_dbal_tc_pg_map_get(
    int unit,
    int core,
    int tc_pg_profile,
    int tc,
    int *pg)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_TC_PG_MAPPING, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_PG_PROFILE, tc_pg_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PG, INST_SINGLE, (uint32 *) pg);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * configure PG params
 */
shr_error_e
dnx_ingress_congestion_dbal_pg_params_set(
    int unit,
    int core,
    int pg,
    dnx_ingress_congestion_pg_params_t * pg_params)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PG_PARAMS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PG_ID, pg);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LOSSLESS, INST_SINGLE, pg_params->is_lossless);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL_ID, INST_SINGLE, pg_params->pool_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE, INST_SINGLE,
                                 pg_params->admit_profile);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get PG params
 */
shr_error_e
dnx_ingress_congestion_dbal_pg_params_get(
    int unit,
    int core,
    int pg,
    dnx_ingress_congestion_pg_params_t * pg_params)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PG_PARAMS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PG_ID, pg);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_LOSSLESS, INST_SINGLE,
                               (uint32 *) &pg_params->is_lossless);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_POOL_ID, INST_SINGLE, (uint32 *) &pg_params->pool_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE, INST_SINGLE,
                               (uint32 *) &pg_params->admit_profile);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * set PG use port guaranteed property
 */
shr_error_e
dnx_ingress_congestion_dbal_pg_use_port_guaranteed_set(
    int unit,
    int core,
    int pg,
    int use_port_guaranteed)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PG_PARAMS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PG_ID, pg);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_USE_PORT_GUARANTEED, INST_SINGLE,
                                 use_port_guaranteed);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get PG use port guaranteed property
 */
shr_error_e
dnx_ingress_congestion_dbal_pg_use_port_guaranteed_get(
    int unit,
    int core,
    int pg,
    int *use_port_guaranteed)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PG_PARAMS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PG_ID, pg);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_USE_PORT_GUARANTEED, INST_SINGLE,
                               (uint32 *) use_port_guaranteed);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set VSQ-E resource allocation
 */
shr_error_e
dnx_ingress_congestion_dbal_vsq_e_resource_alloc_hw_set(
    int unit,
    int core,
    dbal_tables_e table_id,
    uint32 rt_cls,
    int pool,
    int dp,
    dnx_ingress_congestion_vsq_e_resource_alloc_params_t * resource_alloc_params)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, rt_cls);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL, pool);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GUARANTEED_SIZE, INST_SINGLE,
                                 resource_alloc_params->guaranteed);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARED_SIZE, INST_SINGLE,
                                 resource_alloc_params->shared_pool);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADROOM_SIZE, INST_SINGLE,
                                 resource_alloc_params->headroom);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get VSQ-E resource allocation
 */
shr_error_e
dnx_ingress_congestion_dbal_vsq_e_resource_alloc_hw_get(
    int unit,
    int core,
    dbal_tables_e table_id,
    uint32 rt_cls,
    int pool,
    int dp,
    dnx_ingress_congestion_vsq_e_resource_alloc_params_t * resource_alloc_params)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, rt_cls);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL, pool);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GUARANTEED_SIZE, INST_SINGLE,
                               (uint32 *) &resource_alloc_params->guaranteed);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHARED_SIZE, INST_SINGLE,
                               (uint32 *) &resource_alloc_params->shared_pool);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HEADROOM_SIZE, INST_SINGLE,
                               (uint32 *) &resource_alloc_params->headroom);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set VSQ-F resource allocation
 */
shr_error_e
dnx_ingress_congestion_dbal_vsq_f_resource_alloc_hw_set(
    int unit,
    int core,
    dbal_tables_e table_id,
    uint32 rt_cls,
    int dp,
    dnx_ingress_congestion_vsq_f_resource_alloc_params_t * resource_alloc_params)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, rt_cls);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GUARANTEED_SIZE, INST_SINGLE,
                                 resource_alloc_params->guaranteed);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARED_FADT_MAX_SIZE, INST_SINGLE,
                                 resource_alloc_params->shared_pool_fadt.max_threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARED_FADT_MIN_SIZE, INST_SINGLE,
                                 resource_alloc_params->shared_pool_fadt.min_threshold);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARED_FADT_ALPHA_SIGN, INST_SINGLE,
                                 !utilex_is_neg(resource_alloc_params->shared_pool_fadt.alpha));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARED_FADT_ALPHA_ABS, INST_SINGLE,
                                 utilex_abs(resource_alloc_params->shared_pool_fadt.alpha));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADROOM_NOMINAL_SIZE, INST_SINGLE,
                                 resource_alloc_params->nominal_headroom);

    if (table_id != DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_WORDS)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADROOM_TOTAL_SIZE, INST_SINGLE,
                                     resource_alloc_params->max_headroom);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HEADROOM_EXTENSION_SIZE, INST_SINGLE,
                                     resource_alloc_params->headroom_extension);
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set VSQ-F resource allocation
 */
shr_error_e
dnx_ingress_congestion_dbal_vsq_f_resource_alloc_hw_get(
    int unit,
    int core,
    dbal_tables_e table_id,
    uint32 rt_cls,
    int dp,
    dnx_ingress_congestion_vsq_f_resource_alloc_params_t * resource_alloc_params)
{
    uint32 entry_handle_id;
    uint32 alpha_sign, alpha_abs;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE_CLASS, rt_cls);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GUARANTEED_SIZE, INST_SINGLE,
                               (uint32 *) &resource_alloc_params->guaranteed);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHARED_FADT_MAX_SIZE, INST_SINGLE,
                               (uint32 *) &resource_alloc_params->shared_pool_fadt.max_threshold);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHARED_FADT_MIN_SIZE, INST_SINGLE,
                               (uint32 *) &resource_alloc_params->shared_pool_fadt.min_threshold);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHARED_FADT_ALPHA_SIGN, INST_SINGLE, &alpha_sign);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHARED_FADT_ALPHA_ABS, INST_SINGLE, &alpha_abs);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HEADROOM_NOMINAL_SIZE, INST_SINGLE,
                               (uint32 *) &resource_alloc_params->nominal_headroom);

    if (table_id != DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_WORDS)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HEADROOM_TOTAL_SIZE, INST_SINGLE,
                                   (uint32 *) &resource_alloc_params->max_headroom);

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HEADROOM_EXTENSION_SIZE, INST_SINGLE,
                                   (uint32 *) &resource_alloc_params->headroom_extension);
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * If AdjustFactor[3] is set,
     *   Dynamic-Max-Th = Free-Resource << AdjustFactor[2:0]
     *   Otherwise,     Dynamic-Max-Th = Free-Resource >> AdjustFactor[3:0]
     */
    resource_alloc_params->shared_pool_fadt.alpha = (alpha_sign ? alpha_abs : -alpha_abs);

    if (table_id == DBAL_TABLE_INGRESS_CONG_VSQ_F_RATE_CLASS_RESOURCE_ALLOC_WORDS)
    {
        resource_alloc_params->max_headroom = resource_alloc_params->nominal_headroom;
        resource_alloc_params->headroom_extension = 0;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set global VSQ resource allocation
 */
shr_error_e
dnx_ingress_congestion_dbal_global_vsq_resource_alloc_hw_set(
    int unit,
    int core,
    dbal_tables_e table_id,
    int pool_id,
    int is_lossless,
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t * resource_alloc_params)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHARED_SIZE, pool_id,
                                 resource_alloc_params->shared_pool);

    if (is_lossless)
    {
        /*
         * there is a single instance of nominal headroom field
         * should be configured for lossless pool only
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NOMINAL_HEADROOM, INST_SINGLE,
                                     resource_alloc_params->nominal_headroom);
    }

    if (table_id != DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_WORDS)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_HEADROOM, pool_id,
                                     resource_alloc_params->max_headroom);
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get global VSQ resource allocation
 */
shr_error_e
dnx_ingress_congestion_dbal_global_vsq_resource_alloc_hw_get(
    int unit,
    int core,
    dbal_tables_e table_id,
    int pool_id,
    int is_lossless,
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t * resource_alloc_params)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SHARED_SIZE, pool_id,
                               &resource_alloc_params->shared_pool);

    if (is_lossless)
    {
        /*
         * there is a single instance of nominal headroom field
         * should be configured for lossless pool only
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NOMINAL_HEADROOM, INST_SINGLE,
                                   &resource_alloc_params->nominal_headroom);
    }
    else
    {
        /** for non lossless pool headroom must be 0 */
        resource_alloc_params->nominal_headroom = 0;
    }

    if (table_id != DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_WORDS)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_HEADROOM, pool_id,
                                   &resource_alloc_params->max_headroom);
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (table_id == DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_WORDS)
    {
        resource_alloc_params->max_headroom = resource_alloc_params->nominal_headroom;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - write global VSQ threshold to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_vsq_shared_threshold_hw_set(
    int unit,
    int pool_id,
    int dp,
    dnx_ingress_congestion_resource_type_e resource_type,
    uint32 set_threshold,
    uint32 clear_threshold)
{
    uint32 entry_handle_id;
    uint32 encoded_set_threshold, encoded_clear_threshold;

    dbal_fields_e set_thresh_field[] = {
        DBAL_FIELD_WORDS_SET_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD,
        DBAL_FIELD_SRAM_PDS_SET_THRESHOLD
    };

    dbal_fields_e clear_thresh_field[] = {
        DBAL_FIELD_WORDS_CLEAR_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD,
        DBAL_FIELD_SRAM_PDS_CLEAR_THRESHOLD
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VSQ_FREE_SHARED_THRESHOLD_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL_ID, pool_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    if (resource_type == DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_bytes_threshold_encode(unit,
                                                                           set_threshold, &encoded_set_threshold));
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_bytes_threshold_encode(unit,
                                                                           clear_threshold, &encoded_clear_threshold));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_encode(unit, set_threshold, &encoded_set_threshold));
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_encode(unit, clear_threshold, &encoded_clear_threshold));
    }

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 set_thresh_field[resource_type], INST_SINGLE, encoded_set_threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 clear_thresh_field[resource_type], INST_SINGLE, encoded_clear_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read global VSQ threshold from HW
 */
shr_error_e
dnx_ingress_congestion_dbal_vsq_shared_threshold_hw_get(
    int unit,
    int pool_id,
    int dp,
    dnx_ingress_congestion_resource_type_e resource_type,
    uint32 *set_threshold,
    uint32 *clear_threshold)
{
    uint32 entry_handle_id;
    uint32 encoded_set_threshold, encoded_clear_threshold;

    dbal_fields_e set_thresh_field[] = {
        DBAL_FIELD_WORDS_SET_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD,
        DBAL_FIELD_SRAM_PDS_SET_THRESHOLD
    };

    dbal_fields_e clear_thresh_field[] = {
        DBAL_FIELD_WORDS_CLEAR_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD,
        DBAL_FIELD_SRAM_PDS_CLEAR_THRESHOLD
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VSQ_FREE_SHARED_THRESHOLD_TABLE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_POOL_ID, pool_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, set_thresh_field[resource_type], INST_SINGLE,
                               &encoded_set_threshold);
    dbal_value_field32_request(unit, entry_handle_id, clear_thresh_field[resource_type], INST_SINGLE,
                               &encoded_clear_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (resource_type == DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_bytes_threshold_decode(unit, encoded_set_threshold, set_threshold));
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_bytes_threshold_decode(unit,
                                                                           encoded_clear_threshold, clear_threshold));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_decode(unit, encoded_set_threshold, set_threshold));
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_decode(unit, encoded_clear_threshold, clear_threshold));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - write global free SRAM threshold to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_global_free_sram_threshold_hw_set(
    int unit,
    int index /* either dp or tc */ ,
    int is_dp /* true if index is dp */ ,
    dnx_ingress_congestion_global_sram_resource_t resource_type,
    uint32 set_threshold,
    uint32 clear_threshold)
{
    uint32 entry_handle_id;
    uint32 encoded_set_threshold, encoded_clear_threshold;

    dbal_fields_e set_thresh_field[] = {
        DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD
    };

    dbal_fields_e clear_thresh_field[] = {
        DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      (is_dp ? DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_DP_THRESHOLDS_TABLE :
                                       DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_TC_THRESHOLDS_TABLE),
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, (is_dp ? DBAL_FIELD_DP : DBAL_FIELD_TC), index);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_encode(unit, set_threshold, &encoded_set_threshold));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_encode(unit, clear_threshold, &encoded_clear_threshold));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 set_thresh_field[resource_type], INST_SINGLE, encoded_set_threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 clear_thresh_field[resource_type], INST_SINGLE, encoded_clear_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read global free SRAM threshold from HW
 */
shr_error_e
dnx_ingress_congestion_dbal_global_free_sram_threshold_hw_get(
    int unit,
    int index /* either dp or tc */ ,
    int is_dp /* true if index is dp */ ,
    dnx_ingress_congestion_global_sram_resource_t resource_type,
    uint32 *set_threshold,
    uint32 *clear_threshold)
{
    uint32 entry_handle_id;
    uint32 encoded_set_threshold, encoded_clear_threshold;

    dbal_fields_e set_thresh_field[] = {
        DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD
    };

    dbal_fields_e clear_thresh_field[] = {
        DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      (is_dp ? DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_DP_THRESHOLDS_TABLE :
                                       DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_SRAM_PER_TC_THRESHOLDS_TABLE),
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, (is_dp ? DBAL_FIELD_DP : DBAL_FIELD_TC), index);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, set_thresh_field[resource_type], INST_SINGLE,
                               &encoded_set_threshold);
    dbal_value_field32_request(unit, entry_handle_id, clear_thresh_field[resource_type], INST_SINGLE,
                               &encoded_clear_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_decode(unit, encoded_set_threshold, set_threshold));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_decode(unit, encoded_clear_threshold, clear_threshold));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - write global free DRAM threshold to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_global_free_dram_threshold_hw_set(
    int unit,
    int index /* either dp or tc */ ,
    int is_dp /* true if index is dp */ ,
    uint32 set_threshold,
    uint32 clear_threshold)
{
    uint32 entry_handle_id;
    uint32 encoded_set_threshold, encoded_clear_threshold;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      (is_dp ? DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_DP_THRESHOLDS_TABLE :
                                       DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_TC_THRESHOLDS_TABLE),
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, (is_dp ? DBAL_FIELD_DP : DBAL_FIELD_TC), index);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_encode(unit, set_threshold, &encoded_set_threshold));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_encode(unit, clear_threshold, &encoded_clear_threshold));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_DRAM_BDBS_SET_THRESHOLD, INST_SINGLE, encoded_set_threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_DRAM_BDBS_CLEAR_THRESHOLD, INST_SINGLE, encoded_clear_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read global free DRAM threshold from HW
 */
shr_error_e
dnx_ingress_congestion_dbal_global_free_dram_threshold_hw_get(
    int unit,
    int index /* either dp or tc */ ,
    int is_dp /* true if index is dp */ ,
    uint32 *set_threshold,
    uint32 *clear_threshold)
{
    uint32 entry_handle_id;
    uint32 encoded_set_threshold, encoded_clear_threshold;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit,
                                      (is_dp ? DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_DP_THRESHOLDS_TABLE :
                                       DBAL_TABLE_INGRESS_CONG_GLOBAL_FREE_DRAM_PER_TC_THRESHOLDS_TABLE),
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, (is_dp ? DBAL_FIELD_DP : DBAL_FIELD_TC), index);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_BDBS_SET_THRESHOLD, INST_SINGLE,
                               &encoded_set_threshold);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_BDBS_CLEAR_THRESHOLD, INST_SINGLE,
                               &encoded_clear_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_decode(unit, encoded_set_threshold, set_threshold));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_decode(unit, encoded_clear_threshold, clear_threshold));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - write CMD FIFO SRAM threshold to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_cmd_fifo_sram_reject_threshold_hw_set(
    int unit,
    dnx_ingress_congestion_cmd_fifo_type_t fifo_id,
    bcm_cosq_forward_decision_type_t fwd_action,
    dnx_ingress_congestion_global_sram_resource_t resource,
    uint32 set_threshold,
    uint32 clear_threshold)
{
    uint32 entry_handle_id;
    uint32 encoded_set_threshold, encoded_clear_threshold;

    dbal_fields_e set_thresh_field[] = {
        DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD
    };

    dbal_fields_e clear_thresh_field[] = {
        DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_COMMAND_FIFO_SRAM_REJECT_THRESHOLD,
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_ID, fifo_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_ACTION, fwd_action);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_encode(unit, set_threshold, &encoded_set_threshold));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_encode(unit, clear_threshold, &encoded_clear_threshold));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, set_thresh_field[resource], INST_SINGLE, encoded_set_threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 clear_thresh_field[resource], INST_SINGLE, encoded_clear_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read CMD FIFO SRAM threshold from HW
 */
shr_error_e
dnx_ingress_congestion_dbal_cmd_fifo_sram_reject_threshold_hw_get(
    int unit,
    dnx_ingress_congestion_cmd_fifo_type_t fifo_id,
    bcm_cosq_forward_decision_type_t fwd_action,
    dnx_ingress_congestion_global_sram_resource_t resource,
    uint32 *set_threshold,
    uint32 *clear_threshold)
{
    uint32 entry_handle_id;
    uint32 encoded_set_threshold, encoded_clear_threshold;

    dbal_fields_e set_thresh_field[] = {
        DBAL_FIELD_SRAM_PDBS_SET_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_SET_THRESHOLD
    };

    dbal_fields_e clear_thresh_field[] = {
        DBAL_FIELD_SRAM_PDBS_CLEAR_THRESHOLD,
        DBAL_FIELD_SRAM_BUFFERS_CLEAR_THRESHOLD
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_COMMAND_FIFO_SRAM_REJECT_THRESHOLD,
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_ID, fifo_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_ACTION, fwd_action);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, set_thresh_field[resource], INST_SINGLE, &encoded_set_threshold);
    dbal_value_field32_request(unit, entry_handle_id, clear_thresh_field[resource], INST_SINGLE,
                               &encoded_clear_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_decode(unit, encoded_set_threshold, set_threshold));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_decode(unit, encoded_clear_threshold, clear_threshold));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Generic function to get values from table INGRESS_CONG_VOQ_PROFILE_INFO
 */
static shr_error_e
dnx_ingress_congestion_dbal_voq_info_generic_get(
    int unit,
    int core,
    uint32 voq,
    dbal_fields_e dbal_field,
    uint32 *value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VOQ_PROFILE_INFO, &entry_handle_id));

    /*
     *  Select the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set VOQ
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, voq);

    /*
     * Get dbal field
     */
    dbal_value_field32_request(unit, entry_handle_id, dbal_field, INST_SINGLE, value);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Generic function to set values in table INGRESS_CONG_VOQ_PROFILE_INFO
 */
static shr_error_e
dnx_ingress_congestion_dbal_voq_info_generic_set(
    int unit,
    int core,
    uint32 voq,
    dbal_fields_e dbal_field,
    uint32 value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VOQ_PROFILE_INFO, &entry_handle_id));

    /*
     *  Select the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set VOQ
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, voq);

    /*
     * Set dbal field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field, INST_SINGLE, value);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ingress_congestion_dbal_voq_info_clear(
    int unit,
    int core,
    uint32 voq)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VOQ_PROFILE_INFO, &entry_handle_id));

    /*
     *  Select the core
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set VOQ
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, voq);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_ingress_congestion_dbal_voq_rate_class_set(
    int unit,
    int core,
    uint32 voq,
    uint32 rate_class)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_info_generic_set
                    (unit, core, voq, DBAL_FIELD_RATE_CLASS, rate_class));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_ingress_congestion_dbal_voq_rate_class_get(
    int unit,
    int core,
    uint32 voq,
    uint32 *rate_class)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_info_generic_get
                    (unit, core, voq, DBAL_FIELD_RATE_CLASS, rate_class));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_ingress_congestion_dbal_voq_connection_class_set(
    int unit,
    int core,
    uint32 voq,
    uint32 connection_class)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_info_generic_set
                    (unit, core, voq, DBAL_FIELD_CONNECTION_CLASS, connection_class));

exit:
    SHR_FUNC_EXIT;

}

/*
 * See .h file.
 */
shr_error_e
dnx_ingress_congestion_dbal_voq_connection_class_get(
    int unit,
    int core,
    uint32 voq,
    uint32 *connection_class)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_info_generic_get
                    (unit, core, voq, DBAL_FIELD_CONNECTION_CLASS, connection_class));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_ingress_congestion_dbal_voq_traffic_class_set(
    int unit,
    int core,
    uint32 voq,
    uint32 traffic_class)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_info_generic_set
                    (unit, core, voq, DBAL_FIELD_TRAFFIC_CLASS, traffic_class));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_ingress_congestion_dbal_voq_traffic_class_get(
    int unit,
    int core,
    uint32 voq,
    uint32 *traffic_class)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_info_generic_get
                    (unit, core, voq, DBAL_FIELD_TRAFFIC_CLASS, traffic_class));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file.
 */
shr_error_e
dnx_ingress_congestion_dbal_voq_shared_res_threshold_set(
    int unit,
    int core,
    dbal_tables_e dbal_table_id,
    uint32 dp,
    uint32 set_thr,
    uint32 clear_thr)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /*
     * Setting key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set DP
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /*
     * Set thresholds
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_REJECT_STATUS_THRESHOLD, INST_SINGLE, set_thr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLEAR_REJECT_STATUS_THRESHOLD, INST_SINGLE,
                                 clear_thr);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_voq_shared_res_threshold_get(
    int unit,
    int core,
    dbal_tables_e dbal_table_id,
    uint32 dp,
    uint32 *set_thr,
    uint32 *clear_thr)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Taking a handle to DBAL table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /*
     * Setting key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * Set DP
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /*
     * Get thresholds
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SET_REJECT_STATUS_THRESHOLD, INST_SINGLE, set_thr);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CLEAR_REJECT_STATUS_THRESHOLD, INST_SINGLE, clear_thr);

    /*
     * Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set FADT thresholds to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_fadt_hw_set(
    int unit,
    dbal_tables_e table_id,
    int rate_class,
    int is_resource_range,
    bcm_cosq_fadt_threshold_t * fadt)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_FADT_TH, INST_SINGLE, fadt->thresh_max);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_FADT_TH, INST_SINGLE, fadt->thresh_min);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALPHA_SIGN, INST_SINGLE,
                                 !utilex_is_neg(fadt->alpha));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALPHA_ABS, INST_SINGLE, utilex_abs(fadt->alpha));
    if (is_resource_range)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_FREE_RES_TH, INST_SINGLE,
                                     fadt->resource_range_max);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_FREE_RES_TH, INST_SINGLE,
                                     fadt->resource_range_min);
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read FADT thresholds from HW
 */
shr_error_e
dnx_ingress_congestion_dbal_fadt_hw_get(
    int unit,
    dbal_tables_e table_id,
    int rate_class,
    int is_resource_range,
    bcm_cosq_fadt_threshold_t * fadt)
{
    uint32 entry_handle_id;
    uint32 alpha_sign, alpha_abs;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_FADT_TH, INST_SINGLE, &fadt->thresh_max);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MIN_FADT_TH, INST_SINGLE, &fadt->thresh_min);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ALPHA_SIGN, INST_SINGLE, &alpha_sign);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ALPHA_ABS, INST_SINGLE, &alpha_abs);
    if (is_resource_range)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MAX_FREE_RES_TH, INST_SINGLE,
                                   &fadt->resource_range_max);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MIN_FREE_RES_TH, INST_SINGLE,
                                   &fadt->resource_range_min);
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * If AdjustFactor[3] is set,
     *   Dynamic-Max-Th = Free-Resource << AdjustFactor[2:0]
     *   Otherwise,     Dynamic-Max-Th = Free-Resource >> AdjustFactor[3:0]
     */
    fadt->alpha = (alpha_sign ? alpha_abs : -alpha_abs);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set thresholds to HW
 */
shr_error_e
dnx_ingress_congestion_notification_fifo_pb_vsq_pg_hw_get(
    int unit,
    bcm_core_t core,
    uint32 *is_valid,
    dbal_enum_value_field_congestion_notification_resource_type_e * resource_type,
    uint32 *pg_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PB_VSQ_PG_CONGESTION_NOTIFICATION_FIFO,
                                      &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, is_valid);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONGESTION_NOTIFICATION_RESOURCE_TYPE, INST_SINGLE,
                               resource_type);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PG_INDEX, INST_SINGLE, pg_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set thresholds to HW
 */
shr_error_e
dnx_ingress_congestion_notification_fifo_pb_vsq_llfc_hw_get(
    int unit,
    bcm_core_t core,
    uint32 *is_valid,
    dbal_enum_value_field_congestion_notification_resource_type_e * resource_type,
    uint32 *port_index,
    uint32 *pool_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_PB_VSQ_LLFC_CONGESTION_NOTIFICATION_FIFO,
                                      &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, is_valid);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONGESTION_NOTIFICATION_RESOURCE_TYPE, INST_SINGLE,
                               resource_type);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PORT_INDEX, INST_SINGLE, port_index);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_POOL_ID, INST_SINGLE, pool_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set FADT thresholds to HW
 */
shr_error_e
dnx_ingress_congestion_notification_fifo_voq_hw_get(
    int unit,
    bcm_core_t core,
    uint32 *is_valid,
    dbal_enum_value_field_congestion_notification_resource_type_e * resource_type,
    uint32 *voq_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VOQ_CONGESTION_NOTIFICATION_FIFO,
                                      &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, is_valid);
    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONGESTION_NOTIFICATION_RESOURCE_TYPE, INST_SINGLE,
                               resource_type);
    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VOQ, INST_SINGLE, voq_id);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_dram_recovery_set(
    int unit,
    int rate_class,
    uint32 recovery_threshold)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_SRAM_WORDS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RECOVERY_TH, INST_SINGLE, recovery_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_dram_recovery_get(
    int unit,
    int rate_class,
    uint32 *recovery_threshold)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_SRAM_WORDS, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RECOVERY_TH, INST_SINGLE, recovery_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set rate class as DRAM block eligible for all rate classes
 */
shr_error_e
dnx_ingress_congestion_dbal_dram_block_eligible_set_all(
    int unit,
    int dram_block_eligible)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_USE_MODE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BLOCK_ELIGIBLE, INST_SINGLE,
                                 dram_block_eligible);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_dram_use_mode_set(
    int unit,
    int rate_class,
    dbal_enum_value_field_dram_use_mode_e dbal_usage_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_USE_MODE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_USE_MODE, INST_SINGLE, dbal_usage_mode);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_dram_use_mode_get(
    int unit,
    int rate_class,
    dbal_enum_value_field_dram_use_mode_e * dbal_usage_mode)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_USE_MODE, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_RATE_CLASS, rate_class);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_USE_MODE, INST_SINGLE, dbal_usage_mode);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_override_pp_port_with_reassembly_context_enable_range_set(
    int unit,
    int reassembly_ctxt_min,
    int reassembly_ctxt_max,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_INGRESS_CONG_OVERRIDE_PP_PORT_WITH_REASSEMBLY_CONTEXT, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT, reassembly_ctxt_min,
                                     reassembly_ctxt_max);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OVERIDE_ENABLE, INST_SINGLE, enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_override_pp_port_with_reassembly_context_enable_set(
    int unit,
    int reassembly_ctxt,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_override_pp_port_with_reassembly_context_enable_range_set
                    (unit, reassembly_ctxt, reassembly_ctxt, enable));
exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_override_pp_port_with_reassembly_context_enable_get(
    int unit,
    int reassembly_ctxt,
    uint32 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_INGRESS_CONG_OVERRIDE_PP_PORT_WITH_REASSEMBLY_CONTEXT, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REASSEMBLY_CONTEXT, reassembly_ctxt);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_OVERIDE_ENABLE, INST_SINGLE, enable);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_init_settings_config_set(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_INIT_SETTINGS, &entry_handle_id));

    /** set all MC replication to be counted only once */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_COUNT_ONCE, INST_SINGLE, TRUE);

    /** set TC for PG mapping to be taken from TM command */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PG_MAP_TC_SELECT, INST_SINGLE,
                                 DBAL_ENUM_FVAL_PG_MAP_TC_SELECT_TM_TC);

    /** set dequeue report to count SRAM bytes (otherwise will count credit bytes) */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEQ_RPRT_IS_SRAM_BYTES, INST_SINGLE, TRUE);

    /** set cropped packets to count by original size (otherwise count as 256B) */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_CROPPED_ORIG_SIZE, INST_SINGLE, TRUE);

    /** set VOQ resources to not allow overflow */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_WORDS_IN_GRNTD_IS_OVF_ALLOWED, INST_SINGLE,
                                 FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_SRAM_BUFFERS_IN_GRNTD_IS_OVF_ALLOWED,
                                 INST_SINGLE, FALSE);

    /**set VSQ resources to not allow overflow */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_WORDS_GRNTD_IS_OVF_ALLOWED, INST_SINGLE,
                                 FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_SRAM_BUFFERS_GRNTD_IS_OVF_ALLOWED,
                                 INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_PORT_WORDS_SHRD_IS_OVF_ALLOWED, INST_SINGLE,
                                 FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_PORT_WORDS_HDRM_IS_OVF_ALLOWED, INST_SINGLE,
                                 FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_PG_WORDS_SHRD_IS_OVF_ALLOWED, INST_SINGLE,
                                 FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_PG_WORDS_HDRM_IS_OVF_ALLOWED, INST_SINGLE,
                                 FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_WORDS_SHRD_IS_OVF_ALLOWED, INST_SINGLE,
                                 FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_WORDS_HDRM_IS_OVF_ALLOWED, INST_SINGLE,
                                 FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_PORT_SRAM_BUFFERS_SHRD_IS_OVF_ALLOWED,
                                 INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_PORT_SRAM_BUFFERS_HDRM_IS_OVF_ALLOWED,
                                 INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_PG_SRAM_BUFFERS_SHRD_IS_OVF_ALLOWED,
                                 INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_PG_SRAM_BUFFERS_HDRM_IS_OVF_ALLOWED,
                                 INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_SRAM_BUFFERS_SHRD_IS_OVF_ALLOWED, INST_SINGLE,
                                 FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PB_VSQ_SRAM_BUFFERS_HDRM_IS_OVF_ALLOWED, INST_SINGLE,
                                 FALSE);

    /** enable stamp FTMH with FAP port */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_STAMP_FTMH_FAP_PORT_EN, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_STAMP_FTMH_FAP_PORT_EN, INST_SINGLE, TRUE);

    /** enable stamp FTMH with congestion notification indication bit */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_STAMP_FTMH_CNI_BIT_EN, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_STAMP_FTMH_CNI_BIT_EN, INST_SINGLE, TRUE);

    /** enable stamp FTMH with DP */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_STAMP_FTMH_DP_EN, INST_SINGLE, TRUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_STAMP_FTMH_DP_EN, INST_SINGLE, TRUE);

    /** enable VOQ Congestion interrupt */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ_CONG_INDICATION_EN, INST_SINGLE, TRUE);

    if (dnx_data_ingr_congestion.config.feature_get(unit,
                                                    dnx_data_ingr_congestion_config_inband_telemetry_ftmh_extension_eco_fix))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INB_TELEMETRY_ECO_ENABLE, INST_SINGLE, TRUE);
    }

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_global_dp_discard_set(
    int unit,
    uint32 dp_discard)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_DP_DISCARD, &entry_handle_id));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_DISCARD, INST_SINGLE, dp_discard);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_global_dp_discard_get(
    int unit,
    uint32 *dp_discard)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_DP_DISCARD, &entry_handle_id));

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DP_DISCARD, INST_SINGLE, dp_discard);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_last_enqueued_cmd_info_get(
    int unit,
    int core,
    dnx_ingress_congestion_last_enqueued_cmd_info_t * last_cmd_info)
{
    uint32 entry_handle_id;
    uint32 delta;
    int delta_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LAST_ENQ_CMD_INFO, &entry_handle_id));

    /** Select the core*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Getting queue num where last enqueued packet was stored */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_ENQ_CMD_QNUM, INST_SINGLE, &last_cmd_info->qnum);

    /** Getting packet size of last enqueued packet */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_ENQ_CMD_PKT_SIZE, INST_SINGLE,
                               &last_cmd_info->pkt_size);

    /** Getting compensation of last enqueued packet */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_ENQ_CMD_COMPENSATION, INST_SINGLE, &delta);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** translate compensation value from 2's complement into a signed value */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                    (unit, DBAL_TABLE_LAST_ENQ_CMD_INFO, DBAL_FIELD_LAST_ENQ_CMD_COMPENSATION, FALSE, 0, 0,
                     &delta_size));

    last_cmd_info->compensation = UTILEX_TWO_COMPLEMENT_INTO_SIGNED_NUM(delta, delta_size);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_voq_dram_bound_state_get(
    int unit,
    int core,
    int queue_id,
    dbal_enum_value_field_dram_bound_state_e * dram_bound_status)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Allocate table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VOQ_DRAM_BOUND_STATE, &entry_handle_id));

    /**
     * Set Keys
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VOQ, queue_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_BOUND_STATE, INST_SINGLE,
                               (uint32 *) dram_bound_status);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_guarantee_over_admit_set(
    int unit,
    int dp,
    uint32 is_guarantee_over_admit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_DP_GURANTEE_OVER_ADMIT, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_GUARANTEE_OVER_ADMIT, INST_SINGLE,
                                 is_guarantee_over_admit);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_guarantee_over_admit_get(
    int unit,
    int dp,
    uint32 *is_guarantee_over_admit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_DP_GURANTEE_OVER_ADMIT, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Getting value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_GUARANTEE_OVER_ADMIT, INST_SINGLE,
                               is_guarantee_over_admit);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * set vsq words reject mapping
 */
shr_error_e
dnx_ingress_congestion_dbal_vsq_words_reject_map_set(
    int unit,
    int admit_profile,
    const dnx_data_ingr_congestion_init_vsq_words_rjct_map_t * reject_reasons,
    int reject)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VSQ_WORDS_REJECT_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE, admit_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TOTAL_SHARED_BLOCKED,
                               reject_reasons->is_total_shared_blocked);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_PORT_PG_SHARED_BLOCKED,
                               reject_reasons->is_port_pg_shared_blocked);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TOTAL_HEADROOM_BLOCKED,
                               reject_reasons->is_total_headroom_blocked);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_PORT_PG_HEADROOM_BLOCKED,
                               reject_reasons->is_port_pg_headroom_blocked);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_VOQ_IN_GUARANTEED,
                               reject_reasons->is_voq_in_guaranteed);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSQ_GUARANTEED_STATUS,
                               reject_reasons->vsq_guaranteed_status);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REJECT, INST_SINGLE, reject);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * set vsq SRAM reject mapping
 */
shr_error_e
dnx_ingress_congestion_dbal_vsq_sram_reject_map_set(
    int unit,
    int admit_profile,
    const dnx_data_ingr_congestion_init_vsq_sram_rjct_map_t * reject_reasons,
    int reject)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_VSQ_SRAM_REJECT_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE, admit_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TOTAL_SHARED_BLOCKED,
                               reject_reasons->is_total_shared_blocked);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_PORT_PG_SHARED_BLOCKED,
                               reject_reasons->is_port_pg_shared_blocked);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TOTAL_HEADROOM_BLOCKED,
                               reject_reasons->is_total_headroom_blocked);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HEADROOM_EXTENSION_BLOCKED,
                               reject_reasons->is_headroom_extension_blocked);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_PORT_HEADROOM_BLOCKED,
                               reject_reasons->is_port_headroom_blocked);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_PG_HEADROOM_BLOCKED,
                               reject_reasons->is_pg_headroom_blocked);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_VOQ_IN_GUARANTEED,
                               reject_reasons->is_voq_in_guaranteed);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSQ_GUARANTEED_STATUS,
                               reject_reasons->vsq_guaranteed_status);

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REJECT, INST_SINGLE, reject);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * set reject mask
 */
shr_error_e
dnx_ingress_congestion_dbal_reject_mask_set(
    int unit,
    dbal_tables_e dbal_table_id,
    dnx_ingress_congestion_reject_mask_key_t * key,
    dnx_ingress_congestion_reject_mask_t * mask)
{
    uint32 entry_handle_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Setting key fields */
    switch (dbal_table_id)
    {
        case DBAL_TABLE_INGRESS_CONG_VOQ_GUARANTEED_RJCT_MASK:
        case DBAL_TABLE_INGRESS_CONG_VSQ_GUARANTEED_RJCT_MASK:
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_PDS_IN_GUARANTEED,
                                       key->guaranteed.sram_pds_in_guaranteed);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SRAM_BUFFERS_IN_GUARANTEED,
                                       key->guaranteed.sram_buffers_in_guaranteed);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_WORDS_IN_GUARANTEED,
                                       key->guaranteed.words_in_guaranteed);
            break;
        case DBAL_TABLE_INGRESS_CONG_PORT_BASED_VSQ_RJCT_MASK:
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ADMIT_PROFILE,
                                       key->port_based_vsq.admit_profile);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LOSSLESS, key->port_based_vsq.is_lossless);
            break;
        case DBAL_TABLE_INGRESS_CONG_PP_RJCT_MASK:
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_ADMIT_PROFILE, key->pp_admit_profile);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected table %d\n", dbal_table_id);
            break;
    }

    /** Setting value fields */
    for (i = 0; i < dnx_data_ingr_congestion.dbal.admission_test_nof_get(unit); i++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RJCT_BIT, i, mask->mask[i]);
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * set dram block mask
 */
shr_error_e
dnx_ingress_congestion_dbal_dram_block_mask_set(
    int unit,
    int dp,
    uint32 mask[])
{
    uint32 entry_handle_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_DRAM_BLOCK_BITMAP_MASK, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /** Setting value fields */
    for (i = 0; i < DNX_INGRESS_CONGESTION_DRAM_BLOCK_BITMAP_NOF; i++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RJCT_BIT, i, mask[i]);
    }

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * set dram soft block default parameters
 */
shr_error_e
dnx_ingress_congestion_dbal_dram_soft_block_defaults_set(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_DRAM_BLOCK_GLOBAL_PARAMS, &entry_handle_id));

    /** Setting key fields */

    /** Setting value fields */
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_MIN_FREE_SRAM_BUFFERS_FOR_SOFT_BLOCK,
                                               INST_SINGLE, DBAL_PREDEF_VAL_MAX_VALUE);

    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_MIN_FREE_SRAM_PDBS_FOR_SOFT_BLOCK,
                                               INST_SINGLE, DBAL_PREDEF_VAL_MAX_VALUE);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * set TAR FIFO size(s)
 */
shr_error_e
dnx_ingress_congestion_dbal_fifo_size_set(
    int unit,
    uint32 fifo_size)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_FIFO_SIZE, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_SIZE, 0, fifo_size);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - low level function to set mirror on drop group (reject bitmap, type) to HW
 */
shr_error_e
dnx_ingress_congestion_dbal_mirror_on_drop_group_set(
    int unit,
    uint32 flags,
    int group_id,
    bcm_cosq_mirror_on_drop_group_info_t * group_info)
{
    uint32 entry_handle_id;
    int drop_reason_id;
    dbal_enum_value_field_mirror_on_drop_group_type_e group_type;
    const dnx_data_stat_drop_reasons_drop_reason_index_t *drop_reason_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** configure drop group */

    /** allocate handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_GROUP, &entry_handle_id));

    /** set key - group id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP, group_id);

    /** first clear the group */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** set the given drop reasons accordingly, if given array is empty - the group will remain cleared */
    for (drop_reason_id = 0; drop_reason_id < group_info->drop_reasons_count; drop_reason_id++)
    {
        /** translate bcm drop reasons enum to hw index */
        drop_reason_index =
            dnx_data_stat.drop_reasons.drop_reason_index_get(unit, group_info->reasons_array[drop_reason_id]);

        /** set drop reason */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RJCT_MASK, drop_reason_index->index, TRUE);
    }

    /** translate bcm group type to dbal enum */
    switch (group_info->type)
    {
        case bcmCosqMirrorOnDropTypeGlobal:
        {
            group_type = DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_GLOBAL;
            break;
        }
        case bcmCosqMirrorOnDropTypeVoq:
        {
            group_type = DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_VOQ;
            break;
        }
        case bcmCosqMirrorOnDropTypePort:
        {
            group_type = DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PORT;
            break;
        }
        case bcmCosqMirrorOnDropTypePG:
        {
            group_type = DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PG;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Bad group_type=%d.\n", group_info->type);
        }
    }
    /** set chosen group type field */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP_TYPE, INST_SINGLE, group_type);

    /** commit group info */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - low level function to get mirror on drop group (reject bitmap, type) from HW
 */
shr_error_e
dnx_ingress_congestion_dbal_mirror_on_drop_group_get(
    int unit,
    uint32 flags,
    int group_id,
    bcm_cosq_mirror_on_drop_group_info_t * group_info)
{
    uint32 entry_handle_id;
    int drop_reason_id, hw_reason_index;
    uint32 enabled;
    const dnx_data_stat_drop_reasons_drop_reason_index_t *drop_reason_hw_index;
    dbal_enum_value_field_mirror_on_drop_group_type_e group_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** init */
    sal_memset(group_info, 0, sizeof(bcm_cosq_mirror_on_drop_group_info_t));

    /** allocate handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_GROUP, &entry_handle_id));

    /** set key - group id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GROUP, group_id);

    /** get reject types - iterate over all drop reasons and find those that are supported and enabled */
    for (drop_reason_id = 0; drop_reason_id < bcmCosqDropReasonRejectCount; drop_reason_id++)
    {
        /** translate bcm drop reason to hw index */
        drop_reason_hw_index = dnx_data_stat.drop_reasons.drop_reason_index_get(unit, drop_reason_id);

        hw_reason_index = drop_reason_hw_index->index;

        /** unsupported reason, skip it */
        if (hw_reason_index == -1)
        {
            continue;
        }

        /** Request to read enable field of current reject reason */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RJCT_MASK, hw_reason_index, &enabled);

        /** Get entry*/
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /** current drop reason is enabled - add it to array and increase count */
        if (enabled == TRUE)
        {
            /** set the enabled reject reason and increase counter of reasons */
            group_info->reasons_array[group_info->drop_reasons_count] = drop_reason_id;
            group_info->drop_reasons_count++;
        }
    }

    /** get type of group */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GROUP_TYPE, INST_SINGLE, &group_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** translate dbal group type to bcm enum */
    switch (group_type)
    {
        case DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_GLOBAL:
        {
            group_info->type = bcmCosqMirrorOnDropTypeGlobal;
            break;
        }
        case DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_VOQ:
        {
            group_info->type = bcmCosqMirrorOnDropTypeVoq;
            break;
        }
        case DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PORT:
        {
            group_info->type = bcmCosqMirrorOnDropTypePort;
            break;
        }
        case DBAL_ENUM_FVAL_MIRROR_ON_DROP_GROUP_TYPE_PG:
        {
            group_info->type = bcmCosqMirrorOnDropTypePG;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Bad group_type=%d.\n", group_type);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_mirror_on_drop_aging_set(
    int unit,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 entry_handle_id, nof_clock_cycles, max_val;
    dbal_fields_e field_id;
    dnxcmn_time_t time;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** allocate handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_AGING, &entry_handle_id));

    /** Set aging field according to type */
    switch (type)
    {
        case bcmCosqControlMirrorOnDropAgingGlobal:
        {
            field_id = DBAL_FIELD_GROUP_TYPE_GLOBAL;
            break;
        }
        case bcmCosqControlMirrorOnDropAgingVoq:
        {
            field_id = DBAL_FIELD_GROUP_TYPE_VOQ;
            break;
        }
        case bcmCosqControlMirrorOnDropAgingVsq:
        {
            type = field_id = DBAL_FIELD_GROUP_TYPE_VSQ;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong type=%d.\n", type);
        }
    }

    /** translate microseconds to clocks */
    COMPILER_64_SET(time.time, 0, arg);
    time.time_units = DNXCMN_TIME_UNIT_USEC;
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time, &nof_clock_cycles));

    /** Divide by resolution */
    nof_clock_cycles = nof_clock_cycles / dnx_data_ingr_congestion.mirror_on_drop.aging_clocks_resolution_get(unit);

    /** verify that the provided aging time doesn't exceed the maximal possible aging time (field size) */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_AGING,
                                                          DBAL_FIELD_GROUP_TYPE_GLOBAL /** field_id */ ,
                                                          0 /** is_key */ , 0, 0,
                                                          DBAL_PREDEF_VAL_MAX_VALUE /** max value of the field */ ,
                                                          &max_val));
    if (nof_clock_cycles > max_val)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid aging time=%d\n", arg);
    }

    /** set chosen group type field */
    dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, nof_clock_cycles);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_mirror_on_drop_aging_get(
    int unit,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32 entry_handle_id, nof_clock_cycles;
    dbal_fields_e field_id;
    dnxcmn_time_t time;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** allocate handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_AGING, &entry_handle_id));

    /** Set aging field according to type */
    switch (type)
    {
        case bcmCosqControlMirrorOnDropAgingGlobal:
        {
            field_id = DBAL_FIELD_GROUP_TYPE_GLOBAL;
            break;
        }
        case bcmCosqControlMirrorOnDropAgingVoq:
        {
            field_id = DBAL_FIELD_GROUP_TYPE_VOQ;
            break;
        }
        case bcmCosqControlMirrorOnDropAgingVsq:
        {
            type = field_id = DBAL_FIELD_GROUP_TYPE_VSQ;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong type=%d.\n", type);
        }
    }

    /** get aging */
    dbal_value_field32_request(unit, entry_handle_id, field_id, INST_SINGLE, &nof_clock_cycles);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Multiple by resolution to get clocks */
    nof_clock_cycles = nof_clock_cycles * dnx_data_ingr_congestion.mirror_on_drop.aging_clocks_resolution_get(unit);
    /** translate clocks to microseconds */
    SHR_IF_ERR_EXIT(dnxcmn_clock_cycles_to_time_get(unit, nof_clock_cycles, DNXCMN_TIME_UNIT_USEC, &time));

    *arg = COMPILER_64_LO(time.time);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_ingress_congestion_dbal_mirror_on_drop_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** allocate handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_MIRROR_ON_DROP_SHAPER, &entry_handle_id));

    /** set rate to max by default */
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - write ECN marking threshold to HW
 */

shr_error_e
dnx_ingress_congestion_dbal_ecn_marking_threshold_hw_set(
        int unit,
        int core,
        bcm_cosq_resource_t resource,
        uint32 set_threshold)
{
    uint32 entry_handle_id, encoded_set_threshold;
    dbal_fields_e field_id;
    dbal_tables_e table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);


    /** allocate handle*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_ECN_MARKING_THRESHOLDS_TABLE, &entry_handle_id));

    /** Set resource field according to type */
    SHR_IF_ERR_EXIT(dnx_cosq_voq_gl_ecn_resource_to_dbal_info(unit, resource, &table, &field_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** encode the value*/
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_encode(unit, set_threshold, &encoded_set_threshold));

    /** Setting value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, encoded_set_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - read ECN marking threshold from HW
 */

shr_error_e
dnx_ingress_congestion_dbal_ecn_marking_threshold_hw_get(
        int unit,
        int core,
        bcm_cosq_resource_t resource,
        uint32 *set_threshold,
        uint32 *clear_threshold)
{
    uint32 entry_handle_id, encoded_set_threshold;
    dbal_fields_e field_id;
    dbal_tables_e table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** allocate handle*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_CONG_GLOBAL_ECN_MARKING_THRESHOLDS_TABLE, &entry_handle_id));

    /** Set resource field according to type */
    SHR_IF_ERR_EXIT(dnx_cosq_voq_gl_ecn_resource_to_dbal_info(unit, resource, &table, &field_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Setting value fields */
    dbal_value_field32_request(unit, entry_handle_id, field_id, INST_SINGLE, &encoded_set_threshold);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Decode the received value */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_decode(unit, encoded_set_threshold,set_threshold));
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_threshold_decode(unit, encoded_set_threshold,clear_threshold));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}




