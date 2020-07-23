

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DEVICE

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_device.h>



extern shr_error_e ramon_a0_data_device_attach(
    int unit);



static shr_error_e
dnxf_data_device_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "General attributes";
    
    submodule_data->nof_features = _dnxf_data_device_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device general features");

    submodule_data->features[dnxf_data_device_general_production_ready].name = "production_ready";
    submodule_data->features[dnxf_data_device_general_production_ready].doc = "Is device in production level quality.";
    submodule_data->features[dnxf_data_device_general_production_ready].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_serdes_support].name = "serdes_support";
    submodule_data->features[dnxf_data_device_general_serdes_support].doc = "Indicate if SerDes is supported";
    submodule_data->features[dnxf_data_device_general_serdes_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_fabric_support].name = "fabric_support";
    submodule_data->features[dnxf_data_device_general_fabric_support].doc = "Indicate if fabric is supported";
    submodule_data->features[dnxf_data_device_general_fabric_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_delay_exist].name = "delay_exist";
    submodule_data->features[dnxf_data_device_general_delay_exist].doc = "Indicate if there is delay when sending traffic, updating signals, running specific cases";
    submodule_data->features[dnxf_data_device_general_delay_exist].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_cmic_support].name = "cmic_support";
    submodule_data->features[dnxf_data_device_general_cmic_support].doc = "Indicate if cmic is supported";
    submodule_data->features[dnxf_data_device_general_cmic_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_real_interrupts_support].name = "real_interrupts_support";
    submodule_data->features[dnxf_data_device_general_real_interrupts_support].doc = "Indicate if real interrupts are supported";
    submodule_data->features[dnxf_data_device_general_real_interrupts_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_i2c_hw_support].name = "i2c_hw_support";
    submodule_data->features[dnxf_data_device_general_i2c_hw_support].doc = "Indicate if i2c is supported";
    submodule_data->features[dnxf_data_device_general_i2c_hw_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnxf_data_device_general_hw_support].name = "hw_support";
    submodule_data->features[dnxf_data_device_general_hw_support].doc = "Indicate if hw is supported";
    submodule_data->features[dnxf_data_device_general_hw_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnxf_data_device_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device general defines");

    submodule_data->defines[dnxf_data_device_general_define_nof_pvt_monitors].name = "nof_pvt_monitors";
    submodule_data->defines[dnxf_data_device_general_define_nof_pvt_monitors].doc = "Number of device PVT monitors";
    
    submodule_data->defines[dnxf_data_device_general_define_nof_pvt_monitors].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_general_define_pvt_base].name = "pvt_base";
    submodule_data->defines[dnxf_data_device_general_define_pvt_base].doc = "PVT base. Used for correct calculation of the monitor readings";
    
    submodule_data->defines[dnxf_data_device_general_define_pvt_base].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_general_define_pvt_factor].name = "pvt_factor";
    submodule_data->defines[dnxf_data_device_general_define_pvt_factor].doc = "PVT factor Used for correct calculation of the monitor readings";
    
    submodule_data->defines[dnxf_data_device_general_define_pvt_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_general_define_nof_cores].name = "nof_cores";
    submodule_data->defines[dnxf_data_device_general_define_nof_cores].doc = "Number of cores in the device.";
    
    submodule_data->defines[dnxf_data_device_general_define_nof_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_general_define_hard_reset_disable].name = "hard_reset_disable";
    submodule_data->defines[dnxf_data_device_general_define_hard_reset_disable].doc = "Init device without hard reset.";
    
    submodule_data->defines[dnxf_data_device_general_define_hard_reset_disable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_general_define_core_clock_khz].name = "core_clock_khz";
    submodule_data->defines[dnxf_data_device_general_define_core_clock_khz].doc = "Specifies system core clock speed in kHz.";
    
    submodule_data->defines[dnxf_data_device_general_define_core_clock_khz].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_general_define_system_ref_core_clock_khz].name = "system_ref_core_clock_khz";
    submodule_data->defines[dnxf_data_device_general_define_system_ref_core_clock_khz].doc = "Specifies system reference clock speed in kHz.";
    
    submodule_data->defines[dnxf_data_device_general_define_system_ref_core_clock_khz].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_general_define_device_id].name = "device_id";
    submodule_data->defines[dnxf_data_device_general_define_device_id].doc = "BCM Device ID (8XXX)";
    
    submodule_data->defines[dnxf_data_device_general_define_device_id].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_device_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_general_feature_get(
    int unit,
    dnxf_data_device_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, feature);
}


uint32
dnxf_data_device_general_nof_pvt_monitors_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_nof_pvt_monitors);
}

int
dnxf_data_device_general_pvt_base_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_pvt_base);
}

int
dnxf_data_device_general_pvt_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_pvt_factor);
}

uint32
dnxf_data_device_general_nof_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_nof_cores);
}

uint32
dnxf_data_device_general_hard_reset_disable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_hard_reset_disable);
}

uint32
dnxf_data_device_general_core_clock_khz_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_core_clock_khz);
}

uint32
dnxf_data_device_general_system_ref_core_clock_khz_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_system_ref_core_clock_khz);
}

uint32
dnxf_data_device_general_device_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_general, dnxf_data_device_general_define_device_id);
}










static shr_error_e
dnxf_data_device_access_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "access";
    submodule_data->doc = "General access attributes";
    
    submodule_data->nof_features = _dnxf_data_device_access_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device access features");

    
    submodule_data->nof_defines = _dnxf_data_device_access_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device access defines");

    submodule_data->defines[dnxf_data_device_access_define_table_dma_enable].name = "table_dma_enable";
    submodule_data->defines[dnxf_data_device_access_define_table_dma_enable].doc = "Enable/disable table DMA operations.";
    
    submodule_data->defines[dnxf_data_device_access_define_table_dma_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_tdma_timeout_usec].name = "tdma_timeout_usec";
    submodule_data->defines[dnxf_data_device_access_define_tdma_timeout_usec].doc = "Table DMA operation timeout.";
    
    submodule_data->defines[dnxf_data_device_access_define_tdma_timeout_usec].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_tdma_intr_enable].name = "tdma_intr_enable";
    submodule_data->defines[dnxf_data_device_access_define_tdma_intr_enable].doc = "Table DMA done is interrupt driven or by polling.";
    
    submodule_data->defines[dnxf_data_device_access_define_tdma_intr_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_tslam_dma_enable].name = "tslam_dma_enable";
    submodule_data->defines[dnxf_data_device_access_define_tslam_dma_enable].doc = "Enable/disable tslam DMA operations.";
    
    submodule_data->defines[dnxf_data_device_access_define_tslam_dma_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_tslam_timeout_usec].name = "tslam_timeout_usec";
    submodule_data->defines[dnxf_data_device_access_define_tslam_timeout_usec].doc = "Tslam DMA operation timeout.";
    
    submodule_data->defines[dnxf_data_device_access_define_tslam_timeout_usec].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_tslam_int_enable].name = "tslam_int_enable";
    submodule_data->defines[dnxf_data_device_access_define_tslam_int_enable].doc = "Tslam DMA done is interrupt driven or by polling.";
    
    submodule_data->defines[dnxf_data_device_access_define_tslam_int_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_mdio_int_divisor].name = "mdio_int_divisor";
    submodule_data->defines[dnxf_data_device_access_define_mdio_int_divisor].doc = "mdio_int_divisor";
    
    submodule_data->defines[dnxf_data_device_access_define_mdio_int_divisor].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_divisor].name = "mdio_ext_divisor";
    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_divisor].doc = "mdio_ext_divisor.";
    
    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_divisor].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_mdio_int_div_out_delay].name = "mdio_int_div_out_delay";
    submodule_data->defines[dnxf_data_device_access_define_mdio_int_div_out_delay].doc = "number of clock delay between the rising edge of MDC and the starting data of MDIO for internal divisor";
    
    submodule_data->defines[dnxf_data_device_access_define_mdio_int_div_out_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_div_out_delay].name = "mdio_ext_div_out_delay";
    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_div_out_delay].doc = "number of clock delay between the rising edge of MDC and the starting data of MDIO for external divisor";
    
    submodule_data->defines[dnxf_data_device_access_define_mdio_ext_div_out_delay].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_bist_enable].name = "bist_enable";
    submodule_data->defines[dnxf_data_device_access_define_bist_enable].doc = "Determines if to run Memory Built-In Self-Test (MBIST) of internal memory (tables) during startup.";
    
    submodule_data->defines[dnxf_data_device_access_define_bist_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_interval].name = "sbus_dma_interval";
    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_interval].doc = "sbus DMA operation interval.";
    
    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_interval].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_intr_enable].name = "sbus_dma_intr_enable";
    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_intr_enable].doc = "sbus DMA interrupt enable.";
    
    submodule_data->defines[dnxf_data_device_access_define_sbus_dma_intr_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_access_define_mem_clear_chunk_size].name = "mem_clear_chunk_size";
    submodule_data->defines[dnxf_data_device_access_define_mem_clear_chunk_size].doc = "sbus DMA chunk size.";
    
    submodule_data->defines[dnxf_data_device_access_define_mem_clear_chunk_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_device_access_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device access tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_access_feature_get(
    int unit,
    dnxf_data_device_access_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, feature);
}


uint32
dnxf_data_device_access_table_dma_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_table_dma_enable);
}

uint32
dnxf_data_device_access_tdma_timeout_usec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_tdma_timeout_usec);
}

uint32
dnxf_data_device_access_tdma_intr_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_tdma_intr_enable);
}

uint32
dnxf_data_device_access_tslam_dma_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_tslam_dma_enable);
}

uint32
dnxf_data_device_access_tslam_timeout_usec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_tslam_timeout_usec);
}

uint32
dnxf_data_device_access_tslam_int_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_tslam_int_enable);
}

uint32
dnxf_data_device_access_mdio_int_divisor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_mdio_int_divisor);
}

uint32
dnxf_data_device_access_mdio_ext_divisor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_mdio_ext_divisor);
}

uint32
dnxf_data_device_access_mdio_int_div_out_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_mdio_int_div_out_delay);
}

uint32
dnxf_data_device_access_mdio_ext_div_out_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_mdio_ext_div_out_delay);
}

uint32
dnxf_data_device_access_bist_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_bist_enable);
}

uint32
dnxf_data_device_access_sbus_dma_interval_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_sbus_dma_interval);
}

uint32
dnxf_data_device_access_sbus_dma_intr_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_sbus_dma_intr_enable);
}

uint32
dnxf_data_device_access_mem_clear_chunk_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_access, dnxf_data_device_access_define_mem_clear_chunk_size);
}










static shr_error_e
dnxf_data_device_blocks_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "blocks";
    submodule_data->doc = "General block information";
    
    submodule_data->nof_features = _dnxf_data_device_blocks_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device blocks features");

    submodule_data->features[dnxf_data_device_blocks_dch_reset_restore_support].name = "dch_reset_restore_support";
    submodule_data->features[dnxf_data_device_blocks_dch_reset_restore_support].doc = "";
    submodule_data->features[dnxf_data_device_blocks_dch_reset_restore_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnxf_data_device_blocks_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device blocks defines");

    submodule_data->defines[dnxf_data_device_blocks_define_nof_all_blocks].name = "nof_all_blocks";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_all_blocks].doc = "Number of block instances in the device.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_all_blocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fmac].name = "nof_instances_fmac";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fmac].doc = "Number of FMAC instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fmac].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fmac].name = "nof_links_in_fmac";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fmac].doc = "Number of links for each FMAC block.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fmac].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fsrd].name = "nof_instances_fsrd";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fsrd].doc = "Number of FSRD instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_fsrd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fsrd].name = "nof_instances_brdc_fsrd";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fsrd].doc = "Number of BRoaDCast FSRD instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fsrd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dch].name = "nof_instances_dch";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dch].doc = "Number of DCH instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dch].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dch].name = "nof_links_in_dch";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dch].doc = "Number of links for each DCH block.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dch].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_dch_link_groups].name = "nof_dch_link_groups";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_dch_link_groups].doc = "Number of link groups for each DCH block.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_dch_link_groups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cch].name = "nof_instances_cch";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cch].doc = "Number of CCH instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cch].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_cch].name = "nof_links_in_cch";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_cch].doc = "Number of links for each CCH block.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_cch].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcq].name = "nof_links_in_dcq";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcq].doc = "Number of links for each DCQ block.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcq].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_rtp].name = "nof_instances_rtp";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_rtp].doc = "Number of RTP instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_rtp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_occg].name = "nof_instances_occg";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_occg].doc = "Number of OCCG instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_occg].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_eci].name = "nof_instances_eci";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_eci].doc = "Number of ECI instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_eci].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cmic].name = "nof_instances_cmic";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cmic].doc = "Number of CMIC instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_cmic].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mesh_topology].name = "nof_instances_mesh_topology";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mesh_topology].doc = "Number of MESH Topology instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mesh_topology].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_otpc].name = "nof_instances_otpc";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_otpc].doc = "Number of OTPC instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_otpc].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmach].name = "nof_instances_brdc_fmach";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmach].doc = "Number of BRoaDCast FMACH instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmach].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmacl].name = "nof_instances_brdc_fmacl";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmacl].doc = "Number of BRoaDCast FMACL instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmacl].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmac_ac].name = "nof_instances_brdc_fmac_ac";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmac_ac].doc = "Number of BRoaDCast FMAC_AC instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmac_ac].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmac_bd].name = "nof_instances_brdc_fmac_bd";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmac_bd].doc = "Number of BRoaDCast FMAC_BD instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_fmac_bd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_dch].name = "nof_instances_brdc_dch";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_dch].doc = "Number of BRoaDCast DCH instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_dch].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_dcml].name = "nof_instances_brdc_dcml";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_dcml].doc = "Number of BRoaDCast DCML instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_brdc_dcml].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_lcm].name = "nof_instances_lcm";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_lcm].doc = "Number of LCM instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_lcm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mct].name = "nof_instances_mct";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mct].doc = "Number of MCT instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_mct].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_qrh].name = "nof_instances_qrh";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_qrh].doc = "Number of QRH instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_qrh].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dcml].name = "nof_instances_dcml";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dcml].doc = "Number of DCML instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_instances_dcml].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcml].name = "nof_links_in_dcml";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcml].doc = "Number of links for each DCML block.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_dcml].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_lcm].name = "nof_links_in_lcm";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_lcm].doc = "Number of links for each LCM block.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_lcm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_qrh].name = "nof_links_in_qrh";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_qrh].doc = "Number of links for each QRH block.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_qrh].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_qrh_mclbt_instances].name = "nof_qrh_mclbt_instances";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_qrh_mclbt_instances].doc = "Number of QRH MCLBT instances.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_qrh_mclbt_instances].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_dtm_fifos].name = "nof_dtm_fifos";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_dtm_fifos].doc = "Number of FIFOs for each DTM block.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_dtm_fifos].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fsrd].name = "nof_links_in_fsrd";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fsrd].doc = "Number of links for each FSRD block.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_fsrd].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_phy_core].name = "nof_links_in_phy_core";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_phy_core].doc = "Number of links in each PHY core.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_links_in_phy_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnxf_data_device_blocks_define_nof_fmacs_in_fsrd].name = "nof_fmacs_in_fsrd";
    submodule_data->defines[dnxf_data_device_blocks_define_nof_fmacs_in_fsrd].doc = "Number of FMAC blocks for each FSRD block.";
    
    submodule_data->defines[dnxf_data_device_blocks_define_nof_fmacs_in_fsrd].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnxf_data_device_blocks_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device blocks tables");

    
    submodule_data->tables[dnxf_data_device_blocks_table_override].name = "override";
    submodule_data->tables[dnxf_data_device_blocks_table_override].doc = "blocks enable/disable override";
    submodule_data->tables[dnxf_data_device_blocks_table_override].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_blocks_table_override].size_of_values = sizeof(dnxf_data_device_blocks_override_t);
    submodule_data->tables[dnxf_data_device_blocks_table_override].entry_get = dnxf_data_device_blocks_override_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_blocks_table_override].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_blocks_table_override].keys[0].name = "index";
    submodule_data->tables[dnxf_data_device_blocks_table_override].keys[0].doc = "running index";

    
    submodule_data->tables[dnxf_data_device_blocks_table_override].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_blocks_table_override].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_blocks_table_override].nof_values, "_dnxf_data_device_blocks_table_override table values");
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[0].name = "block_type";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[0].type = "char *";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[0].doc = "See soc_block_type_t";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_override_t, block_type);
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[1].name = "block_instance";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[1].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[1].doc = "instance ID within block type";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[1].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_override_t, block_instance);
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[2].name = "value";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[2].type = "int";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[2].doc = "The value to override";
    submodule_data->tables[dnxf_data_device_blocks_table_override].values[2].offset = UTILEX_OFFSETOF(dnxf_data_device_blocks_override_t, value);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_blocks_feature_get(
    int unit,
    dnxf_data_device_blocks_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, feature);
}


uint32
dnxf_data_device_blocks_nof_all_blocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_all_blocks);
}

uint32
dnxf_data_device_blocks_nof_instances_fmac_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_fmac);
}

uint32
dnxf_data_device_blocks_nof_links_in_fmac_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_fmac);
}

uint32
dnxf_data_device_blocks_nof_instances_fsrd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_fsrd);
}

uint32
dnxf_data_device_blocks_nof_instances_brdc_fsrd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_brdc_fsrd);
}

uint32
dnxf_data_device_blocks_nof_instances_dch_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_dch);
}

uint32
dnxf_data_device_blocks_nof_links_in_dch_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_dch);
}

uint32
dnxf_data_device_blocks_nof_dch_link_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_dch_link_groups);
}

uint32
dnxf_data_device_blocks_nof_instances_cch_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_cch);
}

uint32
dnxf_data_device_blocks_nof_links_in_cch_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_cch);
}

uint32
dnxf_data_device_blocks_nof_links_in_dcq_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_dcq);
}

uint32
dnxf_data_device_blocks_nof_instances_rtp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_rtp);
}

uint32
dnxf_data_device_blocks_nof_instances_occg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_occg);
}

uint32
dnxf_data_device_blocks_nof_instances_eci_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_eci);
}

uint32
dnxf_data_device_blocks_nof_instances_cmic_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_cmic);
}

uint32
dnxf_data_device_blocks_nof_instances_mesh_topology_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_mesh_topology);
}

uint32
dnxf_data_device_blocks_nof_instances_otpc_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_otpc);
}

uint32
dnxf_data_device_blocks_nof_instances_brdc_fmach_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_brdc_fmach);
}

uint32
dnxf_data_device_blocks_nof_instances_brdc_fmacl_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_brdc_fmacl);
}

uint32
dnxf_data_device_blocks_nof_instances_brdc_fmac_ac_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_brdc_fmac_ac);
}

uint32
dnxf_data_device_blocks_nof_instances_brdc_fmac_bd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_brdc_fmac_bd);
}

uint32
dnxf_data_device_blocks_nof_instances_brdc_dch_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_brdc_dch);
}

uint32
dnxf_data_device_blocks_nof_instances_brdc_dcml_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_brdc_dcml);
}

uint32
dnxf_data_device_blocks_nof_instances_lcm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_lcm);
}

uint32
dnxf_data_device_blocks_nof_instances_mct_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_mct);
}

uint32
dnxf_data_device_blocks_nof_instances_qrh_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_qrh);
}

uint32
dnxf_data_device_blocks_nof_instances_dcml_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_instances_dcml);
}

uint32
dnxf_data_device_blocks_nof_links_in_dcml_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_dcml);
}

uint32
dnxf_data_device_blocks_nof_links_in_lcm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_lcm);
}

uint32
dnxf_data_device_blocks_nof_links_in_qrh_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_qrh);
}

uint32
dnxf_data_device_blocks_nof_qrh_mclbt_instances_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_qrh_mclbt_instances);
}

uint32
dnxf_data_device_blocks_nof_dtm_fifos_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_dtm_fifos);
}

uint32
dnxf_data_device_blocks_nof_links_in_fsrd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_fsrd);
}

uint32
dnxf_data_device_blocks_nof_links_in_phy_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_links_in_phy_core);
}

uint32
dnxf_data_device_blocks_nof_fmacs_in_fsrd_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_define_nof_fmacs_in_fsrd);
}



const dnxf_data_device_blocks_override_t *
dnxf_data_device_blocks_override_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_override);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_device_blocks_override_t *) data;

}


shr_error_e
dnxf_data_device_blocks_override_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_blocks_override_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_override);
    data = (const dnxf_data_device_blocks_override_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->block_type == NULL ? "" : data->block_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_instance);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->value);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_device_blocks_override_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_blocks, dnxf_data_device_blocks_table_override);

}






static shr_error_e
dnxf_data_device_interrupts_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "interrupts";
    submodule_data->doc = "General interrupts information";
    
    submodule_data->nof_features = _dnxf_data_device_interrupts_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device interrupts features");

    
    submodule_data->nof_defines = _dnxf_data_device_interrupts_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device interrupts defines");

    submodule_data->defines[dnxf_data_device_interrupts_define_nof_interrupts].name = "nof_interrupts";
    submodule_data->defines[dnxf_data_device_interrupts_define_nof_interrupts].doc = "Number of interrupts.";
    
    submodule_data->defines[dnxf_data_device_interrupts_define_nof_interrupts].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_device_interrupts_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device interrupts tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_interrupts_feature_get(
    int unit,
    dnxf_data_device_interrupts_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_interrupts, feature);
}


uint32
dnxf_data_device_interrupts_nof_interrupts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_interrupts, dnxf_data_device_interrupts_define_nof_interrupts);
}










static shr_error_e
dnxf_data_device_custom_features_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "custom_features";
    submodule_data->doc = "Custom features";
    
    submodule_data->nof_features = _dnxf_data_device_custom_features_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device custom_features features");

    
    submodule_data->nof_defines = _dnxf_data_device_custom_features_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device custom_features defines");

    submodule_data->defines[dnxf_data_device_custom_features_define_mesh_topology_fast].name = "mesh_topology_fast";
    submodule_data->defines[dnxf_data_device_custom_features_define_mesh_topology_fast].doc = "Device is in mesh topology fast mode.";
    
    submodule_data->defines[dnxf_data_device_custom_features_define_mesh_topology_fast].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_device_custom_features_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device custom_features tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_custom_features_feature_get(
    int unit,
    dnxf_data_device_custom_features_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_custom_features, feature);
}


uint32
dnxf_data_device_custom_features_mesh_topology_fast_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_custom_features, dnxf_data_device_custom_features_define_mesh_topology_fast);
}










static shr_error_e
dnxf_data_device_properties_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "properties";
    submodule_data->doc = "used to store information related to soc properties";
    
    submodule_data->nof_features = _dnxf_data_device_properties_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device properties features");

    
    submodule_data->nof_defines = _dnxf_data_device_properties_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device properties defines");

    
    submodule_data->nof_tables = _dnxf_data_device_properties_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device properties tables");

    
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].name = "unsupported";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].doc = "unsupported soc properties";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].size_of_values = sizeof(dnxf_data_device_properties_unsupported_t);
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].entry_get = dnxf_data_device_properties_unsupported_entry_str_get;

    
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].nof_keys = 1;
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].keys[0].name = "index";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].keys[0].doc = "running index";

    
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnxf_data_device_properties_table_unsupported].values, dnxc_data_value_t, submodule_data->tables[dnxf_data_device_properties_table_unsupported].nof_values, "_dnxf_data_device_properties_table_unsupported table values");
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[0].name = "property";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[0].type = "char *";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[0].doc = "soc property name";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[0].offset = UTILEX_OFFSETOF(dnxf_data_device_properties_unsupported_t, property);
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[1].name = "suffix";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[1].type = "char *";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[1].doc = "used in case the soc property read using suffix method";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[1].offset = UTILEX_OFFSETOF(dnxf_data_device_properties_unsupported_t, suffix);
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[2].name = "num_max";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[2].type = "int";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[2].doc = "max number of index to check in case using suffix_num method";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[2].offset = UTILEX_OFFSETOF(dnxf_data_device_properties_unsupported_t, num_max);
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[3].name = "per_port";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[3].type = "int";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[3].doc = "1 if the soc property might be read per port";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[3].offset = UTILEX_OFFSETOF(dnxf_data_device_properties_unsupported_t, per_port);
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[4].name = "err_msg";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[4].type = "char *";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[4].doc = "detailed error message with instructions what to do instead";
    submodule_data->tables[dnxf_data_device_properties_table_unsupported].values[4].offset = UTILEX_OFFSETOF(dnxf_data_device_properties_unsupported_t, err_msg);


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_properties_feature_get(
    int unit,
    dnxf_data_device_properties_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_properties, feature);
}




const dnxf_data_device_properties_unsupported_t *
dnxf_data_device_properties_unsupported_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_properties, dnxf_data_device_properties_table_unsupported);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnxf_data_device_properties_unsupported_t *) data;

}


shr_error_e
dnxf_data_device_properties_unsupported_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnxf_data_device_properties_unsupported_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_properties, dnxf_data_device_properties_table_unsupported);
    data = (const dnxf_data_device_properties_unsupported_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->property == NULL ? "" : data->property);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->suffix == NULL ? "" : data->suffix);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num_max);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->per_port);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->err_msg == NULL ? "" : data->err_msg);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnxf_data_device_properties_unsupported_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_properties, dnxf_data_device_properties_table_unsupported);

}






static shr_error_e
dnxf_data_device_ha_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ha";
    submodule_data->doc = "values for sw state and high availability init";
    
    submodule_data->nof_features = _dnxf_data_device_ha_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device ha features");

    
    submodule_data->nof_defines = _dnxf_data_device_ha_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device ha defines");

    submodule_data->defines[dnxf_data_device_ha_define_warmboot_support].name = "warmboot_support";
    submodule_data->defines[dnxf_data_device_ha_define_warmboot_support].doc = "specify if unit should support wb";
    
    submodule_data->defines[dnxf_data_device_ha_define_warmboot_support].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnxf_data_device_ha_define_sw_state_max_size].name = "sw_state_max_size";
    submodule_data->defines[dnxf_data_device_ha_define_sw_state_max_size].doc = "specify the amount of memory (in bytes) preallocated for sw state";
    
    submodule_data->defines[dnxf_data_device_ha_define_sw_state_max_size].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_device_ha_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device ha tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_ha_feature_get(
    int unit,
    dnxf_data_device_ha_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_ha, feature);
}


uint32
dnxf_data_device_ha_warmboot_support_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_ha, dnxf_data_device_ha_define_warmboot_support);
}

uint32
dnxf_data_device_ha_sw_state_max_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_ha, dnxf_data_device_ha_define_sw_state_max_size);
}










static shr_error_e
dnxf_data_device_emulation_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "emulation";
    submodule_data->doc = "Emulation attributes";
    
    submodule_data->nof_features = _dnxf_data_device_emulation_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device emulation features");

    
    submodule_data->nof_defines = _dnxf_data_device_emulation_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device emulation defines");

    submodule_data->defines[dnxf_data_device_emulation_define_emulation_system].name = "emulation_system";
    submodule_data->defines[dnxf_data_device_emulation_define_emulation_system].doc = "Checking if the device is ran on emulation.";
    
    submodule_data->defines[dnxf_data_device_emulation_define_emulation_system].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnxf_data_device_emulation_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device emulation tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnxf_data_device_emulation_feature_get(
    int unit,
    dnxf_data_device_emulation_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_emulation, feature);
}


uint32
dnxf_data_device_emulation_emulation_system_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnxf_data_module_device, dnxf_data_device_submodule_emulation, dnxf_data_device_emulation_define_emulation_system);
}







shr_error_e
dnxf_data_device_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "device";
    module_data->nof_submodules = _dnxf_data_device_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data device submodules");

    
    SHR_IF_ERR_EXIT(dnxf_data_device_general_init(unit, &module_data->submodules[dnxf_data_device_submodule_general]));
    SHR_IF_ERR_EXIT(dnxf_data_device_access_init(unit, &module_data->submodules[dnxf_data_device_submodule_access]));
    SHR_IF_ERR_EXIT(dnxf_data_device_blocks_init(unit, &module_data->submodules[dnxf_data_device_submodule_blocks]));
    SHR_IF_ERR_EXIT(dnxf_data_device_interrupts_init(unit, &module_data->submodules[dnxf_data_device_submodule_interrupts]));
    SHR_IF_ERR_EXIT(dnxf_data_device_custom_features_init(unit, &module_data->submodules[dnxf_data_device_submodule_custom_features]));
    SHR_IF_ERR_EXIT(dnxf_data_device_properties_init(unit, &module_data->submodules[dnxf_data_device_submodule_properties]));
    SHR_IF_ERR_EXIT(dnxf_data_device_ha_init(unit, &module_data->submodules[dnxf_data_device_submodule_ha]));
    SHR_IF_ERR_EXIT(dnxf_data_device_emulation_init(unit, &module_data->submodules[dnxf_data_device_submodule_emulation]));
    
    if (dnxc_data_mgmt_is_ramon(unit))
    {
        SHR_IF_ERR_EXIT(ramon_a0_data_device_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

