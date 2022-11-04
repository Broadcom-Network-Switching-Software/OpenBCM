/**
 * \file algo_vlan.h Internal DNX VLAN Management APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_VLAN_H_INCLUDED
/*
 * {
 */
#define ALGO_VLAN_H_INCLUDED

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/**
 * Resource name defines for algo vlan
 * {
 */
#define DNX_ALGO_VLAN_VSI_RESOURCE "VSI"
#define DNX_VLAN_PORT_INGRESS_VIRTUAL_GPORT_ID_RESOURCE_NAME "VLAN-PORT INGRESS VIRTUAL GPORT ID"
#define DNX_VLAN_PORT_EGRESS_VIRTUAL_GPORT_ID_RESOURCE_NAME "VLAN-PORT EGRESS VIRTUAL GPORT ID"
#define DNX_ALGO_VLAN_TRANSLATE_ACTION_ID_INGRESS "ACTION_ID_INGRESS"
#define DNX_ALGO_VLAN_TRANSLATE_ACTION_ID_EGRESS "ACTION_ID_EGRESS"
#define DNX_ALGO_VLAN_RANGE_TEMPLATE_OUTER         "Vlan range outer"
#define DNX_ALGO_VLAN_RANGE_TEMPLATE_INNER         "Vlan range inner"
#define VLAN_RANGE_DEFAULT                       (BCM_VLAN_NONE)
/**
 * }
 */
/**
 * \brief
 *   allocate resource pool for all VLAN profiles types
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_algo_vlan_init(
    int unit);

/**
 * \brief
 *   deallocate resource pool for all VLAN profiles types
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_algo_vlan_deinit(
    int unit);

 /**
 * \brief - Initialize VLAN resources. \n 
 *  The function will initialize and create all required VLAN res-manager.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 * \par INDIRECT INPUT:
 *   * DNX-Data information such as nof VSIs , which features are enabled.
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * Initialize Res manager pools of VLAN module see the dnx_vlan_algo_res_init .c file which pools are initialized.
 * \remark
 *   * Assume Resource Manager is initialized \see dnx_algo_res_init
 * \see
 *   * None
 */
shr_error_e dnx_algo_vlan_res_init(
    int unit);

/**
 * \brief -
 *   Print the inner vlan range profile template content.
 * \param [in] unit - relevant unit
 * \param [in] data - template data
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 */
void dnx_vlan_range_template_inner_print_cb(
    int unit,
    const void *data);
/**
 * \brief -
 *   Print the outer vlan range profile template content.
 * \param [in] unit - relevant unit
 * \param [in] data - template data
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 */
void dnx_vlan_range_template_outer_print_cb(
    int unit,
    const void *data);

/**
 * \brief
 *   A wrapper function for allocation manager which allocates a single VSI entry.
 *   The function allows allocating a VSI 0 only at init time.
 * \param [in] unit - The unit number.
 * \param [in] module_id - Module ID.
*  \param [in] res_tag_bitmap - Resource tag bitmap.
*  \param [in] flags - DNX_ALGO_RES_ALLOCATE_* flags
*  \param [in] extra_arguments - Placeholder to fill prototype requirement for advanced algorithms.
*  \param [in] element - Pointer to place the allocated element.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_algo_vlan_vsi_res_mngr_allocate(
    int unit,
    uint32 module_id,
    void *res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
 * \brief
 *   A wrapper function for allocation manager which allocates a single VSI entry.
 *   The function allows allocating a VSI 0 only at init time.
 * \param [in] unit - The unit number.
 * \param [out] esem_size - Returned ESEM size value.
 * \return
 *   \retval Negative in case of an error.
 */
shr_error_e dnx_algo_vlan_esem_size_get(
    int unit,
    int *esem_size);

/*
 * }
 */
#endif/*_ALGO_VLAN_API_INCLUDED__*/
