/*! \file bcm56990_a0_pc_internal.h
 *
 * BCM56990_A0 chip specific functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56990_A0_PC_INTERNAL_H
#define BCM56990_A0_PC_INTERNAL_H

#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_pm_drv_internal.h>

/*!
 * \brief Execute driver operation.
 *
 * This function is used to download the PHY firmware.
 *
 * \param [in] unit Unit number.
 * \param [in] port physical port.
 * \param [in] fw_loader_req Request firmware download function.
 * \param [out] fw_loader firmware loader function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56990_a0_phy_firmware_loader(int unit, int port, uint32_t fw_loader_req,
                                phymod_firmware_loader_f *fw_loader);

/*!
 * \brief Execute driver operation.
 *
 * This function is used to dispatch the operation \c to the corresponding
 * driver \c drv.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] op Operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm56990_a0_pc_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op);


/*!
 * \brief Get the PM driver instance for PM8x50.
 *
 * \param [in] unit Unit number.
 *
 * \return The pointer of PM8x50 driver.
 */
extern bcmpc_pm_drv_t *
bcm56990_a0_pm_drv_pm8x50_get(int unit);

#endif /* BCM56990_A0_PC_INTERNAL_H */

