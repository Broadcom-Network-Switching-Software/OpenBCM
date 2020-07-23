/** \file global_vsq.c
 * 
 *
 * Global VSQ functionality for DNX.
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
#include "ingress_congestion_dbal.h"
#include "ingress_congestion_convert.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>

static const dbal_tables_e dnx_global_vsq_resource_alloc_table_id[] = {
    DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_WORDS,
    DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_BUFFERS,
    DBAL_TABLE_INGRESS_CONG_GLOBAL_VSQ_RESOURCE_ALLOC_SRAM_PDS
};

/**
 * \brief -
 * Set global VSQ resource allocation
 */
shr_error_e
dnx_global_vsq_resource_alloc_set(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    int pool_id,
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t * resource_alloc_params)
{
    dbal_tables_e table_id;
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t resource_alloc_hw_params;
    int is_lossless;
    int lossless_pool_id;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_global_vsq_resource_alloc_table_id[rsrc_type];

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.lossless_pool_id.get(unit, core, &lossless_pool_id));
    is_lossless = (lossless_pool_id == pool_id);

    resource_alloc_hw_params.shared_pool =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, rsrc_type, resource_alloc_params->shared_pool);

    resource_alloc_hw_params.nominal_headroom =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, rsrc_type,
                                                                 resource_alloc_params->nominal_headroom);

    resource_alloc_hw_params.max_headroom =
        DNX_INGRESS_CONGESTION_CONVERT_THRESHOLD_TO_INTERNAL_GET(unit, rsrc_type, resource_alloc_params->max_headroom);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_global_vsq_resource_alloc_hw_set(unit, core, table_id,
                                                                                 pool_id, is_lossless,
                                                                                 &resource_alloc_hw_params));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get global VSQ resource allocation.
 * return values in HW units
 */
shr_error_e
dnx_global_vsq_resource_alloc_hw_get(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    int pool_id,
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t * resource_alloc_params)
{
    dbal_tables_e table_id;
    int is_lossless;
    int lossless_pool_id;

    SHR_FUNC_INIT_VARS(unit);

    table_id = dnx_global_vsq_resource_alloc_table_id[rsrc_type];

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.port_based_vsq.lossless_pool_id.get(unit, core, &lossless_pool_id));
    is_lossless = (lossless_pool_id == pool_id);

    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_global_vsq_resource_alloc_hw_get(unit, core, table_id,
                                                                                 pool_id, is_lossless,
                                                                                 resource_alloc_params));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set global VSQ resource allocation
 */
shr_error_e
dnx_global_vsq_resource_alloc_get(
    int unit,
    int core,
    dnx_ingress_congestion_resource_type_e rsrc_type,
    int pool_id,
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t * resource_alloc_params)
{
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t resource_alloc_hw_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_global_vsq_resource_alloc_hw_get(unit, core, rsrc_type, pool_id, &resource_alloc_hw_params));

    resource_alloc_params->shared_pool =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, rsrc_type, resource_alloc_hw_params.shared_pool);

    resource_alloc_params->nominal_headroom =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, rsrc_type,
                                                                 resource_alloc_hw_params.nominal_headroom);

    resource_alloc_params->max_headroom =
        DNX_INGRESS_CONGESTION_CONVERT_INTERNAL_TO_THRESHOLD_GET(unit, rsrc_type,
                                                                 resource_alloc_hw_params.max_headroom);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * clear global VSQ resource allocation
 *
 * VSQ resource allocation registers have reset value of maximum.
 * It does not allow to call resource allocation API, since sum of all parts is above the total.
 * Clear the thresholds here. 
 * Actual resource allocation would be done in Tune module.
 */
shr_error_e
dnx_global_vsq_resource_alloc_clear(
    int unit)
{
    dbal_tables_e table_id;
    dnx_ingress_congestion_global_vsq_resource_alloc_params_t resource_alloc_params;
    dnx_ingress_congestion_resource_type_e rsrc_type;
    int pool_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&resource_alloc_params, 0x0, sizeof(resource_alloc_params));

    for (rsrc_type = 0; rsrc_type < DNX_INGRESS_CONGESTION_RESOURCE_NOF; rsrc_type++)
    {
        table_id = dnx_global_vsq_resource_alloc_table_id[rsrc_type];

        for (pool_id = 0; pool_id < dnx_data_ingr_congestion.vsq.pool_nof_get(unit); pool_id++)
        {

            SHR_IF_ERR_EXIT(dnx_ingress_congestion_dbal_global_vsq_resource_alloc_hw_set(unit, BCM_CORE_ALL, table_id,
                                                                                         pool_id, TRUE,
                                                                                         &resource_alloc_params));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
