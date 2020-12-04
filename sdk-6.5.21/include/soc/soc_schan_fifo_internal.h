/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Internal SCHAN FIFO data structures and API
 */

#ifndef _SOC_SCHAN_FIFO_INTERNAL_H
#define _SOC_SCHAN_FIFO_INTERNAL_H

#include <soc/types.h>
#include <soc/drv.h>
#include <soc/soc_schan_fifo.h>

typedef struct soc_schan_fifo_wparam_s {
  int ch;
  void *cookie;
  soc_async_cb_f  cb_f;
}soc_schan_fifo_wparam_t;

/* This structure is used for SCHAN FIFO cmic specific driver operations*/
typedef struct soc_schan_fifo_drv_op_s {
    int (*soc_schan_fifo_op_prog)(int unit,
            void *data, void *wparam);
    int (*soc_schan_fifo_op_complete)(int unit,
            void *data, void *wparam, uint32 do_not_free_channel);
    void (*soc_schan_fifo_deinit)(int unit);
    int (*soc_schan_fifo_control)(int unit,
           schan_fifo_ctl_t ctl, void *data);
} soc_schan_fifo_drv_op_t;

/* This structure is used for SCHAN FIFO Driver*/

typedef struct soc_schan_fifo_drv_s {
    soc_schan_fifo_drv_op_t op;
    soc_async_handle_t handle;
} soc_schan_fifo_drv_t;


/*******************************************
* @function soc_cmicx_schan_fifo_init
* purpose API to Initialize cmicx SCHAN FIFO
*
* @param unit [in] unit
* @param drv [out] soc_schan_fifo_drv_op_t pointer
* @param config [in] pointer to soc_schan_fifo_config_t
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int soc_cmicx_schan_fifo_init(int unit,
                                soc_schan_fifo_drv_op_t *drv,
                                soc_schan_fifo_config_t *config);


#endif  /* !_SOC_SCHAN_FIFO_INTERNAL_H */
