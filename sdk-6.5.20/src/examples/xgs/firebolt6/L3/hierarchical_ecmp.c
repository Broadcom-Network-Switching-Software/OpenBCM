/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPv4 Unicast H-ECMP
 *
 * Usage    : BCM.0> cint hierarchical_ecmp.c
 *
 * config   : hierarchical_ecmp_config.bcm
 *
 * Log file : hierarchical_ecmp_log.txt
 *
 * Test Topology :
 *
 *                    +-------+      Level 1     Level 2 ECMP
 *                    |       |    +--ECMP1---+-- egress_port1, VLAN 13
 *     ingress_port   |       |    |          +-- egress_port2, VLAN 14
 *          +---------|  SW   |----+
 *    VLAN 12         |       |    |          +-- egress_port3, VLAN 23
 *                    |       |    +--ECMP2---+-- egress_port4, VLAN 24
 *                    +-------+     IP Tunnel +-- egress_port5, VLAN 25
 *
 * Summary:
 * ========
 *   This Cint example to show configuration of the IPv4 unicast H-ECMP scenario
 *   using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Select one ingress and three egress ports and configure them in
 *        Loopback mode.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_hirachical_ecmp()):
 *   ====================================================
 *     a) Configure a basic IPv4 unicast with hirarchical ECMP functional
 *        scenario. This adds the entry in l3 table[defip/LPM] and does
 *        the necessary configurations of vlan, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show', 'l3 defip show', 'l3 multipath show', 'l2 show'
 *        and 'l3 ecmp egress show'
 *
 *     b) Transmit the known IPv4 multicast packet. The contents of the packet
 *        are printed on screen. The packet used matches the route configured
 *        through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port. Also run the 'l3 defip show' to
 *       check the HIT bit status (y i.e Yes) and 'show c' to check the Tx/Rx
 *       packet stats/counters. Each time, the outgoing packet use a different
 *       path or egress-port or next-hop via ECMP.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port1, egress_port2;
bcm_port_t egress_port3, egress_port4, egress_port5;

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i=0,port=0,rv=0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit,&configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
            port_list[port]=i;
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
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_qset_t  qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t
egress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_qset_t  qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 * Select one ingress and three egress ports and configure them in
 * Loopback mode. Install a rule to copy incoming packets to CPU and
 * additional action to drop the packets when it loops back on egress
 * ports. Start packet watcher. Ingress port setup is done in
 * ingress_port_setup(). egress port setup is done in egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
  int port_list[4], i;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  ingress_port = port_list[0];
  egress_port1 = port_list[1];
  egress_port2 = port_list[2];
  egress_port3 = port_list[3];

  if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", ingress_port);
    return -1;
  }

  for (i = 1; i <= 3; i++) {
    if (BCM_E_NONE != egress_port_setup(unit, port_list[i])) {
      printf("egress_port_setup() failed for port %d\n", port_list[i]);
      return -1;
    }
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}

/*
 *  Verification:
 *
 *  In Packet:
 *  ----------
 *   00000000AAAA0000000022228100000C
 *   08004500003A0000000040FF9B110A0A
 *   0A01C0A80A0186DD6000000000362F0A
 *   CC000000000000000000000000000001
 *   DD0000000000000000000000292DD3E9
 *
 *  Out Packet:
 *  -----------
 *   Each time, the outgoing packet use a different path or
 *   next-hop or ECMP.
 */
void
verify(int unit)
{
    char str[512];

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "l3 defip show");
    bshell(unit, "l3 multipath show");
    bshell(unit, "l3 ecmp egress show");
    bshell(unit, "clear c");
    printf("\nSending 16 IPv4 unicast packets to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 16 pbm=%d data=0x00000000AAAA0000000022228100000C08004500003A0000000040FF9B110A0A0A01C0A80A0186DD6000000000362F0ACC000000000000000000000000000001DD0000000000000000000000292DD3E9; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 defip show");
    bshell(unit, "show c");

    return;
}

/* Create vlan and add port to vlan */
int
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Create vlan and add array of ports to vlan */
int
vlan_create_add_ports(int unit, int vid, int count, bcm_port_t *ports)
{
    bcm_pbmp_t pbmp, upbmp;
    int i;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);

    for (i = 0; i < count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, ports[i]);
    }

    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Create L3 interface */
int
create_l3_interface(int unit, int flags, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    *intf_id = l3_intf.l3a_intf_id;

    return rv;
}

/* Create L3 egress object */
int
create_egr_obj(int unit, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
                   bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    /* l3_egress.vlan = l3_if; vid; */
    l3_egress.intf  = l3_if;
    l3_egress.port = gport;
    return bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
}

/* Adding router mac */
int
config_router_mac(int unit, bcm_mac_t router_mac, bcm_vlan_t ing_vlan)
{
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, router_mac, ing_vlan);
    l2_addr.flags |= (BCM_L2_L3LOOKUP | BCM_L2_STATIC);
    return bcm_l2_addr_add(unit, &l2_addr);
}


int
tunnel_initiator_setup(int unit, bcm_ip_t sip, bcm_ip_t dip, bcm_if_t l3if)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    bcm_tunnel_initiator_t_init(&tunnel_init);
    l3_intf.l3a_intf_id = l3if;
    tunnel_init.ttl = 10; /* optional */
    tunnel_init.type = bcmTunnelTypeIp4In4;
    tunnel_init.dip = dip;
    tunnel_init.sip = sip;
    rv = bcm_tunnel_initiator_set(unit, &l3_intf, &tunnel_init);

    return rv;
}

int
set_up_packet_spray(int unit, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchECMPLevel1RandomSeed, 0xff));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchECMPLevel2RandomSeed, 0xf011));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port,
                bcmPortControlECMPLevel1LoadBalancingRandomizer, 8));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port,
                bcmPortControlECMPLevel2LoadBalancingRandomizer, 5));
    return rv;
}

int
rtag7_config(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 flags;

    /* HASH_CONTROL */
    //BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashControl, &flags));
    flags |= BCM_HASH_CONTROL_ECMP_ENHANCE |
             BCM_HASH_CONTROL_MULTIPATH_DIP;
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashControl, flags));

    /* Use port based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_ECMP) */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp, 0));

    flags = BCM_HASH_FIELD_SRCMOD |
            BCM_HASH_FIELD_SRCPORT |
            BCM_HASH_FIELD_PROTOCOL |
            BCM_HASH_FIELD_DSTL4 |
            BCM_HASH_FIELD_SRCL4 |
            BCM_HASH_FIELD_IP4DST_LO |
            BCM_HASH_FIELD_IP4DST_HI |
            BCM_HASH_FIELD_IP4SRC_LO |
            BCM_HASH_FIELD_IP4SRC_HI;

    /* Block A - L3 packet field selection */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, flags));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, flags));
    /* Block B - L3 packet field selection */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, flags));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, flags));

    /* Configure HASH A and HASH B functions */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0Config,
                               BCM_HASH_FIELD_CONFIG_CRC32LO));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1Config,
                               BCM_HASH_FIELD_CONFIG_CRC32HI));

    /* Enable preprocess */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, TRUE));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, TRUE));

    /* Configure Seed */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x55555555));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555));
    return rv;
}

bcm_error_t
config_hirachical_ecmp(int unit)
{
    int i;
    bcm_error_t rv;
    bcm_vlan_t vid_in = 12;
    bcm_port_t port_in = ingress_port;
    bcm_gport_t gport_in;
    bcm_mac_t nxt_hop_in = {0x00,0x00,0x00,0x00,0x22,0x22};
    bcm_mac_t ing_router_mac = {0x00,0x00,0x00,0x00,0xAA,0xAA};
    bcm_pbmp_t pbmp, upbmp;
    bcm_l2_addr_t l2addr;

    bcm_l3_route_t route_info;
    bcm_ip_t dip = 0xC0A80A01;  /* 192.168.10.1 */
    bcm_ip_t mask = 0xffffff00;  /* 255.255.255.0 */

    /* There are 2 ECMP nexthop in tunnel 1 */
    bcm_vlan_t egr_vid_1[2] = {13, 14};
    bcm_port_t egr_port_1[2];
    egr_port_1[0] = egress_port1;
    egr_port_1[1] = egress_port2;
    bcm_gport_t egr_gport_1[2];
    bcm_if_t egr_l3_if_1[2];
    bcm_if_t egr_obj_1[2];

    bcm_mac_t egr_nxt_hop_1  = {0x00,0x00,0x00,0x00,0x11,0x11};
    bcm_mac_t egr_router_mac_1  = {0x00,0x00,0x00,0x00,0xBB,0x1B};

    bcm_ip_t tunnel_1_sip = 0x0a0a0a12;
    bcm_ip_t tunnel_1_dip = 0x0a0a0a11;

    /* There are 3 ECMP nexthop in tunnel 2 */
    bcm_vlan_t egr_vid_2[3] = {23, 24, 25};
    bcm_port_t egr_port_2[3];
    egr_port_2[0] = egress_port3;
    egr_port_2[1] = egress_port4;
    egr_port_2[2] = egress_port5;
    bcm_gport_t egr_gport_2[3];
    bcm_if_t egr_l3_if_2[3];
    bcm_if_t egr_obj_2[3];
    bcm_mac_t egr_nxt_hop_2  = {0x00,0x00,0x00,0x00,0x11,0x21};
    bcm_mac_t egr_router_mac_2  = {0x00,0x00,0x00,0x00,0xBB,0x2B};

    bcm_ip_t tunnel_2_sip = 0x0a0a0a22;
    bcm_ip_t tunnel_2_dip = 0x0a0a0a21;

    rv = rtag7_config(unit);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE));

    /* Get GPORT */
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_in, &gport_in));

    /*
     * Ingress side of the VLAN
     */
    rv = vlan_create_add_port(unit, vid_in, port_in);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Enable ingress L3 lookup */
    bcm_l2_addr_t_init(&l2addr, ing_router_mac, vid_in);
    l2addr.flags |= (BCM_L2_L3LOOKUP | BCM_L2_STATIC);
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

    /* Tunnel 1 */
    for (i=0; i < 2; i++) {
        BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egr_port_1[i], &egr_gport_1[i]));

        /* Egress side of the VLAN */
        rv = vlan_create_add_port(unit, egr_vid_1[i], egr_port_1[i]);
        if(BCM_FAILURE(rv)) {
            printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
            return rv;
        }

        /* Create egress L3 interface and tunnel */
        egr_router_mac_1[5] += i;
        rv = create_l3_interface(unit, BCM_L3_ADD_TO_ARL, egr_router_mac_1, egr_vid_1[i], &egr_l3_if_1[i]);
        if(BCM_FAILURE(rv)) {
            printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
            return rv;
        }
        rv = tunnel_initiator_setup(unit, tunnel_1_sip, tunnel_1_dip, egr_l3_if_1[i]);
        if(BCM_FAILURE(rv)) {
            printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
            return rv;
        }

        /* Egress side of the next hop object */
        egr_nxt_hop_1[5] += i;
        rv = create_egr_obj(unit, egr_l3_if_1[i], egr_nxt_hop_1, egr_gport_1[i], &egr_obj_1[i]);
        if(BCM_FAILURE(rv)) {
            printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
            return rv;
        }
    }

    /* Tunnel 2 */
    for (i=0; i<3; i++) {
        BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egr_port_2[i], &egr_gport_2[i]));

        /* Egress side of the VLAN */
        rv = vlan_create_add_port(unit, egr_vid_2[i], egr_port_2[i]);
        if(BCM_FAILURE(rv)) {
            printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
            return rv;
        }

        /* Create egress L3 interface and tunnel */
        egr_router_mac_2[5] += i;
        rv = create_l3_interface(unit, BCM_L3_ADD_TO_ARL, egr_router_mac_2, egr_vid_2[i], &egr_l3_if_2[i]);
        if(BCM_FAILURE(rv)) {
            printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
            return rv;
        }
        rv = tunnel_initiator_setup(unit, tunnel_2_sip, tunnel_2_dip, egr_l3_if_2[i]);
        if(BCM_FAILURE(rv)) {
            printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
            return rv;
        }

        /* Egress side of the next hop object */
        egr_nxt_hop_2[5] += i;
        rv = create_egr_obj(unit, egr_l3_if_2[i], egr_nxt_hop_2, egr_gport_2[i], &egr_obj_2[i]);
        if(BCM_FAILURE(rv)) {
            printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
            return rv;
        }
    }

    /* ECMP group */
    bcm_l3_egress_ecmp_t ecmp_info;
    bcm_if_t ecmp_group[3];
    bcm_if_t ecmp_group_id, tnl_1_ecmp_group_id, tnl_2_ecmp_group_id;

    /* Set up RND */
    rv = set_up_packet_spray(unit, port_in);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Create level 2 ECMP first */

    /* Level 2 ECMP group for Tunnel 1 */
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    /* ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_ROUND_ROBIN; */
    /* ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT; */
    /* ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM; */
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
    ecmp_info.dynamic_size = 64;
    ecmp_info.max_paths = 64;
    for (i=0; i<2; i++) {
        ecmp_group[i] = egr_obj_1[i];
    }
    BCM_IF_ERROR_RETURN(bcm_l3_egress_ecmp_create(unit, &ecmp_info, 2, ecmp_group));
    tnl_1_ecmp_group_id = ecmp_info.ecmp_intf;

    /* Level 2 ECMP group for Tunnel 2 */
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    /* ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_ROUND_ROBIN; */
    /* ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT; */
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
    ecmp_info.dynamic_size = 64;
    ecmp_info.max_paths = 64;
    for (i=0; i<3; i++) {
        ecmp_group[i] = egr_obj_2[i];
    }
    BCM_IF_ERROR_RETURN(bcm_l3_egress_ecmp_create(unit, &ecmp_info, 3, ecmp_group));
    tnl_2_ecmp_group_id = ecmp_info.ecmp_intf;

    /* Level 1 ECMP group */
    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    ecmp_info.ecmp_group_flags = BCM_L3_ECMP_OVERLAY;
    /* ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT; */
    /* ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_ROUND_ROBIN;*/
    ecmp_info.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
    ecmp_info.dynamic_size = 64;
    ecmp_group[0] = tnl_1_ecmp_group_id;
    ecmp_group[1] = tnl_2_ecmp_group_id;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_ecmp_create(unit, &ecmp_info, 2, ecmp_group));
    ecmp_group_id = ecmp_info.ecmp_intf;

    /*
     * Install the route
     */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_flags = BCM_L3_MULTIPATH;
    route_info.l3a_intf = ecmp_group_id;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route_info));

    return BCM_E_NONE;
}

/*
 * execute:
 *  This functions does the following
 *  a)test setup
 *  b)actual configuration (Done in config_hirachical_ecmp())
 *  c)demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; cancun stat; version";
    bshell(unit, "config show; a ; cancun stat; version");
    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_hirachical_ecmp(): ** start **";
    if (BCM_FAILURE((rv = config_hirachical_ecmp(unit)))) {
        printf("config_hirachical_ecmp() failed.\n");
        return -1;
    }
    print "~~~ #2) config_hirachical_ecmp(): ** end **";

    print "~~~ #3) verify(): ** start **";
    verify(unit);
    print "~~~ #3) verify(): ** end **";
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print "execute(): Start";
  print execute();
  print "execute(): End";
}
