/** \file bcm_int/dnx/cosq/flow_control/pfc_deadlock.h
 * 
 *
 * PFC Deadlock functions for DNX. \n
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * $Copyright:.$
 */

#ifndef _SRC_DNX_PFC_DEADLOCK_CB_H_INCLUDED_
/** { */
#define _SRC_DNX_PFC_DEADLOCK_CB_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>

/*
 * Typedefs
 * {
 */

/**
 * \brief - Struct that holds the needed data for PFC deadlock recovery
 * periodic event callback function
 */
typedef struct pfc_deadlock_recovery_userdata
{
    int phy; /** Phy that triggered the interrupt */
    int pfc_num;  /** PFC that triggered the interrupt */
    bcm_port_t master_logical_port; /** Master logical port of Phy that triggered the interrupt */
} pfc_deadlock_recovery_userdata_t;

 /*
  * }
  */

/*
 * Functions
 * {
 */

/**
 * \brief - Corrective action callback to the PFC Deadlock interrupt
 *
 * \param [in] unit - relevant unit
 * \param [in] block_instance - instance of the block that
 *             triggered the interrupt
 * \param [in] is_cdu - indication if the block is CDU or CDUM/CLU
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 */
int dnx_interrupt_handles_corrective_action_for_pfc_deadlock(
    int unit,
    int block_instance,
    uint8 *is_cdu);

/**
 * \brief - function to set the uused defined callback for PFC Deadlock recovery
 *
 * \param [in] unit - relevant unit
 * \param [in] callback - user defined callback
 * \param [in] userdata - user data used as input to the callback
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 */
shr_error_e dnx_pfc_deadlock_recovery_event_cb_set(
    int unit,
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback,
    void *userdata);

shr_error_e dnx_pfc_deadlock_init(
    int unit);
shr_error_e dnx_pfc_deadlock_deinit(
    int unit);

 /*
  * }
  */

#endif /** _SRC_DNX_PFC_DEADLOCK_CB_H_INCLUDED_ */
