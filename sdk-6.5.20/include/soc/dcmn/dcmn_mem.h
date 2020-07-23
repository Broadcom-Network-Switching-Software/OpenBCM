/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_DCMN_MEM_H
#define _SOC_DCMN_MEM_H
#include <soc/mcm/allenum.h>


int
dcmn_mem_array_wide_access(int unit, uint32 flags, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data,unsigned operation);




#ifdef BCM_SBUSDMA_SUPPORT
#define SOC_DCMN_MAX_SBUSDMA_CHANNELS    3
#define SOC_DCMN_TDMA_CHANNEL            0
#define SOC_DCMN_TSLAM_CHANNEL           1
#define SOC_DCMN_DESC_CHANNEL            2
#define SOC_DCMN_MEM_CLEAR_CHUNK_SIZE    4 
#endif 

#endif 
