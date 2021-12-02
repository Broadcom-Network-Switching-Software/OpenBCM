
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/sat_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern sat_pkt_header_info_t * sat_pkt_header_info_data[SOC_MAX_NUM_DEVICES];



int
sat_pkt_header_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(sat_pkt_header_info_pkt_header_base_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
sat_pkt_header_info_pkt_header_base_dump(int  unit,  int  pkt_header_base_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pkt_header_base_idx_0, I0 = pkt_header_base_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint16") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "sat pkt_header_base") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate sat pkt_header_base\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "sat_pkt_header_info/pkt_header_base.txt",
            "sat_pkt_header_info[%d]->","((sat_pkt_header_info_t*)sw_state_roots_array[%d][SAT_MODULE_ID])->","pkt_header_base[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                SAT_MODULE_ID,
                ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base
                , sizeof(*((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, SAT_MODULE_ID, ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base))
        {
            LOG_CLI((BSL_META("sat_pkt_header_info[]->((sat_pkt_header_info_t*)sw_state_roots_array[][SAT_MODULE_ID])->pkt_header_base[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, SAT_MODULE_ID, ((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "sat_pkt_header_info[%d]->","((sat_pkt_header_info_t*)sw_state_roots_array[%d][SAT_MODULE_ID])->","pkt_header_base[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint16(
                unit,
                &((sat_pkt_header_info_t*)sw_state_roots_array[unit][SAT_MODULE_ID])->pkt_header_base[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t sat_pkt_header_info_info[SOC_MAX_NUM_DEVICES][SAT_PKT_HEADER_INFO_INFO_NOF_ENTRIES];
const char* sat_pkt_header_info_layout_str[SAT_PKT_HEADER_INFO_INFO_NOF_ENTRIES] = {
    "SAT_PKT_HEADER_INFO~",
    "SAT_PKT_HEADER_INFO~PKT_HEADER_BASE~",
};
#endif 
#undef BSL_LOG_MODULE
