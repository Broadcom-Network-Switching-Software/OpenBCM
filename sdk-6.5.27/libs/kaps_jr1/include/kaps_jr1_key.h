

#ifndef __KAPS_JR1_KEY_H
#define __KAPS_JR1_KEY_H



#include <stdint.h>

#include "kaps_jr1_errors.h"
#include "kaps_jr1_device.h"

#ifdef __cplusplus
extern "C" {
#endif





struct kaps_jr1_key;



enum kaps_jr1_key_field_type {
    KAPS_JR1_KEY_FIELD_TERNARY,
    KAPS_JR1_KEY_FIELD_PREFIX, 
    KAPS_JR1_KEY_FIELD_EM,     
    KAPS_JR1_KEY_FIELD_RANGE,  
    KAPS_JR1_KEY_FIELD_TABLE_ID, 
    KAPS_JR1_KEY_FIELD_HOLE,   
    
    KAPS_JR1_KEY_FIELD_DUMMY_FILL, 
    KAPS_JR1_KEY_FIELD_INVALID
};



kaps_jr1_status kaps_jr1_key_init(struct kaps_jr1_device *device, struct kaps_jr1_key **key);



kaps_jr1_status kaps_jr1_key_add_field(struct kaps_jr1_key *key, char *name, uint32_t width_1, enum kaps_jr1_key_field_type type);


kaps_jr1_status kaps_jr1_key_overlay_field(struct kaps_jr1_key *master_key, char *name, uint32_t width_1, enum kaps_jr1_key_field_type type, uint32_t offset_1);





kaps_jr1_status kaps_jr1_key_set_critical_field(struct kaps_jr1_key *key, char *name);





kaps_jr1_status kaps_jr1_key_verify(struct kaps_jr1_key *master_key, struct kaps_jr1_key *db_key, char **error_field);



kaps_jr1_status kaps_jr1_key_print(struct kaps_jr1_key *key, FILE * fp);



#ifdef __cplusplus
}
#endif
#endif 
