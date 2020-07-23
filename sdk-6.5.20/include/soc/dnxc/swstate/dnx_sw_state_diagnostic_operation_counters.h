/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_SW_STATE_DIAGNOSTIC_OPERATION_COUNTERS_H
#define _DNX_SW_STATE_DIAGNOSTIC_OPERATION_COUNTERS_H

#include <soc/dnxc/swstate/sw_state_defs.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <bcm/types.h>
#include <soc/types.h>
#include <soc/dnxc/swstate/sw_state_diagnostics.h>
void dnx_sw_state_diagnostic_operation_counters_print(
    dnx_sw_state_diagnostic_info_t * info,
    const char *layout_str[],
    uint32 nof_entries,
    char *node);

#endif
#endif
