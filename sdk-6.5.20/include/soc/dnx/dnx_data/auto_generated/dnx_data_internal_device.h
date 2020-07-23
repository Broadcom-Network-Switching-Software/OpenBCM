

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_DEVICE_H_

#define _DNX_DATA_INTERNAL_DEVICE_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_device_submodule_general,
    dnx_data_device_submodule_regression,
    dnx_data_device_submodule_emulation,

    
    _dnx_data_device_submodule_nof
} dnx_data_device_submodule_e;








int dnx_data_device_general_feature_get(
    int unit,
    dnx_data_device_general_feature_e feature);



typedef enum
{
    dnx_data_device_general_define_max_nof_system_ports,
    dnx_data_device_general_define_invalid_system_port,
    dnx_data_device_general_define_fmq_system_port,
    dnx_data_device_general_define_invalid_fap_id,
    dnx_data_device_general_define_fap_ids_per_core_bits,
    dnx_data_device_general_define_nof_cores,
    dnx_data_device_general_define_nof_cores_for_traffic,
    dnx_data_device_general_define_nof_sub_units,
    dnx_data_device_general_define_core_max_nof_bits,
    dnx_data_device_general_define_max_nof_fap_ids_per_core,
    dnx_data_device_general_define_max_nof_fap_ids_per_device,
    dnx_data_device_general_define_core_clock_khz,
    dnx_data_device_general_define_system_ref_core_clock_khz,
    dnx_data_device_general_define_ref_core_clock_mhz,
    dnx_data_device_general_define_bus_size_in_bits,
    dnx_data_device_general_define_maximal_core_bandwidth_kbps,
    dnx_data_device_general_define_packet_per_clock,
    dnx_data_device_general_define_nof_faps,
    dnx_data_device_general_define_device_id,
    dnx_data_device_general_define_nof_sku_bits,
    dnx_data_device_general_define_bist_enable,
    dnx_data_device_general_define_nof_bits_for_nof_cores,

    
    _dnx_data_device_general_define_nof
} dnx_data_device_general_define_e;



uint32 dnx_data_device_general_max_nof_system_ports_get(
    int unit);


uint32 dnx_data_device_general_invalid_system_port_get(
    int unit);


uint32 dnx_data_device_general_fmq_system_port_get(
    int unit);


uint32 dnx_data_device_general_invalid_fap_id_get(
    int unit);


uint32 dnx_data_device_general_fap_ids_per_core_bits_get(
    int unit);


uint32 dnx_data_device_general_nof_cores_get(
    int unit);


uint32 dnx_data_device_general_nof_cores_for_traffic_get(
    int unit);


uint32 dnx_data_device_general_nof_sub_units_get(
    int unit);


uint32 dnx_data_device_general_core_max_nof_bits_get(
    int unit);


uint32 dnx_data_device_general_max_nof_fap_ids_per_core_get(
    int unit);


uint32 dnx_data_device_general_max_nof_fap_ids_per_device_get(
    int unit);


uint32 dnx_data_device_general_core_clock_khz_get(
    int unit);


uint32 dnx_data_device_general_system_ref_core_clock_khz_get(
    int unit);


uint32 dnx_data_device_general_ref_core_clock_mhz_get(
    int unit);


uint32 dnx_data_device_general_bus_size_in_bits_get(
    int unit);


uint32 dnx_data_device_general_maximal_core_bandwidth_kbps_get(
    int unit);


uint32 dnx_data_device_general_packet_per_clock_get(
    int unit);


uint32 dnx_data_device_general_nof_faps_get(
    int unit);


uint32 dnx_data_device_general_device_id_get(
    int unit);


uint32 dnx_data_device_general_nof_sku_bits_get(
    int unit);


uint32 dnx_data_device_general_bist_enable_get(
    int unit);


uint32 dnx_data_device_general_nof_bits_for_nof_cores_get(
    int unit);



typedef enum
{
    dnx_data_device_general_table_ecc_err_masking,

    
    _dnx_data_device_general_table_nof
} dnx_data_device_general_table_e;



const dnx_data_device_general_ecc_err_masking_t * dnx_data_device_general_ecc_err_masking_get(
    int unit,
    int block_type);



shr_error_e dnx_data_device_general_ecc_err_masking_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_device_general_ecc_err_masking_info_get(
    int unit);






int dnx_data_device_regression_feature_get(
    int unit,
    dnx_data_device_regression_feature_e feature);



typedef enum
{

    
    _dnx_data_device_regression_define_nof
} dnx_data_device_regression_define_e;




typedef enum
{
    dnx_data_device_regression_table_regression_parms,

    
    _dnx_data_device_regression_table_nof
} dnx_data_device_regression_table_e;



const dnx_data_device_regression_regression_parms_t * dnx_data_device_regression_regression_parms_get(
    int unit);



shr_error_e dnx_data_device_regression_regression_parms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_device_regression_regression_parms_info_get(
    int unit);






int dnx_data_device_emulation_feature_get(
    int unit,
    dnx_data_device_emulation_feature_e feature);



typedef enum
{
    dnx_data_device_emulation_define_emulation_system,

    
    _dnx_data_device_emulation_define_nof
} dnx_data_device_emulation_define_e;



uint32 dnx_data_device_emulation_emulation_system_get(
    int unit);



typedef enum
{

    
    _dnx_data_device_emulation_table_nof
} dnx_data_device_emulation_table_e;






shr_error_e dnx_data_device_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

