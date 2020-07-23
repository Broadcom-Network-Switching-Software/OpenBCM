
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_access_mapper_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_field_tcam_access_mapper_t * dnx_field_tcam_access_mapper_sw_data[SOC_MAX_NUM_DEVICES];



int
dnx_field_tcam_access_mapper_sw_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_dump(unit, -1, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_access_mapper_sw_key_2_entry_id_hash_dump(int  unit,  int  key_2_entry_id_hash_idx_0,  int  key_2_entry_id_hash_idx_1,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = key_2_entry_id_hash_idx_0, I0 = key_2_entry_id_hash_idx_0 + 1;
    int i1 = key_2_entry_id_hash_idx_1, I1 = key_2_entry_id_hash_idx_1 + 1, org_i1 = key_2_entry_id_hash_idx_1;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_field_tcam_access_mapper key_2_entry_id_hash") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_field_tcam_access_mapper key_2_entry_id_hash\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_field_tcam_access_mapper_sw/key_2_entry_id_hash.txt",
            "dnx_field_tcam_access_mapper_sw[%d]->","((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->","key_2_entry_id_hash[][]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
                ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash
                , sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash))
        {
            LOG_CLI((BSL_META("dnx_field_tcam_access_mapper_sw[]->((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[][]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            
            
                DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
                    unit,
                    ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[i0]);

            if (i1 == -1) {
                I1 = dnx_sw_state_get_nof_elements(unit,
                    DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
                    ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[i0]);
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[i0]
                    , sizeof(*((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[i0]), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[i0]))
            {
                LOG_CLI((BSL_META("dnx_field_tcam_access_mapper_sw[]->((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[][]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(dnx_sw_state_get_nof_elements(unit, DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID, ((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[i0]) == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_field_tcam_access_mapper_sw[%d]->","((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[%d][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->","key_2_entry_id_hash[%s%d][%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                SW_STATE_HTB_DEFAULT_PRINT(
                    unit,
                    DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID,
                    &((dnx_field_tcam_access_mapper_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[i0][i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_field_tcam_access_mapper_sw_info[SOC_MAX_NUM_DEVICES][DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES];
const char* dnx_field_tcam_access_mapper_sw_layout_str[DNX_FIELD_TCAM_ACCESS_MAPPER_SW_INFO_NOF_ENTRIES] = {
    "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~",
    "DNX_FIELD_TCAM_ACCESS_MAPPER_SW~KEY_2_ENTRY_ID_HASH~",
};
#endif 
#undef BSL_LOG_MODULE
