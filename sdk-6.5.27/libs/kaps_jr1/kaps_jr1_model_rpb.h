

#ifndef __KAPS_JR1_MODEL_RPB_H
#define __KAPS_JR1_MODEL_RPB_H

#include "kaps_jr1_portable.h"
#include "kaps_jr1_errors.h"


#ifdef __cplusplus
extern "C" {
#endif





struct kaps_jr1_c_model;



struct kaps_jr1_c_model_rpb_entry
{
    uint8_t entry[KAPS_JR1_RPB_WIDTH_8]; 
    uint8_t valid_bit; 
};


struct kaps_jr1_c_model_rpb
{
    struct kaps_jr1_c_model *model;  
    struct kaps_jr1_c_model_rpb_entry rows[ 2 * KAPS_JR1_RPB_MAX_BLOCK_SIZE]; 
};




kaps_jr1_status kaps_jr1_c_model_rpb_create(struct kaps_jr1_c_model *m, uint8_t rpb_id, struct kaps_jr1_c_model_rpb **rpb_pp);


kaps_jr1_status kaps_jr1_c_model_rpb_destroy(struct kaps_jr1_c_model_rpb *rpb);


kaps_jr1_status kaps_jr1_c_model_rpb_write(struct kaps_jr1_c_model_rpb *rpb, uint32_t row_nr, uint8_t * data, uint8_t is_valid);


kaps_jr1_status kaps_jr1_c_model_rpb_read(struct kaps_jr1_c_model_rpb *rpb, uint32_t row_nr, uint8_t * data, uint8_t *is_valid);


kaps_jr1_status kaps_jr1_c_model_rpb_compare(struct kaps_jr1_c_model_rpb *rpb, uint32_t rpb_nr, uint8_t * rpb_key, int32_t * entry_index);


#ifdef __cplusplus
}
#endif


#endif
