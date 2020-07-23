/*! \file bcmpc_sec_internal.h
 *
 * BCMPC SEC (Security) internal APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_SEC_INTERNAL_H
#define BCMPC_SEC_INTERNAL_H

#include <sal/sal_types.h>
#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_tm.h>
#include <bcmpc/bcmpc_sec.h>

/*!
 * \brief Get if SEC is valid on device.
 *
 * \param [in] unit Unit number.
 *
 * \retval 1 - valid, 0 - invalid
 */
extern int
bcmpc_sec_valid_get (int unit);

/*!
 * \brief Check if SEC is valid on PM.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id Unit number.
 *
 * \retval 1 - valid, 0 - invalid
 */
extern int
bcmpc_pm_sec_enabled(int unit, int pm_id);

/*!
 * \brief Execute the SEC operation.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] op SEC operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_sec_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op);

/*!
 * \brief Create mutex for PC -> SEC interactions.
 *
 * This function will be called during component config stage.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_pc_sec_mutex_init(int unit);

/*!
 * \brief Destroy mutex created for PC -> SEC interactions.
 *
 * This function will be called during component stop stage.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmpc_pc_sec_mutex_cleanup(int unit);

/*!
 * \brief Copy relevant port information from SEC.
 *
 * \param [in] num_ports Number of physical ports in configuration array.
 * \param [in] cfg Port configuration array.
 * \param [in] mmu_pcfg MMU port configuration array.
 */
extern void
tm_sec_port_cfg_init(size_t num_ports, bcmpc_sec_port_cfg_t *cfg,
                     bcmpc_mmu_port_cfg_t *mmu_pcfg);

/*!
 * \brief Update SEC configuration.
 *
 * Update SEC configuration via the callbacks of bcmpc_sec_handler_t.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of physical ports in configuration arrays.
 * \param [in] sec_pcfg_old Current port configuration.
 * \param [in] sec_pcfg_new New/requested port configuration.
 *
 * \retval SHR_E_NONE No errors
 * \retval SHR_E_FAIL Fail to register the handler.
 */
extern int
pc_sec_update(int unit, int num_ports,
              bcmpc_sec_port_cfg_t *sec_pcfg_old,
              bcmpc_sec_port_cfg_t *sec_pcfg_new);
#endif /* BCMPC_SEC_INTERNAL_H */
