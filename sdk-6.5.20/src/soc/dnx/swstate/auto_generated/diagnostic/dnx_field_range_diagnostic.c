
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_range_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_range_sw_db_t * dnx_field_range_sw_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_range_sw_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_range_sw_db_ext_l4_ops_range_types_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_range_types_dump(int  unit,  int  ext_l4_ops_range_types_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ext_l4_ops_range_types_idx_0, I0 = ext_l4_ops_range_types_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_range_type_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_range ext_l4_ops_range_types") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_range ext_l4_ops_range_types\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_range_sw_db/ext_l4_ops_range_types.txt",
            "dnx_field_range_sw_db[%d]->","((dnx_field_range_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_RANGE_MODULE_ID])->","ext_l4_ops_range_types[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_range_types
                , sizeof(*((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_range_types), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES)
        {
            LOG_CLI((BSL_META("dnx_field_range_sw_db[]->((dnx_field_range_sw_db_t*)sw_state_roots_array[][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_range_types[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_L4_OPS_NOF_EXT_TYPES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_range_types[i0],
                "dnx_field_range_sw_db[%d]->","((dnx_field_range_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_RANGE_MODULE_ID])->","ext_l4_ops_range_types[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_range_types[i0],
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dump(int  unit,  int  ext_l4_ops_ffc_quals_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_dump(unit, ext_l4_ops_ffc_quals_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_dump(unit, ext_l4_ops_ffc_quals_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_dnx_qual_dump(int  unit,  int  ext_l4_ops_ffc_quals_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ext_l4_ops_ffc_quals_idx_0, I0 = ext_l4_ops_ffc_quals_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_qual_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_range ext_l4_ops_ffc_quals dnx_qual") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_range ext_l4_ops_ffc_quals dnx_qual\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_range_sw_db/ext_l4_ops_ffc_quals/dnx_qual.txt",
            "dnx_field_range_sw_db[%d]->","((dnx_field_range_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_RANGE_MODULE_ID])->","ext_l4_ops_ffc_quals[].dnx_qual: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals
                , sizeof(*((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES)
        {
            LOG_CLI((BSL_META("dnx_field_range_sw_db[]->((dnx_field_range_sw_db_t*)sw_state_roots_array[][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[].dnx_qual: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[i0].dnx_qual,
                "dnx_field_range_sw_db[%d]->","((dnx_field_range_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_RANGE_MODULE_ID])->","ext_l4_ops_ffc_quals[%s%d].dnx_qual: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[i0].dnx_qual,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_dump(int  unit,  int  ext_l4_ops_ffc_quals_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_dump(unit, ext_l4_ops_ffc_quals_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_dump(unit, ext_l4_ops_ffc_quals_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_dump(unit, ext_l4_ops_ffc_quals_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_type_dump(int  unit,  int  ext_l4_ops_ffc_quals_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ext_l4_ops_ffc_quals_idx_0, I0 = ext_l4_ops_ffc_quals_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_input_type_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_range ext_l4_ops_ffc_quals qual_info input_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_range ext_l4_ops_ffc_quals qual_info input_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_range_sw_db/ext_l4_ops_ffc_quals/qual_info/input_type.txt",
            "dnx_field_range_sw_db[%d]->","((dnx_field_range_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_RANGE_MODULE_ID])->","ext_l4_ops_ffc_quals[].qual_info.input_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals
                , sizeof(*((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES)
        {
            LOG_CLI((BSL_META("dnx_field_range_sw_db[]->((dnx_field_range_sw_db_t*)sw_state_roots_array[][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[].qual_info.input_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[i0].qual_info.input_type,
                "dnx_field_range_sw_db[%d]->","((dnx_field_range_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_RANGE_MODULE_ID])->","ext_l4_ops_ffc_quals[%s%d].qual_info.input_type: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[i0].qual_info.input_type,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_input_arg_dump(int  unit,  int  ext_l4_ops_ffc_quals_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ext_l4_ops_ffc_quals_idx_0, I0 = ext_l4_ops_ffc_quals_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_range ext_l4_ops_ffc_quals qual_info input_arg") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_range ext_l4_ops_ffc_quals qual_info input_arg\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_range_sw_db/ext_l4_ops_ffc_quals/qual_info/input_arg.txt",
            "dnx_field_range_sw_db[%d]->","((dnx_field_range_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_RANGE_MODULE_ID])->","ext_l4_ops_ffc_quals[].qual_info.input_arg: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals
                , sizeof(*((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES)
        {
            LOG_CLI((BSL_META("dnx_field_range_sw_db[]->((dnx_field_range_sw_db_t*)sw_state_roots_array[][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[].qual_info.input_arg: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_range_sw_db[%d]->","((dnx_field_range_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_RANGE_MODULE_ID])->","ext_l4_ops_ffc_quals[%s%d].qual_info.input_arg: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[i0].qual_info.input_arg);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_range_sw_db_ext_l4_ops_ffc_quals_qual_info_offset_dump(int  unit,  int  ext_l4_ops_ffc_quals_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = ext_l4_ops_ffc_quals_idx_0, I0 = ext_l4_ops_ffc_quals_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_range ext_l4_ops_ffc_quals qual_info offset") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_range ext_l4_ops_ffc_quals qual_info offset\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_range_sw_db/ext_l4_ops_ffc_quals/qual_info/offset.txt",
            "dnx_field_range_sw_db[%d]->","((dnx_field_range_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_RANGE_MODULE_ID])->","ext_l4_ops_ffc_quals[].qual_info.offset: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals
                , sizeof(*((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES)
        {
            LOG_CLI((BSL_META("dnx_field_range_sw_db[]->((dnx_field_range_sw_db_t*)sw_state_roots_array[][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[].qual_info.offset: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_DATA_MAX_FIELD_L4_OPS_NOF_CONFIGURABLE_RANGES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_range_sw_db[%d]->","((dnx_field_range_sw_db_t*)sw_state_roots_array[%d][DNX_FIELD_RANGE_MODULE_ID])->","ext_l4_ops_ffc_quals[%s%d].qual_info.offset: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_field_range_sw_db_t*)sw_state_roots_array[unit][DNX_FIELD_RANGE_MODULE_ID])->ext_l4_ops_ffc_quals[i0].qual_info.offset);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_range_sw_db_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_RANGE_SW_DB_INFO_NOF_ENTRIES];
const char* dnx_field_range_sw_db_layout_str[DNX_FIELD_RANGE_SW_DB_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_RANGE_SW_DB~",
    "DNX_FIELD_RANGE_SW_DB~EXT_L4_OPS_RANGE_TYPES~",
    "DNX_FIELD_RANGE_SW_DB~EXT_L4_OPS_FFC_QUALS~",
    "DNX_FIELD_RANGE_SW_DB~EXT_L4_OPS_FFC_QUALS~DNX_QUAL~",
    "DNX_FIELD_RANGE_SW_DB~EXT_L4_OPS_FFC_QUALS~QUAL_INFO~",
    "DNX_FIELD_RANGE_SW_DB~EXT_L4_OPS_FFC_QUALS~QUAL_INFO~INPUT_TYPE~",
    "DNX_FIELD_RANGE_SW_DB~EXT_L4_OPS_FFC_QUALS~QUAL_INFO~INPUT_ARG~",
    "DNX_FIELD_RANGE_SW_DB~EXT_L4_OPS_FFC_QUALS~QUAL_INFO~OFFSET~",
};
#endif 
#undef BSL_LOG_MODULE
