
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_sch_config_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_sch_config_t * cosq_config_data[SOC_MAX_NUM_DEVICES];



int
cosq_config_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(cosq_config_hr_group_bw_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(cosq_config_groups_bw_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(cosq_config_ipf_config_mode_dump(unit, filters));
    SHR_IF_ERR_EXIT(cosq_config_flow_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(cosq_config_connector_dump(unit, -1, -1, filters));
    SHR_IF_ERR_EXIT(cosq_config_se_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_hr_group_bw_dump(int  unit,  int  hr_group_bw_idx_0,  int  hr_group_bw_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = hr_group_bw_idx_0, I0 = hr_group_bw_idx_0 + 1;
    int i1 = hr_group_bw_idx_1, I1 = hr_group_bw_idx_1 + 1, org_i1 = hr_group_bw_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_sch_config hr_group_bw") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_sch_config hr_group_bw\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "cosq_config/hr_group_bw.txt",
            "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","hr_group_bw[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw
                , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SCH_CONFIG_MODULE_ID,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[i0]
                    , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[i0]))
            {
                LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","hr_group_bw[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->hr_group_bw[i0][i1]);

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
cosq_config_groups_bw_dump(int  unit,  int  groups_bw_idx_0,  int  groups_bw_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = groups_bw_idx_0, I0 = groups_bw_idx_0 + 1;
    int i1 = groups_bw_idx_1, I1 = groups_bw_idx_1 + 1, org_i1 = groups_bw_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_sch_config groups_bw") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_sch_config groups_bw\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "cosq_config/groups_bw.txt",
            "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","groups_bw[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->groups_bw
                , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->groups_bw), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->groups_bw[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 3;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->groups_bw[i0]
                    , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->groups_bw[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 3)
            {
                LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->groups_bw[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(3 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","groups_bw[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->groups_bw[i0][i1]);

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
cosq_config_ipf_config_mode_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_sch_ipf_config_mode_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_sch_config ipf_config_mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_sch_config ipf_config_mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "cosq_config/ipf_config_mode.txt",
            "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","ipf_config_mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]));

        DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
            unit,
            ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->ipf_config_mode,
            "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","ipf_config_mode: ");

        DNX_SW_STATE_PRINT_OPAQUE_FILE(
            unit,
            ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->ipf_config_mode,
                " ");

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_flow_dump(int  unit,  int  flow_idx_0,  int  flow_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(cosq_config_flow_credit_src_dump(unit, flow_idx_0,flow_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_flow_credit_src_dump(int  unit,  int  flow_idx_0,  int  flow_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(cosq_config_flow_credit_src_weight_dump(unit, flow_idx_0, flow_idx_1, filters));
    SHR_IF_ERR_EXIT(cosq_config_flow_credit_src_mode_dump(unit, flow_idx_0, flow_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_flow_credit_src_weight_dump(int  unit,  int  flow_idx_0,  int  flow_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = flow_idx_0, I0 = flow_idx_0 + 1;
    int i1 = flow_idx_1, I1 = flow_idx_1 + 1, org_i1 = flow_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_sch_config flow credit_src weight") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_sch_config flow credit_src weight\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "cosq_config/flow/credit_src/weight.txt",
            "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","flow[][].credit_src.weight: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow
                , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->flow[][].credit_src.weight: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SCH_CONFIG_MODULE_ID,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]
                    , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]))
            {
                LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->flow[][].credit_src.weight: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","flow[%s%d][%s%d].credit_src.weight: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0][i1].credit_src.weight);

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
cosq_config_flow_credit_src_mode_dump(int  unit,  int  flow_idx_0,  int  flow_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = flow_idx_0, I0 = flow_idx_0 + 1;
    int i1 = flow_idx_1, I1 = flow_idx_1 + 1, org_i1 = flow_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_sch_config flow credit_src mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_sch_config flow credit_src mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "cosq_config/flow/credit_src/mode.txt",
            "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","flow[][].credit_src.mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow
                , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->flow[][].credit_src.mode: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SCH_CONFIG_MODULE_ID,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]
                    , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]))
            {
                LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->flow[][].credit_src.mode: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","flow[%s%d][%s%d].credit_src.mode: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->flow[i0][i1].credit_src.mode);

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
cosq_config_connector_dump(int  unit,  int  connector_idx_0,  int  connector_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(cosq_config_connector_num_cos_dump(unit, connector_idx_0,connector_idx_1, filters));
    SHR_IF_ERR_EXIT(cosq_config_connector_connection_valid_dump(unit, connector_idx_0,connector_idx_1, filters));
    SHR_IF_ERR_EXIT(cosq_config_connector_src_modid_dump(unit, connector_idx_0,connector_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_connector_num_cos_dump(int  unit,  int  connector_idx_0,  int  connector_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = connector_idx_0, I0 = connector_idx_0 + 1;
    int i1 = connector_idx_1, I1 = connector_idx_1 + 1, org_i1 = connector_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_sch_config connector num_cos") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_sch_config connector num_cos\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "cosq_config/connector/num_cos.txt",
            "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","connector[][].num_cos: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector
                , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->connector[][].num_cos: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SCH_CONFIG_MODULE_ID,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]
                    , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]))
            {
                LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->connector[][].num_cos: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","connector[%s%d][%s%d].num_cos: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0][i1].num_cos);

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
cosq_config_connector_connection_valid_dump(int  unit,  int  connector_idx_0,  int  connector_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = connector_idx_0, I0 = connector_idx_0 + 1;
    int i1 = connector_idx_1, I1 = connector_idx_1 + 1, org_i1 = connector_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_sch_config connector connection_valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_sch_config connector connection_valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "cosq_config/connector/connection_valid.txt",
            "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","connector[][].connection_valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector
                , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->connector[][].connection_valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SCH_CONFIG_MODULE_ID,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]
                    , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]))
            {
                LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->connector[][].connection_valid: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","connector[%s%d][%s%d].connection_valid: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0][i1].connection_valid);

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
cosq_config_connector_src_modid_dump(int  unit,  int  connector_idx_0,  int  connector_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = connector_idx_0, I0 = connector_idx_0 + 1;
    int i1 = connector_idx_1, I1 = connector_idx_1 + 1, org_i1 = connector_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_sch_config connector src_modid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_sch_config connector src_modid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "cosq_config/connector/src_modid.txt",
            "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","connector[][].src_modid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector
                , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->connector[][].src_modid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SCH_CONFIG_MODULE_ID,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]
                    , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]))
            {
                LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->connector[][].src_modid: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","connector[%s%d][%s%d].src_modid: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->connector[i0][i1].src_modid);

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
cosq_config_se_dump(int  unit,  int  se_idx_0,  int  se_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(cosq_config_se_child_count_dump(unit, se_idx_0,se_idx_1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
cosq_config_se_child_count_dump(int  unit,  int  se_idx_0,  int  se_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = se_idx_0, I0 = se_idx_0 + 1;
    int i1 = se_idx_1, I1 = se_idx_1 + 1, org_i1 = se_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_sch_config se child_count") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_sch_config se child_count\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "cosq_config/se/child_count.txt",
            "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","se[][].child_count: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se
                , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES)
        {
            LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->se[][].child_count: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SCH_CONFIG_MODULE_ID,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[i0]
                    , sizeof(*((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[i0]))
            {
                LOG_CLI((BSL_META("cosq_config[]->((dnx_sch_config_t*)sw_state_roots_array[][DNX_SCH_CONFIG_MODULE_ID])->se[][].child_count: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SCH_CONFIG_MODULE_ID, ((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "cosq_config[%d]->","((dnx_sch_config_t*)sw_state_roots_array[%d][DNX_SCH_CONFIG_MODULE_ID])->","se[%s%d][%s%d].child_count: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_sch_config_t*)sw_state_roots_array[unit][DNX_SCH_CONFIG_MODULE_ID])->se[i0][i1].child_count);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t cosq_config_info[SOC_MAX_NUM_DEVICES][COSQ_CONFIG_INFO_NOF_ENTRIES];
const char* cosq_config_layout_str[COSQ_CONFIG_INFO_NOF_ENTRIES] = {
    "COSQ_CONFIG~",
    "COSQ_CONFIG~HR_GROUP_BW~",
    "COSQ_CONFIG~GROUPS_BW~",
    "COSQ_CONFIG~IPF_CONFIG_MODE~",
    "COSQ_CONFIG~FLOW~",
    "COSQ_CONFIG~FLOW~CREDIT_SRC~",
    "COSQ_CONFIG~FLOW~CREDIT_SRC~WEIGHT~",
    "COSQ_CONFIG~FLOW~CREDIT_SRC~MODE~",
    "COSQ_CONFIG~CONNECTOR~",
    "COSQ_CONFIG~CONNECTOR~NUM_COS~",
    "COSQ_CONFIG~CONNECTOR~CONNECTION_VALID~",
    "COSQ_CONFIG~CONNECTOR~SRC_MODID~",
    "COSQ_CONFIG~SE~",
    "COSQ_CONFIG~SE~CHILD_COUNT~",
};
#endif 
#undef BSL_LOG_MODULE
