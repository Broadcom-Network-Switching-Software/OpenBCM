

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM



#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include "hbmc_dbal_access.h"
#include <bcm_int/dnx/cmn/dnxcmn.h>












static inline uint32
dnx_hbmc_ns_to_clock(
    int unit,
    uint32 ns)
{
    
    return ns * dnx_data_dram.general_info.frequency_get(unit) / 1000;
}


int
dnx_hbmc_get_sequential_channel(
    int unit,
    int hbm_index,
    int channel_in_hbm)
{
    return hbm_index * dnx_data_dram.general_info.nof_channels_get(unit) + channel_in_hbm;
}


shr_error_e
dnx_hbmc_dbal_access_phy_channel_register_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 address,
    uint32 data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_PHY_CH_REGISTER_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PHY_REG_ADDRESS, address);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PHY_CH_REGISTER_ACCESS, INST_SINGLE, data);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_phy_channel_register_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 address,
    uint32 *data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_PHY_CH_REGISTER_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PHY_REG_ADDRESS, address);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PHY_CH_REGISTER_ACCESS, INST_SINGLE, data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_phy_midstack_register_set(
    int unit,
    int hbm_index,
    uint32 address,
    uint32 data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PHY_MIDSTACK_REGISTER_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PHY_REG_ADDRESS, address);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PHY_CHM_REGISTER_ACCESS, INST_SINGLE, data);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_phy_midstack_register_get(
    int unit,
    int hbm_index,
    uint32 address,
    uint32 *data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PHY_MIDSTACK_REGISTER_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PHY_REG_ADDRESS, address);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PHY_CHM_REGISTER_ACCESS, INST_SINGLE, data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_bist_configuration_set(
    int unit,
    int hbm_index,
    int channel,
    const hbmc_shmoo_bist_info_t * info)
{
    uint32 entry_handle_id;
    
    uint32 *bist_num_actions = (uint32 *) &(info->bist_num_actions[0]);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_BIST_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REFRESH_ENABLE, INST_SINGLE,
                                 info->bist_refresh_enable);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_NUM_OF_COMMANDS, INST_SINGLE, bist_num_actions);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONSECUTIVE_WRITE_COMMANDS, INST_SINGLE,
                                 info->write_weight);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONSECUTIVE_READ_COMMANDS, INST_SINGLE,
                                 info->read_weight);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAME_ROW_COMMANDS, INST_SINGLE,
                                 info->same_row_commands);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_START, INST_SINGLE, info->bank_start_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK_END, INST_SINGLE, info->bank_end_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COLUMN_START, INST_SINGLE, info->column_start_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COLUMN_END, INST_SINGLE, info->column_end_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROW_START, INST_SINGLE, info->row_start_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROW_END, INST_SINGLE, info->row_end_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DRAM_BIST_MODE, INST_SINGLE, info->data_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_ADDRESS, INST_SINGLE,
                                 info->bist_ignore_address);

    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_PRBS_DATA_SEED, INST_SINGLE,
                                         (uint32 *) info->prbs_seeds);
    }
    else
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MISR_MODE, INST_SINGLE, 0x1);

        for (int seed_index = 0; seed_index < HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS; ++seed_index)
        {
            
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_SEED, seed_index,
                                         (info->prbs_seeds[seed_index] & 0xfffff));
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_BIST_CONFIG_DATA_PATTERN_PART1, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);

    for (int pattern_index = 0; pattern_index < HBMC_SHMOO_BIST_NOF_PATTERNS / 2; ++pattern_index)
    {
        
        uint32 *pattern_n = (uint32 *) &(info->data_pattern[pattern_index][0]);

        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_DATA_PATTERN, pattern_index, pattern_n);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_BIST_CONFIG_DATA_PATTERN_PART2, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);

    for (int pattern_index = HBMC_SHMOO_BIST_NOF_PATTERNS / 2; pattern_index < HBMC_SHMOO_BIST_NOF_PATTERNS;
         ++pattern_index)
    {
        
        uint32 *pattern_n = (uint32 *) &(info->data_pattern[pattern_index][0]);
        int pattern_index_in_table = pattern_index - (HBMC_SHMOO_BIST_NOF_PATTERNS / 2);

        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_DATA_PATTERN, pattern_index_in_table,
                                         pattern_n);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_bist_configuration_get(
    int unit,
    int hbm_index,
    int channel,
    hbmc_shmoo_bist_info_t * info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_BIST_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REFRESH_ENABLE, INST_SINGLE,
                               &info->bist_refresh_enable);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_NUM_OF_COMMANDS, INST_SINGLE,
                                   info->bist_num_actions);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONSECUTIVE_WRITE_COMMANDS, INST_SINGLE,
                               &info->write_weight);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CONSECUTIVE_READ_COMMANDS, INST_SINGLE,
                               &info->read_weight);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SAME_ROW_COMMANDS, INST_SINGLE,
                               &info->same_row_commands);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BANK_START, INST_SINGLE, &info->bank_start_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BANK_END, INST_SINGLE, &info->bank_end_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COLUMN_START, INST_SINGLE, &info->column_start_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COLUMN_END, INST_SINGLE, &info->column_end_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ROW_START, INST_SINGLE, &info->row_start_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ROW_END, INST_SINGLE, &info->row_end_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DRAM_BIST_MODE, INST_SINGLE, &info->data_mode);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_ADDRESS, INST_SINGLE,
                               &info->bist_ignore_address);

    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
    {
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_PRBS_DATA_SEED, INST_SINGLE, info->prbs_seeds);
    }
    else
    {
        for (int seed_index = 0; seed_index < HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS; ++seed_index)
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DATA_SEED, seed_index,
                                       &info->prbs_seeds[seed_index]);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_BIST_CONFIG_DATA_PATTERN_PART1, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);

    for (int pattern_index = 0; pattern_index < HBMC_SHMOO_BIST_NOF_PATTERNS / 2; ++pattern_index)
    {
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DATA_PATTERN, pattern_index,
                                       &(info->data_pattern[pattern_index][0]));
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_BIST_CONFIG_DATA_PATTERN_PART2, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);

    for (int pattern_index = HBMC_SHMOO_BIST_NOF_PATTERNS / 2; pattern_index < HBMC_SHMOO_BIST_NOF_PATTERNS;
         ++pattern_index)
    {
        int pattern_index_in_table = pattern_index - (HBMC_SHMOO_BIST_NOF_PATTERNS / 2);

        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_DATA_PATTERN, pattern_index_in_table,
                                       &(info->data_pattern[pattern_index][0]));
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_bist_status_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *read_cmd_counter,
    uint32 *write_cmd_counter,
    uint32 *read_data_counter)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_BIST_STATUS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_WRITE_COMMAND_CNT, INST_SINGLE, write_cmd_counter);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_READ_COMMAND_CNT, INST_SINGLE, read_cmd_counter);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_READ_DATA_CNT, INST_SINGLE, read_data_counter);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_bist_run(
    int unit,
    int hbm_index,
    int channel,
    uint32 start)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_BIST_RUN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIST_GO, INST_SINGLE, start ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_bist_run_done_poll(
    int unit,
    int hbm_index,
    int channel)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_BIST_RUN, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_BIST_GO, 0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_bist_write_lfsr_error_counters_get(
    int unit,
    int hbm_index,
    int channel,
    hbmc_shmoo_bist_err_cnt_t * error_counters)
{
    uint32 entry_handle_id;
    uint32 last_returned_data_0[4];
    uint32 last_returned_data_1[4];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_LAST_RETURNED_DATA, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_LAST_RETURNED_DATA, 0, last_returned_data_0);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_LAST_RETURNED_DATA, 1, last_returned_data_1);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    sal_memset(error_counters, 0, sizeof(*error_counters));
    if (last_returned_data_0[0] || last_returned_data_1[0] || last_returned_data_0[1] || last_returned_data_1[1])
    {
        
        error_counters->bist_data_err_bits[0] = error_counters->bist_data_err_bits[1] = 0xffffffff;
    }

    if (last_returned_data_0[2] || last_returned_data_1[2] || last_returned_data_0[3] || last_returned_data_1[3])
    {
        
        error_counters->bist_data_err_bits[2] = error_counters->bist_data_err_bits[3] = 0xffffffff;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_bist_error_counters_get(
    int unit,
    int hbm_index,
    int channel,
    hbmc_shmoo_bist_err_cnt_t * error_counters)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(error_counters, 0, sizeof(hbmc_shmoo_bist_err_cnt_t));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_BIST_ERR_COUNTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BIST_DATA_ERR_CNT, INST_SINGLE,
                               &error_counters->bist_data_err_cnt);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_BIST_DATA_ERR_BITMAP, INST_SINGLE,
                                   error_counters->bist_data_err_bits);
    if (!dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BIST_DM_ERR_CNT, INST_SINGLE,
                                   &error_counters->bist_dm_err_cnt);
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_BIST_DM_ERR_BITMAP, INST_SINGLE,
                                       error_counters->bist_dm_err_bits);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BIST_DBI_ERR_CNT, INST_SINGLE,
                                   &error_counters->bist_dbi_err_cnt);
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_BIST_DBI_ERR_BITMAP, INST_SINGLE,
                                       error_counters->bist_dbi_err_bits);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_channel_soft_init_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 in_soft_init)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CHANNEL_SOFT_INIT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_INIT, INST_SINGLE, ! !in_soft_init);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_channels_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CHANNEL_SOFT_INIT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_INIT, INST_SINGLE, ! !in_soft_init);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_channels_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CHANNEL_SOFT_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_RESET, INST_SINGLE, ! !in_soft_reset);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_channel_soft_init_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *in_soft_init)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CHANNEL_SOFT_INIT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_INIT, INST_SINGLE, in_soft_init);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hbmc_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_SOFT_INIT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_INIT, INST_SINGLE, ! !in_soft_init);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hbmc_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_SOFT_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_RESET, INST_SINGLE, ! !in_soft_reset);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hrc_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HRC_SOFT_INIT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_INIT, INST_SINGLE, ! !in_soft_init);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hrc_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HRC_SOFT_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_RESET, INST_SINGLE, ! !in_soft_reset);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_tdu_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDU_SOFT_INIT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_INIT, INST_SINGLE, ! !in_soft_init);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_tdu_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDU_SOFT_RESET, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_SOFT_RESET, INST_SINGLE, ! !in_soft_reset);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_enable_refresh_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 enable_refresh)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_TSM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_REFRESH_ENABLE, INST_SINGLE, enable_refresh);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_enable_refresh_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *enable_refresh)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_TSM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TSM_REFRESH_ENABLE, INST_SINGLE, enable_refresh);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_pll_config_set(
    int unit,
    int hbm_index,
    const hbmc_shmoo_pll_t * pll_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PLL_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PDIV, INST_SINGLE, pll_info->pdiv);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_NDIV_INT, INST_SINGLE, pll_info->ndiv_int);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_SSC_MODE, INST_SINGLE, pll_info->ssc_mode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_SSC_LIMIT, INST_SINGLE, pll_info->ssc_limit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_SSC_STEP, INST_SINGLE, pll_info->ssc_step);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_NDIV_P, INST_SINGLE, pll_info->ndiv_p);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_NDIV_Q, INST_SINGLE, pll_info->ndiv_q);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_NDIV_FRAC_MODE_SEL, INST_SINGLE,
                                 pll_info->ndiv_frac_mode_sel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_VCO_FB_DIV_2, INST_SINGLE,
                                 pll_info->vco_fb_div2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_AUX_POST_MDIV, INST_SINGLE,
                                 pll_info->mdiv_aux_post);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_0_MDIV, INST_SINGLE,
                                 pll_info->mdiv_ch[0]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_1_MDIV, INST_SINGLE,
                                 pll_info->mdiv_ch[1]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_2_MDIV, INST_SINGLE,
                                 pll_info->mdiv_ch[2]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_3_MDIV, INST_SINGLE,
                                 pll_info->mdiv_ch[3]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_4_MDIV, INST_SINGLE,
                                 pll_info->mdiv_ch[4]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_5_MDIV, INST_SINGLE,
                                 pll_info->mdiv_ch[5]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_FREFEFF_INFO, INST_SINGLE,
                                 pll_info->fref_eff_info);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_AUX_POST_ENABLEB, INST_SINGLE,
                                 pll_info->enableb_aux_post);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_ENABLEB, INST_SINGLE,
                                 pll_info->enableb_ch_bitmap);
        
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_AUX_POST_DIFFCMOS_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_FAST_LOCK, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_NDIV_RELOCK, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PWRON_PLL_AON, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PWRON_LDO_POSTDIV_AON, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PWRON_LDO_ANALOG_AON, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PWRON_LDO_DCORE_AON, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_RESERVED, INST_SINGLE, 0x30);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_POST_RST_HOLD_SEL, INST_SINGLE, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_DISABLE_WDT_RESET, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_POR_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_BYP_EN, INST_SINGLE, 4);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_pll_config_get(
    int unit,
    int hbm_index,
    hbmc_shmoo_pll_t * pll_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PLL_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PDIV, INST_SINGLE, &pll_info->pdiv);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_NDIV_INT, INST_SINGLE, &pll_info->ndiv_int);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_SSC_MODE, INST_SINGLE, &pll_info->ssc_mode);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_SSC_LIMIT, INST_SINGLE, &pll_info->ssc_limit);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_SSC_STEP, INST_SINGLE, &pll_info->ssc_step);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_NDIV_P, INST_SINGLE, &pll_info->ndiv_p);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_NDIV_Q, INST_SINGLE, &pll_info->ndiv_q);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_NDIV_FRAC_MODE_SEL, INST_SINGLE,
                               &pll_info->ndiv_frac_mode_sel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_VCO_FB_DIV_2, INST_SINGLE,
                               &pll_info->vco_fb_div2);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_AUX_POST_MDIV, INST_SINGLE,
                               &pll_info->mdiv_aux_post);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_0_MDIV, INST_SINGLE, &pll_info->mdiv_ch[0]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_1_MDIV, INST_SINGLE, &pll_info->mdiv_ch[1]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_2_MDIV, INST_SINGLE, &pll_info->mdiv_ch[2]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_3_MDIV, INST_SINGLE, &pll_info->mdiv_ch[3]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_4_MDIV, INST_SINGLE, &pll_info->mdiv_ch[4]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_5_MDIV, INST_SINGLE, &pll_info->mdiv_ch[5]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_FREFEFF_INFO, INST_SINGLE,
                               &pll_info->fref_eff_info);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_AUX_POST_ENABLEB, INST_SINGLE,
                               &pll_info->enableb_aux_post);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_ENABLEB, INST_SINGLE,
                               &pll_info->enableb_ch_bitmap);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_pll_configuration_for_apd_phy(
    int unit,
    int hbm_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PLL_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PWRON_LDO_ANALOG_AON, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PWRON_LDO_DCORE_AON, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PWRON_LDO_POSTDIV_AON, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PWRON_LDO_CML_2_CMOS_AON, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PWRON_LDO_DLL_AON, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_PWRON_PLL_AON, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_ISO_INOUT_AON, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_REFCLK_SOURCE_SEL, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_D_2C_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_SSC_MODE, INST_SINGLE, 1);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_NDIV_INT, INST_SINGLE,
                                 dnx_data_dram.apd_phy.pll_get(unit)->ndiv_int);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_CH_1_MDIV, INST_SINGLE,
                                 dnx_data_dram.apd_phy.pll_get(unit)->ch1_mdiv);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_pll_control_set(
    int unit,
    int hbm_index,
    uint32 reset,
    uint32 post_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PLL_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_RESETB, INST_SINGLE, reset ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PLL_POST_RESETB, INST_SINGLE,
                                 post_reset ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_pll_status_locked_poll(
    int unit,
    int hbm_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PLL_STATUS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 20000, 1000000, entry_handle_id, DBAL_FIELD_HBM_PLL_LOCK, 1));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_phy_control_out_of_reset_config(
    int unit,
    int hbm_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PHY_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);

    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PHY_POWER_RESET, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PHY_IO_RESET, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PHY_IDDQ, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_phy_channel_out_of_reset_config(
    int unit,
    int hbm_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_PHY_CHANNEL_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL_INDEX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PHY_CH_RESET, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PHY_CH_WRITE_FIFO_ENABLE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hbmc_dynamic_memory_access_set(
    int unit,
    int hbm_index,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_ENABLE_DYNAMIC_MEMORY_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_DYNAMIC_MEMORY_ACCESS, INST_SINGLE,
                                 enable ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hcc_dynamic_memory_access_set(
    int unit,
    int hbm_index,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_ENABLE_DYNAMIC_MEMORY_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_DYNAMIC_MEMORY_ACCESS, INST_SINGLE,
                                 enable ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_dynamic_memory_access_set(
    int unit,
    int hbm_index,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_hbmc_dynamic_memory_access_set(unit, hbm_index, enable));
    
    SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_hcc_dynamic_memory_access_set(unit, hbm_index, enable));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hcc_configuration_ecc_dbi_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *ecc,
    uint32 *dbi_read,
    uint32 *dbi_write)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_DBIAC_WRITE, INST_SINGLE, dbi_write);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_DBIAC_READ, INST_SINGLE, dbi_read);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_HBM_ECC, INST_SINGLE, ecc);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hcc_configuration_ecc_dbi_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 ecc,
    uint32 dbi_read,
    uint32 dbi_write)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_DBIAC_WRITE, INST_SINGLE, dbi_write);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_DBIAC_READ, INST_SINGLE, dbi_read);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_ECC, INST_SINGLE, ecc);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hcc_hbm_config(
    int unit,
    int hbm_index)
{
    uint32 entry_handle_id;
    uint32 write_recovery;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_COMMAND_ADDRESS_PARITY, INST_SINGLE,
                                 dnx_data_dram.general_info.command_address_parity_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_DQ_WRITE_PARITY, INST_SINGLE,
                                 dnx_data_dram.general_info.dq_write_parity_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_DQ_READ_PARITY, INST_SINGLE,
                                 dnx_data_dram.general_info.dq_read_parity_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_DBIAC_WRITE, INST_SINGLE,
                                 dnx_data_dram.general_info.dbi_write_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_DBIAC_READ, INST_SINGLE,
                                 dnx_data_dram.general_info.dbi_read_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_WRITE_LATENCY, INST_SINGLE,
                                 dnx_data_dram.general_info.write_latency_get(unit));
    if (!dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_BANK_GROUPS_EN, INST_SINGLE, 1);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_OPERATION_MODE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_PARITY_LATENCY, INST_SINGLE,
                                 dnx_data_dram.general_info.actual_parity_latency_get(unit));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_PIPELINES_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    if (!dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WDQS_LENGTH, INST_SINGLE,
                                     1 + dnx_data_dram.hbm.burst_length_get(unit) / 2 +
                                     (dnx_data_dram.general_info.dq_write_parity_get(unit) *
                                      dnx_data_dram.general_info.actual_parity_latency_get(unit)));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WDQS_DELAY, INST_SINGLE,
                                     dnx_data_dram.general_info.write_latency_get(unit));
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTPUT_ENABLE_LENGTH, INST_SINGLE,
                                 dnx_data_dram.hbm.output_enable_length_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTPUT_ENABLE_DELAY, INST_SINGLE,
                                 dnx_data_dram.hbm.output_enable_delay_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COMMAND_PIPE_EXTRA_DELAY, INST_SINGLE, 2);
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RD_DATA_EN_DELAY, INST_SINGLE,
                                     dnx_data_dram.general_info.read_latency_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RD_DATA_EN_LENGTH, INST_SINGLE,
                                     dnx_data_dram.general_info.read_data_enable_length_get(unit));
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_STACK_ID, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_STACK_ID_VALUE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLIP_PSEUDO_CHANNEL, INST_SINGLE, hbm_index);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_HBM_ROW_ACCESS_TIMINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRAS, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->tras);
    write_recovery = dnx_data_dram.general_info.timing_params_get(unit)->twr +
        dnx_data_dram.general_info.write_latency_get(unit) + dnx_data_dram.hbm.burst_length_get(unit) / 2;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TWR, INST_SINGLE, write_recovery);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRRDS, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->trrds);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRCDWR, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->trcdwr);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRCDRD, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->trcdrd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TFAW, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->tfaw);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRTPL, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->trtpl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRC, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->trc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRRDL, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->trrdl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRTPS, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->trtps);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRP, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->trp);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_HBM_COLUMN_ACCESS_TIMINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TWTRL, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->twtrl +
                                 dnx_data_dram.general_info.write_latency_get(unit) +
                                 dnx_data_dram.hbm.burst_length_get(unit) / 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TWTRS, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->twtrs +
                                 dnx_data_dram.general_info.write_latency_get(unit) +
                                 dnx_data_dram.hbm.burst_length_get(unit) / 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRTW, INST_SINGLE,
                                 dnx_data_dram.general_info.read_latency_get(unit) +
                                 dnx_data_dram.hbm.burst_length_get(unit) / 2 -
                                 dnx_data_dram.general_info.write_latency_get(unit) +
                                 dnx_data_dram.general_info.timing_params_get(unit)->trtw + 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TCCDL, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->tccdl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TCCDS, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->tccds);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TCCDR, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->tccdr);
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_T_WR_TO_RDAP, INST_SINGLE,
                                     dnx_data_dram.general_info.write_latency_get(unit) +
                                     dnx_data_dram.hbm.burst_length_get(unit) / 2 +
                                     UTILEX_MAX(write_recovery -
                                                dnx_data_dram.general_info.timing_params_get(unit)->trtpl,
                                                dnx_data_dram.general_info.timing_params_get(unit)->twtrl));
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_HBM_REFRESH_INTERVALS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFISB_N, 0,
                                 dnx_hbmc_ns_to_clock(unit,
                                                      dnx_data_dram.general_info.refresh_intervals_get(unit,
                                                                                                       0)->trefisb));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFISB_N, 1,
                                 dnx_hbmc_ns_to_clock(unit,
                                                      dnx_data_dram.general_info.refresh_intervals_get(unit,
                                                                                                       1)->trefisb));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFISB_N, 2,
                                 dnx_hbmc_ns_to_clock(unit,
                                                      dnx_data_dram.general_info.refresh_intervals_get(unit,
                                                                                                       2)->trefisb));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFISB_N, 3,
                                 dnx_hbmc_ns_to_clock(unit,
                                                      dnx_data_dram.general_info.refresh_intervals_get(unit,
                                                                                                       3)->trefisb));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFISB_N, 4,
                                 dnx_hbmc_ns_to_clock(unit,
                                                      dnx_data_dram.general_info.refresh_intervals_get(unit,
                                                                                                       4)->trefisb));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFISB_N, 5,
                                 dnx_hbmc_ns_to_clock(unit,
                                                      dnx_data_dram.general_info.refresh_intervals_get(unit,
                                                                                                       5)->trefisb));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFISB_N, 6,
                                 dnx_hbmc_ns_to_clock(unit,
                                                      dnx_data_dram.general_info.refresh_intervals_get(unit,
                                                                                                       6)->trefisb));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFISB_N, 7,
                                 dnx_hbmc_ns_to_clock(unit,
                                                      dnx_data_dram.general_info.refresh_intervals_get(unit,
                                                                                                       7)->trefisb));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFI_N, 0,
                                 dnx_data_dram.general_info.refresh_intervals_get(unit, 0)->trefi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFI_N, 1,
                                 dnx_data_dram.general_info.refresh_intervals_get(unit, 1)->trefi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFI_N, 2,
                                 dnx_data_dram.general_info.refresh_intervals_get(unit, 2)->trefi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFI_N, 3,
                                 dnx_data_dram.general_info.refresh_intervals_get(unit, 3)->trefi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFI_N, 4,
                                 dnx_data_dram.general_info.refresh_intervals_get(unit, 4)->trefi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFI_N, 5,
                                 dnx_data_dram.general_info.refresh_intervals_get(unit, 5)->trefi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFI_N, 6,
                                 dnx_data_dram.general_info.refresh_intervals_get(unit, 6)->trefi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TREFI_N, 7,
                                 dnx_data_dram.general_info.refresh_intervals_get(unit, 7)->trefi);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_HBM_REFRESH_TIMINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRFC, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->trfc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRFCSB, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->trfcsb);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TRREFD, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->trrefd);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_HBM_GENERAL_TIMINGS, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TMRD, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->tmrd);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_TMOD, INST_SINGLE,
                                 dnx_data_dram.general_info.timing_params_get(unit)->tmod);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_HBM_MODE_REGISTER_MANAGER_CONFIG, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UPDATE_HBM_ON_MODE_REGISTER_CHANGE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_UPDATE_PER_MODE_REGISTER_BITMAP, INST_SINGLE,
                                 0xffff);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_channel_wds_inflight_threshold))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_HCC_WRITE_DATA_SOURCE, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INFLIGHT_THREHSOLD, INST_SINGLE,
                                     dnx_data_dram.hbm.wds_size_get(unit) - 2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hcc_tsm_config(
    int unit,
    int hbm_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_TSM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_BANK_DISABLE, INST_SINGLE, 0);
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_seamless_commands_to_same_bank_control))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TSM_ALLOW_SEAMLESS_COMMANDS_TO_SAME_BANK,
                                     INST_SINGLE, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hcc_data_source_config(
    int unit,
    int hbm_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_DATA_SOURCE_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    for (int data_source = 0; data_source < dnx_data_device.general.nof_cores_get(unit); ++data_source)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_READ_INFLIGHTS_DS_N, data_source, 256);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_WRITE_INFLIGHTS_DS_N, data_source, 256);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hcc_data_path_enable_set(
    int unit,
    int hbm_index,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_DATA_PATH_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PHY_2_HBC_READ_PATH_ENABLE, INST_SINGLE,
                                 enable ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hcc_request_path_enable_set(
    int unit,
    int hbm_index,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_REQUEST_PATH_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READ_REQUEST_PATH_ENABLE, INST_SINGLE,
                                 enable ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WRITE_REQUEST_PATH_ENABLE, INST_SINGLE,
                                 enable ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_hrc_enable_set(
    int unit,
    int hbm_index,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HRC_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READ_REQUEST_PATH_ENABLE, INST_SINGLE,
                                 enable ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WRITE_REQUEST_PATH_ENABLE, INST_SINGLE,
                                 enable ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READ_DATA_PATH_ENABLE, INST_SINGLE, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_ecc_enable(
    int unit,
    int hbm_index,
    int channel,
    uint32 enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_ECC, INST_SINGLE, ! !enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_reset_control_set(
    int unit,
    int hbm_index,
    uint32 out_of_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_RESET_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_RESET_N_UI_0, INST_SINGLE, out_of_reset ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_RESET_N_UI_1, INST_SINGLE, out_of_reset ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_cattrip_indication_mask_set(
    int unit,
    int hbm_index,
    uint32 mask_cattrip_indication)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_RESET_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASK_PMX_CATTRIP_INDICATION, INST_SINGLE,
                                 ! !mask_cattrip_indication);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_cattrip_interrupt_clear(
    int unit,
    int hbm_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HBMC_INTERRUPTS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HBM_CATTRIP, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_output_enable_set(
    int unit,
    int hbm_index,
    uint32 output_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CPU_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_C_R_CKE_ARFU_OEN_UI_0, INST_SINGLE,
                                 output_enable ? 0 : 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_C_R_CKE_ARFU_OEN_UI_1, INST_SINGLE,
                                 output_enable ? 0 : 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WDQS_OEN_UI_0, INST_SINGLE, output_enable ? 0 : 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WDQS_OEN_UI_1, INST_SINGLE, output_enable ? 0 : 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_clocks_set(
    int unit,
    int hbm_index,
    uint32 ck_ui_0,
    uint32 ck_ui_1)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CPU_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CK_UI_0, INST_SINGLE, ck_ui_0 ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CK_UI_1, INST_SINGLE, ck_ui_1 ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_clocks_output_enable_set(
    int unit,
    int hbm_index,
    uint32 ck_oen_ui_0,
    uint32 ck_oen_ui_1)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CPU_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CK_OEN_UI_0, INST_SINGLE, ck_oen_ui_0 ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CK_OEN_UI_1, INST_SINGLE, ck_oen_ui_1 ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_cke_set(
    int unit,
    int hbm_index,
    uint32 cke_ui)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_CPU_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CKE_UI_0, INST_SINGLE, cke_ui ? 1 : 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CKE_UI_1, INST_SINGLE, cke_ui ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static inline dbal_fields_e
dnx_hbmc_mr_index_to_dbal_field(
    uint32 mr_index)
{
    dbal_fields_e fields[] = { DBAL_FIELD_MR_0, DBAL_FIELD_MR_1, DBAL_FIELD_MR_2, DBAL_FIELD_MR_3,
        DBAL_FIELD_MR_4, DBAL_FIELD_MR_5, DBAL_FIELD_MR_6, DBAL_FIELD_MR_7,
        DBAL_FIELD_MR_8, DBAL_FIELD_MR_9, DBAL_FIELD_MR_10, DBAL_FIELD_MR_11,
        DBAL_FIELD_MR_12, DBAL_FIELD_MR_13, DBAL_FIELD_MR_14, DBAL_FIELD_MR_15
    };
    return (mr_index >= sizeof(fields) / sizeof(fields[0])) ? (DBAL_FIELD_EMPTY) : (fields[mr_index]);
}


shr_error_e
dnx_hbmc_dbal_access_mr_set(
    int unit,
    uint32 hbm_index,
    uint32 channel,
    uint32 mr_index,
    uint32 value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_MODE_REGISTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, dnx_hbmc_mr_index_to_dbal_field(mr_index), INST_SINGLE, value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_mr_get(
    int unit,
    uint32 hbm_index,
    uint32 channel,
    uint32 mr_index,
    uint32 *value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_MODE_REGISTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    
    dbal_value_field32_request(unit, entry_handle_id, dnx_hbmc_mr_index_to_dbal_field(mr_index), INST_SINGLE, value);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_hbmc_dbal_access_mr_init_values_set(
    int unit,
    uint32 entry_handle_id)
{
    uint32 nof_mrs = dnx_data_dram.general_info.nof_mrs_get(unit);
    uint32 field_val;
    uint32 field_in_struct_val;

    SHR_FUNC_INIT_VARS(unit);

    
    for (int mr_index = 0; mr_index < nof_mrs; ++mr_index)
    {
                
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, dnx_hbmc_mr_index_to_dbal_field(mr_index),
                                                   INST_SINGLE, DBAL_PREDEF_VAL_DEFAULT_VALUE);
    }

    
    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_MR_0, 0,
                                                    DBAL_PREDEF_VAL_DEFAULT_VALUE, &field_val));
    field_in_struct_val = dnx_data_dram.general_info.dbi_read_get(unit);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_MR_0, DBAL_FIELD_DBIAC_READ,
                                                    &field_in_struct_val, &field_val));
    field_in_struct_val = dnx_data_dram.general_info.dbi_write_get(unit);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_MR_0, DBAL_FIELD_DBIAC_WRITE,
                                                    &field_in_struct_val, &field_val));
    field_in_struct_val = dnx_data_dram.general_info.dq_read_parity_get(unit);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_MR_0, DBAL_FIELD_DQ_READ_PARITY,
                                                    &field_in_struct_val, &field_val));
    field_in_struct_val = dnx_data_dram.general_info.dq_write_parity_get(unit);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_MR_0, DBAL_FIELD_DQ_WRITE_PARITY,
                                                    &field_in_struct_val, &field_val));
    field_in_struct_val = dnx_data_dram.general_info.command_address_parity_get(unit);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_MR_0, DBAL_FIELD_CMD_PARITY,
                                                    &field_in_struct_val, &field_val));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MR_0, INST_SINGLE, field_val);

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_MR_2, 0,
                                                    DBAL_PREDEF_VAL_DEFAULT_VALUE, &field_val));
    field_in_struct_val = dnx_data_dram.general_info.write_latency_get(unit) - 1;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_MR_2, DBAL_FIELD_WRITE_LATENCY,
                                                    &field_in_struct_val, &field_val));
    field_in_struct_val = dnx_data_dram.general_info.read_latency_get(unit) - 2;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_MR_2, DBAL_FIELD_READ_LATENCY,
                                                    &field_in_struct_val, &field_val));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MR_2, INST_SINGLE, field_val);

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_MR_3, 0,
                                                    DBAL_PREDEF_VAL_DEFAULT_VALUE, &field_val));
    field_in_struct_val = dnx_data_dram.general_info.timing_params_get(unit)->tras;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_MR_3, DBAL_FIELD_ACTIVATE_TO_PRECHARGE_RAS,
                                                    &field_in_struct_val, &field_val));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MR_3, INST_SINGLE, field_val);

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_MR_4, 0,
                                                    DBAL_PREDEF_VAL_DEFAULT_VALUE, &field_val));
    field_in_struct_val = dnx_data_dram.general_info.actual_parity_latency_get(unit);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_MR_4, DBAL_FIELD_PARITY_LATENCY, &field_in_struct_val, &field_val));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MR_4, INST_SINGLE, field_val);

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_MR_6, 0,
                                                    DBAL_PREDEF_VAL_DEFAULT_VALUE, &field_val));
    field_in_struct_val = dnx_data_dram.general_info.timing_params_get(unit)->trp - 2;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_MR_6, DBAL_FIELD_PRE_TRP_VALUE,
                                                    &field_in_struct_val, &field_val));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MR_6, INST_SINGLE, field_val);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_mr_init_channel_set(
    int unit,
    int hbm_index,
    int channel_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_MODE_REGISTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel_index);
    SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_mr_init_values_set(unit, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_mr_init_set(
    int unit,
    int hbm_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_MODE_REGISTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_mr_init_values_set(unit, entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_update_hbm_on_mode_register_change(
    int unit,
    int hbm_index,
    int channel,
    int update)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_MODE_REGISTER_MANAGER_CONFIG, &entry_handle_id));
    if (hbm_index == DNX_HBMC_ITER_ALL)
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    }
    if (channel == DNX_HBMC_ITER_ALL)
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UPDATE_HBM_ON_MODE_REGISTER_CHANGE, INST_SINGLE,
                                 ! !update);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_data_source_id_get(
    int unit,
    int core,
    uint32 *data_source_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(data_source_id, _SHR_E_PARAM, "data_source_id");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDU_DATA_SOURCE_ID, &entry_handle_id));
    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_HBMC_INDEX, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_TDU_INDEX, 0);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DATA_SOURCE_ID, INST_SINGLE, data_source_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_hbmc_dbal_access_dram_block_write_minus_read_leaky_bucket_config_set(
    int unit)
{
    uint32 entry_handle_id;
    uint32 decrement_thr_factor = dnx_data_dram.dram_block.wmr_decrement_thr_factor_get(unit);
    uint32 reset_on_deassert = dnx_data_dram.dram_block.wmr_reset_on_deassert_get(unit);
    uint32 full_size = dnx_data_dram.dram_block.wmr_full_size_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_TDU_DRAM_BLOCK_WRITE_MINUS_READ_LEAKY_BUCKET_CONFIG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_0, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_1, INST_SINGLE,
                                 2000);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_2, INST_SINGLE,
                                 4000);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_0, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_1, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_2, INST_SINGLE, 4);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_0, INST_SINGLE,
                                 0 * decrement_thr_factor / 100);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_1, INST_SINGLE,
                                 5 * decrement_thr_factor / 100);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_2, INST_SINGLE,
                                 10 * decrement_thr_factor / 100);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_0, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_1, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_2, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_ASSERT_THRESHOLD, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DEASSERT_THRESHOLD, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_RESET_ON_DEASSERT, INST_SINGLE,
                                 reset_on_deassert);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_FULL_SIZE, INST_SINGLE, full_size);
    if (dnx_data_dram.dram_block.feature_get(unit, dnx_data_dram_dram_block_write_minus_read_leaky_bucket_disable))
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_EN, INST_SINGLE, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_hbmc_dbal_access_dram_block_write_plus_read_leaky_bucket_config_set(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_TDU_DRAM_BLOCK_WRITE_PLUS_READ_LEAKY_BUCKET_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_ASSERT_THRESHOLD, INST_SINGLE, 16);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DEASSERT_THRESHOLD, INST_SINGLE, 4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_FULL_SIZE, INST_SINGLE, 50);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_RESET_ON_DEASSERT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_0, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_1, INST_SINGLE, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_2, INST_SINGLE, 4);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_0, INST_SINGLE,
                                 dnx_data_dram.dram_block.wpr_leaky_bucket_increment_th_get(unit, 0)->threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_1, INST_SINGLE,
                                 dnx_data_dram.dram_block.wpr_leaky_bucket_increment_th_get(unit, 1)->threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_2, INST_SINGLE,
                                 dnx_data_dram.dram_block.wpr_leaky_bucket_increment_th_get(unit, 2)->threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE, INST_SINGLE, 1);
    if (dnx_data_dram.dram_block.feature_get(unit, dnx_data_dram_dram_block_write_plus_read_leaky_bucket_disable))
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_EN, INST_SINGLE, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_hbmc_dbal_access_dram_block_write_leaky_bucket_config_set(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDU_DRAM_BLOCK_WRITE_LEAKY_BUCKET_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_ASSERT_THRESHOLD, INST_SINGLE, 16);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DEASSERT_THRESHOLD, INST_SINGLE, 4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_FULL_SIZE, INST_SINGLE, 50);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_RESET_ON_DEASSERT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_0, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_1, INST_SINGLE, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_2, INST_SINGLE, 4);
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_0, INST_SINGLE,
                                 dnx_data_dram.dram_block.write_leaky_bucket_increment_th_get(unit, 0)->threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_1, INST_SINGLE,
                                 dnx_data_dram.dram_block.write_leaky_bucket_increment_th_get(unit, 1)->threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_2, INST_SINGLE,
                                 dnx_data_dram.dram_block.write_leaky_bucket_increment_th_get(unit, 2)->threshold);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_hbmc_dbal_access_dram_block_average_read_inflights_leaky_bucket_config_set(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_TDU_DRAM_BLOCK_AVERAGE_READ_INFLIGHTS_LEAKY_BUCKET_CONFIG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_0, INST_SINGLE,
                                 1800);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_1, INST_SINGLE,
                                 1900);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_THRESHOLD_2, INST_SINGLE,
                                 2000);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_0, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_1, INST_SINGLE, 4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_INCREMENT_SIZE_2, INST_SINGLE, 8);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_0, INST_SINGLE,
                                 1800);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_1, INST_SINGLE,
                                 1700);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_THRESHOLD_2, INST_SINGLE,
                                 1600);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_0, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_1, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DECREMENT_SIZE_2, INST_SINGLE, 4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_ASSERT_THRESHOLD, INST_SINGLE,
                                 dnx_data_dram.dram_block.average_read_inflights_assert_threshold_get(unit));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_DEASSERT_THRESHOLD, INST_SINGLE, 4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_RESET_ON_DEASSERT, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_FULL_SIZE, INST_SINGLE,
                                 dnx_data_dram.dram_block.average_read_inflights_full_size_get(unit));
    if (dnx_data_dram.dram_block.feature_get(unit, dnx_data_dram_dram_block_average_read_inflights_log2_window_size))
    {
        
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_WINDOW_SIZE_LOG_2, INST_SINGLE, 12);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_tdu_configure(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);
    uint32 dram_bitmap = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    uint32 window_size = dnx_data_dram.dram_block.leaky_bucket_window_size_get(unit);
    uint32 nof_tdus_per_dram = dnx_data_dram.address_translation.nof_tdus_per_dram_get(unit);
    uint32 tdu_index;
    uint32 data_source_id;
    uint32 global_hbm_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDU_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READ_DATA_PATH_ENABLE, INST_SINGLE, 1);
    if (!dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READ_DATA_RING_0_EXTRA_DELAY, INST_SINGLE, 12);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READ_DATA_RING_1_EXTRA_DELAY, INST_SINGLE, 0);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WRITE_REQUEST_PATH_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_READ_REQUEST_PATH_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEAKY_BUCKET_WINDOW_SIZE, INST_SINGLE, window_size);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_TDU_DATA_SOURCE_ID, entry_handle_id));
    for (global_hbm_index = 0; global_hbm_index < max_nof_drams; ++global_hbm_index)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_HBMC_INDEX, global_hbm_index);

        for (tdu_index = 0; tdu_index < nof_tdus_per_dram; ++tdu_index)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_TDU_INDEX, tdu_index);

            data_source_id =
                dnx_data_dram.address_translation.tdu_map_get(unit, global_hbm_index, tdu_index)->data_source_id;
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_SOURCE_ID, INST_SINGLE,
                                         (dram_bitmap == 0x2) ? !data_source_id : data_source_id);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    
    if (dnx_data_dram.dram_block.feature_get(unit, dnx_data_dram_dram_block_write_minus_read_leaky_bucket))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_dram_block_write_minus_read_leaky_bucket_config_set(unit));
    }

    SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_dram_block_write_plus_read_leaky_bucket_config_set(unit));

    if (dnx_data_dram.dram_block.feature_get(unit, dnx_data_dram_dram_block_write_leaky_bucket))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_dram_block_write_leaky_bucket_config_set(unit));
    }

    if (dnx_data_dram.dram_block.feature_get(unit, dnx_data_dram_dram_block_average_read_inflights_leaky_bucket))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_dbal_access_dram_block_average_read_inflights_leaky_bucket_config_set(unit));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_tdu_atm_configure(
    int unit,
    const uint32 *atm)
{
    uint32 entry_handle_id;
    uint32 max_nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);
    uint32 atm_column_size = dnx_data_dram.address_translation.matrix_column_size_get(unit);
    uint32 nof_tdus_per_dram = dnx_data_dram.address_translation.nof_tdus_per_dram_get(unit);
    uint32 tdu_index;
    uint32 data_source_id;
    uint32 atm_line_0;
    uint32 global_hbm_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDU_ADDRESS_TRANSLATION_MATRIX, &entry_handle_id));

    for (global_hbm_index = 0; global_hbm_index < max_nof_drams; ++global_hbm_index)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOBAL_HBMC_INDEX, global_hbm_index);

        for (tdu_index = 0; tdu_index < nof_tdus_per_dram; ++tdu_index)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_TDU_INDEX, tdu_index);

            
            data_source_id =
                dnx_data_dram.address_translation.tdu_map_get(unit, global_hbm_index, tdu_index)->data_source_id;

            
            atm_line_0 = (global_hbm_index == data_source_id) ? atm[0] : 0;
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDRESS_TRANSLATION_MATRIX, 0, atm_line_0);

            
            for (int line = 1; line < atm_column_size; ++line)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADDRESS_TRANSLATION_MATRIX, line,
                                             atm[line]);
            }
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_dram_cpu_access_get(
    int unit,
    uint32 module,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_DRAM_CPU_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, module);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK, bank);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ROW, row);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COLUMN, column);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_HBM_DRAM_CPU_ACCESS, INST_SINGLE, data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_dbal_access_dram_cpu_access_set(
    int unit,
    uint32 module,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *pattern)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_HBM_DRAM_CPU_ACCESS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, module);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BANK, bank);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ROW, row);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COLUMN, column);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_HBM_DRAM_CPU_ACCESS, INST_SINGLE, pattern);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
