

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM



#include <sal/appl/sal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/dram/hbmc/hbmc_firmware.h>
#include <bcm_int/dnx/dram/gddr6/gddr6.h>
#include <bcm_int/dnx/dram/dram.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_aux_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>







extern int soc_dnx_read_hbm_device_id(
    const int unit,
    const unsigned hbm_number,
    soc_dnx_hbm_device_id_t * id);


shr_error_e
dnx_dram_power_down_cb_register(
    int unit,
    uint32 flags,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_power_down_cb_register(unit, flags, callback, userdata));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unavailable functionality\n");
    }
exit:
    SHR_FUNC_EXIT;
}

 
shr_error_e
dnx_dram_power_down_cb_unregister(
    int unit,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_power_down_cb_unregister(unit, callback, userdata));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unavailable functionality\n");
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_power_down(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        int temp_monitoring_is_active;

        
        SHR_IF_ERR_EXIT(dnx_hbmc_temp_monitor_is_active(unit, &temp_monitoring_is_active));
        if (temp_monitoring_is_active)
        {
            SHR_IF_ERR_EXIT(dnx_hbmc_temp_monitor_pause(unit));
        }
        
        SHR_IF_ERR_EXIT(dnx_hbmc_power_down(unit));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unavailable functionality\n");
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_reinit(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_reinit(unit, flags));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unavailable functionality\n");
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_traffic_enable_set(
    int unit,
    uint32 flags,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_redirect_traffic_to_ocb(unit, (enable == FALSE), NULL));
    }
    else if (TRUE == dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_redirect_traffic_to_ocb(unit, (enable == FALSE), NULL));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unavailable functionality\n");
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_is_traffic_allowed_into_dram(
    int unit,
    uint8 *allow_traffic_to_dram)
{
    int dram_in_use;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_db.sync_manager.dram_in_use.get(unit, &dram_in_use));
    
    *allow_traffic_to_dram = ! !dram_in_use;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_thermo_sensor_read(
    int unit,
    int interface_id,
    bcm_switch_thermo_sensor_t * sensor_data)
{
    SHR_FUNC_INIT_VARS(unit);
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_temp_get(unit, interface_id, &sensor_data->thermo_sensor_current));
    }
    else if (TRUE == dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_is_supported))
    {
        dnx_dram_vendor_info_t vendor_info;

        SHR_IF_ERR_EXIT(dnx_dram_vendor_info_get(unit, interface_id, &vendor_info));
        if (vendor_info.gddr6_info.manufacturer_vendor_code == SHMOO_G6PHY16_VENDOR_MICRON)
        {
            
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Reading DRAM temperature is not supported for Micron DRAMs.\n");
        }

        if (!dnx_data_dram.general_info.feature_get(unit, dnx_data_dram_general_info_is_temperature_reading_supported))
        {
            
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Reading DRAM temperature is not supported for this device revision.\n");
        }

        SHR_IF_ERR_EXIT(dnx_gddr6_dram_temp_get(unit, interface_id, sensor_data));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unavailable functionality\n");
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_vendor_info_get(
    int unit,
    int interface_id,
    dnx_dram_vendor_info_t * vendor_info)
{
    uint32 vendor_info_is_valid;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vendor_info, _SHR_E_PARAM, "vendor_info");

    
    sal_memset(vendor_info, 0, sizeof(*vendor_info));

    
    if (0 == (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap & (1 << interface_id)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "interface_id %d is invalid\n", interface_id);
    }

    
    if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        
        SHR_IF_ERR_EXIT(dnx_dram_db.vendor_info.is_init.get(unit, interface_id, &vendor_info_is_valid));
        if (vendor_info_is_valid)
        {
            
            SHR_IF_ERR_EXIT(dnx_hbmc_vendor_info_from_sw_state_get(unit, interface_id, &vendor_info->hbm_info));
        }
        else
        {
            
            soc_dnx_hbm_device_id_t device_id;

            
            sal_memset(&device_id, 0, sizeof(device_id));

            
            if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
            {
                dnx_hbmc_firmware_operation_result_hbm_spare_t hbm_spare;

                SHR_IF_ERR_EXIT(dnx_hbmc_firmware_launch_operation
                                (unit, interface_id, DNX_HBMC_FIRMWARE_ALL_CHANNELS,
                                 DNX_HBMC_FIRMWARE_OPERATION_READ_DEVICE_ID));
                SHR_IF_ERR_EXIT(dnx_hbmc_firmware_operation_result_spare_get(unit, interface_id, 6, &hbm_spare));
                SHR_BITCOPY_RANGE(device_id.device_id_raw, 0, &hbm_spare.hbm_spare_data[0], 0, 16);
                SHR_BITCOPY_RANGE(device_id.device_id_raw, 16, &hbm_spare.hbm_spare_data[1], 0, 16);
                SHR_BITCOPY_RANGE(device_id.device_id_raw, 32, &hbm_spare.hbm_spare_data[2], 0, 16);
                SHR_BITCOPY_RANGE(device_id.device_id_raw, 48, &hbm_spare.hbm_spare_data[3], 0, 16);
                SHR_BITCOPY_RANGE(device_id.device_id_raw, 64, &hbm_spare.hbm_spare_data[4], 0, 16);
                SHR_BITCOPY_RANGE(device_id.device_id_raw, 80, &hbm_spare.hbm_spare_data[5], 0, 2);
            }
            else
            {
                SHR_IF_ERR_EXIT(soc_dnx_read_hbm_device_id(unit, interface_id, &device_id));
            }

            SHR_IF_ERR_EXIT(dnx_hbmc_device_id_to_vendor_info_decode(unit, &device_id, &vendor_info->hbm_info));

            
            SHR_IF_ERR_EXIT(dnx_hbmc_vendor_info_to_sw_state_set(unit, interface_id, &vendor_info->hbm_info));
        }
    }
    else if (TRUE == dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_is_supported))
    {
        
        SHR_IF_ERR_EXIT(dnx_dram_db.gddr6_vendor_info.is_init.get(unit, interface_id, &vendor_info_is_valid));
        if (vendor_info_is_valid)
        {
            
            SHR_IF_ERR_EXIT(dnx_gddr6_vendor_info_from_sw_state_get(unit, interface_id, &vendor_info->gddr6_info));
        }
        else
        {
            
            uint32 vendor_id;

            
            SHR_IF_ERR_EXIT(dnx_gddr6_dram_vendor_id_get(unit, interface_id, &vendor_id));
            SHR_IF_ERR_EXIT(dnx_gddr6_dram_vendor_id_to_vendor_info_decode(unit, vendor_id, &vendor_info->gddr6_info));

            
            SHR_IF_ERR_EXIT(dnx_gddr6_vendor_info_from_sw_state_set(unit, interface_id, &vendor_info->gddr6_info));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No known Dram types are supported for this device\n");
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_count(
    int unit,
    int *nof_drams)
{
    uint32 max_nof_dram = dnx_data_dram.general_info.max_nof_drams_get(unit);
    uint32 dram_bitmap = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_drams, _SHR_E_PARAM, "nof_drams");
    shr_bitop_range_count(&dram_bitmap, 0, max_nof_dram, nof_drams);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_index_is_valid(
    int unit,
    int dram_index,
    int *is_valid)
{
    uint32 max_nof_dram = dnx_data_dram.general_info.max_nof_drams_get(unit);
    
    uint32 dram_bitmap[1] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_valid, _SHR_E_PARAM, "is_valid");
    *is_valid = 1;

    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
    if ((dram_index < 0) || (dram_index >= max_nof_dram) || !SHR_BITGET(&dram_bitmap[0], dram_index))
    {
        *is_valid = 0;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_dram_db.init(unit));

    if (soc_sand_is_emulation_system(unit) != 0)
    {
        LOG_CLI_EX("Skip DRAM for emulation!! %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_sw_state_init(unit));
        if (dnx_data_dram.hbm.start_disabled_get(unit) == FALSE)
        {
            if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_apd_phy))
            {
                SHR_IF_ERR_EXIT(dnx_hbmc_init_with_apd_phy(unit));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_hbmc_init(unit));
            }
        }
    }
    else if (dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_sw_state_init(unit));
        SHR_IF_ERR_EXIT(dnx_gddr6_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_dram_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_deinit(unit));
    }
    else if (dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_is_supported))
    {
        
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dram_blocks_enable_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_hbc_blocks_enable_set(unit));
    }
    else if (dnx_data_dram.gddr6.feature_get(unit, dnx_data_dram_gddr6_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_gddr6_dcc_blocks_enable_set(unit));
    }

exit:
    SHR_FUNC_EXIT;
}
