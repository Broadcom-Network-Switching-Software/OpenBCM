/** \file resource_tag_bitmap_utils.h
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * \brief
 * Overview
 *
 * The resource tag bitmap is used to allocate resources in the system according to preset
 * criteria.
 * This files contains all the mutual functions, structs and defines that are relevant for resource_tag_bitmap_t and resource_tag_bitmap_fast_t
 *
 */
#ifndef RESOURCE_TAG_BITMAP_UTILS_H_INCLUDED
/*
 * {
 */
#define RESOURCE_TAG_BITMAP_UTILS_H_INCLUDED

/** { */
/*************
* INCLUDES  *
*************/
#include <soc/dnxc/swstate/sw_state_features.h>

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/alloc.h>
#include <shared/bitop.h>
#include <soc/error.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>

/*************
 * DEFINES   *
 *************/
/** { */

/*
 * If the ALLOC CHECK ONLY / ALLOC SILENTLY flag is passed, use this macros to return error without printing it.
 */
#define RESOURCE_TAG_BITMAP_ERR_EXIT(flags, error, ...)                              \
    do                                                                                  \
    {                                                                                   \
        if(_SHR_IS_FLAG_SET(flags, RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY) ||             \
                _SHR_IS_FLAG_SET(flags, RESOURCE_TAG_BITMAP_ALLOC_SILENTLY))            \
        {                                                                               \
            SHR_SET_CURRENT_ERR(error);                                                 \
            SHR_EXIT();                                                                 \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            SHR_ERR_EXIT(error, __VA_ARGS__)                                            \
        }                                                                               \
    }                                                                                   \
    while(0)

#define RESOURCE_TAG_BITMAP_IF_ERR_EXIT(flags, rv)                                   \
    do                                                                                  \
    {                                                                                   \
        if(_SHR_IS_FLAG_SET(flags, RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY) ||             \
        _SHR_IS_FLAG_SET(flags, RESOURCE_TAG_BITMAP_ALLOC_SILENTLY))                    \
        {                                                                               \
            SHR_SET_CURRENT_ERR(rv);                                                    \
            if (SHR_GET_CURRENT_ERR() != _SHR_E_NONE)                                   \
            {                                                                           \
                SHR_EXIT();                                                             \
            }                                                                           \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            SHR_IF_ERR_EXIT(rv);                                                        \
        }                                                                               \
    }                                                                                   \
    while(0)

/**
 * \brief
 * Flags for \ref resource_tag_bitmap_alloc \ref resource_tag_bitmap_free
 * {
 */
/**
 * For alloc only, allocate the element with ID.
 */
#define RESOURCE_TAG_BITMAP_ALLOC_WITH_ID              DNX_ALGO_RES_ALLOCATE_WITH_ID
/**
 * \brief
 * If this flag is set, then an allocation check will run. All the steps to allocate an
 * element will be taken, and the allocated element / relevant error code will be returned,
 * but no sw state will be changed, and no error will be printed.
 * If the check allocation was successful, then the returned element should be allocated WITH_ID.
 */
#define RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY           DNX_ALGO_RES_ALLOCATE_SIMULATION
/**
 * \brief
 * If this flag is set, then silent allocation will run. All the steps to allocate an
 * element will be taken, and the allocated element / relevant error code will be returned,
 * no error will be printed. If the allocation was failed, the resource will stay as is.
 * In case that allocation needs to be done but the failure of allocation should not be printed, use SILENT allocation.
 * In case that allocation should be not done first and according to the return value and the free element your algorithm should run, use CHECK_ONLY
 */
#define RESOURCE_TAG_BITMAP_ALLOC_SILENTLY             DNX_ALGO_RES_ALLOCATE_SILENTLY
/**
 * \brief
 * If this flag is set, then the allocator will only allocate in a grain that wasn't already set with a tag.
 */
#define RESOURCE_TAG_BITMAP_ALLOC_IN_EMPTY_GRAIN       SAL_BIT(8)

/*
 * Next RESOURCE_TAG_BITMAP_ALLOC_ flags are set by the extra argument parameter (bits 24:31).
 */

/**
 * \brief
 *  If this flag is set, then the allocation will include data for the entries that are allocated.
 * This option is available only if  RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY were set in creation
 * */
#define  RESOURCE_TAG_BITMAP_ALLOC_DATA_PER_ENTRY SAL_BIT(24)
/**
 * \brief
 * If the ALLOC_ALIGN flag is set the first element of the
 * allocated block will be at ((n * align) + offset + bitmap.first_element),
 * where n is some integer and bitmap.first_element is the first element declared
 * at the create of the resource bitmap.
 * If it is not possible to allocate a block with the requested
 * constraints, the call will fail.
 * Note that the alignment is within the specified range of the resource,
 * and not specifically aligned against the absolute value zero;
 * to request the alignment be against zero specify the ALIGN_ZERO flag.
 *
 * If offset >= align, BCM_E_PARAM will be returned.
 * If align is zero or negative and one of the flags (ALIGN_ZERO/ALIGN) is set, it will be treated as error.
 * If neither of the flags (ALIGN_ZERO/ALIGN) is set the align will be set to 1.
 *
 * If WITH_ID is specified, and the requested base element does not
 * comply with the indicated alignment, BCM_E_PARAM will be returned.
 */
#define RESOURCE_TAG_BITMAP_ALLOC_ALIGN_ZERO           SAL_BIT(25)
#define RESOURCE_TAG_BITMAP_ALLOC_ALIGN                SAL_BIT(26)
/**
 * \brief
 * If this flag is set, then the element will be allocated in a grain only
 * if this grain already had this tag before the allocation, whether by a previous
 * allocation or by explicitly setting it with \ref  resource_tag_bitmap_tag_set
 */
#define RESOURCE_TAG_BITMAP_ALWAYS_CHECK_TAG           SAL_BIT(27)
/**
 * \brief
 * If the IGNORE_TAG flag is set, then the element may be allocated at every free element,
 * regardless of tag, and the allocation of this block of elements will not affect the tag assigned
 * to this grain.
 * To use the flag, the \ref RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG flag must
 * be used when creating the bitmap.
 */
#define RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG           SAL_BIT(28)
/**
 * \brief
 * If the ALLOC_SPARSE flag is set allocates according to a pattern.
 * The pattern argument is a bitmap of the elements that are of interest
 * in a single iteration of the pattern (and only the least significant
 * 'length' bits are used; higher bits are ignored).  The bit with value
 * (1 << k) set indicates the element at (elem + k) must be in the block;
 * clear it indicates the element at (elem + k) is not in the block.  This
 * repeats for as many iterations as indicated by 'repeats'.
 *
 * The caller must track the pattern, length and repeats values and
 * provide these values along with the elem value when freeing the block.
 *
 * Any allocation made through this function with ALLOC_SPARSE flag must be freed using the
 * same flag in sw_state_resource_bitmap_free function, or one by one.
 */
#define RESOURCE_TAG_BITMAP_ALLOC_SPARSE               SAL_BIT(29)
/**
 * \brief
 * If this flag is set, then the range_start and range_end fields of the
 * allocation struct will be taken into consideration. An element will only be allocated if
 * it falls in this range.
 *
 * Note that the range is exclusive - range_end is the first element NOT in the range.
 */
#define RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE             SAL_BIT(30)
/**
 * \brief
 * If allocating with WITH_ID, verify that all requested elements are in the same region(grain)
 * If allocating without WITH_ID, verify that all allocated elements are in the same region(grain)
 */
#define RESOURCE_TAG_BITMAP_ALLOC_CROSS_REGION_CHECK             SAL_BIT(31)

/**
 * }
 */
/*
 * Next RESOURCE_TAG_BITMAP_CREATE_ flags are set by the extra argument parameter (bits 25:31).
 */
/**
 * \brief
 * Flags for \ref resource_tag_bitmap_create
 * {
 */

/**
 * \brief
 * When this flag is set during bitmap creation, then the flag
 * \ref RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG could be used during allocation.
 */
#define RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG                 SAL_BIT(25)

/**
 * \brief
 * When this flag is set during creation, then the
 * \ref resource_tag_bitmap_nof_used_elements_in_grain_get function can be used.
 * Note that it's faster compared to \ref resource_tag_bitmap_nof_allocated_elements_in_range_get ,
 * but has higher memory consumption.
 */
#define RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN         SAL_BIT(26)
/**
 * \brief
 * When this tag is set during creation, then a tag can set with \ref resource_tag_bitmap_tag_set
 * with the force_tag option, and simple allocation will not be able to override this tag.
 */
#define RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG                  SAL_BIT(27)

/**
 * \brief
 * When this flag is set during creation, the first and last grains are trimmed to match the alignment of the grains.
 * For example, if the first element is 2 the grain size is 6 and the bitmap's count is 13 (so last element is 14),
 * then the first grain will be 2-5, the second grain will be 6-11, and the third grain will be 12-14.
 * The rest of grains are aligned to 0.
 */
#define RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS         SAL_BIT(28)

/**
 * \brief
 * When this flag is set during creation, the algorithm will guarantee the available resources that have the lowest IDs are returned.
 * Note that when allocating with RESOURCE_TAG_BITMAP_ALLOC_WITH_ID flag, this flag will be 'overruled' and will not affect behavior.
 */
#define RESOURCE_TAG_BITMAP_CREATE_PREFER_LOW_IDS                     SAL_BIT(29)

/**
 * \brief
 * When this flag is set during creation, the algorithm will create an option for data per entry/bit.
 */
#define RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY                     SAL_BIT(30)
/**
 * \brief
 * If no flags are used during this creation, then this flag can be used.
 */
#define RESOURCE_TAG_BITMAP_CREATE_FLAGS_NONE                          0

/**
 * }
 */

/**
 * \brief
 * When multiple offsets are used during allocation, then this is the max numbers of offsets
 * that can be passed.
 */
#define RESOURCE_TAG_BITMAP_MAX_NOF_OFFSETS 4

/**
 * \brief
 * When multiple tags levels are used during creation and allocation, then this is the max numbers of
 * tags levels that can be passed.
 */
#define RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS 2

/**
 * \brief
 * Since we use sparse_alloc and sparse_free as internal implementation of all allocation and free
 *  operations, we use this as pattern and set the repeats = count.
 */
#define _RTB_PATTERN_NOT_SPARSE 0x1

/**
 * \brief
 *  If the max_tags_value is over _RTB_CRITICAL_MAX_TAG_VALUE, then an error will be returned.
 */
#define _RTB_CRITICAL_MAX_TAG_VALUE ((1 << 21) - 1)

/*
 * Pass this as force_tag if you don't wish to change the current force setting.
 * TO BE USED BY RESOURCE TAGGED BITMAP ONLY when the resource was created with RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG flag.
 */
#define _RTB_FORCE_TAG_NO_CHANGE    2

/*
 * This define will be used in the used sw state macros.
 */
#define _RTB_NO_FLAGS  0

/** } */

/*************
 * TYPE DEFS *
 *************/
/** { */

/**
 * \brief
 * Pass this struct when creating a new resource tag bitmap.
 */
typedef struct
{
    char name[DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH];
    /**
     * Minimum valid element ID.
     */
    int first_element;
    /**
     * Total number of elements.
     */
    int count;
    /**
     * Number of elements per grain. Only relevant if tags are used.
     * Count must be evenly divided by this value, unless the flag
     * \ref RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS is set.
     */
    int grains_size[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * Number of tags levels.
     */
    uint8 nof_tags_levels;
    /**
     * Maximum tag value. A max_tag_value of 0 means the bitmap doesn't use tags.
     */
    uint32 max_tags_value[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /**
     * RESOURCE_TAG_BITMAP_CREATE_* flags used for this bitmap.
     */
    uint32 flags;
    /*
     * The size of each data
     * Must be equal to sizeof(type)
     * It will be used when \ref RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY is set.
     */
    uint32 data_per_entry_size;
} resource_tag_bitmap_utils_create_info_t;

/**
 * \brief
 * Pass this struct when allocating a block of elements.
 *
 */
typedef struct
{
    /*
     * RESOURCE_TAG_BITMAP_ALLOC_* flags.
     */
    uint32 flags;
    /*
     * Number of elements to allocate in the block. If ALLOC_SPARSE flag is not set repeats is equal to count.
     */
    int count;
    /*
     * Only relevant if ALLOC_ALIGN flag is set.
     * Base alignment of block.
     */
    int align;
    /*
     * Only relevant if ALLOC_ALIGN flag is set.
     * Possible offsets from base alignment for first element in block. Several offsets
     * can be matched by setting nof_offsets.
     */
    int offs[RESOURCE_TAG_BITMAP_MAX_NOF_OFFSETS];
    /*
     * Only relevant if ALLOC_ALIGN flag is set.
     * Number of offsets to test. If set to 0, then only offs[0] will be used.
     */
    uint8 nof_offsets;
    /*
     * Tag value to use for block. Only relevant if bitmap is using tags.
     */
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * Relevant only if ALLOC_SPARSE flag is set.
     * Bitmapped pattern of elements.
     */
    uint32 pattern;
    /*
     * Relevant only if ALLOC_SPARSE flag is set.
     * Length of pattern.
     */
    int length;
    /*
     * Relevant only if ALLOC_SPARSE flag is set.
     * Number of iterations of pattern.
     */
    int repeats;
    /*
     * Only relevant if ALLOC_IN_RANGE flag is set.
     * Starting ID of range in which to allocate.
     */
    int range_start;
    /*
     * Only relevant if ALLOC_IN_RANGE flag is set.
     * First invalid ID after the required allocation range.
     * I.e., to allocate in the range {3,6}, set range_start to 3 and range_end to 7.
     */
    int range_end;
    /*
     * Boolean indication of whether any of the grains involved were unused before this allocation.
     */
    uint8 first_references[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * Pointer with the data that should be set.
     * It is optional field which is part of the extra arguments and
     * can be used only if RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY flag was given during creation of the resource.
     * The data can be set during allocation only when RESOURCE_TAG_BITMAP_ALLOC_DATA_PER_ENTRY flag is set.
     */
    const void *data;
} resource_tag_bitmap_utils_alloc_info_t;

/*
 * The struct include information that relevant for each level in the resource manager.
 */
typedef struct
{
    /**
     * elements per tag grain
     */
    int grain_size;
    /**
     * bytes per tag
     */
    int tag_size;
    /**
     * pointer to base of tag data (after data)
     */
    SHR_BITDCL *tag_data;
    /**
     * For each grain, indicate whether it's forced tag or a modifiable tag.
     */
    SHR_BITDCL *forced_tag_indication;
    /**
     * Maximum tag value.
     */
    int max_tag_value;
    /**
     * Tag tracking bitmap used for tracking resource that was allocated with RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG
     * and were allocated with RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG flag.
     */
    SHR_BITDCL *tag_tracking_bitmap;
    /**
     * Number of allocated bits in one grain.
     */
    uint32 *nof_allocated_elements_per_grain;
} resource_tag_bitmap_utils_tag_level_info_t;

 /*
  * Struct of resource tag bitmap utils.
  */
typedef struct
{
    char name[DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH];
    /**
     * lowest element ID
     */
    int first_element;
    /**
     * number of elements
     */
    int count;
    /**
     * number of elements in use
     */
    int used;
    /**
     * Data.
     */
    SHR_BITDCL *data;
    /*
     *
     * * The array will hold data per entry/bit of the resource_tag_bitmap. Relevant only if
     * RESOURCE_TAG_BITMAP_ALLOC_DATA_PER_ENTRY flag is set.
     */
    uint8 *data_per_entries;
    /*
     * The size of each data per entry/bit
     * Relevant only if using with RESOURCE_TAG_BITMAP_ALLOC_DATA_PER_ENTRY flag is set.
     */
    uint32 data_per_entry_size;
    /**
     * Number of tags levels.
     */
    uint8 nof_tags_levels;
    /**
     * Flags used to create this resource.
     */
    uint32 flags;
    /*
     *Hold relevant data per level
     * */
    resource_tag_bitmap_utils_tag_level_info_t tag_level_info[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
}  *resource_tag_bitmap_utils_t;

/**
 * Includes the information user needs to supply for tag_get/tag_set in resource.
 * \see
 *      resource_tag_bitmap_tag_set
 *      resource_tag_bitmap_tag_get
 */
typedef struct
{
    /*
     * Element index.
     */
    int element;
    /*
     * How many elements will be set with this tag.
     */
    int nof_elements;
    /*
     * Tag value to use.
     */
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * If the bitmap was created with the RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG flag, * then set this to TRUE to
     * force the tag, and to FALSE to release it.
     */
    int force_tag;
    /*
     * Boolean indication of whether it's the first reference for a tagged grain.
     */
    uint8 first_references[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
} resource_tag_bitmap_utils_tag_info_t;

/**
 * Includes the information user needs to supply for resource creation.
 * The struct can be passed as extra_arguments and then the Basic algorithm will behave as advanced.
 * \see resource_tag_bitmap_create
 */
typedef struct
{
    /*
     * Grain size.
     */
    int grains_size[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * Max tag value.
     */
    uint32 max_tags_value[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * Number of tags
     * */
    uint8 nof_tags_levels;
    /*
     * The size of each data per entry/bit
     */
    uint32 data_per_entry_size;
} resource_tag_bitmap_utils_extra_arg_create_info_t;

typedef struct
{
    /*
     * Base alignment. Only relevant if ALLOC_ALIGN flag is set.
     */
    int align;
    /*
     * Offset from base alignment. Each option will be checked.
     */
    int offs[RESOURCE_TAG_BITMAP_MAX_NOF_OFFSETS];
    /*
     * Number of offsets.
     */
    uint8 nof_offsets;
    /*
     * Tag value to use. Only relevant if bitmap is using tags.
     */
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * Bitmapped pattern of elements. Relevant only if ALLOC_SPARSE flag is set.
     */
    uint32 pattern;
    /*
     * Length of pattern. Relevant only if ALLOC_SPARSE flag is set.
     */
    int length;
    /*
     * Number of iterations of pattern. Relevant only if ALLOC_SPARSE flag is set.
     */
    int repeats;
    /*
     * The start ID of the range in which allocation without ID will occur if the ALLOC_IN_RANGE flag is added.
     */
    int range_start;
    /*
     * The next ID to the last valid ID of the range in which allocation without ID will occur if the ALLOC_IN_RANGE flag is added.
     */
    int range_end;
    /*
     * Boolean indication of whether it's the first reference for a tagged grain.
     */
    uint8 first_references[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * Pointer with the data that should be set.
     * It is optional field which is part of the extra arguments and
     * can be used only if RESOURCE_TAG_BITMAP_CREATE_DATA_PER_ENTRY flag was given during creation of the resource.
     * The data can be set during allocation only when RESOURCE_TAG_BITMAP_ALLOC_DATA_PER_ENTRY flag is set.
     */
    const void *data;
} resource_tag_bitmap_utils_extra_arg_alloc_info_t;

/** } */

/*************
 * FUNCTIONS *
 *************/
/** { */

shr_error_e resource_tag_bitmap_utils_tag_set(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    uint32 tag,
    int tag_level,
    int force_tag,
    int elem_index,
    int elem_count,
    uint8 *first_references);

shr_error_e resource_tag_bitmap_utils_tag_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    void *tag_info);

shr_error_e resource_tag_bitmap_utils_clear(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource);

shr_error_e resource_tag_bitmap_utils_is_allocated(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int element,
    uint8 *is_allocated);

shr_error_e resource_tag_bitmap_utils_nof_free_elements_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int *nof_free_elements);

shr_error_e resource_tag_bitmap_utils_nof_allocated_elements_in_range_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements);

shr_error_e resource_tag_bitmap_utils_free_several(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    uint32 nof_elements,
    int elem,
    void *extra_argument);

shr_error_e resource_tag_bitmap_utils_data_per_entry_set(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int element,
    const void *data);

shr_error_e resource_tag_bitmap_utils_data_per_entry_get(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int element,
    void *data);

int resource_tag_bitmap_utils_get_grain_index(
    resource_tag_bitmap_utils_t resource,
    int tag_level,
    int element_index);

int resource_tag_bitmap_utils_get_grain_start(
    resource_tag_bitmap_utils_t resource,
    int tag_level,
    int element_index);

int resource_tag_bitmap_utils_get_grain_size(
    resource_tag_bitmap_utils_t resource,
    int tag_level,
    int element_index);

int resource_tag_bitmap_utils_get_nof_grains(
    int count,
    int grain_size,
    int first_element,
    int tag_level,
    int flags);

int resource_tag_bitmap_utils_get_nof_free_elements(
    resource_tag_bitmap_utils_t resource,
    int tag_level,
    int grain_idx);

shr_error_e resource_tag_bitmap_utils_tag_get_internal(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int elem_index,
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS]);

uint8 resource_tag_bitmap_utils_is_multilevel_tags_resource(
    resource_tag_bitmap_utils_t resource);

int resource_tag_bitmap_utils_get_nof_small_grains_in_bigger_grains(
    resource_tag_bitmap_utils_t resource);

int resource_tag_bitmap_utils_get_higher_tag_level(
    resource_tag_bitmap_utils_t resource);

int resource_tag_bitmap_utils_get_allocated_block_length(
    resource_tag_bitmap_utils_alloc_info_t alloc_info);

shr_error_e resource_tag_bitmap_utils_create(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t * resource,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    uint32 alloc_flags,
    resource_tag_bitmap_utils_create_info_t * rtb_create_info);

shr_error_e resource_tag_bitmap_utils_print(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    dnx_algo_res_dump_data_t * data);

shr_error_e resource_tag_bitmap_utils_alloc_info_init(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_alloc_info_t * rtb_alloc_info,
    resource_tag_bitmap_utils_extra_arg_alloc_info_t ** alloc_info,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments);

shr_error_e resource_tag_bitmap_utils_alloc_pre_process(
    int unit,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info);

shr_error_e resource_tag_bitmap_utils_alloc_verify(
    int unit,
    resource_tag_bitmap_utils_t resource,
    resource_tag_bitmap_utils_alloc_info_t alloc_info,
    int *elem);

/*
 *  Check that all elements that are required for the current allocation either have the same tag
 *  as the grain they're in, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG is set, or are in a block that has no allocated elements.
 *  If all the elements meet these conditions, then result will be 0. Otherwise, it will be -1.
 */
shr_error_e resource_tag_bitmap_utils_tag_check(
    int unit,
    uint32 alloc_flags,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int elem_index,
    int elem_count,
    uint32 tag,
    int tag_level,
    int *result);

/*
 * This function checks whether requested block and the tag fits
 * If a single bit that's supposed to be allocated is not free, is_free_block will be FALSE.
 * Otherwise, it will be TRUE.
 * tag_result will be 0 if all elements that are required for the current allocation either have the same tag
 * as the grain they're in, RESOURCE_TAG_BITMAP_ALLOC_IGNORE_TAG is set, or are in a block that has no allocated elements.
 Otherwise, it will be -1.
 */
shr_error_e resource_tag_bitmap_utils_check_element_for_allocation(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info,
    int element,
    int allocated_block_length,
    int *is_free_block,
    int *tag_result);

/*
 * Update the relevant fields(data,tags etc.) in the resource tag bitmap according to the allocation
 * */
shr_error_e resource_tag_bitmap_utils_update_resource_after_allocation(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info,
    int pattern_first_index,
    int *elem,
    int *allocated_bits);

/*
 * Update the number of allocated elements in the relevant grains.
 * The operation should set to FALSE when freeing elements and TRUE while allocating elements.
 */
shr_error_e resource_tag_bitmap_utils_allocated_elements_per_bank_update(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int repeats,
    int element,
    int tag_level,
    uint8 operation);

/*
 * In case allocation element/s with ID (RESOURCE_TAG_BITMAP_ALLOC_WITH_ID)
 * this function verify that tag matches and that all elements in the block are free.
 */
shr_error_e resource_tag_bitmap_utils_check_alloc_with_id(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    resource_tag_bitmap_utils_alloc_info_t * alloc_info,
    int allocated_block_length,
    int *pattern_first_index,
    int *elem,
    int *allocation_success);

shr_error_e resource_tag_bitmap_utils_is_grain_in_use(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    uint32 alloc_flags,
    int tag_level,
    int grain_start,
    int grain_size,
    uint8 *grain_in_use);

/*
 * This function checks whether all bits in the requested block are free.
 * If a single bit that's supposed to be allocated is not free, can_allocate will be FALSE.
 * Otherwise, it will be TRUE.
 */
shr_error_e resource_tag_bitmap_utils_is_block_free(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_utils_t resource,
    int pattern,
    int length,
    int repeats,
    int element,
    int *can_allocate);

/** } */

#endif
