/** \file algo_ppmc.c
 * 
 *
 * Field procedures for DNX.
 *
 * Will hold the needed algorithm functions for PP Multicast module
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
/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/stat.h>
#include <bcm_int/dnx/algo/ppmc/algo_ppmc.h>
#include <bcm/multicast.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ppmc_access.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>

/*
 * }
 */

/**
 * Refer to the .h file for more information.
 */
shr_error_e
dnx_algo_ppmc_init(
    int unit)
{
    dnx_algo_res_create_data_t create_data;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init the pp multicast sw state db.
     */
    SHR_IF_ERR_EXIT(ppmc_db.init(unit));

    sal_memset(&create_data, 0, sizeof(dnx_algo_res_create_data_t));
    /** The valid range for MC replication indexes is between 0x300000 and 0x3BFFFF */
    create_data.first_element = dnx_data_ppmc.param.min_mc_replication_id_get(unit);
    create_data.flags = 0;
    create_data.nof_elements = dnx_data_ppmc.param.max_mc_replication_id_get(unit)
        - dnx_data_ppmc.param.min_mc_replication_id_get(unit) + 1;
    create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_BASIC;
    sal_strncpy(create_data.name, DNX_ALGO_PPMC, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(ppmc_db.ppmc_res_manager.create(unit, &create_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information.
 */
shr_error_e
dnx_algo_ppmc_mc_replication_id_alloc(
    int unit,
    uint32 flags,
    int *mc_replication_id)
{
    uint32 alloc_flags;
    SHR_FUNC_INIT_VARS(unit);

    alloc_flags = (_SHR_IS_FLAG_SET(flags, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID)) ? DNX_ALGO_RES_ALLOCATE_WITH_ID : 0;
    SHR_IF_ERR_EXIT(ppmc_db.ppmc_res_manager.allocate_single(unit, alloc_flags, 0, mc_replication_id));
exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information.
 */
shr_error_e
dnx_algo_ppmc_mc_replication_id_free(
    int unit,
    int mc_replication_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Deallocate the PPMC entry from the resource. */
    SHR_IF_ERR_EXIT(ppmc_db.ppmc_res_manager.free_single(unit, mc_replication_id));
exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information.
 */
shr_error_e
dnx_algo_ppmc_mc_replication_id_is_allocated(
    int unit,
    int mc_replication_id,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(ppmc_db.ppmc_res_manager.is_allocated(unit, mc_replication_id, is_allocated));
exit:
    SHR_FUNC_EXIT;
}

/**
 * Refer to the .h file for more information.
 */
shr_error_e
dnx_algo_ppmc_clear_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(ppmc_db.ppmc_res_manager.clear(unit));
exit:
    SHR_FUNC_EXIT;
}
