/** \file dnx_cint_kbp.c
 * 
 * Expose KBP  functions, structures, constans, ...  to cint.
 */
 /*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_KBPDNX

/** fix compilation error when DNX is not defined (file is empty) */
typedef int dnx_cint_kbp_not_empty; /* Make ISO compilers happy. */

#if defined(BCM_DNX_SUPPORT)
#if defined(INCLUDE_KBP)
/* { */
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>

#include <soc/dnx/dbal/dbal.h> 
#include <shared/swstate/sw_state_workarounds.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/swstate/auto_generated/types/kbp_types.h>

extern shr_error_e
dnx_kbp_db_counter_read_initiate(
        int unit,
        int *db);

extern shr_error_e
dnx_kbp_entry_get_counter_value(
        int unit,
        int *db,
        int *entry,
        uint64 *value);

extern shr_error_e
dnx_kbp_db_is_counter_read_complete(
        int unit,
        int *db,
        int * is_complete);


/**
 * in macro CINT_FWRAPPER_CREATE_XXX: 
 * the first line represent the return value 
 * the second line represent the API name 
 * each line represents a input parameter.
 */


CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         dnx_kbp_entry_get_counter_value,
                         int, int,unit,0,0,
                         int*,int,acl_db,1,0,
                         int*,int,acl_entry, 1,0,
                         uint64*,uint64, value, 1,0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         dnx_kbp_db_is_counter_read_complete,
                         int, int,unit,0,0,
                         int*,int,acl_db,1,0,
                         int*,int,is_complete, 1,0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         dnx_kbp_db_counter_read_initiate,
                         int, int,unit,0,0,
                         int*,int,acl_db,1,0);


static cint_function_t __cint_dnx_kbp_functions[] =
    {
        CINT_FWRAPPER_NENTRY("dnx_kbp_db_counter_read_initiate", dnx_kbp_db_counter_read_initiate),
        CINT_FWRAPPER_NENTRY("dnx_kbp_db_is_counter_read_complete", dnx_kbp_db_is_counter_read_complete),
        CINT_FWRAPPER_NENTRY("dnx_kbp_entry_get_counter_value", dnx_kbp_entry_get_counter_value),
        CINT_ENTRY_LAST
    };  

cint_data_t dnx_cint_kbp =
{
    NULL,
    __cint_dnx_kbp_functions,
    NULL,
    NULL, 
    NULL, 
    NULL,
    NULL
}; 
/* } */
#endif /* INCLUDE_KBP */
#endif /* BCM_DNX_SUPPORT */

