
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dbal_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dbal_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dbal_layout.h>





int
dbal_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DBAL_DB,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]),
        "dbal_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DBAL_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_dbal_sw_state_t,
        DNX_SW_STATE_DBAL_DB_NOF_PARAMS,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dbal_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DBAL_DB,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]),
        "dbal_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB,
        dbal_db_info,
        DBAL_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID]),
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_table_type_set(int unit, uint32 sw_tables_idx_0, dbal_sw_state_table_type_e table_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].table_type,
        table_type,
        dbal_sw_state_table_type_e,
        0,
        "dbal_db_sw_tables_table_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        &table_type,
        "dbal_db[%d]->sw_tables[%d].table_type",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        dbal_db_info,
        DBAL_DB_SW_TABLES_TABLE_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_table_type_get(int unit, uint32 sw_tables_idx_0, dbal_sw_state_table_type_e *table_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        table_type);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *table_type = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].table_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].table_type,
        "dbal_db[%d]->sw_tables[%d].table_type",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__TABLE_TYPE,
        dbal_db_info,
        DBAL_DB_SW_TABLES_TABLE_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_alloc_bitmap(int unit, uint32 sw_tables_idx_0, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        _nof_bits_to_alloc,
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_sw_tables_entries_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_set(int unit, uint32 sw_tables_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_clear(int unit, uint32 sw_tables_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_get(int unit, uint32 sw_tables_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_read(int unit, uint32 sw_tables_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_write(int unit, uint32 sw_tables_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_and(int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_or(int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_xor(int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_remove(int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_negate(int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_clear(int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_set(int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_null(int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_test(int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_eq(int unit, uint32 sw_tables_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_entries_bit_range_count(int unit, uint32 sw_tables_idx_0, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        (_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].entries,
        "dbal_db[%d]->sw_tables[%d].entries",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__ENTRIES,
        dbal_db_info,
        DBAL_DB_SW_TABLES_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_hash_table_create(int unit, uint32 sw_tables_idx_0, sw_state_idx_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE);

    SW_STATE_INDEX_HTB_CREATE(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        init_info,
        dbal_sw_state_hash_key,
        uint32,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCREATE,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        "dbal_db[%d]->sw_tables[%d].hash_table",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        dbal_db_info,
        DBAL_DB_SW_TABLES_HASH_TABLE_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_hash_table_find(int unit, uint32 sw_tables_idx_0, const dbal_sw_state_hash_key *key, uint32 *data_index, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_FIND(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        key,
        data_index,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBFIND,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        "dbal_db[%d]->sw_tables[%d].hash_table",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        dbal_db_info,
        DBAL_DB_SW_TABLES_HASH_TABLE_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_hash_table_insert(int unit, uint32 sw_tables_idx_0, const dbal_sw_state_hash_key *key, const uint32 *data_index, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        key,
        data_index,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        "dbal_db[%d]->sw_tables[%d].hash_table",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        dbal_db_info,
        DBAL_DB_SW_TABLES_HASH_TABLE_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_hash_table_get_next(int unit, uint32 sw_tables_idx_0, SW_STATE_INDEX_HASH_TABLE_ITER *iter, const dbal_sw_state_hash_key *key, const uint32 *data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_NEXT(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        iter,
        key,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_NEXT,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        "dbal_db[%d]->sw_tables[%d].hash_table",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        dbal_db_info,
        DBAL_DB_SW_TABLES_HASH_TABLE_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_hash_table_clear(int unit, uint32 sw_tables_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_CLEAR(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCLEAR,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        "dbal_db[%d]->sw_tables[%d].hash_table",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        dbal_db_info,
        DBAL_DB_SW_TABLES_HASH_TABLE_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_hash_table_delete(int unit, uint32 sw_tables_idx_0, const dbal_sw_state_hash_key *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        "dbal_db[%d]->sw_tables[%d].hash_table",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        dbal_db_info,
        DBAL_DB_SW_TABLES_HASH_TABLE_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_hash_table_insert_at_index(int unit, uint32 sw_tables_idx_0, const dbal_sw_state_hash_key *key, uint32 data_idx, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT_AT_INDEX(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        key,
        data_idx,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT_AT_INDEX,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        "dbal_db[%d]->sw_tables[%d].hash_table",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        dbal_db_info,
        DBAL_DB_SW_TABLES_HASH_TABLE_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_hash_table_delete_by_index(int unit, uint32 sw_tables_idx_0, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE_BY_INDEX(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE_BY_INDEX,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        "dbal_db[%d]->sw_tables[%d].hash_table",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        dbal_db_info,
        DBAL_DB_SW_TABLES_HASH_TABLE_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_sw_tables_hash_table_get_by_index(int unit, uint32 sw_tables_idx_0, uint32 data_index, dbal_sw_state_hash_key *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        sw_tables_idx_0,
        DBAL_NOF_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_BY_INDEX(
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        data_index,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_BY_INDEX,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->sw_tables[sw_tables_idx_0].hash_table,
        "dbal_db[%d]->sw_tables[%d].hash_table",
        unit, sw_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__SW_TABLES__HASH_TABLE,
        dbal_db_info,
        DBAL_DB_SW_TABLES_HASH_TABLE_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_mdb_access_skip_read_from_shadow_set(int unit, uint8 skip_read_from_shadow)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_access.skip_read_from_shadow,
        skip_read_from_shadow,
        uint8,
        0,
        "dbal_db_mdb_access_skip_read_from_shadow_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        &skip_read_from_shadow,
        "dbal_db[%d]->mdb_access.skip_read_from_shadow",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        dbal_db_info,
        DBAL_DB_MDB_ACCESS_SKIP_READ_FROM_SHADOW_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_mdb_access_skip_read_from_shadow_get(int unit, uint8 *skip_read_from_shadow)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        skip_read_from_shadow);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        DNX_SW_STATE_DIAG_READ);

    *skip_read_from_shadow = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_access.skip_read_from_shadow;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_access.skip_read_from_shadow,
        "dbal_db[%d]->mdb_access.skip_read_from_shadow",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_ACCESS__SKIP_READ_FROM_SHADOW,
        dbal_db_info,
        DBAL_DB_MDB_ACCESS_SKIP_READ_FROM_SHADOW_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_mdb_phy_tables_nof_entries_set(int unit, uint32 mdb_phy_tables_idx_0, int nof_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mdb_phy_tables_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_phy_tables[mdb_phy_tables_idx_0].nof_entries,
        nof_entries,
        int,
        0,
        "dbal_db_mdb_phy_tables_nof_entries_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        &nof_entries,
        "dbal_db[%d]->mdb_phy_tables[%d].nof_entries",
        unit, mdb_phy_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        dbal_db_info,
        DBAL_DB_MDB_PHY_TABLES_NOF_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_mdb_phy_tables_nof_entries_get(int unit, uint32 mdb_phy_tables_idx_0, int *nof_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mdb_phy_tables_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        nof_entries);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        DNX_SW_STATE_DIAG_READ);

    *nof_entries = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_phy_tables[mdb_phy_tables_idx_0].nof_entries;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_phy_tables[mdb_phy_tables_idx_0].nof_entries,
        "dbal_db[%d]->mdb_phy_tables[%d].nof_entries",
        unit, mdb_phy_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        dbal_db_info,
        DBAL_DB_MDB_PHY_TABLES_NOF_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_mdb_phy_tables_nof_entries_inc(int unit, uint32 mdb_phy_tables_idx_0, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mdb_phy_tables_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_phy_tables[mdb_phy_tables_idx_0].nof_entries,
        inc_value,
        int,
        0,
        "dbal_db_mdb_phy_tables_nof_entries_inc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_INC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_INC,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_phy_tables[mdb_phy_tables_idx_0].nof_entries,
        "dbal_db[%d]->mdb_phy_tables[%d].nof_entries",
        unit, mdb_phy_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        dbal_db_info,
        DBAL_DB_MDB_PHY_TABLES_NOF_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_mdb_phy_tables_nof_entries_dec(int unit, uint32 mdb_phy_tables_idx_0, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        mdb_phy_tables_idx_0,
        DBAL_NOF_PHYSICAL_TABLES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_phy_tables[mdb_phy_tables_idx_0].nof_entries,
        dec_value,
        int,
        0,
        "dbal_db_mdb_phy_tables_nof_entries_dec");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_DEC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_DEC,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->mdb_phy_tables[mdb_phy_tables_idx_0].nof_entries,
        "dbal_db[%d]->mdb_phy_tables[%d].nof_entries",
        unit, mdb_phy_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__MDB_PHY_TABLES__NOF_ENTRIES,
        dbal_db_info,
        DBAL_DB_MDB_PHY_TABLES_NOF_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        dbal_sw_state_table_properties_t,
        dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_tables_properties_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        "dbal_db[%d]->tables_properties",
        unit,
        dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit) * sizeof(dbal_sw_state_table_properties_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_table_status_set(int unit, uint32 tables_properties_idx_0, dbal_table_status_e table_status)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].table_status,
        table_status,
        dbal_table_status_e,
        0,
        "dbal_db_tables_properties_table_status_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        &table_status,
        "dbal_db[%d]->tables_properties[%d].table_status",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_TABLE_STATUS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_table_status_get(int unit, uint32 tables_properties_idx_0, dbal_table_status_e *table_status)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        table_status);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        DNX_SW_STATE_DIAG_READ);

    *table_status = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].table_status;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].table_status,
        "dbal_db[%d]->tables_properties[%d].table_status",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TABLE_STATUS,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_TABLE_STATUS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_set_operations_set(int unit, uint32 tables_properties_idx_0, int nof_set_operations)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_set_operations,
        nof_set_operations,
        int,
        0,
        "dbal_db_tables_properties_nof_set_operations_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        &nof_set_operations,
        "dbal_db[%d]->tables_properties[%d].nof_set_operations",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_SET_OPERATIONS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_set_operations_get(int unit, uint32 tables_properties_idx_0, int *nof_set_operations)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        nof_set_operations);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        DNX_SW_STATE_DIAG_READ);

    *nof_set_operations = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_set_operations;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_set_operations,
        "dbal_db[%d]->tables_properties[%d].nof_set_operations",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_SET_OPERATIONS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_set_operations_inc(int unit, uint32 tables_properties_idx_0, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_set_operations,
        inc_value,
        int,
        0,
        "dbal_db_tables_properties_nof_set_operations_inc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_INC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_INC,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_set_operations,
        "dbal_db[%d]->tables_properties[%d].nof_set_operations",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_SET_OPERATIONS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_set_operations_dec(int unit, uint32 tables_properties_idx_0, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_set_operations,
        dec_value,
        int,
        0,
        "dbal_db_tables_properties_nof_set_operations_dec");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_DEC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_DEC,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_set_operations,
        "dbal_db[%d]->tables_properties[%d].nof_set_operations",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_SET_OPERATIONS,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_SET_OPERATIONS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_get_operations_set(int unit, uint32 tables_properties_idx_0, int nof_get_operations)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_get_operations,
        nof_get_operations,
        int,
        0,
        "dbal_db_tables_properties_nof_get_operations_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        &nof_get_operations,
        "dbal_db[%d]->tables_properties[%d].nof_get_operations",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_GET_OPERATIONS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_get_operations_get(int unit, uint32 tables_properties_idx_0, int *nof_get_operations)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        nof_get_operations);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        DNX_SW_STATE_DIAG_READ);

    *nof_get_operations = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_get_operations;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_get_operations,
        "dbal_db[%d]->tables_properties[%d].nof_get_operations",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_GET_OPERATIONS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_get_operations_inc(int unit, uint32 tables_properties_idx_0, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_get_operations,
        inc_value,
        int,
        0,
        "dbal_db_tables_properties_nof_get_operations_inc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_INC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_INC,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_get_operations,
        "dbal_db[%d]->tables_properties[%d].nof_get_operations",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_GET_OPERATIONS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_get_operations_dec(int unit, uint32 tables_properties_idx_0, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_get_operations,
        dec_value,
        int,
        0,
        "dbal_db_tables_properties_nof_get_operations_dec");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_DEC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_DEC,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_get_operations,
        "dbal_db[%d]->tables_properties[%d].nof_get_operations",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_GET_OPERATIONS,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_GET_OPERATIONS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_entries_set(int unit, uint32 tables_properties_idx_0, int nof_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_entries,
        nof_entries,
        int,
        0,
        "dbal_db_tables_properties_nof_entries_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        &nof_entries,
        "dbal_db[%d]->tables_properties[%d].nof_entries",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_entries_get(int unit, uint32 tables_properties_idx_0, int *nof_entries)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        nof_entries);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        DNX_SW_STATE_DIAG_READ);

    *nof_entries = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_entries;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_entries,
        "dbal_db[%d]->tables_properties[%d].nof_entries",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_entries_inc(int unit, uint32 tables_properties_idx_0, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_entries,
        inc_value,
        int,
        0,
        "dbal_db_tables_properties_nof_entries_inc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_INC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_INC,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_entries,
        "dbal_db[%d]->tables_properties[%d].nof_entries",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_nof_entries_dec(int unit, uint32 tables_properties_idx_0, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_entries,
        dec_value,
        int,
        0,
        "dbal_db_tables_properties_nof_entries_dec");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_DEC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_DEC,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].nof_entries,
        "dbal_db[%d]->tables_properties[%d].nof_entries",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__NOF_ENTRIES,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_NOF_ENTRIES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_tcam_handler_id_set(int unit, uint32 tables_properties_idx_0, uint32 tcam_handler_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].tcam_handler_id,
        tcam_handler_id,
        uint32,
        0,
        "dbal_db_tables_properties_tcam_handler_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        &tcam_handler_id,
        "dbal_db[%d]->tables_properties[%d].tcam_handler_id",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_TCAM_HANDLER_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_tcam_handler_id_get(int unit, uint32 tables_properties_idx_0, uint32 *tcam_handler_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        tcam_handler_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        DNX_SW_STATE_DIAG_READ);

    *tcam_handler_id = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].tcam_handler_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].tcam_handler_id,
        "dbal_db[%d]->tables_properties[%d].tcam_handler_id",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__TCAM_HANDLER_ID,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_TCAM_HANDLER_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_indications_bm_set(int unit, uint32 tables_properties_idx_0, uint32 indications_bm)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].indications_bm,
        indications_bm,
        uint32,
        0,
        "dbal_db_tables_properties_indications_bm_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        &indications_bm,
        "dbal_db[%d]->tables_properties[%d].indications_bm",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_INDICATIONS_BM_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_indications_bm_get(int unit, uint32 tables_properties_idx_0, uint32 *indications_bm)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        indications_bm);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        DNX_SW_STATE_DIAG_READ);

    *indications_bm = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].indications_bm;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].indications_bm,
        "dbal_db[%d]->tables_properties[%d].indications_bm",
        unit, tables_properties_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__INDICATIONS_BM,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_INDICATIONS_BM_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_iterator_optimized_alloc(int unit, uint32 tables_properties_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized,
        dbal_sw_state_iterator_optimized_t,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_tables_properties_iterator_optimized_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized,
        "dbal_db[%d]->tables_properties[%d].iterator_optimized",
        unit, tables_properties_idx_0,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(dbal_sw_state_iterator_optimized_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_ITERATOR_OPTIMIZED_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_iterator_optimized_bitmap_alloc_bitmap(int unit, uint32 tables_properties_idx_0, uint32 iterator_optimized_idx_0, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized,
        iterator_optimized_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
        _nof_bits_to_alloc,
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_tables_properties_iterator_optimized_bitmap_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
        "dbal_db[%d]->tables_properties[%d].iterator_optimized[%d].bitmap",
        unit, tables_properties_idx_0, iterator_optimized_idx_0,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_ITERATOR_OPTIMIZED_BITMAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_iterator_optimized_bitmap_bit_set(int unit, uint32 tables_properties_idx_0, uint32 iterator_optimized_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized,
        iterator_optimized_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
        "dbal_db[%d]->tables_properties[%d].iterator_optimized[%d].bitmap",
        unit, tables_properties_idx_0, iterator_optimized_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_ITERATOR_OPTIMIZED_BITMAP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_iterator_optimized_bitmap_bit_clear(int unit, uint32 tables_properties_idx_0, uint32 iterator_optimized_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized,
        iterator_optimized_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
        "dbal_db[%d]->tables_properties[%d].iterator_optimized[%d].bitmap",
        unit, tables_properties_idx_0, iterator_optimized_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_ITERATOR_OPTIMIZED_BITMAP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_iterator_optimized_bitmap_bit_get(int unit, uint32 tables_properties_idx_0, uint32 iterator_optimized_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized,
        iterator_optimized_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
        "dbal_db[%d]->tables_properties[%d].iterator_optimized[%d].bitmap",
        unit, tables_properties_idx_0, iterator_optimized_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_ITERATOR_OPTIMIZED_BITMAP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_tables_properties_iterator_optimized_bitmap_bit_range_read(int unit, uint32 tables_properties_idx_0, uint32 iterator_optimized_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties,
        tables_properties_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized,
        iterator_optimized_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
         0,
         ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->tables_properties[tables_properties_idx_0].iterator_optimized[iterator_optimized_idx_0].bitmap,
        "dbal_db[%d]->tables_properties[%d].iterator_optimized[%d].bitmap",
        unit, tables_properties_idx_0, iterator_optimized_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__TABLES_PROPERTIES__ITERATOR_OPTIMIZED__BITMAP,
        dbal_db_info,
        DBAL_DB_TABLES_PROPERTIES_ITERATOR_OPTIMIZED_BITMAP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_table_t,
        dnx_data_dbal.table.nof_dynamic_tables_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_tables_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        "dbal_db[%d]->dbal_dynamic_tables",
        unit,
        dnx_data_dbal.table.nof_dynamic_tables_get(unit) * sizeof(dbal_dynamic_table_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_table_id_set(int unit, uint32 dbal_dynamic_tables_idx_0, dbal_tables_e table_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].table_id,
        table_id,
        dbal_tables_e,
        0,
        "dbal_db_dbal_dynamic_tables_table_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        &table_id,
        "dbal_db[%d]->dbal_dynamic_tables[%d].table_id",
        unit, dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_TABLE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_table_id_get(int unit, uint32 dbal_dynamic_tables_idx_0, dbal_tables_e *table_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        table_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        DNX_SW_STATE_DIAG_READ);

    *table_id = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].table_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].table_id,
        "dbal_db[%d]->dbal_dynamic_tables[%d].table_id",
        unit, dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_ID,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_TABLE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_table_name_set(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 table_name_idx_0, CONST char *table_name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        table_name_idx_0,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].table_name[table_name_idx_0],
        table_name,
        char,
        0,
        "dbal_db_dbal_dynamic_tables_table_name_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        table_name,
        "dbal_db[%d]->dbal_dynamic_tables[%d].table_name[%d]",
        unit, dbal_dynamic_tables_idx_0, table_name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_TABLE_NAME_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_table_name_get(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 table_name_idx_0, CONST char **table_name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        table_name);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        table_name_idx_0,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        DNX_SW_STATE_DIAG_READ);

    *table_name = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].table_name[table_name_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].table_name[table_name_idx_0],
        "dbal_db[%d]->dbal_dynamic_tables[%d].table_name[%d]",
        unit, dbal_dynamic_tables_idx_0, table_name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__TABLE_NAME,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_TABLE_NAME_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_labels_set(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 labels_idx_0, dbal_labels_e labels)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].labels);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].labels,
        labels_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].labels[labels_idx_0],
        labels,
        dbal_labels_e,
        0,
        "dbal_db_dbal_dynamic_tables_labels_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        &labels,
        "dbal_db[%d]->dbal_dynamic_tables[%d].labels[%d]",
        unit, dbal_dynamic_tables_idx_0, labels_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_LABELS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_labels_get(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 labels_idx_0, dbal_labels_e *labels)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        labels);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].labels,
        labels_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].labels);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        DNX_SW_STATE_DIAG_READ);

    *labels = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].labels[labels_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].labels[labels_idx_0],
        "dbal_db[%d]->dbal_dynamic_tables[%d].labels[%d]",
        unit, dbal_dynamic_tables_idx_0, labels_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_LABELS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_labels_alloc(int unit, uint32 dbal_dynamic_tables_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].labels,
        dbal_labels_e,
        dnx_data_dbal.table.nof_dynamic_tables_labels_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_tables_labels_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].labels,
        "dbal_db[%d]->dbal_dynamic_tables[%d].labels",
        unit, dbal_dynamic_tables_idx_0,
        dnx_data_dbal.table.nof_dynamic_tables_labels_get(unit) * sizeof(dbal_labels_e) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].labels));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__LABELS,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_LABELS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_data_set(int unit, uint32 dbal_dynamic_tables_idx_0, CONST dbal_logical_table_t *data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].data,
        data,
        dbal_logical_table_t,
        0,
        "dbal_db_dbal_dynamic_tables_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        data,
        "dbal_db[%d]->dbal_dynamic_tables[%d].data",
        unit, dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_data_get(int unit, uint32 dbal_dynamic_tables_idx_0, CONST dbal_logical_table_t **data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        DNX_SW_STATE_DIAG_READ);

    *data = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].data;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].data,
        "dbal_db[%d]->dbal_dynamic_tables[%d].data",
        unit, dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_key_info_data_set(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 key_info_data_idx_0, CONST dbal_table_field_info_t *key_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].key_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].key_info_data,
        key_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].key_info_data[key_info_data_idx_0],
        key_info_data,
        dbal_table_field_info_t,
        0,
        "dbal_db_dbal_dynamic_tables_key_info_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        key_info_data,
        "dbal_db[%d]->dbal_dynamic_tables[%d].key_info_data[%d]",
        unit, dbal_dynamic_tables_idx_0, key_info_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_KEY_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_key_info_data_get(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 key_info_data_idx_0, CONST dbal_table_field_info_t **key_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        key_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].key_info_data,
        key_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].key_info_data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        DNX_SW_STATE_DIAG_READ);

    *key_info_data = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].key_info_data[key_info_data_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].key_info_data[key_info_data_idx_0],
        "dbal_db[%d]->dbal_dynamic_tables[%d].key_info_data[%d]",
        unit, dbal_dynamic_tables_idx_0, key_info_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_KEY_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_key_info_data_alloc(int unit, uint32 dbal_dynamic_tables_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].key_info_data,
        dbal_table_field_info_t,
        dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_tables_key_info_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].key_info_data,
        "dbal_db[%d]->dbal_dynamic_tables[%d].key_info_data",
        unit, dbal_dynamic_tables_idx_0,
        dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit) * sizeof(dbal_table_field_info_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].key_info_data));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__KEY_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_KEY_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_multi_res_info_data_set(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 multi_res_info_data_idx_0, CONST multi_res_info_t *multi_res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].multi_res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].multi_res_info_data,
        multi_res_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].multi_res_info_data[multi_res_info_data_idx_0],
        multi_res_info_data,
        multi_res_info_t,
        0,
        "dbal_db_dbal_dynamic_tables_multi_res_info_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        multi_res_info_data,
        "dbal_db[%d]->dbal_dynamic_tables[%d].multi_res_info_data[%d]",
        unit, dbal_dynamic_tables_idx_0, multi_res_info_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_MULTI_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_multi_res_info_data_get(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 multi_res_info_data_idx_0, CONST multi_res_info_t **multi_res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        multi_res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].multi_res_info_data,
        multi_res_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].multi_res_info_data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        DNX_SW_STATE_DIAG_READ);

    *multi_res_info_data = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].multi_res_info_data[multi_res_info_data_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].multi_res_info_data[multi_res_info_data_idx_0],
        "dbal_db[%d]->dbal_dynamic_tables[%d].multi_res_info_data[%d]",
        unit, dbal_dynamic_tables_idx_0, multi_res_info_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_MULTI_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_multi_res_info_data_alloc(int unit, uint32 dbal_dynamic_tables_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].multi_res_info_data,
        multi_res_info_t,
        dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_tables_multi_res_info_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].multi_res_info_data,
        "dbal_db[%d]->dbal_dynamic_tables[%d].multi_res_info_data",
        unit, dbal_dynamic_tables_idx_0,
        dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit) * sizeof(multi_res_info_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].multi_res_info_data));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__MULTI_RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_MULTI_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_res_info_data_set(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 res_info_data_idx_0, uint32 res_info_data_idx_1, CONST dbal_table_field_info_t *res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data,
        res_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data[res_info_data_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data[res_info_data_idx_0],
        res_info_data_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data[res_info_data_idx_0][res_info_data_idx_1],
        res_info_data,
        dbal_table_field_info_t,
        0,
        "dbal_db_dbal_dynamic_tables_res_info_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        res_info_data,
        "dbal_db[%d]->dbal_dynamic_tables[%d].res_info_data[%d][%d]",
        unit, dbal_dynamic_tables_idx_0, res_info_data_idx_0, res_info_data_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_res_info_data_get(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 res_info_data_idx_0, uint32 res_info_data_idx_1, CONST dbal_table_field_info_t **res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data,
        res_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data[res_info_data_idx_0],
        res_info_data_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data[res_info_data_idx_0]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        DNX_SW_STATE_DIAG_READ);

    *res_info_data = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data[res_info_data_idx_0][res_info_data_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data[res_info_data_idx_0][res_info_data_idx_1],
        "dbal_db[%d]->dbal_dynamic_tables[%d].res_info_data[%d][%d]",
        unit, dbal_dynamic_tables_idx_0, res_info_data_idx_0, res_info_data_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_res_info_data_alloc(int unit, uint32 dbal_dynamic_tables_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit),
        dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit),
        sizeof(dbal_table_field_info_t*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data,
        dbal_table_field_info_t*,
        dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_tables_res_info_data_alloc");

    for(uint32 res_info_data_idx_0 = 0;
         res_info_data_idx_0 < dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit);
         res_info_data_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data[res_info_data_idx_0],
        dbal_table_field_info_t,
        dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_tables_res_info_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data,
        "dbal_db[%d]->dbal_dynamic_tables[%d].res_info_data",
        unit, dbal_dynamic_tables_idx_0,
        dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].res_info_data)+(dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_access_info_set(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 access_info_idx_0, DNX_SW_STATE_BUFF access_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        access_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info[access_info_idx_0],
        access_info,
        DNX_SW_STATE_BUFF,
        0,
        "dbal_db_dbal_dynamic_tables_access_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        &access_info,
        "dbal_db[%d]->dbal_dynamic_tables[%d].access_info[%d]",
        unit, dbal_dynamic_tables_idx_0, access_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_ACCESS_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_access_info_get(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 access_info_idx_0, DNX_SW_STATE_BUFF *access_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        access_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        access_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        DNX_SW_STATE_DIAG_READ);

    *access_info = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info[access_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info[access_info_idx_0],
        "dbal_db[%d]->dbal_dynamic_tables[%d].access_info[%d]",
        unit, dbal_dynamic_tables_idx_0, access_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_ACCESS_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_access_info_alloc(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 _nof_bytes_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        DNX_SW_STATE_BUFF,
        _nof_bytes_to_alloc,
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_tables_access_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        "dbal_db[%d]->dbal_dynamic_tables[%d].access_info",
        unit, dbal_dynamic_tables_idx_0,
        _nof_bytes_to_alloc * sizeof(DNX_SW_STATE_BUFF) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_ACCESS_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_access_info_memread(int unit, uint32 dbal_dynamic_tables_idx_0, uint8 *_dst, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_MEMREAD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        DNX_SW_STATE_DIAG_READ);

    DNX_SW_STATE_MEMREAD(
        unit,
        _dst,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        _offset,
        _len,
        0,
        "dbal_db_dbal_dynamic_tables_access_info_BUFFER");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MEMREAD_LOGGING,
        BSL_LS_SWSTATEDNX_MEMREAD,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        "dbal_db[%d]->dbal_dynamic_tables[%d].access_info",
        unit, dbal_dynamic_tables_idx_0, _len);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_TABLES_ACCESS_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_access_info_memwrite(int unit, uint32 dbal_dynamic_tables_idx_0, const uint8 *_src, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_MEMWRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_MEMWRITE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        _src,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        _offset,
        _len,
        0,
        "dbal_db_dbal_dynamic_tables_access_info_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_access_info_memcmp(int unit, uint32 dbal_dynamic_tables_idx_0, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_MEMCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_MEMCMP(
        unit,
        _buff,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        _offset,
        _len,
        cmp_result,
        0,
        "dbal_db_dbal_dynamic_tables_access_info_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_tables_access_info_memset(int unit, uint32 dbal_dynamic_tables_idx_0, uint32 _offset, size_t _len, int _value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_MEMSET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables,
        dbal_dynamic_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables);

    DNX_SW_STATE_MEMSET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_tables[dbal_dynamic_tables_idx_0].access_info,
        _offset,
        _value,
        _len,
        0,
        "dbal_db_dbal_dynamic_tables_access_info_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_table_t,
        dnx_data_dbal.table.nof_dynamic_xml_tables_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_xml_tables_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        "dbal_db[%d]->dbal_dynamic_xml_tables",
        unit,
        dnx_data_dbal.table.nof_dynamic_xml_tables_get(unit) * sizeof(dbal_dynamic_table_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_table_id_set(int unit, uint32 dbal_dynamic_xml_tables_idx_0, dbal_tables_e table_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].table_id,
        table_id,
        dbal_tables_e,
        0,
        "dbal_db_dbal_dynamic_xml_tables_table_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        &table_id,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].table_id",
        unit, dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_TABLE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_table_id_get(int unit, uint32 dbal_dynamic_xml_tables_idx_0, dbal_tables_e *table_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        table_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        DNX_SW_STATE_DIAG_READ);

    *table_id = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].table_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].table_id,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].table_id",
        unit, dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_ID,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_TABLE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_table_name_set(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 table_name_idx_0, CONST char *table_name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        table_name_idx_0,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].table_name[table_name_idx_0],
        table_name,
        char,
        0,
        "dbal_db_dbal_dynamic_xml_tables_table_name_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        table_name,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].table_name[%d]",
        unit, dbal_dynamic_xml_tables_idx_0, table_name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_TABLE_NAME_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_table_name_get(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 table_name_idx_0, CONST char **table_name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        table_name);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        table_name_idx_0,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        DNX_SW_STATE_DIAG_READ);

    *table_name = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].table_name[table_name_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].table_name[table_name_idx_0],
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].table_name[%d]",
        unit, dbal_dynamic_xml_tables_idx_0, table_name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__TABLE_NAME,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_TABLE_NAME_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_labels_set(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 labels_idx_0, dbal_labels_e labels)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].labels);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].labels,
        labels_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].labels[labels_idx_0],
        labels,
        dbal_labels_e,
        0,
        "dbal_db_dbal_dynamic_xml_tables_labels_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        &labels,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].labels[%d]",
        unit, dbal_dynamic_xml_tables_idx_0, labels_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_LABELS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_labels_get(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 labels_idx_0, dbal_labels_e *labels)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        labels);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].labels,
        labels_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].labels);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        DNX_SW_STATE_DIAG_READ);

    *labels = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].labels[labels_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].labels[labels_idx_0],
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].labels[%d]",
        unit, dbal_dynamic_xml_tables_idx_0, labels_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_LABELS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_labels_alloc(int unit, uint32 dbal_dynamic_xml_tables_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].labels,
        dbal_labels_e,
        dnx_data_dbal.table.nof_dynamic_tables_labels_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_xml_tables_labels_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].labels,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].labels",
        unit, dbal_dynamic_xml_tables_idx_0,
        dnx_data_dbal.table.nof_dynamic_tables_labels_get(unit) * sizeof(dbal_labels_e) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].labels));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__LABELS,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_LABELS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_data_set(int unit, uint32 dbal_dynamic_xml_tables_idx_0, CONST dbal_logical_table_t *data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].data,
        data,
        dbal_logical_table_t,
        0,
        "dbal_db_dbal_dynamic_xml_tables_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        data,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].data",
        unit, dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_data_get(int unit, uint32 dbal_dynamic_xml_tables_idx_0, CONST dbal_logical_table_t **data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        DNX_SW_STATE_DIAG_READ);

    *data = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].data;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].data,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].data",
        unit, dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_key_info_data_set(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 key_info_data_idx_0, CONST dbal_table_field_info_t *key_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].key_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].key_info_data,
        key_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].key_info_data[key_info_data_idx_0],
        key_info_data,
        dbal_table_field_info_t,
        0,
        "dbal_db_dbal_dynamic_xml_tables_key_info_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        key_info_data,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].key_info_data[%d]",
        unit, dbal_dynamic_xml_tables_idx_0, key_info_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_KEY_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_key_info_data_get(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 key_info_data_idx_0, CONST dbal_table_field_info_t **key_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        key_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].key_info_data,
        key_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].key_info_data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        DNX_SW_STATE_DIAG_READ);

    *key_info_data = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].key_info_data[key_info_data_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].key_info_data[key_info_data_idx_0],
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].key_info_data[%d]",
        unit, dbal_dynamic_xml_tables_idx_0, key_info_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_KEY_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_key_info_data_alloc(int unit, uint32 dbal_dynamic_xml_tables_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].key_info_data,
        dbal_table_field_info_t,
        dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_xml_tables_key_info_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].key_info_data,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].key_info_data",
        unit, dbal_dynamic_xml_tables_idx_0,
        dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit) * sizeof(dbal_table_field_info_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].key_info_data));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__KEY_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_KEY_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_set(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 multi_res_info_data_idx_0, CONST multi_res_info_t *multi_res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].multi_res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].multi_res_info_data,
        multi_res_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].multi_res_info_data[multi_res_info_data_idx_0],
        multi_res_info_data,
        multi_res_info_t,
        0,
        "dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        multi_res_info_data,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].multi_res_info_data[%d]",
        unit, dbal_dynamic_xml_tables_idx_0, multi_res_info_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_MULTI_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_get(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 multi_res_info_data_idx_0, CONST multi_res_info_t **multi_res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        multi_res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].multi_res_info_data,
        multi_res_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].multi_res_info_data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        DNX_SW_STATE_DIAG_READ);

    *multi_res_info_data = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].multi_res_info_data[multi_res_info_data_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].multi_res_info_data[multi_res_info_data_idx_0],
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].multi_res_info_data[%d]",
        unit, dbal_dynamic_xml_tables_idx_0, multi_res_info_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_MULTI_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_alloc(int unit, uint32 dbal_dynamic_xml_tables_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].multi_res_info_data,
        multi_res_info_t,
        dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].multi_res_info_data,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].multi_res_info_data",
        unit, dbal_dynamic_xml_tables_idx_0,
        dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit) * sizeof(multi_res_info_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].multi_res_info_data));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__MULTI_RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_MULTI_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_res_info_data_set(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 res_info_data_idx_0, uint32 res_info_data_idx_1, CONST dbal_table_field_info_t *res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data,
        res_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data[res_info_data_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data[res_info_data_idx_0],
        res_info_data_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data[res_info_data_idx_0][res_info_data_idx_1],
        res_info_data,
        dbal_table_field_info_t,
        0,
        "dbal_db_dbal_dynamic_xml_tables_res_info_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        res_info_data,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].res_info_data[%d][%d]",
        unit, dbal_dynamic_xml_tables_idx_0, res_info_data_idx_0, res_info_data_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_res_info_data_get(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 res_info_data_idx_0, uint32 res_info_data_idx_1, CONST dbal_table_field_info_t **res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data,
        res_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data[res_info_data_idx_0],
        res_info_data_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data[res_info_data_idx_0]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        DNX_SW_STATE_DIAG_READ);

    *res_info_data = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data[res_info_data_idx_0][res_info_data_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data[res_info_data_idx_0][res_info_data_idx_1],
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].res_info_data[%d][%d]",
        unit, dbal_dynamic_xml_tables_idx_0, res_info_data_idx_0, res_info_data_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_res_info_data_alloc(int unit, uint32 dbal_dynamic_xml_tables_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit),
        dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit),
        sizeof(dbal_table_field_info_t*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data,
        dbal_table_field_info_t*,
        dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_xml_tables_res_info_data_alloc");

    for(uint32 res_info_data_idx_0 = 0;
         res_info_data_idx_0 < dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit);
         res_info_data_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data[res_info_data_idx_0],
        dbal_table_field_info_t,
        dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_xml_tables_res_info_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].res_info_data",
        unit, dbal_dynamic_xml_tables_idx_0,
        dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].res_info_data)+(dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_access_info_set(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 access_info_idx_0, DNX_SW_STATE_BUFF access_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        access_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info[access_info_idx_0],
        access_info,
        DNX_SW_STATE_BUFF,
        0,
        "dbal_db_dbal_dynamic_xml_tables_access_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        &access_info,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].access_info[%d]",
        unit, dbal_dynamic_xml_tables_idx_0, access_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_ACCESS_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_access_info_get(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 access_info_idx_0, DNX_SW_STATE_BUFF *access_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        access_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        access_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        DNX_SW_STATE_DIAG_READ);

    *access_info = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info[access_info_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info[access_info_idx_0],
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].access_info[%d]",
        unit, dbal_dynamic_xml_tables_idx_0, access_info_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_ACCESS_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_access_info_alloc(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 _nof_bytes_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        DNX_SW_STATE_BUFF,
        _nof_bytes_to_alloc,
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_dynamic_xml_tables_access_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].access_info",
        unit, dbal_dynamic_xml_tables_idx_0,
        _nof_bytes_to_alloc * sizeof(DNX_SW_STATE_BUFF) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_ACCESS_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_access_info_memread(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint8 *_dst, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_MEMREAD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        DNX_SW_STATE_DIAG_READ);

    DNX_SW_STATE_MEMREAD(
        unit,
        _dst,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        _offset,
        _len,
        0,
        "dbal_db_dbal_dynamic_xml_tables_access_info_BUFFER");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_MEMREAD_LOGGING,
        BSL_LS_SWSTATEDNX_MEMREAD,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        "dbal_db[%d]->dbal_dynamic_xml_tables[%d].access_info",
        unit, dbal_dynamic_xml_tables_idx_0, _len);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        dbal_db_info,
        DBAL_DB_DBAL_DYNAMIC_XML_TABLES_ACCESS_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_access_info_memwrite(int unit, uint32 dbal_dynamic_xml_tables_idx_0, const uint8 *_src, uint32 _offset, size_t _len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_MEMWRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_MEMWRITE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        _src,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        _offset,
        _len,
        0,
        "dbal_db_dbal_dynamic_xml_tables_access_info_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_access_info_memcmp(int unit, uint32 dbal_dynamic_xml_tables_idx_0, const uint8 *_buff, uint32 _offset, size_t _len, int *cmp_result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_MEMCMP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_MEMCMP(
        unit,
        _buff,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        _offset,
        _len,
        cmp_result,
        0,
        "dbal_db_dbal_dynamic_xml_tables_access_info_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_dynamic_xml_tables_access_info_memset(int unit, uint32 dbal_dynamic_xml_tables_idx_0, uint32 _offset, size_t _len, int _value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        SW_STATE_FUNC_MEMSET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables,
        dbal_dynamic_xml_tables_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info);

    DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        (_offset + _len - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables);

    DNX_SW_STATE_MEMSET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_DYNAMIC_XML_TABLES__ACCESS_INFO,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_dynamic_xml_tables[dbal_dynamic_xml_tables_idx_0].access_info,
        _offset,
        _value,
        _len,
        0,
        "dbal_db_dbal_dynamic_xml_tables_access_info_BUFFER");

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_fields_field_id_set(int unit, uint32 dbal_fields_idx_0, dbal_fields_e field_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        dbal_fields_idx_0,
        DBAL_NOF_DYNAMIC_FIELDS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[dbal_fields_idx_0].field_id,
        field_id,
        dbal_fields_e,
        0,
        "dbal_db_dbal_fields_field_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        &field_id,
        "dbal_db[%d]->dbal_fields[%d].field_id",
        unit, dbal_fields_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        dbal_db_info,
        DBAL_DB_DBAL_FIELDS_FIELD_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_fields_field_id_get(int unit, uint32 dbal_fields_idx_0, dbal_fields_e *field_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        dbal_fields_idx_0,
        DBAL_NOF_DYNAMIC_FIELDS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        field_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        DNX_SW_STATE_DIAG_READ);

    *field_id = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[dbal_fields_idx_0].field_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[dbal_fields_idx_0].field_id,
        "dbal_db[%d]->dbal_fields[%d].field_id",
        unit, dbal_fields_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_ID,
        dbal_db_info,
        DBAL_DB_DBAL_FIELDS_FIELD_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_fields_field_type_set(int unit, uint32 dbal_fields_idx_0, dbal_field_types_defs_e field_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        dbal_fields_idx_0,
        DBAL_NOF_DYNAMIC_FIELDS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[dbal_fields_idx_0].field_type,
        field_type,
        dbal_field_types_defs_e,
        0,
        "dbal_db_dbal_fields_field_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        &field_type,
        "dbal_db[%d]->dbal_fields[%d].field_type",
        unit, dbal_fields_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        dbal_db_info,
        DBAL_DB_DBAL_FIELDS_FIELD_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_fields_field_type_get(int unit, uint32 dbal_fields_idx_0, dbal_field_types_defs_e *field_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        dbal_fields_idx_0,
        DBAL_NOF_DYNAMIC_FIELDS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        field_type);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *field_type = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[dbal_fields_idx_0].field_type;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[dbal_fields_idx_0].field_type,
        "dbal_db[%d]->dbal_fields[%d].field_type",
        unit, dbal_fields_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_TYPE,
        dbal_db_info,
        DBAL_DB_DBAL_FIELDS_FIELD_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_fields_field_name_set(int unit, uint32 dbal_fields_idx_0, uint32 field_name_idx_0, CONST char *field_name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        dbal_fields_idx_0,
        DBAL_NOF_DYNAMIC_FIELDS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        field_name_idx_0,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[dbal_fields_idx_0].field_name[field_name_idx_0],
        field_name,
        char,
        0,
        "dbal_db_dbal_fields_field_name_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        field_name,
        "dbal_db[%d]->dbal_fields[%d].field_name[%d]",
        unit, dbal_fields_idx_0, field_name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        dbal_db_info,
        DBAL_DB_DBAL_FIELDS_FIELD_NAME_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_fields_field_name_get(int unit, uint32 dbal_fields_idx_0, uint32 field_name_idx_0, CONST char **field_name)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        dbal_fields_idx_0,
        DBAL_NOF_DYNAMIC_FIELDS);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        field_name);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        field_name_idx_0,
        DBAL_MAX_STRING_LENGTH);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        DNX_SW_STATE_DIAG_READ);

    *field_name = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[dbal_fields_idx_0].field_name[field_name_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_fields[dbal_fields_idx_0].field_name[field_name_idx_0],
        "dbal_db[%d]->dbal_fields[%d].field_name[%d]",
        unit, dbal_fields_idx_0, field_name_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_FIELDS__FIELD_NAME,
        dbal_db_info,
        DBAL_DB_DBAL_FIELDS_FIELD_NAME_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_set(int unit, uint32 multi_res_info_status_idx_0, dbal_multi_res_info_status_e multi_res_info_status)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status,
        multi_res_info_status_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status[multi_res_info_status_idx_0],
        multi_res_info_status,
        dbal_multi_res_info_status_e,
        0,
        "dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        &multi_res_info_status,
        "dbal_db[%d]->dbal_ppmc_table_res_types.multi_res_info_status[%d]",
        unit, multi_res_info_status_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        dbal_db_info,
        DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_STATUS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_get(int unit, uint32 multi_res_info_status_idx_0, dbal_multi_res_info_status_e *multi_res_info_status)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status,
        multi_res_info_status_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        multi_res_info_status);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        DNX_SW_STATE_DIAG_READ);

    *multi_res_info_status = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status[multi_res_info_status_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status[multi_res_info_status_idx_0],
        "dbal_db[%d]->dbal_ppmc_table_res_types.multi_res_info_status[%d]",
        unit, multi_res_info_status_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        dbal_db_info,
        DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_STATUS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status,
        dbal_multi_res_info_status_e,
        dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status,
        "dbal_db[%d]->dbal_ppmc_table_res_types.multi_res_info_status",
        unit,
        dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit) * sizeof(dbal_multi_res_info_status_e) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_status));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_STATUS,
        dbal_db_info,
        DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_STATUS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep,
        dbal_multi_res_info_data_t,
        dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep,
        "dbal_db[%d]->dbal_ppmc_table_res_types.multi_res_info_data_sep",
        unit,
        dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit) * sizeof(dbal_multi_res_info_data_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP,
        dbal_db_info,
        DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_DATA_SEP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_set(int unit, uint32 multi_res_info_data_sep_idx_0, CONST multi_res_info_t *multi_res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep,
        multi_res_info_data_sep_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].multi_res_info_data,
        multi_res_info_data,
        multi_res_info_t,
        0,
        "dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        multi_res_info_data,
        "dbal_db[%d]->dbal_ppmc_table_res_types.multi_res_info_data_sep[%d].multi_res_info_data",
        unit, multi_res_info_data_sep_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_DATA_SEP_MULTI_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_get(int unit, uint32 multi_res_info_data_sep_idx_0, CONST multi_res_info_t **multi_res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep,
        multi_res_info_data_sep_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        multi_res_info_data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        DNX_SW_STATE_DIAG_READ);

    *multi_res_info_data = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].multi_res_info_data;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].multi_res_info_data,
        "dbal_db[%d]->dbal_ppmc_table_res_types.multi_res_info_data_sep[%d].multi_res_info_data",
        unit, multi_res_info_data_sep_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__MULTI_RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_DATA_SEP_MULTI_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_set(int unit, uint32 multi_res_info_data_sep_idx_0, uint32 res_info_data_idx_0, dbal_table_field_info_t res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep,
        multi_res_info_data_sep_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].res_info_data,
        res_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].res_info_data[res_info_data_idx_0],
        res_info_data,
        dbal_table_field_info_t,
        0,
        "dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        &res_info_data,
        "dbal_db[%d]->dbal_ppmc_table_res_types.multi_res_info_data_sep[%d].res_info_data[%d]",
        unit, multi_res_info_data_sep_idx_0, res_info_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_DATA_SEP_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_get(int unit, uint32 multi_res_info_data_sep_idx_0, uint32 res_info_data_idx_0, dbal_table_field_info_t *res_info_data)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep,
        multi_res_info_data_sep_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        res_info_data);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].res_info_data,
        res_info_data_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].res_info_data);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        DNX_SW_STATE_DIAG_READ);

    *res_info_data = ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].res_info_data[res_info_data_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].res_info_data[res_info_data_idx_0],
        "dbal_db[%d]->dbal_ppmc_table_res_types.multi_res_info_data_sep[%d].res_info_data[%d]",
        unit, multi_res_info_data_sep_idx_0, res_info_data_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_DATA_SEP_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_alloc(int unit, uint32 multi_res_info_data_sep_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep,
        multi_res_info_data_sep_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].res_info_data,
        dbal_table_field_info_t,
        dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit),
        sw_state_layout_array[unit][DBAL_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].res_info_data,
        "dbal_db[%d]->dbal_ppmc_table_res_types.multi_res_info_data_sep[%d].res_info_data",
        unit, multi_res_info_data_sep_idx_0,
        dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t) / sizeof(*((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->dbal_ppmc_table_res_types.multi_res_info_data_sep[multi_res_info_data_sep_idx_0].res_info_data));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__DBAL_PPMC_TABLE_RES_TYPES__MULTI_RES_INFO_DATA_SEP__RES_INFO_DATA,
        dbal_db_info,
        DBAL_DB_DBAL_PPMC_TABLE_RES_TYPES_MULTI_RES_INFO_DATA_SEP_RES_INFO_DATA_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_direct_map_hash_tbl_create(int unit, sw_state_idx_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE);

    SW_STATE_INDEX_HTB_CREATE(
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        init_info,
        hw_ent_direct_map_hash_key,
        uint32,
        dnx_data_dbal.hw_ent.nof_direct_maps_get(unit),
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCREATE,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        "dbal_db[%d]->hw_ent_direct_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_DIRECT_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_direct_map_hash_tbl_find(int unit, const hw_ent_direct_map_hash_key *key, uint32 *data_index, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_FIND(
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        key,
        data_index,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBFIND,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        "dbal_db[%d]->hw_ent_direct_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_DIRECT_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_direct_map_hash_tbl_insert(int unit, const hw_ent_direct_map_hash_key *key, const uint32 *data_index, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT(
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        key,
        data_index,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        "dbal_db[%d]->hw_ent_direct_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_DIRECT_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_direct_map_hash_tbl_get_next(int unit, SW_STATE_INDEX_HASH_TABLE_ITER *iter, const hw_ent_direct_map_hash_key *key, const uint32 *data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_NEXT(
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        iter,
        key,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_NEXT,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        "dbal_db[%d]->hw_ent_direct_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_DIRECT_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_direct_map_hash_tbl_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_CLEAR(
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCLEAR,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        "dbal_db[%d]->hw_ent_direct_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_DIRECT_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_direct_map_hash_tbl_delete(int unit, const hw_ent_direct_map_hash_key *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE(
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        "dbal_db[%d]->hw_ent_direct_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_DIRECT_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_direct_map_hash_tbl_insert_at_index(int unit, const hw_ent_direct_map_hash_key *key, uint32 data_idx, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT_AT_INDEX(
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        key,
        data_idx,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT_AT_INDEX,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        "dbal_db[%d]->hw_ent_direct_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_DIRECT_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_direct_map_hash_tbl_delete_by_index(int unit, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE_BY_INDEX(
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE_BY_INDEX,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        "dbal_db[%d]->hw_ent_direct_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_DIRECT_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_direct_map_hash_tbl_get_by_index(int unit, uint32 data_index, hw_ent_direct_map_hash_key *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_BY_INDEX(
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        data_index,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_BY_INDEX,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_direct_map_hash_tbl,
        "dbal_db[%d]->hw_ent_direct_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_DIRECT_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_DIRECT_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_groups_map_hash_tbl_create(int unit, sw_state_idx_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE);

    SW_STATE_INDEX_HTB_CREATE(
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        init_info,
        hw_ent_groups_map_hash_key,
        uint32,
        dnx_data_dbal.hw_ent.nof_groups_maps_get(unit),
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCREATE,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        "dbal_db[%d]->hw_ent_groups_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_GROUPS_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_groups_map_hash_tbl_find(int unit, const hw_ent_groups_map_hash_key *key, uint32 *data_index, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_FIND(
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        key,
        data_index,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBFIND,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        "dbal_db[%d]->hw_ent_groups_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_GROUPS_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_groups_map_hash_tbl_insert(int unit, const hw_ent_groups_map_hash_key *key, const uint32 *data_index, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT(
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        key,
        data_index,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        "dbal_db[%d]->hw_ent_groups_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_GROUPS_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_groups_map_hash_tbl_get_next(int unit, SW_STATE_INDEX_HASH_TABLE_ITER *iter, const hw_ent_groups_map_hash_key *key, const uint32 *data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_NEXT(
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        iter,
        key,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_NEXT,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        "dbal_db[%d]->hw_ent_groups_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_GROUPS_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_groups_map_hash_tbl_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_CLEAR(
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCLEAR,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        "dbal_db[%d]->hw_ent_groups_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_GROUPS_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_groups_map_hash_tbl_delete(int unit, const hw_ent_groups_map_hash_key *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE(
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        "dbal_db[%d]->hw_ent_groups_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_GROUPS_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_groups_map_hash_tbl_insert_at_index(int unit, const hw_ent_groups_map_hash_key *key, uint32 data_idx, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT_AT_INDEX(
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        key,
        data_idx,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT_AT_INDEX,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        "dbal_db[%d]->hw_ent_groups_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_GROUPS_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_groups_map_hash_tbl_delete_by_index(int unit, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE_BY_INDEX(
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE_BY_INDEX,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        "dbal_db[%d]->hw_ent_groups_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_GROUPS_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_hw_ent_groups_map_hash_tbl_get_by_index(int unit, uint32 data_index, hw_ent_groups_map_hash_key *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_BY_INDEX(
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        data_index,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_BY_INDEX,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->hw_ent_groups_map_hash_tbl,
        "dbal_db[%d]->hw_ent_groups_map_hash_tbl",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__HW_ENT_GROUPS_MAP_HASH_TBL,
        dbal_db_info,
        DBAL_DB_HW_ENT_GROUPS_MAP_HASH_TBL_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_field_names_override_locations_create(int unit, sw_state_idx_htbl_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE);

    SW_STATE_INDEX_HTB_CREATE(
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        init_info,
        dbal_fields_e,
        uint32,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCREATE,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        "dbal_db[%d]->field_names_override_locations",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        dbal_db_info,
        DBAL_DB_FIELD_NAMES_OVERRIDE_LOCATIONS_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB_CREATE,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_field_names_override_locations_find(int unit, const dbal_fields_e *key, uint32 *data_index, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_FIND(
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        key,
        data_index,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBFIND,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        "dbal_db[%d]->field_names_override_locations",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        dbal_db_info,
        DBAL_DB_FIELD_NAMES_OVERRIDE_LOCATIONS_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_field_names_override_locations_insert(int unit, const dbal_fields_e *key, const uint32 *data_index, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        SW_STATE_FUNC_INSERT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT(
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        key,
        data_index,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        "dbal_db[%d]->field_names_override_locations",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        dbal_db_info,
        DBAL_DB_FIELD_NAMES_OVERRIDE_LOCATIONS_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_field_names_override_locations_get_next(int unit, SW_STATE_INDEX_HASH_TABLE_ITER *iter, const dbal_fields_e *key, const uint32 *data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_NEXT(
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        iter,
        key,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_NEXT,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        "dbal_db[%d]->field_names_override_locations",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        dbal_db_info,
        DBAL_DB_FIELD_NAMES_OVERRIDE_LOCATIONS_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_field_names_override_locations_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_CLEAR(
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBCLEAR,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        "dbal_db[%d]->field_names_override_locations",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        dbal_db_info,
        DBAL_DB_FIELD_NAMES_OVERRIDE_LOCATIONS_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_field_names_override_locations_delete(int unit, const dbal_fields_e *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        SW_STATE_FUNC_DELETE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE(
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        "dbal_db[%d]->field_names_override_locations",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        dbal_db_info,
        DBAL_DB_FIELD_NAMES_OVERRIDE_LOCATIONS_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_field_names_override_locations_insert_at_index(int unit, const dbal_fields_e *key, uint32 data_idx, uint8 *success)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        SW_STATE_FUNC_INSERT_AT_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_INSERT_AT_INDEX(
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        key,
        data_idx,
        success);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_INSERT_AT_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBINSERT_AT_INDEX,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        "dbal_db[%d]->field_names_override_locations",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        dbal_db_info,
        DBAL_DB_FIELD_NAMES_OVERRIDE_LOCATIONS_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_field_names_override_locations_delete_by_index(int unit, uint32 data_index)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        SW_STATE_FUNC_DELETE_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_DELETE_BY_INDEX(
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        data_index);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_DELETE_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBDELETE_BY_INDEX,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        "dbal_db[%d]->field_names_override_locations",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        dbal_db_info,
        DBAL_DB_FIELD_NAMES_OVERRIDE_LOCATIONS_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_field_names_override_locations_get_by_index(int unit, uint32 data_index, dbal_fields_e *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        SW_STATE_FUNC_GET_BY_INDEX,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        DNX_SW_STATE_DIAG_INDEX_HTB);

    SW_STATE_INDEX_HTB_GET_BY_INDEX(
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        data_index,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INDEX_HTB_GET_BY_INDEX_LOGGING,
        BSL_LS_SWSTATEDNX_INDEX_HTBGET_BY_INDEX,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_locations,
        "dbal_db[%d]->field_names_override_locations",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_LOCATIONS,
        dbal_db_info,
        DBAL_DB_FIELD_NAMES_OVERRIDE_LOCATIONS_INFO,
        DNX_SW_STATE_DIAG_INDEX_HTB,
        NULL,
        DNXC_SW_STATE_NO_FLAGS,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_field_names_override_strings_set(int unit, uint32 field_names_override_strings_idx_0, CONST encap_string_type_t *field_names_override_strings)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        field_names_override_strings_idx_0,
        DBAL_NOF_FIELD_NAME_OVERRIDE_INSTANCES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        ((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_strings[field_names_override_strings_idx_0],
        field_names_override_strings,
        encap_string_type_t,
        0,
        "dbal_db_field_names_override_strings_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        field_names_override_strings,
        "dbal_db[%d]->field_names_override_strings[%d]",
        unit, field_names_override_strings_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        dbal_db_info,
        DBAL_DB_FIELD_NAMES_OVERRIDE_STRINGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dbal_db_field_names_override_strings_get(int unit, uint32 field_names_override_strings_idx_0, CONST encap_string_type_t **field_names_override_strings)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        sw_state_roots_array[unit][DBAL_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        field_names_override_strings_idx_0,
        DBAL_NOF_FIELD_NAME_OVERRIDE_INSTANCES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        field_names_override_strings);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        DNX_SW_STATE_DIAG_READ);

    *field_names_override_strings = &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_strings[field_names_override_strings_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        &((dnx_dbal_sw_state_t*)sw_state_roots_array[unit][DBAL_MODULE_ID])->field_names_override_strings[field_names_override_strings_idx_0],
        "dbal_db[%d]->field_names_override_strings[%d]",
        unit, field_names_override_strings_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DBAL_DB__FIELD_NAMES_OVERRIDE_STRINGS,
        dbal_db_info,
        DBAL_DB_FIELD_NAMES_OVERRIDE_STRINGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DBAL_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



const char *
dbal_sw_state_table_type_e_get_name(dbal_sw_state_table_type_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DBAL_SW_TABLE_DIRECT", value, DBAL_SW_TABLE_DIRECT);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DBAL_SW_TABLE_HASH", value, DBAL_SW_TABLE_HASH);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DBAL_NOF_SW_TABLES_TYPES", value, DBAL_NOF_SW_TABLES_TYPES);

    return NULL;
}



const char *
dbal_table_status_e_get_name(dbal_table_status_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DBAL_TABLE_NOT_INITIALIZED", value, DBAL_TABLE_NOT_INITIALIZED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DBAL_TABLE_INITIALIZED", value, DBAL_TABLE_INITIALIZED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DBAL_TABLE_HW_ERROR", value, DBAL_TABLE_HW_ERROR);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DBAL_TABLE_INCOMPATIBLE_IMAGE", value, DBAL_TABLE_INCOMPATIBLE_IMAGE);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DBAL_NOF_TABLE_STATUS", value, DBAL_NOF_TABLE_STATUS);

    return NULL;
}



const char *
dbal_multi_res_info_status_e_get_name(dbal_multi_res_info_status_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DBAL_MULTI_RES_INFO_AVAIL", value, DBAL_MULTI_RES_INFO_AVAIL);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DBAL_MULTI_RES_INFO_USED", value, DBAL_MULTI_RES_INFO_USED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("DBAL_NOF_MULTI_RES_INFO_TYPES", value, DBAL_NOF_MULTI_RES_INFO_TYPES);

    return NULL;
}




dbal_db_cbs dbal_db = 	{
	
	dbal_db_is_init,
	dbal_db_init,
		{
		
			{
			
			dbal_db_sw_tables_table_type_set,
			dbal_db_sw_tables_table_type_get}
		,
			{
			
			dbal_db_sw_tables_entries_alloc_bitmap,
			dbal_db_sw_tables_entries_bit_set,
			dbal_db_sw_tables_entries_bit_clear,
			dbal_db_sw_tables_entries_bit_get,
			dbal_db_sw_tables_entries_bit_range_read,
			dbal_db_sw_tables_entries_bit_range_write,
			dbal_db_sw_tables_entries_bit_range_and,
			dbal_db_sw_tables_entries_bit_range_or,
			dbal_db_sw_tables_entries_bit_range_xor,
			dbal_db_sw_tables_entries_bit_range_remove,
			dbal_db_sw_tables_entries_bit_range_negate,
			dbal_db_sw_tables_entries_bit_range_clear,
			dbal_db_sw_tables_entries_bit_range_set,
			dbal_db_sw_tables_entries_bit_range_null,
			dbal_db_sw_tables_entries_bit_range_test,
			dbal_db_sw_tables_entries_bit_range_eq,
			dbal_db_sw_tables_entries_bit_range_count}
		,
			{
			
			dbal_db_sw_tables_hash_table_create,
			dbal_db_sw_tables_hash_table_find,
			dbal_db_sw_tables_hash_table_insert,
			dbal_db_sw_tables_hash_table_get_next,
			dbal_db_sw_tables_hash_table_clear,
			dbal_db_sw_tables_hash_table_delete,
			dbal_db_sw_tables_hash_table_insert_at_index,
			dbal_db_sw_tables_hash_table_delete_by_index,
			dbal_db_sw_tables_hash_table_get_by_index}
		}
	,
		{
		
			{
			
			dbal_db_mdb_access_skip_read_from_shadow_set,
			dbal_db_mdb_access_skip_read_from_shadow_get}
		}
	,
		{
		
			{
			
			dbal_db_mdb_phy_tables_nof_entries_set,
			dbal_db_mdb_phy_tables_nof_entries_get,
			dbal_db_mdb_phy_tables_nof_entries_inc,
			dbal_db_mdb_phy_tables_nof_entries_dec}
		}
	,
		{
		
		dbal_db_tables_properties_alloc,
			{
			
			dbal_db_tables_properties_table_status_set,
			dbal_db_tables_properties_table_status_get}
		,
			{
			
			dbal_db_tables_properties_nof_set_operations_set,
			dbal_db_tables_properties_nof_set_operations_get,
			dbal_db_tables_properties_nof_set_operations_inc,
			dbal_db_tables_properties_nof_set_operations_dec}
		,
			{
			
			dbal_db_tables_properties_nof_get_operations_set,
			dbal_db_tables_properties_nof_get_operations_get,
			dbal_db_tables_properties_nof_get_operations_inc,
			dbal_db_tables_properties_nof_get_operations_dec}
		,
			{
			
			dbal_db_tables_properties_nof_entries_set,
			dbal_db_tables_properties_nof_entries_get,
			dbal_db_tables_properties_nof_entries_inc,
			dbal_db_tables_properties_nof_entries_dec}
		,
			{
			
			dbal_db_tables_properties_tcam_handler_id_set,
			dbal_db_tables_properties_tcam_handler_id_get}
		,
			{
			
			dbal_db_tables_properties_indications_bm_set,
			dbal_db_tables_properties_indications_bm_get}
		,
			{
			
			dbal_db_tables_properties_iterator_optimized_alloc,
				{
				
				dbal_db_tables_properties_iterator_optimized_bitmap_alloc_bitmap,
				dbal_db_tables_properties_iterator_optimized_bitmap_bit_set,
				dbal_db_tables_properties_iterator_optimized_bitmap_bit_clear,
				dbal_db_tables_properties_iterator_optimized_bitmap_bit_get,
				dbal_db_tables_properties_iterator_optimized_bitmap_bit_range_read}
			}
		}
	,
		{
		
		dbal_db_dbal_dynamic_tables_alloc,
			{
			
			dbal_db_dbal_dynamic_tables_table_id_set,
			dbal_db_dbal_dynamic_tables_table_id_get}
		,
			{
			
			dbal_db_dbal_dynamic_tables_table_name_set,
			dbal_db_dbal_dynamic_tables_table_name_get}
		,
			{
			
			dbal_db_dbal_dynamic_tables_labels_set,
			dbal_db_dbal_dynamic_tables_labels_get,
			dbal_db_dbal_dynamic_tables_labels_alloc}
		,
			{
			
			dbal_db_dbal_dynamic_tables_data_set,
			dbal_db_dbal_dynamic_tables_data_get}
		,
			{
			
			dbal_db_dbal_dynamic_tables_key_info_data_set,
			dbal_db_dbal_dynamic_tables_key_info_data_get,
			dbal_db_dbal_dynamic_tables_key_info_data_alloc}
		,
			{
			
			dbal_db_dbal_dynamic_tables_multi_res_info_data_set,
			dbal_db_dbal_dynamic_tables_multi_res_info_data_get,
			dbal_db_dbal_dynamic_tables_multi_res_info_data_alloc}
		,
			{
			
			dbal_db_dbal_dynamic_tables_res_info_data_set,
			dbal_db_dbal_dynamic_tables_res_info_data_get,
			dbal_db_dbal_dynamic_tables_res_info_data_alloc}
		,
			{
			
			dbal_db_dbal_dynamic_tables_access_info_set,
			dbal_db_dbal_dynamic_tables_access_info_get,
			dbal_db_dbal_dynamic_tables_access_info_alloc,
			dbal_db_dbal_dynamic_tables_access_info_memread,
			dbal_db_dbal_dynamic_tables_access_info_memwrite,
			dbal_db_dbal_dynamic_tables_access_info_memcmp,
			dbal_db_dbal_dynamic_tables_access_info_memset}
		}
	,
		{
		
		dbal_db_dbal_dynamic_xml_tables_alloc,
			{
			
			dbal_db_dbal_dynamic_xml_tables_table_id_set,
			dbal_db_dbal_dynamic_xml_tables_table_id_get}
		,
			{
			
			dbal_db_dbal_dynamic_xml_tables_table_name_set,
			dbal_db_dbal_dynamic_xml_tables_table_name_get}
		,
			{
			
			dbal_db_dbal_dynamic_xml_tables_labels_set,
			dbal_db_dbal_dynamic_xml_tables_labels_get,
			dbal_db_dbal_dynamic_xml_tables_labels_alloc}
		,
			{
			
			dbal_db_dbal_dynamic_xml_tables_data_set,
			dbal_db_dbal_dynamic_xml_tables_data_get}
		,
			{
			
			dbal_db_dbal_dynamic_xml_tables_key_info_data_set,
			dbal_db_dbal_dynamic_xml_tables_key_info_data_get,
			dbal_db_dbal_dynamic_xml_tables_key_info_data_alloc}
		,
			{
			
			dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_set,
			dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_get,
			dbal_db_dbal_dynamic_xml_tables_multi_res_info_data_alloc}
		,
			{
			
			dbal_db_dbal_dynamic_xml_tables_res_info_data_set,
			dbal_db_dbal_dynamic_xml_tables_res_info_data_get,
			dbal_db_dbal_dynamic_xml_tables_res_info_data_alloc}
		,
			{
			
			dbal_db_dbal_dynamic_xml_tables_access_info_set,
			dbal_db_dbal_dynamic_xml_tables_access_info_get,
			dbal_db_dbal_dynamic_xml_tables_access_info_alloc,
			dbal_db_dbal_dynamic_xml_tables_access_info_memread,
			dbal_db_dbal_dynamic_xml_tables_access_info_memwrite,
			dbal_db_dbal_dynamic_xml_tables_access_info_memcmp,
			dbal_db_dbal_dynamic_xml_tables_access_info_memset}
		}
	,
		{
		
			{
			
			dbal_db_dbal_fields_field_id_set,
			dbal_db_dbal_fields_field_id_get}
		,
			{
			
			dbal_db_dbal_fields_field_type_set,
			dbal_db_dbal_fields_field_type_get}
		,
			{
			
			dbal_db_dbal_fields_field_name_set,
			dbal_db_dbal_fields_field_name_get}
		}
	,
		{
		
			{
			
			dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_set,
			dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_get,
			dbal_db_dbal_ppmc_table_res_types_multi_res_info_status_alloc}
		,
			{
			
			dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_alloc,
				{
				
				dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_set,
				dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_multi_res_info_data_get}
			,
				{
				
				dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_set,
				dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_get,
				dbal_db_dbal_ppmc_table_res_types_multi_res_info_data_sep_res_info_data_alloc}
			}
		}
	,
		{
		
		dbal_db_hw_ent_direct_map_hash_tbl_create,
		dbal_db_hw_ent_direct_map_hash_tbl_find,
		dbal_db_hw_ent_direct_map_hash_tbl_insert,
		dbal_db_hw_ent_direct_map_hash_tbl_get_next,
		dbal_db_hw_ent_direct_map_hash_tbl_clear,
		dbal_db_hw_ent_direct_map_hash_tbl_delete,
		dbal_db_hw_ent_direct_map_hash_tbl_insert_at_index,
		dbal_db_hw_ent_direct_map_hash_tbl_delete_by_index,
		dbal_db_hw_ent_direct_map_hash_tbl_get_by_index}
	,
		{
		
		dbal_db_hw_ent_groups_map_hash_tbl_create,
		dbal_db_hw_ent_groups_map_hash_tbl_find,
		dbal_db_hw_ent_groups_map_hash_tbl_insert,
		dbal_db_hw_ent_groups_map_hash_tbl_get_next,
		dbal_db_hw_ent_groups_map_hash_tbl_clear,
		dbal_db_hw_ent_groups_map_hash_tbl_delete,
		dbal_db_hw_ent_groups_map_hash_tbl_insert_at_index,
		dbal_db_hw_ent_groups_map_hash_tbl_delete_by_index,
		dbal_db_hw_ent_groups_map_hash_tbl_get_by_index}
	,
		{
		
		dbal_db_field_names_override_locations_create,
		dbal_db_field_names_override_locations_find,
		dbal_db_field_names_override_locations_insert,
		dbal_db_field_names_override_locations_get_next,
		dbal_db_field_names_override_locations_clear,
		dbal_db_field_names_override_locations_delete,
		dbal_db_field_names_override_locations_insert_at_index,
		dbal_db_field_names_override_locations_delete_by_index,
		dbal_db_field_names_override_locations_get_by_index}
	,
		{
		
		dbal_db_field_names_override_strings_set,
		dbal_db_field_names_override_strings_get}
	}
;
#undef BSL_LOG_MODULE
