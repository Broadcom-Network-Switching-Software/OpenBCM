
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef BCM_DNX_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/kbp_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern kbp_sw_state_t * kbp_sw_state_data[SOC_MAX_NUM_DEVICES];



int
kbp_sw_state_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(kbp_sw_state_is_device_locked_dump(unit, filters));
    SHR_IF_ERR_EXIT(kbp_sw_state_db_handles_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(kbp_sw_state_instruction_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_is_device_locked_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "kbp is_device_locked") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate kbp is_device_locked\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "kbp_sw_state/is_device_locked.txt",
            "kbp_sw_state[%d]->","((kbp_sw_state_t*)sw_state_roots_array[%d][KBP_MODULE_ID])->","is_device_locked: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "kbp_sw_state[%d]->","((kbp_sw_state_t*)sw_state_roots_array[%d][KBP_MODULE_ID])->","is_device_locked: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->is_device_locked);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_dump(int  unit,  int  db_handles_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(kbp_sw_state_db_handles_info_caching_bmp_dump(unit, db_handles_info_idx_0, filters));
    SHR_IF_ERR_EXIT(kbp_sw_state_db_handles_info_db_p_dump(unit, db_handles_info_idx_0, filters));
    SHR_IF_ERR_EXIT(kbp_sw_state_db_handles_info_ad_db_zero_size_p_dump(unit, db_handles_info_idx_0, filters));
    SHR_IF_ERR_EXIT(kbp_sw_state_db_handles_info_ad_entry_zero_size_p_dump(unit, db_handles_info_idx_0, filters));
    SHR_IF_ERR_EXIT(kbp_sw_state_db_handles_info_ad_db_p_dump(unit, db_handles_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(kbp_sw_state_db_handles_info_opt_result_size_dump(unit, db_handles_info_idx_0, filters));
    SHR_IF_ERR_EXIT(kbp_sw_state_db_handles_info_cloned_db_id_dump(unit, db_handles_info_idx_0, filters));
    SHR_IF_ERR_EXIT(kbp_sw_state_db_handles_info_associated_dbal_table_id_dump(unit, db_handles_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_caching_bmp_dump(int  unit,  int  db_handles_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_handles_info_idx_0, I0 = db_handles_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "kbp db_handles_info caching_bmp") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate kbp db_handles_info caching_bmp\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "kbp_sw_state/db_handles_info/caching_bmp.txt",
            "kbp_sw_state[%d]->","((kbp_sw_state_t*)sw_state_roots_array[%d][KBP_MODULE_ID])->","db_handles_info[].caching_bmp: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_KBP_NOF_DBS;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info
                , sizeof(*((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_KBP_NOF_DBS)
        {
            LOG_CLI((BSL_META("kbp_sw_state[]->((kbp_sw_state_t*)sw_state_roots_array[][KBP_MODULE_ID])->db_handles_info[].caching_bmp: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_KBP_NOF_DBS == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "kbp_sw_state[%d]->","((kbp_sw_state_t*)sw_state_roots_array[%d][KBP_MODULE_ID])->","db_handles_info[%s%d].caching_bmp: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[i0].caching_bmp);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_db_p_dump(int  unit,  int  db_handles_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_ad_db_zero_size_p_dump(int  unit,  int  db_handles_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_ad_entry_zero_size_p_dump(int  unit,  int  db_handles_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = db_handles_info_idx_0, I0 = db_handles_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "kbp_ad_entry_t_p") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "kbp db_handles_info ad_entry_zero_size_p") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate kbp db_handles_info ad_entry_zero_size_p\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "kbp_sw_state/db_handles_info/ad_entry_zero_size_p.txt",
            "kbp_sw_state[%d]->","((kbp_sw_state_t*)sw_state_roots_array[%d][KBP_MODULE_ID])->","db_handles_info[].ad_entry_zero_size_p: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_KBP_NOF_DBS;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info
                , sizeof(*((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_KBP_NOF_DBS)
        {
            LOG_CLI((BSL_META("kbp_sw_state[]->((kbp_sw_state_t*)sw_state_roots_array[][KBP_MODULE_ID])->db_handles_info[].ad_entry_zero_size_p: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_KBP_NOF_DBS == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[i0].ad_entry_zero_size_p,
                "kbp_sw_state[%d]->","((kbp_sw_state_t*)sw_state_roots_array[%d][KBP_MODULE_ID])->","db_handles_info[%s%d].ad_entry_zero_size_p: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((kbp_sw_state_t*)sw_state_roots_array[unit][KBP_MODULE_ID])->db_handles_info[i0].ad_entry_zero_size_p,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_ad_db_p_dump(int  unit,  int  db_handles_info_idx_0,  int  ad_db_p_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_opt_result_size_dump(int  unit,  int  db_handles_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_cloned_db_id_dump(int  unit,  int  db_handles_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_db_handles_info_associated_dbal_table_id_dump(int  unit,  int  db_handles_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_instruction_info_dump(int  unit,  int  instruction_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(kbp_sw_state_instruction_info_inst_p_dump(unit, instruction_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_sw_state_instruction_info_inst_p_dump(int  unit,  int  instruction_info_idx_0,  int  inst_p_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t kbp_sw_state_info[SOC_MAX_NUM_DEVICES][KBP_SW_STATE_INFO_NOF_ENTRIES];
const char* kbp_sw_state_layout_str[KBP_SW_STATE_INFO_NOF_ENTRIES] = {
    "KBP_SW_STATE~",
    "KBP_SW_STATE~IS_DEVICE_LOCKED~",
    "KBP_SW_STATE~DB_HANDLES_INFO~",
    "KBP_SW_STATE~DB_HANDLES_INFO~CACHING_BMP~",
    "KBP_SW_STATE~DB_HANDLES_INFO~DB_P~",
    "KBP_SW_STATE~DB_HANDLES_INFO~AD_DB_ZERO_SIZE_P~",
    "KBP_SW_STATE~DB_HANDLES_INFO~AD_ENTRY_ZERO_SIZE_P~",
    "KBP_SW_STATE~DB_HANDLES_INFO~AD_DB_P~",
    "KBP_SW_STATE~DB_HANDLES_INFO~OPT_RESULT_SIZE~",
    "KBP_SW_STATE~DB_HANDLES_INFO~CLONED_DB_ID~",
    "KBP_SW_STATE~DB_HANDLES_INFO~ASSOCIATED_DBAL_TABLE_ID~",
    "KBP_SW_STATE~INSTRUCTION_INFO~",
    "KBP_SW_STATE~INSTRUCTION_INFO~INST_P~",
};
#endif 
#endif  
#endif  
#undef BSL_LOG_MODULE
