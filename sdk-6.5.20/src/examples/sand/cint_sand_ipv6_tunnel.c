/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/* 
* IPv6 tunnel basic info that can be used for IPv6 tunnel encapsualation/termination and IPv6 VXLAN.
*/ 

/* ********* 
  Globals/Aux Variables
 */
      

struct ipv6_tunnel_basic_info_s{
    /*
     * Common
     */
    int nof_tunnels;                        /* number of tunnels */     
    int access_port;                        /* incoming port from access side */
    int core_port;                          /* incoming port from core side */
    int roo;                                /* indicates routing over overlay */
    int access_eth_rif;                     /* access RIF */
    int access_eth_vrf;                     /* access VRF */  
    int core_eth_rif;                       /* core RIF */    
    int core_eth_vrf;                       /* core VRF */
    int core_native_eth_rif;                /* core native RIF */
    int core_tunnel_term_vrf;
    bcm_tunnel_type_t tunnel_type[3];       /* tunnel type */
    bcm_ip6_t dip6[3];                      /* tunnel DIP */
    bcm_ip6_t dip6_mask[3];                 /* tunnel DIP mask */
    bcm_ip6_t sip6[3];                      /* tunnel SIP */
    bcm_ip6_t sip6_mask[3];                 /* tunnel SIP mask */
    bcm_ip6_t subnet_mask[3];               /* tunnel DIP subnet */
    bcm_mac_t access_eth_rif_mac;           /* mac for out RIF */    
    bcm_mac_t core_eth_rif_mac;             /* mac for out RIF */    
    bcm_mac_t core_native_eth_rif_mac;      /* mac for out RIF */
    /*
     * Encapsulation
     */
    int ttl[3];                             /* ttl */
    bcm_tunnel_dscp_select_t dscp_sel[3];   /* dscp_sel */
    int dscp[3];                            /* dscp */
    int flow_label[3];                      /* flow_label */
    uint16 udp_dest_port[3];                /* udp dest port */  
    bcm_if_t core_tunnel_intf[3];
    bcm_gport_t core_tunnel_id[3];
    bcm_mac_t core_next_hop_mac[3];         /* mac for next core hop */
    int core_arp_id[3];                     /* Id for core ARP entry */
    int core_fec_id[3];                     /* Id for core FEC entry */
    bcm_mac_t core_native_next_hop_mac[3];  /* mac for next core native hop */  
    int core_native_arp_id[3];              /* Id for core native ARP entry */
    int core_native_fec_id[3];              /* Id for core native FEC entry */
    bcm_ip_t core_host_dip;                 /* Host DIP for IPv4 */
    bcm_ip_t core_route_dip;                /* Route DIP for IPv4 */
    bcm_ip_t core_route_dip_mask;           /* mask for route DIP */
    bcm_ip6_t core_host_dip6;               /* Host DIP for IPv6 */
    bcm_ip6_t core_route_dip6;              /* Route DIP for IPv6 */
    bcm_ip6_t core_route_dip6_mask;         /* mask for route DIP */
    int access_vlan;
    int provider_vlan; 
    int ipv6_host_per_tunnel;
    /*
     * Termination
     */
    int port_property;                      /* port property */
    bcm_mac_t access_next_hop_mac[3];       /* mac for next access hop */
    int access_arp_id[3];                   /* Id for access ARP entry */
    int access_fec_id[3];                   /* Id for access FEC entry */
    int tunnel_ifs[3];                      /* RIF, property of the TT inLif. */
    int cascade_lookups[3];                 /* Indicates cascade termination lookup, 1: normal cascade lookup. 2: cascade lookup with same dip and vrf*/
    int my_vtep_index[3];                   /* Used to infer in-lif when doing cascade lookup */
    bcm_gport_t access_tunnel_id[3];
    bcm_ip_t  access_host_dip;              /* Host DIP for IPv4 */
    bcm_ip_t  access_route_dip[3];          /* Route DIP for IPv4 */
    bcm_ip_t  access_route_dip_mask;        /* mask for route DIP */
    bcm_ip6_t access_host_dip6;             /* Host DIP for IPv6 */
    bcm_ip6_t access_route_dip6[3];         /* Route DIP for IPv6 */
    bcm_ip6_t access_route_dip6_mask;       /* mask for route DIP */
};

ipv6_tunnel_basic_info_s ipv6_tunnel_basic_info = 
{
    /* nof_tunnels */
    2, 
    /* access_port */
    200,
    /* core_port */
    201,
    /* roo */
    0,
    /* access_eth_rif | access_eth_vrf*/
    100, 2,    
    /* core_eth_rif  | core_eth_vrf */
    10, 3,  
    /* core_native_eth_rif  | core_tunnel_term_vrf */
    15, 4,     
    /* tunnel_type */
    {bcmTunnelTypeIpAnyIn6, bcmTunnelTypeIpAnyIn6, bcmTunnelTypeIpAnyIn6},
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
    /* subnet_mask */
    {
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00},
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00},
    },
    /* access_eth_rif_mac */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /* core_eth_rif_mac */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
    /* core_native_eth_rif_mac */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x22},     
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
    /* core_tunnel_intf */
    {0, 0, 0},
    /* core_tunnel_id */
    {0, 0, 0},
    /*core_next_hop_mac*/
    { 
        {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}, 
        {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d},
        {0x30, 0x00, 0x00, 0x00, 0xcd, 0x1d}
    },    
    /* core_arp_id */
    {5050, 5060, 5070},
    /* core_fec_id */
    {20481, 20483, 20485}, /* Special value for cascaded FEC */   
    /*core_native_next_hop_mac*/
    { 
        {0x01, 0x00, 0x00, 0x00, 0xcd, 0x1d}, 
        {0x21, 0x00, 0x00, 0x00, 0xcd, 0x1d},
        {0x31, 0x00, 0x00, 0x00, 0xcd, 0x1d}
    },    
    /* core_native_arp_id */
    {6050, 6060, 6070},
    /* core_native_fec_id */
    {6001, 6003, 6005}, 
    /* core_host_dip */
    0x7fffff03, 
    /* core_route_dip */
    0x7fffff00,
    /* core_route_dip_mask */
    0xfffffff0,
    /* core_host_dip6 */
    {0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1},
    /*core_route_dip6*/
    {0x35, 0x55, 0x55, 0x55, 0x66, 0x66, 0x66, 0x66, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x88},
    /*core_route_dip6_mask*/
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00},    
    
    /* access_vlan */
    2,  
    /* provider_vlan */
    100,
    /* ipv6_host_per_tunnel */
    0,
    /* port_property */
    9,
    /*access_next_hop_mac*/
    {
        {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01},
        {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02},
        {0x00, 0x0c, 0x00, 0x02, 0x00, 0x03}
    },
    /* access_arp_id */
    {7050, 7060, 7070},
    /* access_fec_id */
    {7001, 7002, 7003},
    /* tunnel_ifs */
    {30, 40, 50},
    /* cascade_lookups */
    {0, 0, 0},
    /* my_vtep_index */
    {1, 2, 3},
    /* access_tunnel_id */
    {0, 0, 0},
    /* access_host_dip */
    {0x01000015},
    /* access_route_dip */
    {0x01000011, 0x01000021,0x01000031}, /* 1.0.0.17, 1.0.0.33, 1.0.0.49 */
    /*  access_route_dip_mask  */
    0xfffffff0,   
    /*  access_host_dip6 */
    {0x20, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x88},
    /*  access_route_dip6 */
    {
        {0x20, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77},
        {0x20, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x88, 0x88},
        {0x20, 0x00, 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x99, 0x99}
    },
    /*  access_route_dip6_mask  */
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};
int verbose = 1;

/*
 * Function that set the port properties for access and core side.
 * Inputs: unit - relevant unit;
 */
int
ipv6_tunnel_set_port_properties(
    int unit)
{
    int rv = BCM_E_NONE;    
    int tunnel_idx;

    if (is_device_or_above(unit, JERICHO2))
    {
        for (tunnel_idx = 0; tunnel_idx < ipv6_tunnel_basic_info.nof_tunnels; tunnel_idx++)
        {
            ipv6_tunnel_basic_info.core_fec_id[tunnel_idx] += 40000;
            ipv6_tunnel_basic_info.core_native_fec_id[tunnel_idx] += 40000;
            ipv6_tunnel_basic_info.access_fec_id[tunnel_idx] += 40000;
        }
    } else if (is_device_or_above(unit, JERICHO_PLUS))
    {
        for (tunnel_idx = 0; tunnel_idx < ipv6_tunnel_basic_info.nof_tunnels; tunnel_idx++)
        {
            ipv6_tunnel_basic_info.core_arp_id[tunnel_idx] += 8192;
            ipv6_tunnel_basic_info.core_native_arp_id[tunnel_idx] += 8192;
            ipv6_tunnel_basic_info.access_arp_id[tunnel_idx] += 8192;
        }
    }
    
    /*
     * set class for both ports
     */
    rv = bcm_port_class_set(unit, ipv6_tunnel_basic_info.access_port, bcmPortClassId, ipv6_tunnel_basic_info.access_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", ipv6_tunnel_basic_info.access_port);
        return rv;
    }

    rv = bcm_port_class_set(unit, ipv6_tunnel_basic_info.core_port, bcmPortClassId, ipv6_tunnel_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", ipv6_tunnel_basic_info.core_port);
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, ipv6_tunnel_basic_info.access_port, ipv6_tunnel_basic_info.access_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, ipv6_tunnel_basic_info.core_port, ipv6_tunnel_basic_info.core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }
    
    /*
     * Set Out-Port default properties
     */
    
    rv = out_port_set(unit, ipv6_tunnel_basic_info.access_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, ipv6_tunnel_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    return rv;
}

/*
 * Configure Ingress interfaces for access side
 * Inputs: unit - relevant unit;
 */
int
ipv6_tunnel_create_l3_interfaces(
    int unit)
{
    int rv = BCM_E_NONE;
    int tunnel_idx;
    int device_is_jericho2;
    l3_ingress_intf ingr_intf;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2\n");
        return rv;
    }

    /*
     * Create routing interface for the routing's IP.
     * We use it as ingress ETH-RIF to perform ETH termination (my-mac procedure), 
     * to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, ipv6_tunnel_basic_info.access_eth_rif,
                             ipv6_tunnel_basic_info.access_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create access_eth_rif\n");
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     */    
    l3_ingress_intf_init(&ingr_intf);
    ingr_intf.intf_id = ipv6_tunnel_basic_info.access_eth_rif;    
    ingr_intf.vrf = ipv6_tunnel_basic_info.access_eth_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set access_eth_rif\n");
    }

    
    /*
     * Create routing interface for the routing's IP.
     * We use it as ingress ETH-RIF to perform ETH termination (my-mac procedure), 
     * to enable IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, ipv6_tunnel_basic_info.core_eth_rif,
                             ipv6_tunnel_basic_info.core_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create core_eth_rif\n");
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     */
    l3_ingress_intf_init(&ingr_intf);
    ingr_intf.intf_id = ipv6_tunnel_basic_info.core_eth_rif;    
    ingr_intf.vrf = ipv6_tunnel_basic_info.core_eth_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_ingress_rif_set core_eth_rif\n");
    }

    /*
     * In DPP (JR1) architecture IP-Tunnel LIF retrieve its RIF properties according to RIF-ID object (from tunnel_if).
     * While in DNX architecture (JR2) RIF properties are retrieved directly from IP-Tunnel LIF.
     */
    if (!device_is_jericho2)
    {
        for (tunnel_idx = 0; tunnel_idx < ipv6_tunnel_basic_info.nof_tunnels; tunnel_idx++)
        {
            /** Create ingress Tunnel interfaces - the function is used for Jericho 1 only. */
            rv = intf_eth_rif_create(unit, ipv6_tunnel_basic_info.tunnel_ifs[tunnel_idx],
                                     ipv6_tunnel_basic_info.core_eth_rif_mac);
            if (rv != BCM_E_NONE)
            {
                printf("Error, intf_eth_rif_create, tunnel_ifs[%d] \n", tunnel_idx);
                return rv;
            }

            /** Set Incoming Tunnel-RIFs properties */
            l3_ingress_intf_init(&ingr_intf);
            ingr_intf.intf_id = ipv6_tunnel_basic_info.tunnel_ifs[tunnel_idx];
            ingr_intf.vrf = ipv6_tunnel_basic_info.access_eth_vrf;
            rv = intf_ingress_rif_set(unit, &ingr_intf);
            if (rv != BCM_E_NONE)
            {
                printf("Error, intf_ingress_rif_set, tunnel_ifs[%d] \n", tunnel_idx);
                return rv;
            }
        }
    }

    if (ipv6_tunnel_basic_info.roo) {
        /*
         * Create routing interface for the routing's IP.
         * We use it as ingress ETH-RIF to perform native-ETH termination (my-mac procedure), 
         * to enable IPv4 routing for this ETH-RIF and to set the VRF.
         */
        rv = intf_eth_rif_create(unit, ipv6_tunnel_basic_info.core_native_eth_rif,
                                 ipv6_tunnel_basic_info.core_native_eth_rif_mac);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_eth_rif_create core_native_eth_rif\n");
            return rv;
        }

        /*
         * Set Incoming ETH-RIF properties, VRF is updated for the rif.
         */
        l3_ingress_intf_init(&ingr_intf);
        ingr_intf.intf_id = ipv6_tunnel_basic_info.core_native_eth_rif;    
        ingr_intf.vrf = ipv6_tunnel_basic_info.core_tunnel_term_vrf;
        rv = intf_ingress_rif_set(unit, &ingr_intf);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_ingress_rif_set core_native_eth_rif\n");
        }
    }

    return rv;
}

/*
 * Function that creates the arp for access side(from core to access outof overlay).
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ipv6_tunnel_create_arp_access(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;
    sand_utils_l3_arp_s arp_structure;

    /*
     * Configure outer ARP entry 
     */
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, ipv6_tunnel_basic_info.access_arp_id[tunnel_idx], 0, 0, 
                                    ipv6_tunnel_basic_info.access_next_hop_mac[tunnel_idx], ipv6_tunnel_basic_info.provider_vlan);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_arp_create\n");
        return rv;
    }
    ipv6_tunnel_basic_info.access_arp_id[tunnel_idx] = arp_structure.l3eg_arp_id;

    return rv;

}

/*
 * Function that creates the arp for core side(from access to core into overlay).
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ipv6_tunnel_create_arp_core(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;
    sand_utils_l3_arp_s arp_structure;

    if (ipv6_tunnel_basic_info.roo) {
        /*
         * Configure native ARP entry 
         */
        sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, ipv6_tunnel_basic_info.core_native_arp_id[tunnel_idx], 0, 0, 
                                        ipv6_tunnel_basic_info.core_native_next_hop_mac[tunnel_idx], ipv6_tunnel_basic_info.provider_vlan);
        rv = sand_utils_l3_arp_create(unit, &arp_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_arp_create\n");
            return rv;
        }
        ipv6_tunnel_basic_info.core_native_arp_id[tunnel_idx] = arp_structure.l3eg_arp_id;
    }
    /*
     * Configure outer ARP entry 
     */
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, ipv6_tunnel_basic_info.core_arp_id[tunnel_idx], 0, 0, 
                                    ipv6_tunnel_basic_info.core_next_hop_mac[tunnel_idx], ipv6_tunnel_basic_info.provider_vlan);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_arp_create\n");
        return rv;
    }
    ipv6_tunnel_basic_info.core_arp_id[tunnel_idx] = arp_structure.l3eg_arp_id;

    return rv;

}

/*
 * Function that creates the arp for access and core side.
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ipv6_tunnel_create_arp(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;

    rv = ipv6_tunnel_create_arp_access(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ipv6_tunnel_create_arp_access\n");
        return rv;
    }

    rv = ipv6_tunnel_create_arp_core(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ipv6_tunnel_create_arp_core\n");
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
ipv6_tunnel_create_fec_access(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;    
    sand_utils_l3_fec_s fec_structure;
    
    /*
     * Create native FEC for routing into overlay:
     *  1) Give a fec id. 
     *  2) Native out rif.
     *  3) Native arp id. 
     *  4) Point to overlay fec.    
     */
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, ipv6_tunnel_basic_info.access_fec_id[tunnel_idx], 0, 0, ipv6_tunnel_basic_info.access_port, 
                                    ipv6_tunnel_basic_info.access_eth_rif, ipv6_tunnel_basic_info.access_arp_id[tunnel_idx]);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_fec_create\n");
        return rv;
    }
    ipv6_tunnel_basic_info.access_fec_id[tunnel_idx] = fec_structure.l3eg_fec_id;

	return rv;
}

/*
 * Function that creates the fec for core side(from access to core into overlay).
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ipv6_tunnel_create_fec_core(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;
    int l3_flags = 0;
    bcm_gport_t gport;
    sand_utils_l3_fec_s fec_structure;

    if (ipv6_tunnel_basic_info.roo) {
        /*
         * Create native FEC for routing into overlay:
         *  1) Give a fec id. 
         *  2) Native out rif.
         *  3) Native arp id. 
         *  4) Point to overlay fec.    
         */
        BCM_GPORT_FORWARD_PORT_SET(gport, ipv6_tunnel_basic_info.core_fec_id[tunnel_idx]);
        sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, ipv6_tunnel_basic_info.core_native_fec_id[tunnel_idx], l3_flags, 0, gport, 
                                    ipv6_tunnel_basic_info.core_native_eth_rif, ipv6_tunnel_basic_info.core_native_arp_id[tunnel_idx]);
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_fec_create\n");
            return rv;
        }
        ipv6_tunnel_basic_info.core_native_fec_id[tunnel_idx] = fec_structure.l3eg_fec_id;
        l3_flags |= BCM_L3_2ND_HIERARCHY;
    }

    /*
     * Create overlay FEC for routing into overlay: 
     * 1) Give a fec id. 
     * 2) Tunnel id of the overlay VXLAN Tunnel. 
     * 3) Tunnel id of the overlay Eth.
     * 4) Out port.
     */
    gport = 0;
    BCM_GPORT_LOCAL_SET(gport, ipv6_tunnel_basic_info.core_port);
    sand_utils_l3_fec_s_common_init(unit, BCM_L3_WITH_ID, 0, &fec_structure, ipv6_tunnel_basic_info.core_fec_id[tunnel_idx], l3_flags, 0, gport, 
                                ipv6_tunnel_basic_info.core_tunnel_intf[tunnel_idx], 0);
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, sand_utils_l3_fec_create\n");
        return rv;
    }
    ipv6_tunnel_basic_info.core_fec_id[tunnel_idx] = fec_structure.l3eg_fec_id;

	return rv;
}

/*
 * Function that creates the fec for access and core side.
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ipv6_tunnel_create_fec(
    int unit,
    int tunnel_idx)
{
    int rv = BCM_E_NONE;
    
    rv = ipv6_tunnel_create_fec_access(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ipv6_tunnel_create_fec_access\n");
        return rv;
    }

    rv = ipv6_tunnel_create_fec_core(unit, tunnel_idx);
    if (rv != BCM_E_NONE)
    {
        printf("Error, ipv6_tunnel_create_fec_core\n");
        return rv;
    }

    return rv;
}

/*
 * Function that map the initiator tunnel type.
 * Inputs: unit - relevant unit;
 * Inputs: tunnel_idx - tunnel index;
 */
int
ipv6_tunnel_tunnel_type_map(
    int unit,
    int type)
{
    int mapped_type = type;

    if (is_device_or_above(unit, JERICHO2)) {
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
int ipv6_tunnel_encap_create(
    int unit,
    int tunnel_idx)
{
    int rv;
    int device_is_jericho2;
    bcm_l3_intf_t l3_intf;
    bcm_tunnel_initiator_t tunnel_init;    
    int ipv6_tunnel_encap_mode;   

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2, device_is_jericho2\n");
        return rv;
    }

    bcm_l3_intf_t_init(&l3_intf);
    bcm_tunnel_initiator_t_init(&tunnel_init);
    sal_memcpy(&(tunnel_init.dip6),&(ipv6_tunnel_basic_info.dip6[tunnel_idx]),16);
    sal_memcpy(&(tunnel_init.sip6),&(ipv6_tunnel_basic_info.sip6[tunnel_idx]),16);
    tunnel_init.flags = 0;
    tunnel_init.dscp_sel = ipv6_tunnel_basic_info.dscp_sel[tunnel_idx];
    tunnel_init.ttl = ipv6_tunnel_basic_info.ttl[tunnel_idx];
    tunnel_init.dscp = ipv6_tunnel_basic_info.dscp[tunnel_idx];
    tunnel_init.flow_label = ipv6_tunnel_basic_info.flow_label[tunnel_idx];
    tunnel_init.type = ipv6_tunnel_basic_info.tunnel_type[tunnel_idx];
    tunnel_init.type = ipv6_tunnel_tunnel_type_map(unit, tunnel_init.type);
    tunnel_init.l3_intf_id = ipv6_tunnel_basic_info.core_arp_id[tunnel_idx];
    if (device_is_jericho2) {
        tunnel_init.encap_access    = bcmEncapAccessTunnel2;
        tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    } else {
        ipv6_tunnel_encap_mode = soc_property_get(unit, "bcm886xx_ip6_tunnel_encapsulation_mode", 1);
        if (ipv6_tunnel_encap_mode == 2) {
            tunnel_init.flags = BCM_TUNNEL_INIT_WIDE;
        }
    }
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, &tunnel_init);
    if(rv != BCM_E_NONE) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }
    ipv6_tunnel_basic_info.core_tunnel_intf[tunnel_idx] = l3_intf.l3a_intf_id;
    ipv6_tunnel_basic_info.core_tunnel_id[tunnel_idx] = tunnel_init.tunnel_id;
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
ipv6_tunnel_term_create(
    int unit,
    int tunnel_idx)
{
    int rv;
    int device_is_jericho2;

    bcm_tunnel_terminator_t               tunnel_term;
    bcm_tunnel_terminator_config_key_t    tunnel_term_config_key;
    bcm_tunnel_terminator_config_action_t tunnel_term_config_action;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, is_device_jericho2, device_is_jericho2 \n");
        return rv;
    }

    /** Create IP tunnel terminator for cascading lookup */
    if (ipv6_tunnel_basic_info.cascade_lookups[tunnel_idx] == 1)
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
        sal_memcpy(tunnel_term_config_key.dip6, ipv6_tunnel_basic_info.dip6[tunnel_idx], 16);
        tunnel_term_config_key.type = ipv6_tunnel_basic_info.tunnel_type[tunnel_idx];
        tunnel_term_config_key.type = ipv6_tunnel_tunnel_type_map(unit, tunnel_term_config_key.type);
        if (device_is_jericho2)
        {   /** VRF is also part of the conmfig key on JR2, dip6_mask is necessary for TCAM lookup */
            sal_memcpy(tunnel_term_config_key.dip6_mask, ipv6_tunnel_basic_info.dip6_mask[tunnel_idx], 16);
            tunnel_term_config_key.vrf = ipv6_tunnel_basic_info.core_eth_vrf;
        }

        /* Create terminator config_action struct */
        bcm_tunnel_terminator_config_action_t_init(&tunnel_term_config_action);
        tunnel_term_config_action.tunnel_class = ipv6_tunnel_basic_info.my_vtep_index[tunnel_idx];

        rv = bcm_tunnel_terminator_config_add(unit, 0, &tunnel_term_config_key, &tunnel_term_config_action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_tunnel_terminator_config_add, my_vtep_index=%d\n", ipv6_tunnel_basic_info.my_vtep_index[tunnel_idx]);
        }

        bcm_tunnel_terminator_t_init(&tunnel_term);
        sal_memcpy(tunnel_term.sip6,      ipv6_tunnel_basic_info.sip6[tunnel_idx],      16);
        sal_memcpy(tunnel_term.sip6_mask, ipv6_tunnel_basic_info.sip6_mask[tunnel_idx], 16);
        tunnel_term.tunnel_class = ipv6_tunnel_basic_info.my_vtep_index[tunnel_idx];
        tunnel_term.flags       |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
    }
    else if (ipv6_tunnel_basic_info.cascade_lookups[tunnel_idx] == 2)
    {
        /*
         * Only aviable for J2:
         * J2:
         *   1) {SIP[127:48]} -> SIPv6-MSBs-ID - lookup in SEM with bcm_tunnel_terminator_create
         *   1) {DIP, VRF, Tunnel type} -> IP-Tunnel ID  - lookup in TCAM with bcm_tunnel_terminator_create (created by MP)
         *   2) {IP-Tunnel ID, SIPv6-MSBs-ID, SIP[47:0]} -> IP-Tunnel In-LIF        - lookup in SEM with bcm_tunnel_terminator_create
         */        
        bcm_tunnel_terminator_t_init(&tunnel_term);
        sal_memcpy(tunnel_term.sip6,      ipv6_tunnel_basic_info.sip6[tunnel_idx],      16);
        sal_memcpy(tunnel_term.sip6_mask, ipv6_tunnel_basic_info.sip6_mask[tunnel_idx], 16);
        /* Make sure the tunnel 0 is the MP terminaiton mode */
        tunnel_term.tunnel_class = ipv6_tunnel_basic_info.access_tunnel_id[0];
        tunnel_term.flags       |= BCM_TUNNEL_TERM_USE_TUNNEL_CLASS;
    }
    /** Create IP tunnel terminator for regular DIP, SIP, VRF, Tunnel-type lookups */
    else
    {
        bcm_tunnel_terminator_t_init(&tunnel_term);
        sal_memcpy(tunnel_term.dip6,      ipv6_tunnel_basic_info.dip6[tunnel_idx],      16);
        sal_memcpy(tunnel_term.dip6_mask, ipv6_tunnel_basic_info.dip6_mask[tunnel_idx], 16);
        sal_memcpy(tunnel_term.sip6,      ipv6_tunnel_basic_info.sip6[tunnel_idx],      16);
        sal_memcpy(tunnel_term.sip6_mask, ipv6_tunnel_basic_info.sip6_mask[tunnel_idx], 16);
    }

    tunnel_term.vrf  = ipv6_tunnel_basic_info.core_eth_vrf;
    tunnel_term.type = ipv6_tunnel_basic_info.tunnel_type[tunnel_idx];
    tunnel_term.type = ipv6_tunnel_tunnel_type_map(unit, tunnel_term.type);
    if (!device_is_jericho2)
    {   /*
         * JR2 tunnel terminator does not support tunnel_if field:
         * In Jericho, tunnel termination LIF has a field RIF, in JR2, the tunnel termination LIF itself is a RIF.
         */
        tunnel_term.tunnel_if = ipv6_tunnel_basic_info.tunnel_ifs[tunnel_idx];
         /*
         * JR2 tunnel terminator does not support port property.
         */
        BCM_PBMP_PORT_SET(tunnel_term.pbmp, ipv6_tunnel_basic_info.port_property);
    }

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }
    ipv6_tunnel_basic_info.access_tunnel_id[tunnel_idx] = tunnel_term.tunnel_id;
    if(verbose >= 1) {
        printf("created IP tunnel term on id:0x%08x \n", tunnel_term.tunnel_id);
    }
    
    if (device_is_jericho2)
    {
        /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
        bcm_l3_ingress_t ing_intf;
        bcm_l3_ingress_t_init(&ing_intf);

        ing_intf.flags = BCM_L3_INGRESS_WITH_ID;        /* must, as we update exist RIF */
        ing_intf.vrf = ipv6_tunnel_basic_info.access_eth_vrf;

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

