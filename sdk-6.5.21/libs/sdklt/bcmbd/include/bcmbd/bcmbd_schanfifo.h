/*! \file bcmbd_schanfifo.h
 *
 * SCHAN FIFO APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_SCHANFIFO_H
#define BCMBD_SCHANFIFO_H

#include <sal/sal_types.h>
#include <bcmdrd_config.h>

/*! Maximum number of SCHAN FIFO devices supported. */
#define SCHANFIFO_DEV_NUM_MAX           BCMDRD_CONFIG_MAX_UNITS

/*!
 * \name Initialization control flags.
 * \anchor SCHANFIFO_IF_xxx
 */

/*! \{ */
/*! Poll wait for done status, otherwise use interrupt. */
#define SCHANFIFO_IF_POLL_WAIT          (1 << 0)
/*! Mask the SER related events from resulting in channel abort. */
#define SCHANFIFO_IF_IGNORE_SER_ABORT   (1 << 1)
/*! Use CCMDMA to write commands, otherwise use PIO write. */
#define SCHANFIFO_IF_CCMDMA_WR          (1 << 2)
/*! \} */

/*!
 * \name Operation control flags.
 * \anchor SCHANFIFO_OF_xxx
 */

/*! \{ */
/*! Set start bit, only for ops_send(). */
#define SCHANFIFO_OF_SET_START          (1 << 0)
/*! Disable response, only for ops_send(). */
#define SCHANFIFO_OF_DIS_RESP           (1 << 1)
/*! Wait for HW to complete, only for status_get(). */
#define SCHANFIFO_OF_WAIT_COMPLETE      (1 << 2)
/*! Clear start bit, only for status_get(). */
#define SCHANFIFO_OF_CLEAR_START        (1 << 3)
/*! \} */

/*!
 * \brief Attach SCHAN FIFO driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_schanfifo_attach(int unit);

/*!
 * \brief Detach SCHAN FIFO driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_schanfifo_detach(int unit);

/*!
 * \brief Get SCHAN FIFO information.
 *
 * \param [in] unit Unit number.
 * \param [out] num_chans Number of channels.
 * \param [out] cmd_mem_wsize Command memory size in 32-bit words.
 *
 * This function should always be called first to check if SCHAN FIFO
 * is supported. The resources supported by hardware will be returned if
 * successful.
 *
 * \retval SHR_E_NONE, No errors.
 * \retval SHR_E_PARAM, Invalid parameters.
 * \retval SHR_E_UNAVAIL, Unsupported or not ready.
 */
extern int
bcmbd_schanfifo_info_get(int unit, int *num_chans, size_t *cmd_mem_wsize);

/*!
 * \brief Initialize SCHAN FIFO.
 *
 * \param [in] unit Unit number.
 * \param [in] max_polls Maximum poll times for operations to complete.
 * \param [in] flags Control flags (\ref SCHANFIFO_IF_xxx).
 *
 * The function will reset all channels and save the control flags
 * which will take effect on all the subsequent ops_send() callings
 * until this function gets invoked again.
 *
 * \retval SHR_E_NONE, No errors.
 * \retval SHR_E_UNAVAIL, Unsupported or not ready.
 */
extern int
bcmbd_schanfifo_init(int unit, uint32_t max_polls, uint32_t flags);

/*!
 * \brief Send operations to SCHAN FIFO channel.
 *
 * \param [in] unit Unit number.
 * \param [in] chan Channel number.
 * \param [in] num_ops Number of operations.
 * \param [in] req_buff Requested operations buffer.
 * \param [in] req_wsize Size of operations buffer in 32-bit words.
 * \param [in] buff_paddr Physical address of requested operations buffer.
 * \param [in] flags Control flags (\ref SCHANFIFO_OF_xxx).
 *
 * This function will write command memory, configure registers, etc.
 * This function does not support SCHANFIFO_OF_WAIT_COMPLETE and
 * SCHANFIFO_OF_CLEAR_START. START bit will be set if SCHANFIFO_OF_SET_START
 * is asserted after sending the operations.
 * This function will not wait for HW to complete.
 *
 * \retval SHR_E_NONE, No errors.
 * \retval SHR_E_PARAM, Invalid parameters.
 * \retval SHR_E_UNAVAIL, Unsupported or not ready.
 */
extern int
bcmbd_schanfifo_ops_send(int unit, int chan, uint32_t num_ops, uint32_t *req_buff,
                         size_t req_wsize, uint64_t buff_paddr, uint32_t flags);

/*!
 * \brief Set start for SCHAN FIFO channel.
 *
 * \param [in] unit Unit number.
 * \param [in] chan Channel number.
 * \param [in] start Start value.
 *
 * This function will set the start value in HW register.
 *
 * \retval SHR_E_NONE, No errors.
 * \retval SHR_E_PARAM, Invalid parameters.
 * \retval SHR_E_UNAVAIL, Unsupported or not ready.
 */
extern int
bcmbd_schanfifo_set_start(int unit, int chan, bool start);

/*!
 * \brief Get SCHAN FIFO channel status.
 *
 * \param [in] unit Unit number.
 * \param [in] chan Channel number.
 * \param [in] num_ops Number of operations.
 * \param [in] flags Control flags (\ref SCHANFIFO_OF_xxx).
 * \param [out] done_ops Number of successful operations.
 * \param [out] resp_buff Responses buffer.
 *
 * This function will read the status register and return the number of
 * successful operations. The response buffer pointer will be returned
 * if resp_buff is not NULL.
 * The channel will be reset if operations fail.
 *
 * \retval SHR_E_NONE, No errors.
 * \retval SHR_E_FAIL, Operation failed.
 * \retval SHR_E_PARAM, Invalid parameters.
 * \retval SHR_E_UNAVAIL, Unsupported or not ready.
 */
extern int
bcmbd_schanfifo_status_get(int unit, int chan, uint32_t num_ops, uint32_t flags,
                           uint32_t *done_ops, uint32_t **resp_buff);

#endif /* BCMBD_SCHANFIFO_H */
