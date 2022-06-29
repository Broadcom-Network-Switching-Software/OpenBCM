
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/reflector_types.h>
#include <soc/dnx/swstate/auto_generated/layout/reflector_layout.h>

dnxc_sw_state_layout_t layout_array_reflector[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO_NOF_PARAMS)];

shr_error_e
reflector_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO);
    layout_array_reflector[idx].name = "reflector_sw_db_info";
    layout_array_reflector[idx].type = "reflector_sw_db_info_t";
    layout_array_reflector[idx].doc = "A struct used to hold reflector SW info";
    layout_array_reflector[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].size_of = sizeof(reflector_sw_db_info_t);
    layout_array_reflector[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__FIRST);
    layout_array_reflector[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC);
    layout_array_reflector[idx].name = "reflector_sw_db_info__uc";
    layout_array_reflector[idx].type = "reflector_uc_sw_db_info_t";
    layout_array_reflector[idx].doc = "L3 UC Reflector / L2 UC external reflector SW db info";
    layout_array_reflector[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].size_of = sizeof(reflector_uc_sw_db_info_t);
    layout_array_reflector[idx].parent  = 0;
    layout_array_reflector[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC__FIRST);
    layout_array_reflector[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC_L2_INT);
    layout_array_reflector[idx].name = "reflector_sw_db_info__uc_l2_int";
    layout_array_reflector[idx].type = "reflector_uc_sw_db_info_t";
    layout_array_reflector[idx].doc = "L2 UC internal Reflector SW db info";
    layout_array_reflector[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].size_of = sizeof(reflector_uc_sw_db_info_t);
    layout_array_reflector[idx].parent  = 0;
    layout_array_reflector[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC_L2_INT__FIRST);
    layout_array_reflector[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC_L2_INT__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC_L2_INT_ONEPASS);
    layout_array_reflector[idx].name = "reflector_sw_db_info__uc_l2_int_onepass";
    layout_array_reflector[idx].type = "reflector_uc_sw_db_info_t";
    layout_array_reflector[idx].doc = "L2 UC internal One-pass Reflector SW db info";
    layout_array_reflector[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].size_of = sizeof(reflector_uc_sw_db_info_t);
    layout_array_reflector[idx].parent  = 0;
    layout_array_reflector[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC_L2_INT_ONEPASS__FIRST);
    layout_array_reflector[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC_L2_INT_ONEPASS__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__SBFD_REFLECTOR_CNT);
    layout_array_reflector[idx].name = "reflector_sw_db_info__sbfd_reflector_cnt";
    layout_array_reflector[idx].type = "uint32";
    layout_array_reflector[idx].doc = "nof sbfd reflector";
    layout_array_reflector[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].size_of = sizeof(uint32);
    layout_array_reflector[idx].parent  = 0;
    layout_array_reflector[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC__ENCAP_ID);
    layout_array_reflector[idx].name = "reflector_sw_db_info__uc__encap_id";
    layout_array_reflector[idx].type = "int";
    layout_array_reflector[idx].doc = "global encap_id allocated for UC reflector (both L2 and L3)";
    layout_array_reflector[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].size_of = sizeof(int);
    layout_array_reflector[idx].parent  = 1;
    layout_array_reflector[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC__IS_ALLOCATED);
    layout_array_reflector[idx].name = "reflector_sw_db_info__uc__is_allocated";
    layout_array_reflector[idx].type = "int";
    layout_array_reflector[idx].doc = "1 if encap for UC reflector allocated";
    layout_array_reflector[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].size_of = sizeof(int);
    layout_array_reflector[idx].parent  = 1;
    layout_array_reflector[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC_L2_INT__ENCAP_ID);
    layout_array_reflector[idx].name = "reflector_sw_db_info__uc_l2_int__encap_id";
    layout_array_reflector[idx].type = "int";
    layout_array_reflector[idx].doc = "global encap_id allocated for UC reflector (both L2 and L3)";
    layout_array_reflector[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].size_of = sizeof(int);
    layout_array_reflector[idx].parent  = 2;
    layout_array_reflector[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC_L2_INT__IS_ALLOCATED);
    layout_array_reflector[idx].name = "reflector_sw_db_info__uc_l2_int__is_allocated";
    layout_array_reflector[idx].type = "int";
    layout_array_reflector[idx].doc = "1 if encap for UC reflector allocated";
    layout_array_reflector[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].size_of = sizeof(int);
    layout_array_reflector[idx].parent  = 2;
    layout_array_reflector[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC_L2_INT_ONEPASS__ENCAP_ID);
    layout_array_reflector[idx].name = "reflector_sw_db_info__uc_l2_int_onepass__encap_id";
    layout_array_reflector[idx].type = "int";
    layout_array_reflector[idx].doc = "global encap_id allocated for UC reflector (both L2 and L3)";
    layout_array_reflector[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].size_of = sizeof(int);
    layout_array_reflector[idx].parent  = 3;
    layout_array_reflector[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_REFLECTOR_SW_DB_INFO__UC_L2_INT_ONEPASS__IS_ALLOCATED);
    layout_array_reflector[idx].name = "reflector_sw_db_info__uc_l2_int_onepass__is_allocated";
    layout_array_reflector[idx].type = "int";
    layout_array_reflector[idx].doc = "1 if encap for UC reflector allocated";
    layout_array_reflector[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].size_of = sizeof(int);
    layout_array_reflector[idx].parent  = 3;
    layout_array_reflector[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_reflector[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_REFLECTOR_SW_DB_INFO, layout_array_reflector, sw_state_layout_array[unit][REFLECTOR_MODULE_ID], DNX_SW_STATE_REFLECTOR_SW_DB_INFO_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
