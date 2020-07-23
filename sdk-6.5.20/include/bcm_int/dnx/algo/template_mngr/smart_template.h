/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 * \file smart_template.h
 */

#ifndef SMART_TEMPLATE_H_INCLUDED
/*
 * { 
 */
#define SMART_TEMPLATE_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
#include <bcm_int/dnx/algo/template_mngr/multi_set.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>

/**
 * \brief Smart template manager.
 *
 * This template manager advanced algorithm is used to allocate template manager profiles
 * in an advanced manner by using a built in resource manager. All the options available to use
 * via the resource_tag_bitmap will be in effect here.
 *
 * NOTE: The smart template currently doesn't support allocating more than one profile per entry.
 * Attempting to allocate would be successful, but there's currently no way to free all allocated profiles.
 */

/*
 * TYPEDEFS:
 * { 
 */

/**
 * \brief
 * Pass this struct when creating the smart template to customize the bitmap that the
 * tagged bitmap that the template will be using.
 */
typedef struct
{
    uint32 resource_flags;
    resource_tag_bitmap_extra_arguments_create_info_t resource_create_info;
} smart_template_create_info_t;

/**
 * \brief
 * Pass this struct when allocating entries to provide specific parameters for the allocation.
 */
typedef struct
{
    uint32 resource_flags;
    uint32 nof_refs;
    resource_tag_bitmap_extra_arguments_alloc_info_t resource_alloc_info;
} smart_template_alloc_info_t;

/*
 * } 
 */

/**
 * \brief
 *   Callback to allocate a free profile of an advanced algorithm.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 *  \param [in] flags -
 *      SW_STATEALGO_TEMPLATE_ALLOCATE_* flags
 *   \param [in] nof_references -
 *      Number of references to be allocated.
 *  \param [in] profile_data -
 *      Pointer to memory holding template data to be saved to a profile.
 *  \param [in] extra_arguments -
 *      A pointer to smart_template_create_info_t. See \ref resource_tag_bitmap_create for
 *      more info on the inner struct.
 *  \param [in,out] profile -
 *      Pointer to place the allocated profile.
 *      \b As \b output - \n
 *        Holds the allocated profile.
 *      \b As \b input - \n
 *        If flag \ref DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID is set, pass the profile to be allocated here.
 *  \param [in,out] first_reference -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of the profile's prior existence. \n
 *        If TRUE, this is the first reference to the profile, and the data needs to be
 *          written to the relevant table.
 *
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 */
shr_error_e dnx_algo_smart_template_allocate(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    uint32 flags,
    int nof_references,
    void *profile_data,
    void *extra_arguments,
    int *profile,
    uint8 *first_reference);

/**
 * \brief
 *   Create a new instance of dnx_algo_smart_template.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 *  \param [in] create_data -
 *      Pointed memory contains setup parameters required for the
 *      creation of the template.
 *  \param [in] extra_arguments -
 *      A pointer to smart_template_alloc_info_t. See \ref resource_tag_bitmap_allocate_several ,
 *      \ref resource_tag_bitmap_allocate_single for more info on the inner struct.
 *  \param [in] alloc_flags - SW state allocation flags.
 *       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 */
shr_error_e dnx_algo_smart_template_create(
    int unit,
    uint32 module_id,
    multi_set_t * multi_set_template,
    dnx_algo_template_create_data_t * create_data,
    void *extra_arguments,
    uint32 alloc_flags);

/**
 * \brief
 *   Callback to free an allocated profile in an advanced algorithm.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 *  \param [in] profile -
 *      Profile to be freed.
 *   \param [in] nof_references -
 *      Number of references to be freed.
 *  \param [in,out] last_reference -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of whether it's the last reference to this profile.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 */
shr_error_e dnx_algo_smart_template_free(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    int profile,
    int nof_references,
    uint8 *last_reference);

/**
 * \brief
 *   Free all reference to all profiles of this template.
 *
 *  \param [in] unit -
 *      Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 */
shr_error_e dnx_algo_smart_template_clear(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template);

/**
 * \brief
 *   Sets tag to a specific element in the allocation_bitmap.
 *
 *  \param [in] unit - Relevant unit.
 *  \param [in] module_id - Module Id.
 *  \param [in] multi_set_template - Mutli set template.
 *  \param [in] algorithm_info - Structure that contains element, tag and force tag flag.
 * \return
 *   shr_error_e
 * \remark
 *      None
 * \see
 *      shr_error_e
 */
shr_error_e dnx_algo_smart_template_tag_set(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    resource_tag_bitmap_tag_info_t * algorithm_info);

/**
* \brief
*   Get the tag of specific element.
*
*    \param [in] unit - Relevant unit.
*    \param [in] module_id - Module ID.
*    \param [in] multi_set_template - Mutli set template.
*    \param [out] algorithm_info - Pointer to save the element, tag and force tag flag.
*  \return
*    shr_error_e - Error return value
*  \remark
*    None
*  \see
*    shr_error_e
 */
shr_error_e dnx_algo_smart_template_tag_get(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    resource_tag_bitmap_tag_info_t * algorithm_info);
/*
 * } 
 */
#endif /* SMART_TEMPLATE_H_INCLUDED */
