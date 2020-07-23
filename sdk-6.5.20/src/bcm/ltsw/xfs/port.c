/*! \file port.c
 *
 * Port Driver for XFS chips.
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
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/port_int.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/stg.h>
#include <bcm_int/ltsw/trunk.h>
#include <bcm_int/ltsw/l2.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/mpls.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/flexstate.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/stack.h>
#include <bcm_int/ltsw/xfs/port.h>
#include <bcm_int/ltsw/oob.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/rate.h>
#include <bcm_int/ltsw/obm.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/mbcm/port.h>

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_PORT

#define L2_IF_INVALID       0xFFFF
#define L2_IF_LOCK(u)       \
        sal_mutex_take(l2_if_pool[(u)].lock, SAL_MUTEX_FOREVER)
#define L2_IF_UNLOCK(u)     \
        sal_mutex_give(l2_if_pool[(u)].lock)

typedef struct l2_if_node_s {
    uint16_t        next;
} l2_if_node_t;

typedef struct l2_if_pool_s {
    sal_mutex_t     lock;
    uint16_t        head;
    uint16_t        size;
    uint16_t        free;
    l2_if_node_t    *node;
} l2_if_pool_t;

static l2_if_pool_t l2_if_pool[BCM_MAX_NUM_UNITS];

#define DIR_ING     0
#define DIR_EGR     1
#define DIR_NUM     2

typedef struct xfs_port_pvlan_s {
    int     vfi;
} xfs_port_pvlan_t;

typedef struct xfs_port_info_s {
    int                 sys_port;
    int                 l2_if;
    xfs_port_pvlan_t    pvlan;
    int                 port_group[DIR_NUM];
} xfs_port_info_t;

typedef struct xfs_port_egr_mask_profile_s {
    bcm_pbmp_t      pbmp;   /* allowed ports */
} xfs_port_egr_mask_profile_t;

#define BLOCK_MASK_SECTION_NUM      4
typedef struct block_mask_section_info_s {
    int traffics[BLOCK_MASK_SECTION_NUM];
} block_mask_section_info_t;

#define MAX_NUM_TPID            4
#define HG3_ETYPE_DEFAULT       0x2bc
#define PROFILE_IFP             1

static int
xfs_pc_invert(int unit, int in_val, int *out_val);

static int
xfs_pc_cml_set(int unit, int in_val, int *out_val);

static int
xfs_pc_cml_get(int unit, int in_val, int *out_val);

static int
xfs_pc_sample_egress_en_set(int unit, int in_val, int *out_val);

static int
xfs_pc_sample_egress_rate_set(int unit, int val, int *out_val);

static int
xfs_pc_sample_egress_rate_get(int unit, int val, int *out_val);

static int
xfs_vlan_phb_str_set(int unit, int in_val, int *out_val);

static int
xfs_vlan_phb_str_get(int unit, int in_val, int *out_val);

static port_control_map_t port_control_port_map[] = {
    {bcmPortControlBridge, BCMI_PT_BRIDGE, NULL, NULL},
    {bcmPortControlForwardStaticL2MovePkt, BCMI_PT_STATIC_MOVE_DROP,
         xfs_pc_invert, xfs_pc_invert},
    {bcmPortControlIP4TunnelTermEnable, BCMI_PT_IPV4_TUNNEL_EN, NULL, NULL},
    {bcmPortControlIP6TunnelTermEnable, BCMI_PT_IPV6_TUNNEL_EN, NULL, NULL},
    {bcmPortControlLoadBalancingNumber, BCMI_PT_LB_HASH_PORT_LB_NUM, NULL, NULL},
    {bcmPortControlL2Learn, BCMI_PT_CML_NEW,
         xfs_pc_cml_set, xfs_pc_cml_get},
    {bcmPortControlL2Move, BCMI_PT_CML_MOVE,
         xfs_pc_cml_set, xfs_pc_cml_get},
    {bcmPortControlL3Ingress, BCMI_PT_ING_L3_IIF, NULL, NULL},
    {bcmPortControlMpls, BCMI_PT_MPLS, NULL, NULL},
    {bcmPortControlVxlanEnable, BCMI_PT_VXLAN_EN, NULL, NULL},
    {bcmPortControlVxlanTunnelbasedVnId, BCMI_PT_VN_ID_LOOKUP_CTRL, NULL, NULL},
    {bcmPortControlPacketTimeStampInsertRx, BCMI_PT_ING_RXTS_INSERT, NULL, NULL},
    {bcmPortControlPacketTimeStampIngressInsertTx, BCMI_PT_ING_TXTS_INSERT, NULL, NULL},
    {bcmPortControlPacketTimeStampIngressInsertTxCancel, BCMI_PT_ING_TXTS_CANCEL, NULL, NULL},
    {bcmPortControlPacketTimeStampInsertTx, BCMI_PT_EGR_TXTS_INSERT, NULL, NULL},
    {bcmPortControlPacketTimeStampDestPortInsertCancel, BCMI_PT_EGR_TS_CANCEL,
     NULL, NULL},
    {bcmPortControlPacketTimeStampRxId, BCMI_PT_ING_TS_OID, NULL, NULL},
    {bcmPortControlPacketTimeStampTxId, BCMI_PT_EGR_TS_OID, NULL, NULL},
    {bcmPortControlPacketTimeStampByteMeterAdjust, BCMI_PT_EGR_TS_ADJUST,
     NULL, NULL},
    {bcmPortControl1588P2PDelay, BCMI_PT_1588_LINK_DELAY, NULL, NULL},
    {bcmPortControlSampleEgressEnable, BCMI_PT_EGR_SFLOW_EN,
         xfs_pc_sample_egress_en_set, NULL},
    {bcmPortControlSampleEgressRate, BCMI_PT_EGR_SFLOW_RATE,
         xfs_pc_sample_egress_rate_set, xfs_pc_sample_egress_rate_get},
    {bcmPortControlIngressSampleProfile, BCMI_PT_SAMPLE_ID, NULL, NULL},
    {bcmPortControlQosRemarkResolution, BCMI_PT_PHB_VLAN_STR,
         xfs_vlan_phb_str_set, xfs_vlan_phb_str_get},
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
    {bcmPortControlRuntThreshold, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlMmuTrafficEnable, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
    {bcmPortControlTxStall, BCMI_PT_ZERO, NULL, NULL, PORT_CTRL_PM},
};

#define TRAFFIC_TYPE_NUM    5

static const xfs_ltsw_port_db_t *xfs_port_db[BCM_MAX_NUM_UNITS];

typedef struct xfs_port_ctrl_info_s {
    xfs_port_info_t             *port;
    bcmint_port_xfs_ha_info_t   *port_ha;
    ltsw_port_profile_info_t    profile_egr_mask;
    block_mask_section_info_t   block_mask_section;
} xfs_port_ctrl_info_t;

static xfs_port_ctrl_info_t *xfs_port_info[BCM_MAX_NUM_UNITS];

static inline xfs_port_info_t *xfs_port(int unit, int port)
{
    return &(xfs_port_info[unit]->port[port]);
}

static inline bcmint_port_xfs_ha_info_t *xfs_port_ha(int unit, int port)
{
    return &(xfs_port_info[unit]->port_ha[port]);
}

static inline ltsw_port_profile_info_t *profile_egr_mask(int unit)
{
    return &(xfs_port_info[unit]->profile_egr_mask);
}

static inline block_mask_section_info_t *block_mask_section(int unit)
{
    return &(xfs_port_info[unit]->block_mask_section);
}


/******************************************************************************
 * Private functions
 */

/*!
 * \brief Get ingress default VLAN priority and cfi of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] prio VLAN priority.
 * \param [out] cfi VLAN CFI.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_default_prio_cfi_get(int unit, int port, int *prio, int *cfi)
{
    int prio_cfi;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_DEFAULT_PRI, &prio_cfi));

    if (prio) {
        *prio = prio_cfi >> 1;
    }
    if (cfi) {
        *cfi = prio_cfi & 1;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set ingress default VLAN priority and cfi of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] prio VLAN priority.
 * \param [in] cfi VLAN CFI.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_default_prio_cfi_set(int unit, int port, int prio, int cfi)
{
    int prio_cfi;

    SHR_FUNC_ENTER(unit);

    prio_cfi = (prio << 1) | (cfi & 1);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_DEFAULT_PRI, prio_cfi));

exit:
    SHR_FUNC_EXIT();
}

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
xfs_pc_invert(int unit, int in_val, int *out_val)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out_val, SHR_E_PARAM);

    *out_val = !(in_val);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to convert cml flags from S/W to H/W.
 *
 * \param [in] unit Unit number.
 * \param [in] in_val Input value.
 * \param [out] out_val Output value.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_pc_cml_set(int unit, int in_val, int *out_val)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out_val, SHR_E_PARAM);

    *out_val = 0;
    if (!(in_val & BCM_PORT_LEARN_FWD)) {
        *out_val |= (1 << 0);
    }
    if (in_val & BCM_PORT_LEARN_CPU) {
        *out_val |= (1 << 1);
    }
    if (in_val & BCM_PORT_LEARN_ARL) {
        *out_val |= (1 << 2);
    }
    if (in_val & BCM_PORT_LEARN_PENDING) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to convert cml flags from H/W to S/W.
 *
 * \param [in] unit Unit number.
 * \param [in] in_val Input value.
 * \param [out] out_val Output value.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_pc_cml_get(int unit, int in_val, int *out_val)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out_val, SHR_E_PARAM);

    *out_val = 0;
    if (!(in_val & (1 << 0))) {
       *out_val |= BCM_PORT_LEARN_FWD;
    }
    if (in_val & (1 << 1)) {
       *out_val |= BCM_PORT_LEARN_CPU;
    }
    if (in_val & (1 << 2)) {
       *out_val |= BCM_PORT_LEARN_ARL;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to convert egress sflow enable status.
 *
 * \param [in] unit Unit number.
 * \param [in] in_val Input value.
 * \param [out] out_val Output value.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_pc_sample_egress_en_set(int unit, int in_val, int *out_val)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out_val, SHR_E_PARAM);

    *out_val = in_val ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to configure port-based egress sample rate.
 *
 * \param [in] unit Unit number.
 * \param [in] in_val Input value.
 * \param [out] out_val Output value.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_pc_sample_egress_rate_set(int unit, int val, int *out_val)
{
    int range_max;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out_val, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_egress_sampler_rate_range_max(unit, &range_max));

    /*
     * rate  - (IN)Every 1/rate a packet will be sampled.
     *
     * 1: indicating sample all.
     * 0: indicating sample none.
     */
    if (val > 0) {
        /* Set sampling threshold. */
        if (val == 1) {
            /* Needs special handling to avoid register overflow. */
            *out_val = range_max;
        } else if (val <= range_max) {
            /* Calculate threshold based on sampling rate. */
            *out_val = (range_max + 1) / val;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        *out_val = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to get configure of port-based egress sample rate.
 *
 * \param [in] unit Unit number.
 * \param [in] in_val Input value.
 * \param [out] out_val Output value.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_pc_sample_egress_rate_get(int unit, int val, int *out_val)
{
    int range_max;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out_val, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_mirror_egress_sampler_rate_range_max(unit, &range_max));
    if (val) {
        *out_val = (range_max + 1) / val;
    } else {
        *out_val = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to convert VLAN PHB strength from S/W to H/W.
 *
 * \param [in] unit Unit number.
 * \param [in] in_val Input value.
 * \param [out] out_val Output value.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_vlan_phb_str_set(int unit, int in_val, int *out_val)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out_val, SHR_E_PARAM);

    if (in_val == BCM_PORT_QOS_SERIAL_RESOLUTION) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sbr_fld_value_get
                (unit, BCMI_LTSW_SBR_TH_EGR_L2_OIF,
                 BCMI_LTSW_SBR_FT_DOT1P_PTR_HIGH, out_val));
    } else if (in_val == BCM_PORT_QOS_LAYERED_RESOLUTION) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sbr_fld_value_get
                (unit, BCMI_LTSW_SBR_TH_EGR_L2_OIF,
                 BCMI_LTSW_SBR_FT_DOT1P_PTR, out_val));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function to convert VLAN PHB strength from H/W to S/W.
 *
 * \param [in] unit Unit number.
 * \param [in] in_val Input value.
 * \param [out] out_val Output value.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_vlan_phb_str_get(int unit, int in_val, int *out_val)
{
    int str;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(out_val, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_fld_value_get
            (unit, BCMI_LTSW_SBR_TH_EGR_L2_OIF,
             BCMI_LTSW_SBR_FT_DOT1P_PTR, &str));
    if (str == in_val) {
        *out_val = BCM_PORT_QOS_LAYERED_RESOLUTION;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_fld_value_get
            (unit, BCMI_LTSW_SBR_TH_EGR_L2_OIF,
             BCMI_LTSW_SBR_FT_DOT1P_PTR_HIGH, &str));
    if (str == in_val) {
        *out_val = BCM_PORT_QOS_SERIAL_RESOLUTION;
        SHR_EXIT();
    }

    SHR_ERR_EXIT(SHR_E_CONFIG);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure bit of L2_PROCESS_CTRL.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] bit Bit position.
 * \param [in] val 0 or 1.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_l2_ctrl_set(int unit, bcm_port_t port, int bit, int val)
{
    int ctrl;

    SHR_FUNC_ENTER(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_EGR_L2_PROCESS_CTRL, &ctrl));

    ctrl = (ctrl & ~(1 << bit)) | ((val & 1) << bit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set
            (unit, port, BCMI_PT_EGR_L2_PROCESS_CTRL, ctrl));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrive bit of L2_PROCESS_CTRL.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] bit Bit position.
 * \param [in] val 0 or 1.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_l2_ctrl_get(int unit, bcm_port_t port, int bit, int *val)
{
    int ctrl;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_EGR_L2_PROCESS_CTRL, &ctrl));

    *val = (ctrl >> bit) & 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure port-based egress discard.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] enable Enable/disable egress discard.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_discard_set(int unit, bcm_port_t port, int enable)
{
    return xfs_port_egr_l2_ctrl_set(unit, port, 2, enable ? 1 : 0);
}


/*!
 * \brief Retrive port-based egress discard.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] enable Enable/disable egress discard.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_discard_get(int unit, bcm_port_t port, int *enable)
{
    return xfs_port_egr_l2_ctrl_get(unit, port, 2, enable);
}

/*!
 * \brief Configure egress port-based bridging.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] enable Enable to allow bridging to the incoming port.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_bridge_set(int unit, bcm_port_t port, int enable)
{
    return xfs_port_egr_l2_ctrl_set(unit, port, 1, enable ? 0 : 1);
}


/*!
 * \brief Retrive egress port-based bridging.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] enable Enable to allow bridging to the incoming port.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_bridge_get(int unit, bcm_port_t port, int *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_egr_l2_ctrl_get(unit, port, 1, enable));

    *enable = !(*enable);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure egress port control.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] type Port type.
 * \param [in] ecn Latency-based ECN remarking.
 * \param [in] tx_byte Enable updating TX byte count that would be inserted into IFA metadata.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_port_ctrl_set(int unit, bcm_port_t port, int type, int ecn, int tx_byte)
{
    int ctrl;

    SHR_FUNC_ENTER(unit);
    PORT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_EGR_PORT_TYPE, &ctrl));

    if (type != -1) {
        ctrl = type | (ctrl & (ECN_ENABLE | TX_BYTE_ENABLE));
    }
    if (ecn != -1) {
        ctrl = ecn ? (ctrl | ECN_ENABLE) : (ctrl & ~ECN_ENABLE);
    }
    if (tx_byte != -1) {
        ctrl = tx_byte ? (ctrl | TX_BYTE_ENABLE) : (ctrl & ~TX_BYTE_ENABLE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set
            (unit, port, BCMI_PT_EGR_PORT_TYPE, ctrl));

exit:
    PORT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress port control.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [out] type Port type.
 * \param [out] ecn Latency-based ECN remarking.
 * \param [out] tx_byte Enable updating TX byte count that would be inserted into IFA metadata.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_port_ctrl_get(int unit, bcm_port_t port, int *type, int *ecn, int *tx_byte)
{
    int ctrl;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_EGR_PORT_TYPE, &ctrl));

    if (type) {
        *type = ctrl & ~(ECN_ENABLE | TX_BYTE_ENABLE);
    }
    if (ecn) {
        *ecn = (ctrl & ECN_ENABLE) ? 1 : 0;
    }
    if (tx_byte) {
        *tx_byte = (ctrl & TX_BYTE_ENABLE) ? 1 : 0;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Configure the source of egress VLAN priority.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] src Source of egress VLAN priority.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_vlan_pri_src_set(int unit, bcm_port_t port, int src)
{
    int en;

    SHR_FUNC_ENTER(unit);

    if (xfs_port_db[unit]->feature->egr_vlan_pri_src) {
        if (src == bcmPortEgressVlanPriSrcPktPri) {
            en = 1;
        } else if ((src == bcmPortEgressVlanPriSrcIntPri) ||
                   (src == bcmPortEgressVlanPriSrcVpn)) {
            en = 0;
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_qos_port_egr_l2_map_set(unit, port, src));
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (xfs_port_egr_l2_ctrl_set(unit, port, 3, en));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the source of egress VLAN priority.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [out] src Source of egress VLAN priority.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_vlan_pri_src_get(int unit, bcm_port_t port, int *src)
{
    int en;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (xfs_port_egr_l2_ctrl_get(unit, port, 3, &en));

    if (en) {
        *src = bcmPortEgressVlanPriSrcPktPri;
    } else {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_qos_port_egr_l2_map_get(unit, port, src));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Cofigure whether to map incoming packet's Vlan to VFI.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] enable Enable to trust incoming packet's Vlan.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_trust_vlan_set(int unit, bcm_port_t port, int enable)
{
    int type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_L2_DEST_TYPE, &type));
    if (type == DEST_TYPE_VP) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_VLAN_MAP_TO_VFI, enable));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrive whether to map incoming packet's Vlan to VFI.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [out] enable Enable to trust incoming packet's Vlan.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_trust_vlan_get(int unit, bcm_port_t port, int *enable)
{
    int type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_L2_DEST_TYPE, &type));
    if (type == DEST_TYPE_VP) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_VLAN_MAP_TO_VFI, enable));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Cofigure whether not to check VLAN membership and STG on ingress port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] disable Not check VLAN membership and STG on ingress port.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_ing_vlan_stg_check_set(int unit, bcm_port_t port, int disable)
{
    uint16_t exp, flags;

    SHR_FUNC_ENTER(unit);

    flags = BCMI_LTSW_VLAN_CHK_ING;
    exp = (disable ? flags : 0);
    if ((xfs_port_ha(unit, port)->vlan_check_disable & flags) != exp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_port_check_set(unit, port, disable, flags));
        xfs_port_ha(unit, port)->vlan_check_disable &= ~flags;
        xfs_port_ha(unit, port)->vlan_check_disable |= exp;
    }

    flags = BCMI_LTSW_STG_CHK_ING;
    exp = (disable ? flags : 0);
    if ((xfs_port_ha(unit, port)->stg_check_disable & flags) != exp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stg_port_check_set(unit, port, disable, flags));
        xfs_port_ha(unit, port)->stg_check_disable &= ~flags;
        xfs_port_ha(unit, port)->stg_check_disable |= exp;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrive whether not to check VLAN membership and STG on ingress port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] disable Not check VLAN membership and STG on ingress port.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_ing_vlan_stg_check_get(int unit, bcm_port_t port, int *disable)
{
    uint16_t flags = BCMI_LTSW_VLAN_CHK_ING;

    *disable = ((xfs_port_ha(unit, port)->vlan_check_disable & flags) == flags)
             ? 1 : 0;

    return SHR_E_NONE;
}


/*!
 * \brief Cofigure whether not to check VLAN membership and STG on egress port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] disable Not check VLAN membership and STG on egress port.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_vlan_stg_check_set(int unit, bcm_port_t port, int disable)
{
    uint16_t exp, flags;

    SHR_FUNC_ENTER(unit);

    flags = BCMI_LTSW_VLAN_CHK_ING_EGR | BCMI_LTSW_VLAN_CHK_EGR;
    exp = (disable ? flags : 0);
    if ((xfs_port_ha(unit, port)->vlan_check_disable & flags) != exp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_port_check_set(unit, port, disable, flags));
        xfs_port_ha(unit, port)->vlan_check_disable &= ~flags;
        xfs_port_ha(unit, port)->vlan_check_disable |= exp;
    }

    flags = BCMI_LTSW_STG_CHK_ING_EGR | BCMI_LTSW_STG_CHK_EGR;
    exp = (disable ? flags : 0);
    if ((xfs_port_ha(unit, port)->stg_check_disable & flags) != exp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stg_port_check_set(unit, port, disable, flags));
        xfs_port_ha(unit, port)->stg_check_disable &= ~flags;
        xfs_port_ha(unit, port)->stg_check_disable |= exp;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrive whether not to check VLAN membership and STG on egress port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] disable Not check VLAN membership and STG on egress port.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_vlan_stg_check_get(int unit, bcm_port_t port, int *disable)
{
    uint16_t flags = BCMI_LTSW_VLAN_CHK_ING_EGR | BCMI_LTSW_VLAN_CHK_EGR;

    *disable = ((xfs_port_ha(unit, port)->vlan_check_disable & flags) == flags)
             ? 1 : 0;

    return SHR_E_NONE;
}


/*!
 * \brief Enable QoS remarking.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] enable QoS remarking.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_qos_remark_enable_set(int unit, bcm_port_t port, int enable)
{
    return bcmi_ltsw_port_tab_bit_set
            (unit, port, BCMI_PT_QOS_REMARK_CTRL, 0, !enable);
}


/*!
 * \brief Retrive QoS remarking enable.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [out] enable QoS remarking.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_qos_remark_enable_get(int unit, bcm_port_t port, int *enable)
{
    int disable;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_bit_get
            (unit, port, BCMI_PT_QOS_REMARK_CTRL, 0, &disable));

    *enable = !disable;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Enable VLAN priority remap.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] enable VLAN priority remap.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_vlan_pri_remap_enable_set(int unit, bcm_port_t port, int enable)
{
    return bcmi_ltsw_port_tab_bit_set
            (unit, port, BCMI_PT_QOS_REMARK_CTRL, 1, enable);
}


/*!
 * \brief Retrive VLAN priority remap enable.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [out] enable VLAN priority remap.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_vlan_pri_remap_enable_get(int unit, bcm_port_t port, int *enable)
{
    return bcmi_ltsw_port_tab_bit_get
            (unit, port, BCMI_PT_QOS_REMARK_CTRL, 1, enable);
}


/*!
 * \brief Enable TOS remap.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [in] enable TOS remap.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_tos_remap_enable_set(int unit, bcm_port_t port, int enable)
{
    return bcmi_ltsw_port_tab_bit_set
            (unit, port, BCMI_PT_QOS_REMARK_CTRL, 2, enable);
}


/*!
 * \brief Retrive TOS remap enable.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 * \param [out] enable TOS remap.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_egr_tos_remap_enable_get(int unit, bcm_port_t port, int *enable)
{
    return bcmi_ltsw_port_tab_bit_get
            (unit, port, BCMI_PT_QOS_REMARK_CTRL, 2, enable);
}


static int
xfs_port_control_vport_get(int unit, bcm_port_t port,
                           bcm_port_control_t type, int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    switch (type) {
        case bcmPortControlL2Learn:
            if (BCM_GPORT_IS_FLOW_PORT(port)) {
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_virtual_port_learn_get(unit, port, (uint32_t*)value));
                SHR_EXIT();
            }

        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xfs_port_control_vport_set(int unit, bcm_port_t port,
                           bcm_port_control_t type, int value)
{
    SHR_FUNC_ENTER(unit);

    switch (type) {
        case bcmPortControlL2Learn:
            if (BCM_GPORT_IS_FLOW_PORT(port)) {
                SHR_IF_ERR_EXIT
                    (bcmi_ltsw_virtual_port_learn_set(unit, port, value));
                SHR_EXIT();
            }

        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get per-port options.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Port control option type.
 * \param [out] value Value returned.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_control_port_get(int unit, bcm_port_t port, bcm_port_control_t type,
                          int *value)
{
    bcmi_ltsw_port_tab_t fld;
    int pm, idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    switch (type) {
        case bcmPortControlOOBFCTxReportIngEnable:
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_oob_port_control_get(unit, port, 0, value));
            SHR_EXIT();

        case bcmPortControlOOBFCTxReportEgrEnable:
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_oob_port_control_get(unit, port, 1, value));
            SHR_EXIT();

        case bcmPortControlEgressVlanPriSrc:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_vlan_pri_src_get(unit, port, value));
            SHR_EXIT();

        case bcmPortControlDiscardEgress:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_discard_get(unit, port, value));
            SHR_EXIT();

        case bcmPortControlBridgeEgress:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_bridge_get(unit, port, value));
            SHR_EXIT();

        case bcmPortControlLatencyEcnEnable:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_port_ctrl_get(unit, port, NULL, value, NULL));
            SHR_EXIT();

        case bcmPortControlTrustIncomingVlan:
            SHR_IF_ERR_EXIT
                (xfs_port_trust_vlan_get(unit, port, value));
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
            if (is_aux_port(unit, port)) {
                SHR_ERR_EXIT(SHR_E_PORT);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_pm_id_get(unit, port, &pm));
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_obm_pm_port_control_get
                    (unit, pm, bcmiObmPortControlVntagEthertype, value));
            SHR_EXIT();

        case bcmPortControlObmClassifierEtagEthertype:
            if (is_aux_port(unit, port)) {
                SHR_ERR_EXIT(SHR_E_PORT);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_pm_id_get(unit, port, &pm));
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_obm_pm_port_control_get
                    (unit, pm, bcmiObmPortControlEtagEthertype, value));
            SHR_EXIT();

        case bcmPortControlTxByteState:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_port_ctrl_get(unit, port, NULL, NULL, value));
            SHR_EXIT();

        case bcmPortControlDoNotCheckVlan:
            SHR_IF_ERR_EXIT
                (xfs_port_ing_vlan_stg_check_get(unit, port, value));
            SHR_EXIT();

        case bcmPortControlDoNotCheckVlanEgress:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_vlan_stg_check_get(unit, port, value));
            SHR_EXIT();

        case bcmPortControlQoSRemarkEgressEnable:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_qos_remark_enable_get(unit, port, value));
            SHR_EXIT();

        case bcmPortControlEgressPktPriCfiCopyIngressEnable:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_vlan_pri_remap_enable_get(unit, port, value));
            SHR_EXIT();

        case bcmPortControlEgressTosCopyIngressEnable:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_tos_remap_enable_get(unit, port, value));
            SHR_EXIT();

        default:
            break;
    }

    /* Check the simple mapping table first. */
    for (idx = 0; idx < COUNTOF(port_control_port_map); idx++) {
        if (port_control_port_map[idx].type == type) {
            if (port_control_port_map[idx].flags & PORT_CTRL_PM) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_ltsw_port_control_pm_get(unit, port, type, value));
            } else {
                fld = port_control_port_map[idx].fld;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_tab_get(unit, port, fld, value));
                if (port_control_port_map[idx].func_get) {
                    /* need to translate the value from H/W to S/W*/
                    SHR_IF_ERR_VERBOSE_EXIT
                        (port_control_port_map[idx].func_get
                            (unit, *value, value));
                }
            }

            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set per-port options.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Port control option type.
 * \param [in] value Port control option value.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
xfs_port_control_port_set(int unit, bcm_port_t port, bcm_port_control_t type,
                          int value)
{
    bcmi_ltsw_port_tab_t fld;
    int pm, idx;
    int rv;

    SHR_FUNC_ENTER(unit);

    switch (type) {
        case bcmPortControlOOBFCTxReportIngEnable:
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_oob_port_control_set(unit, port, 0, value));
            SHR_EXIT();

        case bcmPortControlOOBFCTxReportEgrEnable:
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_oob_port_control_set(unit, port, 1, value));
            SHR_EXIT();

        case bcmPortControlEgressVlanPriSrc:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_vlan_pri_src_set(unit, port, value));
            SHR_EXIT();

        case bcmPortControlDiscardEgress:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_discard_set(unit, port, value));
            SHR_EXIT();

        case bcmPortControlBridgeEgress:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_bridge_set(unit, port, value));
            SHR_EXIT();

        case bcmPortControlLatencyEcnEnable:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_port_ctrl_set(unit, port, -1, value, -1));
            SHR_EXIT();

        case bcmPortControlTrustIncomingVlan:
            SHR_IF_ERR_EXIT
                (xfs_port_trust_vlan_set(unit, port, value));
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
            if (is_aux_port(unit, port)) {
                SHR_ERR_EXIT(SHR_E_PORT);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_pm_id_get(unit, port, &pm));
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_obm_pm_port_control_set
                    (unit, pm, bcmiObmPortControlVntagEthertype, value & 0xFFFF));
            SHR_EXIT();

        case bcmPortControlObmClassifierEtagEthertype:
            if (is_aux_port(unit, port)) {
                SHR_ERR_EXIT(SHR_E_PORT);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_ltsw_port_pm_id_get(unit, port, &pm));
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_obm_pm_port_control_set
                    (unit, pm, bcmiObmPortControlEtagEthertype, value & 0xFFFF));
            SHR_EXIT();

        case bcmPortControlTxByteState:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_port_ctrl_set(unit, port, -1, -1, value));
            SHR_EXIT();

        case bcmPortControlDoNotCheckVlan:
            SHR_IF_ERR_EXIT
                (xfs_port_ing_vlan_stg_check_set(unit, port, value));
            SHR_EXIT();

        case bcmPortControlDoNotCheckVlanEgress:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_vlan_stg_check_set(unit, port, value));
            SHR_EXIT();

        case bcmPortControlQoSRemarkEgressEnable:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_qos_remark_enable_set(unit, port, value));
            SHR_EXIT();

        case bcmPortControlEgressPktPriCfiCopyIngressEnable:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_vlan_pri_remap_enable_set(unit, port, value));
            SHR_EXIT();

        case bcmPortControlEgressTosCopyIngressEnable:
            SHR_IF_ERR_EXIT
                (xfs_port_egr_tos_remap_enable_set(unit, port, value));
            SHR_EXIT();

        default:
            break;
    }

    /* Check the simple mapping table first. */
    for (idx = 0; idx < COUNTOF(port_control_port_map); idx++) {
        if (port_control_port_map[idx].type == type) {
            if (port_control_port_map[idx].flags & PORT_CTRL_PM) {
                PORT_LOCK(unit);
                rv = bcmint_ltsw_port_control_pm_set(unit, port, type, value);
                PORT_UNLOCK(unit);
                if (rv != SHR_E_NONE) {
                    SHR_ERR_EXIT(rv);
                }
            } else {
                fld = port_control_port_map[idx].fld;
                if (port_control_port_map[idx].func_set) {
                    /* Convert from S/W to H/W */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (port_control_port_map[idx].func_set
                            (unit, value, &value));
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_tab_set(unit, port, fld, value));
            }

            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Add an ingress TPID to a port. A port could have multiple TPID.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] pt Enumerator of port tab type.
 * \param [in] tpid TPID to be added.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfs_port_ing_tpid_add(int unit, int port, bcmi_ltsw_port_tab_t pt,
                      uint16_t tpid)
{
    int tpid_index, tpid_bmp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, pt, &tpid_bmp));

    /* allocate a TPID index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid, &tpid_index));

    /* rollback if already in the port TPID list; else add to port TPID list */
    if (tpid_bmp & (1 << tpid_index)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_delete(unit, tpid_index));
    } else {
        tpid_bmp |= (1 << tpid_index);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, port, pt, tpid_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set ingress TPID of a port. TPIDs other than the set one are deleted.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] pt Enumerator of port tab type.
 * \param [in] tpid TPID to be set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfs_port_ing_tpid_set(int unit, int port, bcmi_ltsw_port_tab_t pt,
                      uint16_t tpid)
{
    int tpid_index, tpid_bmp, i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, pt, &tpid_bmp));

    /* allocate a TPID index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid, &tpid_index));

    /* delete all TPIDs in port TPID list */
    for (i = 0; tpid_bmp; i ++) {
        if (tpid_bmp & 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_delete(unit, i));
        }
        tpid_bmp = tpid_bmp >> 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, pt, 1 << tpid_index));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Delete an ingress TPID from a port. Add default TPID if need.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] pt Enumerator of port tab type.
 * \param [in] tpid TPID to be deleted.
 * \param [in] tpid_default Default TPID.
 * \param [in] force TRUE to force add default TPID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfs_port_ing_tpid_delete(int unit, int port, bcmi_ltsw_port_tab_t pt,
                         uint16_t tpid, uint16_t tpid_default, int force)
{
    int tpid_index, tpid_bmp, tpid_default_index;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, pt, &tpid_bmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_lookup(unit, tpid, &tpid_index));

    /* the delete TPID must be in port TPID list */
    if (!(tpid_bmp & (1 << tpid_index))) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    tpid_bmp &= ~(1 << tpid_index);
    
    /* need add default TPID */
    if (force || (tpid_bmp == 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_lookup
                (unit, tpid_default, &tpid_default_index));
        
        /* add default TPID if it's not in port TPID list */
        if (!(tpid_bmp & (1 << tpid_default_index))) {
            tpid_bmp |= (1 << tpid_default_index);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_add
                    (unit, tpid_default, &tpid_default_index));
        }
    }
    
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_delete(unit, tpid_index));
    
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, pt, tpid_bmp));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Delete all ingress TPID from a port, then add default TPID.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] pt Enumerator of port tab type.
 * \param [in] tpid_default Default TPID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfs_port_ing_tpid_delete_all(int unit, int port, bcmi_ltsw_port_tab_t pt,
                             uint16_t tpid_default)
{
    int tpid_bmp, tpid_del_bmp, tpid_default_index, i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, pt, &tpid_bmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_lookup(unit, tpid_default, &tpid_default_index));

    /* add default TPID if it's not in port TPID list */
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
        (bcmi_ltsw_port_tab_set(unit, port, pt, (1 << tpid_default_index)));

exit:
    SHR_FUNC_EXIT();
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
xfs_port_obm_tpid_bmp_set(int unit, int port, int bmp)
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
xfs_port_obm_tpid_bmp_get(int unit, int port, int *bmp)
{
    int i, en;

    SHR_FUNC_ENTER(unit);

    if (is_aux_port(unit, port)) {
        *bmp = 0;
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
xfs_port_obm_tpid_add(int unit, int port, uint16_t tpid)
{
    int tpid_index, tpid_bmp;

    SHR_FUNC_ENTER(unit);

    if (is_aux_port(unit, port)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_obm_tpid_bmp_get(unit, port, &tpid_bmp));

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
            (xfs_port_obm_tpid_bmp_set(unit, port, tpid_bmp));
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
xfs_port_obm_tpid_set(int unit, int port, uint16_t tpid)
{
    int tpid_index, tpid_bmp, i;

    SHR_FUNC_ENTER(unit);

    if (is_aux_port(unit, port)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_obm_tpid_bmp_get(unit, port, &tpid_bmp));

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
        (xfs_port_obm_tpid_bmp_set(unit, port, 1 << tpid_index));

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
xfs_port_obm_tpid_delete(int unit, int port,
                         uint16_t tpid, uint16_t tpid_default, int force)
{
    int tpid_index, tpid_bmp, tpid_default_index;

    SHR_FUNC_ENTER(unit);

    if (is_aux_port(unit, port)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_obm_tpid_bmp_get(unit, port, &tpid_bmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_lookup(unit, tpid, &tpid_index));

    /* the delete TPID must be in OBM TPID list */
    if (!(tpid_bmp & (1 << tpid_index))) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    tpid_bmp &= ~(1 << tpid_index);

    /* need add default TPID */
    if (force || (tpid_bmp == 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_otpid_entry_lookup
                (unit, tpid_default, &tpid_default_index));

        /* add default TPID if it's not in OBM TPID list */
        if (!(tpid_bmp & (1 << tpid_default_index))) {
            tpid_bmp |= (1 << tpid_default_index);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_add
                    (unit, tpid_default, &tpid_default_index));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_delete(unit, tpid_index));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_obm_tpid_bmp_set(unit, port, tpid_bmp));

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
xfs_port_obm_tpid_delete_all(int unit, int port, uint16_t tpid_default)
{
    int tpid_bmp, tpid_del_bmp, tpid_default_index, i;

    SHR_FUNC_ENTER(unit);

    if (is_aux_port(unit, port)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_obm_tpid_bmp_get(unit, port, &tpid_bmp));

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
        (xfs_port_obm_tpid_bmp_set(unit, port, (1 << tpid_default_index)));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get TPID enable bitmap.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] type TPID type.
 * \param [out] bmp TPIDs enabled, in the form of bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
xfs_port_tpid_bmp_get(int unit, int port, int type, int *bmp)
{
    if (type == OUTER_TPID) {
        return bcmi_ltsw_port_tab_get
                (unit, port, BCMI_PT_ING_OTPID_ENABLE, bmp);
    } else if (type == PAYLOAD_OUTER_TPID) {
        return bcmi_ltsw_port_tab_get
                (unit, port, BCMI_PT_ING_PAYLOAD_OTPID_ENABLE, bmp);
    } else if (type & OBM_OUTER_TPID) {
        return xfs_port_obm_tpid_bmp_get(unit, port, bmp);
    } else {
        return SHR_E_INTERNAL;
    }
}


static int
xfs_port_flexctr_id_set(int unit, bcm_gport_t port, int dir, int ctr_id)
{
    SHR_FUNC_ENTER(unit);

    if (dir == BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set
                (unit, port, BCMI_PT_ING_FLEX_CTR_ID, ctr_id));
    } else if (dir == BCMI_LTSW_FLEXCTR_DIR_EGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set
                (unit, port, BCMI_PT_EGR_L2_OIF_VIEW, L2_OIF_VIEW_FLEX_CNTR));
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
xfs_port_flexctr_id_get(int unit, bcm_gport_t port, int dir, int *ctr_id)
{
    SHR_FUNC_ENTER(unit);

    if (dir == BCMI_LTSW_FLEXCTR_DIR_INGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get
                (unit, port, BCMI_PT_ING_FLEX_CTR_ID, ctr_id));
    } else if (dir == BCMI_LTSW_FLEXCTR_DIR_EGRESS) {
        /* In ITPID view it will return -7, don't care */
        *ctr_id = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_ltsw_port_tab_get
                (unit, port, BCMI_PT_EGR_FLEX_CTR_ID, ctr_id),
             SHR_E_NOT_FOUND);
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static inline int
xfs_port_flexctr_info_check(bcmi_ltsw_flexctr_counter_info_t *info)
{
    if ((info->source == bcmFlexctrSourceIngPort) ||
        (info->source == bcmFlexctrSourceEgrPort)) {
        return SHR_E_NONE;
    } else {
        return SHR_E_PARAM;
    }
}

/*!
 * \brief Add port flood block LT to a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_flood_block_lt_add(int unit, int port)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, i, max_port_num;
    uint64_t *mask = NULL;

    SHR_FUNC_ENTER(unit);

    /* mask all ports so that never block */
    max_port_num = PORT_NUM_MAX(unit);
    SHR_ALLOC(mask, max_port_num * sizeof(uint64_t), "port_flood_mask");
    SHR_NULL_CHECK(mask, SHR_E_MEMORY);
    for (i = 0; i < max_port_num; i ++) {
        mask[i] = 1;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_ING_EGR_BLOCK_0s, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_ING_EGR_BLOCK_IDs, port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, MASK_TARGETs, L2_L3_MEMBERs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, MASK_ACTIONs, ANDs));

    for (i = 0; i < 4; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PROFILE_SECTIONs, i));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add
                (eh, MASK_EGR_PORTSs, 0, mask, max_port_num));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit
                (unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FREE(mask);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete port flood block LT from a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port Number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_flood_block_lt_del(int unit, int port)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit, i;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_ING_EGR_BLOCK_0s, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_ING_EGR_BLOCK_IDs, port));

    for (i = 0; i < 4; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PROFILE_SECTIONs, i));

        rv = bcmi_lt_entry_commit
                (unit, eh, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL);
        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get egress mask profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [out] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_egr_mask_lt_get(int unit, int index, void *profile)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    xfs_port_egr_mask_profile_t *p;
    int dunit, i, max_port_num;
    uint32_t cnt;
    uint64_t v64;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_ING_EGR_BLOCK_2s, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PROFILE_SECTIONs, index / 256));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_ING_EGR_BLOCK_IDs, index % 256));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
            (unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    p = (xfs_port_egr_mask_profile_t *)profile;
    BCM_PBMP_CLEAR(p->pbmp);
    max_port_num = PORT_NUM_MAX(unit);
    for (i = 0; i < max_port_num; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_array_get(eh, MASK_EGR_PORTSs, i, &v64, 1, &cnt));
        if (v64) {
            BCM_PBMP_PORT_ADD(p->pbmp, i);
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert egress mask profile entry to LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 * \param [in] profile Profile entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_egr_mask_lt_insert(int unit, int index, void *profile)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    xfs_port_egr_mask_profile_t *p;
    int dunit, i, max_port_num;
    uint64_t v64;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_ING_EGR_BLOCK_2s, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PROFILE_SECTIONs, index / 256));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_ING_EGR_BLOCK_IDs, index % 256));

    p = (xfs_port_egr_mask_profile_t *)profile;
    max_port_num = PORT_NUM_MAX(unit);
    for (i = 0; i < max_port_num; i ++) {
        v64 = (BCM_PBMP_MEMBER(p->pbmp, i) ? 1 : 0);
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_array_add(eh, MASK_EGR_PORTSs, i, &v64, 1));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, MASK_TARGETs, L2_L3_MEMBERs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, MASK_ACTIONs, ANDs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
            (unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete egress mask profile entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile entry index.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_egr_mask_lt_delete(int unit, int index)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORT_ING_EGR_BLOCK_2s, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PROFILE_SECTIONs, index / 256));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PORT_ING_EGR_BLOCK_IDs, index % 256));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
            (unit, eh, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void) bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash of egress mask profile set.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] entries_per_set Number of profile entries in the set.
 * \param [out] hash Hash value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_egr_mask_profile_hash(int unit, void *entries,
                                int entries_per_set, uint32_t *hash)
{
    int size = entries_per_set * sizeof(xfs_port_egr_mask_profile_t);
    *hash = shr_crc32(0, entries, size);
    return SHR_E_NONE;
}

/*!
 * \brief Compare profile set.
 *
 * Compare if given egress mask profile entries equals to
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
xfs_port_egr_mask_profile_cmp(int unit, void *entries,
                               int entries_per_set, int index, int *cmp)
{
    return bcmint_ltsw_port_profile_cmp
            (unit, entries, index, profile_egr_mask(unit), cmp);
}

/*!
 * \brief Recover egress mask profile from system port table after wb.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_egr_mask_profile_recover(int unit)
{
    int entry_idx, dunit;
    uint64_t u64;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* read all entries from ING_SYSTEM_PORT_TABLE */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ING_SYSTEM_PORT_TABLEs, &eh));

    while (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_TRAVERSE,
                                BCMLT_PRIORITY_NORMAL) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, BITMAP_PROFILE_SECTION_SELs, &u64));
        entry_idx = u64 * 256;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, BITMAP_PROFILE_INDEXs, &u64));
        entry_idx += u64;

        /* recover the index */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_port_profile_recover
                (unit, entry_idx, profile_egr_mask(unit)));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize egress mask profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_egr_mask_profile_init(int unit)
{
    xfs_port_egr_mask_profile_t profile;
    ltsw_port_profile_info_t *pinfo = profile_egr_mask(unit);
    int entry_idx;
    uint32_t entry_num;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    pinfo->phd = BCMI_LTSW_PROFILE_EGR_MASK;
    pinfo->entry_idx_min = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, PORT_ING_EGR_BLOCK_2s, &entry_num));
    pinfo->entry_idx_max = entry_num - 1;
    pinfo->entries_per_set = 1;
    pinfo->entry_size = sizeof(xfs_port_egr_mask_profile_t);
    pinfo->lt_get = xfs_port_egr_mask_lt_get;
    pinfo->lt_ins = xfs_port_egr_mask_lt_insert;
    pinfo->lt_del = xfs_port_egr_mask_lt_delete;

    /* unregister in case the profile already exists */
    rv = bcmi_ltsw_profile_unregister(unit, pinfo->phd);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    /* register profile for SYS_PORT_FLOOD_BLOCK */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register
            (unit,
             &pinfo->phd, &pinfo->entry_idx_min, &pinfo->entry_idx_max, 1,
             xfs_port_egr_mask_profile_hash,
             xfs_port_egr_mask_profile_cmp));

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (xfs_port_egr_mask_profile_recover(unit));
        SHR_EXIT();
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, PORT_ING_EGR_BLOCK_2s));

    /* default SYS_PORT_FLOOD_BLOCK profile entry */
    _SHR_PBMP_PORTS_RANGE_ADD(profile.pbmp, 0, PORT_NUM_MAX(unit));
    /* allocate profile entry, all sys port entries refer to it */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, ING_SYSTEM_PORT_TABLEs, &entry_num));
    /* one more reference to ensure default profile won't be released */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_profile_add
            (unit, &profile, entry_num + 1, pinfo, &entry_idx));
    /* first allocation so index must be zero */
    if (entry_idx != 0) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize egress mask profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_egr_mask_profile_deinit(int unit)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* unregister in case the profile already exists */
    rv = bcmi_ltsw_profile_unregister(unit, profile_egr_mask(unit)->phd);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* clear profile table */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, PORT_ING_EGR_BLOCK_2s));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add egress mask profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile Profile to be added.
 * \param [out] index Index of the added or existed profile enrty.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_egr_mask_profile_add(int unit, void *profile, int *index)
{
    return bcmint_ltsw_port_profile_add
            (unit, profile, 1, profile_egr_mask(unit), index);
}

/*!
 * \brief Add egress mask profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile Profile to be added.
 * \param [out] index Index of the added or existed profile enrty.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_egr_mask_profile_add_ref(int unit, int index)
{
    return bcmint_ltsw_port_profile_add_ref
            (unit, index, 1, profile_egr_mask(unit));
}

/*!
 * \brief Get egress mask profile from LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] index Index of the first entry.
 * \param [out] profile Profile entries.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_egr_mask_profile_get(int unit, int index, void *profile)
{
    return bcmint_ltsw_port_profile_get
            (unit, index, profile_egr_mask(unit), profile);

}

/*!
 * \brief Delete egress mask profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] index Index of the first entry to be deleted.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_egr_mask_profile_delete(int unit, int index)
{
    return bcmint_ltsw_port_profile_delete
            (unit, index, profile_egr_mask(unit));
}


/*!
 * \brief Move the content of a LT entry to a new entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] table Logic table name.
 * \param [in] key Key field name.
 * \param [in] from_id Index of current entry.
 * \param [in] to_id Index of new entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_lt_entry_move(int unit, const char *table, const char *key,
                       int from_id, int to_id)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, key, from_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
            (unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, key, to_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
            (unit, eh, BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, key, from_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit
            (unit, eh, BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief WAR to check if R_BLOCK_MASK_SECTION_CONTROL exists. Will be removed with NPL 449.
 *
 * \param [in] unit Unit Number.
 *
 * \return TRUE for exist, FALSE for not exist.
 */
static int
xfs_block_mask_section_control_exist(int unit)
{
    int dunit, rv;
    const char *tbl_desc;

    dunit = bcmi_ltsw_dev_dunit(unit);
    rv = bcmlt_table_desc_get(dunit, R_BLOCK_MASK_SECTION_CONTROLs, &tbl_desc);
    return SHR_SUCCESS(rv);
}


/*!
 * \brief Parser config property "shared_block_mask_section".
 *
 * \param [in] unit Unit Number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_block_mask_section_parser(int unit)
{
    int i, j, end = 0, *traffic_shared, cnt_shared = 0;
    char buf[32], *str, *seg, *s;
    static struct {
        const char *name;
        int traffic;
    } traffic_list[TRAFFIC_TYPE_NUM] = {
        {"bc",  BCM_PORT_FLOOD_BLOCK_BCAST},
        {"umc", BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST},
        {"mc",  BCM_PORT_FLOOD_BLOCK_KNOWN_MCAST},
        {"uuc", BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST},
        {"uc",  BCM_PORT_FLOOD_BLOCK_KNOWN_UCAST},
    };
    static block_mask_section_info_t block_mask_section_dft = {
        {
            BCM_PORT_FLOOD_BLOCK_BCAST | BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST,
            BCM_PORT_FLOOD_BLOCK_KNOWN_MCAST,
            BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST,
            BCM_PORT_FLOOD_BLOCK_KNOWN_UCAST,
        },
    };
    static block_mask_section_info_t block_mask_section_449 = {
        {
            BCM_PORT_FLOOD_BLOCK_KNOWN_MCAST,
            BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST,
            BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST,
            BCM_PORT_FLOOD_BLOCK_BCAST,
        },
    };

    SHR_FUNC_ENTER(unit);

    if (!xfs_block_mask_section_control_exist(unit)) {
        sal_memcpy(block_mask_section(unit), &block_mask_section_449,
                   sizeof(block_mask_section_449));
        SHR_EXIT();
    }

    str = bcmi_ltsw_property_str_get(unit, BCMI_CPN_SHARED_BLOCK_MASK_SECTION);
    if (!str) {
        sal_memcpy(block_mask_section(unit), &block_mask_section_dft,
                   sizeof(block_mask_section_dft));
        SHR_EXIT();
    }

    /* shared block mask section */
    sal_strncpy(buf, str, 32);
    buf[31] = '\0';
    s = seg = buf;
    traffic_shared =
       &(block_mask_section(unit)->traffics[BCMI_LTSW_BLOCK_MASK_SECTION_SHARED]);
    while (1) {
        /* string before '_' or '\0' is a segment */
        if ((*s == '_') || (*s == '\0')) {
            end = (*s == '\0');
            *s = '\0';
            for (i = 0; i < TRAFFIC_TYPE_NUM; i ++) {
                if (sal_strcasecmp(seg, traffic_list[i].name) == 0) {
                    *traffic_shared |= traffic_list[i].traffic;
                    cnt_shared ++;
                    break;
                }
            }
            /* move to behind '_' */
            if (end) {
                break;
            } else {
                seg = s + 1;
            }
        }
        s ++;
    }

    /* unshared block mask sections */
    if (cnt_shared != TRAFFIC_TYPE_NUM - BLOCK_MASK_SECTION_NUM + 1) {
        LOG_ERROR(BSL_LS_BCM_PORT,
            (BSL_META_U(unit, "Warning: Invalid shared_block_mask_section[%s]: "
                "must contain two traffic types. Use default config.\n"), str));
        sal_memcpy(block_mask_section(unit), &block_mask_section_dft,
                   sizeof(block_mask_section_dft));
    } else {
        for (i = 0, j = 0; i < TRAFFIC_TYPE_NUM; i ++) {
            if (j == BCMI_LTSW_BLOCK_MASK_SECTION_SHARED) {
                j ++;
            }
            if ((traffic_list[i].traffic & (*traffic_shared)) == 0) {
                block_mask_section(unit)->traffics[j++] = traffic_list[i].traffic;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Init block mask section selections.
 *
 * \param [in] unit Unit Number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_block_mask_section_init(int unit)
{
    int i, j;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {BCAST_SECTION_SELs,         BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {UNKNOWN_MCAST_SECTION_SELs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {KNOWN_MCAST_SECTION_SELs,   BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3 */ {UNKNOWN_UCAST_SECTION_SELs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*4 */ {KNOWN_UCAST_SECTION_SELs,   BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    struct {
        int traffic;
        int section;
    } list[] = {
        {BCM_PORT_FLOOD_BLOCK_BCAST},
        {BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST},
        {BCM_PORT_FLOOD_BLOCK_KNOWN_MCAST},
        {BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST},
        {BCM_PORT_FLOOD_BLOCK_KNOWN_UCAST},
    };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_block_mask_section_parser(unit));

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }

    if (!xfs_block_mask_section_control_exist(unit)) {
        SHR_EXIT();
    }

    for (i = 0; i < sizeof(list) / sizeof(list[0]); i ++) {
        for (j = 0; j < BLOCK_MASK_SECTION_NUM; j ++) {
            if (list[i].traffic & block_mask_section(unit)->traffics[j]) {
                list[i].section = j;
                break;
            }
        }
    }

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    for (i = 0; i < lt_entry.nfields; i ++) {
        fields[i].u.val = list[i].section;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, R_BLOCK_MASK_SECTION_CONTROLs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Init PORT_ING_EGR_BLOCK_PROFILE.
 *
 * \param [in] unit Unit Number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_ifp_ing_egr_block_init(int unit)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {PORT_ING_EGR_BLOCK_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {SKIP_VLAN_ING_EGR_MEMBERSHIP_CHECKs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = 2;
    lt_entry.attr = 0;
    field[0].u.val = PROFILE_IFP;
    field[1].u.val = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set
            (unit, PORT_ING_EGR_BLOCK_PROFILEs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Count the number of ports which map to the identified port group.
 *
 * \param [in] unit Unit Number.
 * \param [in] pbmp Port range to count.
 * \param [in] port_group Port group.
 *
 * \return Number of ports which map to the identified port group.
 */
static int
xfs_port_group_count(int unit, bcm_pbmp_t pbmp, int dir, int port_group)
{
    int port, count = 0;

    BCM_PBMP_ITER(pbmp, port) {
        if (xfs_port(unit, port)->port_group[dir] == port_group) {
            count ++;
        }
    }

    return count;
}


/*!
 * \brief Set port group of a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] port_group Port group.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_group_set(int unit, bcm_port_t port, int dir, int port_group)
{
    int pipe, ingress, *old_port_group;
    bcm_pbmp_t pbmp;
    bcmi_ltsw_port_tab_t pt;

    SHR_FUNC_ENTER(unit);

    ingress = (dir == DIR_ING) ? 1 : 0;
    old_port_group = &(xfs_port(unit, port)->port_group[dir]);
    if (port_group == *old_port_group) {
        SHR_EXIT();
    }

    pipe = bcmi_ltsw_dev_logic_port_pp_pipe(unit, port);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dev_pp_pipe_logic_pbmp(unit, pipe, &pbmp));

    if (xfs_port_group_count(unit, pbmp, dir, port_group) == 0) {
        /* add vlan xlate */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_xlate_pipe_update(unit, port_group, ingress,
                                              1 << pipe, bcmiVlanTableOpInsert));
    }
    if (xfs_port_group_count
        (unit, pbmp, dir, *old_port_group) == 1) {
        /* delete vlan xlate */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_xlate_pipe_update(unit, *old_port_group, ingress,
                                              1 << pipe, bcmiVlanTableOpDelete));
    }

    pt = (dir == DIR_ING) ? BCMI_PT_ING_PORT_GROUP : BCMI_PT_EGR_PORT_GROUP;
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_tab_set(unit, port, pt, port_group));

    *old_port_group = port_group;

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get port group of a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [out] port_group Port group.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
xfs_port_group_get(int unit, bcm_port_t port, int dir, int *port_group)
{
    *port_group = xfs_port(unit, port)->port_group[dir];

    return SHR_E_NONE;
}


/******************************************************************************
 * Public functions
 */

int
xfs_ltsw_port_db_set(int unit, const xfs_ltsw_port_db_t *db)
{
    xfs_port_db[unit] = db;

    return SHR_E_NONE;
}


int
xfs_ltsw_port_info_init(int unit)
{
    int alloc_size;
    uint32_t ha_req_size, ha_alloc_size;
    void *ptr;

    SHR_FUNC_ENTER(unit);

    SHR_FREE(xfs_port_info[unit]);
    alloc_size = sizeof(xfs_port_ctrl_info_t)
               + sizeof(xfs_port_info_t) * PORT_NUM_MAX(unit);
    SHR_ALLOC(xfs_port_info[unit], alloc_size, "xfs_port_info");
    SHR_NULL_CHECK(xfs_port_info[unit], SHR_E_MEMORY);
    sal_memset(xfs_port_info[unit], 0, alloc_size);

    xfs_port_info[unit]->port = (xfs_port_info_t*)(xfs_port_info[unit] + 1);

    ha_req_size = sizeof(bcmint_port_xfs_ha_info_t) * PORT_NUM_MAX(unit);
    ha_alloc_size = ha_req_size;
    ptr = bcmi_ltsw_ha_mem_alloc
            (unit, BCMI_HA_COMP_ID_PORT, BCMINT_PORT_SUB_COMP_ID_XFS_HA,
             "bcmPortInfo", &ha_alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (!bcmi_warmboot_get(unit)) {
        sal_memset(ptr, 0, ha_alloc_size);
    }
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_issu_struct_info_report
            (unit, BCMI_HA_COMP_ID_PORT, BCMINT_PORT_SUB_COMP_ID_XFS_HA,
             0, sizeof(bcmint_port_xfs_ha_info_t), PORT_NUM_MAX(unit),
             BCMINT_PORT_XFS_HA_INFO_T_ID),
             SHR_E_EXISTS);

    xfs_port_info[unit]->port_ha = ptr;

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_info_deinit(int unit)
{
    int size;

    size = sizeof(xfs_port_ctrl_info_t)
         + sizeof(xfs_port_info_t) * PORT_NUM_MAX(unit);
    sal_memset(xfs_port_info[unit], 0, size);
    SHR_FREE(xfs_port_info[unit]);
    xfs_port_info[unit] = NULL;

    return SHR_E_NONE;
}


int
xfs_ltsw_port_info_recover(int unit)
{
    int dunit, port, l2_if, port_group;
    uint64_t u64;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* read all entries from ING_L2_IIF_TABLE */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ING_L2_IIF_TABLEs, &eh));
    while (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_TRAVERSE,
                                BCMLT_PRIORITY_NORMAL) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, L2_IIFs, &u64));
        l2_if = u64;

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (xfs_ltsw_l2_if_reserve(unit, l2_if, 1),
             SHR_E_NOT_FOUND);
    }

    bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

    /* read all entries from PORT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, PORTs, &eh));
    while (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_TRAVERSE,
                                BCMLT_PRIORITY_NORMAL) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, PORT_IDs, &u64));
        port = u64;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, L2_EIF_IDs, &u64));
        xfs_port_info[unit]->port[port].l2_if = u64;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, ING_SYSTEM_PORT_TABLE_IDs, &u64));
        xfs_port_info[unit]->port[port].sys_port = u64;
    }

    /* recover port group */
    PORT_ALL_ITER(unit, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get
                (unit, port, BCMI_PT_ING_PORT_GROUP, &port_group));
        xfs_port_info[unit]->port[port].port_group[DIR_ING] = port_group;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get
                (unit, port, BCMI_PT_EGR_PORT_GROUP, &port_group));
        xfs_port_info[unit]->port[port].port_group[DIR_EGR] = port_group;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_profile_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_egr_mask_profile_init(unit));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_profile_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_egr_mask_profile_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_profile_init_per_port(int unit, int port)
{
    /* nothing to do since reference count was adjusted in deinit */
    return SHR_E_NONE;
}


int
xfs_ltsw_port_profile_deinit_per_port(int unit, int port)
{
    int pid_old, pid_def = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_EGR_MASK_ID, &pid_old));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_EGR_MASK_ID, pid_def));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_egr_mask_profile_delete(unit, pid_old));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_egr_mask_profile_add_ref(unit, pid_def));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_misc_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize TPID */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_tpid_init(unit));

    /* Initialize block mask section */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_block_mask_section_init(unit));

    if (!bcmi_warmboot_get(unit)) {
        /* Initialize HG3 eth type */
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_port_hg3_eth_type_set(unit, HG3_ETYPE_DEFAULT));

        /* Initialize IFP ING_EGR_BLOCK_PROFILE */
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ifp_ing_egr_block_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_misc_deinit(int unit)
{
    return SHR_E_NONE;
}


int
xfs_ltsw_port_misc_init_per_port(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize TPID */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_tpid_init_per_port(unit, port));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_misc_deinit_per_port(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize TPID */
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_tpid_deinit_per_port(unit, port));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_tpid_recover(int unit)
{
    int port, i, j, bmp;
    int type[3] = {OUTER_TPID, PAYLOAD_OUTER_TPID, OBM_OUTER_TPID};

    SHR_FUNC_ENTER(unit);

    PORT_ALL_ITER(unit, port) {
        for (i = 0; i < 3; i ++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_port_tpid_bmp_get(unit, port, type[i], &bmp));
            for (j = 0; j < MAX_NUM_TPID; j ++) {
                if (bmp & (1 << j)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_ltsw_vlan_otpid_entry_ref_count_add(unit, j, 1));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_tpid_init(int unit)
{
    int port, index;
    uint16_t tpid_default;

    SHR_FUNC_ENTER(unit);

    /* Initialize outer TPID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_init(unit));

    /* Initialize inner TPID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_itpid_init(unit));

    /* Make sure default tpid always exists */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_default_get(unit, &tpid_default));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_entry_add(unit, tpid_default, &index));

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (xfs_ltsw_port_tpid_recover(unit));
        SHR_EXIT();
    }

    /* Enable the default outer TPID. */
    PORT_ALL_ITER(unit, port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_port_tpid_init_per_port(unit, port));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_tpid_init_per_port(int unit, bcm_port_t port)
{
    int rv;
    uint16_t tpid_default;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_default_get(unit, &tpid_default));

    rv = xfs_ltsw_port_tpid_set
            (unit, port, OUTER_TPID | OBM_OUTER_TPID | PAYLOAD_OUTER_TPID,
             tpid_default);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit, "Warning: "
                             "Failed to set %s default TPID: %s\n"),
                  bcmi_ltsw_port_name(unit, port), shr_errmsg(rv)));
    }

    rv = xfs_ltsw_port_inner_tpid_set(unit, port, tpid_default);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit, "Warning: "
                             "Failed to set %s default inner TPID: %s\n"),
                  bcmi_ltsw_port_name(unit, port), shr_errmsg(rv)));
    }


exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_tpid_deinit_per_port(int unit, bcm_port_t port)
{
    int tpid_bmp, i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_ING_OTPID_ENABLE, &tpid_bmp));

    /* delete all TPIDs */
    for (i = 0; tpid_bmp; i ++) {
        if (tpid_bmp & 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_delete(unit, i));
        }
        tpid_bmp = tpid_bmp >> 1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set
            (unit, port, BCMI_PT_EGR_OTPID, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set
            (unit, port, BCMI_PT_ING_OTPID_ENABLE, 0));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_strength_init(int unit)
{
    int id;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t sys_dest[] =
    {
        /*0*/ {SYSTEM_DESTINATION_PORTs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {STRENGTH_PROFILE_INDEXs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t l2_oif[] =
    {
        /*0 */ {L2_OIFs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {STRENGTH_PRFL_IDXs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    /* entry 0 of ING_SYSTEM_DESTINATION_TABLE */
    lt_entry.fields = sys_dest;
    lt_entry.nfields = sizeof(sys_dest) / sizeof(sys_dest[0]);
    lt_entry.attr = 0;
    sys_dest[0].u.val = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get
            (unit, BCMI_LTSW_SBR_PTH_ING_SYSTEM_DESTINATION, BCMI_LTSW_SBR_PET_NONE, &id));
    sys_dest[1].u.val = id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, ING_SYSTEM_DESTINATION_TABLEs, &lt_entry, NULL));

    /* entry 0 of EGR_L2_OIF */
    lt_entry.fields = l2_oif;
    lt_entry.nfields = sizeof(l2_oif) / sizeof(l2_oif[0]);
    lt_entry.attr = 0;
    l2_oif[0].u.val = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get
            (unit, BCMI_LTSW_SBR_PTH_EGR_L2_OIF, BCMI_LTSW_SBR_PET_NONE, &id));
    l2_oif[1].u.val = id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, EGR_L2_OIFs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_strength_deinit(int unit)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t sys_dest[] =
    {
        /*0*/ {SYSTEM_DESTINATION_PORTs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t l2_oif[] =
    {
        /*0 */ {L2_OIFs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    /* entry 0 of ING_SYSTEM_DESTINATION_TABLE */
    lt_entry.fields = sys_dest;
    lt_entry.nfields = sizeof(sys_dest) / sizeof(sys_dest[0]);
    lt_entry.attr = 0;
    sys_dest[0].u.val = 0;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_lt_entry_delete
            (unit, ING_SYSTEM_DESTINATION_TABLEs, &lt_entry, NULL),
         SHR_E_NOT_FOUND);

    /* entry 0 of EGR_L2_OIF */
    lt_entry.fields = l2_oif;
    lt_entry.nfields = sizeof(l2_oif) / sizeof(l2_oif[0]);
    lt_entry.attr = 0;
    l2_oif[0].u.val = 0;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_lt_entry_delete(unit, EGR_L2_OIFs, &lt_entry, NULL),
         SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_gih_cpu_init(int unit)
{
    ltsw_port_tab_field_t fields[3] = {{0}};
    int fields_num, sys_port;

    SHR_FUNC_ENTER(unit);

    fields_num = 0;
    /* port type */
    fields[fields_num].type = BCMI_PT_GIH_CPU_PORT_TYPE;
    fields[fields_num].data.n = PORT_TYPE_ETHERNET;
    fields_num ++;
    /* parser type */
    fields[fields_num].type = BCMI_PT_GIH_CPU_PARSER_TYPE;
    fields[fields_num].data.n = PARSER_TYPE_LOOPBACK;
    fields_num ++;
    /* system port */
    fields[fields_num].type = BCMI_PT_GIH_CPU_SYS_PORT;
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_port_to_sys_port(unit, 0, &sys_port));
    fields[fields_num].data.n = sys_port;
    fields_num ++;

    if (fields_num > 3) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, 0, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_gih_cpu_deinit(int unit)
{
    bcmi_lt_entry_t lt_entry;

    SHR_FUNC_ENTER(unit);

    lt_entry.nfields = 0;
    lt_entry.attr = 0;
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_lt_entry_delete(unit, PORT_GIH_CPUs, &lt_entry, NULL),
         SHR_E_NOT_FOUND);

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
xfs_ltsw_port_control_get(int unit, bcm_port_t port,
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
            (xfs_port_control_vport_get(unit, port, type, value));
        SHR_EXIT();
    }

    /* Now it could be a PROXY GPORT or BCM local port */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_control_gport_validate(unit, port, &port));

    SHR_IF_ERR_EXIT
        (xfs_port_control_port_get(unit, port, type, value));

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
xfs_ltsw_port_control_set(int unit, bcm_port_t port,
                          bcm_port_control_t type, int value)
{
    SHR_FUNC_ENTER(unit);

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
            (xfs_port_control_vport_set(unit, port, type, value));
        SHR_EXIT();
    }

    /* Now it could be a PROXY GPORT or BCM local port */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_control_gport_validate(unit, port, &port));

    SHR_IF_ERR_EXIT
        (xfs_port_control_port_set(unit, port, type, value));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_port_tab_info_get(int unit, bcmi_ltsw_port_tab_t type,
                           const ltsw_port_tab_info_t **info)
{
    SHR_FUNC_ENTER(unit);

    if ((type <= BCMI_PT_ZERO) || (type >= BCMI_PT_CNT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (xfs_port_db[unit]->port_tab_map[type].table == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (xfs_port_db[unit]->port_tab_map[type].table->name == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *info = &(xfs_port_db[unit]->port_tab_map[type]);

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_tabs_info_get(int unit, ltsw_port_tab_field_t *fields, int count)
{
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < count; i ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_port_tab_info_get(unit, fields[i].type, &fields[i].info));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_tabs_op_get(int unit, int port,
                          ltsw_port_tab_field_t *fields, int count,
                          uint32_t *op)
{
    int i;
    ltsw_port_tab_field_t *fld;

    for (i = 0, fld = fields, *op = 0; i < count; i ++, fld ++) {
        fld->op |= PORT_CFG;
        if ((fld->info->flags & SYNC_TO_TRUNK) && !(fld->op & ID_KNOWN)) {
            fld->op |= TRUNK_CFG;
        }
        *op |= fld->op;
    }

    return SHR_E_NONE;
}


int
xfs_ltsw_port_tabs_para_get(int unit, int port,
                            ltsw_port_tab_field_t *fields, int count,
                            uint32_t op)
{
    int i;
    ltsw_port_tab_field_t *fld;

    SHR_FUNC_ENTER(unit);

    for (i = 0, fld = fields; i < count; i ++, fld ++) {
        if (fld->op & ID_KNOWN) {
            continue;
        }

        if ((fld->op & op) == PORT_CFG) {
            if (fld->info->table->index_get) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (fld->info->table->index_get(unit, port, &fld->index));
            } else {
                fld->index = port;
            }
        } else if ((fld->op & op) == TRUNK_CFG) {
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_ltsw_gport_to_trunk_l2_if(unit, port, &fld->index));
            if (fld->index == L2_IF_INVALID) {
                fld->op &= ~TRUNK_CFG;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_l2_if_add(int unit, int l2_if)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t l2_iif[] =
    {
        /*0*/ {L2_IIFs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t l2_iif_ext[] =
    {
        /*0*/ {L2_IIFs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t l2_oif[] =
    {
        /*0 */ {L2_OIFs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t l2_eif[] =
    {
        /*0 */ {L2_EIF_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = l2_iif;
    lt_entry.nfields = sizeof(l2_iif) / sizeof(l2_iif[0]);
    lt_entry.attr = 0;
    l2_iif[0].u.val = l2_if;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, ING_L2_IIF_TABLEs, &lt_entry, NULL));

    lt_entry.fields = l2_iif_ext;
    lt_entry.nfields = sizeof(l2_iif_ext) / sizeof(l2_iif_ext[0]);
    lt_entry.attr = 0;
    l2_iif_ext[0].u.val = l2_if;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, ING_L2_IIF_ATTRIBUTES_TABLEs, &lt_entry, NULL));

    lt_entry.fields = l2_oif;
    lt_entry.nfields = sizeof(l2_oif) / sizeof(l2_oif[0]);
    lt_entry.attr = 0;
    l2_oif[0].u.val = l2_if;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, EGR_L2_OIFs, &lt_entry, NULL));

    lt_entry.fields = l2_eif;
    lt_entry.nfields = sizeof(l2_eif) / sizeof(l2_eif[0]);
    lt_entry.attr = 0;
    l2_eif[0].u.val = l2_if;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, L2_EIF_SYSTEM_DESTINATIONs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_l2_if_del(int unit, int l2_if)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t l2_iif[] =
    {
        /*0*/ {L2_IIFs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t l2_iif_ext[] =
    {
        /*0*/ {L2_IIFs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t l2_oif[] =
    {
        /*0 */ {L2_OIFs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t l2_eif[] =
    {
        /*0 */ {L2_EIF_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = l2_iif;
    lt_entry.nfields = sizeof(l2_iif) / sizeof(l2_iif[0]);
    lt_entry.attr = 0;
    l2_iif[0].u.val = l2_if;
    rv = bcmi_lt_entry_delete(unit, ING_L2_IIF_TABLEs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    lt_entry.fields = l2_iif_ext;
    lt_entry.nfields = sizeof(l2_iif_ext) / sizeof(l2_iif_ext[0]);
    lt_entry.attr = 0;
    l2_iif_ext[0].u.val = l2_if;
    rv = bcmi_lt_entry_delete(unit, ING_L2_IIF_ATTRIBUTES_TABLEs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    lt_entry.fields = l2_oif;
    lt_entry.nfields = sizeof(l2_oif) / sizeof(l2_oif[0]);
    lt_entry.attr = 0;
    l2_oif[0].u.val = l2_if;
    rv = bcmi_lt_entry_delete(unit, EGR_L2_OIFs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    lt_entry.fields = l2_eif;
    lt_entry.nfields = sizeof(l2_eif) / sizeof(l2_eif[0]);
    lt_entry.attr = 0;
    l2_eif[0].u.val = l2_if;
    rv = bcmi_lt_entry_delete(unit, L2_EIF_SYSTEM_DESTINATIONs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_l2_if_add(int unit, int port, int l2_if)
{
    ltsw_port_tab_field_t fields[2] = {{0}};
    int fields_num, id;

    SHR_FUNC_ENTER(unit);

    xfs_port(unit, port)->l2_if = l2_if;

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_l2_if_add(unit, l2_if));

    fields_num = 0;
    /* l2_oif for mc */
    fields[fields_num].type = BCMI_PT_EGR_MC_L2_OIF;
    fields[fields_num].data.n = l2_if;
    fields_num ++;
    /* l2_oif strength profile */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get
            (unit, BCMI_LTSW_SBR_PTH_EGR_L2_OIF, BCMI_LTSW_SBR_PET_DEF, &id));
    fields[fields_num].type = BCMI_PT_L2_OIF_STR_PROFILE_ID;
    fields[fields_num].data.n = id;
    fields_num ++;

    if (fields_num > sizeof(fields)/sizeof(fields[0])) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_l2_if_del(int unit, int port)
{
    int l2_if;

    SHR_FUNC_ENTER(unit);

    l2_if = xfs_port(unit, port)->l2_if;

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_l2_if_del(unit, l2_if));

    xfs_port(unit, port)->l2_if = 0;

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_to_l2_if(int unit, int port, int *l2_if)
{
     *l2_if = xfs_port(unit, port)->l2_if;
     return SHR_E_NONE;
}


int
xfs_ltsw_gport_to_l2_if(int unit, int port, int *l2_if)
{
    SHR_FUNC_ENTER(unit);

    if ((!BCM_GPORT_IS_SET(port)) && PORT_VALID_RANGE(unit, port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_port_to_l2_if(unit, port, l2_if));
    } else if (BCM_GPORT_IS_LOCAL(port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_port_to_l2_if(unit, port, l2_if));
    } else if (BCM_GPORT_IS_TRUNK(port)) {
        bcm_trunk_t tid = BCM_GPORT_TRUNK_GET(port);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_tid_to_l2_if(unit, tid, l2_if));
    } else if (BCM_GPORT_IS_MODPORT(port)) {
        int modid, p, mymodid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &mymodid));
        p = BCM_GPORT_MODPORT_PORT_GET(port);
        modid = BCM_GPORT_MODPORT_MODID_GET(port);

        /* Local port with modport flag. */
        if (mymodid == modid) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, p, &p));
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_ltsw_port_to_l2_if(unit, p, l2_if));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_stk_modport_to_l2_if(unit, modid, p, l2_if));
            if (*l2_if == L2_IF_INVALID) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit, "Error: Failed to "
                                     "enable the module remote port.\n")));
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_gport_to_trunk_l2_if(int unit, int gport, int *l2_if)
{
    int is_trunk;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, gport, BCMI_PT_ING_L2_IIF, l2_if));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_raw_get
            (unit, *l2_if, BCMI_PT_L2_OIF_IS_TRUNK, &is_trunk));

    if (!is_trunk) {
        *l2_if = L2_IF_INVALID;
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_l2_if_to_port(int unit, int l2_if, int *port)
{
     int p;

     PORT_ALL_ITER(unit, p) {
        if (l2_if == xfs_port(unit, p)->l2_if) {
            *port = p;
            return SHR_E_NONE;
        }
     }

     return SHR_E_NOT_FOUND;
}


int
xfs_ltsw_l2_if_to_gport(int unit, int l2_if, int *port)
{
    int rv, tid, use_gport, modid;

    SHR_FUNC_ENTER(unit);

    rv = xfs_ltsw_l2_if_to_port(unit, l2_if, port);
    if (rv == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &modid));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_switch_control_get(unit, bcmSwitchUseGport, &use_gport));
        if (use_gport) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_port_gport_get(unit, *port, port));
        } else if (modid != 0) {
            /*
             * Local port with module id != 0, will always return gport type to
             * restore the correct module id.
             */
            SHR_IF_ERR_EXIT
                (bcm_ltsw_port_gport_get(unit, *port, port));
        }
        SHR_EXIT();
    } else if (rv != SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(rv);
    }

    rv = bcmi_ltsw_trunk_l2_if_to_tid(unit, l2_if, &tid);
    if (rv == SHR_E_NONE) {
        BCM_GPORT_TRUNK_SET(*port, tid);
        SHR_EXIT();
    } else if (rv != SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(rv);
    }

    rv = bcmi_ltsw_stk_l2_if_to_modport(unit, l2_if, port);
    if (rv == SHR_E_NONE) {
        SHR_EXIT();
    } else {
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_l2_if_to_pipes(int unit, int l2_if, uint32_t *pipe_bmp)
{
    int gport = BCM_GPORT_INVALID, pipe, mymod, i, max, cnt, *ports = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_l2_if_to_gport(unit, l2_if, &gport));

    if (!BCM_GPORT_IS_SET(gport)) {
        max = 1;
        SHR_ALLOC(ports, max * sizeof(int), "ports");
        ports[0] = gport;
    } else if (BCM_GPORT_IS_TRUNK(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_local_members_get
                (unit, BCM_GPORT_TRUNK_GET(gport), 0, NULL, &max));
        SHR_ALLOC(ports, sizeof(int) * max, "ports");
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_local_members_get
                (unit, BCM_GPORT_TRUNK_GET(gport), max, ports, &cnt));
    } else if (BCM_GPORT_IS_MODPORT(gport)) {
        max = 1;
        SHR_ALLOC(ports, sizeof(int) * max, "ports");
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &mymod));
        if (mymod == BCM_GPORT_MODPORT_MODID_GET(gport)) {
            ports[0] = BCM_GPORT_MODPORT_PORT_GET(gport);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_stk_remote_port_modport_get(unit, gport, &ports[0]));
        }
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (i = 0, *pipe_bmp = 0; i < max; i ++) {
        pipe = bcmi_ltsw_dev_logic_port_pp_pipe(unit, ports[i]);
        if (pipe >= 0) {
            *pipe_bmp |= 1 << pipe;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FREE(ports);
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_mask_to_l2_if_mask(int unit, int port_mask, int *l2_if_mask)
{
     *l2_if_mask = port_mask;
     return SHR_E_NONE;
}


int
xfs_ltsw_l2_if_mask_to_port_mask(int unit, int l2_if_mask, int *port_mask)
{
     *port_mask = l2_if_mask;
     return SHR_E_NONE;
}


int
xfs_ltsw_sys_port_add(int unit, int sys_port, int flags)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t ing[] =
    {
        /*0*/ {SYSTEM_SOURCEs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t sys_dest[] =
    {
        /*0*/ {SYSTEM_DESTINATION_PORTs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t egr[] =
    {
        /*0*/ {PORT_SYSTEM_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if (flags & BCMI_LTSW_PORT_ING) {
        lt_entry.fields = ing;
        lt_entry.nfields = sizeof(ing) / sizeof(ing[0]);
        lt_entry.attr = 0;
        ing[0].u.val = sys_port;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_insert(unit, ING_SYSTEM_PORT_TABLEs, &lt_entry, NULL));

        lt_entry.fields = sys_dest;
        lt_entry.nfields = sizeof(sys_dest) / sizeof(sys_dest[0]);
        lt_entry.attr = 0;
        sys_dest[0].u.val = sys_port;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_insert(unit, ING_SYSTEM_DESTINATION_TABLEs, &lt_entry, NULL));
    }

    if (flags & BCMI_LTSW_PORT_EGR) {
        lt_entry.fields = egr;
        lt_entry.nfields = sizeof(egr) / sizeof(egr[0]);
        lt_entry.attr = 0;
        egr[0].u.val = sys_port;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_insert(unit, PORT_SYSTEM_DESTINATIONs, &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_sys_port_del(int unit, int sys_port, int flags)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t ing[] =
    {
        /*0*/ {SYSTEM_SOURCEs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t sys_dest[] =
    {
        /*0*/ {SYSTEM_DESTINATION_PORTs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    bcmi_lt_field_t egr[] =
    {
        /*0*/ {PORT_SYSTEM_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if (flags & BCMI_LTSW_PORT_ING) {
        lt_entry.fields = ing;
        lt_entry.nfields = sizeof(ing) / sizeof(ing[0]);
        lt_entry.attr = 0;
        ing[0].u.val = sys_port;
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_lt_entry_delete
                (unit, ING_SYSTEM_PORT_TABLEs, &lt_entry, NULL),
             SHR_E_NOT_FOUND);

        lt_entry.fields = sys_dest;
        lt_entry.nfields = sizeof(sys_dest) / sizeof(sys_dest[0]);
        lt_entry.attr = 0;
        sys_dest[0].u.val = sys_port;
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_lt_entry_delete
                (unit, ING_SYSTEM_DESTINATION_TABLEs, &lt_entry, NULL),
             SHR_E_NOT_FOUND);
    }

    if (flags & BCMI_LTSW_PORT_EGR) {
        lt_entry.fields = egr;
        lt_entry.nfields = sizeof(egr) / sizeof(egr[0]);
        lt_entry.attr = 0;
        egr[0].u.val = sys_port;
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_lt_entry_delete
                (unit, PORT_SYSTEM_DESTINATIONs, &lt_entry, NULL),
             SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_sys_port_add(int unit, int port, int sys_port)
{
    ltsw_port_tab_field_t fields[8] = {{0}};
    int fields_num, id, strength;

    SHR_FUNC_ENTER(unit);

    xfs_port(unit, port)->sys_port = sys_port;

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_sys_port_add
            (unit, sys_port, BCMI_LTSW_PORT_ING | BCMI_LTSW_PORT_EGR));

    fields_num = 0;
    /* ing port -> ing sys port */
    fields[fields_num].type = BCMI_PT_ING_SYS_PORT;
    fields[fields_num].data.n = sys_port;
    fields_num ++;
    /* ing sys port -> ing pp port */
    fields[fields_num].type = BCMI_PT_ING_PP_PORT;
    fields[fields_num].data.n = port;
    fields_num ++;
    /* ing sys port strength profile */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get
            (unit, BCMI_LTSW_SBR_PTH_SYS_PORT, BCMI_LTSW_SBR_PET_DEF, &id));
    fields[fields_num].type = BCMI_PT_SYS_PORT_STR_PROFILE_ID;
    fields[fields_num].data.n = id;
    fields_num ++;
    /* ing sys dest strength profile */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get
            (unit, BCMI_LTSW_SBR_PTH_ING_SYSTEM_DESTINATION,
             BCMI_LTSW_SBR_PET_DEF, &id));
    fields[fields_num].type = BCMI_PT_SYS_DEST_STR_PROFILE_ID;
    fields[fields_num].data.n = id;
    fields_num ++;
    /* loopback or hg3 header strength */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_fld_value_get
            (unit, BCMI_LTSW_SBR_TH_ING_SYSTEM_DESTINATION_TABLE,
             BCMI_LTSW_SBR_FT_L2_OIF, &strength));
    fields[fields_num].type = BCMI_PT_ING_SYS_DEST_L2_OIF_STR;
    fields[fields_num].data.n = strength;
    fields_num ++;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_fld_value_get
            (unit, BCMI_LTSW_SBR_TH_ING_SYSTEM_DESTINATION_TABLE,
             BCMI_LTSW_SBR_FT_L2MC_L3MC_L2_OIF_SYS_DST_VALID, &strength));
    fields[fields_num].type = BCMI_PT_L2MC_L3MC_L2_OIF_SYS_DST_VALID_STR;
    fields[fields_num].data.n = strength;
    fields_num ++;
    /* egr sys port isn't system trunk */
    fields[fields_num].type = BCMI_PT_DEST_SYS_PORT_IS_TRUNK;
    fields[fields_num].data.n = 0;
    fields_num ++;
    /* egr sys port -> egr local port */
    fields[fields_num].type = BCMI_PT_DEST_SYS_PORT_LPORT;
    fields[fields_num].data.n = port;
    fields_num ++;

    if (fields_num > sizeof(fields)/sizeof(fields[0])) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_sys_port_del(int unit, int port)
{
    int sys_port;

    SHR_FUNC_ENTER(unit);

    sys_port = xfs_port(unit, port)->sys_port;

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_sys_port_del
            (unit, sys_port, BCMI_LTSW_PORT_ING | BCMI_LTSW_PORT_EGR));

    xfs_port(unit, port)->sys_port = 0;

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_sys_port_move(int unit, int port, int sys_port)
{
    int old_sys_port;

    SHR_FUNC_ENTER(unit);

    old_sys_port = xfs_port(unit, port)->sys_port;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_ING_SYS_PORT, sys_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_lt_entry_move
            (unit, ING_SYSTEM_PORT_TABLEs, SYSTEM_SOURCEs,
             old_sys_port, sys_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_lt_entry_move
            (unit, ING_SYSTEM_DESTINATION_TABLEs, SYSTEM_DESTINATION_PORTs,
             old_sys_port, sys_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_lt_entry_move
            (unit, PORT_SYSTEM_DESTINATIONs, PORT_SYSTEM_IDs,
             old_sys_port, sys_port));

    xfs_port(unit, port)->sys_port = sys_port;

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_to_sys_port(int unit, int port, int *sys_port)
{
     *sys_port = xfs_port(unit, port)->sys_port;
     return SHR_E_NONE;
}


int
xfs_ltsw_gport_to_sys_port(int unit, int port, int *sys_port)
{
    SHR_FUNC_ENTER(unit);

    if (!BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_port_to_sys_port(unit, port, sys_port));
    } else if (BCM_GPORT_IS_LOCAL(port) || BCM_GPORT_IS_DEVPORT(port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port));
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_port_to_sys_port(unit, port, sys_port));
    } else if (BCM_GPORT_IS_MODPORT(port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stk_sys_port_get
                (unit, BCM_GPORT_MODPORT_MODID_GET(port),
                 BCM_GPORT_MODPORT_PORT_GET(port), sys_port));
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_gport_to_sys_ports(int unit, bcm_port_t port,
                            int array_size, int *sys_port_array, int *count)
{
    bcm_trunk_member_t *tm = NULL;
    bcm_trunk_t tid;
    int i, cnt;

    SHR_FUNC_ENTER(unit);

    if ((array_size < 0) ||
        ((array_size == 0) && (!count)) ||
        ((array_size > 0) && (!sys_port_array))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (BCM_GPORT_IS_TRUNK(port)) {
        tid = BCM_GPORT_TRUNK_GET(port);
        if (array_size > 0) {
            SHR_ALLOC(tm, sizeof(bcm_trunk_member_t) * array_size,
                      "bcm_trunk_member");
            SHR_NULL_CHECK(tm, SHR_E_MEMORY);
            sal_memset(tm, 0, sizeof(bcm_trunk_member_t) * array_size);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_trunk_get(unit, tid, NULL, array_size, tm, &cnt));
        if ((array_size > 0)) {
            if (array_size < cnt) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            for (i = 0; i < cnt; i ++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (xfs_ltsw_gport_to_sys_port(unit, tm[i].gport, sys_port_array + i));
            }
        }
        if (count) {
            *count = cnt;
        }
    } else {
        if (array_size > 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_ltsw_gport_to_sys_port(unit, port, sys_port_array));
        }
        if (count) {
            *count = 1;
        }
    }

exit:
    SHR_FREE(tm);
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_sys_port_to_port(int unit, int sys_port, int *port)
{
     int p;

     PORT_ALL_ITER(unit, p) {
        if (sys_port == xfs_port(unit, p)->sys_port) {
            *port = p;
            return SHR_E_NONE;
        }
     }

     return SHR_E_NOT_FOUND;
}


int
xfs_ltsw_sys_port_to_gport(int unit, int sys_port, int *port)
{
    int rv, mod, p;

    SHR_FUNC_ENTER(unit);

    rv = xfs_ltsw_sys_port_to_port(unit, sys_port, port);
    if (rv == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_gport_get(unit, *port, port));
        SHR_EXIT();
    } else if (rv != SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(rv);
    }

    rv = bcmi_ltsw_stk_sys_port_to_modport_get(unit, sys_port, &mod, &p);
    if (rv == SHR_E_NONE) {
        BCM_GPORT_MODPORT_SET(*port, mod, p);
    } else if (rv != SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_sys_port_map_l2_if(int unit, int sys_port, int l2_if)
{
    ltsw_port_tab_field_t fields[4] = {{0}};
    int fields_num;

    SHR_FUNC_ENTER(unit);

    fields_num = 0;
    /* ing sys port -> l2_iif */
    fields[fields_num].type = BCMI_PT_ING_L2_IIF;
    fields[fields_num].index = sys_port;
    fields[fields_num].op = ID_KNOWN;
    fields[fields_num].data.n = l2_if;
    fields_num ++;
    /* dest sys port -> l2_oif */
    fields[fields_num].type = BCMI_PT_ING_SYS_DEST_L2_OIF;
    fields[fields_num].index = sys_port;
    fields[fields_num].op = ID_KNOWN;
    fields[fields_num].data.n = l2_if;
    fields_num ++;
    /* is l2_oif a trunk */
    fields[fields_num].type = BCMI_PT_L2_OIF_IS_TRUNK;
    fields[fields_num].index = l2_if;
    fields[fields_num].op = ID_KNOWN;
    fields[fields_num].data.n = 0;
    fields_num ++;
    /* l2_oif -> egr sys port */
    fields[fields_num].type = BCMI_PT_L2_OIF_SYS_PORT;
    fields[fields_num].index = l2_if;
    fields[fields_num].op = ID_KNOWN;
    fields[fields_num].data.n = sys_port;
    fields_num ++;

    if (fields_num > 4) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, 0, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_sys_port_demap_l2_if(int unit, int sys_port, int l2_if)
{
    ltsw_port_tab_field_t fields[4] = {{0}};
    int fields_num;

    SHR_FUNC_ENTER(unit);

    fields_num = 0;
    /* ing sys port -> l2_iif */
    fields[fields_num].type = BCMI_PT_ING_L2_IIF;
    fields[fields_num].index = sys_port;
    fields[fields_num].op = ID_KNOWN;
    fields[fields_num].data.n = 0;
    fields_num ++;
    /* dest sys port -> l2_oif */
    fields[fields_num].type = BCMI_PT_ING_SYS_DEST_L2_OIF;
    fields[fields_num].index = sys_port;
    fields[fields_num].op = ID_KNOWN;
    fields[fields_num].data.n = 0;
    fields_num ++;
    /* is l2_oif a trunk */
    fields[fields_num].type = BCMI_PT_L2_OIF_IS_TRUNK;
    fields[fields_num].index = l2_if;
    fields[fields_num].op = ID_KNOWN;
    fields[fields_num].data.n = 0;
    fields_num ++;
    /* l2_oif -> egr sys port */
    fields[fields_num].type = BCMI_PT_L2_OIF_SYS_PORT;
    fields[fields_num].index = l2_if;
    fields[fields_num].op = ID_KNOWN;
    fields[fields_num].data.n = 0;
    fields_num ++;

    if (fields_num > 4) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, 0, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_sys_port_traverse(int unit, bcmi_ltsw_port_traverse_cb cb,
                           void *user_data)
{
    int port, dunit, rv;
    uint64_t u64;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ING_SYSTEM_PORT_TABLEs, &eh));

    while (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_TRAVERSE,
                                BCMLT_PRIORITY_NORMAL) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, SYSTEM_SOURCEs, &u64));
        port = u64;
        rv = cb(unit, port, user_data);
        if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_lt_add(int unit, int port)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {PORT_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = 1;
    lt_entry.attr = 0;
    field[0].u.val = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PORTs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PORT_PROPERTYs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PORT_POLICYs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PORT_ING_VISIBILITYs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PORT_EGR_VISIBILITYs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, TM_SHAPER_PORTs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, MIRROR_PORT_ENCAP_SFLOWs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PORT_MIRRORs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_insert(unit, PORT_EGR_OPAQUEs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_flood_block_lt_add(unit, port));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_lt_del(int unit, int port)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {PORT_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = 1;
    lt_entry.attr = 0;
    field[0].u.val = port;

    rv = bcmi_lt_entry_delete(unit, PORTs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = bcmi_lt_entry_delete(unit, PORT_PROPERTYs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = bcmi_lt_entry_delete(unit, PORT_POLICYs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = bcmi_lt_entry_delete(unit, PORT_ING_VISIBILITYs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = bcmi_lt_entry_delete(unit, PORT_EGR_VISIBILITYs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = bcmi_lt_entry_delete(unit, TM_SHAPER_PORTs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = bcmi_lt_entry_delete(unit, MIRROR_PORT_ENCAP_SFLOWs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = bcmi_lt_entry_delete(unit, PORT_MIRRORs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = bcmi_lt_entry_delete(unit, PORT_EGR_OPAQUEs, &lt_entry, NULL);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    rv = xfs_port_flood_block_lt_del(unit, port);
    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_learn_set(int unit, bcm_port_t port, uint32_t flags)
{
    int cml = 0;

    SHR_FUNC_ENTER(unit);

    if (!(flags & BCM_PORT_LEARN_FWD)) {
        cml |= (1 << 0);
    }
    if (flags & BCM_PORT_LEARN_CPU) {
        cml |= (1 << 1);
    }
    if (flags & BCM_PORT_LEARN_ARL) {
        cml |= (1 << 2);
    }
    if (flags & BCM_PORT_LEARN_PENDING) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_CML_NEW, cml));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_learn_get(int unit, bcm_port_t port, uint32_t *flags)
{
    int cml = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_CML_NEW, &cml));

    *flags = 0;
    if (!(cml & (1 << 0))) {
       *flags |= BCM_PORT_LEARN_FWD;
    }
    if (cml & (1 << 1)) {
       *flags |= BCM_PORT_LEARN_CPU;
    }
    if (cml & (1 << 2)) {
       *flags |= BCM_PORT_LEARN_ARL;
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_l3_mtu_set(int unit, bcm_port_t port, int size)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_MTU_EN, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_MTU, size));

    if (xfs_port_db[unit]->feature->l2_mtu) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_egr_l2_ctrl_set(unit, port, 3, 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_L2_MTU, size));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_l3_mtu_get(int unit, bcm_port_t port, int *size)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_MTU, size));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_port_tpid_set(int unit, bcm_port_t port, int type, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_ing_tpid_set
                (unit, port, BCMI_PT_ING_OTPID_ENABLE, tpid));
    }
    if (type & PAYLOAD_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_ing_tpid_set
                (unit, port, BCMI_PT_ING_PAYLOAD_OTPID_ENABLE, tpid));
    }
    if (type & OBM_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_obm_tpid_set(unit, port, tpid));
    }

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_EGR_OTPID, tpid));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_tpid_add(int unit, bcm_port_t port, int type, uint16_t tpid,
                       int color_select)
{
    SHR_FUNC_ENTER(unit);

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_ing_tpid_add
                (unit, port, BCMI_PT_ING_OTPID_ENABLE, tpid));
    }
    if (type & PAYLOAD_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_ing_tpid_add
                (unit, port, BCMI_PT_ING_PAYLOAD_OTPID_ENABLE, tpid));
    }
    if (type & OBM_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_obm_tpid_add(unit, port, tpid));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_tpid_get(int unit, bcm_port_t port, int type, uint16_t *tpid)
{
    int tpid_egr;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_EGR_OTPID, &tpid_egr));

    *tpid = tpid_egr;

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_tpid_get_all(int unit, bcm_port_t port, int type,
                           int size, uint16_t *tpid_array,
                           int *color_array, int *count)
{
    int tpid_bmp, i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_tpid_bmp_get(unit, port, type, &tpid_bmp));

    if (size == 0) {
        *count = shr_util_popcount(tpid_bmp);
        SHR_EXIT();
    }

    for (i = 0, *count = 0; tpid_bmp && (*count < size); i ++) {
        if (tpid_bmp & 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_get(unit, i, &tpid_array[*count]));

            color_array[*count] = 0;

            (*count) ++;
        }
        tpid_bmp = tpid_bmp >> 1;
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_tpid_delete(int unit, bcm_port_t port, int type, uint16_t tpid)
{
    int tpid_egr, force = 0;
    uint16_t tpid_default = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_default_get(unit, &tpid_default));

    /* if port egr TPID is deleted, set default TPID on it */
    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_EGR_OTPID, &tpid_egr));

        if (tpid == tpid_egr) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_EGR_OTPID, tpid_default));
            force = 1;
        }
    }

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_ing_tpid_delete
                (unit, port, BCMI_PT_ING_OTPID_ENABLE, tpid,
                 tpid_default, force));
    }
    if (type & PAYLOAD_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_ing_tpid_delete
                (unit, port, BCMI_PT_ING_PAYLOAD_OTPID_ENABLE, tpid,
                 tpid_default, force));
    }
    if (type & OBM_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_obm_tpid_delete(unit, port, tpid, tpid_default, force));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_tpid_delete_all(int unit, bcm_port_t port, int type)
{
    uint16_t tpid_default;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_otpid_default_get(unit, &tpid_default));

    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_ing_tpid_delete_all
                (unit, port, BCMI_PT_ING_OTPID_ENABLE, tpid_default));
    }
    if (type & PAYLOAD_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_ing_tpid_delete_all
                (unit, port, BCMI_PT_ING_PAYLOAD_OTPID_ENABLE, tpid_default));
    }
    if (type & OBM_OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_obm_tpid_delete_all(unit, port, tpid_default));
    }

    /* set default TPID on egr */
    if (type & OUTER_TPID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set
                (unit, port, BCMI_PT_EGR_OTPID, tpid_default));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_inner_tpid_set(int unit, bcm_port_t port, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set
            (unit, port, BCMI_PT_EGR_L2_OIF_VIEW, L2_OIF_VIEW_ITPID));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_EGR_ITPID, tpid));

    if (!is_aux_port(unit, port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_obm_port_control_set
                (unit, port, bcmiObmPortControlInnerTpidEnable, (tpid != 0)));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_itpid_set(unit, tpid));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_inner_tpid_get(int unit, bcm_port_t port, uint16_t *tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_itpid_get(unit, tpid));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_untagged_vlan_get(int unit, bcm_port_t port, bcm_vlan_t *vid)
{
    int vfi, type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_L2_DEST_TYPE, &type));
    if (type == DEST_TYPE_VP) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_DEFAULT_VFI, &vfi));

    *vid = vfi;

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_untagged_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vid)
{
    int type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_L2_DEST_TYPE, &type));
    if (type == DEST_TYPE_VP) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_DEFAULT_VFI, vid));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_untagged_priority_get(int unit, bcm_port_t port, int *priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_default_prio_cfi_get(unit, port, priority, NULL));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_untagged_priority_set(int unit, bcm_port_t port, int priority)
{
    int cfi;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_default_prio_cfi_get(unit, port, NULL, &cfi));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_default_prio_cfi_set(unit, port, priority, cfi));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_force_vlan_set(int unit, bcm_port_t port, bcm_vlan_t vlan,
                             int pkt_prio, uint32 flags)
{
    int en, port_type, exist;
    bcm_vlan_control_vlan_t control;

    SHR_FUNC_ENTER(unit);

    /* TD4 doesn't support prio overriding */
    if (pkt_prio >= 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* TD4 doesn't support pvlan untag */
    if (flags & BCM_PORT_FORCE_VLAN_UNTAG) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* vlan must exist */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_exist_check(unit, vlan, &exist));
    if (!exist) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* narrow view doesn't support pvlan */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_vlan_control_vlan_selective_get
            (unit, vlan, BCM_VLAN_CONTROL_VLAN_L2_VIEW_MASK, &control));
    if ((control.l2_view == bcmVlanL2ViewNarrow)
        && (flags & BCM_PORT_FORCE_VLAN_ENABLE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* ingress PVLAN check */
    en = (flags & BCM_PORT_FORCE_VLAN_ENABLE) ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, port, BCMI_PT_ING_PVLAN_EN, en));

    /* ING_VFI_TABLE.SRC_PVLAN_PORT_TYPE */
    port_type = en ? (flags & BCM_PORT_FORCE_VLAN_PORT_TYPE_MASK)
                   : BCM_PORT_FORCE_VLAN_PROMISCUOUS_PORT;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_vlan_force_port_set(unit, vlan, port_type));

    /* L2_HOST_TABLE.DST_PVLAN_PORT_TYPE */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_force_vlan_set(unit, port, vlan, flags));

    xfs_port(unit, port)->pvlan.vfi = (en ? vlan : 0);

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_force_vlan_get(int unit, bcm_port_t port, bcm_vlan_t *vlan,
                             int *pkt_prio, uint32 *flags)
{
    int en, port_type;

    SHR_FUNC_ENTER(unit);

    *vlan = 0;
    *pkt_prio = -1;
    *flags = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_ING_PVLAN_EN, &en));
    if (en) {
        *flags |= BCM_PORT_FORCE_VLAN_ENABLE;
        *vlan = xfs_port(unit, port)->pvlan.vfi;

        /* ING_VFI_TABLE.SRC_PVLAN_PORT_TYPE */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_force_port_get(unit, *vlan, &port_type));
        *flags |= port_type;
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_ing_vlan_action_set(int unit, bcm_port_t port,
                                  bcm_vlan_action_set_t *action,
                                  int action_profile_index)
{
    int tag_preserve;
    uint32_t vfi;
    ltsw_port_tab_field_t fields[4] = {{0}};
    int fields_num;

    SHR_FUNC_ENTER(unit);

    fields_num = 0;

    /* default vfi */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, action->forwarding_domain, &vfi));
    fields[fields_num].type = BCMI_PT_DEFAULT_VFI;
    fields[fields_num].data.n = vfi;
    fields_num ++;

    /* default outer priority */
    fields[fields_num].type = BCMI_PT_DEFAULT_PRI;
    fields[fields_num].data.n =
        (action->priority << 1) | (action->new_outer_cfi & 0x1);
    fields_num ++;

    /* default inner priority */
    fields[fields_num].type = BCMI_PT_DEFAULT_INNER_PRI;
    fields[fields_num].data.n =
        (action->new_inner_pkt_prio << 1) | (action->new_inner_cfi & 0x1);
    fields_num ++;

    /* vlan tag preserve */
    tag_preserve = 0;
    if (action->outer_tag == bcmVlanActionNone) {
        tag_preserve |= 1;
    } else if (action->outer_tag != bcmVlanActionDelete) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (action->inner_tag == bcmVlanActionNone) {
        tag_preserve |= 2;
    } else if (action->inner_tag != bcmVlanActionDelete) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    fields[fields_num].type = BCMI_PT_VLAN_PRESERVE;
    fields[fields_num].data.n = tag_preserve;
    fields_num ++;

    if (fields_num > 4) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, fields_num));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_ing_vlan_action_get(int unit, bcm_port_t port,
                                  bcm_vlan_action_set_t *action,
                                  int *action_profile_index)
{
    bcm_vlan_t vlan;
    ltsw_port_tab_field_t fields[4] = {{0}};
    int fields_num;

    SHR_FUNC_ENTER(unit);

    fields_num = 0;
    /* default vfi */
    fields[fields_num].type = BCMI_PT_DEFAULT_VFI;
    fields_num ++;
    /* default outer priority */
    fields[fields_num].type = BCMI_PT_DEFAULT_PRI;
    fields_num ++;
    /* default inner priority */
    fields[fields_num].type = BCMI_PT_DEFAULT_INNER_PRI;
    fields_num ++;
    /* vlan tag preserve */
    fields[fields_num].type = BCMI_PT_VLAN_PRESERVE;
    fields_num ++;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_get(unit, port, fields, fields_num));

    fields_num = 0;
    /* default vfi */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vpnid_get(unit, fields[fields_num].data.n, &vlan));
    action->forwarding_domain = vlan;
    fields_num ++;
    /* default outer priority */
    action->priority = (fields[fields_num].data.n >> 1) & 0x7;
    action->new_outer_cfi = fields[fields_num].data.n & 0x1;
    fields_num ++;
    /* default inner priority */
    action->new_inner_pkt_prio = (fields[fields_num].data.n >> 1) & 0x7;
    action->new_inner_cfi = fields[fields_num].data.n & 0x1;
    fields_num ++;
    /* vlan tag preserve */
    action->outer_tag = (fields[fields_num].data.n & 0x1) ?
                         bcmVlanActionNone : bcmVlanActionDelete;
    action->inner_tag = (fields[fields_num].data.n & 0x2) ?
                         bcmVlanActionNone : bcmVlanActionDelete;
    fields_num ++;

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_ing_vlan_action_reset(int unit, bcm_port_t port)
{
    bcm_vlan_action_set_t action;

    bcm_vlan_action_set_t_init(&action);
    action.forwarding_domain = BCM_VLAN_DEFAULT;
    action.outer_tag = bcmVlanActionDelete;
    action.inner_tag = bcmVlanActionNone;

    return xfs_ltsw_port_ing_vlan_action_set(unit, port, &action, 0);
}


int
xfs_ltsw_port_egr_vlan_xlate_enable_set(int unit, bcm_port_t port, int enable)
{
    return xfs_port_egr_l2_ctrl_set(unit, port, 0, enable ? 1 : 0);
}


int
xfs_ltsw_port_egr_vlan_xlate_enable_get(int unit, bcm_port_t port, int *enable)
{
    return xfs_port_egr_l2_ctrl_get(unit, port, 0, enable);
}


int
xfs_ltsw_port_class_set(int unit, bcm_port_t port,
                        bcm_port_class_t class, uint32_t class_id)
{
    SHR_FUNC_ENTER(unit);

    switch (class) {
        case bcmPortClassIngress:
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_port_group_set(unit, port, DIR_ING, class_id));
            break;

        case bcmPortClassEgress:
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_port_group_set(unit, port, DIR_EGR, class_id));
            break;

        case bcmPortClassFieldIngress:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_ING_CLASS_ID, class_id));
            break;

        case bcmPortClassOpaqueCtrlId:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_ING_OPAQUE_CTRL_ID, class_id));
            break;

        case bcmPortClassOpaqueCtrlId1:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_ING_OPAQUE_CTRL_ID_1, class_id));
            break;

        case bcmPortClassOpaqueCtrlId2:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_ING_OPAQUE_CTRL_ID_2, class_id));
            break;

        case bcmPortClassOpaqueCtrlId3:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_ING_OPAQUE_CTRL_ID_3, class_id));
            break;

        case bcmPortClassEgressOpaqueCtrlId:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set
                    (unit, port, BCMI_PT_EGR_OPAQUE_CTRL_ID, class_id));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_class_get(int unit, bcm_port_t port,
                        bcm_port_class_t class, uint32_t *class_id)
{
    int id;

    SHR_FUNC_ENTER(unit);

    switch (class) {
        case bcmPortClassIngress:
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_port_group_get(unit, port, DIR_ING, &id));
            break;

        case bcmPortClassEgress:
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_port_group_get(unit, port, DIR_EGR, &id));
            break;

        case bcmPortClassFieldIngress:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_ING_CLASS_ID, &id));
            break;

        case bcmPortClassOpaqueCtrlId:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_ING_OPAQUE_CTRL_ID, &id));
            break;

        case bcmPortClassOpaqueCtrlId1:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_ING_OPAQUE_CTRL_ID_1, &id));
            break;

        case bcmPortClassOpaqueCtrlId2:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_ING_OPAQUE_CTRL_ID_2, &id));
            break;

        case bcmPortClassOpaqueCtrlId3:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_ING_OPAQUE_CTRL_ID_3, &id));
            break;

        case bcmPortClassEgressOpaqueCtrlId:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get
                    (unit, port, BCMI_PT_EGR_OPAQUE_CTRL_ID, &id));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

    *class_id = id;

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_group_to_pipes(int unit, int port_group, int flags, uint32_t *pipe_bmp)
{
    int pipe, num_pipe, dir;
    bcm_pbmp_t pbmp;
    bcm_port_group_range_t range;

    SHR_FUNC_ENTER(unit);

    *pipe_bmp = 0;
    num_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    dir = (flags == BCMI_LTSW_PORT_ING) ? DIR_ING : DIR_EGR;

    /* Get pipe from vp info if port group is dvp group. */
    if(dir == DIR_EGR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_group_range_get(unit, bcmPortGroupDvp, &range));
        if (port_group <= range.max && port_group >= range.min) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_virtual_vp_group_pipe_get(unit,
                                                     port_group, pipe_bmp));
            SHR_EXIT();
        }
    }

    for (pipe = 0; pipe < num_pipe; pipe ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dev_pp_pipe_logic_pbmp(unit, pipe, &pbmp));

        if (xfs_port_group_count(unit, pbmp, dir, port_group)) {
            *pipe_bmp |= (1 << pipe);
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_dscp_phb_map_mode_set(int unit, bcm_port_t port, int mode)
{
    int en;

    SHR_FUNC_ENTER(unit);

    switch (mode) {
        case BCM_PORT_DSCP_MAP_NONE:
            en = 0;
            break;

        case BCM_PORT_DSCP_MAP_ALL:
            en = 1;
            break;

        case BCM_PORT_DSCP_MAP_ZERO:
        case BCM_PORT_DSCP_MAP_IPV4_ONLY:
        case BCM_PORT_DSCP_MAP_IPV6_ONLY:
        case BCM_PORT_DSCP_MAP_IPV4_NONE:
        case BCM_PORT_DSCP_MAP_IPV6_NONE:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set
            (unit, port, BCMI_PT_DSCP_MAP_TO_PHB, en));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_dscp_phb_map_mode_get(int unit, bcm_port_t port, int *mode)
{
    int en;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_DSCP_MAP_TO_PHB, &en));

    *mode = en ? BCM_PORT_DSCP_MAP_ALL : BCM_PORT_DSCP_MAP_NONE;

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_attach_phb(int unit, bcm_port_t port)
{
    return SHR_E_NONE;
}


int
xfs_ltsw_port_detach_phb(int unit, bcm_port_t port)
{
    return SHR_E_NONE;
}


int
xfs_ltsw_port_egr_shaper_set(int unit, bcm_port_t port, uint32_t bandwidth,
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
xfs_ltsw_port_egr_shaper_get(int unit, bcm_port_t port, uint32_t *bandwidth,
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
xfs_ltsw_port_queue_count_get(int unit, bcm_port_t port, uint32_t *count)
{
    int cosq, num_ucq, num_mcq;
    uint64_t val64;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, &num_mcq));

    *count = 0;

    for (cosq = 0; cosq < num_ucq; cosq ++) {
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

    for (; cosq < num_ucq + num_mcq; cosq ++) {
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
xfs_ltsw_port_stat_attach(int unit, bcm_port_t port, uint32_t stat_id)
{
    bcmi_ltsw_flexctr_counter_info_t info = {0};
    int ctr_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_id, &info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_flexctr_info_check(&info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_flexctr_id_get(unit, port, info.direction, &ctr_id));

    if (ctr_id == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit, stat_id));
        SHR_IF_ERR_CONT
            (xfs_port_flexctr_id_set
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
xfs_ltsw_port_stat_detach(int unit, bcm_port_t port, uint32_t stat_id)
{
    bcmi_ltsw_flexctr_counter_info_t info = {0};
    int ctr_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_id, &info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_flexctr_info_check(&info));
    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_flexctr_id_get(unit, port, info.direction, &ctr_id));

    if (ctr_id == info.action_index) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_port_flexctr_id_set
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
xfs_ltsw_port_flexstate_attach(int unit, bcm_port_t port, uint32_t action_id)
{
    bcmi_ltsw_flexstate_counter_info_t info = {0};
    int state_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexstate_counter_id_info_get(unit, action_id, &info));
    if (info.source != bcmFlexstateSourceIngPort) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_ING_FLEX_STATE_ID, &state_id));
    if (state_id == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexstate_attach_counter_id_status_update(unit, action_id));
        SHR_IF_ERR_CONT
            (bcmi_ltsw_port_tab_set
                (unit, port, BCMI_PT_ING_FLEX_STATE_ID, info.action_index));
        if (SHR_FUNC_ERR()) {
            (void)
            bcmi_ltsw_flexstate_detach_counter_id_status_update(unit, action_id);
        }
    } else if (state_id != info.action_index) {
        /* Already attached a different flex state action. */
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_flexstate_detach(int unit, bcm_port_t port, uint32_t action_id)
{
    bcmi_ltsw_flexstate_counter_info_t info = {0};
    int state_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexstate_counter_id_info_get(unit, action_id, &info));
    if (info.source != bcmFlexstateSourceIngPort) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_ING_FLEX_STATE_ID, &state_id));
    if (state_id == info.action_index) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set
                (unit, port, BCMI_PT_ING_FLEX_STATE_ID, BCMI_LTSW_FLEXCTR_ACTION_INVALID));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexstate_detach_counter_id_status_update(unit, action_id));
    } else {
        /* Already attached a different flex counter action. */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_flood_block_set(int unit, bcm_port_t ing_port, bcm_port_t egr_port,
                              uint32_t flags)
{
    int section, block[BLOCK_MASK_SECTION_NUM];
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PROFILE_SECTIONs,       BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {PORT_ING_EGR_BLOCK_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {MASK_EGR_PORTSs,        BCMI_LT_FIELD_F_SET|BCMI_LT_FIELD_F_ARRAY|BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    if (flags & BCM_PORT_FLOOD_BLOCK_ALL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    for (section = 0; section < BLOCK_MASK_SECTION_NUM; section ++) {
        block[section] = flags & block_mask_section(unit)->traffics[section];
        if ((block[section] != 0) &&
            (block[section] != block_mask_section(unit)->traffics[section])) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }


    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[1].u.val = ing_port;
    fields[2].idx = egr_port;

    for (section = 0; section < BLOCK_MASK_SECTION_NUM; section ++) {
        fields[0].u.val = section;
        fields[2].u.val = block[section] ? 0 : 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set(unit, PORT_ING_EGR_BLOCK_0s, &lt_entry, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_flood_block_get(int unit, bcm_port_t ing_port, bcm_port_t egr_port,
                              uint32_t *flags)
{
    int section;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PROFILE_SECTIONs,       BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {PORT_ING_EGR_BLOCK_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {MASK_EGR_PORTSs,        BCMI_LT_FIELD_F_GET|BCMI_LT_FIELD_F_ARRAY, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;
    fields[1].u.val = ing_port;
    fields[2].idx = egr_port;
    *flags = 0;

    for (section = 0; section < BLOCK_MASK_SECTION_NUM; section ++) {
        fields[0].u.val = section;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_get
                (unit, PORT_ING_EGR_BLOCK_0s, &lt_entry, NULL, NULL));
        *flags |=
            (fields[2].u.val ? 0: block_mask_section(unit)->traffics[section]);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_egress_set(int unit, bcm_port_t port, int modid, bcm_pbmp_t pbmp)
{
    int tid, id, pid = -1, pid_old;
    xfs_port_egr_mask_profile_t profile;

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

    BCM_GPORT_MODPORT_SET(port, modid, port);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_EGR_MASK_ID, &pid_old));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_egr_mask_profile_get(unit, pid_old, &profile));

    /* input pbmp is allowed ports */
    BCM_PBMP_ASSIGN(profile.pbmp, pbmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_egr_mask_profile_add(unit, &profile, &pid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set
            (unit, port, BCMI_PT_EGR_MASK_ID, pid));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_egr_mask_profile_delete(unit, pid_old));

exit:
    if (SHR_FUNC_ERR() && (pid != -1)) {
        (void)xfs_port_egr_mask_profile_delete(unit, pid);
    }
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_egress_get(int unit, bcm_port_t port, int modid, bcm_pbmp_t *pbmp)
{
    int tid, id, pid;
    xfs_port_egr_mask_profile_t profile;

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

    BCM_GPORT_MODPORT_SET(port, modid, port);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, port, BCMI_PT_EGR_MASK_ID, &pid));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_egr_mask_profile_get(unit, pid, &profile));

    /* input pbmp is allowed ports */
    BCM_PBMP_ASSIGN(*pbmp, profile.pbmp);

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_match_add(int unit, bcm_gport_t port,
                        bcm_port_match_info_t *match)
{
    int vp;
    bcmi_ltsw_mpls_port_match_t mpls_match;

    SHR_FUNC_ENTER(unit);

    if  (BCM_GPORT_IS_MPLS_PORT(port)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_MPLS));
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    switch (match->match) {
        case BCM_PORT_MATCH_NONE:
            break;

        case BCM_PORT_MATCH_PORT_GROUP_VLAN:
            if (BCM_GPORT_IS_MPLS_PORT(port)) {
                sal_memset(&mpls_match, 0, sizeof(mpls_match));
                mpls_match.criteria = bcmiLtswMplsPortMatchPortGrpVlan;
                mpls_match.match_vlan = match->match_vlan;
                mpls_match.port_group = match->port;
                mpls_match.vpn = match->vpn;
                if (match->action) {
                    sal_memcpy(&mpls_match.action, match->action,
                               sizeof(bcm_vlan_action_set_t));
                } else {
                    bcm_vlan_action_set_t_init(&mpls_match.action);
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_mpls_port_match_set(unit, port, &mpls_match));
            }
            break;

        case BCM_PORT_MATCH_PORT_GROUP_INNER_VLAN:
            if (BCM_GPORT_IS_MPLS_PORT(port)) {
                sal_memset(&mpls_match, 0, sizeof(mpls_match));
                mpls_match.criteria = bcmiLtswMplsPortMatchPortGrpInnerVlan;
                mpls_match.match_inner_vlan = match->match_inner_vlan;
                mpls_match.port_group = match->port;
                mpls_match.vpn = match->vpn;
                if (match->action) {
                    sal_memcpy(&mpls_match.action, match->action,
                               sizeof(bcm_vlan_action_set_t));
                } else {
                    bcm_vlan_action_set_t_init(&mpls_match.action);
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_mpls_port_match_set(unit, port, &mpls_match));
            }
            break;

        case BCM_PORT_MATCH_PORT_GROUP_VLAN_STACKED:
            if (BCM_GPORT_IS_MPLS_PORT(port)) {
                sal_memset(&mpls_match, 0, sizeof(mpls_match));
                mpls_match.criteria = bcmiLtswMplsPortMatchPortGrpVlanStacked;
                mpls_match.match_vlan = match->match_vlan;
                mpls_match.match_inner_vlan = match->match_inner_vlan;
                mpls_match.port_group = match->port;
                mpls_match.vpn = match->vpn;
                if (match->action) {
                    sal_memcpy(&mpls_match.action, match->action,
                               sizeof(bcm_vlan_action_set_t));
                } else {
                    bcm_vlan_action_set_t_init(&mpls_match.action);
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_mpls_port_match_set(unit, port, &mpls_match));
            }
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_match_delete(int unit, bcm_gport_t port,
                           bcm_port_match_info_t *match)
{
    int vp;
    bcmi_ltsw_mpls_port_match_t mpls_match;

    SHR_FUNC_ENTER(unit);

    if  (BCM_GPORT_IS_MPLS_PORT(port)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_MPLS));
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    switch (match->match) {
        case BCM_PORT_MATCH_NONE:
            break;

        case BCM_PORT_MATCH_PORT_GROUP_VLAN:
            if (BCM_GPORT_IS_MPLS_PORT(port)) {
                mpls_match.criteria = bcmiLtswMplsPortMatchPortGrpVlan;
                mpls_match.match_vlan = match->match_vlan;
                mpls_match.port_group = match->port;
                mpls_match.vpn = match->vpn;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_mpls_port_match_clear(unit, port, &mpls_match));
            }
            break;

        case BCM_PORT_MATCH_PORT_GROUP_INNER_VLAN:
            if (BCM_GPORT_IS_MPLS_PORT(port)) {
                mpls_match.criteria = bcmiLtswMplsPortMatchPortGrpInnerVlan;
                mpls_match.match_inner_vlan = match->match_inner_vlan;
                mpls_match.port_group = match->port;
                mpls_match.vpn = match->vpn;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_mpls_port_match_clear(unit, port, &mpls_match));
            }
            break;

        case BCM_PORT_MATCH_PORT_GROUP_VLAN_STACKED:
            if (BCM_GPORT_IS_MPLS_PORT(port)) {
                mpls_match.criteria = bcmiLtswMplsPortMatchPortGrpVlanStacked;
                mpls_match.match_vlan = match->match_vlan;
                mpls_match.match_inner_vlan = match->match_inner_vlan;
                mpls_match.port_group = match->port;
                mpls_match.vpn = match->vpn;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_mpls_port_match_clear(unit, port, &mpls_match));
            }
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_match_multi_get(int unit, bcm_gport_t port, int size,
                              bcm_port_match_info_t *array, int *count)
{
    int vp, i;
    bcmi_ltsw_mpls_port_match_t *mpls_match = NULL;

    SHR_FUNC_ENTER(unit);

    if  (BCM_GPORT_IS_MPLS_PORT(port)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vp_used_get(unit, vp, BCMI_LTSW_VP_TYPE_MPLS));
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    if (BCM_GPORT_IS_MPLS_PORT(port)) {
        if (size == 0) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_mpls_port_match_multi_get
                    (unit, port, 0, NULL, count));
            SHR_EXIT();
        }

        SHR_ALLOC(mpls_match, sizeof(bcmi_ltsw_mpls_port_match_t) * size,
                  "mpls_match");
        sal_memset(mpls_match, 0, sizeof(bcmi_ltsw_mpls_port_match_t) * size);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_mpls_port_match_multi_get
                (unit, port, size, mpls_match, count));
        for (i = 0; i < *count; i ++) {
            bcm_port_match_info_t_init(&array[i]);
            switch (mpls_match[i].criteria) {
                case bcmiLtswMplsPortMatchPortGrpVlan:
                    array[i].match = BCM_PORT_MATCH_PORT_GROUP_VLAN;
                    array[i].port = mpls_match[i].port_group;
                    array[i].match_vlan = mpls_match[i].match_vlan;
                    array[i].vpn = mpls_match[i].vpn;
                    if (array[i].action) {
                        sal_memcpy(array[i].action, &mpls_match[i].action,
                                   sizeof(bcm_vlan_action_set_t));
                    }
                    break;

                case bcmiLtswMplsPortMatchPortGrpInnerVlan:
                    array[i].match = BCM_PORT_MATCH_PORT_GROUP_INNER_VLAN;
                    array[i].port = mpls_match[i].port_group;
                    array[i].match_inner_vlan = mpls_match[i].match_inner_vlan;
                    array[i].vpn = mpls_match[i].vpn;
                    if (array[i].action) {
                        sal_memcpy(array[i].action, &mpls_match[i].action,
                                   sizeof(bcm_vlan_action_set_t));
                    }
                    break;

                case bcmiLtswMplsPortMatchPortGrpVlanStacked:
                    array[i].match = BCM_PORT_MATCH_PORT_GROUP_VLAN_STACKED;
                    array[i].port = mpls_match[i].port_group;
                    array[i].match_vlan = mpls_match[i].match_vlan;
                    array[i].match_inner_vlan = mpls_match[i].match_inner_vlan;
                    array[i].vpn = mpls_match[i].vpn;
                    if (array[i].action) {
                        sal_memcpy(array[i].action, &mpls_match[i].action,
                                   sizeof(bcm_vlan_action_set_t));
                    }
                    break;

                default:
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    }

exit:
    SHR_FREE(mpls_match);
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_hg3_enable_set(int unit, bcm_port_t port, int enable)
{
    int type;
    ltsw_port_tab_field_t fields[4] = {{0}};

    SHR_FUNC_ENTER(unit);

    type = (enable ? PORT_TYPE_SYSTEM : PORT_TYPE_ETHERNET);

    fields[0].type = BCMI_PT_ING_PORT_TYPE;
    fields[0].data.n = (enable ? PORT_TYPE_SYSTEM : PORT_TYPE_ETHERNET);
    fields[1].type = BCMI_PT_EGR_ING_PORT_TYPE;
    fields[1].data.n = (enable ? PORT_TYPE_SYSTEM : PORT_TYPE_ETHERNET);
    fields[2].type = BCMI_PT_PORT_PARSER_TYPE;
    fields[2].data.n = (enable ? PARSER_TYPE_SYSTEM : PARSER_TYPE_ETHERNET);
    fields[3].type = BCMI_PT_SHAPER_IFG_ADJ;
    fields[3].data.n = (enable ? 12 : 0);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_port_tab_multi_set(unit, port, fields, 4));

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_port_egr_port_ctrl_set(unit, port, type, -1, -1));

    if (!is_aux_port(unit, port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_obm_port_control_set
                (unit, port, bcmiObmPortControlHeaderType, (enable ? 1 : 0)));
    }

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_hg3_enable_get(int unit, bcm_port_t port, int *enable)
{
    int type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port, BCMI_PT_ING_PORT_TYPE, &type));

    *enable = (type == PORT_TYPE_SYSTEM) ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_hg3_eth_type_set(int unit, int type)
{
    SHR_FUNC_ENTER(unit);

    /* pipeline HG3 etype based on device */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, 0, BCMI_PT_ING_HG3_ETYPE, type));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set(unit, 0, BCMI_PT_EGR_HG3_ETYPE, type));

    /* OBM HG3 etype based on PM */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_obm_pm_port_control_set
            (unit, -1, bcmiObmPortControlHigig3Ethertype, type));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_hg3_eth_type_get(int unit, int *type)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, 0, BCMI_PT_ING_HG3_ETYPE, type));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_ifp_egr_vlan_check_enable_set(int unit, uint32_t enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_set
            (unit, PROFILE_IFP, BCMI_PT_ING_EGR_VLAN_MEMBERSHIP_CHECK, !enable));

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_ifp_egr_vlan_check_enable_get(int unit, uint32_t *enable)
{
    int en = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get
            (unit, PROFILE_IFP, BCMI_PT_ING_EGR_VLAN_MEMBERSHIP_CHECK, &en));

    *enable = !en;

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_port_vlan_check_disable_get(int unit, bcm_port_t port, uint16_t *flags)
{
    *flags = xfs_port_ha(unit, port)->vlan_check_disable;
    return SHR_E_NONE;
}


int
xfs_ltsw_port_stg_check_disable_get(int unit, bcm_port_t port, uint16_t *flags)
{
    *flags = xfs_port_ha(unit, port)->stg_check_disable;
    return SHR_E_NONE;
}


int
xfs_ltsw_port_dump_sw(int unit)
{
    xfs_port_info_t *pInfo;
    bcmint_port_xfs_ha_info_t *ha;
    int port;

    LOG_CLI((BSL_META_U(unit, "\nSW Information PORT XFS - Unit %d\n"),
             unit));
    LOG_CLI((BSL_META_U(unit, "\n  =========== xfs feature info ===========\n\n")));
    LOG_CLI((BSL_META_U(unit, "  l2_mtu [%d]\n"), xfs_port_db[unit]->feature->l2_mtu));

    LOG_CLI((BSL_META_U(unit, "\n  =========== xfs port info ===========\n\n")));
    LOG_CLI((BSL_META_U(unit, "             sys_port  l2_if  pvlan.vfi  group(i/e)\n")));
    PORT_ALL_ITER(unit, port) {
        pInfo = xfs_port(unit, port);
        LOG_CLI((BSL_META_U(unit, "  %-5s %3d   %4d     %4d    %5d     %4d/%-4d\n"),
                 PORT(unit, port).port_name, port,
                 pInfo->sys_port, pInfo->l2_if, pInfo->pvlan.vfi,
                 pInfo->port_group[DIR_ING], pInfo->port_group[DIR_EGR]));
    }

    LOG_CLI((BSL_META_U(unit, "\n  =========== xfs port HA info ===========\n\n")));
    LOG_CLI((BSL_META_U(unit, "             vln_chk_dis  stg_chk_dis\n")));
    PORT_ALL_ITER(unit, port) {
        ha = xfs_port_ha(unit, port);
        LOG_CLI((BSL_META_U(unit, "  %-5s %3d    0x%04X       0x%04X\n"),
                 PORT(unit, port).port_name, port,
                 ha->vlan_check_disable, ha->stg_check_disable));
    }

    return SHR_E_NONE;
}


int
xfs_ltsw_l2_if_init(int unit, int size)
{
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_FREE(l2_if_pool[unit].node);
    SHR_ALLOC(l2_if_pool[unit].node, sizeof(l2_if_node_t) * (size + 1),
              "l2_if_pool");
    SHR_NULL_CHECK(l2_if_pool[unit].node, SHR_E_MEMORY);
    for (i = 0; i < size; i ++) {
        l2_if_pool[unit].node[i].next = i + 1;
    }
    l2_if_pool[unit].node[size].next = L2_IF_INVALID;       /* guard node */
    l2_if_pool[unit].head = 0;
    l2_if_pool[unit].size = size;
    l2_if_pool[unit].free = size;
    l2_if_pool[unit].lock = sal_mutex_create("l2_iif_lock");

exit:
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_l2_if_deinit(int unit)
{
    SHR_FREE(l2_if_pool[unit].node);
    sal_mutex_destroy(l2_if_pool[unit].lock);
    sal_memset(&l2_if_pool[unit], 0x0, sizeof(l2_if_pool_t));

    return SHR_E_NONE;
}


int
xfs_ltsw_l2_if_reserve(int unit, int l2_if, int num)
{
    int i, end;

    SHR_FUNC_ENTER(unit);
    L2_IF_LOCK(unit);

    if ((l2_if < 0) || (num < 0) || (l2_if + num > l2_if_pool[unit].size)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    for (i = l2_if, end = l2_if + num; i < end; i ++) {
        if (l2_if_pool[unit].node[i].next == L2_IF_INVALID) {
            /* already reserved */
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    /* find the previous node */
    for (i = l2_if - 1; i >= 0; i --) {
        if (l2_if_pool[unit].node[i].next == l2_if) {
            break;
        }
    }
    if (i < 0) {
        l2_if_pool[unit].head = l2_if_pool[unit].node[end - 1].next;
    } else {
        l2_if_pool[unit].node[i].next = l2_if_pool[unit].node[end - 1].next;
    }
    l2_if_pool[unit].free -= num;
    for (i = l2_if, end = l2_if + num; i < end; i ++) {
        l2_if_pool[unit].node[i].next = L2_IF_INVALID;
    }

exit:
    L2_IF_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_l2_if_alloc(int unit, int *l2_if)
{
    SHR_FUNC_ENTER(unit);
    L2_IF_LOCK(unit);

    SHR_NULL_CHECK(l2_if, SHR_E_PARAM);

    /* the pool is not empty */
    if (l2_if_pool[unit].free > 0) {
        *l2_if = l2_if_pool[unit].head;
        l2_if_pool[unit].head = l2_if_pool[unit].node[*l2_if].next;
        l2_if_pool[unit].node[*l2_if].next = L2_IF_INVALID;
        l2_if_pool[unit].free --;
    } else {
        SHR_ERR_EXIT(SHR_E_EMPTY);
    }

exit:
    L2_IF_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_l2_if_free(int unit, int l2_if)
{
    SHR_FUNC_ENTER(unit);
    L2_IF_LOCK(unit);

    if ((l2_if < 0) || (l2_if >= l2_if_pool[unit].size)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* the free l2_if is not in pool */
    if (l2_if_pool[unit].node[l2_if].next == L2_IF_INVALID) {
        l2_if_pool[unit].node[l2_if].next = l2_if_pool[unit].head;
        l2_if_pool[unit].head = l2_if;
        l2_if_pool[unit].free ++;
    } else {
        SHR_ERR_EXIT(SHR_E_FULL);
    }

exit:
    L2_IF_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
xfs_ltsw_l2_if_dump(int unit)
{
    int id, cnt;

    LOG_CLI((BSL_META_U(unit, "\nSW Information L2_IF - Unit %d\n\n"),
             unit));
    LOG_CLI((BSL_META_U(unit, "  L2_IF pool: %d/%d\n"),
             l2_if_pool[unit].free, l2_if_pool[unit].size));
    LOG_CLI((BSL_META_U(unit, "\n  ========================= Available ID =========================\n\n")));
    for (id = l2_if_pool[unit].head, cnt = 0;
         (id != L2_IF_INVALID) && (id != l2_if_pool[unit].size);
         id = l2_if_pool[unit].node[id].next) {
        LOG_CLI((BSL_META_U(unit, "%8d"), id));
        if (cnt % 8 == 7) {
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
        if (cnt ++ > l2_if_pool[unit].size) {
            LOG_CLI((BSL_META_U(unit, "ID pool corruption!\n")));
            return SHR_E_INTERNAL;
        }
    }
     LOG_CLI((BSL_META_U(unit, "\n")));

     return SHR_E_NONE;
}

