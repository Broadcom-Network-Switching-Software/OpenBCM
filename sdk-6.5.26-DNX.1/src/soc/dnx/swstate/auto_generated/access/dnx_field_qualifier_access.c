
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_qualifier_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_qualifier_layout.h>





int
dnx_field_qual_sw_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]),
        "dnx_field_qual_sw_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_init(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_qual_sw_db_t,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_qual_sw_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_field_qualifier_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]),
        "dnx_field_qual_sw_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]),
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_QUAL_USER_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[def_val_idx[1]].field_id,
        DBAL_FIELD_EMPTY);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_QUAL_USER_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[def_val_idx[1]].bcm_id,
        bcmFieldQualifyCount);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_QUAL_PREDEFINED_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->predefined[def_val_idx[1]].field_id,
        DBAL_FIELD_EMPTY);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DNX_DATA_MAX_FIELD_QUAL_LAYER_RECORD_NOF)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->layer_record[def_val_idx[1]].field_id,
        DBAL_FIELD_EMPTY);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_set(int unit, uint32 info_idx_0, CONST dnx_field_user_qual_info_t *info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0],
        info,
        dnx_field_user_qual_info_t,
        0,
        "dnx_field_qual_sw_db_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        info,
        "dnx_field_qual_sw_db[%d]->info[%d]",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_get(int unit, uint32 info_idx_0, dnx_field_user_qual_info_t *info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        DNX_SW_STATE_DIAG_READ);

    *info = ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0],
        "dnx_field_qual_sw_db[%d]->info[%d]",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_valid_set(int unit, uint32 info_idx_0, uint32 valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].valid,
        valid,
        uint32,
        0,
        "dnx_field_qual_sw_db_info_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        &valid,
        "dnx_field_qual_sw_db[%d]->info[%d].valid",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_valid_get(int unit, uint32 info_idx_0, uint32 *valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        valid);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        DNX_SW_STATE_DIAG_READ);

    *valid = ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].valid,
        "dnx_field_qual_sw_db[%d]->info[%d].valid",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_field_id_set(int unit, uint32 info_idx_0, dbal_fields_e field_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].field_id,
        field_id,
        dbal_fields_e,
        0,
        "dnx_field_qual_sw_db_info_field_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        &field_id,
        "dnx_field_qual_sw_db[%d]->info[%d].field_id",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_FIELD_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_field_id_get(int unit, uint32 info_idx_0, dbal_fields_e *field_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        field_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        DNX_SW_STATE_DIAG_READ);

    *field_id = ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].field_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].field_id,
        "dnx_field_qual_sw_db[%d]->info[%d].field_id",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_FIELD_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_size_set(int unit, uint32 info_idx_0, uint32 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].size,
        size,
        uint32,
        0,
        "dnx_field_qual_sw_db_info_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        &size,
        "dnx_field_qual_sw_db[%d]->info[%d].size",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_size_get(int unit, uint32 info_idx_0, uint32 *size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        DNX_SW_STATE_DIAG_READ);

    *size = ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].size,
        "dnx_field_qual_sw_db[%d]->info[%d].size",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_bcm_id_set(int unit, uint32 info_idx_0, int bcm_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].bcm_id,
        bcm_id,
        int,
        0,
        "dnx_field_qual_sw_db_info_bcm_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        &bcm_id,
        "dnx_field_qual_sw_db[%d]->info[%d].bcm_id",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_BCM_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_bcm_id_get(int unit, uint32 info_idx_0, int *bcm_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        bcm_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        DNX_SW_STATE_DIAG_READ);

    *bcm_id = ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].bcm_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].bcm_id,
        "dnx_field_qual_sw_db[%d]->info[%d].bcm_id",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_BCM_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_ref_count_set(int unit, uint32 info_idx_0, uint32 ref_count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].ref_count,
        ref_count,
        uint32,
        0,
        "dnx_field_qual_sw_db_info_ref_count_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        &ref_count,
        "dnx_field_qual_sw_db[%d]->info[%d].ref_count",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_REF_COUNT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_ref_count_get(int unit, uint32 info_idx_0, uint32 *ref_count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        ref_count);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        DNX_SW_STATE_DIAG_READ);

    *ref_count = ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].ref_count;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].ref_count,
        "dnx_field_qual_sw_db[%d]->info[%d].ref_count",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_REF_COUNT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_flags_set(int unit, uint32 info_idx_0, dnx_field_qual_flags_e flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].flags,
        flags,
        dnx_field_qual_flags_e,
        0,
        "dnx_field_qual_sw_db_info_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        &flags,
        "dnx_field_qual_sw_db[%d]->info[%d].flags",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_info_flags_get(int unit, uint32 info_idx_0, dnx_field_qual_flags_e *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        info_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_USER_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        flags);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        DNX_SW_STATE_DIAG_READ);

    *flags = ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[info_idx_0].flags,
        "dnx_field_qual_sw_db[%d]->info[%d].flags",
        unit, info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_INFO_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_predefined_set(int unit, uint32 predefined_idx_0, CONST dnx_field_predefined_qual_info_t *predefined)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predefined_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_PREDEFINED_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->predefined[predefined_idx_0],
        predefined,
        dnx_field_predefined_qual_info_t,
        0,
        "dnx_field_qual_sw_db_predefined_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        predefined,
        "dnx_field_qual_sw_db[%d]->predefined[%d]",
        unit, predefined_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_PREDEFINED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_predefined_get(int unit, uint32 predefined_idx_0, dnx_field_predefined_qual_info_t *predefined)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predefined_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_PREDEFINED_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        predefined);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        DNX_SW_STATE_DIAG_READ);

    *predefined = ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->predefined[predefined_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->predefined[predefined_idx_0],
        "dnx_field_qual_sw_db[%d]->predefined[%d]",
        unit, predefined_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_PREDEFINED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_predefined_field_id_set(int unit, uint32 predefined_idx_0, dbal_fields_e field_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predefined_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_PREDEFINED_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->predefined[predefined_idx_0].field_id,
        field_id,
        dbal_fields_e,
        0,
        "dnx_field_qual_sw_db_predefined_field_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        &field_id,
        "dnx_field_qual_sw_db[%d]->predefined[%d].field_id",
        unit, predefined_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_PREDEFINED_FIELD_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_predefined_field_id_get(int unit, uint32 predefined_idx_0, dbal_fields_e *field_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        predefined_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_PREDEFINED_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        field_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        DNX_SW_STATE_DIAG_READ);

    *field_id = ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->predefined[predefined_idx_0].field_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->predefined[predefined_idx_0].field_id,
        "dnx_field_qual_sw_db[%d]->predefined[%d].field_id",
        unit, predefined_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_PREDEFINED_FIELD_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_layer_record_set(int unit, uint32 layer_record_idx_0, CONST dnx_field_predefined_qual_info_t *layer_record)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        layer_record_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_LAYER_RECORD_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->layer_record[layer_record_idx_0],
        layer_record,
        dnx_field_predefined_qual_info_t,
        0,
        "dnx_field_qual_sw_db_layer_record_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        layer_record,
        "dnx_field_qual_sw_db[%d]->layer_record[%d]",
        unit, layer_record_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_LAYER_RECORD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_layer_record_get(int unit, uint32 layer_record_idx_0, dnx_field_predefined_qual_info_t *layer_record)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        layer_record_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_LAYER_RECORD_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        layer_record);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        DNX_SW_STATE_DIAG_READ);

    *layer_record = ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->layer_record[layer_record_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->layer_record[layer_record_idx_0],
        "dnx_field_qual_sw_db[%d]->layer_record[%d]",
        unit, layer_record_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_LAYER_RECORD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_layer_record_field_id_set(int unit, uint32 layer_record_idx_0, dbal_fields_e field_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        layer_record_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_LAYER_RECORD_NOF);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->layer_record[layer_record_idx_0].field_id,
        field_id,
        dbal_fields_e,
        0,
        "dnx_field_qual_sw_db_layer_record_field_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        &field_id,
        "dnx_field_qual_sw_db[%d]->layer_record[%d].field_id",
        unit, layer_record_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_LAYER_RECORD_FIELD_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_qual_sw_db_layer_record_field_id_get(int unit, uint32 layer_record_idx_0, dbal_fields_e *field_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        layer_record_idx_0,
        DNX_DATA_MAX_FIELD_QUAL_LAYER_RECORD_NOF);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        field_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        DNX_SW_STATE_DIAG_READ);

    *field_id = ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->layer_record[layer_record_idx_0].field_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->layer_record[layer_record_idx_0].field_id,
        "dnx_field_qual_sw_db[%d]->layer_record[%d].field_id",
        unit, layer_record_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
        dnx_field_qual_sw_db_info,
        DNX_FIELD_QUAL_SW_DB_LAYER_RECORD_FIELD_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



const char *
dnx_field_qual_flags_e_get_name(dnx_field_qual_flags_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_QUALIFIER_FLAG_WITH_ID", value, DNX_FIELD_QUALIFIER_FLAG_WITH_ID);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DNX_FIELD_QUALIFIER_FLAG_NOF", value, DNX_FIELD_QUALIFIER_FLAG_NOF);

    return NULL;
}




dnx_field_qual_sw_db_cbs dnx_field_qual_sw_db = 	{
	
	dnx_field_qual_sw_db_is_init,
	dnx_field_qual_sw_db_init,
		{
		
		dnx_field_qual_sw_db_info_set,
		dnx_field_qual_sw_db_info_get,
			{
			
			dnx_field_qual_sw_db_info_valid_set,
			dnx_field_qual_sw_db_info_valid_get}
		,
			{
			
			dnx_field_qual_sw_db_info_field_id_set,
			dnx_field_qual_sw_db_info_field_id_get}
		,
			{
			
			dnx_field_qual_sw_db_info_size_set,
			dnx_field_qual_sw_db_info_size_get}
		,
			{
			
			dnx_field_qual_sw_db_info_bcm_id_set,
			dnx_field_qual_sw_db_info_bcm_id_get}
		,
			{
			
			dnx_field_qual_sw_db_info_ref_count_set,
			dnx_field_qual_sw_db_info_ref_count_get}
		,
			{
			
			dnx_field_qual_sw_db_info_flags_set,
			dnx_field_qual_sw_db_info_flags_get}
		}
	,
		{
		
		dnx_field_qual_sw_db_predefined_set,
		dnx_field_qual_sw_db_predefined_get,
			{
			
			dnx_field_qual_sw_db_predefined_field_id_set,
			dnx_field_qual_sw_db_predefined_field_id_get}
		}
	,
		{
		
		dnx_field_qual_sw_db_layer_record_set,
		dnx_field_qual_sw_db_layer_record_get,
			{
			
			dnx_field_qual_sw_db_layer_record_field_id_set,
			dnx_field_qual_sw_db_layer_record_field_id_get}
		}
	}
;
#undef BSL_LOG_MODULE
