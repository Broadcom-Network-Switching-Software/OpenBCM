
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_DEVICE_H_

#define _DNXF_DATA_DEVICE_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxf/dnxf_data/dnxf_data_verify.h>
#include <soc/types.h>
#include <soc/chip.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_device.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif



shr_error_e dnxf_data_if_device_init(
    int unit);






typedef struct
{
    char *family_name;
    char *family_number;
} dnxf_data_device_general_ctest_full_file_indentifier_t;



typedef enum
{
    dnxf_data_device_general_production_ready,
    dnxf_data_device_general_serdes_support,
    dnxf_data_device_general_fabric_support,
    dnxf_data_device_general_delay_exist,
    dnxf_data_device_general_cmic_support,
    dnxf_data_device_general_real_interrupts_support,
    dnxf_data_device_general_i2c_hw_support,
    dnxf_data_device_general_hw_support,
    dnxf_data_device_general_is_not_all_cores_active,
    dnxf_data_device_general_fabric_advanced_power_down,

    
    _dnxf_data_device_general_feature_nof
} dnxf_data_device_general_feature_e;



typedef int(
    *dnxf_data_device_general_feature_get_f) (
    int unit,
    dnxf_data_device_general_feature_e feature);


typedef uint32(
    *dnxf_data_device_general_nof_pvt_monitors_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_general_nof_main_monitors_get_f) (
    int unit);


typedef int(
    *dnxf_data_device_general_pvt_base_get_f) (
    int unit);


typedef int(
    *dnxf_data_device_general_pvt_factor_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_general_nof_cores_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_general_adapter_lib_ver_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_general_nof_mesh_topology_links_data_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_general_hard_reset_disable_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_general_core_clock_khz_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_general_system_ref_core_clock_khz_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_general_device_id_get_f) (
    int unit);


typedef const dnxf_data_device_general_ctest_full_file_indentifier_t *(
    *dnxf_data_device_general_ctest_full_file_indentifier_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_device_general_feature_get_f feature_get;
    
    dnxf_data_device_general_nof_pvt_monitors_get_f nof_pvt_monitors_get;
    
    dnxf_data_device_general_nof_main_monitors_get_f nof_main_monitors_get;
    
    dnxf_data_device_general_pvt_base_get_f pvt_base_get;
    
    dnxf_data_device_general_pvt_factor_get_f pvt_factor_get;
    
    dnxf_data_device_general_nof_cores_get_f nof_cores_get;
    
    dnxf_data_device_general_adapter_lib_ver_get_f adapter_lib_ver_get;
    
    dnxf_data_device_general_nof_mesh_topology_links_data_get_f nof_mesh_topology_links_data_get;
    
    dnxf_data_device_general_hard_reset_disable_get_f hard_reset_disable_get;
    
    dnxf_data_device_general_core_clock_khz_get_f core_clock_khz_get;
    
    dnxf_data_device_general_system_ref_core_clock_khz_get_f system_ref_core_clock_khz_get;
    
    dnxf_data_device_general_device_id_get_f device_id_get;
    
    dnxf_data_device_general_ctest_full_file_indentifier_get_f ctest_full_file_indentifier_get;
    
    dnxc_data_table_info_get_f ctest_full_file_indentifier_info_get;
} dnxf_data_if_device_general_t;







typedef enum
{

    
    _dnxf_data_device_access_feature_nof
} dnxf_data_device_access_feature_e;



typedef int(
    *dnxf_data_device_access_feature_get_f) (
    int unit,
    dnxf_data_device_access_feature_e feature);


typedef uint32(
    *dnxf_data_device_access_mdio_int_divisor_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_mdio_ext_divisor_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_mdio_int_div_out_delay_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_mdio_ext_div_out_delay_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_bist_enable_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_mem_clear_chunk_size_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_device_access_feature_get_f feature_get;
    
    dnxf_data_device_access_mdio_int_divisor_get_f mdio_int_divisor_get;
    
    dnxf_data_device_access_mdio_ext_divisor_get_f mdio_ext_divisor_get;
    
    dnxf_data_device_access_mdio_int_div_out_delay_get_f mdio_int_div_out_delay_get;
    
    dnxf_data_device_access_mdio_ext_div_out_delay_get_f mdio_ext_div_out_delay_get;
    
    dnxf_data_device_access_bist_enable_get_f bist_enable_get;
    
    dnxf_data_device_access_mem_clear_chunk_size_get_f mem_clear_chunk_size_get;
} dnxf_data_if_device_access_t;






typedef struct
{
    char *block_type;
    int block_instance;
    int value;
} dnxf_data_device_blocks_override_t;


typedef struct
{
    int index;
} dnxf_data_device_blocks_fsrd_sbus_chain_t;


typedef struct
{
    int index;
} dnxf_data_device_blocks_fmac_sbus_chain_t;


typedef struct
{
    int rtp_link;
} dnxf_data_device_blocks_fmac_lane_to_block_map_t;


typedef struct
{
    soc_pbmp_t dch_instance_bmp;
} dnxf_data_device_blocks_dch_instances_to_device_stage_t;


typedef struct
{
    int block_type;
    int clock_reg;
    int clock_field;
    uint32 soft_init_fields[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_RTP];
} dnxf_data_device_blocks_power_down_per_rtp_t;


typedef struct
{
    int block_type;
    int clock_reg;
    int clock_field;
    uint32 soft_init_fields[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCH];
} dnxf_data_device_blocks_power_down_per_dch_t;


typedef struct
{
    int block_type;
    int clock_reg;
    int clock_field;
    uint32 soft_init_fields[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_FMAC];
} dnxf_data_device_blocks_power_down_per_fmac_t;


typedef struct
{
    int block_type;
    int clock_reg;
    int clock_field;
    uint32 soft_init_fields[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_LINKS_IN_DCH];
} dnxf_data_device_blocks_power_down_per_dfl_fbc_t;



typedef enum
{
    dnxf_data_device_blocks_dch_reset_restore_support,

    
    _dnxf_data_device_blocks_feature_nof
} dnxf_data_device_blocks_feature_e;



typedef int(
    *dnxf_data_device_blocks_feature_get_f) (
    int unit,
    dnxf_data_device_blocks_feature_e feature);


typedef uint32(
    *dnxf_data_device_blocks_nof_all_blocks_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_fmac_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_links_in_fmac_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_fsrd_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_brdc_fsrd_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_dch_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_links_in_dch_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_dch_link_groups_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_cch_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_links_in_cch_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_links_in_dcq_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_rtp_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_links_in_rtp_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_occg_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_eci_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_cmic_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_mesh_topology_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_brdc_fmach_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_brdc_fmacl_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_lcm_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_mct_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_qrh_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_dcml_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_links_in_dcml_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_links_in_lcm_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_links_in_qrh_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_dtm_fifos_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_links_in_fsrd_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_links_in_phy_core_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_fmacs_in_fsrd_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_pcu_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_ccml_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_dfl_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_dtm_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_pads_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_dfl_fbc_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_dtl_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_mesh_topology_wrap_get_f) (
    int unit);


typedef const dnxf_data_device_blocks_override_t *(
    *dnxf_data_device_blocks_override_get_f) (
    int unit,
    int index);


typedef const dnxf_data_device_blocks_fsrd_sbus_chain_t *(
    *dnxf_data_device_blocks_fsrd_sbus_chain_get_f) (
    int unit,
    int fsrd_id);


typedef const dnxf_data_device_blocks_fmac_sbus_chain_t *(
    *dnxf_data_device_blocks_fmac_sbus_chain_get_f) (
    int unit,
    int fmac_id);


typedef const dnxf_data_device_blocks_fmac_lane_to_block_map_t *(
    *dnxf_data_device_blocks_fmac_lane_to_block_map_get_f) (
    int unit,
    int fmac_lane);


typedef const dnxf_data_device_blocks_dch_instances_to_device_stage_t *(
    *dnxf_data_device_blocks_dch_instances_to_device_stage_get_f) (
    int unit,
    int device_stage);


typedef const dnxf_data_device_blocks_power_down_per_rtp_t *(
    *dnxf_data_device_blocks_power_down_per_rtp_get_f) (
    int unit,
    int index);


typedef const dnxf_data_device_blocks_power_down_per_dch_t *(
    *dnxf_data_device_blocks_power_down_per_dch_get_f) (
    int unit,
    int index);


typedef const dnxf_data_device_blocks_power_down_per_fmac_t *(
    *dnxf_data_device_blocks_power_down_per_fmac_get_f) (
    int unit,
    int index);


typedef const dnxf_data_device_blocks_power_down_per_dfl_fbc_t *(
    *dnxf_data_device_blocks_power_down_per_dfl_fbc_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnxf_data_device_blocks_feature_get_f feature_get;
    
    dnxf_data_device_blocks_nof_all_blocks_get_f nof_all_blocks_get;
    
    dnxf_data_device_blocks_nof_instances_fmac_get_f nof_instances_fmac_get;
    
    dnxf_data_device_blocks_nof_links_in_fmac_get_f nof_links_in_fmac_get;
    
    dnxf_data_device_blocks_nof_instances_fsrd_get_f nof_instances_fsrd_get;
    
    dnxf_data_device_blocks_nof_instances_brdc_fsrd_get_f nof_instances_brdc_fsrd_get;
    
    dnxf_data_device_blocks_nof_instances_dch_get_f nof_instances_dch_get;
    
    dnxf_data_device_blocks_nof_links_in_dch_get_f nof_links_in_dch_get;
    
    dnxf_data_device_blocks_nof_dch_link_groups_get_f nof_dch_link_groups_get;
    
    dnxf_data_device_blocks_nof_instances_cch_get_f nof_instances_cch_get;
    
    dnxf_data_device_blocks_nof_links_in_cch_get_f nof_links_in_cch_get;
    
    dnxf_data_device_blocks_nof_links_in_dcq_get_f nof_links_in_dcq_get;
    
    dnxf_data_device_blocks_nof_instances_rtp_get_f nof_instances_rtp_get;
    
    dnxf_data_device_blocks_nof_links_in_rtp_get_f nof_links_in_rtp_get;
    
    dnxf_data_device_blocks_nof_instances_occg_get_f nof_instances_occg_get;
    
    dnxf_data_device_blocks_nof_instances_eci_get_f nof_instances_eci_get;
    
    dnxf_data_device_blocks_nof_instances_cmic_get_f nof_instances_cmic_get;
    
    dnxf_data_device_blocks_nof_instances_mesh_topology_get_f nof_instances_mesh_topology_get;
    
    dnxf_data_device_blocks_nof_instances_brdc_fmach_get_f nof_instances_brdc_fmach_get;
    
    dnxf_data_device_blocks_nof_instances_brdc_fmacl_get_f nof_instances_brdc_fmacl_get;
    
    dnxf_data_device_blocks_nof_instances_lcm_get_f nof_instances_lcm_get;
    
    dnxf_data_device_blocks_nof_instances_mct_get_f nof_instances_mct_get;
    
    dnxf_data_device_blocks_nof_instances_qrh_get_f nof_instances_qrh_get;
    
    dnxf_data_device_blocks_nof_instances_dcml_get_f nof_instances_dcml_get;
    
    dnxf_data_device_blocks_nof_links_in_dcml_get_f nof_links_in_dcml_get;
    
    dnxf_data_device_blocks_nof_links_in_lcm_get_f nof_links_in_lcm_get;
    
    dnxf_data_device_blocks_nof_links_in_qrh_get_f nof_links_in_qrh_get;
    
    dnxf_data_device_blocks_nof_dtm_fifos_get_f nof_dtm_fifos_get;
    
    dnxf_data_device_blocks_nof_links_in_fsrd_get_f nof_links_in_fsrd_get;
    
    dnxf_data_device_blocks_nof_links_in_phy_core_get_f nof_links_in_phy_core_get;
    
    dnxf_data_device_blocks_nof_fmacs_in_fsrd_get_f nof_fmacs_in_fsrd_get;
    
    dnxf_data_device_blocks_nof_instances_pcu_get_f nof_instances_pcu_get;
    
    dnxf_data_device_blocks_nof_instances_ccml_get_f nof_instances_ccml_get;
    
    dnxf_data_device_blocks_nof_instances_dfl_get_f nof_instances_dfl_get;
    
    dnxf_data_device_blocks_nof_instances_dtm_get_f nof_instances_dtm_get;
    
    dnxf_data_device_blocks_nof_instances_pads_get_f nof_instances_pads_get;
    
    dnxf_data_device_blocks_nof_instances_dfl_fbc_get_f nof_instances_dfl_fbc_get;
    
    dnxf_data_device_blocks_nof_instances_dtl_get_f nof_instances_dtl_get;
    
    dnxf_data_device_blocks_nof_instances_mesh_topology_wrap_get_f nof_instances_mesh_topology_wrap_get;
    
    dnxf_data_device_blocks_override_get_f override_get;
    
    dnxc_data_table_info_get_f override_info_get;
    
    dnxf_data_device_blocks_fsrd_sbus_chain_get_f fsrd_sbus_chain_get;
    
    dnxc_data_table_info_get_f fsrd_sbus_chain_info_get;
    
    dnxf_data_device_blocks_fmac_sbus_chain_get_f fmac_sbus_chain_get;
    
    dnxc_data_table_info_get_f fmac_sbus_chain_info_get;
    
    dnxf_data_device_blocks_fmac_lane_to_block_map_get_f fmac_lane_to_block_map_get;
    
    dnxc_data_table_info_get_f fmac_lane_to_block_map_info_get;
    
    dnxf_data_device_blocks_dch_instances_to_device_stage_get_f dch_instances_to_device_stage_get;
    
    dnxc_data_table_info_get_f dch_instances_to_device_stage_info_get;
    
    dnxf_data_device_blocks_power_down_per_rtp_get_f power_down_per_rtp_get;
    
    dnxc_data_table_info_get_f power_down_per_rtp_info_get;
    
    dnxf_data_device_blocks_power_down_per_dch_get_f power_down_per_dch_get;
    
    dnxc_data_table_info_get_f power_down_per_dch_info_get;
    
    dnxf_data_device_blocks_power_down_per_fmac_get_f power_down_per_fmac_get;
    
    dnxc_data_table_info_get_f power_down_per_fmac_info_get;
    
    dnxf_data_device_blocks_power_down_per_dfl_fbc_get_f power_down_per_dfl_fbc_get;
    
    dnxc_data_table_info_get_f power_down_per_dfl_fbc_info_get;
} dnxf_data_if_device_blocks_t;







typedef enum
{

    
    _dnxf_data_device_interrupts_feature_nof
} dnxf_data_device_interrupts_feature_e;



typedef int(
    *dnxf_data_device_interrupts_feature_get_f) (
    int unit,
    dnxf_data_device_interrupts_feature_e feature);


typedef uint32(
    *dnxf_data_device_interrupts_nof_interrupts_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_device_interrupts_feature_get_f feature_get;
    
    dnxf_data_device_interrupts_nof_interrupts_get_f nof_interrupts_get;
} dnxf_data_if_device_interrupts_t;






typedef struct
{
    char *name;
    int control_reg;
    int lock_bit;
    int remote_sensor_supported;
    uint32 vtmon_sel_field;
    int peak_data_clear_field;
    int temp_mon_sel;
    int data_reg;
    int thermal_data_field;
    int peak_data_field;
    int block_instance;
    int voltage_sensing;
    uint32 vdd_sel;
    uint32 vss_sel;
    int voltage_bit;
} dnxf_data_device_vtmon_vtmon_access_t;



typedef enum
{
    dnxf_data_device_vtmon_remote_temp_sensors_supported,

    
    _dnxf_data_device_vtmon_feature_nof
} dnxf_data_device_vtmon_feature_e;



typedef int(
    *dnxf_data_device_vtmon_feature_get_f) (
    int unit,
    dnxf_data_device_vtmon_feature_e feature);


typedef uint32(
    *dnxf_data_device_vtmon_vol_factor_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_vtmon_vol_multiple_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_vtmon_vol_divisor_get_f) (
    int unit);


typedef const dnxf_data_device_vtmon_vtmon_access_t *(
    *dnxf_data_device_vtmon_vtmon_access_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnxf_data_device_vtmon_feature_get_f feature_get;
    
    dnxf_data_device_vtmon_vol_factor_get_f vol_factor_get;
    
    dnxf_data_device_vtmon_vol_multiple_get_f vol_multiple_get;
    
    dnxf_data_device_vtmon_vol_divisor_get_f vol_divisor_get;
    
    dnxf_data_device_vtmon_vtmon_access_get_f vtmon_access_get;
    
    dnxc_data_table_info_get_f vtmon_access_info_get;
} dnxf_data_if_device_vtmon_t;







typedef enum
{

    
    _dnxf_data_device_custom_features_feature_nof
} dnxf_data_device_custom_features_feature_e;



typedef int(
    *dnxf_data_device_custom_features_feature_get_f) (
    int unit,
    dnxf_data_device_custom_features_feature_e feature);


typedef int(
    *dnxf_data_device_custom_features_mesh_topology_size_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_custom_features_mesh_topology_fast_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_custom_features_active_core_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_device_custom_features_feature_get_f feature_get;
    
    dnxf_data_device_custom_features_mesh_topology_size_get_f mesh_topology_size_get;
    
    dnxf_data_device_custom_features_mesh_topology_fast_get_f mesh_topology_fast_get;
    
    dnxf_data_device_custom_features_active_core_get_f active_core_get;
} dnxf_data_if_device_custom_features_t;






typedef struct
{
    char *property;
    char *suffix;
    int num_max;
    int per_port;
    char *err_msg;
} dnxf_data_device_properties_unsupported_t;



typedef enum
{

    
    _dnxf_data_device_properties_feature_nof
} dnxf_data_device_properties_feature_e;



typedef int(
    *dnxf_data_device_properties_feature_get_f) (
    int unit,
    dnxf_data_device_properties_feature_e feature);


typedef const dnxf_data_device_properties_unsupported_t *(
    *dnxf_data_device_properties_unsupported_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnxf_data_device_properties_feature_get_f feature_get;
    
    dnxf_data_device_properties_unsupported_get_f unsupported_get;
    
    dnxc_data_table_info_get_f unsupported_info_get;
} dnxf_data_if_device_properties_t;







typedef enum
{

    
    _dnxf_data_device_emulation_feature_nof
} dnxf_data_device_emulation_feature_e;



typedef int(
    *dnxf_data_device_emulation_feature_get_f) (
    int unit,
    dnxf_data_device_emulation_feature_e feature);


typedef uint32(
    *dnxf_data_device_emulation_emulation_system_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_device_emulation_feature_get_f feature_get;
    
    dnxf_data_device_emulation_emulation_system_get_f emulation_system_get;
} dnxf_data_if_device_emulation_t;






typedef struct
{
    int block_id;
    int is_supported;
    int size;
} dnxf_data_device_otp_otp_instance_attributes_t;


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
} dnxf_data_device_otp_reg_addresses_t;



typedef enum
{

    
    _dnxf_data_device_otp_feature_nof
} dnxf_data_device_otp_feature_e;



typedef int(
    *dnxf_data_device_otp_feature_get_f) (
    int unit,
    dnxf_data_device_otp_feature_e feature);


typedef uint32(
    *dnxf_data_device_otp_nof_words_in_row_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_otp_read_timeout_get_f) (
    int unit);


typedef const dnxf_data_device_otp_otp_instance_attributes_t *(
    *dnxf_data_device_otp_otp_instance_attributes_get_f) (
    int unit,
    int otp_instance_index);


typedef const dnxf_data_device_otp_reg_addresses_t *(
    *dnxf_data_device_otp_reg_addresses_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_device_otp_feature_get_f feature_get;
    
    dnxf_data_device_otp_nof_words_in_row_get_f nof_words_in_row_get;
    
    dnxf_data_device_otp_read_timeout_get_f read_timeout_get;
    
    dnxf_data_device_otp_otp_instance_attributes_get_f otp_instance_attributes_get;
    
    dnxc_data_table_info_get_f otp_instance_attributes_info_get;
    
    dnxf_data_device_otp_reg_addresses_get_f reg_addresses_get;
    
    dnxc_data_table_info_get_f reg_addresses_info_get;
} dnxf_data_if_device_otp_t;







typedef enum
{
    dnxf_data_device_docs_is_fap,

    
    _dnxf_data_device_docs_feature_nof
} dnxf_data_device_docs_feature_e;



typedef int(
    *dnxf_data_device_docs_feature_get_f) (
    int unit,
    dnxf_data_device_docs_feature_e feature);



typedef struct
{
    
    dnxf_data_device_docs_feature_get_f feature_get;
} dnxf_data_if_device_docs_t;





typedef struct
{
    
    dnxf_data_if_device_general_t general;
    
    dnxf_data_if_device_access_t access;
    
    dnxf_data_if_device_blocks_t blocks;
    
    dnxf_data_if_device_interrupts_t interrupts;
    
    dnxf_data_if_device_vtmon_t vtmon;
    
    dnxf_data_if_device_custom_features_t custom_features;
    
    dnxf_data_if_device_properties_t properties;
    
    dnxf_data_if_device_emulation_t emulation;
    
    dnxf_data_if_device_otp_t otp;
    
    dnxf_data_if_device_docs_t docs;
} dnxf_data_if_device_t;




extern dnxf_data_if_device_t dnxf_data_device;


#endif 

