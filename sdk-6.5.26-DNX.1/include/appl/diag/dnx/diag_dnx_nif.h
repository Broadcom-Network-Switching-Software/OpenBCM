/** \file diag_dnx_nif.h
 *
 * diagnostics for NIF
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_NIF_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_NIF_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/**
 * \brief - Return the rx fifo status for given port and priority
 *
 * \param [in] unit - chip unit id
 * \param [in] logical_port - logical port
 * \param [in] sch_prio - nif schedule priority
 * \param [out] max_occupancy - indicate the MAX fullness level of the fifo
 * \param [out] fifo_level - indicate the current fullness level of the fifo
 * \param [out] mapped_port - indicate if port is mapped to a counter(only for relevant devices)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int sh_dnx_nif_rx_fifo_status_get(
    int unit,
    bcm_port_t logical_port,
    uint32 sch_prio,
    uint32 *max_occupancy,
    uint32 *fifo_level,
    uint32 *mapped_port);

#endif /** DIAG_DNX_NIF_H_INCLUDED */
