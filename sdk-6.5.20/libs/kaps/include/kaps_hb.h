 

#ifndef __KAPS_HB_H
#define __KAPS_HB_H

 

#include <stdint.h>
#include <stdio.h>

#include "kaps_errors.h"
#include "kaps_device.h"
#include "kaps_db.h"

#ifdef __cplusplus
 extern "C" {
#endif

 

 

 struct kaps_hb_db;

 

 struct kaps_hb;

 

 kaps_status kaps_hb_db_init(struct kaps_device *device, uint32_t id, uint32_t capacity,
                           struct kaps_hb_db **hb_dbp);

 

 kaps_status kaps_hb_db_destroy(struct kaps_hb_db *hb_db);

 

 kaps_status kaps_hb_db_set_property(struct kaps_hb_db *hb_db, enum kaps_db_properties property, ...);

 

 kaps_status kaps_hb_db_get_property(struct kaps_hb_db *hb_db, enum kaps_db_properties property, ...);

 

 kaps_status kaps_hb_db_add_entry(struct kaps_hb_db *hb_db, struct kaps_hb **hb);

 

 kaps_status kaps_hb_db_delete_entry(struct kaps_hb_db *hb_db, struct kaps_hb *hb);

 

 kaps_status kaps_hb_db_delete_all_entries(struct kaps_hb_db *hb_db);

 
 kaps_status kaps_hb_db_delete_unused_entries(struct kaps_hb_db *hb_db);

 
 kaps_status kaps_hb_db_timer(struct kaps_hb_db *hb_db);




kaps_status kaps_db_get_aged_entries(struct kaps_hb_db *hb_db, uint32_t buf_size, uint32_t *num_entries,
                                   struct kaps_entry **entries);



kaps_status kaps_hb_db_refresh_handle(struct kaps_device *device, struct kaps_hb_db *stale_ptr, struct kaps_hb_db **dbp);





struct kaps_aged_entry_iter;



kaps_status kaps_hb_db_aged_entry_iter_init(struct kaps_hb_db *hb_db, struct kaps_aged_entry_iter **iter);



kaps_status kaps_hb_db_aged_entry_iter_next(struct kaps_hb_db *hb_db, struct kaps_aged_entry_iter *iter, struct kaps_entry **entry);


kaps_status kaps_hb_db_aged_entry_iter_destroy(struct kaps_hb_db *hb_db, struct kaps_aged_entry_iter *iter);



kaps_status kaps_hb_entry_get_idle_count(struct kaps_hb_db *hb_db, struct kaps_hb *hb, uint32_t *idle_count);


kaps_status kaps_hb_entry_set_idle_count(struct kaps_hb_db *hb_db, struct kaps_hb *hb, uint32_t idle_count);



kaps_status kaps_hb_entry_get_bit_value(struct kaps_hb_db *hb_db, struct kaps_hb *hb_handle, uint32_t *bit_value, uint8_t clear_on_read);






#ifdef __cplusplus
 }
#endif
#endif 

