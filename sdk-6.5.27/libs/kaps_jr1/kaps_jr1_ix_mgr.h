

#ifndef __KAPS_JR1_IX_MGR_H
#define __KAPS_JR1_IX_MGR_H

#include <stdint.h>
#include "kaps_jr1_errors.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_pool.h"
#include "kaps_jr1_portable.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_resource.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct kaps_jr1_ad_db;


#define KAPS_JR1_MAX_IX_INVOLVED_IN_COMPACTION (20 * 1024)


#define KAPS_JR1_MAX_RESV_IX_FOR_SPECIAL_ENTRIES (2048)


#define KAPS_JR1_MAX_IX_MGR_ALLOCATED_CHUNK_SIZE (60)







    typedef enum kaps_jr1_ix_chunk_type
    {
        IX_FREE_CHUNK = 0,  
        IX_ALLOCATED_CHUNK
                        
    } kaps_jr1_ix_chunk_type;


    typedef enum kaps_jr1_ix_user_type
    {
        IX_USER_LSN = 0,
        IX_USER_OVERFLOW, 
        IX_LMPSOFAR       
    } kaps_jr1_ix_user_type;



    struct kaps_jr1_ix_chunk
    {
        uint32_t start_ix:27;
                          
        uint32_t size:27;  
        uint32_t type:2;
                     
        uint32_t user_type:2;
                          
        void *lsn_ptr;      
        struct kaps_jr1_ad_chunk *ad_info; 
        struct kaps_jr1_ix_chunk *prev_free_chunk;
                                           
        struct kaps_jr1_ix_chunk *next_free_chunk;
                                           
        struct kaps_jr1_ix_chunk *prev_neighbor;
                                           
        struct kaps_jr1_ix_chunk *next_neighbor;
                                           
    };


    struct kaps_jr1_ix_compaction_chunk
    {
        struct kaps_jr1_ix_chunk *kaps_jr1_ix_chunk;    
        int32_t from_start_ix;                  
        int32_t from_end_ix;                    
        int32_t from_size;                      
        int32_t to_start_ix;                    
        int32_t to_end_ix;                      
        int32_t to_size;                        
        int32_t to_ad_blk_no;                   
        int32_t to_offset_in_ad_blk;            
        struct kaps_jr1_ix_compaction_chunk *next;  
    };


    struct kaps_jr1_ad_blk_info
    {
        uint32_t size;                   
        uint32_t allocated;              
        uint32_t ad_blk_num;             
        uint32_t is_compacted;           
    };



    struct kaps_jr1_ix_mgr
    {
        struct kaps_jr1_ix_chunk **free_list;
                                      
        uint32_t *num_ix_in_free_list;
                                   

        uint32_t daisy_chain_id;  

        uint32_t max_ix_per_daisy_chain; 


        uint32_t ix_max_holes_allowed; 
        uint32_t cb_blk;
        uint32_t ad_width_1;  

        enum kaps_jr1_ad_type ad_type; 

        int32_t start_ix;
                      
        int32_t end_ix;
                    
        int32_t size;
                    

        int32_t cb_start_ix;
                           
        int32_t cb_end_ix; 
        int32_t cb_size;   

        uint32_t max_alloc_chunk_size;
                                   
        uint32_t max_num_ix_chunk_levels;  

        int32_t ref_count;
                       
        int32_t is_finalized;
                          
        struct kaps_jr1_ix_chunk *neighbor_list;
                                         
        struct kaps_jr1_ix_chunk *neighbor_list_end;
                                             
        uint32_t num_allocated_ix;          
        uint32_t *ix_translate; 
        uint32_t *ix_segments_start; 
        uint32_t *ix_segments_end;   

        struct kaps_jr1_ad_blk_info *ad_blk_info; 
        struct kaps_jr1_ix_mgr *next;       
        struct kaps_jr1_ad_db *main_ad_db;  

        POOL_DEF(kaps_jr1_ix_chunk) ix_chunk_pool;          

        struct kaps_jr1_device *device;  

        uint32_t max_num_ad_blks; 
        uint32_t cur_num_ad_blks; 
        uint32_t num_entries_per_ad_row; 
        uint32_t num_entries_per_ad_blk; 
        uint32_t ix_boundary_per_ad_blk; 
    };



    struct kaps_jr1_ix_mgr_stats
    {
        uint32_t total_num_allocated_ix;
                                     
        uint32_t total_num_free_ix;  
        uint32_t num_entries_per_sb[256];
        uint32_t num_entries_present[256];

    };


    kaps_jr1_status kaps_jr1_ix_mgr_init(
    struct kaps_jr1_device *device,
    uint32_t max_alloc_chunk_size,
    struct kaps_jr1_ad_db *ad_db,
    uint32_t daisy_chain_id,
    uint8_t is_warmboot);



kaps_jr1_status
kaps_jr1_ix_mgr_init_for_all_ad_dbs(
    struct kaps_jr1_device * device,
    uint32_t max_alloc_chunk_size,
    struct kaps_jr1_db * db,
    uint8_t is_warmboot);




struct kaps_jr1_ix_mgr*
kaps_jr1_ix_mgr_get_mgr_for_lmpsofar_pfx(
    struct kaps_jr1_ad_db *ad_db);





    kaps_jr1_status kaps_jr1_ix_mgr_alloc(
    struct kaps_jr1_ix_mgr *mgr_p,
    struct kaps_jr1_ad_db *ad_db,
    uint32_t rqt_size,
    void *lsn_ptr,
    enum kaps_jr1_ix_user_type user_type,
    struct kaps_jr1_ix_chunk **chunk);



    kaps_jr1_status kaps_jr1_ix_mgr_wb_alloc(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ad_db *ad_db,
    uint32_t rqt_size,
    uint32_t start_ix,
    void *lsn_ptr,
    enum kaps_jr1_ix_user_type user_type,
    struct kaps_jr1_ix_chunk **alloc_chunk_pp);



    kaps_jr1_status kaps_jr1_ix_mgr_wb_finalize(
    struct kaps_jr1_ix_mgr *mgr);




kaps_jr1_status
kaps_jr1_ix_mgr_wb_finalize_for_all_ad_dbs(
    struct kaps_jr1_device *device,
    struct kaps_jr1_ad_db *ad_db);



    kaps_jr1_status kaps_jr1_ix_mgr_check_grow_up(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *cur_ix_chunk,
    uint32_t * grow_up_size_p);


    kaps_jr1_status kaps_jr1_ix_mgr_grow_up(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *cur_ix_chunk,
    uint32_t grow_up_size);


    kaps_jr1_status kaps_jr1_ix_mgr_check_grow_down(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *cur_ix_chunk,
    uint32_t * grow_down_size_p);


    kaps_jr1_status kaps_jr1_ix_mgr_grow_down(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *cur_ix_chunk,
    uint32_t grow_down_size);



    kaps_jr1_status kaps_jr1_ix_mgr_free(
    struct kaps_jr1_ix_mgr *mgr_p,
    struct kaps_jr1_ix_chunk *chunk);




    kaps_jr1_status kaps_jr1_ix_mgr_destroy(
    struct kaps_jr1_ix_mgr *mgr_p);






kaps_jr1_status
kaps_jr1_ix_mgr_destroy_for_all_ad_dbs(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db);




    kaps_jr1_status kaps_jr1_ix_mgr_calc_stats(
    const struct kaps_jr1_ix_mgr *mgr_p,
    struct kaps_jr1_ix_mgr_stats *ix_stats);



    kaps_jr1_status kaps_jr1_ix_mgr_print_stats(
    struct kaps_jr1_ix_mgr_stats *ix_stats);



    kaps_jr1_status kaps_jr1_ix_mgr_verify(
    struct kaps_jr1_ix_mgr *mgr,
    uint32_t recalc_neighbor_list_end);




kaps_jr1_status
kaps_jr1_ix_mgr_verify_for_all_ad_dbs(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db);




kaps_jr1_status
kaps_jr1_ix_mgr_wb_save_ix_translate(
    struct kaps_jr1_ix_mgr *mgr,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle,
    uint32_t * nv_offset);



kaps_jr1_status
kaps_jr1_ix_mgr_wb_restore_ix_translate(
    struct kaps_jr1_ix_mgr *mgr,
    kaps_jr1_device_issu_write_fn read_fn,
    void *handle,
    uint32_t * nv_offset);




kaps_jr1_status
kaps_jr1_ix_mgr_compact(
        struct kaps_jr1_ix_mgr *mgr,
        int32_t enable_move,
        int *was_compaction_done);


#ifdef __cplusplus
}
#endif
#endif 
