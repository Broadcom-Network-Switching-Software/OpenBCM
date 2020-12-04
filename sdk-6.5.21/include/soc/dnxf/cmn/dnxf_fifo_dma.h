/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNXF FIFO DMA H
 */
 
#ifndef _SOC_DNXF_FIFO_DMA_H_
#define _SOC_DNXF_FIFO_DMA_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <soc/types.h>
#include <soc/error.h>
#include <soc/dnxc/dnxc_defs.h>











typedef struct soc_dnxf_fifo_dma_config_s
{
    uint32      max_entries;
    uint32      entry_size;         
    int         is_mem;             
    soc_mem_t   mem;                
    soc_reg_t   reg;                
} soc_dnxf_fifo_dma_config_t;


typedef struct soc_dnxf_fifo_dma_s
{
    soc_dnxf_fifo_dma_config_t   config;             
    uint32                      enable;             
    uint32                      current_entry_id;   
    uint32                      nof_unread_entries; 
    uint32                      read_entries;       
    uint8                       *buffer;
} soc_dnxf_fifo_dma_t;





shr_error_e soc_dnxf_fifo_dma_init(int unit);
shr_error_e soc_dnxf_fifo_dma_deinit(int unit);

shr_error_e soc_dnxf_fifo_dma_channel_entry_get(int unit, int channel, uint32  max_entry_size, uint32 nof_fifo_dma_buf_entries, uint8 *entry);

shr_error_e soc_dnxf_fifo_dma_channel_init(int unit, int channel);
shr_error_e soc_dnxf_fifo_dma_channel_deinit(int unit, int channel);




#endif 
