
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_COMMON_H

#define _DNXC_SW_STATE_COMMON_H

#include <soc/types.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/scache.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/recovery/rollback_journal_utils.h>
#endif

int dnxc_sw_state_memcpy_common(
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *dest,
    uint8 *src,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_memset_common(
    int unit,
    uint32 module_id,
    uint8 *dest,
    uint32 value,
    uint32 size,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_counter_inc_common(
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 inc_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_counter_dec_common(
    int unit,
    uint32 module_id,
    uint8 *ptr_location,
    uint32 dec_value,
    uint32 type_size,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_dynamic_allocated_size_get(
    int unit,
    uint32 module_id,
    uint8 *ptr,
    uint32 *allocated_size);

#endif
