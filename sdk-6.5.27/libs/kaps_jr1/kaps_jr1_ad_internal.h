

#ifndef __KAPS_JR1_AD_INTERNAL_H
#define __KAPS_JR1_AD_INTERNAL_H

#include "kaps_jr1_errors.h"
#include "kaps_jr1_bitmap.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_list.h"
#include "kaps_jr1_ad.h"
#include "kaps_jr1_pool.h"
#include "kaps_jr1_db_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define KAPS_JR1_AD_MAX_INDIRECTION 0x3FFF



    struct kaps_jr1_entry;


    struct kaps_jr1_ad_chunk
    {
        uint32_t ad_blk_no;               
        uint32_t offset_in_ad_blk;        
        struct kaps_jr1_device *device;        
        struct kaps_jr1_ad_chunk *next_chunk;  
        struct kaps_jr1_ad_db *ad_db;          
    };

#define KAPS_JR1_GET_AD_SEQ_NUM(e) ((e)->ad_db_seq_num_0 | ((e)->ad_db_seq_num_1 << 5))



#define KAPS_JR1_AD_STRUCT_MEMBERS   \
    struct kaps_jr1_list_node ad_node;   \
    struct kaps_jr1_entry **entries; \
    uint32_t user_handle:27;       \
    uint32_t ad_db_seq_num_0:5;       \
    uint32_t ad_db_seq_num_1:3;    \
    uint32_t arr_size:16;       \
    uint32_t ref_count:14;      \
    uint32_t is_ms_side:1;      \
    uint32_t is_dangling_ad:1;

    struct kaps_jr1_ad
    {
        KAPS_JR1_AD_STRUCT_MEMBERS uint8_t value[];
                                
    };


    struct kaps_jr1_ad_32b
    {
        KAPS_JR1_AD_STRUCT_MEMBERS uint8_t value[4];
                                
    };


    struct kaps_jr1_ad_64b
    {
        KAPS_JR1_AD_STRUCT_MEMBERS uint8_t value[8];
                                
    };


    struct kaps_jr1_ad_128b
    {
        KAPS_JR1_AD_STRUCT_MEMBERS uint8_t value[16];
                                
    };

#define DBLIST_TO_AD_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_jr1_ad, ad_node)


    struct kaps_jr1_ad_256b
    {
        KAPS_JR1_AD_STRUCT_MEMBERS uint8_t value[32];
                                
    };


    struct kaps_jr1_ad_db
    {
        struct kaps_jr1_db db_info;      
                                                          
        uint32_t num_0b_entries;    
        uint16_t user_width_1;      
        uint8_t pool_init_done;     
        uint8_t user_bytes;         
        uint8_t device_bytes;       
        uint8_t byte_offset;        
        uint8_t ix_max_num_holes_allowed; 
        uint32_t num_writes;        
        uint32_t num_entries_in_ad_db[KAPS_JR1_MAX_NUM_CORES];    
        uint32_t acquire_extra_ix_for_lmpsofar; 

        struct kaps_jr1_allocator *alloc;
        struct kaps_jr1_c_list ad_list;      
        struct kaps_jr1_ad_db *next;     
        struct kaps_jr1_ad_db *hb_ad;    
        struct kaps_jr1_ad_db *parent_ad;
        struct kaps_jr1_ix_mgr *mgr[KAPS_JR1_MAX_NUM_CORES][KAPS_JR1_MAX_NUM_DAISY_CHAINS]; 
        struct kaps_jr1_ix_mgr *last_mgr;

        union
        {
            POOL_DEF(
    kaps_jr1_ad_32b) ad_entry_pool_32;                
            POOL_DEF(
    kaps_jr1_ad_64b) ad_entry_pool_64;                
            POOL_DEF(
    kaps_jr1_ad_128b) ad_entry_pool_128;              
            POOL_DEF(
    kaps_jr1_ad_256b) ad_entry_pool_256;              
        } pool;
            POOL_DEF(
    kaps_jr1_ad_chunk) ad_chunk_pool;          

    };



    kaps_jr1_status kaps_jr1_ad_db_add_entry_internal(
    struct kaps_jr1_ad_db *ad_db,
    uint8_t * value,
    struct kaps_jr1_ad **ad,
    intptr_t user_handle);



    kaps_jr1_status kaps_jr1_ad_db_associate_entry_and_ad(
    struct kaps_jr1_device *device,
    struct kaps_jr1_ad *ad,
    struct kaps_jr1_entry *entry);



    kaps_jr1_status kaps_jr1_ad_db_deassociate_entry_and_ad(
    struct kaps_jr1_device *device,
    struct kaps_jr1_ad *ad,
    struct kaps_jr1_entry *entry);



    kaps_jr1_status kaps_jr1_ad_db_init_internal(
    struct kaps_jr1_device *device,
    uint32_t id,
    uint32_t capacity,
    uint32_t width_1,
    struct kaps_jr1_ad_db **db,
    uint32_t is_internal);



    kaps_jr1_status kaps_jr1_ad_db_update_width(
    struct kaps_jr1_db *db,
    uint32_t new_width);



    uint32_t kaps_jr1_ad_db_get_seq_num(
    struct kaps_jr1_db *db,
    struct kaps_jr1_ad_db *ad_db);



    struct kaps_jr1_ad_db *kaps_jr1_ad_get_ad_db_ptr(
    struct kaps_jr1_db *db,
    uint32_t expected_seq_num);

#ifdef __cplusplus
}
#endif

#endif
