/*! \file bcmpc_tm_internal.h
 *
 * BCMPC TM (Traffic Manager) internal APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_TM_INTERNAL_H
#define BCMPC_TM_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>

/*!
 * \brief Enable/Disable the port-based TM update mode.
 *
 * Enable to update TM for each port addition/deletion.
 *
 * \param [in] unit Unit number.
 * \param [in] enable Enable the port-based update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Unit not found.
 */
extern int
bcmpc_tm_port_based_update_set(int unit, bool enable);

/*!
 * \brief Get the current TM update callback mode.
 *
 * Get the enable state of the port-based TM update.
 *
 * \param [in] unit Unit number.
 * \param [out] enable Set if enable port-based update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Unit not found.
 */
extern int
bcmpc_tm_port_based_update_get(int unit, bool *enable);

/*!
 * \brief Update TM for port addition.
 *
 * This function will be called when we insert a new logical port.
 *
 * In automatic port-based TM update mode, PC will update TM via the
 * callbacks of bcmpc_tm_handler_t for the port addition/deletion.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_tm_port_add(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Update TM for port deletion.
 *
 * This function will be called when we delete a logical port.
 *
 * In automatic port-based TM update mode, PC will update TM via the
 * callbacks of bcmpc_tm_handler_t for the port addition/deletion.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_tm_port_delete(int unit, bcmpc_lport_t lport);

/*!
 * \brief Execute the TM operation.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] op TM operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_tm_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op);

/*!
 * \brief Notify TM for port configuration updates.
 *
 * This function will be called when a logical port
 * speed or encapsulation configuration is changed.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] old_pcfg Old port configuration.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_tm_port_update(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *old_pcfg,
                     bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Create mutex for PC -> TM interactions.
 *
 * This function will be called during component config stage.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pc_tm_mutex_init(int unit);

/*!
 * \brief Destroy mutex created for PC -> TM interactions.
 *
 * This function will be called during component stop stage.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmpc_pc_tm_mutex_cleanup(int unit);

#endif /* BCMPC_TM_INTERNAL_H */

