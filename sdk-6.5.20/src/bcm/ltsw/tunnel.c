/*! \file tunnel.c
 *
 * L3 tunnel management.
 * This file contains the management for L3 tunnel.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/tunnel.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/tunnel.h>
#include <bcm_int/ltsw/mbcm/tunnel.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/index_table_mgmt.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/qos.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/types.h>
#include <bcm_int/ltsw/feature.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_TUNNEL

typedef struct ltsw_tunnel_info_s {
    /* Tunnel initialized flag. */
    int initialized;

    /*! Tunnel module lock. */
    sal_mutex_t tunnel_lock;
} ltsw_tunnel_info_t;

static ltsw_tunnel_info_t ltsw_tunnel_info[BCM_MAX_NUM_UNITS];
#define TUNNEL_INFO(unit) (&ltsw_tunnel_info[unit])

/*! Cause a routine to return SHR_E_INIT if tunnel subsystem is not initialized. */
#define TUNNEL_INIT_CHECK(unit) \
    do { \
        if (TUNNEL_INFO(unit)->initialized == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0) \

/*! Tunnel mutex lock. */
#define TUNNEL_LOCK(unit) \
    do { \
        if (TUNNEL_INFO(unit)->tunnel_lock) { \
            sal_mutex_take(TUNNEL_INFO(unit)->tunnel_lock, SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        }\
    } while (0)

/*! Tunnel mutex unlock. */
#define TUNNEL_UNLOCK(unit) \
    do { \
        if (TUNNEL_INFO(unit)->tunnel_lock) { \
            sal_mutex_give(TUNNEL_INFO(unit)->tunnel_lock); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0)

/* 0~1K entries reserved for IP tunnel */
#define BCMI_LTSW_TUNNEL_ENCAP_SEQ_NUM_BASE_TNL    0
#define BCMI_LTSW_TUNNEL_ENCAP_SEQ_NUM_SIZE_TNL    1024
/* (1K~1k+16) entries reserved for mirror */
#define BCMI_LTSW_TUNNEL_ENCAP_SEQ_NUM_BASE_MIRROR 1024
#define BCMI_LTSW_TUNNEL_ENCAP_SEQ_NUM_SIZE_MIRROR 16
/* (2K~ ) entries reserved for VP */
#define BCMI_LTSW_TUNNEL_ENCAP_SEQ_NUM_BASE_VP     2048
#define BCMI_LTSW_TUNNEL_ENCAP_SEQ_NUM_SIZE_VP     (14 * 1024)

typedef struct ltsw_tnl_encap_seq_info_s {
    /* Indicate if encap sequence number/profile is inited or not. */
    int tnl_encap_seq_prf_inited;
    /* Encap sequence number mutex lock. */
    sal_mutex_t encap_seq_lock;
} ltsw_tnl_encap_seq_info_t;
static ltsw_tnl_encap_seq_info_t ltsw_tnl_encap_seq_info[BCM_MAX_NUM_UNITS];
#define TUNNEL_ENCAP_SEQ_INFO(unit) (&ltsw_tnl_encap_seq_info[unit])

#define TUNNEL_SEQ_PRF_INIT_CHECK(unit) \
    do { \
        if (TUNNEL_ENCAP_SEQ_INFO(unit)->tnl_encap_seq_prf_inited == 0) {\
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0) \

/*! Tunnel mutex lock. */
#define TUNNEL_SEQ_NUM_LOCK(unit) \
    do { \
        if (TUNNEL_ENCAP_SEQ_INFO(unit)->encap_seq_lock) { \
            sal_mutex_take(TUNNEL_ENCAP_SEQ_INFO(unit)->encap_seq_lock, \
                           SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        }\
    } while (0)

/*! Tunnel mutex unlock. */
#define TUNNEL_SEQ_NUM_UNLOCK(unit) \
    do { \
        if (TUNNEL_ENCAP_SEQ_INFO(unit)->encap_seq_lock) { \
            sal_mutex_give(TUNNEL_ENCAP_SEQ_INFO(unit)->encap_seq_lock); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Clean up tunnel module resources.
 *
 * Clean up tunnel module resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_free_resource(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_tunnel_free_resource(unit));

    SHR_FUNC_EXIT();
}

/*!
 * \brief Translate tunnel specific flags to shared (table management) flags.
 *
 * \param [in] unit Unit number.
 * \param [in] tnl_flags Tunnel table flags.
 * \param [out] shr_flags Shared flags.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_flags_to_shr(int unit, uint32_t tnl_flags, uint32_t *shr_flags)
{
    uint32 flag;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(shr_flags, SHR_E_PARAM);

    /*
     * Reserve entry Zero in EGR_TUNNEL table. This reserved entry
     * will not be allocated. The purpose of this entry is that
     * any interface which points to this entry will be treated
     * as a tunnel with no encapsulation or 'NO-protocol" tunnel.
     */
    flag = BCMI_LTSW_IDX_TBL_OP_SKIP_INDEX_ZERO;
    flag |= BCMI_LTSW_IDX_TBL_OP_MATCH_DISABLE;

    if (tnl_flags & BCM_TUNNEL_REPLACE) {
       flag |= BCMI_LTSW_IDX_TBL_OP_REPLACE;
    }

    if (tnl_flags & BCM_TUNNEL_WITH_ID) {
       flag |= BCMI_LTSW_IDX_TBL_OP_WITH_ID;
    }

    *shr_flags = flag;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an IP tunnel initiator.
 *
 * \param [in] unit Unit number.
 * \param [in] tunnel_id Tunnel ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_initiator_destroy(int unit, bcm_gport_t tunnel_id)
{
    int tnl_idx;
    bcmi_ltsw_tunnel_type_t tnl_type;

    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    BCMI_LTSW_GPORT_TUNNEL_ID_GET(tunnel_id, tnl_type, tnl_idx);
    if ((tnl_type != bcmiTunnelTypeIpL3) &&
        (tnl_type != bcmiTunnelTypeIp6L3)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (tnl_idx == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Remove tunnel initiator entry from LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_initiator_del(unit, 0, tunnel_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a tunnel initiator with given properties.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface info. (Should be NULL).
 * \param [in] tunnel The tunnel header information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_initiator_create(int unit, bcm_l3_intf_t *intf,
                             bcm_tunnel_initiator_t *tnl_init_info)
{
    int tnl_idx;
    int support;
    uint32_t shr_flags;
    bcmi_ltsw_qos_map_type_t qos_type;
    bcmi_ltsw_tunnel_type_t tnl_type;
    int rv;

    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    if (intf != NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(tnl_init_info, SHR_E_PARAM);

    /* Validate TTL. */
    if (!BCM_TTL_VALID(tnl_init_info->ttl)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate IP tunnel DSCP SEL. */
    if (tnl_init_info->dscp_sel >= bcmTunnelDscpCount ||
        tnl_init_info->dscp_sel < bcmTunnelDscpAssign) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate IP tunnel DSCP value. */
    if (tnl_init_info->dscp > 63 || tnl_init_info->dscp < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate IP tunnel ECN value. */
    if (tnl_init_info->ecn > 3) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (tnl_init_info->dscp_sel == bcmTunnelDscpMap) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_qos_map_id_resolve(unit, tnl_init_info->qos_map_id,
                                          &qos_type, NULL));
        if (qos_type != bcmiQosMapTypeTnlEcnDscpEgress) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (BCMI_LTSW_TUNNEL_OUTER_HEADER_IPV6(tnl_init_info->type)) {
        if (tnl_init_info->flow_label_sel >= bcmTunnelFlowLabelCount ||
            tnl_init_info->flow_label_sel < bcmTunnelFlowLabelAssign) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (tnl_init_info->flow_label >= (1 << 20)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_type_support_check(unit, tnl_init_info->type, NULL,
                                             &support));
    if (support == FALSE) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (tnl_init_info->flags & BCM_TUNNEL_WITH_ID) {
        BCMI_LTSW_GPORT_TUNNEL_ID_GET(tnl_init_info->tunnel_id, tnl_type,
                                      tnl_idx);
        if ((tnl_type != bcmiTunnelTypeIpL3) &&
            (tnl_type != bcmiTunnelTypeIp6L3)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (tnl_idx == 0) {
            /* Index 0 is reserved. */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        rv = mbcm_ltsw_tunnel_initiator_used_get(unit, tnl_init_info->tunnel_id);
        if ((rv == SHR_E_NONE) &&
            (!(tnl_init_info->flags & BCM_TUNNEL_REPLACE))) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }

    /* Convert tunnel specific flags to table management flags. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_tunnel_flags_to_shr(unit, tnl_init_info->flags, &shr_flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_initiator_create(unit, shr_flags, tnl_init_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the tunnel properties for the given L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface info.(ONLY ifindex used to identify interface).
 * \param [in/out] tunnel The tunnel header information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_initiator_get(int unit, bcm_l3_intf_t *l3_intf,
                          bcm_tunnel_initiator_t *tnl_init_info)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(tnl_init_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_initiator_get(unit, l3_intf, tnl_init_info->tunnel_id,
                                        tnl_init_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all tunnel initiator entries.
 *
 * \param [in] unit Unit number.
 * \param [in] cb User callback function, called once per entry.
 * \param [in] User supplied cookie used in parameter in callback function.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_initiator_traverse(int unit,
                               bcm_tunnel_initiator_traverse_cb cb,
                               void *user_data)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_initiator_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the tunnel property for the given L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface info. (ONLY ifindex used to identify interface).
 * \param [in] The tunnel header information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_initiator_set(int unit, bcm_l3_intf_t *intf,
                          bcm_tunnel_initiator_t *tnl_init_info)
{
    int support;
    uint32_t shr_flags;
    bcmi_ltsw_qos_map_type_t qos_type;

    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(intf, SHR_E_PARAM);
    SHR_NULL_CHECK(tnl_init_info, SHR_E_PARAM);

    /* Validate TTL. */
    if (!BCM_TTL_VALID(tnl_init_info->ttl)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate IP tunnel DSCP SEL. */
    if (tnl_init_info->dscp_sel >= bcmTunnelDscpCount ||
        tnl_init_info->dscp_sel < bcmTunnelDscpAssign) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate IP tunnel DSCP value. */
    if (tnl_init_info->dscp > 63 || tnl_init_info->dscp < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate IP tunnel ECN value. */
    if (tnl_init_info->ecn > 3) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (tnl_init_info->dscp_sel == bcmTunnelDscpMap) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_qos_map_id_resolve(unit, tnl_init_info->dscp_map,
                                          &qos_type, NULL));
        if (qos_type != bcmiQosMapTypeL3Egress) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (BCMI_LTSW_TUNNEL_OUTER_HEADER_IPV6(tnl_init_info->type)) {
        if (tnl_init_info->flow_label_sel >= bcmTunnelFlowLabelCount ||
            tnl_init_info->flow_label_sel < bcmTunnelFlowLabelAssign) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (tnl_init_info->flow_label >= (1 << 20)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_type_support_check(unit, tnl_init_info->type, NULL,
                                             &support));
    if (support == FALSE) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Convert tunnel specific flags to table management flags. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_tunnel_flags_to_shr(unit, tnl_init_info->flags, &shr_flags));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_initiator_set(unit, intf, tnl_init_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the tunnel association for the given L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface info. (ONLY ifindex used to identify interface).
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_initiator_clear(int unit, bcm_l3_intf_t *intf)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(intf, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_initiator_clear(unit, intf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Tunnel terminator structure sanity check routine.
 *
 * \param [in] unit Unit number.
 * \param [in] tnl_term_info The tunnel terminator information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_terminator_validate(int unit, bcm_tunnel_terminator_t *tnl_term_info)
{
    /* Tunnel type supported by device. */
    int support;
    /* IPv6 SIP/DIP mask length. */
    int mask_len;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tnl_term_info, SHR_E_PARAM);

    if (!(tnl_term_info->flags & BCM_TUNNEL_TERM_EM)) {
        if (BCMI_LTSW_TUNNEL_OUTER_HEADER_IPV6(tnl_term_info->type)) {
            mask_len = bcm_ip6_mask_length(tnl_term_info->dip6_mask);
            if (mask_len != _SHR_L3_IP6_MAX_NETLEN) {
               SHR_ERR_EXIT(SHR_E_PARAM);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_mask6_apply(tnl_term_info->sip6_mask,
                                          tnl_term_info->sip6));
        } else {
            if (tnl_term_info->dip_mask != BCMI_LTSW_L3_IP4_FULL_MASK) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Apply source IP subnet mask. */
            tnl_term_info->sip &= tnl_term_info->sip_mask;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_type_support_check(unit, tnl_term_info->type,
                                             &support, NULL));
    if (support == FALSE) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a tunnel terminator for DIP-SIP.
 *
 *  The following restrictions are used:
 *       - DIP must not be 0; DIP mask must be 0xFFFFFFFF.
 *
 * \param [in] unit Unit number.
 * \param [in] tnl_term_info The tunnel information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_terminator_add(int unit, bcm_tunnel_terminator_t *tnl_term_info)
{
    int rv;
    bcm_tunnel_terminator_t tmp_info;

    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(tnl_term_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_tunnel_terminator_validate(unit, tnl_term_info));

    tmp_info = *tnl_term_info;

    rv = mbcm_ltsw_tunnel_terminator_get(unit, &tmp_info);
    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_ERR_EXIT(rv);
    }

    if (rv == SHR_E_NONE) {
        /* If entry found, make sure replace flag is set. */
        if (!(tnl_term_info->flags & BCM_TUNNEL_REPLACE)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_tunnel_terminator_add(unit, tnl_term_info));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a tunnel terminator information.
 *
 * \param [in] unit Unit number.
 * \param [in/out] tnl_term_info Lookup key and extracted tunnel terminator
 *                               information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_terminator_get(int unit, bcm_tunnel_terminator_t *tnl_term_info)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(tnl_term_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_tunnel_terminator_validate(unit, tnl_term_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_terminator_get(unit, tnl_term_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a tunnel terminator.
 *
 * \param [in] unit Unit number.
 * \param [in] tnl_term_info Tunnel terminator parameters.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_terminator_delete(int unit, bcm_tunnel_terminator_t *tnl_term_info)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(tnl_term_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_tunnel_terminator_validate(unit, tnl_term_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_terminator_delete(unit, tnl_term_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all tunnel terminator entries.
 *
 * \param [in] unit Unit number.
 * \param [in] cb User callback function, called once per entry.
 * \param [in] User supplied cookie used in parameter in callback function.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_terminator_traverse(int unit,
                                bcm_tunnel_terminator_traverse_cb cb,
                                void *user_data)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_terminator_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the global tunnel property.
 *
 * \param [in] unit Unit number.
 * \param [in] tconfig Global information about the L3 tunneling config.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_config_set(int unit, bcm_tunnel_config_t *tconfig)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(tconfig, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_config_set(unit, tconfig));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the global tunnel property.
 *
 * \param [in] unit Unit number.
 * \param [in/out] tconfig Global information about the L3 tunneling config.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_tunnel_config_get(int unit, bcm_tunnel_config_t *tconfig)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(tconfig, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_config_get(unit, tconfig));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

int
bcmint_tunnel_sc_info_get(
    int unit,
    const bcmint_tunnel_sc_t **sc_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_sc_info_get(unit, sc_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the tunnel module.
 *
 * Initial tunnel-realated data structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (TUNNEL_INFO(unit)->initialized) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_tunnel_deinit(unit));
    }

    if (ltsw_tunnel_info[unit].tunnel_lock == NULL) {
        ltsw_tunnel_info[unit].tunnel_lock
            = sal_mutex_create("bcmLtswTunnelMutex");
       SHR_NULL_CHECK(ltsw_tunnel_info[unit].tunnel_lock, SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_init(unit));

    TUNNEL_INFO(unit)->initialized = 1;

exit:
    if (SHR_FUNC_ERR()) {
        ltsw_tunnel_free_resource(unit);
        if (ltsw_tunnel_info[unit].tunnel_lock != NULL) {
            sal_mutex_destroy(ltsw_tunnel_info[unit].tunnel_lock);
            ltsw_tunnel_info[unit].tunnel_lock = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the tunnel module.
 *
 * Free tunnel-realated data structure.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!TUNNEL_INFO(unit)->initialized) {
        SHR_EXIT();
    }

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_free_resource(unit));

    TUNNEL_UNLOCK(unit);

    sal_mutex_destroy(ltsw_tunnel_info[unit].tunnel_lock);
    ltsw_tunnel_info[unit].tunnel_lock = NULL;

    TUNNEL_INFO(unit)->initialized = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Specify tunnel switch control behaviors.
 *
 * \param [in] unit Unit number.
 * \param [in] control The desired configuration parameter to modify.
 * \param [in] value The value with which to set the parameter.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_control_set(
    int unit,
    bcmi_ltsw_tunnel_control_t control,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_tunnel_control_set(unit, control, value));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve tunnel switch control behaviors.
 *
 * \param [in] unit Unit number.
 * \param [in] control The desired configuration parameter to retrieve.
 * \param [out] value Pointer to where the retrieved value will be written.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_control_get(
    int unit,
    bcmi_ltsw_tunnel_control_t control,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_tunnel_control_get(unit, control, value));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a tunnel terminator for DIP-SIP.
 *
 * \param [in] unit Unit number.
 * \param [in] tnl_term_info The tunnel terminator information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_terminator_add(int unit, bcm_tunnel_terminator_t *info)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_terminator_add(unit, info));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a tunnel terminator.
 *
 * \param [in] unit Unit number.
 * \param [in] info Tunnel terminator parameters.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_terminator_delete(int unit, bcm_tunnel_terminator_t *info)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_terminator_delete(unit, info));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update a tunnel terminator for DIP-SIP.
 *
 * \param [in] unit Unit number.
 * \param [in] tnl_term_info Tunnel terminator parameters.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_terminator_update(int unit, bcm_tunnel_terminator_t *info)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    info->flags |= BCM_TUNNEL_REPLACE;

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_terminator_add(unit, info));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a tunnel terminator information.
 *
 * \param [in] unit Unit number.
 * \param [in/out] info Lookup key and extracted tunnel terminator information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_terminator_get(int unit, bcm_tunnel_terminator_t *info)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_terminator_get(unit, info));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all tunnel terminator entries.
 *
 * \param [in] unit Unit number.
 * \param [in] cb User callback function, called once per entry.
 * \param [in] User supplied cookie used in parameter in callback function.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_terminator_traverse(int unit,
                                    bcm_tunnel_terminator_traverse_cb cb,
                                    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_terminator_traverse(unit, cb, user_data));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counters entries to the given tunnel.
 *
 * \param [in] unit Unit number.
 * \param [in] terminator Tunnel terminator.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_tunnel_terminator_stat_attach(
    int unit,
    bcm_tunnel_terminator_t *terminator,
    uint32 stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci, pre_ci;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_TNL_TERM_FLEXCTR)) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_UNAVAIL);
    }

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(terminator, SHR_E_PARAM);

    if(terminator->flags & BCM_TUNNEL_TERM_EM) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&pre_ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id, &ci));

    if (ci.source != bcmFlexctrSourceIpTunnelTerminator) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    pre_ci.direction = ci.direction;

    TUNNEL_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_terminator_flexctr_info_get(unit,
                                                      terminator,
                                                      &pre_ci));

    if (pre_ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_tunnel_terminator_stat_attach(unit, terminator, &ci));

    if (SHR_FUNC_ERR()) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                               stat_counter_id));
    }

exit:
    if (locked) {
        TUNNEL_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counters entries to the given tunnel.
 *
 * \param [in] unit Unit number.
 * \param [in] terminator Tunnel terminator.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_tunnel_terminator_stat_detach(
    int unit,
    bcm_tunnel_terminator_t *terminator)
{

    bcmi_ltsw_flexctr_counter_info_t ci;
    bool locked = false;
    uint32_t ctr_id;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_TNL_TERM_FLEXCTR)) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_UNAVAIL);
    }

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(terminator, SHR_E_PARAM);

    if(terminator->flags & BCM_TUNNEL_TERM_EM) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    TUNNEL_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_terminator_flexctr_info_get(unit,
                                                      terminator,
                                                      &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &ctr_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_terminator_stat_detach(unit, terminator));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                           ctr_id));

exit:
    if (locked) {
        TUNNEL_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get stat counter id associated with given tunnel.
 *
 * \param [in] unit Unit number.
 * \param [in] terminator Tunnel terminator.
 * \param [out] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_tunnel_terminator_stat_id_get(
    int unit,
    bcm_tunnel_terminator_t *terminator,
    uint32 *stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci = {0};
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_TNL_TERM_FLEXCTR)) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_UNAVAIL);
    }

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(terminator, SHR_E_PARAM);

    if(terminator->flags & BCM_TUNNEL_TERM_EM) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    TUNNEL_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_terminator_flexctr_info_get(unit,
                                                      terminator,
                                                      &ci));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, stat_counter_id));

exit:
    if (locked) {
        TUNNEL_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set flex counter object value for the given tunnel.
 *
 * \param [in] unit Unit number.
 * \param [in] terminator Tunnel terminator.
 * \param [in] value The flex counter object value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_tunnel_terminator_t *terminator,
    uint32_t value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_TNL_TERM_FLEXCTR)) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_UNAVAIL);
    }

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(terminator, SHR_E_PARAM);

    if(terminator->flags & BCM_TUNNEL_TERM_EM) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    TUNNEL_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_terminator_flexctr_object_set(unit,
                                                        terminator,
                                                        value));

exit:
    if (locked) {
        TUNNEL_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex counter object value for the given tunnel.
 *
 * \param [in] unit Unit number.
 * \param [in] terminator Tunnel terminator.
 * \param [out] value The flex counter object value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_ltsw_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_tunnel_terminator_t *terminator,
    uint32_t *value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_TNL_TERM_FLEXCTR)) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_UNAVAIL);
    }

    TUNNEL_INIT_CHECK(unit);

    SHR_NULL_CHECK(terminator, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    if(terminator->flags & BCM_TUNNEL_TERM_EM) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    TUNNEL_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_terminator_flexctr_object_get(unit,
                                                        terminator,
                                                        value));

exit:
    if (locked) {
        TUNNEL_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an tunnel initiator with given properties.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface info. (Should be NULL).
 * \param [in] tunnel The tunnel header information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_initiator_create(int unit, bcm_l3_intf_t *intf,
                                 bcm_tunnel_initiator_t *tunnel)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_initiator_create(unit, intf, tunnel));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete specific IP tunnel initiator.
 *
 * \param [in] unit Unit number.
 * \param [in] tunnel_id IP tunnel ID.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_initiator_destroy(int unit, bcm_gport_t tunnel_id)
{
    int lock = 0;

    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);
    lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_tunnel_initiator_destroy(unit, tunnel_id));

exit:
    if (lock == 1) {
        TUNNEL_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the tunnel properties for the given L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface info.(ONLY ifindex used to identify interface).
 * \param [in/out] tunnel The tunnel header information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_initiator_get(int unit, bcm_l3_intf_t *l3_intf,
                              bcm_tunnel_initiator_t *tunnel)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_initiator_get(unit, l3_intf, tunnel));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all tunnel initiator entries.
 *
 * \param [in] unit Unit number.
 * \param [in] cb User callback function, called once per entry.
 * \param [in] User supplied cookie used in parameter in callback function.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_initiator_traverse(int unit,
                                   bcm_tunnel_initiator_traverse_cb cb,
                                   void *user_data)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_initiator_traverse(unit, cb, user_data));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the tunnel property for the given L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface info. (ONLY ifindex used to identify interface).
 * \param [in] The tunnel header information.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_initiator_set(int unit, bcm_l3_intf_t *intf,
                              bcm_tunnel_initiator_t *tunnel)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_initiator_set(unit, intf, tunnel));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete the tunnel association for the given L3 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface info. (ONLY ifindex used to identify interface).
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_initiator_clear(int unit, bcm_l3_intf_t *intf)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_initiator_clear(unit, intf));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the global tunnel property.
 *
 * \param [in] unit Unit number.
 * \param [in] tconfig Global information about the L3 tunneling config.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_config_set(int unit, bcm_tunnel_config_t *tconfig)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_config_set(unit, tconfig));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the global tunnel property.
 *
 * \param [in] unit Unit number.
 * \param [in/out] tconfig Global information about the L3 tunneling config.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_tunnel_config_get(int unit, bcm_tunnel_config_t *tconfig)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (ltsw_tunnel_config_get(unit, tconfig));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Initialize egress sequence number profile.
*
* \param [in] unit Unit number.
*
* \retval SHR_E_NONE Success.
* \retval !SHR_E_NONE Failure.
*/
int
bcmi_ltsw_tunnel_encap_seq_profile_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (TUNNEL_ENCAP_SEQ_INFO(unit)->tnl_encap_seq_prf_inited == 1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_tunnel_encap_seq_profile_deinit(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_encap_seq_profile_init(unit));

    if (TUNNEL_ENCAP_SEQ_INFO(unit)->encap_seq_lock == NULL) {
        TUNNEL_ENCAP_SEQ_INFO(unit)->encap_seq_lock
            = sal_mutex_create("bcmLtswEncapSeqMutex");
       SHR_NULL_CHECK(TUNNEL_ENCAP_SEQ_INFO(unit)->encap_seq_lock, SHR_E_MEMORY);
    }

    TUNNEL_ENCAP_SEQ_INFO(unit)->tnl_encap_seq_prf_inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (TUNNEL_ENCAP_SEQ_INFO(unit)->encap_seq_lock != NULL) {
            sal_mutex_destroy(TUNNEL_ENCAP_SEQ_INFO(unit)->encap_seq_lock);
            TUNNEL_ENCAP_SEQ_INFO(unit)->encap_seq_lock = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
* \brief De-Initialize egress sequence number profile.
*
* \param [in] unit Unit number.
*
* \retval SHR_E_NONE Success.
* \retval !SHR_E_NONE Failure.
*/
int
bcmi_ltsw_tunnel_encap_seq_profile_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (TUNNEL_ENCAP_SEQ_INFO(unit)->tnl_encap_seq_prf_inited == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_encap_seq_profile_deinit(unit));

    TUNNEL_ENCAP_SEQ_INFO(unit)->tnl_encap_seq_prf_inited = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate egress encapsulation sequence number entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] flags Operation flags.
 * \param [in] type Sequence Number type.
 * \param [out] index Sequence number index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_encap_seq_num_alloc(int unit, uint32_t flags,
                                     bcmi_ltsw_tunnel_encap_seq_num_type_t type,
                                     int *index)
{
    bool lock = 0;

    SHR_FUNC_ENTER(unit);

    TUNNEL_SEQ_PRF_INIT_CHECK(unit);

    SHR_NULL_CHECK(index, SHR_E_PARAM);

    TUNNEL_SEQ_NUM_LOCK(unit);
    lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_encap_seq_num_alloc(unit, flags, type, index));

exit:
    if (lock == 1) {
        TUNNEL_SEQ_NUM_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Free egress encapsulation sequence number entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Sequence Number type.
 * \param [in] index Sequence number index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_encap_seq_num_free(int unit,
                                    bcmi_ltsw_tunnel_encap_seq_num_type_t type,
                                    int index)
{
    bool lock = 1;

    SHR_FUNC_ENTER(unit);

    TUNNEL_SEQ_PRF_INIT_CHECK(unit);

    TUNNEL_SEQ_NUM_LOCK(unit);
    lock = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_encap_seq_num_free(unit, type, index));

exit:
    if (lock == 1) {
        TUNNEL_SEQ_NUM_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress encapsulation sequence number.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Index of sequence number table.
 * \param [in] pipe Pipe number.
 * \param [out] number Sequence number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_encap_seq_num_get(int unit, int idx, int pipe, uint32_t *number)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_SEQ_PRF_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_encap_seq_num_get(unit, idx, pipe, number));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set egress encapsulation sequence number.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Index of sequence number table.
 * \param [in] pipe Pipe number.
 * \param [in] number Sequence number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_encap_seq_num_set(int unit, int idx, int pipe, uint32_t number)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_SEQ_PRF_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_encap_seq_num_set(unit, idx, pipe, number));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add egress encapsulation sequence number profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Sequence number profile.
 * \param [out] index Index of sequence number profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_encap_seq_profile_add(int unit,
                                       bcmi_ltsw_tunnel_encap_seq_profile_t *entry,
                                       int *index)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_SEQ_PRF_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_encap_seq_profile_add(unit, entry, index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress encapsulation sequence number profile.
 *
 * \param [in] unit Unit number.
 * \param [out] entry Sequence number profile.
 * \param [in] index Index of sequence number profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_encap_seq_profile_get(int unit,
                                       bcmi_ltsw_tunnel_encap_seq_profile_t *entry,
                                       int index)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_SEQ_PRF_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_encap_seq_profile_get(unit, entry, index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete egress encapsulation sequence number profile.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of sequence number profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_encap_seq_profile_delete(int unit,
                                          int index)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_SEQ_PRF_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_encap_seq_profile_delete(unit, index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover egress encapsulation sequence number profile.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of sequence number profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_encap_seq_profile_recover(int unit,
                                           int index)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_SEQ_PRF_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_encap_seq_profile_recover(unit, index));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add tunnel decap port profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Decap port profile.
 * \param [out] index Index of sequence number profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_decap_port_profile_add(
    int unit,
    bcmi_ltsw_tunnel_decap_port_profile_t *entry,
    int *index)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_tunnel_decap_port_profile_add(unit, entry, index));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get tunnel decap port profile.
 *
 * \param [in] unit Unit number.
 * \param [out] entry Decap port profile.
 * \param [in] index Index of sequence number profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_decap_port_profile_get(
    int unit,
    bcmi_ltsw_tunnel_decap_port_profile_t *entry,
    int index)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_tunnel_decap_port_profile_get(unit, entry, index));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete tunnel decap port profile.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of sequence number profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_decap_port_profile_delete(
    int unit,
    int index)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_tunnel_decap_port_profile_delete(unit, index));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover tunnel decap port profile.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of sequence number profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_tunnel_decap_port_profile_recover(
    int unit,
    int index)
{
    SHR_FUNC_ENTER(unit);

    TUNNEL_INIT_CHECK(unit);

    TUNNEL_LOCK(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_tunnel_decap_port_profile_recover(unit, index));

    TUNNEL_UNLOCK(unit);

exit:
    SHR_FUNC_EXIT();
}

