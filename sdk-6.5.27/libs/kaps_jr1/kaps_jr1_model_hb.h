

#ifndef __KAPS_JR1_MODEL_HB_H
#define __KAPS_JR1_MODEL_HB_H

#include "kaps_jr1_portable.h"
#include "kaps_jr1_errors.h"
#include "kaps_jr1_hw_limits.h"


#ifdef __cplusplus
extern "C" {
#endif




struct kaps_jr1_c_model;


#define KAPS_JR1_HB_MAX_NUM_ROWS (16 * 1024)



struct kaps_jr1_c_model_hb_entry
{
    uint8_t entry[KAPS_JR1_HB_ROW_WIDTH_8]; 
};



struct kaps_jr1_c_model_hb_block {
    struct kaps_jr1_c_model *m;   
    struct kaps_jr1_c_model_hb_entry rows[KAPS_JR1_HB_MAX_NUM_ROWS]; 
    uint8_t abs_hb_blk_nr; 
    uint8_t blk_nr_in_bb_cascade;  
    uint8_t lpu_nr; 
};


kaps_jr1_status kaps_jr1_c_model_hb_create(struct kaps_jr1_c_model *m, uint8_t abs_bkt_nr, struct kaps_jr1_c_model_hb_block ** hb_pp);


kaps_jr1_status kaps_jr1_c_model_hb_destroy(struct kaps_jr1_c_model_hb_block * hb_block);




kaps_jr1_status kaps_jr1_c_model_hb_write(struct kaps_jr1_c_model_hb_block * hb_block, uint32_t row_nr, uint8_t * data);



kaps_jr1_status kaps_jr1_c_model_hb_read(struct kaps_jr1_c_model_hb_block * hb_block, uint32_t row_nr, uint8_t * data);


kaps_jr1_status kaps_jr1_c_model_hb_set(struct kaps_jr1_c_model_hb_block * hb_block, uint32_t row_nr, uint32_t bit_num);



#ifdef __cplusplus
}
#endif


#endif

