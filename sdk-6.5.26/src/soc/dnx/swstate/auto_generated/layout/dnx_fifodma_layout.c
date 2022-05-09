
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_fifodma_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_fifodma_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_fifodma[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO_NOF_PARAMS)];

shr_error_e
dnx_fifodma_init_layout_structure(int unit)
{

    shr_thread_manager_handler_t dnx_fifodma_info__fifodma_thread_shr_thread_manager_handler__default_val = NULL;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info";
    layout_array_dnx_fifodma[idx].type = "fifodma_db_t";
    layout_array_dnx_fifodma[idx].doc = "fifodma database";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(fifodma_db_t);
    layout_array_dnx_fifodma[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__FIRST);
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__LAST);

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry";
    layout_array_dnx_fifodma[idx].type = "fifodma_info_t*";
    layout_array_dnx_fifodma[idx].doc = "dnx fifodma configration information.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(fifodma_info_t*);
    layout_array_dnx_fifodma[idx].array_indexes[0].num_elements = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC;
    layout_array_dnx_fifodma[idx].parent  = 0;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FIRST);
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST);
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__FIFODMA_THREAD_SHR_THREAD_MANAGER_HANDLER);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__fifodma_thread_shr_thread_manager_handler";
    layout_array_dnx_fifodma[idx].type = "shr_thread_manager_handler_t";
    layout_array_dnx_fifodma[idx].doc = "Handler for the oam oamp event thread";
    layout_array_dnx_fifodma[idx].default_value= &(dnx_fifodma_info__fifodma_thread_shr_thread_manager_handler__default_val);
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(shr_thread_manager_handler_t);
    layout_array_dnx_fifodma[idx].parent  = 0;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOW_WRITES_DURING_WB | DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MAX_ENTRIES);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__max_entries";
    layout_array_dnx_fifodma[idx].type = "uint32";
    layout_array_dnx_fifodma[idx].doc = "Maximum number of entries.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(uint32);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ENTRY_SIZE);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__entry_size";
    layout_array_dnx_fifodma[idx].type = "uint32";
    layout_array_dnx_fifodma[idx].doc = "Size of entry.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(uint32);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_LO);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__host_buff_lo";
    layout_array_dnx_fifodma[idx].type = "uint32";
    layout_array_dnx_fifodma[idx].doc = "Low 32 bits of host memory buffer pointer";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(uint32);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HOST_BUFF_HI);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__host_buff_hi";
    layout_array_dnx_fifodma[idx].type = "uint32";
    layout_array_dnx_fifodma[idx].doc = "High 32 bits of host memory buffer pointer";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(uint32);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_MEM);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__is_mem";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Boolean value, '0' - register is used; '1' - memory is used.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__MEM);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__mem";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Name of memory.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__REG);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__reg";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Name of register.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__ARRAY_INDEX);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__array_index";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Index of register or memory array.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__COPYNO);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__copyno";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Block parameter in memory access.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__FORCE_ENTRY_SIZE);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__force_entry_size";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Specified entry size which does not match the register or memory size, ignore when it equals to 0.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__THRESHOLD);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__threshold";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Number of valid entries in host memory exceeds which an interrupt will be raised.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__TIMEOUT);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__timeout";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Time before triggering a FIFO_DMA_TIMEOUT interrupt, in microseconds.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_POLL);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__is_poll";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Boolean value, '0' - is in interrupt mode; '1' - is in poll mode.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_ALLOCED);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__is_alloced";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Boolean value, '0' - current fifodma channel is not used; '1' - current fifodma channel is used.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_STARTED);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__is_started";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Boolean value, '0' - current fifodma channel is not started; '1' - current fifodma channel is started.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__IS_PAUSED);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__is_paused";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Boolean value, '0' - current fifodma channel is not paused; '1' - current fifodma channel is paused.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__LAST_ENTRY_ID);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__last_entry_id";
    layout_array_dnx_fifodma[idx].type = "int";
    layout_array_dnx_fifodma[idx].doc = "Current entry location in host memory";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(int);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_FIFODMA_INFO__ENTRY__HANDLER);
    layout_array_dnx_fifodma[idx].name = "dnx_fifodma_info__entry__handler";
    layout_array_dnx_fifodma[idx].type = "sw_state_cb_t";
    layout_array_dnx_fifodma[idx].doc = "Callback function from fifodma users.";
    layout_array_dnx_fifodma[idx].total_size = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].size_of = sizeof(sw_state_cb_t);
    layout_array_dnx_fifodma[idx].parent  = 1;
    layout_array_dnx_fifodma[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_fifodma[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_FIFODMA_INFO, layout_array_dnx_fifodma, sw_state_layout_array[unit][DNX_FIFODMA_MODULE_ID], DNX_SW_STATE_DNX_FIFODMA_INFO_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE
