
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_KEY_TYPES_H__
#define __DNX_FIELD_KEY_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>


typedef struct {
    dnx_field_qual_t qual_type;
    uint16 lsb;
    uint8 size;
} dnx_field_qual_map_in_key_t;

typedef struct {
    dnx_field_qual_map_in_key_t key_qual_map[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint16 key_size_in_bits;
} dnx_field_key_template_t;

typedef struct {
    dbal_enum_value_field_field_key_e id[DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX];
    uint8 offset_on_first_key;
    uint8 nof_bits[DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX];
} dnx_field_key_id_t;


#endif 
