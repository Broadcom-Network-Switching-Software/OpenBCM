
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_mapper_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_tcam_access_mapper_t * dnx_field_tcam_access_mapper_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_tcam_access_mapper_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw_key_2_location_hash_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_location_hash_dump(int  unit,  int  key_2_location_hash_idx_0,  int  key_2_location_hash_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_dump(unit, key_2_location_hash_idx_0,key_2_location_hash_idx_1, -1, filters));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_dump(unit, key_2_location_hash_idx_0,key_2_location_hash_idx_1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_location_hash_head_dump(int  unit,  int  key_2_location_hash_idx_0,  int  key_2_location_hash_idx_1,  int  head_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = key_2_location_hash_idx_0, I0 = key_2_location_hash_idx_0 + 1;
    int i1 = key_2_location_hash_idx_1, I1 = key_2_location_hash_idx_1 + 1, org_i1 = key_2_location_hash_idx_1;
    int i2 = head_idx_0, I2 = head_idx_0 + 1, org_i2 = head_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access_mapper key_2_location_hash head") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access_mapper key_2_location_hash head\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_mapper_sw/key_2_location_hash/head.txt",
            "dnx_field_tcam_access_mapper_sw[%d]->","((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->","key_2_location_hash[][].head[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
                ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash
                , sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_mapper_sw[]->((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[][].head[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
                    ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]
                    , sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_access_mapper_sw[]->((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[][].head[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].head);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
                        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].head);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].head
                        , sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].head), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].head))
                {
                    LOG_CLI((BSL_META("dnx_field_tcam_access_mapper_sw[]->((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[][].head[]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].head) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_tcam_access_mapper_sw[%d]->","((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->","key_2_location_hash[%s%d][%s%d].head[%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].head[i2]);

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
dnx_field_tcam_access_mapper_sw_key_2_location_hash_next_dump(int  unit,  int  key_2_location_hash_idx_0,  int  key_2_location_hash_idx_1,  int  next_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = key_2_location_hash_idx_0, I0 = key_2_location_hash_idx_0 + 1;
    int i1 = key_2_location_hash_idx_1, I1 = key_2_location_hash_idx_1 + 1, org_i1 = key_2_location_hash_idx_1;
    int i2 = next_idx_0, I2 = next_idx_0 + 1, org_i2 = next_idx_0;
    char *s0 = "", *s1 = "", *s2 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access_mapper key_2_location_hash next") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access_mapper key_2_location_hash next\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_mapper_sw/key_2_location_hash/next.txt",
            "dnx_field_tcam_access_mapper_sw[%d]->","((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->","key_2_location_hash[][].next[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
                ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash
                , sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_mapper_sw[]->((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[][].next[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
                    ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]
                    , sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_access_mapper_sw[]->((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[][].next[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                
                
                    DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                        unit,
                        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].next);

                if (i2 == -1) {
                    I2 = dnx_sw_state_get_nof_elements(unit,
                        DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
                        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].next);
                    i2 = dnx_sw_state_dump_check_all_the_same(unit,
                        ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].next
                        , sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].next), I2, &s2) ? I2 - 1 : 0;
                }

                if(i2 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].next))
                {
                    LOG_CLI((BSL_META("dnx_field_tcam_access_mapper_sw[]->((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[][].next[]: ")));
                    LOG_CLI((BSL_META("Invalid index: %d \n"),i2));
                    SHR_EXIT();
                }

                if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].next) == 0)
                {
                    SHR_EXIT();
                }

                for(; i2 < I2; i2++) {
                    dnx_sw_state_dump_update_current_idx(unit, i2);
                    DNX_SW_STATE_PRINT_MONITOR(
                        unit,
                        "dnx_field_tcam_access_mapper_sw[%d]->","((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->","key_2_location_hash[%s%d][%s%d].next[%s%d]: ", s0, i0, s1, i1, s2, i2);

                    DNX_SW_STATE_PRINT_FILE(
                        unit,
                        "[%s%d][%s%d][%s%d]: ", s0, i0, s1, i1, s2, i2);

                    dnx_sw_state_print_uint32(
                        unit,
                        &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_location_hash[i0][i1].next[i2]);

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





dnx_sw_state_diagnostic_info_t dnx_field_tcam_access_mapper_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES];
const char* dnx_field_tcam_access_mapper_sw_layout_str[DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~",
    "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~KEY_2_LOCATION_HASH~",
    "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~KEY_2_LOCATION_HASH~HEAD~",
    "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~KEY_2_LOCATION_HASH~NEXT~",
};
#endif 
#undef BSL_LOG_MODULE
