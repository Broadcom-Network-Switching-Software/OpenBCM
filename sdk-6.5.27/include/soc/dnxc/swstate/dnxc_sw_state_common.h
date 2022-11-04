
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_COMMON_H

#define _DNXC_SW_STATE_COMMON_H

#include <soc/types.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/scache.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/recovery/rollback_journal_utils.h>
#endif

int dnxc_sw_state_dynamic_allocated_size_get(
    int unit,
    uint32 node_id,
    uint8 *ptr,
    uint32 *allocated_size);

#endif
