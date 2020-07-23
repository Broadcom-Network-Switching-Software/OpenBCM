
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef TCAM_ACCESS_H_INCLUDED

#define TCAM_ACCESS_H_INCLUDED

#include <include/soc/dnx/dbal/dbal_structures.h>

shr_error_e dnx_field_tcam_access_entry_add(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

shr_error_e dnx_field_tcam_access_entry_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

shr_error_e dnx_field_tcam_access_entry_delete(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

shr_error_e dnx_field_tcam_access_key_to_access_id(
    int unit,
    int core,
    uint32 app_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 *access_id);

shr_error_e dnx_field_tcam_access_move(
    int unit,
    int core,
    uint32 app_id,
    uint32 position_from,
    uint32 position_to);

shr_error_e dnx_field_tcam_access_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

shr_error_e dnx_field_tcam_access_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end);

shr_error_e dnx_field_tcam_access_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator);

shr_error_e dnx_ser_tcam_access_entry_key_dump(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 mode);

shr_error_e dnx_ser_tcam_access_rewrite_key_from_shadow(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 bank_offset);

shr_error_e dnx_ser_tcam_protect_machine_enable(
    int unit,
    int enable);

shr_error_e dnx_ser_tcam_access_write_hw_entry(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 valid,
    uint32 key_mode,
    uint32 *key,
    uint32 *mask);

#endif
