

#ifndef INCLUDED_KAPS_MODEL_H
#define INCLUDED_KAPS_MODEL_H

#include <stdint.h>

#include "kaps_device.h"
#include "kaps_errors.h"
#include "kaps_init.h"

#ifdef __cplusplus
extern "C" {
#endif


struct kaps_sw_model_stats
{
    
    uint32_t max_num_dba_blocks_lit_per_key[KAPS_HW_MAX_LTRS][KAPS_HW_MAX_KPUS];
};


enum kaps_sw_model_property
{
    KAPS_SW_MODEL_DUMP,    
    KAPS_SW_MODEL_UNROLL_COMPLEX_INST, 
    KAPS_SW_MODEL_TRACE,   
    KAPS_SW_MODEL_TRIG_EVENTS,  
    KAPS_SW_MODEL_INVALID  
};



struct kaps_sw_model_kaps_config
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


struct kaps_sw_model_12k_config
{
    uint32_t num_devices;     
    uint32_t dba_mb;          
    uint32_t algorithmic_lpm; 
    uint32_t algorithmic_acl; 
};


struct kaps_sw_model_op2_config
{
    uint32_t dev_type;         
    int32_t num_cores;
};




kaps_status kaps_sw_model_init(struct kaps_allocator *alloc, enum kaps_device_type type,
                             uint32_t flags, uint32_t kaps_sub_type, void **xpt);




kaps_status kaps_sw_model_init_with_profile(struct kaps_allocator *alloc, enum kaps_device_type type,
                             uint32_t flags, struct kaps_sw_model_kaps_config *config, void **xpt);




kaps_status kaps_sw_model_destroy(void *xpt);



kaps_status kaps_sw_model_get_stats(void *xpt, struct kaps_sw_model_stats *stats);



kaps_status kaps_sw_model_set_property(void *xpt, enum kaps_sw_model_property property, ...);

#ifdef __cplusplus
}
#endif
#endif 
