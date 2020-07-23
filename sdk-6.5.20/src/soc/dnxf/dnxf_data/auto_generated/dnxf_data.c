

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxc/dnxc_verify.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_dev_init.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_dev_init.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_intr.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_intr.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_linkscan.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_linkscan.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_module_testing.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal_module_testing.h>




dnxf_data_if_port_t dnxf_data_port;
dnxf_data_if_dev_init_t dnxf_data_dev_init;
dnxf_data_if_device_t dnxf_data_device;
dnxf_data_if_fabric_t dnxf_data_fabric;
dnxf_data_if_intr_t dnxf_data_intr;
dnxf_data_if_linkscan_t dnxf_data_linkscan;
dnxf_data_if_module_testing_t dnxf_data_module_testing;
static int dnxf_data_valid;



shr_error_e
dnxf_data_if_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (dnxf_data_valid != 0)
    {
        SHR_EXIT();
    }

    
    
    
    dnxf_data_port.general.feature_get = dnxf_data_port_general_feature_get;

    
    dnxf_data_port.general.nof_links_get = dnxf_data_port_general_nof_links_get;
    dnxf_data_port.general.nof_pms_get = dnxf_data_port_general_nof_pms_get;
    dnxf_data_port.general.max_bucket_fill_rate_get = dnxf_data_port_general_max_bucket_fill_rate_get;
    dnxf_data_port.general.link_mask_get = dnxf_data_port_general_link_mask_get;
    dnxf_data_port.general.aldwp_max_val_get = dnxf_data_port_general_aldwp_max_val_get;
    dnxf_data_port.general.fmac_clock_khz_get = dnxf_data_port_general_fmac_clock_khz_get;
    dnxf_data_port.general.speed_max_get = dnxf_data_port_general_speed_max_get;
    dnxf_data_port.general.fw_load_method_get = dnxf_data_port_general_fw_load_method_get;
    dnxf_data_port.general.fw_crc_check_get = dnxf_data_port_general_fw_crc_check_get;
    dnxf_data_port.general.fw_load_verify_get = dnxf_data_port_general_fw_load_verify_get;
    dnxf_data_port.general.mac_bucket_fill_rate_get = dnxf_data_port_general_mac_bucket_fill_rate_get;

    
    dnxf_data_port.general.supported_phys_get = dnxf_data_port_general_supported_phys_get;
    dnxf_data_port.general.supported_phys_info_get = dnxf_data_port_general_supported_phys_info_get;
    dnxf_data_port.general.fmac_bus_size_get = dnxf_data_port_general_fmac_bus_size_get;
    dnxf_data_port.general.fmac_bus_size_info_get = dnxf_data_port_general_fmac_bus_size_info_get;
    
    
    dnxf_data_port.stat.feature_get = dnxf_data_port_stat_feature_get;

    
    dnxf_data_port.stat.thread_interval_get = dnxf_data_port_stat_thread_interval_get;
    dnxf_data_port.stat.thread_counter_pri_get = dnxf_data_port_stat_thread_counter_pri_get;
    dnxf_data_port.stat.thread_timeout_get = dnxf_data_port_stat_thread_timeout_get;

    
    
    
    dnxf_data_port.static_add.feature_get = dnxf_data_port_static_add_feature_get;

    

    
    dnxf_data_port.static_add.port_info_get = dnxf_data_port_static_add_port_info_get;
    dnxf_data_port.static_add.port_info_info_get = dnxf_data_port_static_add_port_info_info_get;
    dnxf_data_port.static_add.serdes_lane_config_get = dnxf_data_port_static_add_serdes_lane_config_get;
    dnxf_data_port.static_add.serdes_lane_config_info_get = dnxf_data_port_static_add_serdes_lane_config_info_get;
    dnxf_data_port.static_add.serdes_tx_taps_get = dnxf_data_port_static_add_serdes_tx_taps_get;
    dnxf_data_port.static_add.serdes_tx_taps_info_get = dnxf_data_port_static_add_serdes_tx_taps_info_get;
    dnxf_data_port.static_add.quad_info_get = dnxf_data_port_static_add_quad_info_get;
    dnxf_data_port.static_add.quad_info_info_get = dnxf_data_port_static_add_quad_info_info_get;
    
    
    dnxf_data_port.lane_map.feature_get = dnxf_data_port_lane_map_feature_get;

    
    dnxf_data_port.lane_map.lane_map_bound_get = dnxf_data_port_lane_map_lane_map_bound_get;

    
    dnxf_data_port.lane_map.info_get = dnxf_data_port_lane_map_info_get;
    dnxf_data_port.lane_map.info_info_get = dnxf_data_port_lane_map_info_info_get;
    
    
    dnxf_data_port.pll.feature_get = dnxf_data_port_pll_feature_get;

    
    dnxf_data_port.pll.nof_lcpll_get = dnxf_data_port_pll_nof_lcpll_get;
    dnxf_data_port.pll.nof_port_in_lcpll_get = dnxf_data_port_pll_nof_port_in_lcpll_get;
    dnxf_data_port.pll.fabric_clock_freq_in_default_get = dnxf_data_port_pll_fabric_clock_freq_in_default_get;
    dnxf_data_port.pll.fabric_clock_freq_out_default_get = dnxf_data_port_pll_fabric_clock_freq_out_default_get;
    dnxf_data_port.pll.nof_plls_per_pm_get = dnxf_data_port_pll_nof_plls_per_pm_get;
    dnxf_data_port.pll.nof_config_words_get = dnxf_data_port_pll_nof_config_words_get;

    
    dnxf_data_port.pll.info_get = dnxf_data_port_pll_info_get;
    dnxf_data_port.pll.info_info_get = dnxf_data_port_pll_info_info_get;
    dnxf_data_port.pll.config_get = dnxf_data_port_pll_config_get;
    dnxf_data_port.pll.config_info_get = dnxf_data_port_pll_config_info_get;
    
    
    dnxf_data_port.synce.feature_get = dnxf_data_port_synce_feature_get;

    

    
    dnxf_data_port.synce.cfg_get = dnxf_data_port_synce_cfg_get;
    dnxf_data_port.synce.cfg_info_get = dnxf_data_port_synce_cfg_info_get;
    
    
    dnxf_data_port.retimer.feature_get = dnxf_data_port_retimer_feature_get;

    

    
    dnxf_data_port.retimer.links_connection_get = dnxf_data_port_retimer_links_connection_get;
    dnxf_data_port.retimer.links_connection_info_get = dnxf_data_port_retimer_links_connection_info_get;
    
    
    dnxf_data_port.features.feature_get = dnxf_data_port_features_feature_get;

    

    

    
    
    
    dnxf_data_dev_init.time.feature_get = dnxf_data_dev_init_time_feature_get;

    
    dnxf_data_dev_init.time.init_total_thresh_get = dnxf_data_dev_init_time_init_total_thresh_get;
    dnxf_data_dev_init.time.appl_init_total_thresh_get = dnxf_data_dev_init_time_appl_init_total_thresh_get;

    
    dnxf_data_dev_init.time.step_thresh_get = dnxf_data_dev_init_time_step_thresh_get;
    dnxf_data_dev_init.time.step_thresh_info_get = dnxf_data_dev_init_time_step_thresh_info_get;
    dnxf_data_dev_init.time.appl_step_thresh_get = dnxf_data_dev_init_time_appl_step_thresh_get;
    dnxf_data_dev_init.time.appl_step_thresh_info_get = dnxf_data_dev_init_time_appl_step_thresh_info_get;
    
    
    dnxf_data_dev_init.shadow.feature_get = dnxf_data_dev_init_shadow_feature_get;

    
    dnxf_data_dev_init.shadow.cache_enable_all_get = dnxf_data_dev_init_shadow_cache_enable_all_get;
    dnxf_data_dev_init.shadow.cache_enable_ecc_get = dnxf_data_dev_init_shadow_cache_enable_ecc_get;
    dnxf_data_dev_init.shadow.cache_enable_parity_get = dnxf_data_dev_init_shadow_cache_enable_parity_get;
    dnxf_data_dev_init.shadow.cache_enable_specific_get = dnxf_data_dev_init_shadow_cache_enable_specific_get;
    dnxf_data_dev_init.shadow.cache_disable_specific_get = dnxf_data_dev_init_shadow_cache_disable_specific_get;

    
    dnxf_data_dev_init.shadow.uncacheable_mem_get = dnxf_data_dev_init_shadow_uncacheable_mem_get;
    dnxf_data_dev_init.shadow.uncacheable_mem_info_get = dnxf_data_dev_init_shadow_uncacheable_mem_info_get;
    
    
    dnxf_data_dev_init.ha.feature_get = dnxf_data_dev_init_ha_feature_get;

    
    dnxf_data_dev_init.ha.warmboot_support_get = dnxf_data_dev_init_ha_warmboot_support_get;
    dnxf_data_dev_init.ha.sw_state_max_size_get = dnxf_data_dev_init_ha_sw_state_max_size_get;
    dnxf_data_dev_init.ha.stable_location_get = dnxf_data_dev_init_ha_stable_location_get;
    dnxf_data_dev_init.ha.stable_size_get = dnxf_data_dev_init_ha_stable_size_get;

    
    dnxf_data_dev_init.ha.stable_filename_get = dnxf_data_dev_init_ha_stable_filename_get;
    dnxf_data_dev_init.ha.stable_filename_info_get = dnxf_data_dev_init_ha_stable_filename_info_get;

    
    
    
    dnxf_data_device.general.feature_get = dnxf_data_device_general_feature_get;

    
    dnxf_data_device.general.nof_pvt_monitors_get = dnxf_data_device_general_nof_pvt_monitors_get;
    dnxf_data_device.general.pvt_base_get = dnxf_data_device_general_pvt_base_get;
    dnxf_data_device.general.pvt_factor_get = dnxf_data_device_general_pvt_factor_get;
    dnxf_data_device.general.nof_cores_get = dnxf_data_device_general_nof_cores_get;
    dnxf_data_device.general.hard_reset_disable_get = dnxf_data_device_general_hard_reset_disable_get;
    dnxf_data_device.general.core_clock_khz_get = dnxf_data_device_general_core_clock_khz_get;
    dnxf_data_device.general.system_ref_core_clock_khz_get = dnxf_data_device_general_system_ref_core_clock_khz_get;
    dnxf_data_device.general.device_id_get = dnxf_data_device_general_device_id_get;

    
    
    
    dnxf_data_device.access.feature_get = dnxf_data_device_access_feature_get;

    
    dnxf_data_device.access.table_dma_enable_get = dnxf_data_device_access_table_dma_enable_get;
    dnxf_data_device.access.tdma_timeout_usec_get = dnxf_data_device_access_tdma_timeout_usec_get;
    dnxf_data_device.access.tdma_intr_enable_get = dnxf_data_device_access_tdma_intr_enable_get;
    dnxf_data_device.access.tslam_dma_enable_get = dnxf_data_device_access_tslam_dma_enable_get;
    dnxf_data_device.access.tslam_timeout_usec_get = dnxf_data_device_access_tslam_timeout_usec_get;
    dnxf_data_device.access.tslam_int_enable_get = dnxf_data_device_access_tslam_int_enable_get;
    dnxf_data_device.access.mdio_int_divisor_get = dnxf_data_device_access_mdio_int_divisor_get;
    dnxf_data_device.access.mdio_ext_divisor_get = dnxf_data_device_access_mdio_ext_divisor_get;
    dnxf_data_device.access.mdio_int_div_out_delay_get = dnxf_data_device_access_mdio_int_div_out_delay_get;
    dnxf_data_device.access.mdio_ext_div_out_delay_get = dnxf_data_device_access_mdio_ext_div_out_delay_get;
    dnxf_data_device.access.bist_enable_get = dnxf_data_device_access_bist_enable_get;
    dnxf_data_device.access.sbus_dma_interval_get = dnxf_data_device_access_sbus_dma_interval_get;
    dnxf_data_device.access.sbus_dma_intr_enable_get = dnxf_data_device_access_sbus_dma_intr_enable_get;
    dnxf_data_device.access.mem_clear_chunk_size_get = dnxf_data_device_access_mem_clear_chunk_size_get;

    
    
    
    dnxf_data_device.blocks.feature_get = dnxf_data_device_blocks_feature_get;

    
    dnxf_data_device.blocks.nof_all_blocks_get = dnxf_data_device_blocks_nof_all_blocks_get;
    dnxf_data_device.blocks.nof_instances_fmac_get = dnxf_data_device_blocks_nof_instances_fmac_get;
    dnxf_data_device.blocks.nof_links_in_fmac_get = dnxf_data_device_blocks_nof_links_in_fmac_get;
    dnxf_data_device.blocks.nof_instances_fsrd_get = dnxf_data_device_blocks_nof_instances_fsrd_get;
    dnxf_data_device.blocks.nof_instances_brdc_fsrd_get = dnxf_data_device_blocks_nof_instances_brdc_fsrd_get;
    dnxf_data_device.blocks.nof_instances_dch_get = dnxf_data_device_blocks_nof_instances_dch_get;
    dnxf_data_device.blocks.nof_links_in_dch_get = dnxf_data_device_blocks_nof_links_in_dch_get;
    dnxf_data_device.blocks.nof_dch_link_groups_get = dnxf_data_device_blocks_nof_dch_link_groups_get;
    dnxf_data_device.blocks.nof_instances_cch_get = dnxf_data_device_blocks_nof_instances_cch_get;
    dnxf_data_device.blocks.nof_links_in_cch_get = dnxf_data_device_blocks_nof_links_in_cch_get;
    dnxf_data_device.blocks.nof_links_in_dcq_get = dnxf_data_device_blocks_nof_links_in_dcq_get;
    dnxf_data_device.blocks.nof_instances_rtp_get = dnxf_data_device_blocks_nof_instances_rtp_get;
    dnxf_data_device.blocks.nof_instances_occg_get = dnxf_data_device_blocks_nof_instances_occg_get;
    dnxf_data_device.blocks.nof_instances_eci_get = dnxf_data_device_blocks_nof_instances_eci_get;
    dnxf_data_device.blocks.nof_instances_cmic_get = dnxf_data_device_blocks_nof_instances_cmic_get;
    dnxf_data_device.blocks.nof_instances_mesh_topology_get = dnxf_data_device_blocks_nof_instances_mesh_topology_get;
    dnxf_data_device.blocks.nof_instances_otpc_get = dnxf_data_device_blocks_nof_instances_otpc_get;
    dnxf_data_device.blocks.nof_instances_brdc_fmach_get = dnxf_data_device_blocks_nof_instances_brdc_fmach_get;
    dnxf_data_device.blocks.nof_instances_brdc_fmacl_get = dnxf_data_device_blocks_nof_instances_brdc_fmacl_get;
    dnxf_data_device.blocks.nof_instances_brdc_fmac_ac_get = dnxf_data_device_blocks_nof_instances_brdc_fmac_ac_get;
    dnxf_data_device.blocks.nof_instances_brdc_fmac_bd_get = dnxf_data_device_blocks_nof_instances_brdc_fmac_bd_get;
    dnxf_data_device.blocks.nof_instances_brdc_dch_get = dnxf_data_device_blocks_nof_instances_brdc_dch_get;
    dnxf_data_device.blocks.nof_instances_brdc_dcml_get = dnxf_data_device_blocks_nof_instances_brdc_dcml_get;
    dnxf_data_device.blocks.nof_instances_lcm_get = dnxf_data_device_blocks_nof_instances_lcm_get;
    dnxf_data_device.blocks.nof_instances_mct_get = dnxf_data_device_blocks_nof_instances_mct_get;
    dnxf_data_device.blocks.nof_instances_qrh_get = dnxf_data_device_blocks_nof_instances_qrh_get;
    dnxf_data_device.blocks.nof_instances_dcml_get = dnxf_data_device_blocks_nof_instances_dcml_get;
    dnxf_data_device.blocks.nof_links_in_dcml_get = dnxf_data_device_blocks_nof_links_in_dcml_get;
    dnxf_data_device.blocks.nof_links_in_lcm_get = dnxf_data_device_blocks_nof_links_in_lcm_get;
    dnxf_data_device.blocks.nof_links_in_qrh_get = dnxf_data_device_blocks_nof_links_in_qrh_get;
    dnxf_data_device.blocks.nof_qrh_mclbt_instances_get = dnxf_data_device_blocks_nof_qrh_mclbt_instances_get;
    dnxf_data_device.blocks.nof_dtm_fifos_get = dnxf_data_device_blocks_nof_dtm_fifos_get;
    dnxf_data_device.blocks.nof_links_in_fsrd_get = dnxf_data_device_blocks_nof_links_in_fsrd_get;
    dnxf_data_device.blocks.nof_links_in_phy_core_get = dnxf_data_device_blocks_nof_links_in_phy_core_get;
    dnxf_data_device.blocks.nof_fmacs_in_fsrd_get = dnxf_data_device_blocks_nof_fmacs_in_fsrd_get;

    
    dnxf_data_device.blocks.override_get = dnxf_data_device_blocks_override_get;
    dnxf_data_device.blocks.override_info_get = dnxf_data_device_blocks_override_info_get;
    
    
    dnxf_data_device.interrupts.feature_get = dnxf_data_device_interrupts_feature_get;

    
    dnxf_data_device.interrupts.nof_interrupts_get = dnxf_data_device_interrupts_nof_interrupts_get;

    
    
    
    dnxf_data_device.custom_features.feature_get = dnxf_data_device_custom_features_feature_get;

    
    dnxf_data_device.custom_features.mesh_topology_fast_get = dnxf_data_device_custom_features_mesh_topology_fast_get;

    
    
    
    dnxf_data_device.properties.feature_get = dnxf_data_device_properties_feature_get;

    

    
    dnxf_data_device.properties.unsupported_get = dnxf_data_device_properties_unsupported_get;
    dnxf_data_device.properties.unsupported_info_get = dnxf_data_device_properties_unsupported_info_get;
    
    
    dnxf_data_device.ha.feature_get = dnxf_data_device_ha_feature_get;

    
    dnxf_data_device.ha.warmboot_support_get = dnxf_data_device_ha_warmboot_support_get;
    dnxf_data_device.ha.sw_state_max_size_get = dnxf_data_device_ha_sw_state_max_size_get;

    
    
    
    dnxf_data_device.emulation.feature_get = dnxf_data_device_emulation_feature_get;

    
    dnxf_data_device.emulation.emulation_system_get = dnxf_data_device_emulation_emulation_system_get;

    

    
    
    
    dnxf_data_fabric.general.feature_get = dnxf_data_fabric_general_feature_get;

    
    dnxf_data_fabric.general.max_nof_pipes_get = dnxf_data_fabric_general_max_nof_pipes_get;
    dnxf_data_fabric.general.device_mode_get = dnxf_data_fabric_general_device_mode_get;
    dnxf_data_fabric.general.local_routing_enable_uc_get = dnxf_data_fabric_general_local_routing_enable_uc_get;
    dnxf_data_fabric.general.local_routing_enable_mc_get = dnxf_data_fabric_general_local_routing_enable_mc_get;

    
    
    
    dnxf_data_fabric.pipes.feature_get = dnxf_data_fabric_pipes_feature_get;

    
    dnxf_data_fabric.pipes.max_nof_pipes_get = dnxf_data_fabric_pipes_max_nof_pipes_get;
    dnxf_data_fabric.pipes.nof_valid_mapping_options_get = dnxf_data_fabric_pipes_nof_valid_mapping_options_get;
    dnxf_data_fabric.pipes.nof_pipes_get = dnxf_data_fabric_pipes_nof_pipes_get;
    dnxf_data_fabric.pipes.system_contains_multiple_pipe_device_get = dnxf_data_fabric_pipes_system_contains_multiple_pipe_device_get;
    dnxf_data_fabric.pipes.system_tdm_priority_get = dnxf_data_fabric_pipes_system_tdm_priority_get;

    
    dnxf_data_fabric.pipes.map_get = dnxf_data_fabric_pipes_map_get;
    dnxf_data_fabric.pipes.map_info_get = dnxf_data_fabric_pipes_map_info_get;
    dnxf_data_fabric.pipes.valid_map_config_get = dnxf_data_fabric_pipes_valid_map_config_get;
    dnxf_data_fabric.pipes.valid_map_config_info_get = dnxf_data_fabric_pipes_valid_map_config_info_get;
    
    
    dnxf_data_fabric.multicast.feature_get = dnxf_data_fabric_multicast_feature_get;

    
    dnxf_data_fabric.multicast.table_row_size_in_uint32_get = dnxf_data_fabric_multicast_table_row_size_in_uint32_get;
    dnxf_data_fabric.multicast.id_range_get = dnxf_data_fabric_multicast_id_range_get;
    dnxf_data_fabric.multicast.mode_get = dnxf_data_fabric_multicast_mode_get;
    dnxf_data_fabric.multicast.priority_map_enable_get = dnxf_data_fabric_multicast_priority_map_enable_get;

    
    
    
    dnxf_data_fabric.fifos.feature_get = dnxf_data_fabric_fifos_feature_get;

    
    dnxf_data_fabric.fifos.granularity_get = dnxf_data_fabric_fifos_granularity_get;
    dnxf_data_fabric.fifos.min_depth_get = dnxf_data_fabric_fifos_min_depth_get;
    dnxf_data_fabric.fifos.max_unused_resources_get = dnxf_data_fabric_fifos_max_unused_resources_get;
    dnxf_data_fabric.fifos.rx_resources_get = dnxf_data_fabric_fifos_rx_resources_get;
    dnxf_data_fabric.fifos.mid_resources_get = dnxf_data_fabric_fifos_mid_resources_get;
    dnxf_data_fabric.fifos.tx_resources_get = dnxf_data_fabric_fifos_tx_resources_get;
    dnxf_data_fabric.fifos.rx_full_offset_get = dnxf_data_fabric_fifos_rx_full_offset_get;
    dnxf_data_fabric.fifos.mid_full_offset_get = dnxf_data_fabric_fifos_mid_full_offset_get;
    dnxf_data_fabric.fifos.tx_full_offset_get = dnxf_data_fabric_fifos_tx_full_offset_get;

    
    dnxf_data_fabric.fifos.rx_depth_per_pipe_get = dnxf_data_fabric_fifos_rx_depth_per_pipe_get;
    dnxf_data_fabric.fifos.rx_depth_per_pipe_info_get = dnxf_data_fabric_fifos_rx_depth_per_pipe_info_get;
    dnxf_data_fabric.fifos.mid_depth_per_pipe_get = dnxf_data_fabric_fifos_mid_depth_per_pipe_get;
    dnxf_data_fabric.fifos.mid_depth_per_pipe_info_get = dnxf_data_fabric_fifos_mid_depth_per_pipe_info_get;
    dnxf_data_fabric.fifos.tx_depth_per_pipe_get = dnxf_data_fabric_fifos_tx_depth_per_pipe_get;
    dnxf_data_fabric.fifos.tx_depth_per_pipe_info_get = dnxf_data_fabric_fifos_tx_depth_per_pipe_info_get;
    
    
    dnxf_data_fabric.congestion.feature_get = dnxf_data_fabric_congestion_feature_get;

    
    dnxf_data_fabric.congestion.nof_profiles_get = dnxf_data_fabric_congestion_nof_profiles_get;
    dnxf_data_fabric.congestion.nof_threshold_priorities_get = dnxf_data_fabric_congestion_nof_threshold_priorities_get;
    dnxf_data_fabric.congestion.nof_threshold_levels_get = dnxf_data_fabric_congestion_nof_threshold_levels_get;
    dnxf_data_fabric.congestion.nof_threshold_index_dimensions_get = dnxf_data_fabric_congestion_nof_threshold_index_dimensions_get;
    dnxf_data_fabric.congestion.nof_thresholds_get = dnxf_data_fabric_congestion_nof_thresholds_get;
    dnxf_data_fabric.congestion.nof_dfl_banks_get = dnxf_data_fabric_congestion_nof_dfl_banks_get;
    dnxf_data_fabric.congestion.nof_dfl_sub_banks_get = dnxf_data_fabric_congestion_nof_dfl_sub_banks_get;
    dnxf_data_fabric.congestion.nof_dfl_bank_entries_get = dnxf_data_fabric_congestion_nof_dfl_bank_entries_get;
    dnxf_data_fabric.congestion.nof_rci_bits_get = dnxf_data_fabric_congestion_nof_rci_bits_get;

    
    dnxf_data_fabric.congestion.thresholds_info_get = dnxf_data_fabric_congestion_thresholds_info_get;
    dnxf_data_fabric.congestion.thresholds_info_info_get = dnxf_data_fabric_congestion_thresholds_info_info_get;
    
    
    dnxf_data_fabric.cell.feature_get = dnxf_data_fabric_cell_feature_get;

    
    dnxf_data_fabric.cell.fifo_dma_fabric_cell_nof_entries_per_cell_get = dnxf_data_fabric_cell_fifo_dma_fabric_cell_nof_entries_per_cell_get;
    dnxf_data_fabric.cell.rx_cpu_cell_max_payload_size_get = dnxf_data_fabric_cell_rx_cpu_cell_max_payload_size_get;
    dnxf_data_fabric.cell.rx_max_nof_cpu_buffers_get = dnxf_data_fabric_cell_rx_max_nof_cpu_buffers_get;
    dnxf_data_fabric.cell.source_routed_cells_header_offset_get = dnxf_data_fabric_cell_source_routed_cells_header_offset_get;
    dnxf_data_fabric.cell.fifo_dma_nof_channels_get = dnxf_data_fabric_cell_fifo_dma_nof_channels_get;
    dnxf_data_fabric.cell.fifo_dma_entry_size_get = dnxf_data_fabric_cell_fifo_dma_entry_size_get;
    dnxf_data_fabric.cell.fifo_dma_max_nof_entries_get = dnxf_data_fabric_cell_fifo_dma_max_nof_entries_get;
    dnxf_data_fabric.cell.fifo_dma_min_nof_entries_get = dnxf_data_fabric_cell_fifo_dma_min_nof_entries_get;
    dnxf_data_fabric.cell.fifo_dma_enable_get = dnxf_data_fabric_cell_fifo_dma_enable_get;
    dnxf_data_fabric.cell.fifo_dma_buffer_size_get = dnxf_data_fabric_cell_fifo_dma_buffer_size_get;
    dnxf_data_fabric.cell.fifo_dma_threshold_get = dnxf_data_fabric_cell_fifo_dma_threshold_get;
    dnxf_data_fabric.cell.fifo_dma_timeout_get = dnxf_data_fabric_cell_fifo_dma_timeout_get;
    dnxf_data_fabric.cell.rx_thread_pri_get = dnxf_data_fabric_cell_rx_thread_pri_get;

    
    
    
    dnxf_data_fabric.topology.feature_get = dnxf_data_fabric_topology_feature_get;

    
    dnxf_data_fabric.topology.max_link_score_get = dnxf_data_fabric_topology_max_link_score_get;
    dnxf_data_fabric.topology.nof_local_modid_get = dnxf_data_fabric_topology_nof_local_modid_get;
    dnxf_data_fabric.topology.nof_local_modid_fe13_get = dnxf_data_fabric_topology_nof_local_modid_fe13_get;
    dnxf_data_fabric.topology.load_balancing_mode_get = dnxf_data_fabric_topology_load_balancing_mode_get;

    
    
    
    dnxf_data_fabric.gpd.feature_get = dnxf_data_fabric_gpd_feature_get;

    
    dnxf_data_fabric.gpd.in_time_get = dnxf_data_fabric_gpd_in_time_get;
    dnxf_data_fabric.gpd.out_time_get = dnxf_data_fabric_gpd_out_time_get;

    
    
    
    dnxf_data_fabric.reachability.feature_get = dnxf_data_fabric_reachability_feature_get;

    
    dnxf_data_fabric.reachability.table_row_size_in_uint32_get = dnxf_data_fabric_reachability_table_row_size_in_uint32_get;
    dnxf_data_fabric.reachability.table_group_size_get = dnxf_data_fabric_reachability_table_group_size_get;
    dnxf_data_fabric.reachability.rmgr_units_get = dnxf_data_fabric_reachability_rmgr_units_get;
    dnxf_data_fabric.reachability.rmgr_nof_links_get = dnxf_data_fabric_reachability_rmgr_nof_links_get;
    dnxf_data_fabric.reachability.gpd_rmgr_time_factor_get = dnxf_data_fabric_reachability_gpd_rmgr_time_factor_get;
    dnxf_data_fabric.reachability.gen_rate_link_delta_get = dnxf_data_fabric_reachability_gen_rate_link_delta_get;
    dnxf_data_fabric.reachability.gen_rate_full_cycle_get = dnxf_data_fabric_reachability_gen_rate_full_cycle_get;
    dnxf_data_fabric.reachability.gpd_gen_rate_full_cycle_get = dnxf_data_fabric_reachability_gpd_gen_rate_full_cycle_get;
    dnxf_data_fabric.reachability.watchdog_rate_get = dnxf_data_fabric_reachability_watchdog_rate_get;

    
    
    
    dnxf_data_fabric.hw_snake.feature_get = dnxf_data_fabric_hw_snake_feature_get;

    

    

    
    
    
    dnxf_data_intr.general.feature_get = dnxf_data_intr_general_feature_get;

    
    dnxf_data_intr.general.nof_interrupts_get = dnxf_data_intr_general_nof_interrupts_get;
    dnxf_data_intr.general.polled_irq_mode_get = dnxf_data_intr_general_polled_irq_mode_get;

    
    dnxf_data_intr.general.active_on_init_get = dnxf_data_intr_general_active_on_init_get;
    dnxf_data_intr.general.active_on_init_info_get = dnxf_data_intr_general_active_on_init_info_get;
    dnxf_data_intr.general.disable_on_init_get = dnxf_data_intr_general_disable_on_init_get;
    dnxf_data_intr.general.disable_on_init_info_get = dnxf_data_intr_general_disable_on_init_info_get;
    dnxf_data_intr.general.disable_print_on_init_get = dnxf_data_intr_general_disable_print_on_init_get;
    dnxf_data_intr.general.disable_print_on_init_info_get = dnxf_data_intr_general_disable_print_on_init_info_get;
    
    
    dnxf_data_intr.ser.feature_get = dnxf_data_intr_ser_feature_get;

    

    
    dnxf_data_intr.ser.mem_mask_get = dnxf_data_intr_ser_mem_mask_get;
    dnxf_data_intr.ser.mem_mask_info_get = dnxf_data_intr_ser_mem_mask_info_get;
    dnxf_data_intr.ser.xor_mem_get = dnxf_data_intr_ser_xor_mem_get;
    dnxf_data_intr.ser.xor_mem_info_get = dnxf_data_intr_ser_xor_mem_info_get;

    
    
    
    dnxf_data_linkscan.general.feature_get = dnxf_data_linkscan_general_feature_get;

    
    dnxf_data_linkscan.general.interval_get = dnxf_data_linkscan_general_interval_get;
    dnxf_data_linkscan.general.error_delay_get = dnxf_data_linkscan_general_error_delay_get;
    dnxf_data_linkscan.general.max_error_get = dnxf_data_linkscan_general_max_error_get;
    dnxf_data_linkscan.general.thread_priority_get = dnxf_data_linkscan_general_thread_priority_get;
    dnxf_data_linkscan.general.m0_pause_enable_get = dnxf_data_linkscan_general_m0_pause_enable_get;

    
    dnxf_data_linkscan.general.pbmp_get = dnxf_data_linkscan_general_pbmp_get;
    dnxf_data_linkscan.general.pbmp_info_get = dnxf_data_linkscan_general_pbmp_info_get;

    
    
    
    dnxf_data_module_testing.unsupported.feature_get = dnxf_data_module_testing_unsupported_feature_get;

    
    dnxf_data_module_testing.unsupported.supported_def_get = dnxf_data_module_testing_unsupported_supported_def_get;
    dnxf_data_module_testing.unsupported.unsupported_def_get = dnxf_data_module_testing_unsupported_unsupported_def_get;

    
    dnxf_data_module_testing.unsupported.supported_table_get = dnxf_data_module_testing_unsupported_supported_table_get;
    dnxf_data_module_testing.unsupported.supported_table_info_get = dnxf_data_module_testing_unsupported_supported_table_info_get;
    dnxf_data_module_testing.unsupported.unsupported_table_get = dnxf_data_module_testing_unsupported_unsupported_table_get;
    dnxf_data_module_testing.unsupported.unsupported_table_info_get = dnxf_data_module_testing_unsupported_unsupported_table_info_get;
    
    
    dnxf_data_module_testing.types.feature_get = dnxf_data_module_testing_types_feature_get;

    

    
    dnxf_data_module_testing.types.all_get = dnxf_data_module_testing_types_all_get;
    dnxf_data_module_testing.types.all_info_get = dnxf_data_module_testing_types_all_info_get;
    
    
    dnxf_data_module_testing.property_methods.feature_get = dnxf_data_module_testing_property_methods_feature_get;

    
    dnxf_data_module_testing.property_methods.numeric_range_get = dnxf_data_module_testing_property_methods_numeric_range_get;

    
    dnxf_data_module_testing.property_methods.enable_get = dnxf_data_module_testing_property_methods_enable_get;
    dnxf_data_module_testing.property_methods.enable_info_get = dnxf_data_module_testing_property_methods_enable_info_get;
    dnxf_data_module_testing.property_methods.port_enable_get = dnxf_data_module_testing_property_methods_port_enable_get;
    dnxf_data_module_testing.property_methods.port_enable_info_get = dnxf_data_module_testing_property_methods_port_enable_info_get;
    dnxf_data_module_testing.property_methods.suffix_enable_get = dnxf_data_module_testing_property_methods_suffix_enable_get;
    dnxf_data_module_testing.property_methods.suffix_enable_info_get = dnxf_data_module_testing_property_methods_suffix_enable_info_get;
    dnxf_data_module_testing.property_methods.range_get = dnxf_data_module_testing_property_methods_range_get;
    dnxf_data_module_testing.property_methods.range_info_get = dnxf_data_module_testing_property_methods_range_info_get;
    dnxf_data_module_testing.property_methods.port_range_get = dnxf_data_module_testing_property_methods_port_range_get;
    dnxf_data_module_testing.property_methods.port_range_info_get = dnxf_data_module_testing_property_methods_port_range_info_get;
    dnxf_data_module_testing.property_methods.suffix_range_get = dnxf_data_module_testing_property_methods_suffix_range_get;
    dnxf_data_module_testing.property_methods.suffix_range_info_get = dnxf_data_module_testing_property_methods_suffix_range_info_get;
    dnxf_data_module_testing.property_methods.direct_map_get = dnxf_data_module_testing_property_methods_direct_map_get;
    dnxf_data_module_testing.property_methods.direct_map_info_get = dnxf_data_module_testing_property_methods_direct_map_info_get;
    dnxf_data_module_testing.property_methods.port_direct_map_get = dnxf_data_module_testing_property_methods_port_direct_map_get;
    dnxf_data_module_testing.property_methods.port_direct_map_info_get = dnxf_data_module_testing_property_methods_port_direct_map_info_get;
    dnxf_data_module_testing.property_methods.suffix_direct_map_get = dnxf_data_module_testing_property_methods_suffix_direct_map_get;
    dnxf_data_module_testing.property_methods.suffix_direct_map_info_get = dnxf_data_module_testing_property_methods_suffix_direct_map_info_get;
    dnxf_data_module_testing.property_methods.custom_get = dnxf_data_module_testing_property_methods_custom_get;
    dnxf_data_module_testing.property_methods.custom_info_get = dnxf_data_module_testing_property_methods_custom_info_get;

    
    dnxf_data_valid = 1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_if_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    
    

    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    _dnxc_data[unit].name = "dnxf_data";
    _dnxc_data[unit].nof_modules = _dnxf_data_module_nof;
    DNXC_DATA_ALLOC(_dnxc_data[unit].modules, dnxc_data_module_t,  _dnxc_data[unit].nof_modules, "_dnxc_data modules");

    
    SHR_IF_ERR_EXIT(dnxf_data_if_init(unit));
    
    SHR_IF_ERR_EXIT(dnxf_data_port_init(unit, &_dnxc_data[unit].modules[dnxf_data_module_port]));
    SHR_IF_ERR_EXIT(dnxf_data_dev_init_init(unit, &_dnxc_data[unit].modules[dnxf_data_module_dev_init]));
    SHR_IF_ERR_EXIT(dnxf_data_device_init(unit, &_dnxc_data[unit].modules[dnxf_data_module_device]));
    SHR_IF_ERR_EXIT(dnxf_data_fabric_init(unit, &_dnxc_data[unit].modules[dnxf_data_module_fabric]));
    SHR_IF_ERR_EXIT(dnxf_data_intr_init(unit, &_dnxc_data[unit].modules[dnxf_data_module_intr]));
    SHR_IF_ERR_EXIT(dnxf_data_linkscan_init(unit, &_dnxc_data[unit].modules[dnxf_data_module_linkscan]));
    SHR_IF_ERR_EXIT(dnxf_data_module_testing_init(unit, &_dnxc_data[unit].modules[dnxf_data_module_module_testing]));

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_parse_init(unit));

    
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_set(unit, DNXC_DATA_STATE_F_INIT_DONE));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_values_set(unit));

    
    DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(dnxf_data_property_unsupported_verify(unit)));

    
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_set(unit, DNXC_DATA_STATE_F_DEV_DATA_DONE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_data_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_CONT(dnxf_data_if_deinit(unit));

    
    SHR_IF_ERR_CONT(dnxc_data_mgmt_deinit(unit, &_dnxc_data[unit]));

    SHR_FUNC_EXIT;
}


#undef BSL_LOG_MODULE

