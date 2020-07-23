/** \file ffc_alloc_mngr.h
 *
 * Interface for advanced ffc resource manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_access.h>

#define DNX_ALGO_FIELD_IPMF_1_FFC              "IPMF_1_FFC"

/**
* \brief
*  Init the advanced ffc resource allocation manager.
*
*  \param [in] unit - Device ID
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_ffc_ipmf_1_alloc_mngr_init(
    int unit);

/**
* \brief
*  The advanced ffc resource allocating manager "bitmap create" function.
*  Creates the bitmap that represents the allocation
*
*  \param [in] unit - Device ID
*  \param [in] module_id - Module ID.
*  \param [in] res_tag_bitmap - Resource tag bitmap.
*  \param [in] create_data - Creation data.
*  \param [in] extra_arguments - Optional extra arguments (not used)
*  \param [in] nof_elements - Number of elements of the resource manager. Specified in the relevant sw state xml file.
*  \param [in] alloc_flags - SW state allocation flags.
*       This flags value is determend by the variable type (dnx data for example) of the additional variables specified in the xml
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_ffc_ipmf_1_bitmap_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

/**
* \brief
*  The advanced ffc resource allocating manager "bitmap allocate" function.
*  Allocates a new element according to the given ffc_type.
*
*  \param [in] unit - Device ID
*  \param [in] module_id - Module ID.
*  \param [in] res_tag_bitmap - Resource tag bitmap.
*  \param [in] flags - flags for allocation (not used)
*  \param [in] ranges_v - Bitmap of the ranges available for allocation
*  \param [out] element - Index for the element that was allocated
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_ffc_ipmf_1_bitmap_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *ranges_v,
    int *element);
