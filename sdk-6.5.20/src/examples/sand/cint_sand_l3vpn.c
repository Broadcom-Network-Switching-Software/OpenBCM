/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *
 * The below CINT demonstrates MPLS L3VPN which show BCM886XX being PE-based L3VPN.
 * Available for 88660_A0 and above.
 * MPLS L3VPN model consists of three kind of devices:
 * 1. Customer Edge device (CE): Resides on customer network and connected with one or more interfaces to provide edge device.
 * The CE is not familiar with the existance of VPN and not with the MPLS core application.
 * 2. Provide Edge device (PE): Resides on service provider network and connects to one or more CEs.
 * PE is responsible on receiving and sending traffic from / to the MPLS network between the CE and P.
 * PE is in charge of all the VPN processing.
 * 3. Provider device (P): Backbone router on a service provider network. Only in charge of the MPLS network in the backbone and not familir with the VPN processing.
 * It may be that PE device will fucntional as P for other VPN processing of not directly connected CE to MPLS backbone.
 *
 * In the CINT below we show BCM886XX as a PE device.
 * Terminology compared between BCM SDK (and Dune Arch) to L3VPN network:
 * 1. Site (IP address or group of IPs) in PE device usually being termed as IP route address or IP host address (in case of exact match)
 * 2. VPN instance - different routes may belong to VPN instances. Each VPN instance usually maintain its own routing table.
 *  VPN instance is usually implemented by VRF object. VRF is a set by the RIF interface (L3 interface).
 * 3. VC label, VLAN - The identification of VPN instance is usually being handled by VLAN from the CE side or by VC label from the MPLS core side.
 *  VC label is usually implemented by a regular LSP label in BCM SDK. bcm_mpls_tunnel_terminator_create in charge of the termination part
 * and bcm_mpls_tunnel_initaitor_create in charge of the encapsulation part.
 * We note that there is no need to use MPLS_PORT for this functionality as we don't need to learn nor to maintain any L2VPN properties.
 *  VLAN usually may provide from CE side the VRF by:
 *  (a) A premiliary lookup of Port x VLAN which creates AC-LIF
 * (b) AC-LIF is only used for RIF mapped (and so VRF mapped).
 *  In case RIF is equal to VLAN user may drop AC-LIF creation and relay on Port default AC-LIF.
 * Port default AC-LIF is set on init and provide the capability of mapping RIF directly form VLAN.
 *  For more information on VLAN - RIF model mappings please see cint_ip_route_explicit_rif.c
 *
 * The device can function also as IP-GRE core application instead of MPLS but in cint_l3vpn we focused on MPLS core
 *
 *
 * The below BCM SDK settings try to provide L3VPN example topology and traffic examples show UC, MC packet scenarios in this network topology:
 * BCM> sand/utility/cint_sand_utils_global.c
 * BCM> sand/utility/cint_sand_utils_vlan.c
 * BCM> sand/utility/cint_sand_utils_mpls.c
 * BCM> sand/utility/cint_sand_utils_l3.c
 * BCM> sand/cint_sand_l3vpn.c
 * BCM> cint
 * cint> int inP = 201;
 * cint> int outP = 202;
 * cint> l3vpn_run_defaults_with_ports(unit, inP, outP);
 *
 *
 *
 *
 *
 * Traffic:
 *   - UC packets:
 *     - Packet Incoming from access side to mpls network: CE1->P1
 *     - Packet Incoming from MPLS network to access side.
 *     - Packet is IP routed in access side.
 *     - Packet is routed in MPLS network
 *
 *
 *   - MC packets:
 *     - Packet Incoming from MPLS network, to P2, CE1. (Bud node)
 *     - Packet Incoming from MPLS network, to PE2 (Continue)
 *     - Packet Incoming from MPLS network, to CE1, CE2 (Leaf node)
 *     - Packet Incoming from access side, to CE2, P1
 *
 *
 *  Traffic from RouterCE1 to RouterP
 *
 *  Routing to overlay: host10 to host90
 *  Purpose: - check mpls & vc encapsulations, check new ethernet header
 *
 *    Send:
 *             ----------------------------------------
 *        eth: |    DA       |     SA          | VLAN |
 *             ----------------------------------------
 *             | routerPE1_mac | routerCE1_mac |  v1  |
 *             ----------------------------------------
 *                 --------------------------
 *             ip: |   SIP     | DIP        |
 *                 --------------------------
 *                 | host10_ip |  host90_ip |
 *                 --------------------------
 *    Receive:
 *             -------------------------------------
 *        eth: |    DA       |     SA      | VLAN  |
 *             -------------------------------------
 *             | routerP_mac | routerPE1_mac |  v2 |
 *             -------------------------------------
 *        mpls:   --------------------------
 *                |   LABEL   | LABEL      |
 *                --------------------------
 *                | MPLS LABEL|  VC_LABEL  |
 *                --------------------------
 *                          --------------------------
 *                      ip: |   SIP     | DIP        |
 *                          --------------------------
 *                          | host10_ip |  host90_ip |
 *                          --------------------------
 *
 *
 *
 *
 *   Traffic from RouterP2 to RouterCE1
 *
 *   Tunnel termination, ip routing: host91 to host 11
 *   Purpose: - check mpls and vc label termination, check new ethernet header
 *
 *    Send:
 *             -------------------------------------
 *        eth: |    DA         |   SA      | VLAN  |
 *             -------------------------------------
 *             | routerPE1_mac | routerP_mac |  v2 |
 *             -------------------------------------
 *        mpls:   --------------------------
 *                |   LABEL   | LABEL      |
 *                --------------------------
 *                | MPLS LABEL|  VC_LABEL  |
 *                --------------------------
 *                          --------------------------
 *                      ip: |   SIP     | DIP        |
 *                          --------------------------
 *                          | host81_ip |  host11_ip |
 *                          --------------------------
 *
 *    Receive:
 *             ----------------------------------------
 *        eth: |    DA         |   SA          | VLAN |
 *             ----------------------------------------
 *             | routerCE1_mac | routerPE1_mac |  v1  |
 *             ----------------------------------------
 *                 --------------------------
 *             ip: |   SIP     | DIP        |
 *                 --------------------------
 *                 | host81_ip |  host11_ip |
 *                 --------------------------
 *
 *
 *
 *   Traffic from RouterCE2 to RouterCE1
 *
 *   ip routing: host21 to host 11
 *
 *    Send:
 *             ----------------------------------------
 *        eth: |    DA         |   SA          | VLAN |
 *             ----------------------------------------
 *             | routerPE1_mac | routerCE2_mac |  v1  |
 *             ----------------------------------------
 *                 --------------------------
 *             ip: |   SIP     | DIP        |
 *                 --------------------------
 *                 | host21_ip |  host11_ip |
 *                 --------------------------
 *
 *    Receive:
 *             ----------------------------------------
 *        eth: |    DA         |   SA          | VLAN |
 *             ----------------------------------------
 *             | routerCE1_mac | routerPE1_mac |  v1  |
 *             ----------------------------------------
 *                 --------------------------
 *             ip: |   SIP     | DIP        |
 *                 --------------------------
 *                 | host21_ip |  host11_ip |
 *                 --------------------------
 *
 *
 *
 *   Traffic from RouterP2 to RouterP3
 *
 *   mpls Tunnel swap
 *   Purpose: - check swap mpls, check new ethernet header
 *
 *    Send:
 *             -------------------------------------
 *        eth: |    DA         |   SA      | VLAN  |
 *             -------------------------------------
 *             | routerPE1_mac | routerPE2_mac |  v2 |
 *             -------------------------------------
 *        mpls:   --------------------------
 *                |   LABEL   | LABEL      |
 *                --------------------------
 *                | MPLS LABEL|  VC_LABEL  |
 *                --------------------------
 *                          --------------------------
 *                      ip: |   SIP     | DIP        |
 *                          --------------------------
 *                          | host81_ip |  host11_ip |
 *                          --------------------------
 */


struct cint_l3vpn_info_s {
    int ce_port; /* access port */
    int p_port; /* provider port */
    uint8 my_mac[6]; /* my mac of the router */
    uint8 next_hop_to_P1_mac[6]; /* next hop to P1 for mpls tunnel */
    uint8 next_hop_to_P2_mac[6]; /* next hop to P2 for mpls tunnel */
    uint8 ce1_mac[6]; /* next hop for routing to customer edge */
    int vlan_access_CE1; /* vlan in access network. For CE1. */
    int vlan_access_CE2; /* vlan in access network. For CE2 */
    int vlan_to_vpn_P1; /*   vlan in core network. For P1 */
    int vlan_to_vpn_P2; /* vlan in core network. For P2  */
    int vlan_to_vpn_P1_hi_fec; /* vlan in core network. For P1 for hi-fec implementation */

    uint8 enable_hierarchical_fec; /* add VC label and mpls tunnels using hierarchical fec.
                                      If disabled, only create mpls tunnels using 1 fec */

    bcm_mpls_label_t mpls_label_to_access;  /* mpls label in access direction. Label to terminate */
    bcm_mpls_label_t mpls_in_label_swap;    /* mpls label in network provider. Label to swap with mpls_eg_label_swap */
    bcm_mpls_label_t mpls_eg_label_swap;    /* mpls label in network provider. swap label mpls_in_label_swap */

    int arp_id_push;                        /* ARP ID for push entries */
    int arp_id_swap;                        /* ARP ID for swap entries */
    int arp_id_ip_route;                    /* ARP ID for IP route */
    int fec_id_tunnel;                      /* FEC ID for routing into tunnel */
    int fec_id_swap;                        /* FEC ID for swap */
    int fec_id_ip_route;                    /* FEC ID for IP route */
    int mpls_tunnel_id;

    int vrf;

};

int verbose1 = 1;
cint_l3vpn_info_s l3vpn_info;

void
l3vpn_info_init (
    int ce_port,
    int p_port,
    uint8* my_mac,
    uint8* next_hop_to_P1_mac,
    uint8* next_hop_to_P2_mac,
    uint8* ce1_mac,
    int* vlans,
    uint8 enable_hierarchical_fec,
    bcm_mpls_label_t* mpls_labels,
    int vrf)
{
    l3vpn_info.ce_port = ce_port;
    l3vpn_info.p_port = p_port;
    sal_memcpy(l3vpn_info.my_mac, my_mac, 6);
    sal_memcpy(l3vpn_info.next_hop_to_P1_mac, next_hop_to_P1_mac, 6);
    sal_memcpy(l3vpn_info.next_hop_to_P2_mac, next_hop_to_P2_mac, 6);
    sal_memcpy(l3vpn_info.ce1_mac, ce1_mac, 6);
    l3vpn_info.vlan_access_CE1       = vlans[0];
    l3vpn_info.vlan_access_CE2      = vlans[1];
    l3vpn_info.vlan_to_vpn_P1          = vlans[2];
    l3vpn_info.vlan_to_vpn_P2         = vlans[3];
    l3vpn_info.vlan_to_vpn_P1_hi_fec  = vlans[4];
    l3vpn_info.enable_hierarchical_fec = enable_hierarchical_fec;
    l3vpn_info.mpls_label_to_access    = mpls_labels[0];
    l3vpn_info.mpls_in_label_swap      = mpls_labels[1];
    l3vpn_info.mpls_eg_label_swap      = mpls_labels[2];
    l3vpn_info.vrf      = vrf;
}

int
l3vpn_l3_intf_configuration (
    int unit)
{
    int rv;
    int vrf = 0;
    sand_utils_l3_eth_rif_s eth_rif_structure;
    int ing_intf;  /* interface_id for ingress router interface */
    int egr_intf;  /* interface_id for egress router interface */

    /* 1. create ingress router instance for access packets:
     * - create a bridge instance with vsi = vlan
     * - create flooding MC group for bridge instance
     * - add port to vlan
     * - create router instance
     * - create incoming router interface and outgoing interface */
    rv = vlan__open_vlan_per_mc(unit, l3vpn_info.vlan_to_vpn_P1, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", l3vpn_info.vlan_to_vpn_P1, unit);
    }
    rv = bcm_vlan_gport_add(unit, l3vpn_info.vlan_to_vpn_P1, l3vpn_info.ce_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", l3vpn_info.ce_port, l3vpn_info.vlan_to_vpn_P1);
        return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, l3vpn_info.vlan_to_vpn_P1, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, l3vpn_info.my_mac, l3vpn_info.vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rv;
    }
    ing_intf = eth_rif_structure.l3_rif;

    /* 2. create egress router instance for access packets */
    rv = vlan__open_vlan_per_mc(unit, l3vpn_info.vlan_to_vpn_P2, 0x1);
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", l3vpn_info.vlan_to_vpn_P2, unit);
    }

    rv = bcm_vlan_gport_add(unit, l3vpn_info.vlan_to_vpn_P2, l3vpn_info.p_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", l3vpn_info.p_port, l3vpn_info.vlan_to_vpn_P2);
        return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, l3vpn_info.vlan_to_vpn_P2, 0, BCM_L3_INGRESS_GLOBAL_ROUTE, l3vpn_info.my_mac, l3vpn_info.vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create core_native_eth_rif\n");
        return rv;
    }
    egr_intf = eth_rif_structure.l3_rif;

    return rv;
}

int
l3vpn_arp_entry_configuration (
    int unit)
{
    int rv;

    sand_utils_l3_arp_s arp_structure;
    uint32 l3_flags;

    /**  Configure ARP entry for push action (CE to PE1)  **/
    l3_flags = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, 0, 0, l3_flags, l3vpn_info.next_hop_to_P1_mac, l3vpn_info.vlan_to_vpn_P2);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    l3vpn_info.arp_id_push = arp_structure.l3eg_arp_id;

    /**  Configure ARP entry for access routing (PE to CE1) or (CE2 to CE1)  **/
    l3_flags = 0;
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, 0, 0, l3_flags, l3vpn_info.ce1_mac, l3vpn_info.vlan_to_vpn_P1);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    l3vpn_info.arp_id_ip_route = arp_structure.l3eg_arp_id;

    /**  Configure ARP entry for swap action (PE to PE2)  **/
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, 0, 0, l3_flags, l3vpn_info.next_hop_to_P2_mac, l3vpn_info.vlan_to_vpn_P2);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }
    l3vpn_info.arp_id_swap = arp_structure.l3eg_arp_id;

    return rv;
}

int
l3vpn_fec_entry_configuration (
    int unit)
{
    int rv;
    sand_utils_l3_fec_s fec_structure;

    /**  Configure FEC entry: route to mpls tunnel  **/
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, l3vpn_info.p_port);
    fec_structure.tunnel_gport = l3vpn_info.mpls_tunnel_id;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    l3vpn_info.fec_id_tunnel = fec_structure.l3eg_fec_id;

    /**  Configure FEC entry: ip route  **/
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, l3vpn_info.ce_port);
    fec_structure.tunnel_gport = l3vpn_info.arp_id_ip_route;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    l3vpn_info.fec_id_ip_route = fec_structure.l3eg_fec_id;


    /**  Configure FEC entry: swap action  **/
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);
    BCM_GPORT_LOCAL_SET(fec_structure.destination, l3vpn_info.p_port);
    fec_structure.tunnel_gport = l3vpn_info.arp_id_swap;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }
    l3vpn_info.fec_id_swap = fec_structure.l3eg_fec_id;

    return rv;
}

int
l3vpn_mpls_tunnel_configuration (
    int unit)
{
    int rv;
    int label_index;
    bcm_mpls_egress_label_t label_array[2];
    int labels[2];
    int tunnel_ttl[4] = {20,40,60,64};

    /** create tunnels: to build 2 mpls headers (mpls label and VC label) **/
    labels[0] = 200; /* VC label */
    labels[1] = 400; /* mpls label */
    for (label_index = 0; label_index < 2; label_index++)
    {
        bcm_mpls_egress_label_t_init(&label_array[label_index]);

        /** Labels with the biggest member index */
        label_array[label_index].label = labels[label_index];
        label_array[label_index].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        if (is_device_or_above(unit, JERICHO2)){
            label_array[label_index].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            label_array[label_index].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        } else {
            label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
            label_array[label_index].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
        label_array[label_index].action = BCM_MPLS_EGRESS_ACTION_PUSH;
        label_array[label_index].ttl = tunnel_ttl[label_index];

        /* Only outermost entry carry arp */
        if (label_index == 1) {
            label_array[label_index].l3_intf_id = l3vpn_info.arp_id_push;
        }
    }

    /** Create the mpls tunnel with one call. **/
    rv = sand_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    l3vpn_info.mpls_tunnel_id = label_array[0].tunnel_id;

    if (verbose >= 1) {
        printf("Configured mpls tunnels %x %x\n", labels[0], labels[1]);
    }

    return rv;
}

int
l3vpn_mpls_termination_tunnel_configuration (
    int unit,
    bcm_mpls_label_t terminated_label,
    int label_index)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    bcm_if_t l3_intf_id;
    bcm_l3_ingress_t l3_ing_if;

    /** tunnel termination: terminate mpls headers **/
    bcm_mpls_tunnel_switch_t_init(&entry);

    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.label = terminated_label;
    if (soc_property_get(unit, "mpls_termination_label_index_enable", 0))
    {
        int lbl_val = terminated_label;
        BCM_MPLS_INDEXED_LABEL_SET(&entry.label, &lbl_val, label_index);
    }
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    /*
     * Set Incoming Tunnel-RIF properties
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        /* In case of J2 the RIF is the mpls tunnel id
         * In case of J1 the RIF is the tunnel interface (RIF) */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, entry.tunnel_id);

        bcm_l3_ingress_t_init(&l3_ing_if);
        l3_ing_if.flags = BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_WITH_ID;
        l3_ing_if.vrf = l3vpn_info.vrf;
        rv = bcm_l3_ingress_create(unit, l3_ing_if, l3_intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ingress_create\n");
            return rv;
        }
    }

    if (verbose >= 1) {
        printf("Configured tunnel termination for labels %x\n", terminated_label);
    }

    return rv;
}

int
l3vpn_mpls_termination_stack_configuration (
    int unit)
{
    int rv;
    int labels[2];
    int label_indexes[2];  /* Relevant only when mpls_termination_label_index_enable is set */
    int i, nof_labels;

    labels[0]        = l3vpn_info.mpls_label_to_access;  /* mpls label */
    label_indexes[0] = 1;
    labels[1]        = 200;                              /* vc label */
    label_indexes[1] = 2;
    nof_labels = 2;

    /*
     * Create a stack of MPLS labels to be terminated
     */
    for (i = 0; i < nof_labels; i++)
    {
        rv = l3vpn_mpls_termination_tunnel_configuration(unit, labels[i], label_indexes[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in l3vpn_mpls_termination_tunnel_configuration\n");
            return rv;
        }

    }

    return rv;
}

int
l3vpn_forward_entry_configuration (
    int unit)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    sand_utils_l3_route_ipv4_s route_ipv4_structure;
    int route, mask;

    /** 1. perform mpls swap **/
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    entry.label = l3vpn_info.mpls_in_label_swap , /* incoming label to swap */
    entry.egress_label.label = l3vpn_info.mpls_eg_label_swap,  /* new label */
    entry.flags |= BCM_MPLS_SWITCH_TTL_DECREMENT;
    BCM_L3_ITF_SET(entry.egress_if, BCM_L3_ITF_TYPE_FEC, l3vpn_info.fec_id_swap);
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mpls_tunnel_switch_create\n", rv);
        return rv;
    }

    /** 2. add routing entry (to provider network) **/
    route = 0x5A5A5A5A; /* 90.90.90.90 */
    mask  = 0xfffffff0;

    sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, route, mask, 0/*vrf*/, 0/*flags*/, 0, l3vpn_info.fec_id_tunnel);
    rv = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
        return rv;
    }

    /** 3. add routing entry (to customer network) **/
    route = 0x0B0B0B0B; /* 11.11.11.11 */
    mask  = 0xfffffff0;

    sand_utils_l3_route_ipv4_s_common_init(unit, 0, &route_ipv4_structure, route, mask, 0/*vrf*/, 0/*flags*/, 0, l3vpn_info.fec_id_ip_route);
    rv = sand_utils_l3_route_ipv4_add(unit, &route_ipv4_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function sand_utils_l3_route_ipv4_add, \n");
        return rv;
    }

    return rv;

}

int
l3vpn_run (
    int unit)
{
    int rv;

    if (verbose1) {
        printf("l3vpn_run create vswitch\n");
    }

    rv = l3vpn_l3_intf_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3vpn_l3_intf_configuration\n");
        return rv;
    }

    rv = l3vpn_arp_entry_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3vpn_arp_entry_configuration\n");
        return rv;
    }

    rv = l3vpn_mpls_tunnel_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3vpn_mpls_tunnel_configuration\n");
        return rv;
    }

    rv = l3vpn_fec_entry_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3vpn_fec_entry_configuration\n");
        return rv;
    }

    rv = l3vpn_forward_entry_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3vpn_forward_entry_configuration\n");
        return rv;
    }

    rv = l3vpn_mpls_termination_stack_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3vpn_mpls_termination_stack_configuration\n");
        return rv;
    }

    return rv;
}

int
l3vpn_run_defaults_with_ports (
    int unit,
    int accessPort,
    int networkPort)
{
    uint8 my_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 next_hop_to_P2_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8 ce1_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int vrf = 0;
    int vlans[5] = { 10, /* vlan in access network. For CE1. */
                     20, /* vlan in access network. For CE2 */
                     30, /* vlan in core network. For P1 */
                     40, /* vlan in core network. For P2 */
                     50  /* vlan in core network. For P1 for hi-fec */
                    };
    bcm_mpls_label_t mpls_labels[3] = { 1000, /* mpls label to terminate */
                                        100,  /* mpls label in network provider. Label to swap with mpls_eg_label_swap */
                                        101   /* mpls label in network provider. swap label mpls_in_label_swap */
                                      };

    my_mac[5] = 0x55;
    next_hop_to_P1_mac[5] = 0x12;
    next_hop_to_P2_mac[5] = 0x34;
    ce1_mac[4] = 0xce;
    ce1_mac[5] = 0x1;

    l3vpn_info_init(accessPort, /* access port  */
                    networkPort, /* provider port */
                    my_mac, /* my mac of the router */
                    next_hop_to_P1_mac, /* next hop to P1 for mpls tunnel */
                    next_hop_to_P2_mac, /* next hop to P2 for mpls tunnel */
                    ce1_mac, /* next hop for routing to customer edge CE1 */
                    vlans, /* vlan in access network. For CE1. */
                    0,  /* also create tunnels using hi-fec */
                    mpls_labels,
                    vrf   /* vrf */
                    );

    return l3vpn_run(unit);
}

int
l3vpn_run_with_defaults (
    int unit)
{
    return l3vpn_run_defaults_with_ports(unit,
                                         201, /* access port  */
                                         202  /* provider port */);
}
