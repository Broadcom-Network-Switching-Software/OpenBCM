/*! \file bcm56780_a0_sec.h
 *
 * File containing chip related defines and internal functions for bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_SEC_H
#define BCM56780_A0_SEC_H

#include <bcmsec/bcmsec_drv.h>
#include <bcmpc/bcmpc_sec.h>

/*! Number of physical ports 0 - 166.*/
#define TD4_X9_NUM_PHYS_PORTS 167

/*! Number of physical ports 0 - 166.*/
#define TD4_X9_NUM_SEC_BLK    6

/*! Number of Port Macros 0 - 19.*/
#define TD4_X9_NUM_PORT_MACRO 20

/*! Number of data path pipe per device. */
#define TD4_X9_PIPES_PER_DEV 4

/*! Number of device port per pipe. */
#define TD4_X9_DEV_PORTS_PER_PIPE 20

/*! Number of device port per device. */
#define TD4_X9_DEV_PORTS_PER_DEV (TD4_X9_DEV_PORTS_PER_PIPE * TD4_X9_PIPES_PER_DEV)

/*!
 * \brief SEC PC configuration
 *
 * Perform all port configuration dependent SEC configurations.
 *
 * This function is bound to the port control(PC) driver and will only be invoked by PC during
 * initialization or after a port update.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of ports in array.
 * \param [in] old_cfg Old MMU port config.
 * \param [in] new_cfg New MMU port config.
 *
 * \retval none
 * \retval SHR_E_PARAM Hardware table not found.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */
extern int
bcm56780_a0_sec_pc_configure(int unit,
                             size_t num_ports,
                             const bcmpc_sec_port_cfg_t *old_cfg,
                             const bcmpc_sec_port_cfg_t *new_cfg);

/*!
 * \brief Chip-specific initialization
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Hardware table not found or DPR frequency range check failed.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */
extern int
bcm56780_a0_sec_chip_init(int unit, bool warm);

/*!
 * \brief IMM Initialization functions for bcm56780_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid handler.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56780_a0_sec_imm_init(int unit);

/*!
 * \brief IMM pre-population functions for bcm56780_a0.
 *
 * \param [in] unit unit number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid LT field values.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56780_a0_sec_imm_populate(int unit);

/*!
 * \brief IMM update functions for bcm56780_a0.
 *
 * \param [in] unit unit number.
 * \param [in] pport Physical port.
 * \param [in] up Port is up/down.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid LT field values.
 * \retval SHR_E_NOT_FOUND lt not found.
 */
extern int
bcm56780_a0_sec_imm_update(int unit, int pport, int up);

/*!
 * \brief Flexport configuration validate.
 *
 * \param [in] unit unit number.
 * \param [in] num_ports Array depth.
 * \param [in] old_cfg Old MMU port config.
 * \param [in] new_cfg New MMU port config.
 *
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid configurations.
 */
extern int
bcm56780_a0_sec_validate(int unit, size_t num_ports,
                         const bcmpc_sec_port_cfg_t *old_cfg,
                         const bcmpc_sec_port_cfg_t *new_cfg);

/*!
 * \brief SEC TDM
 *
 * \param [in] unit unit number.
 *
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid configurations.
 */
int
bcm56780_a0_sec_tdm_init(int unit);

/*!
 * \brief Get SEC port from physical port.
 *
 * \param [in] unit unit number.
 * \param [in] pport Phyical port.
 * \param [out] pm Port Macro.
 * \param [out] sec_port SEC port as defined in regsfile.
 * \param [out] sec_local_port SEC port number within SEC block
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid configurations.
 */
int
bcm56780_a0_sec_pport_to_secport(int unit, bcmsec_pport_t pport,
                                 int *pm, int *sec_port,
                                 int *sec_local_port);
#endif /* BCM56780_A0_SEC_H */
