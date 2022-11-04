
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_ofp_rate_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_ofp_rate_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_ofp_rate[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB_NOF_PARAMS)];

shr_error_e
dnx_ofp_rate_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db";
    layout_array_dnx_ofp_rate[idx].type = "dnx_ofp_rate_db_t";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(dnx_ofp_rate_db_t);
    layout_array_dnx_ofp_rate[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__FIRST);
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__otm";
    layout_array_dnx_ofp_rate[idx].type = "dnx_ofp_rate_db_otm_t";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(dnx_ofp_rate_db_otm_t);
    layout_array_dnx_ofp_rate[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_ofp_rate[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_ofp_rate[idx].array_indexes[1].num_elements = dnx_data_egr_queuing.params.nof_q_pairs_get(unit);
    layout_array_dnx_ofp_rate[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_ofp_rate[idx].parent  = 0;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__FIRST);
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__qpair";
    layout_array_dnx_ofp_rate[idx].type = "dnx_ofp_rate_db_qpair_shaping_t";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(dnx_ofp_rate_db_qpair_shaping_t);
    layout_array_dnx_ofp_rate[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_ofp_rate[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_ofp_rate[idx].array_indexes[1].num_elements = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS;
    layout_array_dnx_ofp_rate[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_ofp_rate[idx].parent  = 0;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__FIRST);
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__TCG);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__tcg";
    layout_array_dnx_ofp_rate[idx].type = "dnx_ofp_rate_db_tcg_shaping_t";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(dnx_ofp_rate_db_tcg_shaping_t);
    layout_array_dnx_ofp_rate[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_ofp_rate[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_ofp_rate[idx].array_indexes[1].num_elements = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS;
    layout_array_dnx_ofp_rate[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_ofp_rate[idx].array_indexes[2].num_elements = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG;
    layout_array_dnx_ofp_rate[idx].array_indexes[2].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_ofp_rate[idx].parent  = 0;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__FIRST);
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__otm_cal";
    layout_array_dnx_ofp_rate[idx].type = "dnx_ofp_rate_db_cal_t";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(dnx_ofp_rate_db_cal_t);
    layout_array_dnx_ofp_rate[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_ofp_rate[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_ofp_rate[idx].array_indexes[1].num_elements = DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS;
    layout_array_dnx_ofp_rate[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_ofp_rate[idx].parent  = 0;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__FIRST);
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__calcal_is_modified";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_ofp_rate[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_ofp_rate[idx].parent  = 0;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__calcal_len";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].array_indexes[0].num_elements = DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES;
    layout_array_dnx_ofp_rate[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_ofp_rate[idx].parent  = 0;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__otm__cal_instance";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].parent  = 1;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__otm__shaping";
    layout_array_dnx_ofp_rate[idx].type = "dnx_ofp_rate_db_otm_shaping_t";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(dnx_ofp_rate_db_otm_shaping_t);
    layout_array_dnx_ofp_rate[idx].parent  = 1;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__FIRST);
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__otm__shaping__rate";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].parent  = 8;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__otm__shaping__burst";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].parent  = 8;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__otm__shaping__valid";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].parent  = 8;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__qpair__rate";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].parent  = 2;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__qpair__valid";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].parent  = 2;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__tcg__rate";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].parent  = 3;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__tcg__valid";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].parent  = 3;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__otm_cal__rate";
    layout_array_dnx_ofp_rate[idx].type = "uint64";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint64);
    layout_array_dnx_ofp_rate[idx].parent  = 4;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__otm_cal__modified";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].parent  = 4;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES);
    layout_array_dnx_ofp_rate[idx].name = "dnx_ofp_rate_db__otm_cal__nof_calcal_instances";
    layout_array_dnx_ofp_rate[idx].type = "uint32";
    layout_array_dnx_ofp_rate[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].size_of = sizeof(uint32);
    layout_array_dnx_ofp_rate[idx].parent  = 4;
    layout_array_dnx_ofp_rate[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_ofp_rate[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_OFP_RATE_DB, layout_array_dnx_ofp_rate, sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID], DNX_SW_STATE_DNX_OFP_RATE_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
