/**
 * \file algo_l3_fec.h Internal DNX L3 Management APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_L3_FEC_H_INCLUDED

#define ALGO_L3_FEC_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/**
 *INCLUDE FILES:
 * {
 */
/**
 * }
 */

/**
 *Function declarations:
 * {
 */
/**
 * \brief Wrapper function for allocation manager which creates tagged allocation bitmap for FEC entries.
 *           This function is called on algo L3 init and it only creates the resource.
 *           The table is created using the number of elements = number of FECs from DNX data
 *           and grain size equal to the number of groups indicated by the stage map (from DNX data).
 *           The tag will represent the FEC group hierarchy,
 *           Refer to resource_tag_bitmap_create() for details about the input parameters.
 */
shr_error_e dnx_algo_l3_fec_bitmap_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

/**
 * \brief Wrapper function for allocation manager which allocates FEC entries.
 *        Upon allocating the first element in the 8k physical FECs range of the stage map,
 *        that range of values is saved for the given hierarchy in extra_arguments.
 *        Refer to resource_tag_bitmap_alloc() for details about the input parameters.
 *        extra_arguments - pointer to uint32 which represents the tag (Super FEC type)
 */
shr_error_e dnx_algo_l3_fec_bitmap_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
 * \brief Wrapper function for allocation manager which frees FEC entries.
 *        Upon freeing the last element in the 8k physical FECs range of the stage map,
 *        that range of values is updated and is no longer saved for a given hierarchy.
 *        Refer to resource_tag_bitmap_free() for details about the input parameters.
 */
shr_error_e dnx_algo_l3_fec_bitmap_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element);

/**
 * }
 */
#endif/*_ALGO_L3_API_INCLUDED__*/
