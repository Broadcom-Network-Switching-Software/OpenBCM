
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_SYSTEM_SNAPSHOT_MANAGER_H

#define _DNX_SYSTEM_SNAPSHOT_MANAGER_H

#include <soc/types.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>
#include <soc/dnx/dnx_er_db.h>

typedef struct dnx_state_snapshot_manager_d
{
    uint32 is_on_counter;
    uint32 entry_counter;
    uint32 is_suppressed_counter;
} dnx_state_snapshot_manager_t;

shr_error_e dnx_state_snapshot_manager_init(
    int unit,
    dnx_er_db_t * dnx_er_db);

shr_error_e dnx_state_snapshot_manager_deinit(
    int unit,
    dnx_er_db_t * dnx_er_db);

shr_error_e dnx_state_comparison_start(
    int unit);

shr_error_e dnx_state_comparison_end_and_compare(
    int unit);

shr_error_e dnx_state_comparison_suppress(
    int unit,
    uint8 is_suppressed);

uint8 dnx_state_comparison_is_suppressed(
    int unit,
    dnx_er_db_t * dnx_er_db);

shr_error_e dnx_state_comparison_exclude_by_stamp(
    int unit,
    char *stamp,
    int is_swstate);

shr_error_e dnx_state_comparison_print_journal(
    int unit,
    dnx_rollback_journal_subtype_e type);

shr_error_e dnx_state_comparison_journal_logger_state_change(
    int unit,
    dnx_rollback_journal_subtype_e type);

#define DNX_STATE_COMPARISON_SUPPRESS(_unit, _is_suppressed)\
    dnx_state_comparison_suppress(_unit, _is_suppressed)

#define DNX_STATE_COMPARISON_START(_unit)\
    SHR_IF_ERR_EXIT(dnx_state_comparison_start(_unit))

#define DNX_STATE_COMPARISON_END_AND_COMPARE(_unit)\
    SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(_unit))

#define DNX_STATE_COMPARISON_END_AND_COMPARE_NEGATIVE(_unit)\
do {                                                                                                              \
    if (_SHR_E_NONE == dnx_state_comparison_end_and_compare(_unit))                                               \
    {                                                                                                             \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected system state difference for negative test, but none was found\n");\
    }                                                                                                             \
} while(0)

#endif
