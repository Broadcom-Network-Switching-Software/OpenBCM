/*
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
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX


uint32 dnx_sw_state_info_size_get(dnx_sw_state_diagnostic_info_t *info, const char *layout_str[], uint32 nof_entries, char *node, uint32 static_element_size) {
    uint32 size = 0;
    int i;
    for (i = 0; i < nof_entries; i++) {
        if (is_prefix(node, layout_str[i])) {
            size += info[i].size;
        }
    }
    
    size +=static_element_size;
    return size;
}

void dnx_sw_state_size_print(uint32 size) {
    if (size < 10 * 1024) {
        cli_out("%u bytes\n", size);
    } else if (size < 10 * 1024 * 1024) {
        cli_out("%u KB\n", size / 1024);
    } else {
        cli_out("%u MB\n", size / (1024 * 1024));
    }
}

#else 
typedef int make_iso_compilers_happy;
#endif
