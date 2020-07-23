
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SAT

 
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dbal/dbal.h>









 



shr_error_e
dnx_sat_gtf_credit_config(
    int unit,
    int pipe_id,
    int credit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SAT_TX_CONFIG, &entry_handle_id));

    
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_SAT_PIPE_ID, pipe_id);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_BACKPRESSURE, INST_SINGLE, credit);

    
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
