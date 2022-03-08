/** \file adapter_cint_data.c 
 * 
 * Expose Adapter internal functions, structures, constans, ...
 * to cint.
 *
 */
 /*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_LIB_CINT)

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

#if defined(BCM_DNX_SUPPORT)
#if defined(ADAPTER_SERVER_MODE)

#include <soc/dnxc/dnxc_adapter_reg_access.h>

CINT_FWRAPPER_CREATE_RP6(int,int, 0, 0,
                         adapter_write_buffer_to_socket,
                         int,int,unit,0,0,
                         int,int,ms_id,0,0,
                         int,int,target,0,0,
                         int,int,len,0,0,
                         int,int,num_of_signals,0,0,
                         uint8*,uint8,buf,1,0);

CINT_FWRAPPER_CREATE_RP5(int,int, 0, 0,
                         adapter_ms_run_unit_test,
                         int,int,unit,0,0,
                         char*,char,ms_name,1,0,
                         uint32,uint32,unit_test_id,0,0,
                         uint32*,uint32,res,1,0,
                         char*,char,message,1,0);

static cint_function_t __cint_adapter_functions[] =
    {
        CINT_FWRAPPER_ENTRY(adapter_write_buffer_to_socket),
        CINT_FWRAPPER_ENTRY(adapter_ms_run_unit_test),

        CINT_ENTRY_LAST
    };

cint_data_t adapter_cint_data =
{
    NULL,
    __cint_adapter_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

#endif /* defined(ADAPTER_SERVER_MODE) */
#endif /* defined(BCM_DNX_SUPPORT) */
#endif /* defined(INCLUDE_LIB_CINT) */
