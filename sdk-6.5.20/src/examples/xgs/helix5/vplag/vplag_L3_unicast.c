/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : VPLAG L3 Unicast
 *
 * Usage    : BCM.0> cint vplag_L3_unicast.c
 *
 * config   : vplag_L3_unicast_config.bcm
 *
 * Log file : vplag_L3_unicast_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *   vp_lag_port1   |                              |
 * +----------------+                              |
 *                  |                              |
 *                  |                              |  vp_lag_port3
 *                  |          SWITCH              +-----------------+
 *                  |                              |
 *                  |                              |
 *   vp_lag_port2   |                              |
 * +----------------+                              |
 *                  |                              |
 *                  +------------------------------+
 *
 * Summary:
 * ========
 *   This Cint example to show configuration of the VPLAG L3 Unicast scenario
 *   entry using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. Out of these
 *        three ports, two ports are used as ingress_ports and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_vplag_l3_unicast()):
 *   =============================================================
 *     a) Configure a basic VPLAG L3 Unicast functional scenario. This adds the
 *        host in l3 table[host] and does the necessary configurations of vlan,
 *        interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show', 'trunk show'
 *        'l3 egress show' and 'l3 l3table show'
 *
 *     b) Transmit the IPv4 packet with different DIP and ETAG. The contents of
 *        the packet are printed on screen. The packet used matches the route
 *        configured through script.
 *
 *     c) Expected Result:
 *     ===================
 *      a) Sending packet with ETAG:0xc, VID:3, DIP:2.2.2.2 & SIP:1.1.1.1 to port3
 *         - Expected is: Port1 receive etag 0xb, VID 2 packet and observe 'l2 show'
 *         for l2 address learning and 'l3 l3table show' for DIP 2.2.2.3 hit.
 *      b) Modify DIP to 2.2.2.3 and re-sending packet with ETAG:0xc, VID:3,
 *         DIP:2.2.2.3 & SIP:1.1.1.1 to port3.
 *         - Expected is: Port1 receive etag 0xa, VID 2 packet and observe
 *         'l3 l3table show' DIP 2.2.2.3 hit
 *      c) Sending packet with ETAG:0xa, VID:2, SIP:2.2.2.2 & DIP:1.1.1.1 to port1.
 *         - Expected is: A new mac address is learned on VPLAGi trunk port and
 *         observe 'l2 show'
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t vp_lag_port1;
bcm_port_t vp_lag_port2;
bcm_port_t vp_lag_port3;

int unit = 0;


/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
            port_list[port] = i;
            port++;
        }
    }

    if ((0 == port) || (port != num_ports)) {
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
    bcm_field_qset_t qset;
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
    bcm_field_qset_t qset;
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

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress port/s. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
{
    int port_list[3], i;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    vp_lag_port1 = port_list[0];
    vp_lag_port2 = port_list[1];
    vp_lag_port3 = port_list[2];

    for (i = 0; i <= 1; i++) {
        if (BCM_E_NONE != ingress_port_setup(unit, port_list[i])) {
            printf("ingress_port_setup() failed for port %d\n", port_list[i]);
            return -1;
        }
    }

    if (BCM_E_NONE != egress_port_setup(unit, vp_lag_port3)) {
        printf("egress_port_setup() failed for port %d\n", vp_lag_port3);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

/*
 * Verification:
 *
 Case 1:
 ******

 * Send the below ETAG:0xc, VID:3 pkt to port 3 ( DST Ip : 2.2.2.2 - SRC Ip : 1.1.1.1 )
 00000000002200000000222288880000
 000C00008100000308004500002E0000
 0000401174BA0202020201010101003F
 003F001AB0E50001000102032FB3ACF2

 * Expected result
 * Port1 receive etag 0xb, VID 2 packet
 0000 0000 1111 0000 0000 0011 8888 0000
 000b 0000 8100 0002 0800 4500 002e 0000
 0000 3f11 75ba 0202 0202 0101 0101 003f
 003f 001a b0e5 0001 0001 0203 2fb3 acf2
 f9b8 b246
 * BCM.0> l2 show
 mac=00:00:00:00:22:22 vlan=3 GPORT=0x8c000003 port=0x8c000003(extender) Hit
 * BCM.0> l3 l3table show
 Unit 0, free L3 table entries: 8188
 Entry VRF IP address       Mac Address           INTF MOD PORT    CLASS HIT
 1     0    1.1.1.2          00:00:00:00:00:00  200256    0    0         0 n      (ECMP)
 2     0    2.2.2.2          00:00:00:00:00:00  400007    0    0         0 y
 3     0    2.2.2.3          00:00:00:00:00:00  400007    0    0         0 n
 4     0    1.1.1.1          00:00:00:00:00:00  200256    0    0         0 y      (ECMP)

 Case 2:
 ******

 *Modify the DST IP and send the below ETAG:0xc, VID:3 pkt to port 3 ( DST Ip : 2.2.2.3 - SRC Ip : 1.1.1.1 )
 00000000002200000000222288880000
 000C00008100000308004500002E0000
 0000401174B90202020301010101003F
 003F001AB0E50001000102032FB3ACF2

 * Expected result
 * Port1 receive etag 0xa, VID 2 packet
 0000 0000 1111 0000 0000 0011 8888 0000
 000b 0000 8100 0002 0800 4500 002e 0000
 0000 3f11 75ba 0202 0202 0101 0101 003f
 003f 001a b0e5 0001 0001 0203 2fb3 acf2
 8b74 2c07
 * BCM.0> l3 l3table show
 Unit 0, free L3 table entries: 8188
 Entry VRF IP address       Mac Address           INTF MOD PORT    CLASS HIT
 1     0    1.1.1.2          00:00:00:00:00:00  200256    0    0         0 n      (ECMP)
 2     0    2.2.2.2          00:00:00:00:00:00  400007    0    0         0 y
 3     0    2.2.2.3          00:00:00:00:00:00  400007    0    0         0 y
 4     0    1.1.1.1          00:00:00:00:00:00  200256    0    0         0 y      (ECMP)

 Case 3:
 ******
 * Send the below ETAG:0xa, VID:2 pkt to port 1 ( DST Ip : 1.1.1.1 - SRC Ip : 2.2.2.2 )
 00000000222200000000112288880000
 000A00008100000208004500002E0000
 0000401174BA0101010102020202003F
 003F001AB0E50001000102032FB3ACF2

 * Expected result
 A new mac address is learned on VPLAG
 mac=00:00:00:00:11:22 vlan=2 GPORT=0x0 Trunk=1089 Hit
 */

void
verify(int unit)
{
    char str[512];

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
    bshell(unit, "l3 l3table show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "trunk show");
    bshell(unit, "clear c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, vp_lag_port1, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, vp_lag_port2, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, vp_lag_port3, BCM_PORT_DISCARD_NONE));

    printf("\na) Sending packet with ETAG:0xc, VID:3, DIP:2.2.2.2 & SIP:1.1.1.1 to port:%d\n", vp_lag_port3);
    printf("\nExpected Result: Port1 receive etag 0xb, VID 2 packet and observe 'l2 show' for l2 address learning and 'l3 l3table show' for DIP 2.2.2.3 hit\n");
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000002200000000222288880000000C00008100000308004500002E00000000401174BA0202020201010101003F003F001AB0E50001000102032FB3ACF2; sleep quiet 1", vp_lag_port3);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "l2 show");
    bshell(unit, "l3 l3table show");
    bshell(unit, "sleep 4");
    bshell(unit, "clear c");

    printf("\nb) Modify DIP to 2.2.2.3 and re-sending packet with ETAG:0xc, VID:3, DIP:2.2.2.3 & SIP:1.1.1.1 to port:%d\n", vp_lag_port3);
    printf("\nExpected Result: Port1 receive etag 0xa, VID 2 packet and observe 'l3 l3table show' DIP 2.2.2.3 hit\n");
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000002200000000222288880000000C00008100000308004500002E00000000401174B90202020301010101003F003F001AB0E50001000102032FB3ACF2; sleep quiet 1", vp_lag_port3);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "l2 show");
    bshell(unit, "l3 l3table show");
    bshell(unit, "sleep 4");
    bshell(unit, "clear c");

    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, vp_lag_port1, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, vp_lag_port2, BCM_PORT_DISCARD_ALL));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, vp_lag_port3, BCM_PORT_DISCARD_NONE));

    printf("\nc) Sending packet with ETAG:0xa, VID:2, SIP:2.2.2.2 & DIP:1.1.1.1 to port:%d\n", vp_lag_port1);
    printf("\nExpected Result: A new mac address is learned on VPLAGi trunk port, observe 'l2 show'\n");
    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000222200000000112288880000000A00008100000208004500002E00000000401174BA0101010102020202003F003F001AB0E50001000102032FB3ACF2; sleep quiet 1", vp_lag_port1);
    bshell(unit, str);
    bshell(unit, "show c");
    bshell(unit, "l2 show");
    bshell(unit, "l3 l3table show");
    bshell(unit, "sleep 4");
    bshell(unit, "clear c");

    return;
}



/* Create NIV Virtual Port */
/* Create NIV UC VP based on (port, vif) */
int
vis_uc_vp_create(int unit, bcm_gport_t gport, uint16 vif, bcm_gport_t *ext_port_id)
{
    bcm_extender_port_t ext_port_info;
    int rv;

    bcm_extender_port_t_init(&ext_port_info);
    ext_port_info.port = gport;
    ext_port_info.extended_port_vid = vif;
    rv = bcm_extender_port_add(unit, &ext_port_info);
    *ext_port_id = ext_port_info.extender_port_id;
    return rv;
}

/* Create NIV MC VP based on (port, VIF vector) */
int
vis_mc_vp_create(int unit, bcm_gport_t gport, uint16 vector, bcm_gport_t *ext_id)
{
    bcm_extender_port_t ext_mc_port_info;
    int rv;

    bcm_extender_port_t_init(&ext_mc_port_info);
    ext_mc_port_info.flags = BCM_NIV_FORWARD_MULTICAST;
    ext_mc_port_info.port = gport;
    ext_mc_port_info.extended_port_vid = vector;
    rv = bcm_extender_port_add(unit, &ext_mc_port_info);
    *ext_id = ext_mc_port_info.extender_port_id;
    return rv;
}

int
lagcreate(int tgid,int withid)
{
    int rc=0;
    int flags=0;


    if(withid)
    {
        flags|= BCM_TRUNK_FLAG_WITH_ID;
    }

    if((rc=bcm_trunk_create(0,flags,&tgid))<0)
    {
        printf("Error:%s\r\n",bcm_errmsg(rc));
    }

    printf("trunk id=%u\r\n",tgid);

    return rc;
}

int
vplagcreate(int tgid,int withid)
{
    int rc=0;
    int flags=0;
    flags=BCM_TRUNK_FLAG_VP;
    if(withid)
    {
        flags|= BCM_TRUNK_FLAG_WITH_ID;
    }
    if((rc=bcm_trunk_create(0,flags,&tgid))<0)
    {
        printf("Error:%s\r\n",bcm_errmsg(rc));
    }
    printf("trunk id=%u\r\n",tgid);
    return rc;
}

int
vplag_l3_intf_create(bcm_if_t l3a_intf_id, bcm_vlan_t l3a_vid, bcm_mac_t mac_addr)
{
    bcm_l3_intf_t               l3_intf;
    int                         retval;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = (BCM_L3_ADD_TO_ARL | BCM_L3_WITH_ID);
    l3_intf.l3a_intf_id = l3a_intf_id;
    sal_memcpy(l3_intf.l3a_mac_addr, mac_addr, sizeof(l3_intf.l3a_mac_addr));
    l3_intf.l3a_vid = l3a_vid;
    retval=bcm_l3_intf_create(unit, &l3_intf);

    return retval;
}

int
vplag_l3_egress_create(uint32 flags, bcm_if_t intf, bcm_mac_t mac_addr, bcm_vlan_t vlan, bcm_port_t port, bcm_if_t *egr_obj)
{
    bcm_l3_egress_t             l3_egress;
    int                         retval;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags = flags;
    l3_egress.intf = intf;
    sal_memcpy(l3_egress.mac_addr,  mac_addr, sizeof(l3_egress.mac_addr));
    l3_egress.vlan = vlan;
    l3_egress.port = port;
    retval=bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj);

    return retval;
}

int
vplag_l3_host_add(uint32 l3a_flags, bcm_ip_t l3a_ip_addr, bcm_if_t l3a_intf, bcm_vrf_t l3a_vrf)
{
    bcm_l3_host_t              host_info;
    int                        retval;

    bcm_l3_host_t_init(&host_info);
    host_info.l3a_flags |= l3a_flags;
    host_info.l3a_ip_addr = l3a_ip_addr;
    host_info.l3a_intf = l3a_intf;
    host_info.l3a_vrf = l3a_vrf;
    retval=bcm_l3_host_add(unit, &host_info);

    return retval;
}

bcm_error_t
config_vplag_l3_unicast(int unit)
{
    bcm_trunk_member_t members[2];
    bcm_trunk_member_t members1[2];
    bcm_trunk_info_t   t_add_info;
    bcm_trunk_info_t   t_add_info1;
    bcm_multicast_t    vlan_mc_group = 0;
    bcm_l3_egress_t    l3_egress;
    bcm_l3_intf_t      l3_intf;
    bcm_gport_t        vp_lag_gport;
    bcm_gport_t        gport_1, gport_2, gport_3;
    bcm_gport_t        ext_port1, ext_port2, ext_port3;
    bcm_gport_t        gport_trunk1;
    bcm_port_t         port_1 = vp_lag_port1;
    bcm_port_t         port_2 = vp_lag_port2;
    bcm_port_t         port_3 = vp_lag_port3;
    bcm_vlan_t         vlan_id_2 = 2;
    bcm_vlan_t         vlan_id_3 = 3;
    bcm_pbmp_t         pbmp, ubmp;
    bcm_vrf_t          vrf = 0;
    bcm_if_t           mpath_egr_obj[2];
    /* Router-1 params */
    bcm_vlan_t         R1_vid = 2;
    bcm_mac_t          R1_mac = {0x00,0x0,0x0,0x0,0x0,0x11};
    bcm_mac_t          R1_nh_mac = {0x00,0x00,0x0,0x0,0x11,0x11};
    bcm_if_t           R1_intf = 11;
    bcm_ip_t           R1_nh_ip_1 = 0x01010101;
    bcm_ip_t           R1_nh_ip_2 = 0x01010102;
    bcm_if_t           R1_egr_obj_1;
    bcm_if_t           R1_egr_obj_2;
    bcm_if_t           R1_mpath_egr_obj;
    /* Router-2 params */
    bcm_vlan_t         R2_vid = 3;
    bcm_mac_t          R2_mac = {0x0,0x0,0x0,0x0,0x0,0x22};
    bcm_mac_t          R2_nh_mac = {0x0,0x0,0x0,0x0,0x22,0x22};
    bcm_if_t           R2_intf = 22;
    bcm_ip_t           R2_nh_ip_1 = 0x02020202;
    bcm_ip_t           R2_nh_ip_2 = 0x02020203;
    bcm_if_t           R2_egr_obj_1;
    int                retval=0;
    int                tgid1;
    int                tgid2;
    bcm_trunk_chip_info_t trunk_info;


    BCM_GPORT_LOCAL_SET(gport_1, port_1);
    BCM_GPORT_LOCAL_SET(gport_2, port_2);
    BCM_GPORT_LOCAL_SET(gport_3, port_3);

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchEtagEthertype, 0x8888));
    /* Enabling Egress mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set (unit, bcmSwitchL3EgressMode, 1));

    /* Configure port properties */
    /* On Port1, Port2 and Port3, drop if packet doesn't have a VNTAG */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_1, bcmPortControlNonEtagDrop, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_2, bcmPortControlNonEtagDrop, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_3, bcmPortControlNonEtagDrop, 1));

    bcm_trunk_chip_info_t_init(trunk_info);
    BCM_IF_ERROR_RETURN(bcm_trunk_chip_info_get(unit, &trunk_info));

    print trunk_info.trunk_id_min;
    print trunk_info.trunk_id_max;

    print trunk_info.vp_id_min;
    print trunk_info.vp_id_max;

    tgid1 = trunk_info.trunk_id_max;
    tgid2 = trunk_info.vp_id_max;

    lagcreate(tgid1,1);
    print bcm_trunk_info_t_init(&t_add_info);
    t_add_info.dlf_index  = BCM_TRUNK_UNSPEC_INDEX;
    t_add_info.mc_index   = BCM_TRUNK_UNSPEC_INDEX;
    t_add_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;
    t_add_info.psc = BCM_TRUNK_PSC_SRCMAC;
    print bcm_trunk_member_t_init(&members[0]);
    print bcm_trunk_member_t_init(&members[1]);
    members[0].gport = gport_1;
    members[1].gport = gport_2;
    BCM_IF_ERROR_RETURN(bcm_trunk_set(0, tgid1, &t_add_info, 1, members));
    BCM_GPORT_TRUNK_SET(gport_trunk1, tgid1);

    /* Create NIV UC VP for vif 0xa on trunk */
    print vis_uc_vp_create(unit, gport_trunk1, 0xa, &ext_port1);

    /* Create NIV UC VP for vif 0xb on trunk */
    print vis_uc_vp_create(unit, gport_trunk1, 0xb, &ext_port2);

    /* Create NIV UC VP for vif 0xc on trunk */
    print vis_uc_vp_create(unit, gport_3, 0xc, &ext_port3);

    /* VLAN create */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan_id_2));
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vlan_id_3));

    /* DLF/Broadcast/Unknown MC configuration */
    /* Create and configure VLAN's broadcast, unknown multicast, and unknown unicast groups */
    BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_EXTENDER, &vlan_mc_group));

    bcm_vlan_control_vlan_t vlan_control;
    vlan_control.broadcast_group = vlan_mc_group;
    vlan_control.unknown_multicast_group = vlan_mc_group;
    vlan_control.unknown_unicast_group = vlan_mc_group;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vlan_id_2, vlan_control));
    BCM_IF_ERROR_RETURN(bcm_multicast_control_set(unit,vlan_mc_group,bcmMulticastVpTrunkResolve,1));

    /* Add ports to VLAN VPs */
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vlan_id_2, gport_1, 0));
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vlan_id_2, gport_2, 0));

    /* DLF/Broadcast/Unknown MC configuration */
    /* Create and configure VLAN's broadcast, unknown multicast, and unknown unicast groups */
    BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, BCM_MULTICAST_TYPE_EXTENDER, &vlan_mc_group));

    vlan_control.broadcast_group = vlan_mc_group;
    vlan_control.unknown_multicast_group = vlan_mc_group;
    vlan_control.unknown_unicast_group = vlan_mc_group;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vlan_id_3, vlan_control));

    /* Add ports to VLAN */
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vlan_id_3, ext_port3, 0));

    vplagcreate(tgid2,1);
    print bcm_trunk_info_t_init(&t_add_info1);
    t_add_info1.dlf_index  = BCM_TRUNK_UNSPEC_INDEX;
    t_add_info1.mc_index   = BCM_TRUNK_UNSPEC_INDEX;
    t_add_info1.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;
    t_add_info1.psc = BCM_TRUNK_PSC_PORTFLOW;
    print bcm_trunk_member_t_init(&members1[0]);
    print bcm_trunk_member_t_init(&members1[1]);
    members1[0].gport = ext_port1;
    members1[1].gport = ext_port2;
    BCM_IF_ERROR_RETURN(bcm_trunk_set(0, tgid2, &t_add_info1, 2, members1));

    /* IPV4 settings */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(0, ext_port3, bcmPortControlIP4, 1));

    BCM_GPORT_TRUNK_SET(vp_lag_gport, tgid2);
    BCM_IF_ERROR_RETURN(bcm_port_control_set(0, vp_lag_gport, bcmPortControlIP4, 1));

    /* Create L3 interfaces */
    /* Router-1:: Create L3 interface */
    retval = vplag_l3_intf_create (R1_intf, R1_vid, R1_mac);
    printf("\r\n bcm_l3_intf_create = %s, R1_intf=%d \n", bcm_errmsg(retval), R1_intf);
    /* Router-2:: Create L3 interface */
    retval = vplag_l3_intf_create (R2_intf, R2_vid, R2_mac);
    printf("\r\n bcm_l3_intf_create = %s, R1_intf=%d \n", bcm_errmsg(retval), R2_intf);

    /* Create L3 egress object */
    /* Router-1:: Create L3 egress object 1*/
    retval = vplag_l3_egress_create (0, R1_intf, R1_nh_mac, R1_vid, ext_port1, &R1_egr_obj_1);
    printf("\r\n bcm_l3_egress_create = %s, egr_obj_acc1=%d \n", bcm_errmsg(retval), R1_egr_obj_1);
    /* Router-1:: Create L3 egress object 2*/
    retval = vplag_l3_egress_create (0, R1_intf, R1_nh_mac, R1_vid, ext_port2, &R1_egr_obj_2);
    printf("\r\n bcm_l3_egress_create = %s, egr_obj_acc1=%d \n", bcm_errmsg(retval), R1_egr_obj_2);
    /* Router-2:: Create L3 egress object */
    retval = vplag_l3_egress_create (0, R2_intf, R2_nh_mac, R2_vid, ext_port3, &R2_egr_obj_1);
    printf("\r\n bcm_l3_egress_create = %s, egr_obj_acc1=%d \n", bcm_errmsg(retval), R2_egr_obj_1);

    /* Multipath Create */
    mpath_egr_obj[0]= R1_egr_obj_1;
    mpath_egr_obj[1]= R1_egr_obj_2;
    retval = bcm_l3_egress_multipath_create(unit, 0x0, 2,mpath_egr_obj, &R1_mpath_egr_obj);
    printf("\r\n bcm_l3_egress_multipath_create = %s \n", bcm_errmsg(retval));

    /* Add route */
    retval = vplag_l3_host_add (BCM_L3_MULTIPATH, R1_nh_ip_1, R1_mpath_egr_obj, vrf);
    printf("\r\n vplag_l3_host_add = %s, R1_nh_ip_1=%d \n", bcm_errmsg(retval), R1_nh_ip_1);

    retval = vplag_l3_host_add (BCM_L3_MULTIPATH, R1_nh_ip_2, R1_mpath_egr_obj, vrf);
    printf("\r\n vplag_l3_host_add = %s, R1_nh_ip_2=%d \n", bcm_errmsg(retval), R1_nh_ip_2);

    retval = vplag_l3_host_add (0, R2_nh_ip_1, R2_egr_obj_1, vrf);
    printf("\r\n vplag_l3_host_add = %s, R2_nh_ip_1=%d \n", bcm_errmsg(retval), R2_nh_ip_1);

    retval = vplag_l3_host_add (0, R2_nh_ip_2, R2_egr_obj_1, vrf);
    printf("\r\n vplag_l3_host_add = %s, R2_nh_ip_2=%d \n", bcm_errmsg(retval), R2_nh_ip_2);

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_vplag_l3_unicast())
 *  c) demonstrates the functionality(done in verify()).
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

    print "~~~ #2) config_vplag_l3_unicast(): ** start **";
    if (BCM_FAILURE((rv = config_vplag_l3_unicast(unit)))) {
        printf("config_vplag_l3_unicast() failed.\n");
        return -1;
    }
    print "~~~ #2) config_vplag_l3_unicast(): ** end **";

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
