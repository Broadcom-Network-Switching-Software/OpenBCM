/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : Vxlan Bridging , IPv4 network to network transit
 *            VxLAN GBP SID in incoming packet is used to distribute traffic in egress Trunk
 *            (NOTE: NtoN Transit is actually simple L3 Unicast flow)
 * 
 * Usage    : BCM.0> cint td4_hsdk_vxlan_gbp_ipv4_NtoN_uc_trunk.c
 *
 * config   : bcm56880_a0-generic-32x400_vxlan.config.yml
 *
 * Log file : td4_hsdk_vxlan_gbp_ipv4_NtoN_uc_trunk_log.txt
 *
 * Test Topology :
 *
 *                                +----------------------+  VLAN=23              Underlay+NextHop
 *                                |                      |          +---------+
 *                                |                      |          |
 * SMAC 00::02                    |                      +--------------------+
 *          +---+          VLAN=22|                      |00::33:33 |              +---+
 * Host(net)|   +-----------------+      Trident+4       |          +---------+    |   | 00::03
 *          +---+    ingress_port |                      |          |              +---+
 * DMAC 00::22:22                 |                      |          +---------+
 *                                |                      |          |
 *                                |                      |          +---------+
 *                                |                      |           Egress
 *                                |                      |           Trunk
 *                                |                      |
 *                                +----------------------+
 *
 *
 * Summary:
 * ========
 * Cint example to demonstrate VxLAN Bridging for Transit packets
 *             - Incoming VxLAN packets carry GBP SID in VxLAN header
 *             - GBP-ID is used to distribute traffic across egress trunk members
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the others as
 *        egress_port.
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
 *        network port's setting, tunnel setup, interface and next hop.
 *     b) Add an L3 overlay route that does routing of incoming VxLAN packet
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l2 show',  'trunk show', 'l3 egress show', 'l3 route show' and traversing the vpn and vp's
 *
 *     b) Transmit one known unicast packets. The contents of the packets
 *        are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see the routed packet on the egress port
 *       and 'show c' to check the Tx/Rx packet stats/counters.
 *       Also, 'show c s' shows the packet distribution among Trunk members.
 */
/*
NETWORK-TO-NETWORK:-
Transit-case: packets sent from (network) to (network)

Ingress Packet (network):
  ###[ Ethernet ]###
    dst= 00:00:00:00:22:22
    src= 00:00:00:00:00:02
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 22
     type= 0x800
  ###[ IP ]###
     version= 4
     proto= udp
     src= 10.10.10.1
     dst= 192.168.10.1
  ###[ UDP ]###
     sport= 57275
     dport= 4789
  ###[ VXLAN ]###
     flags= Group, Instance
     Group Policy ID = 0x0FAB, 0x0FAC, 0x0FAD, 0x0FAE, 0x0AAB <== GBP SID in 5 packets (packet's only changing contents)
     vni= 0x12345
     reserved2= 0x0
  ###[ Ethernet ]###
                 dst= 00:00:00:00:bb:bb
                 src= 00:00:00:00:aa:aa
                 type= 0x800
  ###[ 802.1Q ]###
                 vlan= 21
                 type= 0x800
  ###[ IP ]###
                 version= 4
                 proto= udp
                    src= 2.2.2.2
                    dst= 1.1.1.1
  ###[ UDP ]###

Ingress Hex Packets (network):
======
tx port=1 data=00000000222200000000000281000016080045000082000040003F115CB70A0A0A01C0A80A01DFBB12B5006E790B88000FAB0123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374
tx port=1 data=00000000222200000000000281000016080045000082000040003F115CB70A0A0A01C0A80A01DFBB12B5006E790B88000FAC0123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374
tx port=1 data=00000000222200000000000281000016080045000082000040003F115CB70A0A0A01C0A80A01DFBB12B5006E790B88000FAD0123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374
tx port=1 data=00000000222200000000000281000016080045000082000040003F115CB70A0A0A01C0A80A01DFBB12B5006E790B88000FAE0123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374
tx port=1 data=00000000222200000000000281000016080045000082000040003F115CB70A0A0A01C0A80A01DFBB12B5006E790B88000AAB0123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

Egress Packet (network) :

Header Info

  ###[ Ethernet ]###
    dst= 00:00:00:00:00:03
    src= 00:00:00:00:33:33
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 23
     type= 0x800
  ###[ IP ]###
     version= 4
     proto= udp
     src= 10.10.10.1
     dst= 192.168.10.1
  ###[ UDP ]###
     sport= 57275
     dport= 4789
  ###[ VXLAN ]###
     flags= Group, Instance
     Group Policy ID = 0x0FAB, 0x0FAC, 0x0FAD, 0x0FAE, 0x0AAB <== GBP SID in 5 outgoing packets 
     vni= 0x12345
     reserved2= 0x0
  ###[ Ethernet ]###
                 dst= 00:00:00:00:bb:bb
                 src= 00:00:00:00:aa:aa
                 type= 0x800
  ###[ 802.1Q ]###
     vlan= 21
     type= 0x800
  ###[ IP ]###
        version= 4
        proto= udp
        src= 2.2.2.2
        dst= 1.1.1.1
  ###[ UDP ]###
         sport= 53
         dport= 53

Packet Dump

Egress packets:
data[0000]: {000000000003} {000000003333} 8100 0017
data[0010]: 0800 4500 0082 0000 4000 3e11 5db7 0a0a
data[0020]: 0a01 c0a8 0a01 dfbb 12b5 006e 790b 8800
data[0030]: 0fab 0123 4500 0000 0000 bbbb 0000 0000
data[0040]: aaaa 8100 0015 0800 4500 0050 0001 0000
data[0050]: 4011 7497 0202 0202 0101 0101 0035 0035
data[0060]: 003c 32f6 7465 7374 7465 7374 7465 7374
data[0070]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0080]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0090]: 7465 7374 7465 7374


data[0000]: {000000000003} {000000003333} 8100 0017
data[0010]: 0800 4500 0082 0000 4000 3e11 5db7 0a0a
data[0020]: 0a01 c0a8 0a01 dfbb 12b5 006e 790b 8800
data[0030]: 0fac 0123 4500 0000 0000 bbbb 0000 0000
data[0040]: aaaa 8100 0015 0800 4500 0050 0001 0000
data[0050]: 4011 7497 0202 0202 0101 0101 0035 0035
data[0060]: 003c 32f6 7465 7374 7465 7374 7465 7374
data[0070]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0080]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0090]: 7465 7374 7465 7374

data[0000]: {000000000003} {000000003333} 8100 0017
data[0010]: 0800 4500 0082 0000 4000 3e11 5db7 0a0a
data[0020]: 0a01 c0a8 0a01 dfbb 12b5 006e 790b 8800
data[0030]: 0fad 0123 4500 0000 0000 bbbb 0000 0000
data[0040]: aaaa 8100 0015 0800 4500 0050 0001 0000
data[0050]: 4011 7497 0202 0202 0101 0101 0035 0035
data[0060]: 003c 32f6 7465 7374 7465 7374 7465 7374
data[0070]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0080]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0090]: 7465 7374 7465 7374

data[0000]: {000000000003} {000000003333} 8100 0017
data[0010]: 0800 4500 0082 0000 4000 3e11 5db7 0a0a
data[0020]: 0a01 c0a8 0a01 dfbb 12b5 006e 790b 8800
data[0030]: 0fae 0123 4500 0000 0000 bbbb 0000 0000
data[0040]: aaaa 8100 0015 0800 4500 0050 0001 0000
data[0050]: 4011 7497 0202 0202 0101 0101 0035 0035
data[0060]: 003c 32f6 7465 7374 7465 7374 7465 7374
data[0070]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0080]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0090]: 7465 7374 7465 7374

data[0000]: {000000000003} {000000003333} 8100 0017
data[0010]: 0800 4500 0082 0000 4000 3e11 5db7 0a0a
data[0020]: 0a01 c0a8 0a01 dfbb 12b5 006e 790b 8800
data[0030]: 0aab 0123 4500 0000 0000 bbbb 0000 0000
data[0040]: aaaa 8100 0015 0800 4500 0050 0001 0000
data[0050]: 4011 7497 0202 0202 0101 0101 0035 0035
data[0060]: 003c 32f6 7465 7374 7465 7374 7465 7374
data[0070]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0080]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0090]: 7465 7374 7465 7374

*/

/* Reset C interpreter*/
cint_reset();
int NO_OF_TRUNK_MEMBERS = 5;
bcm_error_t show_vxlan_vtep_config(int unit);

bcm_port_t ingress_port;
bcm_port_t egress_port[NO_OF_TRUNK_MEMBERS];
bcm_gport_t ingress_phy_gport;
bcm_gport_t egress_phy_gport[NO_OF_TRUNK_MEMBERS];

/* struct for Access Port Params for creating access VP*/
struct vxlan_acc_cfg_t {
    bcm_gport_t   phy_Gport;/* Input : gport of the port*/
    bcm_vlan_t    acc_vlan; /* Input */
    bcm_mac_t     acc_local_mac;
    bcm_mac_t     acc_remote_mac;
    bcm_gport_t   acc_gport; /* Output:Access VP gport */
};

/* struct for Network Port params */
struct vxlan_net_cfg_t {
    bcm_port_t    phy_Gport;/* Input : gport of the port*/
    bcm_vlan_t    net_vlan;
    bcm_mac_t     net_local_mac;
    bcm_mac_t     net_remote_mac;/*For carrying DMAC*/
    bcm_ip_t      remote_ip;
    uint32        vxlan_vnid;
    bcm_ip_t      local_ip;
    bcm_vrf_t     vrf;
    bcm_gport_t   nw_gport; /* Output:Nw VP gport */
};

struct vxlan_cfg_t {
    uint16    udp_dp; /*VxLAN UDP dest port*/
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

    for(i=0; i<NO_OF_TRUNK_MEMBERS; i++)
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

    for(i=0; i<NO_OF_TRUNK_MEMBERS; i++)
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
    int port_list[NO_OF_TRUNK_MEMBERS+1]; /*Container for Trunk member egress ports and ingress port */
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
        printf("egress_port_setup() failed");
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
    snprintf(pkt_data[0],512,"%s","00000000222200000000000281000016080045000082000040003F115CB70A0A0A01C0A80A01DFBB12B5006E790B88000FAB0123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374");
    snprintf(pkt_data[1],512,"%s","00000000222200000000000281000016080045000082000040003F115CB70A0A0A01C0A80A01DFBB12B5006E790B88000FAC0123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374");
    snprintf(pkt_data[2],512,"%s","00000000222200000000000281000016080045000082000040003F115CB70A0A0A01C0A80A01DFBB12B5006E790B88000FAD0123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374");
    snprintf(pkt_data[3],512,"%s","00000000222200000000000281000016080045000082000040003F115CB70A0A0A01C0A80A01DFBB12B5006E790B88000FAE0123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374");
    snprintf(pkt_data[4],512,"%s","00000000222200000000000281000016080045000082000040003F115CB70A0A0A01C0A80A01DFBB12B5006E790B88000AAB0123450000000000BBBB00000000AAAA810000150800450000500001000040117497020202020101010100350035003C32F674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374");
    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
    bshell(unit, "trunk show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "l3 host show");
    bshell(unit, "clear counters");

    for(i=0; i<sizeof(pkt_data)/512; i++)
    {
        printf("\nSending known unicast packet No.[%d] to ingress_port :%d\n", i+1,ingress_port);
        snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 2", ingress_port,pkt_data[i]);
        bshell(unit, str);
        bshell(unit, "show counters");
    }

    /* Dump the Configured VPN & VP's*/
    show_vxlan_vtep_config(unit);

    printf("\n----------------------------------------------\n");
    printf("Distribution of %d Packets on Trunk member ports:",sizeof(pkt_data)/512);
    printf("\n----------------------------------------------\n");
    bshell(unit, "sleep quiet 1;show counters Changed Same NZ");

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

/* Create vlan and add Trunk member ports to vlan */
    int
vlan_create_add_trunk_ports(int unit, int vid)
{
    bcm_pbmp_t pbmp, upbmp;
    int i=0;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    for(i=0; i<NO_OF_TRUNK_MEMBERS; i++)
    {
        BCM_PBMP_PORT_ADD(pbmp, egress_port[i]);
    }
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* vxlan system configuration */
    int
vxlan_system_config(int unit, vxlan_cfg_t *vxlan_cfg)
{
    printf("Configure Vxlan Global Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_vlan_control_set(unit, bcmVlanTranslate, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchVxlanUdpDestPortSet, 4789));
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
    if(BCM_GPORT_IS_TRUNK(gport)){
        l3_egress.trunk = gport;
        l3_egress.flags |= BCM_L3_TGID;
    }
    else
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
    if (rv != BCM_E_EXISTS && BCM_FAILURE(rv)) {
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

/* L3 route entry for Routing */
    int
add_to_l3_route_table(int unit, bcm_ip_t ip, bcm_vrf_t vrf, bcm_if_t egr_if, int is_route)
{
    if (is_route)     {
        bcm_l3_route_t route;
        bcm_ip_t mask = 0xFFFFFF00;

        bcm_l3_route_t_init(&route);
        route.l3a_subnet = ip;
        route.l3a_ip_mask = mask;
        route.l3a_vrf = vrf;
        route.l3a_intf = egr_if;
        
        return bcm_l3_route_add(unit, &route);
    }
    else {
        bcm_l3_host_t host;

        bcm_l3_host_t_init(&host);
        host.l3a_intf = egr_if;
        host.l3a_ip_addr = ip;
        host.l3a_vrf = vrf;
        return bcm_l3_host_add(unit, &host);
    }
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
    tiinfo.valid_elements |= BCM_FLOW_TUNNEL_INIT_UDP_DPORT_VALID;
    tiinfo.udp_dst_port = vxlan_cfg->udp_dp;
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

/*Configure Routing */
bcm_error_t
config_vxlan_overlay_network_route(int unit, vxlan_net_cfg_t *vxlan_net)
{
    int rv = 0;
    bcm_if_t      net_intf_id_ol;
    bcm_if_t      net_egr_id_ol;

    printf("Create network L3 interface (OL).\n");
    rv = create_l3_interface(unit, 0, 0, vxlan_net->net_local_mac, vxlan_net->net_vlan, &net_intf_id_ol);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface (OL): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create network L3 Egress object (OL).\n");
    rv = vxlan_create_egr_obj(unit, BCM_L3_FLOW_ONLY, net_intf_id_ol, vxlan_net->net_remote_mac,
                    vxlan_net->phy_Gport, vxlan_net->net_vlan, &net_egr_id_ol);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring egress obj (OL): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Network to Network : Add L3 Route entry\n");
    rv = add_to_l3_route_table(unit, vxlan_net->remote_ip, vxlan_net->vrf, net_egr_id_ol, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring L3 Route: %s.\n", bcm_errmsg(rv));
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
    int i = 0;

    print vxlan_net->phy_Gport;

    if(BCM_GPORT_IS_TRUNK(vxlan_net->phy_Gport)) {/*If gport is a trunk, execute all settings on each member port */
        rv = vlan_create_add_trunk_ports(unit, vxlan_net->net_vlan);
        if (BCM_FAILURE(rv)) {
           printf("\nError in configuring vlan: %s.\n", bcm_errmsg(rv));
           return rv;
        }

        for(i = 0; i < NO_OF_TRUNK_MEMBERS; i++) {
            /* Set port default vlan id */
            BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port[i], vxlan_net->net_vlan));
            /* Set default inner tpid per port */
            BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit, egress_port[i], 0x9100));
            /* Set default outer tpid per port */
            BCM_IF_ERROR_RETURN(bcm_port_tpid_set(unit, egress_port[i], 0x8100));
            /* Set port learning */
            BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port[i], BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
            /* Port must be added to VPN to pass membership check */
            bcm_gport_t gport_network_n = BCM_GPORT_INVALID;
            BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port[i], &gport_network_n));
            BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vxlan_cfg->vxlan_vpn, gport_network_n, 0));

            rv = vxlan_network_port_config(unit, egress_phy_gport[i]);
            if (BCM_FAILURE(rv)) {
                printf("\nError in network config: %s.\n", bcm_errmsg(rv));
                return rv;
            }
        }
    }
    else {
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
    }

    printf("Create network L3 interface\n");
    rv = create_l3_interface(unit, BCM_L3_ADD_TO_ARL, BCM_L3_INTF_UNDERLAY, vxlan_net->net_local_mac, vxlan_net->net_vlan, &net_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 ingress interface. */
    
    bcm_if_t ingress_if;
    bcm_l3_ingress_t l3_ingress;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.vrf = vxlan_net->vrf;
    rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);

    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring ingress l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    /* Config vlan_id to l3_iif mapping. */
    bcm_vlan_control_vlan_t vlan_ctrl;
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    rv = bcm_vlan_control_vlan_selective_set(unit, vxlan_net->net_vlan, BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK, &vlan_ctrl);
    if (BCM_FAILURE(rv)) {
        printf("\nError in associating ingress l3 interface to VLAN: %s.\n", bcm_errmsg(rv));
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
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_VFI;
    einfo.vnid = vxlan_net->vxlan_vnid;
    einfo.vpn  = vxlan_cfg->vxlan_vpn;
    einfo.valid_elements = BCM_FLOW_ENCAP_VNID_VALID | BCM_FLOW_ENCAP_VPN_VALID;
    rv = bcm_flow_encap_add(unit, &einfo, 0, NULL);
    if ((BCM_FAILURE(rv)) && (rv!=BCM_E_EXISTS)) {
        printf("\nNetwork: bcm_flow_encap_add() failed: %s.\n", bcm_errmsg(rv));
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
    if ((BCM_FAILURE(rv)) && (rv!=BCM_E_EXISTS)) {
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

    printf("Network: Add flow match (match: SIP => network SVP)\n");
    bcm_flow_match_config_t_init(&match_info);
    match_info.flow_handle = flow_handle;
    match_info.criteria = BCM_FLOW_MATCH_CRITERIA_SIP;
    match_info.sip = vxlan_net->remote_ip;
    match_info.flow_port = *net_flow_port;
    match_info.valid_elements = BCM_FLOW_MATCH_SIP_VALID |
                            BCM_FLOW_MATCH_FLOW_PORT_VALID;
    rv = vxlan_flow_match_add(unit, match_info);
    if (rv!=BCM_E_EXISTS && BCM_FAILURE(rv)) {
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


    /**** UDF CREATION ****/
    /* VxLAN GBP ID lies in RESERVED-1 field of VxLAN header
       It's 2 bytes off from Start of VxLAN header.
       And it's composed of 2 bytes (16-bits)
       As there's no Flex Digest action to use this field directly
       UDF is used to match and contain the GBP-ID
       And that UDF chunk is used in Flex Digest*/
    int udf_id;
    const int offset = 2; /*2 bytes from begining of VxLAN header*/

    bcm_udf_multi_chunk_info_t chunk_info;
    bcm_udf_multi_chunk_info_t_init(&chunk_info);
    chunk_info.offset = offset * 8;
    chunk_info.abstract_pkt_format = bcmUdfAbstractPktFormatUdpVxlan;/*Begining of VxLAN header*/
    BCM_UDF_CBMP_CHUNK_ADD(chunk_info.two_byte_chunk_bmap, 2);/*GBP-ID is 2-byte (16 bit) field*/

    BCM_IF_ERROR_RETURN( bcm_udf_multi_chunk_create(unit, NULL, &chunk_info, &udf_id));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_init(unit));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_seed_profile_create(unit, 0, &seed_profile_id));
    value = 0xbabaface;
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id,
                                              bcmFlexDigestNormSeedControlBinASeed,
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
                                         bcmFlexDigestHashProfileControlXorSaltBinA,
                                         1));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlBinA0FunctionSelection,
                                         BCM_FLEXDIGEST_HASH_FUNCTION_CRC32LO));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_set(unit, hash_profile_id,
                                         bcmFlexDigestHashProfileControlBinA1FunctionSelection,
                                         BCM_FLEXDIGEST_HASH_FUNCTION_CRC32HI));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_salt_set(unit, bcmFlexDigestHashBinA, 16, salt_a));

    BCM_FLEXDIGEST_QSET_INIT(qset);
    BCM_FLEXDIGEST_QSET_ADD(qset, bcmFlexDigestQualifyMatchId);
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

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                        bcmFlexDigestActionExtractBinSetACmds0,
                        BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_2, 0xffff));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_entry_install(unit, entry));

    /* Trunk FlexDigest */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelTrunkUc, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelTrunkNonUnicast, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, ingress_port, bcmSwitchTrunkUnicastHashOffset, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, ingress_port, bcmSwitchTrunkNonUnicastHashOffset, 0));

    return BCM_E_NONE;
}

/* Configure Trunk */
int
config_trunk(int unit, int tid)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t member_array[NO_OF_TRUNK_MEMBERS];
    int i=0;

    bcm_trunk_info_t_init(&trunk_info);
    trunk_info.flags      = BCM_TRUNK_FLAG_FAILOVER_ALL_LOCAL;
    trunk_info.psc        = BCM_TRUNK_PSC_PORTFLOW;
    trunk_info.dlf_index  = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index   = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;

    for(i = 0; i < NO_OF_TRUNK_MEMBERS; i++)
    {
        bcm_trunk_member_t_init(&member_array[i]);
        BCM_GPORT_LOCAL_SET(member_array[i].gport, egress_port[i]);
    }

    BCM_IF_ERROR_RETURN(bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID,
                                                &tid));
    BCM_IF_ERROR_RETURN(bcm_trunk_set(unit, tid,
                                &trunk_info, NO_OF_TRUNK_MEMBERS, member_array));

    return BCM_E_NONE;
}


/* vxlan Configuration function */
    bcm_error_t
config_vxlan_ipv4_vtep(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* VPN Configuration variables */
    vxlan_cfg_t vxlan_cfg = {
        4789,
        0x7010,
        0,
        0
    };

    vxlan_net_cfg_t vxlan_net = {
        ingress_phy_gport, /* port */
        22, /* vlan */
        "00:00:00:00:22:22",/* local mac */
        "00:00:00:00:00:02",/* remote mac */
        0xC0A80101,  /*192.168.1.1 remote IP*/
        0x12345,     /* vnid */
        0x0a0a0a01,  /*10.10.10.1 local IP*/
        1,
        0
    };

    vxlan_net_cfg_t vxlan_net_2 = {
        egress_phy_gport[0], /* port(Will be overwritten by trunk gport */
        23, /* vlan */
        "00:00:00:00:33:33",/* local mac */
        "00:00:00:00:00:03",/* remote mac */
        0xC0A80A01,  /*192.168.10.1 remote IP*/
        0x12345,     /* vnid */
        0x0b0b0b01,  /*11.11.11.1 local IP*/
        1,
        0
    };

    bcm_trunk_t tid = 1; /*Trunk ID*/
    
    /* flow API */
    bcm_flow_handle_t            flow_handle;
    bcm_gport_t   acc_flow_port;
    bcm_gport_t   net_flow_port_2;
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

    rv = config_FlexDigest(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Flex Digest Configuration: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_trunk(unit, tid);
    if (BCM_FAILURE(rv)) {
        printf("\nError in trunk create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set VxLAN network port 2 to have egress as trunk */
    BCM_GPORT_TRUNK_SET(vxlan_net_2.phy_Gport, tid);
    
    rv = config_vxlan_network(unit, &vxlan_cfg, &vxlan_net_2, &net_flow_port_2, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_network(unit, &vxlan_cfg, &vxlan_net, &net_flow_port, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_tunnel(unit, &vxlan_cfg, &vxlan_net_2, net_flow_port_2, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in tunnel config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_tunnel(unit, &vxlan_cfg, &vxlan_net, net_flow_port, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in tunnel config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_overlay_network_route(unit, vxlan_net_2);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network route: %s.\n", bcm_errmsg(rv));
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
