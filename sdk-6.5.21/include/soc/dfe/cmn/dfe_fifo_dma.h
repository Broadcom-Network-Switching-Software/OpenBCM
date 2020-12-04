/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE FIFO DMA H
 */
 
#ifndef _SOC_DFE_FIFO_DMA_H_
#define _SOC_DFE_FIFO_DMA_H_

#include <soc/types.h>
#include <soc/error.h>
#include <soc/dcmn/dcmn_defs.h>











typedef struct soc_dfe_fifo_dma_config_s
{
    uint32      max_entries;
    uint32      entry_size;         
    int         is_mem;             
    soc_mem_t   mem;                
    soc_reg_t   reg;                
} soc_dfe_fifo_dma_config_t;


typedef struct soc_dfe_fifo_dma_s
{
    soc_dfe_fifo_dma_config_t   config;             
    uint32                      enable;             
    uint32                      current_entry_id;   
    uint32                      nof_unread_entries; 
    uint32                      read_entries;       
    uint8                       *buffer;
} soc_dfe_fifo_dma_t;





soc_error_t soc_dfe_fifo_dma_init(int unit);
soc_error_t soc_dfe_fifo_dma_deinit(int unit);

soc_error_t soc_dfe_fifo_dma_channel_entry_get(int unit, int channel, uint32  max_entry_size, uint32 nof_fifo_dma_buf_entries, uint8 *entry);
soc_error_t soc_dfe_fifo_dma_channel_clear(int unit, int channel);

soc_error_t soc_dfe_fifo_dma_channel_init(int unit, int channel);
soc_error_t soc_dfe_fifo_dma_channel_deinit(int unit, int channel);




#endif 
