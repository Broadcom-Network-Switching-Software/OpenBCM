
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
#include <soc/dnx/swstate/auto_generated/diagnostic/stif_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_stif_db_t * dnx_stif_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_stif_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_stif_db_instance_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_stif_db_source_mapping_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_instance_dump(int  unit,  int  instance_idx_0,  int  instance_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_stif_db_instance_logical_port_dump(unit, instance_idx_0,instance_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_instance_logical_port_dump(int  unit,  int  instance_idx_0,  int  instance_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = instance_idx_0, I0 = instance_idx_0 + 1;
    int i1 = instance_idx_1, I1 = instance_idx_1 + 1, org_i1 = instance_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "bcm_port_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "stif instance logical_port") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate stif instance logical_port\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_stif_db/instance/logical_port.txt",
            "dnx_stif_db[%d]->","((dnx_stif_db_t*)sw_state_roots_array[%d][STIF_MODULE_ID])->","instance[][].logical_port: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                STIF_MODULE_ID,
                ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance
                , sizeof(*((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance))
        {
            LOG_CLI((BSL_META("dnx_stif_db[]->((dnx_stif_db_t*)sw_state_roots_array[][STIF_MODULE_ID])->instance[][].logical_port: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    STIF_MODULE_ID,
                    ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[i0]
                    , sizeof(*((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[i0]))
            {
                LOG_CLI((BSL_META("dnx_stif_db[]->((dnx_stif_db_t*)sw_state_roots_array[][STIF_MODULE_ID])->instance[][].logical_port: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_stif_db[%d]->","((dnx_stif_db_t*)sw_state_roots_array[%d][STIF_MODULE_ID])->","instance[%s%d][%s%d].logical_port: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->instance[i0][i1].logical_port);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_source_mapping_dump(int  unit,  int  source_mapping_idx_0,  int  source_mapping_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_stif_db_source_mapping_first_port_dump(unit, source_mapping_idx_0,source_mapping_idx_1, filters));
    SHR_IF_ERR_EXIT(dnx_stif_db_source_mapping_second_port_dump(unit, source_mapping_idx_0,source_mapping_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_source_mapping_first_port_dump(int  unit,  int  source_mapping_idx_0,  int  source_mapping_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = source_mapping_idx_0, I0 = source_mapping_idx_0 + 1;
    int i1 = source_mapping_idx_1, I1 = source_mapping_idx_1 + 1, org_i1 = source_mapping_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "bcm_port_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "stif source_mapping first_port") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate stif source_mapping first_port\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_stif_db/source_mapping/first_port.txt",
            "dnx_stif_db[%d]->","((dnx_stif_db_t*)sw_state_roots_array[%d][STIF_MODULE_ID])->","source_mapping[][].first_port: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                STIF_MODULE_ID,
                ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping
                , sizeof(*((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping))
        {
            LOG_CLI((BSL_META("dnx_stif_db[]->((dnx_stif_db_t*)sw_state_roots_array[][STIF_MODULE_ID])->source_mapping[][].first_port: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    STIF_MODULE_ID,
                    ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]
                    , sizeof(*((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]))
            {
                LOG_CLI((BSL_META("dnx_stif_db[]->((dnx_stif_db_t*)sw_state_roots_array[][STIF_MODULE_ID])->source_mapping[][].first_port: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_stif_db[%d]->","((dnx_stif_db_t*)sw_state_roots_array[%d][STIF_MODULE_ID])->","source_mapping[%s%d][%s%d].first_port: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0][i1].first_port);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_stif_db_source_mapping_second_port_dump(int  unit,  int  source_mapping_idx_0,  int  source_mapping_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = source_mapping_idx_0, I0 = source_mapping_idx_0 + 1;
    int i1 = source_mapping_idx_1, I1 = source_mapping_idx_1 + 1, org_i1 = source_mapping_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "bcm_port_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "stif source_mapping second_port") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate stif source_mapping second_port\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_stif_db/source_mapping/second_port.txt",
            "dnx_stif_db[%d]->","((dnx_stif_db_t*)sw_state_roots_array[%d][STIF_MODULE_ID])->","source_mapping[][].second_port: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                STIF_MODULE_ID,
                ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping
                , sizeof(*((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping))
        {
            LOG_CLI((BSL_META("dnx_stif_db[]->((dnx_stif_db_t*)sw_state_roots_array[][STIF_MODULE_ID])->source_mapping[][].second_port: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    STIF_MODULE_ID,
                    ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]
                    , sizeof(*((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]))
            {
                LOG_CLI((BSL_META("dnx_stif_db[]->((dnx_stif_db_t*)sw_state_roots_array[][STIF_MODULE_ID])->source_mapping[][].second_port: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, STIF_MODULE_ID, ((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_stif_db[%d]->","((dnx_stif_db_t*)sw_state_roots_array[%d][STIF_MODULE_ID])->","source_mapping[%s%d][%s%d].second_port: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_stif_db_t*)sw_state_roots_array[unit][STIF_MODULE_ID])->source_mapping[i0][i1].second_port);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_stif_db_info[SOC_MAX_NUM_DEVICES][DNX_STIF_DB_INFO_NOF_ENTRIES];
const char* dnx_stif_db_layout_str[DNX_STIF_DB_INFO_NOF_ENTRIES] = {
    "DNX_STIF_DB~",
    "DNX_STIF_DB~INSTANCE~",
    "DNX_STIF_DB~INSTANCE~LOGICAL_PORT~",
    "DNX_STIF_DB~SOURCE_MAPPING~",
    "DNX_STIF_DB~SOURCE_MAPPING~FIRST_PORT~",
    "DNX_STIF_DB~SOURCE_MAPPING~SECOND_PORT~",
};
#endif 
#undef BSL_LOG_MODULE
