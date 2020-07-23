/*! \file bcm56780_a0_sec_port.h
 *
 * File containing chip related defines and internal functions for bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_SEC_PORT_H
#define BCM56780_A0_SEC_PORT_H

#include <bcmsec/bcmsec_drv.h>

/*!
 * \brief SEC TX port up sequence.
 *
 * This function is bound to the port control(PC) driver and will
 * be invoked by PC during initialization or
 * after a port update.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 *
 * \retval none
 * \retval SHR_E_PARAM Hardware table not found.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */
int
bcm56780_a0_sec_port_tx_up(int unit, bcmsec_pport_t pport);

/*!
 * \brief SEC TX port down sequence.
 *
 * This function is bound to the port control(PC) driver and will
 * be invoked by PC during initialization or
 * after a port update.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 *
 * \retval none
 * \retval SHR_E_PARAM Hardware table not found.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */
int
bcm56780_a0_sec_port_tx_down(int unit, bcmsec_pport_t pport);

/*!
 * \brief SEC RX port down sequence.
 *
 * This function is bound to the port control(PC) driver and will
 * be invoked by PC during initialization or
 * after a port update.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 *
 * \retval none
 * \retval SHR_E_PARAM Hardware table not found.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */
int
bcm56780_a0_sec_port_rx_down(int unit, bcmsec_pport_t pport);

/*!
 * \brief SEC PM enable for Macsec
 *
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID
 * \param [in] pm_id Port Macro ID
 * \param [in] enable Enable/Disable
 * \param [out] opcode operation status
 *
 * \retval SHR_E_NONE
 * \retval SHR_E_PARAM Parameter error
 */
extern int
bcm56780_a0_sec_pm_enable(int unit, bcmltd_sid_t ltid,
                       uint32_t pm_id, uint32_t enable,
                       uint8_t *opcode);

/*!
 * \brief SEC port block init routine
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE
 * \retval SHR_E_PARAM Parameter error
 */
extern int
bcm56780_a0_sec_port_init(int unit);

/*!
 * \brief SEC port info get
 *
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] info Port info
 *
 * \retval SHR_E_NONE
 * \retval SHR_E_PARAM Parameter error
 */
extern int
bcm56780_a0_sec_port_info_get(int unit, int pport,
                              sec_pt_port_info_t *info);

/*!
 * \brief SEC egress port control set
 *
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID
 * \param [in] port_control Port control
 * \param [out] opcode Opcode
 *
 * \retval SHR_E_NONE
 * \retval SHR_E_PARAM Parameter error
 */
extern int
bcm56780_a0_sec_egr_port_control_set(int unit, bcmltd_sid_t ltid,
                               bcmsec_encrypt_port_control_t *port_control,
                               uint32_t *opcode);
/*!
 * \brief Decrypt port control set
 *
 * \param [in] unit unit number.
 * \param [in] ltid Logical table id
 * \param [in] pport Physical port
 * \param [in] in Field list
 * \param [out] opcode Opcode
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM invalid configurations.
 */
extern int
bcm56780_a0_decrypt_port_control_set(int unit, bcmltd_sid_t ltid,
                              int pport, bcmltd_field_t *in, uint32_t *opcode);

#endif /* BCM56780_A0_SEC_PORT_H */
