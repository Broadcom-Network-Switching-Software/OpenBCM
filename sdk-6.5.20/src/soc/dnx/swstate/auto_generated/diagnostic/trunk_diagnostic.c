
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
#include <soc/dnx/swstate/auto_generated/diagnostic/trunk_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern trunk_info_t * trunk_db_data[SOC_MAX_NUM_DEVICES];



int
trunk_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(trunk_db_pools_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(trunk_db_trunk_system_port_db_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_dump(int  unit,  int  pools_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_dump(unit, pools_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_psc_dump(unit, pools_idx_0, groups_idx_0, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_psc_profile_id_dump(unit, pools_idx_0, groups_idx_0, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_members_dump(unit, pools_idx_0, groups_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_nof_members_dump(unit, pools_idx_0, groups_idx_0, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_last_member_added_index_dump(unit, pools_idx_0, groups_idx_0, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_in_use_dump(unit, pools_idx_0, groups_idx_0, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_pp_ports_dump(unit, pools_idx_0, groups_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_in_header_type_dump(unit, pools_idx_0, groups_idx_0, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_out_header_type_dump(unit, pools_idx_0, groups_idx_0, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_flags_dump(unit, pools_idx_0, groups_idx_0, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_master_trunk_id_dump(unit, pools_idx_0, groups_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_psc_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups psc") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups psc\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/psc.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].psc: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].psc: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].psc: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].psc: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].psc);

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
trunk_db_pools_groups_psc_profile_id_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups psc_profile_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups psc_profile_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/psc_profile_id.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].psc_profile_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].psc_profile_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].psc_profile_id: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].psc_profile_id: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].psc_profile_id);

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
trunk_db_pools_groups_members_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  int  members_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_members_system_port_dump(unit, pools_idx_0, groups_idx_0, members_idx_0, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_members_flags_dump(unit, pools_idx_0, groups_idx_0, members_idx_0, filters));
    SHR_IF_ERR_EXIT(trunk_db_pools_groups_members_index_dump(unit, pools_idx_0, groups_idx_0, members_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_pools_groups_members_system_port_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  int  members_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    int i2 = members_idx_0, I2 = members_idx_0 + 1, org_i2 = members_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups members system_port") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups members system_port\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/members/system_port.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].members[].system_port: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].members[].system_port: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].members[].system_port: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        TRUNK_MODULE_ID,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members
                        , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members))
                {
                    LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].members[].system_port: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].members[%s%d].system_port: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members[i2].system_port);

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




int
trunk_db_pools_groups_members_flags_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  int  members_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    int i2 = members_idx_0, I2 = members_idx_0 + 1, org_i2 = members_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups members flags") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups members flags\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/members/flags.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].members[].flags: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].members[].flags: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].members[].flags: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        TRUNK_MODULE_ID,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members
                        , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members))
                {
                    LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].members[].flags: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].members[%s%d].flags: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members[i2].flags);

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




int
trunk_db_pools_groups_members_index_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  int  members_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    int i2 = members_idx_0, I2 = members_idx_0 + 1, org_i2 = members_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups members index") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups members index\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/members/index.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].members[].index: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].members[].index: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].members[].index: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        TRUNK_MODULE_ID,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members
                        , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members))
                {
                    LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].members[].index: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].members[%s%d].index: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_int(
                        unit,
                        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].members[i2].index);

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




int
trunk_db_pools_groups_nof_members_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups nof_members") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups nof_members\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/nof_members.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].nof_members: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].nof_members: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].nof_members: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].nof_members: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].nof_members);

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
trunk_db_pools_groups_last_member_added_index_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups last_member_added_index") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups last_member_added_index\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/last_member_added_index.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].last_member_added_index: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].last_member_added_index: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].last_member_added_index: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].last_member_added_index: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].last_member_added_index);

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
trunk_db_pools_groups_in_use_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups in_use") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups in_use\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/in_use.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].in_use: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].in_use: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].in_use: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].in_use: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].in_use);

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
trunk_db_pools_groups_pp_ports_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  int  pp_ports_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    int i2 = pp_ports_idx_0, I2 = pp_ports_idx_0 + 1, org_i2 = pp_ports_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups pp_ports") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups pp_ports\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/pp_ports.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].pp_ports[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].pp_ports[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].pp_ports[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].pp_ports);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        TRUNK_MODULE_ID,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].pp_ports);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].pp_ports
                        , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].pp_ports), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].pp_ports))
                {
                    LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].pp_ports[]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].pp_ports) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].pp_ports[%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].pp_ports[i2]);

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




int
trunk_db_pools_groups_in_header_type_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups in_header_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups in_header_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/in_header_type.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].in_header_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].in_header_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].in_header_type: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].in_header_type: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].in_header_type);

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
trunk_db_pools_groups_out_header_type_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups out_header_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups out_header_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/out_header_type.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].out_header_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].out_header_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].out_header_type: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].out_header_type: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].out_header_type);

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
trunk_db_pools_groups_flags_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups flags") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups flags\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/flags.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].flags: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].flags: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].flags: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].flags: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].flags);

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
trunk_db_pools_groups_master_trunk_id_dump(int  unit,  int  pools_idx_0,  int  groups_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pools_idx_0, I0 = pools_idx_0 + 1;
    int i1 = groups_idx_0, I1 = groups_idx_0 + 1, org_i1 = groups_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk pools groups master_trunk_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk pools groups master_trunk_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/pools/groups/master_trunk_id.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[].groups[].master_trunk_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].master_trunk_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    TRUNK_MODULE_ID,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups
                    , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups))
            {
                LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->pools[].groups[].master_trunk_id: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","pools[%s%d].groups[%s%d].master_trunk_id: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->pools[i0].groups[i1].master_trunk_id);

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
trunk_db_trunk_system_port_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(trunk_db_trunk_system_port_db_trunk_system_port_entries_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_dump(int  unit,  int  trunk_system_port_entries_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_dump(unit, trunk_system_port_entries_idx_0, filters));
    SHR_IF_ERR_EXIT(trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_dump(unit, trunk_system_port_entries_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_trunk_id_dump(int  unit,  int  trunk_system_port_entries_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = trunk_system_port_entries_idx_0, I0 = trunk_system_port_entries_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk trunk_system_port_db trunk_system_port_entries trunk_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk trunk_system_port_db trunk_system_port_entries trunk_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/trunk_system_port_db/trunk_system_port_entries/trunk_id.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","trunk_system_port_db.trunk_system_port_entries[].trunk_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[].trunk_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","trunk_system_port_db.trunk_system_port_entries[%s%d].trunk_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[i0].trunk_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
trunk_db_trunk_system_port_db_trunk_system_port_entries_flags_dump(int  unit,  int  trunk_system_port_entries_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = trunk_system_port_entries_idx_0, I0 = trunk_system_port_entries_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "trunk trunk_system_port_db trunk_system_port_entries flags") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate trunk trunk_system_port_db trunk_system_port_entries flags\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "trunk_db/trunk_system_port_db/trunk_system_port_entries/flags.txt",
            "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","trunk_system_port_db.trunk_system_port_entries[].flags: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                TRUNK_MODULE_ID,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries
                , sizeof(*((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries))
        {
            LOG_CLI((BSL_META("trunk_db[]->((trunk_info_t*)sw_state_roots_array[][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[].flags: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, TRUNK_MODULE_ID, ((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "trunk_db[%d]->","((trunk_info_t*)sw_state_roots_array[%d][TRUNK_MODULE_ID])->","trunk_system_port_db.trunk_system_port_entries[%s%d].flags: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((trunk_info_t*)sw_state_roots_array[unit][TRUNK_MODULE_ID])->trunk_system_port_db.trunk_system_port_entries[i0].flags);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t trunk_db_info[SOC_MAX_NUM_DEVICES][TRUNK_DB_INFO_NOF_ENTRIES];
const char* trunk_db_layout_str[TRUNK_DB_INFO_NOF_ENTRIES] = {
    "TRUNK_DB~",
    "TRUNK_DB~POOLS~",
    "TRUNK_DB~POOLS~GROUPS~",
    "TRUNK_DB~POOLS~GROUPS~PSC~",
    "TRUNK_DB~POOLS~GROUPS~PSC_PROFILE_ID~",
    "TRUNK_DB~POOLS~GROUPS~MEMBERS~",
    "TRUNK_DB~POOLS~GROUPS~MEMBERS~SYSTEM_PORT~",
    "TRUNK_DB~POOLS~GROUPS~MEMBERS~FLAGS~",
    "TRUNK_DB~POOLS~GROUPS~MEMBERS~INDEX~",
    "TRUNK_DB~POOLS~GROUPS~NOF_MEMBERS~",
    "TRUNK_DB~POOLS~GROUPS~LAST_MEMBER_ADDED_INDEX~",
    "TRUNK_DB~POOLS~GROUPS~IN_USE~",
    "TRUNK_DB~POOLS~GROUPS~PP_PORTS~",
    "TRUNK_DB~POOLS~GROUPS~IN_HEADER_TYPE~",
    "TRUNK_DB~POOLS~GROUPS~OUT_HEADER_TYPE~",
    "TRUNK_DB~POOLS~GROUPS~FLAGS~",
    "TRUNK_DB~POOLS~GROUPS~MASTER_TRUNK_ID~",
    "TRUNK_DB~TRUNK_SYSTEM_PORT_DB~",
    "TRUNK_DB~TRUNK_SYSTEM_PORT_DB~TRUNK_SYSTEM_PORT_ENTRIES~",
    "TRUNK_DB~TRUNK_SYSTEM_PORT_DB~TRUNK_SYSTEM_PORT_ENTRIES~TRUNK_ID~",
    "TRUNK_DB~TRUNK_SYSTEM_PORT_DB~TRUNK_SYSTEM_PORT_ENTRIES~FLAGS~",
};
#endif 
#undef BSL_LOG_MODULE
