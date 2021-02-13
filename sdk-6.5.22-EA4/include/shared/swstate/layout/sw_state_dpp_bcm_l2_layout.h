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
#ifndef _SHR_sw_state_dpp_bcm_l2_H_
#define _SHR_sw_state_dpp_bcm_l2_H_


#define SW_STATE_DPP_BCM_L2_MYMAC_MSB_IS_SET_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_L2_L2_INITIALIZED_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_L2_BCM_PETRA_L2_FREEZE_STATE_FROZEN_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_L2_BCM_PETRA_L2_FREEZE_STATE_SAVE_AGE_ENA_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_L2_BCM_PETRA_L2_FREEZE_STATE_SAVE_LEARN_ENA_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_L2_BCM_PETRA_L2_FREEZE_STATE_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_BCM_L2_BCM_PETRA_L2_FREEZE_STATE_FROZEN_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_L2_BCM_PETRA_L2_FREEZE_STATE_SAVE_AGE_ENA_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_L2_BCM_PETRA_L2_FREEZE_STATE_SAVE_LEARN_ENA_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_BCM_L2_MPLS_EXTENDED_LABEL_ETHER_TYPE_INDEX_TABLE_IS_SET_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_L2_ETHER_TYPE_INDEX_TABLE_REF_CNT_ETH_TYPE_REF_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_L2_ETHER_TYPE_INDEX_TABLE_REF_CNT_OUTER_TPID_REF_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_L2_ETHER_TYPE_INDEX_TABLE_REF_CNT_INNER_TPID_REF_LAYOUT_NOF_MAX_LAYOUT_NODES (1) 
#define SW_STATE_DPP_BCM_L2_ETHER_TYPE_INDEX_TABLE_REF_CNT_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_BCM_L2_ETHER_TYPE_INDEX_TABLE_REF_CNT_ETH_TYPE_REF_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_L2_ETHER_TYPE_INDEX_TABLE_REF_CNT_OUTER_TPID_REF_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_L2_ETHER_TYPE_INDEX_TABLE_REF_CNT_INNER_TPID_REF_LAYOUT_NOF_MAX_LAYOUT_NODES) 
#define SW_STATE_DPP_BCM_L2_LAYOUT_NOF_MAX_LAYOUT_NODES (1 + SW_STATE_DPP_BCM_L2_MYMAC_MSB_IS_SET_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_L2_L2_INITIALIZED_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_L2_BCM_PETRA_L2_FREEZE_STATE_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_L2_MPLS_EXTENDED_LABEL_ETHER_TYPE_INDEX_TABLE_IS_SET_LAYOUT_NOF_MAX_LAYOUT_NODES + SW_STATE_DPP_BCM_L2_ETHER_TYPE_INDEX_TABLE_REF_CNT_LAYOUT_NOF_MAX_LAYOUT_NODES) 

#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_mymac_msb_is_set_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_l2_initialized_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_bcm_petra_l2_freeze_state_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_bcm_petra_l2_freeze_state_frozen_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_bcm_petra_l2_freeze_state_save_age_ena_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_bcm_petra_l2_freeze_state_save_learn_ena_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_mpls_extended_label_ether_type_index_table_is_set_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_ether_type_index_table_ref_cnt_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_ether_type_index_table_ref_cnt_eth_type_ref_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_ether_type_index_table_ref_cnt_outer_tpid_ref_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 
#ifdef BCM_PETRA_SUPPORT
#if defined(BCM_PETRA_SUPPORT)
int sw_state_dpp_bcm_l2_ether_type_index_table_ref_cnt_inner_tpid_ref_layout_node_create(int unit, int* root_node_id, uint32* next_free_node_id);
#endif /* defined(BCM_PETRA_SUPPORT)*/ 
#endif /* BCM_PETRA_SUPPORT*/ 

#endif /* _SHR_sw_state_dpp_bcm_l2_H_ */
