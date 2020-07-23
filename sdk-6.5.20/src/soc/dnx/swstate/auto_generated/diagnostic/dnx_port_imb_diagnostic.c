
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_port_imb_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_imbm_info_t * imbm_data[SOC_MAX_NUM_DEVICES];



int
imbm_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(imbm_imbs_in_use_dump(unit, filters));
    SHR_IF_ERR_EXIT(imbm_imb_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(imbm_imb_type_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(imbm_pmd_lock_counter_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(imbm_credit_tx_reset_mutex_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
imbm_imbs_in_use_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_imb imbs_in_use") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_imb imbs_in_use\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "imbm/imbs_in_use.txt",
            "imbm[%d]->","((dnx_imbm_info_t*)sw_state_roots_array[%d][DNX_PORT_IMB_MODULE_ID])->","imbs_in_use: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "imbm[%d]->","((dnx_imbm_info_t*)sw_state_roots_array[%d][DNX_PORT_IMB_MODULE_ID])->","imbs_in_use: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imbs_in_use);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
imbm_imb_dump(int  unit,  int  imb_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
imbm_imb_type_dump(int  unit,  int  imb_type_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = imb_type_idx_0, I0 = imb_type_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "imb_dispatch_type_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_imb imb_type") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_imb imb_type\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "imbm/imb_type.txt",
            "imbm[%d]->","((dnx_imbm_info_t*)sw_state_roots_array[%d][DNX_PORT_IMB_MODULE_ID])->","imb_type[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]));

        if (i0 == -1) {
            I0 = SOC_MAX_NUM_PORTS;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb_type
                , sizeof(*((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb_type), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= SOC_MAX_NUM_PORTS)
        {
            LOG_CLI((BSL_META("imbm[]->((dnx_imbm_info_t*)sw_state_roots_array[][DNX_PORT_IMB_MODULE_ID])->imb_type[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(SOC_MAX_NUM_PORTS == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
                unit,
                ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb_type[i0],
                "imbm[%d]->","((dnx_imbm_info_t*)sw_state_roots_array[%d][DNX_PORT_IMB_MODULE_ID])->","imb_type[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_OPAQUE_FILE(
                unit,
                ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->imb_type[i0],
                    "[%s%d]: ", s0, i0);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
imbm_pmd_lock_counter_dump(int  unit,  int  pmd_lock_counter_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = pmd_lock_counter_idx_0, I0 = pmd_lock_counter_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_imb pmd_lock_counter") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_imb pmd_lock_counter\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "imbm/pmd_lock_counter.txt",
            "imbm[%d]->","((dnx_imbm_info_t*)sw_state_roots_array[%d][DNX_PORT_IMB_MODULE_ID])->","pmd_lock_counter[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]));

        if (i0 == -1) {
            I0 = SOC_MAX_NUM_PORTS;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->pmd_lock_counter
                , sizeof(*((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->pmd_lock_counter), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= SOC_MAX_NUM_PORTS)
        {
            LOG_CLI((BSL_META("imbm[]->((dnx_imbm_info_t*)sw_state_roots_array[][DNX_PORT_IMB_MODULE_ID])->pmd_lock_counter[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(SOC_MAX_NUM_PORTS == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "imbm[%d]->","((dnx_imbm_info_t*)sw_state_roots_array[%d][DNX_PORT_IMB_MODULE_ID])->","pmd_lock_counter[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->pmd_lock_counter[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
imbm_credit_tx_reset_mutex_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_mutex_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_port_imb credit_tx_reset_mutex") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_port_imb credit_tx_reset_mutex\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "imbm/credit_tx_reset_mutex.txt",
            "imbm[%d]->","((dnx_imbm_info_t*)sw_state_roots_array[%d][DNX_PORT_IMB_MODULE_ID])->","credit_tx_reset_mutex: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "imbm[%d]->","((dnx_imbm_info_t*)sw_state_roots_array[%d][DNX_PORT_IMB_MODULE_ID])->","credit_tx_reset_mutex: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_mutex(
            unit,
            &((dnx_imbm_info_t*)sw_state_roots_array[unit][DNX_PORT_IMB_MODULE_ID])->credit_tx_reset_mutex);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t imbm_info[SOC_MAX_NUM_DEVICES][IMBM_INFO_NOF_ENTRIES];
const char* imbm_layout_str[IMBM_INFO_NOF_ENTRIES] = {
    "IMBM~",
    "IMBM~IMBS_IN_USE~",
    "IMBM~IMB~",
    "IMBM~IMB_TYPE~",
    "IMBM~PMD_LOCK_COUNTER~",
    "IMBM~CREDIT_TX_RESET_MUTEX~",
};
#endif 
#undef BSL_LOG_MODULE
