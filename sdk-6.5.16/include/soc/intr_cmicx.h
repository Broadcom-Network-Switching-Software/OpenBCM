/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        intr_cmicx.h
 * Purpose:     CMICX interrupt include file
 */
#ifndef _SOC_INTR_CMICX_H
#define _SOC_INTR_CMICX_H

#include <soc/intr.h>
#include <soc/mcm/intr_cmicx.h>
#include <soc/mcm/intr_iproc.h>

#define CMICX_INTR_REG_NUM     (8)
#define CMCx_SBUSDMA_CHy_BASE  (CMC0_SBUSDMA_CH0_DONE)
#define INTR_SBUSDMA(x, y)     (CMC0_SBUSDMA_CH0_DONE + 48*x + y)
#define INTR_SCHAN(x)          (SCHAN_CH0_OP_DONE + x)
#define INTR_SCHAN_FIFO(x)     (SCHAN_FIFO_CH0_DONE + x)
#define INTR_FIFO_DMA(x)       (FIFO_CH0_DMA_INTR + x)

/*******************************************
* @function soc_cmicx_intr_init
* purpose initialize CMICX interrupt framework
* @param unit [out] soc_cmic_intr_op_t pointer
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_cmicx_intr_init(int unit, soc_cmic_intr_op_t *intr_op);

/*******************************************
* @function soc_cmicx_intr
* purpose SOC CMICX Interrupt Service Routine
*
* @param unit [in] unit
*
*
* @end
 */
extern void
soc_cmicx_intr(void *_unit);

#ifdef SEPARATE_PKTDMA_INTR_HANDLER
/*******************************************
* @function soc_cmicx_pktdma_intr
* purpose SOC CMICX Interrupt Service Routine
*
* @param unit [in] unit
*
*
* @end
 */
extern void
soc_cmicx_pktdma_intr(void *_unit);
#endif

#endif  /* !_SOC_INTR_CMICX_H */
