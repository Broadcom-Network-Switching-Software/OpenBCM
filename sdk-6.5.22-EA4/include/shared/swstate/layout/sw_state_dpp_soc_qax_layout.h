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
#ifndef _SHR_sw_state_dpp_soc_qax_H_
#define _SHR_sw_state_dpp_soc_qax_H_


#define SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LB_EGR_IF_BMP_USED_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LB_ING_MODEM_FRAGMENT_CNT_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LB_ING_MODEM_BYTE_CNT_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LB_EGR_LBG_FIFO_START_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LB_EGR_LBG_FIFO_NOF_ENTRIES_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LB_EGR_IF_BMP_USED_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LB_ING_MODEM_FRAGMENT_CNT_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LB_ING_MODEM_BYTE_CNT_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LB_EGR_LBG_FIFO_START_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LB_EGR_LBG_FIFO_NOF_ENTRIES_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_SOC_QAX_TM_GUARANTEED_INFO_GUARANTEED_TOTAL_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_TM_GUARANTEED_INFO_GUARANTEED_USED_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_TM_GUARANTEED_INFO_GUARANTEED_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_TM_GUARANTEED_INFO_GUARANTEED_TOTAL_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_TM_GUARANTEED_INFO_GUARANTEED_USED_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_SOC_QAX_TM_GUARANTEED_INFO_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_TM_GUARANTEED_INFO_GUARANTEED_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_SOC_QAX_TM_LOSSLESS_POOL_ID_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_TM_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_TM_LB_INFO_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_TM_GUARANTEED_INFO_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_TM_LOSSLESS_POOL_ID_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_SOC_QAX_PP_MEP_DB_FLEXIBLE_VERIFICATION_USE_INDICATOR_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_PP_MEP_DB_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_PP_MEP_DB_FLEXIBLE_VERIFICATION_USE_INDICATOR_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_SOC_QAX_PP_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_PP_MEP_DB_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_IPSEC_PORT_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_NUM_OF_IPSEC_CHANNELS_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_NUM_OF_BFD_CHANNELS_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_SA_TO_TUNNEL_MAP_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_SA_INITIAL_SEQ_NUM_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_SA_MTU_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_SA_FRAGMENTATION_ENABLE_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_SA_ENCRYPT_TYPE_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_SA_AUTH_TYPE_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_SA_SA_DIRECTION_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_SA_SPI_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_SA_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_IPSEC_SA_INITIAL_SEQ_NUM_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_SA_MTU_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_SA_FRAGMENTATION_ENABLE_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_SA_ENCRYPT_TYPE_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_SA_AUTH_TYPE_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_SA_SA_DIRECTION_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_SA_SPI_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_CONTEXT_ID_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_LOCAL_IP_IPV4_ADDR_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_LOCAL_IP_IPV6_ADDR_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_LOCAL_IP_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_LOCAL_IP_IPV4_ADDR_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_LOCAL_IP_IPV6_ADDR_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_REMOTE_IP_IPV4_ADDR_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_REMOTE_IP_IPV6_ADDR_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_REMOTE_IP_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_REMOTE_IP_IPV4_ADDR_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_REMOTE_IP_IPV6_ADDR_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_PROTOCOL_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_TTL_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_DSCP_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_PORT_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_CONTEXT_ID_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_LOCAL_IP_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_REMOTE_IP_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_PROTOCOL_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_TTL_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_DSCP_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_PORT_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_SOC_QAX_IPSEC_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_IPSEC_IPSEC_PORT_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_NUM_OF_IPSEC_CHANNELS_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_NUM_OF_BFD_CHANNELS_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_SA_TO_TUNNEL_MAP_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_SA_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_TUNNEL_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_SOC_QAX_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_SOC_QAX_TM_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_PP_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_SOC_QAX_IPSEC_LAYOUT_NOF_MAX_LAYOUT_NODES) 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_lb_info_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_lb_info_lb_egr_if_bmp_used_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_lb_info_lb_ing_modem_fragment_cnt_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_lb_info_lb_ing_modem_byte_cnt_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_lb_info_lb_egr_lbg_fifo_start_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_lb_info_lb_egr_lbg_fifo_nof_entries_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_guaranteed_info_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_guaranteed_info_guaranteed_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_guaranteed_info_guaranteed_total_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_guaranteed_info_guaranteed_used_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_tm_lossless_pool_id_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_pp_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_pp_mep_db_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_pp_mep_db_flexible_verification_use_indicator_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_ipsec_port_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_num_of_ipsec_channels_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_num_of_bfd_channels_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_sa_to_tunnel_map_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_sa_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_sa_initial_seq_num_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_sa_mtu_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_sa_fragmentation_enable_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_sa_encrypt_type_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_sa_auth_type_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_sa_sa_direction_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_sa_spi_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_context_id_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_local_ip_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_local_ip_ipv4_addr_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_local_ip_ipv6_addr_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_remote_ip_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_remote_ip_ipv4_addr_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_remote_ip_ipv6_addr_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_protocol_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_ttl_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_dscp_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_soc_qax_ipsec_tunnel_port_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#endif /* _SHR_sw_state_dpp_soc_qax_H_ */
