/*! \file bfd_common.c
 *
 * Common functions for both XGS and XFS platforms for BFD.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(INCLUDE_BFD)
/* API related header files */
#include <bcm/bfd.h>

/* SDKLT specific header files */
#include <bcmltd/chip/bcmltd_str.h>

/* Shared library header files */
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>

/* Internal header files */
#include <bcm_int/control.h>
#include <bcm_int/ltsw/bfd_int.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/issu.h>

#define BSL_LOG_MODULE BSL_LS_BCM_BFD

/* BFD global information */
bcm_int_bfd_info_t bfd_glb_info[BCM_MAX_NUM_UNITS] = {{0}};

/* Checks that endpoint index is within valid range */
#define BFD_LTSW_ENDPOINT_INDEX_CHECK(u_, index_)                            \
    if ((index_) < 0 || (index_) >= BFD_LTSW_INFO(u_)->max_endpoints) {      \
        SHR_ERR_EXIT(SHR_E_PARAM);                                    \
    }

/* Checks that SHA1 Authentication index is valid */
#define BFD_LTSW_AUTH_SHA1_INDEX_CHECK(u_, index_)                          \
    if ((index_) >= BFD_LTSW_INFO(u_)->max_auth_sha1_keys) {                \
        SHR_ERR_EXIT(SHR_E_PARAM);                                   \
    }

/* Checks that Simple Password Authentication index is valid */
#define BFD_LTSW_AUTH_SP_INDEX_CHECK(u_, index_)                            \
    if ((index_) >= BFD_LTSW_INFO(u_)->max_auth_simple_password_keys) {     \
        SHR_ERR_EXIT(SHR_E_PARAM);                                   \
    }
/******************************************************************************
 * Private Functions
 */

static void
bcm_ltsw_bfd_info_init(int unit, bcm_int_bfd_info_t *bfd_info)
{

    bfd_info->max_endpoints = bcmi_ltsw_property_get(unit,
            BCMI_CPN_BFD_NUM_SESSIONS,
            256);

    bfd_info->max_auth_simple_password_keys = bcmi_ltsw_property_get(unit,
            BCMI_CPN_BFD_SIMPLE_PASSWORD_KEYS,
            0);

    bfd_info->max_auth_sha1_keys = bcmi_ltsw_property_get(unit,
            BCMI_CPN_BFD_SHA1_KEYS,
            0);

    bfd_info->max_pkt_size = bcmi_ltsw_property_get(unit,
            BCMI_CPN_BFD_MAX_PKT_SIZE,
            0);

    bfd_info->state_down_event_on_ep_creation = bcmi_ltsw_property_get(unit,
            BCMI_CPN_BFD_SESSION_DOWN_EVENT_ON_CREATE,
            0);

    bfd_info->static_remote_disc = bcmi_ltsw_property_get(unit,
            BCMI_CPN_BFD_REMOTE_DISCRIMINATOR,
            0);

    bfd_info->cpi = bcmi_ltsw_property_get(unit,
            BCMI_CPN_BFD_CONTROL_PLANE_INDEPENDENCE,
            1);

    bfd_info->use_ep_id_as_disc = bcmi_ltsw_property_get(unit,
            BCMI_CPN_BFD_USE_ENDPOINT_ID_AS_DISCRIMINATOR,
            0);

    bfd_info->tx_raw_ingress = bcmi_ltsw_property_get(unit,
            BCMI_CPN_BFD_TX_RAW_INGRESS_ENABLE,
            0);
}

static int
bcm_ltsw_bfd_clear_tables(int unit)
{
    int i, num;
    const char *ltname[] = {
        OAM_BFD_AUTH_SHA1s,
        OAM_BFD_AUTH_SIMPLE_PASSWORDs,
        OAM_BFD_CONTROLs,
        OAM_BFD_EVENTs,
        OAM_BFD_EVENT_STATUSs,
        OAM_BFD_IPV4_ENDPOINTs,
        OAM_BFD_IPV6_ENDPOINTs,
        OAM_BFD_TNL_IPV4_ENDPOINTs,
        OAM_BFD_TNL_IPV6_ENDPOINTs,
        OAM_BFD_TNL_MPLS_ENDPOINTs,
    };

    SHR_FUNC_ENTER(unit);

    num = COUNTOF(ltname);

    for (i = 0; i < num; i++) {
        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, ltname[i]));
    }
    SHR_VERBOSE_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

static void
bcm_ltsw_bfd_event_callback(bcmlt_table_notif_info_t *notify_info, void *user_data)
{
    bcm_int_bfd_info_t *bfd_info;
    bfd_ltsw_event_handler_t *event_handler;
    bcm_bfd_event_types_t cb_events;
    bcm_bfd_event_type_t  reported_event;
    int unit, sess_id ;
    bcmlt_entry_handle_t ent_hdl;
    uint32_t rv = 0;

    unit = notify_info->unit;
    ent_hdl = notify_info->entry_hdl;

    bfd_info = BFD_LTSW_INFO(unit);


    rv = bcm_ltsw_bfd_event_entry_get(unit, ent_hdl, &reported_event, &sess_id);
    if (SHR_FAILURE(rv)) {
        return;
    }

    /* Loop over registered callbacks,
     * If any match the events field, then invoke
     */
    sal_memset(&cb_events, 0, sizeof(cb_events));
    for (event_handler = bfd_info->event_handler_list;
            event_handler != NULL;
            event_handler = event_handler->next) {
        if (SHR_BITGET(event_handler->event_types.w, reported_event)) {
            SHR_BITSET(cb_events.w, reported_event);
            event_handler->cb(unit, 0,
                    cb_events, sess_id,
                    event_handler->user_data);
        }
    }
}
/******************************************************************************
 * Public Functions
 */
/*
 * Function:
 *      bcm_ltsw_bfd_init_common
 * Purpose:
 *      bfd initialization common to all devices.
 * Parameters:
 *      unit              - (IN)     BCM device number
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
int bcm_ltsw_bfd_init_common(int unit)
{
    int warmboot = false;
    bcm_int_bfd_info_t *bfd_info = BFD_LTSW_INFO(unit);
    uint32_t ltid_bitmap_size = 0;
    uint32_t id_to_type_map_size = 0;
    uint32_t ha_req_size, ha_alloc_size;
    void *ptr = NULL;
    uint32_t rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    warmboot = bcmi_warmboot_get(unit);

    /* Get values from config file */
    bcm_ltsw_bfd_info_init(unit, bfd_info);

    if (!warmboot) {
        if (bfd_info->init) {
            SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_bfd_detach_common(unit));
        }

        SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_bfd_control_entry_set(unit, false, bfd_info));
    }

    /* Get and check if app got initialized.
       During warm boot entry_get will return not_found,
       If app was not initialized in cold boot */

    rv = bcm_ltsw_bfd_control_entry_get(unit, bfd_info);

    if ((rv != SHR_E_NONE) || (!bfd_info->init)) {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                        "BFD(unit %d) Warning: BFD app not initialized.\n"),
                    unit));
        SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_bfd_detach_common(unit));
        SHR_EXIT();
    }

    ltid_bitmap_size = sizeof (bcmi_bfd_epid_bmp_t);

    ha_req_size = ltid_bitmap_size;
    ha_alloc_size = ha_req_size;

    ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_BFD, BFD_SUB_COMP_ID_GLOBAL_LIST,
                                 "bcmBfd", &ha_alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);

    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* Report the structure array to ISSU. */
    rv = bcmi_ltsw_issu_struct_info_report(unit,
                                           BCMI_HA_COMP_ID_BFD,
                                           BFD_SUB_COMP_ID_GLOBAL_LIST,
                                           0,
                                           sizeof(bcmi_bfd_epid_bmp_t),
                                           1,
                                           BCMI_BFD_EPID_BMP_T_ID);

    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (!warmboot) {
        sal_memset(ptr, 0, ha_req_size);
    }

    bfd_info->ha_info.glb_id_bmp = ptr;
    ptr = NULL;

    /*Allocate for all max endpoints*/
    id_to_type_map_size = sizeof(bcmi_bfd_ep_info_t) * bfd_info->max_endpoints;

    ha_req_size = id_to_type_map_size;
    ha_alloc_size = ha_req_size;

    ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_BFD, BFD_SUB_COMP_ID_PER_EP_INFO,
                                 "bcmBfd", &ha_alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);

    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warmboot) {
        sal_memset(ptr, 0, ha_req_size);
    }

    /* Report the structure array to ISSU. */
    rv = bcmi_ltsw_issu_struct_info_report(unit,
                                           BCMI_HA_COMP_ID_BFD,
                                           BFD_SUB_COMP_ID_PER_EP_INFO,
                                           0,
                                           sizeof(bcmi_bfd_ep_info_t),
                                           bfd_info->max_endpoints,
                                           BCMI_BFD_EP_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    bfd_info->ha_info.ep_info = ptr;

    SHR_IF_ERR_EXIT
        (bcmlt_table_subscribe(unit,
                "OAM_BFD_EVENT",
                &bcm_ltsw_bfd_event_callback,
                NULL));

    bfd_info->init = 1;
exit:
    if(SHR_FUNC_ERR()) {
        SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_bfd_detach_common(unit));
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_detach_common
 * Purpose:
 *      BFD de-initialization common to all devices.
 * Parameters:
 *      unit              - (IN)     BCM device number
 * Returns:
 *      SHR_E_XXX   - BCM error code.
 */
int
bcm_ltsw_bfd_detach_common(int unit)
{
    int warmboot = false;
    bcm_int_bfd_info_t *bfd_info = BFD_LTSW_INFO(unit);
    SHR_FUNC_ENTER(unit);

    warmboot = bcmi_warmboot_get(unit);

    if (!warmboot) {
        /* Clear all LTs */
        SHR_IF_ERR_VERBOSE_EXIT(
                bcm_ltsw_bfd_clear_tables(unit));
    }

    if (!warmboot && bfd_info->ha_info.glb_id_bmp) {
        (void)bcmi_ltsw_ha_mem_free(unit, bfd_info->ha_info.glb_id_bmp);
    }
    if (!warmboot && bfd_info->ha_info.ep_info) {
        (void)bcmi_ltsw_ha_mem_free(unit, bfd_info->ha_info.ep_info);
    }

    bfd_info->init = 0;
exit:
    SHR_FUNC_EXIT();
}

static int bcm_ltsw_int_bfd_endpoint_create(int unit,
        bcm_bfd_endpoint_info_t *endpoint_info) {
    int glb_id = 0;
    bool ep_created = false;

    SHR_FUNC_ENTER(unit);

    glb_id = endpoint_info->id;

    if (endpoint_info->type == bcmBFDTunnelTypeUdp) {
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_bfd_ipv6_endpoint_entry_set(unit, false, endpoint_info));
            ep_created = true;

            BFD_EP_ENCAP_TYPE(unit, glb_id) = endpoint_info->type | BCM_LTSW_BFD_ENDPOINT_IPV6;

            SHR_IF_ERR_EXIT
                (bcm_ltsw_bfd_ipv6_endpoint_entry_get(unit, endpoint_info));

        } else {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_bfd_ipv4_endpoint_entry_set(unit, false, endpoint_info));
            ep_created = true;

            BFD_EP_ENCAP_TYPE(unit, glb_id) = endpoint_info->type;

            SHR_IF_ERR_EXIT
                (bcm_ltsw_bfd_ipv4_endpoint_entry_get(unit, endpoint_info));
        }
    } else if (BFD_ENCAP_TYPE_TNL_IPV4(endpoint_info->type)) {
                SHR_IF_ERR_EXIT
                    (bcm_ltsw_bfd_tnl_ipv4_endpoint_entry_set(unit, false, endpoint_info));
                ep_created = true;

                BFD_EP_ENCAP_TYPE(unit, glb_id) = endpoint_info->type;

                SHR_IF_ERR_EXIT
                    (bcm_ltsw_bfd_tnl_ipv4_endpoint_entry_get(unit, endpoint_info));
    } else if (BFD_ENCAP_TYPE_TNL_IPV6(endpoint_info->type)) {

                SHR_IF_ERR_EXIT
                    (bcm_ltsw_bfd_tnl_ipv6_endpoint_entry_set(unit, false, endpoint_info));
                ep_created = true;

                BFD_EP_ENCAP_TYPE(unit, glb_id) = endpoint_info->type;

                SHR_IF_ERR_EXIT
                    (bcm_ltsw_bfd_tnl_ipv6_endpoint_entry_get(unit, endpoint_info));
    } else if (BFD_ENCAP_TYPE_TNL_MPLS(endpoint_info->type)) {
                SHR_IF_ERR_EXIT
                    (bcm_ltsw_bfd_tnl_mpls_endpoint_entry_set(unit, false, endpoint_info));
                ep_created = true;

                BFD_EP_ENCAP_TYPE(unit, glb_id) = endpoint_info->type;

                SHR_IF_ERR_EXIT
                    (bcm_ltsw_bfd_tnl_mpls_endpoint_entry_get(unit, endpoint_info));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    BFD_GLB_ID_BMP_SET(unit, glb_id);
    BFD_EP_EGRESS_IF(unit, glb_id) = endpoint_info->egress_if;
    BFD_EP_GPORT(unit, glb_id) = endpoint_info->gport;
    BFD_EP_TX_GPORT(unit, glb_id) = endpoint_info->tx_gport;

exit:
    if (SHR_FAILURE(_func_rv) && ep_created) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_endpoint_entry_delete(unit, glb_id));
    }
    SHR_FUNC_EXIT();
}


static int bcm_ltsw_int_bfd_update_validity_check (int unit,
        bcm_bfd_endpoint_info_t *endpoint_info,
        bcm_bfd_endpoint_info_t *cur_info) {
    uint32_t encap = 0;

    SHR_FUNC_ENTER(unit);

    /* Update the encap data */
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_ENCAP_SET) {
        encap = 1;
    }

    /* IP/MPLS values can't change without encap flag */
    if (!encap) {
        if (cur_info->dst_ip_addr != endpoint_info->dst_ip_addr) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (sal_memcmp(cur_info->dst_ip6_addr, endpoint_info->dst_ip6_addr,
                    BCM_IP6_ADDRLEN) != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (cur_info->src_ip_addr != endpoint_info->src_ip_addr) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (sal_memcmp(cur_info->src_ip6_addr, endpoint_info->src_ip6_addr,
                    BCM_IP6_ADDRLEN) != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (cur_info->ip_tos != endpoint_info->ip_tos) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (cur_info->ip_ttl != endpoint_info->ip_ttl) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (cur_info->inner_dst_ip_addr != endpoint_info->inner_dst_ip_addr) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (sal_memcmp(cur_info->inner_dst_ip6_addr, endpoint_info->inner_dst_ip6_addr,
                    BCM_IP6_ADDRLEN) != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (cur_info->inner_src_ip_addr != endpoint_info->inner_src_ip_addr) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (sal_memcmp(cur_info->inner_src_ip6_addr, endpoint_info->inner_src_ip6_addr,
                    BCM_IP6_ADDRLEN) != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (cur_info->inner_ip_tos != endpoint_info->inner_ip_tos) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (cur_info->inner_ip_ttl != endpoint_info->inner_ip_ttl) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (cur_info->udp_src_port != endpoint_info->udp_src_port) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (cur_info->label != endpoint_info->label) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int bcm_ltsw_int_bfd_endpoint_update(int unit,
        bcm_bfd_endpoint_info_t *endpoint_info)
{
    bcm_bfd_endpoint_info_t cur_info = {0};
    int encap_type = 0;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    encap_type = BFD_EP_ENCAP_TYPE(unit, endpoint_info->id);

    /* Get the current entry */
    cur_info.id = endpoint_info->id;

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_get_common(unit, endpoint_info->id,
                                          &cur_info));

    /* Validate if encap parameters are changed */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_int_bfd_update_validity_check(unit, endpoint_info, &cur_info));

    if (encap_type == bcmBFDTunnelTypeUdp) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_ipv4_endpoint_entry_set(unit, true, endpoint_info));
    } else if (encap_type == (bcmBFDTunnelTypeUdp | BCM_LTSW_BFD_ENDPOINT_IPV6)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_ipv6_endpoint_entry_set(unit, true, endpoint_info));
    } else if (BFD_ENCAP_TYPE_TNL_IPV4(endpoint_info->type)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_tnl_ipv4_endpoint_entry_set(unit, true, endpoint_info));
    } else if (BFD_ENCAP_TYPE_TNL_IPV6(endpoint_info->type)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_tnl_ipv6_endpoint_entry_set(unit, true, endpoint_info));
    } else if (BFD_ENCAP_TYPE_TNL_MPLS(endpoint_info->type)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_tnl_mpls_endpoint_entry_set(unit, true, endpoint_info));
    }

    /* Do endpoint get to check if update is success, if not roll back to prev entry */
   if (encap_type == bcmBFDTunnelTypeUdp) {
        rv = bcm_ltsw_bfd_ipv4_endpoint_entry_get(unit, endpoint_info);
    } else if (encap_type == (bcmBFDTunnelTypeUdp | BCM_LTSW_BFD_ENDPOINT_IPV6)) {
        rv = bcm_ltsw_bfd_ipv6_endpoint_entry_get(unit, endpoint_info);
    } else if (BFD_ENCAP_TYPE_TNL_IPV4(endpoint_info->type)) {
        rv = bcm_ltsw_bfd_tnl_ipv4_endpoint_entry_get(unit, endpoint_info);
    } else if (BFD_ENCAP_TYPE_TNL_IPV6(endpoint_info->type)) {
        rv = bcm_ltsw_bfd_tnl_ipv6_endpoint_entry_get(unit, endpoint_info);
    } else if (BFD_ENCAP_TYPE_TNL_MPLS(endpoint_info->type)) {
        rv = bcm_ltsw_bfd_tnl_mpls_endpoint_entry_get(unit, endpoint_info);
    }

   if (SHR_FAILURE(rv)) {
       if (encap_type == bcmBFDTunnelTypeUdp) {
           SHR_IF_ERR_EXIT
               (bcm_ltsw_bfd_ipv4_endpoint_entry_set(unit, true, &cur_info));
       } else if (encap_type == (bcmBFDTunnelTypeUdp | BCM_LTSW_BFD_ENDPOINT_IPV6)) {
           SHR_IF_ERR_EXIT
               (bcm_ltsw_bfd_ipv6_endpoint_entry_set(unit, true, &cur_info));
       } else if (BFD_ENCAP_TYPE_TNL_IPV4(endpoint_info->type)) {
           SHR_IF_ERR_EXIT
               (bcm_ltsw_bfd_tnl_ipv4_endpoint_entry_set(unit, true, &cur_info));
       } else if (BFD_ENCAP_TYPE_TNL_IPV6(endpoint_info->type)) {
           SHR_IF_ERR_EXIT
               (bcm_ltsw_bfd_tnl_ipv6_endpoint_entry_set(unit, true, &cur_info));
       } else if (BFD_ENCAP_TYPE_TNL_MPLS(endpoint_info->type)) {
           SHR_IF_ERR_EXIT
               (bcm_ltsw_bfd_tnl_mpls_endpoint_entry_set(unit, true, &cur_info));
       }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_endpoint_create_common(int unit,
                              bcm_bfd_endpoint_info_t *endpoint_info)
{
    bcm_int_bfd_info_t *bfd_info = BFD_LTSW_INFO(unit);
    bool id_found = FALSE;
    uint8_t local_echo = 0, update = 0;
    int id = 0;

    SHR_FUNC_ENTER(unit);

    if (bfd_info == NULL || !bfd_info->init) {
        /* App not initialized. */
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (endpoint_info == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* A unique local discriminator should be nonzero */
    if (!bfd_info->use_ep_id_as_disc && !endpoint_info->local_discr) {
         LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                        "BFD(unit %d) Warning: Local discriminator cannot be zero.\n"),
                    unit));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Enable the BFD echo mode */
    local_echo = (endpoint_info->flags & BCM_BFD_ECHO) ? 1 : 0;

    /* BFD Echo mode should be enabled with IPv4/IPv6 Enacap only*/
    if (local_echo && (endpoint_info->type != bcmBFDTunnelTypeUdp)) {
        LOG_ERROR(BSL_LS_BCM_BFD, (BSL_META_U(unit,
                        "ECHO mode is supported on BFD over IPV4 and IPV6 tunnel only\n")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Create or Update */
    update = (endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE) ? 1 : 0;

    if (endpoint_info->flags & BCM_BFD_ENDPOINT_WITH_ID) {
        BFD_LTSW_ENDPOINT_INDEX_CHECK(unit, endpoint_info->id);

            if (!update && BFD_GLB_ID_BMP_GET(unit, endpoint_info->id)) {
                LOG_ERROR(BSL_LS_BCM_BFD,
                    (BSL_META_U(unit,
                        "BFD(unit %d) Error: Endpoint ID is already in use\n"), unit));
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }

            if (update && !BFD_GLB_ID_BMP_GET(unit, endpoint_info->id)) {
                LOG_ERROR(BSL_LS_BCM_BFD,
                    (BSL_META_U(unit,
                        "BFD(unit %d) Error: Endpoint ID not found\n"), unit));
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
    } else {
        if (update) {    /* Update specified with no ID */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        for (id = 0; id < bfd_info->max_endpoints; id++) {
            if (!BFD_GLB_ID_BMP_GET(unit, id)) {
                id_found = TRUE;
                endpoint_info->id = id;
                break;
            }
        }

        if (id_found == FALSE) {
            LOG_ERROR(BSL_LS_BCM_BFD,
                    (BSL_META_U(unit,
                                "BFD(unit %d) Error: All BFD IDs are in use\n"), unit));
            SHR_ERR_EXIT(SHR_E_FULL);
        }
        endpoint_info->id = id;
    }

    if (!update) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_int_bfd_endpoint_create(unit, endpoint_info));
    } else {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_int_bfd_endpoint_update(unit, endpoint_info));
    }

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_endpoint_destroy_common(int unit,
                                         bcm_bfd_endpoint_t endpoint) {
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_ENDPOINT_INDEX_CHECK(unit, endpoint);

    if (!BFD_GLB_ID_BMP_GET(unit, endpoint)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_entry_delete(unit, endpoint));
exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_destroy_all_common(int unit) {
    int id = 0;
    bcm_int_bfd_info_t *bfd_info = BFD_LTSW_INFO(unit);

    SHR_FUNC_ENTER(unit);

    for (id = 0; id < bfd_info->max_endpoints; id++) {
        if (BFD_GLB_ID_BMP_GET(unit, id)) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_bfd_endpoint_entry_delete(unit, id));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_endpoint_get_common(int unit,
        bcm_bfd_endpoint_t endpoint,
        bcm_bfd_endpoint_info_t *endpoint_info) {

    int encap_type = 0;
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_ENDPOINT_INDEX_CHECK(unit, endpoint);

    if (!BFD_GLB_ID_BMP_GET(unit, endpoint)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    endpoint_info->id = endpoint;
    encap_type = BFD_EP_ENCAP_TYPE(unit, endpoint);

    if (encap_type == bcmBFDTunnelTypeUdp) {
        endpoint_info->type = bcmBFDTunnelTypeUdp;
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_ipv4_endpoint_entry_get(unit, endpoint_info));

    } else if (encap_type == (bcmBFDTunnelTypeUdp | BCM_LTSW_BFD_ENDPOINT_IPV6)) {
        endpoint_info->type = bcmBFDTunnelTypeUdp;
        endpoint_info->flags |= BCM_BFD_ENDPOINT_IPV6;
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_ipv6_endpoint_entry_get(unit, endpoint_info));

    } else if (BFD_ENCAP_TYPE_TNL_IPV4(encap_type)) {
        endpoint_info->type = encap_type;
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_tnl_ipv4_endpoint_entry_get(unit, endpoint_info));

    } else if (BFD_ENCAP_TYPE_TNL_IPV6(encap_type)) {
        endpoint_info->type = encap_type;
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_tnl_ipv6_endpoint_entry_get(unit, endpoint_info));

    } else if (BFD_ENCAP_TYPE_TNL_MPLS(encap_type)) {
        endpoint_info->type = encap_type;
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_tnl_mpls_endpoint_entry_get(unit, endpoint_info));
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_status_entry_get(unit, endpoint, endpoint_info));

    endpoint_info->id = endpoint;
    endpoint_info->egress_if = BFD_EP_EGRESS_IF(unit, endpoint);
    endpoint_info->gport = BFD_EP_GPORT(unit, endpoint);
    endpoint_info->tx_gport = BFD_EP_TX_GPORT(unit, endpoint);

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_tx_set_common(int unit, bool enable) {

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_tx_start_field_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

int bcm_ltsw_bfd_endpoint_poll_common(int unit, bcm_bfd_endpoint_t endpoint)
{

    SHR_FUNC_ENTER(unit);

    BFD_LTSW_ENDPOINT_INDEX_CHECK(unit, endpoint);

    if (!BFD_GLB_ID_BMP_GET(unit, endpoint)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_poll_field_set(unit, endpoint));
exit:
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_bfd_auth_simple_password_set_common(int unit,
        int index,
        bcm_bfd_auth_simple_password_t *sp)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_PARAM_NULL_CHECK(sp);

    BFD_LTSW_AUTH_SP_INDEX_CHECK(unit, index);

    /* Password length MUST be from 1 to 16 bytes */
    if ((sp->length == 0) ||
            ( sp->length > SIMPLE_PASSWORD_MAX_LENGTH)) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "BFD simple password length must be from 1 to 16 bytes\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_sp_entry_set(unit, index, sp));

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_sp_entry_get(unit, index, sp));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_auth_simple_password_get_common(int unit,
        int index,
        bcm_bfd_auth_simple_password_t *sp)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_PARAM_NULL_CHECK(sp);

    BFD_LTSW_AUTH_SP_INDEX_CHECK(unit, index);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_sp_entry_get(unit, index, sp));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_auth_sha1_set_common(int unit,
        int index,
        bcm_bfd_auth_sha1_t *sha1)
{

    SHR_FUNC_ENTER(unit);

    BFD_LTSW_PARAM_NULL_CHECK(sha1);

    BFD_LTSW_AUTH_SHA1_INDEX_CHECK(unit, index);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_auth_sha1_entry_set(unit, index, sha1));

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_auth_sha1_entry_get(unit, index, sha1));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_auth_sha1_get_common(int unit,
        int index,
        bcm_bfd_auth_sha1_t *sha1)
{

    SHR_FUNC_ENTER(unit);

    BFD_LTSW_PARAM_NULL_CHECK(sha1);

    BFD_LTSW_AUTH_SHA1_INDEX_CHECK(unit, index);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_auth_sha1_entry_get(unit, index, sha1));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_endpoint_stat_get_common(int unit,
                                bcm_bfd_endpoint_t endpoint,
                                bcm_bfd_endpoint_stat_t *ctr_info, uint32 options)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_ENDPOINT_INDEX_CHECK(unit, endpoint);

    BFD_LTSW_PARAM_NULL_CHECK(ctr_info);

    if (!BFD_GLB_ID_BMP_GET(unit, endpoint)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (options & BCM_BFD_ENDPOINT_STAT_CLEAR) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_endpoint_stat_entry_set(unit, endpoint));
    } else {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_endpoint_stat_entry_get(unit, endpoint, ctr_info));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_event_register_common(int unit,
                             bcm_bfd_event_types_t event_types,
                             bcm_bfd_event_cb cb, void *user_data)
{
    bcm_int_bfd_info_t *bfd_info = BFD_LTSW_INFO(unit);
    bfd_ltsw_event_handler_t *event_handler;
    bfd_ltsw_event_handler_t *previous = NULL;
    bcm_bfd_event_type_t event_type;
    int result;
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_PARAM_NULL_CHECK(cb);

    SHR_BITTEST_RANGE(event_types.w, 0, bcmBFDEventCount, result);

    if (result == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (SHR_BITGET(event_types.w, bcmBFDEventStateChange) ||
            SHR_BITGET(event_types.w,  bcmBFDEventEndpointFlagsChange))
    {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (event_handler = bfd_info->event_handler_list;
            event_handler != NULL;
            event_handler = event_handler->next) {
        if (event_handler->cb == cb) {
            break;
        }
        previous = event_handler;
    }

    if (event_handler == NULL) {
        /* This handler hasn't been registered yet */
        BCMINT_BFD_MEM_ALLOC(event_handler, sizeof(*event_handler), "BFD event handler");

        event_handler->next = NULL;
        event_handler->cb = cb;

        SHR_BITCLR_RANGE(event_handler->event_types.w, 0, bcmBFDEventCount);
        if (previous != NULL) {
            previous->next = event_handler;
        } else {
            bfd_info->event_handler_list = event_handler;
        }
    }

    for (event_type = 0; event_type < bcmBFDEventCount; ++event_type) {
        if (SHR_BITGET(event_types.w, event_type)) {
            if (!SHR_BITGET(event_handler->event_types.w, event_type)) {
                /* This handler isn't handling this event yet */
                SHR_BITSET(event_handler->event_types.w, event_type);
            }
        }
    }

    event_handler->user_data = user_data;

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_event_control_entry_set(unit, event_types));
exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_bfd_event_unregister_common(int unit,
        bcm_bfd_event_types_t event_types,
        bcm_bfd_event_cb cb)
{
    bcm_int_bfd_info_t *bfd_info = BFD_LTSW_INFO(unit);
    bfd_ltsw_event_handler_t *event_handler;
    bfd_ltsw_event_handler_t *previous = NULL;
    bcm_bfd_event_type_t event_type;
    int result;
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_PARAM_NULL_CHECK(cb);

    SHR_BITTEST_RANGE(event_types.w, 0, bcmBFDEventCount, result);

    if (result == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (SHR_BITGET(event_types.w, bcmBFDEventStateChange) ||
            SHR_BITGET(event_types.w,  bcmBFDEventEndpointFlagsChange))
    {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (event_handler = bfd_info->event_handler_list;
            event_handler != NULL;
            event_handler = event_handler->next) {
        if (event_handler->cb == cb) {
            break;
        }
        previous = event_handler;
    }

    if (event_handler == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    for (event_type = 0; event_type < bcmBFDEventCount; ++event_type) {
        if (SHR_BITGET(event_types.w, event_type)) {
            SHR_BITCLR(event_handler->event_types.w, event_type);
        }
    }

    SHR_BITTEST_RANGE(event_handler->event_types.w, 0, bcmBFDEventCount,
            result);

    if (result == 0) {
        /* No more events for this handler to handle */
        if (previous != NULL) {
            previous->next = event_handler->next;
        } else {
            bfd_info->event_handler_list = event_handler->next;
        }

        BCMINT_BFD_MEM_FREE(event_handler);
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_event_control_entry_set(unit, event_types));
exit:
    SHR_FUNC_EXIT();
}
#endif
