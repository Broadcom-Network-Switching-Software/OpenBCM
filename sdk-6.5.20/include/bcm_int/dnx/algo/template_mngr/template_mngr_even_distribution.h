/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * algo_port_pp_esem_cmd_allocation.h
 *
 *  Created on: Feb 25, 2020
 *      Author: eb892187
 */

#ifndef INCLUDE_BCM_INT_DNX_ALGO_TEMPLATE_MNGR_TEMPLATE_MNGR_EVEN_DISTRIBUTION_H_
#define INCLUDE_BCM_INT_DNX_ALGO_TEMPLATE_MNGR_TEMPLATE_MNGR_EVEN_DISTRIBUTION_H_

#include <bcm_int/dnx/algo/template_mngr/multi_set.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>

/**
 * \brief -
 *   This function is used to allocate in even distribution.
 *   The distribution is even over grain size.
 *
 *   In order to use this Template Manager the following arguments must be defined
 *    and given to the 'create' function in the 'init' stage as a part of the 'extra_arguments' input:
 *   (Since we are using 'smart template' functions
 *     the extra information is stored in the 'smart_template_create_info_t' structure)
 *   'resource_flags' field must have the following flags set
 *     [1] RESOURCE_TAG_BITMAP_CREATE_ALLOW_IGNORING_TAG
 *     [2] RESOURCE_TAG_BITMAP_CREATE_ALLOW_FORCING_TAG
 *   'resource_create_info.grain_size' must be set to the value that we would like to even the distribution on
 *   'resource_create_info.max_tag_value' must be set to be the same as grain size
 *
 * \param [in] unit - Relevant unit
 * \param [in] module_id - Relevant module_id
 * \param [in] multi_set_template - Relevant template manager
 * \param [in] flags - Allocation flags (template manager flags)
 * \param [in] profile_data - The data that we would like to allocate id to
 * \param [in] nof_references - Number of references to the profile
 * \param [in] extra_arguments - Extra allocation arguments
 * \param [in,out] profile - If 'allocate with ID' flag is set this parameter should point to the asked ID
 *      this parameter will contain the ID of the allocated profile associated with the profile data
 * \param [in,out] first_reference -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of whether it's the first reference to this profile. * \return
 *   shr_error_e
 *
 * \remark
 *   Reverse order allocation:
 *   this function is allocating in reverse order
 *   (i.e. the larger ID's will be prioritized over the smallest ones)
 *   Note that this priority is secondary to the even distribution
 *
 * \see
 *   dnx_algo_smart_template_create
 *   dnx_algo_smart_template_allocate
 */
shr_error_e dnx_algo_template_mngr_even_distribution_allocate(
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
 * \brief -
 *   This function is used to free element allocated element *
 *
 * \param [in] unit - Relevant unit
 * \param [in] module_id - Relevant module_id
 * \param [in] multi_set_template - Relevant template manager
 * \param [in] profile - The profile we want to free
 * \param [in] nof_references - Number of references to be freed.
 * \param [in,out] last_reference -
 *      Pointer to memory to load output into. \n
 *      \b As \b output - \n
 *        Will be filled with boolean indication of whether it's the last reference to this profile.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   see dnx_algo_port_pp_esem_cmd_allocate instructions for this template manager
 *
 * \see
 *   dnx_algo_smart_template_free
 */
shr_error_e dnx_algo_template_mngr_even_distribution_free(
    int unit,
    uint32 module_id,
    multi_set_t multi_set_template,
    int profile,
    int nof_references,
    uint8 *last_reference);

#endif /* INCLUDE_BCM_INT_DNX_ALGO_TEMPLATE_MNGR_TEMPLATE_MNGR_EVEN_DISTRIBUTION_H_ */
