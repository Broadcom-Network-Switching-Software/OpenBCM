

#ifndef __KAPS_JR1_MODEL_H
#define __KAPS_JR1_MODEL_H

#include <stdio.h>

#include "kaps_jr1_allocator.h"
#include "kaps_jr1_sw_model.h"
#include "kaps_jr1_model_ads.h"
#include "kaps_jr1_model_bb.h"
#include "kaps_jr1_model_rpb.h"
#include "kaps_jr1_model_hb.h"

#ifdef __cplusplus
extern "C" {
#endif





struct kaps_jr1_model_stats
{
    uint32_t num_rpb_writes;    
    uint32_t num_ads_writes;    
    uint32_t num_ads2_writes;   
    uint32_t num_small_bb_writes; 
    uint32_t num_bb_writes;     
    
    uint32_t num_hb_writes;     
    uint32_t num_hb_reads;      
    uint32_t num_hb_dumps;      
    uint32_t num_hb_copy;       
    uint32_t num_rpb_misses;    
    uint32_t num_ads_lmpsofar_valid_hits; 
    uint32_t num_ads_bpm_hits; 
    uint32_t num_ads2_lmpsofar_valid_hits; 
    uint32_t num_ads2_bpm_hits; 

};

#define KAPS_JR1_INVALID_BB_NR (0xFFFF)

typedef struct kaps_jr1_model_coherency_search_t
{
        uint32_t interface;
        uint8_t rpb_key[20];
        uint8_t expected_ad[KAPS_JR1_MAX_AD_WIDTH_8];
} kaps_jr1_model_coherency_search;


typedef struct kaps_jr1_tcam_log_file_entry_t
{
    uint32_t row_nr;
    char *tcam_entry_str; 
} kaps_jr1_tcam_log_file_entry;




struct kaps_jr1_c_model
{
    uint32_t   id;          

    enum kaps_jr1_device_pair_type pair_type;          
    
    uint32_t sub_type;          

    uint32_t profile;       

    FILE *log_file;         

    uint32_t debug_mode;    

    struct kaps_jr1_sw_model_config config;

    struct kaps_jr1_allocator *alloc; 

    struct kaps_jr1_c_model_rpb *rpb_blocks[KAPS_JR1_HW_MAX_NUM_RPB_BLOCKS]; 

    struct kaps_jr1_c_model_ads_block *ads_blocks[KAPS_JR1_MAX_NUM_ADS_BLOCKS]; 

    struct kaps_jr1_c_model_bb *bucket_blocks[KAPS_JR1_MAX_NUM_BB]; 

    struct kaps_jr1_c_model_bb *small_bbs[KAPS_JR1_TOTAL_NUM_SMALL_BB];

    struct kaps_jr1_c_model_ads_block *ads2_blocks[KAPS_JR1_HW_MAX_NUM_ADS2_BLOCKS];

    struct kaps_jr1_c_model_hb_block *rpb_hb_blocks[KAPS_JR1_HW_MAX_NUM_RPB_BLOCKS]; 

    struct kaps_jr1_c_model_hb_block *bb_hb_blocks[KAPS_JR1_MAX_NUM_BB]; 

    struct kaps_jr1_c_model_hb_block *small_bb_hb_blocks[KAPS_JR1_TOTAL_NUM_SMALL_BB]; 

    uint32_t ads2_depth[KAPS_JR1_HW_MAX_NUM_ADS2_BLOCKS]; 

    uint16_t format_map_to_gran[KAPS_JR1_MAX_NUM_GRANULARITIES + 1]; 

    uint16_t small_bb_format_map_to_gran[KAPS_JR1_MAX_NUM_GRANULARITIES + 1]; 

    uint8_t search_tag; 

    uint8_t is_hb_supported; 

    uint32_t max_num_levels; 

    uint8_t num_levels_in_search[KAPS_JR1_HW_MAX_NUM_RPB_BLOCKS]; 

    struct kaps_jr1_model_stats stats;  

    uint32_t was_log_file_set;  

    uint32_t perform_coherency_search;

    uint32_t coherency_search_cnt;

    uint8_t coherency_rpb_search_key[KAPS_JR1_LPM_KEY_MAX_WIDTH_8];

    struct kaps_jr1_search_result coherency_search_result;

    uint8_t coherency_expected_ad_value[KAPS_JR1_MAX_AD_WIDTH_8];

    uint32_t ad_width_1;

};

#ifdef __cplusplus
}
#endif

#endif 
