/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file local_inlif_allocation.c
 *
 * Local inlif allocation file.
 *
 * Local inlif in DNX arch includes two types of lifs: shared by cores (SBC) and dedicated per core (DPC).
 *  The SBC lifs are used for tunnel termination and similar apps, and the rest of the apps
 *  are DPC.
 *
 * For allocation, we keep one resource for the SBC lifs, and a duplicate per core for the DPC lifs.
 * In all operations, we decide which resource to access by the dbal_table_id and core_id that the user passes.
 * The dbal_table_id is mapped to the relevant physical table, which we currently assume to mean:
 *       DBAL_PHYSICAL_TABLE_INLIF_1 - SBC lif table.
 *       DBAL_PHYSICAL_TABLE_INLIF_2 - DPC lif table.
 * We then use it to access the relevant resource.
 *
 * Another thing that is considered during the allocation is how many IDs to allocate - lifs can use either 120b
 *  or 180b wide entries. We use the dbal result type to determine how many need to be allocated. See the full
 *  algorithm description in RESOURCE MANAGER ADVANCED ALGORTIHM - START.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF
/*************
 * INCLUDES  *
 */
/*
 * {
 */

#include <shared/util.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_mngr_access.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_advanced_algorithms.h>

/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */
/**
 * \brief Resource name for the resource manager.
 *      This resource name assumes that the lowest lif table is a signleton used by both cores,
 *      and that the other inlif tables are duplicated per core.
 */
#define DNX_ALGO_LOCAL_INLIF_SHARED_BY_CORES_RESOURCE    "Inlif table shared by cores"

/**
 * \brief Resource name for the resource manager.
 *      See \ref DNX_ALGO_LOCAL_INLIF_SHARED_BY_CORES_RESOURCE for the assumptions behind this resource name.
 */
#define DNX_ALGO_LOCAL_INLIF_DEDICATED_PER_CORE           "Inlif table dedicated per core"

/**
 * This define will be used for the mapping between the entry_size and the tag value.
 */
#define DNX_ALGO_INLIF_MINIMUM_ENTRY_SIZE           2

/**
 * Maximum inlif entry size.
 */
#define DNX_ALGO_INLIF_MAXIMUM_ENTRY_SIZE           3

/**
 * The number of different entry sizes. In practice, it's the difference between the maximum and minimum entry size.
 */
#define DNX_ALGO_INLIF_NOF_ENTRY_SIZES              (DNX_ALGO_INLIF_MAXIMUM_ENTRY_SIZE - DNX_ALGO_INLIF_MINIMUM_ENTRY_SIZE + 1)

/**
 * Maximum tag value for the advanced resource. For each entry size, it can be either legal or illegal.
 */
#define DNX_ALGO_INLIF_MAX_TAG_SIZE                 (DNX_ALGO_INLIF_NOF_ENTRY_SIZES * 2 - 1)

/**
 * Flags for the resource manager advanced algorithm.
 * {
 */

/*
 * This flag indicates that the allocation is only a placeholder allocation
 */
#define DNX_ALGO_LOCAL_INLIF_RES_ALLOCATE_SET_ILLEGAL_TAG      SAL_BIT(25)

#define DNX_ALGO_LOCAL_INLIF_RES_ALLOCATE_SET_ILLEGAL_TAG_ONLY SAL_BIT(26)

/**
 * }
 */

/*
 * }
 */

/*
 * }
 */
/*************
 * MACROS    *
 */
/*
 * {
 */
#define DNX_ALGO_LOCAL_INLIF_ENTRY_SIZE_TO_TAG(_entry_size, _illegal_tag) \
    ((_entry_size) - DNX_ALGO_INLIF_MINIMUM_ENTRY_SIZE + ((_illegal_tag) ? DNX_ALGO_INLIF_NOF_ENTRY_SIZES : 0))

#define DNX_ALGO_LOCAL_INLIF_TAG_IS_ILLEGAL_TAG(_tag) \
    (_tag >= DNX_ALGO_INLIF_NOF_ENTRY_SIZES)

#define DNX_ALGO_LOCAL_INLIF_TAG_TO_ENTRY_SIZE(_tag) \
    ((_tag) + DNX_ALGO_INLIF_MINIMUM_ENTRY_SIZE - ((DNX_ALGO_LOCAL_INLIF_TAG_IS_ILLEGAL_TAG(_tag)) ? DNX_ALGO_INLIF_NOF_ENTRY_SIZES : 0))
/*
 * }
 */
/*************
 * TYPE DEFS *
 */
/*
 * {
 */
extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

/*
 * }
 */
/*************
 * GLOBALS   *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */

/**
 * RESOURCE MANAGER ADVANCED ALGORTIHM - START
 *  The advanced algorithm consists of a tagged bitmap that has a grain size equal to the least common denominator.
 *  Each grain can be used to allocate only entries of a certain size. The tag's value is the entry size in the tagged
 *  grain. We pass this tag as a uint8 in the extra arguments field of the allocation function. When deleting, we
 *  first check the tag, make sure that the index is a multiple of the entry size, and delete.
 *  {
 */

/**
 * \brief
 *
 * Create the advanced algorithm instance. See the algorithm's description above.
 *
 * \ref
 *  resource_tag_bitmap_create
 */
shr_error_e
dnx_lif_local_inlif_resource_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags)
{
    dnx_algo_res_create_data_t res_tag_bitmap_create_info;
    resource_tag_bitmap_extra_arguments_create_info_t extra_create_info;
    resource_tag_bitmap_alloc_info_t alloc_info;
    int grain_size, extra_entries, element;
    int illegal_tag;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create a res_tag_bitmap to manage the entries.
     * The bitmap size must be a multiple of the grain_size, so we create extra virtual entries to complete the last grain,
     * and then allocate them so they won't be used.
     * In addition, the bitmap needs to start from 0 so the grains will be aligned with 0. So we also add these entries
     * to the resource and allocate them so they won't be used.
     */
    grain_size = dnx_data_lif.in_lif.inlif_resource_tag_size_get(unit);

    extra_entries = grain_size - ((create_data->first_element + create_data->nof_elements) % grain_size);

    sal_memset(&extra_create_info, 0, sizeof(extra_create_info));
    extra_create_info.grain_size = grain_size;
    extra_create_info.max_tag_value = DNX_ALGO_INLIF_MAX_TAG_SIZE;

    sal_memset(&res_tag_bitmap_create_info, 0, sizeof(res_tag_bitmap_create_info));
    res_tag_bitmap_create_info.first_element = 0;
    res_tag_bitmap_create_info.nof_elements = create_data->first_element + create_data->nof_elements + extra_entries;
    res_tag_bitmap_create_info.flags = RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_create
                    (unit, module_id, res_tag_bitmap, &res_tag_bitmap_create_info, &extra_create_info, nof_elements,
                     alloc_flags));

    /*
     * Allocate the extra entries so they won't be used.
     */
    sal_memset(&alloc_info, 0, sizeof(alloc_info));
    illegal_tag = _SHR_IS_FLAG_SET(create_data->flags, DNX_ALGO_LOCAL_INLIF_RES_ALLOCATE_SET_ILLEGAL_TAG);
    alloc_info.count = extra_entries;
    alloc_info.flags = RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
    alloc_info.tag = DNX_ALGO_LOCAL_INLIF_ENTRY_SIZE_TO_TAG(DNX_ALGO_INLIF_MINIMUM_ENTRY_SIZE, illegal_tag);
    element = create_data->first_element + create_data->nof_elements;
    SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc(unit, module_id, *res_tag_bitmap, alloc_info, &element));

    if (create_data->first_element > 0)
    {
        /*
         * Allocate the first entries so they won't be used.
         */
        alloc_info.count = create_data->first_element;
        element = 0;
        SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc(unit, module_id, *res_tag_bitmap, alloc_info, &element));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * See above a description of the allocation algorithm.
 *
 * \see
 * dnx_algo_res_allocate_cb
 */
shr_error_e
dnx_lif_local_inlif_resource_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    uint32 tag, illegal_tag;
    int entry_size;
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
    resource_tag_bitmap_tag_info_t tag_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tag_info, 0, sizeof(tag_info));

    /*
     * The extra arguments represent both the tag and the entry size (see above).
     */
    entry_size = *(int *) extra_arguments;
    illegal_tag = _SHR_IS_FLAG_SET(flags, DNX_ALGO_LOCAL_INLIF_RES_ALLOCATE_SET_ILLEGAL_TAG)
        || _SHR_IS_FLAG_SET(flags, DNX_ALGO_LOCAL_INLIF_RES_ALLOCATE_SET_ILLEGAL_TAG_ONLY);

    if (illegal_tag && !_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Shouldn't get here. Can't set illegal tag without indicating WITH_ID.");
    }

    tag = DNX_ALGO_LOCAL_INLIF_ENTRY_SIZE_TO_TAG(entry_size, illegal_tag);

    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_LOCAL_INLIF_RES_ALLOCATE_SET_ILLEGAL_TAG_ONLY))
    {
        /*
         * Don't allocate, just set this grain with the illegal tag. Only one element is enough to set the entire grain.
         */
        tag_info.tag = tag;
        tag_info.force_tag = TRUE;
        tag_info.element = *element;
        tag_info.nof_elements = 1;
        SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_set(unit, module_id, res_tag_bitmap, &tag_info));
    }
    else
    {
        sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
        res_tag_bitmap_alloc_info.flags = RESOURCE_TAG_BITMAP_ALLOC_ALIGN;
        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_RES_ALLOCATE_WITH_ID))
        {
            res_tag_bitmap_alloc_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_WITH_ID;
        }
        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE_FOR_SMALLER))
        {

            /**In case of reserve to smaller entry - allocate an index in an empty 6th-pack.
             * We first copy the large entry to the new (smaller) entry, fix the pointers and only at the end move to the new lif. We use an empty grain to avoid overrun of existing entries*/
            res_tag_bitmap_alloc_info.flags |= RESOURCE_TAG_BITMAP_ALLOC_IN_EMPTY_GRAIN;
            res_tag_bitmap_alloc_info.align = dnx_data_lif.in_lif.inlif_resource_tag_size_get(unit);
        }
        else
        {
            /**Use any empty element*/
            res_tag_bitmap_alloc_info.align = entry_size;
        }
        res_tag_bitmap_alloc_info.tag = tag;
        res_tag_bitmap_alloc_info.count = entry_size;

        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE))
        {
            SHR_IF_ERR_EXIT(dnx_lif_local_inlif_resource_free(unit, module_id, res_tag_bitmap, *element));
        }
        SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc(unit, module_id, res_tag_bitmap, res_tag_bitmap_alloc_info, element));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * To free an entry, we verify that the entry size ('entry_size') is not 0 (which indicates that it's not actually an allcoated entry)
 * and that the given element ('element') is indeed the start of the entry (indicated by it being a multiple of the entry size).
 *
 * The first entry will always be a multiple of the entry size because during the allocation we align it to be like that.
 *
 * We don't check other input arguments because we assume the only function to call this one is \ref dnx_algo_res_free
 *    and that other validations will be done there.
 *
 * \see
 * dnx_algo_res_free_cb
 */
shr_error_e
dnx_lif_local_inlif_resource_free_verify(
    int unit,
    uint8 entry_size,
    int element)
{
    SHR_FUNC_INIT_VARS(unit);

    if (entry_size == 0)
    {
        /*
         * If the entry size that came from the tag is 0, it means no entry has been allocated in this grain.
         */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Local inlif 0x%x is not allocated in this lif table.", element);
    }

    if ((element % entry_size) != 0)
    {
        /*
         * The entry size must be a denominator of the element.
         */
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Local inlif 0x%x is not in the correct offset for this n-pack. "
                     "The pack's entry size is: %d.", element, entry_size);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * To free an entry we check its tag to get the entry's size, make sure that the element is
 * indeed the start of the entry (indicated by it being a multiple of the entry size) and then free
 * a number of entries identical to the entry size.
 *
 * \see
 * dnx_algo_res_free_cb
 * resource_tag_bitmap_tag_get
 */
shr_error_e
dnx_lif_local_inlif_resource_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element)
{
    int entry_size;
    resource_tag_bitmap_alloc_info_t res_tag_bitmap_free_info;
    resource_tag_bitmap_tag_info_t tag_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the tag. It will be used to verify input and to indicate the number of elements to be freed.
     */
    sal_memset(&tag_info, 0, sizeof(tag_info));
    tag_info.element = element;
    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_get(unit, module_id, res_tag_bitmap, &tag_info));

    entry_size = DNX_ALGO_LOCAL_INLIF_TAG_TO_ENTRY_SIZE(tag_info.tag);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_local_inlif_resource_free_verify(unit, entry_size, element));

    sal_memset(&res_tag_bitmap_free_info, 0, sizeof(resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_free_info.count = entry_size;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_advanced_free
                    (unit, module_id, res_tag_bitmap, res_tag_bitmap_free_info, element));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The free several function is overridden in this case to release grains that were force set to illegal tag.
 * It shouldn't actually be used to free any resources.
 *
 * \see
 * dnx_algo_res_free_cb
 * resource_tag_bitmap_tag_set
 */
shr_error_e
dnx_lif_local_inlif_resource_free_several(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 nof_elements,
    int element)
{
    resource_tag_bitmap_tag_info_t tag_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&tag_info, 0, sizeof(tag_info));
    /*
     * Get the tag. It will be used to verify input and to indicate the number of elements to be freed.
     */
    tag_info.element = element;
    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_get(unit, module_id, res_tag_bitmap, &tag_info));

    if (!DNX_ALGO_LOCAL_INLIF_TAG_IS_ILLEGAL_TAG(tag_info.tag))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This function should only be used to release illegal tags.");
    }

    tag_info.force_tag = FALSE;
    tag_info.element = element;
    tag_info.nof_elements = nof_elements;

    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_set(unit, module_id, res_tag_bitmap, &tag_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The advanced fucntionality in this function compared to the basic algorithm is that it only returns TRUE
 * for the first index that a lif takes, instead of all the indexes like a simple bitmap would return.
 *
 * We do this because we assume that because inlif allocation is internal to the module and not exposed
 * to the rest of the SDK, it will not be necessary to ever check if a local inlif is allocated for anything other
 * than iterations. If that changes, then this needs to be reviewed.
 *
 * \see
 * dnx_algo_res_is_allocated_cb
 */
shr_error_e
dnx_lif_local_inlif_resource_is_allocated(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint8 *is_allocated)
{
    uint8 entry_size;
    resource_tag_bitmap_tag_info_t tag_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the tag. Only if the index is a multiple of the entry size then it's considered allocated.
     */
    sal_memset(&tag_info, 0, sizeof(tag_info));
    tag_info.element = element;
    SHR_IF_ERR_EXIT(resource_tag_bitmap_tag_get(unit, module_id, res_tag_bitmap, &tag_info));

    entry_size = DNX_ALGO_LOCAL_INLIF_TAG_TO_ENTRY_SIZE(tag_info.tag);

    if (entry_size != 0 && (element % entry_size == 0))
    {
        /*
         * The element is a multiple of the entry size. In that case, the simple bitmap
         * can handle it.
         */
        SHR_IF_ERR_EXIT(resource_tag_bitmap_is_allocated(unit, module_id, res_tag_bitmap, element, is_allocated));
    }
    else
    {
        *is_allocated = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * }
 * RESOURCE MANAGER ADVANCED ALGORTIHM - END
 */

/*
 * INLIF ALLOCATION FUNCTIONS - START
 * {
 */

/**Get number of elements needed for inlif entry*/
static shr_error_e
dnx_algo_local_inlif_get_element_size(
    int unit,
    dbal_tables_e table_id,
    uint32 dbal_result_type,
    int *entry_size_indexes)
{
    int entry_size_bits;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the entry size associated with this result type, and convert it from bit size to index size.
     * We get the bit size of an individual entry from the dnx data.
     */
    SHR_IF_ERR_EXIT(dbal_tables_payload_size_get(unit, table_id, dbal_result_type, &entry_size_bits));

    *entry_size_indexes = UTILEX_DIV_ROUND_UP(entry_size_bits, dnx_data_lif.in_lif.inlif_index_bit_size_get(unit));

    if (*entry_size_indexes < DNX_ALGO_INLIF_MINIMUM_ENTRY_SIZE)
    {
        /*
         * Inlif entry size may be small, but it always takes at least the minimum number of indexes.
         */
        *entry_size_indexes = DNX_ALGO_INLIF_MINIMUM_ENTRY_SIZE;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_inlif_allocate(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info)
{
    int allocated_element;
    int entry_size_indexes;
    dbal_physical_tables_e physical_table_id;
    int current_core;
    int flags;
    SHR_FUNC_INIT_VARS(unit);

    flags =
        (_SHR_IS_FLAG_SET(inlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE)) ?
        DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE : 0;

    if (_SHR_IS_FLAG_SET(inlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE))
    {
        flags |= DNX_ALGO_RES_ALLOCATE_WITH_ID;
        flags |= DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE;
        allocated_element = inlif_info->local_inlif;
    }

    if (_SHR_IS_FLAG_SET(inlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE_FOR_SMALLER))
    {
        flags |= DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE_FOR_SMALLER;
    }

    SHR_IF_ERR_EXIT(dnx_algo_local_inlif_get_element_size
                    (unit, inlif_info->dbal_table_id, inlif_info->dbal_result_type, &entry_size_indexes));

    /*
     * Get the physical table associated with this lif format.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, inlif_info->dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                                                   &physical_table_id));

    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        /*
         * Use the vlan translation resource, and keep it in index 0.
         */
        SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_shared_by_cores.allocate_single(unit,
                                                                                            flags,
                                                                                            (void *)
                                                                                            &entry_size_indexes,
                                                                                            &allocated_element));

        inlif_info->local_inlif = allocated_element;
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        if (!DNXCMN_CHIP_IS_MULTI_CORE(unit))
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.allocate_single(unit,
                                                                                              flags,
                                                                                              (void *)
                                                                                              &entry_size_indexes,
                                                                                              &allocated_element));
        }
        else if (inlif_info->core_id == _SHR_CORE_ALL)
        {
            /*
             * If the inlif is allocated on all cores, then allocate it on the all cores resource first,
             * and mark it as an all cores lif in the indication bitmap.
             * Then allocate WITH_ID for every single core.
             */
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.allocate_single(unit,
                                                                                              flags,
                                                                                              (void *)
                                                                                              &entry_size_indexes,
                                                                                              &allocated_element));

            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                            inlif_table_dpc_all_cores_indication.bit_set(unit, allocated_element));

            flags = DNX_ALGO_RES_ALLOCATE_WITH_ID | DNX_ALGO_LOCAL_INLIF_RES_ALLOCATE_SET_ILLEGAL_TAG | flags;

            DNXCMN_CORES_ITER(unit, inlif_info->core_id, current_core)
            {
                SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                                inlif_table_dedicated_per_core.allocate_single(unit, current_core, flags,
                                                                               (void *) &entry_size_indexes,
                                                                               &allocated_element));
            }

        }
        else
        {
            /*
             * If lif is not shared by all cores, then first allocate it for its core, then mark its grain as illegal for the
             * all_cores resource. We don't actually allocate it on the all cores resource, because it's not actually allocated
             * on all cores.
             */
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                            inlif_table_dedicated_per_core.allocate_single(unit, inlif_info->core_id, flags,
                                                                           (void *) &entry_size_indexes,
                                                                           &allocated_element));

            flags = DNX_ALGO_RES_ALLOCATE_WITH_ID | DNX_ALGO_LOCAL_INLIF_RES_ALLOCATE_SET_ILLEGAL_TAG_ONLY | flags;

            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.allocate_single(unit,
                                                                                              flags,
                                                                                              (void *)
                                                                                              &entry_size_indexes,
                                                                                              &allocated_element));
        }

        inlif_info->local_inlif = allocated_element;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong dbal logical table: %s",
                     dbal_logical_table_to_string(unit, inlif_info->dbal_table_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function decides whether an inlif can be reused.
 * It can be reused when:
 * - It requires the same amount of indexes inside a 6-th pack
 * - It's of a different size, but is alone in its 6-th pack
 * - It is the original lif
 */
shr_error_e
dnx_algo_local_inlif_can_be_reused(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info,
    int *can_be_reused,
    int *smaller_inlif)
{
    int old_entry_size_indexes, new_entry_size_indexes;
    dbal_physical_tables_e physical_table_id;
    int first_element, grain_size;
    int nof_allocated_lifs_in_grain = 0;

    SHR_FUNC_INIT_VARS(unit);

    *can_be_reused = TRUE;
    /**Compare new, old lif size - if it matches - CAN be reused*/
    SHR_IF_ERR_EXIT(dnx_algo_local_inlif_get_element_size
                    (unit, inlif_info->dbal_table_id, inlif_info->dbal_result_type, &new_entry_size_indexes));
    SHR_IF_ERR_EXIT(dnx_algo_local_inlif_get_element_size
                    (unit, inlif_info->dbal_table_id, inlif_info->old_result_type, &old_entry_size_indexes));

    if (old_entry_size_indexes == new_entry_size_indexes)
    {
        SHR_EXIT();
    }

    /**Inlif is not alone in 6th-pack*/
    if (inlif_info->local_inlif % 6)
    {
        *can_be_reused = FALSE;
        SHR_EXIT();
    }

    /*
     * Get the physical table associated with this lif format.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, inlif_info->dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                                                   &physical_table_id));

    /**Get-the n pack index*/
    grain_size = dnx_data_lif.in_lif.inlif_resource_tag_size_get(unit);
    first_element = UTILEX_ALIGN_DOWN(inlif_info->local_inlif, grain_size);

    /**inlif table 1 --> SBC*/
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_shared_by_cores.nof_allocated_elements_in_range_get(unit,
                                                                                                                first_element,
                                                                                                                grain_size,
                                                                                                                &nof_allocated_lifs_in_grain));
    }
    /**inlif table 2 --> DPC --> differentiate between multi core, shr_core_all, dedicated core*/
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        if ((!DNXCMN_CHIP_IS_MULTI_CORE(unit)) || (inlif_info->core_id == _SHR_CORE_ALL))
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.nof_allocated_elements_in_range_get(unit,
                                                                                                                  first_element,
                                                                                                                  grain_size,
                                                                                                                  &nof_allocated_lifs_in_grain));
        }
        else
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                            inlif_table_dedicated_per_core.nof_allocated_elements_in_range_get(unit,
                                                                                               inlif_info->core_id,
                                                                                               first_element,
                                                                                               grain_size,
                                                                                               &nof_allocated_lifs_in_grain));
        }
    }
    /**If the number of current indexes in the n-pack matches the old entry-size --> this is the original lif*/
    if (nof_allocated_lifs_in_grain != old_entry_size_indexes)
    {
        *can_be_reused = FALSE;
    }

exit:
    if (smaller_inlif && (*can_be_reused == FALSE))
    {
        *smaller_inlif = (old_entry_size_indexes > new_entry_size_indexes) ? TRUE : FALSE;
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_inlif_free(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info)
{
    dbal_physical_tables_e physical_table_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the physical table associated with this lif format.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, inlif_info->dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                                                   &physical_table_id));

    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_shared_by_cores.free_single(unit, inlif_info->local_inlif));
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        int current_core;
        if (!DNXCMN_CHIP_IS_MULTI_CORE(unit))
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.free_single(unit,
                                                                                          inlif_info->local_inlif));
        }
        else if (inlif_info->core_id == _SHR_CORE_ALL)
        {
            /*
             * If the inlif is freed on all cores, then free it from the all_cores resources, and from all cores of
             * the regular resource.
             */
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.free_single(unit,
                                                                                          inlif_info->local_inlif));

            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                            inlif_table_dpc_all_cores_indication.bit_clear(unit, inlif_info->local_inlif));

            DNXCMN_CORES_ITER(unit, inlif_info->core_id, current_core)
            {
                SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dedicated_per_core.free_single(unit, current_core,
                                                                                                   inlif_info->local_inlif));
            }

        }
        else
        {
            /*
             * If lif is not shared by all cores, then first free it from its core.
             * Then check for all cores whether they use this grain. If none of them do, then free it in the all_cores resource
             * as well.
             */
            int free_all_cores, first_element, grain_size;
            int nof_allocated_lifs_in_grain;
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                            inlif_table_dedicated_per_core.free_single(unit, inlif_info->core_id,
                                                                       inlif_info->local_inlif));

            grain_size = dnx_data_lif.in_lif.inlif_resource_tag_size_get(unit);
            first_element = UTILEX_ALIGN_DOWN(inlif_info->local_inlif, grain_size);
            free_all_cores = TRUE;
            DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, current_core)
            {
                SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                                inlif_table_dedicated_per_core.nof_allocated_elements_in_range_get(unit, current_core,
                                                                                                   first_element,
                                                                                                   grain_size,
                                                                                                   &nof_allocated_lifs_in_grain));

                if (nof_allocated_lifs_in_grain > 0)
                {
                    /*
                     * If we found an allocated element, it means we can't free the all_cores resource.
                     * Break the loops.
                     */
                    free_all_cores = FALSE;
                    break;
                }
            }

            if (free_all_cores)
            {
                /*
                 * Remove the illegal tag from the grain, which would make it usable.
                 * Pass 2 as the number of entries to remove, otherwise it would use the free callback instead
                 * of the free_several callback.
                 */
                SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.free_several(unit,
                                                                                               2,
                                                                                               inlif_info->local_inlif));
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong dbal logical table: %s",
                     dbal_logical_table_to_string(unit, inlif_info->dbal_table_id));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_inlif_allocation_init(
    int unit)
{
    dnx_algo_res_create_data_t res_data;
    int entry_size_indexes;
    int allocated_value, nof_lifs;
    uint32 core_id;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create the resources for the lif tables.
     */
    sal_memset(&res_data, 0, sizeof(res_data));

    /*
     * Allocate local INLIF-1 resources
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, &nof_lifs, NULL));

    res_data.first_element = 0;
    res_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    res_data.nof_elements = nof_lifs;
    res_data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_LIF_LOCAL_INLIF;
    sal_strncpy(res_data.name, DNX_ALGO_LOCAL_INLIF_SHARED_BY_CORES_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_shared_by_cores.create(unit, &res_data, NULL));

    /*
     * We need to reserve an In-LIF from INLIF-1 for AC trap.
     *   - table is DBAL_TABLE_IN_AC_INFO_DB.
     *   - result type is DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE.
     *
     * HW configuration is done by dnx_vlan_port_ingress_drop_lif_create()
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_inlif_get_element_size
                    (unit, DBAL_TABLE_IN_AC_INFO_DB, DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE,
                     &entry_size_indexes));
    allocated_value = dnx_data_lif.in_lif.drop_in_lif_get(unit);

    SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_shared_by_cores.allocate_single(unit,
                                                                                        DNX_ALGO_RES_ALLOCATE_WITH_ID,
                                                                                        (void *) &entry_size_indexes,
                                                                                        &allocated_value));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_set
                    (unit, allocated_value, _SHR_CORE_ALL, DBAL_TABLE_IN_AC_INFO_DB,
                     DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_LARGE, 0));

    /*
     * Allocate local INLIF-2/3 resources
     * Start the resource from DNX_ALGO_INLIF_MINIMUM_ENTRY_SIZE because entry 0 can't be used.
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, NULL, &nof_lifs));
    res_data.first_element = DNX_ALGO_INLIF_MINIMUM_ENTRY_SIZE;
    res_data.nof_elements = nof_lifs - res_data.first_element;
    /*
     * The illegal tag flag indicates that the first and last grain of the inlif won't be used
     * for assymetric allocation, but only for symmetric allocation.
     */
    res_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM | DNX_ALGO_LOCAL_INLIF_RES_ALLOCATE_SET_ILLEGAL_TAG;

    if (DNXCMN_CHIP_IS_MULTI_CORE(unit))
    {
        /*
         * We only need the dedicated per core resource and indication if the chip is actually per core. Otherwise,
         * we can save space by not allocating it, and only using the _all_cores resource.
         */
        sal_strncpy(res_data.name, DNX_ALGO_LOCAL_INLIF_DEDICATED_PER_CORE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dedicated_per_core.alloc
                        (unit, dnx_data_device.general.nof_cores_get(unit)));
        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dedicated_per_core.create
                            (unit, core_id, &res_data, NULL));
        }

        SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores_indication.alloc_bitmap(unit, nof_lifs));
    }

    /*
     * DPC lif table - all cores.
     */
    res_data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    sal_strncpy(res_data.name, "Inlif table dedicated per core for all cores.", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.create(unit, &res_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_local_inlif_allocation_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 * INLIF ALLOCATION FUNCTIONS - END
 */

/*
 * }
 */
