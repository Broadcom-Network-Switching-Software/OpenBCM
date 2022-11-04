
/*
 *                                                            $
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. $
 */

#ifndef DNXC_ADAPTER_REGMEM_ACCESS_H_INCLUDED

#define DNXC_ADAPTER_REGMEM_ACCESS_H_INCLUDED

#ifdef ADAPTER_SERVER_MODE
#ifdef BCM_ACCESS_SUPPORT

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/access/auto_generated/common_enum.h>

shr_error_e dnxc_adapter_local_regmem_access_handle(
    access_runtime_info_t * runtime_info,
    uint32 flags,
    access_local_regmem_id_t regmem,
    access_block_instance_num_t inst,
    uint16 array_index,
    uint32 mem_index,
    int nof_addresses,
    uint32 *entry_data);

shr_error_e dnxc_adapter_regmem_direct_access_handle(
    const int unit,
    uint32 flags,
    access_regmem_sbus_info_t sbus_access_info,
    uint32 *entry_data);
#endif
#endif

#endif
