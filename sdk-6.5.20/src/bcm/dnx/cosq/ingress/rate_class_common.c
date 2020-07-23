/** \file rate_class_common.c
 * 
 *
 * Common functionality for VOQ and VSQ rate classes
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
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>

#include "ingress_congestion.h"
#include "ingress_congestion_convert.h"
#include "ingress_congestion_dbal.h"

/**
 * \brief -
 *     Sets WRED parameters per rate-class and drop precedence,
 *     including wred-enable.
 */
int
dnx_rate_class_common_wred_set(
    int unit,
    dbal_tables_e table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    dnx_ingress_congestion_wred_info_t * wred)
{
    dnx_ingress_congestion_wred_hw_params_t wred_hw_params;

    SHR_FUNC_INIT_VARS(unit);

    dnx_ingress_congestion_convert_wred_to_hw_get(unit, wred, &wred_hw_params);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_wred_hw_set(unit, table_id, key, &wred_hw_params));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * get WRED parameters and translate them to user units
 */
int
dnx_rate_class_common_wred_get(
    int unit,
    dbal_tables_e table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    dnx_ingress_congestion_wred_info_t * info)
{
    dnx_ingress_congestion_wred_hw_params_t wred_hw_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_wred_hw_get(unit, table_id, key, &wred_hw_params));

    dnx_ingress_congestion_convert_hw_to_wred_get(unit, &wred_hw_params, info);

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -  Set WRED average configuration
 */
int
dnx_rate_class_common_wred_avrg_set(
    int unit,
    dbal_tables_e wred_table_id,
    dbal_tables_e avrg_table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    uint32 gain)
{
    dnx_ingress_congestion_wred_avrg_params_t avrg_params;

    int wred_any_enable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_wred_enable_hw_get(unit, wred_table_id, key, &wred_any_enable));

    /** WRED exp wq parameter is same for all dps and ECN */
    avrg_params.avrg_en = wred_any_enable;
    avrg_params.avrg_weight = gain;

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_wred_weight_hw_set(unit, avrg_table_id,
                                                                   key->core_id, key->rate_class, &avrg_params));

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief -  get WRED average configuration
 */
int
dnx_rate_class_common_wred_avrg_get(
    int unit,
    dbal_tables_e wred_table_id,
    dbal_tables_e avrg_table_id,
    dnx_ingress_congestion_dbal_wred_key_t * key,
    uint32 *gain)
{
    dnx_ingress_congestion_wred_avrg_params_t avrg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_wred_weight_hw_get(unit, avrg_table_id,
                                                                   key->core_id, key->rate_class, &avrg_params));
    *gain = avrg_params.avrg_weight;

exit:
    SHR_FUNC_EXIT;
}
