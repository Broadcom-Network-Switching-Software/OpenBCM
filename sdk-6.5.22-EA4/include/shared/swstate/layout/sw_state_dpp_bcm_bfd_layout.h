/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
*/
#ifndef _SHR_sw_state_dpp_bcm_bfd_H_
#define _SHR_sw_state_dpp_bcm_bfd_H_


#define SW_STATE_DPP_BCM_BFD_YOURDISCRIMINATOR_REF_COUNT_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_S_HOP_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_S_HOP_TOS_VALUE_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_S_HOP_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_BCM_BFD_S_HOP_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_S_HOP_TOS_VALUE_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_BCM_BFD_TRAP_INFO_TRAP_IDS_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_TRAP_INFO_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_BCM_BFD_TRAP_INFO_TRAP_IDS_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_BCM_BFD_MPLS_UDP_SPORT_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD__BCM_BFD_ENDPOINT_LIST_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_IPV4_UDP_SPORT_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_IPV4_MULTI_HOP_ACC_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_PDU_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_MPLS_TP_CC_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_TX_RATE_VALUE_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_TX_RATE_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_NO_OF_SBFD_ENDPOINTS_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_REFLECTOR_TRAP_ID_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_REFLECTOR_STATUS_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_NO_OF_SBFD_REFLECTOR_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_BFD_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_BCM_BFD_YOURDISCRIMINATOR_REF_COUNT_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_S_HOP_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_TRAP_INFO_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_MPLS_UDP_SPORT_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD__BCM_BFD_ENDPOINT_LIST_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_IPV4_UDP_SPORT_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_IPV4_MULTI_HOP_ACC_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_PDU_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_MPLS_TP_CC_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_TX_RATE_VALUE_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_TX_RATE_REF_COUNTER_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_NO_OF_SBFD_ENDPOINTS_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_REFLECTOR_TRAP_ID_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_REFLECTOR_STATUS_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_BFD_NO_OF_SBFD_REFLECTOR_LAYOUT_NOF_MAX_LAYOUT_NODES) 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_YourDiscriminator_ref_count_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_s_hop_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_s_hop_ref_counter_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_s_hop_tos_value_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_trap_info_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_trap_info_trap_ids_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_mpls_udp_sport_ref_counter_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd__bcm_bfd_endpoint_list_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_ipv4_udp_sport_ref_counter_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_ipv4_multi_hop_acc_ref_counter_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_pdu_ref_counter_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_mpls_tp_cc_ref_counter_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_tx_rate_value_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_tx_rate_ref_counter_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_no_of_sbfd_endpoints_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_reflector_trap_id_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_reflector_status_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_bfd_no_of_sbfd_reflector_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#endif /* _SHR_sw_state_dpp_bcm_bfd_H_ */
