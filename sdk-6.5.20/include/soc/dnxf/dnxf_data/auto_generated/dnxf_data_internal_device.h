

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTERNAL_DEVICE_H_

#define _DNXF_DATA_INTERNAL_DEVICE_H_

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnxf_data_device_submodule_general,
    dnxf_data_device_submodule_access,
    dnxf_data_device_submodule_blocks,
    dnxf_data_device_submodule_interrupts,
    dnxf_data_device_submodule_custom_features,
    dnxf_data_device_submodule_properties,
    dnxf_data_device_submodule_ha,
    dnxf_data_device_submodule_emulation,

    
    _dnxf_data_device_submodule_nof
} dnxf_data_device_submodule_e;








int dnxf_data_device_general_feature_get(
    int unit,
    dnxf_data_device_general_feature_e feature);



typedef enum
{
    dnxf_data_device_general_define_nof_pvt_monitors,
    dnxf_data_device_general_define_pvt_base,
    dnxf_data_device_general_define_pvt_factor,
    dnxf_data_device_general_define_nof_cores,
    dnxf_data_device_general_define_hard_reset_disable,
    dnxf_data_device_general_define_core_clock_khz,
    dnxf_data_device_general_define_system_ref_core_clock_khz,
    dnxf_data_device_general_define_device_id,

    
    _dnxf_data_device_general_define_nof
} dnxf_data_device_general_define_e;



uint32 dnxf_data_device_general_nof_pvt_monitors_get(
    int unit);


int dnxf_data_device_general_pvt_base_get(
    int unit);


int dnxf_data_device_general_pvt_factor_get(
    int unit);


uint32 dnxf_data_device_general_nof_cores_get(
    int unit);


uint32 dnxf_data_device_general_hard_reset_disable_get(
    int unit);


uint32 dnxf_data_device_general_core_clock_khz_get(
    int unit);


uint32 dnxf_data_device_general_system_ref_core_clock_khz_get(
    int unit);


uint32 dnxf_data_device_general_device_id_get(
    int unit);



typedef enum
{

    
    _dnxf_data_device_general_table_nof
} dnxf_data_device_general_table_e;









int dnxf_data_device_access_feature_get(
    int unit,
    dnxf_data_device_access_feature_e feature);



typedef enum
{
    dnxf_data_device_access_define_table_dma_enable,
    dnxf_data_device_access_define_tdma_timeout_usec,
    dnxf_data_device_access_define_tdma_intr_enable,
    dnxf_data_device_access_define_tslam_dma_enable,
    dnxf_data_device_access_define_tslam_timeout_usec,
    dnxf_data_device_access_define_tslam_int_enable,
    dnxf_data_device_access_define_mdio_int_divisor,
    dnxf_data_device_access_define_mdio_ext_divisor,
    dnxf_data_device_access_define_mdio_int_div_out_delay,
    dnxf_data_device_access_define_mdio_ext_div_out_delay,
    dnxf_data_device_access_define_bist_enable,
    dnxf_data_device_access_define_sbus_dma_interval,
    dnxf_data_device_access_define_sbus_dma_intr_enable,
    dnxf_data_device_access_define_mem_clear_chunk_size,

    
    _dnxf_data_device_access_define_nof
} dnxf_data_device_access_define_e;



uint32 dnxf_data_device_access_table_dma_enable_get(
    int unit);


uint32 dnxf_data_device_access_tdma_timeout_usec_get(
    int unit);


uint32 dnxf_data_device_access_tdma_intr_enable_get(
    int unit);


uint32 dnxf_data_device_access_tslam_dma_enable_get(
    int unit);


uint32 dnxf_data_device_access_tslam_timeout_usec_get(
    int unit);


uint32 dnxf_data_device_access_tslam_int_enable_get(
    int unit);


uint32 dnxf_data_device_access_mdio_int_divisor_get(
    int unit);


uint32 dnxf_data_device_access_mdio_ext_divisor_get(
    int unit);


uint32 dnxf_data_device_access_mdio_int_div_out_delay_get(
    int unit);


uint32 dnxf_data_device_access_mdio_ext_div_out_delay_get(
    int unit);


uint32 dnxf_data_device_access_bist_enable_get(
    int unit);


uint32 dnxf_data_device_access_sbus_dma_interval_get(
    int unit);


uint32 dnxf_data_device_access_sbus_dma_intr_enable_get(
    int unit);


uint32 dnxf_data_device_access_mem_clear_chunk_size_get(
    int unit);



typedef enum
{

    
    _dnxf_data_device_access_table_nof
} dnxf_data_device_access_table_e;









int dnxf_data_device_blocks_feature_get(
    int unit,
    dnxf_data_device_blocks_feature_e feature);



typedef enum
{
    dnxf_data_device_blocks_define_nof_all_blocks,
    dnxf_data_device_blocks_define_nof_instances_fmac,
    dnxf_data_device_blocks_define_nof_links_in_fmac,
    dnxf_data_device_blocks_define_nof_instances_fsrd,
    dnxf_data_device_blocks_define_nof_instances_brdc_fsrd,
    dnxf_data_device_blocks_define_nof_instances_dch,
    dnxf_data_device_blocks_define_nof_links_in_dch,
    dnxf_data_device_blocks_define_nof_dch_link_groups,
    dnxf_data_device_blocks_define_nof_instances_cch,
    dnxf_data_device_blocks_define_nof_links_in_cch,
    dnxf_data_device_blocks_define_nof_links_in_dcq,
    dnxf_data_device_blocks_define_nof_instances_rtp,
    dnxf_data_device_blocks_define_nof_instances_occg,
    dnxf_data_device_blocks_define_nof_instances_eci,
    dnxf_data_device_blocks_define_nof_instances_cmic,
    dnxf_data_device_blocks_define_nof_instances_mesh_topology,
    dnxf_data_device_blocks_define_nof_instances_otpc,
    dnxf_data_device_blocks_define_nof_instances_brdc_fmach,
    dnxf_data_device_blocks_define_nof_instances_brdc_fmacl,
    dnxf_data_device_blocks_define_nof_instances_brdc_fmac_ac,
    dnxf_data_device_blocks_define_nof_instances_brdc_fmac_bd,
    dnxf_data_device_blocks_define_nof_instances_brdc_dch,
    dnxf_data_device_blocks_define_nof_instances_brdc_dcml,
    dnxf_data_device_blocks_define_nof_instances_lcm,
    dnxf_data_device_blocks_define_nof_instances_mct,
    dnxf_data_device_blocks_define_nof_instances_qrh,
    dnxf_data_device_blocks_define_nof_instances_dcml,
    dnxf_data_device_blocks_define_nof_links_in_dcml,
    dnxf_data_device_blocks_define_nof_links_in_lcm,
    dnxf_data_device_blocks_define_nof_links_in_qrh,
    dnxf_data_device_blocks_define_nof_qrh_mclbt_instances,
    dnxf_data_device_blocks_define_nof_dtm_fifos,
    dnxf_data_device_blocks_define_nof_links_in_fsrd,
    dnxf_data_device_blocks_define_nof_links_in_phy_core,
    dnxf_data_device_blocks_define_nof_fmacs_in_fsrd,

    
    _dnxf_data_device_blocks_define_nof
} dnxf_data_device_blocks_define_e;



uint32 dnxf_data_device_blocks_nof_all_blocks_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_fmac_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_links_in_fmac_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_fsrd_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_brdc_fsrd_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_dch_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_links_in_dch_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_dch_link_groups_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_cch_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_links_in_cch_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_links_in_dcq_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_rtp_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_occg_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_eci_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_cmic_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_mesh_topology_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_otpc_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_brdc_fmach_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_brdc_fmacl_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_brdc_fmac_ac_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_brdc_fmac_bd_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_brdc_dch_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_brdc_dcml_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_lcm_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_mct_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_qrh_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_instances_dcml_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_links_in_dcml_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_links_in_lcm_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_links_in_qrh_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_qrh_mclbt_instances_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_dtm_fifos_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_links_in_fsrd_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_links_in_phy_core_get(
    int unit);


uint32 dnxf_data_device_blocks_nof_fmacs_in_fsrd_get(
    int unit);



typedef enum
{
    dnxf_data_device_blocks_table_override,

    
    _dnxf_data_device_blocks_table_nof
} dnxf_data_device_blocks_table_e;



const dnxf_data_device_blocks_override_t * dnxf_data_device_blocks_override_get(
    int unit,
    int index);



shr_error_e dnxf_data_device_blocks_override_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_device_blocks_override_info_get(
    int unit);






int dnxf_data_device_interrupts_feature_get(
    int unit,
    dnxf_data_device_interrupts_feature_e feature);



typedef enum
{
    dnxf_data_device_interrupts_define_nof_interrupts,

    
    _dnxf_data_device_interrupts_define_nof
} dnxf_data_device_interrupts_define_e;



uint32 dnxf_data_device_interrupts_nof_interrupts_get(
    int unit);



typedef enum
{

    
    _dnxf_data_device_interrupts_table_nof
} dnxf_data_device_interrupts_table_e;









int dnxf_data_device_custom_features_feature_get(
    int unit,
    dnxf_data_device_custom_features_feature_e feature);



typedef enum
{
    dnxf_data_device_custom_features_define_mesh_topology_fast,

    
    _dnxf_data_device_custom_features_define_nof
} dnxf_data_device_custom_features_define_e;



uint32 dnxf_data_device_custom_features_mesh_topology_fast_get(
    int unit);



typedef enum
{

    
    _dnxf_data_device_custom_features_table_nof
} dnxf_data_device_custom_features_table_e;









int dnxf_data_device_properties_feature_get(
    int unit,
    dnxf_data_device_properties_feature_e feature);



typedef enum
{

    
    _dnxf_data_device_properties_define_nof
} dnxf_data_device_properties_define_e;




typedef enum
{
    dnxf_data_device_properties_table_unsupported,

    
    _dnxf_data_device_properties_table_nof
} dnxf_data_device_properties_table_e;



const dnxf_data_device_properties_unsupported_t * dnxf_data_device_properties_unsupported_get(
    int unit,
    int index);



shr_error_e dnxf_data_device_properties_unsupported_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnxf_data_device_properties_unsupported_info_get(
    int unit);






int dnxf_data_device_ha_feature_get(
    int unit,
    dnxf_data_device_ha_feature_e feature);



typedef enum
{
    dnxf_data_device_ha_define_warmboot_support,
    dnxf_data_device_ha_define_sw_state_max_size,

    
    _dnxf_data_device_ha_define_nof
} dnxf_data_device_ha_define_e;



uint32 dnxf_data_device_ha_warmboot_support_get(
    int unit);


uint32 dnxf_data_device_ha_sw_state_max_size_get(
    int unit);



typedef enum
{

    
    _dnxf_data_device_ha_table_nof
} dnxf_data_device_ha_table_e;









int dnxf_data_device_emulation_feature_get(
    int unit,
    dnxf_data_device_emulation_feature_e feature);



typedef enum
{
    dnxf_data_device_emulation_define_emulation_system,

    
    _dnxf_data_device_emulation_define_nof
} dnxf_data_device_emulation_define_e;



uint32 dnxf_data_device_emulation_emulation_system_get(
    int unit);



typedef enum
{

    
    _dnxf_data_device_emulation_table_nof
} dnxf_data_device_emulation_table_e;






shr_error_e dnxf_data_device_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

