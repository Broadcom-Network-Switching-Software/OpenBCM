
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/algo_lif_types.h>
#include <soc/dnx/swstate/auto_generated/layout/algo_lif_layout.h>

dnxc_sw_state_layout_t layout_array_algo_lif[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO_NOF_PARAMS)];

shr_error_e
algo_lif_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO);
    layout_array_algo_lif[idx].name = "local_outlif_info";
    layout_array_algo_lif[idx].type = "local_outlif_info_t";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(local_outlif_info_t);
    layout_array_algo_lif[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__FIRST);
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__DISABLE_MDB_CLUSTERS);
    layout_array_algo_lif[idx].name = "local_outlif_info__disable_mdb_clusters";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "Set this to TRUE during tests to force the outlif allocation to use eedb banks instead of EEDB clusters.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].parent  = 0;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__DISABLE_EEDB_DATA_BANKS);
    layout_array_algo_lif[idx].name = "local_outlif_info__disable_eedb_data_banks";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "Set this to TRUE during tests to force the outlif allocation to use mdb clusters instead of EEDB banks.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].parent  = 0;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__OUTLIF_BANK_INFO);
    layout_array_algo_lif[idx].name = "local_outlif_info__outlif_bank_info";
    layout_array_algo_lif[idx].type = "local_outlif_bank_info_t*";
    layout_array_algo_lif[idx].doc = "Info per outlif bank.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(local_outlif_bank_info_t*);
    layout_array_algo_lif[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_algo_lif[idx].parent  = 0;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__OUTLIF_BANK_INFO__FIRST);
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__OUTLIF_BANK_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO);
    layout_array_algo_lif[idx].name = "local_outlif_info__logical_phase_info";
    layout_array_algo_lif[idx].type = "outlif_logical_phase_info_t*";
    layout_array_algo_lif[idx].doc = "Info per logical phase.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(outlif_logical_phase_info_t*);
    layout_array_algo_lif[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_algo_lif[idx].parent  = 0;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO__FIRST);
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__PHYSICAL_PHASE_TO_LOGICAL_PHASE_MAP);
    layout_array_algo_lif[idx].name = "local_outlif_info__physical_phase_to_logical_phase_map";
    layout_array_algo_lif[idx].type = "uint8*";
    layout_array_algo_lif[idx].doc = "Map from eedb physical phase to outlif logical phase.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8*);
    layout_array_algo_lif[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_algo_lif[idx].parent  = 0;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__EEDB_BANKS_INFO);
    layout_array_algo_lif[idx].name = "local_outlif_info__eedb_banks_info";
    layout_array_algo_lif[idx].type = "eedb_banks_info_t";
    layout_array_algo_lif[idx].doc = "Information required to manage the physical EEDB banks";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(eedb_banks_info_t);
    layout_array_algo_lif[idx].parent  = 0;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__EEDB_BANKS_INFO__FIRST);
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__EEDB_BANKS_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__OUTLIF_BANK_INFO__ASSIGNED);
    layout_array_algo_lif[idx].name = "local_outlif_info__outlif_bank_info__assigned";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "Indication of whether this bank is assigned to a specific physical database or not, and which type (MDB or EEDB)";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].parent  = 3;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__OUTLIF_BANK_INFO__IN_USE);
    layout_array_algo_lif[idx].name = "local_outlif_info__outlif_bank_info__in_use";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "Boolean indication of whether this bank is in use or not.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].parent  = 3;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__OUTLIF_BANK_INFO__USED_LOGICAL_PHASE);
    layout_array_algo_lif[idx].name = "local_outlif_info__outlif_bank_info__used_logical_phase";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "EEDB Logical phase using this table (0-7).";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].parent  = 3;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__OUTLIF_BANK_INFO__LL_IN_USE);
    layout_array_algo_lif[idx].name = "local_outlif_info__outlif_bank_info__ll_in_use";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "Boolean indication of whether this bank is using linked list or not.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].parent  = 3;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__OUTLIF_BANK_INFO__LL_INDEX);
    layout_array_algo_lif[idx].name = "local_outlif_info__outlif_bank_info__ll_index";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "Index of the linked list bank that belongs to this bank, from the logcial phase's list.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].parent  = 3;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__OUTLIF_BANK_INFO__EEDB_DATA_BANKS);
    layout_array_algo_lif[idx].name = "local_outlif_info__outlif_bank_info__eedb_data_banks";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "If this outlif bank is using EEDB banks to store data, then these are their indexes.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].array_indexes[0].num_elements = MAX_EEDB_BANKS_PER_OUTLIF_BANK;
    layout_array_algo_lif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_algo_lif[idx].parent  = 3;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO__PHYSICAL_PHASE);
    layout_array_algo_lif[idx].name = "local_outlif_info__logical_phase_info__physical_phase";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "Physical phase this logical phase is mapped to.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].parent  = 4;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO__ADDRESS_GRANULARITY);
    layout_array_algo_lif[idx].name = "local_outlif_info__logical_phase_info__address_granularity";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "MDB address granularity for this logical phase";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].parent  = 4;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO__FIRST_BANK);
    layout_array_algo_lif[idx].name = "local_outlif_info__logical_phase_info__first_bank";
    layout_array_algo_lif[idx].type = "uint32";
    layout_array_algo_lif[idx].doc = "First outlif bank used by this logical phase.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint32);
    layout_array_algo_lif[idx].parent  = 4;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO__LAST_BANK);
    layout_array_algo_lif[idx].name = "local_outlif_info__logical_phase_info__last_bank";
    layout_array_algo_lif[idx].type = "uint32";
    layout_array_algo_lif[idx].doc = "Last outlif bank used by this logical phase.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint32);
    layout_array_algo_lif[idx].parent  = 4;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO__FIRST_NO_LL_BANK);
    layout_array_algo_lif[idx].name = "local_outlif_info__logical_phase_info__first_no_ll_bank";
    layout_array_algo_lif[idx].type = "uint32";
    layout_array_algo_lif[idx].doc = "First outlif bank used by this logical phase above 512k.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint32);
    layout_array_algo_lif[idx].parent  = 4;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO__LAST_NO_LL_BANK);
    layout_array_algo_lif[idx].name = "local_outlif_info__logical_phase_info__last_no_ll_bank";
    layout_array_algo_lif[idx].type = "uint32";
    layout_array_algo_lif[idx].doc = "Last outlif bank used by this logical phase above 512k.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint32);
    layout_array_algo_lif[idx].parent  = 4;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO__FIRST_LL_BANK);
    layout_array_algo_lif[idx].name = "local_outlif_info__logical_phase_info__first_ll_bank";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "First linked list bank available for this phase.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].parent  = 4;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO__LAST_LL_BANK);
    layout_array_algo_lif[idx].name = "local_outlif_info__logical_phase_info__last_ll_bank";
    layout_array_algo_lif[idx].type = "uint8";
    layout_array_algo_lif[idx].doc = "Last linked list bank available for this phase.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(uint8);
    layout_array_algo_lif[idx].parent  = 4;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__LOGICAL_PHASE_INFO__DBAL_VALID_COMBINATIONS);
    layout_array_algo_lif[idx].name = "local_outlif_info__logical_phase_info__dbal_valid_combinations";
    layout_array_algo_lif[idx].type = "sw_state_htbl_t";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(sw_state_htbl_t);
    layout_array_algo_lif[idx].parent  = 4;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__EEDB_BANKS_INFO__BANK_OCCUPATION_BITMAP);
    layout_array_algo_lif[idx].name = "local_outlif_info__eedb_banks_info__bank_occupation_bitmap";
    layout_array_algo_lif[idx].type = "SHR_BITDCL*";
    layout_array_algo_lif[idx].doc = "Occupation bitmap of the eedb banks.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_algo_lif[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP;
    layout_array_algo_lif[idx].parent  = 6;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__EEDB_BANKS_INFO__OUTLIF_PREFIX_OCCUPATION_BITMAP);
    layout_array_algo_lif[idx].name = "local_outlif_info__eedb_banks_info__outlif_prefix_occupation_bitmap";
    layout_array_algo_lif[idx].type = "SHR_BITDCL*";
    layout_array_algo_lif[idx].doc = "Occupation bitmap of the outlif prefixes. Keep it to avoid aliasing";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_algo_lif[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP;
    layout_array_algo_lif[idx].parent  = 6;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_LOCAL_OUTLIF_INFO__EEDB_BANKS_INFO__USED_DATA_BANK_PER_LOGICAL_PHASE);
    layout_array_algo_lif[idx].name = "local_outlif_info__eedb_banks_info__used_data_bank_per_logical_phase";
    layout_array_algo_lif[idx].type = "sw_state_multihead_ll_t";
    layout_array_algo_lif[idx].doc = "For each logical phase, keep a linked list of the eedb banks used for data in this phase.";
    layout_array_algo_lif[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].size_of = sizeof(sw_state_multihead_ll_t);
    layout_array_algo_lif[idx].parent  = 6;
    layout_array_algo_lif[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_algo_lif[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_LOCAL_OUTLIF_INFO, layout_array_algo_lif, sw_state_layout_array[unit][ALGO_LIF_MODULE_ID], DNX_SW_STATE_LOCAL_OUTLIF_INFO_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
