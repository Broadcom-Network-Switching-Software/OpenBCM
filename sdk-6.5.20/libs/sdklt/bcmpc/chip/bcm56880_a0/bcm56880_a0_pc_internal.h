/*! \file bcm56880_a0_pc_internal.h
 *
 * BCM56880 chip specific functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56880_A0_PC_INTERNAL_H
#define BCM56880_A0_PC_INTERNAL_H

#include <bcmpc/bcmpc_types_internal.h>

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
bcm56880_a0_phy_firmware_loader(int unit, int port, uint32_t fw_loader_req,
                                phymod_firmware_loader_f *fw_loader);

#endif /* BCM56880_A0_PC_INTERNAL_H */

