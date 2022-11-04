

#ifndef __KAPS_JR1_ALGO_HW_H
#define __KAPS_JR1_ALGO_HW_H

#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_resource_algo.h"
#include "kaps_jr1_device_alg.h"

#ifdef __cplusplus
extern "C"
{
#endif









    struct kaps_jr1_pct
    {
        union
        {
            struct kaps_jr1_ads kaps_jr1;
        } u;
    };



    enum kaps_jr1_device_advanced_properties
    {
        KAPS_JR1_DEVICE_PROP_ADV_PRINT = 1000,  
        KAPS_JR1_DEVICE_PROP_NO_COMPRESSION,    
        KAPS_JR1_DEVICE_ADV_PROP_XML_DUMP_FMT,  
        KAPS_JR1_DEVICE_PROP_LPT_MODE,          
        KAPS_JR1_DEVICE_PROP_ENABLE_COMPACTION,  
        KAPS_JR1_DEVICE_ADV_PROP_INVALID         
    };

    



    struct kaps_jr1_device_resource
    {
        struct kaps_jr1_simple_dba *pcm_dba_mgr[HW_MAX_PCM_BLOCKS];
                                                            
        uint16_t num_rpb_blks;
                             

        uint16_t num_rows_in_rpb[KAPS_JR1_HW_MAX_NUM_RPB_BLOCKS];  

        FILE *vector_logger;
        uint16_t dba_width_1;
                            
        uint16_t sram_dt_per_db;
                             
        uint16_t dba_dt_per_db;
                             

        uint16_t lpu_word_size_8;
                             

        uint16_t inplace_width_1;
                                  

        uint16_t total_num_bb;
                            
        uint16_t num_bb_in_one_bb_cascade;
                             
        uint16_t bb_width_1; 
        uint32_t num_rows_in_bb; 
        
        uint16_t total_lpus;
        uint16_t max_lpu_per_db; 

        uint16_t max_num_uda_chunk_levels;
        
        uint16_t total_small_bb;
                             
        uint16_t num_rows_in_small_bb;
                                   

        uint32_t num_ads2_blks; 
        uint32_t ads2_depth[KAPS_JR1_HW_MAX_NUM_ADS2_BLOCKS];  
        uint32_t max_ads2_x_table_size; 
        uint32_t ads2_rpt_lmpsofar_virtual_ix_start; 

        uint16_t lpm_gran_array[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
                                                       
        uint16_t lpm_middle_level_gran_array[KAPS_JR1_HW_MAX_NUM_LPU_GRAN];
                                                                    

        uint16_t lpm_num_gran;
                            
        uint16_t lpm_middle_level_num_gran;
                                        

        uint32_t incr_in_bbs;    
        uint32_t has_algorithmic_lpm:1;
                                    
        uint32_t device_print_advance:2;
                                     
        uint32_t no_overflow_lpm:1; 
        uint32_t disable_header_print:1;
                                     

        uint32_t num_algo_levels:2;
                                

        uint32_t reduced_algo_levels:1;
                                    

        uint32_t num_db_in_one_group:4; 

        void *it_mgr;            

        void *res_hdl;           

        uint64_t running_sum;
                          

        uint32_t ads_width_1;  
        uint32_t ads_width_8;  

        uint32_t num_daisy_chains; 
        uint32_t is_hw_mapped_ix;  

    };


    kaps_jr1_status kaps_jr1_algo_hw_write_rpt_data(
    struct kaps_jr1_device *device,
    uint8_t rpt_num,
    uint8_t * data,
    uint8_t * mask,
    uint16_t entry_nr);


    kaps_jr1_status kaps_jr1_algo_hw_delete_rpt_data(
    struct kaps_jr1_device *device,
    uint8_t rpt_num,
    uint16_t entry_nr);


    kaps_jr1_status kaps_jr1_algo_hw_write_pct(
    struct kaps_jr1_device *device,
    struct kaps_jr1_pct *pct,
    uint8_t pct_num,
    uint16_t entry_nr);


    kaps_jr1_status algo_hw_write_rpt_block_config(
    struct kaps_jr1_device *device,
    uint8_t enable);


    kaps_jr1_status algo_hw_power_up_rpt(
    struct kaps_jr1_device *device);



    kaps_jr1_status algo_hw_init_rpt_uit(
    struct kaps_jr1_device *device);


    kaps_jr1_status kaps_jr1_instruction_finalize_op_adv(
    struct kaps_jr1_instruction *instruction);


    kaps_jr1_status write_op_srch_attr_ctrl_dynamic(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    uint8_t end_udc);


    kaps_jr1_status write_op_udc_ctrl_dynamic(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    int32_t start_row,
    int32_t start_lpu,
    int32_t num_row,
    int32_t num_lpu,
    int32_t dt_index);


    kaps_jr1_status kaps_jr1_instruction_finalize_12k_adv(
    struct kaps_jr1_instruction *instruction);



    kaps_jr1_status kaps_jr1_device_advanced_enable_udm(
    struct kaps_jr1_device *device,
    uint32_t udmno,
    int32_t is_lsn,
    int32_t is_xor_ad,
    int32_t is_udm_pair,
    uint8_t enable_udm);



    kaps_jr1_status kaps_jr1_device_advanced_enable_udc_xor(
    struct kaps_jr1_device *device,
    uint32_t udcno,
    uint8_t enable_xor);



    kaps_jr1_status kaps_jr1_device_save_shadow(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun);



    kaps_jr1_status kaps_jr1_device_restore_shadow(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun);


    kaps_jr1_status kaps_jr1_device_alloc_dt_dynamic(
    struct kaps_jr1_db *db_main);


    kaps_jr1_status kaps_jr1_device_delete_dt_dynamic(
    struct kaps_jr1_db *db_main);


    kaps_jr1_status kaps_jr1_device_lpm_memory_stats(
    struct kaps_jr1_device *device,
    FILE * fp);





#ifdef __cplusplus
}
#endif
#endif
