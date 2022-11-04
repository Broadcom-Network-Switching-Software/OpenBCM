
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */




#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_external.h>
#include <soc/dnxc/swstate/auto_generated/access/sw_state_external_access.h>
#include <soc/dnxc/swstate/auto_generated/layout/sw_state_external_layout.h>
#include <soc/dnxc/swstate/auto_generated/diagnostic/sw_state_external_diagnostic.h>



#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL


static int _dnxc_sw_state_external_verify(
    int unit,
    uint32 module_id,
    uint32 id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if(module_id > DNXC_SW_STATE_EXTERNAL_COUNT)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Module Id(%d) is not suported for external alloc.", module_id);
    }

    if(id > DNXC_SW_STATE_EXTERNAL_OFFSETS_PER_MODULE)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "The id(%d) of the externally allocated pointer is bigger than expected(DNXC_SW_STATE_EXTERNAL_OFFSETS_PER_MODULE).", module_id);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


static int _dnxc_sw_state_external_offset_set(
    int unit,
    uint32 module_id,
    uint32 id,
    uint32 offset)
{

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_INVOKE_VERIFY_DNXC(_dnxc_sw_state_external_verify(unit, module_id, id));

    SHR_IF_ERR_EXIT(sw_state_external.offsets.set(unit, module_id, id, offset));

    DNXC_SW_STATE_FUNC_RETURN;
}


static int _dnxc_sw_state_external_offset_get(
    int unit,
    uint32 module_id,
    uint32 id,
    uint32 *offset)
{

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_INVOKE_VERIFY_DNXC(_dnxc_sw_state_external_verify(unit, module_id, id));

    SHR_IF_ERR_EXIT(sw_state_external.offsets.get(unit, module_id, id, offset));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_external_alloc(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 size,
    char *dbg_string,
    uint32 id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_INVOKE_VERIFY_DNXC(_dnxc_sw_state_external_verify(unit, module_id, id));

    
    DNX_SW_STATE_DIAG_INFO_PRE(unit, DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS, DNX_SW_STATE_DIAG_ALLOC);
    
    DNX_SW_STATE_ALLOC_BASIC(unit, DNXC_SW_STATE_LAYOUT_NODE_ID_GET(SW_STATE_EXTERNAL_MODULE_ID, id), 
                                ptr_location, 1 , size, DNXC_SW_STATE_EXTERNAL_ALLOC_LOCATION, dbg_string);
    
    
    DNX_SW_STATE_DIAG_INFO_POST(unit, DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS, sw_state_external_info,
                                    SW_STATE_EXTERNAL_OFFSETS_INFO, DNX_SW_STATE_DIAG_ALLOC, NULL, sw_state_layout_array[unit][SW_STATE_EXTERNAL_MODULE_ID]);
    
    SHR_IF_ERR_EXIT(_dnxc_sw_state_external_offset_set(unit, module_id, id, 
                                                        dnxc_sw_state_dispatcher[unit][SW_STATE_EXTERNAL_MODULE_ID].calc_offset(unit, *ptr_location)));
    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_external_memwrite(
    int unit,
    uint8 *ptr_location,
    uint8 *src,
    uint32 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    DNX_SW_STATE_MEMWRITE_BASIC(unit, DNXC_SW_STATE_LAYOUT_NODE_ID_GET(SW_STATE_EXTERNAL_MODULE_ID, 0), src, 0 , ptr_location, 0 , size, DNXC_SW_STATE_NO_FLAGS, "External Memwrite");

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_external_memget(
    int unit,
    uint32 module_id,
    uint32 id,
    uint8 **ptr_location,
    uint32 *size)
{
    uint32 offset = 0;
    uint32 nof_elements, element_size;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    *ptr_location = NULL;
    if (size != NULL)
    {
        *size = 0;
    }

    
    SHR_IF_ERR_EXIT(_dnxc_sw_state_external_offset_get(unit, module_id, id, &offset));

    
    if (offset == 0)
    {
        SHR_EXIT();
    }

    
    *ptr_location = dnxc_sw_state_dispatcher[unit][SW_STATE_EXTERNAL_MODULE_ID].calc_pointer(unit, offset);

    if (size != NULL)
    {
        
        SHR_IF_ERR_EXIT(dnxc_sw_state_dispatcher[unit][module_id].alloc_size(unit, DNXC_SW_STATE_LAYOUT_NODE_ID_GET(SW_STATE_EXTERNAL_MODULE_ID, 0), *ptr_location, &nof_elements, &element_size));
        *size = nof_elements * element_size;
    }

    DNXC_SW_STATE_FUNC_RETURN;
}
