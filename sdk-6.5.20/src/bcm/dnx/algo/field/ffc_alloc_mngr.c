/** \file ffc_alloc_mngr.c
 *
 * Implementation for advanced ffc resource manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/dnx/algo/field/ffc_alloc_mngr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_access.h>

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#define DNX_FFC_REGION(i) (DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE * (i))

shr_error_e
dnx_algo_ffc_ipmf_1_bitmap_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    uint8 tag = TRUE;
    dnx_algo_res_create_data_t res_tag_bitmap_create_info;
    resource_tag_bitmap_extra_arguments_create_info_t extra_create_info;
    resource_tag_bitmap_tag_info_t tag_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tag_info, 0, sizeof(tag_info));

    /*
     * Create the ffc bitmap.
     */
    sal_memset(&extra_create_info, 0, sizeof(extra_create_info));
    extra_create_info.grain_size = DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE;
    extra_create_info.max_tag_value = TRUE;

    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(res_tag_bitmap_create_info));
    res_tag_bitmap_create_info.first_element = 0;
    res_tag_bitmap_create_info.nof_elements = DNX_FIELD_ALGO_FFC_BMP_REGION_COUNT * DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE;
    res_tag_bitmap_create_info.flags = RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create(unit, module_id, res_tag_bitmap, &res_tag_bitmap_create_info,
                                               &extra_create_info, nof_elements, alloc_flags));

    /*
     * Tag whole region as available.
     */
    tag_info.tag = tag;
    tag_info.force_tag = TRUE;
    tag_info.element = 0;
    tag_info.nof_elements = DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE * DNX_FIELD_ALGO_FFC_BMP_REGION_COUNT;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_set(unit, module_id, *res_tag_bitmap, &tag_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_ffc_ipmf_1_bitmap_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *ranges_v,
    int *element)
{
    uint8 *ranges;
    uint8 cur_range;
    uint32 enable_tag = TRUE;
    uint32 disable_tag = FALSE;
    uint32 flag_check_only_was_set = flags & RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY;
    int ii;
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
    int rv;
    uint8 dnx_algo_ffc_ipmf_1_order[DNX_FIELD_ALGO_FFC_BMP_REGION_COUNT] = { 1, 3, 2, 0 };
    resource_tag_bitmap_tag_info_t tag_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tag_info, 0, sizeof(tag_info));

    ranges = (uint8 *) ranges_v;

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(res_tag_bitmap_alloc_info));
    res_tag_bitmap_alloc_info.tag = enable_tag;
    res_tag_bitmap_alloc_info.count = 1;
    res_tag_bitmap_alloc_info.flags = flags | RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG;

    /*
     * Disable all tags.
     */

    for (ii = 0; ii < DNX_FIELD_ALGO_FFC_BMP_REGION_COUNT; ii++)
    {
        tag_info.tag = disable_tag;
        tag_info.force_tag = TRUE;
        tag_info.element = DNX_FFC_REGION(ii);
        tag_info.nof_elements = DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE;

        SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_set(unit, module_id, res_tag_bitmap, &tag_info));

    }

    /*
     * Loop on available ranges and set enable tag to enable the current range
     * according to input from field_map.c
     * Apply the ranges one by one by priority, and check if we can allocate.
     */
    res_tag_bitmap_alloc_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY;
    for (ii = 0; ii < DNX_FIELD_ALGO_FFC_BMP_REGION_COUNT; ii++)
    {
        cur_range = dnx_algo_ffc_ipmf_1_order[ii];

        if (*ranges & SAL_BIT(cur_range))
        {
            tag_info.tag = enable_tag;
            tag_info.force_tag = TRUE;
            tag_info.element = DNX_FFC_REGION(cur_range);
            tag_info.nof_elements = DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE;

            SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_set(unit, module_id, res_tag_bitmap, &tag_info));
            rv = resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_RESOURCE);
            if (SHR_SUCCESS(rv))
            {
                break;
            }
        }
    }

    /*
     * Enable the check only flag only if it was requested by the function.
     */
    res_tag_bitmap_alloc_info.flags &= ~RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY;
    res_tag_bitmap_alloc_info.flags |= flag_check_only_was_set;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_ffc_ipmf_1_alloc_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t create_data;
    uint32 sub_resource_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_data, 0, sizeof(create_data));

    /*
     * Set bitmap info
     */
    create_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    create_data.first_element = 0;
    create_data.nof_elements = DNX_FIELD_ALGO_FFC_BMP_REGION_SIZE * DNX_FIELD_ALGO_FFC_BMP_REGION_COUNT;
    create_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_FFC;
    sal_strncpy(create_data.name, DNX_ALGO_FIELD_IPMF_1_FFC, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /*
     * Create ffc bitmap
     */

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_1_ffc.alloc(unit,
                                                        dnx_data_field.stage.stage_info_get(unit,
                                                                                            DNX_FIELD_STAGE_IPMF1)->nof_contexts));
    for (sub_resource_id = 0;
         sub_resource_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
         sub_resource_id++)
    {
        SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_1_ffc.create(unit, sub_resource_id, &create_data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}
