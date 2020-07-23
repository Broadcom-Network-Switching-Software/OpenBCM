

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_DEVICE_H_

#define _DNX_DATA_DEVICE_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_device_init(
    int unit);






typedef struct
{
    
    int ecc_1bit_err_mask_reg;
    
    int ecc_1bit_err_field[DNX_MAX_NOF_ECC_ERR_MEMS];
    
    int ecc_2bit_err_mask_reg;
    
    int ecc_2bit_err_field[DNX_MAX_NOF_ECC_ERR_MEMS];
} dnx_data_device_general_ecc_err_masking_t;



typedef enum
{
    
    dnx_data_device_general_production_ready,
    
    dnx_data_device_general_hard_reset_disable,
    
    dnx_data_device_general_multi_core,
    
    dnx_data_device_general_rqp_ecc_err_exist,
    
    dnx_data_device_general_new_crc32_calc,
    
    dnx_data_device_general_protection_state_ignore,
    
    dnx_data_device_general_delay_exist,
    
    dnx_data_device_general_serdes_support,
    
    dnx_data_device_general_snooping_support,
    
    dnx_data_device_general_mirroring_support,
    
    dnx_data_device_general_injection_support,
    
    dnx_data_device_general_fabric_support,
    
    dnx_data_device_general_flexe_support,
    
    dnx_data_device_general_cmic_support,
    
    dnx_data_device_general_real_interrupts_support,
    
    dnx_data_device_general_i2c_hw_support,
    
    dnx_data_device_general_ring_protection_hw_support,
    
    dnx_data_device_general_hw_support,
    
    dnx_data_device_general_ext_encap_is_supported,

    
    _dnx_data_device_general_feature_nof
} dnx_data_device_general_feature_e;



typedef int(
    *dnx_data_device_general_feature_get_f) (
    int unit,
    dnx_data_device_general_feature_e feature);


typedef uint32(
    *dnx_data_device_general_max_nof_system_ports_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_invalid_system_port_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_fmq_system_port_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_invalid_fap_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_fap_ids_per_core_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_cores_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_cores_for_traffic_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_sub_units_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_core_max_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_max_nof_fap_ids_per_core_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_max_nof_fap_ids_per_device_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_core_clock_khz_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_system_ref_core_clock_khz_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_ref_core_clock_mhz_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_bus_size_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_maximal_core_bandwidth_kbps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_packet_per_clock_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_faps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_device_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_sku_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_bist_enable_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_bits_for_nof_cores_get_f) (
    int unit);


typedef const dnx_data_device_general_ecc_err_masking_t *(
    *dnx_data_device_general_ecc_err_masking_get_f) (
    int unit,
    int block_type);



typedef struct
{
    
    dnx_data_device_general_feature_get_f feature_get;
    
    dnx_data_device_general_max_nof_system_ports_get_f max_nof_system_ports_get;
    
    dnx_data_device_general_invalid_system_port_get_f invalid_system_port_get;
    
    dnx_data_device_general_fmq_system_port_get_f fmq_system_port_get;
    
    dnx_data_device_general_invalid_fap_id_get_f invalid_fap_id_get;
    
    dnx_data_device_general_fap_ids_per_core_bits_get_f fap_ids_per_core_bits_get;
    
    dnx_data_device_general_nof_cores_get_f nof_cores_get;
    
    dnx_data_device_general_nof_cores_for_traffic_get_f nof_cores_for_traffic_get;
    
    dnx_data_device_general_nof_sub_units_get_f nof_sub_units_get;
    
    dnx_data_device_general_core_max_nof_bits_get_f core_max_nof_bits_get;
    
    dnx_data_device_general_max_nof_fap_ids_per_core_get_f max_nof_fap_ids_per_core_get;
    
    dnx_data_device_general_max_nof_fap_ids_per_device_get_f max_nof_fap_ids_per_device_get;
    
    dnx_data_device_general_core_clock_khz_get_f core_clock_khz_get;
    
    dnx_data_device_general_system_ref_core_clock_khz_get_f system_ref_core_clock_khz_get;
    
    dnx_data_device_general_ref_core_clock_mhz_get_f ref_core_clock_mhz_get;
    
    dnx_data_device_general_bus_size_in_bits_get_f bus_size_in_bits_get;
    
    dnx_data_device_general_maximal_core_bandwidth_kbps_get_f maximal_core_bandwidth_kbps_get;
    
    dnx_data_device_general_packet_per_clock_get_f packet_per_clock_get;
    
    dnx_data_device_general_nof_faps_get_f nof_faps_get;
    
    dnx_data_device_general_device_id_get_f device_id_get;
    
    dnx_data_device_general_nof_sku_bits_get_f nof_sku_bits_get;
    
    dnx_data_device_general_bist_enable_get_f bist_enable_get;
    
    dnx_data_device_general_nof_bits_for_nof_cores_get_f nof_bits_for_nof_cores_get;
    
    dnx_data_device_general_ecc_err_masking_get_f ecc_err_masking_get;
    
    dnxc_data_table_info_get_f ecc_err_masking_info_get;
} dnx_data_if_device_general_t;






typedef struct
{
    
    char *system_mode_name;
} dnx_data_device_regression_regression_parms_t;



typedef enum
{

    
    _dnx_data_device_regression_feature_nof
} dnx_data_device_regression_feature_e;



typedef int(
    *dnx_data_device_regression_feature_get_f) (
    int unit,
    dnx_data_device_regression_feature_e feature);


typedef const dnx_data_device_regression_regression_parms_t *(
    *dnx_data_device_regression_regression_parms_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_device_regression_feature_get_f feature_get;
    
    dnx_data_device_regression_regression_parms_get_f regression_parms_get;
    
    dnxc_data_table_info_get_f regression_parms_info_get;
} dnx_data_if_device_regression_t;







typedef enum
{
    
    dnx_data_device_emulation_fmac_supported,

    
    _dnx_data_device_emulation_feature_nof
} dnx_data_device_emulation_feature_e;



typedef int(
    *dnx_data_device_emulation_feature_get_f) (
    int unit,
    dnx_data_device_emulation_feature_e feature);


typedef uint32(
    *dnx_data_device_emulation_emulation_system_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_device_emulation_feature_get_f feature_get;
    
    dnx_data_device_emulation_emulation_system_get_f emulation_system_get;
} dnx_data_if_device_emulation_t;





typedef struct
{
    
    dnx_data_if_device_general_t general;
    
    dnx_data_if_device_regression_t regression;
    
    dnx_data_if_device_emulation_t emulation;
} dnx_data_if_device_t;




extern dnx_data_if_device_t dnx_data_device;


#endif 

