/*! \file bcmbd_m0ssq_mbox.c
 *
 * M0SSQ MBOX(mailbox) driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_debug.h>

#include <sal/sal_types.h>
#include <sal/sal_sleep.h>

#include <bcmbd/bcmbd_m0ssq_internal.h>
#include <bcmbd/bcmbd_m0ssq_fifo_internal.h>
#include <bcmbd/bcmbd_m0ssq_mbox_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

#define MBOX_HDR_SIZE     (sizeof(bcmbd_m0ssq_mbox_int_msg_t))
#define MBOX_ENT_SIZE                      sizeof(uint32_t)
#define MBOX_MAX_RX_MSGS_PER_CALL          200
#define MBOX_TX_TIMEOUT_CNT                1000

/*!
 * Mbox configuration is stored M0SSQ SRAM.
 * Offset of each mbox configuration are listed below
 */
#define U0_MBOX_BASE_OFFSET                 0x0
#define U0_MBOX_SIZE_OFFSET                 0x4
#define U1_MBOX_BASE_OFFSET                 0x8
#define U1_MBOX_SIZE_OFFSET                 0xc
#define U2_MBOX_BASE_OFFSET                 0x10
#define U2_MBOX_SIZE_OFFSET                 0x14
#define U3_MBOX_BASE_OFFSET                 0x18
#define U3_MBOX_SIZE_OFFSET                 0x1c
#define MBOX_INTERRUPT_MODE_OFFSET          0x20
#define MAX_MBOX_MBOX_PER_CORE_OFFSET       0x24

/*! Size of MBOX configuration space. */
#define MBOX_SRAM_FWREG_SIZE                256

/*! Total size of all MBOXs. */
#define MBOX_SRAM_TOTAL_SIZE                (4 * 4096)

/*! Invalid uC number. */
#define MBOX_INVALID_UC                     ((uint32_t) -1)

/*******************************************************************************
 * Local variables
 */

static bcmbd_m0ssq_mbox_dev_t *m0ssq_mbox_dev[BCMDRD_CONFIG_MAX_UNITS] = { NULL };

/*******************************************************************************
 * Private functions
 */

static void
m0ssq_mbox_msg_resp_free(bcmbd_m0ssq_mbox_int_msg_t **int_msg,
                         bcmbd_m0ssq_mbox_int_msg_t **int_resp)
{
    if ((int_msg != NULL) && (*int_msg != NULL))
    {
        sal_free(*int_msg);
    }

    if ((int_resp != NULL) && (*int_resp != NULL))
    {
        sal_free(*int_resp);
    }
}

static int
m0ssq_mbox_msg_resp_alloc(bcmbd_m0ssq_mbox_int_msg_t **int_msg,
                          bcmbd_m0ssq_mbox_int_msg_t **int_resp,
                          uint32_t size)
{
    if (int_msg == NULL) {
        return SHR_E_FAIL;
    }

    *int_msg = sal_alloc(size, "bcmbdM0ssqRecvBuff");
    if (*int_msg == NULL) {
        return SHR_E_FAIL;
    }

    if (int_resp != NULL) {
        *int_resp = sal_alloc(size, "bcmbdM0ssqRespBuff");
        if (*int_resp == NULL) {
            sal_free(*int_msg);
            *int_msg = NULL;
            return SHR_E_FAIL;
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Built-in software interrupt handler for MBOX.
 */
static void
bcmbd_m0ssq_mbox_built_in_rxmsg_intr(int unit, uint32_t param)
{
    uint32_t id;
    mbox_t *mbox;
    bcmbd_m0ssq_mbox_dev_t *dev;
    uint32_t uc = param;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return;
    }

    dev = m0ssq_mbox_dev[unit];
    if (dev == NULL ||
        dev->init_done == 0)
    {
        return;
    }

    for (id = 0; id < dev->num_of_mbox; id++)
    {
        mbox = &dev->mbox[id];
        if (mbox->uc == MBOX_INVALID_UC ||
            mbox->uc != uc ||
            mbox->inuse == 0 ||
            mbox->recv_msg_handler == NULL)
        {
            continue;
        }

        bcmbd_m0ssq_mbox_process_recv_msgs(unit, id);
    }
}

/*******************************************************************************
 * Public functions shared by M0SSQ and MBOX drivers.
 */

int
bcmbd_m0ssq_mbox_dev_get(int unit, bcmbd_m0ssq_mbox_dev_t **dev)
{
    if (m0ssq_mbox_dev[unit] == NULL ||
        m0ssq_mbox_dev[unit]->init_done == 0)
    {
        return SHR_E_FAIL;
    }

    *dev = m0ssq_mbox_dev[unit];

    return SHR_E_NONE;
}

int
bcmbd_m0ssq_mbox_init(int unit, bcmbd_m0ssq_mem_t *mem, uint32_t num_of_mbox)
{
    uint32_t type;
    uint32_t zero;
    uint32_t mbox_base, mbox_size, mbox_total_size;
    uint32_t mbox_id;
    int rv;
    bcmbd_m0ssq_mbox_dev_t *dev;
    bcmbd_m0ssq_mem_t new_mem;
    bcmbd_m0ssq_fifo_t *fifo;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    if (m0ssq_mbox_dev[unit] != NULL &&
        m0ssq_mbox_dev[unit]->init_done)
    {
        return SHR_E_NONE;
    }

    dev = sal_alloc(sizeof(bcmbd_m0ssq_mbox_dev_t), "bcmbdM0ssqMboxDev");
    if (dev == NULL) {
        return SHR_E_MEMORY;
    }

    dev->num_of_mbox = num_of_mbox;
    mbox_total_size = MBOX_SRAM_TOTAL_SIZE;
    mbox_base = mem->base + MBOX_SRAM_FWREG_SIZE;
    mbox_size = mbox_total_size / num_of_mbox / MBOX_CHN_TYPE_MAX;

    /* Write the mailbox information into M0SSQ shared SRAM. */
    mem->write32(mem, U0_MBOX_BASE_OFFSET, &mbox_base, 4);
    mem->write32(mem, U0_MBOX_SIZE_OFFSET, &mbox_total_size, 4);
    mem->write32(mem, MAX_MBOX_MBOX_PER_CORE_OFFSET, &num_of_mbox, 4);
    mem->write32(mem, MBOX_INTERRUPT_MODE_OFFSET, &zero, 4);

    /*
     * Chunk input memory block into smaller ones and
     * init each smaller memory block as fifo.
     */

    /* Create a new memory object by inherit the input memroy object. */
    sal_memcpy(&new_mem, mem, sizeof(bcmbd_m0ssq_mem_t));

    for (mbox_id = 0; mbox_id < dev->num_of_mbox; mbox_id++) {
        for (type = 0; type < MBOX_CHN_TYPE_MAX; type++) {
             fifo = &(dev->mbox[mbox_id].chan[type]);

             /* Initialize a new memory object with new base and size. */
             new_mem.base  = mbox_base;
             new_mem.size  = mbox_size;

             /* Initialize a FIFO with the new memory object and entry size. */
             rv = bcmbd_m0ssq_fifo_init(fifo, &new_mem, MBOX_ENT_SIZE);
             if (SHR_FAILURE(rv)) {
                 LOG_ERROR(BSL_LOG_MODULE,
                           (BSL_META_U(unit,
                                       "FIFO init fail for mailbox %d channel %d\n"),
                            mbox_id, type));
                 return SHR_E_FAIL;
             };

             /* Increase the base address for next memory object. */
             mbox_base += mbox_size;
        }
        dev->mbox[mbox_id].recv_msg_handler = NULL;
        dev->mbox[mbox_id].uc = MBOX_INVALID_UC;
        dev->mbox[mbox_id].inuse = 0;
    }

    /* Install MBOX driver-specific data and mark it is done. */
    dev->mbox_size = mbox_size;
    dev->init_done = 1;
    m0ssq_mbox_dev[unit] = dev;

    return SHR_E_NONE;
}

int
bcmbd_m0ssq_mbox_cleanup(int unit)
{
    uint32_t mbox_id, type;
    bcmbd_m0ssq_mbox_dev_t *dev = m0ssq_mbox_dev[unit];
    bcmbd_m0ssq_fifo_t *fifo;

    for (mbox_id = 0; mbox_id < dev->num_of_mbox; mbox_id++) {
        if (dev->mbox[mbox_id].uc != MBOX_INVALID_UC) {
            /* Stop uCs' software interrupt. */
            bcmbd_m0ssq_uc_swintr_enable_set(unit, dev->mbox[mbox_id].uc, 0);

            /* Uninstall uCs' software interrupt handler. */
            bcmbd_m0ssq_uc_swintr_handler_set(unit, dev->mbox[mbox_id].uc,
                                              NULL, MBOX_INVALID_UC);
        }

        /* Uninstall MBOX RX message handler. */
        bcmbd_m0ssq_mbox_msg_handler_set(unit, mbox_id, NULL);

        /* Cleanup FIFO resources. */
        for (type = 0; type < MBOX_CHN_TYPE_MAX; type++) {
            fifo = &(dev->mbox[mbox_id].chan[type]);
            bcmbd_m0ssq_fifo_cleanup(fifo);
        }
    }

    SHR_FREE(m0ssq_mbox_dev[unit]);

    return SHR_E_NONE;
}

/*******************************************************************************
 * Public Functions
 */

int
bcmbd_m0ssq_mbox_alloc(int unit, const char *mbox_name, uint32_t *mbox_id)
{
    bcmbd_m0ssq_mbox_dev_t *dev;
    uint32_t id;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = m0ssq_mbox_dev[unit];
    SHR_NULL_CHECK(dev, SHR_E_FAIL);

    /* Static allocation */
    if (sal_strncmp(mbox_name, "led", sal_strlen("led")) == 0) {
        id = 0;
    } else if (sal_strncmp(mbox_name, "linkscan", sal_strlen("linkscan")) == 0) {
        id = 1;
    } else {
        
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    if (dev->mbox[id].inuse) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    *mbox_id = id;
    dev->mbox[id].inuse = 1;

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_mbox_free(int unit, uint32_t mbox_id)
{
    bcmbd_m0ssq_mbox_dev_t *dev;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = m0ssq_mbox_dev[unit];
    SHR_NULL_CHECK(dev, SHR_E_FAIL);

    if (dev->mbox[mbox_id].inuse) {
        dev->mbox[mbox_id].inuse = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_m0ssq_mbox_msg_handler_set(int unit,
                                 uint32_t mbox_id,
                                 bcmbd_m0ssq_mbox_msg_handler_f msg_handler)
{
    bcmbd_m0ssq_mbox_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = m0ssq_mbox_dev[unit];
    if (dev == NULL) {
        return SHR_E_FAIL;
    }

    /* Install mbox message handler */
    dev->mbox[mbox_id].recv_msg_handler = msg_handler;

    return SHR_E_NONE;
}

int
bcmbd_m0ssq_mbox_uc_swintr_attach(int unit, uint32_t mbox_id,
                                  uint32_t uc)
{
    bcmbd_m0ssq_mbox_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = m0ssq_mbox_dev[unit];
    if (dev == NULL) {
        return SHR_E_FAIL;
    }

    /* Attach uC into mbox */
    dev->mbox[mbox_id].uc = uc;

    /* Install built-in software interrupt handler for MBOX. */
    return bcmbd_m0ssq_uc_swintr_handler_set(unit, uc,
                                             bcmbd_m0ssq_mbox_built_in_rxmsg_intr,
                                             uc);
}

int
bcmbd_m0ssq_mbox_process_recv_msgs(int unit, uint32_t mbox_id)
{
    uint32_t size;
    bcmbd_m0ssq_mbox_dev_t *dev;
    mbox_t *mbox;
    bcmbd_m0ssq_fifo_t *fifo;
    bcmbd_m0ssq_mbox_int_msg_t *int_msg = NULL;
    bcmbd_m0ssq_mbox_int_msg_t *int_resp = NULL;
    bcmbd_m0ssq_mbox_msg_t msg;
    bcmbd_m0ssq_mbox_resp_t resp;
    uint32_t msg_cnt;
    int ret, rv;
    uint32_t resp_pos;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit))
    {
        return SHR_E_UNIT;
    }

    dev = m0ssq_mbox_dev[unit];
    if (dev == NULL) {
        return SHR_E_INIT;
    }

    /* Get MBOX by ID. */
    mbox = &dev->mbox[mbox_id];

    /* Get MBOX RX channel. */
    fifo = &mbox->chan[MBOX_CHN_TYPE_RX];

    /* One FIFO can be used by only one thread. Lock it before use.*/
    bcmbd_m0ssq_fifo_lock(fifo);

    /* Allocate memory for mailbox message and response. */
    SHR_IF_ERR_EXIT
        (m0ssq_mbox_msg_resp_alloc(&int_msg, &int_resp,
                                   fifo->size));

    /* Start to receive messages. Up to MBOX_MAX_RX_MSGS_PER_CALL messages
     * can be received per call.
     */
    for (msg_cnt = 0; msg_cnt < MBOX_MAX_RX_MSGS_PER_CALL; msg_cnt++)
    {
        /* Read message header. */
        rv = bcmbd_m0ssq_fifo_peek_read(fifo,
                                        int_msg,
                                        MBOX_HDR_SIZE);
        if (rv == SHR_E_EMPTY) {

            /* FIFO is empty. */
            SHR_ERR_EXIT(rv);
        } else {

            /* Return if there is any other error. */
            SHR_IF_ERR_EXIT(rv);
        }

        size = int_msg->size * MBOX_ENT_SIZE + MBOX_HDR_SIZE;

        if (int_msg->flags & MBOX_RESP_REQUIRED) {
            resp_pos = bcmbd_m0ssq_fifo_rp_get(fifo);
        } else {
            resp_pos = 0;
        }

        /* Read entire message. */
        SHR_IF_ERR_EXIT
            (bcmbd_m0ssq_fifo_peek_read(fifo, int_msg, size));

        msg.id = int_msg->id;
        msg.datalen = int_msg->size * MBOX_ENT_SIZE;
        msg.data = int_msg->data;
        resp.datalen = 0;
        resp.data = int_resp->data;

        if (resp_pos) { /* If message response is required. */

            /* Invoke customer handler to process message. */
            ret = SHR_E_FAIL;
            if (mbox->recv_msg_handler != NULL) {
                ret = mbox->recv_msg_handler(unit, &msg, &resp);
            }

            /* Prepare response to send back. */
            int_resp->id = int_msg->id;
            int_resp->flags = int_msg->flags;
            int_resp->size = (resp.datalen + MBOX_ENT_SIZE - 1) / MBOX_ENT_SIZE;

            if (int_msg->size >= int_resp->size && (ret == SHR_E_NONE)) {
                int_resp->size = (resp.datalen + MBOX_ENT_SIZE - 1) / MBOX_ENT_SIZE;
                sal_memcpy(int_resp->data, resp.data, resp.datalen);

                /* Write the reponse data into FIFO */
                size = MBOX_HDR_SIZE + int_resp->size * MBOX_ENT_SIZE;
                SHR_IF_ERR_EXIT(bcmbd_m0ssq_fifo_pos_write(fifo,
                                                        resp_pos,
                                                        int_resp,
                                                        size));

                /* Mark the response flag as success and ready. */
                int_resp->flags &= ~(MBOX_RESP_REQUIRED);
                int_resp->flags |= (MBOX_RESP_SUCCESS | MBOX_RESP_READY);
            } else {
                if (msg.datalen >= MBOX_ENT_SIZE) {
                    int_resp->size = 1;
                    int_resp->data[0] = ret;
                } else {
                    int_resp->size = 0;
                }

                /* Write the reponse data into FIFO */
                size = MBOX_HDR_SIZE + int_resp->size * MBOX_ENT_SIZE;
                SHR_IF_ERR_EXIT(bcmbd_m0ssq_fifo_pos_write(fifo, resp_pos,
                                                           int_resp,
                                                           size));

                /* Mark the response flag as ready only. */
                int_resp->flags &= ~(MBOX_RESP_REQUIRED);
                int_resp->flags |= (MBOX_RESP_READY);
            }

            /* Update the flag field in last step of response. */
            SHR_IF_ERR_EXIT(bcmbd_m0ssq_fifo_pos_write(fifo,
                                                       resp_pos,
                                                       int_resp,
                                                       sizeof(uint32_t)));

        } else {  /* If message response is not required. */

            /* Invoke customer handler to process message. */
            if (mbox->recv_msg_handler != NULL) {
                if (SHR_FAILURE(mbox->recv_msg_handler(unit, &msg, NULL))) {
                    LOG_ERROR(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                              "Messgage handling failed for mbox %d msg id %d\n"),
                              mbox_id, msg.id));
                };
            }
        };

        /* Message process complete, make rp move forward. */
        size = MBOX_HDR_SIZE + int_msg->size * MBOX_ENT_SIZE;
        bcmbd_m0ssq_fifo_rp_forward(fifo, size);
    }

exit:
    /* Free allocated memory in this function. */
    m0ssq_mbox_msg_resp_free(&int_msg, &int_resp);

    /* Release FIFO. */
    bcmbd_m0ssq_fifo_unlock(fifo);

    SHR_FUNC_EXIT();
}


int
bcmbd_m0ssq_mbox_msg_send(int unit, uint32_t mbox_id,
                          bcmbd_m0ssq_mbox_msg_t *msg,
                          bcmbd_m0ssq_mbox_resp_t *resp)
{
    uint32_t size;
    int timeout_cnt = MBOX_TX_TIMEOUT_CNT;
    uint32_t wp = 0;
    bool require_response = (resp != NULL);
    bcmbd_m0ssq_fifo_t *fifo;
    bcmbd_m0ssq_mbox_dev_t *dev;
    bcmbd_m0ssq_mbox_int_msg_t *int_msg = NULL;
    bcmbd_m0ssq_mbox_int_msg_t *int_resp = NULL;

    SHR_FUNC_ENTER(unit);

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    /* Response data length is less than or
     * equal to messsage data length.
     */
    if ((msg == NULL) || (msg->datalen % 4))
    {
        return SHR_E_PARAM;
    }

    dev = m0ssq_mbox_dev[unit];
    if (dev == NULL ||
        msg == NULL ||
        dev->mbox[mbox_id].inuse == 0)
    {
        return SHR_E_FAIL;
    };

    /* Get MBOX TX channel. */
    fifo  = &dev->mbox[mbox_id].chan[MBOX_CHN_TYPE_TX];

    /* One FIFO can be used by only one thread. Lock it before use.*/
    bcmbd_m0ssq_fifo_lock(fifo);

    /* Check if there is enough free space to write. */
    size = MBOX_HDR_SIZE + msg->datalen;
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_fifo_write_check(fifo, size));

    /* Allocate memory for mailbox message and response. */
    SHR_IF_ERR_EXIT
        (m0ssq_mbox_msg_resp_alloc(&int_msg, &int_resp, size));


    /* Convert user message to internal mailbox message. */
    int_msg->id = msg->id;
    int_msg->size = (msg->datalen) / sizeof(uint32_t);
    if (msg->data) {
        sal_memcpy(int_msg->data, msg->data, msg->datalen);
    }

    /* Setup message flags if response is required. */
    if (require_response) {
        /*  Response is required. Mark relative flags. */
        int_msg->flags = MBOX_SYNC_MSG | MBOX_RESP_REQUIRED;

        /* Get current write pointer for message response*/
        wp = bcmbd_m0ssq_fifo_wp_get(fifo);

    } else {
        /*  Response is not required. Mark relative flags. */
        int_msg->flags = MBOX_ASYNC_STATUS;
    }

    /* Send message out. */
    SHR_IF_ERR_EXIT
        (bcmbd_m0ssq_fifo_write(fifo, (uint32_t *) int_msg, size));

    /* Exit if reponse is not required. */
    if (!require_response) {
        SHR_EXIT();
    }

    /* Wait for response with a timeout. */
    while (timeout_cnt) {

        /* Try read response from FIFO. */
        SHR_IF_ERR_EXIT
            (bcmbd_m0ssq_fifo_pos_read(fifo, wp,
                                       (uint32_t *) int_resp,
                                       size));

        /* Check if the response is ready. */
        if (int_resp->flags & MBOX_RESP_READY) {
            if (int_resp->flags & MBOX_RESP_SUCCESS) {
                if (resp->data && resp->datalen) {
                   sal_memcpy(resp->data, int_resp->data, resp->datalen);
                }
                LOG_DEBUG(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Response ready %d\n"), int_msg->id));
                SHR_EXIT();
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Response a failure for mbox %d msg id %d\n"),
                                     mbox_id, int_msg->id));
                if (int_resp->size && int_resp->data[0] != SHR_E_NONE) {
                    SHR_ERR_EXIT(int_resp->data[0]);
                } else {
                    SHR_ERR_EXIT(SHR_E_FAIL);
                }
            }
        }

        sal_usleep(100);
        timeout_cnt--;
    }

    if (!timeout_cnt) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "No response for msg %d\n"), msg->id));
        SHR_ERR_EXIT(SHR_E_TIMEOUT);
    }

exit:
    /* Free allocated memory in this function. */
    m0ssq_mbox_msg_resp_free(&int_msg, &int_resp);

    /* Release FIFO. */
    bcmbd_m0ssq_fifo_unlock(fifo);

    SHR_FUNC_EXIT();
}

