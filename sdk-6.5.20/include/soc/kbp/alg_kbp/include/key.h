/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __KEY_H
#define __KEY_H



#include <stdint.h>

#include "errors.h"
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif





struct kbp_key;



enum kbp_key_field_type {
    KBP_KEY_FIELD_TERNARY,
    KBP_KEY_FIELD_PREFIX, 
    KBP_KEY_FIELD_EM,     
    KBP_KEY_FIELD_RANGE,  
    KBP_KEY_FIELD_TABLE_ID, 
    KBP_KEY_FIELD_HOLE,   
    
    KBP_KEY_FIELD_DUMMY_FILL, 
    KBP_KEY_FIELD_INVALID
};



kbp_status kbp_key_init(struct kbp_device *device, struct kbp_key **key);



kbp_status kbp_key_add_field(struct kbp_key *key, char *name, uint32_t width_1, enum kbp_key_field_type type);


kbp_status kbp_key_overlay_field(struct kbp_key *master_key, char *name, uint32_t width_1, enum kbp_key_field_type type, uint32_t offset_1);





kbp_status kbp_key_set_critical_field(struct kbp_key *key, char *name);





kbp_status kbp_key_verify(struct kbp_key *master_key, struct kbp_key *db_key, char **error_field);



kbp_status kbp_key_print(struct kbp_key *key, FILE * fp);



#ifdef __cplusplus
}
#endif
#endif 
