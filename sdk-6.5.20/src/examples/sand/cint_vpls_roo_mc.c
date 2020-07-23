/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/* Description of CINT */
/*cints required
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ipmc_route_basic.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/cint_vpls_roo_mc.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint
 * int rv = 0;
 * int unit = 0;
 * int port1 = 200;
 * int port2 = 201;
 * int port3 = 202;
 * int port4 = 203;
 * rv = vpls_roo_mc_main(unit,port1,port2,port3,port4);
 * print rv
 *
 *
 *                              Network topology
 *
 *            
 *           Access                                    Core
 *
 * Ports    access_port_1(200)                       core_port_1(202)
 *          
 *          access_port_2(201)                       core_port_2(203)
 *
 *                      
 *                      Access    ============ >  Tunnel flow
 *
 *
 *                             +------------------+
 *                             |                  |
 *                             |                  |
 *       access_port_1 (200)   |   DA -IPMC comp  |
 *          ------------------>+                  |            
 *       access_eth_rif (40)   |   DIP lookup ----+----------->IPMC Group (6000)
 *                             |                  |
 *                             |                  |
 *                             |                  |
 *                             +------------------+                                                        
 *                                                         +--------------------------------------------+
 *                                                         |                Network copy 1              |
 *                                                         |CUD : core_port_1 + core_native_eth_rif (60)|
 *                                                         |         Routed pkt - Routing modfications  |
 *                                                         |                    - Happen on native hdr  |
 *                       +-------------------------------->+   Tunnel encap :                           |
 *                       |                                 |        MPLS_LABEL -0x4444                  |
 *                       |                                 |        PWE LABEL  -0x6666                  |
 *                       |                                 |                                            |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Network copy 2              |
 *                       |                                 |CUD : core_port_2 + core_native_eth_rif (61)|
 *                       |                                 |         Routed pkt - Routing modfications  |
 *                       |                                 |                    - Happen on native hdr  |
 *                       +-------------------------------->+   Tunnel encap :                           |
 *                       |                                 |        MPLS_LABEL -0x4445                  |
 *                       |                                 |        PWE LABEL  -0x6665                  |
 *                       |                                 |                                            |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Network copy 3              |
 *                       |                                 |   CUD : core_port_2 + access_eth_rif (40)  |
 *                       |                                 |         fallback2bridge:Native HDR unmodify|
 *   +-----------+       +-------------------------------->+   Tunnel encap :                           |
 *   | IPMC Group|       |                                 |        MPLS_LABEL -0x4447                  |
 *   |           |       |                                 |        PWE LABEL  -0x6667                  |
 *   |  6000     +======>+                                 |                                            |
 *   |           |       |                                 +--------------------------------------------+
 *   +-----------+       |
 *                       |
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Access copy 1               |
 *                       |                                 |   CUD :access_port_1 + access_eth_rif (40) |
 *                       |-------------------------------->|        fallback2bridge                     |
 *                       |                                 |        srcport exclude will discard pkt    |
 *                       |                                 |                                            |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Access copy 2               |
 *                       |                                 |   CUD :access_port_2 + access_eth_rif (41) |
 *                       +-------------------------------->|        routed pkt:native hdr modified      |
 *                       |                                 |                                            |
 *                       |                                 |                                            |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Access copy 3               |
 *                       |                                 |   CUD :access_port_1 + access_eth_rif (42) |
 *                       +-------------------------------->|       routed pkt:native hdr modified       |
 *                       |                                 |                                            |
 *                       |                                 |                                            |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Access copy 4               |
 *                       |                                 |   CUD :access_port_2 + access_eth_rif (40) |
 *                       +-------------------------------->|     fallbak2bridge : native hdr unmodified |
 *                                                         |                                            |
 *                                                         |                                            |
 *                                                         +--------------------------------------------+
 * 
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 *                              Tunnel  ===================> Access flow
 *
 *   TX Pkt : IPv4oETH0oMPLSoMPLSoETH0 
 *            Outer ETH - UC
 *            MPLS Label - 0x4445
 *            PWE label -0x6667
 *            Inner ETH - Compatible MC
 *
 *                        +-------------------+              +-------------------+
 *                        |                   |              |                   |
 *                        | VT1 : OETH Term   |              | Inner ETH :       |
 *          core_port_2   | MPLS_LABEL -POP   |              |  Compatible MC    |
 *            ----------->|                   |------------->|                   |--------->IPMC Group 2
 *             txpkt      | PWE_LABEL =>      |              |   DIP Lookup      |            (7000)
 *                        |(native_eth_rif +3)|              |                   |
 *                        |                   |              |                   |
 *                        +-------------------+              +-------------------+
 * 
 *                                                         +--------------------------------------------+
 *                                                         |                Access copy 1               |
 *                                                         |   CUD : Access_Port_1 + access_eth_rif (40)|
 *                                                         |         Routed pkt - Routing modfications  |
 *                       +-------------------------------->+   Tunnel decap :                           |
 *                       |                                 |        MPLS_LABEL -0x4445 - POP            |
 *                       |                                 |        PWE LABEL  -0x6667 ->               |
 *                       |                                 |                    (core_native_eth_rif +3)|
 *                       |                                 +--------------------------------------------+
 *                       +  
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Access copy 2               |
 *                       |                                 |   CUD : Access_Port_2 + access_eth_rif (41)|
 *                       |                                 |         Routed pkt - Routing modfications  |
 *                       +-------------------------------->+   Tunnel decap :                           |
 *                       |                                 |        MPLS_LABEL -0x4445 - POP            |
 *                       |                                 |        PWE LABEL  -0x6667 ->               |
 *                       |                                 |                    (core_native_eth_rif +3)|
 *                       |                                 +--------------------------------------------+
 *   +-----------+       +
 *   | IPMC Group|       |
 *   |           |       |
 *   |  7000     +======>|                                 +--------------------------------------------+
 *   |           |       +                                 |                Access copy 3               |
 *   |           |       |                                 |   CUD : Access_Port_2+core_native_eth_rif+3|
 *   |           |       |-------------------------------->+   Tunnel decap :                           |
 *   +-----------+       |                                 |        MPLS_LABEL -0x4445 - POP            |
 *                       |                                 |        PWE LABEL  -0x6667 ->               |
 *                       +                                 |                    (core_native_eth_rif +3)|
 *                       +                                 |  intf match fall back2bridge               |
 *                       |                                 +--------------------------------------------+
 *                       |
 *                       +
 *                       |
 *                       |                                 +--------------------------------------------+
 *                       |                                 |                Network copy 1              |
 *                       |                                 |   CUD : core_Port_1+core_native_eth_rif+1  |
 *                       +                                 +   Tunnel decap :                           |
 *                       |                                 |        MPLS_LABEL -0x4445 - POP            |
 *                       +-------------------------------->|        PWE LABEL  -0x6667 ->               |
 *                                                         |                    (core_native_eth_rif +3)|
 *                                                         |                                            |
 *                                                         |  Split horixzon check will happen          |
 *                                                         |  pkt gets dropped                          |
 *                                                          +--------------------------------------------+
 *
 * Note :
 *      Mostly MAC address are reused and incremented the same mac id for different copies
 */

/* Global Definition */
int NUM_OF_CORE_PORTS = 2;
int NUM_OF_ACCESS_PORTS = 2;

int core_arp_flags2 = 0;

/* Main struct definition */

struct cint_vpls_roo_mc_info_s
{
    int access_port[NUM_OF_ACCESS_PORTS];       /* Access ports */
    int core_port[NUM_OF_CORE_PORTS];   /* core ports */
    /*
     * access port details
     */
    int access_eth_rif;         /* access eth rif */
    bcm_mac_t access_eth_rif_mac;       /* access eth rif mac */
    bcm_mac_t access_next_hop_mac;      /* access next hop mac */
    int access_arp_id[NUM_OF_ACCESS_PORTS];     /* arp id for the native header update */
    /*
     * core overlay properties
     */
    int core_overlay_eth_rif;   /* core overlay eth rif */
    bcm_mac_t core_overlay_eth_rif_mac; /* core overlay eth rif mac */
    bcm_mac_t core_overlay_next_hop_mac;        /* core overlay next hop mac */
    int core_overlay_arp_id[NUM_OF_CORE_PORTS + 1];     /* core arp id for overlay hdr update Here we send 3 encap pkts 
                                                         * into core (2routed + 1 fall backbridge) */

    /*
     * core native properties
     */
    int core_native_eth_rif;    /* core native eth rif */
    bcm_mac_t core_native_eth_rif_mac;  /* core native eth rif mac */
    bcm_mac_t core_native_next_hop_mac; /* core native next hop mac */
    int core_native_arp_id[NUM_OF_CORE_PORTS + 1];      /* core arp id for core native hdr update Here we send 3 encap
                                                         * pkts into core (2routed + 1 fall backbridge) */

    /*
     * MPLS related parameter
     */
    bcm_mpls_label_t core_overlay_mpls_tunnel_label_1;  /* pushed label1 */
    bcm_mpls_label_t core_overlay_mpls_tunnel_label_2;  /* pushed label2 */
    bcm_mpls_label_t core_overlay_pwe_label;    /* pwe label */
    bcm_if_t core_overlay_mpls_tunnel_id[NUM_OF_CORE_PORTS + 1];        /* Tunnel label encap pointer */
    bcm_gport_t core_overlay_mpls_port_id_eg[NUM_OF_CORE_PORTS + 1];
    bcm_if_t core_overlay_pwe_encap_id[NUM_OF_CORE_PORTS + 1];  /* pwe label encap id */

    uint32 dip1;                /* ip dip */
    uint32 sip1;                /* ip sip */
    uint32 dip_mask;
    uint32 sip_mask;
    int ipmc_group1;            /* ipmc group for access to core flow */
    bcm_mac_t access_my_mac;
    bcm_gport_t core_overlay_mpls_port_id_ing;  /* mpls_port id for termination */
    uint32 dip2;                /* ip dip for termination */
    uint32 sip2;                /* ip sip for ip termination */
    int ipmc_group2;            /* ipmc group for core to access flow */
};

/* main struct initialization */
cint_vpls_roo_mc_info_s cint_vpls_roo_mc_info = {
    /*
     * access_port , core_port 
     */
    {200, 201}, {202, 203},
    /*
     * access_eth_rif
     */
    40,
    /*
     * access_eth_rif_mac
     */
    {0x01, 0x00, 0x5E, 0x48, 0xC8, 0x02},
    /*
     * access_next_hop_mac
     */
    {0x00, 0x12, 0x34, 0x56, 0x78, 0x90},
    /*
     * access_arp_id
     */
    {0, 0},
    /*
     * core_overlay_eth_rif
     */
    50,
    /*
     * core_overlay_eth_rif_mac
     */
    {0x00, 0x00, 0xAB, 0xCD, 0xEF, 0x00},
    /*
     * core_overlay_next_hop_mac
     */
    {0x00, 0xFF, 0xAB, 0xCD, 0xEF, 0x00},
    /*
     * core_overlay_arp_id
     */
    {0, 0, 0},
    /*
     * core_native_eth_rif
     */
    60,
    /*
     * core_native_eth_rif_mac
     */
    {0x00, 0x00, 0xFE, 0xDC, 0xBA, 0x00},
    /*
     * core_native_next_hop_mac
     */
    {0x00, 0x11, 0xAB, 0xCD, 0xEF, 0x00},
    /*
     * core_native_arp_id
     */
    {0, 0, 0},
    /*
     * core_overlay_mpls_tunnel_label_1
     */
    0x4444,
    /*
     * core_overlay_mpls_tunnel_label_2
     */
    0x5555,
    /*
     * core_overlay_pwe_label
     */
    0x6666,
    /*
     * core_overlay_mpls_tunnel_id
     */
    {8194, 8195, 8196},
    /*
     * core_overlay_mpls_port_id_eg
     */
    {0, 0, 0},
    /*
     * core_overlay_pwe_encap_id
     */
    {0, 0, 0},
    /*
     * dip1
     */
    0xE0C8C802,
    /*
     * sip1
     */
    0x7FFFFF01,
    /*
     * dip_mask
     */
    0xFFFFFFFF,
    /*
     * sip_mask
     */
    0,
    /*
     * ipmc_group1
     */
    6000,
    /*
     * access_my_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0x12, 0x34},
    /*
     * core_overlay_mpls_port_id_ing
     */
    0,
    /*
     * dip2
     */
    0xE0C8C804,
    /*
     * sip2
     */
    0x7FFFFF03,
    /*
     * ipmc_group2
     */
    7000,
};

/* Init call*/
void
vpls_roo_mc_init(
    int access_port_1,
    int access_port_2,
    int core_port_1,
    int core_port_2)
{
    /*
     * Access ports 
     */
    cint_vpls_roo_mc_info.access_port[0] = access_port_1;
    cint_vpls_roo_mc_info.access_port[1] = access_port_2;
    /*
     * Core ports 
     */
    cint_vpls_roo_mc_info.core_port[0] = core_port_1;
    cint_vpls_roo_mc_info.core_port[1] = core_port_2;
}

/* Create port properties*/
int
vpls_roo_mc_configure_port_properties(
    int unit,
    int *access_port,
    int *core_port)
{
    int rv = BCM_E_NONE;
    int i;

    /*
     * create vlan domain
     */

    for (i = 0; i < NUM_OF_CORE_PORTS; i++)
    {
        rv = bcm_port_class_set(unit, core_port[i], bcmPortClassId, core_port[i]);
        printf("core_port=%d, vlan domain =%d\n", core_port[i], core_port[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set, port=%d, \n", core_port[i]);
            return rv;
        }

        rv = in_port_intf_set(unit, cint_vpls_roo_mc_info.core_port[i], cint_vpls_roo_mc_info.core_overlay_eth_rif + i);
        printf("core_port=%d, overlay_eth_rif =%d\n", core_port[i], cint_vpls_roo_mc_info.core_overlay_eth_rif + i);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in_port_intf_set intf_in\n");
            return rv;
        }

        rv = out_port_set(unit, cint_vpls_roo_mc_info.core_port[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, out_port_set intf_out\n");
            return rv;
        }
    }

    /*
     * Core fall back to bridge
     */
    /*
     * Core port with rif same as incoming rif
     */
    rv = in_port_intf_set(unit, cint_vpls_roo_mc_info.core_port[0], cint_vpls_roo_mc_info.access_eth_rif);
    printf("core fall2bridge : core_port=%d, rif =%d\n", cint_vpls_roo_mc_info.core_port[0],
           cint_vpls_roo_mc_info.access_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * create ingress eth rif for the ports
     */

    for (i = 0; i < NUM_OF_ACCESS_PORTS; i++)
    {
        rv = bcm_port_class_set(unit, access_port[i], bcmPortClassId, access_port[i]);
        printf("access_port=%d, vlan domain =%d\n", access_port[i], access_port[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set, port=%d, \n", access_port);
            return rv;
        }
        /*
         * Access port
         */
        rv = in_port_intf_set(unit, cint_vpls_roo_mc_info.access_port[i], (cint_vpls_roo_mc_info.access_eth_rif + i));
        printf("access_port=%d, eth_rif =%d\n", cint_vpls_roo_mc_info.access_port[i],
               (cint_vpls_roo_mc_info.access_eth_rif + i));
        if (rv != BCM_E_NONE)
        {
            printf("Error, in_port_intf_set intf_in\n");
            return rv;
        }

        /*
         * Set Out-Port default properties 
         */
        rv = out_port_set(unit, cint_vpls_roo_mc_info.access_port[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, out_port_set intf_out\n");
            return rv;
        }
    }

    return rv;
}

/* create VSI*/
int
vpls_roo_mc_create_eth_rifs(
    int unit)
{

    int rv;
    int i;

    /*
     * create vlan based on the vsi (vpn) 
     */
    for (i = 0; i < NUM_OF_CORE_PORTS; i++)
    {
        rv = bcm_vlan_create(unit, (cint_vpls_roo_mc_info.core_native_eth_rif + i));
        printf("bcm_vlan_create : %d\n", (cint_vpls_roo_mc_info.core_native_eth_rif + i));
        if (rv != BCM_E_NONE)
        {
            printf("Failed (%d) to create VLAN %d\n", rv, (cint_vpls_roo_mc_info.core_native_eth_rif + i));
            return rv;
        }

        /*
         * create vlan based on the vsi (vpn) 
         */
        rv = bcm_vlan_create(unit, (cint_vpls_roo_mc_info.core_overlay_eth_rif + i));
        printf("bcm_vlan_create : %d\n", (cint_vpls_roo_mc_info.core_overlay_eth_rif + i));
        if (rv != BCM_E_NONE)
        {
            printf("Failed (%d) to create VLAN %d\n", rv, (cint_vpls_roo_mc_info.core_overlay_eth_rif + i));
            return rv;
        }
    }

    /*
     * create vlan based on the vsi (vpn) 
     */
    for (i = 0; i < NUM_OF_ACCESS_PORTS; i++)
    {
        rv = bcm_vlan_create(unit, (cint_vpls_roo_mc_info.access_eth_rif + i));
        printf(" ACCESS bcm_vlan_create : %d\n", (cint_vpls_roo_mc_info.access_eth_rif + i));
        if (rv != BCM_E_NONE)
        {
            printf("Failed (%d) to create VLAN %d\n", rv, (cint_vpls_roo_mc_info.access_eth_rif + i));
            return rv;
        }
    }

    rv = bcm_vlan_create(unit, (cint_vpls_roo_mc_info.core_native_eth_rif + 3));
    printf("bcm_vlan_create : %d\n", (cint_vpls_roo_mc_info.core_native_eth_rif + 3));
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, (cint_vpls_roo_mc_info.core_native_eth_rif + i));
        return rv;
    }

    return rv;
}

int
vpls_roo_mc_create_l3_interfaces(
    int unit)
{

    int rv;
    int i;
    bcm_mac_t temp_mac;
    bcm_l3_ingress_t ingress_l3_interface;
    bcm_l3_ingress_t_init(&ingress_l3_interface);

    for (i = 0; i < NUM_OF_CORE_PORTS; i++)
    {
        temp_mac = cint_vpls_roo_mc_info.core_native_eth_rif_mac;
        temp_mac[5] = temp_mac[5] + i;
        rv = intf_eth_rif_create(unit, cint_vpls_roo_mc_info.core_native_eth_rif + i, temp_mac);
        printf("intf_eth_rif_create: native_rif =%d ", cint_vpls_roo_mc_info.core_native_eth_rif + i);
        vpls_roo_mc_print_mac(temp_mac);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_eth_rif_create pwe_intf mac%d\n", temp_mac);
        }

        temp_mac = cint_vpls_roo_mc_info.core_overlay_eth_rif_mac;
        temp_mac[5] = temp_mac[5] + i;
        rv = intf_eth_rif_create(unit, cint_vpls_roo_mc_info.core_overlay_eth_rif + i, temp_mac);
        printf("intf_eth_rif_create : overlay_rif =%d ", cint_vpls_roo_mc_info.core_overlay_eth_rif + i);
        vpls_roo_mc_print_mac(temp_mac);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_eth_rif_create pwe_intf\n");
        }

    }

    temp_mac = cint_vpls_roo_mc_info.core_overlay_eth_rif_mac;
    temp_mac[5] = temp_mac[5] + 2;
    rv = intf_eth_rif_create(unit, cint_vpls_roo_mc_info.core_overlay_eth_rif + 2, temp_mac);
    printf("intf_eth_rif_create : rif =%d ", cint_vpls_roo_mc_info.core_overlay_eth_rif + 2);
    vpls_roo_mc_print_mac(temp_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
    }

    for (i = 0; i < NUM_OF_ACCESS_PORTS; i++)
    {
        temp_mac = cint_vpls_roo_mc_info.access_my_mac;
        temp_mac[5] = temp_mac[5] + i;
        rv = intf_eth_rif_create(unit, cint_vpls_roo_mc_info.access_eth_rif + i, temp_mac);
        printf("intf_eth_rif_create : rif =%d ", cint_vpls_roo_mc_info.access_eth_rif + i);
        vpls_roo_mc_print_mac(temp_mac);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_eth_rif_create pwe_intf\n");
        }
    }

    /*
     * routed copy on src port
     */
    temp_mac = cint_vpls_roo_mc_info.access_my_mac;
    temp_mac[5] = temp_mac[5] + 2;
    rv = intf_eth_rif_create(unit, cint_vpls_roo_mc_info.access_eth_rif + 2, temp_mac);
    printf("intf_eth_rif_create : rif =%d ", cint_vpls_roo_mc_info.access_eth_rif + 2);
    vpls_roo_mc_print_mac(temp_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
    }

    temp_mac = cint_vpls_roo_mc_info.core_native_eth_rif_mac;
    temp_mac[5] = temp_mac[5] + 1;
    rv = intf_eth_rif_create(unit, cint_vpls_roo_mc_info.core_native_eth_rif + 3, temp_mac);
    printf("intf_eth_rif_create : rif =%d ", cint_vpls_roo_mc_info.core_native_eth_rif + 3);
    vpls_roo_mc_print_mac(temp_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf mac%d\n", temp_mac);
    }

    /*
     * set vrf for the access rif
     */
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.intf_id = cint_vpls_roo_mc_info.access_eth_rif;
    ingr_itf.vrf = cint_vpls_roo_mc_info.access_eth_rif;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * Set vrf for native eth rif for tunnel to access flow
     */
    ingr_itf.intf_id = cint_vpls_roo_mc_info.core_native_eth_rif + 3;
    ingr_itf.vrf = cint_vpls_roo_mc_info.core_native_eth_rif + 3;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    return rv;
}
/* create ARP entries*/
int
vpls_roo_mc_create_arp_entries(
    int unit)
{
    int rv;
    int i;
    bcm_mac_t temp_mac;
    create_l3_egress_s l3_egress;

    for (i = 0; i < NUM_OF_CORE_PORTS; i++)
    {
        /*
         * Configure native ARP entry 
         */
        temp_mac = cint_vpls_roo_mc_info.core_native_next_hop_mac;
        temp_mac[5] = temp_mac[5] + i;
        
        l3_egress.arp_encap_id = 0;
        l3_egress.l3_flags = BCM_L3_NATIVE_ENCAP;
        l3_egress.next_hop_mac_addr = temp_mac;
        l3_egress.vlan = cint_vpls_roo_mc_info.core_native_eth_rif + i;

        rv = l3__egress_only_encap__create(unit, &l3_egress);

        cint_vpls_roo_mc_info.core_native_arp_id[i] = l3_egress.arp_encap_id;

        printf("l3__egress_only_encap__create: arp_id =%d,rif=%d", cint_vpls_roo_mc_info.core_native_arp_id[i],
               cint_vpls_roo_mc_info.core_native_eth_rif + i);
        vpls_roo_mc_print_mac(temp_mac);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create AC egress object ARP only\n");
            return rv;
        }

        /*
         * Configure overlay ARP entry 
         */
        temp_mac = cint_vpls_roo_mc_info.core_overlay_next_hop_mac;
        temp_mac[5] = temp_mac[5] + i;

        l3_egress.arp_encap_id = 0;
        l3_egress.l3_flags = 0;
        l3_egress.next_hop_mac_addr = temp_mac;
        l3_egress.vlan = cint_vpls_roo_mc_info.core_overlay_eth_rif + i;
        l3_egress.l3_flags2 |= core_arp_flags2;

        rv = l3__egress_only_encap__create(unit, &l3_egress);

        cint_vpls_roo_mc_info.core_overlay_arp_id[i] = l3_egress.arp_encap_id;

        printf("l3__egress_only_encap__create: arp_id =%d,rif=%d", cint_vpls_roo_mc_info.core_overlay_arp_id[i],
               cint_vpls_roo_mc_info.core_overlay_eth_rif + i);
        vpls_roo_mc_print_mac(temp_mac);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create AC egress object ARP only\n");
            return rv;
        }
    }

    /*
     * Network fall back to bridge copy
     */
    temp_mac = cint_vpls_roo_mc_info.core_native_next_hop_mac;
    temp_mac[5] = temp_mac[5] + 2;

    l3_egress.arp_encap_id = 0;
    l3_egress.l3_flags = BCM_L3_NATIVE_ENCAP;
    l3_egress.next_hop_mac_addr = temp_mac;
    l3_egress.vlan = cint_vpls_roo_mc_info.access_eth_rif;

    rv = l3__egress_only_encap__create(unit, &l3_egress);

    cint_vpls_roo_mc_info.core_native_arp_id[2] = l3_egress.arp_encap_id;

    printf("l3__egress_only_encap__create: core fall2bridge arp_id =%d,rif=%d",
           cint_vpls_roo_mc_info.core_native_arp_id[2], cint_vpls_roo_mc_info.access_eth_rif);
    vpls_roo_mc_print_mac(temp_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }

    /*
     * Configure overlay ARP entry 
     */
    temp_mac = cint_vpls_roo_mc_info.core_overlay_next_hop_mac;
    temp_mac[5] = temp_mac[5] + 2;

    l3_egress.arp_encap_id = 0;
    l3_egress.l3_flags = 0;
    l3_egress.next_hop_mac_addr = temp_mac;
    l3_egress.vlan = cint_vpls_roo_mc_info.core_overlay_eth_rif + 2;
    l3_egress.l3_flags2 |= core_arp_flags2;

    rv = l3__egress_only_encap__create(unit, &l3_egress);

    cint_vpls_roo_mc_info.core_overlay_arp_id[2] = l3_egress.arp_encap_id;

    printf("l3__egress_only_encap__create: arp_id =%d,rif=%d", cint_vpls_roo_mc_info.core_overlay_arp_id[2],
           cint_vpls_roo_mc_info.core_overlay_eth_rif + 2);
    vpls_roo_mc_print_mac(temp_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }

    /*
     * Configure access ARP entry 
     */
    for (i = 0; i < NUM_OF_ACCESS_PORTS; i++)
    {
        temp_mac = cint_vpls_roo_mc_info.access_next_hop_mac;
        temp_mac[5] = temp_mac[5] + i;

        l3_egress.arp_encap_id = 0;
        l3_egress.l3_flags = 0;
        l3_egress.next_hop_mac_addr = temp_mac;
        l3_egress.vlan = cint_vpls_roo_mc_info.access_eth_rif + i;

        rv = l3__egress_only_encap__create(unit, &l3_egress);

        cint_vpls_roo_mc_info.access_arp_id[i] = l3_egress.arp_encap_id;

        printf("l3__egress_only_encap__create: arp_id =%d,rif=%d", cint_vpls_roo_mc_info.access_arp_id[i],
               cint_vpls_roo_mc_info.access_eth_rif + i);
        vpls_roo_mc_print_mac(temp_mac);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create AC egress object ARP only\n");
            return rv;
        }
    }

    return rv;

}
/* Create MPLS tunnel entry*/
int
vpls_roo_mc_create_mpls_tunnel(
    int unit,
    bcm_mpls_label_t outer_label,
    bcm_mpls_label_t inner_label,
    int num_labels,
    int arp_id,
    bcm_if_t * tunnel_id)
{

    bcm_mpls_egress_label_t label_array[2];
    int rv;

    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    /*
     * Label to be pushed as part of the MPLS tunnel.
     */
    label_array[0].label = outer_label;

    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, *tunnel_id);
    if (num_labels == 2)
    {
        label_array[1].label = inner_label;
        label_array[1].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        BCM_L3_ITF_SET(label_array[1].tunnel_id, BCM_L3_ITF_TYPE_LIF, *tunnel_id);
    }
    /*
     * Set the next pointer of this entry to be the arp. This configuration is new compared to Jericho, where the arp
     * pointer used to be connected to the EEDB entry via bcm_l3_egress_create (with egress flag indication).
     */
    label_array[0].l3_intf_id = arp_id;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *tunnel_id = label_array[0].tunnel_id;

    return rv;

}

/* Create PWE label */
int
vpls_roo_mc_mpls_port_add_encapsulation(
    int unit)
{
    int rv = BCM_E_NONE;
    int i;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
    mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    for (i = 0; i < NUM_OF_CORE_PORTS + 1; i++)
    {
        mpls_port.egress_label.label = cint_vpls_roo_mc_info.core_overlay_pwe_label + i;
        rv = bcm_mpls_port_add(unit, 0, mpls_port);
        printf("mpls_port_add_encapsulation : label=%x lif=%x\n", cint_vpls_roo_mc_info.core_overlay_pwe_label + i, mpls_port.encap_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
            return rv;
        }

        cint_vpls_roo_mc_info.core_overlay_mpls_port_id_eg[i] = mpls_port.mpls_port_id;
        cint_vpls_roo_mc_info.core_overlay_pwe_encap_id[i] = mpls_port.encap_id;
    }

    return rv;
}

/* create fwd lookup entres*/
int
vpls_roo_mc_fwd_lookup_entires_create(
    int unit)
{
    int rv = BCM_E_NONE;

    /*
     * Add IPMC route in LEM for Native to tunnel flow
     */
    rv = add_ipv4_ipmc(unit, cint_vpls_roo_mc_info.dip1, cint_vpls_roo_mc_info.dip_mask, cint_vpls_roo_mc_info.sip1,
                       cint_vpls_roo_mc_info.sip_mask, cint_vpls_roo_mc_info.access_eth_rif,
                       cint_vpls_roo_mc_info.access_eth_rif, cint_vpls_roo_mc_info.ipmc_group1, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv4_ipmc to LEM table, \n");
        return rv;
    }

    /*
     * Add KAPS entry for tunnel to native flow
     */
    rv = add_ipv4_ipmc(unit, cint_vpls_roo_mc_info.dip2, cint_vpls_roo_mc_info.dip_mask, cint_vpls_roo_mc_info.sip2,
                       cint_vpls_roo_mc_info.sip_mask, (cint_vpls_roo_mc_info.core_native_eth_rif + 3),
                       (cint_vpls_roo_mc_info.core_native_eth_rif + 3), cint_vpls_roo_mc_info.ipmc_group2, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_ipv4_ipmc to KAPS table, \n");
        return rv;
    }

    return rv;

}

/* create mc group for native to tunnel scenario */
int
vpls_roo_mc_create_ipmc_group1(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_if_t ifs[3];
    int i;
    bcm_if_t rep_idx = 0x300100;
    int system_headers_mode;
    /** JR2: Create fabric MC for each core in case of egress Multicast. */
    system_headers_mode = soc_property_get(unit, "system_headers_mode", 1);

    /*
     * mcid create
     */
    int flags;
    flags = BCM_MULTICAST_WITH_ID | (system_headers_mode?BCM_MULTICAST_INGRESS_GROUP:BCM_MULTICAST_EGRESS_GROUP);
    rv = bcm_multicast_create(unit, flags, &cint_vpls_roo_mc_info.ipmc_group1);


    if ((system_headers_mode == 0) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
    {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1; /* Ignore it in single core devices*/
        rv = bcm_fabric_multicast_set(unit, cint_vpls_roo_mc_info.ipmc_group1, 0, nof_cores, mreps);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_multicast_set \n");
            return rv;
        }
    }


    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_create\n");
        return rv;
    }

    /*
     * Add ports to mc group
     */
    bcm_multicast_replication_t rep_array;
    bcm_multicast_replication_t_init(&rep_array);

    /*
     * Network copies - to both core ports
     */
    for (i = 0; i < NUM_OF_CORE_PORTS; i++)
    {
        /* Network Copies 1, 2 */
        rep_array.port = cint_vpls_roo_mc_info.core_port[i];
        rep_array.encap1 = rep_idx++;
        rv = bcm_multicast_add(unit, cint_vpls_roo_mc_info.ipmc_group1,system_headers_mode?BCM_MULTICAST_INGRESS_GROUP:BCM_MULTICAST_EGRESS_GROUP, 1, &rep_array);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_multicast_add \n");
            return rv;
        }
        printf("MC Group (0x%x): Added Port=0x%x, Rep_Idx=0x%x\n", cint_vpls_roo_mc_info.ipmc_group1,
            rep_array.port, rep_array.encap1);
        BCM_L3_ITF_SET(ifs[0], BCM_L3_ITF_TYPE_RIF, cint_vpls_roo_mc_info.core_native_eth_rif + i);
        BCM_L3_ITF_SET(ifs[1], BCM_L3_ITF_TYPE_LIF, cint_vpls_roo_mc_info.core_overlay_pwe_encap_id[i]);
        ifs[2] = cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_id[i];
        rv = bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &rep_array.encap1, 3, ifs);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_multicast_encap_extension_create \n");
            return rv;
        }
        printf("PPMC: CUD=0x%x  -->  ITF0=0x%x, ITF1=0x%x, ITF2=0x%x\n", rep_array.encap1, ifs[0], ifs[1], ifs[2]);

    }

    /* Network copy 3 */
    rep_array.port = cint_vpls_roo_mc_info.core_port[1];
    rep_array.encap1 = rep_idx++;
    rv = bcm_multicast_add(unit, cint_vpls_roo_mc_info.ipmc_group1, system_headers_mode?BCM_MULTICAST_INGRESS_GROUP:BCM_MULTICAST_EGRESS_GROUP, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add \n");
        return rv;
    }
    printf("MC Group (0x%x): Added Port=0x%x, Rep_Idx=0x%x\n", cint_vpls_roo_mc_info.ipmc_group1,
        rep_array.port, rep_array.encap1);
    BCM_L3_ITF_SET(ifs[0], BCM_L3_ITF_TYPE_RIF, cint_vpls_roo_mc_info.access_eth_rif);
    BCM_L3_ITF_SET(ifs[1], BCM_L3_ITF_TYPE_LIF, cint_vpls_roo_mc_info.core_overlay_pwe_encap_id[i]);
    ifs[2] = cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_id[i];
    rv = bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &rep_array.encap1, 3, ifs);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_encap_extension_create \n");
        return rv;
    }
    printf("PPMC: CUD=0x%x  -->  ITF0=0x%x, ITF1=0x%x, ITF2=0x%x\n", rep_array.encap1, ifs[0], ifs[1], ifs[2]);

    /*
     * Access copies On Src port fall back to bridge copy On non source port routed copy
     */
    for (i = 0; i < NUM_OF_ACCESS_PORTS; i++)
    {
        /* Access Copies 1, 2 */
        rep_array.port = cint_vpls_roo_mc_info.access_port[i];
        rep_array.encap1 = ((cint_vpls_roo_mc_info.access_eth_rif + i) & 0x3fffff);
        rv = bcm_multicast_add(unit, cint_vpls_roo_mc_info.ipmc_group1, system_headers_mode?BCM_MULTICAST_INGRESS_GROUP:BCM_MULTICAST_EGRESS_GROUP, 1, &rep_array);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_multicast_add \n");
            return rv;
        }
        printf("MC Group (0x%x): Added Port=0x%x, Rep_Idx=0x%x\n", cint_vpls_roo_mc_info.ipmc_group1,
            rep_array.port, rep_array.encap1);
    }

    /*
     * Routed copy on source port
     */
    /* Access Copy 3 */
    rep_array.port = cint_vpls_roo_mc_info.access_port[0];
    rep_array.encap1 = ((cint_vpls_roo_mc_info.access_eth_rif + 2) & 0x3fffff);
    rv = bcm_multicast_add(unit, cint_vpls_roo_mc_info.ipmc_group1, system_headers_mode?BCM_MULTICAST_INGRESS_GROUP:BCM_MULTICAST_EGRESS_GROUP, 1, &rep_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_add route to source port");
        return rv;
    }
    printf("MC Group (0x%x): Added Port=0x%x, Rep_Idx=0x%x\n", cint_vpls_roo_mc_info.ipmc_group1,
        rep_array.port, rep_array.encap1);


    /* Access Copy 4 */
    rep_array.port = cint_vpls_roo_mc_info.access_port[1];
    rep_array.encap1 = ((cint_vpls_roo_mc_info.access_eth_rif) & 0x3fffff);
    rv = bcm_multicast_add(unit,cint_vpls_roo_mc_info.ipmc_group1 , system_headers_mode?BCM_MULTICAST_INGRESS_GROUP:BCM_MULTICAST_EGRESS_GROUP, 1, &rep_array);

    if (rv != BCM_E_NONE)
    {
         printf("Error, in bcm_multicast_add \n");
         return rv;
    }
    printf("MC Group (0x%x): Added Port=0x%x, Rep_Idx=0x%x\n", cint_vpls_roo_mc_info.ipmc_group1,
        rep_array.port, rep_array.encap1);

    return rv;
}

int
vpls_roo_mc_create_termination_stack(
    int unit,
    bcm_mpls_label_t * labels,
    int nof_labels)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv;
    int i;
    /*
     * Create a stack of MPLS labels to be terminated 
     */

    for (i = 0; i < nof_labels; i++)
    {
        bcm_mpls_tunnel_switch_t_init(&entry);

        entry.action = BCM_MPLS_SWITCH_ACTION_POP;

        /*
         * incoming label
         * only the mpls tunnel label needs to be defined here.
         * pwe label will be handed as part of the ingress mpls_port_add api
         */
        entry.label = labels[i];

        rv = bcm_mpls_tunnel_switch_create(unit, &entry);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_mpls_tunnel_switch_create\n");
            return rv;
        }
    }

    return rv;

}

int
vpls_roo_mc_mpls_port_add_termination(
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
    if (is_device_or_above(unit, JERICHO2))
    {
        mpls_port.flags2 |= (vpn ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);
        mpls_port.flags2 |= BCM_MPLS_PORT2_PLATFORM_NAMESPACE;
    }
    mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;

    /*
     * encap_id is the egress outlif - used for learning 
     */
    mpls_port.encap_id = cint_vpls_roo_mc_info.core_overlay_pwe_encap_id[1];
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = cint_vpls_roo_mc_info.core_overlay_pwe_label + 1;
    /** connect PWE entry to created MPLS encapsulation entry for learning */
    mpls_port.port = cint_vpls_roo_mc_info.core_port[1];

    rv = bcm_mpls_port_add(unit, vpn, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add termination\n", rv);
        return rv;
    }

    cint_vpls_roo_mc_info.core_overlay_mpls_port_id_ing = mpls_port.mpls_port_id;

    return rv;
}

/* create ipmc group for tunnel to native flow*/
int
vpls_roo_mc_create_ipmc_group2(
    int unit)
{
    int rv = BCM_E_NONE;
    int i;
    int flags;

    /*
     * mcid create
     */
    flags = BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP;
    rv = bcm_multicast_create(unit, flags, &cint_vpls_roo_mc_info.ipmc_group2);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_multicast_create\n");
        return rv;
    }

    /*
     * Add ports to mc group
     */
    bcm_multicast_replication_t rep_array;
    bcm_multicast_replication_t_init(&rep_array);

    /*
     * access copies
     */
    for (i = 0; i < NUM_OF_ACCESS_PORTS; i++)
    {
        rep_array.port = cint_vpls_roo_mc_info.access_port[i];
        rep_array.encap1 = ((cint_vpls_roo_mc_info.access_eth_rif + i) & 0x3fffff);

        rv = bcm_multicast_add(unit, cint_vpls_roo_mc_info.ipmc_group2, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);

        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_multicast_add \n");
            return rv;
        }
    }

    /*
     * network routed copy split horizon will apply for (i=0; i<NUM_OF_CORE_PORTS-1; i++){ rep_array.port =
     * cint_vpls_roo_mc_info.core_port[i]; rep_array.encap1 = ((cint_vpls_roo_mc_info.core_native_eth_rif + i) &
     * 0x3fffff);
     * 
     * rv = bcm_multicast_add(unit,cint_vpls_roo_mc_info.ipmc_group2 , BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
     * 
     * if (rv != BCM_E_NONE) { printf("Error, in bcm_multicast_add \n"); return rv; } }
     */

    /*
     * Fall back to bridge rep_array.port = cint_vpls_roo_mc_info.access_port[0]; rep_array.encap1 =
     * ((cint_vpls_roo_mc_info.core_native_eth_rif + 3) & 0x3fffff);
     * 
     * rv = bcm_multicast_add(unit,cint_vpls_roo_mc_info.ipmc_group2 , BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
     * 
     * if (rv != BCM_E_NONE) { printf("Error, in bcm_multicast_add \n"); return rv; } 
     */

    return rv;
}

int
vpls_roo_mc_print_mac(
    bcm_mac_t mac)
{
    int i;
    printf(" mac=");
    for (i = 0; i < 6; i++)
    {
        if (i != 5)
        {
            printf("%x:", mac[i]);
        }
        else
        {
            printf("%x\n", mac[i]);
        }
    }
}

/* Main function having required calls*/
int
vpls_roo_mc_main(
    int unit,
    int access_port_1,
    int access_port_2,
    int core_port_1,
    int core_port_2)
{
    int rv = BCM_E_NONE;
    int i;

    /*
     * Ports assignment
     */
    vpls_roo_mc_init(access_port_1, access_port_2, core_port_1, core_port_2);

    /*
     * Set port properties like create vlan domain and create in and out lifs
     */
    rv = vpls_roo_mc_configure_port_properties(unit, &cint_vpls_roo_mc_info.access_port,
                                               &cint_vpls_roo_mc_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error %d, in vpls_roo_mc_configure_port_properties\n", rv);
        return rv;
    }

    /*
     * create vsi
     */
    rv = vpls_roo_mc_create_eth_rifs(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_eth_rifs\n", rv);
        return rv;
    }

    /*
     * Configure L3 interfaces 
     */
    rv = vpls_roo_mc_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * arpid to mac
     */
    rv = vpls_roo_mc_create_arp_entries(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_arp_entries\n", rv);
        return rv;

    }

    for (i = 0; i < NUM_OF_CORE_PORTS; i++)
    {
        /*
         * Configure a push mpls tunnel entry.
         */
        rv = vpls_roo_mc_create_mpls_tunnel(unit,
                                            cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_1 + i,
                                            cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_2 + i,
                                            1, cint_vpls_roo_mc_info.core_overlay_arp_id[i],
                                            &cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_id[i]);
        printf("create_mpls_tunnel : label_1 =%x, label_2=%x, overlay_arp_id=%d, mpls_tunnel_id=%d\n",
               cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_1 + i,
               cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_2 + i, cint_vpls_roo_mc_info.core_overlay_arp_id[i],
               cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_id[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vpls_roo_mc_create_mpls_tunnel\n", rv);
            return rv;
        }
    }

    /*
     * core fall back to switch copy
     */
    rv = vpls_roo_mc_create_mpls_tunnel(unit,
                                        cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_1 + 2,
                                        cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_2 + 2,
                                        1, cint_vpls_roo_mc_info.core_overlay_arp_id[2],
                                        &cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_id[2]);
    printf("create_mpls_tunnel : label_1 =%x, label_2=%x, overlay_arp_id=%d, mpls_tunnel_id=%d\n",
           cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_1 + 2,
           cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_2 + 2, cint_vpls_roo_mc_info.core_overlay_arp_id[2],
           cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_id[2]);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_mpls_tunnel\n", rv);
        return rv;
    }

    /*
     * configure pwe label
     */

    rv = vpls_roo_mc_mpls_port_add_encapsulation(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    /*
     * insert LEM and KAPS entries for fwd lookup
     */
    rv = vpls_roo_mc_fwd_lookup_entires_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_fwd_lookup_entires_create\n", rv);
        return rv;
    }

    /*
     * create IPMC group for native to tunnel flow
     */
    rv = vpls_roo_mc_create_ipmc_group1(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_ipmc_group1\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow 
     */
    bcm_mpls_label_t label_array[2];
    label_array[0] = cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_1 + 1;
    label_array[1] = cint_vpls_roo_mc_info.core_overlay_mpls_tunnel_label_2 + 1;
    rv = vpls_roo_mc_create_termination_stack(unit, label_array, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_termination_stack\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - ingress flow 
     */
    rv = vpls_roo_mc_mpls_port_add_termination(unit, cint_vpls_roo_mc_info.core_native_eth_rif + 3);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_mpls_port_add_termination\n", rv);
        return rv;
    }

    /*
     * create IPMC group for native to tunnel flow
     */
    rv = vpls_roo_mc_create_ipmc_group2(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_mc_create_ipmc_group2\n", rv);
        return rv;
    }

    return rv;
}
