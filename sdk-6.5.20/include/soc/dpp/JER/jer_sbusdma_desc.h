/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SOC_DPP_JER_SBUSDMA_DESC_H
#define _SOC_DPP_JER_SBUSDMA_DESC_H




#include <soc/dpp/SAND/Utils/sand_header.h>






























uint32 jer_sbusdma_desc_init(int unit, uint32 desc_num_max, uint32 mem_buff_size, uint32 timeout_usec);

uint32 jer_sbusdma_desc_deinit(int unit);

uint32 jer_sbusdma_desc_wait_done(int unit);

uint32 jer_sbusdma_desc_add_mem(int unit, soc_mem_t mem, uint32 array_index, int blk, uint32 offset, void *entry_data);

uint32 jer_sbusdma_desc_add_reg(int unit, soc_reg_t reg, int instance, uint32 array_index, void *entry_data);

uint32 jer_sbusdma_desc_add_fifo_dma(int unit, soc_mem_t mem, uint32 array_index, int blk, uint32 offset, void *entry_data, uint32 count, uint32 addr_shift, uint8 new_desc);

uint32 jer_sbusdma_desc_status(int unit, uint32 *desc_num_max, uint32 *mem_buff_size, uint32 *timeout_usec);

uint32 jer_sbusdma_desc_is_enabled(int unit);



#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

