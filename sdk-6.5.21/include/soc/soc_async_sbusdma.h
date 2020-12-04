/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *
 * File:        soc_async_sbusdma.h
 * Purpose:     Maps out structures used for SBUSDMA ASYNC operations and
 *              exports routines.
 */

#ifndef _SOC_ASYNC_SBUSDMA_H
#define _SOC_ASYNC_SBUSDMA_H

#include <soc/defs.h>
#include <soc/types.h>
#include <sal/types.h>
#include <soc/sbusdma.h>
#include <soc/soc_async.h>


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
extern int
soc_async_mem_array_sbusdma_write(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf);


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
extern int
soc_async_mem_sbusdma_write(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf);

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
extern int
soc_async_mem_array_sbusdma_read(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf);


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
extern int
soc_async_mem_sbusdma_read(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf);


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
extern int
soc_async_mem_sbusdma_clear_specific(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf);


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
extern int
soc_async_mem_sbusdma_clear(int unit,
        soc_sbusdma_reg_param_t *param, soc_async_cb_f   cbf);

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
extern int
soc_async_sbusdma_ch_try_get(int unit, int cmc, int ch);

/*******************************************
* @function soc_async_sbusdma_ch_put
* purpose put back the freed channel on cmc
*
* @param cmc [in] cmc number 0-1
* @param ch [in] channel number 0-2
*
* @returns SOC_E_PARAM
* @returns SOC_E_NONE
*
* @end
 */
extern int
soc_async_sbusdma_ch_put(int unit, int cmc, int ch);

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
extern int
soc_async_sbusdma_init(int unit, soc_async_prop_t  *prop);

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
extern int
soc_async_sbusdma_deinit(int unit);

#endif

