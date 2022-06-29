
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#if defined(INCLUDE_KBP)
#include <soc/dnx/swstate/auto_generated/types/kbp_fwd_tcam_access_mapper_types.h>
#include <soc/dnx/swstate/auto_generated/layout/kbp_fwd_tcam_access_mapper_layout.h>

dnxc_sw_state_layout_t layout_array_kbp_fwd_tcam_access_mapper[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER_NOF_PARAMS)];

shr_error_e
kbp_fwd_tcam_access_mapper_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER);
    layout_array_kbp_fwd_tcam_access_mapper[idx].name = "kbp_fwd_tcam_access_mapper";
    layout_array_kbp_fwd_tcam_access_mapper[idx].type = "kbp_fwd_tcam_access_mapper_t";
    layout_array_kbp_fwd_tcam_access_mapper[idx].doc = "Maps KBP IPv4/6 FWD TCAM keys to entry ids";
    layout_array_kbp_fwd_tcam_access_mapper[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp_fwd_tcam_access_mapper[idx].size_of = sizeof(kbp_fwd_tcam_access_mapper_t);
    layout_array_kbp_fwd_tcam_access_mapper[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp_fwd_tcam_access_mapper[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER__FIRST);
    layout_array_kbp_fwd_tcam_access_mapper[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER__KEY_2_ENTRY_ID_HASH);
    layout_array_kbp_fwd_tcam_access_mapper[idx].name = "kbp_fwd_tcam_access_mapper__key_2_entry_id_hash";
    layout_array_kbp_fwd_tcam_access_mapper[idx].type = "sw_state_htbl_t";
    layout_array_kbp_fwd_tcam_access_mapper[idx].doc = "The hash that maps the key to entry id";
    layout_array_kbp_fwd_tcam_access_mapper[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp_fwd_tcam_access_mapper[idx].size_of = sizeof(sw_state_htbl_t);
    layout_array_kbp_fwd_tcam_access_mapper[idx].array_indexes[0].num_elements = DNX_KBP_MAX_NOF_TCAM_HASH_TABLE_INDICES;
    layout_array_kbp_fwd_tcam_access_mapper[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_kbp_fwd_tcam_access_mapper[idx].parent  = 0;
    layout_array_kbp_fwd_tcam_access_mapper[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp_fwd_tcam_access_mapper[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_kbp_fwd_tcam_access_mapper[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOC_EXCEPTION;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER, layout_array_kbp_fwd_tcam_access_mapper, sw_state_layout_array[unit][KBP_FWD_TCAM_ACCESS_MAPPER_MODULE_ID], DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#endif  
#undef BSL_LOG_MODULE
