

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DEVICE

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_device.h>



extern shr_error_e jr2_a0_data_device_attach(
    int unit);
extern shr_error_e jr2_b0_data_device_attach(
    int unit);
extern shr_error_e jr2_b1_data_device_attach(
    int unit);
extern shr_error_e j2c_a0_data_device_attach(
    int unit);
extern shr_error_e j2c_a1_data_device_attach(
    int unit);
extern shr_error_e q2a_a0_data_device_attach(
    int unit);
extern shr_error_e q2a_b0_data_device_attach(
    int unit);
extern shr_error_e q2a_b1_data_device_attach(
    int unit);
extern shr_error_e j2p_a0_data_device_attach(
    int unit);



static shr_error_e
dnx_data_device_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "General attributes";
    
    submodule_data->nof_features = _dnx_data_device_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device general features");

    submodule_data->features[dnx_data_device_general_production_ready].name = "production_ready";
    submodule_data->features[dnx_data_device_general_production_ready].doc = "Is device in production level quality.";
    submodule_data->features[dnx_data_device_general_production_ready].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_hard_reset_disable].name = "hard_reset_disable";
    submodule_data->features[dnx_data_device_general_hard_reset_disable].doc = "Init the device without hard reset.";
    submodule_data->features[dnx_data_device_general_hard_reset_disable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_multi_core].name = "multi_core";
    submodule_data->features[dnx_data_device_general_multi_core].doc = "Is device multi core";
    submodule_data->features[dnx_data_device_general_multi_core].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_rqp_ecc_err_exist].name = "rqp_ecc_err_exist";
    submodule_data->features[dnx_data_device_general_rqp_ecc_err_exist].doc = "RQP ecc error exist";
    submodule_data->features[dnx_data_device_general_rqp_ecc_err_exist].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_new_crc32_calc].name = "new_crc32_calc";
    submodule_data->features[dnx_data_device_general_new_crc32_calc].doc = "New crc32 calculation";
    submodule_data->features[dnx_data_device_general_new_crc32_calc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_protection_state_ignore].name = "protection_state_ignore";
    submodule_data->features[dnx_data_device_general_protection_state_ignore].doc = "Protection state ignoring capability";
    submodule_data->features[dnx_data_device_general_protection_state_ignore].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_delay_exist].name = "delay_exist";
    submodule_data->features[dnx_data_device_general_delay_exist].doc = "Indicate if there is delay when sending traffic, updating signals, running specific cases";
    submodule_data->features[dnx_data_device_general_delay_exist].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_serdes_support].name = "serdes_support";
    submodule_data->features[dnx_data_device_general_serdes_support].doc = "Indicate if SerDes is supported";
    submodule_data->features[dnx_data_device_general_serdes_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_snooping_support].name = "snooping_support";
    submodule_data->features[dnx_data_device_general_snooping_support].doc = "Indicate if snooping is supported";
    submodule_data->features[dnx_data_device_general_snooping_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_mirroring_support].name = "mirroring_support";
    submodule_data->features[dnx_data_device_general_mirroring_support].doc = "Indicate if mirroring is supported";
    submodule_data->features[dnx_data_device_general_mirroring_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_injection_support].name = "injection_support";
    submodule_data->features[dnx_data_device_general_injection_support].doc = "Indicate if both injection to any place in the pipe and injection via any port is supported";
    submodule_data->features[dnx_data_device_general_injection_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_fabric_support].name = "fabric_support";
    submodule_data->features[dnx_data_device_general_fabric_support].doc = "Indicate if fabric is supported";
    submodule_data->features[dnx_data_device_general_fabric_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_flexe_support].name = "flexe_support";
    submodule_data->features[dnx_data_device_general_flexe_support].doc = "Indicate if flexe is supported";
    submodule_data->features[dnx_data_device_general_flexe_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_cmic_support].name = "cmic_support";
    submodule_data->features[dnx_data_device_general_cmic_support].doc = "Indicate if cmic is supported";
    submodule_data->features[dnx_data_device_general_cmic_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_real_interrupts_support].name = "real_interrupts_support";
    submodule_data->features[dnx_data_device_general_real_interrupts_support].doc = "Indicate if real interrupts are supported";
    submodule_data->features[dnx_data_device_general_real_interrupts_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_i2c_hw_support].name = "i2c_hw_support";
    submodule_data->features[dnx_data_device_general_i2c_hw_support].doc = "Indicate if i2c is supported";
    submodule_data->features[dnx_data_device_general_i2c_hw_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_ring_protection_hw_support].name = "ring_protection_hw_support";
    submodule_data->features[dnx_data_device_general_ring_protection_hw_support].doc = "Indicate if ring protection is supported";
    submodule_data->features[dnx_data_device_general_ring_protection_hw_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_hw_support].name = "hw_support";
    submodule_data->features[dnx_data_device_general_hw_support].doc = "Indicate if hw is supported";
    submodule_data->features[dnx_data_device_general_hw_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_device_general_ext_encap_is_supported].name = "ext_encap_is_supported";
    submodule_data->features[dnx_data_device_general_ext_encap_is_supported].doc = "Indicate if extended encapsulation";
    submodule_data->features[dnx_data_device_general_ext_encap_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_device_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device general defines");

    submodule_data->defines[dnx_data_device_general_define_max_nof_system_ports].name = "max_nof_system_ports";
    submodule_data->defines[dnx_data_device_general_define_max_nof_system_ports].doc = "Max number of system ports, may use software to indicate invalid system port";
    
    submodule_data->defines[dnx_data_device_general_define_max_nof_system_ports].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_device_general_define_invalid_system_port].name = "invalid_system_port";
    submodule_data->defines[dnx_data_device_general_define_invalid_system_port].doc = "invalid system port - defined as the last system port";
    
    submodule_data->defines[dnx_data_device_general_define_invalid_system_port].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_device_general_define_fmq_system_port].name = "fmq_system_port";
    submodule_data->defines[dnx_data_device_general_define_fmq_system_port].doc = "FMQ reserved system port";
    
    submodule_data->defines[dnx_data_device_general_define_fmq_system_port].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_device_general_define_invalid_fap_id].name = "invalid_fap_id";
    submodule_data->defines[dnx_data_device_general_define_invalid_fap_id].doc = "invalid FAP id - defined as the last FAP id";
    
    submodule_data->defines[dnx_data_device_general_define_invalid_fap_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_device_general_define_fap_ids_per_core_bits].name = "fap_ids_per_core_bits";
    submodule_data->defines[dnx_data_device_general_define_fap_ids_per_core_bits].doc = "number of bits in FAP IDs per core";
    
    submodule_data->defines[dnx_data_device_general_define_fap_ids_per_core_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_device_general_define_nof_cores].name = "nof_cores";
    submodule_data->defines[dnx_data_device_general_define_nof_cores].doc = "Number of cores in the device.";
    
    submodule_data->defines[dnx_data_device_general_define_nof_cores].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_device_general_define_nof_cores_for_traffic].name = "nof_cores_for_traffic";
    submodule_data->defines[dnx_data_device_general_define_nof_cores_for_traffic].doc = "Number of cores in the device used for traffic.";
    
    submodule_data->defines[dnx_data_device_general_define_nof_cores_for_traffic].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_device_general_define_nof_sub_units].name = "nof_sub_units";
    submodule_data->defines[dnx_data_device_general_define_nof_sub_units].doc = "Number of sub units in the device.";
    
    submodule_data->defines[dnx_data_device_general_define_nof_sub_units].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_device_general_define_core_max_nof_bits].name = "core_max_nof_bits";
    submodule_data->defines[dnx_data_device_general_define_core_max_nof_bits].doc = "Max number of cores bits to represent core. should be log2(nof_cores)";
    
    submodule_data->defines[dnx_data_device_general_define_core_max_nof_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_max_nof_fap_ids_per_core].name = "max_nof_fap_ids_per_core";
    submodule_data->defines[dnx_data_device_general_define_max_nof_fap_ids_per_core].doc = "maximal number of FAP IDs per core";
    
    submodule_data->defines[dnx_data_device_general_define_max_nof_fap_ids_per_core].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_max_nof_fap_ids_per_device].name = "max_nof_fap_ids_per_device";
    submodule_data->defines[dnx_data_device_general_define_max_nof_fap_ids_per_device].doc = "maximal number of FAP IDs per device";
    
    submodule_data->defines[dnx_data_device_general_define_max_nof_fap_ids_per_device].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_core_clock_khz].name = "core_clock_khz";
    submodule_data->defines[dnx_data_device_general_define_core_clock_khz].doc = "Core clock frequency [KHZ]";
    
    submodule_data->defines[dnx_data_device_general_define_core_clock_khz].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_system_ref_core_clock_khz].name = "system_ref_core_clock_khz";
    submodule_data->defines[dnx_data_device_general_define_system_ref_core_clock_khz].doc = "Specifies system reference clock speed in kHz.";
    
    submodule_data->defines[dnx_data_device_general_define_system_ref_core_clock_khz].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_ref_core_clock_mhz].name = "ref_core_clock_mhz";
    submodule_data->defines[dnx_data_device_general_define_ref_core_clock_mhz].doc = "Reference clock comes from the board";
    
    submodule_data->defines[dnx_data_device_general_define_ref_core_clock_mhz].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_bus_size_in_bits].name = "bus_size_in_bits";
    submodule_data->defines[dnx_data_device_general_define_bus_size_in_bits].doc = "Core bus size in bits";
    
    submodule_data->defines[dnx_data_device_general_define_bus_size_in_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_maximal_core_bandwidth_kbps].name = "maximal_core_bandwidth_kbps";
    submodule_data->defines[dnx_data_device_general_define_maximal_core_bandwidth_kbps].doc = "Maximal core bandwidth in Kbits per second";
    
    submodule_data->defines[dnx_data_device_general_define_maximal_core_bandwidth_kbps].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_packet_per_clock].name = "packet_per_clock";
    submodule_data->defines[dnx_data_device_general_define_packet_per_clock].doc = "Core packets per clock";
    
    submodule_data->defines[dnx_data_device_general_define_packet_per_clock].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_nof_faps].name = "nof_faps";
    submodule_data->defines[dnx_data_device_general_define_nof_faps].doc = "Number of supported faps in the system.";
    
    submodule_data->defines[dnx_data_device_general_define_nof_faps].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_device_id].name = "device_id";
    submodule_data->defines[dnx_data_device_general_define_device_id].doc = "BCM Device ID (8XXX)";
    
    submodule_data->defines[dnx_data_device_general_define_device_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_nof_sku_bits].name = "nof_sku_bits";
    submodule_data->defines[dnx_data_device_general_define_nof_sku_bits].doc = "nof LSB bits of device_is dedicated for SKU";
    
    submodule_data->defines[dnx_data_device_general_define_nof_sku_bits].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_bist_enable].name = "bist_enable";
    submodule_data->defines[dnx_data_device_general_define_bist_enable].doc = "Determines if to run Memory Built-In Self-Test (MBIST) of internal memories (tables) during startup.";
    
    submodule_data->defines[dnx_data_device_general_define_bist_enable].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_device_general_define_nof_bits_for_nof_cores].name = "nof_bits_for_nof_cores";
    submodule_data->defines[dnx_data_device_general_define_nof_bits_for_nof_cores].doc = "Number of bits to represent core.";
    
    submodule_data->defines[dnx_data_device_general_define_nof_bits_for_nof_cores].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_device_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device general tables");

    
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].name = "ecc_err_masking";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].doc = "Table containing all registers and fields that need their ECC errors to be masked due to issues when reading";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].size_of_values = sizeof(dnx_data_device_general_ecc_err_masking_t);
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].entry_get = dnx_data_device_general_ecc_err_masking_entry_str_get;

    
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].nof_keys = 1;
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].keys[0].name = "block_type";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].keys[0].doc = "HW Block type";

    
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values, dnxc_data_value_t, submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].nof_values, "_dnx_data_device_general_table_ecc_err_masking table values");
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[0].name = "ecc_1bit_err_mask_reg";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[0].type = "int";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[0].doc = "Register for masking 1 bit ECC errors";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[0].offset = UTILEX_OFFSETOF(dnx_data_device_general_ecc_err_masking_t, ecc_1bit_err_mask_reg);
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[1].name = "ecc_1bit_err_field";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[1].type = "int[DNX_MAX_NOF_ECC_ERR_MEMS]";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[1].doc = "Array of fields for masking 1 bit ECC errors. Last element of the array myst always be -1.";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[1].offset = UTILEX_OFFSETOF(dnx_data_device_general_ecc_err_masking_t, ecc_1bit_err_field);
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[2].name = "ecc_2bit_err_mask_reg";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[2].type = "int";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[2].doc = "Register for masking 2 bit ECC errors";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[2].offset = UTILEX_OFFSETOF(dnx_data_device_general_ecc_err_masking_t, ecc_2bit_err_mask_reg);
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[3].name = "ecc_2bit_err_field";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[3].type = "int[DNX_MAX_NOF_ECC_ERR_MEMS]";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[3].doc = "Array of fields for masking 2 bit ECC errors. Last element of the array myst always be -1.";
    submodule_data->tables[dnx_data_device_general_table_ecc_err_masking].values[3].offset = UTILEX_OFFSETOF(dnx_data_device_general_ecc_err_masking_t, ecc_2bit_err_field);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_device_general_feature_get(
    int unit,
    dnx_data_device_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, feature);
}


uint32
dnx_data_device_general_max_nof_system_ports_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_max_nof_system_ports);
}

uint32
dnx_data_device_general_invalid_system_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_invalid_system_port);
}

uint32
dnx_data_device_general_fmq_system_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_fmq_system_port);
}

uint32
dnx_data_device_general_invalid_fap_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_invalid_fap_id);
}

uint32
dnx_data_device_general_fap_ids_per_core_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_fap_ids_per_core_bits);
}

uint32
dnx_data_device_general_nof_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_nof_cores);
}

uint32
dnx_data_device_general_nof_cores_for_traffic_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_nof_cores_for_traffic);
}

uint32
dnx_data_device_general_nof_sub_units_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_nof_sub_units);
}

uint32
dnx_data_device_general_core_max_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_core_max_nof_bits);
}

uint32
dnx_data_device_general_max_nof_fap_ids_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_max_nof_fap_ids_per_core);
}

uint32
dnx_data_device_general_max_nof_fap_ids_per_device_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_max_nof_fap_ids_per_device);
}

uint32
dnx_data_device_general_core_clock_khz_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_core_clock_khz);
}

uint32
dnx_data_device_general_system_ref_core_clock_khz_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_system_ref_core_clock_khz);
}

uint32
dnx_data_device_general_ref_core_clock_mhz_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_ref_core_clock_mhz);
}

uint32
dnx_data_device_general_bus_size_in_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_bus_size_in_bits);
}

uint32
dnx_data_device_general_maximal_core_bandwidth_kbps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_maximal_core_bandwidth_kbps);
}

uint32
dnx_data_device_general_packet_per_clock_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_packet_per_clock);
}

uint32
dnx_data_device_general_nof_faps_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_nof_faps);
}

uint32
dnx_data_device_general_device_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_device_id);
}

uint32
dnx_data_device_general_nof_sku_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_nof_sku_bits);
}

uint32
dnx_data_device_general_bist_enable_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_bist_enable);
}

uint32
dnx_data_device_general_nof_bits_for_nof_cores_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_define_nof_bits_for_nof_cores);
}



const dnx_data_device_general_ecc_err_masking_t *
dnx_data_device_general_ecc_err_masking_get(
    int unit,
    int block_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_table_ecc_err_masking);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, block_type, 0);
    return (const dnx_data_device_general_ecc_err_masking_t *) data;

}


shr_error_e
dnx_data_device_general_ecc_err_masking_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_device_general_ecc_err_masking_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_table_ecc_err_masking);
    data = (const dnx_data_device_general_ecc_err_masking_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ecc_1bit_err_mask_reg);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_MAX_NOF_ECC_ERR_MEMS, data->ecc_1bit_err_field);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ecc_2bit_err_mask_reg);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_MAX_NOF_ECC_ERR_MEMS, data->ecc_2bit_err_field);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_device_general_ecc_err_masking_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_device, dnx_data_device_submodule_general, dnx_data_device_general_table_ecc_err_masking);

}






static shr_error_e
dnx_data_device_regression_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "regression";
    submodule_data->doc = "regression attributes";
    
    submodule_data->nof_features = _dnx_data_device_regression_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device regression features");

    
    submodule_data->nof_defines = _dnx_data_device_regression_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device regression defines");

    
    submodule_data->nof_tables = _dnx_data_device_regression_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device regression tables");

    
    submodule_data->tables[dnx_data_device_regression_table_regression_parms].name = "regression_parms";
    submodule_data->tables[dnx_data_device_regression_table_regression_parms].doc = "system mode SoC property";
    submodule_data->tables[dnx_data_device_regression_table_regression_parms].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_device_regression_table_regression_parms].size_of_values = sizeof(dnx_data_device_regression_regression_parms_t);
    submodule_data->tables[dnx_data_device_regression_table_regression_parms].entry_get = dnx_data_device_regression_regression_parms_entry_str_get;

    
    submodule_data->tables[dnx_data_device_regression_table_regression_parms].nof_keys = 0;

    
    submodule_data->tables[dnx_data_device_regression_table_regression_parms].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_device_regression_table_regression_parms].values, dnxc_data_value_t, submodule_data->tables[dnx_data_device_regression_table_regression_parms].nof_values, "_dnx_data_device_regression_table_regression_parms table values");
    submodule_data->tables[dnx_data_device_regression_table_regression_parms].values[0].name = "system_mode_name";
    submodule_data->tables[dnx_data_device_regression_table_regression_parms].values[0].type = "char *";
    submodule_data->tables[dnx_data_device_regression_table_regression_parms].values[0].doc = "The system mode name";
    submodule_data->tables[dnx_data_device_regression_table_regression_parms].values[0].offset = UTILEX_OFFSETOF(dnx_data_device_regression_regression_parms_t, system_mode_name);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_device_regression_feature_get(
    int unit,
    dnx_data_device_regression_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_regression, feature);
}




const dnx_data_device_regression_regression_parms_t *
dnx_data_device_regression_regression_parms_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_device, dnx_data_device_submodule_regression, dnx_data_device_regression_table_regression_parms);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_device_regression_regression_parms_t *) data;

}


shr_error_e
dnx_data_device_regression_regression_parms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_device_regression_regression_parms_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_device, dnx_data_device_submodule_regression, dnx_data_device_regression_table_regression_parms);
    data = (const dnx_data_device_regression_regression_parms_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->system_mode_name == NULL ? "" : data->system_mode_name);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_device_regression_regression_parms_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_device, dnx_data_device_submodule_regression, dnx_data_device_regression_table_regression_parms);

}






static shr_error_e
dnx_data_device_emulation_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "emulation";
    submodule_data->doc = "Emulation attributes";
    
    submodule_data->nof_features = _dnx_data_device_emulation_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data device emulation features");

    submodule_data->features[dnx_data_device_emulation_fmac_supported].name = "fmac_supported";
    submodule_data->features[dnx_data_device_emulation_fmac_supported].doc = "if set to 0 it means fmac is not simulated, should be skipped during initialization.";
    submodule_data->features[dnx_data_device_emulation_fmac_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_device_emulation_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data device emulation defines");

    submodule_data->defines[dnx_data_device_emulation_define_emulation_system].name = "emulation_system";
    submodule_data->defines[dnx_data_device_emulation_define_emulation_system].doc = "Checking if the device is ran on emulation.";
    
    submodule_data->defines[dnx_data_device_emulation_define_emulation_system].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_device_emulation_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data device emulation tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_device_emulation_feature_get(
    int unit,
    dnx_data_device_emulation_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_emulation, feature);
}


uint32
dnx_data_device_emulation_emulation_system_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_device, dnx_data_device_submodule_emulation, dnx_data_device_emulation_define_emulation_system);
}







shr_error_e
dnx_data_device_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "device";
    module_data->nof_submodules = _dnx_data_device_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data device submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_device_general_init(unit, &module_data->submodules[dnx_data_device_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_device_regression_init(unit, &module_data->submodules[dnx_data_device_submodule_regression]));
    SHR_IF_ERR_EXIT(dnx_data_device_emulation_init(unit, &module_data->submodules[dnx_data_device_submodule_emulation]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_device_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_device_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b1_data_device_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_device_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a1_data_device_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_device_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_device_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b1_data_device_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_device_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_device_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

