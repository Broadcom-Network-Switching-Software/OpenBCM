/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : Vxlan Bridging , IPv4 access to network VP LAG (LAG of virtual network ports) unicast Flow
 *
 * Usage    : BCM.0> cint TD4_HSDK_VxLAN_v4_vplag_acc_to_net_uc.c
 *
 * config   : bcm56880_a0-generic-32x400_vxlan_vplag-enabled.config.yml
 *
 * Log file : TD4_HSDK_VxLAN_v4_vplag_acc_to_net_uc_log.txt
 *
 * Test Topology :
 *                                                                                     Underlay+NextHop
 *                                +----------------------+  VLAN=22   egress_port[0]     +---+
 *                                |                      +-------------------------------+   |
 *                                |                      |00::22:22                      +---+
 * SMAC 00::bb:bb                 |                      |                               00::02
 *          +---+          VLAN=21|                      |  VLAN=23   egress_port[1]     +---+
 * Host(acc)|   +-----------------+                      +-------------------------------+   |
 *          +---+    ingress_port |                      |00::22:23                      +---+
 *                                |                      |                               00::03
 *                                |                      |
 *                                |                      |  VLAN=24   egress_port[2]     +---+
 *                                |                      +-------------------------------+   |
 *                                |      Trident-4       |00::22:24                      +---+
 *                                |                      |                               00::04
 *                                |                      |
 *                                |                      |  VLAN=25   egress_port[3]     +---+
 *                                |                      +-------------------------------+   |
 *                                |                      |00::22:25                      +---+
 *                                |                      |                               00::05
 *                                |                      |
 *                                |                      |  VLAN=26   egress_port[4]     +---+
 *                                |                      +-------------------------------+   |
 *                                |                      |00::22:26                      +---+
 *                                +----------------------+                               00::06
 *                                                                     VP LAG
 *
 * Summary:
 * ========
 * Cint example to demonstrate VxLAN Bridging configuration for tunnel Initiation -
 *             - access to network, with multiple network ports being a LAG (VP LAG)
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects six ports and configure them in Loopback mode. Out of these
 *        six ports, one port is used as ingress_port and the others as
 *        egress_ports.
 *
 *     b) Install an IFP rule to copy ingress & egress packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Configuration (Done in config_vxlan_ipv4_vtep()):
 *   =========================================================
 *     a) Configure a VxLAN Bridging configuration.
 *        This does the necessary configurations of vxlan global settings, vlan,
 *        access and network port setting, tunnel setup, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l2 show', traversing the vpn and vp(s)
 *
 *     b) Transmit 5 known unicast packets. The contents of the packets
 *        are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see the tunnel packet on the egress port with the IPv4 encapsulation
 *       and 'show c' to check the Tx/Rx packet stats/counters.
 *       Also, 'show c s' shows the packet distribution among VP LAG member ports.
 */

/*
Packets:

ACCESS-TO-NETWORK:-
Encap-case: packets sent from ingress_port(access) to egress_port(network)

Ingress Packets: (No. of packets = 5)
======

Header Info

  ###[ Ethernet ]###
    dst= 00:00:00:00:aa:aa
    src= 00:00:00:00:bb:bb
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 21
     type= 0x800
  ###[ IP ]###
        version= 4
        proto= udp
        src= 1.1.1.1, 1.1.1.2, 1.1.1.5, 1.1.1.6, 1.1.1.20 <== 5 packets' only changing contents
        dst= 2.2.2.2
  ###[ UDP ]###
          sport= 53
          dport= 54

Ingress Hex Packet:
tx port=1 data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174BA010101010202020200350036001AB0F8000102030405060708090A0B0C0D0E0F101133CF192C

tx port=1 data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174B9010101020202020200350036001AB0F7000102030405060708090A0B0C0D0E0F1011975E7DF7

tx port=1 data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174b6010101050202020200350036001AB0F8000102030405060708090A0B0C0D0E0F101133CF192C

tx port=1 data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174B5010101060202020200350036001AB0F3000102030405060708090A0B0C0D0E0F101196025567

tx port=1 data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174a7010101140202020200350036001AB0F8000102030405060708090A0B0C0D0E0F101133CF192C

Egress Packet :
======

Header Info

  ###[ Ethernet ]###
    dst= 00:00:00:00:00:02
    src= 00:00:00:00:22:22
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 22
     type= 0x800
  ###[ IP ]###
     version= 4
     proto= udp
     src= 10.10.10.1
     dst= 192.168.1.1
  ###[ UDP ]###
     sport= 65535
     dport= 8472
  ###[ VXLAN ]###
     flags= Instance
     reserved1= 0
     vni= 0x12345
     reserved2= 0x0
  ###[ Ethernet ]###
                 dst= 00:00:00:00:aa:aa
                 src= 00:00:00:00:bb:bb
                 type= 0x800
  ###[ 802.1Q ]###
     vlan= 21
     type= 0x800
  ###[ IP ]###
        version= 4
        proto= udp
        src= 1.1.1.1, 1.1.1.2, 1.1.1.5, 1.1.1.6, 1.1.1.20 <== 5 packets' contents
        dst= 2.2.2.2
  ###[ UDP ]###

Packet Dump
Egress packet#1 -
data[0000]: {000000000006} {000000002226} 8100 001a
data[0010]: 0800 4500 0064 0000 0000 3f11 a5cb 0a0a
data[0020]: 0a06 c0a8 0106 cdb9 12b5 0050 0000 0800
data[0030]: 0000 0123 4500 0000 0000 aaaa 0000 0000
data[0040]: bbbb 0800 4500 002e 0000 0000 4011 74ba
data[0050]: 0101 0101 0202 0202 0035 0036 001a b0f8
data[0060]: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
data[0070]: 1011 33cf 192c

Egress packet#2 -
data[0000]: {000000000006} {000000002226} 8100 001a
data[0010]: 0800 4500 0064 0000 0000 3f11 a5cb 0a0a
data[0020]: 0a06 c0a8 0106 cdb9 12b5 0050 0000 0800
data[0030]: 0000 0123 4500 0000 0000 aaaa 0000 0000
data[0040]: bbbb 0800 4500 002e 0000 0000 4011 74b9
data[0050]: 0101 0102 0202 0202 0035 0036 001a b0f7
data[0060]: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
data[0070]: 1011 975e 7df7

Egress packet#3 -
data[0000]: {000000000005} {000000002225} 8100 0019
data[0010]: 0800 4500 0064 0000 0000 3f11 a5cd 0a0a
data[0020]: 0a05 c0a8 0105 cdb9 12b5 0050 0000 0800
data[0030]: 0000 0123 4500 0000 0000 aaaa 0000 0000
data[0040]: bbbb 0800 4500 002e 0000 0000 4011 74b6
data[0050]: 0101 0105 0202 0202 0035 0036 001a b0f8
data[0060]: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
data[0070]: 1011 33cf 192c

Egress packet#4 -
data[0000]: {000000000002} {000000002222} 8100 0016
data[0010]: 0800 4500 0064 0000 0000 3f11 a5d3 0a0a
data[0020]: 0a02 c0a8 0102 cdb9 12b5 0050 0000 0800
data[0030]: 0000 0123 4500 0000 0000 aaaa 0000 0000
data[0040]: bbbb 0800 4500 002e 0000 0000 4011 74b5
data[0050]: 0101 0106 0202 0202 0035 0036 001a b0f3
data[0060]: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
data[0070]: 1011 9602 5567

Egress packet#5 -
data[0000]: {000000000002} {000000002222} 8100 0016
data[0010]: 0800 4500 0064 0000 0000 3f11 a5d3 0a0a
data[0020]: 0a02 c0a8 0102 cdb9 12b5 0050 0000 0800
data[0030]: 0000 0123 4500 0000 0000 aaaa 0000 0000
data[0040]: bbbb 0800 4500 002e 0000 0000 4011 74a7
data[0050]: 0101 0114 0202 0202 0035 0036 001a b0f8
data[0060]: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
data[0070]: 1011 33cf 192c
*/

/* Reset C interpreter*/
cint_reset();
int NO_OF_VPLAG_MEMBERS = 5;
bcm_error_t show_vxlan_vtep_config(int unit);

bcm_port_t ingress_port;
bcm_port_t egress_port[NO_OF_VPLAG_MEMBERS];
bcm_gport_t ingress_phy_gport;
bcm_gport_t egress_phy_gport[NO_OF_VPLAG_MEMBERS];

/* struct for Access Port Params for creating access VP*/
struct vxlan_acc_cfg_t {
    bcm_gport_t   phy_Gport;/* Input : gport of the port*/
    bcm_vlan_t    acc_vlan; /* access vlan*/
    bcm_mac_t     acc_local_mac;
    bcm_mac_t     acc_remote_mac;
    bcm_gport_t   acc_gport; /* Output:Access VP gport */
};

/* struct for Network Port params */
struct vxlan_net_cfg_t {
    bcm_port_t    phy_Gport;/* Input : gport of the port*/
    bcm_vlan_t    net_vlan;
    bcm_mac_t     net_local_mac;
    bcm_mac_t     net_remote_mac;
    bcm_ip_t      remote_ip;
    uint32        vxlan_vnid;
    bcm_ip_t      local_ip;
    uint16 udp_sp;
    bcm_gport_t   nw_gport; /* Output:Nw VP gport */
};

struct vxlan_cfg_t {
    bcm_vpn_t vxlan_vpn;
    uint32    vxlan_vnid;/*use vnid for vp sharing*/
    bcm_multicast_t mc_group;
};

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
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    group_config.mode = bcmFieldGroupModeAuto;
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
	group_config.priority = 1;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

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
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
	group_config.mode = bcmFieldGroupModeAuto;
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
	BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
	group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified set of ports to CPU. Ports are also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
    bcm_error_t
multi_egress_port_setup(int unit, bcm_port_t *port)
{
    bcm_field_group_config_t group_config;
    bcm_field_entry_t entry;
    int i=0;

    for(i=0; i<NO_OF_VPLAG_MEMBERS; i++)
        {
            BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port[i], BCM_PORT_LOOPBACK_MAC));
        }

    bcm_field_group_config_t_init(&group_config);
	group_config.mode = bcmFieldGroupModeAuto;
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
	group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    for(i=0; i<NO_OF_VPLAG_MEMBERS; i++)
    {
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port[i], BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
    bcm_error_t
test_setup(int unit)
{
    int port_list[NO_OF_VPLAG_MEMBERS+1]; /*Container for VP LAG egress ports and ingress port */
        int i=0;
    if (BCM_E_NONE != portNumbersGet(unit, port_list, NO_OF_VPLAG_MEMBERS+1)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    for(i=0; i < NO_OF_VPLAG_MEMBERS; i++)
    {
        egress_port[i] = port_list[i+1];
    }
    if (BCM_E_NONE != bcm_port_gport_get(unit, ingress_port, &(ingress_phy_gport))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", ingress_port);
        return -1;
    }

    for(i=0; i < NO_OF_VPLAG_MEMBERS; i++)
    {
        if (BCM_E_NONE !=bcm_port_gport_get(unit, egress_port[i], &(egress_phy_gport[i]))){
            printf("bcm_port_gport_get() failed to get gport for port %d\n", egress_port[i]);
            return -1;
        }
    }

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != multi_egress_port_setup(unit,egress_port)) {
        printf("multi_egress_port_setup() failed");
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

    void
verify(int unit)
{
    char str[512];
    char pkt_data[5][512];
    int i=0;
    /*Initalize the pkt_data with the actual ingress packets to be used for testing*/
    snprintf(pkt_data[0],512,"%s","00000000AAAA00000000BBBB8100001508004500002E00000000401174BA010101010202020200350036001AB0F8000102030405060708090A0B0C0D0E0F101133CF192C");

    snprintf(pkt_data[1],512,"%s","00000000AAAA00000000BBBB8100001508004500002E00000000401174B9010101020202020200350036001AB0F7000102030405060708090A0B0C0D0E0F1011975E7DF7");

    snprintf(pkt_data[2],512,"%s","00000000AAAA00000000BBBB8100001508004500002E00000000401174b6010101050202020200350036001AB0F8000102030405060708090A0B0C0D0E0F101133CF192C");

    snprintf(pkt_data[3],512,"%s","00000000AAAA00000000BBBB8100001508004500002E00000000401174B5010101060202020200350036001AB0F3000102030405060708090A0B0C0D0E0F101196025567");

    snprintf(pkt_data[4],512,"%s","00000000AAAA00000000BBBB8100001508004500002E00000000401174a7010101140202020200350036001AB0F8000102030405060708090A0B0C0D0E0F101133CF192C");

//  bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "clear counters");

    for(i=0; i<sizeof(pkt_data)/512; i++)
    {
        printf("\nSending known unicast packet No.[%d] to ingress_port access side:%d\n", i+1,ingress_port);
        snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 8", ingress_port,pkt_data[i]);
        bshell(unit, str);
        bshell(unit, "show counters");
    }
    /* Dump the Configured VPN & VP's*/
    show_vxlan_vtep_config(unit);

    printf("\n----------------------------------------------\n");
    bshell(unit, "l2 show");

    printf("\n----------------------------------------------\n");
    printf("Distribution of %d Packets on trunk ports:",sizeof(pkt_data)/512);
    printf("\n----------------------------------------------\n");
    bshell(unit, "sleep quiet 1;show counters Changed Same NZ");
    return;
}

/* Create vlan and add port to vlan */
    int
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_vlan_create(unit, vid);
    if (BCM_FAILURE(rv) && (rv!=BCM_E_EXISTS)) {
        printf("\vlan_create_add_port() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* vxlan system configuration */
    int
vxlan_system_config(int unit, vxlan_cfg_t *vxlan_cfg)
{
    printf("Configure Vxlan Global Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_vlan_control_set(unit, bcmVlanTranslate, 1));
    return BCM_E_NONE;
}

/* vxlan configuration specific to access physical port */
    int
vxlan_access_port_config(int unit, bcm_gport_t gport_acc)
{
    printf("Configure Vxlan Access Port Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, gport_acc, bcmVlanTranslateIngressEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_acc, bcmPortControlVxlanEnable, 0));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_acc, bcmPortControlVxlanTunnelbasedVnId, 0));
    return BCM_E_NONE;
}

/* vxlan configuration specific to network physical port */
    int
vxlan_network_port_config(int unit, bcm_gport_t gport_net)
{
    printf("Configure Vxlan Network Port Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_net, bcmPortControlVxlanEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_net, bcmPortControlVxlanTunnelbasedVnId, 0));
    return BCM_E_NONE;
}

/* Create L3 interface */
    int
create_l3_interface(int unit, int flags, int intf_flags, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
	l3_intf.l3a_intf_flags = intf_flags;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    *intf_id = l3_intf.l3a_intf_id;
    return rv;
}

    int
vxlan_create_egr_obj(int unit, uint32 flag, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
        int vid, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags2 |= flag;
    l3_egress.intf  = l3_if;
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.mtu = 1500;
    l3_egress.port = gport;
    return bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
}

/* Create the VPN for vxlan */
    int
create_vxlan_vpn(int unit, vxlan_cfg_t *vxlan_cfg)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_multicast_t mc_group=0;

    printf("Create Multicast group for UUC/MC/BC.\n");
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_FLOW, &mc_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in multicast create: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Create Flow VPN.\n");

    bcm_flow_vpn_config_t vpn_info;
    bcm_flow_vpn_config_t_init (&vpn_info);
    vpn_info.flags = BCM_FLOW_VPN_ELAN;
    vpn_info.broadcast_group         = mc_group;
    vpn_info.unknown_unicast_group   = mc_group;
    vpn_info.unknown_multicast_group = mc_group;
    BCM_IF_ERROR_RETURN(bcm_flow_vpn_create(unit, &(vxlan_cfg->vxlan_vpn), &vpn_info));

    vxlan_cfg->mc_group=mc_group;

    return BCM_E_NONE;
}

/* Common code for Access/Network VP's */
    int
create_vxlan_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t *vp)
{
    bcm_error_t rv = BCM_E_NONE;
    printf("Create Flow VP.\n");
    bcm_flow_port_t  flow_port;
    bcm_flow_port_t_init(&flow_port);
    flow_port.flags = flags;
    rv = bcm_flow_port_create(unit, vpn, &flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_flow_port_create() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    *vp = flow_port.flow_port_id;
    return BCM_E_NONE;
}

/* Match criteria configuration */
    int
vxlan_flow_match_add(int unit, bcm_flow_match_config_t match_info)
{
    bcm_error_t rv = BCM_E_NONE;
    rv = bcm_flow_match_add(unit, &match_info, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_flow_match_add() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Encap configuration */
    int
vxlan_flow_port_encap_set(int unit, bcm_flow_port_encap_t peinfo)
{
    bcm_error_t rv = BCM_E_NONE;
    rv =  bcm_flow_port_encap_set(unit, &peinfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_flow_port_encap_set() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Static L2 entry for forwarding */
    int
add_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port)
{
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = port;
    return bcm_l2_addr_add(unit, &l2_addr);
}

/* Static L2 entry for forwarding with LAG destination*/
    int
add_lag_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int lag_id)
{
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC|BCM_L2_TRUNK_MEMBER;
    l2_addr.tgid = lag_id;
    return bcm_l2_addr_add(unit, &l2_addr);
}

/* Access Vxlan VP creation */
    int
config_vxlan_access(int unit, vxlan_cfg_t *vxlan_cfg, vxlan_acc_cfg_t *vxlan_acc, bcm_gport_t *acc_flow_port, bcm_flow_handle_t flow_handle)
{
    /* Access Configuartion variables */
    bcm_error_t rv = BCM_E_NONE;
    bcm_flow_port_encap_t        peinfo;
    bcm_flow_match_config_t      match_info;
    bcm_port_t                  phy_port;

    print vxlan_acc->phy_Gport;

    rv = bcm_port_local_get(unit, vxlan_acc->phy_Gport, &(phy_port));
    if (BCM_FAILURE(rv)) {
        printf("\nError in getting LocalPort: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vlan_create_add_port(unit, vxlan_acc->acc_vlan, phy_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring vlan: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set port default vlan id */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, phy_port, vxlan_acc->acc_vlan));
    /* Set default inner tpid per port */
    BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit, phy_port, 0x9100));
    /* Set default outer tpid per port */
    BCM_IF_ERROR_RETURN(bcm_port_tpid_set(unit, phy_port, 0x8100));
    /* Set port learning */
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, phy_port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
    /* Port must be added to VPN to pass membership check */
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vxlan_cfg->vxlan_vpn, vxlan_acc->phy_Gport, 0));

    rv = vxlan_access_port_config(unit, vxlan_acc->phy_Gport);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create Access Flow Port (VP).\n");
    rv = create_vxlan_vp(unit, vxlan_cfg->vxlan_vpn, 0, acc_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access vp: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    vxlan_acc->acc_gport=(*acc_flow_port);

    printf("Access: Add flow match (match: port,vlan criteria).\n");
    int acc_port_class = vxlan_acc->phy_Gport & 0xff;
    BCM_IF_ERROR_RETURN(bcm_port_class_set(unit, vxlan_acc->phy_Gport, bcmPortClassIngress, acc_port_class));
    bcm_flow_match_config_t_init(&match_info);
    match_info.criteria  = BCM_FLOW_MATCH_CRITERIA_PORT_GROUP_VLAN;
    match_info.port      = acc_port_class;
    match_info.vlan      = vxlan_acc->acc_vlan;
    match_info.vpn       = vxlan_cfg->vxlan_vpn;
    match_info.valid_elements = (BCM_FLOW_MATCH_PORT_VALID | BCM_FLOW_MATCH_VLAN_VALID |
                                 BCM_FLOW_MATCH_VPN_VALID);
    rv = vxlan_flow_match_add(unit, match_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access flow match (VPN assignment): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_flow_match_config_t_init(&match_info);
    match_info.flow_handle = flow_handle;
    match_info.criteria = BCM_FLOW_MATCH_CRITERIA_PORT;
    match_info.flow_port = vxlan_acc->acc_gport;
    match_info.port = vxlan_acc->phy_Gport;
    match_info.valid_elements = BCM_FLOW_MATCH_PORT_VALID | BCM_FLOW_MATCH_FLOW_PORT_VALID;
    rv = vxlan_flow_match_add(unit, match_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access flow match (SVP assignment): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Tunnel creation and associating it with Nw VP */
    int
config_vxlan_tunnel(int unit, vxlan_cfg_t *vxlan_cfg, vxlan_net_cfg_t *vxlan_net, bcm_gport_t net_flow_port, bcm_flow_handle_t flow_handle)
{
    /* Tunnel Configuration variables */
    bcm_error_t rv = BCM_E_NONE;
    bcm_flow_tunnel_initiator_t  tiinfo;
    bcm_flow_tunnel_terminator_t ttinfo;

    printf("Tunnel_init: Create Flow Tunnel Initiator.\n");
    bcm_flow_tunnel_initiator_t_init(&tiinfo);
    tiinfo.flow_handle = flow_handle;
    tiinfo.type = bcmTunnelTypeVxlan;
    tiinfo.valid_elements = BCM_FLOW_TUNNEL_INIT_DIP_VALID |
        BCM_FLOW_TUNNEL_INIT_SIP_VALID |
        BCM_FLOW_TUNNEL_INIT_TTL_VALID;
    tiinfo.flow_port = net_flow_port;
    tiinfo.sip = vxlan_net->local_ip;
    tiinfo.dip = vxlan_net->remote_ip;
    tiinfo.ttl = 0x3f;
    rv = bcm_flow_tunnel_initiator_create(0,&tiinfo,0,NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring tunnel: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Tunnel_term: Create Flow Tunnel Terminator.\n");
    bcm_flow_tunnel_terminator_t_init(&ttinfo);
    ttinfo.flow_handle = flow_handle;
    ttinfo.valid_elements = BCM_FLOW_TUNNEL_TERM_DIP_VALID;
    ttinfo.dip = vxlan_net->local_ip;
    ttinfo.type = bcmTunnelTypeVxlan;
    rv = bcm_flow_tunnel_terminator_create(0,&ttinfo,0,NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring tunnel: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/* vxlan network VP creation . tunnel is not created */
    int
config_vxlan_network(int unit, vxlan_cfg_t *vxlan_cfg, vxlan_net_cfg_t *vxlan_net, bcm_gport_t *net_flow_port, bcm_flow_handle_t flow_handle)
{
    /* Network Configuartion variables */
    bcm_error_t rv = BCM_E_NONE;
    bcm_if_t      net_intf_id;
    bcm_if_t      net_egr_id;
    bcm_flow_port_encap_t        peinfo;
    bcm_flow_match_config_t      match_info;
    bcm_flow_encap_config_t      einfo;
    bcm_port_t                  phy_port;

    print vxlan_net->phy_Gport;

    rv = bcm_port_local_get(unit, vxlan_net->phy_Gport, &(phy_port));
    if (BCM_FAILURE(rv)) {
        printf("\nError in getting LocalPort: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vlan_create_add_port(unit, vxlan_net->net_vlan, phy_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring vlan: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set port default vlan id */
    BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, phy_port, vxlan_net->net_vlan));
    /* Set default inner tpid per port */
    BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit, phy_port, 0x9100));
    /* Set default outer tpid per port */
    BCM_IF_ERROR_RETURN(bcm_port_tpid_set(unit, phy_port, 0x8100));
    /* Set port learning */
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, phy_port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
    /* Port must be added to VPN to pass membership check */
    bcm_gport_t gport_network_n = BCM_GPORT_INVALID;
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, phy_port, &gport_network_n));
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vxlan_cfg->vxlan_vpn, gport_network_n, 0));

    rv = vxlan_network_port_config(unit, vxlan_net->phy_Gport);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create network L3 interface\n");
    rv = create_l3_interface(unit, 0, BCM_L3_INTF_UNDERLAY, vxlan_net->net_local_mac, vxlan_net->net_vlan, &net_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    int station_id;
    bcm_l2_station_t l2_station;
    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, vxlan_net->net_local_mac, sizeof(vxlan_net->net_local_mac));;
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    rv = bcm_l2_station_add(0, &station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring MY_STATION: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create network L3 Egress object.\n");
    rv = vxlan_create_egr_obj(unit, BCM_L3_FLOW_ONLY|BCM_L3_FLAGS2_UNDERLAY, net_intf_id, vxlan_net->net_remote_mac,
            vxlan_net->phy_Gport, vxlan_net->net_vlan, &net_egr_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring egress obj: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create Network Flow Port.\n");
    rv = create_vxlan_vp(unit, vxlan_cfg->vxlan_vpn, BCM_FLOW_PORT_NETWORK, net_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network vp: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    vxlan_net->nw_gport=*net_flow_port;

    printf("Network: encap add, VFI-->VNID\n");
    bcm_flow_match_config_t_init(&match_info);
    match_info.flow_handle    = flow_handle;
    match_info.criteria       = BCM_FLOW_MATCH_CRITERIA_VN_ID;
    match_info.vpn            = vxlan_cfg->vxlan_vpn;
    match_info.vnid           = vxlan_net->vxlan_vnid;
    match_info.valid_elements = BCM_FLOW_MATCH_VPN_VALID | BCM_FLOW_MATCH_VNID_VALID;
    rv = bcm_flow_match_get(unit, &match_info, 0, NULL);
    if (rv == BCM_E_NOT_FOUND) { /*Create a new flow only if one doesn't exist */
        bcm_flow_encap_config_t_init(&einfo);
        einfo.flow_handle = flow_handle;
        einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_VFI;
        einfo.vnid = vxlan_net->vxlan_vnid;
        einfo.vpn  = vxlan_cfg->vxlan_vpn;
        einfo.valid_elements = BCM_FLOW_ENCAP_VNID_VALID | BCM_FLOW_ENCAP_VPN_VALID;
        rv = bcm_flow_encap_add(unit, &einfo, 0, NULL);
        if (BCM_FAILURE(rv)) {
            printf("\nNetwork: bcm_flow_encap_add() failed: %s\n", bcm_errmsg(rv));
            return rv;
        }

        printf("Network: Configure match condition to derive VPN (Match: {VN_ID}, Derive: VFI (VPN))\n");
        bcm_flow_match_config_t_init(&match_info);
        match_info.flow_handle    = flow_handle;
        match_info.criteria       = BCM_FLOW_MATCH_CRITERIA_VN_ID;
        match_info.vpn            = vxlan_cfg->vxlan_vpn;
        match_info.vnid           = vxlan_net->vxlan_vnid;
        match_info.valid_elements = BCM_FLOW_MATCH_VPN_VALID |
            BCM_FLOW_MATCH_VNID_VALID;
        rv = vxlan_flow_match_add(unit, match_info);
        if (BCM_FAILURE(rv)) {
            printf("\nError in configuring network flow match: %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    printf("Network; Set Encapsulation. VP=>egress-obj{Device,Port}.\n");
    bcm_flow_port_encap_t_init(&peinfo);
    peinfo.flow_handle = flow_handle;
    peinfo.flow_port = *net_flow_port;
    peinfo.egress_if = net_egr_id;
    peinfo.valid_elements = BCM_FLOW_PORT_ENCAP_PORT_VALID |
        BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID;
    rv = vxlan_flow_port_encap_set(unit, peinfo);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network encap: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Network: Add flow match (match: SIP => network SVP)\n");
    bcm_flow_match_config_t_init(&match_info);
    match_info.flow_handle = flow_handle;
    match_info.criteria = BCM_FLOW_MATCH_CRITERIA_SIP;
    match_info.sip = vxlan_net->remote_ip;
    match_info.flow_port = *net_flow_port;
    match_info.valid_elements = BCM_FLOW_MATCH_SIP_VALID |
        BCM_FLOW_MATCH_FLOW_PORT_VALID;
    rv = vxlan_flow_match_add(unit, match_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network flow match: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* FlexDigest configuration*/
    int
config_FlexDigest(int unit)
{
    int seed_profile_id, norm_profile_id, hash_profile_id;
    int value;
    uint16 salt_a[16] = {
        0xabcd,
        0xbbcb,
        0xbabe,
        0x0123,
        0x7654,
        0xabcd,
        0xaaaa,
        0x5555,
        0xffff,
        0x0000,
        0xa5a5,
        0xface,
        0xbbbb,
        0xcccc,
        0xdddd,
        0xeeee
    };
    bcm_flexdigest_qset_t  qset;
    bcm_flexdigest_group_t group;
    bcm_flexdigest_entry_t entry;
    bcm_flexdigest_match_id_t match_id;
    int pri = 0;

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_init(unit));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_seed_profile_create(unit, 0, &seed_profile_id));
    value = 0xbabaface;
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id,
                                              bcmFlexDigestNormSeedControlBinASeed,
                                              value));
    value = 0xa5a5a5a5;
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id,
                                              bcmFlexDigestNormSeedControlBinBSeed,
                                              value));
    value = 0x5a5a5a5a;
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id,
                                              bcmFlexDigestNormSeedControlBinCSeed,
                                              value));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_profile_create(unit, 0, &norm_profile_id));
    value = 1;
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_profile_set(unit, norm_profile_id,
                                         bcmFlexDigestNormProfileControlNorm, value));
    value = 0;
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_profile_set(unit, norm_profile_id,
                                         bcmFlexDigestNormProfileControlSeedId, value));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_create(unit, 0, &hash_profile_id));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlPreProcessBinA,
                                         1));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlPreProcessBinB,
                                         1));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlPreProcessBinC,
                                         0));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlXorSaltBinA,
                                         1));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlXorSaltBinB,
                                         1));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlXorSaltBinC,
                                         0));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlBinA0FunctionSelection,
                                         BCM_FLEXDIGEST_HASH_FUNCTION_CRC32LO));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlBinA1FunctionSelection,
                                         BCM_FLEXDIGEST_HASH_FUNCTION_CRC32HI));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlBinB0FunctionSelection,
										 BCM_FLEXDIGEST_HASH_FUNCTION_CRC32_ETH_LO));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlBinB1FunctionSelection,
										 BCM_FLEXDIGEST_HASH_FUNCTION_CRC32_ETH_HI));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlBinC0FunctionSelection,
                                         BCM_FLEXDIGEST_HASH_FUNCTION_CRC16));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlBinC1FunctionSelection,
                                         BCM_FLEXDIGEST_HASH_FUNCTION_CRC16CCITT));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_salt_set(unit, bcmFlexDigestHashBinA, 16, salt_a));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_salt_set(unit, bcmFlexDigestHashBinB, 16, salt_a));

    BCM_FLEXDIGEST_QSET_INIT(qset);
    BCM_FLEXDIGEST_QSET_ADD(qset, bcmFlexDigestQualifyMatchId);
    BCM_FLEXDIGEST_QSET_ADD(qset, bcmFlexDigestQualifyL4Valid);
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_group_create(unit, qset, pri, &group));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_entry_priority_set(unit, entry, 2));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_match_id_create(unit, &match_id));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL2HdrL2));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_qualify_MatchId(unit, entry, match_id));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_qualify_L4Valid(unit, entry, 1, 0x1));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds0,  BCM_FLEXDIGEST_FIELD_OUTER_MACSA_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds1,  BCM_FLEXDIGEST_FIELD_OUTER_MACSA_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds2,  BCM_FLEXDIGEST_FIELD_OUTER_MACSA_32_47, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds3,  BCM_FLEXDIGEST_FIELD_OUTER_MACDA_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds4,  BCM_FLEXDIGEST_FIELD_OUTER_MACDA_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds5,  BCM_FLEXDIGEST_FIELD_OUTER_MACDA_32_47, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds6,  BCM_FLEXDIGEST_FIELD_OUTER_OTAG_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds7,  BCM_FLEXDIGEST_FIELD_OUTER_OTAG_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds8,  BCM_FLEXDIGEST_FIELD_OUTER_ETHERTYPE, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds9,  BCM_FLEXDIGEST_FIELD_INGRESS_PP_PORT, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds10, BCM_FLEXDIGEST_FIELD_L2_IIF, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds11, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_A_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds12, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_A_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds13, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds14, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds15, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_C_0_15, 0xffff));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds0,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds1,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds2,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_32_47, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds3,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_48_63, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds4,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_64_79, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds5,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_80_95, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds6,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_96_111, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds7,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_112_127, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds8,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds9,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds10, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_32_47, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds11, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_48_63, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds12, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_64_79, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds13, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_80_95, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds14, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_96_111, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds15, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_112_127, 0xffff));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds0, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds1, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds2, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds3, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds4, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds5, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds6, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds7, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds8, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds9, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds10, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds11, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds12, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds13, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds14, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds15, 0, 0x1));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_entry_install(unit, entry));

    /* VPLAG FlexDigest */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelVpTrunk, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, ingress_port, bcmSwitchVpTrunkHashOffset, 48));

    return BCM_E_NONE;
}

/*VPLAG configuration*/
    int
config_vplag(int unit,int *vplag_id,bcm_gport_t *flow_gport_array)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t member_array[NO_OF_VPLAG_MEMBERS];
    int i=0;

    bcm_trunk_info_t_init(&trunk_info);
    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;
    trunk_info.dlf_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index= BCM_TRUNK_UNSPEC_INDEX;

    for(i=0; i<NO_OF_VPLAG_MEMBERS; i++)
    {
        bcm_trunk_member_t_init(&member_array[i]);
        member_array[i].gport=flow_gport_array[i];
    }

    BCM_IF_ERROR_RETURN(bcm_trunk_create(unit, BCM_TRUNK_FLAG_VP, vplag_id));
    BCM_IF_ERROR_RETURN(bcm_trunk_set(unit,*vplag_id, &trunk_info, NO_OF_VPLAG_MEMBERS, member_array));

    bcm_gport_t vplag_gport;
    BCM_GPORT_TRUNK_SET(vplag_gport, *vplag_id);
    printf("VPLAG GPORT = 0x%x\n", vplag_gport);

    return BCM_E_NONE;
}

/* vxlan Configuration function */
    bcm_error_t
config_vxlan_ipv4_vtep(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* VPN Configuration variables */
    vxlan_cfg_t vxlan_cfg = {
        0x7010,
        0,
        0
    };

    vxlan_acc_cfg_t vxlan_acc = {
        ingress_phy_gport, /* port */
        21,/*vlan */
        "00:00:00:00:11:11",/*local mac */
        "00:00:00:00:00:01",/* remote mac */
        0
    };

    vxlan_net_cfg_t vxlan_net[NO_OF_VPLAG_MEMBERS] = { /*Parameters of all Network ports that port VPLAG */
        {
            egress_phy_gport[0], /* Egress port */
            22, /* vlan */
            "00:00:00:00:22:22",/* local mac */
            "00:00:00:00:00:02",/* remote mac */
            0xC0A80102,  /*192.168.1.1 remote IP*/
            0x12345,     /* vnid */
            0x0a0a0a02,  /*10.10.10.1 local IP*/
            0xffff,
            0
        },
        {
            egress_phy_gport[1], /* Egress port */
            23, /* vlan */
            "00:00:00:00:22:23",/* local mac */
            "00:00:00:00:00:03",/* remote mac */
            0xC0A80103,  /*192.168.1.1 remote IP*/
            0x12345,     /* vnid */
            0x0a0a0a03,  /*10.10.10.1 local IP*/
            0xffff,
            0
        },
        {
            egress_phy_gport[2], /* Egress port */
            24, /* vlan */
            "00:00:00:00:22:24",/* local mac */
            "00:00:00:00:00:04",/* remote mac */
            0xC0A80104,  /*192.168.1.1 remote IP*/
            0x12345,     /* vnid */
            0x0a0a0a04,  /*10.10.10.1 local IP*/
            0xffff,
            0
        },
        {
            egress_phy_gport[3], /* Egress port */
            25, /* vlan */
            "00:00:00:00:22:25",/* local mac */
            "00:00:00:00:00:05",/* remote mac */
            0xC0A80105,  /*192.168.1.1 remote IP*/
            0x12345,     /* vnid */
            0x0a0a0a05,  /*10.10.10.1 local IP*/
            0xffff,
            0
        },
        {
            egress_phy_gport[4], /* Egress port */
            26, /* vlan */
            "00:00:00:00:22:26",/* local mac */
            "00:00:00:00:00:06",/* remote mac */
            0xC0A80106,  /*192.168.1.1 remote IP*/
            0x12345,     /* vnid */
            0x0a0a0a06,  /*10.10.10.1 local IP*/
            0xffff,
            0
        },
    };


    /* flow API */
    bcm_flow_handle_t            flow_handle;
    bcm_gport_t   acc_flow_port;
    bcm_gport_t   net_flow_port[NO_OF_VPLAG_MEMBERS];
    int vplag_id;

    /* working variables */
    uint32 flags=0;
    int i=0;

    rv = vxlan_system_config(unit, &vxlan_cfg);
    if (BCM_FAILURE(rv)) {
        printf("\nError in global setting: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Get flow-handler for Classic_Vxlan\n");
    rv = bcm_flow_handle_get(unit, "CLASSIC_VXLAN", &flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in getting flow handle: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vxlan_vpn(unit, &vxlan_cfg);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vpn create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_access(unit, &vxlan_cfg, &vxlan_acc, &acc_flow_port, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_FlexDigest(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in FlexDigest config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    for(i=0;i<NO_OF_VPLAG_MEMBERS;i++)
    {
        rv = config_vxlan_network(unit, &vxlan_cfg, &vxlan_net[i], &net_flow_port[i], flow_handle);
        if (BCM_FAILURE(rv)) {
            printf("\nError in network config: %s.\n", bcm_errmsg(rv));
            return rv;
        }

        rv = config_vxlan_tunnel(unit, &vxlan_cfg, &vxlan_net[i], net_flow_port[i], flow_handle);
        if (BCM_FAILURE(rv)) {
            printf("\nError in tunnel config: %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /*Create a VPLAG of all the VxLAN network ports*/
    rv = config_vplag(unit,&vplag_id,net_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in VPLAG config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Access-to-Network: Add payload-dest-mac Address => Network-flow-port (VP).\n");
    rv = add_lag_to_l2_table(unit, "00:00:00:00:aa:aa", vxlan_cfg.vxlan_vpn, vplag_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in l2 entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;

}
/* verifying the VPN VP configuration using traverse*/
    int
vpn_traverse(int unit,bcm_vpn_t vpn, bcm_flow_vpn_config_t *info, void *data)
{
    int maxArr=20;
    int count=0,iter=0;
    bcm_flow_port_t flow_portArr[maxArr];

    printf("\n Vpn :%d \n",vpn);
    printf("\n VpnInfo : \n");
    print *info;
    BCM_IF_ERROR_RETURN(bcm_flow_port_get_all(unit,vpn,maxArr,flow_portArr,&count));

    printf("\n Flow Ports %d\n",count);
            for(iter=0;(iter<count) && (iter < maxArr);++iter)
    {
        print flow_portArr[iter];
    }
}

/* verifying the VPN configuration */
    bcm_error_t
show_vxlan_vtep_config(int unit)
{
    uint32 null_user_data=0;
    void  *user_data_ptr=&null_user_data;
    bcm_flow_vpn_traverse(unit,vpn_traverse,user_data_ptr);
    return 0;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_vxlan_ipv4_vtep())
 *  c) demonstrates the functionality(done in verify()).
 */
    bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;

    print "config show; attach; version";
    bshell(unit, "config show; a ; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_vxlan_ipv4_vtep(): ** start **";
    if (BCM_FAILURE((rv = config_vxlan_ipv4_vtep(unit)))) {
        printf("config_vxlan_ipv4_vtep() failed.\n");
        return -1;
    }
    print "~~~ #2) config_vxlan_ipv4_vtep(): ** end **";

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
