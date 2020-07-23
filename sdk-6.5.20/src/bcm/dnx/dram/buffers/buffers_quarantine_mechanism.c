

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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include "buffers_quarantine_mechanism.h"
#include <bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h>
#include <bcm_int/dnx/dram/buffers/dram_buffers.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <sal/appl/io.h>
#include <shared/util.h>





#define ACTIVATE 1
#define ACTION_DONE 0
#define TOKEN_MAX_SIZE 20


#define DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE (1000)




static shr_error_e
dnx_dram_buffers_quarantine_read_buffer_verify(
    int unit,
    bcm_core_t core,
    uint32 *buffer,
    uint32 *buffer_is_valid,
    uint32 *quarantine_reason)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(buffer, _SHR_E_PARAM, "buffer");
    SHR_NULL_CHECK(buffer_is_valid, _SHR_E_PARAM, "buffer_is_valid");
    SHR_NULL_CHECK(quarantine_reason, _SHR_E_PARAM, "quarantine_reason");

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dram_buffers_quarantine_read_buffer(
    int unit,
    bcm_core_t core,
    uint32 *buffer,
    uint32 *buffer_is_valid,
    uint32 *quarantine_reason)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_dram_buffers_quarantine_read_buffer_verify
                          (unit, core, buffer, buffer_is_valid, quarantine_reason));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NEXT_QUARANTINED_BDB_IN_FIFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUARANTINED_BUFFER_IS_VALID, INST_SINGLE,
                               buffer_is_valid);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUARANTINED_BDB, INST_SINGLE, buffer);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QUARANTINE_REASON, INST_SINGLE, quarantine_reason);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dram_buffers_quarantined_buffer_restore(
    int unit,
    bcm_core_t core,
    uint32 buffer)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RELEASE_BDB_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RELEASE_ACTIVATE, INST_SINGLE, ACTIVATE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_TO_RELEASE, INST_SINGLE, buffer);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_RELEASE_BDB_CONTROL, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_RELEASE_ACTIVATE, ACTION_DONE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_buffers_quarantine_buffer_to_quarantine_schedule(
    int unit,
    bcm_core_t core,
    uint32 buffer)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DELETE_BDB_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_ACTIVATE, INST_SINGLE, ACTIVATE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_TO_DELETE, INST_SINGLE, buffer);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_buffers_quarantine_buffer_to_quarantine_done(
    int unit,
    bcm_core_t core)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DELETE_BDB_CONTROL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_DELETE_ACTIVATE, ACTION_DONE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dram_buffers_quarantined_buffer_delete(
    int unit,
    bcm_core_t core,
    uint32 buffer)
{
    uint32 entry_handle_id;
    uint32 deleted_bdbs_counter;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DELETED_BDB_COUNTER, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DELETED_BDB_COUNTER, INST_SINGLE,
                               &deleted_bdbs_counter);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETED_BDB_COUNTER, INST_SINGLE,
                                 ++deleted_bdbs_counter);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dram_buffers_quarantine_nof_deleted_buffers_get_verify(
    int unit,
    uint32 *nof_deleted_buffers)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_deleted_buffers, _SHR_E_PARAM, "nof_deleted_buffers");

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_buffers_quarantine_nof_deleted_buffers_get(
    int unit,
    uint32 *nof_deleted_buffers)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_dram_buffers_quarantine_nof_deleted_buffers_get_verify(unit, nof_deleted_buffers));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DELETED_BDB_COUNTER, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DELETED_BDB_COUNTER, INST_SINGLE, nof_deleted_buffers);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dram_buffers_quarantine_nof_free_buffers_get_verify(
    int unit,
    uint32 *nof_free_buffers)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_free_buffers, _SHR_E_PARAM, "nof_free_buffers");

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_buffers_quarantine_nof_free_buffers_get(
    int unit,
    uint32 *nof_free_buffers)
{
    uint32 entry_handle_id;
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_dram_buffers_quarantine_nof_free_buffers_get_verify(unit, nof_free_buffers));
    *nof_free_buffers = 0;

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DQM_CONTROL, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENQ_EN, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_BDB_CONFIG, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_ALLOCATE_EN, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_DQM_CONTROL, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RELEASE_ALL_BDBS, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    
    for (int core = 0; core < nof_cores; ++core)
    {
        uint32 free_buffers;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FREE_BDB_COUNTER, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FREE_BDB_COUNTER, INST_SINGLE, &free_buffers);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        *nof_free_buffers += free_buffers;
    }

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_BDB_CONFIG, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_ALLOCATE_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_DQM_CONTROL, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RELEASE_ALL_BDBS, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENQ_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_buffers_quarantine_buffer_flush_all(
    int unit)
{
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    for (int core = 0; core < nof_cores; ++core)
    {
        uint32 buffer;
        uint32 buffer_is_valid;
        uint32 quarantine_reason;
        do
        {
            SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_read_buffer
                            (unit, core, &buffer, &buffer_is_valid, &quarantine_reason));
            if (buffer_is_valid)
            {
                SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantined_buffer_restore(unit, core, buffer));
            }
        }
        while (buffer_is_valid);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dram_buffers_buffer_verify(
    int unit,
    uint32 buffer)
{
    uint32 nof_bdbs = dnx_data_dram.buffers.nof_bdbs_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    if (buffer >= nof_bdbs)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Received buffer (%d) is out of range. Should be smaller than %d\n", buffer, nof_bdbs);
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dram_buffers_core_verify(
    int unit,
    uint32 core)
{
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    if (core >= nof_cores)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Received core (%d) is out of range. Should be smaller than %d\n", core, nof_cores);
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dram_buffers_quarantine_delete_reasons_set(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_DELETE_BDB_CONFIGS, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_LAST_BUFF_CRC_EN, INST_SINGLE, 1);
    if (dnx_data_dram.buffers.feature_get(unit, dnx_data_dram_buffers_quarantine_buffers_if_tdu_errors))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_TDU_ERR_EN, INST_SINGLE, 1);
    }
    if (dnx_data_dram.buffers.feature_get(unit, dnx_data_dram_buffers_quarantine_buffers_if_tdu_corrected_errors))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_TDU_CORRECTED_ERR_EN, INST_SINGLE, 1);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_CPYDAT_ECC_ERR_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_CPYDAT_CRC_ERR_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DELETE_CPYDAT_PKTSIZE_ERR_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dram_buffers_quarantine_nof_error_get(
    int unit,
    bcm_core_t core,
    uint32 buffer,
    uint32 *nof_errors)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BDB_ERRORS_COUNTER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_INDEX, buffer);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ERROR_COUNTER, INST_SINGLE, nof_errors);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dram_buffers_quarantine_nof_error_set(
    int unit,
    bcm_core_t core,
    uint32 buffer,
    uint32 nof_errors)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BDB_ERRORS_COUNTER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_INDEX, buffer);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERROR_COUNTER, INST_SINGLE, nof_errors);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



static shr_error_e
dnx_dram_buffers_quarantine_token_read(
    int unit,
    FILE * p_file,
    uint32 *val,
    uint8 *token_found)
{
    int index = 0;
    char token[TOKEN_MAX_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    *token_found = FALSE;
    while ((token[index] = (char) sal_fgetc(p_file)) > 0)
    {
        if (!_shr_isxdigit(token[index]))
        {
            if (index > 0)
            {
                *token_found = TRUE;
            }
            break;
        }

        if (index == (TOKEN_MAX_SIZE - 1))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "token is too big\n");
        }
        ++index;
    }

    if (*token_found)
    {
        *val = _shr_ctoi(token);
    }
    else
    {
        *val = -1;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dram_buffers_initiated_deletion(
    int unit,
    bcm_core_t core,
    uint32 buffer_identifier)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_delete_bdb_supported))
    {
        
        uint32 delete_ptr_failed;

        SHR_IF_ERR_EXIT(dnx_dram_buffers_buffer_verify(unit, buffer_identifier));
        SHR_IF_ERR_EXIT(dnx_dram_buffers_core_verify(unit, core));
        
        SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_DELETE_BDB_POINTER,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_PTR_TO_DELETE, INST_SINGLE, buffer_identifier,
                                         GEN_DBAL_FIELD_LAST_MARK));
        
        SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_DELETE_BDB_POINTER,
                                         1, 1,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                         GEN_DBAL_FIELD32, DBAL_FIELD_DELETE_PTR_FAILED, INST_SINGLE,
                                         &delete_ptr_failed, GEN_DBAL_FIELD_LAST_MARK));
        if (delete_ptr_failed)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "\r\n" "HW indicates failure to delete buffer %d on core %d.\r\n", buffer_identifier, core);
        }
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantined_buffer_delete(unit, core, buffer_identifier));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dram_buffers_bdbs_pre_post_deletion_enable(
    int unit,
    int enable)
{
    uint32 enable_enqueue;
    uint32 enable_allocate;
    uint32 release_all_bdbs;
    uint32 move_all_ptr_to_mem;
    int poll_total_count, poll_counter;
    bcm_core_t core;
    uint32 nof_cores;

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_delete_bdb_supported))
    {
        nof_cores = dnx_data_device.general.nof_cores_get(unit);
        if (enable != FALSE)
        {
            uint32 nof_bdbs;
            int matched;
            uint32 nof_free_buffers;

            enable_enqueue = 0;
            enable_allocate = 0;
            release_all_bdbs = 1;
            move_all_ptr_to_mem = 1;
            
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_BDB_CONFIG,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_BDB_ALLOCATE_EN, INST_SINGLE, enable_allocate,
                                             GEN_DBAL_FIELD_LAST_MARK));
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_DQM_CONTROL,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_ENQ_EN, INST_SINGLE, enable_enqueue,
                                             GEN_DBAL_FIELD_LAST_MARK));
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_DQM_CONTROL,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_RELEASE_ALL_BDBS, INST_SINGLE,
                                             release_all_bdbs, GEN_DBAL_FIELD_LAST_MARK));
            poll_total_count = NUM_POLL_TOTAL_COUNT;
            SHR_IF_ERR_EXIT(dnx_dram_buffers_nof_bdbs_per_core_get(unit, &nof_bdbs));
            for (core = 0; core < nof_cores; core++)
            {
                matched = FALSE;
                for (poll_counter = 0; poll_counter < poll_total_count; poll_counter++)
                {
                    
                    nof_free_buffers = 0;
                    SHR_IF_ERR_EXIT
                        (dnx_dbal_gen_get(unit, DBAL_TABLE_FREE_BDB_COUNTER,
                                          1, 1,
                                          GEN_DBAL_FIELD32, DBAL_FIELD_CORE_ID, core,
                                          GEN_DBAL_FIELD32, DBAL_FIELD_FREE_BDB_COUNTER, INST_SINGLE, &nof_free_buffers,
                                          GEN_DBAL_FIELD_LAST_MARK));
                    if (nof_free_buffers == nof_bdbs)
                    {
                        matched = TRUE;
                        break;
                    }
                }
                if (matched == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "\r\n"
                                 "Number of free buffers on core %d after %d poll cycles is %d (Should be %d)\r\n",
                                 core, poll_counter, nof_free_buffers, nof_bdbs);
                }
            }
            
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_BDM_INIT,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_MOVE_ALL_PTR_TO_MEM, INST_SINGLE,
                                             move_all_ptr_to_mem, GEN_DBAL_FIELD_LAST_MARK));
        }
        else
        {
            enable_enqueue = 1;
            enable_allocate = 1;
            release_all_bdbs = 0;
            move_all_ptr_to_mem = 0;
            
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_BDM_INIT,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_MOVE_ALL_PTR_TO_MEM, INST_SINGLE,
                                             move_all_ptr_to_mem, GEN_DBAL_FIELD_LAST_MARK));
            
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_DQM_CONTROL,
                                             0, 1,
                                             GEN_DBAL_FIELD32, DBAL_FIELD_RELEASE_ALL_BDBS, INST_SINGLE,
                                             release_all_bdbs, GEN_DBAL_FIELD_LAST_MARK));
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set
                            (unit, DBAL_TABLE_BDB_CONFIG, 0, 1, GEN_DBAL_FIELD32, DBAL_FIELD_BDB_ALLOCATE_EN,
                             INST_SINGLE, enable_allocate, GEN_DBAL_FIELD_LAST_MARK));
            SHR_IF_ERR_EXIT(dnx_dbal_gen_set
                            (unit, DBAL_TABLE_DQM_CONTROL, 0, 1, GEN_DBAL_FIELD32, DBAL_FIELD_ENQ_EN, INST_SINGLE,
                             enable_enqueue, GEN_DBAL_FIELD_LAST_MARK));
        }
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_buffers_quarantine_init(
    int unit)
{
    char *deleted_buffers_file;
    uint32 buffer_delete_threshold = dnx_data_dram.buffers.allowed_errors_get(unit);
    FILE *p_file = NULL;
    uint32 buffer_to_mark_for_deleting;
    uint32 core;
    uint8 value_found;
    uint8 mutex_is_mine = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0)
    {
        SHR_EXIT();
    }

    
    if (buffer_delete_threshold == 0)
    {
        SHR_EXIT();
    }

    
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_delete_reasons_set(unit));

    
    SHR_IF_ERR_EXIT(dnx_dram_db.sync_manager.deleted_buffers_file_mutex.take(unit, sal_mutex_FOREVER));
    mutex_is_mine = 1;

    
    deleted_buffers_file = dnx_data_dram.buffers.deleted_buffers_info_get(unit)->deleted_buffers_file;

    
    p_file = sal_fopen(deleted_buffers_file, "r");
    if (p_file == NULL)
    {
        
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_dram_buffers_bdbs_pre_post_deletion_enable(unit, TRUE));
    
    for (;;)
    {
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_token_read
                        (unit, p_file, &buffer_to_mark_for_deleting, &value_found));
        if (value_found == FALSE)
        {
            break;
        }
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_token_read(unit, p_file, &core, &value_found));
        if (value_found == FALSE)
        {
            break;
        }
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_buffer_verify(unit, buffer_to_mark_for_deleting));
        SHR_IF_ERR_EXIT(dnx_dram_buffers_core_verify(unit, core));
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_error_set
                        (unit, core, buffer_to_mark_for_deleting, buffer_delete_threshold));
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_initiated_deletion(unit, core, buffer_to_mark_for_deleting));
    }
    SHR_IF_ERR_EXIT(dnx_dram_buffers_bdbs_pre_post_deletion_enable(unit, FALSE));

    

exit:
    
    if (p_file != NULL)
    {
        sal_fclose(p_file);
    }
    
    if (mutex_is_mine)
    {
        SHR_IF_ERR_CONT(dnx_dram_db.sync_manager.deleted_buffers_file_mutex.give(unit));
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dram_buffers_quarantine_buffer_to_file_write(
    int unit,
    bcm_core_t core,
    uint32 buffer)
{
    FILE *p_file = NULL;
    char *deleted_buffers_file;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_dram_db.sync_manager.deleted_buffers_file_mutex.take(unit, sal_mutex_FOREVER));

    
    deleted_buffers_file = dnx_data_dram.buffers.deleted_buffers_info_get(unit)->deleted_buffers_file;

    
    p_file = sal_fopen(deleted_buffers_file, "a");
    if (p_file == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unable to open file\n");
    }

    
    if (sal_fprintf(p_file, "%u %u ", buffer, core) < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unable to write to file\n");
    }

exit:
    
    if (p_file != NULL)
    {
        sal_fclose(p_file);
    }
    
    SHR_IF_ERR_CONT(dnx_dram_db.sync_manager.deleted_buffers_file_mutex.give(unit));
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_dram_buffers_quarantine_buffer_nof_errors_update(
    int unit,
    bcm_core_t core,
    uint32 buffer)
{
    uint32 buffer_delete_threshold = dnx_data_dram.buffers.allowed_errors_get(unit);
    uint32 nof_errors;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_error_get(unit, core, buffer, &nof_errors));
    
    if (nof_errors == buffer_delete_threshold)
    {
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantined_buffer_delete(unit, core, buffer));
        SHR_EXIT();
    }

    
    ++nof_errors;
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_error_set(unit, core, buffer, nof_errors));

    
    if (nof_errors == buffer_delete_threshold)
    {
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantined_buffer_delete(unit, core, buffer));
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_buffer_to_file_write(unit, core, buffer));
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantined_buffer_restore(unit, core, buffer));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dram_buffers_quarantine_fifo_not_empty_interrupt_handler(
    int unit,
    bcm_core_t core)
{
    uint32 buffer;
    uint32 buffer_is_valid;
    uint32 quarantine_reason;
    uint32 buffer_delete_threshold = dnx_data_dram.buffers.allowed_errors_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    
    if (buffer_delete_threshold == 0)
    {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "%s(): Buffer Quarantine Mechanism is not active\n"), FUNCTION_NAME()));
        SHR_EXIT();
    }

    
    do
    {
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_read_buffer
                        (unit, core, &buffer, &buffer_is_valid, &quarantine_reason));
        
        if (buffer_is_valid)
        {
            
            SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_buffer_nof_errors_update(unit, core, buffer));
        }
    }
    while (buffer_is_valid);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_buffers_quarantine_ecc_error_interrupt_handler(
    int unit,
    uint32 hbc_block)
{
    uint32 entry_handle_id;
    uint32 hbm_index = hbc_block / dnx_data_dram.general_info.max_nof_drams_get(unit);
    uint32 channel[1] = { hbc_block / dnx_data_dram.general_info.nof_channels_get(unit) };
    uint32 bank[1] = { 0 };
    uint32 row[1] = { 0 };
    uint32 column[1] = { 0 };
    uint32 logical_ptr[1] = { 0 };
    uint32 logical_buffer[1] = { 0 };
    uint32 bdb_index = 0;
    uint32 data_source_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_HCC_LAST_ECC_ERROR, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_HBMC_INDEX, hbm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, channel[0]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_ECC_ERR_BANK, INST_SINGLE, bank);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_ECC_ERR_ROW, INST_SINGLE, row);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LAST_ECC_ERR_COLUMN, INST_SINGLE, column);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0x3)
    {
        logical_ptr[0] |= SHR_IS_BITSET(row, 7) << 0;
        logical_ptr[0] |= SHR_IS_BITSET(row, 8) << 1;
        logical_ptr[0] |= SHR_IS_BITSET(row, 9) << 2;
        logical_ptr[0] |= SHR_IS_BITSET(row, 10) << 3;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 8) ^ SHR_IS_BITSET(bank, 2)) << 4;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 7) ^ hbm_index ^ SHR_IS_BITSET(column, 4)) << 5;
        logical_ptr[0] |= SHR_IS_BITSET(column, 5) << 6;
        logical_ptr[0] |= SHR_IS_BITSET(row, 0) << 7;
        logical_ptr[0] |= SHR_IS_BITSET(row, 1) << 8;
        logical_ptr[0] |= SHR_IS_BITSET(row, 2) << 9;
        logical_ptr[0] |= SHR_IS_BITSET(row, 3) << 10;
        logical_ptr[0] |= SHR_IS_BITSET(row, 4) << 11;
        logical_ptr[0] |= SHR_IS_BITSET(row, 5) << 12;
        logical_ptr[0] |= SHR_IS_BITSET(row, 6) << 13;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 10) ^ SHR_IS_BITSET(channel, 2)) << 14;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 10) ^ SHR_IS_BITSET(bank, 1)) << 15;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 9) ^ SHR_IS_BITSET(bank, 0)) << 16;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 7) ^ SHR_IS_BITSET(bank, 3)) << 17;
        logical_ptr[0] |= SHR_IS_BITSET(row, 11) << 18;
        logical_ptr[0] |= SHR_IS_BITSET(row, 12) << 19;
        logical_ptr[0] |= SHR_IS_BITSET(row, 13) << 20;
        logical_ptr[0] |= SHR_IS_BITSET(column, 3) << 21;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 8) ^ SHR_IS_BITSET(channel, 0)) << 22;
        logical_ptr[0] |= (SHR_IS_BITSET(row, 9) ^ SHR_IS_BITSET(channel, 1)) << 23;
        logical_ptr[0] |= SHR_IS_BITSET(column, 4) << 24;
    }

    
    SHR_BITCOPY_RANGE(logical_buffer, 0, logical_ptr, 4, 20);;
    
    bdb_index = logical_buffer[0] / 8;
    
    data_source_id = SHR_IS_BITSET(logical_ptr, 24);

    
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_buffer_nof_errors_update(unit, data_source_id, bdb_index));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_buffers_quarantine_restore_deleted_buffers_after_soft_init(
    int unit)
{
    uint32 buffers_array[DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE];
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
    int size;
    int core = 0;
    int index = 0;
    uint8 is_dram_db_inited = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0)
    {
        SHR_EXIT();
    }

    
    SHR_IF_ERR_EXIT(dnx_dram_db.is_init(unit, &is_dram_db_inited));
    if (!is_dram_db_inited)
    {
        SHR_EXIT();
    }

    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_delete_bdb_supported))
    {
        SHR_IF_ERR_EXIT(dnx_dram_buffers_bdbs_pre_post_deletion_enable(unit, TRUE));

        
        for (core = 0; core < nof_cores; ++core)
        {
            
            sal_memset(buffers_array, 0, sizeof(buffers_array));
            
            SHR_IF_ERR_EXIT(dnx_dram_buffers_status_get
                            (unit, core, TRUE, buffers_array, DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE, &size));

            
            if (size > DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE)
            {
                
                LOG_CLI((BSL_META("Too many buffers were deleted on core %d, deleting %d out of %d that were found\n"),
                         core, DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE, size));
                
                size = DNX_DRAM_BUFFERS_ARRAY_MAX_SIZE;
            }

            
            for (index = 0; index < size; ++index)
            {
                
                SHR_IF_ERR_EXIT(dnx_dram_buffers_initiated_deletion(unit, core, buffers_array[index]));
            }
        }

        SHR_IF_ERR_EXIT(dnx_dram_buffers_bdbs_pre_post_deletion_enable(unit, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}
