/**
 * \file algo/sch/sch_alloc_mngr.h 
 *
 * Internal DNX SCH Alloc Mngr APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __SCH_ALLOC_MNGR_H_INCLUDED__
#define __SCH_ALLOC_MNGR_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Includes
 * {
 */
#include <bcm/types.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
/*
 * }
 */

/*
 * Defines:
 * {
 */

#define DNX_SCH_ALLOC_FIRST_IN_TAG(element,tag_size)  (element & (~(tag_size-1)))

/*
 * Max numbers of offsets.
 */
#define DNX_SCH_ALLOC_MAX_NOF_OFFSETS 3

/*
 * }
 */

/*
 * Enums:
 * {
 */
typedef enum
{
    DNX_SCH_REGION_TYPE_CON = 0,
    DNX_SCH_REGION_TYPE_INTERDIG_1 = 1,
    DNX_SCH_REGION_TYPE_SE = 3,
    DNX_SCH_REGION_TYPE_SE_HR = 15,
    DNX_SCH_REGION_TYPE_NOF,
    DNX_SCH_REGION_INVALID_MAPPING_MODE = 0xffff
} dnx_sch_region_type_e;

/*
 * }
 */

/*
 * Structs:
 * {
 */
/**
 * \brief 
 *   sch allocate flows from a region using patterns depending on flow and region type
 *
 *  info needed for flow allocation.
 */

typedef struct
{
    /*
     * pattern depending on flow and region type
     */
    uint32 pattern;

    /*
     * required tag:
     * * aggregated-SE mode in SE only region (shared shaper)
     * * modid in contiguous mode
     */
    int tags[RESOURCE_TAG_BITMAP_MAX_NOF_TAGS_LEVELS];

    /*
     * should the tag be ignored in allocation
     */
    int ignore_tag;

    /*
     * offset, part of the pattern
     */
    uint32 offs[DNX_SCH_ALLOC_MAX_NOF_OFFSETS];

    /*
     * number of valid offsets in offs array
     */
    int nof_offsets;

    /*
     * length of pattern
     */
    uint32 length;

    /*
     *  number of iterations of pattern
     */
    uint32 repeats;

    /*
     * Base alignment,  part of the pattern
     */
    int align;

} dnx_algo_sch_alloc_info_t;

/**
 * \brief 
 *   extra arguments needed for resource manager advanced
 *   algorithm.
 */
typedef struct
{
    /*
     * core id
     */
    int core_id;

    /*
     * region index
     */
    int region_index;
} dnx_algo_sch_extra_args_t;
/*
 * }
 */

/*
 * Functions:
 * {
 */

/**
 * \brief
 *   Create a new instance of region bitmap resource manager.
 * 
 *  \param [in] unit - Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap.
 *  \param [in] create_data -
 *    Pointed memory contains setup parameters required for the
 *    creation of the resource. See \ref dnx_algo_res_create_data_t
 *  \param [in] extra_arguments
 *      Pointer for extra arguments to be passed to the create function of an advanced algorithm.
 *  \param [in] alloc_flags - SW state allocation flags.
 *       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
 *  \retval Zero if no error was detected
 *  \retval Negative if error was detected. See \ref shr_error_e
 *  \remark
 *    The function creates 1 long bitmap in size of (region_size = 1024 flows)
 *  \see
 *    dnx_algo_res_create
 *    sw_state_res_tag_bitmap_create
 *    sw_state_res_tag_bitmap_tag_set
 */
shr_error_e dnx_algo_sch_bitmap_create(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 alloc_flags);

/**
 * \brief
 *   Allocate bit(s) in a region in flows bitmap.
 *   Each bit represent a flow.
 * 
 *  \param [in] unit -
 *    Relevant unit.
 *   \param [in] node_id - Node ID.
 *   \param [in] res_tag_bitmap - Resource tag bitmap.
 *  \param [in] flags -
 *    Special treatment:
 *      0 (None) - allocate some free element. 
 *      DNX_ALGO_RES_ALLOCATE_WITH_ID - allocate specific element (see
 *      also 'element' param info).
 *  \param [in] alloc_info -
 *    Pointer to dnx_algo_sch_alloc_info_t that holds information
 *    about how many bits to allocate .
 *    See \ref dnx_algo_sch_alloc_info_t
 *  \param [in,out] element - 
 *    Holds the allocated element id (serves as output if no flags were used).
 *    If WITH_ID flag was used this param serves as input and holds the
 *    element id number to allocate.
 *    If more than 1 bit allocated, element will hold the first allocated bit.
 *  \retval Zero if no error was detected
 *  \retval Negative if error was detected. See \ref shr_error_e
 *  \remark
 *    Pay attention that the element is an index in the full bitmap.
 *  \see
 *    dnx_algo_res_allocate
 *    sw_state_res_tag_bitmap_alloc_align_tag_sparce
 *    dnx_algo_sch_bitmap_create
 */
shr_error_e dnx_algo_sch_bitmap_allocate(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    uint32 flags,
    void *alloc_info,
    int *element);

/**
 * \brief
 *   Check whether an element is allocated.
 * 
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] node_id - Node ID.
 * \param [in] res_tag_bitmap - Resource tag bitmap.
 * \param [in] element -
 *      Element to be checked.
 * \param [out] is_allocated -
 *      Pointer to be filled with allocated / free indication.
 * \par DIRECT OUTPUT:
 *    shr_error_e - 
 * \remark
 *    None
 */
shr_error_e dnx_algo_sch_bitmap_is_allocated(
    int unit,
    uint32 node_id,
    void *res_tag_bitmap,
    int element,
    uint8 *is_allocated);

/*
 * } 
 */

#endif /* __SCH_ALLOC_MNGR_H_INCLUDED__ */
