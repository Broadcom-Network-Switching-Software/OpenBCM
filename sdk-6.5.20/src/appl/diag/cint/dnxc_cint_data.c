/** \file dnxc_cint_data.c 
 * 
 * Expose DNX internal functions, structures, constans, ...  to cint.
 */
 /*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

typedef int dnxc_cint_data_not_empty; /* Make ISO compilers happy. */

#if defined(INCLUDE_LIB_CINT) && (defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT))

#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h> 
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h> 
#include <soc/dnxc/dnxc_intr_handler.h>

CINT_FWRAPPER_CREATE_RP5(const uint32*,uint32, 1, 0,
                         dnxc_data_utils_generic_data_get,
                         int,int,unit,0,0,
                         char *,char,module, 1,0,
                         char *,char,submodule, 1,0,
                         char *,char,data, 1,0,
                         char *,char,member, 1,0);
CINT_FWRAPPER_CREATE_RP6(const uint32*,uint32, 1, 0,
                         dnxc_data_utils_generic_1d_data_get,
                         int,int,unit,0,0,
                         char *,char,module, 1,0,
                         char *,char,submodule, 1,0,
                         char *,char,data, 1,0,
                         char *,char,member, 1,0,
                         int,int,key1, 0,0);
CINT_FWRAPPER_CREATE_RP7(const uint32*,uint32, 1, 0,
                         dnxc_data_utils_generic_2d_data_get,
                         int,int,unit,0,0,
                         char *,char,module, 1,0,
                         char *,char,submodule, 1,0,
                         char *,char,data, 1,0,
                         char *,char,member, 1,0,
                         int,int,key1, 0,0,
                         int,int,key2, 0,0);
CINT_FWRAPPER_CREATE_RP2(int,int, 0, 0,
                         dnxc_int_name_to_id,
                         int,int,unit,0,0,
                         char *,char,name, 1,0);

static cint_function_t __cint_dnxc_functions[] = 
    {
        CINT_FWRAPPER_NENTRY("dnxc_data_get", dnxc_data_utils_generic_data_get),
        CINT_FWRAPPER_NENTRY("dnxc_data_1d_get", dnxc_data_utils_generic_1d_data_get),
        CINT_FWRAPPER_NENTRY("dnxc_data_2d_get", dnxc_data_utils_generic_2d_data_get),
        CINT_FWRAPPER_NENTRY("dnxc_int_name_to_id", dnxc_int_name_to_id),
        CINT_ENTRY_LAST
    };  

cint_data_t dnxc_cint_data = 
{
    NULL,
    __cint_dnxc_functions,
    NULL,
    NULL, 
    NULL, 
    NULL, 
    NULL
}; 

#endif /* INCLUDE_LIB_CINT && (BCM_DNX_SUPPORT || BCM_DNXF_SUPPORT)*/

