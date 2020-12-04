/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON FIFO DMA H
 */


#ifndef _SOC_RAMON_FIFO_DMA_H_
#define _SOC_RAMON_FIFO_DMA_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 





#include <soc/error.h>
#include <soc/dnxf/cmn/dnxf_fifo_dma.h>





shr_error_e soc_ramon_fifo_dma_channel_init(int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info);
shr_error_e soc_ramon_fifo_dma_channel_deinit(int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info);
shr_error_e soc_ramon_fifo_dma_channel_read_entries(int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info);
shr_error_e soc_ramon_fifo_dma_fabric_cell_validate(int unit);


#endif 

