/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * INFO: this module implement a size functionality for the SW State infrastructure layer,
 *
 */


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
    if (size < 1 * 1024) {
       cli_out("%d bytes (%d bytes)\n", size, size);
    } else if (size < 1 * 1024 * 1024) {
        cli_out("%d.%03d KB. (%d bytes) \n", BYTES_TO_KBS(size), BYTES_TO_KBS_CARRY(size), size);
    } else {
        cli_out("%d.%06d MB. (%d bytes)\n", BYTES_TO_MBS(size), BYTES_TO_MBS_CARRY(size), size);
    }
}

#else 
typedef int make_iso_compilers_happy;
#endif
