

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_DEVICE_H_

#define _DNXF_DATA_DEVICE_H_

#include <soc/property.h>
#include <sal/limits.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxf/dnxf_data/dnxf_data_verify.h>
#include <soc/chip.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_device.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnxf_data_if_device_init(
    int unit);







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

    
    _dnxf_data_device_general_feature_nof
} dnxf_data_device_general_feature_e;



typedef int(
    *dnxf_data_device_general_feature_get_f) (
    int unit,
    dnxf_data_device_general_feature_e feature);


typedef uint32(
    *dnxf_data_device_general_nof_pvt_monitors_get_f) (
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



typedef struct
{
    
    dnxf_data_device_general_feature_get_f feature_get;
    
    dnxf_data_device_general_nof_pvt_monitors_get_f nof_pvt_monitors_get;
    
    dnxf_data_device_general_pvt_base_get_f pvt_base_get;
    
    dnxf_data_device_general_pvt_factor_get_f pvt_factor_get;
    
    dnxf_data_device_general_nof_cores_get_f nof_cores_get;
    
    dnxf_data_device_general_hard_reset_disable_get_f hard_reset_disable_get;
    
    dnxf_data_device_general_core_clock_khz_get_f core_clock_khz_get;
    
    dnxf_data_device_general_system_ref_core_clock_khz_get_f system_ref_core_clock_khz_get;
    
    dnxf_data_device_general_device_id_get_f device_id_get;
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
    *dnxf_data_device_access_table_dma_enable_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_tdma_timeout_usec_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_tdma_intr_enable_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_tslam_dma_enable_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_tslam_timeout_usec_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_tslam_int_enable_get_f) (
    int unit);


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
    *dnxf_data_device_access_sbus_dma_interval_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_sbus_dma_intr_enable_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_access_mem_clear_chunk_size_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_device_access_feature_get_f feature_get;
    
    dnxf_data_device_access_table_dma_enable_get_f table_dma_enable_get;
    
    dnxf_data_device_access_tdma_timeout_usec_get_f tdma_timeout_usec_get;
    
    dnxf_data_device_access_tdma_intr_enable_get_f tdma_intr_enable_get;
    
    dnxf_data_device_access_tslam_dma_enable_get_f tslam_dma_enable_get;
    
    dnxf_data_device_access_tslam_timeout_usec_get_f tslam_timeout_usec_get;
    
    dnxf_data_device_access_tslam_int_enable_get_f tslam_int_enable_get;
    
    dnxf_data_device_access_mdio_int_divisor_get_f mdio_int_divisor_get;
    
    dnxf_data_device_access_mdio_ext_divisor_get_f mdio_ext_divisor_get;
    
    dnxf_data_device_access_mdio_int_div_out_delay_get_f mdio_int_div_out_delay_get;
    
    dnxf_data_device_access_mdio_ext_div_out_delay_get_f mdio_ext_div_out_delay_get;
    
    dnxf_data_device_access_bist_enable_get_f bist_enable_get;
    
    dnxf_data_device_access_sbus_dma_interval_get_f sbus_dma_interval_get;
    
    dnxf_data_device_access_sbus_dma_intr_enable_get_f sbus_dma_intr_enable_get;
    
    dnxf_data_device_access_mem_clear_chunk_size_get_f mem_clear_chunk_size_get;
} dnxf_data_if_device_access_t;






typedef struct
{
    
    char *block_type;
    
    int block_instance;
    
    int value;
} dnxf_data_device_blocks_override_t;



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
    *dnxf_data_device_blocks_nof_instances_otpc_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_brdc_fmach_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_brdc_fmacl_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_brdc_fmac_ac_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_brdc_fmac_bd_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_brdc_dch_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_blocks_nof_instances_brdc_dcml_get_f) (
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
    *dnxf_data_device_blocks_nof_qrh_mclbt_instances_get_f) (
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


typedef const dnxf_data_device_blocks_override_t *(
    *dnxf_data_device_blocks_override_get_f) (
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
    
    dnxf_data_device_blocks_nof_instances_occg_get_f nof_instances_occg_get;
    
    dnxf_data_device_blocks_nof_instances_eci_get_f nof_instances_eci_get;
    
    dnxf_data_device_blocks_nof_instances_cmic_get_f nof_instances_cmic_get;
    
    dnxf_data_device_blocks_nof_instances_mesh_topology_get_f nof_instances_mesh_topology_get;
    
    dnxf_data_device_blocks_nof_instances_otpc_get_f nof_instances_otpc_get;
    
    dnxf_data_device_blocks_nof_instances_brdc_fmach_get_f nof_instances_brdc_fmach_get;
    
    dnxf_data_device_blocks_nof_instances_brdc_fmacl_get_f nof_instances_brdc_fmacl_get;
    
    dnxf_data_device_blocks_nof_instances_brdc_fmac_ac_get_f nof_instances_brdc_fmac_ac_get;
    
    dnxf_data_device_blocks_nof_instances_brdc_fmac_bd_get_f nof_instances_brdc_fmac_bd_get;
    
    dnxf_data_device_blocks_nof_instances_brdc_dch_get_f nof_instances_brdc_dch_get;
    
    dnxf_data_device_blocks_nof_instances_brdc_dcml_get_f nof_instances_brdc_dcml_get;
    
    dnxf_data_device_blocks_nof_instances_lcm_get_f nof_instances_lcm_get;
    
    dnxf_data_device_blocks_nof_instances_mct_get_f nof_instances_mct_get;
    
    dnxf_data_device_blocks_nof_instances_qrh_get_f nof_instances_qrh_get;
    
    dnxf_data_device_blocks_nof_instances_dcml_get_f nof_instances_dcml_get;
    
    dnxf_data_device_blocks_nof_links_in_dcml_get_f nof_links_in_dcml_get;
    
    dnxf_data_device_blocks_nof_links_in_lcm_get_f nof_links_in_lcm_get;
    
    dnxf_data_device_blocks_nof_links_in_qrh_get_f nof_links_in_qrh_get;
    
    dnxf_data_device_blocks_nof_qrh_mclbt_instances_get_f nof_qrh_mclbt_instances_get;
    
    dnxf_data_device_blocks_nof_dtm_fifos_get_f nof_dtm_fifos_get;
    
    dnxf_data_device_blocks_nof_links_in_fsrd_get_f nof_links_in_fsrd_get;
    
    dnxf_data_device_blocks_nof_links_in_phy_core_get_f nof_links_in_phy_core_get;
    
    dnxf_data_device_blocks_nof_fmacs_in_fsrd_get_f nof_fmacs_in_fsrd_get;
    
    dnxf_data_device_blocks_override_get_f override_get;
    
    dnxc_data_table_info_get_f override_info_get;
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







typedef enum
{

    
    _dnxf_data_device_custom_features_feature_nof
} dnxf_data_device_custom_features_feature_e;



typedef int(
    *dnxf_data_device_custom_features_feature_get_f) (
    int unit,
    dnxf_data_device_custom_features_feature_e feature);


typedef uint32(
    *dnxf_data_device_custom_features_mesh_topology_fast_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_device_custom_features_feature_get_f feature_get;
    
    dnxf_data_device_custom_features_mesh_topology_fast_get_f mesh_topology_fast_get;
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

    
    _dnxf_data_device_ha_feature_nof
} dnxf_data_device_ha_feature_e;



typedef int(
    *dnxf_data_device_ha_feature_get_f) (
    int unit,
    dnxf_data_device_ha_feature_e feature);


typedef uint32(
    *dnxf_data_device_ha_warmboot_support_get_f) (
    int unit);


typedef uint32(
    *dnxf_data_device_ha_sw_state_max_size_get_f) (
    int unit);



typedef struct
{
    
    dnxf_data_device_ha_feature_get_f feature_get;
    
    dnxf_data_device_ha_warmboot_support_get_f warmboot_support_get;
    
    dnxf_data_device_ha_sw_state_max_size_get_f sw_state_max_size_get;
} dnxf_data_if_device_ha_t;







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
    
    dnxf_data_if_device_general_t general;
    
    dnxf_data_if_device_access_t access;
    
    dnxf_data_if_device_blocks_t blocks;
    
    dnxf_data_if_device_interrupts_t interrupts;
    
    dnxf_data_if_device_custom_features_t custom_features;
    
    dnxf_data_if_device_properties_t properties;
    
    dnxf_data_if_device_ha_t ha;
    
    dnxf_data_if_device_emulation_t emulation;
} dnxf_data_if_device_t;




extern dnxf_data_if_device_t dnxf_data_device;


#endif 

