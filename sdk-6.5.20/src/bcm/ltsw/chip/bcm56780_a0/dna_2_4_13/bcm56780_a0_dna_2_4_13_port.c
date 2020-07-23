/*! \file bcm56780_a0_dna_2_4_13_port.c
 *
 * BCM56780_A0 DNA_2_4_13 PORT driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/port.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/xfs/port.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>


/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_PORT

static const char* shaper_mode_list[] = {
    [SHAPER_MODE_BYTE]  = BYTE_MODEs,
    [SHAPER_MODE_PKT]   = PACKET_MODEs,
};
static const ltsw_port_tab_sym_list_t shaper_mode_map = {
    .len    = SHAPER_MODE_CNT,
    .syms   = shaper_mode_list,
};

static const char* dest_type_list[] = {
    [DEST_TYPE_NO_OP]       = NO_OPs,
    [DEST_TYPE_L2_OIF]      = L2_OIFs,
    [DEST_TYPE_RESERVED]    = RESERVEDs,
    [DEST_TYPE_VP]          = VPs,
    [DEST_TYPE_ECMP]        = ECMPs,
    [DEST_TYPE_NHOP]        = NHOPs,
    [DEST_TYPE_L2MC_GROUP]  = L2MC_GROUPs,
    [DEST_TYPE_L3MC_GROUP]  = L3MC_GROUPs,
    [DEST_TYPE_RESERVED_1]  = RESERVED_1s,
    [DEST_TYPE_ECMP_MEMBER] = ECMP_MEMBERs,
};
static const ltsw_port_tab_sym_list_t dest_type_map = {
    .len    = DEST_TYPE_CNT,
    .syms   = dest_type_list,
};

static const int ing_port_type_list[] = {
    [PORT_TYPE_ETHERNET]    = 1,
    [PORT_TYPE_SYSTEM]      = 2,
};
static const ltsw_port_tab_int_list_t ing_port_type_map = {
    .len    = PORT_TYPE_CNT,
    .vals   = ing_port_type_list,
};

static const int egr_ing_port_type_list[] = {
    [PORT_TYPE_ETHERNET]    = 0,
    [PORT_TYPE_SYSTEM]      = 3,
    [PORT_TYPE_CPU]         = 1,
};
static const ltsw_port_tab_int_list_t egr_ing_port_type_map = {
    .len    = PORT_TYPE_CNT,
    .vals   = egr_ing_port_type_list,
};

static const int egr_port_type_list[] = {
    [PORT_TYPE_ETHERNET]                                = 0,
    [PORT_TYPE_SYSTEM]                                  = 3,
    [PORT_TYPE_LOOPBACK]                                = 2,
    [PORT_TYPE_CPU]                                     = 1,
    [TX_BYTE_ENABLE | PORT_TYPE_ETHERNET]               = 4 | 0,
    [TX_BYTE_ENABLE | PORT_TYPE_SYSTEM]                 = 4 | 3,
    [TX_BYTE_ENABLE | PORT_TYPE_LOOPBACK]               = 4 | 2,
    [TX_BYTE_ENABLE | PORT_TYPE_CPU]                    = 4 | 1,
    [ECN_ENABLE | PORT_TYPE_ETHERNET]                   = 8 | 0,
    [ECN_ENABLE | PORT_TYPE_SYSTEM]                     = 8 | 3,
    [ECN_ENABLE | PORT_TYPE_LOOPBACK]                   = 8 | 2,
    [ECN_ENABLE | PORT_TYPE_CPU]                        = 8 | 1,
    [TX_BYTE_ENABLE | ECN_ENABLE | PORT_TYPE_ETHERNET]  = 4 | 8 | 0,
    [TX_BYTE_ENABLE | ECN_ENABLE | PORT_TYPE_SYSTEM]    = 4 | 8 | 3,
    [TX_BYTE_ENABLE | ECN_ENABLE | PORT_TYPE_LOOPBACK]  = 4 | 8 | 2,
    [TX_BYTE_ENABLE | ECN_ENABLE | PORT_TYPE_CPU]       = 4 | 8 | 1,
};
static const ltsw_port_tab_int_list_t egr_port_type_map = {
    .len    = sizeof(egr_port_type_list) / sizeof(egr_port_type_list[0]),
    .vals   = egr_port_type_list,
};

static const int port_parser_type_list[] = {
    [PARSER_TYPE_ETHERNET]  = 3,
    [PARSER_TYPE_SYSTEM]    = 11,
    [PARSER_TYPE_LOOPBACK]  = 1,
};
static const ltsw_port_tab_int_list_t port_parser_type_map = {
    .len    = PARSER_TYPE_CNT,
    .vals   = port_parser_type_list,
};

static const ltsw_port_tab_com_fld_t sample_id_fld_list[] = {
    {SAMPLER_INDEX_3_0s,    4,  0},
    {SAMPLER_INDEX_5_4s,    2,  4},
};

static const ltsw_port_tab_com_fld_list_t sample_id_fld_map = {
    .len    = sizeof(sample_id_fld_list)/sizeof(sample_id_fld_list[0]),
    .flds   = sample_id_fld_list,
};

static const ltsw_port_tab_com_fld_t egr_mask_id_fld_list[] = {
    {BITMAP_PROFILE_INDEXs,         8,  0},
    {BITMAP_PROFILE_SECTION_SELs,   2,  8},
};

static const ltsw_port_tab_com_fld_list_t egr_mask_id_fld_map = {
    .len    = sizeof(egr_mask_id_fld_list)/sizeof(egr_mask_id_fld_list[0]),
    .flds   = egr_mask_id_fld_list,
};

static const char* l2_oif_view_list[] = {
    [L2_OIF_VIEW_ITPID]     = VT_L2_OIF_ITPIDs,
    [L2_OIF_VIEW_FLEX_CNTR] = VT_L2_OIF_FLEX_CNTRs,
};
static const ltsw_port_tab_sym_list_t l2_oif_view_map = {
    .len    = L2_OIF_VIEW_CNT,
    .syms   = l2_oif_view_list,
};

static const ltsw_port_tab_lt_info_t ing_system_port_table          = {ING_SYSTEM_PORT_TABLEs, SYSTEM_SOURCEs, xfs_ltsw_gport_to_sys_port};
static const ltsw_port_tab_lt_info_t ing_system_destination_table   = {ING_SYSTEM_DESTINATION_TABLEs, SYSTEM_DESTINATION_PORTs, xfs_ltsw_gport_to_sys_port};
static const ltsw_port_tab_lt_info_t port_system_destination        = {PORT_SYSTEM_DESTINATIONs, PORT_SYSTEM_IDs, xfs_ltsw_gport_to_sys_port};
static const ltsw_port_tab_lt_info_t ing_l2_iif_table               = {ING_L2_IIF_TABLEs, L2_IIFs, xfs_ltsw_gport_to_l2_if};
static const ltsw_port_tab_lt_info_t ing_l2_iif_attributes_table    = {ING_L2_IIF_ATTRIBUTES_TABLEs, L2_IIFs, xfs_ltsw_gport_to_l2_if};
static const ltsw_port_tab_lt_info_t egr_l2_oif                     = {EGR_L2_OIFs, L2_OIFs, xfs_ltsw_gport_to_l2_if};
static const ltsw_port_tab_lt_info_t l2_eif_system_destination      = {L2_EIF_SYSTEM_DESTINATIONs, L2_EIF_IDs, xfs_ltsw_gport_to_l2_if};
static const ltsw_port_tab_lt_info_t port                           = {PORTs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_property                  = {PORT_PROPERTYs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_policy                    = {PORT_POLICYs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_ing_visibility            = {PORT_ING_VISIBILITYs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_egr_visibility            = {PORT_EGR_VISIBILITYs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_ing_ts_ptp                = {PORT_ING_TS_PTPs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_mirror                    = {PORT_MIRRORs, PORT_IDs};
static const ltsw_port_tab_lt_info_t mirror_port_encap_sflow        = {MIRROR_PORT_ENCAP_SFLOWs, PORT_IDs};
static const ltsw_port_tab_lt_info_t tm_shaper_port                 = {TM_SHAPER_PORTs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_egr_opaque                = {PORT_EGR_OPAQUEs, PORT_IDs};
static const ltsw_port_tab_lt_info_t r_hg3_ethertype                = {R_HG3_ETHERTYPEs};
static const ltsw_port_tab_lt_info_t r_egr_hg3_etype_reserved       = {R_EGR_HG3_ETYPE_RESERVEDs};
static const ltsw_port_tab_lt_info_t port_ing_block_loopback        = {PORT_ING_BLOCK_LOOPBACKs};
static const ltsw_port_tab_lt_info_t port_ing_egr_block_cpu         = {PORT_ING_EGR_BLOCK_CPUs};
static const ltsw_port_tab_lt_info_t port_ing_egr_block_profile     = {PORT_ING_EGR_BLOCK_PROFILEs, PORT_ING_EGR_BLOCK_PROFILE_IDs};

static const ltsw_port_tab_info_t port_tab_map[BCMI_PT_CNT] = {
    [BCMI_PT_ING_OTPID_ENABLE]      = {&ing_system_port_table, OUTER_L2HDR_OTPID_ENABLEs},
    [BCMI_PT_ING_PAYLOAD_OTPID_ENABLE]  = {&ing_system_port_table, INNER_L2HDR_OTPID_ENABLEs},
    [BCMI_PT_ING_L2_IIF]            = {&ing_system_port_table, L2_IIFs},
    [BCMI_PT_ING_PP_PORT]           = {&ing_system_port_table, INGRESS_PP_PORTs},
    [BCMI_PT_LB_HASH_PORT_LB_NUM]   = {&ing_system_port_table, LOAD_BALANCE_NUMBERs},
    [BCMI_PT_RANGE_MAP_INDEX]       = {&ing_system_port_table, RANGE_TABLE_INDEXs},
    [BCMI_PT_ING_FLEX_CTR_ID]       = {&ing_system_port_table, FLEX_CTR_ACTIONs},
    [BCMI_PT_EGR_MASK_ID]           = {&ing_system_port_table, NULL, FLD_IS_COMBINED, &egr_mask_id_fld_map},
    [BCMI_PT_IFP_PBM_INDEX]         = {&ing_system_port_table, IPBM_INDEXs},
    [BCMI_PT_SYS_PORT_STR_PROFILE_ID]   = {&ing_system_port_table, STRENGTH_PROFILE_INDEXs},
    [BCMI_PT_ING_SYS_DEST_L2_OIF]   = {&ing_system_destination_table, L2_OIFs},
    [BCMI_PT_ING_SYS_DEST_L2_OIF_STR]   = {&ing_system_destination_table, L2_OIF_STRENGTHs},
    [BCMI_PT_SYS_DEST_STR_PROFILE_ID]   = {&ing_system_destination_table, STRENGTH_PROFILE_INDEXs},
    [BCMI_PT_L2MC_L3MC_L2_OIF_SYS_DST_VALID_STR]    = {&ing_system_destination_table, L2MC_L3MC_L2_OIF_SYS_DST_VALID_STRENGTHs},
    [BCMI_PT_DEFAULT_VFI]           = {&ing_l2_iif_table, VFIs, SYNC_TO_TRUNK},
    [BCMI_PT_DEFAULT_PRI]           = {&ing_l2_iif_table, DEFAULT_OTAG_DOT1P_CFIs, SYNC_TO_TRUNK},
    [BCMI_PT_DEFAULT_INNER_PRI]     = {&ing_l2_iif_table, DEFAULT_ITAG_DOT1P_CFIs, SYNC_TO_TRUNK},
    [BCMI_PT_CML_NEW]               = {&ing_l2_iif_table, CML_FLAGS_NEWs, SYNC_TO_TRUNK},
    [BCMI_PT_CML_MOVE]              = {&ing_l2_iif_table, CML_FLAGS_MOVEs, SYNC_TO_TRUNK},
    [BCMI_PT_VLAN_MAP_TO_VFI]       = {&ing_l2_iif_table, VLAN_TO_VFI_MAPPING_ENABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_VLAN_PRESERVE]         = {&ing_l2_iif_table, VLAN_TAG_PRESERVE_CTRLs, SYNC_TO_TRUNK},
    [BCMI_PT_ING_PORT_GROUP]        = {&ing_l2_iif_table, PORT_GROUP_IDs, SYNC_TO_TRUNK},
    [BCMI_PT_ING_CLASS_ID]          = {&ing_l2_iif_table, CLASS_IDs, SYNC_TO_TRUNK},
    [BCMI_PT_ING_PVLAN_EN]          = {&ing_l2_iif_table, PVLAN_CHECK_ENABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_ING_VT_EN]             = {&ing_l2_iif_table, VLAN_XLATE_ENABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_IPV4_TUNNEL_EN]        = {&ing_l2_iif_table, IPV4_TUNNEL_ENABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_IPV6_TUNNEL_EN]        = {&ing_l2_iif_table, IPV6_TUNNEL_ENABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_ING_L3_IIF]            = {&ing_l2_iif_table, L3_IIFs, SYNC_TO_TRUNK},
    [BCMI_PT_MPLS]                  = {&ing_l2_iif_table, MPLS_ENABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_VN_ID_LOOKUP_CTRL]     = {&ing_l2_iif_table, VN_ID_LOOKUP_CTRLs, SYNC_TO_TRUNK},
    [BCMI_PT_VXLAN_EN]              = {&ing_l2_iif_table, VXLAN_ENABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_ING_OPAQUE_CTRL_ID]    = {&ing_l2_iif_table, OPAQUE_CTRL_IDs, SYNC_TO_TRUNK},
    [BCMI_PT_ING_OPAQUE_CTRL_ID_1]  = {&ing_l2_iif_table, OPAQUE_CTRL_ID_1s, SYNC_TO_TRUNK},
    [BCMI_PT_L2_DEST]               = {&ing_l2_iif_table, DESTINATIONs, SYNC_TO_TRUNK},
    [BCMI_PT_L2_DEST_TYPE]          = {&ing_l2_iif_table, DESTINATION_TYPEs, FLD_IS_SYMBOL | SYNC_TO_TRUNK, &dest_type_map},
    [BCMI_PT_NHOP_2_OR_ECMP_GROUP_INDEX_1]  = {&ing_l2_iif_table, NHOP_2_OR_ECMP_GROUP_INDEX_1s, SYNC_TO_TRUNK},
    [BCMI_PT_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX]  = {&ing_l2_iif_table, ECMP_AND_PROT_SWT_SFC_PROFILE_INDEXs, SYNC_TO_TRUNK},
    [BCMI_PT_OVLAN_MAP_TO_PHB]      = {&ing_l2_iif_attributes_table, USE_OTAG_DOT1P_CFI_FOR_PHBs, SYNC_TO_TRUNK},
    [BCMI_PT_IVLAN_MAP_TO_PHB]      = {&ing_l2_iif_attributes_table, USE_ITAG_DOT1P_CFI_FOR_PHBs, SYNC_TO_TRUNK},
    [BCMI_PT_DSCP_MAP_TO_PHB]       = {&ing_l2_iif_attributes_table, USE_DSCP_FOR_PHBs, SYNC_TO_TRUNK},
    [BCMI_PT_VLAN_PHB]              = {&ing_l2_iif_attributes_table, DOT1P_PTRs, SYNC_TO_TRUNK},
    [BCMI_PT_DSCP_PHB]              = {&ing_l2_iif_attributes_table, DSCP_PTRs, SYNC_TO_TRUNK},
    [BCMI_PT_QOS_REMARK_CTRL]       = {&ing_l2_iif_attributes_table, QOS_REMARK_CTRLs, SYNC_TO_TRUNK},
    [BCMI_PT_MIRROR_ID_0]           = {&ing_l2_iif_attributes_table, MIRROR_INDEX_0s, SYNC_TO_TRUNK},
    [BCMI_PT_MIRROR_ID_1]           = {&ing_l2_iif_attributes_table, MIRROR_INDEX_1s, SYNC_TO_TRUNK},
    [BCMI_PT_MIRROR_ID_2]           = {&ing_l2_iif_attributes_table, MIRROR_INDEX_2s, SYNC_TO_TRUNK},
    [BCMI_PT_MIRROR_ID_3]           = {&ing_l2_iif_attributes_table, MIRROR_INDEX_3s, SYNC_TO_TRUNK},
    [BCMI_PT_SAMPLE_ID]             = {&ing_l2_iif_attributes_table, NULL, FLD_IS_COMBINED | SYNC_TO_TRUNK, &sample_id_fld_map},
    [BCMI_PT_SC_METER_INDEX]        = {&ing_l2_iif_attributes_table, STORM_CTRL_METER_INDEXs, SYNC_TO_TRUNK},
    [BCMI_PT_SC_BC_EN]              = {&ing_l2_iif_attributes_table, STORM_CTRL_BCAST_ENABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_SC_MC_EN]              = {&ing_l2_iif_attributes_table, STORM_CTRL_MCAST_ENABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_SC_UMC_EN]             = {&ing_l2_iif_attributes_table, STORM_CTRL_UMCAST_ENABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_SC_UUC_EN]             = {&ing_l2_iif_attributes_table, STORM_CTRL_UUCAST_ENABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_ING_FLEX_STATE_ID]     = {&ing_l2_iif_attributes_table, FLEX_STATE_ACTIONs, SYNC_TO_TRUNK},
    [BCMI_PT_STATIC_MOVE_DROP]      = {&ing_l2_iif_attributes_table, STATIC_MOVE_DROPs, SYNC_TO_TRUNK},
    [BCMI_PT_BRIDGE]                = {&ing_l2_iif_attributes_table, SRC_PORT_KNOCKOUT_DISABLEs, SYNC_TO_TRUNK},
    [BCMI_PT_HONOR_SRC_DISCARD_ON_STN_MOVE] = {&ing_l2_iif_attributes_table, HONOR_SRC_DISCARD_ON_STN_MOVEs, SYNC_TO_TRUNK},
    [BCMI_PT_EGR_L2_OIF_VIEW]       = {&egr_l2_oif, VIEW_Ts, FLD_IS_SYMBOL | SYNC_TO_TRUNK, &l2_oif_view_map},
    [BCMI_PT_EGR_OTPID]             = {&egr_l2_oif, TPID_0s, SYNC_TO_TRUNK},
    [BCMI_PT_EGR_ITPID]             = {&egr_l2_oif, TPID_1s, SYNC_TO_TRUNK},
    [BCMI_PT_EGR_PORT_GROUP]        = {&egr_l2_oif, EGR_PORT_GROUP_IDs, SYNC_TO_TRUNK},
    [BCMI_PT_EGR_L2_PROCESS_CTRL]   = {&egr_l2_oif, L2_PROCESS_CTRLs, SYNC_TO_TRUNK},
    [BCMI_PT_PHB_VLAN]              = {&egr_l2_oif, DOT1P_REMARK_BASE_PTRs, SYNC_TO_TRUNK},
    [BCMI_PT_PHB_DSCP]              = {&egr_l2_oif, TOS_REMARK_BASE_PTRs, SYNC_TO_TRUNK},
    [BCMI_PT_PHB_VLAN_STR]          = {&egr_l2_oif, DOT1P_PTR_STRs, SYNC_TO_TRUNK},
    [BCMI_PT_EGR_OPAQUE_CTRL_ID]    = {&egr_l2_oif, EFP_CTRL_IDs, SYNC_TO_TRUNK},
    [BCMI_PT_EGR_FLEX_CTR_ID]       = {&egr_l2_oif, FLEX_CTR_ACTIONs, SYNC_TO_TRUNK},
    [BCMI_PT_L2_OIF_STR_PROFILE_ID] = {&egr_l2_oif, STRENGTH_PRFL_IDXs},
    [BCMI_PT_L2_OIF_IS_TRUNK]       = {&l2_eif_system_destination, IS_TRUNKs},
    [BCMI_PT_L2_OIF_SYS_PORT]       = {&l2_eif_system_destination, SYSTEM_PORTs},
    [BCMI_PT_DEST_SYS_PORT_IS_TRUNK]= {&port_system_destination, IS_TRUNK_SYSTEMs},
    [BCMI_PT_DEST_SYS_PORT_LPORT]   = {&port_system_destination, PORT_IDs},
    [BCMI_PT_DEST_SYS_PORT_TRUNK]   = {&port_system_destination, TRUNK_SYSTEM_IDs},
    [BCMI_PT_DEST_SYS_PORT_DLB_ID_VALID] = {&port_system_destination, DLB_ID_VALIDs},
    [BCMI_PT_DEST_SYS_PORT_DLB_ID]  = {&port_system_destination, DLB_IDs},
    [BCMI_PT_ING_SYS_PORT]          = {&port, ING_SYSTEM_PORT_TABLE_IDs},
    [BCMI_PT_EGR_MC_L2_OIF]         = {&port, L2_EIF_IDs},
    [BCMI_PT_MTU_EN]                = {&port, MTU_CHECKs},
    [BCMI_PT_MTU]                   = {&port, MTUs},
    [BCMI_PT_ING_PORT_TYPE]         = {&port_property, PORT_TYPEs, FLD_IS_ENUM, &ing_port_type_map},
    [BCMI_PT_EGR_PORT_TYPE]         = {&port_property, EGR_PORT_PROPERTYs, FLD_IS_ENUM, &egr_port_type_map},
    [BCMI_PT_EGR_ING_PORT_TYPE]     = {&port_property, ING_PORT_PROPERTYs, FLD_IS_ENUM, &egr_ing_port_type_map},
    [BCMI_PT_PORT_PARSER_TYPE]      = {&port_property, PORT_PARSERs, FLD_IS_ENUM, &port_parser_type_map},
    [BCMI_PT_PORT_COS_QUEUE_MAP_ID] = {&port_policy, PORT_COS_Q_MAP_IDs},
    [BCMI_PT_SHAPER_MODE]           = {&tm_shaper_port, SHAPING_MODEs, FLD_IS_SYMBOL, &shaper_mode_map},
    [BCMI_PT_SHAPER_RATE]           = {&tm_shaper_port, BANDWIDTH_KBPSs},
    [BCMI_PT_SHAPER_RATE_REAL]      = {&tm_shaper_port, BANDWIDTH_KBPS_OPERs},
    [BCMI_PT_SHAPER_BURST]          = {&tm_shaper_port, BURST_SIZE_KBITSs},
    [BCMI_PT_SHAPER_BURST_REAL]     = {&tm_shaper_port, BURST_SIZE_KBITS_OPERs},
    [BCMI_PT_SHAPER_BURST_AUTO]     = {&tm_shaper_port, BURST_SIZE_AUTOs},
    [BCMI_PT_SHAPER_IFG_ADJ]        = {&tm_shaper_port, INTER_FRAME_GAP_BYTEs},
    [BCMI_PT_SHAPER_IFG_ADJ_EN]     = {&tm_shaper_port, INTER_FRAME_GAP_ENCAPs},
    [BCMI_PT_ING_RXTS_INSERT]       = {&port_ing_visibility, TIMESTAMPs},
    [BCMI_PT_ING_TXTS_INSERT]       = {&port_ing_visibility, TX_TIMESTAMPs},
    [BCMI_PT_ING_TXTS_CANCEL]       = {&port_ing_visibility, SKIP_TX_TIMESTAMPs},
    [BCMI_PT_EGR_TXTS_INSERT]       = {&port_egr_visibility, TIMESTAMPs},
    [BCMI_PT_EGR_TS_CANCEL]         = {&port_egr_visibility, SKIP_TIMESTAMPs},
    [BCMI_PT_ING_TS_OID]            = {&port_ing_visibility, TIMESTAMP_ORIGINs},
    [BCMI_PT_EGR_TS_OID]            = {&port_egr_visibility, TIMESTAMP_ORIGINs},
    [BCMI_PT_EGR_TS_ADJUST]         = {&port_egr_visibility, ADJUST_METERSs},
    [BCMI_PT_1588_LINK_DELAY]       = {&port_ing_ts_ptp, LINK_DELAYs},
    [BCMI_PT_MIRROR_CPU]            = {&port_mirror, CPUs},
    [BCMI_PT_EGR_SFLOW_EN]          = {&mirror_port_encap_sflow, SAMPLE_EGRs},
    [BCMI_PT_EGR_SFLOW_RATE]        = {&mirror_port_encap_sflow, SAMPLE_EGR_RATEs},
    [BCMI_PT_EGR_OPAQUE_OBJ0]       = {&port_egr_opaque, OPAQUE_OBJ0s},
    [BCMI_PT_ING_HG3_ETYPE]         = {&r_hg3_ethertype, TYPEs},
    [BCMI_PT_EGR_HG3_ETYPE]         = {&r_egr_hg3_etype_reserved, TYPEs},
    [BCMI_PT_LOOPBACK_EN]           = {&port_ing_block_loopback, MASK_PORTSs, FLD_IS_ARRAY},
    [BCMI_PT_CPU_EN]                = {&port_ing_egr_block_cpu, MASK_PORTSs, FLD_IS_ARRAY},
    [BCMI_PT_ING_EGR_VLAN_MEMBERSHIP_CHECK] = {&port_ing_egr_block_profile, SKIP_VLAN_ING_EGR_MEMBERSHIP_CHECKs},
    [BCMI_PT_ING_EGR_BLOCK_EN]      = {&port_ing_egr_block_profile, SKIP_ING_EGR_BLOCKs, FLD_IS_ARRAY},
};

static const xfs_ltsw_port_feature_t feature = {
    .l2_mtu = 0,
    .egr_vlan_pri_src = 1,
};

static const xfs_ltsw_port_db_t port_db = {
    &feature,
    port_tab_map,
};

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Initialize port feature database.
 *
 * \param [in] unit Unit Number.
 * \param [in|out] feature feature database pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_dna_2_4_13_ltsw_port_db_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_db_set(unit, &port_db));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief port driver function variable for bcm56780_a0 DNA_2_4_13 device.
 */
static mbcm_ltsw_port_drv_t bcm56780_a0_dna_2_4_13_ltsw_port_drv = {
    .port_db_init   = bcm56780_a0_dna_2_4_13_ltsw_port_db_init,
};


/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_dna_2_4_13_ltsw_port_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_port_drv_set(unit, &bcm56780_a0_dna_2_4_13_ltsw_port_drv));

exit:
    SHR_FUNC_EXIT();
}


