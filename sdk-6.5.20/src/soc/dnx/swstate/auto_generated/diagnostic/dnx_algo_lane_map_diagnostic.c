
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_lane_map_diagnostic.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_algo_lane_map_db_t * dnx_algo_lane_map_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_algo_lane_map_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_db_lane_map_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_dump(int  unit,  int  lane_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_db_lane_map_map_size_dump(unit, lane_map_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_db_lane_map_lane2serdes_dump(unit, lane_map_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_db_lane_map_serdes2lane_dump(unit, lane_map_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_map_size_dump(int  unit,  int  lane_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = lane_map_idx_0, I0 = lane_map_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_lane_map lane_map map_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_lane_map lane_map map_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_lane_map_db/lane_map/map_size.txt",
            "dnx_algo_lane_map_db[%d]->","((dnx_algo_lane_map_db_t*)sw_state_roots_array[%d][DNX_ALGO_LANE_MAP_MODULE_ID])->","lane_map[].map_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_ALGO_NOF_LANE_MAP_TYPES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map
                , sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_ALGO_NOF_LANE_MAP_TYPES)
        {
            LOG_CLI((BSL_META("dnx_algo_lane_map_db[]->((dnx_algo_lane_map_db_t*)sw_state_roots_array[][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[].map_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_ALGO_NOF_LANE_MAP_TYPES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_algo_lane_map_db[%d]->","((dnx_algo_lane_map_db_t*)sw_state_roots_array[%d][DNX_ALGO_LANE_MAP_MODULE_ID])->","lane_map[%s%d].map_size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].map_size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_lane2serdes_dump(int  unit,  int  lane_map_idx_0,  int  lane2serdes_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_dump(unit, lane_map_idx_0, lane2serdes_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_dump(unit, lane_map_idx_0, lane2serdes_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_dump(int  unit,  int  lane_map_idx_0,  int  lane2serdes_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = lane_map_idx_0, I0 = lane_map_idx_0 + 1;
    int i1 = lane2serdes_idx_0, I1 = lane2serdes_idx_0 + 1, org_i1 = lane2serdes_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_lane_map lane_map lane2serdes rx_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_lane_map lane_map lane2serdes rx_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_lane_map_db/lane_map/lane2serdes/rx_id.txt",
            "dnx_algo_lane_map_db[%d]->","((dnx_algo_lane_map_db_t*)sw_state_roots_array[%d][DNX_ALGO_LANE_MAP_MODULE_ID])->","lane_map[].lane2serdes[].rx_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_ALGO_NOF_LANE_MAP_TYPES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map
                , sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_ALGO_NOF_LANE_MAP_TYPES)
        {
            LOG_CLI((BSL_META("dnx_algo_lane_map_db[]->((dnx_algo_lane_map_db_t*)sw_state_roots_array[][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[].lane2serdes[].rx_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_ALGO_NOF_LANE_MAP_TYPES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_ALGO_LANE_MAP_MODULE_ID,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes
                    , sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_ALGO_LANE_MAP_MODULE_ID, ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes))
            {
                LOG_CLI((BSL_META("dnx_algo_lane_map_db[]->((dnx_algo_lane_map_db_t*)sw_state_roots_array[][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[].lane2serdes[].rx_id: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_ALGO_LANE_MAP_MODULE_ID, ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_algo_lane_map_db[%d]->","((dnx_algo_lane_map_db_t*)sw_state_roots_array[%d][DNX_ALGO_LANE_MAP_MODULE_ID])->","lane_map[%s%d].lane2serdes[%s%d].rx_id: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes[i1].rx_id);

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
dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_dump(int  unit,  int  lane_map_idx_0,  int  lane2serdes_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = lane_map_idx_0, I0 = lane_map_idx_0 + 1;
    int i1 = lane2serdes_idx_0, I1 = lane2serdes_idx_0 + 1, org_i1 = lane2serdes_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_lane_map lane_map lane2serdes tx_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_lane_map lane_map lane2serdes tx_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_lane_map_db/lane_map/lane2serdes/tx_id.txt",
            "dnx_algo_lane_map_db[%d]->","((dnx_algo_lane_map_db_t*)sw_state_roots_array[%d][DNX_ALGO_LANE_MAP_MODULE_ID])->","lane_map[].lane2serdes[].tx_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_ALGO_NOF_LANE_MAP_TYPES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map
                , sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_ALGO_NOF_LANE_MAP_TYPES)
        {
            LOG_CLI((BSL_META("dnx_algo_lane_map_db[]->((dnx_algo_lane_map_db_t*)sw_state_roots_array[][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[].lane2serdes[].tx_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_ALGO_NOF_LANE_MAP_TYPES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_ALGO_LANE_MAP_MODULE_ID,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes
                    , sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_ALGO_LANE_MAP_MODULE_ID, ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes))
            {
                LOG_CLI((BSL_META("dnx_algo_lane_map_db[]->((dnx_algo_lane_map_db_t*)sw_state_roots_array[][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[].lane2serdes[].tx_id: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_ALGO_LANE_MAP_MODULE_ID, ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_algo_lane_map_db[%d]->","((dnx_algo_lane_map_db_t*)sw_state_roots_array[%d][DNX_ALGO_LANE_MAP_MODULE_ID])->","lane_map[%s%d].lane2serdes[%s%d].tx_id: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].lane2serdes[i1].tx_id);

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
dnx_algo_lane_map_db_lane_map_serdes2lane_dump(int  unit,  int  lane_map_idx_0,  int  serdes2lane_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_dump(unit, lane_map_idx_0, serdes2lane_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_dump(unit, lane_map_idx_0, serdes2lane_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_dump(int  unit,  int  lane_map_idx_0,  int  serdes2lane_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = lane_map_idx_0, I0 = lane_map_idx_0 + 1;
    int i1 = serdes2lane_idx_0, I1 = serdes2lane_idx_0 + 1, org_i1 = serdes2lane_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_lane_map lane_map serdes2lane rx_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_lane_map lane_map serdes2lane rx_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_lane_map_db/lane_map/serdes2lane/rx_id.txt",
            "dnx_algo_lane_map_db[%d]->","((dnx_algo_lane_map_db_t*)sw_state_roots_array[%d][DNX_ALGO_LANE_MAP_MODULE_ID])->","lane_map[].serdes2lane[].rx_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_ALGO_NOF_LANE_MAP_TYPES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map
                , sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_ALGO_NOF_LANE_MAP_TYPES)
        {
            LOG_CLI((BSL_META("dnx_algo_lane_map_db[]->((dnx_algo_lane_map_db_t*)sw_state_roots_array[][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[].serdes2lane[].rx_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_ALGO_NOF_LANE_MAP_TYPES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_ALGO_LANE_MAP_MODULE_ID,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane
                    , sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_ALGO_LANE_MAP_MODULE_ID, ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane))
            {
                LOG_CLI((BSL_META("dnx_algo_lane_map_db[]->((dnx_algo_lane_map_db_t*)sw_state_roots_array[][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[].serdes2lane[].rx_id: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_ALGO_LANE_MAP_MODULE_ID, ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_algo_lane_map_db[%d]->","((dnx_algo_lane_map_db_t*)sw_state_roots_array[%d][DNX_ALGO_LANE_MAP_MODULE_ID])->","lane_map[%s%d].serdes2lane[%s%d].rx_id: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane[i1].rx_id);

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
dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_dump(int  unit,  int  lane_map_idx_0,  int  serdes2lane_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = lane_map_idx_0, I0 = lane_map_idx_0 + 1;
    int i1 = serdes2lane_idx_0, I1 = serdes2lane_idx_0 + 1, org_i1 = serdes2lane_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_algo_lane_map lane_map serdes2lane tx_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_algo_lane_map lane_map serdes2lane tx_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_algo_lane_map_db/lane_map/serdes2lane/tx_id.txt",
            "dnx_algo_lane_map_db[%d]->","((dnx_algo_lane_map_db_t*)sw_state_roots_array[%d][DNX_ALGO_LANE_MAP_MODULE_ID])->","lane_map[].serdes2lane[].tx_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_ALGO_NOF_LANE_MAP_TYPES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map
                , sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_ALGO_NOF_LANE_MAP_TYPES)
        {
            LOG_CLI((BSL_META("dnx_algo_lane_map_db[]->((dnx_algo_lane_map_db_t*)sw_state_roots_array[][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[].serdes2lane[].tx_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_ALGO_NOF_LANE_MAP_TYPES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_ALGO_LANE_MAP_MODULE_ID,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane
                    , sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_ALGO_LANE_MAP_MODULE_ID, ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane))
            {
                LOG_CLI((BSL_META("dnx_algo_lane_map_db[]->((dnx_algo_lane_map_db_t*)sw_state_roots_array[][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[].serdes2lane[].tx_id: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_ALGO_LANE_MAP_MODULE_ID, ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_algo_lane_map_db[%d]->","((dnx_algo_lane_map_db_t*)sw_state_roots_array[%d][DNX_ALGO_LANE_MAP_MODULE_ID])->","lane_map[%s%d].serdes2lane[%s%d].tx_id: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[i0].serdes2lane[i1].tx_id);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_algo_lane_map_db_info[SOC_MAX_NUM_DEVICES][DNX_ALGO_LANE_MAP_DB_INFO_NOF_ENTRIES];
const char* dnx_algo_lane_map_db_layout_str[DNX_ALGO_LANE_MAP_DB_INFO_NOF_ENTRIES] = {
    "DNX_ALGO_LANE_MAP_DB~",
    "DNX_ALGO_LANE_MAP_DB~LANE_MAP~",
    "DNX_ALGO_LANE_MAP_DB~LANE_MAP~MAP_SIZE~",
    "DNX_ALGO_LANE_MAP_DB~LANE_MAP~LANE2SERDES~",
    "DNX_ALGO_LANE_MAP_DB~LANE_MAP~LANE2SERDES~RX_ID~",
    "DNX_ALGO_LANE_MAP_DB~LANE_MAP~LANE2SERDES~TX_ID~",
    "DNX_ALGO_LANE_MAP_DB~LANE_MAP~SERDES2LANE~",
    "DNX_ALGO_LANE_MAP_DB~LANE_MAP~SERDES2LANE~RX_ID~",
    "DNX_ALGO_LANE_MAP_DB~LANE_MAP~SERDES2LANE~TX_ID~",
};
#endif 
#undef BSL_LOG_MODULE
