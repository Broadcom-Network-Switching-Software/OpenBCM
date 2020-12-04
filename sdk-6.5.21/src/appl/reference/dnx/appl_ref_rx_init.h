/** \file appl_ref_rx_init.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef APPL_REF_RX_INIT_H_INCLUDED
#define APPL_REF_RX_INIT_H_INCLUDED

#include <shared/shrextend/shrextend_error.h>

/**
 * \brief
 * This function initialize and active CPU RX application.
 *
 *\param [in] unit - Relevant unit.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_rx_init(
    int unit);

/**
 * \brief
 * Disable CPU RX.
 * 1. Shutdown RX thread and free all related resource
 * 2. Free RX pool
 *
 *\param [in] unit - Relevant unit.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_rx_deinit(
    int unit);

#endif /* APPL_REF_RX_INIT_H_INCLUDED */
