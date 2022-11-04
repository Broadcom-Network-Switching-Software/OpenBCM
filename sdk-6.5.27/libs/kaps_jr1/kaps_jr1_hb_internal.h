

#ifndef __KAPS_JR1_HB_INTERNAL_H
#define __KAPS_JR1_HB_INTERNAL_H

#include "kaps_jr1_errors.h"
#include "kaps_jr1_bitmap.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_list.h"
#include "kaps_jr1_hb.h"
#include "kaps_jr1_pool.h"
#include "kaps_jr1_db_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define KAPS_JR1_HB_SPECIAL_VALUE (0xFFFFFFFF)



    struct kaps_jr1_entry;

#define KAPS_JR1_DBLIST_TO_HB_ENTRY(ptr)  CONTAINER_OF((ptr), struct kaps_jr1_hb, hb_node)


    struct kaps_jr1_aging_entry
    {
        struct kaps_jr1_entry *entry;
                                
        uint32_t num_idles;    
        uint8_t intermediate_hb_val;
                                 
    };




    struct kaps_jr1_hb
    {
        struct kaps_jr1_list_node hb_node;
                                     
        struct kaps_jr1_entry *entry;
        uint32_t user_handle:24;
        uint32_t hb_db_seq_num:8;
                                
        uint32_t bit_no:30;     
        uint32_t bit_allocated:1;
                                
        uint32_t value:1;       
    };


    struct kaps_jr1_hb_db
    {
        struct kaps_jr1_db db_info;      
        struct kaps_jr1_allocator *alloc;
        struct kaps_jr1_c_list hb_list;      
        uint32_t age_count;         
        uint32_t bit_iter;          
        uint32_t capacity;          

            POOL_DEF(
    kaps_jr1_hb) kaps_jr1_hb_pool;            
    };


    kaps_jr1_status kaps_jr1_hb_db_add_entry_internal(
    struct kaps_jr1_hb_db *hb_db,
    struct kaps_jr1_hb **hb,
    intptr_t user_handle);

    typedef kaps_jr1_status(
    *hb_ix_changed_callback) (
    void *client,
    struct kaps_jr1_entry * entry,
    uint32_t old_index,
    uint32_t new_index);

    kaps_jr1_status kaps_jr1_hb_cb(
    void *client,
    struct kaps_jr1_entry *entry,
    uint32_t old_index,
    uint32_t new_index);



    

    kaps_jr1_status kaps_jr1_hb_wb_save_state(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun);

    kaps_jr1_status kaps_jr1_hb_wb_restore_state(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun);

    kaps_jr1_status kaps_jr1_hb_db_wb_add_entry(
    struct kaps_jr1_hb_db *hb_db,
    uint32_t bit_no,
    uintptr_t user_handle_p);

    kaps_jr1_status kaps_jr1_hb_cr_init(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *cb_fun);

#ifdef __cplusplus
}
#endif

#endif
