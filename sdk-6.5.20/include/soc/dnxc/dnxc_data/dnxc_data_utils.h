
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXC_DATA_UTILS_H_

#define _DNXC_DATA_UTILS_H_

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>

#define DNXC_DATA_UTILS_STRING_MAX (1024)

shr_error_e dnxc_data_utils_dump_verify(
    int unit,
    uint32 state_flags,
    uint32 data_flags,
    char **data_labels,
    uint32 dump_flags,
    char *dump_label,
    int *dump);

const uint32 *dnxc_data_utils_generic_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member);

const uint32 *dnxc_data_utils_generic_1d_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    int key1);

const uint32 *dnxc_data_utils_generic_value_get(
    int unit,
    char *dnx_data_str);

const uint32 *dnxc_data_utils_generic_2d_data_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    int key1,
    int key2);

dnxc_data_define_t *dnxc_data_utils_define_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *define_name);

dnxc_data_feature_t *dnxc_data_utils_feature_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *feature_name);

dnxc_data_table_t *dnxc_data_utils_table_info_get(
    int unit,
    char *module_name,
    char *submodule_name,
    char *table_name);

uint32 *dnxc_data_utils_table_member_data_pointer_get(
    int unit,
    dnxc_data_table_t * table_info,
    char *member_name,
    int key1,
    int key2);

shr_error_e dnxc_data_utils_label_set(
    int unit,
    char **labels,
    char *label);

shr_error_e dnxc_data_utils_supported_values_get(
    int unit,
    dnxc_data_property_t * property,
    char *buffer,
    int buffer_size,
    int show_mapping);

shr_error_e dnxc_data_utils_property_val_get(
    int unit,
    dnxc_data_property_t * property,
    char *val_str,
    char *buffer_property_val_str,
    int buffer_size);

shr_error_e dnxc_data_utils_generic_property_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    const dnxc_data_property_t ** property);

shr_error_e dnxc_data_utils_generic_property_val_get(
    int unit,
    char *module,
    char *submodule,
    char *data,
    char *member,
    char *buffer,
    int buffer_size);

#endif
