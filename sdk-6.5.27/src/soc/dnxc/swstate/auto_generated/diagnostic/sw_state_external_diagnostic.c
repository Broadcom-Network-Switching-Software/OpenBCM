
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/auto_generated/diagnostic/sw_state_external_diagnostic.h>
#include <soc/dnxc/swstate/dnxc_sw_state_external.h>
#include <soc/dnxc/swstate/auto_generated/layout/sw_state_external_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern sw_state_external_t * sw_state_external_data[SOC_MAX_NUM_DEVICES];



int
sw_state_external_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(sw_state_external_offsets_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
sw_state_external_offsets_dump(int  unit,  int  offsets_idx_0,  int  offsets_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = offsets_idx_0, I0 = offsets_idx_0 + 1;
    int i1 = offsets_idx_1, I1 = offsets_idx_1 + 1, org_i1 = offsets_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "sw_state_external offsets") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate sw_state_external offsets\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "sw_state_external/offsets.txt",
            "sw_state_external[%d]->","((sw_state_external_t*)sw_state_roots_array[%d][SW_STATE_EXTERNAL_MODULE_ID])->","offsets[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNXC_SW_STATE_EXTERNAL_COUNT;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID])->offsets
                , sizeof(*((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID])->offsets), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNXC_SW_STATE_EXTERNAL_COUNT)
        {
            LOG_CLI((BSL_META("sw_state_external[]->((sw_state_external_t*)sw_state_roots_array[][SW_STATE_EXTERNAL_MODULE_ID])->offsets[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNXC_SW_STATE_EXTERNAL_COUNT == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNXC_SW_STATE_EXTERNAL_OFFSETS_PER_MODULE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID])->offsets[i0]
                    , sizeof(*((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID])->offsets[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNXC_SW_STATE_EXTERNAL_OFFSETS_PER_MODULE)
            {
                LOG_CLI((BSL_META("sw_state_external[]->((sw_state_external_t*)sw_state_roots_array[][SW_STATE_EXTERNAL_MODULE_ID])->offsets[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNXC_SW_STATE_EXTERNAL_OFFSETS_PER_MODULE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "sw_state_external[%d]->","((sw_state_external_t*)sw_state_roots_array[%d][SW_STATE_EXTERNAL_MODULE_ID])->","offsets[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((sw_state_external_t*)sw_state_roots_array[unit][SW_STATE_EXTERNAL_MODULE_ID])->offsets[i0][i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_sw_state_diagnostic_info_t sw_state_external_info[SOC_MAX_NUM_DEVICES][SW_STATE_EXTERNAL_INFO_NOF_ENTRIES];
const char* sw_state_external_layout_str[SW_STATE_EXTERNAL_INFO_NOF_ENTRIES] = {
    "SW_STATE_EXTERNAL~",
    "SW_STATE_EXTERNAL~OFFSETS~",
};
#endif 
#undef BSL_LOG_MODULE
