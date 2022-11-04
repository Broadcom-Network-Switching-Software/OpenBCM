

#ifndef __KAPS_JR1_DB_INTERNAL_H
#define __KAPS_JR1_DB_INTERNAL_H

#include "kaps_jr1_portable.h"
#include "kaps_jr1_list.h"
#include "kaps_jr1_db.h"
#include "kaps_jr1_resource.h"
#include "kaps_jr1_bitmap.h"
#include "kaps_jr1_db_wb.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct kaps_jr1_ad;

#define KAPS_JR1_GET_DB_PARENT(db) ((db)->parent ? (db)->parent : (db))



#define KAPS_JR1_SSDBLIST_TO_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_jr1_db, node)





    struct kaps_jr1_db_internal_stats
    {
        uint32_t num_of_piowrs;
                              
        uint32_t num_of_shuffles;
                              
        uint32_t num_blk_ops;
                            
    };




    struct kaps_jr1_db_pio_stats
    {
        uint32_t num_of_piowrs;
                              
        uint32_t num_of_piords;
                              
        uint32_t num_of_shuffles;
                              
        uint32_t num_of_worst_case_shuffles;
                                         
        uint32_t num_ix_cbs;
        uint32_t num_blk_ops;
                            
        uint32_t num_ad_writes;
                            
    };


    struct kaps_jr1_db_fn_impl
    {
        kaps_jr1_status(
    *db_add_prefix) (
    struct kaps_jr1_db * db,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_jr1_entry ** entry);
        kaps_jr1_status(
    *entry_add_ad) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    struct kaps_jr1_ad * ad);
        kaps_jr1_status(
    *entry_add_hb) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    struct kaps_jr1_hb * hb);
        kaps_jr1_status(
    *get_hb) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    struct kaps_jr1_hb ** hb);
        kaps_jr1_status(
    *db_delete_entry) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry);
        kaps_jr1_status(
    *db_install) (
    struct kaps_jr1_db * db);
        kaps_jr1_status(
    *db_search) (
    struct kaps_jr1_db * db,
    uint8_t * key,
    struct kaps_jr1_entry ** entry,
    int32_t * index,
    int32_t * prio_len);
        kaps_jr1_status(
    *get_prefix_handle) (
    struct kaps_jr1_db * db,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_jr1_entry ** entry);
        kaps_jr1_status(
    *db_stats) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_db_stats * stats);
        kaps_jr1_status(
    *db_delete_all_entries) (
    struct kaps_jr1_db * db);
        kaps_jr1_status(
    *db_delete_all_pending_entries) (
    struct kaps_jr1_db * db);
        
        kaps_jr1_status(
    *db_process_hit_bits) (
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db);
        kaps_jr1_status(
    *db_get_algo_hit_bit_value) (
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    uint8_t clear_on_read,
    uint32_t * bit_value);
        kaps_jr1_status(
    *get_data_len) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * e,
    uint32_t * len_1,
    uint8_t ** data);

        kaps_jr1_status(
    *db_set_key) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_key * key);
        kaps_jr1_status(
    *db_set_property) (
    struct kaps_jr1_db * db,
    uint32_t property,
    va_list vl);
        kaps_jr1_status(
    *db_get_property) (
    struct kaps_jr1_db * db,
    uint32_t property,
    va_list vl);
        kaps_jr1_status(
    *entry_set_property) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    uint32_t property,
    va_list vl);
        kaps_jr1_status(
    *db_add_em) (
    struct kaps_jr1_db * db,
    uint8_t * data,
    struct kaps_jr1_entry ** entry);
        kaps_jr1_status(
    *entry_print) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    FILE * fp);
        kaps_jr1_status(
    *db_add_table) (
    struct kaps_jr1_db * db,
    uint32_t id,
    uint32_t is_clone,
    struct kaps_jr1_db ** table);
        kaps_jr1_status(
    *entry_get_priority) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    uint32_t * prio_length);
        kaps_jr1_status(
    *entry_get_index) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    int32_t * nindices,
    int32_t ** indices);
        kaps_jr1_status(
    *entry_get_info) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    struct kaps_jr1_entry_info * info);
        kaps_jr1_status(
    *update_ad) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_ad * ad_handle,
    struct kaps_jr1_entry * e,
    uint32_t num_bytes);
        
        kaps_jr1_status(
    *get_ad) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * e,
    struct kaps_jr1_ad * ad_handle,
    struct kaps_jr1_device ** device);
        
        kaps_jr1_status(
    *save_property) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_wb_cb_functions * cb_fun);
        kaps_jr1_status(
    *restore_property) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_wb_cb_functions * cb_fun);
        kaps_jr1_status(
    *save_state) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_wb_cb_functions * cb_fun);
        kaps_jr1_status(
    *restore_state) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_wb_cb_functions * cb_fun);
        kaps_jr1_status(
    *save_cr_state) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_wb_cb_functions * cb_fun);
        kaps_jr1_status(
    *restore_cr_state) (
    struct kaps_jr1_db * db,
    struct kaps_jr1_wb_cb_functions * cb_fun);
        uint32_t(
    *cr_calc_entry_mem) (
    struct kaps_jr1_db * db);
        kaps_jr1_status(
    *cr_store_entry) (
    struct kaps_jr1_db * db_p,
    struct kaps_jr1_entry * entry_p);
        kaps_jr1_status(
    *cr_update_entry) (
    struct kaps_jr1_db * db_p,
    struct kaps_jr1_entry * entry_p);
        kaps_jr1_status(
    *cr_store_ad_entry) (
    struct kaps_jr1_db * ad_db,
    struct kaps_jr1_ad * ad_handle);
        kaps_jr1_status(
    *cr_store_trigger) (
    struct kaps_jr1_db * db_p,
    void *trigger);
        kaps_jr1_status(
    *cr_delete_entry) (
    struct kaps_jr1_db * db_p,
    void *entry_p,
    uint32_t is_start);
        kaps_jr1_status(
    *cr_entry_shuffle) (
    struct kaps_jr1_db * db_p,
    void *entry_p,
    uint32_t is_start);
        kaps_jr1_status(
    *cr_entry_flush) (
    struct kaps_jr1_db * db_p,
    void *entry_p,
    uint32_t is_start);
        kaps_jr1_status(
    *cr_entry_update_ad) (
    struct kaps_jr1_device * device,
    struct kaps_jr1_entry * entry_p,
    struct kaps_jr1_ad * ad_handle,
    uint8_t * value);
        kaps_jr1_status(
    *cr_reconcile) (
    struct kaps_jr1_db * db_p);
        kaps_jr1_status(
    *reconcile_end) (
    struct kaps_jr1_db * db);
        kaps_jr1_status(
    *set_used) (
    struct kaps_jr1_entry * entry);
        kaps_jr1_status(
    *reset_used) (
    struct kaps_jr1_entry * entry);
        kaps_jr1_status(
    *lock_db) (
    struct kaps_jr1_db * db);
        kaps_jr1_status(
    *db_destroy) (
    struct kaps_jr1_db * db);
    };

    struct kaps_jr1_db_common_info
    {
        uint32_t mapped_to_acl:1;     
        uint32_t is_em:1;             
        uint32_t user_specified:1;    
        uint32_t finalized_resources:1;
                                      
        uint32_t calc_shuffle_stats:1;
        uint32_t hw_res_alloc:1;      
        uint32_t enable_dynamic_allocation:1;
                                          
        uint32_t is_algo:1;           
        uint32_t is_cascaded:1;      

        uint8_t defer_deletes_to_install;
                                      

        enum kaps_jr1_db_insertion_mode insertion_mode;
                                                
        uint32_t capacity;            
        int32_t cur_capacity;         
        uint32_t index_range_min;     
        uint32_t index_range_max;     

        int16_t num_ab;               
        uint16_t start_hb_blk;        
        uint16_t num_hb_blks;         
        int32_t uda_mb;               
        int32_t total_bb_size_in_bits;

        uint8_t meta_priority;        

        int16_t user_num_ab;          
        int32_t user_uda_mb;          

        kaps_jr1_db_index_callback callback_fn;
                                       
        void *callback_handle;        

        uint16_t fit_error;           

        uint32_t *user_hdl_table_size; 
        void **user_hdl_to_entry;     

        struct kaps_jr1_db_pio_stats pio_stats;
                                        

        union
        {
            struct kaps_jr1_db *ad;        
            struct kaps_jr1_db *db;        
        } ad_info;

        union
        {
            struct kaps_jr1_db *hb;        
            struct kaps_jr1_db *db;        
        } hb_info;

        uint16_t entry_size;          
        uint16_t nv_db_iter;          
        struct kaps_jr1_entry **del_entry_list;
                                        
        uint32_t num_pending_del_entries;
                                      
        uint32_t max_pending_del_entries;
                                      

        FILE *fp;                      
        FILE *fp_wb;                   

        int32_t total_bb_rows;              
    };




    struct kaps_jr1_db
    {
        struct kaps_jr1_list_node node;        
        enum kaps_jr1_db_type type;        
        struct kaps_jr1_db *parent;        
        struct kaps_jr1_db *next_tab;      
        struct kaps_jr1_key *key;          
        struct kaps_jr1_device *device;    
        struct kaps_jr1_db_fn_impl *fn_table;
                                       
        struct kaps_jr1_instruction **instructions;
                                            
        int16_t ninstructions;        
        uint16_t seq_num;             

        union
        {
            int16_t key_width_1;      
            int16_t ad_width_1;       
        } width;

        union
        {
            struct kaps_jr1_db_hw_resource *db_res;
            struct kaps_jr1_ad_db_hw_resource *ad_res;
                                               
        } hw_res;

        struct kaps_jr1_c_list db_list;        
        struct kaps_jr1_c_list db_pending_list;


        struct kaps_jr1_db_common_info *common_info;
                                               

        struct kaps_jr1_lpm_mgr *lpm_mgr;   

        struct kaps_jr1_db *clone_of;      
        void *handle;                 

        uint32_t tid;                 
        uint32_t is_clone:1;          
        uint32_t has_clones:1;        
        uint32_t has_tables:1;        
        uint32_t has_dup_db:1;        
        uint32_t has_dup_ad:1;        
        uint32_t is_destroyed:1;      
        uint32_t is_destroy_in_progress:1;
                                      
        uint32_t is_internal:1;       
        uint32_t is_main_bc_db:1;     
        uint32_t is_bc_required:1;    
        uint32_t is_public:1;         

        uint32_t rpb_id;              
        uint8_t ltr_db_id;            

        uint32_t num_algo_levels_in_db;  

        uint32_t rqt_cnt;           


        char *description;            
        uintptr_t stale_ptr;          
        uintptr_t hb_ad_stale_ptr;    

        struct kaps_jr1_db *main_bc_db;    
        uint32_t bc_bitmap;           

    };



#define KAPS_JR1_DBLIST_TO_KAPS_JR1_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_jr1_entry, db_node)



#define KAPS_JR1_ENTRY_STRUCT_MEMBERS      \
    struct kaps_jr1_list_node db_node;         \
    struct kaps_jr1_ad *ad_handle;         \
    uint32_t db_seq_num:8;            \
    uint32_t hb_user_handle:24;       \
    uint32_t core:1;                  \
    uint32_t user_handle:27;

    struct kaps_jr1_entry
    {
    KAPS_JR1_ENTRY_STRUCT_MEMBERS};



    kaps_jr1_status kaps_jr1_lpm_db_init(
    struct kaps_jr1_device *device,
    uint32_t id,
    uint32_t capacity,
    struct kaps_jr1_db **dbp);




    struct kaps_jr1_db *kaps_jr1_lpm_get_res_db(
    struct kaps_jr1_db *db);



    kaps_jr1_status kaps_jr1_lpm_destroy(
    struct kaps_jr1_device *device);


    struct kaps_jr1_db *kaps_jr1_db_get_main_db(
    struct kaps_jr1_db *db);



    struct kaps_jr1_db *kaps_jr1_db_get_bc_db_on_device(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *inp_db);

    kaps_jr1_status kaps_jr1_db_init_internal(
    struct kaps_jr1_device *device,
    enum kaps_jr1_db_type type,
    uint32_t id,
    uint32_t capacity,
    struct kaps_jr1_db **dbp,
    int32_t is_broadcast);

#ifdef __cplusplus
}
#endif
#endif
