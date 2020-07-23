/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : Vxlan Bridging , IPv4 access to network trunk ports Flow
 *
 * Usage    : BCM.0> cint vxlan_v4_acc_to_net_trunk.c
 *
 * config   : config_td3_flex.bcm
 *
 * Log file : vxlan_v4_acc_to_net_trunk_log.txt
 *
 * Test Topology :
 *
 *                                +----------------------+  VLAN=22   egress_port[0]
 *                                |                      |           +------------------+  Underlay+NextHop
 *                                |                      |Trunk=3    |egress_port[1]         +---+
 * SMAC 00::bb:bb                 |                      +------------------------------+    |   |
 *          +---+          VLAN=21|                      |00::22:22  |egress_port[2]         +---+
 * Host(acc)|   +-----------------+      Trident+3       |           +------------------+    00::02
 *          +---+    ingress_port |                      |           |egress_port[3]
 *                                |                      |           +------------------+
 *                                |                      |           |egress_port[4]
 *                                |                      |           +------------------+
 *                                |                      |
 *                                |                      |
 *                                |                      |
 *                                +----------------------+
 * Summary:
 * ========
 * Cint example to demonstrate VxLAN Bridging configuration for tunnel Initiation -
 *             - access to network, with network port being a trunk (encap flow)
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy ingress & egress packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Configuration (Done in config_vxlan_ipv4_vtep()):
 *   =========================================================
 *     a) Configure a VxLAN Bridging configuration with SDTAG operations.
 *        This does the necessary configurations of vxlan global settings, vlan,
 *        access and network port setting, tunnel setup, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l2 show', 'trunk show', traversing the vpn and vp's
 *
 *     b) Transmit 5 known unicast packets. The contents of the packets
 *        are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see the tunnel packet on the egress port with the IPv4 encapsulation
 *       and 'show c' to check the Tx/Rx packet stats/counters.
 *       Also, 'show c s' shows the packet distribution among trunk member ports.
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
        src= 1.1.1.1, 1.1.1.2, 1.1.1.3, 1.1.1.6, 1.1.1.7 <== 5 packets' only changing contents
        dst= 2.2.2.2
  ###[ UDP ]###
          sport= 53
          dport= 54

Ingress Hex Packet:
tx port=1 data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174BA010101010202020200350036001AB0F8000102030405060708090A0B0C0D0E0F101133CF192C

tx port=1 data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174B9010101020202020200350036001AB0F7000102030405060708090A0B0C0D0E0F1011975E7DF7

tx port=1 data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174B8010101030202020200350036001AB0F6000102030405060708090A0B0C0D0E0F1011D621E6E1

tx port=1 data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174B5010101060202020200350036001AB0F3000102030405060708090A0B0C0D0E0F101196025567

tx port=1 data=0x00000000AAAA00000000BBBB8100001508004500002E00000000401174B4010101070202020200350036001AB0F2000102030405060708090A0B0C0D0E0F1011D77DCE71

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
        src= 1.1.1.1, 1.1.1.2, 1.1.1.3, 1.1.1.6, 1.1.1.7 <== 5 packets' contents
        dst= 2.2.2.2
  ###[ UDP ]###

Packet Dump
Egress packet#1 -
data[0000]: {000000000002} {000000002222} 8100 0016
data[0010]: 0800 4500 0064 0000 0000 3f11 a5d5 0a0a
data[0020]: 0a01 c0a8 0101 ffff 2118 0050 0000 0000
data[0030]: 0000 0123 4500 0000 0000 aaaa 0000 0000
data[0040]: bbbb 0800 4500 002e 0000 0000 4011 74ba
data[0050]: 0101 0101 0202 0202 0035 0036 001a b0f8
data[0060]: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
data[0070]: 1011 33cf 192c c36b b81f

Egress packet#2 -
data[0000]: {000000000002} {000000002222} 8100 0016
data[0010]: 0800 4500 0064 0001 0000 3f11 a5d4 0a0a
data[0020]: 0a01 c0a8 0101 ffff 2118 0050 0000 0000
data[0030]: 0000 0123 4500 0000 0000 aaaa 0000 0000
data[0040]: bbbb 0800 4500 002e 0000 0000 4011 74b9
data[0050]: 0101 0102 0202 0202 0035 0036 001a b0f7
data[0060]: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
data[0070]: 1011 975e 7df7 e707 e61d

Egress packet#3 -
data[0000]: {000000000002} {000000002222} 8100 0016
data[0010]: 0800 4500 0064 0002 0000 3f11 a5d3 0a0a
data[0020]: 0a01 c0a8 0101 ffff 2118 0050 0000 0000
data[0030]: 0000 0123 4500 0000 0000 aaaa 0000 0000
data[0040]: bbbb 0800 4500 002e 0000 0000 4011 74b8
data[0050]: 0101 0103 0202 0202 0035 0036 001a b0f6
data[0060]: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
data[0070]: 1011 d621 e6e1 ea03 8450

Egress packet#4 -
data[0000]: {000000000002} {000000002222} 8100 0016
data[0010]: 0800 4500 0064 0003 0000 3f11 a5d2 0a0a
data[0020]: 0a01 c0a8 0101 ffff 2118 0050 0000 0000
data[0030]: 0000 0123 4500 0000 0000 aaaa 0000 0000
data[0040]: bbbb 0800 4500 002e 0000 0000 4011 74b5
data[0050]: 0101 0106 0202 0202 0035 0036 001a b0f3
data[0060]: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
data[0070]: 1011 9602 5567 ce6f da52

Egress packet#5 -
data[0000]: {000000000002} {000000002222} 8100 0016
data[0010]: 0800 4500 0064 0004 0000 3f11 a5d1 0a0a
data[0020]: 0a01 c0a8 0101 ffff 2118 0050 0000 0000
data[0030]: 0000 0123 4500 0000 0000 aaaa 0000 0000
data[0040]: bbbb 0800 4500 002e 0000 0000 4011 74b4
data[0050]: 0101 0107 0202 0202 0035 0036 001a b0f2
data[0060]: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
data[0070]: 1011 d77d ce71 53db c116

*/

/* Reset C interpreter*/
cint_reset();
int NO_OF_TRUNK_MEMBERS = 5;
bcm_error_t show_vxlan_vtep_config(int unit);

bcm_port_t ingress_port;
bcm_port_t egress_port[NO_OF_TRUNK_MEMBERS];
bcm_gport_t ingress_phy_gport;
bcm_gport_t egress_phy_gport[NO_OF_TRUNK_MEMBERS];
bcm_gport_t trunk_gport;

/* struct for Access Port Params for creating access VP*/
struct vxlan_acc_cfg_t {
    bcm_gport_t   phy_Gport;/* Input : gport of the port/trunk*/
    bcm_vlan_t    acc_vlan; /* access vlan*/
    bcm_mac_t     acc_local_mac;
    bcm_mac_t     acc_remote_mac;
    bcm_gport_t   acc_gport; /* Output:Access VP gport */
};

/* struct for Network Port params */
struct vxlan_net_cfg_t {
    bcm_port_t    phy_Gport;/* Input : gport of the port/trunk*/
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
    uint16 udp_dp;
    bcm_vpn_t vxlan_vpn;
    uint32    vxlan_vnid;/*use vnid for vp sharing*/
    bcm_multicast_t mc_group;
};

/* struct for RTAG7 configuration */
struct rtag7_cfg_t {
    int block_select; /* 0 = 'A block';1 = 'B block'*/

    /** Block A settings **/
    uint32 seed_a;                  /*Seed*/
    int pre_process_a;              /*Pre-process enable*/
    int block_n_offset_a;           /*Block (A0 or A1) and offset to be used for unicast trunk*/
    int block_n_offset_nonucast_a;  /*Block (A0 or A1) and offset to be used for non-unicast trunk*/
    int hash_algo_a_0;              /*A0 hash algorithm*/
    int hash_algo_a_1;              /*A1 hash algorithm*/
    int tcp_ueq_l4_ports_a;         /*Packet field selection for TCP/UDP packets with unequal L4 ports*/
    int tcp_eq_l4_ports_a;          /*Packet field selection for TCP/UDP packets with equal L4 ports*/
    int ipv4_a;                     /*Packet field selection for non-TCP/UDP IP packets*/
    int l2_a;                       /*Packet field selection for L2 packets*/

    /** Block B settings **/
    uint32 seed_b;                  /*Seed*/
    int pre_process_b;              /*Pre-process enable*/
    int block_n_offset_b;           /*Block (B0 or B1) and offset to be used for unicast trunk*/
    int block_n_offset_nonucast_b;  /*Block (B0 or B1) and offset to be used for non-unicast trunk*/
    int hash_algo_b_0;              /*B0 hash algorithm*/
    int hash_algo_b_1;              /*B1 hash algorithm*/
    int tcp_ueq_l4_ports_b;         /*Packet field selection for TCP/UDP packets with unequal L4 ports*/
    int tcp_eq_l4_ports_b;          /*Packet field selection for TCP/UDP packets with equal L4 ports*/
    int ipv4_b;                     /*Packet field selection for non-TCP/UDP IP packets*/
    int l2_b;                       /*Packet field selection for L2 packets*/
};

/* struct for Trunk configuration */
struct trunk_cfg_t {
    bcm_trunk_t     trunk_group_id; /*Trunk group ID value*/
    uint32          flags;
    int             psc;            /*Port selection Criterion (RTAG number) */
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
    bcm_field_qset_t qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
        int i=0;

        for(i=0; i<NO_OF_TRUNK_MEMBERS; i++)
        {
            BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port[i], BCM_PORT_LOOPBACK_MAC));
            BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port[i], BCM_PORT_DISCARD_ALL));
        }
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

    for(i=0; i<NO_OF_TRUNK_MEMBERS; i++)
    {
        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));
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
    int port_list[NO_OF_TRUNK_MEMBERS+1]; /*Container for trunk egress ports and ingress port */
        int i=0;
    if (BCM_E_NONE != portNumbersGet(unit, port_list, NO_OF_TRUNK_MEMBERS+1)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    for(i=0; i < NO_OF_TRUNK_MEMBERS; i++)
    {
        egress_port[i] = port_list[i+1];
    }
    if (BCM_E_NONE != bcm_port_gport_get(unit, ingress_port, &(ingress_phy_gport))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", ingress_port);
        return -1;
    }

    for(i=0; i < NO_OF_TRUNK_MEMBERS; i++)
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

    snprintf(pkt_data[2],512,"%s","00000000AAAA00000000BBBB8100001508004500002E00000000401174B8010101030202020200350036001AB0F6000102030405060708090A0B0C0D0E0F1011D621E6E1");

    snprintf(pkt_data[3],512,"%s","00000000AAAA00000000BBBB8100001508004500002E00000000401174B5010101060202020200350036001AB0F3000102030405060708090A0B0C0D0E0F101196025567");

    snprintf(pkt_data[4],512,"%s","00000000AAAA00000000BBBB8100001508004500002E00000000401174B4010101070202020200350036001AB0F2000102030405060708090A0B0C0D0E0F1011D77DCE71");

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
    bshell(unit, "trunk show");
    bshell(unit, "clear c");

    for(i=0; i<sizeof(pkt_data)/512; i++)
    {
        printf("\nSending known unicast packet No.[%d] to ingress_port access side:%d\n", i+1,ingress_port);
        snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 1", ingress_port,pkt_data[i]);
        bshell(unit, str);
        bshell(unit, "show c");
    }
    /* Dump the Configured VPN & VP's*/
    show_vxlan_vtep_config(unit);

    printf("\n-----------------------------------------\n");
    printf("Distribution of %d Packets on trunk ports:",sizeof(pkt_data)/512);
    printf("\n-----------------------------------------\n");
    bshell(unit, "sleep quiet 1;show c s");
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

/* Create vlan and add trunk ports to vlan */
        int
vlan_create_add_trunk_ports(int unit, int vid)
{
    bcm_pbmp_t pbmp, upbmp;
    int i=0;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    for(i=0; i<NO_OF_TRUNK_MEMBERS; i++)
    {
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_CLEAR(upbmp);
        BCM_PBMP_PORT_ADD(pbmp, egress_port[i]);
    }
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);

}
/* vxlan system configuration */
    int
vxlan_system_config(int unit, vxlan_cfg_t *vxlan_cfg)
{
    printf("Configure Vxlan Global Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1));
    BCM_IF_ERROR_RETURN(bcm_vlan_control_set(unit, bcmVlanTranslate, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchVxlanUdpDestPortSet, vxlan_cfg->udp_dp));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchVxlanTunnelMissToCpu, 1));
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

/* vxlan configuration to network trunk member physical ports */
    int
vxlan_network_trunk_ports_config(int unit)
{
    int i=0;
    for(i=0; i<NO_OF_TRUNK_MEMBERS; i++)
    {
        BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, egress_phy_gport[i], bcmPortControlVxlanEnable, 1));
        BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, egress_phy_gport[i], bcmPortControlVxlanTunnelbasedVnId, 0));
    }
    return BCM_E_NONE;
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

    int
vxlan_create_egr_obj(int unit, uint32 flag, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
        int vid, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags2 |= flag;
    l3_egress.intf  = l3_if;
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.vlan = vid;
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
    vpn_info.flags = BCM_FLOW_VPN_ELAN | BCM_FLOW_VPN_WITH_ID;
    vpn_info.broadcast_group         = mc_group;
    vpn_info.unknown_unicast_group   = mc_group;
    vpn_info.unknown_multicast_group = mc_group;
    BCM_IF_ERROR_RETURN(bcm_flow_vpn_create(unit, vxlan_cfg->vxlan_vpn, &vpn_info));

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
    flow_port.flags = BCM_FLOW_PORT_SERVICE_TAGGED | flags;
    flow_port.egress_service_tpid = 0x8100;
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
        printf("\nAccess: bcm_flow_port_encap_set() failed: %s\n", bcm_errmsg(rv));
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

/* Access Vxlan VP creation */
    int
config_vxlan_access(int unit, vxlan_cfg_t *vxlan_cfg, vxlan_acc_cfg_t *vxlan_acc, bcm_gport_t *acc_flow_port)
{
    /* Access Configuartion variables */
    bcm_error_t rv = BCM_E_NONE;
    bcm_if_t      acc_intf_id;
    bcm_if_t      acc_egr_id;
    bcm_flow_port_encap_t        peinfo;
    bcm_flow_match_config_t      match_info;
    bcm_port_t                  phy_port;

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

    rv = vxlan_access_port_config(unit, vxlan_acc->phy_Gport);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create access L3 interface\n");
    rv = create_l3_interface(unit, 0, vxlan_acc->acc_local_mac, vxlan_acc->acc_vlan, &acc_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create access L3 Egress object.\n");
    rv = vxlan_create_egr_obj(unit, 0, acc_intf_id, vxlan_acc->acc_remote_mac, vxlan_acc->phy_Gport, vxlan_acc->acc_vlan, &acc_egr_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring egress obj: %s.\n", bcm_errmsg(rv));
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
    bcm_flow_match_config_t_init(&match_info);
    match_info.criteria  = BCM_FLOW_MATCH_CRITERIA_PORT_VLAN;
    match_info.port      = vxlan_acc->phy_Gport;
    match_info.vlan      = vxlan_acc->acc_vlan;
    match_info.flow_port = *acc_flow_port;
    match_info.valid_elements = (BCM_FLOW_MATCH_PORT_VALID |
            BCM_FLOW_MATCH_VLAN_VALID |
            BCM_FLOW_MATCH_FLOW_PORT_VALID);
    rv = vxlan_flow_match_add(unit, match_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access flow match: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Access: Set Encapsulation. VP=>egress-obj{Device,Port}.\n");
    bcm_flow_port_encap_t_init(&peinfo);
    peinfo.flow_port = *acc_flow_port;
    peinfo.egress_if = acc_egr_id;
    peinfo.valid_elements = BCM_FLOW_PORT_ENCAP_PORT_VALID |
        BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID;
    /*sdtag*/
    peinfo.valid_elements |= (BCM_FLOW_PORT_ENCAP_FLAGS_VALID |
            BCM_FLOW_PORT_ENCAP_TPID_VALID |
            BCM_FLOW_PORT_ENCAP_VLAN_VALID);
    peinfo.vlan = vxlan_acc->acc_vlan;
    peinfo.tpid = 0x8100;
    peinfo.flags = BCM_FLOW_ENCAP_FLAG_SERVICE_TAGGED |
        BCM_FLOW_ENCAP_FLAG_SERVICE_VLAN_ADD;
    /*end:sdtag*/
    rv = vxlan_flow_port_encap_set(unit, peinfo);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access encap: %s.\n", bcm_errmsg(rv));
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
    tiinfo.valid_elements = BCM_FLOW_TUNNEL_INIT_DIP_VALID |
        BCM_FLOW_TUNNEL_INIT_SIP_VALID |
        BCM_FLOW_TUNNEL_INIT_UDP_DPORT_VALID |
        BCM_FLOW_TUNNEL_INIT_UDP_SPORT_VALID |
        BCM_FLOW_TUNNEL_INIT_DSCP_VALID |
        BCM_FLOW_TUNNEL_INIT_TTL_VALID;
    tiinfo.flow_port = net_flow_port;
    tiinfo.type = bcmTunnelTypeVxlan;
    tiinfo.udp_src_port = vxlan_net->udp_sp;
    tiinfo.udp_dst_port = vxlan_cfg->udp_dp;
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

    if(BCM_GPORT_IS_TRUNK(vxlan_net->phy_Gport)) /*If gport is a trunk, execute all settings on each member port */
    {
        rv = vlan_create_add_trunk_ports(unit,vxlan_net->net_vlan);
        if (BCM_FAILURE(rv)) {
            printf("\nError in configuring vlan: %s.\n", bcm_errmsg(rv));
            return rv;
        }

        rv = vxlan_network_trunk_ports_config(unit);
        if (BCM_FAILURE(rv)) {
            printf("\nError in trunk network config: %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }
    else
    {
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

        rv = vxlan_network_port_config(unit, vxlan_net->phy_Gport);
        if (BCM_FAILURE(rv)) {
            printf("\nError in network config: %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    printf("Create network L3 interface\n");
    rv = create_l3_interface(unit, BCM_L3_ADD_TO_ARL, vxlan_net->net_local_mac, vxlan_net->net_vlan, &net_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create network L3 Egress object.\n");
    rv = vxlan_create_egr_obj(unit, BCM_L3_FLOW_ONLY, net_intf_id, vxlan_net->net_remote_mac,
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
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_VFI;
    einfo.vnid = vxlan_net->vxlan_vnid;
    einfo.vpn  = vxlan_cfg->vxlan_vpn;
    einfo.valid_elements = BCM_FLOW_ENCAP_VNID_VALID |
        BCM_FLOW_ENCAP_VPN_VALID;
    /*sdtag*/
    einfo.valid_elements |= ( BCM_FLOW_ENCAP_FLAGS_VALID|
            BCM_FLOW_ENCAP_TPID_VALID |
            BCM_FLOW_ENCAP_VLAN_VALID);
    einfo.flags = BCM_FLOW_ENCAP_FLAG_SERVICE_TAGGED |
        BCM_FLOW_ENCAP_FLAG_VLAN_DELETE;
    /*end:sdtag*/
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

    printf("Network: Set Egress-vlan-xlate key-type for network VP\n");
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, *net_flow_port,
                bcmVlanPortTranslateEgressKey, bcmVlanTranslateEgressKeyVpn));

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
/* RTAG7 configuration*/
    int
config_rtag7(int unit,rtag7_cfg_t *rtag7_cfg)
{
    int hash_control = 0;
    if(rtag7_cfg->block_select == 0) /*Execute A block related settings*/
    {
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed0, rtag7_cfg->seed_a));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, rtag7_cfg->pre_process_a));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0UnicastOffset,rtag7_cfg->block_n_offset_a));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0NonUnicastOffset,rtag7_cfg->block_n_offset_nonucast_a));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0Config,rtag7_cfg->hash_algo_a_0));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField0Config1,rtag7_cfg->hash_algo_a_1));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0,rtag7_cfg->tcp_ueq_l4_ports_a));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField0, rtag7_cfg->tcp_eq_l4_ports_a));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, rtag7_cfg->ipv4_a));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashL2Field0, rtag7_cfg->l2_a));

        BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashControl, &hash_control));
        hash_control  |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE;
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashControl, hash_control));
    }
    if(rtag7_cfg->block_select == 1) /*Execute B block related settings*/
    {
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashSeed1, rtag7_cfg->seed_b));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, rtag7_cfg->pre_process_b));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0UnicastOffset,rtag7_cfg->block_n_offset_b));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0NonUnicastOffset,rtag7_cfg->block_n_offset_nonucast_b));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1Config,rtag7_cfg->hash_algo_b_0));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashField1Config1,rtag7_cfg->hash_algo_b_1));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1,rtag7_cfg->tcp_ueq_l4_ports_b));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpPortsEqualField1, rtag7_cfg->tcp_eq_l4_ports_b));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, rtag7_cfg->ipv4_b));
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashL2Field1, rtag7_cfg->l2_b));

        BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, bcmSwitchHashControl, &hash_control));
        hash_control  |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE;
        BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashControl, hash_control));
    }
    return BCM_E_NONE;
}

/*Trunk configuration*/
    int
config_trunk(int unit,trunk_cfg_t *trunk_cfg)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t member_array[NO_OF_TRUNK_MEMBERS];
    int i=0;

    bcm_trunk_info_t_init(&trunk_info);
    trunk_info.psc = trunk_cfg->psc;
    trunk_info.dlf_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index= BCM_TRUNK_UNSPEC_INDEX;

    for(i=0; i<NO_OF_TRUNK_MEMBERS; i++)
    {
        bcm_trunk_member_t_init(&member_array[i]);
        BCM_GPORT_LOCAL_SET(member_array[i].gport,egress_port[i]);
    }

    BCM_IF_ERROR_RETURN(bcm_trunk_create(unit,trunk_cfg->flags,
                                                trunk_cfg->trunk_group_id));
    BCM_GPORT_TRUNK_SET(trunk_gport,trunk_cfg->trunk_group_id);
    BCM_IF_ERROR_RETURN(bcm_trunk_set(unit,trunk_cfg->trunk_group_id,&trunk_info,NO_OF_TRUNK_MEMBERS,member_array));

    return BCM_E_NONE;
}

/* vxlan Configuration function */
    bcm_error_t
config_vxlan_ipv4_vtep(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    /* RTAG7 configuration variables */
    rtag7_cfg_t rtag7_cfg = {
        0,              /*Block select. 0='A',1='B'*/

        /*Block A Settings*/
        0x11111111,     /*Seed*/
        0,              /*Pre-process Enable/Disable*/
        5,              /*Unicast offset*/
        5,              /*Non-unicast offset*/

        BCM_HASH_FIELD_CONFIG_CRC32LO, /*A0 hash function*/
        BCM_HASH_FIELD_CONFIG_CRC32LO, /*A1 hash function*/

        /*Field select for TCP/UDP with (L4 Src port neq Dst port)*/
        BCM_HASH_FIELD_IP4SRC_LO|BCM_HASH_FIELD_IP4SRC_HI|
        BCM_HASH_FIELD_IP4DST_LO|BCM_HASH_FIELD_IP4DST_HI|
        BCM_HASH_FIELD_DSTL4|BCM_HASH_FIELD_SRCL4,

        /*Field select for TCP/UDP with (L4 Src port eq Dst port)*/
        BCM_HASH_FIELD_IP4SRC_LO|BCM_HASH_FIELD_IP4SRC_HI|
        BCM_HASH_FIELD_IP4DST_LO|BCM_HASH_FIELD_IP4DST_HI|
        BCM_HASH_FIELD_DSTL4|BCM_HASH_FIELD_SRCL4,

        /*Field select for non-TCP/UDP IP packets*/
        BCM_HASH_FIELD_IP4SRC_LO|BCM_HASH_FIELD_IP4SRC_HI|
        BCM_HASH_FIELD_IP4DST_LO|BCM_HASH_FIELD_IP4DST_HI,

        /*Field select for L2 packets*/
        BCM_HASH_FIELD_MACSA_LO|BCM_HASH_FIELD_MACSA_MI|
        BCM_HASH_FIELD_MACSA_HI,

        /*Block B Settings*/
        /*Fill here if Block B settings are needed*/
    };

    /* Trunk Configuration variables */
    trunk_cfg_t trunk_cfg = {
        3,                      /*Trunk group ID*/
        BCM_TRUNK_FLAG_WITH_ID, /*flags*/
        BCM_TRUNK_PSC_PORTFLOW, /*Port selection criterion*/
    };

    /* VPN Configuration variables */
    vxlan_cfg_t vxlan_cfg = {
        8472,
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

    vxlan_net_cfg_t vxlan_net = {
        -1, /* port (Later filled with Trunk gport ID) */
        22, /* vlan */
        "00:00:00:00:22:22",/* local mac */
        "00:00:00:00:00:02",/* remote mac */
        0xC0A80101,  /*192.168.1.1 remote IP*/
        0x12345,     /* vnid */
        0x0a0a0a01,  /*10.10.10.1 local IP*/
        0xffff,
        0
    };


    /* flow API */
    bcm_flow_handle_t            flow_handle;
    bcm_gport_t   acc_flow_port;
    bcm_gport_t   net_flow_port;

    /* working variables */
    uint32 flags=0;

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


    rv = config_vxlan_access(unit, &vxlan_cfg, &vxlan_acc, &acc_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
        rv = config_trunk(unit, &trunk_cfg);
    if (BCM_FAILURE(rv)) {
        printf("\nError in trunk config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
        rv = config_rtag7(unit, &rtag7_cfg);
    if (BCM_FAILURE(rv)) {
        printf("\nError in RTAG7 config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
        vxlan_net.phy_Gport = trunk_gport; /*Give the gport ID of network port as Trunk gport value */
    rv = config_vxlan_network(unit, &vxlan_cfg, &vxlan_net, &net_flow_port, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_tunnel(unit, &vxlan_cfg, &vxlan_net, net_flow_port, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in tunnel config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Access-to-Network: Add payload-dest-mac Address => Network-flow-port (VP).\n");
    rv = add_to_l2_table(unit, "00:00:00:00:aa:aa", vxlan_cfg.vxlan_vpn, net_flow_port);
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
    bcm_flow_vpn_traverse(unit,vpn_traverse,NULL);
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
