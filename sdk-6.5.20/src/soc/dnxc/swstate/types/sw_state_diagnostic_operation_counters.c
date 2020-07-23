/** \file sw_state_diagnostic_operation_counters.c
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: this module implement a size functionality for the SW State infrastructure layer,
 */


#include <soc/dnxc/swstate/sw_state_features.h>


#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX


void dnx_sw_state_diagnostic_operation_counters_print(
    dnx_sw_state_diagnostic_info_t * info,
    const char *layout_str[],
    uint32 nof_entries,
    char *node) {
 
        int i;
        dnx_sw_state_diagnostic_info_t diag_info;
        sal_memset(&diag_info, 0, sizeof(dnx_sw_state_diagnostic_info_t));
        

        for (i = 0; i < nof_entries; i++) {
            if (is_prefix(node, layout_str[i])) {
                diag_info.modify_operations_counter               += info[i].modify_operations_counter;
                diag_info.read_operations_counter                 += info[i].read_operations_counter;
                diag_info.bitmap_operations_counter               += info[i].bitmap_operations_counter;
                diag_info.pbmp_operations_counter                 += info[i].pbmp_operations_counter;
                diag_info.mutex_operations_counter                += info[i].mutex_operations_counter;
                diag_info.occupation_bitmap_operations_counter    += info[i].occupation_bitmap_operations_counter;
                diag_info.hash_table_operations_counter           += info[i].hash_table_operations_counter;
                diag_info.linked_list_operations_counter          += info[i].linked_list_operations_counter;
                diag_info.cb_db_operations_counter                += info[i].cb_db_operations_counter;
            }
        }
        if (diag_info.modify_operations_counter > 0)             cli_out("modify_operations_counter:     %u\n",diag_info.modify_operations_counter);
        if (diag_info.read_operations_counter > 0)               cli_out("read_operations_counter:       %u\n",diag_info.read_operations_counter);
        if (diag_info.bitmap_operations_counter > 0)             cli_out("bitmap_operations_counter:     %u\n",diag_info.bitmap_operations_counter);
        if (diag_info.pbmp_operations_counter > 0)               cli_out("pbmp_operations_counter:       %u\n",diag_info.pbmp_operations_counter);
        if (diag_info.mutex_operations_counter > 0)              cli_out("mutex_operations_counter:      %u\n",diag_info.mutex_operations_counter);
        if (diag_info.occupation_bitmap_operations_counter > 0)  cli_out("occupation_bitmap_operations   %u\n",diag_info.occupation_bitmap_operations_counter);
        if (diag_info.hash_table_operations_counter > 0)         cli_out("hash_table_operations_counter: %u\n",diag_info.hash_table_operations_counter);
        if (diag_info.linked_list_operations_counter > 0)        cli_out("linked_list_operations_counter:%u\n",diag_info.linked_list_operations_counter);
        if (diag_info.cb_db_operations_counter > 0)              cli_out("cb_db_operations_counter:%u\n",diag_info.cb_db_operations_counter);
    }
#else 
typedef int make_iso_compilers_happy;
#endif
