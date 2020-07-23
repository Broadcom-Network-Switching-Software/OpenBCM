/*! \file bcmcth_port_system_table_commit.h
 *
 * This file contains port system
 * related data structures and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_PORT_SYSTEM_TABLE_COMMIT_H
#define BCMCTH_PORT_SYSTEM_TABLE_COMMIT_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * PORT_SYSTEM_ID - shared data between PORT_SYSTEM_ID
 * transform and operation mode transform.
 */
extern uint32_t bcmcth_port_system_id;

/*!
 * \brief Port_system id prepare
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_port_system_table_prepare(int unit,
                                 uint32_t trans_id,
                                 const bcmltd_generic_arg_t *arg);


/*!
 * \brief Port_system id commit
 *
 * Discard the backup state and copy current state
 * to backup state as the current
 * transaction is getting committed.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_port_system_table_commit(int unit,
                                uint32_t trans_id,
                                const bcmltd_generic_arg_t *arg);

/*!
 * \brief Port system id abort
 *
 * Rollback the current state to backup state
 * as the current transaction is getting aborted.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_port_system_table_abort(int unit,
                               uint32_t trans_id,
                               const bcmltd_generic_arg_t *arg);

#endif /* BCMCTH_PORT_SYSTEM_TABLE_COMMIT_H */
