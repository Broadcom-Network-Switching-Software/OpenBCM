
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/instru_counters_diagnostic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/swstate/auto_generated/layout/instru_counters_layout.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern instru_sync_counters_sw_state_t * instru_sync_counters_data[SOC_MAX_NUM_DEVICES];



int
instru_sync_counters_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(instru_sync_counters_icgm_countes_dump(unit, filters));
    SHR_IF_ERR_EXIT(instru_sync_counters_nif_countes_dump(unit, filters));
    SHR_IF_ERR_EXIT(instru_sync_counters_ports_for_pfc_counters_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(instru_sync_counters_nof_ports_for_pfc_counters_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_icgm_countes_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(instru_sync_counters_icgm_countes_sync_counters_started_dump(unit, filters));
    SHR_IF_ERR_EXIT(instru_sync_counters_icgm_countes_tod_1588_timestamp_start_dump(unit, filters));
    SHR_IF_ERR_EXIT(instru_sync_counters_icgm_countes_per_core_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_icgm_countes_sync_counters_started_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "instru_counters icgm_countes sync_counters_started") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate instru_counters icgm_countes sync_counters_started\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "instru_sync_counters/icgm_countes/sync_counters_started.txt",
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","icgm_countes.sync_counters_started: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","icgm_countes.sync_counters_started: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.sync_counters_started);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_icgm_countes_tod_1588_timestamp_start_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint64") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "instru_counters icgm_countes tod_1588_timestamp_start") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate instru_counters icgm_countes tod_1588_timestamp_start\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "instru_sync_counters/icgm_countes/tod_1588_timestamp_start.txt",
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","icgm_countes.tod_1588_timestamp_start: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","icgm_countes.tod_1588_timestamp_start: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint64(
            unit,
            &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.tod_1588_timestamp_start);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_icgm_countes_per_core_info_dump(int  unit,  int  per_core_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_dump(unit, per_core_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_icgm_countes_per_core_info_record_last_read_nof_dump(int  unit,  int  per_core_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = per_core_info_idx_0, I0 = per_core_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "instru_counters icgm_countes per_core_info record_last_read_nof") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate instru_counters icgm_countes per_core_info record_last_read_nof\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "instru_sync_counters/icgm_countes/per_core_info/record_last_read_nof.txt",
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","icgm_countes.per_core_info[].record_last_read_nof: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES__PER_CORE_INFO__RECORD_LAST_READ_NOF,
                ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info
                , sizeof(*((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES__PER_CORE_INFO__RECORD_LAST_READ_NOF, ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info))
        {
            LOG_CLI((BSL_META("instru_sync_counters[]->((instru_sync_counters_sw_state_t*)sw_state_roots_array[][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info[].record_last_read_nof: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES__PER_CORE_INFO__RECORD_LAST_READ_NOF, ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","icgm_countes.per_core_info[%s%d].record_last_read_nof: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->icgm_countes.per_core_info[i0].record_last_read_nof);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_nif_countes_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(instru_sync_counters_nif_countes_sync_counters_started_dump(unit, filters));
    SHR_IF_ERR_EXIT(instru_sync_counters_nif_countes_tod_1588_timestamp_start_dump(unit, filters));
    SHR_IF_ERR_EXIT(instru_sync_counters_nif_countes_per_ethu_info_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_nif_countes_sync_counters_started_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "instru_counters nif_countes sync_counters_started") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate instru_counters nif_countes sync_counters_started\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "instru_sync_counters/nif_countes/sync_counters_started.txt",
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","nif_countes.sync_counters_started: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","nif_countes.sync_counters_started: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint8(
            unit,
            &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.sync_counters_started);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_nif_countes_tod_1588_timestamp_start_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint64") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "instru_counters nif_countes tod_1588_timestamp_start") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate instru_counters nif_countes tod_1588_timestamp_start\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "instru_sync_counters/nif_countes/tod_1588_timestamp_start.txt",
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","nif_countes.tod_1588_timestamp_start: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","nif_countes.tod_1588_timestamp_start: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint64(
            unit,
            &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.tod_1588_timestamp_start);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_nif_countes_per_ethu_info_dump(int  unit,  int  per_ethu_info_idx_0,  int  per_ethu_info_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_dump(unit, per_ethu_info_idx_0,per_ethu_info_idx_1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_nif_countes_per_ethu_info_record_last_read_nof_dump(int  unit,  int  per_ethu_info_idx_0,  int  per_ethu_info_idx_1,  int  record_last_read_nof_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = per_ethu_info_idx_0, I0 = per_ethu_info_idx_0 + 1;
    int i1 = per_ethu_info_idx_1, I1 = per_ethu_info_idx_1 + 1, org_i1 = per_ethu_info_idx_1;
    int i2 = record_last_read_nof_idx_0, I2 = record_last_read_nof_idx_0 + 1, org_i2 = record_last_read_nof_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "instru_counters nif_countes per_ethu_info record_last_read_nof") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate instru_counters nif_countes per_ethu_info record_last_read_nof\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "instru_sync_counters/nif_countes/per_ethu_info/record_last_read_nof.txt",
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","nif_countes.per_ethu_info[][].record_last_read_nof[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__RECORD_LAST_READ_NOF,
                ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info
                , sizeof(*((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__RECORD_LAST_READ_NOF, ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info))
        {
            LOG_CLI((BSL_META("instru_sync_counters[]->((instru_sync_counters_sw_state_t*)sw_state_roots_array[][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[][].record_last_read_nof[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__RECORD_LAST_READ_NOF, ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__RECORD_LAST_READ_NOF,
                    ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0]
                    , sizeof(*((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__RECORD_LAST_READ_NOF, ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0]))
            {
                LOG_CLI((BSL_META("instru_sync_counters[]->((instru_sync_counters_sw_state_t*)sw_state_roots_array[][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[][].record_last_read_nof[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__RECORD_LAST_READ_NOF, ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0][i1].record_last_read_nof);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__RECORD_LAST_READ_NOF,
                        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0][i1].record_last_read_nof);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0][i1].record_last_read_nof
                        , sizeof(*((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0][i1].record_last_read_nof), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__RECORD_LAST_READ_NOF, ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0][i1].record_last_read_nof))
                {
                    LOG_CLI((BSL_META("instru_sync_counters[]->((instru_sync_counters_sw_state_t*)sw_state_roots_array[][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[][].record_last_read_nof[]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__RECORD_LAST_READ_NOF, ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0][i1].record_last_read_nof) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","nif_countes.per_ethu_info[%s%d][%s%d].record_last_read_nof[%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nif_countes.per_ethu_info[i0][i1].record_last_read_nof[i2]);

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
instru_sync_counters_ports_for_pfc_counters_dump(int  unit,  int  ports_for_pfc_counters_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(instru_sync_counters_ports_for_pfc_counters_port_dump(unit, ports_for_pfc_counters_idx_0, filters));
    SHR_IF_ERR_EXIT(instru_sync_counters_ports_for_pfc_counters_tc_bmp_dump(unit, ports_for_pfc_counters_idx_0, filters));
    SHR_IF_ERR_EXIT(instru_sync_counters_ports_for_pfc_counters_pfc_pm_idx_dump(unit, ports_for_pfc_counters_idx_0, filters));
    SHR_IF_ERR_EXIT(instru_sync_counters_ports_for_pfc_counters_pfc_port_idx_base_dump(unit, ports_for_pfc_counters_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_ports_for_pfc_counters_port_dump(int  unit,  int  ports_for_pfc_counters_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ports_for_pfc_counters_idx_0, I0 = ports_for_pfc_counters_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "instru_counters ports_for_pfc_counters port") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate instru_counters ports_for_pfc_counters port\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "instru_sync_counters/ports_for_pfc_counters/port.txt",
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","ports_for_pfc_counters[].port: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters
                , sizeof(*((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC)
        {
            LOG_CLI((BSL_META("instru_sync_counters[]->((instru_sync_counters_sw_state_t*)sw_state_roots_array[][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters[].port: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","ports_for_pfc_counters[%s%d].port: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters[i0].port);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_ports_for_pfc_counters_tc_bmp_dump(int  unit,  int  ports_for_pfc_counters_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ports_for_pfc_counters_idx_0, I0 = ports_for_pfc_counters_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "instru_counters ports_for_pfc_counters tc_bmp") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate instru_counters ports_for_pfc_counters tc_bmp\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "instru_sync_counters/ports_for_pfc_counters/tc_bmp.txt",
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","ports_for_pfc_counters[].tc_bmp: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters
                , sizeof(*((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC)
        {
            LOG_CLI((BSL_META("instru_sync_counters[]->((instru_sync_counters_sw_state_t*)sw_state_roots_array[][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters[].tc_bmp: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","ports_for_pfc_counters[%s%d].tc_bmp: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters[i0].tc_bmp);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_ports_for_pfc_counters_pfc_pm_idx_dump(int  unit,  int  ports_for_pfc_counters_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ports_for_pfc_counters_idx_0, I0 = ports_for_pfc_counters_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "instru_counters ports_for_pfc_counters pfc_pm_idx") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate instru_counters ports_for_pfc_counters pfc_pm_idx\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "instru_sync_counters/ports_for_pfc_counters/pfc_pm_idx.txt",
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","ports_for_pfc_counters[].pfc_pm_idx: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters
                , sizeof(*((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC)
        {
            LOG_CLI((BSL_META("instru_sync_counters[]->((instru_sync_counters_sw_state_t*)sw_state_roots_array[][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters[].pfc_pm_idx: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","ports_for_pfc_counters[%s%d].pfc_pm_idx: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters[i0].pfc_pm_idx);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_ports_for_pfc_counters_pfc_port_idx_base_dump(int  unit,  int  ports_for_pfc_counters_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ports_for_pfc_counters_idx_0, I0 = ports_for_pfc_counters_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "instru_counters ports_for_pfc_counters pfc_port_idx_base") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate instru_counters ports_for_pfc_counters pfc_port_idx_base\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "instru_sync_counters/ports_for_pfc_counters/pfc_port_idx_base.txt",
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","ports_for_pfc_counters[].pfc_port_idx_base: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters
                , sizeof(*((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC)
        {
            LOG_CLI((BSL_META("instru_sync_counters[]->((instru_sync_counters_sw_state_t*)sw_state_roots_array[][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters[].pfc_port_idx_base: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_INSTRU_SYNCED_COUNTERS_NOF_PORTS_FOR_PFC == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","ports_for_pfc_counters[%s%d].pfc_port_idx_base: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->ports_for_pfc_counters[i0].pfc_port_idx_base);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}



int
instru_sync_counters_nof_ports_for_pfc_counters_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "instru_counters nof_ports_for_pfc_counters") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate instru_counters nof_ports_for_pfc_counters\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "instru_sync_counters/nof_ports_for_pfc_counters.txt",
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","nof_ports_for_pfc_counters: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "instru_sync_counters[%d]->","((instru_sync_counters_sw_state_t*)sw_state_roots_array[%d][INSTRU_COUNTERS_MODULE_ID])->","nof_ports_for_pfc_counters: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((instru_sync_counters_sw_state_t*)sw_state_roots_array[unit][INSTRU_COUNTERS_MODULE_ID])->nof_ports_for_pfc_counters);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_sw_state_diagnostic_info_t instru_sync_counters_info[SOC_MAX_NUM_DEVICES][INSTRU_SYNC_COUNTERS_INFO_NOF_ENTRIES];
const char* instru_sync_counters_layout_str[INSTRU_SYNC_COUNTERS_INFO_NOF_ENTRIES] = {
    "INSTRU_SYNC_COUNTERS~",
    "INSTRU_SYNC_COUNTERS~ICGM_COUNTES~",
    "INSTRU_SYNC_COUNTERS~ICGM_COUNTES~SYNC_COUNTERS_STARTED~",
    "INSTRU_SYNC_COUNTERS~ICGM_COUNTES~TOD_1588_TIMESTAMP_START~",
    "INSTRU_SYNC_COUNTERS~ICGM_COUNTES~PER_CORE_INFO~",
    "INSTRU_SYNC_COUNTERS~ICGM_COUNTES~PER_CORE_INFO~RECORD_LAST_READ_NOF~",
    "INSTRU_SYNC_COUNTERS~NIF_COUNTES~",
    "INSTRU_SYNC_COUNTERS~NIF_COUNTES~SYNC_COUNTERS_STARTED~",
    "INSTRU_SYNC_COUNTERS~NIF_COUNTES~TOD_1588_TIMESTAMP_START~",
    "INSTRU_SYNC_COUNTERS~NIF_COUNTES~PER_ETHU_INFO~",
    "INSTRU_SYNC_COUNTERS~NIF_COUNTES~PER_ETHU_INFO~RECORD_LAST_READ_NOF~",
    "INSTRU_SYNC_COUNTERS~PORTS_FOR_PFC_COUNTERS~",
    "INSTRU_SYNC_COUNTERS~PORTS_FOR_PFC_COUNTERS~PORT~",
    "INSTRU_SYNC_COUNTERS~PORTS_FOR_PFC_COUNTERS~TC_BMP~",
    "INSTRU_SYNC_COUNTERS~PORTS_FOR_PFC_COUNTERS~PFC_PM_IDX~",
    "INSTRU_SYNC_COUNTERS~PORTS_FOR_PFC_COUNTERS~PFC_PORT_IDX_BASE~",
    "INSTRU_SYNC_COUNTERS~NOF_PORTS_FOR_PFC_COUNTERS~",
};
#endif 
#undef BSL_LOG_MODULE
