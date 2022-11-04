

#ifndef __KAPS_JR1_DEVICE_INTERNAL_H
#define __KAPS_JR1_DEVICE_INTERNAL_H

#include <stdint.h>
#include "kaps_jr1_device.h"
#include "kaps_jr1_xpt.h"
#include "kaps_jr1_list.h"
#include "kaps_jr1_legacy.h"
#include "kaps_jr1_internal_errors.h"
#include "kaps_jr1_hb_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define KAPS_JR1_NV_MAGIC_NUMBER                         (0x12121212)


#define BUILD_VERSION_STR_(a, b, c) a b c
#define BUILD_VERSION_STR(a, b, c) BUILD_VERSION_STR_(a, b, c)

#ifndef KAPS_JR1_DEBUG_SUFFIX
#define KAPS_JR1_DEBUG_SUFFIX ""
#endif

#ifndef KAPS_JR1_BUILD_DATE
#define KAPS_JR1_BUILD_DATE ""
#endif

#define KAPS_JR1_SDK_VERSION_FULL BUILD_VERSION_STR(KAPS_JR1_SDK_VERSION, KAPS_JR1_BUILD_DATE, KAPS_JR1_DEBUG_SUFFIX)



    struct kaps_jr1_shadow_device;
    struct kaps_jr1_ad_db;
    struct kaps_jr1_ad_chunk;
    struct kaps_jr1_ad;



    char *kaps_jr1_device_db_name(
    struct kaps_jr1_db *db);

    enum kaps_jr1_issu_status
    {
        KAPS_JR1_ISSU_INIT = 0,         
        KAPS_JR1_ISSU_SAVE_COMPLETED = 1,
                                    
        KAPS_JR1_ISSU_RESTORE_START = 2,
        KAPS_JR1_ISSU_RESTORE_END = 3,  
        KAPS_JR1_ISSU_RECONCILE_START = 4,
                                    
        KAPS_JR1_ISSU_RECONCILE_END = 5 
    };



    struct kaps_jr1_nv_memory_manager
    {
        uint32_t num_db;

        uint32_t offset_device_info_start;

        uint32_t offset_device_pending_list;

        uint32_t offset_device_lpm_info;

        uint32_t offset_device_lpm_shadow_info;

        uint32_t offset_device_hb_info;

        uint32_t *offset_db_trie_has_been_built;

        uint32_t *offset_db_trie_sb_bitmap;

        uint32_t *offset_db_info_start;
        uint32_t *offset_db_info_end;

        struct kaps_jr1_cr_pool_mgr *kaps_jr1_cr_pool_mgr;
    };

    struct kaps_jr1_device_property
    {
        uint32_t lpt_mode:1;                
        uint32_t read_type:2;               
        uint32_t return_error_on_asserts:1; 
        uint32_t print_heap_usage:1;        
    };

    struct kaps_jr1_seq_num_to_ptr
    {
        struct kaps_jr1_db **db_ptr;       
        struct kaps_jr1_ad_db **ad_db_ptr; 
        struct kaps_jr1_hb_db **hb_db_ptr; 
        uint32_t is_memory_allocated;    
        uint32_t num_db;              
        uint32_t num_ad_db;           
        uint32_t num_hb_db;           
    };

#define KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(d, e, ad)                                              \
    {                                                                                           \
        uint32_t ad_db_seq_num_merged = ((e)->ad_db_seq_num_0 | ((e)->ad_db_seq_num_1 << 5));   \
        kaps_jr1_sassert (ad_db_seq_num_merged);                                                          \
        kaps_jr1_sassert (ad_db_seq_num_merged <= d->seq_num_to_ptr->num_ad_db);                          \
        kaps_jr1_sassert (d->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num_merged]);                            \
        ad = d->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num_merged];                                \
    }

#define KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(d, e, db)                          \
    {                                                                    \
        kaps_jr1_sassert ((e)->db_seq_num);                                        \
        kaps_jr1_sassert ((e)->db_seq_num <= d->seq_num_to_ptr->num_db);           \
        kaps_jr1_sassert (d->seq_num_to_ptr->db_ptr[(e)->db_seq_num]);             \
        db = d->seq_num_to_ptr->db_ptr[(e)->db_seq_num];                 \
    }


enum kaps_jr1_tcam_format 
    {
        KAPS_JR1_TCAM_FORMAT_1,
        KAPS_JR1_TCAM_FORMAT_2,
        KAPS_JR1_TCAM_FORMAT_3
    };



enum kaps_jr1_xpt_version
{
    KAPS_JR1_XPT_VERSION_1,
    KAPS_JR1_XPT_VERSION_2
};


typedef struct kaps_jr1_prefix_log_record_t
{
    uint8_t operation;
    uint8_t db_id;
    uint8_t pfx_len;
    uint8_t pfx_data[20];
    uint8_t pfx_ad[KAPS_JR1_MAX_AD_WIDTH_8];
} kaps_jr1_prefix_log_record;




    struct kaps_jr1_device
    {
        struct kaps_jr1_allocator *alloc;        
        enum kaps_jr1_device_type type;          
        enum kaps_jr1_device_pair_type pair_type; 
        
        struct kaps_jr1_device_resource *hw_res; 

        struct kaps_jr1_shadow_device *kaps_jr1_shadow; 

        struct kaps_jr1_device *main_dev;        
        struct kaps_jr1_device *next_dev;        
        struct kaps_jr1_device *smt;             

        struct kaps_jr1_device *other_core;      
        struct memory_map *map;             
        struct kaps_jr1_c_list db_list;              
        struct kaps_jr1_c_list inst_list;            
        struct kaps_jr1_lpm_mgr *lpm_mgr;        
        struct kaps_jr1_device_config config;    
        struct kaps_jr1_nlm_allocator nlm_alloc;    
        struct kaps_jr1_xpt *xpt;                          
        FILE *log_device_state;             
        FILE *dynamic_alloc_fp;             
        enum kaps_jr1_issu_status issu_status;   
        struct kaps_jr1_device_property prop;    
        char *description;                  
        const char *lpm_color;              
        enum kaps_jr1_xpt_version xpt_version;  
        uint8_t num_ab_per_sb;              
        uint8_t num_dba_sb;                 
        uint8_t max_num_clone_parallel_lookups;
                                            
        uint16_t num_ab; 
        uint16_t max_ad_width_1;            

        uint16_t num_hb_blocks;             
        uint16_t num_rows_in_hb_block;      
        uint16_t num_small_hb_blocks;       
        uint16_t num_rows_in_small_hb_block; 

        uint32_t max_search_key_width_1;    

        uint32_t is_config_locked:1;        
        uint32_t smt_no:1;                  
        uint32_t device_no:3;               
        uint32_t issu_in_progress:1;        
        uint32_t lpm_lock_done:1;           
        uint32_t silicon_sub_type:4;        

        uint32_t debug_level:3;             
        uint32_t dump_xml_data_with_entries:1;
                                            
        uint32_t is_alg_db_present:1;       
        uint32_t is_wb_continue:1;          
        uint32_t is_generic_bc_device:1;    

        uint32_t txn_in_progress:1;                         
        uint32_t nv_shadow:1;                               
        uint32_t fatal_transport_error:1;                   

        uint32_t core_id:1;                                 
        uint32_t num_cores:2;                               
        uint32_t instruction_cascading_present:1;           
        uint32_t inplace_ad_present:1;                      

        uint32_t map_print:1;                               

        uint32_t execute_the_next_loop:1;                   
        uint32_t id;                                        
        uint16_t max_num_searches;                          
        uint32_t flags;                                     
        uint32_t dba_offset;                                
        uint32_t uda_offset;                                
        uint32_t small_bb_offset;                           
        uint32_t large_bb_mb;                               

        uint32_t num_of_piowrs;                             
        uint32_t num_of_piords;                             
        uint32_t num_blk_ops;                               

        uint32_t next_algo_color;                           

        struct kaps_jr1_aging_entry *aging_table;               
        struct kaps_jr1_aging_entry *small_aging_table;         
        uint32_t aging_table_size;                          
        uint32_t small_aging_table_size;                    
        uint8_t *aging_valid_memory;                        
        uint8_t *hb_buffer;                                 
        uint8_t *hb_vbuffer;                                

        struct kaps_jr1_device *main_bc_device;                  
        struct kaps_jr1_device *next_bc_device;                  
        uint32_t bc_id;                                     
        uint32_t db_bc_bitmap;                              
        uint32_t generic_bc_bitmap;                         

        kaps_jr1_device_issu_read_fn nv_read_fn;                 
        kaps_jr1_device_issu_write_fn nv_write_fn;               
        void *nv_handle;                                    
        void *nv_ptr;                                       
        uint32_t is_fresh_nv;                               
        uint32_t nv_size;                                   
        enum kaps_jr1_restore_status *cr_status;                 
        struct kaps_jr1_nv_memory_manager *nv_mem_mgr;           

        char verbose_error_string[1024];                    
        uint32_t dump_on_assert;                            
        char fname_dump[256];                               

        struct kaps_jr1_seq_num_to_ptr *seq_num_to_ptr;          
        int32_t map_print_offset;                           
        enum kaps_jr1_tcam_format tcam_format;
        uint8_t combine_tcam_xy_write;
        uint8_t is_bb_compaction_enabled;

        uint32_t num_rows_in_each_hb_block[KAPS_JR1_MAX_NUM_HB_BLOCKS];

        kaps_jr1_prefix_log_record *pfx_log_records;
    
        uint32_t cur_num_pfx_log_records;
    
        uint32_t max_num_pfx_log_records;

        uint8_t is_blackhole_mode;
    };


    kaps_jr1_status kaps_jr1_device_parse_state(
    FILE * bin_fp,
    FILE * txt_fp);



    kaps_jr1_status kaps_jr1_device_advanced_init(
    struct kaps_jr1_device *device);



    kaps_jr1_status kaps_jr1_device_advanced_set_property(
    struct kaps_jr1_device *device,
    uint32_t property,
    va_list vl);



    kaps_jr1_status kaps_jr1_device_advanced_get_property(
    struct kaps_jr1_device *device,
    uint32_t property,
    va_list vl);



    kaps_jr1_status kaps_jr1_device_advanced_destroy(
    struct kaps_jr1_device *device);




    kaps_jr1_status kaps_jr1_device_advanced_lock(
    struct kaps_jr1_device *device);



    kaps_jr1_status kaps_jr1_device_check_lpm_constraints(
    struct kaps_jr1_device *device);


    kaps_jr1_status kaps_jr1_device_save_two_level_bb_info(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun);


    kaps_jr1_status kaps_jr1_device_restore_two_level_bb_info(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun);






    kaps_jr1_status kaps_jr1_initialize_advanced_shadow(
    struct kaps_jr1_device *device);



    kaps_jr1_status kaps_jr1_device_log_error(
    struct kaps_jr1_device *device,
    kaps_jr1_status return_status,
    char *fmt,
    ...);




    struct kaps_jr1_device *kaps_jr1_get_main_bc_device(
    struct kaps_jr1_device *device);


    int32_t kaps_jr1_check_if_inst_needed(
    struct kaps_jr1_device *device);




uint32_t
kaps_jr1_device_get_final_level_offset(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db);




uint32_t
kaps_jr1_device_get_num_final_level_bbs(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db);



struct kaps_jr1_aging_entry*
kaps_jr1_device_get_active_aging_table(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db);




uint32_t 
kaps_jr1_device_get_active_num_hb_blocks(
    struct kaps_jr1_device *device, 
    struct kaps_jr1_db *db);



uint32_t 
kaps_jr1_device_get_active_num_hb_rows(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db);





    kaps_jr1_status kaps_jr1_dm_init(
    struct kaps_jr1_device *device);



    kaps_jr1_status kaps_jr1_dm_reg_write(
    struct kaps_jr1_device *device,
    uint32_t blk_num,
    uint32_t reg_num,
    uint32_t nbytes,
    uint8_t * data);

    kaps_jr1_status kaps_jr1_dm_reg_read(
    struct kaps_jr1_device *device,
    uint32_t blk_num,
    uint32_t reg_num,
    uint32_t nbytes,
    uint8_t * o_data);

 
    kaps_jr1_status kaps_jr1_dm_ad_write(
    struct kaps_jr1_device *device,
    uint32_t ab_num,
    uint32_t uda_addr,
    uint32_t nbytes,
    uint8_t * data);


    kaps_jr1_status kaps_jr1_dm_ad_read(
    struct kaps_jr1_device *device,
    uint32_t ab_num,
    uint32_t uda_addr,
    uint32_t nbytes,
    uint8_t * o_data);


    kaps_jr1_status kaps_jr1_dm_search(
    struct kaps_jr1_device *device,
    uint8_t * key,
    enum kaps_jr1_search_interface search_interface,
    struct kaps_jr1_search_result *kaps_jr1_result);


    kaps_jr1_status kaps_jr1_dm_hb_read(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    uint32_t block_num,
    uint32_t row_num,
    uint8_t * data);


    kaps_jr1_status kaps_jr1_dm_hb_write(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    uint32_t block_num,
    uint32_t row_num,
    uint8_t * data);


    kaps_jr1_status kaps_jr1_dm_hb_dump(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    uint32_t start_blk_num,
    uint32_t start_row_num,
    uint32_t end_blk_num,
    uint32_t end_row_num,
    uint8_t clear_on_read,
    uint8_t * data);


    kaps_jr1_status kaps_jr1_dm_hb_copy(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    uint32_t src_blk_num,
    uint32_t src_row_num,
    uint32_t dest_blk_num,
    uint32_t dest_row_num,
    uint16_t source_mask,
    uint8_t rotate_right,
    uint8_t perform_clear);

#ifdef __cplusplus
}
#endif

#endif                          
