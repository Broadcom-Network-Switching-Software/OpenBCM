/** \file algo_multicast.c
 *
 * Wrapper functions for tag bitmap allocation manager used by FEC PATH SELECT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FAILOVER
/*
 * INCLUDE FILES:
 * {
 */
#include <bcm_int/dnx/algo/multicast/algo_multicast.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
/*
 * }
 */

/*
 * See .h
 */
shr_error_e
dnx_algo_multicast_bitmap_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(create_data, _SHR_E_PARAM, "create_data");

    if (dnx_data_multicast.params.mcdb_allocation_method_get(unit) == DNX_ALGO_MULTICAST_ALLOCATION_BASIC)
    {
        SHR_IF_ERR_EXIT(resource_tag_bitmap_create
                        (unit, module_id, res_tag_bitmap, create_data, extra_arguments, nof_elements, alloc_flags));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Multicast allocation method is not supported %u",
                     dnx_data_multicast.params.mcdb_allocation_method_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}
