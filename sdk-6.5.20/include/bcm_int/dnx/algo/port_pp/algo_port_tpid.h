/**
 * \file algo_port_tpid.h 
 * Internal DNX Port TPID Managment APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_PORT_TPID_H_INCLUDED
/*
 * { 
 */
#define ALGO_PORT_TPID_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

#include <bcm_int/dnx/algo/template_mngr/template_mngr.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/port_tpid_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief 
 * Template name defines for algo Port TPID LLVP profile.
 * \see
 * dnx_algo_port_tpid_init
 */
#define DNX_ALGO_PORT_TPID_LLVP_PROFILE_INGRESS_TEMPLATE "Ingress LLVP Profile Template"

#define DNX_ALGO_PORT_TPID_LLVP_PROFILE_EGRESS_TEMPLATE  "Egress LLVP Profile Template"

/**
 * \brief
 * Template name defines for egress acceptable frame type profile.
 * \see
 * dnx_algo_port_tpid_init
 */
#define DNX_ALGO_EGRESS_ACCEPTABLE_FRAME_TYPE_PROFILE_TEMPLATE "Egree Acceptable Frame Type Profile Template"

/**
 * \brief 
 * Resource allocation name defines for algo Port TPID TAG-Struct.
 * \see
 * dnx_algo_port_tpid_init
 */
#define DNX_ALGO_PORT_TPID_SW_TAG_STRUCT_INGRESS    "SW TAG Struct Ingress resource manager"
#define DNX_ALGO_PORT_TPID_SW_TAG_STRUCT_EGRESS     "SW TAG Struct Egress resource manager"
#define DNX_ALGO_PORT_TPID_SW_TAG_STRUCT_SYMMETRIC  "SW TAG Struct Symmetric resource manager"
#define DNX_ALGO_PORT_TPID_HW_TAG_STRUCT_INGRESS    "HW TAG Struct Ingress resource manager"
#define DNX_ALGO_PORT_TPID_HW_TAG_STRUCT_EGRESS     "HW TAG Struct Egress resource manager"

/** 
 * \brief 
 *  
 * Create the advanced algorithm instance - PORT TPID HW Tag-Struct. 
 *  
 * \ref 
 *  resource_tag_bitmap_create
 */
shr_error_e dnx_port_tpid_tag_struct_hw_resource_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

/** 
 * \brief 
 * Allocation function for the advanced algorithm - PORT TPID HW Tag-Struct. 
 *  
 * \see 
 * dnx_algo_res_allocate_cb 
 */
shr_error_e dnx_port_tpid_tag_struct_hw_resource_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
 * \brief - 
 *   Intialize Port TPID algorithms.
 *   The function creates:
 *   - PORT TPID Template profiles for both ingress and egress.
 *   - PORT TPID TAG-Struct resource allocation for ingress, egress and symmtric.
 * 
 * \param [in] unit - 
 *      Relevant unit.
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *  Please refer to DNX Init/Deinit sequence.
 */
shr_error_e dnx_algo_port_tpid_init(
    int unit);

/**
 * \brief - 
 *   Deintialize port TPID algorithm.
 *   The function destroys:
 *   - PORT TPID Template profiles for both ingress and egress.
 *   - PORT TPID TAG-Struct resource allocation for ingress, egress and symmtric.
 * \param [in] unit - 
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *  Please refer to DNX Init/Deinit sequence.
 */
shr_error_e dnx_algo_port_tpid_deinit(
    int unit);

/**
 * }
 */
/**
 * \brief - 
 * Function for printing ingress Port PP Template: 
 * Per data entry in the template which is accessed by the key 
 * {is_prio, outer_tpid, inner_tpid} it would print that LLVP 
 * data. 
 * 
 * \param [in] unit - relevant unit.
 * \param [in] data - pointer to ingress Port PP TPID Template 
 * \return
 * \remark
 *   * None
 * \see
 * algo_port_tpid_print_ingress
 * shr_error_e
 */
void dnx_algo_port_tpid_print_ingress_cb(
    int unit,
    const void *data);

/**
 * \brief - 
 * Function for printing egress Port PP Tempalte: 
 * Per each data entry in the template which is accessed by the 
 * key {outer_tpid, is_prio, inner_tpid} it would print that 
 * LLVP data. 
 * 
 * \param [in] unit - relevant unit.
 * \param [in] data - pointer to egress Port PP TPID Template 
 * \return
 * \remark
 *   * None
 * \see 
 * algo_port_tpid_print_ingress
 * shr_error_e
 */
void dnx_algo_port_tpid_print_egress_cb(
    int unit,
    const void *data);

/**
 * \brief -
 *   Intialize Port PP TPID algorithms.
 *   The function creats PORT TPID Template profiles for both
 *   ingress and egress.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * dnx_algo_port_pp_init
 */
shr_error_e algo_port_tpid_init(
    int unit);

/*
 */
shr_error_e algo_port_tpid_deinit(
    int unit);

/*
 * } 
 */
#endif /* ALGO_PORT_TPID_H_INCLUDED */
