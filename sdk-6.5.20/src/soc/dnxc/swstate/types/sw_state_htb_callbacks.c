
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/dnxc/swstate/sw_state_features.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/swstate/auto_generated/types/algo_lif_types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/lif_table_mngr_types.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#endif


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

#include <soc/dnxc/swstate/callbacks/sw_state_ll_callbacks.h>
#include <sal/core/libc.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/multithread_analyzer.h>
#ifdef DNXC_MTA_ENABLED
#include <soc/dnxc/swstate/auto_generated/types/multithread_analyzer_types.h>
#endif

int
dnx_algo_lif_mapping_htb_entry_print(
    int unit,
    void *key,
    void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[ 0x%x , 0x%x ] ", *mykey, *mydata);

    return 0;
}

int
sw_state_htb_example_entry_print(
    int unit,
    void *key,
    void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[%d,%d] ", *mykey, *mydata);

    return 0;
}

int
sw_state_htb_multithread_print(
    int unit,
    void *key,
    void *data)
{

#ifdef DNXC_MTA_ENABLED
    thread_access_log_entry_t *mykey;

    mykey = (thread_access_log_entry_t *) key;

    DNX_SW_STATE_PRINT(unit, "[%s,%s,%s,%s,%s,%s] ",
                    dnxc_multithread_analyzer_thread_name_get(unit, mykey->thread_id),
                    dnxc_multithread_analyzer_resource_type_name_get(mykey->resource_type),
                    dnxc_multithread_analyzer_resource_id_name_get(unit, mykey->resource_type, mykey->resource_id),
                    mykey->is_write ? "write" : "read", mykey->function_name,
                    dnxc_multithread_analyzer_mutex_list_get(mykey->mutex_name));
#else
    DNX_SW_STATE_PRINT(unit, "should never be called\n");
#endif


    return 0;
}

int
kbp_ipv4_fwd_tcam_access_mapping_htb_entry_print(
    int unit,
    void *key,
    void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[0x%x,0x%x] ", *mykey, *mydata);

    return 0;
}

int
kbp_ipv6_fwd_tcam_access_mapping_htb_entry_print(
    int unit,
    void *key,
    void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[0x%x,0x%x] ", *mykey, *mydata);

    return 0;
}

#ifdef BCM_DNX_SUPPORT
int
dnx_algo_lif_local_outlif_valid_combinations_print_cb(
    int unit,
    void *key,
    void *data)
{
    dnx_local_outlif_ll_indication_e *mydata;
    dbal_to_phase_info_t *mykey;

    mykey = (dbal_to_phase_info_t *) key;
    mydata = (dnx_local_outlif_ll_indication_e *) data;

    DNX_SW_STATE_PRINT(unit, "[%s,%s, LL existance: %s] ", dbal_logical_table_to_string(unit, mykey->dbal_table), dbal_result_type_to_string(unit, mykey->dbal_table, mykey->dbal_result_type), (*mydata == DNX_LOCAL_OUTLIF_LL_ALWAYS) ? "Always" : (*mydata == DNX_LOCAL_OUTLIF_LL_OPTIONAL) ? "Optional" : "Never");

    return 0;
}

int
dnx_algo_lif_table_mngr_valid_fields_htb_entry_print_cb(
    int unit,
    void *key,
    void *data)
{
    int *local_lif = (int *)key;
    local_lif_info_t *local_lif_info = (local_lif_info_t *)data;

    DNX_SW_STATE_PRINT(unit, "Local LIF 0x%x: [0x%x] ", *local_lif, local_lif_info->valid_fields);
    return 0;
}

int
dnx_algo_lif_table_mngr_table_info_htb_entry_print_cb(
    int unit,
    void *key,
    void *data)
{
    int *dbal_table_id = (int *)key;
    lif_table_mngr_table_info_t *sw_state_table_info = (lif_table_mngr_table_info_t *)data;
    uint32 result_type_idx, max_result_types, result_type;

    DNX_SW_STATE_PRINT(unit, "Forbidden fields for DBAL table %s: [0x%x,0x%x]", dbal_logical_table_to_string(unit, *dbal_table_id),
                       sw_state_table_info->forbidden_fields_lo, sw_state_table_info->forbidden_fields_hi);

    DNX_SW_STATE_PRINT(unit, "Sorted result types for DBAL table %s:", dbal_logical_table_to_string(unit, *dbal_table_id));

    max_result_types = sizeof(sw_state_table_info->result_type)/sizeof(sw_state_table_info->result_type[0]);
    for (result_type_idx = 0; result_type_idx < max_result_types; result_type_idx++)
    {
        result_type = sw_state_table_info->result_type[result_type_idx];
        if (result_type != LIF_TABLE_MANAGER_INVALID_RESULT_TYPE)
        {
            DNX_SW_STATE_PRINT(unit, " #%d: %s", result_type_idx, dbal_result_type_to_string(unit, *dbal_table_id, result_type));
        }
        else
        {
            break;
        }
    }

    return 0;
}

int
dnx_algo_lif_mngr_local_outlif_info_htb_entry_print_cb(
    int unit,
    void *key,
    void *data)
{
    int *local_lif = (int *)key;
    egress_lif_info_t *sw_state_outlif_info = (egress_lif_info_t *)data;

    DNX_SW_STATE_PRINT(unit, "Local LIF 0x%x: DBAL Table [%s], Result Type [%s], ll_exists [%d], stap_pp_count_profile [%d], logical phase [%d]", *local_lif,
                       dbal_logical_table_to_string(unit, sw_state_outlif_info->dbal_table_id),
                       dbal_result_type_to_string(unit, sw_state_outlif_info->dbal_table_id, sw_state_outlif_info->dbal_result_type),
                       sw_state_outlif_info->ll_exists, sw_state_outlif_info->stat_pp_etpp_is_count_profile,
                       sw_state_outlif_info->logical_phase);


    return 0;
}

#else

int
dnx_algo_lif_local_outlif_valid_combinations_print_cb(
         int unit,
         void *key,
         void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[0x%x,0x%x] ", *mykey, *mydata);

    return 0;
}

int
dnx_algo_lif_table_mngr_valid_fields_htb_entry_print_cb(
         int unit,
         void *key,
         void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[0x%x,0x%x] ", *mykey, *mydata);

    return 0;
}

int
dnx_algo_lif_table_mngr_table_info_htb_entry_print_cb(
         int unit,
         void *key,
         void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[0x%x,0x%x] ", *mykey, *mydata);

    return 0;
}

int
dnx_algo_lif_mngr_local_outlif_info_htb_entry_print_cb(
         int unit,
         void *key,
         void *data)
{
    int *mydata;
    int *mykey;

    mydata = (int *) data;
    mykey = (int *) key;

    DNX_SW_STATE_PRINT(unit, "[0x%x,0x%x] ", *mykey, *mydata);

    return 0;
}

#endif

#undef _ERR_MSG_MODULE_NAME


