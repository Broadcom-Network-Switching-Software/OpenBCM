

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_PROPERTY_H_

#define _DNX_DATA_PROPERTY_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif




shr_error_e dnx_data_property_port_static_add_ucode_port_read(
    int unit,
    int port,
    dnx_data_port_static_add_ucode_port_t *ucode_port);


shr_error_e dnx_data_property_port_static_add_ext_stat_speed_read(
    int unit,
    int ext_stat_port,
    dnx_data_port_static_add_ext_stat_speed_t *ext_stat_speed);


shr_error_e dnx_data_property_port_static_add_serdes_tx_taps_read(
    int unit,
    int port,
    dnx_data_port_static_add_serdes_tx_taps_t *serdes_tx_taps);


shr_error_e dnx_data_property_port_static_add_ext_stat_global_serdes_tx_taps_read(
    int unit,
    dnx_data_port_static_add_ext_stat_global_serdes_tx_taps_t *ext_stat_global_serdes_tx_taps);


shr_error_e dnx_data_property_port_static_add_ext_stat_lane_serdes_tx_taps_read(
    int unit,
    int lane,
    dnx_data_port_static_add_ext_stat_lane_serdes_tx_taps_t *ext_stat_lane_serdes_tx_taps);


shr_error_e dnx_data_property_port_static_add_fabric_quad_info_quad_enable_read(
    int unit,
    int quad,
    int *quad_enable);


shr_error_e dnx_data_property_port_static_add_erp_exist_read(
    int unit,
    int core,
    dnx_data_port_static_add_erp_exist_t *erp_exist);


shr_error_e dnx_data_property_port_static_add_fabric_fw_load_method_read(
    int unit,
    uint32 *fabric_fw_load_method);


shr_error_e dnx_data_property_port_static_add_fabric_fw_crc_check_read(
    int unit,
    uint32 *fabric_fw_crc_check);


shr_error_e dnx_data_property_port_static_add_fabric_fw_load_verify_read(
    int unit,
    uint32 *fabric_fw_load_verify);


shr_error_e dnx_data_property_port_static_add_nif_fw_load_method_read(
    int unit,
    uint32 *nif_fw_load_method);


shr_error_e dnx_data_property_port_static_add_nif_fw_crc_check_read(
    int unit,
    uint32 *nif_fw_crc_check);


shr_error_e dnx_data_property_port_static_add_nif_fw_load_verify_read(
    int unit,
    uint32 *nif_fw_load_verify);



shr_error_e dnx_data_property_port_general_fabric_port_base_read(
    int unit,
    uint32 *fabric_port_base);







































shr_error_e dnx_data_property_ingr_congestion_vsq_vsq_f_default_read(
    int unit,
    uint32 *vsq_f_default);


















































shr_error_e dnx_data_property_dram_gddr6_refresh_intervals_trefiab_read(
    int unit,
    uint32 *trefiab);


shr_error_e dnx_data_property_dram_gddr6_refresh_intervals_trefisb_read(
    int unit,
    uint32 *trefisb);


shr_error_e dnx_data_property_dram_gddr6_dq_map_read(
    int unit,
    int dram_index,
    int byte,
    dnx_data_dram_gddr6_dq_map_t *dq_map);


shr_error_e dnx_data_property_dram_gddr6_dq_channel_swap_read(
    int unit,
    int dram_index,
    dnx_data_dram_gddr6_dq_channel_swap_t *dq_channel_swap);


shr_error_e dnx_data_property_dram_gddr6_dq_byte_map_read(
    int unit,
    int dram_index,
    int byte,
    dnx_data_dram_gddr6_dq_byte_map_t *dq_byte_map);


shr_error_e dnx_data_property_dram_gddr6_ca_map_read(
    int unit,
    int dram_index,
    int channel,
    dnx_data_dram_gddr6_ca_map_t *ca_map);



shr_error_e dnx_data_property_dram_general_info_dram_info_dram_bitmap_read(
    int unit,
    uint32 *dram_bitmap);


shr_error_e dnx_data_property_dram_general_info_timing_params_read(
    int unit,
    dnx_data_dram_general_info_timing_params_t *timing_params);


shr_error_e dnx_data_property_dram_general_info_write_latency_read(
    int unit,
    uint32 *write_latency);


shr_error_e dnx_data_property_dram_general_info_read_latency_read(
    int unit,
    uint32 *read_latency);


shr_error_e dnx_data_property_dram_general_info_crc_write_latency_read(
    int unit,
    uint32 *crc_write_latency);


shr_error_e dnx_data_property_dram_general_info_crc_read_latency_read(
    int unit,
    uint32 *crc_read_latency);





























shr_error_e dnx_data_property_elk_connectivity_topology_read(
    int unit,
    dnx_data_elk_connectivity_topology_t *topology);


shr_error_e dnx_data_property_elk_connectivity_kbp_serdes_tx_taps_read(
    int unit,
    int kbp_inst,
    dnx_data_elk_connectivity_kbp_serdes_tx_taps_t *kbp_serdes_tx_taps);


shr_error_e dnx_data_property_elk_connectivity_kbp_lane_serdes_tx_taps_read(
    int unit,
    int lane_id,
    dnx_data_elk_connectivity_kbp_lane_serdes_tx_taps_t *kbp_lane_serdes_tx_taps);



















shr_error_e dnx_data_property_fabric_links_polarity_tx_polarity_read(
    int unit,
    int link,
    uint32 *tx_polarity);


shr_error_e dnx_data_property_fabric_links_polarity_rx_polarity_read(
    int unit,
    int link,
    uint32 *rx_polarity);





shr_error_e dnx_data_property_fabric_mesh_multicast_enable_read(
    int unit,
    uint32 *multicast_enable);




shr_error_e dnx_data_property_fabric_pipes_map_read(
    int unit,
    dnx_data_fabric_pipes_map_t *map);







shr_error_e dnx_data_property_fabric_dtqs_nof_active_dtqs_read(
    int unit,
    uint32 *nof_active_dtqs);



shr_error_e dnx_data_property_fabric_ilkn_bypass_info_links_read(
    int unit,
    int pm_id,
    uint32 *links);















shr_error_e dnx_data_property_fc_coe_mac_address_read(
    int unit,
    dnx_data_fc_coe_mac_address_t *mac_address);



shr_error_e dnx_data_property_fc_oob_calender_length_read(
    int unit,
    int oob_id,
    dnx_data_fc_oob_calender_length_t *calender_length);


shr_error_e dnx_data_property_fc_oob_calender_rep_count_read(
    int unit,
    int oob_id,
    dnx_data_fc_oob_calender_rep_count_t *calender_rep_count);



shr_error_e dnx_data_property_fc_inband_calender_length_read(
    int unit,
    int ilkn_id,
    dnx_data_fc_inband_calender_length_t *calender_length);


























































































































































































shr_error_e dnx_data_property_l3_rif_nof_rifs_read(
    int unit,
    uint32 *nof_rifs);












shr_error_e dnx_data_property_lane_map_nif_mapping_read(
    int unit,
    int lane_id,
    dnx_data_lane_map_nif_mapping_t *mapping);



shr_error_e dnx_data_property_lane_map_fabric_mapping_read(
    int unit,
    int lane_id,
    dnx_data_lane_map_fabric_mapping_t *mapping);



shr_error_e dnx_data_property_lane_map_ilkn_remapping_read(
    int unit,
    int lane_id,
    int ilkn_id,
    dnx_data_lane_map_ilkn_remapping_t *remapping);


























shr_error_e dnx_data_property_macsec_ingress_sa_per_sc_nof_read(
    int unit,
    uint32 *sa_per_sc_nof);



shr_error_e dnx_data_property_macsec_egress_sa_per_sc_nof_read(
    int unit,
    uint32 *sa_per_sc_nof);











shr_error_e dnx_data_property_mdb_eedb_outlif_physical_phase_granularity_data_granularity_read(
    int unit,
    int outlif_physical_phase,
    uint32 *data_granularity);
























shr_error_e dnx_data_property_module_testing_property_methods_custom_read(
    int unit,
    int link,
    int pipe,
    dnx_data_module_testing_property_methods_custom_t *custom);


















shr_error_e dnx_data_property_nif_ilkn_phys_read(
    int unit,
    int ilkn_id,
    dnx_data_nif_ilkn_phys_t *phys);


shr_error_e dnx_data_property_nif_ilkn_properties_is_over_fabric_read(
    int unit,
    int ilkn_id,
    uint32 *is_over_fabric);














































































shr_error_e dnx_data_property_sbusdma_desc_global_enable_module_desc_dma_enable_read(
    int unit,
    int module_enum_val,
    uint32 *enable);








shr_error_e dnx_data_property_sch_flow_region_type_read(
    int unit,
    int core,
    int region,
    dnx_data_sch_flow_region_type_t *region_type);


shr_error_e dnx_data_property_sch_flow_nof_remote_cores_read(
    int unit,
    int core,
    int region,
    dnx_data_sch_flow_nof_remote_cores_t *nof_remote_cores);






























































































































#endif 

