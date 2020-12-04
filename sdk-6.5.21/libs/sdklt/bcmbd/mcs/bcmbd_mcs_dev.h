/*! \file bcmbd_mcs_dev.h
 *
 * Local definitions for MCS device driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_MCS_DEV_H
#define BCMBD_MCS_DEV_H

#include <sal/sal_time.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>

#include <bcmbd/mcs_shared/mcs_msg_common.h>
/* Structures for UC message receive Linked List. */

/*! linked list element */
typedef struct ll_element_s {
    /*! Next pointer */
    struct ll_element_s *p_next;
    /*! Previous pointer */
    struct ll_element_s *p_prev;
} ll_element_t;

/*! Doubly linked list ctrl structure */
typedef struct ll_ctrl_s {
    /*! pointer to the first element */
    ll_element_t    *p_head;
    /*! pointer to the last element */
    ll_element_t    *p_tail;
    /*! number of elements in the list */
    uint32_t          ll_count;
} ll_ctrl_t;

/*! linked list node */
typedef struct mcs_msg_ll_node_s {
    /*! LL linkage */
    ll_element_t  msg_q;
    /*! Msg Data */
    mcs_msg_data_t  msg_data;
} mcs_msg_ll_node_t;

/*!
 * \brief MCS device structure.
 */
typedef struct bcmbd_mcs_msg_dev_s {
    /*! uC active flag. */
    int sw_intr_active;
    /*! S/W Interrupt Semaphore */
    sal_sem_t sw_intr[MCS_NUM_UC];
    /*! Base addresses of the uC Messaging areas. */
    uint32_t mcs_msg_area[MCS_NUM_UC];
    /*! MCS message lock. */
    sal_mutex_t mcs_msg_control;
    /*! MCS message active semaphore */
    sal_sem_t mcs_msg_active[MCS_NUM_UC];
    /*! MCS message receive semaphores */
    sal_sem_t mcs_msg_rcv_sems[MCS_NUM_UC][MAX_MCS_MSG_CLASS + 1];
    /*! MCS message ACK semaphores. */
    sal_sem_t mcs_msg_ack_sems[MCS_NUM_UC][NUM_MCS_MSG_SLOTS];
    /*! Linked list for storing messages received per class. */
    ll_ctrl_t mcs_msg_rcvd_ll[MCS_NUM_UC][MAX_MCS_MSG_CLASS + 1];
    /*! MCS message ACK data buffer. */
    uint8_t *mcs_msg_ack_data[MCS_NUM_UC][NUM_MCS_MSG_SLOTS];
    /*! Semaphores for senders waiting for a free slot. */
    sal_sem_t mcs_msg_send_queue_sems[MCS_NUM_UC];
    /*! MCS message previous IN status. */
    mcs_msg_host_status_t mcs_msg_prev_status_in[MCS_NUM_UC];
    /*! MCS message previous OUT status. */
    mcs_msg_host_status_t mcs_msg_prev_status_out[MCS_NUM_UC];
    /*! Free slot timeout. */
    sal_usecs_t mcs_msg_queue_timeout;
    /*! Control semaphore timeout. */
    sal_usecs_t mcs_msg_ctl_timeout;
    /*! ACK/NAK wait timeout. */
    sal_usecs_t mcs_msg_send_timeout;
    /*! Delay between send retries. */
    sal_usecs_t mcs_msg_send_retry_delay;
    /*! System-type message control. */
    sal_mutex_t mcs_msg_system_control;
    /*! System-type message count (incremented by thread). */
    uint8_t mcs_msg_system_count;
    /*! Callback for uC applications on shutdown. */
    bcmbd_mcs_appl_cb_t *mcs_msg_appl_cb[MCS_NUM_UC][MAX_MCS_MSG_CLASS + 1];
    /*! Data for Callback for uC applications on shutdown. */
    void *mcs_msg_appl_cb_data[MCS_NUM_UC][MAX_MCS_MSG_CLASS + 1];
} bcmbd_mcs_msg_dev_t;

/*! MCS console ring buffer size */
#define MCS_CON_BUF_SIZE 1024

/*!
 * \brief MCS console structure.
 */
typedef struct mcs_con_s {
    /*! MCS console thread. */
    shr_thread_t mcs_con_thread;
    /*! MCS console ring buffer. */
    char *mcs_con_buffer;
    /*! MCS console ring buffer write index. */
    uint32_t mcs_con_buf_write_idx;
    /*! MCS console ring buffer read index. */
    uint32_t mcs_con_buf_read_idx;
} mcs_con_t;

/*!
 * \brief MCS device structure.
 */
typedef struct mcs_dev_s {
    /*! Number of MCS Processors. */
    int mcs_num_uc;
    /*! MCS messaging driver */
    bcmbd_mcs_msg_dev_t mcs_msg_dev;
    /*! MCS host console. */
    mcs_con_t mcs_con[MCS_NUM_UC];
    /*! Device initialization indicator. */
    bool initialized;
} mcs_dev_t;

#endif /* BCMBD_MCS_DEV_H */
