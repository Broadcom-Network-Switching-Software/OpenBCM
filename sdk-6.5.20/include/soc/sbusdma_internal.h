/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        sbusdma_internal.h
 * Purpose:     Maps out structures used for SBUSDMA operations and
 *              exports routines.
 */

#ifndef _SOC_SBUSDMA_INTERNAL_H
#define _SOC_SBUSDMA_INTERNAL_H

#include <soc/defs.h>
#include <soc/types.h>
#include <sal/types.h>
#include <sal/core/time.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <soc/sbusdma.h>
#if defined(BCM_SOC_ASYNC_SUPPORT) || (defined(BCM_CMICX_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT))
#include <soc/soc_async.h>
#endif

#define CMIC_CMCx_SBUSDMA_CHSELECT_TIMEOUT    (1000)

typedef struct cmic_sbusdma_wait_s {
    int         opcode;
    int         intr_enb;
    int         timeout;
    int         intr;
    int         count;
    int         ch;
    int         cmc;
#ifdef PRINT_DMA_TIME
    sal_usecs_t   start_time;
#endif
}cmic_sbusdma_wait_t;

#if defined(BCM_SOC_ASYNC_SUPPORT) || (defined(BCM_CMICX_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT))
typedef struct cmic_sbusdma_wparam_s {
  cmic_sbusdma_wait_t wait;
  void *cookie;
  soc_async_cb_f  cb_f;
}cmic_sbusdma_wparam_t;
#endif

/* Register SBUS DMA function pointers */
typedef struct soc_sbusdma_reg_drv_s {
    int (*soc_mem_sbusdma_read)(int unit, soc_sbusdma_reg_param_t  *param);
    int (*soc_mem_sbusdma_write)(int unit, soc_sbusdma_reg_param_t *param);
    int (*soc_mem_sbusdma_clear)(int unit, soc_sbusdma_reg_param_t *param);
    int (*soc_sbusdma_read_prog)(int unit,
            void *data, void *wparam);
    int (*soc_sbusdma_write_prog)(int unit,
            void *data, void *wparam);
    int (*soc_sbusdma_clear_prog)(int unit,
            void *data, void *wparam);
    int (*soc_sbusdma_reg_complete)(int unit,
            void *data, void *wparam, uint32 do_not_free_channel);
    uint32 (*soc_sbusdma_cmc_max_num_ch_get)(int unit);
}soc_sbusdma_reg_drv_t;

/* Descriptor SBUS DMA function pointers */

typedef struct soc_sbusdma_desc_drv_s {
int (*soc_sbusdma_desc_create)(int unit, soc_sbusdma_desc_ctrl_t *ctrl,
                        soc_sbusdma_desc_cfg_t *cfg,  uint8 alloc_memory, void **desc);
int (*soc_sbusdma_program)(int unit, int *cmc, _soc_sbusdma_state_t *swd,
                           int *ch);
int (*soc_sbusdma_desc_detach)(int unit);
int (*soc_sbusdma_wait)(int unit, int cmc, int ch, int interval,
                       uint8 intrEnb);
}soc_sbusdma_desc_drv_t;

typedef struct soc_sbusdma_cmic_ch_s {
    int (*soc_sbusdma_ch_try_get)(int unit, int cmc, int ch);
    int (*soc_sbusdma_ch_put)(int unit, int cmc, int ch);
} soc_sbusdma_cmic_ch_t;

#define SOC_CHECK_MULTI_CMC(unit) \
         (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc) && \
                (SOC_PCI_CMCS_NUM(unit) > 1))

/*******************************************
* @function cmicm_sbusdma_ch_init
* purpose API to Initialize sbusdma channel seletion mechanism
*
* @param unit [in] unit
* @param timeout [in] int
* @param cmic_ch [out] pointer type soc_sbusdma_cmic_ch_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
cmicm_sbusdma_ch_init(int unit, int timeout,
                           soc_sbusdma_cmic_ch_t *cmic_ch);



/*******************************************
* @function cmicm_sbusdma_ch_put
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
extern int
cmicm_sbusdma_ch_put(int unit, int cmc, int ch);

/*******************************************
* @function cmicm_sbusdma_ch_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [out] cmc number
* @param ch [out] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
cmicm_sbusdma_ch_get(int unit, int *cmc, int *ch);

/*******************************************
* @function cmicm_sbusdma_ch_try_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [in] cmc number
* @param ch [in] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
cmicm_sbusdma_ch_try_get(int unit, int cmc, int ch);

/*******************************************
* @function cmicm_sbusdma_reg_init
* purpose API to Initialize cmicm register DMA
*
* @param unit [in] unit
* @param drv [out] soc_sbusdma_reg_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int cmicm_sbusdma_reg_init(int unit, soc_sbusdma_reg_drv_t *drv);

/*******************************************
* @function cmicm_sbusdma_desc_init
* purpose API to Initialize cmicm Descriptor DMA
*
* @param unit [in] unit
* @param drv [out] soc_sbusdma_desc_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int cmicm_sbusdma_desc_init(int unit, soc_sbusdma_desc_drv_t *drv);

/*******************************************
* @function cmicm_sbusdma_curr_op_details
* purpose API to print cmic operation registers
*
* @param unit [in] unit
* @param cmc [in] cmc
* @param ch [in] channel#
*
* @returns void
*
* @end
 */
extern void
cmicm_sbusdma_curr_op_details(int unit, int cmc, int ch);

/*******************************************
* @function cmicm_sbusdma_error_details
* purpose API to print cmic error registers
*
* @param unit [in] unit
* @param rval [in] return value
*
* @returns void
*
* @end
 */
extern void
cmicm_sbusdma_error_details(int unit, uint32 rval);

/*******************************************
* @function cmicx_sbusdma_ch_init
* purpose API to Initialize sbusdma channel seletion mechanism
*
* @param unit [in] intt
* @param timeout [in] int
* @param cmic_ch [out] pointer type soc_sbusdma_cmic_ch_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
cmicx_sbusdma_ch_init(int unit, int timeout,
                      soc_sbusdma_cmic_ch_t *cmic_ch);

/*******************************************
* @function cmicx_sbusdma_ch_deinit
* purpose API to de-Initialize sbusdma channel
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
cmicx_sbusdma_ch_deinit(int unit);

/*******************************************
* @function cmicx_sbusdma_ch_put
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
extern int
cmicx_sbusdma_ch_put(int unit, int cmc, int ch);

/*******************************************
* @function cmicx_sbusdma_ch_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [out] cmc number
* @param ch [out] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
cmicx_sbusdma_ch_get(int unit, int *cmc, int *ch);

/*******************************************
* @function cmicx_sbusdma_ch_try_get
* purpose get idle channel on cmc
*
* @param unit [in] unit #
* @param cmc [out] cmc number
* @param ch [out] channel number
*
* @returns SOC_E_BUSY
* @returns SOC_E_NONE
*
* @end
 */
extern int
cmicx_sbusdma_ch_try_get(int unit, int cmc, int ch);

/*******************************************
* @function cmicx_sbusdma_ch_endian_set
* set the endianess value
*
* @param cmc [in] cmc number
* @param ch [in] channel number
* @param ch [in] big_endian
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
extern int
cmicx_sbusdma_ch_endian_set(int unit, int cmc, int ch, int be);

/*******************************************
* @function cmicx_sbusdma_vchan_to_cmc_ch
* API takes vchan as input and return cmc and channel
*
* @param unit [in] int
* @param vchan [in] virtual channel
* @param cmc [out] cmc associated
* @param channel [out] channel associated
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
cmicx_sbusdma_vchan_to_cmc_ch(int unit, int vchan,
                               int *cmc, int *ch);

/*******************************************
* @function cmicx_sbusdma_error_details
* purpose Print error details
*
* @param unit [in] unit
* @param rval [in] Error value
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern void
cmicx_sbusdma_error_details(int unit, uint32 rval);

/*******************************************
* @function cmicx_sbusdma_curr_op_details
* purpose Print operation details
*
* @param unit [in] unit
* @param cmc [in] cmc number
* @param ch [in] channel number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern void
cmicx_sbusdma_curr_op_details(int unit, int cmc, int ch);

/*******************************************
* @function cmicx_sbusdma_reg_init
* purpose API to Initialize cmicx Register DMA
*
* @param unit [in] unit
* @param drv [out] soc_sbusdma_desc_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
cmicx_sbusdma_reg_init(int unit, soc_sbusdma_reg_drv_t *drv);


/*******************************************
* @function cmicx_sbusdma_desc_init
* purpose API to Initialize cmicx Descriptor DMA
*
* @param unit [in] unit
* @param drv [out] soc_sbusdma_desc_drv_t pointer
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
cmicx_sbusdma_desc_init(int unit, soc_sbusdma_desc_drv_t *drv);

/*******************************************
* @function cmicx_sbusdma_intr_init
* purpose API to Initialize SBUSDMA interrupts
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
cmicx_sbusdma_intr_init(int unit);

/*******************************************
* @function cmicx_sbusdma_intr_wait
* purpose API to wait for DMA complete using interrupt mode
*
* @param unit [in] unit
* @param cmc [in] cmc
* @param ch [in] channel
* @param interval [in] timeout
* @param do_not_perform_start_operation [in] if the argument is non-zero, the operation is assumed to be already started.
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
cmicx_sbusdma_intr_wait(int unit, int cmc, int ch, int timeout, uint32 do_not_perform_start_operation);

/*******************************************
* @function cmicx_sbusdma_poll_wait
* purpose API to wait DMA to complete using Polling mode
*
* @param unit [in] unit
* @param cmc [in] cmc
* @param ch [in] channel
* @param interval [in] timeout
* @param do_not_perform_start_operation [in] if the argument is non-zero, the operation is assumed to be already started.
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
cmicx_sbusdma_poll_wait(int unit, int cmc, int ch, int timeout, uint32 do_not_perform_start_operation);

/*******************************************
* @function cmicx_sbusdma_stop
* start the SBUSDMA
*
* @param unit [in] unit
* @param cmc [in] cmc
* @param ch [in] channel
*
* @returns SOC_E_NONE
*
* @end
 */
int
cmicx_sbusdma_stop(int unit, int cmc, int ch);

int
_cmicx_sbusdma_start(int unit, int cmc, int ch);



#endif /* !_SOC_SBUSDMA_INTERNAL_H */
