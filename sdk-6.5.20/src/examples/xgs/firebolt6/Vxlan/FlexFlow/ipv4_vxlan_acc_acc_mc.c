/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : Vxlan Bridging , IPv4 access to access flow
 *            for uuc/bc and known-multicast with SDTAG operations
 *
 * Usage    : BCM.0> cint ipv4_vxlan_acc_acc_mc.c
 *
 * config   : config_fb6_flex.bcm
 *
 * Log file : ipv4_vxlan_acc_acc_mc_log.txt
 *
 * Test Topology :
 *
 *                       +----------------------+
 *                       |Firebolt-6            |
 *                       |                      |VLAN-22
 *                       |              Port_2  +--------------
 *                VLAN-21|                      |           00:00:00:00:22:22
 *         +-------------+Port_1                |
 *  00:00:00:00:11:11    |                      |
 *                       |                      |
 *                       |                      |VLAN-23
 *                       |               Port_3 +--------------
 *                       |                      |           00:00:00:00:33:33
 *                       |  VLAN-21,22,23       |
 *                       |  belong to same VFI  |
 *                       +----------------------+
 *
 * Summary:
 * ========
 * Cint example to demonstrate VxLAN Bridging configuration for access to access multicast -
 *             - access to access bridging
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
 *     a) Configure a VxLAN Bridging configuration with SDTAG operations.
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
 *       We can see the packet on the egress port
 *       and 'show c' to check the Tx/Rx packet stats/counters.
 */
/*
 *  UUC/UMC/BC multicast group has {port1, port2, port3} as member
 *  known multicast group has {port2} as member

 * Packets:

step-1: Unknown unicast case
========
Packet sent:
Input (ce0):
    ###[ Ethernet ]###
     dst= 00:00:00:00:44:44
     src= 00:00:00:00:11:11
     type= 0x8100
    ###[ 802.1Q ]###
     prio= 0
     id= 0
     vlan= 21
     type= 0x800
    ###[ IP ]###
        version= 4
        proto= udp
        src= 1.1.1.1
        dst= 4.4.4.4
    ###[ UDP ]###

Input (ce0) Hex:
tx port=1 data=000000004444000000001111810000150800450000500000400040113094010101010404040400350035003C2EF274657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

Output(ce1):
data[0000]: 00000000 44440000 00001111 81000016
data[0010]: 08004500 00500000 40004011 30940101
data[0020]: 01010404 04040035 0035003c 2ef27465
data[0030]: 73747465 73747465 73747465 73747465
data[0040]: 73747465 73747465 73747465 73747465
data[0050]: 73747465 73747465 73747465 737437d3
data[0060]: 6ea5

Output(ce2):
data[0000]: 00000000 44440000 00001111 81000017
data[0010]: 08004500 00500000 40004011 30940101
data[0020]: 01010404 04040035 0035003c 2ef27465
data[0030]: 73747465 73747465 73747465 73747465
data[0040]: 73747465 73747465 73747465 73747465
data[0050]: 73747465 73747465 73747465 737466a4
data[0060]: 97ee

step-2: Known Multicast case
=======
Packet sent from ce0 to ce1:
Input (ce0):
  ###[ Ethernet ]###
    dst= 01:00:5e:01:01:01
    src= 00:00:00:00:11:11
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 21
     type= 0x800
  ###[ IP ]###
        version= 4
        proto= udp
        src= 1.1.1.1
        dst= 225.1.1.1
  ###[ UDP ]###

Input (ce0) Hex:
tx port=1 data=01005E01010100000000111181000015080045000050000040004011569901010101E101010100350035003C54F774657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

Output(ce1):
data[0000]: 01005e01 01010000 00001111 81000016
data[0010]: 08004500 00500000 40004011 56990101
data[0020]: 0101e101 01010035 0035003c 54f77465
data[0030]: 73747465 73747465 73747465 73747465
data[0040]: 73747465 73747465 73747465 73747465
data[0050]: 73747465 73747465 73747465 7374603c
data[0060]: 1f4e
*/
/* Reset C interpreter*/
cint_reset();

bcm_error_t show_vxlan_vtep_config(int unit);

bcm_port_t ingress_port;
bcm_port_t egress_port_1;
bcm_port_t egress_port_2;
bcm_gport_t ingress_phy_gport;
bcm_gport_t egress_phy_gport_1;
bcm_gport_t egress_phy_gport_2;

/* struct for Access Port Params for creating access VP*/
struct vxlan_acc_cfg_t {
    bcm_gport_t   phy_Gport;/* Input : gport of the port/trunk*/
    bcm_vlan_t    acc_vlan; /* access vlan*/
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
};

struct vxlan_cfg_t {
    uint16 udp_dp;
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
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_qset_t qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_port_t     port;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
        BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
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
    int port_list[3];
    bcm_pbmp_t pbmp;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port_1 = port_list[1];
    egress_port_2 = port_list[2];

    if (BCM_E_NONE != bcm_port_gport_get(unit, ingress_port, &(ingress_phy_gport))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE !=bcm_port_gport_get(unit, egress_port_1, &(egress_phy_gport_1))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", egress_port_1);
        return -1;
    }

    if (BCM_E_NONE !=bcm_port_gport_get(unit, egress_port_2, &(egress_phy_gport_2))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", egress_port_2);
        return -1;
    }

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port_1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port_2);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for port %d\n", egress_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

/*
 * Verification:
 *   In Packet:
 *   ----------
 *  000000004444000000001111
 *  810000150800450000500000
 *  400040113094010101010404
 *  040400350035003C2EF27465
 *  737474657374746573747465
 *  737474657374746573747465
 *  737474657374746573747465
 *  737474657374746573747465
 *  7374
 *
 *   Out Packet:
 *   -----------
 *  00000000 44440000 00001111 81000016
 *  08004500 00500000 40004011 30940101
 *  01010404 04040035 0035003c 2ef27465
 *  73747465 73747465 73747465 73747465
 *  73747465 73747465 73747465 73747465
 *  73747465 73747465 73747465 737437d3
 *  6ea5
 */
    void
verify(int unit)
{
    char str[512];

    bshell(unit, "hm ieee");
    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
    bshell(unit, "clear c");
    printf("\nSending unknown unicast packet to ingress_port access side:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000004444000000001111810000150800450000500000400040113094010101010404040400350035003C2EF274657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    printf("\nSending known multicast packet to ingress_port access side:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x01005E01010100000000111181000015080045000050000040004011569901010101E101010100350035003C54F774657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

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

int
vxlan_create_mc_egr_obj(int unit, int l3_if, vxlan_net_cfg_t *vxlan_net, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    bcm_if_t vp;
    vp = BCM_GPORT_FLOW_PORT_ID_GET(vxlan_net->nw_gport);
    l3_egress.encap_id = vp;
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
        printf("\nbcm_flow_port_encap_set() failed: %s\n", bcm_errmsg(rv));
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

/* Configure UUC/UMC/BC multicast group */
int
config_mc_group(int unit,bcm_multicast_t bc_group,bcm_if_t egr_id,bcm_gport_t gport)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_if_t encap_id;
    rv = bcm_multicast_egress_object_encap_get(unit,bc_group,egr_id,&encap_id);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_multicast_egress_object_encap_get failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_multicast_egress_add(unit,bc_group,gport,encap_id);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_multicast_egress_add failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/*Configure known multicast group*/
int
config_vxlan_known_mcast(int unit,bcm_if_t egr_id,vxlan_acc_cfg_t *vxlan_acc)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_multicast_t known_mcast_group;
    bcm_if_t encap_id;
    printf("Create Multicast group for UUC/MC/BC.\n");
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_FLOW, &known_mcast_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in multicast create: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_multicast_egress_object_encap_get(unit,known_mcast_group,egr_id,&encap_id);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_multicast_egress_object_encap_get failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_multicast_egress_add(unit,known_mcast_group,vxlan_acc->phy_Gport,encap_id);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_multicast_egress_add failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    vxlan_acc->mc_group = known_mcast_group;
    return rv;
}

/* Access Vxlan VP creation */
int
config_vxlan_access(int unit, vxlan_cfg_t *vxlan_cfg, vxlan_acc_cfg_t *vxlan_acc,int known_mc, bcm_gport_t *acc_flow_port)
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
    rv = create_l3_interface(unit, 0, vxlan_acc->acc_remote_mac, vxlan_acc->acc_vlan, &acc_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create access L3 Egress object.\n");
    rv = vxlan_create_egr_obj(unit, 0, BCM_IF_INVALID, vxlan_acc->acc_remote_mac, vxlan_acc->phy_Gport, vxlan_acc->acc_vlan, &acc_egr_id);
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
    match_info.criteria  = BCM_FLOW_MATCH_CRITERIA_PORT;
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
                   BCM_FLOW_ENCAP_FLAG_VLAN_REPLACE;
    /*end:sdtag*/
    rv = vxlan_flow_port_encap_set(unit, peinfo);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access encap: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_mc_group(unit,vxlan_cfg->mc_group,acc_egr_id,vxlan_acc->phy_Gport);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access encap: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    if(known_mc){
        printf("Configure known multicast group\n");
        rv = config_vxlan_known_mcast(unit,acc_egr_id,vxlan_acc);
        if (BCM_FAILURE(rv)) {
            printf("\nError in configuring access encap: %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    printf("Add L2 Entry \n");
    rv = add_to_l2_table(unit, vxlan_acc->acc_local_mac, vxlan_cfg->vxlan_vpn, *acc_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in l2 entry config: %s.\n", bcm_errmsg(rv));
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
    rv = create_l3_interface(unit, BCM_L3_ADD_TO_ARL, vxlan_net->net_local_mac, vxlan_net->net_vlan, &net_intf_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create network L3 Egress object.\n");
    rv = vxlan_create_mc_egr_obj(unit,net_intf_id,vxlan_net,&net_egr_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring egress obj: %s.\n", bcm_errmsg(rv));
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
    /*sdtag*/
    einfo.valid_elements |= ( BCM_FLOW_ENCAP_FLAGS_VALID|
            BCM_FLOW_ENCAP_TPID_VALID |
            BCM_FLOW_ENCAP_VLAN_VALID);
    einfo.flags = BCM_FLOW_ENCAP_FLAG_SERVICE_TAGGED |
        BCM_FLOW_ENCAP_FLAG_VLAN_DELETE;
    /*end:sdtag*/
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

/* vxlan Configuration function */
bcm_error_t
config_vxlan_ipv4_vtep(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* VPN Configuration variables */
    vxlan_cfg_t vxlan_cfg = {
        8472,
        0x7010,
        0,
        0
    };

    vxlan_acc_cfg_t vxlan_acc_1 = {
        ingress_phy_gport, /* port */
        21,/*vlan */
        "00:00:00:00:11:11",/*local mac */
        "00:00:00:00:00:01",/* remote mac */
        0,
        0
    };

    vxlan_acc_cfg_t vxlan_acc_2 = {
        egress_phy_gport_1, /* port */
        22,/*vlan */
        "00:00:00:00:22:22",/*local mac */
        "00:00:00:00:00:01",/* remote mac */
        0,
        0
    };

    vxlan_acc_cfg_t vxlan_acc_3 = {
        egress_phy_gport_2, /* port */
        23,/*vlan */
        "00:00:00:00:33:33",/*local mac */
        "00:00:00:00:00:01",/* remote mac */
        0,
        0
    };

    /* flow API */
    bcm_flow_handle_t  flow_handle;
    bcm_gport_t   acc_flow_port_1;
    bcm_gport_t   acc_flow_port_2;
    bcm_gport_t   acc_flow_port_3;

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
    /*create vxlan_acc_1 port for unknown traffic*/
    rv = config_vxlan_access(unit, &vxlan_cfg, &vxlan_acc_1,0, &acc_flow_port_1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    /*create vxlan_acc_1 port for unknown and known traffic*/
    rv = config_vxlan_access(unit, &vxlan_cfg, &vxlan_acc_2,1, &acc_flow_port_2);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    /*create vxlan_acc_1 port for unknown traffic*/
    rv = config_vxlan_access(unit, &vxlan_cfg, &vxlan_acc_3,0, &acc_flow_port_3);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Network-to-Access: Add payload-src-mac Address => Access-flow-port (VP).\n");
    rv = add_l2_mc_entry(unit, "01:00:5e:01:01:01", vxlan_cfg.vxlan_vpn, vxlan_acc_2.mc_group);
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
