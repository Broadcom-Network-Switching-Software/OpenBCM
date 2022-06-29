
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxf/swstate/auto_generated/types/dnxf_types.h>
#include <soc/dnxf/swstate/auto_generated/layout/dnxf_layout.h>

dnxc_sw_state_layout_t layout_array_dnxf[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE_NOF_PARAMS)];

shr_error_e
dnxf_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE);
    layout_array_dnxf[idx].name = "dnxf_state";
    layout_array_dnxf[idx].type = "dnxf_state_t";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(dnxf_state_t);
    layout_array_dnxf[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INFO);
    layout_array_dnxf[idx].name = "dnxf_state__info";
    layout_array_dnxf[idx].type = "soc_dnxf_info_t";
    layout_array_dnxf[idx].doc = "General device info";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_info_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INFO__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID);
    layout_array_dnxf[idx].name = "dnxf_state__modid";
    layout_array_dnxf[idx].type = "soc_dnxf_modid_t";
    layout_array_dnxf[idx].doc = "Module-ID Mapping Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_modid_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MC);
    layout_array_dnxf[idx].name = "dnxf_state__mc";
    layout_array_dnxf[idx].type = "soc_dnxf_mc_t";
    layout_array_dnxf[idx].doc = "Multicast Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_mc_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MC__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MC__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LB);
    layout_array_dnxf[idx].name = "dnxf_state__lb";
    layout_array_dnxf[idx].type = "soc_dnxf_lb_t";
    layout_array_dnxf[idx].doc = "Load Balancing Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_lb_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LB__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__PORT);
    layout_array_dnxf[idx].name = "dnxf_state__port";
    layout_array_dnxf[idx].type = "soc_dnxf_port_t";
    layout_array_dnxf[idx].doc = "Port Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_port_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__PORT__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__PORT__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__CABLE_SWAP);
    layout_array_dnxf[idx].name = "dnxf_state__cable_swap";
    layout_array_dnxf[idx].type = "soc_dnxf_cable_swap_t";
    layout_array_dnxf[idx].doc = "Cable swap Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_cable_swap_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INTR);
    layout_array_dnxf[idx].name = "dnxf_state__intr";
    layout_array_dnxf[idx].type = "soc_dnxf_intr_t";
    layout_array_dnxf[idx].doc = "Interrupt Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_intr_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INTR__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INTR__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB);
    layout_array_dnxf[idx].name = "dnxf_state__lane_map_db";
    layout_array_dnxf[idx].type = "soc_dnxf_lane_map_db_t";
    layout_array_dnxf[idx].doc = "Lane Mapping Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_lane_map_db_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYNCE);
    layout_array_dnxf[idx].name = "dnxf_state__synce";
    layout_array_dnxf[idx].type = "soc_dnxc_synce_status_t";
    layout_array_dnxf[idx].doc = "SyncE status Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxc_synce_status_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYNCE__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYNCE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE);
    layout_array_dnxf[idx].name = "dnxf_state__system_upgrade";
    layout_array_dnxf[idx].type = "soc_dnxf_system_upgrade_t";
    layout_array_dnxf[idx].doc = "System Upgrade status Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_system_upgrade_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__HARD_RESET);
    layout_array_dnxf[idx].name = "dnxf_state__hard_reset";
    layout_array_dnxf[idx].type = "soc_dnxf_hard_reset_t";
    layout_array_dnxf[idx].doc = "Hard reset CB register";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_hard_reset_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__HARD_RESET__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__HARD_RESET__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MIB);
    layout_array_dnxf[idx].name = "dnxf_state__mib";
    layout_array_dnxf[idx].type = "soc_dnxc_mib_db_t";
    layout_array_dnxf[idx].doc = "Mib Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxc_mib_db_t);
    layout_array_dnxf[idx].parent  = 0;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MIB__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MIB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP);
    layout_array_dnxf[idx].name = "dnxf_state__info__active_core_bmp";
    layout_array_dnxf[idx].type = "SHR_BITDCL*";
    layout_array_dnxf[idx].doc = "bitmap of active cores in the device";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP;
    layout_array_dnxf[idx].parent  = 1;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ACCESS_PTR;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL);
    layout_array_dnxf[idx].name = "dnxf_state__info__soft_init_reg_above_64_val";
    layout_array_dnxf[idx].type = "uint32*";
    layout_array_dnxf[idx].doc = "soft init reg value used for putting device out of reset when not all cores are active";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 1;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP);
    layout_array_dnxf[idx].name = "dnxf_state__modid__local_map";
    layout_array_dnxf[idx].type = "soc_dnxf_modid_local_map_t*";
    layout_array_dnxf[idx].doc = "mapping of local module-ID to module-ID";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_modid_local_map_t*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 2;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__LAST)-1;
    layout_array_dnxf[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP);
    layout_array_dnxf[idx].name = "dnxf_state__modid__group_map";
    layout_array_dnxf[idx].type = "soc_dnxf_modid_group_map_t*";
    layout_array_dnxf[idx].doc = "mapping of group-ID to module-IDs.\n                                                                                        used in FE2 multi-stage only";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxf_modid_group_map_t*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 2;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP);
    layout_array_dnxf[idx].name = "dnxf_state__modid__modid_to_group_map";
    layout_array_dnxf[idx].type = "soc_module_t*";
    layout_array_dnxf[idx].doc = "mapping module-ID to group-ID.\n                                                                                   used in FE2 multi-stage only";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_module_t*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 2;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID);
    layout_array_dnxf[idx].name = "dnxf_state__modid__local_map__valid";
    layout_array_dnxf[idx].type = "uint32";
    layout_array_dnxf[idx].doc = "boolean value. '0' - mapping not valid ; '1' - mapping valid";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32);
    layout_array_dnxf[idx].parent  = 16;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID);
    layout_array_dnxf[idx].name = "dnxf_state__modid__local_map__module_id";
    layout_array_dnxf[idx].type = "uint32";
    layout_array_dnxf[idx].doc = "mapped module-ID";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32);
    layout_array_dnxf[idx].parent  = 16;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW);
    layout_array_dnxf[idx].name = "dnxf_state__modid__group_map__raw";
    layout_array_dnxf[idx].type = "SHR_BITDCL*";
    layout_array_dnxf[idx].doc = "bitmap of mapped module-IDs";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP;
    layout_array_dnxf[idx].parent  = 17;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MC__MODE);
    layout_array_dnxf[idx].name = "dnxf_state__mc__mode";
    layout_array_dnxf[idx].type = "uint32";
    layout_array_dnxf[idx].doc = "MC table mode (ID range). 0: 64K ; 1: 128K ; 2: 128K HALF ; 3: 256K ; 4: 256K ; 5: 512K";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32);
    layout_array_dnxf[idx].parent  = 3;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MC__ID_MAP);
    layout_array_dnxf[idx].name = "dnxf_state__mc__id_map";
    layout_array_dnxf[idx].type = "uint32*";
    layout_array_dnxf[idx].doc = "valid MC-ID bitmap";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 3;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED);
    layout_array_dnxf[idx].name = "dnxf_state__mc__nof_mc_groups_created";
    layout_array_dnxf[idx].type = "uint32";
    layout_array_dnxf[idx].doc = "Number of created multicast groups";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32);
    layout_array_dnxf[idx].parent  = 3;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK);
    layout_array_dnxf[idx].name = "dnxf_state__lb__group_to_first_link";
    layout_array_dnxf[idx].type = "soc_port_t**";
    layout_array_dnxf[idx].doc = "first link of a group per core";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_port_t**);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].array_indexes[1].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 4;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF);
    layout_array_dnxf[idx].name = "dnxf_state__port__cl72_conf";
    layout_array_dnxf[idx].type = "uint32*";
    layout_array_dnxf[idx].doc = "cl72 state. '0' - cl72 disabled ; '1' - cl72 enabled";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 5;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG);
    layout_array_dnxf[idx].name = "dnxf_state__port__phy_lane_config";
    layout_array_dnxf[idx].type = "int*";
    layout_array_dnxf[idx].doc = "phy_lane_config bitmap, same as in bcm_port_resource API. Includes firmware configurations";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 5;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__PORT__SPEED);
    layout_array_dnxf[idx].name = "dnxf_state__port__speed";
    layout_array_dnxf[idx].type = "int*";
    layout_array_dnxf[idx].doc = "link rate";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 5;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE);
    layout_array_dnxf[idx].name = "dnxf_state__port__fec_type";
    layout_array_dnxf[idx].type = "int*";
    layout_array_dnxf[idx].doc = "FEC type";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 5;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER);
    layout_array_dnxf[idx].name = "dnxf_state__port__is_connected_to_repeater";
    layout_array_dnxf[idx].type = "SHR_BITDCL*";
    layout_array_dnxf[idx].doc = "bitmap of links connected to a repeater";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(SHR_BITDCL*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_BITMAP;
    layout_array_dnxf[idx].parent  = 5;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED);
    layout_array_dnxf[idx].name = "dnxf_state__port__isolation_status_stored";
    layout_array_dnxf[idx].type = "int*";
    layout_array_dnxf[idx].doc = "Flag used to indicate if we stoered the original isolation status.";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 5;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_COUNTER;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS);
    layout_array_dnxf[idx].name = "dnxf_state__port__orig_isolated_status";
    layout_array_dnxf[idx].type = "int*";
    layout_array_dnxf[idx].doc = "Used to store the original isolation status before we isolate prior to putting FMAC in reset.";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 5;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE);
    layout_array_dnxf[idx].name = "dnxf_state__cable_swap__is_enable";
    layout_array_dnxf[idx].type = "int*";
    layout_array_dnxf[idx].doc = "is cable swap enabled";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 6;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER);
    layout_array_dnxf[idx].name = "dnxf_state__cable_swap__is_master";
    layout_array_dnxf[idx].type = "int*";
    layout_array_dnxf[idx].doc = "is cable swap operating as a master";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 6;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INTR__FLAGS);
    layout_array_dnxf[idx].name = "dnxf_state__intr__flags";
    layout_array_dnxf[idx].type = "uint32*";
    layout_array_dnxf[idx].doc = "Interrupt flags";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 7;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT);
    layout_array_dnxf[idx].name = "dnxf_state__intr__storm_timed_count";
    layout_array_dnxf[idx].type = "uint32*";
    layout_array_dnxf[idx].doc = "Interrupt storm time count value";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 7;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD);
    layout_array_dnxf[idx].name = "dnxf_state__intr__storm_timed_period";
    layout_array_dnxf[idx].type = "uint32*";
    layout_array_dnxf[idx].doc = "Interrupt time period value";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 7;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL);
    layout_array_dnxf[idx].name = "dnxf_state__intr__storm_nominal";
    layout_array_dnxf[idx].type = "uint32";
    layout_array_dnxf[idx].doc = "interrupt storm nominal";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32);
    layout_array_dnxf[idx].parent  = 7;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP);
    layout_array_dnxf[idx].name = "dnxf_state__lane_map_db__serdes_map";
    layout_array_dnxf[idx].type = "soc_dnxc_lane_map_db_t";
    layout_array_dnxf[idx].doc = "comon Lane Mapping Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxc_lane_map_db_t);
    layout_array_dnxf[idx].parent  = 8;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP);
    layout_array_dnxf[idx].name = "dnxf_state__lane_map_db__link_to_fmac_lane_map";
    layout_array_dnxf[idx].type = "int*";
    layout_array_dnxf[idx].doc = "Maps a port to their corresponding blocks";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 8;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE);
    layout_array_dnxf[idx].name = "dnxf_state__lane_map_db__serdes_map__map_size";
    layout_array_dnxf[idx].type = "int";
    layout_array_dnxf[idx].doc = "nof lanes represented by the map database";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int);
    layout_array_dnxf[idx].parent  = 39;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES);
    layout_array_dnxf[idx].name = "dnxf_state__lane_map_db__serdes_map__lane2serdes";
    layout_array_dnxf[idx].type = "soc_dnxc_lane_map_db_map_t*";
    layout_array_dnxf[idx].doc = "defines serdes rx and tx for each lane";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxc_lane_map_db_map_t*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 39;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__LAST)-1;
    layout_array_dnxf[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE);
    layout_array_dnxf[idx].name = "dnxf_state__lane_map_db__serdes_map__serdes2lane";
    layout_array_dnxf[idx].type = "soc_dnxc_lane_map_db_map_t*";
    layout_array_dnxf[idx].doc = "defines lane rx and tx for each serdes";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(soc_dnxc_lane_map_db_map_t*);
    layout_array_dnxf[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnxf[idx].parent  = 39;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__FIRST);
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__LAST)-1;
    layout_array_dnxf[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_FORCE_INTERFACE_GENERATION;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID);
    layout_array_dnxf[idx].name = "dnxf_state__lane_map_db__serdes_map__lane2serdes__rx_id";
    layout_array_dnxf[idx].type = "int";
    layout_array_dnxf[idx].doc = "lane is mapped to this serdes rx or serdes is mapped to this lane rx";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int);
    layout_array_dnxf[idx].parent  = 42;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID);
    layout_array_dnxf[idx].name = "dnxf_state__lane_map_db__serdes_map__lane2serdes__tx_id";
    layout_array_dnxf[idx].type = "int";
    layout_array_dnxf[idx].doc = "lane is mapped to this serdes tx or serdes is mapped to this lane tx";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int);
    layout_array_dnxf[idx].parent  = 42;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID);
    layout_array_dnxf[idx].name = "dnxf_state__lane_map_db__serdes_map__serdes2lane__rx_id";
    layout_array_dnxf[idx].type = "int";
    layout_array_dnxf[idx].doc = "lane is mapped to this serdes rx or serdes is mapped to this lane rx";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int);
    layout_array_dnxf[idx].parent  = 43;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID);
    layout_array_dnxf[idx].name = "dnxf_state__lane_map_db__serdes_map__serdes2lane__tx_id";
    layout_array_dnxf[idx].type = "int";
    layout_array_dnxf[idx].doc = "lane is mapped to this serdes tx or serdes is mapped to this lane tx";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int);
    layout_array_dnxf[idx].parent  = 43;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED);
    layout_array_dnxf[idx].name = "dnxf_state__synce__master_synce_enabled";
    layout_array_dnxf[idx].type = "int";
    layout_array_dnxf[idx].doc = "master synce is enabled";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int);
    layout_array_dnxf[idx].parent  = 9;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED);
    layout_array_dnxf[idx].name = "dnxf_state__synce__slave_synce_enabled";
    layout_array_dnxf[idx].type = "int";
    layout_array_dnxf[idx].doc = "slave synce is enabled";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int);
    layout_array_dnxf[idx].parent  = 9;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED);
    layout_array_dnxf[idx].name = "dnxf_state__synce__master_set1_synce_enabled";
    layout_array_dnxf[idx].type = "int";
    layout_array_dnxf[idx].doc = "master to set1 synce is enabled";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int);
    layout_array_dnxf[idx].parent  = 9;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED);
    layout_array_dnxf[idx].name = "dnxf_state__synce__slave_set1_synce_enabled";
    layout_array_dnxf[idx].type = "int";
    layout_array_dnxf[idx].doc = "slave to set1 synce is enabled";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int);
    layout_array_dnxf[idx].parent  = 9;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0);
    layout_array_dnxf[idx].name = "dnxf_state__system_upgrade__state_0";
    layout_array_dnxf[idx].type = "uint32";
    layout_array_dnxf[idx].doc = "System upgrade state 0";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32);
    layout_array_dnxf[idx].parent  = 10;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1);
    layout_array_dnxf[idx].name = "dnxf_state__system_upgrade__state_1";
    layout_array_dnxf[idx].type = "uint32";
    layout_array_dnxf[idx].doc = "System upgrade state 1";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32);
    layout_array_dnxf[idx].parent  = 10;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2);
    layout_array_dnxf[idx].name = "dnxf_state__system_upgrade__state_2";
    layout_array_dnxf[idx].type = "uint32";
    layout_array_dnxf[idx].doc = "System upgrade state 2";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32);
    layout_array_dnxf[idx].parent  = 10;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK);
    layout_array_dnxf[idx].name = "dnxf_state__hard_reset__callback";
    layout_array_dnxf[idx].type = "bcm_switch_hard_reset_callback_t";
    layout_array_dnxf[idx].doc = "pointer to hard reset callback function";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(bcm_switch_hard_reset_callback_t);
    layout_array_dnxf[idx].parent  = 11;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA);
    layout_array_dnxf[idx].name = "dnxf_state__hard_reset__userdata";
    layout_array_dnxf[idx].type = "uint32*";
    layout_array_dnxf[idx].doc = "user data pointer";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(uint32*);
    layout_array_dnxf[idx].parent  = 11;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP);
    layout_array_dnxf[idx].name = "dnxf_state__mib__counter_pbmp";
    layout_array_dnxf[idx].type = "bcm_pbmp_t";
    layout_array_dnxf[idx].doc = "MIB counter bitmap Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(bcm_pbmp_t);
    layout_array_dnxf[idx].parent  = 12;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL);
    layout_array_dnxf[idx].name = "dnxf_state__mib__interval";
    layout_array_dnxf[idx].type = "int";
    layout_array_dnxf[idx].doc = "MIB counter interval Data Base";
    layout_array_dnxf[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].size_of = sizeof(int);
    layout_array_dnxf[idx].parent  = 12;
    layout_array_dnxf[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnxf[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNXF_STATE, layout_array_dnxf, sw_state_layout_array[unit][DNXF_MODULE_ID], DNX_SW_STATE_DNXF_STATE_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
