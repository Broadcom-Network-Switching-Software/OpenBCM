/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __MODEL_H
#define __MODEL_H

#include <stdint.h>

#include "device.h"
#include "errors.h"
#include "init.h"

#ifdef __cplusplus
extern "C" {
#endif


struct kbp_sw_model_stats
{
    
    uint32_t max_num_dba_blocks_lit_per_key[KBP_HW_MAX_LTRS][KBP_HW_MAX_KPUS];
};


enum kbp_sw_model_property
{
    KBP_SW_MODEL_DUMP,    
    KBP_SW_MODEL_UNROLL_COMPLEX_INST, 
    KBP_SW_MODEL_TRACE,   
    KBP_SW_MODEL_TRIG_EVENTS,  
    KBP_SW_MODEL_INVALID  
};



struct kbp_sw_model_kaps_config
{
    uint8_t id; 
    uint8_t sub_type;   
    uint16_t total_num_rpb; 
    uint16_t total_num_small_bb; 
    uint16_t total_num_bb;  
    uint16_t num_rows_in_rpb; 
    uint16_t num_rows_in_small_bb; 
    uint16_t num_rows_in_bb; 
    uint16_t profile;        
};


struct kbp_sw_model_12k_config
{
    uint32_t num_devices;     
    uint32_t dba_mb;          
    uint32_t algorithmic_lpm; 
    uint32_t algorithmic_acl; 
};


struct kbp_sw_model_op2_config
{
    uint32_t dev_type;         
    int32_t num_cores;
};


struct kbp_sw_model_config
{
    union {
        struct kbp_sw_model_kaps_config config_kaps;    
        struct kbp_sw_model_12k_config config_12k;      
        struct kbp_sw_model_op2_config config_op2;      
    } u;
};



kbp_status kbp_sw_model_init(struct kbp_allocator *alloc, enum kbp_device_type type,
                             uint32_t flags, struct kbp_sw_model_config *config, void **xpt);



kbp_status kbp_sw_model_destroy(void *xpt);



kbp_status kbp_sw_model_get_stats(void *xpt, struct kbp_sw_model_stats *stats);



kbp_status kbp_sw_model_set_property(void *xpt, enum kbp_sw_model_property property, ...);

#ifdef __cplusplus
}
#endif
#endif 
