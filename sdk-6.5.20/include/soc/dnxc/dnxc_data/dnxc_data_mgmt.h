
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXC_DATA_MGMT_H_

#define _DNXC_DATA_MGMT_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <shared/alloc.h>
#include <shared/dbx/dbx_file.h>
#include <soc/mcm/allenum.h>

#define DNXC_DATA_STATE_F_INIT_DONE      (0x1)

#define DNXC_DATA_STATE_F_DEV_DATA_DONE  (0x2)

#define DNXC_DATA_STATE_F_PROP_DONE      (0x4)

#define DNXC_DATA_STATE_F_BCM_INIT_DONE  (0x8)

#define DNXC_DATA_STATE_F_OVERRIDE       (0x100)

#define DNXC_DATA_F_ALL                  (0x0)

#define DNXC_DATA_F_DEFINE               (0x1)

#define DNXC_DATA_F_FEATURE              (0x2)

#define DNXC_DATA_F_TABLE                (0x4)

#define DNXC_DATA_F_NUMERIC              (0x8)

#define DNXC_DATA_FLAG_DATA_TYPES_MASK   (0xF)

#define DNXC_DATA_F_PROPERTY             (0x100)

#define DNXC_DATA_F_INIT_ONLY            (0x200)

#define DNXC_DATA_F_SUPPORTED            (0x1000)

#define DNXC_DATA_F_CHANGED              (0x2000)

#define DNXC_DATA_F_PROPERTY_INTERNAL     (0x4000)

#define DNXC_DATA_LABEL_FLEXE SAL_BIT(0)

#define DNXC_DATA_LABEL_MACSEC SAL_BIT(1)

#define DNXC_DATA_MGMT_MAX_TABLE_KEYS                 (2)

#define DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH         (1000)

#define DNXC_DATA_LABELS_MAX                           (5)

#define DNXC_DATA_ALLOC(ptr, type, nof_elements, msg)\
    ptr = sal_alloc(sizeof(type) * nof_elements, msg);\
    if (ptr == NULL)\
    {\
        SHR_ERR_EXIT(_SHR_E_MEMORY, "failed to allocate data of %s\n", msg);\
    }\
    sal_memset(ptr, 0, sizeof(type) * nof_elements)

#define DNXC_DATA_FREE(ptr)\
    if (ptr != NULL) sal_free(ptr);\
    ptr = NULL

#define DNXC_DATA_MGMT_ARR_STR(buffer, size, data)\
    do\
    {\
        int i, offset = 0;\
        for (i = 0; i < size; i++)\
        {\
            if (offset + 10 > DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH) break; \
            offset += sal_sprintf(buffer + offset, "%08x ", data[i]);\
        }\
    } while(0)

#define DNXC_DATA_MGMT_PBMP_STR(buffer, data)\
    do\
    {\
        int i, offset = 0;\
        for (i = 0; i < _SHR_PBMP_WORD_MAX; i++)\
        {\
            if (offset + 10 > DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH) break;\
            offset += sal_sprintf(buffer + offset, "%08x ", _SHR_PBMP_WORD_GET(data, i));\
        }\
    } while(0)

typedef enum
{

    dnxc_data_property_method_invalid = 0,

    dnxc_data_property_method_enable = 1,

    dnxc_data_property_method_port_enable = 2,

    dnxc_data_property_method_suffix_enable = 3,

    dnxc_data_property_method_port_suffix_enable = 4,

    dnxc_data_property_method_suffix_only_enable = 5,

    dnxc_data_property_method_range = 6,

    dnxc_data_property_method_range_signed = 7,

    dnxc_data_property_method_port_range = 8,

    dnxc_data_property_method_port_range_signed = 9,

    dnxc_data_property_method_suffix_range = 10,

    dnxc_data_property_method_suffix_only_range = 11,

    dnxc_data_property_method_suffix_range_signed = 12,

    dnxc_data_property_method_port_suffix_range = 13,

    dnxc_data_property_method_port_suffix_range_signed = 14,

    dnxc_data_property_method_suffix_only_range_signed = 15,

    dnxc_data_property_method_direct_map = 16,

    dnxc_data_property_method_port_direct_map = 17,

    dnxc_data_property_method_suffix_direct_map = 18,

    dnxc_data_property_method_suffix_only_direct_map = 19,

    dnxc_data_property_method_port_suffix_direct_map = 20,

    dnxc_data_property_method_custom = 21,

    dnxc_data_property_method_pbmp = 22,

    dnxc_data_property_method_port_pbmp = 23,

    dnxc_data_property_method_suffix_pbmp = 24,

    dnxc_data_property_method_str = 25,

    dnxc_data_property_method_port_str = 26,

    dnxc_data_property_method_suffix_str = 27,

    dnxc_data_property_method_port_suffix_str = 28
} dnxc_data_property_method_e;

typedef struct
{

    char *name;

    uint32 val;

    int is_default;

    int is_invalid;
} dnxc_data_property_map_t;

typedef struct
{

    char *name;

    char *doc;

    dnxc_data_property_method_e method;

    char *method_str;

    uint32 is_static;

    char *suffix;

    uint32 range_min;
    uint32 range_max;

    dnxc_data_property_map_t *mapping;
    int nof_mapping;

    uint8 skip_is_active_check;

    uint32 label;

} dnxc_data_property_t;

typedef struct
{

    int key_size[DNXC_DATA_MGMT_MAX_TABLE_KEYS];
} dnxc_data_table_info_t;

typedef shr_error_e(
    *dnxc_data_table_entry_str_get_f) (
    int unit,
    char *buffer,
    int key1,
    int key2,
    int value_index);

typedef shr_error_e(
    *dnxc_data_value_set_f) (
    int unit);

typedef shr_error_e(
    *dnxc_data_key_map_f) (
    int unit,
    int key0_val,
    int key1_val,
    int *key0_index,
    int *key1_index);

typedef shr_error_e(
    *dnxc_data_key_map_reverse_f) (
    int unit,
    int key0_index,
    int key1_indexl,
    int *key0_val,
    int *key1_val);

typedef const dnxc_data_table_info_t *(
    *dnxc_data_table_info_get_f) (
    int unit);

typedef struct
{

    char *name;

    char *type;

    char *doc;

    char *default_val;

    int offset;

    dnxc_data_property_t property;

} dnxc_data_value_t;

typedef struct
{

    char *name;

    char *doc;

    int size;
} dnxc_data_key_t;

typedef struct
{

    uint32 flags;

    char *labels[DNXC_DATA_LABELS_MAX];

    char *name;

    char *doc;

    uint32 nof_keys;

    dnxc_data_key_t keys[DNXC_DATA_MGMT_MAX_TABLE_KEYS];

    uint32 nof_values;

    dnxc_data_value_t *values;

    uint32 size_of_values;

    char *data;

    dnxc_data_table_entry_str_get_f entry_get;

    dnxc_data_value_set_f set;

    dnxc_data_key_map_f key_map;

    dnxc_data_key_map_reverse_f key_map_reverse;

    dnxc_data_table_info_t info_get;

    dnxc_data_property_t property;

    xml_node dynamic_data_node;

} dnxc_data_table_t;

typedef struct
{

    uint32 flags;

    char *labels[DNXC_DATA_LABELS_MAX];

    char *name;

    char *doc;

    uint32 data;

    uint32 default_data;

    dnxc_data_value_set_f set;

    dnxc_data_property_t property;

    xml_node dynamic_data_node;

} dnxc_data_feature_t;

typedef struct
{

    uint32 flags;

    char *labels[DNXC_DATA_LABELS_MAX];

    char *name;

    char *doc;

    uint32 data;

    uint32 default_data;

    dnxc_data_value_set_f set;

    dnxc_data_property_t property;

    xml_node dynamic_data_node;

} dnxc_data_define_t;

typedef struct
{

    char *name;

    char *doc;

    uint32 nof_features;

    dnxc_data_feature_t *features;

    uint32 nof_defines;

    dnxc_data_define_t *defines;

    uint32 nof_tables;

    dnxc_data_table_t *tables;
} dnxc_data_submodule_t;

typedef struct
{

    char *name;

    dnxc_data_submodule_t *submodules;

    uint32 nof_submodules;
} dnxc_data_module_t;

typedef struct
{

    char *name;

    dnxc_data_module_t *modules;

    uint32 nof_modules;

    uint32 state;
} dnxc_data_t;

extern dnxc_data_t _dnxc_data[];

shr_error_e dnxc_data_mgmt_access_verify(
    int unit,
    uint32 state_flags,
    uint32 data_flags,
    dnxc_data_value_set_f set_func);

shr_error_e dnxc_data_mgmt_state_set(
    int unit,
    uint32 flags);

shr_error_e dnxc_data_mgmt_state_reset(
    int unit,
    uint32 flags);

shr_error_e dnxc_data_mgmt_deinit(
    int unit,
    dnxc_data_t * unit_data);

int dnxc_data_mgmt_feature_data_get(
    int unit,
    int module_index,
    int submodule_index,
    int feature_index);

uint32 dnxc_data_mgmt_define_data_get(
    int unit,
    int module_index,
    int submodule_index,
    int define_index);

dnxc_data_table_t *dnxc_data_mgmt_table_get(
    int unit,
    int module_index,
    int submodule_index,
    int table_index);

const dnxc_data_table_info_t *dnxc_data_mgmt_table_info_get(
    int unit,
    int module_index,
    int submodule_index,
    int table_index);

char *dnxc_data_mgmt_table_data_get(
    int unit,
    dnxc_data_table_t * table,
    int key1,
    int key2);

char *dnxc_data_mgmt_table_data_diag_get(
    int unit,
    dnxc_data_table_t * table,
    int key1,
    int key2);

shr_error_e dnxc_data_mgmt_property_read(
    int unit,
    dnxc_data_property_t * property,
    int key,
    void *value);

shr_error_e dnxc_data_mgmt_define_changed_set(
    int unit,
    dnxc_data_define_t * define);

shr_error_e dnxc_data_mgmt_feature_changed_set(
    int unit,
    dnxc_data_feature_t * feature);

shr_error_e dnxc_data_mgmt_table_changed_set(
    int unit,
    dnxc_data_table_t * table);

shr_error_e dnxc_data_mgmt_values_set(
    int unit);

shr_error_e dnxc_data_mgmt_table_entry_changed_get(
    int unit,
    dnxc_data_table_t * table,
    int key0,
    int key1,
    int *changed);

int dnxc_data_mgmt_is_jr2_a0(
    int unit);
int dnxc_data_mgmt_is_jr2_b0(
    int unit);
int dnxc_data_mgmt_is_jr2_b1(
    int unit);
int dnxc_data_mgmt_is_j2c_a0(
    int unit);
int dnxc_data_mgmt_is_j2c_a1(
    int unit);
int dnxc_data_mgmt_is_q2a_a0(
    int unit);
int dnxc_data_mgmt_is_q2a_b0(
    int unit);
int dnxc_data_mgmt_is_q2a_b1(
    int unit);
int dnxc_data_mgmt_is_j2p_a0(
    int unit);
int dnxc_data_mgmt_is_ramon(
    int unit);

#endif
