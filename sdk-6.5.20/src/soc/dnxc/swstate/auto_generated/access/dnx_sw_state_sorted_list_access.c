
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_sorted_list_access.h>



shr_error_e
dnx_sw_state_sorted_list_cb_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, dnx_sw_state_sorted_list_cb * cb)
{
    if (!strcmp(cb_db->function_name,"tcam_sorted_list_cmp_priority"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = tcam_sorted_list_cmp_priority;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"sw_state_sorted_list_cmp"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = sw_state_sorted_list_cmp;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"sw_state_sorted_list_cmp32"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = sw_state_sorted_list_cmp32;
        }
        return _SHR_E_NONE;
    }

    return _SHR_E_NOT_FOUND;
}



#undef BSL_LOG_MODULE
