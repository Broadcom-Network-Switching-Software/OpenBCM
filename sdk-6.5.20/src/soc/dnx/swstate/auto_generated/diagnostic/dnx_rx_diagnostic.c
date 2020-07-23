
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_rx_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern rx_paser_info_t * dnx_rx_parser_info_data[SOC_MAX_NUM_DEVICES];



int
dnx_rx_parser_info_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_ftmh_lb_key_size_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_ftmh_stacking_ext_size_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_user_header_enable_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_pph_base_size_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_out_lif_x_1_ext_size_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_out_lif_x_2_ext_size_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_out_lif_x_3_ext_size_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_in_lif_ext_size_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_oamp_system_port_0_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_oamp_system_port_1_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_dma_channel_to_cpu_port_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_rx_parser_info_cpu_port_to_dma_channel_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_ftmh_lb_key_size_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_ftmh_stacking_ext_size_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_user_header_enable_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_pph_base_size_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_out_lif_x_1_ext_size_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_out_lif_x_2_ext_size_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_out_lif_x_3_ext_size_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_in_lif_ext_size_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_oamp_system_port_0_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_oamp_system_port_1_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_dma_channel_to_cpu_port_dump(int  unit,  int  dma_channel_to_cpu_port_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dma_channel_to_cpu_port_idx_0, I0 = dma_channel_to_cpu_port_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_rx dma_channel_to_cpu_port") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_rx dma_channel_to_cpu_port\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_rx_parser_info/dma_channel_to_cpu_port.txt",
            "dnx_rx_parser_info[%d]->","((rx_paser_info_t*)sw_state_roots_array[%d][DNX_RX_MODULE_ID])->","dma_channel_to_cpu_port[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->dma_channel_to_cpu_port);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_RX_MODULE_ID,
                ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->dma_channel_to_cpu_port);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->dma_channel_to_cpu_port
                , sizeof(*((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->dma_channel_to_cpu_port), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_RX_MODULE_ID, ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->dma_channel_to_cpu_port))
        {
            LOG_CLI((BSL_META("dnx_rx_parser_info[]->((rx_paser_info_t*)sw_state_roots_array[][DNX_RX_MODULE_ID])->dma_channel_to_cpu_port[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_RX_MODULE_ID, ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->dma_channel_to_cpu_port) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_rx_parser_info[%d]->","((rx_paser_info_t*)sw_state_roots_array[%d][DNX_RX_MODULE_ID])->","dma_channel_to_cpu_port[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->dma_channel_to_cpu_port[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_rx_parser_info_cpu_port_to_dma_channel_dump(int  unit,  int  cpu_port_to_dma_channel_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = cpu_port_to_dma_channel_idx_0, I0 = cpu_port_to_dma_channel_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_rx cpu_port_to_dma_channel") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_rx cpu_port_to_dma_channel\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_rx_parser_info/cpu_port_to_dma_channel.txt",
            "dnx_rx_parser_info[%d]->","((rx_paser_info_t*)sw_state_roots_array[%d][DNX_RX_MODULE_ID])->","cpu_port_to_dma_channel[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID]));

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->cpu_port_to_dma_channel);

        if (i0 == -1) {
            I0 = dnx_sw_state_get_nof_elements(unit,
                DNX_RX_MODULE_ID,
                ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->cpu_port_to_dma_channel);
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->cpu_port_to_dma_channel
                , sizeof(*((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->cpu_port_to_dma_channel), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= dnx_sw_state_get_nof_elements(unit, DNX_RX_MODULE_ID, ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->cpu_port_to_dma_channel))
        {
            LOG_CLI((BSL_META("dnx_rx_parser_info[]->((rx_paser_info_t*)sw_state_roots_array[][DNX_RX_MODULE_ID])->cpu_port_to_dma_channel[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(dnx_sw_state_get_nof_elements(unit, DNX_RX_MODULE_ID, ((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->cpu_port_to_dma_channel) == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_rx_parser_info[%d]->","((rx_paser_info_t*)sw_state_roots_array[%d][DNX_RX_MODULE_ID])->","cpu_port_to_dma_channel[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((rx_paser_info_t*)sw_state_roots_array[unit][DNX_RX_MODULE_ID])->cpu_port_to_dma_channel[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_rx_parser_info_info[SOC_MAX_NUM_DEVICES][DNX_RX_PARSER_INFO_INFO_NOF_ENTRIES];
const char* dnx_rx_parser_info_layout_str[DNX_RX_PARSER_INFO_INFO_NOF_ENTRIES] = {
    "DNX_RX_PARSER_INFO~",
    "DNX_RX_PARSER_INFO~FTMH_LB_KEY_SIZE~",
    "DNX_RX_PARSER_INFO~FTMH_STACKING_EXT_SIZE~",
    "DNX_RX_PARSER_INFO~USER_HEADER_ENABLE~",
    "DNX_RX_PARSER_INFO~PPH_BASE_SIZE~",
    "DNX_RX_PARSER_INFO~OUT_LIF_X_1_EXT_SIZE~",
    "DNX_RX_PARSER_INFO~OUT_LIF_X_2_EXT_SIZE~",
    "DNX_RX_PARSER_INFO~OUT_LIF_X_3_EXT_SIZE~",
    "DNX_RX_PARSER_INFO~IN_LIF_EXT_SIZE~",
    "DNX_RX_PARSER_INFO~OAMP_SYSTEM_PORT_0~",
    "DNX_RX_PARSER_INFO~OAMP_SYSTEM_PORT_1~",
    "DNX_RX_PARSER_INFO~DMA_CHANNEL_TO_CPU_PORT~",
    "DNX_RX_PARSER_INFO~CPU_PORT_TO_DMA_CHANNEL~",
};
#endif 
#undef BSL_LOG_MODULE
