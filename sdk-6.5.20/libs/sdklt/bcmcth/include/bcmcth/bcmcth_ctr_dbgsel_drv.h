/*! \file bcmcth_ctr_dbgsel_drv.h
 *
 * BCMCTH CTR_DBGSEL driver objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_CTR_DBGSEL_DRV_H
#define BCMCTH_CTR_DBGSEL_DRV_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>

/*!
 * \brief Counter debug select triggers.
 */
typedef enum {
    /*! Enable to count ECMP not resolved. */
    CTR_ING_DBG_SEL_ECMP_NOT_RESOLVED,

    /*!
     * Enable to count received virtual output queue (VOQ) based
     * flow control message packets.
     */
    CTR_ING_DBG_SEL_RX_VOQ_FLOW_CTRL,

    /*! Enable to count unknown BFD version or discard dropped. */
    CTR_ING_DBG_SEL_BFD_UNKNOWN_VER_OR_DISCARD,

    /*!
     * Enable to count BFD packets with non-zero version in the ACH header
     * or the ACH channel type is unknown.
     */
    CTR_ING_DBG_SEL_BFD_UNKNOWN_ACH_ERR,

    /*!
     * Enable to count unrecognized BFD control packets received from the
     * PW VCCM type 1/2/3 or MPLS-TP control channel.
     */
    CTR_ING_DBG_SEL_BFD_UNKNOWN_CTRL_PKT,

    /*! Enable to count TRILL RPF check fail drops. */
    CTR_ING_DBG_SEL_TRILL_RPF_CHECK_FAIL_DROP,

    /*! Enable to count TRILL hop count check fail drops. */
    CTR_ING_DBG_SEL_TRILL_HOPCOUNT_CHECK_FAIL,

    /*! Enable to count TRILL routing bridge lookup miss drops. */
    CTR_ING_DBG_SEL_TRILL_RBRIDGE_LOOKUP_MISS_DROP,

    /*! Enable to count TRILL error drops. */
    CTR_ING_DBG_SEL_OFFSET_TRILL_ERRORS_DROP,

    /*! Enable to count TRILL frame drops on access port. */
    CTR_ING_DBG_SEL_TRILL_FRAME_ON_ACCESS_PORT_DROP,

    /*! Enable to count non-TRILL frame drops on network port. */
    CTR_ING_DBG_SEL_NON_TRILL_FRAME_ON_NETWORK_PORT_DROP,

    /*! Enable to count NIV forwarding error drops. */
    CTR_ING_DBG_SEL_NIV_FORWARDING_DROP,

    /*! Enable to count packets dropped due to VNTAG error. */
    CTR_ING_DBG_SEL_VNTAG_ERROR,

    /*! Enable to count packets dropped due to MAC limit exceeded. */
    CTR_ING_DBG_SEL_MAC_LIMIT_DROP,

    /*! Enable to count MAC limit exceeded and packet not dropped. */
    CTR_ING_DBG_SEL_MAC_LIMIT_NODROP,

    /*! Enable to count L2 DoS attack packets. */
    CTR_ING_DBG_SEL_DOS_L2,

    /*! Enable to count class based learning drop. */
    CTR_ING_DBG_SEL_CLASS_BASED_LEARN_DROP,

    /*! Enable to count packets dropped due to L2/L3 lookup destination discard. */
    CTR_ING_DBG_SEL_DST_DISCARD,

    /*! Enable to count VLAN field processor (FP) drops. */
    CTR_ING_DBG_SEL_FP_VLAN_DROP,

    /*! Enable to count packets dropped due to uRPF lookup failure. */
    CTR_ING_DBG_SEL_URPF,

    /*! Enable to count received HiGig non-unicast packets. */
    CTR_ING_DBG_SEL_RX_HIGIG_NON_UC,

    /*! Enable to count received HiGig unicast packets. */
    CTR_ING_DBG_SEL_RX_HIGIG_UC,

    /*! Enable to count packets dropped due to parity error. */
    CTR_ING_DBG_SEL_PARITY_ERR_DROP,

    /*!
     * Enable to count trunk failover loopback packets discarded
     * due to the backup port being down in packet pre-processing stage.
     */
    CTR_ING_DBG_SEL_TRUNK_FAILOVER_LOOPBACK_DISCARD,

    /*!
     * Enable to count trunk failover loopback packets in
     * packet pre-processing stage.
     */
    CTR_ING_DBG_SEL_TRUNK_FAILOVER_LOOPBACK,

    /*! Enable to count packets with multicast index error. */
    CTR_ING_DBG_SEL_MC_INDEX_ERR,

    /*! Enable to count HiGig header error packets. */
    CTR_ING_DBG_SEL_HIGIG_ERR,

    /*! Enable to count received VLAN drop packets. */
    CTR_ING_DBG_SEL_RX_VLAN_DROP,

    /*! Enable to count received tunnel error packets. */
    CTR_ING_DBG_SEL_RX_TNL_ERR,

    /*! Enable to count the number of tunnel packets received. */
    CTR_ING_DBG_SEL_RX_TNL,

    /*! Enable to count packets copied to CPU due to egress L3 MTU violation. */
    CTR_ING_DBG_SEL_L3_MTU_ERR,

    /*! Enable to count DoS fragment error packets. */
    CTR_ING_DBG_SEL_DOS_FRAGMENT,

    /*! Enable to count DoS ICMP error packets. */
    CTR_ING_DBG_SEL_DOS_ICMP,

    /*! Enable to count DoS L4 header error packets. */
    CTR_ING_DBG_SEL_DOS_L4_HDR_ERR,

    /*! Enable to count HiGig mirror packet. */
    CTR_ING_DBG_SEL_HIGIG_MIRROR,

    /*! Enable to count DOS L3 header error packets. */
    CTR_ING_DBG_SEL_DOS_L3_HDR_ERR,

    /*! Enable to count unknown HiGig header type packets. */
    CTR_ING_DBG_SEL_HIGIG_UNKNOWN_HDR_TYPE,

    /*! Enable to count received HiGig end-to-end IBP packets. */
    CTR_ING_DBG_SEL_RX_HIGIG_IBP,

    /*! Enable to count received HiGig end-to-end HOL packets. */
    CTR_ING_DBG_SEL_RX_HIGIG_HOL,

    /*! Enable to count received HiGig IPIC pause packets. */
    CTR_ING_DBG_SEL_RX_HIGIG_PAUSE,

    /*! Enable to count zero port bitmap drop condition. */
    CTR_ING_DBG_SEL_RX_DROP,

    /*! Enable to count multicast (L2+L3) packets that are dropped. */
    CTR_ING_DBG_SEL_RX_MC_DROP,

    /*! Enable to count packets dropped by the FP. */
    CTR_ING_DBG_SEL_FP_DROP,

    /*! Enable to count bridged multicast packets. */
    CTR_ING_DBG_SEL_MC_BRIDGED,

    /*! Enable to count received policy discards. */
    CTR_ING_DBG_SEL_RX_POLICY_DISCARD,

    /*!
     * Enable to count packets dropped when the ingress port is not
     * in a forwarding state.
     */
    CTR_ING_DBG_SEL_RX_PORT_DROP,

    /*! Enable to count good received unicast (L2+L3) packets. */
    CTR_ING_DBG_SEL_RX_UC,

    /*! Enable to count packets dropped due to unavailability of TM buffer resources. */
    CTR_ING_DBG_SEL_RX_TM_BUFFER_DISCARD,

    /*! Enable to count routed IPv6 multicast packets. */
    CTR_ING_DBG_SEL_IPV6_MC_ROUTED,

    /*!
     * Enable to count received IPv6 header error packets
     * (IPv6 martian error addresses + link local).
     */
    CTR_ING_DBG_SEL_RX_IPV6_HDR_ERR,

    /*! Enable to count good received IPv6 L3 packets, including tunneled packets. */
    CTR_ING_DBG_SEL_RX_IPV6,

    /*! Enable to count received L3 IPv6 discarded packets. */
    CTR_ING_DBG_SEL_RX_IPV6_DISCARD,

    /*! Enable to count routed IPv4 multicast packets. */
    CTR_ING_DBG_SEL_IPV4_MC_ROUTED,

    /*!
     * Enable to count received IPv4 header error packets
     * (martian error address + unicast with Class D + multicast & ~ClassD)
     */
    CTR_ING_DBG_SEL_RX_IPV4_HDR_ERR,

    /*! Enable to count good received IPv4 L3, including tunneled packets. */
    CTR_ING_DBG_SEL_RX_IPV4,

    /*! Enable to count received L3 IPv4 discarded packets. */
    CTR_ING_DBG_SEL_RX_IPV4_DISCARD,

    /*!
     * Enable to count packets dropped due to mismatched egress port for
     * the VLAN.
     */
    CTR_ING_DBG_SEL_VLAN_MEMBERSHIP_DROP,

    /*! Enable to count packets dropped due to VLAN blocked ports. */
    CTR_ING_DBG_SEL_VLAN_BLOCKED_DROP,

    /*! Enable to count packets dropped due to spanning tree group state. */
    CTR_ING_DBG_SEL_VLAN_STG_DROP,

    /*!
     * Enable to count packets dropped due to multicast and broadcast trunk
     * block mask.
     */
    CTR_ING_DBG_SEL_NONUC_TRUNK_RESOLUTION_MASK,

    /*!
     * Enable to count packets dropped due to
     * unknown unicast, unknown multicast, known multicast,
     * and broadcast block masks.
     */
    CTR_ING_DBG_SEL_NONUC_MASK,

    /*! Enable to count packets dropped due to multicast errors. */
    CTR_ING_DBG_SEL_MC_DROP,

    /*! Enable to count packets dropped due to MAC block mask. */
    CTR_ING_DBG_SEL_MAC_MASK,

    /*!
     * Enable to count packets dropped due to block traffic
     * from egressing based on the ingress port.
     */
    CTR_ING_DBG_SEL_ING_EGR_MASK,

    /*! Enable to count packets dropped due to ingress FP delayed action. */
    CTR_ING_DBG_SEL_FP_ING_DELAYED_DROP,

    /*! Enable to count packets dropped due to FP redirection mask. */
    CTR_ING_DBG_SEL_FP_REDIRECT_MASK,

    /*! Enable to count packets dropped due to egress mask. */
    CTR_ING_DBG_SEL_EGR_MASK,

    /*! Enable to count packets dropped due to source port knockout. */
    CTR_ING_DBG_SEL_SRC_PORT_KNOCKOUT_DROP,

    /*! Enable to count non-IP packet discard dropped. */
    CTR_ING_DBG_SEL_NON_IP_DISCARD,

    /*! Enable to count packets dropped due to MTU check fail. */
    CTR_ING_DBG_SEL_MTU_CHECK_FAIL,

    /*! Enable to count packets dropped due to block masks. */
    CTR_ING_DBG_SEL_BLOCK_MASK_DROP,

    /*! Enable to count packets dropped due to PVLAN VID mismatch. */
    CTR_ING_DBG_SEL_PVLAN_VID_MISMATCH,

    /*! Enable to count packets dropped due to invalid VLAN. */
    CTR_ING_DBG_SEL_INVALID_VLAN,

    /*! Enable to count packets dropped due to TPID mismatch. */
    CTR_ING_DBG_SEL_INVALID_TPID,

    /*!
     * Enable to count packets dropped due to ingress port not in
     * VLAN membership.
     */
    CTR_ING_DBG_SEL_PORT_ING_VLAN_MEMBERSHIP,

    /*! Enable to count VLAN cross-connect drop. */
    CTR_ING_DBG_SEL_VLAN_CC_OR_PBT,

    /*! Enable to count packets dropped due to VLAN FP. */
    CTR_ING_DBG_SEL_VLAN_FP,

    /*! Enable to count packets dropped due to MPLS TTL check fail. */
    CTR_ING_DBG_SEL_MPLS_TTL_CHECK_FAIL,

    /*! Enable to count packets dropped due to MPLS label lookup miss. */
    CTR_ING_DBG_SEL_MPLS_LABEL_MISS,

    /*! Enable to count packets dropped due to MPLS invalid payload. */
    CTR_ING_DBG_SEL_MPLS_INVALID_PAYLOAD,

    /*! Enable to count packets dropped due to MPLS invalid control word. */
    CTR_ING_DBG_SEL_MPLS_INVALID_CW,

    /*! Enable to count packets dropped due to MPLS invalid action. */
    CTR_ING_DBG_SEL_MPLS_INVALID_ACTION,

    /*! Enable to count packets dropped due to MPLS generic associated label lookup miss. */
    CTR_ING_DBG_SEL_MPLS_GAL_LABEL,

    /*! Enable to count tunnel decap ECN error drop. */
    CTR_ING_DBG_SEL_TNL_DECAP_ECN,

    /*! Enable to count time sync drop. */
    CTR_ING_DBG_SEL_TIME_SYNC_PKT,

    /*!
     * Enable to count packets dropped due to port configuration for dropping
     * all tagged packets or untagged packets.
     */
    CTR_ING_DBG_SEL_TAG_UNTAG_DISCARD,

    /*! Enable to count packets dropped due to source route. */
    CTR_ING_DBG_SEL_SRC_ROUTE,

    /*! Enable to count packets dropped due to spanning tree. */
    CTR_ING_DBG_SEL_SPANNING_TREE_STATE,

    /*! Enable to count packets dropped due to tunnel errors. */
    CTR_ING_DBG_SEL_TNL_ERROR,

    /*! Enable to count packets dropped due to protection. */
    CTR_ING_DBG_SEL_PROTECTION_DATA_DROP,

    /*! Enable to count packets dropped due to next hop indication. */
    CTR_ING_DBG_SEL_NHOP_DROP,

    /*! Enable to count packets dropped due to ingress FP drop. */
    CTR_ING_DBG_SEL_FP_ING_DROP,

    /*! Enable to count packets dropped due to invalid FP redirection. */
    CTR_ING_DBG_SEL_FP_REDIRECT_DROP,

    /*! Enable to count packets dropped due to ECMP resolution error. */
    CTR_ING_DBG_SEL_ECMP_RESOLUTION_ERR,

    /*! Enable to count private VLAN VP filter drop. */
    CTR_ING_DBG_SEL_PVLAN_VP_FILTER,

    /*! Enable to count packets dropped due to protocol packet drop control. */
    CTR_ING_DBG_SEL_PROTCOL_PKT_CTRL_DROP,

    /*! Enable to count port filtering mode drop. */
    CTR_ING_DBG_SEL_PORT_FILTERING_MODE,

    /*! Enable to count packets dropped due to L2 my station lookup miss. */
    CTR_ING_DBG_SEL_L2_MY_STATION,

    /*! Enable to count MPLS stage drop. */
    CTR_ING_DBG_SEL_MPLS_STAGE,

    /*! Enable to count source MAC address is zero. */
    CTR_ING_DBG_SEL_SRC_MAC_ZERO,

    /*! Enable to count L3 TTL error drop. */
    CTR_ING_DBG_SEL_L3_TTL_ERR,

    /*! Enable to count L3 header error drop. */
    CTR_ING_DBG_SEL_L3_HDR_ERR,

    /*! Enable to count L3 destination lookup miss drop. */
    CTR_ING_DBG_SEL_DST_L3_LOOKUP_MISS,

    /*! Enable to count L3 source discard drop. */
    CTR_ING_DBG_SEL_SRC_L3_DISCARD,

    /*! Enable to count L3 destination discard drop. */
    CTR_ING_DBG_SEL_DST_L3_DISCARD,

    /*! Enable to count L2 source static move drop. */
    CTR_ING_DBG_SEL_SRC_L2_STATIC_MOVE,

    /*! Enable to count packets dropped due to L2 source discard. */
    CTR_ING_DBG_SEL_SRC_L2_DISCARD,

    /*! Enable to count packets dropped due to L2 destination discard. */
    CTR_ING_DBG_SEL_DST_L2_DISCARD,

    /*! Enable to count IP multicast header error drop. */
    CTR_ING_DBG_SEL_IP_MC_DROP,

    /*!
     * Enable to count packets dropped due to source MAC address equal to
     * destination MAC address.
     */
    CTR_ING_DBG_SEL_SRC_MAC_EQUALS_DST_MAC,

    /*!
     * Enable to count packets looped back because of trunk failover
     * in packet discard stage.
     */
    CTR_ING_DBG_SEL_TRUNK_FAIL_LOOPBACK,

    /*! Enable to count packets dropped due to L4 DoS attack. */
    CTR_ING_DBG_SEL_DOS_L4_ATTACK,

    /*! Enable to count packets dropped due to L3 DoS attack. */
    CTR_ING_DBG_SEL_DOS_L3_ATTACK,

    /*! Enable to count packets dropped due to IPv6 protocol error. */
    CTR_ING_DBG_SEL_IPV6_PROTOCOL_ERR,

    /*! Enable to count packets dropped due to IPv4 protocol error. */
    CTR_ING_DBG_SEL_IPV4_PROTOCOL_ERR,

    /*! Enable to count packets dropped due to MAC control frame. */
    CTR_ING_DBG_SEL_MAC_CONTROL_FRAME,

    /*! Enable to count packets dropped due to parity/ecc errors. */
    CTR_ING_DBG_SEL_COMPOSITE_ERROR,

    /*! Enable to count packets dropped due to CPU managed learning (CML). */
    CTR_ING_DBG_SEL_CPU_MANAGED_LEARNING,

    /*! Enable to count packets dropped due to CFI or L3 disable. */
    CTR_ING_DBG_SEL_CFI_OR_L3_DISABLE,

    /*! Enable to count packets dropped due to BPDU. */
    CTR_ING_DBG_SEL_BPDU,

    /*! Enable to count BFD terminated drop. */
    CTR_ING_DBG_SEL_BFD_TERMINATED_DROP,

    /*!
     * Enable to count INT (In-band Network Telemetry) packets
     * dropped due to request vector match failure.
     */
    CTR_ING_DBG_SEL_INT_VECTOR_MISS_MATCH_DROP,

    /*!
     * Enable to count INT-DP (In-band Network Telemetry Data-plane
     * Probe) packets dropped due to exception, including turn around
     * and hop limit exceeding.
     */
    CTR_ING_DBG_SEL_INT_DP_EXCEPTION_DROP,

    /*!
     * Enable to count packets dropped due to VFI ingress adaption first
     * lookup miss drop or both first and second lookup miss drop.
     */
    CTR_ING_DBG_SEL_VFI_ING_ADAPT_LOOKUP_MISS_DROP,

    /*!
     * Enable to count packets dropped due to overlay
     * and underlay next hop or L3 egress interface ID range error drop.
     */
    CTR_ING_DBG_SEL_OVERLAY_UNDERLAY_NHOP_EIF_RANGE_ERROR_DROP,

    /*!
     * Enable to count VXLAN packet dropped due to source IP
     * lookup miss during tunnel decapsulation.
     */
    CTR_ING_DBG_SEL_TNL_VXLAN_DECAP_SRC_IP_LOOKUP_MISS_DROP,

    /*! Enable to count packets whose size is too small. */
    CTR_EGR_DBG_SEL_PKT_TOO_SMALL,

    /*! Enable to count invalid IEEE 1588 packets transmitted. */
    CTR_EGR_DBG_SEL_INVALID_1588_PKT,

    /*! Enable to count packets dropped due to NIV prune. */
    CTR_EGR_DBG_SEL_NIV_PRUNE,

    /*!
     * Enable to count egress packets dropped due to packet size becoming
     * too large after additional bytes were added.
     */
    CTR_EGR_DBG_SEL_PKT_TOO_LARGE,

    /*! Enable to count packets dropped due to module ID greater than 31. */
    CTR_EGR_DBG_SEL_MODID_GT_31,

    /*! Enable to count packets dropped due to IPMC self interface pruned. */
    CTR_EGR_DBG_SEL_IPMC_L3_SELF_INTF,

    /*! Enable to count packets dropped due to parity error. */
    CTR_EGR_DBG_SEL_PARITY_ERR,

    /*! Enable to count packets dropped due to L2 MTU check failure. */
    CTR_EGR_DBG_SEL_L2_MTU_FAIL,

    /*! Enable to count packets dropped due to unknown HiGig packets. */
    CTR_EGR_DBG_SEL_UNKNOWN_HIGIG,

    /*! Enable to count packets dropped due to HiGig2 packet destination is not a HiGig port. */
    CTR_EGR_DBG_SEL_INVALID_HIGIG2_DST_PORT,

    /*! Enable to count HiGig multicast packets transmitted. */
    CTR_EGR_DBG_SEL_HIGIG_MC,

    /*! Enable to count HiGig unicast packets transmitted. */
    CTR_EGR_DBG_SEL_HIGIG_UC,

    /*! Enable to count packets dropped due to link local source IP . */
    CTR_EGR_DBG_SEL_SRC_IP_LINK_LOCAL,

    /*! Enable to count mirroring packets. */
    CTR_EGR_DBG_SEL_MIRROR,

    /*! Enable to count packets dropped due to any condition. */
    CTR_EGR_DBG_SEL_PKT_DROP,

    /*! Enable to count dropped L2 multicast packets*/
    CTR_EGR_DBG_SEL_L2_MC_DROP,

    /*!
     * Enable to count packets dropped due to forwarding spanning
     * tree state.
     */
    CTR_EGR_DBG_SEL_STG_DROP,

    /*! Enable to count packets dropped due to VLAN translation lookup miss. */
    CTR_EGR_DBG_SEL_VLAN_XLATE_MISS,

    /*! Enable to count packets dropped due to invalid VLAN ID. */
    CTR_EGR_DBG_SEL_INVALID_VLAN,

    /*! Enable to count packets of VLAN tagged transmitted. */
    CTR_EGR_DBG_SEL_VLAN_TAGGED,

    /*! Enable to count packets dropped due to CFI bit set. */
    CTR_EGR_DBG_SEL_CFI_DROP,

    /*! Enable to count packets dropped due to TTL threshold exceeded. */
    CTR_EGR_DBG_SEL_TTL_DROP,

    /*! Enable to count tunnel error packets transmitted. */
    CTR_EGR_DBG_SEL_TNL_ERR,

    /*! Enable to count tunnel packets transmitted. */
    CTR_EGR_DBG_SEL_TNL_PKT,

    /*! Enable to count IPv6 IPMC aged and drop packets. */
    CTR_EGR_DBG_SEL_IPMCV6_DROP,

    /*! Enable to count good IPv6 IPMC packets transmitted. */
    CTR_EGR_DBG_SEL_IPMCV6_PKT,

    /*! Enable to count IPv6 L3 aged and drop packets transmitted. */
    CTR_EGR_DBG_SEL_IPV6_DROP,

    /*! Enable to count good IPv6 L3 unicast packets transmitted. */
    CTR_EGR_DBG_SEL_IPV6_PKT,

    /*! Enable to count IPv4 IPMC aged and drop packets transmitted. */
    CTR_EGR_DBG_SEL_IPMCV4_DROP,

    /*! Enable to count good IPv4 IPMC packets transmitted. */
    CTR_EGR_DBG_SEL_IPMCV4_PKT,

    /*! Enable to count IPv4 L3 aged and drop packets transmitted. */
    CTR_EGR_DBG_SEL_IPV4_DROP,

    /*! Enable to count good IPv4 L3 unicast packets transmitted. */
    CTR_EGR_DBG_SEL_IPV4_PKT,

    /*!
     * Enable to count multi-cell packets whose SOP cell size is smaller than
     * 64 bytes after decap.
     */
    CTR_EGR_DBG_SEL_CELL_TOO_SMALL,

    /*!
     * Enable to count packets dropped due to loopback packet destination
     * is also loopback port.
     */
    CTR_EGR_DBG_SEL_LB_TO_LB_DROP,

    CTR_DBG_SEL_NUM
} ctr_dbgsel_trigger;

/*!
 * \brief CTR_DBGSEL LT operations.
 */
typedef enum {
    /*! LT insertion. */
    CTR_DBGSEL_INSERT,

    /*! LT updating. */
    CTR_DBGSEL_UPDATE,

    /*! LT deletion. */
    CTR_DBGSEL_DELETE
} ctr_dbgsel_lt_op;

/*!
 * \brief Trigger structure.
 */
typedef struct ctr_dbgsel_trigger_s {
    /*! Activation state of the trigger. */
    bool enable;

    /*! LT field name of the trigger. */
    ctr_dbgsel_trigger name;
} ctr_dbgsel_trigger_t;

/*!
 * \brief Set the hardware for CTR_DBG_SEL_EGR_STG_PROFILEt.
 *
 * \param [in] unit Unit number.
 * \param [in] ingress ingress/egress identifier.
 * \param [in] idx Debug register selector.
 * \param [in] trigger_array Trigger array.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ctr_dbgsel_set_f)(
    int unit,
    bool ingress,
    int op,
    uint8_t idx,
    ctr_dbgsel_trigger_t *trigger_array);

/*!
 * \brief CTR_DBGSEL driver object
 *
 * CTR_DBGSEL driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH CTR_DBGSEL module by
 * \ref bcmcth_ctr_dbgsel_drv_init from the top layer. BCMCTH CTR_DBGSEL internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_ctr_dbgsel_drv_s {
    /*! Set debug counter triggers. */
    bcmcth_ctr_dbgsel_set_f dbgsel_set;
} bcmcth_ctr_dbgsel_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,\
                             _fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_ctr_dbgsel_drv_t *_bc##_cth_ctr_dbgsel_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Install the device-specific CTR_DBGSEL driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_dbgsel_drv_init(int unit);

/*!
 * \brief Do the internal hardware configuration for UPDATE operation.
 *
 * \param [in] unit Unit number.
 * \param [in] ingress Ingress/Egress switch.
 * \param [in] op LT operation.
 * \param [in] idx Debug register selector.
 * \param [in] trigger_array Trigger array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ctr_dbgsel_set(int unit, bool ingress, int op,
                      uint8_t idx, ctr_dbgsel_trigger_t *trigger_array);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The CTR_DBGSEL custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_dbgsel_imm_init(int unit);

#endif /* BCMCTH_CTR_DBGSEL_DRV_H */

