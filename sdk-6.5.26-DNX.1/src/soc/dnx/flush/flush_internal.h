/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef FLUSH_INTERNAL_H_INCLUDED
#define FLUSH_INTERNAL_H_INCLUDED

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/l2/l2.h>

#define FLUSH_MACHINE_STATUS_IDLE 0

#define MDB_LEM_ITER(unit,lem_id)    \
    for ((lem_id) = 0; (lem_id) < dnx_data_l2.general.lem_nof_dbs_get(unit); (lem_id)++)

void dbal_flush_reset_flush_work_buffers(
    int unit,
    dnx_l2_action_type_t l2_action_type);

shr_error_e dbal_flush_end(
    int unit,
    dnx_l2_action_type_t l2_action_type);

shr_error_e dbal_flush_start(
    int unit,
    uint8 bulk_commit,
    uint8 hw_accelerated,
    uint8 non_blocking,
    dnx_l2_action_type_t l2_action_type);

shr_error_e dbal_flush_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);

shr_error_e dbal_flush_buffer_init(
    int unit);

void dbal_flush_buffer_deinit(
    int unit);

shr_error_e dbal_flush_rules_set(
    int unit,
    int app_db_id,
    dbal_tables_e table_id,
    int nof_rules,
    dbal_flush_shadow_info_t * rules_info);

shr_error_e dbal_flush_db_clear(
    int unit,
    dnx_l2_action_type_t l2_action_type);

shr_error_e dbal_flush_in_work(
    int unit,
    dnx_l2_action_type_t l2_action_type,
    int *flush_in_work);

shr_error_e dbal_flush_init(
    int unit);

#endif
