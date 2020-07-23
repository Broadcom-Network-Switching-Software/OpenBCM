/** \file algo_rx.h
 * 
 * Internal DNX resource manager APIs for Rx module:
 *  Traps - ingress, engress
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef ALGO_RX_H_INCLUDED
/*
 * { 
 */
#define ALGO_RX_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */

#include <shared/shrextend/shrextend_error.h>
#include <bcm/rx.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/*
 * }
 */

/**
 * \brief
 *   Print a recycle command template entry.
 * \par DIRECT INPUT
 *   \param [in] unit -the unit number
 *   \param [in] data -Pointer to data \n
 *       \b As \b input - \n
 *       pointer to data of type dnx_rx_trap_recycle_cmd_config_t which
 *       includes properties of recycle command
 * \par INDIRECT INPUT
 *   None
 * \par DIRECT OUTPUT
 *   None
 * \par INDIRECT OUTPUT
 *    the printout
 * \see
 *   * None
 */
void dnx_algo_rx_trap_recycle_cmd_print_entry_cb(
    int unit,
    const void *data);

/**
 * \brief
 *   Print the trap gport (encoding trap_id, fwd_strength, snp_strength) template entry.
 * \par DIRECT INPUT
 *   \param [in] unit -the unit number
 *   \param [in] data -Pointer to data \n
 *       \b As \b input - \n
 *       pointer to data of type dnx_rx_trap_erpp_config_t which
 *       includes properties of erpp action profile
 * \par INDIRECT INPUT
 *   None
 * \par DIRECT OUTPUT
 *   None
 * \par INDIRECT OUTPUT
 *    the printout
 * \see
 *   * None
 */
void dnx_algo_rx_trap_gport_print_entry_cb(
    int unit,
    const void *data);

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_PREDEFINED pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ingress predefined trap_id newly allocated
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_ingress_predefined_allocate(
    int unit,
    int *trap_id_p);

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_USER_DEFINED pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] alloc_flags  -  User define trap can be allocated BCM_RX_TRAP_WITH_ID
*   \param [in] alloc_id_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ingress user_defined trap_id newly allocated
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_ingress_user_define_allocate(
    int unit,
    int alloc_flags,
    int *alloc_id_p);

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ERPP pool
* \par DIRECT INPUT: 
*   \param [in] unit     - Unit id
*   \param [in] flags    - Flags
*   \param [in] trap_id_p  - Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ERPP trap_id
*           newly allocaed
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_erpp_allocate(
    int unit,
    int flags,
    int *trap_id_p);

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_ pool
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit id
*   \param [in] flags     - Flags(WITH_ID)
*   \param [in] type     -  bcmRxTrapTypeEgTx.. to allocate
*   \param [out] trap_id_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ETPP trap_id newly allocaed
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_etpp_allocate(
    int unit,
    int flags,
    bcm_rx_trap_t type,
    int *trap_id_p);

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_OAM pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ETPP OAM trap_id newly allocated
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_etpp_oam_allocate(
    int unit,
    int *trap_id_p);

/**
* \brief
*   Deallocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_PREDEFINED pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_ingress_predefined_deallocate(
    int unit,
    int trap_id);

/**
* \brief  
*   Deallocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_USER_DEFINED pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_ingress_user_define_deallocate(
    int unit,
    int trap_id);

/**
* \brief  
*   Deallocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ERPP pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_erpp_deallocate(
    int unit,
    int trap_id);

/**
* \brief  
*   Deallocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_PREDEFINED pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_etpp_deallocate(
    int unit,
    int trap_id);

/**
* \brief  
*   Deallocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_OAM pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_etpp_oam_deallocate(
    int unit,
    int trap_id);

/**
* \brief
*   Check whether specific ingress pre-defined trap was already allocated 
*   in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_PREDEFINED
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit ID
*   \param [in] trap_id         -  Pre-defined trap id to check
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if the ingress predefined trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_ingress_predefined_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p);

/**
* \brief  
*   Check whether specific ingress user define trap was already allocated
*   in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_USER_DEFINED
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit Id
*   \param [in] trap_id         -  User define trap id to check
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if ingress user_define trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_ingress_user_define_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p);

/**
* \brief  
*   Check whether specific erpp trap was already allocated
*   in DNX_ALGO_RX_TRAP_RES_MNGR_ERPP
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit id
*   \param [in] trap_id         -  Id of erpp trap
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if the erpp trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_erpp_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p);

/**
* \brief  
*   Check whether specific erpp trap was already allocated
*   in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_PREDEFINED
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit id
*   \param [in] trap_id         -  Id of etpp trap
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if the etpp trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_etpp_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p);

/**
* \brief  
*   Check whether specific etpp trap was already allocated
*   in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_OAM
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit id
*   \param [in] trap_id         -  Id of etpp oam trap
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if the etpp oam trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_id_etpp_oam_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p);

/**
* \brief
*   Check if the ICMP compression type is allocated.
* \par DIRECT INPUT:
*   \param [in] unit            -  Unit id
*   \param [in] compressed_type -  ICMP compressed type
*   \param [out] is_allocated_p -  Is allocated
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_icmp_compressed_type_is_allocated(
    int unit,
    int compressed_type,
    uint8 *is_allocated_p);

/**
* \brief
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_PROTOCOL_ICMP_TYPE_MAP pool
* \par DIRECT INPUT:
*   \param [in] unit                - Unit id
*   \param [in] alloc_flags         - Allocation flags
*   \param [out] compressed_type_p  - Compressed ICMP Type
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_icmp_compressed_type_allocate(
    int unit,
    int alloc_flags,
    int *compressed_type_p);

/**
* \brief
*   Deallocate ICMP compression type
* \par DIRECT INPUT:
*   \param [in] unit            -  Unit id
*   \param [in] compressed_type -  ICMP compressed type
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_algo_rx_trap_icmp_compressed_type_deallocate(
    int unit,
    int compressed_type);

/**
 * \brief - Initialize rx algorithms
 *  Resource managers for traps - ingress
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * rx resource manager
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_rx_init(
    int unit);

/*
 * MACROs
 * {
 */

/**
 * \brief - res mngr for ingress traps - predefined traps
 */
#define DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_PREDEFINED            "TRAP_INGRESS_PREDEFINED"
/**
 * \brief - res mngr for ingress traps - user_define traps 
 */
#define DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_USER_DEFINED          "TRAP_INGRESS_USER_DEFINED"
/**
 * \brief - res mngr for ERPP traps
 */
#define DNX_ALGO_RX_TRAP_RES_MNGR_ERPP                          "TRAP_ERPP"
/**
 * \brief - res mngr for ETPP traps - predefined traps
 */
#define DNX_ALGO_RX_TRAP_RES_MNGR_ETPP                          "TRAP_ETPP"
/**
 * \brief - res mngr for ETPP traps - OAM traps
 */
#define DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_OAM                      "TRAP_ETPP_OAM"
/**
 * \brief - template manager for Protocol(ICMP) type map
 */
#define DNX_ALGO_RX_TRAP_RES_MNGR_PROTOCOL_ICMP_TYPE_MAP        "TRAP_PROTOCOL_ICMP_MAP"
/**
 * \brief - template manager for recycle command
 */
#define DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_RECYCLE_CMD              "TRAP_RECYCLE_CMD"
/**
 * \brief - template mngr for action profile of ERPP traps                  
 */
#define DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_ERPP_ACTION_PROFILE      "TRAP_ERPP_ACTION_PROFILE"

/**
 * \brief - template mngr for action profile of ETPP traps                  
 */
#define DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_ETPP_ACTION_PROFILE      "TRAP_ETPP_ACTION_PROFILE"

/**
 * \brief - template manager for ingress lif traps
 */
#define DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_INGRESS_LIF              "TRAP_INGRESS_LIF"
/**
 * \brief - template manager for ETPP lif traps
 */
#define DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_ETPP_LIF                 "TRAP_ETPP_LIF"

/*
 * }
 */

/*
 * } 
 */
#endif/*_ALGO_MIRROR_INCLUDED__*/
