/*! \file bcmcth_ctr_dbgsel_imm.c
 *
 * BCMCTH ING/EGR Debug Counter Trigger Selection IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmcth/bcmcth_ctr_dbgsel_drv.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTRDBGSEL

/*******************************************************************************
 * Private Functions
 */

/*
 * Helper function to initialize map of field id to enum.
 */
static void
ctr_dbgsel_map_init(bcmltd_sid_t sid, ctr_dbgsel_trigger *fid2enum)
{
    if (sid == CTR_ING_DEBUG_SELECTt) {
        fid2enum[CTR_ING_DEBUG_SELECTt_ECMP_NOT_RESOLVEDf]
                = CTR_ING_DBG_SEL_ECMP_NOT_RESOLVED;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_VOQ_FLOW_CTRLf]
                = CTR_ING_DBG_SEL_RX_VOQ_FLOW_CTRL;
        fid2enum[CTR_ING_DEBUG_SELECTt_BFD_UNKNOWN_VER_OR_DISCARDf]
                = CTR_ING_DBG_SEL_BFD_UNKNOWN_VER_OR_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_BFD_UNKNOWN_ACH_ERRf]
                = CTR_ING_DBG_SEL_BFD_UNKNOWN_ACH_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_BFD_UNKNOWN_CTRL_PKTf]
                = CTR_ING_DBG_SEL_BFD_UNKNOWN_CTRL_PKT;
        fid2enum[CTR_ING_DEBUG_SELECTt_TRILL_RPF_CHECK_FAIL_DROPf]
                = CTR_ING_DBG_SEL_TRILL_RPF_CHECK_FAIL_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_TRILL_HOPCOUNT_CHECK_FAILf]
                = CTR_ING_DBG_SEL_TRILL_HOPCOUNT_CHECK_FAIL;
        fid2enum[CTR_ING_DEBUG_SELECTt_TRILL_RBRIDGE_LOOKUP_MISS_DROPf]
                = CTR_ING_DBG_SEL_TRILL_RBRIDGE_LOOKUP_MISS_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_OFFSET_TRILL_ERRORS_DROPf]
                = CTR_ING_DBG_SEL_OFFSET_TRILL_ERRORS_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_TRILL_FRAME_ON_ACCESS_PORT_DROPf]
                = CTR_ING_DBG_SEL_TRILL_FRAME_ON_ACCESS_PORT_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_NON_TRILL_FRAME_ON_NETWORK_PORT_DROPf]
                = CTR_ING_DBG_SEL_NON_TRILL_FRAME_ON_NETWORK_PORT_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_NIV_FORWARDING_DROPf]
                = CTR_ING_DBG_SEL_NIV_FORWARDING_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_VNTAG_ERRORf]
                = CTR_ING_DBG_SEL_VNTAG_ERROR;
        fid2enum[CTR_ING_DEBUG_SELECTt_MAC_LIMIT_DROPf]
                = CTR_ING_DBG_SEL_MAC_LIMIT_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_MAC_LIMIT_NODROPf]
                = CTR_ING_DBG_SEL_MAC_LIMIT_NODROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_DOS_L2f]
                = CTR_ING_DBG_SEL_DOS_L2;
        fid2enum[CTR_ING_DEBUG_SELECTt_CLASS_BASED_LEARN_DROPf]
                = CTR_ING_DBG_SEL_CLASS_BASED_LEARN_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_DST_DISCARDf]
                = CTR_ING_DBG_SEL_DST_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_FP_VLAN_DROPf]
                = CTR_ING_DBG_SEL_FP_VLAN_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_URPFf]
                = CTR_ING_DBG_SEL_URPF;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_HIGIG_NON_UCf]
                = CTR_ING_DBG_SEL_RX_HIGIG_NON_UC;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_HIGIG_UCf]
                = CTR_ING_DBG_SEL_RX_HIGIG_UC;
        fid2enum[CTR_ING_DEBUG_SELECTt_PARITY_ERR_DROPf]
                = CTR_ING_DBG_SEL_PARITY_ERR_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_TRUNK_FAILOVER_LOOPBACK_DISCARDf]
                = CTR_ING_DBG_SEL_TRUNK_FAILOVER_LOOPBACK_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_TRUNK_FAILOVER_LOOPBACKf]
                = CTR_ING_DBG_SEL_TRUNK_FAILOVER_LOOPBACK;
        fid2enum[CTR_ING_DEBUG_SELECTt_MC_INDEX_ERRf]
                = CTR_ING_DBG_SEL_MC_INDEX_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_HIGIG_ERRf]
                = CTR_ING_DBG_SEL_HIGIG_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_VLAN_DROPf]
                = CTR_ING_DBG_SEL_RX_VLAN_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_TNL_ERRf]
                = CTR_ING_DBG_SEL_RX_TNL_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_TNLf]
                = CTR_ING_DBG_SEL_RX_TNL;
        fid2enum[CTR_ING_DEBUG_SELECTt_L3_MTU_ERRf]
                = CTR_ING_DBG_SEL_L3_MTU_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_DOS_FRAGMENTf]
                = CTR_ING_DBG_SEL_DOS_FRAGMENT;
        fid2enum[CTR_ING_DEBUG_SELECTt_DOS_ICMPf]
                = CTR_ING_DBG_SEL_DOS_ICMP;
        fid2enum[CTR_ING_DEBUG_SELECTt_DOS_L4_HDR_ERRf]
                = CTR_ING_DBG_SEL_DOS_L4_HDR_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_HIGIG_MIRRORf]
                = CTR_ING_DBG_SEL_HIGIG_MIRROR;
        fid2enum[CTR_ING_DEBUG_SELECTt_DOS_L3_HDR_ERRf]
                = CTR_ING_DBG_SEL_DOS_L3_HDR_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_HIGIG_UNKNOWN_HDR_TYPEf]
                = CTR_ING_DBG_SEL_HIGIG_UNKNOWN_HDR_TYPE;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_HIGIG_IBPf]
                = CTR_ING_DBG_SEL_RX_HIGIG_IBP;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_HIGIG_HOLf]
                = CTR_ING_DBG_SEL_RX_HIGIG_HOL;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_HIGIG_PAUSEf]
                = CTR_ING_DBG_SEL_RX_HIGIG_PAUSE;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_DROPf]
                = CTR_ING_DBG_SEL_RX_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_MC_DROPf]
                = CTR_ING_DBG_SEL_RX_MC_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_FP_DROPf]
                = CTR_ING_DBG_SEL_FP_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_MC_BRIDGEDf]
                = CTR_ING_DBG_SEL_MC_BRIDGED;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_POLICY_DISCARDf]
                = CTR_ING_DBG_SEL_RX_POLICY_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_PORT_DROPf]
                = CTR_ING_DBG_SEL_RX_PORT_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_UCf]
                = CTR_ING_DBG_SEL_RX_UC;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_TM_BUFFER_DISCARDf]
                = CTR_ING_DBG_SEL_RX_TM_BUFFER_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_IPV6_MC_ROUTEDf]
                = CTR_ING_DBG_SEL_IPV6_MC_ROUTED;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_IPV6_HDR_ERRf]
                = CTR_ING_DBG_SEL_RX_IPV6_HDR_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_IPV6f]
                = CTR_ING_DBG_SEL_RX_IPV6;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_IPV6_DISCARDf]
                = CTR_ING_DBG_SEL_RX_IPV6_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_IPV4_MC_ROUTEDf]
                = CTR_ING_DBG_SEL_IPV4_MC_ROUTED;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_IPV4_HDR_ERRf]
                = CTR_ING_DBG_SEL_RX_IPV4_HDR_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_IPV4f]
                = CTR_ING_DBG_SEL_RX_IPV4;
        fid2enum[CTR_ING_DEBUG_SELECTt_RX_IPV4_DISCARDf]
                = CTR_ING_DBG_SEL_RX_IPV4_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_VLAN_MEMBERSHIP_DROPf]
                = CTR_ING_DBG_SEL_VLAN_MEMBERSHIP_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_VLAN_BLOCKED_DROPf]
                = CTR_ING_DBG_SEL_VLAN_BLOCKED_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_VLAN_STG_DROPf]
                = CTR_ING_DBG_SEL_VLAN_STG_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_NONUC_TRUNK_RESOLUTION_MASKf]
                = CTR_ING_DBG_SEL_NONUC_TRUNK_RESOLUTION_MASK;
        fid2enum[CTR_ING_DEBUG_SELECTt_NONUC_MASKf]
                = CTR_ING_DBG_SEL_NONUC_MASK;
        fid2enum[CTR_ING_DEBUG_SELECTt_MC_DROPf]
                = CTR_ING_DBG_SEL_MC_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_MAC_MASKf]
                = CTR_ING_DBG_SEL_MAC_MASK;
        fid2enum[CTR_ING_DEBUG_SELECTt_ING_EGR_MASKf]
                = CTR_ING_DBG_SEL_ING_EGR_MASK;
        fid2enum[CTR_ING_DEBUG_SELECTt_FP_ING_DELAYED_DROPf]
                = CTR_ING_DBG_SEL_FP_ING_DELAYED_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_FP_REDIRECT_MASKf]
                = CTR_ING_DBG_SEL_FP_REDIRECT_MASK;
        fid2enum[CTR_ING_DEBUG_SELECTt_EGR_MASKf]
                = CTR_ING_DBG_SEL_EGR_MASK;
        fid2enum[CTR_ING_DEBUG_SELECTt_SRC_PORT_KNOCKOUT_DROPf]
                = CTR_ING_DBG_SEL_SRC_PORT_KNOCKOUT_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_NON_IP_DISCARDf]
                = CTR_ING_DBG_SEL_NON_IP_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_MTU_CHECK_FAILf]
                = CTR_ING_DBG_SEL_MTU_CHECK_FAIL;
        fid2enum[CTR_ING_DEBUG_SELECTt_BLOCK_MASK_DROPf]
                = CTR_ING_DBG_SEL_BLOCK_MASK_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_PVLAN_VID_MISMATCHf]
                = CTR_ING_DBG_SEL_PVLAN_VID_MISMATCH;
        fid2enum[CTR_ING_DEBUG_SELECTt_INVALID_VLANf]
                = CTR_ING_DBG_SEL_INVALID_VLAN;
        fid2enum[CTR_ING_DEBUG_SELECTt_INVALID_TPIDf]
                = CTR_ING_DBG_SEL_INVALID_TPID;
        fid2enum[CTR_ING_DEBUG_SELECTt_PORT_ING_VLAN_MEMBERSHIPf]
                = CTR_ING_DBG_SEL_PORT_ING_VLAN_MEMBERSHIP;
        fid2enum[CTR_ING_DEBUG_SELECTt_VLAN_CC_OR_PBTf]
                = CTR_ING_DBG_SEL_VLAN_CC_OR_PBT;
        fid2enum[CTR_ING_DEBUG_SELECTt_VLAN_FPf]
                = CTR_ING_DBG_SEL_VLAN_FP;
        fid2enum[CTR_ING_DEBUG_SELECTt_MPLS_TTL_CHECK_FAILf]
                = CTR_ING_DBG_SEL_MPLS_TTL_CHECK_FAIL;
        fid2enum[CTR_ING_DEBUG_SELECTt_MPLS_LABEL_MISSf]
                = CTR_ING_DBG_SEL_MPLS_LABEL_MISS;
        fid2enum[CTR_ING_DEBUG_SELECTt_MPLS_INVALID_PAYLOADf]
                = CTR_ING_DBG_SEL_MPLS_INVALID_PAYLOAD;
        fid2enum[CTR_ING_DEBUG_SELECTt_MPLS_INVALID_CWf]
                = CTR_ING_DBG_SEL_MPLS_INVALID_CW;
        fid2enum[CTR_ING_DEBUG_SELECTt_MPLS_INVALID_ACTIONf]
                = CTR_ING_DBG_SEL_MPLS_INVALID_ACTION;
        fid2enum[CTR_ING_DEBUG_SELECTt_MPLS_GAL_LABELf]
                = CTR_ING_DBG_SEL_MPLS_GAL_LABEL;
        fid2enum[CTR_ING_DEBUG_SELECTt_TNL_DECAP_ECNf]
                = CTR_ING_DBG_SEL_TNL_DECAP_ECN;
        fid2enum[CTR_ING_DEBUG_SELECTt_TIME_SYNC_PKTf]
                = CTR_ING_DBG_SEL_TIME_SYNC_PKT;
        fid2enum[CTR_ING_DEBUG_SELECTt_TAG_UNTAG_DISCARDf]
                = CTR_ING_DBG_SEL_TAG_UNTAG_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_SRC_ROUTEf]
                = CTR_ING_DBG_SEL_SRC_ROUTE;
        fid2enum[CTR_ING_DEBUG_SELECTt_SPANNING_TREE_STATEf]
                = CTR_ING_DBG_SEL_SPANNING_TREE_STATE;
        fid2enum[CTR_ING_DEBUG_SELECTt_TNL_ERRORf]
                = CTR_ING_DBG_SEL_TNL_ERROR;
        fid2enum[CTR_ING_DEBUG_SELECTt_PROTECTION_DATA_DROPf]
                = CTR_ING_DBG_SEL_PROTECTION_DATA_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_NHOP_DROPf]
                = CTR_ING_DBG_SEL_NHOP_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_FP_ING_DROPf]
                = CTR_ING_DBG_SEL_FP_ING_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_FP_REDIRECT_DROPf]
                = CTR_ING_DBG_SEL_FP_REDIRECT_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_ECMP_RESOLUTION_ERRf]
                = CTR_ING_DBG_SEL_ECMP_RESOLUTION_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_PVLAN_VP_FILTERf]
                = CTR_ING_DBG_SEL_PVLAN_VP_FILTER;
        fid2enum[CTR_ING_DEBUG_SELECTt_PROTCOL_PKT_CTRL_DROPf]
                = CTR_ING_DBG_SEL_PROTCOL_PKT_CTRL_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_PORT_FILTERING_MODEf]
                = CTR_ING_DBG_SEL_PORT_FILTERING_MODE;
        fid2enum[CTR_ING_DEBUG_SELECTt_L2_MY_STATIONf]
                = CTR_ING_DBG_SEL_L2_MY_STATION;
        fid2enum[CTR_ING_DEBUG_SELECTt_MPLS_STAGEf]
                = CTR_ING_DBG_SEL_MPLS_STAGE;
        fid2enum[CTR_ING_DEBUG_SELECTt_SRC_MAC_ZEROf]
                = CTR_ING_DBG_SEL_SRC_MAC_ZERO;
        fid2enum[CTR_ING_DEBUG_SELECTt_L3_TTL_ERRf]
                = CTR_ING_DBG_SEL_L3_TTL_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_L3_HDR_ERRf]
                = CTR_ING_DBG_SEL_L3_HDR_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_DST_L3_LOOKUP_MISSf]
                = CTR_ING_DBG_SEL_DST_L3_LOOKUP_MISS;
        fid2enum[CTR_ING_DEBUG_SELECTt_SRC_L3_DISCARDf]
                = CTR_ING_DBG_SEL_SRC_L3_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_DST_L3_DISCARDf]
                = CTR_ING_DBG_SEL_DST_L3_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_SRC_L2_STATIC_MOVEf]
                = CTR_ING_DBG_SEL_SRC_L2_STATIC_MOVE;
        fid2enum[CTR_ING_DEBUG_SELECTt_SRC_L2_DISCARDf]
                = CTR_ING_DBG_SEL_SRC_L2_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_DST_L2_DISCARDf]
                = CTR_ING_DBG_SEL_DST_L2_DISCARD;
        fid2enum[CTR_ING_DEBUG_SELECTt_IP_MC_DROPf]
                = CTR_ING_DBG_SEL_IP_MC_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_SRC_MAC_EQUALS_DST_MACf]
                = CTR_ING_DBG_SEL_SRC_MAC_EQUALS_DST_MAC;
        fid2enum[CTR_ING_DEBUG_SELECTt_TRUNK_FAIL_LOOPBACKf]
                = CTR_ING_DBG_SEL_TRUNK_FAIL_LOOPBACK;
        fid2enum[CTR_ING_DEBUG_SELECTt_DOS_L4_ATTACKf]
                = CTR_ING_DBG_SEL_DOS_L4_ATTACK;
        fid2enum[CTR_ING_DEBUG_SELECTt_DOS_L3_ATTACKf]
                = CTR_ING_DBG_SEL_DOS_L3_ATTACK;
        fid2enum[CTR_ING_DEBUG_SELECTt_IPV6_PROTOCOL_ERRf]
                = CTR_ING_DBG_SEL_IPV6_PROTOCOL_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_IPV4_PROTOCOL_ERRf]
                = CTR_ING_DBG_SEL_IPV4_PROTOCOL_ERR;
        fid2enum[CTR_ING_DEBUG_SELECTt_MAC_CONTROL_FRAMEf]
                = CTR_ING_DBG_SEL_MAC_CONTROL_FRAME;
        fid2enum[CTR_ING_DEBUG_SELECTt_COMPOSITE_ERRORf]
                = CTR_ING_DBG_SEL_COMPOSITE_ERROR;
        fid2enum[CTR_ING_DEBUG_SELECTt_CPU_MANAGED_LEARNINGf]
                = CTR_ING_DBG_SEL_CPU_MANAGED_LEARNING;
        fid2enum[CTR_ING_DEBUG_SELECTt_CFI_OR_L3_DISABLEf]
                = CTR_ING_DBG_SEL_CFI_OR_L3_DISABLE;
        fid2enum[CTR_ING_DEBUG_SELECTt_BPDUf]
                = CTR_ING_DBG_SEL_BPDU;
        fid2enum[CTR_ING_DEBUG_SELECTt_BFD_TERMINATED_DROPf]
                = CTR_ING_DBG_SEL_BFD_TERMINATED_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_INBAND_TELEMETRY_VECTOR_MISS_MATCH_DROPf]
                = CTR_ING_DBG_SEL_INT_VECTOR_MISS_MATCH_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_INBAND_TELEMETRY_DATAPLANE_EXCEPTION_DROPf]
                = CTR_ING_DBG_SEL_INT_DP_EXCEPTION_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_VFI_ING_ADAPT_LOOKUP_MISS_DROPf]
                = CTR_ING_DBG_SEL_VFI_ING_ADAPT_LOOKUP_MISS_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_OVERLAY_UNDERLAY_NHOP_EIF_RANGE_ERROR_DROPf]
                = CTR_ING_DBG_SEL_OVERLAY_UNDERLAY_NHOP_EIF_RANGE_ERROR_DROP;
        fid2enum[CTR_ING_DEBUG_SELECTt_TNL_VXLAN_DECAP_SRC_IP_LOOKUP_MISS_DROPf]
                = CTR_ING_DBG_SEL_TNL_VXLAN_DECAP_SRC_IP_LOOKUP_MISS_DROP;
    } else if (sid == CTR_EGR_DEBUG_SELECTt) {
        fid2enum[CTR_EGR_DEBUG_SELECTt_PKT_TOO_SMALLf]
                = CTR_EGR_DBG_SEL_PKT_TOO_SMALL;
        fid2enum[CTR_EGR_DEBUG_SELECTt_INVALID_1588_PKTf]
                = CTR_EGR_DBG_SEL_INVALID_1588_PKT;
        fid2enum[CTR_EGR_DEBUG_SELECTt_NIV_PRUNEf]
                = CTR_EGR_DBG_SEL_NIV_PRUNE;
        fid2enum[CTR_EGR_DEBUG_SELECTt_PKT_TOO_LARGEf]
                = CTR_EGR_DBG_SEL_PKT_TOO_LARGE;
        fid2enum[CTR_EGR_DEBUG_SELECTt_MODID_GT_31f]
                = CTR_EGR_DBG_SEL_MODID_GT_31;
        fid2enum[CTR_EGR_DEBUG_SELECTt_IPMC_L3_SELF_INTFf]
                = CTR_EGR_DBG_SEL_IPMC_L3_SELF_INTF;
        fid2enum[CTR_EGR_DEBUG_SELECTt_PARITY_ERRf]
                = CTR_EGR_DBG_SEL_PARITY_ERR;
        fid2enum[CTR_EGR_DEBUG_SELECTt_L2_MTU_FAILf]
                = CTR_EGR_DBG_SEL_L2_MTU_FAIL;
        fid2enum[CTR_EGR_DEBUG_SELECTt_UNKNOWN_HIGIGf]
                = CTR_EGR_DBG_SEL_UNKNOWN_HIGIG;
        fid2enum[CTR_EGR_DEBUG_SELECTt_INVALID_HIGIG2_DST_PORTf]
                = CTR_EGR_DBG_SEL_INVALID_HIGIG2_DST_PORT;
        fid2enum[CTR_EGR_DEBUG_SELECTt_HIGIG_MCf]
                = CTR_EGR_DBG_SEL_HIGIG_MC;
        fid2enum[CTR_EGR_DEBUG_SELECTt_HIGIG_UCf]
                = CTR_EGR_DBG_SEL_HIGIG_UC;
        fid2enum[CTR_EGR_DEBUG_SELECTt_SRC_IP_LINK_LOCALf]
                = CTR_EGR_DBG_SEL_SRC_IP_LINK_LOCAL;
        fid2enum[CTR_EGR_DEBUG_SELECTt_MIRRORf]
                = CTR_EGR_DBG_SEL_MIRROR;
        fid2enum[CTR_EGR_DEBUG_SELECTt_PKT_DROPf]
                = CTR_EGR_DBG_SEL_PKT_DROP;
        fid2enum[CTR_EGR_DEBUG_SELECTt_L2_MC_DROPf]
                = CTR_EGR_DBG_SEL_L2_MC_DROP;
        fid2enum[CTR_EGR_DEBUG_SELECTt_STG_DROPf]
                = CTR_EGR_DBG_SEL_STG_DROP;
        fid2enum[CTR_EGR_DEBUG_SELECTt_VLAN_XLATE_MISSf]
                = CTR_EGR_DBG_SEL_VLAN_XLATE_MISS;
        fid2enum[CTR_EGR_DEBUG_SELECTt_INVALID_VLANf]
                = CTR_EGR_DBG_SEL_INVALID_VLAN;
        fid2enum[CTR_EGR_DEBUG_SELECTt_VLAN_TAGGEDf]
                = CTR_EGR_DBG_SEL_VLAN_TAGGED;
        fid2enum[CTR_EGR_DEBUG_SELECTt_CFI_DROPf]
                = CTR_EGR_DBG_SEL_CFI_DROP;
        fid2enum[CTR_EGR_DEBUG_SELECTt_TTL_DROPf]
                = CTR_EGR_DBG_SEL_TTL_DROP;
        fid2enum[CTR_EGR_DEBUG_SELECTt_TNL_ERRf]
                = CTR_EGR_DBG_SEL_TNL_ERR;
        fid2enum[CTR_EGR_DEBUG_SELECTt_TNL_PKTf]
                = CTR_EGR_DBG_SEL_TNL_PKT;
        fid2enum[CTR_EGR_DEBUG_SELECTt_IPMCV6_DROPf]
                = CTR_EGR_DBG_SEL_IPMCV6_DROP;
        fid2enum[CTR_EGR_DEBUG_SELECTt_IPMCV6_PKTf]
                = CTR_EGR_DBG_SEL_IPMCV6_PKT;
        fid2enum[CTR_EGR_DEBUG_SELECTt_IPV6_DROPf]
                = CTR_EGR_DBG_SEL_IPV6_DROP;
        fid2enum[CTR_EGR_DEBUG_SELECTt_IPV6_PKTf]
                = CTR_EGR_DBG_SEL_IPV6_PKT;
        fid2enum[CTR_EGR_DEBUG_SELECTt_IPMCV4_DROPf]
                = CTR_EGR_DBG_SEL_IPMCV4_DROP;
        fid2enum[CTR_EGR_DEBUG_SELECTt_IPMCV4_PKTf]
                = CTR_EGR_DBG_SEL_IPMCV4_PKT;
        fid2enum[CTR_EGR_DEBUG_SELECTt_IPV4_DROPf]
                = CTR_EGR_DBG_SEL_IPV4_DROP;
        fid2enum[CTR_EGR_DEBUG_SELECTt_IPV4_PKTf]
                = CTR_EGR_DBG_SEL_IPV4_PKT;
        fid2enum[CTR_EGR_DEBUG_SELECTt_LB_TO_LB_DROPf]
                = CTR_EGR_DBG_SEL_LB_TO_LB_DROP;
        fid2enum[CTR_EGR_DEBUG_SELECTt_CELL_TOO_SMALLf]
                = CTR_EGR_DBG_SEL_CELL_TOO_SMALL;
    }
}

/*!
 * \brief imm CTR_ING_DEBUG_SELECTt and CTR_EGR_DEBUG_SELECTt notification
 * input fields parsing.
 *
 * Parse imm CTR_ING_DEBUG_SELECTt and CTR_EGR_DEBUG_SELECTt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid logical table ID.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] idx Counter debug select index.
 * \param [out] trigger_array Trigger array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
ctr_dbgsel_lt_fields_parse(int unit,
                           bcmltd_sid_t sid,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           uint8_t *idx,
                           ctr_dbgsel_trigger_t *trigger_array)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    int i;
    ctr_dbgsel_trigger ingfid2enum[CTR_ING_DEBUG_SELECTt_FIELD_COUNT];
    ctr_dbgsel_trigger egrfid2enum[CTR_EGR_DEBUG_SELECTt_FIELD_COUNT];

    SHR_FUNC_ENTER(unit);

    if (sid == CTR_ING_DEBUG_SELECTt) {
        ctr_dbgsel_map_init(sid, ingfid2enum);
    } else if (sid == CTR_EGR_DEBUG_SELECTt) {
        ctr_dbgsel_map_init(sid, egrfid2enum);
    }

    /* Parse key field. */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        if (fid == CTR_ING_DEBUG_SELECTt_CTR_ING_DEBUG_SELECT_IDf ||
            fid == CTR_EGR_DEBUG_SELECTt_CTR_EGR_DEBUG_SELECT_IDf) {
            *idx = fval;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        gen_field = gen_field->next;
    }

    /* Parse data field. */
    i = 0;
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        trigger_array[i].enable = fval;
        if (sid == CTR_ING_DEBUG_SELECTt) {
            trigger_array[i].name = ingfid2enum[fid];
        } else if (sid == CTR_EGR_DEBUG_SELECTt) {
            trigger_array[i].name = egrfid2enum[fid];
        }
        gen_field = gen_field->next;
        i++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Do the internal hardware configuration for UPDATE operation.
 *
 * \param [in] unit Unit number.
 * \param [in] ingress Ingress/Egress switch.
 * \param [in] idx Debug register selector.
 * \param [in] trigger_array Trigger array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ctr_dbgsel_lt_update(int unit, bool ingress, int op, uint8_t idx,
                     ctr_dbgsel_trigger_t *trigger_array)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_dbgsel_set(unit, ingress, op, idx, trigger_array));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * IMM event handler
 */
/*!
 * \brief IMM logical table validate callback function.
 *
 * Validate the field values of the logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
ctr_sel_validate(int unit,
                 bcmltd_sid_t sid,
                 bcmimm_entry_event_t action,
                 const bcmltd_field_t *key,
                 const bcmltd_field_t *data,
                 void *context)
{
    SHR_FUNC_ENTER(unit);

    /* Validate the field on CTR_ING_DEBUG_SELECTt and CTR_EGR_DEBUG_SELECTt */
    if ((key->id != CTR_ING_DEBUG_SELECTt_CTR_ING_DEBUG_SELECT_IDf) ||
        (key->id != CTR_EGR_DEBUG_SELECTt_CTR_EGR_DEBUG_SELECT_IDf)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

static void
ctr_dbgsel_trigger_array_init(ctr_dbgsel_trigger_t *trigger_array)
{
    int i;
    for (i = 0; i < CTR_DBG_SEL_NUM; i++){
        trigger_array[i].enable = FALSE;
        trigger_array[i].name = CTR_DBG_SEL_NUM;
    }
}

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                      the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context This is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
ctr_sel_stage(int unit,
              bcmltd_sid_t sid,
              uint32_t trans_id,
              bcmimm_entry_event_t event,
              const bcmltd_field_t *key,
              const bcmltd_field_t *data,
              void *context,
              bcmltd_fields_t *output_fields)
{
    uint8_t idx = 0;
    bool ingress;
    int op;
    ctr_dbgsel_trigger_t trigger_array[CTR_DBG_SEL_NUM];

    SHR_FUNC_ENTER(unit);

    ctr_dbgsel_trigger_array_init(trigger_array);
    SHR_IF_ERR_VERBOSE_EXIT
        (ctr_dbgsel_lt_fields_parse(unit, sid, key, data,
                                    &idx, trigger_array));

    if (sid == CTR_ING_DEBUG_SELECTt) {
        ingress = 1;
    } else {
        /* CTR_EGR_DEBUG_SELECTt */
        ingress = 0;
    }

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            op = CTR_DBGSEL_INSERT;
            break;
        case BCMIMM_ENTRY_UPDATE:
            op = CTR_DBGSEL_UPDATE;
            break;
        case BCMIMM_ENTRY_DELETE:
            op = CTR_DBGSEL_DELETE;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ctr_dbgsel_lt_update(unit, ingress, op, idx, trigger_array));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR_ING_DEBUG_SELECT and CTR_EGR_DEBUG_SELECT In-memory event
 * callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to CTR_ING_DEBUG_SELECT and CTR_EGR_DEBUG_SELECT logical table entry
 * commit stages.
 */
static bcmimm_lt_cb_t event_hdl = {

    /*! Validate function. */
    .validate = ctr_sel_validate,

    /*! Staging function. */
    .stage = ctr_sel_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_ctr_dbgsel_imm_init(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for CTR_ING_DEBUG_SELECT LTs here.
     */
    rv = bcmlrd_map_get(unit, CTR_ING_DEBUG_SELECTt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 CTR_ING_DEBUG_SELECTt,
                                 &event_hdl,
                                 NULL));
    }
    /*
     * To register callback for CTR_EGR_DEBUG_SELECT LTs here.
     */
    rv = bcmlrd_map_get(unit, CTR_EGR_DEBUG_SELECTt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 CTR_EGR_DEBUG_SELECTt,
                                 &event_hdl,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

