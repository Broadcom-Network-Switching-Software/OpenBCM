/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __KBP_MEM_TEST_H
#define __KBP_MEM_TEST_H


#include "kbp_portable.h"
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

struct kbp_mem_test_info {
    uint32_t debug_prints; 
    void *handle;          

    uint64_t test_duration_in_usec;  
    uint32_t modified_addr_length;   
    uint32_t modified_addr_count;    
    uint32_t *modified_addr;         

    uint32_t inject_error_addr_length; 
    uint32_t *inject_error_addr;      

    
    kbp_status (*kbp_mem_test_usleep)(void *handle, uint64_t);

    
    kbp_status (*kbp_mdio_register_write_80)(void *handle, uint8_t kbp_select, uint8_t device_id, uint16_t addr, uint16_t* wr_data);

    
    kbp_status (*kbp_mdio_register_read_80)(void *handle, uint8_t kbp_select, uint8_t device_id, uint16_t addr, uint16_t* rd_data);

    
    kbp_status (*kbp_mdio_register_write_64)(void *handle, uint8_t kbp_select, uint8_t device_id, uint16_t addr, uint16_t* wr_data);

    
    kbp_status (*kbp_mdio_register_read_64)(void *handle, uint8_t kbp_select, uint8_t device_id, uint16_t addr, uint16_t* rd_data);
};


kbp_status kbp_mem_test_dba(struct kbp_device *device, struct kbp_mem_test_info *info);



kbp_status kbp_mem_test_uda(struct kbp_device *device, struct kbp_mem_test_info *info);


kbp_status kbp_mem_test_rblock(struct kbp_device *device, struct kbp_mem_test_info *info);


kbp_status kbp_mem_test_ublock(struct kbp_device *device, struct kbp_mem_test_info *info);



kbp_status kbp_mem_test_rw_dba(struct kbp_device *device, uint32_t debug_prints);



kbp_status kbp_mem_test_rw_uda(struct kbp_device *device, uint32_t debug_prints);



kbp_status kbp_mem_test_rw_advance( struct kbp_device *device, uint32_t debug_prints);
kbp_status kbp_mem_test_rw_rblock(struct kbp_device *device, uint32_t debug_prints);
kbp_status kbp_mem_test_rw_ublock(struct kbp_device *device, uint32_t debug_prints);
kbp_status kbp_mem_test_rw_pblock(struct kbp_device *device, uint32_t debug_prints);
kbp_status kbp_mem_test_rw_ctx_buffer(struct kbp_device *device, uint32_t debug_prints);
kbp_status kbp_mem_test_rw_ltr(struct kbp_device *device, uint32_t debug_prints);

#ifdef __cplusplus
}
#endif

#endif 

