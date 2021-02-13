

#ifndef __KAPS_KEY_H
#define __KAPS_KEY_H



#include <stdint.h>

#include "kaps_errors.h"
#include "kaps_device.h"

#ifdef __cplusplus
extern "C" {
#endif





struct kaps_key;



enum kaps_key_field_type {
    KAPS_KEY_FIELD_TERNARY,
    KAPS_KEY_FIELD_PREFIX, 
    KAPS_KEY_FIELD_EM,     
    KAPS_KEY_FIELD_RANGE,  
    KAPS_KEY_FIELD_TABLE_ID, 
    KAPS_KEY_FIELD_HOLE,   
    
    KAPS_KEY_FIELD_DUMMY_FILL, 
    KAPS_KEY_FIELD_INVALID
};



kaps_status kaps_key_init(struct kaps_device *device, struct kaps_key **key);



kaps_status kaps_key_add_field(struct kaps_key *key, char *name, uint32_t width_1, enum kaps_key_field_type type);


kaps_status kaps_key_overlay_field(struct kaps_key *master_key, char *name, uint32_t width_1, enum kaps_key_field_type type, uint32_t offset_1);





kaps_status kaps_key_set_critical_field(struct kaps_key *key, char *name);





kaps_status kaps_key_verify(struct kaps_key *master_key, struct kaps_key *db_key, char **error_field);



kaps_status kaps_key_print(struct kaps_key *key, FILE * fp);



#ifdef __cplusplus
}
#endif
#endif 
