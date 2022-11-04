
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_EXTERNAL_H
#define _DNXC_SW_STATE_EXTERNAL_H

#include <shared/bsl.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>

typedef enum
{
    DNXC_SW_STATE_EXTERNAL_INVALID = -1,
    DNXC_SW_STATE_EXTERNAL_KAPS_MODULE_ID,
    DNXC_SW_STATE_EXTERNAL_KAPS_CTEST_MODULE_ID,
    DNXC_SW_STATE_EXTERNAL_XLACL_MODULE_ID,
    DNXC_SW_STATE_EXTERNAL_COUNT
} dnxc_sw_state_external_modules_e;

int dnxc_sw_state_external_alloc(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 size,
    char *dbg_string,
    uint32 id);

int dnxc_sw_state_external_memwrite(
    int unit,
    uint8 *ptr_location,
    uint8 *src,
    uint32 size);

int dnxc_sw_state_external_memget(
    int unit,
    uint32 module_id,
    uint32 id,
    uint8 **ptr_location,
    uint32 *size);

#endif
