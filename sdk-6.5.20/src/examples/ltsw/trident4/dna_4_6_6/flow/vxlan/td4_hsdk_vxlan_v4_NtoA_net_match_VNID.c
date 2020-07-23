/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
    
/*
 * Feature  : Vxlan Bridging , IPv4 network to access tunnel termination flow
 *            with network port match criteria based on VN_ID
 *
 * Usage    : BCM.0> cint TD4_HSDK_VXLAN_v4_NtoA_net_match_VNID.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : TD4_HSDK_VXLAN_v4_NtoA_net_match_VNID_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |   VLAN-22        Underlay-NextHop
 *                                  |                      | ingress_port_1     +---+
 *   DMAC 00::bb:bb                 |                      +--------------------+   |
 *            +---+          VLAN-21|                      |00::22:22           +---+
 *   Host(acc)|   +-----------------+      Trident-4       |                    00::02
 *            +---+   egress_port_1 |                      |
 *                                  |                      |
 *                                  |                      |                  Underlay-NextHop
 *                                  |                      | ingress_port_2     +---+
 *   DMAC 00::00:dd                 |                      +--------------------+   |
 *            +---+          VLAN-21|                      |00::22:22           +---+
 *   Host(acc)|   +-----------------+      Trident-4       |                    00::02
 *            +---+   egress_port_2 |                      |
 *                                  |                      |
 *                                  +----------------------+
 *
 * Summary:
 *
 * Cint example to demonstrate VxLAN Bridging configuration for tunnel termination -
 *             - network to access(decap flow)
 *
 * Detailed steps done in the CINT script:
 *
 *   1) Step1 - Test Setup (Done in test_setup()):
 *
 *     a) Selects four ports and configure them in Loopback mode. Out of these
 *        four ports, two ports are used as ingress_port and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy ingress & egress packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Configuration (Done in config_vxlan_ipv4_vtep()):
 *
 *     a) Configure a VxLAN Bridging configuration.
 *        This does the necessary configurations of vxlan global settings, vlan,
 *        access and network port setting, tunnel setup, interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *
 *     a) Check the configurations by 'vlan show', 'l2 show',traversing the vpn and vp's
 *
 *     b) Transmit the known unicast packet. The contents of the packet
 *        are printed on screen.
 *
 *     c) Expected Result:
 *
 *       We can see the packet on the egress port
 *       and 'show c' to check the Tx/Rx packet stats/counters.
 */
/*
NETWORK-TO-ACCESS:-
Decap-case: packet sent from (network) to (access)

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
     src= 192.168.1.1
     dst= 10.10.10.1
  ###[ UDP ]###
     sport= 49152
     dport= 4789
  ###[ VXLAN ]###
     flags= Instance
     reserved1= 0
     vni= 0x12345
     reserved2= 0x0
  ###[ Ethernet ]###
                 dst= 00:00:00:00:bb:bb
                 src= 00:00:00:00:aa:aa
                 type= 0x800
  ###[ IP ]###
                 version= 4
                 proto= udp
                    src= 2.2.2.2
                    dst= 1.1.1.1
  ###[ UDP ]###

Input Hex (network):

00000000222200000000002281000016080045000024000100004011a514c0a801010a0a0a01c00012b500100941080000000123450000000000bbbb00000000aaaa810000150800450000500001000040117497020202020101010100350035003c32f674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

Output (access):

[bcmPWN.0]Packet[2]: data[0000]: {00000000bbbb} {00000000aaaa} 8100 0015
[bcmPWN.0]Packet[2]: data[0010]: 0800 4500 0050 0001 0000 4011 7497 0202 
[bcmPWN.0]Packet[2]: data[0020]: 0202 0101 0101 0035 0035 003c 32f6 7465 
[bcmPWN.0]Packet[2]: data[0030]: 7374 7465 7374 7465 7374 7465 7374 7465 
[bcmPWN.0]Packet[2]: data[0040]: 7374 7465 7374 7465 7374 7465 7374 7465 
[bcmPWN.0]Packet[2]: data[0050]: 7374 7465 7374 7465 7374 7465 7374 7465 
[bcmPWN.0]Packet[2]: data[0060]: 7374 

Input Hex(network):

000000002233000000000003810000180800450000240001000040119009c0a8020214141401c00012b50010002408000000053535000000000000dd00000000cccc810000170800450000500001000040117497020202020101010100350035003c32f674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374

Output (access):

[bcmPWN.0]Packet[4]: data[0000]: {0000000000dd} {00000000cccc} 8100 0017
[bcmPWN.0]Packet[4]: data[0010]: 0800 4500 0050 0001 0000 4011 7497 0202 
[bcmPWN.0]Packet[4]: data[0020]: 0202 0101 0101 0035 0035 003c 32f6 7465 
[bcmPWN.0]Packet[4]: data[0030]: 7374 7465 7374 7465 7374 7465 7374 7465 
[bcmPWN.0]Packet[4]: data[0040]: 7374 7465 7374 7465 7374 7465 7374 7465 
[bcmPWN.0]Packet[4]: data[0050]: 7374 7465 7374 7465 7374 7465 7374 7465 
[bcmPWN.0]Packet[4]: data[0060]: 7374 
*/

/* Reset C interpreter*/
cint_reset();

bcm_error_t show_vxlan_vtep_config(int unit);

bcm_port_t egress_port_1;
bcm_port_t egress_port_2;
bcm_port_t ingress_port_1;
bcm_port_t ingress_port_2;
bcm_gport_t egress_phy_gport_1;
bcm_gport_t egress_phy_gport_2;
bcm_gport_t ingress_phy_gport_1;
bcm_gport_t ingress_phy_gport_2;

/* struct for Access Port Params for creating access VP*/
struct vxlan_acc_cfg_t {
    bcm_gport_t   phy_Gport;/* Input : gport of the port/trunk*/
    bcm_vlan_t    acc_vlan; /* Input */
    bcm_mac_t     acc_local_mac;
    bcm_mac_t     acc_remote_mac;
    int           port_class;
};

/* struct for Network Port params */
struct vxlan_net_cfg_t {
    bcm_port_t    phy_Gport;/* Input : gport of the port/trunk*/
    bcm_vlan_t    net_vlan;
    bcm_mac_t     net_local_mac;
    bcm_mac_t     net_remote_mac;
    bcm_ip_t      remote_ip;
    bcm_ip_t      local_ip;
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
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_qset_t qset;
    bcm_field_group_config_t gcfg1;
    bcm_field_entry_t entry;
    bcm_port_t     port;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    bcm_field_group_config_t_init(gcfg1);
    gcfg1.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    gcfg1.qset= qset;
    gcfg1.priority= 2;
    gcfg1.mode= bcmFieldGroupModeAuto;
    gcfg1.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    BCM_FIELD_ASET_ADD(gcfg1.aset, bcmFieldActionCopyToCpu);
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg1));

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, gcfg1.group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

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
    bcm_field_qset_t qset;
    bcm_field_group_config_t gcfg1;
    bcm_field_entry_t entry;
    bcm_port_t     port;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

    bcm_field_group_config_t_init(gcfg1);
    gcfg1.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    gcfg1.qset= qset;
    gcfg1.priority= 3;
    gcfg1.mode= bcmFieldGroupModeAuto;
    gcfg1.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    BCM_FIELD_ASET_ADD(gcfg1.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(gcfg1.aset, bcmFieldActionDrop);
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &gcfg1));

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, gcfg1.group, &entry));

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
    int port_list[4];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    egress_port_1 = port_list[0];
    egress_port_2 = port_list[1];
    ingress_port_1 = port_list[2];
    ingress_port_2 = port_list[3];

    if (BCM_E_NONE != bcm_port_gport_get(unit, egress_port_1, &(egress_phy_gport_1))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", egress_port_1);
        return -1;
    }

    if (BCM_E_NONE != bcm_port_gport_get(unit, egress_port_2, &(egress_phy_gport_2))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", egress_port_2);
        return -1;
    }

    if (BCM_E_NONE !=bcm_port_gport_get(unit, ingress_port_1, &(ingress_phy_gport_1))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", ingress_port_1);
        return -1;
    }

    if (BCM_E_NONE !=bcm_port_gport_get(unit, ingress_port_2, &(ingress_phy_gport_2))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", ingress_port_2);
        return -1;
    }

    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port_1);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port_2);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, pbmp)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port_1);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port_1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port_2);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for port %d\n", egress_port_1);
        return -1;
    }

    bshell(unit, "pw start report +raw +pmd +decode");

    return BCM_E_NONE;
}

void
verify(int unit)
{
    char str[1184];

    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
    bshell(unit, "clear c");
    printf("\nSending known unicast packet to ingress_port network side:%d\n", ingress_port_1);
    snprintf(str, 1184, "tx 1 pbm=%d data=00000000222200000000002281000016080045000024000100004011a514c0a801010a0a0a01c00012b500100941080000000123450000000000bbbb00000000aaaa810000150800450000500001000040117497020202020101010100350035003c32f674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374; sleep quiet 1", ingress_port_1);
    bshell(unit, str);
    bshell(unit, "sleep 2; show c");
    bshell(unit, "l2 show");

    printf("\nSending known unicast packet to ingress_port network side:%d\n", ingress_port_2);
    snprintf(str, 1184, "tx 1 pbm=%d data=000000002233000000000003810000180800450000240001000040119009c0a8020214141401c00012b50010002408000000053535000000000000dd00000000cccc810000170800450000500001000040117497020202020101010100350035003c32f674657374746573747465737474657374746573747465737474657374746573747465737474657374746573747465737474657374; sleep quiet 1", ingress_port_2);
    bshell(unit, str);
    bshell(unit, "sleep 2; show c");
    bshell(unit, "l2 show");
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


/* vxlan configuration specific to access physical port */
int
vxlan_access_port_config(int unit, bcm_port_t port_acc, int port_class)
{
    printf("Configure Vxlan Access Port Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_vlan_control_port_set(unit, port_acc, bcmVlanTranslateIngressEnable, 1));
    printf("We are using loopback. To avoid learned network VP being overwritten, disable learning on access port. It's for test purpose\n");
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, port_acc, BCM_PORT_LEARN_FWD));
    BCM_IF_ERROR_RETURN(bcm_port_tpid_set(unit, port_acc, 0x8100));

    BCM_IF_ERROR_RETURN(bcm_port_class_set(unit, port_acc, bcmPortClassIngress, port_class));
    return BCM_E_NONE;
}

/* vxlan configuration specific to network physical port */
int
vxlan_network_port_config(int unit, bcm_gport_t port_net)
{
    printf("Configure Vxlan Network Port Configuration.\n");
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_net, bcmPortControlVxlanEnable, 1));
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_net, bcmPortControlVxlanTunnelbasedVnId, 0));
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, port_net, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
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

    rv = vxlan_access_port_config(unit, phy_port, vxlan_acc->port_class);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vxlan_cfg->vxlan_vpn, vxlan_acc->phy_Gport, 0));

    printf("Create Access Flow Port (VP).\n");
    rv = create_vxlan_vp(unit, vxlan_cfg->vxlan_vpn, 0, acc_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access vp: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    /* VPN assignment, port_group + outer_VID -> VPN*/
    bcm_flow_match_config_t_init(&match_info);
    match_info.criteria = BCM_FLOW_MATCH_CRITERIA_PORT_GROUP_VLAN;
    match_info.valid_elements = BCM_FLOW_MATCH_PORT_VALID|BCM_FLOW_MATCH_VLAN_VALID;
    match_info.port = vxlan_acc->port_class;
    match_info.vlan = vxlan_acc->acc_vlan;
    match_info.valid_elements |= BCM_FLOW_MATCH_VPN_VALID;
    match_info.vpn = vxlan_cfg->vxlan_vpn;
    BCM_IF_ERROR_RETURN(bcm_flow_match_add(unit, &match_info, 0, NULL));

    printf("Access: Add flow match (match: port criteria).\n");
    bcm_flow_match_config_t_init(&match_info);
    match_info.criteria  = BCM_FLOW_MATCH_CRITERIA_PORT;
    match_info.port      = vxlan_acc->phy_Gport;
    match_info.flow_port = *acc_flow_port;
    match_info.valid_elements = (BCM_FLOW_MATCH_PORT_VALID |
                                 BCM_FLOW_MATCH_FLOW_PORT_VALID);
    rv = vxlan_flow_match_add(unit, match_info);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring access flow match: %s.\n", bcm_errmsg(rv));
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
        BCM_FLOW_TUNNEL_INIT_TTL_VALID;
    tiinfo.flow_port = net_flow_port;
    tiinfo.type = bcmTunnelTypeVxlan;
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
config_vxlan_network(int unit, vxlan_cfg_t *vxlan_cfg, vxlan_net_cfg_t *vxlan_net, bcm_gport_t *net_flow_port, uint32 same_vp, bcm_flow_handle_t flow_handle)
{
    /* Network Configuartion variables */
    bcm_error_t rv = BCM_E_NONE;
    bcm_if_t      net_intf_id;
    bcm_if_t      net_egr_id;
    bcm_flow_port_encap_t        peinfo;
    bcm_flow_match_config_t      match_info;
    bcm_flow_encap_config_t      einfo;
    bcm_port_t                  phy_port;

    if (same_vp != 1) {
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

        rv = vxlan_network_port_config(unit, phy_port);
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

        printf("Create network L3 Egress object.\n");
        rv = vxlan_create_egr_obj(unit, BCM_L3_FLAGS2_UNDERLAY, net_intf_id, vxlan_net->net_remote_mac,
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
        printf("Network: Set Encapsulation. VP=>egress-obj{Device,Port}.\n");
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
    }

    BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vxlan_cfg->vxlan_vpn, vxlan_net->phy_Gport, 0));

    printf("Network: encap add, VFI-->VNID\n");
    bcm_flow_encap_config_t_init(&einfo);
    einfo.flow_handle = flow_handle;
    einfo.criteria = BCM_FLOW_ENCAP_CRITERIA_VFI;
    einfo.vnid = vxlan_cfg->vxlan_vnid;
    einfo.vpn  = vxlan_cfg->vxlan_vpn;
    einfo.valid_elements = BCM_FLOW_ENCAP_VNID_VALID |
        BCM_FLOW_ENCAP_VPN_VALID;

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
    match_info.vnid           = vxlan_cfg->vxlan_vnid;
    match_info.valid_elements = BCM_FLOW_MATCH_VPN_VALID |
                                BCM_FLOW_MATCH_VNID_VALID;
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
    vxlan_cfg_t vxlan_cfg_1 = {
        0x8064,
        0x12345,     /* vnid */
        0
    };

    vxlan_cfg_t vxlan_cfg_2 = {
        0x8074,
        0x53535,     /* vnid */
        0
    };

    vxlan_acc_cfg_t vxlan_acc_1 = {
        egress_phy_gport_1, /* port */
        21,/*vlan */
        "00:00:00:00:11:11",/*local mac */
        "00:00:00:00:00:01",/* remote mac */
        33
    };

    
    vxlan_acc_cfg_t vxlan_acc_2 = {
        egress_phy_gport_2, /* port */
        23,/*vlan */
        "00:00:00:00:11:12",/*local mac */
        "00:00:00:00:00:02",/* remote mac */
        34
    };


    vxlan_net_cfg_t vxlan_net_1 = {
        ingress_phy_gport_1, /* port */
        22, /* vlan */
        "00:00:00:00:22:22",/* local mac */
        "00:00:00:00:00:02",/* remote mac */
        0xC0A80101,  /*192.168.1.1 remote IP*/
        0x0a0a0a01  /*10.10.10.1 local IP*/
    };


    vxlan_net_cfg_t vxlan_net_2 = {
        ingress_phy_gport_2, /* port */
        24, /* vlan */
        "00:00:00:00:22:33",/* local mac */
        "00:00:00:00:00:03",/* remote mac */
        0xC0A80202,  /*192.168.2.2 remote IP*/
        0x14141401  /*20.20.20.1 local IP*/
    };

    /* flow API */
    bcm_flow_handle_t  flow_handle;
    bcm_gport_t   acc_flow_port_1;
    bcm_gport_t   acc_flow_port_2;
    bcm_gport_t   net_flow_port_1;
    bcm_gport_t   net_flow_port_2;

    printf("Get flow-handler for Classic_Vxlan\n");
    rv = bcm_flow_handle_get(unit, "CLASSIC_VXLAN", &flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in getting flow handle: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vxlan_vpn(unit, &vxlan_cfg_1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vpn create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vxlan_vpn(unit, &vxlan_cfg_2);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vpn create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\nSetup Access port %d\n", egress_port_1);
    rv = config_vxlan_access(unit, &vxlan_cfg_1, &vxlan_acc_1, &acc_flow_port_1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\nSetup Access port %d\n", egress_port_2);
    rv = config_vxlan_access(unit, &vxlan_cfg_2, &vxlan_acc_2, &acc_flow_port_2);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /*create net_flow_port_1 with vnid 0x12345*/
    rv = config_vxlan_network(unit, &vxlan_cfg_1, &vxlan_net_1, &net_flow_port_1, 0, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /*create vxlan tunnel with vp net_flow_port_1*/
    rv = config_vxlan_tunnel(unit, &vxlan_cfg_1, &vxlan_net_1, net_flow_port_1, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in tunnel config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /*create net_flow_port_2 with vnid 0x53535*/
    rv = config_vxlan_network(unit, &vxlan_cfg_2, &vxlan_net_2, &net_flow_port_2, 0, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /*create vxlan tunnel with vp net_flow_port_2*/
    rv = config_vxlan_tunnel(unit, &vxlan_cfg_2, &vxlan_net_2, net_flow_port_2, flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in tunnel config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Network-to-Access: Add payload-dest-mac Address => Access-flow-port (VP).\n");
    rv = add_to_l2_table(unit, "00:00:00:00:bb:bb", vxlan_cfg_1.vxlan_vpn, egress_port_1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in l2 entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Network-to-Access: Add payload-dest-mac Address => Access-flow-port (VP).\n");
    rv = add_to_l2_table(unit, "00:00:00:00:00:dd", vxlan_cfg_2.vxlan_vpn, egress_port_2);
    if (BCM_FAILURE(rv)) {
        printf("\nError in l2 entry config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_l2_station_t l2_station;
    int station_id;
    /* MY_STATION Termination */
    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, vxlan_net_1.net_local_mac, sizeof(vxlan_net_1.net_local_mac));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    print bcm_l2_station_add(unit, &station_id, &l2_station);

    /* MY_STATION Termination */
    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, vxlan_net_2.net_local_mac, sizeof(vxlan_net_2.net_local_mac));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    print bcm_l2_station_add(unit, &station_id, &l2_station);
    
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
    int user_data = 0;
    bcm_flow_vpn_traverse(unit,vpn_traverse,&user_data);
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
