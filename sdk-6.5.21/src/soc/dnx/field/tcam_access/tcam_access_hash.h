
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef TCAM_ACCESS_HASH_H_INCLUDED

#define TCAM_ACCESS_HASH_H_INCLUDED

#include <include/soc/dnx/dbal/dbal_structures.h>

shr_error_e dnx_field_tcam_access_hash_key_insert_at_index(
    int unit,
    int core,
    uint32 bank_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 index);

shr_error_e dnx_field_tcam_access_hash_key_find(
    int unit,
    int core,
    uint32 bank_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 *index,
    uint8 *found);

shr_error_e dnx_field_tcam_access_hash_delete(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 key_size);

#endif
