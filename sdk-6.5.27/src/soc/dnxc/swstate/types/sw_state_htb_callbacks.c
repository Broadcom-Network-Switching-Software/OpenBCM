
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/multicast_types.h>
#include <soc/dnx/swstate/auto_generated/types/algo_lif_types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/lif_table_mngr_types.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/multicast/multicast.h>
#include <soc/dnx/swstate/auto_generated/types/flow_types.h>
#include <src/bcm/dnx/flow/flow_def.h>
#include <shared/utilex/utilex_bitstream.h>
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
dnx_mcdb_hash_table_entry_print_cb(
    int unit,
    void *key,
    void *data)
{
    uint32 *mydata;
    mcdb_hash_table_key_t *mykey;
    uint32 destination = 0;
    uint32 destination_type = 0;
    uint32 group_id = 0;
    uint32 is_ingress = 0;
    uint32 core_id = 0;
    uint32 info = 0;
    uint32 info_type = 0;
    uint32 ref_num = 0;
    uint32 destination2 = 0;
    uint32 destination2_type = 0;
    uint32 same_if_dspa_valid = 0;
    uint32 same_if_dspa = 0;
    uint32 same_if_outlif_valid = 0;
    uint32 same_if_outlif = 0;
    uint32 forward_group_valid = 0;
    uint32 forward_group = 0;
    uint32 *key_temp = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO_ERR_EXIT(key_temp, (sizeof(uint32) * BYTES2WORDS(DNX_DATA_MAX_MULTICAST_MCDB_HASH_TABLE_MAX_KEY_SIZE_IN_WORDS)), "key_temp", "%s%s%s", EMPTY, EMPTY, EMPTY);

    mydata = (uint32 *) data;
    mykey = (mcdb_hash_table_key_t *) key;

    
    SHR_IF_ERR_EXIT(utilex_U8_to_U32(mykey->key, dnx_data_multicast.mcdb_hash_table.max_key_size_in_words_get(unit), key_temp));

    if (dnx_data_multicast.params.feature_get(unit, dnx_data_multicast_params_mcdb_formats_v2))
    {
        
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.destination_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.destination_nof_bits_get(unit), &destination));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.destination_type_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.destination_type_nof_bits_get(unit), &destination_type));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.destination2_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.destination2_nof_bits_get(unit), &destination2));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.destination2_type_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.destination2_type_nof_bits_get(unit), &destination2_type));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.info_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.info_nof_bits_get(unit), &info));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.info_type_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.info_type_nof_bits_get(unit), &info_type));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.same_if_dspa_valid_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.same_if_dspa_valid_nof_bits_get(unit), &same_if_dspa_valid));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.same_if_dspa_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.same_if_dspa_nof_bits_get(unit), &same_if_dspa));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.same_if_outlif_valid_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.same_if_outlif_valid_nof_bits_get(unit), &same_if_outlif_valid));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.same_if_outlif_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.same_if_outlif_nof_bits_get(unit), &same_if_outlif));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.forward_group_valid_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.forward_group_valid_nof_bits_get(unit), &forward_group_valid));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.forward_group_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.forward_group_nof_bits_get(unit), &forward_group));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.group_id_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.group_id_nof_bits_get(unit), &group_id));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.is_static_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.is_static_nof_bits_get(unit), &is_ingress));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field
                        (key_temp, dnx_data_multicast.mcdb_hash_table.reference_num_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.reference_num_nof_bits_get(unit), &ref_num));

        DNX_SW_STATE_PRINT(unit,"Group ID:0x%x", group_id);
        DNX_SW_STATE_PRINT(unit,"\tIs Static ME:0x%x", ((is_ingress == TRUE) ? 0 : 1));
        DNX_SW_STATE_PRINT(unit,"\tDestination:0x%x", destination);
        DNX_SW_STATE_PRINT(unit,"\tDestination type:0x%x", destination_type);
        DNX_SW_STATE_PRINT(unit,"\tDestination2:0x%x", destination2);
        DNX_SW_STATE_PRINT(unit,"\tDestination2 type:0x%x", destination2_type);
        if (info_type == 0)
        {
            DNX_SW_STATE_PRINT(unit, "\tEncap ID:0x%x", info);
        }
        else
        {
            DNX_SW_STATE_PRINT(unit, "\tFailover ID:0x%x", info);
        }
        DNX_SW_STATE_PRINT(unit,"\tSame IF DSPA Valid:0x%x", same_if_dspa_valid);
        if (same_if_dspa_valid == TRUE)
        {
            DNX_SW_STATE_PRINT(unit,"\tSame IF DSPA:0x%x", same_if_dspa);
        }
        DNX_SW_STATE_PRINT(unit,"\tSame IF OutLIF Valid:0x%x", same_if_outlif_valid);
        if (same_if_outlif_valid == TRUE)
        {
            DNX_SW_STATE_PRINT(unit,"\tSame IF OutLIF:0x%x", same_if_outlif);
        }
        DNX_SW_STATE_PRINT(unit,"\tForward Group Valid:0x%x", forward_group_valid);
        if (forward_group_valid == TRUE)
        {
            DNX_SW_STATE_PRINT(unit,"\tForward Group:0x%x", forward_group);
        }
        DNX_SW_STATE_PRINT(unit,"\tReference:0x%x", ref_num);
        DNX_SW_STATE_PRINT(unit,"\tMCDB Entry:0x%x", *mydata);
    }
    else
    {
        
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(key_temp, dnx_data_multicast.mcdb_hash_table.destination_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.destination_nof_bits_get(unit), &destination));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(key_temp, dnx_data_multicast.mcdb_hash_table.reference_num_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.reference_num_nof_bits_get(unit), &ref_num));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(key_temp, dnx_data_multicast.mcdb_hash_table.destination_type_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.destination_type_nof_bits_get(unit), &destination_type));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(key_temp, dnx_data_multicast.mcdb_hash_table.info_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.info_nof_bits_get(unit), &info));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(key_temp, dnx_data_multicast.mcdb_hash_table.group_id_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.group_id_nof_bits_get(unit), &group_id));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(key_temp, dnx_data_multicast.mcdb_hash_table.is_ingress_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.is_ingress_nof_bits_get(unit), &is_ingress));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(key_temp, dnx_data_multicast.mcdb_hash_table.core_id_start_bit_get(unit), dnx_data_multicast.mcdb_hash_table.core_id_nof_bits_get(unit), &core_id));

        DNX_SW_STATE_PRINT(unit, "Core ID:0x%x Group ID:0x%x Is Ingress:0x%x Encap ID:0x%x Destination Type:0x%x Destination:0x%x Reference:0x%x - MCDB Entry:0x%x", core_id, group_id, is_ingress, info, destination_type, destination, ref_num, *mydata);
    }

exit:
    SHR_FREE(key_temp);
    SHR_FUNC_EXIT;
}

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

#ifdef BCM_DNX_SUPPORT
    DNX_SW_STATE_PRINT(unit, "Forbidden fields for DBAL table %s: [0x%x,0x%x]", dbal_logical_table_to_string(unit, *dbal_table_id),
                       sw_state_table_info->forbidden_fields_lo, sw_state_table_info->forbidden_fields_hi);
#endif

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

int
dnx_algo_flow_dbal_table_to_valid_result_types_print_cb(
    int unit,
    void *key,
    void *data)
{
    uint32 *mydata;
    flow_app_encap_info_t *mykey;

    mykey = (flow_app_encap_info_t *) key;
    mydata = (uint32 *) data;

    DNX_SW_STATE_PRINT(unit, "[%s, phase: %d, result types bitmap: %d] ", dbal_logical_table_to_string(unit, mykey->dbal_table), mykey->encap_access, *mydata);

    return 0;
}

#else

int
dnx_mcdb_hash_table_entry_print_cb(
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
dnx_algo_flow_dbal_table_to_valid_result_types_print_cb(
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


