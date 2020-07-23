
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
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_hash_table_access.h>



shr_error_e
dnx_sw_state_hash_table_print_cb_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, dnx_sw_state_hash_table_print_cb * cb)
{
    if (!strcmp(cb_db->function_name,"dnx_algo_lif_mapping_htb_entry_print"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_lif_mapping_htb_entry_print;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"sw_state_htb_example_entry_print"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = sw_state_htb_example_entry_print;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"sw_state_htb_multithread_print"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = sw_state_htb_multithread_print;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"kbp_ipv4_fwd_tcam_access_mapping_htb_entry_print"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = kbp_ipv4_fwd_tcam_access_mapping_htb_entry_print;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"kbp_ipv6_fwd_tcam_access_mapping_htb_entry_print"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = kbp_ipv6_fwd_tcam_access_mapping_htb_entry_print;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"dnx_algo_lif_local_outlif_valid_combinations_print_cb"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_lif_local_outlif_valid_combinations_print_cb;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"dnx_algo_lif_table_mngr_valid_fields_htb_entry_print_cb"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_lif_table_mngr_valid_fields_htb_entry_print_cb;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"dnx_algo_lif_table_mngr_table_info_htb_entry_print_cb"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_lif_table_mngr_table_info_htb_entry_print_cb;
        }
        return _SHR_E_NONE;
    }
    if (!strcmp(cb_db->function_name,"dnx_algo_lif_mngr_local_outlif_info_htb_entry_print_cb"))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_lif_mngr_local_outlif_info_htb_entry_print_cb;
        }
        return _SHR_E_NONE;
    }

    return _SHR_E_NOT_FOUND;
}



#undef BSL_LOG_MODULE
