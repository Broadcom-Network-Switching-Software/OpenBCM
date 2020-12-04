/** \file bcm_int/dnx/switch/switch.h
 *
 * Internal DNX SWITCH APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SWITCH_H_INCLUDED
/* { */
#define SWITCH_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

/*
 * }
 */

/*
 * DEFINES
 * {
 */
/** Port header type index for both directions - IN and OUT */
#define DNX_SWITCH_PORT_HEADER_TYPE_INDEX_BOTH 0
/** Port header type index IN */
#define DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN 1
/** Port header type index OUT */
#define DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT 2

#define DNX_SWITCH_INTR_BLOCK_MAX_NUM       2048
#define DNX_SWITCH_INTR_BLOCK_WIDTH         BITS2WORDS(DNX_SWITCH_INTR_BLOCK_MAX_NUM)

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * This MACRO is used for running verification function only if their verification mode is enable.
 * unit                     -> The unit number
 * _expr                    -> The verification function with all the required values.
 * _module_verification_id  -> the bcm_switch_module_verify_t value of the calling module.
 */
#define DNX_SWITCH_VERIFY_IF_ENABLE_DNX(_unit,_expr,_module_verification_id)                    \
do                                                                                              \
{                                                                                               \
  uint8 enable;                                                                                 \
  SHR_IF_ERR_EXIT(switch_db.module_verification.get(_unit, _module_verification_id, &enable));  \
  if (enable)                                                                                   \
  {                                                                                             \
       SHR_INVOKE_VERIFY_DNX(_expr);                                                            \
  }                                                                                             \
} while (0)

/*
 * }
 */

/**
 * \brief
 * Verify network_group_id range
*/
shr_error_e dnx_switch_network_group_id_verify(
    int unit,
    int is_ingress,
    bcm_switch_network_group_t network_group_id);

/**
 * \brief - Get routed learning mode for supported applications according to input flags
 *
 * \param [in] unit - unit Id
 * \param [out] arg - output flags (BCM_SWITCH_CONTROL_L3_LEARN_XXX)
 *
 * \return
 *   shr_error_e - Error type
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_switch_control_routed_learn_get(
    int unit,
    int *arg);

/**
 * \brief - Retrieve the switch header type according to port and port direction
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] direction - Port direction
 * \param [out] switch_header_type - Switch Header Type
 *
 * Port direction can be 0, 1 and 2. 0 means both directions (in and out), 1 means 'in' and 2 means 'out'.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_switch_header_type_get(
    int unit,
    bcm_port_t port,
    int direction,
    int *switch_header_type);

/**
 * \brief - convert BCM header types to DNX header types
 *
 * \param [in] unit - unit Id
 * \param [in] bcm_header_type - BCM header type (BCM_SWITCH_PORT_HEADER_TYPE_...)
 * \param [out] dnx_header_type - DNX header type (DNX_ALGO_PORT_HEADER_MODE_...)
 *
 * \return
 *   shr_error_e - Error type
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_switch_bcm_to_dnx_header_type_convert(
    int unit,
    int bcm_header_type,
    dnx_algo_port_header_mode_e * dnx_header_type);

/**
 * \brief - Get the prt profile and the switch header type of in port
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [out] switch_header_type - Switch port header type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_switch_header_type_in_get(
    int unit,
    bcm_port_t port,
    int *switch_header_type);
/**
 * \brief - initialize the switch module
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_switch_init(
    int unit);

/**
 * \brief - initialize the switch PP module.
 * \param [in] unit -  Unit-ID
 * \return
 *   shr_error_e
 * \remarks
 *   * None
 */
int dnx_switch_pp_init(
    int unit);

/**
 * \brief - Return invalid destination information in case that invalid destination interrupr occurs
 *
 * \param [in] unit - Relevant unit
 * \param [in] block_instance - Block instance id
 * \param [in] msg - Message that will be printed in case that invalid destination interrupt occurs
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_switch_interrupt_handles_corrective_action_invalid_destination(
    int unit,
    int block_instance,
    char *msg);
/* } */
#endif /* SWITCH_H_INCLUDED */
