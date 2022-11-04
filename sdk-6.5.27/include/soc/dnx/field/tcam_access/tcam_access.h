
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef TCAM_ACCESS_H_INCLUDED

#define TCAM_ACCESS_H_INCLUDED

#include <include/soc/dnx/dbal/dbal_structures.h>

shr_error_e dnx_field_tcam_access_entry_add(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

shr_error_e dnx_field_tcam_access_tcam_hit_per_core_get(
    int unit,
    int core_id,
    uint32 position,
    uint8 *entry_hit_p);

shr_error_e dnx_field_tcam_access_tcam_hit_indication_reset_all(
    int unit);

shr_error_e dnx_field_tcam_access_entry_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

shr_error_e dnx_field_tcam_access_entry_delete(
    int unit,
    dbal_physical_tables_e physical_table,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags);

shr_error_e dnx_field_tcam_access_entry_delete_aux(
    int unit,
    int core,
    uint32 app_id,
    uint32 position,
    uint8 is_move_operation);

shr_error_e dnx_field_tcam_access_key_to_access_id(
    int unit,
    int core,
    uint32 app_id,
    uint32 *key,
    uint32 *key_mask,
    uint32 *access_id);

shr_error_e dnx_field_tcam_access_move(
    int unit,
    int core,
    uint32 app_id,
    uint32 position_from,
    uint32 position_to,
    uint8 remove_old_from_hw);

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

shr_error_e dnx_field_tcam_access_move_line(
    int unit,
    int core,
    uint32 app_id,
    uint32 line_from,
    uint32 line_to);

shr_error_e dnx_field_tcam_access_line_delete(
    int unit,
    int core,
    uint32 app_id,
    uint32 line);

shr_error_e dnx_field_tcam_access_access_id_allocate(
    int unit,
    int *access_id);

shr_error_e dnx_field_tcam_access_access_id_free(
    int unit,
    int access_id);

shr_error_e dnx_field_tcam_access_entry_key_write(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *key,
    uint32 *key_mask,
    uint32 key_mode,
    uint32 part,
    uint8 valid);

#endif
