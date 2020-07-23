/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file resource_tag_bitmap.h
 *
 * \brief
 * Overview
 *
 * The resource tag bitmap is used to allocate resources in the system according to preset
 * criteria.
 *
 * Glossary:
 * bitmap / resource / tagged bitmap - one instance of resource_tag_bitmap.
 * element - a single bit in the bitmap, usually mapped to a table index (outside of the bitmap scope).
 * block - one or more elements that are allocated in a single allocation. Usually consecutive, but in the
 *          case of sparse allocation (allocation using the \ref RESOURCE_TAG_BITMAP_ALLOC_SPARSE
 *          flag) , could be non-consecutive.
 * pool - the set of all elements in the resource.
 * grain - a set of consecutive elements that have a common property represented by tag.
 * tag - an identifier of a property common to all elements in a grain.
 *
 *
 * During allocation, the bitmap will look for a free block of elements as defined by the user,
 * in a grain which fits the given tag.
 * If a matching free block was found, then the first element of that block will be returned.
 * If no grain with the require tag existed, then the grain will search for a free block
 * in a grain that doesn't have a tag, and will assign the tag to the grain.
 *
 * If no tag is used, then blocks can be allocated anywhere in the pool.
 *
 * Some exceptions can be made to the basic algorithm, by using the
 * RESOURCE_TAG_BITMAP_ALLOC_* flags.
 */
#ifndef RESOURCE_TAG_BITMAP_H_INCLUDED
/*
 * {
 */
#define RESOURCE_TAG_BITMAP_H_INCLUDED

/** { */
/*************
* INCLUDES  *
 */
/** { */
/* must be first */
#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>

#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
/** } */

/*************
 * DEFINES   *
 */
/** { */

/**
 * \brief
 * Flags for \ref resource_tag_bitmap_alloc \ref resource_tag_bitmap_free
 * {
 */
/**
 * For alloc only, allocate the element with ID.
 */
#define RESOURCE_TAG_BITMAP_ALLOC_WITH_ID              SAL_BIT(0)
/**
 * \brief
 * If this flag is set, then an allocation check will run. All the steps to allocate an
 * element will be taken, and the allocated element / relevant error code will be returned,
 * but no sw state will be changed, and no error will be printed.
 * If the check allocation was successful, then the returned element should be allocated WITH_ID.
 */
#define RESOURCE_TAG_BITMAP_ALLOC_CHECK_ONLY           SAL_BIT(1)
/**
 * \brief
 * If this flag is set, then the allocator will only allocate in a grain that wasn't already set with a tag.
 */
#define RESOURCE_TAG_BITMAP_ALLOC_IN_EMPTY_GRAIN       SAL_BIT(8)

/*
 * Next RESOURCE_TAG_BITMAP_ALLOC_ flags are set by the extra argument parameter (bits 25:31).
 */
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
 * If offset >= align, BCM_E_PARAM will be returned. If align is zero or negative, it will
 * be treated as if it were 1.
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
 * When this flag is set during creation, then some algorithm optimizations will be
 * skipped to save memory.
 * It's unnecessary to use it unless the max_tag_value is very high.
 * If the max_tag_value is above \ref RESOURCE_TAG_BITMAP_CRITICAL_MAX_TAG_VALUE
 * then this flag will be set automatically.
 */
#define RESOURCE_TAG_BITMAP_CREATE_NO_OPTIMIZATION_PER_TAG            SAL_BIT(26)
/**
 * \brief
 * When this flag is set during creation, then the
 * \ref resource_tag_bitmap_nof_used_elements_in_grain_get function can be used.
 * Note that it's faster compared to \ref sw_state_resource_nof_allocated_elements_in_range_get ,
 * but has higher memry consumption.
 */
#define RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN         SAL_BIT(27)
/**
 * \brief
 * When this tag is set during creation, then a tag can set with \ref resource_tag_bitmap_tag_set
 * with the force_tag option, and simple allocation will not be able to override this tag.
 */
#define RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG                  SAL_BIT(28)


#define RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS         SAL_BIT(29)

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

/** } */

/*************
 * TYPE DEFS *
 */
/** { */

/**
 * \brief
 * Pass this struct when creating a new resource tag bitmap.
 */
typedef struct
{
    /**
     * Minimum valid element ID.
     */
    int low_id;
    /**
     * Total number of elements.
     */
    int count;
    /**
     * Number of elements per grain. Only relevant if tags are used.
     * Count must be evenly divided by this value, unless the flag
     * \ref RESOURCE_TAG_BITMAP_CREATE_TRIM_FIRST_AND_LAST_GRAINS is set.
     */
    int grain_size;
    /**
     * Maximum tag value. A max_tag_value of 0 means the bitmap doesn't use tags.
     */
    uint32 max_tag_value;
    /**
     * RESOURCE_TAG_BITMAP_CREATE_* flags used for this bitmap.
     */
    uint32 flags;
} resource_tag_bitmap_create_info_t;

/**
 * \brief
 * Pass this struct when creating or freeing a block of elements.
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
    uint32 tag;
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
     * First invalid ID after the required alloation range.
     * I.e., to allocate in the range {3,6}, set range_start to 3 and range_end to 7.
     */
    int range_end;
} resource_tag_bitmap_alloc_info_t;

/*
 * FOR INTERNAL USE ONLY:
 * Internal struct of resource tag bitmap.
 */
typedef struct
{
    /**
     * lowest element ID
     */
    int low;
    /**
     * number of elements
     */
    int count;
    /**
     * number of elements in use
     */
    int used;
    /**
     * elements per tag grain
     */
    int grainSize;
    /**
     * bytes per tag
     */
    int tagSize;
    /**
     * first element of last freed block
     */
    int *lastFree;
    /**
     * next element after last alloc block
     */
    int *nextAlloc;
    /**
     * pointer to base of tag data (after data)
     */
    SHR_BITDCL *tagData;
    /**
     * For each grain, indicate whether it's forced tag or a modifiable tag.
     */
    SHR_BITDCL *forced_tag_indication;
    /**
     * Data.
     */
    SHR_BITDCL *data;
    /**
     * Maximum tag value.
     */
    int max_tag_value;
    /**
     * Tag tracking bitmap.
     */
    SHR_BITDCL *tag_tracking_bitmap;
    /**
     * Flags used to create this res_tag_bitmap.
     */
    uint32 flags;
    /**
     * Number of allocated bits in one grain.
     */
    uint32 *nof_allocated_elements_per_grain;
}  *resource_tag_bitmap_t;

/**
 * Includes the information user needs to supply for tag_get/tag_set in res_tag_bitmap.
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
    uint32 tag;
    /*
     * If the bitmap was created with the RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG flag,
     *                          then set this to TRUE to force the tag, and to FALSE to release it.
     */
    int force_tag;
} resource_tag_bitmap_tag_info_t;

/**
 * Includes the information user needs to supply for res_tag_bitmap creation.
 * The struct can be passed as extra_arguments and then the Basic algorithm will behave as advanced.
 * \see resource_tag_bitmap_create
 */
typedef struct
{
    /*
     * Grain size.
     */
    int grain_size;
    /*
     * Max tag value.
     */
    uint32 max_tag_value;
} resource_tag_bitmap_extra_arguments_create_info_t;

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
    uint32 tag;
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
} resource_tag_bitmap_extra_arguments_alloc_info_t;

/** } */

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
 * \brief - Create a tagged bitmap resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to be destroyed.
 * \param [in] create_info - Relevant create information.
 * \param [in] extra_arguments - Placeholder to fill prototype requirement for advanced algorithms.
 * \param [in] count - Count of the resource tag bitmap. Specified in the relevant sw state xml file.
 * \param [in] alloc_flags - SW state allocation flags.
 *       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   There are count / grain_size grains, and each one has a tag associated.
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    uint32 count,
    uint32 alloc_flags);

/**
 * \brief
 *   Allocate an element or block of elements of a particular resource
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] module_id - Module ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be allocated into.
 *   \param [in] alloc_info - All relevant data needed during the allocation.
 *   \param [in,out] elem - Pointer to place the allocated element.
 *       \b As \b output - \n
 *       Holds the first *allocated* element in the pattern.
 *       \b As \b input - \n
 *       If flag \ref DNX_ALGO_RES_ALLOCATE_WITH_ID is set, pass the first element to be *allocated* here.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      This will allocate a single block of the requested number of elements
 *      of this resource (each of which may be a number of elements taken from
 *      the underlying pool).
 *
 *   \see
 *   * None
 */
shr_error_e resource_tag_bitmap_alloc(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t alloc_info,
    int *elem);

/*!
* \brief
*   Allocate a free element.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Resource tag bitmap.
*    \param [in] flags - DNX_ALGO_RES_ALLOCATE_* flags
*    \param [in] extra_arguments - Placeholder to fill prototype requirement for advanced algorithms.
*    \param [in] element - Pointer to place the allocated element.
*      \b As \b output - \n
*        Holds the allocated element.
*      \b As \b input - \n
*        If flag \ref DNX_ALGO_RES_ALLOCATE_WITH_ID is set, pass the element to be allocated here.
*   \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    dnx_algo_res_allocate
*    sw_state_res_tag_bitmap_alloc
*    shr_error_e
 */
shr_error_e resource_tag_bitmap_allocate_single(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
* \brief
*   Allocate several elements.
*
*   \param [in] unit - Relevant unit.
*   \param [in] module_id - Module ID.
*   \param [in] res_tag_bitmap - Resource tag bitmap.
*   \param [in] flags - DNX_ALGO_RES_ALLOCATE_* flags
*   \param [in] nof_elements - Number of elements.
*   \param [in] extra_arguments - Placeholder to fill prototype requirement for advanced algorithms.
*   \param [in,out] element -Pointer to place the allocated element.
*       \b As \b output - \n
*       Holds the allocated element.
*       \b As \b input - \n
*       If flag \ref DNX_ALGO_RES_ALLOCATE_WITH_ID is set, pass the element to be allocated here.
*   \return
*       shr_error_e - Error return value
*   \remark
*       None
*   \see
*       dnx_algo_res_allocate_several
*       sw_state_res_tag_bitmap_allocate_several
*       shr_error_e
 */
shr_error_e resource_tag_bitmap_allocate_several(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element);

/**
 * \brief
 *   Free a single element of a particular bitmap.
 *   \param [in] unit - Relevant unit.
 *   \param [in] module_id - Module ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be freed from.
 *   \param [in] elem - The element to free.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *
 *   \see
 *     * None
 */
shr_error_e resource_tag_bitmap_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int elem);

/**
 * \brief
 *   Free an element or block of elements of a particular bitmap.
 *   The only fields used in the free info are count for freeing one or more consecutive elements.
 *   \param [in] unit - Relevant unit.
 *   \param [in] module_id - Module ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be freed from.
 *   \param [in] nof_elements - Number of elements to br freed.
 *   \param [in] elem - The element to free(used a starting point).
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *
 *   \see
 *     * None
 */
shr_error_e resource_tag_bitmap_free_several(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 nof_elements,
    int elem);

/**
 * \brief
 *   Free an element or block of elements of a particular bitmap.
 *   The only fields used in the free info are count for freeing one or more consecutive elements,
 *   or the fields associated with sparse allocation.
 *
 *
 *   \param [in] unit -
 *       Relevant unit.
 *   \param [in] module_id - Module ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be freed from.
 *   \param [in] free_info -
 *       The only fields taken into consideration are the ALLOC_SPARSE flag and the fields related to it.
 *   \param [in] elem -
 *      The element to free.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *
 *   \see
 *     * None
 */
shr_error_e resource_tag_bitmap_advanced_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t free_info,
    int elem);

/**
 * \brief - Destroy a tagged bitmap resource
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to be destroyed.
 * \param [in] extra_arguments - Placeholder to fill prototype requirement for advanced algorithms.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_destroy(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    void *extra_arguments);

/*
 *  Ensure that all grains in an alloc are tagged accordingly.
 */

/**
 * \brief - Force set a tag to a range of elements..
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to set tag on.
 * \param [in] tag_info - Tag info.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_tag_set(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_tag_info_t * tag_info);

/**
 * \brief - Get the tag for a specific element.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to get the tag from.
 * \param [in] tag_info - Tag info.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_tag_get(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_tag_info_t * tag_info);

/**
 * \brief
 *   Clear all the elements of the bitmap.
 *
 *   \param [in] unit -
 *       Relevant unit.
 *   \param [in] module_id - Module ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be cleared.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      None
 *   \see
 *       dnx_algo_res_simple_bitmap_clear
 */
shr_error_e resource_tag_bitmap_clear(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap);

/**
 * \brief - Check the status of a specific element.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] count - Number of elements to check in this block.
 * \param [in] elem - The element to check (or base of the block to check).
 *
 * \return
 *   shr_error_e
 *    _SHR_E_NOT_FOUND if the element is not in use.
 *    _SHR_E_EXISTS if the element is in use.
 *    _SHR_E_PARAM if the element is not valid.
 *    _SHR_E_* as appropriate otherwise.
 *
 * \remark
 *      This will check whether the requested block of the resource is
 *      allocated.  Note that if any element of the resource in the range of
 *      [elem..(elem+count-1)] (inclusive) is not free, it returns
 *      BCM_E_EXISTS; it will only return BCM_E_NOT_FOUND if all elements
 *      within the specified block are free.
 *
 *      Normally this should be called to check on a specific block (one that
 *      is thought to exist or in preparation for allocating it WITH_ID).
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_check(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int count,
    int elem);

/**
 * \brief - Check the status of a block of elements.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] tag - Value of the tag to check.
 * \param [in] count - Number of elements to check in this block.
 * \param [in] elem - The element to check (or base of the block to check).
 *
 * \return
 *   shr_error_e
 *      _SHR_E_EMPTY if none of the elements are in use.
 *      _SHR_E_FULL if all of the elements are in use.
 *      _SHR_E_CONFIG if elements are in use but block(s) do not match the tag.
 *      _SHR_E_EXISTS if some of the elements are in use but not all of them.
 *      _SHR_E_PARAM if any of the elements is not valid.
 *      _SHR_E_* as appropriate otherwise.
 * \remark
 *      As \ref resource_tag_bitmap_check except this:
 *
 *      WANRING: The tagged bitmap allocator does not track blocks internally
 *      and so it is possible that if there are two adjacent blocks with
 *      identical tags both allocated and this is called to check whether safe
 *      to 'reallocate', will falsely indicate that it can be done.  However,
 *      this will consider different tags to indicate different blocks, so will
 *      not assert false true for the case if the adjacent blocks have
 *      different tags, unless the block size is smaller than the tag size, in
 *      which case it still could falsely claim the operation is valid. Also,
 *      'reallocate' in a similar manner of a large block to a smaller block
 *      could leak underlying resources.
 * \see
 *   resource_tag_bitmap_check
 */
shr_error_e resource_tag_bitmap_check_all_tag(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 tag,
    int count,
    int elem);

/**
 * \brief - Check the status of a block of elements.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] count - Number of elements to check in this block.
 * \param [in] elem - The element to check (or base of the block to check).
 *
 * \return
 *   shr_error_e
 *      _SHR_E_EMPTY if none of the elements are in use.
 *      _SHR_E_FULL if all of the elements are in use.
 *      _SHR_E_CONFIG if elements are in use but block(s) do not match the tag.
 *      _SHR_E_EXISTS if some of the elements are in use but not all of them.
 *      _SHR_E_PARAM if any of the elements is not valid.
 *      _SHR_E_* as appropriate otherwise.
 * \remark
 *      As \ref resource_tag_bitmap_check_all_tag except without explicit tag.
 *
 *      Unlike the other supported operations that work without explicit tag, this
 *      function assumes the first tag in the block is the desired tag.  If you
 *      want the normal default tag behaviour, provide NULL as the tag argument and
 *      call resource_tag_bitmap_check_all_tag instead.
 * \see
 *   resource_tag_bitmap_check
 */
shr_error_e resource_tag_bitmap_check_all(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int count,
    int elem);

/**
 * \brief - Returns the number of free elements for this resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [out] nof_free_elements - Pointer will be filled with the number
 *                                  of free elements in this resource.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_nof_free_elements_get(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int *nof_free_elements);

/**
* \brief
*   Gets number of used elements in the given range.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Relevant res tag bitmap.
*    \param [in] range_start - First element in range.
*    \param [in] nof_elements_in_range - Range size.
*    \param [out] nof_allocated_elements - Pointer to the memory where the return value will be saved.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
 */
shr_error_e sw_state_resource_nof_allocated_elements_in_range_get(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int range_start,
    int nof_elements_in_range,
    int *nof_allocated_elements);

/**
 * \brief - Returns the number of allocated elements per grain.
 *          Note that this API will be active only if _CREATE_GET_NOF_ELEMENTS_PER_GRAIN flag was given
 *          during creation of the resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] grain_index - Relevant grain index.
 * \param [out] nof_allocated_elements - Pointer will be filled with the number
 *                                  of allocated bits in this grain.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_nof_used_elements_in_grain_get(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 grain_index,
    uint32 *nof_allocated_elements);

/**
 * \brief - Returns the memory size of a bitmap. Used mainly by sw state diagnostics.
 *
 * \param [in] create_info - Relevant create information.
 *
 * \return
 *   int
 *
 * \remark
 *   None
 * \see
 *   None
 */
int resource_tag_bitmap_size_get(
    resource_tag_bitmap_create_info_t create_info);

/**
 * \brief - Returns the create info used during create.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] module_id - Module ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] create_info - Pointer to location in which will be saved the create info.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e resource_tag_bitmap_create_info_get(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_create_info_t * create_info);

/*
* \brief
*   Check whether an element was allocated.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] res_tag_bitmap - Resource tag bitmap.
*    \param [in] element - Element to be checked.
*    \param [in] is_allocated - Pointer to be filled with allocated / free indication.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    dnx_algo_res_allocate
*    sw_state_res_tag_bitmap_check
*    shr_error_e
 */
shr_error_e resource_tag_bitmap_is_allocated(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint8 *is_allocated);

/*
 * }
 */

/*
 * } RESOURCE_TAG_BITMAP_H_INCLUDED
 */
#endif
