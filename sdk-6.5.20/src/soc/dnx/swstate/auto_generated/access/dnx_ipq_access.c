
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ipq_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_ipq_diagnostic.h>



dnx_ipq_db_t* dnx_ipq_db_dummy = NULL;



int
dnx_ipq_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]),
        "dnx_ipq_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_IPQ_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_ipq_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_ipq_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]),
        "dnx_ipq_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_ipq_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queues_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues,
        dnx_ipq_base_queue_t*,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_ipq_db_base_queues_alloc");

    for(uint32 base_queues_idx_0 = 0;
         base_queues_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         base_queues_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0],
        dnx_ipq_base_queue_t,
        dnx_data_ipq.queues.nof_queues_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_ipq_db_base_queues_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues,
        "dnx_ipq_db[%d]->base_queues",
        unit,
        dnx_data_ipq.queues.nof_queues_get(unit) * sizeof(dnx_ipq_base_queue_t) / sizeof(*((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues)+(dnx_data_ipq.queues.nof_queues_get(unit) * sizeof(dnx_ipq_base_queue_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queues_num_cos_set(int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, uint8 num_cos)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues,
        base_queues_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0],
        base_queues_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0][base_queues_idx_1].num_cos,
        num_cos,
        uint8,
        0,
        "dnx_ipq_db_base_queues_num_cos_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_IPQ_MODULE_ID,
        &num_cos,
        "dnx_ipq_db[%d]->base_queues[%d][%d].num_cos",
        unit, base_queues_idx_0, base_queues_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUES_NUM_COS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queues_num_cos_get(int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, uint8 *num_cos)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues,
        base_queues_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        num_cos);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0],
        base_queues_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    *num_cos = ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0][base_queues_idx_1].num_cos;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0][base_queues_idx_1].num_cos,
        "dnx_ipq_db[%d]->base_queues[%d][%d].num_cos",
        unit, base_queues_idx_0, base_queues_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUES_NUM_COS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queues_sys_port_ref_counter_set(int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, int sys_port_ref_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues,
        base_queues_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0],
        base_queues_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0][base_queues_idx_1].sys_port_ref_counter,
        sys_port_ref_counter,
        int,
        0,
        "dnx_ipq_db_base_queues_sys_port_ref_counter_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_IPQ_MODULE_ID,
        &sys_port_ref_counter,
        "dnx_ipq_db[%d]->base_queues[%d][%d].sys_port_ref_counter",
        unit, base_queues_idx_0, base_queues_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUES_SYS_PORT_REF_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queues_sys_port_ref_counter_get(int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, int *sys_port_ref_counter)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues,
        base_queues_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        sys_port_ref_counter);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0],
        base_queues_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    *sys_port_ref_counter = ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0][base_queues_idx_1].sys_port_ref_counter;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0][base_queues_idx_1].sys_port_ref_counter,
        "dnx_ipq_db[%d]->base_queues[%d][%d].sys_port_ref_counter",
        unit, base_queues_idx_0, base_queues_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUES_SYS_PORT_REF_COUNTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queues_sys_port_ref_counter_inc(int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0][base_queues_idx_1].sys_port_ref_counter,
        inc_value,
        int,
        0,
        "dnx_ipq_db_base_queues_sys_port_ref_counter_inc");

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUES_SYS_PORT_REF_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queues_sys_port_ref_counter_dec(int unit, uint32 base_queues_idx_0, uint32 base_queues_idx_1, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues,
        base_queues_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0],
        base_queues_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queues[base_queues_idx_0][base_queues_idx_1].sys_port_ref_counter,
        dec_value,
        int,
        0,
        "dnx_ipq_db_base_queues_sys_port_ref_counter_dec");

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUES_SYS_PORT_REF_COUNTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_alloc_bitmap(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        dnx_data_ipq.queues.nof_queues_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_ipq_db_base_queue_is_asymm_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit,
        dnx_data_ipq.queues.nof_queues_get(unit));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_clear(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_read(int unit, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_write(int unit, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_and(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_or(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_xor(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_remove(int unit, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_negate(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_clear(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_set(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_null(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_test(int unit, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_eq(int unit, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_ipq_db_base_queue_is_asymm_bit_range_count(int unit, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_IPQ_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_IPQ_MODULE_ID,
        sw_state_roots_array[unit][DNX_IPQ_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_IPQ_MODULE_ID,
        ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_IPQ_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_IPQ_MODULE_ID,
         0,
         ((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_IPQ_MODULE_ID,
        &((dnx_ipq_db_t*)sw_state_roots_array[unit][DNX_IPQ_MODULE_ID])->base_queue_is_asymm,
        "dnx_ipq_db[%d]->base_queue_is_asymm",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_IPQ_MODULE_ID,
        dnx_ipq_db_info,
        DNX_IPQ_DB_BASE_QUEUE_IS_ASYMM_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_ipq_db_cbs dnx_ipq_db = 	{
	
	dnx_ipq_db_is_init,
	dnx_ipq_db_init,
		{
		
		dnx_ipq_db_base_queues_alloc,
			{
			
			dnx_ipq_db_base_queues_num_cos_set,
			dnx_ipq_db_base_queues_num_cos_get}
		,
			{
			
			dnx_ipq_db_base_queues_sys_port_ref_counter_set,
			dnx_ipq_db_base_queues_sys_port_ref_counter_get,
			dnx_ipq_db_base_queues_sys_port_ref_counter_inc,
			dnx_ipq_db_base_queues_sys_port_ref_counter_dec}
		}
	,
		{
		
		dnx_ipq_db_base_queue_is_asymm_alloc_bitmap,
		dnx_ipq_db_base_queue_is_asymm_bit_set,
		dnx_ipq_db_base_queue_is_asymm_bit_clear,
		dnx_ipq_db_base_queue_is_asymm_bit_get,
		dnx_ipq_db_base_queue_is_asymm_bit_range_read,
		dnx_ipq_db_base_queue_is_asymm_bit_range_write,
		dnx_ipq_db_base_queue_is_asymm_bit_range_and,
		dnx_ipq_db_base_queue_is_asymm_bit_range_or,
		dnx_ipq_db_base_queue_is_asymm_bit_range_xor,
		dnx_ipq_db_base_queue_is_asymm_bit_range_remove,
		dnx_ipq_db_base_queue_is_asymm_bit_range_negate,
		dnx_ipq_db_base_queue_is_asymm_bit_range_clear,
		dnx_ipq_db_base_queue_is_asymm_bit_range_set,
		dnx_ipq_db_base_queue_is_asymm_bit_range_null,
		dnx_ipq_db_base_queue_is_asymm_bit_range_test,
		dnx_ipq_db_base_queue_is_asymm_bit_range_eq,
		dnx_ipq_db_base_queue_is_asymm_bit_range_count}
	}
;
#undef BSL_LOG_MODULE
