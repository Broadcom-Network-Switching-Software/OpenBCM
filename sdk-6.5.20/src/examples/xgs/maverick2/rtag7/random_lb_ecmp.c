/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Randomized Load-Balancing - ECMP
 *
 *  Usage    : BCM.0> cint random_lb_ecmp.c
 *
 *  config   : rtag7_config.bcm
 *
 *  Log file : random_lb_ecmp_log.txt
 *
 *  Test Topology :
 *
 *                +-----------+
 *                |           |        +--NextHop1/EgressPort1--->
 *                |           |        |
 *  IngressPort-->|SWITCH/L3  |--ECMP--|
 *                |           |        |
 *                |           |        +--NextHop2/EgressPort2--->
 *                +-----------+
 *
 *   Summary:
 *   ========
 *     This cint example configures randomized load balancing on the ECMP group using
 *     BCM APIs
 *
 *   Detailed steps done in the CINT script
 *   ======================================
 *     1) Step1 - Test Setup (Done in test_setup())
 *     ============================================
 *       a) Select one ingress port and two egress ports and configure them in Loopback mode.
 *
 *     2) Step2 - Configuration (Done in random_lb_ecmp())
 *     =================================================== 
 *       a) Create vlans 12, 13 and 14 and ingress_port, egress_port_1 and egress_port_2 as
 *          members, respectively.
 *       b) Create two L3 interfaces - on vlan13(egr_l3_if_1) and vlan14(egr_l3_if_2).
 *       c) Create two egress objects - egr_obj_1(associated with egr_l3_if_1) and egr_obj_2
 *          (associated with egr_l3_if_2). 
 *       d) Create an ECMP group with egress objects egr_obj_1 and egr_obj_2. Configure this
 *          ECMP group to use randomized load balancing for next hop resolution.
 *       e) Create an L3 route to reach 192.168.10.1 via this ECMP group
 *
 *     3) Step3 - Verification (Done in verify())
 *     ==========================================
 *       a) Transmit 50 copies of the below packet on the ingress port and verify that packets
 *          are load balanced across the egress objects/ports using "show counters" 
 *          Packet
 *          ====== 
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 192.168.10.1
 *          SIP 10.10.10.1
 *          00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 3C 00 00 00 00 40 FF 9B 0F 0A 0A
 *          0A 01 C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09
 *          0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19
 *          1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 2E CB
 *          F6 84
 *       b) Expected Result
 *          ===============
 *          The packets should be load balanced across the egress object/ports. This can be
 *          checked using "show counters".
 */

/* Reset c interpreter*/
cint_reset();
bcm_port_t ingress_port;
bcm_port_t egress_port_1;
bcm_port_t egress_port_2;

/* This function populates the port_list_arr with the required
   number of ports
*/
bcm_error_t portNumbersGet(int unit, bcm_port_t *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit,&configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
            port_list[port] = i;
            port++;
        }
    }

    if (( port == 0 ) || ( port != num_ports )) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and sets the discard bit
 * in the PORT table
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));
    return BCM_E_NONE;
}
/*
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    int port_list[4];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port_1 = port_list[1];
    egress_port_2 = port_list[2];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed");
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_1)) {
        printf("egress_port_setup() failed for port:%d", egress_port_1);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_2)) {
        printf("egress_port_setup() failed for port:%d", egress_port_2);
        return -1;
    }

    return BCM_E_NONE;
}

/* Create vlan and add port to vlan */
bcm_error_t vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_vlan_create(unit, vid);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_create() for vlan:%d %s.\n", vid, bcm_errmsg (rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_port_add() for vlan:%d %s.\n", vid, bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

/* Create L3 interface */
bcm_error_t create_l3_interface(int unit, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_l3_intf_create() for vlan:%d %s.\n", vid, bcm_errmsg (rv));
        return rv;
    }
    *intf_id = l3_intf.l3a_intf_id;

    return rv;
}

/* Create L3 egress object */
bcm_error_t create_egr_obj(int unit, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
                   bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;
    bcm_error_t rv = BCM_E_NONE; 

    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.intf  = l3_if;
    l3_egress.port = gport;
    rv = bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_l3_egress_create() for intf:%d %s.\n", l3_if, bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

/* Adding router mac */
bcm_error_t config_router_mac(int unit, bcm_mac_t router_mac, bcm_vlan_t ing_vlan)
{
    bcm_l2_addr_t l2_addr;
    bcm_error_t rv = BCM_E_NONE;

    bcm_l2_addr_t_init(&l2_addr, router_mac, ing_vlan);
    l2_addr.flags |= (BCM_L2_L3LOOKUP | BCM_L2_STATIC);
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_l2_addr_add():%s.\n", bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

bcm_error_t set_up_ecmp_packet_spray(int unit, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_switch_control_set(unit, bcmSwitchECMPLevel1RandomSeed, 0x4321);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set with ECMPLevel1RandomSeed:%s.\n",
                bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchECMPLevel2RandomSeed, 0x4321);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set with ECMPLevel2RandomSeed:%s.\n",
                bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_port_control_set(unit, port,
            bcmPortControlECMPLevel1LoadBalancingRandomizer, 11);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_port_control_set with ECMPLevel1LoadBalancingRandomizer:%s.\n",
                bcm_errmsg (rv));
        return rv;
    }
    rv = bcm_port_control_set(unit, port,
            bcmPortControlECMPLevel2LoadBalancingRandomizer, 8);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_port_control_set with ECMPLevel2LoadBalancingRandomizer:%s.\n",
                bcm_errmsg (rv));
        return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t random_lb_ecmp(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_vlan_t vid_in = 12;
    bcm_port_t port_in = ingress_port;

    bcm_vlan_t egr_vid_1 = 13;
    bcm_port_t egr_port_1 = egress_port_1;
    bcm_gport_t egr_gport_1;
    bcm_if_t egr_l3_if_1;
    bcm_if_t egr_obj_1;

    bcm_vlan_t egr_vid_2 = 14;
    bcm_port_t egr_port_2 = egress_port_2;
    bcm_gport_t egr_gport_2;
    bcm_if_t egr_l3_if_2;
    bcm_if_t egr_obj_2;

    bcm_l3_route_t route_info;

    bcm_mac_t egr_nxt_hop_1 = { 0x00, 0x00, 0x00, 0x00, 0x11, 0x11 };
    bcm_mac_t egr_router_mac_1 = { 0x00, 0x00, 0x00, 0x00, 0xBB, 0x1B };

    bcm_mac_t egr_nxt_hop_2 = { 0x00, 0x00, 0x00, 0x00, 0x11, 0x21 };
    bcm_mac_t egr_router_mac_2 = { 0x00, 0x00, 0x00, 0x00, 0xBB, 0x2B };

    bcm_mac_t router_mac_in = { 0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA };

    bcm_ip_t dip = 0xC0A80A01;	/* 192.168.10.1 */
    bcm_ip_t mask = 0xffffff00;	/* 255.255.255.0 */

    bcm_if_t ecmp_group[2];
    bcm_if_t ecmp_group_id;

    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_switch_control_set %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = set_up_ecmp_packet_spray(unit, port_in);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing set_up_ecmp_packet_spray%s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_port_gport_get(unit, egr_port_1, &egr_gport_1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_port_gport_get for port:%d %s.\n", egr_port_1, bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_port_gport_get(unit, egr_port_2, &egr_gport_2);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_port_gport_get for port:%d %s.\n", egr_port_2, bcm_errmsg (rv));
        return rv;
    }

    /*
     * Ingress side of the VLAN
     */
    rv = vlan_create_add_port (unit, vid_in, port_in);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vlan_create_add_port for vlan:%d %s\n",  vid_in, bcm_errmsg (rv));
        return rv;
    }

    /* Enable ingress L3 lookup */
    rv = config_router_mac (unit, router_mac_in, vid_in);

    if (BCM_FAILURE (rv))
    {
        printf ("Error executing config_router_mac %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /*
     * Egress side of the VLAN
     */
    rv = vlan_create_add_port (unit, egr_vid_1, egr_port_1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vlan_create_add_port for vlan:%d %s\n", egr_vid_1, bcm_errmsg (rv));
        return rv;
    }
    rv = vlan_create_add_port (unit, egr_vid_2, egr_port_2);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vlan_create_add_port for vlan:%d %s\n", egr_vid_2, bcm_errmsg (rv));
        return rv;
    }

    /*
     * Create egress L3 interface
     */
    rv = create_l3_interface (unit, egr_router_mac_1, egr_vid_1, &egr_l3_if_1);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing create_l3_interface for vlan:%d %s\n", egr_vid_1, bcm_errmsg (rv));
        return rv;
    }

    rv = create_l3_interface (unit, egr_router_mac_2, egr_vid_2, &egr_l3_if_2);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vlan_create_add_port for vlan:%d %s\n", egr_vid_2, bcm_errmsg (rv));
        return rv;
    }

    /*
     * Egress side of the next hop object
     */
    rv = create_egr_obj (unit, egr_l3_if_1, egr_nxt_hop_1, egr_gport_1,
            &egr_obj_1);

    if (BCM_FAILURE (rv))
    {
        printf ("Error executing create_egr_obj for intf:%d %s\n", egr_l3_if_1, bcm_errmsg (rv));
        return rv;
    }
    rv = create_egr_obj (unit, egr_l3_if_2, egr_nxt_hop_2, egr_gport_2,
            &egr_obj_2);

    if (BCM_FAILURE (rv))
    {
        printf ("Error executing vlan_create_add_port for intf:%d %s\n", egr_l3_if_2, bcm_errmsg (rv));
        return rv;
    }

    /* ECMP group */
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_l3_egress_ecmp_t_init (&ecmp_info);
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
    ecmp_group[0] = egr_obj_1;
    ecmp_group[1] = egr_obj_2;
    rv = bcm_l3_egress_ecmp_create (unit, &ecmp_info, 2, ecmp_group);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_l3_egress_ecmp_create %s.\n", bcm_errmsg (rv));
        return rv;
    }
    ecmp_group_id = ecmp_info.ecmp_intf;

    /*
     * Install the route for DIP, note this is routed out on the port_out
     */
    bcm_l3_route_t_init (&route_info);
    route_info.l3a_flags = BCM_L3_MULTIPATH;
    route_info.l3a_intf = ecmp_group_id;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    rv = bcm_l3_route_add (unit, &route_info);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_l3_route_add %s.\n", bcm_errmsg (rv));
        return rv;
    }
    
    return BCM_E_NONE;
}

void verify(int unit)
{
    char   str[512];

    bshell(unit, "hm ieee");

    snprintf(str, 512, "tx 50 pbm=%d data=0x00000000AAAA0000000022228100000C08004500003C0000000040FF9B0F0A0A0A01C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20212223242526272ECBF684; sleep 1", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "show counters");
    bshell(unit, str); 
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; a ; version");
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = random_lb_ecmp(unit)))) {
        printf("random_lb_ecmp() failed.\n");
        return -1;
    }

    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

