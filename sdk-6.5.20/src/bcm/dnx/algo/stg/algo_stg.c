/** \file algo_stg.c
 *
 *   Procedures for initializing STG resources.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  All Rights Reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR
/**
* INCLUDE FILES:
* {
*/

#include <soc/dnx/dnx_data/auto_generated/dnx_data_stg.h>

#include <bcm/types.h>
#include <bcm_int/dnx/algo/stg/algo_stg.h>
#include <shared/shrextend/shrextend_debug.h>

/**
 * }
 */

/**
 * see .h file for description
 */
shr_error_e
dnx_algo_stg_init(
    int unit)
{
    dnx_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init sw state stg_db.
     */
    SHR_IF_ERR_EXIT(algo_stg_db.init(unit));

    /*
     * STG_ID resource management - Allocate STG ID
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.flags = 0;
    /** The available STG IDs defined by BCM APIs are 1 ~ nof_topology_ids.*/
    data.first_element = 1;
    data.nof_elements = dnx_data_stg.stg.nof_topology_ids_get(unit);
    sal_strncpy(data.name, DNX_ALGO_RES_TYPE_STG_ID, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_stg_db.stg_id.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}
