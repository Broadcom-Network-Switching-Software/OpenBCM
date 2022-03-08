
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#include <appl/diag/system.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnxc/dnxc_adapter_reg_access.h>
#include <soc/dnx/pp_stage.h>

#include <src/appl/diag/dnx/pp/kleap/diag_dnx_ikleap.h>
#include <include/soc/dnx/mdb_internal_shared.h>

#include "sand_signals_internal.h"

#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

typedef struct {
    char*  start; 
    char*  data;
    uint32 length;
    int    nof;
} adapter_info_t;

#define ADAPTER_INFO_GET_UINT32(mc_var, mc_info)                                                               \
    if((mc_info)->length >= sizeof(uint32))                                                                    \
    {                                                                                                          \
        mc_var = bcm_ntohl(*((uint32 *)(mc_info)->data));                                                      \
        (mc_info)->length -= sizeof(uint32); (mc_info)->data += sizeof(uint32);                                \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Request 4(uint32) vs %u%s%s\n", (mc_info)->length, EMPTY, EMPTY); \
    }

#define ADAPTER_INFO_SKIP_MEM(mc_mem_length, mc_info)                                                          \
    if((mc_info)->length >= mc_mem_length)                                                                     \
    {                                                                                                          \
        (mc_info)->length -= mc_mem_length; (mc_info)->data += mc_mem_length;                                  \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Signal size:%u is more than buffer available:%u%s\n",             \
                                                    mc_mem_length, (mc_info)->length, EMPTY);                  \
    }

#define ADAPTER_INFO_GET_MEM(mc_mem, mc_mem_length, mc_info)                                                   \
    if(mc_mem_length > DSIG_MAX_SIZE_STR)                                                                      \
    {                                                                                                          \
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Signal size:%u is more than maximum supported:%u%s\n",            \
                                                mc_mem_length, DSIG_MAX_SIZE_STR, EMPTY);                      \
    }                                                                                                          \
    if((mc_info)->length >= mc_mem_length)                                                                     \
    {                                                                                                          \
        sal_memcpy(mc_mem, (mc_info)->data, mc_mem_length);                                                    \
        (mc_info)->length -= mc_mem_length; (mc_info)->data += mc_mem_length;                                  \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Signal size:%u is more than buffer available:%u%s\n",             \
                                                    mc_mem_length, (mc_info)->length, EMPTY);                  \
    }

#define MAX_NOF_KEYS 4
#define MAX_NOF_PAYLOADS 8
#define DEFAULT_RESULT_SIZE 96
#define DIRECT_MDB_TYPE 0
#define EM_MDB_TYPE 1
#define EEDB_MDB_TYPE 2
#define LPM_MDB_TYPE 3
#define TCAM_MDB_TYPE 4

static void
sand_adapter_clear_block_signals(
    int unit,
    pp_block_t *cur_pp_block)
{
    pp_stage_t *cur_pp_stage;
    int i_st;

    
    for(i_st = 0; i_st < cur_pp_block->stage_num; i_st++)
    {
        cur_pp_stage = &cur_pp_block->stages[i_st];

        if(cur_pp_stage->stage_debug_signals != NULL)
        {
            sal_memset(cur_pp_stage->stage_debug_signals, 0, sizeof(debug_signal_t) * DNX_MAX_NOF_DEBUG_SIGNALS);
            cur_pp_stage->stage_signal_num = 0;
        }
    }
    cur_pp_block->adapter_signal_num = 0;
}


static shr_error_e
add_stage_signal(
    int unit,
    uint32 *input_buffer,
    uint32 start_bit,
    uint32 nof_bits,
    char *to,
    char *from,
    char *attr,
    char *interface_index,
    int multy_data_index,
    pp_stage_t *cur_pp_stage
    )
{
    debug_signal_t *debug_signal;
    SHR_FUNC_INIT_VARS(unit);

    if ( nof_bits == 0 )
    {
        SHR_EXIT();
    }
    debug_signal = &cur_pp_stage->stage_debug_signals[cur_pp_stage->stage_signal_num++];
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(
            input_buffer, start_bit, nof_bits, debug_signal->adapter_value));
    debug_signal->size = nof_bits;
    SHR_IF_ERR_EXIT(sal_snprintf(debug_signal->to, RHSTRING_MAX_SIZE - 1, "%s", to));
    SHR_IF_ERR_EXIT(sal_snprintf(debug_signal->from, RHSTRING_MAX_SIZE - 1, "%s", from));
    if ( multy_data_index > 0)
    {
        SHR_IF_ERR_EXIT(sal_snprintf(debug_signal->attribute, RHSTRING_MAX_SIZE - 1, "%s_%s_%d",
                attr, interface_index, multy_data_index));
    }
    else if ( sal_strncmp(interface_index, "Learning", sal_strlen("Learning")) == 0 )
    {
        SHR_IF_ERR_EXIT(sal_snprintf(debug_signal->attribute, RHSTRING_MAX_SIZE - 1, "%s_%s_%s",
                "Learning", attr, &interface_index[sal_strlen("Learning") + 1]));
    }
    else if ( sal_strncmp(interface_index, "Traps", sal_strlen("Traps")) == 0 )
    {
        SHR_IF_ERR_EXIT(sal_snprintf(debug_signal->attribute, RHSTRING_MAX_SIZE - 1, "%s_%s_%s",
                "Traps", attr, &interface_index[sal_strlen("Traps") + 1]));
    }
    else if ( sal_strcmp(interface_index, "") == 0 )
    {
        SHR_IF_ERR_EXIT(sal_snprintf(debug_signal->attribute, RHSTRING_MAX_SIZE - 1, "%s", attr));
    }
    else
    {
        SHR_IF_ERR_EXIT(sal_snprintf(debug_signal->attribute, RHSTRING_MAX_SIZE - 1, "%s_%s", attr, interface_index));
    }

exit:
   SHR_FUNC_EXIT;
}



static shr_error_e
sand_adapter_memory_id_and_type_2_db_id(
       int unit,
       int mdb_type,
       int memory_id,
       int *db_id)
{
    int i;
    SHR_FUNC_INIT_VARS(unit);

    
    if ( mdb_type == TCAM_MDB_TYPE )
    {
        *db_id=DBAL_PHYSICAL_TABLE_TCAM;
        SHR_EXIT();
    }

    if ( mdb_type == EEDB_MDB_TYPE )
    {
        *db_id=DBAL_NOF_PHYSICAL_TABLES;
        SHR_EXIT();
    }

    for (i = 0; i < DBAL_NOF_PHYSICAL_TABLES; i++)
    {
        if ( dnx_data_mdb.pdbs.mdb_adapter_mapping_get(unit, i)->mdb_type == mdb_type &&
             dnx_data_mdb.pdbs.mdb_adapter_mapping_get(unit, i)->memory_id == memory_id )
        {
            *db_id=i;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(_SHR_E_INTERNAL, "DB id not found. mdb_type: %d, memory_id: %d\n", mdb_type, memory_id);
exit:
   SHR_FUNC_EXIT;
}



static int
sand_adapter_signal_skip_check(
       int unit,
       int mdb_type,
       char *interface)
{
    if ( mdb_type == EEDB_MDB_TYPE )
    {
        return 1;
    }

    if (sal_strncmp(interface, "SkipSignal", sal_strlen("SkipSignal")) == 0)
    {
        return 1;
    }

    return 0;
}


static void
sand_adapter_get_appdb_size_and_encode(
       int unit,
       int mdb_type,
       int db_id,
       uint32 key_size,
       uint32 *app_db_size,
       uint32 *is_app_db_part_of_key)
{
    if ( mdb_type == EM_MDB_TYPE )
    {
        *is_app_db_part_of_key = 0;
        *app_db_size = dnx_data_mdb.em.em_info_get(unit, db_id)->tid_size;
    }
    else if ( mdb_type == LPM_MDB_TYPE)
    {
        *is_app_db_part_of_key = 1;
        *app_db_size = 6;
    }
    else if ( mdb_type == TCAM_MDB_TYPE && key_size == 166 )
    {
        *is_app_db_part_of_key = 0;
        *app_db_size = 6;
    }
    else
    {
        *is_app_db_part_of_key = 0;
        *app_db_size = 0;
    }
}


static shr_error_e
sand_adapter_prepare_result(
       int unit,
       int mdb_type,
       int db_id,
       char *db_name,
       uint32 * adapter_value,
       uint32 data_index,
       uint32 data_size,
       uint32 *res_size,
       uint32 *res)
{
    const uint32 full_vmv_size = 6, strength_bits_size = 3;
    uint32 res_arr[DEFAULT_RESULT_SIZE] = {0}, full_vmv_value = 0;
    uint8 vmv_size = 0, vmv_value = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    if ( data_size == 0 )
    {
        (*res_size) = DEFAULT_RESULT_SIZE;
        SHR_IF_ERR_EXIT(utilex_bitstream_clear(res, DEFAULT_RESULT_SIZE));
        SHR_EXIT();
    }

    
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(adapter_value, data_index, data_size, res_arr));

    if (mdb_type == EM_MDB_TYPE)
    {
        (*res_size) = data_size;
        
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index+data_size-full_vmv_size,
                full_vmv_size, &full_vmv_value));
        SHR_IF_ERR_EXIT(mdb_em_vmv_calculation_by_full_vmv(unit, db_id, full_vmv_value, &vmv_value, &vmv_size));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(res_arr, vmv_size, data_size-vmv_size, res));
        SHR_EXIT();
    }
    else if (sal_strncmp(db_name, "KAPS", sal_strlen("KAPS")) == 0 )
    {
        (*res_size) = DEFAULT_RESULT_SIZE;
        
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(res_arr, 93-data_size, data_size-strength_bits_size, res));
        SHR_EXIT();
    }
    else
    {
        (*res_size) = data_size;
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(res_arr, 0, data_size, res));
        SHR_EXIT();
    }
exit:
   SHR_FUNC_EXIT;
}


shr_error_e
sand_signals_adapter_get_interface_index(
    int unit,
    int adapter_stage_index,
    int kbr_index,
    uint32 memory_id,
    dbal_physical_tables_e db_id,
    char *db_name,
    dnx_pp_stage_e stage_id,
    char *stage_name,
    char *res)
{
    int interface_index, caller_id_key, stage_and_mdb_key;
    stage_t STAGE_INFO;
    SHR_FUNC_INIT_VARS(unit);

    STAGE_INFO = dnx_ikleap_stages_get_by_name(unit, stage_name);

    
    if (adapter_stage_index != 0 && kbr_index < STAGE_INFO.nof_kbrs &&
        (adapter_stage_index == STAGE_INFO.stage_index_in_pipe[0] || adapter_stage_index == STAGE_INFO.stage_index_in_pipe[1])
       )
    {
        interface_index = STAGE_INFO.kbr_if_id[kbr_index];

        if (interface_index == INVALID_IF)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }

        
        if (sal_strncmp(stage_name, "FWD1", 4) == 0 && kbr_index == 6
            && !dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_separate_fwd_learn_mact))
        {
            sal_sprintf(res, "%s_%d", "Learning", interface_index);
            SHR_EXIT();
        }
        else if (sal_strncmp(stage_name, "FWD1", 4) && (kbr_index == 12 || kbr_index == 13))
        {
            sal_sprintf(res, "%s_%d", "Traps", interface_index);
            SHR_EXIT();
        }
        else if (interface_index == SINGLE_IF)
        {
            sal_sprintf(res, "%s", "");
        }
        else
        {
            sal_sprintf(res, "%d", interface_index);
        }

        SHR_EXIT();
    }

    
    {
        if (kbr_index != 0 )
        {
            caller_id_key = kbr_index;
        }
        else if(memory_id != 0)
        {
            caller_id_key = memory_id;
        }
        else
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }

        stage_and_mdb_key = stage_id+100*db_id;
        if(sal_strncmp(dnx_data_adapter.mdb.lookup_caller_id_mapping_get(unit, caller_id_key, stage_and_mdb_key)->interface,
                        "NotFound", sal_strlen("NotFound")) == 0)
        {
            {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "\n Signal interface not found. caller_id_key: %d stage_and_mdb_key: %d stage_name: %s stage_id: %d db_name: %s db_id: %d \n",
                                                                caller_id_key, stage_and_mdb_key, stage_name, stage_id, db_name, db_id);
            }
        }
        else if(sal_strncmp(dnx_data_adapter.mdb.lookup_caller_id_mapping_get(unit, caller_id_key, stage_and_mdb_key)->interface,
                             "Empty", sal_strlen("Empty")) == 0)
        {
            sal_sprintf(res, "%s", "");
            SHR_EXIT();
        }
        else if(sal_strncmp(dnx_data_adapter.mdb.lookup_caller_id_mapping_get(unit, caller_id_key, stage_and_mdb_key)->interface,
                             "UnknownInterface", sal_strlen("UnknownInterface")) == 0)
        {
            {
                sal_sprintf(res, "%s", "SkipSignal");
                SHR_EXIT();
            }
        }
        else
        {
            sal_sprintf(res, "%s", dnx_data_adapter.mdb.lookup_caller_id_mapping_get(unit, caller_id_key, stage_and_mdb_key)->interface);
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sand_adapter_parse_mdb_signal(
    int unit,
    int signal_size,
    uint32 * adapter_value,
    pp_stage_t *cur_pp_stage)
{
    uint32 data_index = signal_size;
    uint32 ms_id = 0, stage_and_intrf = 0, mdb_type = 0, memory_id = 0,
            nof_keys = 0, nof_data_payloads = 0, hit = 0, error = 0;
    uint32 key_sizes[MAX_NOF_KEYS] = {0}, data_sizes[MAX_NOF_PAYLOADS] = {0};
    char db_name[RHNAME_MAX_SIZE], interface_index[RHNAME_MAX_SIZE];
    int db_id = 0;
    uint32 app_db_size = 0, is_app_db_part_of_key = 0, app_db = 0, is_crps_signal = 0;
    int i = 0, skip_signal_flag = 0;
    uint32 result_size = 0, result_array[DEFAULT_RESULT_SIZE] = {0};

    SHR_FUNC_INIT_VARS(unit);

    while ( data_index > 0)
    {
        
        data_index-=16;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 16, &ms_id));
        ms_id = bcm_ntohs(ms_id);

        
        data_index-=16;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 16, &stage_and_intrf));
        stage_and_intrf = bcm_ntohs(stage_and_intrf);

        
        data_index-=8;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 8, &mdb_type));

        
        data_index-=16;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 16, &memory_id));
        memory_id = bcm_ntohs(memory_id);

        
        is_crps_signal = (mdb_type == EM_MDB_TYPE && memory_id == dnx_data_mdb.pdbs.adapter_mapping_crps_id_get(unit));
        if (is_crps_signal)
        {
            sal_sprintf(db_name, "%s", "CRPS");
            sal_sprintf(interface_index, "%s", "crps");
        }
        else if (mdb_type != EEDB_MDB_TYPE)
        {
            
            if ( sand_adapter_memory_id_and_type_2_db_id(unit, mdb_type, memory_id, &db_id) != _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "db_id was not found. stage: %s mdb_type: %d, memory_id: %d\n",
                                                                        cur_pp_stage->name, mdb_type, memory_id);
            }

            
            sal_strncpy(db_name, dbal_physical_table_to_string(unit, db_id), RHNAME_MAX_SIZE - 1);

            
            SHR_IF_ERR_EXIT(sand_signals_adapter_get_interface_index(unit, stage_and_intrf/1000, stage_and_intrf%1000, memory_id,
                                             db_id, db_name, cur_pp_stage->pp_id, cur_pp_stage->name, interface_index));
        }
        
        skip_signal_flag = sand_adapter_signal_skip_check(unit, mdb_type, interface_index);

        
        data_index-=8;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 8, &nof_keys));

        if (nof_keys > MAX_NOF_KEYS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal nof_keys. Expected nof_keys <= %d, "
                    "actual nof_keys: %d \n", MAX_NOF_KEYS, nof_keys);
        }

        for ( i = 0; i < nof_keys; i++ )
        {
            
            if ( mdb_type == TCAM_MDB_TYPE && nof_keys == 2)
            {
                
                data_index-=8;
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 8, &app_db_size));

                
                data_index-=app_db_size;
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, app_db_size, &app_db));
                app_db = bcm_ntohs(app_db);

                
                data_index-=16;
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 16, &(key_sizes[i])));
                key_sizes[i] = bcm_ntohs(key_sizes[i]);

                nof_keys = 1;
            }
            else if (is_crps_signal)
            {
                app_db_size = 0;
                
                data_index-=16;
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 16, &(key_sizes[i])));
                key_sizes[i] = bcm_ntohs(key_sizes[i]);
            }
            else
            {
                
                data_index-=16;
                SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 16, &(key_sizes[i])));
                key_sizes[i] = bcm_ntohs(key_sizes[i]);

                if ( skip_signal_flag == 0 )
                {
                    
                    sand_adapter_get_appdb_size_and_encode(unit, mdb_type, db_id, key_sizes[i],
                                                &app_db_size, &is_app_db_part_of_key);

                    
                    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(
                            adapter_value, data_index-app_db_size, app_db_size, &app_db));

                    if ( is_app_db_part_of_key == 0 )
                    {
                        key_sizes[i] -= app_db_size;
                        data_index -= app_db_size;
                    }
                }
            }

            
            if ( skip_signal_flag == 0 )
            {
                SHR_IF_ERR_EXIT(add_stage_signal(unit, &app_db, 0, app_db_size,
                        db_name, cur_pp_stage->name, "Appdb", interface_index, i, cur_pp_stage));
            }

            if (key_sizes[i] > data_index)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Signal parsing error, Key size is incorrect.");
            }

            
            data_index-=key_sizes[i];
            if ( skip_signal_flag == 0 )
            {
                SHR_IF_ERR_EXIT(add_stage_signal(unit, adapter_value, data_index, key_sizes[i],
                        db_name, cur_pp_stage->name, "Key", interface_index, i, cur_pp_stage));
            }
        }

        
        data_index-=8;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 8, &nof_data_payloads));

        if (nof_data_payloads > MAX_NOF_PAYLOADS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Ilegal nof_data_payloads. Expected nof_data_payloads <= %d, "
                    "actual nof_keys: %d \n", MAX_NOF_PAYLOADS, nof_data_payloads);
        }

        for ( i = 0; i < nof_data_payloads; i++ )
        {
            
            data_index-=16;
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 16, &(data_sizes[i])));
            data_sizes[i] = bcm_ntohs(data_sizes[i]);

            if (data_sizes[i] >= data_index)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Signal parsing error, Data size is incorrect.");
            }

            
            data_index-=data_sizes[i];
            if (skip_signal_flag == 0)
            {
                SHR_IF_ERR_EXIT(sand_adapter_prepare_result(unit, mdb_type, db_id, db_name, adapter_value, data_index,
                                                            data_sizes[i], &result_size, result_array));

                SHR_IF_ERR_EXIT(add_stage_signal(unit, result_array, 0, result_size, cur_pp_stage->name,
                                                      db_name, "Result", interface_index, i, cur_pp_stage));
            }
        }

        
        data_index-=8;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 8, &hit));
        if ( skip_signal_flag == 0 )
        {
            SHR_IF_ERR_EXIT(add_stage_signal(unit, adapter_value, data_index, 8,
                    cur_pp_stage->name, db_name, "Hit", interface_index, 0, cur_pp_stage));
        }

        
        data_index-=8;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(adapter_value, data_index, 8, &error));
        if ( skip_signal_flag == 0 )
        {
            SHR_IF_ERR_EXIT(add_stage_signal(unit, adapter_value, data_index, 8,
                    cur_pp_stage->name, db_name, "Error", interface_index, 0, cur_pp_stage));
        }

    }

    if (data_index != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Signal was not completly read, data_index expected to be 0. "
                "data_index: %d \n", data_index);
    }

exit:
   SHR_FUNC_EXIT;
}

void
sand_adapter_clear_signals(
    int unit)
{
    device_t * device;
    int i_block;

    if ((device = sand_signal_device_get(unit)) != NULL)
    {
        for (i_block = 0; i_block < device->block_num; i_block++)
        {
            sand_adapter_clear_block_signals(unit, &device->pp_blocks[i_block]);
        }
    }
}

shr_error_e
sand_adapter_get_stage_signals(
        device_t * device,
        int unit,
        int core,
        char *block_n,
        pp_stage_t *pp_stage,
        int flags)
{
    adapter_info_t adapter_info;
    uint32 name_length;
    int retry_count = 3;
    int i_sig;
    pp_stage_t *subst_pp_stage = NULL, *cur_pp_stage = NULL;

    SHR_FUNC_INIT_VARS(unit);

    
    if(pp_stage->status & SIGNAL_STAGE_HAS_SUBSTITUTE)
    {
        cur_pp_stage = pp_stage->peer_stage;
    }
    else
    {
        cur_pp_stage = pp_stage;
    }

    sal_memset(&adapter_info, 0 , sizeof(adapter_info_t));

    if(cur_pp_stage->ms_id == DSIG_ILLEGAL_MS_ID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal MS_ID for stage:%s\n", cur_pp_stage->name);
    }

    while(retry_count)
    {
        int rv;
        rv = adapter_get_signal(unit, core, cur_pp_stage->ms_id, &adapter_info.length, &adapter_info.start);
        if(rv == _SHR_E_EMPTY)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }

        if((adapter_info.length == 0) || (adapter_info.start == NULL))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No signal info received from adapter for stage:%s length:%u\n",
                                                                             cur_pp_stage->name, adapter_info.length);
        }
        adapter_info.data = adapter_info.start;
        
        ADAPTER_INFO_GET_UINT32(adapter_info.nof, &adapter_info);
        
        if((adapter_info.nof == 0) && (flags & SIGNALS_MATCH_RETRY))
        {   
            if(adapter_info.start != NULL)
            {
                sal_free(adapter_info.start);
                adapter_info.start = NULL;
            }
            retry_count--;
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Retry:%d for stage:%s buffer length:%u\n"),
                                                     3 - retry_count, cur_pp_stage->name, adapter_info.length));
        }
        else
        {   
            break;
        }
    }

    if(adapter_info.nof == 0)
    {
        
        SHR_EXIT();
    }
    if (adapter_info.nof > DNX_MAX_NOF_DEBUG_SIGNALS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect DNX_MAX_NOF_DEBUG_SIGNALS (%d) < %d \n", DNX_MAX_NOF_DEBUG_SIGNALS, adapter_info.nof);
    }

    
    if (cur_pp_stage->stage_debug_signals == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "cur_pp_stage->stage_debug_signals not allocated \n");
    }
    cur_pp_stage->stage_signal_num = 0;

    
    if(cur_pp_stage->status & SIGNAL_STAGE_IS_SUBSTITUTE)
    {
        if((subst_pp_stage = cur_pp_stage->peer_stage) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Peer stage should be initiated for:%s\n", cur_pp_stage->name);
        }
        
        if (subst_pp_stage->stage_debug_signals == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "subst_pp_stage->stage_debug_signals not allocated \n");
        }
        subst_pp_stage->stage_signal_num = 0;
    }

    for(i_sig = 0; i_sig < adapter_info.nof; i_sig++)
    {
        int signal_size;
        debug_signal_t *debug_signal, *asic_debug_signal;
        char signal_str[DSIG_MAX_SIZE_STR + 1];
        char signal_name[DSIG_MAX_SIZE_STR];
        ADAPTER_INFO_GET_UINT32(name_length, &adapter_info);
        ADAPTER_INFO_GET_MEM(signal_name, name_length, &adapter_info);
        if(name_length >= RHNAME_MAX_SIZE)
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("Signal:%s is longer than max:%d\n"), signal_name, name_length));
        }
        
        ADAPTER_INFO_GET_UINT32(signal_size, &adapter_info);
        if(signal_size > DSIG_MAX_SIZE_BITS)
        {
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("Signal:%s-%s-%s size:%d is more than maximum:%d\n"),
                                           block_n, cur_pp_stage->name, signal_name, signal_size, DSIG_MAX_SIZE_BITS));
            ADAPTER_INFO_SKIP_MEM((signal_size + 3) / 4 + 2, &adapter_info);
            continue;
        }

        
        signal_name[name_length] = 0;

        
        if (strcmp(signal_name,"MDB_memory_access") == 0) {
            uint32 mdb_memory_access_value[DSIG_MAX_SIZE_UINT32];
            sal_memset(mdb_memory_access_value, 0, sizeof(mdb_memory_access_value));
            sal_memset(signal_str, 0, DSIG_MAX_SIZE_STR + 1);
            ADAPTER_INFO_GET_MEM(signal_str, (signal_size + 3) / 4 + 2, &adapter_info);
            parse_long_integer(mdb_memory_access_value, DSIG_MAX_SIZE_UINT32, signal_str);
            SHR_IF_ERR_EXIT(sand_adapter_parse_mdb_signal(unit, signal_size, mdb_memory_access_value, cur_pp_stage));
            continue;
        }

        
        if((asic_debug_signal = sand_adapter_extract_signal(unit, device, signal_name, block_n, cur_pp_stage->name)) != NULL)
        {
            debug_signal = &cur_pp_stage->stage_debug_signals[cur_pp_stage->stage_signal_num++];
        }
        else if((subst_pp_stage != NULL) && ((asic_debug_signal =
                    sand_adapter_extract_signal(unit, device, signal_name, block_n, subst_pp_stage->name)) != NULL))
        {
            debug_signal = &subst_pp_stage->stage_debug_signals[subst_pp_stage->stage_signal_num++];
        }
        else
        {
            debug_signal = &cur_pp_stage->stage_debug_signals[cur_pp_stage->stage_signal_num++];
        }

        debug_signal->size = signal_size;

        sal_memset(signal_str, 0, DSIG_MAX_SIZE_STR + 1);
        ADAPTER_INFO_GET_MEM(signal_str, (debug_signal->size + 3) / 4 + 2, &adapter_info);
        parse_long_integer(debug_signal->adapter_value, DSIG_MAX_SIZE_UINT32, signal_str);

        
        sal_snprintf(debug_signal->hw, RHSTRING_MAX_SIZE - 1, "%s", signal_name);
        
        if(asic_debug_signal != NULL)
        {
            sal_strncpy(debug_signal->to,        asic_debug_signal->to, RHNAME_MAX_SIZE - 1);
            sal_strncpy(debug_signal->from,      asic_debug_signal->from, RHNAME_MAX_SIZE - 1);
            sal_strncpy(debug_signal->attribute, asic_debug_signal->attribute, RHNAME_MAX_SIZE - 1);
            debug_signal->description = sand_signal_metadata_description(device, debug_signal->attribute);
            if(!ISEMPTY(asic_debug_signal->expansion))
            {
                sal_strncpy(debug_signal->expansion, asic_debug_signal->expansion, RHNAME_MAX_SIZE - 1);
            }
            if(!ISEMPTY(asic_debug_signal->resolution))
            {
                sal_strncpy(debug_signal->resolution, asic_debug_signal->resolution, RHNAME_MAX_SIZE - 1);
            }
        }
        else
        {
            
            if(cur_pp_stage->stage_to != NULL)
            {
                sal_strncpy(debug_signal->to,    cur_pp_stage->stage_to->name, RHNAME_MAX_SIZE - 1);
            }
            sal_strncpy(debug_signal->from,      cur_pp_stage->name, RHNAME_MAX_SIZE - 1);
            sal_strncpy(debug_signal->attribute, signal_name, RHNAME_MAX_SIZE - 1);
            
            if ((sand_signal_struct_get(device, signal_name, cur_pp_stage->name, NULL, block_n)) != NULL)
            {   
                sal_strncpy(debug_signal->expansion, signal_name, RHNAME_MAX_SIZE - 1);
            }
            if ((sand_signal_resolve_get(device, signal_name, cur_pp_stage->name, NULL, block_n)) != NULL)
            {   
                sal_strncpy(debug_signal->resolution, signal_name, RHNAME_MAX_SIZE - 1);
            }
        }
    }
    if(adapter_info.length != 0)
    {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("%d bytes left in %s buffer\n"), adapter_info.length, cur_pp_stage->name));
    }

exit:
    if(adapter_info.start != NULL)
        sal_free(adapter_info.start);
    SHR_FUNC_EXIT;
}

shr_error_e
sand_adapter_update_stages(
        int unit,
        device_t * device)
{
    adapter_info_t stage_info_m;
    int i_bl, i_st;
    int i_ms, name_length;
    pp_stage_t *pp_stage;
    pp_block_t *pp_block;
    uint32 ms_id;
    char adapter_name[RHNAME_MAX_SIZE];
    int stage_found;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(adapter_get_block_names(unit, (int *)&stage_info_m.length, &stage_info_m.start));
    if((stage_info_m.start == NULL) || (stage_info_m.length == 0))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NOT_FOUND, "Adapter Returned NULL buffer %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    stage_info_m.data = stage_info_m.start;
    
    ADAPTER_INFO_GET_UINT32(stage_info_m.nof, &stage_info_m);
    
    for(i_ms = 0; (i_ms < stage_info_m.nof) && (stage_info_m.length > sizeof(uint32) * 2); i_ms++)
    {
        stage_found = FALSE;
        ADAPTER_INFO_GET_UINT32(ms_id, &stage_info_m);
        ADAPTER_INFO_GET_UINT32(name_length,  &stage_info_m);
        ADAPTER_INFO_GET_MEM(adapter_name, name_length, &stage_info_m);
        adapter_name[name_length] = 0;
        for (i_bl = 0; i_bl < device->block_num; i_bl++)
        {
            
            if(stage_found == TRUE)
                break;

            pp_block = &device->pp_blocks[i_bl];
            for (i_st = 0; i_st < pp_block->stage_num; i_st++)
            {
                pp_stage = &pp_block->stages[i_st];
                if(!sal_strcasecmp(pp_stage->adapter_name, adapter_name))
                {
                    pp_stage->ms_id = ms_id;
                    
                    if(i_st != pp_block->stage_num - 1)
                    {
                        pp_stage->stage_to = &pp_block->stages[i_st + 1];
                    }
                    
                    if(pp_stage->peer_stage != NULL)
                    {
                        int i_cur_st;
                        pp_stage_t *cur_pp_stage;
                        for (i_cur_st = 0; i_cur_st < pp_block->stage_num; i_cur_st++)
                        {
                            cur_pp_stage = &pp_block->stages[i_cur_st];
                            if(!sal_strcasecmp(cur_pp_stage->name, pp_stage->peer_stage->name))
                            {
                                pp_stage->peer_stage->stage_to = &pp_block->stages[i_cur_st + 1];
                                break;
                            }
                        }
                    }
                    stage_found = TRUE;
                }
            }
        }
        if(stage_found == FALSE)
        {
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META("PP Stage not found for adapter MS:%s ID:%d\n"), adapter_name, ms_id));
        }
    }

    if(stage_info_m.length != 0)
    {
        LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META("%d bytes left in stages buffer\n"), stage_info_m.length));
    }
exit:
    if(stage_info_m.start != NULL)
        sal_free(stage_info_m.start);
    SHR_FUNC_EXIT;
}

debug_signal_t *
sand_adapter_extract_signal(int unit, device_t *device, char *name_n, char *block_n, char *from_n)
{
    pp_block_t *cur_pp_block;
    debug_signal_t *debug_signal;
    int i, j;

    
    if(ISEMPTY(name_n) || ISEMPTY(block_n) || ISEMPTY(from_n))
        return NULL;

    for (i = 0; i < device->block_num; i++)
    {
        cur_pp_block = &device->pp_blocks[i];

        if (sal_strcasecmp(cur_pp_block->name, block_n))
            continue;

        for (j = 0; j < cur_pp_block->block_signal_num; j++)
        {
            debug_signal = &cur_pp_block->block_debug_signals[j];
            if (sal_strcasecmp(debug_signal->from, from_n))
                continue;
            if (!sal_strcasecmp(debug_signal->to, "PEM"))
                continue;

            if (!sal_strcasecmp(debug_signal->attribute, name_n) || !sal_strcasecmp(debug_signal->adapter_name, name_n))
                return debug_signal;
        }
    }

    return NULL;
}
