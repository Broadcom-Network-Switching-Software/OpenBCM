
#ifndef __KAPS_JR1_MODEL_XPT_H
#define __KAPS_JR1_MODEL_XPT_H

#include "kaps_jr1_errors.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_allocator.h"
#include "kaps_jr1_xpt.h"

#ifdef __cplusplus
extern "C" {
#endif




struct kaps_jr1_c_model;
struct kaps_jr1_ads;
struct kaps_jr1_sw_model_user_config;







struct kaps_jr1_model_xpt
{
    struct kaps_jr1_xpt xpt_common; 
    struct kaps_jr1_c_model *model;      
    struct kaps_jr1_allocator *alloc;    
};




kaps_jr1_status kaps_jr1_c_model_xpt_init(enum kaps_jr1_device_pair_type pair_type,
                                uint32_t dev_id, uint32_t kaps_jr1_sub_type, uint32_t profile,
                                struct kaps_jr1_model_xpt **xpt);



kaps_jr1_status kaps_jr1_c_model_xpt_set_log(struct kaps_jr1_model_xpt *xpt, FILE *fp);






kaps_jr1_status kaps_jr1_c_model_xpt_destroy(struct kaps_jr1_model_xpt *xpt);





#ifdef __cplusplus
}
#endif

#endif



