/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        soc_schan_fifo.c
 * Purpose:     SOC Schan-FIFO driver, supports both sync and async modes.
 */

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/soc_schan_fifo_internal.h>


STATIC soc_schan_fifo_drv_t     _schan_fifo_drv[SOC_MAX_NUM_DEVICES];

#ifdef BCM_SOC_ASYNC_SUPPORT

/*******************************************
* @function _soc_schan_fifo_cb_f
* purpose this function calls used defined callback
*
* @param unit [in] unit #
* @param data [in] User parameter
* @param cookie [in] user/driver data
* @param cookie [in] status
*
* @end
 */
STATIC void
_soc_schan_fifo_cb_f(int unit,
                    void *data, void *cookie, int status)
{
    soc_schan_fifo_wparam_t *wparam;

    wparam = (soc_schan_fifo_wparam_t *)cookie;
    if (wparam) {
        wparam->cb_f(unit, data, wparam->cookie, status);
        sal_free(wparam);
    }
}

/*******************************************
* @function _soc_schan_fifo_msg_alloc
* purpose this function to initialize sbuus message
*
* @param unit [in] unit #
* @param msg [in, out] pointer to pointer soc_async_msg_t
* @param param [in] user data pointer
* @param cookie [in]cookie pointer
* @param cbf [in] user call back function
*
* @returns SOC_E_BUSY
* @returns SOC_E_MEMORY
* @end
 */
STATIC int
_soc_schan_fifo_msg_alloc(int unit,
                     soc_async_msg_t  **msg,
                     soc_schan_fifo_msg_t *param,
                     void *cookie,
                     soc_async_cb_f   cbf)
{
    soc_schan_fifo_wparam_t *wparam;
    soc_schan_fifo_drv_op_t *op = &_schan_fifo_drv[unit].op;

    wparam = sal_alloc(sizeof(soc_schan_fifo_wparam_t),
                       "soc_schan_fifo_wparam");

    if(!wparam) {
        return SOC_E_MEMORY;
    }

    if (SOC_FAILURE(soc_async_msg_alloc(
                _schan_fifo_drv[unit].handle, msg))) {
        sal_free(wparam);
        return SOC_E_MEMORY;
    }
    (*msg)->unit = unit;
    (*msg)->type = p_schan_fifo;
    (*msg)->data = param;
    (*msg)->cookie = wparam;
    (*msg)->proc_f = op->soc_schan_fifo_op_prog;
    (*msg)->wait_f = op->soc_schan_fifo_op_complete;
    (*msg)->cb_f = _soc_schan_fifo_cb_f;
    wparam->cb_f = cbf;
    wparam->cookie = cookie;

    return SOC_E_NONE;
}

/*******************************************
* @function _soc_schan_fifo_msg_free
* purpose this function to initialize sbuus message
*
* @param unit [in] unit #
* @param msg [in] pointer to soc_async_msg_t
*
* @returns SOC_E_BUSY
* @returns SOC_E_MEMORY
* @end
 */
STATIC int
_soc_schan_fifo_msg_free(int unit,
                     soc_async_msg_t  *msg)
{
   soc_schan_fifo_wparam_t *wparam = NULL;

   if (msg == NULL)
       return SOC_E_PARAM;

   wparam = (soc_schan_fifo_wparam_t *)msg->cookie;

   if (wparam) {
       sal_free(wparam);
   }

   soc_async_msg_free(_schan_fifo_drv[unit].handle, msg);

   return SOC_E_NONE;
}

#endif
/*******************************************
* @function soc_schan_fifo_msg_send
* purpose cmic SCHAN FIFO operation
*
* @param unit [in] unit
* @param msg [in, out] soc_schan_fifo_msg_t
* @param cookie [in] pointer to void, user data
* @param cbf [in] soc_async_cb_f, call back function
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
 */
extern int
soc_schan_fifo_msg_send(int unit,
             soc_schan_fifo_msg_t *msg,
             void *cookie,
             soc_async_cb_f cbf)
{
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
                 (BSL_META_U(unit,
                             "  Enter: soc_schan_fifo_msg_send\n")));

#ifdef BCM_SOC_ASYNC_SUPPORT
    /* If callback is defined it is async call */
    if (cbf != NULL) {
        soc_async_msg_t  *async_m;
        rv = _soc_schan_fifo_msg_alloc(unit, &async_m, msg, cookie, cbf);
        if (rv == SOC_E_NONE) {
            if (_schan_fifo_drv[unit].handle) {
                rv = soc_async_msg_queue(
                        _schan_fifo_drv[unit].handle, async_m);
                if (SOC_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_SOC_SCHANFIFO,
                                  (BSL_META_U(unit,
                                              " Unable to Queue = %d\n"), rv));
                    (void)_soc_schan_fifo_msg_free(unit, async_m);
                }
            } else {
               rv = SOC_E_INIT;
            }
        }

    }
    else
#endif
    {
        soc_schan_fifo_wparam_t wparam;
        soc_schan_fifo_drv_op_t *op = &_schan_fifo_drv[unit].op;

        if (op->soc_schan_fifo_op_prog != NULL) {
            rv = op->soc_schan_fifo_op_prog(unit, msg, &wparam);
        } else {
            rv = SOC_E_INIT;
        }
        if (rv == SOC_E_NONE) {
            if (op->soc_schan_fifo_op_complete != NULL) {
                rv = op->soc_schan_fifo_op_complete(unit,
                                              msg, &wparam, 0);
            } else {
                rv = SOC_E_INIT;
            }
        }
    }
/*
 * COVERITY
 * The allocated message memory (async_m )is freed in Async Module
 */
 /* coverity[leaked_storage: FALSE] */

    return rv;

}

/*******************************************
* @function soc_schan_fifo_control
* purpose API to control SCHAN FIFO
*
* @param unit [in] unit
* @param ctl [in] schan_fifo_ctl_t
* @param data [in, out] pointer to void
*
* @returns SOC_E_NONE
*
* @end
 */
int
soc_schan_fifo_control(int unit,
                 schan_fifo_ctl_t ctl, void *data)
{
    soc_schan_fifo_drv_op_t *op = &_schan_fifo_drv[unit].op;

    switch (ctl) {
#ifdef BCM_SOC_ASYNC_SUPPORT
        case CTL_FIFO_START:
            if (_schan_fifo_drv[unit].handle) {
                soc_async_msg_start(_schan_fifo_drv[unit].handle);
            }
        break;
        case CTL_FIFO_STOP:
            if (_schan_fifo_drv[unit].handle) {
                soc_async_msg_stop(_schan_fifo_drv[unit].handle);
            }
        break;
        case CTL_FIFO_ABORT:

            if (_schan_fifo_drv[unit].handle) {
                soc_async_flush_queue(_schan_fifo_drv[unit].handle);
            }
            if (op->soc_schan_fifo_control) {
                (void)op->soc_schan_fifo_control(unit, ctl, NULL);
            }

        break;
        case CTL_FIFO_FLUSH:

            if (_schan_fifo_drv[unit].handle) {
                soc_async_flush_queue(_schan_fifo_drv[unit].handle);
            }

        break;
#endif
        case CTL_FIFO_MAX_MSG_GET:
        case CTL_FIFO_RESP_ALLOC:
        case CTL_FIFO_RESP_FREE:
            if (op->soc_schan_fifo_control) {
                (void)op->soc_schan_fifo_control(unit, ctl, data);
            }
        break;

        default:
            LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
              (BSL_META_U(unit,":undefined control command = %d\n"), ctl));
        break;

    }

    return SOC_E_NONE;
}

/*******************************************
* @function soc_schan_fifo_deinit
* purpose API to deInitialize and config SCHAN FIFO
* It will abort the current operation, flush the queue
* and release all the resources.
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_schan_fifo_deinit(int unit)
{
   soc_schan_fifo_drv_op_t *op = &_schan_fifo_drv[unit].op;

   if (op->soc_schan_fifo_deinit) {
       (void)op->soc_schan_fifo_deinit(unit);
   }
#ifdef BCM_SOC_ASYNC_SUPPORT
   if (_schan_fifo_drv[unit].handle) {
       (void)soc_async_proc_deinit(_schan_fifo_drv[unit].handle);
       _schan_fifo_drv[unit].handle = NULL;
   }
#endif
   return SOC_E_NONE;
}

/*******************************************
* @function soc_schan_fifo_init
* purpose API to Initialize and config SCHAN FIFO
* It will initialize the SCHAN FIFO hardware and
* allocate resources for async operation.
* In case of sync only set soc_async_prop_t pointer
* to be NULL.
*
* @param unit [in] unit
* @param prop [in] pointer to soc_async_prop_t
* @param config [in] pointer to soc_schan_fifo_config_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_schan_fifo_init(int unit,
                   soc_async_prop_t  *prop,
                   soc_schan_fifo_config_t *config)
{
    int rv = SOC_E_NONE;

    /* Cleanup if already initialized */
    soc_schan_fifo_deinit(unit);

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        SOC_IF_ERROR_RETURN(soc_cmicx_schan_fifo_init(unit,
                                &_schan_fifo_drv[unit].op, config));
    }
#endif

#ifdef BCM_SOC_ASYNC_SUPPORT
    /* check if async support is needed */
    if (!prop) {
        return SOC_E_NONE;
    }
    rv = soc_async_proc_init(
                  unit,
                  prop,
                  &_schan_fifo_drv[unit].handle);

    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_SCHANFIFO,
              (BSL_META_U(unit,
                 "Failed to initialize Async Proc =%d\n"), rv));
        (void)soc_schan_fifo_deinit(unit);
        return rv;
    }
#endif
    LOG_VERBOSE(BSL_LS_SOC_SCHANFIFO,
              (BSL_META_U(unit,":SUCCESS\n")));

    return rv;

}

