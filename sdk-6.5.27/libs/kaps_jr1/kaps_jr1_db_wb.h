

#ifndef __KAPS_JR1_DB_WB_H
#define __KAPS_JR1_DB_WB_H

#include "kaps_jr1_portable.h"
#include "kaps_jr1_list.h"
#include "kaps_jr1_db.h"
#include "kaps_jr1_resource.h"
#include "kaps_jr1_bitmap.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define INVALID_PAIRED_AB_NUM (0xFFFF)

    struct kaps_jr1_ab_wb_info
    {
        uint16_t ab_num; 
        uint16_t num_slots;
                         
        uint8_t conf;    
        uint8_t blk_cleared;
                         
        uint8_t device_no;
                         
        uint8_t translate;
                         
        uint16_t paired_ab_num;    
        uint32_t base_addr;     
        uint32_t base_idx;     
        struct kaps_jr1_db *db;
                          
        uint32_t is_dup_ab:1;
                          
    };

    struct kaps_jr1_ad_db_wb_info
    {
        uintptr_t stale_ptr;          
        uintptr_t hb_ad_stale_ptr;    
        uint32_t id;                  
        uint32_t ad_width;            
        uint32_t capacity;            
        uint8_t user_specified;   
        int32_t uda_mb;             
        uint32_t num_entries_in_ad_db[KAPS_JR1_MAX_NUM_CORES]; 
        struct kaps_jr1_ad_db_hw_resource ad_hw_res;
                                             
    };

    struct kaps_jr1_db_wb_pio_stats
    {
        uint32_t num_of_piowrs;
                            
        uint32_t num_of_piords;
                            
        uint32_t num_of_shuffles;
                              
        uint32_t num_of_worst_case_shuffles;
                                         
        uint32_t num_ix_cbs;
                         
        uint32_t num_blk_ops;
                          
        uint32_t num_ad_writes;
                            
    };

    struct kaps_jr1_db_wb_info
    {
        uintptr_t stale_ptr;          
        uint32_t tid;                 
        uint32_t capacity;            
        int32_t max_capacity;      
        uintptr_t clone_of;           
        enum kaps_jr1_db_type type;        
        uint16_t desc;                
        uint8_t meta_priority;       
        uint32_t enable_dynamic_allocation:1;
                                          
        uint32_t is_table:1;          
        uint32_t is_clone:1;          
        uint32_t has_clones:1;        
        uint32_t has_tables:1;        
        uint32_t user_specified:1;    
        uint32_t callback_data:1;     
        uint32_t is_algorithmic:1;    
        uint32_t is_user_specified_algo_type:1;
                                            
        uint32_t smt_no:1;            
        uint32_t calc_shuffle_stats:1;
        uint32_t has_dup_ad:1;        
        uint32_t is_algo:1;           
        uint32_t is_main_bc_db:1;       
        uint32_t is_bc_required:1;    

        uint32_t mapped_to_acl:1;     
        uint32_t is_em:1;             
        uint32_t has_hb:1;            
        uint32_t is_cascaded:1;       
        uint32_t hb_id;               
        uint32_t hb_capacity;         
        uint32_t hb_age_count;        
        uintptr_t hb_stale_ptr;       
        uint32_t defer_deletes;       

        uint32_t num_ad_databases;    
        uint32_t alg_type;            
        struct kaps_jr1_print_key *key;    
        struct kaps_jr1_db_wb_pio_stats pio_stats;
                                           
        uint32_t bc_bitmap;           
        uint32_t user_num_ab;         
        uint32_t user_uda_mb;         
        uint32_t index_range_min;     
        uint32_t index_range_max;     

        uint32_t num_algo_levels_in_db; 
        kaps_jr1_db_index_callback callback_fn;
                                       
        void *callback_handle;        
        enum kaps_jr1_db_insertion_mode insertion_mode;   

        char description[];          
    };

    struct kaps_jr1_wb_cb_functions
    {
        kaps_jr1_device_issu_read_fn read_fn;
                                        
        kaps_jr1_device_issu_write_fn write_fn;
                                        
        void *handle;                  
        uint32_t *nv_offset;           
        uint8_t *nv_ptr;               
    };

    struct kaps_jr1_cr_table_entry
    {
        uint32_t offset:24;          
        uint32_t is_valid:8;         
    };

    enum kaps_jr1_cr_entry_status
    {
        KAPS_JR1_CR_ENTRY_UNHANDLED = 0,
        KAPS_JR1_CR_ENTRY_COMMITED = 1,
        KAPS_JR1_CR_ENTRY_DELETE = 2,
        KAPS_JR1_CR_DANGLING_AD = 3,
    };


    kaps_jr1_status kaps_jr1_db_wb_save_info(
    struct kaps_jr1_db *db,
    struct kaps_jr1_wb_cb_functions *wb_fn);


    kaps_jr1_status kaps_jr1_db_wb_restore_info(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    struct kaps_jr1_wb_cb_functions *wb_fn);



    kaps_jr1_status kaps_jr1_db_reconcile_start(
    struct kaps_jr1_db *db);



    kaps_jr1_status kaps_jr1_db_reconcile_end(
    struct kaps_jr1_db *db);



    kaps_jr1_status kaps_jr1_lpm_wb_save_state(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fn);



    kaps_jr1_status kaps_jr1_lpm_wb_restore_state(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fn);



    kaps_jr1_status kaps_jr1_lpm_wb_pre_processing(
    struct kaps_jr1_device *device);



    kaps_jr1_status kaps_jr1_lpm_wb_post_processing(
    struct kaps_jr1_device *device);


    kaps_jr1_status kaps_jr1_lpm_cr_save_state(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fn);


    kaps_jr1_status kaps_jr1_lpm_cr_restore_state(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fn);


    kaps_jr1_status kaps_jr1_lpm_cr_get_adv_ptr(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun);



    uint32_t kaps_jr1_db_entry_get_nv_offset(
    struct kaps_jr1_db *db,
    uint32_t user_handle);

#ifdef __cplusplus
}
#endif
#endif
