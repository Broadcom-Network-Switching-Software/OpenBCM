
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_hash_table_access.h>



shr_error_e
dnx_sw_state_hash_table_print_cb_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, dnx_sw_state_hash_table_print_cb * cb)
{
    if (!sal_strncmp(cb_db->function_name,"dnx_algo_lif_mapping_htb_entry_print", 36))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_lif_mapping_htb_entry_print;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"sw_state_htb_example_entry_print", 32))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = sw_state_htb_example_entry_print;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"sw_state_htb_multithread_print", 30))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = sw_state_htb_multithread_print;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"kbp_ipv4_fwd_tcam_access_mapping_htb_entry_print", 48))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = kbp_ipv4_fwd_tcam_access_mapping_htb_entry_print;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"kbp_ipv6_fwd_tcam_access_mapping_htb_entry_print", 48))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = kbp_ipv6_fwd_tcam_access_mapping_htb_entry_print;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"dnx_algo_lif_local_outlif_valid_combinations_print_cb", 53))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_lif_local_outlif_valid_combinations_print_cb;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"dnx_algo_lif_table_mngr_valid_fields_htb_entry_print_cb", 55))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_lif_table_mngr_valid_fields_htb_entry_print_cb;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"dnx_algo_lif_table_mngr_table_info_htb_entry_print_cb", 53))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_lif_table_mngr_table_info_htb_entry_print_cb;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"dnx_algo_lif_mngr_local_outlif_info_htb_entry_print_cb", 54))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_lif_mngr_local_outlif_info_htb_entry_print_cb;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"dnx_algo_flow_dbal_table_to_valid_result_types_print_cb", 55))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_algo_flow_dbal_table_to_valid_result_types_print_cb;
        }
        return _SHR_E_NONE;
    }
    if (!sal_strncmp(cb_db->function_name,"dnx_mcdb_hash_table_entry_print_cb", 34))
    {
        if(!dryRun && cb != NULL)
        {
            *cb = dnx_mcdb_hash_table_entry_print_cb;
        }
        return _SHR_E_NONE;
    }

    return _SHR_E_NOT_FOUND;
}



#undef BSL_LOG_MODULE
