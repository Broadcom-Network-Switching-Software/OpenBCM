/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mbcm.c
 */

#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mymac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_port.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_fcf.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_fec.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mpls_term.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oamp_pe.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_l3_src_bind.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ip_tcam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_extender.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_parse.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ptp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_extend_p2p.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>

CONST mbcm_pp_functions_t mbcm_pp_arad_driver = {
	
	
	

	
	arad_pp_oam_oamp_protection_packet_header_raw_get,
	arad_pp_oam_oamp_protection_packet_header_raw_set,
	arad_pp_oam_oamp_1dm_get,
	arad_pp_oam_oamp_1dm_set,
	arad_pp_oam_bfd_acc_endpoint_tx_disable,
	arad_pp_oam_classifier_default_profile_add,
	arad_pp_oam_classifier_default_profile_remove,
	NULL,                         
	NULL,                         
	arad_pp_oam_counter_increment_bitmap_set,
	NULL,                    
	NULL  ,
	arad_pp_oam_inlif_profile_map_get,
	arad_pp_oam_inlif_profile_map_set,
	arad_pp_oam_my_cfm_mac_delete,
	arad_pp_oam_my_cfm_mac_get,
	arad_pp_oam_my_cfm_mac_set,
	arad_pp_oam_bfd_discriminator_rx_range_get,
	arad_pp_oam_bfd_discriminator_rx_range_set,
	arad_pp_oam_oamp_dm_get,
	arad_pp_oam_oamp_pe_gen_mem_set,
    NULL,
	NULL, 	
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	arad_pp_oam_oamp_lm_get,
	arad_pp_oam_oamp_loopback_get,
	arad_pp_oam_oamp_loopback_set,
	arad_pp_oam_oamp_punt_event_hendling_profile_get,
	arad_pp_oam_oamp_punt_event_hendling_profile_set,
	arad_pp_oam_oamp_rx_trap_codes_delete,
	arad_pp_oam_oamp_rx_trap_codes_set ,
	arad_pp_oam_oamp_search_for_lm_dm,
	NULL,                   
	NULL,                   
	 arad_pp_oam_slm_set,
	arad_pp_oam_tod_set,
	arad_pp_oam_tod_get,
	NULL, 
	NULL, 
	arad_pp_oamp_pe_use_1dm_check,
	NULL,                     
	NULL,                    
	arad_pp_flp_dbal_bfd_echo_program_tables_init,
	arad_pp_lem_access_bfd_one_hop_lem_entry_add,
	arad_pp_lem_access_bfd_one_hop_lem_entry_remove,
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	NULL, 
    NULL, 
    arad_pp_oam_classifier1_mip_passive_entries,
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    soc_arad_pp_oamp_control_punt_packet_int_pri_get,
    soc_arad_pp_oamp_control_ccm_weight_get,
    soc_arad_pp_oamp_control_sat_weight_get,
    soc_arad_pp_oamp_control_response_weight_get,
    soc_arad_oamp_cpu_port_dp_tc_set,
    soc_arad_oamp_tmx_arb_weight_set,
    soc_arad_oamp_sat_arb_weight_set,
    soc_arad_oamp_response_weight_set,
    arad_pp_oam_eth_oam_opcode_map_set_unsafe,
    soc_arad_pp_oam_classifier_oem_mep_mip_conflict_check,
    soc_arad_pp_set_mep_data_in_gen_mem,
    arad_pp_oamp_pe_program_profile_get,
    soc_arad_pp_oam_dma_clear,
    soc_arad_pp_oam_dma_set,
    NULL, 
    NULL, 
    NULL, 
    soc_arad_pp_oam_classifier_find_ep_id_by_lif_dir_and_mdl,
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL, 
   

	
	NULL, 
	NULL, 
	arad_pp_eg_encap_access_clear_bank,
	NULL,                    
	NULL,                    
	NULL,                    
	NULL,                    
	NULL,                    
	NULL,                    
	NULL,                    
	NULL,                    
	NULL,    
	NULL,                     
	NULL,                     
	NULL, 
    NULL, 
    NULL, 
	
	arad_pp_frwrd_fec_entry_uc_rpf_mode_set,
	arad_pp_frwrd_fcf_npv_switch_get,
	arad_pp_frwrd_fcf_npv_switch_set,
	arad_pp_frwrd_fcf_vsan_mode_get,
	arad_pp_frwrd_fcf_vsan_mode_set,
	arad_pp_frwrd_fec_is_protected_get,
    arad_pp_frwrd_extend_p2p_lem_entry_add,
    arad_pp_frwrd_extend_p2p_lem_entry_remove,
    arad_pp_frwrd_extend_p2p_lem_entry_get,
	
	arad_pp_aging_num_of_cycles_get,
	arad_pp_frwrd_mact_clear_access_bit,
	arad_pp_frwrd_mact_cpu_counter_learn_limit_get,
	arad_pp_frwrd_mact_cpu_counter_learn_limit_set,
	NULL,   
	NULL,                   
	NULL,                   
	NULL,                   
	arad_pp_frwrd_mact_opport_mode_get,
	arad_pp_frwrd_mact_transplant_static_get,
	arad_pp_frwrd_mact_transplant_static_set,
	
	arad_pp_occ_mgmt_app_get,
	arad_pp_occ_mgmt_app_set,
	arad_pp_occ_mgmt_deinit,
	arad_pp_occ_mgmt_get_app_mask,
	arad_pp_occ_mgmt_init,
	
	arad_pp_l2_lif_extender_get,
	arad_pp_l2_lif_extender_remove,
	arad_pp_extender_deinit,
	arad_pp_extender_eve_etag_format_get,
	arad_pp_extender_eve_etag_format_set,
	arad_pp_extender_global_etag_ethertype_get,
	arad_pp_extender_global_etag_ethertype_set,
	arad_pp_extender_init,
	arad_pp_extender_port_info_get,
	arad_pp_extender_port_info_set,
	
	arad_pp_frwrd_ip_ipmc_ssm_add,
	arad_pp_frwrd_ip_ipmc_ssm_delete,
	arad_pp_frwrd_ip_ipmc_ssm_get,
	arad_pp_frwrd_ip_ipmc_ssm_clear,
	NULL,                
	NULL,             
	NULL,                
	
	NULL,                   
	NULL,                   
	NULL,                   
	NULL,                   
	arad_pp_mpls_termination_spacial_labels_init,
	NULL,
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	NULL,
	
	NULL,                   
	NULL,                   
	NULL,                   
	arad_pp_mymac_vrid_mymac_map_set_to_all_vsi,
	arad_pp_mymac_vrrp_cam_info_delete,
	arad_pp_mymac_vrrp_cam_info_get,
	arad_pp_mymac_vrrp_cam_info_set,
	
	arad_pp_mtr_policer_global_sharing_get,
	arad_pp_mtr_policer_global_sharing_set,
    NULL, 
    NULL, 	
    arad_pp_mtr_none_ethernet_packet_ptr_get,
    arad_pp_mtr_none_ethernet_packet_ptr_set,    
	NULL, 
	NULL, 
	arad_pp_mtr_policer_ipg_compensation_get,
	arad_pp_mtr_policer_ipg_compensation_set,
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	NULL, 
	
	NULL,                   
	NULL,                   
	NULL,                   
	NULL,                  
	NULL,                   
	NULL,                   
	NULL,                   
	NULL,                   
	
	arad_pp_ip6_compression_add,
	arad_pp_ip6_compression_delete,
	arad_pp_ip6_compression_get,
	
	arad_pp_mtr_meter_ins_bucket_get,
	arad_pp_metering_dp_map_entry_get,
	arad_pp_metering_dp_map_entry_set,
	arad_pp_metering_pcd_entry_get,
	arad_pp_metering_pcd_entry_set,
	
	arad_pp_port_additional_tpids_get,
	arad_pp_port_additional_tpids_set,
	arad_pp_port_property_get,
	arad_pp_port_property_set,
	
	NULL,
	NULL,                    
	
	NULL,                 
	NULL,                
    NULL,               
    NULL,               
    NULL,               
    NULL,               
    NULL,               
    arad_pp_flp_not_found_trap_config_set,
    arad_pp_flp_not_found_trap_config_get,
	
	NULL,                     
	NULL, 
	NULL, 
	NULL, 
	
	NULL, 
	NULL, 
	
	NULL, 
	NULL, 
	arad_pp_llp_parse_packet_format_eg_info_get,
	arad_pp_llp_parse_packet_format_eg_info_set,
	
	arad_pp_ptp_p2p_delay_get,
	arad_pp_ptp_p2p_delay_set,
	
	NULL,                    
	NULL,                   
	NULL,                   
	NULL,                   
	
	arad_pp_fp_presel_max_id_get,
	
	arad_pp_eg_trill_entry_set,
    
    NULL,                    
    NULL, 
    NULL, 
	NULL, 
	NULL, 
	
	arad_pp_lif_ing_vlan_edit_pcp_map_dp_set,
	arad_pp_lif_ing_vlan_edit_pcp_map_dp_get,
	arad_pp_lif_ing_vlan_edit_inner_global_info_set,
	arad_pp_lif_ing_vlan_edit_inner_global_info_get,
	arad_pp_lif_ing_vlan_edit_outer_global_info_set,
	arad_pp_lif_ing_vlan_edit_outer_global_info_get,
	NULL, 
	NULL, 
	NULL, 
	NULL, 
    NULL,  
    NULL, 
    NULL, 
    NULL, 
    arad_pp_eg_encap_access_default_ac_entry_vlan_edit_profile_set, 
    arad_pp_eg_encap_access_default_ac_entry_vlan_edit_profile_get, 
    arad_pp_meter_tc_map_set,
    arad_pp_meter_tc_map_get,
    NULL, 
    NULL, 
    NULL, 
    NULL, 
    NULL,  
    NULL, 
    NULL,   
    NULL, 
    NULL, 
    NULL  
};
