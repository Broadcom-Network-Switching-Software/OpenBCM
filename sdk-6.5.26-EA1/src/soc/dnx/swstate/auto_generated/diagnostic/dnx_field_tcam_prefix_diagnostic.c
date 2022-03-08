
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_prefix_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_prefix_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_tcam_prefix_sw_t * dnx_field_tcam_prefix_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_tcam_prefix_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw_banks_prefix_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_prefix_sw_banks_prefix_dump(int  unit,  int  banks_prefix_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw_banks_prefix_core_prefix_map_dump(unit, banks_prefix_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_prefix_sw_banks_prefix_core_prefix_map_dump(int  unit,  int  banks_prefix_idx_0,  int  core_prefix_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw_banks_prefix_core_prefix_map_prefix_handler_map_dump(unit, banks_prefix_idx_0, core_prefix_map_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_prefix_sw_banks_prefix_core_prefix_map_prefix_handler_map_dump(int  unit,  int  banks_prefix_idx_0,  int  core_prefix_map_idx_0,  int  prefix_handler_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = banks_prefix_idx_0, I0 = banks_prefix_idx_0 + 1;
    int i1 = core_prefix_map_idx_0, I1 = core_prefix_map_idx_0 + 1, org_i1 = core_prefix_map_idx_0;
    int i2 = prefix_handler_map_idx_0, I2 = prefix_handler_map_idx_0 + 1, org_i2 = prefix_handler_map_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_prefix banks_prefix core_prefix_map prefix_handler_map") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_prefix banks_prefix core_prefix_map prefix_handler_map\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_prefix_sw/banks_prefix/core_prefix_map/prefix_handler_map.txt",
            "dnx_field_tcam_prefix_sw[%d]->","((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->","banks_prefix[].core_prefix_map[].prefix_handler_map[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__PREFIX_HANDLER_MAP,
                ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix
                , sizeof(*((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__PREFIX_HANDLER_MAP, ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_prefix_sw[]->((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[].core_prefix_map[].prefix_handler_map[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__PREFIX_HANDLER_MAP, ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__PREFIX_HANDLER_MAP,
                    ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map
                    , sizeof(*((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__PREFIX_HANDLER_MAP, ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_prefix_sw[]->((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[].core_prefix_map[].prefix_handler_map[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__PREFIX_HANDLER_MAP, ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map[i1].prefix_handler_map);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__PREFIX_HANDLER_MAP,
                        ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map[i1].prefix_handler_map);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map[i1].prefix_handler_map
                        , sizeof(*((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map[i1].prefix_handler_map), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__PREFIX_HANDLER_MAP, ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map[i1].prefix_handler_map))
                {
                    LOG_CLI((BSL_META("dnx_field_tcam_prefix_sw[]->((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[].core_prefix_map[].prefix_handler_map[]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__PREFIX_HANDLER_MAP, ((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map[i1].prefix_handler_map) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_tcam_prefix_sw[%d]->","((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->","banks_prefix[%s%d].core_prefix_map[%s%d].prefix_handler_map[%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((dnx_field_tcam_prefix_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_PREFIX_MODULE_ID])->banks_prefix[i0].core_prefix_map[i1].prefix_handler_map[i2]);

                }
                i2 = org_i2;
                dnx_sw_state_dump_end_of_stride(unit);
            }
            i1 = org_i1;
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_tcam_prefix_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_PREFIX_SW_INFO_NOF_ENTRIES];
const char* dnx_field_tcam_prefix_sw_layout_str[DNX_FIELD_TCAM_PREFIX_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_TCAM_PREFIX_SW~",
    "DNX_FIELD_TCAM_PREFIX_SW~BANKS_PREFIX~",
    "DNX_FIELD_TCAM_PREFIX_SW~BANKS_PREFIX~CORE_PREFIX_MAP~",
    "DNX_FIELD_TCAM_PREFIX_SW~BANKS_PREFIX~CORE_PREFIX_MAP~PREFIX_HANDLER_MAP~",
};
#endif 
#undef BSL_LOG_MODULE
