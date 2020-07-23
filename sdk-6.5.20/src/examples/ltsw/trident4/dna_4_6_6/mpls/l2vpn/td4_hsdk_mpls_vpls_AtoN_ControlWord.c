/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : TD4(56880) VPLS (L2 MPLS) tunnel initiation flow with control
 *            word (BCM_MPLS_PORT_CONTROL_WORD)
 *
 * Usage    : BCM.0> cint td4_hsdk_mpls_vpls_AtoN_ControlWord.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : td4_hsdk_mpls_vpls_AtoN_ControlWord_log.txt
 *
 * Test Topology :
 *
 *                    +-------------------+
 *                    |                   |
 *      VLAN-11       |                   |     VLAN-22
 *   ing_acc_port_1   |                   |  egr_net_port_3
 *   ---------------->+                   +---------------->
 *   Ingress packet   |                   |  Egress with MPLS tunnel header
 *                    |                   |
 *                    |     Trident-4     |
 *      VLAN-12       |                   |     VLAN-23
 *   egr_acc_port_2   |                   |  egr_net_port_4
 *   <----------------+                   +---------------->
 *   Egress packet    |                   |  Egress with MPLS tunnel header
 *                    |                   |
 *                    |                   |
 *                    +-------------------+
 *
 *   Unicast entry:
 *                L2 add ingress-dest-mac => Network VPort 1 (egr_net_port_3)
 *
 *   UUC/UMC/BC multicast Group :
 *                Access port 1  - (ing_acc_port_1)
 *                Access port 2  - (egr_acc_port_2)
 *                Network port 1 - (egr_net_port_3)
 *                Network port 2 - (egr_net_port_4)
 *
 * Summary:
 * ========
 *   Cint script to demonstrate VPLS (L2 MPLS) tunnel initiation with control
 *   word.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *      a) Selects four ports and configure them in Loopback mode. Out of these
 *         four ports, one port is used as ingress port and the others as
 *         egress ports.
 *
 *      b) Install an IFP rule to copy ingress & egress packets to CPU and
 *         start packet watcher.
 *
 *      Note: IFP rule is meant for a verification purpose and is not part of
 *            the feature configuration.
 *
 *   2) Step2 - Configuration (Done in):
 *   ===================================
 *      a) Ports are configured in the specific vlan/vfi
 *
 *      b) Configure VPLS tunnel initiation/termination scenario with
 *         necessary configurations.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *      a) Check the configurations by 'vlan show', 'l3 intf show',
 *         'l3 egress show' and 'multicast show'.
 *
 *      b) Transmit packets with VLAN-11 on ing_acc_port_1.
 *         The contents of the packet are printed on screen.
 *
 *      c) Expected Result:
 *      ===================
 *         We can see that customer packet egress as VPLS tunnel packet on
 *         network port with control word 00000000 (4-byte).
 *         'show c' & bcm_stat_get used to check the Tx/Rx packet
 *         stats/counters.
 *
 *   Ingress Packets : ing_acc_port_1
 *   0000 0000 0022 0000 0000 0011 8100 000b
 *   0800 4500 0036 0000 0000 40ff 65ac 0a00
 *   0014 0a00 000a 0001 0203 0405 0607 0809
 *   0a0b 0c0d 0e0f 1011 1213 1415 1617 1819
 *   1a1b 1c1d 1e1f 2021 2394 12f0
 *
 *   Egress Packet : egr_net_port_3
 *   0000 0000 00aa 0000 0000 00bb 8100 0016
 *   8847 0011 1016 0044 4116 0000 0000 0000
 *   0000 0022 0000 0000 0011 8100 000b 0800
 *   4500 0036 0000 0000 40ff 65ac 0a00 0014
 *   0a00 000a 0001 0203 0405 0607 0809 0a0b
 *   0c0d 0e0f 1011 1213 1415 1617 1819 1a1b
 *   1c1d 1e1f 2021 2394 12f0
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t    ing_acc_port_1;
bcm_port_t    egr_acc_port_2;
bcm_port_t    egr_net_port_3;
bcm_port_t    egr_net_port_4;
bcm_gport_t   ing_acc_gport_1;
bcm_gport_t   egr_acc_gport_2;
bcm_gport_t   egr_net_gport_3;
bcm_gport_t   egr_net_gport_4;

struct l2vpn_cfg_t {
	bcm_vpn_t       mpls_vpn;             /* VPN */
	bcm_multicast_t bcast_group;          /* Multicast group */
};

/* struct for access port params */
struct l2vpn_acc_cfg_t {
	bcm_port_t         acc_port;          /* Access port */
	bcm_gport_t        acc_gport;         /* Access gport */
	bcm_gport_t        acc_vport;         /* Access VP gport */
	bcm_vlan_t         acc_vlan;          /* Access VLAN */
    bcm_vlan_t         acc_inner_vlan;    /* Access inner VLAN */

	bcm_mac_t          host_mac;          /* HOST MAC */
	bcm_mac_t          remote_host_mac;   /* Remote HOST MAC */

    bcm_mpls_port_match_t criteria;       /* Access match criteria */
    int                port_class;        /* Access port class */
};

/* struct for network Port params */
struct l2vpn_net_cfg_t {
	bcm_port_t         net_port;          /* Network port */
	bcm_gport_t        net_gport;         /* Network gport */
	bcm_gport_t        net_vport;         /* Network VP gport */
	bcm_vlan_t         net_vlan;          /* Network VLAN */

	bcm_mac_t          net_local_mac;     /* Outer SRC MAC */
	bcm_mac_t          net_remote_mac;    /* Outer DST MAC */

    bcm_mpls_port_match_t criteria;       /* Network match criteria */
    bcm_mpls_label_t   vc_label_init;     /* VC init label */
    bcm_mpls_label_t   vc_label_term;     /* VC term label */
    bcm_mpls_label_t   tunnel_label_init; /* Tunnel init label */
    bcm_mpls_label_t   tunnel_label_term; /* Tunnel term label */
    bcm_gport_t        tunnel_id;         /* Tunnel ID */
    int                num_labels;        /* No.of labels */
    uint8              ttl;               /* TTL value */
    uint8              exp;               /* EXP value */

    int                port_class;        /* Network port class */
    bcm_switch_network_group_t network_group_id; /* Network group ID */
};

/*
 * This function is written so that hardcoding of port
 * numbers in cint scripts is removed. This function gives
 * required number of ports.
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    bcm_port_config_t  configP;
    bcm_error_t        rv = 0;
    bcm_pbmp_t         ports_pbmp;
    int                i = 0, port = 0;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_config_get: %s.\n",bcm_errmsg(rv));
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
 * Configures the port in loopback mode and install
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_group_config_t    group_config;
    bcm_field_entry_t           entry;
    bcm_port_t                  port;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
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
 * Configures the port in loopback mode and install
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet from the egress port.
 */
bcm_error_t
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_group_t  group;
    bcm_field_entry_t  entry;
    bcm_port_t         port;

    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
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
                                                 bcmFieldActionCopyToCpu,
                                                 0, 0));
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
    int       num_ports = 4;
    int       port_list[num_ports];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ing_acc_port_1 = port_list[0];
    egr_acc_port_2 = port_list[1];
    egr_net_port_3 = port_list[2];
    egr_net_port_4 = port_list[3];

    if (BCM_E_NONE !=bcm_port_gport_get(unit, ing_acc_port_1,
                                        &(ing_acc_gport_1))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                        ing_acc_port_1);
        return -1;
    }

    if (BCM_E_NONE != bcm_port_gport_get(unit, egr_acc_port_2,
                                         &(egr_acc_gport_2))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                         egr_acc_port_2);
        return -1;
    }

    if (BCM_E_NONE != bcm_port_gport_get(unit, egr_net_port_3,
                                         &(egr_net_gport_3))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                         egr_net_port_3);
        return -1;
    }

    if (BCM_E_NONE != bcm_port_gport_get(unit, egr_net_port_4,
                                         &(egr_net_gport_4))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                         egr_net_port_4);
        return -1;
    }

    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ing_acc_port_1);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, pbmp)) {
        printf("ingress_port_multi_setup() failed for port %d\n",
                                         ingress_port);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egr_acc_port_2);
    BCM_PBMP_PORT_ADD(pbmp, egr_net_port_3);
    BCM_PBMP_PORT_ADD(pbmp, egr_net_port_4);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for egress port \n");
        return -1;
    }

    bshell(unit, "pw start report +raw +decode +pmd");

    return BCM_E_NONE;
}

/* Verify function call */
void
verify(int unit)
{
    char      str[512];
    uint64    in_pkt, out_pkt, in_bytes, out_bytes;

    print "vlan show";
    bshell(unit, "vlan show");
    print "l2 show";
    bshell(unit, "l2 show");
    print "l3 intf show";
    bshell(unit, "l3 intf show");
    print "l3 egress show";
    bshell(unit, "l3 egress show");
    print "multicast show";
    bshell(unit, "multicast show");
    bshell(unit, "clear c");

    printf("\nSending packet to ingress port:%d\n", ing_acc_port_1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000220000000000118100000b0800450000360000000040FF65AC0A0000140A00000A000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F2021239412F0; sleep quiet 3", ing_acc_port_1);
    bshell(unit, str);

    print "show c";
    bshell(unit, "show c");

    printf("\n******Port stats check******\n");

    printf("--------Access port (%d) stats--------\n", ing_acc_port_1);
    COMPILER_64_ZERO(in_pkt);
    COMPILER_64_ZERO(in_bytes);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, ing_acc_port_1,
                                     snmpDot1dTpPortInFrames, &in_pkt));
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, ing_acc_port_1,
                                     snmpIfInOctets, &in_bytes));
    printf("Rx : %d packets / %d bytes\n", COMPILER_64_LO(in_pkt),
                                           COMPILER_64_LO(in_bytes));

    printf("--------Access port (%d) stats--------\n", egr_acc_port_2);
    COMPILER_64_ZERO(out_pkt);
    COMPILER_64_ZERO(out_bytes);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egr_acc_port_2,
                                     snmpDot1dTpPortOutFrames, &out_pkt));
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egr_acc_port_2,
                                     snmpIfOutOctets, &out_bytes));
    printf("Tx : %d packets / %d bytes\n", COMPILER_64_LO(out_pkt),
                                           COMPILER_64_LO(out_bytes));
    if (COMPILER_64_LO(out_pkt) != 0) {
        printf("Packet verify failed. Expected out packet 0 but get %d\n",
                                           COMPILER_64_LO(out_pkt));
        return -1;
    }

    printf("--------Network port (%d) stats--------\n", egr_net_port_3);
    COMPILER_64_ZERO(out_pkt);
    COMPILER_64_ZERO(out_bytes);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egr_net_port_3,
                                     snmpDot1dTpPortOutFrames, &out_pkt));
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egr_net_port_3,
                                     snmpIfOutOctets, &out_bytes));
    printf("Tx : %d packets / %d bytes\n", COMPILER_64_LO(out_pkt),
                                           COMPILER_64_LO(out_bytes));
    if (COMPILER_64_LO(out_pkt) != 1) {
        printf("Packet verify failed. Expected out packet 1 but get %d\n",
                                           COMPILER_64_LO(out_pkt));
        return -1;
    }

    printf("--------Network port (%d) stats--------\n", egr_net_port_4);
    COMPILER_64_ZERO(out_pkt);
    COMPILER_64_ZERO(out_bytes);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egr_net_port_4,
                                     snmpDot1dTpPortOutFrames, &out_pkt));
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egr_net_port_4,
                                     snmpIfOutOctets, &out_bytes));
    printf("Tx : %d packets / %d bytes\n", COMPILER_64_LO(out_pkt),
                                           COMPILER_64_LO(out_bytes));
    if (COMPILER_64_LO(out_pkt) != 0) {
        printf("Packet verify failed. Expected out packet 0 but get %d\n",
                                           COMPILER_64_LO(out_pkt));
        return -1;
    }

    return;
}

/* Create vlan and add port to vlan */
bcm_error_t
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t         pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}

/* Create L2 station Entry */
bcm_error_t
l2_station_add(int unit, bcm_mac_t mac_addr, bcm_vlan_t vlan, int flags)
{
    bcm_l2_station_t   l2_station;
    int                station_id;
    int                rv = BCM_E_NONE;

    bcm_l2_station_t_init(&l2_station);
    l2_station.dst_mac = mac_addr;
    l2_station.vlan = vlan;
    l2_station.dst_mac_mask = "FF:FF:FF:FF:FF:FF";
    l2_station.flags = flags;
    return bcm_l2_station_add(unit, &station_id, &l2_station);
}

/* Create L3 interface */
bcm_error_t
create_l3_interface(int unit, int flags, int intf_flags, bcm_mac_t local_mac,
                    int vid, bcm_if_t *intf_id)
{
    bcm_l3_intf_t      l3_intf;
    bcm_error_t        rv = BCM_E_NONE;

    /* L3 interface and egress object for access */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags = flags;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    l3_intf.l3a_intf_flags = intf_flags;
    rv = bcm_l3_intf_create(unit, &l3_intf);

    *intf_id = l3_intf.l3a_intf_id;

    return rv;
}

/* Create L3 next_hop */
bcm_error_t
create_egr_object(int unit, uint32 flag, int l3_if, bcm_mac_t nh_mac,
                  bcm_gport_t gport, int vid, bcm_if_t *egr_obj_id)
{
    bcm_l3_egress_t    l3_egress;
    bcm_error_t        rv = BCM_E_NONE;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags2 |= flag;
    l3_egress.intf  = l3_if;
    sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
    l3_egress.vlan = vid;
    l3_egress.port = gport;
    rv = bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);

    return rv;
}

/* Create VPLS vpn */
bcm_error_t
mpls_vpn_create(int unit, l2vpn_cfg_t *l2vpn_cfg)
{
    bcm_mpls_vpn_config_t  vpn_info;
    bcm_error_t            rv = BCM_E_NONE;

    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_MPLS_VPN_VPLS;
    vpn_info.broadcast_group = l2vpn_cfg->bcast_group;
    vpn_info.unknown_unicast_group = l2vpn_cfg->bcast_group;
    vpn_info.unknown_multicast_group = l2vpn_cfg->bcast_group;
    rv = bcm_mpls_vpn_id_create(unit, &vpn_info);

    printf("VPN ID = %x\n", vpn_info.vpn);

    l2vpn_cfg->mpls_vpn = vpn_info.vpn;

    return rv;
}

/* Create mpls port */
bcm_error_t
mpls_port_create(int unit, bcm_vpn_t vpn, bcm_mpls_port_t *mpls_port)
{
    bcm_error_t           rv = BCM_E_NONE;

    rv = bcm_mpls_port_add(unit, vpn, mpls_port);
    if (BCM_FAILURE(rv)) {
        printf("\nbcm_mpls_port_add() failed: %s\n", bcm_errmsg(rv));
        return rv;
	}
    return rv;
}

/* Create mpls access port and bind it to vpn */
bcm_error_t
config_l2vpn_access (int unit, l2vpn_cfg_t *l2vpn_cfg,
                     l2vpn_acc_cfg_t *l2vpn_acc_cfg)
{
    bcm_vlan_action_set_t  vlan_action;
    bcm_mpls_port_t        acc_mpls_port;
    bcm_error_t            rv = BCM_E_NONE;

    printf("Create VLAN %d\n", l2vpn_acc_cfg->acc_vlan);
    rv = vlan_create_add_port(unit, l2vpn_acc_cfg->acc_vlan,
                              l2vpn_acc_cfg->acc_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in vlan_create_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Port must be added to VPN to pass membership check */
	rv = bcm_vlan_gport_add(unit, l2vpn_cfg->mpls_vpn,
                            l2vpn_acc_cfg->acc_gport, 0);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_gport_add: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enable ingress VLAN translation */
    rv = bcm_vlan_control_port_set(unit, l2vpn_acc_cfg->acc_port,
                                   bcmVlanTranslateIngressEnable, TRUE);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_vlan_control_port_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set port classification ID */
    rv = bcm_port_class_set(unit, l2vpn_acc_cfg->acc_port,bcmPortClassIngress,
                            l2vpn_acc_cfg->port_class);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_class_set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create mpls access port */
    bcm_mpls_port_t_init(&acc_mpls_port);
    acc_mpls_port.port_group = l2vpn_acc_cfg->port_class;
    acc_mpls_port.match_vlan = l2vpn_acc_cfg->acc_vlan;
    acc_mpls_port.criteria = l2vpn_acc_cfg->criteria;
    acc_mpls_port.port     = l2vpn_acc_cfg->acc_gport;
    rv = mpls_port_create(unit, l2vpn_cfg->mpls_vpn, &acc_mpls_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in access mpls_port_create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    l2vpn_acc_cfg->acc_vport = acc_mpls_port.mpls_port_id;
    printf("Access MPLS port = 0x%x\n", l2vpn_acc_cfg->acc_vport);

    /* Add the port to the multicast group */
    rv = bcm_multicast_egress_add(unit, l2vpn_cfg->bcast_group,
                                  l2vpn_acc_cfg->acc_gport, BCM_IF_INVALID);
    if (BCM_FAILURE(rv)) {
        printf("\nError executing bcm_multicast_egress_add(): %s.\n",
                                         bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Create mpls tunnel label */
bcm_error_t
mpls_tunnel_initiator(int unit, uint32 label, uint8 ttl, uint32 flags,
                      bcm_gport_t *tunnel_id)
{
    bcm_mpls_tunnel_encap_t     tunnel_encap;
    bcm_error_t                 rv = BCM_E_NONE;

    bcm_mpls_tunnel_encap_t_init(&tunnel_encap);
    tunnel_encap.label_array[0].label = label;
    tunnel_encap.label_array[0].ttl = ttl;
    tunnel_encap.label_array[0].flags = flags;
    tunnel_encap.num_labels = 1;
    rv = bcm_mpls_tunnel_encap_create(unit, 0, &tunnel_encap);

    *tunnel_id = tunnel_encap.tunnel_id;
    printf("Tunnel ID = %x\n", *tunnel_id);

    return rv;
}

/* Create mpls tunnel termination label */
bcm_error_t
mpls_tunnel_terminator(int unit, uint32 label, uint32 flags,
                       bcm_mpls_switch_action_t action)
{
    bcm_mpls_tunnel_switch_t    info;
    bcm_error_t                 rv = BCM_E_NONE;

    bcm_mpls_tunnel_switch_t_init(&info);
    info.label = label;
    info.port  = BCM_GPORT_INVALID;
    info.action = action;
    rv = bcm_mpls_tunnel_switch_add(unit, &info);

    return rv;
}

/* Create & configure mpls network port */
bcm_error_t
config_l2vpn_network (int unit, l2vpn_cfg_t *l2vpn_cfg,
                      l2vpn_net_cfg_t *l2vpn_net_cfg)
{
    bcm_vlan_action_set_t  vlan_action;
    bcm_multicast_encap_t  mc_encap;
    bcm_mpls_port_t        net_mpls_port;
    bcm_error_t            rv = BCM_E_NONE;
    bcm_if_t               l3_intf;
    bcm_if_t               egr_obj;
    int                    encap_id = 0;


    printf("Create VLAN %d\n", l2vpn_net_cfg->net_vlan);
    rv = vlan_create_add_port(unit, l2vpn_net_cfg->net_vlan,
                              l2vpn_net_cfg->net_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in vlan_create_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Port must be added to VPN to pass membership check */
	rv = bcm_vlan_gport_add(unit, l2vpn_cfg->mpls_vpn,
                            l2vpn_net_cfg->net_gport, 0);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_gport_add: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create an L3 interface to be used for the MPLS tunnel */
    rv = create_l3_interface(unit, 0, BCM_L3_INTF_UNDERLAY,
                             l2vpn_net_cfg->net_local_mac,
                             l2vpn_net_cfg->net_vlan, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_l3_interface: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L3 egress object for MPLS tunnel */
    rv = create_egr_object(unit, BCM_L3_FLAGS2_UNDERLAY, l3_intf,
                           l2vpn_net_cfg->net_remote_mac,
                           l2vpn_net_cfg->net_gport,
                           l2vpn_net_cfg->net_vlan, &egr_obj);
    if (BCM_FAILURE(rv)) {
        printf("\nError in create_egr_object: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set MPLS tunnel initiator */
    rv = mpls_tunnel_initiator(unit, l2vpn_net_cfg->tunnel_label_init,
                               l2vpn_net_cfg->ttl,
                               BCM_MPLS_EGRESS_LABEL_TTL_SET,
                               &l2vpn_net_cfg->tunnel_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in mpls_tunnel_initiator: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add MPLS label lookup for tunnel termination */
    rv = mpls_tunnel_terminator(unit, l2vpn_net_cfg->tunnel_label_term,
                                BCM_MPLS_SWITCH_COUNTED,
                                BCM_MPLS_SWITCH_ACTION_POP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in mpls_tunnel_terminator: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create mpls provider port */
    bcm_mpls_port_t_init(&net_mpls_port);
    net_mpls_port.flags = BCM_MPLS_PORT_NETWORK | BCM_MPLS_PORT_ENCAP_WITH_ID;
    /* BCM_MPLS_PORT_CONTROL_WORD - sequence number zero as control word */
    net_mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    net_mpls_port.port = l2vpn_net_cfg->net_gport;
    net_mpls_port.criteria = l2vpn_net_cfg->criteria;
    net_mpls_port.match_label = l2vpn_net_cfg->vc_label_term;
    net_mpls_port.encap_id = egr_obj;

    net_mpls_port.egress_label.label = l2vpn_net_cfg->vc_label_init;
    net_mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    net_mpls_port.egress_label.ttl = l2vpn_net_cfg->ttl;

    net_mpls_port.network_group_id = l2vpn_net_cfg->network_group_id;
    net_mpls_port.tunnel_id = l2vpn_net_cfg->tunnel_id;
    rv = mpls_port_create(unit, l2vpn_cfg->mpls_vpn, &net_mpls_port);
    if (BCM_FAILURE(rv)) {
        printf("\nError in network mpls_port_create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    l2vpn_net_cfg->net_vport = net_mpls_port.mpls_port_id;
    printf("Network MPLS port = 0x%x\n", l2vpn_net_cfg->net_vport);

    /* Create multicast encap id */
    print bcm_multicast_encap_t_init(&mc_encap);
    mc_encap.encap_type = bcmMulticastEncapTypeL2Tunnel;
    mc_encap.ul_egress_if = egr_obj;
    mc_encap.l3_intf = 0;
    mc_encap.port_id = l2vpn_net_cfg->net_vport;
    rv = bcm_multicast_encap_create(unit, &mc_encap, &encap_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_encap_create : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add the multicast encap id to the multicast group */
    rv = bcm_multicast_egress_add(unit, l2vpn_cfg->bcast_group,
                                  l2vpn_net_cfg->net_gport, encap_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_egress_add : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Configure MY STATION entry */
    rv = l2_station_add(unit, l2vpn_net_cfg->net_local_mac,
                        l2vpn_net_cfg->net_vlan, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in l2_station_add: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int
add_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port)
{
	bcm_l2_addr_t      l2_addr;

	bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
	l2_addr.flags |= BCM_L2_STATIC;
	l2_addr.port   = port;
	return bcm_l2_addr_add(unit, &l2_addr);
}

/* Switch VPLS configuration function */
bcm_error_t
setup_vpls(int unit)
{
    bcm_error_t        rv = BCM_E_NONE;
    bcm_pbmp_t         pbmp, ubmp;

	/* VPN configuration variables */
	l2vpn_cfg_t l2vpn_cfg = {
		0x0,                       /* VPN */
		0x0                        /* Multicast group */
	};

	/* Access port 1 configuration variables */
	l2vpn_acc_cfg_t l2vpn_acc_1 = {
		ing_acc_port_1,            /* Access port */
		ing_acc_gport_1,           /* Access gport */
        0x0,                       /* Access VP gport */
		11,                        /* Access VLAN */
        0x0,                       /* Access inner VLAN */
        "00:00:00:00:00:11",       /* HOST mac */
        "00:00:00:00:00:22",       /* Remote HOST mac */
        BCM_MPLS_PORT_MATCH_PORT_GROUP_VLAN, /* Access match criteria */
        31                         /* Access port class */
	};

	/* Access port 2 configuration variables */
	l2vpn_acc_cfg_t l2vpn_acc_2 = {
		egr_acc_port_2,            /* Access port */
		egr_acc_gport_2,           /* Access gport */
        0x0,                       /* Access VP gport */
		12,                        /* Access VLAN */
        0x0,                       /* Access inner VLAN */
        "00:00:00:00:00:12",       /* HOST mac */
        "00:00:00:00:00:23",       /* Remote HOST mac */
        BCM_MPLS_PORT_MATCH_PORT_GROUP_VLAN, /* Access match criteria */
        32                         /* Access port class */
	};

	/* Network port 1 configuration variables */
	l2vpn_net_cfg_t l2vpn_net_1 = {
		egr_net_port_3,            /* Network port */
		egr_net_gport_3,           /* Network gport */
        0x0,                       /* Network VP gport */
		22,                        /* Network VLAN */
		"00:00:00:00:00:BB",       /* local mac */
		"00:00:00:00:00:AA",       /* remote mac */
        BCM_MPLS_PORT_MATCH_LABEL, /* Network match criteria */
		0x444,                     /* VC init label */
		0x333,                     /* VC term label */
		0x111,                     /* Tunnel init label */
		0x222,                     /* Tunnel term label */
		0x0,                       /* Tunnel ID */
        0x1,                       /* Number of labels */
        0x16,                      /* TTL value */
        0x0,                       /* EXP value */
        33,                        /* Network port class */
        1                          /* Network group ID */
	};

	/* Network port 2 configuration variables */
	l2vpn_net_cfg_t l2vpn_net_2 = {
		egr_net_port_4,            /* Network port */
		egr_net_gport_4,           /* Network gport */
        0x0,                       /* Network VP gport */
		23,                        /* Network VLAN */
		"00:00:00:00:00:DD",       /* local mac */
		"00:00:00:00:00:CC",       /* remote mac */
        BCM_MPLS_PORT_MATCH_LABEL, /* Network match criteria */
		0x888,                     /* VC init label */
		0x777,                     /* VC term label */
		0x555,                     /* Tunnel init label */
		0x666,                     /* Tunnel term label */
		0x0,                       /* Tunnel ID */
        0x1,                       /* Number of labels */
        0x16,                      /* TTL value */
        0x0,                       /* EXP value */
        34,                        /* Network port class */
        2                          /* Network group ID */
	};

    /* Create a broadcast group for VPLS L2 lookup DLF or broadcast traffic */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VPLS,
                              &l2vpn_cfg.bcast_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create VPLS VPN */
    rv = mpls_vpn_create(unit, &l2vpn_cfg);
    if (BCM_FAILURE(rv)) {
        printf("\nError in mpls_vpn_create: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Access port - 1 */
    /* Create mpls access port 1 and bind it to vpn */
	rv = config_l2vpn_access(unit, &l2vpn_cfg, &l2vpn_acc_1);
	if (BCM_FAILURE(rv)) {
		printf("\nError in access 1 config: %s.\n", bcm_errmsg(rv));
		return rv;
	}

    /* Access port - 2 */
    /* Create mpls access port 2 and bind it to vpn */
	rv = config_l2vpn_access(unit, &l2vpn_cfg, &l2vpn_acc_2);
	if (BCM_FAILURE(rv)) {
		printf("\nError in access 2 config: %s.\n", bcm_errmsg(rv));
		return rv;
	}

    /* Network port - 1 */
    /* Create & configure mpls network port 1 */
	rv = config_l2vpn_network(unit, &l2vpn_cfg, &l2vpn_net_1);
	if (BCM_FAILURE(rv)) {
		printf("\nError in network 1 config: %s.\n", bcm_errmsg(rv));
		return rv;
	}

    /* Network port - 2 */
    /* Create & configure mpls network port 2 */
	rv = config_l2vpn_network(unit, &l2vpn_cfg, &l2vpn_net_2);
	if (BCM_FAILURE(rv)) {
		printf("\nError in network 1 config: %s.\n", bcm_errmsg(rv));
		return rv;
	}

	printf("Access-to-Network: Add payload-dest-mac => Network VPort 1 \n");
	rv = add_to_l2_table(unit, l2vpn_acc_1.remote_host_mac, l2vpn_cfg.mpls_vpn,
                         l2vpn_net_1.net_vport);
	if (BCM_FAILURE(rv)) {
		printf("\nError in l2 entry config: %s.\n", bcm_errmsg(rv));
		return rv;
	}

    return rv;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in setup_vpls())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; version";
    bshell(unit, "config show; attach ; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) setup_vpls(): ** start **";
    if (BCM_FAILURE((rv = setup_vpls(unit)))) {
        printf("setup_vpls() failed.\n");
        return -1;
    }
    print "~~~ #2) setup_vpls(): ** end **";

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
