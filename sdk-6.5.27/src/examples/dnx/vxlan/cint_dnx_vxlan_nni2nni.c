
/*
 * Test Scenario
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vxlan.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_multicast.c
  cint ../../../../src/examples/dnx/vxlan/cint_dnx_vxlan_nni2nni.c
  cint
  vxlan_nni2nni_example(0,201,202,203);
  exit;
 *
 * VXLAN core -> flood
 *
  tx 1 psrc=201 data=0x000c000200000000070001008100000f080045000057000000008011ef83a0000001ab000011555512b50043000008000000001388000000000000ff000c000200018100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00000011cd1d000c00020000810000640800450000570000000038113782a0000002ab0000122c4d12b50043000008000000001389000000000000ff000c000200018100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Source port: 0, Destination port: 0
 * Data: 0x0000000000ff000c000200018100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * VXLAN core -> access
 *
  tx 1 psrc=201 data=0x000c000200000000070001008100000f080045000057000000008011ef83a0000001ab000011555512b50043000008000000001388000000000000f0000c000200018100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x0000000000f0000c000200018100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * VXLAN core -> core
 *
  tx 1 psrc=201 data=0x000c000200000000070001008100000f080045000057000000008011ef83a0000001ab000011555512b5004300000800000000138800000000001122000c000200018100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00000011cd1d000c00020000810000640800450000570000000038113782a0000002ab000012fe6412b5004300000800000000138900000000001122000c000200018100001effff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Test Scenario - end
 *
 */
int is_native_eve; /* If setting native VLAN editing */


dnx_utils_vxlan_learn_mode_t learn_mode = LEARN_SYM_OUTLIF_PORT;

/*
 * VXLAN global structure
 */
struct cint_vxlan_basic_info_s
{
    bcm_vlan_t access_vid;                  /* access vid for AC */
    int eth_rif_intf_access;                /* in RIF */
    int eth_rif_intf_core;              /* out RIF */
    bcm_mac_t intf_core_mac_address;    /* mymac for core in RIF */
    bcm_mac_t tunnel_next_hop_mac[2];          /* mac for next hop */
    int access_vrf;                         /* VRF, resolved as Tunnel-InLif property */
    int core_vrf;                       /* VRF after termination, resolved as Tunn-InLif property */
    int encap_fec_id;                       /* FEC id */
    int tunnel_arp_id;                      /* Id for ARP entry */
    bcm_vlan_t core_vlan;               /* VID assigned to the outgoing packet */
    bcm_ip_t tunnel_dip[2];                    /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;               /* tunnel DIP mask */
    bcm_ip_t tunnel_sip[2];                    /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;               /* tunnel SIP mask */
    bcm_tunnel_type_t tunnel_type;          /* tunnel type */
    int tunnel_ttl;                         /* Ip tunnel header TTL */
    int tunnel_dscp;                        /* Ip tunnel header differentiated services code point */
    int encap_tunnel_id;                    /* Encapsulation tunnel id */
    int term_tunnel_id;                     /* Termination tunnel id */
    bcm_gport_t vxlan_port_id;              /* VXLAN port id */
    bcm_gport_t vlan_port_id;               /* VLAN port id */
    int vpn_id;                             /* VPN ID */
    int default_vpn;                        /* default VPN ID for ingress termination */
    int ingress_vni;                        /* VNI of the VXLAN for packet coming from network*/
    int egress_vni;                         /* VNI of the VXLAN for packet going to network */
    int vxlan_network_group_id;             /* ingress and egress orientation for VXLAN */
    int vxlan_vdc_enable;                   /* Option to enable/disable VXLAN VDC support */
    int vxlan_vdc;                          /* VXLAN VDC value */
    bcm_mac_t local_vm_mac;                 /* mac for bridge fwd */
    bcm_mac_t remote_vm_mac;                /* mac for bridge fwd */
    bcm_gport_t egress_native_virtual_ac;   /* egress native vlan port. Represent the AC info in ESEM entry */
};

cint_vxlan_basic_info_s cint_vxlan_basic_info = {
    /*
     * access_vid
     */
    510,
    /*
     * eth_rif_intf_access | eth_rif_intf_core
     */
    5, 15,
    /*
     * intf_core_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /*
     * tunnel_next_hop_mac
     */
    {{0x00, 0x02, 0x00, 0x00, 0xcd, 0x1d}, {0x00, 0x00, 0x00, 0x11, 0xcd, 0x1d}},
    /*
     * access_vrf, core_vrf
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
     * core_vlan
     */
    10,
    /*
     * tunnel_dip
     */
    {0xAB000011 /* 171.0.0.17 */, 0xAB000012 /* 171.0.0.18 */ },
    /*
     * tunnel_dip_mask
     */
    0xffffffff,
    /*
     * tunnel_sip
     */
    {0xA0000001 /* 160.0.0.1 */, 0xA0000002 /* 160.0.0.2 */ },
    /*
     * tunnel_sip_mask
     */
    0xffffffff,
    /*
     * tunnel_type
     */
    bcmTunnelTypeVxlan,
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
     * ingress vni
     */
    5000,
    /*
     * egress vni
     */
    5001,

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
     * local_vm_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0xf0},
    /*
     * remote_vm_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0x11, 0x22},
    /*
     * egress native virtual ac
     */
    0
};

int
vxlan_basic_init(
    int unit)
{
    int first_fec_in_hier;


    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec_in_hier), "");

    cint_vxlan_basic_info.encap_fec_id = first_fec_in_hier;

    return BCM_E_NONE;
}

int
vxlan_mc_dip_init(
    int unit)
{
    bcm_ip6_t ipv6_mc = {0xFF, 0xFF, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE};

    sal_memcpy(&(cint_vxlan_basic_info.tunnel_dip6), ipv6_mc, 16);

    return BCM_E_NONE;
}

/* 
 * Main function for configuring the VXLAN VPN
 */
int
vxlan_open_vpn(
    int unit)
{

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.broadcast_group = cint_vxlan_basic_info.vpn_id;
    vpn_config.vpn = cint_vxlan_basic_info.vpn_id;
    vpn_config.unknown_unicast_group = cint_vxlan_basic_info.vpn_id;
    vpn_config.unknown_multicast_group = cint_vxlan_basic_info.vpn_id;
    vpn_config.vnid = BCM_VXLAN_VNI_INVALID;
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_vpn_create(unit, &vpn_config), "");

    /* Create multicast group */
    BCM_IF_ERROR_RETURN_MSG(multicast__open_mc_group(unit, &(cint_vxlan_basic_info.vpn_id), 0), "");

    bcm_vxlan_network_domain_config_t config;
    bcm_vxlan_network_domain_config_t_init(&config);
    /*
     * Update network domain for the created VNI->VSI
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
    config.vsi              = cint_vxlan_basic_info.vpn_id;
    config.vni              = cint_vxlan_basic_info.ingress_vni;
    config.network_domain   = cint_vxlan_basic_info.vxlan_vdc;
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");

    /*
     * Update network domain for the created VSI->VNI
     */
    config.flags            = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;
    config.vsi              = cint_vxlan_basic_info.vpn_id;
    config.vni              = cint_vxlan_basic_info.egress_vni;
    config.network_domain   = cint_vxlan_basic_info.vxlan_vdc;
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &config), "");

    return BCM_E_NONE;
}

int
vxlan_tunnel_initiator_create(
    int unit, int tunnel_idx)
{
    char error_msg[200]={0,};
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);

    /*
     * Create IP tunnel initiator for encapsulating Vxlan tunnel header
     */
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip = cint_vxlan_basic_info.tunnel_dip[tunnel_idx]; /* default: 171.0.0.1 */
    tunnel_init.sip = cint_vxlan_basic_info.tunnel_sip[tunnel_idx]; /* default: 172.0.0.1 */
    tunnel_init.flags = 0;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    tunnel_init.dscp = cint_vxlan_basic_info.tunnel_dscp;
    tunnel_init.ttl = cint_vxlan_basic_info.tunnel_ttl;
    tunnel_init.type = cint_vxlan_basic_info.tunnel_type;
    tunnel_init.l3_intf_id = cint_vxlan_basic_info.tunnel_arp_id;
    tunnel_init.encap_access = bcmEncapAccessTunnel2;

    if (cint_vxlan_basic_info.encap_tunnel_id != 0) {
        tunnel_init.tunnel_id = cint_vxlan_basic_info.encap_tunnel_id;
        tunnel_init.flags |= BCM_TUNNEL_WITH_ID;
    }


    snprintf(error_msg, sizeof(error_msg), "tunnel index: %d, tunnel_id: 0x%x", tunnel_idx, cint_vxlan_basic_info.encap_tunnel_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init), error_msg);

    cint_vxlan_basic_info.encap_tunnel_id = tunnel_init.tunnel_id;
    printf("tunnel created with tunnel_id: 0x%x\n", cint_vxlan_basic_info.encap_tunnel_id);

    /* Update the egress orientation for the outlif */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, tunnel_init.tunnel_id, bcmPortClassForwardEgress, cint_vxlan_basic_info.vxlan_network_group_id), "");

    return BCM_E_NONE;
}

int
vxlan_tunnel_terminator_create(
    int unit, int index)
{
    bcm_tunnel_terminator_t tunnel_term;

    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup in TCAM
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);

    tunnel_term.dip = cint_vxlan_basic_info.tunnel_dip[index];
    tunnel_term.dip_mask = cint_vxlan_basic_info.tunnel_dip_mask;
    tunnel_term.sip = cint_vxlan_basic_info.tunnel_sip[index];
    tunnel_term.sip_mask = cint_vxlan_basic_info.tunnel_sip_mask;
    tunnel_term.vrf      = cint_vxlan_basic_info.core_vrf;
    tunnel_term.type     = cint_vxlan_basic_info.tunnel_type;
    /* In case it's symmetric tunnel, set termination tunnel id same as encapsulation */
    if (learn_mode == LEARN_SYM_OUTLIF_PORT || learn_mode == LEARN_SYM_OUTLIF_FEC) {

        int encap_global_lif = BCM_GPORT_TUNNEL_ID_GET(cint_vxlan_basic_info.encap_tunnel_id);
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term.tunnel_id, encap_global_lif);
        tunnel_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term), "");


    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, tunnel_term.tunnel_id, bcmPortClassForwardIngress, cint_vxlan_basic_info.vxlan_network_group_id), "");

    cint_vxlan_basic_info.term_tunnel_id = tunnel_term.tunnel_id;
    return BCM_E_NONE;
}

int vxlan_split_horizon_set(int unit, int vpn, int vdc)
{
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
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");

        cint_vxlan_basic_info.egress_native_virtual_ac = vlan_port.vlan_port_id;
    }

    return BCM_E_NONE;
}

int
vxlan_network_domain_set(
    int unit)
{
    char error_msg[200]={0,};

    if (cint_vxlan_basic_info.vxlan_vdc_enable) {
        snprintf(error_msg, sizeof(error_msg), "port %x", cint_vxlan_basic_info.term_tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_vxlan_basic_info.term_tunnel_id, bcmPortClassIngress, cint_vxlan_basic_info.vxlan_vdc), error_msg);

        snprintf(error_msg, sizeof(error_msg), "port %x", cint_vxlan_basic_info.encap_tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_vxlan_basic_info.encap_tunnel_id, bcmPortClassEgress, cint_vxlan_basic_info.vxlan_vdc), error_msg);
    }

    return BCM_E_NONE;
}

int vxlan_vxlan_port_add(
    int unit,
    int core_port)
{
    bcm_if_t egress_if;
    bcm_vxlan_port_t vxlan_port;

    /* For learn mode is FEC only(pointing to tunnel), or outlif + FEC(pointing to ARP) */
    BCM_L3_ITF_SET(egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_basic_info.encap_fec_id);

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
    vxlan_port.match_port = core_port;
    vxlan_port.match_tunnel_id = cint_vxlan_basic_info.term_tunnel_id;
    /* if fwd domain lookup fail, and trap is not supported, we use a default fwd domain */
    if (!*dnxc_data_get(unit, "trap", "ingress", "fwd_domain_mode_trap", NULL)) {
        vxlan_port.default_vpn = cint_vxlan_basic_info.default_vpn;
    }
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

    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_port_add(unit, cint_vxlan_basic_info.vpn_id, &vxlan_port), "");

    cint_vxlan_basic_info.vxlan_port_id = vxlan_port.vxlan_port_id;

    /* Add to multicast group */
    BCM_IF_ERROR_RETURN_MSG(multicast__gport_encap_add(unit, cint_vxlan_basic_info.vpn_id, core_port, cint_vxlan_basic_info.encap_tunnel_id, egress_mc), "");

    return BCM_E_NONE;
}

int vxlan_vlan_port_add(
    int unit,
    int port)
{
    bcm_vlan_port_t vlan_port;

    /* C_VLAN -> 0x8100 */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port = port;
    vlan_port.match_vlan = cint_vxlan_basic_info.access_vid;
    vlan_port.flags = 0;
    vlan_port.vsi = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    cint_vxlan_basic_info.vlan_port_id = vlan_port.vlan_port_id;

    /* Add ac to vswitch */
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, cint_vxlan_basic_info.vpn_id, vlan_port.vlan_port_id), "");

    /* Add ac to multicast group */
    BCM_IF_ERROR_RETURN_MSG(multicast__gport_encap_add(unit, cint_vxlan_basic_info.vpn_id, port, vlan_port.vlan_port_id, egress_mc), "for AC");

    /* Add port to VLAN */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, cint_vxlan_basic_info.access_vid, port, 0), "");

    return BCM_E_NONE;
}

int vxlan_l2_addr_add(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_t vid,
    int gport)
{
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.port = gport;
    l2addr.flags = BCM_L2_STATIC;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2addr), "");
    return BCM_E_NONE;
}



int vxlan_configure_fec(int unit, int core_port)
{
    char error_msg[200]={0,};
    /*
     * Configure tunnel fec entry
     */
    if (learn_mode == LEARN_FEC_ONLY) {
        int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
        snprintf(error_msg, sizeof(error_msg), "fec_id=0x%x", cint_vxlan_basic_info.encap_fec_id);
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_vxlan_basic_info.encap_fec_id, cint_vxlan_basic_info.encap_tunnel_id, 0, core_port, 0, fwd_flag), error_msg);
    }

    /*
     * configure ARP fec entry for learning purpose
    */
    if (learn_mode == LEARN_SYM_OUTLIF_FEC || learn_mode == LEARN_ASYM_OUTLIF_FEC) {
        int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
        snprintf(error_msg, sizeof(error_msg), "fec_id=0x%x", cint_vxlan_basic_info.encap_fec_id);
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_vxlan_basic_info.encap_fec_id, cint_vxlan_basic_info.tunnel_arp_id, 0, core_port, 0, fwd_flag), error_msg);
    }
    return BCM_E_NONE;
}

/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * access_port - port where core host is connected to.
 * core_port - port where DC Fabric router is connected to.
 */
int vxlan_nni2nni_example(
    int unit,
    int core_port_in,
    int core_port_out,
    int access_port)
{
    char error_msg[200]={0,};

    /* Init parameters */
    BCM_IF_ERROR_RETURN_MSG(vxlan_basic_init(unit), "");

    /*
     * Build L2 VPN
     */
    snprintf(error_msg, sizeof(error_msg), "vpn=%d", cint_vxlan_basic_info.vpn_id);
    BCM_IF_ERROR_RETURN_MSG(vxlan_open_vpn(unit), error_msg);

    /* Set core In-Port to In ETh-RIF */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, core_port_in, cint_vxlan_basic_info.eth_rif_intf_core), "intf_in");

    /*
     * Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "out_port=%d", access_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, access_port), error_msg);

    /*
     * Create ETH-RIF and set its properties
     */
    /* core side */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_vxlan_basic_info.eth_rif_intf_core);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_vxlan_basic_info.eth_rif_intf_core, cint_vxlan_basic_info.intf_core_mac_address), error_msg);

    /*
     * Set Incoming ETH-RIF properties
     */
    l3_ingress_intf ingress_rif;

    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_vxlan_basic_info.core_vrf;
    ingress_rif.intf_id = cint_vxlan_basic_info.eth_rif_intf_core;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");



    /*
     * Create ARP entry for VXLAN tunnel and set ARP properties.
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, BCM_L3_EGRESS_ONLY, &cint_vxlan_basic_info.tunnel_arp_id,
                                       cint_vxlan_basic_info.tunnel_next_hop_mac[0], cint_vxlan_basic_info.core_vlan), "");



    /*
     * Create the tunnel initiator
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_tunnel_initiator_create(unit, 0), "");

    int network_domain = 1; /* for split horizon */
    if (*dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL))
    {
        /* Set egress tunnel's network group for split horizon*/
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_vxlan_basic_info.encap_tunnel_id, bcmPortClassEgress, network_domain), "");
    }

    BCM_IF_ERROR_RETURN_MSG(vxlan_configure_fec(unit, core_port_in), "");

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_tunnel_terminator_create(unit, 0), "");

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_vxlan_port_add(unit, core_port_in), "");

    int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);
    /* At ESEM for vsi, netork_domain -> VNI, outlif profile
       create a virtual AC which to configure the outlif profile in ESEM entry. */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vsi = cint_vxlan_basic_info.vpn_id;
    vlan_port.match_class_id = nwk_grp_from_esem ? network_domain : cint_vxlan_basic_info.vxlan_vdc;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;

    vlan_port.egress_network_group_id = nwk_grp_from_esem ? cint_vxlan_basic_info.vxlan_network_group_id : 0;

    printf("updating network group in ESEM: %d \n", vlan_port.egress_network_group_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
    cint_vxlan_basic_info.egress_native_virtual_ac = vlan_port.vlan_port_id;

    /** creaet a 2nd tunnel */

    cint_vxlan_basic_info.vxlan_network_group_id = 0;


    /*
     * Create ARP entry for VXLAN tunnel and set ARP properties.
     */
    cint_vxlan_basic_info.core_vlan = 100;
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_vxlan_basic_info.eth_rif_intf_core);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_vxlan_basic_info.core_vlan, cint_vxlan_basic_info.intf_core_mac_address), error_msg);


    cint_vxlan_basic_info.tunnel_arp_id = 0;
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, BCM_L3_EGRESS_ONLY, &cint_vxlan_basic_info.tunnel_arp_id,
                                       cint_vxlan_basic_info.tunnel_next_hop_mac[1], cint_vxlan_basic_info.core_vlan), "");

    /*
     * Create the tunnel initiator
     */
    cint_vxlan_basic_info.encap_tunnel_id = 0;
    BCM_IF_ERROR_RETURN_MSG(vxlan_tunnel_initiator_create(unit, 1), "");

    cint_vxlan_basic_info.encap_fec_id++;
    BCM_IF_ERROR_RETURN_MSG(vxlan_configure_fec(unit, core_port_out), "");



    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    cint_vxlan_basic_info.term_tunnel_id = 0;
    BCM_IF_ERROR_RETURN_MSG(vxlan_tunnel_terminator_create(unit, 1), "");

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_vxlan_port_add(unit, core_port_out), "");

    /*
     * Add AC
    */
    BCM_IF_ERROR_RETURN_MSG(vxlan_vlan_port_add(unit, access_port), "");


    BCM_IF_ERROR_RETURN_MSG(vxlan_l2_addr_add (unit, cint_vxlan_basic_info.local_vm_mac, cint_vxlan_basic_info.vpn_id, access_port), "");

    BCM_IF_ERROR_RETURN_MSG(vxlan_l2_addr_add (unit, cint_vxlan_basic_info.remote_vm_mac, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vxlan_port_id), "");

    return BCM_E_NONE;
}
