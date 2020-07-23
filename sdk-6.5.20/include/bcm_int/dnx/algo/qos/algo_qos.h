/**
 * \file algo_qos.h Internal DNX L3 Managment APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_QOS_H_INCLUDED
/*
 * { 
 */
#define ALGO_QOS_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * Resource name defines for algo qos 
 * {
 */

#define DNX_QOS_INGRESS_RESOURCE "INGRESS QOS"
#define DNX_QOS_INGRESS_OPCODE_RESOURCE "INGRESS QOS OPCODE"
#define DNX_QOS_INGRESS_ECN_RESOURCE "INGRESS QOS ECN"
#define DNX_QOS_INGRESS_POLICER_RESOURCE "INGRESS QOS POLICER"
#define DNX_QOS_INGRESS_VLAN_PCP_RESOURCE "INGRESS QOS VLAN PCP"
#define DNX_QOS_EGRESS_RESOURCE "EGRESS QOS"
#define DNX_QOS_EGRESS_MPLS_PHP_RESOURCE "EGRESS QOS MPLS PHP"
#define DNX_QOS_EGRESS_OPCODE_RESOURCE "EGRESS QOS OPCODE"
#define DNX_QOS_EGRESS_PHP_OPCODE_RESOURCE "EGRESS QOS PHP OPCODE"
#define DNX_QOS_EGRESS_ECN_RESOURCE "EGRESS QOS ECN"
#define DNX_QOS_EGRESS_POLICER_RESOURCE "EGRESS QOS POLICER"
#define DNX_QOS_EGRESS_OAM_PCP_MAP_RESOURCE "EGRESS QOS OAM PRIORITY MAP"
#define DNX_QOS_EGRESS_COS_PROFILES_RESOURCE "EGRESS QOS COS PROFILE"

#define DNX_ALGO_QOS_EGRESS_TTL_PIPE_PROFILE_TEMPLATE "EGRESS QOS TTL PIPE PROFILE TEMPLATE"
#define DNX_ALGO_QOS_INGRESS_PROPAGATION_PROFILE_TEMPLATE "INGRESS PROPAGATION PROFILE TEMPLATE"
#define DNX_ALGO_QOS_EGRESS_NETWORK_QOS_PROFILE_TEMPLATE "EGRESS NETWORK_QOS PROFILE TEMPLATE"
/**
 * }
 */
/**
 * \brief 
 *   allocate resource pool for all QOS profiles types
 * \param [in] unit - 
 *   The unit number. 
 * \return  
 *   \retval Negative in case of an error. 
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_algo_qos_init(
    int unit);

/**
* \brief
* Print an entry of qos propagation profile template. 
* \param [in] unit - the unit number 
* \param [in] data - Pointer of the struct to be printed.
* \return 
*     None 
 
*/
void dnx_algo_qos_propagation_profile_template_print_cb(
    int unit,
    const void *data);

/**
* \brief
*   Print an entry of egress ttl pipe profile template. 
* \param [in] unit - the unit number 
* \param [in] data - Pointer of the struct to be printed.
* \return 
*     None 
 */
void dnx_algo_qos_egress_ttl_pipe_profile_template_print_cb(
    int unit,
    const void *data);

/**
* \brief
*   Print an entry of egress network_qos profile template. 
* \param [in] unit - the unit number 
* \param [in] data - Pointer of the struct to be printed.
* \return 
*     None 
 */
void dnx_algo_qos_egress_network_qos_profile_template_print_cb(
    int unit,
    const void *data);

/*
 * } 
 */
#endif/*_ALGO_QOS_API_INCLUDED__*/
