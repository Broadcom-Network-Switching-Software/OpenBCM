

#ifndef __KAPS_JR1_CR_POOL_MGR_H
#define __KAPS_JR1_CR_POOL_MGR_H

#include "kaps_jr1_errors.h"
#include "kaps_jr1_device.h"

#ifdef __cplusplus
extern "C"
{
#endif



#define KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB  (128)

#define KAPS_JR1_CR_INVALID_POOL_ID (255)

#define KAPS_JR1_CR_LPM_MAX_NUM_ENTRIES_PER_POOL (128 * 1024)

#define KAPS_JR1_CR_GET_POOL_MGR(dev, cr_mgr)                                     \
    do {                                                                 \
        uint8_t *nv_device_ptr = dev->nv_ptr;                            \
        nv_device_ptr += dev->nv_size;                                   \
        nv_device_ptr -= sizeof(struct kaps_jr1_cr_pool_mgr);                     \
        cr_mgr = (struct kaps_jr1_cr_pool_mgr *)nv_device_ptr;                    \
    } while (0);



    struct kaps_jr1_cr_pool_node
    {
        uint32_t pool_id:8; 
        uint32_t in_use:1;  
        uint32_t num_valid_entries:23;
                                   
        uint32_t start_user_handle;
                                
        uint32_t end_user_handle;
                                
        uint32_t start_nv_offset;
                                
        uint32_t end_nv_offset; 
    };



    struct kaps_jr1_cr_pools_for_db
    {
        struct kaps_jr1_db *db;
                         
        struct kaps_jr1_cr_pool_node node_info[KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB];
                                                                      
        uint32_t entry_size_8;
                            
        uint32_t num_entries_per_pool;
                                   
    };



    struct kaps_jr1_cr_pool_mgr
    {
        uint16_t num_dbs;
                        
        uint32_t free_end_nv_offset;
                                 
        struct kaps_jr1_cr_pools_for_db *pools_for_db;
                                               
    };



    struct kaps_jr1_cr_pool_entry_iter
    {
        uint32_t entry_nr;
                        
    };


    kaps_jr1_status kaps_jr1_cr_pool_mgr_init(
    struct kaps_jr1_device *device,
    uint8_t * end_nv_ptr,
    struct kaps_jr1_cr_pool_mgr **mgr_pp);


    kaps_jr1_status kaps_jr1_cr_pool_mgr_associate_user_handle(
    struct kaps_jr1_cr_pool_mgr *mgr,
    struct kaps_jr1_db *db,
    uint32_t user_handle,
    uint32_t * pool_id);


    kaps_jr1_status kaps_jr1_cr_pool_mgr_disassociate_user_handle(
    struct kaps_jr1_cr_pool_mgr *mgr,
    struct kaps_jr1_db *db,
    uint32_t user_handle);


    kaps_jr1_status kaps_jr1_cr_pool_mgr_get_num_slots(
    struct kaps_jr1_cr_pool_mgr *mgr,
    struct kaps_jr1_db *db,
    uint32_t * num_active_slots,
    uint32_t * num_total_slots);


    kaps_jr1_status kaps_jr1_cr_pool_mgr_get_entry_nv_ptr(
    struct kaps_jr1_cr_pool_mgr *mgr,
    struct kaps_jr1_db *db,
    uint32_t user_handle,
    uint8_t * found,
    uint8_t ** entry_nv_ptr);


    kaps_jr1_status kaps_jr1_cr_pool_mgr_get_user_handle(
    struct kaps_jr1_cr_pool_mgr *mgr,
    struct kaps_jr1_db *db,
    uint8_t * entry_nv_ptr,
    uint8_t * found_p,
    uint32_t * user_handle);


    kaps_jr1_status kaps_jr1_cr_pool_mgr_get_user_handle_from_nv_location(
    struct kaps_jr1_cr_pool_mgr *mgr,
    struct kaps_jr1_db *db,
    uint32_t nv_location,
    uint8_t * found_p,
    uint32_t * user_handle);


    kaps_jr1_status kaps_jr1_cr_pool_mgr_iter_init(
    struct kaps_jr1_cr_pool_mgr *mgr,
    struct kaps_jr1_db *db,
    struct kaps_jr1_cr_pool_entry_iter **iter);


    kaps_jr1_status kaps_jr1_cr_pool_mgr_iter_next(
    struct kaps_jr1_cr_pool_mgr *mgr,
    struct kaps_jr1_db *db,
    struct kaps_jr1_cr_pool_entry_iter *iter,
    uint8_t ** entry_nv_ptr);


    kaps_jr1_status kaps_jr1_cr_pool_mgr_iter_destroy(
    struct kaps_jr1_cr_pool_mgr *mgr,
    struct kaps_jr1_db *db,
    struct kaps_jr1_cr_pool_entry_iter *iter);


    kaps_jr1_status kaps_jr1_cr_pool_mgr_refresh_db_handles(
    struct kaps_jr1_device *device);


    uint32_t kaps_jr1_cr_pool_mgr_is_nv_exhausted(
    struct kaps_jr1_device *device,
    uint32_t num_bytes_needed);


    kaps_jr1_status kaps_jr1_cr_pool_mgr_get_stats(
    struct kaps_jr1_device *device);



#ifdef __cplusplus
}
#endif
#endif
