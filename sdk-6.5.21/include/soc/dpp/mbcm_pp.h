/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mbcm_pp.h
 * Purpose:     Multiplexing of the bcm layer for PP
 *
 * Different chip families require such different implementations
 * of some basic BCM layer functionality that the functions are
 * multiplexed to allow a fast runtime decision as to which function
 * to call.  This file contains the basic declarations for this
 * process.
 * 
 *
 * See internal/design/soft_arch/xgs_plan.txt for more info.
 *
 * Conventions:
 *    MBCM_PP is the multiplexed pp bcm prefix
 *    _f is the function type declaration postfix
 */

#ifndef _SOC_DPP_MBCM_PP_H
#define _SOC_DPP_MBCM_PP_H

#include <bcm/types.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/error.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_vlan_edit.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oamp_pe.h>
#include <soc/dpp/PPC/ppc_api_extender.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>
#include <soc/dpp/PPC/ppc_api_port.h>
#include <soc/dpp/PPC/ppc_api_fp.h>
#include <soc/dpp/PPC/ppc_api_eg_encap.h>
#include <soc/dpp/PPC/ppc_api_eg_mirror.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/PPC/ppc_api_l3_src_bind.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>
#include <soc/dpp/PPC/ppc_api_metering.h>
#include <soc/dpp/PPC/ppc_api_diag.h>
#include <soc/dpp/PPC/ppc_api_lif.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_eg_encap_access.h>
#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif_ing_vlan_edit.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lag.h>

typedef enum soc_pp_chip_family_e {
    BCM_PP_FAMILY_PETRAB,   
    BCM_PP_FAMILY_ARAD,
    BCM_PP_FAMILY_JERICHO,
    BCM_PP_FAMILY_JERICHO_PLUS,
    BCM_PP_FAMILY_QAX
} soc_pp_chip_family_t;



 
typedef soc_error_t 
    (*mbcm_pp_frwrd_mact_opport_mode_get_f)(
       SOC_SAND_IN  int                   unit,
       SOC_SAND_OUT  uint32               *opport
       );
 
typedef soc_error_t
    (*mbcm_pp_frwrd_mact_clear_access_bit_f)(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  int                                 fid,
       SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS             *dst_mac_address
       );

typedef soc_error_t 
   (*mbcm_pp_frwrd_mact_event_handle_info_set_dma_f)(
       SOC_SAND_IN int enable_dma,
       SOC_SAND_IN  int unit
       );

typedef soc_error_t 
   (*mbcm_pp_frwrd_mact_learning_dma_set_f)(
       SOC_SAND_IN  int unit
       );

typedef soc_error_t 
   (*mbcm_pp_frwrd_mact_learning_dma_unset_f)(
       SOC_SAND_IN  int unit
       );
 
typedef soc_error_t 
   (*mbcm_pp_frwrd_mact_event_handler_callback_register_f)(
       SOC_SAND_IN  int                                      unit,
       SOC_SAND_IN  l2_event_fifo_interrupt_handler_cb_t     event_handler_cb
       );

typedef soc_error_t
  (*mbcm_pp_frwrd_mact_cpu_counter_learn_limit_set_f) (
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint8   disable
  );

typedef soc_error_t
  (*mbcm_pp_frwrd_mact_cpu_counter_learn_limit_get_f) (
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint8*  is_enabled
  );

typedef soc_error_t
  (*mbcm_pp_frwrd_mact_transplant_static_set_f) (
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint8   enable
  );

typedef soc_error_t
  (*mbcm_pp_frwrd_mact_transplant_static_get_f) (
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint8*  is_enabled
  );

typedef soc_error_t 
  (*mbcm_pp_mpls_termination_spacial_labels_init_f)(
    int unit
  );

typedef soc_error_t 
  (*mbcm_pp_mpls_termination_l4b_l5l_init_f)(
    int unit
  );

typedef soc_error_t
  (*mbcm_pp_mpls_termination_range_action_set_f)(
    int unit, 
    uint32 entry_index, 
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *range_action_info,
	uint32 flags
  );

typedef soc_error_t 
  (*mbcm_pp_mpls_termination_range_action_get_f)(
    int unit, 
    uint32 entry_index, 
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *range_action_info
  );

typedef soc_error_t 
  (*mbcm_pp_mpls_termination_range_profile_set_f)(
    int unit, 
    uint32 entry_index, 
    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *range_profile_info
  );

typedef soc_error_t 
  (*mbcm_pp_mpls_termination_range_profile_get_f)(
    int unit, 
    uint32 entry_index, 
    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *range_profile_info
  );
typedef soc_error_t 
  (*mbcm_pp_mpls_termination_vccv_type_ttl1_oam_classification_set_f)(
    int unit, 
    uint8 vccv_type_ttl1_oam_classification_enable
  );

typedef soc_error_t 
  (*mbcm_pp_mpls_termination_vccv_type_ttl1_oam_classification_get_f)(
    int unit, 
    uint8 * vccv_type_ttl1_oam_classification_enable
  );
typedef soc_error_t
  (*mbcm_pp_mpls_termination_first_label_bit_map_set_f)(
    int unit,
    uint32 termination_bit_map
  );

typedef soc_error_t
  (*mbcm_pp_mpls_termination_first_label_bit_map_get_f)(
    int unit,
    uint32 * termination_bit_map
  );
typedef soc_error_t
  (*mbcm_pp_mpls_termination_first_label_bit_map_init_f)(
    int unit,
    uint32 termination_bit_map
  );
typedef soc_error_t
    (*mbcm_pp_frwrd_fcf_vsan_mode_set_f)(int unit, int enable);

typedef soc_error_t
    (*mbcm_pp_frwrd_fcf_vsan_mode_get_f)(int unit, int* enable);

typedef soc_error_t
    (*mbcm_pp_frwrd_fcf_npv_switch_set_f)(int unit, int enable);

typedef soc_error_t
    (*mbcm_pp_frwrd_fcf_npv_switch_get_f)(int unit, int* enable);

typedef soc_error_t
    (*mbcm_pp_mymac_protocol_group_set_f)(int unit, uint32 protocols, uint32 group);

typedef soc_error_t 
    (*mbcm_pp_mymac_protocol_group_get_protocol_by_group_f)(int unit, uint8 group, uint32 *protocols);

typedef soc_error_t 
    (*mbcm_pp_mymac_protocol_group_get_group_by_protocols_f)(int unit, uint32 protocols, uint8 *group);

typedef soc_error_t
(*mbcm_pp_mymac_vrid_mymac_map_set_to_all_vsi_f)(int unit, int cam_index, int enable);

typedef soc_error_t
(*mbcm_pp_mymac_vrrp_cam_info_set_f)(int unit, SOC_PPC_VRRP_CAM_INFO *info);

typedef soc_error_t
(*mbcm_pp_mymac_vrrp_cam_info_get_f)(int unit, SOC_PPC_VRRP_CAM_INFO *info);

typedef soc_error_t
(*mbcm_pp_mymac_vrrp_cam_info_delete_f)(int unit, uint8 cam_index);

typedef soc_error_t 
(*mbcm_pp_port_property_set_f)(int unit, int core, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 value);

typedef soc_error_t 
(*mbcm_pp_port_property_get_f)(int unit, int core, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 *value);

typedef soc_error_t
(*mbcm_pp_oam_my_cfm_mac_delete_f)(int unit, int core, SOC_SAND_PP_MAC_ADDRESS *dst_mac_address, uint32 table_index);

typedef soc_error_t 
(*mbcm_pp_oam_my_cfm_mac_set_f)(int unit, int core, SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address, uint32 table_index);

typedef soc_error_t 
(*mbcm_pp_oam_my_cfm_mac_get_f)(int unit, int core, SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address, uint32 table_index);

typedef soc_error_t 
(*mbcm_pp_occ_mgmt_app_set_f)(int unit, SOC_OCC_MGMT_TYPE interface_type, int application_type, uint32 val, SHR_BITDCL *full_profile);

typedef soc_error_t 
(*mbcm_pp_occ_mgmt_app_get_f)(int unit, SOC_OCC_MGMT_TYPE interface_type, int application_type, SHR_BITDCL *full_profile, uint32 *val);

typedef soc_error_t 
(*mbcm_pp_occ_mgmt_get_app_mask_f)(int unit, SOC_OCC_MGMT_TYPE interface_type, int application_type, SHR_BITDCL *mask);

typedef soc_error_t 
(*mbcm_pp_occ_mgmt_init_f)(int unit);

typedef soc_error_t 
(*mbcm_pp_occ_mgmt_deinit_f)(int unit);

typedef soc_error_t 
(*mbcm_pp_presel_max_id_get_f)(int unit, SOC_PPC_FP_DATABASE_STAGE  stage, int *max_presel_id);

typedef soc_error_t
(*mbcm_pp_eg_rif_profile_set_f)(int unit, uint32 rif_profile_index, QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT  *tbl_data);

typedef soc_error_t
(*mbcm_pp_eg_rif_profile_get_f)(int unit, uint32 rif_profile_index, QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT  *tbl_data);

typedef soc_error_t
(*mbcm_pp_eg_rif_profile_delete_f)(int unit, uint32 rif_profile_index);


typedef soc_error_t (*mbcm_pp_ingress_protection_state_set_f)(
    int unit,
    uint32 protection_ndx,
    uint8 path_state);

typedef soc_error_t (*mbcm_pp_ingress_protection_state_get_f)(
    int unit,
    uint32 protection_ndx,
    uint8 *path_state);

typedef soc_error_t (*mbcm_pp_egress_protection_state_set_f)(
    int unit,
    uint32 protection_ndx,
    uint8 path_state);

typedef soc_error_t (*mbcm_pp_egress_protection_state_get_f)(
    int unit,
    uint32 protection_ndx,
    uint8 *path_state);

typedef soc_error_t (* mbcm_pp_aging_num_of_cycles_get_f) (
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  aging_profile,
    SOC_SAND_OUT int     *fid_aging_cycles
);

typedef soc_error_t (*mbcm_pp_lif_glem_access_entry_add_f         )(int unit, int global_lif_index, int egress_lif_index);
typedef soc_error_t (*mbcm_pp_lif_glem_access_entry_remove_f      )(int unit, int global_lif_index);
typedef soc_error_t (*mbcm_pp_lif_glem_access_entry_by_key_get_f  )(int unit, int global_lif_id, int *egress_lif_id, uint8 *accessed, uint8 *is_found);

typedef soc_error_t 
(*mbcm_pp_port_additional_tpids_set_f) (int unit, SOC_PPC_ADDITIONAL_TPID_VALUES *additional_tpids);

typedef soc_error_t 
(*mbcm_pp_port_additional_tpids_get_f) (int unit, SOC_PPC_ADDITIONAL_TPID_VALUES *additional_tpids);






typedef soc_error_t (*mbcm_pp_oam_two_ints_one_uint8_star) (int, int, uint8*);
typedef soc_error_t (*mbcm_pp_oam_two_ints_one_uint8) (int, int, uint8);
typedef soc_error_t (*mbcm_pp_oam_three_ints) (int, int, int);
typedef soc_error_t (*mbcm_pp_oam_one_int_two_int_stars) (int, int*, int*);
typedef soc_error_t (*mbcm_pp_oam_two_ints_one_const_uint8_star) (int, int, const uint8*);
typedef soc_error_t (*mbcm_pp_oam_one_int_three_uint8s) (int, uint8, uint8, uint8);
typedef soc_error_t (*mbcm_pp_oam_oamp_rx_trap_code) (int, SOC_PPC_OAM_MEP_TYPE, uint32);

typedef soc_error_t (*mbcm_pp_oam_oamp_gen_mem_get_f) (int, int, uint32 *);

typedef soc_error_t (*mbcm_pp_oam_two_ints) (int,int);
typedef soc_error_t (*mbcm_pp_oam_counter_increment_bitmap) (int,int);


typedef soc_error_t (*mbcm_pp_network_group_config_set_f)
    (SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32 source_network_group_id,
    SOC_SAND_IN uint32 dest_network_group_id, 
    SOC_SAND_IN uint32 is_filter);

typedef soc_error_t (*mbcm_pp_network_group_config_get_f)
    (SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32 source_network_group_id,
    SOC_SAND_IN uint32 dest_network_group_id,
    SOC_SAND_OUT uint32 *is_filter);


typedef soc_error_t (*mbcm_pp_oam_inlif_profile_map_set_f)
(SOC_SAND_IN int,SOC_SAND_IN uint32,SOC_SAND_IN uint32);
typedef soc_error_t (*mbcm_pp_oam_inlif_profile_map_get_f)
(SOC_SAND_IN int,SOC_SAND_IN uint32,SOC_SAND_OUT uint32*);

typedef uint32 (*mbcm_pp_oam_oamp_punt_event_hendling_profile_set_f)
(SOC_SAND_IN int unit, SOC_SAND_IN uint32 profile_ndx, SOC_SAND_IN SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA *punt_profile_data);

typedef uint32 (*mbcm_pp_oam_oamp_punt_event_hendling_profile_get_f)
(SOC_SAND_IN int unit, SOC_SAND_IN uint32 profile_ndx, SOC_SAND_OUT SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA *punt_profile_data);

typedef soc_error_t (*mbcm_pp_oam_classifier_default_profile_add_f)
(SOC_SAND_IN int,SOC_SAND_IN ARAD_PP_OAM_DEFAULT_EP_ID,SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY*,SOC_SAND_IN uint8);

typedef soc_error_t (*mbcm_pp_oam_classifier_default_profile_remove_f)
(SOC_SAND_IN int,SOC_SAND_IN ARAD_PP_OAM_DEFAULT_EP_ID);

typedef soc_error_t (*mbcm_pp_oamp_pe_use_1dm_check_f)
(SOC_SAND_IN int, SOC_SAND_IN uint32);

typedef soc_error_t (*mbcm_pp_oam_tod_set_f) (int, uint8, uint64);
typedef soc_error_t (*mbcm_pp_oam_tod_get_f) (int, uint8, uint64*);

typedef soc_error_t (*mbcm_pp_oam_tod_leap_control_set_f) (int, bcm_oam_timestamp_format_t, bcm_oam_tod_leap_control_t);
typedef soc_error_t (*mbcm_pp_oam_tod_leap_control_get_f) (int, bcm_oam_timestamp_format_t, bcm_oam_tod_leap_control_t*);


typedef soc_error_t (*mbcm_pp_oamp_report_mode_set_f) (int, SOC_PPC_OAM_REPORT_MODE);

typedef soc_error_t (*mbcm_pp_oamp_report_mode_get_f) (int, SOC_PPC_OAM_REPORT_MODE*);

typedef soc_error_t (*mbcm_pp_oam_bfd_flexible_verification_set_f)(int unit, SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO *info);
typedef soc_error_t (*mbcm_pp_oam_bfd_flexible_verification_get_f)(int unit, SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO *info);
typedef soc_error_t (*mbcm_pp_oam_bfd_flexible_verification_delete_f)(int unit, SOC_PPC_OAM_BFD_FLEXIBLE_VERIFICATION_INFO *info);

typedef soc_error_t (*mbcm_pp_oam_bfd_mep_db_ext_data_set_f)    (int unit, SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_INFO *info);
typedef soc_error_t (*mbcm_pp_oam_bfd_mep_db_ext_data_get_f)    (int unit, SOC_PPC_OAM_BFD_MEP_DB_EXT_DATA_INFO *info);
typedef soc_error_t (*mbcm_pp_oam_bfd_mep_db_ext_data_delete_f) (int unit, int extension_idx);

typedef soc_error_t (*mbcm_pp_oamp_control_lm_pcp_counter_range_min_get_f)(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *range_min
  );
typedef soc_error_t (*mbcm_pp_oamp_control_lm_pcp_counter_range_max_get_f)(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *range_max
  );
typedef soc_error_t (*mbcm_pp_oamp_control_punt_packet_int_pri_get_f)(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *DP_and_TC
  );
typedef soc_error_t (*mbcm_pp_oamp_control_ccm_weight_get_f)(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *ccm_weight
  );
typedef soc_error_t (*mbcm_pp_oamp_control_sat_weight_get_f)(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *sat_weight
  );
typedef soc_error_t (*mbcm_pp_oamp_control_response_weight_get_f)(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *response_weight
  );

typedef soc_error_t (*mbcm_pp_rif_global_urpf_mode_set_f) (int, int, int);

typedef int (*mbcm_pp_bfd_one_hop_lem_entry_add_f)
(int,const ARAD_PP_LEM_BFD_ONE_HOP_ENTRY_INFO *);

typedef int (*mbcm_pp_bfd_one_hop_lem_entry_remove_f)
(int,  uint32, uint32);

typedef int (*mbcm_pp_oam_bfd_acc_endpoint_tx_disable_f) (int, uint32, const SOC_PPC_OAM_OAMP_MEP_DB_ENTRY*);

typedef uint32 (*mbcm_pp_oam_oamp_loopback_set_f)(const int,ARAD_PP_OAM_LOOPBACK_INFO*);
typedef uint32 (*mbcm_pp_oam_oamp_loopback_get_f)(const int,ARAD_PP_OAM_LOOPBACK_INFO*);

typedef soc_error_t (*mbcm_pp_oam_oamp_lm_dm_pointed_f)(int,ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO*);
typedef soc_error_t (*mbcm_pp_oam_oamp_search_for_lm_dm_f)(int, uint32, uint32*);
typedef soc_error_t (*mbcm_pp_oam_oamp_dm_get_f)(int, SOC_PPC_OAM_OAMP_DM_INFO_GET*, uint8*);
typedef soc_error_t (*mbcm_pp_oam_oamp_lm_get_f)(int, SOC_PPC_OAM_OAMP_LM_INFO_GET*);


typedef soc_error_t (*mbcm_pp_eg_encap_ether_type_index_clear_f) (int, int);
typedef soc_error_t (*mbcm_pp_eg_encap_ether_type_index_set_f) (SOC_SAND_IN int, SOC_SAND_IN int, SOC_SAND_IN SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO*); 
typedef soc_error_t (*mbcm_pp_eg_encap_ether_type_index_get_f) (SOC_SAND_IN int, SOC_SAND_IN int, SOC_SAND_OUT SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO*); 
typedef soc_error_t (*mbcm_pp_eg_trill_entry_set_f) (SOC_SAND_IN int, SOC_SAND_IN uint32, SOC_SAND_IN SOC_PPC_EG_ENCAP_TRILL_INFO*);


typedef soc_error_t (*mbcm_pp_lif_additional_data_set_f)
    (SOC_SAND_IN int unit, 
    SOC_SAND_IN int lif_id,
    SOC_SAND_IN uint8 is_ingress,
    SOC_SAND_IN uint64 data,
    SOC_SAND_IN uint8 is_clear);

typedef soc_error_t (*mbcm_pp_lif_additional_data_get_f)
    (SOC_SAND_IN int unit, 
    SOC_SAND_IN int lif_id,
    SOC_SAND_IN uint8 is_ingress,
    SOC_SAND_OUT uint64 *data);

typedef soc_error_t (*mbcm_pp_lif_is_wide_entry_f)
    (SOC_SAND_IN int unit, 
    SOC_SAND_IN int lif_id,
    SOC_SAND_IN uint8 is_ingress,
    SOC_SAND_OUT uint8 *is_wide_entry,
    SOC_SAND_OUT uint8 *ext_type);


typedef soc_error_t (*mbcm_pp_ip6_compression_add_f) (SOC_SAND_IN int, SOC_SAND_INOUT SOC_PPC_IPV6_COMPRESSED_ENTRY*, SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE*); 
typedef soc_error_t (*mbcm_pp_ip6_compression_delete_f) (SOC_SAND_IN int, SOC_SAND_INOUT SOC_PPC_IPV6_COMPRESSED_ENTRY*, SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE*);
typedef soc_error_t (*mbcm_pp_ip6_compression_get_f) (SOC_SAND_IN int, SOC_SAND_INOUT SOC_PPC_IPV6_COMPRESSED_ENTRY*, SOC_SAND_OUT uint8*);


typedef soc_error_t (*mbcm_pp_diag_kaps_lkup_info_get_f)(SOC_SAND_IN int unit, SOC_SAND_IN int core_id);

typedef soc_error_t (*mbcm_pp_get_load_balancing_diagnostic_signals_f)(SOC_SAND_IN int unit, SOC_SAND_INOUT ARAD_PP_LB_PRINT_SIGNALS* lb_signals);

typedef soc_error_t (*mbcm_pp_eg_encap_direct_bank_set_f)(int unit, int bank, uint8 is_mapped);

typedef soc_error_t (*mbcm_pp_eg_encap_extension_mapping_set_f)(SOC_SAND_IN int unit, SOC_SAND_IN int bank, SOC_SAND_IN uint32 is_extended, SOC_SAND_IN uint32 extesnion_bank);

typedef soc_error_t (*mbcm_pp_eg_encap_extension_mapping_get_f)(SOC_SAND_IN int unit, SOC_SAND_IN int bank, SOC_SAND_OUT uint32* is_extended, SOC_SAND_OUT uint32* extesnion_bank);

typedef soc_error_t (*mbcm_pp_eg_encap_extension_type_set_f)(SOC_SAND_IN int unit, SOC_SAND_IN int bank, SOC_SAND_IN uint8 is_ext_data);

typedef soc_error_t (*mbcm_pp_eg_encap_extension_type_get_f)(SOC_SAND_IN int unit, SOC_SAND_IN int bank, SOC_SAND_OUT uint8* is_ext_data);



typedef soc_error_t (*mbcm_pp_ipmc_ssm_tcam_entry_add_f)(SOC_SAND_IN int unit,
                                                         SOC_SAND_IN SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
                                                         SOC_SAND_IN SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
                                                         SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE* success);

typedef soc_error_t (*mbcm_pp_ipmc_ssm_tcam_entry_delete_f)(SOC_SAND_IN int unit,
                                                            SOC_SAND_IN SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key);

typedef soc_error_t (*mbcm_pp_ipmc_ssm_tcam_entry_get_f)(SOC_SAND_IN  int unit,
                                                         SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
                                                         SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
                                                         SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS*    route_status,
                                                         SOC_SAND_OUT uint8* found);

typedef soc_error_t (*mbcm_pp_extender_port_info_set_f)(int unit, SOC_PPC_PORT port, SOC_PPC_EXTENDER_PORT_INFO *port_info);
typedef soc_error_t (*mbcm_pp_extender_port_info_get_f)(int unit, SOC_PPC_PORT port, SOC_PPC_EXTENDER_PORT_INFO *port_info);

typedef soc_error_t (*mbcm_pp_extender_init_f)(int unit);
typedef soc_error_t (*mbcm_pp_extender_deinit_f)(int unit);

typedef soc_error_t (*mbcm_pp_flp_dbal_bfd_echo_program_tables_init_f)(int unit);

typedef uint32 (*mbcm_pp_oam_classifier_oem1_entry_set_unsafe_f)(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY *oem1_key, SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload);

typedef uint32 (*mbcm_pp_oam_classifier_oem1_init_f)(SOC_SAND_IN int unit);


typedef soc_error_t (*mbcm_pp_mtr_meter_ins_bucket_get_f)(int unit,
							  int core_id,
							  SOC_PPC_MTR_METER_ID *meter_ins_ndx,
							  int *cbl,
							  int *ebl);

typedef soc_error_t (*mbcm_pp_ptp_p2p_delay_set_f) (SOC_SAND_IN int, SOC_SAND_IN SOC_PPC_PORT, SOC_SAND_IN int);
typedef soc_error_t (*mbcm_pp_ptp_p2p_delay_get_f) (SOC_SAND_IN int, SOC_SAND_IN SOC_PPC_PORT, SOC_SAND_OUT int*);
 
typedef soc_error_t(*mbcm_pp_mtr_policer_global_sharing_set_f)
  (
	int                         unit,
	int                         core_id,
	int        					meter_id,
	int							meter_group,
	uint32* 					global_sharing_ptr
  );

typedef soc_error_t(*mbcm_pp_mtr_policer_global_sharing_get_f)
  (
   int                         	unit,
   int                         	core_id,
   int        					meter_id,
   int							meter_group,
   uint32* 						global_sharing_ptr
  );

typedef soc_error_t(*mbcm_pp_mtr_policer_global_mef_10_3_set_f)
  (
	int                         unit,
	int* 					    arg
  );

typedef soc_error_t(*mbcm_pp_mtr_policer_global_mef_10_3_get_f)
  (
    int                        	unit,
    int* 					    arg   
  );

typedef soc_error_t(*mbcm_pp_mtr_none_ethernet_packet_ptr_get_f)
  (
	int                         unit,
	int* 					    arg
  );

typedef soc_error_t(*mbcm_pp_mtr_none_ethernet_packet_ptr_set_f)
  (
    int                        	unit,
    int* 					    arg   
  );  

typedef uint32(*mbcm_pp_mtr_policer_ipg_compensation_set_f)
  (
	int                         unit,
	uint8 ipg_compensation_enabled
  );

typedef uint32(*mbcm_pp_mtr_policer_ipg_compensation_get_f)
  (
	int                         unit,
	uint8 *ipg_compensation_enabled
  );

typedef uint32(*mbcm_pp_mtr_policer_hdr_compensation_set_f)
  (
	int                         unit,
	int                         core_id,
	uint32                      pp_port,
	int                         compensation
  );

typedef uint32(*mbcm_pp_mtr_policer_hdr_compensation_get_f)
  (
	int                         unit,
	int                         core_id,
	uint32                      pp_port,
	int                         *compensation
  );


typedef uint32(*mbcm_pp_metering_pcd_entry_get_f)
  (
	SOC_SAND_IN int                         unit,
	SOC_SAND_OUT SOC_PPC_MTR_COLOR_DECISION_INFO *pcd_entry
  );

typedef uint32(*mbcm_pp_metering_pcd_entry_set_f)
  (
	SOC_SAND_IN int                         unit,
	SOC_SAND_IN SOC_PPC_MTR_COLOR_DECISION_INFO *pcd_entry
  );

typedef uint32(*mbcm_pp_metering_dp_map_entry_get_f)
  (
	SOC_SAND_IN int                         unit,
	SOC_SAND_OUT SOC_PPC_MTR_COLOR_RESOLUTION_INFO *dp_map_entry
  );

typedef uint32(*mbcm_pp_metering_dp_map_entry_set_f)
  (
	SOC_SAND_IN int                         unit,
	SOC_SAND_IN SOC_PPC_MTR_COLOR_RESOLUTION_INFO *dp_map_entry
  );




typedef soc_error_t (*mbcm_pp_eg_encap_map_encap_intpri_color_set_f)(
    int                            unit,
    int                            index,
    SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO *  entry_info
  );

typedef soc_error_t (*mbcm_pp_eg_encap_map_encap_intpri_color_get_f)(
    int                            unit,
    int                            index,
    SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO *  entry_info
  );


typedef soc_error_t (*mbcm_pp_eg_etpp_trap_set_f )(
    int                            unit,
    SOC_PPC_TRAP_ETPP_TYPE trap,
    SOC_PPC_TRAP_ETPP_INFO *entry_info
  );

typedef soc_error_t (*mbcm_pp_eg_etpp_trap_get_f )(
    int                            unit,
    SOC_PPC_TRAP_ETPP_TYPE trap,
    SOC_PPC_TRAP_ETPP_INFO *entry_info
  );

typedef soc_error_t (*mbcm_pp_eg_etpp_out_lif_mtu_map_set_f )(
    int unit,
    uint32 out_lif_profile_bit_mask,
    uint32 mtu_profile,
    uint32 mtu_val
  );

typedef soc_error_t (*mbcm_pp_eg_etpp_out_lif_mtu_map_get_f )(
    int unit,
    uint32 out_lif_profile,
    uint32 *mtu_val
  );

typedef soc_error_t (*mbcm_pp_eg_etpp_out_lif_mtu_check_set_f)(
    int unit,
    uint32 enable
  );

typedef soc_error_t (*mbcm_pp_trap_ingress_ser_set_f)(
    int unit,
    int enable,
    int trap_hw_id
  );

typedef uint32 (*mbcm_pp_flp_not_found_trap_config_set_f)(
    int unit,
    int hw_trap_id
  );

typedef uint32 (*mbcm_pp_flp_not_found_trap_config_get_f)(
    int unit,
    int *hw_trap_id
  );

typedef soc_error_t (*mbcm_pp_eg_encap_access_clear_bank_f)(int unit, int bank_id);
typedef soc_error_t (*mbcm_pp_llp_parse_packet_format_eg_info_set_f)
(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );

typedef soc_error_t(*mbcm_pp_llp_parse_packet_format_eg_info_get_f)
  (
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT SOC_PPC_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );


typedef soc_error_t (*mbcm_pp_extender_global_etag_ethertype_set_f)(int unit, uint16 etag_tpid);
typedef soc_error_t (*mbcm_pp_extender_global_etag_ethertype_get_f)(int unit, uint16 *etag_tpid);
typedef soc_error_t (*mbcm_pp_oam_loopback_tst_info_init_f)(SOC_SAND_IN int unit,
        SOC_SAND_INOUT JER_PP_OAM_LOOPBACK_TST_INFO *lb_tst_info);

typedef soc_error_t (*mbcm_pp_oam_oamp_lb_tst_header_set_f)(
         SOC_SAND_IN int unit, 
         SOC_SAND_IN JER_PP_OAM_LOOPBACK_TST_INFO *lb_tst_info,
         SOC_SAND_INOUT uint8* header_buffer, 
         SOC_SAND_OUT int *header_offset);

typedef soc_error_t (*mbcm_pp_diag_glem_signals_get_f)(int unit, int core_id, soc_ppc_diag_glem_signals_t *result);

typedef soc_error_t (*mbcm_pp_l2_lif_extender_remove_f)(int unit, SOC_PPC_L2_LIF_EXTENDER_KEY *extender_key, int *lif_index);

typedef uint32 (*mbcm_pp_l2_lif_extender_get_f)(
    int                                unit,
    SOC_PPC_L2_LIF_EXTENDER_KEY        *extender_key,
    SOC_PPC_LIF_ID                     *lif_index,
    SOC_PPC_L2_LIF_EXTENDER_INFO       *extender_info,
    uint8                              *found);

typedef uint32 (*mbcm_pp_mtr_eth_policer_params_set_f)(
    SOC_SAND_IN  int                         	unit,
	SOC_SAND_IN  int                         	core_id,
    SOC_SAND_IN  SOC_PPC_PORT                   port_ndx,
	SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE           eth_type_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO    *policer_info);

typedef uint32 (*mbcm_pp_mtr_eth_policer_params_get_f)(
    SOC_SAND_IN  int                         	unit,
	SOC_SAND_IN  int                         	core_id,
    SOC_SAND_IN  SOC_PPC_PORT                   port_ndx,
	SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE           eth_type_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO    *policer_info);

typedef uint32 (*mbcm_pp_mtr_eth_policer_glbl_profile_set_f)(
	SOC_SAND_IN  int                      		unit,
	SOC_SAND_IN  int                         	core_id,
	SOC_SAND_IN  uint32                    		glbl_profile_idx,
	SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO 	*policer_info);

typedef uint32 (*mbcm_pp_mtr_eth_policer_glbl_profile_get_f)(
	SOC_SAND_IN  int                      		unit,
	SOC_SAND_IN  int                         	core_id,
    SOC_SAND_IN  uint32                    		glbl_profile_idx,
	SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO 	*policer_info);

typedef uint32 (*mbcm_pp_mtr_eth_policer_glbl_profile_map_set_f)(
	SOC_SAND_IN  int                  	unit,
	SOC_SAND_IN  int                    core_id,
	SOC_SAND_IN  SOC_PPC_PORT           port,
	SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE   eth_type_ndx,
	SOC_SAND_IN  uint32                 glbl_profile_idx);

typedef uint32 (*mbcm_pp_mtr_eth_policer_glbl_profile_map_get_f)(
	SOC_SAND_IN  int                  	unit,
	SOC_SAND_IN  int                    core_id,
	SOC_SAND_IN  SOC_PPC_PORT           port,
	SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE   eth_type_ndx,
	SOC_SAND_OUT  uint32                *glbl_profile_idx);

typedef uint32(*mbcm_pp_ecmp_hash_slb_combine_get_f)
  (
	int unit,
	int *combine_slb
  );

typedef uint32(*mbcm_pp_ecmp_hash_slb_combine_set_f)
  (
	int unit,
	int combine_slb
  );
	
typedef soc_error_t (*mbcm_pp_extender_eve_etag_format_set_f)(int unit, uint32 edit_profile, uint8 is_extender);
typedef soc_error_t (*mbcm_pp_extender_eve_etag_format_get_f)(int unit, uint32 edit_profile, uint8 *is_extender);

typedef soc_error_t (*mbcm_pp_eg_encap_ip_tunnel_size_protocol_template_set_f) (
   int                                                    unit, 
   int                                                    encapsulation_mode, 
   SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO *ip_tunnel_size_protocol_template); 

typedef soc_error_t (*mbcm_pp_ipmc_ssm_entry_add_f)(SOC_SAND_IN  int unit,
                                                    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
                                                    SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
                                                    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE* success);

typedef soc_error_t (*mbcm_pp_ipmc_ssm_entry_delete_f)(SOC_SAND_IN int unit,
                                                       SOC_SAND_IN SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key);

typedef soc_error_t (*mbcm_pp_ipmc_ssm_entry_get_f )( SOC_SAND_IN  int unit,
                                                      SOC_SAND_IN  SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
                                                      SOC_SAND_OUT SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
                                                      SOC_SAND_OUT SOC_PPC_FRWRD_IP_ROUTE_STATUS*    route_status,
                                                      SOC_SAND_OUT uint8* found);

typedef soc_error_t (*mbcm_pp_ipmc_ssm_entry_clear_f)(SOC_SAND_IN  int unit);

typedef uint32(*mbcm_pp_frwrd_fec_is_protected_get_f)
  (
    int                         unit,
	uint32                      fec_ndx,
	uint8						*is_protected
  );

typedef soc_error_t(*mbcm_pp_frwrd_extend_p2p_lem_entry_add_f)
  (
    int unit, 
    SOC_PPC_FRWRD_MATCH_INFO *fwd_match_info,
    SOC_PPC_FRWRD_DECISION_INFO *frwrd_info
  );

typedef soc_error_t(*mbcm_pp_frwrd_extend_p2p_lem_entry_remove_f)
  (
    int unit, 
    SOC_PPC_FRWRD_MATCH_INFO *fwd_match_info
  );

typedef soc_error_t(*mbcm_pp_frwrd_extend_p2p_lem_entry_get_f)
  (
    int unit, 
    SOC_PPC_FRWRD_MATCH_INFO *fwd_match_info,
    SOC_PPC_FRWRD_DECISION_INFO *frwrd_info,
    uint8 * found
  );

typedef soc_error_t (*mbcm_dpp_pp_eg_encap_null_value_set_f) (
      int                           unit,
      uint32                       *value
  );

typedef soc_error_t (*mbcm_dpp_pp_eg_encap_null_value_get_f) (
      int                           unit,
      uint32                       *value
  );

typedef soc_error_t (*mbcm_pp_eg_vlan_edit_packet_is_tagged_set_f)(
    SOC_SAND_IN int                                unit,
    SOC_SAND_IN SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY   *eg_command_key,
    SOC_SAND_IN uint32                             is_tagged
    );

typedef soc_error_t (*mbcm_pp_eg_vlan_edit_packet_is_tagged_get_f)(
   SOC_SAND_IN int                                 unit,
   SOC_SAND_IN SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY    *eg_command_key,
   SOC_SAND_OUT uint32                             *is_tagged
   );

typedef soc_error_t (*mbcm_pp_eg_filter_default_port_membership_set_f)(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_PORT                      out_port_ndx,
    SOC_SAND_IN  uint8                             is_member
    );

typedef soc_error_t (*mbcm_pp_eg_filter_default_port_membership_get_f)(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_PORT                      out_port_ndx,
    SOC_SAND_OUT uint8                             *is_member
   );

typedef soc_error_t (*mbcm_pp_eg_encap_push_2_swap_init_f)(
    SOC_SAND_IN  int                               unit
   );


typedef uint32
(*mbcm_pp_eg_encap_header_compensation_per_cud_set_f)(int unit, 
                                                 int cud_msb, 
                                                 int value);

typedef uint32
(*mbcm_pp_eg_encap_header_compensation_per_cud_get_f)(int unit, 
                                                      int cud_msb, 
                                                      int *value);

   
   typedef uint32 (*mbcm_dpp_oam_oamp_protection_packet_header_raw_set_f)(
      SOC_SAND_IN  int                                    unit,
	  SOC_SAND_IN  SOC_PPC_OAM_OAMP_PROTECTION_HEADER     *packet_protection_header
  );
  
typedef uint32 (*mbcm_dpp_oam_oamp_protection_packet_header_raw_get_f)(
      int                                    unit,
	  SOC_PPC_OAM_OAMP_PROTECTION_HEADER     *packet_protection_header
  );

typedef uint32 (*mbcm_pp_frwrd_fec_entry_urpf_mode_set_f)(
      int                                    unit,
	  uint32                                 fec_ndx,
	  uint32                                 uc_rpf_mode
  );


typedef soc_error_t (*mbcm_pp_eg_encap_additional_label_profile_set_f) (
      int                           unit,
      SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO *additional_label_profile_info,
      int profile_index
  );

typedef soc_error_t (*mbcm_pp_eg_encap_additional_label_profile_get_f) (
      int                           unit,
      SOC_PPC_EG_ENCAP_ADDITIONAL_LABEL_PROFILE_INFO *additional_label_profile_info,
      int profile_index
  );

typedef soc_error_t
  (*mbcm_pp_eg_pmf_mirror_params_set_f)(
    int                          unit,
    uint32                       mirror_profile,
    dpp_outbound_mirror_config_t *config
  ) ;


typedef soc_error_t (*mbcm_pp_oam_oamp_rmep_db_ext_get_f)(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_RMEP_DB_EXT_ENTRY  *rmep_db_ext_entry
  ); 

typedef soc_error_t (*mbcm_pp_oam_oamp_sd_sf_profile_set_f)(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB      *sd_sf_profile_data
  );

typedef soc_error_t (*mbcm_pp_oam_oamp_sd_sf_profile_get_f)(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB     *sd_sf_profile_data
  );
  
typedef soc_error_t (*mbcm_pp_oam_oamp_sd_sf_1711_config_set_f)(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint8                                 d_excess_thresh,
    SOC_SAND_IN  uint8                                 clr_low_thresh,
    SOC_SAND_IN  uint8                                 clr_high_thresh,
    SOC_SAND_IN  uint8                                 num_entry
  );

typedef soc_error_t (*mbcm_pp_oam_oamp_sd_sf_1711_config_get_f)(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_INOUT  uint8                                 *d_excess_thresh,
    SOC_SAND_INOUT  uint8                                 *clr_low_thresh,
    SOC_SAND_INOUT  uint8                                 *clr_high_thresh,
    SOC_SAND_INOUT  uint8                                 *num_entry
  );
  
typedef soc_error_t (*mbcm_pp_oam_oamp_sd_sf_scanner_set_f)(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                              scan_index
  );
  
typedef soc_error_t (*mbcm_pp_oam_oamp_sd_sf_1711_db_set_f)(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint32                   y1711_sd_sf_id,
    SOC_SAND_IN  uint32                   sd_sf_1711_db_format,
    SOC_SAND_IN  uint8                    ccm_tx_rate,
    SOC_SAND_IN  uint8                    alert_method
  );

typedef soc_error_t (*mbcm_pp_oam_oamp_sd_sf_1711_db_get_f)(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   y1711_sd_sf_id,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_SD_SF_Y_1711_DB_ENTRY  *sd_sf_1711_entry
  );
  
typedef soc_error_t (*mbcm_pp_oam_oamp_sd_sf_db_set_f)(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  uint32                   sd_sf_db_index,
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY       *sd_sf_entry
  );

typedef int (*mbcm_pp_oam_oamp_sd_sf_db_get_f)(
  SOC_SAND_IN  int                   unit,
  SOC_SAND_IN  uint32                 sd_sf_db_index,    
    SOC_PPC_OAM_OAMP_SD_SF_DB_ENTRY  *sd_sf_entry
);
typedef soc_error_t (*mbcm_pp_oam_oamp_1711_pe_profile_update_f)(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN SOC_PPC_OAM_MEP_TYPE  mep_type,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  uint32                   cnt_enable
);

typedef soc_error_t (*mbcm_pp_oam_classifier1_mip_passive_entries_f)( int, const  SOC_PPC_OAM_LIF_PROFILE_DATA*);

typedef int (*mbcm_pp_soc_arad_oamp_cpu_port_dp_tc_set_f)(int unit,uint32 dp, uint32 tc);
typedef int (*mbcm_pp_soc_arad_oamp_tmx_arb_weight_set_f)(int unit,uint32 txm_arbiter_weight);
typedef int (*mbcm_pp_soc_jer_oamp_sat_arb_weight_set_f)(int unit,uint32 sat_arbiter_weight);
typedef int (*mbcm_pp_soc_arad_oamp_response_weight_set_f)(int unit,uint32 rsp_arbiter_weight);
typedef int (*mbcm_pp_set_mep_data_in_gen_mem_f)(int unit,uint32 mep_id, uint16 data_1,uint16 data_2);
typedef int (*mbcm_pp_oam_classifier_oem_mep_mip_conflict_check_f)(int unit, uint32 oam_lif, uint8 is_mip);
typedef void (*mbcm_pp_oamp_pe_program_profile_get_t)(int unit, ARAD_PP_OAMP_PE_PROGRAMS program_id, uint32 *program_profile);
typedef uint32 (*mbcm_pp_arad_pp_oam_dma_clear_f)(int unit);
typedef uint32 (*mbcm_pp_arad_pp_oam_dma_set_f)(int unit);
typedef int (*mbcm_pp_soc_qax_diag_oamp_counter_set_f)(int unit, uint8 type, uint8 filter, uint16 value);
typedef int (*mbcm_pp_soc_qax_diag_oamp_counter_get_f)(int unit, uint8 type, uint8 *filter, uint16 *value, uint32 *counter_val);
typedef int (*mbcm_pp_get_mep_data_in_gen_mem_f)(int unit,uint32 mep_id, uint32 *data_1,uint32 *data_2);
typedef soc_error_t (*mbcm_pp_soc_jer_oamp_rfc6374_lm_control_code_set_f)(int unit, uint8 is_response, uint32 rfc6374_lm_control_code);
typedef soc_error_t (*mbcm_pp_soc_jer_oamp_rfc6374_lm_control_code_get_f)(int unit, uint8 is_response, uint32 *rfc6374_lm_control_code);
typedef soc_error_t (*mbcm_pp_soc_jer_oamp_rfc6374_dm_control_code_set_f)(int unit, uint8 is_response, uint32 rfc6374_dm_control_code);
typedef soc_error_t (*mbcm_pp_soc_jer_oamp_rfc6374_dm_control_code_get_f)(int unit, uint8 is_response, uint32 *rfc6374_dm_control_code);
typedef soc_error_t (*mbcm_pp_soc_jer_oamp_rfc6374_lm_dm_config_f)(int unit, uint8 is_ilm, uint8 timestamp_format);
typedef soc_error_t (*mbcm_pp_soc_jer_oamp_rfc6374_rx_trap_codes_set_f)(int unit, uint8 is_rfc6374_lm, uint8 is_rfc6374_dm, uint32 trap_code);
typedef soc_error_t (*mbcm_pp_soc_jer_oamp_rfc6374_rx_filter_table_set_f)(int unit, int control_code, uint8 is_response, uint32 punt_2_ignore_1_pass_0);
typedef soc_error_t (*mbcm_pp_soc_jer_oamp_rfc6374_rx_filter_table_get_f)(int unit, int control_code, uint8 is_response, uint32 * punt_2_ignore_1_pass_0);

typedef uint32 (*mbcm_pp_lif_ing_vlan_edit_pcp_map_dp_set_f)(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dp_profile_ndx,
    SOC_SAND_IN  uint32                                   dp_ndx,
    SOC_SAND_IN  uint32                                sel_pcp_dei,
    SOC_SAND_IN  uint8                                   out_pcp,
    SOC_SAND_IN  uint8                                 out_dei
  );

typedef uint32 (*mbcm_pp_lif_ing_vlan_edit_pcp_map_dp_get_f)(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dp_profile_ndx,
    SOC_SAND_IN  uint32                                   dp_ndx,
    SOC_SAND_IN  uint32                                 sel_pcp_dei,
    SOC_SAND_OUT uint8                                   *out_pcp,
    SOC_SAND_OUT uint8                                 *out_dei
  );

typedef uint32 (*mbcm_pp_lif_ing_vlan_edit_inner_global_info_set_f)(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_IN int value
  );

typedef uint32 (*mbcm_pp_lif_ing_vlan_edit_outer_global_info_set_f)(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_IN int value
  );

typedef uint32 (*mbcm_pp_lif_ing_vlan_edit_inner_global_info_get_f)(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_OUT int *value
  );

typedef uint32 (*mbcm_pp_lif_ing_vlan_edit_outer_global_info_get_f)(
    SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 in_lif_profile,
   SOC_SAND_OUT int *value
  );

typedef soc_error_t (*mbcm_pp_lif_default_native_ac_outlif_set_f)(
  SOC_SAND_IN  int                   unit,
  SOC_SAND_IN  uint32                local_out_lif_id
  ); 

typedef soc_error_t (*mbcm_pp_lif_default_outlif_get_f)(
  SOC_SAND_IN  int                   unit,
  SOC_SAND_OUT  uint32*                local_out_lif_id
  ); 


typedef soc_error_t (*mbcm__pp_eg_encap_access_default_ac_entry_vlan_edit_profile_set_f) (
     SOC_SAND_IN  int     unit, 
     SOC_SAND_OUT uint32  vlan_edit_profile
     );

typedef soc_error_t (*mbcm__pp_eg_encap_access_default_ac_entry_vlan_edit_profile_get_f) (
     SOC_SAND_IN  int      unit, 
     SOC_SAND_OUT uint32*  vlan_edit_profile
     );

typedef uint32 (*mbcm_pp_port_eg_ttl_inheritance_set_f)(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint64 outlif_profiles
  );

typedef uint32 (*mbcm_pp_port_eg_ttl_inheritance_get_f)(
    SOC_SAND_IN int unit,
    SOC_SAND_OUT uint64 *outlif_profiles
  );

typedef uint32 (*mbcm_pp_port_eg_qos_inheritance_set_f)(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint64 outlif_profiles
  );

typedef uint32 (*mbcm_pp_port_eg_qos_inheritance_get_f)(
    SOC_SAND_IN int unit,
    SOC_SAND_OUT uint64 *outlif_profiles
  );

typedef uint32 (*mbcm_pp_meter_tc_map_set_f) (
     SOC_SAND_IN  int      unit, 
     SOC_SAND_IN  int      tc,
     SOC_SAND_OUT uint32      meter_tc
     );

typedef uint32 (*mbcm_pp_meter_tc_map_get_f) (
     SOC_SAND_IN  int      unit, 
     SOC_SAND_IN  int      tc,
     SOC_SAND_OUT uint32      *meter_tc
     );



typedef uint32 (*mbcm_pp_port_eg_qos_marking_set_f) (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN bcm_port_t port,
    SOC_SAND_IN int enable
  );

typedef uint32 (*mbcm_pp_port_eg_qos_marking_get_f) (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN bcm_port_t port,
    SOC_SAND_OUT int* enable
  );

typedef uint32 (*mbcm_pp_oam_eth_oam_opcode_map_set_unsafe_f)(int);
  
typedef soc_error_t (*mbcm_pp_oam_classifier_find_ep_id_by_lif_dir_and_mdl_f)(
                 int unit,
                 uint32 lif,
                 uint8  mdl,
                 uint8  is_upmep,
                 uint8  *is_mip,
                 uint8  *found,
                 int    *oam_id
               );

typedef soc_error_t (*mbcm_pp_pppoe_anti_spoofing_add_f) (SOC_SAND_IN int, SOC_SAND_INOUT SOC_PPC_SRC_BIND_PPPOE_ENTRY*, SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE*); 
typedef soc_error_t (*mbcm_pp_pppoe_anti_spoofing_delete_f) (SOC_SAND_IN int, SOC_SAND_INOUT SOC_PPC_SRC_BIND_PPPOE_ENTRY*, SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE*);
typedef soc_error_t (*mbcm_pp_pppoe_anti_spoofing_get_f) (SOC_SAND_IN int, SOC_SAND_INOUT SOC_PPC_SRC_BIND_PPPOE_ENTRY*, SOC_SAND_OUT uint8*);

typedef uint32 (*mbcm_pp_eg_qos_ether_dscp_remark_set_f)(SOC_SAND_IN int, SOC_SAND_IN int);
typedef uint32 (*mbcm_pp_eg_qos_ether_dscp_remark_get_f)(SOC_SAND_IN int, SOC_SAND_OUT int*);


typedef soc_error_t (*mbcm_pp_bfd_oamp_udp_src_port_check_enable_f)(SOC_SAND_IN int, SOC_SAND_IN uint8);


 typedef  uint32
(*mbcm_pp_seamless_bfd_rx_dst_port_set_f)(
        SOC_SAND_IN  int                                       unit,
        SOC_SAND_IN  uint32                                    flags,
        SOC_SAND_IN  uint32                                    udp_port
        );
 typedef  uint32
(*mbcm_pp_seamless_bfd_rx_dst_port_get_f)(
        SOC_SAND_IN  int                                       unit,
        SOC_SAND_OUT  uint32                                   *udp_port
        );



typedef uint32
    (*mbcm_pp_oamp_pe_seamless_bfd_src_port_set_f)(
        SOC_SAND_IN   int                                 unit,
        SOC_SAND_OUT  uint32                              src_port);
typedef uint32
    (*mbcm_pp_oamp_pe_seamless_bfd_src_port_get_f)(
        SOC_SAND_IN   int                                 unit,
        SOC_SAND_OUT  uint32                             *src_port);





typedef struct mbcm_pp_functions_s {
	
	
	

	
	mbcm_dpp_oam_oamp_protection_packet_header_raw_get_f   	mbcm_dpp_oam_oamp_protection_packet_header_raw_get;
	mbcm_dpp_oam_oamp_protection_packet_header_raw_set_f   	mbcm_dpp_oam_oamp_protection_packet_header_raw_set;
	mbcm_pp_oam_two_ints_one_uint8_star      	mbcm_pp_oam_1dm_get;
	mbcm_pp_oam_two_ints_one_uint8        	mbcm_pp_oam_1dm_set;
	mbcm_pp_oam_bfd_acc_endpoint_tx_disable_f      	mbcm_pp_oam_bfd_acc_endpoint_tx_disable;
	mbcm_pp_oam_classifier_default_profile_add_f   	mbcm_pp_oam_classifier_default_profile_add;
	mbcm_pp_oam_classifier_default_profile_remove_f	mbcm_pp_oam_classifier_default_profile_remove;
	mbcm_pp_oam_classifier_oem1_entry_set_unsafe_f   	mbcm_pp_oam_classifier_oem1_entry_set_unsafe;
	mbcm_pp_oam_classifier_oem1_init_f               	mbcm_pp_oam_classifier_oem1_init;
	mbcm_pp_oam_counter_increment_bitmap           	mbcm_pp_oam_counter_increment_bitmap_set;
	mbcm_pp_oam_two_ints                              	mbcm_pp_oam_dm_trigger_set;
	mbcm_pp_oam_one_int_three_uint8s          	mbcm_pp_oam_egress_pcp_set_by_profile_and_tc;
	mbcm_pp_oam_inlif_profile_map_get_f            	mbcm_pp_oam_inlif_profile_map_get;
	mbcm_pp_oam_inlif_profile_map_set_f            	mbcm_pp_oam_inlif_profile_map_set;
	mbcm_pp_oam_my_cfm_mac_delete_f                	mbcm_pp_oam_my_cfm_mac_delete;
	mbcm_pp_oam_my_cfm_mac_get_f                   	mbcm_pp_oam_my_cfm_mac_get;
	mbcm_pp_oam_my_cfm_mac_set_f                   	mbcm_pp_oam_my_cfm_mac_set;
	mbcm_pp_oam_one_int_two_int_stars        	mbcm_pp_oam_oam_bfd_discriminator_rx_range_get;
	mbcm_pp_oam_three_ints               	mbcm_pp_oam_oam_bfd_discriminator_rx_range_set;
	mbcm_pp_oam_oamp_dm_get_f                        	mbcm_pp_oam_oamp_dm_get;
	mbcm_pp_oam_three_ints            	mbcm_pp_oam_oamp_gen_mem_set;
	mbcm_pp_oam_oamp_gen_mem_get_f      mbcm_pp_oam_oamp_gen_mem_get;
	mbcm_pp_oam_loopback_tst_info_init_f                mbcm_pp_oam_loopback_tst_info_init;
	mbcm_pp_oam_oamp_lb_tst_header_set_f             	mbcm_pp_oam_oamp_lb_tst_header_set;
	mbcm_pp_oam_oamp_lm_dm_pointed_f                 	mbcm_pp_oam_oamp_lm_dm_delete;
	mbcm_pp_oam_oamp_lm_dm_pointed_f                 	mbcm_pp_oam_oamp_lm_dm_pointed_find;
	mbcm_pp_oam_oamp_lm_dm_pointed_f                 	mbcm_pp_oam_oamp_lm_dm_set;
	mbcm_pp_oam_oamp_lm_get_f                        	mbcm_pp_oam_oamp_lm_get;
	mbcm_pp_oam_oamp_loopback_get_f                 	mbcm_pp_oam_oamp_loopback_get;
	mbcm_pp_oam_oamp_loopback_set_f                 	mbcm_pp_oam_oamp_loopback_set;
	mbcm_pp_oam_oamp_punt_event_hendling_profile_get_f 	mbcm_pp_oam_oamp_punt_event_hendling_profile_get;
	mbcm_pp_oam_oamp_punt_event_hendling_profile_set_f 	mbcm_pp_oam_oamp_punt_event_hendling_profile_set;
	mbcm_pp_oam_oamp_rx_trap_code                    	mbcm_pp_oam_oamp_rx_trap_codes_delete;
	mbcm_pp_oam_oamp_rx_trap_code                    	mbcm_pp_oam_oamp_rx_trap_codes_set;
	mbcm_pp_oam_oamp_search_for_lm_dm_f              	mbcm_pp_oam_oamp_search_for_lm_dm;
	mbcm_pp_oam_two_ints_one_uint8_star            	mbcm_pp_oam_sa_addr_msbs_get;
	mbcm_pp_oam_two_ints_one_const_uint8_star      	mbcm_pp_oam_sa_addr_msbs_set;
	mbcm_pp_oam_two_ints                                  	mbcm_pp_oam_slm_set;
	mbcm_pp_oam_tod_set_f                          	mbcm_pp_oam_tod_set;
	mbcm_pp_oam_tod_get_f                          	mbcm_pp_oam_tod_get;
	mbcm_pp_oam_tod_leap_control_set_f              mbcm_pp_oam_tod_leap_control_set;
	mbcm_pp_oam_tod_leap_control_get_f              mbcm_pp_oam_tod_leap_control_get;
	mbcm_pp_oamp_pe_use_1dm_check_f                	mbcm_pp_oamp_pe_use_1dm_check;
	mbcm_pp_oamp_report_mode_get_f                 	mbcm_pp_oamp_report_mode_get;
	mbcm_pp_oamp_report_mode_set_f                 	mbcm_pp_oamp_report_mode_set;
	mbcm_pp_flp_dbal_bfd_echo_program_tables_init_f      	mbcm_pp_flp_dbal_bfd_echo_program_tables_init;
	mbcm_pp_bfd_one_hop_lem_entry_add_f            	mbcm_pp_lem_access_bfd_one_hop_lem_entry_add;
	mbcm_pp_bfd_one_hop_lem_entry_remove_f 	mbcm_pp_lem_access_bfd_one_hop_lem_entry_remove;
	mbcm_pp_oam_oamp_rmep_db_ext_get_f                  mbcm_pp_oam_oamp_rmep_db_ext_get;
	mbcm_pp_oam_oamp_sd_sf_profile_set_f                mbcm_pp_oam_oamp_sd_sf_profile_set;
	mbcm_pp_oam_oamp_sd_sf_profile_get_f                mbcm_pp_oam_oamp_sd_sf_profile_get;
	mbcm_pp_oam_oamp_sd_sf_1711_config_set_f            mbcm_pp_oam_oamp_sd_sf_1711_config_set;
	mbcm_pp_oam_oamp_sd_sf_1711_config_get_f            mbcm_pp_oam_oamp_sd_sf_1711_config_get;
	mbcm_pp_oam_oamp_sd_sf_scanner_set_f                mbcm_pp_oam_oamp_sd_sf_scanner_set;
	mbcm_pp_oam_oamp_sd_sf_1711_db_set_f                mbcm_pp_oam_oamp_sd_sf_1711_db_set;
	mbcm_pp_oam_oamp_sd_sf_1711_db_get_f                mbcm_pp_oam_oamp_sd_sf_1711_db_get;
	mbcm_pp_oam_oamp_sd_sf_db_set_f                     mbcm_pp_oam_oamp_sd_sf_db_set;
	mbcm_pp_oam_oamp_sd_sf_db_get_f                     mbcm_pp_oam_oamp_sd_sf_db_get;
	mbcm_pp_oam_oamp_1711_pe_profile_update_f           mbcm_pp_oam_oamp_1711_pe_profile_update;
	mbcm_pp_oam_classifier1_mip_passive_entries_f       mbcm_pp_oam_classifier1_mip_passive_entries;
    mbcm_pp_oam_bfd_flexible_verification_set_f         mbcm_pp_oam_bfd_flexible_verification_set;
    mbcm_pp_oam_bfd_flexible_verification_get_f         mbcm_pp_oam_bfd_flexible_verification_get;
    mbcm_pp_oam_bfd_flexible_verification_delete_f      mbcm_pp_oam_bfd_flexible_verification_delete;
    mbcm_pp_oam_bfd_mep_db_ext_data_set_f               mbcm_pp_oam_bfd_mep_db_ext_data_set;
    mbcm_pp_oam_bfd_mep_db_ext_data_get_f               mbcm_pp_oam_bfd_mep_db_ext_data_get;
    mbcm_pp_oam_bfd_mep_db_ext_data_delete_f            mbcm_pp_oam_bfd_mep_db_ext_data_delete;
    mbcm_pp_oamp_control_lm_pcp_counter_range_min_get_f mbcm_pp_oamp_control_lm_pcp_counter_range_min_get;
    mbcm_pp_oamp_control_lm_pcp_counter_range_max_get_f mbcm_pp_oamp_control_lm_pcp_counter_range_max_get;
    mbcm_pp_oamp_control_punt_packet_int_pri_get_f      mbcm_pp_oamp_control_punt_packet_int_pri_get;
    mbcm_pp_oamp_control_ccm_weight_get_f               mbcm_pp_oamp_control_ccm_weight_get;
    mbcm_pp_oamp_control_sat_weight_get_f               mbcm_pp_oamp_control_sat_weight_get;
    mbcm_pp_oamp_control_response_weight_get_f          mbcm_pp_oamp_control_response_weight_get;
    mbcm_pp_soc_arad_oamp_cpu_port_dp_tc_set_f          mbcm_pp_soc_arad_oamp_cpu_port_dp_tc_set;
    mbcm_pp_soc_arad_oamp_tmx_arb_weight_set_f          mbcm_pp_soc_arad_oamp_tmx_arb_weight_set;
    mbcm_pp_soc_jer_oamp_sat_arb_weight_set_f           mbcm_pp_soc_jer_oamp_sat_arb_weight_set;
    mbcm_pp_soc_arad_oamp_response_weight_set_f         mbcm_pp_soc_arad_oamp_response_weight_set;
    mbcm_pp_oam_eth_oam_opcode_map_set_unsafe_f         mbcm_pp_oam_eth_oam_opcode_map_set_unsafe;
    mbcm_pp_oam_classifier_oem_mep_mip_conflict_check_f mbcm_pp_oam_classifier_oem_mep_mip_conflict_check;
	mbcm_pp_set_mep_data_in_gen_mem_f                    mbcm_pp_set_mep_data_in_gen_mem;
    mbcm_pp_oamp_pe_program_profile_get_t               mbcm_pp_oamp_pe_program_profile_get;
	        mbcm_pp_arad_pp_oam_dma_clear_f                     mbcm_pp_arad_pp_oam_dma_clear;
	        mbcm_pp_arad_pp_oam_dma_set_f                     mbcm_pp_arad_pp_oam_dma_set;
    mbcm_pp_soc_qax_diag_oamp_counter_set_f                 mbcm_pp_soc_qax_diag_oamp_counter_set;
    mbcm_pp_soc_qax_diag_oamp_counter_get_f                 mbcm_pp_soc_qax_diag_oamp_counter_get;
    mbcm_pp_get_mep_data_in_gen_mem_f                    mbcm_pp_get_mep_data_in_gen_mem;
    mbcm_pp_oam_classifier_find_ep_id_by_lif_dir_and_mdl_f mbcm_pp_oam_classifier_find_ep_id_by_lif_dir_and_mdl;
    mbcm_pp_soc_jer_oamp_rfc6374_lm_control_code_set_f  mbcm_pp_soc_jer_oamp_rfc6374_lm_control_code_set;
    mbcm_pp_soc_jer_oamp_rfc6374_lm_control_code_get_f  mbcm_pp_soc_jer_oamp_rfc6374_lm_control_code_get;
    mbcm_pp_soc_jer_oamp_rfc6374_dm_control_code_set_f  mbcm_pp_soc_jer_oamp_rfc6374_dm_control_code_set;
    mbcm_pp_soc_jer_oamp_rfc6374_dm_control_code_get_f  mbcm_pp_soc_jer_oamp_rfc6374_dm_control_code_get;
    mbcm_pp_soc_jer_oamp_rfc6374_lm_dm_config_f  mbcm_pp_soc_jer_oamp_rfc6374_lm_dm_config;
    mbcm_pp_soc_jer_oamp_rfc6374_rx_trap_codes_set_f mbcm_pp_soc_jer_oamp_rfc6374_rx_trap_codes_set;
    mbcm_pp_soc_jer_oamp_rfc6374_rx_filter_table_set_f mbcm_pp_soc_jer_oamp_rfc6374_rx_filter_table_set;
    mbcm_pp_soc_jer_oamp_rfc6374_rx_filter_table_get_f mbcm_pp_soc_jer_oamp_rfc6374_rx_filter_table_get;
    
	
	mbcm_dpp_pp_eg_encap_null_value_get_f            	mbcm_dpp_pp_eg_encap_null_value_get;
	mbcm_dpp_pp_eg_encap_null_value_set_f            	mbcm_dpp_pp_eg_encap_null_value_set;
	mbcm_pp_eg_encap_access_clear_bank_f                	mbcm_pp_encap_access_clear_bank;
	mbcm_pp_eg_encap_direct_bank_set_f             	mbcm_pp_eg_encap_direct_bank_set;
	mbcm_pp_eg_encap_ether_type_index_clear_f      	mbcm_pp_eg_encap_ether_type_index_clear;
	mbcm_pp_eg_encap_ether_type_index_get_f        	mbcm_pp_eg_encap_ether_type_index_get;
	mbcm_pp_eg_encap_ether_type_index_set_f        	mbcm_pp_eg_encap_ether_type_index_set;
	mbcm_pp_eg_encap_extension_mapping_get_f       	mbcm_pp_eg_encap_extension_mapping_get;
	mbcm_pp_eg_encap_extension_mapping_set_f       	mbcm_pp_eg_encap_extension_mapping_set;
	mbcm_pp_eg_encap_extension_type_get_f          	mbcm_pp_eg_encap_extension_type_get;
	mbcm_pp_eg_encap_extension_type_set_f          	mbcm_pp_eg_encap_extension_type_set;
	mbcm_pp_eg_encap_ip_tunnel_size_protocol_template_set_f 	mbcm_pp_eg_encap_ip_tunnel_size_protocol_template_set;
	mbcm_pp_eg_encap_map_encap_intpri_color_get_f        	mbcm_pp_eg_encap_map_encap_intpri_color_get;
	mbcm_pp_eg_encap_map_encap_intpri_color_set_f        	mbcm_pp_eg_encap_map_encap_intpri_color_set;
	mbcm_pp_eg_encap_push_2_swap_init_f              	mbcm_pp_eg_encap_push_2_swap_init;
    mbcm_pp_eg_encap_header_compensation_per_cud_set_f mbcm_pp_eg_encap_header_compensation_per_cud_set;
    mbcm_pp_eg_encap_header_compensation_per_cud_get_f mbcm_pp_eg_encap_header_compensation_per_cud_get;
	
	mbcm_pp_frwrd_fec_entry_urpf_mode_set_f          	mbcm_pp_frwrd_fec_entry_urpf_mode_set;
	mbcm_pp_frwrd_fcf_npv_switch_get_f             	mbcm_pp_frwrd_fcf_npv_switch_get;
	mbcm_pp_frwrd_fcf_npv_switch_set_f             	mbcm_pp_frwrd_fcf_npv_switch_set;
	mbcm_pp_frwrd_fcf_vsan_mode_get_f              	mbcm_pp_frwrd_fcf_vsan_mode_get;
	mbcm_pp_frwrd_fcf_vsan_mode_set_f              	mbcm_pp_frwrd_fcf_vsan_mode_set;
	mbcm_pp_frwrd_fec_is_protected_get_f             	mbcm_pp_frwrd_fec_is_protected_get;
	mbcm_pp_frwrd_extend_p2p_lem_entry_add_f             	mbcm_pp_frwrd_extend_p2p_lem_entry_add;
	mbcm_pp_frwrd_extend_p2p_lem_entry_remove_f             	mbcm_pp_frwrd_extend_p2p_lem_entry_remove;
	mbcm_pp_frwrd_extend_p2p_lem_entry_get_f             	mbcm_pp_frwrd_extend_p2p_lem_entry_get;
	
	mbcm_pp_aging_num_of_cycles_get_f               	mbcm_pp_aging_num_of_cycles_get;
	mbcm_pp_frwrd_mact_clear_access_bit_f 	mbcm_pp_frwrd_mact_clear_access_bit;
	mbcm_pp_frwrd_mact_cpu_counter_learn_limit_get_f 	mbcm_pp_frwrd_mact_cpu_counter_learn_limit_get;
	mbcm_pp_frwrd_mact_cpu_counter_learn_limit_set_f 	mbcm_pp_frwrd_mact_cpu_counter_learn_limit_set;
	mbcm_pp_frwrd_mact_event_handle_info_set_dma_f 	mbcm_pp_frwrd_mact_event_handle_info_set_dma;
	mbcm_pp_frwrd_mact_event_handler_callback_register_f 	mbcm_pp_frwrd_mact_event_handler_callback_register;
	mbcm_pp_frwrd_mact_learning_dma_set_f 	mbcm_pp_frwrd_mact_learning_dma_set;
	mbcm_pp_frwrd_mact_learning_dma_unset_f 	mbcm_pp_frwrd_mact_learning_dma_unset;
	mbcm_pp_frwrd_mact_opport_mode_get_f 	mbcm_pp_frwrd_mact_opport_mode_get;
	mbcm_pp_frwrd_mact_transplant_static_get_f 	mbcm_pp_frwrd_mact_transplant_static_get;
	mbcm_pp_frwrd_mact_transplant_static_set_f 	mbcm_pp_frwrd_mact_transplant_static_set;
	
	mbcm_pp_occ_mgmt_app_get_f                     	mbcm_pp_occ_mgmt_app_get;
	mbcm_pp_occ_mgmt_app_set_f                     	mbcm_pp_occ_mgmt_app_set;
	mbcm_pp_occ_mgmt_deinit_f                      	mbcm_pp_occ_mgmt_deinit;
	mbcm_pp_occ_mgmt_get_app_mask_f                	mbcm_pp_occ_mgmt_get_app_mask;
	mbcm_pp_occ_mgmt_init_f                        	mbcm_pp_occ_mgmt_init;
	
	mbcm_pp_l2_lif_extender_get_f                    	mbcm_pp_l2_lif_extender_get;
	mbcm_pp_l2_lif_extender_remove_f                 	mbcm_pp_l2_lif_extender_remove;
	mbcm_pp_extender_deinit_f                      	mbcm_pp_extender_deinit;
	mbcm_pp_extender_eve_etag_format_get_f           	mbcm_pp_extender_eve_etag_format_get;
	mbcm_pp_extender_eve_etag_format_set_f           	mbcm_pp_extender_eve_etag_format_set;
	mbcm_pp_extender_global_etag_ethertype_get_f            	mbcm_pp_extender_global_etag_ethertype_get;
	mbcm_pp_extender_global_etag_ethertype_set_f            	mbcm_pp_extender_global_etag_ethertype_set;
	mbcm_pp_extender_init_f                        	mbcm_pp_extender_init;
	mbcm_pp_extender_port_info_get_f               	mbcm_pp_extender_port_info_get;
	mbcm_pp_extender_port_info_set_f               	mbcm_pp_extender_port_info_set;
	
	mbcm_pp_ipmc_ssm_entry_add_f                     	mbcm_pp_ipmc_ssm_entry_add;
	mbcm_pp_ipmc_ssm_entry_delete_f                  	mbcm_pp_ipmc_ssm_entry_delete;
	mbcm_pp_ipmc_ssm_entry_get_f                     	mbcm_pp_ipmc_ssm_entry_get;
    mbcm_pp_ipmc_ssm_entry_clear_f                     	mbcm_pp_ipmc_ssm_entry_clear;
	mbcm_pp_ipmc_ssm_tcam_entry_add_f              	mbcm_pp_ipmc_ssm_tcam_entry_add;
	mbcm_pp_ipmc_ssm_tcam_entry_delete_f           	mbcm_pp_ipmc_ssm_tcam_entry_delete;
	mbcm_pp_ipmc_ssm_tcam_entry_get_f              	mbcm_pp_ipmc_ssm_tcam_entry_get;
	
	mbcm_pp_mpls_termination_range_action_get_f    	mbcm_pp_mpls_termination_range_action_get;
	mbcm_pp_mpls_termination_range_action_set_f    	mbcm_pp_mpls_termination_range_action_set;
	mbcm_pp_mpls_termination_range_profile_get_f   	mbcm_pp_mpls_termination_range_profile_get;
	mbcm_pp_mpls_termination_range_profile_set_f   	mbcm_pp_mpls_termination_range_profile_set;
	mbcm_pp_mpls_termination_spacial_labels_init_f 	mbcm_pp_mpls_termination_spacial_labels_init;
	mbcm_pp_mpls_termination_l4b_l5l_init_f 	    mbcm_pp_mpls_termination_l4b_l5l_init;
    mbcm_pp_mpls_termination_vccv_type_ttl1_oam_classification_set_f mbcm_pp_mpls_termination_vccv_type_ttl1_oam_classification_set;
    mbcm_pp_mpls_termination_vccv_type_ttl1_oam_classification_get_f mbcm_pp_mpls_termination_vccv_type_ttl1_oam_classification_get;
    mbcm_pp_mpls_termination_first_label_bit_map_set_f mbcm_pp_mpls_termination_first_label_bit_map_set;
    mbcm_pp_mpls_termination_first_label_bit_map_get_f mbcm_pp_mpls_termination_first_label_bit_map_get;
    mbcm_pp_mpls_termination_first_label_bit_map_init_f mbcm_pp_mpls_termination_first_label_bit_map_init;

	
	mbcm_pp_mymac_protocol_group_get_group_by_protocols_f 	mbcm_pp_mymac_protocol_group_get_group_by_protocols;
	mbcm_pp_mymac_protocol_group_get_protocol_by_group_f 	mbcm_pp_mymac_protocol_group_get_protocol_by_group;
	mbcm_pp_mymac_protocol_group_set_f 	mbcm_pp_mymac_protocol_group_set;
	mbcm_pp_mymac_vrid_mymac_map_set_to_all_vsi_f  	mbcm_pp_mymac_vrid_mymac_map_set_to_all_vsi;
	mbcm_pp_mymac_vrrp_cam_info_delete_f           	mbcm_pp_mymac_vrrp_cam_info_delete;
	mbcm_pp_mymac_vrrp_cam_info_get_f              	mbcm_pp_mymac_vrrp_cam_info_get;
	mbcm_pp_mymac_vrrp_cam_info_set_f              	mbcm_pp_mymac_vrrp_cam_info_set;
	
	mbcm_pp_mtr_policer_global_sharing_get_f	mbcm_pp_mtr_policer_global_sharing_get;
	mbcm_pp_mtr_policer_global_sharing_set_f	mbcm_pp_mtr_policer_global_sharing_set;
	mbcm_pp_mtr_policer_global_mef_10_3_get_f	mbcm_pp_mtr_policer_global_mef_10_3_get;
	mbcm_pp_mtr_policer_global_mef_10_3_set_f	mbcm_pp_mtr_policer_global_mef_10_3_set;
	mbcm_pp_mtr_none_ethernet_packet_ptr_get_f	mbcm_pp_mtr_none_ethernet_packet_ptr_get;
	mbcm_pp_mtr_none_ethernet_packet_ptr_set_f	mbcm_pp_mtr_none_ethernet_packet_ptr_set;	
	mbcm_pp_mtr_policer_hdr_compensation_get_f       	mbcm_pp_mtr_policer_hdr_compensation_get;
	mbcm_pp_mtr_policer_hdr_compensation_set_f       	mbcm_pp_mtr_policer_hdr_compensation_set;
	mbcm_pp_mtr_policer_ipg_compensation_get_f	mbcm_pp_mtr_policer_ipg_compensation_get;
	mbcm_pp_mtr_policer_ipg_compensation_set_f	mbcm_pp_mtr_policer_ipg_compensation_set;
	mbcm_pp_mtr_eth_policer_glbl_profile_get_f	mbcm_pp_mtr_eth_policer_glbl_profile_get;
	mbcm_pp_mtr_eth_policer_glbl_profile_map_get_f	mbcm_pp_mtr_eth_policer_glbl_profile_map_get;
	mbcm_pp_mtr_eth_policer_glbl_profile_map_set_f	mbcm_pp_mtr_eth_policer_glbl_profile_map_set;
	mbcm_pp_mtr_eth_policer_glbl_profile_set_f	mbcm_pp_mtr_eth_policer_glbl_profile_set;
	mbcm_pp_mtr_eth_policer_params_get_f	mbcm_pp_mtr_eth_policer_params_get;
	mbcm_pp_mtr_eth_policer_params_set_f	mbcm_pp_mtr_eth_policer_params_set;
	
	mbcm_pp_network_group_config_get_f             	mbcm_pp_network_group_config_get;
	mbcm_pp_network_group_config_set_f             	mbcm_pp_network_group_config_set;
	mbcm_pp_lif_additional_data_get_f              	mbcm_pp_lif_additional_data_get;
	mbcm_pp_lif_additional_data_set_f              	mbcm_pp_lif_additional_data_set;
	mbcm_pp_lif_glem_access_entry_add_f            	mbcm_pp_lif_glem_access_entry_add;
	mbcm_pp_lif_glem_access_entry_by_key_get_f     	mbcm_pp_lif_glem_access_entry_by_key_get;
	mbcm_pp_lif_glem_access_entry_remove_f         	mbcm_pp_lif_glem_access_entry_remove;
	mbcm_pp_lif_is_wide_entry_f                    	mbcm_pp_lif_is_wide_entry;
	
	mbcm_pp_ip6_compression_add_f                  	mbcm_pp_ip6_compression_add;
	mbcm_pp_ip6_compression_delete_f               	mbcm_pp_ip6_compression_delete;
	mbcm_pp_ip6_compression_get_f                  	mbcm_pp_ip6_compression_get;
	
	mbcm_pp_mtr_meter_ins_bucket_get_f	mbcm_pp_mtr_meter_ins_bucket_get;
	mbcm_pp_metering_dp_map_entry_get_f              	mbcm_pp_metering_dp_map_entry_get;
	mbcm_pp_metering_dp_map_entry_set_f              	mbcm_pp_metering_dp_map_entry_set;
	mbcm_pp_metering_pcd_entry_get_f                 	mbcm_pp_metering_pcd_entry_get;
	mbcm_pp_metering_pcd_entry_set_f                 	mbcm_pp_metering_pcd_entry_set;
	
	mbcm_pp_port_additional_tpids_get_f            	mbcm_pp_port_additional_tpids_get;
	mbcm_pp_port_additional_tpids_set_f            	mbcm_pp_port_additional_tpids_set;
	mbcm_pp_port_property_get_f                    	mbcm_pp_port_property_get;
	mbcm_pp_port_property_set_f                    	mbcm_pp_port_property_set;
	
	mbcm_pp_diag_glem_signals_get_f                  	mbcm_pp_diag_glem_signals_get;
	mbcm_pp_diag_kaps_lkup_info_get_f              	mbcm_pp_diag_kaps_lkup_info_get;
	
	mbcm_pp_eg_etpp_trap_get_f                       	mbcm_pp_eg_etpp_trap_get;
	mbcm_pp_eg_etpp_trap_set_f                       	mbcm_pp_eg_etpp_trap_set;
    mbcm_pp_eg_pmf_mirror_params_set_f              mbcm_pp_eg_pmf_mirror_params_set;
    mbcm_pp_eg_etpp_out_lif_mtu_map_set_f           mbcm_pp_eg_etpp_out_lif_mtu_map_set;
    mbcm_pp_eg_etpp_out_lif_mtu_map_get_f           mbcm_pp_eg_etpp_out_lif_mtu_map_get;
    mbcm_pp_eg_etpp_out_lif_mtu_check_set_f         mbcm_pp_eg_etpp_out_lif_mtu_check_set;
    mbcm_pp_trap_ingress_ser_set_f                  mbcm_pp_trap_ingress_ser_set;
    mbcm_pp_flp_not_found_trap_config_set_f         mbcm_pp_flp_not_found_trap_config_set;
    mbcm_pp_flp_not_found_trap_config_get_f         mbcm_pp_flp_not_found_trap_config_get;
	
	mbcm_pp_rif_global_urpf_mode_set_f             	mbcm_pp_rif_global_urpf_mode_set;
	mbcm_pp_eg_rif_profile_delete_f                	mbcm_pp_eg_rif_profile_delete;
	mbcm_pp_eg_rif_profile_get_f                   	mbcm_pp_eg_rif_profile_get;
	mbcm_pp_eg_rif_profile_set_f                   	mbcm_pp_eg_rif_profile_set;
	
	mbcm_pp_eg_vlan_edit_packet_is_tagged_get_f      	mbcm_pp_eg_vlan_edit_packet_is_tagged_get;
	mbcm_pp_eg_vlan_edit_packet_is_tagged_set_f      	mbcm_pp_eg_vlan_edit_packet_is_tagged_set;
	
	mbcm_pp_eg_filter_default_port_membership_get_f  	mbcm_pp_eg_filter_default_port_membership_get;
	mbcm_pp_eg_filter_default_port_membership_set_f  	mbcm_pp_eg_filter_default_port_membership_set;
	mbcm_pp_llp_parse_packet_format_eg_info_get_f  	mbcm_pp_llp_parse_packet_format_eg_info_get;
	mbcm_pp_llp_parse_packet_format_eg_info_set_f  	mbcm_pp_llp_parse_packet_format_eg_info_set;
	
	mbcm_pp_ptp_p2p_delay_get_f                    	mbcm_pp_ptp_p2p_delay_get;
	mbcm_pp_ptp_p2p_delay_set_f                    	mbcm_pp_ptp_p2p_delay_set;
	
	mbcm_pp_egress_protection_state_get_f          	mbcm_pp_egress_protection_state_get;
	mbcm_pp_egress_protection_state_set_f          	mbcm_pp_egress_protection_state_set;
	mbcm_pp_ingress_protection_state_get_f         	mbcm_pp_ingress_protection_state_get;
	mbcm_pp_ingress_protection_state_set_f         	mbcm_pp_ingress_protection_state_set;
	
	mbcm_pp_presel_max_id_get_f                    	mbcm_pp_presel_max_id_get;
	
	mbcm_pp_eg_trill_entry_set_f                   	mbcm_pp_eg_trill_entry_set;
    
    mbcm_pp_get_load_balancing_diagnostic_signals_f     mbcm_pp_get_load_balancing_diagnostic_signal;
    mbcm_pp_ecmp_hash_slb_combine_set_f                 mbcm_pp_ecmp_hash_slb_combine_set;
    mbcm_pp_ecmp_hash_slb_combine_get_f                 mbcm_pp_ecmp_hash_slb_combine_get;

    mbcm_pp_eg_encap_additional_label_profile_set_f   mbcm_pp_eg_encap_additional_label_profile_set;
    mbcm_pp_eg_encap_additional_label_profile_get_f   mbcm_pp_eg_encap_additional_label_profile_get;
  
    mbcm_pp_lif_ing_vlan_edit_pcp_map_dp_set_f        mbcm_pp_lif_ing_vlan_edit_pcp_map_dp_set;
    mbcm_pp_lif_ing_vlan_edit_pcp_map_dp_get_f        mbcm_pp_lif_ing_vlan_edit_pcp_map_dp_get;
    mbcm_pp_lif_ing_vlan_edit_inner_global_info_set_f   mbcm_pp_lif_ing_vlan_edit_inner_global_info_set;
    mbcm_pp_lif_ing_vlan_edit_inner_global_info_get_f   mbcm_pp_lif_ing_vlan_edit_inner_global_info_get;
    mbcm_pp_lif_ing_vlan_edit_outer_global_info_set_f   mbcm_pp_lif_ing_vlan_edit_outer_global_info_set;
    mbcm_pp_lif_ing_vlan_edit_outer_global_info_get_f   mbcm_pp_lif_ing_vlan_edit_outer_global_info_get;
    mbcm_pp_port_eg_ttl_inheritance_set_f                   mbcm_pp_port_eg_ttl_inheritance_set;
    mbcm_pp_port_eg_ttl_inheritance_get_f                   mbcm_pp_port_eg_ttl_inheritance_get;
    mbcm_pp_port_eg_qos_inheritance_set_f                   mbcm_pp_port_eg_qos_inheritance_set;
    mbcm_pp_port_eg_qos_inheritance_get_f                   mbcm_pp_port_eg_qos_inheritance_get;
    mbcm_pp_port_eg_qos_marking_set_f                  mbcm_pp_port_eg_qos_marking_set;
    mbcm_pp_port_eg_qos_marking_get_f                  mbcm_pp_port_eg_qos_marking_get;
  
  mbcm_pp_lif_default_native_ac_outlif_set_f                            mbcm_pp_lif_default_native_ac_outlif_set;
  mbcm_pp_lif_default_outlif_get_f                                      mbcm_pp_lif_default_native_ac_outlif_get;
  mbcm__pp_eg_encap_access_default_ac_entry_vlan_edit_profile_set_f     mbcm_pp_eg_encap_access_default_ac_entry_vlan_edit_profile_set; 
  mbcm__pp_eg_encap_access_default_ac_entry_vlan_edit_profile_get_f     mbcm_pp_eg_encap_access_default_ac_entry_vlan_edit_profile_get; 
    mbcm_pp_meter_tc_map_set_f mbcm_pp_meter_tc_map_set;
    mbcm_pp_meter_tc_map_get_f mbcm_pp_meter_tc_map_get;
	
	mbcm_pp_pppoe_anti_spoofing_add_f                  	mbcm_pp_pppoe_anti_spoofing_add;
	mbcm_pp_pppoe_anti_spoofing_delete_f               	mbcm_pp_pppoe_anti_spoofing_delete;
	mbcm_pp_pppoe_anti_spoofing_get_f                  	mbcm_pp_pppoe_anti_spoofing_get;

    mbcm_pp_eg_qos_ether_dscp_remark_set_f              mbcm_pp_eg_qos_ether_dscp_remark_set;
	mbcm_pp_eg_qos_ether_dscp_remark_get_f              mbcm_pp_eg_qos_ether_dscp_remark_get;

    mbcm_pp_bfd_oamp_udp_src_port_check_enable_f        mbcm_pp_bfd_oamp_udp_src_port_check_enable;
    mbcm_pp_seamless_bfd_rx_dst_port_set_f              mbcm_pp_seamless_bfd_rx_dst_port_set;
    mbcm_pp_seamless_bfd_rx_dst_port_get_f              mbcm_pp_seamless_bfd_rx_dst_port_get;
    mbcm_pp_oamp_pe_seamless_bfd_src_port_set_f         mbcm_pp_oamp_pe_seamless_bfd_src_port_set;
    mbcm_pp_oamp_pe_seamless_bfd_src_port_get_f         mbcm_pp_oamp_pe_seamless_bfd_src_port_get;
} mbcm_pp_functions_t;


extern CONST mbcm_pp_functions_t *mbcm_pp_driver[BCM_MAX_NUM_UNITS]; 
extern soc_pp_chip_family_t mbcm_pp_family[BCM_MAX_NUM_UNITS];

extern CONST mbcm_pp_functions_t mbcm_pp_arad_driver;
extern CONST mbcm_pp_functions_t mbcm_pp_jericho_driver;
extern CONST mbcm_pp_functions_t mbcm_pp_jericho_plus_driver;
extern CONST mbcm_pp_functions_t mbcm_pp_qax_driver;

extern int mbcm_dpp_pp_init(int unit);


#define MBCM_PP_DRIVER_CALL(unit, function, args) ((mbcm_pp_driver[unit] && mbcm_pp_driver[unit]->function)?mbcm_pp_driver[unit]->function args:SOC_E_INTERNAL)
#define MBCM_PP_DRIVER_CALL_VOID(unit, function, args) (mbcm_pp_driver[unit]->function args)





#endif 
