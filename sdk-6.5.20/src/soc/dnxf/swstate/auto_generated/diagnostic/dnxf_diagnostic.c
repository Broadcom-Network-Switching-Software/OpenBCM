
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
#include <soc/dnxf/swstate/auto_generated/diagnostic/dnxf_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnxf_state_t * dnxf_state_data[SOC_MAX_NUM_DEVICES];



int
dnxf_state_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_modid_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnxf_state_mc_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnxf_state_lb_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnxf_state_port_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnxf_state_intr_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnxf_state_lane_map_db_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnxf_state_synce_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_modid_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_modid_local_map_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnxf_state_modid_group_map_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnxf_state_modid_modid_to_group_map_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_modid_local_map_dump(int  unit,  int  local_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_modid_local_map_valid_dump(unit, local_map_idx_0, filters));
    SHR_IF_ERR_EXIT(dnxf_state_modid_local_map_module_id_dump(unit, local_map_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_modid_local_map_valid_dump(int  unit,  int  local_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = local_map_idx_0, I0 = local_map_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf modid local_map valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf modid local_map valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/modid/local_map/valid.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","modid.local_map[].valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->modid.local_map[].valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","modid.local_map[%s%d].valid: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map[i0].valid);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_modid_local_map_module_id_dump(int  unit,  int  local_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = local_map_idx_0, I0 = local_map_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf modid local_map module_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf modid local_map module_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/modid/local_map/module_id.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","modid.local_map[].module_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->modid.local_map[].module_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","modid.local_map[%s%d].module_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map[i0].module_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_modid_group_map_dump(int  unit,  int  group_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_modid_group_map_raw_dump(unit, group_map_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_modid_group_map_raw_dump(int  unit,  int  group_map_idx_0,  int  raw_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = group_map_idx_0, I0 = group_map_idx_0 + 1;
    int i1 = raw_idx_0, I1 = raw_idx_0 + 1, org_i1 = raw_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf modid group_map raw") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf modid group_map raw\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/modid/group_map/raw.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","modid.group_map[].raw[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->modid.group_map[].raw[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = SOC_DNXF_MODID_NOF_UINT32_SIZE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[i0].raw
                    , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[i0].raw), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= SOC_DNXF_MODID_NOF_UINT32_SIZE)
            {
                LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->modid.group_map[].raw[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(SOC_DNXF_MODID_NOF_UINT32_SIZE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","modid.group_map[%s%d].raw[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[i0].raw[i1]);

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
dnxf_state_modid_modid_to_group_map_dump(int  unit,  int  modid_to_group_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = modid_to_group_map_idx_0, I0 = modid_to_group_map_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "soc_module_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf modid modid_to_group_map") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf modid modid_to_group_map\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/modid/modid_to_group_map.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","modid.modid_to_group_map[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        if (i0 == -1) {
            I0 = SOC_DNXF_MODID_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= SOC_DNXF_MODID_NOF)
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->modid.modid_to_group_map[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(SOC_DNXF_MODID_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","modid.modid_to_group_map[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_mc_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_mc_mode_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnxf_state_mc_id_map_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnxf_state_mc_nof_mc_groups_created_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_mc_mode_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf mc mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf mc mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/mc/mode.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","mc.mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","mc.mode: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.mode);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_mc_id_map_dump(int  unit,  int  id_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = id_map_idx_0, I0 = id_map_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf mc id_map") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf mc id_map\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/mc/id_map.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","mc.id_map[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->mc.id_map[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","mc.id_map[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_mc_nof_mc_groups_created_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf mc nof_mc_groups_created") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf mc nof_mc_groups_created\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/mc/nof_mc_groups_created.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","mc.nof_mc_groups_created: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","mc.nof_mc_groups_created: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.nof_mc_groups_created);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_lb_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_lb_group_to_first_link_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_lb_group_to_first_link_dump(int  unit,  int  group_to_first_link_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = group_to_first_link_idx_0, I0 = group_to_first_link_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "soc_port_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf lb group_to_first_link") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf lb group_to_first_link\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/lb/group_to_first_link.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lb.group_to_first_link[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->lb.group_to_first_link[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lb.group_to_first_link[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_port_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_port_cl72_conf_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnxf_state_port_phy_lane_config_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnxf_state_port_is_connected_to_repeater_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnxf_state_port_isolation_status_stored_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnxf_state_port_orig_isolated_status_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_port_cl72_conf_dump(int  unit,  int  cl72_conf_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = cl72_conf_idx_0, I0 = cl72_conf_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf port cl72_conf") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf port cl72_conf\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/port/cl72_conf.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","port.cl72_conf[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->port.cl72_conf[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","port.cl72_conf[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_port_phy_lane_config_dump(int  unit,  int  phy_lane_config_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = phy_lane_config_idx_0, I0 = phy_lane_config_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf port phy_lane_config") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf port phy_lane_config\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/port/phy_lane_config.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","port.phy_lane_config[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->port.phy_lane_config[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","port.phy_lane_config[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_port_is_connected_to_repeater_dump(int  unit,  int  is_connected_to_repeater_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = is_connected_to_repeater_idx_0, I0 = is_connected_to_repeater_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "SHR_BITDCL") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf port is_connected_to_repeater") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf port is_connected_to_repeater\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/port/is_connected_to_repeater.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","port.is_connected_to_repeater[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->port.is_connected_to_repeater[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","port.is_connected_to_repeater[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_shr_bitdcl(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_port_isolation_status_stored_dump(int  unit,  int  isolation_status_stored_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = isolation_status_stored_idx_0, I0 = isolation_status_stored_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf port isolation_status_stored") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf port isolation_status_stored\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/port/isolation_status_stored.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","port.isolation_status_stored[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->port.isolation_status_stored[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","port.isolation_status_stored[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_port_orig_isolated_status_dump(int  unit,  int  orig_isolated_status_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = orig_isolated_status_idx_0, I0 = orig_isolated_status_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf port orig_isolated_status") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf port orig_isolated_status\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/port/orig_isolated_status.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","port.orig_isolated_status[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->port.orig_isolated_status[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","port.orig_isolated_status[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_intr_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_intr_flags_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnxf_state_intr_storm_timed_count_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnxf_state_intr_storm_timed_period_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnxf_state_intr_storm_nominal_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_intr_flags_dump(int  unit,  int  flags_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = flags_idx_0, I0 = flags_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf intr flags") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf intr flags\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/intr/flags.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","intr.flags[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->intr.flags[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","intr.flags[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_intr_storm_timed_count_dump(int  unit,  int  storm_timed_count_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = storm_timed_count_idx_0, I0 = storm_timed_count_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf intr storm_timed_count") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf intr storm_timed_count\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/intr/storm_timed_count.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","intr.storm_timed_count[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->intr.storm_timed_count[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","intr.storm_timed_count[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_intr_storm_timed_period_dump(int  unit,  int  storm_timed_period_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = storm_timed_period_idx_0, I0 = storm_timed_period_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf intr storm_timed_period") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf intr storm_timed_period\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/intr/storm_timed_period.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","intr.storm_timed_period[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->intr.storm_timed_period[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","intr.storm_timed_period[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_intr_storm_nominal_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf intr storm_nominal") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf intr storm_nominal\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/intr/storm_nominal.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","intr.storm_nominal: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","intr.storm_nominal: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_nominal);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_lane_map_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_lane_map_db_map_size_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnxf_state_lane_map_db_lane2serdes_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnxf_state_lane_map_db_serdes2lane_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_lane_map_db_map_size_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf lane_map_db map_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf lane_map_db map_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/lane_map_db/map_size.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lane_map_db.map_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lane_map_db.map_size: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.map_size);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_lane_map_db_lane2serdes_dump(int  unit,  int  lane2serdes_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_lane_map_db_lane2serdes_rx_id_dump(unit, lane2serdes_idx_0, filters));
    SHR_IF_ERR_EXIT(dnxf_state_lane_map_db_lane2serdes_tx_id_dump(unit, lane2serdes_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_lane_map_db_lane2serdes_rx_id_dump(int  unit,  int  lane2serdes_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = lane2serdes_idx_0, I0 = lane2serdes_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf lane_map_db lane2serdes rx_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf lane_map_db lane2serdes rx_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/lane_map_db/lane2serdes/rx_id.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lane_map_db.lane2serdes[].rx_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->lane_map_db.lane2serdes[].rx_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lane_map_db.lane2serdes[%s%d].rx_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes[i0].rx_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_lane_map_db_lane2serdes_tx_id_dump(int  unit,  int  lane2serdes_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = lane2serdes_idx_0, I0 = lane2serdes_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf lane_map_db lane2serdes tx_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf lane_map_db lane2serdes tx_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/lane_map_db/lane2serdes/tx_id.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lane_map_db.lane2serdes[].tx_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->lane_map_db.lane2serdes[].tx_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lane_map_db.lane2serdes[%s%d].tx_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.lane2serdes[i0].tx_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_lane_map_db_serdes2lane_dump(int  unit,  int  serdes2lane_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_lane_map_db_serdes2lane_rx_id_dump(unit, serdes2lane_idx_0, filters));
    SHR_IF_ERR_EXIT(dnxf_state_lane_map_db_serdes2lane_tx_id_dump(unit, serdes2lane_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_lane_map_db_serdes2lane_rx_id_dump(int  unit,  int  serdes2lane_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = serdes2lane_idx_0, I0 = serdes2lane_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf lane_map_db serdes2lane rx_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf lane_map_db serdes2lane rx_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/lane_map_db/serdes2lane/rx_id.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lane_map_db.serdes2lane[].rx_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->lane_map_db.serdes2lane[].rx_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lane_map_db.serdes2lane[%s%d].rx_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane[i0].rx_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_lane_map_db_serdes2lane_tx_id_dump(int  unit,  int  serdes2lane_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = serdes2lane_idx_0, I0 = serdes2lane_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf lane_map_db serdes2lane tx_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf lane_map_db serdes2lane tx_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/lane_map_db/serdes2lane/tx_id.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lane_map_db.serdes2lane[].tx_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNXF_MODULE_ID,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane
                , sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane))
        {
            LOG_CLI((BSL_META("dnxf_state[]->((dnxf_state_t*)sw_state_roots_array[][DNXF_MODULE_ID])->lane_map_db.serdes2lane[].tx_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNXF_MODULE_ID, ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","lane_map_db.serdes2lane[%s%d].tx_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes2lane[i0].tx_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_synce_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnxf_state_synce_master_synce_enabled_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnxf_state_synce_slave_synce_enabled_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_synce_master_synce_enabled_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf synce master_synce_enabled") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf synce master_synce_enabled\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/synce/master_synce_enabled.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","synce.master_synce_enabled: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","synce.master_synce_enabled: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.master_synce_enabled);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnxf_state_synce_slave_synce_enabled_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnxf synce slave_synce_enabled") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnxf synce slave_synce_enabled\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnxf_state/synce/slave_synce_enabled.txt",
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","synce.slave_synce_enabled: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnxf_state[%d]->","((dnxf_state_t*)sw_state_roots_array[%d][DNXF_MODULE_ID])->","synce.slave_synce_enabled: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.slave_synce_enabled);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnxf_state_info[SOC_MAX_NUM_DEVICES][DNXF_STATE_INFO_NOF_ENTRIES];
const char* dnxf_state_layout_str[DNXF_STATE_INFO_NOF_ENTRIES] = {
    "DNXF_STATE~",
    "DNXF_STATE~MODID~",
    "DNXF_STATE~MODID~LOCAL_MAP~",
    "DNXF_STATE~MODID~LOCAL_MAP~VALID~",
    "DNXF_STATE~MODID~LOCAL_MAP~MODULE_ID~",
    "DNXF_STATE~MODID~GROUP_MAP~",
    "DNXF_STATE~MODID~GROUP_MAP~RAW~",
    "DNXF_STATE~MODID~MODID_TO_GROUP_MAP~",
    "DNXF_STATE~MC~",
    "DNXF_STATE~MC~MODE~",
    "DNXF_STATE~MC~ID_MAP~",
    "DNXF_STATE~MC~NOF_MC_GROUPS_CREATED~",
    "DNXF_STATE~LB~",
    "DNXF_STATE~LB~GROUP_TO_FIRST_LINK~",
    "DNXF_STATE~PORT~",
    "DNXF_STATE~PORT~CL72_CONF~",
    "DNXF_STATE~PORT~PHY_LANE_CONFIG~",
    "DNXF_STATE~PORT~IS_CONNECTED_TO_REPEATER~",
    "DNXF_STATE~PORT~ISOLATION_STATUS_STORED~",
    "DNXF_STATE~PORT~ORIG_ISOLATED_STATUS~",
    "DNXF_STATE~INTR~",
    "DNXF_STATE~INTR~FLAGS~",
    "DNXF_STATE~INTR~STORM_TIMED_COUNT~",
    "DNXF_STATE~INTR~STORM_TIMED_PERIOD~",
    "DNXF_STATE~INTR~STORM_NOMINAL~",
    "DNXF_STATE~LANE_MAP_DB~",
    "DNXF_STATE~LANE_MAP_DB~MAP_SIZE~",
    "DNXF_STATE~LANE_MAP_DB~LANE2SERDES~",
    "DNXF_STATE~LANE_MAP_DB~LANE2SERDES~RX_ID~",
    "DNXF_STATE~LANE_MAP_DB~LANE2SERDES~TX_ID~",
    "DNXF_STATE~LANE_MAP_DB~SERDES2LANE~",
    "DNXF_STATE~LANE_MAP_DB~SERDES2LANE~RX_ID~",
    "DNXF_STATE~LANE_MAP_DB~SERDES2LANE~TX_ID~",
    "DNXF_STATE~SYNCE~",
    "DNXF_STATE~SYNCE~MASTER_SYNCE_ENABLED~",
    "DNXF_STATE~SYNCE~SLAVE_SYNCE_ENABLED~",
};
#endif 
#undef BSL_LOG_MODULE
