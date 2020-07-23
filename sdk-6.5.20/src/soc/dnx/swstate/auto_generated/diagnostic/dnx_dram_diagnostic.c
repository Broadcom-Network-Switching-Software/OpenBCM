
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
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_dram_diagnostic.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)


extern dnx_dram_db_t * dnx_dram_db_data[SOC_MAX_NUM_DEVICES];



int
dnx_dram_db_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_dram_db_is_initialized_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_sync_manager_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_power_down_callback_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_override_bist_configurations_during_tuning_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_channel_soft_init_after_bist_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_traffic_tuning_core_mode_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_dynamic_calibration_enabled_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_temperature_monitoring_thread_state_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_temperature_monitoring_thread_handler_dump(unit, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_dump(unit, -1, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_gddr6_vendor_info_dump(unit, -1, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_is_initialized_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram is_initialized") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram is_initialized\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/is_initialized.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","is_initialized: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","is_initialized: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->is_initialized);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_sync_manager_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_dram_db_sync_manager_deleted_buffers_file_mutex_dump(unit, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_sync_manager_deleted_buffers_file_mutex_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "sw_state_mutex_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram sync_manager deleted_buffers_file_mutex") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram sync_manager deleted_buffers_file_mutex\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/sync_manager/deleted_buffers_file_mutex.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","sync_manager.deleted_buffers_file_mutex: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","sync_manager.deleted_buffers_file_mutex: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_mutex(
            unit,
            &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->sync_manager.deleted_buffers_file_mutex);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_power_down_callback_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_power_down_callback_callback_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "bcm_switch_dram_power_down_callback_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram power_down_callback callback") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram power_down_callback callback\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/power_down_callback/callback.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","power_down_callback.callback: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.callback,
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","power_down_callback.callback: ");

        DNX_SW_STATE_PRINT_OPAQUE_FILE(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.callback,
                " ");

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_power_down_callback_userdata_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_power_down_callback_userdata_t") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram power_down_callback userdata") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram power_down_callback userdata\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/power_down_callback/userdata.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","power_down_callback.userdata: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.userdata,
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","power_down_callback.userdata: ");

        DNX_SW_STATE_PRINT_OPAQUE_FILE(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.userdata,
                " ");

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_power_down_callback_power_on_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram power_down_callback power_on") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram power_down_callback power_on\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/power_down_callback/power_on.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","power_down_callback.power_on: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","power_down_callback.power_on: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_uint32(
            unit,
            &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->power_down_callback.power_on);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_override_bist_configurations_during_tuning_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram override_bist_configurations_during_tuning") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram override_bist_configurations_during_tuning\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/override_bist_configurations_during_tuning.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","override_bist_configurations_during_tuning: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","override_bist_configurations_during_tuning: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->override_bist_configurations_during_tuning);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_channel_soft_init_after_bist_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram channel_soft_init_after_bist") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram channel_soft_init_after_bist\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/channel_soft_init_after_bist.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","channel_soft_init_after_bist: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","channel_soft_init_after_bist: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->channel_soft_init_after_bist);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_traffic_tuning_core_mode_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram traffic_tuning_core_mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram traffic_tuning_core_mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/traffic_tuning_core_mode.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","traffic_tuning_core_mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","traffic_tuning_core_mode: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->traffic_tuning_core_mode);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_dynamic_calibration_enabled_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram dynamic_calibration_enabled") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram dynamic_calibration_enabled\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/dynamic_calibration_enabled.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","dynamic_calibration_enabled: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_MONITOR(
            unit,
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","dynamic_calibration_enabled: ");

        DNX_SW_STATE_PRINT_FILE(
            unit,
            " ");

        dnx_sw_state_print_int(
            unit,
            &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->dynamic_calibration_enabled);

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_dump(int  unit,  int  dynamic_calibration_was_enabled_after_dram_tune_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = dynamic_calibration_was_enabled_after_dram_tune_idx_0, I0 = dynamic_calibration_was_enabled_after_dram_tune_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "int") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram dynamic_calibration_was_enabled_after_dram_tune") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram dynamic_calibration_was_enabled_after_dram_tune\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/dynamic_calibration_was_enabled_after_dram_tune.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","dynamic_calibration_was_enabled_after_dram_tune[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->dynamic_calibration_was_enabled_after_dram_tune
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->dynamic_calibration_was_enabled_after_dram_tune), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->dynamic_calibration_was_enabled_after_dram_tune[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","dynamic_calibration_was_enabled_after_dram_tune[%s%d]: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_int(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->dynamic_calibration_was_enabled_after_dram_tune[i0]);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_temperature_monitoring_thread_state_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "dnx_dram_temperature_monitoring_state_e") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram temperature_monitoring_thread_state") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram temperature_monitoring_thread_state\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/temperature_monitoring_thread_state.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","temperature_monitoring_thread_state: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        DNX_SW_STATE_PRINT_OPAQUE_MONITOR(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->temperature_monitoring_thread_state,
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","temperature_monitoring_thread_state: ");

        DNX_SW_STATE_PRINT_OPAQUE_FILE(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->temperature_monitoring_thread_state,
                " ");

        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_temperature_monitoring_thread_handler_dump(int  unit,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_is_init_dump(unit, vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_gen2_test_dump(unit, vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_ecc_dump(unit, vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_density_dump(unit, vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_manufacturer_id_dump(unit, vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_manufacturing_location_dump(unit, vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_manufacturing_year_dump(unit, vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_manufacturing_week_dump(unit, vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_serial_number_dump(unit, vendor_info_idx_0, -1, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_addressing_mode_dump(unit, vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_channel_available_dump(unit, vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_hbm_stack_hight_dump(unit, vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_vendor_info_model_part_number_dump(unit, vendor_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_is_init_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info is_init") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info is_init\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/is_init.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].is_init: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].is_init: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].is_init: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].is_init);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_gen2_test_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info gen2_test") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info gen2_test\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/gen2_test.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].gen2_test: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].gen2_test: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].gen2_test: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].gen2_test);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_ecc_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info ecc") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info ecc\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/ecc.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].ecc: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].ecc: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].ecc: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].ecc);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_density_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info density") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info density\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/density.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].density: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].density: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].density: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].density);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturer_id_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info manufacturer_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info manufacturer_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/manufacturer_id.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].manufacturer_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].manufacturer_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].manufacturer_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].manufacturer_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturing_location_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info manufacturing_location") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info manufacturing_location\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/manufacturing_location.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].manufacturing_location: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].manufacturing_location: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].manufacturing_location: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].manufacturing_location);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturing_year_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info manufacturing_year") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info manufacturing_year\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/manufacturing_year.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].manufacturing_year: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].manufacturing_year: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].manufacturing_year: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].manufacturing_year);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_manufacturing_week_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info manufacturing_week") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info manufacturing_week\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/manufacturing_week.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].manufacturing_week: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].manufacturing_week: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].manufacturing_week: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].manufacturing_week);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_serial_number_dump(int  unit,  int  vendor_info_idx_0,  int  serial_number_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    int i1 = serial_number_idx_0, I1 = serial_number_idx_0 + 1, org_i1 = serial_number_idx_0;
    char *s0 = "", *s1 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info serial_number") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info serial_number\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/serial_number.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].serial_number[]: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].serial_number[]: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            if (i1 == -1) {
                I1 = 2;
                i1 = dnx_sw_state_dump_check_all_the_same(unit,
                    ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].serial_number
                    , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].serial_number), I1, &s1) ? I1 - 1 : 0;
            }

            if(i1 >= 2)
            {
                LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].serial_number[]: ")));
                LOG_CLI((BSL_META("Invalid index: %d \n"),i1));
                SHR_EXIT();
            }

            if(2 == 0)
            {
                SHR_EXIT();
            }

            for(; i1 < I1; i1++) {
                dnx_sw_state_dump_update_current_idx(unit, i1);
                DNX_SW_STATE_PRINT_MONITOR(
                    unit,
                    "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].serial_number[%s%d]: ", s0, i0, s1, i1);

                DNX_SW_STATE_PRINT_FILE(
                    unit,
                    "[%s%d][%s%d]: ", s0, i0, s1, i1);

                dnx_sw_state_print_uint32(
                    unit,
                    &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].serial_number[i1]);

            }
            i1 = org_i1;
            dnx_sw_state_dump_end_of_stride(unit);
        }
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_addressing_mode_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info addressing_mode") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info addressing_mode\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/addressing_mode.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].addressing_mode: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].addressing_mode: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].addressing_mode: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].addressing_mode);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_channel_available_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info channel_available") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info channel_available\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/channel_available.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].channel_available: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].channel_available: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].channel_available: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].channel_available);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_hbm_stack_hight_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info hbm_stack_hight") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info hbm_stack_hight\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/hbm_stack_hight.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].hbm_stack_hight: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].hbm_stack_hight: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].hbm_stack_hight: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].hbm_stack_hight);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_vendor_info_model_part_number_dump(int  unit,  int  vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = vendor_info_idx_0, I0 = vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram vendor_info model_part_number") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram vendor_info model_part_number\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/vendor_info/model_part_number.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[].model_part_number: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->vendor_info[].model_part_number: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","vendor_info[%s%d].model_part_number: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->vendor_info[i0].model_part_number);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_dump(int  unit,  int  gddr6_vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    SHR_IF_ERR_EXIT(dnx_dram_db_gddr6_vendor_info_is_init_dump(unit, gddr6_vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_dump(unit, gddr6_vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_gddr6_vendor_info_revision_id_dump(unit, gddr6_vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_gddr6_vendor_info_density_dump(unit, gddr6_vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_gddr6_vendor_info_internal_wck_dump(unit, gddr6_vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_gddr6_vendor_info_wck_granularity_dump(unit, gddr6_vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_gddr6_vendor_info_wck_frequency_dump(unit, gddr6_vendor_info_idx_0, filters));
    SHR_IF_ERR_EXIT(dnx_dram_db_gddr6_vendor_info_vddq_off_dump(unit, gddr6_vendor_info_idx_0, filters));
    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_is_init_dump(int  unit,  int  gddr6_vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = gddr6_vendor_info_idx_0, I0 = gddr6_vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram gddr6_vendor_info is_init") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram gddr6_vendor_info is_init\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/gddr6_vendor_info/is_init.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[].is_init: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[].is_init: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[%s%d].is_init: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[i0].is_init);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_dump(int  unit,  int  gddr6_vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = gddr6_vendor_info_idx_0, I0 = gddr6_vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram gddr6_vendor_info manufacturer_vendor_code") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram gddr6_vendor_info manufacturer_vendor_code\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/gddr6_vendor_info/manufacturer_vendor_code.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[].manufacturer_vendor_code: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[].manufacturer_vendor_code: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[%s%d].manufacturer_vendor_code: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[i0].manufacturer_vendor_code);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_revision_id_dump(int  unit,  int  gddr6_vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = gddr6_vendor_info_idx_0, I0 = gddr6_vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram gddr6_vendor_info revision_id") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram gddr6_vendor_info revision_id\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/gddr6_vendor_info/revision_id.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[].revision_id: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[].revision_id: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[%s%d].revision_id: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[i0].revision_id);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_density_dump(int  unit,  int  gddr6_vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = gddr6_vendor_info_idx_0, I0 = gddr6_vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram gddr6_vendor_info density") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram gddr6_vendor_info density\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/gddr6_vendor_info/density.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[].density: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[].density: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[%s%d].density: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[i0].density);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_internal_wck_dump(int  unit,  int  gddr6_vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = gddr6_vendor_info_idx_0, I0 = gddr6_vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram gddr6_vendor_info internal_wck") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram gddr6_vendor_info internal_wck\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/gddr6_vendor_info/internal_wck.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[].internal_wck: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[].internal_wck: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[%s%d].internal_wck: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[i0].internal_wck);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_wck_granularity_dump(int  unit,  int  gddr6_vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = gddr6_vendor_info_idx_0, I0 = gddr6_vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram gddr6_vendor_info wck_granularity") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram gddr6_vendor_info wck_granularity\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/gddr6_vendor_info/wck_granularity.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[].wck_granularity: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[].wck_granularity: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[%s%d].wck_granularity: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[i0].wck_granularity);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_wck_frequency_dump(int  unit,  int  gddr6_vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = gddr6_vendor_info_idx_0, I0 = gddr6_vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram gddr6_vendor_info wck_frequency") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram gddr6_vendor_info wck_frequency\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/gddr6_vendor_info/wck_frequency.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[].wck_frequency: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[].wck_frequency: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[%s%d].wck_frequency: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[i0].wck_frequency);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_dram_db_gddr6_vendor_info_vddq_off_dump(int  unit,  int  gddr6_vendor_info_idx_0,  dnx_sw_state_dump_filters_t  filters)
{
    int i0 = gddr6_vendor_info_idx_0, I0 = gddr6_vendor_info_idx_0 + 1;
    char *s0 = "";
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (dnx_sw_state_compare(filters.typefilter, "uint32") != TRUE)
    {
         SHR_EXIT();
    }

    if (dnx_sw_state_compare(filters.namefilter, "dnx_dram gddr6_vendor_info vddq_off") != TRUE)
    {
         SHR_EXIT();
    }

    if (filters.nocontent)
    {
         DNX_SW_STATE_PRINT(unit, "swstate dnx_dram gddr6_vendor_info vddq_off\n");
    }
    else
    {
        dnx_sw_state_dump_attach_file(
            unit,
            "dnx_dram_db/gddr6_vendor_info/vddq_off.txt",
            "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[].vddq_off: ");

        DNX_SW_STATE_DUMP_PTR_NULL_CHECK(
            unit,
            ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID]));

        if (i0 == -1) {
            I0 = 2;
            i0 = dnx_sw_state_dump_check_all_the_same(unit,
                ((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info
                , sizeof(*((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info), I0, &s0) ? I0 - 1 : 0;
        }

        if(i0 >= 2)
        {
            LOG_CLI((BSL_META("dnx_dram_db[]->((dnx_dram_db_t*)sw_state_roots_array[][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[].vddq_off: ")));
            LOG_CLI((BSL_META("Invalid index: %d \n"),i0));
            SHR_EXIT();
        }

        if(2 == 0)
        {
            SHR_EXIT();
        }

        for(; i0 < I0; i0++) {
            dnx_sw_state_dump_update_current_idx(unit, i0);
            DNX_SW_STATE_PRINT_MONITOR(
                unit,
                "dnx_dram_db[%d]->","((dnx_dram_db_t*)sw_state_roots_array[%d][DNX_DRAM_MODULE_ID])->","gddr6_vendor_info[%s%d].vddq_off: ", s0, i0);

            DNX_SW_STATE_PRINT_FILE(
                unit,
                "[%s%d]: ", s0, i0);

            dnx_sw_state_print_uint32(
                unit,
                &((dnx_dram_db_t*)sw_state_roots_array[unit][DNX_DRAM_MODULE_ID])->gddr6_vendor_info[i0].vddq_off);

        }
        dnx_sw_state_dump_end_of_stride(unit);
        dnx_sw_state_dump_detach_file(
            unit);

    }

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_sw_state_diagnostic_info_t dnx_dram_db_info[SOC_MAX_NUM_DEVICES][DNX_DRAM_DB_INFO_NOF_ENTRIES];
const char* dnx_dram_db_layout_str[DNX_DRAM_DB_INFO_NOF_ENTRIES] = {
    "DNX_DRAM_DB~",
    "DNX_DRAM_DB~IS_INITIALIZED~",
    "DNX_DRAM_DB~SYNC_MANAGER~",
    "DNX_DRAM_DB~SYNC_MANAGER~DELETED_BUFFERS_FILE_MUTEX~",
    "DNX_DRAM_DB~POWER_DOWN_CALLBACK~",
    "DNX_DRAM_DB~POWER_DOWN_CALLBACK~CALLBACK~",
    "DNX_DRAM_DB~POWER_DOWN_CALLBACK~USERDATA~",
    "DNX_DRAM_DB~POWER_DOWN_CALLBACK~POWER_ON~",
    "DNX_DRAM_DB~OVERRIDE_BIST_CONFIGURATIONS_DURING_TUNING~",
    "DNX_DRAM_DB~CHANNEL_SOFT_INIT_AFTER_BIST~",
    "DNX_DRAM_DB~TRAFFIC_TUNING_CORE_MODE~",
    "DNX_DRAM_DB~DYNAMIC_CALIBRATION_ENABLED~",
    "DNX_DRAM_DB~DYNAMIC_CALIBRATION_WAS_ENABLED_AFTER_DRAM_TUNE~",
    "DNX_DRAM_DB~TEMPERATURE_MONITORING_THREAD_STATE~",
    "DNX_DRAM_DB~TEMPERATURE_MONITORING_THREAD_HANDLER~",
    "DNX_DRAM_DB~VENDOR_INFO~",
    "DNX_DRAM_DB~VENDOR_INFO~IS_INIT~",
    "DNX_DRAM_DB~VENDOR_INFO~GEN2_TEST~",
    "DNX_DRAM_DB~VENDOR_INFO~ECC~",
    "DNX_DRAM_DB~VENDOR_INFO~DENSITY~",
    "DNX_DRAM_DB~VENDOR_INFO~MANUFACTURER_ID~",
    "DNX_DRAM_DB~VENDOR_INFO~MANUFACTURING_LOCATION~",
    "DNX_DRAM_DB~VENDOR_INFO~MANUFACTURING_YEAR~",
    "DNX_DRAM_DB~VENDOR_INFO~MANUFACTURING_WEEK~",
    "DNX_DRAM_DB~VENDOR_INFO~SERIAL_NUMBER~",
    "DNX_DRAM_DB~VENDOR_INFO~ADDRESSING_MODE~",
    "DNX_DRAM_DB~VENDOR_INFO~CHANNEL_AVAILABLE~",
    "DNX_DRAM_DB~VENDOR_INFO~HBM_STACK_HIGHT~",
    "DNX_DRAM_DB~VENDOR_INFO~MODEL_PART_NUMBER~",
    "DNX_DRAM_DB~GDDR6_VENDOR_INFO~",
    "DNX_DRAM_DB~GDDR6_VENDOR_INFO~IS_INIT~",
    "DNX_DRAM_DB~GDDR6_VENDOR_INFO~MANUFACTURER_VENDOR_CODE~",
    "DNX_DRAM_DB~GDDR6_VENDOR_INFO~REVISION_ID~",
    "DNX_DRAM_DB~GDDR6_VENDOR_INFO~DENSITY~",
    "DNX_DRAM_DB~GDDR6_VENDOR_INFO~INTERNAL_WCK~",
    "DNX_DRAM_DB~GDDR6_VENDOR_INFO~WCK_GRANULARITY~",
    "DNX_DRAM_DB~GDDR6_VENDOR_INFO~WCK_FREQUENCY~",
    "DNX_DRAM_DB~GDDR6_VENDOR_INFO~VDDQ_OFF~",
};
#endif 
#undef BSL_LOG_MODULE
