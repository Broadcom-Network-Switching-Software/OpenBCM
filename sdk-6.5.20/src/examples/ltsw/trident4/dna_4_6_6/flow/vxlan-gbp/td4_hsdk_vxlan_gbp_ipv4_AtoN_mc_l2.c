/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : TD4(56880) Vxlan Bridging, access to network known multicast IPv4 tunnel initiation
 *            with Vxlan GBP-SID assignment for a l2 class-id
 *
 * Usage    : BCM.0> cint td4_hsdk_vxlan_gbp_ipv4_AtoN_uc_l2.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : td4_hsdk_vxlan_gbp_ipv4_AtoN_uc_l2_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |   VLAN-22        Underlay-NextHop
 *                                  |                      |  egress_port       +---+
 *   SMAC 00::bb:bb                 |                      +--------------------+   |
 *            +---+                 |                      |00::22:22           +---+
 *   Host(acc)|   +-----------------+      Trident-4       |                    00::02
 *            +---+    ingress_port |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  +----------------------+
 *
 * Summary:
 * ========
 *
 * Cint example to demonstrate Vxlan bridging configuration for tunnel Initiation -
 *             - access to network (encap flow)
 *             - VxLAN GBP 'G' bit is set in VxLAN header
 *             - VxLAN GBP SID is added in VxLAN heade
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
 *     Note: IFP rule is meant for a verification purpose and is not part of the
 *           feature configuration.
 *
 *   2) Step2 - Configuration (Done in config_vxlan_ipv4_vtep()):
 *   ===========================================================
 *     a) Creates a vxlan VPN.
 *     b) Perform access-side configuration.
 *     c) Perform network-side configuration.
 *     d) Add an entry into L2 table pointing to Network Virtual port.
 *     e) Assign a l2 class-id in the L2 table entry
 *     f) Match fp rule on the l2-class-id and assign gbp sid
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l2 show',traversing the vpn
 *        and vp's
 *
 *     b) Transmit known multicast packets single outer tagged
 *        The contents of the packets are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *     Egress tunnel packet on the network port with the vxlan encapsulation and gbp sid set.
 */

/*
Packet:

ACCESS-TO-NETWORK:-
Encap-case: packet sent from ingress_port(access) to egress_port(network)

example Ingress double tagged Packet:
=====================================
Header Info
  ###[ Ethernet ]###
    dst= 01:00:5E:01:01:02
    src= 00:00:00:00:11:11
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 21
     type = 0x800
  ###[ IP ]###
        version= 4
        proto= udp
        src= 1.1.1.1
        dst= 225.1.1.1
  ###[ UDP ]###

Ingress Hex Packet:
tx port=1 data=0x01005E01010100000000111181000015080045000050000040004011569901010101E101010100350035003C54F774657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

Egress Packet :
=============
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
     sport= 49152
     dport= 4789
  ###[ VXLAN ]###
     flags= 0x8800
     gbp = 0x0fab
     vni= 0x12345
  ###[ Ethernet ]###
                 dst= 01:00:5E:01:01:02
                 src= 00:00:00:00:11:11
                 type= 0x800
  ###[ IP ]###
        version= 4
        proto= udp
        src= 1.1.1.1
        dst= 225.1.1.1
  ###[ UDP ]###

Packet Dump
==========
data[0000]: {000000000002} {000000002222} 8100 0016
data[0010]: 0800 4500 0082 0000 0000 3f11 a5b7 0a0a
data[0020]: 0a01 c0a8 0101 c000 12b5 006e 0000 8800
data[0030]: 0fab 0123 4500 0100 5e01 0101 0000 0000
data[0040]: 1111 0800 4500 0050 0000 4000 4011 5699
data[0050]: 0101 0101 e101 0101 0035 0035 003c 54f7
data[0060]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0070]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0080]: 7465 7374 7465 7374 7465 7374 7465 7374
data[0090]: 7465 7374
*/

/* Reset C interpreter*/
cint_reset();

bcm_port_t    ingress_port;
bcm_port_t    egress_port;
bcm_gport_t   ingress_phy_gport;
bcm_gport_t   egress_phy_gport;

/* struct for Access Port Params for creating access VP*/
struct vxlan_acc_cfg_t {
    bcm_port_t    acc_port;     /*access physical port*/
    bcm_gport_t   phy_Gport;    /*Input : gport of the port/trunk*/
    bcm_vlan_t    acc_vlan;     /*access vlan*/
    bcm_mac_t     acc_local_mac;
    bcm_mac_t     acc_remote_mac;
    bcm_mac_t     acc_remote_mc_mac;
    bcm_gport_t   acc_gport;    /* Output:Access VP gport */
    int           port_class;
    int           flood_group; /* Multicast flood group */
};

/* struct for Network Port params for creating network VP*/
struct vxlan_net_cfg_t {
    bcm_port_t    net_port;       /*network physical port*/
    bcm_port_t    phy_Gport;      /*Input : gport of the port/trunk*/
    bcm_vlan_t    net_vlan;       /*tunnel vid*/
    bcm_mac_t     net_local_mac;
    bcm_mac_t     net_remote_mac;
    bcm_ip_t      remote_ip;
    bcm_ip_t      remote_mc_ip;
    uint32        vxlan_vnid;
    bcm_ip_t      local_ip;
    uint16        udp_sp;
    bcm_gport_t   nw_gport;       /* Output:Nw VP gport */
    int           dvp_group_id;
};

struct vxlan_cfg_t {
    uint16          udp_dp;
    bcm_vpn_t       vxlan_vpn;
	bcm_multicast_t bc_group;
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
    int i = 0, port = 0, rv = BCM_E_NONE;
    bcm_port_config_t configP;
    bcm_pbmp_t        ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",
                                           bcm_errmsg(rv));
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
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_group_config_t group_config;
    bcm_field_entry_t        entry;
    bcm_port_t               port;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.priority = 1;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                                  BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group,
                                                   &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,
                                                  BCM_FIELD_EXACT_MATCH_MASK));

        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                                 bcmFieldActionCopyToCpu,
                                                 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }

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
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_port_t        port;

    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_setup port: %d setup\n", port);

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                                  BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group,
                                                   &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,
                                                  BCM_FIELD_EXACT_MATCH_MASK));

        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                                 bcmFieldActionCopyToCpu, 0,
                                                 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                                 bcmFieldActionDrop, 0, 0));

        BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
    }
    return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_multi_setup() and egress_port_multi_setup().
 */
bcm_error_t
test_setup(int unit)
{
    uint32 num_ports = 2;
    int port_list[num_ports];
    bcm_pbmp_t    pbmp;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];

    if (BCM_E_NONE != bcm_port_gport_get(unit, ingress_port,
                                         &(ingress_phy_gport)))
    {
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                                  ingress_port);
        return -1;
    }

    if (BCM_E_NONE !=bcm_port_gport_get(unit, egress_port,
                                        &(egress_phy_gport)))
    {
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                                  egress_port);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, pbmp))
    {
        printf("ingress_port_multi_setup() failed for port %d\n",
                                                  ingress_port);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp))
    {
        printf("egress_port_multi_setup() failed for port %d\n",
                                                  egress_port);
        return -1;
    }

    bshell(unit, "pw start report +all");
    return BCM_E_NONE;
}

/* verifying the VPN configuration */
bcm_error_t
show_vxlan_vtep_config(int unit)
{
    bcm_flow_vpn_traverse(unit,vpn_traverse,NULL);
    return BCM_E_NONE;
}

/*
 * Verification:
 */
void
verify(int unit)
{
    char str[512];
    uint64 in_pkt, out_pkt, in_bytes, out_bytes;

    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
	bshell(unit, "multicast show");
    bshell(unit, "clear c");

    printf("\nSending known multicast packet to ingress_port access side:%d\n",
                                         ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x01005E01010100000000111181000015080045000050000040004011569901010101E101010100350035003C54F774657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    /* dump l2 table to see if SA learned againt SVP */
    bshell(unit, "l2 show");

    /* Dump the Configured VPN & VP's*/
    show_vxlan_vtep_config(unit);

    printf("\n******Port stats check******\n");

    printf("--------Access port (%d) stats--------\n", ingress_port);
    COMPILER_64_ZERO(in_pkt);
    COMPILER_64_ZERO(in_bytes);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, ingress_port, snmpDot1dTpPortInFrames, &in_pkt));
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, ingress_port, snmpIfInOctets, &in_bytes));
    printf("Rx : %d packets / %d bytes\n", COMPILER_64_LO(in_pkt),COMPILER_64_LO(in_bytes));

    printf("--------Network port (%d) stats--------\n", egress_port);
    COMPILER_64_ZERO(out_pkt);
    COMPILER_64_ZERO(out_bytes);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egress_port, snmpDot1dTpPortOutFrames, &out_pkt));
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egress_port, snmpIfOutOctets, &out_bytes));
    printf("Tx : %d packets / %d bytes\n", COMPILER_64_LO(out_pkt),COMPILER_64_LO(out_bytes));
    if (COMPILER_64_LO(out_pkt) != 1) {
        printf("Packet verify failed. Expected out packet 1 but get %d\n", COMPILER_64_LO(out_pkt));
        return -1;
    }
    return;
}

/* Create vlan and add port to vlan */
bcm_error_t
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_port_add: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

bcm_error_t
vxlan_system_config(int unit, vxlan_cfg_t *vxlan_cfg)
{
    printf("Configure Vxlan Global Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_vlan_control_set(unit, bcmVlanTranslate, TRUE));
    return BCM_E_NONE;
}

/* vxlan configuration specific to access physical port */
bcm_error_t
vxlan_access_port_config(int unit, bcm_gport_t gport_acc)
{
    printf("Configure Vxlan access port configuration\n");

    BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit, gport_acc, 0x9100));
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, gport_acc,
                                              bcmVlanTranslateIngressEnable,
                                              TRUE));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_acc,
                                         bcmPortControlVxlanEnable,
                                         FALSE));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_acc,
                                             bcmPortControlVxlanTunnelbasedVnId,
                                             FALSE));
    return BCM_E_NONE;
}

/* vxlan configuration specific to network physical port */
bcm_error_t
vxlan_network_port_config(int unit, bcm_gport_t gport_net)
{
    printf("Configure Vxlan Network Port Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_net,
                                             bcmPortControlVxlanEnable, TRUE));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_net,
                                             bcmPortControlVxlanTunnelbasedVnId,
                                             FALSE));
    return BCM_E_NONE;
}

/* Create L3 interface */
bcm_error_t
create_l3_interface(int unit, int flags, bcm_mac_t local_mac, int vid,
                    bcm_if_t *intf_id, int intf_flags)
{
    bcm_l3_intf_t l3_intf;
    bcm_error_t   rv=BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_flags      |= flags;
    l3_intf.l3a_intf_flags |= intf_flags;
    l3_intf.l3a_vid         = vid;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    *intf_id = l3_intf.l3a_intf_id;

    return rv;
}

bcm_error_t
vxlan_create_egr_obj(int unit, uint32 flag, int l3_if, bcm_mac_t nh_mac,
                     bcm_gport_t gport, int vid, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.flags2 |= flag;
    l3_egress.intf    = l3_if;
    l3_egress.vlan    = vid;
    l3_egress.port    = gport;
    return bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
}

/* Create the VPN for vxlan */
bcm_error_t
create_vxlan_vpn(int unit, vxlan_cfg_t *vxlan_cfg)
{
    bcm_flow_vpn_config_t vpn_info;
	bcm_multicast_t       bc_group=0;
    bcm_error_t           rv=BCM_E_NONE;

    printf("Create Multicast group for UUC/MC/BC.\n");
	rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_FLOW, &bc_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in multicast create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create Flow VPN.\n");

    bcm_flow_vpn_config_t_init (&vpn_info);
    vpn_info.flags                   = BCM_FLOW_VPN_ELAN |
                                 BCM_FLOW_VPN_WITH_ID;
	vpn_info.broadcast_group         = bc_group;
	vpn_info.unknown_unicast_group   = bc_group;
	vpn_info.unknown_multicast_group = bc_group;
    BCM_IF_ERROR_RETURN(bcm_flow_vpn_create(unit, vxlan_cfg->vxlan_vpn,
                                        &vpn_info));
	vxlan_cfg->bc_group=bc_group;

    return BCM_E_NONE;
}

/* Common code for Access/Network VP's */
bcm_error_t
create_vxlan_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t *vp)
{
    bcm_flow_port_t  flow_port;
    bcm_error_t      rv=BCM_E_NONE;

    printf("Create Flow VP.\n");
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
bcm_error_t
vxlan_flow_match_add(int unit, bcm_flow_match_config_t match_info)
{
    bcm_error_t rv=BCM_E_NONE;

    rv = bcm_flow_match_add(unit, &match_info, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_flow_match_add() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Encap configuration */
bcm_error_t
vxlan_flow_port_encap_set(int unit, bcm_flow_port_encap_t peinfo)
{
    bcm_error_t rv=BCM_E_NONE;

    rv =  bcm_flow_port_encap_set(unit, &peinfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nAccess: bcm_flow_port_encap_set() failed: %s\n",
                                              bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Static L2 entry for forwarding */
bcm_error_t
add_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port, int l2_class)
{
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags |= BCM_L2_STATIC;
    l2_addr.port   = port;
    l2_addr.group = l2_class;
    return bcm_l2_addr_add(unit, &l2_addr);
}

/* Static L2MC entry for forwarding */
bcm_error_t
add_l2_mc_entry(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id,
                bcm_multicast_t mc_group, int l2_class)
{
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags |= BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = mc_group;
    l2_addr.group = l2_class;
    return bcm_l2_addr_add(unit, &l2_addr);
}

/* Access Vxlan VP creation */
bcm_error_t
config_vxlan_access(int unit, vxlan_cfg_t *vxlan_cfg,
                    vxlan_acc_cfg_t *vxlan_acc, bcm_gport_t *acc_flow_port,
                    bcm_flow_handle_t flow_handle)
{
    /* Access Configuartion variables */
    bcm_flow_match_config_t match_info;
    bcm_flow_port_encap_t   peinfo;
    bcm_error_t             rv=BCM_E_NONE;
    bcm_vlan_action_set_t   vlan_action;

    BCM_IF_ERROR_RETURN(bcm_port_class_set(unit, vxlan_acc->acc_port, bcmPortClassIngress,
                                           vxlan_acc->port_class));
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vxlan_cfg->vxlan_vpn,
                                           vxlan_acc->phy_Gport, 0));
    printf("Configure VLAN \n");
    rv = vlan_create_add_port(unit, vxlan_acc->acc_vlan, vxlan_acc->acc_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring vlan: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vxlan_access_port_config(unit, vxlan_acc->phy_Gport);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    /*This is optional as VP on access-side is not
    * supported in DNA_4_4_11
    */
    printf("Create Access Flow Port (VP).\n");
    rv = create_vxlan_vp(unit, vxlan_cfg->vxlan_vpn, 0, acc_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access vp: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    vxlan_acc->acc_gport=(*acc_flow_port);
    if (vxlan_acc->flood_group == 1) {
        printf("UnKnown group: 0x%x\n", vxlan_cfg->bc_group);
        BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, vxlan_cfg->bc_group,
                                                     vxlan_acc->phy_Gport,
                                                     BCM_IF_INVALID));
    } else if (vxlan_acc->flood_group == 2) {
        printf("Known MC group: 0x%x\n", vxlan_cfg->mc_group);
        BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, vxlan_cfg->mc_group,
                                                     vxlan_acc->phy_Gport,
                                                     BCM_IF_INVALID));
    } else {
        printf("UnKnown group: 0x%x\n", vxlan_cfg->bc_group);
        BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, vxlan_cfg->bc_group,
                                                     vxlan_acc->phy_Gport,
                                                     BCM_IF_INVALID));
        printf("Known MC group: 0x%x\n", vxlan_cfg->mc_group);
        BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, vxlan_cfg->mc_group,
                                                     vxlan_acc->phy_Gport,
                                                     BCM_IF_INVALID));
    }

	printf("Access SVP assignment \n");
    bcm_flow_match_config_t_init(&match_info);
    match_info.flow_handle = flow_handle;
    match_info.criteria    = BCM_FLOW_MATCH_CRITERIA_PORT;
    match_info.flow_port   = *acc_flow_port;
    match_info.port        = vxlan_acc->phy_Gport;
    match_info.valid_elements = (BCM_FLOW_MATCH_PORT_VALID |
                           BCM_FLOW_MATCH_FLOW_PORT_VALID);
    rv = vxlan_flow_match_add(unit, match_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access flow match: %s.\n",
                                                      bcm_errmsg(rv));
        return rv;
    }

    printf("VPN assignment Incoming Port -> VPN \n");
    /* Configure ingress port group */
    bcm_vlan_action_set_t_init(&vlan_action);
    rv = bcm_vlan_port_default_action_get(unit, vxlan_acc->phy_Gport,
                                          &vlan_action);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_vlan_port_default_action_get(): %s.\n",
                                          bcm_errmsg(rv));
    /*Configure ING_L2_IIF_TABLE.VLAN_TO_VFI_MAPPING_ENABLE to 0*/
        return rv;
    }

    /* Port must be added to VPN to pass membership check */
    vlan_action.forwarding_domain = vxlan_cfg->vxlan_vpn;
    rv =  bcm_vlan_port_default_action_set(unit, vxlan_acc->phy_Gport,
                                           vlan_action);
    if (BCM_FAILURE(rv)) {
        printf("\nError executing bcm_port_untagged_vlan_set(): %s.\n",
                                           bcm_errmsg(rv));
        return rv;
    }

    /*Configure ING_L2_IIF_TABLE.VFI*/
    rv = bcm_port_control_set(unit, vxlan_acc->phy_Gport,
                              bcmPortControlTrustIncomingVlan, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError executing bcm_port_control_set(): %s.\n",
                                          bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Tunnel creation and associating it with Nw VP */
bcm_error_t
config_vxlan_tunnel(int unit, vxlan_cfg_t *vxlan_cfg,
                    vxlan_net_cfg_t *vxlan_net, bcm_gport_t net_flow_port,
                    bcm_flow_handle_t flow_handle)
{
    /* Tunnel Configuration variables */
    bcm_flow_tunnel_terminator_t ttinfo;
    bcm_flow_tunnel_initiator_t  tiinfo;
    bcm_error_t                  rv=BCM_E_NONE;

    printf("Tunnel_init: Create Flow Tunnel Initiator.\n");
    bcm_flow_tunnel_initiator_t_init(&tiinfo);
    tiinfo.flow_handle = flow_handle;
    tiinfo.flow_port   = net_flow_port;
    tiinfo.type        = bcmTunnelTypeVxlan;
    tiinfo.sip         = vxlan_net->local_ip;
    tiinfo.dip         = vxlan_net->remote_ip;
    tiinfo.ttl         = 0x3f;
    tiinfo.udp_dst_port = vxlan_cfg->udp_dp;
    tiinfo.valid_elements = BCM_FLOW_TUNNEL_INIT_DIP_VALID |
                            BCM_FLOW_TUNNEL_INIT_SIP_VALID |
                            BCM_FLOW_TUNNEL_INIT_UDP_DPORT_VALID |
                            BCM_FLOW_TUNNEL_INIT_TTL_VALID;
    rv = bcm_flow_tunnel_initiator_create(unit, &tiinfo, 0,NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring tunnel: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Tunnel_term: Create Flow Tunnel Terminator.\n");
    bcm_flow_tunnel_terminator_t_init(&ttinfo);
    ttinfo.flow_handle    = flow_handle;
    ttinfo.valid_elements = BCM_FLOW_TUNNEL_TERM_DIP_VALID;
    ttinfo.dip            = vxlan_net->local_ip;
    ttinfo.type           = bcmTunnelTypeVxlan;
    rv = bcm_flow_tunnel_terminator_create(unit, &ttinfo, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring tunnel: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/* vxlan network VP creation . tunnel is not created */
bcm_error_t
config_vxlan_network(int unit, vxlan_cfg_t *vxlan_cfg,
                     vxlan_net_cfg_t *vxlan_net, bcm_gport_t *net_flow_port,
                     bcm_flow_handle_t flow_handle)
{
    /* Network Configuartion variables */
    bcm_flow_match_config_t match_info;
    bcm_flow_encap_config_t einfo;
    bcm_flow_port_encap_t   peinfo;
    bcm_error_t rv = BCM_E_NONE;
    bcm_if_t net_intf_id;
    bcm_if_t net_egr_id;
    uint32  flags=0;
    bcm_multicast_encap_t   mc_encap;
    int  encap_id = 0;

    /* Port must be added to VPN to pass membership check */
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vxlan_cfg->vxlan_vpn,
                                           vxlan_net->phy_Gport, 0));

    rv = vlan_create_add_port(unit, vxlan_net->net_vlan, vxlan_net->net_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring vlan: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vxlan_network_port_config(unit, vxlan_net->phy_Gport);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vxlan_network_port_config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create network L3 interface\n");
    rv = create_l3_interface(unit, 0, vxlan_net->net_local_mac,
                             vxlan_net->net_vlan, &net_intf_id,
                             BCM_L3_INTF_UNDERLAY);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring l3 interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create network L3 Egress object.\n");
    rv = vxlan_create_egr_obj(unit, BCM_L3_FLAGS2_UNDERLAY, net_intf_id,
                              vxlan_net->net_remote_mac, vxlan_net->phy_Gport,
                              vxlan_net->net_vlan, &net_egr_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring egress obj: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create Network Flow Port.\n");
    flags = BCM_FLOW_PORT_NETWORK | BCM_FLOW_PORT_MULTICAST_GROUP_REMAP;
    rv = create_vxlan_vp(unit, vxlan_cfg->vxlan_vpn, flags, net_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network vp: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    vxlan_net->nw_gport=*net_flow_port;

	printf("Network: Set Encapsulation. VP=>egress-obj{Device,Port}.\n");
	bcm_flow_port_encap_t_init(&peinfo);
	peinfo.flow_handle    = flow_handle;
    peinfo.dvp_group      = vxlan_net->dvp_group_id;
	peinfo.flow_port      = *net_flow_port;
	peinfo.valid_elements = BCM_FLOW_PORT_ENCAP_DVP_GROUP_VALID |
		                    BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID;
	peinfo.egress_if      = net_egr_id;
	rv = vxlan_flow_port_encap_set(unit, peinfo);
	if (BCM_FAILURE(rv)) {
		printf("\nError in configuring network encap: %s.\n", bcm_errmsg(rv));
		return rv;
	}
    printf("Network: encap add, VFI-->VNID\n");
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle    = flow_handle;
	einfo.criteria       = BCM_FLOW_ENCAP_CRITERIA_VFI_DVP_GROUP;
    einfo.dvp_group      = vxlan_net->dvp_group_id;
    einfo.vnid           = vxlan_net->vxlan_vnid;
    einfo.vpn            = vxlan_cfg->vxlan_vpn;
    einfo.valid_elements = BCM_FLOW_ENCAP_VNID_VALID |
		                   BCM_FLOW_ENCAP_VPN_VALID |
                           BCM_FLOW_ENCAP_DVP_GROUP_VALID;
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
        printf("\nError in configuring network flow match: %s.\n",
                                                      bcm_errmsg(rv));
        return rv;
    }


    printf("Network: Add flow match (match: SIP => network SVP)\n");
    bcm_flow_match_config_t_init(&match_info);
    match_info.flow_handle    = flow_handle;
    match_info.criteria       = BCM_FLOW_MATCH_CRITERIA_SIP;
    match_info.sip            = vxlan_net->remote_ip;
    match_info.flow_port      = *net_flow_port;
    match_info.valid_elements = BCM_FLOW_MATCH_SIP_VALID |
                            BCM_FLOW_MATCH_FLOW_PORT_VALID;
    rv = vxlan_flow_match_add(unit, match_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring network flow match: %s.\n",
                                                      bcm_errmsg(rv));
        return rv;
    }

    bcm_multicast_encap_t_init(&mc_encap);
    mc_encap.encap_type = bcmMulticastEncapTypeL2Tunnel;
    mc_encap.ul_egress_if = net_egr_id;
    mc_encap.port_id = *net_flow_port;
    rv = bcm_multicast_encap_create(unit, &mc_encap, &encap_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_encap_create: %s.\n",
                                                      bcm_errmsg(rv));
        return rv;
    }
    printf("UnKnown group: 0x%x\n", vxlan_cfg->bc_group);

    rv = bcm_multicast_egress_add(unit, vxlan_cfg->bc_group, vxlan_net->phy_Gport, encap_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_egress_add: %s.\n",
                                                      bcm_errmsg(rv));
        return rv;
    }
    printf("Known MC group: 0x%x\n", vxlan_cfg->mc_group);

    rv = bcm_multicast_egress_add(unit, vxlan_cfg->mc_group, vxlan_net->phy_Gport, encap_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_egress_add: %s.\n",
                                                      bcm_errmsg(rv));
        return rv;
    }

	return BCM_E_NONE;
}

/*
 * Configure the vxlan gbp assignment for ingress port
 */
bcm_error_t
config_gbp_sid_port(int unit, bcm_port_t inport,int class_id)
{
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_field_group_config_t group_config;
    int gbp_sid = 0xfab;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifySrcClassL2);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset,bcmFieldActionVxlanGbpEnable);
    BCM_FIELD_ASET_ADD(group_config.aset,bcmFieldActionAssignVxlanGbpId);
    group_config.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    group_config.priority=3;
    group_config.mode= bcmFieldGroupModeAuto;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit,&group_config));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit,group_config.group,&entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, inport, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_SrcClassL2(unit, entry,class_id, 0xF));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionVxlanGbpEnable, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionAssignVxlanGbpId, gbp_sid, 0));
    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

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
        0x8064,
        0,
        0
    };

    vxlan_acc_cfg_t vxlan_acc = {
        ingress_port,
        ingress_phy_gport, /* gPort */
        21,                  /* vlan */
        "00:00:00:00:11:11", /* local mac */
        "00:00:00:00:00:01", /* remote mac */
        "01:00:5E:01:01:01", /* remote MC mac */
        0,
        33,
        3
    };

    vxlan_net_cfg_t vxlan_net = {
        egress_port,
        egress_phy_gport,  /* gPort */
        22,                  /* vlan */
        "00:00:00:00:22:22", /* local mac */
        "00:00:00:00:00:02", /* remote mac */
        0xC0A80101,          /* 192.168.1.1 remote IP*/
        0xe1020101,          /* 225.2.1.1 remote MC IP*/
        0x12345,             /* vnid */
        0x0a0a0a01,          /* 10.10.10.1 local IP*/
        0xffff,
        0,
        0xBC2
    };

    /* flow API */
    bcm_flow_handle_t flow_handle;
    bcm_gport_t       acc_flow_port;
    bcm_gport_t       net_flow_port;
    int class_id = 0xA;

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
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_FLOW,
                              vxlan_cfg.mc_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in multicast create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_access(unit, &vxlan_cfg, &vxlan_acc, &acc_flow_port,
                             flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_network(unit, &vxlan_cfg, &vxlan_net, &net_flow_port,
                              flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_vxlan_tunnel(unit, &vxlan_cfg, &vxlan_net, net_flow_port,
                             flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in tunnel config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Access-to-Network: Add payload-src-mac Address => Access-flow-port (VP).\n");
    rv = add_l2_mc_entry(unit,vxlan_acc.acc_remote_mc_mac, vxlan_cfg.vxlan_vpn,
                         vxlan_cfg.mc_group, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in l2 MC entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Add payload-src-mac to match on src_class_id\n");
    rv = add_to_l2_table(unit, vxlan_acc.acc_local_mac, vxlan_cfg.vxlan_vpn,
                         vxlan_acc.acc_port,class_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in l2 MC entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_gbp_sid_port(unit,vxlan_acc.acc_port,class_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError executing config_gbp_sid_port(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* verifying the VPN VP configuration using traverse*/
bcm_error_t
vpn_traverse(int unit,bcm_vpn_t vpn, bcm_flow_vpn_config_t *info, void *data)
{
    int   maxArr=20;
    bcm_flow_port_t flow_portArr[maxArr];
    int   count=0, iter=0;

    printf("\n Vpn :%d \n",vpn);
    printf("\n VpnInfo : \n");
    print *info;
    BCM_IF_ERROR_RETURN(bcm_flow_port_get_all(unit, vpn, maxArr, flow_portArr,
                                              &count));

    printf("\n Flow Ports %d\n",count);
    for(iter=0;(iter<count) && (iter < maxArr);++iter)
    {
        print flow_portArr[iter];
    }

    return BCM_E_NONE;
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
    int    unit = 0;

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

