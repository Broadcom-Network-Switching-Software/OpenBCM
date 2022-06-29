
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */



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


int32
tcam_sorted_list_cmp_priority(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size)
{
    uint32 prio_1, prio_2;
    uint8 handler_1, handler_2;

    if ((buffer1 == NULL) || (buffer2 == NULL))
    {
        
        assert(0);
    }
    
    prio_1 = *((uint32*)buffer1);
    
    handler_1 = *(buffer1+sizeof(uint32));
    prio_2 = *((uint32*)buffer2);
    handler_2 = *(buffer2+sizeof(uint32));
    return (handler_1 == handler_2)? (prio_1 - prio_2) : (handler_1 - handler_2);
}

int32
sw_state_sorted_list_cmp_uint8(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size)
{
    
    uint8 value1 = *buffer1;
    uint8 value2 = *buffer2;
    return value1 - value2;
}

int32
sw_state_sorted_list_cmp_uint32(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size)
{
    
    uint32 value1 = *((uint32 *)buffer1);
    uint32 value2 = *((uint32 *)buffer2);
    return value1 - value2;
}


int32
sw_state_sorted_list_cmp_uint64(
    uint8 *buffer1,
    uint8 *buffer2,
    uint32 size)
{
    
    uint64 value1 = *((uint64 *)buffer1);
    uint64 value2 = *((uint64 *)buffer2);

    return ( COMPILER_64_EQ(value1, value2) ) ? 0 : ( (COMPILER_64_GT(value1, value2) ) ? 1 : (-1));
}

#undef _ERR_MSG_MODULE_NAME


