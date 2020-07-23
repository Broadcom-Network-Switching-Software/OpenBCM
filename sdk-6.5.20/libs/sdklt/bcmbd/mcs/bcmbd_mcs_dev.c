/*! \file bcmbd_mcs_dev.c
 *
 * MCS driver and Messaging APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <shr/shr_timeout.h>
#include <sal_config.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sem.h>
#include <sal/sal_sleep.h>
#include <sal/sal_time.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmbd/bcmbd_mcs.h>
#include <bcmbd/bcmbd_mcs_internal.h>

#include "bcmbd_mcs_dev.h"

#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

#define MCS_SYS_THREAD_PRI      (SAL_THREAD_PRIO_DEFAULT-5)
#define MCS_THREAD_PRI          (SAL_THREAD_PRIO_DEFAULT)

/* Macros for passing unit + uC numbers to the threads */
#define ENCODE_UNIT_UC(_unit, _uc) ((void *) (uintptr_t) ((_unit << 16) | _uc))
#define DECODE_UNIT(_val)       ((int) (((uintptr_t) (_val)) >> 16))
#define DECODE_UC(_val)         (((uintptr_t) (_val)) & 0xffff)

/* Offset of field inside the structure */
#define MCS_MSG_OFFSETOF(_x)    (offsetof(mcs_msg_area_t, _x))

#define MCS_MSG_DATA_SIZE       (sizeof(mcs_msg_data_t))

/*! Max number of messages pending */
#define MCS_MSG_EVENT_QUEUE_SIZE (1000)

/* Size of Buffer to test host endianess */
#define MCS_MSG_TEST_DMA_SIZE   (32)

/* MCS Message areas */
#define MCS_MSG_OUT_AREA(_base) ((_base))
#define MCS_MSG_IN_AREA(_base)  ((_base) + sizeof(mcs_msg_area_t))

/* MCS Console buffer areas */
#define MCS_CON_PRINT_BUF_SIZE  (256)
#define MCS_CON_IN_AREA(_base)  ((_base) + (2 * sizeof(mcs_msg_area_t)))
#define MCS_CON_OUT_AREA(_base) ((_base) + (2 * sizeof(mcs_msg_area_t)) + \
                                 MCS_CON_PRINT_BUF_SIZE)

/*******************************************************************************
 * Local data
 */

static mcs_dev_t mcs_dev[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * Static routine to read a value from the MCS
 */
static uint32_t
mcs_rd(int unit, int uc, uint32_t mcs_addr)
{
    uint32_t pci_addr, val;

    pci_addr = bcmbd_mcs_to_pci_addr(unit, uc, mcs_addr);
    (void)bcmdrd_hal_iproc_read(unit, pci_addr, &val);
    return val;
}

/*!
 * Static routine to write a value to the MCS
 */
static void
mcs_wr(int unit, int uc, uint32_t mcs_addr, uint32_t val)
{
    uint32_t pci_addr;

    pci_addr = bcmbd_mcs_to_pci_addr(unit, uc, mcs_addr);
    (void)bcmdrd_hal_iproc_write(unit, pci_addr, val);
}

/*!
 * Static routine to add received message to receive LL
 * Inserts an element at tail of LL.
 */
static void
ll_insert_tail(ll_ctrl_t *p_ll_ctrl,  ll_element_t *p_element)
{
    p_element->p_prev = p_ll_ctrl->p_tail;
    p_element->p_next = NULL;

    if(p_ll_ctrl->ll_count) {
        /* LL is not empty */
        p_ll_ctrl->p_tail->p_next = p_element;
    } else {
        /* LL is empty */
        p_ll_ctrl->p_head = p_element;
    }

    p_ll_ctrl->ll_count++;
    p_ll_ctrl->p_tail = p_element;
}

/*!
 * Static routine to remove message from receive LL
 * Removes an element from head of LL.
 */
static ll_element_t *
ll_remove_head (ll_ctrl_t *p_ll_ctrl)
{
    ll_element_t *p_el;

    if(p_ll_ctrl->ll_count) {
        /* keep the removed head */
        p_el = p_ll_ctrl->p_head;

       /*update ll_ctrl*/
        p_ll_ctrl->p_head = p_el->p_next;
        p_ll_ctrl->ll_count--;
        if(p_ll_ctrl->ll_count) {
            /* there are still elements in the list */
            p_ll_ctrl->p_head->p_prev = NULL;
        } else {
            /* p_el was the only list element */
            p_ll_ctrl->p_tail = NULL;
        }

        /* disconnect the removed element from the list */
        p_el->p_next = NULL;
        p_el->p_prev = NULL;

        return p_el;
    }
    return NULL;
}

/*!
 * \brief Notify eApps of MCS events.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] event MCS event to notify.
 *
 * \return Always SHR_E_NONE.
 */
static int
mcs_apps_notify(int unit, int uc, bcmbd_mcs_event_t event)
{
    int appl;
    void *data;
    bcmbd_mcs_msg_dev_t *mdev = &(mcs_dev[unit].mcs_msg_dev);

    /* Notify registered applications */
    for (appl = 0; appl <= MAX_MCS_MSG_CLASS; appl++) {
        if (mdev->mcs_msg_appl_cb[uc][appl]) {
            data = mdev->mcs_msg_appl_cb_data[uc][appl];
            (mdev->mcs_msg_appl_cb[uc][appl])(unit, uc, event, data);
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Process the MCS message status from a uC.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 *
 * \return 0 if no errors, 1 if uC reset.
 */
static int
mcs_msg_process_status(int unit, int uc)
{
    mcs_dev_t *dev = &mcs_dev[unit];
    bcmbd_mcs_msg_dev_t *mdev = &(dev->mcs_msg_dev);
    uint32_t msg_base = mdev->mcs_msg_area[uc];
    uint32_t area_in = MCS_MSG_IN_AREA(msg_base);
    uint32_t area_out = MCS_MSG_OUT_AREA(msg_base);
    uint32_t con_in_data_addr = MCS_CON_IN_AREA(msg_base);
    mcs_msg_host_status_t cur_stat_in;
    int out_status;
    uint32_t i;
    mcs_msg_ll_node_t *msg_node = NULL;

    /* Set when new out status */
    out_status = 0;

    /* Snapshot the status from the uC memory */
    bcmdrd_hal_iproc_read(unit, (area_in + MCS_MSG_OFFSETOF(status)),
                          &cur_stat_in);

    if (MCS_MSG_STATUS_STATE(cur_stat_in) == MCS_MSG_RESET_STATE) {
        /* Got a reset. Restart init */
        return 1;
    }

    sal_mutex_take(mdev->mcs_msg_control, SAL_SEM_FOREVER);

    /*  Process the ACKS */
    for (i = MCS_MSG_STATUS_ACK_INDEX(mdev->mcs_msg_prev_status_in[uc]);
         i != MCS_MSG_STATUS_ACK_INDEX(cur_stat_in);
         i = MCS_MSG_INCR(i)) {

        /* See if any thread is waiting for an ack */
        if (mdev->mcs_msg_ack_sems[uc][i] != NULL) {
            *mdev->mcs_msg_ack_data[uc][i] =
                MCS_MSG_ACK_BIT(cur_stat_in, i);
            sal_sem_give(mdev->mcs_msg_ack_sems[uc][i]);
            mdev->mcs_msg_ack_sems[uc][i] = NULL;
            mdev->mcs_msg_ack_data[uc][i] = NULL;
        }

        /* Give back one for each ACK received */
        sal_sem_give(mdev->mcs_msg_send_queue_sems[uc]);
    }

    for (i = MCS_MSG_STATUS_SENT_INDEX(mdev->mcs_msg_prev_status_in[uc]);
        i != MCS_MSG_STATUS_SENT_INDEX(cur_stat_in); i = MCS_MSG_INCR(i)) {
        /* Read the message data in in 2 32-bit chunks */
        mcs_msg_data_t msg;
        uint32_t msg_class;
        uint32_t addr = area_in + MCS_MSG_OFFSETOF(data[0].words[0]) +
                        (MCS_MSG_DATA_SIZE * i);
        bcmdrd_hal_iproc_read(unit, addr, &(msg.words[0]));
        addr = area_in + MCS_MSG_OFFSETOF(data[0].words[1]) +
               (MCS_MSG_DATA_SIZE * i);
        bcmdrd_hal_iproc_read(unit, addr, &(msg.words[1]));
        addr = area_in + MCS_MSG_OFFSETOF(data[0].words[2]) +
               (MCS_MSG_DATA_SIZE * i);
        bcmdrd_hal_iproc_read(unit, addr, &(msg.words[2]));
        msg_class = MCS_MSG_MCLASS_GET(msg);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "UC%d: msg recv mclass 0x%02x sclass 0x%02x "
                                "len 0x%04x data 0x%08x data1 0x%08x\n"),
                     uc, msg_class,
                     MCS_MSG_SUBCLASS_GET(msg),
                     MCS_MSG_LEN_GET(msg),
                     MCS_MSG_DATA_GET(msg),
                     MCS_MSG_DATA1_GET(msg)));
        if (msg_class > MAX_MCS_MSG_CLASS) {
            MCS_MSG_ACK_BIT_CLEAR(mdev->mcs_msg_prev_status_out[uc], i);
        } else if ((msg_class == MCS_MSG_CLASS_SYSTEM) &&
                  (MCS_MSG_SUBCLASS_GET(msg) == MCS_MSG_SUBCLASS_SYSTEM_PING)) {
            /* Special case - PING just gets an ACK */
            MCS_MSG_ACK_BIT_SET(mdev->mcs_msg_prev_status_out[uc], i);
        } else if ((msg_class == MCS_MSG_CLASS_SYSTEM) &&
                (MCS_MSG_SUBCLASS_GET(msg) == MCS_MSG_SUBCLASS_SYSTEM_CON_IN)) {
            /* until 8 chars, the data is part of the message. Above that, we
             * have to read from the FW buffer area.
             */
            uint32_t len = MCS_MSG_LEN_GET(msg);
            uint32_t tem[64];
            char *temc = (char *)&tem[0];
            mcs_con_t *con =  &(dev->mcs_con[uc]);
            if (con->mcs_con_buffer != NULL) {
                if (len > 8) {
                    int idx, wsize = (len + 3) / 4;
                    for (idx = 0; idx < wsize; idx++) {
                        tem[idx] = mcs_rd(unit, uc, con_in_data_addr + (idx * 4));
                    }
                } else {
                    tem[0] = MCS_MSG_DATA_GET(msg);
                    tem[1] = MCS_MSG_DATA1_GET(msg);
                }
            }
            MCS_MSG_ACK_BIT_SET(mdev->mcs_msg_prev_status_out[uc], i);
            if (con->mcs_con_buffer != NULL) {
                /* Enquque the data in the circular print buffer */
                uint32_t widx = con->mcs_con_buf_write_idx;
                if (len > (MCS_CON_BUF_SIZE - widx))
                {
                    /* Wrap around */
                    uint32_t len1 = MCS_CON_BUF_SIZE - widx;
                    sal_memcpy(&con->mcs_con_buffer[widx], &temc[0], len1);
                    sal_memcpy(&con->mcs_con_buffer[0], &temc[len1],
                               (len - len1));
                    con->mcs_con_buf_write_idx = (len - len1);
                } else {
                    sal_memcpy(&con->mcs_con_buffer[widx], &temc[0], len);
                    con->mcs_con_buf_write_idx += len;
                }
            }
        } else {
            if (((mdev->mcs_msg_rcvd_ll[uc][msg_class]).ll_count) <
                                                    MCS_MSG_EVENT_QUEUE_SIZE) {
                /* Enqueue the message in respective LL */
                msg_node = sal_alloc(sizeof(mcs_msg_ll_node_t),
                                     "bcmbdMcsCpuMsgNode");
                if (msg_node != NULL) {
                    MCS_MSG_ACK_BIT_SET(mdev->mcs_msg_prev_status_out[uc], i);
                    msg_node->msg_data.words[0] = msg.words[0];
                    msg_node->msg_data.words[1] = msg.words[1];
                    msg_node->msg_data.words[2] = msg.words[2];
                    ll_insert_tail(&(mdev->mcs_msg_rcvd_ll[uc][msg_class]),
                                   (ll_element_t *)msg_node);

                    /* Kick the waiter */
                    sal_sem_give(mdev->mcs_msg_rcv_sems[uc][msg_class]);

                } else {
                    /* Malloc failed, no room for the message - NACK it */
                    MCS_MSG_ACK_BIT_CLEAR(mdev->mcs_msg_prev_status_out[uc], i);
                    LOG_WARN(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "Could not malloc, Sending NACK. "
                                         "Msg Class=%d\n"), msg_class));
                }
            } else {
                MCS_MSG_ACK_BIT_CLEAR(mdev->mcs_msg_prev_status_out[uc], i);
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "More than 1000 nodes in LL for "
                                     "Msg Class=%d\n"), msg_class));
            }
        }

        out_status = 1;
    }

    /* Update the status out with the final index */
    MCS_MSG_STATUS_ACK_INDEX_W(mdev->mcs_msg_prev_status_out[uc], i);

    /* Remember previous status for next time */
    mdev->mcs_msg_prev_status_in[uc] = cur_stat_in;

    if (out_status) {
        /*
         * We have determined the new status out - write it and notify
         * the other uC by writing the word once here and interrupting
         * the uC.
         */
        bcmdrd_hal_iproc_write(unit, (area_out + MCS_MSG_OFFSETOF(status)),
                          mdev->mcs_msg_prev_status_out[uc]);
        bcmbd_mcs_intr_set(unit, uc); /* Wake up call */
    }

    sal_mutex_give(mdev->mcs_msg_control);
    return 0;
}

/*!
 * \brief Internal routine to wait for the MCS message to become active.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 *
 * \retval SHR_E_NONE Handshake complete, msg active.
 * \retval SHR_E_UNAVAIL System shut down.
 */
static int
mcs_msg_active_wait(int unit, uint32_t uc)
{
    bcmbd_mcs_msg_dev_t *mdev = &(mcs_dev[unit].mcs_msg_dev);

    if (((mdev->sw_intr_active & (1 << uc)) == 0) ||
        (mdev->mcs_msg_active[uc] == NULL)) {
        return (SHR_E_UNAVAIL);
    }

    /* Wait for the semaphore to be available */
    if (!sal_sem_take(mdev->mcs_msg_active[uc], 10000000)) {
        sal_sem_give(mdev->mcs_msg_active[uc]);
    }

    return (SHR_E_NONE);
}

/*!
 * \brief Internal routine to Wait for a message of a given type from a uC.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] mclass Message class.
 * \param [in] msg Pointer to buffer for message (64 bits).
 * \param [in] is_system If this receive is done by system thread
 *              so no wait on messaging being up
 *
 * \retval SHR_E_NONE Message ACK'd (agent received msg).
 * \retval SHR_E_xxx Message NAK'd (no agent or buffer space full or timeout).
 */
static int
mcs_msg_receive(int unit, int uc, uint8_t mclass,
                mcs_msg_data_t *msg, int timeout, int is_system)
{
    int rc = SHR_E_NONE;
    bcmbd_mcs_msg_dev_t *mdev = &(mcs_dev[unit].mcs_msg_dev);
    mcs_msg_ll_node_t *msg_node;

    if ((mdev->sw_intr_active & (1 << uc)) == 0) {
        return SHR_E_INIT;
    }

    /* Wait/Check if msg available to be received */
    if ( !mdev->mcs_msg_rcv_sems[uc][mclass] ||
         sal_sem_take(mdev->mcs_msg_rcv_sems[uc][mclass], timeout) ) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "UC%d semtake - mcs_msg_rcv_sems failed\n"), uc));
        return SHR_E_TIMEOUT;
    }

    /* System messages must be exchanged in order to bring up messaging */
    /* So only wait for messaging to be up if this is not a system message */
    if (!is_system && mcs_msg_active_wait(unit, uc) != SHR_E_NONE) {
        return SHR_E_INIT;
    }

    /* Block others for a bit */
    if (sal_mutex_take(mdev->mcs_msg_control, mdev->mcs_msg_ctl_timeout)) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "UC%d semtake - mcs_msg_control timed out\n"), uc));
        return SHR_E_TIMEOUT;
    }

    /* Remove the message from Receive LL head*/
    msg_node = (mcs_msg_ll_node_t *)ll_remove_head(
                                       &(mdev->mcs_msg_rcvd_ll[uc][mclass]));
    if (msg_node != NULL){
        msg->words[0] = msg_node->msg_data.words[0];
        msg->words[1] = msg_node->msg_data.words[1];
        msg->words[2] = msg_node->msg_data.words[2];
        sal_free(msg_node);
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "UC%d NULL node returned from LL\n"), uc));
        /* Check the uc_msg_control mutex availability, to avoid runtime err */
        if (mdev->mcs_msg_control != NULL)  {
            sal_mutex_give(mdev->mcs_msg_control);
        }
        return SHR_E_INTERNAL;
    }

    /* Return the global message control mutex */
    sal_mutex_give(mdev->mcs_msg_control);

    if ((rc == SHR_E_NONE) && (MCS_MSG_MCLASS_GET((*msg)) != mclass)) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "UC%d Reply from wrong mclass\n"), uc));
        rc = SHR_E_INTERNAL;            /* Reply from wrong mclass */
    }
    return (rc);
}

static void
mcs_msg_thread_stop(bcmbd_mcs_msg_dev_t *mdev, int uc)
{
    int i;
    sal_sem_t tsem;
    mcs_msg_ll_node_t *msg_node;

    sal_sem_take(mdev->mcs_msg_active[uc], 0);

    /* NAK all of the current messages */
    for (i = 0; i < NUM_MCS_MSG_SLOTS; i++) {
        if (mdev->mcs_msg_ack_sems[uc][i] != NULL) {
            tsem = mdev->mcs_msg_ack_sems[uc][i];
            mdev->mcs_msg_ack_sems[uc][i] = NULL;
            sal_sem_give(tsem);
        }
    }

    /* Init the rest of our structures */
    if (mdev->mcs_msg_send_queue_sems[uc] != NULL) {
        sal_sem_destroy(mdev->mcs_msg_send_queue_sems[uc]);
        mdev->mcs_msg_send_queue_sems[uc] = NULL;
    }

    /* Clear receive LL and set Semaphore count to 0 */
    sal_mutex_take(mdev->mcs_msg_control, SAL_SEM_FOREVER);
    for (i = 0; i <= MAX_MCS_MSG_CLASS; i++) {
        while( mdev->mcs_msg_rcvd_ll[uc][i].ll_count ) {
            msg_node = (mcs_msg_ll_node_t *)ll_remove_head(
                                             &(mdev->mcs_msg_rcvd_ll[uc][i]));
            if (msg_node != NULL) {
                sal_free(msg_node);
            }
            /* Decrement the counting semaphore count to be in sync with LL */
            sal_sem_take(mdev->mcs_msg_rcv_sems[uc][i], 10000000);
        };

        /* Kick the waiter */
        sal_sem_give(mdev->mcs_msg_rcv_sems[uc][i]);
    }
    sal_mutex_give(mdev->mcs_msg_control);
}

static void
mcs_msg_thread(shr_thread_t tc, void *arg)
{
    int unit = DECODE_UNIT(arg);
    int uc = DECODE_UC(arg);
    mcs_dev_t *dev = &mcs_dev[unit];
    bcmbd_mcs_msg_dev_t *mdev = &(dev->mcs_msg_dev);
    int i;
    uint32_t msg_base;
    uint32_t area_in;
    uint32_t area_out;
    mcs_msg_host_status_t cur_stat_in;
    sal_sem_t tsem;
    mcs_msg_ll_node_t *msg_node;

    /* Use the message control mutex to pick a UC */
    sal_mutex_take(mdev->mcs_msg_control, SAL_SEM_FOREVER);

    /* This uC is ready to start */
    mdev->sw_intr[uc] = sal_sem_create("SW interrupt",
                                       SAL_SEM_BINARY, 0);
    if (mdev->sw_intr[uc] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "MCS Msg Thread failed "
                            "(swIntr) %d\n"), uc));
        sal_mutex_give(mdev->mcs_msg_control);
        return;
    }

    bcmbd_mcs_intr_enable(unit, uc);

    sal_mutex_give(mdev->mcs_msg_control);

    msg_base = mdev->mcs_msg_area[uc];
    area_in = MCS_MSG_IN_AREA(msg_base);
    area_out = MCS_MSG_OUT_AREA(msg_base);

    /* While not killed off */
    while (1) {
        /* We must write the IN status area to zero.  This is becuase
        the ECC for the SRAM where it lives might not be initialized
        and the resulting ECC error would cause a PCIe error which
        would cause the SDK to crash.  Zero is safe in this protocol
        since we will not consider that a reset or init state */

        bcmdrd_hal_iproc_write(unit, (area_in + MCS_MSG_OFFSETOF(status)), 0);
        mdev->mcs_msg_prev_status_out[uc] = 0;

        /* Handshake protocol:

               Master (SDK host):
                   (restart or receive RESET state)
                   RESET STATE
                   Wait for INIT state
                   INIT state
                   Wait for READY state
                   READY state

               Slave (uC)
                   (retart or recieve RESET state)
                   RESET state
                   Wait for RESET state
                   INIT state
                   Wait for INIT state
                   READY sate

        */

        /* Phase 1 of init handshake (wait for INIT/RESET state) */
        while (1) {
            bcmdrd_hal_iproc_read(unit, (area_in + MCS_MSG_OFFSETOF(status)),
                                  &cur_stat_in);
            if (MCS_MSG_STATUS_STATE(cur_stat_in) == MCS_MSG_INIT_STATE) {
                /* Our partner is the subordinate so we go to
                   INIT state when we see him go to INIT state */
                break;
            }

            /* Set the status and write it out.  We do that inside the
               loop in the first phase to avoid a race where the uKernel
               is clearing memory to clear ECC errors and the first write
               is lost.  This is not needed after the first phase since
               we see the uKernel msg area change state. */
            mdev->mcs_msg_prev_status_out[uc] = MCS_MSG_RESET_STATE;
            bcmdrd_hal_iproc_write(unit, (area_out + MCS_MSG_OFFSETOF(status)),
                              mdev->mcs_msg_prev_status_out[uc]);

            bcmbd_mcs_intr_set(unit, uc); /* Wake up call */

            if (sal_sem_take(mdev->sw_intr[uc], SAL_SEM_FOREVER)) {
                mcs_msg_thread_stop(mdev, uc);
                return;
            }
            if ((mdev->sw_intr_active & (1 << uc)) == 0) { /* If exit signaled */
                mcs_msg_thread_stop(mdev, uc);
                return;
            }
        }

        /* Proceed to INIT state */
        MCS_MSG_STATUS_STATE_W(mdev->mcs_msg_prev_status_out[uc],
                               MCS_MSG_INIT_STATE);
        bcmdrd_hal_iproc_write(unit, (area_out + MCS_MSG_OFFSETOF(status)),
                               mdev->mcs_msg_prev_status_out[uc]);
        bcmbd_mcs_intr_set(unit, uc); /* Wake up call */

        /* Phase 2 of init handshake (wait for READY/INIT state */
        while (1) {
            /* Get the status once */
            bcmdrd_hal_iproc_read(unit, (area_in + MCS_MSG_OFFSETOF(status)),
                                  &cur_stat_in);
            if (MCS_MSG_STATUS_STATE(cur_stat_in) == MCS_MSG_READY_STATE) {
                /* Our partner is the subordinate so we go to
                   READY state when we see him go to READY state */
                break;
            } else if (MCS_MSG_STATUS_STATE(cur_stat_in) == MCS_MSG_RESET_STATE) {
                /* The subordinate has requested another
                   reset.  process_msg_status will detect this
                   and we will go to the top of the loop */
                break;
            }

            if (sal_sem_take(mdev->sw_intr[uc], SAL_SEM_FOREVER)) {
                mcs_msg_thread_stop(mdev, uc);
                return;
            }
            if ((mdev->sw_intr_active & (1 << uc)) == 0) { /* If exit signaled */
                mcs_msg_thread_stop(mdev, uc);
                return;
            }
        }

        MCS_MSG_STATUS_STATE_W(mdev->mcs_msg_prev_status_out[uc],
                               MCS_MSG_READY_STATE);
        bcmdrd_hal_iproc_write(unit, (area_out + MCS_MSG_OFFSETOF(status)),
                          mdev->mcs_msg_prev_status_out[uc]);
        mdev->mcs_msg_prev_status_in[uc] = cur_stat_in;
        bcmbd_mcs_intr_set(unit, uc); /* Wake up call */

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "UC%d messaging system: up.\n"), uc));

        /* Give one count for each slot (all free to start) */
        for (i = 0; i < NUM_MCS_MSG_SLOTS; i++) {
            sal_sem_give(mdev->mcs_msg_send_queue_sems[uc]);
        }

        /* Start normal processing */
        while (1) {
            if (mcs_msg_process_status(unit, uc)) {
                break;              /* Received reset */
            }
            if (sal_sem_take(mdev->sw_intr[uc], SAL_SEM_FOREVER)) {
                mcs_msg_thread_stop(mdev, uc);
                return;
            }
            if ((mdev->sw_intr_active & (1 << uc)) == 0) {
                /* If exit signaled */
                mcs_msg_thread_stop(mdev, uc);
                return;
            }
        }

        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "CPU messaging back to reset.\n")));
        /* Connection failed - go back to reset */
        if ((mdev->sw_intr_active & (1 << uc)) == 0) {
            /* If exit signaled */
            mcs_msg_thread_stop(mdev, uc);
            return;
        }

        sal_sem_take(mdev->mcs_msg_active[uc], 0);    /* Get or timeout */

        /* Take all of the slots */
        for (i = 0; i < NUM_MCS_MSG_SLOTS; i++) {
            /* These will timeout immedaitely */
            sal_sem_take(mdev->mcs_msg_send_queue_sems[uc], 0);
        }

        /* Clear receive LL and set Semaphore count to 0 */
        sal_mutex_take(mdev->mcs_msg_control, SAL_SEM_FOREVER);
        for (i = 0; i <= MAX_MCS_MSG_CLASS; i++) {
            while( mdev->mcs_msg_rcvd_ll[uc][i].ll_count ){
                msg_node = (mcs_msg_ll_node_t *)ll_remove_head(
                                               &(mdev->mcs_msg_rcvd_ll[uc][i]));
                if (msg_node != NULL){
                    sal_free(msg_node);
                }
                /* Decrement the counting sema count to be in sync with LL */
                sal_sem_take(mdev->mcs_msg_rcv_sems[uc][i], 10000000);
            };

            /* Kick the waiter */
            sal_sem_give(mdev->mcs_msg_rcv_sems[uc][i]);
        }
        sal_mutex_give(mdev->mcs_msg_control);

        /* Let the receive thread kick in and exit */
        sal_thread_yield();

        for (i = 0; i < NUM_MCS_MSG_SLOTS; i++) {
            if (mdev->mcs_msg_ack_sems[uc][i] != NULL) {
                /* Kick the waiter */
                tsem = mdev->mcs_msg_ack_sems[uc][i];
                mdev->mcs_msg_ack_sems[uc][i] = NULL;
                sal_sem_give(tsem);
            }
        }
    } /* end while (1) */

    mcs_msg_thread_stop(mdev, uc);
    return;
}

static void
mcs_msg_system_thread(shr_thread_t tc, void *arg)
{
    int unit = DECODE_UNIT(arg);
    int uc = DECODE_UC(arg);
    bcmbd_mcs_msg_dev_t *mdev = &(mcs_dev[unit].mcs_msg_dev);
    mcs_msg_data_t rcv, send;
    uint8_t *endian_buf = NULL;
    uint64_t endian_buff_paddr;

    while (1) {
        while (mcs_msg_receive(unit, uc, MCS_MSG_CLASS_SYSTEM, &rcv,
                               SAL_SEM_FOREVER, 1) != SHR_E_NONE) {
            if (mcs_msg_active_wait(unit, uc) != SHR_E_NONE) {
                if (endian_buf) {
                    /* Free the buffer if it was previously allocated */
                    bcmdrd_hal_dma_free(unit, MCS_MSG_TEST_DMA_SIZE,
                                        endian_buf, endian_buff_paddr);
                    endian_buf = NULL;
                }

                /* Let the receive thread kick in and exit */
                sal_thread_yield();

                /* wait 0.5sec until all threads exit */
                sal_usleep(500000);

                /* Clear the swIntr, after exiting all threads */
                sal_mutex_take(mdev->mcs_msg_control, SAL_SEM_FOREVER);

                if (mdev->sw_intr[uc] != NULL) {
                    sal_sem_destroy(mdev->sw_intr[uc]);
                    mdev->sw_intr[uc] = NULL;
                }
                sal_mutex_give(mdev->mcs_msg_control);

                /* Down and not coming back up - exit thread */
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "System thread exiting.\n")));
                return;
            }
        }

        if (endian_buf) {
            /* Free the buffer if it was previously allocated */
            bcmdrd_hal_dma_free(unit, MCS_MSG_TEST_DMA_SIZE,
                                endian_buf, endian_buff_paddr);
            endian_buf = NULL;
        }

        if (MCS_MSG_SUBCLASS_GET(rcv) == MCS_MSG_SUBCLASS_SYSTEM_INFO) {
            int send_rv = SHR_E_INTERNAL;

            /* Tell Firmware to pull local data to determine endianness */
            MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_SYSTEM);
            MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_SYSTEM_DMA_ENDIAN);
            MCS_MSG_LEN_SET(send, 0);
            endian_buf =  bcmdrd_hal_dma_alloc(unit, MCS_MSG_TEST_DMA_SIZE,
                                       "uKernel DMA", &endian_buff_paddr);
            endian_buf[0] = 1;
            endian_buf[1] = 2;
            endian_buf[2] = 3;
            endian_buf[3] = 4;
            MCS_MSG_DATA_SET(send, ((uint32_t)(endian_buff_paddr & 0xffffffff)));
            MCS_MSG_DATA1_SET(send, ((uint32_t)((endian_buff_paddr >> 32) & 0xffffffff)));

            send_rv = bcmbd_mcs_msg_send(unit, uc, &send,
                                           mdev->mcs_msg_send_timeout);

            if (send_rv == SHR_E_NONE) {
                /* Indicate to waiters that we are up */
                sal_sem_give(mdev->mcs_msg_active[uc]);
            }
        } else if (MCS_MSG_SUBCLASS_GET(rcv) == MCS_MSG_SUBCLASS_SYSTEM_EXCEPTION) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Firmware exception in uC (%d)\n"), uc));
            (void)mcs_apps_notify(unit, uc, BCMBD_MCS_EVENT_EXCEPTION);
        }
    }
}

static void
mcs_con_print_thread(shr_thread_t tc, void *arg)
{
    int unit = DECODE_UNIT(arg);
    int uc = DECODE_UC(arg);
    mcs_dev_t *dev = &mcs_dev[unit];
    mcs_con_t *con =  &(dev->mcs_con[uc]);

    while (1) {
        if (shr_thread_stopping(tc)) {
            break;
        }
        sal_usleep(5); /* Yield */
        if (con->mcs_con_buffer == NULL) {
            continue;
        }
        while (con->mcs_con_buf_write_idx != con->mcs_con_buf_read_idx) {
            char ch = *((char *)&con->mcs_con_buffer[con->mcs_con_buf_read_idx]);
            bcmbd_mcs_putc(ch);
            con->mcs_con_buf_read_idx ++;
            if (con->mcs_con_buf_read_idx == MCS_CON_BUF_SIZE)
            {
                con->mcs_con_buf_read_idx = 0;
            }
        }
    }
}

/*******************************************************************************
 * Public functions
 */
int
bcmbd_mcs_con_cmd_send(int unit, int uc, char *cmd, int waitms)
{
    bcmbd_mcs_msg_dev_t *mdev = &(mcs_dev[unit].mcs_msg_dev);
    uint32_t msg_area = mdev->mcs_msg_area[uc];
    uint32_t con_out_data_addr = MCS_CON_OUT_AREA(msg_area);
    int size = sal_strlen(cmd) + 1; /* Including null char */
    int wsize = (size + 3) / 4;
    uint32_t *wcmd = (uint32_t *)cmd;
    int i, rv;
    mcs_msg_data_t send;

    for (i = 0; i < wsize; i++, wcmd++) {
        mcs_wr(unit, uc, con_out_data_addr + (i * 4), *wcmd);
    }

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_SYSTEM);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_SYSTEM_CON_OUT);

    rv = bcmbd_mcs_msg_send(unit, uc, &send, mdev->mcs_msg_send_timeout);

    if (waitms > 0) {
        sal_usleep(waitms * 1000);
    }

    return rv;
}

int
bcmbd_mcs_con_mode_set(int unit, int uc, bool enable)
{
    mcs_dev_t *dev = &mcs_dev[unit];
    mcs_con_t *con =  &(dev->mcs_con[uc]);

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    if (!dev->initialized) {
        return SHR_E_UNAVAIL;
    }

    if (enable) {
        con->mcs_con_buffer = sal_alloc(MCS_CON_BUF_SIZE,"bcmbdMcsConBuf");
        if (con->mcs_con_buffer == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "MCS con %d buffer alloc failed \n"), uc));
            return SHR_E_MEMORY;
        }
        con->mcs_con_buf_write_idx = 0;
        con->mcs_con_buf_read_idx = 0;

        con->mcs_con_thread = shr_thread_start("bcmbdMcsCon",
                              SAL_THREAD_PRIO_DEFAULT,
                              mcs_con_print_thread,
                              ENCODE_UNIT_UC(unit, uc));
        if (con->mcs_con_thread == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "MCS con thread %d creation failed \n"), uc));
            sal_free(con->mcs_con_buffer);
            return SHR_E_MEMORY;
        }
    } else {
        (void) shr_thread_stop(con->mcs_con_thread, 100000);
        con->mcs_con_thread = NULL;
        sal_free(con->mcs_con_buffer);
        con->mcs_con_buffer = NULL;
    }
    return SHR_E_NONE;
}

int
bcmbd_mcs_con_mode_get(int unit, int uc, bool *enabled)
{
    mcs_dev_t *dev = &mcs_dev[unit];

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    if (!dev->initialized) {
        return SHR_E_UNAVAIL;
    }

    *enabled = (dev->mcs_con[uc].mcs_con_thread == NULL) ? false : true;
    return SHR_E_NONE;
}

int
bcmbd_mcs_msg_send(int unit, int uc, mcs_msg_data_t *msg, sal_usecs_t timeout)
{
    int index, rc = SHR_E_NONE;
    uint8_t ack;
    uint32_t msg_base, area_out, addr;
    sal_sem_t ack_sem;
    mcs_msg_data_t omsg;
    mcs_dev_t *dev = &mcs_dev[unit];
    bcmbd_mcs_msg_dev_t *mdev = &(dev->mcs_msg_dev);
    sal_usecs_t start_time, end_time, time_elapsed;

    /* Figure out when to give up */
    start_time = sal_time_usecs();

    if ((mdev->sw_intr_active & (1 << uc)) == 0) {
        return SHR_E_INIT;
    }

    ack_sem = sal_sem_create("mcs_msg_send", SAL_SEM_BINARY, 0);

    msg_base = mdev->mcs_msg_area[uc];

    while (1) {                 /* Retry loop */
        rc = SHR_E_NONE;

        /* Wait for a semaphore indicating a free slot */
        if (sal_sem_take(mdev->mcs_msg_send_queue_sems[uc],
                         mdev->mcs_msg_queue_timeout)) {
            rc = SHR_E_TIMEOUT;
            break;
        }

        /* Block others for a bit */
        if (sal_mutex_take(mdev->mcs_msg_control, mdev->mcs_msg_ctl_timeout) == 0) {
            if (MCS_MSG_STATUS_STATE(mdev->mcs_msg_prev_status_out[uc]) !=
                MCS_MSG_READY_STATE) {
                sal_mutex_give(mdev->mcs_msg_control);
                rc = SHR_E_INIT;
                break;
            }

            /* Room for the message - update the local status copy */
            index = MCS_MSG_STATUS_SENT_INDEX(mdev->mcs_msg_prev_status_out[uc]);
            MCS_MSG_STATUS_SENT_INDEX_W(mdev->mcs_msg_prev_status_out[uc],
                                        MCS_MSG_INCR(index));

            mdev->mcs_msg_ack_data[uc][index] = &ack;
            mdev->mcs_msg_ack_sems[uc][index] = ack_sem;

            omsg.words[0] = msg->words[0];
            omsg.words[1] = msg->words[1];
            omsg.words[2] = msg->words[2];

            /* Update the outbound area that contains the new message */
            area_out = MCS_MSG_OUT_AREA(msg_base);
            addr = area_out + MCS_MSG_OFFSETOF(status);
            bcmdrd_hal_iproc_write(unit, addr,
                                   mdev->mcs_msg_prev_status_out[uc]);
            addr = area_out + MCS_MSG_OFFSETOF(data[0].words[0]) +
                   (MCS_MSG_DATA_SIZE * index);
            bcmdrd_hal_iproc_write(unit, addr, omsg.words[0]);
            addr = area_out + MCS_MSG_OFFSETOF(data[0].words[1]) +
                   (MCS_MSG_DATA_SIZE * index);
            bcmdrd_hal_iproc_write(unit, addr, omsg.words[1]);
            addr = area_out + MCS_MSG_OFFSETOF(data[0].words[2]) +
                   (MCS_MSG_DATA_SIZE * index);
            bcmdrd_hal_iproc_write(unit, addr, omsg.words[2]);
            addr = area_out + MCS_MSG_OFFSETOF(status);
            bcmdrd_hal_iproc_write(unit, addr, mdev->mcs_msg_prev_status_out[uc]);

            bcmbd_mcs_intr_set(unit, uc); /* Wake up call */

            /* Return the global message control mutex */
            sal_mutex_give(mdev->mcs_msg_control);

            /* Wait for the ACK semaphore to be posted indicating that
               a response has been received */
            if (sal_sem_take(ack_sem, mdev->mcs_msg_send_timeout)) {
                mdev->mcs_msg_ack_data[uc][index] = NULL;
                mdev->mcs_msg_ack_sems[uc][index] = NULL;
                rc = SHR_E_TIMEOUT;
                LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "MCS:%d sal_sem_take failed\n"), uc));
                break;
            }

            if (ack == 1) {
                rc = SHR_E_NONE;
                break;              /* Message received OK */
            }

            end_time = sal_time_usecs();
            if (end_time > start_time) {
                time_elapsed = end_time - start_time;
            } else {
                time_elapsed = SAL_USECS_TIMESTAMP_ROLLOVER -
                               start_time + end_time;
            }
            if (time_elapsed >= timeout) {
                mdev->mcs_msg_ack_data[uc][index] = NULL;
                mdev->mcs_msg_ack_sems[uc][index] = NULL;
                rc = SHR_E_TIMEOUT;
                LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "UC:%d time out.\n"), uc));
                break;              /* Timeout */
            }

            sal_usleep(mdev->mcs_msg_send_retry_delay);
        }
    }
    sal_sem_destroy(ack_sem);
    return rc;
}

int
bcmbd_mcs_msg_receive(int unit, int uc, uint8_t mclass,
                      mcs_msg_data_t *msg, int timeout)
{
    return mcs_msg_receive(unit, uc, mclass, msg, timeout, 0);
}

int
bcmbd_mcs_msg_send_receive(int unit, int uc, mcs_msg_data_t *send,
                           mcs_msg_data_t *reply, sal_usecs_t timeout)
{
    int rc;
    sal_usecs_t start = sal_time_usecs();
    sal_usecs_t now, time_elapsed;

    rc = bcmbd_mcs_msg_send(unit, uc, send, timeout);
    if (rc != SHR_E_NONE) {         /* send failed */
        return rc;
    }

    now = sal_time_usecs();
    /* Check for time rollover and adjust the timeout accordingly */
    if (now > start) {
        time_elapsed = now - start;
    } else {
        time_elapsed = SAL_USECS_TIMESTAMP_ROLLOVER - start + now;
    }

    if (timeout <= time_elapsed) {
        rc = SHR_E_TIMEOUT;
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "UC:%d time out.\n"), uc));
    } else if (rc == SHR_E_NONE) {      /* If ack'd */
        timeout -= time_elapsed;
        rc = bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_MCLASS_GET((*send)),
                                   reply, timeout);
    }

    return (rc);
}

int
bcmbd_mcs_msg_receive_cancel(int unit, int uc, uint8_t mclass)
{
    bcmbd_mcs_msg_dev_t *mdev = &(mcs_dev[unit].mcs_msg_dev);
    mcs_msg_ll_node_t *msg_node;
    ll_ctrl_t *mcs_msg_rcv_ll;

    if ((mdev->sw_intr_active & (1 << uc)) == 0) {
        return SHR_E_INIT;
    }

    /* Block others for a bit */
    if (sal_mutex_take(mdev->mcs_msg_control, mdev->mcs_msg_ctl_timeout)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "UC%d semtake - "
                              "mcs_msg_control timed out\n"), uc));
        return SHR_E_TIMEOUT;
    }

    /* Clear receive LL and set semaphore count to 0 */
    mcs_msg_rcv_ll = &mdev->mcs_msg_rcvd_ll[uc][mclass];
    while (mcs_msg_rcv_ll->ll_count) {
        msg_node = (mcs_msg_ll_node_t *)ll_remove_head(mcs_msg_rcv_ll);
        if (msg_node != NULL) {
            sal_free(msg_node);
        }

        /* Decrement the counting semaphore count to be in sync with LL */
        sal_sem_take(mdev->mcs_msg_rcv_sems[uc][mclass], 10000000);
    }

    /* Kick the waiter */
    sal_sem_give(mdev->mcs_msg_rcv_sems[uc][mclass]);

    sal_mutex_give(mdev->mcs_msg_control);
    return SHR_E_NONE;
}

int
bcmbd_mcs_uc_msg_start(int unit)
{
    int i, rv = SHR_E_NONE;
    bcmbd_mcs_msg_dev_t *mdev = &(mcs_dev[unit].mcs_msg_dev);

    if ((mdev == NULL) || (mdev->mcs_msg_control != NULL)) {
        return SHR_E_INIT;
    }

    mdev->sw_intr_active = 0;
    mdev->mcs_msg_control = NULL;

    mdev->mcs_msg_queue_timeout = 200000000;
    mdev->mcs_msg_ctl_timeout = 1000000;
    mdev->mcs_msg_send_timeout = 10000000;
    mdev->mcs_msg_send_retry_delay = 100;

    /* Init a few fields so that the threads can sort themselves out */
    mdev->mcs_msg_control= sal_mutex_create("Msgctrl");

    for (i = 0; i < COUNTOF(mdev->sw_intr); i++) {
        mdev->sw_intr[i] = NULL;
    }

    for (i = 0; i < COUNTOF(mdev->mcs_msg_active); i++) {
        /* Init the active sem and hold it */
        mdev->mcs_msg_active[i] = sal_sem_create("msgActive",
                                          SAL_SEM_BINARY, 0);
    }

    /* Init things for the system msg threads */
    mdev->mcs_msg_system_control = sal_mutex_create("SysMsgCtrl");
    mdev->mcs_msg_system_count = 0;

    return rv;
}


int
bcmbd_mcs_uc_msg_uc_start(int unit, int uc)
{
    int i, rv = SHR_E_NONE;
    bcmbd_mcs_msg_dev_t *mdev = &(mcs_dev[unit].mcs_msg_dev);
    shr_thread_t thread_uc_msg;
    shr_thread_t thread_uc_sys_msg;

    /* Check for the master control for this uC */
    if ((mdev == NULL) || ((mdev->sw_intr_active & (1 << uc)) != 0)) {
        return (SHR_E_BUSY);
    }

    /* Check if uC is in reset */
    rv = bcmbd_mcs_uc_is_reset(unit, uc, &i);
    if (SHR_FAILURE(rv) || (i)) {
        return SHR_E_INIT;
    }

    if (uc == 0) {
        /* The uC0 could be running PCIEv3 FW loader from flash.
         * use the MCS_CONFIG_SUPPORTED keyword to check
         * if we have a uKernel firmware loaded or not.
         */
        if (mcs_rd(unit, uc, MCS_CFG_FEAT_ADDR) != MCS_CONFIG_SUPPORTED) {
            return SHR_E_INIT;
        }
    }

    sal_mutex_take(mdev->mcs_msg_control, SAL_SEM_FOREVER);

    /* Init some data structures */
    mdev->mcs_msg_send_queue_sems[uc] =
            sal_sem_create("MCS msg queue", SAL_SEM_COUNTING, 0);
    if (mdev->mcs_msg_send_queue_sems[uc] == NULL) {
        sal_mutex_give(mdev->mcs_msg_control);
        return (SHR_E_MEMORY);
    }

    /* Initialize receive LL, Create receive semaphores,*/
    for (i = 0; i <= MAX_MCS_MSG_CLASS; i++) {
        mdev->mcs_msg_rcvd_ll[uc][i].p_head = NULL;
        mdev->mcs_msg_rcvd_ll[uc][i].p_tail = NULL;
        mdev->mcs_msg_rcvd_ll[uc][i].ll_count = 0;
        mdev->mcs_msg_rcv_sems[uc][i] = sal_sem_create(
                                          "us_msg_rcv", SAL_SEM_COUNTING, 0);
        mdev->mcs_msg_appl_cb[uc][i] = NULL;
        mdev->mcs_msg_appl_cb_data[uc][i] = NULL;
    }

    /* Clear Ack Semaphores and data*/
    for (i = 0; i < NUM_MCS_MSG_SLOTS; i++) {
        mdev->mcs_msg_ack_sems[uc][i] = NULL;
        mdev->mcs_msg_ack_data[uc][i] = NULL;
    }

    mdev->sw_intr_active |= (1 << uc);   /* This uC is active */

    thread_uc_msg = shr_thread_start("bcmbdMcsMsg",
                          MCS_THREAD_PRI,
                          mcs_msg_thread,
                          ENCODE_UNIT_UC(unit, uc));
    if (thread_uc_msg == NULL) {
        sal_mutex_give(mdev->mcs_msg_control);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "MCS msg thread %d creation failed \n"), uc));
        return SHR_E_MEMORY;
    }

    thread_uc_sys_msg = shr_thread_start("bcmbdMcsSystemMsg",
                          MCS_SYS_THREAD_PRI,
                          mcs_msg_system_thread,
                          ENCODE_UNIT_UC(unit, uc));
    if (thread_uc_sys_msg == NULL) {
        (void) shr_thread_stop(thread_uc_msg, 100000);
        sal_mutex_give(mdev->mcs_msg_control);
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                      "MCS system msg thread %d creation failed \n"),
                   uc));
        return SHR_E_MEMORY;
    }

    sal_mutex_give(mdev->mcs_msg_control);

    if (mcs_msg_active_wait(unit, uc) != SHR_E_NONE) {
        /* Down and not coming back up */
        return (SHR_E_UNAVAIL);
    }

    return rv;
}


int
bcmbd_mcs_uc_msg_uc_stop(int unit, int uc)
{
    int i;
    bcmbd_mcs_msg_dev_t *mdev = &(mcs_dev[unit].mcs_msg_dev);

    /* Check for the master control for this uC */
    if ((mdev == NULL) || ((mdev->sw_intr_active & (1 << uc)) == 0)) {
        return (SHR_E_INIT);
    }

    sal_mutex_take(mdev->mcs_msg_control, SAL_SEM_FOREVER);
    mdev->sw_intr_active &= ~(1 << uc);   /* This uC is inactive */

    if (mdev->sw_intr[uc] != NULL) {
        sal_sem_give(mdev->sw_intr[uc]); /* Kick msg thread */
    }

    while (1) {
        if (mdev->sw_intr[uc] == NULL) {
            break;                      /* It is shut down */
        }
        sal_mutex_give(mdev->mcs_msg_control);
        sal_thread_yield();
        sal_mutex_take(mdev->mcs_msg_control, SAL_SEM_FOREVER);
    }

    /* Destroy receive semaphores */
    for (i = 0; i <= MAX_MCS_MSG_CLASS; i++) {
       if (mdev->mcs_msg_rcv_sems[uc][i] != NULL) {
           sal_sem_give(mdev->mcs_msg_rcv_sems[uc][i]);
           sal_thread_yield();
           sal_sem_destroy(mdev->mcs_msg_rcv_sems[uc][i]);
           mdev->mcs_msg_rcv_sems[uc][i] = NULL;
       }
    }

    sal_mutex_give(mdev->mcs_msg_control);

    return (SHR_E_NONE);
}

void
bcmbd_mcs_isr(int unit, uint32_t uc)
{
    mcs_dev_t *dev = &mcs_dev[unit];
    bcmbd_mcs_msg_dev_t *mdev = &(dev->mcs_msg_dev);

    if (mdev->sw_intr[uc]) {
        sal_sem_give(mdev->sw_intr[uc]);
    }
}

int
bcmbd_mcs_init(int unit, int num_uc, uint32_t *msg_areas)
{
    int uc;
    mcs_dev_t *dev = &mcs_dev[unit];
    uint32_t *msg_base = msg_areas;

    SHR_FUNC_ENTER(unit);

    sal_memset(dev, 0, sizeof(*dev));
    dev->mcs_num_uc = num_uc;
    for (uc = 0; uc < num_uc; uc++,msg_base++) {
        dev->mcs_msg_dev.mcs_msg_area[uc] = *msg_base;
    }

    dev->initialized = true;

    SHR_FUNC_EXIT();
}

int
bcmbd_mcs_cleanup(int unit)
{
    mcs_dev_t *dev = &mcs_dev[unit];

    SHR_FUNC_ENTER(unit);

    dev->initialized = false;

    SHR_FUNC_EXIT();
}

int
bcmbd_mcs_uc_status(int unit, int uc, bcmbd_ukernel_info_t *info)
{
    mcs_dev_t *dev = &mcs_dev[unit];
    const bcmdrd_chip_info_t *cinfo;
    int i, isRst, rv = SHR_E_NONE;
    mcs_msg_data_t send, rcv;
    uint8_t *version_buf = NULL;
    uint64_t version_buff_paddr;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    if (!dev->initialized) {
        return SHR_E_UNAVAIL;
    }

    rv = bcmbd_mcs_uc_is_reset(unit, uc, &isRst);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    if (isRst) {
        sal_strncpy(info->status, "Reset", MCS_INFO_STATUS_SIZE);
        return rv;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_UNAVAIL;
    }

    if (uc == 0) {
        /* The uC0 could be running PCIEv3 FW loader from flash.
         * use the MCS_CONFIG_SUPPORTED keyword to check
         * if we have a uKernel firmware loaded or not.
         */
        if (mcs_rd(unit, uc, MCS_CFG_FEAT_ADDR) != MCS_CONFIG_SUPPORTED) {
            sal_strncpy(info->status, "Invalid FW", MCS_INFO_STATUS_SIZE);
            return SHR_E_INIT;
        }
    }

    info->cpsr = mcs_rd(unit, uc, MCS_INF_CPSR_ADDR);
    if (info->cpsr) {
        sal_strncpy(info->status, "Fault", MCS_INFO_STATUS_SIZE);
        /* Get debug values from the Firmware */
        info->type = mcs_rd(unit, uc, MCS_INF_EX_TYPE_ADDR);
        info->dfsr = mcs_rd(unit, uc, MCS_INF_DFSR_ADDR);
        info->dfar = mcs_rd(unit, uc, MCS_INF_DFAR_ADDR);
        info->ifsr = mcs_rd(unit, uc, MCS_INF_IFSR_ADDR);
        info->ifar = mcs_rd(unit, uc, MCS_INF_IFAR_ADDR);
        /* Get copy of R0-R15 */
        for (i = 0; i < 16; i++) {
            info->armreg[i] = mcs_rd(unit, uc, (MCS_INF_R0_ADDR + (i*4)));
        }
    } else {
        sal_strncpy(info->status, "OK", MCS_INFO_STATUS_SIZE);
        info->fw_version[0] = 0;
        version_buf =  bcmdrd_hal_dma_alloc(unit, 256,
                                            "uKernel ver", &version_buff_paddr);
        if (version_buf) {
            MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_SYSTEM);
            MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_SYSTEM_VERSION);
            MCS_MSG_LEN_SET(send, 256);
            MCS_MSG_DATA_SET(send, ((uint32_t)(version_buff_paddr & 0xffffffff)));
            MCS_MSG_DATA1_SET(send, ((uint32_t)((version_buff_paddr >> 32) & 0xffffffff)));
            rv = bcmbd_mcs_msg_send(unit, uc, &send,
                                    dev->mcs_msg_dev.mcs_msg_send_timeout);
            if (SHR_SUCCESS(rv)) {
                rv = bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_VERSION,
                                           &rcv, 5*1000*1000);
                if (SHR_SUCCESS(rv)) {
                    sal_strlcpy(info->fw_version, (char *)version_buf, 100);
                }
            }
            bcmdrd_hal_dma_free(unit, 256, version_buf, version_buff_paddr);
        }
    }

    return rv;
}

int
bcmbd_mcs_num_uc(int unit)
{
    mcs_dev_t *dev = &mcs_dev[unit];

    return dev->mcs_num_uc;
}

int
bcmbd_mcs_appl_cb_register(int unit, int uc, int msg_class,
                           bcmbd_mcs_appl_cb_t *cb_fn, void *cb_data)
{
    bcmbd_mcs_msg_dev_t *mdev = &(mcs_dev[unit].mcs_msg_dev);

    mdev->mcs_msg_appl_cb[uc][msg_class] = cb_fn;
    mdev->mcs_msg_appl_cb_data[uc][msg_class] = cb_data;

    return SHR_E_NONE;
}
