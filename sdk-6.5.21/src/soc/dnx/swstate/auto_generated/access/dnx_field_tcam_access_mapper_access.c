
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_mapper_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_mapper_diagnostic.h>



dnx_field_tcam_access_mapper_t* dnx_field_tcam_access_mapper_sw_dummy = NULL;



int
dnx_field_tcam_access_mapper_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]),
        "dnx_field_tcam_access_mapper_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_tcam_access_mapper_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_mapper_sw_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]),
        "dnx_field_tcam_access_mapper_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_field_tcam_access_mapper_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_location_hash_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_data_device.general.nof_cores_get(unit),
        dnx_data_field.tcam.nof_banks_get(unit),
        sizeof(dnx_field_tcam_access_mapper_hash*));

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash,
        dnx_field_tcam_access_mapper_hash*,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_mapper_sw_key_2_location_hash_alloc");

    for(uint32 key_2_location_hash_idx_0 = 0;
         key_2_location_hash_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         key_2_location_hash_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0],
        dnx_field_tcam_access_mapper_hash,
        dnx_data_field.tcam.nof_banks_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_mapper_sw_key_2_location_hash_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash,
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_location_hash",
        unit,
        dnx_data_field.tcam.nof_banks_get(unit) * sizeof(dnx_field_tcam_access_mapper_hash) / sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash)+(dnx_data_field.tcam.nof_banks_get(unit) * sizeof(dnx_field_tcam_access_mapper_hash)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_LOCATION_HASH_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_set(int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 head_idx_0, uint32 head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash,
        key_2_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].head);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0],
        key_2_location_hash_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].head,
        head_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].head[head_idx_0],
        head,
        uint32,
        0,
        "dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &head,
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_location_hash[%d][%d].head[%d]",
        unit, key_2_location_hash_idx_0, key_2_location_hash_idx_1, head_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_LOCATION_HASH_HEAD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_get(int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 head_idx_0, uint32 *head)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash,
        key_2_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        head);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0],
        key_2_location_hash_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].head);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].head,
        head_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    *head = ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].head[head_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].head[head_idx_0],
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_location_hash[%d][%d].head[%d]",
        unit, key_2_location_hash_idx_0, key_2_location_hash_idx_1, head_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_LOCATION_HASH_HEAD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_alloc(int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 nof_instances_to_alloc_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash,
        key_2_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0],
        key_2_location_hash_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].head,
        uint32,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].head,
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_location_hash[%d][%d].head",
        unit, key_2_location_hash_idx_0, key_2_location_hash_idx_1,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].head));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_LOCATION_HASH_HEAD_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].head[def_val_idx[0]],
        DNX_FIELD_TCAM_ACCESS_HASH_NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_set(int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 next_idx_0, uint32 next)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash,
        key_2_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].next);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0],
        key_2_location_hash_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].next,
        next_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].next[next_idx_0],
        next,
        uint32,
        0,
        "dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &next,
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_location_hash[%d][%d].next[%d]",
        unit, key_2_location_hash_idx_0, key_2_location_hash_idx_1, next_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_LOCATION_HASH_NEXT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_get(int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 next_idx_0, uint32 *next)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash,
        key_2_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        next);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0],
        key_2_location_hash_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].next);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].next,
        next_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    *next = ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].next[next_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].next[next_idx_0],
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_location_hash[%d][%d].next[%d]",
        unit, key_2_location_hash_idx_0, key_2_location_hash_idx_1, next_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_LOCATION_HASH_NEXT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_alloc(int unit, uint32 key_2_location_hash_idx_0, uint32 key_2_location_hash_idx_1, uint32 nof_instances_to_alloc_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash,
        key_2_location_hash_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0],
        key_2_location_hash_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].next,
        uint32,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].next,
        "dnx_field_tcam_access_mapper_sw[%d]->key_2_location_hash[%d][%d].next",
        unit, key_2_location_hash_idx_0, key_2_location_hash_idx_1,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].next));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
        dnx_field_tcam_access_mapper_sw_info,
        DNX_FIELD_TCAM_ACCESS_MAPPER_SW_KEY_2_LOCATION_HASH_NEXT_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[key_2_location_hash_idx_0][key_2_location_hash_idx_1].next[def_val_idx[0]],
        DNX_FIELD_TCAM_ACCESS_HASH_NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_field_tcam_access_mapper_sw_cbs dnx_field_tcam_access_mapper_sw = 	{
	
	dnx_field_tcam_access_mapper_sw_is_init,
	dnx_field_tcam_access_mapper_sw_init,
		{
		
		dnx_field_tcam_access_mapper_sw_key_2_location_hash_alloc,
			{
			
			dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_set,
			dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_get,
			dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_alloc}
		,
			{
			
			dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_set,
			dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_get,
			dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_alloc}
		}
	}
;
#undef BSL_LOG_MODULE
