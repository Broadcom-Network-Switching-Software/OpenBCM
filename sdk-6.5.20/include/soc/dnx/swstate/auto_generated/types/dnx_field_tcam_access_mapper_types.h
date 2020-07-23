
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_ACCESS_MAPPER_TYPES_H__
#define __DNX_FIELD_TCAM_ACCESS_MAPPER_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>


typedef struct {
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
} dnx_field_tcam_access_mapper_hash_key;


typedef struct {
    
    sw_state_htbl_t** key_2_entry_id_hash;
} dnx_field_tcam_access_mapper_t;


#endif 
