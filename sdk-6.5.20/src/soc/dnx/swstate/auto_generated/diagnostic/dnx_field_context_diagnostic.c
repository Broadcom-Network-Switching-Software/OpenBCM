
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_context_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_context_sw_t * dnx_field_context_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_context_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf2_info_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_state_table_info_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_name_dump(unit, context_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_dump(unit, context_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_dump(unit, context_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_dump(unit, context_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_mode_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_context_compare_mode_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/mode.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.mode: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.mode,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.mode: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.mode,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_is_set_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 is_set") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 is_set\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/is_set.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.is_set: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.is_set: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.is_set: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.is_set);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_dump(unit, context_info_idx_0, key_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_dump(unit, context_info_idx_0, key_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_dump(unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_dump(unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_dump(unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_qual_type_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = key_qual_map_idx_0, I2 = key_qual_map_idx_0 + 1, org_i2 = key_qual_map_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_qual_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_template key_qual_map qual_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_template key_qual_map qual_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/key_template/key_qual_map/qual_type.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].qual_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].qual_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].qual_type: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_template.key_qual_map
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_template.key_qual_map), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].qual_type: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_template.key_qual_map[i2].qual_type,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].key_template.key_qual_map[%s%d].qual_type: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_OPAQUE_FILE(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_template.key_qual_map[i2].qual_type,
                            "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_lsb_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = key_qual_map_idx_0, I2 = key_qual_map_idx_0 + 1, org_i2 = key_qual_map_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint16") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_template key_qual_map lsb") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_template key_qual_map lsb\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/key_template/key_qual_map/lsb.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].lsb: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].lsb: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].lsb: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_template.key_qual_map
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_template.key_qual_map), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].lsb: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].key_template.key_qual_map[%s%d].lsb: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint16(
                        unit,
                        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_template.key_qual_map[i2].lsb);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_qual_map_size_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = key_qual_map_idx_0, I2 = key_qual_map_idx_0 + 1, org_i2 = key_qual_map_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_template key_qual_map size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_template key_qual_map size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/key_template/key_qual_map/size.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].size: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_template.key_qual_map
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_template.key_qual_map), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_qual_map[].size: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].key_template.key_qual_map[%s%d].size: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint8(
                        unit,
                        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_template.key_qual_map[i2].size);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_template_key_size_in_bits_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint16") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_template key_size_in_bits") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_template key_size_in_bits\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/key_template/key_size_in_bits.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_size_in_bits: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_size_in_bits: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_template.key_size_in_bits: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].key_template.key_size_in_bits: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint16(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_template.key_size_in_bits);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_dump(unit, context_info_idx_0, key_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_id_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  id_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = id_idx_0, I2 = id_idx_0 + 1, org_i2 = id_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_enum_value_field_field_key_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_id id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_id id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/key_id/id.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.id[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.id[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.id[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_id.id
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_id.id), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.id[]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_id.id[i2],
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].key_id.id[%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_OPAQUE_FILE(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_id.id[i2],
                            "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_full_key_allocated_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_id full_key_allocated") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_id full_key_allocated\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/key_id/full_key_allocated.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.full_key_allocated: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.full_key_allocated: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.full_key_allocated: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].key_id.full_key_allocated: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_id.full_key_allocated);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_valid_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_id bit_range bit_range_valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_id bit_range bit_range_valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/key_id/bit_range/bit_range_valid.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.bit_range.bit_range_valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.bit_range.bit_range_valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.bit_range.bit_range_valid: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].key_id.bit_range.bit_range_valid: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_id.bit_range.bit_range_valid);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_offset_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_id bit_range bit_range_offset") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_id bit_range bit_range_offset\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/key_id/bit_range/bit_range_offset.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.bit_range.bit_range_offset: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.bit_range.bit_range_offset: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.bit_range.bit_range_offset: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].key_id.bit_range.bit_range_offset: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_id.bit_range.bit_range_offset);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_key_id_bit_range_bit_range_size_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_id bit_range bit_range_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info key_id bit_range bit_range_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/key_id/bit_range/bit_range_size.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.bit_range.bit_range_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.bit_range.bit_range_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].key_id.bit_range.bit_range_size: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].key_id.bit_range.bit_range_size: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].key_id.bit_range.bit_range_size);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_dump(unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_dump(unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_dump(unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_type_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = attach_info_idx_0, I2 = attach_info_idx_0 + 1, org_i2 = attach_info_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_input_type_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info attach_info input_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info attach_info input_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/attach_info/input_type.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].input_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].input_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].input_type: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].attach_info
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].attach_info), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].input_type: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].attach_info[i2].input_type,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].attach_info[%s%d].input_type: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_OPAQUE_FILE(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].attach_info[i2].input_type,
                            "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_input_arg_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = attach_info_idx_0, I2 = attach_info_idx_0 + 1, org_i2 = attach_info_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info attach_info input_arg") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info attach_info input_arg\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/attach_info/input_arg.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].input_arg: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].input_arg: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].input_arg: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].attach_info
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].attach_info), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].input_arg: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].attach_info[%s%d].input_arg: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_int(
                        unit,
                        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].attach_info[i2].input_arg);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_1_key_info_attach_info_offset_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = attach_info_idx_0, I2 = attach_info_idx_0 + 1, org_i2 = attach_info_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info attach_info offset") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_1 key_info attach_info offset\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_1/key_info/attach_info/offset.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].offset: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].offset: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].offset: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].attach_info
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].attach_info), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_1.key_info[].attach_info[].offset: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_1.key_info[%s%d].attach_info[%s%d].offset: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_int(
                        unit,
                        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_1.key_info[i1].attach_info[i2].offset);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_dump(unit, context_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_mode_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_context_compare_mode_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/mode.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.mode: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.mode,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.mode: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.mode,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_is_set_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 is_set") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 is_set\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/is_set.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.is_set: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.is_set: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.is_set: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.is_set);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_dump(unit, context_info_idx_0, key_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_dump(unit, context_info_idx_0, key_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_dump(unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_dump(unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_dump(unit, context_info_idx_0, key_info_idx_0, key_qual_map_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_qual_type_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = key_qual_map_idx_0, I2 = key_qual_map_idx_0 + 1, org_i2 = key_qual_map_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_qual_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_template key_qual_map qual_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_template key_qual_map qual_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/key_template/key_qual_map/qual_type.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].qual_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].qual_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].qual_type: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_template.key_qual_map
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_template.key_qual_map), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].qual_type: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_template.key_qual_map[i2].qual_type,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].key_template.key_qual_map[%s%d].qual_type: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_OPAQUE_FILE(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_template.key_qual_map[i2].qual_type,
                            "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_lsb_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = key_qual_map_idx_0, I2 = key_qual_map_idx_0 + 1, org_i2 = key_qual_map_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint16") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_template key_qual_map lsb") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_template key_qual_map lsb\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/key_template/key_qual_map/lsb.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].lsb: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].lsb: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].lsb: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_template.key_qual_map
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_template.key_qual_map), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].lsb: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].key_template.key_qual_map[%s%d].lsb: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint16(
                        unit,
                        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_template.key_qual_map[i2].lsb);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_qual_map_size_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = key_qual_map_idx_0, I2 = key_qual_map_idx_0 + 1, org_i2 = key_qual_map_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_template key_qual_map size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_template key_qual_map size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/key_template/key_qual_map/size.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].size: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_template.key_qual_map
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_template.key_qual_map), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_qual_map[].size: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].key_template.key_qual_map[%s%d].size: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint8(
                        unit,
                        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_template.key_qual_map[i2].size);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_template_key_size_in_bits_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint16") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_template key_size_in_bits") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_template key_size_in_bits\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/key_template/key_size_in_bits.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_size_in_bits: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_size_in_bits: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_template.key_size_in_bits: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].key_template.key_size_in_bits: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint16(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_template.key_size_in_bits);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_dump(unit, context_info_idx_0, key_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_id_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  id_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = id_idx_0, I2 = id_idx_0 + 1, org_i2 = id_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_enum_value_field_field_key_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_id id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_id id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/key_id/id.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.id[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.id[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.id[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_id.id
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_id.id), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.id[]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_id.id[i2],
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].key_id.id[%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_OPAQUE_FILE(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_id.id[i2],
                            "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_full_key_allocated_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_id full_key_allocated") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_id full_key_allocated\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/key_id/full_key_allocated.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.full_key_allocated: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.full_key_allocated: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.full_key_allocated: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].key_id.full_key_allocated: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_id.full_key_allocated);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_dump(unit, context_info_idx_0, key_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_valid_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_id bit_range bit_range_valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_id bit_range bit_range_valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/key_id/bit_range/bit_range_valid.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.bit_range.bit_range_valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.bit_range.bit_range_valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.bit_range.bit_range_valid: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].key_id.bit_range.bit_range_valid: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_id.bit_range.bit_range_valid);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_offset_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_id bit_range bit_range_offset") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_id bit_range bit_range_offset\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/key_id/bit_range/bit_range_offset.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.bit_range.bit_range_offset: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.bit_range.bit_range_offset: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.bit_range.bit_range_offset: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].key_id.bit_range.bit_range_offset: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_id.bit_range.bit_range_offset);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_key_id_bit_range_bit_range_size_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_id bit_range bit_range_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info key_id bit_range bit_range_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/key_id/bit_range/bit_range_size.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.bit_range.bit_range_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.bit_range.bit_range_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].key_id.bit_range.bit_range_size: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].key_id.bit_range.bit_range_size: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].key_id.bit_range.bit_range_size);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_dump(unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_dump(unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_dump(unit, context_info_idx_0, key_info_idx_0, attach_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_type_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = attach_info_idx_0, I2 = attach_info_idx_0 + 1, org_i2 = attach_info_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_input_type_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info attach_info input_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info attach_info input_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/attach_info/input_type.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].input_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].input_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].input_type: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].attach_info
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].attach_info), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].input_type: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].attach_info[i2].input_type,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].attach_info[%s%d].input_type: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_OPAQUE_FILE(
                        unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].attach_info[i2].input_type,
                            "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_input_arg_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = attach_info_idx_0, I2 = attach_info_idx_0 + 1, org_i2 = attach_info_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info attach_info input_arg") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info attach_info input_arg\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/attach_info/input_arg.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].input_arg: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].input_arg: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].input_arg: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].attach_info
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].attach_info), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].input_arg: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].attach_info[%s%d].input_arg: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_int(
                        unit,
                        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].attach_info[i2].input_arg);

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
dnx_field_context_sw_context_info_context_ipmf1_info_compare_info_pair_2_key_info_attach_info_offset_dump(int  unit,  int  context_info_idx_0,  int  key_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_info_idx_0, I1 = key_info_idx_0 + 1, org_i1 = key_info_idx_0;
    int i2 = attach_info_idx_0, I2 = attach_info_idx_0 + 1, org_i2 = attach_info_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info attach_info offset") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info compare_info pair_2 key_info attach_info offset\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/compare_info/pair_2/key_info/attach_info/offset.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].offset: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].offset: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].offset: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_KEYS_IN_COMPARE_MODE == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                if (i2 == -1) {
                    I2 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].attach_info
                        , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].attach_info), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
                {
                    LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.compare_info.pair_2.key_info[].attach_info[].offset: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.compare_info.pair_2.key_info[%s%d].attach_info[%s%d].offset: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_int(
                        unit,
                        &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.compare_info.pair_2.key_info[i1].attach_info[i2].offset);

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
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_dump(unit, context_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_mode_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_context_hash_mode_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/mode.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.mode: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.mode,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.mode: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.mode,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_is_set_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info is_set") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info is_set\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/is_set.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.is_set: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.is_set: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.is_set: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.is_set);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_dump(unit, context_info_idx_0, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_dump(unit, context_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_dump(unit, context_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_dump(int  unit,  int  context_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_dump(unit, context_info_idx_0, key_qual_map_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_dump(unit, context_info_idx_0, key_qual_map_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_dump(unit, context_info_idx_0, key_qual_map_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_qual_type_dump(int  unit,  int  context_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_qual_map_idx_0, I1 = key_qual_map_idx_0 + 1, org_i1 = key_qual_map_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_qual_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info key_template key_qual_map qual_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info key_template key_qual_map qual_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/key_template/key_qual_map/qual_type.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[].qual_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[].qual_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[].qual_type: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[i1].qual_type,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%s%d].qual_type: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[i1].qual_type,
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

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
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_lsb_dump(int  unit,  int  context_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_qual_map_idx_0, I1 = key_qual_map_idx_0 + 1, org_i1 = key_qual_map_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint16") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info key_template key_qual_map lsb") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info key_template key_qual_map lsb\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/key_template/key_qual_map/lsb.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[].lsb: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[].lsb: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[].lsb: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%s%d].lsb: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint16(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[i1].lsb);

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
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_qual_map_size_dump(int  unit,  int  context_info_idx_0,  int  key_qual_map_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = key_qual_map_idx_0, I1 = key_qual_map_idx_0 + 1, org_i1 = key_qual_map_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info key_template key_qual_map size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info key_template key_qual_map size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/key_template/key_qual_map/size.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[].size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[].size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[].size: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[%s%d].size: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint8(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_template.key_qual_map[i1].size);

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
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_template_key_size_in_bits_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint16") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info key_template key_size_in_bits") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info key_template key_size_in_bits\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/key_template/key_size_in_bits.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint16(
                unit,
                &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_template.key_size_in_bits);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_dump(unit, context_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_dump(unit, context_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_id_dump(int  unit,  int  context_info_idx_0,  int  id_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = id_idx_0, I1 = id_idx_0 + 1, org_i1 = id_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dbal_enum_value_field_field_key_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info key_id id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info key_id id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/key_id/id.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.key_id.id[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_id.id[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_id.id
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_id.id), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_id.id[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_id.id[i1],
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.key_id.id[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_id.id[i1],
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

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
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_full_key_allocated_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info key_id full_key_allocated") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info key_id full_key_allocated\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/key_id/full_key_allocated.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.key_id.full_key_allocated: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_id.full_key_allocated: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.key_id.full_key_allocated: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_id.full_key_allocated);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_dump(unit, context_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_valid_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info key_id bit_range bit_range_valid") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info key_id bit_range bit_range_valid\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/key_id/bit_range/bit_range_valid.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_valid: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_valid: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_valid: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_valid);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_offset_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info key_id bit_range bit_range_offset") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info key_id bit_range bit_range_offset\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/key_id/bit_range/bit_range_offset.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_offset: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_offset: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_offset: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_offset);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_key_id_bit_range_bit_range_size_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info key_id bit_range bit_range_size") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info key_id bit_range bit_range_size\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/key_id/bit_range/bit_range_size.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_size: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_size: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_size: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.key_id.bit_range.bit_range_size);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_dump(int  unit,  int  context_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_dump(unit, context_info_idx_0, attach_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_dump(unit, context_info_idx_0, attach_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_dump(unit, context_info_idx_0, attach_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_type_dump(int  unit,  int  context_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = attach_info_idx_0, I1 = attach_info_idx_0 + 1, org_i1 = attach_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_input_type_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info attach_info input_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info attach_info input_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/attach_info/input_type.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.attach_info[].input_type: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.attach_info[].input_type: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.attach_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.attach_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.attach_info[].input_type: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                    unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.attach_info[i1].input_type,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.attach_info[%s%d].input_type: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_OPAQUE_FILE(
                    unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.attach_info[i1].input_type,
                        "[%s%d][%s%d]: ", s0, i0, s1, i1);

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
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_input_arg_dump(int  unit,  int  context_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = attach_info_idx_0, I1 = attach_info_idx_0 + 1, org_i1 = attach_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info attach_info input_arg") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info attach_info input_arg\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/attach_info/input_arg.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.attach_info[].input_arg: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.attach_info[].input_arg: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.attach_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.attach_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.attach_info[].input_arg: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.attach_info[%s%d].input_arg: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.attach_info[i1].input_arg);

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
dnx_field_context_sw_context_info_context_ipmf1_info_hashing_info_key_info_attach_info_offset_dump(int  unit,  int  context_info_idx_0,  int  attach_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = attach_info_idx_0, I1 = attach_info_idx_0 + 1, org_i1 = attach_info_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf1_info hashing_info key_info attach_info offset") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf1_info hashing_info key_info attach_info offset\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf1_info/hashing_info/key_info/attach_info/offset.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf1_info.hashing_info.key_info.attach_info[].offset: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.attach_info[].offset: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.attach_info
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.attach_info), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf1_info.hashing_info.key_info.attach_info[].offset: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf1_info.hashing_info.key_info.attach_info[%s%d].offset: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_int(
                    unit,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf1_info.hashing_info.key_info.attach_info[i1].offset);

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
dnx_field_context_sw_context_info_context_ipmf2_info_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_dump(unit, context_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_context_ipmf2_info_cascaded_from_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_context_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info context_ipmf2_info cascaded_from") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info context_ipmf2_info cascaded_from\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/context_ipmf2_info/cascaded_from.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].context_ipmf2_info.cascaded_from: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].context_ipmf2_info.cascaded_from: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf2_info.cascaded_from,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].context_ipmf2_info.cascaded_from: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].context_ipmf2_info.cascaded_from,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_state_table_info_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_state_table_info_mode_dump(unit, context_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_state_table_info_is_set_dump(unit, context_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_state_table_info_mode_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_field_context_state_table_mode_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info state_table_info mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info state_table_info mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/state_table_info/mode.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].state_table_info.mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].state_table_info.mode: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].state_table_info.mode,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].state_table_info.mode: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].state_table_info.mode,
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_state_table_info_is_set_dump(int  unit,  int  context_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint8") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info state_table_info is_set") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info state_table_info is_set\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/state_table_info/is_set.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].state_table_info.is_set: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].state_table_info.is_set: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].state_table_info.is_set: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint8(
                unit,
                &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].state_table_info.is_set);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_name_dump(int  unit,  int  context_info_idx_0,  int  name_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_context_sw_context_info_name_value_dump(unit, context_info_idx_0, name_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_context_sw_context_info_name_value_dump(int  unit,  int  context_info_idx_0,  int  name_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = context_info_idx_0, I0 = context_info_idx_0 + 1;
    int i1 = name_idx_0, I1 = name_idx_0 + 1, org_i1 = name_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_string_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_context context_info name value") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_context context_info name value\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_context_sw/context_info/name/value.txt",
            "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[].name[].value: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_CONTEXT_MODULE_ID,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info
                , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info))
        {
            LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].name[].value: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_CONTEXT_MODULE_ID, ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = DNX_FIELD_STAGE_NOF;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].name
                    , sizeof(*((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].name), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= DNX_FIELD_STAGE_NOF)
            {
                LOG_CLI((BSL_META("dnx_field_context_sw[]->((dnx_field_context_sw_t*)sw_state_roots_array[][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[].name[].value: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(DNX_FIELD_STAGE_NOF == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_context_sw[%d]->","((dnx_field_context_sw_t*)sw_state_roots_array[%d][DNX_FIELD_CONTEXT_MODULE_ID])->","context_info[%s%d].name[%s%d].value: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                SW_STATE_STRING_STRING_PRINT(
                    unit,
                    DNX_FIELD_CONTEXT_MODULE_ID,
                    &((dnx_field_context_sw_t*)sw_state_roots_array[unit][DNX_FIELD_CONTEXT_MODULE_ID])->context_info[i0].name[i1].value);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_context_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_CONTEXT_SW_INFO_NOF_ENTRIES];
const char* dnx_field_context_sw_layout_str[DNX_FIELD_CONTEXT_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_CONTEXT_SW~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~MODE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~IS_SET~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_TEMPLATE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~QUAL_TYPE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~LSB~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~SIZE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_TEMPLATE~KEY_SIZE_IN_BITS~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_ID~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_ID~ID~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_ID~FULL_KEY_ALLOCATED~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_ID~BIT_RANGE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_ID~BIT_RANGE~BIT_RANGE_VALID~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_ID~BIT_RANGE~BIT_RANGE_OFFSET~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~KEY_ID~BIT_RANGE~BIT_RANGE_SIZE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~ATTACH_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~ATTACH_INFO~INPUT_TYPE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~ATTACH_INFO~INPUT_ARG~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_1~KEY_INFO~ATTACH_INFO~OFFSET~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~MODE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~IS_SET~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_TEMPLATE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~QUAL_TYPE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~LSB~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~SIZE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_TEMPLATE~KEY_SIZE_IN_BITS~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_ID~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_ID~ID~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_ID~FULL_KEY_ALLOCATED~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_ID~BIT_RANGE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_ID~BIT_RANGE~BIT_RANGE_VALID~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_ID~BIT_RANGE~BIT_RANGE_OFFSET~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~KEY_ID~BIT_RANGE~BIT_RANGE_SIZE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~ATTACH_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~ATTACH_INFO~INPUT_TYPE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~ATTACH_INFO~INPUT_ARG~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~COMPARE_INFO~PAIR_2~KEY_INFO~ATTACH_INFO~OFFSET~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~MODE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~IS_SET~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_TEMPLATE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~QUAL_TYPE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~LSB~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_TEMPLATE~KEY_QUAL_MAP~SIZE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_TEMPLATE~KEY_SIZE_IN_BITS~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_ID~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_ID~ID~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_ID~FULL_KEY_ALLOCATED~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_ID~BIT_RANGE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_ID~BIT_RANGE~BIT_RANGE_VALID~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_ID~BIT_RANGE~BIT_RANGE_OFFSET~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~KEY_ID~BIT_RANGE~BIT_RANGE_SIZE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~ATTACH_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~ATTACH_INFO~INPUT_TYPE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~ATTACH_INFO~INPUT_ARG~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF1_INFO~HASHING_INFO~KEY_INFO~ATTACH_INFO~OFFSET~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF2_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~CONTEXT_IPMF2_INFO~CASCADED_FROM~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~STATE_TABLE_INFO~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~STATE_TABLE_INFO~MODE~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~STATE_TABLE_INFO~IS_SET~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~NAME~",
    "DNX_FIELD_CONTEXT_SW~CONTEXT_INFO~NAME~VALUE~",
};
#endif 
#undef BSL_LOG_MODULE
