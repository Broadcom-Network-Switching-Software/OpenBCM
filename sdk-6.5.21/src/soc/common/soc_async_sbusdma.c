/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        soc_async_sbusdma.c
 * Purpose:     SOC SBUS-DMA driver, supports both sync and async modes.
 */

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/sbusdma_internal.h>
#include <soc/soc_async_sbusdma.h>

#if defined(BCM_SBUSDMA_SUPPORT) && defined(BCM_SOC_ASYNC_SUPPORT)

#if (!defined(_LIMITS_H)) && !defined(_LIBC_LIMITS_H_)

#if (!defined(INT_MIN)) && !defined(INT_MAX)
#define INT_MIN (((int)1)<<(sizeof(int)*8-1))
#define INT_MAX (~INT_MIN)
#endif

#ifndef UINT_MAX
#define UINT_MAX ((unsigned)-1)
#endif

#endif

typedef struct soc_async_sbusdma_reg_s {
    soc_sbusdma_reg_drv_t drv;
    soc_sbusdma_cmic_ch_t  ch;
    soc_async_handle_t handle;
} soc_async_sbusdma_reg_t;

STATIC soc_async_sbusdma_reg_t _async_sbusdma_reg[SOC_MAX_NUM_DEVICES];

/*******************************************
* @function _soc_async_sbusdma_cb_f
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
_soc_async_sbusdma_cb_f(int unit,
                    void *data, void *cookie, int status)
{
    cmic_sbusdma_wparam_t *wparam;

    wparam = (cmic_sbusdma_wparam_t *)cookie;
    if (wparam) {
        wparam->cb_f(unit, data, wparam->cookie, status);
        sal_free(wparam);
    }
}

/*******************************************
* @function _soc_async_sbusdma_msg_alloc
* purpose this function to initialize sbus message
*
* @param unit [in] unit #
* @param msg [in, out] pointer to pointer soc_async_msg_t
* @param param [in] user data pointer
* @param cbf [in] user call back function
*
* @returns SOC_E_BUSY
* @returns SOC_E_MEMORY
* @end
 */
STATIC int
_soc_async_sbusdma_msg_alloc(int unit,
                     soc_async_msg_t  **msg,
                     soc_sbusdma_reg_param_t *param,
                     soc_async_cb_f   cbf)
{
    cmic_sbusdma_wparam_t *wparam;

    wparam = sal_alloc(sizeof(cmic_sbusdma_wparam_t), "cmicx_sbusdma_wparam");

    if(!wparam) {
        return SOC_E_MEMORY;
    }

    if (SOC_FAILURE(soc_async_msg_alloc(_async_sbusdma_reg[unit].handle, msg))) {
        sal_free(wparam);
        return SOC_E_MEMORY;
    }
    (*msg)->unit = unit;
    (*msg)->type = p_sbus_dma;
    (*msg)->data = param;
    (*msg)->cookie = wparam;
    (*msg)->wait_f = _async_sbusdma_reg[unit].drv.soc_sbusdma_reg_complete;
    (*msg)->cb_f = _soc_async_sbusdma_cb_f;
    wparam->cb_f = cbf;
    wparam->cookie = NULL;

    return SOC_E_NONE;
}

/*******************************************
* @function _soc_async_sbusdma_msg_free
* purpose this function to initialize sbus message
*
* @param unit [in] unit #
* @param msg [in] pointer to soc_async_msg_t
*
* @returns SOC_E_BUSY
* @returns SOC_E_MEMORY
* @end
 */
STATIC int
_soc_async_sbusdma_msg_free(int unit,
                     soc_async_msg_t  *msg)
{
   cmic_sbusdma_wparam_t *wparam = NULL;

   if (msg == NULL)
       return SOC_E_PARAM;

   wparam = (cmic_sbusdma_wparam_t *)msg->cookie;

   if (wparam) {
       sal_free(wparam);
   }

   soc_async_msg_free(_async_sbusdma_reg[unit].handle, msg);

   return SOC_E_NONE;
}

/*******************************************
* @function soc_async_mem_array_sbusdma_write
* purpose DMA acceleration for soc_mem_write_range()
* If callback function is NULL then the call
* will behave as synchronous/Blocked.
*
* @param unit [in] unit #
* @param param [in] soc_sbusdma_reg_param_t pointer
* @param cbf [out] soc_async_cb_f, call back function pointer
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int
soc_async_mem_array_sbusdma_write(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;
    soc_async_msg_t  *msg;
    soc_timeout_t to;
    cmic_sbusdma_wait_t  *wait;

    /* If callback is defined it is async call */
    if (cbf != NULL) {
        rv = _soc_async_sbusdma_msg_alloc(unit, &msg, param, cbf);
        if (rv == SOC_E_NONE) {
            wait = &((cmic_sbusdma_wparam_t *)(msg->cookie))->wait;
            wait->intr_enb = soc->tslamDmaIntrEnb;
            wait->timeout = soc->tslamDmaTimeout;
            msg->proc_f = _async_sbusdma_reg[unit].drv.soc_sbusdma_write_prog;
            soc_timeout_init(&to, wait->timeout, 100);
            do {
                rv = soc_async_msg_queue(_async_sbusdma_reg[unit].handle, msg);
                if (rv == SOC_E_NONE) {
                    break;
                }
            } while(!soc_timeout_check(&to));

            if (SOC_FAILURE(rv)) {
               (void)_soc_async_sbusdma_msg_free(unit, msg);
            }
        }

    } else {
        if (_async_sbusdma_reg[unit].drv.soc_mem_sbusdma_write != NULL) {
            rv = _async_sbusdma_reg[unit].drv.soc_mem_sbusdma_write(unit,
                                                      param);
        } else {
            rv = SOC_E_INIT;
        }
    }

    return rv;
}

/*******************************************
* @function soc_async_mem_sbusdma_write
* purpose DMA acceleration for soc_mem_write_range()
* This calls 'soc_mem_array_sbusdma_async_write'
*
* @param unit [in] unit #
* @param param [in] soc_sbusdma_reg_param_t pointer
* @param cbf [out] soc_async_cb_f, call back function pointer
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int
soc_async_mem_sbusdma_write(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf)
{
    param->array_id_start = 0;
    param->array_id_end = 0;
    param->flags= 0;

    return soc_async_mem_array_sbusdma_write(unit, param, cbf);
}

/*******************************************
* @function soc_async_mem_array_sbusdma_read
* purpose DMA acceleration for soc_mem_read_range()
* If callback function is NULL then the call
* will behave as synchronous/Blocked.
*
* @param unit [in] unit #
* @param param [in] soc_sbusdma_reg_param_t pointer
* @param cbf [out] soc_async_cb_f, call back function pointer
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int
soc_async_mem_array_sbusdma_read(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;
    soc_async_msg_t  *msg;
    soc_timeout_t to;
    cmic_sbusdma_wait_t  *wait;

    /* If callback is defined it is async call */
    if (cbf != NULL) {
        rv = _soc_async_sbusdma_msg_alloc(unit, &msg, param, cbf);
        if (rv == SOC_E_NONE) {
            wait = &((cmic_sbusdma_wparam_t *)(msg->cookie))->wait;
            wait->intr_enb = soc->tableDmaIntrEnb;
            wait->timeout = soc->tableDmaTimeout;
            msg->proc_f = _async_sbusdma_reg[unit].drv.soc_sbusdma_read_prog;
            soc_timeout_init(&to, wait->timeout, 100);

            do {
                rv = soc_async_msg_queue(_async_sbusdma_reg[unit].handle, msg);
                if (rv == SOC_E_NONE) {
                    break;
                }
            } while(!soc_timeout_check(&to));

            if (SOC_FAILURE(rv)) {
                (void)_soc_async_sbusdma_msg_free(unit, msg);
            }
        }

    } else {
        if (_async_sbusdma_reg[unit].drv.soc_mem_sbusdma_read != NULL) {
            rv = _async_sbusdma_reg[unit].drv.soc_mem_sbusdma_read(unit,
                                                      param);
        }  else {
            rv = SOC_E_INIT;
        }
    }
    return rv;
}

/*******************************************
* @function soc_async_mem_sbusdma_read
* purpose DMA acceleration for soc_mem_read_range()
* This calls 'soc_mem_array_sbusdma_async_read'
*
* @param unit [in] unit #
* @param param [in] soc_sbusdma_reg_param_t pointer
* @param cbf [out] soc_async_cb_f, call back function pointer
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int
soc_async_mem_sbusdma_read(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf)
{
     param->array_id_start = 0;

     return soc_async_mem_array_sbusdma_read(unit, param, cbf);
}

/*******************************************
* @function soc_async_mem_sbusdma_clear_specific
* purpose Clear a specific memory/table region using
* DMA write (slam) acceleration.
* If callback function is NULL then the call
* will behave as synchronous/Blocked.
*
* @param unit [in] unit #
* @param param [in] soc_sbusdma_reg_param_t pointer
* @param cbf [out] soc_async_cb_f, call back function pointer
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int
soc_async_mem_sbusdma_clear_specific(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;
    soc_async_msg_t  *msg;
    soc_timeout_t to;

    /* If callback is defined it is async call */
    if (cbf != NULL) {
         rv = _soc_async_sbusdma_msg_alloc(unit, &msg, param, cbf);
        if (rv == SOC_E_NONE) {
            msg->proc_f = _async_sbusdma_reg[unit].drv.soc_sbusdma_clear_prog;
            msg->wait_f = NULL;
            soc_timeout_init(&to, soc->tslamDmaTimeout, 100);
            do {
                rv = soc_async_msg_queue(_async_sbusdma_reg[unit].handle, msg);
                if (rv == SOC_E_NONE) {
                    break;
                }
            } while(!soc_timeout_check(&to));

            if (SOC_FAILURE(rv)) {
                (void)_soc_async_sbusdma_msg_free(unit, msg);
            }
        }

    } else {
        if (_async_sbusdma_reg[unit].drv.soc_mem_sbusdma_clear != NULL) {
            rv = _async_sbusdma_reg[unit].drv.soc_mem_sbusdma_clear(unit,
                                                      param);
        }  else {
            rv = SOC_E_INIT;
        }
    }

    return rv;
}

/*******************************************
* @function soc_async_mem_sbusdma_clear
* This function calls 'soc_mem_sbusdma_async_clear_specific'
* This calls 'soc_async_mem_sbusdma_clear_specific'
*
* @param unit [in] unit #
* @param param [in] soc_sbusdma_reg_param_t pointer
* @param cbf [out] soc_async_cb_f, call back function pointer
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int
soc_async_mem_sbusdma_clear(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf)
{

    param->array_id_start = 0;
    param->array_id_end = UINT_MAX;
    param->index_begin = INT_MIN;
    param->index_end = INT_MAX;

    return soc_async_mem_sbusdma_clear_specific(unit, param, cbf);
}

/*******************************************
* @function soc_async_sbusdma_ch_try_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [out] cmc number 0-1
* @param ch [out] channel number 0-2
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
int soc_async_sbusdma_ch_try_get(int unit, int cmc, int ch)
{
    int rv = SOC_E_NONE;

    if (_async_sbusdma_reg[unit].ch.soc_sbusdma_ch_try_get != NULL) {
        rv =
         _async_sbusdma_reg[unit].ch.soc_sbusdma_ch_try_get(unit, cmc, ch);
    } else {
        rv = SOC_E_INIT;
    }

    return rv;
}

/*******************************************
* @function soc_async_sbusdma_ch_put
* purpose put back the freed channel on cmc
*
* @param cmc [in] cmc number
* @param ch [in] channel number
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
int soc_async_sbusdma_ch_put(int unit, int cmc, int ch)
{
   int rv;

   if (_async_sbusdma_reg[unit].ch.soc_sbusdma_ch_put != NULL) {
        rv =
         _async_sbusdma_reg[unit].ch.soc_sbusdma_ch_put(unit, cmc, ch);
   } else {
       rv = SOC_E_INIT;
   }

   return rv;
}
/*******************************************
* @function soc_async_sbusdma_deinit
* purpose API to DeInitialize ASYNC SBUS DMA
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
soc_async_sbusdma_deinit(int unit)
{
#ifdef BCM_CMICX_SUPPORT
   if (soc_feature(unit, soc_feature_cmicx)) {
       (void)cmicx_sbusdma_ch_deinit(unit);

       if (_async_sbusdma_reg[unit].handle) {
           (void)soc_async_proc_deinit(_async_sbusdma_reg[unit].handle);
           _async_sbusdma_reg[unit].handle = NULL;
       }

   }
#endif
   return SOC_E_NONE;
}

/*******************************************
* @function soc_async_sbusdma_init
* purpose API to Initialize ASYNC SBUS DMA
*
* @param unit [in] unit
* @param prop [in] soc_async_prop_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
soc_async_sbusdma_init(int unit, soc_async_prop_t  *prop)
{
    int rv = SOC_E_NONE;

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        SOC_IF_ERROR_RETURN(cmicx_sbusdma_reg_init(unit,
            &_async_sbusdma_reg[unit].drv));

        SOC_IF_ERROR_RETURN(cmicx_sbusdma_ch_init(unit,
            CMIC_CMCx_SBUSDMA_CHSELECT_TIMEOUT,
            &_async_sbusdma_reg[unit].ch));

        SOC_IF_ERROR_RETURN(cmicx_sbusdma_intr_init(unit));
    }

    rv = soc_async_proc_init(
                  unit,
                  prop,
                  &_async_sbusdma_reg[unit].handle);

    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_DMA,
              (BSL_META_U(unit,
                 "Failed to initialize Async Proc =%d\n"), rv));
        (void)soc_async_sbusdma_deinit(unit);
        return rv;
    }
    LOG_VERBOSE(BSL_LS_SOC_DMA,
              (BSL_META_U(unit,":SUCCESS\n")));
#endif
    return rv;
}

#endif /* defined(BCM_SBUSDMA_SUPPORT) && defined(BCM_SOC_ASYNC_SUPPORT)  */

