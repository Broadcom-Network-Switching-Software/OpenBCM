
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_sorted_list_access.h>



shr_error_e
dnx_sw_state_sorted_list_cb_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, dnx_sw_state_sorted_list_cb * cb)
{
    if (!sal_strncmp(cb_db->function_name,"sw_state_sorted_list_cmp_uint64", 31))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = sw_state_sorted_list_cmp_uint64;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"sw_state_sorted_list_cmp_uint32", 31))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = sw_state_sorted_list_cmp_uint32;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"sw_state_sorted_list_cmp_uint8", 30))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = sw_state_sorted_list_cmp_uint8;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"tcam_sorted_list_cmp_priority", 29))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = tcam_sorted_list_cmp_priority;
        }
        return _SHR_E_NONE;
    }

    return _SHR_E_NOT_FOUND;
}


#undef BSL_LOG_MODULE
