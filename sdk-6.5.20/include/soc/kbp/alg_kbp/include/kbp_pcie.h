/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __KBP_PCIE_H
#define __KBP_PCIE_H



#include <stdint.h>

#include "errors.h"
#include "init.h"
#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif





struct kbp_search_result;


enum kbp_pcie_properties {
    KBP_PCIE_NON_DMA,            
    KBP_PCIE_VERBOSE_LEVEL,      
    KBP_PCIE_NEED_RESPONSE,      
    KBP_PCIE_INTERRUPT,          
    KBP_PCIE_REPLACE_SEARCH,     
    KBP_PCIE_PROP_INVALID        
};



enum kbp_pcie_reset_type {
    KBP_PCIE_SW_SRST,           
    KBP_PCIE_SW_CRST,           
    KBP_PCIE_SW_DIS_EVIC,       
    KBP_PCIE_SW_DIS_RSPF_FC,    
    KBP_PCIE_PM0_SW_SRST,       
    KBP_PCIE_PM1_SW_SRST,       
    KBP_PCIE_SOFT_SRST,         
    KBP_PCIE_DMA_RST,           
    KBP_PCIE_RST_INVALID        
};



struct kbp_pcie_settings {
    uint32_t req_q_size;      
    uint32_t resp_q_size[5];  
};



kbp_status kbp_pcie_set_property(void *xpt, enum kbp_pcie_properties property, ...);



kbp_status kbp_pcie_init(enum kbp_device_type type, uint32_t flags, uint32_t device_no, struct kbp_allocator *alloc,
                         struct kbp_device_config *config, struct kbp_pcie_settings *driver_settings,
                         void **xpt);



kbp_status kbp_pcie_destroy(struct kbp_allocator *alloc, void *xpt);



kbp_status kbp_pcie_soft_reset(void *xpt);



typedef kbp_status (*kbp_op_search) (void *handle, uint32_t ltr, uint32_t ctx,
                                     const uint8_t *key, uint32_t key_len,
                                     struct kbp_search_result *result);




kbp_status kbp_pcie_reset(enum kbp_device_type type, void *xpt, enum kbp_pcie_reset_type reset_type);



kbp_status kbp_pcie_sat_bus_read(void *xpt, uint32_t size, uint32_t addr, uint8_t *data);



kbp_status kbp_pcie_sat_bus_write(void *xpt, uint32_t size, uint32_t base_addr, uint8_t *data);



#ifdef __cplusplus
}
#endif
#endif 

