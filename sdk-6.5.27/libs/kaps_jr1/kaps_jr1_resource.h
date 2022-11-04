

#ifndef __KAPS_JR1_RESOURCE_H
#define __KAPS_JR1_RESOURCE_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "kaps_jr1_device.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_errors.h"
#include "kaps_jr1_list.h"
#include "kaps_jr1_bitmap.h"



#ifdef __cplusplus
extern "C"
{
#endif

    struct kaps_jr1_db;
    struct kaps_jr1_instruction;
    struct kaps_jr1_device;
    struct kaps_jr1_key;
    struct memory_map;
    struct kaps_jr1_ab_info;
    struct kaps_jr1_ad_db;



#define MAX_PC_BRANCHES_PER_DB 4



#define INVALID_AB (0xFFFFFFFF)



#define FAILED_FIT_DBA  1



#define FAILED_FIT_AD   2


#define UDM_FULLY_OCCUPIED 0xF



#define MAX_DB_PER_SB 8



#define MAX_AD_BANKS 2

    struct kaps_jr1_db_hw_resource
    {
        int8_t ad_type;  
        struct kaps_jr1_c_list ab_list;
                                  
        struct kaps_jr1_c_list cas_ab_list;
                                   

        struct kaps_jr1_c_list rpb_ab_list;                             

        int8_t start_mcor;
                         
        int8_t end_mcor; 
        uint16_t kpu_bmp;
        const char *color;
                         
        uint8_t user_specified_algo_type;
                                      

        struct
        {
            uint8_t alloc_udm[KAPS_JR1_MAX_NUM_CORES][KAPS_JR1_UDM_PER_UDC][KAPS_JR1_ALLOC_UDM_MAX_COLS];
                                                                                           
            uint8_t max_lsn_size;         
            uint8_t max_lsn_size_budget;  
            uint16_t uda_mb;              
            uint8_t combined_lsn_size;    
            uint16_t combined_uda_mb;     
        } lsn_info[KAPS_JR1_HW_MAX_DT_PER_DB]; 

        struct
        {
            uint8_t rpt_bmp[HW_MAX_PCM_BLOCKS / KAPS_JR1_BITS_IN_BYTE];
                                                                
            uint8_t dba_bmp[KAPS_JR1_HW_MAX_AB / KAPS_JR1_BITS_IN_BYTE];
                                                             
            uint8_t num_dba_dt;
                            
            uint8_t num_sram_dt;
                             
            uint8_t max_lsn_size[MAX_PC_BRANCHES_PER_DB];
                                                      
        } user_specified;
                      
        uint32_t start_ait_sb;             
        uint32_t num_ait_sb;               
        uint32_t start_lpu;                
        uint32_t end_lpu;                  
        uint32_t start_small_bb_nr;        
        uint32_t end_small_bb_nr;          
        uint32_t num_small_bb_per_row;     
        uint32_t max_num_bricks_per_ab;        
        uint32_t small_bb_brick_bitmap_num_bytes; 

        uint8_t *small_bb_brick_bitmap;
                                                         
        uint16_t rpt_map[MAX_PC_BRANCHES_PER_DB];
                                                
        uint8_t rpt_width_8[MAX_PC_BRANCHES_PER_DB];
        uint32_t dba_budget:9;              
        uint32_t num_dt:4;   
        uint32_t num_allocated_dt:3;   
        uint32_t num_dba_dt:4;
                             
        uint32_t num_dba_dt_user:4;
                                 
        uint32_t algorithmic:1;      
        uint32_t num_bbs:8;  
      
    };


    typedef enum
    {
        AB_ANY,
        AB_NORMAL,
        AB_SMALL
    } ab_type_t;





    enum kaps_jr1_ad_type
    {
        KAPS_JR1_AD_TYPE_NONE,                     
        KAPS_JR1_AD_TYPE_CUSTOM,                   
        KAPS_JR1_AD_TYPE_INDEX,                    
        KAPS_JR1_AD_TYPE_INDIRECTION,              
        KAPS_JR1_AD_TYPE_INDIRECTION_WITH_DUPLICATION,
                                               
        KAPS_JR1_AD_TYPE_INPLACE                   
    };




    struct bb_partition_info
    {
        uint32_t start_row;        
        uint32_t num_rows;         
        struct kaps_jr1_db *owner;      
        struct bb_partition_info *next;
                                    
    };

    enum large_bb_config_type
    {
        KAPS_JR1_LARGE_BB_WIDTH_A000_B000 = 0x1,
        KAPS_JR1_LARGE_BB_WIDTH_A000_B480 = 0x2,
        KAPS_JR1_LARGE_BB_WIDTH_A000_B960 = 0x4,
        KAPS_JR1_LARGE_BB_WIDTH_A480_B000 = 0x8,
        KAPS_JR1_LARGE_BB_WIDTH_A480_B480 = 0x10,
        KAPS_JR1_LARGE_BB_WIDTH_A960_B000 = 0x20
    };



    struct bb_info
    {
        uint32_t width_1;          
        struct bb_partition_info *row;
                                    
        enum large_bb_config_type bb_config;
                                         
        uint32_t bb_config_options;
                                
        uint32_t bb_num_rows;
                          
        uint32_t num_units_in_bb;
                              
    };






    struct kaps_jr1_rpb_info
    {
        uint16_t rpt_width;
                        
        uint16_t rpt_depth;
                        
        struct kaps_jr1_db *db;
                            
    };





    struct memory_map
    {
        struct kaps_jr1_ab_info *ab_memory;          
        struct kaps_jr1_ab_info *rpb_ab;             

        struct kaps_jr1_rpb_info rpb_info[HW_MAX_PCM_BLOCKS];  

        uint8_t num_bb;                          
        struct bb_info *bb_map; 


        uint8_t num_small_bb;                    
        struct bb_info *small_bb;                

    };


    struct kaps_jr1_hb_bank_list
    {
        uint8_t start_sb;           
        uint8_t num_sb;             
        uint8_t bank_no;            
        uint32_t num_idx;           
        struct kaps_jr1_hb_bank_list *next;
                                     
    };


    struct kaps_jr1_hb_db_hw_resource
    {
        struct kaps_jr1_hb_bank_list *hb_bank_list;
                                             
    };


    struct kaps_jr1_ad_db_hw_resource
    {
        enum kaps_jr1_ad_type ad_type;  
        int8_t dynamic_ad_chunk;
                             

        struct kaps_jr1_hb_db_hw_resource *hb_res;
                                           
    };



    kaps_jr1_status kaps_jr1_resource_init_module(
    struct kaps_jr1_device *device);



    kaps_jr1_status kaps_jr1_resource_init_mem_map(
    struct kaps_jr1_device *main_device);


    struct memory_map *resource_kaps_jr1_get_memory_map(
    struct kaps_jr1_device *device);


    struct kaps_jr1_db *kaps_jr1_resource_get_res_db(
    struct kaps_jr1_db *db);



    void kaps_jr1_resource_fini_module(
    struct kaps_jr1_device *device);



    kaps_jr1_status kaps_jr1_resource_add_database(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    uint32_t id,
    uint32_t capacity,
    uint32_t width_1,
    enum kaps_jr1_db_type type);



    void kaps_jr1_resource_free_database(
    struct kaps_jr1_device *dev,
    struct kaps_jr1_db *db);



    kaps_jr1_status kaps_jr1_resource_db_set_key(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    struct kaps_jr1_key *key);



    kaps_jr1_status kaps_jr1_resource_db_add_ad(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    struct kaps_jr1_db *ad);



    kaps_jr1_status kaps_jr1_resource_db_add_hb(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    struct kaps_jr1_db *hb_db);





    kaps_jr1_status kaps_jr1_resource_finalize(
    struct kaps_jr1_device *device);



    uint32_t kaps_jr1_resource_normalized_ad_width(
    struct kaps_jr1_device *device,
    uint32_t ad_width_1);





    kaps_jr1_status kaps_jr1_resource_set_algorithmic(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    int32_t value);


    void kaps_jr1_resource_set_user_specified_algo_mode(
    struct kaps_jr1_db *db);


    uint8_t kaps_jr1_resource_get_is_user_specified_algo_mode(
    struct kaps_jr1_db *db);





    uint32_t kaps_jr1_resource_get_ab_usage(
    struct kaps_jr1_device *this_device,
    struct kaps_jr1_db *db);



    void kaps_jr1_resource_print_ab_info(
    struct kaps_jr1_db *db,
    FILE * fp);




    void kaps_jr1_resource_print_db_capacity(
    FILE * fp,
    struct kaps_jr1_db *db);



    kaps_jr1_status kaps_jr1_resource_dynamic_ad_alloc(
    struct kaps_jr1_db *ad,
    uint8_t * sb_bitmap,
    int32_t num_sb_needed);







    enum kaps_jr1_ad_type kaps_jr1_resource_get_ad_type(
    struct kaps_jr1_db *db);



    void kaps_jr1_resource_set_ad_type(
    struct kaps_jr1_db *db,
    enum kaps_jr1_ad_type ad_type);




    struct kaps_jr1_c_list *kaps_jr1_resource_get_ab_list(
    struct kaps_jr1_db *db);




    uint8_t kaps_jr1_db_get_algorithmic(
    const struct kaps_jr1_db *db);


    kaps_jr1_status kaps_jr1_resource_wb_pre_process(
    struct kaps_jr1_device *device);




    kaps_jr1_status kaps_jr1_standalone_resource_assign_rpbs(
    struct kaps_jr1_device *device);


    kaps_jr1_status resource_db_set_user_specified(
    struct kaps_jr1_db *db,
    uint32_t resource_value,
    ...);


    kaps_jr1_status resource_release_all_resources(
    struct kaps_jr1_db *db);







    int kaps_jr1_print_to_file(
    struct kaps_jr1_device *device,
    FILE * fp,
    const char *fmt,
    ...);



#ifdef __cplusplus
}
#endif
#endif
