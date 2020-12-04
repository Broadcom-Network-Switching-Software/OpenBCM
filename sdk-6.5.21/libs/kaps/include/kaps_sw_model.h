

#ifndef INCLUDED_KAPS_MODEL_H
#define INCLUDED_KAPS_MODEL_H

#include <stdint.h>

#include "kaps_device.h"
#include "kaps_errors.h"
#include "kaps_init.h"

#ifdef __cplusplus
extern "C" {
#endif


enum kaps_sw_model_property
{
    KAPS_SW_MODEL_DUMP,    
    KAPS_SW_MODEL_UNROLL_COMPLEX_INST, 
    KAPS_SW_MODEL_TRACE,   
    KAPS_SW_MODEL_TRIG_EVENTS,  
    KAPS_SW_MODEL_INVALID  
};


enum kaps_model_tcam_format
{
    KAPS_MODEL_TCAM_FORMAT_1,
    KAPS_MODEL_TCAM_FORMAT_2
};



struct kaps_sw_model_kaps_config
{
    uint8_t id; 
    uint8_t sub_type;   
    uint16_t total_num_rpb; 
    uint16_t total_num_ads; 
    uint16_t is_small_bb_present; 
    uint16_t total_num_small_bb; 
    uint16_t total_num_ads2; 
    uint16_t total_num_bb;  
    uint16_t num_rows_in_rpb; 
    uint16_t num_rows_in_small_bb; 
    uint16_t num_rows_in_bb; 

    uint16_t profile;        

    uint16_t small_bb_start; 
    uint16_t small_bb_end;   
    uint16_t large_bb_start; 
    uint16_t large_bb_end;   

    uint16_t num_chained_bb;  

    uint16_t is_x_y_valid_bit_common; 

    enum kaps_model_tcam_format tcam_format; 
};





kaps_status kaps_sw_model_init(struct kaps_allocator *alloc, enum kaps_device_type, uint32_t flags,
                     uint32_t dev_id, uint32_t kaps_sub_type, uint32_t profile, void **xpt);




kaps_status kaps_sw_model_init_with_config(struct kaps_allocator *alloc, enum kaps_device_type type,
                             uint32_t flags, struct kaps_sw_model_kaps_config *config, void **xpt);





kaps_status kaps_sw_model_destroy(void *xpt);




kaps_status kaps_sw_model_set_property(void *xpt, enum kaps_sw_model_property property, ...);

#ifdef __cplusplus
}
#endif
#endif 
