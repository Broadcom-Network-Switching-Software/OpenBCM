
/** \file algo/sch/sch_alloc_mngr.c
 *
 * Implementation for advanced sch resource manager.
 *
 * Manage flows allocation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/**
* Include files:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <bcm_int/dnx/algo/sch/sch_alloc_mngr.h>
#include <bcm_int/dnx/cosq/cosq.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_sch_alloc_mngr_access.h>

/** 
 * } 
 */

/**
* Structs:
* {
*/

/** 
 * } 
 */

/**
* Static Functions:
* {
*/

/** 
 * } 
 */

/**
* Functions:
* {
*/

/**
 * See .h file.
 */
shr_error_e
dnx_algo_sch_bitmap_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    uint8 tag_grain;
    int max_tag_value;
    int nof_remote_cores;
    dnx_sch_region_type_e region_type;
    dnx_algo_res_create_data_t res_tag_bitmap_create_info;
    resource_tag_bitmap_extra_arguments_create_info_t extra_create_info;
    dnx_algo_sch_extra_args_t *extra_args;
    int region_index;

    SHR_FUNC_INIT_VARS(unit);

    extra_args = (dnx_algo_sch_extra_args_t *) extra_arguments;
    region_index = extra_args->region_index;

    nof_remote_cores = dnx_data_sch.flow.nof_remote_cores_get(unit, extra_args->core_id, region_index)->val;
    region_type = dnx_data_sch.flow.region_type_get(unit, extra_args->core_id, region_index)->type;

    /** consider pass this information from the module level in tag_info array per tag type */
    if (region_type == DNX_SCH_REGION_TYPE_SE || region_type == DNX_SCH_REGION_TYPE_SE_HR)
    {
        /** On Scheduling Elements region, tag is used to mark shared shaper */
        tag_grain = dnx_data_sch.sch_alloc.tag_size_aggregate_se_get(unit);
        max_tag_value = dnx_data_sch.sch_alloc.shared_shaper_max_tag_value_get(unit);
    }
    else
    {
        /** on other regions (connectors and interdigitated), tag is used to mark remote modid */
        tag_grain = dnx_data_sch.sch_alloc.tag_size_con_get(unit);
        max_tag_value = dnx_data_device.general.nof_faps_get(unit) - 1;
    }

    sal_memset(&extra_create_info, 0, sizeof(extra_create_info));
    extra_create_info.grain_size = tag_grain;
    extra_create_info.max_tag_value = max_tag_value;

    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(res_tag_bitmap_create_info));
    res_tag_bitmap_create_info.first_element = create_data->first_element;
    res_tag_bitmap_create_info.nof_elements = create_data->nof_elements / nof_remote_cores;

    /*
     * we don't use allocation with ignore tag.
     * if this functionality is required, RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG should be specified
     */
    res_tag_bitmap_create_info.flags = RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create(unit, module_id, res_tag_bitmap,
                                               &res_tag_bitmap_create_info, &extra_create_info, nof_elements,
                                               alloc_flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_algo_sch_bitmap_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element)
{
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_free_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Free the allocated bits 
     */
    sal_memset(&res_tag_bitmap_free_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_free_info.pattern = 1;
    res_tag_bitmap_free_info.length = 1;
    res_tag_bitmap_free_info.repeats = 1;
    res_tag_bitmap_free_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_SPARSE;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_advanced_free
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_free_info, element));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_sch_bitmap_is_allocated(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint8 *is_allocated)
{
    resource_tag_bitmap_create_info_t create_info;
    int base_element;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create_info_get(unit, module_id, res_tag_bitmap, &create_info));

    base_element = element % create_info.count;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_is_allocated(unit, module_id, res_tag_bitmap, base_element, is_allocated));
exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file.
 */
shr_error_e
dnx_algo_sch_bitmap_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *alloc_info,
    int *element)
{
    dnx_algo_sch_alloc_info_t *alloc_info_p = (dnx_algo_sch_alloc_info_t *) alloc_info;
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
    int with_id, check_only;
    int indx;
    shr_error_e rc;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));

    /*
     * Translate the allocation flags.
     * The input flags are of type DNX_ALGO_RES_ALLOCATE_*, but the sw_state_res_tag_bitmap use a
     * different set of flags.
     */
    res_tag_bitmap_alloc_info.flags = RESOURCE_TAG_BITMAP_ALLOC_SPARSE;

    /** With ID */
    with_id = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID);
    res_tag_bitmap_alloc_info.flags |= (with_id) ? RESOURCE_TAG_BITMAP_ALLOC_WITH_ID : 0;

    /** Ignore Tag */
    if (alloc_info_p->ignore_tag)
    {
        res_tag_bitmap_alloc_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG;
    }

    /** Check only */
    check_only = _SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_SIMULATION);
    if (check_only)
    {
        res_tag_bitmap_alloc_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY;
    }

    res_tag_bitmap_alloc_info.align = alloc_info_p->align;
    for (indx = 0; indx < alloc_info_p->nof_offsets; indx++)
    {
        res_tag_bitmap_alloc_info.offs[indx] = alloc_info_p->offs[indx];
    }
    res_tag_bitmap_alloc_info.nof_offsets = alloc_info_p->nof_offsets;
    res_tag_bitmap_alloc_info.tag = alloc_info_p->tag;
    res_tag_bitmap_alloc_info.pattern = alloc_info_p->pattern;
    res_tag_bitmap_alloc_info.length = alloc_info_p->length;
    res_tag_bitmap_alloc_info.repeats = alloc_info_p->repeats;

    rc = resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element);

    if (check_only)
    {
        SHR_IF_ERR_EXIT_NO_MSG(rc);
    }
    else
    {
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 */
