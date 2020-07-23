
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/dnxc/swstate/sw_state_features.h>



#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <shared/utilex/utilex_framework.h>
#include <sal/core/libc.h>
#include <assert.h>

#define DNX_TCAM_DB_LIST_KEY_SIZE                               (sizeof(uint32))
#define DNX_TCAM_DB_LIST_DATA_SIZE                              (sizeof(DNX_TCAM_PRIO_LOCATION))


static shr_error_e
dnx_tcam_prio_location_clear(
    DNX_TCAM_PRIO_LOCATION * info)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);

    SHR_NULL_CHECK(info, _SHR_E_INTERNAL, "info");
    sal_memset(info, 0x0, sizeof(*info));
exit:
    SHR_FUNC_EXIT;
}


int32
tcam_sorted_list_cmp_priority(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size)
{
    uint32 prio_1, prio_2;
    DNX_TCAM_PRIO_LOCATION prio_location_1, prio_location_2;

    if ((buffer1 == NULL) || (buffer2 == NULL))
    {
        
        assert(0);
    }
    
    if (size == DNX_TCAM_DB_LIST_KEY_SIZE)
    {
        prio_1 = *((uint32*)buffer1);
        prio_2 = *((uint32*)buffer2);
        return (((int32) prio_1) - ((int32) prio_2));
    }
    else if (size == DNX_TCAM_DB_LIST_DATA_SIZE)
    {   
        dnx_tcam_prio_location_clear(&prio_location_1);
        dnx_tcam_prio_location_clear(&prio_location_2);
        
        
        sal_memcpy(&prio_location_1, buffer1, sizeof(DNX_TCAM_PRIO_LOCATION));
        
        sal_memcpy(&prio_location_2, buffer2, sizeof(DNX_TCAM_PRIO_LOCATION));
        return ((prio_location_1.entry_id_first !=
                 prio_location_2.entry_id_first)
                || (prio_location_1.entry_id_last != prio_location_2.entry_id_last)) ? 1 : 0;
    }
    else
    {
        
        assert(0);
        return 0;
    }
}

int32
sw_state_sorted_list_cmp(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size)
{
    
    int32 ret_val = 0; 
    uint8 value1 = *buffer1;
    uint8 value2 = *buffer2;
    ret_val += value1;
    ret_val -= value2;
    return ret_val;
}

int32
sw_state_sorted_list_cmp32(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size)
{
    
    int32 ret_val = 0;
    uint32 value1 = *((uint32 *)buffer1);
    uint32 value2 = *((uint32 *)buffer2);
    ret_val += value1;
    ret_val -= value2;
    return ret_val;
}

#undef _ERR_MSG_MODULE_NAME


