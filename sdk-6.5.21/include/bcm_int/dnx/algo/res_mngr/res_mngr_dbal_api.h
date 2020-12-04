/** \file res_mngr_dbal_api.h
 *
 * Internal DNX resource manager DBAL APIs 
 * this file include all the APIs available only for DBAL. DBAL is a lower level module that genarlly doesn't call to 
 * BCM layer APIs. those APIs are special cases that need to be used by DBAL for resource management purposes.  
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef ALGO_RES_MNGR_DBAL_API_INCLUDED
/* { */
#define ALGO_RES_MNGR_DBAL_API_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/dbal.h>
/**
 * This define will be return from dnx_algo_res_get_next if we can't provide the next element.
 */
#define DNX_ALGO_RES_DBAL_ILLEGAL_ELEMENT (-1)

/**
* \brief Retuns is_allocated if the relevant resource allocated according to the resource name. 
*   \param [in] unit -
*       Relevant unit.
*   \param [in] core_id -
*       Relevant core. If resource is per core, it must be a valid core id, otherwise, must be _SHR_CORE_ANY.
*   \param [in] sub_resource_index -
*       Relevant sub resource index.
*   \param [in] index -
*       Relevant resource index.
*   \param [in] table_id -
*       related table ID that requested the resource, table_id is 0, it means that asking if the resource is alocated at
*       all not related to specific table
*   \param [in] result_type - related result type that requested the resource
*   \param [in] field_id -
*       dbal field ID that is relatd to the resource
*   \param [in] is_aloc_table_any - checks if the value is allocated regardsless to the table ID.
*   \param [out] is_allocated - pointer that indicates if the field is allocated.
*  \return
*    \retval Zero if no error occured
*    \retval  See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_dump_data_t
*    shr_error_e
 */
shr_error_e dnx_algo_res_dbal_is_allocated(
    int unit,
    int core_id,
    int sub_resource_index,
    int index,
    dbal_tables_e table_id,
    int result_type,
    dbal_fields_e field_id,
    int is_aloc_table_any,
    uint8 *is_allocated);

/**
* \brief Given a resource name, core, sub resource index and element, returns the next allocated element for this 
*       resource on this core. The returned element will be equal to or greater than the given element. It is the user's
*       responsibility to update it between iterations. When there are no more allocated elements,
*       DNX_ALGO_RES_ILLEGAL_ELEMENT will be returned.
*   \param [in] unit -
*       Relevant unit.
*   \param [in] core_id -
*       Relevant core. If resource is per core, it must be a valid core id, otherwise, must be _SHR_CORE_ANY.
*   \param [in] sub_resource_index -
*       Relevant sub resource index.
*   \param [in] table_id - related table ID that requested the resource
*   \param [in] field_id - relevant dbal field ID
*   \param [out] result_type - the result type of the next allocated value
*   \param [in,out] index -
*       \b As \b input - \n
*       The element to start searching for the next allocated element from.
*       \b As \b output - \n
*       he next allocated element. It will be >= from the element given as input.
*  \return
*    \retval Zero if element exists
*    \retval _SHR_E_NOT_FOUND if element is not exists. See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_dump_data_t
*    shr_error_e
 */
shr_error_e dnx_algo_res_dbal_get_next(
    int unit,
    int core_id,
    int sub_resource_index,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    int *result_type,
    int *index);

/** this function is used only for DBAL unit-tests do not use it  */
shr_error_e dnx_algo_res_dbal_alloc(
    int unit,
    int core_id,
    int sub_resource_index,
    int index,
    dbal_tables_e table_id,
    int result_type,
    dbal_fields_e field_id);

/** this function is used only for DBAL unit-tests do not use it  */
shr_error_e dnx_algo_res_dbal_free(
    int unit,
    int core_id,
    int sub_resource_index,
    int index,
    dbal_tables_e table_id,
    int result_type,
    dbal_fields_e field_id);
/* } */

#if 0
/**
* \brief
*   Retuns the max number of elementes in the resource.
*   \param [in] unit -
*       Relevant unit.
*   \param [in] core_id -
*       Relevant core. If resource is per core, it must be a valid core id, otherwise, must be _SHR_CORE_ANY.
*   \param [in] sub_resource_index -
*       Relevant sub resource index.
*   \param [in] resource_name - Name used to create this resource.
*   \param [out] nof_elements - number of elements.
*  \return
*    \retval Zero if element exists
*    \retval _SHR_E_NOT_FOUND if element is nit exists. See \ref shr_error_e
*  \remark
*    None
*  \see
*    dnx_algo_res_dump_data_t
*    shr_error_e
 */
shr_error_e dnx_algo_res_dbal_nof_elements_get(
    int unit,
    int core_id,
    int sub_resource_index,
    char *resource_name,
    uint32 *nof_elements);
#endif
#endif/*_ALGO_RES_MNGR_DBAL_API_INCLUDED_*/
