
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_iqs_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_iqs_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_iqs[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB_NOF_PARAMS)];

shr_error_e
dnx_iqs_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db";
    layout_array_dnx_iqs[idx].type = "dnx_iqs_db_t";
    layout_array_dnx_iqs[idx].doc = "DB for Ingress Queue Scheduling";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(dnx_iqs_db_t);
    layout_array_dnx_iqs[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].first_child_index = 1;
    layout_array_dnx_iqs[idx].last_child_index = 3;
    layout_array_dnx_iqs[idx].next_node_index = 1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__CREDIT_REQUEST_PROFILE);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__credit_request_profile";
    layout_array_dnx_iqs[idx].type = "dnx_iqs_credit_request_profile_db_t*";
    layout_array_dnx_iqs[idx].doc = "data per credit request profile";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(dnx_iqs_credit_request_profile_db_t*);
    layout_array_dnx_iqs[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_iqs[idx].parent  = 0;
    layout_array_dnx_iqs[idx].first_child_index = 4;
    layout_array_dnx_iqs[idx].last_child_index = 5;
    layout_array_dnx_iqs[idx].next_node_index = 2;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__credit_watchdog";
    layout_array_dnx_iqs[idx].type = "dnx_iqs_credit_watchdog_db_t";
    layout_array_dnx_iqs[idx].doc = "DB for credit watchdog mechanism";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(dnx_iqs_credit_watchdog_db_t);
    layout_array_dnx_iqs[idx].parent  = 0;
    layout_array_dnx_iqs[idx].first_child_index = 6;
    layout_array_dnx_iqs[idx].last_child_index = 10;
    layout_array_dnx_iqs[idx].next_node_index = 3;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__DELETE_CREDIT_REQUEST_PROFILE);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__delete_credit_request_profile";
    layout_array_dnx_iqs[idx].type = "dnx_iqs_delete_credit_request_profile_db_t";
    layout_array_dnx_iqs[idx].doc = "data per delete credit request profile";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(dnx_iqs_delete_credit_request_profile_db_t);
    layout_array_dnx_iqs[idx].parent  = 0;
    layout_array_dnx_iqs[idx].first_child_index = 11;
    layout_array_dnx_iqs[idx].last_child_index = 12;
    layout_array_dnx_iqs[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__CREDIT_REQUEST_PROFILE__VALID);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__credit_request_profile__valid";
    layout_array_dnx_iqs[idx].type = "int";
    layout_array_dnx_iqs[idx].doc = "Will be set to 1, after the user set the thresholds";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(int);
    layout_array_dnx_iqs[idx].parent  = 1;
    layout_array_dnx_iqs[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].next_node_index = 5;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__CREDIT_REQUEST_PROFILE__THRESHOLDS);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__credit_request_profile__thresholds";
    layout_array_dnx_iqs[idx].type = "bcm_cosq_delay_tolerance_t";
    layout_array_dnx_iqs[idx].doc = "store the orignal values (since resolution might be changed)";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(bcm_cosq_delay_tolerance_t);
    layout_array_dnx_iqs[idx].parent  = 1;
    layout_array_dnx_iqs[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG__QUEUE_MIN);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__credit_watchdog__queue_min";
    layout_array_dnx_iqs[idx].type = "int";
    layout_array_dnx_iqs[idx].doc = "min queue included in WD mechanism";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(int);
    layout_array_dnx_iqs[idx].parent  = 2;
    layout_array_dnx_iqs[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].next_node_index = 7;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG__QUEUE_MAX);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__credit_watchdog__queue_max";
    layout_array_dnx_iqs[idx].type = "int";
    layout_array_dnx_iqs[idx].doc = "max queue included in WD mechanism";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(int);
    layout_array_dnx_iqs[idx].parent  = 2;
    layout_array_dnx_iqs[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].next_node_index = 8;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG__ENABLE);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__credit_watchdog__enable";
    layout_array_dnx_iqs[idx].type = "int";
    layout_array_dnx_iqs[idx].doc = "enable / disable WD mechanism";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(int);
    layout_array_dnx_iqs[idx].parent  = 2;
    layout_array_dnx_iqs[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].next_node_index = 9;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG__RETRANSMIT_TH);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__credit_watchdog__retransmit_th";
    layout_array_dnx_iqs[idx].type = "int";
    layout_array_dnx_iqs[idx].doc = "time threshold for retransmit";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(int);
    layout_array_dnx_iqs[idx].parent  = 2;
    layout_array_dnx_iqs[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].next_node_index = 10;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG__DELETE_TH);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__credit_watchdog__delete_th";
    layout_array_dnx_iqs[idx].type = "int*";
    layout_array_dnx_iqs[idx].doc = "delete threshold per profile";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(int*);
    layout_array_dnx_iqs[idx].array_indexes[0].size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_dnx_iqs[idx].parent  = 2;
    layout_array_dnx_iqs[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__DELETE_CREDIT_REQUEST_PROFILE__VALID);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__delete_credit_request_profile__valid";
    layout_array_dnx_iqs[idx].type = "uint32";
    layout_array_dnx_iqs[idx].doc = "Indicates if delete profile ID is valid";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(uint32);
    layout_array_dnx_iqs[idx].parent  = 3;
    layout_array_dnx_iqs[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].next_node_index = 12;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_IQS_DB__DELETE_CREDIT_REQUEST_PROFILE__ID);
    layout_array_dnx_iqs[idx].name = "dnx_iqs_db__delete_credit_request_profile__id";
    layout_array_dnx_iqs[idx].type = "int";
    layout_array_dnx_iqs[idx].doc = "delete profile ID";
    layout_array_dnx_iqs[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].size_of = sizeof(int);
    layout_array_dnx_iqs[idx].parent  = 3;
    layout_array_dnx_iqs[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_iqs[idx].next_node_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_IQS_DB, layout_array_dnx_iqs, sw_state_layout_array[unit][DNX_IQS_MODULE_ID], DNX_SW_STATE_DNX_IQS_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
