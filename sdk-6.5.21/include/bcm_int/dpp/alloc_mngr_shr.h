/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr_shr.h
 * Purpose:     Resource allocation shared between alloc and wb.
 */

#ifndef  INCLUDE_ALLOC_MNGR_SHR_H
#define  INCLUDE_ALLOC_MNGR_SHR_H

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/drv.h>
#include <shared/swstate/sw_state_resmgr.h>
#include <shared/shr_template.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/vlan.h>
#include <soc/dpp/PPD/ppd_api_eg_encap.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>

/* Res managemr Defines and Structures. */
typedef enum _dpp_am_res_e {
    /****************************************/
    /************   TM POOLS   **************/
    /****************************************/
    dpp_am_res_mc_dynamic = 0,
    dpp_am_res_trap_single_instance,
    dpp_am_res_trap_user_define,
    dpp_am_res_trap_virtual,
    dpp_am_res_trap_reserved_mc,
    dpp_am_res_trap_prog,
    dpp_am_res_trap_egress,
    dpp_am_res_snoop_cmd,
    dpp_am_res_qos_egr_pcp_vlan,

    /****************************************/
    /************   PP POOLS   **************/
    /****************************************/
    dpp_am_res_vsi_vlan,
    dpp_am_res_vsi_mstp,
    dpp_am_res_lif_pwe,
    dpp_am_res_lif_ip_tnl,
    dpp_am_res_lif_dynamic,
    dpp_am_res_fec_global,
    dpp_am_res_failover_common_id,
    dpp_am_res_failover_ingress_id,
    dpp_am_res_failover_fec_id,
    dpp_am_res_failover_egress_id,
    dpp_am_res_eep_global,
    dpp_am_res_eep_ip_tnl, /* used for ip tunnels*/
    dpp_am_res_eep_mpls_tunnel, /* used for mpls/pwe tunnels*/
    dpp_am_res_glbl_src_ip,
    dpp_am_res_glbl_ttl,
    dpp_am_res_glbl_tos,
    dpp_am_res_eg_out_ac,
    dpp_am_res_eg_out_rif,
    dpp_am_res_eg_data_erspan,
    dpp_am_res_ipv6_tunnel,
    dpp_am_res_eg_data_trill_invalid_entry,
    dpp_am_res_qos_ing_elsp,
    dpp_am_res_qos_ing_lif_cos,
    dpp_am_res_qos_ing_pcp_vlan,
    dpp_am_res_qos_ing_cos_opcode,
    dpp_am_res_qos_egr_remark_id,
    dpp_am_res_qos_egr_mpls_php_id,
    dpp_am_res_meter_a, /* must be directly before meter_b */
    dpp_am_res_meter_b, /* must be directly after meter_a */
    dpp_am_res_ether_policer,
#ifdef BCM_ARAD_SUPPORT
    dpp_am_res_ecmp_id,
#endif 
    dpp_am_res_qos_egr_l2_i_tag,
#ifdef BCM_88660
    dpp_am_res_qos_egr_dscp_exp_marking,
#endif /* BCM_88660 */
    dpp_am_res_rp_id, /* RPIDs for IPMC BIDIR*/
    dpp_am_res_oam_ma_index,
    dpp_am_res_oam_mep_id_short,
    dpp_am_res_oam_mep_id_long,
    dpp_am_res_oam_mep_id_long_non_48_maid,
    dpp_am_res_oam_mep_id_long_48_maid,
    dpp_am_res_oam_mep_id_jumbo_tlv,
    dpp_am_res_oam_mep_id_no_jumbo_tlv,
    dpp_am_res_oam_rmep_id,
    dpp_am_res_oam_trap_code_upmep_ftmh_header,
    dpp_am_res_pon_channel_profile,
    dpp_am_res_vlan_edit_action_ingress,
    dpp_am_res_vlan_edit_action_egress, 
    dpp_am_res_trill_virtual_nick_name,
    dpp_am_res_local_inlif_common,
    dpp_am_res_local_inlif_wide,
    dpp_am_res_local_outlif,
    dpp_am_res_eep_trill,
    dpp_am_res_global_inlif,
    dpp_am_res_global_outlif,
    dpp_am_res_obs_inlif,
    dpp_am_res_obs_eg_encap,
    dpp_am_res_vsq_src_port,
    dpp_am_res_vsq_pg,
    dpp_am_res_map_encap_intpri_color,
    dpp_am_res_field_entry_id,
    dpp_am_res_field_direct_extraction_entry_id,
    dpp_am_res_oam_mep_id,
    dpp_am_res_oam_mep_db_id,
    dpp_am_res_trap_etpp,
    dpp_am_res_global_sync_lif,
    dpp_am_res_oam_sd_sf_id,
    dpp_am_res_oam_y_1711_sd_sf_id,
    dpp_am_res_qos_ing_color,
    dpp_am_res_ipsec_sa_id,
    dpp_am_res_ipsec_tunnel_id,
    dpp_am_res_eg_data_l2tp,
    dpp_am_res_count /* MUST BE LAST -- NOT A VALID ID */
} _dpp_am_res_t;

/* 
 * Pool structure for all pools other than cosq. 
 * for cosq see bcm_dpp_am_cosq_pool_info_t
 * A pool_id represents mapping between resource_id and core_id.
 */
typedef struct {
    int                 pool_id;        /* The pool on which the resource is allocated. */
    int                 res_id;         /* The resource using the pool. */
    int                 core_id;        /* The core using the pool.*/    
    sw_state_res_allocator_t res_type;       /* resource storage type */
    int                 start;          /* resource start */
    int                 count;          /* count of resources */
    int                 max_elements_per_allocation; /* required for wb storage */
} bcm_dpp_am_pool_info_t;


/* Template management Defines and Structures */
typedef enum _dpp_am_template_e {
    /****************************************/
    /************ TM TEMPLATES **************/
    /****************************************/
    dpp_am_template_egress_thresh = 0,
    dpp_am_template_mirror_action_profile,
    dpp_am_template_egress_interface_unicast_thresh,
    dpp_am_template_egress_interface_multicast_thresh,
    dpp_am_template_user_defined_traps,
    dpp_am_template_snoop_cmd,
    dpp_am_template_trap_reserved_mc,
    dpp_am_template_prog_trap,
    dpp_am_template_queue_rate_cls,
    dpp_am_template_system_red_dp_pr,
    dpp_am_template_vsq_pg_tc_mapping,
    dpp_am_template_vsq_rate_cls_ct,
    dpp_am_template_vsq_rate_cls_cttc,
    dpp_am_template_vsq_rate_cls_ctcc,
    dpp_am_template_vsq_rate_cls_pp,
    dpp_am_template_vsq_rate_cls_src_port,
    dpp_am_template_vsq_rate_cls_pg,
    dpp_am_template_queue_discount_cls,
    dpp_am_template_pp_port_discount_cls,
    dpp_am_template_egress_port_discount_cls_type_raw,
    dpp_am_template_egress_port_discount_cls_type_cpu,
    dpp_am_template_egress_port_discount_cls_type_eth,
    dpp_am_template_egress_port_discount_cls_type_tm,
    dpp_am_template_cosq_port_hr_flow_control, /* e2e port hr flow control profile */
    dpp_am_template_cosq_sched_class,          /* scheduler (cl) class */
    dpp_am_template_fabric_tdm_link_ptr,       /* TDM Direct routing link pointer */
    dpp_am_template_ingress_flow_tc_mapping,    /* Ingress Flow TC Mapping Profiles */
    dpp_am_template_ingress_uc_tc_mapping,      /* Ingress UC TC Mapping Profiles */
    dpp_am_template_fc_generic_pfc_mapping,     /* Flow Control Generic PFC Profiles */
    dpp_am_template_fc_generic_pfc_mapping_c0,     /* Flow Control Generic PFC Profiles for priority 0 */
    dpp_am_template_fc_generic_pfc_mapping_c1,     /* Flow Control Generic PFC Profiles for priority 1 */
    dpp_am_template_fc_generic_pfc_mapping_c2,     /* Flow Control Generic PFC Profiles for priority 2 */
    dpp_am_template_fc_generic_pfc_mapping_c3,     /* Flow Control Generic PFC Profiles for priority 3 */
    dpp_am_template_fc_generic_pfc_mapping_c4,     /* Flow Control Generic PFC Profiles for priority 4 */
    dpp_am_template_fc_generic_pfc_mapping_c5,     /* Flow Control Generic PFC Profiles for priority 5 */
    dpp_am_template_fc_generic_pfc_mapping_c6,     /* Flow Control Generic PFC Profiles for priority 6 */
    dpp_am_template_fc_generic_pfc_mapping_c7,     /* Flow Control Generic PFC Profiles for priority 7 */
    dpp_am_template_cnm_queue_profile,
    dpp_am_template_tpid_profile,
    dpp_am_template_egress_queue_mapping,
    dpp_am_template_trap_egress,
    dpp_am_template_nrdy_threshold,

    /****************************************/
    /************   PP POOLS   **************/
    /****************************************/
    dpp_am_template_vlan_edit_profile_mapping,    /* VLAN Edit Profile */
    dpp_am_template_vlan_edit_profile_mapping_eg, /* Egress VLAN Edit Profile */      
    dpp_am_template_vsi_egress_profile,
    dpp_am_template_vsi_ingress_profile,
    dpp_am_template_reserved_mc,
    dpp_am_template_tpid_class,
    dpp_am_template_mpls_push_profile,
    dpp_am_template_lif_term_profile,
    dpp_am_template_lif_term_pw_with_cw_profile,
    dpp_am_template_port_mact_sa_drop_profile,
    dpp_am_template_port_mact_da_unknown_profile,
    /* don't separate between below 4 templates */
    dpp_am_template_meter_profile_a_low, 
    dpp_am_template_meter_profile_b_low, 
    /* don't separate between above 4 templates */
    dpp_am_template_l2_event_handle,            /* from even-profile, to <event x self/learn/shadow> */
    dpp_am_template_l2_vsi_learn_profile,       /* from learn-profile to <limit, event-profile, aging-profile> */
    dpp_am_template_l2_flooding_profile,        /* flooding by <port_profile,lif_profile> */
    dpp_am_template_vlan_port_protocol_profile, /* port protocol mapping. <port,profile> ->10ethertype,tc,vlan */
    dpp_am_template_ip_tunnel_src_ip,
    dpp_am_template_ip_tunnel_ttl,
    dpp_am_template_ip_tunnel_tos,
    dpp_am_template_ttl_scope_index, 
    dpp_am_template_oam_icc_map,
    dpp_am_template_oam_sa_mac,
    dpp_am_template_oam_punt_event_hendling_profile,
    dpp_am_template_oam_mep_profile_non_accelerated,
    dpp_am_template_oam_mep_profile_accelerated,
    dpp_am_template_bfd_mep_profile_non_accelerated,
    dpp_am_template_bfd_mep_profile_accelerated,
    dpp_am_template_oam_tx_priority,
    dpp_am_template_bfd_ip_dip,
    dpp_am_template_mpls_pwe_push_profile,
    dpp_am_template_bfd_req_interval_pointer,
    dpp_am_template_bfd_tos_ttl_profile,
    dpp_am_template_bfd_src_ip_profile,
    dpp_am_template_bfd_flags_profile,
    dpp_am_template_oam_lmm_nic_tables_profile,
    dpp_am_template_oam_lmm_oui_tables_profile,
    dpp_am_template_oam_eth1731_profile,
    dpp_am_template_oam_local_port_2_system_port,
    dpp_am_template_oam_oamp_pe_gen_mem,
    dpp_am_template_port_tpid_class_egress_acceptable_frame_type,
    dpp_am_template_ptp_port_profile,
    dpp_am_template_l3_vrrp,
    dpp_am_template_l3_rif_mac_termination_combination,
    dpp_am_template_eedb_roo_ll_format_eth_type_index,
    dpp_am_template_tpid_class_eg,
    dpp_am_template_out_rif_profile,
    dpp_am_template_oamp_pe_gen_mem_maid_48, 
    dpp_am_template_ip_tunnel_encapsulation_mode,
    dpp_am_template_crps_inpp_port_compensation_profile,
    dpp_am_template_crps_intm_port_compensation_profile,
    dpp_am_template_stat_interface_ing_port_compensation_profile,
    dpp_am_template_stat_interface_egr_port_compensation_profile,
    dpp_am_template_scheduler_adjust_size_final_delta, /* relevant for Jericho only */
    dpp_am_template_lif_mtu_profile, /*Relevant only for JB0 and above*/
    dpp_am_template_oamp_flex_ver_mask,
    dpp_am_template_oamp_cls_flex_crc,
    dpp_am_template_l2cp_egress_profile,
    dpp_am_template_oam_sd_sf_profile,
    dpp_am_template_bfd_sbfd_non_acc_src_ip_profile,
    dpp_am_template_count /* MUST BE LAST -- NOT A VALID ID */
} _dpp_am_template_t;

/* 
 * Template structure for templates other than those already supported by cosq
 * for warmboot.
 * A pool_id represents mapping between template id and core id.
 */
typedef struct {
    int                    pool_id;         /* Pool id of the pool. */
    int                    template_id;     /* Template id using the pool. */
    int                    core_id;         /* Core id using the pool. */
    shr_template_manage_t  manager;
    int                    start;
    int                    count;
    int                    max_entities; /* max referring entities */
    uint32                 global_max;  /* Max referring entinties on all templates. */
    size_t                 data_size;
    shr_template_manage_hash_compare_extras_t hashExtra;
} bcm_dpp_am_template_info_t;

/* An array representing the various hash compare callbacks. */
typedef enum _bcm_dpp_am_template_hash_compare_cb_idx_e {
    _bcm_dpp_am_template_hash_compare_cb_idx_memcpy,
    _bcm_dpp_am_template_hash_compare_cb_idx_user_defined_trap,
    _bcm_dpp_am_template_hash_compare_cb_idx_snoop_cmd,
    _bcm_dpp_am_template_hash_compare_cb_idx_discount_cls,
    _bcm_dpp_am_template_hash_compare_cb_idx_count
} _bcm_dpp_am_template_hash_compare_cb_idx_t;

extern shr_template_to_stream_t _bcm_dpp_am_template_to_stream_arr[_bcm_dpp_am_template_hash_compare_cb_idx_count];
extern shr_template_from_stream_t _bcm_dpp_am_template_from_stream_arr[_bcm_dpp_am_template_hash_compare_cb_idx_count];


/*Below is the global array of call back functions used to print the template data */
extern shr_template_print_func_t _bcm_dpp_am_template_print_func[dpp_am_template_count];

int
_bcm_dpp_pp_resource_setup(int unit, int res_id, int core_id, int pool_id);

int
_bcm_dpp_template_setup(int unit, int template_id, int core_id, int pool_id);

int
_bcm_dpp_am_template_tinfo_get(int unit, int pool_idx, int template_id, bcm_dpp_am_template_info_t* t_info);

int
bcm_dpp_am_l2_vsi_vlan_alloc(int unit,
                             uint32 flags,
                             bcm_vlan_t *vsi);
int
bcm_dpp_am_l2_vpn_pwe_alloc(int unit,
                            uint32 lif_term_types,
                            uint32 flags,
                            int count,
                            SOC_PPC_LIF_ID *lif);
int
bcm_dpp_am_l3_lif_ip_tnl_alloc(int unit,
                               uint32 flags,
                               SOC_PPC_LIF_ID *lif);
int
bcm_dpp_am_l2_lif_mim_alloc(int unit);

int
bcm_dpp_am_mc_alloc(int unit,
                    uint32 flags, /* flags should be SHR_RES_ALLOC_WITH_ID; */
                    SOC_TMC_MULT_ID  *mc_id,
                    uint8 is_egress);


typedef enum {
  /* Standard FEC */
  _BCM_DPP_AM_FEC_ALLOC_USAGE_STANDARD,
  /* L3 ECMP - DEPRECATED - DO NOT USE - only used by bcm_l3_egress_multipath_* */
  _BCM_DPP_AM_FEC_ALLOC_USAGE_ECMP,
  /* Group-B FEC (for Hierarchical FEC) */
  _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED
} _bcm_dpp_am_fec_alloc_usage;

int
bcm_dpp_am_fec_global_alloc(int unit,
                            uint32 flags,
                            _bcm_dpp_am_fec_alloc_usage usage,
                            int size,
                            int aligned,
                            int *elem); 
int
bcm_dpp_am_fec_alloc(int unit,
                     uint32 flags,
                     _bcm_dpp_am_fec_alloc_usage usage,
                     int size,/*non-zero for protection or ECMP*/
                     SOC_PPC_FEC_ID *fec_id);
int
bcm_dpp_am_failover_alloc(int unit,
                          uint32 flags,
                          int32 failover_type,
                          int32 nof_alloc_ids,
                          bcm_failover_t *failover_id);

#define _BCM_DPP_AM_L3_EEP_TYPE_DEFAULT (0) /* Link-layer */
#define _BCM_DPP_AM_L3_EEP_TYPE_ROO_LINKER_LAYER (1) /* ROO Linker-layer */


int
bcm_dpp_am_l3_eep_alloc(int unit,
                        uint32 types,
                        uint32 flags,
                        int *eep);

#define _BCM_DPP_AM_IP_TUNNEL_EEP_TYPE_DEFAULT (0) /* IP tunnel  */
#define _BCM_DPP_AM_IP_TUNNEL_EEP_TYPE_ROO     (1) /* ROO IP tunnel */
int
bcm_dpp_am_ip_tunnel_eep_alloc(int unit,
                               uint32 types, 
                               uint32 flags,
                               int *eep);

int
bcm_dpp_am_l2_encap_eep_alloc(int unit, uint32 flags,int *eep);
int
bcm_dpp_am_l2_encap_eep_is_alloced(int unit, int eep);
int
bcm_dpp_am_l2_encap_eep_dealloc(int unit, int eep);

#define _BCM_DPP_AM_MPLS_EEP_TYPE_PWE                (0)
#define _BCM_DPP_AM_MPLS_EEP_TYPE_SIMPLE_MPLS_TUNNEL (1)
#define _BCM_DPP_AM_MPLS_EEP_TYPE_SECOND_MPLS_TUNNEL (2)
/* This type is for creating PWE with EGRESS_ONLY flag. The functionality is the same as _BCM_DPP_AM_MPLS_EEP_TYPE_SECOND_MPLS_TUNNEL */
#define _BCM_DPP_AM_MPLS_EEP_TYPE_PWE_EGRESS_ONLY    (3)
int
bcm_dpp_am_mpls_eep_alloc(int unit,
                          uint32 type,
                          uint32 flags,
                          int count,
                          int *eep);

int
bcm_dpp_am_ip_tunnel_glbl_src_ip_alloc(int unit,
                                       uint32 flags,
                                       int *idx);

int
bcm_dpp_am_ip_tunnel_glbl_ttl_alloc(int unit,
                                    uint32 flags,
                                    int *idx);
int
bcm_dpp_am_ip_tunnel_glbl_tos_alloc(int unit,
                                    uint32 flags,
                                    int *idx);

#define _BCM_DPP_AM_OUT_AC_TYPE_DEFAULT           (0)
#define _BCM_DPP_AM_OUT_AC_TYPE_PON_3_TAGS_DATA   (1)
#define _BCM_DPP_AM_OUT_AC_TYPE_PON_3_TAGS_OUT_AC (2)
#ifdef BCM_88660_A0
#define _BCM_DPP_AM_OUT_AC_TYPE_BIG_OUT_AC        (3)
#define _BCM_DPP_AM_OUT_AC_TYPE_DATA_MPLS_PHASE   (4)
#endif

int
bcm_dpp_am_out_ac_alloc(int unit,
                        uint32 types,
                        uint32 flags,
                        SOC_PPC_AC_ID *out_ac);

int
bcm_dpp_am_qos_ing_elsp_alloc(int unit,
                              uint32 flags,
                              int *qos_id);

int
bcm_dpp_am_qos_ing_lif_cos_alloc(int unit,
                                 uint32 flags,
                                 int *qos_id);


int
bcm_dpp_am_qos_ing_pcp_vlan_alloc(int unit,
                                  uint32 flags,
                                  int *qos_id);

int
bcm_dpp_am_qos_ing_color_alloc(int unit,
                                  uint32 flags,
                                  int *qos_id);

int
bcm_dpp_am_qos_egr_remark_id_alloc(int unit,
                                   uint32 flags,
                                   int *qos_id);

int
bcm_dpp_am_qos_egr_mpls_php_id_alloc(int unit,
                                   uint32 flags,
                                   int *qos_id);

int
bcm_dpp_am_qos_egr_pcp_vlan_alloc(int unit,
                                  uint32 flags,
                                  int *qos_id);

int
bcm_dpp_am_qos_ing_cos_opcode_alloc(int unit,
                                    uint32 flags,
                                    int *qos_id);

int
bcm_dpp_am_qos_egr_l2_i_tag_alloc(int unit,
                                  uint32 flags,
                                  int *qos_id);

#ifdef BCM_88660
int
bcm_dpp_am_qos_egr_dscp_exp_marking_alloc(int unit,
                                  uint32 flags,
                                  int *qos_id);
#endif /* BCM_88660 */

int
bcm_dpp_am_meter_alloc(int unit,
					   uint32 flags,
					   int core,
					   int meter_group,
					   int nof_meters, 
					   int *act_meter_core,
					   int *act_meter_group,
					   int *meter_id);

int
bcm_dpp_am_policer_alloc(int unit,
						 int core_id,
                         uint32 flags,
                         int size,
                         int *policer_id);
int
bcm_dpp_am_vrf_alloc(int unit,
                     uint32 flags,
                     int *vrf_id);

int
bcm_dpp_am_vlan_edit_action_id_alloc(int unit,
                                     uint32 flags,
                                     int *action_id);

int 
_bcm_dpp_am_trap_alloc(int unit,
                       int flags,
                       int trap_id,
                       int *elem);
int
_bcm_dpp_am_trap_single_instance_alloc(int unit,
                                       int flags,
                                       int *elem);

int
_bcm_dpp_am_trap_single_instance_etpp_alloc(int unit,
                                       int flags,
                                       int *elem);
int
_bcm_dpp_am_trap_user_define_alloc(int unit,
                                   int flags,
                                   int *elem);
int
_bcm_dpp_am_trap_virtual_alloc(int unit,
                               int flags,
                               int *elem);

int
_bcm_dpp_am_trap_egress_alloc(int unit,
                              int flags,
                              int *elem);
int
_bcm_dpp_am_snoop_alloc(int unit,
                        int flags,
                        int snoop_cmnd,
                        int *elem);
int
_bcm_dpp_am_snoop_cmd_alloc(int unit,
                            int flags,
                            int *elem);

int 
_bcm_dpp_am_template_user_defined_trap_allocate(int unit,
                                                uint32 flags,
                                                int port,
                                                const bcm_dpp_user_defined_traps_t* data,
                                                int *is_allocated,
                                                int *user_defined_trap);

int 
_bcm_dpp_am_template_tpid_profile_allocate_group(int unit, 
                                                 uint32 flags, 
                                                 uint32 *data, 
                                                 int ref_count, 
                                                 int *is_allocated, 
                                                 int *template_param);


int 
_bcm_dpp_am_template_vlan_edit_profile_mapping_alloc(int unit, 
                                                     int flags, 
                                                     SOC_PPC_LIF_ID lif_id, 
                                                     bcm_dpp_vlan_edit_profile_mapping_info_t* mapping_profile, 
                                                     int *template_param,
                                                     int *is_allocated);

int 
_bcm_dpp_am_template_vlan_edit_profile_eg_mapping_allocate(int unit, 
                                                           uint32 flags, 
                                                           bcm_dpp_vlan_egress_edit_profile_info_t *mapping_profile, 
                                                           uint32 *template_id, 
                                                           int *is_allocated);

int _bcm_dpp_am_template_mpls_push_profile_alloc(int unit, 
                                                 int flags, 
                                                 int push_profile, 
                                                 SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO *push_profile_info, 
                                                 int *new_push_profile,
                                                 int *is_allocated);
int
_bcm_dpp_am_template_trap_egress_allocate(int unit,
                                          int flags,
                                          SOC_SAND_IN SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *data,
                                          int *template_param);

#endif /* INCLUDE_ALLOC_MNGR_SHR_H */
