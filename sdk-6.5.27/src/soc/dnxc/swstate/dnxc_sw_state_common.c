
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */



#include <shared/bsl.h>
#include <shared/mem_measure_tool.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_common.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/swstate/dnx_sw_state_journal.h>
#endif



#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

 
void**  sw_state_roots_array[SOC_MAX_NUM_DEVICES];


dnxc_sw_state_layout_t**  sw_state_layout_array[SOC_MAX_NUM_DEVICES];


int dnxc_sw_state_dynamic_allocated_size_get(
    int unit,
    uint32 node_id,
    uint8 *ptr,
    uint32 *allocated_size)
{
    uint32 size = 0;
    uint32 nof_elements_retrieved = 0;
    uint32 element_size_retrieved = 0;

    if(DNXC_SW_STATE_ALLOC_SIZE(unit, node_id, (uint8 *)ptr, &nof_elements_retrieved, &element_size_retrieved) != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META_U(unit, "sw state OUT OF BOUND ERROR: Failed to retrieved size\n")));
        return SOC_E_INTERNAL;
    }
    size = nof_elements_retrieved * element_size_retrieved;
    *allocated_size = size;

    return SOC_E_NONE;
}
