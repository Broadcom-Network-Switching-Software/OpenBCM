/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        knet.c
 * Purpose:     Kernel Networking Management
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_KNET

#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/dcbformats.h>
#include <soc/knet.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/knet.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/control.h>
#include <bcm/trunk.h>
#include <bcm/port.h>
#include <soc/dpp/trunk_sw_db.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/headers.h>
#include <soc/dpp/port_sw_db.h>
#include <shared/bsl.h>

#ifdef INCLUDE_KNET

/*
 * DPP Meta data for KNET filter
 * Fields of system headers and last Word of DCB
 */
typedef struct dpp_meta_data_s
{
#ifdef LE_HOST
    uint32 trap_qualifier:16,    /* Internal.Trap-Qualifier */
           trap_id:16;           /* Internal.Trap-Code */
#else
    uint32 trap_id:16,           /* Internal.Trap-Code */
           trap_qualifier:16;    /* Internal.Trap-Qualifier */
#endif

#ifdef LE_HOST
    uint32 forward_domain:16,    /* IPvX-UC-Routed: Out-RIF, IPvX-MC-Routed: In-RIF, Bridge : VSI */
           spa:16;               /* FTMH.Source-system-port-aggregate */
#else
    uint32 spa:16,               /* FTMH.Source-system-port-aggregate */
           forward_domain:16;    /* IPvX-UC-Routed: Out-RIF, IPvX-MC-Routed: In-RIF, Bridge : VSI */
#endif

#ifdef LE_HOST
    uint32 reserve:30,
           action_type:2;        /* Internal.TM-action-type */
#else
    uint32 action_type:2,        /* Internal.TM-action-type */
           reserve:30;
#endif
/** Last DW of DCB */
#ifdef  LE_HOST
    uint32 count:16,             /* Transferred byte count */
           end:1,                /* End bit (RX) */
           start:1,              /* Start bit (RX) */
           error:1,              /* Cell Error (RX) */
           :12,                  /* Don't Care */
           done:1;               /* Descriptor Done */
#else
    uint32 done:1,               /* Descriptor Done */
           :12,                  /* Don't Care */
           error:1,              /* Cell Error (RX) */
           start:1,              /* Start bit (RX) */
           end:1,                /* End bit (RX) */
           count:16;             /* Transferred byte count */
#endif
}dpp_meta_data_t;
#endif /* INCLUDE_KNET */

/*
 * Function:
 *     bcm_petra_knet_init
 * Purpose:
 *     Initialize the kernel networking subsystem.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_petra_knet_init(int unit)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    bcm_knet_filter_t filter;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(bcm_petra_knet_cleanup(unit));

    bcm_knet_filter_t_init(&filter);
    filter.type = BCM_KNET_FILTER_T_RX_PKT;
    filter.dest_type = BCM_KNET_DEST_T_BCM_RX_API;
    filter.priority = 255;
    sal_strcpy(filter.desc, "DefaultRxAPI");
    BCMDNX_IF_ERR_EXIT(bcm_petra_knet_filter_create(unit, &filter));
exit:
    BCMDNX_FUNC_RETURN;
#endif
}

/*
 * Function:
 *      bcm_petra_knet_netif_create
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
bcm_petra_knet_netif_create(int unit, bcm_knet_netif_t *netif)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    kcom_msg_netif_create_t netif_create;
    bcm_port_interface_info_t interface_info;
    uint32 ptch_ssp = 0, tm_port = 0;
    /** Injected packets to CPU port 0 by default */
    bcm_port_t injected_local_port = 0;
    bcm_core_t ptch_ssp_core = 0, core = 0;
    _bcm_dpp_gport_info_t gport_info;
    bcm_port_t sysport_gport;
    uint8 *system_headers;
    uint32 cpu_channel;
    ARAD_PORT_HEADER_TYPE header_type_incoming, header_type_outgoing;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(netif);

    sal_memset(&netif_create, 0, sizeof(netif_create));
    netif_create.hdr.opcode = KCOM_M_NETIF_CREATE;
    netif_create.hdr.unit = unit;

    switch (netif->type)
    {
        case BCM_KNET_NETIF_T_TX_CPU_INGRESS:
            netif_create.netif.type = KCOM_NETIF_T_VLAN;
            break;
        case BCM_KNET_NETIF_T_TX_LOCAL_PORT:
            netif_create.netif.type = KCOM_NETIF_T_PORT;
            break;
        case BCM_KNET_NETIF_T_TX_META_DATA:
            if (netif->flags & BCM_KNET_NETIF_F_RCPU_ENCAP)
            {
                netif_create.netif.type = KCOM_NETIF_T_META;
                break;
            }
            else
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: flag BCM_KNET_NETIF_F_RCPU_ENCAP is mandatory for netif type BCM_KNET_NETIF_T_TX_META_DATA\n")));
            }
        default:;
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: Unsupported interface type %d\n"), netif->type));
    }

    if (netif->flags & BCM_KNET_NETIF_F_RCPU_ENCAP)
    {
        if (netif->type != BCM_KNET_NETIF_T_TX_META_DATA)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: type BCM_KNET_NETIF_T_TX_META_DATA is mandatory for netif flag BCM_KNET_NETIF_F_RCPU_ENCAP\n")));
        }
        else
        {
            netif_create.netif.flags |= KCOM_NETIF_F_RCPU_ENCAP;
        }
    }
    if (netif->flags & BCM_KNET_NETIF_F_ADD_TAG)
    {
        netif_create.netif.flags |= KCOM_NETIF_F_ADD_TAG;
    }
    if (netif->flags & BCM_KNET_NETIF_F_KEEP_RX_TAG)
    {
        netif_create.netif.flags |= KCOM_NETIF_F_KEEP_RX_TAG;
    }

    /** Verify if the VLAN value is valid */
    BCM_DPP_VLAN_CHK_ID(unit, netif->vlan);
    netif_create.netif.vlan = netif->vlan;
    netif_create.netif.port = netif->port;

    if (netif->type == BCM_KNET_NETIF_T_TX_CPU_INGRESS)
    {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, netif->port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        /** Get PP Port */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, gport_info.local_port, &ptch_ssp, &ptch_ssp_core));
        /** Get interface type */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, gport_info.local_port, &(interface_info.interface)));
        /** If Tx port is CPU port, packets are injected from the configured port */
        if (interface_info.interface == BCM_PORT_IF_CPU)
        {
            injected_local_port = gport_info.local_port;
            /** Get TM Port */
            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, injected_local_port, &tm_port, &core));
        }
        else
        {
           bcm_port_config_t port_config;
           bcm_gport_t cpu_port;
           uint8 has_cpu_port = FALSE;

           BCMDNX_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
           BCM_PBMP_ITER(port_config.cpu, cpu_port)
           {
               /** Get TM Port */
               BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, cpu_port, &tm_port, &core));
               /** Find first CPU port on the same core with configured port. */
               if (core == ptch_ssp_core)
               {
                   injected_local_port = cpu_port;
                   has_cpu_port = TRUE;
                   break;
               }
           }
           if (!has_cpu_port)
           {
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NO CPU port on the same core with %d\n"),netif->port));
           }
        }
    }
    else if (netif->type == BCM_KNET_NETIF_T_TX_LOCAL_PORT)
    {
        /** Get PP port */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, injected_local_port, &ptch_ssp, &ptch_ssp_core));
        /** Get TM Port */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, injected_local_port, &tm_port, &core));
    }

    /** Build system headers appended to start of raw packets */
    system_headers = netif_create.netif.system_headers;
    sal_memset(system_headers, 0, sizeof(netif_create.netif.system_headers));
    /** Zero system_headers_size */
    netif_create.netif.system_headers_size = 0;

    arad_port_header_type_get(unit, core, tm_port, &header_type_incoming, &header_type_outgoing);

    if ((header_type_incoming == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2) ||
        (header_type_incoming == SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP))
    {
        /** Get CPU channel which will be set to DCB.MH0 */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, injected_local_port, &cpu_channel));
        netif_create.netif.qnum = (cpu_channel & 0xff);

        if (netif->type == BCM_KNET_NETIF_T_TX_CPU_INGRESS)
        {
            /** Append PTCH */
            system_headers[0] = 0xd0;
            system_headers[1] = ptch_ssp & 0xff;
            netif_create.netif.system_headers_size += DPP_HDR_PTCH_TYPE2_LEN;
        }
        else if (netif->type == BCM_KNET_NETIF_T_TX_LOCAL_PORT)
        {
            soc_dpp_itmh_t itmh;

            /** Append PTCH: Parser-Program-Control = 0, indicate next header is ITMH */
            system_headers[0] = 0x50;
            system_headers[1] = ptch_ssp & 0xff;
            netif_create.netif.system_headers_size += DPP_HDR_PTCH_TYPE2_LEN;

            /** Get the system port from logical port as destination port */
            BCMDNX_IF_ERR_EXIT(bcm_stk_gport_sysport_get(unit, netif->port, &sysport_gport));

            /** Append ITMH */
            sal_memset(&itmh, 0, sizeof(soc_dpp_itmh_t));
            if (SOC_IS_JERICHO(unit)) {
                /* DPP_HDR_ITMH_FWD_TYPE_UNICAST_DIRECT */
                itmh.base.jer_dest_info._rsvd_0 =0;
                itmh.base.jer_dest_info.dest_info_hi = 0x1;
                itmh.base.jer_dest_info.dest_info_mi = (BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport) >> 8) & 0xff;
                itmh.base.jer_dest_info.dest_info_lo = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport_gport) & 0xff;
                sal_memcpy(&system_headers[DPP_HDR_PTCH_TYPE2_LEN], &itmh.base.raw.bytes[0], DPP_HDR_ITMH_BASE_LEN);
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: Unsupported device type\n")));
            }
            netif_create.netif.system_headers_size += DPP_HDR_ITMH_BASE_LEN;
        }
    }
    else
    {
        BCMDNX_ERR_EXIT_MSG(_SHR_E_INTERNAL, (_BSL_BCM_MSG("Injected port is not found for port %d type %d\n"), netif->port, netif->type));
    }

    sal_memcpy(netif_create.netif.macaddr, netif->mac_addr, 6);
    sal_memcpy(netif_create.netif.name, netif->name,
               sizeof(netif_create.netif.name) - 1);
    netif_create.netif.cb_user_data = netif->cb_user_data;

    BCMDNX_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *)&netif_create, sizeof(netif_create), sizeof(netif_create)));
    /* ID and interface name are assigned by kernel */
    netif->id = netif_create.netif.id;
    sal_memcpy(netif->name, netif_create.netif.name,
               sizeof(netif->name) - 1);
exit:
    BCMDNX_FUNC_RETURN;
#endif
}

/*
 * Function:
 *      bcm_petra_knet_netif_destroy
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
bcm_petra_knet_netif_destroy(int unit, int netif_id)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    kcom_msg_netif_destroy_t netif_destroy;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&netif_destroy, 0, sizeof(netif_destroy));
    netif_destroy.hdr.opcode = KCOM_M_NETIF_DESTROY;
    netif_destroy.hdr.unit = unit;

    netif_destroy.hdr.id = netif_id;

    BCMDNX_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *)&netif_destroy, sizeof(netif_destroy), sizeof(netif_destroy)));
exit:
    BCMDNX_FUNC_RETURN;
#endif
}

/*
 * Function:
 *      bcm_petra_knet_netif_get
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
bcm_petra_knet_netif_get(int unit, int netif_id, bcm_knet_netif_t *netif)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    kcom_msg_netif_get_t netif_get;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(netif);

    sal_memset(&netif_get, 0, sizeof(netif_get));
    netif_get.hdr.opcode = KCOM_M_NETIF_GET;
    netif_get.hdr.unit = unit;

    netif_get.hdr.id = netif_id;

    BCMDNX_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *)&netif_get, sizeof(netif_get.hdr), sizeof(netif_get)));

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
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("KNET: Unsupported interface type\n")));
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
    sal_memcpy(netif->mac_addr, netif_get.netif.macaddr, 6);
    sal_memcpy(netif->name, netif_get.netif.name,
               sizeof(netif->name) - 1);
    netif->cb_user_data = netif_get.netif.cb_user_data;

exit:
    BCMDNX_FUNC_RETURN;
#endif
}

/*
 * Function:
 *      bcm_petra_knet_netif_traverse
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
bcm_petra_knet_netif_traverse(int unit, bcm_knet_netif_traverse_cb trav_fn,
                            void *user_data)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int idx;
    bcm_knet_netif_t netif;
    kcom_msg_netif_list_t netif_list;
    BCMDNX_INIT_FUNC_DEFS;

    if (trav_fn == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: trav_fn is NULL\n")));
    }

    sal_memset(&netif_list, 0, sizeof(netif_list));
    netif_list.hdr.opcode = KCOM_M_NETIF_LIST;
    netif_list.hdr.unit = unit;

    BCMDNX_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *)&netif_list, sizeof(netif_list.hdr), sizeof(netif_list)));
    for (idx = 0; idx < netif_list.ifcnt; idx++) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_knet_netif_get(unit, netif_list.id[idx], &netif));
        trav_fn(unit, &netif, user_data);
    }

exit:
    BCMDNX_FUNC_RETURN;

#endif
}

/*
 * Function:
 *      bcm_petra_knet_filter_create
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
bcm_petra_knet_filter_create(int unit, bcm_knet_filter_t *filter)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int idx, pdx;
    int oob_size;
    int data_offset;
    kcom_msg_filter_create_t filter_create;
    dpp_meta_data_t *meta_data = NULL;
    dpp_meta_data_t *meta_mask = NULL;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(filter);

    sal_memset(&filter_create, 0, sizeof(filter_create));
    filter_create.hdr.opcode = KCOM_M_FILTER_CREATE;
    filter_create.hdr.unit = unit;

    filter_create.filter.type = KCOM_FILTER_T_RX_PKT;

    switch (filter->dest_type)
    {
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
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: Unsupported destination type\n")));
    }

    switch (filter->mirror_type)
    {
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
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: Cannot mirror to callback\n")));
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: Unsupported mirror type\n")));
    }

    if (filter->mirror_type && (filter->dest_type != BCM_KNET_DEST_T_NETIF))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: Mirror doesn't actually work when Filter destination is not BCM_KNET_DEST_T_NETIF\n")));
    }

    if (filter->flags & BCM_KNET_FILTER_F_STRIP_TAG)
    {
        filter_create.filter.flags |= KCOM_FILTER_F_STRIP_TAG;
    }

    filter_create.filter.dest_id = filter->dest_id;
    filter_create.filter.dest_proto = filter->dest_proto;
    filter_create.filter.mirror_id = filter->mirror_id;
    filter_create.filter.mirror_proto = filter->mirror_proto;

    if (filter->priority > 255 || filter->priority < 0)
    {
        /** Priority is of type unit8 in KNET kernel module */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: filter priority is out of range (0 ~ 255)\n")));
    }
    filter_create.filter.priority = filter->priority;
    sal_strncpy(filter_create.filter.desc, filter->desc,
                sizeof(filter_create.filter.desc) - 1);

    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODPORT)
    {
        if ((filter->match_flags & BCM_KNET_FILTER_M_SRC_MODID) != BCM_KNET_FILTER_M_SRC_MODID)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: filter to match source module id and module port should be configured together\n")));
        }
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODID)
    {
        if ((filter->match_flags & BCM_KNET_FILTER_M_SRC_MODPORT) != BCM_KNET_FILTER_M_SRC_MODPORT)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: filter to match source module id and module port should be configured together\n")));
        }
    }

    oob_size = 0;
    if (filter->match_flags & ~BCM_KNET_FILTER_M_RAW)
    {
        oob_size = sizeof(dpp_meta_data_t);
    }

    /** Create inverted mask */
    for (idx = 0; idx < oob_size; idx++)
    {
        filter_create.filter.mask.b[idx] = 0xff;
    }

    meta_data = (dpp_meta_data_t *) & filter_create.filter.data;
    meta_mask = (dpp_meta_data_t *) & filter_create.filter.mask;

    if (filter->match_flags & BCM_KNET_FILTER_M_VLAN)
    {
        BCM_DPP_VLAN_CHK_ID(unit, filter->m_vlan);
        meta_data->forward_domain = filter->m_vlan;
        meta_mask->forward_domain = 0;
    }

    /** FTMH.SSPA is set for both M_INGPORT and M_SRC_MODPORT */
    if ((filter->match_flags & BCM_KNET_FILTER_M_INGPORT)
            || ((filter->match_flags & BCM_KNET_FILTER_M_SRC_MODID) && (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODPORT)))
    {
        uint8  is_lag_not_phys = 0;
        uint32 lag_port_member_id = 0;
        uint32 lag_id = 0;
        uint32 sys_phys_port_id = 0;
        uint32 spa = 0;
        int    spa_unit = 0;

        meta_mask->spa = 0;
        if ((filter->match_flags & BCM_KNET_FILTER_M_INGPORT) && BCM_GPORT_IS_TRUNK(filter->m_ingport))
        {
            uint32 sys_lag_port_id_nof_bits;
            ARAD_PORT_LAG_MODE lag_mode;
            uint32 lag_port_member_id_mask = 0xffff;

            spa_unit = unit;
            is_lag_not_phys =1;
            lag_port_member_id = 0;
            lag_id = BCM_GPORT_TRUNK_GET(filter->m_ingport);
            sys_phys_port_id = 0;
            /* mask out lag member id */
            arad_ports_lag_mode_get_unsafe(unit, &lag_mode, &sys_lag_port_id_nof_bits);
            meta_mask->spa |= ((lag_port_member_id_mask << sys_lag_port_id_nof_bits) & (SOC_SAND_BIT(13) - SOC_SAND_BIT(sys_lag_port_id_nof_bits) + SOC_SAND_BIT(13)));
            filter_create.filter.is_src_modport= FALSE;
            filter_create.filter.spa_unit = unit;
        }
        else
        {
            bcm_gport_t gport_modport = 0;
            bcm_module_t modid = 0;
            bcm_port_t port = 0;
            bcm_gport_t gport_sysport = 0;
            bcm_trunk_t tid = BCM_TRUNK_INVALID;

            if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT)
            {
                /** Convert local port or gport to modport */
                BCMDNX_IF_ERR_EXIT(bcm_port_gport_get(unit, filter->m_ingport, &gport_modport));
                modid = BCM_GPORT_MODPORT_MODID_GET(gport_modport);
                port = BCM_GPORT_MODPORT_PORT_GET(gport_modport);
                spa_unit = unit;
                filter_create.filter.is_src_modport= FALSE;
                filter_create.filter.spa_unit = unit;
            }
            else
            {
                int my_modid = 0;
                int num_modid = 0;
                int remote_unit = 0;

                /** Get unit per modid */
                bcm_stk_modid_get(unit, &my_modid);
                bcm_stk_modid_count(unit, &num_modid);
                if ((filter->m_src_modid >= my_modid) && (filter->m_src_modid < (my_modid + num_modid)))
                {
                    /** modid on locol device */
                    spa_unit = unit;
                    filter_create.filter.is_src_modport= TRUE;
                    filter_create.filter.spa_unit = unit;
                }
                else
                {
                    while (remote_unit < BCM_CONTROL_MAX)
                    {
                        if BCM_UNIT_VALID(remote_unit)
                        {
                            bcm_stk_modid_get(remote_unit, &my_modid);
                            bcm_stk_modid_count(remote_unit, &num_modid);
                            if ((filter->m_src_modid >= my_modid) && (filter->m_src_modid < (my_modid + num_modid)))
                            {
                                /** modid on remote device */
                                spa_unit = remote_unit;
                                filter_create.filter.is_src_modport= TRUE;
                                filter_create.filter.spa_unit = remote_unit;
                                break;
                            }
                        }
                        remote_unit ++;
                    }

                    if (remote_unit == BCM_CONTROL_MAX)
                    {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: configured modid is out of range\n")));
                    }
                }

                BCM_GPORT_MODPORT_SET(gport_modport, filter->m_src_modid, filter->m_src_modport);
                modid = filter->m_src_modid;
                port = filter->m_src_modport;
            }

            /** Convert modport gport to system gport */
            BCMDNX_IF_ERR_EXIT(bcm_stk_gport_sysport_get(spa_unit, gport_modport, &gport_sysport));

            /** Get trunk id from modport, ignore NOT_FOUND */
            bcm_trunk_find(spa_unit, modid, port, &tid);
            if (tid != BCM_TRUNK_INVALID)
            {
                int index;
                int member_count = 0;
                bcm_trunk_info_t trunk_info;
                bcm_trunk_member_t trunk_port_members[256];
                int max_member_count = 256;

                is_lag_not_phys = 1;
                lag_id = (uint32)tid;
                BCMDNX_IF_ERR_EXIT(bcm_trunk_get(spa_unit, tid, &trunk_info, max_member_count, trunk_port_members, &member_count));
                for (index = 0; index < member_count; index++)
                {
                    if (gport_modport == trunk_port_members[index].gport)
                    {
                        lag_port_member_id = index;
                        break;
                    }
                }
            }
            else
            {
                is_lag_not_phys = 0;
                sys_phys_port_id = BCM_GPORT_SYSTEM_PORT_ID_GET(gport_sysport);
            }
        }
        /** Build SSPA */
        arad_ports_logical_sys_id_build(spa_unit, is_lag_not_phys, (uint32)lag_id, lag_port_member_id, sys_phys_port_id, &spa);
        meta_data->spa = (spa & 0xffff);
    }

    if (filter->match_flags & BCM_KNET_FILTER_M_REASON)
    {
        if (BCM_RX_REASON_GET(filter->m_reason, bcmRxReasonMirror))
        {
            meta_data->action_type = bcmPktDnxFtmhActionTypeInboundMirror;
            meta_mask->action_type = 0;
        }
        else if (BCM_RX_REASON_GET(filter->m_reason, bcmRxReasonSampleSource))
        {
            meta_data->action_type = bcmPktDnxFtmhActionTypeSnoop;
            meta_mask->action_type = 0;
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: Unsupported Rx reason\n")));
        }
    }

    if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE)
    {
        meta_data->trap_id = (filter->m_fp_rule >> 16) & 0xffff;
        meta_data->trap_qualifier = filter->m_fp_rule & 0xffff;
        meta_mask->trap_id = 0;
        meta_mask->trap_qualifier = 0;
        if (meta_data->trap_id)
        {
            meta_mask->trap_id = 0;
        }
        if (meta_data->trap_qualifier)
        {
            meta_mask->trap_qualifier = 0;
        }
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_ERROR)
    {
        meta_data->error = 1;
        meta_mask->error = 0;
    }

    /** Invert inverted mask */
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
        filter_create.filter.oob_data_size = SOC_DCB_SIZE(unit);
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

    filter_create.filter.cb_user_data = filter->cb_user_data;

    BCMDNX_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *)&filter_create, sizeof(filter_create), sizeof(filter_create)));
    /* ID is assigned by kernel */
    filter->id = filter_create.filter.id;
exit:
    BCMDNX_FUNC_RETURN;

#endif
}

/*
 * Function:
 *      bcm_petra_knet_filter_destroy
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
bcm_petra_knet_filter_destroy(int unit, int filter_id)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    kcom_msg_filter_destroy_t filter_destroy;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&filter_destroy, 0, sizeof(filter_destroy));
    filter_destroy.hdr.opcode = KCOM_M_FILTER_DESTROY;
    filter_destroy.hdr.unit = unit;

    filter_destroy.hdr.id = filter_id;

    BCMDNX_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *)&filter_destroy, sizeof(filter_destroy), sizeof(filter_destroy)));
exit:
    BCMDNX_FUNC_RETURN;
#endif
}

/*
 * Function:
 *      bcm_petra_knet_filter_get
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
bcm_petra_knet_filter_get(int unit, int filter_id, bcm_knet_filter_t *filter)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    kcom_msg_filter_get_t filter_get;
    dpp_meta_data_t *meta_data;
    dpp_meta_data_t *meta_mask;
    int idx, rdx, fdx;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(filter);

    sal_memset(&filter_get, 0, sizeof(filter_get));
    filter_get.hdr.opcode = KCOM_M_FILTER_GET;
    filter_get.hdr.unit = unit;

    filter_get.hdr.id = filter_id;

    BCMDNX_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *)&filter_get, sizeof(filter_get.hdr), sizeof(filter_get)));

    bcm_knet_filter_t_init(filter);
    switch (filter_get.filter.type) {
    case KCOM_FILTER_T_RX_PKT:
        filter->type = BCM_KNET_FILTER_T_RX_PKT;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("KNET: Unsupported filter type\n")));
        break;
    }

    switch (filter_get.filter.dest_type)
    {
        case KCOM_DEST_T_NULL:
            filter->dest_type = BCM_KNET_DEST_T_NULL;
            break;
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

    switch (filter_get.filter.mirror_type)
    {
        case KCOM_DEST_T_NETIF:
            filter->mirror_type = BCM_KNET_DEST_T_NETIF;
            break;
        case KCOM_DEST_T_API:
            filter->mirror_type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        case KCOM_DEST_T_CB:
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

    /** There is criterion in meta data */
    if (filter_get.filter.oob_data_size)
    {
        meta_data = (dpp_meta_data_t *)&filter_get.filter.data;
        meta_mask = (dpp_meta_data_t *)&filter_get.filter.mask;

        if (meta_mask->action_type)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_REASON;
            BCM_RX_REASON_CLEAR_ALL(filter->m_reason);
            if (meta_data->action_type == bcmPktDnxFtmhActionTypeInboundMirror)
            {
                BCM_RX_REASON_SET(filter->m_reason, bcmRxReasonMirror);
            }
            else if (meta_data->action_type == bcmPktDnxFtmhActionTypeSnoop)
            {
                BCM_RX_REASON_SET(filter->m_reason, bcmRxReasonSampleSource);
            }
            else
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("KNET: Unsupported rx reason type\n")));
            }
        }

        if (meta_mask->forward_domain)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_VLAN;
            filter->m_vlan = meta_data->forward_domain;
        }

        if (meta_mask->spa & 0xffff)
        {
            uint8 is_lag = FALSE;
            uint32 lag_id = 0, lag_member_id = 0, sys_phys_port_id = 0;
            bcm_gport_t gport_sysport = 0;
            bcm_gport_t gport_modport = 0;
            int spa_unit = 0;
            _bcm_dpp_gport_info_t gport_info;

            /** Get unit of SPA */
            if (filter_get.filter.is_src_modport)
            {
                filter->match_flags |= BCM_KNET_FILTER_M_SRC_MODID;
                filter->match_flags |= BCM_KNET_FILTER_M_SRC_MODPORT;
                spa_unit = filter_get.filter.spa_unit;
            }
            else
            {
                filter->match_flags |= BCM_KNET_FILTER_M_INGPORT;
                spa_unit = filter_get.filter.spa_unit;
            }

            /** Decode SPA */
            arad_ports_logical_sys_id_parse(spa_unit, meta_data->spa,
                                                      &is_lag, &lag_id, &lag_member_id,
                                                      &sys_phys_port_id);
            /** Get system gport */
            if (is_lag)
            {
                uint32 flags = 0;

                /** Map SPA to its corresponding sysport gport */
                BCMDNX_IF_ERR_EXIT(bcm_trunk_spa_to_system_phys_port_map_get(spa_unit, flags, meta_data->spa, &gport_sysport));

                if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT)
                {
                    /** Return trunk gport if LAG member is mask out */
                    if (meta_mask->spa != 0xffff)
                    {
                        BCM_GPORT_TRUNK_SET(filter->m_ingport, lag_id);
                    }
                    else
                    {
                        /** Convert system gport to local port */
                        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(spa_unit, gport_sysport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
                        filter->m_ingport = gport_info.local_port;
                    }
                }
                else
                {
                    /** Convert system gport to modport */
                    BCMDNX_IF_ERR_EXIT(bcm_stk_sysport_gport_get(spa_unit, gport_sysport, &gport_modport));
                    filter->m_src_modport = BCM_GPORT_MODPORT_PORT_GET(gport_modport);
                    filter->m_src_modid = BCM_GPORT_MODPORT_MODID_GET(gport_modport);

                }
            }
            else
            {
                BCM_GPORT_SYSTEM_PORT_ID_SET(gport_sysport, sys_phys_port_id);
                if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT)
                {
                    /** Convert system gport to local port */
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(spa_unit, gport_sysport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
                    filter->m_ingport = gport_info.local_port;
                }
                else
                {
                    /** Convert system gport to modport */
                    BCMDNX_IF_ERR_EXIT(bcm_stk_sysport_gport_get(spa_unit, gport_sysport, &gport_modport));
                    filter->m_src_modport = BCM_GPORT_MODPORT_PORT_GET(gport_modport);
                    filter->m_src_modid = BCM_GPORT_MODPORT_MODID_GET(gport_modport);
                }
            }
        }

        if (meta_mask->trap_id || meta_mask->trap_qualifier)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_FP_RULE;
            filter->m_fp_rule = (meta_data->trap_id << 16 | meta_data->trap_qualifier);
        }

        if (meta_mask->error)
        {
            filter->match_flags |= BCM_KNET_FILTER_M_ERROR;
        }
    }

    if (filter_get.filter.pkt_data_size)
    {
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
    }
    else
    {
        /*
         * If a filter contains no raw packet data then we copy the OOB
         * data into the raw data buffer while raw_size remains zero.
         * Note that this functionality is intended for debugging only.
         */
        for (idx = 0; idx < sizeof(dpp_meta_data_t); idx++) {
            filter->m_raw_data[idx] = filter_get.filter.data.b[idx];
            filter->m_raw_mask[idx] = filter_get.filter.mask.b[idx];
        }
    }
    filter->cb_user_data = filter_get.filter.cb_user_data;

exit:
    BCMDNX_FUNC_RETURN;

#endif
}

/*
 * Function:
 *      bcm_petra_knet_filter_traverse
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
bcm_petra_knet_filter_traverse(int unit, bcm_knet_filter_traverse_cb trav_fn,
                             void *user_data)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int idx;
    bcm_knet_filter_t filter;
    kcom_msg_filter_list_t filter_list;
    BCMDNX_INIT_FUNC_DEFS;

    if (trav_fn == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("KNET: trav_fn is NULL\n")));
    }

    sal_memset(&filter_list, 0, sizeof(filter_list));
    filter_list.hdr.opcode = KCOM_M_FILTER_LIST;
    filter_list.hdr.unit = unit;

    BCMDNX_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *)&filter_list, sizeof(filter_list.hdr), sizeof(filter_list)));
    for (idx = 0; idx < filter_list.fcnt; idx++) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_knet_filter_get(unit, filter_list.id[idx], &filter));
        trav_fn(unit, &filter, user_data);
    }

exit:
    BCMDNX_FUNC_RETURN;

#endif
}

/*
 * Function:
 *      bcm_petra_knet_cleanup
 * Purpose:
 *      Clean up the kernel networking subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_knet_cleanup(int unit)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int idx;
    kcom_msg_filter_list_t filter_list;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&filter_list, 0, sizeof(filter_list));
    filter_list.hdr.opcode = KCOM_M_FILTER_LIST;
    filter_list.hdr.unit = unit;
    BCMDNX_IF_ERR_EXIT(soc_knet_cmd_req((kcom_msg_t *) & filter_list, sizeof(filter_list.hdr), sizeof(filter_list)));
    for (idx = 0; idx < filter_list.fcnt; idx++)
    {
        BCMDNX_IF_ERR_EXIT(bcm_petra_knet_filter_destroy(unit, filter_list.id[idx]));
    }

exit:
    BCMDNX_FUNC_RETURN;
#endif
}
