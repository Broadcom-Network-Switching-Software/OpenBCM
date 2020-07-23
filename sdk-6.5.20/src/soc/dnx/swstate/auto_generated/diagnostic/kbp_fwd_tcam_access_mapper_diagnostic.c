
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#if defined(INCLUDE_KBP)
#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/kbp_fwd_tcam_access_mapper_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern kbp_fwd_tcam_access_mapper_t * kbp_fwd_tcam_access_mapper_data[SOC_MAX_NUM_DEVICES];



int
kbp_fwd_tcam_access_mapper_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
kbp_fwd_tcam_access_mapper_key_2_entry_id_hash_dump(int  unit,  int  key_2_entry_id_hash_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = key_2_entry_id_hash_idx_0, I0 = key_2_entry_id_hash_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_htbl_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "kbp_fwd_tcam_access_mapper key_2_entry_id_hash") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate kbp_fwd_tcam_access_mapper key_2_entry_id_hash\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "kbp_fwd_tcam_access_mapper/key_2_entry_id_hash.txt",
            "kbp_fwd_tcam_access_mapper[%d]->","((kbp_fwd_tcam_access_mapper_t*)sw_state_roots_array[%d][KBP_FWD_TCAM_ACCESS_MAPPER_MODULE_ID])->","key_2_entry_id_hash[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((kbp_fwd_tcam_access_mapper_t*)sw_state_roots_array[unit][KBP_FWD_TCAM_ACCESS_MAPPER_MODULE_ID]));

        if (i0 == -1) {
            I0 = DNX_KBP_MAX_NOF_TCAM_HASH_TABLE_INDICES;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((kbp_fwd_tcam_access_mapper_t*)sw_state_roots_array[unit][KBP_FWD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash
                , sizeof(*((kbp_fwd_tcam_access_mapper_t*)sw_state_roots_array[unit][KBP_FWD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= DNX_KBP_MAX_NOF_TCAM_HASH_TABLE_INDICES)
        {
            LOG_CLI((BSL_META("kbp_fwd_tcam_access_mapper[]->((kbp_fwd_tcam_access_mapper_t*)sw_state_roots_array[][KBP_FWD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(DNX_KBP_MAX_NOF_TCAM_HASH_TABLE_INDICES == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "kbp_fwd_tcam_access_mapper[%d]->","((kbp_fwd_tcam_access_mapper_t*)sw_state_roots_array[%d][KBP_FWD_TCAM_ACCESS_MAPPER_MODULE_ID])->","key_2_entry_id_hash[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            SW_STATE_HTB_DEFAULT_PRINT(
                unit,
                KBP_FWD_TCAM_ACCESS_MAPPER_MODULE_ID,
                &((kbp_fwd_tcam_access_mapper_t*)sw_state_roots_array[unit][KBP_FWD_TCAM_ACCESS_MAPPER_MODULE_ID])->key_2_entry_id_hash[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t kbp_fwd_tcam_access_mapper_info[SOC_MAX_NUM_DEVICES][KBP_FWD_TCAM_ACCESS_MAPPER_INFO_NOF_ENTRIES];
const char* kbp_fwd_tcam_access_mapper_layout_str[KBP_FWD_TCAM_ACCESS_MAPPER_INFO_NOF_ENTRIES] = {
    "KBP_FWD_TCAM_ACCESS_MAPPER~",
    "KBP_FWD_TCAM_ACCESS_MAPPER~KEY_2_ENTRY_ID_HASH~",
};
#endif 
#endif  
#undef BSL_LOG_MODULE
