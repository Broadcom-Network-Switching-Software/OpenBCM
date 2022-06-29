
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_fabric_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_fabric_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_fabric_db_t * dnx_fabric_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_fabric_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_fabric_db_modids_to_group_map_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_links_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_load_balancing_shr_thread_manager_handler_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_system_upgrade_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_cable_swap_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_signal_quality_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_mesh_topology_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_modids_to_group_map_dump(int  unit,  int  modids_to_group_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_fabric_db_modids_to_group_map_nof_modids_dump(unit, modids_to_group_map_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_modids_to_group_map_modids_dump(unit, modids_to_group_map_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_modids_to_group_map_nof_modids_dump(int  unit,  int  modids_to_group_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = modids_to_group_map_idx_0, I0 = modids_to_group_map_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric modids_to_group_map nof_modids") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric modids_to_group_map nof_modids\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/modids_to_group_map/nof_modids.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","modids_to_group_map[].nof_modids: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__NOF_MODIDS,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map
                , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__NOF_MODIDS, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map))
        {
            LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->modids_to_group_map[].nof_modids: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__NOF_MODIDS, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","modids_to_group_map[%s%d].nof_modids: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[i0].nof_modids);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_modids_to_group_map_modids_dump(int  unit,  int  modids_to_group_map_idx_0,  int  modids_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = modids_to_group_map_idx_0, I0 = modids_to_group_map_idx_0 + 1;
    int i1 = modids_idx_0, I1 = modids_idx_0 + 1, org_i1 = modids_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric modids_to_group_map modids") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric modids_to_group_map modids\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/modids_to_group_map/modids.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","modids_to_group_map[].modids[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__MODIDS,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map
                , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__MODIDS, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map))
        {
            LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->modids_to_group_map[].modids[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__MODIDS, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[i0].modids);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__MODIDS,
                    ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[i0].modids);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[i0].modids
                    , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[i0].modids), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__MODIDS, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[i0].modids))
            {
                LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->modids_to_group_map[].modids[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__MODIDS, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[i0].modids) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","modids_to_group_map[%s%d].modids[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->modids_to_group_map[i0].modids[i1]);

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
dnx_fabric_db_links_dump(int  unit,  int  links_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_fabric_db_links_is_link_allowed_dump(unit, links_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_links_is_link_isolated_dump(unit, links_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_links_is_link_allowed_dump(int  unit,  int  links_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = links_idx_0, I0 = links_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric links is_link_allowed") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric links is_link_allowed\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/links/is_link_allowed.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","links[].is_link_allowed: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FABRIC_DB__LINKS__IS_LINK_ALLOWED,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links
                , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__LINKS__IS_LINK_ALLOWED, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links))
        {
            LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->links[].is_link_allowed: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__LINKS__IS_LINK_ALLOWED, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","links[%s%d].is_link_allowed: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links[i0].is_link_allowed);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_links_is_link_isolated_dump(int  unit,  int  links_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = links_idx_0, I0 = links_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric links is_link_isolated") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric links is_link_isolated\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/links/is_link_isolated.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","links[].is_link_isolated: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FABRIC_DB__LINKS__IS_LINK_ISOLATED,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links
                , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__LINKS__IS_LINK_ISOLATED, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links))
        {
            LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->links[].is_link_isolated: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__LINKS__IS_LINK_ISOLATED, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","links[%s%d].is_link_isolated: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->links[i0].is_link_isolated);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_load_balancing_shr_thread_manager_handler_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_system_upgrade_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_fabric_db_system_upgrade_state_0_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_system_upgrade_state_1_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_system_upgrade_state_2_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_system_upgrade_state_0_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric system_upgrade state_0") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric system_upgrade state_0\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/system_upgrade/state_0.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","system_upgrade.state_0: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","system_upgrade.state_0: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_0);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_system_upgrade_state_1_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric system_upgrade state_1") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric system_upgrade state_1\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/system_upgrade/state_1.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","system_upgrade.state_1: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","system_upgrade.state_1: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_1);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_system_upgrade_state_2_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric system_upgrade state_2") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric system_upgrade state_2\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/system_upgrade/state_2.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","system_upgrade.state_2: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","system_upgrade.state_2: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->system_upgrade.state_2);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_cable_swap_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_fabric_db_cable_swap_is_enable_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_cable_swap_is_master_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_cable_swap_is_enable_dump(int  unit,  int  is_enable_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = is_enable_idx_0, I0 = is_enable_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric cable_swap is_enable") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric cable_swap is_enable\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/cable_swap/is_enable.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","cable_swap.is_enable[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__IS_ENABLE,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable
                , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__IS_ENABLE, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable))
        {
            LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__IS_ENABLE, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","cable_swap.is_enable[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_enable[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_cable_swap_is_master_dump(int  unit,  int  is_master_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = is_master_idx_0, I0 = is_master_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric cable_swap is_master") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric cable_swap is_master\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/cable_swap/is_master.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","cable_swap.is_master[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__IS_MASTER,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master
                , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__IS_MASTER, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master))
        {
            LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->cable_swap.is_master[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__IS_MASTER, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","cable_swap.is_master[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->cable_swap.is_master[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_signal_quality_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_fabric_db_signal_quality_lane_state_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_signal_quality_lane_state_dump(int  unit,  int  lane_state_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_fabric_db_signal_quality_lane_state_prev_error_rate_dump(unit, lane_state_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_signal_quality_lane_state_prev_cnt_dump(unit, lane_state_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_fabric_db_signal_quality_lane_state_prev_uncorrectable_cnt_dump(unit, lane_state_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_signal_quality_lane_state_prev_error_rate_dump(int  unit,  int  lane_state_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = lane_state_idx_0, I0 = lane_state_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric signal_quality lane_state prev_error_rate") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric signal_quality lane_state prev_error_rate\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/signal_quality/lane_state/prev_error_rate.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","signal_quality.lane_state[].prev_error_rate: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FABRIC_DB__SIGNAL_QUALITY__LANE_STATE__PREV_ERROR_RATE,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state
                , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__SIGNAL_QUALITY__LANE_STATE__PREV_ERROR_RATE, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state))
        {
            LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state[].prev_error_rate: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__SIGNAL_QUALITY__LANE_STATE__PREV_ERROR_RATE, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","signal_quality.lane_state[%s%d].prev_error_rate: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state[i0].prev_error_rate);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_signal_quality_lane_state_prev_cnt_dump(int  unit,  int  lane_state_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = lane_state_idx_0, I0 = lane_state_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint64") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric signal_quality lane_state prev_cnt") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric signal_quality lane_state prev_cnt\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/signal_quality/lane_state/prev_cnt.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","signal_quality.lane_state[].prev_cnt: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FABRIC_DB__SIGNAL_QUALITY__LANE_STATE__PREV_CNT,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state
                , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__SIGNAL_QUALITY__LANE_STATE__PREV_CNT, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state))
        {
            LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state[].prev_cnt: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__SIGNAL_QUALITY__LANE_STATE__PREV_CNT, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","signal_quality.lane_state[%s%d].prev_cnt: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint64(
                unit,
                &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state[i0].prev_cnt);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_signal_quality_lane_state_prev_uncorrectable_cnt_dump(int  unit,  int  lane_state_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = lane_state_idx_0, I0 = lane_state_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint64") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric signal_quality lane_state prev_uncorrectable_cnt") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric signal_quality lane_state prev_uncorrectable_cnt\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/signal_quality/lane_state/prev_uncorrectable_cnt.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","signal_quality.lane_state[].prev_uncorrectable_cnt: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FABRIC_DB__SIGNAL_QUALITY__LANE_STATE__PREV_UNCORRECTABLE_CNT,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state
                , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__SIGNAL_QUALITY__LANE_STATE__PREV_UNCORRECTABLE_CNT, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state))
        {
            LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state[].prev_uncorrectable_cnt: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__SIGNAL_QUALITY__LANE_STATE__PREV_UNCORRECTABLE_CNT, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","signal_quality.lane_state[%s%d].prev_uncorrectable_cnt: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint64(
                unit,
                &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->signal_quality.lane_state[i0].prev_uncorrectable_cnt);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_mesh_topology_dump(int  unit,  int  mesh_topology_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_fabric_db_mesh_topology_links_dump(unit, mesh_topology_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_fabric_db_mesh_topology_links_dump(int  unit,  int  mesh_topology_idx_0,  int  links_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = mesh_topology_idx_0, I0 = mesh_topology_idx_0 + 1;
    int i1 = links_idx_0, I1 = links_idx_0 + 1, org_i1 = links_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_fabric mesh_topology links") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_fabric mesh_topology links\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_fabric_db/mesh_topology/links.txt",
            "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","mesh_topology[].links[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY__LINKS,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology
                , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY__LINKS, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology))
        {
            LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->mesh_topology[].links[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY__LINKS, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology[i0].links);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY__LINKS,
                    ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology[i0].links);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology[i0].links
                    , sizeof(*((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology[i0].links), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY__LINKS, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology[i0].links))
            {
                LOG_CLI((BSL_META("dnx_fabric_db[]->((dnx_fabric_db_t*)sw_state_roots_array[][DNX_FABRIC_MODULE_ID])->mesh_topology[].links[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY__LINKS, ((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology[i0].links) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_fabric_db[%d]->","((dnx_fabric_db_t*)sw_state_roots_array[%d][DNX_FABRIC_MODULE_ID])->","mesh_topology[%s%d].links[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_fabric_db_t*)sw_state_roots_array[unit][DNX_FABRIC_MODULE_ID])->mesh_topology[i0].links[i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_sw_state_diagnostic_info_t dnx_fabric_db_info[SOC_MAX_NUM_DEVICES][DNX_FABRIC_DB_INFO_NOF_ENTRIES];
const char* dnx_fabric_db_layout_str[DNX_FABRIC_DB_INFO_NOF_ENTRIES] = {
    "DNX_FABRIC_DB~",
    "DNX_FABRIC_DB~MODIDS_TO_GROUP_MAP~",
    "DNX_FABRIC_DB~MODIDS_TO_GROUP_MAP~NOF_MODIDS~",
    "DNX_FABRIC_DB~MODIDS_TO_GROUP_MAP~MODIDS~",
    "DNX_FABRIC_DB~LINKS~",
    "DNX_FABRIC_DB~LINKS~IS_LINK_ALLOWED~",
    "DNX_FABRIC_DB~LINKS~IS_LINK_ISOLATED~",
    "DNX_FABRIC_DB~LOAD_BALANCING_SHR_THREAD_MANAGER_HANDLER~",
    "DNX_FABRIC_DB~SYSTEM_UPGRADE~",
    "DNX_FABRIC_DB~SYSTEM_UPGRADE~STATE_0~",
    "DNX_FABRIC_DB~SYSTEM_UPGRADE~STATE_1~",
    "DNX_FABRIC_DB~SYSTEM_UPGRADE~STATE_2~",
    "DNX_FABRIC_DB~CABLE_SWAP~",
    "DNX_FABRIC_DB~CABLE_SWAP~IS_ENABLE~",
    "DNX_FABRIC_DB~CABLE_SWAP~IS_MASTER~",
    "DNX_FABRIC_DB~SIGNAL_QUALITY~",
    "DNX_FABRIC_DB~SIGNAL_QUALITY~LANE_STATE~",
    "DNX_FABRIC_DB~SIGNAL_QUALITY~LANE_STATE~PREV_ERROR_RATE~",
    "DNX_FABRIC_DB~SIGNAL_QUALITY~LANE_STATE~PREV_CNT~",
    "DNX_FABRIC_DB~SIGNAL_QUALITY~LANE_STATE~PREV_UNCORRECTABLE_CNT~",
    "DNX_FABRIC_DB~MESH_TOPOLOGY~",
    "DNX_FABRIC_DB~MESH_TOPOLOGY~LINKS~",
};
#endif 
#undef BSL_LOG_MODULE
