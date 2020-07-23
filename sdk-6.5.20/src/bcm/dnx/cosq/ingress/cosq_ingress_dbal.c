/**
 * \file cosq_ingress_dbal.c
 * 
 *
 * General COSQ Ingress functionality for DNX. \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cosq/cosq.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_cosq_ingress_access.h>
#include "cosq_ingress.h"

/**
 * 
 * See cosq_ingress.h
 */
shr_error_e
dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_set(
    int unit,
    int dbal_table_id,
    int core_id,
    uint32 grant_size,
    uint32 timer_cycles)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Timer cycles == 0 means every clock cycle. This means
     *  that for correct calculations timer_cycles will be
     *  decremented with 1 before writing to HW  */
    timer_cycles -= 1;

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Set Grant size */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GRANT_SIZE, INST_SINGLE, grant_size);

    /** Set Timer cycles */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TIMER_CYCLES, INST_SINGLE, timer_cycles);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to get actual ingress rate grant size and 
 *        timer cycles
 *  
 * \param unit - unit
 * \param dbal_table_id - relevant dbal_table_id retrieved with
 *                      with dnx_ingress_receive_table_id_get
 * \param core - core
 * \param grant_size - grant_size value
 * \param timer_cycles - timer_cycles value 
 * 
 * \return shr_error_e 
 */
shr_error_e
dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_get(
    int unit,
    int dbal_table_id,
    int core,
    uint32 *grant_size,
    uint32 *timer_cycles)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Get grant size */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GRANT_SIZE, INST_SINGLE, grant_size);

    /** Get timer cycles */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TIMER_CYCLES, INST_SINGLE, timer_cycles);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Timer cycles == 0 means every clock cycle. This means
     *  that for correct calculations timer_cycles will be
     *  incremented by 1 from the HW value  */
    *timer_cycles += 1;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
