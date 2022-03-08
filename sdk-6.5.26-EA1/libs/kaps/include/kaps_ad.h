

#ifndef __KAPS_AD_H
#define __KAPS_AD_H



#include <stdint.h>
#include <stdio.h>

#include "kaps_errors.h"
#include "kaps_device.h"
#include "kaps_db.h"

#ifdef __cplusplus
extern "C" {
#endif





struct kaps_ad_db;



struct kaps_ad;



kaps_status kaps_ad_db_init(struct kaps_device *device, uint32_t id, uint32_t capacity,
                          uint32_t width_1, struct kaps_ad_db **db);



kaps_status kaps_ad_db_destroy(struct kaps_ad_db *db);



kaps_status kaps_ad_db_set_resource(struct kaps_ad_db *db, enum kaps_hw_resource resource, ...);



kaps_status kaps_ad_db_get_resource(struct kaps_ad_db *db, enum kaps_hw_resource resource, ...);



kaps_status kaps_ad_db_set_property(struct kaps_ad_db *db, enum kaps_db_properties property, ...);



kaps_status kaps_ad_db_get_property(struct kaps_ad_db *db, enum kaps_db_properties property, ...);



kaps_status kaps_ad_db_add_entry(struct kaps_ad_db *db, uint8_t * value, struct kaps_ad **ad);



kaps_status kaps_ad_db_delete_entry(struct kaps_ad_db *db, struct kaps_ad *ad);



kaps_status kaps_ad_db_update_entry(struct kaps_ad_db *db, struct kaps_ad *ad, uint8_t * value);



kaps_status kaps_ad_db_get(struct kaps_ad_db *db, struct kaps_ad *ad, uint8_t * value);



kaps_status kaps_ad_entry_print(struct kaps_ad_db *db, struct kaps_ad *ad, FILE * fp);



kaps_status kaps_ad_db_delete_all_entries(struct kaps_ad_db *db);




kaps_status kaps_ad_db_delete_unused_entries(struct kaps_ad_db *db);








kaps_status kaps_ad_db_refresh_handle(struct kaps_device *device, struct kaps_ad_db *stale_ptr, struct kaps_ad_db **dbp);



#ifdef __cplusplus
}
#endif
#endif 
