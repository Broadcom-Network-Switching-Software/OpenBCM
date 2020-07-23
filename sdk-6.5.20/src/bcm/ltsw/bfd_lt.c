/*! \file bfd_lt.c
 *
 * Common functions for both XGS and XFS platforms for
 * accessing OAM_BFD_* LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_BFD)
/* API related header files */
#include <bcm/bfd.h>
#include <bcm/types.h>

/* Internal common header files */
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/cosq.h>

/* Internal BFD header files */
#include <bcm_int/ltsw/bfd_int.h>

/* SDKLT specific header files */
#include <bcmltd/chip/bcmltd_str.h>

/* Shared library header files */
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>

#define BSL_LOG_MODULE BSL_LS_BCM_BFD

/* VLAN Tag - 802.1Q */
typedef struct bfd_vlan_tag_s {
    uint16      tpid;    /* 16: Tag Protocol Identifier */
    struct {
        uint8   prio;    /*  3: Priority Code Point */
        uint8   cfi;     /*  1: Canonical Format Indicator */
        uint16  vid;     /* 12: Vlan Identifier */
    } tci;               /* Tag Control Identifier */
} bfd_vlan_tag_t;

/* L2 Header */
typedef struct bfd_l2_header_t {
    bcm_mac_t       dst_mac;      /* 48: Destination MAC */
    bcm_mac_t       src_mac;      /* 48: Source MAC */
    bfd_vlan_tag_t  outer_vlan_tag;    /* VLAN Tag */
    bfd_vlan_tag_t  inner_vlan_tag;    /* inner VLAN Tag */
    uint16          etype;        /* 16: Ether Type */
} bfd_l2_header_t;

#define MICRO_BFD_DST_MAC(a_)   \
    (((a_)[0] == 0x01) && ((a_)[1] == 0x00) && ((a_)[2] == 0x5E) && \
    ((a_)[3] == 0x90) && ((a_)[4] == 0x00) && ((a_)[5] == 0x01))

#define BFD_MPLS_MAX_LABELS    3   /* Max MPLS labels */

#define LTSW_BFD_MPLS_ROUTER_ALERT_LABEL      1
#define LTSW_BFD_MPLS_GAL_LABEL               13

/******************************************************************************
 * Private Functions
 */


static
const char *bcm_ltsw_bfd_vlan_tag_type_symbol_get(
        uint8_t num_vlans)
{
    switch(num_vlans) {
        case 0:
            return UNTAGGEDs;
        case 1:
            return SINGLE_TAGGEDs;
        case 2:
            return DOUBLE_TAGGEDs;
        default:
            break;
    }
    return("");
}

/******************************************************************************
 * Public Functions
 */

int
bcm_ltsw_bfd_control_entry_set(int unit,
        bool update,
        bcm_int_bfd_info_t *entry)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_CONTROLs, &entry_hdl));

    /* BFD will be enabled by default */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, BFDs, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               LOCAL_STATE_DOWN_EVENT_ON_ENDPOINT_CREATIONs,
                               entry->state_down_event_on_ep_creation));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, CONTROL_PLANE_INDEPENDENCEs,
                               entry->cpi));

    if (!update) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MAX_ENDPOINTSs,
                                   entry->max_endpoints));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MAX_AUTH_SHA1_KEYSs,
                                   entry->max_auth_sha1_keys));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MAX_AUTH_SIMPLE_PASSWORD_KEYSs,
                                   entry->max_auth_simple_password_keys));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MAX_PKT_SIZEs, entry->max_pkt_size));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, INTERNAL_LOCAL_DISCRIMINATORs,
                                   entry->use_ep_id_as_disc));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, STATIC_REMOTE_DISCRIMINATORs,
                                   entry->static_remote_disc));
    }

    if (update == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_control_entry_get(int unit,
        bcm_int_bfd_info_t *entry)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    uint64_t val = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_CONTROLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATEs, &val));
    if (val != 0) {
        /* Non zero indicates some sort of failure */
        entry->init = false;
    } else {
        /* Zero indicates success */
        entry->init = true;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl,
                        LOCAL_STATE_DOWN_EVENT_ON_ENDPOINT_CREATIONs, &val));
    entry->state_down_event_on_ep_creation = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, CONTROL_PLANE_INDEPENDENCEs, &val));
    entry->cpi = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TXs, &val));
    entry->tx = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MAX_ENDPOINTS_OPERs, &val));
    entry->max_endpoints = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MAX_AUTH_SHA1_KEYS_OPERs, &val));
    entry->max_auth_sha1_keys = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MAX_AUTH_SIMPLE_PASSWORD_KEYS_OPERs,
                               &val));
    entry->max_auth_simple_password_keys = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MAX_PKT_SIZE_OPERs, &val));
    entry->max_pkt_size = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, INTERNAL_LOCAL_DISCRIMINATOR_OPERs,
                               &val));
    entry->use_ep_id_as_disc = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, STATIC_REMOTE_DISCRIMINATOR_OPERs,
                               &val));
    entry->static_remote_disc = val;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/* Endpoint create internal functions */

static int
bcm_ltsw_bfd_tx_raw_l2_header_get(int unit,
        bcmlt_entry_handle_t *entry_hdl,
        bcm_bfd_endpoint_info_t *endpoint_info,
        bcm_port_t port)
{
    bfd_l2_header_t l2;
    bcm_pbmp_t pbmp, ubmp;
    uint64_t val = 0;
    const char* symbol;
    int num_vlan = 0;
    bcm_mac_t mac_zero = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_mac_t mac_resv = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x03};
    bcm_vlan_control_vlan_t vc;
    uint16_t tpid         = 0;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);

    sal_memset(&l2, 0, sizeof(l2));

    /* MAC Validation */
    if (!(sal_memcmp(endpoint_info->dst_mac, mac_zero, sizeof(bcm_mac_t))) ||
            !(sal_memcmp(endpoint_info->dst_mac, mac_resv, sizeof(bcm_mac_t)))) {
        return SHR_E_PARAM;
    }
    if (!(sal_memcmp(endpoint_info->src_mac, mac_zero, sizeof(bcm_mac_t))) ||
            !(sal_memcmp(endpoint_info->src_mac, mac_resv, sizeof(bcm_mac_t)))) {
        return SHR_E_PARAM;
    }

    if (port) {
        /* Get TPID */
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_tpid_get(unit, port, &tpid));
    } else {
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_USE_PKT_VLAN_ID) {

            SHR_IF_ERR_EXIT
                (bcm_ltsw_vlan_control_vlan_get(unit, endpoint_info->pkt_vlan_id, &vc));

            tpid = vc.outer_tpid;
        }
    }
    bcmi_ltsw_util_mac_to_uint64(&val, endpoint_info->dst_mac);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, DST_MACs, val));

    bcmi_ltsw_util_mac_to_uint64(&val, endpoint_info->src_mac);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, SRC_MACs, val));

    /* VLAN Tag */
    l2.outer_vlan_tag.tpid     = tpid;
    l2.outer_vlan_tag.tci.vid  = endpoint_info->pkt_vlan_id;
    l2.outer_vlan_tag.tci.prio = endpoint_info->vlan_pri;
    l2.outer_vlan_tag.tci.cfi  = 0;

    if (endpoint_info->pkt_vlan_id)
    {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_vlan_port_get(unit,
                                    l2.outer_vlan_tag.tci.vid,
                                    &pbmp,
                                    &ubmp));
    }

    if (port && BCM_PBMP_MEMBER(ubmp, port)) {
        /* Set to 0 to indicate untagged */
        l2.outer_vlan_tag.tpid = 0;
        num_vlan = 0;
    }

    if (port && endpoint_info->pkt_inner_vlan_id != 0) {
        if (SHR_FAILURE(bcm_ltsw_port_inner_tpid_get(unit, port,
                        &tpid))) {
            return SHR_E_INTERNAL;
        }
        l2.inner_vlan_tag.tpid     = tpid;
        l2.inner_vlan_tag.tci.prio = endpoint_info->inner_vlan_pri;
        l2.inner_vlan_tag.tci.cfi  = 0;
        l2.inner_vlan_tag.tci.vid  = endpoint_info->pkt_inner_vlan_id;
    } else {
        /* Set to 0 to indicate untagged */
        l2.inner_vlan_tag.tpid = 0;
        num_vlan = 2;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, VLAN_IDs,
                               l2.outer_vlan_tag.tci.vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, VLAN_PRIs,
                               l2.outer_vlan_tag.tci.prio));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, TPIDs,
                               l2.outer_vlan_tag.tpid ));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, INNER_VLAN_IDs,
                               l2.inner_vlan_tag.tci.vid ));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, INNER_VLAN_PRIs,
                               l2.inner_vlan_tag.tci.prio));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, INNER_TPIDs,
                               l2.inner_vlan_tag.tpid));

    symbol = bcm_ltsw_bfd_vlan_tag_type_symbol_get(num_vlan);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(*entry_hdl, TAG_TYPEs, symbol));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_bfd_l2_header_get(int unit,
        bcmlt_entry_handle_t *entry_hdl,
        bcm_bfd_endpoint_info_t *endpoint_info,
        bcm_port_t port)
{
    uint16 tpid;
    bcm_l3_egress_t l3_egress;
    bcm_l3_intf_t l3_intf;
    bfd_l2_header_t l2;
    bcm_pbmp_t pbmp, ubmp;
    uint64_t val = 0;
    const char* symbol;
    int num_vlan = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&l2, 0, sizeof(l2));

    /* Get L3 interfaces */
    bcm_l3_egress_t_init(&l3_egress);
    bcm_l3_intf_t_init(&l3_intf);
    SHR_IF_ERR_EXIT
        (bcm_ltsw_l3_egress_get(unit, endpoint_info->egress_if, &l3_egress));

    l3_intf.l3a_intf_id = l3_egress.intf;
    SHR_IF_ERR_EXIT
        (bcm_ltsw_l3_intf_get(unit, &l3_intf));

    /* Get TPID */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_port_tpid_get(unit, port, &tpid));

    bcmi_ltsw_util_mac_to_uint64(&val, l3_egress.mac_addr);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, DST_MACs, val));

    bcmi_ltsw_util_mac_to_uint64(&val, l3_intf.l3a_mac_addr);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, SRC_MACs, val));

    /*
     * VLAN Tag
     */
    l2.outer_vlan_tag.tpid     = tpid;
    l2.outer_vlan_tag.tci.vid  = l3_intf.l3a_vid;
    l2.outer_vlan_tag.tci.prio = endpoint_info->vlan_pri;
    l2.outer_vlan_tag.tci.cfi  = 0;
    num_vlan = 1;

    SHR_IF_ERR_EXIT(bcm_ltsw_vlan_port_get(unit,
                l2.outer_vlan_tag.tci.vid,
                &pbmp,
                &ubmp));

    if ((endpoint_info->flags & BCM_BFD_ENDPOINT_USE_PKT_VLAN_ID) &&
            (endpoint_info->type == bcmBFDTunnelTypeUdp)) {
        /*
         * Check the outer vlan id status, being set in BFD Tx pkt
         * Whether it's set by pkt_vlan_id or default l3 interface
         */
        l2.outer_vlan_tag.tci.vid  = endpoint_info->pkt_vlan_id;
    } else if (BCM_PBMP_MEMBER(ubmp, port)) {
        /* Set to 0 to indicate untagged */
        l2.outer_vlan_tag.tpid = 0;
        num_vlan = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, VLAN_IDs,
                               l2.outer_vlan_tag.tci.vid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, VLAN_PRIs,
                               l2.outer_vlan_tag.tci.prio));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, TPIDs,
                               l2.outer_vlan_tag.tpid ));

    if (l3_intf.l3a_inner_vlan != 0) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_inner_tpid_get(unit, port, &tpid));

        l2.inner_vlan_tag.tpid     = tpid;
        l2.inner_vlan_tag.tci.prio = endpoint_info->inner_vlan_pri;
        l2.inner_vlan_tag.tci.cfi  = 0;
        l2.inner_vlan_tag.tci.vid  = l3_intf.l3a_inner_vlan;
        num_vlan = 2;

    } else {
        l2.inner_vlan_tag.tpid = 0;  /* Set to 0 to indicate untagged */
        l2.inner_vlan_tag.tci.prio = 0;
        l2.inner_vlan_tag.tci.vid = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, INNER_VLAN_IDs,
                               l2.inner_vlan_tag.tci.vid ));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, INNER_VLAN_PRIs,
                               l2.inner_vlan_tag.tci.prio));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, INNER_TPIDs,
                               l2.inner_vlan_tag.tpid));


    symbol = bcm_ltsw_bfd_vlan_tag_type_symbol_get(num_vlan);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(*entry_hdl, TAG_TYPEs, symbol));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_trunk_active_member_get(int unit, bcm_trunk_t tid,
        bcm_trunk_info_t *trunk_info,
        int member_max,
        bcm_trunk_member_t *active_member_array,
        int *active_member_count)
{
    int                  member_count;
    bcm_trunk_member_t   member_array[BCM_TRUNK_MAX_PORTCNT];
    int                  filled_count = 0;

    SHR_FUNC_ENTER(unit);

    if ((member_max > 0) && (NULL == active_member_array)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((member_max > 0) && (NULL == active_member_count)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_trunk_get(unit, tid, NULL, BCM_TRUNK_MAX_PORTCNT,
                           member_array, &member_count));

    if (0 < member_count) {
        int i;
        if (active_member_array != NULL) {
            sal_memset(active_member_array, 0,
                    sizeof(bcm_trunk_member_t) * member_max);
        }

        *active_member_count = 0;

        /* Get only the active member ports */
        for (i = 0; i < member_count; i++) {
            if (!(member_array[i].flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE)) {
                if ( (active_member_array != NULL) && (member_max > 0) ) {
                    sal_memcpy(&active_member_array[*active_member_count],
                            &member_array[i], sizeof(member_array[i]));
                    filled_count++;
                }
                (*active_member_count)++;

                if ( (member_max > 0) && (filled_count == member_max) ) {
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static int
bcm_ltsw_bfd_endpoint_gport_resolve(int unit,
        uint8 is_tx,
        bcm_bfd_endpoint_info_t *endpoint_info,
        bcm_module_t *module_id,
        bcm_port_t   *port_id,
        bcm_trunk_t *trunk_id,
        int *local_id,
        uint8 *is_trunk_bfd,
        uint8 *is_micro_bfd)
{
    bcm_trunk_member_t *member_array = NULL; /* Trunk member array. */
    int                member_count = 0;     /* Trunk Member count. */
    int                rv = SHR_E_NONE;      /* Return status.      */
    int                idx = 0 , link = 0;
    bcm_l3_egress_t    l3_egress;
    bcm_trunk_member_t   member_array_max[BCM_TRUNK_MAX_PORTCNT];

    SHR_FUNC_ENTER(unit);

    sal_memset(&l3_egress, 0, sizeof(l3_egress));

    if(!is_tx) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_port_gport_resolve(unit, endpoint_info->gport, module_id,
                                          port_id, trunk_id, local_id));
    } else {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_port_gport_resolve(unit, endpoint_info->tx_gport, module_id,
                                          port_id, trunk_id, local_id));
    }

    if (BCM_GPORT_IS_TRUNK(endpoint_info->gport)) {
        if (BCM_TRUNK_INVALID == *trunk_id)  {
            /* Has to be a valid Trunk. */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (BCM_GPORT_IS_TRUNK(endpoint_info->gport)) {
            if (BCM_TRUNK_INVALID == *trunk_id)  {
                /* Has to be a valid Trunk. */
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Get count of ports in this trunk. */
            SHR_IF_ERR_EXIT
                (bcm_ltsw_trunk_get(unit, *trunk_id, NULL, BCM_TRUNK_MAX_PORTCNT,
                                    member_array_max, &member_count));
            if (0 == member_count) {
                /* No members have been added to the trunk group yet */
                LOG_ERROR(BSL_LS_BCM_BFD,
                        (BSL_META_U(unit,
                                    "\n No members have been added to trunk yet \n")));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            member_array = sal_alloc(sizeof(bcm_trunk_member_t) * member_count, "Trunk info");
            if (NULL == member_array) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            /* Get Trunk Info for the Trunk ID. */
            rv = bcm_ltsw_trunk_active_member_get(unit, *trunk_id, NULL, member_count,
                    member_array, &member_count);
            if (SHR_FAILURE(rv)) {
                sal_free(member_array);
                SHR_ERR_EXIT(rv);
            }
            for(idx = 0;idx < member_count; idx++) {
                /* Get the Modid and Port value using Trunk Index value. */
                rv = bcmi_ltsw_port_gport_resolve
                    (unit, member_array[idx].gport,
                     module_id, port_id, trunk_id, local_id);
                if (SHR_FAILURE(rv)) {
                    sal_free(member_array);
                }

                rv = bcm_ltsw_port_link_status_get(unit, *port_id, &link);
                if (SHR_FAILURE(rv)) {
                    sal_free(member_array);
                    SHR_ERR_EXIT(rv);
                }
                if (link == BCM_PORT_LINK_STATUS_UP) {
                    break;
                }
            }
            /*Only when there are active members in trunk then
              below condition should be set */
            if((!is_tx) && member_count) {
                *is_trunk_bfd = 1;
            }
            if(!(*is_trunk_bfd)) {
                LOG_ERROR(BSL_LS_BCM_BFD,
                        (BSL_META_U(unit,
                                    "\n Could not get valid port for tx from trunk \n")));
                sal_free(member_array);
                SHR_ERR_EXIT(SHR_E_PORT);
            }
            sal_free(member_array);
        } else if (!is_tx) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_l3_egress_get(unit, endpoint_info->egress_if,
                                        &l3_egress));
            /* Use the Modid, Port value and determine if the port
             * belongs to a Trunk. */
            if (BCM_TRUNK_INVALID == *trunk_id) {

            }
            rv = bcm_ltsw_trunk_find(unit, *module_id, *port_id, trunk_id);
        }

        if ((SHR_SUCCESS(rv)) || *trunk_id != BCM_TRUNK_INVALID) {
            /* If Dest MAC is dedicated multicast MAC address 01-00-5E-90-00-01
             * and treat it as micro BFD */
            if ((!is_tx) && MICRO_BFD_DST_MAC(l3_egress.mac_addr)){
                *is_micro_bfd = 1;
            }
        } else if(MICRO_BFD_DST_MAC(l3_egress.mac_addr)){
            LOG_ERROR(BSL_LS_BCM_BFD,
                    (BSL_META_U(unit,
                                "\n Dedicated MAC 01-00-5E-90-00-01 must not be used for regular BFD session\n")));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_bfd_tx_port_get(int unit,
        bcmlt_entry_handle_t *entry_hdl,
        bcm_bfd_endpoint_info_t *ep_info,
        bcm_port_t *tx_port,
        bool is_tx)
{
    bcm_module_t module_id;
    bcm_port_t port_id;
    bcm_trunk_t trunk_id = BCM_TRUNK_INVALID;
    int local_id, is_local;
    bcm_l3_egress_t l3_egress;
    uint8 is_trunk_bfd = 0, is_micro_bfd = 0;
    const char *symbol;
    int num_ucq, num_mcq;

    SHR_FUNC_ENTER(unit);

    /* Resolve TX module and port */
    sal_memset(&l3_egress, 0, sizeof(l3_egress));
    if ((BCM_GPORT_INVALID != ep_info->gport) || (BCM_GPORT_INVALID != ep_info->tx_gport)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_endpoint_gport_resolve(unit, is_tx, ep_info, &module_id,
                                                 &port_id, &trunk_id, &local_id, &is_trunk_bfd, &is_micro_bfd));
    } else if (SHR_SUCCESS
            (bcm_ltsw_l3_egress_get(unit, ep_info->egress_if,
                                    &l3_egress))) {
        module_id = l3_egress.module;
        port_id = l3_egress.port;
    } else {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    /* Get local port used for TX BFD packet */
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_modid_is_local(unit, module_id, &is_local));
    if (is_local) {    /* Ethernet port */
        *tx_port = port_id;
    } else {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    /* Get local port used for TX BFD packet */
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_modid_is_local(unit, module_id, &is_local));
    if (is_local) {    /* Ethernet port */
        *tx_port = port_id;
    } else {
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    /*
     * Check that configuration contains valid cos queue for the tx port
     */
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_cosq_num_queue_get(unit, &num_ucq, &num_mcq));

    if (ep_info->int_pri >= num_ucq) {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                        "BFD(unit %d) : Invalid int_pri value (%d).\n"),
                    unit, ep_info->int_pri));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (is_micro_bfd) {
        symbol = MICROs;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(*entry_hdl, TYPEs, symbol));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*entry_hdl, PORT_IDs,
                               *tx_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*entry_hdl, COSs,
                               ep_info->int_pri));
exit:
    SHR_FUNC_EXIT();
}




static int
bcm_ltsw_bfd_mpls_label_get(uint32 label, uint8 exp, uint8 s, uint8 ttl,
                             shr_util_pack_mpls_label_t *mpls)
{
    sal_memset(mpls, 0, sizeof(*mpls));

    mpls->label = label;
    mpls->exp   = exp;
    mpls->s     = s;
    mpls->ttl   = ttl;

    return BCM_E_NONE;
}

static int
bcm_ltsw_bfd_mpls_gal_label_get(shr_util_pack_mpls_label_t *mpls,
                                 bcm_bfd_endpoint_info_t *endpoint_info)
{
    uint8 exp = 0;
    uint8 ttl = 0;
    uint8 user_specified_exp = 0;
    uint8 user_specified_ttl = 0;
    uint32 user_gal_label = 0;
    uint32 user_specified_label = 0;
    user_gal_label = endpoint_info->gal_label;

    /* Decode 32bit gal label,  parse the exp, ttl value and ignore gal label field */
    BCM_MPLS_HEADER_EXP_GET(user_gal_label, user_specified_exp);
    BCM_MPLS_HEADER_TTL_GET(user_gal_label,user_specified_ttl);
    BCM_MPLS_HEADER_LABEL_GET(user_gal_label, user_specified_label);

    if (user_gal_label &&
        (user_specified_label != LTSW_BFD_MPLS_GAL_LABEL)) {
        return BCM_E_PARAM;

    }
    exp = user_specified_exp?user_specified_exp:0;
    ttl = user_specified_ttl?user_specified_ttl:1;

    /* Always gal_exp holds 3 bit value, range 0 to 7 */
    return bcm_ltsw_bfd_mpls_label_get(LTSW_BFD_MPLS_GAL_LABEL,
                                        exp, 1, ttl, mpls);
}

static int
bcm_ltsw_bfd_mpls_router_alert_label_get(shr_util_pack_mpls_label_t *mpls)
{
    return bcm_ltsw_bfd_mpls_label_get(LTSW_BFD_MPLS_ROUTER_ALERT_LABEL,
                                        0, 0, 1, mpls);
}

static int
bcm_ltsw_bfd_mpls_labels_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info,
                              int max_count, uint64_t *labels,
                              int *mpls_count)
{
    int count = 0;
    bcm_l3_egress_t l3_egress;
    bcm_mpls_port_t mpls_port;
    int i;
    shr_util_pack_mpls_label_t mpls[BFD_MPLS_MAX_LABELS];
    bcm_mpls_tunnel_encap_t tunnel_encap;
    bcm_l3_intf_t l3_intf;

    SHR_FUNC_ENTER(unit);

    /* Get MPLS Tunnel Label(s) */
    bcm_l3_egress_t_init(&l3_egress);
    if (SHR_FAILURE(bcm_ltsw_l3_egress_get(unit, endpoint_info->egress_if,
                                          &l3_egress))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    l3_intf.l3a_intf_id = l3_egress.intf;
    SHR_IF_ERR_EXIT
        (bcm_ltsw_l3_intf_get(unit, &l3_intf));

    /* Look for a tunnel associated with this interface */
    tunnel_encap.tunnel_id = l3_intf.l3a_tunnel_idx;

    if (SHR_SUCCESS
        (bcm_ltsw_mpls_tunnel_encap_get(unit, &tunnel_encap))) {

        for (i = 0; i < tunnel_encap.num_labels; i++) {
             if (tunnel_encap.label_array[i].label) {
                 bcm_ltsw_bfd_mpls_label_get(tunnel_encap.label_array[i].label,
                                              tunnel_encap.label_array[i].exp,
                                              0,
                                              tunnel_encap.label_array[i].ttl,
                                              &mpls[count++]);
             }
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_BFD,
                  (BSL_META_U(unit,
                              "MPLS tunnel initiator information get failed\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* MPLS Router Alert */
    if (endpoint_info->type == bcmBFDTunnelTypePweRouterAlert) {
        bcm_ltsw_bfd_mpls_router_alert_label_get(&mpls[count++]);
    }

    /* Get L2 VPN PW VC label */
    if (BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport)) {
        /* Get mpls port and label info */
        bcm_mpls_port_t_init(&mpls_port);
        mpls_port.mpls_port_id = endpoint_info->gport;
        if (SHR_FAILURE
            (bcm_ltsw_mpls_port_get(unit, endpoint_info->vpn,
                                   &mpls_port))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            if (endpoint_info->type == bcmBFDTunnelTypePweTtl) {
                mpls_port.egress_label.ttl = 0x1;
            }

            bcm_ltsw_bfd_mpls_label_get(mpls_port.egress_label.label,
                                         mpls_port.egress_label.exp,
                                         0,
                                         mpls_port.egress_label.ttl,
                                         &mpls[count++]);
        }
    }

    /* Set Bottom of Stack if there is no GAL label */
    if (!(endpoint_info->type == bcmBFDTunnelTypeMplsTpCc ||
        endpoint_info->type == bcmBFDTunnelTypeMplsTpCcCv)) {
        mpls[count-1].s = 1;
    } else {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_mpls_gal_label_get(&mpls[count], endpoint_info));
        mpls[count].s = 1;
        count++;
    }

    *mpls_count = count;

    for (i = 0; i < count; i++) {
        labels[i] = ((mpls[i].label & 0xfffff) << 12) | ((mpls[i].exp & 0x7) << 9) |
                    ((mpls[i].s & 0x1) << 8) | mpls[i].ttl;
    }
exit:
    SHR_FUNC_EXIT();

}

static
const char *bcm_ltsw_bfd_diag_code_symbol_get(
        bcm_bfd_diag_code_t diag)
{
    switch(diag) {
        case bcmBFDDiagCodeNone:
            return NO_DIAGNOSTICs;
            break;
        case bcmBFDDiagCodeCtrlDetectTimeExpired:
            return CONTROL_DETECTION_TIME_EXPIREDs;
            break;
        case bcmBFDDiagCodeEchoFailed:
            return ECHO_FUNCTION_FAILEDs;
            break;
        case bcmBFDDiagCodeNeighbourSignalledDown:
            return NEIGHBOR_SIGNALED_SESSION_DOWNs;
            break;
        case bcmBFDDiagCodeForwardingPlaneReset:
            return FORWARDING_PLANE_RESETs;
            break;
        case bcmBFDDiagCodePathDown:
            return PATH_DOWNs;
            break;
        case bcmBFDDiagCodeConcatPathDown:
            return CONCATENATED_PATH_DOWNs;
            break;
        case bcmBFDDiagCodeAdminDown:
            return ADMINISTRATIVELY_DOWNs;
            break;
        case bcmBFDDiagCodeRevConcatPathDown:
            return REVERSE_CONCATENATED_PATH_DOWNs;
            break;
        case bcmBFDDiagCodeMisConnectivityDefect:
            return MISCONNECTIVITY_DEFECTs;
            break;
        default:
            break;
    }
    return("");
}

static
const char *bcm_ltsw_bfd_auth_type_symbol_get(
                             bcm_bfd_auth_type_t type)

{
     switch(type) {
        case bcmBFDAuthTypeNone:
            return NONEs;
            break;
        case bcmBFDAuthTypeSimplePassword:
            return SIMPLE_PASSWORDs;
            break;
        case bcmBFDAuthTypeKeyedSHA1:
            return KEYED_SHA1s;
            break;
        case bcmBFDAuthTypeMeticulousKeyedSHA1:
            return METICULOUS_KEYED_SHA1s;
            break;
        default:
            break;
    }
    return("");
}

static
const char *
bcm_ltsw_bfd_tnl_mpls_ep_encap_type_symbol_get(
        bcm_bfd_tunnel_type_t type)
{

  switch(type) {
        case bcmBFDTunnelTypeMpls:
            return MPLS_LSPs;
            break;
        case bcmBFDTunnelTypeMplsPhp:
            return MPLS_LSP_PHPs;
            break;
        case bcmBFDTunnelTypePweControlWord:
        case bcmBFDTunnelTypePweRouterAlert:
        case bcmBFDTunnelTypePweTtl:
            return PWs;
            break;
        case bcmBFDTunnelTypeMplsTpCc:
            return MPLS_TP_CCs;
            break;
        case bcmBFDTunnelTypeMplsTpCcCv:
            return MPLS_TP_CC_CVs;
            break;
        default:
            break;
    }
    return("");

}

static
bcm_bfd_diag_code_t bcm_ltsw_bfd_diag_code_enum_get(
        const char * str)
{
    if(!sal_strcmp(str, NO_DIAGNOSTICs)) {
        return bcmBFDDiagCodeNone;
    }
    if(!sal_strcmp(str, CONTROL_DETECTION_TIME_EXPIREDs)) {
        return bcmBFDDiagCodeCtrlDetectTimeExpired;
    }
    if(!sal_strcmp(str, ECHO_FUNCTION_FAILEDs)) {
        return bcmBFDDiagCodeEchoFailed;
    }
    if(!sal_strcmp(str, NEIGHBOR_SIGNALED_SESSION_DOWNs)) {
        return bcmBFDDiagCodeNeighbourSignalledDown;
    }
    if(!sal_strcmp(str, FORWARDING_PLANE_RESETs)) {
        return bcmBFDDiagCodeForwardingPlaneReset;
    }
    if(!sal_strcmp(str, PATH_DOWNs)) {
        return bcmBFDDiagCodePathDown;
    }
    if(!sal_strcmp(str, ADMINISTRATIVELY_DOWNs)) {
        return bcmBFDDiagCodeAdminDown;
    }
    if(!sal_strcmp(str, REVERSE_CONCATENATED_PATH_DOWNs)) {
        return bcmBFDDiagCodeRevConcatPathDown;
    }
    if(!sal_strcmp(str, MISCONNECTIVITY_DEFECTs)) {
        return bcmBFDDiagCodeMisConnectivityDefect;
    }
    if(!sal_strcmp(str, CONCATENATED_PATH_DOWNs)) {
        return bcmBFDDiagCodeConcatPathDown;
    }
    return bcmBFDDiagCodeNone;
}

static
bcm_bfd_auth_type_t
bcm_ltsw_bfd_auth_type_enum_get(
        const char * str)
{
    if(!sal_strcmp(str, NONEs)) {
        return bcmBFDAuthTypeNone;
    }

    if(!sal_strcmp(str, SIMPLE_PASSWORDs)) {
        return bcmBFDAuthTypeSimplePassword;
    }

    if(!sal_strcmp(str, KEYED_SHA1s)) {
        return bcmBFDAuthTypeKeyedSHA1;
    }

    if(!sal_strcmp(str, METICULOUS_KEYED_SHA1s)) {
        return bcmBFDAuthTypeMeticulousKeyedSHA1;
    }
    return(bcmBFDAuthTypeNone);
}

uint8_t bcm_ltsw_bfd_vlan_tag_type_enum_get(
        const char * str)
{
    if(!sal_strcmp(str, UNTAGGEDs)) {
        return 0;
    }
    if(!sal_strcmp(str, SINGLE_TAGGEDs)) {
        return 1;
    }
    if(!sal_strcmp(str, DOUBLE_TAGGEDs)) {
        return 2;
    }
    return 0;
}

bcm_bfd_event_type_t
bcm_ltsw_bfd_event_enum_get(const char *str) {
    if(!sal_strcmp(str, LOCAL_STATE_ADMIN_DOWNs)) {
        return bcmBFDEventEndpointLocalSessionStateAdminDown;
    }
    if(!sal_strcmp(str, LOCAL_STATE_DOWNs)) {
        return bcmBFDEventEndpointLocalSessionStateDown;
    }
    if(!sal_strcmp(str, LOCAL_STATE_INITs)) {
        return bcmBFDEventEndpointLocalSessionStateInit;
    }
    if(!sal_strcmp(str, LOCAL_STATE_UPs)) {
        return bcmBFDEventEndpointLocalSessionStateUp;
    }
    if(!sal_strcmp(str, REMOTE_STATE_MODE_CHANGEs)) {
        return bcmBFDEventRemoteStateDiag;
    }
    if(!sal_strcmp(str, REMOTE_DISCRIMINATOR_CHANGEs)) {
        return bcmBFDEventSessionDiscriminatorChange;
    }
    if(!sal_strcmp(str, REMOTE_PARAMETER_CHANGEs)) {
        return bcmBFDEventParameterChange;
    }
    if(!sal_strcmp(str, REMOTE_POLL_BIT_SETs)) {
        return bcmBFDEventEndpointRemotePollBitSet;
    }
    if(!sal_strcmp(str, REMOTE_FINAL_BIT_SETs)) {
        return bcmBFDEventEndpointRemoteFinalBitSet;
    }
    if(!sal_strcmp(str, MISCONNECTIVITY_DEFECTs)) {
        return bcmBFDEventEndpointMisConnectivityDefect;
    }
    if(!sal_strcmp(str, MISCONNECTIVITY_DEFECT_CLEARs)) {
        return bcmBFDEventEndpointMisConnectivityDefectClear;
    }
    if(!sal_strcmp(str, UNEXPECTED_MEG_DEFECTs)) {
        return bcmBFDEventEndpoinUnExpectedMeg;
    }
    if(!sal_strcmp(str, UNEXPECTED_MEG_DEFECT_CLEARs)) {
        return bcmBFDEventEndpoinUnExpectedMegClear;
    }
    return bcmBFDEventCount;
}

static int
bcm_ltsw_bfd_endpoint_common_param_set(int unit,
                    bcmlt_entry_handle_t *entry_hdl,
                    bcm_bfd_endpoint_info_t *ep_info)
{
    const char *symbol;
    bcm_port_t     tx_port = 0;
    bcm_int_bfd_info_t *bfd_info;

    SHR_FUNC_ENTER(unit);

    bfd_info = BFD_LTSW_INFO(unit);

    if (ep_info->flags & BCM_BFD_ENDPOINT_PASSIVE) {
        symbol = PASSIVEs;
    } else {
        symbol = ACTIVEs;
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(*entry_hdl, ROLEs, symbol));

    if (ep_info->flags & BCM_BFD_ENDPOINT_DEMAND) {
        symbol =  DEMANDs;
    } else {
        symbol = ASYNCHRONOUSs;
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(*entry_hdl, MODEs, symbol));

    if (!BFD_ENCAP_TYPE_TNL_MPLS(ep_info->type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*entry_hdl, ECHOs,
                                   (ep_info->flags & BCM_BFD_ECHO) ? 1 : 0));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*entry_hdl, MIN_ECHO_RX_INTERVAL_USECSs,
                                   ep_info->local_min_echo));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*entry_hdl, RX_LOOKUP_VLAN_IDs,
                                   ep_info->rx_pkt_vlan_id));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*entry_hdl, LOCAL_DISCRIMINATORs,
                               ep_info->local_discr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*entry_hdl, REMOTE_DISCRIMINATORs,
                               ep_info->remote_discr));

    symbol = bcm_ltsw_bfd_diag_code_symbol_get(ep_info->local_diag);
    if (symbol) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(*entry_hdl, DIAG_CODEs, symbol));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*entry_hdl, MIN_TX_INTERVAL_USECSs,
                               ep_info->local_min_tx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*entry_hdl, MIN_RX_INTERVAL_USECSs,
                               ep_info->local_min_rx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*entry_hdl, DETECT_MULTIPLIERs,
                               ep_info->local_detect_mult));

    symbol = bcm_ltsw_bfd_auth_type_symbol_get(ep_info->auth);
    if (symbol) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(*entry_hdl, AUTH_TYPEs, symbol));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (ep_info->auth == bcmBFDAuthTypeSimplePassword) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*entry_hdl, OAM_BFD_AUTH_SIMPLE_PASSWORD_IDs,
                                   ep_info->auth_index));
    }

    if ((ep_info->auth == bcmBFDAuthTypeKeyedSHA1) ||
            (ep_info->auth == bcmBFDAuthTypeMeticulousKeyedSHA1)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*entry_hdl, OAM_BFD_AUTH_SHA1_IDs,
                                   ep_info->auth_index));
        if (ep_info->flags & BCM_BFD_ENDPOINT_SHA1_SEQUENCE_INCR) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(*entry_hdl, SHA1_SEQ_NUM_INCREMENTs,
                                       true));
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*entry_hdl, INITIAL_SHA1_SEQ_NUMs,
                                   ep_info->tx_auth_seq));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(*entry_hdl, UDP_SRC_PORTs,
                               ep_info->udp_src_port));

    /* Fill port and cos */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_tx_port_get(unit, entry_hdl,
                                  ep_info, &tx_port, 0));
    /* Fill L2  header params */
    if (bfd_info->tx_raw_ingress) {

        if (BCM_GPORT_INVALID != ep_info->tx_gport) {

            /* Fill port and cos */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_bfd_tx_port_get(unit, entry_hdl,
                                          ep_info, &tx_port, 1));

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_symbol_add(*entry_hdl, TX_MODEs,
                                              CPU_MASQUERADEs));
        } else {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_symbol_add(*entry_hdl, TX_MODEs,
                                              RAW_ETHERNETs));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_bfd_tx_raw_l2_header_get(unit , entry_hdl,
                                        ep_info, tx_port));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_bfd_l2_header_get(unit , entry_hdl,
                                        ep_info, tx_port));
    }
exit:
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_bfd_ipv4_endpoint_entry_set(int unit,
        bool update,
        bcm_bfd_endpoint_info_t *ep_info)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *symbol;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_IPV4_ENDPOINTs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_endpoint_common_param_set(dunit,
                                                &entry_hdl, ep_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_IPV4_ENDPOINT_IDs,
                               ep_info->id));

    if(ep_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) {
        symbol = MULTI_HOPs;
    } else {
        /* Single hop */
        symbol = SINGLE_HOPs;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, TYPEs, symbol));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SRC_IPV4s, ep_info->src_ip_addr));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DST_IPV4s, ep_info->dst_ip_addr));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, TOSs, ep_info->ip_tos));

    if (!(ep_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)) {
        if (ep_info->ip_ttl == 0) {
            ep_info->ip_ttl = 255;
        } else if (ep_info->ip_ttl != 255) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, TTLs, ep_info->ip_ttl));

    if (update == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_ipv6_endpoint_entry_set(int unit,
        bool update,
        bcm_bfd_endpoint_info_t *ep_info)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *symbol;
    uint64_t ipv6_lower = 0, ipv6_upper = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_IPV6_ENDPOINTs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_endpoint_common_param_set(dunit,
                                                &entry_hdl, ep_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_IPV6_ENDPOINT_IDs,
                               ep_info->id));

    if(ep_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) {
        symbol = MULTI_HOPs;
    } else {
        /* Single hop */
        symbol = SINGLE_HOPs;
    }
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, TYPEs, symbol));

    sal_memcpy(&ipv6_lower, ep_info->src_ip6_addr, 8);
    sal_memcpy(&ipv6_upper, &(ep_info->src_ip6_addr[8]), 8);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SRC_IPV6_LOWERs, ipv6_lower));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SRC_IPV6_UPPERs, ipv6_upper));

    sal_memcpy(&ipv6_lower, ep_info->dst_ip6_addr, 8);
    sal_memcpy(&ipv6_upper, &(ep_info->dst_ip6_addr[8]), 8);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DST_IPV6_LOWERs, ipv6_lower));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DST_IPV6_UPPERs, ipv6_upper));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, TRAFFIC_CLASSs,
                               ep_info->ip_tos));

    if (!(ep_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)) {
        if (ep_info->ip_ttl == 0) {
            ep_info->ip_ttl = 255;
        } else if (ep_info->ip_ttl != 255) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, HOP_LIMITs,
                              ep_info->ip_ttl));

    if (update == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_tnl_ipv4_endpoint_entry_set(int unit,
        bool update,
        bcm_bfd_endpoint_info_t *ep_info)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *symbol;
    uint64_t ipv6_lower = 0, ipv6_upper = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_IPV4_ENDPOINTs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_endpoint_common_param_set(dunit,
                                                &entry_hdl, ep_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_IPV4_ENDPOINT_IDs,
        ep_info->id));

    if ((ep_info->type == bcmBFDTunnelTypeIp4in4) ||
        (ep_info->type == bcmBFDTunnelTypeGRE)) {
        symbol = IPV4s;
    } else {
        symbol = IPV6s;
    }

    if ((ep_info->type == bcmBFDTunnelTypeGRE) ||
        (ep_info->type == bcmBFDTunnelTypeGre4In6)) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, GREs, true));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, INNER_IP_TYPEs, symbol));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SRC_IPV4s, ep_info->src_ip_addr));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DST_IPV4s, ep_info->dst_ip_addr));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, TOSs, ep_info->ip_tos));

    if (ep_info->ip_ttl == 0) {
        ep_info->ip_ttl = 255;
    } else if (ep_info->ip_ttl != 255) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, TTLs, ep_info->ip_ttl));

    if (ep_info->type == bcmBFDTunnelTypeIp4in4) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_SRC_IPV4s,
            ep_info->inner_src_ip_addr));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_DST_IPV4s,
            ep_info->inner_dst_ip_addr));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_TOSs,
            ep_info->inner_ip_tos));
    }

    if (ep_info->type == bcmBFDTunnelTypeIp4in6) {
        sal_memcpy(&ipv6_lower, ep_info->inner_src_ip6_addr, 8);
        sal_memcpy(&ipv6_upper, &(ep_info->inner_src_ip6_addr[8]), 8);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_SRC_IPV6_LOWERs, ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_SRC_IPV6_UPPERs, ipv6_upper));

        sal_memcpy(&ipv6_lower, ep_info->inner_dst_ip6_addr, 8);
        sal_memcpy(&ipv6_upper, &(ep_info->inner_dst_ip6_addr[8]), 8);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_DST_IPV6_LOWERs, ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_DST_IPV6_UPPERs, ipv6_upper));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_TRAFFIC_CLASSs,
            ep_info->inner_ip_tos));
    }

    if (update == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_tnl_ipv6_endpoint_entry_set(int unit,
        bool update,
        bcm_bfd_endpoint_info_t *ep_info)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *symbol;
    uint64_t ipv6_lower = 0, ipv6_upper = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_IPV6_ENDPOINTs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_endpoint_common_param_set(dunit,
                                                &entry_hdl, ep_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_IPV6_ENDPOINT_IDs,
                               ep_info->id));

    if ((ep_info->type == bcmBFDTunnelTypeIp6in4) ||
        (ep_info->type == bcmBFDTunnelTypeGre6In4)) {
        symbol = IPV4s;
    } else {
        symbol = IPV6s;
    }

    if ((ep_info->type == bcmBFDTunnelTypeGre6In4) ||
        (ep_info->type == bcmBFDTunnelTypeGre6In6)) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, GREs, true));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, INNER_IP_TYPEs, symbol));

    sal_memcpy(&ipv6_lower, ep_info->src_ip6_addr, 8);
    sal_memcpy(&ipv6_upper, &(ep_info->src_ip6_addr[8]), 8);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SRC_IPV6_LOWERs, ipv6_lower));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, SRC_IPV6_UPPERs, ipv6_upper));

    sal_memcpy(&ipv6_lower, ep_info->dst_ip6_addr, 8);
    sal_memcpy(&ipv6_upper, &(ep_info->dst_ip6_addr[8]), 8);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DST_IPV6_LOWERs, ipv6_lower));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, DST_IPV6_UPPERs, ipv6_upper));

    if (ep_info->type == bcmBFDTunnelTypeIp6in4) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_SRC_IPV4s,
                                    ep_info->inner_src_ip_addr));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_DST_IPV4s,
                                    ep_info->inner_dst_ip_addr));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_TOSs,
                                    ep_info->ip_tos));
    }

    if (ep_info->type == bcmBFDTunnelTypeIp6in6) {
        sal_memcpy(&ipv6_lower, ep_info->inner_src_ip6_addr, 8);
        sal_memcpy(&ipv6_upper, &(ep_info->inner_src_ip6_addr[8]),
                8);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_SRC_IPV6_LOWERs,
                                   ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_SRC_IPV6_UPPERs,
                                   ipv6_upper));

        sal_memcpy(&ipv6_lower, ep_info->inner_dst_ip6_addr, 8);
        sal_memcpy(&ipv6_upper, &(ep_info->inner_dst_ip6_addr[8]),
                8);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_DST_IPV6_LOWERs,
                                   ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_DST_IPV6_UPPERs,
                                   ipv6_upper));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, INNER_TRAFFIC_CLASSs,
                            ep_info->inner_ip_tos));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, TRAFFIC_CLASSs,
                               ep_info->ip_tos));

    if (ep_info->ip_ttl == 0) {
        ep_info->ip_ttl = 255;
    } else if (ep_info->ip_ttl != 255) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, HOP_LIMITs,
                               ep_info->ip_ttl));

    if (update == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_tnl_mpls_endpoint_entry_set(int unit,
        bool update,
        bcm_bfd_endpoint_info_t *ep_info)
{
    int dunit, i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *symbol;
    uint64_t labels[BFD_LTSW_MAX_MPLS_LABELS];
    uint64_t mep_id[BFD_LTSW_MEP_IDENTIFIER_MAX_LENGTH];
    uint64_t ipv6_lower = 0, ipv6_upper = 0;
    int      mpls_count = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_MPLS_ENDPOINTs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_endpoint_common_param_set(dunit,
                                                &entry_hdl, ep_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_MPLS_ENDPOINT_IDs,
                               ep_info->id));

    symbol = bcm_ltsw_bfd_tnl_mpls_ep_encap_type_symbol_get(
            ep_info->type);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, ENCAP_TYPEs, symbol));

    if (ep_info->flags & BCM_BFD_ENDPOINT_IPV6) {
        symbol = IPV6s;
    } else {
        symbol = IPV4s;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, IP_ENCAP_TYPEs, symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_LOOKUP_LABELs,
                                ep_info->label));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_mpls_labels_get(unit, ep_info,
                                     BFD_MPLS_MAX_LABELS, labels, &mpls_count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, NUM_LABELSs, mpls_count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(entry_hdl,
                                     LABEL_STACKs,
                                     0,
                                     labels,
                                     mpls_count));

    if (ep_info->flags & BCM_BFD_ENDPOINT_PWE_ACH) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, PW_ACHs, 1));
    }

    if (ep_info->flags & BCM_BFD_ENDPOINT_IPV6) {
        sal_memcpy(&ipv6_lower, ep_info->src_ip6_addr, 8);
        sal_memcpy(&ipv6_upper, &(ep_info->src_ip6_addr[8]), 8);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, SRC_IPV6_LOWERs, ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, SRC_IPV6_UPPERs, ipv6_upper));

        sal_memcpy(&ipv6_lower, ep_info->dst_ip6_addr, 8);
        sal_memcpy(&ipv6_upper, &(ep_info->dst_ip6_addr[8]), 8);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_IPV6_LOWERs, ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_IPV6_UPPERs, ipv6_upper));
    } else {

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, SRC_IPV4s,
                                   ep_info->src_ip_addr));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, DST_IPV4s,
                                   ep_info->dst_ip_addr));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, TOSs,
            ep_info->ip_tos));
    }

    if (ep_info->type == bcmBFDTunnelTypeMplsTpCcCv) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, SOURCE_MEP_IDENTIFIER_LENGTHs,
                                   ep_info->mep_id_length));

        if (ep_info->mep_id_length) {
            for ( i = 0; i < ep_info->mep_id_length; i++) {
                mep_id[i] = ep_info->mep_id[i];
            }


            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(entry_hdl,
                                             SOURCE_MEP_IDENTIFIERs,
                                             0,
                                             mep_id,
                                             ep_info->mep_id_length));
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, REMOTE_MEP_IDENTIFIER_LENGTHs,
                                    ep_info->remote_mep_id_length));

        if (ep_info->remote_mep_id_length) {
            for ( i = 0; i < ep_info->remote_mep_id_length; i++) {
                mep_id[i] = ep_info->remote_mep_id[i];
            }


            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(entry_hdl,
                                             REMOTE_MEP_IDENTIFIERs,
                                             0,
                                             mep_id,
                                             ep_info->remote_mep_id_length));
        }
    }
    if (update == false) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

static int
bcm_ltsw_bfd_endpoint_common_param_get(int unit,
        bcmlt_entry_handle_t *entry_hdl,
        bcm_bfd_endpoint_info_t *ep_info)
{
    uint64_t val = 0;
    const char *symbol;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(*entry_hdl, ROLEs, &symbol));

    if(!sal_strcmp(symbol, PASSIVEs)) {
        ep_info->flags |= BCM_BFD_ENDPOINT_PASSIVE;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(*entry_hdl, MODEs, &symbol));
    if (!sal_strcmp(symbol, DEMANDs)) {
        ep_info->flags |= BCM_BFD_ENDPOINT_DEMAND;
    }

    if (!BFD_ENCAP_TYPE_TNL_MPLS(ep_info->type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(*entry_hdl, ECHOs, &val));
        if (val) {
            ep_info->flags |= BCM_BFD_ECHO;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(*entry_hdl, MIN_ECHO_RX_INTERVAL_USECSs, &val));
        ep_info->local_min_echo = val;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(*entry_hdl, RX_LOOKUP_VLAN_IDs, &val));
        ep_info->rx_pkt_vlan_id = val;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(*entry_hdl, LOCAL_DISCRIMINATORs, &val ));
    ep_info->local_discr = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(*entry_hdl, REMOTE_DISCRIMINATORs, &val ));
    ep_info->remote_discr = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(*entry_hdl, DIAG_CODEs, &symbol));
    ep_info->local_diag = bcm_ltsw_bfd_diag_code_enum_get(symbol);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(*entry_hdl, MIN_TX_INTERVAL_USECSs, &val));
    ep_info->local_min_tx = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(*entry_hdl, MIN_RX_INTERVAL_USECSs, &val));
    ep_info->local_min_rx = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(*entry_hdl, DETECT_MULTIPLIERs, &val));
    ep_info->local_detect_mult = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(*entry_hdl, AUTH_TYPEs, &symbol));
    ep_info->auth = bcm_ltsw_bfd_auth_type_enum_get(symbol);

    if (ep_info->auth == bcmBFDAuthTypeSimplePassword) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(*entry_hdl, OAM_BFD_AUTH_SIMPLE_PASSWORD_IDs,
                                   &val));
        ep_info->auth_index = val;
    }

    if ((ep_info->auth == bcmBFDAuthTypeKeyedSHA1) ||
            (ep_info->auth == bcmBFDAuthTypeMeticulousKeyedSHA1)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(*entry_hdl, OAM_BFD_AUTH_SHA1_IDs,
                                   &val));
        ep_info->auth_index = val;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(*entry_hdl, SHA1_SEQ_NUM_INCREMENTs,
                                   &val));
        if (val) {
            ep_info->flags |= BCM_BFD_ENDPOINT_SHA1_SEQUENCE_INCR;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(*entry_hdl, INITIAL_SHA1_SEQ_NUMs,
                                   &val));

        ep_info->tx_auth_seq = val;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(*entry_hdl, UDP_SRC_PORTs, &val));
    ep_info->udp_src_port = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(*entry_hdl, COSs, &val));
    ep_info->int_pri = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(*entry_hdl, VLAN_IDs, &val));
    ep_info->pkt_vlan_id = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(*entry_hdl, VLAN_PRIs, &val));
    ep_info->vlan_pri = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(*entry_hdl, INNER_VLAN_IDs, &val));
    ep_info->pkt_inner_vlan_id = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(*entry_hdl, INNER_VLAN_PRIs, &val));
    ep_info->inner_vlan_pri = val;
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_ipv4_endpoint_entry_get(int unit,
        bcm_bfd_endpoint_info_t *ep_info)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    const char *symbol;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_IPV4_ENDPOINTs, &entry_hdl));

    val = ep_info->id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_IPV4_ENDPOINT_IDs, val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATEs, &val));

    if (val != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_endpoint_common_param_get(dunit, &entry_hdl, ep_info));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, TYPEs, &symbol));
    if(!sal_strcmp(symbol, MULTI_HOPs)) {
        ep_info->flags |= BCM_BFD_ENDPOINT_MULTIHOP;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, SRC_IPV4s, &val));
    ep_info->src_ip_addr = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, DST_IPV4s, &val));
    ep_info->dst_ip_addr = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, TOSs, &val));
    ep_info->ip_tos = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, TTLs, &val));
    ep_info->ip_ttl = val;
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_ipv6_endpoint_entry_get(int unit,
        bcm_bfd_endpoint_info_t *ep_info)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    const char *symbol;
    uint64_t ipv6_lower = 0, ipv6_upper = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_IPV6_ENDPOINTs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_IPV6_ENDPOINT_IDs, ep_info->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATEs, &val));

    if (val != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_endpoint_common_param_get(dunit, &entry_hdl, ep_info));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, TYPEs, &symbol));
    if(!sal_strcmp(symbol, MULTI_HOPs)) {
        ep_info->flags |= BCM_BFD_ENDPOINT_MULTIHOP;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, SRC_IPV6_LOWERs, &ipv6_lower));
    sal_memcpy(&ep_info->src_ip6_addr, &ipv6_lower, 8);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, SRC_IPV6_UPPERs, &ipv6_upper));
    sal_memcpy(&ep_info->src_ip6_addr[8], &ipv6_upper, 8);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, DST_IPV6_LOWERs, &ipv6_lower));
    sal_memcpy(&ep_info->dst_ip6_addr, &ipv6_lower, 8);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, DST_IPV6_UPPERs, &ipv6_upper));
    sal_memcpy(&ep_info->dst_ip6_addr[8], &ipv6_upper, 8);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, TRAFFIC_CLASSs, &val));
    ep_info->ip_tos = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, HOP_LIMITs, &val));
    ep_info->ip_ttl = val;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_tnl_ipv4_endpoint_entry_get(int unit,
        bcm_bfd_endpoint_info_t *ep_info)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    uint64_t ipv6_lower = 0, ipv6_upper = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_IPV4_ENDPOINTs, &entry_hdl));

    val = ep_info->id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_IPV4_ENDPOINT_IDs, val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATEs, &val));

    if (val != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_endpoint_common_param_get(dunit, &entry_hdl, ep_info));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, SRC_IPV4s, &val));
    ep_info->src_ip_addr = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, DST_IPV4s, &val));
    ep_info->dst_ip_addr = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, TOSs, &val));
    ep_info->ip_tos = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, TTLs, &val));
    ep_info->ip_ttl = val;

    ep_info->inner_ip_ttl = 255;

    if (ep_info->type == bcmBFDTunnelTypeIp4in4 ||
        ep_info->type == bcmBFDTunnelTypeGRE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_SRC_IPV4s, &val));
        ep_info->inner_src_ip_addr = val;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_DST_IPV4s, &val));
        ep_info->inner_dst_ip_addr = val;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_TOSs, &val));
        ep_info->inner_ip_tos = val;
    }

    if (ep_info->type == bcmBFDTunnelTypeIp4in6 ||
        ep_info->type == bcmBFDTunnelTypeGre4In6) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_SRC_IPV6_LOWERs, &ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_SRC_IPV6_UPPERs, &ipv6_upper));
        sal_memcpy(&ep_info->inner_src_ip6_addr, &ipv6_lower, 8);
        sal_memcpy(&(ep_info->inner_src_ip6_addr[8]),&ipv6_upper, 8);


        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_DST_IPV6_LOWERs, &ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_DST_IPV6_UPPERs, &ipv6_upper));
        sal_memcpy(&ep_info->inner_dst_ip6_addr, &ipv6_lower, 8);
        sal_memcpy(&(ep_info->inner_dst_ip6_addr[8]),&ipv6_upper, 8);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_TRAFFIC_CLASSs, &val));
        ep_info->inner_ip_tos = val;
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_tnl_ipv6_endpoint_entry_get(int unit,
        bcm_bfd_endpoint_info_t *ep_info)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    uint64_t ipv6_lower = 0, ipv6_upper = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_IPV6_ENDPOINTs, &entry_hdl));

    val = ep_info->id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_IPV6_ENDPOINT_IDs, val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATEs, &val));

    if (val != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_endpoint_common_param_get(dunit, &entry_hdl, ep_info));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, SRC_IPV6_LOWERs, &ipv6_lower));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, SRC_IPV6_UPPERs, &ipv6_upper));
    sal_memcpy(&ep_info->src_ip6_addr, &ipv6_lower, 8);
    sal_memcpy(&(ep_info->src_ip6_addr[8]),&ipv6_upper, 8);


    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, DST_IPV6_LOWERs, &ipv6_lower));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, DST_IPV6_UPPERs, &ipv6_upper));
    sal_memcpy(&ep_info->dst_ip6_addr, &ipv6_lower, 8);
    sal_memcpy(&(ep_info->dst_ip6_addr[8]),&ipv6_upper, 8);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, TRAFFIC_CLASSs, &val));
    ep_info->ip_tos = val;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, HOP_LIMITs, &val));
    ep_info->ip_ttl = val;

    ep_info->inner_ip_ttl = 255;

    if (ep_info->type == bcmBFDTunnelTypeIp6in4 ||
            ep_info->type == bcmBFDTunnelTypeGre6In4) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_SRC_IPV4s, &val));
        ep_info->inner_src_ip_addr = val;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_DST_IPV4s, &val));
        ep_info->inner_dst_ip_addr = val;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_TOSs, &val));
        ep_info->inner_ip_tos = val;
    }

    if (ep_info->type == bcmBFDTunnelTypeIp6in6 ||
            ep_info->type == bcmBFDTunnelTypeGre6In6) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_SRC_IPV6_LOWERs, &ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_SRC_IPV6_UPPERs, &ipv6_upper));
        sal_memcpy(&ep_info->inner_src_ip6_addr, &ipv6_lower, 8);
        sal_memcpy(&(ep_info->inner_src_ip6_addr[8]),&ipv6_upper, 8);


        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_DST_IPV6_LOWERs, &ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_DST_IPV6_UPPERs, &ipv6_upper));
        sal_memcpy(&ep_info->inner_dst_ip6_addr, &ipv6_lower, 8);
        sal_memcpy(&(ep_info->inner_dst_ip6_addr[8]),&ipv6_upper, 8);

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, INNER_TRAFFIC_CLASSs, &val));
        ep_info->inner_ip_tos = val;
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_tnl_mpls_endpoint_entry_get(int unit,
        bcm_bfd_endpoint_info_t *ep_info)
{
    int dunit, i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    uint64_t ipv6_lower = 0, ipv6_upper = 0;
    uint32_t r_cnt;
    const char *symbol;
    int      mpls_count = 0;
    uint64_t labels[BFD_LTSW_MAX_MPLS_LABELS];

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_MPLS_ENDPOINTs, &entry_hdl));

    val = ep_info->id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_MPLS_ENDPOINT_IDs, val));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATEs, &val));

    if (val != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_bfd_endpoint_common_param_get(dunit, &entry_hdl, ep_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_LOOKUP_LABELs, &val));
    ep_info->label = val;


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, PW_ACHs, &val));
    if (val) {
        ep_info->flags |= BCM_BFD_ENDPOINT_PWE_ACH;
    }

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(entry_hdl, IP_ENCAP_TYPEs, &symbol));

    if(!sal_strcmp(symbol, IPV4s)) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, SRC_IPV4s, &val));
        ep_info->src_ip_addr = val;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, DST_IPV4s, &val));
        ep_info->dst_ip_addr = val;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, TOSs, &val));
        ep_info->ip_tos = val;
    }

    if(!sal_strcmp(symbol, IPV6s)) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, SRC_IPV6_LOWERs, &ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, SRC_IPV6_UPPERs, &ipv6_upper));

        sal_memcpy(ep_info->src_ip6_addr,&ipv6_lower,  8);
        sal_memcpy( &(ep_info->src_ip6_addr[8]),&ipv6_upper, 8);


        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, DST_IPV6_LOWERs, &ipv6_lower));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, DST_IPV6_UPPERs, &ipv6_upper));

        sal_memcpy(ep_info->dst_ip6_addr,&ipv6_lower,  8);
        sal_memcpy( &(ep_info->dst_ip6_addr[8]),&ipv6_upper, 8);

    }

    if(ep_info->type == bcmBFDTunnelTypeMplsTpCcCv) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, SOURCE_MEP_IDENTIFIER_LENGTHs, &val));
        ep_info->mep_id_length = val;

        for ( i = 0; i < ep_info->mep_id_length; i++) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(entry_hdl,
                                             SOURCE_MEP_IDENTIFIERs,
                                             i,
                                             &val,
                                             1,
                                             &r_cnt));
            ep_info->mep_id[i] = val;

        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, REMOTE_MEP_IDENTIFIER_LENGTHs, &val));
        ep_info->remote_mep_id_length =val;

        for ( i = 0; i < ep_info->remote_mep_id_length; i++) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(entry_hdl,
                                             REMOTE_MEP_IDENTIFIERs,
                                             i,
                                             &val,
                                             1,
                                             &r_cnt));
            ep_info->remote_mep_id[i] = val;

        }
    }
    if ((ep_info->type == bcmBFDTunnelTypeMplsTpCc) ||
            (ep_info->type == bcmBFDTunnelTypeMplsTpCcCv)) {

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, NUM_LABELSs, &val));
        mpls_count = val;

        if (mpls_count) {
            for ( i = 0; i < mpls_count; i++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(entry_hdl,
                                                 LABEL_STACKs,
                                                 i,
                                                 &val,
                                                 1,
                                                 &r_cnt));
                labels[i] = val;
            }
        }
        ep_info->gal_label = labels[mpls_count-1];
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_endpoint_entry_delete(int unit, bcm_bfd_endpoint_t glb_id)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int encap_type = 0;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    encap_type = BFD_EP_ENCAP_TYPE(unit, glb_id);

    if (encap_type == (bcmBFDTunnelTypeUdp | BCM_LTSW_BFD_ENDPOINT_IPV6)) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, OAM_BFD_IPV6_ENDPOINTs, &entry_hdl));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, OAM_BFD_IPV6_ENDPOINT_IDs,
                                   glb_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

    } else if (encap_type == bcmBFDTunnelTypeUdp) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, OAM_BFD_IPV4_ENDPOINTs, &entry_hdl));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, OAM_BFD_IPV4_ENDPOINT_IDs,
                                   glb_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

    } else if (BFD_ENCAP_TYPE_TNL_IPV4(encap_type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_IPV4_ENDPOINTs, &entry_hdl));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_IPV4_ENDPOINT_IDs,
                                   glb_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

    } else if (BFD_ENCAP_TYPE_TNL_IPV6(encap_type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_IPV6_ENDPOINTs, &entry_hdl));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_IPV6_ENDPOINT_IDs,
                                   glb_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

    } else if (BFD_ENCAP_TYPE_TNL_MPLS(encap_type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_MPLS_ENDPOINTs, &entry_hdl));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_MPLS_ENDPOINT_IDs, glb_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
    }

    BFD_GLB_ID_BMP_CLEAR(unit, glb_id);
    BFD_EP_ENCAP_TYPE(unit, glb_id) = 0;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_tx_start_field_set(int unit, bool enable) {

    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_CONTROLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TXs, enable));


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_endpoint_poll_field_set(int unit, bcm_bfd_endpoint_t endpoint) {

    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int encap_type = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    encap_type = BFD_EP_ENCAP_TYPE(unit, endpoint);

    if (encap_type == (bcmBFDTunnelTypeUdp | BCM_LTSW_BFD_ENDPOINT_IPV6)) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, OAM_BFD_IPV6_ENDPOINTs, &entry_hdl));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, OAM_BFD_IPV6_ENDPOINT_IDs, endpoint));

    } else if (encap_type == bcmBFDTunnelTypeUdp) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, OAM_BFD_IPV4_ENDPOINTs, &entry_hdl));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, OAM_BFD_IPV4_ENDPOINT_IDs,
                                   endpoint));

    } else if (BFD_ENCAP_TYPE_TNL_IPV4(encap_type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_IPV4_ENDPOINTs, &entry_hdl));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_IPV4_ENDPOINT_IDs,
                                   endpoint));

    } else if (BFD_ENCAP_TYPE_TNL_IPV6(encap_type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_IPV6_ENDPOINTs, &entry_hdl));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_IPV6_ENDPOINT_IDs,
                                   endpoint));

    } else if (BFD_ENCAP_TYPE_TNL_MPLS(encap_type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, OAM_BFD_TNL_MPLS_ENDPOINTs, &entry_hdl));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, OAM_BFD_TNL_MPLS_ENDPOINT_IDs, endpoint));

    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, POLL_SEQUENCEs,
                               true));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_endpoint_sp_entry_set(int unit, int index, bcm_bfd_auth_simple_password_t *sp)
{
    int dunit, i;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t auth[SIMPLE_PASSWORD_MAX_LENGTH];
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_AUTH_SIMPLE_PASSWORDs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_AUTH_SIMPLE_PASSWORD_IDs, index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, PASSWORD_LENs, sp->length));

    for ( i = 0; i < sp->length; i++) {
        auth[i] = sp->password[i];
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(entry_hdl,
                                     PASSWORDs,
                                     0,
                                     auth,
                                     sp->length));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_endpoint_sp_entry_get(int unit, int index, bcm_bfd_auth_simple_password_t *sp)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    uint32_t r_cnt, i;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_AUTH_SIMPLE_PASSWORDs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_AUTH_SIMPLE_PASSWORD_IDs, index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATEs, &val));

    if (val != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_bfd_endpoint_sp_entry_delete(unit, index));

        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, PASSWORD_LENs, &val));
    sp->length = val;

    for ( i = 0; i < sp->length; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         PASSWORDs,
                                         i,
                                         &val,
                                         1,
                                         &r_cnt));
        sp->password[i] = val;
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_endpoint_sp_entry_delete(int unit, int index)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_AUTH_SIMPLE_PASSWORDs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_AUTH_SIMPLE_PASSWORD_IDs, index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_endpoint_auth_sha1_entry_set(int unit, int index, bcm_bfd_auth_sha1_t *sha1)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t key[SHA1_KEY_LENGTH];
    uint32_t i;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_AUTH_SHA1s, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_AUTH_SHA1_IDs, index));

    for ( i = 0; i < SHA1_KEY_LENGTH; i++) {
        key[i] = sha1->key[i];
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(entry_hdl,
                                     SHA1_KEYs,
                                     0,
                                     key,
                                     SHA1_KEY_LENGTH));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_endpoint_auth_sha1_entry_get(int unit, int index,  bcm_bfd_auth_sha1_t *sha1)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    uint32_t r_cnt, i;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_AUTH_SHA1s, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_AUTH_SHA1_IDs, index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, OPERATIONAL_STATEs, &val));

    if (val != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_bfd_endpoint_auth_sha1_entry_delete(unit, index));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (i = 0; i < SHA1_KEY_LENGTH; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(entry_hdl,
                                         SHA1_KEYs,
                                         i,
                                         &val,
                                         1,
                                         &r_cnt));
        sha1->key[i] = val;
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_endpoint_auth_sha1_entry_delete(int unit, int index)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_AUTH_SHA1s, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, OAM_BFD_AUTH_SHA1_IDs, index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_discard_stat_entry_get(int unit,
        bcm_bfd_discard_stat_t *discarded_info) {
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_STATSs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_VERSION_ERRORs, &val));
    discarded_info->bfd_ver_err = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_LENGTH_ERRORs, &val));
    discarded_info->bfd_len_err = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_ZERO_DETECT_MULTIPLIERs, &val));
    discarded_info->bfd_detect_mult = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_ZERO_MY_DISCRIMINATORs, &val));
    discarded_info->bfd_my_disc = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_P_AND_F_BITS_SETs, &val));
    discarded_info->bfd_p_f_bit = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_M_BIT_SETs, &val));
    discarded_info->bfd_m_bit = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_AUTH_MISMATCHs, &val));
    discarded_info->bfd_auth_type_mismatch = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_AUTH_SIMPLE_PASSWORD_ERRORs, &val));
    discarded_info->bfd_auth_simple_err = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_AUTH_SHA1_ERRORs, &val));
    discarded_info->bfd_auth_m_sha1_err = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_ENDPOINT_NOT_FOUNDs, &val));
    discarded_info->bfd_sess_mismatch = val;


exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_discard_stat_entry_set(int unit,
        bcm_bfd_discard_stat_t *discarded_info) {
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_STATSs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_VERSION_ERRORs, discarded_info->bfd_ver_err));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_LENGTH_ERRORs, discarded_info->bfd_len_err));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_ZERO_DETECT_MULTIPLIERs, discarded_info->bfd_detect_mult));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_ZERO_MY_DISCRIMINATORs, discarded_info->bfd_my_disc));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_P_AND_F_BITS_SETs, discarded_info->bfd_p_f_bit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_M_BIT_SETs, discarded_info->bfd_m_bit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_AUTH_MISMATCHs, discarded_info->bfd_auth_type_mismatch));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_AUTH_SIMPLE_PASSWORD_ERRORs, discarded_info->bfd_auth_simple_err));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_AUTH_SHA1_ERRORs, discarded_info->bfd_auth_m_sha1_err));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_ENDPOINT_NOT_FOUNDs, discarded_info->bfd_sess_mismatch));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_endpoint_stat_entry_get(int unit, bcm_bfd_endpoint_t endpoint,
        bcm_bfd_endpoint_stat_t *ctr_info)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int encap_type = 0;
    const char *table_name = NULL;
    const char *key_name = NULL;
    uint64_t val;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    encap_type = BFD_EP_ENCAP_TYPE(unit, endpoint);

    if (encap_type == (bcmBFDTunnelTypeUdp | BCM_LTSW_BFD_ENDPOINT_IPV6)) {
        table_name = OAM_BFD_IPV6_ENDPOINT_STATSs;
        key_name = OAM_BFD_IPV6_ENDPOINT_IDs;
    } else if (encap_type == bcmBFDTunnelTypeUdp) {
        table_name = OAM_BFD_IPV4_ENDPOINT_STATSs;
        key_name = OAM_BFD_IPV4_ENDPOINT_IDs;
    } else if (BFD_ENCAP_TYPE_TNL_IPV4(encap_type)) {
        table_name = OAM_BFD_TNL_IPV4_ENDPOINT_STATSs;
        key_name = OAM_BFD_TNL_IPV4_ENDPOINT_IDs;
    } else if (BFD_ENCAP_TYPE_TNL_IPV6(encap_type)) {
        table_name = OAM_BFD_TNL_IPV6_ENDPOINT_STATSs;
        key_name = OAM_BFD_TNL_IPV6_ENDPOINT_IDs;
    } else if (BFD_ENCAP_TYPE_TNL_MPLS(encap_type)) {
        table_name = OAM_BFD_TNL_MPLS_ENDPOINT_STATSs;
        key_name = OAM_BFD_TNL_MPLS_ENDPOINT_IDs;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, key_name, endpoint));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_CNTs, &val));
    ctr_info->packets_in = val;
    ctr_info->packets_in_64 = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_DISCARD_CNTs, &val));
    ctr_info->packets_drop_64 = val;
    ctr_info->packets_drop = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, RX_PKT_AUTH_FAILURE_DISCARD_CNTs, &val));
    ctr_info->packets_auth_drop_64 = val;
    ctr_info->packets_auth_drop = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, TX_PKT_CNTs, &val));
    ctr_info->packets_out_64 = val;
    ctr_info->packets_out = val;

    if (!BFD_ENCAP_TYPE_TNL_MPLS(encap_type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, RX_ECHO_REPLY_PKT_CNTs, &val));
        ctr_info->packets_echo_reply = val;
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_endpoint_stat_entry_set(int unit, bcm_bfd_endpoint_t endpoint)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int encap_type = 0;
    const char *table_name = NULL;
    const char *key_name = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    encap_type = BFD_EP_ENCAP_TYPE(unit, endpoint);

    if (encap_type == (bcmBFDTunnelTypeUdp | BCM_LTSW_BFD_ENDPOINT_IPV6)) {
        table_name = OAM_BFD_IPV6_ENDPOINT_STATSs;
        key_name = OAM_BFD_IPV6_ENDPOINT_IDs;
    } else if (encap_type == bcmBFDTunnelTypeUdp) {
        table_name = OAM_BFD_IPV4_ENDPOINT_STATSs;
        key_name = OAM_BFD_IPV4_ENDPOINT_IDs;
    } else if (BFD_ENCAP_TYPE_TNL_IPV4(encap_type)) {
        table_name = OAM_BFD_TNL_IPV4_ENDPOINT_STATSs;
        key_name = OAM_BFD_TNL_IPV4_ENDPOINT_IDs;
    } else if (BFD_ENCAP_TYPE_TNL_IPV6(encap_type)) {
        table_name = OAM_BFD_TNL_IPV6_ENDPOINT_STATSs;
        key_name = OAM_BFD_TNL_IPV6_ENDPOINT_IDs;
    } else if (BFD_ENCAP_TYPE_TNL_MPLS(encap_type)) {
        table_name = OAM_BFD_TNL_MPLS_ENDPOINT_STATSs;
        key_name = OAM_BFD_TNL_MPLS_ENDPOINT_IDs;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, key_name, endpoint));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_CNTs, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_DISCARD_CNTs, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, RX_PKT_AUTH_FAILURE_DISCARD_CNTs, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TX_PKT_CNTs, 0));

    if (!BFD_ENCAP_TYPE_TNL_MPLS(encap_type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, RX_ECHO_REPLY_PKT_CNTs, 0));

    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_event_control_entry_set(int unit, bcm_bfd_event_types_t event_types)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, OAM_BFD_EVENT_CONTROLs, &entry_hdl));

    if (SHR_BITGET(event_types.w,
                bcmBFDEventEndpointLocalSessionStateAdminDown)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, LOCAL_STATE_ADMIN_DOWNs,
                                   true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventEndpointLocalSessionStateDown)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, LOCAL_STATE_DOWNs, true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventEndpointLocalSessionStateInit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, LOCAL_STATE_INITs, true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventEndpointLocalSessionStateUp)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, LOCAL_STATE_UPs, true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventRemoteStateDiag)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, REMOTE_STATE_MODE_CHANGEs, true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventSessionDiscriminatorChange)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, REMOTE_DISCRIMINATOR_CHANGEs, true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventParameterChange)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, REMOTE_PARAMETER_CHANGEs, true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventEndpointRemotePollBitSet)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, REMOTE_POLL_BIT_SETs, true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventEndpointRemoteFinalBitSet)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, REMOTE_FINAL_BIT_SETs, true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventEndpointMisConnectivityDefect)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MISCONNECTIVITY_DEFECTs, true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventEndpointMisConnectivityDefectClear)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, MISCONNECTIVITY_DEFECT_CLEARs, true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventEndpoinUnExpectedMeg)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, UNEXPECTED_MEG_DEFECTs, true));
    }
    if (SHR_BITGET(event_types.w,
                bcmBFDEventEndpoinUnExpectedMegClear)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, UNEXPECTED_MEG_DEFECT_CLEARs, true));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_bfd_event_entry_get(int unit, bcmlt_entry_handle_t entry_hdl,
        bcm_bfd_event_type_t *event, int *ep_id) {
    const char* str;
    uint64_t val = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, EVENTs, &str));
    *event = bcm_ltsw_bfd_event_enum_get(str);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, ENDPOINT_TYPEs, &str));

    if(!sal_strcmp(str, IPV4s)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, OAM_BFD_IPV4_ENDPOINT_IDs, &val));
        *ep_id = val;
    }

    if(!sal_strcmp(str, IPV6s)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, OAM_BFD_IPV6_ENDPOINT_IDs, &val));
        *ep_id = val;
    }

    if(!sal_strcmp(str, TNL_IPV4s)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, OAM_BFD_TNL_IPV4_ENDPOINT_IDs, &val));
        *ep_id = val;
    }

    if(!sal_strcmp(str, TNL_IPV6s)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, OAM_BFD_TNL_IPV6_ENDPOINT_IDs, &val));
        *ep_id = val;
    }

    if(!sal_strcmp(str, TNL_MPLSs)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, OAM_BFD_TNL_MPLS_ENDPOINT_IDs, &val));
        *ep_id = val;
    }

exit:
    SHR_FUNC_EXIT();

}

bcm_bfd_event_type_t
bcm_ltsw_bfd_state_enum_get(const char *str) {
    if(!sal_strcmp(str, ADMIN_DOWNs)) {
        return bcmBFDStateAdminDown;
    }
    if(!sal_strcmp(str, DOWNs)) {
        return bcmBFDStateDown;
    }
    if(!sal_strcmp(str, INITs)) {
        return bcmBFDStateInit;
    }
    if(!sal_strcmp(str, UPs)) {
        return bcmBFDStateUp;
    }
    return bcmBFDStateCount;
}


int bcm_ltsw_bfd_endpoint_status_entry_get(int unit, bcm_bfd_endpoint_t endpoint,
        bcm_bfd_endpoint_info_t *ep_info)
{
    int dunit;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int encap_type = 0;
    const char *table_name = NULL;
    const char *key_name = NULL;
    uint64_t val;
    const char* str;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    encap_type = BFD_EP_ENCAP_TYPE(unit, endpoint);

    if (encap_type == (bcmBFDTunnelTypeUdp | BCM_LTSW_BFD_ENDPOINT_IPV6)) {
        table_name = OAM_BFD_IPV6_ENDPOINT_STATUSs;
        key_name = OAM_BFD_IPV6_ENDPOINT_IDs;
    } else if (encap_type == bcmBFDTunnelTypeUdp) {
        table_name = OAM_BFD_IPV4_ENDPOINT_STATUSs;
        key_name = OAM_BFD_IPV4_ENDPOINT_IDs;
    } else if (BFD_ENCAP_TYPE_TNL_IPV4(encap_type)) {
        table_name = OAM_BFD_TNL_IPV4_ENDPOINT_STATUSs;
        key_name = OAM_BFD_TNL_IPV4_ENDPOINT_IDs;
    } else if (BFD_ENCAP_TYPE_TNL_IPV6(encap_type)) {
        table_name = OAM_BFD_TNL_IPV6_ENDPOINT_STATUSs;
        key_name = OAM_BFD_TNL_IPV6_ENDPOINT_IDs;
    } else if (BFD_ENCAP_TYPE_TNL_MPLS(encap_type)) {
        table_name = OAM_BFD_TNL_MPLS_ENDPOINT_STATUSs;
        key_name = OAM_BFD_TNL_MPLS_ENDPOINT_IDs;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, table_name, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, key_name, endpoint));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, LOCAL_STATEs, &str));
    ep_info->local_state = bcm_ltsw_bfd_state_enum_get(str);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, REMOTE_STATEs, &str));
    ep_info->remote_state = bcm_ltsw_bfd_state_enum_get(str);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, LOCAL_DIAG_CODEs, &str));
    ep_info->local_diag = bcm_ltsw_bfd_diag_code_enum_get(str);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, REMOTE_DIAG_CODEs, &str));
    ep_info->remote_diag = bcm_ltsw_bfd_diag_code_enum_get(str);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, LOCAL_AUTH_SEQ_NUMs, &val));
    ep_info->tx_auth_seq = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, REMOTE_AUTH_SEQ_NUMs, &val));
    ep_info->rx_auth_seq = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, POLL_SEQUENCE_ACTIVEs, &val));
    if (val) {
        ep_info->local_flags |= BCM_BFD_ENDPOINT_LOCAL_FLAGS_P;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, REMOTE_MODEs, &str));

    if (!sal_strcmp(str, DEMANDs)) {
        ep_info->flags |= BCM_BFD_ENDPOINT_DEMAND;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, LOCAL_DISCRIMINATORs, &val));
    ep_info->local_discr = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, REMOTE_DISCRIMINATORs, &val));
    ep_info->remote_discr = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, REMOTE_DETECT_MULTIPLIERs, &val));
    ep_info->remote_detect_mult = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, REMOTE_MIN_TX_INTERVAL_USECSs, &val));
    ep_info->remote_min_tx = val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, REMOTE_MIN_RX_INTERVAL_USECSs, &val));
    ep_info->remote_min_rx = val;

    if (!BFD_ENCAP_TYPE_TNL_MPLS(encap_type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, REMOTE_MIN_ECHO_RX_INTERVAL_USECSs, &val));
        ep_info->remote_min_echo = val;
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}
#endif
