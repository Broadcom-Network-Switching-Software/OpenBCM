/*! \file bcmbd_schanfifo_internal.h
 *
 * SCHAN FIFO data types used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_SCHANFIFO_INTERNAL_H
#define BCMBD_SCHANFIFO_INTERNAL_H

#include <sal/sal_types.h>

struct schanfifo_ctrl_s;

/*! Get information. */
typedef int (*schanfifo_info_get_t)(struct schanfifo_ctrl_s *ctrl,
                                    int *num_chans, size_t *cmd_mem_wsize);
/*! Initialize device. */
typedef int (*schanfifo_init_t)(struct schanfifo_ctrl_s *ctrl,
                                uint32_t max_polls, uint32_t flags);
/*! Send operations. */
typedef int (*schanfifo_ops_send_t)(struct schanfifo_ctrl_s *ctrl,
                                    int chan, uint32_t num_ops,
                                    uint32_t *req_buff, size_t req_wsize,
                                    uint64_t buff_paddr, uint32_t flags);
/*! Set start. */
typedef int (*schanfifo_set_start_t)(struct schanfifo_ctrl_s *ctrl,
                                     int chan, bool start);
/*! Get status. */
typedef int (*schanfifo_status_get_t)(struct schanfifo_ctrl_s *ctrl,
                                      int chan, uint32_t num_ops, uint32_t flags,
                                      uint32_t *done_ops, uint32_t **resp_buff);

/*!
 * \brief Exported operation functions.
 */
typedef struct schanfifo_ops_s {
    /*! Get information. */
    schanfifo_info_get_t        info_get;
    /*! Initialize device. */
    schanfifo_init_t            dev_init;
    /*! Send operations. */
    schanfifo_ops_send_t        ops_send;
    /*! Set start. */
    schanfifo_set_start_t       start_set;
    /*! Get status. */
    schanfifo_status_get_t      status_get;
} schanfifo_ops_t;

/*!
 * \brief SCHAN FIFO device control.
 */
typedef struct schanfifo_ctrl_s {
    /*! Device number. */
    int unit;

    /*! Number of channels. */
    int channels;

    /*! Size of command memory in 32-bit words. */
    size_t cmds_wsize;

    /*! Pointer to the exported funtions structure. */
    schanfifo_ops_t *ops;

    /*! Indicate if device is attached. */
    int active;
} schanfifo_ctrl_t;

#endif /* BCMBD_SCHANFIFO_INTERNAL_H */
