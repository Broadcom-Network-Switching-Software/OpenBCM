/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 

#ifndef __KBP_TAP_H
#define __KBP_TAP_H



#include <stdint.h>
#include <stdio.h>

#include "errors.h"
#include "device.h"
#include "db.h"


#ifdef __cplusplus
 extern "C" {
#endif



struct kbp_tap_db;

struct kbp_tap_record;

struct kbp_tap_instruction;

struct kbp_tap_db_value;

struct kbp_tap_db_pointer;

struct kbp_tap_db_attribute;



enum kbp_tap_entry_type {
    KBP_TAP_ENTRY_TYPE_SINGLE = 0, 
    KBP_TAP_ENTRY_TYPE_PAIR   = 1, 
    KBP_TAP_ENTRY_TYPE_INVALID     
};


enum kbp_tap_record_length {
    KBP_TAP_RECORD_LENGTH_64 = 0,      
    KBP_TAP_RECORD_LENGTH_72 = 1,      
    KBP_TAP_RECORD_LENGTH_80 = 2,      
    KBP_TAP_RECORD_LENGTH_96 = 3,      
};


enum kbp_tap_record_type {
    KBP_TAP_RECORD_TYPE_NULL    = 0,   
    KBP_TAP_RECORD_TYPE_RSVD    = 1,   
    KBP_TAP_RECORD_TYPE_INGRESS = 2,   
    KBP_TAP_RECORD_TYPE_EGRESS  = 3,   
};


enum kbp_tap_instruction_type {
    KBP_TAP_INSTRUCTION_TYPE_INGRESS = 0, 
    KBP_TAP_INSTRUCTION_TYPE_EGRESS  = 1, 
    KBP_TAP_INSTRUCTION_TYPE_INVALID      
};


enum kbp_tap_record_field_type {
    KBP_TAP_RECORD_FIELD_VALUE,
    KBP_TAP_RECORD_FIELD_POINTER,
    KBP_TAP_RECORD_FIELD_ATTRIBUTE,
    KBP_TAP_RECORD_FIELD_OPCODE,
    KBP_TAP_RECORD_FIELD_OPCODE_EXT,
    KBP_TAP_RECORD_FIELD_HOLE,
    KBP_TAP_RECORD_FIELD_INVALID
};


enum kbp_tap_instruction_properties {
    KBP_TAP_INST_PROP_OPCODE_EXT_VALUE, 
    KBP_TAP_INST_PROP_INVALID
};


enum kbp_tap_db_properties {
     KBP_TAP_DB_PROP_RATE_COMPUTE, 
     KBP_TAP_DB_PROP_INVALID
};


kbp_status kbp_tap_db_init(struct kbp_device *device, uint32_t id,
                           uint32_t capacity, enum kbp_tap_entry_type type,
                           int32_t set_size, struct kbp_tap_db **tap_dbp);


kbp_status kbp_tap_db_destroy(struct kbp_tap_db *tap_db);



kbp_status kbp_tap_db_set_property(struct kbp_tap_db *tap_db, enum kbp_tap_db_properties property, ...);


kbp_status kbp_tap_record_init(struct kbp_device *device, enum kbp_tap_record_type type,
                               struct kbp_tap_record **record);


kbp_status kbp_tap_record_add_field(struct kbp_tap_record *record, char *name, uint32_t width_1,
                                    enum kbp_tap_record_field_type type);


kbp_status kbp_tap_instruction_init(struct kbp_device *device, uint32_t id,
                                    enum kbp_tap_instruction_type inst_type,
                                    struct kbp_tap_instruction **instr);


kbp_status kbp_tap_instruction_destroy(struct kbp_tap_instruction *instruction);



kbp_status kbp_tap_instruction_add_db(struct kbp_tap_instruction *inst, struct kbp_tap_db *db,
                                      uint32_t pointer_min, uint32_t pointer_max);


kbp_status kbp_tap_instruction_set_record(struct kbp_tap_instruction *inst, struct kbp_tap_record *record);


kbp_status kbp_tap_instruction_install(struct kbp_tap_instruction *instruction);


kbp_status kbp_tap_db_value_init(struct kbp_tap_db *db, struct kbp_tap_db_value **value);


kbp_status kbp_tap_db_value_set_field(struct kbp_tap_db_value *value, char *name,  uint32_t width_1);


kbp_status kbp_tap_db_pointer_init(struct kbp_tap_db *db, struct kbp_tap_db_pointer **pointer);


kbp_status kbp_tap_db_pointer_add_field(struct kbp_tap_db_pointer *pointer, char *name,  uint32_t width_1);


kbp_status kbp_tap_db_set_pointer(struct kbp_tap_db *db, struct kbp_tap_db_pointer *pointer);


kbp_status kbp_tap_db_attribute_init(struct kbp_tap_db *db, struct kbp_tap_db_attribute **attr);


kbp_status kbp_tap_db_attribute_add_field(struct kbp_tap_db_attribute *attr, char *name,  uint32_t width_1);


kbp_status kbp_tap_db_set_attribute(struct kbp_tap_db *db, struct kbp_tap_db_attribute *attr);


kbp_status kbp_tap_instruction_set_property(struct kbp_tap_instruction *instruction,
                                            enum kbp_tap_instruction_properties property, ...);


kbp_status kbp_tap_db_map_attr_value_to_entry(struct kbp_tap_db *db, int32_t attr_value,
											  int32_t entry_in_set);


kbp_status kbp_tap_db_read_initiate(struct kbp_tap_db *db);


kbp_status kbp_tap_db_is_read_complete(struct kbp_tap_db *db, int32_t *is_complete);


kbp_status kbp_tap_db_entry_get_value(struct kbp_tap_db *db, uint32_t entry_num,
                                      uint32_t counter_offset, uint64_t *value);


kbp_status kbp_tap_db_entry_pair_get_value(struct kbp_tap_db *db, uint32_t entry_num,
                                           uint32_t counter_offset, uint64_t *packet_count,
                                           uint64_t *byte_count);






kbp_status kbp_tap_db_refresh_handle(struct kbp_device *device, struct kbp_tap_db *stale_ptr,
                                     struct kbp_tap_db **dpp);


kbp_status kbp_tap_instruction_refresh_handle(struct kbp_device *device,
                                              struct kbp_tap_instruction *stale_ptr,
                                              struct kbp_tap_instruction **inst);



kbp_status kbp_tap_dump_port_regs(struct kbp_device *device, int32_t port_id, FILE *fp);



#ifdef __cplusplus
}
#endif
#endif 

