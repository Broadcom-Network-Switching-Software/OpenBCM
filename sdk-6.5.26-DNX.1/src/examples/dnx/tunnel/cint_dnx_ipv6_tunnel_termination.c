/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_dnx_ipv6_tunnel_termination.c Purpose: Various examples for IPv6 Tunnels.
 */

/*
 * cint_dnx_ipv6_tunnel_termination.c
 *
 *  Created on: Nov 1, 2017
 *      Author: rs016217
 *
 *Examples of Jericho2-only tunnel termination
 *
 * Scenarios configured in this CINT:
 *
 *     1. IPv6 tunnel termination with GRE4 or GRE8 additional header for IPv4 forwarding header
 *        ( ipv6_tunnel_termination_generic(unit, provider_port, access_port, IPv6TerminationTypeMP, bcmTunnelTypeGreAnyIn6) )
 *
 *     2. IPv6 tunnel termination with GRE4 or GRE8 additional header for IPv6 forwarding header
 *        ( cint_ipv6_tunnel_info.forwarding_type = IPv6TerminationForwardTypeIPv6;
 *          ipv6_tunnel_termination_generic(unit, provider_port, access_port, IPv6TerminationTypeMP, bcmTunnelTypeGreAnyIn6) )
 *
 *     3. IPv6 tunnel termination with GRE4 or GRE8 additional header for MPLS forwarding header
 *        ( cint_ipv6_tunnel_info.forwarding_type = IPv6TerminationForwardTypeMPLS;
 *          ipv6_tunnel_termination_generic(unit, provider_port, access_port, IPv6TerminationTypeMP, bcmTunnelTypeGreAnyIn6) )
 *
 *     4. IPv6 tunnel termination with UDP and VxLAN_GPE additional headers for bridging over overlay
 *        ( cint_ipv6_tunnel_info.forwarding_type = IPv6TerminationForwardTypeBridge;
 *          ipv6_tunnel_termination_generic(unit, provider_port, access_port, IPv6TerminationTypeMP, bcmTunnelTypeVxlan6) )
 *
 *     5. IPv6 tunnel termination with GRE8 additional headers for IPv4 routing with TNI look-up
 *        ( cint_ipv6_tunnel_info.forwarding_type = IPv6TerminationForwardTypeIPv4TNI;
 *          ipv6_tunnel_termination_generic(unit, provider_port, access_port, IPv6TerminationTypeMP, bcmTunnelTypeGreAnyIn6) )
 *
 *     6. IPv6 tunnel termination with GRE8 additional headers for IPv6 routing with TNI look-up
 *        ( cint_ipv6_tunnel_info.forwarding_type = IPv6TerminationForwardTypeIPv6TNI;
 *          ipv6_tunnel_termination_generic(unit, provider_port, access_port, IPv6TerminationTypeMP, bcmTunnelTypeGreAnyIn6) )
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vxlan.c
 * cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../../../src/examples/dnx/tunnel/cint_dnx_ipv6_tunnel_termination.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int provider_port = 200;
 * int access_port = 201;
 * cint_ipv6_tunnel_info.forwarding_type  = IPv6TerminationForwardTypeIPv6;
 * int bcm_tunnel_type = bcmTunnelTypeGreAnyIn6;
 * int term_type = IPv6TerminationTypeMP;
 * rv = ipv6_tunnel_termination_generic(unit, provider_port, access_port, term_type, bcm_tunnel_type);
 * print rv;
 *
 */

enum ipv6_term_forward_type_e {
    IPv6TerminationForwardTypeIPv4 = 0,
    IPv6TerminationForwardTypeIPv6 = 1,
    IPv6TerminationForwardTypeMPLS = 2,
    IPv6TerminationForwardTypeBridge = 3,
    IPv6TerminationForwardTypeIPv4TNI = 4,
    IPv6TerminationForwardTypeIPv6TNI = 5,
    IPv6TerminationForwardTypeP2P = 6
 };

enum ipv6_term_type_e {
    IPv6TerminationTypeMP    = 0,
    IPv6TerminationTypeP2P   = 1,
    IPv6TerminationTypeP2PMP = 2
};

struct cint_ipv6_tunnel_info_s {
    int eth_rif_intf_provider;              /* in RIF */
    int eth_rif_intf_access;                /* out RIF */
    int tunnel_if;                          /* RIF, property of the tunnel termination inLif.*/
    bcm_mac_t intf_provider_mac_address;    /* mac for in RIF */
    bcm_mac_t intf_access_mac_address;      /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac;             /* mac for next hop */
    int access_vrf;                         /* router instance VRF resolved at after tunnel termination*/
    int provider_vrf;                       /* router instance VRF resolved at VTT1 */
    int default_vrf_for_unknown_vni;        /* default vrf for unknown VNI */
    int fec_id;                             /* FEC id */
    int arp_id;                             /* Id for ARP entry */
    bcm_ip_t route_ipv4_dip;                /* Route DIP */
    uint32 route_ipv4_dip_mask;             /* mask for route DIP */
    int route_mpls_label;                   /* MPLS label used for forwarding */
    int mpls_label;                         /* MPLS label Swapped */

    bcm_ip6_t route_ipv6_dip;               /* Route DIP */
    bcm_ip6_t route_ipv6_dip_mask;          /* Route DIP mask */

    bcm_ip6_t tunnel_ipv6_dip;              /* tunnel DIP */
    bcm_ip6_t tunnel_ipv6_dip_mask;         /* tunnel DIP mask*/
    bcm_ip6_t tunnel_ipv6_sip;              /* tunnel SIP */
    bcm_ip6_t tunnel_ipv6_sip_mask;         /* tunnel SIP mask */
    bcm_tunnel_type_t tunnel_type;          /* bcmTunnelType */
    int tunnel_class;                       /* the intermediate object that connects the two steps of tunnel termination */
    int tunnel_priority;                    /* the priority of tcam entry for v6-MP term object */
    int tunnel_id;                          /* tunnel id */
    int term_bud;                           /* termination flag for BUD node */

    int vxlan_network_group_id;             /* ingress and egress VLAN orientation for bridging over VxLAN IPv6 */
    int vxlan_port_id;                      /* Vxlan Port ID */
    int vpn_id;                             /* VPN ID for bridging over VxLAN IPv6 */
    int vni;                                /* VNI of the VXLAN/GENEVE for bridging over VxLAN IPv6 */
    int tni_vrf;                            /* when there is TNI look-up in the VTE */
    ipv6_term_forward_type_e forwarding_type; /* indicate how to configure the fwd */
};


cint_ipv6_tunnel_info_s cint_ipv6_tunnel_info = {
    /*
     * eth_rif_intf_provider | eth_rif_intf_access | tunnel_if
     */
    15, 100, 20,
    /*
     * intf_provider_mac_address | intf_access_mac_address | arp_next_hop_mac |
     */
    { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 }, { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 }, { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d },
    /*
     * Access VRF | Provider VRF | Default VRF
     */
    5, 1, 0,
    /*
     * fec_id
     */
    45001,
    /*
     * arp_id
     */
    0,
    /*
     * route_ipv4_dip
     */
    0xA0A1A1A2 /* 160.161.161.162 */,
    /*
     * route_ipv4_dip_mask
     */
    0xfffffff0,
    /* route_mpls_label*/
    0x5A5A,
    /* mpls_label swapped */
    0x5AD,
    /*
     * route_ipv6_dip
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x66, 0x77 },
    /*
     * route_ipv6_dip_mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * tunnel DIP
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 },
    /*
     * tunnel DIP mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * tunnel SIP
     */
    { 0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1 },
    /*
     * tunnel SIP mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * bcmTunnelType
     */
    bcmTunnelTypeGreAnyIn6,
    /*tunnel_class*/
    7,
    /* tunnel_priority */
    4,
    /* tunnel_id */
    7007,
    /* term_bud */
    0,
    /*
    * vxlan_group_id | vxlan_port_id | VPN_ID | VNI
    */
    1, 0, 15, 5000,
    /* VRF from TNI look-up */
    9,
    /*
     * forwarding type
     */
    IPv6TerminationForwardTypeIPv4
};


/*
 * Interfaces initialisation function for ip tunnel termination scenarios in Jericho 2
 * The function ip_tunnel_termination_open_route_interfaces implements the basic configuration,
 * on top of witch all tunnel terminators are built. All tunnel termination scenarios will call it.
 * Inputs:   unit          - relevant unit;
 *           provider_port - incoming port;
 *           access_port   - outgoing port;
 */
int
ipv6_tunnel_termination_open_route_interfaces(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "ipv6_tunnel_termination_open_route_interfaces";

    /*
     * Configure ingress interfaces
     */
    rv = ipv6_tunnel_termination_open_route_interfaces_provider(unit, provider_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_tunnel_termination_open_route_interfaces_provider \n", proc_name);
        return rv;
    }

    /*
     * Configure egress interfaces
     */
    rv = ipv6_tunnel_termination_open_route_interfaces_access(unit, access_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_tunnel_termination_open_route_interfaces_access \n", proc_name);
        return rv;
    }

    /*
     * Create ARP entry and set its properties
     */
    if (cint_ipv6_tunnel_info.tunnel_type != bcmTunnelTypeVxlan6Gpe &&
        cint_ipv6_tunnel_info.tunnel_type != bcmTunnelTypeVxlan6)
    {
        rv = l3__egress_only_encap__create(unit, 0, &(cint_ipv6_tunnel_info.arp_id),
                                           cint_ipv6_tunnel_info.arp_next_hop_mac,
                                           cint_ipv6_tunnel_info.eth_rif_intf_access);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_encap__create \n", proc_name);
            return rv;
        }
    }

    return rv;
}

/*
 * Configure Ingress interfaces
 * Inputs: provider_port - incomming port;
 *         *proc_name    - main function name;
 */
int
ipv6_tunnel_termination_open_route_interfaces_provider(
    int unit,
    int provider_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;
    l3_ingress_intf ingr_itf;

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, provider_port, cint_ipv6_tunnel_info.eth_rif_intf_provider);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set eth_rif_intf_provider \n", proc_name);
        return rv;
    }

    /*
     * Create routing interface for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable
     * IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create_vrf(unit,
            cint_ipv6_tunnel_info.eth_rif_intf_provider,
            cint_ipv6_tunnel_info.provider_vrf,
            cint_ipv6_tunnel_info.intf_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create_vrf, eth_rif_intf_provider\n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Configure Egress interfaces
 * Inputs: access_port - outgoing port;
 *         *proc_name  - main function name;
 */
int
ipv6_tunnel_termination_open_route_interfaces_access(
    int unit,
    int access_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set access port\n", proc_name);
        return rv;
    }
    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_ipv6_tunnel_info.eth_rif_intf_access,
                             cint_ipv6_tunnel_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_access\n", proc_name);
        return rv;
    }

    return rv;
}

int
ipv6_tunnel_termination_ilm_switch_tunnel_create(
    int unit,
    bcm_mpls_label_t label,
    int fec_id)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;

    /*
     * No flag is needed to indicate the action, since the information for the swap action is
     * included in the EEDB, pointed from the ILM.
     */

    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = label;
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    /** FEC entry pointing to port */
    BCM_GPORT_FORWARD_PORT_SET(entry.port, fec_id);
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;
}

int
ipv6_tunnel_termination_create_mpls_swap_tunnel(
    int unit,
    int *mpls_tunnel_id)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    /*
     * Swapping label.
     * The label swapped for MPLS forwarding context.
     */
    label_array[0].label = cint_ipv6_tunnel_info.mpls_label;

    /*
     * Set the next pointer of this entry to be the arp. This configuration is new compared to
     * Jericho, where the arp pointer used to be connected to the EEDB entry via
     * bcm_l3_egress_create (with egress flag indication).
     */
    label_array[0].l3_intf_id = cint_ipv6_tunnel_info.arp_id;

    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label_array[0].egress_qos_model.egress_qos =bcmQosEgressModelUniform;
    label_array[0].egress_qos_model.egress_ttl =bcmQosEgressModelUniform;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *mpls_tunnel_id = label_array[0].tunnel_id;

    return rv;
}

int ipv6_tunnel_termination_vrf_update(
    int unit,
    int vrf,
    int tunnel_id)
{
    int  rv;

    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
    bcm_l3_ingress_t ingress_in_lif;
    bcm_l3_ingress_t_init(&ingress_in_lif);
    bcm_if_t in_lif_id;

    ingress_in_lif.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */

   /*
    * For Vxlan-gpe/Geneve6 tunnels, vrf is not a tunnel in-lif property,
    * vrf is resolved by additional lookup: VNI, network domain -> FODO.
    *
    */
    if (cint_ipv6_tunnel_info.tunnel_type != bcmTunnelTypeVxlan6Gpe &&
        cint_ipv6_tunnel_info.tunnel_type != bcmTunnelTypeVxlan6 &&
        cint_ipv6_tunnel_info.tunnel_type != bcmTunnelTypeGeneve6)
    {
        ingress_in_lif.vrf = vrf;
    }

    /* Convert tunnel's GPort ID to intf ID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(in_lif_id, tunnel_id);

    rv = bcm_l3_ingress_create(unit, ingress_in_lif, in_lif_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error: ipv6_tunnel_terminator_create failed -> bcm_l3_ingress_create() failed.\n");
        return rv;
    }
    return rv;
}

/*
 * Create the tunnel terminator.
 *
 *  term_type:
 *      - IPv6TerminationTypeP2P(0),
 *      - IPv6TerminationTypeMP(1),
 *      - IPv6TerminationTypeP2PMP(2),
 *
 * tunnel_term_id - the ID of the tunnel termination in-lif.
 *
 */
int ipv6_tunnel_terminator_create(
    int unit,
    int term_type,
    int *tunnel_term_id)
{

    int rv;
    int mp_tunnel_id;
    bcm_tunnel_terminator_t tunnel_termination_obj;
    bcm_tunnel_terminator_config_action_t p2p_tunnel_terminator_config_action;
    bcm_tunnel_terminator_config_key_t p2p_tunnel_terminator_config_key;

    /* Create IPv6 tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_termination_obj);

    tunnel_termination_obj.vrf              = cint_ipv6_tunnel_info.provider_vrf;
    tunnel_termination_obj.type             = cint_ipv6_tunnel_info.tunnel_type;

    /* for vxlan-gpe/geneve, set default vrf in case no hit in VNI->FODO */
    if ((cint_ipv6_tunnel_info.tunnel_type == bcmTunnelTypeGeneve) || (cint_ipv6_tunnel_info.tunnel_type == bcmTunnelTypeGeneve6)){
        tunnel_termination_obj.default_vrf = cint_ipv6_tunnel_info.default_vrf_for_unknown_vni;
    }

    if (cint_ipv6_tunnel_info.tunnel_id != 0)
    {
        tunnel_termination_obj.flags            = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
        BCM_GPORT_TUNNEL_ID_SET(tunnel_termination_obj.tunnel_id, cint_ipv6_tunnel_info.tunnel_id);
    }

    /* term bud */
    if (cint_ipv6_tunnel_info.term_bud)
    {
        tunnel_termination_obj.flags |= BCM_TUNNEL_TERM_BUD;
    }

    if (cint_ipv6_tunnel_info.forwarding_type == IPv6TerminationForwardTypeP2P)
    {
        tunnel_termination_obj.flags |= BCM_TUNNEL_TERM_CROSS_CONNECT;
    }

    if (term_type == IPv6TerminationTypeP2P)
    {
        /*
         * create v6-P2P intermediate object and term object
         */
        tunnel_termination_obj.flags |=  BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;

        /* config_key */
        p2p_tunnel_terminator_config_key.dip6               = cint_ipv6_tunnel_info.tunnel_ipv6_dip;
        p2p_tunnel_terminator_config_key.dip6_mask          = cint_ipv6_tunnel_info.tunnel_ipv6_dip_mask;
        p2p_tunnel_terminator_config_key.type               = cint_ipv6_tunnel_info.tunnel_type;
        p2p_tunnel_terminator_config_key.vrf                = cint_ipv6_tunnel_info.provider_vrf;
        p2p_tunnel_terminator_config_key.flags              = tunnel_termination_obj.flags;

        /* config_action */
        p2p_tunnel_terminator_config_action.tunnel_class    = cint_ipv6_tunnel_info.tunnel_class;

        /* Flags parameter is not used in the function. */
        bcm_tunnel_terminator_config_add(unit, 0, p2p_tunnel_terminator_config_key, p2p_tunnel_terminator_config_action);

        /* v6-P2P term object */
        tunnel_termination_obj.sip6             = cint_ipv6_tunnel_info.tunnel_ipv6_sip;
        tunnel_termination_obj.sip6_mask        = cint_ipv6_tunnel_info.tunnel_ipv6_sip_mask;
        tunnel_termination_obj.tunnel_class     = cint_ipv6_tunnel_info.tunnel_class;

        rv = bcm_tunnel_terminator_create(unit, &tunnel_termination_obj);
        if (rv != BCM_E_NONE)
        {
            printf("Error: ipv6_tunnel_terminator_create failed -> bcm_tunnel_terminator_create() %d failed.\n", term_type);
            return rv;
        }
    }
    else
    {
        /*
         * create v6-MP term object
         * used for both IPv6TerminationTypeMP and IPv6TerminationTypeP2PMP
         */
        tunnel_termination_obj.dip6             = cint_ipv6_tunnel_info.tunnel_ipv6_dip;
        tunnel_termination_obj.dip6_mask        = cint_ipv6_tunnel_info.tunnel_ipv6_dip_mask;
        tunnel_termination_obj.priority         = cint_ipv6_tunnel_info.tunnel_priority;

        rv = bcm_tunnel_terminator_create(unit, &tunnel_termination_obj);
        if (rv != BCM_E_NONE)
        {
            printf("Error: ipv6_tunnel_terminator_create failed -> bcm_tunnel_terminator_create() %d failed.\n", term_type);
            return rv;
        }
    }

    rv = ipv6_tunnel_termination_vrf_update(unit, cint_ipv6_tunnel_info.access_vrf, tunnel_termination_obj.tunnel_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv6_tunnel_termination_vrf_update - Term Type: %d\n", term_type);
        return rv;
    }

    if (term_type == IPv6TerminationTypeP2PMP)
    {
        /*
        * create v6-P2P term object with v6-MP term-object
        */
        mp_tunnel_id = tunnel_termination_obj.tunnel_id;

        bcm_tunnel_terminator_t_init(&tunnel_termination_obj);
        tunnel_termination_obj.flags            = BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
        tunnel_termination_obj.sip6             = cint_ipv6_tunnel_info.tunnel_ipv6_sip;
        tunnel_termination_obj.sip6_mask        = cint_ipv6_tunnel_info.tunnel_ipv6_sip_mask;
        tunnel_termination_obj.vrf              = cint_ipv6_tunnel_info.provider_vrf;
        tunnel_termination_obj.type             = cint_ipv6_tunnel_info.tunnel_type;

        tunnel_termination_obj.tunnel_class     = mp_tunnel_id;

        rv = bcm_tunnel_terminator_create(unit, &tunnel_termination_obj);
        if (rv != BCM_E_NONE)
        {
            printf("Error: ipv6_tunnel_terminator_create failed -> bcm_tunnel_terminator_create() %s failed.\n", term_type);
            return rv;
        }

        rv = ipv6_tunnel_termination_vrf_update(unit, cint_ipv6_tunnel_info.access_vrf, tunnel_termination_obj.tunnel_id);
        if (rv != BCM_E_NONE) {
            printf("Error, ipv6_tunnel_termination_vrf_update - Term Type: %s\n", term_type);
            return rv;
        }
    }

    *tunnel_term_id = tunnel_termination_obj.tunnel_id;

    return rv;
}

/*
 * Main function for IPv6 tunnel termination.
 *  provider_port    - the source port of entry
 *  access port      - destination port for the traffic
 *  term_type        - MP or P2P, or P2P with MP object
 *  bcm_tunnel_type  - the type of the tunnel, passed to ipv6_tunnel_terminator_create.
 */
int ipv6_tunnel_termination_generic(
    int unit,
    int provider_port,
    int access_port,
    int term_type,
    int bcm_tunnel_type)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    int mpls_tunnel_id;
    int term_tunnel_id;

    proc_name = "ipv6_tunnel_termination_generic";

    /** Select the bcmTunnelTypes and create the tunnel terminator */
    cint_ipv6_tunnel_info.tunnel_type = bcm_tunnel_type;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_ipv6_tunnel_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    if (cint_ipv6_tunnel_info.forwarding_type == IPv6TerminationForwardTypeBridge)
    {
        /*
         * Build L2 VPN
         */
        rv = dnx_utils_vxlan_open_vpn(unit, cint_ipv6_tunnel_info.vpn_id, 0, cint_ipv6_tunnel_info.vni);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, dnx_utils_vxlan_open_vpn, vpn=%d, \n", proc_name, cint_ipv6_tunnel_info.vpn_id);
            return rv;
        }
    }

    rv = ipv6_tunnel_termination_open_route_interfaces(unit, provider_port, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): ERROR : ipv6_tunnel_termination_open_route_interfaces failed to execute! \n", proc_name);
        return rv;
    }

    /** Create the tunnel termination lookup and the tunnel termination inlif */
    rv = ipv6_tunnel_terminator_create(unit, term_type, &term_tunnel_id);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): ERROR : ipv6_tunnel_terminator_create failed to execute! \n", proc_name);
        return rv;
    }

    int flags2;
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    /* Check if the method is invoked for IPv4 or IPv6 forwarding header*/
    if(cint_ipv6_tunnel_info.forwarding_type == IPv6TerminationForwardTypeIPv4 ||
        cint_ipv6_tunnel_info.forwarding_type == IPv6TerminationForwardTypeIPv6)
    {
        /*
         * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
         */

        rv = l3__egress_only_fec__create(unit,
                cint_ipv6_tunnel_info.fec_id,
                cint_ipv6_tunnel_info.eth_rif_intf_access,
                cint_ipv6_tunnel_info.arp_id,
                access_port,
                0, flags2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error: create egress object FEC only\n", proc_name);
            return rv;
        }

        if(cint_ipv6_tunnel_info.forwarding_type == IPv6TerminationForwardTypeIPv4)
        {
            /*
             * Add Route of IPv4 header entry
             */
            rv = add_route_ipv4(unit, cint_ipv6_tunnel_info.route_ipv4_dip,
                    cint_ipv6_tunnel_info.route_ipv4_dip_mask,
                    cint_ipv6_tunnel_info.access_vrf,
                    cint_ipv6_tunnel_info.fec_id);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error: add_route_ipv4() failed.\n", proc_name);
                return rv;
            }
        }
        else
        {
            /*
             * Add Route of IPv6 header entry
             */
            rv = add_route_ipv6(unit, cint_ipv6_tunnel_info.route_ipv6_dip,
                    cint_ipv6_tunnel_info.route_ipv6_dip_mask,
                    cint_ipv6_tunnel_info.access_vrf,
                    cint_ipv6_tunnel_info.fec_id);
            if(rv != BCM_E_NONE){
                printf("%s(): Error: add_route_ipv6() failed.\n", proc_name);
                return rv;
            }
        }
    }
    /*
     * MPLS forwarding
     */
    else if (cint_ipv6_tunnel_info.forwarding_type == IPv6TerminationForwardTypeMPLS)
    {

        /*
         * Add MPLS route to IP-Tunnel.
         */

        rv = ipv6_tunnel_termination_create_mpls_swap_tunnel(unit, &mpls_tunnel_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, ipv6_tunnel_termination_create_mpls_swap_tunnel\n", proc_name);
            return rv;
        }

        /*
         * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
         */
        rv = l3__egress_only_fec__create(unit, cint_ipv6_tunnel_info.fec_id, mpls_tunnel_id, 0, access_port, 0, flags2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
            return rv;
        }
        rv = ipv6_tunnel_termination_ilm_switch_tunnel_create(unit, cint_ipv6_tunnel_info.route_mpls_label,
                cint_ipv6_tunnel_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in ipv6_tunnel_termination_ilm_switch_tunnel_create \n");
            return rv;
        }
    }

    /*
    * Bridging over overlay
    */
    else if (cint_ipv6_tunnel_info.forwarding_type == IPv6TerminationForwardTypeBridge)
    {

       bcm_if_t encap_tunnel_intf;
       bcm_gport_t vxlan_port_id;

       proc_name = "vxlan_tunnel_termination_tcam";

       /*
        * Create ARP entry and set its properties
        */
       rv = l3__egress_only_encap__create(unit, 0, &(cint_ipv6_tunnel_info.arp_id),
                                          cint_ipv6_tunnel_info.intf_access_mac_address,
                                          cint_ipv6_tunnel_info.eth_rif_intf_access);
       if (rv != BCM_E_NONE)
       {
           printf("%s(): Error, create egress object ARP only\n", proc_name);
           return rv;
       }

       /*
        * Create FEC and configure its: Outlif (ARP), Destination port
        */
       rv = l3__egress_only_fec__create(unit, cint_ipv6_tunnel_info.fec_id, encap_tunnel_intf,
                                        0, access_port, 0, flags2);
       if (rv != BCM_E_NONE)
       {
           printf("%s(): Error, create egress object FEC only\n", proc_name);
           return rv;
       }

       /*
        * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
        */

       rv = vxlan_port_add(unit, provider_port, term_tunnel_id, &vxlan_port_id);
       if (rv != BCM_E_NONE)
       {
           printf("%s(): Error, vxlan_port_add \n", proc_name);
           return rv;
       }
       cint_ipv6_tunnel_info.vxlan_port_id = vxlan_port_id;

       /*
        * Create an MACT entry for VXLAN tunnel, mapped to the VPN
        */
       rv = mact_entry_add(unit, access_port);
       if (rv != BCM_E_NONE)
       {
           printf("%s(): Error, mact_entry_add\n", proc_name);
           return rv;
       }

       /*
        * IPv4 forwarding with TNI look-up
        */
    }
    else if (cint_ipv6_tunnel_info.forwarding_type == IPv6TerminationForwardTypeIPv4TNI)
    {

        /*
         * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
         */
        rv = l3__egress_only_fec__create(unit,
                cint_ipv6_tunnel_info.fec_id,
                cint_ipv6_tunnel_info.eth_rif_intf_access,
                cint_ipv6_tunnel_info.arp_id,
                access_port,
                0, flags2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error: create egress object FEC only\n", proc_name);
            return rv;
        }

        /*
         * Add Route of IPv4 header entry
         */
        rv = add_route_ipv4(unit, cint_ipv6_tunnel_info.route_ipv4_dip,
                cint_ipv6_tunnel_info.route_ipv4_dip_mask,
                cint_ipv6_tunnel_info.tni_vrf,
                cint_ipv6_tunnel_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error: add_route_ipv4() failed.\n", proc_name);
            return rv;
        }

    }
    else if (cint_ipv6_tunnel_info.forwarding_type == IPv6TerminationForwardTypeP2P)
    {
        /*
         * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
         */

        rv = l3__egress_only_fec__create(unit,
                cint_ipv6_tunnel_info.fec_id,
                cint_ipv6_tunnel_info.eth_rif_intf_access,
                cint_ipv6_tunnel_info.arp_id,
                access_port,
                0, flags2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error: create egress object FEC only\n", proc_name);
            return rv;
        }
        bcm_vswitch_cross_connect_t cross_connect;
        bcm_vswitch_cross_connect_t_init(&cross_connect);
        cross_connect.port1 = term_tunnel_id;
        BCM_GPORT_FORWARD_PORT_SET(cross_connect.port2, cint_ipv6_tunnel_info.fec_id);
        cross_connect.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        rv = bcm_vswitch_cross_connect_add(unit, &cross_connect);
        if(rv != BCM_E_NONE)
        {
            printf("Error bcm_vswitch_cross_connect_add. rv = %d \n", rv);
            return rv;
        }
    }

    return rv;
}

/*
 * Configure the bridging
 */
int
mact_entry_add(
    int unit,
    int vxlan_port_id)
{
    int rv;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr,  cint_ipv6_tunnel_info.arp_next_hop_mac, cint_ipv6_tunnel_info.vpn_id);
    l2addr.port = vxlan_port_id;
    l2addr.mac = cint_ipv6_tunnel_info.arp_next_hop_mac;
    l2addr.vid = cint_ipv6_tunnel_info.vpn_id;
    l2addr.l2mc_group = cint_ipv6_tunnel_info.vpn_id;
    l2addr.flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_l2_addr_add \n", proc_name);
        return rv;
    }
    return rv;
}

/*
 * Connext the VxLAN port to the termination LIF
 */
int
vxlan_port_add(
    int unit,
    int core_port,
    int tunnel_term_id,
    bcm_gport_t * vxlan_port_id)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    dnx_utils_vxlan_port_add_s vxlan_port_add;
    dnx_utils_vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn = cint_ipv6_tunnel_info.vpn_id;
    vxlan_port_add.in_port = core_port;
    vxlan_port_add.in_tunnel = tunnel_term_id;
    BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_ipv6_tunnel_info.fec_id);
    vxlan_port_add.flags = 0;
    vxlan_port_add.network_group_id = cint_ipv6_tunnel_info.vxlan_network_group_id;
    vxlan_port_add.add_to_mc_group = 0;

    rv = dnx_utils_vxlan_port_add(unit, vxlan_port_add);
    *vxlan_port_id = vxlan_port_add.vxlan_port_id;
    if (rv != BCM_E_NONE)
    {
        printf("Error, dnx_utils_vxlan_port_add\n");
    }
    return rv;
}

int ip_tunnel_term_dont_terminate_route_by_tunnel_dip(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;

    char *proc_name;
    proc_name = "ip_tunnel_term_dont_terminate_route_by_tunnel_dip";

    cint_ipv6_tunnel_info.forwarding_type = IPv6TerminationForwardTypeIPv6;
    rv = ipv6_tunnel_termination_generic(unit, provider_port, access_port, 0, bcmTunnelTypeIpAnyIn6);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_term_basic\n", proc_name);
        return rv;
    }

    rv = add_route_ipv6(unit, cint_ipv6_tunnel_info.tunnel_ipv6_dip,
                    cint_ipv6_tunnel_info.tunnel_ipv6_dip_mask,
                    cint_ipv6_tunnel_info.access_vrf,
                    cint_ipv6_tunnel_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: add_route_ipv6() failed.\n", proc_name);
        return rv;
    }

    rv = add_route_ipv6(unit, cint_ipv6_tunnel_info.tunnel_ipv6_dip,
                cint_ipv6_tunnel_info.tunnel_ipv6_dip_mask,
                cint_ipv6_tunnel_info.provider_vrf,
                cint_ipv6_tunnel_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error: add_route_ipv6() failed.\n", proc_name);
        return rv;
    }

    return rv;
}


/*
 * Tunnel example to create P2P termination object with MP defined object
 */

int ipv6_tunnel_term_same_dip_vrf(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;

    char *proc_name;
    proc_name = "ipv6_tunnel_term_same_dip_vrf";

    cint_ipv6_tunnel_info.route_ipv4_dip        = 0x7FFFFF03;
    cint_ipv6_tunnel_info.route_ipv4_dip_mask   = 0xFFFFFF00;
    rv = ipv6_tunnel_termination_generic(unit, provider_port, access_port,
                                        IPv6TerminationTypeP2PMP,
                                        bcmTunnelTypeIpAnyIn6);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_tunnel_termination_generic\n",proc_name);
        return rv;
    }
    return rv;
}


/*
 * Tunnel example to create 2 tunnels with different ip but same in_vrf
 * the 1st tunnel is IpAnyIn6
 * the 2nd tunnel is GreAnyIn6
 */
int ipv6_tunnel_term_ipany_and_greany(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;
    int term_tunnel_id;
    char *proc_name;
    proc_name = "ipv6_tunnel_term_ipany_and_greany";

    cint_ipv6_tunnel_info.route_ipv4_dip        = 0x7FFFFF03;
    cint_ipv6_tunnel_info.route_ipv4_dip_mask   = 0xFFFFFF00;

    /* create  without WITH_ID flags */
    cint_ipv6_tunnel_info.tunnel_id = 0;
    rv = ipv6_tunnel_termination_generic(unit, provider_port, access_port,
                                        IPv6TerminationTypeMP,
                                        bcmTunnelTypeIpAnyIn6);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_tunnel_termination_generic\n",proc_name);
        return rv;
    }

    /* Create the 2nd tunnel - GreAnyIn6 */
    bcm_ip6_t sip6 = {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x03, 0x00, 0x77, 0xff, 0xee, 0xed, 0xec, 0xeb};
    bcm_ip6_t dip6 = {0x20, 0x00, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9,0xe8, 0xe7, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1};
    sal_memcpy(&(cint_ipv6_tunnel_info.tunnel_ipv6_dip),&dip6,16);
    sal_memcpy(&(cint_ipv6_tunnel_info.tunnel_ipv6_sip),&sip6,16);

    cint_ipv6_tunnel_info.tunnel_type = bcmTunnelTypeGreAnyIn6;
    cint_ipv6_tunnel_info.tunnel_id = 0;
    /** Create the tunnel termination lookup and the tunnel termination inlif */
    rv = ipv6_tunnel_terminator_create(unit, IPv6TerminationTypeMP, &term_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_tunnel_terminator_create\n",proc_name);
        return rv;
    }

    /*
     * Add Route of IPv6 header entry
     */
    rv = add_route_ipv6(unit, cint_ipv6_tunnel_info.route_ipv6_dip,
                            cint_ipv6_tunnel_info.route_ipv6_dip_mask,
                            cint_ipv6_tunnel_info.access_vrf,
                            cint_ipv6_tunnel_info.fec_id);
    if(rv != BCM_E_NONE){
        printf("%s(): Error: add_route_ipv6() failed.\n", proc_name);
        return rv;
    }

    return rv;

}

/*
 * Tunnel example to create 2 IpAnyIn6 tunnels with different ip but same in_vrf
 */
int ipv6_tunnel_term_ipany_and_ipany(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;
    int term_tunnel_id;
    char *proc_name;
    proc_name = "ipv6_tunnel_term_ipany_and_ipany";

    cint_ipv6_tunnel_info.route_ipv4_dip        = 0x7FFFFF03;
    cint_ipv6_tunnel_info.route_ipv4_dip_mask   = 0xFFFFFF00;

    /* create  without WITH_ID flags */
    cint_ipv6_tunnel_info.tunnel_id = 0;
    rv = ipv6_tunnel_termination_generic(unit, provider_port, access_port,
                                        IPv6TerminationTypeMP,
                                        bcmTunnelTypeIpAnyIn6);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_tunnel_termination_generic\n",proc_name);
        return rv;
    }

    /* Create the 2nd tunnel - GreAnyIn6 */
    bcm_ip6_t sip6 = {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x03, 0x00, 0x77, 0xff, 0xee, 0xed, 0xec, 0xeb};
    bcm_ip6_t dip6 = {0x20, 0x00, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9,0xe8, 0xe7, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1};
    sal_memcpy(&(cint_ipv6_tunnel_info.tunnel_ipv6_dip),&dip6,16);
    sal_memcpy(&(cint_ipv6_tunnel_info.tunnel_ipv6_sip),&sip6,16);

    cint_ipv6_tunnel_info.tunnel_type = bcmTunnelTypeIpAnyIn6;
    cint_ipv6_tunnel_info.tunnel_id = 0;
    /** Create the tunnel termination lookup and the tunnel termination inlif */
    rv = ipv6_tunnel_terminator_create(unit, IPv6TerminationTypeMP, &term_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_tunnel_terminator_create\n",proc_name);
        return rv;
    }

    /*
     * Add Route of IPv6 header entry
     */
    rv = add_route_ipv6(unit, cint_ipv6_tunnel_info.route_ipv6_dip,
                            cint_ipv6_tunnel_info.route_ipv6_dip_mask,
                            cint_ipv6_tunnel_info.access_vrf,
                            cint_ipv6_tunnel_info.fec_id);
    if(rv != BCM_E_NONE){
        printf("%s(): Error: add_route_ipv6() failed.\n", proc_name);
        return rv;
    }

    return rv;

}

/*
 * Tunnel example to create tunnels with both IPv4 and IPv6 forwarding
 *  bcm_tunnel_type -   could be IpAnyIn6 or GreAnyIn6
 *  term_type       -   could be any of following:
 *                      IPv6TerminationTypeMP/IPv6TerminationTypeP2P/IPv6TerminationTypeP2PMP
 *  is_subnet       -   could be:
 *                      0; tunnel dip is fully masked
 *                      1; tunnel dip is partially masked
 */
int ipv6_tunnel_term_with_ipvx_fwd(
    int unit,
    int provider_port,
    int access_port,
    int bcm_tunnel_type,
    int term_type,
    int is_subnet)
{
    int rv = BCM_E_NONE;
    int term_tunnel_id;
    char *proc_name;
    proc_name = "ipv6_tunnel_term_with_ipvx_fwd";

    cint_ipv6_tunnel_info.route_ipv4_dip        = 0x7FFFFF03;
    cint_ipv6_tunnel_info.route_ipv4_dip_mask   = 0xFFFFFF00;

    /* create without WITH_ID flags */
    cint_ipv6_tunnel_info.tunnel_id = 0;

    if (is_subnet)
    {
        bcm_ip6_t mask6_subnet = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00};
        sal_memcpy(&(cint_ipv6_tunnel_info.tunnel_ipv6_dip_mask),&mask6_subnet,16);
    }
    rv = ipv6_tunnel_termination_generic(unit, provider_port, access_port, term_type, bcm_tunnel_type);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_tunnel_termination_generic\n",proc_name);
        return rv;
    }

    /*
     * Add Route of IPv6 header entry
     */
    rv = add_route_ipv6(unit, cint_ipv6_tunnel_info.route_ipv6_dip,
                            cint_ipv6_tunnel_info.route_ipv6_dip_mask,
                            cint_ipv6_tunnel_info.access_vrf,
                            cint_ipv6_tunnel_info.fec_id);
    if (rv != BCM_E_NONE){
        printf("%s(): Error: add_route_ipv6() failed.\n", proc_name);
        return rv;
    }
    return rv;
}

int
vxlan_network_domain_management_l3_ingress(
    int unit, uint32 vni, int vrf)
{
    int rv;
    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VNI->VRF
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L3_MAPPING;
    config.vrf              = vrf;
    config.vni              = vni;
    config.network_domain   = 0;

    rv = bcm_vxlan_network_domain_config_add(unit, &config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_network_domain_config_add \n");
        return rv;
    }
    return rv;
}


int geneve6_tunnel_term_with_ipvx_fwd(
    int unit,
    int provider_port,
    int access_port,
    int term_type,
    int is_subnet)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "geneve6_tunnel_term_with_ipvx_fwd";

    rv = ipv6_tunnel_term_with_ipvx_fwd(unit, provider_port, access_port, bcmTunnelTypeGeneve6, term_type, is_subnet);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_tunnel_termination_generic\n",proc_name);
        return rv;
    }

    rv = vxlan_network_domain_management_l3_ingress(unit, cint_ipv6_tunnel_info.vni, cint_ipv6_tunnel_info.access_vrf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_network_domain_management_l3_ingress\n", proc_name);
        return rv;
    }

    return rv;
}


int geneve6_tunnel_term_with_mpls_fwd(
    int unit,
    int provider_port,
    int access_port,
    int term_type,
    int is_subnet)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "geneve6_tunnel_term_with_mpls_fwd";

    cint_ipv6_tunnel_info.forwarding_type   = IPv6TerminationForwardTypeMPLS;

    /* create without WITH_ID flags */
    cint_ipv6_tunnel_info.tunnel_id = 0;

    if (is_subnet)
    {
        bcm_ip6_t mask6_subnet = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00};
        sal_memcpy(&(cint_ipv6_tunnel_info.tunnel_ipv6_dip_mask),&mask6_subnet,16);
    }
    rv = ipv6_tunnel_termination_generic(unit, provider_port, access_port, term_type, bcmTunnelTypeGeneve6);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ipv6_tunnel_termination_generic\n",proc_name);
        return rv;
    }


    rv = vxlan_network_domain_management_l3_ingress(unit, cint_ipv6_tunnel_info.vni, cint_ipv6_tunnel_info.access_vrf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_network_domain_management_l3_ingress\n", proc_name);
        return rv;
    }
    return rv;
}

