/*! \file vlan.c
 *
 * VLAN Driver for XGS chips.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm/l3.h>
#include <bcm/port.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/xgs/vlan.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/vlan_int.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_types.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
* Stub definitions
 */


static bcm_pbmp_t SOC_PBMP_STACK_CURRENT = {{ 0 }};

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_VLAN

/*!
 * \brief Vlan block profile structure.
 *
 * This data structure is used to identify different block functionality
 * per vlan.
 */
typedef struct ltsw_vlan_block_s {
    /*! Inverse of mask A ANDed with the port bitmap. */
    bcm_pbmp_t mask_a;
    /*! Inverse of mask B ANDed with the port bitmap. */
    bcm_pbmp_t mask_b;
    /*! Selection for broadcast packets between mask A and mask B. */
    uint8_t    broadcast_mask_sel;
    /*! Selection for unknown unicast packets between mask A and mask B. */
    uint8_t    unknown_unicast_mask_sel;
    /*! Selection for unknown multicast packets between mask A and mask B. */
    uint8_t    unknown_mulitcast_mask_sel;
    /*! Selection for known multicast packets between mask A and mask B. */
    uint8_t    known_mulitcast_mask_sel;
} ltsw_vlan_block_t;

#define VLAN_BLOCK_MASK_SEL_COUNT             0x4

/*! Brief Flood mapping structure. */
static shr_enum_map_t flood_mode_map[] = {
    {(char *)FLOOD_ALLs,  BCM_VLAN_MCAST_FLOOD_ALL},
    {(char *)FLOOD_UMCs,  BCM_VLAN_MCAST_FLOOD_UNKNOWN},
    {(char *)FLOOD_NONEs, BCM_VLAN_MCAST_FLOOD_NONE},
};

/*! Brief block mask mode mapping structure. */
static shr_enum_map_t block_mask_mode_map[] = {
    {(char *)USE_NONEs,             0},
    {(char *)USE_BLOCK_MASK_As,     1},
    {(char *)USE_BLOCK_MASK_Bs,     2},
    {(char *)USE_BLOCK_MASK_A_Bs,   3},
};

/*! Flags for VLAN profile. */
#define LTSW_VLAN_PROFILE_PHB2_ENABLE         0x00000001
#define LTSW_VLAN_PROFILE_PHB2_USE_INNER_TAG  0x00000002
#define LTSW_VLAN_PROFILE_TRUST_DOT1P         0x00000004

/*! Flags for VLAN TAG actions. */
#define VLAN_TAG_ACTION_NONE                  0x0
#define VLAN_TAG_ACTION_ADD                   0x1
#define VLAN_TAG_ACTION_REPLACE               0x2
#define VLAN_TAG_ACTION_DELETE                0x3
#define VLAN_TAG_ACTION_COPY                  0x4

#define VLAN_TAG_ACTION_ENCODE(act) \
    ((act) == bcmVlanActionNone ? VLAN_TAG_ACTION_NONE :       \
     (act) == bcmVlanActionAdd ? VLAN_TAG_ACTION_ADD :         \
     (act) == bcmVlanActionReplace ? VLAN_TAG_ACTION_REPLACE : \
     (act) == bcmVlanActionDelete ? VLAN_TAG_ACTION_DELETE :   \
     VLAN_TAG_ACTION_COPY)

#define VLAN_TAG_ACTION_DECODE(hw_act) \
    ((hw_act) == VLAN_TAG_ACTION_NONE ? bcmVlanActionNone :       \
     (hw_act) == VLAN_TAG_ACTION_ADD ? bcmVlanActionAdd :         \
     (hw_act) == VLAN_TAG_ACTION_REPLACE ? bcmVlanActionReplace : \
     (hw_act) == VLAN_TAG_ACTION_DELETE ? bcmVlanActionDelete :   \
     bcmVlanActionCopy)

#define VLAN_PRI_CFI_ACTION_NONE              0x0
#define VLAN_PRI_CFI_ACTION_ADD               0x1
#define VLAN_PRI_CFI_ACTION_REPLACE           0x2
#define VLAN_PRI_CFI_ACTION_COPY              0x3


#define VLAN_PRI_CFI_ACTION_ENCODE(act) \
    ((act) == bcmVlanActionNone ? VLAN_PRI_CFI_ACTION_NONE :       \
     (act) == bcmVlanActionAdd ? VLAN_PRI_CFI_ACTION_ADD :         \
     (act) == bcmVlanActionReplace ? VLAN_PRI_CFI_ACTION_REPLACE : \
     VLAN_PRI_CFI_ACTION_COPY)

#define VLAN_PRI_CFI_ACTION_DECODE(hw_act) \
    ((hw_act) == VLAN_PRI_CFI_ACTION_NONE ? bcmVlanActionNone :       \
     (hw_act) == VLAN_PRI_CFI_ACTION_ADD ? bcmVlanActionAdd :         \
     (hw_act) == VLAN_PRI_CFI_ACTION_REPLACE ? bcmVlanActionReplace : \
     bcmVlanActionCopy)

/*! Check VLAN ID. */
#define VLAN_ID_CHECK(vid)                     \
    do {                                       \
        if (vid > BCM_VLAN_MAX ||              \
            vid <= BCM_VLAN_MIN) {             \
            SHR_ERR_EXIT(SHR_E_PARAM);  \
        }                                      \
    } while(0)

/*! Flags for all the valid vlan control. */
#define LTSW_XGS_VLAN_CONTROL_VLAN_ALL_VALID_MASK           \
    (BCM_VLAN_CONTROL_VLAN_VRF_MASK |                       \
     BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK |           \
     BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK |                \
     BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK |                \
     BCM_VLAN_CONTROL_VLAN_IP6_MCAST_FLOOD_MODE_MASK |      \
     BCM_VLAN_CONTROL_VLAN_IP4_MCAST_FLOOD_MODE_MASK |      \
     BCM_VLAN_CONTROL_VLAN_L2_MCAST_FLOOD_MODE_MASK |       \
     BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK |                  \
     BCM_VLAN_CONTROL_VLAN_FORWARDING_MODE_MASK |           \
     BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK |           \
     BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK |   \
     BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK |     \
     BCM_VLAN_CONTROL_VLAN_PROTOCOL_PKT_MASK |              \
     BCM_VLAN_CONTROL_VLAN_L3_IF_CLASS_MASK)

/*! Flags for all the valid vlan control flags. */
#define LTSW_XGS_VLAN_CONTROL_VLAN_SUPPORTED_CTRL_FLAGS     \
    (BCM_VLAN_LEARN_DISABLE |                               \
     BCM_VLAN_IGMP_SNOOP_DISABLE |                          \
     BCM_VLAN_IP4_MCAST_L2_DISABLE |                        \
     BCM_VLAN_IP6_MCAST_L2_DISABLE |                        \
     BCM_VLAN_L2_CLASS_ID_ONLY |                            \
     BCM_VLAN_MPLS_DISABLE |                                \
     BCM_VLAN_NON_UCAST_DROP |                              \
     BCM_VLAN_NON_UCAST_TOCPU |                             \
     BCM_VLAN_UNKNOWN_UCAST_DROP |                          \
     BCM_VLAN_UNKNOWN_UCAST_TOCPU)

/*! Flags for all VLAN profile related control flags. */
#define LTSW_XGS_VLAN_CONTROL_VLAN_PROFILE_CTRL_FLAGS       \
    (BCM_VLAN_LEARN_DISABLE |                               \
     BCM_VLAN_NON_UCAST_TOCPU |                             \
     BCM_VLAN_NON_UCAST_DROP |                              \
     BCM_VLAN_UNKNOWN_UCAST_TOCPU |                         \
     BCM_VLAN_UNKNOWN_UCAST_DROP |                          \
     BCM_VLAN_IP6_MCAST_L2_DISABLE |                        \
     BCM_VLAN_IP4_MCAST_L2_DISABLE)

/*! Brief VLAN TAG actions. */
static const char *vlan_tag_action[] = {
    DO_NOT_MODIFYs,
    ADDs,
    REPLACEs,
    DELETEs
};

/*!
 * \brief Vlan profile structure.
 *
 * This data structure is used to identify vlan profile structure.
 */
typedef struct ltsw_vlan_profile_s {
    /*! Outer TPID. */
    uint16_t               outer_tpid;
    /*! VLAN Flags. */
    uint32_t               flags;
    /*! IP6 multicast flood mode. */
    bcm_vlan_mcast_flood_t ip6_mcast_flood_mode;
    /*! IP4 multicast flood mode. */
    bcm_vlan_mcast_flood_t ip4_mcast_flood_mode;
    /*! L2 multicast flood mode. */
    bcm_vlan_mcast_flood_t l2_mcast_flood_mode;
    /*! Vlan block profile. */
    ltsw_vlan_block_t      block;
    /*! Pointer to queue map. */
    int                    qm_ptr;
    /*! Pointer to dot1p map. */
    int                    trust_dot1p_ptr;
    /*! VLAN profile Flags. */
    uint32_t               profile_flags;
    /*! VLAN Protocol Packet control. */
    bcm_vlan_protocol_packet_ctrl_t   protocol_pkt;
} ltsw_vlan_profile_t;

/*!
 * \brief VLAN ing tag action profile structure.
 *
 * This data structure is used to identify vlan ing tag action profile structure.
 */
typedef struct ltsw_vlan_tag_action_profile_s {
    /*! The outer CFI/DE action if incoming packet is single outer-tagged. */
    uint8_t                sot_ocfi_action;
    /*! The outer 802.1p/PCP action if incoming packet is single outer-tagged. */
    uint8_t                sot_opri_action;
    /*! The outer VLAN tag action if incoming packet is single outer-tagged. */
    uint8_t                sot_otag_action;
    /*! The outer VLAN tag action if incoming packet is single outer-tagged and priority-tagged. */
    uint8_t                sot_potag_action;
    /*! The outer CFI/DE action if incoming packet is untagged. */
    uint8_t                ut_ocfi_action;
    /*! The outer 802.1p/PCP action if incoming packet is untagged. */
    uint8_t                ut_opri_action;
    /*! The outer VLAN tag action if incoming packet untagged. */
    uint8_t                ut_otag_action;
} ltsw_vlan_tag_action_profile_t;

/*!
 * \brief VLAN ing egr member ports profile structure.
 *
 * This data structure is used to identify ing egr member ports profile structure.
 */
typedef struct ltsw_vlan_member_ports_profile_s {
    /*! Egress member ports applied on membership check. */
    bcm_pbmp_t egr_member_ports;
} ltsw_vlan_member_ports_profile_t;

/* Reserved entry index in VLAN profile table. */
#define VLAN_PROFILE_ING_EGR_MSHP_CHK_RESV_EMPTY 0

#define VLAN_PROFILE_RESV 0

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Get vlan tag action value from sybmol.
 *
 * \param [in]   unit Unit number.
 * \param [in]   symbol String.
 * \param [out]  value dest_type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
vlan_tag_action_symbol_to_scalar(int unit, const char *symbol, uint64 *value)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < COUNTOF(vlan_tag_action); i++) {
        if (!sal_strcmp(symbol, vlan_tag_action[i])) {
            break;
        }
    }

    if (i >= COUNTOF(vlan_tag_action)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *value = i;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set member ports field into an LT entry container.
 *
 * This function is used to set member ports field into an LT entry container.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  ent_hdl              LT entry handle.
 * \param [in]  fld_name             LT field name.
 * \param [in]  member_ports         Member ports.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_set(int unit,
                      bcmlt_entry_handle_t ent_hdl,
                      const char *fld_name,
                      bcm_pbmp_t member_ports)
{
    int max_port_num = 0;
    uint64_t value = 0;
    uint32_t i = 0;
    SHR_FUNC_ENTER(unit);

    max_port_num = bcmi_ltsw_dev_logic_port_num(unit);
    for (i = 0; i < max_port_num; i++) {
        value = BCM_PBMP_MEMBER(member_ports, i) ? 1 : 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(ent_hdl, fld_name, i, &value, 1));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get member ports field from an LT entry container.
 *
 * This function is used to set member ports fields from an LT entry container.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  ent_hdl              LT entry handle.
 * \param [in]  fld_name             LT field name.
 * \param [out] member_ports         Member ports.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_get(int unit,
                      bcmlt_entry_handle_t ent_hdl,
                      const char *fld_name,
                      bcm_pbmp_t *member_ports)
{
    int max_port_num = 0;
    uint64_t value = 0;
    uint32_t r_elem_cnt = 0, i = 0;
    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(*member_ports);
    max_port_num = bcmi_ltsw_dev_logic_port_num(unit);
    for (i = 0; i < max_port_num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(ent_hdl, fld_name, i,
                                         &value, 1, &r_elem_cnt));
        if(value) {
            BCM_PBMP_PORT_ADD(*member_ports, i);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set default vlan protocol packet control value.
 *
 * This function is used to set default vlan protocol packet control value if none action bit is set.
 *
 * \param [in/out] prot_pkt          Protocol packet actions to be configured.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static void
vlan_protocol_pkt_ctrl_default_set(bcm_vlan_protocol_packet_ctrl_t *prot_pkt)
{
    int defined_pkt_fwd_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE |
                                 BCM_VLAN_PROTO_PKT_DROP_ENABLE |
                                 BCM_VLAN_PROTO_PKT_FLOOD_ENABLE;

    if (!(prot_pkt->mmrp_action & defined_pkt_fwd_action)) {
        prot_pkt->mmrp_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    if (!(prot_pkt->srp_action & defined_pkt_fwd_action)) {
        prot_pkt->srp_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    if (!(prot_pkt->igmp_report_leave_action & defined_pkt_fwd_action)) {
        prot_pkt->igmp_report_leave_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    if (!(prot_pkt->igmp_query_action & defined_pkt_fwd_action)) {
        prot_pkt->igmp_query_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    if (!(prot_pkt->igmp_unknown_msg_action & defined_pkt_fwd_action)) {
        prot_pkt->igmp_unknown_msg_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    if (!(prot_pkt->mld_report_done_action & defined_pkt_fwd_action)) {
        prot_pkt->mld_report_done_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    if (!(prot_pkt->mld_query_action & defined_pkt_fwd_action)) {
        prot_pkt->mld_query_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    if (!(prot_pkt->ip4_mc_router_adv_action & defined_pkt_fwd_action)) {
        prot_pkt->ip4_mc_router_adv_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    if (!(prot_pkt->ip4_rsvd_mc_action & defined_pkt_fwd_action)) {
        prot_pkt->ip4_rsvd_mc_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    if (!(prot_pkt->ip6_rsvd_mc_action & defined_pkt_fwd_action)) {
        prot_pkt->ip6_rsvd_mc_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    if (!(prot_pkt->ip6_mc_router_adv_action & defined_pkt_fwd_action)) {
        prot_pkt->ip6_mc_router_adv_action = BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
    }

    return;
}

/*!
 * \brief Validate the protocol packet control actions
 *
 * \param [in]  unit       Unit Number.
 * \param [in]  pkt        Per VLAN Protocol Packet control.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval SHR_E_PARAM               Wrong param.
 */
static int
vlan_protocol_packet_actions_validate(int unit,
                                          bcm_vlan_protocol_packet_ctrl_t *pkt)
{
    int pkt_action;

    pkt_action = pkt->mmrp_action;
    if (pkt_action) {
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
            (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
            return SHR_E_PARAM;
        }
    }
    pkt_action = pkt->srp_action;
    if (pkt_action) {
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
            (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
            return SHR_E_PARAM;
        }
    }
    pkt_action = pkt->arp_reply_action;
    if ((pkt_action & BCM_VLAN_PROTO_PKT_FORWARD_ENABLE) ||
        (pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE)) {
        return SHR_E_PARAM;
    }
    pkt_action = pkt->arp_request_action;
    if ((pkt_action & BCM_VLAN_PROTO_PKT_FORWARD_ENABLE) ||
        (pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE)) {
        return SHR_E_PARAM;
    }
    pkt_action = pkt->nd_action;
    if ((pkt_action & BCM_VLAN_PROTO_PKT_FORWARD_ENABLE) ||
        (pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE)) {
        return SHR_E_PARAM;
    }
    pkt_action = pkt->dhcp_action;
    if ((pkt_action & BCM_VLAN_PROTO_PKT_FORWARD_ENABLE) ||
        (pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE)) {
        return SHR_E_PARAM;
    }
    pkt_action = pkt->igmp_report_leave_action;
    if (pkt_action) {
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
            (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
            return SHR_E_PARAM;
        }
    }
    pkt_action = pkt->igmp_query_action;
    if (pkt_action) {
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
            (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
            return SHR_E_PARAM;
        }
    }
    pkt_action = pkt->igmp_unknown_msg_action;
    if (pkt_action) {
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
            (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
            return SHR_E_PARAM;
        }
    }
    pkt_action = pkt->mld_report_done_action;
    if (pkt_action) {
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
            (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
            return SHR_E_PARAM;
        }
    }
    pkt_action = pkt->mld_query_action;
    if (pkt_action) {
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
            (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
            return SHR_E_PARAM;
        }
    }
    pkt_action = pkt->ip4_rsvd_mc_action;
    if (pkt_action) {
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
            (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
            return SHR_E_PARAM;
        }
    }
    pkt_action = pkt->ip6_rsvd_mc_action;
    if (pkt_action) {
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
            (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
            return SHR_E_PARAM;
        }
    }
    pkt_action = pkt->ip4_mc_router_adv_action;
    if (pkt_action) {
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
            (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
            return SHR_E_PARAM;
        }
    }
    pkt_action = pkt->ip6_mc_router_adv_action;
    if (pkt_action) {
        if ((pkt_action & BCM_VLAN_PROTO_PKT_FLOOD_ENABLE) &&
            (pkt_action & BCM_VLAN_PROTO_PKT_DROP_ENABLE)) {
            return SHR_E_PARAM;
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Encode VLAN flood mode to pfm value.
 *
 * This function is used to encode VLAN flood mode to pfm value.
 *
 * \param [in] unit                  Unit Number.
 * \param [in] mode                  Vlan flood mode..
 * \param [out]pfm                   PFM value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_mcast_flood_mode_to_pfm(int unit,
                             bcm_vlan_mcast_flood_t mode, char **pfm)
{
    int i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pfm, SHR_E_PARAM);

    for (i = 0; i < COUNTOF(flood_mode_map); i++) {
        if (flood_mode_map[i].val == (int) mode) {
            *pfm = flood_mode_map[i].name;
            break;
        }
    }

    if (i == COUNTOF(flood_mode_map)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Decode pfm value to VLAN flood mode.
 *
 * This function is used to decode pfm value to VLAN flood mode.
 *
 * \param [in] unit                  Unit Number.
 * \param [out]pfm                   PFM value.
 * \param [out]mode                  Vlan flood mode.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_mcast_pfm_to_flood_mode(int unit, const char *pfm,
                             bcm_vlan_mcast_flood_t *mode)
{
    int i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pfm, SHR_E_PARAM);
    SHR_NULL_CHECK(mode, SHR_E_PARAM);

    for (i = 0; i < COUNTOF(flood_mode_map); i++) {
        if (sal_strcmp(flood_mode_map[i].name, pfm) == 0) {
            *mode = (bcm_vlan_mcast_flood_t)flood_mode_map[i].val;
            break;
        }
    }

    if (i == COUNTOF(flood_mode_map)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Encode mask sel to block mask mode.
 *
 * This function is used to encode mask sel to block mask mode.
 *
 * \param [in] unit                  Unit Number.
 * \param [in] sel                   Vlan mask sel.
 * \param [out]mode                  Block mask mode.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_mask_sel_to_block_mask_mode(int unit, uint8_t sel, char **mode)
{
    int i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mode, SHR_E_PARAM);

    for (i = 0; i < COUNTOF(block_mask_mode_map); i++) {
        if (block_mask_mode_map[i].val == (int)sel) {
            *mode = block_mask_mode_map[i].name;
            break;
        }
    }

    if (i == COUNTOF(block_mask_mode_map)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Decode block mask mode to mask sel.
 *
 * This function is used to decode block mask mode to mask sel.
 *
 * \param [in] unit                  Unit Number.
 * \param [in] mode                  Block mask mode.
 * \param [out]sel                   Vlan mask sel.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_block_mask_mode_to_mask_sel(int unit, const char *mode, uint8_t *sel)
{
    int i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mode, SHR_E_PARAM);
    SHR_NULL_CHECK(sel, SHR_E_PARAM);

    for (i = 0; i < COUNTOF(block_mask_mode_map); i++) {
        if (sal_strcmp(block_mask_mode_map[i].name, mode) == 0) {
            *sel = block_mask_mode_map[i].val;
            break;
        }
    }

    if (i == COUNTOF(block_mask_mode_map)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set vlan protocol packet control value.
 *
 * This function is used to set vlan protocol packet control value.
 *
 * \param [in/out] pkt_ctrl_1        Protocol packet actions from API.
 * \param [in/out] pkt_ctrl_2        Interal protocol packet actions.
 * \param [in]     direction         Transfer diection.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static void
vlan_protocol_pkt_ctrl_transfer(bcmi_ltsw_port_profile_port_pkt_ctrl_t *pkt_ctrl_1,
                                bcm_vlan_protocol_packet_ctrl_t *pkt_ctrl_2,
                                int direction)
{
    if (direction) {
        /* Copy SRP packets to CPU port. */
        pkt_ctrl_2->srp_action |=
            pkt_ctrl_1->prot_pkt_ctrl.s.srp_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Action for forwarding of SRP pkts. */
        pkt_ctrl_2->srp_action |=
            (pkt_ctrl_1->prot_pkt_ctrl.s.srp_fwd_action == 2) ?
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
            (pkt_ctrl_1->prot_pkt_ctrl.s.srp_fwd_action == 1) ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        /* Send ND Packets to CPU port. */
        pkt_ctrl_2->nd_action |=
            pkt_ctrl_1->prot_pkt_ctrl.s.nd_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Drop ND Packets. */
        pkt_ctrl_2->nd_action |=
            pkt_ctrl_1->prot_pkt_ctrl.s.nd_drop ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : 0;
        /* Copy MMRP packets to CPU port. */
        pkt_ctrl_2->mmrp_action |=
            pkt_ctrl_1->prot_pkt_ctrl.s.mmrp_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Action for forwarding of MMRP pkts. */
        pkt_ctrl_2->mmrp_action |=
            (pkt_ctrl_1->prot_pkt_ctrl.s.mmrp_fwd_action == 2) ?
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
            (pkt_ctrl_1->prot_pkt_ctrl.s.mmrp_fwd_action == 1) ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        /* Send DHCP Packets to CPU port. */
        pkt_ctrl_2->dhcp_action |=
            pkt_ctrl_1->prot_pkt_ctrl.s.dhcp_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Drop DHCP Packets. */
        pkt_ctrl_2->dhcp_action |=
            pkt_ctrl_1->prot_pkt_ctrl.s.dhcp_drop ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : 0;
        /* Send ARP and RARP Reply Packets to CPU port. */
        pkt_ctrl_2->arp_request_action |=
            pkt_ctrl_1->prot_pkt_ctrl.s.arp_request_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Send ARP and RARP Request Packets to CPU port. */
        pkt_ctrl_2->arp_request_action |=
            pkt_ctrl_1->prot_pkt_ctrl.s.arp_request_drop ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : 0;
        /* Send ARP and RARP Reply Packets to CPU port. */
        pkt_ctrl_2->arp_reply_action |=
            pkt_ctrl_1->prot_pkt_ctrl.s.arp_reply_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Drop ARP and RARP Reply Packets. */
        pkt_ctrl_2->arp_reply_action |=
            pkt_ctrl_1->prot_pkt_ctrl.s.arp_reply_drop ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : 0;
        /* Copy IGMP report leave packets to CPU port. */
        pkt_ctrl_2->igmp_report_leave_action |=
            pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_report_leave_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Action for forwarding of IGMP report leave pkts. */
        pkt_ctrl_2->igmp_report_leave_action |=
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_report_leave_fwd_action == 2) ?
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_report_leave_fwd_action == 1) ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        /* Copy IGMP query packets to CPU port. */
        pkt_ctrl_2->igmp_query_action |=
            pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_query_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Action for forwarding of IGMP query pkts. */
        pkt_ctrl_2->igmp_query_action |=
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_query_fwd_action == 2) ?
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_query_fwd_action == 1) ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        /* Copy IGMP unknown msg packets to CPU port. */
        pkt_ctrl_2->igmp_unknown_msg_action |=
            pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_unknown_msg_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Action for forwarding of IGMP unknown msg pkts. */
        pkt_ctrl_2->igmp_unknown_msg_action |=
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_unknown_msg_fwd_action == 2) ?
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_unknown_msg_fwd_action == 1) ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        /* Copy MLD report done packets to CPU port. */
        pkt_ctrl_2->mld_report_done_action |=
            pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.mld_report_done_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Action for forwarding of MLD report done pkts. */
        pkt_ctrl_2->mld_report_done_action |=
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.mld_report_done_fwd_action == 2) ?
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.mld_report_done_fwd_action == 1) ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        /* Copy MLD query packets to CPU port. */
        pkt_ctrl_2->mld_query_action |=
            pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.mld_query_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Action for forwarding of MLD query pkts. */
        pkt_ctrl_2->mld_query_action |=
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.mld_query_fwd_action == 2) ?
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.mld_query_fwd_action == 1) ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        /* Copy IPv4 reserved multicast packets to CPU port. */
        pkt_ctrl_2->ip4_rsvd_mc_action |=
            pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv4_resvd_mc_pkt_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Action for forwarding of IPv4 reserved multicast pkts. */
        pkt_ctrl_2->ip4_rsvd_mc_action |=
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv4_resvd_mc_pkt_fwd_action == 2) ?
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv4_resvd_mc_pkt_fwd_action == 1) ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        /* Copy IPv4 multicast router packets to CPU port. */
        pkt_ctrl_2->ip4_mc_router_adv_action |=
            pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv4_mc_router_adv_pkt_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Action for forwarding of IPv4 multicast router pkts. */
        pkt_ctrl_2->ip4_mc_router_adv_action |=
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv4_mc_router_adv_pkt_fwd_action == 2) ?
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv4_mc_router_adv_pkt_fwd_action == 1) ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        /* Copy IPv6 reserved multicast packets to CPU port. */
        pkt_ctrl_2->ip6_rsvd_mc_action |=
            pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv6_resvd_mc_pkt_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Action for forwarding of IPv6 reserved multicast pkts. */
        pkt_ctrl_2->ip6_rsvd_mc_action |=
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv6_resvd_mc_pkt_fwd_action == 2) ?
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv6_resvd_mc_pkt_fwd_action == 1) ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        /* Copy IPv6 multicast router packets to CPU port. */
        pkt_ctrl_2->ip6_mc_router_adv_action |=
            pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv6_mc_router_adv_pkt_to_cpu ?
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE : 0;
        /* Action for forwarding of IPv6 multicast router pkts. */
        pkt_ctrl_2->ip6_mc_router_adv_action |=
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv6_mc_router_adv_pkt_fwd_action == 2) ?
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE :
            (pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv6_mc_router_adv_pkt_fwd_action == 1) ?
            BCM_VLAN_PROTO_PKT_DROP_ENABLE : BCM_VLAN_PROTO_PKT_FORWARD_ENABLE;
        /* Use PFM rules for flooding */
        pkt_ctrl_2->flood_action_according_to_pfm =
            pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.pfm_rule_apply;
    } else {
        /* Copy SRP packets to CPU port. */
        pkt_ctrl_1->prot_pkt_ctrl.s.srp_to_cpu  =
            pkt_ctrl_2->srp_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Action for forwarding of SRP pkts. */
        pkt_ctrl_1->prot_pkt_ctrl.s.srp_fwd_action =
            pkt_ctrl_2->srp_action &
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
            (pkt_ctrl_2->srp_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
        /* Send ND Packets to CPU port. */
        pkt_ctrl_1->prot_pkt_ctrl.s.nd_to_cpu =
            pkt_ctrl_2->nd_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Drop ND Packets. */
        pkt_ctrl_1->prot_pkt_ctrl.s.nd_drop =
            pkt_ctrl_2->nd_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0;
        /* Copy MMRP packets to CPU port. */
        pkt_ctrl_1->prot_pkt_ctrl.s.mmrp_to_cpu =
            pkt_ctrl_2->mmrp_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Action for forwarding of MMRP pkts. */
        pkt_ctrl_1->prot_pkt_ctrl.s.mmrp_fwd_action =
            pkt_ctrl_2->mmrp_action &
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
            (pkt_ctrl_2->mmrp_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
        /* Send DHCP Packets to CPU port. */
        pkt_ctrl_1->prot_pkt_ctrl.s.dhcp_to_cpu =
            pkt_ctrl_2->dhcp_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Drop DHCP Packets. */
        pkt_ctrl_1->prot_pkt_ctrl.s.dhcp_drop =
            pkt_ctrl_2->dhcp_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0;
        /* Send ARP and RARP Reply Packets to CPU port. */
        pkt_ctrl_1->prot_pkt_ctrl.s.arp_request_to_cpu =
            pkt_ctrl_2->arp_request_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Send ARP and RARP Request Packets to CPU port. */
        pkt_ctrl_1->prot_pkt_ctrl.s.arp_request_drop =
            pkt_ctrl_2->arp_request_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0;
        /* Send ARP and RARP Reply Packets to CPU port. */
        pkt_ctrl_1->prot_pkt_ctrl.s.arp_reply_to_cpu =
            pkt_ctrl_2->arp_reply_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Drop ARP and RARP Reply Packets. */
        pkt_ctrl_1->prot_pkt_ctrl.s.arp_reply_drop =
            pkt_ctrl_2->arp_reply_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0;
        /* Copy IGMP report leave packets to CPU port. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_report_leave_to_cpu =
            pkt_ctrl_2->igmp_report_leave_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Action for forwarding of IGMP report leave pkts. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_report_leave_fwd_action =
            pkt_ctrl_2->igmp_report_leave_action &
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
            (pkt_ctrl_2->igmp_report_leave_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
        /* Copy IGMP query packets to CPU port. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_query_to_cpu =
            pkt_ctrl_2->igmp_query_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Action for forwarding of IGMP query pkts. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_query_fwd_action =
            pkt_ctrl_2->igmp_query_action &
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
            (pkt_ctrl_2->igmp_query_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
        /* Copy IGMP unknown msg packets to CPU port. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_unknown_msg_to_cpu =
            pkt_ctrl_2->igmp_unknown_msg_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Action for forwarding of IGMP unknown msg pkts. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.igmp_unknown_msg_fwd_action =
            pkt_ctrl_2->igmp_unknown_msg_action &
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
            (pkt_ctrl_2->igmp_unknown_msg_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
        /* Copy MLD report done packets to CPU port. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.mld_report_done_to_cpu =
            pkt_ctrl_2->mld_report_done_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Action for forwarding of MLD report done pkts. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.mld_report_done_fwd_action =
            pkt_ctrl_2->mld_report_done_action &
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
            (pkt_ctrl_2->mld_report_done_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
        /* Copy MLD query packets to CPU port. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.mld_query_to_cpu =
            pkt_ctrl_2->mld_query_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Action for forwarding of MLD query pkts. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.mld_query_fwd_action =
            pkt_ctrl_2->mld_query_action &
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
            (pkt_ctrl_2->mld_query_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
        /* Copy IPv4 reserved multicast packets to CPU port. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv4_resvd_mc_pkt_to_cpu =
            pkt_ctrl_2->ip4_rsvd_mc_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Action for forwarding of IPv4 reserved multicast pkts. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv4_resvd_mc_pkt_fwd_action =
            pkt_ctrl_2->ip4_rsvd_mc_action &
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
            (pkt_ctrl_2->ip4_rsvd_mc_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
        /* Copy IPv4 multicast router packets to CPU port. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv4_mc_router_adv_pkt_to_cpu =
            pkt_ctrl_2->ip4_mc_router_adv_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Action for forwarding of IPv4 multicast router pkts. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv4_mc_router_adv_pkt_fwd_action =
            pkt_ctrl_2->ip4_mc_router_adv_action &
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
            (pkt_ctrl_2->ip4_mc_router_adv_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
        /* Copy IPv6 reserved multicast packets to CPU port. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv6_resvd_mc_pkt_to_cpu =
            pkt_ctrl_2->ip6_rsvd_mc_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Action for forwarding of IPv6 reserved multicast pkts. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv6_resvd_mc_pkt_fwd_action =
            pkt_ctrl_2->ip6_rsvd_mc_action &
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
            (pkt_ctrl_2->ip6_rsvd_mc_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
        /* Copy IPv6 multicast router packets to CPU port. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv6_mc_router_adv_pkt_to_cpu =
            pkt_ctrl_2->ip6_mc_router_adv_action &
            BCM_VLAN_PROTO_PKT_TOCPU_ENABLE ? 1 : 0;
        /* Action for forwarding of IPv6 multicast router pkts. */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.ipv6_mc_router_adv_pkt_fwd_action =
            pkt_ctrl_2->ip6_mc_router_adv_action &
            BCM_VLAN_PROTO_PKT_FLOOD_ENABLE ? 2 :
            (pkt_ctrl_2->ip6_mc_router_adv_action &
            BCM_VLAN_PROTO_PKT_DROP_ENABLE ? 1 : 0);
        /* Use PFM rules for flooding */
        pkt_ctrl_1->igmp_mld_pkt_ctrl.s1.pfm_rule_apply =
            pkt_ctrl_2->flood_action_according_to_pfm;
    }

    return;
}

/*!
 * \brief Set protocol packet actions.
 *
 * This function is used to set protocol packet actions for given protocol
 * control table index.
 *
 * \param [in] unit                  Unit Number.
 * \param [in] prot_pkt              Protocol packet actions to be configured.
 * \param [in] old_prot_pkt_index    Old index to LT PORT_PKT_CTRL.
 * \param [out]prot_pkt_index        Index to LT PORT_PKT_CTRL.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_protocol_pkt_ctrl_set(int unit,
                           bcm_vlan_protocol_packet_ctrl_t *prot_pkt,
                           int old_prot_pkt_index,
                           int *prot_pkt_index)
{
    bcmi_ltsw_port_profile_port_pkt_ctrl_t port_pkt_ctrl;

    SHR_FUNC_ENTER(unit);

    sal_memset(&port_pkt_ctrl, 0,
               sizeof(bcmi_ltsw_port_profile_port_pkt_ctrl_t));

    vlan_protocol_pkt_ctrl_transfer(&port_pkt_ctrl, prot_pkt, FALSE);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_add(unit, &port_pkt_ctrl,
                                                  prot_pkt_index));
    if (old_prot_pkt_index >= 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_profile_port_pkt_ctrl_delete(unit,
                                                         old_prot_pkt_index));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get protocol packet actions.
 *
 * This function is used to get protocol packet actions for given protocol
 * control table index.
 *
 * \param [in] unit                  Unit Number.
 * \param [out]prot_pkt              Protocol packet actions.
 * \param [out]prot_pkt_index        Index to LT PORT_PKT_CTRL.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_protocol_pkt_ctrl_get(int unit,
                           bcm_vlan_protocol_packet_ctrl_t *prot_pkt,
                           int prot_pkt_index)
{
    bcmi_ltsw_port_profile_port_pkt_ctrl_t port_pkt_ctrl;

    SHR_FUNC_ENTER(unit);

    sal_memset(&port_pkt_ctrl, 0,
               sizeof(bcmi_ltsw_port_profile_port_pkt_ctrl_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_get(unit, prot_pkt_index,
                                                  &port_pkt_ctrl));

    vlan_protocol_pkt_ctrl_transfer(&port_pkt_ctrl, prot_pkt, TRUE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into LT VLAN_PROFILE.
 *
 * This function is used to add an entry into LT VLAN_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              Profile entry.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_profile_entry_add(int unit, ltsw_vlan_profile_t *profile, int *profile_idx)
{
    bcmlt_entry_handle_t entry_hdl       = BCMLT_INVALID_HDL;
    int                  rv              = SHR_E_NONE;
    int                  entries_per_set = 1;
    int                  index           = 0;
    uint64_t             value           = 0;
    char                 *pfm            = NULL;
    char                 *mode           = NULL;
    int                  dunit           = 0;
    bcmlt_field_def_t    fld_def;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, VLAN_PROFILEs, BLOCK_MASK_As, &fld_def));

    rv = bcmi_ltsw_profile_index_allocate(unit, BCMI_LTSW_PROFILE_VLAN,
                                          profile, 0,
                                          entries_per_set, profile_idx);
    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        if (*profile_idx == VLAN_PROFILE_RESV) {
            /* Count rollback since ref_count of default entry is always 1. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_decrease(
                                            unit, BCMI_LTSW_PROFILE_VLAN,
                                            entries_per_set, *profile_idx, 1));
        }
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Add an entry into LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_PROFILE_IDs, *profile_idx));

    /* PORT_PKT_CONTROL_ID logical table index */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_protocol_pkt_ctrl_set(unit,
                                    &(profile->protocol_pkt), -1, &index));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_PKT_CONTROL_IDs, index));

    /* VLAN_OUTER_TPID logical table index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_lookup(unit, profile->outer_tpid, &index));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_OUTER_TPID_IDs, index));

    /*
     * Trust outer vlan tag and map 802.1p/CFI bits
     * of the outer vlan tag to PHB
     */
    value = profile->profile_flags & LTSW_VLAN_PROFILE_TRUST_DOT1P ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PHB_ING_L2_OTAG_MAPs, value));

    /* PHB_ING_L2_TAGGED_TO_INT_PRI logical table index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               PHB_ING_L2_TAGGED_TO_INT_PRI_IDs,
                               profile->trust_dot1p_ptr));

    /* MAC source address learning */
    value = (profile->flags & BCM_VLAN_LEARN_DISABLE) ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(entry_hdl, NO_LEARNINGs, value));

    /* IPv6 multicast packet flooding mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_mcast_flood_mode_to_pfm(unit,
                                      profile->ip6_mcast_flood_mode, &pfm));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, L3_IPV6_PFMs, pfm));

    /* IPv4 multicast packet flooding mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_mcast_flood_mode_to_pfm(unit,
                                      profile->ip4_mcast_flood_mode, &pfm));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, L3_IPV4_PFMs, pfm));

    /* L2 multicast packet flooding mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_mcast_flood_mode_to_pfm(unit,
                                      profile->l2_mcast_flood_mode, &pfm));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, L2_PFMs, pfm));

    /* Copy L2 non-unicast packets to CPU */
    value = (profile->flags & BCM_VLAN_NON_UCAST_TOCPU) ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, L2_NON_UCAST_TOCPUs, value));

    /* Drop L2 non-unicast packets */
    value = (profile->flags & BCM_VLAN_NON_UCAST_DROP) ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, L2_NON_UCAST_DROPs, value));

    /* Copy L2 miss lookup packets to CPU */
    value = (profile->flags & BCM_VLAN_UNKNOWN_UCAST_TOCPU) ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, L2_MISS_TOCPUs, value));

    /* Drop L2 miss lookup packets */
    value = (profile->flags & BCM_VLAN_UNKNOWN_UCAST_DROP) ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, L2_MISS_DROPs, value));

    /* L2-only forwarding for IPv6 multicast packets */
    value = (profile->flags & BCM_VLAN_IP6_MCAST_L2_DISABLE) ? 0 : 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, IPV6_MC_L2_FWDs, value));

    /* L2-only forwarding for IPv4 multicast packets */
    value = (profile->flags & BCM_VLAN_IP4_MCAST_L2_DISABLE) ? 0 : 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, IPV4_MC_L2_FWDs, value));

    /* Inverse of mask A ANDed with the port bitmap */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, entry_hdl,
                               BLOCK_MASK_As, profile->block.mask_a));

    /* Inverse of mask B ANDed with the port bitmap */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, entry_hdl,
                               BLOCK_MASK_Bs, profile->block.mask_b));

    /* Selection for unknown unicast packets between mask A and mask B */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_mask_sel_to_block_mask_mode(unit,
                            profile->block.unknown_unicast_mask_sel, &mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, UNKNOWN_UC_MASK_MODEs, mode));

    /* Selection for unknown multicast packets between mask A and mask B */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_mask_sel_to_block_mask_mode(unit,
                            profile->block.unknown_mulitcast_mask_sel, &mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, UNKNOWN_MC_MASK_MODEs, mode));

    /* Selection for known multicast packets between mask A and mask B */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_mask_sel_to_block_mask_mode(unit,
                            profile->block.known_mulitcast_mask_sel, &mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, MC_MASK_MODEs, mode));

    /* Selection for broadcast packets between mask A and mask B */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_mask_sel_to_block_mask_mode(unit,
                            profile->block.broadcast_mask_sel, &mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, BC_MASK_MODEs, mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a entry from LT VLAN_PROFILE.
 *
 * This function is used to get a entry from LT VLAN_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile              Profile entry.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_profile_entry_get(int unit, ltsw_vlan_profile_t *profile, int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t             value     = 0;
    const char           *pfm      = NULL;
    const char           *mode     = NULL;
    int                  dunit     = 0;
    bcmlt_field_def_t    fld_def;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, VLAN_PROFILEs, BLOCK_MASK_As, &fld_def));

    /* Find an entry from LT VLAN profile */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* PORT_PKT_CONTROL_ID logical table index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, PORT_PKT_CONTROL_IDs, &value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_protocol_pkt_ctrl_get(unit, &(profile->protocol_pkt), value));

    /* VLAN_OUTER_TPID logical table index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, VLAN_OUTER_TPID_IDs, &value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_get(unit, value, &profile->outer_tpid));

    /*
     * Trust outer vlan tag and map 802.1p/CFI bits
     * of the outer vlan tag to PHB
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, PHB_ING_L2_OTAG_MAPs, &value));
    if (value) {
        profile->profile_flags |= LTSW_VLAN_PROFILE_TRUST_DOT1P;
    }

    /* PHB_ING_L2_TAGGED_TO_INT_PRI logical table index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl,
                               PHB_ING_L2_TAGGED_TO_INT_PRI_IDs, &value));
    profile->trust_dot1p_ptr = (int)value;

    /* MAC source address learning */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, NO_LEARNINGs, &value));
    if (value) {
        profile->flags |= BCM_VLAN_LEARN_DISABLE;
    }

    /* IPv6 multicast packet flooding mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, L3_IPV6_PFMs, &pfm));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_mcast_pfm_to_flood_mode(unit,
                                      pfm, &(profile->ip6_mcast_flood_mode)));

    /* IPv4 multicast packet flooding mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, L3_IPV4_PFMs, &pfm));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_mcast_pfm_to_flood_mode(unit,
                                      pfm, &(profile->ip4_mcast_flood_mode)));

    /* L2 multicast packet flooding mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, L2_PFMs, &pfm));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_mcast_pfm_to_flood_mode(unit,
                                      pfm, &(profile->l2_mcast_flood_mode)));

    /* Copy L2 non-unicast packets to CPU */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, L2_NON_UCAST_TOCPUs, &value));
    if (value) {
        profile->flags |=  BCM_VLAN_NON_UCAST_TOCPU;
    }

    /* Drop L2 non-unicast packets */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, L2_NON_UCAST_DROPs, &value));
    if (value) {
        profile->flags |=  BCM_VLAN_NON_UCAST_DROP;
    }

    /* Copy L2 miss lookup packets to CPU */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, L2_MISS_TOCPUs, &value));
    if (value) {
        profile->flags |=  BCM_VLAN_UNKNOWN_UCAST_TOCPU;
    }

    /* Drop L2 miss lookup packets */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, L2_MISS_DROPs, &value));
    if (value) {
        profile->flags |=  BCM_VLAN_UNKNOWN_UCAST_DROP;
    }

    /* L2-only forwarding for IPv6 multicast packets */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, IPV6_MC_L2_FWDs, &value));
    if (!value) {
        profile->flags |=  BCM_VLAN_IP6_MCAST_L2_DISABLE;
    }

    /* L2-only forwarding for IPv4 multicast packets */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, IPV4_MC_L2_FWDs, &value));
    if (!value) {
        profile->flags |=  BCM_VLAN_IP4_MCAST_L2_DISABLE;
    }

    /* Inverse of mask A ANDed with the port bitmap */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_get(unit, entry_hdl,
                               BLOCK_MASK_As, &(profile->block.mask_a)));

    /* Inverse of mask B ANDed with the port bitmap */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_get(unit, entry_hdl,
                               BLOCK_MASK_Bs, &(profile->block.mask_b)));

    /* Selection for unknown unicast packets between mask A and mask B */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, UNKNOWN_UC_MASK_MODEs, &mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_mask_mode_to_mask_sel(unit, mode,
                            &profile->block.unknown_unicast_mask_sel));

    /* Selection for unknown multicast packets between mask A and mask B */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, UNKNOWN_MC_MASK_MODEs, &mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_mask_mode_to_mask_sel(unit, mode,
                            &profile->block.unknown_mulitcast_mask_sel));

    /* Selection for known multicast packets between mask A and mask B */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, MC_MASK_MODEs, &mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_mask_mode_to_mask_sel(unit, mode,
                            &profile->block.known_mulitcast_mask_sel));

    /* Selection for broadcast packets between mask A and mask B */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, BC_MASK_MODEs, &mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_mask_mode_to_mask_sel(unit, mode,
                            &profile->block.broadcast_mask_sel));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a entry from LT VLAN_PROFILE.
 *
 * This function is used to delete a entry from LT VLAN_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int                  rv        = SHR_E_NONE;
    uint64_t             value     = 0;
    int                  dunit     = 0;
    SHR_FUNC_ENTER(unit);

    /* No ref_count decrease for default entry. */
    if (profile_idx == VLAN_PROFILE_RESV) {
        SHR_EXIT();
    }

    rv = bcmi_ltsw_profile_index_free(unit, BCMI_LTSW_PROFILE_VLAN, profile_idx);
    if (rv == SHR_E_BUSY) {
        /* Profile index is still used by others and return directly */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, PORT_PKT_CONTROL_IDs, &value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_profile_port_pkt_ctrl_delete(unit, value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash signature for LT VLAN_PROFILE.
 *
 * This function is used to calculate hash signature for LT VLAN_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              VLAN profile.
 * \param [in]  entries_per_set      Number of entries in the VLAN profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_profile_hash_cb(int unit, void *entries,
                     int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(ltsw_vlan_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set of LT VLAN_PROFILE.
 *
 * This function is used to compare profile set of LT VLAN_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              VLAN profile.
 * \param [in]  entries_per_set      Number of entries in the VLAN profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_profile_cmp_cb(int unit, void *entries,
                    int entries_per_set, int index, int *cmp)
{
    ltsw_vlan_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&profile, 0, sizeof(ltsw_vlan_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_get(unit, &profile, index));

    *cmp = sal_memcmp(entries, (void *)&profile, sizeof(ltsw_vlan_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register VLAN profile.
 *
 * This function is used to register VLAN profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_PROFILEs, VLAN_PROFILE_IDs,
                                       &idx_min, &idx_max));

    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_profile_hash_cb, vlan_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count of VLAN profile.
 *
 * This function is used to recover hash and reference count of VLAN profile.
 *
 * \param [in] unit                  Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN;
    ltsw_vlan_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(ltsw_vlan_profile_t));

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, index, &ref_count));

    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_profile_entry_get(unit, &profile, index));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit, profile_hdl, &profile, entries_per_set, index));
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, index, 1));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the LT VLAN_PROFILE.
 *
 * This function is used to initialize the LT VLAN_PROFILE
 * (PT - VLAN_PROFILE_TABm and VLAN_PROFILE_2m).
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, VLAN_PROFILEs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_PROFILEs, VLAN_PROFILE_IDs,
                                       &idx_min, &idx_max));

    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_profile_hash_cb, vlan_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the LT VLAN_PROFILE.
 *
 * This function is used to de-initialize the LT VLAN_PROFILE
 * (PT - VLAN_PROFILE_TABm and VLAN_PROFILE_2m).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vd                   Default VLAN information.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_profile_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, BCMI_LTSW_PROFILE_VLAN),
         SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * This function is used to add an entry into VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_profile_entry_add(int unit,
                                    int profile_idx,
                                    ltsw_vlan_member_ports_profile_t *profile)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Add an entry into VLAN_EGR_MEMBER_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_ING_EGR_MEMBER_PORTS_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs, profile_idx));

    /* Set egress member ports applied on membership check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, ent_hdl, EGR_MEMBER_PORTSs,
                               profile->egr_member_ports));

    /* Insert VLAN_EGR_MEMBER_PROFILE entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * This function is used to add an entry into VLAN_EGR_MEMBER_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              Profile entry.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_profile_add(int unit,
                              ltsw_vlan_member_ports_profile_t *profile,
                              int *profile_idx)
{
    int entries_per_set = 1;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Try to get a free profile index. */
    rv = bcmi_ltsw_profile_index_allocate(unit,
                                          BCMI_LTSW_PROFILE_VLAN_ING_EGR_MEMBER_PORTS_PROFILE,
                                          profile, 0,
                                          entries_per_set, profile_idx);

    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        if (*profile_idx == VLAN_PROFILE_ING_EGR_MSHP_CHK_RESV_EMPTY) {
            /* Count rollback since ref_count of default entry is always 1. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_decrease(
                    unit, BCMI_LTSW_PROFILE_VLAN_ING_EGR_MEMBER_PORTS_PROFILE,
                    entries_per_set, *profile_idx, 1));
        }
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Add the information into VLAN_EGR_MEMBER_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_entry_add(unit, *profile_idx, profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from LT VLAN_ING_EGR_MEMBER_PORTS_PROFILE.
 *
 * This function is used to get an entry from LT VLAN_ING_EGR_MEMBER_PORTS_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile              Profile entry.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_profile_entry_get(int unit,
                                    ltsw_vlan_member_ports_profile_t *profile,
                                    int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    /* Find an entry from LT VLAN_ING_EGR_MEMBER_PORTS_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_ING_EGR_MEMBER_PORTS_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_get(unit, entry_hdl, EGR_MEMBER_PORTSs,
                               &(profile->egr_member_ports)));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * This function is used to delete an entry from VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_ING_EGR_MEMBER_PORTS_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from VLAN_ING_EGR_MEMBER_PORTS_PROFILE.
 *
 * This function is used to delete an entry from VLAN_ING_EGR_MEMBER_PORTS_PROFILE profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_profile_delete(int unit, int profile_idx)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* No ref_count decrease for default entry. */
    if (profile_idx == VLAN_PROFILE_ING_EGR_MSHP_CHK_RESV_EMPTY) {
        SHR_EXIT();
    }

    rv = bcmi_ltsw_profile_index_free(unit,
                                      BCMI_LTSW_PROFILE_VLAN_ING_EGR_MEMBER_PORTS_PROFILE,
                                      profile_idx);

    /* Return directly if profile index is still used by others. */
    if (rv == SHR_E_BUSY) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Delete the information from VLAN_ING_EGR_MEMBER_PORTS_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_entry_delete(unit, profile_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry of empty pbmp into VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * This function is used to add an entry into VLAN_EGR_MEMBER_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_empty_member_ports_profile_add(int unit)
{
    ltsw_vlan_member_ports_profile_t profile;
    int profile_idx = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * Add VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID.
     */
    BCM_PBMP_CLEAR(profile.egr_member_ports);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_add(unit,
                                       &profile,
                                       &profile_idx));

    if (profile_idx != VLAN_PROFILE_ING_EGR_MSHP_CHK_RESV_EMPTY) {
        (void)vlan_member_ports_profile_entry_delete(unit, profile_idx);
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash signature for LT VLAN_ING_EGR_MEMBER_PORTS_PROFILE.
 *
 * This function is used to calculate hash signature for LT VLAN_ING_EGR_MEMBER_PORTS_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              VLAN profile.
 * \param [in]  entries_per_set      Number of entries in the VLAN profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_profile_hash_cb(int unit, void *entries,
                                  int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(ltsw_vlan_member_ports_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set of LT VLAN_ING_EGR_MEMBER_PORTS_PROFILE.
 *
 * This function is used to compare profile set of LT VLAN_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              VLAN profile.
 * \param [in]  entries_per_set      Number of entries in the VLAN profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_profile_cmp_cb(int unit, void *entries,
                                 int entries_per_set, int index, int *cmp)
{
    ltsw_vlan_member_ports_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&profile, 0, sizeof(ltsw_vlan_member_ports_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_entry_get(unit, &profile, index));

    *cmp = sal_memcmp(entries, (void *)&profile,
                      sizeof(ltsw_vlan_member_ports_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register ingress egress membership check profile.
 *
 * This function is used to register ingress egress membership check profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_ING_EGR_MEMBER_PORTS_PROFILE;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_ING_EGR_MEMBER_PORTS_PROFILEs,
                                       VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                                       &idx_min, &idx_max));

    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_member_ports_profile_hash_cb,
                                    vlan_member_ports_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count of ingress egress membership check profile.
 *
 * This function is used to recover hash and reference count of ingress egress
 * membership check profile.
 *
 * \param [in] unit                  Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_ING_EGR_MEMBER_PORTS_PROFILE;
    ltsw_vlan_member_ports_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(ltsw_vlan_member_ports_profile_t));

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, index, &ref_count));

    if (ref_count == 0) {

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_member_ports_profile_entry_get(unit, &profile, index));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit, profile_hdl, &profile, entries_per_set, index));
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, index, 1));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the LT VLAN_ING_EGR_MEMBER_PORTS_PROFILE.
 *
 * This function is used to initialize the LT VLAN_ING_EGR_MEMBER_PORTS_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_ING_EGR_MEMBER_PORTS_PROFILE;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, VLAN_ING_EGR_MEMBER_PORTS_PROFILEs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_ING_EGR_MEMBER_PORTS_PROFILEs,
                                       VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                                       &idx_min, &idx_max));

    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_member_ports_profile_hash_cb,
                                    vlan_member_ports_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the LT VLAN_ING_EGR_MEMBER_PORTS_PROFILE.
 *
 * This function is used to initialize the LT VLAN_ING_EGR_MEMBER_PORTS_PROFILE
 * (PT - EGR_VLAN_TAG_ACTION_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_profile_detach(int unit)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, BCMI_LTSW_PROFILE_VLAN_ING_EGR_MEMBER_PORTS_PROFILE),
         SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * This function is used to add an entry into LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * \param [in] unit                 Unit Number.
 * \param [in] profile_idx          Profile entry index.
 * \param [in] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_profile_entry_add(int unit,
                                      int profile_idx,
                                      ltsw_vlan_tag_action_profile_t *profile)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t             value     = 0;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    /* Find an entry from LT VLAN_ING_TAG_ACTION_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_ING_TAG_ACTION_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_ING_TAG_ACTION_PROFILE_IDs, profile_idx));

    value = profile->sot_ocfi_action;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, SOT_OCFIs, vlan_tag_action[value]));

    value = profile->sot_opri_action;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, SOT_OPRIs, vlan_tag_action[value]));

    value = profile->sot_otag_action;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, SOT_OTAGs, vlan_tag_action[value]));

    value = profile->sot_potag_action;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, SOT_POTAGs, vlan_tag_action[value]));

    value = profile->ut_ocfi_action;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, UT_OCFIs, vlan_tag_action[value]));

    value = profile->ut_opri_action;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, UT_OPRIs, vlan_tag_action[value]));

    value = profile->ut_otag_action;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, UT_OTAGs, vlan_tag_action[value]));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a profile into LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * This function is used to add an entry into LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * \param [in] unit                 Unit Number.
 * \param [in] profile_idx          Profile entry index.
 * \param [in] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_profile_add(int unit,
                                ltsw_vlan_tag_action_profile_t *profile,
                                int *profile_idx)
{
    int entries_per_set = 1;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Try to get a free profile index. */
    rv = bcmi_ltsw_profile_index_allocate(unit,
                                          BCMI_LTSW_PROFILE_VLAN_ING_TAG_ACTION,
                                          profile, 0,
                                          entries_per_set, profile_idx);

    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Add the information into VLAN_ING_TAG_ACTION_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_tag_action_profile_entry_add(unit, *profile_idx, profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from VLAN_ING_TAG_ACTION_PROFILE table.
 *
 * This function is used to delete an entry from VLAN_ING_TAG_ACTION_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_ING_TAG_ACTION_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_ING_TAG_ACTION_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from VLAN_ING_TAG_ACTION_PROFILE.
 *
 * This function is used to delete an entry from VLAN_ING_TAG_ACTION_PROFILE profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_profile_delete(int unit, int profile_idx)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit,
                                      BCMI_LTSW_PROFILE_VLAN_ING_TAG_ACTION,
                                      profile_idx);

    /* Return directly if profile index is still used by others. */
    if (rv == SHR_E_BUSY) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Delete the information from VLAN_ING_TAG_ACTION_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_tag_action_profile_entry_delete(unit, profile_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a default profile into LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * This function is used to add an entry into LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * \param [in] unit                 Unit Number.
 * \param [in] profile_idx          Profile entry index.
 * \param [in] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_default_profile_add(int unit,
                                        int *profile_idx)
{
    ltsw_vlan_tag_action_profile_t profile;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(ltsw_vlan_tag_action_profile_t));
    profile.ut_otag_action   = VLAN_TAG_ACTION_ADD;
    profile.ut_opri_action   = VLAN_TAG_ACTION_ADD;
    profile.ut_ocfi_action   = VLAN_TAG_ACTION_ADD;
    profile.sot_potag_action = VLAN_TAG_ACTION_REPLACE;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_tag_action_profile_add(unit,
                                         &profile,
                                         profile_idx));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a entry from LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * This function is used to get a entry from LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile              Profile entry.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_profile_entry_get(int unit,
                                      ltsw_vlan_tag_action_profile_t *profile,
                                      int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *sym_value;
    int dunit = 0;
    uint64_t sca_value;

    SHR_FUNC_ENTER(unit);

    /* Find an entry from LT VLAN_ING_TAG_ACTION_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_ING_TAG_ACTION_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_ING_TAG_ACTION_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, SOT_OCFIs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->sot_ocfi_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, SOT_OPRIs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->sot_opri_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, SOT_OTAGs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->sot_otag_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, SOT_POTAGs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->sot_potag_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, UT_OCFIs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->ut_ocfi_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, UT_OPRIs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->ut_opri_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, UT_OTAGs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->ut_otag_action = (uint8_t) sca_value;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash signature for LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * This function is used to calculate hash signature for LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              VLAN profile.
 * \param [in]  entries_per_set      Number of entries in the VLAN profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_profile_hash_cb(int unit, void *entries,
                                    int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(ltsw_vlan_tag_action_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set of LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * This function is used to compare profile set of LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              VLAN profile.
 * \param [in]  entries_per_set      Number of entries in the VLAN profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_profile_cmp_cb(int unit, void *entries,
                                   int entries_per_set, int index, int *cmp)
{
    ltsw_vlan_tag_action_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&profile, 0, sizeof(ltsw_vlan_tag_action_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_tag_action_profile_entry_get(unit, &profile, index));

    *cmp = sal_memcmp(entries, (void *)&profile,
                      sizeof(ltsw_vlan_tag_action_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register VLAN_ING_TAG_ACTION_PROFILE profile.
 *
 * This function is used to register VLAN_ING_TAG_ACTION_PROFILE profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_ING_TAG_ACTION;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_ING_TAG_ACTION_PROFILEs,
                                       VLAN_ING_TAG_ACTION_PROFILE_IDs,
                                       &idx_min, &idx_max));

    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_ing_tag_action_profile_hash_cb,
                                    vlan_ing_tag_action_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register VLAN_ING_TAG_ACTION_PROFILE profile.
 *
 * This function is used to register VLAN_ING_TAG_ACTION_PROFILE profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_ING_TAG_ACTION;
    ltsw_vlan_tag_action_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(ltsw_vlan_tag_action_profile_t));

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, index, &ref_count));

    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_tag_action_profile_entry_get(unit, &profile, index));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit, profile_hdl, &profile, entries_per_set, index));
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, index, 1));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initialize the LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * This function is used to initialize the LT VLAN_ING_TAG_ACTION_PROFILE
 * (PT - ING_VLAN_TAG_ACTION_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vd                   Default VLAN information.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_profile_init(int unit)

{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_ING_TAG_ACTION;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, VLAN_ING_TAG_ACTION_PROFILEs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_ING_TAG_ACTION_PROFILEs,
                                       VLAN_ING_TAG_ACTION_PROFILE_IDs,
                                       &idx_min, &idx_max));

    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_ing_tag_action_profile_hash_cb,
                                    vlan_ing_tag_action_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * This function is used to add an entry into LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * \param [in] unit                 Unit Number.
 * \param [in] profile_idx          Profile entry index.
 * \param [in] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_profile_entry_add(int unit,
                                      int profile_idx,
                                      ltsw_vlan_tag_action_profile_t *profile)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t             value     = 0;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    /* Add an entry into LT VLAN_EGR_TAG_ACTION_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_EGR_TAG_ACTION_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_EGR_TAG_ACTION_PROFILE_IDs, profile_idx));

    value = profile->sot_ocfi_action;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, SOT_OCFIs, vlan_tag_action[value]));

    value = profile->sot_opri_action;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, SOT_OPRIs, vlan_tag_action[value]));

    value = profile->sot_otag_action;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, SOT_OTAGs, vlan_tag_action[value]));

    value = profile->sot_potag_action;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, SOT_POTAGs, vlan_tag_action[value]));

    value = profile->ut_ocfi_action;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, UT_OCFIs, vlan_tag_action[value]));

    value = profile->ut_opri_action;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, UT_OPRIs, vlan_tag_action[value]));

    value = profile->ut_otag_action;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, UT_OTAGs, vlan_tag_action[value]));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a profile into LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * This function is used to add an entry into LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * \param [in] unit                 Unit Number.
 * \param [in] profile_idx          Profile entry index.
 * \param [in] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_profile_add(int unit,
                                ltsw_vlan_tag_action_profile_t *profile,
                                int *profile_idx)
{
    int entries_per_set = 1;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Try to get a free profile index. */
    rv = bcmi_ltsw_profile_index_allocate(unit,
                                          BCMI_LTSW_PROFILE_VLAN_EGR_TAG_ACTION,
                                          profile, 0,
                                          entries_per_set, profile_idx);

    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Add the information into VLAN_EGR_TAG_ACTION_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_tag_action_profile_entry_add(unit, *profile_idx, profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a default profile into LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * This function is used to add an entry into LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * \param [in] unit                 Unit Number.
 * \param [in] profile_idx          Profile entry index.
 * \param [in] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_default_profile_add(int unit,
                                        int *profile_idx)
{
    ltsw_vlan_tag_action_profile_t profile;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(ltsw_vlan_tag_action_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_tag_action_profile_add(unit,
                                         &profile,
                                         profile_idx));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a entry from LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * This function is used to get a entry from LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile              Profile entry.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_profile_entry_get(int unit,
                                      ltsw_vlan_tag_action_profile_t *profile,
                                      int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t sca_value = 0;
    int dunit = 0;
    const char *sym_value;

    SHR_FUNC_ENTER(unit);

    /* Find an entry from LT VLAN profile */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_EGR_TAG_ACTION_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_EGR_TAG_ACTION_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, SOT_OCFIs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->sot_ocfi_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, SOT_OPRIs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->sot_opri_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, SOT_OTAGs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->sot_otag_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, SOT_POTAGs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->sot_potag_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, UT_OCFIs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->ut_ocfi_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, UT_OPRIs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->ut_opri_action = (uint8_t) sca_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, UT_OTAGs, &sym_value));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_symbol_to_scalar(unit, sym_value, &sca_value));
    profile->ut_otag_action = (uint8_t) sca_value;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from VLAN_EGR_TAG_ACTION_PROFILE table.
 *
 * This function is used to delete an entry from VLAN_EGR_TAG_ACTION_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_EGR_TAG_ACTION_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_EGR_TAG_ACTION_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * This function is used to delete an entry from VLAN_EGR_TAG_ACTION_PROFILE profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_profile_delete(int unit, int profile_idx)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit,
                                      BCMI_LTSW_PROFILE_VLAN_EGR_TAG_ACTION,
                                      profile_idx);

    /* Return directly if profile index is still used by others. */
    if (rv == SHR_E_BUSY) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Delete the information from VLAN_EGR_TAG_ACTION_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_tag_action_profile_entry_delete(unit, profile_idx));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Calculate hash signature for LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * This function is used to calculate hash signature for LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              VLAN profile.
 * \param [in]  entries_per_set      Number of entries in the VLAN profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_profile_hash_cb(int unit, void *entries,
                                    int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(ltsw_vlan_tag_action_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set of LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * This function is used to compare profile set of LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              VLAN profile.
 * \param [in]  entries_per_set      Number of entries in the VLAN profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_profile_cmp_cb(int unit, void *entries,
                                   int entries_per_set, int index, int *cmp)
{
    ltsw_vlan_tag_action_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&profile, 0, sizeof(ltsw_vlan_tag_action_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_tag_action_profile_entry_get(unit, &profile, index));

    *cmp = sal_memcmp(entries, (void *)&profile, sizeof(ltsw_vlan_tag_action_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register VLAN_EGR_TAG_ACTION_PROFILE profile.
 *
 * This function is used to register VLAN_EGR_TAG_ACTION_PROFILE profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_EGR_TAG_ACTION;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_EGR_TAG_ACTION_PROFILEs,
                                       VLAN_EGR_TAG_ACTION_PROFILE_IDs,
                                       &idx_min, &idx_max));

    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_egr_tag_action_profile_hash_cb,
                                    vlan_egr_tag_action_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count of VLAN_EGR_TAG_ACTION_PROFILE profile.
 *
 * This function is used to recover hash and reference count of
 * VLAN_EGR_TAG_ACTION_PROFILE profile.
 *
 * \param [in] unit                  Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_EGR_TAG_ACTION;
    ltsw_vlan_tag_action_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(ltsw_vlan_tag_action_profile_t));

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, index, &ref_count));

    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_tag_action_profile_entry_get(unit, &profile, index));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit, profile_hdl, &profile, entries_per_set, index));
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, index, 1));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * This function is used to initialize the LT VLAN_EGR_TAG_ACTION_PROFILE
 * (PT - EGR_VLAN_TAG_ACTION_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vd                   Default VLAN information.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_profile_init(int unit)

{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_EGR_TAG_ACTION;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, VLAN_EGR_TAG_ACTION_PROFILEs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_EGR_TAG_ACTION_PROFILEs, VLAN_EGR_TAG_ACTION_PROFILE_IDs,
                                       &idx_min, &idx_max));

    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_egr_tag_action_profile_hash_cb,
                                    vlan_egr_tag_action_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a default entry into LT VLAN_PROFILE.
 *
 * This function is used to add a default entry into LT VLAN_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_profile_default_entry_add(int unit, int *profile_idx)
{
    ltsw_vlan_profile_t     profile;
    bcm_vlan_mcast_flood_t  mode;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(ltsw_vlan_profile_t));

    /* Default multicast flood mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_flood_default_get(unit, &mode));

    profile.ip6_mcast_flood_mode = mode;
    profile.ip4_mcast_flood_mode = mode;
    profile.l2_mcast_flood_mode  = mode;

    /* Default outer tpid value. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_default_get(unit, &profile.outer_tpid));

    /* Default VLAN Protocol Packet control */
    vlan_protocol_pkt_ctrl_default_set(&(profile.protocol_pkt));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_add(unit, &profile, profile_idx));

    if (*profile_idx != VLAN_PROFILE_RESV) {
        (void)vlan_profile_entry_delete(unit, *profile_idx);
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief De-initialize the LT VLAN_ING_TAG_ACTION_PROFILE.
 *
 * This function is used to de-initialize the LT VLAN_ING_TAG_ACTION_PROFILE
 * (PT - ING_VLAN_TAG_ACTION_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_tag_action_profile_detach(int unit)

{
    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_profile_register_check(unit,
                                         BCMI_LTSW_PROFILE_VLAN_ING_TAG_ACTION)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit,
                                          BCMI_LTSW_PROFILE_VLAN_ING_TAG_ACTION));

    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the LT VLAN_EGR_TAG_ACTION_PROFILE.
 *
 * This function is used to initialize the LT VLAN_EGR_TAG_ACTION_PROFILE
 * (PT - EGR_VLAN_TAG_ACTION_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_tag_action_profile_detach(int unit)

{
    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_profile_register_check(unit,
                                         BCMI_LTSW_PROFILE_VLAN_EGR_TAG_ACTION)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_unregister(unit,
                                          BCMI_LTSW_PROFILE_VLAN_EGR_TAG_ACTION));

    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Added default tag action
 *
 * This function is used to add default action into ING and EGR tag action profile
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_tag_action_default_set(int unit)
{
    int profile = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_tag_action_default_profile_add(unit, &profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_tag_action_default_profile_add(unit, &profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the VLAN tables.
 *
 * This function is used to de-initialize the VLAN tables.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_table_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* VLAN Tables will be cleared during init. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_tag_action_profile_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_tag_action_profile_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an outer TPID entry from LT VLAN_OUTER_TPID.
 *
 * This function is used to get a outer TPID entry from LT VLAN_OUTER_TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                Entry index.
 * \param [out]  tpid                Outer TPID value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_otpid_entry_get(int unit, int index, uint16_t *tpid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int                  rv        = SHR_E_NONE;
    uint64_t             value     = 0;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_OUTER_TPIDs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_OUTER_TPID_IDs, index));

    rv = bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, ING_TPIDs, &value));

    *tpid = (uint16_t)value;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an outer TPID entry into LT VLAN_OUTER_TPID.
 *
 * This function is used to get a outer TPID entry into LT VLAN_OUTER_TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                Entry index.
 * \param [in]  tpid                 Outer TPID value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_otpid_entry_add(int unit, int index, uint16_t tpid)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int                  dunit     = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_OUTER_TPIDs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_OUTER_TPID_IDs, index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, ING_TPIDs, (uint64_t)tpid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, EGR_TPIDs, (uint64_t)tpid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the VLAN outer TPID tables.
 *
 * This function is used to initialize the VLAN outer TPID tables.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_otpid_table_init(int unit)
{
    uint16_t   otpid_def[] = {0x8100, 0x9100, 0x88a8, 0xaa8a};
    int        tpid_num    = 0;
    int        index       = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_otpid_max_num(unit, &tpid_num));

    /* Update VLAN TPID information */
    for (index = 0; index < tpid_num; index++) {
        if (bcmi_warmboot_get(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_otpid_entry_get(unit, index, &otpid_def[index]));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_otpid_entry_add(unit, index, otpid_def[index]));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_otpid_info_tpid_set(unit, index,
                                                otpid_def[index]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set default VLAN during initialization.
 *
 * This function is used to set default VLAN during initialization.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] vd                   Default VLAN information.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_default_vlan_populate(int unit, bcm_vlan_data_t *vd)
{

    bcm_pbmp_t  pbmp_all;
    bcm_pbmp_t  pbmp_lb;
    bcm_pbmp_t  pbmp_cmic;
    bcm_pbmp_t  pbmp_e_all;
    bcm_pbmp_t  pbmp_local;
    int port_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp_all));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_LB, &pbmp_lb));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_CPU, &pbmp_cmic));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp_e_all));

    /* Set default VLAN  */
    BCM_PBMP_CLEAR(pbmp_local);
    BCM_PBMP_ASSIGN(pbmp_local, pbmp_all);
    vd->vlan_tag = BCM_VLAN_DEFAULT;
    BCM_PBMP_ASSIGN(vd->port_bitmap, pbmp_local);
    BCM_PBMP_REMOVE(vd->port_bitmap, pbmp_lb);
    BCM_PBMP_ASSIGN(vd->ut_port_bitmap, pbmp_local);
    BCM_PBMP_REMOVE(vd->ut_port_bitmap, pbmp_cmic);
    BCM_PBMP_REMOVE(vd->ut_port_bitmap, pbmp_lb);

    port_type = bcmint_vlan_info_default_port_type_get(unit);

    if (port_type == 1 || port_type == 3) {
        /* No CPU port in default VLAN. */
        BCM_PBMP_REMOVE(vd->port_bitmap, pbmp_cmic);
    }

    if (port_type == 2 || port_type == 3) {
        /* No Ethernet port in default VLAN. */
        BCM_PBMP_REMOVE(vd->port_bitmap, pbmp_e_all);
        BCM_PBMP_REMOVE(vd->ut_port_bitmap, pbmp_e_all);
    }

     if (!bcmint_vlan_info_auto_stack_get(unit)) {
        BCM_PBMP_REMOVE(vd->port_bitmap, SOC_PBMP_STACK_CURRENT);
        BCM_PBMP_REMOVE(vd->ut_port_bitmap, SOC_PBMP_STACK_CURRENT);
     }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add default VLAN entry.
 *
 * This function is used to add default VLAN entry.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_default_entry_add(int unit)
{
    bcm_pbmp_t           pbmp;
#ifndef BCM_VLAN_NO_DEFAULT_ETHER
    bcm_pbmp_t           pbmp_e_all;
#endif /* BCM_VLAN_NO_DEFAULT_ETHER */
    int                  profile_idx = 0;
    int                  dunit = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcm_vlan_data_t      vd;
    uint16_t             tpid = 0;
    int                  tpid_index = 0;
    ltsw_vlan_member_ports_profile_t ports_profile;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_default_vlan_populate(unit, &vd));

    BCM_PBMP_ASSIGN(pbmp, vd.ut_port_bitmap);

#ifndef BCM_VLAN_NO_DEFAULT_ETHER
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp_e_all));
    BCM_PBMP_OR(pbmp, pbmp_e_all);
#endif /* BCM_VLAN_NO_DEFAULT_ETHER */

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Allocate entry container */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &entry_hdl));

    /* Add VLAN ID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, vd.vlan_tag));

    /*
     * Set EGR_MEMBER_PORTS: vd.port_bitmap -> EGR_MEMBER_PORTSs ->
     * VLAN_TAB.PORT_BITMAPf/EGR_VLAN.PORT_BITMAPf
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, entry_hdl,
                               EGR_MEMBER_PORTSs, vd.port_bitmap));

    /*
     * Add VLAN_ING_EGR_MEMBER_PORTS_PROFILE_ID
     */
    sal_memset (&ports_profile, 0, sizeof(ports_profile));
    BCM_PBMP_ASSIGN(ports_profile.egr_member_ports, vd.port_bitmap);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_add(unit,
                                       &ports_profile,
                                       &profile_idx));

    /*
     * Set ING_MEMBER_PORTSs: vd.port_bitmap -> ING_MEMBER_PORTSs ->
     * VLAN_TAB.ING_PORT_BITMAPf
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                               profile_idx));

    /*
     * Set ING_MEMBER_PORTSs: vd.port_bitmap -> ING_MEMBER_PORTSs ->
     * VLAN_TAB.ING_PORT_BITMAPf
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, entry_hdl,
                               ING_MEMBER_PORTSs, vd.port_bitmap));

    /* VLAN_MPLS.MPLS_ENABLE -> 1 */
    if (ltsw_feature(unit, LTSW_FT_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MPLSs, 1));
    }

    /* VLAN_TAB.STG/EGR_VLAN.STG -> BCM_STG_DEFAULT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_STG_IDs, BCM_STG_DEFAULT));

    /* L3_IIF_IDs ->  VLAN_MPLS.L3_IIF  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, L3_IIF_IDs, vd.vlan_tag));

    /*
     * Set UNTAGGED_MEMBER_PORTS:  pbmp -> UNTAGGED_MEMBER_PORTS ->
     * EGR_VLAN.UT_BITMAPf
     */
     SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, entry_hdl,
                               UNTAGGED_MEMBER_PORTSs, pbmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_default_get(unit, &tpid));

    /* Add the default outer TPID entry twice during init so that
     * the default entry does not get removed even when no table
     * entry is referencing to the default TPID entry.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid, &tpid_index));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid, &tpid_index));

    /* VLAN_OUTER_TPID_ID -> EGR_VLAN.OUTER_TPID_INDEXf */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_OUTER_TPID_IDs,
                               (uint64_t)tpid_index));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_default_entry_add(unit, &profile_idx));

    /* VLAN_PROFILE_ID -> VLAN_TAB.VLAN_PROFILE_PTRf*/
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_PROFILE_IDs,
                               (uint64_t)profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

    /* Set default vlan into VLAN information */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_info_default_vlan_set(unit, vd.vlan_tag));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set member ports into VLAN table.
 *
 * This function is used to set member ports into VLAN table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  ing_pbmp      Ingress member ports.
 * \param [in]  ing_egr_pbmp  Egress member ports in ingress pipeline.
 * \param [in]  egr_pbmp      Egress member ports.
 * \param [in]  egr_ubmp      Untagged member ports.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
vlan_entry_port_set(int unit, bcm_vlan_t vid,
                    bcm_pbmp_t ing_pbmp, bcm_pbmp_t ing_egr_pbmp,
                    bcm_pbmp_t egr_pbmp, bcm_pbmp_t egr_ubmp,
                    uint64_t ori_profile_ptr)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;
    ltsw_vlan_member_ports_profile_t new_profile;
    int new_profile_ptr = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_IDs, vid));

    BCM_PBMP_ASSIGN(new_profile.egr_member_ports, ing_egr_pbmp);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_add(unit,
                                       &new_profile,
                                       &new_profile_ptr));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_delete(unit, ori_profile_ptr));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                               new_profile_ptr));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, eh,
                               EGR_MEMBER_PORTSs, egr_pbmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, eh,
                               UNTAGGED_MEMBER_PORTSs, egr_ubmp));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, eh,
                               ING_MEMBER_PORTSs, ing_pbmp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get member ports from VLAN table.
 *
 * This function is used to get member ports from VLAN table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [out] ing_pbmp      Ingress member ports.
 * \param [out] ing_egr_pbmp  Egress member ports in ingress pipeline.
 * \param [out] egr_pbmp      Egress member ports.
 * \param [out] egr_ubmp      Untagged member ports.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
vlan_entry_port_get(int unit, bcm_vlan_t vid,
                    bcm_pbmp_t *ing_pbmp, bcm_pbmp_t *ing_egr_pbmp,
                    bcm_pbmp_t *egr_pbmp, bcm_pbmp_t *egr_ubmp,
                    uint64_t *port_profile_ptr)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    ltsw_vlan_member_ports_profile_t profile;
    int dunit;
    uint64_t profile_index;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_IDs, vid));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                               &profile_index));

    sal_memset(&profile, 0, sizeof(profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_entry_get(unit,
                                             &profile,
                                             profile_index));
    if (ing_egr_pbmp != NULL) {
        BCM_PBMP_ASSIGN(*ing_egr_pbmp, profile.egr_member_ports);
    }

    if (egr_pbmp != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_member_ports_get(unit, eh,
                                   EGR_MEMBER_PORTSs, egr_pbmp));
    }
    if (egr_ubmp != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_member_ports_get(unit, eh,
                                   UNTAGGED_MEMBER_PORTSs, egr_ubmp));
    }
    if (ing_pbmp != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_member_ports_get(unit, eh,
                                   ING_MEMBER_PORTSs, ing_pbmp));
    }
    if (port_profile_ptr != NULL) {
        *port_profile_ptr = profile_index;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a VLAN entry in VLAN table
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_entry_create(int unit, bcm_vlan_t vid)
{
    int pre_config = FALSE, rv, tpid_index = 0;
    int vlan_profile_delete_flag = FALSE, otpid_delete_flag = FALSE;
    int profile_index = 0;
    bcm_pbmp_t  pbmp_local;
    ltsw_vlan_member_ports_profile_t profile;
    int profile_idx = 0;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(pbmp_local);
    pre_config = bcmint_vlan_info_pre_cfg_get(unit, vid);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_IDs, vid));
    rv = bcmi_lt_entry_commit
             (unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    if (SHR_E_NONE == rv) {
        if (pre_config) {
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    } else if (rv != SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_STG_IDs, 0));
    if (ltsw_feature(unit, LTSW_FT_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, MPLSs, 0x1));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, eh,
                               UNTAGGED_MEMBER_PORTSs, pbmp_local));

    BCM_PBMP_CLEAR(profile.egr_member_ports);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_add(unit,
                                       &profile,
                                       &profile_idx));

    if (profile_idx != VLAN_PROFILE_ING_EGR_MSHP_CHK_RESV_EMPTY) {
        (void)vlan_member_ports_profile_entry_delete(unit, profile_idx);
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                               profile_idx));

    if (!pre_config) {
        uint16_t tpid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_default_get(unit, &tpid));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid, &tpid_index));
        otpid_delete_flag = TRUE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, VLAN_OUTER_TPID_IDs, tpid_index));

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_profile_default_entry_add(unit, &profile_index));
        vlan_profile_delete_flag = TRUE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, VLAN_PROFILE_IDs, profile_index));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    otpid_delete_flag = FALSE;
    vlan_profile_delete_flag = FALSE;
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    if (otpid_delete_flag) {
        (void)bcmi_ltsw_vlan_otpid_entry_delete(unit, tpid_index);
    }
    if (vlan_profile_delete_flag) {
        (void)vlan_profile_entry_delete(unit, profile_index);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear a VLAN entry in VLAN_TAB
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_entry_destroy(int unit, bcm_vlan_t vid)
{
    uint64_t vlan_profile_ptr = 0;
    uint64_t port_profile_ptr = 0;
    ltsw_vlan_profile_t profile;
    int tpid_ptr, dunit = 0;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_IDs, vid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                               &port_profile_ptr));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_delete(unit, port_profile_ptr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, VLAN_PROFILE_IDs, &vlan_profile_ptr));

    sal_memset(&profile, 0, sizeof(ltsw_vlan_profile_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_get(unit, &profile, vlan_profile_ptr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_lookup(unit,
                                           profile.outer_tpid, &tpid_ptr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_delete(unit, tpid_ptr));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_delete(unit, vlan_profile_ptr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();

}

/*!
 * \brief Internal function to validate the given port to be a
 *         supported GPORT. Once validation successfully completes, resolve the
 *         given port to its local port representation.
 *
 * \param [in] unit                 Unit number.
 * \param [in] in_port              Port/gport to be validated and resolved.
 * \param [out] out_port            Local port. If NULL then the function would
 *                                  only validate the given port.
 *
 * \retval SHR_E_NONE                 No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_gport_resolve(int unit, bcm_port_t in_port, bcm_port_t *out_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out_port, SHR_E_PARAM);

    if (BCM_GPORT_IS_LOCAL(in_port) ||
        BCM_GPORT_IS_LOCAL_CPU(in_port) ||
        BCM_GPORT_IS_DEVPORT(in_port) ||
        BCM_GPORT_IS_MODPORT(in_port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, in_port, out_port));
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the validation of VLAN control configuration.
 *
 * This function is used to check the validation of VLAN control configuration.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  valid_fields  Valid fields for VLAN control structure,
 *                            BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [in]  control       VLAN control structure.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
vlan_control_validate(int unit,
                      bcm_vlan_t vid,
                      uint32_t valid_fields,
                      bcm_vlan_control_vlan_t *control)
{
    uint32_t supported_flags = 0;
    uint32_t supported_fields = 0;
    int l3_iif_min, l3_iif_max;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vid);

    supported_fields = LTSW_XGS_VLAN_CONTROL_VLAN_ALL_VALID_MASK;

    supported_flags  = LTSW_XGS_VLAN_CONTROL_VLAN_SUPPORTED_CTRL_FLAGS;

    if (valid_fields != BCM_VLAN_CONTROL_VLAN_ALL_MASK) {
        if (valid_fields & (~supported_fields)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    if (control == NULL) {
        SHR_EXIT();
    }

    if (control->flags & (~supported_flags)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (control->flags2 != 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((control->learn_flags != 0) || (control->sr_flags != 0)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (bcmVlanUrpfDisable != control->urpf_mode) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_MODE_MASK) {
        if ((control->forwarding_mode != bcmVlanForwardBridging) &&
            (control->forwarding_mode != bcmVlanForwardSingleCrossConnect)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_ing_intf_id_range_get(unit, &l3_iif_min, &l3_iif_max));

        if (control->ingress_if != 0 &&
           (control->ingress_if > l3_iif_max ||
            control->ingress_if < l3_iif_min)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* Cannot set l3 class id of vlan if ingress_map_mode=1 */
        if(control->flags & BCM_VLAN_L3_CLASS_ID) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    if (control->forwarding_vlan > BCM_VLAN_MAX) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (control->forwarding_mode == bcmVlanForwardSingleCrossConnect) {
        /* No L2 learning for cross-connect */
        if (0 == (control->flags & BCM_VLAN_LEARN_DISABLE)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Internal function to validate the vlan action.
 *
 * \param [in] unit                Unit number.
 * \param [inout]port              Port number.
 * \param [in] action              tag actions to be verified.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
vlan_action_validate(int unit, bcm_port_t *port,
                     bcm_vlan_action_set_t *action)
{
    bcm_port_t port_in;

    SHR_FUNC_ENTER(unit);

    port_in = *port;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port_in, port));

    if (action == NULL) {
        SHR_EXIT();
    }

    VLAN_ID_CHECK(action->new_outer_vlan);

    if (action->priority < -1 ||
        action->priority > 7 || action->new_outer_cfi > 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Double Tag operations not supported */
    if ((bcmVlanActionNone != action->dt_outer) ||
        (bcmVlanActionNone != action->dt_outer_prio) ||
        (bcmVlanActionNone != action->dt_outer_pkt_prio) ||
        (bcmVlanActionNone != action->dt_outer_cfi) ||
        (bcmVlanActionNone != action->dt_inner) ||
        (bcmVlanActionNone != action->dt_inner_prio) ||
        (bcmVlanActionNone != action->dt_inner_pkt_prio) ||
        (bcmVlanActionNone != action->dt_inner_cfi)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Two tags not supported */
    if ((bcmVlanActionNone != action->ot_inner) ||
        (bcmVlanActionNone != action->ot_inner_pkt_prio) ||
        (bcmVlanActionNone != action->ot_inner_cfi) ||
        (bcmVlanActionNone != action->it_outer) ||
        (bcmVlanActionNone != action->it_outer_pkt_prio) ||
        (bcmVlanActionNone != action->it_outer_cfi)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* No Inner tag operations supported */
    if ((bcmVlanActionNone != action->it_inner) ||
        (bcmVlanActionNone != action->it_inner_prio) ||
        (bcmVlanActionNone != action->it_inner_pkt_prio) ||
        (bcmVlanActionNone != action->it_inner_cfi) ||
        (bcmVlanActionNone != action->ut_inner) ||
        (bcmVlanActionNone != action->ut_inner_pkt_prio) ||
        (bcmVlanActionNone != action->ut_inner_cfi)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    switch (action->ot_outer) {
        case bcmVlanActionNone:
        case bcmVlanActionReplace:
        case bcmVlanActionDelete:
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    switch (action->ot_outer_prio) {
        case bcmVlanActionNone:
        case bcmVlanActionReplace:
        case bcmVlanActionDelete:
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    switch (action->ot_outer_pkt_prio) {
        case bcmVlanActionNone:
            break;
        case bcmVlanActionReplace:
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    switch (action->ot_outer_cfi) {
        case bcmVlanActionNone:
            break;
        case bcmVlanActionReplace:
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    switch (action->ut_outer) {
        case bcmVlanActionNone:
        case bcmVlanActionAdd:
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    switch (action->ut_outer_pkt_prio) {
        case bcmVlanActionNone:
            break;
        case bcmVlanActionAdd:
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    switch (action->ut_outer_cfi) {
        case bcmVlanActionNone:
            break;
        case bcmVlanActionAdd:
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Internal function to set vlan tag action profile.
 *
 * \param [out] profile            Vlan action profile.
 * \param [in] action              Vlan action structure.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static void
vlan_tag_action_profile_set(ltsw_vlan_tag_action_profile_t *profile,
                            bcm_vlan_action_set_t *action)
{

    sal_memset(profile, 0, sizeof(ltsw_vlan_tag_action_profile_t));

    profile->sot_otag_action  = VLAN_TAG_ACTION_ENCODE(action->ot_outer);
    profile->sot_potag_action = VLAN_TAG_ACTION_ENCODE(action->ot_outer_prio);
    profile->ut_otag_action   = VLAN_TAG_ACTION_ENCODE(action->ut_outer);
    profile->sot_opri_action  = VLAN_PRI_CFI_ACTION_ENCODE(action->ot_outer_pkt_prio);
    profile->sot_ocfi_action  = VLAN_PRI_CFI_ACTION_ENCODE(action->ot_outer_cfi);
    profile->ut_opri_action   = VLAN_PRI_CFI_ACTION_ENCODE(action->ut_outer_pkt_prio);
    profile->ut_ocfi_action   = VLAN_PRI_CFI_ACTION_ENCODE(action->ut_outer_cfi);
}

/*!
 * \brief Internal function to set vlan tag action profile.
 *
 * \param [in] profile             Vlan action profile.
 * \param [in] action              tag actions to be filled.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static void
vlan_tag_action_set(ltsw_vlan_tag_action_profile_t *profile,
                    bcm_vlan_action_set_t *action)
{
    action->ot_outer = VLAN_TAG_ACTION_DECODE(profile->sot_otag_action);
    action->ot_outer_prio = VLAN_TAG_ACTION_DECODE(profile->sot_potag_action);
    action->ut_outer = VLAN_TAG_ACTION_DECODE(profile->ut_otag_action);
    action->ot_outer_pkt_prio = VLAN_PRI_CFI_ACTION_DECODE(profile->sot_opri_action);
    action->ot_outer_cfi = VLAN_PRI_CFI_ACTION_DECODE(profile->sot_ocfi_action);
    action->ut_outer_pkt_prio = VLAN_PRI_CFI_ACTION_DECODE(profile->ut_opri_action);
    action->ut_outer_cfi = VLAN_PRI_CFI_ACTION_DECODE(profile->ut_ocfi_action);
}

/*!
 * \brief Internal function to find shortest bitmap.
 *
 * \param [in] unit                BCM device number.
 * \param [in] array               Bitmaps array.
 * \param [in] arr_size            Array size.
 * \param [out] min_pbm            Shortest bitmap.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
vlan_block_bitmap_min(int unit, bcm_pbmp_t *array,
                      int arr_size, bcm_pbmp_t *min_pbm)
{
    int idx, min, mask_len;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(array, SHR_E_PARAM);
    BCM_PBMP_CLEAR(*min_pbm);

    min = 0;
    for (idx = 0; idx < arr_size; idx++) {
        BCM_PBMP_COUNT(array[idx], mask_len);
        if (0 == mask_len) {
            continue;
        } else {
            if ((0 == min) || ((0 != min) && (mask_len < min)))  {
                min = mask_len;
                BCM_PBMP_ASSIGN(*min_pbm, array[idx]);
            }
        }
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Internal function to clear those masks in the array that exactly match
 *  the given mask.
 *
 * \param [in] unit                BCM device number.
 * \param [in] array               Bitmaps array.
 * \param [in] arr_size            Array size.
 * \param [in] mask                Mask.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
vlan_block_bitmap_clear(int unit, bcm_pbmp_t *array,
                        int arr_size, bcm_pbmp_t mask)
{
    int idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(array, SHR_E_PARAM);

    for (idx = 0; idx < arr_size; idx++) {
        if (BCM_PBMP_EQ(array[idx], mask)) {
            BCM_PBMP_CLEAR(array[idx]);
        }
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Internal function to identify block mask combination which will produce
 *    requested block mask.
 *
 * \param [in] unit                BCM device number.
 * \param [in] request             Requested mask.
 * \param [in] first               Bitmaps array.
 * \param [in] second              Bitmaps array.
 * \param [out] sel                Block mask.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
vlan_block_select_get(int unit, bcm_pbmp_t request,
                      bcm_pbmp_t first,
                      bcm_pbmp_t second,
                      uint8_t *sel)
{
    bcm_pbmp_t or_mask;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    SHR_NULL_CHECK(sel, SHR_E_PARAM);

    if (BCM_PBMP_IS_NULL(request)) {
        *sel = 0;
    } else if (BCM_PBMP_EQ(first, request)) {
        *sel = 0x1;
    } else if (BCM_PBMP_EQ(second, request)) {
        *sel = 0x2;
    } else {
        BCM_PBMP_ASSIGN(or_mask, first);
        BCM_PBMP_OR(or_mask, second);
        if (BCM_PBMP_EQ(or_mask, request)) {
            *sel = 0x3;
        } else {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Internal function to compose block mask based.
 *
 * \param [in] unit                BCM device number.
 * \param [in] first_mask          First mask.
 * \param [in] second_mask         Second mask.
 * \param [in] select              Mask select.
 * \param [out] result_mask        Output mask.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
static int
vlan_block_mask_create(int unit, bcm_pbmp_t first_mask,
                       bcm_pbmp_t second_mask,
                       uint8_t select, bcm_pbmp_t *result_mask)
{
    SHR_FUNC_ENTER(unit);

    /* Input parameters check */
    SHR_NULL_CHECK(result_mask, SHR_E_PARAM);

    switch (select) {
        case 0:
            BCM_PBMP_CLEAR(*result_mask);
            break;
        case 1:
            BCM_PBMP_ASSIGN(*result_mask, first_mask);
            break;
        case 2:
            BCM_PBMP_ASSIGN(*result_mask, second_mask);
            break;
        case 3:
            BCM_PBMP_ASSIGN(*result_mask, first_mask);
            BCM_PBMP_OR(*result_mask, second_mask);
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM);
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set port-specific VLAN configuration.
 *
 * This function is used to set port-specific VLAN configuration.
 *
 * \param [in] unit               Unit number.
 * \param [inout]port             Port number.
 * \param [in]  type              Control port type.
 *
 * \retval SHR_E_NONE             No errors.
 * \retval !SHR_E_NONE            Failure.
 */
static int
vlan_control_port_set(int unit,
                      int port,
                      bcm_vlan_control_port_t type,
                      int arg)
{
    bcm_port_t port_out;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port_out));

    arg = (arg ? 1 : 0);
    switch (type) {
        case bcmVlanPortVerifyOuterTpid:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set(unit, port_out,
                                        BCMI_PT_OUTER_TPID_VERIFY, arg));
            SHR_EXIT();
        case bcmVlanPortOuterTpidSelect:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set(unit, port_out,
                                        BCMI_PT_USE_TABLE_VLAN_OUTER_TPID_ID,
                                        arg));
            SHR_EXIT();
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port-specific VLAN configuration.
 *
 * This function is used to get port-specific VLAN configuration.
 *
 * \param [in] unit               Unit number.
 * \param [in]port                Port number.
 * \param [in] type               Control port type.
 * \param [out] arg               Arg vlaue which is dependent on control type.
 *
 * \retval SHR_E_NONE             No errors.
 * \retval !SHR_E_NONE            Failure.
 */
static int
vlan_control_port_get(int unit,
                      bcm_port_t port,
                      bcm_vlan_control_port_t type,
                      int *arg)
{
    bcm_port_t port_out;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port_out));

    switch (type) {
        case bcmVlanPortVerifyOuterTpid:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get(unit, port_out,
                                        BCMI_PT_OUTER_TPID_VERIFY, arg));
            SHR_EXIT();
        case bcmVlanPortOuterTpidSelect:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get(unit, port_out,
                                        BCMI_PT_USE_TABLE_VLAN_OUTER_TPID_ID,
                                        arg));
            SHR_EXIT();
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear VLAN entries.
 *
 * This function is used to clear VLAN entries in VLAN tables.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_table_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, VLANs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the profiles of VLAN module.
 *
 * This function is used to initialize the profiles of VLAN module.
 *
 * \param [in] unit            Unit number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_module_profile_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_tag_action_profile_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_tag_action_profile_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the VLAN module.
 *
 * This function is used to initialize the VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear VLAN entries in VLAN. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_table_clear(unit));

    /* Initialize profiles in VLAN module. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_module_profile_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add default entries for different VLAN tables.
 *
 * This function is used to add default entries for different VLAN tables.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_module_default_entry_add(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_default_set(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_empty_member_ports_profile_add(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_default_entry_add(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Re-initialize the profiles of VLAN module.
 *
 * This function is used to re-initialize the profiles of VLAN module.
 *
 * \param [in] unit            Unit number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_module_profile_reinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Register ingress egress membership check profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_register(unit));

    /* Recover reserved entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_profile_recover(unit,
                                     VLAN_PROFILE_ING_EGR_MSHP_CHK_RESV_EMPTY));

    /* Register VLAN profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_register(unit));

    /* Recover reserved entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_recover(unit, VLAN_PROFILE_RESV));

    /* Register VLAN ingress tag action profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_tag_action_profile_register(unit));

    /* Register VLAN egress tag action profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_tag_action_profile_register(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover VLAN_PROFILE table related reference count.
 *
 * This function is used to recover VLAN_PROFILE table related reference count.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_profile_table_recover(int unit)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int dunit = 0;
    uint64_t value = 0;
    int profile_idx = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_PROFILEs, &ent_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, ent_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        /* Recover reference count of PORT_PKT_CONTROL. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, PORT_PKT_CONTROL_IDs, &value));
        profile_idx = (int)value;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_profile_port_pkt_ctrl_recover(unit, profile_idx));
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover SW states from the VLAN table.
 *
 * This function is used to recover SW states from the VLAN table.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_table_recover(int unit)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int dunit = 0;
    uint64_t value = 0;
    int tpid_index = -1;
    int profile_idx = 0;
    bcm_vlan_t vlan = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &ent_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, ent_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        /* Recover TPID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, VLAN_OUTER_TPID_IDs, &value));
        tpid_index = (int)value;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_ref_count_add(unit, tpid_index, 1));

        /* Recover the profile for ingress egress membership check. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                                   &value));
        profile_idx = (int)value;
        if (profile_idx != VLAN_PROFILE_ING_EGR_MSHP_CHK_RESV_EMPTY) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_member_ports_profile_recover(unit, profile_idx));
        }

        /* Recover the profile for VLAN profile. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, VLAN_PROFILE_IDs,
                                   &value));

        profile_idx = (int)value;
        if (profile_idx != VLAN_PROFILE_RESV) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_profile_recover(unit, profile_idx));
        }

        /* Set the usage bitmap of VLANs. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, VLAN_IDs, &value));
        vlan = (bcm_vlan_t)value;
        if ((vlan <= BCM_VLAN_MAX) ||
            (!bcmint_vlan_info_pre_cfg_get(unit, vlan))) {
            bcmint_vlan_info_set(unit, vlan);
        }
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover reference count of ING/EGR tag actions for PORT tables.
 *
 * This function is used to update reference count of ING/EGR tag actions for
 * PORT tables.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_tag_action_ref_count_port_recover(int unit)
{
    int          index = 0;
    bcm_pbmp_t   pbmp_local;
    bcm_port_t   port;
    bcm_pbmp_t   pbmp_all;
    bcm_vlan_action_set_t action;

    SHR_FUNC_ENTER(unit);

    /* All ports have the reference to ING/EGR tag actions */
    BCM_PBMP_CLEAR(pbmp_local);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp_all));
    BCM_PBMP_ASSIGN(pbmp_local, pbmp_all);

    BCM_PBMP_ITER(pbmp_local, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_ing_vlan_action_get(unit, port, &action, &index));

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_tag_action_profile_recover(unit, index));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_egr_vlan_action_get(unit, port, &action, &index));

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_tag_action_profile_recover(unit, index));
    }

    if (SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)){
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover the SW state in VLAN module.
 *
 * This function is used to recover the SW state in VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_reinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Re-initialize the profiles in VLAN module. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_module_profile_reinit(unit));

    /* Recover SW states related to VLAN_PROFILE. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_table_recover(unit));

    /* Recover SW states related to VLAN. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_table_recover(unit));

    /* Recover reference count of ING/EGR tag actions for PORT tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_tag_action_ref_count_port_recover(unit));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize VLAN moudle.
 *
 * This function is used to initialize the all VLAN table information with
 * default VLAN.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xgs_ltsw_vlan_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_reinit(unit));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_init(unit));

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_module_default_entry_add(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the VLAN moudle.
 *
 * This function is used to de-initialize the VLAN moudle.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xgs_ltsw_vlan_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_table_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize outer TPID information.
 *
 * This function is used to initialize outer TPID information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xgs_ltsw_vlan_otpid_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_otpid_table_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize outer TPID information.
 *
 * This function is used to de-initialize outer TPID information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xgs_ltsw_vlan_otpid_detach(int unit)
{
    COMPILER_REFERENCE(unit);
    return SHR_E_NONE;
}

/*!
 * \brief Add VLAN outer TPID.
 *
 * \param [in] unit      Unit Number.
 * \param [in] index     Entry index.
 * \param [in] tpid      Outer TPID value.
 *
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
xgs_ltsw_vlan_otpid_add(int unit, int index, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_otpid_entry_add(unit, index, tpid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_otpid_info_tpid_set(unit, index, tpid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_otpid_info_ref_count_add(unit, index, 1));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set global inner TPID.
 *
 * This function is used to set global inner TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 TPID to be set.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xgs_ltsw_vlan_itpid_set(int unit, uint16_t tpid)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_INNER_TPIDs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, ING_TPIDs, (uint64_t)tpid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, EGR_TPIDs, (uint64_t)tpid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, ent_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get global inner TPID.
 *
 * This function is used to get global inner TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] tpid                 Inner TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xgs_ltsw_vlan_itpid_get(int unit, uint16_t *tpid)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t value = 0;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_INNER_TPIDs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, ING_TPIDs, &value));

    *tpid = (uint16_t)value;

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_port_add(int unit, bcm_vlan_t vid, bcm_pbmp_t ing_pbmp,
                       bcm_pbmp_t egr_pbmp, bcm_pbmp_t egr_ubmp, int flags)
{
    bcm_pbmp_t ori_ing_egr_pbmp, ori_egr_pbmp;
    bcm_pbmp_t ori_egr_ubmp, ori_ing_pbmp;
    bcm_pbmp_t ubmp, pbmp, ie_pbmp, ing_egr_pbmp;
    uint64_t port_profile_ptr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vid,
                             &ori_ing_pbmp, &ori_ing_egr_pbmp,
                             &ori_egr_pbmp, &ori_egr_ubmp,
                             &port_profile_ptr));

    /* Make the port tagged/untagged if its already added to the vlan. */
    BCM_PBMP_ASSIGN(ubmp, ori_egr_ubmp);
    BCM_PBMP_ASSIGN(pbmp, ori_egr_pbmp);
    BCM_PBMP_ASSIGN(ie_pbmp, ori_ing_egr_pbmp);

    BCM_PBMP_ASSIGN(ing_egr_pbmp, egr_pbmp);

    BCM_PBMP_XOR(ubmp, egr_ubmp);
    BCM_PBMP_REMOVE(pbmp, ubmp);
    BCM_PBMP_REMOVE(ie_pbmp, ubmp);

    BCM_PBMP_REMOVE(egr_pbmp, pbmp);
    BCM_PBMP_REMOVE(egr_ubmp, pbmp);
    BCM_PBMP_REMOVE(ing_egr_pbmp, ie_pbmp);
    BCM_PBMP_REMOVE(ing_pbmp, ori_ing_pbmp);

    /* Only allow untagged ports on the vlan. */
    BCM_PBMP_AND(egr_ubmp, egr_pbmp);

    if (BCM_PBMP_NOT_NULL(egr_pbmp) ||
        BCM_PBMP_NOT_NULL(egr_ubmp) ||
        BCM_PBMP_NOT_NULL(ing_pbmp) ||
        BCM_PBMP_NOT_NULL(ing_egr_pbmp)) {

        /* Set egress untagged ports. */
        BCM_PBMP_REMOVE(ori_egr_ubmp, egr_pbmp);
        BCM_PBMP_OR(ori_egr_ubmp, egr_ubmp);

        /* Set egress ports in ingress pipeline. */
        if (!(flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
            BCM_PBMP_OR(ori_ing_egr_pbmp, ing_egr_pbmp);
        }

        /* Set ingress ports. */
        BCM_PBMP_OR(ori_ing_pbmp, ing_pbmp);

        /* Set egress ports. */
        BCM_PBMP_OR(ori_egr_pbmp, egr_pbmp);

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_entry_port_set(unit, vid,
                                 ori_ing_pbmp, ori_ing_egr_pbmp,
                                 ori_egr_pbmp, ori_egr_ubmp,
                                 port_profile_ptr));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_port_get(int unit, bcm_vlan_t vid, bcm_pbmp_t *ing_pbmp,
                       bcm_pbmp_t *egr_pbmp, bcm_pbmp_t *egr_ubmp)
{
    bcm_pbmp_t ing_egr_pbmp;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(ing_egr_pbmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vid,
                             ing_pbmp, &ing_egr_pbmp,
                             egr_pbmp, egr_ubmp, NULL));

    if (egr_pbmp) {
        BCM_PBMP_OR(*egr_pbmp, ing_egr_pbmp);
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_port_remove(int unit, bcm_vlan_t vid, bcm_pbmp_t ing_pbmp,
                          bcm_pbmp_t egr_pbmp, bcm_pbmp_t egr_ubmp)
{
    bcm_pbmp_t ori_ing_egr_pbmp, ori_egr_pbmp;
    bcm_pbmp_t ori_egr_ubmp, ori_ing_pbmp;
    uint64_t profile_ptr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vid,
                             &ori_ing_pbmp, &ori_ing_egr_pbmp,
                             &ori_egr_pbmp, &ori_egr_ubmp, &profile_ptr));

    BCM_PBMP_REMOVE(ori_ing_pbmp, ing_pbmp);
    BCM_PBMP_REMOVE(ori_ing_egr_pbmp, egr_pbmp);
    BCM_PBMP_REMOVE(ori_egr_pbmp, egr_pbmp);
    BCM_PBMP_REMOVE(ori_egr_ubmp, egr_ubmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_set(unit, vid,
                             ori_ing_pbmp, ori_ing_egr_pbmp,
                             ori_egr_pbmp, ori_egr_ubmp, profile_ptr));
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_create(int unit, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_create(unit, vid));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_destroy(int unit, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_destroy(unit, vid));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_gport_add(int unit, bcm_vlan_t vlan, bcm_gport_t port, int flags)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp, ubmp, ing_pbmp;
    uint32_t supported_flags = 0;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vlan);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_gport_resolve(unit, port, &local_port));

    /* Skip internal loopback port.*/
    if (bcmi_ltsw_port_is_type(unit, local_port, BCMI_LTSW_PORT_TYPE_LB)) {
        SHR_EXIT();
    }

    supported_flags = BCM_VLAN_GPORT_ADD_UNTAGGED |
                      BCM_VLAN_GPORT_ADD_INGRESS_ONLY |
                      BCM_VLAN_GPORT_ADD_EGRESS_ONLY |
                      BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY;
    if ((~supported_flags) & flags) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY) &&
        (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (flags & BCM_VLAN_GPORT_ADD_INGRESS_ONLY) {
        if (flags & BCM_VLAN_GPORT_ADD_UNTAGGED) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if ((flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY) ||
            (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (flags & (BCM_VLAN_PORT_EGRESS_ONLY |
                 BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
        BCM_PBMP_CLEAR(ing_pbmp);
    } else {
        BCM_PBMP_PORT_SET(ing_pbmp, local_port);
    }

    if (flags & BCM_VLAN_PORT_INGRESS_ONLY) {
        BCM_PBMP_CLEAR(pbmp);
    } else {
        BCM_PBMP_PORT_SET(pbmp, local_port);
    }

    if (flags & BCM_VLAN_PORT_UNTAGGED) {
        /* No such thing as untagged CPU. */
        if (local_port != bcmi_ltsw_port_cpu(unit)){
            BCM_PBMP_PORT_SET(ubmp, local_port);
        }
    } else {
        BCM_PBMP_CLEAR(ubmp);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_vlan_port_add(unit, vlan, ing_pbmp, pbmp, ubmp, flags));
exit:
    SHR_FUNC_EXIT();

}

int
xgs_ltsw_vlan_gport_delete(int unit, bcm_vlan_t vlan,
                           bcm_gport_t port, int flags)
{
    bcm_pbmp_t ing_pbmp, pbmp, ubmp;
    bcm_port_t local_port;
    uint32_t supported_flags = 0;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vlan);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_gport_resolve(unit, port, &local_port));
    supported_flags = BCM_VLAN_GPORT_ADD_UNTAGGED |
                      BCM_VLAN_GPORT_ADD_INGRESS_ONLY |
                      BCM_VLAN_GPORT_ADD_EGRESS_ONLY |
                      BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY;
    if (flags & ~supported_flags) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (flags &
        (BCM_VLAN_PORT_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
        BCM_PBMP_CLEAR(ing_pbmp);
    } else {
        BCM_PBMP_PORT_SET(ing_pbmp, local_port);
    }
    if (flags & BCM_VLAN_PORT_INGRESS_ONLY) {
        BCM_PBMP_CLEAR(pbmp);
    } else {
        BCM_PBMP_PORT_SET(pbmp, local_port);
    }
    if (flags & BCM_VLAN_PORT_UNTAGGED) {
        BCM_PBMP_PORT_SET(ubmp, local_port);
    } else {
        BCM_PBMP_CLEAR(ubmp);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_vlan_port_remove(unit, vlan, ing_pbmp, pbmp, ubmp));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_gport_delete_all(int unit, bcm_vlan_t vlan)
{
    bcm_pbmp_t pbmp;
    uint64_t ori_port_profile_ptr;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vlan);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vlan, NULL, NULL,NULL, NULL,
                             &ori_port_profile_ptr));
    BCM_PBMP_CLEAR(pbmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_set(unit, vlan, pbmp, pbmp, pbmp, pbmp,
                             ori_port_profile_ptr));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_gport_get(int unit, bcm_vlan_t vlan,
                        bcm_gport_t port, int *flags)
{
    bcm_pbmp_t pbmp, ubmp, ing_pbmp, ing_egr_pbmp;
    bcm_port_t local_port;
    uint32_t local_flags = 0;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vlan);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_gport_resolve(unit, port, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vlan, &ing_pbmp, &ing_egr_pbmp,
                             &pbmp, &ubmp, NULL));

    BCM_PBMP_OR(pbmp, ing_egr_pbmp);

    if (!BCM_PBMP_MEMBER(pbmp, local_port)) {
        local_flags |= BCM_VLAN_PORT_INGRESS_ONLY;
    }
    if (!BCM_PBMP_MEMBER(ing_pbmp, local_port)) {
        local_flags |= BCM_VLAN_PORT_EGRESS_ONLY;
    }
    if (local_flags ==
        (BCM_VLAN_PORT_INGRESS_ONLY | BCM_VLAN_PORT_EGRESS_ONLY)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    if (BCM_PBMP_MEMBER(ubmp, local_port)) {
        local_flags |= BCM_VLAN_PORT_UNTAGGED;
    }
    if (local_flags & BCM_VLAN_PORT_EGRESS_ONLY) {
        if (!BCM_PBMP_MEMBER(ing_egr_pbmp, local_port)) {
            local_flags &= ~BCM_VLAN_PORT_EGRESS_ONLY;
            local_flags |= BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY;
        }
    }
    *flags = local_flags;
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_gport_get_all(int unit, bcm_vlan_t vlan, int array_max,
                            bcm_gport_t *gport_array, int *flags_array,
                            int* array_size)
{
    bcm_pbmp_t          pbmp, ubmp, ing_pbmp, bmp, ing_egr_pbmp;
    bcm_port_t          port, gport;
    int                 port_cnt = 0;
    uint32_t            local_flags = 0;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vlan);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vlan, &ing_pbmp, &ing_egr_pbmp,
                             &pbmp, &ubmp, NULL));
    BCM_PBMP_ASSIGN(bmp, pbmp);
    BCM_PBMP_OR(bmp, ubmp);
    BCM_PBMP_OR(bmp, ing_pbmp);
    BCM_PBMP_OR(pbmp, ing_egr_pbmp);
    BCM_PBMP_ITER(bmp, port) {
        if ((port_cnt == array_max) && (gport_array)) {
            break;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_gport_get(unit, port, &gport));
        local_flags = 0;
        if (!BCM_PBMP_MEMBER(pbmp, port)) {
            local_flags |= BCM_VLAN_PORT_INGRESS_ONLY;
        }
         if (!BCM_PBMP_MEMBER(ing_pbmp, port)) {
            local_flags |= BCM_VLAN_PORT_EGRESS_ONLY;
        }
        if (local_flags ==
            (BCM_VLAN_PORT_INGRESS_ONLY | BCM_VLAN_PORT_EGRESS_ONLY)) {
            continue;
        }
        if (local_flags & BCM_VLAN_PORT_EGRESS_ONLY) {
            if (!BCM_PBMP_MEMBER(ing_egr_pbmp, port)) {
                local_flags &= ~BCM_VLAN_PORT_EGRESS_ONLY;
                local_flags |= BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY;
            }
        }
        if (flags_array) {
            if (BCM_PBMP_MEMBER(ubmp, port)) {
                local_flags |= BCM_VLAN_PORT_UNTAGGED;
            }
            flags_array[port_cnt] = local_flags;
        }
        if (gport_array) {
            gport_array[port_cnt] = gport;
        }
        port_cnt++;
    }

    if (!port_cnt) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *array_size = port_cnt;
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_control_vlan_set(int unit, bcm_vlan_t vid,
                               uint32_t valid_fields,
                               bcm_vlan_control_vlan_t *control)
{
    uint16_t old_outer_tpid;
    int rv, insert = FALSE, tpid_index = -1;
    uint64_t data;
    int profile_index = 0, new_profile_index;
    ltsw_vlan_profile_t *old_vpt, vpt, profile;
    int vlan_exist, pre_config, dunit = 0;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *fwd_mode[3] = {BRIDGEs, SINGLE_XCONNECTs};

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vid);

    /* Check the validation of VLAN control configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_control_validate(unit, vid, valid_fields, control));

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_PROTOCOL_PKT_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_protocol_packet_actions_validate(unit,
                                                   &control->protocol_pkt));
    }

    sal_memset(&profile, 0, sizeof(ltsw_vlan_profile_t));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_IDs, vid));
    rv = bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);
    if (SHR_E_NOT_FOUND == rv) {
        insert = TRUE;
    } else if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }

    if (ltsw_feature(unit, LTSW_FT_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, MPLSs,
                (control->flags & BCM_VLAN_MPLS_DISABLE) ? 0x0 : 0x1));
    }

    pre_config = bcmint_vlan_info_pre_cfg_get(unit, vid);
    vlan_exist = bcmint_vlan_info_exist_check(unit, vid);

    if (vlan_exist) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, VLAN_PROFILE_IDs, &data));
        profile_index = (int)data;
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_profile_entry_get(unit, &profile, profile_index));
        old_vpt = &profile;
        vpt = *old_vpt;
        old_outer_tpid = old_vpt->outer_tpid;
    } else {
        uint16_t dft_tpid;

        rv = bcmlt_entry_field_get(eh, VLAN_PROFILE_IDs, &data);
        if (SHR_E_NOT_FOUND == rv) {
            profile_index = VLAN_PROFILE_RESV;
        } else if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        } else {
            profile_index = (int) data;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_profile_entry_get(unit, &profile, profile_index));
        old_vpt = &profile;
        vpt = *old_vpt;
        old_outer_tpid = old_vpt->outer_tpid;

        if (!pre_config) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_default_get(unit, &dft_tpid));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_add(unit, dft_tpid, &tpid_index));
            old_outer_tpid = dft_tpid;
        }
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK) {
        if (old_outer_tpid != control->outer_tpid) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_lookup
                     (unit, old_outer_tpid, &tpid_index));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_delete(unit, tpid_index));
            SHR_IF_ERR_CONT
                (bcmi_ltsw_vlan_otpid_entry_add
                       (unit, control->outer_tpid, &tpid_index));
            if (SHR_FUNC_ERR()) {
                (void)bcmi_ltsw_vlan_otpid_entry_add
                         (unit, old_outer_tpid, &tpid_index);
                SHR_EXIT();
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh,
                                       VLAN_OUTER_TPID_IDs, tpid_index));
        }
    }

    vpt.flags = control->flags &
                LTSW_XGS_VLAN_CONTROL_VLAN_PROFILE_CTRL_FLAGS;
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK) {
        vpt.outer_tpid = control->outer_tpid;
    } else {
        vpt.outer_tpid = old_outer_tpid;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IP6_MCAST_FLOOD_MODE_MASK) {
        vpt.ip6_mcast_flood_mode = control->ip6_mcast_flood_mode;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IP4_MCAST_FLOOD_MODE_MASK) {
        vpt.ip4_mcast_flood_mode = control->ip4_mcast_flood_mode;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_L2_MCAST_FLOOD_MODE_MASK) {
        vpt.l2_mcast_flood_mode = control->l2_mcast_flood_mode;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_PROTOCOL_PKT_MASK) {
        sal_memcpy(&(vpt.protocol_pkt), &(control->protocol_pkt),
                    sizeof(bcm_vlan_protocol_packet_ctrl_t));
        vlan_protocol_pkt_ctrl_default_set(&(vpt.protocol_pkt));
    }

    if (vlan_exist) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_profile_entry_add(unit, &vpt, &new_profile_index));
        if (new_profile_index != profile_index) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_profile_entry_delete(unit, profile_index));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add
                    (eh, VLAN_PROFILE_IDs, new_profile_index));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_profile_entry_add(unit, &vpt, &new_profile_index));
        if (new_profile_index != profile_index) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add
                    (eh, VLAN_PROFILE_IDs, new_profile_index));
            if (pre_config) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_profile_entry_delete(unit, profile_index));
            }
        }
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, L3_IIF_IDs, control->ingress_if));
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, FIDs, control->forwarding_vlan));
    }

    if ((valid_fields & BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK) &&
        (!((!(control->flags & BCM_VLAN_L2_CLASS_ID_ONLY)) &&
             (control->flags & BCM_VLAN_L3_CLASS_ID)))) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add
                (eh, CLASS_IDs, control->if_class));
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_MODE_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add
                (eh, FORWARDING_BEHAVIORs,
                         fwd_mode[control->forwarding_mode]));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, IGMP_MLD_SNOOPINGs,
            (0 == (control->flags & BCM_VLAN_IGMP_SNOOP_DISABLE))));
    if (!vlan_exist) {
        bcmint_vlan_info_pre_cfg_set(unit, vid);
    }

    if (insert) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_control_vlan_get(int unit, bcm_vlan_t vid, uint32_t valid_fields,
                               bcm_vlan_control_vlan_t *control)
{
    uint64_t profile_index = 0;
    int pre_cfg, vlan_exist, dunit = 0;
    uint64_t data;
    ltsw_vlan_profile_t vp, *vpt = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vid);

    if (control == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(control, 0, sizeof(bcm_vlan_control_vlan_t));
    sal_memset(&vp, 0, sizeof(ltsw_vlan_profile_t));

    vlan_exist = bcmint_vlan_info_exist_check(unit, vid);
    pre_cfg = bcmint_vlan_info_pre_cfg_get(unit, vid);

    if ((!vlan_exist) && (!pre_cfg)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_IDs, vid));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    if (ltsw_feature(unit, LTSW_FT_MPLS)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, MPLSs, &data));
    }
    if (0 == data) {
        control->flags |= BCM_VLAN_MPLS_DISABLE;
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IF_CLASS_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, CLASS_IDs, &data));
        control->if_class = (int)data;
        control->flags |= BCM_VLAN_L2_CLASS_ID_ONLY;
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_MODE_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, FORWARDING_BEHAVIORs, &data));
        control->forwarding_mode = (bcm_vlan_forward_t) data;
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, FIDs, &data));
        control->forwarding_vlan = (bcm_vlan_t)data;
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, L3_IIF_IDs, &data));
        control->ingress_if = (bcm_if_t)data;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, VLAN_PROFILE_IDs, &data));
    profile_index = data;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_get(unit, &vp, profile_index));

    vpt = &vp;
    control->flags |= vpt->flags;
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK) {
        control->outer_tpid = vpt->outer_tpid;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IP6_MCAST_FLOOD_MODE_MASK) {
        control->ip6_mcast_flood_mode = vpt->ip6_mcast_flood_mode;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_IP4_MCAST_FLOOD_MODE_MASK) {
        control->ip4_mcast_flood_mode = vpt->ip4_mcast_flood_mode;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_L2_MCAST_FLOOD_MODE_MASK) {
        control->l2_mcast_flood_mode = vpt->l2_mcast_flood_mode;
    }
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_PROTOCOL_PKT_MASK) {
        sal_memcpy(&control->protocol_pkt,
            &vpt->protocol_pkt, sizeof(bcm_vlan_protocol_packet_ctrl_t));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, IGMP_MLD_SNOOPINGs, &data));
    if (0 == data) {
        control->flags |= BCM_VLAN_IGMP_SNOOP_DISABLE;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_port_action_set(int unit,
                              bcm_port_t port,
                              bcm_vlan_action_set_t *action)
{
    bcm_port_t port_out;
    ltsw_vlan_tag_action_profile_t profile;
    int new_profile_idx, old_profile_idx;
    bcm_vlan_action_set_t tmp_action;

    SHR_FUNC_ENTER(unit);

    port_out = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_action_validate(unit, &port_out, action));

    vlan_tag_action_profile_set(&profile, action);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_tag_action_profile_add(unit,
                                         &profile,
                                         &new_profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_ing_vlan_action_get(unit, port_out, &tmp_action,
                                            &old_profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_ing_vlan_action_set(unit, port_out, action,
                                            new_profile_idx));

    if (old_profile_idx > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_tag_action_profile_delete(unit, old_profile_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_port_action_get(int unit,
                              bcm_port_t port,
                              bcm_vlan_action_set_t *action)
{
    bcm_port_t port_out;
    int old_profile_idx;
    ltsw_vlan_tag_action_profile_t profile;

    SHR_FUNC_ENTER(unit);

    port_out = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_action_validate(unit, &port_out, NULL));

    bcm_vlan_action_set_t_init(action);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_ing_vlan_action_get(unit, port_out, action,
                                            &old_profile_idx));

    sal_memset(&profile, 0, sizeof(profile));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_tag_action_profile_entry_get(unit, &profile, old_profile_idx));

    vlan_tag_action_set(&profile, action);

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_port_action_reset(int unit,
                                bcm_port_t port)
{
    bcm_port_t port_out;
    bcm_vlan_action_set_t action, tmp_action;
    int def_profile_idx, old_profile_idx;

    SHR_FUNC_ENTER(unit);

    port_out = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_action_validate(unit, &port_out, NULL));

    bcm_vlan_action_set_t_init(&action);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_ing_vlan_action_get(unit, port_out, &tmp_action,
                                            &old_profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_tag_action_default_profile_add(unit, &def_profile_idx));

    action.new_outer_vlan = BCM_VLAN_DEFAULT;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_ing_vlan_action_set(unit, port_out, &action,
                                            def_profile_idx));
    if (old_profile_idx > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_tag_action_profile_delete(unit, old_profile_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_egr_vlan_port_action_set(int unit,
                                  bcm_port_t port,
                                  bcm_vlan_action_set_t *action)
{
    bcm_port_t port_out;
    ltsw_vlan_tag_action_profile_t profile;
    int new_profile_idx, old_profile_idx;
    bcm_vlan_action_set_t tmp_action;

    SHR_FUNC_ENTER(unit);

    port_out = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_action_validate(unit, &port_out, action));

    vlan_tag_action_profile_set(&profile, action);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_tag_action_profile_add(unit,
                                         &profile,
                                         &new_profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_egr_vlan_action_get(unit, port_out, &tmp_action,
                                            &old_profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_egr_vlan_action_set(unit, port_out, action,
                                            new_profile_idx));

    if (old_profile_idx > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_tag_action_profile_delete(unit, old_profile_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_egr_vlan_port_action_get(int unit,
                                  bcm_port_t port,
                                  bcm_vlan_action_set_t *action)
{
    bcm_port_t port_out;
    int old_profile_idx;
    ltsw_vlan_tag_action_profile_t profile;

    SHR_FUNC_ENTER(unit);

    port_out = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_action_validate(unit, &port_out, NULL));

    bcm_vlan_action_set_t_init(action);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_egr_vlan_action_get(unit, port_out, action,
                                            &old_profile_idx));

    sal_memset(&profile, 0, sizeof(profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_tag_action_profile_entry_get(unit, &profile, old_profile_idx));

    vlan_tag_action_set(&profile, action);

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_egr_vlan_port_action_reset(int unit,
                                    bcm_port_t port)
{
    bcm_port_t port_out;
    bcm_vlan_action_set_t action;
    int old_profile_idx;

    SHR_FUNC_ENTER(unit);

    port_out = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_action_validate(unit, &port_out, NULL));

    bcm_vlan_action_set_t_init(&action);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_egr_vlan_action_get(unit, port_out, &action,
                                            &old_profile_idx));

    if (old_profile_idx > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_tag_action_profile_delete(unit, old_profile_idx));

        bcm_vlan_action_set_t_init(&action);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_egr_vlan_action_set(unit, port_out, &action, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_mcast_flood_set(int unit,
                              bcm_vlan_t vid,
                              bcm_vlan_mcast_flood_t mode)
{
    int dunit = 0;
    uint64_t data;
    int vlan_exist, profile_index, new_profile_index;
    ltsw_vlan_profile_t profile;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vid);

    vlan_exist = bcmint_vlan_info_exist_check(unit, vid);
    if (!vlan_exist) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&profile, 0, sizeof(ltsw_vlan_profile_t));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_IDs, vid));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, VLAN_PROFILE_IDs, &data));
    profile_index = (int)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_get(unit, &profile, profile_index));
    profile.ip6_mcast_flood_mode = mode;
    profile.ip4_mcast_flood_mode = mode;
    profile.l2_mcast_flood_mode  = mode;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_add(unit, &profile, &new_profile_index));

    if (new_profile_index != profile_index) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_profile_entry_delete(unit, profile_index));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, VLAN_PROFILE_IDs,
                                   new_profile_index));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_mcast_flood_get(int unit,
                              bcm_vlan_t vid,
                              bcm_vlan_mcast_flood_t *mode)
{
    int dunit = 0;
    uint64_t data;
    int vlan_exist, profile_index;
    ltsw_vlan_profile_t profile;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vid);

    vlan_exist = bcmint_vlan_info_exist_check(unit, vid);
    if (!vlan_exist) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&profile, 0, sizeof(ltsw_vlan_profile_t));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_IDs, vid));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, VLAN_PROFILE_IDs, &data));
    profile_index = (int)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_get(unit, &profile, profile_index));

    *mode = profile.l2_mcast_flood_mode;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_block_set(int unit,
                        bcm_vlan_t vid,
                        bcm_vlan_block_t *block)
{
    bcm_pbmp_t array[VLAN_BLOCK_MASK_SEL_COUNT];
    ltsw_vlan_block_t vlan_block;
    ltsw_vlan_profile_t profile;
    int profile_index, new_profile_index, dunit = 0;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;
    int vlan_exist;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vid);

    vlan_exist = bcmint_vlan_info_exist_check(unit, vid);
    if (!vlan_exist) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    BCM_PBMP_ASSIGN(array[0], block->broadcast);
    BCM_PBMP_ASSIGN(array[1], block->unknown_unicast);
    BCM_PBMP_ASSIGN(array[2], block->unknown_multicast);
    BCM_PBMP_ASSIGN(array[3], block->known_multicast);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_bitmap_min(unit, array, VLAN_BLOCK_MASK_SEL_COUNT,
                               &vlan_block.mask_a));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_bitmap_clear(unit, array, VLAN_BLOCK_MASK_SEL_COUNT,
                                 vlan_block.mask_a));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_bitmap_min(unit, array, VLAN_BLOCK_MASK_SEL_COUNT,
                               &vlan_block.mask_b));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_select_get(unit, block->broadcast, vlan_block.mask_a,
                               vlan_block.mask_b,
                               &vlan_block.broadcast_mask_sel));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_select_get(unit, block->unknown_unicast, vlan_block.mask_a,
                               vlan_block.mask_b,
                               &vlan_block.unknown_unicast_mask_sel));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_select_get(unit, block->unknown_multicast, vlan_block.mask_a,
                               vlan_block.mask_b,
                               &vlan_block.unknown_mulitcast_mask_sel));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_select_get(unit, block->known_multicast, vlan_block.mask_a,
                               vlan_block.mask_b,
                               &vlan_block.known_mulitcast_mask_sel));

    sal_memset(&profile, 0, sizeof(ltsw_vlan_profile_t));
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_IDs, vid));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, VLAN_PROFILE_IDs, &data));
    profile_index = (int)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_get(unit, &profile, profile_index));

    profile.block = vlan_block;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_add(unit, &profile, &new_profile_index));

    if (new_profile_index != profile_index) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_profile_entry_delete(unit, profile_index));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, VLAN_PROFILE_IDs,
                                   new_profile_index));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_block_get(int unit,
                        bcm_vlan_t vid,
                        bcm_vlan_block_t *block)
{
    ltsw_vlan_block_t *vlan_block;
    int profile_index, dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;
    ltsw_vlan_profile_t profile;
    int vlan_exist;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vid);

    vlan_exist = bcmint_vlan_info_exist_check(unit, vid);
    if (!vlan_exist) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&profile, 0, sizeof(ltsw_vlan_profile_t));
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, VLAN_IDs, vid));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, VLAN_PROFILE_IDs, &data));
    profile_index = (int)data;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_get(unit, &profile, profile_index));

    vlan_block = &profile.block;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_mask_create(unit, vlan_block->mask_a,
                                vlan_block->mask_b,
                                vlan_block->broadcast_mask_sel,
                                &block->broadcast));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_mask_create(unit, vlan_block->mask_a,
                                vlan_block->mask_b,
                                vlan_block->unknown_unicast_mask_sel,
                                &block->unknown_unicast));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_mask_create(unit, vlan_block->mask_a,
                                vlan_block->mask_b,
                                vlan_block->unknown_mulitcast_mask_sel,
                                &block->unknown_multicast));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_block_mask_create(unit, vlan_block->mask_a,
                                vlan_block->mask_b,
                                vlan_block->known_mulitcast_mask_sel,
                                &block->known_multicast));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_control_set(int unit, bcm_vlan_control_t type, int arg)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE, insert = FALSE;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_CONTROLs, &eh));
    rv = bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        insert = TRUE;
    }

    switch (type) {
        case bcmVlanDropUnknown:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, UNKNOWN_VLAN_TO_CPUs, !arg));
            break;
        case bcmVlanShared:
            arg = (arg ? 1 : 0);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, GLOBAL_SHARED_VLANs, arg));
            break;
        case bcmVlanSharedID:
            if (arg > BCM_VLAN_MAX || arg < BCM_VLAN_MIN) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, GLOBAL_FIDs, arg));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (insert == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh,
                                  BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh,
                                  BCMLT_OPCODE_UPDATE,
                                  BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_control_get(int unit, bcm_vlan_control_t type, int *arg)
{
    int rv = SHR_E_NONE;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_CONTROLs, &eh));
    rv = bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(rv);
    }

    switch (type) {
        case bcmVlanDropUnknown:
            if (rv == SHR_E_NONE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, UNKNOWN_VLAN_TO_CPUs, &data));
            }
            *arg = !(int)data;
            break;
        case bcmVlanShared:
            if (rv == SHR_E_NONE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, GLOBAL_SHARED_VLANs, &data));
            }
            *arg = (int)data;
            break;
        case bcmVlanSharedID:
            if (rv == SHR_E_NONE) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, GLOBAL_FIDs, &data));
            }
            *arg = (int)data;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_control_port_set(int unit,
                               bcm_port_t port,
                               bcm_vlan_control_port_t type,
                               int arg)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port_out;

    SHR_FUNC_ENTER(unit);

    if (port == -1) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp));

        BCM_PBMP_ITER(pbmp, port_out) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_control_port_set(unit, port_out, type, arg));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_control_port_set(unit, port, type, arg));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_control_port_get(int unit,
                               bcm_port_t port,
                               bcm_vlan_control_port_t type,
                               int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_control_port_get(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a VLAN cross connect entry.
 *
 * This function is used to add a VLAN cross connect entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] port_1            First port in the cross-connect.
 * \param [in] port_2            Second port in the cross-connect.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_cross_connect_add(int unit,
                                bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan,
                                bcm_gport_t port_1, bcm_gport_t port_2)
{
    int dunit = 0;
    int id = 0;
    bcm_module_t modid = 0;
    bcm_port_t local_port_1 = 0;
    bcm_port_t local_port_2 = 0;
    bcm_trunk_t tgid_1 = -1;
    bcm_trunk_t tgid_2 = -1;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (BCM_VLAN_INVALID != inner_vlan) {
        SHR_ERR_EXIT(BCM_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_resolve(unit, port_1, &modid,
                                      &local_port_1, &tgid_1, &id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_resolve(unit, port_2, &modid,
                                      &local_port_2, &tgid_2, &id));

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L2_FDB_VLAN_CCs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, outer_vlan));

    if (tgid_1 == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_1_IS_TRUNKs, 0));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_1_MODPORTs, local_port_1));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_1_IS_TRUNKs, 1));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_1_TRUNK_IDs, tgid_1));
    }

    if (tgid_2 == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_2_IS_TRUNKs, 0));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_2_MODPORTs, local_port_2));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_2_IS_TRUNKs, 1));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_2_TRUNK_IDs, tgid_2));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a VLAN cross connect entry.
 *
 * This function is used to delete a VLAN cross connect entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_cross_connect_delete(int unit,
                                   bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan)
{
    int dunit = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (BCM_VLAN_INVALID != inner_vlan) {
        SHR_ERR_EXIT(BCM_E_UNAVAIL);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L2_FDB_VLAN_CCs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, outer_vlan));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all VLAN cross connect entries.
 *
 * This function is used to delete all VLAN cross connect entries.
 *
 * \param [in] unit              Unit number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_cross_connect_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, L2_FDB_VLAN_CCs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse VLAN cross connect entries.
 *
 * This function is used to traverse VLAN cross connect entries.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data to be passed to callback function.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_cross_connect_traverse(int unit,
                                     bcm_vlan_cross_connect_traverse_cb cb,
                                     void *user_data)
{
    int rv = SHR_E_NONE;
    int dunit = 0;
    uint64_t vid = 0;
    uint64_t dest_1 = 0;
    uint64_t dest_2 = 0;
    uint64_t is_trunk_1 = 0;
    uint64_t is_trunk_2 = 0;
    bcm_module_t modid = 0;
    bcm_gport_t gport_1 = BCM_GPORT_INVALID;
    bcm_gport_t gport_2 = BCM_GPORT_INVALID;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (cb == NULL) {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &modid));

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L2_FDB_VLAN_CCs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, VLAN_IDs, &vid));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, DST_1_IS_TRUNKs, &is_trunk_1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, DST_2_IS_TRUNKs, &is_trunk_2));

        if (is_trunk_1 == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, DST_1_MODPORTs, &dest_1));
            BCM_GPORT_MODPORT_SET(gport_1, modid, dest_1);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, DST_1_TRUNK_IDs, &dest_1));
            BCM_GPORT_TRUNK_SET(gport_1, dest_1);
        }

        if (is_trunk_2 == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, DST_2_MODPORTs, &dest_2));
            BCM_GPORT_MODPORT_SET(gport_2, modid, dest_2);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(entry_hdl, DST_2_TRUNK_IDs, &dest_2));
            BCM_GPORT_TRUNK_SET(gport_2, dest_2);
        }

        rv = cb(unit, vid, BCM_VLAN_INVALID, gport_1, gport_2, user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given vlan.
 *
 * This function is used to attach flex counter to the given vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_stat_attach(int unit, bcm_vlan_t vid,
                          bcmi_ltsw_flexctr_counter_info_t *info)
{
    int dunit = 0;
    uint64_t flex_ctr_action = 0;
    const char *data_field = NULL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (sal_strcmp(info->table_name, VLANs)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (info->direction == BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        data_field = CTR_ING_EFLEX_ACTIONs;
    } else if (info->direction == BCMI_LTSW_FLEXCTR_DIR_EGRESS) {
        data_field = CTR_EGR_EFLEX_ACTIONs;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Allocate entry container. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, info->table_name, &entry_hdl));

    /* Add VLAN ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, vid));

    /* Find VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Get current flex counter action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, data_field, &flex_ctr_action));

    /* Already attached a flex counter action. */
    if (flex_ctr_action != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    /* Clear entry content. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(entry_hdl));

    /* Add VLAN ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, vid));

    flex_ctr_action = (uint64_t)info->action_index;
    /* Attach flex counter action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, data_field, flex_ctr_action));

    /* Update VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries to the given vlan.
 *
 * This function is used to detach flex counter to the given vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_stat_detach(int unit, bcm_vlan_t vid,
                          bcmi_ltsw_flexctr_counter_info_t *info)
{
    int dunit = 0;
    uint64_t flex_ctr_action = 0;
    const char *data_field = NULL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (sal_strcmp(info->table_name, VLANs)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (info->direction == BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        data_field = CTR_ING_EFLEX_ACTIONs;
    } else if (info->direction == BCMI_LTSW_FLEXCTR_DIR_EGRESS) {
        data_field = CTR_EGR_EFLEX_ACTIONs;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Allocate entry container. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, info->table_name, &entry_hdl));

    /* Add VLAN ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, vid));

    /* Find VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Get current flex counter action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, data_field, &flex_ctr_action));

    /* Current flex counter action is not the detached one. */
    if (flex_ctr_action != (uint64_t)info->action_index) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Clear entry content. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(entry_hdl));

    /* Add VLAN ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, vid));

    /* Detach flex counter action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, data_field,
                               BCMI_LTSW_FLEXCTR_ACTION_INVALID));

    /* Update VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set ingress vlan qos id.
 *
 * This function is used to set ingress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] id                vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_ing_qos_set(int unit, bcm_vlan_t vid, int id)
{
    int dunit = 0;
    uint64_t profile_idx, new_profile_idx = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    ltsw_vlan_profile_t profile;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vid);

    sal_memset(&profile, 0, sizeof(ltsw_vlan_profile_t));

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, vid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, VLAN_PROFILE_IDs, &profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_get(unit, &profile, (int)profile_idx));

    if (id > 0) {
        profile.profile_flags |= LTSW_VLAN_PROFILE_TRUST_DOT1P;

    } else {
        profile.profile_flags &= ~LTSW_VLAN_PROFILE_TRUST_DOT1P;
    }

    profile.trust_dot1p_ptr = id;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_add(unit, &profile, (int *)&new_profile_idx));
    if (new_profile_idx != profile_idx) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_profile_entry_delete(unit, profile_idx));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add
                (entry_hdl, VLAN_PROFILE_IDs, new_profile_idx));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ingress vlan qos id.
 *
 * This function is used to get ingress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] id               vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_ing_qos_get(int unit, bcm_vlan_t vid, int *id)
{
    int dunit = 0;
    uint64_t profile_idx;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    ltsw_vlan_profile_t profile;

    SHR_FUNC_ENTER(unit);

    VLAN_ID_CHECK(vid);

    sal_memset(&profile, 0, sizeof(ltsw_vlan_profile_t));

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, vid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, VLAN_PROFILE_IDs, &profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_entry_get(unit, &profile, (int)profile_idx));

    if (profile.profile_flags & LTSW_VLAN_PROFILE_TRUST_DOT1P) {
        *id = profile.trust_dot1p_ptr;
    } else {
        *id = 0;
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set egress vlan qos id.
 *
 * This function is used to set egress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] id                vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_egr_qos_set(int unit, bcm_vlan_t vid, int id)
{
    int rv = SHR_E_NONE;
    int dunit = 0;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, vid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(
            entry_hdl, REMARK_DOT1Ps, (id == 0) ? 0 : 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(
            entry_hdl, REMARK_CFIs, (id == 0) ? 0 : 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(
            entry_hdl, PHB_EGR_VLAN_INT_PRI_TO_L2_OTAG_IDs, id));

    rv = bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress vlan qos id.
 *
 * This function is used to get egress vlan qos id.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] id               vlan qos id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_egr_qos_get(int unit, bcm_vlan_t vid, int *id)
{
    int dunit = 0;
    uint64_t data;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, VLAN_IDs, vid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get
            (entry_hdl, PHB_EGR_VLAN_INT_PRI_TO_L2_OTAG_IDs, &data));
    *id = data;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set vlan switch control configurations.
 *
 * \param [in] unit              Unit number.
 * \param [in] type              The desired configuration parameter.
 * \param [in] value             The value to set the parameter.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_internal_control_set(int unit,
                                   bcmi_ltsw_vlan_control_t type,
                                   int value)
{
    int rv = SHR_E_NONE;
    int dunit = 0;
    int insert = FALSE;
    const char *table_name = NULL;
    const char *field_name = NULL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    switch (type) {
        case bcmiVlanControlUnknownVlanToCpu:
            table_name = VLAN_CONTROLs;
            field_name = UNKNOWN_VLAN_TO_CPUs;
            break;
        case bcmiVlanControlSharedVlanMismatchToCpu:
            table_name = VLAN_CONTROLs;
            field_name = PRIVATE_VLAN_MISMATCH_TO_CPUs;
            break;
        case bcmiVlanControlBpduInvalidVlanDrop:
            table_name = VLAN_CONTROLs;
            field_name = DROP_INVALID_VLAN_BPDUs;
            break;
        case bcmiVlanControlSharedVlanEnable:
            table_name = VLAN_CONTROLs;
            field_name = SHARED_VLANs;
            break;
        case bcmiVlanControlSnapNonZeroOui:
            table_name = VLAN_CONTROLs;
            field_name = MATCH_NON_ZERO_OUI_SNAPs;
            break;
        case bcmiVlanControlEgressStgCheckIngressEnable:
            table_name = VLAN_CONTROLs;
            field_name = EGR_STG_CHECKs;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    rv = bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        insert = TRUE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, field_name, value));

    if (insert == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE,
                                  BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get vlan switch control configurations.
 *
 * \param [in] unit              Unit number.
 * \param [in] type              The desired configuration parameter.
 * \param [in] value             The value pointer to get the parameter.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xgs_ltsw_vlan_internal_control_get(int unit,
                                   bcmi_ltsw_vlan_control_t type,
                                   int *value)
{
    int rv = SHR_E_NONE;
    int dunit = 0;
    uint64_t data = 0;
    const char *table_name = NULL;
    const char *field_name = NULL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    switch (type) {
        case bcmiVlanControlUnknownVlanToCpu:
            table_name = VLAN_CONTROLs;
            field_name = UNKNOWN_VLAN_TO_CPUs;
            break;
        case bcmiVlanControlSharedVlanMismatchToCpu:
            table_name = VLAN_CONTROLs;
            field_name = PRIVATE_VLAN_MISMATCH_TO_CPUs;
            break;
        case bcmiVlanControlBpduInvalidVlanDrop:
            table_name = VLAN_CONTROLs;
            field_name = DROP_INVALID_VLAN_BPDUs;
            break;
        case bcmiVlanControlSharedVlanEnable:
            table_name = VLAN_CONTROLs;
            field_name = SHARED_VLANs;
            break;
        case bcmiVlanControlSnapNonZeroOui:
            table_name = VLAN_CONTROLs;
            field_name = MATCH_NON_ZERO_OUI_SNAPs;
            break;
        case bcmiVlanControlEgressStgCheckIngressEnable:
            table_name = VLAN_CONTROLs;
            field_name = EGR_STG_CHECKs;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    rv = bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, field_name, &data));
    } else if (rv != SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(rv);
    }
    *value = (int)data;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_stg_set(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t stg)
{
    int                  dunit     = 0;
    const char *vlan_tbl_name;
    const char *vlan_id_fld_name;
    const char *stg_id_fld_name;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Allocate entry container. */
    dunit = bcmi_ltsw_dev_dunit(unit);

    vlan_tbl_name = VLANs;
    vlan_id_fld_name = VLAN_IDs;
    stg_id_fld_name = VLAN_STG_IDs;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, vlan_tbl_name, &entry_hdl));

    /* Add VLAN ID. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, vlan_id_fld_name, vid));

    /* Update STG ID. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, stg_id_fld_name, stg));

    /* Update VLAN entry. */
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit,
                              entry_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_stg_get(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t *stg)
{
    int                  dunit     = 0;
    uint64_t             stg_id    = 0;
    const char *vlan_tbl_name;
    const char *vlan_id_fld_name;
    const char *stg_id_fld_name;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Allocate entry container. */
    dunit = bcmi_ltsw_dev_dunit(unit);

    vlan_tbl_name = VLANs;
    vlan_id_fld_name = VLAN_IDs;
    stg_id_fld_name = VLAN_STG_IDs;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, vlan_tbl_name, &entry_hdl));

    /* Add VLAN ID. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, vlan_id_fld_name, vid));

    /* Update VLAN entry. */
    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit,
                              entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    /* Get STG ID. */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, stg_id_fld_name, &stg_id));

    *stg = (bcm_stg_t)stg_id;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_vlan_traverse(int unit,
                       bcmi_ltsw_vlan_traverse_cb cb, void *user_data)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0, rv;
    uint64_t data;
    bcmi_ltsw_vlan_info_t info;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLANs, &eh));

    sal_memset(&info, 0, sizeof(bcmi_ltsw_vlan_info_t));

    while ((rv = bcmi_lt_entry_commit(unit, eh,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, VLAN_IDs, &data));
        info.vid = (bcm_vlan_t)data;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, VLAN_STG_IDs, &data));
        info.stg = (int)data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cb(unit, &info, user_data));
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}
