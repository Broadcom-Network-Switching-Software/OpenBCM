/*! \file bcmpc_imm_port_info.h
 *
 * Logical Table Custom Handlers for PC_PORT_INFO.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the logical table custom handlers for PC_PORT_INFO.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_IMM_PORT_INFO_H
#define BCMPC_IMM_PORT_INFO_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief PC_PORT_INFO table insert
 *
 * \param [in] unit Unit Number.
 * \param [in] port Logical port number.
 * \param [in] status Last operation state structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_imm_port_info_insert(int unit, bcmpc_lport_t port,
                           bcmpc_port_oper_status_t *status);

/*!
 * \brief PC_PORT_INFO table update
 *
 * \param [in] unit Unit Number.
 * \param [in] port Logical port number.
 * \param [in] status Last operation state structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_imm_port_info_update(int unit, bcmpc_lport_t port,
                           bcmpc_port_oper_status_t *status);

#endif /* BCMPC_IMM_PORT_INFO_H */
