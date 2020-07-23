/**
 * \file algo_l3_ecmp.h Internal DNX L3 Managment APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_L3_ECMP_H_INCLUDED
/*
 * {
 */
#define ALGO_L3_ECMP_H_INCLUDED

#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * }
 */

/**
 * \brief Wrapper function for allocation manager which creates tagged allocation bitmap for ECMP groups.
 *           This function is called on algo L3 init and it only creates the resource.
 *           The table is created using the number of elements = number of ECMP groups from DNX data
 *           and grain size equal to the number of groups indicated by the stage map (from DNX data).
 *           The tag will represent the ECMP group hierarchy,
 *           Refer to resource_tag_bitmap_create() for details about the input parameters.
 */
shr_error_e dnx_algo_l3_ecmp_bitmap_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

/**
 * \brief Wrapper function for allocation manager which allocates ECMP groups.
 *        Upon allocating the first element in the 2k range of the stage map,
 *        that range of values is saved for the given hierarchy in extra_arguments.
 *        Refer to resource_tag_bitmap_alloc() for details about the input parameters.
 *        extra_arguments - pointer to uint32 which represents the tag (ECMP hierarchy)
 */
shr_error_e dnx_algo_l3_ecmp_bitmap_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

 /**
 * \brief Wrapper function for allocation manager which frees ECMP groups.
 *        Upon freeing an element, the SW state numeric that counts the number of elements in each stage
 *        is updated with the decremented value.
 *        Refer to resource_tag_bitmap_free() for details about the input parameters.
 */
shr_error_e dnx_algo_l3_ecmp_bitmap_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element);

#endif/*_ALGO_L3_API_INCLUDED__*/
