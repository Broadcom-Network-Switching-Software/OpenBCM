/** \file voq_rate_class.c
 *
 * 
 * VOQ rate class functionality
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_error.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>

#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>

#include <bcm_int/dnx/cosq/cosq_dbal_utils.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include "ingress_congestion.h"
#include "ingress_congestion_convert.h"
#include "ingress_congestion_dbal.h"

static const dbal_tables_e dnx_voq_rate_class_fadt_tail_drop_table_id[] = {
    /** total bytes */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_WORDS,
    /** SRAM buffers */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_BUFFERS,
    /** SRAM PDs */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_FADT_DROP_SRAM_PDS
};

static const dbal_tables_e dnx_voq_rate_class_guaranteed_table_id[] = {
    /** total bytes */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_WORDS,
    /** SRAM buffers */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_BUFFERS,
    /** SRAM PDs */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_GUARANTEED_SRAM_PDS
};

static const dbal_tables_e dnx_voq_rate_class_dram_bound_table_id[] = {
    /** SRAM bytes */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_WORDS,
    /** SRAM buffers */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_BUFFERS,
    /** SRAM PDs */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_BOUND_SRAM_PDS
};

static const dbal_tables_e dnx_voq_rate_class_dram_recovery_fail_table_id[] = {
    /** SRAM bytes */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_WORDS,
    /** SRAM buffers */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_BUFFERS,
    /** SRAM PDs */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_DRAM_RECOVERY_FAIL_SRAM_PDS
};

static const dbal_tables_e dnx_voq_rate_class_congestion_notification_table_id[] = {
    /** SRAM bytes */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_WORDS,
    /** SRAM buffers */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_BUFERS,
    /** SRAM PDs */
    DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_CONGESTION_NOTIFICATION_SRAM_PDS
};

/**
 * \brief - set fadt tail drop thresholds to sw state - per rate class per drop precedence
 */
static shr_error_e
dnx_voq_rate_class_fadt_tail_drop_sw_state_set(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rate_class,
    uint32 drop_precedence,
    bcm_cosq_fadt_threshold_t * fadt)
{
    bcm_cosq_fadt_threshold_t fadt_rounded = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** save to SW state values that are rounded according to HW granularity */
    fadt_rounded.thresh_max =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, fadt->thresh_max);
    fadt_rounded.thresh_max =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, fadt_rounded.thresh_max);
    fadt_rounded.thresh_min =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, fadt->thresh_min);
    fadt_rounded.thresh_min =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, fadt_rounded.thresh_min);
    fadt_rounded.alpha = fadt->alpha;

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.fadt_tail_drop_thresholds.resource.
                    dp.set(unit, rate_class, resource, drop_precedence, fadt_rounded));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_voq_rate_class_fadt_tail_drop_hw_set(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rate_class,
    uint32 drop_precedence,
    bcm_cosq_fadt_threshold_t * fadt)
{
    dbal_tables_e table_id;
    bcm_cosq_fadt_threshold_t hw_fadt;
    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_voq_rate_class_fadt_tail_drop_table_id[resource];

    hw_fadt.thresh_max = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, fadt->thresh_max);
    hw_fadt.thresh_min = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, fadt->thresh_min);
    hw_fadt.alpha = fadt->alpha;

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_fadt_drop_hw_set
                    (unit, table_id, rate_class, drop_precedence, &hw_fadt));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Gets FADT drop parameters from HW - per rate-class and drop precedence.
 * The FADT drop mechanism drops packets that are mapped to queues that exceed thresholds
 * of this structure.
 */
int
dnx_voq_rate_class_fadt_tail_drop_hw_get(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    uint32 dp,
    bcm_cosq_fadt_threshold_t * fadt)
{
    dbal_tables_e table_id;
    bcm_cosq_fadt_threshold_t hw_fadt;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_voq_rate_class_fadt_tail_drop_table_id[resource];

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_voq_fadt_drop_hw_get(unit, table_id, rt_cls, dp, &hw_fadt));

    fadt->thresh_max = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, hw_fadt.thresh_max);
    fadt->thresh_min = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, hw_fadt.thresh_min);
    fadt->alpha = hw_fadt.alpha;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Sets FADT drop parameters - per rate-class and drop precedence.
 * The FADT drop mechanism drops packets that are mapped to queues that exceed thresholds
 * of this structure.
 * This function is mutex protected and updates SW-State
 */
int
dnx_voq_rate_class_fadt_tail_drop_set(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    uint32 dp,
    bcm_cosq_fadt_threshold_t * fadt)
{
    int is_mutex_locked = FALSE;
    int dram_in_use;
    bcm_cosq_fadt_threshold_t default_tail_drop_fadt_ocb_only_thresholds = {
        .thresh_min = 0,
        .thresh_max = dnx_data_ingr_congestion.fadt_tail_drop.default_max_size_byte_threshold_for_ocb_only_get(unit),
        .alpha = 0,
        .resource_range_max = 0,
        .resource_range_min = 0,
    };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Take mutex
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.fadt_mutex.take(unit, sal_mutex_FOREVER));
    is_mutex_locked = TRUE;

    /*
     * Update SW-State
     */
    SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_tail_drop_sw_state_set(unit, resource, rt_cls, dp, fadt));

    /*
     * Check sw-state if need to update HW
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_in_use.get(unit, &dram_in_use));
    if (!dram_in_use && resource == DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES &&
        fadt->thresh_max > default_tail_drop_fadt_ocb_only_thresholds.thresh_max)
    {
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_tail_drop_hw_set(unit, resource, rt_cls, dp,
                                                                 &default_tail_drop_fadt_ocb_only_thresholds));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_tail_drop_hw_set(unit, resource, rt_cls, dp, fadt));
    }

exit:
    if (is_mutex_locked)
    {
        /** give the mutex before exiting */
        SHR_IF_ERR_CONT(dnx_ingress_congestion_db.sync_manager.fadt_mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Gets FADT drop parameters from SW state - per rate-class and drop precedence.
 * The FADT drop mechanism drops packets that are mapped to queues that exceed thresholds
 * of this structure.
 */
int
dnx_voq_rate_class_fadt_tail_drop_sw_get(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    uint32 dp,
    bcm_cosq_fadt_threshold_t * fadt)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.fadt_tail_drop_thresholds.resource.
                    dp.get(unit, rt_cls, resource, dp, fadt));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set the size of committed queue size (i.e., the
 * guaranteed memory) for each VOQ, even in the case that a
 * set of queues consume most of the memory resources.
 *
 * Note: threshold value is in HW granularity.
 */
int
dnx_voq_rate_class_guaranteed_set(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    uint32 guaranteed_thresh)
{
    dbal_tables_e table_id;

    int nof_keys = 1;
    dnx_cosq_dbal_field_t key[1];
    dnx_cosq_dbal_field_t result;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_voq_rate_class_guaranteed_table_id[resource];

    /** key -- rate class */
    key[0].id = DBAL_FIELD_VOQ_RATE_CLASS;
    key[0].value = rt_cls;

    /** results */
    result.id = DBAL_FIELD_GUARANTEED;
    result.value = guaranteed_thresh;

    SHR_IF_ERR_EXIT(dnx_cosq_dbal_entry_set(unit, table_id, nof_keys, key, &result));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get the size of committed queue size (i.e., the
 * guaranteed memory) for each VOQ, even in the case that a
 * set of queues consume most of the memory resources.
 *
 * Note: returned value is in HW granularity.
 */
int
dnx_voq_rate_class_guaranteed_get(
    int unit,
    dnx_ingress_congestion_resource_type_e resource,
    uint32 rt_cls,
    uint32 *guaranteed_thresh)
{
    dbal_tables_e table_id;

    int nof_keys = 1;
    dnx_cosq_dbal_field_t key[1];
    dnx_cosq_dbal_field_t result;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_voq_rate_class_guaranteed_table_id[resource];

    /** key -- rate class */
    key[0].id = DBAL_FIELD_VOQ_RATE_CLASS;
    key[0].value = rt_cls;

    /** results */
    result.id = DBAL_FIELD_GUARANTEED;

    SHR_IF_ERR_EXIT(dnx_cosq_dbal_entry_get(unit, table_id, nof_keys, key, &result));

    *guaranteed_thresh = result.value;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set ECN static drop threshold
 */
int
dnx_voq_rate_class_ecn_drop_set(
    int unit,
    uint32 rt_cls,
    uint32 ecn_drop_thresh)
{
    dbal_tables_e table_id;

    int nof_keys = 1;
    dnx_cosq_dbal_field_t key[1];
    dnx_cosq_dbal_field_t result;

    SHR_FUNC_INIT_VARS(unit);

    table_id = DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN;

    /** key -- rate class */
    key[0].id = DBAL_FIELD_RATE_CLASS;
    key[0].value = rt_cls;

    /** results */
    result.id = DBAL_FIELD_DROP_TH;
    result.value = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit,
                                                                            DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                            ecn_drop_thresh);

    SHR_IF_ERR_EXIT(dnx_cosq_dbal_entry_set(unit, table_id, nof_keys, key, &result));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get ECN static drop threshold
 */
int
dnx_voq_rate_class_ecn_drop_get(
    int unit,
    uint32 rt_cls,
    uint32 *ecn_drop_thresh)
{
    dbal_tables_e table_id;

    int nof_keys = 1;
    dnx_cosq_dbal_field_t key[1];
    dnx_cosq_dbal_field_t result;

    SHR_FUNC_INIT_VARS(unit);

    table_id = DBAL_TABLE_INGRESS_CONG_VOQ_RATE_CLASS_WRED_ECN;

    /** key -- rate class */
    key[0].id = DBAL_FIELD_RATE_CLASS;
    key[0].value = rt_cls;

    /** results */
    result.id = DBAL_FIELD_DROP_TH;

    SHR_IF_ERR_EXIT(dnx_cosq_dbal_entry_get(unit, table_id, nof_keys, key, &result));

    *ecn_drop_thresh = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit,
                                                                                DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES,
                                                                                result.value);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Sets congestion notification FADT drop parameters - per rate class profile.
 * The FADT drop mechanism drops packets that are mapped to queues that exceed thresholds
 * of this structure.
 */
shr_error_e
dnx_voq_rate_class_congestion_notification_fadt_set(
    int unit,
    int rate_class,
    dnx_ingress_congestion_resource_type_e resource,
    bcm_cosq_fadt_threshold_t * fadt)
{
    int is_res_range = FALSE;
    dbal_tables_e table_id;
    bcm_cosq_fadt_threshold_t hw_fadt = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_voq_rate_class_congestion_notification_table_id[resource];
    /*
     * Convert DRAM thresholds to internal values (HW resolution)
     */
    hw_fadt.thresh_max = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, fadt->thresh_max);
    hw_fadt.thresh_min = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, resource, fadt->thresh_min);
    hw_fadt.alpha = fadt->alpha;

    /*
     * Write values to HW
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_fadt_hw_set(unit, table_id, rate_class, is_res_range, &hw_fadt));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Sets congestion notification FADT drop parameters - per rate class profile.
 * The FADT drop mechanism drops packets that are mapped to queues that exceed thresholds
 * of this structure.
 */
shr_error_e
dnx_voq_rate_class_congestion_notification_fadt_get(
    int unit,
    int rate_class,
    dnx_ingress_congestion_resource_type_e resource,
    bcm_cosq_fadt_threshold_t * fadt)
{
    int is_res_range = FALSE;
    dbal_tables_e table_id;
    bcm_cosq_fadt_threshold_t hw_fadt = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_voq_rate_class_congestion_notification_table_id[resource];

    /*
     * Get values from HW
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_fadt_hw_get(unit, table_id, rate_class, is_res_range, &hw_fadt));
    /*
     * Convert internal values (HW resolution) to thresholds
     */
    fadt->thresh_max = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, hw_fadt.thresh_max);
    fadt->thresh_min = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, resource, hw_fadt.thresh_min);
    fadt->alpha = hw_fadt.alpha;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Sets DRAM FADT drop parameters - per rate class profile.
 * The FADT drop mechanism drops packets that are mapped to queues that exceed thresholds
 * of this structure.
 */
static shr_error_e
dnx_voq_rate_class_fadt_dram_set(
    int unit,
    dbal_tables_e table_id,
    int rate_class,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    bcm_cosq_fadt_threshold_t * fadt)
{
    int is_res_range = FALSE;
    bcm_cosq_fadt_threshold_t hw_fadt = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    is_res_range = dnx_data_ingr_congestion.dram_bound.resource_get(unit, resource)->is_resource_range;

    /*
     * Convert DRAM thresholds to internal values (HW resolution)
     */
    SHR_IF_ERR_EXIT(dnx_ingress_cogestion_convert_fadt_dram_threshold_to_internal_get(unit, resource, fadt, &hw_fadt));

    /*
     * Write values to HW
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_fadt_hw_set(unit, table_id, rate_class, is_res_range, &hw_fadt));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Gets DRAM FADT drop parameters - per rate class profile.
 * The FADT drop mechanism drops packets that are mapped to queues that exceed thresholds
 * of this structure.
 */
static shr_error_e
dnx_voq_rate_class_fadt_dram_get(
    int unit,
    dbal_tables_e table_id,
    int rate_class,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    bcm_cosq_fadt_threshold_t * fadt)
{
    int is_res_range = FALSE;
    bcm_cosq_fadt_threshold_t hw_fadt = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(fadt, 0, sizeof(bcm_cosq_fadt_threshold_t));

    is_res_range = dnx_data_ingr_congestion.dram_bound.resource_get(unit, resource)->is_resource_range;
    /*
     * Get Values from HW
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_fadt_hw_get(unit, table_id, rate_class, is_res_range, &hw_fadt));

    /*
     * Convert to DRAM threshold values
     */
    SHR_IF_ERR_EXIT(dnx_ingress_cogestion_convert_fadt_internal_to_dram_threshold_get(unit, resource, &hw_fadt, fadt));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Sets DRAM bound FADT drop parameters - per rate-class.
 */
shr_error_e
dnx_voq_rate_class_fadt_dram_bound_set(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    uint32 rt_cls,
    bcm_cosq_fadt_threshold_t * fadt)
{
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_voq_rate_class_dram_bound_table_id[resource];

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_set(unit, table_id, rt_cls, resource, fadt));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Gets DRAM bound FADT drop parameters - per rate-class.
 */
shr_error_e
dnx_voq_rate_class_fadt_dram_bound_get(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    uint32 rt_cls,
    bcm_cosq_fadt_threshold_t * fadt)
{
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_voq_rate_class_dram_bound_table_id[resource];

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_get(unit, table_id, rt_cls, resource, fadt));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Sets DRAM recovery fail FADT drop parameters - per rate-class.
 */
shr_error_e
dnx_voq_rate_class_fadt_dram_recovery_fail_set(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    uint32 rt_cls,
    bcm_cosq_fadt_threshold_t * fadt)
{
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_voq_rate_class_dram_recovery_fail_table_id[resource];

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_set(unit, table_id, rt_cls, resource, fadt));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Gets DRAM recovery fail FADT drop parameters - per rate-class.
 */
shr_error_e
dnx_voq_rate_class_fadt_dram_recovery_fail_get(
    int unit,
    dnx_ingress_congestion_dram_bound_resource_type_e resource,
    uint32 rt_cls,
    bcm_cosq_fadt_threshold_t * fadt)
{
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_voq_rate_class_dram_recovery_fail_table_id[resource];

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_dram_get(unit, table_id, rt_cls, resource, fadt));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Sets DRAM recovery parameters - per rate-class.
 */
shr_error_e
dnx_voq_rate_class_dram_recovery_set(
    int unit,
    uint32 rt_cls,
    uint32 recovery_threshold)
{
    uint32 hw_recovery_thr;
    dnx_ingress_congestion_dram_bound_resource_type_e resource = DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES;
    SHR_FUNC_INIT_VARS(unit);

    hw_recovery_thr = DNX_INGRESS_CONGESTION_CONVERT_DRAM_THRESHOLD_TO_INTERNAL_GET(unit, resource, recovery_threshold);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_dram_recovery_set(unit, rt_cls, hw_recovery_thr));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Gets DRAM recovery parameters - per rate-class.
 */
shr_error_e
dnx_voq_rate_class_dram_recovery_get(
    int unit,
    uint32 rt_cls,
    uint32 *recovery_threshold)
{
    uint32 hw_recovery_thr;
    dnx_ingress_congestion_dram_bound_resource_type_e resource = DNX_INGRESS_CONGESTION_DRAM_BOUND_RESOURCE_SRAM_BYTES;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_dram_recovery_get(unit, rt_cls, &hw_recovery_thr));

    *recovery_threshold =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_DRAM_THRESHOLD_GET(unit, resource, hw_recovery_thr);
exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify function for dnx_voq_rate_class_color_size_set
 */
static shr_error_e
dnx_voq_rate_class_color_size_set_verify(
    int unit,
    bcm_gport_t gport,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size)
{
    int rate_class, guaranteed_change;
    int alpha_max = 0, alpha_min = 0;
    int is_dram_present = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    dnx_ingress_congestion_resource_type_e res_type = DNX_INGRESS_CONGESTION_RESOURCE_INVALID;
    uint32 old_guaranteed, new_guaranteed, max_res, rate_class_nof_queues;
    int core_idx;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get threshold type from flags
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_color_size_res_type_get(unit, flags, &res_type));

    /*
     * Verify gport size
     */
    max_res = dnx_data_ingr_congestion.info.resource_get(unit, res_type)->max;
    if (gport_size->size_min > max_res || gport_size->size_max > max_res || gport_size->size_fadt_min > max_res)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Gport size exceeds max resources %d", max_res);
    }
    /*
     * if there is no DRAM, total bytes threshold should be static (other resources are in OCB)
     */
    if (!is_dram_present &&
        res_type == DNX_INGRESS_CONGESTION_RESOURCE_TOTAL_BYTES && gport_size->size_fadt_min != gport_size->size_max)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Total bytes threshold should be static when no DRAMs present");
    }
    /*
     * Verify alpha
     */
    alpha_min = dnx_data_ingr_congestion.info.resource_get(unit, res_type)->fadt_alpha_min;
    alpha_max = dnx_data_ingr_congestion.info.resource_get(unit, res_type)->fadt_alpha_max;
    if (gport_size->size_alpha_max < alpha_min || gport_size->size_alpha_max > alpha_max)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Alpha value %d is invalid. Alpha should be in range %d-%d",
                     gport_size->size_alpha_max, alpha_min, alpha_max);
    }

    /*
     * Get current guaranteed for the rate class (in HW resolution)
     */
    rate_class = BCM_GPORT_PROFILE_GET(gport);

    /*
     * Verify guaranteed for all cores
     */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.
                        ref_count.get(unit, core_idx, rate_class, &rate_class_nof_queues));
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_guaranteed_get(unit, res_type, rate_class, &old_guaranteed));

        new_guaranteed = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, res_type, gport_size->size_min);
        guaranteed_change = (new_guaranteed - old_guaranteed) * rate_class_nof_queues;

        SHR_IF_ERR_EXIT(dnx_ingress_congestion_res_type_guaranteed_verify(unit, core_idx, res_type, guaranteed_change));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_voq_rate_class_color_size_set(
    int unit,
    bcm_gport_t gport,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size)
{
    bcm_cosq_fadt_threshold_t fadt_tail_drop;
    dnx_ingress_congestion_resource_type_e res_type = DNX_INGRESS_CONGESTION_RESOURCE_INVALID;
    uint32 rate_class_nof_queues, old_guaranteed, new_guaranteed;
    int is_color_blind = 0;
    int guaranteed_change = 0;
    int rate_class, first_dp, last_dp, dp;
    int core_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_voq_rate_class_color_size_set_verify(unit, gport, color, flags, gport_size));

    rate_class = BCM_GPORT_PROFILE_GET(gport);

    is_color_blind = (flags & BCM_COSQ_GPORT_SIZE_COLOR_BLIND);

    /*
     * Get resource type from flags
     */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_color_size_res_type_get(unit, flags, &res_type));
    /*
     * Get current guaranteed for the rate class (in HW resolution)
     */
    SHR_IF_ERR_EXIT(dnx_voq_rate_class_guaranteed_get(unit, res_type, rate_class, &old_guaranteed));

    /*
     * Set tail drop thresholds
     */
    fadt_tail_drop.thresh_max = gport_size->size_max;
    fadt_tail_drop.thresh_min = gport_size->size_fadt_min;
    fadt_tail_drop.alpha = gport_size->size_alpha_max;

    if (is_color_blind)
    {
        first_dp = 0;
        last_dp = DNX_COSQ_NOF_DP - 1;
    }
    else
    {
        first_dp = last_dp = color;
    }
    for (dp = first_dp; dp <= last_dp; ++dp)
    {
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_tail_drop_set(unit, res_type, rate_class, dp, &fadt_tail_drop));
    }

    /*
     * Set current Guaranteed - we need to set the guaranteed in HW resolution
     */
    new_guaranteed = DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, res_type, gport_size->size_min);
    SHR_IF_ERR_EXIT(dnx_voq_rate_class_guaranteed_set(unit, res_type, rate_class, new_guaranteed));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.voq_rate_class.
                        ref_count.get(unit, core_idx, rate_class, &rate_class_nof_queues));

        /*
         * Calculate guaranteed change.
         * since guaranteed size can theoretically reach 4G, we might need for the calculation more than 32 bits.
         * however, since in HW we have different granularity, we will do all internal calculations
         * according to HW granularity (which is taken from DNX data)
         */
        guaranteed_change = (new_guaranteed - old_guaranteed) * rate_class_nof_queues;
        /*
         * Update HW with the new total guaranteed
         */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_shared_res_reject_status_thr_set
                        (unit, core_idx, res_type, guaranteed_change));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief -
 * Gets FADT drop and guaranteed parameters - per rate-class and drop precedence.
 * 
 * \param [in] unit - unit.
 * \param [in] gport - VOQ rate class gport
 * \param [in] color - DP
 * \param [in] flags - API flags
 * \param [in] get_hw - should get be done from HW
 * \param [out] gport_size - FADT drop and guaranteed parameters
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_voq_rate_class_color_size_dual_get(
    int unit,
    bcm_gport_t gport,
    bcm_color_t color,
    uint32 flags,
    int get_hw,
    bcm_cosq_gport_size_t * gport_size)
{
    int rate_class, dp;
    int is_color_blind = 0;
    uint32 current_guaranteed;
    dnx_ingress_congestion_resource_type_e res_type = DNX_INGRESS_CONGESTION_RESOURCE_INVALID;
    bcm_cosq_fadt_threshold_t fadt_tail_drop;

    SHR_FUNC_INIT_VARS(unit);

    rate_class = BCM_GPORT_PROFILE_GET(gport);
    is_color_blind = (flags & BCM_COSQ_GPORT_SIZE_COLOR_BLIND);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_color_size_res_type_get(unit, flags, &res_type));

    dp = is_color_blind ? 0 : color;
    if (get_hw)
    {
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_tail_drop_hw_get(unit, res_type, rate_class, dp, &fadt_tail_drop));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_voq_rate_class_fadt_tail_drop_sw_get(unit, res_type, rate_class, dp, &fadt_tail_drop));
    }
    gport_size->size_max = fadt_tail_drop.thresh_max;
    gport_size->size_fadt_min = fadt_tail_drop.thresh_min;
    gport_size->size_alpha_max = fadt_tail_drop.alpha;

    /*
     * Get Guaranteed threshold
     */
    SHR_IF_ERR_EXIT(dnx_voq_rate_class_guaranteed_get(unit, res_type, rate_class, &current_guaranteed));
    gport_size->size_min = DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, res_type, current_guaranteed);

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_voq_rate_class_color_size_get(
    int unit,
    bcm_gport_t gport,
    bcm_color_t color,
    uint32 flags,
    bcm_cosq_gport_size_t * gport_size)
{
    int get_hw = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_voq_rate_class_color_size_dual_get(unit, gport, color, flags, get_hw, gport_size));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_voq_rate_class_ocb_only_set(
    int unit,
    int rate_class,
    int is_ocb_only)
{
    dbal_enum_value_field_dram_use_mode_e dram_use_mode;
    SHR_FUNC_INIT_VARS(unit);

    dram_use_mode = (is_ocb_only) ? DBAL_ENUM_FVAL_DRAM_USE_MODE_SRAM_ONLY : DBAL_ENUM_FVAL_DRAM_USE_MODE_NORMAL;

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_dram_use_mode_set(unit, rate_class, dram_use_mode));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_voq_rate_class_ocb_only_get(
    int unit,
    int rate_class,
    int *is_ocb_only)
{
    dbal_enum_value_field_dram_use_mode_e dram_use_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_dram_use_mode_get(unit, rate_class, &dram_use_mode));

    *is_ocb_only = (dram_use_mode == DBAL_ENUM_FVAL_DRAM_USE_MODE_SRAM_ONLY);

exit:
    SHR_FUNC_EXIT;

}
