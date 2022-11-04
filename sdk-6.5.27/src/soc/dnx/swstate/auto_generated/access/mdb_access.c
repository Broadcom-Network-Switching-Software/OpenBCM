
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/mdb_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/mdb_layout.h>





int
mdb_db_infos_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_MDB_DB_INFOS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]),
        "mdb_db_infos[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_init(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        mdb_tables_info,
        DNX_SW_STATE_MDB_DB_INFOS_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_init",
        DNX_SW_STATE_DIAG_ALLOC,
        mdb_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_MDB_DB_INFOS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]),
        "mdb_db_infos[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS,
        mdb_db_infos_info,
        MDB_DB_INFOS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID]),
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DBAL_NOF_PHYSICAL_TABLES)

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[2], DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[def_val_idx[1]].em_bank_info[def_val_idx[2]].macro_type,
        MDB_NOF_MACRO_TYPES);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_payload_type[def_val_idx[1]],
        DBAL_NOF_ENUM_DIRECT_PAYLOAD_SIZES_VALUES);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.cache_enabled,
        TRUE);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_set(int unit, uint32 em_key_format_idx_0, CONST em_key_format_info *em_key_format)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0],
        em_key_format,
        em_key_format_info,
        0,
        "mdb_db_infos_em_key_format_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        em_key_format,
        "mdb_db_infos[%d]->em_key_format[%d]",
        unit, em_key_format_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_get(int unit, uint32 em_key_format_idx_0, em_key_format_info *em_key_format)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        em_key_format);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        DNX_SW_STATE_DIAG_READ);

    *em_key_format = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0],
        "mdb_db_infos[%d]->em_key_format[%d]",
        unit, em_key_format_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_key_size_set(int unit, uint32 em_key_format_idx_0, uint32 key_size_idx_0, uint8 key_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_size_idx_0,
        DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].key_size[key_size_idx_0],
        key_size,
        uint8,
        0,
        "mdb_db_infos_em_key_format_key_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        &key_size,
        "mdb_db_infos[%d]->em_key_format[%d].key_size[%d]",
        unit, em_key_format_idx_0, key_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_KEY_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_key_size_get(int unit, uint32 em_key_format_idx_0, uint32 key_size_idx_0, uint8 *key_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        key_size);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        key_size_idx_0,
        DNX_DATA_MAX_MDB_EM_MAX_NOF_TIDS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *key_size = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].key_size[key_size_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].key_size[key_size_idx_0],
        "mdb_db_infos[%d]->em_key_format[%d].key_size[%d]",
        unit, em_key_format_idx_0, key_size_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__KEY_SIZE,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_KEY_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_em_bank_info_is_flex_set(int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint8 is_flex)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_bank_info_idx_0,
        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].is_flex,
        is_flex,
        uint8,
        0,
        "mdb_db_infos_em_key_format_em_bank_info_is_flex_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        &is_flex,
        "mdb_db_infos[%d]->em_key_format[%d].em_bank_info[%d].is_flex",
        unit, em_key_format_idx_0, em_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_IS_FLEX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_em_bank_info_is_flex_get(int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint8 *is_flex)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        is_flex);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_bank_info_idx_0,
        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        DNX_SW_STATE_DIAG_READ);

    *is_flex = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].is_flex;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].is_flex,
        "mdb_db_infos[%d]->em_key_format[%d].em_bank_info[%d].is_flex",
        unit, em_key_format_idx_0, em_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__IS_FLEX,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_IS_FLEX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_em_bank_info_macro_index_set(int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint8 macro_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_bank_info_idx_0,
        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].macro_index,
        macro_index,
        uint8,
        0,
        "mdb_db_infos_em_key_format_em_bank_info_macro_index_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        &macro_index,
        "mdb_db_infos[%d]->em_key_format[%d].em_bank_info[%d].macro_index",
        unit, em_key_format_idx_0, em_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_MACRO_INDEX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_em_bank_info_macro_index_get(int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint8 *macro_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        macro_index);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_bank_info_idx_0,
        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        DNX_SW_STATE_DIAG_READ);

    *macro_index = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].macro_index;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].macro_index,
        "mdb_db_infos[%d]->em_key_format[%d].em_bank_info[%d].macro_index",
        unit, em_key_format_idx_0, em_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_INDEX,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_MACRO_INDEX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_em_bank_info_bank_size_used_nof_rows_set(int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint32 bank_size_used_nof_rows)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_bank_info_idx_0,
        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].bank_size_used_nof_rows,
        bank_size_used_nof_rows,
        uint32,
        0,
        "mdb_db_infos_em_key_format_em_bank_info_bank_size_used_nof_rows_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        &bank_size_used_nof_rows,
        "mdb_db_infos[%d]->em_key_format[%d].em_bank_info[%d].bank_size_used_nof_rows",
        unit, em_key_format_idx_0, em_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_BANK_SIZE_USED_NOF_ROWS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_em_bank_info_bank_size_used_nof_rows_get(int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint32 *bank_size_used_nof_rows)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        bank_size_used_nof_rows);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_bank_info_idx_0,
        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        DNX_SW_STATE_DIAG_READ);

    *bank_size_used_nof_rows = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].bank_size_used_nof_rows;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].bank_size_used_nof_rows,
        "mdb_db_infos[%d]->em_key_format[%d].em_bank_info[%d].bank_size_used_nof_rows",
        unit, em_key_format_idx_0, em_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__BANK_SIZE_USED_NOF_ROWS,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_BANK_SIZE_USED_NOF_ROWS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_em_bank_info_macro_type_set(int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, mdb_macro_types_e macro_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_bank_info_idx_0,
        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].macro_type,
        macro_type,
        mdb_macro_types_e,
        0,
        "mdb_db_infos_em_key_format_em_bank_info_macro_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        &macro_type,
        "mdb_db_infos[%d]->em_key_format[%d].em_bank_info[%d].macro_type",
        unit, em_key_format_idx_0, em_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_MACRO_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_em_bank_info_macro_type_get(int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, mdb_macro_types_e *macro_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        macro_type);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_bank_info_idx_0,
        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *macro_type = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].macro_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].macro_type,
        "mdb_db_infos[%d]->em_key_format[%d].em_bank_info[%d].macro_type",
        unit, em_key_format_idx_0, em_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__MACRO_TYPE,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_MACRO_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_set(int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint32 cluster_mapping)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_bank_info_idx_0,
        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].cluster_mapping,
        cluster_mapping,
        uint32,
        0,
        "mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        &cluster_mapping,
        "mdb_db_infos[%d]->em_key_format[%d].em_bank_info[%d].cluster_mapping",
        unit, em_key_format_idx_0, em_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_CLUSTER_MAPPING_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_get(int unit, uint32 em_key_format_idx_0, uint32 em_bank_info_idx_0, uint32 *cluster_mapping)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        cluster_mapping);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_bank_info_idx_0,
        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        DNX_SW_STATE_DIAG_READ);

    *cluster_mapping = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].cluster_mapping;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].em_bank_info[em_bank_info_idx_0].cluster_mapping,
        "mdb_db_infos[%d]->em_key_format[%d].em_bank_info[%d].cluster_mapping",
        unit, em_key_format_idx_0, em_bank_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__EM_BANK_INFO__CLUSTER_MAPPING,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_EM_BANK_INFO_CLUSTER_MAPPING_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_nof_address_bits_set(int unit, uint32 em_key_format_idx_0, uint32 nof_address_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].nof_address_bits,
        nof_address_bits,
        uint32,
        0,
        "mdb_db_infos_em_key_format_nof_address_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        &nof_address_bits,
        "mdb_db_infos[%d]->em_key_format[%d].nof_address_bits",
        unit, em_key_format_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_NOF_ADDRESS_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_nof_address_bits_get(int unit, uint32 em_key_format_idx_0, uint32 *nof_address_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        nof_address_bits);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        DNX_SW_STATE_DIAG_READ);

    *nof_address_bits = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].nof_address_bits;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].nof_address_bits,
        "mdb_db_infos[%d]->em_key_format[%d].nof_address_bits",
        unit, em_key_format_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_ADDRESS_BITS,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_NOF_ADDRESS_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_nof_ways_set(int unit, uint32 em_key_format_idx_0, uint32 nof_ways)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].nof_ways,
        nof_ways,
        uint32,
        0,
        "mdb_db_infos_em_key_format_nof_ways_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        &nof_ways,
        "mdb_db_infos[%d]->em_key_format[%d].nof_ways",
        unit, em_key_format_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_NOF_WAYS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_nof_ways_get(int unit, uint32 em_key_format_idx_0, uint32 *nof_ways)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        nof_ways);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        DNX_SW_STATE_DIAG_READ);

    *nof_ways = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].nof_ways;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].nof_ways,
        "mdb_db_infos[%d]->em_key_format[%d].nof_ways",
        unit, em_key_format_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_WAYS,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_NOF_WAYS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_nof_flex_ways_set(int unit, uint32 em_key_format_idx_0, uint32 nof_flex_ways)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].nof_flex_ways,
        nof_flex_ways,
        uint32,
        0,
        "mdb_db_infos_em_key_format_nof_flex_ways_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        &nof_flex_ways,
        "mdb_db_infos[%d]->em_key_format[%d].nof_flex_ways",
        unit, em_key_format_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_NOF_FLEX_WAYS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_key_format_nof_flex_ways_get(int unit, uint32 em_key_format_idx_0, uint32 *nof_flex_ways)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_key_format_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        nof_flex_ways);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        DNX_SW_STATE_DIAG_READ);

    *nof_flex_ways = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].nof_flex_ways;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_key_format[em_key_format_idx_0].nof_flex_ways,
        "mdb_db_infos[%d]->em_key_format[%d].nof_flex_ways",
        unit, em_key_format_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_KEY_FORMAT__NOF_FLEX_WAYS,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_KEY_FORMAT_NOF_FLEX_WAYS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_number_of_alloc_clusters_set(int unit, uint32 db_idx_0, int number_of_alloc_clusters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].number_of_alloc_clusters,
        number_of_alloc_clusters,
        int,
        0,
        "mdb_db_infos_db_number_of_alloc_clusters_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        &number_of_alloc_clusters,
        "mdb_db_infos[%d]->db[%d].number_of_alloc_clusters",
        unit, db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_NUMBER_OF_ALLOC_CLUSTERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_number_of_alloc_clusters_get(int unit, uint32 db_idx_0, int *number_of_alloc_clusters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        number_of_alloc_clusters);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        DNX_SW_STATE_DIAG_READ);

    *number_of_alloc_clusters = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].number_of_alloc_clusters;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].number_of_alloc_clusters,
        "mdb_db_infos[%d]->db[%d].number_of_alloc_clusters",
        unit, db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_ALLOC_CLUSTERS,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_NUMBER_OF_ALLOC_CLUSTERS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_number_of_clusters_set(int unit, uint32 db_idx_0, int number_of_clusters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].number_of_clusters,
        number_of_clusters,
        int,
        0,
        "mdb_db_infos_db_number_of_clusters_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        &number_of_clusters,
        "mdb_db_infos[%d]->db[%d].number_of_clusters",
        unit, db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_NUMBER_OF_CLUSTERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_number_of_clusters_get(int unit, uint32 db_idx_0, int *number_of_clusters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        number_of_clusters);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        DNX_SW_STATE_DIAG_READ);

    *number_of_clusters = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].number_of_clusters;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].number_of_clusters,
        "mdb_db_infos[%d]->db[%d].number_of_clusters",
        unit, db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NUMBER_OF_CLUSTERS,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_NUMBER_OF_CLUSTERS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_nof_cluster_per_entry_set(int unit, uint32 db_idx_0, int nof_cluster_per_entry)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].nof_cluster_per_entry,
        nof_cluster_per_entry,
        int,
        0,
        "mdb_db_infos_db_nof_cluster_per_entry_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        &nof_cluster_per_entry,
        "mdb_db_infos[%d]->db[%d].nof_cluster_per_entry",
        unit, db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_NOF_CLUSTER_PER_ENTRY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_nof_cluster_per_entry_get(int unit, uint32 db_idx_0, int *nof_cluster_per_entry)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        nof_cluster_per_entry);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        DNX_SW_STATE_DIAG_READ);

    *nof_cluster_per_entry = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].nof_cluster_per_entry;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].nof_cluster_per_entry,
        "mdb_db_infos[%d]->db[%d].nof_cluster_per_entry",
        unit, db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__NOF_CLUSTER_PER_ENTRY,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_NOF_CLUSTER_PER_ENTRY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_set(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, CONST mdb_cluster_info_t *clusters_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0],
        clusters_info,
        mdb_cluster_info_t,
        0,
        "mdb_db_infos_db_clusters_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        clusters_info,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d]",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_get(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, mdb_cluster_info_t *clusters_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        clusters_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        DNX_SW_STATE_DIAG_READ);

    *clusters_info = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0],
        "mdb_db_infos[%d]->db[%d].clusters_info[%d]",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_alloc(int unit, uint32 db_idx_0, uint32 nof_instances_to_alloc_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        mdb_cluster_info_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][MDB_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_db_clusters_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        "mdb_db_infos[%d]->db[%d].clusters_info",
        unit, db_idx_0,
        nof_instances_to_alloc_0 * sizeof(mdb_cluster_info_t) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[def_val_idx[0]].macro_type,
        MDB_NOF_MACRO_TYPES);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_macro_type_set(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, mdb_macro_types_e macro_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].macro_type,
        macro_type,
        mdb_macro_types_e,
        0,
        "mdb_db_infos_db_clusters_info_macro_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        &macro_type,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].macro_type",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_MACRO_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_macro_type_get(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, mdb_macro_types_e *macro_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        macro_type);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *macro_type = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].macro_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].macro_type,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].macro_type",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_TYPE,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_MACRO_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_macro_index_set(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 macro_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].macro_index,
        macro_index,
        uint8,
        0,
        "mdb_db_infos_db_clusters_info_macro_index_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        &macro_index,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].macro_index",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_MACRO_INDEX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_macro_index_get(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 *macro_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        macro_index);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        DNX_SW_STATE_DIAG_READ);

    *macro_index = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].macro_index;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].macro_index,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].macro_index",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__MACRO_INDEX,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_MACRO_INDEX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_cluster_index_set(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 cluster_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].cluster_index,
        cluster_index,
        uint8,
        0,
        "mdb_db_infos_db_clusters_info_cluster_index_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        &cluster_index,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].cluster_index",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_CLUSTER_INDEX_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_cluster_index_get(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 *cluster_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        cluster_index);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        DNX_SW_STATE_DIAG_READ);

    *cluster_index = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].cluster_index;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].cluster_index,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].cluster_index",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_INDEX,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_CLUSTER_INDEX_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_cluster_position_in_entry_set(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 cluster_position_in_entry)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].cluster_position_in_entry,
        cluster_position_in_entry,
        uint8,
        0,
        "mdb_db_infos_db_clusters_info_cluster_position_in_entry_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        &cluster_position_in_entry,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].cluster_position_in_entry",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_CLUSTER_POSITION_IN_ENTRY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_cluster_position_in_entry_get(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint8 *cluster_position_in_entry)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        cluster_position_in_entry);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        DNX_SW_STATE_DIAG_READ);

    *cluster_position_in_entry = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].cluster_position_in_entry;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].cluster_position_in_entry,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].cluster_position_in_entry",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__CLUSTER_POSITION_IN_ENTRY,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_CLUSTER_POSITION_IN_ENTRY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_start_address_set(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint32 start_address)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].start_address,
        start_address,
        uint32,
        0,
        "mdb_db_infos_db_clusters_info_start_address_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        &start_address,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].start_address",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_START_ADDRESS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_start_address_get(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint32 *start_address)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        start_address);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        DNX_SW_STATE_DIAG_READ);

    *start_address = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].start_address;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].start_address,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].start_address",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__START_ADDRESS,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_START_ADDRESS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_end_address_set(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint32 end_address)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].end_address,
        end_address,
        uint32,
        0,
        "mdb_db_infos_db_clusters_info_end_address_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        &end_address,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].end_address",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_END_ADDRESS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_clusters_info_end_address_get(int unit, uint32 db_idx_0, uint32 clusters_info_idx_0, uint32 *end_address)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        end_address);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info,
        clusters_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        DNX_SW_STATE_DIAG_READ);

    *end_address = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].end_address;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].clusters_info[clusters_info_idx_0].end_address,
        "mdb_db_infos[%d]->db[%d].clusters_info[%d].end_address",
        unit, db_idx_0, clusters_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__CLUSTERS_INFO__END_ADDRESS,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_CLUSTERS_INFO_END_ADDRESS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_address_space_bits_set(int unit, uint32 db_idx_0, int address_space_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].address_space_bits,
        address_space_bits,
        int,
        0,
        "mdb_db_infos_db_address_space_bits_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        &address_space_bits,
        "mdb_db_infos[%d]->db[%d].address_space_bits",
        unit, db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_ADDRESS_SPACE_BITS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_db_address_space_bits_get(int unit, uint32 db_idx_0, int *address_space_bits)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        db_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        address_space_bits);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        DNX_SW_STATE_DIAG_READ);

    *address_space_bits = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].address_space_bits;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->db[db_idx_0].address_space_bits,
        "mdb_db_infos[%d]->db[%d].address_space_bits",
        unit, db_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__DB__ADDRESS_SPACE_BITS,
        mdb_db_infos_info,
        MDB_DB_INFOS_DB_ADDRESS_SPACE_BITS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_capacity_set(int unit, uint32 capacity_idx_0, int capacity)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        capacity_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->capacity[capacity_idx_0],
        capacity,
        int,
        0,
        "mdb_db_infos_capacity_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        &capacity,
        "mdb_db_infos[%d]->capacity[%d]",
        unit, capacity_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        mdb_db_infos_info,
        MDB_DB_INFOS_CAPACITY_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_capacity_get(int unit, uint32 capacity_idx_0, int *capacity)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        capacity_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        capacity);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        DNX_SW_STATE_DIAG_READ);

    *capacity = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->capacity[capacity_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->capacity[capacity_idx_0],
        "mdb_db_infos[%d]->capacity[%d]",
        unit, capacity_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__CAPACITY,
        mdb_db_infos_info,
        MDB_DB_INFOS_CAPACITY_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_entry_capacity_estimate_set(int unit, uint32 em_entry_capacity_estimate_idx_0, uint32 em_entry_capacity_estimate_idx_1, int em_entry_capacity_estimate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_entry_capacity_estimate_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_entry_capacity_estimate_idx_1,
        DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate[em_entry_capacity_estimate_idx_0][em_entry_capacity_estimate_idx_1],
        em_entry_capacity_estimate,
        int,
        0,
        "mdb_db_infos_em_entry_capacity_estimate_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        &em_entry_capacity_estimate,
        "mdb_db_infos[%d]->em_entry_capacity_estimate[%d][%d]",
        unit, em_entry_capacity_estimate_idx_0, em_entry_capacity_estimate_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_ENTRY_CAPACITY_ESTIMATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_entry_capacity_estimate_get(int unit, uint32 em_entry_capacity_estimate_idx_0, uint32 em_entry_capacity_estimate_idx_1, int *em_entry_capacity_estimate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_entry_capacity_estimate_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        em_entry_capacity_estimate);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_entry_capacity_estimate_idx_1,
        DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        DNX_SW_STATE_DIAG_READ);

    *em_entry_capacity_estimate = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate[em_entry_capacity_estimate_idx_0][em_entry_capacity_estimate_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate[em_entry_capacity_estimate_idx_0][em_entry_capacity_estimate_idx_1],
        "mdb_db_infos[%d]->em_entry_capacity_estimate[%d][%d]",
        unit, em_entry_capacity_estimate_idx_0, em_entry_capacity_estimate_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_ENTRY_CAPACITY_ESTIMATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_entry_capacity_estimate_average_set(int unit, uint32 em_entry_capacity_estimate_average_idx_0, uint32 em_entry_capacity_estimate_average_idx_1, int em_entry_capacity_estimate_average)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_entry_capacity_estimate_average_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_entry_capacity_estimate_average_idx_1,
        DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate_average[em_entry_capacity_estimate_average_idx_0][em_entry_capacity_estimate_average_idx_1],
        em_entry_capacity_estimate_average,
        int,
        0,
        "mdb_db_infos_em_entry_capacity_estimate_average_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        &em_entry_capacity_estimate_average,
        "mdb_db_infos[%d]->em_entry_capacity_estimate_average[%d][%d]",
        unit, em_entry_capacity_estimate_average_idx_0, em_entry_capacity_estimate_average_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_em_entry_capacity_estimate_average_get(int unit, uint32 em_entry_capacity_estimate_average_idx_0, uint32 em_entry_capacity_estimate_average_idx_1, int *em_entry_capacity_estimate_average)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_entry_capacity_estimate_average_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        em_entry_capacity_estimate_average);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        em_entry_capacity_estimate_average_idx_1,
        DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        DNX_SW_STATE_DIAG_READ);

    *em_entry_capacity_estimate_average = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate_average[em_entry_capacity_estimate_average_idx_0][em_entry_capacity_estimate_average_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->em_entry_capacity_estimate_average[em_entry_capacity_estimate_average_idx_0][em_entry_capacity_estimate_average_idx_1],
        "mdb_db_infos[%d]->em_entry_capacity_estimate_average[%d][%d]",
        unit, em_entry_capacity_estimate_average_idx_0, em_entry_capacity_estimate_average_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE,
        mdb_db_infos_info,
        MDB_DB_INFOS_EM_ENTRY_CAPACITY_ESTIMATE_AVERAGE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_eedb_payload_type_set(int unit, uint32 eedb_payload_type_idx_0, uint32 eedb_payload_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        eedb_payload_type_idx_0,
        DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_payload_type[eedb_payload_type_idx_0],
        eedb_payload_type,
        uint32,
        0,
        "mdb_db_infos_eedb_payload_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        &eedb_payload_type,
        "mdb_db_infos[%d]->eedb_payload_type[%d]",
        unit, eedb_payload_type_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        mdb_db_infos_info,
        MDB_DB_INFOS_EEDB_PAYLOAD_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_eedb_payload_type_get(int unit, uint32 eedb_payload_type_idx_0, uint32 *eedb_payload_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        eedb_payload_type_idx_0,
        DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        eedb_payload_type);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *eedb_payload_type = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_payload_type[eedb_payload_type_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_payload_type[eedb_payload_type_idx_0],
        "mdb_db_infos[%d]->eedb_payload_type[%d]",
        unit, eedb_payload_type_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_PAYLOAD_TYPE,
        mdb_db_infos_info,
        MDB_DB_INFOS_EEDB_PAYLOAD_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_cache_enabled_set(int unit, uint8 cache_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.cache_enabled,
        cache_enabled,
        uint8,
        0,
        "mdb_db_infos_mdb_cluster_infos_cache_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        &cache_enabled,
        "mdb_db_infos[%d]->mdb_cluster_infos.cache_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_CACHE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_cache_enabled_get(int unit, uint8 *cache_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        cache_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        DNX_SW_STATE_DIAG_READ);

    *cache_enabled = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.cache_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.cache_enabled,
        "mdb_db_infos[%d]->mdb_cluster_infos.cache_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__CACHE_ENABLED,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_CACHE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_alloc(int unit, uint32 macro_cluster_assoc_idx_0, uint32 nof_instances_to_alloc_1)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        macro_cluster_assoc_idx_0,
        MDB_NOF_MACRO_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0],
        cluster_assoc_info,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][MDB_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0],
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_assoc[%d]",
        unit, macro_cluster_assoc_idx_0,
        nof_instances_to_alloc_1 * sizeof(cluster_assoc_info) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_ASSOC_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_1)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0][def_val_idx[0]].pdb,
        MDB_NOF_PHYSICAL_TABLES);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_set(int unit, uint32 macro_cluster_assoc_idx_0, uint32 macro_cluster_assoc_idx_1, mdb_physical_table_e pdb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        macro_cluster_assoc_idx_0,
        MDB_NOF_MACRO_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0],
        macro_cluster_assoc_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0][macro_cluster_assoc_idx_1].pdb,
        pdb,
        mdb_physical_table_e,
        0,
        "mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        &pdb,
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_assoc[%d][%d].pdb",
        unit, macro_cluster_assoc_idx_0, macro_cluster_assoc_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_ASSOC_PDB_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_get(int unit, uint32 macro_cluster_assoc_idx_0, uint32 macro_cluster_assoc_idx_1, mdb_physical_table_e *pdb)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        macro_cluster_assoc_idx_0,
        MDB_NOF_MACRO_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        pdb);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0],
        macro_cluster_assoc_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        DNX_SW_STATE_DIAG_READ);

    *pdb = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0][macro_cluster_assoc_idx_1].pdb;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0][macro_cluster_assoc_idx_1].pdb,
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_assoc[%d][%d].pdb",
        unit, macro_cluster_assoc_idx_0, macro_cluster_assoc_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__PDB,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_ASSOC_PDB_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_set(int unit, uint32 macro_cluster_assoc_idx_0, uint32 macro_cluster_assoc_idx_1, uint8 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        macro_cluster_assoc_idx_0,
        MDB_NOF_MACRO_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0],
        macro_cluster_assoc_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0][macro_cluster_assoc_idx_1].flags,
        flags,
        uint8,
        0,
        "mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        &flags,
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_assoc[%d][%d].flags",
        unit, macro_cluster_assoc_idx_0, macro_cluster_assoc_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_ASSOC_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_get(int unit, uint32 macro_cluster_assoc_idx_0, uint32 macro_cluster_assoc_idx_1, uint8 *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        macro_cluster_assoc_idx_0,
        MDB_NOF_MACRO_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        flags);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0],
        macro_cluster_assoc_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        DNX_SW_STATE_DIAG_READ);

    *flags = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0][macro_cluster_assoc_idx_1].flags;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_assoc[macro_cluster_assoc_idx_0][macro_cluster_assoc_idx_1].flags,
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_assoc[%d][%d].flags",
        unit, macro_cluster_assoc_idx_0, macro_cluster_assoc_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_ASSOC__FLAGS,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_ASSOC_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_alloc(int unit, uint32 macro_cluster_cache_idx_0, uint32 nof_instances_to_alloc_1, uint32 nof_instances_to_alloc_2)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE,
        nof_instances_to_alloc_1,
        nof_instances_to_alloc_2,
        sizeof(cluster_data*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0],
        cluster_data*,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][MDB_MODULE_ID],
        1,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_alloc");

    for(uint32 macro_cluster_cache_idx_1 = 0;
         macro_cluster_cache_idx_1 < nof_instances_to_alloc_1;
         macro_cluster_cache_idx_1++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1],
        cluster_data,
        nof_instances_to_alloc_2,
        sw_state_layout_array[unit][MDB_MODULE_ID],
        2,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0],
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_cache[%d]",
        unit, macro_cluster_cache_idx_0,
        nof_instances_to_alloc_2 * sizeof(cluster_data) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0])+(nof_instances_to_alloc_2 * sizeof(cluster_data)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_CACHE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_set(int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 macro_cluster_cache_idx_2, uint32 data_idx_0, uint32 data_idx_1, uint32 data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        macro_cluster_cache_idx_0,
        MDB_NOF_MACRO_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0],
        macro_cluster_cache_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1],
        macro_cluster_cache_idx_2);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data,
        data_idx_0);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        data_idx_1,
        DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_UINT32);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data[data_idx_0][data_idx_1],
        data,
        uint32,
        0,
        "mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        &data,
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_cache[%d][%d][%d].data[%d][%d]",
        unit, macro_cluster_cache_idx_0, macro_cluster_cache_idx_1, macro_cluster_cache_idx_2, data_idx_0, data_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_CACHE_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_get(int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 macro_cluster_cache_idx_2, uint32 data_idx_0, uint32 data_idx_1, uint32 *data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        macro_cluster_cache_idx_0,
        MDB_NOF_MACRO_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0],
        macro_cluster_cache_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1],
        macro_cluster_cache_idx_2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data,
        data_idx_0);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        data_idx_1,
        DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_UINT32);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        DNX_SW_STATE_DIAG_READ);

    *data = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data[data_idx_0][data_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data[data_idx_0][data_idx_1],
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_cache[%d][%d][%d].data[%d][%d]",
        unit, macro_cluster_cache_idx_0, macro_cluster_cache_idx_1, macro_cluster_cache_idx_2, data_idx_0, data_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_CACHE_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_read(int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 macro_cluster_cache_idx_2, uint32 data_idx_0, uint32 from_idx, uint32 nof_elements, uint32 *dest)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        SW_STATE_FUNC_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        DNX_SW_STATE_DIAG_READ);

    DNX_SW_STATE_RANGE_READ(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        dest,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data[data_idx_0],
        uint32,
        from_idx,
        nof_elements,
        0,
        "mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_read");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEREAD,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data[data_idx_0],
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_cache[%d][%d][%d].data[%d]",
        unit, macro_cluster_cache_idx_0, macro_cluster_cache_idx_1, macro_cluster_cache_idx_2, data_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_CACHE_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_write(int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 macro_cluster_cache_idx_2, uint32 data_idx_0, uint32 from_idx, uint32 nof_elements, const uint32 *source)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        SW_STATE_FUNC_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_COPY(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data[data_idx_0],
        source,
        uint32,
        from_idx,
        nof_elements,
        0,
        "mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_write");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEWRITE,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data[data_idx_0],
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_cache[%d][%d][%d].data[%d]",
        unit, macro_cluster_cache_idx_0, macro_cluster_cache_idx_1, macro_cluster_cache_idx_2, data_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_CACHE_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_fill(int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 macro_cluster_cache_idx_2, uint32 data_idx_0, uint32 from_idx, uint32 nof_elements, uint32 value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        SW_STATE_FUNC_RANGE_FILL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_RANGE_FILL(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data[data_idx_0],
        uint32,
        from_idx,
        nof_elements,
        value,
        0,
        "mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_fill");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_RANGE_FILL_LOGGING,
        BSL_LS_SWSTATEDNX_RANGEFILL,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data[data_idx_0],
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_cache[%d][%d][%d].data[%d]",
        unit, macro_cluster_cache_idx_0, macro_cluster_cache_idx_1, macro_cluster_cache_idx_2, data_idx_0, nof_elements);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_CACHE_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_alloc(int unit, uint32 macro_cluster_cache_idx_0, uint32 macro_cluster_cache_idx_1, uint32 macro_cluster_cache_idx_2, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        macro_cluster_cache_idx_0,
        MDB_NOF_MACRO_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0],
        macro_cluster_cache_idx_1);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1],
        macro_cluster_cache_idx_2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data,
        uint32[DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_UINT32],
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][MDB_MODULE_ID],
        0,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data,
        "mdb_db_infos[%d]->mdb_cluster_infos.macro_cluster_cache[%d][%d][%d].data",
        unit, macro_cluster_cache_idx_0, macro_cluster_cache_idx_1, macro_cluster_cache_idx_2,
        nof_instances_to_alloc_0 * sizeof(uint32[DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_UINT32]) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.macro_cluster_cache[macro_cluster_cache_idx_0][macro_cluster_cache_idx_1][macro_cluster_cache_idx_2].data));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__MACRO_CLUSTER_CACHE__DATA,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_MACRO_CLUSTER_CACHE_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_set(int unit, uint32 eedb_banks_hitbit_idx_0, uint32 eedb_banks_hitbit_idx_1, uint32 eedb_banks_hitbit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        eedb_banks_hitbit_idx_0,
        DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[eedb_banks_hitbit_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[eedb_banks_hitbit_idx_0],
        eedb_banks_hitbit_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[eedb_banks_hitbit_idx_0][eedb_banks_hitbit_idx_1],
        eedb_banks_hitbit,
        uint32,
        0,
        "mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        &eedb_banks_hitbit,
        "mdb_db_infos[%d]->mdb_cluster_infos.eedb_banks_hitbit[%d][%d]",
        unit, eedb_banks_hitbit_idx_0, eedb_banks_hitbit_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_EEDB_BANKS_HITBIT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_get(int unit, uint32 eedb_banks_hitbit_idx_0, uint32 eedb_banks_hitbit_idx_1, uint32 *eedb_banks_hitbit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        eedb_banks_hitbit_idx_0,
        DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        eedb_banks_hitbit);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[eedb_banks_hitbit_idx_0],
        eedb_banks_hitbit_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[eedb_banks_hitbit_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        DNX_SW_STATE_DIAG_READ);

    *eedb_banks_hitbit = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[eedb_banks_hitbit_idx_0][eedb_banks_hitbit_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[eedb_banks_hitbit_idx_0][eedb_banks_hitbit_idx_1],
        "mdb_db_infos[%d]->mdb_cluster_infos.eedb_banks_hitbit[%d][%d]",
        unit, eedb_banks_hitbit_idx_0, eedb_banks_hitbit_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_EEDB_BANKS_HITBIT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_alloc(int unit, uint32 eedb_banks_hitbit_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        eedb_banks_hitbit_idx_0,
        DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[eedb_banks_hitbit_idx_0],
        uint32,
        dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows/dnx_data_mdb.dh.nof_data_rows_per_hitbit_row_get(unit),
        sw_state_layout_array[unit][MDB_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[eedb_banks_hitbit_idx_0],
        "mdb_db_infos[%d]->mdb_cluster_infos.eedb_banks_hitbit[%d]",
        unit, eedb_banks_hitbit_idx_0,
        dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows/dnx_data_mdb.dh.nof_data_rows_per_hitbit_row_get(unit) * sizeof(uint32) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_banks_hitbit[eedb_banks_hitbit_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_BANKS_HITBIT,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_EEDB_BANKS_HITBIT_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_set(int unit, uint32 small_kaps_bb_hitbit_idx_0, uint32 small_kaps_bb_hitbit_idx_1, uint32 small_kaps_bb_hitbit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit,
        small_kaps_bb_hitbit_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[small_kaps_bb_hitbit_idx_0],
        small_kaps_bb_hitbit_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[small_kaps_bb_hitbit_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[small_kaps_bb_hitbit_idx_0][small_kaps_bb_hitbit_idx_1],
        small_kaps_bb_hitbit,
        uint32,
        0,
        "mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        &small_kaps_bb_hitbit,
        "mdb_db_infos[%d]->mdb_cluster_infos.small_kaps_bb_hitbit[%d][%d]",
        unit, small_kaps_bb_hitbit_idx_0, small_kaps_bb_hitbit_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_SMALL_KAPS_BB_HITBIT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_get(int unit, uint32 small_kaps_bb_hitbit_idx_0, uint32 small_kaps_bb_hitbit_idx_1, uint32 *small_kaps_bb_hitbit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit,
        small_kaps_bb_hitbit_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        small_kaps_bb_hitbit);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[small_kaps_bb_hitbit_idx_0],
        small_kaps_bb_hitbit_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[small_kaps_bb_hitbit_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        DNX_SW_STATE_DIAG_READ);

    *small_kaps_bb_hitbit = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[small_kaps_bb_hitbit_idx_0][small_kaps_bb_hitbit_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[small_kaps_bb_hitbit_idx_0][small_kaps_bb_hitbit_idx_1],
        "mdb_db_infos[%d]->mdb_cluster_infos.small_kaps_bb_hitbit[%d][%d]",
        unit, small_kaps_bb_hitbit_idx_0, small_kaps_bb_hitbit_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_SMALL_KAPS_BB_HITBIT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        dnx_data_mdb.kaps.nof_small_bbs_get(unit)*dnx_data_mdb.kaps.nof_dbs_get(unit)/MDB_LPM_DBS_IN_PAIR,
        dnx_data_mdb.kaps.nof_rows_in_small_bb_max_get(unit),
        sizeof(uint32*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit,
        uint32*,
        dnx_data_mdb.kaps.nof_small_bbs_get(unit)*dnx_data_mdb.kaps.nof_dbs_get(unit)/MDB_LPM_DBS_IN_PAIR,
        sw_state_layout_array[unit][MDB_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_alloc");

    for(uint32 small_kaps_bb_hitbit_idx_0 = 0;
         small_kaps_bb_hitbit_idx_0 < dnx_data_mdb.kaps.nof_small_bbs_get(unit)*dnx_data_mdb.kaps.nof_dbs_get(unit)/MDB_LPM_DBS_IN_PAIR;
         small_kaps_bb_hitbit_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit[small_kaps_bb_hitbit_idx_0],
        uint32,
        dnx_data_mdb.kaps.nof_rows_in_small_bb_max_get(unit),
        sw_state_layout_array[unit][MDB_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit,
        "mdb_db_infos[%d]->mdb_cluster_infos.small_kaps_bb_hitbit",
        unit,
        dnx_data_mdb.kaps.nof_rows_in_small_bb_max_get(unit) * sizeof(uint32) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_bb_hitbit)+(dnx_data_mdb.kaps.nof_rows_in_small_bb_max_get(unit) * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_BB_HITBIT,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_SMALL_KAPS_BB_HITBIT_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_set(int unit, uint32 small_kaps_rpb_hitbit_idx_0, uint32 small_kaps_rpb_hitbit_idx_1, uint32 small_kaps_rpb_hitbit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit,
        small_kaps_rpb_hitbit_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[small_kaps_rpb_hitbit_idx_0],
        small_kaps_rpb_hitbit_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[small_kaps_rpb_hitbit_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[small_kaps_rpb_hitbit_idx_0][small_kaps_rpb_hitbit_idx_1],
        small_kaps_rpb_hitbit,
        uint32,
        0,
        "mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        &small_kaps_rpb_hitbit,
        "mdb_db_infos[%d]->mdb_cluster_infos.small_kaps_rpb_hitbit[%d][%d]",
        unit, small_kaps_rpb_hitbit_idx_0, small_kaps_rpb_hitbit_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_SMALL_KAPS_RPB_HITBIT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_get(int unit, uint32 small_kaps_rpb_hitbit_idx_0, uint32 small_kaps_rpb_hitbit_idx_1, uint32 *small_kaps_rpb_hitbit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit,
        small_kaps_rpb_hitbit_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        small_kaps_rpb_hitbit);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[small_kaps_rpb_hitbit_idx_0],
        small_kaps_rpb_hitbit_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[small_kaps_rpb_hitbit_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        DNX_SW_STATE_DIAG_READ);

    *small_kaps_rpb_hitbit = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[small_kaps_rpb_hitbit_idx_0][small_kaps_rpb_hitbit_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[small_kaps_rpb_hitbit_idx_0][small_kaps_rpb_hitbit_idx_1],
        "mdb_db_infos[%d]->mdb_cluster_infos.small_kaps_rpb_hitbit[%d][%d]",
        unit, small_kaps_rpb_hitbit_idx_0, small_kaps_rpb_hitbit_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_SMALL_KAPS_RPB_HITBIT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        dnx_data_mdb.kaps.nof_dbs_get(unit),
        dnx_data_mdb.kaps.nof_rows_in_small_rpb_hitbits_max_get(unit),
        sizeof(uint32*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit,
        uint32*,
        dnx_data_mdb.kaps.nof_dbs_get(unit),
        sw_state_layout_array[unit][MDB_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_alloc");

    for(uint32 small_kaps_rpb_hitbit_idx_0 = 0;
         small_kaps_rpb_hitbit_idx_0 < dnx_data_mdb.kaps.nof_dbs_get(unit);
         small_kaps_rpb_hitbit_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit[small_kaps_rpb_hitbit_idx_0],
        uint32,
        dnx_data_mdb.kaps.nof_rows_in_small_rpb_hitbits_max_get(unit),
        sw_state_layout_array[unit][MDB_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit,
        "mdb_db_infos[%d]->mdb_cluster_infos.small_kaps_rpb_hitbit",
        unit,
        dnx_data_mdb.kaps.nof_rows_in_small_rpb_hitbits_max_get(unit) * sizeof(uint32) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.small_kaps_rpb_hitbit)+(dnx_data_mdb.kaps.nof_rows_in_small_rpb_hitbits_max_get(unit) * sizeof(uint32)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__SMALL_KAPS_RPB_HITBIT,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_SMALL_KAPS_RPB_HITBIT_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_direct_address_mapping_set(int unit, uint32 direct_address_mapping_idx_0, uint32 direct_address_mapping_idx_1, uint32 direct_address_mapping)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        direct_address_mapping_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[direct_address_mapping_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[direct_address_mapping_idx_0],
        direct_address_mapping_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[direct_address_mapping_idx_0][direct_address_mapping_idx_1],
        direct_address_mapping,
        uint32,
        0,
        "mdb_db_infos_mdb_cluster_infos_direct_address_mapping_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        &direct_address_mapping,
        "mdb_db_infos[%d]->mdb_cluster_infos.direct_address_mapping[%d][%d]",
        unit, direct_address_mapping_idx_0, direct_address_mapping_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_DIRECT_ADDRESS_MAPPING_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_direct_address_mapping_get(int unit, uint32 direct_address_mapping_idx_0, uint32 direct_address_mapping_idx_1, uint32 *direct_address_mapping)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        direct_address_mapping_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        direct_address_mapping);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[direct_address_mapping_idx_0],
        direct_address_mapping_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[direct_address_mapping_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        DNX_SW_STATE_DIAG_READ);

    *direct_address_mapping = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[direct_address_mapping_idx_0][direct_address_mapping_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[direct_address_mapping_idx_0][direct_address_mapping_idx_1],
        "mdb_db_infos[%d]->mdb_cluster_infos.direct_address_mapping[%d][%d]",
        unit, direct_address_mapping_idx_0, direct_address_mapping_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_DIRECT_ADDRESS_MAPPING_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_direct_address_mapping_alloc(int unit, uint32 direct_address_mapping_idx_0, uint32 nof_instances_to_alloc_1)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        direct_address_mapping_idx_0,
        MDB_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[direct_address_mapping_idx_0],
        uint32,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][MDB_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_mdb_cluster_infos_direct_address_mapping_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[direct_address_mapping_idx_0],
        "mdb_db_infos[%d]->mdb_cluster_infos.direct_address_mapping[%d]",
        unit, direct_address_mapping_idx_0,
        nof_instances_to_alloc_1 * sizeof(uint32) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[direct_address_mapping_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__DIRECT_ADDRESS_MAPPING,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_DIRECT_ADDRESS_MAPPING_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_1)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.direct_address_mapping[direct_address_mapping_idx_0][def_val_idx[0]],
        MDB_DIRECT_INVALID_START_ADDRESS);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_set(int unit, uint32 eedb_address_mapping_idx_0, uint32 eedb_address_mapping)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping,
        eedb_address_mapping_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping[eedb_address_mapping_idx_0],
        eedb_address_mapping,
        uint32,
        0,
        "mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        &eedb_address_mapping,
        "mdb_db_infos[%d]->mdb_cluster_infos.eedb_address_mapping[%d]",
        unit, eedb_address_mapping_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_EEDB_ADDRESS_MAPPING_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_get(int unit, uint32 eedb_address_mapping_idx_0, uint32 *eedb_address_mapping)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping,
        eedb_address_mapping_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        eedb_address_mapping);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        DNX_SW_STATE_DIAG_READ);

    *eedb_address_mapping = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping[eedb_address_mapping_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping[eedb_address_mapping_idx_0],
        "mdb_db_infos[%d]->mdb_cluster_infos.eedb_address_mapping[%d]",
        unit, eedb_address_mapping_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_EEDB_ADDRESS_MAPPING_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][MDB_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping,
        "mdb_db_infos[%d]->mdb_cluster_infos.eedb_address_mapping",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_CLUSTER_INFOS__EEDB_ADDRESS_MAPPING,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_CLUSTER_INFOS_EEDB_ADDRESS_MAPPING_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], nof_instances_to_alloc_0)

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_cluster_infos.eedb_address_mapping[def_val_idx[0]],
        MDB_DIRECT_INVALID_START_ADDRESS);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_max_fec_id_value_set(int unit, uint32 max_fec_id_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->max_fec_id_value,
        max_fec_id_value,
        uint32,
        0,
        "mdb_db_infos_max_fec_id_value_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        &max_fec_id_value,
        "mdb_db_infos[%d]->max_fec_id_value",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        mdb_db_infos_info,
        MDB_DB_INFOS_MAX_FEC_ID_VALUE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_max_fec_id_value_get(int unit, uint32 *max_fec_id_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        max_fec_id_value);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        DNX_SW_STATE_DIAG_READ);

    *max_fec_id_value = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->max_fec_id_value;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->max_fec_id_value,
        "mdb_db_infos[%d]->max_fec_id_value",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MAX_FEC_ID_VALUE,
        mdb_db_infos_info,
        MDB_DB_INFOS_MAX_FEC_ID_VALUE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_eedb_bank_traffic_lock_local_lifs_set(int unit, uint32 local_lifs_idx_0, uint32 local_lifs_idx_1, int local_lifs)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        local_lifs_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[local_lifs_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[local_lifs_idx_0],
        local_lifs_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[local_lifs_idx_0][local_lifs_idx_1],
        local_lifs,
        int,
        0,
        "mdb_db_infos_eedb_bank_traffic_lock_local_lifs_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        &local_lifs,
        "mdb_db_infos[%d]->eedb_bank_traffic_lock.local_lifs[%d][%d]",
        unit, local_lifs_idx_0, local_lifs_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        mdb_db_infos_info,
        MDB_DB_INFOS_EEDB_BANK_TRAFFIC_LOCK_LOCAL_LIFS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_eedb_bank_traffic_lock_local_lifs_get(int unit, uint32 local_lifs_idx_0, uint32 local_lifs_idx_1, int *local_lifs)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        local_lifs_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        local_lifs);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[local_lifs_idx_0],
        local_lifs_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[local_lifs_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        DNX_SW_STATE_DIAG_READ);

    *local_lifs = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[local_lifs_idx_0][local_lifs_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[local_lifs_idx_0][local_lifs_idx_1],
        "mdb_db_infos[%d]->eedb_bank_traffic_lock.local_lifs[%d][%d]",
        unit, local_lifs_idx_0, local_lifs_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        mdb_db_infos_info,
        MDB_DB_INFOS_EEDB_BANK_TRAFFIC_LOCK_LOCAL_LIFS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_eedb_bank_traffic_lock_local_lifs_alloc(int unit, uint32 local_lifs_idx_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        local_lifs_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[local_lifs_idx_0],
        int,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][MDB_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_eedb_bank_traffic_lock_local_lifs_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[local_lifs_idx_0],
        "mdb_db_infos[%d]->eedb_bank_traffic_lock.local_lifs[%d]",
        unit, local_lifs_idx_0,
        nof_instances_to_alloc_1 * sizeof(int) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.local_lifs[local_lifs_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__LOCAL_LIFS,
        mdb_db_infos_info,
        MDB_DB_INFOS_EEDB_BANK_TRAFFIC_LOCK_LOCAL_LIFS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_set(int unit, uint32 global_lif_destination_idx_0, int global_lif_destination)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        global_lif_destination_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.global_lif_destination[global_lif_destination_idx_0],
        global_lif_destination,
        int,
        0,
        "mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        &global_lif_destination,
        "mdb_db_infos[%d]->eedb_bank_traffic_lock.global_lif_destination[%d]",
        unit, global_lif_destination_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        mdb_db_infos_info,
        MDB_DB_INFOS_EEDB_BANK_TRAFFIC_LOCK_GLOBAL_LIF_DESTINATION_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_get(int unit, uint32 global_lif_destination_idx_0, int *global_lif_destination)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        global_lif_destination_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        global_lif_destination);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        DNX_SW_STATE_DIAG_READ);

    *global_lif_destination = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.global_lif_destination[global_lif_destination_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->eedb_bank_traffic_lock.global_lif_destination[global_lif_destination_idx_0],
        "mdb_db_infos[%d]->eedb_bank_traffic_lock.global_lif_destination[%d]",
        unit, global_lif_destination_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__EEDB_BANK_TRAFFIC_LOCK__GLOBAL_LIF_DESTINATION,
        mdb_db_infos_info,
        MDB_DB_INFOS_EEDB_BANK_TRAFFIC_LOCK_GLOBAL_LIF_DESTINATION_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_fec_db_info_fec_db_map_set(int unit, uint32 bank_id, uint32 fec_db_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_db_map,
        bank_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_db_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_db_map[bank_id],
        fec_db_map,
        uint32,
        0,
        "mdb_db_infos_fec_db_info_fec_db_map_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        &fec_db_map,
        "mdb_db_infos[%d]->fec_db_info.fec_db_map[%d]",
        unit, bank_id);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        mdb_db_infos_info,
        MDB_DB_INFOS_FEC_DB_INFO_FEC_DB_MAP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_fec_db_info_fec_db_map_get(int unit, uint32 bank_id, uint32 *fec_db_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_db_map,
        bank_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        fec_db_map);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_db_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        DNX_SW_STATE_DIAG_READ);

    *fec_db_map = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_db_map[bank_id];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_db_map[bank_id],
        "mdb_db_infos[%d]->fec_db_info.fec_db_map[%d]",
        unit, bank_id);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        mdb_db_infos_info,
        MDB_DB_INFOS_FEC_DB_INFO_FEC_DB_MAP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_fec_db_info_fec_db_map_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_db_map,
        uint32,
        dnx_data_l3.fec.max_nof_banks_get(unit),
        sw_state_layout_array[unit][MDB_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_fec_db_info_fec_db_map_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_db_map,
        "mdb_db_infos[%d]->fec_db_info.fec_db_map",
        unit,
        dnx_data_l3.fec.max_nof_banks_get(unit) * sizeof(uint32) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_db_map));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_DB_MAP,
        mdb_db_infos_info,
        MDB_DB_INFOS_FEC_DB_INFO_FEC_DB_MAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_fec_db_info_fec_id_range_start_set(int unit, uint32 index, uint32 fec_id_range_start)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_start,
        index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_start);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_start[index],
        fec_id_range_start,
        uint32,
        0,
        "mdb_db_infos_fec_db_info_fec_id_range_start_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        &fec_id_range_start,
        "mdb_db_infos[%d]->fec_db_info.fec_id_range_start[%d]",
        unit, index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        mdb_db_infos_info,
        MDB_DB_INFOS_FEC_DB_INFO_FEC_ID_RANGE_START_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_fec_db_info_fec_id_range_start_get(int unit, uint32 index, uint32 *fec_id_range_start)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_start,
        index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        fec_id_range_start);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_start);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        DNX_SW_STATE_DIAG_READ);

    *fec_id_range_start = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_start[index];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_start[index],
        "mdb_db_infos[%d]->fec_db_info.fec_id_range_start[%d]",
        unit, index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        mdb_db_infos_info,
        MDB_DB_INFOS_FEC_DB_INFO_FEC_ID_RANGE_START_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_fec_db_info_fec_id_range_start_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_start,
        uint32,
        dnx_data_mdb.direct.nof_fec_dbs_get(unit),
        sw_state_layout_array[unit][MDB_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_fec_db_info_fec_id_range_start_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_start,
        "mdb_db_infos[%d]->fec_db_info.fec_id_range_start",
        unit,
        dnx_data_mdb.direct.nof_fec_dbs_get(unit) * sizeof(uint32) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_start));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_START,
        mdb_db_infos_info,
        MDB_DB_INFOS_FEC_DB_INFO_FEC_ID_RANGE_START_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_fec_db_info_fec_id_range_size_set(int unit, uint32 index, uint32 fec_id_range_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_size,
        index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_size[index],
        fec_id_range_size,
        uint32,
        0,
        "mdb_db_infos_fec_db_info_fec_id_range_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        &fec_id_range_size,
        "mdb_db_infos[%d]->fec_db_info.fec_id_range_size[%d]",
        unit, index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        mdb_db_infos_info,
        MDB_DB_INFOS_FEC_DB_INFO_FEC_ID_RANGE_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_fec_db_info_fec_id_range_size_get(int unit, uint32 index, uint32 *fec_id_range_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_size,
        index);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        fec_id_range_size);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *fec_id_range_size = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_size[index];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_size[index],
        "mdb_db_infos[%d]->fec_db_info.fec_id_range_size[%d]",
        unit, index);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        mdb_db_infos_info,
        MDB_DB_INFOS_FEC_DB_INFO_FEC_ID_RANGE_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_fec_db_info_fec_id_range_size_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_size,
        uint32,
        dnx_data_mdb.direct.nof_fec_dbs_get(unit),
        sw_state_layout_array[unit][MDB_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "mdb_db_infos_fec_db_info_fec_id_range_size_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_size,
        "mdb_db_infos[%d]->fec_db_info.fec_id_range_size",
        unit,
        dnx_data_mdb.direct.nof_fec_dbs_get(unit) * sizeof(uint32) / sizeof(*((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->fec_db_info.fec_id_range_size));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__FEC_DB_INFO__FEC_ID_RANGE_SIZE,
        mdb_db_infos_info,
        MDB_DB_INFOS_FEC_DB_INFO_FEC_ID_RANGE_SIZE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_profile_set(int unit, uint8 mdb_profile)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_profile,
        mdb_profile,
        uint8,
        0,
        "mdb_db_infos_mdb_profile_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        &mdb_profile,
        "mdb_db_infos[%d]->mdb_profile",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_PROFILE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_profile_get(int unit, uint8 *mdb_profile)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        mdb_profile);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        DNX_SW_STATE_DIAG_READ);

    *mdb_profile = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_profile;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_profile,
        "mdb_db_infos[%d]->mdb_profile",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_PROFILE,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_PROFILE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_em_init_done_set(int unit, uint8 mdb_em_init_done)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_em_init_done,
        mdb_em_init_done,
        uint8,
        0,
        "mdb_db_infos_mdb_em_init_done_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        &mdb_em_init_done,
        "mdb_db_infos[%d]->mdb_em_init_done",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_EM_INIT_DONE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_mdb_em_init_done_get(int unit, uint8 *mdb_em_init_done)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        mdb_em_init_done);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        DNX_SW_STATE_DIAG_READ);

    *mdb_em_init_done = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_em_init_done;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->mdb_em_init_done,
        "mdb_db_infos[%d]->mdb_em_init_done",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__MDB_EM_INIT_DONE,
        mdb_db_infos_info,
        MDB_DB_INFOS_MDB_EM_INIT_DONE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_kaps_scan_cfg_on_set(int unit, uint32 kaps_scan_cfg_on_idx_0, uint32 kaps_scan_cfg_on)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kaps_scan_cfg_on_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_on[kaps_scan_cfg_on_idx_0],
        kaps_scan_cfg_on,
        uint32,
        0,
        "mdb_db_infos_kaps_scan_cfg_on_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        &kaps_scan_cfg_on,
        "mdb_db_infos[%d]->kaps_scan_cfg_on[%d]",
        unit, kaps_scan_cfg_on_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        mdb_db_infos_info,
        MDB_DB_INFOS_KAPS_SCAN_CFG_ON_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_kaps_scan_cfg_on_get(int unit, uint32 kaps_scan_cfg_on_idx_0, uint32 *kaps_scan_cfg_on)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kaps_scan_cfg_on_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        kaps_scan_cfg_on);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        DNX_SW_STATE_DIAG_READ);

    *kaps_scan_cfg_on = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_on[kaps_scan_cfg_on_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_on[kaps_scan_cfg_on_idx_0],
        "mdb_db_infos[%d]->kaps_scan_cfg_on[%d]",
        unit, kaps_scan_cfg_on_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_ON,
        mdb_db_infos_info,
        MDB_DB_INFOS_KAPS_SCAN_CFG_ON_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_kaps_scan_cfg_paused_set(int unit, uint32 kaps_scan_cfg_paused_idx_0, uint32 kaps_scan_cfg_paused)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kaps_scan_cfg_paused_idx_0,
        2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_paused[kaps_scan_cfg_paused_idx_0],
        kaps_scan_cfg_paused,
        uint32,
        0,
        "mdb_db_infos_kaps_scan_cfg_paused_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        &kaps_scan_cfg_paused,
        "mdb_db_infos[%d]->kaps_scan_cfg_paused[%d]",
        unit, kaps_scan_cfg_paused_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        mdb_db_infos_info,
        MDB_DB_INFOS_KAPS_SCAN_CFG_PAUSED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
mdb_db_infos_kaps_scan_cfg_paused_get(int unit, uint32 kaps_scan_cfg_paused_idx_0, uint32 *kaps_scan_cfg_paused)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        sw_state_roots_array[unit][MDB_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        kaps_scan_cfg_paused_idx_0,
        2);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        kaps_scan_cfg_paused);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        DNX_SW_STATE_DIAG_READ);

    *kaps_scan_cfg_paused = ((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_paused[kaps_scan_cfg_paused_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        &((mdb_tables_info*)sw_state_roots_array[unit][MDB_MODULE_ID])->kaps_scan_cfg_paused[kaps_scan_cfg_paused_idx_0],
        "mdb_db_infos[%d]->kaps_scan_cfg_paused[%d]",
        unit, kaps_scan_cfg_paused_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_MDB_DB_INFOS__KAPS_SCAN_CFG_PAUSED,
        mdb_db_infos_info,
        MDB_DB_INFOS_KAPS_SCAN_CFG_PAUSED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][MDB_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




mdb_db_infos_cbs mdb_db_infos = 	{
	
	mdb_db_infos_is_init,
	mdb_db_infos_init,
		{
		
		mdb_db_infos_em_key_format_set,
		mdb_db_infos_em_key_format_get,
			{
			
			mdb_db_infos_em_key_format_key_size_set,
			mdb_db_infos_em_key_format_key_size_get}
		,
			{
			
				{
				
				mdb_db_infos_em_key_format_em_bank_info_is_flex_set,
				mdb_db_infos_em_key_format_em_bank_info_is_flex_get}
			,
				{
				
				mdb_db_infos_em_key_format_em_bank_info_macro_index_set,
				mdb_db_infos_em_key_format_em_bank_info_macro_index_get}
			,
				{
				
				mdb_db_infos_em_key_format_em_bank_info_bank_size_used_nof_rows_set,
				mdb_db_infos_em_key_format_em_bank_info_bank_size_used_nof_rows_get}
			,
				{
				
				mdb_db_infos_em_key_format_em_bank_info_macro_type_set,
				mdb_db_infos_em_key_format_em_bank_info_macro_type_get}
			,
				{
				
				mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_set,
				mdb_db_infos_em_key_format_em_bank_info_cluster_mapping_get}
			}
		,
			{
			
			mdb_db_infos_em_key_format_nof_address_bits_set,
			mdb_db_infos_em_key_format_nof_address_bits_get}
		,
			{
			
			mdb_db_infos_em_key_format_nof_ways_set,
			mdb_db_infos_em_key_format_nof_ways_get}
		,
			{
			
			mdb_db_infos_em_key_format_nof_flex_ways_set,
			mdb_db_infos_em_key_format_nof_flex_ways_get}
		}
	,
		{
		
			{
			
			mdb_db_infos_db_number_of_alloc_clusters_set,
			mdb_db_infos_db_number_of_alloc_clusters_get}
		,
			{
			
			mdb_db_infos_db_number_of_clusters_set,
			mdb_db_infos_db_number_of_clusters_get}
		,
			{
			
			mdb_db_infos_db_nof_cluster_per_entry_set,
			mdb_db_infos_db_nof_cluster_per_entry_get}
		,
			{
			
			mdb_db_infos_db_clusters_info_set,
			mdb_db_infos_db_clusters_info_get,
			mdb_db_infos_db_clusters_info_alloc,
				{
				
				mdb_db_infos_db_clusters_info_macro_type_set,
				mdb_db_infos_db_clusters_info_macro_type_get}
			,
				{
				
				mdb_db_infos_db_clusters_info_macro_index_set,
				mdb_db_infos_db_clusters_info_macro_index_get}
			,
				{
				
				mdb_db_infos_db_clusters_info_cluster_index_set,
				mdb_db_infos_db_clusters_info_cluster_index_get}
			,
				{
				
				mdb_db_infos_db_clusters_info_cluster_position_in_entry_set,
				mdb_db_infos_db_clusters_info_cluster_position_in_entry_get}
			,
				{
				
				mdb_db_infos_db_clusters_info_start_address_set,
				mdb_db_infos_db_clusters_info_start_address_get}
			,
				{
				
				mdb_db_infos_db_clusters_info_end_address_set,
				mdb_db_infos_db_clusters_info_end_address_get}
			}
		,
			{
			
			mdb_db_infos_db_address_space_bits_set,
			mdb_db_infos_db_address_space_bits_get}
		}
	,
		{
		
		mdb_db_infos_capacity_set,
		mdb_db_infos_capacity_get}
	,
		{
		
		mdb_db_infos_em_entry_capacity_estimate_set,
		mdb_db_infos_em_entry_capacity_estimate_get}
	,
		{
		
		mdb_db_infos_em_entry_capacity_estimate_average_set,
		mdb_db_infos_em_entry_capacity_estimate_average_get}
	,
		{
		
		mdb_db_infos_eedb_payload_type_set,
		mdb_db_infos_eedb_payload_type_get}
	,
		{
		
			{
			
			mdb_db_infos_mdb_cluster_infos_cache_enabled_set,
			mdb_db_infos_mdb_cluster_infos_cache_enabled_get}
		,
			{
			
			mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_alloc,
				{
				
				mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_set,
				mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_pdb_get}
			,
				{
				
				mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_set,
				mdb_db_infos_mdb_cluster_infos_macro_cluster_assoc_flags_get}
			}
		,
			{
			
			mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_alloc,
				{
				
				mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_set,
				mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_get,
				mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_read,
				mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_write,
				mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_range_fill,
				mdb_db_infos_mdb_cluster_infos_macro_cluster_cache_data_alloc}
			}
		,
			{
			
			mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_set,
			mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_get,
			mdb_db_infos_mdb_cluster_infos_eedb_banks_hitbit_alloc}
		,
			{
			
			mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_set,
			mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_get,
			mdb_db_infos_mdb_cluster_infos_small_kaps_bb_hitbit_alloc}
		,
			{
			
			mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_set,
			mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_get,
			mdb_db_infos_mdb_cluster_infos_small_kaps_rpb_hitbit_alloc}
		,
			{
			
			mdb_db_infos_mdb_cluster_infos_direct_address_mapping_set,
			mdb_db_infos_mdb_cluster_infos_direct_address_mapping_get,
			mdb_db_infos_mdb_cluster_infos_direct_address_mapping_alloc}
		,
			{
			
			mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_set,
			mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_get,
			mdb_db_infos_mdb_cluster_infos_eedb_address_mapping_alloc}
		}
	,
		{
		
		mdb_db_infos_max_fec_id_value_set,
		mdb_db_infos_max_fec_id_value_get}
	,
		{
		
			{
			
			mdb_db_infos_eedb_bank_traffic_lock_local_lifs_set,
			mdb_db_infos_eedb_bank_traffic_lock_local_lifs_get,
			mdb_db_infos_eedb_bank_traffic_lock_local_lifs_alloc}
		,
			{
			
			mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_set,
			mdb_db_infos_eedb_bank_traffic_lock_global_lif_destination_get}
		}
	,
		{
		
			{
			
			mdb_db_infos_fec_db_info_fec_db_map_set,
			mdb_db_infos_fec_db_info_fec_db_map_get,
			mdb_db_infos_fec_db_info_fec_db_map_alloc}
		,
			{
			
			mdb_db_infos_fec_db_info_fec_id_range_start_set,
			mdb_db_infos_fec_db_info_fec_id_range_start_get,
			mdb_db_infos_fec_db_info_fec_id_range_start_alloc}
		,
			{
			
			mdb_db_infos_fec_db_info_fec_id_range_size_set,
			mdb_db_infos_fec_db_info_fec_id_range_size_get,
			mdb_db_infos_fec_db_info_fec_id_range_size_alloc}
		}
	,
		{
		
		mdb_db_infos_mdb_profile_set,
		mdb_db_infos_mdb_profile_get}
	,
		{
		
		mdb_db_infos_mdb_em_init_done_set,
		mdb_db_infos_mdb_em_init_done_get}
	,
		{
		
		mdb_db_infos_kaps_scan_cfg_on_set,
		mdb_db_infos_kaps_scan_cfg_on_get}
	,
		{
		
		mdb_db_infos_kaps_scan_cfg_paused_set,
		mdb_db_infos_kaps_scan_cfg_paused_get}
	}
;
#undef BSL_LOG_MODULE
