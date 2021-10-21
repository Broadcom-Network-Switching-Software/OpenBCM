
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_COMMON

#include <sal/types.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/sand/sand_signals.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_visibility_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <include/soc/dnx/dbal/auto_generated/dbal_defines_enum_field_types.h>
#include <soc/drv.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#define DNX_VISIBILITY_ENGINE_CALC_IDX(_pema_idx, _direction, _win_idx) (((_pema_idx << 4) & 0x00F0) \
    | ((_direction << 3) & 0x0008) \
    | (_win_idx & 0x0007))

#define DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block_m)                           \
    if (!dnx_data_debug.mem.block_map_get(unit, asic_block)->is_asic_block)   \
    {                                                                          \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Bad asic block id:%d\n", asic_block_m); \
    }


/* *INDENT-OFF* */

static dnx_visibility_block_map_t *dnx_visibility_block_map[SOC_MAX_NUM_DEVICES];


static int **mem_name_per_block_array[SOC_MAX_NUM_DEVICES];


static dnx_visibility_block_info_t **dnx_visibility_block_info[SOC_MAX_NUM_DEVICES];





static shr_error_e
dnx_visibility_dbal_enum_max_value_get(
    int unit,
    dbal_fields_e field_id,
    uint32 *max_value)
{
    uint32 i, max_value_internal;

    SHR_FUNC_INIT_VARS(unit);

    max_value_internal = 0;
    SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get(unit, field_id, INVALID_ENUM, &i));
    while (i != INVALID_ENUM)
    {
        max_value_internal++;
        SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get(unit, field_id, i, &i));
    }

    *max_value = max_value_internal;
exit:
    SHR_FUNC_EXIT;
}

char *
dnx_debug_mem_name(
    int unit,
    dbal_enum_value_field_pp_asic_block_e asic_block)
{
    uint32 block_max_index;
    shr_error_e rv = dnx_visibility_dbal_enum_max_value_get(unit, DBAL_FIELD_PP_ASIC_BLOCK, &block_max_index);
    if (rv != _SHR_E_NONE)
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Faild to get max value of block index\n")));
        return NULL;
    }

    if (((int) asic_block < 0) || ((int) asic_block >= block_max_index))
    {
        return "Unknown block";
    }

    if (dnx_visibility_block_map[unit][asic_block].table == DBAL_TABLE_EMPTY)
    {
        return "Unknown block";
    }

    return dnx_visibility_block_map[unit][asic_block].name;
}


static shr_error_e
dnx_debug_cache_init(
    int unit)
{
    int i_bl, i_line;
    int core_idx;
    uint32 block_max_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_visibility_dbal_enum_max_value_get(unit, DBAL_FIELD_PP_ASIC_BLOCK, &block_max_index));

    
    if ((dnx_visibility_block_map[unit] = utilex_alloc(sizeof(dnx_visibility_block_map_t) * (block_max_index))) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of dnx_visibility_block_map[unit=%d] failed\n", unit);
    }

    for (i_bl = 0; i_bl < block_max_index; i_bl++)
    {
        if ((dnx_visibility_block_map[unit][i_bl].name = utilex_alloc(sizeof(char) * (RHNAME_MAX_SIZE + 1))) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY,
                         "Memory allocation for block_name=%s dnx_visibility_block_map[unit=%d][block=%d] failed\n",
                         dnx_visibility_block_map[unit][i_bl].name, unit, i_bl);
        }
        sal_strncpy(dnx_visibility_block_map[unit][i_bl].name,
                    dbal_enum_id_to_string(unit, DBAL_FIELD_PP_ASIC_BLOCK,
                                           dnx_data_debug.mem.block_map_get(unit, i_bl)->block_id), RHNAME_MAX_SIZE);
        dnx_visibility_block_map[unit][i_bl].table = dnx_data_debug.mem.block_map_get(unit, i_bl)->dbal_table;
        dnx_visibility_block_map[unit][i_bl].valids_field = dnx_data_debug.mem.block_map_get(unit, i_bl)->dbal_field;
        dnx_visibility_block_map[unit][i_bl].direction = dnx_data_debug.mem.block_map_get(unit, i_bl)->direction;
    }

    
    if ((mem_name_per_block_array[unit] = utilex_alloc(sizeof(int *) * (block_max_index))) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of mem_name_per_block_array[unit=%d] failed\n", unit);
    }
    for (i_bl = 0; i_bl < block_max_index; i_bl++)
    {
        mem_name_per_block_array[unit][i_bl] = utilex_alloc(sizeof(int) * (2));
        mem_name_per_block_array[unit][i_bl][0] = dnx_data_debug.mem.mem_array_get(unit, i_bl)->mem1;
        mem_name_per_block_array[unit][i_bl][1] = dnx_data_debug.mem.mem_array_get(unit, i_bl)->mem2;
    }

    if ((dnx_visibility_block_info[unit] =
         utilex_alloc(sizeof(dnx_visibility_block_info_t *) * (block_max_index))) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of dnx_visibility_block_info[unit=%d] failed\n", unit);
    }

    for (i_bl = 0; i_bl < block_max_index; i_bl++)
    {
        if (dnx_data_debug.mem.params_get(unit, i_bl)->valid)
        {
            if (dnx_visibility_block_info[unit][i_bl] != NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of info for block:%s was not cleared properly\n",
                             dnx_visibility_block_map[unit][i_bl].name);
            }
            if ((dnx_visibility_block_info[unit][i_bl] = utilex_alloc(sizeof(dnx_visibility_block_info_t))) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of info for block:%s failed\n",
                             dnx_visibility_block_map[unit][i_bl].name);
            }

            
            dnx_visibility_block_info[unit][i_bl]->line_nof = dnx_data_debug.mem.params_get(unit, i_bl)->line_nof;
            if (dnx_visibility_block_info[unit][i_bl]->line_nof > DNX_VISIBILITY_BLOCK_MAX_NOF_LINES)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY,
                             "block:%s has more lines than DNX_VISIBILITY_BLOCK_MAX_NOF_LINES (%d), need to update the max\n",
                             dnx_visibility_block_map[unit][i_bl].name, DNX_VISIBILITY_BLOCK_MAX_NOF_LINES);
            }
            dnx_visibility_block_info[unit][i_bl]->width = dnx_data_debug.mem.params_get(unit, i_bl)->width;

            
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
            {
                
                dnx_visibility_block_info[unit][i_bl]->line_size =
                    BITS2BYTES(UTILEX_ALIGN_UP(dnx_visibility_block_info[unit][i_bl]->width, CHUNK_SIZE_BITS));
                
                dnx_visibility_block_info[unit][i_bl]->data_nof =
                    dnx_visibility_block_info[unit][i_bl]->line_size / BITS2BYTES(CHUNK_SIZE_BITS);
                
                for (i_line = 0; i_line < DNX_VISIBILITY_BLOCK_MAX_NOF_LINES; i_line++)
                {
                    if (i_line < dnx_visibility_block_info[unit][i_bl]->line_nof)
                    {
                        if ((dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core_idx][i_line] =
                             utilex_alloc(dnx_visibility_block_info[unit][i_bl]->line_size)) == NULL)
                        {
                            SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation of %d for line %d in %s block failed\n",
                                         dnx_visibility_block_info[unit][i_bl]->line_size,
                                         i_line, dnx_visibility_block_map[unit][i_bl].name);
                        }
                    }
                    else
                    {
                        dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core_idx][i_line] = NULL;
                    }
                    dnx_visibility_block_info[unit][i_bl]->sig_cash_status[core_idx][i_line] =
                        DNX_VISIBILITY_SIG_CACHE_STATUS_EMPTY;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_debug_cache_deinit(
    int unit)
{
    int i_bl, i_line;
    int core_idx;
    uint32 block_max_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_visibility_dbal_enum_max_value_get(unit, DBAL_FIELD_PP_ASIC_BLOCK, &block_max_index));

    
    if (dnx_visibility_block_map[unit] != NULL)
    {
        for (i_bl = 0; i_bl < block_max_index; i_bl++)
        {
            utilex_free(dnx_visibility_block_map[unit][i_bl].name);
        }
        utilex_free(dnx_visibility_block_map[unit]);
        dnx_visibility_block_map[unit] = NULL;
    }

    
    if (mem_name_per_block_array[unit] != NULL)
    {
        for (i_bl = 0; i_bl < block_max_index; i_bl++)
        {
            utilex_free(mem_name_per_block_array[unit][i_bl]);
        }
        utilex_free(mem_name_per_block_array[unit]);
        mem_name_per_block_array[unit] = NULL;
    }

    if (dnx_visibility_block_info[unit] != NULL)
    {
        for (i_bl = 0; i_bl < block_max_index; i_bl++)
        {
            
            if (dnx_visibility_block_info[unit][i_bl] != NULL)
            {
                DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
                {
                    for (i_line = 0; i_line < DNX_VISIBILITY_BLOCK_MAX_NOF_LINES; i_line++)
                    {
                        if (dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core_idx][i_line] != NULL)
                        {
                            utilex_free(dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core_idx][i_line]);
                            dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core_idx][i_line] = NULL;
                        }
                    }
                }
                utilex_free(dnx_visibility_block_info[unit][i_bl]);
                dnx_visibility_block_info[unit][i_bl] = NULL;
            }
        }
        utilex_free(dnx_visibility_block_info[unit]);
        dnx_visibility_block_info[unit] = NULL;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_debug_pema_fetch(
    int unit,
    int core,
    int direction,
    dbal_enum_value_field_pema_id_e pema_id,
    int window,
    uint32 *value_array)
{
    uint32 entry_handle_id;
    int size;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMA_DEBUG_SIGNALS_WINDOW_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PEMA_ID, pema_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PEMA_DIRECTION, INST_SINGLE, direction);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PEMA_WINDOW, INST_SINGLE, window);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMA_DEBUG_SIGNALS_DATA, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PEMA_ID, pema_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_PEMA_DEBUG_SIGNALS_DATA, DBAL_FIELD_PEMA_DATA,
                                               FALSE, 0, 0, &size));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_PEMA_DATA, 0,
                                                            &value_array[0]));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_PEMA_DATA, 1,
                                                            &value_array[BITS2WORDS(size)]));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_debug_pema_read(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int direction,
    dbal_enum_value_field_pema_id_e pema_id,
    int window,
    uint32 *value_array)
{
    int line_index;

    SHR_FUNC_INIT_VARS(unit);
    
    SHR_NULL_CHECK(value_array, _SHR_E_PARAM, "value_array");
    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");
    DNXCMN_CORE_VALIDATE(unit, core, 0);

    line_index = DNX_VISIBILITY_ENGINE_CALC_IDX(pema_id, direction, window);

    
    if (dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index] ==
        DNX_VISIBILITY_SIG_CACHE_STATUS_EMPTY)
    {
        SHR_IF_ERR_EXIT(dnx_debug_pema_fetch(unit, core, direction, pema_id, window,
                                             dnx_visibility_block_info[unit][asic_block]->sig_cash_p[core]
                                             [line_index]));
        dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index] =
            DNX_VISIBILITY_SIG_CACHE_STATUS_VALID;
    }

    if (dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index] ==
        DNX_VISIBILITY_SIG_CACHE_STATUS_VALID)
    {
        
        sal_memcpy(value_array, dnx_visibility_block_info[unit][asic_block]->sig_cash_p[core][line_index],
                   dnx_visibility_block_info[unit][asic_block]->line_size);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_EMPTY, "PEM cache not valid");
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_debug_reg_read(
    int unit,
    int core,
    char *hw_name,
    int size,
    uint32 *value_array)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_fields_e field_id;
    char **tokens = NULL;
    uint32 realtokens = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_NULL_CHECK(value_array, _SHR_E_PARAM, "value_array");
    DNXCMN_CORE_VALIDATE(unit, core, 0);

    
    tokens = utilex_str_split(hw_name, ".", 2, &realtokens);
    if (realtokens != 2 || (tokens == NULL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong dbal format:%s for Debug Signal\n", hw_name);
    }

    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, tokens[0], &table_id));
    SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, tokens[1], &field_id));
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    
    if (size > 32)
    {
        int dbal_field_size;
        
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_id, field_id, FALSE, 0, 0, &dbal_field_size));

        if (size > dbal_field_size)
        {   
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, 0, value_array));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, 1, value_array
                                                                    + BITS2WORDS(dbal_field_size)));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                                    value_array));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_id, INST_SINGLE, value_array));
    }
exit:
    utilex_str_split_free(tokens, realtokens);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_debug_mem_fetch(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index)
{
    uint32 *data_ptr;
    int block;
    int num_of_blocks;
    uint32 temp_buffer[DNX_VISIBILITY_MAX_MEM_IN_WORDS];
    int mem_array_index = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    mem_array_index = dnx_data_debug.general.mem_array_index_get(unit);
    num_of_blocks = (SOC_MEM_BLOCK_MAX(unit, mem_name_per_block_array[unit][asic_block][mem_array_index])
                     - SOC_MEM_BLOCK_MIN(unit, mem_name_per_block_array[unit][asic_block][mem_array_index]) + 1);
    
    if (DBAL_MAX_NUM_OF_CORES != 1)
    {
        num_of_blocks = (num_of_blocks + 1) / DBAL_MAX_NUM_OF_CORES;
    }

    block =
        SOC_MEM_BLOCK_MIN(unit, mem_name_per_block_array[unit][asic_block][mem_array_index]) + (core * num_of_blocks);

    data_ptr = dnx_visibility_block_info[unit][asic_block]->sig_cash_p[core][line_index];

    SHR_IF_ERR_EXIT_WITH_LOG(soc_mem_array_read
                             (unit, mem_name_per_block_array[unit][asic_block][mem_array_index], 0, block, line_index,
                              temp_buffer), "Failed reading memory %s port=%d arr_ind=%d", SOC_MEM_NAME(unit,
                                                                                                        mem_name_per_block_array
                                                                                                        [unit]
                                                                                                        [asic_block]
                                                                                                        [mem_array_index]),
                             block, 0);

    sal_memcpy(data_ptr, temp_buffer, dnx_visibility_block_info[unit][asic_block]->line_size);

exit:
    DBAL_FUNC_FREE_VARS;
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}



static shr_error_e
dnx_debug_mem_clear(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index)
{
    uint32 temp_buffer[DNX_VISIBILITY_MAX_MEM_IN_WORDS] = { 0 };
    int mem_array_index = 0;
    soc_mem_t memory;
    int array_index;
    int block;
    int num_of_blocks;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_SUPPRESS(unit);

    mem_array_index = dnx_data_debug.general.mem_array_index_get(unit);
    memory = mem_name_per_block_array[unit][asic_block][mem_array_index];
    if (memory == 0)
    {
        SHR_EXIT();
    }
    array_index = 0;

    num_of_blocks = (SOC_MEM_BLOCK_MAX(unit, mem_name_per_block_array[unit][asic_block][mem_array_index])
                     - SOC_MEM_BLOCK_MIN(unit, mem_name_per_block_array[unit][asic_block][mem_array_index]) + 1);
    
    if (DBAL_MAX_NUM_OF_CORES != 1)
    {
        num_of_blocks = (num_of_blocks + 1) / DBAL_MAX_NUM_OF_CORES;
    }

    block =
        SOC_MEM_BLOCK_MIN(unit, mem_name_per_block_array[unit][asic_block][mem_array_index]) + (core * num_of_blocks);

    SHR_IF_ERR_EXIT_WITH_LOG(soc_mem_array_write(unit,
                                                 memory,
                                                 array_index,
                                                 block,
                                                 line_index,
                                                 temp_buffer),
                             "Failed clearing memory %s arr_ind=%d block=%d", SOC_MEM_NAME(unit, memory), array_index,
                             block);

exit:
    DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    SHR_FUNC_EXIT;
}

#ifdef ADAPTER_SERVER_MODE

static shr_error_e
dnx_debug_mem_get(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index,
    uint32 *reset_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dnx_visibility_block_map[unit][asic_block].table, &entry_handle_id));

    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LINE, line_index);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SIGRESET, INST_SINGLE, reset_val);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif


shr_error_e
dnx_debug_mem_line_is_ready(
    int unit,
    int core_id,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index)
{
    uint32 entry_handle_id;
    uint32 block_valids[2] = { 0, 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");
    DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block);
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    
    if (core_id == DBAL_CORE_ALL)
    {
        core_id = DBAL_CORE_DEFAULT;
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (dnx_visibility_block_info[unit][asic_block]->line_nof > 32)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                dnx_visibility_block_map[unit][asic_block].valids_field,
                                                                INST_SINGLE, block_valids));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                            dnx_visibility_block_map[unit][asic_block].valids_field,
                                                            INST_SINGLE, block_valids));
    }

    if (((line_index < 32) && ((block_valids[0] & SAL_BIT(line_index)) == 0)) ||
        ((line_index >= 32) && ((block_valids[1] & SAL_BIT(line_index - 32)) == 0)))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_debug_mem_read(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index,
    uint32 **data_ptr)
{
    shr_error_e ready_status = _SHR_E_EMPTY;
    int visibility_mode;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_NULL_CHECK(data_ptr, _SHR_E_PARAM, "value_array");
    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");
    DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block);
    DNXCMN_CORE_VALIDATE(unit, core, 0);

    if (line_index >= dnx_visibility_block_info[unit][asic_block]->line_nof)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Line Index:%d is out of range(%d) for:%s\n", line_index,
                     dnx_visibility_block_info[unit][asic_block]->line_nof, dnx_debug_mem_name(unit, asic_block));
    }

    SHR_IF_ERR_EXIT(visibility_info_db.visib_mode.get(unit, &visibility_mode));

    
    if ((dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index] ==
         DNX_VISIBILITY_SIG_CACHE_STATUS_VALID)
        || (dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index] ==
            DNX_VISIBILITY_SIG_CACHE_STATUS_VALUE_NA))
    {
        
        if (visibility_mode == bcmInstruVisModeAlways)
        {
            if ((ready_status = dnx_debug_mem_line_is_ready(unit, core, asic_block, line_index)) == _SHR_E_NONE)
            {
                SHR_IF_ERR_EXIT(dnx_debug_mem_fetch(unit, core, asic_block, line_index));
            }
        }
    }
    else if ((ready_status = dnx_debug_mem_line_is_ready(unit, core, asic_block, line_index)) == _SHR_E_NONE)
    {
        
        SHR_IF_ERR_EXIT(dnx_debug_mem_fetch(unit, core, asic_block, line_index));
        dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index] =
            DNX_VISIBILITY_SIG_CACHE_STATUS_VALID;
    }

    
    if ((visibility_mode == bcmInstruVisModeAlways) && (ready_status == _SHR_E_NONE))
    {
        SHR_IF_ERR_EXIT(dnx_debug_mem_clear(unit, core, asic_block, line_index));
    }

    if (dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index] ==
        DNX_VISIBILITY_SIG_CACHE_STATUS_VALID)
    {
        
        *data_ptr = dnx_visibility_block_info[unit][asic_block]->sig_cash_p[core][line_index];
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_selective_fetch_all(
    int unit,
    int core)
{
    int visibility_mode;
    uint32 i_bl, i_line;
    uint32 entry_handle_id;
    uint32 line_valids[2];
    uint32 block_max_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(visibility_info_db.visib_mode.get(unit, &visibility_mode));

    if (visibility_mode != bcmInstruVisModeSelective)
    {
#ifdef DNX_EMULATION
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "On Emulation, signals can be read in selective mode only - Change mode and resend packet!!\n");
#endif
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_visibility_dbal_enum_max_value_get(unit, DBAL_FIELD_PP_ASIC_BLOCK, &block_max_index));
    for (i_bl = 0; i_bl < block_max_index; i_bl++)
    {
        if (dnx_visibility_block_map[unit][i_bl].table == DBAL_TABLE_EMPTY)
        {
            continue;
        }
        line_valids[0] = 0;
        line_valids[1] = 0;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_value_field_arr32_request(unit, entry_handle_id, dnx_visibility_block_map[unit][i_bl].valids_field,
                                       INST_SINGLE, line_valids);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        for (i_line = 0; i_line < dnx_visibility_block_info[unit][i_bl]->line_nof; i_line++)
        {
            if (dnx_visibility_block_info[unit][i_bl]->sig_cash_status[core][i_line] ==
                DNX_VISIBILITY_SIG_CACHE_STATUS_EMPTY)
            {
                if (utilex_bitstream_test_bit(line_valids, i_line))
                {
                    SHR_IF_ERR_EXIT(dnx_debug_mem_fetch(unit, core, i_bl, i_line));
                    dnx_visibility_block_info[unit][i_bl]->sig_cash_status[core][i_line] =
                        DNX_VISIBILITY_SIG_CACHE_STATUS_VALID;
                }
                else
                {
                    dnx_visibility_block_info[unit][i_bl]->sig_cash_status[core][i_line] =
                        DNX_VISIBILITY_SIG_CACHE_STATUS_VALUE_NA;
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_debug_mem_lines(
    int unit,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int *size_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");
    DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block);

    *size_p = dnx_visibility_block_info[unit][asic_block]->line_nof;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_debug_mem_cache(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index,
    int *status_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");
    DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block);

    *status_p = dnx_visibility_block_info[unit][asic_block]->sig_cash_status[core][line_index];

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_debug_mem_is_ready(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block)
{
    uint32 entry_handle_id;
    uint32 block_valids[2] = { 0, 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_visibility_block_info[unit][asic_block], _SHR_E_PARAM, "dnx_visibility_block_info");
    DNX_PP_ASIC_BLOCK_VERIFY(unit, asic_block);
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    
    if (core == DBAL_CORE_ALL)
    {
        core = DBAL_CORE_DEFAULT;
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (dnx_visibility_block_info[unit][asic_block]->line_nof > 32)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                dnx_visibility_block_map[unit][asic_block].valids_field,
                                                                INST_SINGLE, block_valids));
        if ((block_valids[0] == 0) && (block_valids[1] == 0))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                            dnx_visibility_block_map[unit][asic_block].valids_field,
                                                            INST_SINGLE, block_valids));
        if (block_valids[0] == 0)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_debug_block_is_ready(
    int unit,
    int core,
    dnx_pp_block_e pp_block)
{
#ifdef ADAPTER_SERVER_MODE
    rhlist_t *dsig_list = NULL;
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    {
        match_t match_m;

        sal_memset(&match_m, 0, sizeof(match_t));

        if ((dsig_list = utilex_rhlist_create("prt_print", sizeof(signal_output_t), 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
        }

        match_m.block = dnx_pp_block_name(unit, pp_block);

        SHR_SET_CURRENT_ERR(sand_signal_get(unit, 0, &match_m, dsig_list, NULL));
        if (!SHR_FUNC_VAL_IS(_SHR_E_NONE) || (RHLNUM(dsig_list) == 0))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        }
    }
#else
    {
        dbal_enum_value_field_pp_asic_block_e asic_block;

        switch (pp_block)
        {
            case DNX_PP_BLOCK_IRPP:
                asic_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
                break;
            case DNX_PP_BLOCK_ITPP:
                
                asic_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA;
                break;
            case DNX_PP_BLOCK_ERPP:
                asic_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ERPP;
                break;
            case DNX_PP_BLOCK_ETPP:
                asic_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPA;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid pp block:%d\n", pp_block);
                break;
        }
        SHR_SET_CURRENT_ERR(dnx_debug_mem_is_ready(unit, core, asic_block));
    }
#endif
exit:
#ifdef ADAPTER_SERVER_MODE
    sand_signal_list_free(dsig_list);
#endif
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_visibility_resume(
    int unit,
    int core,
    uint32 flags)
{
    int core_idx;
    bcm_instru_vis_mode_control_t mode = -1;
    int i_line, i_bl;
    uint32 block_max_index;
#ifdef ADAPTER_SERVER_MODE
    uint32 reset_val = 0;
#endif
    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    
    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_debug_mem_clear(unit, core_idx, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA, 0));
        SHR_IF_ERR_EXIT(dnx_debug_mem_get(unit, core_idx, DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA, 0, &reset_val));
        if (reset_val != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result from Adapter visibility register");
        }
    }
    SHR_EXIT();
#endif

    
    
    SHR_IF_ERR_EXIT(dnx_visibility_mode_get(unit, &mode, FALSE));
    
    if (mode == bcmInstruVisModeSelective)
    {
        SHR_IF_ERR_EXIT(dnx_visibility_mode_set(unit, bcmInstruVisModeNone));
    }

    SHR_IF_ERR_EXIT(dnx_visibility_dbal_enum_max_value_get(unit, DBAL_FIELD_PP_ASIC_BLOCK, &block_max_index));
    DNXCMN_CORES_ITER(unit, core, core_idx)
    {
        
        for (i_bl = 0; i_bl < block_max_index; i_bl++)
        {
            if (!(flags & dnx_visibility_block_map[unit][i_bl].direction))
            {
                continue;
            }
            for (i_line = 0; i_line < dnx_visibility_block_info[unit][i_bl]->line_nof; i_line++)
            {
                SHR_IF_ERR_EXIT(dnx_debug_mem_clear(unit, core_idx, i_bl, i_line));
            }
        }
        
        for (i_bl = 0; i_bl < block_max_index; i_bl++)
        {
            if (!(flags & dnx_visibility_block_map[unit][i_bl].direction))
            {
                continue;
            }
            for (i_line = 0; i_line < dnx_visibility_block_info[unit][i_bl]->line_nof; i_line++)
            {
                if (flags & DNX_VISIBILITY_RESUME_ZERO)
                {
                    sal_memset(dnx_visibility_block_info[unit][i_bl]->sig_cash_p[core_idx][i_line], 0,
                               dnx_visibility_block_info[unit][i_bl]->line_size);
                }
                dnx_visibility_block_info[unit][i_bl]->sig_cash_status[core_idx][i_line] =
                    DNX_VISIBILITY_SIG_CACHE_STATUS_EMPTY;
            }
        }
    }

exit:
    
    if (mode == bcmInstruVisModeSelective)
    {
        SHR_SET_CURRENT_ERR(dnx_visibility_mode_set(unit, mode));
    }
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_mode_set(
    int unit,
    bcm_instru_vis_mode_control_t mode)
{
    uint32 entry_handle_id;
    int always_status, selective_status;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    switch (mode)
    {
        case bcmInstruVisModeAlways:
            always_status = TRUE;
            selective_status = FALSE;
            break;
        case bcmInstruVisModeSelective:
            always_status = FALSE;
            selective_status = TRUE;
            break;
        case bcmInstruVisModeNone:
            always_status = FALSE;
            selective_status = FALSE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown visibility mode:%d\n", mode);
            break;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

    if (dnx_data_debug.general.has_etpp_visibility_enabler_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_ENABLE,
                                     INST_SINGLE, selective_status);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_ENABLE,
                                     INST_SINGLE, selective_status);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_GLOBAL_ENABLE,
                                     INST_SINGLE, selective_status);
    }

    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_ENABLE,
                                     INST_SINGLE, selective_status);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPA_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPB_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPC_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPD_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPE_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPPF_ALWAYS_ENABLE, INST_SINGLE, always_status);

    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_ALWAYS_ENABLE, INST_SINGLE, always_status);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPPA_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPPB_ALWAYS_ENABLE, INST_SINGLE, always_status);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPPC_ALWAYS_ENABLE, INST_SINGLE, always_status);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(visibility_info_db.visib_mode.set(unit, mode));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_mode_get(
    int unit,
    bcm_instru_vis_mode_control_t * mode_p,
    int full_flag)
{
    uint32 entry_handle_id;
    uint32 always_status = SAL_UINT32_MAX, selective_status = SAL_UINT32_MAX, tmp_status;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(mode_p, _SHR_E_PARAM, "mode_p");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    
    if (dnx_data_debug.general.has_etpp_visibility_enabler_get(unit))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_ENABLE,
                                                            INST_SINGLE, &tmp_status));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_GLOBAL_ENABLE, INST_SINGLE, &tmp_status));
    }
    DISCERN_STATUS(selective_status, tmp_status, "Ingress Selective Status");
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_IPPA_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
    DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");
    
    if (full_flag == TRUE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPPB_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPPC_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPPD_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPPE_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IPPF_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Ingress Always Status");

        if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_ENABLE, INST_SINGLE, &tmp_status));
            DISCERN_STATUS(selective_status, tmp_status, "Egress Selective Status");
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ERPP_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
            DISCERN_STATUS(always_status, tmp_status, "Egress Always Status");
        }

        if (dnx_data_debug.general.has_etpp_visibility_enabler_get(unit))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_ENABLE, INST_SINGLE, &tmp_status));
            DISCERN_STATUS(selective_status, tmp_status, "Egress Selective Status");
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPPA_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Egress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPPB_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Egress Always Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPPC_ALWAYS_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(always_status, tmp_status, "Egress Always Status");
    }
    if (selective_status == 1)
    {
        if (always_status == 1)
        {
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Both Always and Selective Modes are Set\n");
        }
        *mode_p = bcmInstruVisModeSelective;
    }
    else if (always_status == 1)
    {
        *mode_p = bcmInstruVisModeAlways;
    }
    else
    {
        *mode_p = bcmInstruVisModeNone;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_sampling_set(
    int unit,
    uint32 direction,
    uint32 period)
{
    uint32 entry_handle_id;
    int enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_PP_VERIFY_DIRECTION(unit, direction);
    
    if (period != 0)
    {
        enable = 1;
    }
    else
    {
        enable = 0;
    }

    if (dnx_data_debug.general.has_periodic_visibility_get(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        
        if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_SAMPLING_ENABLE,
                                         INST_SINGLE, enable);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_SAMPLING_PERIOD,
                                         INST_SINGLE, period);
        }

        if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
        {
            if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_SAMPLING_ENABLE,
                                             INST_SINGLE, enable);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_SAMPLING_PERIOD,
                                             INST_SINGLE, period);
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_SAMPLING_ENABLE,
                                         INST_SINGLE, enable);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_SAMPLING_PERIOD,
                                         INST_SINGLE, period);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_visibility_sampling_get(
    int unit,
    uint32 direction,
    uint32 *period_p)
{
    uint32 entry_handle_id;
    uint32 status = SAL_UINT32_MAX, tmp_status;
    uint32 period = SAL_UINT32_MAX, tmp_period;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(period_p, _SHR_E_PARAM, "period_p");
    DNX_PP_VERIFY_DIRECTION(unit, direction);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_SAMPLING_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(status, tmp_status, "Sampling Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_SAMPLING_PERIOD, INST_SINGLE, &tmp_period));
        DISCERN_STATUS(period, tmp_period, "Sampling Period");
    }

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_SAMPLING_ENABLE, INST_SINGLE,
                             &tmp_status));
            DISCERN_STATUS(status, tmp_status, "Sampling Status");
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_SAMPLING_PERIOD, INST_SINGLE,
                             &tmp_period));
            DISCERN_STATUS(period, tmp_period, "Sampling Period");
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_SAMPLING_ENABLE, INST_SINGLE, &tmp_status));
        DISCERN_STATUS(status, tmp_status, "Sampling Status");
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_SAMPLING_PERIOD, INST_SINGLE, &tmp_period));
        DISCERN_STATUS(period, tmp_period, "Sampling Period");
    }

    if (status == 0)
        *period_p = 0;
    else
        *period_p = period;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_pp_port_enable_set(
    int unit,
    int core,
    int pp_port,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    if ((enable != FALSE) && (enable != TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal enable value:%d\n", enable);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_ENABLE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_pp_port_enable_get(
    int unit,
    int core,
    int pp_port,
    int *enable_p)
{
    uint32 entry_handle_id;
    uint32 enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_ENABLE,
                                                        INST_SINGLE, &enable));
    *enable_p = enable;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_visibility_pp_dsp_enable_set(
    int unit,
    int core,
    int pp_dsp,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    if ((enable != FALSE) && (enable != TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal enable value:%d\n", enable);
    }
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_PP_DSP_TABLE, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, pp_dsp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    
    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_ENABLE, INST_SINGLE, enable);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_visibility_pp_dsp_enable_get(
    int unit,
    int core,
    int pp_dsp,
    int *enable_p)
{
    uint32 entry_handle_id;
    uint32 erpp_enable = 0, etpp_enable = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_PP_DSP_TABLE, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, pp_dsp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    
    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_ENABLE, INST_SINGLE, &erpp_enable));
    }
    if (dnx_data_debug.general.has_etpp_visibility_enabler_get(unit))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_ENABLE, INST_SINGLE, &etpp_enable));
    }

    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp) &&
        dnx_data_debug.general.has_etpp_visibility_enabler_get(unit))
    {
        DISCERN_STATUS(etpp_enable, erpp_enable, "ERPP and ETPP visibility Enable statues are Inconsistent");
    }

    *enable_p = erpp_enable;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_visibility_port_ingress_verify(
    int unit,
    bcm_gport_t gport)
{
    int port_is_in_lag = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!((BCM_GPORT_IS_LOCAL(gport) || ((gport >= 0) && (gport < SOC_MAX_NUM_PORTS))) || BCM_GPORT_IS_TRUNK(gport)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Port %d is neither local nor trunk gport! It should be used local port or trunk gport!\n", gport);
    }

    if (!BCM_GPORT_IS_TRUNK(gport))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, gport, &port_is_in_lag));
        if (port_is_in_lag)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Ingress direction, port cannot be trunk member, gport=0x%08x. It should be used trunk gport instead.\n",
                         gport);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_port_enable_set(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 gpm_gport_flags;

    SHR_FUNC_INIT_VARS(unit);

    gpm_gport_flags = DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE;

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_visibility_port_ingress_verify(unit, gport));
        gpm_gport_flags |= DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY;
    }

    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS) && (BCM_GPORT_IS_TRUNK(gport)))
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress direction, port cannot be trunk, gport=0x%08x\n", gport);
    }
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, gpm_gport_flags, &gport_info));

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        uint32 pp_port_index;
        
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set(unit,
                                                              gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                              gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                              enable));
        }
    }
    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS) &&
        (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp) ||
         dnx_data_debug.general.has_etpp_visibility_enabler_get(unit)))
    {
        uint32 pp_dsp;
        dnx_algo_port_info_s port_info;
        bcm_core_t core;
        
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info.local_port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_PP_DSP(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, gport_info.local_port, &core, &pp_dsp));
            SHR_IF_ERR_EXIT(dnx_visibility_pp_dsp_enable_set(unit, core, pp_dsp, enable));
        }
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_port_enable_get(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int *enable_p)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int enable = SAL_UINT32_MAX, cur_enable;
    uint32 gpm_gport_flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");
    DNX_PP_VERIFY_DIRECTION(unit, direction);

    gpm_gport_flags = DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE;

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_visibility_port_ingress_verify(unit, gport));
        gpm_gport_flags |= DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY;
    }

    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS) && (BCM_GPORT_IS_TRUNK(gport)))
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress direction, port cannot be trunk, gport=0x%08x\n", gport);
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, gpm_gport_flags, &gport_info));

    
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        uint32 pp_port_index;
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_get(unit,
                                                              gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                              gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                              &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "Ingress Port Enable Status");
        }
    }
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_PP_DSP_TABLE, &entry_handle_id));
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        uint32 pp_dsp;
        dnx_algo_port_info_s port_info;
        bcm_core_t core;
        
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info.local_port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_PP_DSP(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, gport_info.local_port, &core, &pp_dsp));
            SHR_IF_ERR_EXIT(dnx_visibility_pp_dsp_enable_get(unit, core, pp_dsp, &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "Ingress and Egress visibility enable Status are inconsistent");
        }
    }
    *enable_p = enable;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_pp_port_force_set(
    int unit,
    int core,
    int pp_port,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    if ((enable != FALSE) && (enable != TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal enable value:%d\n", enable);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_FORCE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_pp_port_force_get(
    int unit,
    int core,
    int pp_port,
    int *enable_p)
{
    uint32 entry_handle_id;
    uint32 enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_FORCE,
                                                        INST_SINGLE, &enable));

    *enable_p = enable;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_visibility_pp_dsp_force_set(
    int unit,
    int core,
    int pp_dsp,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    if ((enable != FALSE) && (enable != TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal enable value:%d\n", enable);
    }
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_PP_DSP_TABLE, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, pp_dsp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    
    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_FORCE, INST_SINGLE, enable);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_FORCE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_visibility_pp_dsp_force_get(
    int unit,
    int core,
    int pp_dsp,
    int *enable_p)
{
    uint32 entry_handle_id;
    uint32 erpp_force = 0, etpp_force = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_PP_DSP_TABLE, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, pp_dsp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    
    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_FORCE, INST_SINGLE, &erpp_force));
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_FORCE, INST_SINGLE, &etpp_force));

    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
    {
        DISCERN_STATUS(etpp_force, erpp_force, "ERPP and ETPP visibility Force statues are Inconsistent");
    }

    *enable_p = erpp_force;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_port_force_set(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int enable)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 gpm_gport_flags;

    SHR_FUNC_INIT_VARS(unit);

    gpm_gport_flags = DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE;

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_visibility_port_ingress_verify(unit, gport));
        gpm_gport_flags |= DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY;
    }

    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS) && (BCM_GPORT_IS_TRUNK(gport)))
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress direction, port cannot be trunk, gport=0x%08x\n", gport);
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, gpm_gport_flags, &gport_info));
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        uint32 pp_port_index;
        
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(dnx_visibility_pp_port_force_set(unit,
                                                             gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                             gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                             enable));
        }
    }
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        uint32 pp_dsp;
        dnx_algo_port_info_s port_info;
        bcm_core_t core;

        
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info.local_port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_PP_DSP(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, gport_info.local_port, &core, &pp_dsp));
            SHR_IF_ERR_EXIT(dnx_visibility_pp_dsp_force_set(unit, core, pp_dsp, enable));
        }
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_port_force_get(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int *enable_p)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;
    int enable = SAL_UINT32_MAX, cur_enable = SAL_UINT32_MAX;
    uint32 gpm_gport_flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable_p, _SHR_E_PARAM, "enable_p");
    DNX_PP_VERIFY_DIRECTION(unit, direction);

    gpm_gport_flags = DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE;

    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_visibility_port_ingress_verify(unit, gport));
        gpm_gport_flags |= DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY;
    }

    if ((direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS) && (BCM_GPORT_IS_TRUNK(gport)))
    {
        
        SHR_ERR_EXIT(_SHR_E_PARAM, "Egress direction, port cannot be trunk, gport=0x%08x\n", gport);
    }

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport, gpm_gport_flags, &gport_info));
    
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            SHR_IF_ERR_EXIT(dnx_visibility_pp_port_force_get(unit,
                                                             gport_info.internal_port_pp_info.core_id[pp_port_index],
                                                             gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                             &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "Ingress and Egress visibility enable Status are inconsistent");
        }
    }
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_PP_DSP_TABLE, &entry_handle_id));
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        bcm_core_t core;
        uint32 pp_dsp;
        dnx_algo_port_info_s port_info;
        
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info.local_port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_PP_DSP(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_dsp_get(unit, gport_info.local_port, &core, &pp_dsp));
            SHR_IF_ERR_EXIT(dnx_visibility_pp_dsp_force_get(unit, core, pp_dsp, &cur_enable));
            DISCERN_STATUS(enable, cur_enable, "Ingress Port Force Status");
        }
    }
    *enable_p = enable;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



static shr_error_e
dnx_visibility_prt_qualifier_set(
    int unit,
    int prt_qualifier,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    
    if ((enable != FALSE) && (enable != TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal enable value:%d\n", enable);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PRT_QUALIFIER_INFO, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER, prt_qualifier);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_ENABLE, INST_SINGLE, enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_prt_qualifier_get(
    int unit,
    uint32 *prt_qualifier_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_PRT_QUALIFIER,
                                                  DBAL_ENUM_FVAL_PRT_QUALIFIER_SET_VISIBILITY, prt_qualifier_p));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_port_egress_enable(
    int unit)
{
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));

    SHR_IF_ERR_EXIT(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, port, TRUE));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_port_egress_disable(
    int unit)
{
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));

    SHR_IF_ERR_EXIT(dnx_visibility_port_enable_set(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, port, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_visibility_port_init_all(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_ENABLE, INST_SINGLE, FALSE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VISIBILITY_FORCE, INST_SINGLE, FALSE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp) ||
        dnx_data_debug.general.has_etpp_visibility_enabler_get(unit))
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PP_ATTRIBUTES_PER_PP_DSP_TABLE, entry_handle_id));

        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PP_DSP, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);

        if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
        {
            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_ENABLE, INST_SINGLE, FALSE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_VISIBILITY_FORCE, INST_SINGLE, FALSE);
        }

        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_ENABLE, INST_SINGLE, FALSE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_VISIBILITY_FORCE, INST_SINGLE, FALSE);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_visibility_global_set(
    int unit,
    int is_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_VISIBILITY_CFG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IRPP_VISIBILITY_GLOBAL_ENABLE, INST_SINGLE,
                                 is_enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(dnx_debug_cache_init(unit));
    
    if (!SOC_WARM_BOOT(unit))
    {
        int i_qual;
        int enable;
        int core_idx;
        uint32 max_prt_qualifier;

        
        SHR_IF_ERR_EXIT(visibility_info_db.init(unit));
        SHR_IF_ERR_EXIT(dnx_visibility_port_init_all(unit));
        
        SHR_IF_ERR_EXIT(dnx_visibility_mode_set(unit, bcmInstruVisModeSelective));

        SHR_IF_ERR_EXIT(dnx_visibility_sampling_set(unit,
                                                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS |
                                                    BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, 0));

        
        SHR_IF_ERR_EXIT(dnx_visibility_dbal_enum_max_value_get(unit, DBAL_FIELD_PRT_QUALIFIER, &max_prt_qualifier));
        for (i_qual = 0; i_qual < max_prt_qualifier; i_qual++)
        {
            uint8 is_illegal_i_qual;
            SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_PRT_QUALIFIER, i_qual, &is_illegal_i_qual));
            if (is_illegal_i_qual)
            {
                continue;
            }

            if (i_qual == DBAL_ENUM_FVAL_PRT_QUALIFIER_SET_VISIBILITY)
                enable = TRUE;
            else
                enable = FALSE;

            SHR_IF_ERR_EXIT(dnx_visibility_prt_qualifier_set(unit, i_qual, enable));
        }

        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
        {
            
            uint32 i_bl;
            SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get(unit, DBAL_FIELD_PP_ASIC_BLOCK, INVALID_ENUM, &i_bl));
            while (i_bl != INVALID_ENUM)
            {
                int i_line;
                if (dnx_visibility_block_map[unit][i_bl].table == DBAL_TABLE_EMPTY)
                {
                    SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get(unit, DBAL_FIELD_PP_ASIC_BLOCK, i_bl, &i_bl));
                    continue;
                }
                for (i_line = 0; i_line < dnx_visibility_block_info[unit][i_bl]->line_nof; i_line++)
                {
                    SHR_IF_ERR_EXIT(dnx_debug_mem_clear(unit, core_idx, i_bl, i_line));
                }
                SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get(unit, DBAL_FIELD_PP_ASIC_BLOCK, i_bl, &i_bl));
            }
        }

        if (dnx_data_debug.general.global_visibility_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_visibility_global_set(unit, TRUE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_debug_cache_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_visibility_engine_clear(
    int unit,
    int core)
{
    int pema_idx, direction, win_idx;
    int line_index;
    int window_max_val, direction_max_val;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                    (unit, DBAL_TABLE_PEMA_DEBUG_SIGNALS_WINDOW_CONFIG, DBAL_FIELD_PEMA_DIRECTION, FALSE, 0, 0,
                     &direction_max_val));
    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get
                    (unit, DBAL_TABLE_PEMA_DEBUG_SIGNALS_WINDOW_CONFIG, DBAL_FIELD_PEMA_WINDOW, FALSE, 0, 0,
                     &window_max_val));

    if ((dnx_visibility_block_info[unit][DBAL_ENUM_FVAL_PP_ASIC_BLOCK_TO_ENGINE] != NULL)
        && (dnx_visibility_block_info[unit][DBAL_ENUM_FVAL_PP_ASIC_BLOCK_FROM_ENGINE] != NULL))
    {
        for (pema_idx = 0; pema_idx < DBAL_NOF_ENUM_PEMA_ID_VALUES; pema_idx++)
        {
            for (direction = 0; direction <= direction_max_val; direction++)
            {
                for (win_idx = 0; win_idx <= window_max_val; win_idx++)
                {
                    line_index = DNX_VISIBILITY_ENGINE_CALC_IDX(pema_idx, direction, win_idx);
                    dnx_visibility_block_info[unit][DBAL_ENUM_FVAL_PP_ASIC_BLOCK_TO_ENGINE]->sig_cash_status[core]
                        [line_index] = DNX_VISIBILITY_SIG_CACHE_STATUS_EMPTY;
                    dnx_visibility_block_info[unit][DBAL_ENUM_FVAL_PP_ASIC_BLOCK_FROM_ENGINE]->sig_cash_status[core]
                        [line_index] = DNX_VISIBILITY_SIG_CACHE_STATUS_EMPTY;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
