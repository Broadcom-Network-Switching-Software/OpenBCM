
/*
 * Test Scenario: flooding, UC packets
 *
 * Example of basic VXLAN configuration
 *
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_multicast.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_l3.c
 * cint ../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
 * cint ../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../src/examples/dnx/vxlan/cint_dnx_vxlan_basic.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int access_port = 200;
 * int provider_port = 203;
 * int vdc_enable = 0;
 * rv = vxlan_example(unit, access_port, provider_port, vdc_enable);
 * print rv;
 *
 * Traffic:
 * vxlan- unknown destination- packet from vxlan tunnel- flood
 * Purpose: - check VXLAN tunnel termination
 *          - check split horizon filter
 *          - learn native SA
 * Packet flow:
 * - tunnel is terminated
 * - get VSI from VNI
 * - learn reverse FEC for native SA
 * - packet is bridged: forwarded according to DA and VSI
 * - no hit, do flooding
 * - get flooding MC group from VSI
 * - for MC group entry to access: AC outlif and port
 * - for MC group entry to core: tunnel outlif and port
 * -                             entry is filtered at split horizon filter at egress
 *
 * tx 1 psrc=201 data=0x000c0002000000000700010081000014080045000057000000008011d955a00f1011a10f1011555512b5004300000800000000138800000000000100000000000581810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20

 * Received packets on unit 0 should be:
 * 0x000000000100000000000581810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 *  vxlan- unknown destination- packet from user - flood
 *  Purpose: - check VXLAN tunnel encapsulation
 * Packet flow:
 * - get VSI from port default VSI
 * - learn AC for SA
 * - packet is bridged: forwarded according to DA and VSI
 * - no hit, do flooding
 * - get flooding MC group from VSI
 * - for MC group entry to access: AC outlif and port
 * - for MC group entry to core: tunnel outlif and port
 *   - vxlan tunnel is built using IP tunnel eedb entry.
 *   - vxlan header.vni is resovled from VSI -> vni
 *   - ethernet header is built using LL eedb entry (IP tunnel eedb entry point to LL eedb entry):
 *   - DA from LL eedb entry, SA from LL eedb entry.VSI.myMac
 *   - VLAN = entry.VSI
 *
 * tx 1 psrc=200 data=0000000000ab000007000123810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * packet1:
 *  0x0000000000ab000007000123810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * packet2:
 *  0x00000000cd1d000c0002000081000064080045281801000000003c110584a00f1011a10f10115000555517ed000008000000001388000000000000ab000007000123810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * vxlan- known learned destination- packet from user -
 * Purpose: - send packet using learned tunnel from previous flow
 * Packet flow:
 * - get VSI from port default VSI
 * - packet is bridged: forwarded according to DA and VSI
 * - hit, result is FEC: tunnel outlif + port
 *   - vxlan tunnel is built using IP tunnel eedb entry.
 *   - vxlan header.vni is resovled from VSI -> vni
 *   - ethernet header is built using LL eedb entry (IP tunnel eedb entry point to LL eedb entry):
 *   - DA from LL eedb entry, SA from LL eedb entry.VSI.myMac
 *   - VLAN = entry.VSI
 *
 * tx 1 psrc=200 data=0x000000000581000012345678810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * 0x00000000cd1d000c0002000081000064080045280bfe000000003c111187a00f1011a10f1011500055550bea00000800000000138800000000000581000012345678810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * vxlan- known learned destination- packet from vxlan tunnel- flood
 * Purpose: - send packet using learned AC outlif from previous flow
 *
 * tx 1 psrc=201 data=0x000c0002000000000700010081000014080045000057000000008011d955a00f1011a10f1011555512b50043000008000000001388000000000000ab000007000123810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * 0x0000000000ab000007000123810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

int is_native_eve; /* If setting native VLAN editing */

/*
 * Define learn mode
 *   0. Learn FEC only, tunnel point to ARP
 *   1. Learn outlif + port, tunnel has to be symmetric
 *   2. Learn outlif + FEC, tunnel is symmetric, ARP is brought out by FEC
 *   3. Learn outlif + FEC, tunnel is asymmetric, ARP is brought out by FEC
 */
enum cint_sand_vxlan_learn_mode_t {
    LEARN_FEC_ONLY = 0,
    LEARN_SYM_OUTLIF_PORT = 1,
    LEARN_SYM_OUTLIF_FEC = 2,
    LEARN_ASYM_OUTLIF_FEC = 3
};

cint_sand_vxlan_learn_mode_t learn_mode;

/*
 * VXLAN global structure
 */
struct cint_vxlan_basic_info_s
{
    bcm_vlan_t access_vid;                  /* access vid for AC */
    int eth_rif_intf_access;                /* in RIF */
    int eth_rif_intf_provider;              /* out RIF */
    bcm_mac_t intf_access_mac_address;      /* mymac for access in RIF */
    bcm_mac_t intf_provider_mac_address;    /* mymac for provider in RIF */
    bcm_mac_t tunnel_next_hop_mac;          /* mac for next hop */
    int access_vrf;                         /* VRF, resolved as Tunnel-InLif property */
    int provider_vrf;                       /* VRF after termination, resolved as Tunn-InLif property */
    int encap_fec_id;                       /* FEC id */
    int tunnel_arp_id;                      /* Id for ARP entry */
    bcm_vlan_t provider_vlan;               /* VID assigned to the outgoing packet */
    bcm_ip_t tunnel_dip;                    /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;               /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                    /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;               /* tunnel SIP mask */
    bcm_ip6_t tunnel_dip6;                  /* tunnel DIP6 */
    bcm_ip6_t tunnel_dip6_mask;             /* tunnel DIP6 mask */
    bcm_ip6_t tunnel_sip6;                  /* tunnel SIP6 */
    bcm_ip6_t tunnel_sip6_mask;             /* tunnel SIP6 mask*/
    bcm_tunnel_type_t tunnel_type;          /* tunnel type */
    int is_ipv6_vxlan;                      /* if it's VXLAN6 tunnel */
    int tunnel_ttl;                         /* Ip tunnel header TTL */
    int tunnel_dscp;                        /* Ip tunnel header differentiated services code point */
    int encap_tunnel_id;                    /* Encapsulation tunnel id */
    int term_tunnel_id;                     /* Termination tunnel id */
    bcm_gport_t vxlan_port_id;              /* VXLAN port id */
    bcm_gport_t vlan_port_id;               /* VLAN port id */
    int vpn_id;                             /* VPN ID */
    int default_vpn;                        /* default VPN ID for ingress termination */
    int vni;                                /* VNI of the VXLAN */
    int vxlan_network_group_id;             /* ingress and egress orientation for VXLAN */
    int vxlan_vdc_enable;                   /* Option to enable/disable VXLAN VDC support */
    int vxlan_vdc;                          /* VXLAN VDC value */
    bcm_mac_t access_eth_fwd_mac;           /* mac for bridge fwd */
    bcm_mac_t provider_eth_fwd_mac;         /* mac for bridge fwd */
    bcm_gport_t egress_native_virtual_ac;   /* egress native vlan port. Represent the AC info in ESEM entry */
};

cint_vxlan_basic_info_s cint_vxlan_basic_info = {
    /*
     * access_vid
     */
    510,
    /*
     * eth_rif_intf_access | eth_rif_intf_provider
     */
    5, 10,
    /*
     * intf_access_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /*
     * intf_provider_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01},
    /*
     * tunnel_next_hop_mac
     */
    {0x00, 0x02, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * access_vrf, provider_vrf
     */
    5, 1,
    /*
     * encap_fec_id
     */
    0,
    /*
     * tunnel_arp_id
     */
    0,
    /*
     * provider_vlan
     */
    10,
    /*
     * tunnel_dip
     */
    0xAB000001, /* 171.0.0.1 */
    /*
     * tunnel_dip_mask
     */
    0xffffffff,
    /*
     * tunnel_sip
     */
    0xAC000001, /* 172.0.0.1 */
    /*
     * tunnel_sip_mask
     */
    0xffffffff,
    /*
     * tunnel DIP6
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 },
    /*
     * tunnel DIP6 mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * tunnel DIP6
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x77, 0x88, 0x99 },
    /*
     * tunnel SIP6 mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * tunnel_type
     */
    bcmTunnelTypeGreAnyIn4,
    /*
     * is_ipv6_vxlan
     */
    0,
    /*
     * tunnel_ttl
     */
    56,
    /*
     * tunnel_dscp
     */
    10,
    /*
     * encap tunnel id
     */
    0,
    /*
     * term tunnel id
     */
    0,
    /*
     * vxlan_port_id
     */
    0,
    /*
     * vlan_port_id
     */
    0,
    /*
     * vpn_id
     */
    15,
    /*
     * default_vpn
     */
    1023,
    /*
     * vni
     */
    5000,
    /*
     * VXLAN vxlan_network_group_id
     */
    1,
    /*
     * VXLAN vxlan_vdc_enable
     */
    0,
    /*
     * VXLAN vxlan_vdc
     */
    0,
    /*
     * access_eth_fwd_mac
     */
    {0x00, 0x00, 0xA5, 0x5A, 0xA5, 0x5A},
    /*
     * provider_eth_fwd_mac
     */
    {0x00, 0x00, 0xA6, 0x6A, 0xA6, 0x6A},
    /*
     * egress native virtual ac
     */
    0
};

void
vxlan_basic_init(
    int unit,
    int is_ipv6_vxlan,
    int vdc_enable)
{
    int rv = BCM_E_NONE;
    int first_fec_in_hier;

    if (is_ipv6_vxlan) {
        cint_vxlan_basic_info.is_ipv6_vxlan = 1;
        cint_vxlan_basic_info.tunnel_type = bcmTunnelTypeVxlan6;
    } else {
        cint_vxlan_basic_info.is_ipv6_vxlan = 0;
        cint_vxlan_basic_info.tunnel_type = bcmTunnelTypeVxlan;
    }

    if (vdc_enable) {
        cint_vxlan_basic_info.vxlan_vdc_enable = 1;
        cint_vxlan_basic_info.vxlan_vdc = 10;
    }

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    cint_vxlan_basic_info.encap_fec_id = first_fec_in_hier;

    return rv;
}

int
vxlan_mc_dip_init(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_ip6_t ipv6_mc = {0xFF, 0xFF, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE};

    sal_memcpy(&(cint_vxlan_basic_info.tunnel_dip6), ipv6_mc, 16);

    return rv;
}

/* 
 * Main function for configuring the VXLAN VPN
 */
int
vxlan_open_vpn(
    int unit,
    int vpn_id,
    int vni)
{
    int rv = BCM_E_NONE;

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.broadcast_group = vpn_id;
    vpn_config.vpn = vpn_id;
    vpn_config.unknown_unicast_group = vpn_id;
    vpn_config.unknown_multicast_group = vpn_id;

    if (cint_vxlan_basic_info.vxlan_vdc_enable) {
        vpn_config.vnid = BCM_VXLAN_VNI_INVALID;
    } else {
        vpn_config.vnid = vni;
    }
    rv = bcm_vxlan_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_vxlan_vpn_create \n");
        return rv;
    }

    /* Create multicast group */
    rv = multicast__open_mc_group(unit, &vpn_id, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__open_mc_group\n");
        return rv;
    }

    /* If network domain is valid, configure VNI->VSI & VSI->VNI mapping with network domain */
    if(cint_vxlan_basic_info.vxlan_vdc_enable)
    {
        bcm_vxlan_network_domain_config_t config;
        bcm_vxlan_network_domain_config_t_init(&config);
        /*
         * Update network domain for the created VNI->VSI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = vpn_id;
        config.vni              = vni;
        config.network_domain   = cint_vxlan_basic_info.vxlan_vdc;
        rv = bcm_vxlan_network_domain_config_add(unit, &config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vxlan_network_domain_config_add \n");
            return rv;
        }

        /*
         * Update network domain for the created VSI->VNI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = vpn_id;
        config.vni              = vni;
        config.network_domain   = cint_vxlan_basic_info.vxlan_vdc;
        rv = bcm_vxlan_network_domain_config_add(unit, &config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vxlan_network_domain_config_add \n");
            return rv;
        }
    }

    return rv;
}

/* 
 * Main function for destroying the VXLAN VPN
 */
int
vxlan_destroy(
    int unit,
    int vpn_id,
    int vni)
{
    int rv = BCM_E_NONE;

    /* Remove the mapping */
    if(cint_vxlan_basic_info.vxlan_vdc_enable)
    {
        bcm_vxlan_network_domain_config_t config;
        bcm_vxlan_network_domain_config_t_init(&config);
        /*
         * Update network domain for the created VNI->VSI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = vpn_id;
        config.vni              = vni;
        config.network_domain   = cint_vxlan_basic_info.vxlan_vdc;
        rv = bcm_vxlan_network_domain_config_remove(unit, &config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vxlan_network_domain_config_remove \n");
            return rv;
        }

        /*
         * Update network domain for the created VSI->VNI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = vpn_id;
        config.vni              = vni;
        config.network_domain   = cint_vxlan_basic_info.vxlan_vdc;
        rv = bcm_vxlan_network_domain_config_remove(unit, &config);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vxlan_network_domain_config_remove \n");
            return rv;
        }
    }

    /* Delete VXLAN port */
    rv = bcm_vxlan_port_delete(unit, vpn_id, cint_vxlan_basic_info.vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_vxlan_port_delete \n");
        return rv;
    }

    /* Destroy VPN */
    rv = bcm_vxlan_vpn_destroy(unit, vpn_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_vxlan_vpn_destroy \n");
        return rv;
    }

    return rv;
}


int
vxlan_tunnel_initiator_create(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);

    /*
     * Create IP tunnel initiator for encapsulating Vxlan tunnel header
     */
    bcm_tunnel_initiator_t_init(&tunnel_init);
    if (cint_vxlan_basic_info.is_ipv6_vxlan) {
        tunnel_init.dip6 = cint_vxlan_basic_info.tunnel_dip6; /* default: 2001:0DB8:0000:0000:0211:22FF:FE33:4455 */
        tunnel_init.sip6 = cint_vxlan_basic_info.tunnel_sip6; /* default: 2001:0DB8:0000:0000:0211:22FF:FE77:8899 */
    } else {
        tunnel_init.dip = cint_vxlan_basic_info.tunnel_dip; /* default: 171.0.0.1 */
        tunnel_init.sip = cint_vxlan_basic_info.tunnel_sip; /* default: 172.0.0.1 */
    }
    tunnel_init.flags = 0;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    tunnel_init.dscp = cint_vxlan_basic_info.tunnel_dscp;
    tunnel_init.ttl = cint_vxlan_basic_info.tunnel_ttl;
    tunnel_init.type = cint_vxlan_basic_info.tunnel_type;
    tunnel_init.l3_intf_id = cint_vxlan_basic_info.tunnel_arp_id;
    tunnel_init.encap_access = bcmEncapAccessTunnel2;

    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    cint_vxlan_basic_info.encap_tunnel_id = tunnel_init.tunnel_id;

    /* Update the egress orientation for the outlif */
    rv = bcm_port_class_set(unit, tunnel_init.tunnel_id, bcmPortClassForwardEgress, cint_vxlan_basic_info.vxlan_network_group_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set \n");
        return rv;
    }

    int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);

    /* At ESEM for vsi, netork_domain -> VNI, outlif profile
       create a virtual AC which to configure the outlif profile in ESEM entry.
     */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vsi = cint_vxlan_basic_info.vpn_id;
    vlan_port.match_class_id = cint_vxlan_basic_info.vxlan_vdc;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.egress_network_group_id = nwk_grp_from_esem ? cint_vxlan_basic_info.vxlan_network_group_id : 0;

    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create \n");
        return rv;
    }

    cint_vxlan_basic_info.egress_native_virtual_ac = vlan_port.vlan_port_id;

    return rv;
}

int
vxlan_tunnel_terminator_create(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_terminator_t tunnel_term;

    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup in TCAM
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    if (cint_vxlan_basic_info.is_ipv6_vxlan) {
        tunnel_term.dip6 = cint_vxlan_basic_info.tunnel_dip6;
        tunnel_term.dip6_mask = cint_vxlan_basic_info.tunnel_dip6_mask;
        tunnel_term.sip6 = cint_vxlan_basic_info.tunnel_sip6;
        tunnel_term.sip6_mask = cint_vxlan_basic_info.tunnel_sip6_mask;        
    } else {
        tunnel_term.dip = cint_vxlan_basic_info.tunnel_dip;
        tunnel_term.dip_mask = cint_vxlan_basic_info.tunnel_dip_mask;
        tunnel_term.sip = cint_vxlan_basic_info.tunnel_sip;
        tunnel_term.sip_mask = cint_vxlan_basic_info.tunnel_sip_mask;
    }
    tunnel_term.vrf      = cint_vxlan_basic_info.provider_vrf;
    tunnel_term.type     = cint_vxlan_basic_info.tunnel_type;
    /* In case it's symmetric tunnel, set termination tunnel id same as encapsulation */
    if (learn_mode == LEARN_SYM_OUTLIF_PORT || learn_mode == LEARN_SYM_OUTLIF_FEC) {
        tunnel_term.tunnel_id = cint_vxlan_basic_info.encap_tunnel_id;
        tunnel_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    cint_vxlan_basic_info.term_tunnel_id = tunnel_term.tunnel_id;
    return rv;
}

int vxlan_split_horizon_set(int unit, int vpn, int vdc)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);

    if (nwk_grp_from_esem) {
        /* At ESEM for vsi, netork_domain -> VNI, outlif profile
           create a virtual AC which to configure the outlif profile in ESEM entry.
         */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vsi = vpn; /* vsi, part of the key in ESEM lookup */
        vlan_port.match_class_id = vdc; /* network domain, part of the key in ESEM lookup */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        if (cint_vxlan_basic_info.egress_native_virtual_ac != 0) {
            vlan_port.flags |= BCM_VLAN_PORT_REPLACE | BCM_VLAN_PORT_WITH_ID;
            vlan_port.vlan_port_id = cint_vxlan_basic_info.egress_native_virtual_ac;
        }
        vlan_port.egress_network_group_id = cint_vxlan_basic_info.vxlan_network_group_id;
        rv = bcm_vlan_port_create(unit, vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_create \n", proc_name);
            return rv;
        }

        cint_vxlan_basic_info.egress_native_virtual_ac = vlan_port.vlan_port_id;
    }

    return rv;
}

int
vxlan_network_domain_set(
    int unit)
{
    int rv = BCM_E_NONE;

    if (cint_vxlan_basic_info.vxlan_vdc_enable) {
        rv = bcm_port_class_set(unit, cint_vxlan_basic_info.term_tunnel_id, bcmPortClassIngress, cint_vxlan_basic_info.vxlan_vdc);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set port %x\n",cint_vxlan_basic_info.term_tunnel_id);
            return rv;
        }

        rv = bcm_port_class_set(unit, cint_vxlan_basic_info.encap_tunnel_id, bcmPortClassEgress, cint_vxlan_basic_info.vxlan_vdc);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set port %x\n",cint_vxlan_basic_info.encap_tunnel_id);
            return rv;
        }
    }

    return rv;
}

int vxlan_vxlan_port_add(
    int unit,
    int provider_port)
{
    int rv = BCM_E_NONE;
    bcm_if_t egress_if;
    bcm_vxlan_port_t vxlan_port;

    /* For learn mode is FEC only(pointing to tunnel), or outlif + FEC(pointing to ARP) */
    BCM_L3_ITF_SET(egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_basic_info.encap_fec_id);

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
    vxlan_port.match_port = provider_port;
    vxlan_port.match_tunnel_id = cint_vxlan_basic_info.term_tunnel_id;
    vxlan_port.default_vpn = cint_vxlan_basic_info.default_vpn;
    vxlan_port.network_group_id = cint_vxlan_basic_info.vxlan_network_group_id;

    /* in case it's not outlif + port, set flag: BCM_VXLAN_PORT_EGRESS_TUNNEL to indicate FEC is valid */
    if (learn_mode != LEARN_SYM_OUTLIF_PORT) {
        vxlan_port.flags = BCM_VXLAN_PORT_EGRESS_TUNNEL;
        vxlan_port.egress_if = egress_if;
    }

    /* in case it's learn FEC only. Don't set the tunnel outlif. */
    if (learn_mode != LEARN_FEC_ONLY) {
        vxlan_port.egress_tunnel_id = cint_vxlan_basic_info.encap_tunnel_id;
    }

    rv = bcm_vxlan_port_add(unit, cint_vxlan_basic_info.vpn_id, &vxlan_port);
    if(rv != BCM_E_NONE) {
        printf("Error,in bcm_vxlan_port_add \n");
        return rv;
    }

    cint_vxlan_basic_info.vxlan_port_id = vxlan_port.vxlan_port_id;

    /* Add to multicast group */
    rv = multicast__gport_encap_add(unit, cint_vxlan_basic_info.vpn_id, provider_port, cint_vxlan_basic_info.encap_tunnel_id, egress_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__gport_encap_add\n");
        return rv;
    }

    return rv;
}

int vxlan_vlan_port_add(
    int unit,
    int port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /* C_VLAN -> 0x8100 */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port = port;
    vlan_port.match_vlan = cint_vxlan_basic_info.access_vid;
    vlan_port.flags = 0;
    vlan_port.vsi = 0;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    cint_vxlan_basic_info.vlan_port_id = vlan_port.vlan_port_id;

    /* Add ac to vswitch */
    rv = bcm_vswitch_port_add(unit, cint_vxlan_basic_info.vpn_id, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /* Add ac to multicast group */
    rv = multicast__gport_encap_add(unit, cint_vxlan_basic_info.vpn_id, port, vlan_port.vlan_port_id, egress_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__gport_encap_add for AC\n");
        return rv;
    }

    /* Add port to VLAN */
    rv = bcm_vlan_gport_add(unit, cint_vxlan_basic_info.access_vid, port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    return rv;
}

int vxlan_l2_addr_add(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_t vid,
    int gport)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.port = gport;
    l2addr.flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add \n");
        return rv;
    }
    return rv;
}

/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * access_port - port where core host is connected to.
 * provider_port - port where DC Fabric router is connected to.
 */
int vxlan_example(
    int unit,
    int is_ipv6_vxlan,
    int access_port,
    int provider_port,
    int vdc_enable)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "vxlan_example";

    /* Init parameters */
    vxlan_basic_init(unit, is_ipv6_vxlan, vdc_enable);

    /*
     * Build L2 VPN
     */
    rv = vxlan_open_vpn(unit, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vni);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vxlan_open_vpn, vpn=%d\n", proc_name, cint_vxlan_basic_info.vpn_id);
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    /* access port */
    rv = in_port_intf_set(unit, access_port, cint_vxlan_basic_info.eth_rif_intf_access);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /* provider port */
    rv = in_port_intf_set(unit, provider_port, cint_vxlan_basic_info.eth_rif_intf_provider);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set out_port%d\n",proc_name,access_port);
        return rv;
    }

    rv = out_port_set(unit, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set out_port %d\n",proc_name,provider_port);
        return rv;
    }

    /*
     * Create ETH-RIF and set its properties
     */
    /* access side */
    rv = intf_eth_rif_create(unit, cint_vxlan_basic_info.eth_rif_intf_access, cint_vxlan_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_vxlan_basic_info.eth_rif_intf_access);
        return rv;
    }

    /* provider side */
    rv = intf_eth_rif_create(unit, cint_vxlan_basic_info.eth_rif_intf_provider, cint_vxlan_basic_info.intf_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_vxlan_basic_info.eth_rif_intf_provider);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties
     */
    l3_ingress_intf ingress_rif;

    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_vxlan_basic_info.provider_vrf;
    ingress_rif.intf_id = cint_vxlan_basic_info.eth_rif_intf_provider;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create\n",proc_name);
        return rv;
    }

    /*
     * Create ARP entry for VXLAN tunnel and set ARP properties.
     */
    rv = l3__egress_only_encap__create(unit, BCM_L3_EGRESS_ONLY, &cint_vxlan_basic_info.tunnel_arp_id,
                                       cint_vxlan_basic_info.tunnel_next_hop_mac, cint_vxlan_basic_info.provider_vlan);
    if(rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_encap__create\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     */
    rv = vxlan_tunnel_initiator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in vxlan_tunnel_initiator_create \n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = vxlan_tunnel_terminator_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function ip_tunnel_term_create \n", proc_name);
        return rv;
    }


    /*
     * Configure tunnel fec entry
     */
    if (learn_mode == LEARN_FEC_ONLY) {
        rv = l3__egress_only_fec__create(unit, cint_vxlan_basic_info.encap_fec_id, cint_vxlan_basic_info.encap_tunnel_id, 0, provider_port, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create fec_id=0x%x\n", proc_name, cint_vxlan_basic_info.encap_fec_id);
            return rv;
        }
    }

    /*
     * configure ARP fec entry for learning purpose
    */
    if (learn_mode == LEARN_SYM_OUTLIF_FEC || learn_mode == LEARN_ASYM_OUTLIF_FEC) {
        rv = l3__egress_only_fec__create(unit, cint_vxlan_basic_info.encap_fec_id, cint_vxlan_basic_info.tunnel_arp_id, 0, provider_port, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_fec__create fec_id=0x%x\n", proc_name, cint_vxlan_basic_info.encap_fec_id);
            return rv;
        }
    }

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_vxlan_port_add(unit, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_vxlan_port_add \n", proc_name);
        return rv;
    }

    /*
     * Set ingress/egress VXLAN tunnel's network domain for VSI->VNI & VNI->VSI mapping
     */
    rv = vxlan_network_domain_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_network_domain_set \n", proc_name);
        return rv;
    }

    /*
     * Add AC
    */
    rv = vxlan_vlan_port_add(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_vlan_port_add \n", proc_name);
        return rv;
    }

    /*
     * Add MACT entry
    */
    /*
     * add bridge entry to provider network
     */
    rv = vxlan_l2_addr_add (unit, cint_vxlan_basic_info.access_eth_fwd_mac, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l2_addr_add\n", proc_name);
        return rv;
    }

    /*
     * add bridge entry to access network
     */
    rv = vxlan_l2_addr_add (unit, cint_vxlan_basic_info.provider_eth_fwd_mac, cint_vxlan_basic_info.vpn_id, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l2_addr_add\n", proc_name);
        return rv;
    }

    /* Create native EVE on AC */
    if (is_native_eve)
    {
        rv = vlan_translate_ive_eve_translation_set(unit, cint_vxlan_basic_info.egress_native_virtual_ac, /* lif  */
                                                           0x8100,                     /* outer_tpid */
                                                           0,                          /* inner_tpid */
                                                           bcmVlanActionReplace,       /* outer_action */
                                                           0,                          /* inner_action */
                                                           20,                         /* new_outer_vid */
                                                           0,                          /* new_inner_vid */
                                                           5,                          /* vlan_edit_profile */
                                                           8,                          /* tag_format - here is ctag */
                                                           FALSE                       /* is_ive */
                                                           );
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, create egress vlan editing\n", proc_name);
            return rv;
        }
    }

    return rv;
}
