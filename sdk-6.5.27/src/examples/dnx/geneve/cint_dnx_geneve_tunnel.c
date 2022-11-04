/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** GENEVE termination global structure */
struct cint_geneve_tunnel_termination_info_s
{
    int geneve_network_group_id;            /* ingress and egress orientation for GENEVE */
    int inner_vlan;                         /* Virtual machine VID */
    int eth_rif_intf_core;                  /* Core RIF */
    int eth_rif_intf_virtual_machine;       /* Virtual Machine RIF */
    int eth_rif_virtual_machine_vrf;        /* router instance VRF resolved at VTT1 */
    int eth_rif_core_vrf;                   /* VRF after termination,resolved as Tunnel-InLif property */
    bcm_mac_t core_mac_address;             /* mac for next hop */
    bcm_mac_t virtual_machine_mac_address;  /* tunnel next hop MAC DA */
    bcm_mac_t virtual_machine_da;           /* Remote VM DA */
    bcm_ip_t tunnel_dip;                    /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;               /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                    /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;               /* tunnel SIP mask */
    int arp_id;                             /* Id for ARP entry */
    int fec_id;                             /* FEC id */
    bcm_gport_t tunnel_id;                  /* Tunnel ID */
    int tunnel_if;                          /* Tunnel Interface */
    int vpn_id;                             /* VPN ID */
    int vni;                                /* VNI of the GENEVE */
    bcm_gport_t geneve_port_id;             /* geneve gport */
    int txsci;
};

cint_geneve_tunnel_termination_info_s cint_geneve_tunnel_term_info = {
    /*
     * GENEVE Orientation, Inner VLAN
     */
    1, 510,
    /*
     * eth_rif_intf_core | eth_rif_intf_virtual_machine
     */
    15, 100,
    /*
     * virtual_machine VRF, Core VRF
     */
    5, 1,
    /*
     * core_mac_address | virtual_machine_mac_address | Virtual Machine DA
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0x00, 0xf1},
    /*
     * tunnel DIP
     */
    0xAB000001, /* 171.0.0.1 */
    /*
     * tunnel DIP mask
     */
    0xffffffff,
    /*
     * tunnel SIP
     */
    0xAC000001, /* 172.0.0.1 */
    /*
     * tunnel SIP mask
     */
    0xffffffff,
    /*
     * arp_id
     */
    0,
    /*
     * fec id | tunnel id | tunnel_if
     */
    5000, 20, 10,
    /*
     * VPN ID
     */
    15,
    /*
     * VNI
     */
    5000,
    /* geneve port id */
    0,
    /*
     * txsci
     */
    0
};

struct cint_geneve_stat_pp_info_s {
    uint32 stat_id;      /* Object statistic ID */
    int stat_pp_profile; /* Statistics profile */
};

struct cint_geneve_tunnel_encapsulation_info_s {
    int vpn_id;                                 /* VPN ID */
    int vni;                                    /* VNID of the GENEVE */
    int geneve_network_group_id;                 /* ingress and egress orientation for GENEVE */
    int virtual_vlan;                           /* Virtual machine VID */
    int core_vlan;                              /* Outer ETH header VID */
    bcm_mac_t dmac_in;                          /* DA of in ETH packet*/
    int eth_rif_intf_core;                      /* Core RIF */
    int eth_rif_core_vrf;                       /* VRF - a router instance for the tunnels's DIP */
    int eth_rif_vm_vrf;                         /* VRF after termination, resolved as Tunnel-InLif property */
    bcm_mac_t intf_core_mac_addr;               /* mac for next hop */
    bcm_mac_t tunnel_next_hop_mac;              /* tunnel next hop MAC DA */
    bcm_ip_t tunnel_dip;                        /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;                   /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                        /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;                   /* tunnel SIP mask*/
    int tunnel_arp_id;                          /* Id for ARP entry */
    int tunnel_fec_id;                          /* FEC id */
    bcm_gport_t tunnel_id;                      /* Tunnel ID */
    bcm_ip_t route_dip;                         /* Route DIP */
    uint32 route_dip_mask;                      /* mask for route DIP */
    int is_native_vlan_editing;                 /* indicate if we want to perform native vlan editing for geneve encapsulation */
    bcm_gport_t geneve_port_id;                 /* geneve gport */
    int svtag_enable;                           /* enable an SVTAG lookup */
    int l3_intf;                                /* l3 interface */
    int vpn_vni_is_mapped;                      /* indicate if the vpn <-> vni is to be configured in bcm_vxlan_vpn_create, if false need to be configured separately */
    cint_geneve_stat_pp_info_s stat_pp_per_vni; /* stat pp information per VNI */
    int double_udp;
};

cint_geneve_tunnel_encapsulation_info_s cint_geneve_tunnel_init_info =
{
    /*
     * vpn_id | vni
     */
       15,      5000,
    /*
     * geneve_network_group_id
     */
       1,
    /*
     * virtual_vlan | core_vlan
     */
       510,      100,
    /*
     * dmac_in
     */
       {0x00, 0x00, 0x00, 0x00, 0x00, 0xf1},
    /*
     * eth_rif_intf_core
     */
       15,
    /*
     * eth_rif_core_vrf | eth_rif_vm_vrf
     */
       1,                 5,
    /*
     * intf_core_mac_addr
     */
       {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /*
     * tunnel_next_hop_mac
     */
       {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * tunnel_dip = 171.0.0.17 | tunnel_dip_mask
     */
       0xAB000011,               0xffffffff,
    /*
     * tunnel_sip = 160.0.0.1 | tunnel_sip_mask
     */
       0xA0000001,              0xffffffff,
    /*
     * tunnel_arp_id | tunnel_fec_id | tunnel_id
     */
       9000,           5000,          0,
    /*
     * route_dip
     */
       0x7fffff03, /* 127.255.255.03 */
    /*
     * route_dip_mask
     */
       0xfffffff0,
    /*
     * is_native_vlan_editing
     */
       0,
    /*
     * geneve port id
     */
       0,
    /*
     * svtag_enable
     */
       0,
     /*
     * l3 interface
     */
       0,
     /*
      * vpn_vni_is_mapped
      */
       1,
     /* stat pp per vni */
     /* Object statistic ID , Statistics profile */
       {0,                      0},
     /*
      * double_udp
      */
       0
};

int
mact_entry_add(
    int unit,
    int geneve_port_id)
{
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, cint_geneve_tunnel_term_info.virtual_machine_da, cint_geneve_tunnel_term_info.vpn_id);
    l2addr.port = geneve_port_id;
    l2addr.mac = cint_geneve_tunnel_term_info.virtual_machine_da;
    l2addr.vid = cint_geneve_tunnel_term_info.vpn_id;
    l2addr.l2mc_group = cint_geneve_tunnel_term_info.vpn_id;
    l2addr.flags = BCM_L2_STATIC;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2addr), "");
    return BCM_E_NONE;
}

int
vxlan_port_add(
    int unit,
    int core_port,
    int tunnel_term_id,
    int tunnel_encapsulation_id,
    bcm_gport_t * geneve_port_id)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    dnx_utils_vxlan_port_add_s vxlan_port_add;
    dnx_utils_vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn = cint_geneve_tunnel_term_info.vpn_id;
    vxlan_port_add.in_port = core_port;
    vxlan_port_add.in_tunnel = tunnel_term_id;
    vxlan_port_add.out_tunnel = tunnel_encapsulation_id;
    BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_geneve_tunnel_term_info.fec_id);
    vxlan_port_add.flags = 0;
    vxlan_port_add.network_group_id = cint_geneve_tunnel_term_info.geneve_network_group_id;
    vxlan_port_add.add_to_mc_group = 0;
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_vxlan_port_add(unit, vxlan_port_add), "");
    *geneve_port_id = vxlan_port_add.vxlan_port_id;
    return BCM_E_NONE;
}

/* 
 * Main function for configuring the GENEVE VPN - uses same I\F as VXLAN VPN
 */
int
geneve_open_vpn(
    int unit,
    int vpn_id,
    int vni)
{

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn_id;
    vpn_config.broadcast_group = vpn_id;
    vpn_config.unknown_unicast_group = vpn_id;
    vpn_config.unknown_multicast_group = vpn_id;
    vpn_config.vnid = vni;
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_vpn_create(unit, &vpn_config), "");
    return BCM_E_NONE;
}

int
vlan_port_add(
    int unit,
    bcm_gport_t in_port,
    bcm_gport_t * port_id)
{
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * the match criteria is port:1, out-vlan:510 
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = in_port;
    vlan_port.match_vlan = cint_geneve_tunnel_term_info.inner_vlan;
    vlan_port.egress_vlan = 0;
    vlan_port.flags = 0;
    vlan_port.vsi = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    /*
     * handle of the created gport 
     */
    *port_id = vlan_port.vlan_port_id;

    return BCM_E_NONE;
}

int
vswitch_add_port(
    int unit,
    bcm_vlan_t vsi,
    bcm_port_t phy_port,
    bcm_gport_t gport)
{

    /*
     * add to vswitch 
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, vsi, gport), "");

    return BCM_E_NONE;
}

int
geneve_tunnel_terminator_create(
    int unit,
    int is_term_test,
    int *term_tunnel_id)
{
    bcm_tunnel_terminator_t tunnel_term;

    /** Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.type = bcmTunnelTypeGeneve;
    if (is_term_test == TRUE)
    {
        tunnel_term.dip      = cint_geneve_tunnel_term_info.tunnel_dip;
        tunnel_term.dip_mask = cint_geneve_tunnel_term_info.tunnel_dip_mask;
        tunnel_term.sip      = cint_geneve_tunnel_term_info.tunnel_sip;
        tunnel_term.sip_mask = cint_geneve_tunnel_term_info.tunnel_sip_mask;
        tunnel_term.vrf      = cint_geneve_tunnel_term_info.eth_rif_core_vrf;
        tunnel_term.flags = BCM_TUNNEL_TERM_STAT_ENABLE;
    }
    else /** init_test */
    {
        tunnel_term.dip      = cint_geneve_tunnel_init_info.tunnel_dip;
        tunnel_term.dip_mask = cint_geneve_tunnel_init_info.tunnel_dip_mask;
        tunnel_term.sip      = cint_geneve_tunnel_init_info.tunnel_sip;
        tunnel_term.sip_mask = cint_geneve_tunnel_init_info.tunnel_sip_mask;
        tunnel_term.vrf      = cint_geneve_tunnel_init_info.eth_rif_core_vrf;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term), "");

    *term_tunnel_id = tunnel_term.tunnel_id;

    return BCM_E_NONE;
}

int
geneve_tunnel_initiator_create(
    int unit,
    int is_term_test,
    bcm_if_t arp_itf,
    bcm_if_t * tunnel_intf,
    int *encap_tunnel_id)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);

    /** init the tunnel_init */
    bcm_tunnel_initiator_t_init(&tunnel_init);

    /** Create IP tunnel initiator for encapsulating GENEVE tunnel header */
    tunnel_init.flags = 0;
    tunnel_init.dscp = 10;      /* default: 10 */
    tunnel_init.type = bcmTunnelTypeGeneve;
    tunnel_init.l3_intf_id = arp_itf;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.ttl = 64;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;

    if (is_term_test == TRUE)
    {
        tunnel_init.dip = cint_geneve_tunnel_term_info.tunnel_dip;
        tunnel_init.sip = cint_geneve_tunnel_term_info.tunnel_sip;
    }
    else /** init_test */
    {
        tunnel_init.dip = cint_geneve_tunnel_init_info.tunnel_dip;
        tunnel_init.sip = cint_geneve_tunnel_init_info.tunnel_sip;
        tunnel_init.flags |= BCM_TUNNEL_INIT_STAT_ENABLE;
        if (cint_geneve_tunnel_init_info.svtag_enable)
        {
            tunnel_init.flags |= BCM_TUNNEL_INIT_SVTAG_ENABLE;
            if (cint_geneve_tunnel_init_info.double_udp)
            {
                tunnel_init.flags |= BCM_TUNNEL_INIT_FIXED_UDP_SRC_PORT;
            }
        }
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init), "");
    *tunnel_intf = l3_intf.l3a_intf_id;

    if (is_term_test == TRUE)
    {
        *encap_tunnel_id = tunnel_init.tunnel_id;
    }
    else /** init_test */
    {
        cint_geneve_tunnel_init_info.l3_intf = l3_intf.l3a_intf_id;
        cint_geneve_tunnel_init_info.tunnel_id = tunnel_init.tunnel_id;
    }

    return BCM_E_NONE;
}

/*
 * The basic GENEVE example with parameters:
 * Unit - relevant unit
 * core_port - port where core routers and aggregation switches are connected.
 * vm_port - port where virtual machines are connected.
 */
int
geneve_tunnel_termination_basic(
    int unit,
    int core_port,
    int virtual_machine_port)
{
    char error_msg[200]={0,};
    char *proc_name;
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id;
    int encap_tunnel_id;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    bcm_gport_t vlan_port_id;

    proc_name = "geneve_tunnel_termination_basic";
    printf("%s(): function starts\n", proc_name);

    /*
     * Build L2 VPN - geneve and vxlan share this API
     */
    snprintf(error_msg, sizeof(error_msg), "vpn=%d", cint_geneve_tunnel_term_info.vpn_id);
    BCM_IF_ERROR_RETURN_MSG(vxlan_open_vpn(unit, cint_geneve_tunnel_term_info.vpn_id, cint_geneve_tunnel_term_info.vni), error_msg);

    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, core_port, cint_geneve_tunnel_term_info.eth_rif_intf_core), "core");

    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, virtual_machine_port), "vm_port");

    /*
     * Create Routing interface for the tunnel's DIP. We will use it as ingress ETH-RIF to perform
     * LL termination (my-mac procedure), to enable the IPv4 routing for this ETH-RIF and to set the VRF.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_geneve_tunnel_term_info.eth_rif_intf_core,
                             cint_geneve_tunnel_term_info.core_mac_address), "eth_rif_intf_core");

    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_geneve_tunnel_term_info.eth_rif_intf_virtual_machine,
                             cint_geneve_tunnel_term_info.virtual_machine_mac_address), "eth_rif_intf_virtual_machine");

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_geneve_tunnel_term_info.eth_rif_intf_core;
    ingr_itf.vrf = cint_geneve_tunnel_term_info.eth_rif_core_vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "intf_out");

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    BCM_IF_ERROR_RETURN_MSG(geneve_tunnel_terminator_create(unit, TRUE, &term_tunnel_id), "");

    /*
     * Create ARP entry and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &(cint_geneve_tunnel_term_info.arp_id),
                                       cint_geneve_tunnel_term_info.virtual_machine_mac_address,
                                       cint_geneve_tunnel_term_info.eth_rif_intf_virtual_machine),
                                       "create egress object ARP only");

    /*
     * Create the tunnel initiator
     */
    BCM_IF_ERROR_RETURN_MSG(geneve_tunnel_initiator_create(unit, TRUE, cint_geneve_tunnel_term_info.arp_id, &encap_tunnel_intf, &encap_tunnel_id), "");

    /*
     * Create FEC and configure its: Outlif (ARP), Destination port
     */
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_geneve_tunnel_term_info.fec_id), "");

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_geneve_tunnel_term_info.fec_id, encap_tunnel_intf,
                                     0, virtual_machine_port, 0, 0), "create egress object FEC only");

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     * GENEVE uses the same vxlan_port_ APIs as VXLAN
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_port_add(unit, core_port, term_tunnel_id, encap_tunnel_id, &cint_geneve_tunnel_term_info.geneve_port_id), "");

    BCM_IF_ERROR_RETURN_MSG(vlan_port_add(unit, virtual_machine_port, &vlan_port_id), "");

    BCM_IF_ERROR_RETURN_MSG(vswitch_add_port(unit, cint_geneve_tunnel_term_info.vpn_id, virtual_machine_port, vlan_port_id), "");

    /*
     * Create an MACT entry for GENEVE tunnel, mapped to the VPN
     */
    BCM_IF_ERROR_RETURN_MSG(mact_entry_add(unit, virtual_machine_port), "");

    return BCM_E_NONE;
}

/*
 * Configure Ingress interfaces
 * Inputs: core_port - incomming port;
 */
int geneve_encapsulation_open_route_interfaces_access(
    int  unit,
    int  core_port)
{
    l3_ingress_intf ingr_itf;

    /** Set In-Port to In ETh-RIF - for incoming port, we configure default VSI. */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, core_port, cint_geneve_tunnel_init_info.eth_rif_intf_core), "intf_in");

    /*
     * Create Routing interface for the tunnel's DIP.
     * We use it as ingress ETH-RIF to perform LL termination (my-mac procedure),
     * to enable the IPv4 routing for this ETH-RIF and to set the VRF.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_geneve_tunnel_init_info.eth_rif_intf_core,
                                   cint_geneve_tunnel_init_info.intf_core_mac_addr), "eth_rif_intf_core");

    return BCM_E_NONE;
}

/*
 * Configure Egress interfaces
 * Inputs: vm_port - outgoing port;
 */
int
geneve_encapsulation_open_route_interfaces_provider(
    int  unit,
    int  vm_port)
{

    /** Set Out-Port default properties */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, vm_port), "vm_port");

    /** Set In-Port to In ETh-RIF - for incoming port, we configure default VSI. */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, vm_port, cint_geneve_tunnel_init_info.eth_rif_intf_core), "intf_in");

    /* configure SA for ETH-RIF configured in tunnel's arp */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_geneve_tunnel_init_info.core_vlan,
                             cint_geneve_tunnel_init_info.intf_core_mac_addr), "eth_rif_intf_core");

    return BCM_E_NONE;
}

/*
 * LIFs Initialization
 * The function ip_tunnel_encapsulation_open_route_interfaces serves as a configuration template to
 * more advanced tests
 * Inputs: unit - relevant unit;
 *         access_port - incoming port;
 *         provider_port - outgoing port;
 */
int
geneve_encapsulation_open_route_interfaces(
    int  unit,
    int  access_port,
    int  provider_port)
{

    cint_geneve_tunnel_init_info.tunnel_arp_id = 5050;
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_geneve_tunnel_init_info.tunnel_fec_id), "");

    /** Configure ingress interfaces */
    BCM_IF_ERROR_RETURN_MSG(geneve_encapsulation_open_route_interfaces_access(unit, access_port), "");

    /** Configure egress interfaces */
    BCM_IF_ERROR_RETURN_MSG(geneve_encapsulation_open_route_interfaces_provider(unit, provider_port), "");

    /** Create ARP entry for GENEVE tunnel and set ARP properties. */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, BCM_L3_EGRESS_ONLY,
                                       &(cint_geneve_tunnel_init_info.tunnel_arp_id),
                                       cint_geneve_tunnel_init_info.tunnel_next_hop_mac,
                                       cint_geneve_tunnel_init_info.core_vlan), "");

    return BCM_E_NONE;
}

/*
 * Ports Initialization
 * The function vxlan_ports_add configures VXLAN, VLAN and VSWITCH ports with the Out-Lif
 * Inputs: unit - relevant unit;
 *         core_port - physical port;
 *         vm_port - virtual machine port;
 *         tunnel_id - GENEVE tunnel id;
 *         *geneve_port_id - GENEVE port id;
 */
int geneve_ports_add(
    int unit,
    int core_port,
    int vm_port,
    bcm_gport_t tunnel_id,
    bcm_gport_t *geneve_port_id)
{
    char error_msg[200]={0,};
    int vxlan_port_flags = 0;
    dnx_utils_vxlan_port_add_s vxlan_port_add;
    bcm_gport_t vlan_port_id;

    /* Add VXLAN port properties and configure tunnel terminator In-Lif and initiatior Out-Lif*/
    dnx_utils_vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn              = cint_geneve_tunnel_init_info.vpn_id;
    vxlan_port_add.in_port          = core_port;
    vxlan_port_add.in_tunnel        = tunnel_id;
    vxlan_port_add.flags            = vxlan_port_flags;
    vxlan_port_add.network_group_id = cint_geneve_tunnel_init_info.geneve_network_group_id;
    vxlan_port_add.add_to_mc_group = 0;
    BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_geneve_tunnel_init_info.tunnel_fec_id);

    printf("Before vxlan port add: update vxlan outlif\n");
    print vxlan_port_add;
    snprintf(error_msg, sizeof(error_msg), "in_gport=0x%08x", tunnel_id);
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_vxlan_port_add(unit, vxlan_port_add), error_msg);

    *geneve_port_id = vxlan_port_add.vxlan_port_id;

    snprintf(error_msg, sizeof(error_msg), "in_intf=0x%08x", vlan_port_id);
    BCM_IF_ERROR_RETURN_MSG(vlan_port_add(unit, vm_port, &vlan_port_id), error_msg);

    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, cint_geneve_tunnel_init_info.vpn_id, vlan_port_id), "");

    return BCM_E_NONE;
}

/*
 * The basic GENEVE example with parameters:
 *     Unit - relevant unit
 *     core_port - port where core routers and aggregation switches are connected.
 *     vm_port - port where virtual machines are connected.
 */
int geneve_tunnel_encapsulation_basic(
    int unit,
    int core_port,
    int vm_port)
{
    char *proc_name;
    char error_msg[200]={0,};
    int device_is_qux;
    bcm_if_t tunnel_out_rif_intf;
    bcm_gport_t tunnel_id;
    bcm_if_t l3egid;
    bcm_l2_addr_t l2addr2;

    proc_name = "geneve_tunnel_encapsulation_basic";
    printf("%s(): function starts\n", proc_name);

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_geneve_tunnel_init_info.tunnel_fec_id), "");

    /** Build L2 VPN */
    snprintf(error_msg, sizeof(error_msg), "vpn=%d", cint_geneve_tunnel_init_info.vpn_id);
    BCM_IF_ERROR_RETURN_MSG(geneve_open_vpn(unit, cint_geneve_tunnel_init_info.vpn_id, cint_geneve_tunnel_init_info.vni), error_msg);

    /** configure entry: VNI+AC */
    if (cint_geneve_tunnel_init_info.is_native_vlan_editing) {
        /* At ESEM for vsi, netork_domain -> VNI, AC information
           create a virtual AC which represent the AC information in ESEM entry.
           The virtual AC save the key to access the ESEM entry.
           In this case: vsi, network domain
           */
        bcm_vlan_port_t vlan_port;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.vsi = cint_geneve_tunnel_init_info.vpn_id; /* vsi, part of the key in ESEM lookup */
        vlan_port.match_class_id = 0; /* network domain, part of the key in ESEM lookup */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.egress_network_group_id = 1;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");

        /** vlan port translation set */
        int vlan_edit_profile = 3;
        bcm_vlan_port_translation_t vlan_port_translation;
        bcm_vlan_port_translation_t_init(&vlan_port_translation);
        vlan_port_translation.flags = BCM_VLAN_ACTION_SET_EGRESS;
        vlan_port_translation.gport = vlan_port.vlan_port_id;
        vlan_port_translation.new_outer_vlan = 12;
        vlan_port_translation.new_inner_vlan = 0;
        vlan_port_translation.vlan_edit_class_id = vlan_edit_profile;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_translation_set(unit, &vlan_port_translation), "");

        /** Create action ID */
        int action_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id), "");

        /** Set translation action */
        bcm_vlan_action_set_t action;
        bcm_vlan_action_set_t_init(&action);
        action.outer_tpid = 0x8100;
        action.inner_tpid = 0x8100;
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS,
                                              action_id, &action), "");
        /** Set translation action class (map edit_profile & tag_format to action_id) */
        int tag_format_ctag = 8;
        bcm_vlan_translate_action_class_t action_class;
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = vlan_edit_profile;
        action_class.tag_format_class_id = tag_format_ctag;
        action_class.vlan_translation_action_id = action_id;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");
    }

    /** Initialize ports, LIFs, Eth-RIFs etc */
    BCM_IF_ERROR_RETURN_MSG(geneve_encapsulation_open_route_interfaces(unit, core_port, vm_port), "");

    /** Build tunnel terminations and router interfaces*/
    BCM_IF_ERROR_RETURN_MSG(geneve_tunnel_terminator_create(unit, FALSE, &tunnel_id), "");

    /** Create the tunnel initiator */
    BCM_IF_ERROR_RETURN_MSG(geneve_tunnel_initiator_create(unit, FALSE, cint_geneve_tunnel_init_info.tunnel_arp_id,
                                        &tunnel_out_rif_intf, NULL), "");

    /** Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF */
    snprintf(error_msg, sizeof(error_msg), "fec_id=0x%x", cint_geneve_tunnel_init_info.tunnel_fec_id);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_geneve_tunnel_init_info.tunnel_fec_id,
                                     tunnel_out_rif_intf, 0, core_port, 0, 0), error_msg);
    printf("geneve tunnel fec_id: 0x%x\n", cint_geneve_tunnel_init_info.tunnel_fec_id);

    /* update tunnel init orientation.
     * It's not done by vxlan_port_add since we configure it with tunnel term lif and FEC */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_geneve_tunnel_init_info.tunnel_id, bcmPortClassForwardEgress,
        cint_geneve_tunnel_init_info.geneve_network_group_id), "create in update orientation for tunnel outlif");

    /** Initialize ports: VXLAN/VLAN/VSWITCH/TPID, GENEVE uses vxlan_port_XXXX APIs */
    BCM_IF_ERROR_RETURN_MSG(geneve_ports_add(unit, core_port, vm_port, tunnel_id, &cint_geneve_tunnel_init_info.geneve_port_id), "");

    /** Create an MACT entry for GENEVE tunnel, mapped to the VPN */
    BCM_IF_ERROR_RETURN_MSG(vxlan_l2_addr_add(unit, cint_geneve_tunnel_init_info.dmac_in,
                                 cint_geneve_tunnel_init_info.vpn_id,
                                 cint_geneve_tunnel_init_info.geneve_port_id), "");

    return BCM_E_NONE;
}

/*
 * Main function for destroying the VXLAN VPN
 */
int
geneve_vxlan_port_destroy(
    int unit,
    int is_term_test)
{
    int geneve_port_id = (is_term_test == TRUE) ? cint_geneve_tunnel_term_info.geneve_port_id :
                                                  cint_geneve_tunnel_init_info.geneve_port_id;
    int vpn_id = (is_term_test == TRUE) ? cint_geneve_tunnel_term_info.vpn_id :
                                          cint_geneve_tunnel_init_info.vpn_id;

    /* Delete VXLAN port */
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_port_delete(unit, vpn_id, geneve_port_id), "");

    /* Destroy VPN */
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_vpn_destroy(unit, vpn_id), "");

    return BCM_E_NONE;
}
