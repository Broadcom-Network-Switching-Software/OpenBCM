

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM


#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx_dispatch.h>

#include <sal/appl/sal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pvt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <sal/core/boot.h>
#include "hbmc_monitor.h"
#include "hbmc_dbal_access.h"
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/dram/hbmc/hbmc_firmware.h>
#include <bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/dram/buffers/dram_buffers.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_hard_reset_access.h>






typedef struct dnx_hbmc_temp_stub_s
{
    uint32 value;
    struct dnx_hbmc_temp_stub_s *next;
} dnx_hbmc_temp_stub_t;

dnx_hbmc_temp_stub_t *dnx_hbmc_temp_stub_head[BCM_LOCAL_UNITS_MAX] = { NULL };


shr_error_e
dnx_hbmc_temp_get(
    int unit,
    int interface_id,
    int *value)
{
    uint32 dram_bitmap[1] = { 0 };
    uint32 nof_hbms = dnx_data_dram.general_info.max_nof_drams_get(unit);
    int hbm_iter;
    uint32 final_temp = 0;
    SHR_FUNC_INIT_VARS(unit);

    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    if (!
        (((interface_id >= 0) && (interface_id < nof_hbms) && (SHR_BITGET(dram_bitmap, interface_id)))
         || (interface_id == DNX_HBMC_ITER_ALL)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid interface_id=%d\n", interface_id);
    }

    
    if (dnx_hbmc_temp_stub_head[unit] != NULL)
    {
        dnx_hbmc_temp_stub_t *remove_stub = dnx_hbmc_temp_stub_head[unit];
        *value = dnx_hbmc_temp_stub_head[unit]->value;
        dnx_hbmc_temp_stub_head[unit] = dnx_hbmc_temp_stub_head[unit]->next;
        sal_free(remove_stub);
    }
    else if (SAL_BOOT_PLISIM)
    {
        
        *value = 30;
        SHR_EXIT();
    }
    else
    {
        SHR_BIT_ITER(dram_bitmap, nof_hbms, hbm_iter)
        {
            uint32 this_hbm_temp_read = 0;

            
            if (interface_id != DNX_HBMC_ITER_ALL && interface_id != hbm_iter)
            {
                continue;
            }

            
            if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
            {
                dnx_hbmc_firmware_operation_result_hbm_spare_t hbm_spare;

                SHR_IF_ERR_EXIT(dnx_hbmc_firmware_launch_operation
                                (unit, hbm_iter, DNX_HBMC_FIRMWARE_ALL_CHANNELS,
                                 DNX_HBMC_FIRMWARE_OPERATION_READ_DEVICE_TEMP));
                SHR_IF_ERR_EXIT(dnx_hbmc_firmware_operation_result_spare_get(unit, hbm_iter, 1, &hbm_spare));
                this_hbm_temp_read = hbm_spare.hbm_spare_data[0];
            }
            
            else
            {
                SHR_IF_ERR_EXIT(dnx_drv_soc_dnx_read_hbm_temp(unit, hbm_iter, &this_hbm_temp_read));
            }

            final_temp = UTILEX_MAX(final_temp, this_hbm_temp_read);
        }
        *value = final_temp;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_traffic_to_hbm_restore(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_usage_restore(unit));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_hbmc_traffic_is_empty_verify(
    int unit,
    uint8 *hbm_is_empty)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(hbm_is_empty, _SHR_E_PARAM, "hbm_is_empty");

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_traffic_to_hbm_stop(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_dram_usage_stop(unit));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_traffic_is_empty_get(
    int unit,
    uint8 *hbm_is_empty)
{
    uint32 nof_deleted_buffers;
    uint32 nof_buffers;
    uint32 nof_free_buffers;
    uint32 bdbs_per_core;
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_hbmc_traffic_is_empty_verify(unit, hbm_is_empty));

    
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_buffer_flush_all(unit));

    
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_deleted_buffers_get(unit, &nof_deleted_buffers));

    
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_nof_free_buffers_get(unit, &nof_free_buffers));

    
    SHR_IF_ERR_EXIT(dnx_dram_buffers_nof_bdbs_per_core_get(unit, &bdbs_per_core));
    nof_buffers = nof_cores * bdbs_per_core;
    if (nof_buffers - nof_deleted_buffers > nof_free_buffers)
    {
        *hbm_is_empty = FALSE;
    }
    else if (nof_buffers - nof_deleted_buffers < nof_free_buffers)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "number of free buffers exceed number of buffers in the system, something is wrong\n");
    }
    else
    {
        *hbm_is_empty = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_temp_log(
    int unit,
    void *user_data)
{
    int nof_pvt_mon = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
    bcm_switch_temperature_monitor_t *temperature_array_p = NULL;

    int temperature_count;
    uint32 power_on;
    int temp = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(temperature_array_p, sizeof(*temperature_array_p) * nof_pvt_mon, "temperature_array_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    
    SHR_IF_ERR_EXIT(dnx_dram_db.power_down_callback.power_on.get(unit, &power_on));

    
    if (power_on)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_hbmc_temp_get(unit, DNX_HBMC_ITER_ALL, &temp),
                                 "HBM temperature monitoring failed, temp read was not successful %s%s%s\n", EMPTY,
                                 EMPTY, EMPTY);
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "HBM temperature: %d C\n"), temp));
    }

    
    SHR_IF_ERR_EXIT(bcm_dnx_switch_temperature_monitor_get(unit, nof_pvt_mon, temperature_array_p, &temperature_count));
    for (int sensor = 0; sensor < temperature_count; ++sensor)
    {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Device sensor #%d temperature: %d.%d C\n"), sensor,
                     temperature_array_p[sensor].curr / 10, temperature_array_p[sensor].curr % 10));
    }

exit:
    SHR_FREE(temperature_array_p);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_temp_monitor(
    int unit,
    void *user_data)
{
    dnx_dram_temperature_monitoring_state_e previous_state;
    periodic_event_handler_t temp_monitor_handler;
    int temp = 0;
    uint8 hbm_is_empty;
    int nof_pvt_mon = dnx_data_pvt.general.nof_pvt_monitors_get(unit);
    bcm_switch_temperature_monitor_t *temperature_array_p = NULL;
    int temperature_count;
    int power_down_temperature_threshold_detected = 0;
    int stop_traffic_temperature_threshold_detected = 0;
    int restore_traffic_temperature_threshold_detected = 0;
    bcm_switch_hard_reset_callback_t hard_reset_callback;
    dnx_hard_reset_callback_userdata_t callback_userdata;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(temperature_array_p, sizeof(*temperature_array_p) * nof_pvt_mon, "temperature_array_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);
    
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_hbmc_temp_get(unit, DNX_HBMC_ITER_ALL, &temp),
                             "HBM temperature monitoring failed, temp read was not successful %s%s%s\n", EMPTY, EMPTY,
                             EMPTY);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "HBM temperature: %d C\n"), temp));
    
    SHR_IF_ERR_EXIT(bcm_dnx_switch_temperature_monitor_get(unit, nof_pvt_mon, temperature_array_p, &temperature_count));
    for (int sensor = 0; sensor < temperature_count; ++sensor)
    {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Device sensor #%d temperature: %d.%d C\n"), sensor,
                     temperature_array_p[sensor].curr / 10, temperature_array_p[sensor].curr % 10));
    }
    
    SHR_IF_ERR_EXIT(dnx_dram_db.temperature_monitoring_thread_state.get(unit, &previous_state));
    if (temp >= dnx_data_dram.hbm.power_down_temp_threshold_get(unit))
    {
        
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "HBM temperature exceeded power-down threshold - %d C, shutting down the HBM\n"),
                  dnx_data_dram.hbm.power_down_temp_threshold_get(unit)));
        power_down_temperature_threshold_detected = 1;
    }
    else if ((previous_state == DNX_DRAM_NORMAL_TEMP)
             && (temp >= dnx_data_dram.hbm.stop_traffic_temp_threshold_get(unit)))
    {
        
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "HBM temperature exceeded stop-traffic threshold - %d C, traffic is routed through OCB only\n"),
                  dnx_data_dram.hbm.stop_traffic_temp_threshold_get(unit)));
        
        SHR_IF_ERR_EXIT(dnx_dram_db.temperature_monitoring_thread_state.set(unit, DNX_DRAM_HIGH_TEMP));
        stop_traffic_temperature_threshold_detected = 1;
    }
    else if ((previous_state == DNX_DRAM_HIGH_TEMP)
             && (temp <= dnx_data_dram.hbm.restore_traffic_temp_threshold_get(unit)))
    {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "HBM temperature dropped below restore-traffic threshold - %d C, traffic can be routed through both HBM and OCB\n"),
                  dnx_data_dram.hbm.restore_traffic_temp_threshold_get(unit)));
        restore_traffic_temperature_threshold_detected = 1;
    }
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_low_temperature_dram_protction))
    {
        
        if (temp <= dnx_data_dram.hbm.power_down_low_temp_threshold_get(unit))
        {
            
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit, "HBM temperature fell below power down threshold - %d C, shutting down the HBM\n"),
                      dnx_data_dram.hbm.power_down_low_temp_threshold_get(unit)));
            power_down_temperature_threshold_detected = 1;
        }
        else if ((previous_state == DNX_DRAM_NORMAL_TEMP)
                 && (temp <= dnx_data_dram.hbm.stop_traffic_low_temp_threshold_get(unit)))
        {
            
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit,
                       "HBM temperature fell below stop traffic threshold - %d C, traffic is routed through OCB only\n"),
                      dnx_data_dram.hbm.stop_traffic_low_temp_threshold_get(unit)));
            
            SHR_IF_ERR_EXIT(dnx_dram_db.temperature_monitoring_thread_state.set(unit, DNX_DRAM_LOW_TEMP));
            stop_traffic_temperature_threshold_detected = 1;
        }
        else if ((previous_state == DNX_DRAM_LOW_TEMP)
                 && (temp >= dnx_data_dram.hbm.restore_traffic_low_temp_threshold_get(unit)))
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit,
                       "HBM temperature exceeded restore traffic threshold - %d C, traffic can be routed through both HBM and OCB\n"),
                      dnx_data_dram.hbm.restore_traffic_low_temp_threshold_get(unit)));
            restore_traffic_temperature_threshold_detected = 1;
        }
    }
    if (power_down_temperature_threshold_detected)
    {
        
        
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_hbmc_traffic_to_hbm_stop(unit),
                                 "HBM temperature monitoring failed%s%s%s\n", EMPTY, EMPTY, EMPTY);

        
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_hbmc_power_down(unit),
                                 "HBM power down failed, failed to shut down power to HBM%s%s%s\n", EMPTY, EMPTY,
                                 EMPTY);

        
        SHR_IF_ERR_EXIT(dnx_dram_db.temperature_monitoring_thread_handler.get(unit, &temp_monitor_handler));
        SHR_IF_ERR_EXIT(periodic_event_stop(temp_monitor_handler, 1));
        
        SHR_IF_ERR_EXIT(dnx_hbmc_traffic_is_empty_get(unit, &hbm_is_empty));
        
        if (!hbm_is_empty)
        {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit, "%s(): HBM failed to get empty prior to shutting down, Hard reset is required.\n"),
                      FUNCTION_NAME()));
            
            SHR_IF_ERR_EXIT(dnx_hard_reset_db.hard_reset_callback.callback.get(unit, &hard_reset_callback));
            SHR_IF_ERR_EXIT(dnx_hard_reset_db.hard_reset_callback.userdata.get(unit, &callback_userdata));
            hard_reset_callback(unit, 0, callback_userdata);
        }
    }
    else if (stop_traffic_temperature_threshold_detected)
    {
        
        
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_hbmc_traffic_to_hbm_stop(unit),
                                 "HBM temperature monitoring failed%s%s%s\n", EMPTY, EMPTY, EMPTY);

        
    }
    else if (restore_traffic_temperature_threshold_detected)
    {
        
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_hbmc_traffic_to_hbm_restore(unit),
                                 "HBM temperature monitoring failed\n%s%s%s", EMPTY, EMPTY, EMPTY);
        
        SHR_IF_ERR_EXIT(dnx_dram_db.temperature_monitoring_thread_state.set(unit, DNX_DRAM_NORMAL_TEMP));
    }

exit:
    SHR_FREE(temperature_array_p);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_hbmc_temp_stub_set(
    int unit,
    int nof_elements,
    uint32 *temp_arr)
{
    dnx_hbmc_temp_stub_t *cur_element = NULL;
    dnx_hbmc_temp_stub_t *temp_element = NULL;
    dnx_hbmc_temp_stub_t *first_element = NULL;
    int i;
    SHR_FUNC_INIT_VARS(unit);
    
    if (dnx_hbmc_temp_stub_head[unit] != NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dnx_hbmc_temp_stub_head!=NULL, cannot configure new stub\n");
    }
    
    if (nof_elements == 0)
    {
        SHR_EXIT();
    }

    
    for (i = 0; i < nof_elements; i++)
    {
        temp_element = (dnx_hbmc_temp_stub_t *) sal_alloc(sizeof(dnx_hbmc_temp_stub_t), "dnx_hbm_tem_stub");
        if (temp_element == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "sal_alloc returned NULL pointer for index=%d\n", i);
        }
        temp_element->value = temp_arr[i];
        temp_element->next = NULL;
        
        if (i == 0)
        {
            first_element = temp_element;
        }
        if (cur_element != NULL)
        {
            cur_element->next = temp_element;
        }
        cur_element = temp_element;
    }

exit:
    
    
     
    dnx_hbmc_temp_stub_head[unit] = first_element;
    SHR_FUNC_EXIT;
}
