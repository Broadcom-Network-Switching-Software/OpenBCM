/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Kernel Networking Management
 */

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/higig.h>
#include <soc/dcbformats.h>
#include <soc/knet.h>

#include <bcm/knet.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/esw/cosq.h>
#include <shared/bsl.h>

#ifdef INCLUDE_KNET

STATIC uint32
_rx_reason_get(int unit, soc_rx_reasons_t *reasons)
{
    soc_rx_reason_t *map;
    uint32 reason = 0;
    uint32 mask = 1;
    int i;

    if ((map = (SOC_DCB_RX_REASON_MAPS(unit))[0]) == NULL) {
        return reason;
    }

    for (i = 0; i < 32; i++) {
        if (SOC_RX_REASON_GET(*reasons, map[i])) {
            reason |= mask;
        }
        mask <<= 1;
    }
    return reason;
}

STATIC uint32
_rx_reason_hi_get(int unit, soc_rx_reasons_t *reasons)
{
    soc_rx_reason_t *map;
    uint32 reason = 0;
    uint32 mask = 1;
    int i;

    if ((map = (SOC_DCB_RX_REASON_MAPS(unit))[0]) == NULL) {
        return reason;
    }

    if (soc_feature(unit, soc_feature_dcb_reason_hi)) {
        for (i = 32; i < 64; i++) {
            if (SOC_RX_REASON_GET(*reasons, map[i])) {
                reason |= mask;
            }
            mask <<= 1;
        }
    }
    return reason;
}

STATIC void
_higig_match_set(bcm_knet_filter_t *filter, uint32 *mh_data, uint32 *mh_mask)
{
#if defined(BCM_HIGIG_SUPPORT)
    soc_higig_hdr_t *hg_data = (soc_higig_hdr_t *)mh_data;
    soc_higig_hdr_t *hg_mask = (soc_higig_hdr_t *)mh_mask;

    if (filter == NULL) {
        return;
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODPORT) {
        hg_data->src_port = filter->m_src_modport;
        hg_mask->src_port = 0;
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODID) {
        hg_data->src_mod = filter->m_src_modid;
        hg_mask->src_mod = 0;
    }
    /* Adjust for endianess because soc_higig_hdr_t uses bit fields. */
    mh_data[0] = soc_ntohl(mh_data[0]);
    mh_data[1] = soc_ntohl(mh_data[1]);
    mh_data[2] = soc_ntohl(mh_data[2]);
    mh_mask[0] = soc_ntohl(mh_mask[0]);
    mh_mask[1] = soc_ntohl(mh_mask[1]);
    mh_mask[2] = soc_ntohl(mh_mask[2]);
#endif
}

STATIC void
_higig2_match_set(bcm_knet_filter_t *filter, uint32 *mh_data, uint32 *mh_mask)
{
#if defined(BCM_HIGIG2_SUPPORT)
    soc_higig2_hdr_t *hg_data = (soc_higig2_hdr_t *)mh_data;
    soc_higig2_hdr_t *hg_mask = (soc_higig2_hdr_t *)mh_mask;

    if (filter == NULL) {
        return;
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODPORT) {
        hg_data->ppd_overlay1.src_port = filter->m_src_modport;
        hg_mask->ppd_overlay1.src_port = 0;
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODID) {
        hg_data->ppd_overlay1.src_mod = filter->m_src_modid;
        hg_mask->ppd_overlay1.src_mod = 0;
    }
    /* Adjust for endianess because soc_higig2_hdr_t uses bit fields. */
    mh_data[0] = soc_ntohl(mh_data[0]);
    mh_data[1] = soc_ntohl(mh_data[1]);
    mh_data[2] = soc_ntohl(mh_data[2]);
    mh_data[3] = soc_ntohl(mh_data[3]);
    mh_mask[0] = soc_ntohl(mh_mask[0]);
    mh_mask[1] = soc_ntohl(mh_mask[1]);
    mh_mask[2] = soc_ntohl(mh_mask[2]);
    mh_mask[3] = soc_ntohl(mh_mask[3]);
#endif
}

STATIC int
_trav_filter_clean(int unit, bcm_knet_filter_t *filter, void *user_data)
{
    return bcm_esw_knet_filter_destroy(unit, filter->id);
}

#endif /* INCLUDE_KNET */

/*
 * Function:
 *      bcm_esw_knet_init
 * Purpose:
 *      Initialize the kernel networking subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_knet_init(int unit)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    bcm_knet_filter_t filter;

    rv = bcm_esw_knet_cleanup(unit);

    if (BCM_SUCCESS(rv)) {
        /* Map all queues to primary Rx DMA channel */
        rv = _bcm_common_rx_queue_channel_set(unit, -1, 1);
    }

    if (soc_property_get(unit, spn_KNET_FILTER_PERSIST, 0)) {
        /* Do not create default filter */
        return rv;
    }

    if (BCM_SUCCESS(rv)) {
        bcm_knet_filter_t_init(&filter);
        filter.type = BCM_KNET_FILTER_T_RX_PKT;
        filter.dest_type = BCM_KNET_DEST_T_BCM_RX_API;
        filter.priority = 255;
        sal_strcpy(filter.desc, "DefaultRxAPI");
        rv = bcm_esw_knet_filter_create(unit, &filter);
    }
    return rv; 
#endif
}

/*
 * Function:
 *      bcm_esw_knet_cleanup
 * Purpose:
 *      Clean up the kernel networking subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_knet_cleanup(int unit)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;

    if (soc_property_get(unit, spn_KNET_FILTER_PERSIST, 0)) {
        /* Leave filters in place */
        return BCM_E_NONE;
    }

    rv = bcm_esw_knet_filter_traverse(unit, _trav_filter_clean, NULL);

    return BCM_SUCCESS(rv) ? BCM_E_NONE : rv; 
#endif
}

/*
 * Function:
 *      bcm_esw_knet_netif_create
 * Purpose:
 *      Create a kernel network interface.
 * Parameters:
 *      unit - (IN) Unit number.
 *      netif - (IN/OUT) Network interface configuration
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_knet_netif_create(int unit, bcm_knet_netif_t *netif)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    int qnum;
    kcom_msg_netif_create_t netif_create;
    int max_port_cos_queues;

    sal_memset(&netif_create, 0, sizeof(netif_create));
    netif_create.hdr.opcode = KCOM_M_NETIF_CREATE;
    netif_create.hdr.unit = unit;

    switch (netif->type) {
    case BCM_KNET_NETIF_T_TX_CPU_INGRESS:
        netif_create.netif.type = KCOM_NETIF_T_VLAN;
        break;
    case BCM_KNET_NETIF_T_TX_LOCAL_PORT:
        netif_create.netif.type = KCOM_NETIF_T_PORT;
        break;
    case BCM_KNET_NETIF_T_TX_META_DATA:
        netif_create.netif.type = KCOM_NETIF_T_META;
        break;
    default:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "KNET: Unsupported interface type\n")));
        return BCM_E_PARAM;
    }

    if (netif->flags & BCM_KNET_NETIF_F_ADD_TAG) {
        netif_create.netif.flags |= KCOM_NETIF_F_ADD_TAG;
    }
    if (netif->flags & BCM_KNET_NETIF_F_RCPU_ENCAP) {
        netif_create.netif.flags |= KCOM_NETIF_F_RCPU_ENCAP;
    }
    if (netif->flags & BCM_KNET_NETIF_F_KEEP_RX_TAG) {
        netif_create.netif.flags |= KCOM_NETIF_F_KEEP_RX_TAG;
    }
    netif_create.netif.cb_user_data = netif->cb_user_data;
    netif_create.netif.vlan = netif->vlan;
    netif_create.netif.port = netif->port;
    netif_create.netif.phys_port = SOC_INFO(unit).port_l2p_mapping[netif->port];

    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_cosq_num_get(unit, &max_port_cos_queues));
    } else {
        BCM_IF_ERROR_RETURN(bcm_esw_cosq_config_get(unit, &max_port_cos_queues));
    }
    if (!((netif->cosq >= 0) && (netif->cosq < max_port_cos_queues))) {
        return BCM_E_PARAM;
    }

    if (BCM_SUCCESS(soc_esw_hw_qnum_get(unit, netif->port, netif->cosq, &qnum))) {
        netif_create.netif.qnum = qnum;
    }
    sal_memcpy(netif_create.netif.macaddr, netif->mac_addr, 6);
    sal_memcpy(netif_create.netif.name, netif->name,
               sizeof(netif_create.netif.name) - 1);

    rv = soc_knet_cmd_req((kcom_msg_t *)&netif_create,
                          sizeof(netif_create), sizeof(netif_create));
    if (BCM_SUCCESS(rv)) {
        /* ID and interface name are assigned by kernel */
        netif->id = netif_create.netif.id;
        sal_memcpy(netif->name, netif_create.netif.name,
                   sizeof(netif->name) - 1);
    }
    return rv;
#endif
}

/*
 * Function:
 *      bcm_esw_knet_netif_destroy
 * Purpose:
 *      Destroy a kernel network interface.
 * Parameters:
 *      unit - (IN) Unit number.
 *      netif_id - (IN) Network interface ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_knet_netif_destroy(int unit, int netif_id)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    kcom_msg_netif_destroy_t netif_destroy;

    sal_memset(&netif_destroy, 0, sizeof(netif_destroy));
    netif_destroy.hdr.opcode = KCOM_M_NETIF_DESTROY;
    netif_destroy.hdr.unit = unit;

    netif_destroy.hdr.id = netif_id;
    
    return soc_knet_cmd_req((kcom_msg_t *)&netif_destroy,
                            sizeof(netif_destroy), sizeof(netif_destroy));
#endif
}

/*
 * Function:
 *      bcm_esw_knet_netif_get
 * Purpose:
 *      Get a kernel network interface configuration.
 * Parameters:
 *      unit - (IN) Unit number.
 *      netif_id - (IN) Network interface ID
 *      netif - (OUT) Network interface configuration
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_knet_netif_get(int unit, int netif_id, bcm_knet_netif_t *netif)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    kcom_msg_netif_get_t netif_get;

    sal_memset(&netif_get, 0, sizeof(netif_get));
    netif_get.hdr.opcode = KCOM_M_NETIF_GET;
    netif_get.hdr.unit = unit;

    netif_get.hdr.id = netif_id;

    rv = soc_knet_cmd_req((kcom_msg_t *)&netif_get,
                          sizeof(netif_get.hdr), sizeof(netif_get));

    if (BCM_SUCCESS(rv)) {
        bcm_knet_netif_t_init(netif);

        switch (netif_get.netif.type) {
        case KCOM_NETIF_T_VLAN:
            netif->type = BCM_KNET_NETIF_T_TX_CPU_INGRESS;
            break;
        case KCOM_NETIF_T_PORT:
            netif->type = BCM_KNET_NETIF_T_TX_LOCAL_PORT;
            break;
        case KCOM_NETIF_T_META:
            netif->type = BCM_KNET_NETIF_T_TX_META_DATA;
            break;
        default:
            /* Unknown type - defaults to VLAN */
            break;
        }

        if (netif_get.netif.flags & KCOM_NETIF_F_ADD_TAG) {
            netif->flags |= BCM_KNET_NETIF_F_ADD_TAG;
        }
        if (netif_get.netif.flags & KCOM_NETIF_F_RCPU_ENCAP) {
            netif->flags |= BCM_KNET_NETIF_F_RCPU_ENCAP;
        }
        if (netif_get.netif.flags & KCOM_NETIF_F_KEEP_RX_TAG) {
            netif->flags |= BCM_KNET_NETIF_F_KEEP_RX_TAG;
        }

        netif->id = netif_get.netif.id;
        netif->vlan = netif_get.netif.vlan;
        netif->port = netif_get.netif.port;
        netif->cosq = netif_get.netif.qnum -
                      SOC_INFO(unit).port_uc_cosq_base[netif->port];
        sal_memcpy(netif->mac_addr, netif_get.netif.macaddr, 6);
        sal_memcpy(netif->name, netif_get.netif.name,
                   sizeof(netif->name) - 1);
    }

    return rv;
#endif
}

/*
 * Function:
 *      bcm_esw_knet_netif_traverse
 * Purpose:
 *      Traverse kernel network interface objects
 * Parameters:
 *      unit - (IN) Unit number.
 *      trav_fn - (IN) User provided call back function
 *      user_data - (IN) User provided data used as input param for callback function
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_knet_netif_traverse(int unit, bcm_knet_netif_traverse_cb trav_fn,
                            void *user_data)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv, idx;
    bcm_knet_netif_t netif;
    kcom_msg_netif_list_t netif_list;

    if (trav_fn == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(&netif_list, 0, sizeof(netif_list));
    netif_list.hdr.opcode = KCOM_M_NETIF_LIST;
    netif_list.hdr.unit = unit;

    rv = soc_knet_cmd_req((kcom_msg_t *)&netif_list,
                          sizeof(netif_list.hdr), sizeof(netif_list));

    if (BCM_SUCCESS(rv)) {
        for (idx = 0; idx < netif_list.ifcnt; idx++) {
            rv = bcm_esw_knet_netif_get(unit, netif_list.id[idx], &netif);
            if (BCM_SUCCESS(rv)) {
                rv = trav_fn(unit, &netif, user_data);
            }
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    return rv;
#endif
}

/*
 * Function:
 *      bcm_esw_knet_filter_create
 * Purpose:
 *      Create a kernel packet filter.
 * Parameters:
 *      unit - (IN) Unit number.
 *      filter - (IN/OUT) Rx packet filter configuration
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_knet_filter_create(int unit, bcm_knet_filter_t *filter)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    int idx, pdx;
    int oob_size;
    int data_offset;
    uint32 reason, reason_hi, bpdu;
    kcom_msg_filter_create_t filter_create;

    sal_memset(&filter_create, 0, sizeof(filter_create));
    filter_create.hdr.opcode = KCOM_M_FILTER_CREATE;
    filter_create.hdr.unit = unit;

    filter_create.filter.type = KCOM_FILTER_T_RX_PKT;

    switch (filter->dest_type) {
    case BCM_KNET_DEST_T_NULL:
        filter_create.filter.dest_type = KCOM_DEST_T_NULL;
        break;
    case BCM_KNET_DEST_T_NETIF:
        filter_create.filter.dest_type = KCOM_DEST_T_NETIF;
        break;
    case BCM_KNET_DEST_T_BCM_RX_API:
        filter_create.filter.dest_type = KCOM_DEST_T_API;
        break;
    case BCM_KNET_DEST_T_CALLBACK:
        filter_create.filter.dest_type = KCOM_DEST_T_CB;
        break;
    default:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "KNET: Unsupported destination type\n")));
        return BCM_E_PARAM;
    }

    switch (filter->mirror_type) {
    case BCM_KNET_DEST_T_NULL:
        filter_create.filter.mirror_type = KCOM_DEST_T_NULL;
        break;
    case BCM_KNET_DEST_T_NETIF:
        filter_create.filter.mirror_type = KCOM_DEST_T_NETIF;
        break;
    case BCM_KNET_DEST_T_BCM_RX_API:
        filter_create.filter.mirror_type = KCOM_DEST_T_API;
        break;
    case BCM_KNET_DEST_T_CALLBACK:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "KNET: Cannot mirror to callback\n")));
        return BCM_E_PARAM;
    default:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "KNET: Unsupported mirror type\n")));
        return BCM_E_PARAM;
    }

    if (filter->flags & BCM_KNET_FILTER_F_STRIP_TAG) {
        filter_create.filter.flags |= KCOM_FILTER_F_STRIP_TAG;
    }

    filter_create.filter.cb_user_data = filter->cb_user_data;
    filter_create.filter.dest_id = filter->dest_id;
    filter_create.filter.dest_proto = filter->dest_proto;
    filter_create.filter.mirror_id = filter->mirror_id;
    filter_create.filter.mirror_proto = filter->mirror_proto;

    filter_create.filter.priority = filter->priority;
    sal_strlcpy(filter_create.filter.desc, filter->desc,
                sizeof(filter_create.filter.desc));

    oob_size = 0;
    if (filter->match_flags & ~BCM_KNET_FILTER_M_RAW) {
        oob_size = SOC_DCB_SIZE(unit);
    }

    /* Create inverted mask */
    for (idx = 0; idx < oob_size; idx++) {
        filter_create.filter.mask.b[idx] = 0xff;
    }

    bpdu = 0;
    if (BCM_RX_REASON_GET(filter->m_reason, bcmRxReasonBpdu)){
        bpdu = 1;
    }
    reason = _rx_reason_get(unit, &filter->m_reason);
    reason_hi = 0;
    if (soc_feature(unit, soc_feature_dcb_reason_hi)) {
        reason_hi = _rx_reason_hi_get(unit, &filter->m_reason);
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 38) {
        reason &= ~0x30000;
        if ((BCM_RX_REASON_GET(filter->m_reason, socRxReasonSampleSource) &&
             BCM_RX_REASON_GET(filter->m_reason, socRxReasonSampleSourceFlex)) ||
            (BCM_RX_REASON_GET(filter->m_reason, socRxReasonSampleSource) &&
             BCM_RX_REASON_GET(filter->m_reason, socRxReasonSampleDest)) ||
            (BCM_RX_REASON_GET(filter->m_reason, socRxReasonSampleSourceFlex) &&
             BCM_RX_REASON_GET(filter->m_reason, socRxReasonSampleDest))) {
            /* TH3 can not support these cases in one filter */
            return BCM_E_PARAM;
        } else {
            if (BCM_RX_REASON_GET(filter->m_reason, socRxReasonSampleSourceFlex)) {
                reason |= 0x10000;
            }
            if (BCM_RX_REASON_GET(filter->m_reason, socRxReasonSampleDest)) {
                reason |= 0x20000;
            }
            if (BCM_RX_REASON_GET(filter->m_reason, socRxReasonSampleSource)) {
                reason |= 0x30000;
            }
        }
    }
#endif

    /* Check if specified reason is supported */
    if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
        if ((reason + reason_hi + bpdu) == 0) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "KNET: Unsupported Rx reason\n")));
            return BCM_E_PARAM;
        }
    }

#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 9) {
        dcb9_t *dcb_data = (dcb9_t *)&filter_create.filter.data;
        dcb9_t *dcb_mask = (dcb9_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 11) {
        dcb11_t *dcb_data = (dcb11_t *)&filter_create.filter.data;
        dcb11_t *dcb_mask = (dcb11_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_RAPTOR_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 12) {
        dcb12_t *dcb_data = (dcb12_t *)&filter_create.filter.data;
        dcb12_t *dcb_mask = (dcb12_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;

        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_FIREBOLT2_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 13) {
        dcb13_t *dcb_data = (dcb13_t *)&filter_create.filter.data;
        dcb13_t *dcb_mask = (dcb13_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 14) {
        dcb14_t *dcb_data = (dcb14_t *)&filter_create.filter.data;
        dcb14_t *dcb_mask = (dcb14_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->reason_hi = reason_hi;
            dcb_mask->reason_hi = ~reason_hi;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined (BCM_RAVEN_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 15) {
        dcb15_t *dcb_data = (dcb15_t *)&filter_create.filter.data;
        dcb15_t *dcb_mask = (dcb15_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else if (SOC_DCB_TYPE(unit) == 18) {
        dcb18_t *dcb_data = (dcb18_t *)&filter_create.filter.data;
        dcb18_t *dcb_mask = (dcb18_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined (BCM_SCORPION_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 16) {
        dcb16_t *dcb_data = (dcb16_t *)&filter_create.filter.data;
        dcb16_t *dcb_mask = (dcb16_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined (BCM_HAWKEYE_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 17) {
        dcb17_t *dcb_data = (dcb17_t *)&filter_create.filter.data;
        dcb17_t *dcb_mask = (dcb17_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_SHADOW_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 19) {
        dcb19_t *dcb_data = (dcb19_t *)&filter_create.filter.data;
        dcb19_t *dcb_mask = (dcb19_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->reason_hi = reason_hi;
            dcb_mask->reason_hi = ~reason_hi;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if ((SOC_DCB_TYPE(unit) == 20) || (SOC_DCB_TYPE(unit) == 30)) {
        dcb20_t *dcb_data = (dcb20_t *)&filter_create.filter.data;
        dcb20_t *dcb_mask = (dcb20_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->reason_hi = reason_hi;
            dcb_mask->reason_hi = ~reason_hi;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 21) {
        dcb21_t *dcb_data = (dcb21_t *)&filter_create.filter.data;
        dcb21_t *dcb_mask = (dcb21_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->reason_hi = reason_hi;
            dcb_mask->reason_hi = ~reason_hi;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 23) {
        dcb23_t *dcb_data = (dcb23_t *)&filter_create.filter.data;
        dcb23_t *dcb_mask = (dcb23_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->word4.overlay1.outer_vid = filter->m_vlan;
            dcb_mask->word4.overlay1.outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->reason_hi = reason_hi;
            dcb_mask->reason_hi = ~reason_hi;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_KATANA_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 24) {
        dcb24_t *dcb_data = (dcb24_t *)&filter_create.filter.data;
        dcb24_t *dcb_mask = (dcb24_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->word4.overlay1.outer_vid = filter->m_vlan;
            dcb_mask->word4.overlay1.outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->reason_hi = reason_hi;
            dcb_mask->reason_hi = ~reason_hi;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 26) {
        dcb26_t *dcb_data = (dcb26_t *)&filter_create.filter.data;
        dcb26_t *dcb_mask = (dcb26_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->word4.overlay1.outer_vid = filter->m_vlan;
            dcb_mask->word4.overlay1.outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->reason_hi = reason_hi;
            dcb_mask->reason_hi = ~reason_hi;
            dcb_data->switch_pkt = 1;
            dcb_mask->switch_pkt = 0;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 29) {
        dcb29_t *dcb_data = (dcb29_t *)&filter_create.filter.data;
        dcb29_t *dcb_mask = (dcb29_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->word4.overlay1.outer_vid = filter->m_vlan;
            dcb_mask->word4.overlay1.outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->word2.overlay1.reason_hi = reason_hi;
            dcb_mask->word2.overlay1.reason_hi = ~reason_hi;
            dcb_data->switch_pkt = 1;
            dcb_mask->switch_pkt = 0;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_GREYHOUND_SUPPORT)
        if (SOC_DCB_TYPE(unit) == 31) {
            dcb31_t *dcb_data = (dcb31_t *)&filter_create.filter.data;
            dcb31_t *dcb_mask = (dcb31_t *)&filter_create.filter.mask;
    
            /* Set module header match fields */
            _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);
    
            if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
                dcb_data->word4.overlay1.outer_vid = filter->m_vlan;
                dcb_mask->word4.overlay1.outer_vid = 0;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
                dcb_data->srcport = filter->m_ingport;
                dcb_mask->srcport = 0;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
                dcb_data->reason = reason;
                dcb_mask->reason = ~reason;
                dcb_data->reason_hi = reason_hi;
                dcb_mask->reason_hi = ~reason_hi;
                dcb_data->bpdu = bpdu;
                dcb_mask->bpdu = ~bpdu;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
                dcb_data->match_rule = filter->m_fp_rule;
                dcb_mask->match_rule = 0;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
                dcb_data->error = 1;
                dcb_mask->error = 0;
            }
        } else
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 32) {
        dcb32_t *dcb_data = (dcb32_t *)&filter_create.filter.data;
        dcb32_t *dcb_mask = (dcb32_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->reason_hi = reason_hi;
            dcb_mask->reason_hi = ~reason_hi;
            dcb_data->switch_pkt = 1;
            dcb_mask->switch_pkt = 0;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 33) {
        dcb33_t *dcb_data = (dcb33_t *)&filter_create.filter.data;
        dcb33_t *dcb_mask = (dcb33_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->word4.overlay1.outer_vid = filter->m_vlan;
            dcb_mask->word4.overlay1.outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->reason_hi = reason_hi;
            dcb_mask->reason_hi = ~reason_hi;
            dcb_data->switch_pkt = 1;
            dcb_mask->switch_pkt = 0;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 34) {
        dcb34_t *dcb_data = (dcb34_t *)&filter_create.filter.data;
        dcb34_t *dcb_mask = (dcb34_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->word4.overlay1.outer_vid = filter->m_vlan;
            dcb_mask->word4.overlay1.outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->reason_hi = reason_hi;
            dcb_mask->reason_hi = ~reason_hi;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_APACHE_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 35) {
        dcb35_t *dcb_data = (dcb35_t *)&filter_create.filter.data;
        dcb35_t *dcb_mask = (dcb35_t *)&filter_create.filter.mask;

        /* Set module header match fields */
        _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            dcb_data->word4.overlay1.outer_vid = filter->m_vlan;
            dcb_mask->word4.overlay1.outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->srcport = filter->m_ingport;
            dcb_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            dcb_data->reason = reason;
            dcb_mask->reason = ~reason;
            dcb_data->reason_hi = reason_hi;
            dcb_mask->reason_hi = ~reason_hi;
            dcb_data->switch_pkt = 1;
            dcb_mask->switch_pkt = 0;
            dcb_data->bpdu = bpdu;
            dcb_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 36) {
        ep_to_cpu_hdr36_t *hdr_data = (ep_to_cpu_hdr36_t *)&filter_create.filter.data;
        ep_to_cpu_hdr36_t *hdr_mask = (ep_to_cpu_hdr36_t *)&filter_create.filter.mask;
        dcb36_t *dcb_data = (dcb36_t *)&filter_create.filter.data.b[sizeof(ep_to_cpu_hdr36_t) - sizeof(dcb36_t)];
        dcb36_t *dcb_mask = (dcb36_t *)&filter_create.filter.mask.b[sizeof(ep_to_cpu_hdr36_t) - sizeof(dcb36_t)];

        oob_size = sizeof(ep_to_cpu_hdr36_t);

        /* Create inverted mask */
        for (idx = 0; idx < oob_size; idx++) {
            filter_create.filter.mask.b[idx] = 0xff;
        }

        /* Set module header match fields */
        _higig2_match_set(filter, &hdr_data->mh0, &hdr_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            hdr_data->outer_vid = filter->m_vlan;
            hdr_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            hdr_data->word8.overlay1.srcport = filter->m_ingport;
            hdr_mask->word8.overlay1.srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            hdr_data->reason = reason;
            hdr_mask->reason = ~reason;
            hdr_data->reason_hi = reason_hi;
            hdr_mask->reason_hi = ~reason_hi;
            hdr_data->switch_pkt = 1;
            hdr_mask->switch_pkt = 0;
            hdr_data->bpdu = bpdu;
            hdr_mask->bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            hdr_data->match_rule = filter->m_fp_rule;
            hdr_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

#if defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_DCB_TYPE(unit) == 37) {
            dcb37_t *dcb_data = (dcb37_t *)&filter_create.filter.data;
            dcb37_t *dcb_mask = (dcb37_t *)&filter_create.filter.mask;

            /* Set module header match fields */
            _higig2_match_set(filter, &dcb_data->mh0, &dcb_mask->mh0);

            if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
                dcb_data->word4.overlay1.outer_vid = filter->m_vlan;
                dcb_mask->word4.overlay1.outer_vid = 0;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
                dcb_data->srcport = filter->m_ingport;
                dcb_mask->srcport = 0;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
                dcb_data->reason = reason;
                dcb_mask->reason = ~reason;
                dcb_data->reason_hi = reason_hi;
                dcb_mask->reason_hi = ~reason_hi;
                dcb_data->bpdu = bpdu;
                dcb_mask->bpdu = ~bpdu;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
                dcb_data->match_rule = filter->m_fp_rule;
                dcb_mask->match_rule = 0;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
                dcb_data->error = 1;
                dcb_mask->error = 0;
            }
        }  else
#endif

#if defined(BCM_FIRELIGHT_SUPPORT)
        if (SOC_DCB_TYPE(unit) == 40) {
            ep_to_cpu_hdr40_t *hdr_data = (ep_to_cpu_hdr40_t *)&filter_create.filter.data;
            ep_to_cpu_hdr40_t *hdr_mask = (ep_to_cpu_hdr40_t *)&filter_create.filter.mask;
            dcb40_t *dcb_data = (dcb40_t *)&filter_create.filter.data.b[sizeof(ep_to_cpu_hdr40_t) - sizeof(dcb40_t)];
            dcb40_t *dcb_mask = (dcb40_t *)&filter_create.filter.mask.b[sizeof(ep_to_cpu_hdr40_t) - sizeof(dcb40_t)];

            oob_size = sizeof(ep_to_cpu_hdr40_t);

            /* Create inverted mask */
            for (idx = 0; idx < oob_size; idx++) {
                filter_create.filter.mask.b[idx] = 0xff;
            }

            /* Set module header match fields */
            _higig2_match_set(filter, &hdr_data->mh0, &hdr_mask->mh0);
            if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
                hdr_data->word6.overlay1.outer_vid = filter->m_vlan;
                hdr_mask->word6.overlay1.outer_vid = 0;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
                hdr_data->srcport = filter->m_ingport;
                hdr_mask->srcport = 0;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
                hdr_data->reason = reason;
                hdr_mask->reason = ~reason;
                hdr_data->reason_hi = reason_hi;
                hdr_mask->reason_hi = ~reason_hi;
                hdr_data->switch_pkt = 1;
                hdr_mask->switch_pkt = 0;
                hdr_data->bpdu = bpdu;
                hdr_mask->bpdu = ~bpdu;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
                hdr_data->match_rule = filter->m_fp_rule;
                hdr_mask->match_rule = 0;
            }
            if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
                dcb_data->error = 1;
                dcb_mask->error = 0;
            }
        } else
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_DCB_TYPE(unit) == 38) {
        ep_to_cpu_hdr38_t *hdr_data = (ep_to_cpu_hdr38_t *)&filter_create.filter.data;
        ep_to_cpu_hdr38_t *hdr_mask = (ep_to_cpu_hdr38_t *)&filter_create.filter.mask;
        dcb38_t *dcb_data;
        dcb38_t *dcb_mask;

        oob_size = soc_cmicx_pktdma_header_size_get(unit);
        dcb_data = (dcb38_t *)&filter_create.filter.data.b[oob_size - sizeof(dcb38_t)];
        dcb_mask = (dcb38_t *)&filter_create.filter.mask.b[oob_size - sizeof(dcb38_t)];

        /* Create inverted mask */
        for (idx = 0; idx < oob_size; idx++) {
            filter_create.filter.mask.b[idx] = 0xff;
        }

        /* Set module header match fields */
        _higig2_match_set(filter, &hdr_data->mh0, &hdr_mask->mh0);

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            hdr_data->outer_vid = filter->m_vlan;
            hdr_mask->outer_vid = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            hdr_data->srcport = filter->m_ingport;
            hdr_mask->srcport = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            hdr_data->reason = reason;
            hdr_mask->reason = ~(reason | ((reason & 0x30000) ? 0x30000 : 0));
            hdr_data->reason_hi = reason_hi;
            hdr_mask->reason_hi = ~reason_hi;
            hdr_data->switch_pkt = 1;
            hdr_mask->switch_pkt = 0;
            hdr_data->word0.overlay1.bpdu = bpdu;
            hdr_mask->word0.overlay1.bpdu = ~bpdu;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            hdr_data->match_rule = filter->m_fp_rule;
            hdr_mask->match_rule = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            dcb_data->error = 1;
            dcb_mask->error = 0;
        }
    } else
#endif

    if (filter->match_flags & ~BCM_KNET_FILTER_M_RAW) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "KNET: Unsupported DCB format\n")));

        /* Avoid partial build failures */
        _higig_match_set(NULL, NULL, NULL);
        _higig2_match_set(NULL, NULL, NULL);

        return BCM_E_UNAVAIL;
    }

    /* Invert inverted mask */
    for (idx = 0; idx < oob_size; idx++) {
        filter_create.filter.mask.b[idx] ^= 0xff;
    }

    filter_create.filter.oob_data_size = oob_size;

    if (filter->match_flags & BCM_KNET_FILTER_M_RAW) {
        data_offset = 0;
        for (pdx = 0; pdx < filter->raw_size; pdx++) {
            if (filter->m_raw_mask[pdx] != 0) {
                data_offset = pdx;
                break;
            }
        }
        idx = oob_size;
        for (; pdx < filter->raw_size; pdx++) {
            /* Check for array overflow */
            if (idx >= KCOM_FILTER_BYTES_MAX) {
                return BCM_E_PARAM;
            }
            filter_create.filter.data.b[idx] = filter->m_raw_data[pdx];
            filter_create.filter.mask.b[idx] = filter->m_raw_mask[pdx];
            idx++;
        }
        filter_create.filter.pkt_data_offset = data_offset;
        filter_create.filter.pkt_data_size = filter->raw_size - data_offset;
    }

    /*
     * If no match flags are set we treat raw filter data as OOB data.
     * Note that this functionality is intended for debugging only.
     */
    if (filter->match_flags == 0) {
        for (idx = 0; idx < filter->raw_size; idx++) {
            /* Check for array overflow */
            if (idx >= KCOM_FILTER_BYTES_MAX) {
                return BCM_E_PARAM;
            }
            filter_create.filter.data.b[idx] = filter->m_raw_data[idx];
            filter_create.filter.mask.b[idx] = filter->m_raw_mask[idx];
        }
    }

    /* Dump raw data for debugging purposes */
    for (idx = 0; idx < BYTES2WORDS(oob_size); idx++) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "OOB[%d]: 0x%08x [0x%08x]\n"), idx,
                     filter_create.filter.data.w[idx],
                     filter_create.filter.mask.w[idx]));
    }
    for (idx = 0; idx < filter_create.filter.pkt_data_size; idx++) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "PKT[%d]: 0x%02x [0x%02x]\n"),
                     idx + filter_create.filter.pkt_data_offset,
                     filter_create.filter.data.b[idx + oob_size],
                     filter_create.filter.mask.b[idx + oob_size]));
    }

    rv = soc_knet_cmd_req((kcom_msg_t *)&filter_create,
                          sizeof(filter_create), sizeof(filter_create));

    if (BCM_SUCCESS(rv)) {
        /* ID is assigned by kernel */
        filter->id = filter_create.filter.id;
    }
    return rv;
#endif
}

/*
 * Function:
 *      bcm_esw_knet_filter_destroy
 * Purpose:
 *      Destroy a kernel packet filter.
 * Parameters:
 *      unit - (IN) Unit number.
 *      filter_id - (IN) Rx packet filter ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_knet_filter_destroy(int unit, int filter_id)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    kcom_msg_filter_destroy_t filter_destroy;

    sal_memset(&filter_destroy, 0, sizeof(filter_destroy));
    filter_destroy.hdr.opcode = KCOM_M_FILTER_DESTROY;
    filter_destroy.hdr.unit = unit;

    filter_destroy.hdr.id = filter_id;

    return soc_knet_cmd_req((kcom_msg_t *)&filter_destroy,
                            sizeof(filter_destroy), sizeof(filter_destroy));
#endif
}

/*
 * Function:
 *      bcm_esw_knet_filter_get
 * Purpose:
 *      Get a kernel packet filter configuration.
 * Parameters:
 *      unit - (IN) Unit number.
 *      filter_id - (IN) Rx packet filter ID
 *      filter - (OUT) Rx packet filter configuration
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_knet_filter_get(int unit, int filter_id, bcm_knet_filter_t *filter)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    kcom_msg_filter_get_t filter_get;
    dcb_t *dcb_data;
    dcb_t *dcb_mask;
    soc_rx_reasons_t no_reasons;
    soc_rx_reasons_t mask_reasons;
    int idx, rdx, fdx;

    sal_memset(&filter_get, 0, sizeof(filter_get));
    filter_get.hdr.opcode = KCOM_M_FILTER_GET;
    filter_get.hdr.unit = unit;

    filter_get.hdr.id = filter_id;

    rv = soc_knet_cmd_req((kcom_msg_t *)&filter_get,
                          sizeof(filter_get.hdr), sizeof(filter_get));

    if (BCM_SUCCESS(rv)) {
        bcm_knet_filter_t_init(filter);

        switch (filter_get.filter.type) {
        case KCOM_FILTER_T_RX_PKT:
            filter->type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        default:
            /* Unknown type */
            break;
        }

        switch (filter_get.filter.dest_type) {
        case KCOM_DEST_T_NETIF:
            filter->dest_type = BCM_KNET_DEST_T_NETIF;
            break;
        case KCOM_DEST_T_API:
            filter->dest_type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        case KCOM_DEST_T_CB:
            filter->dest_type = BCM_KNET_DEST_T_CALLBACK;
            break;
        default:
            filter->dest_type = BCM_KNET_DEST_T_NULL;
            break;
        }

        switch (filter_get.filter.mirror_type) {
        case KCOM_DEST_T_NETIF:
            filter->mirror_type = BCM_KNET_DEST_T_NETIF;
            break;
        case KCOM_DEST_T_API:
            filter->mirror_type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        case KCOM_DEST_T_CB:
            /* Should never get here, but keep for completeness */
            filter->mirror_type = BCM_KNET_DEST_T_CALLBACK;
            break;
        default:
            filter->mirror_type = BCM_KNET_DEST_T_NULL;
            break;
        }

        if (filter_get.filter.flags & KCOM_FILTER_F_STRIP_TAG) {
            filter->flags |= BCM_KNET_FILTER_F_STRIP_TAG;
        }

        filter->dest_id = filter_get.filter.dest_id;
        filter->dest_proto = filter_get.filter.dest_proto;
        filter->mirror_id = filter_get.filter.mirror_id;
        filter->mirror_proto = filter_get.filter.mirror_proto;

        filter->id = filter_get.filter.id;
        filter->priority = filter_get.filter.priority;
        sal_memcpy(filter->desc, filter_get.filter.desc,
                   sizeof(filter->desc) - 1);

        dcb_data = (dcb_t *)&filter_get.filter.data;
        dcb_mask = (dcb_t *)&filter_get.filter.mask;

        sal_memset(&no_reasons, 0, sizeof(no_reasons));
        SOC_DCB_RX_REASONS_GET(unit, dcb_mask, &mask_reasons);
        if (sal_memcmp(&mask_reasons, &no_reasons, sizeof(mask_reasons))) {
            filter->match_flags |= BCM_KNET_FILTER_M_REASON;
            SOC_DCB_RX_REASONS_GET(unit, dcb_data, &filter->m_reason);
        }
        if (SOC_DCB_RX_OUTER_VID_GET(unit, dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_VLAN;
            filter->m_vlan = SOC_DCB_RX_OUTER_VID_GET(unit, dcb_data);
        }
        if (SOC_DCB_RX_INGPORT_GET(unit, dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_INGPORT;
            filter->m_ingport = SOC_DCB_RX_INGPORT_GET(unit, dcb_data);
        }
        if (SOC_DCB_RX_SRCPORT_GET(unit, dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_SRC_MODPORT;
            filter->m_src_modport = SOC_DCB_RX_SRCPORT_GET(unit, dcb_data);
        }
        if (SOC_DCB_RX_SRCMOD_GET(unit, dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_SRC_MODID;
            filter->m_src_modid = SOC_DCB_RX_SRCMOD_GET(unit, dcb_data);
        }
        if (SOC_DCB_RX_MATCHRULE_GET(unit, dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_FP_RULE;
            filter->m_fp_rule = SOC_DCB_RX_MATCHRULE_GET(unit, dcb_data);
        }
        if (SOC_DCB_RX_ERROR_GET(unit,dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_ERROR;
        }
        if (filter_get.filter.pkt_data_size) {
            filter->match_flags |= BCM_KNET_FILTER_M_RAW;
            rdx = filter_get.filter.pkt_data_offset;
            fdx = filter_get.filter.oob_data_size;
            for (idx = 0; idx < filter_get.filter.pkt_data_size; idx++) {
                filter->m_raw_data[rdx] = filter_get.filter.data.b[fdx];
                filter->m_raw_mask[rdx] = filter_get.filter.mask.b[fdx];
                rdx++;
                fdx++;
            }
            filter->raw_size = rdx;
        } else {
            /*
             * If a filter contains no raw packet data then we copy the OOB
             * data into the raw data buffer while raw_size remains zero.
             * Note that this functionality is intended for debugging only.
             */
            for (idx = 0; idx < SOC_DCB_SIZE(unit); idx++) {
                filter->m_raw_data[idx] = filter_get.filter.data.b[idx];
                filter->m_raw_mask[idx] = filter_get.filter.mask.b[idx];
            }
        }
    }

    return rv;
#endif
}

/*
 * Function:
 *      bcm_esw_knet_filter_traverse
 * Purpose:
 *      Traverse kernel packet filter objects
 * Parameters:
 *      unit - (IN) Unit number.
 *      trav_fn - (IN) User provided call back function
 *      user_data - (IN) User provided data used as input param for callback function
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_knet_filter_traverse(int unit, bcm_knet_filter_traverse_cb trav_fn, 
                             void *user_data)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv, idx;
    bcm_knet_filter_t filter;
    kcom_msg_filter_list_t filter_list;

    if (trav_fn == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(&filter_list, 0, sizeof(filter_list));
    filter_list.hdr.opcode = KCOM_M_FILTER_LIST;
    filter_list.hdr.unit = unit;

    rv = soc_knet_cmd_req((kcom_msg_t *)&filter_list,
                          sizeof(filter_list.hdr), sizeof(filter_list));

    if (BCM_SUCCESS(rv)) {
        for (idx = 0; idx < filter_list.fcnt; idx++) {
            rv = bcm_esw_knet_filter_get(unit, filter_list.id[idx], &filter);
            if (BCM_SUCCESS(rv)) {
                rv = trav_fn(unit, &filter, user_data);
            }
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    return rv;
#endif
}
