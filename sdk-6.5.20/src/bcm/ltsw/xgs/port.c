/*! \file port.c
 *
 * Port Driver for XGS chips.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/port.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/port_int.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/mbcm/port.h>
#include <bcm_int/ltsw/xgs/port.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/obm.h>
#include <bcm_int/ltsw/feature.h>

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>

#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_PORT

#define MAX_TPID_CNT 4

typedef int (*profile_transfer_f)(const char *fld_name,
                                    void *prof, int set, int *val,
                                    const char **val_str,
                                    int index_offset, int index_num);

static int
xgs_pc_invert(int unit, int in_val, int *out_val);

static port_control_map_t xgs_port_control_port_map[] = {
    {bcmPortControlBridge, BCMI_PT_BRIDGE},
    {bcmPortControlMpls, BCMI_PT_MPLS},
    {bcmPortControlIP4, BCMI_PT_V4L3},
    {bcmPortControlIP6, BCMI_PT_V6L3},
    {bcmPortControlIP4Mcast, BCMI_PT_V4IPMC},
    {bcmPortControlIP6Mcast, BCMI_PT_V6IPMC},
    {bcmPortControlTrustIncomingVlan, BCMI_PT_TRUST_INCOMING_VID},
    /* {bcmPortControlDoNotCheckVlan, BCMI_PT_SKIP_VLAN_CHECK}, */
    {bcmPortControlFilterEgress, BCMI_PT_FP_EGR},
    {bcmPortControlFilterIngress, BCMI_PT_FP_ING},
    {bcmPortControlLoadBalancingNumber, BCMI_PT_LB_HASH_PORT_LB_NUM},
    {bcmPortControlEgressModifyDscp, BCMI_PT_TRUST_PHB_EGR_DSCP_MAP},
    {bcmPortControlEgressVlanPriUsesPktPri, BCMI_PT_TRUST_PHB_EGR_PORT_L2_OTAG,
     xgs_pc_invert, xgs_pc_invert},
    {bcmPortControlPacketTimeStampInsertRx, BCMI_PT_ING_RXTS_INSERT},
    {bcmPortControlPacketTimeStampInsertTx, BCMI_PT_EGR_TXTS_INSERT},
    {bcmPortControlPacketTimeStampSrcPortInsertCancel, BCMI_PT_ING_TS_CANCEL},
    {bcmPortControlPacketTimeStampDestPortInsertCancel, BCMI_PT_EGR_TS_CANCEL},
    {bcmPortControlPacketTimeStampRxId, BCMI_PT_ING_TS_OID},
    {bcmPortControlPacketTimeStampTxId, BCMI_PT_EGR_TS_OID},
    {bcmPortControlPassControlFrames, BCMI_PT_PASS_CTRL_FRAMES},
    {bcmPortControlFilterLookup, BCMI_PT_FP_VLAN},
    {bcmPortControlIEEE8021ASEnableIngress, BCMI_PT_IEEE_802_1AS_ENABLE},
    {bcmPortControlIfaEnable, BCMI_PT_IFA_ENABLE},
    {bcmPortControlIoamEnable, BCMI_PT_IOAM_ENABLE},
    {bcmPortControlIntEnable, BCMI_PT_INT_DP_ENABLE},
    {bcmPortControlPreservePacketPriority, BCMI_PT_VLAN_PHB},
    {bcmPortControlForwardStaticL2MovePkt, BCMI_PT_STATIC_MOVE_DROP_DISABLE},
    {bcmPortControlECMPLevel1LoadBalancingRandomizer, BCMI_PT_OFFSET_ECMP_RANDOM},
    {bcmPortControlECMPLevel2LoadBalancingRandomizer, BCMI_PT_OFFSET_ECMP_LVL2_RANDOM},
    {bcmPortControlTrunkLoadBalancingRandomizer, BCMI_PT_OFFSET_TRUNK_RANDOM},
    {bcmPortControlSampleIngressEnable, BCMI_PT_MIRROR_ING_ENCAP_SAMPLE},
    {bcmPortControlSampleEgressEnable, BCMI_PT_MIRROR_EGR_ENCAP_SAMPLE},
    {bcmPortControlFieldEgressClassSelect, BCMI_PT_EGR_CLASS_ID_SEL},
    {bcmPortControlIP4McastL2, BCMI_PT_PORT_SYSTEM_L2_FWD_IPMCV4},
    {bcmPortControlIP6McastL2, BCMI_PT_PORT_SYSTEM_L2_FWD_IPMCV6},
    {bcmPortControlModEnable, BCMI_PT_MIRROR_ON_DROP},
    {bcmPortControlModProfileId, BCMI_PT_TM_MIRROR_ON_DROP_PROFILE_ID},
    {bcmPortControlVxlanEnable, BCMI_PT_VXLAN_EN},
    {bcmPortControlVxlanDefaultTunnelEnable, BCMI_PT_VXLAN_SVP_DEFAULT_NETWORK},
    {bcmPortControlVxlanGportAssignmentCriteria, BCMI_PT_VXLAN_SVP_KEY_MODE},
    {bcmPortControl1588P2PDelay, BCMI_PT_1588_LINK_DELAY, NULL, NULL},
    {bcmPortControlEgressL2OpaqueTag, BCMI_PT_EGR_OPAQUE_TAG, NULL, NULL},

    /* PM controls */
    {bcmPortControlLinkFaultLocal, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlLinkFaultRemote, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlLinkFaultLocalEnable, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlLinkFaultRemoteEnable, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlRxEnable, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlTxEnable, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlStatOversize, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlPFCEthertype, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlPFCOpcode, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlPFCReceive, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlPFCTransmit, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlPFCRefreshTime, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlPFCXOffTime, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlPFCDestMacOui, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlPFCDestMacNonOui, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlPFCPassFrames, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlPassControlFrames, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlRuntThreshold, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlMmuTrafficEnable, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlTxStall, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
};

static const char* port_type_list[] = {
    [XGS_PORT_TYPE_ETHERNET]    = ETHERNETs,
    [XGS_PORT_TYPE_LOOPBACK]    = LOOPBACKs,
};
static const ltsw_port_tab_sym_list_t port_type_map = {
    .len    = XGS_PORT_TYPE_CNT,
    .syms   = port_type_list,
};



static const char* op_mode_list[] = {
    [OP_MODE_NORMAL]    = NORMALs,
    [OP_MODE_L3_IIF]    = L3_IIFs,
    [OP_MODE_VRF]       = VRFs,
    [OP_MODE_VLAN]      = VLANs,
};
static const ltsw_port_tab_sym_list_t op_mode_map = {
    .len    = OP_MODE_CNT,
    .syms   = op_mode_list,
};

static const char* fp_class_id_sel_list[] = {
    [FP_CLASS_ID_SEL_INVALID]    = INVALIDs,
    [FP_CLASS_ID_SEL_PORT]       = PORTs,
    [FP_CLASS_ID_SEL_ALPM_COMPRESSION_SRC] = ALPM_COMPRESSION_SRCs,
    [FP_CLASS_ID_SEL_L3_IIF]     = L3_IIFs,
    [FP_CLASS_ID_SEL_VFP_HI]     = VFP_HIs,
    [FP_CLASS_ID_SEL_VFP_LO]     = VFP_LOs,
    [FP_CLASS_ID_SEL_L2_SRC]     = L2_SRCs,
    [FP_CLASS_ID_SEL_L2_DST]     = L2_DSTs,
    [FP_CLASS_ID_SEL_L3_SRC]     = L3_SRCs,
    [FP_CLASS_ID_SEL_L3_DST]     = L3_DSTs,
    [FP_CLASS_ID_SEL_VLAN]       = VLANs,
    [FP_CLASS_ID_SEL_VRF]        = VRFs,
    [FP_CLASS_ID_SEL_ALPM_COMPRESSION_DST] = ALPM_COMPRESSION_DSTs,
    [FP_CLASS_ID_SEL_EM]         = EMs,
    [FP_CLASS_ID_SEL_IFP]        = IFPs,
};
static const ltsw_port_tab_sym_list_t fp_class_id_sel_map = {
    .len    = FP_CLASS_ID_SEL_CNT,
    .syms   = fp_class_id_sel_list,
};

static const char* xgs_shaper_mode_list[] = {
    [SHAPER_MODE_BYTE]  = BYTE_MODEs,
    [SHAPER_MODE_PKT]   = PACKET_MODEs,
};
static const ltsw_port_tab_sym_list_t xgs_shaper_mode_map = {
    .len    = SHAPER_MODE_CNT,
    .syms   = xgs_shaper_mode_list,
};

#define MAX_NUM_LTS         32

static const ltsw_port_tab_lt_info_t port                   = {PORTs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_membership_policy = {PORT_MEMBERSHIP_POLICYs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_policy            = {PORT_POLICYs, PORT_IDs};
/*static const ltsw_port_tab_lt_info_t port_vlan_xlate        = {PORT_VLAN_XLATEs, PORT_IDs};*/
static const ltsw_port_tab_lt_info_t port_learn             = {PORT_LEARNs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_bridge            = {PORT_BRIDGEs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_fp                = {PORT_FPs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_lb                = {PORT_LBs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_phb               = {PORT_PHBs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_ing_visibility    = {PORT_ING_VISIBILITYs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_egr_visibility    = {PORT_EGR_VISIBILITYs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_pvlan             = {PORT_PVLANs, PORT_IDs};
static const ltsw_port_tab_lt_info_t tm_shaper_port         = {TM_SHAPER_PORTs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_system            = {PORT_SYSTEMs, PORT_SYSTEM_IDs};
static const ltsw_port_tab_lt_info_t mirror_port_encap_sf   = {MIRROR_PORT_ENCAP_SFLOWs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_tm_mirror_on_drop = {PORT_TM_MIRROR_ON_DROPs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_ecn               = {PORT_ECNs, PORT_IDs};

static const char *port_tab_lt_list[BCM_MAX_NUM_UNITS][MAX_NUM_LTS];
static const char *port_tab_lt_key_list[BCM_MAX_NUM_UNITS][MAX_NUM_LTS];
static int port_tab_lt_num[BCM_MAX_NUM_UNITS];

static ltsw_port_tab_info_t port_tab_map_def[BCMI_PT_CNT] = {
    [BCMI_PT_PORT_TYPE]                         = {&port, PORT_TYPEs, FLD_IS_SYMBOL, &port_type_map},
    [BCMI_PT_VLAN_ASSIGNMENT_BASED_MAC]         = {&port, VLAN_ASSIGNMENT_BASED_MACs, SYNC_TO_LPORT},
    [BCMI_PT_VLAN_ASSIGNMENT_BASED_IPV4]        = {&port, VLAN_ASSIGNMENT_BASED_IPV4s, SYNC_TO_LPORT},
    [BCMI_PT_OPERATING_MODE]                    = {&port, OPERATING_MODEs, FLD_IS_SYMBOL | SYNC_TO_LPORT, &op_mode_map},
    [BCMI_PT_MPLS]                              = {&port, MPLSs, SYNC_TO_LPORT},
    [BCMI_PT_V4IPMC]                            = {&port, V4IPMCs, SYNC_TO_LPORT},
    [BCMI_PT_V4L3]                              = {&port, V4L3s, SYNC_TO_LPORT},
    [BCMI_PT_V6IPMC]                            = {&port, V6IPMCs, SYNC_TO_LPORT},
    [BCMI_PT_V6L3]                              = {&port, V6L3s, SYNC_TO_LPORT},
    [BCMI_PT_MTU_EN]                            = {&port, MTU_CHECKs},
    [BCMI_PT_MTU]                               = {&port, MTUs},
    [BCMI_PT_IPMC_USE_L3_IIF]                   = {&port, IPMC_USE_L3_IIFs, SYNC_TO_LPORT},
    [BCMI_PT_IEEE_802_1AS_ENABLE]               = {&port, IEEE_802_1ASs, SYNC_TO_LPORT},
    [BCMI_PT_VLAN_ING_TAG_ACTION_PROFILE_ID]    = {&port, VLAN_ING_TAG_ACTION_PROFILE_IDs, SYNC_TO_LPORT},
    [BCMI_PT_ING_OPRI]                          = {&port, ING_OPRIs },
    [BCMI_PT_ING_OVID]                          = {&port, ING_OVIDs, SYNC_TO_LPORT},
    [BCMI_PT_ING_OCFI]                          = {&port, ING_OCFIs, SYNC_TO_LPORT},
    [BCMI_PT_VLAN_EGR_TAG_ACTION_PROFILE_ID]    = {&port, VLAN_EGR_TAG_ACTION_PROFILE_IDs},
    [BCMI_PT_EGR_OPRI]                          = {&port, EGR_OPRIs},
    [BCMI_PT_EGR_OVID]                          = {&port, EGR_OVIDs},
    [BCMI_PT_EGR_OCFI]                          = {&port, EGR_OCFIs},
    [BCMI_PT_IFA_ENABLE]                        = {&port, INBAND_TELEMETRY_IFAs, SYNC_TO_LPORT},
    [BCMI_PT_IOAM_ENABLE]                       = {&port, INBAND_TELEMETRY_IOAMs, SYNC_TO_LPORT},
    [BCMI_PT_INT_DP_ENABLE]                     = {&port, INBAND_TELEMETRY_DATAPLANEs, SYNC_TO_LPORT},
    [BCMI_PT_ING_FLEX_CTR_ID]                   = {&port, CTR_ING_EFLEX_ACTIONs, SYNC_TO_LPORT},
    [BCMI_PT_EGR_FLEX_CTR_ID]                   = {&port, CTR_EGR_EFLEX_ACTIONs},
    [BCMI_PT_IPBM_INDEX]                        = {&port, INPORT_BITMAP_INDEXs, SYNC_TO_LPORT},
    [BCMI_PT_PORT_PKT_CONTROL_ID]               = {&port, PORT_PKT_CONTROL_IDs },
    [BCMI_PT_EGR_VLAN_MEMBERSHIP_CHECK]         = {&port_membership_policy, EGR_VLAN_MEMBERSHIP_CHECKs, },
    [BCMI_PT_ING_VLAN_MEMBERSHIP_CHECK]         = {&port_membership_policy, ING_VLAN_MEMBERSHIP_CHECKs, SYNC_TO_LPORT},
    [BCMI_PT_DEVICE_TS_PTP_MSG_CONTROL_PROFILE_ID] = {&port, DEVICE_TS_PTP_MSG_CONTROL_PROFILE_IDs, SYNC_TO_LPORT},
    [BCMI_PT_NTP_TC]                            = {&port, NTP_TCs, SYNC_TO_LPORT},
    /* [BCMI_PT_SKIP_VLAN_CHECK]                   = {&port_membership_policy, SKIP_VLAN_CHECKs, }, */
    [BCMI_PT_TRUST_INCOMING_VID]                = {&port_policy, TRUST_INCOMING_VIDs, SYNC_TO_LPORT},
    [BCMI_PT_PASS_ON_OUTER_TPID_MATCH]          = {&port_policy, PASS_ON_OUTER_TPID_MATCHs, FLD_IS_ARRAY},
    [BCMI_PT_VLAN_OUTER_TPID_ID]                = {&port_policy, VLAN_OUTER_TPID_IDs},
    [BCMI_PT_PASS_ON_EGR_OUTER_TPID_MATCH]      = {&port_policy, PASS_ON_EGR_OUTER_TPID_MATCHs, FLD_IS_ARRAY},
    [BCMI_PT_ING_VLAN_OUTER_TPID_ID]            = {&port_policy, ING_VLAN_OUTER_TPID_IDs},
    [BCMI_PT_PORT_COS_QUEUE_MAP_ID]             = {&port_policy, PORT_COS_Q_MAP_IDs},
    [BCMI_PT_PASS_CTRL_FRAMES]                  = {&port_policy, PASS_PAUSE_FRAMESs, SYNC_TO_LPORT},
    [BCMI_PT_EGR_CFI_AS_CNG]                    = {&port_policy, EGR_CFI_AS_CNGs, FLD_IS_ARRAY},
    [BCMI_PT_ING_CFI_AS_CNG]                    = {&port_policy, ING_CFI_AS_CNGs, FLD_IS_ARRAY},
    [BCMI_PT_DROP_BPDU]                         = {&port_policy, DROP_BPDUs, SYNC_TO_LPORT},
    [BCMI_PT_DROP_UNTAG]                        = {&port_policy, DROP_UNTAGs },
    [BCMI_PT_DROP_TAG]                          = {&port_policy, DROP_TAGs },
    /*
    [BCMI_PT_PORT_GRP]                          = {&port_vlan_xlate, PORT_GRPs, },
    [BCMI_PT_OPRI_MAPPED]                       = {&port_vlan_xlate, OPRI_MAPPEDs, FLD_IS_ARRAY},
    [BCMI_PT_OCFI_MAPPED]                       = {&port_vlan_xlate, OCFI_MAPPEDs, FLD_IS_ARRAY},
    [BCMI_PT_IPRI_MAPPED]                       = {&port_vlan_xlate, IPRI_MAPPEDs, FLD_IS_ARRAY},
    [BCMI_PT_ICFI_MAPPED]                       = {&port_vlan_xlate, ICFI_MAPPEDs, FLD_IS_ARRAY},
    [BCMI_PT_ING_XLATE_TBL_SEL_FIRST_LOOKUP]    = {&port_vlan_xlate, ING_XLATE_TBL_SEL_FIRST_LOOKUPs, FLD_IS_SYMBOL, &ing_xlate_map},
    [BCMI_PT_ING_XLATE_TBL_SEL_SECOND_LOOKUP]   = {&port_vlan_xlate, ING_XLATE_TBL_SEL_SECOND_LOOKUPs, FLD_IS_SYMBOL, &ing_xlate_map},
    */
    /* [BCMI_PT_MAC_LEARN_AS_PENDING]              = {&port_learn, MAC_LEARN_AS_PENDINGs, }, */
    [BCMI_PT_MAC_LEARN]                         = {&port_learn, MAC_LEARNs, SYNC_TO_LPORT},
    [BCMI_PT_MAC_COPY_TO_CPU]                   = {&port_learn, MAC_COPY_TO_CPUs },
    [BCMI_PT_MAC_DROP]                          = {&port_learn, MAC_DROPs },
    [BCMI_PT_STATIC_MOVE_DROP_DISABLE]          = {&port_learn, STATIC_MOVE_DROP_DISABLEs },
    [BCMI_PT_MAC_MOVE]                          = {&port_learn, MAC_MOVEs},
    [BCMI_PT_MAC_MOVE_COPY_TO_CPU]              = {&port_learn, MAC_MOVE_COPY_TO_CPUs },
    [BCMI_PT_MAC_MOVE_DROP]                     = {&port_learn, MAC_MOVE_DROPs },
    [BCMI_PT_BRIDGE]                            = {&port_bridge, BRIDGEs, SYNC_TO_LPORT},
    [BCMI_PT_FP_EGR]                            = {&port_fp, FP_EGRs, },
    [BCMI_PT_FP_ING]                            = {&port_fp, FP_INGs, SYNC_TO_LPORT},
    [BCMI_PT_FP_VLAN]                           = {&port_fp, FP_VLANs, SYNC_TO_LPORT},
    [BCMI_PT_ING_PORT_GROUP]                    = {&port_fp, FP_VLAN_PORT_GRPs, SYNC_TO_LPORT},
    [BCMI_PT_EGR_PORT_GROUP]                    = {&port_fp, FP_EGR_PORT_GRPs,},
    [BCMI_PT_ING_GRP_SEL_CLASS_ID]              = {&port_fp, FP_ING_GRP_SEL_CLASS_IDs, SYNC_TO_LPORT},
    [BCMI_PT_EGR_CLASS_ID_SEL]                  = {&port_fp, FP_EGR_CLASS_ID_SELECTs, FLD_IS_SYMBOL, &fp_class_id_sel_map},
    [BCMI_PT_LB_HASH_PORT_LB_NUM]               = {&port_lb, LB_HASH_PORT_LB_NUMs },
    [BCMI_PT_OFFSET_ECMP_RANDOM]                = {&port_lb, OFFSET_ECMP_RANDOMs },
    [BCMI_PT_OFFSET_TRUNK_RANDOM]               = {&port_lb, OFFSET_TRUNK_RANDOMs },
    [BCMI_PT_TRUST_PHB_EGR_DSCP_MAP]            = {&port_phb, TRUST_PHB_EGR_DSCP_MAPs, },
    [BCMI_PT_TRUST_PHB_EGR_PORT_L2_OTAG]        = {&port_phb, TRUST_PHB_EGR_PORT_L2_OTAGs, },
    [BCMI_PT_TRUST_PHB_ING_DSCP_V4]             = {&port_phb, TRUST_PHB_ING_DSCP_V4s },
    [BCMI_PT_TRUST_PHB_ING_DSCP_V6]             = {&port_phb, TRUST_PHB_ING_DSCP_V6s },
    [BCMI_PT_VLAN_PHB]                          = {&port_phb, PHB_ING_L2_OTAG_REMAP_IDs, SYNC_TO_LPORT},
    [BCMI_PT_DSCP_PHB]                          = {&port_phb, PHB_ING_IP_DSCP_TO_INT_PRI_REMAP_IDs},
    [BCMI_PT_DOT1P_PHB]                         = {&port_phb, PHB_ING_L2_IDs},
    [BCMI_PT_ING_RXTS_INSERT]                   = {&port_ing_visibility, TIMESTAMPs},
    [BCMI_PT_EGR_TXTS_INSERT]                   = {&port_egr_visibility, TIMESTAMPs},
    [BCMI_PT_ING_TS_CANCEL]                     = {&port_ing_visibility, SKIP_TIMESTAMPs},
    [BCMI_PT_EGR_TS_CANCEL]                     = {&port_egr_visibility, SKIP_TIMESTAMPs},
    [BCMI_PT_ING_TS_OID]                        = {&port_ing_visibility, TIMESTAMP_ORIGINs},
    [BCMI_PT_EGR_TS_OID]                        = {&port_egr_visibility, TIMESTAMP_ORIGINs},
    [BCMI_PT_ING_PVLAN]                         = {&port_pvlan, ING_PVLANs },
    [BCMI_PT_EGR_PVLAN]                         = {&port_pvlan, EGR_PVLANs},
    [BCMI_PT_PVLAN_EGR_UNTAG]                   = {&port_pvlan, EGR_UNTAGs},
    [BCMI_PT_PVLAN_EGR_REPLACE_PRI]             = {&port_pvlan, EGR_REPLACE_PRIs},
    [BCMI_PT_PVLAN_EGR_PRI]                     = {&port_pvlan, EGR_PRIs},
    [BCMI_PT_PVLAN_EGR_VID]                     = {&port_pvlan, EGR_VIDs},
    [BCMI_PT_SHAPER_IFG_ADJ]                    = {&tm_shaper_port, INTER_FRAME_GAP_BYTEs},
    [BCMI_PT_SHAPER_IFG_ADJ_EN]                 = {&tm_shaper_port, INTER_FRAME_GAP_ENCAPs},

    [BCMI_PT_SHAPER_MODE]                       = {&tm_shaper_port, SHAPING_MODEs, FLD_IS_SYMBOL, &xgs_shaper_mode_map},
    [BCMI_PT_SHAPER_RATE]                       = {&tm_shaper_port, BANDWIDTH_KBPSs},
    [BCMI_PT_SHAPER_RATE_REAL]                  = {&tm_shaper_port, BANDWIDTH_KBPS_OPERs},
    [BCMI_PT_SHAPER_BURST]                      = {&tm_shaper_port, BURST_SIZE_KBITSs},
    [BCMI_PT_SHAPER_BURST_REAL]                 = {&tm_shaper_port, BURST_SIZE_KBITS_OPERs},
    [BCMI_PT_SHAPER_BURST_AUTO]                 = {&tm_shaper_port, BURST_SIZE_AUTOs},

    [BCMI_PT_MIRROR_ING_ENCAP_SAMPLE]           = {&mirror_port_encap_sf, SAMPLE_INGs},
    [BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_RATE]      = {&mirror_port_encap_sf, SAMPLE_ING_RATEs},
    [BCMI_PT_MIRROR_EGR_ENCAP_SAMPLE]           = {&mirror_port_encap_sf, SAMPLE_EGRs},
    [BCMI_PT_MIRROR_EGR_ENCAP_SAMPLE_RATE]      = {&mirror_port_encap_sf, SAMPLE_EGR_RATEs},
    [BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_CPU]       = {&mirror_port_encap_sf, SAMPLE_ING_CPUs},
    [BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_INSTANCE]  = {&mirror_port_encap_sf, SAMPLE_ING_MIRROR_INSTANCEs},
    [BCMI_PT_PORT_SYSTEM_OPERATING_MODE]        = {&port_system, OPERATING_MODEs, FLD_IS_SYMBOL, &op_mode_map},
    [BCMI_PT_PORT_SYSTEM_L3_IIF_ID]             = {&port_system, L3_IIF_IDs},
    [BCMI_PT_PORT_SYSTEM_VRF_ID]                = {&port_system, VRF_IDs},
    [BCMI_PT_PORT_SYSTEM_L2_FWD_IPMCV4]         = {&port_system, L2_FWD_IPMCV4s},
    [BCMI_PT_PORT_SYSTEM_L2_FWD_IPMCV6]         = {&port_system, L2_FWD_IPMCV6s},
    [BCMI_PT_PORT_SYSTEM_PROFILE_ID]            = {&port_system, PORT_SYSTEM_PROFILE_IDs},
    [BCMI_PT_PORT_SYSTEM_FP_VLAN_PORT_GRP]      = {&port_system, FP_VLAN_PORT_GRPs},
    [BCMI_PT_PORT_SYSTEM_CLASS_ID]              = {&port_system, CLASS_IDs},
    [BCMI_PT_PORT_SYSTEM_BLOCKED_EGR_PORTS]     = {&port_system, BLOCKED_EGR_PORTSs, FLD_IS_ARRAY},
    [BCMI_PT_PORT_SYSTEM_IPBM_INDEX]            = {&port_system, INPORT_BITMAP_INDEXs},
    [BCMI_PT_ECN_IP_TO_MPLS_EXP_PRI]            = {&port_system, ECN_IP_TO_MPLS_EXP_PRIORITYs},
    [BCMI_PT_ECN_IP_TO_MPLS_EXP_ID]             = {&port_system, ECN_IP_TO_MPLS_EXP_IDs},
    [BCMI_PT_ECN_CNG_TO_MPLS_EXP_PRI]           = {&port_system, ECN_CNG_TO_MPLS_EXP_PRIORITYs},
    [BCMI_PT_ECN_CNG_TO_MPLS_EXP_ID]            = {&port_system, ECN_CNG_TO_MPLS_EXP_IDs},
    [BCMI_PT_ECN_LATENCY_PROFILE_ID]            = {&port_ecn, ECN_LATENCY_PROFILE_IDs},

    [BCMI_PT_OUTER_TPID_VERIFY]                 = {&port_policy, OUTER_TPID_VERIFYs, SYNC_TO_LPORT},
    [BCMI_PT_USE_TABLE_VLAN_OUTER_TPID_ID]      = {&port_policy, USE_TABLE_VLAN_OUTER_TPID_IDs},
    [BCMI_PT_MIRROR_ON_DROP]                    = {&port_tm_mirror_on_drop, MIRROR_ON_DROPs},
    [BCMI_PT_TM_MIRROR_ON_DROP_PROFILE_ID]      = {&port_tm_mirror_on_drop, TM_MIRROR_ON_DROP_PROFILE_IDs},
};

static ltsw_port_tab_info_t *port_tab_map[BCM_MAX_NUM_UNITS];

static const ltsw_port_tab_lt_info_t lport_membership_policy = {PORT_SYSTEM_MEMBERSHIP_POLICY_PROFILEs, PORT_SYSTEM_PROFILE_IDs};
static const ltsw_port_tab_lt_info_t lport_pvlan             = {PORT_SYSTEM_PVLAN_PROFILEs, PORT_SYSTEM_PROFILE_IDs};
static const ltsw_port_tab_lt_info_t lport_phb               = {PORT_SYSTEM_PHB_PROFILEs, PORT_SYSTEM_PROFILE_IDs};
static const ltsw_port_tab_lt_info_t lport_learn             = {PORT_SYSTEM_LEARN_PROFILEs, PORT_SYSTEM_PROFILE_IDs};
static const ltsw_port_tab_lt_info_t lport_bridge            = {PORT_SYSTEM_BRIDGE_PROFILEs, PORT_SYSTEM_PROFILE_IDs};
static const ltsw_port_tab_lt_info_t lport_policy            = {PORT_SYSTEM_POLICY_PROFILEs, PORT_SYSTEM_PROFILE_IDs};
static const ltsw_port_tab_lt_info_t lport_fp                = {PORT_SYSTEM_FP_PROFILEs, PORT_SYSTEM_PROFILE_IDs};
static const ltsw_port_tab_lt_info_t lport                   = {PORT_SYSTEM_PROFILEs, PORT_SYSTEM_PROFILE_IDs};
static const ltsw_port_tab_lt_info_t lport_tm_mirror_on_drop = {PORT_SYSTEM_TM_MIRROR_ON_DROP_PROFILEs, PORT_SYSTEM_PROFILE_IDs};

/* Depth of array field */
#define DEPTH_ARRAY_FIELD   4

static const char *lport_tab_lt_list[BCM_MAX_NUM_UNITS][MAX_NUM_LTS];
static int lport_tab_lt_num[BCM_MAX_NUM_UNITS];

static ltsw_port_tab_info_t lport_tab_map_def[BCMI_PT_CNT] = {
    [BCMI_PT_ING_VLAN_MEMBERSHIP_CHECK]         = {&lport_membership_policy, ING_VLAN_MEMBERSHIP_CHECKs,},
    [BCMI_PT_ING_PVLAN]                         = {&lport_pvlan, ING_PVLANs},
    [BCMI_PT_VLAN_PHB]                          = {&lport_phb, PHB_ING_L2_OTAG_REMAP_IDs},
    [BCMI_PT_TRUST_PHB_ING_DSCP_V4]             = {&lport_phb, TRUST_PHB_ING_DSCP_V4s},
    [BCMI_PT_TRUST_PHB_ING_DSCP_V6]             = {&lport_phb, TRUST_PHB_ING_DSCP_V6s},
    [BCMI_PT_MAC_DROP]                          = {&lport_learn, MAC_DROPs, },
    [BCMI_PT_MAC_COPY_TO_CPU]                   = {&lport_learn, MAC_COPY_TO_CPUs, },
    [BCMI_PT_MAC_LEARN]                         = {&lport_learn, MAC_LEARNs, },
    [BCMI_PT_BRIDGE]                            = {&lport_bridge, BRIDGEs, },
    [BCMI_PT_TRUST_INCOMING_VID]                = {&lport_policy, TRUST_INCOMING_VIDs, },
    [BCMI_PT_PASS_CTRL_FRAMES]                  = {&lport_policy, PASS_PAUSE_FRAMESs},
    [BCMI_PT_PASS_ON_OUTER_TPID_MATCH]          = {&lport_policy, PASS_ON_OUTER_TPID_MATCHs, FLD_IS_ARRAY},
    [BCMI_PT_DROP_BPDU]                         = {&lport_policy, DROP_BPDUs},
    [BCMI_PT_DROP_UNTAG]                        = {&lport_policy, DROP_UNTAGs},
    [BCMI_PT_DROP_TAG]                          = {&lport_policy, DROP_TAGs},
    [BCMI_PT_OUTER_TPID_VERIFY]                 = {&lport_policy, OUTER_TPID_VERIFYs},
    [BCMI_PT_FP_VLAN]                           = {&lport_fp, FP_VLANs},
    [BCMI_PT_FP_ING]                            = {&lport_fp, FP_INGs, },
    [BCMI_PT_ING_PORT_GROUP]                    = {&lport_fp, FP_VLAN_PORT_GRPs,},
    [BCMI_PT_ING_GRP_SEL_CLASS_ID]              = {&lport_fp, FP_ING_GRP_SEL_CLASS_IDs,},
    [BCMI_PT_V6L3]                              = {&lport, V6L3s, },
    [BCMI_PT_V6IPMC]                            = {&lport, V6IPMCs, },
    [BCMI_PT_V4L3]                              = {&lport, V4L3s, },
    [BCMI_PT_V4IPMC]                            = {&lport, V4IPMCs, },
    [BCMI_PT_OPERATING_MODE]                    = {&lport, OPERATING_MODEs, FLD_IS_SYMBOL, &op_mode_map},
    [BCMI_PT_IEEE_802_1AS_ENABLE]               = {&lport, IEEE_802_1ASs},
    [BCMI_PT_VLAN_ING_TAG_ACTION_PROFILE_ID]    = {&lport, VLAN_ING_TAG_ACTION_PROFILE_IDs},
    [BCMI_PT_ING_OVID]                          = {&lport, ING_OVIDs},
    [BCMI_PT_ING_OPRI]                          = {&lport, ING_OPRIs},
    [BCMI_PT_ING_OCFI]                          = {&lport, ING_OCFIs},
    [BCMI_PT_IFA_ENABLE]                        = {&lport, INBAND_TELEMETRY_IFAs},
    [BCMI_PT_IOAM_ENABLE]                       = {&lport, INBAND_TELEMETRY_IOAMs},
    [BCMI_PT_INT_DP_ENABLE]                     = {&lport, INBAND_TELEMETRY_DATAPLANEs},
    [BCMI_PT_MPLS]                              = {&lport, MPLSs, },
    [BCMI_PT_IPMC_USE_L3_IIF]                   = {&lport, IPMC_USE_L3_IIFs},
    [BCMI_PT_ING_FLEX_CTR_ID]                   = {&lport, CTR_ING_EFLEX_ACTIONs},
    [BCMI_PT_IPBM_INDEX]                        = {&lport, INPORT_BITMAP_INDEXs, SYNC_TO_LPORT},
    [BCMI_PT_MIRROR_ON_DROP]                    = {&lport_tm_mirror_on_drop, MIRROR_ON_DROPs},
    [BCMI_PT_TM_MIRROR_ON_DROP_PROFILE_ID]      = {&lport_tm_mirror_on_drop, TM_MIRROR_ON_DROP_PROFILE_IDs},
    [BCMI_PT_DEVICE_TS_PTP_MSG_CONTROL_PROFILE_ID] = {&lport, DEVICE_TS_PTP_MSG_CONTROL_PROFILE_IDs},
    [BCMI_PT_NTP_TC]                            = {&lport, NTP_TCs},
};

static ltsw_port_tab_info_t *lport_tab_map[BCM_MAX_NUM_UNITS];

#define XGS_COLOR_GREEN   0
#define XGS_COLOR_YELLOW  3
#define XGS_COLOR_RED     1
#define XGS_COLOR_NONE    0
#define XGS_COLOR_ENCODING(color) \
          (((color) == bcmColorGreen) ? XGS_COLOR_GREEN : \
            (((color) == bcmColorYellow) ? XGS_COLOR_YELLOW : \
             (((color) == bcmColorRed) ? XGS_COLOR_RED :  XGS_COLOR_NONE)))

typedef struct xgs_port_profile_s {
    ltsw_port_profile_info_t    port_pkt_ctrl;
    bool                        port_system_mode;
    ltsw_port_profile_info_t    port_system[MAX_INST_NUM_LPORT_TAB];
} xgs_port_profile_t;

static xgs_port_profile_t *xgs_port_profile[BCM_MAX_NUM_UNITS];

#define PROFILE_PORT_PKT_CTRL(_u)       (&(xgs_port_profile[(_u)]->port_pkt_ctrl))
#define PROFILE_PORT_SYSTEM(_u, p)      (&(xgs_port_profile[(_u)]->port_system[p]))
#define PORT_SYSTEM_PROFILE_MODE(_u)    (xgs_port_profile[(_u)]->port_system_mode)

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Callback function to invert port control option value.
 *
 * \param [in] unit Unit number.
 * \param [in] in_val Input value.
 * \param [out] out_val Output value.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xgs_pc_invert(int unit, int in_val, int *out_val)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out_val, SHR_E_PARAM);

    *out_val = !(in_val);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Profile data of PORT_PKT_CONTROL is saved into pkt_ctrl_prof.
 * Save data gotten from LT field to pkt_ctrl_prof. Or get data from pkt_ctrl_prof,
 * and then update the data to LT fields.
 *
 * \param [in] fld_name LT data field name.
 * \param [in] prof Profile data.
 * \param [in] set 1: save data to profile entry, 0: get data from profile entry.
 * \param [in/out] val Data come from profile entry, or will be saved to profile entry.
 * \param [in/out] val_str String data come from profile entry, or will be saved to profile entry.
 * \param [in] index_offset Index offset for array LT fields.
 * \param [in] index_num For array LT fields, index number of parameter "val".
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_pkt_ctrl_profile_transfer(const char *fld_name,
                                   void *prof, int set, int *val,
                                   const char **val_str,
                                   int index_offset, int index_num)
{
    const char *fwd_action_t[] = { FORWARDs, DROPs, FLOODs };
    int len = 0, i, type_num = sizeof(fwd_action_t) / sizeof(&fwd_action_t[0]);
    bool symbol = 0;
    bcmi_ltsw_port_profile_port_pkt_ctrl_t *pkt_ctrl_prof = NULL;

    pkt_ctrl_prof = (bcmi_ltsw_port_profile_port_pkt_ctrl_t *)prof;

    if (set == 0) {
        *val_str = NULL;
        *val = 0;
    } else if (val_str != NULL && *val_str != NULL) {
        for (i = 0; i < type_num; i++) {
            len = sal_strlen(fwd_action_t[i]);
            if (!sal_memcmp(*val_str, fwd_action_t[i], len)) {
                *val = i;
                break;
            }
        }
        if (i == type_num) {
            return SHR_E_NOT_FOUND;
        }
    }

    len = sal_strlen(fld_name);
    if (!sal_memcmp(fld_name, SRP_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.srp_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.srp_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, SRP_FWD_ACTIONs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.srp_fwd_action = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.srp_fwd_action;
            symbol = 1;
        }
    } else if (!sal_memcmp(fld_name, ND_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.nd_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.nd_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, ND_DROPs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.nd_drop = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.nd_drop;
        }
    } else if (!sal_memcmp(fld_name, MMRP_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.mmrp_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.mmrp_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, MMRP_FWD_ACTIONs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.mmrp_fwd_action = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.mmrp_fwd_action;
            symbol = 1;
        }
    } else if (!sal_memcmp(fld_name, DHCP_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.dhcp_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.dhcp_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, DHCP_DROPs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.dhcp_drop = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.dhcp_drop;
        }
    } else if (!sal_memcmp(fld_name, ARP_REQUEST_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.arp_request_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.arp_request_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, ARP_REQUEST_DROPs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.arp_request_drop = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.arp_request_drop;
        }
    } else if (!sal_memcmp(fld_name, ARP_REPLY_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.arp_reply_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.arp_reply_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, ARP_REPLY_DROPs, len)) {
        if (set) {
            pkt_ctrl_prof->prot_pkt_ctrl.s.arp_reply_drop = *val;
        } else {
            *val = pkt_ctrl_prof->prot_pkt_ctrl.s.arp_reply_drop;
        }
    } else if (!sal_memcmp(fld_name, PFM_RULE_APPLYs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.pfm_rule_apply = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.pfm_rule_apply;
        }
    } else if (!sal_memcmp(fld_name, IGMP_REPORT_LEAVE_FWD_ACTIONs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_report_leave_fwd_action = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_report_leave_fwd_action;
            symbol = 1;
        }
    } else if (!sal_memcmp(fld_name, IGMP_REPORT_LEAVE_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_report_leave_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_report_leave_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, IGMP_QUERY_FWD_ACTIONs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_query_fwd_action = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_query_fwd_action;
            symbol = 1;
        }
    } else if (!sal_memcmp(fld_name, IGMP_QUERY_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_query_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_query_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, IGMP_UNKNOWN_MSG_FWD_ACTIONs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_unknown_msg_fwd_action = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_unknown_msg_fwd_action;
            symbol = 1;
        }
    } else if (!sal_memcmp(fld_name, IGMP_UNKNOWN_MSG_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_unknown_msg_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.igmp_unknown_msg_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, IPV4_RESVD_MC_PKT_FWD_ACTIONs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv4_resvd_mc_pkt_fwd_action = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv4_resvd_mc_pkt_fwd_action;
            symbol = 1;
        }
    } else if (!sal_memcmp(fld_name, IPV4_RESVD_MC_PKT_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv4_resvd_mc_pkt_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv4_resvd_mc_pkt_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv4_mc_router_adv_pkt_fwd_action = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv4_mc_router_adv_pkt_fwd_action;
            symbol = 1;
        }
    } else if (!sal_memcmp(fld_name, IPV4_MC_ROUTER_ADV_PKT_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv4_mc_router_adv_pkt_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv4_mc_router_adv_pkt_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, MLD_REPORT_DONE_FWD_ACTIONs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.mld_report_done_fwd_action = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.mld_report_done_fwd_action;
            symbol = 1;
        }
    } else if (!sal_memcmp(fld_name, MLD_REPORT_DONE_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.mld_report_done_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.mld_report_done_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, MLD_QUERY_FWD_ACTIONs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.mld_query_fwd_action = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.mld_query_fwd_action;
            symbol = 1;
        }
    } else if (!sal_memcmp(fld_name, MLD_QUERY_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.mld_query_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.mld_query_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, IPV6_RESVD_MC_PKT_FWD_ACTIONs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv6_resvd_mc_pkt_fwd_action = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv6_resvd_mc_pkt_fwd_action;
            symbol = 1;
        }
    } else if (!sal_memcmp(fld_name, IPV6_RESVD_MC_PKT_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv6_resvd_mc_pkt_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv6_resvd_mc_pkt_to_cpu;
        }
    } else if (!sal_memcmp(fld_name, IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv6_mc_router_adv_pkt_fwd_action = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv6_mc_router_adv_pkt_fwd_action;
            symbol = 1;
        }
    } else if (!sal_memcmp(fld_name, IPV6_MC_ROUTER_ADV_PKT_TO_CPUs, len)) {
        if (set) {
            pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv6_mc_router_adv_pkt_to_cpu = *val;
        } else {
            *val = pkt_ctrl_prof->igmp_mld_pkt_ctrl.s1.ipv6_mc_router_adv_pkt_to_cpu;
        }
    } else {
        return SHR_E_NOT_FOUND;
    }
    if (set == 0 && symbol) {
        if (*val >= type_num) {
            return SHR_E_INTERNAL;
        } else if (val_str != NULL) {
            *val_str = fwd_action_t[*val];
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Profile data of LPORT_TAB is saved into lport_tab.
 * Save data gotten from LT field to lport_tab. Or get data from lport_tab.
 *
 * \param [in] fld_name LT data field name.
 * \param [in] prof Profile data.
 * \param [in] set 1: save data to profile entry, 0: get data from profile entry.
 * \param [in/out] val Data come from profile entry, or will be saved to profile entry.
 * \param [in/out] val_str String data come from profile entry, or will be saved to profile entry.
 * \param [in] index_offset Index offset for array LT fields.
 * \param [in] index_num For array LT fields, index number of parameter "val".
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_system_profile_transfer(const char *fld_name, void *prof,
                                 int set, int *val, const char **val_str,
                                 int index_offset, int index_num)
{
    int len = 0, i = 0;
    bcmi_ltsw_port_system_prof_t *lport_tab = (bcmi_ltsw_port_system_prof_t *)prof;

    if (set == 0) {
        *val_str = NULL;
        *val = 0;
    }

    len = sal_strlen(fld_name);
    if (!sal_memcmp(fld_name, ING_VLAN_MEMBERSHIP_CHECKs, len)) {
        if (set) {
            lport_tab->en_ifilter = *val;
        } else {
            *val = lport_tab->en_ifilter;
        }
    } else if (!sal_memcmp(fld_name, MIRROR_ON_DROPs, len)) {
        if (set) {
            lport_tab->mirror_on_drop_mmu_eligible = *val;
        } else {
            *val = lport_tab->mirror_on_drop_mmu_eligible;
        }
    } else if (!sal_memcmp(fld_name, TM_MIRROR_ON_DROP_PROFILE_IDs, len)) {
        if (set) {
            lport_tab->mirror_on_drop_mmu_profile = *val;
        } else {
            *val = lport_tab->mirror_on_drop_mmu_profile;
        }
    } else if (!sal_memcmp(fld_name, ING_PVLANs, len)) {
        if (set) {
            lport_tab->pvlan_enable = *val;
        } else {
            *val = lport_tab->pvlan_enable;
        }
    } else if (!sal_memcmp(fld_name, TRUST_PHB_ING_DSCP_V4s, len)) {
        if (set) {
            lport_tab->trust_dscp_v4 = *val;
        } else {
            *val = lport_tab->trust_dscp_v4;
        }
    } else if (!sal_memcmp(fld_name, TRUST_PHB_ING_DSCP_V6s, len)) {
        if (set) {
            lport_tab->trust_dscp_v6 = *val;
        } else {
            *val = lport_tab->trust_dscp_v6;
        }
    } else if (!sal_memcmp(fld_name, PHB_ING_IP_DSCP_TO_INT_PRI_REMAP_IDs, len)) {
        if (set) {
            lport_tab->trust_dscp_ptr = *val;
        } else {
            *val = lport_tab->trust_dscp_ptr;
        }
    } else if (!sal_memcmp(fld_name, PHB_ING_L2_IDs, len)) {
        if (set) {
            lport_tab->trust_dot1p_ptr = *val;
        } else {
            *val = lport_tab->trust_dot1p_ptr;
        }
    } else if (!sal_memcmp(fld_name, PHB_ING_L2_OTAG_REMAP_IDs, len)) {
        if (set) {
            lport_tab->dot1p_remap_pointer = *val;
        } else {
            *val = lport_tab->dot1p_remap_pointer;
        }
    } else if (!sal_memcmp(fld_name, MAC_DROPs, len)) {
        if (set) {
            lport_tab->drop = *val;
        } else {
            *val = lport_tab->drop;
        }
    } else if (!sal_memcmp(fld_name, MAC_COPY_TO_CPUs, len)) {
        if (set) {
            lport_tab->tocpu = *val;
        } else {
            *val = lport_tab->tocpu;
        }
    } else if (!sal_memcmp(fld_name, MAC_LEARNs, len)) {
        if (set) {
            lport_tab->hw_learn = *val;
        } else {
            *val = lport_tab->hw_learn;
        }
    } else if (!sal_memcmp(fld_name, BRIDGEs, len)) {
        if (set) {
            lport_tab->bridge = *val;
        } else {
            *val = lport_tab->bridge;
        }
    } else if (!sal_memcmp(fld_name, TRUST_INCOMING_VIDs, len)) {
        if (set) {
            lport_tab->trust_incoming_vid = *val;
        } else {
            *val = lport_tab->trust_incoming_vid;
        }
    } else if (!sal_memcmp(fld_name, DROP_UNTAGs, len)) {
        if (set) {
            lport_tab->port_dis_untag = *val;
        } else {
            *val = lport_tab->port_dis_untag;
        }
    } else if (!sal_memcmp(fld_name, DROP_TAGs, len)) {
        if (set) {
            lport_tab->port_dis_tag = *val;
        } else {
            *val = lport_tab->port_dis_tag;
        }
    } else if (!sal_memcmp(fld_name, PASS_PAUSE_FRAMESs, len)) {
        if (set) {
            lport_tab->pass_control_frames = *val;
        } else {
            *val = lport_tab->pass_control_frames;
        }
    } else if (!sal_memcmp(fld_name, OUTER_TPID_VERIFYs, len)) {
        if (set) {
            lport_tab->outer_tpid_verify = *val;
        } else {
            *val = lport_tab->outer_tpid_verify;
        }
    } else if (!sal_memcmp(fld_name, PASS_ON_OUTER_TPID_MATCHs, len)) {
        if (set) {
            if (index_num + index_offset > DEPTH_ARRAY_FIELD) {
                return SHR_E_PARAM;
            }
            for (i = index_offset; i < index_offset + index_num; i++) {
                if (val[i - index_offset] == 1) {
                    lport_tab->outer_tpid_enable |= 1 << i;
                }
            }
        } else {
            for (i = index_offset; i < index_num + index_offset; i++) {
                val[i - index_offset] = (lport_tab->outer_tpid_enable & (1 << i)) ? 1 : 0;
            }
        }
    } else if (!sal_memcmp(fld_name, DROP_BPDUs, len)) {
        if (set) {
            lport_tab->drop_bpdu = *val;
        } else {
            *val = lport_tab->drop_bpdu;
        }
    } else if (!sal_memcmp(fld_name, ING_CFI_AS_CNGs, len)) {
        if (set) {
            if (index_num + index_offset > DEPTH_ARRAY_FIELD) {
                return SHR_E_PARAM;
            }
            lport_tab->cfi_as_cng = 0;
            for (i = index_offset; i < index_num + index_offset; i++) {
                if (val[i - index_offset] == 1) {
                    lport_tab->cfi_as_cng |= 1 << i;
                }
            }
        } else {
            for (i = index_offset; i < index_num + index_offset; i++) {
                val[i - index_offset] = (lport_tab->cfi_as_cng & (1 << i)) ? 1 : 0;
            }
        }
    } else if (!sal_memcmp(fld_name, FP_VLANs, len)) {
        if (set) {
            lport_tab->vfp_enable = *val;
        } else {
            *val = lport_tab->vfp_enable;
        }
    } else if (!sal_memcmp(fld_name, FP_INGs, len)) {
        if (set) {
            lport_tab->filter_enable = *val;
        } else {
            *val = lport_tab->filter_enable;
        }
    } else if (!sal_memcmp(fld_name, FP_VLAN_PORT_GRPs, len)) {
        if (set) {
            lport_tab->vfp_port_group_id = *val;
        } else {
            *val = lport_tab->vfp_port_group_id;
        }
    } else if (!sal_memcmp(fld_name, USE_TABLE_FP_VLAN_PORT_GRPs, len)) {
        if (set) {
            lport_tab->use_port_table_group_id = *val;
        } else {
            *val = lport_tab->use_port_table_group_id;
        }
    } else if (!sal_memcmp(fld_name, FP_ING_GRP_SEL_CLASS_IDs, len)) {
        if (set) {
            lport_tab->ifp_key_sel_class_id = *val;
        } else {
            *val = lport_tab->ifp_key_sel_class_id;
        }
    } else if (!sal_memcmp(fld_name, V6L3s, len)) {
        if (set) {
            lport_tab->v6l3_enable = *val;
        } else {
            *val = lport_tab->v6l3_enable;
        }
    } else if (!sal_memcmp(fld_name, V6IPMCs, len)) {
        if (set) {
            lport_tab->v6ipmc_enable = *val;
        } else {
            *val = lport_tab->v6ipmc_enable;
        }
    } else if (!sal_memcmp(fld_name, V4L3s, len)) {
        if (set) {
            lport_tab->v4l3_enable = *val;
        } else {
            *val = lport_tab->v4l3_enable;
        }
    } else if (!sal_memcmp(fld_name, V4IPMCs, len)) {
        if (set) {
            lport_tab->v4ipmc_enable = *val;
        } else {
            *val = lport_tab->v4ipmc_enable;
        }
    } else if (!sal_memcmp(fld_name, OPERATING_MODEs, len)) {
        if (set) {
            if (!sal_memcmp(*val_str, NORMALs, sal_strlen(NORMALs))) {
                lport_tab->port_operation = 0;
            } else if (!sal_memcmp(*val_str, L3_IIFs, sal_strlen(L3_IIFs))) {
                lport_tab->port_operation = 2;
            } else if (!sal_memcmp(*val_str, VRFs, sal_strlen(VRFs))) {
                lport_tab->port_operation = 3;
            } else {
                return SHR_E_PARAM;
            }
        } else {
            if (lport_tab->port_operation == 0) {
                *val_str = NORMALs;
                val[0] = 0;
            } else if (lport_tab->port_operation == 2) {
                *val_str = L3_IIFs;
                val[0] = 2;
            } else if (lport_tab->port_operation == 3) {
                *val_str = VRFs;
                val[0] = 3;
            } else {
                return SHR_E_INTERNAL;
            }
        }
    } else if (!sal_memcmp(fld_name, PORT_PKT_CONTROL_IDs, len)) {
        if (set) {
            lport_tab->protocol_pkt_index = *val;
        } else {
            *val = lport_tab->protocol_pkt_index;
        }
    } else if (!sal_memcmp(fld_name, MPLSs, len)) {
        if (set) {
            lport_tab->mpls = *val;
        } else {
            *val = lport_tab->mpls;
        }
    } else if (!sal_memcmp(fld_name, IPMC_USE_L3_IIFs, len)) {
        if (set) {
            lport_tab->ipmc_do_vlan = *val;
        } else {
            *val = lport_tab->ipmc_do_vlan;
        }
    } else if (!sal_memcmp(fld_name, IEEE_802_1ASs, len)) {
        if (set) {
            lport_tab->ieee_802_1as_enable = *val;
        } else {
            *val = lport_tab->ieee_802_1as_enable;
        }
    } else if (!sal_memcmp(fld_name, VLAN_ING_TAG_ACTION_PROFILE_IDs, len)) {
        if (set) {
            lport_tab->tag_action_profile_ptr = *val;
        } else {
            *val = lport_tab->tag_action_profile_ptr;
        }
    } else if (!sal_memcmp(fld_name, ING_OVIDs, len)) {
        if (set) {
            lport_tab->port_vid = *val;
        } else {
            *val = lport_tab->port_vid;
        }
    } else if (!sal_memcmp(fld_name, ING_OPRIs, len)) {
        if (set) {
            lport_tab->port_pri = *val;
        } else {
            *val = lport_tab->port_pri;
        }
    } else if (!sal_memcmp(fld_name, ING_OCFIs, len)) {
        if (set) {
            lport_tab->ocfi = *val;
        } else {
            *val = lport_tab->ocfi;
        }
    } else if (!sal_memcmp(fld_name, INBAND_TELEMETRY_IFAs, len)) {
        if (set) {
            lport_tab->int_ifa_enable = *val;
        } else {
            *val = lport_tab->int_ifa_enable;
        }
    } else if (!sal_memcmp(fld_name, INBAND_TELEMETRY_IOAMs, len)) {
        if (set) {
            lport_tab->int_ioam_enable = *val;
        } else {
            *val = lport_tab->int_ioam_enable;
        }
    } else if (!sal_memcmp(fld_name, INBAND_TELEMETRY_DATAPLANEs, len)) {
        if (set) {
            lport_tab->int_dp_enable = *val;
        } else {
            *val = lport_tab->int_dp_enable;
        }
    } else if (!sal_memcmp(fld_name, CTR_ING_EFLEX_ACTIONs, len)) {
        if (set) {
            lport_tab->flex_ctr_action = *val;
        } else {
            *val = lport_tab->flex_ctr_action;
        }
    } else if (!sal_memcmp(fld_name, INPORT_BITMAP_INDEXs, len)) {
        if (set) {
            lport_tab->ipbm_index = *val;
        } else {
            *val = lport_tab->ipbm_index;
        }
    } else if (!sal_memcmp(fld_name, DEVICE_TS_PTP_MSG_CONTROL_PROFILE_IDs, len)) {
        if (set) {
            lport_tab->ptp_msg_ctrl_profile_id = *val;
        } else {
            *val = lport_tab->ptp_msg_ctrl_profile_id;
        }
    } else if (!sal_memcmp(fld_name, NTP_TCs, len)) {
        if (set) {
            lport_tab->ntp_tc_enable = *val;
        } else {
            *val = lport_tab->ntp_tc_enable;
        }
    } else {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

/*!
 * \brief Get name of LTs which are mapped to LPORT_TAB.
 *
 * \param [out] lport_info Information of LTs.
 * \param [out] info_num Number of item of LTs' information.
 * \param [out] lt_list LT name list.
 * \param [out] lt_num Number of LT.
 *
 * \return NONE.
 */
static void
xgs_port_system_profile_info_get(int unit, const ltsw_port_tab_info_t **lport_info,
                                 const char ***lt_list, int *lt_num)
{
    int i = 0, j = 0, name_len = 0;
    const char *tbl_name;

    if (lport_tab_map[unit] == NULL) {
        lport_tab_map[unit] = lport_tab_map_def;
    }
    *lport_info = lport_tab_map[unit];
    *lt_num = 0;
    *lt_list = lport_tab_lt_list[unit];

    if (lport_tab_lt_list[unit][0] == NULL) {
        for (i = 0; i < BCMI_PT_CNT; i++) {
            if (lport_tab_map[unit][i].table == NULL) {
                continue;
            }
            tbl_name = lport_tab_map[unit][i].table->name;
            name_len = sal_strlen(tbl_name);
            for (j = 0; j < MAX_NUM_LTS; j++) {
                if (lport_tab_lt_list[unit][j] == NULL) {
                    lport_tab_lt_list[unit][j] = tbl_name;
                    break;
                } else if (sal_memcmp(lport_tab_lt_list[unit][j],
                                      tbl_name, name_len) == 0) {
                    break;
                }
            }
        }
        j = 0;
        while (lport_tab_lt_list[unit][j] != NULL) {
            j++;
        }
        lport_tab_lt_num[unit] = j;
    }
    *lt_num = lport_tab_lt_num[unit];
}

/*!
 * \brief Get name of LTs which are mapped to PORT_TAB.
 *
 * \param [out] lt_list LT name list.
 * \param [out] lt_key_list LT key field list.
 * \param [out] lt_num Number of LT.
 *
 * \return NONE.
 */
static void
xgs_port_tab_lt_list_get(int unit, const char ***lt_list,
                         const char ***lt_key_list, int *lt_num)
{
    int i = 0, j = 0, name_len = 0;
    const char *tbl_name, *key;

    *lt_num = 0;
    *lt_list = port_tab_lt_list[unit];
    *lt_key_list = port_tab_lt_key_list[unit];

    if (port_tab_map[unit] == NULL) {
        port_tab_map[unit] = port_tab_map_def;
    }
    if (port_tab_lt_list[unit][0] == NULL) {
        for (i = 0; i < BCMI_PT_CNT; i++) {
            if (port_tab_map[unit][i].table == NULL) {
                continue;
            }
            tbl_name = port_tab_map[unit][i].table->name;
            key = port_tab_map[unit][i].table->key;
            name_len = sal_strlen(tbl_name);
            for (j = 0; j < MAX_NUM_LTS; j++) {
                if (port_tab_lt_list[unit][j] == NULL) {
                    port_tab_lt_list[unit][j] = tbl_name;
                    port_tab_lt_key_list[unit][j] = key;
                    break;
                } else if (sal_memcmp(port_tab_lt_list[unit][j], tbl_name, name_len) == 0) {
                    break;
                }
            }
        }
        j = 0;
        while (port_tab_lt_list[unit][j] != NULL) {
            j++;
        }
        port_tab_lt_num[unit]= j;
    }
    *lt_num = port_tab_lt_num[unit];
}

/*!
 * \brief Set profile data to LT entry handler.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name LT name.
 * \param [in] entry_hdl LT entry handler.
 * \param [in] profile Profile data.
 * \param [in] transfer Transfer routine.
 *
 * \return NONE.
 */
static int
xgs_port_profile_data_set(int unit, const char *lt_name,
                               bcmlt_entry_handle_t entry_hdl, void *profile,
                               profile_transfer_f transfer)
{
    bcmlt_field_def_t *field_defs_array = NULL, *field_defs;
    int field_info_size = 0, field_count = 64;
    const char *data_fld_name = NULL, *fld_str_val = NULL;
    int rv, fld_val_i[DEPTH_ARRAY_FIELD];
    uint32_t require_array_size = 0, j = 0, k;
    uint64_t fld_val[DEPTH_ARRAY_FIELD];

    SHR_FUNC_ENTER(unit);

    field_info_size = field_count * sizeof(bcmlt_field_def_t);

    SHR_ALLOC(field_defs_array, field_info_size, "Field_def_array");
    SHR_NULL_CHECK(field_defs_array, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_defs_get(unit, lt_name,
                                    field_count, field_defs_array,
                                    &require_array_size));
    for (j = 0; j < require_array_size; j++) {
        field_defs = &field_defs_array[j];
        if (field_defs->key) {
            continue;
        }
        /* Data fields */
        data_fld_name = field_defs->name;
        rv = transfer(data_fld_name, profile, 0,
                      fld_val_i, &fld_str_val, 0,
                      DEPTH_ARRAY_FIELD);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        }
        for (k = 0; k < DEPTH_ARRAY_FIELD; k++) {
            fld_val[k] = fld_val_i[k];
        }
        if (field_defs->depth > DEPTH_ARRAY_FIELD) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        if (field_defs->symbol) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(entry_hdl,
                                              data_fld_name,
                                              fld_str_val));
        } else if (field_defs->depth > 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(entry_hdl,
                                             data_fld_name,
                                             0, fld_val,
                                             field_defs->depth));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, data_fld_name, fld_val[0]));
        }
    }

exit:
    SHR_FREE(field_defs_array);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get data from LT entry handler, and then save the data into profile.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_name LT name.
 * \param [in] entry_hdl LT entry handler.
 * \param [in] profile Profile data.
 * \param [in] transfer Transfer routine.
 *
 * \return NONE.
 */
static int
xgs_port_profile_data_get(int unit, const char *lt_name,
                               bcmlt_entry_handle_t entry_hdl, void *profile,
                               profile_transfer_f transfer)
{
    uint64_t fld_val[DEPTH_ARRAY_FIELD];
    uint32_t j, k, r_elem_cnt;
    bcmlt_field_def_t *field_defs;
    int fld_val_i[DEPTH_ARRAY_FIELD];
    bcmlt_field_def_t *field_defs_array = NULL;
    const char *data_fld_name = NULL, *fld_str_val = NULL;
    uint32_t require_array_size = 0;
    int field_info_size = 0, field_count = 64;

    SHR_FUNC_ENTER(unit);

    field_info_size = field_count * sizeof(bcmlt_field_def_t);

    SHR_ALLOC(field_defs_array, field_info_size, "Field_def_array");
    SHR_NULL_CHECK(field_defs_array, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_defs_get(unit, lt_name,
                                    field_count, field_defs_array,
                                    &require_array_size));
    for (j = 0; j < require_array_size; j++) {
        field_defs = &field_defs_array[j];
        if (field_defs->key) {
            continue;
        }
        fld_str_val = NULL;
        /* Data fields */
        data_fld_name = field_defs->name;
        r_elem_cnt = 1;
        if (field_defs->symbol) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(entry_hdl,
                                              data_fld_name,
                                              &fld_str_val));
        } else if (field_defs->depth > 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(entry_hdl,
                                             data_fld_name,
                                             0, fld_val, DEPTH_ARRAY_FIELD,
                                             &r_elem_cnt));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, data_fld_name, fld_val));
        }
        for (k = 0; k < DEPTH_ARRAY_FIELD; k++) {
            fld_val_i[k] = fld_val[k];
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (transfer(data_fld_name, profile, 1, fld_val_i,
                      &fld_str_val, 0, r_elem_cnt),
             SHR_E_NOT_FOUND);
    }

exit:
    SHR_FREE(field_defs_array);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get LPORT_TAB profile entry from LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [out] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_system_profile_get(int unit, int logic_index, void *profile)
{
    int i = 0, lt_num;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_transaction_info_t trans_info;
    bcmlt_entry_info_t entry_info;
    const char *table_name, **lt_names;
    int free_entry_hdl = 0, dunit;
    const ltsw_port_tab_info_t *lport_info;
    int pipe, index;

    SHR_FUNC_ENTER(unit);

    pipe = LOGIC_INDEX_TO_PIPE(logic_index);
    index = LOGIC_INDEX_TO_INDEX(logic_index);

    sal_memset(profile, 0, sizeof(bcmi_ltsw_port_system_prof_t));

    dunit = bcmi_ltsw_dev_dunit(unit);

    xgs_port_system_profile_info_get(unit, &lport_info, &lt_names, &lt_num);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    for (i = 0; i < lt_num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_names[i], &entry_hdl));
        free_entry_hdl = 1;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, PORT_SYSTEM_PROFILE_IDs, index));

        if (PORT_SYSTEM_PROFILE_MODE(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, PIPEs, pipe));
        }
        /* Add entry into transaction. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_transaction_entry_add(trans_hdl, BCMLT_OPCODE_LOOKUP, entry_hdl));
        free_entry_hdl = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_info_get(trans_hdl, &trans_info));

    for (i = 0; i < trans_info.num_entries; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_transaction_entry_num_get(trans_hdl, i, &entry_info));

        entry_hdl = entry_info.entry_hdl;
        table_name = entry_info.table_name;

        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_profile_data_get(unit, table_name, entry_hdl, profile,
                                       xgs_port_system_profile_transfer));
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fail to get data from LTs\n")));
    }
    if (free_entry_hdl) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert entry into LPORT_TAB profile entry through LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [out] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_system_profile_insert(int unit, int logic_index, void *profile)
{
    int i = 0, lt_num;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char **lt_names;
    int free_entry_hdl = 0, dunit, pipe, index;
    const ltsw_port_tab_info_t *lport_info;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    pipe = LOGIC_INDEX_TO_PIPE(logic_index);
    index = LOGIC_INDEX_TO_INDEX(logic_index);

    xgs_port_system_profile_info_get(unit, &lport_info, &lt_names, &lt_num);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    for (i = 0; i < lt_num; i++) {
        /* Allocate new entry */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_names[i], &entry_hdl));

        free_entry_hdl = 1;
        /* Key field */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, PORT_SYSTEM_PROFILE_IDs, index));

        if (PORT_SYSTEM_PROFILE_MODE(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, PIPEs, pipe));
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_profile_data_set(unit, lt_names[i], entry_hdl, profile,
                                       xgs_port_system_profile_transfer));

        /* Add previous entry into transaction. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_transaction_entry_add(trans_hdl,
                                         BCMLT_OPCODE_INSERT,
                                         entry_hdl));
        free_entry_hdl = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fail to insert data to LTs\n")));
    }
    if (free_entry_hdl) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete entry from LPORT_TAB profile entry through LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_system_profile_delete(int unit, int logic_index)
{
    int i = 0, lt_num;
    bcmlt_transaction_hdl_t trans_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int free_entry_hdl = 0, dunit, index, pipe;
    const ltsw_port_tab_info_t *lport_info;
    const char **lt_names;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    pipe = LOGIC_INDEX_TO_PIPE(logic_index);
    index = LOGIC_INDEX_TO_INDEX(logic_index);

    xgs_port_system_profile_info_get(unit, &lport_info, &lt_names, &lt_num);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    for (i = 0; i < lt_num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_names[i], &entry_hdl));
        free_entry_hdl = 1;
        /* Key field */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, PORT_SYSTEM_PROFILE_IDs, index));

        if (PORT_SYSTEM_PROFILE_MODE(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(entry_hdl, PIPEs, pipe));
        }
        /* Add entry into transaction. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_transaction_entry_add(trans_hdl,
                                         BCMLT_OPCODE_DELETE,
                                         entry_hdl));
        free_entry_hdl = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fail to delete data from LTs\n")));
    }
    if (free_entry_hdl) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash of LPORT_TAB profile set.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [out] hash Hash value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_system_profile_hash(int unit, void *entries,
                             int entries_per_set, uint32_t *hash)
{
    *hash = shr_crc32(0, entries, entries_per_set * sizeof(bcmi_ltsw_port_system_prof_t));
    return SHR_E_NONE;
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given LPORT_TAB profile entries equals to
 * the entries in LTs on PIPE 0.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [in] index Profile table index to be compared.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_system_profile_0_cmp(int unit, void *entries,
                            int entries_per_set, int index, int *cmp)
{
    return bcmint_ltsw_port_profile_cmp
            (unit, entries, index, PROFILE_PORT_SYSTEM(unit, 0), cmp);
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given LPORT_TAB profile entries equals to
 * the entries in LTs on PIPE 1.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [in] index Profile table index to be compared.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_system_profile_1_cmp(int unit, void *entries,
                            int entries_per_set, int index, int *cmp)
{
    return bcmint_ltsw_port_profile_cmp
            (unit, entries, index, PROFILE_PORT_SYSTEM(unit, 1), cmp);
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given LPORT_TAB profile entries equals to
 * the entries in LTs on PIPE 2.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [in] index Profile table index to be compared.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_system_profile_2_cmp(int unit, void *entries,
                            int entries_per_set, int index, int *cmp)
{
    return bcmint_ltsw_port_profile_cmp
            (unit, entries, index, PROFILE_PORT_SYSTEM(unit, 2), cmp);
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given LPORT_TAB profile entries equals to
 * the entries in LTs on PIPE 3.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [in] index Profile table index to be compared.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_system_profile_3_cmp(int unit, void *entries,
                            int entries_per_set, int index, int *cmp)
{
    return bcmint_ltsw_port_profile_cmp
            (unit, entries, index, PROFILE_PORT_SYSTEM(unit, 3), cmp);
}

/*!
 * \brief Recover LPORT_TAB profile through LTs during warmboot.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_system_profile_recover(int unit)
{
    int entry_idx, dunit;
    uint64_t u64;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int pipe = 0, port_system_id;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_SYSTEMs, &eh));
    while (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_TRAVERSE,
                                BCMLT_PRIORITY_NORMAL) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, PORT_SYSTEM_PROFILE_IDs, &u64));
        entry_idx = u64;

        if (PORT_SYSTEM_PROFILE_MODE(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, PORT_SYSTEM_IDs, &u64));
            port_system_id = u64;
            pipe = bcmi_ltsw_dev_logic_port_pp_pipe(unit, port_system_id);
        } else {
            pipe = 0;
        }
        /* recover the index */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_profile_recover(unit, entry_idx,
                                              PROFILE_PORT_SYSTEM(unit, pipe)));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get PORT_PKT_CONTROL profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [out] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_lt_port_pkt_ctrl_get(int unit, int index, void *profile)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    sal_memset(profile, 0, sizeof(bcmi_ltsw_port_profile_port_pkt_ctrl_t));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_PKT_CONTROLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_PKT_CONTROL_IDs, index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_profile_data_get(unit, PORT_PKT_CONTROLs, entry_hdl, profile,
                                   xgs_port_pkt_ctrl_profile_transfer));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert PORT_PKT_CONTROL profile entry to LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [in] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_lt_port_pkt_ctrl_insert(int unit, int index, void *profile)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_PKT_CONTROLs, &entry_hdl));
    /* Key field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_PKT_CONTROL_IDs, index));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_profile_data_set(unit, PORT_PKT_CONTROLs, entry_hdl, profile,
                                   xgs_port_pkt_ctrl_profile_transfer));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete PORT_PKT_CONTROL profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_lt_port_pkt_ctrl_delete(int unit, int index)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_PKT_CONTROL_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[0].u.val = index;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_delete(unit, PORT_PKT_CONTROLs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash of PORT_PKT_CONTROL profile set.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [out] hash Hash value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_profile_port_pkt_ctrl_hash(int unit, void *entries,
                                    int entries_per_set, uint32_t *hash)
{
    *hash = shr_crc32(0, entries, entries_per_set * sizeof(bcmi_ltsw_port_profile_port_pkt_ctrl_t));
    return SHR_E_NONE;
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given PORT_PKT_CONTROL profile entries equals to
 * the entries in LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [in] index Profile table index to be compared.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_profile_port_pkt_ctrl_cmp(int unit, void *entries,
                                   int entries_per_set, int index, int *cmp)
{
    return bcmint_ltsw_port_profile_cmp
            (unit, entries, index, PROFILE_PORT_PKT_CTRL(unit), cmp);
}

/*!
 * \brief Recover PORT_PKT_CONTROL profile from PORT after warmboot.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xgs_port_profile_port_pkt_ctrl_recover(int unit)
{
    int entry_idx, port, dunit;
    uint64_t u64;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* read all entries from PORT.PORT_PKT_CONTROL_ID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORTs, &eh));
    while (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_TRAVERSE,
                                BCMLT_PRIORITY_NORMAL) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, PORT_IDs, &u64));
        port = u64;
        if (IS_LB_PORT(unit, port)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, PORT_PKT_CONTROL_IDs, &u64));
        entry_idx = u64;

        /* recover the index */
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_port_profile_port_pkt_ctrl_recover(unit, entry_idx));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

static int
xgs_port_control_vport_get(int unit, bcm_port_t port,
                           bcm_port_control_t type, int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_port_control_get(unit, port, type, value));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_control_vport_set(int unit, bcm_port_t port,
                           bcm_port_control_t type, int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_virtual_port_control_set(unit, port, type, value));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_ltsw_port_move_set(int unit, bcm_port_t port, uint32_t flags)
{
    ltsw_port_tab_field_t fields[3] = {{0}};
    uint32_t valid_flags;

    SHR_FUNC_ENTER(unit);

    valid_flags = BCM_PORT_LEARN_FWD | BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_ARL;
    if ((flags | valid_flags) != valid_flags) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    fields[0].type = BCMI_PT_MAC_MOVE_DROP;
    fields[0].data.n = (flags & BCM_PORT_LEARN_FWD) ? 0 : 1;
    fields[1].type = BCMI_PT_MAC_MOVE_COPY_TO_CPU;
    fields[1].data.n = (flags & BCM_PORT_LEARN_CPU) ? 1 : 0;
    fields[2].type = BCMI_PT_MAC_MOVE;
    fields[2].data.n = (flags & BCM_PORT_LEARN_ARL) ? 1 : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, 3));

exit:
    SHR_FUNC_EXIT();
}


static int
xgs_ltsw_port_move_get(int unit, bcm_port_t port, uint32_t *flags)
{
    ltsw_port_tab_field_t fields[3] = {{0}};

    SHR_FUNC_ENTER(unit);

    fields[0].type = BCMI_PT_MAC_MOVE_DROP;
    fields[1].type = BCMI_PT_MAC_MOVE_COPY_TO_CPU;
    fields[2].type = BCMI_PT_MAC_MOVE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, fields, 3));

    *flags = 0;
    if (!(fields[0].data.n)) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (fields[1].data.n) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (fields[2].data.n) {
       *flags |= BCM_PORT_LEARN_ARL;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_sample_set(int unit, bcm_port_t port, bcm_port_control_t type, int value)
{
    ltsw_port_tab_field_t port_tab_field[3];
    int fld_num = 0;
    uint32_t width = 0;
    uint64_t field_val = 0;

    SHR_FUNC_ENTER(unit);

    switch (type) {
    case bcmPortControlSampleIngressDest:
        if (value < 0 || value > 3) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        port_tab_field[fld_num].type = BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_CPU;
        port_tab_field[fld_num].data.n = (value & BCM_PORT_CONTROL_SAMPLE_DEST_CPU) ? 1 : 0;
        fld_num++;

        port_tab_field[fld_num].type = BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_INSTANCE;
        port_tab_field[fld_num].data.n = (value & BCM_PORT_CONTROL_SAMPLE_DEST_MIRROR) ? 1 : 0;
        fld_num++;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_tab_multi_set(unit, port, port_tab_field, fld_num));
        break;
    case bcmPortControlSampleIngressRate:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_width_get(unit, MIRROR_PORT_ENCAP_SFLOWs,
                                     SAMPLE_ING_RATEs, &width));
        field_val = 1;
        field_val = (field_val << width) - 1;
        if (value < 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        port_tab_field[fld_num].type = BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_RATE;
        port_tab_field[fld_num].data.n = (value == 0) ? 0 :
            ((value == 1) ? field_val : ((field_val + 1) / value));
        fld_num++;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_tab_multi_set(unit, port, port_tab_field, fld_num));
        break;
    case bcmPortControlSampleEgressRate:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_width_get(unit, MIRROR_PORT_ENCAP_SFLOWs,
                                     SAMPLE_EGR_RATEs, &width));
        field_val = 1;
        field_val = (field_val << width) - 1;
        if (value < 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        port_tab_field[fld_num].type = BCMI_PT_MIRROR_EGR_ENCAP_SAMPLE_RATE;
        port_tab_field[fld_num].data.n = (value == 0) ? 0 :
            ((value == 1) ? field_val : ((field_val + 1) / value));
        fld_num++;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_tab_multi_set(unit, port, port_tab_field, fld_num));
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_telemetry_dataplane_ctrl_set(int unit, bcm_port_t port, int en)
{
    int fld_num, dunit, rv;
    uint64_t *array_fld_val = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint32_t cnt;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    fld_num = PORT_NUM_MAX(unit);
    SHR_ALLOC(array_fld_val, sizeof(uint64_t) * fld_num, "ArrayFieldDataDataPlane");
    SHR_NULL_CHECK(array_fld_val, SHR_E_MEMORY);
    sal_memset(array_fld_val, 0, sizeof(uint64_t) * fld_num);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_INBAND_TELEMETRY_DATAPLANE_CONTROLs, &eh));
    rv = bcmi_lt_entry_commit
             (unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        /* Add a new entry with default value if entry not existing */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit
                (unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit
                (unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get(eh, EGR_PORTS_TURNAROUNDs, 0,
                                     array_fld_val, fld_num, &cnt));
    array_fld_val[port] = en ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(eh, EGR_PORTS_TURNAROUNDs, 0,
                                     array_fld_val, fld_num));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    SHR_FREE(array_fld_val);
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

static int
xgs_port_telemetry_dataplane_ctrl_get(int unit, bcm_port_t port, int *en)
{
    int fld_num, dunit, rv;
    uint64_t *array_fld_val = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint32_t cnt;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    fld_num = PORT_NUM_MAX(unit);
    SHR_ALLOC(array_fld_val, sizeof(uint64_t) * fld_num, "ArrayFieldDataDataPlane");
    SHR_NULL_CHECK(array_fld_val, SHR_E_MEMORY);
    sal_memset(array_fld_val, 0, sizeof(uint64_t) * fld_num);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, MON_INBAND_TELEMETRY_DATAPLANE_CONTROLs, &eh));

    rv = bcmi_lt_entry_commit
             (unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        /* Add a new entry with default value if entry not existing */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit
                (unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit
                (unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get(eh, EGR_PORTS_TURNAROUNDs, 0,
                                     array_fld_val, fld_num, &cnt));
    *en = array_fld_val[port];

exit:
    SHR_FREE(array_fld_val);
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

static int
xgs_port_sample_get(int unit, bcm_port_t port, bcm_port_control_t type, int *value)
{
    ltsw_port_tab_field_t port_tab_field[3];
    int fld_num = 0;
    uint32_t width = 0;
    uint64_t field_val = 0;

    SHR_FUNC_ENTER(unit);

    switch (type) {
    case bcmPortControlSampleIngressDest:
        port_tab_field[fld_num].type = BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_CPU;
        fld_num++;
        port_tab_field[fld_num].type = BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_INSTANCE;
        fld_num++;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_tab_multi_get(unit, port, port_tab_field, fld_num));
        *value = port_tab_field[0].data.n ? BCM_PORT_CONTROL_SAMPLE_DEST_CPU : 0;
        *value |= port_tab_field[1].data.n ? BCM_PORT_CONTROL_SAMPLE_DEST_MIRROR : 0;
        break;
    case bcmPortControlSampleIngressRate:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_width_get(unit, MIRROR_PORT_ENCAP_SFLOWs,
                                     SAMPLE_ING_RATEs, &width));
        port_tab_field[fld_num].type = BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_RATE;
        fld_num++;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_tab_multi_get(unit, port, port_tab_field, fld_num));
        if (port_tab_field[0].data.n) {
            field_val = 1;
            *value = (field_val << width) / port_tab_field[0].data.n;
        } else {
            *value = 0;
        }
        break;
    case bcmPortControlSampleEgressRate:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_width_get(unit, MIRROR_PORT_ENCAP_SFLOWs,
                                     SAMPLE_EGR_RATEs, &width));
        port_tab_field[fld_num].type = BCMI_PT_MIRROR_EGR_ENCAP_SAMPLE_RATE;
        fld_num++;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_tab_multi_get(unit, port, port_tab_field, fld_num));
        if (port_tab_field[0].data.n) {
            field_val = 1;
            *value = (field_val << width) / port_tab_field[0].data.n;
        } else {
            *value = 0;
        }
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_operating_mode_get(int unit, bcm_port_t port, bcm_port_control_t type, int *value)
{
    int temp;

    SHR_FUNC_ENTER(unit);

    switch (type) {
    case bcmPortControlL3Ingress:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_PORT_SYSTEM_L3_IIF_ID, value));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get
             (unit, port, BCMI_PT_OPERATING_MODE, &temp));

        if ((*value && temp != OP_MODE_L3_IIF) ||
            (*value == 0 && temp != OP_MODE_NORMAL)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_EXIT();

    case bcmPortControlVrf:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_PORT_SYSTEM_VRF_ID, value));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get
             (unit, port, BCMI_PT_OPERATING_MODE, &temp));

        if ((*value && temp != OP_MODE_VRF) ||
            (*value == 0 && temp != OP_MODE_NORMAL)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_EXIT();

    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_operating_mode_set(int unit, bcm_port_t port, bcm_port_control_t type, int value)
{
    ltsw_port_tab_field_t port_tab_field[3];
    int fld_num = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(port_tab_field, 0x0, sizeof(port_tab_field));
    /* Handle other types. */
    switch (type) {
    case bcmPortControlL3Ingress:
        if (value > 0) {
            port_tab_field[fld_num].type = BCMI_PT_PORT_SYSTEM_L3_IIF_ID;
            port_tab_field[fld_num].data.n = value;
            fld_num++;

            port_tab_field[fld_num].type = BCMI_PT_PORT_SYSTEM_OPERATING_MODE;
            port_tab_field[fld_num].data.n = OP_MODE_L3_IIF;
            fld_num++;

            port_tab_field[fld_num].type = BCMI_PT_OPERATING_MODE;
            port_tab_field[fld_num].data.n = OP_MODE_L3_IIF;
            fld_num++;
        } else {
            port_tab_field[fld_num].type = BCMI_PT_PORT_SYSTEM_L3_IIF_ID;
            port_tab_field[fld_num].data.n = 0;
            fld_num++;

            port_tab_field[fld_num].type = BCMI_PT_PORT_SYSTEM_OPERATING_MODE;
            port_tab_field[fld_num].data.n = OP_MODE_NORMAL;
            fld_num++;

            port_tab_field[fld_num].type = BCMI_PT_OPERATING_MODE;
            port_tab_field[fld_num].data.n = OP_MODE_NORMAL;
            fld_num++;
        }
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_port_tab_multi_set(unit, port, port_tab_field, fld_num));
        SHR_EXIT();

    case bcmPortControlVrf:
        if (value > 0) {
            port_tab_field[fld_num].type = BCMI_PT_PORT_SYSTEM_VRF_ID;
            port_tab_field[fld_num].data.n = value;
            fld_num++;

            port_tab_field[fld_num].type = BCMI_PT_PORT_SYSTEM_OPERATING_MODE;
            port_tab_field[fld_num].data.n = OP_MODE_VRF;
            fld_num++;

            port_tab_field[fld_num].type = BCMI_PT_OPERATING_MODE;
            port_tab_field[fld_num].data.n = OP_MODE_VRF;
            fld_num++;
        } else {
            port_tab_field[fld_num].type = BCMI_PT_PORT_SYSTEM_VRF_ID;
            port_tab_field[fld_num].data.n = 0;
            fld_num++;

            port_tab_field[fld_num].type = BCMI_PT_PORT_SYSTEM_OPERATING_MODE;
            port_tab_field[fld_num].data.n = OP_MODE_NORMAL;
            fld_num++;

            port_tab_field[fld_num].type = BCMI_PT_OPERATING_MODE;
            port_tab_field[fld_num].data.n = OP_MODE_NORMAL;
            fld_num++;
        }
        SHR_IF_ERR_EXIT
            (bcmint_ltsw_port_tab_multi_set(unit, port, port_tab_field, fld_num));
        SHR_EXIT();

    default:
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_control_port_get(int unit, bcm_port_t port, bcm_port_control_t type, int *value)
{
    bcmi_ltsw_port_tab_t fld;
    int idx, depth = COUNTOF(xgs_port_control_port_map), pm, val_tmp;
    port_control_map_t *ctrl_port_ptr = xgs_port_control_port_map;
    bcm_port_t port_out;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port_out));

    /* Handle other types. */
    switch (type) {
    case bcmPortControlL3Ingress:
    case bcmPortControlVrf:
        SHR_IF_ERR_EXIT
            (xgs_port_operating_mode_get(unit, port_out, type, value));
        SHR_EXIT();
    case bcmPortControlL2Learn:
        SHR_IF_ERR_EXIT
            (xgs_ltsw_port_learn_get(unit, port_out, (uint32_t *)value));
        SHR_EXIT();
    case bcmPortControlL2Move:
        SHR_IF_ERR_EXIT
            (xgs_ltsw_port_move_get(unit, port_out, (uint32_t *)value));
        SHR_EXIT();
    case bcmPortControlSampleIngressDest:
    case bcmPortControlSampleIngressRate:
    case bcmPortControlSampleEgressRate:
        SHR_IF_ERR_EXIT
            (xgs_port_sample_get(unit, port_out, type, value));
        SHR_EXIT();
    case bcmPortControlIntTurnaround:
        SHR_IF_ERR_EXIT
            (xgs_port_telemetry_dataplane_ctrl_get(unit, port_out, value));
        SHR_EXIT();

    case bcmPortControlObmClassifierPriority:
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_port_control_get
                (unit, port, bcmiObmPortControlDefaultPktPri, value));
        SHR_EXIT();

    case bcmPortControlObmClassifierEnableEtag:
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_port_control_get
                (unit, port, bcmiObmPortControlEtagMapEnable, value));
        SHR_EXIT();

    case bcmPortControlObmClassifierEnableMpls:
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_port_control_get
                (unit, port, bcmiObmPortControlMplsMapEnable, value));
        SHR_EXIT();

    case bcmPortControlObmClassifierEnableDscp:
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_port_control_get
                (unit, port, bcmiObmPortControlDscpMapEnable, value));
        SHR_EXIT();

    case bcmPortControlObmClassifierVntagEthertype:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_pm_id_get(unit, port, &pm));
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_pm_port_control_get
                (unit, pm, bcmiObmPortControlVntagEthertype, value));
        SHR_EXIT();

    case bcmPortControlObmClassifierEtagEthertype:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_pm_id_get(unit, port, &pm));
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_pm_port_control_get
                (unit, pm, bcmiObmPortControlEtagEthertype, value));
        SHR_EXIT();

    case bcmPortControlVxlanVpnAssignmentCriteria:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, port_out,
                                    BCMI_PT_VXLAN_VFI_KEY_MODE,
                                    value));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, port_out,
                                    BCMI_PT_VXLAN_VFI_USE_PKT_OVID,
                                    &val_tmp));
        *value |= val_tmp ? 0x2 : 0x0;
        SHR_EXIT();

    case bcmPortControlVxlanTerminationMatchCriteria:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, port_out,
                                    BCMI_PT_VXLAN_USE_PKT_OVID,
                                    value));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, port_out,
                                    BCMI_PT_VXLAN_KEY_MODE,
                                    &val_tmp));
        *value |= val_tmp ? 0x2 : 0x0;
        SHR_EXIT();

    default:
        break;
    }

    /* Check the simple mapping table first. */
    for (idx = 0; idx < depth; idx++) {
        if (ctrl_port_ptr[idx].type != type) {
            continue;
        }
        if (ctrl_port_ptr[idx].flags & PORT_CTRL_PM) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_control_pm_get(unit, port, type, value));
        } else {
            fld = ctrl_port_ptr[idx].fld;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get(unit, port_out, fld, value));
            if (ctrl_port_ptr[idx].func_get) {
                /* need to translate the value from H/W to S/W*/
                SHR_IF_ERR_VERBOSE_EXIT
                    (ctrl_port_ptr[idx].func_get(unit, *value, value));
            }
        }
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_control_port_set(int unit, bcm_port_t port, bcm_port_control_t type, int value)
{
    bcmi_ltsw_port_tab_t fld;
    int idx, depth = COUNTOF(xgs_port_control_port_map), rv, hit = 0, pm;
    port_control_map_t *ctrl_port_ptr = xgs_port_control_port_map;
    bcm_port_t port_out;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port_out));

    /* Handle other types. */
    switch (type) {
    case bcmPortControlL3Ingress:
    case bcmPortControlVrf:
        SHR_IF_ERR_EXIT
            (xgs_port_operating_mode_set(unit, port_out, type, value));
        SHR_EXIT();

    case bcmPortControlL2Learn:
        SHR_IF_ERR_EXIT
            (xgs_ltsw_port_learn_set(unit, port_out, value));
        SHR_EXIT();

    case bcmPortControlL2Move:
        SHR_IF_ERR_EXIT
            (xgs_ltsw_port_move_set(unit, port_out, value));
        SHR_EXIT();

    case bcmPortControlSampleIngressDest:
    case bcmPortControlSampleIngressRate:
    case bcmPortControlSampleEgressRate:
        SHR_IF_ERR_EXIT
            (xgs_port_sample_set(unit, port_out, type, value));
        SHR_EXIT();

    case bcmPortControlIntTurnaround:
        SHR_IF_ERR_EXIT
            (xgs_port_telemetry_dataplane_ctrl_set(unit, port_out, value));
        SHR_EXIT();

    case bcmPortControlObmClassifierPriority:
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_port_control_set
                (unit, port, bcmiObmPortControlDefaultPktPri, value & 0x7));
        SHR_EXIT();

    case bcmPortControlObmClassifierEnableEtag:
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_port_control_set
                (unit, port, bcmiObmPortControlEtagMapEnable, value ? 1 : 0));
        SHR_EXIT();

    case bcmPortControlObmClassifierEnableMpls:
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_port_control_set
                (unit, port, bcmiObmPortControlMplsMapEnable, value ? 1 : 0));
        SHR_EXIT();

    case bcmPortControlObmClassifierEnableDscp:
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_port_control_set
                (unit, port, bcmiObmPortControlDscpMapEnable, value ? 1 : 0));
        SHR_EXIT();

    case bcmPortControlObmClassifierVntagEthertype:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_pm_id_get(unit, port, &pm));
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_pm_port_control_set
                (unit, pm, bcmiObmPortControlVntagEthertype, value & 0xFFFF));
        SHR_EXIT();

    case bcmPortControlObmClassifierEtagEthertype:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_pm_id_get(unit, port, &pm));
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_obm_pm_port_control_set
                (unit, pm, bcmiObmPortControlEtagEthertype, value & 0xFFFF));
        SHR_EXIT();

    case bcmPortControlVxlanVpnAssignmentCriteria:
        if (value > bcmPortVxlanVpnAssignOnVnidOuterVlanSrcIp ||
            value < bcmPortVxlanVpnAssignOnVnid) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, port_out,
                                    BCMI_PT_VXLAN_VFI_KEY_MODE,
                                    (value & 0x1)));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, port_out,
                                    BCMI_PT_VXLAN_VFI_USE_PKT_OVID,
                                    ((value & 0x2) >> 1)));
        SHR_EXIT();

    case bcmPortControlVxlanTerminationMatchCriteria:
        if (value > bcmPortVxlanTerminationMatchOuterVlanSrcDstIp ||
            value < bcmPortVxlanTerminationMatchDstIp) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, port_out,
                                    BCMI_PT_VXLAN_USE_PKT_OVID,
                                    (value & 0x1)));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, port_out,
                                    BCMI_PT_VXLAN_KEY_MODE,
                                    ((value & 0x2) >> 1)));
        SHR_EXIT();

    default:
        break;
    }
    /* Check the simple mapping table first. */
    for (idx = 0; idx < depth; idx++) {
        if (ctrl_port_ptr[idx].type != type) {
            continue;
        }
        /*
         * Take bcmPortControlPassControlFrames for example,
         * this API needs to call bcmint_ltsw_port_control_pm_set and
         * bcmi_ltsw_port_tab_set.
         */
        if (ctrl_port_ptr[idx].flags & PORT_CTRL_PM) {
            PORT_LOCK(unit);
            rv = bcmint_ltsw_port_control_pm_set(unit, port, type, value);
            PORT_UNLOCK(unit);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else {
            fld = ctrl_port_ptr[idx].fld;
            if (ctrl_port_ptr[idx].func_set) {
                /* Convert from S/W to H/W */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ctrl_port_ptr[idx].func_set(unit, value, &value));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set(unit, port_out, fld, value));
        }
        hit = 1;
    }
    if (hit == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_sample_rate_set(int unit, bcm_port_t port,
                              int ingress_rate, int egress_rate)
{
    int ingress_dest = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (xgs_port_control_port_get(unit, port, bcmPortControlSampleIngressDest,
                                   &ingress_dest));

    if (ingress_rate > 0) {
        SHR_IF_ERR_EXIT
            (xgs_port_control_port_set(unit, port, bcmPortControlSampleIngressRate,
                                       ingress_rate));
        SHR_IF_ERR_EXIT
            (xgs_port_control_port_set(unit, port,
                                       bcmPortControlSampleIngressEnable, 1));

        ingress_dest |= BCM_PORT_CONTROL_SAMPLE_DEST_CPU;
        SHR_IF_ERR_EXIT
            (xgs_port_control_port_set(unit, port, bcmPortControlSampleIngressDest,
                                       ingress_dest));
    } else {
        SHR_IF_ERR_EXIT
            (xgs_port_control_port_set(unit, port,
                                       bcmPortControlSampleIngressEnable, 0));

        ingress_dest &= ~BCM_PORT_CONTROL_SAMPLE_DEST_CPU;
        SHR_IF_ERR_EXIT
            (xgs_port_control_port_set(unit, port, bcmPortControlSampleIngressDest,
                                       ingress_dest));
    }
    if (egress_rate > 0) {
        SHR_IF_ERR_EXIT
            (xgs_port_control_port_set(unit, port, bcmPortControlSampleEgressRate,
                                       egress_rate));
        SHR_IF_ERR_EXIT
            (xgs_port_control_port_set(unit, port,
                                       bcmPortControlSampleEgressEnable, 1));
    } else {
        SHR_IF_ERR_EXIT
            (xgs_port_control_port_set(unit, port,
                                       bcmPortControlSampleEgressEnable, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_sample_rate_get(int unit, bcm_port_t port,
                              int *ingress_rate, int *egress_rate)
{
    int enable = 0;

    SHR_FUNC_ENTER(unit);

    if ((NULL ==  ingress_rate) || (NULL == egress_rate)) {
        SHR_IF_ERR_EXIT(BCM_E_PARAM);
    }
    SHR_IF_ERR_EXIT
        (xgs_port_control_port_get(unit, port,
                                   bcmPortControlSampleIngressEnable, &enable));
    if (enable) {
        SHR_IF_ERR_EXIT
            (xgs_port_control_port_get(unit, port,
                                       bcmPortControlSampleIngressRate, ingress_rate));
    } else {
        *ingress_rate = 0;
    }
    SHR_IF_ERR_EXIT
        (xgs_port_control_port_get(unit, port,
                                   bcmPortControlSampleEgressEnable, &enable));
    if (enable) {
        SHR_IF_ERR_EXIT
            (xgs_port_control_port_get(unit, port,
                                       bcmPortControlSampleEgressRate, egress_rate));
    } else {
        *egress_rate = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_flexctr_id_set(int unit, bcm_gport_t port, int dir, int ctr_id)
{
    SHR_FUNC_ENTER(unit);

    if (dir == BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set
                (unit, port, BCMI_PT_ING_FLEX_CTR_ID, ctr_id));
    } else if (dir == BCMI_LTSW_FLEXCTR_DIR_EGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set
                (unit, port, BCMI_PT_EGR_FLEX_CTR_ID, ctr_id));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_flexctr_id_get(int unit, bcm_gport_t port, int dir, int *ctr_id)
{
    SHR_FUNC_ENTER(unit);

    if (dir == BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get
                (unit, port, BCMI_PT_ING_FLEX_CTR_ID, ctr_id));
    } else if (dir == BCMI_LTSW_FLEXCTR_DIR_EGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get
                (unit, port, BCMI_PT_EGR_FLEX_CTR_ID, ctr_id));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static inline int
xgs_port_flexctr_info_check(bcmi_ltsw_flexctr_counter_info_t *info)
{
    if ((info->source == bcmFlexctrSourceIngPort) ||
        (info->source == bcmFlexctrSourceEgrPort)) {
        return SHR_E_NONE;
    } else {
        return SHR_E_PARAM;
    }
}

/*!
 * \brief Set OBM TPID enable bitmap.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] bmp TPIDs to be enabled, in the form of bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xgs_port_obm_tpid_bmp_set(int unit, int port, int bmp)
{
    int i, en;

    SHR_FUNC_ENTER(unit);

    if (is_aux_port(unit, port)) {
        SHR_EXIT();
    }

    for (i = 0; i < 4; i ++) {
        en = (bmp >> i) & 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_obm_port_control_set
                (unit, port, bcmiObmPortControlOuterTpid0Enable + i, en));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get OBM TPID enable bitmap.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [out] bmp TPIDs enabled, in the form of bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xgs_port_obm_tpid_bmp_get(int unit, int port, int *bmp)
{
    int i, en;

    SHR_FUNC_ENTER(unit);

    if (is_aux_port(unit, port)) {
        SHR_EXIT();
    }

    for (i = 0, *bmp = 0; i < 4; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_obm_port_control_get
                (unit, port, bcmiObmPortControlOuterTpid0Enable + i, &en));
        *bmp |= (en << i);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an OBM TPID to a port. A port could have multiple TPID.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] tpid TPID to be added.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xgs_port_obm_tpid_add(int unit, int port, uint16_t tpid)
{
    int tpid_index, tpid_bmp;

    SHR_FUNC_ENTER(unit);

    if (is_aux_port(unit, port)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_obm_tpid_bmp_get(unit, port, &tpid_bmp));

    /* allocate a TPID index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid, &tpid_index));

    /* rollback if already in the OBM TPID list; else add to OBM TPID list */
    if (tpid_bmp & (1 << tpid_index)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_delete(unit, tpid_index));
    } else {
        tpid_bmp |= (1 << tpid_index);
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_obm_tpid_bmp_set(unit, port, tpid_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set OBM TPID of a port. TPIDs other than the set one are deleted.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] tpid TPID to be set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xgs_port_obm_tpid_set(int unit, int port, uint16_t tpid)
{
    int tpid_index, tpid_bmp, i;

    SHR_FUNC_ENTER(unit);

    if (is_aux_port(unit, port)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_obm_tpid_bmp_get(unit, port, &tpid_bmp));

    /* allocate a TPID index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid, &tpid_index));

    /* delete all TPIDs in OBM TPID list */
    for (i = 0; tpid_bmp; i ++) {
        if (tpid_bmp & 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_delete(unit, i));
        }
        tpid_bmp = tpid_bmp >> 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_obm_tpid_bmp_set(unit, port, (1 << tpid_index)));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an OBM TPID from a port. Add default TPID if need.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] tpid TPID to be deleted.
 * \param [in] tpid_default Default TPID.
 * \param [in] force TRUE to force add default TPID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xgs_port_obm_tpid_delete(int unit, int port,
                         uint16_t tpid, uint16_t tpid_default)
{
    int tpid_index, tpid_bmp, tpid_default_index;

    SHR_FUNC_ENTER(unit);

    if (is_aux_port(unit, port)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_obm_tpid_bmp_get(unit, port, &tpid_bmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_lookup(unit, tpid, &tpid_index));

    /* the delete TPID must be in OBM TPID list */
    if ((tpid_bmp & (1 << tpid_index)) == 0) {
        if (tpid == tpid_default) {
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }
    tpid_bmp &= ~(1 << tpid_index);

    /* need add default TPID */
    if (tpid_bmp == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_lookup
                (unit, tpid_default, &tpid_default_index));

        /* add default TPID if it's not in OBM TPID list */
        tpid_bmp |= (1 << tpid_default_index);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_add
                (unit, tpid_default, &tpid_default_index));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_delete(unit, tpid_index));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_obm_tpid_bmp_set(unit, port, tpid_bmp));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all OBM TPID from a port, then add default TPID.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] tpid_default Default TPID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xgs_port_obm_tpid_delete_all(int unit, int port, uint16_t tpid_default)
{
    int tpid_bmp, tpid_del_bmp, tpid_default_index, i;

    SHR_FUNC_ENTER(unit);

    if (is_aux_port(unit, port)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_obm_tpid_bmp_get(unit, port, &tpid_bmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_lookup(unit, tpid_default, &tpid_default_index));

    /* add default TPID if it's not in OBM TPID list */
    if (!(tpid_bmp & (1 << tpid_default_index))) {
        tpid_bmp |= (1 << tpid_default_index);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid_default, &tpid_default_index));
    }

    /* delete all TPIDs except default TPID */
    tpid_del_bmp = tpid_bmp & ~(1 << tpid_default_index);
    for (i = 0; tpid_del_bmp; i ++) {
        if (tpid_del_bmp & 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_delete(unit, i));
        }
        tpid_del_bmp = tpid_del_bmp >> 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_obm_tpid_bmp_set(unit, port, (1 << tpid_default_index)));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_tpid_set(int unit, bcm_port_t port, int type, uint16_t tpid)
{
    int tpid_index, i;
    uint64_t tpid_en[MAX_TPID_CNT] = {0, 0, 0, 0};

    SHR_FUNC_ENTER(unit);

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_get(unit, port,
                                          BCMI_PT_PASS_ON_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
        /* allocate a TPID index */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid, &tpid_index));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_get(unit, port,
                                          BCMI_PT_PASS_ON_PAYLOAD_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
        /* allocate a PAYLOAD TPID index */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_payload_otpid_entry_add(unit, tpid, &tpid_index));
    }
    /* delete all TPIDs in port TPID list */
    for (i = 0; i < MAX_TPID_CNT; i ++) {
        if (tpid_en[i]) {
            if (type & OUTER_TPID) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_otpid_entry_delete(unit, i));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_payload_otpid_entry_delete(unit, i));
            }
        }
        tpid_en[i] = 0;
    }
    tpid_en[tpid_index] = 1;

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_set(unit, port,
                                          BCMI_PT_PASS_ON_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_set(unit, port,
                                          BCMI_PT_PASS_ON_EGR_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_VLAN_OUTER_TPID_ID, tpid_index));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_ING_VLAN_OUTER_TPID_ID, tpid_index));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_set(unit, port,
                                          BCMI_PT_PASS_ON_PAYLOAD_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_tpid_add(int unit, bcm_port_t port, int type, uint16_t tpid, int *tpid_index)
{
    uint64_t tpid_en[MAX_TPID_CNT] = {0, 0, 0, 0};

    SHR_FUNC_ENTER(unit);

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_get(unit, port,
                                          BCMI_PT_PASS_ON_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
        /* allocate a TPID index */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid, tpid_index));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_get(unit, port,
                                          BCMI_PT_PASS_ON_PAYLOAD_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
        /* allocate a TPID index */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_payload_otpid_entry_add(unit, tpid, tpid_index));
    }
    /* rollback if already in the port TPID list; else add to port TPID list */
    if (tpid_en[*tpid_index]) {
        if (type & OUTER_TPID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_delete(unit, *tpid_index));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_payload_otpid_entry_delete(unit, *tpid_index));
        }
    } else {
        tpid_en[*tpid_index] = 1;

        if (type & OUTER_TPID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_array_set(unit, port,
                                              BCMI_PT_PASS_ON_OUTER_TPID_MATCH,
                                              tpid_en, MAX_TPID_CNT));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_array_set(unit, port,
                                              BCMI_PT_PASS_ON_EGR_OUTER_TPID_MATCH,
                                              tpid_en, MAX_TPID_CNT));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_array_set(unit, port,
                                              BCMI_PT_PASS_ON_PAYLOAD_OUTER_TPID_MATCH,
                                              tpid_en, MAX_TPID_CNT));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_tpid_delete(int unit, bcm_port_t port, int type, uint16_t tpid, uint16_t tpid_default)
{
    int tpid_index, default_tpid_index, egr_tpid_index;
    uint16_t  egr_tpid = 0, i;
    uint64_t tpid_en[MAX_TPID_CNT] = {0, 0, 0, 0};

    SHR_FUNC_ENTER(unit);

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_get(unit, port,
                                          BCMI_PT_PASS_ON_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
        /* if port egr TPID is deleted, set default TPID on it */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_lookup(unit, tpid, &tpid_index));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_get(unit, port,
                                          BCMI_PT_PASS_ON_PAYLOAD_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
        /* if port egr TPID is deleted, set default TPID on it */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_payload_otpid_entry_lookup(unit, tpid, &tpid_index));

    }
    if (tpid_en[tpid_index]) {
        if (type & OUTER_TPID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get(unit, port,
                                        BCMI_PT_VLAN_OUTER_TPID_ID,
                                        &egr_tpid_index));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_get(unit, egr_tpid_index, &egr_tpid));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_delete(unit, tpid_index));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_payload_otpid_entry_delete(unit, tpid_index));
        }
        tpid_en[tpid_index] = 0;
        if (type & OUTER_TPID) {
            if (egr_tpid == tpid) {
                /* allocate a default TPID index */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid_default, &default_tpid_index));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_tab_set(unit, port,
                                            BCMI_PT_VLAN_OUTER_TPID_ID,
                                            default_tpid_index));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_tab_set(unit, port,
                                            BCMI_PT_ING_VLAN_OUTER_TPID_ID,
                                            default_tpid_index));
                tpid_en[default_tpid_index] = 1;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_array_set(unit, port,
                                              BCMI_PT_PASS_ON_OUTER_TPID_MATCH,
                                              tpid_en, MAX_TPID_CNT));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_array_set(unit, port,
                                              BCMI_PT_PASS_ON_EGR_OUTER_TPID_MATCH,
                                              tpid_en, MAX_TPID_CNT));
        } else {
            for (i = 0; i < MAX_TPID_CNT; i++) {
                if (tpid_en[i]) {
                    break;
                }
            }
            if (i == MAX_TPID_CNT) {
                /* allocate a default TPID index */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_payload_otpid_entry_add(unit, tpid_default, &default_tpid_index));
                tpid_en[default_tpid_index] = 1;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_array_set(unit, port,
                                              BCMI_PT_PASS_ON_PAYLOAD_OUTER_TPID_MATCH,
                                              tpid_en, MAX_TPID_CNT));
        }
    } else {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_port_tpid_recover(int unit)
{
    uint64_t tpid_en[MAX_TPID_CNT] = {0, 0, 0, 0};
    int i, port, tpid_bmp;

    SHR_FUNC_ENTER(unit);

    PORT_ALL_ITER(unit, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_get(unit, port,
                                          BCMI_PT_PASS_ON_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
        for (i = 0; i < MAX_TPID_CNT; i ++) {
            if (tpid_en[i]) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_otpid_entry_ref_count_add(unit, i, 1));
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_obm_tpid_bmp_get(unit, port, &tpid_bmp));

        for (i = 0; i < MAX_TPID_CNT; i++) {
            if (tpid_bmp & (1 << i)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_otpid_entry_ref_count_add(unit, i, 1));
            }
        }
        if (ltsw_feature(unit, LTSW_FT_PAYLOAD_TPID) == 0) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_get(unit, port,
                                          BCMI_PT_PASS_ON_PAYLOAD_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
        for (i = 0; i < MAX_TPID_CNT; i ++) {
            if (tpid_en[i]) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_payload_otpid_entry_ref_count_add(unit, i, 1));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */

int
xgs_ltsw_port_tpid_set(int unit, bcm_port_t port, int type, uint16_t tpid)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (!ltsw_feature(unit, LTSW_FT_PAYLOAD_TPID) && (type & PAYLOAD_OUTER_TPID)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    if ((type & (OUTER_TPID | OBM_OUTER_TPID | PAYLOAD_OUTER_TPID)) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_tpid_set(unit, port, OUTER_TPID, tpid));
    }
    if (type & PAYLOAD_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_tpid_set(unit, port, PAYLOAD_OUTER_TPID, tpid));
    }
    if (type & OBM_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_obm_tpid_set(unit, port, tpid));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_tpid_add(int unit, bcm_port_t port, int type, uint16_t tpid,
                       int color_select)
{
    int tpid_index;
    uint64_t val[MAX_TPID_CNT] = {0, 0, 0, 0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (!ltsw_feature(unit, LTSW_FT_PAYLOAD_TPID) && (type & PAYLOAD_OUTER_TPID)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    if ((type & (OUTER_TPID | OBM_OUTER_TPID | PAYLOAD_OUTER_TPID)) == 0 ||
         color_select == BCM_COLOR_INNER_CFI) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_tpid_add(unit, port, OUTER_TPID, tpid, &tpid_index));

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_ltsw_port_tab_array_get(unit, port, BCMI_PT_EGR_CFI_AS_CNG,
                                          val, MAX_TPID_CNT),
             SHR_E_NOT_FOUND);

        if (color_select == BCM_COLOR_PRIORITY) {
            val[tpid_index] = 0;
        } else if (color_select == BCM_COLOR_OUTER_CFI) {
            val[tpid_index] = 1;
        } else {
                /*
                 * Already checked color_select param,
                 * Should never get here.
                 */
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_set(unit, port, BCMI_PT_EGR_CFI_AS_CNG,
                                          val, MAX_TPID_CNT));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_set(unit, port, BCMI_PT_ING_CFI_AS_CNG,
                                          val, MAX_TPID_CNT));
    }
    if (type & PAYLOAD_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_tpid_add(unit, port, PAYLOAD_OUTER_TPID, tpid, &tpid_index));
    }
    if (type & OBM_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_obm_tpid_add(unit, port, tpid));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_tpid_delete(int unit, bcm_port_t port, int type, uint16_t tpid)
{
    uint16_t tpid_default = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (!ltsw_feature(unit, LTSW_FT_PAYLOAD_TPID) && (type & PAYLOAD_OUTER_TPID)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    if ((type & (OUTER_TPID | OBM_OUTER_TPID | PAYLOAD_OUTER_TPID)) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_default_get(unit, &tpid_default));

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_tpid_delete(unit, port, OUTER_TPID, tpid, tpid_default));
    }
    if (type & PAYLOAD_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_payload_otpid_default_get(unit, &tpid_default));

        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_tpid_delete(unit, port, PAYLOAD_OUTER_TPID, tpid, tpid_default));
    }
    if (type & OBM_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_obm_tpid_delete(unit, port, tpid, tpid_default));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_tpid_delete_all(int unit, bcm_port_t port, int type)
{
    uint16_t tpid_default = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (!ltsw_feature(unit, LTSW_FT_PAYLOAD_TPID) && (type & PAYLOAD_OUTER_TPID)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    if ((type & (OUTER_TPID | OBM_OUTER_TPID | PAYLOAD_OUTER_TPID)) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_default_get(unit, &tpid_default));

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_port_tpid_set(unit, port, OUTER_TPID, tpid_default));
    }
    if (type & PAYLOAD_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_payload_otpid_default_get(unit, &tpid_default));

        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_port_tpid_set(unit, port, PAYLOAD_OUTER_TPID, tpid_default));
    }
    if (type & OBM_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_obm_tpid_delete_all(unit, port, tpid_default));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_tpid_get(int unit, bcm_port_t port, int type, uint16_t *tpid)
{
    int tpid_index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if ((type & OUTER_TPID) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_VLAN_OUTER_TPID_ID, &tpid_index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_get(unit, tpid_index, tpid));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_tpid_get_all(int unit, bcm_port_t port, int type,
                           int size, uint16_t *tpid_array,
                           int *color_array, int *count)
{
    int index, cnt = 0, tpid_bmp = 0;
    uint64_t val[MAX_TPID_CNT] = {0, 0, 0, 0};
    uint64_t tpid_en[MAX_TPID_CNT] = {0, 0, 0, 0};
    uint16_t tpid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (!ltsw_feature(unit, LTSW_FT_PAYLOAD_TPID) && (type & PAYLOAD_OUTER_TPID)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    if ((type & (OUTER_TPID | OBM_OUTER_TPID | PAYLOAD_OUTER_TPID)) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_get(unit, port,
                                          BCMI_PT_PASS_ON_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
    } else if (type & OBM_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_obm_tpid_bmp_get(unit, port, &tpid_bmp));

        for (index = 0; index < MAX_TPID_CNT; index++) {
            if (tpid_bmp & 1) {
                tpid_en[index] = 1;
            } else {
                tpid_en[index] = 0;
            }
            tpid_bmp = tpid_bmp >> 1;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_array_get(unit, port,
                                          BCMI_PT_PASS_ON_PAYLOAD_OUTER_TPID_MATCH,
                                          tpid_en, MAX_TPID_CNT));
    }
    if (size == 0) {
        for (index = 0; index < MAX_TPID_CNT; index++) {
            if (tpid_en[index]) {
                cnt++;
            }
        }
        *count = cnt;
        /* Exit */
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_array_get(unit, port, BCMI_PT_EGR_CFI_AS_CNG,
                                      val, MAX_TPID_CNT));

    for (index = 0; index < MAX_TPID_CNT; index++) {
        if (cnt < size && tpid_en[index]) {
            if (type & PAYLOAD_OUTER_TPID) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_payload_otpid_entry_get(unit, index, &tpid));
                tpid_array[cnt] = tpid;
                cnt++;
                continue;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_otpid_entry_get(unit, index, &tpid));
            }
            tpid_array[cnt] = tpid;

            if (val[index]) {
                color_array[cnt] = BCM_COLOR_OUTER_CFI;
            } else {
                color_array[cnt] = BCM_COLOR_PRIORITY;
            }
            cnt++;
        }
    }
    *count = cnt;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_profile_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_system_profile_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_profile_port_pkt_ctrl_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_profile_deinit(int unit)
{
    return SHR_E_NONE;
}

int
xgs_ltsw_port_profile_init_per_port(int unit, int port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_profile_port_pkt_ctrl_per_port_init(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_system_profile_per_port_init(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_profile_deinit_per_port(int unit, int port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_profile_port_pkt_ctrl_per_port_deinit(unit, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_system_profile_per_port_deinit(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_info_init(int unit)
{
    int alloc_size;

    SHR_FUNC_ENTER(unit);

    SHR_FREE(xgs_port_profile[unit]);
    alloc_size = sizeof(xgs_port_profile_t);
    SHR_ALLOC(xgs_port_profile[unit], alloc_size, "xgs_port_profile");
    SHR_NULL_CHECK(xgs_port_profile[unit], SHR_E_MEMORY);
    sal_memset(xgs_port_profile[unit], 0, alloc_size);

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_info_deinit(int unit)
{
    int size;

    size = sizeof(xgs_port_profile_t);
    sal_memset(xgs_port_profile[unit], 0, size);
    SHR_FREE(xgs_port_profile[unit]);
    xgs_port_profile[unit] = NULL;

    return SHR_E_NONE;
}

int
xgs_ltsw_port_info_recover(int unit)
{
    return SHR_E_NONE;
}

int
xgs_ltsw_port_misc_init(int unit)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t port_ctrl_fields[] = {
        {EGR_BLOCK_L2s,    BCMI_LT_FIELD_F_SET, 0, {1}},
        {EGR_BLOCK_L3s,    BCMI_LT_FIELD_F_SET, 0, {1}},
    };

    SHR_FUNC_ENTER(unit);

    entry.fields = port_ctrl_fields;
    entry.nfields = sizeof(port_ctrl_fields) / sizeof(port_ctrl_fields[0]);
    entry.attr = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, PORT_CONTROLs, &entry, NULL));

    /* Initialize TPID */
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_tpid_init(unit));

    
    /* if (SAL_BOOT_SIMULATION && skip_partial_port_hw_init) {
        SHR_EXIT();
    }*/
#if 0
    /* Program the PROTOCOL_PKT_INDEX pointers */
    
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_timesync_profile_init(unit));
#endif
    

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_misc_deinit(int unit)
{
    return SHR_E_NONE;
}

int
xgs_ltsw_port_misc_init_per_port(int unit, int port)
{
    uint16_t tpid_default;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_default_get(unit, &tpid_default));

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_tpid_set(unit, port, OUTER_TPID,
                                tpid_default));

    if (ltsw_feature(unit, LTSW_FT_PAYLOAD_TPID)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_payload_otpid_default_get(unit, &tpid_default));
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_port_tpid_set(unit, port, PAYLOAD_OUTER_TPID,
                                    tpid_default));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_misc_deinit_per_port(int unit, int port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_tpid_delete_all(unit, port, OUTER_TPID));

    if (ltsw_feature(unit, LTSW_FT_PAYLOAD_TPID)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_port_tpid_delete_all(unit, port, PAYLOAD_OUTER_TPID));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_service_deinit_per_port(int unit, int port)
{
    int i = 0, lt_num = 0, rv;
    const char **lt_names, **lt_key_list;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int free_entry_hdl = 0, dunit;
    uint64_t port_u = (uint64_t)port;

    SHR_FUNC_ENTER(unit);

    xgs_port_tab_lt_list_get(unit, &lt_names, &lt_key_list, &lt_num);

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0; i < lt_num; i++) {
        rv = bcmlt_entry_allocate(dunit, lt_names[i], &entry_hdl);
        if (rv == SHR_E_PARAM) {
            continue;
        }
        free_entry_hdl = 1;
        /* Key field */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, lt_key_list[i], port_u));

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL),
             SHR_E_NOT_FOUND);

        bcmlt_entry_free(entry_hdl);

        free_entry_hdl = 0;
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "ERROR: "
                              "Failed to delete entry from LT %s by key %s\n"),
                   lt_names[i], lt_key_list[i]));
    }
    if (free_entry_hdl) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_tpid_init(int unit)
{
    int port, rv, index;
    uint16_t tpid_default, payload_tpid_default;

    SHR_FUNC_ENTER(unit);

    /* Initialize outer TPID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_default_get(unit, &tpid_default));
    /* reserve index for default tpid */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid_default, &index));

    if (ltsw_feature(unit, LTSW_FT_PAYLOAD_TPID)) {
        /* Initialize payload outer TPID */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_payload_otpid_init(unit));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_payload_otpid_default_get(unit, &payload_tpid_default));
        /* reserve index for default tpid */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_payload_otpid_entry_add(unit, payload_tpid_default, &index));
    }
    if (!bcmi_warmboot_get(unit)) {
        /* Enable the default outer TPID. */
        PORT_ALL_ITER(unit, port) {
            rv = xgs_ltsw_port_tpid_set(unit, port, OUTER_TPID,
                                        tpid_default);
            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit,
                                     "Warning: "
                                     "Failed to set %s port default TPID: %s\n"),
                          bcmi_ltsw_port_name(unit, port), shr_errmsg(rv)));
            }
            if (!ltsw_feature(unit, LTSW_FT_PAYLOAD_TPID)) {
                continue;
            }
            rv = xgs_ltsw_port_tpid_set(unit, port, PAYLOAD_OUTER_TPID,
                                        payload_tpid_default);
            if (SHR_FAILURE(rv)) {
                LOG_WARN(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit,
                                     "Warning: "
                                     "Failed to set %s port default PAYLOAD TPID: %s\n"),
                          bcmi_ltsw_port_name(unit, port), shr_errmsg(rv)));
            }
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_tpid_recover(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

ltsw_port_profile_info_t *
xgs_ltsw_port_profile_port_pkt_ctrl(int unit)
{
    return PROFILE_PORT_PKT_CTRL(unit);
}

int
xgs_ltsw_port_profile_port_pkt_ctrl_init(int unit)
{
    bcmi_ltsw_port_profile_port_pkt_ctrl_t *profile = NULL;
    ltsw_port_profile_info_t *pinfo = PROFILE_PORT_PKT_CTRL(unit);
    int port, profile_size, entry_idx;

    SHR_FUNC_ENTER(unit);

    pinfo->phd = BCMI_LTSW_PROFILE_PORT_PKT_CTRL;
    pinfo->entry_size = sizeof(bcmi_ltsw_port_profile_port_pkt_ctrl_t);
    pinfo->lt_get = xgs_port_lt_port_pkt_ctrl_get;
    pinfo->lt_ins = xgs_port_lt_port_pkt_ctrl_insert;
    pinfo->lt_del = xgs_port_lt_port_pkt_ctrl_delete;

    /* unregister in case the profile already exists */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, pinfo->phd), SHR_E_NOT_FOUND);
    /* register profile for PORT_PKT_CONTROL */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register
            (unit,
             &pinfo->phd, &pinfo->entry_idx_min, &pinfo->entry_idx_max, 1,
             xgs_port_profile_port_pkt_ctrl_hash,
             xgs_port_profile_port_pkt_ctrl_cmp));

    if (bcmi_warmboot_get(unit)) {
        /* just recover from PORT
         * other LTs which use the profile are covered by their modules */
        SHR_IF_ERR_EXIT
            (xgs_port_profile_port_pkt_ctrl_recover(unit));
        SHR_EXIT();
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, PORT_PKT_CONTROLs));
    /* default PORT_PKT_CONTROL profile entry */
    profile_size = pinfo->entries_per_set
                 * sizeof(bcmi_ltsw_port_profile_port_pkt_ctrl_t);
    SHR_ALLOC(profile, profile_size, "profile");
    SHR_NULL_CHECK(profile, SHR_E_MEMORY);
    sal_memset(profile, 0, profile_size);
    /* allocate profile entry, all PORT entries other than LB refer to it */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_profile_add
            (unit, profile, (NUM_ALL_PORT(unit) - NUM_LB_PORT(unit)),
             pinfo, &entry_idx));

    /* this is the first allocation so index must be zero */
    if (entry_idx != 0) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    /* write zero to PORT.PORT_PKT_CONTROL_ID in case it's non-zero */
    PORT_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port)) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set
                (unit, port, BCMI_PT_PORT_PKT_CONTROL_ID, 0));
    }

exit:
    SHR_FREE(profile);
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_profile_port_pkt_ctrl_per_port_init(int unit, int port)
{
    ltsw_port_profile_info_t *pinfo = PROFILE_PORT_PKT_CTRL(unit);
    bcmi_ltsw_port_profile_port_pkt_ctrl_t *profile = NULL;
    int profile_size, entry_idx;

    SHR_FUNC_ENTER(unit);

    profile_size = pinfo->entries_per_set
                 * sizeof(bcmi_ltsw_port_profile_port_pkt_ctrl_t);
    SHR_ALLOC(profile, profile_size, "profile");
    SHR_NULL_CHECK(profile, SHR_E_MEMORY);
    sal_memset(profile, 0, profile_size);

    if (IS_LB_PORT(unit, port) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_profile_add
                (unit, profile, 1, pinfo, &entry_idx));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set
                (unit, port, BCMI_PT_PORT_PKT_CONTROL_ID, entry_idx));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FREE(profile);
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_profile_port_pkt_ctrl_per_port_deinit(int unit, int port)
{
    int entry_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port,
                                BCMI_PT_PORT_PKT_CONTROL_ID, &entry_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_profile_delete
            (unit, entry_idx, PROFILE_PORT_PKT_CTRL(unit)));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_profile_port_pkt_ctrl_add(int unit, void *profile, int *index)
{
    return bcmint_ltsw_port_profile_add
            (unit, profile, 1, PROFILE_PORT_PKT_CTRL(unit), index);
}

int
xgs_ltsw_port_profile_port_pkt_ctrl_get(int unit, int index, void *profile)
{
    return bcmint_ltsw_port_profile_get
            (unit, index, PROFILE_PORT_PKT_CTRL(unit), profile);
}

int
xgs_ltsw_port_profile_port_pkt_ctrl_delete(int unit, int index)
{
    return bcmint_ltsw_port_profile_delete
            (unit, index, PROFILE_PORT_PKT_CTRL(unit));
}

int
xgs_ltsw_port_profile_port_pkt_ctrl_recover(int unit, int index)
{
    return bcmint_ltsw_port_profile_recover
            (unit, index, PROFILE_PORT_PKT_CTRL(unit));
}

int
xgs_ltsw_port_profile_port_pkt_ctrl_update(int unit, int port,
                                           const char *field_name,
                                           int field_value)
{
    int profile_old_index = 0, profile_index = 0, val = 0;
    bcmi_ltsw_port_profile_port_pkt_ctrl_t profile;
    const char *val_str = NULL;

    SHR_FUNC_ENTER(unit);

    if (IS_LB_PORT(unit, port)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_PORT_PKT_CONTROL_ID, &profile_old_index));

    sal_memset(&profile, 0, sizeof(profile));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_profile_port_pkt_ctrl_get(unit, profile_old_index,
                                                 (void *)(&profile)));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_pkt_ctrl_profile_transfer(field_name, &profile, 0,
                                            &val, &val_str, 0, 0));
    if (field_value != val) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_pkt_ctrl_profile_transfer(field_name, &profile, 1,
                                                &field_value, NULL, 0, 0));
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_port_profile_port_pkt_ctrl_add(unit, (void *)(&profile),
                                                     &profile_index));
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_port_profile_port_pkt_ctrl_delete(unit, profile_old_index));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set
                (unit, port, BCMI_PT_PORT_PKT_CONTROL_ID, profile_index));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_profile_port_pkt_ctrl_field_get(int unit, int port,
                                              const char *field_name,
                                              int *field_value)
{
    int profile_old_index = 0;
    bcmi_ltsw_port_profile_port_pkt_ctrl_t profile;
    const char *val_str = NULL;

    SHR_FUNC_ENTER(unit);

    if (IS_LB_PORT(unit, port)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_PORT_PKT_CONTROL_ID, &profile_old_index));

    sal_memset(&profile, 0, sizeof(profile));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_profile_port_pkt_ctrl_get(unit, profile_old_index,
                                                 (void *)(&profile)));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_pkt_ctrl_profile_transfer(field_name, &profile, 0,
                                            field_value, &val_str, 0, 0));
exit:
    SHR_FUNC_EXIT();
}

ltsw_port_profile_info_t *
xgs_ltsw_port_system_profile(int unit, int pipe)
{
    return PROFILE_PORT_SYSTEM(unit, pipe);
}

int
xgs_ltsw_port_system_profile_init(int unit)
{
    int port, profile_size, entry_idx, lts_num, i, entry_ref_count;
    const char **lts_name;
    const ltsw_port_tab_info_t *lport_info;
    bcmi_ltsw_port_system_prof_t *profile = NULL;
    ltsw_port_profile_info_t *pinfo = NULL;
    int pipe, pipe_num;
    bcm_pbmp_t pbmp;
    bcmi_ltsw_profile_hdl_t phd_array[] = {
        BCMI_LTSW_PROFILE_SYSTEM_PORT_PIPE0,
        BCMI_LTSW_PROFILE_SYSTEM_PORT_PIPE1,
        BCMI_LTSW_PROFILE_SYSTEM_PORT_PIPE2,
        BCMI_LTSW_PROFILE_SYSTEM_PORT_PIPE3
    };
    bcmi_ltsw_profile_cmp_cb cmp_cb[] = {
        xgs_port_system_profile_0_cmp,
        xgs_port_system_profile_1_cmp,
        xgs_port_system_profile_2_cmp,
        xgs_port_system_profile_3_cmp
    };

    SHR_FUNC_ENTER(unit);

    (void)xgs_ltsw_port_system_profile_opermode_get(unit, &PORT_SYSTEM_PROFILE_MODE(unit));
    pipe_num = bcmi_ltsw_dev_max_pp_pipe_num(unit);

    for (pipe = 0; pipe < pipe_num && pipe < COUNTOF(phd_array); pipe++) {
        pinfo = PROFILE_PORT_SYSTEM(unit, pipe);
        if (PORT_SYSTEM_PROFILE_MODE(unit)) {
            pinfo->phd = phd_array[pipe];
        } else {
            /* When port_system_profile_mode is false, four PP pipes share the same one 'phd'. */
            pinfo->phd = phd_array[0];
        }
        pinfo->entry_size = sizeof(bcmi_ltsw_port_system_prof_t);
        pinfo->lt_get = xgs_port_system_profile_get;
        pinfo->lt_ins = xgs_port_system_profile_insert;
        pinfo->lt_del = xgs_port_system_profile_delete;

        if (PORT_SYSTEM_PROFILE_MODE(unit) == 0 && pipe != 0) {
            continue;
        }
        /* unregister in case the profile already exists */
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_ltsw_profile_unregister(unit, pinfo->phd), SHR_E_NOT_FOUND);
        /* register profile for LPORT_TAB */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_register
                (unit, &pinfo->phd, &pinfo->entry_idx_min, &pinfo->entry_idx_max,
                 1, xgs_port_system_profile_hash, cmp_cb[pipe]));
    }
    if (bcmi_warmboot_get(unit)) {
        /* just recover from PORT
         * other LTs which use the profile are covered by their modules */
        SHR_IF_ERR_EXIT
            (xgs_port_system_profile_recover(unit));
        SHR_EXIT();
    }
    xgs_port_system_profile_info_get(unit, &lport_info, &lts_name, &lts_num);

    /* clear profile table */
    for (i = 0; i < lts_num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, lts_name[i]));
    }
    /* default LPORT_TAB profile entry */
    profile_size = pinfo->entries_per_set
                 * sizeof(bcmi_ltsw_port_system_prof_t);
    SHR_ALLOC(profile, profile_size, "profile");
    SHR_NULL_CHECK(profile, SHR_E_MEMORY);
    sal_memset(profile, 0, profile_size);

    profile->hw_learn = 1;
    profile->ipmc_do_vlan = 1;
    profile->filter_enable = 1;
    profile->v4ipmc_enable = 1;
    profile->v6ipmc_enable = 1;
    profile->v4l3_enable = 1;
    profile->v6l3_enable = 1;
    profile->v6ipmc_enable = 1;

    for (pipe = 0; pipe < pipe_num; pipe++) {
        BCM_PBMP_CLEAR(pbmp);
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_dev_pp_pipe_logic_pbmp(unit, pipe, &pbmp));
        BCM_PBMP_COUNT(pbmp, entry_ref_count);
        /* allocate profile entry, all PORT entries */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_profile_add
                (unit, (void *)profile, entry_ref_count,
                 PROFILE_PORT_SYSTEM(unit, pipe), &entry_idx));
        /* this is the first allocation so index must be zero */
        if (entry_idx != 0) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
        /* write zero to PORT_SYSTEM.PORT_SYSTEM_PROFILE_ID in case it's non-zero */
        BCM_PBMP_ITER(pbmp, port) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set(unit, port,
                                        BCMI_PT_PORT_SYSTEM_PROFILE_ID,
                                        entry_idx));
        }
    }

exit:
    SHR_FREE(profile);
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_system_profile_per_port_init(int unit, int port)
{
    ltsw_port_profile_info_t *pinfo = NULL;
    bcmi_ltsw_port_system_prof_t *profile = NULL;
    int profile_size, entry_idx, pipe, dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    pipe = bcmi_ltsw_dev_logic_port_pp_pipe(dunit, port);
    pinfo = PROFILE_PORT_SYSTEM(unit, pipe);

    /* default LPORT_TAB profile entry */
    profile_size = pinfo->entries_per_set
                 * sizeof(bcmi_ltsw_port_system_prof_t);
    SHR_ALLOC(profile, profile_size, "profile");
    SHR_NULL_CHECK(profile, SHR_E_MEMORY);
    sal_memset(profile, 0, profile_size);

    profile->hw_learn = 1;
    profile->bridge = 1;
    profile->ipmc_do_vlan = 1;
    profile->filter_enable = 1;
    profile->mpls = 1;
    profile->v4ipmc_enable = 1;
    profile->v6ipmc_enable = 1;
    profile->v4l3_enable = 1;
    profile->v6l3_enable = 1;
    profile->v6ipmc_enable = 1;

    /* allocate profile entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_profile_add
            (unit, (void *)profile, 1, pinfo, &entry_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port,
                                BCMI_PT_PORT_SYSTEM_PROFILE_ID,
                                entry_idx));
exit:
    SHR_FREE(profile);
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_system_profile_per_port_deinit(int unit, int port)
{
    int entry_idx, dunit, pipe;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    pipe = bcmi_ltsw_dev_logic_port_pp_pipe(dunit, port);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port,
                                BCMI_PT_PORT_SYSTEM_PROFILE_ID, &entry_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_profile_delete
            (unit, entry_idx, PROFILE_PORT_SYSTEM(unit, pipe)));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_system_profile_opermode_get(int unit, bool *mode)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t fld_val;

    SHR_FUNC_ENTER(unit);

    *mode = 0;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_CONFIGs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, PORT_SYSTEM_PROFILE_OPERMODE_PIPEUNIQUEs, &fld_val));
    *mode = fld_val ? 1 : 0;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_profile_lport_tab_update(int unit, ltsw_port_tab_field_t *fields,
                                       int fields_cnt)
{
    int i, j, lt_num;
    bcmi_ltsw_port_tab_t type;
    const ltsw_port_tab_info_t *tab_map = lport_tab_map[unit];
    const char *field_name, *fld_val_s, **lt_names;
    uint32_t fld_type = 0;
    int fld_start_idx = 0, fld_idx_num, port, fld_val[DEPTH_ARRAY_FIELD];
    ltsw_port_tab_sym_list_t *sym_list;
    int prof_index, prof_index_old, dunit, pipe;
    bcmi_ltsw_port_system_prof_t profile;
    ltsw_port_tab_field_t *fld_ptr = NULL;
    ltsw_port_profile_info_t *port_sys = NULL;

    SHR_FUNC_ENTER(unit);

    port = fields->index;

    dunit = bcmi_ltsw_dev_dunit(unit);

    pipe = bcmi_ltsw_dev_logic_port_pp_pipe(dunit, port);

    xgs_port_system_profile_info_get(unit, &tab_map, &lt_names, &lt_num);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_PORT_SYSTEM_PROFILE_ID, &prof_index_old));

    sal_memset(&profile, 0, sizeof(bcmi_ltsw_port_system_prof_t));
    port_sys = PROFILE_PORT_SYSTEM(unit, pipe);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_profile_get(unit, prof_index_old, port_sys, &profile));

    for (i = 0; i < fields_cnt; i++) {
        fld_ptr = &fields[i];
        if ((fld_ptr->info->flags & SYNC_TO_LPORT) == 0) {
            continue;
        }
        type = fld_ptr->type;
        if (type >= BCMI_PT_CNT) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        field_name = tab_map[type].field;
        if (field_name == NULL) {
            LOG_WARN(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                                 "Warning: "
                                 "Dont register type[%d] to lport_tab_map.\n"), type));
            continue;
        }
        fld_type = fld_ptr->info->flags & (FLD_IS_SYMBOL | FLD_IS_ARRAY | FLD_IS_COMBINED);
        fld_start_idx = 0;
        fld_idx_num = 1;
        if (fld_type == 0) {
            fld_val[0] = fld_ptr->data.n;
        } else if (fld_type == FLD_IS_SYMBOL) {
            sym_list = (ltsw_port_tab_sym_list_t *)fld_ptr->info->list;
            if ((fld_ptr->data.n < 0) || ((int)(fld_ptr->data.n) >= sym_list->len)) {
                fld_val_s = NULL;
            } else {
                fld_val_s = sym_list->syms[fld_ptr->data.n];
            }
            SHR_NULL_CHECK(fld_val_s, SHR_E_FAIL);
        } else if (fld_type == FLD_IS_ARRAY) {
            for (j = 0; j < DEPTH_ARRAY_FIELD && j < fld_ptr->data.a.size; j++) {
                fld_val[j] = fld_ptr->data.a.buf[j];
            }
            fld_start_idx = fld_ptr->data.a.idx;
            fld_idx_num = fld_ptr->data.a.size;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_system_profile_transfer(field_name, (void *)(&profile), 1,
                                              fld_val, &fld_val_s, fld_start_idx,
                                              fld_idx_num));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_profile_add(unit, (void *)(&profile), 1,
                                      port_sys, &prof_index));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_profile_delete(unit, prof_index_old, port_sys));

    if (prof_index != prof_index_old) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_PORT_SYSTEM_PROFILE_ID, prof_index));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [out] value Current value of the port feature.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
xgs_ltsw_port_control_get(int unit, bcm_port_t port,
                          bcm_port_control_t type, int *value)
{
    SHR_FUNC_ENTER(unit);

    /* Handle virtual port firstly */
    if (BCM_GPORT_IS_TRUNK(port) ||
        BCM_GPORT_IS_VLAN_PORT(port) ||
        BCM_GPORT_IS_NIV_PORT(port) ||
        BCM_GPORT_IS_VXLAN_PORT(port) ||
        BCM_GPORT_IS_MIM_PORT(port) ||
        BCM_GPORT_IS_MPLS_PORT(port) ||
        BCM_GPORT_IS_L2GRE_PORT(port) ||
        BCM_GPORT_IS_FLOW_PORT(port) ||
        BCM_GPORT_IS_EXTENDER_PORT(port)) {
        SHR_IF_ERR_EXIT
            (xgs_port_control_vport_get(unit, port, type, value));
        SHR_EXIT();
    }

    /* Now it could be a PROXY GPORT or BCM local port */
    SHR_IF_ERR_EXIT
        (xgs_port_control_port_get(unit, port, type, value));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [in] value Value of the port feature to be set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
xgs_ltsw_port_control_set(int unit, bcm_port_t port,
                          bcm_port_control_t type, int value)
{
    SHR_FUNC_ENTER(unit);

    /* Handle virtual port firstly */
    if (BCM_GPORT_IS_TRUNK(port) ||
        BCM_GPORT_IS_VLAN_PORT(port) ||
        BCM_GPORT_IS_NIV_PORT(port) ||
        BCM_GPORT_IS_VXLAN_PORT(port) ||
        BCM_GPORT_IS_MIM_PORT(port) ||
        BCM_GPORT_IS_MPLS_PORT(port) ||
        BCM_GPORT_IS_L2GRE_PORT(port) ||
        BCM_GPORT_IS_FLOW_PORT(port) ||
        BCM_GPORT_IS_EXTENDER_PORT(port)) {
        SHR_IF_ERR_EXIT
            (xgs_port_control_vport_set(unit, port, type, value));
        SHR_EXIT();
    }

    /* Now it could be a PROXY GPORT or BCM local port */
    SHR_IF_ERR_EXIT
        (xgs_port_control_port_set(unit, port, type, value));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_tab_info_get(int unit, bcmi_ltsw_port_tab_t type,
                           const ltsw_port_tab_info_t **info)
{
    SHR_FUNC_ENTER(unit);

    if (port_tab_map[unit] == NULL) {
        port_tab_map[unit] = port_tab_map_def;
    }
    if ((type <= BCMI_PT_ZERO) || (type >= BCMI_PT_CNT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else if (port_tab_map[unit][type].table == NULL ||
               port_tab_map[unit][type].table->name == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    *info = &port_tab_map[unit][type];

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_tab_info_update(int unit, bcmi_ltsw_port_tab_t type,
                              const ltsw_port_tab_info_t *info)
{
    int alloc_size = 0;
    ltsw_port_tab_info_t *port_tab;

    if ((type <= BCMI_PT_ZERO) || (type >= BCMI_PT_CNT)) {
        return SHR_E_PARAM;
    }
    if (port_tab_map[unit] == NULL) {
        alloc_size = BCMI_PT_CNT * sizeof(ltsw_port_tab_info_t);
        port_tab = sal_alloc(alloc_size, "ltswPortTabInfo");
        if (port_tab == NULL) {
            return SHR_E_MEMORY;
        }
        sal_memcpy(port_tab, port_tab_map_def, alloc_size);
        port_tab_map[unit] = port_tab;
    }
    port_tab_map[unit][type].table = info->table;
    port_tab_map[unit][type].field = info->field;
    port_tab_map[unit][type].flags = info->flags;
    port_tab_map[unit][type].list = info->list;

    return SHR_E_NONE;
}

int
xgs_ltsw_port_lport_tab_info_update(int unit, bcmi_ltsw_port_tab_t type,
                                    const ltsw_port_tab_info_t *info)
{
    int alloc_size = 0;
    ltsw_port_tab_info_t *lport_tab;

    if ((type <= BCMI_PT_ZERO) || (type >= BCMI_PT_CNT)) {
        return SHR_E_PARAM;
    }
    if (lport_tab_map[unit] == NULL) {
        alloc_size = BCMI_PT_CNT * sizeof(ltsw_port_tab_info_t);
        lport_tab = sal_alloc(alloc_size, "ltswPortTabInfo");
        if (lport_tab == NULL) {
            return SHR_E_MEMORY;
        }
        sal_memcpy(lport_tab, lport_tab_map_def, alloc_size);
        lport_tab_map[unit] = lport_tab;
    }
    lport_tab_map[unit][type].table = info->table;
    lport_tab_map[unit][type].field = info->field;
    lport_tab_map[unit][type].flags = info->flags;
    lport_tab_map[unit][type].list = info->list;

    return SHR_E_NONE;
}

int
xgs_ltsw_port_tabs_info_get(int unit, ltsw_port_tab_field_t *fields, int count)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < count; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_port_tab_info_get(unit, fields[i].type, &fields[i].info));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_tabs_op_get(int unit, int port,
                          ltsw_port_tab_field_t *fields, int count,
                          uint32_t *op)
{
    int is_lport, is_local;
    int mod = -1, tid, id;
    int i;
    ltsw_port_tab_field_t *fld;

    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_PROXY(port)) {
        is_lport = 1;
    } else {
        is_lport = 0;
    }

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_resolve(unit, port, &mod, &port, &tid, &id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_modid_is_local(unit, mod, &is_local));
    } else {
        is_local = 1;
    }

    for (i = 0, fld = fields, *op = 0; i < count; i ++, fld ++) {
        if (fld->info->table == NULL) {
            /* No operation */
            fld->op = 0;
            continue;
        }
        if (is_local) {
            /*
             * local port - both logical port and local Lport - need
             * config port_tab and lport_tab.
             */
            if (fld->info->flags & SYNC_TO_LPORT) {
                fld->op |= LPORT_CFG;
            }
            fld->op |= PORT_CFG;
        } else {
            /* non-local Lport only configs lport_tab */
            fld->op |= LPORT_CFG;
        }
        if (is_lport) {
            /* set RTAG7 flag for RTAG7 fields  */
        }
        *op |= fld->op;
    }


exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_tabs_para_get(int unit, int port,
                            ltsw_port_tab_field_t *fields, int count,
                            uint32_t op)
{
    int i;
    ltsw_port_tab_field_t *fld;

    for (i = 0, fld = fields; i < count; i ++, fld ++) {
        fld->index = port;
    }

    return SHR_E_NONE;
}

int
xgs_ltsw_port_learn_set(int unit, bcm_port_t port, uint32_t flags)
{
    ltsw_port_tab_field_t fields[3] = {{0}};
    uint32_t valid_flags;

    SHR_FUNC_ENTER(unit);

    valid_flags = BCM_PORT_LEARN_FWD | BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_ARL;
    if ((flags | valid_flags) != valid_flags) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    fields[0].type = BCMI_PT_MAC_DROP;
    fields[0].data.n = (flags & BCM_PORT_LEARN_FWD) ? 0 : 1;
    fields[1].type = BCMI_PT_MAC_COPY_TO_CPU;
    fields[1].data.n = (flags & BCM_PORT_LEARN_CPU) ? 1 : 0;
    fields[2].type = BCMI_PT_MAC_LEARN;
    fields[2].data.n = (flags & BCM_PORT_LEARN_ARL) ? 1 : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, 3));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_learn_get(int unit, bcm_port_t port, uint32_t *flags)
{
    ltsw_port_tab_field_t fields[3] = {{0}};

    SHR_FUNC_ENTER(unit);

    fields[0].type = BCMI_PT_MAC_DROP;
    fields[1].type = BCMI_PT_MAC_COPY_TO_CPU;
    fields[2].type = BCMI_PT_MAC_LEARN;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, fields, 3));

    *flags = 0;
    if (!(fields[0].data.n)) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (fields[1].data.n) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (fields[2].data.n) {
       *flags |= BCM_PORT_LEARN_ARL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_l3_enable_set(int unit, bcm_port_t port, int enable)
{
    ltsw_port_tab_field_t fields[2] = {{0}};

    SHR_FUNC_ENTER(unit);

    fields[0].type = BCMI_PT_V4L3;
    fields[0].data.n = enable ? 1 : 0;
    fields[1].type = BCMI_PT_V6L3;
    fields[1].data.n = enable ? 1 : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, 2));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_l3_enable_get(int unit, bcm_port_t port, int *enable)
{
    ltsw_port_tab_field_t fields[2] = {{0}};

    SHR_FUNC_ENTER(unit);

    fields[0].type = BCMI_PT_V4L3;
    fields[1].type = BCMI_PT_V6L3;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, fields, 2));

    *enable = (fields[0].data.n || fields[1].data.n);

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_l3_mtu_set(int unit, bcm_port_t port, int size)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_MTU, size));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_MTU_EN, 1));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_l3_mtu_get(int unit, bcm_port_t port, int *size)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_MTU, size));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_ipmc_modify_set(int unit, bcm_port_t port, uint32_t flags)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {PORT_IDs,         BCMI_LT_FIELD_F_SET, 0, {0}},
        {SKIP_TTL_DECs,    BCMI_LT_FIELD_F_SET, 0, {0}},
        {SKIP_SA_REPLACEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        {SKIP_TTL_CHECKs,  BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].u.val = port;
    fields[1].u.val = flags & BCM_PORT_IPMC_MODIFY_NO_TTL ? 1 : 0;
    fields[2].u.val = flags & BCM_PORT_IPMC_MODIFY_NO_SRCMAC ? 1 : 0;
    fields[3].u.val = flags & BCM_PORT_IPMC_CHECK_NO_TTL ? 1 : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, L3_MC_PORT_CONTROLs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_ipmc_modify_get(int unit, bcm_port_t port, uint32_t *flags)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {PORT_IDs,         BCMI_LT_FIELD_F_SET, 0, {0}},
        {SKIP_TTL_DECs,    BCMI_LT_FIELD_F_GET, 0, {0}},
        {SKIP_SA_REPLACEs, BCMI_LT_FIELD_F_GET, 0, {0}},
        {SKIP_TTL_CHECKs,  BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].u.val = port;

    rv = bcmi_lt_entry_get(unit, L3_MC_PORT_CONTROLs, &entry, NULL, NULL);
    if (rv == SHR_E_NOT_FOUND) {
        /* Insert default values to this LT */
        entry.nfields = 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, L3_MC_PORT_CONTROLs, &entry, NULL));
        /* Get again */
        entry.nfields = sizeof(fields) / sizeof(fields[0]);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, L3_MC_PORT_CONTROLs, &entry, NULL, NULL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    *flags = 0;
    *flags |= fields[1].u.val ? BCM_PORT_IPMC_MODIFY_NO_TTL : 0;
    *flags |= fields[2].u.val ? BCM_PORT_IPMC_MODIFY_NO_SRCMAC : 0;
    *flags |= fields[3].u.val ? BCM_PORT_IPMC_CHECK_NO_TTL : 0;

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_queue_count_get(int unit, bcm_port_t port, uint32_t *count)
{
    int cosq, cosq_num;
    uint64_t val64;
    bcmi_ltsw_cosq_port_map_info_t info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_port_map_info_get(unit, port, &info));

    *count = 0;

    for (cosq = 0; cosq < info.num_uc_q; cosq ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_cosq_stat_sync_get
                (unit, port, cosq, bcmCosqStatEgressUCQueueBytesCurrent,
                 &val64));
        *count += val64;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_cosq_stat_sync_get
                (unit, port, cosq, bcmCosqStatEgressUCQueueMinBytesCurrent,
                 &val64));
        *count += val64;
    }
    cosq_num = info.num_uc_q + info.num_mc_q;
    for (cosq = info.num_uc_q; cosq < cosq_num; cosq ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_cosq_stat_sync_get
                (unit, port, cosq, bcmCosqStatEgressMCQueueBytesCurrent,
                 &val64));
        *count += val64;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_cosq_stat_sync_get
                (unit, port, cosq, bcmCosqStatEgressMCQueueMinBytesCurrent,
                 &val64));
        *count += val64;
    }

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_stat_attach(int unit, bcm_port_t port, uint32_t stat_id)
{
    bcmi_ltsw_flexctr_counter_info_t info = {0};
    int ctr_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_id, &info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_flexctr_info_check(&info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_flexctr_id_get(unit, port, info.direction, &ctr_id));

    if (ctr_id == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit, stat_id));
        SHR_IF_ERR_CONT
            (xgs_port_flexctr_id_set
                (unit, port, info.direction, info.action_index));
        if (SHR_FUNC_ERR()) {
            (void)
            bcmi_ltsw_flexctr_detach_counter_id_status_update(unit, stat_id);
        }
    } else if (ctr_id != info.action_index) {
        /* Already attached a different flex counter action. */
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_stat_detach(int unit, bcm_port_t port, uint32_t stat_id)
{
    bcmi_ltsw_flexctr_counter_info_t info = {0};
    int ctr_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_id, &info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_flexctr_info_check(&info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_port_flexctr_id_get(unit, port, info.direction, &ctr_id));

    if (ctr_id == info.action_index) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_port_flexctr_id_set
                (unit, port, info.direction, BCMI_LTSW_FLEXCTR_ACTION_INVALID));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit, stat_id));
    } else {
        /* Already attached a different flex counter action. */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_flood_block_set(int unit, bcm_port_t ing_port, bcm_port_t egr_port,
                              uint32_t flags)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {PORT_IDs,    BCMI_LT_FIELD_F_SET, 0, {0}},
        {BCs,         BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        {UNKNOWN_UCs, BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        {UNKNOWN_MCs, BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        {KNOWN_MCs,   BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        {ALLs,   BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].u.val = ing_port;
    fields[1].idx = fields[2].idx = fields[3].idx = egr_port;
    fields[4].idx = fields[5].idx = egr_port;
    fields[1].u.val = flags & BCM_PORT_FLOOD_BLOCK_BCAST ? 1 : 0;
    fields[2].u.val = flags & BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST ? 1 : 0;
    fields[3].u.val = flags & BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST ? 1 : 0;
    fields[4].u.val = flags & BCM_PORT_FLOOD_BLOCK_KNOWN_MCAST ? 1 : 0;
    fields[5].u.val = flags & BCM_PORT_FLOOD_BLOCK_ALL ? 1 : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, PORT_FLOOD_BLOCKs, &entry, NULL));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_flood_block_get(int unit, bcm_port_t ing_port, bcm_port_t egr_port,
                              uint32_t *flags)
{
    bcmi_lt_entry_t entry;
    bcmi_lt_field_t fields[] = {
        {PORT_IDs,    BCMI_LT_FIELD_F_SET, 0, {0}},
        {BCs,         BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_ARRAY, 0, {0}},
        {UNKNOWN_UCs, BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_ARRAY, 0, {0}},
        {UNKNOWN_MCs, BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_ARRAY, 0, {0}},
        {KNOWN_MCs,   BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_ARRAY, 0, {0}},
        {ALLs,        BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_ARRAY, 0, {0}},
    };
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    entry.fields = fields;
    entry.nfields = sizeof(fields) / sizeof(fields[0]);
    entry.attr = 0;
    fields[0].u.val = ing_port;
    fields[1].idx = fields[2].idx = fields[3].idx = egr_port;
    fields[4].idx = fields[5].idx = egr_port;
    rv = bcmi_lt_entry_get(unit, PORT_FLOOD_BLOCKs, &entry, NULL, NULL);
    if (rv == SHR_E_NOT_FOUND) {
        /* Insert default values to this LT */
        entry.nfields = 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, PORT_FLOOD_BLOCKs, &entry, NULL));
        /* Get again */
        entry.nfields = sizeof(fields) / sizeof(fields[0]);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get(unit, PORT_FLOOD_BLOCKs, &entry, NULL, NULL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    *flags = 0;
    *flags |= fields[1].u.val ? BCM_PORT_FLOOD_BLOCK_BCAST : 0;
    *flags |= fields[2].u.val ? BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST : 0;
    *flags |= fields[3].u.val ? BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST : 0;
    *flags |= fields[4].u.val ? BCM_PORT_FLOOD_BLOCK_KNOWN_MCAST : 0;
    *flags |= fields[5].u.val ? BCM_PORT_FLOOD_BLOCK_ALL : 0;

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_bpdu_enable_set(int unit, bcm_port_t port, int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_DROP_BPDU, !enable));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_bpdu_enable_get(int unit, bcm_port_t port, int *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_DROP_BPDU, enable));
    *enable ^= 1;

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_ing_vlan_action_set(int unit, bcm_port_t port,
                                  bcm_vlan_action_set_t *action,
                                  int action_profile_index)
{
    ltsw_port_tab_field_t fields[5] = {{0}};
    int fields_num;
    uint32_t vfi;

    SHR_FUNC_ENTER(unit);

    fields_num = 0;
    if (ltsw_feature(unit, LTSW_FT_VFI)) {
        /* default vfi */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vfi_idx_get(unit, action->forwarding_domain, &vfi));
        fields[fields_num].type = BCMI_PT_DEFAULT_VFI;
        fields[fields_num].data.n = vfi;
        fields_num ++;
    }

    fields[fields_num].type = BCMI_PT_VLAN_ING_TAG_ACTION_PROFILE_ID;
    fields[fields_num].data.n = action_profile_index;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_ING_OPRI;
    fields[fields_num].data.n = action->priority;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_ING_OVID;
    fields[fields_num].data.n = action->new_outer_vlan;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_ING_OCFI;
    fields[fields_num].data.n = action->new_outer_cfi;
    fields_num ++;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_ing_vlan_action_get(int unit, bcm_port_t port,
                                  bcm_vlan_action_set_t *action,
                                  int *action_profile_index)

{
    ltsw_port_tab_field_t fields[5] = {{0}};
    int fields_num;
    uint32_t vfi;
    bcm_vpn_t vpn;

    SHR_FUNC_ENTER(unit);

    fields_num = 0;

    fields[fields_num].type = BCMI_PT_VLAN_ING_TAG_ACTION_PROFILE_ID;
    fields_num ++;

    /* Sometimes, user only wants to get old profile index. */
    if (action != NULL) {
        fields[fields_num].type = BCMI_PT_ING_OPRI;
        fields_num ++;

        fields[fields_num].type = BCMI_PT_ING_OVID;
        fields_num ++;

        fields[fields_num].type = BCMI_PT_ING_OCFI;
        fields_num ++;

        if (ltsw_feature(unit, LTSW_FT_VFI)) {
            /* default vfi */
            fields[fields_num].type = BCMI_PT_DEFAULT_VFI;
            fields_num ++;
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, fields, fields_num));

    *action_profile_index = fields[0].data.n;
    /* Sometimes, user only wants to get old profile index. */
    if (action != NULL) {
        action->priority = fields[1].data.n;

        action->new_outer_vlan = fields[2].data.n;

        action->new_outer_cfi = fields[3].data.n;

        if (ltsw_feature(unit, LTSW_FT_VFI)) {
            vfi = fields[4].data.n;
            /* default vfi */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_virtual_vpnid_get(unit, vfi, &vpn));
            action->forwarding_domain = vpn;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_egr_vlan_action_set(int unit, bcm_port_t port,
                                  bcm_vlan_action_set_t *action,
                                  int action_profile_index)
{
    ltsw_port_tab_field_t fields[4] = {{0}};
    int fields_num;

    SHR_FUNC_ENTER(unit);

    fields_num = 0;

    fields[fields_num].type = BCMI_PT_VLAN_EGR_TAG_ACTION_PROFILE_ID;
    fields[fields_num].data.n = action_profile_index;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_EGR_OPRI;
    fields[fields_num].data.n = action->priority;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_EGR_OVID;
    fields[fields_num].data.n = action->new_outer_vlan;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_EGR_OCFI;
    fields[fields_num].data.n = action->new_outer_cfi;
    fields_num ++;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_egr_vlan_action_get(int unit, bcm_port_t port,
                                  bcm_vlan_action_set_t *action,
                                  int *action_profile_index)

{
    ltsw_port_tab_field_t fields[4] = {{0}};
    int fields_num;

    SHR_FUNC_ENTER(unit);

    fields_num = 0;

    fields[fields_num].type = BCMI_PT_VLAN_EGR_TAG_ACTION_PROFILE_ID;
    fields_num ++;
    /* Sometimes, user only wants old action profile index. */
    if (action != NULL) {
        fields[fields_num].type = BCMI_PT_EGR_OPRI;
        fields_num ++;

        fields[fields_num].type = BCMI_PT_EGR_OVID;
        fields_num ++;

        fields[fields_num].type = BCMI_PT_EGR_OCFI;
        fields_num ++;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, fields, fields_num));

    *action_profile_index = fields[0].data.n;
    /* Sometimes, user only wants old action profile index. */
    if (action != NULL) {

        action->priority = fields[1].data.n;

        action->new_outer_vlan = fields[2].data.n;

        action->new_outer_cfi = fields[3].data.n;
    }
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_force_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vlan,
                             int pkt_prio, uint32 flags)
{
    ltsw_port_tab_field_t fields[6] = {{0}};
    int fields_num, vlan_valid = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_exist_check(unit, vlan, &vlan_valid));
    if (vlan_valid == 0 || pkt_prio > 7) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    fields_num = 0;

    fields[fields_num].type = BCMI_PT_ING_PVLAN;
    fields[fields_num].data.n = (flags & BCM_PORT_FORCE_VLAN_ENABLE) ? 1 : 0;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_EGR_PVLAN;
    fields[fields_num].data.n = (flags & BCM_PORT_FORCE_VLAN_ENABLE) ? 1 : 0;
    fields_num ++;

    if (flags & BCM_PORT_FORCE_VLAN_ENABLE) {
        fields[fields_num].type = BCMI_PT_PVLAN_EGR_UNTAG;
        fields[fields_num].data.n = (flags & BCM_PORT_FORCE_VLAN_UNTAG) ? 1 : 0;
        fields_num ++;

        if ((flags & BCM_PORT_FORCE_VLAN_UNTAG) == 0) {
            fields[fields_num].type = BCMI_PT_PVLAN_EGR_VID;
            fields[fields_num].data.n = vlan;
            fields_num ++;

            fields[fields_num].type = BCMI_PT_PVLAN_EGR_REPLACE_PRI;
            fields[fields_num].data.n = (pkt_prio < 0) ? 0 : 1;
            fields_num ++;

            if (pkt_prio > 0) {
                fields[fields_num].type = BCMI_PT_PVLAN_EGR_PRI;
                fields[fields_num].data.n = pkt_prio;
                fields_num ++;
            }
        }
    } else {
        /* No operations */
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_force_vlan_get(int unit, bcm_port_t port, bcm_vlan_t *vlan,
                             int *pkt_prio, uint32 *flags)
{
    ltsw_port_tab_field_t fields[6] = {{0}};
    int fields_num;

    SHR_FUNC_ENTER(unit);

    fields_num = 0;

    fields[fields_num].type = BCMI_PT_ING_PVLAN;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_EGR_PVLAN;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_PVLAN_EGR_UNTAG;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_PVLAN_EGR_REPLACE_PRI;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_PVLAN_EGR_PRI;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_PVLAN_EGR_VID;
    fields_num ++;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, fields, fields_num));

    *flags = 0;
    /* BCMI_PT_ING_PVLAN */
    if (fields[0].data.n) {
        *flags |= BCM_PORT_FORCE_VLAN_ENABLE;
        /* BCMI_PT_PVLAN_EGR_UNTAG */
        if (fields[2].data.n) {
            *flags |= BCM_PORT_FORCE_VLAN_UNTAG;
        } else {
            /* BCMI_PT_PVLAN_EGR_VID */
            *vlan = fields[5].data.n;
            /* BCMI_PT_PVLAN_EGR_REPLACE_PRI */
            if (fields[3].data.n) {
                /* BCMI_PT_PVLAN_EGR_PRI */
                *pkt_prio = fields[4].data.n;
            } else {
                *pkt_prio = -1;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_vlan_member_get(int unit, bcm_port_t port, uint32_t *flags)
{
    ltsw_port_tab_field_t fields[2] = {{0}};
    int fields_num;

    SHR_FUNC_ENTER(unit);

    if (flags == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    fields_num = 0;

    fields[fields_num].type = BCMI_PT_ING_VLAN_MEMBERSHIP_CHECK;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_EGR_VLAN_MEMBERSHIP_CHECK;
    fields_num ++;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, fields, fields_num));

    *flags = 0;
    if (fields[0].data.n) {
        *flags |= BCM_PORT_VLAN_MEMBER_INGRESS;
    }
    if (fields[1].data.n) {
        *flags |= BCM_PORT_VLAN_MEMBER_EGRESS;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_vlan_member_set(int unit, bcm_port_t port, uint32_t flags)
{
    ltsw_port_tab_field_t fields[2] = {{0}};
    int fields_num;

    SHR_FUNC_ENTER(unit);

    if ((flags & (BCM_PORT_VLAN_MEMBER_EGRESS | BCM_PORT_VLAN_MEMBER_INGRESS)) == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    fields_num = 0;

    fields[fields_num].type = BCMI_PT_ING_VLAN_MEMBERSHIP_CHECK;
    fields[fields_num].data.n = (flags & BCM_PORT_VLAN_MEMBER_INGRESS) ? 1 : 0;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_EGR_VLAN_MEMBERSHIP_CHECK;
    fields[fields_num].data.n = (flags & BCM_PORT_VLAN_MEMBER_EGRESS) ? 1 : 0;
    fields_num ++;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_untagged_priority_get(int unit, bcm_port_t port, int *priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_ING_OPRI, priority));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_untagged_priority_set(int unit, bcm_port_t port, int priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_ING_OPRI, priority));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_untagged_vlan_get(int unit, bcm_port_t port, bcm_vlan_t *vid_ptr)
{
    int data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_ING_OVID, &data));

    *vid_ptr = data;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_untagged_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_ING_OVID, (int)vid));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_tgid_get(int unit, bcm_port_t port, int *tid, int *psc)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_SYSTEM_IDs,    BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {SYSTEM_PORT_TYPEs,  BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*2 */ {TRUNK_IDs,          BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    int rv;

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(bcmi_lt_field_t);
    lt_entry.attr = 0;
    fields[0].u.val = port;

    rv = bcmi_lt_entry_get(unit, PORT_SYSTEMs, &lt_entry, NULL, NULL);

    if (rv == SHR_E_NOT_FOUND) {
        *tid = 0;
        *psc = 0;
        SHR_EXIT();
    } else if (sal_memcmp(fields[1].u.sym_val, TRUNK_PORTs, sal_strlen(TRUNK_PORTs)) == 0) {
        *tid = fields[2].u.val;
        *psc = 0;
    } else if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_tgid_set(int unit, bcm_port_t port, int tid, int psc)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_SYSTEM_IDs,    BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {SYSTEM_PORT_TYPEs,  BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*2 */ {TRUNK_IDs,          BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(bcmi_lt_field_t);
    lt_entry.attr = 0;
    fields[0].u.val = port;
    fields[1].u.sym_val = TRUNK_PORTs;
    fields[2].u.val = tid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, PORT_SYSTEMs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_dscp_phb_map_mode_set(int unit, bcm_port_t port, int mode)
{
    int ipv4 = -1, ipv6 = -1, port_tab_fld_num = 0;
    ltsw_port_tab_field_t port_tab_field[2];

    SHR_FUNC_ENTER(unit);

    sal_memset(&port_tab_field, 0x0, sizeof(port_tab_field));

    switch (mode) {
        case BCM_PORT_DSCP_MAP_NONE:
            ipv4 = 0;
            ipv6 = 0;
            break;
        case BCM_PORT_DSCP_MAP_ALL:
            ipv4 = 1;
            ipv6 = 1;
            break;
        case BCM_PORT_DSCP_MAP_IPV4_ONLY:
            ipv4 = 1;
            break;
        case BCM_PORT_DSCP_MAP_IPV6_ONLY:
            ipv6 = 1;
            break;
        case BCM_PORT_DSCP_MAP_IPV4_NONE:
            ipv4 = 0;
            break;
        case BCM_PORT_DSCP_MAP_IPV6_NONE:
            ipv6 = 0;
            break;
        case BCM_PORT_DSCP_MAP_ZERO:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (ipv4 > -1) {
        port_tab_field[port_tab_fld_num].type = BCMI_PT_TRUST_PHB_ING_DSCP_V4;
        port_tab_field[port_tab_fld_num].data.n = ipv4;
        port_tab_fld_num++;
    }
    if (ipv6 > -1) {
        port_tab_field[port_tab_fld_num].type = BCMI_PT_TRUST_PHB_ING_DSCP_V6;
        port_tab_field[port_tab_fld_num].data.n = ipv6;
        port_tab_fld_num++;
    }
    if (port_tab_fld_num > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_tab_multi_set(unit, port, port_tab_field,
                                            port_tab_fld_num));
    }
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_dscp_phb_map_mode_get(int unit, bcm_port_t port, int *mode)
{
    int ipv4, ipv6;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_TRUST_PHB_ING_DSCP_V4, &ipv4));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_TRUST_PHB_ING_DSCP_V6, &ipv6));

    if (ipv4 && ipv6) {
        *mode = BCM_PORT_DSCP_MAP_ALL;
    } else if (ipv4) {
        *mode = BCM_PORT_DSCP_MAP_IPV4_ONLY;
    } else if (ipv6) {
        *mode = BCM_PORT_DSCP_MAP_IPV6_ONLY;
    } else {
        *mode = BCM_PORT_DSCP_MAP_NONE;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_class_get(int unit, bcm_port_t port,
                        bcm_port_class_t class, uint32 *class_id)
{
    int id;

    SHR_FUNC_ENTER(unit);

    switch (class) {
        case bcmPortClassFieldLookup:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_ING_PORT_GROUP, &id));
            break;

        case bcmPortClassFieldIngress:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_PORT_SYSTEM_CLASS_ID, &id));
            break;

        case bcmPortClassFieldEgress:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_EGR_PORT_GROUP, &id));
            break;

        case bcmPortClassFieldIngressGroupSel:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_ING_GRP_SEL_CLASS_ID, &id));
            break;
        case bcmPortClassFieldIngressSystemPort:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_PORT_SYSTEM_IPBM_INDEX, &id));
            break;
        case bcmPortClassFieldIngressDevicePort:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_IPBM_INDEX, &id));
            break;
        case bcmPortClassVlanTranslateIngress:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_VFI_ING_ADAPT_PORT_GRP, &id));
            break;
        case bcmPortClassVlanTranslateEgress:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_VFI_EGR_ADAPT_PORT_GRP, &id));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            break;
    }

    *class_id = id;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_class_set(int unit, bcm_port_t port,
                        bcm_port_class_t class, uint32 class_id)
{
    int id = class_id;
    const ltsw_port_tab_info_t *info = NULL;
    uint32_t width = 0;

    SHR_FUNC_ENTER(unit);

    switch (class) {
        case bcmPortClassFieldLookup:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_ING_PORT_GROUP, id));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_PORT_SYSTEM_FP_VLAN_PORT_GRP, id));
            break;

        case bcmPortClassFieldIngress:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_PORT_SYSTEM_CLASS_ID, id));
            break;

        case bcmPortClassFieldEgress:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_EGR_PORT_GROUP, id));
            break;

        case bcmPortClassFieldIngressGroupSel:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_ING_GRP_SEL_CLASS_ID, id));
            break;
        case bcmPortClassFieldIngressSystemPort:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_PORT_SYSTEM_IPBM_INDEX, id));
            break;
        case bcmPortClassFieldIngressDevicePort:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_IPBM_INDEX, id));
            break;
        case bcmPortClassVlanTranslateIngress:
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_port_tab_info_get(unit, BCMI_PT_VFI_ING_ADAPT_PORT_GRP, &info));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_width_get(unit, info->table->name, info->field, &width));
            if (id >= ((1 << width) - 1)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_VFI_ING_ADAPT_PORT_GRP, id));
            break;
        case bcmPortClassVlanTranslateEgress:
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_port_tab_info_get(unit, BCMI_PT_VFI_EGR_ADAPT_PORT_GRP, &info));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_width_get(unit, info->table->name, info->field, &width));
            if (id >= ((1 << width) - 1)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_VFI_EGR_ADAPT_PORT_GRP, id));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_egr_shaper_set(int unit, bcm_port_t port, uint32_t bandwidth,
                             uint32_t burst, bcmi_ltsw_port_shaper_mode_t mode)
{
    ltsw_port_tab_field_t fields[4] = {{0}};
    int fields_num = 0;

    SHR_FUNC_ENTER(unit);

    if ((bandwidth == 0) || (burst == 0)) { /* disabe shaper */
        /* shaper rate 0 to disable shaper */
        fields[fields_num].type = BCMI_PT_SHAPER_RATE;
        fields[fields_num].data.n = 0;
        fields_num ++;

        /* burst is calculated from rate */
        fields[fields_num].type = BCMI_PT_SHAPER_BURST_AUTO;
        fields[fields_num].data.n = 1;
        fields_num ++;
    } else {
        /* shaper mode */
        fields[fields_num].type = BCMI_PT_SHAPER_MODE;
        fields[fields_num].data.n = mode;
        fields_num ++;

        /* shaper rate */
        fields[fields_num].type = BCMI_PT_SHAPER_RATE;
        fields[fields_num].data.n = bandwidth;
        fields_num ++;

        /* shaper burst */
        fields[fields_num].type = BCMI_PT_SHAPER_BURST;
        fields[fields_num].data.n = burst;
        fields_num ++;

        /* burst is not calculated from rate */
        fields[fields_num].type = BCMI_PT_SHAPER_BURST_AUTO;
        fields[fields_num].data.n = 0;
        fields_num ++;
    }

    if (fields_num > 4) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_egr_shaper_get(int unit, bcm_port_t port, uint32_t *bandwidth,
                             uint32_t *burst, bcmi_ltsw_port_shaper_mode_t *mode)
{
    ltsw_port_tab_field_t fields[3] = {{0}};
    int fields_num = 0;

    SHR_FUNC_ENTER(unit);

    /* shaper mode */
    fields[fields_num].type = BCMI_PT_SHAPER_MODE;
    fields_num ++;

    /* real shaper rate */
    fields[fields_num].type = BCMI_PT_SHAPER_RATE_REAL;
    fields_num ++;

    /* real shaper burst */
    fields[fields_num].type = BCMI_PT_SHAPER_BURST_REAL;
    fields_num ++;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, fields, fields_num));

    fields_num = 0;
    *mode = fields[fields_num ++].data.n;
    *bandwidth = fields[fields_num ++].data.n;
    *burst = fields[fields_num ++].data.n;

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_discard_get(int unit, bcm_port_t port, int *mode)
{
    ltsw_port_tab_field_t fields[2] = {{0}};
    int fields_num = 0;
    int drop_tag = 0, drop_untag = 0;

    SHR_FUNC_ENTER(unit);

    fields[fields_num].type = BCMI_PT_DROP_TAG;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_DROP_UNTAG;
    fields_num ++;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, fields, fields_num));
    /* BCMI_PT_DROP_TAG */
    drop_tag = fields[0].data.n ? 1 : 0;
    /* BCMI_PT_DROP_UNTAG */
    drop_untag = fields[1].data.n ? 1 : 0;

    if (drop_tag && drop_untag) {
        *mode = BCM_PORT_DISCARD_ALL;
    } else if (drop_tag) {
        *mode = BCM_PORT_DISCARD_TAG;
    } else if (drop_untag) {
        *mode = BCM_PORT_DISCARD_UNTAG;
    } else {
        *mode = BCM_PORT_DISCARD_NONE;
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_discard_set(int unit, bcm_port_t port, int mode)
{
    ltsw_port_tab_field_t fields[2] = {{0}};
    int fields_num = 0;
    int drop_tag = 0, drop_untag = 0;

    SHR_FUNC_ENTER(unit);

    switch (mode) {
        case BCM_PORT_DISCARD_TAG:
            drop_tag = 1;
            break;
        case BCM_PORT_DISCARD_UNTAG:
            drop_untag = 1;
            break;
        case BCM_PORT_DISCARD_NONE:
            /* Nothing */
            break;
        case BCM_PORT_DISCARD_ALL:
            drop_tag = 1;
            drop_untag = 1;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    fields[fields_num].type = BCMI_PT_DROP_TAG;
    fields[fields_num].data.n = drop_tag ? 1 : 0;
    fields_num ++;

    fields[fields_num].type = BCMI_PT_DROP_UNTAG;
    fields[fields_num].data.n = drop_untag ? 1 : 0;
    fields_num ++;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_egress_set(int unit, bcm_port_t port, int modid, bcm_pbmp_t pbmp)
{
    bcm_pbmp_t mask_pbmp;
    bcm_pbmp_t pbmp_all;
    int tid, id;
    int fld_num = 0, i;
    uint64_t *array_fld_val = NULL;

    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmi_ltsw_port_gport_resolve(unit, port, &modid, &port, &tid, &id));
        if ((tid != -1) || (id != -1)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else if ((!MODID_VALID_RANGE(unit, modid))
            || (!PORT_VALID_RANGE(unit, port))){
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp_all));

    BCM_PBMP_NEGATE(mask_pbmp, pbmp);
    BCM_PBMP_AND(mask_pbmp, pbmp_all);

    fld_num = PORT_NUM_MAX(unit);
    SHR_ALLOC(array_fld_val, sizeof(uint64_t) * fld_num, "ArrayFieldData");
    SHR_NULL_CHECK(array_fld_val, SHR_E_MEMORY);

    for (i = 0; i < fld_num; i++) {
        array_fld_val[i] = BCM_PBMP_MEMBER(mask_pbmp, i) ? 1 : 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_array_set(unit, port,
                                      BCMI_PT_PORT_SYSTEM_BLOCKED_EGR_PORTS,
                                      array_fld_val, fld_num));
exit:
    SHR_FREE(array_fld_val);
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_port_egress_get(int unit, bcm_port_t port, int modid, bcm_pbmp_t *pbmp)
{
    int tid, id;
    int fld_num = 0, i;
    bcm_pbmp_t pbmp_all, pbmp_lb;
    uint64_t *array_fld_val = NULL;

    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmi_ltsw_port_gport_resolve(unit, port, &modid, &port, &tid, &id));
        if ((tid != -1) || (id != -1)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else if ((!MODID_VALID_RANGE(unit, modid))
            || (!PORT_VALID_RANGE(unit, port))){
        SHR_ERR_EXIT(SHR_E_PORT);
    }
    BCM_PBMP_CLEAR(*pbmp);

    fld_num = PORT_NUM_MAX(unit);
    SHR_ALLOC(array_fld_val, sizeof(uint64_t) * fld_num, "ArrayFieldData");
    SHR_NULL_CHECK(array_fld_val, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_array_get(unit, port,
                                      BCMI_PT_PORT_SYSTEM_BLOCKED_EGR_PORTS,
                                      array_fld_val, fld_num));
    for (i = 0; i < fld_num; i++) {
        if (array_fld_val[i] == 0) {
            BCM_PBMP_PORT_ADD(*pbmp, i);
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp_all));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_LB, &pbmp_lb));

    BCM_PBMP_AND(*pbmp, pbmp_all);
    BCM_PBMP_REMOVE(*pbmp, pbmp_lb);

exit:
    SHR_FREE(array_fld_val);
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_port_dump_sw(int unit)
{
    ltsw_port_profile_info_t *prof;
    int pipe, pipe_num;

    LOG_CLI((BSL_META_U(unit, "\nSW Information PORT XGS - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit, "\n  ============== Profile info =============\n\n")));
    LOG_CLI((BSL_META_U(unit, "                   hdl  min  max  size  set\n")));
    prof = PROFILE_PORT_PKT_CTRL(unit);
    if (prof->phd) {
        LOG_CLI((BSL_META_U(unit, "  port_pkt_ctrl    %2d    %d %5d %5d %3d\n"),
                 prof->phd, prof->entry_idx_min, prof->entry_idx_max,
                 prof->entry_size, prof->entries_per_set));
    }
    pipe_num = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    for (pipe = 0; pipe < pipe_num; pipe++) {
        prof = PROFILE_PORT_SYSTEM(unit, pipe);
        if (prof->phd) {
            LOG_CLI((BSL_META_U(unit, "  port_system_%d    %2d    %d %5d %5d %3d\n"),
                     prof->inst, prof->phd, prof->entry_idx_min, prof->entry_idx_max,
                     prof->entry_size, prof->entries_per_set));
        }
    }

    return SHR_E_NONE;
}

