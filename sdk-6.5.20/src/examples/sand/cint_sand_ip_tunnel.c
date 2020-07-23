/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/* 
* IPv4 tunnel basic info that can be used for IPv4 tunnel encapsualation/termination and IPv4 VXLAN.
*/ 

/* ********* 
  Globals/Aux Variables
 */
struct ip_tunnel_basic_info_s{
    int nof_tunnels;                        /* number of tunnels */     
    int access_port;                        /* incoming port from access side */
    int provider_port;                      /* incoming port from provider side */
    int roo;                                /* indicates routing over overlay */
    /* RIF info */
    int access_eth_rif;                     /* access RIF */
    int access_eth_vrf;                     /* access VRF */  
    int provider_eth_rif;                   /* provider RIF */    
    int provider_eth_vrf;                   /* provider VRF */
    int provider_native_eth_rif;            /* provider native RIF */ 
    int provider_native_eth_vrf;            /* provider native VRF */
    bcm_mac_t access_eth_rif_mac;           /* sa mac for out RIF */    
    bcm_mac_t provider_eth_rif_mac;         /* sa mac for out RIF */    
    bcm_mac_t provider_native_eth_rif_mac;  /* sa mac for out RIF */
    
    /* Tunnel info */
    bcm_tunnel_type_t tunnel_type[3];       /* tunnel type */
    int tunnel_flags[3];                    /* tunnel flags */
    /* IPv4 */
    bcm_ip_t dip[3];                        /* tunnel DIP */
    bcm_ip_t dip_mask[3];                   /* tunnel DIP mask */
    bcm_ip_t sip[3];                        /* tunnel SIP */
    bcm_ip_t sip_mask[3];                   /* tunnel SIP mask */
    bcm_ip_t subnet_mask[3];                /* tunnel DIP subnet */
    /* IPv6 */
    bcm_ip6_t dip6[3];                      /* tunnel DIP */
    bcm_ip6_t dip6_mask[3];                 /* tunnel DIP mask */
    bcm_ip6_t sip6[3];                      /* tunnel SIP */
    bcm_ip6_t sip6_mask[3];                 /* tunnel SIP mask */
    bcm_ip6_t v6_subnet_mask[3];            /* tunnel DIP subnet */
    int ttl[3];                             /* ttl */
    bcm_tunnel_dscp_select_t dscp_sel[3];   /* dscp_sel */
    int dscp[3];                            /* dscp */
    int flow_label[3];                      /* flow_label */
    uint16 udp_dest_port[3];                /* udp dest port */ 
    
    
    /* Encapsulation info */
    int encap_flag;                         /* indicate the encapsulation flag: 1: outer arp use arp+ac. */
    bcm_if_t encap_tunnel_intf[3];
    bcm_gport_t encap_tunnel_id[3];
    bcm_mac_t encap_next_hop_mac[3];        /* da mac for next core hop */
    int encap_arp_id[3];                    /* Id for core ARP entry */
    int arp_fec_id[3];                      /* Id for core ARP FEC entry (used for learning case in vxlan)*/
    int encap_fec_id[3];                    /* Id for core FEC entry */
    bcm_mac_t encap_native_next_hop_mac[3]; /* mac for next core native hop */  
    int encap_native_arp_id[3];             /* Id for core native ARP entry */
    int encap_native_fec_id[3];             /* Id for core native FEC entry */
    int encap_is_hierarchy[3];              /* Indicates whether it is hierarchy */
    int encap_is_ecmp[3];                   /* Indicates whether it is hierarchy FEC */
    bcm_if_t encap_ecmp[3];                 /* Id for core ECMP entry */
    bcm_if_t encap_native_ecmp[3];          /* Id for core native ECMP entry */    

    /* Encapsulation route info */
    bcm_ip_t encap_host_dip;                /* Host DIP for IPv4 */
    bcm_ip_t encap_route_dip_mask;          /* mask for route DIP */
    bcm_ip6_t encap_host_dip6;              /* Host DIP for IPv6 */
    bcm_ip6_t encap_route_dip6_mask;        /* mask for route DIP */
    int access_vlan;
    int provider_vlan;
    int provider_native_vlan;
    int ipv6_host_per_tunnel;

    /* Termination info */
    int skip_ethernet_flag;                 /* indicate to skip ethernet after terminating ip tunnel. */
    int term_with_id_flag[3];                       /* indicate to use with_id flag */
    int term_bud;                           /* indicate to tunnel term bud node. */
    int port_property;                      /* port property */
    int v6_cascade_lookups[3];              /* Indicates cascade termination lookup, 1: normal cascade lookup. 2: cascade lookup with same dip and vrf*/
    int v6_my_vtep_index[3];                /* Used to infer in-lif when doing cascade lookup */
    bcm_mac_t term_next_hop_mac[3];         /* da mac for next access hop */
    int term_arp_id[3];                     /* Id for access ARP entry */
    int term_fec_id[3];                     /* Id for access FEC entry */
    int tunnel_rif[3];                      /* RIF, property of the TT inLif. */
    int tunnel_vrf[3];                      /* VRF, property of the TT RIF. */
    bcm_gport_t term_tunnel_id[3];

    /* Termination route info */    
    bcm_ip_t  term_host_dip[3];             /* Host DIP for IPv4 */
    bcm_ip_t  term_route_dip_mask;          /* mask for route DIP */    
    bcm_ip6_t term_host_dip6[3];            /* Host DIP for IPv6 */
    bcm_ip6_t term_route_dip6_mask;         /* mask for route DIP */
    bcm_ip_t  no_term_route_dip;            /* Route DIP for IPv4 */
    bcm_ip_t  no_term_route_dip_mask;       /* mask for route DIP */  

    /* QOS */
    int ing_qos_id;
    int egr_qos_id;

    /* SVTAG */
    int svtag_enable;                       /* Enable SVTAG lookup */
};

ip_tunnel_basic_info_s ip_tunnel_basic_info = 
{
    /* nof_tunnels */
    2, 
    /* access_port */
    200,
    /* provider_port */
    201,
    /* roo */
    0,
    
    /********** RIF info **********/
    /* access_eth_rif | access_eth_vrf*/
    5, 2,    
    /* provider_eth_rif  | provider_eth_vrf */
    10, 3,  
    /* provider_native_eth_rif  | provider_native_eth_vrf  */
    15, 4,    
    /* access_eth_rif_mac */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /* provider_eth_rif_mac */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /* provider_native_eth_rif_mac */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x22},

    /********** Tunnel info **********/
    /* tunnel_type */
    {bcmTunnelTypeIpAnyIn4, bcmTunnelTypeIpAnyIn4, bcmTunnelTypeIpAnyIn4},
    /* tunnel_flags */
    {0, 0, 0},
    /* dip */
    {   
        0xA10f1011, /* 161.15.16.17 */
        0xAB0f1011, /* 171.15.16.17 */
        0xB50f1011  /* 181.15.16.17 */
    },
    /* dip6_mask */
    {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    /* sip6 */
    {
        0xA00f1011, /* 160.15.16.17 */ 
        0xAA0f1011, /* 170.15.16.17 */
        0xB40f1011  /* 180.15.16.17 */
    },
    /* sip6_mask */
    {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
    /* subnet_mask */
    {0xFFFFFF00, 0xFFFFF000, 0xFFFF0000},
    /* dip6 */
    {
        {0x20, 0x00, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1},
        {0x20, 0x00, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1},
        {0x20, 0x00, 0xDE, 0xDD, 0xDC, 0xDB, 0xDA, 0xD9, 0xD8, 0xD7, 0xD6, 0xD5, 0xD4, 0xD3, 0xD2, 0xD1},
    },
    /* dip6_mask */
    {
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    },
    /* sip6 */
    {
        {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x77, 0xFF, 0xFE, 0xFD, 0xFC, 0xFB},
        {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x77, 0xFF, 0xEE, 0xED, 0xEC, 0xEB},
        {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x77, 0xFF, 0xDE, 0xDD, 0xDC, 0xDB},
    },
    /* sip6_mask */
    {
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    },
    /* v6_subnet_mask */
    {
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},
    },
    /* ttl */
    {60, 50, 40},
    /*dscp_sel*/
    {bcmTunnelDscpAssign, bcmTunnelDscpAssign, bcmTunnelDscpAssign},
    /* dscp */
    {40, 30, 20},
    /* flow_label */
    {0, 0, 0},
    /* udp_dest_port */
    {0x6666, 0x7777, 0x8888},

    /********** Encapsulation info **********/
    /* encap_flag */
    0,
    /* encap_tunnel_intf */
    {0, 0, 0},
    /* encap_tunnel_id */
    {0, 0, 0},
    /*encap_next_hop_mac*/
    { 
        {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}, 
        {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d},
        {0x30, 0x00, 0x00, 0x00, 0xcd, 0x1d}
    },    
    /* encap_arp_id */
    {5050, 5060, 5070},
    /* encap arp fec id */
    {5007, 5009, 5011}, 
    /* encap_fec_id */
    {5001, 5003, 5005},
    /* encap_native_next_hop_mac */
    { 
        {0x01, 0x00, 0x00, 0x00, 0xcd, 0x1d}, 
        {0x21, 0x00, 0x00, 0x00, 0xcd, 0x1d},
        {0x31, 0x00, 0x00, 0x00, 0xcd, 0x1d}
    },    
    /* encap_native_arp_id */
    {6050, 6060, 6070},
    /* encap_native_fec_id */
    {6001, 6003, 6005}, 
    /* encap_is_hierarchy */
    {0, 0, 0}, 
    /* encap_is_ecmp */
    {0, 0, 0},
    /* encap_ecmp */
    {2150, 2151, 2152},
    /* encap_native_ecmp */
    {0, 0, 0},

    /********** Encapsulation route info **********/
    /* encap_host_dip */
    0x7fffff03, 
    /* encap_route_dip_mask */
    0xfffffff0,
    /* encap_host_dip6 */
    {0x20, 0x00, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE},
    /* encap_route_dip6_mask*/
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00},    
    
    /* access_vlan */
    50,  
    /* provider_vlan */
    100,
    /* provider_native_vlan */
    150,
    /* ipv6_host_per_tunnel */
    0,


    /********** Termination info **********/
    /* skip_ethernet_flag */
    0,
    /* termination with id flags */
    {0, 0, 0}, 
    /* term_bud */
    0,
    /* port_property */
    9,
    /* v6_cascade_lookups */
    {0, 0, 0},
    /* v6_my_vtep_index */
    {1, 2, 3},
    /* term_next_hop_mac*/
    {
        {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01},
        {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02},
        {0x00, 0x0c, 0x00, 0x02, 0x00, 0x03}
    },
    /* term_arp_id */
    {7050, 7060, 7070},
    /* term_fec_id */
    {7001, 7003, 7005},
    /* tunnel_rif*/
    {30, 40, 50},
    /* tunnel_vrf*/
    {22, 33, 44},
    /* term_tunnel_id */
    {0, 0, 0},

    /********** Termination route info **********/
    /* term_host_dip */
    {0x01000011, 0x03000011,0x05000011}, /* 1.0.0.17, 3.0.0.17, 5.0.0.17 */
    /*  term_route_dip_mask  */
    0xffffff00,
    /*  term_host_dip6 */
    {
        {0x20, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77},
        {0x20, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x77, 0x77, 0x88, 0x88},
        {0x20, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x88, 0x88, 0x99, 0x99}
    },
    /*  term_route_dip6_mask  */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0},    
    /*  no_term_route_dip  */
    0xBF0f1011, /* 181.15.16.17 */
    /*  no_term_route_dip_mask  */
    0xffffff00,

    /********** QOS **********/
    /* ing_qos_id */
    0,
    /* egr_qos_id */
    0,
    /* SVTAG */
    0
};
int verbose = 1;

/*
 * Function used to map the fec and arp id for different device.
 */
int
ip_tunnel_fec_arp_map(int unit)
{   
    int rv;
    int tunnel_idx;

    if (is_device_or_above(unit, JERICHO2))
    {
        int hier_1_fec;
        int encap_id;
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &hier_1_fec);
        if (rv != BCM_E_NONE)
        {
            printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
            return rv;
        }
        if(ip_tunnel_basic_info.roo)
        {
            rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &encap_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
                return rv;
            }
        }
        else
        {
            encap_id = hier_1_fec + 20;
        }

        for (tunnel_idx = 0; tunnel_idx < ip_tunnel_basic_info.nof_tunnels; tunnel_idx++)
        {
            /* Special process for the native fec id */
            ip_tunnel_basic_info.encap_fec_id[tunnel_idx] = encap_id++;
            ip_tunnel_basic_info.encap_native_fec_id[tunnel_idx] = hier_1_fec++;
            ip_tunnel_basic_info.term_fec_id[tunnel_idx] = hier_1_fec++;
            ip_tunnel_basic_info.arp_fec_id[tunnel_idx] = hier_1_fec++;
        }
    } else if (is_device_or_above(unit, JERICHO_PLUS)) 
    {
        for (tunnel_idx = 0; tunnel_idx < ip_tunnel_basic_info.nof_tunnels; tunnel_idx++)
        {
            ip_tunnel_basic_info.encap_arp_id[tunnel_idx] += 8192;
            ip_tunnel_basic_info.encap_native_arp_id[tunnel_idx] += 8192;
            ip_tunnel_basic_info.term_arp_id[tunnel_idx] += 8192;
            ip_tunnel_basic_info.arp_fec_id[tunnel_idx] += 8192;

        }
    } else if (!is_device_or_above(unit, JERICHO))
    {
        for (tunnel_idx = 0; tunnel_idx < ip_tunnel_basic_info.nof_tunnels; tunnel_idx++)
        {
            ip_tunnel_basic_info.encap_arp_id[tunnel_idx] = 0;
            ip_tunnel_basic_info.term_arp_id[tunnel_idx] = 0;
            ip_tunnel_basic_info.arp_fec_id[tunnel_idx] = 0;

        }
    }

    return rv;
}

/*
 * Interfaces initialisation function for ip tunnel termination and encapsulation scenarios
 * The function ip_tunnel_open_route_interfaces implements the basic configuration,
 * on top of witch all tunnel terminators and initiators are built. All tunnel termination and encapsulation scenarios will call it.
 * Inputs:   unit          - relevant unit;
 *           access_port - access port;
 *           provider_port   - provider port;
 */
int
ip_tunnel_open_route_interfaces(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "ip_tunnel_open_route_interfaces";

    /*
     * Configure ingress interfaces
     */
    rv = ip_tunnel_open_route_interfaces_access(unit, access_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_open_route_interfaces_access \n");
        return rv;
    }

    /*
     * Configure egress interfaces
     */
    rv = ip_tunnel_open_route_interfaces_provider(unit, provider_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_open_route_interfaces_provider \n");
        return rv;
    }

    return rv;
}

/*
 * Configure ip tunnel termination egress interfaces and ip tunnel encapsulation ingress interfaces
 * Inputs: access_port - outgoing port;
 *         *proc_name  - main function name;
 */
int
ip_tunnel_open_route_interfaces_access(
    int unit,
    int access_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;
    sand_utils_l3_eth_rif_s eth_rif_structure;

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, access_port, ip_tunnel_basic_info.access_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set access_eth_rif\n", proc_name);
        return rv; 
    }

    /* Set RIF -> My-Mac  and RIF -> VRF */
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, ip_tunnel_basic_info.access_eth_rif, 0, 0, ip_tunnel_basic_info.access_eth_rif_mac, ip_tunnel_basic_info.access_eth_vrf);
    eth_rif_structure.ing_qos_map_id = ip_tunnel_basic_info.ing_qos_id;
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create access_eth_rif \n");
        return rv;
    } 

    return rv;
}


/*
 * Configure ip tunnel termination ingress interfaces and ip tunnel encapsulation egress interfaces
 * Inputs: provider_port - incomming port;
 *         *proc_name    - main function name;
 */
int
ip_tunnel_open_route_interfaces_provider(
    int unit,
    int provider_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;
    int tunnel_idx;
    sand_utils_l3_eth_rif_s eth_rif_structure;

    /*
     * Add VLAN to the multicast group 1
     */
    rv = vlan__open_vlan_per_mc(unit, ip_tunnel_basic_info.provider_eth_rif, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", ip_tunnel_basic_info.provider_eth_rif, unit);
    }
    rv = bcm_vlan_gport_add(unit, ip_tunnel_basic_info.provider_eth_rif, ip_tunnel_basic_info.access_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", ip_tunnel_basic_info.access_port, ip_tunnel_basic_info.provider_eth_rif);
      return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, provider_port, ip_tunnel_basic_info.provider_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set provider_eth_rif \n", proc_name);
        return rv;
    }

    /* Set RIF -> My-Mac  and RIF -> VRF */
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, ip_tunnel_basic_info.provider_eth_rif, 0, 0, ip_tunnel_basic_info.provider_eth_rif_mac, ip_tunnel_basic_info.provider_eth_vrf);
    eth_rif_structure.ing_qos_map_id = ip_tunnel_basic_info.ing_qos_id;
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create provider_eth_rif \n");
        return rv;
    } 

    /*
     * In DPP (JR1) architecture IP-Tunnel LIF retrieve its RIF properties according to RIF-ID object (from tunnel_if).
     * While in DNX architecture (JR2) RIF properties are retrieved directly from IP-Tunnel LIF.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        for (tunnel_idx = 0; tunnel_idx < ip_tunnel_basic_info.nof_tunnels; tunnel_idx++)
        {
            /* Set RIF -> My-Mac  and RIF -> VRF */
            sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, ip_tunnel_basic_info.tunnel_rif[tunnel_idx], 0, 0, ip_tunnel_basic_info.provider_eth_rif_mac, ip_tunnel_basic_info.tunnel_vrf[tunnel_idx]);
            rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
            if (rv != BCM_E_NONE)
            {
                printf("Error, sand_utils_l3_eth_rif_create tunnel_rif[%d] \n", tunnel_idx);
                return rv;
            } 
        }
    }    

    if (ip_tunnel_basic_info.roo) {
        /* Set RIF -> My-Mac  and RIF -> VRF */
        sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, ip_tunnel_basic_info.provider_native_eth_rif, 0, 0, ip_tunnel_basic_info.provider_native_eth_rif_mac, ip_tunnel_basic_info.provider_native_eth_vrf);
        eth_rif_structure.ing_qos_map_id = ip_tunnel_basic_info.ing_qos_id;
        eth_rif_structure.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        eth_rif_structure.native_routing_vlan_tags = 1; /* native ethernet compensation */
        rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_eth_rif_create provider_native_eth_rif \n");
            return rv;
        } 
    }

    return rv;
}

/*
 * Interfaces initialisation function for ip tunnel termination scenarios
 * The function ip_tunnel_term_open_route_interfaces implements the basic configuration,
 * on top of witch all tunnel terminators are built. All tunnel termination scenarios will call it.
 * Inputs:   unit          - relevant unit;
 *           access_port - access port;
 *           provider_port   - provider port;
 */
int
ip_tunnel_term_open_route_interfaces(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "ip_tunnel_term_open_route_interfaces";

    /*
     * Configure ingress interfaces
     */
    rv = ip_tunnel_term_open_route_interfaces_provider(unit, provider_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_term_open_route_interfaces_provider \n", proc_name);
        return rv;
    }

    /*
     * Configure egress interfaces
     */
    rv = ip_tunnel_term_open_route_interfaces_access(unit, access_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_term_open_route_interfaces_access \n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Configure ip tunnel termination ingress interfaces
 * Inputs: provider_port - incomming port;
 *         *proc_name    - main function name;
 */
int
ip_tunnel_term_open_route_interfaces_provider(
    int unit,
    int provider_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;
    int tunnel_idx;
    sand_utils_l3_eth_rif_s eth_rif_structure;

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, provider_port, ip_tunnel_basic_info.provider_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set provider_eth_rif \n", proc_name);
        return rv;
    }

    /* Set RIF -> My-Mac  and RIF -> VRF */
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, ip_tunnel_basic_info.provider_eth_rif, 0, 0, ip_tunnel_basic_info.provider_eth_rif_mac, ip_tunnel_basic_info.provider_eth_vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create provider_eth_rif \n");
        return rv;
    }

    /*
     * In DPP (JR1) architecture IP-Tunnel LIF retrieve its RIF properties according to RIF-ID object (from tunnel_if).
     * While in DNX architecture (JR2) RIF properties are retrieved directly from IP-Tunnel LIF.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        for (tunnel_idx = 0; tunnel_idx < ip_tunnel_basic_info.nof_tunnels; tunnel_idx++)
        {
            /* Set RIF -> My-Mac  and RIF -> VRF */
            sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, ip_tunnel_basic_info.tunnel_rif[tunnel_idx], 0, 0, ip_tunnel_basic_info.provider_eth_rif_mac, ip_tunnel_basic_info.tunnel_vrf[tunnel_idx]);
            rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
            if (rv != BCM_E_NONE)
            {
                printf("Error, sand_utils_l3_eth_rif_create tunnel_rif[%d] \n", tunnel_idx);
                return rv;
            }
        }
    }

    return rv;
}

/*
 * Configure ip tunnel termination egress interfaces
 * Inputs: access_port - outgoing port;
 *         *proc_name  - main function name;
 */
int
ip_tunnel_term_open_route_interfaces_access(
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
    rv = intf_eth_rif_create(unit, ip_tunnel_basic_info.access_eth_rif,
                             ip_tunnel_basic_info.access_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create access_eth_rif\n", proc_name);
        return rv;
    }

    return rv;
}

 /*
  * Interfaces initialisation function for ip tunnel encapsulation scenarios
  * The function ip_tunnel_encap_open_route_interfaces implements the basic configuration,
  * on top of witch all tunnel initiators are built. All tunnel encapsulation scenarios will call it.
  * Inputs:   unit          - relevant unit;
  *           access_port - access port;
  *           provider_port   - provider port;
 */
int
ip_tunnel_encap_open_route_interfaces(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "ip_tunnel_encap_open_route_interfaces";

    /*
     * Configure ingress interfaces
     */
    rv = ip_tunnel_encap_open_route_interfaces_access(unit, access_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encap_open_route_interfaces_access\n", proc_name);
        return rv;
    }

    /*
     * Configure egress interfaces
     */
    rv = ip_tunnel_encap_open_route_interfaces_provider(unit, provider_port, proc_name);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, ip_tunnel_encap_open_route_interfaces_provider\n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Configure ip tunnel encapsulation ingress interfaces
 * Inputs: access_port - incomming port;
 *         *proc_name - main function name;
 */
int
ip_tunnel_encap_open_route_interfaces_access(
    int unit,
    int access_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;
    sand_utils_l3_eth_rif_s eth_rif_structure;

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, access_port, ip_tunnel_basic_info.access_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set access_eth_rif\n", proc_name);
        return rv; 
    }

    /* Set RIF -> My-Mac  and RIF -> VRF */
    sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, ip_tunnel_basic_info.access_eth_rif, 0, 0, ip_tunnel_basic_info.access_eth_rif_mac, ip_tunnel_basic_info.access_eth_vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_eth_rif_create access_eth_rif \n");
        return rv;
    }

    return rv;
}

/*
 * Configure ip tunnel encapsulation egress interfaces
 * Inputs: provider_port - outgoing port;
 *         *proc_name - main function name;
 */
int
ip_tunnel_encap_open_route_interfaces_provider(
    int unit,
    int provider_port,
    char *proc_name)
{
    int rv = BCM_E_NONE;

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set provider_port\n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel encapsulation.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, ip_tunnel_basic_info.provider_eth_rif,
                             ip_tunnel_basic_info.provider_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create provider_eth_rif\n", proc_name);
    }

    return rv;
}

/*
 * Function that creates the arp for access side(from core to access outof overlay).
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ip_tunnel_term_create_arp(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;
    sand_utils_l3_arp_s arp_structure;

    /*
     * Configure outer ARP entry 
     */
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, ip_tunnel_basic_info.term_arp_id[tunnel_idx], 0, 0, 
                                    ip_tunnel_basic_info.term_next_hop_mac[tunnel_idx], ip_tunnel_basic_info.access_eth_rif);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_arp_create\n");
        return rv;
    }
    ip_tunnel_basic_info.term_arp_id[tunnel_idx] = arp_structure.l3eg_arp_id;

    return rv;

}

/*
 * Function that creates the arp native for core side(from access to core into overlay).
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ip_tunnel_encap_create_arp_native(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;
    uint32 l3_flags = 0;
    sand_utils_l3_arp_s arp_structure;
    
    if (ip_tunnel_basic_info.roo) {
        if (is_device_or_above(unit,JERICHO_PLUS)) {
            l3_flags = BCM_L3_NATIVE_ENCAP;
        }
        sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, ip_tunnel_basic_info.encap_native_arp_id[tunnel_idx], l3_flags, 0, 
                                        ip_tunnel_basic_info.encap_native_next_hop_mac[tunnel_idx], 0);
        rv = sand_utils_l3_arp_create(unit, &arp_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_arp_create roo\n");
            return rv;
        }
        ip_tunnel_basic_info.encap_native_arp_id[tunnel_idx] = arp_structure.l3eg_arp_id;
            
    }

    return rv;

}


/*
 * Function that creates the arp overlay for core side(from access to core into overlay).
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ip_tunnel_encap_create_arp(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;
    sand_utils_l3_arp_s arp_structure;

    /* In ROO, until Jericho, the overlay LL encapsulation is built with a different API call 
     * (bcm_l2_egress_create instead of bcm_l3_egress create)
     * Jericho notes: 
     *  - In Jericho, api l2_egress_create configures only 12 lsbs for overlay LL encapsulation, 
     *    unlike in arad+ version, where all the 48bits are saved in the LL encapsulation
     *  - In Jericho, we support only 8 msbs of overlay outer vlan: 4 lsbs of overlay outer vlan
     *    and overlay inner vlan will be supported when eedb extension (1/4 EEDB entry)
     * 36 MSBs are  global configuration.
     *  
     * For QAX devices, use bcm_l3_egress API. 
     */
    if (!is_device_or_above(unit,JERICHO_PLUS) && ip_tunnel_basic_info.roo) {
        bcm_l2_egress_t l2_egress_overlay;
        
        bcm_l2_egress_t_init(&l2_egress_overlay);
        if (ip_tunnel_basic_info.encap_arp_id[tunnel_idx]) {
            l2_egress_overlay.flags    = BCM_L2_EGRESS_WITH_ID;         
            BCM_L3_ITF_SET(ip_tunnel_basic_info.encap_arp_id[tunnel_idx], BCM_L3_ITF_TYPE_LIF, ip_tunnel_basic_info.encap_arp_id[tunnel_idx]);
            l2_egress_overlay.encap_id = ip_tunnel_basic_info.encap_arp_id[tunnel_idx]; 
        }
        sal_memcpy(l2_egress_overlay.dest_mac, ip_tunnel_basic_info.encap_next_hop_mac[tunnel_idx], 6);
        sal_memcpy(l2_egress_overlay.src_mac, ip_tunnel_basic_info.provider_eth_rif_mac, 6);
        l2_egress_overlay.outer_vlan = ip_tunnel_basic_info.provider_eth_rif; 
        l2_egress_overlay.ethertype  = 0x0800;
        l2_egress_overlay.outer_tpid = 0x8100;
        l2_egress_overlay.vlan_qos_map_id = ip_tunnel_basic_info.egr_qos_id;
        rv = bcm_l2_egress_create(unit, &l2_egress_overlay);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_egress_create\n");
            return rv;
        }
        ip_tunnel_basic_info.encap_arp_id[tunnel_idx] = l2_egress_overlay.encap_id;        
    } else {
        uint32 l3_flags2 = 0;

        /* Check whether the arp+ac flag is set */
        if ((ip_tunnel_basic_info.encap_flag & 0x1) != 0) {
            l3_flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
        }

        sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, ip_tunnel_basic_info.encap_arp_id[tunnel_idx], 0, l3_flags2,
                                        ip_tunnel_basic_info.encap_next_hop_mac[tunnel_idx], ip_tunnel_basic_info.provider_eth_rif);
        arp_structure.qos_map_id = ip_tunnel_basic_info.egr_qos_id;
        rv = sand_utils_l3_arp_create(unit, &arp_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_arp_create\n");
            return rv;
        }
        ip_tunnel_basic_info.encap_arp_id[tunnel_idx] = arp_structure.l3eg_arp_id;  
    }

    return rv;

}


/*
 * Function that creates the arp for access and core side.
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ip_tunnel_create_arp(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;

    rv = ip_tunnel_term_create_arp(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_term_create_arp\n");
        return rv;
    }

    rv = ip_tunnel_encap_create_arp(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_encap_create_arp\n");
        return rv;
    }

    return rv;
}

/*
 * Function that creates the fec for access side(from core to access outof overlay).
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ip_tunnel_term_create_fec(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;    
    sand_utils_l3_fec_s fec_structure;
    printf("ip_tunnel_basic_info.term_fec_id[tunnel_idx] %d %d",ip_tunnel_basic_info.term_fec_id[tunnel_idx],tunnel_idx);
    /*
     * Create native FEC for routing into overlay:
     *  1) Give a fec id. 
     *  2) Native out rif.
     *  3) Native arp id. 
     *  4) Point to overlay fec.    
     */
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, ip_tunnel_basic_info.term_fec_id[tunnel_idx], 0, 0, ip_tunnel_basic_info.access_port, 
                                    ip_tunnel_basic_info.access_eth_rif, ip_tunnel_basic_info.term_arp_id[tunnel_idx]);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_fec_create\n");
        return rv;
    }
    ip_tunnel_basic_info.term_fec_id[tunnel_idx] = fec_structure.l3eg_fec_id;

	return rv;
}

/*
 * Function that creates the fec native for core side(from access to core into overlay).
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ip_tunnel_encap_create_fec_native(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;
    int l3_flags = 0;
    bcm_gport_t gport;
    sand_utils_l3_fec_s fec_structure;

    if (ip_tunnel_basic_info.roo) {
        if (ip_tunnel_basic_info.encap_is_hierarchy[tunnel_idx]) {
            if (ip_tunnel_basic_info.encap_is_ecmp[tunnel_idx]) {
                BCM_GPORT_FORWARD_PORT_SET(gport, ip_tunnel_basic_info.encap_ecmp[0]);  
            } else {
                BCM_GPORT_FORWARD_PORT_SET(gport, ip_tunnel_basic_info.encap_fec_id[0]);
            }            
        }
        /* Hierarchical FEC does not work when the 1st FEC hierarchy is unprotected.
        * In case we are using hierarchical FEC, the first FEC must be protected.
        * This was fixed in Jericho B0.
        */
        if (!is_device_or_above(unit,JERICHO_B0)) {
            int fec_id;
            int encoded_fec;
            bcm_failover_t failover_id;
            
            /* create failover id for VxLAN */
            rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_failover_create\n");
                return rv;
            }    

            fec_id = ip_tunnel_basic_info.encap_native_fec_id[tunnel_idx];
            rv = l3__egress_only_fec__create_inner(unit, fec_id, ip_tunnel_basic_info.provider_native_eth_rif, 
                ip_tunnel_basic_info.encap_native_arp_id[tunnel_idx], gport, 0,
                failover_id, 0,&encoded_fec);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, l3__egress_only_fec__create_inner\n");
                return rv;
            }
            if(verbose >= 1) {
                printf("fec 0x%08X encoded_fec 0x%08X.\r\n",fec_id, encoded_fec);
            }

            /* if protection is enabled , also create another fec(primary) */
            if (failover_id != 0)
            {
                fec_id--;
                rv = l3__egress_only_fec__create_inner(unit, fec_id, ip_tunnel_basic_info.provider_native_eth_rif, 
                    ip_tunnel_basic_info.encap_native_arp_id[tunnel_idx], gport, 0,
                    failover_id, 1,&encoded_fec);
                if (rv != BCM_E_NONE)
                {
                    printf("%s(): Error, l3__egress_only_fec__create_inner primary\n");
                    return rv;
                }
                if(verbose >= 1) {
                    printf("Failover, fec 0x%08X encoded_fec 0x%08X.\r\n",fec_id, encoded_fec);
                }
            }
            ip_tunnel_basic_info.encap_native_fec_id[tunnel_idx] = encoded_fec;
        } else {
            /*
             * Create native FEC for routing into overlay:
             *  1) Give a fec id. 
             *  2) Native out rif.
             *  3) Native arp id. 
             *  4) Point to overlay fec or overlay ECMP.    
             */  
            sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, ip_tunnel_basic_info.encap_native_fec_id[tunnel_idx], l3_flags, 0, gport, 
                                        ip_tunnel_basic_info.provider_native_eth_rif, ip_tunnel_basic_info.encap_native_arp_id[tunnel_idx]);
            rv = sand_utils_l3_fec_create(unit, &fec_structure);
            if (rv != BCM_E_NONE)
            {
                printf("Error, sand_utils_l3_fec_create\n");
                return rv;
            }
            ip_tunnel_basic_info.encap_native_fec_id[tunnel_idx] = fec_structure.l3eg_fec_id;
        }
    }

	return rv;
}

/*
 * Function that creates the fec or core side(from access to core into overlay).
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ip_tunnel_encap_create_fec(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;
    int allocation_flags = 0;
    int l3_flags = 0;
    bcm_gport_t gport;
    sand_utils_l3_fec_s fec_structure;

    if (ip_tunnel_basic_info.roo) {
        if (is_device_or_above(unit,JERICHO2)) {
            l3_flags |= BCM_L3_2ND_HIERARCHY;
        }
    }

    printf("ip_tunnel_basic_info.encap_fec_id[tunnel_idx] %d %d l3_flags %d\n",ip_tunnel_basic_info.encap_fec_id[tunnel_idx],tunnel_idx, l3_flags);
    /*
     * Create overlay FEC for routing into overlay: 
     * 1) Give a fec id. 
     * 2) Tunnel id of the overlay VXLAN Tunnel. 
     * 3) Tunnel id of the overlay Eth.
     * 4) Out port.
     */
    gport = 0;
    if (BCM_GPORT_IS_TRUNK(ip_tunnel_basic_info.provider_port)) 
    {
        gport = ip_tunnel_basic_info.provider_port;
    } else {
        BCM_GPORT_LOCAL_SET(gport, ip_tunnel_basic_info.provider_port);
    }

    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, ip_tunnel_basic_info.encap_fec_id[tunnel_idx], l3_flags, 0, gport, 
                                ip_tunnel_basic_info.encap_tunnel_intf[tunnel_idx], 0);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_fec_create\n");
        return rv;
    }
    ip_tunnel_basic_info.encap_fec_id[tunnel_idx] = fec_structure.l3eg_fec_id;

	return rv;
}

/*
 * Function that creates the fec for access and core side.
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ip_tunnel_create_fec(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;
    
    rv = ip_tunnel_term_create_fec(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_term_create_fec\n");
        return rv;
    }

    rv = ip_tunnel_encap_create_fec(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ip_tunnel_encap_create_fec\n");
        return rv;
    }

    return rv;
}

/* 
 * Function that creates the FEC which point to ARP. 
 * Used when vxlan learning is: symmetric lif + ARP FEC 
 * Inputs: tunnel_idx - tunnel index 
 */ 
int
arp_create_fec(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;
    int l3_flags = 0;
    bcm_gport_t gport;
    sand_utils_l3_fec_s fec_structure;

    BCM_GPORT_LOCAL_SET(gport, ip_tunnel_basic_info.provider_port);

    /*
     * Create native FEC for routing into overlay:
     *  1) Give a fec id. 
     *  2) arp id. 
     *  3) Point to arp fec 
     */  
     printf("Create arp fec (for learning purpose) \n");
     sand_utils_l3_fec_s_common_init(unit, 
                                     BCM_L3_WITH_ID, 0, &fec_structure, ip_tunnel_basic_info.arp_fec_id[tunnel_idx], 
                                     l3_flags, 0, gport, ip_tunnel_basic_info.encap_arp_id[tunnel_idx], 0);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_fec_create\n");
        return rv;
    }
    ip_tunnel_basic_info.arp_fec_id[tunnel_idx] = fec_structure.l3eg_fec_id;

    return rv;
}

/*
 * Function that check the  tunnel type.
 * Inputs: unit - relevant unit;
 * Inputs: type - tunnel type;
 */
int
ip_tunnel_tunnel_type_is_ipv6(
    int unit,
    bcm_tunnel_type_t type)
{
    if ((type == bcmTunnelTypeIp4In6) ||
        (type == bcmTunnelTypeIp6In6) ||
        (type == bcmTunnelTypeIpAnyIn6) ||
        (type == bcmTunnelTypeGre4In6) ||
        (type == bcmTunnelTypeGre6In6) ||
        (type == bcmTunnelTypeGreAnyIn6) ||
        (type == bcmTunnelTypeVxlan6) ||
        (type == bcmTunnelTypeVxlan6Gpe) ||
        (type == bcmTunnelTypeUdp6) ||
        (type == bcmTunnelTypeSR6)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
 * Function that map the initiator tunnel type.
 * Inputs: unit - relevant unit;
 * Inputs: type - tunnel type;
 */
int
ip_tunnel_tunnel_type_map(
    int unit,
    int type)
{
    int mapped_type = type;

    if (is_device_or_above(unit, JERICHO2)) {
        if ((type == bcmTunnelTypeIp4In4) || (type == bcmTunnelTypeIp6In4)) {
            mapped_type = bcmTunnelTypeIpAnyIn4;
        }
        if ((type == bcmTunnelTypeGre4In4) || (type == bcmTunnelTypeGre6In4)) {
            mapped_type = bcmTunnelTypeGreAnyIn4;
        }
        if ((type == bcmTunnelTypeIp4In6) || (type == bcmTunnelTypeIp6In6)) {
            mapped_type = bcmTunnelTypeIpAnyIn6;
        }
        if ((type == bcmTunnelTypeGre4In6) || (type == bcmTunnelTypeGre6In6)) {
            mapped_type = bcmTunnelTypeGreAnyIn6;
        }
    }

    return mapped_type;
}

/*
 * Function that creates the general-case tunnel initiator.
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int ip_tunnel_encap_create(
    int unit,
    int tunnel_idx)
{
    int rv;
    int device_is_jericho2;
    bcm_l3_intf_t l3_intf;
    bcm_tunnel_initiator_t tunnel_init;  

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2, device_is_jericho2\n");
        return rv;
    }

    bcm_l3_intf_t_init(&l3_intf);
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.type = ip_tunnel_basic_info.tunnel_type[tunnel_idx];
    tunnel_init.type = ip_tunnel_tunnel_type_map(unit, tunnel_init.type);
    tunnel_init.flags= ip_tunnel_basic_info.tunnel_flags[tunnel_idx];

    if (tunnel_init.flags & BCM_TUNNEL_WITH_ID) {
        tunnel_init.tunnel_id = ip_tunnel_basic_info.encap_tunnel_id[tunnel_idx]; 
    }

    if(ip_tunnel_basic_info.svtag_enable) {
        tunnel_init.flags |= BCM_TUNNEL_INIT_SVTAG_ENABLE;
    }


    if (!ip_tunnel_tunnel_type_is_ipv6(unit, tunnel_init.type)) {        
        tunnel_init.dip = ip_tunnel_basic_info.dip[tunnel_idx];
        tunnel_init.sip = ip_tunnel_basic_info.sip[tunnel_idx];
    } else {
        sal_memcpy(&(tunnel_init.dip6),&(ip_tunnel_basic_info.dip6[tunnel_idx]),16);
        sal_memcpy(&(tunnel_init.sip6),&(ip_tunnel_basic_info.sip6[tunnel_idx]),16);

        if (device_is_jericho2) {
            tunnel_init.encap_access    = bcmEncapAccessTunnel2;
            tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
        } else {
            int ipv6_tunnel_encap_mode;
            
            ipv6_tunnel_encap_mode = soc_property_get(unit, "bcm886xx_ip6_tunnel_encapsulation_mode", 1);
            if (ipv6_tunnel_encap_mode == 2) {
                tunnel_init.flags = BCM_TUNNEL_INIT_WIDE;
            }
        }
    }
    tunnel_init.ttl = ip_tunnel_basic_info.ttl[tunnel_idx];
    
    if (is_device_or_above(unit, JERICHO2)) {
        if (ip_tunnel_basic_info.dscp_sel[tunnel_idx] == bcmTunnelDscpAssign)  {
            tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        }
        else
        {
            tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        }
        if(ip_tunnel_basic_info.tunnel_flags[tunnel_idx] & BCM_TUNNEL_INIT_USE_OUTER_TTL)
        {
            tunnel_init.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        }
        else
        {
            tunnel_init.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        }
    } else {
        tunnel_init.dscp_sel = ip_tunnel_basic_info.dscp_sel[tunnel_idx];   
    }
    tunnel_init.dscp = ip_tunnel_basic_info.dscp[tunnel_idx];
    tunnel_init.flow_label = ip_tunnel_basic_info.flow_label[tunnel_idx];    
    tunnel_init.l3_intf_id = ip_tunnel_basic_info.encap_arp_id[tunnel_idx];

    rv = bcm_tunnel_initiator_create(unit, &l3_intf, &tunnel_init);
    if(rv != BCM_E_NONE) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }
    ip_tunnel_basic_info.encap_tunnel_intf[tunnel_idx] = l3_intf.l3a_intf_id;
    ip_tunnel_basic_info.encap_tunnel_id[tunnel_idx] = tunnel_init.tunnel_id;
    if(verbose >= 1) {
        printf("created IP tunnel encap on intf:0x%08x id:0x%08x \n",l3_intf.l3a_intf_id, tunnel_init.tunnel_id);
    }
    return rv;
}

/*
 * Function that creates the general-case tunnel terminator.
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ip_tunnel_term_create(
    int unit,
    int tunnel_idx)
{
    int rv;
    int device_is_jericho2;
    bcm_tunnel_terminator_t tunnel_term;
    bcm_tunnel_terminator_config_key_t tunnel_term_config_key;
    bcm_tunnel_terminator_config_action_t tunnel_term_config_action;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2, device_is_jericho2 \n");
        return rv;
    }

    if (!ip_tunnel_tunnel_type_is_ipv6(unit, ip_tunnel_basic_info.tunnel_type[tunnel_idx])) {
        bcm_tunnel_terminator_t_init(&tunnel_term);
        tunnel_term.dip = ip_tunnel_basic_info.dip[tunnel_idx];
        tunnel_term.dip_mask = ip_tunnel_basic_info.dip_mask[tunnel_idx];   
        tunnel_term.sip = ip_tunnel_basic_info.sip[tunnel_idx];
        tunnel_term.sip_mask = ip_tunnel_basic_info.sip_mask[tunnel_idx];
    } else {
        /** Create IP tunnel terminator for cascading lookup */
        if (ip_tunnel_basic_info.v6_cascade_lookups[tunnel_idx] == 1)
        {
            /*
             * If enabled, 2 consecutive lookups will be made:
             * J1:
             *   1) {DIP} -> my-vtep-index (config_action.tunnel_class) - lookup in TCAM with bcm_tunnel_terminator_config_add
             *   2) {SIP, my-vtep-index, VRF} -> IP-Tunnel In-LIF       - lookup in TCAM with bcm_tunnel_terminator_create
             * J2:
             *   1) {SIP[127:48]} -> SIPv6-MSBs-ID - lookup in SEM with bcm_tunnel_terminator_create
             *   1) {DIP, VRF, Tunnel type} -> my-vtep-index (config_action.tunnel_class) - lookup in TCAM with bcm_tunnel_terminator_config_add
             *   2) {my-vtep-index, SIPv6-MSBs-ID, SIP[47:0]} -> IP-Tunnel In-LIF        - lookup in SEM with bcm_tunnel_terminator_create
             */
            bcm_tunnel_terminator_config_key_t_init(&tunnel_term_config_key);
            sal_memcpy(tunnel_term_config_key.dip6, ip_tunnel_basic_info.dip6[tunnel_idx], 16);
            tunnel_term_config_key.type = ip_tunnel_basic_info.tunnel_type[tunnel_idx];
            tunnel_term_config_key.type = ip_tunnel_tunnel_type_map(unit, tunnel_term_config_key.type);
            if (device_is_jericho2)
            {   /** VRF is also part of the conmfig key on JR2, dip6_mask is necessary for TCAM lookup */
                sal_memcpy(tunnel_term_config_key.dip6_mask, ip_tunnel_basic_info.dip6_mask[tunnel_idx], 16);
                tunnel_term_config_key.vrf = ip_tunnel_basic_info.provider_eth_vrf;
            }

            /* Create terminator config_action struct */
            bcm_tunnel_terminator_config_action_t_init(&tunnel_term_config_action);
            tunnel_term_config_action.tunnel_class = ip_tunnel_basic_info.v6_my_vtep_index[tunnel_idx];

            rv = bcm_tunnel_terminator_config_add(unit, 0, &tunnel_term_config_key, &tunnel_term_config_action);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_tunnel_terminator_config_add, v6_my_vtep_index=%d\n", ip_tunnel_basic_info.v6_my_vtep_index[tunnel_idx]);
            }

            bcm_tunnel_terminator_t_init(&tunnel_term);
            sal_memcpy(tunnel_term.sip6,      ip_tunnel_basic_info.sip6[tunnel_idx],      16);
            sal_memcpy(tunnel_term.sip6_mask, ip_tunnel_basic_info.sip6_mask[tunnel_idx], 16);
            tunnel_term.tunnel_class = ip_tunnel_basic_info.v6_my_vtep_index[tunnel_idx];
            tunnel_term.flags       |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
        }
        else if (ip_tunnel_basic_info.v6_cascade_lookups[tunnel_idx] == 2)
        {
            /*
             * Only aviable for J2:
             * J2:
             *   1) {SIP[127:48]} -> SIPv6-MSBs-ID - lookup in SEM with bcm_tunnel_terminator_create
             *   1) {DIP, VRF, Tunnel type} -> IP-Tunnel ID  - lookup in TCAM with bcm_tunnel_terminator_create (created by MP)
             *   2) {IP-Tunnel ID, SIPv6-MSBs-ID, SIP[47:0]} -> IP-Tunnel In-LIF        - lookup in SEM with bcm_tunnel_terminator_create
             */        
            bcm_tunnel_terminator_t_init(&tunnel_term);
            sal_memcpy(tunnel_term.sip6,      ip_tunnel_basic_info.sip6[tunnel_idx],      16);
            sal_memcpy(tunnel_term.sip6_mask, ip_tunnel_basic_info.sip6_mask[tunnel_idx], 16);
            /* Make sure the tunnel 0 is the MP terminaiton mode */
            tunnel_term.tunnel_class = ip_tunnel_basic_info.term_tunnel_id[0];
            tunnel_term.flags       |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
        }
        /** Create IP tunnel terminator for regular DIP, SIP, VRF, Tunnel-type lookups */
        else
        {
            bcm_tunnel_terminator_t_init(&tunnel_term);
            sal_memcpy(tunnel_term.dip6,      ip_tunnel_basic_info.dip6[tunnel_idx],      16);
            sal_memcpy(tunnel_term.dip6_mask, ip_tunnel_basic_info.dip6_mask[tunnel_idx], 16);
            sal_memcpy(tunnel_term.sip6,      ip_tunnel_basic_info.sip6[tunnel_idx],      16);
            sal_memcpy(tunnel_term.sip6_mask, ip_tunnel_basic_info.sip6_mask[tunnel_idx], 16);
        }
    }
    tunnel_term.vrf = ip_tunnel_basic_info.provider_eth_vrf;
    tunnel_term.type = ip_tunnel_basic_info.tunnel_type[tunnel_idx]; 
    tunnel_term.type = ip_tunnel_tunnel_type_map(unit, tunnel_term.type);
    if (!is_device_or_above(unit, JERICHO2))
    {   /*
         * JR2 tunnel terminator does not support tunnel_if field:
         * In Jericho, tunnel termination LIF has a field RIF, in JR2, the tunnel termination LIF itself is a RIF.
         */
        tunnel_term.tunnel_if = ip_tunnel_basic_info.tunnel_rif[tunnel_idx];
        /*
         * JR2 tunnel terminator does not support port property.
         */
        BCM_PBMP_PORT_SET(tunnel_term.pbmp, ip_tunnel_basic_info.port_property);
    }
    /* check skip ethernet */
    if (ip_tunnel_basic_info.skip_ethernet_flag) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_ETHERNET;
    }
    if (ip_tunnel_basic_info.ing_qos_id) {        
        if (is_device_or_above(unit, JERICHO2)) {
            tunnel_term.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
        } else {
            tunnel_term.flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP | BCM_TUNNEL_TERM_USE_OUTER_TTL;
        }
    }
    /* check term bud */
    if (ip_tunnel_basic_info.term_bud) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_BUD;
    }

    /* check with id */
    if (ip_tunnel_basic_info.term_with_id_flag[tunnel_idx]) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
        tunnel_term.tunnel_id = ip_tunnel_basic_info.term_tunnel_id[tunnel_idx];
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }
    ip_tunnel_basic_info.term_tunnel_id[tunnel_idx] = tunnel_term.tunnel_id;
    if(verbose >= 1) {
        printf("created IP tunnel term on id:0x%08x \n", tunnel_term.tunnel_id);
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
        bcm_l3_ingress_t ing_intf;
        bcm_l3_ingress_t_init(&ing_intf);

        ing_intf.flags = BCM_L3_INGRESS_WITH_ID;        /* must, as we update exist RIF */
        ing_intf.vrf = ip_tunnel_basic_info.tunnel_vrf[tunnel_idx];
        ing_intf.qos_map_id = ip_tunnel_basic_info.ing_qos_id; /* Update ingress qos */

        /** Convert tunnel's GPort ID to intf ID */
        bcm_if_t intf_id;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);

        rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ingress_create \n");
            return rv;
        }
    }

    return rv;
}


