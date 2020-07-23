/*! \file bcmbd_m0ssq_mbox_internal.h
 *
 * M0SSQ MBOX driver internal definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_M0SSQ_MBOX_INTERNAL_H
#define BCMBD_M0SSQ_MBOX_INTERNAL_H

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <sal/sal_time.h>
#include <sal/sal_mutex.h>

#include <bsl/bsl.h>

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_m0ssq_internal.h>
#include <bcmbd/bcmbd_m0ssq_fifo_internal.h>
#include <bcmbd/bcmbd_m0ssq_mbox.h>

/*! Maximun number of mboxes for a unit. */
#define MAX_NUM_OF_MBOX          8

/*! Maximun number of MBOX_CHN_TYPE_XX.  */
#define MBOX_CHN_TYPE_MAX        2

/*! MBOX TX channel. */
#define MBOX_CHN_TYPE_TX         1

/*! MBOX RX channel. */
#define MBOX_CHN_TYPE_RX         0

/*!
 * Message/reponse flags definition for
 * \ref bcmbd_m0ssq_mbox_int_msg_t
 */

/*! Flag of message indicates message is syncronous. */
#define MBOX_SYNC_MSG                0x1

/*! Flag of message indicates message required reponse. */
#define MBOX_RESP_REQUIRED           0x2

/*! Flag of reponse indicates response of message is ready. */
#define MBOX_RESP_READY              0x4

/*! Flag of reponse indicates message is proccessed successfully. */
#define MBOX_RESP_SUCCESS            0x8

/*! Flag of message indicates message is asyncronous. */
#define MBOX_ASYNC_STATUS            0x10

/*!
 * \brief Internal message format for M0SSQ-MBOX communication.
 */
typedef struct bcmbd_m0ssq_mbox_int_msg_s {

    /*! Flags to identify the message type. */
    uint32_t flags;

    /*! Message identifier. */
    uint32_t id;

    /*! Size of payload in words. */
    uint32_t  size;

    /*! Lower 32bit of 64 bit metadata preserved to use it
     *  while handling response.
     */
    uint32_t  tag0;

    /*! Upper 32bit of 64 bit metadata preserved to use it
     *  while handling response.
     */
    uint32_t  tag;

    /*! Variable size message payload. */
    uint32_t  data[];

} bcmbd_m0ssq_mbox_int_msg_t;

/*******************************************************************************
 * M0SSQ and M0SSQ-MBOX shared structure.
 */

/*!
 * \brief Data for a MBOX instance.
 */
typedef struct mbox_s {

    /*! FIFO channels within a MBOX. */
    bcmbd_m0ssq_fifo_t chan[MBOX_CHN_TYPE_MAX];

    /*! MBOX id. */
    uint32_t mbox_id;

    /*! Flag indicates MBOX is in use. */
    bool inuse;

    /*! Recieved message handler. */
    bcmbd_m0ssq_mbox_msg_handler_f recv_msg_handler;

    /*! uC connect with host by this MBOX. */
    uint32_t uc;

} mbox_t;

/*!
 * \brief MBOX driver data for a unit.
 */
typedef struct bcmbd_m0ssq_mbox_dev_s {

    /*! Flag indicates if mboxes are inited done or not. */
    bool init_done;

    /*! MBOX intances for a unit. */
    mbox_t mbox[MAX_NUM_OF_MBOX];

    /*! Number of MBOXs for a unit. */
    uint32_t num_of_mbox;

    /*! Size of each MBOX. */
    uint32_t mbox_size;

} bcmbd_m0ssq_mbox_dev_t;

/*!
 * \brief Get a MBOX driver data of a unit.
 *
 * \param [in] unit Unit number.
 * \param [out] dev MBOX driver instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if the mbox driver is not avaiable.
 */
extern int
bcmbd_m0ssq_mbox_dev_get(int unit, bcmbd_m0ssq_mbox_dev_t **dev);

/*!
 * \brief MBOX driver initialization.
 *
 * This function is used to initialize the MBOX driver for a
 * unit.
 *
 * \param [in] unit Unit number.
 * \param [in] mem  A memory block for mboxes use.
 * \param [in] num_of_mbox Number of mbox in a memory block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL MBOX driver initialization failed.
 */
extern int
bcmbd_m0ssq_mbox_init(int unit, bcmbd_m0ssq_mem_t *mem,
                      uint32_t num_of_mbox);

/*!
 * \brief MBOX driver cleanup.
 *
 * This function is used to cleanup the MBOX driver for a
 * unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL MBOX driver cleanup failed.
 */
extern int
bcmbd_m0ssq_mbox_cleanup(int unit);

#endif /* BCMBD_M0SSQ_MBOX_INTERNAL_H */
