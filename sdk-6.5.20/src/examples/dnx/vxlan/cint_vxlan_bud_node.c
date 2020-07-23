/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Example of basic VXLAN configuration
 *
 * cint ../../src/examples/sand/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_vxlan.c
 * cint ../../src/examples/dnx/utility/cint_dnx_util_rch.c
 * cint ../../src/examples/dnx/cint_vxlan_bud_node.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int in_core_port = 200;
 * int out_core_port = 201;
 * int virtual_machine_port = 202;
 * int virtual_machine_port_mc = 203;
 * int recycle_port = 40;
 * rv = vxlan_bud_node_configuration(unit, in_core_port, out_core_port, virtual_machine_port, virtual_machine_port_mc, recycle_port);
 * print rv;
 */

/*
 * VXLAN global structure
 */
struct cint_vxlan_tunnel_bud_node_info_s
{
    int vxlan_network_group_id;                 /* ingress and egress orientation for VXLAN */
    int inner_vlan;                             /* Virtual machine VID */
    int eth_rif_intf_core;                      /* Core RIF */
    int eth_rif_core_vrf;                       /* router instance VRF resolved at VTT1 */
    int eth_rif_intf_core2;                     /* another core RIF */
    int eth_rif_virtual_machine_vrf;            /* VRF after termination */
    int eth_rif_intf_core_route_mc;                  /* Core RIF for outgoing packets*/
    bcm_mac_t core_mac_address;                 /* mac for next hop */
    bcm_mac_t virtual_machine_mac_address;      /* MAC SA */
    bcm_mac_t virtual_machine_da;               /* Remote VM DA */
    bcm_mac_t route_mc_mac_address;             /* route mc next hop MAC DA */
    bcm_ip_t tunnel_init_dip;                   /* tunnel init DIP */
    bcm_ip_t tunnel_dip_mask;                   /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                        /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;                   /* tunnel SIP mask */
    int arp_id;                                 /* Id for ARP entry */
    int fec_id;                                 /* FEC id */
    int vpn_id;                                 /* VPN ID */
    int vni;                                    /* VNI of the VXLAN */
    int vxlan_tunnel_terminator_flags;          /* Flags for ip tunnel terminator of type vxlan */
    int mc_group;                               /* Multicast group ID*/
    int out_ports [4];                          /* Outgoing ports, associated with the MC group */
    int nof_replications;                       /* Number of MC replications to be made*/
    int tunnel_mc_dip;                          /* MC DIP used for 2nd pass tunnel termination*/
    int uuc_mcid;                               /* MCID used for unknown DA*/
    int recycle_encap_id;                       /* recycle encap id */
    int in_core_port;                           /* port at core side */
};

cint_vxlan_tunnel_bud_node_info_s cint_vxlan_tunnel_bud_node_info = {
    /*
     * VXLAN Orientation, Inner VLAN
     */
    1, 20,
    /*
     * eth_rif_intf_core | eth_rif_core_vrf
     */
    15, 1,
    /*
     * eth_rif_intf_core2, eth_rif_virtual_machine_vrf | eth_rif_intf_core_route_mc
     */
    100, 5, 10,
    /*
     * core_mac_address | virtual_machine_mac_address | virtual_machine_da 
     */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0x00, 0xf1},
    /*
     * route_mc_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0xf1},
    /*
     * tunnel_init_dip
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
     * arp_id | fec_id 
     */
    0,          40961,
    /*
     * vpn_id | vni
     */
    15,         5000,
    /*
     * vxlan_tunnel_terminator_flags
     */
    BCM_TUNNEL_TERM_BUD,
    /*
     * mc_group
     */
    6000,
    /*
     * out_ports
     */
    {201, 40, 202, 203},
    /*
     * nof_replications
     */
    2,
    /*
     * tunnel_mc_dip          |   uuc_mcid
     */
    0xE0020202 /** 224.2.2.2 */,    100, 
    /*
     * recycle encap id
     */
    0, 
    /*
     * core port
    */
    0
};


int
vxlan_port_add(
    int unit,
    int core_port,
    int tunnel_term_id,
    int tunnel_encapsulation_id,
    bcm_gport_t * vxlan_port_id)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    vxlan_port_add_s vxlan_port_add;
    vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn = cint_vxlan_tunnel_bud_node_info.vpn_id;
    vxlan_port_add.in_port = core_port;
    vxlan_port_add.in_tunnel = tunnel_term_id;
    vxlan_port_add.out_tunnel = tunnel_encapsulation_id;
    BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_tunnel_bud_node_info.fec_id);
    vxlan_port_add.flags = 0;
    vxlan_port_add.network_group_id = cint_vxlan_tunnel_bud_node_info.vxlan_network_group_id;

    
    rv = vxlan__vxlan_port_add(unit, vxlan_port_add);
    *vxlan_port_id = vxlan_port_add.vxlan_port_id;

    return BCM_E_NONE;
}

int
vxlan_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t * tunnel_intf,
    int *encap_tunnel_id)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);

    /*
     * Create IP tunnel initiator for encapsulating Vxlan tunnel header
     */
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip = cint_vxlan_tunnel_bud_node_info.tunnel_init_dip;  /* default: 171.0.0.17 */
    tunnel_init.sip = cint_vxlan_tunnel_bud_node_info.tunnel_sip;  /* default: 160.0.0.1 */
    tunnel_init.flags = 0;
    tunnel_init.dscp = 10;      /* default: 10 */
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type = bcmTunnelTypeVxlan;
    tunnel_init.l3_intf_id = arp_itf;
    tunnel_init.ttl = 64;       /* default: 64 */
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;   /* default: -1 */
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }
    *tunnel_intf = l3_intf.l3a_intf_id;
    *encap_tunnel_id = tunnel_init.tunnel_id;
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
    char *proc_name;

    proc_name = "vxlan_open_vpn";
    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn_id;
    vpn_config.broadcast_group = vpn_id;
    vpn_config.unknown_unicast_group = vpn_id;
    vpn_config.unknown_multicast_group = vpn_id;
    vpn_config.vnid = vni;
    rv = bcm_vxlan_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_vxlan_vpn_create \n", proc_name);
        return rv;
    }
    return rv;
}

int
vxlan_tunnel_terminator_create(
    int unit,
    int *term_tunnel_id)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup in VTT3 SEM
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip      = cint_vxlan_tunnel_bud_node_info.tunnel_mc_dip;
    tunnel_term.dip_mask = cint_vxlan_tunnel_bud_node_info.tunnel_dip_mask;
    tunnel_term.sip      = cint_vxlan_tunnel_bud_node_info.tunnel_sip;
    tunnel_term.sip_mask = cint_vxlan_tunnel_bud_node_info.tunnel_sip_mask;
    tunnel_term.vrf      = cint_vxlan_tunnel_bud_node_info.eth_rif_core_vrf;
    tunnel_term.type     = bcmTunnelTypeVxlan;
    tunnel_term.flags    = cint_vxlan_tunnel_bud_node_info.vxlan_tunnel_terminator_flags;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    *term_tunnel_id = tunnel_term.tunnel_id;
    return rv;
}

/*
 * Create a single MC replication
 * replications - pointer to a replication to create
 * port - the port that is assign to this replication.
 */
int
set_multicast_replication(
    bcm_multicast_replication_t * replications,
    bcm_gport_t port,
    int encap)
{
    bcm_multicast_replication_t_init(replications);
    replications->encap1 = encap;
    replications->port = port;
    return BCM_E_NONE;
}

/*
 * Create an MC group
 * - arp_id - ARP-ID
 * - replications - an array of replications.
 * - mc_group - the MC group
 */
int
create_multicast(
    int unit,
    bcm_multicast_replication_t * replications,
    int nof_replications,
    bcm_multicast_t mc_group)
{
    int rv = BCM_E_NONE;
    int flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;

    rv = bcm_multicast_create(unit, flags, &mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_multicast_add(unit, mc_group, BCM_MULTICAST_INGRESS_GROUP, nof_replications, replications);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    return rv;
}

/*
 * Add an IPv4 entry
 * - mc_ip_addr - the MC group DIP
 * - mc_ip_mask - the MC group DIP mask
 * - s_ip_addr - the SIP
 * - s_ip_mask - SIP mask
 * - eth_rif_id - the ETH RIF
 * - vrf -VRF
 * - mc_group - the MC group
 */
int
add_ipv4_ipmc(
    int unit,
    bcm_ip_t mc_ip_addr,
    bcm_ip_t mc_ip_mask,
    bcm_ip_t s_ip_addr,
    bcm_ip_t s_ip_mask,
    bcm_vrf_t eth_rif_id,
    int vrf,
    bcm_multicast_t mc_group,
    int fec)
{

    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip_addr;
    data.mc_ip_mask = mc_ip_mask;
    data.s_ip_addr = s_ip_addr;
    data.s_ip_mask = s_ip_mask;
    data.vid = eth_rif_id;
    data.vrf = vrf;
    data.l3a_intf = fec;
    data.group = mc_group;
    /*
     * Creates the entry 
     */
    rv = bcm_ipmc_add(unit, &data);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    return rv;
}

/*
 * Create a MC group for the case of unknown MC DA.
 * Used after the 2nd pass tunnel termination on the inner DA
 */
int l2_unknown_multicast_da_create(
    int unit,
    int vxlan_port_id)
{
    int flags;
    int rv = BCM_E_NONE;
    flags = (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP);
    rv = bcm_multicast_create(unit, flags, &cint_vxlan_tunnel_bud_node_info.vpn_id);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_create\n");
        return rv;
    }

    flags = BCM_MULTICAST_INGRESS_GROUP;

    bcm_multicast_replication_t rep_array;
    bcm_multicast_replication_t_init(&rep_array);

    rep_array.port = cint_vxlan_tunnel_bud_node_info.out_ports[2];
    rv = bcm_multicast_add(unit, cint_vxlan_tunnel_bud_node_info.vpn_id, flags, 1, &rep_array);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add\n");
        return rv;
    }

    rep_array.port = cint_vxlan_tunnel_bud_node_info.out_ports[3];
    rv = bcm_multicast_add(unit, cint_vxlan_tunnel_bud_node_info.vpn_id, flags, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add in Port= Port_2\n");
        return rv;
    }
    return rv;
}

/*
 * Create an MACT entry for VXLAN tunnel, mapped to the VPN 
 */
int
vxlan_bud_node_l2_addr_add(
    int unit,
    bcm_mac_t l2_vm_dmac_addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr2;

    bcm_l2_addr_t_init(&l2addr2, l2_vm_dmac_addr, cint_vxlan_tunnel_bud_node_info.vpn_id);
    l2addr2.mac        = l2_vm_dmac_addr;
    l2addr2.vid        = cint_vxlan_tunnel_bud_node_info.vpn_id;
    l2addr2.l2mc_group = cint_vxlan_tunnel_bud_node_info.vpn_id;
    l2addr2.flags      = BCM_L2_MCAST;

    rv = bcm_l2_addr_add(unit, &l2addr2);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * VXLAN BUD node example with parameters:
 * Unit - relevant unit
 * in_core_port - port where core routers and aggregation switches are connected.
 * out_core_port - port where core routers and aggregation switches are connected.
 * virtual_machine_port - port to which virtual machines are connected.
 * virtual_machine_mc_port - port to which virtual machines are connected, packet goes out with new VXLAN header encapsulated
 * recycle_port - recycle port, for packets going on the ingress 2nd pass.
 */
int
vxlan_bud_node_configuration(
    int unit,
    int in_core_port,
    int out_core_port,
    int virtual_machine_port,
    int virtual_machine_mc_port,
    int recycle_port)
{
    int rv;
    char *proc_name;
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id;
    int encap_tunnel_id;
    int recycle_entry_encap_id;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    bcm_multicast_replication_t replications_1st_pass[2];
    bcm_multicast_replication_t replications_2nd_pass[3];
    bcm_gport_t vlan_port_id, vlan_port_id_mc;
    bcm_gport_t vxlan_port_id, vxlan_port_id_filtered;
    uint32 is_j2_mode;

    is_j2_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));

    proc_name = "vxlan_bud_node_configuration";

    cint_vxlan_tunnel_bud_node_info.in_core_port = in_core_port;

    cint_vxlan_tunnel_bud_node_info.out_ports[0] = out_core_port;
    cint_vxlan_tunnel_bud_node_info.out_ports[1] = recycle_port;
    cint_vxlan_tunnel_bud_node_info.out_ports[2] = virtual_machine_port;
    cint_vxlan_tunnel_bud_node_info.out_ports[3] = virtual_machine_mc_port;

    /** Build L2 VPN */
    rv = vxlan_open_vpn(unit, cint_vxlan_tunnel_bud_node_info.vpn_id, cint_vxlan_tunnel_bud_node_info.vni);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_open_vpn, vpn=%d, \n", proc_name, cint_vxlan_tunnel_bud_node_info.vpn_id);
        return rv;
    }

    /** Set In-Port to In ETh-RIF for incoming VXLAN packets from core*/
    rv = in_port_intf_set(unit, in_core_port, cint_vxlan_tunnel_bud_node_info.eth_rif_intf_core);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set - in_core_port\n", proc_name);
        return rv;
    }

    /** Set Out-Port default properties for the packets going on on the 1st pass - to the core as MC packets*/
    rv = out_port_set(unit, out_core_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set out_core_port \n", proc_name);
        return rv;
    }

    /** Set Out-Port default properties for the packets going on on the 2nd pass - out of the core*/
    rv = out_port_set(unit, virtual_machine_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set virtual_machine_port \n", proc_name);
        return rv;
    }

    /** Set Out-Port default properties for the packets going on on the 2nd pass - to another vxlan core*/
    rv = out_port_set(unit, virtual_machine_mc_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set virtual_machine_port \n", proc_name);
        return rv;
    }

    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, recycle_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set recycle_port \n", proc_name);
        return rv;
    }

    /** configure port as recycle port, Part of context selection at
     *  2nd pass VTT1.
      */
    rv = bcm_port_control_set(unit, recycle_port, bcmPortControlOverlayRecycle, 1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_port_control_set \n", proc_name);
        return rv;
    }

    /*
     * Create Routing interface for the tunnel's DIP. We will use it as ingress ETH-RIF to perform
     * LL termination (my-mac procedure), to enable the IPv4 routing for this ETH-RIF and to set the VRF.
     */
    if (is_j2_mode)
    {
        rv = intf_eth_rif_create(unit, cint_vxlan_tunnel_bud_node_info.eth_rif_intf_core,
                             cint_vxlan_tunnel_bud_node_info.core_mac_address);
    }
    else
    {
        rv = intf_eth_rif_create_with_qos_uniform(unit, cint_vxlan_tunnel_bud_node_info.eth_rif_intf_core,
                             cint_vxlan_tunnel_bud_node_info.core_mac_address);
    }
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_core \n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_vxlan_tunnel_bud_node_info.eth_rif_intf_core;
    ingr_itf.vrf = cint_vxlan_tunnel_bud_node_info.eth_rif_core_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set eth_rif_intf_core \n", proc_name);
        return rv;
    }

    /* 
     * Create egress routing interface used to build the LL after tunnel encapsulation. 
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_vxlan_tunnel_bud_node_info.eth_rif_intf_core2,
                             cint_vxlan_tunnel_bud_node_info.virtual_machine_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_core2\n", proc_name);
        return rv;
    }

    /** Create egress routing interface for routing in the core.
     *  used for the 1st pass MC copy  */
    rv = intf_eth_rif_create(unit, cint_vxlan_tunnel_bud_node_info.eth_rif_intf_core_route_mc,
                             cint_vxlan_tunnel_bud_node_info.route_mc_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_core_route_mc\n", proc_name);
        return rv;
    }

    /** Create entry, which will be built the recycle header for the copy going on the 2nd pass*/
    rv = create_recycle_entry(unit, &recycle_entry_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create_recycle_entry \n", proc_name);
        return rv;
    }
    cint_vxlan_tunnel_bud_node_info.recycle_encap_id = BCM_L3_ITF_VAL_GET(recycle_entry_encap_id);

    /** Create Mc replication for the mc copy  for 1st pass MC, routing entry according to tunnel DIP*/
    rv = set_multicast_replication(&replications_1st_pass[1], cint_vxlan_tunnel_bud_node_info.out_ports[0], cint_vxlan_tunnel_bud_node_info.eth_rif_intf_core_route_mc);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function set_multicast_replication eth_rif_intf_core_route_mc, \n");
        return rv;
    }
    /** Create Mc replication for the recycle copy*/
    rv = set_multicast_replication(&replications_1st_pass[0], cint_vxlan_tunnel_bud_node_info.out_ports[1], cint_vxlan_tunnel_bud_node_info.recycle_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function set_multicast_replication recycle_entry_encap_id, \n");
        return rv;
    }
    /** Create a multicast group with the recycle outlif and the
     *  core routed out-rif   */
    rv = create_multicast(unit, replications_1st_pass, cint_vxlan_tunnel_bud_node_info.nof_replications, cint_vxlan_tunnel_bud_node_info.mc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function create_multicast, \n");
        return rv;
    }

    /** Add IPv4 IPMC entry for 1st pass */
    rv = add_ipv4_ipmc(unit, 
                       cint_vxlan_tunnel_bud_node_info.tunnel_mc_dip, 
                       cint_vxlan_tunnel_bud_node_info.tunnel_dip_mask, 
                       cint_vxlan_tunnel_bud_node_info.tunnel_sip, 
                       cint_vxlan_tunnel_bud_node_info.tunnel_sip_mask,
                       cint_vxlan_tunnel_bud_node_info.eth_rif_intf_core, 
                       cint_vxlan_tunnel_bud_node_info.eth_rif_core_vrf, 
                       cint_vxlan_tunnel_bud_node_info.mc_group, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv4_ipmc to LEM table, \n");
        return rv;
    }



    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = vxlan_tunnel_terminator_create(unit, &term_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /** Create ARP entry and set its properties */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_vxlan_tunnel_bud_node_info.arp_id),
                                       cint_vxlan_tunnel_bud_node_info.virtual_machine_mac_address,
                                       cint_vxlan_tunnel_bud_node_info.eth_rif_intf_core2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n", proc_name);
        return rv;
    }

    /** Create the tunnel initiator */
    rv = vxlan_tunnel_initiator_create(unit, cint_vxlan_tunnel_bud_node_info.arp_id, &encap_tunnel_intf, &encap_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }

    /** Create FEC and configure its: Outlif (ARP), Destination port */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vxlan_tunnel_bud_node_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, cint_vxlan_tunnel_bud_node_info.fec_id, encap_tunnel_intf, 0, virtual_machine_mc_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /** Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif */
    
    rv = vxlan_port_add(unit, recycle_port, term_tunnel_id, encap_tunnel_id, &vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_port_add \n", proc_name);
    }

    /*
     * Create an MACT entry for VXLAN tunnel, mapped to the VPN
     */
    rv = vxlan_bud_node_l2_addr_add(unit, 
                                    cint_vxlan_tunnel_bud_node_info.virtual_machine_da);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l2_addr_add\n", proc_name);
        return rv;
    }

    /** Add 2nd pass MC entry - unknown DA */
    rv = l2_unknown_multicast_da_create(unit, vxlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l2_unknown_multicas_da_add\n", proc_name);
        return rv;
    }
   return rv;
}

/**vxlan termination tunnel qos update*/
int vxlan_tunnel_term_qos_update(int unit, int qos_model)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_info;

    /** get IP tunnel terminator  */
    bcm_tunnel_terminator_t_init(&tunnel_info);
    tunnel_info.dip      = cint_vxlan_tunnel_bud_node_info.tunnel_mc_dip;
    tunnel_info.dip_mask = cint_vxlan_tunnel_bud_node_info.tunnel_dip_mask;
    tunnel_info.sip      = cint_vxlan_tunnel_bud_node_info.tunnel_sip;
    tunnel_info.sip_mask = cint_vxlan_tunnel_bud_node_info.tunnel_sip_mask;
    tunnel_info.vrf      = cint_vxlan_tunnel_bud_node_info.eth_rif_core_vrf;
    tunnel_info.type     = bcmTunnelTypeVxlan;
    tunnel_info.flags    = cint_vxlan_tunnel_bud_node_info.vxlan_tunnel_terminator_flags;
    rv = bcm_tunnel_terminator_get(unit, &tunnel_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vxlan_tunnel_terminator_get\n");
        return rv;
    }
    /*replace qos model*/
    tunnel_info.ingress_qos_model.ingress_phb = qos_model;
    tunnel_info.ingress_qos_model.ingress_remark = qos_model;
    tunnel_info.ingress_qos_model.ingress_ttl = qos_model;
    tunnel_info.ingress_qos_model.ingress_ecn = bcmQosIngressEcnModelInvalid;

    tunnel_info.flags |= (BCM_TUNNEL_TERM_TUNNEL_WITH_ID | BCM_TUNNEL_REPLACE);

    rv = bcm_tunnel_terminator_create(unit, &tunnel_info);
    if (rv != BCM_E_NONE)
    {
        printf("%s Error, bcm_vxlan_tunnel_terminator_create\n", "vxlan_tunnel_ingress_qos_update");
        return rv;
    }
    return rv;
}
