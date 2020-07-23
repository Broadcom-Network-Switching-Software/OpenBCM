
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_qualifier_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_qual_sw_db_t * dnx_field_qual_sw_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_qual_sw_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_qual_sw_db_info_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db_info_valid_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db_info_field_id_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db_info_size_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db_info_bcm_id_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db_info_ref_count_dump(unit, info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db_info_flags_dump(unit, info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_qual_sw_db_info_valid_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_qualifier info valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_qualifier info valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_qual_sw_db/info/valid.txt",
            "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[].valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_QUAL_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info
                , sizeof(*((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_QUAL_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_qual_sw_db[]->((dnx_field_qual_sw_db_t*)sw_state_roots_array[][DNX_FIELD_QUALIFIER_MODULE_ID])->info[].valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_QUAL_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[%s%d].valid: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[i0].valid);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_qual_sw_db_info_field_id_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_fields_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_qualifier info field_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_qualifier info field_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_qual_sw_db/info/field_id.txt",
            "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[].field_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_QUAL_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info
                , sizeof(*((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_QUAL_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_qual_sw_db[]->((dnx_field_qual_sw_db_t*)sw_state_roots_array[][DNX_FIELD_QUALIFIER_MODULE_ID])->info[].field_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_QUAL_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[i0].field_id,
                "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[%s%d].field_id: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[i0].field_id,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_qual_sw_db_info_size_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_qualifier info size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_qualifier info size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_qual_sw_db/info/size.txt",
            "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[].size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_QUAL_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info
                , sizeof(*((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_QUAL_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_qual_sw_db[]->((dnx_field_qual_sw_db_t*)sw_state_roots_array[][DNX_FIELD_QUALIFIER_MODULE_ID])->info[].size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_QUAL_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[%s%d].size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[i0].size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_qual_sw_db_info_bcm_id_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_qualifier info bcm_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_qualifier info bcm_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_qual_sw_db/info/bcm_id.txt",
            "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[].bcm_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_QUAL_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info
                , sizeof(*((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_QUAL_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_qual_sw_db[]->((dnx_field_qual_sw_db_t*)sw_state_roots_array[][DNX_FIELD_QUALIFIER_MODULE_ID])->info[].bcm_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_QUAL_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[%s%d].bcm_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[i0].bcm_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_qual_sw_db_info_ref_count_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_qualifier info ref_count") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_qualifier info ref_count\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_qual_sw_db/info/ref_count.txt",
            "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[].ref_count: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_QUAL_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info
                , sizeof(*((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_QUAL_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_qual_sw_db[]->((dnx_field_qual_sw_db_t*)sw_state_roots_array[][DNX_FIELD_QUALIFIER_MODULE_ID])->info[].ref_count: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_QUAL_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[%s%d].ref_count: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[i0].ref_count);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_qual_sw_db_info_flags_dump(int  unit,  int  info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = info_idx_0, I0 = info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_qual_flags_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_qualifier info flags") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_qualifier info flags\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_qual_sw_db/info/flags.txt",
            "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[].flags: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_QUAL_USER_NOF;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info
                , sizeof(*((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_QUAL_USER_NOF)
        {
            LOG_CLI((BSL_META("dnx_field_qual_sw_db[]->((dnx_field_qual_sw_db_t*)sw_state_roots_array[][DNX_FIELD_QUALIFIER_MODULE_ID])->info[].flags: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_QUAL_USER_NOF == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[i0].flags,
                "dnx_field_qual_sw_db[%d]->","((dnx_field_qual_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_QUALIFIER_MODULE_ID])->","info[%s%d].flags: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_qual_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_QUALIFIER_MODULE_ID])->info[i0].flags,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_qual_sw_db_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_QUAL_SW_DB_INFO_NOF_ENTRIES];
const char* dnx_field_qual_sw_db_layout_str[DNX_FIELD_QUAL_SW_DB_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_QUAL_SW_DB~",
    "DNX_FIELD_QUAL_SW_DB~INFO~",
    "DNX_FIELD_QUAL_SW_DB~INFO~VALID~",
    "DNX_FIELD_QUAL_SW_DB~INFO~FIELD_ID~",
    "DNX_FIELD_QUAL_SW_DB~INFO~SIZE~",
    "DNX_FIELD_QUAL_SW_DB~INFO~BCM_ID~",
    "DNX_FIELD_QUAL_SW_DB~INFO~REF_COUNT~",
    "DNX_FIELD_QUAL_SW_DB~INFO~FLAGS~",
};
#endif 
#undef BSL_LOG_MODULE
