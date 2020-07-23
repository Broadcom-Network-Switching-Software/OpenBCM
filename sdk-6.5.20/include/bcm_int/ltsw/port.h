/*! \file port.h
 *
 * Port Module Emulator above SDKLT
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_LTSW_PORT_H
#define BCM_LTSW_PORT_H

#include <bcm/types.h>

#include <sal/sal_types.h>


/*!
 * \brief Port type definitions.
 */
/*! Ethernet port. */
#define BCMI_LTSW_PORT_TYPE_ETH     (1 << 0)
/*! HG port. */
#define BCMI_LTSW_PORT_TYPE_HG      (1 << 1)
/*! CPU port. */
#define BCMI_LTSW_PORT_TYPE_CPU     (1 << 2)
/*! Management port. */
#define BCMI_LTSW_PORT_TYPE_MGMT    (1 << 3)
/*! Internal loopback port. */
#define BCMI_LTSW_PORT_TYPE_LB      (1 << 4)
/*! Spare port. */
#define BCMI_LTSW_PORT_TYPE_SPARE   (1 << 5)
/*! Management port ability. */
#define BCMI_LTSW_PORT_TYPE_MA      (1 << 6)
/*! XLPORT. */
#define BCMI_LTSW_PORT_TYPE_XL      (1 << 7)
/*! CLPORT. */
#define BCMI_LTSW_PORT_TYPE_CL      (1 << 8)
/*! CDPORT. */
#define BCMI_LTSW_PORT_TYPE_CD      (1 << 9)
/*! GE PORT. */
#define BCMI_LTSW_PORT_TYPE_GE      (1 << 10)
/*! XE PORT. */
#define BCMI_LTSW_PORT_TYPE_XE      (1 << 11)
/*! CE PORT. */
#define BCMI_LTSW_PORT_TYPE_CE      (1 << 12)
/*! CDE PORT. */
#define BCMI_LTSW_PORT_TYPE_CDE     (1 << 13)

/*! EP recirculation data buffer port. */
#define BCMI_LTSW_PORT_TYPE_RDB     (1 << 14)

/*! Front pannel port. */
#define BCMI_LTSW_PORT_TYPE_PORT    (BCMI_LTSW_PORT_TYPE_ETH   \
                                    | BCMI_LTSW_PORT_TYPE_HG)
/*! All valid port. */
#define BCMI_LTSW_PORT_TYPE_ALL     (BCMI_LTSW_PORT_TYPE_PORT   \
                                    | BCMI_LTSW_PORT_TYPE_CPU   \
                                    | BCMI_LTSW_PORT_TYPE_MGMT  \
                                    | BCMI_LTSW_PORT_TYPE_LB    \
                                    | BCMI_LTSW_PORT_TYPE_RDB)

/*!
 * \brief Port name max length.
 */
#define BCMI_LTSW_PORT_NAME_MAX     8

/*!
 * \brief Flags of direction.
 */
#define BCMI_LTSW_PORT_ING          0x1
#define BCMI_LTSW_PORT_EGR          0x2

/*!
 * \brief Shared block mask section.
 */
#define BCMI_LTSW_BLOCK_MASK_SECTION_SHARED     0


/*!
 * \brief Fields of LTs mapped to LPORT_TAB profile.
 */
typedef struct bcmi_ltsw_lport_tab_lt_fld_info_s {
    /*! LT field name */
    const char *lt_fld_name;
    /*! LT name */
    const char *lt_name;
    /*! LT field flags */
    uint32_t flags;
    /*! Index number of array field */
    uint32_t index_num;
} bcmi_ltsw_lport_tab_lt_fld_info_t;

/*!
 * \brief LPORT_TAB profile information.
 *
 * This data structure defines the information in LPORT_TAB profile.
 */
typedef struct bcmi_ltsw_lport_tab_s {
    /*!
     * Enable dropping packets that are not a member of the port's ingress VLAN.
     * LT PORT_SYSTEM_MEMBERSHIP_POLICY_PROFILE.ING_VLAN_MEMBERSHIP_CHECK.
     */
    uint32_t en_ifilter                  : 1;
    /*!
     * Enable to do TM mirror on drop.
     * LT PORT_SYSTEM_TM_MIRROR_ON_DROP_PROFILE.MIRROR_ON_DROP.
     */
    uint32_t mirror_on_drop_mmu_eligible : 1;
    /*!
     * Index into the TM_MIRROR_ON_DROP_PROFILE table.
     * LT PORT_SYSTEM_TM_MIRROR_ON_DROP_PROFILE.TM_MIRROR_ON_DROP_PROFILE_ID.
     */
    uint32_t mirror_on_drop_mmu_profile  : 8;
    /*!
     * Enable ingress private VLAN.
     * LT PORT_SYSTEM_PVLAN_PROFILE.ING_PVLAN.
     */
    uint32_t pvlan_enable                : 1;
    /*!
     * Enable to trust incoming IPv4 DSCP and map to PHB.
     * LT PORT_SYSTEM_PHB_PROFILE.TRUST_PHB_ING_DSCP_V4.
     */
    uint32_t trust_dscp_v4               : 1;
    /*!
     * Enable to trust incoming IPv6 DSCP and map to PHB.
     * LT PORT_SYSTEM_PHB_PROFILE.TRUST_PHB_ING_DSCP_V6.
     */
    uint32_t trust_dscp_v6               : 1;
    /*!
     * PHB_ING_IP_DSCP_TO_INT_PRI_REMAP logical table index.
     * Applicable only if TRUST_PHB_ING_DSCP_V4 is enabled for IPv4 packets
     * and TRUST_PHB_ING_DSCP_V6 is enabled for IPv6 packets.
     * LT PORT_SYSTEM_PHB_PROFILE.PHB_ING_IP_DSCP_TO_INT_PRI_REMAP_ID.
     */
    uint32_t trust_dscp_ptr              : 8;
    /*!
     * PHB_ING_L2_TAGGED_TO_INT_PRI logical table index or
     * PHB_ING_L2_UNTAGGED_TO_INT_PRI logical table index depending on
     * whether the packet is tagged or untagged.
     * LT PORT_SYSTEM_PHB_PROFILE.PHB_ING_L2_ID.
     */
    uint32_t trust_dot1p_ptr             : 8;
    /*!
     * PHB_ING_L2_OTAG_REMAP logical table index.
     * LT PORT_SYSTEM_PHB_PROFILE.PHB_ING_L2_OTAG_REMAP_ID.
     */
    uint32_t dot1p_remap_pointer         : 8;
    /*!
     * cml_flags_new[0] : Enable dropping packets with unknown source MAC address.
     * cml_flags_new[1] : Enable copying to CPU for packets with unknown source MAC address.
     * cml_flags_new[3] : Enable MAC address learning.
     * LT PORT_SYSTEM_LEARN_PROFILE.MAC_DROP, MAC_COPY_TO_CPU, MAC_LEARN.
     */
    uint32_t drop                        : 1;
    uint32_t tocpu                       : 1;
    uint32_t hw_learn                    : 1;
    /*!
     * Enable L2 forwarding of broadcast, unknown unicast and multicast packets.
     * LT PORT_SYSTEM_BRIDGE_PROFILE.BRIDGE.
     */
    uint32_t bridge                      : 1;
    /*!
     * Enable trusting incoming VLAN tag.
     * LT PORT_SYSTEM_POLICY_PROFILE.TRUST_INCOMING_VID.
     */
    uint32_t trust_incoming_vid          : 1;
    /*!
     * Enable dropping untagged packets.
     * LT PORT_SYSTEM_POLICY_PROFILE.DROP_UNTAG.
     */
    uint32_t port_dis_untag              : 1;
    /*!
     * Enable dropping tagged packets.
     * LT PORT_SYSTEM_POLICY_PROFILE.DROP_TAG.
     */
    uint32_t port_dis_tag                : 1;
    /*!
     * Enable passing pause frames.
     * LT PORT_SYSTEM_POLICY_PROFILE.PASS_PAUSE_FRAMES.
     */
    uint32_t pass_control_frames         : 1;
    /*!
     * Enable matching of packet TPID idx against configured VLAN.VLAN_OUTER_TPID_ID
     * and drop on mismatch.
     * LT PORT_SYSTEM_POLICY_PROFILE.OUTER_TPID_VERIFY.
     */
    uint32_t outer_tpid_verify           : 1;
    /*!
     * Enable matching of packet TPID against the VLAN_OUTER_TPID logical table.
     * Array is indexed by VLAN_OUTER_TPID_ID.
     * LT PORT_SYSTEM_POLICY_PROFILE.PASS_ON_OUTER_TPID_MATCH[4].
     */
    uint32_t outer_tpid_enable           : 4;
    /*!
     * Enable dropping BPDU packets.
     * LT PORT_SYSTEM_POLICY_PROFILE.DROP_BPDU.
     */
    uint32_t drop_bpdu                   : 1;
    /*!
     * If enabled, ingress CFI carries QoS (drop precedence) information per TPID.
     * Array is indexed by VLAN_OUTER_TPID_ID.
     * LT PORT_SYSTEM_POLICY_PROFILE.ING_CFI_AS_CNG[4].
     */
    uint32_t cfi_as_cng                  : 4;
    /*!
     * Enable VLAN FP.
     * LT PORT_SYSTEM_FP_PROFILE.FP_VLAN.
     */
    uint32_t vfp_enable                  : 1;
    /*!
     * Enable Ingress FP.
     * LT PORT_SYSTEM_FP_PROFILE.FP_ING.
     */
    uint32_t filter_enable               : 1;
    /*!
     * VLAN FP key port group.
     * LT PORT_SYSTEM_FP_PROFILE.FP_VLAN_PORT_GRP.
     */
    uint32_t vfp_port_group_id           : 9;
    /*!
     * VLAN FP key port group.
     * LT PORT_SYSTEM_FP_PROFILE.USE_TABLE_FP_VLAN_PORT_GRP.
     */
    uint32_t use_port_table_group_id     : 1;
    /*!
     * Ingress FP group selection class ID.
     * LT PORT_SYSTEM_FP_PROFILE.FP_ING_GRP_SEL_CLASS_ID.
     */
    uint32_t ifp_key_sel_class_id        : 16;
    /*!
     * Ingress FP group selection class ID.
     * LT PORT_SYSTEM_PROFILE.V6L3.
     */
    uint32_t v6l3_enable                 : 1;
    /*!
     * Enable L3 forwarding for IPv6 multicast packets.
     * LT PORT_SYSTEM_PROFILE.V6IPMC.
     */
    uint32_t v6ipmc_enable               : 1;
    /*!
     * Enable L3 forwarding for IPv4 unicast packets.
     * LT PORT_SYSTEM_PROFILE.V4L3.
     */
    uint32_t v4l3_enable                 : 1;
    /*!
     * Enable L3 forwarding for IPv4 multicast packets.
     * LT PORT_SYSTEM_PROFILE.V4IPMC.
     */
    uint32_t v4ipmc_enable               : 1;
    /*!
     * Operating mode of the port.
     * LT PORT_SYSTEM_PROFILE.OPERATING_MODE.
     */
    uint32_t port_operation              : 3;
    /*!
     * PORT_PKT_CONTROL logical table index.
     * LT PORT_SYSTEM_PROFILE.PORT_PKT_CONTROL_ID.
     */
    uint32_t protocol_pkt_index          : 8;
    /*!
     * Enable MPLS.
     * LT PORT_SYSTEM_PROFILE.MPLS.
     */
    uint32_t mpls                        : 1;
    /*!
     * Enable using L3_IIF as part of IP multicast search key.
     * LT PORT_SYSTEM_PROFILE.IPMC_USE_L3_IIF.
     */
    uint32_t ipmc_do_vlan                : 1;
    /*!
     * Enable times-sync.
     * LT PORT_SYSTEM_PROFILE.IEEE_802_1AS.
     */
    uint32_t ieee_802_1as_enable         : 1;
    /*!
     * VLAN_ING_TAG_ACTION_PROFILE logical table index.
     * LT PORT_SYSTEM_PROFILE.VLAN_ING_TAG_ACTION_PROFILE_ID.
     */
    uint32_t tag_action_profile_ptr      : 8;
    /*!
     * Ingress outer VLAN ID.
     * LT PORT_SYSTEM_PROFILE.ING_OVID.
     */
    uint32_t port_vid                    : 12;
    /*!
     * Ingress outer DOT1P priority.
     * LT PORT_SYSTEM_PROFILE.ING_OPRI.
     */
    uint32_t port_pri                    : 3;
    /*!
     * Ingress outer CFI.
     * LT PORT_SYSTEM_PROFILE.ING_OCFI.
     */
    uint32_t ocfi                        : 1;
    /*!
     * Enable inband telemetry.
     * LT PORT_SYSTEM_PROFILE.INBAND_TELEMETRY_IFA.
     */
    uint32_t int_ifa_enable              : 1;
    /*!
     * Enable inband telemetry.
     * LT PORT_SYSTEM_PROFILE.INBAND_TELEMETRY_IOAM.
     */
    uint32_t int_ioam_enable             : 1;
    /*!
     * Enable inband telemetry.
     * LT PORT_SYSTEM_PROFILE.INBAND_TELEMETRY_DATAPLANE.
     */
    uint32_t int_dp_enable               : 1;
    /*!
     * Ingress enhanced flex counter action.
     * LT PORT_SYSTEM_PROFILE.CTR_ING_EFLEX_ACTION.
     */
    uint32_t flex_ctr_action             : 16;
    /*!
     * IPBM index to generate Input Port Bitmap field of the IFP..
     * LT PORT_SYSTEM_PROFILE.IPBM_INDEX.
     */
    uint32_t ipbm_index                  : 6;
    /*!
     * DEVICE_TS_PTP_MSG_CONTROL_PROFILE logical table index.
     * LT PORT_SYSTEM_PROFILE.DEVICE_TS_PTP_MSG_CONTROL_PROFILE_ID.
     */
    uint32_t ptp_msg_ctrl_profile_id     : 8;
    /*!
     * Enable NTP-TC.
     * LT PORT_SYSTEM_PROFILE.NTP_TC.
     */
    uint32_t ntp_tc_enable               : 1;
} bcmi_ltsw_port_system_prof_t;

/*!
 * \brief PROTOCOL_PKT_CONTROL profile information.
 *
 * This data structure defines the information in PROTOCOL_PKT_CONTROL profile.
 */
typedef struct bcmi_ltsw_port_profile_port_pkt_ctrl_s {
    union {
        struct {
            /*! Copy SRP packets to CPU port. */
            uint32_t srp_to_cpu          : 1;
            /*! Action for forwarding of SRP pkts. */
            uint32_t srp_fwd_action      : 2;
            /*! Send ND Packets to CPU port. */
            uint32_t nd_to_cpu           : 1;
            /*! Drop ND Packets. */
            uint32_t nd_drop             : 1;
            /*! Copy MMRP packets to CPU port. */
            uint32_t mmrp_to_cpu         : 1;
            /*! Action for forwarding of MMRP pkts. */
            uint32_t mmrp_fwd_action     : 2;
            /*! Send DHCP Packets to CPU port. */
            uint32_t dhcp_to_cpu         : 1;
            /*! Drop DHCP Packets. */
            uint32_t dhcp_drop           : 1;
            /*! Send ARP and RARP Reply Packets to CPU port. */
            uint32_t arp_request_to_cpu  : 1;
            /*! Send ARP and RARP Request Packets to CPU port. */
            uint32_t arp_request_drop    : 1;
            /*! Send ARP and RARP Reply Packets to CPU port. */
            uint32_t arp_reply_to_cpu    : 1;
            /*! Drop ARP and RARP Reply Packets. */
            uint32_t arp_reply_drop      : 1;
        } s;
        uint32_t v;
    } prot_pkt_ctrl;
    /*! IGMP and MLD pkts control. */
    union {
        struct {
            /*!
             * Enable to use Port Filtering Multicast (PFM) rules that are
             * applied to IGMP/MLD/reserved multicast packets when
             * flooded to VLAN.
             */
            uint32_t pfm_rule_apply               : 1;
            /*! Forwarding action for IGMP report/leave messages. */
            uint32_t igmp_report_leave_fwd_action : 2;
            /*! Enable to copy IGMP report/leave to the CPU. */
            uint32_t igmp_report_leave_to_cpu     : 1;
            /*! Forwarding action for IGMP query messages. */
            uint32_t igmp_query_fwd_action        : 2;
            /*! Enable to copy IGMP query to the CPU. */
            uint32_t igmp_query_to_cpu            : 1;
            /*! Forwarding action for unknown IGMP messages. */
            uint32_t igmp_unknown_msg_fwd_action  : 2;
            /*! Enable to copy unknown IGMP messages to the CPU. */
            uint32_t igmp_unknown_msg_to_cpu      : 1;
            /*! Forwarding action for IPv4 reserved multicast packets. */
            uint32_t ipv4_resvd_mc_pkt_fwd_action : 2;
            /*! Enable to copy IPv4 reserved multicast packets to the CPU. */
            uint32_t ipv4_resvd_mc_pkt_to_cpu     : 1;
            /*!
             * Forwarding action for multicast router
             * advertisement/solicitation/termination packets.
             */
            uint32_t ipv4_mc_router_adv_pkt_fwd_action : 2;
            /*!
             * Enable to copy multicast router
             * advertisement/solicitation/termination packets to the CPU.
             */
            uint32_t ipv4_mc_router_adv_pkt_to_cpu : 1;
            /*! Forwarding action for MLD report/done messages. */
            uint32_t mld_report_done_fwd_action    : 2;
            /*! Enable to copy MLD report/done to the CPU. */
            uint32_t mld_report_done_to_cpu        : 1;
            /*! Forwarding action for MLD query messages. */
            uint32_t mld_query_fwd_action          : 2;
            /*! Enable to copy MLD query to the CPU. */
            uint32_t mld_query_to_cpu              : 1;
            /*! Forwarding action for IPv6 reserved multicast packets. */
            uint32_t ipv6_resvd_mc_pkt_fwd_action  : 2;
            /*! Enable to copy IPv6 reserved multicast packets to the CPU. */
            uint32_t ipv6_resvd_mc_pkt_to_cpu      : 1;
            /*!
             * Forwarding action for multicast router
             * advertisement/solicitation/termination packets.
             */
            uint32_t ipv6_mc_router_adv_pkt_fwd_action : 2;
            /*!
             * Enable to copy multicast router
             * advertisement/solicitation/termination packets to the CPU.
             */
            uint32_t ipv6_mc_router_adv_pkt_to_cpu : 1;
        } s1;
        uint32_t v1;
    } igmp_mld_pkt_ctrl;
} bcmi_ltsw_port_profile_port_pkt_ctrl_t;


/*!
 * \brief Structure to contain all possible information of a gport.
 */
typedef struct bcmi_ltsw_gport_info_s {
    bcm_port_t      port;
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             mpls_id;
    int             mim_id;
    int             wlan_id;
    int             trill_id;
    int             l2gre_id;
    int             vxlan_id;
    int             flow_id;
    int             vlan_vp_id;
    int             niv_id;
    int             extender_id;
    int             subport_id;
    int             scheduker_id;
    uint32_t        gport_type;
} bcmi_ltsw_gport_info_t;


/*!
 * \brief Enumerator for port tab access.
 */
typedef enum bcmi_ltsw_port_tab_e {
    BCMI_PT_ZERO = 0,
    BCMI_PT_PORT_TYPE,
    BCMI_PT_ING_PORT_TYPE,
    BCMI_PT_EGR_PORT_TYPE,
    BCMI_PT_EGR_ING_PORT_TYPE,
    BCMI_PT_PORT_PARSER_TYPE,
    BCMI_PT_VLAN_ASSIGNMENT_BASED_MAC,
    BCMI_PT_VLAN_ASSIGNMENT_BASED_IPV4,
    BCMI_PT_EGR_VLAN_MEMBERSHIP_CHECK,
    BCMI_PT_ING_VLAN_MEMBERSHIP_CHECK,
    BCMI_PT_ING_EGR_VLAN_MEMBERSHIP_CHECK,
    BCMI_PT_ING_EGR_BLOCK_EN,
    BCMI_PT_TRUST_INCOMING_VID,
    BCMI_PT_PORT_GRP,
    BCMI_PT_OPRI_MAPPED,
    BCMI_PT_OCFI_MAPPED,
    BCMI_PT_IPRI_MAPPED,
    BCMI_PT_ICFI_MAPPED,
    BCMI_PT_ING_XLATE_TBL_SEL_FIRST_LOOKUP,
    BCMI_PT_ING_XLATE_TBL_SEL_SECOND_LOOKUP,
    BCMI_PT_MAC_LEARN,
    BCMI_PT_MAC_LEARN_AS_PENDING,
    BCMI_PT_MAC_COPY_TO_CPU,
    BCMI_PT_MAC_DROP,
    BCMI_PT_MAC_MOVE,
    BCMI_PT_MAC_MOVE_COPY_TO_CPU,
    BCMI_PT_MAC_MOVE_DROP,
    BCMI_PT_MPLS,
    BCMI_PT_OPERATING_MODE,
    BCMI_PT_V4IPMC,
    BCMI_PT_V4L3,
    BCMI_PT_V6IPMC,
    BCMI_PT_V6L3,
    BCMI_PT_IPMC_USE_L3_IIF,
    BCMI_PT_BRIDGE,
    BCMI_PT_FP_EGR,
    BCMI_PT_FP_ING,
    BCMI_PT_FP_VLAN,
    BCMI_PT_IFP_PBM_INDEX,
    BCMI_PT_LB_HASH_PORT_LB_NUM,
    BCMI_PT_SKIP_VLAN_CHECK,
    BCMI_PT_PASS_ON_OUTER_TPID_MATCH,
    BCMI_PT_PASS_ON_PAYLOAD_OUTER_TPID_MATCH,
    BCMI_PT_VLAN_OUTER_TPID_ID,
    BCMI_PT_PASS_ON_EGR_OUTER_TPID_MATCH,
    BCMI_PT_ING_VLAN_OUTER_TPID_ID,
    BCMI_PT_PORT_COS_QUEUE_MAP_ID,
    BCMI_PT_TRUST_PHB_EGR_DSCP_MAP,
    BCMI_PT_TRUST_PHB_EGR_PORT_L2_OTAG,
    BCMI_PT_ING_OTPID_ENABLE,
    BCMI_PT_ING_PAYLOAD_OTPID_ENABLE,
    BCMI_PT_ING_L2_IIF,
    BCMI_PT_ING_SYS_PORT,
    BCMI_PT_ING_PP_PORT,
    BCMI_PT_ING_SYS_DEST_L2_OIF,
    BCMI_PT_ING_SYS_DEST_L2_OIF_STR,
    BCMI_PT_SYS_PORT_STR_PROFILE_ID,
    BCMI_PT_SYS_DEST_STR_PROFILE_ID,
    BCMI_PT_L2MC_L3MC_L2_OIF_SYS_DST_VALID_STR,
    BCMI_PT_ING_SYS_DEST_NHOP_2,
    BCMI_PT_EGR_MC_L2_OIF,
    BCMI_PT_EGR_OTPID,
    BCMI_PT_EGR_ITPID,
    BCMI_PT_L2_OIF_STR_PROFILE_ID,
    BCMI_PT_DEFAULT_VFI,
    BCMI_PT_DEFAULT_PRI,
    BCMI_PT_DEFAULT_INNER_PRI,
    BCMI_PT_CML_NEW,
    BCMI_PT_CML_MOVE,
    BCMI_PT_VLAN_MAP_TO_VFI,
    BCMI_PT_VLAN_PRESERVE,
    BCMI_PT_ING_PORT_GROUP,
    BCMI_PT_ING_CLASS_ID,
    BCMI_PT_ING_PVLAN_EN,
    BCMI_PT_ING_VT_EN,
    BCMI_PT_IPV4_TUNNEL_EN,
    BCMI_PT_IPV6_TUNNEL_EN,
    BCMI_PT_OVLAN_MAP_TO_PHB,
    BCMI_PT_IVLAN_MAP_TO_PHB,
    BCMI_PT_DSCP_MAP_TO_PHB,
    BCMI_PT_VLAN_PHB,
    BCMI_PT_DSCP_PHB,
    BCMI_PT_DOT1P_PHB,
    BCMI_PT_PHB_VLAN,
    BCMI_PT_PHB_DSCP,
    BCMI_PT_PHB_VLAN_STR,
    BCMI_PT_QOS_REMARK_CTRL,
    BCMI_PT_EGR_PORT_GROUP,
    BCMI_PT_EGR_L2_PROCESS_CTRL,
    BCMI_PT_L2_MTU,
    BCMI_PT_L2_DEST,
    BCMI_PT_L2_DEST_TYPE,
    BCMI_PT_L2_OIF_IS_TRUNK,
    BCMI_PT_L2_OIF_SYS_PORT,
    BCMI_PT_DEST_SYS_PORT_IS_TRUNK,
    BCMI_PT_DEST_SYS_PORT_LPORT,
    BCMI_PT_DEST_SYS_PORT_TRUNK,
    BCMI_PT_DEST_SYS_PORT_DLB_ID_VALID,
    BCMI_PT_DEST_SYS_PORT_DLB_ID,
    BCMI_PT_ING_L3_IIF,
    BCMI_PT_HONOR_SRC_DISCARD_ON_STN_MOVE,
    BCMI_PT_STATIC_MOVE_DROP,
    BCMI_PT_VXLAN_EN,
    BCMI_PT_VXLAN_KEY_MODE,
    BCMI_PT_VXLAN_USE_PKT_OVID,
    BCMI_PT_VXLAN_VFI_KEY_MODE,
    BCMI_PT_VXLAN_VFI_USE_PKT_OVID,
    BCMI_PT_VXLAN_SVP_KEY_MODE,
    BCMI_PT_VXLAN_SVP_DEFAULT_NETWORK,
    BCMI_PT_VN_ID_LOOKUP_CTRL,
    BCMI_PT_SHAPER_MODE,
    BCMI_PT_SHAPER_RATE,
    BCMI_PT_SHAPER_RATE_REAL,
    BCMI_PT_SHAPER_BURST,
    BCMI_PT_SHAPER_BURST_REAL,
    BCMI_PT_SHAPER_BURST_AUTO,
    BCMI_PT_SHAPER_IFG_ADJ,
    BCMI_PT_SHAPER_IFG_ADJ_EN,
    BCMI_PT_MIRROR_ID,
    BCMI_PT_MIRROR_ID_0 = BCMI_PT_MIRROR_ID,
    BCMI_PT_MIRROR_ID_1 = BCMI_PT_MIRROR_ID + 1,
    BCMI_PT_MIRROR_ID_2 = BCMI_PT_MIRROR_ID + 2,
    BCMI_PT_MIRROR_ID_3 = BCMI_PT_MIRROR_ID + 3,
    BCMI_PT_MIRROR_CPU,
    BCMI_PT_RANGE_MAP_INDEX,
    BCMI_PT_SAMPLE_ID,
    BCMI_PT_ING_RXTS_INSERT,
    BCMI_PT_ING_TXTS_INSERT,
    BCMI_PT_EGR_TXTS_INSERT,
    BCMI_PT_ING_TS_CANCEL,
    BCMI_PT_ING_TXTS_CANCEL,
    BCMI_PT_EGR_TS_CANCEL,
    BCMI_PT_ING_TS_OID,
    BCMI_PT_EGR_TS_OID,
    BCMI_PT_EGR_TS_ADJUST,
    BCMI_PT_EGR_L2_OIF_VIEW,
    BCMI_PT_ING_FLEX_CTR_ID,
    BCMI_PT_EGR_FLEX_CTR_ID,
    BCMI_PT_ING_FLEX_STATE_ID,
    BCMI_PT_EGR_MASK_ID,
    BCMI_PT_EGR_SFLOW_EN,
    BCMI_PT_EGR_SFLOW_RATE,
    BCMI_PT_MTU_EN,
    BCMI_PT_MTU,
    BCMI_PT_ING_OPAQUE_CTRL_ID,
    BCMI_PT_EGR_OPAQUE_CTRL_ID,
    BCMI_PT_NHOP_2_OR_ECMP_GROUP_INDEX_1,
    BCMI_PT_ECMP_AND_PROT_SWT_SFC_PROFILE_INDEX,
    BCMI_PT_SC_METER_INDEX,
    BCMI_PT_SC_BC_EN,
    BCMI_PT_SC_MC_EN,
    BCMI_PT_SC_UMC_EN,
    BCMI_PT_SC_UUC_EN,
    BCMI_PT_ING_HG3_ETYPE,
    BCMI_PT_EGR_HG3_ETYPE,
    BCMI_PT_LOOPBACK_EN,
    BCMI_PT_CPU_EN,

    BCMI_PT_PASS_CTRL_FRAMES,
    BCMI_PT_IEEE_802_1AS_ENABLE,
    BCMI_PT_IFA_ENABLE,
    BCMI_PT_IOAM_ENABLE,
    BCMI_PT_INT_DP_ENABLE,
    BCMI_PT_EGR_CFI_AS_CNG,
    BCMI_PT_ING_CFI_AS_CNG,
    BCMI_PT_DROP_BPDU,

    BCMI_PT_VLAN_ING_TAG_ACTION_PROFILE_ID,
    BCMI_PT_ING_OPRI,
    BCMI_PT_ING_OVID,
    BCMI_PT_ING_OCFI,

    BCMI_PT_VLAN_EGR_TAG_ACTION_PROFILE_ID,
    BCMI_PT_EGR_OPRI,
    BCMI_PT_EGR_OVID,
    BCMI_PT_EGR_OCFI,

    BCMI_PT_ING_PVLAN,
    BCMI_PT_EGR_PVLAN,
    BCMI_PT_PVLAN_EGR_UNTAG,
    BCMI_PT_PVLAN_EGR_REPLACE_PRI,
    BCMI_PT_PVLAN_EGR_PRI,
    BCMI_PT_PVLAN_EGR_VID,

    BCMI_PT_TRUST_PHB_ING_DSCP_V4,
    BCMI_PT_TRUST_PHB_ING_DSCP_V6,

    BCMI_PT_DROP_UNTAG,
    BCMI_PT_DROP_TAG,

    BCMI_PT_STATIC_MOVE_DROP_DISABLE,

    BCMI_PT_EGR_CLASS_ID_SEL,
    BCMI_PT_ING_GRP_SEL_CLASS_ID,
    BCMI_PT_IPBM_INDEX,
    BCMI_PT_OFFSET_ECMP_RANDOM,
    BCMI_PT_OFFSET_ECMP_LVL2_RANDOM,
    BCMI_PT_OFFSET_TRUNK_RANDOM,
    BCMI_PT_PORT_PKT_CONTROL_ID,

    BCMI_PT_MIRROR_ING_ENCAP_SAMPLE,
    BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_RATE,
    BCMI_PT_MIRROR_EGR_ENCAP_SAMPLE,
    BCMI_PT_MIRROR_EGR_ENCAP_SAMPLE_RATE,
    BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_CPU,
    BCMI_PT_MIRROR_ING_ENCAP_SAMPLE_INSTANCE,

    BCMI_PT_MIRROR_ON_DROP,
    BCMI_PT_TM_MIRROR_ON_DROP_PROFILE_ID,

    BCMI_PT_PORT_SYSTEM_IPBM_INDEX,
    BCMI_PT_PORT_SYSTEM_OPERATING_MODE,
    BCMI_PT_PORT_SYSTEM_L3_IIF_ID,
    BCMI_PT_PORT_SYSTEM_VRF_ID,
    BCMI_PT_PORT_SYSTEM_L2_FWD_IPMCV4,
    BCMI_PT_PORT_SYSTEM_L2_FWD_IPMCV6,
    BCMI_PT_PORT_SYSTEM_PROFILE_ID,
    BCMI_PT_PORT_SYSTEM_FP_VLAN_PORT_GRP,
    BCMI_PT_PORT_SYSTEM_CLASS_ID,
    BCMI_PT_PORT_SYSTEM_BLOCKED_EGR_PORTS,

    BCMI_PT_OUTER_TPID_VERIFY,
    BCMI_PT_USE_TABLE_VLAN_OUTER_TPID_ID,

    BCMI_PT_ECN_IP_TO_MPLS_EXP_PRI,
    BCMI_PT_ECN_IP_TO_MPLS_EXP_ID,
    BCMI_PT_ECN_CNG_TO_MPLS_EXP_PRI,
    BCMI_PT_ECN_CNG_TO_MPLS_EXP_ID,
    BCMI_PT_ECN_LATENCY_PROFILE_ID,

    BCMI_PT_1588_LINK_DELAY,
    BCMI_PT_EGR_OPAQUE_OBJ0,
    BCMI_PT_ING_OPAQUE_CTRL_ID_1,
    BCMI_PT_ING_OPAQUE_CTRL_ID_2,
    BCMI_PT_ING_OPAQUE_CTRL_ID_3,

    BCMI_PT_VFI_EGR_ADAPT_PORT_GRP_LOOKUP,
    BCMI_PT_VFI_EGR_ADAPT_PORT_GRP_LOOKUP_MISS_DROP,
    BCMI_PT_VXLAN_VFI_EGR_ADAPT_PORT_GRP_LOOKUP,
    BCMI_PT_VXLAN_VFI_EGR_ADAPT_PORT_GRP_LOOKUP_MISS_DROP,
    BCMI_PT_VFI_EGR_ADAPT_PORT_GRP,
    BCMI_PT_VFI_ING_ADAPT_PORT_GRP,
    BCMI_PT_VFI_ING_ADAPT_FIRST_LOOKUP,
    BCMI_PT_VFI_ING_ADAPT_FIRST_LOOKUP_PORT_GRP_MODE,
    BCMI_PT_VFI_ING_ADAPT_SECOND_LOOKUP,
    BCMI_PT_VFI_ING_ADAPT_SECOND_LOOKUP_PORT_GRP_MODE,

    BCMI_PT_DEVICE_TS_PTP_MSG_CONTROL_PROFILE_ID,
    BCMI_PT_NTP_TC,

    BCMI_PT_GIH_CPU_PORT_TYPE,
    BCMI_PT_GIH_CPU_PARSER_TYPE,
    BCMI_PT_GIH_CPU_SYS_PORT,

    BCMI_PT_EGR_OPAQUE_TAG,

    BCMI_PT_CNT,
} bcmi_ltsw_port_tab_t;

/*!
 * \brief Enumerator for BCMI_PT_PORT_TYPE.
 */
typedef enum bcmi_ltsw_port_type_e {
    PORT_TYPE_ETHERNET = 0,
    PORT_TYPE_SYSTEM,
    PORT_TYPE_LOOPBACK,
    PORT_TYPE_CPU,
    PORT_TYPE_CNT,
} bcmi_ltsw_port_type_t;

/*!
 * \brief Enumerator for BCMI_XGS_PT_PORT_TYPE.
 */
typedef enum bcmi_xgs_ltsw_port_type_e {
    XGS_PORT_TYPE_ETHERNET = 0,
    XGS_PORT_TYPE_LOOPBACK = 2,
    XGS_PORT_TYPE_CNT,
} bcmi_xgs_ltsw_port_type_t;

/*!
 * \brief Enumerator for BCMI_PT_ING_XLATE_TBL_SEL_FIRST_LOOKUP.
 */
typedef enum bcmi_ltsw_ing_xlate_tbl_sel_e {
    VLAN_ING_XLATE_IVID_OVID = 0,
    VLAN_ING_XLATE_IVID_OVID_MODPORT,
    VLAN_ING_XLATE_IVID_OVID_TRUNK,
    VLAN_ING_XLATE_IVID_OVID_PORT_GRP,
    VLAN_ING_XLATE_OTAG,
    VLAN_ING_XLATE_OTAG_MODPORT,
    VLAN_ING_XLATE_OTAG_TRUNK,
    VLAN_ING_XLATE_OTAG_PORT_GRP,
    VLAN_ING_XLATE_ITAG,
    VLAN_ING_XLATE_ITAG_MODPORT,
    VLAN_ING_XLATE_ITAG_TRUNK,
    VLAN_ING_XLATE_ITAG_PORT_GRP,
    VLAN_ING_XLATE_OVID,
    VLAN_ING_XLATE_OVID_MODPORT,
    VLAN_ING_XLATE_OVID_TRUNK,
    VLAN_ING_XLATE_OVID_PORT_GRP,
    VLAN_ING_XLATE_IVID,
    VLAN_ING_XLATE_IVID_MODPORT,
    VLAN_ING_XLATE_IVID_TRUNK,
    VLAN_ING_XLATE_IVID_PORT_GRP,
    VLAN_ING_XLATE_CFI_PRI,
    VLAN_ING_XLATE_CFI_PRI_MODPORT,
    VLAN_ING_XLATE_CFI_PRI_TRUNK,
    VLAN_ING_XLATE_CFI_PRI_PORT_GRP,
    VLAN_ING_XLATE_CNT,
} bcmi_ltsw_ing_xlate_tbl_sel_t;

/*!
 * \brief Enumerator for BCMI_PT_OPERATING_MODE.
 */
typedef enum bcmi_ltsw_port_op_mode_e {
    OP_MODE_NORMAL = 0,
    OP_MODE_L3_IIF,
    OP_MODE_VRF,
    OP_MODE_VLAN,
    OP_MODE_CNT,
} bcmi_ltsw_port_op_mode_t;

/*!
 * \brief Enumerator for BCMI_PT_SHAPER_MODE.
 */
typedef enum bcmi_ltsw_port_shaper_mode_e {
    SHAPER_MODE_BYTE = 0,
    SHAPER_MODE_PKT,
    SHAPER_MODE_CNT,
} bcmi_ltsw_port_shaper_mode_t;

/*!
 * \brief Enumerator for BCMI_PT_PORT_PARSER_TYPE.
 */
typedef enum bcmi_ltsw_port_parser_type_e {
    PARSER_TYPE_NONE = 0,
    PARSER_TYPE_ETHERNET,
    PARSER_TYPE_SYSTEM,
    PARSER_TYPE_LOOPBACK,
    PARSER_TYPE_CNT,
} bcmi_ltsw_port_parser_type_t;

/*!
 * \brief Enumerator for BCMI_PT_PORT_DEST_TYPE.
 */
typedef enum bcmi_ltsw_port_dest_type_e {
    DEST_TYPE_NO_OP = 0,
    DEST_TYPE_L2_OIF,
    DEST_TYPE_RESERVED,
    DEST_TYPE_VP,
    DEST_TYPE_ECMP,
    DEST_TYPE_NHOP,
    DEST_TYPE_L2MC_GROUP,
    DEST_TYPE_L3MC_GROUP,
    DEST_TYPE_RESERVED_1,
    DEST_TYPE_ECMP_MEMBER,
    DEST_TYPE_CNT,
} bcmi_ltsw_port_dest_type_t;

/*!
 * \brief Enumerator for BCMI_PT_PORT_DEST_TYPE.
 */
typedef enum bcmi_ltsw_l2_oif_view_e {
    L2_OIF_VIEW_ITPID = 0,
    L2_OIF_VIEW_FLEX_CNTR,
    L2_OIF_VIEW_CNT,
} bcmi_ltsw_l2_oif_view_t;

/*!
 * \brief Enumerator for FP_CLASS_ID_SEL.
 */
typedef enum bcmi_ltsw_fp_class_id_sel_e {
    FP_CLASS_ID_SEL_INVALID = 0,
    FP_CLASS_ID_SEL_PORT,
    FP_CLASS_ID_SEL_ALPM_COMPRESSION_SRC,
    FP_CLASS_ID_SEL_L3_IIF,
    FP_CLASS_ID_SEL_VFP_HI,
    FP_CLASS_ID_SEL_VFP_LO,
    FP_CLASS_ID_SEL_L2_SRC,
    FP_CLASS_ID_SEL_L2_DST,
    FP_CLASS_ID_SEL_L3_SRC,
    FP_CLASS_ID_SEL_L3_DST,
    FP_CLASS_ID_SEL_VLAN,
    FP_CLASS_ID_SEL_VRF,
    FP_CLASS_ID_SEL_ALPM_COMPRESSION_DST,
    FP_CLASS_ID_SEL_EM = 13,
    FP_CLASS_ID_SEL_IFP = 15,
    FP_CLASS_ID_SEL_CNT,
} bcmi_ltsw_fp_class_id_sel_t;

#define ECN_ENABLE          8
#define TX_BYTE_ENABLE      4

typedef enum bcmi_ltsw_port_resource_type_e {
    BCMI_PORT_GROUP_ING,
    BCMI_PORT_GROUP_EGR,
    BCMI_PORT_GROUP_L2_IF,
    BCMI_PORT_GROUP_DVP,
    BCMI_PORT_RESOURCE_CNT,
} bcmi_ltsw_port_resource_type_t;

typedef struct bcmi_ltsw_port_resource_spec_e {
    int min;
    int max;
} bcmi_ltsw_port_resource_spec_t;

/*!
 * \brief Callback definition for port traverse.
 */
typedef int (*bcmi_ltsw_port_traverse_cb)(int unit, int port, void *user_data);


/* Port validation interfaces */
/*!
 * \brief Identifies if given modid is local on a given unit.
 *
 * \param [in] unit Unit number.
 * \param [in] modid Module ID.
 * \param [out] result TRUE if modid is local, FALSE otherwise.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_modid_is_local(int unit, bcm_module_t modid, int *result);

/*!
 * \brief Get the addressable local port from the given Gport.
 *
 * Gport must be types that deal with logical BCM ports (MODPORT, LOCAL, ...).
 * Logical port does not need to be a valid port, only addressable.
 *
 * \param [in] unit Unit number.
 * \param [in] gport Global port identifier.
 * \param [out] local_port Local port encoded in gport.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_addressable_local_get(int unit,
                                     bcm_gport_t gport, bcm_port_t *local_port);

/*!
 * \brief Helper funtion to construct a gport from given information.
 *
 * The modid and port are translated from the local space to application space.
 *
 * \param [in] unit Unit number.
 * \param [in] gport_info Structure that contains information to encode gport.
 * \param [out] gport Global port identifier.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_gport_construct(int unit, bcmi_ltsw_gport_info_t *gport_info,
                               bcm_gport_t *gport);

/*!
 * \brief Helper funtion to validate port/gport parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] port_in Port / Gport to validate.
 * \param [out] port_out Port number if valid.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_gport_validate(int unit, bcm_port_t port_in, bcm_port_t *port_out);

/*!
 * \brief Helper funtion to validate PHY specific gport.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port / Gport to validate.
 * \param [out] local_port Local Port number.
 * \param [out] phyn Local Phy number.
 * \param [out] phy_lane Lane number.
 * \param [out] sys_side System side(1) or line side(0).
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_gport_phyn_validate(int unit, bcm_port_t port,
                                   bcm_port_t *local_port, int *phyn,
                                   int *phy_lane, int *sys_side);

/*!
 * \brief Helper funtion to get modid, port, and trunk_id from a bcm_gport_t.
 *
 * \param [in] unit Unit number.
 * \param [in] gport Global port identifier.
 * \param [out] modid Module ID.
 * \param [out] port Port number.
 * \param [out] trunk_id Trunk ID.
 * \param [out] id HW ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_gport_resolve(int unit, bcm_gport_t gport,
                             bcm_module_t *modid, bcm_port_t *port,
                             bcm_trunk_t *trunk_id, int *id);

/*!
 * \brief Traverse over all local ports and gports.
 *
 * \param [in] unit Unit number.
 * \param [in] gport_type Gport type to traverse (BCM_GPORT_TYPE_XXX).
 * \param [in] cb Callback function.
 * \param [in] user_data User data to a call back.
 *
 * \return Number of matched ports.
 */
extern int
bcmi_ltsw_port_gport_traverse(int unit, uint32_t gport_type,
                              bcmi_ltsw_port_traverse_cb cb, void *user_data);

/*!
 * \brief Traverse over all local ports.
 *
 * \param [in] unit Unit number.
 * \param [in] cb Callback function.
 * \param [in] user_data User data to a call back.
 *
 * \return Number of matched ports.
 */
extern int
bcmi_ltsw_port_traverse(int unit, bcmi_ltsw_port_traverse_cb cb,
                        void *user_data);

/*!
 * \brief Traverse over all local ports and gports.
 *
 * \param [in] unit Unit number.
 * \param [in] tbmp Bmp of gport types to traverse (BCM_GPORT_TYPE_NONE_XXX).
 * \param [in] cb Callback function.
 * \param [in] user_data User data to a call back.
 *
 * \return Number of matched ports.
 */
extern int
bcmi_ltsw_sys_port_traverse(int unit, bcmi_ltsw_port_traverse_cb cb,
                            void *user_data);
/*!
 * \brief Count ports which match identified port type(s).
 *
 * \param [in] unit Unit number.
 * \param [in] port_type Port type bitmap (BCMI_LTSW_PORT_TYPE_xxx).
 *
 * \return Number of matched ports.
 */
extern int
bcmi_ltsw_port_number_count(int unit, uint32_t port_type);

/*!
 * \brief Find out ports which match identified port type(s).
 *
 * \param [in] unit Unit number.
 * \param [in] port_type Port type bitmap (BCMI_LTSW_PORT_TYPE_xxx).
 * \param [out] pbmp Port bitmap of matched ports.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_bitmap(int unit, uint32_t port_type, bcm_pbmp_t *pbmp);

/*!
 * \brief Check if port is identified type(s).
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] port_type Port type bitmap (BCMI_LTSW_PORT_TYPE_xxx).
 *
 * \return TRUE if port is identified type(s).
 */
extern int
bcmi_ltsw_port_is_type(int unit, bcm_port_t port, uint32_t port_type);

/*!
 * \brief Helper functiom to return CPU port.
 *
 * \param [in] unit Unit number.
 *
 * \return CPU port.
 */
extern int
bcmi_ltsw_port_cpu(int unit);

/*!
 * \brief Get pipe id of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] pipe Pipe of a port.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_pipe_get(int unit, int port, int *pipe);

/*!
 * \brief Get physical port of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return Physical port of the port.
 */
extern int
bcmi_ltsw_port_to_pport(int unit, int port);

/*!
 * \brief Get port of a physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return Port of the physical port.
 */
int
bcmi_ltsw_pport_to_port(int unit, int pport);

/*!
 * \brief Get port name.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return Port name.
 */
extern char *
bcmi_ltsw_port_name(int unit, bcm_port_t port);

/*!
 * \brief Allocate an ID from L2_IF pool.
 *
 * \param [in] unit Unit number.
 * \param [out] l2_if Allocated ID.
 *
 * \retval SHR_E_EMPTY No available ID to allocacte.
 */
extern int
bcmi_ltsw_l2_if_alloc(int unit, int *l2_if);

/*!
 * \brief Free an ID to L2_IF pool.
 *
 * \param [in] unit Unit number.
 * \param [in] l2_if ID to free.
 *
 * \retval SHR_E_FULL The ID is already free.
 */
extern int
bcmi_ltsw_l2_if_free(int unit, int l2_if);

/*!
 * \brief Dump L2_IF pool.
 *
 * \param [in] unit Unit Number.
 */
extern void
bcmi_ltsw_l2_if_dump_sw(int unit);

/*!
 * \brief Add specified entry to L2_IF based tables.
 *
 * \param [in] unit Unit Number.
 * \param [in] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l2_if_add(int unit, int l2_if);

/*!
 * \brief Delete specified entry of L2_IF based tables.
 *
 * \param [in] unit Unit Number.
 * \param [in] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l2_if_del(int unit, int l2_if);

/*!
 * \brief Get gport from L2 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] l2_if L2 interface.
 * \param [out] port Gport.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l2_if_to_port(int unit, int l2_if, int *port);

/*!
 * \brief Get L2 interface from port or gport.
 *
 * \param [in] unit Unit number.
 * \param [in] port Local logical port or gport.
 * \param [out] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_port_to_l2_if(int unit, int port, int *l2_if);

/*!
 * \brief Get port mask from L2 interface mask.
 *
 * \param [in] unit Unit number.
 * \param [in] l2_if_mask L2 interface mask.
 * \param [out] port_mask Local logical port mask.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l2_if_mask_to_port_mask(int unit, int l2_if_mask, int *port_mask);

/*!
 * \brief Get L2 interface mask from port mask.
 *
 * \param [in] unit Unit number.
 * \param [in] port_mask Local logical port mask.
 * \param [out] l2_if_mask L2 interface mask.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_port_mask_to_l2_if_mask(int unit, int port_mask, int *l2_if_mask);

/*!
 * \brief Add specified entry to System Port based tables.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_port System port.
 * \param [in] flags Operation flags BCMI_LTSW_PORT_XXX.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_sys_port_add(int unit, int sys_port, int flags);

/*!
 * \brief Delete specified entry of System Port based tables.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_port System port.
 * \param [in] flags Operation flags BCMI_LTSW_PORT_XXX.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_sys_port_del(int unit, int sys_port, int flags);

/*!
 * \brief Change the system port of a port.
 * The settings of the old system port is moved to the new system port.
 * The old system port is deleted then.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] sys_port New system port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_port_sys_port_move(int unit, bcm_port_t port, int sys_port);

/*!
 * \brief Get gport from system port.
 *
 * \param [in] unit Unit number.
 * \param [in] sys_port System port.
 * \param [out] port Gport.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_sys_port_to_port(int unit, int sys_port, bcm_port_t *port);

/*!
 * \brief Get system port from port or gport.
 *
 * \param [in] unit Unit number.
 * \param [in] port Local logical port or gport.
 * \param [out] sys_port System port.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_port_to_sys_port(int unit, bcm_port_t port, int *sys_port);

/*!
 * \brief Get system ports from port or gport.
 *
 * If gport is a trunk, get system ports from its member ports.
 *
 * \param [in] unit Unit number.
 * \param [in] port Local logical port or gport.
 * \param [in] array_size Size of system port array.
 * \param [out] sys_port_array System port array.
 * \param [out] count Real count of system ports.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_port_to_sys_ports(int unit, bcm_port_t port,
                            int array_size, int *sys_port_array, int *count);

/*!
 * \brief Map System Port with L2 interface.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_port System port.
 * \param [in] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_sys_port_map_l2_if(int unit, int sys_port, int l2_if);

/*!
 * \brief Demap System Port and L2 interface.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_port System port.
 * \param [in] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_sys_port_demap_l2_if(int unit, int sys_port, int l2_if);

/* Port table interfaces */
/*!
 * \brief Set single PORT/LPORT field.
 *
 * Helper funtion for port init and port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [in] data Value to be set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_tab_set(int unit, bcm_port_t port,
                       bcmi_ltsw_port_tab_t type, int data);

/*!
 * \brief Get single PORT/LPORT field.
 *
 * Helper funtion for port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [out] data The gotten value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_tab_get(int unit, bcm_port_t port,
                       bcmi_ltsw_port_tab_t type, int *data);

/*!
 * \brief Set multiple PORT/LPORT field.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerators of port tab types.
 * \param [in] data Values to be set.
 * \param [in] cnt Number of port tab types.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_tab_multi_set(int unit, bcm_port_t port,
                             bcmi_ltsw_port_tab_t *type, int *data,
                             int cnt);

/*!
 * \brief Get multiple PORT/LPORT field.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerators of port tab types.
 * \param [out] data The gotten values of each port tab types.
 * \param [in] cnt Number of port tab types.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_tab_multi_get(int unit, bcm_port_t port,
                             bcmi_ltsw_port_tab_t *type, int *data,
                             int cnt);

/*!
 * \brief Set single PORT/LPORT array field.
 *
 * Helper funtion for port init and port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [in] data Array to be set.
 * \param [in] size Array size.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_tab_array_set(int unit, bcm_port_t port,
                             bcmi_ltsw_port_tab_t type,
                             uint64_t *data, int size);

/*!
 * \brief Get single PORT/LPORT array field.
 *
 * Helper funtion for port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [out] data The gotten array.
 * \param [in] size The number of elements to get.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_tab_array_get(int unit, bcm_port_t port,
                             bcmi_ltsw_port_tab_t type,
                             uint64_t *data, int size);

/*!
 * \brief Set the identified bit of PORT/LPORT field.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [in] bit Bit position.
 * \param [in] data Value to be set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_tab_bit_set(int unit, bcm_port_t port,
                           bcmi_ltsw_port_tab_t type, int bit, int data);

/*!
 * \brief Get the identified bit of PORT/LPORT field.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [in] bit Bit position.
 * \param [out] data The gotten value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_tab_bit_get(int unit, bcm_port_t port,
                           bcmi_ltsw_port_tab_t type, int bit, int *data);

/*!
 * \brief Set single PORT/LPORT field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] index Table index
 * \param [in] type Enumerator of port tab type.
 * \param [in] data Value to be set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_set(int unit, int index,
                           bcmi_ltsw_port_tab_t type, int data);

/*!
 * \brief Get single PORT/LPORT field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enumerator of port tab type.
 * \param [out] data The gotten value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_get(int unit, int index,
                           bcmi_ltsw_port_tab_t type, int *data);

/*!
 * \brief Set multiple PORT/LPORT field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] index Table index
 * \param [in] type Enumerators of port tab types.
 * \param [in] data Values to be set.
 * \param [in] cnt Number of port tab types.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_multi_set(int unit, int index,
                                 bcmi_ltsw_port_tab_t *type, int *data,
                                 int cnt);

/*!
 * \brief Get multiple PORT/LPORT field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] index Table index.
 * \param [in] type Enumerators of port tab types.
 * \param [out] data The gotten values of each port tab types.
 * \param [in] cnt Number of port tab types.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_multi_get(int unit, int index,
                                 bcmi_ltsw_port_tab_t *type, int *data,
                                 int cnt);

/*!
 * \brief Set single PORT/LPORT array field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] index Table index
 * \param [in] type Enumerator of port tab type.
 * \param [in] data Array to be set.
 * \param [in] size Array size.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_array_set(int unit, int index,
                                 bcmi_ltsw_port_tab_t type,
                                 uint64_t *data, int size);

/*!
 * \brief Get single PORT/LPORT array field.
 *
 * Must identify port table index instead port id.
 *
 * \param [in] unit Unit number.
 * \param [in] index Table index
 * \param [in] type Enumerator of port tab type.
 * \param [out] data The gotten array.
 * \param [in] size The number of elements to get.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_tab_raw_array_get(int unit, int index,
                                 bcmi_ltsw_port_tab_t type,
                                 uint64_t *data, int size);

/*!
 * \brief Add a LPORT_TAB profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] profile Profile to be added.
 * \param [out] index Index of the added or existed profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_port_profile_lport_tab_add(int unit,
                                     bcmi_ltsw_port_system_prof_t *profile,
                                     int *index);
/*!
 * \brief Get LPORT_TAB profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [out] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_profile_lport_tab_get(int unit, int index,
                                     bcmi_ltsw_port_system_prof_t *profile);
/*!
 * \brief Delete a LPORT_TAB profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of the entry to be deleted.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_profile_lport_tab_delete(int unit, int index);

/*!
 * \brief Recover hash and reference count for LPORT_TAB profile.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile index to be recovered.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_profile_lport_tab_recover(int unit, int index);

/*!
 * \brief Add a PORT_PKT_CONTROL profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] profile Profile to be added.
 * \param [out] index Index of the added or existed profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_profile_port_pkt_ctrl_add(int unit,
                                         bcmi_ltsw_port_profile_port_pkt_ctrl_t *profile,
                                         int *index);

/*!
 * \brief Get PORT_PKT_CONTROL profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [out] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_profile_port_pkt_ctrl_get(int unit, int index,
                                         bcmi_ltsw_port_profile_port_pkt_ctrl_t *profile);

/*!
 * \brief Delete a PORT_PKT_CONTROL profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of the entry to be deleted.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_profile_port_pkt_ctrl_delete(int unit, int index);

/*!
 * \brief Recover hash and reference count for PORT_PKT_CONTROL profile.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile index to be recovered.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_profile_port_pkt_ctrl_recover(int unit, int index);

/*!
 * \brief Update one field of PORT_PKT_CONTROL.
 *
 * \param [in] unit Unit Number.
 * \param [in] port port ID.
 * \param [in] lt_fld_name Field name of PORT_PKT_CONTROL.
 * \param [in] lt_fld_value Field value of PORT_PKT_CONTROL.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_port_profile_port_pkt_ctrl_update(int unit, int port,
                                            const char *field_name,
                                            int field_value);

/*!
 * \brief Get one field value of PORT_PKT_CONTROL.
 *
 * \param [in] unit Unit Number.
 * \param [in] port port ID.
 * \param [in] lt_fld_name Field name of PORT_PKT_CONTROL.
 * \param [out] lt_fld_value Field value of PORT_PKT_CONTROL.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_port_profile_port_pkt_ctrl_field_get(int unit, int port,
                                            const char *field_name,
                                            int *field_value);
/*!
 * \brief Set port default vlan tag actions.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] action Vlan tag actions.
 * \param [in] profile_index profile index of vlan tag action.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_port_ing_vlan_action_set(int unit, bcm_port_t port,
                                   bcm_vlan_action_set_t *action,
                                   int profile_index);

/*!
 * \brief Get port default vlan tag actions.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] action Vlan tag actions.
 * \param [out] profile_index profile index of vlan tag action.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_port_ing_vlan_action_get(int unit, bcm_port_t port,
                                   bcm_vlan_action_set_t *action,
                                   int *profile_index);

/*!
 * \brief Set port default vlan tag actions to reset values.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_port_ing_vlan_action_reset(int unit, bcm_port_t port);

/*!
 * \brief Set port EGR vlan tag actions.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] action Vlan tag actions.
 * \param [in] profile_index profile index of vlan tag action.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_port_egr_vlan_action_set(int unit, bcm_port_t port,
                                   bcm_vlan_action_set_t *action,
                                   int profile_index);

/*!
 * \brief Get port EGR vlan tag actions.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] action Vlan tag actions.
 * \param [out] profile_index profile index of vlan tag action.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_port_egr_vlan_action_get(int unit, bcm_port_t port,
                                   bcm_vlan_action_set_t *action,
                                   int *profile_index);

/*!
 * \brief Set egress vlan translation enable.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] enable Egress vlan translation enable.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_port_egr_vlan_xlate_enable_set(int unit, bcm_port_t port, int enable);

/*!
 * \brief Get egress vlan translation enable.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] enable Egress vlan translation enable.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
bcmi_ltsw_port_egr_vlan_xlate_enable_get(int unit, bcm_port_t port, int *enable);

/*
 * \brief Configure ASF mode on the given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in]  mode ASF enable or ASF mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_asf_mode_set(int unit, bcm_port_t port, int mode);

/*
 * \brief Returns the ASF mode configured on the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in]  mode ASF enable or ASF mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_asf_mode_get(int unit, bcm_port_t port, int *mode);

/*!
 * \brief Set HG3 Ethernet Type.
 *
 * \param [in] unit Unit Number.
 * \param [in] type HG3 Ethernet Type.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_hg3_eth_type_set(int unit, int type);

/*!
 * \brief Get HG3 Ethernet Type.
 *
 * \param [in] unit Unit Number.
 * \param [in] type HG3 Ethernet Type.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_hg3_eth_type_get(int unit, int *type);

/*!
 * \brief Get which pipes are using identified port group.
 *
 * \param [in] unit Unit Number.
 * \param [in] port_group Port group.
 * \param [in] flags Operation flags BCMI_LTSW_PORT_XXX.
 * \param [out] pipe_bmp Pipe bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_port_group_to_pipes(int unit, int port_group, int flags,
                              uint32_t *pipe_bmp);

/*!
 * \brief Get the spec of port module resources.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Resource type.
 * \param [out] spec Spec of identified resource.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_port_resource_spec_get(int unit, bcmi_ltsw_port_resource_type_t type,
                                 const bcmi_ltsw_port_resource_spec_t **spec);

/*!
 * \brief Enable egress VLAN membership check for IFP redirection.
 *
 * \param [in] unit Unit Number.
 * \param [in] enable Enable or disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_port_ifp_egr_vlan_check_enable_set(int unit, uint32_t enable);

/*!
 * \brief Get egress VLAN membership check enable for IFP redirection.
 *
 * \param [in] unit Unit Number.
 * \param [out] enable Enable or disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_port_ifp_egr_vlan_check_enable_get(int unit, uint32_t *enable);

/*!
 * \brief Get flags to disable VLAN membership check.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] flags Flags to disable VLAN membership check.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_port_vlan_check_disable_get(int unit, bcm_port_t port, uint16_t *flags);

/*!
 * \brief Get flags to disable STG check.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] flags Flags to disable STG check.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_port_stg_check_disable_get(int unit, bcm_port_t port, uint16_t *flags);

/*!
 * \brief Get port number form dport number.
 *
 * \param [in] unit Unit Number.
 * \param [in] dport Dport number.
 *
 * \retval Port number.
 */
int
bcmi_ltsw_dport_to_port(int unit, int dport);

/*!
 * \brief Get dport number form port number.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 *
 * \retval Dport number.
 */
int
bcmi_ltsw_port_to_dport(int unit, int port);

/*!
 * \brief Map port number to dport number.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] dport Dport number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_dport_map(int unit, int port, int dport);

/*
 * \brief Dump per-port asf configurations to diag Shell.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  Logical port number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_port_asf_diag(int unit, bcm_port_t port);

/*
 * \brief Print per-port traffic forwarding mode in diag Shell.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  Logical port number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_ltsw_port_asf_show(int unit, bcm_port_t port);

/*!
 * \brief De-initialize the PORT interface layer for the specified SOC device.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_deinit(int unit);

/*!
 * \brief Dump sw infomation of port module.
 *
 * \param [in] unit Unit number.
 *
 * \return no return.
 */
extern void
bcmi_ltsw_port_dump_sw(int unit);

/*!
 * \brief Helper funtion to Remaps module and port from HW space to API space.
 *
 * If mod_out == NULL then port_out will be local logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] mod_in Module ID to map.
 * \param [in] port_in Port number to map.
 * \param [out] mod_out Module ID after mapping.
 * \param [out] port_out Port number after mapping.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_modport_hw2api_map(int unit, bcm_module_t mod_in, bcm_port_t port_in,
                             bcm_module_t *mod_out, bcm_port_t *port_out);

/*!
 * \brief Get PortMacro id by logic port.
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] pm_id PortMacro id.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_pm_id_get(int unit, bcm_port_t port, int * pm_id);

/*!
 * \brief lag failover enable.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] enable Enable or Disable.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_lag_failover_enable_set(int unit, bcm_port_t port, uint32_t enable);

/*!
 * \brief lag failover enable get.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [OUT] enable Enable or Disable.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_lag_failover_enable_get(int unit, bcm_port_t port, uint32_t *enable);

/*!
 * \brief lag failover loopback get.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [OUT] value loopback or not.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_port_lag_failover_loopback_get(int unit, bcm_port_t port, uint32_t *value);

/*!
 * \brief Get which pipes are using identified  L2 interface.
 *
 * \param [in] unit Unit Number.
 * \param [in] l2_if L2 interface.
 * \param [out] pipe_bmp Pipe bitmap.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_l2_if_to_pipes(int unit, int l2_if, uint32_t *pipe_bmp);
#endif

