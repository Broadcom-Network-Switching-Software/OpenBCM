/*! \file l3.h
 *
 * L3 header file.
 * This file contains L3 definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef BCMI_LTSW_L3_H
#define BCMI_LTSW_L3_H
#include <shr/shr_types.h>
#include <bcm_int/ltsw/types.h>

#define BCMI_LTSW_L3_MTU_DEFAULT_SIZE 0x3FFF

/*!
 * L3 MTU type.
 */
typedef enum {
    /*! L3_UC_MTU. */
    L3_MTU_TYPE_UC,

    /*! L3_MC_TYPE encap. */
    L3_MTU_TYPE_MC,

    /*! L3_UC_TUNNEL_MTU encap. */
    L3_MTU_TYPE_TUNNEL,

    /*! Last mtu type. */
    L3_MTU_TYPE_COUNT
} l3_mtu_type;


typedef struct bcmi_ltsw_l3_mtu_cfg_s {

    /*! L3 MTU index. */
    int index;

    /*! L3 MTU size. */
    int mtu;

    /*! L3 MTU type. */
    l3_mtu_type type;
} bcmi_ltsw_l3_mtu_cfg_t;

/*!
 * \brief Application switch control type of L3 for a specific device.
 */
typedef enum bcmi_ltsw_l3_control_s {

    /*! Non-IP packets w/DA of my router MAC to CPU. */
    bcmiL3ControlNonIpL3ErrToCpu = 0,

    /*! DIP not found in L3/LPM tables. */
    bcmiL3ControlUnknownL3DestToCpu = 1,

    /*! Invalid IPv6 packets to CPU. */
    bcmiL3ControlV6L3ErrToCpu = 2,

    /*! IPv6 unknown dest packets to CPU. */
    bcmiL3ControlV6L3DstMissToCpu = 3,

    /*! Drop IPv6 with link local sources. */
    bcmiL3ControlV6L3LocalLinkDrop = 4,

    /*! Invalid IPv6 packets to CPU. */
    bcmiL3ControlV4L3ErrToCpu = 5,

    /*! IPv6 unknown dest packets to CPU. */
    bcmiL3ControlV4L3DstMissToCpu = 6,

    /*! Martian packets to CPU. */
    bcmiL3ControlMartianAddrToCpu = 7,

    /*! L3 unicast w/invalid TTL to CPU. */
    bcmiL3ControlL3UcTtlErrToCpu = 8,

    /*! Slowpath packets to CPU. */
    bcmiL3ControlL3SlowpathToCpu = 9,

    /*! L3 MTU check failure. */
    bcmiL3ControlL3MtuFailToCpu = 10,

    /*! Copy L3 Ucast with TTL 1 to CPU. */
    bcmiL3ControlL3UcastTtl1ToCpu = 11,

    /*! Enables/Disables source hit bit lookup update in the L3 table */
    bcmiL3ControlL3SrcHitEnable = 12,

    /*! Programmable value for user defined extension header 1 & 2. */
    bcmiL3ControlFlexIP6ExtHdr = 13,

    /*! Egress programmable value for user defined extension header 1 & 2. */
    bcmiL3ControlFlexIP6ExtHdrEgress = 14,

    /*!
     * Do not drop IPv6. It will override drop control
     * bcmSwitchV6L3LocalLinkDrop if source and destination are local link.
     */
    bcmiL3ControlV6L3SrcDstLocalLinkDropCancel = 15,

    /*! MC unknown Index Error to CPU. */
    bcmiL3ControlMcastUnknownErrToCpu = 16,

    /*! Incoming port not in IPMC entry. */
    bcmiL3ControlIpmcPortMissToCpu = 17,

    /*! IPMC pkt has header err or TTL=0. */
    bcmiL3ControlIpmcErrorToCpu = 18,

    /*! IPMC packets w/invalid TTL to CPU. */
    bcmiL3ControlIpmcTtlErrToCpu = 19,

    /*! IGMP packets with reserved MC address threated as IGMP. */
    bcmiL3ControlIgmpReservedMcastEnable = 20,

    /*! MLD packets with reserved MC address threated as MLD. */
    bcmiL3ControlMldReservedMcastEnable = 21,

    /*! Stricter checks for MLD packets are enable. */
    bcmiL3ControlMldDirectAttachedOnly = 22,

    /*! Threat unknown IPMC as L2 Mcast. */
    bcmiL3ControlUnknownIpmcAsMcast = 23,

    /*! Drop unknown IPMC for incomming tunnel. */
    bcmiL3ControlTunnelUnknownIpmcDrop = 24,

    /*! Copy L3 Mcast with TTL 1 to CPU. */
    bcmiL3ControlIpmcTtl1ToCpu = 25,

    /*!
     * Allow both L2 and L3 copies of a packet to
     * go out on a physical port if it happens to be a member of
     * both the L2 and L3 bitmaps of the replication group.
     */
    bcmiL3ControlIpmcSameVlanPruning = 26,

    /*! Hash Select for Multipath. */
    bcmiL3ControlEcmpHashMultipath = 27,

    /*! Hash Control of fields. */
    bcmiL3ControlEcmpHashControl = 28,

    /*! Enable/Disable flow based hashing for ECMP. */
    bcmiL3ControlEcmpHashUseFlowSelEcmp = 29,

    /*!
     * To select the most significant bits of the 16-bit ECMP hash value
     * before the modulo function.
     */
    bcmiL3ControlEcmpHashBitCountSelect = 30,

    /*! Enable unicast packets with IGMP payload. */
    bcmiL3ControlIgmpUcastEnable = 31,

    /*! Enable unicast packets with MLD payload. */
    bcmiL3ControlMldUcastEnable = 32,

    /*! Check MACDA and IPv4 DIP address  match. */
    bcmiL3ControlIp4McastL2DestCheck = 33,

    /*! Check MACDA and IPv6 DIP address match. */
    bcmiL3ControlIp6McastL2DestCheck = 34,

    /*! Enable forwarding of known IPMC packets to the incoming vlan. */
    bcmiL3ControlIpmcSameVlanL3Route = 35,

    /*! Enable/Disable flow based hashing for weighted ECMP. */
    bcmiL3ControlFlowSelWeightedEcmp = 36,

    /*! Enables/Disables parsing into WESP payload. */
    bcmiL3ControlWESPProtocolEnable = 37,

    /*! WESP protocol number. */
    bcmiL3ControlWESPProtocol = 38,

    /*! Random single level ECMP seed. */
    bcmiL3ControlECMPRandomSeed = 39,

    /*! Random level1 ECMP seed. */
    bcmiL3ControlECMPLevel1RandomSeed = 40,

    /*! Random level2 ECMP seed. */
    bcmiL3ControlECMPLevel2RandomSeed = 41,

    /* Enable/Disable flow based hashing for overlay ECMP. */
    bcmiL3ControlFlowSelOverlayEcmp = 42,

    /* Enable/Disable flow based hashing for L2 ECMP. */
    bcmiL3ControlFlowSelL2Ecmp = 43,

    /* Enable/Disable flow based hashing for overlay L2 ECMP. */
    bcmiL3ControlFlowSelOverlayL2Ecmp = 44,

    /* Enable/Disable flow based hashing for resilient ECMP. */
    bcmiL3ControlFlowSelRhEcmp = 45,

    /* Enable/Disable flow based hashing for overlay resilient ECMP. */
    bcmiL3ControlFlowSelOverlayRhEcmp = 46,

    /* Enable/Disable flow based hashing for overlay weighted ECMP. */
    bcmiL3ControlFlowSelOverlayWeightedEcmp = 47,

    /*! Enable/Disable flow based hashing for ECMP DLB. */
    bcmiL3ControlFlowSelEcmpDynamic = 48,

    /*! L3 control type count. */
    bcmiL3ControlCount

} bcmi_ltsw_l3_control_t;

typedef struct bcmi_ltsw_l3_ipv4_options_profile_s {
    /*! Enable drop. */
    bool     drop;

    /*! Enable copy to CPU. */
    bool     copy_to_cpu;
} bcmi_ltsw_l3_ipv4_options_profile_t;


#define BCMI_LTSW_L3_PC_ATTR_IPMC_UNTAG      (1 << 0)
#define BCMI_LTSW_L3_PC_ATTR_IPMC_TTL_DEC    (1 << 1)
#define BCMI_LTSW_L3_PC_ATTR_IPMC_TTL_CHECK  (1 << 2)
#define BCMI_LTSW_L3_PC_ATTR_IPMC_SA_REPLACE (1 << 3)
#define BCMI_LTSW_L3_PC_ATTR_IPMC_VLAN       (1 << 4)

typedef struct bcmi_ltsw_l3_port_control_s {
    /*! Enable untag IPMC packets. */
    bool     ipmc_untag;

    /*! Enable TTL decrement for IPMC packets. */
    bool     ipmc_ttl_dec;

    /*! Enable ttl check for IPMC packets. */
    bool     ipmc_ttl_check;

    /*! Enable to replace SA for IPMC packets. */
    bool     ipmc_sa_replace;

    /*! Untag VID for IPMC packets. */
    uint16_t ipmc_vid;

} bcmi_ltsw_l3_port_control_t;

/*!
 * \brief Set mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [in] cfg MTU Information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_mtu_set(
    int unit,
    bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Get mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [out] cfg MTU Information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_mtu_get(
    int unit,
    bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Delete mtu value.
 *
 * \param [in] unit Unit Number.
 * \param [in] mtu_idx MTU entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_mtu_delete(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg);

/*!
 * \brief Recover mtu database from LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] mtu_idx MTU entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_mtu_recover(int unit, int mtu_idx);

/*! Full ipv4 mask.*/
#define BCMI_LTSW_L3_IP4_FULL_MASK 0xffffffff

/*! L3_IP_OPTION_CONTROL_PROFILE size per profile. */
#define BCMI_LTSW_L3_IP_OPTION_PROFILE_CHUNK 256

/*! L3 IP options Default Profile */
#define BCM_L3_IP4_OPTIONS_DEF_PROFILE_INDEX 0x00

extern int bcmi_ltsw_l3_mask6_apply(bcm_ip6_t mask, bcm_ip6_t addr);

/*!
 * \brief Set L3 controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] control Switch control.
 * \param [in] value Type-based controlling value for L3.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_control_set(
    int unit,
    bcmi_ltsw_l3_control_t control,
    uint32_t value);

/*!
 * \brief Get L3 controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] control Switch control.
 * \param [out] value Type-based controlling value for L3.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_control_get(
    int unit,
    bcmi_ltsw_l3_control_t control,
    uint32_t *value);

/*!
 * \brief Set L3 port controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] control Control type.
 * \param [in] info Control infomation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_port_control_set(
    int unit,
    int port,
    int control,
    bcmi_ltsw_l3_port_control_t *info);

/*!
 * \brief Get L3 port controlling information.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] control Control type.
 * \param [out] info Control infomation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_port_control_get(
    int unit,
    int port,
    int control,
    bcmi_ltsw_l3_port_control_t *info);

/*!
 * \brief Set ipv6 reserved multicast address and mask.
 *
 * \param [in] unit Unit number.
 * \param [in] num Number of ipv6 address/mask pairs
 * \param [in] ip6_addr  Ipv6 address.
 * \param [in] ip6_mask Ipv6 address mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_ipv6_reserved_multicast_addr_multi_set(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask);

/*!
 * \brief Get ipv6 reserved multicast address and mask.
 *
 * \param [in] unit Unit number.
 * \param [in] num Number of ipv6 address/mask pairs
 * \param [out] ip6_addr  Ipv6 address.
 * \param [out] ip6_mask Ipv6 address mask.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_ipv6_reserved_multicast_addr_multi_get(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask);

/*!
 * \brief Translate Profile ID into hardware table index used by API.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Profile ID.
 * \param [out] hw_idx Hardware index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_l3_ip4_options_profile_id2idx(
    int unit,
    int profile_id,
    int *hw_idx);

/*!
 * \brief Translate hardware table index into Profile ID used by API.
 *
 * \param [in] unit Unit Number.
 * \param [in] hw_idx Hardware index.
 * \param [out] profile_id Profile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcm_ltsw_l3_ip4_options_profile_idx2id(
    int unit,
    int hw_idx,
    int *profile_id);

#endif /* BCMI_LTSW_L3_H */
