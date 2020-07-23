

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
#include <bcm_int/dnx/dram/buffers/dram_buffers.h>
#include <bcm_int/dnx/dram/dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include "buffers_quarantine_mechanism.h"








shr_error_e
dnx_dram_buffers_nof_bdbs_per_core_get(
    int unit,
    uint32 *bdbs_per_core)
{
    int nof_drams = dnx_data_dram.general_info.max_nof_drams_get(unit);
    int nof_enabled_drams = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bdbs_per_core, _SHR_E_PARAM, "bdbs_per_core");
    SHR_IF_ERR_EXIT(dnx_dram_count(unit, &nof_enabled_drams));
    *bdbs_per_core = dnx_data_dram.buffers.nof_bdbs_get(unit) / nof_drams * nof_enabled_drams;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_buffers_init(
    int unit)
{
    uint32 nof_bdbs_per_bank;
    uint32 bdbs_per_core;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_dram_db.sync_manager.deleted_buffers_file_mutex.create(unit));

    
    SHR_IF_ERR_EXIT(dnx_dram_buffers_nof_bdbs_per_core_get(unit, &bdbs_per_core));
    nof_bdbs_per_bank = bdbs_per_core / dnx_data_dram.buffers.nof_fpc_banks_get(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BDM_FPC_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_FIRST_PTR, INST_ALL, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_LAST_PTR, INST_ALL,
                                 (nof_bdbs_per_bank == 0) ? nof_bdbs_per_bank : nof_bdbs_per_bank - 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_BDM_INIT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_INIT, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_BDM_INIT, entry_handle_id));
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_FPC_INIT, 0));
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_BDM_INIT_STATUS, 0));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_BDB_CONFIG, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_ALLOCATE_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_DDP_GENERAL_CONFIGURATION, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WR_DATA_PATH_EN, INST_ALL, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RD_DATA_PATH_EN, INST_ALL, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_init(unit));

    if (dnx_data_dram.buffers.feature_get(unit, dnx_data_dram_buffers_bdb_release_mechanism_usage_counters))
    {
        
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_DDP_FIFOS_CONFIG, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PKUP_BDB_RLS_TXI_IRDY_TH, INST_SINGLE, 12);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_buffers_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dram_buffers_status_get(
    int unit,
    int core,
    uint8 deleted,
    uint32 *buffers_array,
    int max_size,
    int *size)
{
    uint32 entry_handle_id;
    
    uint32 core_rule_value[1] = { core };
    
    uint32 error_counter_rule_value[1];
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    *size = 0;

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BDB_ERRORS_COUNTER, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                           DBAL_CONDITION_EQUAL_TO, core_rule_value, NULL));
    if (deleted)
    {
        
        error_counter_rule_value[0] = dnx_data_dram.buffers.allowed_errors_get(unit);
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_ERROR_COUNTER,
                                                                 INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                                                                 error_counter_rule_value, NULL));
    }
    else
    {
        
        error_counter_rule_value[0] = 0;
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_ERROR_COUNTER,
                                                                 INST_SINGLE, DBAL_CONDITION_BIGGER_THAN,
                                                                 error_counter_rule_value, NULL));
    }
    
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    
    while (!is_end)
    {
        if (*size < max_size)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_BDB_INDEX, &(buffers_array[*size])));
        }
        ++(*size);
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
