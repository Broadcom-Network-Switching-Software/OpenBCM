
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
#include <soc/dnx/swstate/auto_generated/diagnostic/adapter_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern adapter_basic_access_info_t * adapter_root[SOC_MAX_NUM_DEVICES];
extern adapter_basic_access_info_t * adapter_context_db_data[SOC_MAX_NUM_DEVICES];



int
adapter_context_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(adapter_context_db_params_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_dump(int  unit,  int  params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(adapter_context_db_params_adapter_rx_tx_fd_dump(unit, params_idx_0, filters));
    SHR_IF_ERR_EXIT(adapter_context_db_params_adapter_rx_tx_mutex_dump(unit, params_idx_0, filters));
    SHR_IF_ERR_EXIT(adapter_context_db_params_adapter_mem_reg_fd_dump(unit, params_idx_0, filters));
    SHR_IF_ERR_EXIT(adapter_context_db_params_adapter_mem_reg_mutex_dump(unit, params_idx_0, filters));
    SHR_IF_ERR_EXIT(adapter_context_db_params_adapter_sdk_interface_fd_dump(unit, params_idx_0, filters));
    SHR_IF_ERR_EXIT(adapter_context_db_params_adapter_sdk_interface_mutex_dump(unit, params_idx_0, filters));
    SHR_IF_ERR_EXIT(adapter_context_db_params_adapter_external_events_fd_dump(unit, params_idx_0, filters));
    SHR_IF_ERR_EXIT(adapter_context_db_params_adapter_external_events_mutex_dump(unit, params_idx_0, filters));
    SHR_IF_ERR_EXIT(adapter_context_db_params_adapter_external_events_port_dump(unit, params_idx_0, filters));
    SHR_IF_ERR_EXIT(adapter_context_db_params_adapter_server_address_dump(unit, params_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_rx_tx_fd_dump(int  unit,  int  params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_rx_tx_mutex_dump(int  unit,  int  params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = params_idx_0, I0 = params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_mutex_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "adapter params adapter_rx_tx_mutex") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate adapter params adapter_rx_tx_mutex\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "adapter_context_db/params/adapter_rx_tx_mutex.txt",
            "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[params_idx_0].adapter_rx_tx_mutex: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit])->params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ADAPTER_MODULE_ID,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params
                , sizeof(*((adapter_basic_access_info_t*)adapter_root[unit])->params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params))
        {
            LOG_CLI((BSL_META("adapter_context_db[]->((adapter_basic_access_info_t*)adapter_root[])->params[params_idx_0].adapter_rx_tx_mutex: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[i0].adapter_rx_tx_mutex: ");

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[i0]: ");

            dnx_sw_state_print_mutex(
                unit,
                &((adapter_basic_access_info_t*)adapter_root[unit])->params[i0].adapter_rx_tx_mutex);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_mem_reg_fd_dump(int  unit,  int  params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_mem_reg_mutex_dump(int  unit,  int  params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = params_idx_0, I0 = params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_mutex_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "adapter params adapter_mem_reg_mutex") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate adapter params adapter_mem_reg_mutex\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "adapter_context_db/params/adapter_mem_reg_mutex.txt",
            "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[params_idx_0].adapter_mem_reg_mutex: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit])->params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ADAPTER_MODULE_ID,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params
                , sizeof(*((adapter_basic_access_info_t*)adapter_root[unit])->params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params))
        {
            LOG_CLI((BSL_META("adapter_context_db[]->((adapter_basic_access_info_t*)adapter_root[])->params[params_idx_0].adapter_mem_reg_mutex: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[i0].adapter_mem_reg_mutex: ");

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[i0]: ");

            dnx_sw_state_print_mutex(
                unit,
                &((adapter_basic_access_info_t*)adapter_root[unit])->params[i0].adapter_mem_reg_mutex);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_sdk_interface_fd_dump(int  unit,  int  params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_sdk_interface_mutex_dump(int  unit,  int  params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = params_idx_0, I0 = params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_mutex_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "adapter params adapter_sdk_interface_mutex") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate adapter params adapter_sdk_interface_mutex\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "adapter_context_db/params/adapter_sdk_interface_mutex.txt",
            "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[params_idx_0].adapter_sdk_interface_mutex: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit])->params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ADAPTER_MODULE_ID,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params
                , sizeof(*((adapter_basic_access_info_t*)adapter_root[unit])->params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params))
        {
            LOG_CLI((BSL_META("adapter_context_db[]->((adapter_basic_access_info_t*)adapter_root[])->params[params_idx_0].adapter_sdk_interface_mutex: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[i0].adapter_sdk_interface_mutex: ");

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[i0]: ");

            dnx_sw_state_print_mutex(
                unit,
                &((adapter_basic_access_info_t*)adapter_root[unit])->params[i0].adapter_sdk_interface_mutex);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_external_events_fd_dump(int  unit,  int  params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_external_events_mutex_dump(int  unit,  int  params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = params_idx_0, I0 = params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_mutex_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "adapter params adapter_external_events_mutex") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate adapter params adapter_external_events_mutex\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "adapter_context_db/params/adapter_external_events_mutex.txt",
            "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[params_idx_0].adapter_external_events_mutex: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit])->params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ADAPTER_MODULE_ID,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params
                , sizeof(*((adapter_basic_access_info_t*)adapter_root[unit])->params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params))
        {
            LOG_CLI((BSL_META("adapter_context_db[]->((adapter_basic_access_info_t*)adapter_root[])->params[params_idx_0].adapter_external_events_mutex: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[i0].adapter_external_events_mutex: ");

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[i0]: ");

            dnx_sw_state_print_mutex(
                unit,
                &((adapter_basic_access_info_t*)adapter_root[unit])->params[i0].adapter_external_events_mutex);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_external_events_port_dump(int  unit,  int  params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = params_idx_0, I0 = params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "adapter params adapter_external_events_port") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate adapter params adapter_external_events_port\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "adapter_context_db/params/adapter_external_events_port.txt",
            "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[params_idx_0].adapter_external_events_port: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit])->params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ADAPTER_MODULE_ID,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params
                , sizeof(*((adapter_basic_access_info_t*)adapter_root[unit])->params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params))
        {
            LOG_CLI((BSL_META("adapter_context_db[]->((adapter_basic_access_info_t*)adapter_root[])->params[params_idx_0].adapter_external_events_port: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[i0].adapter_external_events_port: ");

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[i0]: ");

            dnx_sw_state_print_uint32(
                unit,
                &((adapter_basic_access_info_t*)adapter_root[unit])->params[i0].adapter_external_events_port);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
adapter_context_db_params_adapter_server_address_dump(int  unit,  int  params_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = params_idx_0, I0 = params_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "adapter params adapter_server_address") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate adapter params adapter_server_address\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "adapter_context_db/params/adapter_server_address.txt",
            "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[params_idx_0].adapter_server_address: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((adapter_basic_access_info_t*)adapter_root[unit])->params);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                ADAPTER_MODULE_ID,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((adapter_basic_access_info_t*)adapter_root[unit])->params
                , sizeof(*((adapter_basic_access_info_t*)adapter_root[unit])->params), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params))
        {
            LOG_CLI((BSL_META("adapter_context_db[]->((adapter_basic_access_info_t*)adapter_root[])->params[params_idx_0].adapter_server_address: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, ADAPTER_MODULE_ID, ((adapter_basic_access_info_t*)adapter_root[unit])->params) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "adapter_context_db[%d]->","((adapter_basic_access_info_t*)adapter_root[%d])->","params[i0].adapter_server_address: ");

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[i0]: ");

            dnx_sw_state_print_uint32(
                unit,
                &((adapter_basic_access_info_t*)adapter_root[unit])->params[i0].adapter_server_address);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t adapter_context_db_info[SOC_MAX_NUM_DEVICES][ADAPTER_CONTEXT_DB_INFO_NOF_ENTRIES];
const char* adapter_context_db_layout_str[ADAPTER_CONTEXT_DB_INFO_NOF_ENTRIES] = {
    "ADAPTER_CONTEXT_DB~",
    "ADAPTER_CONTEXT_DB~PARAMS~",
    "ADAPTER_CONTEXT_DB~PARAMS~ADAPTER_RX_TX_FD~",
    "ADAPTER_CONTEXT_DB~PARAMS~ADAPTER_RX_TX_MUTEX~",
    "ADAPTER_CONTEXT_DB~PARAMS~ADAPTER_MEM_REG_FD~",
    "ADAPTER_CONTEXT_DB~PARAMS~ADAPTER_MEM_REG_MUTEX~",
    "ADAPTER_CONTEXT_DB~PARAMS~ADAPTER_SDK_INTERFACE_FD~",
    "ADAPTER_CONTEXT_DB~PARAMS~ADAPTER_SDK_INTERFACE_MUTEX~",
    "ADAPTER_CONTEXT_DB~PARAMS~ADAPTER_EXTERNAL_EVENTS_FD~",
    "ADAPTER_CONTEXT_DB~PARAMS~ADAPTER_EXTERNAL_EVENTS_MUTEX~",
    "ADAPTER_CONTEXT_DB~PARAMS~ADAPTER_EXTERNAL_EVENTS_PORT~",
    "ADAPTER_CONTEXT_DB~PARAMS~ADAPTER_SERVER_ADDRESS~",
};
#endif 
#undef BSL_LOG_MODULE
