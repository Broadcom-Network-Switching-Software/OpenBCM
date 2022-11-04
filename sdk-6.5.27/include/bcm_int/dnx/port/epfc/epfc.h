/** \file epfc.h
 * 
 *
 * Internal DNX Encrypted PFC APIs invoked from other modules
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _PFC_H_INCLUDED_
/** { */
#define _PFC_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/**
 * \brief - Get Encrypted PFC statistics
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] type - Stat type
 * \param [out] value - Stat value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_port_encrypted_pfc_tx_stat_get(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *value);

/**
 * \brief - Clear Encrypted PFC statistics
 *
 * \param [in] unit - Relevant unit
 * \param [in] port - Port
 * \param [in] type - Stat type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_port_encrypted_pfc_tx_stat_clear(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type);

/**
 * \brief - Enable / disable encrypted PFC frame transmission
 *
 * \param [in] unit - Relevant unit
 * \param [in] arg - Enable / disable
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int bcm_dnx_port_encrypted_pfc_tx_enable_set(
    int unit,
    int arg);

int bcm_dnx_port_encrypted_pfc_tx_enable_get(
    int unit,
    int *arg);

#endif /** _PFC_H_INCLUDED_ */
