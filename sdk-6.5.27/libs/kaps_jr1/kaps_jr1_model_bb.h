

#ifndef __KAPS_JR1_MODEL_BB_H
#define __KAPS_JR1_MODEL_BB_H

#include "kaps_jr1_portable.h"
#include "kaps_jr1_errors.h"
#include "kaps_jr1_hw_limits.h"


#ifdef __cplusplus
extern "C" {
#endif




struct kaps_jr1_c_model;


#define KAPS_JR1_BB_KEY_WIDTH_1 (168)

#define KAPS_JR1_BB_KEY_WIDTH_8 (21)

#define KAPS_JR1_INVALID_SEARCH_INTERFACE (0xFFFF)


struct kaps_jr1_c_model_bb_pfx {
    uint8_t pfx_data[KAPS_JR1_BB_KEY_WIDTH_8]; 
    uint16_t num_bits_to_compare; 
};



struct kaps_jr1_c_model_bb_result {
    uint32_t bb_nr; 
    uint32_t row_nr; 
    int32_t is_hit; 
    uint32_t hit_index; 
    uint32_t match_ad; 
    uint32_t match_len; 
};



struct kaps_jr1_c_model_bb_entry
{
    uint8_t entry[KAPS_JR1_BKT_WIDTH_8]; 
};



struct kaps_jr1_c_model_bb {
    struct kaps_jr1_c_model *m;   
    struct kaps_jr1_c_model_bb_entry rows[KAPS_JR1_BKT_MAX_NUM_ROWS]; 
    uint8_t bb_global_config_reg[KAPS_JR1_REGISTER_WIDTH_8]; 
    uint8_t abs_bkt_nr; 
    uint8_t bkt_nr_in_bb_cascade;  
    uint8_t lpu_nr; 
    uint32_t bb_config_options; 
    enum large_bb_config_type bb_config; 
    uint32_t bb_num_rows;                
    uint32_t search_interface_num; 
};



kaps_jr1_status 
kaps_jr1_c_model_bb_create(
    struct kaps_jr1_c_model *m, 
    uint8_t abs_bkt_nr,
    uint32_t is_small_bb,
    struct kaps_jr1_c_model_bb ** bb_pp);


kaps_jr1_status kaps_jr1_c_model_bb_destroy(struct kaps_jr1_c_model_bb * bb);


kaps_jr1_status kaps_jr1_c_model_bb_write(struct kaps_jr1_c_model_bb * bb, uint32_t row_nr, uint8_t * data);



kaps_jr1_status kaps_jr1_c_model_bb_global_config_write(struct kaps_jr1_c_model_bb * bb, uint8_t * data);




kaps_jr1_status kaps_jr1_c_model_bb_read(struct kaps_jr1_c_model_bb * bb, uint32_t row_nr, uint8_t * data);


kaps_jr1_status kaps_jr1_c_model_bb_compare(
    struct kaps_jr1_c_model_bb * bb,
    uint32_t search_interface,
    uint8_t * bb_key, 
    uint16_t row_nr, 
    uint16_t gran,
    uint16_t bb_type,
    struct kaps_jr1_c_model_bb_result * result);


kaps_jr1_status kaps_jr1_c_model_bb_print(struct kaps_jr1_c_model_bb *bb);



void kaps_jr1_c_model_bb_debug(
    struct kaps_jr1_c_model_bb *bb,
    uint32_t bb_type_bit,
    uint32_t row_nr, 
    uint32_t gran);

#ifdef __cplusplus
}
#endif

#endif


