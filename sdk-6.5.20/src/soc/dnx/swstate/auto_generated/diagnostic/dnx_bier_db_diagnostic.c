
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_bier_db_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_bier_db_t * dnx_bier_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_bier_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_bier_db_set_size_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_bier_db_set_size_dump(int  unit,  int  set_size_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = set_size_idx_0, I0 = set_size_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_bier_db set_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_bier_db set_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_bier_db/set_size.txt",
            "dnx_bier_db[%d]->","((dnx_bier_db_t*)sw_state_roots_array[%d][DNX_BIER_DB_MODULE_ID])->","set_size[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_bier_db_t*)sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_MULTICAST_PARAMS_NOF_MCDB_ENTRIES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_bier_db_t*)sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID])->set_size
                , sizeof(*((dnx_bier_db_t*)sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID])->set_size), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_MULTICAST_PARAMS_NOF_MCDB_ENTRIES)
        {
            LOG_CLI((BSL_META("dnx_bier_db[]->((dnx_bier_db_t*)sw_state_roots_array[][DNX_BIER_DB_MODULE_ID])->set_size[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_MULTICAST_PARAMS_NOF_MCDB_ENTRIES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_bier_db[%d]->","((dnx_bier_db_t*)sw_state_roots_array[%d][DNX_BIER_DB_MODULE_ID])->","set_size[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_bier_db_t*)sw_state_roots_array[unit][DNX_BIER_DB_MODULE_ID])->set_size[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_bier_db_info[SOC_MAX_NUM_DEVICES][DNX_BIER_DB_INFO_NOF_ENTRIES];
const char* dnx_bier_db_layout_str[DNX_BIER_DB_INFO_NOF_ENTRIES] = {
    "DNX_BIER_DB~",
    "DNX_BIER_DB~SET_SIZE~",
};
#endif 
#undef BSL_LOG_MODULE
