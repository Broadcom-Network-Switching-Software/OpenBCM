/*
 * Test Scenario - start
 *
 * ./bcm.user
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_multicast.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vxlan.c
 * cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
 * cint ../../../../src/examples/dnx/geneve/cint_dnx_geneve_basic.c
 * cint
 * learn_mode=LEARN_FEC_ONLY;
 * geneve_example(0,0,202,203,0);
 * exit;
 *
 * // core -> access - flood
 * tx 1 psrc=203 data=0x000c000200010000070001008100000a080045000057000000008011e393ac000001ab000001555517c100430000000017c100138800000000000100000000000581810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * // Received packets on unit 0 should be:
 * // Source port: 0, Destination port: 0
 * // Data: 0x000000000100000000000581810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * // access -> core - flood
 * tx 1 psrc=202 data=0x000011000123000022000556810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * // Received packets on unit 0 should be:
 * // Source port: 0, Destination port: 0
 * // Data: 0x00020000cd1d000c000200018100a00a0800450a00620000000038112b7fac000001ab00000176cf17c1004e00000000000000138800000011000123000022000556810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * // core -> access
 * tx 1 psrc=203 data=0x000c000200010000070001008100000a080045000057000000008011e393ac000001ab000001555517c100430000000017c100138800000022000556000000000581810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * // Received packets on unit 0 should be:
 * // Source port: 0, Destination port: 0
 * // Data: 0x000022000556000000000581810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * // access -> core
 * tx 1 psrc=202 data=0x000000000581000022000556810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * // Received packets on unit 0 should be:
 * // Source port: 0, Destination port: 0
 * // Data: 0x00020000cd1d000c000200018100a00a0800450a00620000000038112b7fac000001ab0000012ea017c1004e00000000000000138800000000000581000022000556810001feffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * Test Scenario - end
*/


int is_native_eve; /* If setting native VLAN editing */

dnx_utils_vxlan_learn_mode_t learn_mode;

/*
 * GENEVE global structure
 */
struct cint_geneve_basic_info_s
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
    int is_geneve6;                         /* if it's GENEVE6 tunnel */
    int tunnel_ttl;                         /* Ip tunnel header TTL */
    int tunnel_dscp;                        /* Ip tunnel header differentiated services code point */
    int encap_tunnel_id;                    /* Encapsulation tunnel id */
    int term_tunnel_id;                     /* Termination tunnel id */
    bcm_gport_t geneve_port_id;             /* GENEVE port id */
    bcm_gport_t vlan_port_id;               /* VLAN port id */
    int vpn_id;                             /* VPN ID */
    int default_vpn;                        /* default VPN ID for ingress termination */
    int vni;                                /* VNI of the GENEVE */
    int geneve_network_group_id;            /* ingress and egress orientation for GENEVE */
    int geneve_network_group_1;             /* network group 1 - for orientation test */
    int geneve_network_group_2;             /* network group 2 - for orientation test */
    int ac_network_group;                   /* AC network group - for orientation test */
    int geneve_vdc_enable;                  /* Option to enable/disable GENEVE VDC support */
    int geneve_vdc;                         /* GENEVE VDC value */
    bcm_mac_t access_eth_fwd_mac;           /* mac for bridge fwd */
    bcm_mac_t provider_eth_fwd_mac;         /* mac for bridge fwd */
    bcm_gport_t egress_native_virtual_ac;   /* egress native vlan port. Represent the AC info in ESEM entry */
    int svtag_enable;                       /* Enable SVTAG lookup */
    int double_udp;                         /* for GENEVE over ESP support, enable SVTAG lookup with double udp flag */
    int stat_pp_enable;                     /* Enable stat pp - BCM_TUNNEL_INIT_STAT_ENABLE*/
    int stat_pp_per_vni;                    /* Set to 1 - for statistics testing per vni */
    int stat_id;                            /* stat_id when stat_pp_per_vni is set */
    int stat_pp_profile;                    /* stat_pp_profile when stat_pp_per_vni is set */
};

cint_geneve_basic_info_s cint_geneve_basic_info = {
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
     * is_geneve6
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
     * geneve_port_id
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
     * GENEVE network_group_id | network_group_1 | network_group_2 | ac_network_group
     */
    1, 1, 2, 0,

    /*
     * GENEVE vdc_enable
     */
    0,
    /*
     * GENEVE vdc
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
    0,
    /*
     * svtag_enable
     */
    0,
    /*
     * double_udp
     */
    0,
    /*
     * stat_pp_enable
     */
    0,
    /*
     * stat_pp_per_vni | stat_id | stat_pp_profile |
     */
    0, 0, 0
};

int geneve_basic_init(
    int unit,
    int is_geneve6,
    int vdc_enable)
{
    int first_fec_in_hier;

    if (is_geneve6) {
        cint_geneve_basic_info.is_geneve6   = 1;
        cint_geneve_basic_info.tunnel_type  = bcmTunnelTypeGeneve6;
    } else {
        cint_geneve_basic_info.is_geneve6   = 0;
        cint_geneve_basic_info.tunnel_type  = bcmTunnelTypeGeneve;
    }

    if (vdc_enable) {
        cint_geneve_basic_info.geneve_vdc_enable = 1;
        cint_geneve_basic_info.geneve_vdc        = 10;
    }

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec_in_hier), "");

    cint_geneve_basic_info.encap_fec_id = first_fec_in_hier;

    return BCM_E_NONE;
}

void geneve_mc_dip_init()
{
    bcm_ip6_t ipv6_mc = {0xFF, 0xFF, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE};

    sal_memcpy(&(cint_geneve_basic_info.tunnel_dip6), ipv6_mc, 16);

}

/*
 * Main function for configuring the VXLAN VPN
 */
int geneve_open_vpn(
    int unit,
    int vpn_id,
    int vni)
{

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags                    = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.broadcast_group          = vpn_id;
    vpn_config.vpn                      = vpn_id;
    vpn_config.unknown_unicast_group    = vpn_id;
    vpn_config.unknown_multicast_group  = vpn_id;

    if (cint_geneve_basic_info.geneve_vdc_enable || cint_geneve_basic_info.stat_pp_per_vni) {
        vpn_config.vnid = BCM_VXLAN_VNI_INVALID;
        printf("\n\n==> configure vnid with BCM_VXLAN_VNI_INVALID\n\n");
    } else {
        vpn_config.vnid = vni;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_vpn_create(unit, &vpn_config), "");

    /* Create multicast group */
    BCM_IF_ERROR_RETURN_MSG(multicast__open_mc_group(unit, &vpn_id, 0), "");

    /* If network domain is valid, configure VNI->VSI & VSI->VNI mapping with network domain */
    if(cint_geneve_basic_info.geneve_vdc_enable)
    {
        bcm_vxlan_network_domain_config_t config;
        bcm_vxlan_network_domain_config_t_init(&config);
        /*
         * Update network domain for the created VNI->VSI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = vpn_id;
        config.vni              = vni;
        config.network_domain   = cint_geneve_basic_info.geneve_vdc;
        BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");

        /*
         * Update network domain for the created VSI->VNI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = vpn_id;
        config.vni              = vni;
        config.network_domain   = cint_geneve_basic_info.geneve_vdc;
        BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");
    }

    if (cint_geneve_basic_info.stat_pp_per_vni)
    {
        bcm_vxlan_network_domain_config_t config;
        config.stat_id          = cint_geneve_basic_info.stat_id; /* Object statistic ID */
        config.stat_pp_profile  = cint_geneve_basic_info.stat_pp_profile; /* Statistics profile */
        config.vsi              = cint_geneve_basic_info.vpn_id;
        config.vni              = cint_geneve_basic_info.vni;
        config.network_domain   = 0;
        config.flags = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");
    }

    return BCM_E_NONE;
}

int geneve_vpn_destroy(int unit)
{

    /* Remove the mapping */
    if(cint_geneve_basic_info.geneve_vdc_enable)
    {
        bcm_vxlan_network_domain_config_t config;
        bcm_vxlan_network_domain_config_t_init(&config);
        /*
         * Update network domain for the created VNI->VSI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = cint_geneve_basic_info.vpn_id;
        config.vni              = cint_geneve_basic_info.vni;
        config.network_domain   = cint_geneve_basic_info.geneve_vdc;
        BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_remove(unit, &config), "");

        /*
         * Update network domain for the created VSI->VNI
         */
        config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        config.vsi              = cint_geneve_basic_info.vpn_id;
        config.vni              = cint_geneve_basic_info.vni;
        config.network_domain   = cint_geneve_basic_info.geneve_vdc;
        BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_remove(unit, &config), "");
    }

    if (cint_geneve_basic_info.stat_pp_per_vni)
    {
        bcm_vxlan_network_domain_config_t config;
        config.stat_id          = cint_geneve_basic_info.stat_id; /* Object statistic ID */
        config.stat_pp_profile  = cint_geneve_basic_info.stat_pp_profile; /* Statistics profile */
        config.vsi              = cint_geneve_basic_info.vpn_id;
        config.vni              = cint_geneve_basic_info.vni;
        config.network_domain   = 0;
        config.flags = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
        BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_remove(unit, &config), "");
    }

    /* Destroy VPN */
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_vpn_destroy(unit, cint_geneve_basic_info.vpn_id), "");

    return BCM_E_NONE;
}

/*
 * Main function for destroying the GENEVE VPN
 */
int geneve_destroy(int unit, int vpn_id, int vni)
{

    /* Delete GENEVE port */
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_port_delete(unit, cint_geneve_basic_info.vpn_id, cint_geneve_basic_info.geneve_port_id), "");

    /* Destroy VPN */
    BCM_IF_ERROR_RETURN_MSG(geneve_vpn_destroy(unit), "");

    return BCM_E_NONE;
}


int geneve_tunnel_initiator_create(int unit)
{
    char *proc_name;
    proc_name = "geneve_tunnel_initiator_create";

    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);

    /*
     * Create IP tunnel initiator for encapsulating Geneve/Geneve6 tunnel header
     */
    bcm_tunnel_initiator_t_init(&tunnel_init);
    if (cint_geneve_basic_info.is_geneve6) {
        tunnel_init.dip6 = cint_geneve_basic_info.tunnel_dip6; /* default: 2001:0DB8:0000:0000:0211:22FF:FE33:4455 */
        tunnel_init.sip6 = cint_geneve_basic_info.tunnel_sip6; /* default: 2001:0DB8:0000:0000:0211:22FF:FE77:8899 */
    } else {
        tunnel_init.dip = cint_geneve_basic_info.tunnel_dip; /* default: 171.0.0.1 */
        tunnel_init.sip = cint_geneve_basic_info.tunnel_sip; /* default: 172.0.0.1 */
    }
    tunnel_init.flags       = 0;
    tunnel_init.type        = cint_geneve_basic_info.tunnel_type;
    tunnel_init.l3_intf_id  = cint_geneve_basic_info.tunnel_arp_id;

    tunnel_init.encap_access    = bcmEncapAccessTunnel2;
    tunnel_init.dscp            = cint_geneve_basic_info.tunnel_dscp;
    tunnel_init.ttl             = cint_geneve_basic_info.tunnel_ttl;
    tunnel_init.egress_qos_model.egress_qos     = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.egress_qos_model.egress_ttl     = bcmQosEgressModelPipeMyNameSpace;

    if  (cint_geneve_basic_info.svtag_enable) {
        tunnel_init.flags |= BCM_TUNNEL_INIT_SVTAG_ENABLE ;
        if  (cint_geneve_basic_info.double_udp) {
            tunnel_init.flags |= BCM_TUNNEL_INIT_FIXED_UDP_SRC_PORT;
            tunnel_init.encap_access = bcmEncapAccessInvalid;
        }
    }

    if (cint_geneve_basic_info.stat_pp_enable) {
        tunnel_init.flags |= BCM_TUNNEL_INIT_STAT_ENABLE;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init), "");

    cint_geneve_basic_info.encap_tunnel_id = tunnel_init.tunnel_id;
    printf("%s(): tunnel created with tunnel_id: 0x%x\n", proc_name, cint_geneve_basic_info.encap_tunnel_id);

    /* Update the egress orientation for the outlif */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, tunnel_init.tunnel_id, bcmPortClassForwardEgress, cint_geneve_basic_info.geneve_network_group_id), "");

    /* At ESEM for vsi, netork_domain -> VNI, outlif profile
       create a virtual AC which to configure the outlif profile in ESEM entry. */
    
    int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);

    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vsi = cint_geneve_basic_info.vpn_id;
    vlan_port.match_class_id = cint_geneve_basic_info.geneve_vdc;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.egress_network_group_id = nwk_grp_from_esem ? cint_geneve_basic_info.geneve_network_group_id : 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
    cint_geneve_basic_info.egress_native_virtual_ac = vlan_port.vlan_port_id;

    return BCM_E_NONE;
}

int geneve_tunnel_terminator_create(int unit)
{

    bcm_tunnel_terminator_t tunnel_term;

    /*
     * Create Geneve/Geneve6 tunnel terminator for SIP,DIP, VRF lookup in TCAM
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    if (cint_geneve_basic_info.is_geneve6) {
        tunnel_term.dip6        = cint_geneve_basic_info.tunnel_dip6;
        tunnel_term.dip6_mask   = cint_geneve_basic_info.tunnel_dip6_mask;
        tunnel_term.sip6        = cint_geneve_basic_info.tunnel_sip6;
        tunnel_term.sip6_mask   = cint_geneve_basic_info.tunnel_sip6_mask;
    } else {
        tunnel_term.dip         = cint_geneve_basic_info.tunnel_dip;
        tunnel_term.dip_mask    = cint_geneve_basic_info.tunnel_dip_mask;
        tunnel_term.sip         = cint_geneve_basic_info.tunnel_sip;
        tunnel_term.sip_mask    = cint_geneve_basic_info.tunnel_sip_mask;
    }

    tunnel_term.vrf      = cint_geneve_basic_info.provider_vrf;
    tunnel_term.type     = cint_geneve_basic_info.tunnel_type;

    /* In case it's symmetric tunnel, set termination tunnel id same as encapsulation */
    if (learn_mode == LEARN_SYM_OUTLIF_PORT || learn_mode == LEARN_SYM_OUTLIF_FEC) {

        int encap_global_lif = BCM_GPORT_TUNNEL_ID_GET(cint_geneve_basic_info.encap_tunnel_id);
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term.tunnel_id, encap_global_lif);

        tunnel_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    }

    if (cint_geneve_basic_info.stat_pp_enable) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_STAT_ENABLE;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term), "");

    cint_geneve_basic_info.term_tunnel_id = tunnel_term.tunnel_id;
    return BCM_E_NONE;
}

int geneve_split_horizon_set(int unit, int vpn, int vdc)
{

    bcm_vlan_port_t vlan_port;

    int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);

    if (nwk_grp_from_esem) {
        /* At ESEM for vsi, netork_domain -> VNI, outlif profile
           create a virtual AC which to configure the outlif profile in ESEM entry.
         */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vsi               = vpn;  /* vsi, part of the key in ESEM lookup */
        vlan_port.match_class_id    = vdc;  /* network domain, part of the key in ESEM lookup */
        vlan_port.criteria          = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
        vlan_port.flags             = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;

        if (cint_geneve_basic_info.egress_native_virtual_ac != 0) {
            vlan_port.flags |= BCM_VLAN_PORT_REPLACE | BCM_VLAN_PORT_WITH_ID;
            vlan_port.vlan_port_id = cint_geneve_basic_info.egress_native_virtual_ac;
        }

        vlan_port.egress_network_group_id = cint_geneve_basic_info.geneve_network_group_id;

        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");

        cint_geneve_basic_info.egress_native_virtual_ac = vlan_port.vlan_port_id;
    }

    return BCM_E_NONE;
}

int geneve_network_domain_set(int unit)
{
    char error_msg[200]={0,};


    if (cint_geneve_basic_info.geneve_vdc_enable) {
        snprintf(error_msg, sizeof(error_msg), "port %x", cint_geneve_basic_info.term_tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_geneve_basic_info.term_tunnel_id, bcmPortClassIngress, cint_geneve_basic_info.geneve_vdc), error_msg);

        snprintf(error_msg, sizeof(error_msg), "port %x", cint_geneve_basic_info.encap_tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_geneve_basic_info.encap_tunnel_id, bcmPortClassEgress, cint_geneve_basic_info.geneve_vdc), error_msg);
    }

    return BCM_E_NONE;
}

int geneve_vxlan_port_add(
    int unit,
    int provider_port)
{

    bcm_if_t egress_if;
    bcm_vxlan_port_t vxlan_port;

    /* For learn mode is FEC only(pointing to tunnel), or outlif + FEC(pointing to ARP) */
    BCM_L3_ITF_SET(egress_if, BCM_L3_ITF_TYPE_FEC, cint_geneve_basic_info.encap_fec_id);

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.criteria         = BCM_VXLAN_PORT_MATCH_VN_ID;
    vxlan_port.match_port       = provider_port;
    vxlan_port.match_tunnel_id  = cint_geneve_basic_info.term_tunnel_id;
    /* if fwd domain lookup fail, and trap is not supported, we use a default fwd domain */
    if (!*dnxc_data_get(unit, "trap", "ingress", "fwd_domain_mode_trap", NULL)) {
        vxlan_port.default_vpn      = cint_geneve_basic_info.default_vpn;
    }
    vxlan_port.network_group_id = cint_geneve_basic_info.geneve_network_group_id;

    /* in case it's not outlif + port, set flag: BCM_VXLAN_PORT_EGRESS_TUNNEL to indicate FEC is valid */
    if (learn_mode != LEARN_SYM_OUTLIF_PORT) {
        vxlan_port.flags        = BCM_VXLAN_PORT_EGRESS_TUNNEL;
        vxlan_port.egress_if    = egress_if;
    }

    /* in case it's learn FEC only. Don't set the tunnel outlif. */
    if (learn_mode != LEARN_FEC_ONLY) {
        vxlan_port.egress_tunnel_id = cint_geneve_basic_info.encap_tunnel_id;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_port_add(unit, cint_geneve_basic_info.vpn_id, &vxlan_port), "");

    cint_geneve_basic_info.geneve_port_id = vxlan_port.vxlan_port_id;

    /* Add to multicast group */
    BCM_IF_ERROR_RETURN_MSG(multicast__gport_encap_add(unit, cint_geneve_basic_info.vpn_id, provider_port, cint_geneve_basic_info.encap_tunnel_id, egress_mc), "");

    return BCM_E_NONE;
}

int geneve_vlan_port_add(
    int unit,
    int port)
{
    char *proc_name;
    proc_name = "geneve_vlan_port_add";

    bcm_vlan_port_t vlan_port;

    /* C_VLAN -> 0x8100 */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria      = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port          = port;
    vlan_port.match_vlan    = cint_geneve_basic_info.access_vid;
    vlan_port.flags         = 0;
    vlan_port.vsi           = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    cint_geneve_basic_info.vlan_port_id = vlan_port.vlan_port_id;

    /* Add ac to vswitch */
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, cint_geneve_basic_info.vpn_id, vlan_port.vlan_port_id), "");

    /* Add ac to multicast group */

    BCM_IF_ERROR_RETURN_MSG(multicast__gport_encap_add(unit, cint_geneve_basic_info.vpn_id, port, vlan_port.vlan_port_id, egress_mc), "for AC");
    printf("%s():  multicast__gport_encap_add for AC, port %d\n", proc_name, port);

    /* Add port to VLAN */

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, cint_geneve_basic_info.access_vid, port, 0), "");

    return BCM_E_NONE;
}

int geneve_l2_addr_add(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_t vid,
    int gport)
{

    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.port     = gport;
    l2addr.flags    = BCM_L2_STATIC;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2addr), "");
    return BCM_E_NONE;
}

/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * access_port - port where core host is connected to.
 * provider_port - port where DC Fabric router is connected to.
 */
int geneve_example(
    int unit,
    int is_geneve6,
    int access_port,
    int provider_port,
    int vdc_enable)
{
    char error_msg[200]={0,};

    /* Init parameters */
    BCM_IF_ERROR_RETURN_MSG(geneve_basic_init(unit, is_geneve6, vdc_enable), "");

    /*
     * Build L2 VPN
     */
    snprintf(error_msg, sizeof(error_msg), "vpn=%d", cint_geneve_basic_info.vpn_id);
    BCM_IF_ERROR_RETURN_MSG(geneve_open_vpn(unit, cint_geneve_basic_info.vpn_id, cint_geneve_basic_info.vni), error_msg);

    /*
     * Set In-Port to In ETh-RIF
     */
    /* access port */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, access_port, cint_geneve_basic_info.eth_rif_intf_access), "intf_in");

    /* provider port */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, provider_port, cint_geneve_basic_info.eth_rif_intf_provider), "intf_in");

    /*
     * Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "out_port=%d", access_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, access_port), error_msg);

    snprintf(error_msg, sizeof(error_msg), "out_port=%d", provider_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, provider_port), error_msg);

    /*
     * Create ETH-RIF and set its properties
     */
    /* access side */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_geneve_basic_info.eth_rif_intf_access);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_geneve_basic_info.eth_rif_intf_access, cint_geneve_basic_info.intf_access_mac_address), error_msg);

    /* provider side */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_geneve_basic_info.eth_rif_intf_provider);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_geneve_basic_info.eth_rif_intf_provider, cint_geneve_basic_info.intf_provider_mac_address), error_msg);

    /*
     * Set Incoming ETH-RIF properties
     */
    l3_ingress_intf ingress_rif;

    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_geneve_basic_info.provider_vrf;
    ingress_rif.intf_id = cint_geneve_basic_info.eth_rif_intf_provider;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");

    /*
     * Create ARP entry for GENEVE tunnel and set ARP properties.
     */

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, BCM_L3_EGRESS_ONLY, &cint_geneve_basic_info.tunnel_arp_id,
                                       cint_geneve_basic_info.tunnel_next_hop_mac, cint_geneve_basic_info.provider_vlan), "");

    /*
     * Create the tunnel initiator
     */
    BCM_IF_ERROR_RETURN_MSG(geneve_tunnel_initiator_create(unit), "");

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    BCM_IF_ERROR_RETURN_MSG(geneve_tunnel_terminator_create(unit), "");

    /*
     * Configure tunnel fec entry
     */
    if (learn_mode == LEARN_FEC_ONLY) {
        int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
        snprintf(error_msg, sizeof(error_msg), "fec_id=0x%x", cint_geneve_basic_info.encap_fec_id);
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_geneve_basic_info.encap_fec_id,
                                                cint_geneve_basic_info.encap_tunnel_id,
                                                0, provider_port, 0, fwd_flag), error_msg);
    }

    /*
     * configure ARP fec entry for learning purpose
    */
    if (learn_mode == LEARN_SYM_OUTLIF_FEC || learn_mode == LEARN_ASYM_OUTLIF_FEC) {
        int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;

        snprintf(error_msg, sizeof(error_msg), "fec_id=0x%x", cint_geneve_basic_info.encap_fec_id);
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_geneve_basic_info.encap_fec_id,
                                                cint_geneve_basic_info.tunnel_arp_id,
                                                0, provider_port, 0, fwd_flag), error_msg);
    }
    /*
     *   following steps was done in geneve_tunnel_initiator_create() already,
     *   Repeat for double check
    */

    /* Update the egress orientation for the outlif */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_geneve_basic_info.encap_tunnel_id, bcmPortClassForwardEgress,
                                    cint_geneve_basic_info.geneve_network_group_id), "");

    /*
     * Add GENEVE port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    BCM_IF_ERROR_RETURN_MSG(geneve_vxlan_port_add(unit, provider_port), "");

    /*
     * Set ingress/egress GENEVE tunnel's network domain for VSI->VNI & VNI->VSI mapping
     */
    BCM_IF_ERROR_RETURN_MSG(geneve_network_domain_set(unit), "");

    /*
     * Add AC
    */
    BCM_IF_ERROR_RETURN_MSG(geneve_vlan_port_add(unit, access_port), "");

    /*
     * Add MACT entry
    */
    /*
     * add bridge entry to provider network
     */
    BCM_IF_ERROR_RETURN_MSG(geneve_l2_addr_add(unit, cint_geneve_basic_info.access_eth_fwd_mac, cint_geneve_basic_info.vpn_id, cint_geneve_basic_info.geneve_port_id), "");

    /*
     * add bridge entry to access network
     */
    BCM_IF_ERROR_RETURN_MSG(geneve_l2_addr_add(unit, cint_geneve_basic_info.provider_eth_fwd_mac, cint_geneve_basic_info.vpn_id, access_port), "");

    /* Create native EVE on AC */
    if (is_native_eve)
    {
        BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit, cint_geneve_basic_info.egress_native_virtual_ac, /* lif  */
                                                           0x8100,                     /* outer_tpid */
                                                           0,                          /* inner_tpid */
                                                           bcmVlanActionReplace,       /* outer_action */
                                                           0,                          /* inner_action */
                                                           20,                         /* new_outer_vid */
                                                           0,                          /* new_inner_vid */
                                                           5,                          /* vlan_edit_profile */
                                                           8,                          /* tag_format - here is ctag */
                                                           FALSE                       /* is_ive */
                                                           ), "create egress vlan editing");
    }

    return BCM_E_NONE;
}


 /*
  * set the flags to BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE:
  *     Configure the orientation filters(drop the packet):
  *     filter AC inLif (network group:    0)   ==>  Geneve outLif (network group : 1)
  *     filter Geneve inLif(network group : 1)  ==>  AC outLif (network group: 0)
  *     filter Geneve inLif(network group : 1)  ==>  Geneve outLif(network group: 2)
  *
  *set the flags to 0:
  *     Disabled the packet filter for all
  *
  */
 int
 geneve_orientation_filters_set(int unit, uint32 flags)
 {

    bcm_switch_network_group_config_t network_group_config;

    /* set network_group orientations for GENEVE tunnel LIFs */
    cint_geneve_basic_info.geneve_network_group_1 = 1;
    cint_geneve_basic_info.geneve_network_group_2 = 2;
    /* set the AC network group */
    cint_geneve_basic_info.ac_network_group = 0;

    char* proc_name;
    proc_name = "geneve_orientation_filters_set";

    printf("%s(): Add orientation filter 1: from AC (%d) to GENEVE Tunnel (%d) \n",
            proc_name, cint_geneve_basic_info.ac_network_group, cint_geneve_basic_info.geneve_network_group_1);

    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id  = cint_geneve_basic_info.geneve_network_group_1;
    network_group_config.config_flags           = flags;

    /** Set orientation filter: filter AC inLif (network group: 0) to GENEVE outLif (network group : 1) */
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit,
                                            cint_geneve_basic_info.ac_network_group,
                                            &network_group_config), "");

    printf("%s(): Add orientation filter 2: from GENEVE (%d) to AC (%d) \n",
            proc_name, cint_geneve_basic_info.geneve_network_group_1, cint_geneve_basic_info.ac_network_group);

    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id  = cint_geneve_basic_info.ac_network_group;
    network_group_config.config_flags           = flags;

    /** Set orientation filter: filter GENEVE inLif(network group : 2) to AC outLif(network group: 0) */
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit,
                                             cint_geneve_basic_info.geneve_network_group_1,
                                             &network_group_config), "");

    printf("%s(): Add orientation filter 3: from GENEVE (%d) to GENEVE (%d) \n",
            proc_name, cint_geneve_basic_info.geneve_network_group_1, cint_geneve_basic_info.geneve_network_group_2);

    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id  = cint_geneve_basic_info.geneve_network_group_2;
    network_group_config.config_flags           = flags;

    /**.Set orientation filter: filter GENEVE inLif(network group : 1) to GENEVE outLif(network group: 2) */
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit,
                                             cint_geneve_basic_info.geneve_network_group_1,
                                             &network_group_config), "");

    return BCM_E_NONE;
 }



/* precondition:
 * geneve_open_vpn has been called with BCM_VXLAN_VNI_INVALID
 * */
int geneve_encapsulation_vpn_to_vni_configuration(int unit, int stat_id, int stat_pp_profile)
{
    bcm_vxlan_network_domain_config_t config;
    config.stat_id          = stat_id; /* Object statistic ID */
    config.stat_pp_profile  = stat_pp_profile; /* Statistics profile */
    config.vsi              = cint_geneve_basic_info.vpn_id;
    config.vni              = cint_geneve_basic_info.vni;
    config.network_domain   = 0;
    config.flags = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");
    return BCM_E_NONE;
}
