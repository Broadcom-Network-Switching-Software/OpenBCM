
/*
 *                                                            $
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved. $
 */

#ifndef DNXC_ADAPTER_REGMEM_ACCESS_H_INCLUDED

#define DNXC_ADAPTER_REGMEM_ACCESS_H_INCLUDED

#ifdef ADAPTER_SERVER_MODE
#ifdef BCM_ACCESS_SUPPORT

#include <shared/shrextend/shrextend_debug.h>
#include <soc/access/auto_generated/common_enum.h>

shr_error_e dnxc_adapter_regmem_access_handle(
    const int unit,
    uint32 flags,
    access_regmem_id_t regmem,
    access_block_instance_num_t inst,
    uint16 array_index,
    uint32 mem_index,
    uint32 *entry_data);

#endif
#endif

#endif
