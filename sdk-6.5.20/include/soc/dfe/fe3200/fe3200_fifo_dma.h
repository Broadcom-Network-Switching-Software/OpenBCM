/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 FIFO DMA H
 */


#ifndef _SOC_FE3200_FIFO_DMA_H_
#define _SOC_FE3200_FIFO_DMA_H_





#include <soc/error.h>
#include <soc/dfe/cmn/dfe_fifo_dma.h>






typedef enum soc_fe3200_fifo_dma_channel_e
{
    soc_fe3200_fifo_dma_channel_fabric_cell_dcl_0 = 0,
    soc_fe3200_fifo_dma_channel_fabric_cell_dcl_1 = 1,
    soc_fe3200_fifo_dma_channel_fabric_cell_dcl_2 = 2, 
    soc_fe3200_fifo_dma_channel_fabric_cell_dcl_3 = 3
} soc_fe3200_fifo_dma_channel_t;





soc_error_t soc_fe3200_fifo_dma_channel_init(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
soc_error_t soc_fe3200_fifo_dma_channel_deinit(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
soc_error_t soc_fe3200_fifo_dma_channel_clear(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
soc_error_t soc_fe3200_fifo_dma_channel_read_entries(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info);
soc_error_t soc_fe3200_fifo_dma_fabric_cell_validate(int unit);


#endif 

