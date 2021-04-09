

#ifndef __KAPS_DB_H
#define __KAPS_DB_H

#include <stdint.h>

#include "kaps_errors.h"
#include "kaps_device.h"
#include "kaps_hw_limits.h"

#ifdef __cplusplus
extern "C" {
#endif





struct kaps_db;



struct kaps_entry;



struct kaps_ad;



struct kaps_hb;



struct kaps_key;



struct kaps_ad_db;



enum kaps_db_insertion_mode {
    KAPS_DB_INSERTION_MODE_NONE,
    KAPS_DB_INSERTION_MODE_NORMAL,
    KAPS_DB_INSERTION_MODE_ENTRY_WITH_INDEX,
    KAPS_DB_INSERTION_MODE_RELATIVE
};




enum kaps_entry_position
{
    KAPS_DB_ENTRY_ADD_BEFORE = 0,
    KAPS_DB_ENTRY_ADD_AFTER = 1
};





enum kaps_db_entry_meta_priority {
    KAPS_ENTRY_META_PRIORITY_0 = 0, 
    KAPS_ENTRY_META_PRIORITY_1 = 1,
    KAPS_ENTRY_META_PRIORITY_2 = 2,
    KAPS_ENTRY_META_PRIORITY_3 = 3  
};



struct kaps_hb_db;



enum kaps_db_properties {
    KAPS_PROP_USE_MCOR,                
    KAPS_PROP_MCOR_VALUE,              
    KAPS_PROP_ALGORITHMIC,             
    KAPS_PROP_INDEX_RANGE,             
    KAPS_PROP_INDEX_CALLBACK,          
    KAPS_PROP_MIN_PRIORITY,            
    KAPS_PROP_DESCRIPTION,             
    KAPS_PROP_CASCADE_DEVICE,          
    KAPS_PROP_REDUCED_INDEX_CALLBACKS, 
    KAPS_PROP_SAVE_IX_SPACE,           
    KAPS_PROP_DMA_TAG,                 
    KAPS_PROP_ENTRY_META_PRIORITY,     
    KAPS_PROP_ENABLE_DB_COMPACTION,    
    KAPS_PROP_MAX_CAPACITY,            
    KAPS_PROP_AGE_COUNT,               
    KAPS_PROP_PAIR_WITH_DB,            
    KAPS_PROP_DEFER_DELETES,           
    KAPS_PROP_SCALE_UP_CAPACITY,       
    KAPS_PROP_LOG,                     
    KAPS_PROP_MAX_PRIORITY,            
    KAPS_PROP_SEND_PRIORITY_OVER_AD,   
    KAPS_PROP_ENABLE_COUNTERS,         
    KAPS_PROP_COUNTER_CYCLE,           
    KAPS_PROP_COUNTER_TYPE,            
    KAPS_PROP_XL_DB,                   
    KAPS_PROP_LOCALITY,                
    KAPS_PROP_REPLICATE_DB,            
    KAPS_PROP_MC_DB,                   
    KAPS_PROP_ENABLE_DYNAMIC_ALLOCATION,
    KAPS_PROP_UTILISE_UDA_HOLES,        
    KAPS_PROP_NUM_LEVELS_IN_DB,         
    KAPS_PROP_INVALID                  
};



enum kaps_entry_properties {
    KAPS_ENTRY_PROP_META_PRIORITY, 
    KAPS_ENTRY_PROP_INVALID        
};





kaps_status kaps_db_init(struct kaps_device *device, enum kaps_db_type type,
                       uint32_t id, uint32_t capacity, struct kaps_db **dbp);


kaps_status kaps_db_create_broadcast(struct kaps_device *device,
                                   struct kaps_db *orig_db, struct kaps_db **bc_db);




kaps_status kaps_db_destroy(struct kaps_db *db);



kaps_status kaps_db_set_resource(struct kaps_db *db, enum kaps_hw_resource resource, ...);



kaps_status kaps_db_get_resource(struct kaps_db *db, enum kaps_hw_resource resource, ...);



kaps_status kaps_db_set_key(struct kaps_db *db, struct kaps_key *key);



kaps_status kaps_db_set_property(struct kaps_db *db, enum kaps_db_properties property, ...);



kaps_status kaps_db_get_property(struct kaps_db *db, enum kaps_db_properties property, ...);



kaps_status kaps_db_print(struct kaps_db *db, FILE * fp);



kaps_status kaps_db_set_ad(struct kaps_db *db, struct kaps_ad_db *ad_db);



kaps_status kaps_db_set_hb(struct kaps_db *db, struct kaps_hb_db *hb_db);




kaps_status kaps_db_add_table(struct kaps_db *db, uint32_t id, struct kaps_db **table);



kaps_status kaps_db_clone(struct kaps_db *db, uint32_t id, struct kaps_db **clone);




kaps_status kaps_db_add_prefix(struct kaps_db *db, uint8_t *prefix, uint32_t length,
                             struct kaps_entry **entry);



kaps_status kaps_db_add_prefix_with_index(struct kaps_db *db, uint8_t *prefix,
                                        uint32_t length, uint32_t index);



kaps_status kaps_db_add_em(struct kaps_db *db, uint8_t *data, struct kaps_entry **entry);


kaps_status kaps_db_add_em_with_index(struct kaps_db *db, uint8_t *data, uint32_t index);



kaps_status kaps_db_delete_entry(struct kaps_db *db, struct kaps_entry *entry);



kaps_status kaps_entry_set_property(struct kaps_db *db, struct kaps_entry *entry, enum kaps_entry_properties property, ...);



kaps_status kaps_entry_add_range(struct kaps_db *db, struct kaps_entry *entry,
                               uint16_t lo, uint16_t hi, int32_t range_no);



kaps_status kaps_entry_add_ad(struct kaps_db *db, struct kaps_entry *entry, struct kaps_ad *ad);



kaps_status kaps_entry_add_hb(struct kaps_db *db, struct kaps_entry *entry, struct kaps_hb *hb);



kaps_status kaps_entry_print(struct kaps_db *db, struct kaps_entry *entry, FILE * fp);



kaps_status kaps_db_install(struct kaps_db *db);



kaps_status kaps_db_delete_all_entries(struct kaps_db *db);



kaps_status kaps_db_delete_all_pending_entries(struct kaps_db *db);



struct kaps_db_stats {
    uint32_t num_entries;       
    uint32_t capacity_estimate; 
    uint32_t range_expansion;   
};



kaps_status kaps_db_stats(struct kaps_db *db, struct kaps_db_stats *stats);



typedef void (*kaps_db_index_callback)(void *handle, struct kaps_db *db, struct kaps_entry *entry,
                                      int32_t old_index, int32_t new_index);



kaps_status kaps_db_get_prefix_handle(struct kaps_db *db, uint8_t *prefix, uint32_t length, struct kaps_entry **entry);



kaps_status kaps_db_search(struct kaps_db *db, uint8_t *key, struct kaps_entry **entry,
                         int32_t *index, int32_t *prio_len);




struct kaps_entry_iter;



kaps_status kaps_db_entry_iter_init(struct kaps_db *db, struct kaps_entry_iter **iter);



kaps_status kaps_db_entry_iter_next(struct kaps_db *db, struct kaps_entry_iter *iter, struct kaps_entry **entry);



kaps_status kaps_db_entry_iter_destroy(struct kaps_db *db, struct kaps_entry_iter *iter);



kaps_status kaps_entry_get_priority(struct kaps_db *db, struct kaps_entry *entry, uint32_t *prio_length);



kaps_status kaps_entry_get_ad(struct kaps_db *db, struct kaps_entry *entry, struct kaps_ad **ad);



kaps_status kaps_entry_get_hb(struct kaps_db *db, struct kaps_entry *entry, struct kaps_hb **hb);




kaps_status kaps_entry_get_index(struct kaps_db *db, struct kaps_entry *entry, int32_t *nindices, int32_t **indices);



kaps_status kaps_entry_free_index_array(struct kaps_db *db, int32_t *indices);



struct kaps_entry_info
{
    uint8_t width_8;   
    uint8_t nranges;   
    uint8_t active;    

    uint8_t which_half; 

    uint16_t meta_priority; 
    uint32_t prio_len; 
    uint8_t data[KAPS_HW_MAX_DBA_WIDTH_8]; 
    uint8_t mask[KAPS_HW_MAX_DBA_WIDTH_8]; 
    struct kaps_ad *ad_handle; 
    struct kaps_ad_db *ad_db; 

};



kaps_status kaps_entry_get_info(struct kaps_db *db, struct kaps_entry *entry, struct kaps_entry_info *info);







kaps_status kaps_entry_set_used(struct kaps_db *db, struct kaps_entry *entry);



kaps_status kaps_db_refresh_handle(struct kaps_device *device, struct kaps_db *stale_ptr, struct kaps_db **dbp);



#ifdef __cplusplus
}

#endif
#endif   

