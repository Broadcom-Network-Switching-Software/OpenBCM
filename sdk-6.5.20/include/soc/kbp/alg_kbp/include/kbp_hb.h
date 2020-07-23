/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 

#ifndef __KBP_HB_H
#define __KBP_HB_H

 

#include <stdint.h>
#include <stdio.h>

#include "errors.h"
#include "device.h"
#include "db.h"

#ifdef __cplusplus
 extern "C" {
#endif

 

 

struct kbp_hb_db;

 

struct kbp_hb;

 

kbp_status kbp_hb_db_init(struct kbp_device *device, uint32_t id, uint32_t capacity,
                          struct kbp_hb_db **hb_dbp);

 

kbp_status kbp_hb_db_destroy(struct kbp_hb_db *hb_db);

 

kbp_status kbp_hb_db_set_property(struct kbp_hb_db *hb_db, enum kbp_db_properties property, ...);

 

kbp_status kbp_hb_db_get_property(struct kbp_hb_db *hb_db, enum kbp_db_properties property, ...);

 

kbp_status kbp_hb_db_add_entry(struct kbp_hb_db *hb_db, struct kbp_hb **hb);

 

kbp_status kbp_hb_db_delete_entry(struct kbp_hb_db *hb_db, struct kbp_hb *hb);

 

kbp_status kbp_hb_db_delete_all_entries(struct kbp_hb_db *hb_db);

 
kbp_status kbp_hb_db_delete_unused_entries(struct kbp_hb_db *hb_db);

 
kbp_status kbp_hb_db_timer(struct kbp_hb_db *hb_db);




kbp_status kbp_db_get_aged_entries_count(struct kbp_hb_db *hb_db, uint32_t *num_entries);



kbp_status kbp_db_get_aged_entries(struct kbp_hb_db *hb_db, uint32_t buf_size, uint32_t *num_entries, struct kbp_entry **entries);



kbp_status kbp_hb_db_refresh_handle(struct kbp_device *device, struct kbp_hb_db *stale_ptr, struct kbp_hb_db **dbp);





struct kbp_aged_entry_iter;



kbp_status kbp_hb_db_aged_entry_iter_init(struct kbp_hb_db *hb_db, struct kbp_aged_entry_iter **iter);



kbp_status kbp_hb_db_aged_entry_iter_next(struct kbp_hb_db *hb_db, struct kbp_aged_entry_iter *iter, struct kbp_entry **entry);


kbp_status kbp_hb_db_aged_entry_iter_destroy(struct kbp_hb_db *hb_db, struct kbp_aged_entry_iter *iter);



kbp_status kbp_hb_entry_get_idle_count(struct kbp_hb_db *hb_db, struct kbp_hb *hb, uint32_t *idle_count);


kbp_status kbp_hb_entry_set_idle_count(struct kbp_hb_db *hb_db, struct kbp_hb *hb, uint32_t idle_count);



kbp_status kbp_hb_entry_get_bit_value(struct kbp_hb_db *hb_db, struct kbp_hb *hb_handle, uint32_t *bit_value, uint8_t clear_on_read);


kbp_status kbp_hb_db_read_initiate(struct kbp_hb_db *hb_db);


kbp_status kbp_hb_db_is_read_complete(struct kbp_hb_db *hb_db, int32_t *is_complete);




#ifdef __cplusplus
 }
#endif
#endif 

