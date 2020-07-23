/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : Vxlan Riot, IPv4 network to access tunnel termination unicast flow, VFI is derived from {SVP+VNID}
 *            VxLAN GBP-ID validation by GBP-DID derived through L3 host entry
 *
 * Usage    : BCM.0> cint td4_hsdk_vxlan_gbp_ipv4_NtoA_uc_l3.c
 *
 * config   : bcm56880_a0-generic-32x400.config_em.yml
 *
 * Log file : td4_hsdk_vxlan_gbp_ipv4_NtoA_uc_l3_log.txt
 *
 * Test Topology :
 *
 *                                  +----------------------+
 *                                  |                      |   VLAN-11        Underlay-NextHop
 *                                  |                      | ingress_port       +---+
 *   DMAC 00::00:00:01              |                      +<-------------------+   |
 *            +---+          VLAN-10|                      |00::03:01           +---+
 *   Host(acc)|   +-----------------+      Trident-4       |                    00::01:01
 *            +---+     00::22:22:22|                      |
 *                                  |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  |                      |
 *                                  +----------------------+
 *
 * Summary:
 * 
 * Cint example to demonstrate VxLAN Riot configuration for tunnel termination -
 *             - network to access(decap flow)
 *             - GBP-DID assigned by L3 host entry through class ID
 *             - Incoming packet's GBP-SID and DID matching done by FP
 *             - Packet processed further only if match occurs, else dropped
 *
 * Detailed steps done in the CINT script:
 * 
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   
 *     a) Selects ports for network side and for access side
 *        and configure them in Loopback mode.
 *
 *     b) Install an IFP rule to copy ingress & egress packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Configuration (Done in config_vxlan_ipv4_vtep()):
 *   
 *     a) Configure a VxLAN Riot configuration.
 *        Setup network port for routing into vxlan tunnel.
 *        This does the necessary configurations of vxlan global settings, vlan,
 *        access and network port setting, tunnel setup, interface and next hop.
 *     b) The GBP DID is configured as class ID in L3 host entry which the decap packet hits
 *     c) Exact Match entry is configured to extract GBP-SID in incoming packet and set class ID for IFP
 *     d) IFP policy is configured as two entries. 
 *        If class ID set by EM(GBP-SID) matches class ID set by L3 host(GBP-DID)
 *        then send the decapsulated packet. Else, drop the packet.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   
 *     a) Check the configurations by 'vlan show', 'l2 show',traversing the vpn and vp's
 *
 *     b) Transmit the known unicast VxLAN packet with valid GBP-SID.The contents of the packet
 *        are printed on screen.
 *        As GBP match occurs, decapsulated packet should successfully egress out
 *
 *     b) Transmit the known unicast VxLAN packet with in-valid GBP-SID.The contents of the packet
 *        are printed on screen.
 *        As GBP match doesn't occur, decapsulated packet should be dropped (shouldn't egress out)
 *
 *     c) Expected Result:
 *     
 *       We can see the decapsulated packet on the egress port if it goes out
 *       and 'show c' to check the Tx/Rx packet stats/counters.
 */

/*
Packet:

NETWORK TO ACCESS:-
Decap-case: packet sent from network port to access port 

Ingress Packet#1 (Valid GBP-SID):
======

Header Info
  ###[ Ethernet ]###
    dst= 00:00:00:00:03:01
    src= 00:00:00:00:01:01
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 11
     type= 0x8000
  ###[ IP ]###
     version= 4
     proto= udp
     dst= 10.10.10.1
     src= 20.20.20.1
  ###[ UDP ]###
     sport= 65535
     dport= 8472
  ###[ VXLAN ]###
     flags= Group, Instance
     Group Policy ID = 4011(0x0FAB) <- GBP SID (matching)
     vni= 0x12345
     reserved2= 0x0
  ###[ Ethernet ]###
                 src= 00:00:00:00:04:01
                 dst= 00:00:00:00:02:01
                 type= 0x8100
  ###[ 802.1Q ]###
     vlan= 11
     type= 0x8000
  ###[ IP ]###
        version= 4
        proto= udp
        dst= 30.30.30.1
        src= 40.40.40.1
  ###[ UDP ]###

Ingress Hex Packet:
tx port=2 data=0000000003010000000001018100000B080045000068000040000A113466141414010A0A0A01D23412B500547D8888000FAB0123450000000000040100000000020108004500002E000040000A06E482282828011E1E1E0181815151B1B2B3B4A1A2A3A4500F01F53A9F00001112131415161718191A

Ingress Packet#2 (In-Valid GBP-SID):
======

Header Info
  ###[ Ethernet ]###
    dst= 00:00:00:00:03:01
    src= 00:00:00:00:01:01
    type= 0x8100
  ###[ 802.1Q ]###
     vlan= 11
     type= 0x8000
  ###[ IP ]###
     version= 4
     proto= udp
     dst= 10.10.10.1
     src= 20.20.20.1
  ###[ UDP ]###
     sport= 65535
     dport= 8472
  ###[ VXLAN ]###
     flags= Group, Instance
     Group Policy ID = 3564(0x0DEC) <- GBP SID (non-matching)
     vni= 0x12345
     reserved2= 0x0
  ###[ Ethernet ]###
                 src= 00:00:00:00:04:01
                 dst= 00:00:00:00:02:01
                 type= 0x8100
  ###[ 802.1Q ]###
     vlan= 11
     type= 0x8000
  ###[ IP ]###
        version= 4
        proto= udp
        dst= 30.30.30.1
        src= 40.40.40.1
  ###[ UDP ]###

Ingress Hex Packet:
tx port=2 data=0000000003010000000001018100000B080045000068000040000A113466141414010A0A0A01D23412B500547D8888000DEC0123450000000000040100000000020108004500002E000040000A06E482282828011E1E1E0181815151B1B2B3B4A1A2A3A4500F01F53A9F00001112131415161718191A

Egress Packet:
======
Header Info
  ###[ Ethernet ]###
                 src= 00:00:00:22:22:22
                 dst= 00:00:00:00:00:01
                 type= 0x8100
  ###[ 802.1Q ]###
     vlan= 10
     type= 0x8000
  ###[ IP ]###
        version= 4
        proto= udp
        dst= 30.30.30.1
        src= 40.40.40.1
  ###[ UDP ]###

(NOTE: As the second ingress packet is dropped, there will be only one egress packet)

Packet Dump
data[0000]: {000000000001} {000000222222} 8100 000a
data[0010]: 0800 4500 002e 0000 4000 0906 e582 2828
data[0020]: 2801 1e1e 1e01 8181 5151 b1b2 b3b4 a1a2
data[0030]: a3a4 500f 01f5 3a9f 0000 1112 1314 1516
data[0040]: 1718 191a 

*/

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_gport_t ingress_phy_gport;

bcm_port_t egress_port;
bcm_gport_t egress_phy_gport;

/* struct for Access Port Params for creating access VP*/
struct vxlan_acc_cfg_t {
    bcm_port_t    acc_port;     /*access physical port*/
    bcm_gport_t   phy_Gport;    /*Input : gport of the port/trunk*/
    bcm_vlan_t    acc_vlan;     /*access vlan*/
    bcm_mac_t     acc_ol_mac;   /*payload dst mac*/
    bcm_mac_t     acc_ol_nh_mac;/*payload src mac*/
    bcm_vpn_t     acc_vpn;      /*access vpn*/
    uint32        acc_vpn_vnid; /* dummy for Access side VPN */
    bcm_ip_t      acc_src_ip;   /*payload sip*/
    bcm_ip_t      acc_dest_ip;  /*payload dip*/
    bcm_multicast_t mcast_acc;  /*mcast group*/
    bcm_gport_t   acc_gport;    /* Output:Access VP gport */
};

/* struct for Network Port params for creating network VP*/
struct vxlan_net_cfg_t {
    bcm_port_t    net_port;     /*network physical port*/
    bcm_port_t    phy_Gport;    /*Input : gport of the port/trunk*/
    bcm_vlan_t    net_ol_vlan;  /*overlay payload vlan*/
    bcm_mac_t     net_ol_mac;   /*overlay payload src mac*/
    bcm_mac_t     net_ol_nh_mac; /*overlay payload dst mac*/
    bcm_vlan_t    net_ul_vlan;    /*tunnel vid*/
    bcm_mac_t     net_ul_mac;     /*tunnel src mac*/
    bcm_mac_t     net_ul_nh_mac;  /*tunnel remote mac*/
    bcm_vpn_t     net_vpn;       /*network vpn*/
    uint32        net_vpn_vnid;  /*network tunnel vnid*/
    bcm_ip_t      net_src_ip;  /*tunnel local ip*/
    bcm_ip_t      net_dest_ip; /*tunnel remote ip*/
    bcm_multicast_t mcast_net;  /*mcast group*/
    bcm_gport_t   net_gport;   /*Output:Nw VP gport */
};

/*struct for vxlan setup*/
struct vxlan_cfg_t {
    uint16 udp_src_port;
    uint16 udp_dest_port;
    bcm_vrf_t vrf;  /*routing vrf id*/
    bcm_vpn_t vxlan_vpn;
    uint32    vxlan_vnid;   /*use vnid for vp sharing*/
    bcm_multicast_t mc_group;
    uint16 gbp_id; /*GBP ID to be checked in VxLAN header*/
};

/*
 * This function is written so that hardcoding of port
 * numbers in cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = BCM_E_NONE;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s\n",bcm_errmsg(rv));
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
	bcm_field_group_config_t  group_config;
	bcm_field_aset_t    aset;
	bcm_field_qset_t 	qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_port_t     port;

	bcm_field_group_config_t_init(&group_config); 
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
	BCM_FIELD_ASET_INIT(aset);
	BCM_FIELD_ASET_ADD(aset, bcmFieldActionCopyToCpu);
	
	group_config.qset = qset;
	group_config.aset = aset;
	group_config.mode = bcmFieldGroupModeAuto;
	group_config.priority = 1;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
	group = group_config.group;

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_setup port: %d\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                                  BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,
                                                   BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                                bcmFieldActionCopyToCpu, 0, 0));

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
    bcm_field_group_config_t  group_config;
	bcm_field_aset_t    aset;
	bcm_field_qset_t 	qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_port_t     port;

    bcm_field_group_config_t_init(&group_config); 
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
	BCM_FIELD_ASET_INIT(aset);
	BCM_FIELD_ASET_ADD(aset, bcmFieldActionCopyToCpu);
	BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
	
	group_config.qset = qset;
	group_config.aset = aset;
	group_config.mode = bcmFieldGroupModeAuto;
	group_config.priority = 2;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));
	group = group_config.group;

    BCM_PBMP_ITER(pbm, port) {
        printf("egress_port_multi_setup port: %d\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit,port,
                                                  BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

        BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,
                                                BCM_FIELD_EXACT_MATCH_MASK));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                                bcmFieldActionCopyToCpu, 0, 0));
        BCM_IF_ERROR_RETURN(bcm_field_action_add(unit,entry,bcmFieldActionDrop,
                                                    0, 0));

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
    bcm_pbmp_t     pbmp;
	
    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    egress_port = port_list[0];
    ingress_port = port_list[1];

    if (BCM_E_NONE != bcm_port_gport_get(unit, ingress_port,
                                        &(ingress_phy_gport))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                                            ingress_port);
        return -1;
    }

    if (BCM_E_NONE !=bcm_port_gport_get(unit,egress_port,&(egress_phy_gport))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                                                egress_port);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, pbmp)) {
        printf("ingress_port_multi_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for port %d\n", egress_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");
	
	/* Set default outer tpid per port */
    BCM_IF_ERROR_RETURN(bcm_port_tpid_set(unit, ingress_port, 0x8100));
    BCM_IF_ERROR_RETURN(bcm_port_tpid_set(unit, egress_port, 0x8100));
    /* Set port learning */
    BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, ingress_port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
	BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));

    return BCM_E_NONE;
}

/* verifying the VPN configuration */
bcm_error_t
show_vxlan_vtep_config(int unit)
{
	int userdata= 0x56880;
	
    bcm_flow_vpn_traverse(unit,vpn_traverse,&userdata);
    return BCM_E_NONE;
}

/*Verify function call*/
void
verify(int unit)
{
    int i = 0;
    char str[512];
    char pkt_data[2][512];

    /* Packet with Valid GBP-ID */
    snprintf(pkt_data[0],512,"%s","0000000003010000000001018100000B080045000068000040000A113466141414010A0A0A01D23412B500547D8888000FAB0123450000000000040100000000020108004500002E000040000A06E482282828011E1E1E0181815151B1B2B3B4A1A2A3A4500F01F53A9F00001112131415161718191A");

    /* Packet with In-valid GBP-ID */
    snprintf(pkt_data[1],512,"%s","0000000003010000000001018100000B080045000068000040000A113466141414010A0A0A01D23412B500547D8888000DEC0123450000000000040100000000020108004500002E000040000A06E482282828011E1E1E0181815151B1B2B3B4A1A2A3A4500F01F53A9F00001112131415161718191A");

    bshell(unit, "vlan show");
    bshell(unit, "l2 show");
    bshell(unit, "l3 host show");
    bshell(unit, "l3 egress show");
    bshell(unit, "l3 intf show");
    bshell(unit, "fp show");
    bshell(unit, "clear c");

    /* Packet 1: With Valid GBP-SID. Should be processed */
    printf("\nSending VxLAN packet with Valid GBP-SID to ingress_port :%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 5", ingress_port,pkt_data[0]);
    bshell(unit, str);
    bshell(unit, "show counters");
    bshell(unit, "clear c");

    /* Packet 2: With Invalid GBP-SID. Should be dropped */
    printf("\nSending VxLAN packet with Invalid GBP-SID to ingress_port :%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x%s; sleep quiet 5", ingress_port,pkt_data[1]);
    bshell(unit, str);
    bshell(unit, "show counters");
    bshell(unit, "clear c");

    printf("\nverify l2 learning:\n");
    bshell(unit, "l2 show");
    printf("\nverify l3 route\n");
    bshell(unit, "l3 host show");

    /* Dump the Configured VPN & VP's*/
    show_vxlan_vtep_config(unit);

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

/* vxlan configuration specific to network physical port */
bcm_error_t
vxlan_network_port_config(int unit, bcm_gport_t gport_net)
{
    printf("Configure Vxlan network port configuration\n");
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_net,
                            bcmPortControlVxlanEnable, 1));
	
	/* When bcmPortControlVxlanTunnelbasedVnId = 1, {SVP + VNID} -> VFI */
    BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, gport_net,
                             bcmPortControlVxlanTunnelbasedVnId, 1));
    return BCM_E_NONE;
}

/* Create the VPN for vxlan */
bcm_error_t
create_vxlan_vpn(int unit,bcm_multicast_t *vxlan_mc_group,bcm_vpn_t *vxlan_vpn)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_multicast_t mc_group = 0;
    bcm_vpn_t vpn = 0;

    printf("create multicast group for UUC/MC/BC\n");
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_FLOW, &mc_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_create: %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_flow_vpn_config_t vpn_info;
    bcm_flow_vpn_config_t_init (&vpn_info);
    vpn_info.flags = BCM_FLOW_VPN_ELAN;
    vpn_info.broadcast_group         = mc_group;
    vpn_info.unknown_unicast_group   = mc_group;
    vpn_info.unknown_multicast_group = mc_group;
    BCM_IF_ERROR_RETURN(bcm_flow_vpn_create(unit, &vpn, &vpn_info));

    *vxlan_mc_group = mc_group;
    *vxlan_vpn = vpn;

    return BCM_E_NONE;
}

/* Common code for Access/Network VP's */
bcm_error_t
create_vxlan_vp(int unit, bcm_vpn_t vpn, uint32 flags, bcm_gport_t *vp)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_flow_port_t  flow_port;
    bcm_flow_port_t_init(&flow_port);
	flow_port.flags = flags;
    rv = bcm_flow_port_create(unit, vpn, &flow_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_flow_port_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
    *vp = flow_port.flow_port_id;
    return BCM_E_NONE;
}

/* Access Vxlan VP creation */
bcm_error_t
config_vxlan_access(int unit,vxlan_cfg_t *vxlan_cfg,vxlan_acc_cfg_t *vxlan_acc,
                    bcm_gport_t *acc_flow_port)
{
	
    /* Access Configuartion variables */
    bcm_error_t rv = BCM_E_NONE;
	bcm_flow_port_t flow_port;
	bcm_if_t intf_id_acc;
	bcm_l3_intf_t l3_intf;
	bcm_l3_ingress_t l3_ingress;
	bcm_if_t egr_obj_acc;
    bcm_l3_egress_t l3_egress;
    bcm_l3_host_t host;
	
	bcm_if_t ingress_if = 0x1b3;
	
	rv = vlan_create_add_port(unit, vxlan_acc->acc_vlan, vxlan_acc->acc_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in Access:vlan_create_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }
	
	/* Set port learning */
	rv = bcm_port_learn_set(unit, vxlan_acc->acc_port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("Error in Access:bcm_port_learn_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
	
	printf("Access: create VPN\n");
    rv = create_vxlan_vpn(unit,vxlan_acc->mcast_acc,vxlan_acc->acc_vpn);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Access:create_vxlan_vpn: %s\n", bcm_errmsg(rv));
        return rv;
    }
    	
    printf("Access: create flow port vp\n");
    rv = create_vxlan_vp(unit, vxlan_acc->acc_vpn, 0, acc_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Access:create_vxlan_vp: %s\n", bcm_errmsg(rv));
        return rv;
    }
    vxlan_acc->acc_gport=(*acc_flow_port);
    printf("Access: flow port=0x%x\n",vxlan_acc->acc_gport);
	
    /* Create Access L3 interfacec */
	bcm_l3_intf_t_init(&l3_intf);
	sal_memcpy(l3_intf.l3a_mac_addr, vxlan_acc->acc_ol_mac, 6);
	l3_intf.l3a_vid = vxlan_acc->acc_vlan;
	l3_intf.l3a_mtu = 1500;
	rv = bcm_l3_intf_create(0, &l3_intf);
	if (BCM_FAILURE(rv)) {
        printf("\nError in Access:bcm_l3_intf_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
	printf("Access: egr_l3_intf: 0x%x\n",l3_intf.l3a_intf_id);
	intf_id_acc = l3_intf.l3a_intf_id;

	/* Create L3 ingress interface */
	bcm_l3_ingress_t_init(&l3_ingress);
	l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
	l3_ingress.vrf = vxlan_cfg->vrf;
	l3_ingress.ipmc_intf_id  = intf_id_acc;
	rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);
	if (BCM_FAILURE(rv)) {
        printf("\nError in Access:bcm_l3_ingress_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
	printf("Access: L3 ingress interface: 0x%x\n",ingress_if);

	/* Create L3 egress object */
	bcm_l3_egress_t_init(&l3_egress);
	sal_memcpy(l3_egress.mac_addr, vxlan_acc->acc_ol_nh_mac, 6);
	l3_egress.port = vxlan_acc->acc_port;
	l3_egress.intf = intf_id_acc;
	rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_acc);
	if (BCM_FAILURE(rv)) {
        printf("\nError in Access:bcm_l3_egress_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
	printf("Access: Egress obj access: 0x%x\n",egr_obj_acc);

	/* Add IP address to host table */
	bcm_l3_host_t_init(&host);
	host.l3a_intf = egr_obj_acc;
	host.l3a_ip_addr = vxlan_acc->acc_src_ip;
	host.l3a_vrf = vxlan_cfg->vrf;
    host.l3a_lookup_class = vxlan_cfg->gbp_id; /*Add GBP-ID as Destination Class (DID)*/
	rv = bcm_l3_host_add(unit, &host);
	if (BCM_FAILURE(rv)) {
        printf("\nError in Access:bcm_l3_host_add: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Add L3 Host entry for RIOT\n");
	
    return BCM_E_NONE;
}

/* Tunnel creation and associating it with Nw VP */
bcm_error_t
config_vxlan_tunnel(int unit,vxlan_cfg_t *vxlan_cfg, vxlan_net_cfg_t *vxlan_net,
                    bcm_gport_t net_flow_port, bcm_flow_handle_t flow_handle)
{
    /* Tunnel Configuration variables */
    bcm_error_t rv = BCM_E_NONE;
    bcm_flow_tunnel_initiator_t  tiinfo;
    bcm_flow_tunnel_terminator_t ttinfo;

    printf("Tunnel_init: create flow tunnel initiator\n");
    bcm_flow_tunnel_initiator_t_init(&tiinfo);
    tiinfo.flow_handle = flow_handle;
    tiinfo.valid_elements = BCM_FLOW_TUNNEL_INIT_DIP_VALID |
        BCM_FLOW_TUNNEL_INIT_SIP_VALID |
        BCM_FLOW_TUNNEL_INIT_TTL_VALID;
    tiinfo.flow_port = net_flow_port;
    tiinfo.type = bcmTunnelTypeVxlan;
    tiinfo.sip = vxlan_net->net_src_ip;
    tiinfo.dip = vxlan_net->net_dest_ip;
    tiinfo.ttl = 0x3f;
    rv = bcm_flow_tunnel_initiator_create(0,&tiinfo,0,NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_flow_tunnel_initiator_create: %s\n",
                                                             bcm_errmsg(rv));
        return rv;
    }
	
    printf("Tunnel_term: create flow tunnel terminator\n");
    bcm_flow_tunnel_terminator_t_init(&ttinfo);
    ttinfo.flow_handle = flow_handle;
    ttinfo.valid_elements = BCM_FLOW_TUNNEL_TERM_DIP_VALID;
    ttinfo.dip = vxlan_net->net_src_ip;
    ttinfo.type = bcmTunnelTypeVxlan;
    rv = bcm_flow_tunnel_terminator_create(unit,&ttinfo,0,NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_flow_tunnel_terminator_create: %s\n",
                                                                bcm_errmsg(rv));
        return rv;
    }
	
    return BCM_E_NONE;
}

/* vxlan network VP creation . tunnel is not created */
bcm_error_t
config_vxlan_network(int unit,vxlan_cfg_t *vxlan_cfg,vxlan_acc_cfg_t *vxlan_acc,
                     vxlan_net_cfg_t *vxlan_net,bcm_gport_t *net_flow_port,
                     bcm_flow_handle_t flow_handle)
{
    /* Network Configuartion variables */
    bcm_error_t rv = BCM_E_NONE;
	bcm_l2_addr_t l2addr;
	bcm_l2_station_t l2_station;
	int station_id;
	bcm_flow_match_config_t info;
	bcm_flow_port_t flow_port;
	bcm_l3_intf_t l3_intf;
	bcm_if_t intf_id_net;
	bcm_l3_ingress_t l3_ingress;
	bcm_if_t egr_obj_net;
    bcm_l3_egress_t l3_egress;
	
	bcm_if_t ingress_if = 0x1b0;

    /*Set VxLAN UDP dest port system wide*/
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchVxlanUdpDestPortSet, vxlan_cfg->udp_dest_port));
    
    rv = vlan_create_add_port(unit,vxlan_net->net_ol_vlan,vxlan_net->net_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Network:vlan_create_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

	/* Set port learning */
	print bcm_port_learn_set(unit, vxlan_net->net_port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);

	/* VXLAN enable & VFI assignment KEY selection */
    rv = vxlan_network_port_config(unit, vxlan_net->net_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Network:vxlan_network_port_config: %s\n", bcm_errmsg(rv));
        return rv;
    }
	
	printf("Access: create VPN\n");
    rv = create_vxlan_vpn(unit,vxlan_net->mcast_net,vxlan_net->net_vpn);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Network:create_vxlan_vpn: %s\n", bcm_errmsg(rv));
        return rv;
    }
    
	printf("Network: create flow port vp\n");
    rv = create_vxlan_vp(unit,vxlan_net->net_vpn,BCM_FLOW_PORT_NETWORK,
                         net_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Network:create_vxlan_vp: %s\n", bcm_errmsg(rv));
        return rv;
    }
    vxlan_net->net_gport=*net_flow_port;
    printf("Network: flow port=0x%x\n",vxlan_net->net_gport);

	/* Disable port learning. Bind nexthop to VP before enabling learning */
	print bcm_port_learn_set(unit, *net_flow_port, BCM_PORT_LEARN_FWD);
	
	bcm_l2_station_t_init(&l2_station);
	sal_memcpy(l2_station.dst_mac, vxlan_net->net_ul_mac, 6);
	l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
	rv = bcm_l2_station_add(unit, &station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Network:bcm_l2_station_add: %s\n", bcm_errmsg(rv));
        return rv;
    }
	
	/* SVP assignment */
	bcm_flow_match_config_t_init(&info);
	info.flow_handle = flow_handle;
	info.criteria = BCM_FLOW_MATCH_CRITERIA_SIP;
	info.flow_port = *net_flow_port;
	info.sip = vxlan_net->net_dest_ip;
	info.valid_elements = BCM_FLOW_MATCH_SIP_VALID |
						  BCM_FLOW_MATCH_FLOW_PORT_VALID;
	rv = bcm_flow_match_add(unit, &info, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Network:bcm_flow_match_add: %s\n", bcm_errmsg(rv));
        return rv;
    }
	
	/* VFI assignment: {SVP + VNID} -> VFI */
	printf("Network: configure match condition to derive VPN (Match:{VN_ID + SVP},Derive: VFI (VPN))\n");
	bcm_flow_match_config_t_init(&info);
	info.flow_handle = flow_handle;
	info.criteria = BCM_FLOW_MATCH_CRITERIA_SVP_VNID;
	info.vpn = 0x7000 + vxlan_net->net_ol_vlan;
	info.flow_port = *net_flow_port;
	info.vnid = vxlan_net->net_vpn_vnid;
	info.valid_elements = BCM_FLOW_MATCH_VPN_VALID |
						  BCM_FLOW_MATCH_FLOW_PORT_VALID |
						  BCM_FLOW_MATCH_VNID_VALID;
	rv = bcm_flow_match_add(unit, &info, 0, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Network:bcm_flow_match_add: %s\n", bcm_errmsg(rv));
        return rv;
    }
	
	bcm_l2_addr_t_init(&l2addr, vxlan_net->net_ol_mac, vxlan_net->net_ol_vlan);
	l2addr.flags |= BCM_L2_STATIC | BCM_L2_L3LOOKUP;
	rv = bcm_l2_addr_add(unit, &l2addr);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Network:bcm_l2_addr_add: %s\n", bcm_errmsg(rv));
        return rv;
    }
	
    /* Create Network L3 interface */
	bcm_l3_intf_t_init(&l3_intf);
	sal_memcpy(l3_intf.l3a_mac_addr, vxlan_net->net_ol_mac, 6);
	l3_intf.l3a_vid = vxlan_net->net_ol_vlan;
	l3_intf.l3a_mtu = 1500;
	rv = bcm_l3_intf_create(0, &l3_intf);
	if (BCM_FAILURE(rv)) {
        printf("\nError in Network:bcm_l3_intf_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
	printf("Network: Network egr_l3_intf: 0x%x\n",l3_intf.l3a_intf_id);
	intf_id_net = l3_intf.l3a_intf_id;
	
    /* Create L3 ingress interface */
	bcm_l3_ingress_t_init(&l3_ingress);
	l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
	l3_ingress.vrf = vxlan_cfg->vrf;
	l3_ingress.ipmc_intf_id  = intf_id_net;
	rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);
	if (BCM_FAILURE(rv)) {
        printf("\nError in Network:bcm_l3_ingress_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
	printf("Network: L3 ingress interface: 0x%x\n",ingress_if);

	/* Create L3 egress object */
	bcm_l3_egress_t_init(&l3_egress);
	sal_memcpy(l3_egress.mac_addr, vxlan_net->net_ol_nh_mac, 6);
	l3_egress.port = vxlan_net->net_port;
	l3_egress.intf = intf_id_net;
	rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_net);
	if (BCM_FAILURE(rv)) {
        printf("\nError in Network:bcm_l3_egress_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
	printf("Network: Egress obj access: 0x%x\n",egr_obj_net);

	print bcm_port_control_set(unit, vxlan_net->net_port, bcmPortControlL3Ingress, ingress_if); 

	bcm_vlan_control_vlan_t vlan_ctrl;
	bcm_vlan_control_vlan_t_init(&vlan_ctrl);
	vlan_ctrl.ingress_if = ingress_if;
	print bcm_vlan_control_vlan_selective_set(unit,vxlan_net->net_ol_vlan, BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK, &vlan_ctrl);

    return BCM_E_NONE;
}

/* Exact Match FP entry
- Matches Packets with valid GBP-SID in VxLAN header
- Action is "Set Class ID" used by IFP for SID-DID Matching
*/
bcm_error_t
config_em_fp(int unit, uint16 gbp_sid) {
        bcm_field_group_config_t group_config;
    int entry_id;

    uint8 first_byte_of_gbp_sid = (gbp_sid >> 8) & 0xFF;
    uint8 second_byte_of_gbp_sid = gbp_sid & 0xFF;

    bcm_field_group_config_t_init(&group_config);

    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_ASET_INIT(group_config.aset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngressExactMatch);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifyOuterL5PayloadByte3);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifyOuterL5PayloadByte4);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionAssignOpaqueObject0);

    group_config.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    group_config.priority = 10;
    group_config.mode= bcmFieldGroupModeAuto;
    print (bcm_field_group_config_create(unit, &group_config));

    /* Entry-1: No OP entry */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry_id));
    print (bcm_field_qualify_OuterL5PayloadByte3(unit, entry_id, first_byte_of_gbp_sid, 0xFF)); /* Qualify GBP ID first byte*/
    BCM_IF_ERROR_RETURN(bcm_field_qualify_OuterL5PayloadByte4(unit, entry_id, second_byte_of_gbp_sid, 0xFF)); /* Qualify GBP ID second byte*/

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_id, bcmFieldActionAssignOpaqueObject0, gbp_sid, 0)); /* Assign Class ID for IFP */

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_id));

    return BCM_E_NONE;
}

/* FP Group that has two entries:
Entry 1:
 - Matches class ID set by EM based on GBP-SID
 - Matches class ID set by L3 table : GBP-DID
   If both match, Action is "No operation"
Entry 2:
- Default entry to catch GBP-ID mismatches
- Qualifies InPort
- Action is "Drop"
*/
bcm_error_t
config_gbp_ifp_policy(int unit, bcm_port_t inport, uint16 gbp_sid, uint16 gbp_did) {
    bcm_field_group_config_t group_config;
    int entry_id;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_ASET_INIT(group_config.aset);

    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifyExactMatchOpaqueObject0);

    BCM_FIELD_QSET_ADD(group_config.qset,bcmFieldQualifyDstClassL3);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);

    group_config.flags= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    group_config.priority = 10;
    group_config.mode= bcmFieldGroupModeAuto;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    /* Entry-1: No OP entry */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry_id));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry_id, inport, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_ExactMatchOpaqueObject0(unit, entry_id, gbp_sid, 0xFFFF)); /* Qualify EM Class ID */
    BCM_IF_ERROR_RETURN(bcm_field_qualify_DstClassL3(unit, entry_id, gbp_did, 0xFFFF)); /* Qualify L3 Dest class ID */

    /*Action: NO-OP*/

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_id));

    /* Entry-2: Default Drop entry */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry_id));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry_id, inport, BCM_FIELD_EXACT_MATCH_MASK));

    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry_id, bcmFieldActionDrop, 0, 0)); /* Drop */

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry_id));

    return BCM_E_NONE;
}
 
/* vxlan Configuration function */
bcm_error_t
config_vxlan_ipv4_vtep(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* VPN Configuration variables */
    vxlan_cfg_t vxlan_cfg = {
        0xFFFF, /*udp_src_port*/
        0x12B5, /*udp_dest_port*/
        10,      /*vrf*/
        0,      /*vxlan_vpn*/
        0,      /*vxlan_vnid*/
        0,      /*mc_group*/
        0xFAB   /*VxLAN GBP ID*/
    };

    vxlan_acc_cfg_t vxlan_acc = {
        egress_port,            /*access physical port*/
        egress_phy_gport,       /*gport of the port/trunk*/
        10,                      /*access vlan*/
        "00:00:00:22:22:22",     /*payload dst mac*/
        "00:00:00:00:00:01",     /*payload src mac*/
        0,                       /*access vpn*/
        0,                       /* dummy for Access side VPN */
        0x1e1e1e01,              /*payload sip*/
        0x28282801,              /*payload dip*/
        0,                       /*mcast group*/
        0                        /* Output:Access VP gport */
    };

    vxlan_net_cfg_t vxlan_net = {
        ingress_port,             /*network physical port*/
        ingress_phy_gport,        /*gport of the port/trunk*/
        11,                      /*overlay payload vlan*/
        "00:00:00:00:04:01",     /*overlay payload src mac*/
        "00:00:00:00:02:01",     /*overlay payload dst mac*/
        30,                      /*tunnel vid*/
        "00:00:00:00:03:01",     /*tunnel src mac*/
        "00:00:00:00:01:01",     /*tunnel remote mac*/
        0,                       /*network vpn*/
        0x12345,                 /*network tunnel vnid*/
        0x0a0a0a01,              /*tunnel local ip*/
        0x14141401,              /*tunnel remote ip*/
        0,                       /*mcast group*/
        0                        /*Output:Nw VP gport */
    };

    /* flow API */
    bcm_flow_handle_t  flow_handle;
    bcm_gport_t   acc_flow_port;
    bcm_gport_t   net_flow_port;
	
    printf("Get flow handler for CLASSIC_VXLAN\n");
    rv = bcm_flow_handle_get(unit, "CLASSIC_VXLAN", &flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_flow_handle_get: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\nSetup access VP on port %d\n", vxlan_acc.acc_port);
    rv = config_vxlan_access(unit, &vxlan_cfg, &vxlan_acc, &acc_flow_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_vxlan_access: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*create network flow port with vnid 0x12345*/
    printf("\nSetup network VP on port %d\n", vxlan_net.net_port);
    rv = config_vxlan_network(unit,&vxlan_cfg,&vxlan_acc,&vxlan_net,
                              &net_flow_port,flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_vxlan_network: %s\n", bcm_errmsg(rv));
        return rv;
    }
	
    /*create vxlan tunnel with network flow port*/
    rv = config_vxlan_tunnel(unit,&vxlan_cfg,&vxlan_net,net_flow_port,
                             flow_handle);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_vxlan_tunnel: %s\n", bcm_errmsg(rv));
        return rv;
    }
	
    /*Create EM entry that matches GBP-ID and sets a class ID*/
    rv = config_em_fp(unit,vxlan_cfg.gbp_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring Exact Match entry: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*Create IFP policy to allow packets that have GBP (SID == DID) */
    rv = config_gbp_ifp_policy(unit, ingress_port, vxlan_cfg.gbp_id, vxlan_cfg.gbp_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring GBP IFP policy: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/* verifying the VPN VP configuration using traverse*/
bcm_error_t
vpn_traverse(int unit,bcm_vpn_t vpn, bcm_flow_vpn_config_t *info, void *data)
{
    int maxArr=20;
    int count=0,iter=0;
    bcm_flow_port_t flow_portArr[maxArr];

    printf("\n Vpn :%d \n",vpn);
    printf("\n VpnInfo : \n");
    print *info;
    BCM_IF_ERROR_RETURN(bcm_flow_port_get_all(unit,vpn,maxArr,
                                                flow_portArr,&count));

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
 *  c) demonstrates the functionality(done in verify())
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
