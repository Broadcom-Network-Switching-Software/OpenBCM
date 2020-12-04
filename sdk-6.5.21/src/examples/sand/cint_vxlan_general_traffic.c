/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

enum learn_mode_t {
    LEARN_FEC_ONLY = 0, 
    LEARN_SYM_OUTLIF_PORT = 1,
    LEARN_SYM_OUTLIF_FEC = 2
};

/*
 * VXLAN global structure
 */
struct cint_vxlan_basic_info_s {
    int vpn_id;                         /* VPN ID */
    int vni;                            /* VNID of the VXLAN */
    int vxlan_network_group_id;         /* ingress and egress orientation for VXLAN */
    int virtual_vlan;                   /* Virtual machine VID */
    int core_vlan;                      /* Outer ETH header VID */
    int eth_rif_intf_core;              /* Core RIF */
    int eth_rif_core_vrf;               /* VRF - a router instance for the tunnels's DIP */
    int eth_rif_vm_vrf;                 /* VRF after termination,resolved as Tunnel-InLif property */
    bcm_mac_t virtual_machine_dmac;     /* Remote VM (inner)DA for after TT*/
    bcm_mac_t tunnel_core_mac_addr;     /* Core My-MAC - tunnel DA/SA TT/TI resp.*/
    bcm_mac_t tunnel_next_hop_addr;     /* tunnel next hop MAC DA for TI */
    bcm_mac_t virtual_machine_smac;     /* Virtual machine inner SA for after TT */
    bcm_ip_t tunnel_dip;                /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;           /* tunnel DIP mask */
    bcm_ip_t tunnel_term_sip;           /* tunnel SIP */
    bcm_ip_t tunnel_term_sip_mask;      /* tunnel SIP mask*/
    bcm_ip_t tunnel_init_sip;           /* tunnel SIP */
    int encap_arp_id;                   /* Id for ARP entry */
    int encap_fec_id;                   /* FEC id */
    int encap_arp_fec_id;               /* FEC id. Used for learning: FEC (ARP) + tunnel outlif */
    int tunnel_if;                      /* Tunnel Interface */
    bcm_gport_t tunnel_id;              /* Tunnel ID */
    bcm_ip_t route_dip;                 /* Route DIP */
    uint32 route_dip_mask;              /* mask for route DIP */
    int vxlan_tunnel_type;              /* VxLAN or VxLAN-GPE */
    int tunnel_term_id;                 /* Tunnel terminatino id */
    int tunnel_init_id;                 /* Tunnel init id */
    learn_mode_t learn_mode;            /* indicate how should we learn */
};



cint_vxlan_basic_info_s cint_vxlan_basic_info =
{
    /*
     * vpn_id | vni
     */
       15,      5000,
    /*
     * vxlan_network_group_id
     */
       1,
    /*
     * virtual_vlan | core_vlan
     */
       510,           100,
    /* 
     * eth_rif_intf_core
     */
       20,
    /*
     * eth_rif_core_vrf | eth_rif_vm_vrf
     */
       1,                 5,
    /*
     * virtual_machine_dmac
     */
       {0x00, 0x00, 0x00, 0x00, 0x01, 0x00},
    /*
     * tunnel_core_mac_addr
     */
       {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /*
     * tunnel_next_hop_addr
     */
       {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * virtual_machine_smac
     */
       {0x00, 0x00, 0x00, 0x00, 0x05, 0x81},
    /*
     * tunnel_dip = 171.0.0.17 | tunnel_dip_mask
     */
       0xAB000011,               0xffffffff,
    /*
     * tunnel_term_sip = 160.0.0.1  | tunnel_term_sip_mask
     */
       0xA0000001,                    0xffffffff,
    /*
     * tunnel_init_sip = 170.0.0.17
     */
       0xAA000011,
    /*
     * encap_arp_id | encap_fec_id | encap_arp_fec_id | tunnel_if | tunnel_id
     */
       9000,          5000,          6100,              10,         0,
    /*
     * route_dip
     */
       0x7fffff03, /* 127.255.255.03 */
    /*
     * route_dip_mask
     */
       0xfffffff0,
    /*
     * vxlan_tunnel_type
     */
       bcmTunnelTypeVxlan,
    /* 
     * tunnel_term_id
     */ 
       7000,
    /*
     * tunnel_init_id
     */
       8000,
    /* 
     * learn_mode
     */
       LEARN_FEC_ONLY
};

/*
 * LIFs Initialization
 * The function ip_tunnel_encapsulation_open_route_interfaces serves as a configuration template to
 * more advanced tests
 * Inputs: unit - relevant unit;
 *         access_port - incoming port;
 *         provider_port - outgoing port;
 *         *proc_name - main function name;
 */
int
vxlan_general_open_route_interfaces(
    int  unit,
    int  core_port,
    int  vm_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    l3_ingress_intf ingr_itf;

    char *proc_name;
    proc_name = "vxlan_general_open_route_interfaces";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2\n", proc_name);
        return rv;
    }
    else if(device_is_jericho2)
    {
        cint_vxlan_basic_info.encap_arp_id     = 5050;
        cint_vxlan_basic_info.encap_fec_id     = 40961;
        cint_vxlan_basic_info.encap_arp_fec_id = 41961;
    }

    /** Set In-Port to In ETh-RIF - for incoming port, we configure default VSI. */
    rv = in_port_intf_set(unit, core_port, cint_vxlan_basic_info.eth_rif_intf_core);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }
    rv = in_port_intf_set(unit, vm_port, cint_vxlan_basic_info.virtual_vlan);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }

    /** Set Out-Port default properties */
    rv = out_port_set(unit, vm_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set vm_port \n", proc_name);
        return rv;
    }

    /*
     * Create Routing interface for the tunnel's DIP. 
     * We use it as ingress ETH-RIF to perform LL termination (my-mac procedure),
     * to enable the IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_vxlan_basic_info.eth_rif_intf_core,
                             cint_vxlan_basic_info.tunnel_core_mac_addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create, eth_rif_intf_core \n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_vxlan_basic_info.eth_rif_intf_core;
    ingr_itf.vrf     = cint_vxlan_basic_info.eth_rif_core_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set intf_out \n", proc_name);
        return rv;
    }

    /** Needed only for JER1 */
    if(!device_is_jericho2)
    {
        /*
         * Create ingress Tunnel interface
         * The RIF property of the InLif used only for JR1
         */
        rv = intf_eth_rif_create(unit, cint_vxlan_basic_info.tunnel_if,
                                       cint_vxlan_basic_info.tunnel_core_mac_addr);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_eth_rif_create tunnel_if 2 \n", proc_name);
            return rv;
        }
        /*
         * Set Incoming Tunnel-RIF properties
         */
        ingr_itf.intf_id = cint_vxlan_basic_info.tunnel_if;
        ingr_itf.vrf     = cint_vxlan_basic_info.eth_rif_vm_vrf;
        rv = intf_ingress_rif_set(unit, &ingr_itf);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, intf_ingress_rif_set \n", proc_name);
            return rv;
        }
    }


    /** Create ARP entry for VXLAN tunnel and set ARP properties. */
    rv = l3__egress_only_encap__create(unit, BCM_L3_EGRESS_ONLY,
                                       &(cint_vxlan_basic_info.encap_arp_id),
                                       cint_vxlan_basic_info.tunnel_next_hop_addr,
                                       cint_vxlan_basic_info.core_vlan);
    if(rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3_egress_only_encap__create\n", proc_name);
        return rv;
    }


    return rv;
}

/*
 * MAIN configuration procedure
 */
int
vxlan_general_traffic_example(
    int unit,
    int core_port,
    int vm_port)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    int term_tunnel_id;
    bcm_if_t encap_tunnel_intf;
    bcm_gport_t vlan_port_id;
    bcm_gport_t vxlan_port_id;
    char *proc_name;
    proc_name = "vxlan_general_traffic_example";

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2\n", proc_name);
        return rv;
    }

    /** Init L2 VXLAN module for JER1 */
    if(!device_is_jericho2)
    {
        rv = bcm_vxlan_init(unit);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, bcm_vxlan_init\n", proc_name);
            return rv;
        }
    }

    rv = vxlan_open_vpn(unit, cint_vxlan_basic_info.vpn_id, cint_vxlan_basic_info.vni);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_open_vpn\n", proc_name);
        return rv;
    }

    rv = vxlan_general_open_route_interfaces(unit, core_port, vm_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_general_open_route_interfaces\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = vxlan_tunnel_terminator_create(unit, &term_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_tunnel_terminator_create\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     */
    rv = vxlan_tunnel_initiator_create(unit, cint_vxlan_basic_info.encap_arp_id, &encap_tunnel_intf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_tunnel_initiator_create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif (Tunnel), Destination port
     */
    rv = l3__egress_only_fec__create(unit, cint_vxlan_basic_info.encap_fec_id, encap_tunnel_intf,
                                     0, core_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif (ARP), Destination port
     */
    rv = l3__egress_only_fec__create(unit, cint_vxlan_basic_info.encap_arp_fec_id, 0,
                                     cint_vxlan_basic_info.encap_arp_id, core_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /*
     * Initialize ports: VXLAN/VLAN/VSWITCH/TPID
     * Add VXLAN port properties and configure the tunnel terminator
     * In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_ports_add(unit, core_port, vm_port, term_tunnel_id, &vxlan_port_id, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_ports_add\n", proc_name);
        return rv;
    }

    /*
     * Create an MACT entry for VXLAN tunnel, mapped to the VPN
     */
    rv = vxlan_l2_addr_add(unit, cint_vxlan_basic_info.virtual_machine_dmac, vm_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l2_addr_add\n", proc_name);
        return rv;
    }

    
    return rv;
}

/*
 * Create and configure a VXLAN VPN
 */
int vxlan_open_vpn(
    int unit,
    int vpn,
    int vni)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    char *proc_name;
    int flags;

    proc_name = "vxlan_open_vpn";
    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2 \n", proc_name);
        return rv;
    }

    if(!device_is_jericho2)
    {
        rv = multicast__open_mc_group(unit, &vpn, BCM_MULTICAST_TYPE_L2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, multicast__open_mc_group\n", proc_name);
            return rv;
        }
    }

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn                     = vpn;
    vpn_config.broadcast_group         = vpn;
    vpn_config.unknown_unicast_group   = vpn;
    vpn_config.unknown_multicast_group = vpn;
    vpn_config.vnid                    = vni;

    rv = bcm_vxlan_vpn_create(unit,&vpn_config);
    if(rv != BCM_E_NONE)
    {
        printf("%s(): error in bcm_vxlan_vpn_create \n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Create and configure VXLAN tunnel termination
 */
int vxlan_tunnel_terminator_create(
    int unit,
    bcm_gport_t *tunnel_id)
{
    int rv = BCM_E_NONE;
    int device_is_jericho2;
    bcm_tunnel_terminator_t tunnel_term;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2 device_is_jericho2 \n");
        return rv;
    }

    /** Create the tunnel termination lookup and the tunnel termination In-Lif */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip      = cint_vxlan_basic_info.tunnel_dip;       /* default: 171.0.0.17 */
    tunnel_term.dip_mask = cint_vxlan_basic_info.tunnel_dip_mask;  /* default: 255.255.255.255 */
    tunnel_term.sip      = cint_vxlan_basic_info.tunnel_term_sip;       /* default: 160.0.0.1 */
    tunnel_term.sip_mask = cint_vxlan_basic_info.tunnel_term_sip_mask;  /* default: 255.255.255.255 */
    tunnel_term.vrf      = cint_vxlan_basic_info.eth_rif_core_vrf; /* default: 1 */
    tunnel_term.type     = cint_vxlan_basic_info.vxlan_tunnel_type;

    BCM_GPORT_TUNNEL_ID_SET(tunnel_term.tunnel_id, cint_vxlan_basic_info.tunnel_term_id);
    tunnel_term.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;

    if(!device_is_jericho2)
    {
        tunnel_term.tunnel_id = 0xf;
        tunnel_term.tunnel_if = cint_vxlan_basic_info.tunnel_if;
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_tunnel_terminator_create, \n", proc_name);
        return rv;
    }

    *tunnel_id = tunnel_term.tunnel_id;

    if (!device_is_jericho2)
    {
         /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
        bcm_l3_ingress_t ing_intf;
        bcm_l3_ingress_t_init(&ing_intf);
        bcm_if_t intf_id;

        ing_intf.flags = BCM_L3_INGRESS_WITH_ID;        /* must, as we update exist RIF */
        ing_intf.vrf   = cint_vxlan_basic_info.eth_rif_vm_vrf;

        /*
         * Convert tunnel's GPort ID to intf ID 
         */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);
        rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ingress_create\n");
            return rv;
        }
    }

    printf("tunnel term tunnel id: 0x%x\n", tunnel_term.tunnel_id);

    return rv;
}

/*
 * Create and configure VXLAN tunnel encapsulation
 */
int vxlan_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t *itf)
{
    int rv;
    int device_is_jericho2;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2 device_is_jericho2 \n");
        return rv;
    }

    bcm_l3_intf_t_init(&l3_intf);

    /* Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = cint_vxlan_basic_info.tunnel_dip;
    tunnel_init.sip        = cint_vxlan_basic_info.tunnel_init_sip;
    tunnel_init.flags      = 0;
    tunnel_init.dscp       = 10;
    tunnel_init.type       = cint_vxlan_basic_info.vxlan_tunnel_type;
    tunnel_init.l3_intf_id = arp_itf;
    tunnel_init.ttl        = 64;
    if(device_is_jericho2)
    {
        tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    }
    else
    {
        tunnel_init.dscp_sel = bcmTunnelDscpAssign;
        tunnel_init.vlan = cint_vxlan_basic_info.core_vlan; /* For JR2 the vlan is set at LL*/
    }
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */

    /* if we learn sym outlif + port or outlif + FEC, the outlif is symmetric. 
       so we have tunnel init outlif = tunnel term inlif */ 
    if ((cint_vxlan_basic_info.learn_mode == LEARN_SYM_OUTLIF_PORT)
    || (cint_vxlan_basic_info.learn_mode == LEARN_SYM_OUTLIF_FEC)){
        BCM_GPORT_TUNNEL_ID_SET(tunnel_init.tunnel_id, cint_vxlan_basic_info.tunnel_term_id);
    }
    else {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_init.tunnel_id, cint_vxlan_basic_info.tunnel_init_id);
    }
    tunnel_init.flags |= BCM_TUNNEL_WITH_ID;



    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
        return rv;
    }

    *itf = l3_intf.l3a_intf_id;
    cint_vxlan_basic_info.tunnel_id = tunnel_init.tunnel_id;

    printf("tunnel encap tunnel id: 0x%x \n", cint_vxlan_basic_info.tunnel_id);
    printf("tunnel encap l3 itf: 0x%x \n", *itf);

    return rv;
}

/*
 * Ports Initialization
 * The function vxlan_ports_add configures VXLAN, VLAN and VSWITCH ports with the Out-Lif
 * Inputs: unit - relevant unit;
 *         core_port - physical port;
 *         vm_port - virtual machine port;
 *         tunnel_id - VXLAN tunnel id;
 *         *vxlan_port_id - VXLAN port id;
 *         *proc_name - main function name;
 */
int vxlan_ports_add(
    int unit,
    int core_port,
    int vm_port,
    bcm_gport_t tunnel_id,
    bcm_gport_t *vxlan_port_id,
    char* proc_name)
{
    int rv = BCM_E_NONE;
    int vxlan_port_flags = 0;
    int device_is_jericho2;
    vxlan_port_add_s vxlan_port_add;
    bcm_gport_t vlan_port_id;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, is_device_jericho2\n", proc_name);
        return rv;
    }


    /* Add VXLAN port properties and configure tunnel terminator In-Lif and initiatior Out-Lif*/
    vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn              = cint_vxlan_basic_info.vpn_id;
    vxlan_port_add.in_tunnel        = tunnel_id;
    vxlan_port_add.flags            = vxlan_port_flags;
    vxlan_port_add.network_group_id = cint_vxlan_basic_info.vxlan_network_group_id;
    vxlan_port_add.in_port          = core_port;

    if (cint_vxlan_basic_info.learn_mode == LEARN_FEC_ONLY) {
        BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_basic_info.encap_fec_id);
    }
    else if (cint_vxlan_basic_info.learn_mode == LEARN_SYM_OUTLIF_PORT) {
        vxlan_port_add.out_tunnel       = cint_vxlan_basic_info.tunnel_id;
    }
    else if (cint_vxlan_basic_info.learn_mode == LEARN_SYM_OUTLIF_FEC) {
        BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_basic_info.encap_arp_fec_id);
        vxlan_port_add.out_tunnel       = cint_vxlan_basic_info.tunnel_id;
    }


    printf("Before vxlan port add: update vxlan outlif\n");
    
    vxlan__vxlan_port_add(unit, vxlan_port_add);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan__vxlan_port_add, in_gport=0x%08x\n", proc_name, tunnel_id);
        return rv;
    }

    *vxlan_port_id = vxlan_port_add.vxlan_port_id;

    if (!device_is_jericho2) {
        /** Configure the Outgoing port TPID properties. */
        port_tpid_init(vm_port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, port_tpid_set \n", proc_name);
            return rv;
        }

        /* Configure the Incomming port TPID properties. */
        port_tpid_init(core_port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, port_tpid_set \n", proc_name);
            return rv;
        }
    }

    rv = vlan_port_add(unit, vm_port, &vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vswitch_metro_add_port_1 2, in_intf=0x%08x\n", proc_name, vlan_port_id);
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, cint_vxlan_basic_info.vpn_id, vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vswitch_port_add\n", proc_name);
        return rv;
    }

    if (!device_is_jericho2)
    {
        /*
         * update Multicast to have the added port 
         */
        rv = multicast__vlan_port_add(unit, cint_vxlan_basic_info.vpn_id, vm_port, vlan_port_id, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, multicast__vlan_port_add\n");
            return rv;
        }
    }

    return rv;
}


int
vxlan_port_add(
    int unit,
    int core_port,
    int tunnel_term_id,
    int tunnel_encapsulation_id,
    bcm_gport_t * vxlan_port_id)
{
    int rv = BCM_E_NONE;

    vxlan_port_add_s vxlan_port_add;
    vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn        = cint_vxlan_basic_info.vpn_id;
    vxlan_port_add.in_port    = core_port;
    vxlan_port_add.in_tunnel  = tunnel_term_id;
    vxlan_port_add.out_tunnel = tunnel_encapsulation_id;
    vxlan_port_add.flags      = 0;
    vxlan_port_add.network_group_id = cint_vxlan_basic_info.vxlan_network_group_id;
    BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_basic_info.encap_fec_id);

    
    rv = vxlan__vxlan_port_add(unit, vxlan_port_add);
    *vxlan_port_id = vxlan_port_add.vxlan_port_id;

    return BCM_E_NONE;
}

/*
 * Create and configure VLAN port
 */
int vlan_port_add(
    int unit,
    bcm_gport_t in_port,
    bcm_gport_t *port_id)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);

    /* the match criteria is port:1, out-vlan:510   */
    vlan_port.criteria    = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port        = in_port;
    vlan_port.match_vlan  = cint_vxlan_basic_info.virtual_vlan;
    vlan_port.flags       = 0;
    vlan_port.vsi         = 0;

    rv = bcm_vlan_port_create(unit,&vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    /* handle of the created gport */
    *port_id = vlan_port.vlan_port_id;

    return rv;
}


/*
 * Create an MACT entry for VXLAN tunnel, mapped to the VPN 
 */
int
vxlan_l2_addr_add (
    int unit,
    bcm_mac_t l2_vm_dmac_addr,
    bcm_gport_t vxlan_port_id)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr2;

    bcm_l2_addr_t_init(&l2addr2, l2_vm_dmac_addr, cint_vxlan_basic_info.vpn_id);
    l2addr2.port       = vxlan_port_id;
    l2addr2.mac        = l2_vm_dmac_addr;
    l2addr2.vid        = cint_vxlan_basic_info.vpn_id;
    l2addr2.l2mc_group = cint_vxlan_basic_info.vpn_id;
    l2addr2.flags      = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr2);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_l2_addr_add\n", proc_name);
        return rv;
    }

    return rv;
}

