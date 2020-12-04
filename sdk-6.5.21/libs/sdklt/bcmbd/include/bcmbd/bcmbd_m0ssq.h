/*! \file bcmbd_m0ssq.h
 *
 *  APIs for M0SSQ (ARM Cortex-M0 based Sub-System Quad).
 *
 *      M0SSQ contains four ARM Cortex-M0 uCs and a shared
 *  SRAM.
 *      User can use API in this file to
 *          - Download M0 firmware.
 *          - Start/stop M0 uc running.
 *          - Install/uninstall software interrupt handler of M0 uCs.
 *          - Enable/Disable software interrupt of M0 uCs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_M0SSQ_H
#define BCMBD_M0SSQ_H

#include <sal/sal_types.h>
#include <bcmbd/bcmbd_intr.h>

/*******************************************************************************
 * BCMBD_M0SSQ public APIs.
 */
/*!
 * \brief Load firmware into a uC of M0SSQ.
 *
 * This function loads firmware into uC memory.
 *
 * \param [in] unit Unit number.
 * \param [in] uc uC controller number.
 * \param [in] offset Byte offset of uC memory.
 * \param [in] buf Firmware buffer.
 * \param [in] len Length of firmware buffer.
 *
 * \retval SHR_E_NONE Firmware successfully loaded.
 * \retval SHR_E_FAIL Failed to load firmware.
 */
extern int
bcmbd_m0ssq_uc_fw_load(int unit, uint32_t uc, uint32_t offset,
                       const uint8_t *buf, uint32_t len);

/*!
 * \brief Get number of uC in M0SSQ.
 *
 * \param [in] unit Unit number.
 *
 * \retval Number of uC.
 */
extern int
bcmbd_m0ssq_uc_num_get(int unit);

/*!
 * \brief Start a M0SSQ uC.
 *
 * \param [in]  unit Unit number.
 * \param [in]  uc uC number.
 *
 * \retval SHR_E_NONE Start a uC successfully.
 * \retval SHR_E_FAIL Fail to start a uC.
 */
extern int
bcmbd_m0ssq_uc_start(int unit, uint32_t uc);

/*!
 * \brief Stop a M0SSQ uC.
 *
 * \param [in]  unit Unit number.
 * \param [in]  uc uC number.
 *
 * \retval SHR_E_NONE Stop a uC successfully.
 * \retval SHR_E_FAIL Fail to stop a uC.
 */
extern int
bcmbd_m0ssq_uc_stop(int unit, uint32_t uc);

/*!
 * \brief Check if uC is started or not.
 *
 * \param [in]  unit Unit number.
 * \param [in]  uc uC number.
 * \param [out] start Indicate uC is started or not.
 *
 * \return SHR_E_NONE if successful.
 */
int
bcmbd_m0ssq_uc_start_get(int unit,
                         uint32_t uc,
                         bool *start);

/*!
 * \brief Install uC interrupt hanlder.
 *
 * This function is used to install uC's software interrupt
 * handler.
 *
 * \param [in] unit Unit number.
 * \param [in] uc uC number.
 * \param [in] handler Pointer of handler.
 *                     NULL : uninstall uC interrupt handler.
 * \param [in] param User defined parameter of interrupt handler.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Interriupt handler setup failed.
 */
extern int
bcmbd_m0ssq_uc_swintr_handler_set(int unit, uint32_t uc,
                                  bcmbd_intr_f handler,
                                  uint32_t param);
/*!
 * \brief uC interrupt enable/disable.
 *
 * This function is used to enable/disable uC software
 * interrupt.
 *
 * \param [in] unit Unit number.
 * \param [in] uc uC number.
 * \param [in] enable !0 to enable interrupt.
 *                    0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Interriupt handler setup failed.
 */
extern int
bcmbd_m0ssq_uc_swintr_enable_set(int unit,
                                 uint32_t uc,
                                 bool enable);

#endif /* BCMBD_M0SSQ_H */
