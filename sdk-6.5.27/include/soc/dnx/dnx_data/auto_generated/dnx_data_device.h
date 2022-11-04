
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_DEVICE_H_

#define _DNX_DATA_DEVICE_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
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


typedef struct
{
    char *family_name;
    char *family_number;
} dnx_data_device_general_ctest_full_file_indentifier_t;



typedef enum
{
    dnx_data_device_general_production_ready,
    dnx_data_device_general_hard_reset_disable,
    dnx_data_device_general_stop_on_access_init_error,
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
    dnx_data_device_general_fai_unknown_address_supported,
    dnx_data_device_general_itpp_cud_valid_check,
    dnx_data_device_general_high_traffic_support,
    dnx_data_device_general_ire_negative_delta_support,
    dnx_data_device_general_ipc_exists,
    dnx_data_device_general_disable_maximal_core_bandwidth_support,
    dnx_data_device_general_disable_gated_top_level_sampling,
    dnx_data_device_general_cross_connect_support,
    dnx_data_device_general_ignore_mdb_sample_diff,
    dnx_data_device_general_xltcam_support,

    
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
    *dnx_data_device_general_max_nof_faps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_fap_ids_per_core_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_cores_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_valid_cores_bitmap_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_cores_for_traffic_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_max_nof_layer_protocols_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_ipcs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_core_groups_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_cores_per_core_group_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_nof_avs_status_bits_get_f) (
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
    *dnx_data_device_general_core_clock_ndiv_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_core_clock_mdiv_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_core_clock_ndiv_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_core_clock_mdiv_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_bus_size_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_maximal_core_bandwidth_mbps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_enforce_maximal_core_bandwidth_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_general_max_mpps_get_f) (
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


typedef const dnx_data_device_general_ctest_full_file_indentifier_t *(
    *dnx_data_device_general_ctest_full_file_indentifier_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_device_general_feature_get_f feature_get;
    
    dnx_data_device_general_max_nof_system_ports_get_f max_nof_system_ports_get;
    
    dnx_data_device_general_invalid_system_port_get_f invalid_system_port_get;
    
    dnx_data_device_general_fmq_system_port_get_f fmq_system_port_get;
    
    dnx_data_device_general_invalid_fap_id_get_f invalid_fap_id_get;
    
    dnx_data_device_general_max_nof_faps_get_f max_nof_faps_get;
    
    dnx_data_device_general_fap_ids_per_core_bits_get_f fap_ids_per_core_bits_get;
    
    dnx_data_device_general_nof_cores_get_f nof_cores_get;
    
    dnx_data_device_general_valid_cores_bitmap_get_f valid_cores_bitmap_get;
    
    dnx_data_device_general_nof_cores_for_traffic_get_f nof_cores_for_traffic_get;
    
    dnx_data_device_general_max_nof_layer_protocols_get_f max_nof_layer_protocols_get;
    
    dnx_data_device_general_nof_ipcs_get_f nof_ipcs_get;
    
    dnx_data_device_general_nof_core_groups_get_f nof_core_groups_get;
    
    dnx_data_device_general_nof_cores_per_core_group_get_f nof_cores_per_core_group_get;
    
    dnx_data_device_general_nof_avs_status_bits_get_f nof_avs_status_bits_get;
    
    dnx_data_device_general_core_max_nof_bits_get_f core_max_nof_bits_get;
    
    dnx_data_device_general_max_nof_fap_ids_per_core_get_f max_nof_fap_ids_per_core_get;
    
    dnx_data_device_general_max_nof_fap_ids_per_device_get_f max_nof_fap_ids_per_device_get;
    
    dnx_data_device_general_core_clock_khz_get_f core_clock_khz_get;
    
    dnx_data_device_general_system_ref_core_clock_khz_get_f system_ref_core_clock_khz_get;
    
    dnx_data_device_general_ref_core_clock_mhz_get_f ref_core_clock_mhz_get;
    
    dnx_data_device_general_core_clock_ndiv_size_get_f core_clock_ndiv_size_get;
    
    dnx_data_device_general_core_clock_mdiv_size_get_f core_clock_mdiv_size_get;
    
    dnx_data_device_general_core_clock_ndiv_offset_get_f core_clock_ndiv_offset_get;
    
    dnx_data_device_general_core_clock_mdiv_offset_get_f core_clock_mdiv_offset_get;
    
    dnx_data_device_general_bus_size_in_bits_get_f bus_size_in_bits_get;
    
    dnx_data_device_general_maximal_core_bandwidth_mbps_get_f maximal_core_bandwidth_mbps_get;
    
    dnx_data_device_general_enforce_maximal_core_bandwidth_get_f enforce_maximal_core_bandwidth_get;
    
    dnx_data_device_general_max_mpps_get_f max_mpps_get;
    
    dnx_data_device_general_packet_per_clock_get_f packet_per_clock_get;
    
    dnx_data_device_general_nof_faps_get_f nof_faps_get;
    
    dnx_data_device_general_device_id_get_f device_id_get;
    
    dnx_data_device_general_nof_sku_bits_get_f nof_sku_bits_get;
    
    dnx_data_device_general_bist_enable_get_f bist_enable_get;
    
    dnx_data_device_general_nof_bits_for_nof_cores_get_f nof_bits_for_nof_cores_get;
    
    dnx_data_device_general_ecc_err_masking_get_f ecc_err_masking_get;
    
    dnxc_data_table_info_get_f ecc_err_masking_info_get;
    
    dnx_data_device_general_ctest_full_file_indentifier_get_f ctest_full_file_indentifier_get;
    
    dnxc_data_table_info_get_f ctest_full_file_indentifier_info_get;
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
    dnx_data_device_emulation_ecc_enable,

    
    _dnx_data_device_emulation_feature_nof
} dnx_data_device_emulation_feature_e;



typedef int(
    *dnx_data_device_emulation_feature_get_f) (
    int unit,
    dnx_data_device_emulation_feature_e feature);


typedef uint32(
    *dnx_data_device_emulation_emulation_system_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_emulation_framer_init_mode_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_device_emulation_feature_get_f feature_get;
    
    dnx_data_device_emulation_emulation_system_get_f emulation_system_get;
    
    dnx_data_device_emulation_framer_init_mode_get_f framer_init_mode_get;
} dnx_data_if_device_emulation_t;






typedef struct
{
    int block_id;
    int is_supported;
    int size;
} dnx_data_device_otp_otp_instance_attributes_t;


typedef struct
{
    uint32 mode;
    uint32 cpu_addr;
    uint32 cmd;
    uint32 cmd_start;
    uint32 cpu_status;
    uint32 cpu_data_0;
    uint32 cpu_data_1;
    uint32 cpu_data_2;
} dnx_data_device_otp_reg_addresses_t;



typedef enum
{

    
    _dnx_data_device_otp_feature_nof
} dnx_data_device_otp_feature_e;



typedef int(
    *dnx_data_device_otp_feature_get_f) (
    int unit,
    dnx_data_device_otp_feature_e feature);


typedef uint32(
    *dnx_data_device_otp_nof_words_in_row_get_f) (
    int unit);


typedef uint32(
    *dnx_data_device_otp_read_timeout_get_f) (
    int unit);


typedef const dnx_data_device_otp_otp_instance_attributes_t *(
    *dnx_data_device_otp_otp_instance_attributes_get_f) (
    int unit,
    int otp_instance_index);


typedef const dnx_data_device_otp_reg_addresses_t *(
    *dnx_data_device_otp_reg_addresses_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_device_otp_feature_get_f feature_get;
    
    dnx_data_device_otp_nof_words_in_row_get_f nof_words_in_row_get;
    
    dnx_data_device_otp_read_timeout_get_f read_timeout_get;
    
    dnx_data_device_otp_otp_instance_attributes_get_f otp_instance_attributes_get;
    
    dnxc_data_table_info_get_f otp_instance_attributes_info_get;
    
    dnx_data_device_otp_reg_addresses_get_f reg_addresses_get;
    
    dnxc_data_table_info_get_f reg_addresses_info_get;
} dnx_data_if_device_otp_t;







typedef enum
{
    dnx_data_device_docs_is_fap,

    
    _dnx_data_device_docs_feature_nof
} dnx_data_device_docs_feature_e;



typedef int(
    *dnx_data_device_docs_feature_get_f) (
    int unit,
    dnx_data_device_docs_feature_e feature);



typedef struct
{
    
    dnx_data_device_docs_feature_get_f feature_get;
} dnx_data_if_device_docs_t;





typedef struct
{
    
    dnx_data_if_device_general_t general;
    
    dnx_data_if_device_regression_t regression;
    
    dnx_data_if_device_emulation_t emulation;
    
    dnx_data_if_device_otp_t otp;
    
    dnx_data_if_device_docs_t docs;
} dnx_data_if_device_t;




extern dnx_data_if_device_t dnx_data_device;


#endif 

