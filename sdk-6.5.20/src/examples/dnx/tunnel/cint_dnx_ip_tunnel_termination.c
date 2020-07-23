/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_ip_tunnel_termination.c Purpose: Various examples for IPTunnels in Jericho 2.
 */

/*
 * Examples of Jericho2-only tunnel termination
 *
 * Scenarios configured in this CINT:
 *
 *     1. Ipv4 termination with Unicast RPF check on the native Ipv4 SIP: loose and strict
 *        (ipv4_tunnel_termination_with_uc_rpf)
 *
 *     2. Ipv4 termination with Multicast RPF check on the native Ipv4 SIP: explicit and SIP-based
 *        (ipv4_tunnel_termination_with_mc_rpf)
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/cint_ip_route_rpf_basic.c
 * cint ../../../../src/examples/dnx/tunnel/cint_dnx_ip_tunnel_termination.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int provider_port = 200;
 * int access_port = 201;
 * rv = ipv4_tunnel_termination_with_mc_rpf(unit, provider_port, access_port_1, access_port_2, vid);
 * print rv;
 */
struct cint_ip_tunnel_basic_info_s
{
    int eth_rif_intf_provider;          /* in RIF */
    int eth_rif_intf_access;            /* out RIF */
    bcm_mac_t intf_provider_mac_address;/* mac for in RIF */
    bcm_mac_t intf_access_mac_address;  /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac;         /* mac for next hop */
    int access_vrf;                     /* router instance VRF resolved at VTT1 */
    int provider_vrf;                   /* VRF after termination, resolved as Tunn-InLif property */
    int fec_id;                         /* FEC id */
    int arp_id;                         /* Id for ARP entry */
    int ipmc_group;                     /* IP multicast group id */
    bcm_ip_t route_dip;                 /* Route DIP */
    bcm_ip_t mask;                      /* mask for route DIP */
    bcm_ip_t ipmc_dip;                  /* DIP multicast adress */
    bcm_ip_t mrpf_sip;                  /* SIP for MC RPF check */
    bcm_ip_t mrpf_sip_mask;             /* SIP mask for MC RPF check */
    bcm_ip_t tunnel_dip;                /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;           /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;           /* tunnel SIP mask */
    int tunnel_type;                    /* bcmTunnelType */
    int mpls_fwd_label;                 /* MPLS forwarding label */
    bcm_ip6_t ipv6_dip;                 /* ipv6 forwarding dip */
    uint32    vni_v4;                   /* vni used in ipv4 termination */
    uint32    vni_v6;                   /* vni used in ipv6 termination */
    bcm_gport_t tunnel_id_v4;           /* tunnel gport of Vxlan tunnel terminator */
    bcm_gport_t tunnel_id_v6;           /* tunnel gport of Vxlan6 tunnel terminator */
    int priority;
};

cint_ip_tunnel_basic_info_s cint_ip_tunnel_basic_info = {
    /*
     * eth_rif_intf_provider | eth_rif_intf_access
     */
    15, 100,
    /*
     * intf_provider_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /*
     * intf_access_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01},
    /*
     * arp_next_hop_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * access_vrf, provider_vrf
     */
    5, 1,
    /*
     * fec_id
     */
    40961,
    /*
     * arp_id
     */
    0,
    /*
     * ipmc_group
     */
    6000,
    /*
     * route_dip (Ipv4)
     */
    0xA0A1A1A2, /* 160.161.161.162 */
    /*
     * mask (Ipv4 route entry )
     */
    0xfffffff0,
    /*
     * ipmc_dip (Ipv4)
     */
    0xe0020202, /** 224.2.2.2 */
    /*
     * mrpf_sip
     */
    0xc0800100,
    /*
     * mrpf_sip_mask
     */
    0,
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
     * tunnel_type
     */
    bcmTunnelTypeGreAnyIn4,
    /*
     * mpls_fwd_label
     */
    1453,
    /*
     * ipv6_dip
     */
    {
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xff, 0x13
    },
    /*
     * vni_v4 | vni_v6 | tunnel_intf_v4 | tunnel_intf_v4 | priority
     */
    5000, 5001, 0, 0, 0
 };

/* configure tunnel termination lookups:
   ISEM DIP, SIP, VRF, tunnel_type  -> inLif
   TCAM DIP, SIP, VRF, tunnel_type  -> inLif
   ISEM DIP,      VRF, tunnel_type  -> inLif*/
struct cint_ip_tunnel_termination_priority_info_s {
    bcm_ip_t tunnel_dip;                /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;           /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;           /* tunnel SIP mask */
    int access_vrf;                     /* property of tunnel termination inlif */
    bcm_mac_t next_hop;                 /* next hop ethernet DA (used to identify which tunnel termination happened */
    int priority;
};


int nof_tunnel_termination_priorities = 8;
cint_ip_tunnel_termination_priority_info_s cint_ip_tunnel_termination_priorities_info[8] = {
    /* Configure tunnel termination so that when sending a IPv4oGREoIPV4oETH,
       all 3 tunnels are candidate for termination: 
            ISEM DIP, SIP, VRF, tunnel_type  -> inLif1 -> VRF (1). FWD: VRF (1) x DIP -> LL (DA1), ETH-RIF, port
            TCAM DIP, SIP, VRF, tunnel_type  -> inLif2 -> VRF (2). FWD: VRF (2) x DIP -> LL (DA2), ETH-RIF, port
            ISEM DIP,      VRF, tunnel_type  -> inLif3 -> VRF (3). FWD: VRF (3) x DIP -> LL (DA3), ETH-RIF, port */

    /* configure ISEM DIP1, SIP1, VRF, tunnel_type  -> inLif -> VRF (1) */
    {
        /* tunnel DIP */
        0xA0A0A0A0, /* 160.160.160.160 */
        /* tunnel DIP mask */
        0xFFFFFFFF, 
        /* tunnel SIP */
        0xAC000001, /*172.0.0.1 */
        /* tunnel SIP mask */
        0xFFFFFFFF, 
        /* VRF */
        11, 
        /* next hop */
        {0x00, 0x00, 0x00, 0x00, 0xaa, 0xaa}, 0
    }, 
    /* configure TCAM DIP, SIP, VRF, tunnel_type  -> inLif -> VRF (2) */
    {
        /* tunnel DIP: 160.160.160.160 */
        0xA0A0A0A0, /*  */
        /* tunnel DIP mask: 255.255.255.240 */
        0xFFFFFFF0, 
        /* tunnel SIP 172.0.0.1 */
        0xAC000001, 
        /* tunnel SIP mask */
        0xFFFFFFFF, 
        /* VRF */
        12, 
        /* next hop */
        {0x00, 0x00, 0x00, 0x00, 0xbb, 0xbb}, 0
    }, 
    /* configure ISEM DIP, VRF, tunnel_type  -> inLif -> VRF (3) */
    {
        /* tunnel DIP */
        0xA0A0A0A0, /* 160.160.160.160 */
        /* tunnel DIP mask */
        0xFFFFFFFF, 
        /* tunnel SIP */
        0x0, 
        /* tunnel SIP mask */
        0x0, 
        /* VRF */
        13, 
        /* next hop */
        {0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc}, 0
    }, 


    /* Configure tunnel termination so that when sending a IPv4oGREoIPV4oETH,
       2 tunnels are candidate for termination: 
            TCAM DIP, SIP, VRF, tunnel_type  -> inLif2 -> VRF (2)  FWD: VRF (2) x DIP -> LL (DA2), ETH-RIF, port 
            ISEM DIP,      VRF, tunnel_type  -> inLif3 -> VRF (3)  FWD: VRF (3) x DIP -> LL (DA3), ETH-RIF, port  */
    /* configure TCAM DIP, SIP, VRF, tunnel_type  -> inLif -> VRF (2) */
    {
        /* tunnel DIP: 160.160.170.160 */
        0xA0A0AAA0, 
        /* tunnel DIP mask: 255.255.255.240 */
        0xFFFFFFF0, 
        /* tunnel SIP 172.0.0.1 */
        0xAC000001,
        /* tunnel SIP mask */
        0xFFFFFFFF, 
        /* VRF */
        12, 
        /* next hop */
        {0x00, 0x00, 0x00, 0x00, 0xbb, 0xbb}, 0
    }, 

    /* configure ISEM DIP, VRF, tunnel_type  -> inLif -> VRF (3) */
    {
        /* tunnel DIP */
        0xA0A0AAA0, /* 160.160.170.160 */
        /* tunnel DIP mask */
        0xFFFFFFFF, 
        /* tunnel SIP */
        0x0, 
        /* tunnel SIP mask */
        0x0, 
        /* VRF */
        13, 
        /* next hop */
        {0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc}, 0
    }, 

    /* Configure tunnel termination so that when sending a IPv4oGREoIPV4oETH,
       1 tunnels is candidate for termination:                     
            ISEM DIP,      VRF, tunnel_type  -> inLif3 -> VRF (3)   FWD: VRF (3) x DIP -> LL (DA3), ETH-RIF, port  */
    /* configure ISEM DIP, VRF, tunnel_type  -> inLif -> VRF (3) */
    {
        /* tunnel DIP */
        0xA0A0ABA0, /* 160.160.171.160 */
        /* tunnel DIP mask */
        0xFFFFFFFF, 
        /* tunnel SIP */
        0x0, 
        /* tunnel SIP mask */
        0x0, 
        /* VRF */
        13,
        /* next hop */
        {0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc}, 0
    },

    {
        /* tunnel DIP: 180.160.170.161 */
        0xB4A0AAA1,
        /* tunnel DIP mask: 255.255.255.240 */
        0xFFFFFFFF,
        /* tunnel SIP 172.0.0.1 */
        0xAC000001,
        /* tunnel SIP mask */
        0xFFFFFF00,
        /* VRF */
        16,
        /* next hop */
        {0x00, 0x00, 0x00, 0x00, 0xbb, 0xbb}, 3
    },
    {
        /* tunnel DIP: 180.160.170.161 */
        0xB4A0AAA1,
        /* tunnel DIP mask: 255.255.255.240 */
        0xFFFFFFF0,
        /* tunnel SIP 172.0.0.1 */
        0xAC000001,
        /* tunnel SIP mask */
        0xFFFFFF00,
        /* VRF */
        15,
        /* next hop */
        {0x00, 0x00, 0x00, 0x00, 0xaa, 0xbb}, 2
    }
};

/*
struct cint_ip_tunnel_termination_priority_infos_s {
      Configure tunnel termination so that when sending a IPv4oGREoIPV4oETH,
       all 3 tunnels are candidate for termination: 
            ISEM DIP, SIP, VRF, tunnel_type  -> inLif1 -> VRF (1)
            TCAM DIP, SIP, VRF, tunnel_type  -> inLif2 -> VRF (2)
            ISEM DIP,      VRF, tunnel_type  -> inLif3 -> VRF (3)  
    cint_ip_tunnel_termination_priority_info_s three_candidates_dip_sip_vrf_tunnel_type_isem_config; 
    cint_ip_tunnel_termination_priority_info_s three_candidates_dip_sip_vrf_tunnel_type_tcam_config; 
    cint_ip_tunnel_termination_priority_info_s three_candidates_dip_vrf_tunnel_type_isem_config; 
      Configure tunnel termination so that when sending a IPv4oGREoIPV4oETH,
       2 tunnels are candidate for termination: 
            TCAM DIP, SIP, VRF, tunnel_type  -> inLif2 -> VRF (2)
            ISEM DIP,      VRF, tunnel_type  -> inLif3 -> VRF (3)  
    cint_ip_tunnel_termination_priority_info_s two_candidates_dip_sip_vrf_tunnel_type_tcam_config; 
    cint_ip_tunnel_termination_priority_info_s two_candidates_dip_vrf_tunnel_type_isem_config; 
      Configure tunnel termination so that when sending a IPv4oGREoIPV4oETH,
       1 tunnels is candidate for termination: 
            ISEM DIP,      VRF, tunnel_type  -> inLif3 -> VRF (3)  
    cint_ip_tunnel_termination_priority_info_s one_candidates_dip_vrf_tunnel_type_isem_config; 
};
*/


/*
 * Interfaces initialisation function for ip tunnel termination scenarios in Jericho 2
 * The function ip_tunnel_termination_open_route_interfaces implements the basic configuration,
 * on top of witch all tunnel terminators are built. All tunnel termination scenarios will call it.
 * Inputs:   unit          - relevant unit;
 *           provider_port - incoming port;
 *           access_port   - outgoing port;
 */
int
ip_tunnel_termination_open_route_interfaces(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "ip_tunnel_termination_open_route_interfaces";

    /*
     * Configure ingress interfaces
     */
    rv = ip_tunnel_termination_open_route_interfaces_provider(unit, provider_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_open_route_interfaces_provider \n", proc_name);
        return rv;
    }

    /*
     * Configure egress interfaces
     */
    rv = ip_tunnel_termination_open_route_interfaces_access(unit, access_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_open_route_interfaces_access \n", proc_name);
        return rv;
    }

    /*
     * Create ARP entry and set its properties
     */
    
    rv = l3__egress_only_encap__create(unit, 0, &(cint_ip_tunnel_basic_info.arp_id),
                                       cint_ip_tunnel_basic_info.arp_next_hop_mac,
                                       cint_ip_tunnel_basic_info.eth_rif_intf_access);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_encap__create \n", proc_name);
        return rv;
    }

    /* dummy entry */
    int arp_id_dummy = 0; 
    rv = l3__egress_only_encap__create(unit, 0, &(arp_id_dummy),
                                       cint_ip_tunnel_basic_info.arp_next_hop_mac, 
                                       cint_ip_tunnel_basic_info.eth_rif_intf_access);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_encap__create \n", proc_name);
        return rv;
    }


    return rv;
}

/*
 * Configure Ingress interfaces
 * Inputs: provider_port - incomming port;
 *         *proc_name    - main function name;
 */
int
ip_tunnel_termination_open_route_interfaces_provider(
    int unit,
    int provider_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;
    l3_ingress_intf ingr_itf;

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, provider_port, cint_ip_tunnel_basic_info.eth_rif_intf_provider);
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
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_provider,
                             cint_ip_tunnel_basic_info.intf_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_provider \n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = cint_ip_tunnel_basic_info.eth_rif_intf_provider;
    ingr_itf.vrf = cint_ip_tunnel_basic_info.provider_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set \n", proc_name);
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
ip_tunnel_termination_open_route_interfaces_access(
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
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_access,
                             cint_ip_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_access\n", proc_name);
        return rv;
    }

    return rv;
}

/* Main function for VXLAN-GPE IP tunnel termination with MPLS forwarding on Jericho 2
 * The function implements VXLANoUDPoIPv4 tunnel termination on MPLSoVXLANoUDPoIPv4oETH packet
 * Inputs: unit            - relevant unit;
 *         provider_port   - incoming port;
 *         access_port     - outgoing port;
 */
int
ip_tunnel_term_vxlan_gpe_mpls_fwd(
    int unit,
    int provider_port,
    int access_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    bcm_if_t tunnel_intf;
    bcm_if_t mpls_intf_id;
    cint_ip_tunnel_basic_info.tunnel_type = bcmTunnelTypeVxlanGpe;

    proc_name = "ip_tunnel_term_vxlan_gpe_mpls_fwd";

    /** Initialize ports, LIFs, Eth-RIFs etc */
    rv = ip_tunnel_termination_open_route_interfaces(unit, provider_port, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_open_route_interfaces\n", proc_name);
        return rv;
    }

    /** Create VXLAN-GPE IP tunnel terminator */
    rv = tunnel_terminator_create(unit, &tunnel_intf, &cint_ip_tunnel_basic_info.tunnel_id_v4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_terminator_create\n", proc_name);
        return rv;
    }

    /** Add MPLS route to IP-Tunnel.*/
    rv = ip_tunnel_termination_create_mpls_swap_tunnel(unit, &mpls_intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_create_mpls_swap_tunnel\n", proc_name);
        return rv;
    }

    /** Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, mpls_intf_id, 0,
                                     access_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create\n", proc_name);
        return rv;
    }

    /** Create mpls LSF for port pointed by FEC, pointed by ECMP group id */
    rv = ip_tunnel_termination_ilm_switch_tunnel_create(unit,
                                                        cint_ip_tunnel_basic_info.mpls_fwd_label,
                                                        cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_ilm_switch_tunnel_create\n", proc_name);
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


/*
 * Main function for validating tunnel termination lookup priorities. 
 * We'll consider GRE4 tunnel termination and IPV4 fwd. 
 * It's expected that: 
 * 1) ISEM lookup: DIP, SIP, VRF, Tunnel-type is highest priority 
 * 2) TCAM lookup: DIP, SIP, VRF, Tunnel-type is medium priority 
 * 3) ISEM lookup: DIP, VRF, Tunnel-type is lowest priority 
 * To distinguish between result, we'll have different inLif per tunnel terminatio result, 
 * where is configured different VRFs. Then at fwd, we'll have different RIF ? LL ? which will build distinct ETH headers. 
 * ISEM_1 -> inLif1 -> VRF_1     VRF_1 x DIP -> RIF, LL 
 * TCAM_2 -> inLif2 -> VRF_2     VRF_2 x DIP -> RIF, LL 
 * ISEM_3 -> inLif3 -> VRF_3     VRF_3 x DIP -> RIF, LL 
 */
int
ip_tunnel_term_lookup_priorities(
    int unit,
    int provider_port,
    int access_port)
{
    char *proc_name = "ip_tunnel_term_lookup_priorities";
    int rv = BCM_E_NONE;

    bcm_if_t tunnel_intf;

    /** Initialize ports, LIFs, Eth-RIFs etc */
    rv = ip_tunnel_termination_open_route_interfaces(unit, provider_port, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_termination_open_route_interfaces\n", proc_name);
        return rv;
    }

    /** Set Out-Port default properties */
    rv = out_port_set(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set, access_port\n", proc_name);
        return rv;
    }
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /* Configure tunnel termination lookup.
       More than one tunnel will be candidate. To identify the successful tunnel, we'll have a different LL.DA */
    int i; 
    for (i=0; i< nof_tunnel_termination_priorities; i++) {

        cint_ip_tunnel_basic_info.tunnel_dip = cint_ip_tunnel_termination_priorities_info[i].tunnel_dip;
        cint_ip_tunnel_basic_info.tunnel_dip_mask = cint_ip_tunnel_termination_priorities_info[i].tunnel_dip_mask;
        cint_ip_tunnel_basic_info.tunnel_sip = cint_ip_tunnel_termination_priorities_info[i].tunnel_sip;
        cint_ip_tunnel_basic_info.tunnel_sip_mask = cint_ip_tunnel_termination_priorities_info[i].tunnel_sip_mask;
        cint_ip_tunnel_basic_info.access_vrf = cint_ip_tunnel_termination_priorities_info[i].access_vrf;
        cint_ip_tunnel_basic_info.priority = cint_ip_tunnel_termination_priorities_info[i].priority;

        /* add tunnel termination entry
           create a tunnel termination inlif
           configure the VRF of tunnel termination inlif
           */
        rv = tunnel_terminator_create(unit, &tunnel_intf, &cint_ip_tunnel_basic_info.tunnel_id_v4);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, tunnel_terminator_create\n", proc_name);
            return rv;
        }

        
        /* create ARP entry: one per candidate */
        int arp_id; 
        rv = l3__egress_only_encap__create(unit, 0, &(arp_id),
                                           cint_ip_tunnel_termination_priorities_info[i].next_hop, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_encap__create \n", proc_name);
            return rv;
        }
        /* dummy entry */
        int arp_id_dummy = 0; 
        rv = l3__egress_only_encap__create(unit, 0, &(arp_id_dummy),
                                           cint_ip_tunnel_termination_priorities_info[i].next_hop, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, l3__egress_only_encap__create \n", proc_name);
            return rv;
        }

        /** Create FEC  */
        cint_ip_tunnel_basic_info.fec_id++;
        cint_ip_tunnel_basic_info.fec_id++;

        bcm_gport_t gport; 
        BCM_GPORT_LOCAL_SET(gport, access_port);
        rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id, 
                                         cint_ip_tunnel_basic_info.eth_rif_intf_access, 
                                         arp_id, gport, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, l3__egress_only_fec__create \n");
            return rv;
        }

        /** Create fwd */
        rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip, 
                           cint_ip_tunnel_termination_priorities_info[i].access_vrf,
                           cint_ip_tunnel_basic_info.fec_id, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, add_host_ipv4, route_dip \n");
            return rv;
        }

    }
    return rv;
}




/*
 * Main function for VXLAN-GPE IP tunnel termination with IPv6 forwarding on Jericho 2
 * The function implements VXLANoUDPoIPv4 tunnel termination on IPv6oVXLANoUDPoIPv4oETH packet
 * Inputs: unit          - relevant unit;
 *         provider_port - incoming port;
 *         access_port   - outgoing port;
 */
int
ip_tunnel_term_vxlan_gpe_ipvx_fwd_strict_uc_rpf(
    int unit,
    int provider_port_v4,
    int provider_port_v6,
    int access_port)
{
    char *proc_name;
    proc_name = "ip_tunnel_term_vxlan_gpe_ipvx_fwd_strict_uc_rpf";
    int rv                 = BCM_E_NONE;
    int eth_rif_intf_in_v4 = 15;
    int eth_rif_intf_in_v6 = 20;
    int provider_vrf_v4    = 100;
    int provider_vrf_v6    = 200;
    int access_vrf_v4 = 10;
    int access_vrf_v6 = 20;
    int fec_rpf_strict_v4;
    int fec_rpf_strict_v6;
    bcm_gport_t gport      = 0;
    bcm_if_t tunnel_intf_v4 = 0;
    bcm_if_t tunnel_intf_v6 = 0;
    l3_ingress_intf ingr_itf;
    bcm_ip_t route_sip_rpf_hit_rif_miss_v4  = 0xc0800101; /* 192.128.1.1 */
    bcm_ip_t route_sip_rpf_hit_rif_hit_v4   = 0xc0800102; /* 192.128.1.2 */
    bcm_ip6_t route_sip_rpf_hit_rif_miss_v6 = {0xCE, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9,
                                               0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1};
    bcm_ip6_t route_sip_rpf_hit_rif_hit_v6  = {0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9,
                                               0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1};

    cint_ip_tunnel_basic_info.tunnel_type   = bcmTunnelTypeVxlanGpe;

    cint_ip_tunnel_basic_info.tunnel_sip_mask = 0x00000000; 

    /** Configure traps  */
    rv = l3_dnx_ip_rpf_config_traps(&unit, 1);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_rpf_config_traps\n");
        return rv;
    }
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    fec_rpf_strict_v4 = cint_ip_tunnel_basic_info.fec_id + 2;
    fec_rpf_strict_v6 = fec_rpf_strict_v4 + 2;

    /** Set In-Port to In ETh-RIF: tunnel providers for RPF with IPv4 and IPv6 forwarding */
    rv = in_port_intf_set(unit, provider_port_v4, eth_rif_intf_in_v4);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set eth_rif_intf_in_v4 \n");
        return rv;
    }
    rv = in_port_intf_set(unit, provider_port_v6, eth_rif_intf_in_v6);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set eth_rif_intf_in_v6 \n");
        return rv;
    }

    /** Set Out-Port default properties */
    rv = out_port_set(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set, access_port\n", proc_name);
        return rv;
    }

    /*
     * Create routing interfaces for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure),
     * to enable IPv4 and IPv6 routing for this ETH-RIF and to set the VRF.
     */
    /** Strict RPF Providers */
    rv = intf_eth_rif_create(unit, eth_rif_intf_in_v4,
                             cint_ip_tunnel_basic_info.intf_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create eth_rif_intf_in_v4 \n");
        return rv;
    }
    rv = intf_eth_rif_create(unit, eth_rif_intf_in_v6,
                             cint_ip_tunnel_basic_info.intf_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create eth_rif_intf_in_v6 \n");
        return rv;
    }
    /** Acess */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_access,
                             cint_ip_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_access\n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = eth_rif_intf_in_v4;
    ingr_itf.vrf = provider_vrf_v4;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set, eth_rif_intf_in_v4 \n");
        return rv;
    }
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = eth_rif_intf_in_v6;
    ingr_itf.vrf = provider_vrf_v6;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set, eth_rif_intf_in_v6 \n");
        return rv;
    }

    /** Create VXLAN-GPE IP tunnel terminators */
    cint_ip_tunnel_basic_info.provider_vrf = provider_vrf_v4;

    rv = tunnel_terminator_create(unit, &tunnel_intf_v4, &cint_ip_tunnel_basic_info.tunnel_id_v4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_terminator_create, provider_vrf_v4 \n", proc_name);
        return rv;
    }
    cint_ip_tunnel_basic_info.provider_vrf = provider_vrf_v6;

    rv = tunnel_terminator_create(unit, &tunnel_intf_v6, &cint_ip_tunnel_basic_info.tunnel_id_v6);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, tunnel_terminator_create, provider_vrf_v6 \n", proc_name);
        return rv;
    }

     /** Set Tunnel termination In-LIFs properties for RPF forwarding */
    rv = intf_ingress_rif_rpf_set(unit, tunnel_intf_v4, provider_vrf_v4,
                                  bcmL3IngressUrpfStrict);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_rpf_set, tunnel_intf_v4 \n");
        return rv;
    }
    rv = intf_ingress_rif_rpf_set(unit, tunnel_intf_v6, provider_vrf_v6,
                                  bcmL3IngressUrpfStrict);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_rpf_set, tunnel_intf_v6 \n");
        return rv;
    }

    /** Create ARP entry and set its properties */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_ip_tunnel_basic_info.arp_id),
                                       cint_ip_tunnel_basic_info.arp_next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_encap__create \n", proc_name);
        return rv;
    }

    /** Create FEC and configure its: Outlif, Destination port, Global Out-EthRIF */
    BCM_GPORT_LOCAL_SET(gport, access_port);
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id,
                                     cint_ip_tunnel_basic_info.eth_rif_intf_access,
                                     cint_ip_tunnel_basic_info.arp_id, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, l3__egress_only_fec__create, fec_id\n", proc_name);
        return rv;
    }
    /** FECs for RPF strict */
    rv = l3__egress_only_fec__create(unit, fec_rpf_strict_v4, tunnel_intf_v4, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3__egress_only_fec__create, fec_rpf_strict_v4 \n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec_rpf_strict_v6, tunnel_intf_v6, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3__egress_only_fec__create, fec_rpf_strict_v6 \n");
        return rv;
    }

    /** IPv4 DIP forwarding entry */
    rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip, access_vrf_v4,
                       cint_ip_tunnel_basic_info.fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv4, route_dip \n");
        return rv;
    }
    /** Strict RPF IPv4 route entries - hit and miss */
    rv = add_host_ipv4(unit, route_sip_rpf_hit_rif_miss_v4, access_vrf_v4,
                       cint_ip_tunnel_basic_info.fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_host_ipv4, route_sip_rpf_hit_rif_miss_v4 \n", proc_name);
        return rv;
    }
    rv = add_host_ipv4(unit, route_sip_rpf_hit_rif_hit_v4, access_vrf_v4, fec_rpf_strict_v4, 0,0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_host_ipv4, route_sip_rpf_hit_rif_hit_v4 \n", proc_name);
        return rv;
    }

    /** IPv6 DIP forwarding entry */
    rv = add_host_ipv6(unit, cint_ip_tunnel_basic_info.ipv6_dip, access_vrf_v6,
                       cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv6, ipv6_dip \n");
        return rv;
    }
    /** Strict RPF IPv6 route entries - hit and miss */
    rv = add_host_ipv6(unit, route_sip_rpf_hit_rif_miss_v6, access_vrf_v6,
                       cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv6, route_sip_rpf_hit_rif_miss_v6 \n");
        return rv;
    }
    rv = add_host_ipv6(unit, route_sip_rpf_hit_rif_hit_v6, access_vrf_v6, fec_rpf_strict_v6);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv6, route_sip_rpf_hit_rif_hit_v6 \n");
        return rv;
    }
    rv = vxlan_network_domain_management_l3_ingress(unit, cint_ip_tunnel_basic_info.vni_v4, access_vrf_v4);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vxlan_network_domain_management_l3_ingress \n");
        return rv;
    }
    rv = vxlan_network_domain_management_l3_ingress(unit, cint_ip_tunnel_basic_info.vni_v6, access_vrf_v6);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vxlan_network_domain_management_l3_ingress \n");
        return rv;
    }

    return rv;
}

/*
 * Function that creates the general-case tunnel terminator.
 * Inputs: unit - relevant unit;
 */
int
tunnel_terminator_create(
    int unit,
    bcm_if_t *tunnel_inlif_id,
    bcm_gport_t *tunnel_id)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /** Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = cint_ip_tunnel_basic_info.tunnel_dip;
    tunnel_term.dip_mask = cint_ip_tunnel_basic_info.tunnel_dip_mask;
    tunnel_term.sip = cint_ip_tunnel_basic_info.tunnel_sip;
    tunnel_term.sip_mask = cint_ip_tunnel_basic_info.tunnel_sip_mask;
    tunnel_term.vrf = cint_ip_tunnel_basic_info.provider_vrf;
    tunnel_term.type = cint_ip_tunnel_basic_info.tunnel_type;
    if (cint_ip_tunnel_basic_info.priority > 0)
    {
        tunnel_term.priority = cint_ip_tunnel_basic_info.priority;
    }
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */

    /** For VXLAN-GPE, no need to update the VRF.
     *  VRF is retrieved from VNI->VRF lookup  */ 
    if ((cint_ip_tunnel_basic_info.tunnel_type != bcmTunnelTypeVxlanGpe) 
        && (cint_ip_tunnel_basic_info.tunnel_type != bcmTunnelTypeVxlan6Gpe)){
        bcm_l3_ingress_t ing_intf;
        bcm_l3_ingress_t_init(&ing_intf);
        bcm_if_t intf_id;

        ing_intf.flags = BCM_L3_INGRESS_WITH_ID;    /* must, as we update exist RIF */
        ing_intf.vrf = cint_ip_tunnel_basic_info.access_vrf;

        /** Convert tunnel's GPort ID to intf ID */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);

        rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ingress_create\n");
            return rv;
        }
        *tunnel_inlif_id = intf_id;
    }
    else {
        /** Convert tunnel's GPort ID to intf ID */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(*tunnel_inlif_id, tunnel_term.tunnel_id);
    }
    *tunnel_id = tunnel_term.tunnel_id;
    return rv;
}

int
ip_tunnel_termination_ilm_switch_tunnel_create(
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
ip_tunnel_termination_create_mpls_swap_tunnel(
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
    label_array[0].label = cint_ip_tunnel_basic_info.mpls_fwd_label;

    /*
     * Set the next pointer of this entry to be the arp. This configuration is new compared to
     * Jericho, where the arp pointer used to be connected to the EEDB entry via
     * bcm_l3_egress_create (with egress flag indication).
     */
    label_array[0].l3_intf_id = cint_ip_tunnel_basic_info.arp_id;

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

/*
* Main function for Ipv4in4 tunnel termination scenario with Unicast RPF checks in Jericho 2
* Inputs:
* unit                 - relevant unit;
* provider_port_loose  - incoming port rpf loose;
* provider_port_strict - incoming port rpf strict;
* access_port          - outgoing port;
* vsi_loose            - vsi -> In-Eth-RIF for rpf loose;
* vsi_strict           - vsi -> In-Eth-RIF for rpf strict;
* provider_vrf_loose   - vrf for rpf loose;
* provider_vrf_strict  - vrf for rpf strict;
*/
int
ipv4_tunnel_termination_with_uc_rpf(
    int unit,
    int provider_port_loose,
    int provider_port_strict,
    int access_port,
    int vsi_loose,
    int vsi_strict,
    int provider_vrf_loose,
    int provider_vrf_strict)
{
    char *proc_name;
    proc_name = "ipv4_tunnel_termination_with_uc_rpf";
    int rv                     = BCM_E_NONE;
    int eth_rif_intf_in_loose  = vsi_loose;
    int eth_rif_intf_in_strict = vsi_strict;
    int fec_rpf_loose = 40981;
    int fec_rpf_strict = 40982;
    bcm_gport_t gport          = 0;
    bcm_ip_t route_sip_rpf_hit_rif_miss = 0xc0800101; /* 192.128.1.1 */
    bcm_ip_t route_sip_rpf_hit_rif_hit  = 0xc0800102; /* 192.128.1.2 */
    l3_ingress_intf ingr_itf;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_ip_tunnel_basic_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    fec_rpf_loose = cint_ip_tunnel_basic_info.fec_id + 2;
    fec_rpf_strict = fec_rpf_loose + 2;
    /** Configure traps  */
    rv = l3_dnx_ip_rpf_config_traps(&unit, 1);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_rpf_config_traps\n");
        return rv;
    }

    /** Set In-Port to In ETh-RIF: tunnel provider, rpf in loose and strict */
    rv = in_port_intf_set(unit, provider_port_loose, eth_rif_intf_in_loose);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set eth_rif_intf_in_loose\n");
        return rv;
    }
    rv = in_port_intf_set(unit, provider_port_strict, eth_rif_intf_in_strict);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set eth_rif_intf_in_strict\n");
        return rv;
    }

    /** Set Out-Port default properties */
    rv = out_port_set(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set access_port\n", proc_name);
        return rv;
    }

    /*
     * Create routing interface for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure),
     * to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    /** Loose RPF Provider */
    rv = intf_eth_rif_create(unit, eth_rif_intf_in_loose,
                             cint_ip_tunnel_basic_info.intf_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in_loose\n");
        return rv;
    }
    /** Strict RPF Provider */
    rv = intf_eth_rif_create(unit, eth_rif_intf_in_strict,
                             cint_ip_tunnel_basic_info.intf_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in_strict\n");
        return rv;
    }
    /** Acess */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_access,
                             cint_ip_tunnel_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_access\n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = eth_rif_intf_in_loose;
    ingr_itf.vrf     = provider_vrf_loose;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set eth_rif_intf_in_loose\n");
        return rv;
    }
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = eth_rif_intf_in_strict;
    ingr_itf.vrf     = provider_vrf_strict;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set eth_rif_intf_in_strict\n");
        return rv;
    }

    /** Select the bcmTunnelTypes */
    cint_ip_tunnel_basic_info.tunnel_type = bcmTunnelTypeIpAnyIn4;

    /** Create the tunnel termination lookup and the tunnel termination inlif */
    cint_ip_tunnel_basic_info.provider_vrf = provider_vrf_loose;

    bcm_if_t tunnel_inlif_intf_id_rpf_loose;
    rv = tunnel_terminator_create(unit, &tunnel_inlif_intf_id_rpf_loose, &cint_ip_tunnel_basic_info.tunnel_id_v4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error,tunnel_terminator_create \n", proc_name);
        return rv;
    }

    cint_ip_tunnel_basic_info.provider_vrf = provider_vrf_strict;

    bcm_if_t tunnel_inlif_intf_id_rpf_strict;
    rv = tunnel_terminator_create(unit, &tunnel_inlif_intf_id_rpf_strict, &cint_ip_tunnel_basic_info.tunnel_id_v4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error,tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /** Set Tunnel termination In-LIF properties for RPF forwarding */
    rv = intf_ingress_rif_rpf_set(unit, tunnel_inlif_intf_id_rpf_loose, provider_vrf_loose,
                                  bcmL3IngressUrpfLoose);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_rpf_set intf_in_loose vrf_loose\n");
        return rv;
    }
    rv = intf_ingress_rif_rpf_set(unit, tunnel_inlif_intf_id_rpf_strict, provider_vrf_strict,
                                  bcmL3IngressUrpfStrict);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_rpf_set intf_in_strict vrf_strict\n");
        return rv;
    }

    /*
     * Create ARP entry and set its properties
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_ip_tunnel_basic_info.arp_id),
                                       cint_ip_tunnel_basic_info.arp_next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif (ARP), Destination port, Global Out-EthRIF
     */
    /** Loose and Strict FECs */
    BCM_GPORT_LOCAL_SET(gport, access_port);
    rv = l3__egress_only_fec__create(unit, cint_ip_tunnel_basic_info.fec_id,
                                     cint_ip_tunnel_basic_info.eth_rif_intf_access,
                                     cint_ip_tunnel_basic_info.arp_id, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, fec_rpf_loose\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec_rpf_strict, tunnel_inlif_intf_id_rpf_strict, 0,
                                     gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, fec_rpf_strict\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, fec_rpf_loose, tunnel_inlif_intf_id_rpf_loose, 0,
                                     gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /*
     *  Add Route entries:
     *  Loose and Strict mode: two each for successful RPF check
     */
    /** Loose */
    rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip, provider_vrf_loose,
                       cint_ip_tunnel_basic_info.fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv4, route_dip, vrf_loose\n");
        return rv;
    }
    rv = add_host_ipv4(unit, route_sip_rpf_hit_rif_miss, provider_vrf_loose,
                       cint_ip_tunnel_basic_info.fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_host_ipv4, route_sip_rpf_hit_rif_miss, vrf_loose\n", proc_name);
        return rv;
    }
    rv = add_host_ipv4(unit, route_sip_rpf_hit_rif_hit, provider_vrf_loose, fec_rpf_loose, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_host_ipv4, route_sip_rpf_hit_rif_hit, vrf_loose\n", proc_name);
        return rv;
    }

    /** Strict */
    rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.route_dip, provider_vrf_strict,
                       cint_ip_tunnel_basic_info.fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, add_host_ipv4, route_dip, vrf_strict\n");
        return rv;
    }
    rv = add_host_ipv4(unit, route_sip_rpf_hit_rif_miss, provider_vrf_strict,
                       cint_ip_tunnel_basic_info.fec_id, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_host_ipv4, route_sip_rpf_hit_rif_miss, vrf_strict\n", proc_name);
        return rv;
    }
    rv = add_host_ipv4(unit, route_sip_rpf_hit_rif_hit, provider_vrf_strict, fec_rpf_strict, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, add_host_ipv4, route_sip_rpf_hit_rif_hit, vrf_strict\n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Main function for Ipv4in4 tunnel termination scenario with Multicast RPF checks in Jericho 2
 * Inputs:
 * unit                 - relevant unit;
 * provider_port_strict - incoming port multicast rpf (strict - expilcit and SIP-based);
 * access_port          - outgoing port;
 * vsi_strict           - vsi -> In-Eth-RIF for mc rpf;
 * provider_vrf_strict  - vrf for mc rpf;
 */
int
ipv4_tunnel_termination_with_mc_rpf(
    int unit,
    int provider_port,
    int access_port_1,
    int access_port_2,
    int rif)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    proc_name = "ipv4_tunnel_termination_with_mc_rpf";

    int tt_inlif_masked       = 0; /* TT In-LIF is masked -> <VRF,DIP> lookup in LPM */
    int nof_mc_replications   = 2;
    int access_ports[2]       = { access_port_1, access_port_2 };
    int fec_ids[4]            = { 65537, 65539, 65541, 65543 };  /** RPF mode-specific inerfaces, FECs, IP addresses*/
    uint32 flags[4]           = { BCM_L3_MC_RPF_SIP_BASE, BCM_L3_MC_RPF_SIP_BASE,
                                  BCM_L3_MC_RPF_EXPLICIT, BCM_L3_MC_RPF_EXPLICIT };
    bcm_mac_t mc_next_hop_mac = { 0x01, 0x00, 0x5E, 0x02, 0x02, 0x02 };
    /*
     * Packet arriving at the ingress must have multicast-compatible destination address:
     * DA[47:23]=={24'h01_00_5E, 1'b0} and Ethernet-Header.DA[22:0]==IP-Header.DIP[22:0],
     * or IPHeader.DIP[31:28] = to 4'hE - in this case it's the former.
     */
    cint_ip_tunnel_basic_info.arp_next_hop_mac      = mc_next_hop_mac;
    cint_ip_tunnel_basic_info.route_dip             = 0xE0020202;  /* 224.2.2.2 */
    cint_ip_tunnel_basic_info.mask                  = 0xFFFFFFFF;  /* 255.255.255.255 */
    cint_ip_tunnel_basic_info.eth_rif_intf_provider = rif;
    cint_ip_tunnel_basic_info.arp_id                = 6520;

    l3_ingress_intf ingr_itf;
    bcm_if_t tunnel_inlif_intf_id;
    bcm_gport_t gport_1, gport_2;
    bcm_gport_t mc_gport;
    bcm_multicast_replication_t replications[nof_mc_replications];

    /** Configure traps  */
    rv = l3_dnx_ip_mc_rpf_config_traps(unit);
    if (rv != BCM_E_NONE && rv != BCM_E_PARAM)
    {
        printf("Error, l3_dnx_ip_mc_rpf_config_traps\n");
        return rv;
    }

    /** Set In-Port to In ETh-RIF: tunnel provider */
    rv = in_port_intf_set(unit, provider_port, cint_ip_tunnel_basic_info.eth_rif_intf_provider);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set cint_ip_tunnel_basic_info.eth_rif_intf_provider\n");
        return rv;
    }

    /** Set Out-Port default properties */
    rv = out_port_set(unit, access_port_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set, access_port_1\n");
        return rv;
    }

    rv = out_port_set(unit, access_port_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set, access_port_2\n");
        return rv;
    }

    /*
     * Create routing interface for the tunnel's IP.
     * We are using it as ingress ETH-RIF to perform ETH termination (my-mac procedure),
     * to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_ip_tunnel_basic_info.eth_rif_intf_provider,
                             cint_ip_tunnel_basic_info.intf_provider_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create cint_ip_tunnel_basic_info.eth_rif_intf_provider\n");
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = cint_ip_tunnel_basic_info.eth_rif_intf_provider;
    ingr_itf.vrf = cint_ip_tunnel_basic_info.provider_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set cint_ip_tunnel_basic_info.eth_rif_intf_provider\n");
        return rv;
    }

    /** Create access interfaces for multicast replications */
    int i;
    int intf_out = cint_ip_tunnel_basic_info.eth_rif_intf_access;
    for (i = 0; i < nof_mc_replications; i++)
    {
        /** Create access L3 ETH-RIF interface */
        if (intf_out != cint_ip_tunnel_basic_info.eth_rif_intf_provider)
        {
            rv = intf_eth_rif_create(unit, intf_out,
                                     cint_ip_tunnel_basic_info.intf_access_mac_address);
            if (rv != BCM_E_NONE)
            {
                printf("intf_eth_rif_create eth_rif_intf_access\n");
                return rv;
            }
        }
        /** Set the replication */
        set_multicast_replication(&replications[i], access_ports[i], intf_out);

        /** Increment access interface id and mac_address for the next replication*/
        intf_out++;
        cint_ip_tunnel_basic_info.intf_access_mac_address[5]++;
    }

    /** Create the multicast group */
    rv = create_multicast(unit, replications, cint_ipmc_info.nof_replications, cint_ip_tunnel_basic_info.ipmc_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function create_multicast\n");
        return rv;
    }

    /** Select the bcmTunnelTypes */
    cint_ip_tunnel_basic_info.tunnel_type = bcmTunnelTypeIpAnyIn4;

    /** Create the tunnel termination lookup and the tunnel termination inlif */
    rv = tunnel_terminator_create(unit, &tunnel_inlif_intf_id, &cint_ip_tunnel_basic_info.tunnel_id_v4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error,tunnel_terminator_create \n", proc_name);
        return rv;
    }

    int tunnel_in_lifs[4] = { tunnel_inlif_intf_id, tunnel_inlif_intf_id, tunnel_inlif_intf_id,
                              tunnel_inlif_intf_id + 42 };

    /** Create ARP entry and set its properties */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_ip_tunnel_basic_info.arp_id),
                                       cint_ip_tunnel_basic_info.arp_next_hop_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n", proc_name);
        return rv;
    }

    BCM_GPORT_MCAST_SET(mc_gport, cint_ip_tunnel_basic_info.ipmc_group);
    /** Create FECs for SIP-based and Explicit RPF */
    for (i = 0; i < 4; i++)
    {
        rv = l3__egress_only_fec__create(unit, fec_ids[i], tunnel_in_lifs[i], 0, mc_gport, flags[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC inner, fecs[0]\n");
            return rv;
        }
    }

    /** For SIP-based RPF we need to add an UC entry with the IP of the SRC IP */
    rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.mrpf_sip, cint_ip_tunnel_basic_info.access_vrf, fec_ids[0], 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host\n");
        return rv;
    }
    /** For failing Explicit RPF we need to add an UC entry with the IP of the SRC IP and a wrong FEC */
    rv = add_host_ipv4(unit, cint_ip_tunnel_basic_info.mrpf_sip + 1, cint_ip_tunnel_basic_info.access_vrf, fec_ids[3],
            0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host\n");
        return rv;
    }

    /** Create four IPMC entries - two leading to SIP-based RPF, two leading to Explicit RPF. */
    for (i = 0; i < 4; i++)
    {
        rv = add_ipv4_ipmc(unit, cint_ip_tunnel_basic_info.ipmc_dip, cint_ip_tunnel_basic_info.mask, 0,
                cint_ip_tunnel_basic_info.mrpf_sip_mask, tunnel_inlif_intf_id, cint_ip_tunnel_basic_info.access_vrf, 0,
                fec_ids[i], 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in function add_ipv4_ipmc - iter = %d \n", i);
            return rv;
        }
        cint_ip_tunnel_basic_info.ipmc_dip++;
    }

    return rv;
}

int
vxlan_network_domain_update(
    int unit,
    int update_value,
    int tunnel_term_id)
{
    int rv;
    rv = bcm_port_class_set (unit, tunnel_term_id, bcmPortClassIngress, update_value);
    return rv;
}
