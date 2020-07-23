/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef FLUSH_INTERNAL_H_INCLUDED
#define FLUSH_INTERNAL_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#define FLUSH_MACHINE_STATUS_IDLE 0


shr_error_e dbal_flush_end(
    int unit);


shr_error_e dbal_flush_start(
    int unit,
    uint8 bulk_commit,
    uint8 hw_accelerated,
    uint8 non_blocking);


shr_error_e dbal_flush_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle);


shr_error_e dbal_flush_rules_set(
    int unit,
    int app_db_id,
    int nof_rules,
    dbal_flush_shadow_info_t * rules_info);


shr_error_e dbal_flush_db_clear(
    int unit);


shr_error_e dbal_flush_in_work(
    int unit,
    int *flush_in_work);


shr_error_e dbal_flush_init(
    int unit);

#endif
