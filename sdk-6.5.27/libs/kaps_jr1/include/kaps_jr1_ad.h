

#ifndef __KAPS_JR1_AD_H
#define __KAPS_JR1_AD_H



#include <stdint.h>
#include <stdio.h>

#include "kaps_jr1_errors.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_db.h"

#ifdef __cplusplus
extern "C" {
#endif





struct kaps_jr1_ad_db;



struct kaps_jr1_ad;



kaps_jr1_status kaps_jr1_ad_db_init(struct kaps_jr1_device *device, uint32_t id, uint32_t capacity,
                          uint32_t width_1, struct kaps_jr1_ad_db **db);



kaps_jr1_status kaps_jr1_ad_db_destroy(struct kaps_jr1_ad_db *db);



kaps_jr1_status kaps_jr1_ad_db_set_resource(struct kaps_jr1_ad_db *db, enum kaps_jr1_hw_resource resource, ...);



kaps_jr1_status kaps_jr1_ad_db_get_resource(struct kaps_jr1_ad_db *db, enum kaps_jr1_hw_resource resource, ...);



kaps_jr1_status kaps_jr1_ad_db_set_property(struct kaps_jr1_ad_db *db, enum kaps_jr1_db_properties property, ...);



kaps_jr1_status kaps_jr1_ad_db_get_property(struct kaps_jr1_ad_db *db, enum kaps_jr1_db_properties property, ...);



kaps_jr1_status kaps_jr1_ad_db_add_entry(struct kaps_jr1_ad_db *db, uint8_t * value, struct kaps_jr1_ad **ad);



kaps_jr1_status kaps_jr1_ad_db_delete_entry(struct kaps_jr1_ad_db *db, struct kaps_jr1_ad *ad);



kaps_jr1_status kaps_jr1_ad_db_update_entry(struct kaps_jr1_ad_db *db, struct kaps_jr1_ad *ad, uint8_t * value);



kaps_jr1_status kaps_jr1_ad_db_get(struct kaps_jr1_ad_db *db, struct kaps_jr1_ad *ad, uint8_t * value);



kaps_jr1_status kaps_jr1_ad_entry_print(struct kaps_jr1_ad_db *db, struct kaps_jr1_ad *ad, FILE * fp);



kaps_jr1_status kaps_jr1_ad_db_delete_all_entries(struct kaps_jr1_ad_db *db);




kaps_jr1_status kaps_jr1_ad_db_delete_unused_entries(struct kaps_jr1_ad_db *db);








kaps_jr1_status kaps_jr1_ad_db_refresh_handle(struct kaps_jr1_device *device, struct kaps_jr1_ad_db *stale_ptr, struct kaps_jr1_ad_db **dbp);



#ifdef __cplusplus
}
#endif
#endif 
