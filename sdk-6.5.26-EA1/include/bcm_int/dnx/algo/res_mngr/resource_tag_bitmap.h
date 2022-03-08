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
*************/
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
 *************/
/** { */

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

/** } */

/*************
 * TYPE DEFS *
 *************/
/** { */

/**
 * \brief
 * Pass this struct when creating a new resource tag bitmap.
 *
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
    int grain_size[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * Number of tags.
     */
    uint8 nof_tags_levels;
    /**
     * Maximum tag value. A max_tag_value of 0 means the bitmap doesn't use tags.
     */
    uint32 max_tag_value[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /**
     * RESOURCE_TAG_BITMAP_CREATE_* flags used for this bitmap.
     */
    uint32 flags;
} resource_tag_bitmap_create_info_t;

/**
 * \brief
 * Pass this struct when creating or freeing a block of elements.
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
    uint8 first_reference;
    /*
     * Relevant only while freeing elements.
     * Boolean indication of whether any of the grains became empty as a result of the current change
     */
    uint8 last_reference;
} resource_tag_bitmap_alloc_info_t;

/*
 * FOR INTERNAL USE ONLY:
 * Internal struct of resource tag bitmap.
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
     * elements per tag grain
     */
    int grainSize;
    /**
     * bytes per tag
     */
    int tagSize;
    /**
     * represent nof bytes for the first tag
     */
    int firstTagSize;
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
     * Number of tags levels.
     */
    uint8 nof_tags_levels;
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
    uint32 tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * If the bitmap was created with the RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG flag, * then set this to TRUE to
     * force the tag, and to FALSE to release it.
     */
    int force_tag;
    /*
     * Boolean indication of whether it's the first reference for a tagged grain.
     */
    uint8 first_reference;
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
    int grain_size[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * Max tag value.
     */
    uint32 max_tag_value[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];
    /*
     * Number of tags
     * */
    uint8 nof_tags_levels;
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
    uint8 first_reference;
} resource_tag_bitmap_extra_arguments_alloc_info_t;
/** } */

/*************
* GLOBALS   *
*************/
/*
 * {
 */
/*
 * }
 */

/*************
* FUNCTIONS *
*************/
/*
 * {
 */
/**
 * \brief - Return number of grains in a tagged bitmap resource.
 *
 * \param [in] count - number of elements in the tagged bitmap resource
 * \param [in] grain_size - number of elements in the grain.
 * \param [in] low - element with the lowest id.
 * \param [in] flags - flags of the tagged bitmap resource
 *
 * \return
 *    number of grains in the tagged bitmap resource.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int resource_tag_bitmap_get_nof_grains(
    int count,
    int grain_size,
    int low,
    int flags);

/**
 * \brief - Create a tagged bitmap resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to be destroyed.
 * \param [in] create_info - Relevant create information.
 * \param [in] extra_arguments - Placeholder to fill prototype requirement for advanced algorithms.
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
    uint32 node_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_info,
    void *extra_arguments,
    uint32 alloc_flags);

/**
 * \brief
 *   Allocate an element or block of elements of a particular resource
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be allocated into.
 *   \param [in,out] alloc_info - All relevant data needed during the allocation.
 *                                  alloc_info.first_reference is an output that 
 *                                  indicate whether it's the first reference for a tagged grain.   
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
*****************************************************/
shr_error_e resource_tag_bitmap_alloc(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t * alloc_info,
    int *elem);

/*!
* \brief
*   Allocate a free element.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
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
*****************************************************/
shr_error_e resource_tag_bitmap_allocate_single(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
* \brief
*   Allocate several elements.
*
*   \param [in] unit - Relevant unit.
*   \param [in] node_id - Node ID.
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
*****************************************************/
shr_error_e resource_tag_bitmap_allocate_several(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    uint32 nof_elements,
    void *extra_arguments,
    int *element);

/**
 * \brief
 *   Free a single element of a particular bitmap.
 *   \param [in] unit - Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be freed from.
 *   \param [in] elem - The element to free.
 *   \param [out] extra_argument -  Will be filled with boolean indication of whether it's the last reference to that tagged grain.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *
 *   \see
 *     * None
*****************************************************/
shr_error_e resource_tag_bitmap_free(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int elem,
    void *extra_argument);

/**
 * \brief
 *   Free an element or block of elements of a particular bitmap.
 *   The only fields used in the free info are count for freeing one or more consecutive elements.
 *   \param [in] unit - Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be freed from.
 *   \param [in] nof_elements - Number of elements to br freed.
 *   \param [in] elem - The element to free(used a starting point).
 *   \param [out] extra_argument -  Will be filled with boolean indication of whether it's the last reference to that tagged grain.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *
 *   \see
 *     * None
*****************************************************/
shr_error_e resource_tag_bitmap_free_several(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 nof_elements,
    int elem,
    void *extra_argument);

/**
* \brief
*   Prints resource data includes tags and grains
*
*   \param [in] unit -  Relevant unit.
*   \param [in] node_id - Node ID.
*   \param [in] res_tag_bitmap - Resource tag bitmap to print.
*   \param [in] data - relevant data to be printed.
*  \return
*    \retval Zero if no error was detected
*    \retval Negative if error was detected. See \ref shr_error_e
*  \remark
*    None
*  \see
*    None
*****************************************************/

int resource_tag_bitmap_print(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_dump_data_t * data);

/**
 * \brief
 *   Free an element or block of elements of a particular bitmap.
 *   The only fields used in the free info are count for freeing one or more consecutive elements,
 *   or the fields associated with sparse allocation.
 *
 *   \param [in] unit -
 *       Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be freed from.
 *   \param [in,out] free_info -
 *       The only fields taken into consideration are the ALLOC_SPARSE flag and the fields related to it. 
 *          In addition, last_refernce will be an output that will represent whether it's the last reference for a tagged grain.
 *   \param [in] elem -
 *      The element to free.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *
 *   \see
 *     * None
*****************************************************/
shr_error_e resource_tag_bitmap_advanced_free(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_alloc_info_t * free_info,
    int elem);

/*
 *  Ensure that all grains in an alloc are tagged accordingly.
 */

/**
 * \brief - Force set a tag to a range of elements..
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap to set tag on.
 * \param [in,out] tag_info - Tag info.
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
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    void *tag_info);

/**
 * \brief - Get the tag for a specific element.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
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
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    void *tag_info);

/**
 * \brief
 *   Clear all the elements of the bitmap.
 *
 *   \param [in] unit -
 *       Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap to be cleared.
 *   \return
 *      BCM_E_NONE if successful
 *      BCM_E_* as appropriate otherwise
 *   \remark
 *      None
 *   \see
 *       dnx_algo_res_simple_bitmap_clear
*****************************************************/
shr_error_e resource_tag_bitmap_clear(
    int unit,
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap);

/**
 * \brief - Returns the number of free elements for this resource.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
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
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int *nof_free_elements);

/**
* \brief
*   Gets number of used elements in the given range.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
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
*****************************************************/
shr_error_e resource_tag_bitmap_nof_allocated_elements_in_range_get(
    int unit,
    uint32 node_id,
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
 * \param [in] node_id - Node ID.
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
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 grain_index,
    uint32 *nof_allocated_elements);

/**
 * \brief - Returns the create info used during create.
 *
 * \param [in] unit - ID of the device to be used with this bitmap.
 * \param [in] node_id - Node ID.
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
    uint32 node_id,
    resource_tag_bitmap_t res_tag_bitmap,
    resource_tag_bitmap_create_info_t * create_info);

/*
* \brief
*   Check whether an element was allocated.
*
*    \param [in] unit - Relevant unit.
*    \param [in] node_id - Node ID.
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
*****************************************************/
shr_error_e resource_tag_bitmap_is_allocated(
    int unit,
    uint32 node_id,
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
