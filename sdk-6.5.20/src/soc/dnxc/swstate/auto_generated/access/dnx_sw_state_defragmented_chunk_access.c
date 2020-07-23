
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_defragmented_chunk_access.h>
#include <soc/dnxc/swstate/types/sw_state_defragmented_chunk.h>



shr_error_e
sw_state_defragmented_cunk_move_cb_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, sw_state_defragmented_cunk_move_cb * cb)
{
    if (!strcmp(cb_db->function_name,"dnx_algo_consistent_hashing_manager_profile_move"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_consistent_hashing_manager_profile_move;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"sw_state_defragmented_chunk_move_test"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = sw_state_defragmented_chunk_move_test;
        }
        return _SHR_E_NONE;
    }

    return _SHR_E_NOT_FOUND;
}



#endif  
#undef BSL_LOG_MODULE
