/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : Vxlan Bridging, IPv4 network to access tunnel termination flow
 *            for uuc/bc and known-multicast.
 *
 * Usage    : BCM.0> cint ipv4_vxlan_net_acc_mc.c
 *
 * config   : config_fb6_flex.bcm
 *
 * Log file : ipv4_vxlan_net_acc_mc_log.txt
 *
 * Test Topology :
 *                                  +----------------------+ known_mc <-
 *                                  |                      |   VLAN-22        Underlay-NextHop
 *                                  |                      | ingress_port       +---+
 *   SMAC 00::bb:bb                 |                      +--------------------+   |
 *            +---+          VLAN-21|                      |01:00:5e:02:02:02   +---+
 *   Host(acc)|   +-----------------+     Firebolt-6       |                    00::02
 *            +---+     egress_port |                      |
 *                                  |                      |   VLAN-23       Underlay-NextHop
 *                                  |                      | ingress_port       +---+
 *                                  |                      +--------------------+   |
 *                                  |                      |01:00:5e:02:02:02   +---+
 *                                  |                      |                    00::02
 *                                  |                      | uuc/bc <-
 *                                  +----------------------+
 *
 * Summary:
 * ========
 * Cint example to demonstrate VxLAN tunnel termination for uuc/bc and known multicast traffic -
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. Out of these
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
 *     a) Configure a VxLAN Bridging configuration with two network ports.
 *        This does the necessary configurations of vxlan global settings, vlan,
 *        access and network port setting, tunnel setup, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l2 show',traversing the vpn and vp's
 *
 *     b) Transmit the known unicast packet. The contents of the packet
 *        are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see the tunnel terminated packet on the access port and
 *       'show c' to check the Tx/Rx packet stats/counters.
 */
/*
Packet:
Known Multicast case (decap, Network=>Access)
=======
Input (network port):
   ###[ Ethernet ]###
     dst= 01:00:5e:02:02:02
     src= 00:00:00:00:00:02
     type= 0x8100
   ###[ 802.1Q ]###
     vlan= 22
     type= 0x800
   ###[ IP ]###
        version= 4
        proto= udp
        src= 10.10.10.1
        dst= 225.2.2.2
   ###[ UDP ]###
           sport= 55555
           dport= 8472
   ###[ VXLAN ]###
              flags= Instance
              reserved1= 0
              vni= 0x12345
              reserved2= 0x0
   ###[ Ethernet ]###
                 dst= 01:00:5e:01:01:01
                 src= 00:00:00:00:bb:bb
                 type= 0x800
   ###[ IP ]###
                    version= 4
                    proto= udp
                    src= 1.1.1.1
                    dst= 225.1.1.1
   ###[ UDP ]###

Input Hex (ce1: Network):
Known multicast case:
0x01005E02020200000000000281000016080045000086000000003f1184580a0a0a01e1020202ffff211800720000000000000123450001005e01010100000000BBBB81000015080045000050000040004011569901010101e101010100350035003c54f7746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573745fe8d7f4

Output (Access):
[bcmPW.0]Packet[2]: data[0000]: {01005e010101} {00000000bbbb} 8100 0015
[bcmPW.0]Packet[2]: data[0010]: 0800 4500 0050 0000 4000 4011 5699 0101
[bcmPW.0]Packet[2]: data[0020]: 0101 e101 0101 0035 0035 003c 54f7 7465
[bcmPW.0]Packet[2]: data[0030]: 7374 7465 7374 7465 7374 7465 7374 7465
[bcmPW.0]Packet[2]: data[0040]: 7374 7465 7374 7465 7374 7465 7374 7465
[bcmPW.0]Packet[2]: data[0050]: 7374 7465 7374 7465 7374 7465 7374 7465
[bcmPW.0]Packet[2]: data[0060]: 7374 5fe8 d7f4 1a92 8bb3

Input Hex (ce1: Network):
Broadcast case:
0x01005e02020200000000000281000017080045000086000000003f1185580a0a0a02e1020101ffff2118007200000000000001234500ffffffffffff00000000000181000015080045000050000040004011569901010101e101010100350035003c54f77465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737443e074ac

Output(Access):
[bcmPW.0]Packet[4]: data[0000]: {ffffffffffff} {000000000001} 8100 0015
[bcmPW.0]Packet[4]: data[0010]: 0800 4500 0050 0000 4000 4011 5699 0101
[bcmPW.0]Packet[4]: data[0020]: 0101 e101 0101 0035 0035 003c 54f7 7465
[bcmPW.0]Packet[4]: data[0030]: 7374 7465 7374 7465 7374 7465 7374 7465
[bcmPW.0]Packet[4]: data[0040]: 7374 7465 7374 7465 7374 7465 7374 7465
[bcmPW.0]Packet[4]: data[0050]: 7374 7465 7374 7465 7374 7465 7374 7465
[bcmPW.0]Packet[4]: data[0060]: 7374 43e0 74ac e369 283c
*/

/* Reset C interpreter*/
cint_reset();

bcm_error_t show_vxlan_vtep_config(int unit);

bcm_port_t egress_port;
bcm_port_t ingress_port_1;
bcm_port_t ingress_port_2;
bcm_gport_t egress_phy_gport;
bcm_gport_t ingress_phy_gport_1;
bcm_gport_t ingress_phy_gport_2;

/* struct for Access Port Params for creating access VP*/
struct vxlan_acc_cfg_t {
    bcm_gport_t   phy_Gport;/* Input : gport of the port/trunk*/
    bcm_vlan_t    acc_vlan; /* Input */
    bcm_mac_t     acc_local_mac;
    bcm_mac_t     acc_remote_mac;
    bcm_gport_t   acc_gport; /* Output:Access VP gport */
    bcm_multicast_t mc_group;
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
    bcm_multicast_t mc_group;
};

struct vxlan_cfg_t {
    uint16 udp_dp;
    bcm_vpn_t vxlan_vpn;
    uint32    vxlan_vnid;/*use vnid for vp sharing*/
    bcm_multicast_t bc_group;
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
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_qset_t qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_port_t     port;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

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
    int port_list[3];
    bcm_pbmp_t pbmp;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port_2 = port_list[2];
    ingress_port_1 = port_list[1];
    egress_port = port_list[0];

    if (BCM_E_NONE != bcm_port_gport_get(unit, egress_port, &(egress_phy_gport))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE !=bcm_port_gport_get(unit, ingress_port_1, &(ingress_phy_gport_1))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", egress_port_1);
        return -1;
    }

    if (BCM_E_NONE !=bcm_port_gport_get(unit, ingress_port_2, &(ingress_phy_gport_2))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", egress_port_2);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port_1);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port_1);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port_2);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for port %d\n", ingress_port_1);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

/*
 * Verification:
 *   Out Packet:
 *   ----------
 *  01005E010101000000001111
 *  810000150800450000500000
 *  40004011569901010101E101
 *  010100350035003C54F77465
 *  737474657374746573747465
 *  737474657374746573747465
 *  737474657374746573747465
 *  7374746573747465737474657374
 *
 *   IN Packet:
 *   -----------
 *  01005e02 02020000 00002222 81000016
 *  08004525 00820000 00003f11 84370a0a
 *  0a01e102 0202ffff 2118006e 00000000
 *  00000123 45000100 5e010101 00000000
 *  11118100 00150800 45000050 00004000
 *  40115699 01010101 e1010101 00350035
 *  003c54f7 74657374 74657374 74657374
 *  74657374 74657374 74657374 74657374
 *  74657374 74657374 74657374 74657374
 *  74657374 ae10d5d7
 */
void
verify(int unit)
{
    char str[1184];

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
    bshell(unit, "clear c");
    printf("\nSending known multicast packet to ingress_port network side:%d\n", ingress_port_1);
    snprintf(str, 1184, "tx 1 pbm=%d data=0x01005E02020200000000000281000016080045000086000000003f1184580a0a0a01e1020202ffff211800720000000000000123450001005e01010100000000BBBB81000015080045000050000040004011569901010101e101010100350035003c54f7746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573745fe8d7f4; sleep quiet 1", ingress_port_1);
    bshell(unit, str);
    bshell(unit, "sleep 1; show c");

    printf("\nSending broadcast packet to ingress_port network side:%d\n", ingress_port_2);
    snprintf(str, 1184, "tx 1 pbm=%d data=0x01005e02020200000000000281000017080045000086000000003f1185580a0a0a02e1020101ffff2118007200000000000001234500ffffffffffff00000000000181000015080045000050000040004011569901010101e101010100350035003c54f77465737474657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737443e074ac; sleep quiet 1", ingress_port_2);
    bshell(unit, str);
    bshell(unit, "sleep 1; show c");

  /* Dump the Configured VPN & VP's*/
    show_vxlan_vtep_config(unit);

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
create_acc_egr_obj(int unit, int l3_if, vxlan_acc_cfg_t *vxlan_acc, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;
    bcm_if_t vp;
    bcm_l3_egress_t_init(&l3_egress);
    vp = BCM_GPORT_FLOW_PORT_ID_GET(vxlan_acc->acc_gport);
    l3_egress.encap_id = vp;
    l3_egress.intf  = l3_if;
    sal_memcpy(l3_egress.mac_addr, vxlan_acc->acc_local_mac, sizeof(vxlan_acc->acc_local_mac));
    l3_egress.vlan = vxlan_acc->acc_vlan;
    l3_egress.port = vxlan_acc->phy_Gport;

    return bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
}

int
vxlan_create_mc_egr_obj(int unit,int l3_if,vxlan_net_cfg_t *vxlan_net,int flags,int flags2,bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;
    bcm_l3_egress_t_init(&l3_egress);
    bcm_if_t vp;
    vp = BCM_GPORT_FLOW_PORT_ID_GET(vxlan_net->nw_gport);
    l3_egress.encap_id = vp;
    l3_egress.flags = flags;
    l3_egress.flags2 = flags2;
    l3_egress.intf  = l3_if;
    sal_memcpy(l3_egress.mac_addr, vxlan_net->net_remote_mac, sizeof(vxlan_net->net_remote_mac));
    l3_egress.vlan = vxlan_net->net_vlan;
    l3_egress.port = vxlan_net->phy_Gport;
    return bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
}

/* Create the VPN for vxlan */
int
create_vxlan_vpn(int unit, vxlan_cfg_t *vxlan_cfg)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_multicast_t bc_group=0;

    printf("Create Multicast group for UUC/MC/BC.\n");
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_FLOW, &bc_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in multicast create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create Flow VPN.\n");
    bcm_flow_vpn_config_t vpn_info;
    bcm_flow_vpn_config_t_init (&vpn_info);
    vpn_info.flags = BCM_FLOW_VPN_ELAN | BCM_FLOW_VPN_WITH_ID;
    vpn_info.broadcast_group         = bc_group;
    vpn_info.unknown_unicast_group   = bc_group;
    vpn_info.unknown_multicast_group = bc_group;
    BCM_IF_ERROR_RETURN(bcm_flow_vpn_create(unit, vxlan_cfg->vxlan_vpn, &vpn_info));

    vxlan_cfg->bc_group=bc_group;

    return BCM_E_NONE;
}

/* Create ipmc group */
int
create_bc_group(int unit,bcm_multicast_t bc_group,bcm_gport_t gport,bcm_if_t egr_id)
{
    bcm_if_t encap_id;

    BCM_IF_ERROR_RETURN(bcm_multicast_egress_object_encap_get(unit,bc_group,egr_id,&encap_id));
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit,bc_group,gport, encap_id));

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
        printf("\nAccess: bcm_flow_port_encap_set() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Static L2MC entry for forwarding */
int
add_l2_mc_entry(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, bcm_multicast_t mc_group)
{
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = mc_group;
    return bcm_l2_addr_add(unit, &l2_addr);
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
config_vxlan_access(int unit,vxlan_cfg_t *vxlan_cfg,vxlan_acc_cfg_t *vxlan_acc,bcm_multicast_t mc_group,bcm_gport_t *acc_flow_port)
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

    printf("Create Access Flow Port (VP)\n");
    rv = create_vxlan_vp(unit, vxlan_cfg->vxlan_vpn, 0, acc_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access vp: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    vxlan_acc->acc_gport = *acc_flow_port;

    printf("Create access L3 interface\n");
    rv = create_l3_interface(unit, 0, vxlan_acc->acc_local_mac, vxlan_acc->acc_vlan, &acc_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create access L3 Egress object.\n");
    rv = create_acc_egr_obj(unit, acc_intf_id, vxlan_acc, &acc_egr_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring egress obj: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_bc_group(unit,mc_group,vxlan_acc->phy_Gport,acc_egr_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in adding egress obj: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_bc_group(unit,vxlan_cfg->bc_group,vxlan_acc->phy_Gport,acc_egr_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in adding egress obj: %s.\n", bcm_errmsg(rv));
        return rv;
    }

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
    ttinfo.dip = vxlan_net->remote_ip;
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
config_vxlan_network(int unit,vxlan_cfg_t *vxlan_cfg,vxlan_net_cfg_t *vxlan_net,bcm_gport_t *net_flow_port,bcm_multicast_t mc_group,int known_mcast, bcm_flow_handle_t flow_handle)
{
    /* Network Configuartion variables */
    bcm_error_t rv = BCM_E_NONE;
    bcm_if_t      net_intf_id;
    bcm_if_t      net_egr_id;
    bcm_flow_port_encap_t        peinfo;
    bcm_flow_match_config_t      match_info;
    bcm_flow_encap_config_t      einfo;
    bcm_port_t                  phy_port;

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

    printf("Create Network Flow Port.\n");
    rv = create_vxlan_vp(unit, vxlan_cfg->vxlan_vpn, BCM_FLOW_PORT_NETWORK, net_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network vp: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    vxlan_net->nw_gport=*net_flow_port;

    printf("Create network L3 interface\n");
    rv = create_l3_interface(unit,0,vxlan_net->net_local_mac,vxlan_net->net_vlan,&net_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    if(known_mcast == 1) {
        printf("Create network L3 Egress object.\n");
        rv = vxlan_create_mc_egr_obj(unit,net_intf_id,vxlan_net,BCM_L3_IPMC,BCM_L3_FLOW_ONLY,&net_egr_id);
        if (BCM_FAILURE(rv)) {
            printf("\nError in configuring egress obj: %s.\n", bcm_errmsg(rv));
            return rv;
        }

        rv = create_bc_group(unit,mc_group,vxlan_net->phy_Gport,net_egr_id);
        if (BCM_FAILURE(rv)) {
            printf("\nError in adding egress obj: %s.\n", bcm_errmsg(rv));
            return rv;
        }

        printf("Network: encap add, VFI-->VNID\n");
        bcm_flow_encap_config_t_init(&einfo);
        einfo.flow_handle = flow_handle;
        einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_VFI;
        einfo.vnid = vxlan_net->vxlan_vnid;
        einfo.vpn  = vxlan_cfg->vxlan_vpn;
        einfo.valid_elements = BCM_FLOW_ENCAP_VNID_VALID |
            BCM_FLOW_ENCAP_VPN_VALID;
        rv = bcm_flow_encap_add(unit, &einfo, 0, NULL);
        if (BCM_FAILURE(rv)) {
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
        if (BCM_FAILURE(rv)) {
            printf("\nError in configuring network flow match: %s.\n", bcm_errmsg(rv));
            return rv;
        }

        printf("Network: Add flow match (match: SIP => network SVP)\n");
        bcm_flow_match_config_t_init(&match_info);
        match_info.flow_handle = flow_handle;
        match_info.criteria = BCM_FLOW_MATCH_CRITERIA_SIP;
        match_info.sip = vxlan_net->local_ip;
        match_info.flow_port = *net_flow_port;
        match_info.valid_elements = BCM_FLOW_MATCH_SIP_VALID |
                                    BCM_FLOW_MATCH_FLOW_PORT_VALID;
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
   } else {
        printf("Create network L3 Egress object.\n");
        rv = vxlan_create_mc_egr_obj(unit,net_intf_id,vxlan_net,
                                     BCM_L3_IPMC,BCM_L3_FLOW_ONLY,&net_egr_id);
        if (BCM_FAILURE(rv)) {
            printf("\nError in configuring egress obj: %s.\n", bcm_errmsg(rv));
            return rv;
        }

        printf("Network: Add flow match (match: SIP => network SVP)\n");
        bcm_flow_match_config_t_init(&match_info);
        match_info.flow_handle = flow_handle;
        match_info.criteria = BCM_FLOW_MATCH_CRITERIA_SIP;
        match_info.sip = vxlan_net->local_ip;
        match_info.flow_port = *net_flow_port;
        match_info.valid_elements = BCM_FLOW_MATCH_SIP_VALID |
                                    BCM_FLOW_MATCH_FLOW_PORT_VALID;
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

        rv = create_bc_group(unit,vxlan_cfg->bc_group,vxlan_net->phy_Gport,net_egr_id);
        if (BCM_FAILURE(rv)) {
            printf("\nError in adding egress obj: %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    printf("Network: Set Egress-vlan-xlate key-type for network VP\n");
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, *net_flow_port,
                    bcmVlanPortTranslateEgressKey, bcmVlanTranslateEgressKeyVpn));
    return BCM_E_NONE;
}

/* vxlan Configuration function */
bcm_error_t
config_vxlan_ipv4_vtep(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* VPN Configuration variables */
    vxlan_cfg_t vxlan_cfg = {
        8472,
        0x7010,
        0x12345,     /* vnid */
        0
    };

    vxlan_acc_cfg_t vxlan_acc = {
        egress_phy_gport, /* port */
        21,/*vlan */
        "00:00:00:00:11:11",/*local mac */
        "00:00:00:00:00:01",/* remote mac */
        0,
        0
    };

    vxlan_net_cfg_t vxlan_net = {
        ingress_phy_gport_1, /* port */
        22, /* vlan */
        "00:00:00:00:22:22",/* local mac */
        "01:00:5e:02:02:02",/* remote mac */
        0xe1020202,  /*225.2.2.2*/
        0x12345,     /* vnid */
        0x0a0a0a01,  /*10.10.10.1 local IP*/
        0xffff,
        0,
        0
    };

    vxlan_net_cfg_t vxlan_net_1 = {
        ingress_phy_gport_2, /* port */
        23, /* vlan */
        "00:00:00:00:22:22",/* local mac */
        "01:00:5e:02:02:02",/* remote mac */
        0xe1020101,  /*225.2.1.1 remote IP*/
        0x12345,     /* vnid */
        0x0a0a0a02,  /*10.10.10.1 local IP*/
        0xffff,
        0,
        0
    };

    /* flow API */
    bcm_flow_handle_t   flow_handle;
    bcm_gport_t   acc_flow_port;
    bcm_gport_t   net_flow_port;
    bcm_gport_t   net_flow_port_1;

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

    printf("Configure known multicast group\n");
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_FLOW,vxlan_net.mc_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in multicast create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_access(unit,&vxlan_cfg,&vxlan_acc,vxlan_net.mc_group,&acc_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_network(unit,&vxlan_cfg,&vxlan_net,&net_flow_port,vxlan_net.mc_group,1,flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_tunnel(unit, &vxlan_cfg, &vxlan_net, net_flow_port, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in tunnel config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_network(unit,&vxlan_cfg,&vxlan_net_1,&net_flow_port_1,0,0,flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_tunnel(unit, &vxlan_cfg, &vxlan_net_1, net_flow_port_1, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in tunnel config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Network-to-Access: Add payload-src-mac Address => Access-flow-port (VP).\n");
    rv = add_l2_mc_entry(unit, "01:00:5e:01:01:01", vxlan_cfg.vxlan_vpn,vxlan_net.mc_group);
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
