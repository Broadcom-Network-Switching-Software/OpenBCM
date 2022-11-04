

#ifndef __KAPS_JR1_DB_H
#define __KAPS_JR1_DB_H

#include <stdint.h>

#include "kaps_jr1_errors.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_hw_limits.h"

#ifdef __cplusplus
extern "C" {
#endif





struct kaps_jr1_db;



struct kaps_jr1_entry;



struct kaps_jr1_ad;



struct kaps_jr1_hb;



struct kaps_jr1_key;



struct kaps_jr1_ad_db;



enum kaps_jr1_db_insertion_mode {
    KAPS_JR1_DB_INSERTION_MODE_NONE,
    KAPS_JR1_DB_INSERTION_MODE_NORMAL,
    KAPS_JR1_DB_INSERTION_MODE_ENTRY_WITH_INDEX,
    KAPS_JR1_DB_INSERTION_MODE_RELATIVE
};




enum kaps_jr1_entry_position
{
    KAPS_JR1_DB_ENTRY_ADD_BEFORE = 0,
    KAPS_JR1_DB_ENTRY_ADD_AFTER = 1
};





enum kaps_jr1_db_entry_meta_priority {
    KAPS_JR1_ENTRY_META_PRIORITY_0 = 0, 
    KAPS_JR1_ENTRY_META_PRIORITY_1 = 1,
    KAPS_JR1_ENTRY_META_PRIORITY_2 = 2,
    KAPS_JR1_ENTRY_META_PRIORITY_3 = 3  
};



struct kaps_jr1_hb_db;



enum kaps_jr1_db_properties {
    KAPS_JR1_PROP_ALGORITHMIC,             
    KAPS_JR1_PROP_INDEX_CALLBACK,          
    KAPS_JR1_PROP_DESCRIPTION,             
    KAPS_JR1_PROP_REDUCED_INDEX_CALLBACKS, 
    KAPS_JR1_PROP_DMA_TAG,                 
    KAPS_JR1_PROP_ENTRY_META_PRIORITY,     
    KAPS_JR1_PROP_AGE_COUNT,               
    KAPS_JR1_PROP_DEFER_DELETES,           
    KAPS_JR1_PROP_LOG,                     
    KAPS_JR1_PROP_ENABLE_DYNAMIC_ALLOCATION,
    KAPS_JR1_PROP_NUM_LEVELS_IN_DB,         
    KAPS_JR1_PROP_INVALID                  
};



enum kaps_jr1_entry_properties {
    KAPS_JR1_ENTRY_PROP_META_PRIORITY, 
    KAPS_JR1_ENTRY_PROP_INVALID        
};





kaps_jr1_status kaps_jr1_db_init(struct kaps_jr1_device *device, enum kaps_jr1_db_type type,
                       uint32_t id, uint32_t capacity, struct kaps_jr1_db **dbp);




kaps_jr1_status kaps_jr1_db_destroy(struct kaps_jr1_db *db);



kaps_jr1_status kaps_jr1_db_set_resource(struct kaps_jr1_db *db, enum kaps_jr1_hw_resource resource, ...);



kaps_jr1_status kaps_jr1_db_get_resource(struct kaps_jr1_db *db, enum kaps_jr1_hw_resource resource, ...);



kaps_jr1_status kaps_jr1_db_set_key(struct kaps_jr1_db *db, struct kaps_jr1_key *key);



kaps_jr1_status kaps_jr1_db_set_property(struct kaps_jr1_db *db, enum kaps_jr1_db_properties property, ...);



kaps_jr1_status kaps_jr1_db_get_property(struct kaps_jr1_db *db, enum kaps_jr1_db_properties property, ...);



kaps_jr1_status kaps_jr1_db_print(struct kaps_jr1_db *db, FILE * fp);



kaps_jr1_status kaps_jr1_db_set_ad(struct kaps_jr1_db *db, struct kaps_jr1_ad_db *ad_db);



kaps_jr1_status kaps_jr1_db_set_hb(struct kaps_jr1_db *db, struct kaps_jr1_hb_db *hb_db);




kaps_jr1_status kaps_jr1_db_add_table(struct kaps_jr1_db *db, uint32_t id, struct kaps_jr1_db **table);



kaps_jr1_status kaps_jr1_db_clone(struct kaps_jr1_db *db, uint32_t id, struct kaps_jr1_db **clone);




kaps_jr1_status kaps_jr1_db_add_prefix(struct kaps_jr1_db *db, uint8_t *prefix, uint32_t length,
                             struct kaps_jr1_entry **entry);



kaps_jr1_status kaps_jr1_db_add_prefix_with_index(struct kaps_jr1_db *db, uint8_t *prefix,
                                        uint32_t length, uint32_t index);



kaps_jr1_status kaps_jr1_db_add_em(struct kaps_jr1_db *db, uint8_t *data, struct kaps_jr1_entry **entry);


kaps_jr1_status kaps_jr1_db_add_em_with_index(struct kaps_jr1_db *db, uint8_t *data, uint32_t index);



kaps_jr1_status kaps_jr1_db_delete_entry(struct kaps_jr1_db *db, struct kaps_jr1_entry *entry);



kaps_jr1_status kaps_jr1_entry_set_property(struct kaps_jr1_db *db, struct kaps_jr1_entry *entry, enum kaps_jr1_entry_properties property, ...);





kaps_jr1_status kaps_jr1_entry_add_ad(struct kaps_jr1_db *db, struct kaps_jr1_entry *entry, struct kaps_jr1_ad *ad);



kaps_jr1_status kaps_jr1_entry_add_hb(struct kaps_jr1_db *db, struct kaps_jr1_entry *entry, struct kaps_jr1_hb *hb);



kaps_jr1_status kaps_jr1_entry_print(struct kaps_jr1_db *db, struct kaps_jr1_entry *entry, FILE * fp);



kaps_jr1_status kaps_jr1_db_install(struct kaps_jr1_db *db);



kaps_jr1_status kaps_jr1_db_delete_all_entries(struct kaps_jr1_db *db);



kaps_jr1_status kaps_jr1_db_delete_all_pending_entries(struct kaps_jr1_db *db);



struct kaps_jr1_db_stats {
    uint32_t num_entries;       
    uint32_t capacity_estimate; 
};



kaps_jr1_status kaps_jr1_db_stats(struct kaps_jr1_db *db, struct kaps_jr1_db_stats *stats);



typedef void (*kaps_jr1_db_index_callback)(void *handle, struct kaps_jr1_db *db, struct kaps_jr1_entry *entry,
                                      int32_t old_index, int32_t new_index);



kaps_jr1_status kaps_jr1_db_get_prefix_handle(struct kaps_jr1_db *db, uint8_t *prefix, uint32_t length, struct kaps_jr1_entry **entry);



kaps_jr1_status kaps_jr1_db_search(struct kaps_jr1_db *db, uint8_t *key, struct kaps_jr1_entry **entry,
                         int32_t *index, int32_t *prio_len);




struct kaps_jr1_entry_iter;



kaps_jr1_status kaps_jr1_db_entry_iter_init(struct kaps_jr1_db *db, struct kaps_jr1_entry_iter **iter);



kaps_jr1_status kaps_jr1_db_entry_iter_next(struct kaps_jr1_db *db, struct kaps_jr1_entry_iter *iter, struct kaps_jr1_entry **entry);



kaps_jr1_status kaps_jr1_db_entry_iter_destroy(struct kaps_jr1_db *db, struct kaps_jr1_entry_iter *iter);



kaps_jr1_status kaps_jr1_entry_get_priority(struct kaps_jr1_db *db, struct kaps_jr1_entry *entry, uint32_t *prio_length);



kaps_jr1_status kaps_jr1_entry_get_ad(struct kaps_jr1_db *db, struct kaps_jr1_entry *entry, struct kaps_jr1_ad **ad);



kaps_jr1_status kaps_jr1_entry_get_hb(struct kaps_jr1_db *db, struct kaps_jr1_entry *entry, struct kaps_jr1_hb **hb);




kaps_jr1_status kaps_jr1_entry_get_index(struct kaps_jr1_db *db, struct kaps_jr1_entry *entry, int32_t *nindices, int32_t **indices);



kaps_jr1_status kaps_jr1_entry_free_index_array(struct kaps_jr1_db *db, int32_t *indices);



struct kaps_jr1_entry_info
{
    uint8_t width_8;   
    uint8_t nranges;   
    uint8_t active;    

    uint16_t meta_priority; 
    uint32_t prio_len; 
    uint8_t data[KAPS_JR1_HW_MAX_DBA_WIDTH_8]; 
    uint8_t mask[KAPS_JR1_HW_MAX_DBA_WIDTH_8]; 
    struct kaps_jr1_ad *ad_handle; 
    struct kaps_jr1_ad_db *ad_db; 
    uint32_t ad_width_in_bits; 
};



kaps_jr1_status kaps_jr1_entry_get_info(struct kaps_jr1_db *db, struct kaps_jr1_entry *entry, struct kaps_jr1_entry_info *info);







kaps_jr1_status kaps_jr1_entry_set_used(struct kaps_jr1_db *db, struct kaps_jr1_entry *entry);



kaps_jr1_status kaps_jr1_db_refresh_handle(struct kaps_jr1_device *device, struct kaps_jr1_db *stale_ptr, struct kaps_jr1_db **dbp);



#ifdef __cplusplus
}

#endif
#endif   

