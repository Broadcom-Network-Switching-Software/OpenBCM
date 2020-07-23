
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_fifodma_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern fifodma_db_t * dnx_fifodma_info_data[SOC_MAX_NUM_DEVICES];



int
dnx_fifodma_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_fifodma_info_entry_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_max_entries_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_mem_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_mem_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_reg_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_array_index_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_copyno_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_force_entry_size_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_threshold_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_timeout_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_poll_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_entry_size_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_alloced_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_started_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_is_paused_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_handler_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_last_entry_id_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_host_buff_lo_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_fifodma_info_entry_host_buff_hi_dump(int  unit,  int  entry_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_fifodma_info_info[SOC_MAX_NUM_DEVICES][DNX_FIFODMA_INFO_INFO_NOF_ENTRIES];
const char* dnx_fifodma_info_layout_str[DNX_FIFODMA_INFO_INFO_NOF_ENTRIES] = {
    "DNX_FIFODMA_INFO~",
    "DNX_FIFODMA_INFO~ENTRY~",
    "DNX_FIFODMA_INFO~ENTRY~MAX_ENTRIES~",
    "DNX_FIFODMA_INFO~ENTRY~IS_MEM~",
    "DNX_FIFODMA_INFO~ENTRY~MEM~",
    "DNX_FIFODMA_INFO~ENTRY~REG~",
    "DNX_FIFODMA_INFO~ENTRY~ARRAY_INDEX~",
    "DNX_FIFODMA_INFO~ENTRY~COPYNO~",
    "DNX_FIFODMA_INFO~ENTRY~FORCE_ENTRY_SIZE~",
    "DNX_FIFODMA_INFO~ENTRY~THRESHOLD~",
    "DNX_FIFODMA_INFO~ENTRY~TIMEOUT~",
    "DNX_FIFODMA_INFO~ENTRY~IS_POLL~",
    "DNX_FIFODMA_INFO~ENTRY~ENTRY_SIZE~",
    "DNX_FIFODMA_INFO~ENTRY~IS_ALLOCED~",
    "DNX_FIFODMA_INFO~ENTRY~IS_STARTED~",
    "DNX_FIFODMA_INFO~ENTRY~IS_PAUSED~",
    "DNX_FIFODMA_INFO~ENTRY~HANDLER~",
    "DNX_FIFODMA_INFO~ENTRY~LAST_ENTRY_ID~",
    "DNX_FIFODMA_INFO~ENTRY~HOST_BUFF_LO~",
    "DNX_FIFODMA_INFO~ENTRY~HOST_BUFF_HI~",
};
#endif 
#undef BSL_LOG_MODULE
