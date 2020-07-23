/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __DB_H
#define __DB_H

#include <stdint.h>

#include "errors.h"
#include "device.h"
#include "hw_limits.h"

#ifdef __cplusplus
extern "C" {
#endif





struct kbp_db;



struct kbp_entry;



struct kbp_ad;



struct kbp_hb;



struct kbp_key;



struct kbp_ad_db;



enum kbp_db_insertion_mode {
    KBP_DB_INSERTION_MODE_NONE,
    KBP_DB_INSERTION_MODE_NORMAL,
    KBP_DB_INSERTION_MODE_ENTRY_WITH_INDEX,
    KBP_DB_INSERTION_MODE_RELATIVE
};



enum kbp_db_mc_type {
    KBP_DB_MC_TYPE_RESET,
    KBP_DB_MC_TYPE1,
    KBP_DB_MC_TYPE2,
    KBP_DB_MC_TYPE_INVALID
};



enum kbp_entry_position
{
    KBP_DB_ENTRY_ADD_BEFORE = 0,
    KBP_DB_ENTRY_ADD_AFTER = 1
};





enum kbp_db_entry_meta_priority {
    KBP_ENTRY_META_PRIORITY_0 = 0, 
    KBP_ENTRY_META_PRIORITY_1 = 1,
    KBP_ENTRY_META_PRIORITY_2 = 2,
    KBP_ENTRY_META_PRIORITY_3 = 3  
};



struct kbp_hb_db;



enum kbp_db_properties {
    KBP_PROP_USE_MCOR,                
    KBP_PROP_MCOR_VALUE,              
    KBP_PROP_ALGORITHMIC,             
    KBP_PROP_INDEX_RANGE,             
    KBP_PROP_INDEX_CALLBACK,          
    KBP_PROP_MIN_PRIORITY,            
    KBP_PROP_DESCRIPTION,             
    KBP_PROP_CASCADE_DEVICE,          
    KBP_PROP_REDUCED_INDEX_CALLBACKS, 
    KBP_PROP_SAVE_IX_SPACE,           
    KBP_PROP_DMA_TAG,                 
    KBP_PROP_ENTRY_META_PRIORITY,     
    KBP_PROP_ENABLE_DB_COMPACTION,    
    KBP_PROP_MAX_CAPACITY,            
    KBP_PROP_AGE_COUNT,               
    KBP_PROP_PAIR_WITH_DB,            
    KBP_PROP_DEFER_DELETES,           
    KBP_PROP_SCALE_UP_CAPACITY,       
    KBP_PROP_LOG,                     
    KBP_PROP_MAX_PRIORITY,            
    KBP_PROP_SEND_PRIORITY_OVER_AD,   
    KBP_PROP_ENABLE_COUNTERS,         
    KBP_PROP_COUNTER_CYCLE,           
    KBP_PROP_COUNTER_TYPE,            
    KBP_PROP_XL_DB,                   
    KBP_PROP_LOCALITY,                
    KBP_PROP_REPLICATE_DB,            
    KBP_PROP_MC_DB,                   
    KBP_PROP_ENABLE_DYNAMIC_ALLOCATION,
    KBP_PROP_UTILISE_UDA_HOLES,        
    KBP_PROP_SET_INDIRECTION_AD,      
    KBP_PROP_INVALID                  
};



enum kbp_entry_properties {
    KBP_ENTRY_PROP_META_PRIORITY, 
    KBP_ENTRY_PROP_INVALID        
};





kbp_status kbp_db_init(struct kbp_device *device, enum kbp_db_type type,
                       uint32_t id, uint32_t capacity, struct kbp_db **dbp);


kbp_status kbp_db_create_broadcast(struct kbp_device *device,
                                   struct kbp_db *orig_db, struct kbp_db **bc_db);




kbp_status kbp_db_destroy(struct kbp_db *db);



kbp_status kbp_db_set_resource(struct kbp_db *db, enum kbp_hw_resource resource, ...);



kbp_status kbp_db_get_resource(struct kbp_db *db, enum kbp_hw_resource resource, ...);



kbp_status kbp_db_set_key(struct kbp_db *db, struct kbp_key *key);



kbp_status kbp_db_set_property(struct kbp_db *db, enum kbp_db_properties property, ...);



kbp_status kbp_db_get_property(struct kbp_db *db, enum kbp_db_properties property, ...);



kbp_status kbp_db_print(struct kbp_db *db, FILE * fp);



kbp_status kbp_db_set_ad(struct kbp_db *db, struct kbp_ad_db *ad_db);



kbp_status kbp_db_set_hb(struct kbp_db *db, struct kbp_hb_db *hb_db);




kbp_status kbp_db_add_table(struct kbp_db *db, uint32_t id, struct kbp_db **table);



kbp_status kbp_db_clone(struct kbp_db *db, uint32_t id, struct kbp_db **clone);



kbp_status kbp_db_add_ace(struct kbp_db *db, uint8_t *data, uint8_t *mask,
                          uint32_t priority, struct kbp_entry **entry);



kbp_status kbp_db_add_ace_with_index(struct kbp_db *db, uint8_t *data, uint8_t *mask,
                                     uint32_t priority, uint32_t index);



kbp_status kbp_db_update_ace(struct kbp_db *db, struct kbp_entry *entry, uint8_t *data, uint8_t *mask);


kbp_status kbp_db_relative_add_ace(struct kbp_db *db, uint8_t *data, uint8_t *mask,
                                   struct kbp_entry *relative_entry,
                                   enum kbp_entry_position entry_position,
                                   uint32_t priority,
                                   struct kbp_entry **entry);



kbp_status kbp_db_add_prefix(struct kbp_db *db, uint8_t *prefix, uint32_t length,
                             struct kbp_entry **entry);



kbp_status kbp_db_add_prefix_with_index(struct kbp_db *db, uint8_t *prefix,
                                        uint32_t length, uint32_t index);



kbp_status kbp_db_add_em(struct kbp_db *db, uint8_t *data, struct kbp_entry **entry);


kbp_status kbp_db_add_em_with_index(struct kbp_db *db, uint8_t *data, uint32_t index);



kbp_status kbp_db_delete_entry(struct kbp_db *db, struct kbp_entry *entry);



kbp_status kbp_entry_set_property(struct kbp_db *db, struct kbp_entry *entry, enum kbp_entry_properties property, ...);



kbp_status kbp_entry_add_range(struct kbp_db *db, struct kbp_entry *entry,
                               uint16_t lo, uint16_t hi, int32_t range_no);



kbp_status kbp_entry_add_ad(struct kbp_db *db, struct kbp_entry *entry, struct kbp_ad *ad);



kbp_status kbp_entry_add_hb(struct kbp_db *db, struct kbp_entry *entry, struct kbp_hb *hb);



kbp_status kbp_entry_print(struct kbp_db *db, struct kbp_entry *entry, FILE * fp);



kbp_status kbp_db_install(struct kbp_db *db);



kbp_status kbp_db_delete_all_entries(struct kbp_db *db);



kbp_status kbp_db_delete_all_pending_entries(struct kbp_db *db);



struct kbp_db_stats {
    uint32_t num_entries;       
    uint32_t capacity_estimate; 
    uint32_t range_expansion;   
};



kbp_status kbp_db_stats(struct kbp_db *db, struct kbp_db_stats *stats);



typedef void (*kbp_db_index_callback)(void *handle, struct kbp_db *db, struct kbp_entry *entry,
                                      int32_t old_index, int32_t new_index);



kbp_status kbp_db_get_prefix_handle(struct kbp_db *db, uint8_t *prefix, uint32_t length, struct kbp_entry **entry);



kbp_status kbp_db_search(struct kbp_db *db, uint8_t *key, struct kbp_entry **entry,
                         int32_t *index, int32_t *prio_len);


kbp_status kbp_db_counter_read_initiate(struct kbp_db *db);


kbp_status kbp_db_is_counter_read_complete(struct kbp_db *db, int32_t *is_complete);



struct kbp_entry_iter;



kbp_status kbp_db_entry_iter_init(struct kbp_db *db, struct kbp_entry_iter **iter);



kbp_status kbp_db_entry_iter_next(struct kbp_db *db, struct kbp_entry_iter *iter, struct kbp_entry **entry);



kbp_status kbp_db_entry_iter_destroy(struct kbp_db *db, struct kbp_entry_iter *iter);



kbp_status kbp_entry_get_priority(struct kbp_db *db, struct kbp_entry *entry, uint32_t *prio_length);



kbp_status kbp_entry_get_ad(struct kbp_db *db, struct kbp_entry *entry, struct kbp_ad **ad);



kbp_status kbp_entry_get_hb(struct kbp_db *db, struct kbp_entry *entry, struct kbp_hb **hb);


kbp_status kbp_entry_get_counter_value(struct kbp_db *db, struct kbp_entry *entry, uint64_t *value);



kbp_status kbp_entry_get_index(struct kbp_db *db, struct kbp_entry *entry, int32_t *nindices, int32_t **indices);



kbp_status kbp_entry_free_index_array(struct kbp_db *db, int32_t *indices);



struct kbp_entry_info
{
    uint8_t width_8;   
    uint8_t nranges;   
    uint8_t active;    

    uint8_t which_half; 

    uint16_t meta_priority; 
    uint32_t prio_len; 
    uint8_t data[KBP_HW_MAX_DBA_WIDTH_8]; 
    uint8_t mask[KBP_HW_MAX_DBA_WIDTH_8]; 
    struct kbp_ad *ad_handle; 
    struct kbp_ad_db *ad_db; 
    struct kbp_entry_rinfo  {
        uint16_t lo;  
        uint16_t hi;  
    } rinfo[KBP_HW_MAX_RANGE_COMPARES]; 
};



kbp_status kbp_entry_get_info(struct kbp_db *db, struct kbp_entry *entry, struct kbp_entry_info *info);







kbp_status kbp_entry_set_used(struct kbp_db *db, struct kbp_entry *entry);



kbp_status kbp_db_refresh_handle(struct kbp_device *device, struct kbp_db *stale_ptr, struct kbp_db **dbp);



#ifdef __cplusplus
}

#endif
#endif   

