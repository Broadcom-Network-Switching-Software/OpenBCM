/**
 * \file algo_mpls.h Internal DNX MPLS Managment APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef ALGO_MPLS_H_INCLUDED
/*
 * { 
 */
#define ALGO_MPLS_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * Resource name defines for algo mpls 
 * {
 */

#define DNX_MPLS_TERMINATION_PROFILE_TEMPLATE "TERMINATION PROFILE"

#define DNX_MPLS_PUSH_PROFILE_TEMPLATE        "PUSH PROFILE"

/**
 * }
 */

typedef struct
{
    /*
     * indicates the ttl model, whether set according to push profile or copy from previous header 
     */
    uint8 ttl_model;
    /*
     * indicates the exp model, whether set according to push profile or copy from previous header 
     */
    uint8 exp_model;
    /*
     *  Has Control-word. Used to determine whether to add CW
     *  under the tunnel header. The value of CW is set
     *  according to soc_ppd_eg_encap_glbl_info_set()
     */
    uint8 has_cw;
    /*
     *  TTL to set in the tunnel header in case tunnel model is
     *  pipe. Range: 0 - 255.
     */
    uint8 ttl;
    /*
     *  EXP to set in the pushed label header in case tunnel
     *  model is pipe.
     */
    uint8 exp;
    /*
     *  Select 1 of 16 Remark profiles.
     */
    uint8 remark_profile;
    /*
     *  If set, Entropy label will be added. 
     */
    uint8 add_entropy_label;
    /*
     *  If set, Entropy label indicator will be added. 
     */
    uint8 add_entropy_label_indicator;
    /*
     *  ESEM extraction command.
     */
    int esem_command_profile;
} dnx_jr_mode_mpls_push_profile_t;
/**
 * \brief 
 *   allocate resource pool for all MPLS profiles types
 * \param [in] unit - 
 *   The unit number. 
 * \return  
 *   \retval Negative in case of an error. 
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_algo_mpls_init(
    int unit);

/**
* \brief
*   Print an entry of the enablers vector template. See
*       \ref dnx_algo_template_print_data_cb for more details.
* \param [in] unit - the unit number 
* \param [in] data - Pointer of the struct to be printed.
* \return 
*     None 
 */
void dnx_algo_mpls_termination_profile_template_print_cb(
    int unit,
    const void *data);

/**
* \brief
*   Print an entry of the push profile template. See
*       \ref dnx_algo_template_print_data_cb for more details.
* \param [in] unit - the unit number 
* \param [in] data - Pointer of the struct to be printed.
* \return 
*     None 
 */

void dnx_algo_mpls_push_profile_template_print_cb(
    int unit,
    const void *data);
/*
 * } 
 */
#endif/*_ALGO_MPLS_API_INCLUDED__*/
