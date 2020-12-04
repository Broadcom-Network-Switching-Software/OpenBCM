
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef TCAM_ACCESS_SHADOW_H_INCLUDED

#define TCAM_ACCESS_SHADOW_H_INCLUDED

#include <include/soc/dnx/dbal/dbal_structures.h>

shr_error_e dnx_field_tcam_access_entry_key_read_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 tcam_bank_offset,
    uint32 *data,
    uint8 *valid_bits);

shr_error_e dnx_field_tcam_access_entry_key_write_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 tcam_bank_offset,
    int valid,
    dnx_field_tcam_access_part_e part,
    uint32 *data);

shr_error_e dnx_field_tcam_access_entry_payload_read_shadow(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 action_bank_offset,
    uint32 *payload);

shr_error_e dnx_field_tcam_access_entry_payload_write_shadow(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 action_bank_offset,
    uint32 *payload);

shr_error_e dnx_field_tcam_access_entry_decoded_read_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *key,
    uint32 *key_mask);

shr_error_e dnx_field_tcam_access_entry_equal_shadow(
    int unit,
    int core,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 bank_id,
    uint32 bank_offset,
    uint8 skip_entry_in_use,
    uint8 *equal);

#endif
