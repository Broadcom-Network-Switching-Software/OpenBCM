/*! \file bcmbd_schanfifo.c
 *
 * SCHAN FIFO driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal_config.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_schanfifo.h>
#include <bcmbd/bcmbd_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_SCHANFIFO

static schanfifo_ctrl_t schanfifo_ctrl[SCHANFIFO_DEV_NUM_MAX];

int
bcmbd_schanfifo_attach(int unit)
{
    schanfifo_ctrl_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    ctrl = bcmbd_schanfifo_ctrl_get(unit);
    if (!ctrl) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(ctrl, 0, sizeof(*ctrl));
    ctrl->unit = unit;
    ctrl->active = 1;

exit:
    SHR_FUNC_EXIT();
}

int
bcmbd_schanfifo_detach(int unit)
{
    schanfifo_ctrl_t *ctrl = &schanfifo_ctrl[unit];

    if (!ctrl->active) {
        return SHR_E_NONE;
    }

    ctrl->active = 0;

    return SHR_E_NONE;
}

int
bcmbd_schanfifo_info_get(int unit, int *num_chans, size_t *cmd_mem_wsize)
{
    schanfifo_ctrl_t *ctrl = &schanfifo_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (!num_chans || !cmd_mem_wsize) {
        return SHR_E_PARAM;
    }

    return ctrl->ops->info_get(ctrl, num_chans, cmd_mem_wsize);
}

int
bcmbd_schanfifo_init(int unit, uint32_t max_polls, uint32_t flags)
{
    schanfifo_ctrl_t *ctrl = &schanfifo_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

   return ctrl->ops->dev_init(ctrl, max_polls, flags);
}

int
bcmbd_schanfifo_ops_send(int unit, int chan, uint32_t num_ops, uint32_t *req_buff,
                         size_t req_wsize, uint64_t buff_paddr, uint32_t flags)
{
    schanfifo_ctrl_t *ctrl = &schanfifo_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (chan < 0 || chan >= ctrl->channels || !num_ops ||
        !req_buff || req_wsize > ctrl->cmds_wsize) {
        return SHR_E_PARAM;
    }


   return ctrl->ops->ops_send(ctrl, chan, num_ops, req_buff, req_wsize, buff_paddr, flags);
}

int
bcmbd_schanfifo_set_start(int unit, int chan, bool start)
{
    schanfifo_ctrl_t *ctrl = &schanfifo_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (chan < 0 || chan >= ctrl->channels) {
        return SHR_E_PARAM;
    }

   return ctrl->ops->start_set(ctrl, chan, start);
}

int
bcmbd_schanfifo_status_get(int unit, int chan, uint32_t num_ops, uint32_t flags,
                           uint32_t *done_ops, uint32_t **resp_buff)
{
    schanfifo_ctrl_t *ctrl = &schanfifo_ctrl[unit];

    if (!ctrl || !ctrl->active || !ctrl->ops) {
        return SHR_E_UNAVAIL;
    }

    if (chan < 0 || chan >= ctrl->channels || !num_ops || !done_ops) {
        return SHR_E_PARAM;
    }

   return ctrl->ops->status_get(ctrl, chan, num_ops, flags, done_ops, resp_buff);
}

schanfifo_ctrl_t *
bcmbd_schanfifo_ctrl_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return &schanfifo_ctrl[unit];
}
