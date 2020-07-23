/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : TD4(56880) L3 MPLS tunnel initiation flow with ECMP
 *
 * Usage    : BCM.0> cint td4_hsdk_l3mpls_init_ecmp.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : td4_hsdk_l3mpls_init_ecmp_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              |         +---> Egress port 1
 *                  |                              |         |
 *     ingress_port |                              |         |
 * +----------------+          SWITCH              +---ECMP--+
 *                  |                              |         |
 *                  |                              |         |
 *                  |                              |         +---> Egress port 2
 *                  |                              |
 *                  +------------------------------+
 * Summary:
 * ========
 *   Cint script to demonstrate L3 MPLS tunnel initiation with ECMP.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. Out of these
 *        three ports, one port is used as ingress_port and the others as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_l3_mpls()):
 *   =========================================================
 *     a) Configure a basic L3 MPLS tunnel initiation functional scenario and
 *        does the necessary configurations of vlan, interface, next hop, ECMP
 *        and tunnel + VC labels.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        and 'l3 egress show'
 *
 *     b) Transmit the known IPv4 unicast packet with different SRC IP.
 *        The contents of the packet are printed on screen. The packet
 *        used matches the route configured through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are all changed as the packet is
 *       routed through the egress port and also tunnel + VC labels are pushed
 *       as part of tunnel initiation on egress port. And 'show c' to check
 *       the Tx/Rx packet stats/counters.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t    ing_acc_port_1;
bcm_port_t    egr_net_port_2;
bcm_port_t    egr_net_port_3;
bcm_gport_t   ing_acc_gport_1;
bcm_gport_t   egr_net_gport_2;
bcm_gport_t   egr_net_gport_3;
int           NO_OF_ECMP_MEMBERS = 2;

/* struct for access port params */
struct l3vpn_acc_cfg_t {
    bcm_port_t         acc_port;          /* Access port */
    bcm_gport_t        acc_gport;         /* Access gport */
    bcm_vlan_t         acc_vlan;          /* Access VLAN */
    bcm_mac_t          acc_mac;           /* INTF MAC */
    bcm_mac_t          acc_local_mac;     /* Outer SRC MAC */
    bcm_mac_t          acc_remote_mac;    /* Outer DST MAC */
    bcm_ip_t           dip;               /* Destination IP */
    bcm_ip_t           mask;              /* Destination IP mask */
    bcm_if_t           l3_ingress_if;     /* L3 ingress interface */
    int                vrf_id;            /* VRF ID */
};

/* struct for network port params */
struct l3vpn_net_cfg_t {
    bcm_port_t         net_port;          /* Network port */
    bcm_gport_t        net_gport;         /* Network gport */
    bcm_vlan_t         net_vlan;          /* Network VLAN */
    bcm_mac_t          net_mac;           /* INTF MAC */
    bcm_mac_t          net_local_mac;     /* Outer SRC MAC */
    bcm_mac_t          net_remote_mac;    /* Outer DST MAC */
    bcm_ip_t           dip;               /* Destination IP */
    bcm_ip_t           mask;              /* Destination IP mask */
    bcm_if_t           l3_ingress_if;     /* L3 ingress interface */
    bcm_if_t           l3_egr_obj;        /* L3 egress object */
    bcm_if_t           l3_ul_egr_obj;     /* L3 UL egress object */
    bcm_mpls_label_t   tunnel_label_init; /* Tunnel init label */
    bcm_mpls_label_t   vc_label_init;     /* VC init label */
    bcm_mpls_label_t   tunnel_label_term; /* Tunnel termination label */
    bcm_mpls_label_t   vc_label_term;     /* VC termination label */
    int                vrf_id;            /* VRF ID */
};

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
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
ingress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_entry_t        entry;
    bcm_field_group_config_t group_config;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                              BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.priority = 1;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group,
                                               &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,
                                                 BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                             bcmFieldActionCopyToCpu, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    printf("ingress_port_setup configured for ingress_port : %d\n", port);

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
    bcm_field_entry_t        entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                              BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group,
                                               &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,
                                                 BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                             bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop,
                                             0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    printf("egress_port_setup configured for egress_port : %d\n", port);

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
 * egress ports. Check ingress_port_setup() and egress_port_multi_setup().
 */
bcm_error_t
test_setup(int unit)
{
    bcm_pbmp_t    pbmp;
    int           num_ports = 3;
    int           port_list[num_ports];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ing_acc_port_1 = port_list[0];
    egr_net_port_2 = port_list[1];
    egr_net_port_3 = port_list[2];

    if (BCM_E_NONE !=bcm_port_gport_get(unit, ing_acc_port_1,
                                        &(ing_acc_gport_1))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                        ing_acc_port_1);
        return -1;
    }

    if (BCM_E_NONE != bcm_port_gport_get(unit, egr_net_port_2,
                                         &(egr_net_gport_2))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                         egr_net_port_2);
        return -1;
    }

    if (BCM_E_NONE != bcm_port_gport_get(unit, egr_net_port_3,
                                         &(egr_net_gport_3))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n",
                                         egr_net_port_3);
        return -1;
    }

    if (BCM_E_NONE != ingress_port_setup(unit, ing_acc_port_1)) {
        printf("ingress_port_setup() failed for port %d\n", ing_acc_port_1);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egr_net_port_2);
    BCM_PBMP_PORT_ADD(pbmp, egr_net_port_3);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for egress port \n");
        return -1;
    }

    bshell(unit, "pw start report +raw +decode +pmd");

    return BCM_E_NONE;
}

void
verify(int unit)
{
    char str[512];
    uint64    in_pkt, out_pkt, in_bytes, out_bytes;

    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "clear c");
    printf("\nSending IPv4 unicast packet 1 to ingress_port:%d\n", ing_acc_port_1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001000000AABBCC8100000B08004500002E0000000040FF9B1DC0A80A010A0A0A01000102030405060708090A0B0C0D0E0F101112131415161718190258A932; sleep quiet 1", ing_acc_port_1);
    bshell(unit, str);
    printf("\nSending IPv4 unicast packet 2 to ingress_port:%d\n", ing_acc_port_1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001000000AABBCC8100000B08004500002E0000000040FF9B13C0A80A0B0A0A0A01000102030405060708090A0B0C0D0E0F101112131415161718190258A932; sleep quiet 2", ing_acc_port_1);
    bshell(unit, str);
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

    printf("--------Network port (%d) stats--------\n", egr_net_port_2);
    COMPILER_64_ZERO(out_pkt);
    COMPILER_64_ZERO(out_bytes);
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egr_net_port_2,
                                     snmpDot1dTpPortOutFrames, &out_pkt));
    BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egr_net_port_2,
                                     snmpIfOutOctets, &out_bytes));
    printf("Tx : %d packets / %d bytes\n", COMPILER_64_LO(out_pkt),
                                           COMPILER_64_LO(out_bytes));
    if (COMPILER_64_LO(out_pkt) != 1) {
        printf("Packet verify failed. Expected out packet 1 but get %d\n",
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


    return;
}

/* Create vlan and add port to vlan */
int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t        rv = BCM_E_NONE;
    bcm_pbmp_t         pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    return BCM_E_NONE;
}

/* Create mpls access port */
bcm_error_t
config_l3_mpls_access (int unit, l3vpn_acc_cfg_t *l3vpn_acc_cfg)
{
    bcm_l3_ingress_t   l3_ingress;
    bcm_l3_egress_t    l3_egress;
    bcm_l2_addr_t      l2addr;
    bcm_l3_intf_t      l3_intf;
    bcm_error_t        rv = BCM_E_NONE;
    bcm_if_t           egr_obj;
    bcm_if_t           ingress_if;

    /* Create customer vlan */
    rv = create_vlan_add_port(unit, l3vpn_acc_cfg->acc_vlan,
                              l3vpn_acc_cfg->acc_port);
    if (BCM_FAILURE(rv)) {
        printf("create_vlan_add_port return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enable L3 lookup on customer side : add intf mac to l2 */
    bcm_l2_addr_t_init(&l2addr, l3vpn_acc_cfg->acc_mac,
                       l3vpn_acc_cfg->acc_vlan);
    l2addr.flags |= (BCM_L2_STATIC | BCM_L2_L3LOOKUP);
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

    /* Create L3 interface  */
    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, l3vpn_acc_cfg->acc_local_mac,
               sizeof(l3vpn_acc_cfg->acc_local_mac));
    l3_intf.l3a_vid = l3vpn_acc_cfg->acc_vlan;
    l3_intf.l3a_vrf = l3vpn_acc_cfg->vrf_id;
    l3_intf.l3a_mtu = 1500;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    printf("l3 intf = %d\n", l3_intf.l3a_intf_id);

    /* Use the same ID to allocate the ingress interface (L3_IIF) */
    ingress_if = l3_intf.l3a_intf_id;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_REPLACE;
    l3_ingress.vrf = l3vpn_acc_cfg->vrf_id;
    l3_ingress.ipmc_intf_id  = l3_intf.l3a_intf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if));
    l3vpn_acc_cfg->l3_ingress_if = ingress_if;

    /* Create Egress object to customer side */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, l3vpn_acc_cfg->acc_remote_mac,
                                   sizeof(l3vpn_acc_cfg->acc_remote_mac));
    l3_egress.intf     = l3_intf.l3a_intf_id;
    l3_egress.vlan     = l3vpn_acc_cfg->acc_vlan;
    l3_egress.port     = l3vpn_acc_cfg->acc_gport;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj));

    /* Adding IP address to host table */
    bcm_l3_host_t host;
    bcm_l3_host_t_init(&host);
    host.l3a_intf = egr_obj;
    host.l3a_ip_addr = l3vpn_acc_cfg->dip;
    host.l3a_vrf = l3vpn_acc_cfg->vrf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_host_add(unit, &host));

    return rv;
}

/* Create mpls network port */
bcm_error_t
config_l3_mpls_network (int unit, l3vpn_acc_cfg_t *l3vpn_acc_cfg,
                        l3vpn_net_cfg_t *l3vpn_net_cfg)
{
    bcm_mpls_tunnel_switch_t tunnel_switch;
    bcm_vlan_control_vlan_t  vlan_control;
    bcm_mpls_tunnel_encap_t  tunnel_encap;
    bcm_l3_ingress_t         l3_ingress;
    bcm_l3_egress_t          ul_l3_egress;
    bcm_l3_egress_t          l3_egress;
    bcm_l3_intf_t            l3_intf;
    bcm_error_t              rv = BCM_E_NONE;
    bcm_if_t                 ul_egr_obj_id;
    bcm_if_t                 ingress_if_egr;
    bcm_if_t                 egr_obj;

    /* Create provider vlan */
    rv = create_vlan_add_port(unit, l3vpn_net_cfg->net_vlan,
                              l3vpn_net_cfg->net_port);
    if (BCM_FAILURE(rv)) {
        printf("create_vlan_add_port return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* MPLS tunnel encap configuration */
    bcm_mpls_tunnel_encap_t_init(&tunnel_encap);
    tunnel_encap.label_array[0].label = l3vpn_net_cfg->tunnel_label_init;
    tunnel_encap.label_array[0].ttl = 64;
    tunnel_encap.label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    tunnel_encap.num_labels = 1;
    BCM_IF_ERROR_RETURN(bcm_mpls_tunnel_encap_create(unit, 0, &tunnel_encap));

    /* create L3 intf for provider side */
    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = l3vpn_net_cfg->net_vlan;
    l3_intf.l3a_mtu = 1500;
    l3_intf.l3a_tunnel_idx = tunnel_encap.tunnel_id;
    l3_intf.l3a_mpls_label = l3vpn_net_cfg->vc_label_init;
    l3_intf.l3a_mpls_flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    print l3_intf.l3a_intf_id;

    ingress_if_egr = l3_intf.l3a_intf_id;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_REPLACE;
    l3_ingress.vrf  = l3vpn_net_cfg->vrf_id;
    l3_ingress.ipmc_intf_id  = l3_intf.l3a_intf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress,
                                              &ingress_if_egr));

    /*
     * configure VLAN_ID to L3_IIF mapping
     */
    bcm_vlan_control_vlan_t_init(&vlan_control);
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, l3vpn_acc_cfg->acc_vlan,
                                                  &vlan_control));
    vlan_control.ingress_if = ingress_if_egr;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, l3vpn_acc_cfg->acc_vlan,
                                                  vlan_control));

    /* create  overlay L3 egress object */
    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.intf = l3_intf.l3a_intf_id;
    l3_egress.vlan = l3vpn_net_cfg->net_vlan;
    l3_egress.port = l3vpn_net_cfg->net_gport;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj));
    l3vpn_net_cfg->l3_egr_obj = egr_obj;

    /* new APIs to add underlay L3 Interface */
    bcm_l3_intf_t ul_l3_intf;
    bcm_l3_intf_t_init(&ul_l3_intf);
    ul_l3_intf.l3a_intf_flags = BCM_L3_INTF_UNDERLAY;
    sal_memcpy(ul_l3_intf.l3a_mac_addr, l3vpn_net_cfg->net_mac,
                                        sizeof(l3vpn_net_cfg->net_mac));
    ul_l3_intf.l3a_vrf = l3vpn_net_cfg->vrf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &ul_l3_intf));

    /* Create underlay L3 egress object */
    bcm_l3_egress_t_init(&ul_l3_egress);
    sal_memcpy(ul_l3_egress.mac_addr, l3vpn_net_cfg->net_remote_mac,
                                      sizeof(l3vpn_net_cfg->net_remote_mac));
    ul_l3_egress.intf = ul_l3_intf.l3a_intf_id;
    ul_l3_egress.port = l3vpn_net_cfg->net_gport;
    ul_l3_egress.flags2 = BCM_L3_FLAGS2_UNDERLAY;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &ul_l3_egress,
                                             &ul_egr_obj_id));
    l3vpn_net_cfg->l3_ul_egr_obj = ul_egr_obj_id;

    /* Add MPLS table entry for incoming label matching */
    bcm_mpls_tunnel_switch_t_init (&tunnel_switch);
    tunnel_switch.label   = l3vpn_net_cfg->tunnel_label_term;
    tunnel_switch.port    = BCM_GPORT_INVALID;
    tunnel_switch.action  = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel_switch.ingress_if = l3vpn_acc_cfg->l3_ingress_if;
    rv = bcm_mpls_tunnel_switch_add(unit, &tunnel_switch);
    if (BCM_FAILURE(rv)) {
        printf("bcm_mpls_tunnel_switch_add return error : %s.\n",
                                                  bcm_errmsg(rv));
        return rv;
    }

    /* Add MPLS table entry for incoming label matching */
    bcm_mpls_tunnel_switch_t_init (&tunnel_switch);
    tunnel_switch.label   = l3vpn_net_cfg->vc_label_term;
    tunnel_switch.port    = BCM_GPORT_INVALID;
    tunnel_switch.action  = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel_switch.ingress_if = l3vpn_acc_cfg->l3_ingress_if;
    rv = bcm_mpls_tunnel_switch_add(unit, &tunnel_switch);
    if (BCM_FAILURE(rv)) {
        printf("bcm_mpls_tunnel_switch_add return error : %s.\n",
                                                  bcm_errmsg(rv));
        return rv;
    }

    /* Enable MPLS Lookup for incoming packet */
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, l3vpn_net_cfg->net_local_mac,
                                          l3vpn_net_cfg->net_vlan));

    return rv;
}

int
config_FlexDigest(int unit)
{
    bcm_flexdigest_match_id_t   match_id;
    bcm_flexdigest_group_t      group;
    bcm_flexdigest_entry_t      entry;
    bcm_flexdigest_qset_t       qset;
    int                         seed_profile_id;
    int                         norm_profile_id;
    int                         hash_profile_id;
    int                         value;
    int                         pri = 0;
    uint16                      salt_a[16] = { 0xabcd,
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
                                               0xeeee};

    BCM_IF_ERROR_RETURN(bcm_flexdigest_init(unit));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_norm_seed_profile_create(unit, 0,
                                &seed_profile_id));
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
                       bcmFlexDigestNormProfileControlNorm,
                       value));
    value = 0;
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_profile_set(unit, norm_profile_id,
                       bcmFlexDigestNormProfileControlSeedId,
                       value));

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
	BCM_IF_ERROR_RETURN
        (bcm_flexdigest_group_create(unit, qset, pri, &group));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_entry_priority_set(unit, entry, 2));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_match_id_create(unit, &match_id));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_match_add(unit, match_id,
                                  bcmFlexDigestMatchOuterL2HdrL2));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_qualify_MatchId(unit, entry, match_id));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds0,
                           BCM_FLEXDIGEST_FIELD_OUTER_MACSA_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds1,
                           BCM_FLEXDIGEST_FIELD_OUTER_MACSA_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds2,
                           BCM_FLEXDIGEST_FIELD_OUTER_MACSA_32_47, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds3,
                           BCM_FLEXDIGEST_FIELD_OUTER_MACDA_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds4,
                           BCM_FLEXDIGEST_FIELD_OUTER_MACDA_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds5,
                           BCM_FLEXDIGEST_FIELD_OUTER_MACDA_32_47, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds6,
                           BCM_FLEXDIGEST_FIELD_OUTER_OTAG_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds7,
                           BCM_FLEXDIGEST_FIELD_OUTER_OTAG_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds8,
                           BCM_FLEXDIGEST_FIELD_OUTER_ETHERTYPE, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds9,
                           BCM_FLEXDIGEST_FIELD_INGRESS_PP_PORT, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds10,
                           BCM_FLEXDIGEST_FIELD_L2_IIF, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds11,
                           BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_A_0_15,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds12,
                           BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_A_16_31,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds13,
                           BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_0_15,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds14,
                           BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_16_31,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetACmds15,
                           BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_C_0_15,
                           0xffff));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds0,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds1,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds2,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_32_47,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds3,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_48_63,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds4,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_64_79,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds5,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_80_95,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds6,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_96_111,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds7,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_112_127,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds8,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds9,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds10,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_32_47,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds11,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_48_63,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds12,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_64_79,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds13,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_80_95,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds14,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_96_111,
                           0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetBCmds15,
                           BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_112_127,
                           0xffff));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds0, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds1, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds2, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds3, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds4, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds5, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds6, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds7, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds8, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds9, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds10, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds11, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds12, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds13, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds14, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry,
                           bcmFlexDigestActionExtractBinSetCCmds15, 0, 0x1));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_entry_install(unit, entry));

    /* ECMP FlexDigest */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                           bcmSwitchHashUseFlowSelEcmpOverlay, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,
                           bcmSwitchHashUseFlowSelEcmp, 0));

    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, ing_acc_port_1,
                           bcmSwitchECMPOverlayHashOffset, 48));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, ing_acc_port_1,
                           bcmSwitchECMPUnderlayHashSet0Offset, 48));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, ing_acc_port_1,
                           bcmSwitchEcmpDynamicHashOffset, 48));

    return BCM_E_NONE;
}

bcm_error_t
mpls_create_ecmp_obj(int unit, uint32 flags, int no_of_intfs,
                     bcm_if_t *egr_obj_array, bcm_if_t *egr_obj_ul_array,
                     bcm_if_t *ecmp_obj_id)
{
    bcm_l3_egress_ecmp_t   ecmp_info;
    bcm_l3_ecmp_member_t   ecmp_group[no_of_intfs];
    bcm_error_t            rv = BCM_E_NONE;
    int                    i;

    bcm_l3_egress_ecmp_t_init(&ecmp_info);
    for(i=0; i<no_of_intfs; i++)
    {
        bcm_l3_ecmp_member_t_init(&ecmp_group[i]);
        ecmp_group[i].egress_if = egr_obj_array[i];
        if(BCM_L3_ECMP_OVERLAY == flags)
        {
            ecmp_group[i].egress_if_2 = egr_obj_ul_array[i];
        }
    }
    ecmp_info.ecmp_group_flags |= flags;

    rv = bcm_l3_ecmp_create(unit,0, &ecmp_info, no_of_intfs, ecmp_group);
    *ecmp_obj_id = ecmp_info.ecmp_intf;

    return rv;
}

bcm_error_t
config_l3_mpls(int unit)
{
    bcm_error_t              rv = BCM_E_NONE;
    bcm_if_t                 net_egr_obj_ol[NO_OF_ECMP_MEMBERS];
    bcm_if_t                 net_egr_obj_ul[NO_OF_ECMP_MEMBERS];
    bcm_if_t                 net_ecmp_obj_id_ul;
    bcm_if_t                 net_ecmp_obj_id_ol;

	/* Access port configuration variables */
    l3vpn_acc_cfg_t l3vpn_acc = {
        ing_acc_port_1,           /* Access port */
        ing_acc_gport_1,          /* Access gport */
        11,                       /* Access VLAN */
        "00:00:00:00:00:01",      /* INTF MAC */
        "00:00:00:13:02:01",      /* Outer SRC MAC */
        "00:00:00:13:02:02",      /* Outer DST MAC */
        0xC0A80A01,               /* Destination IP */
        0xffffff00,               /* Destination IP mask */
        0,                        /* L3 ingress interface */
        33                        /* VRF ID */
    };

	/* Network port 1 configuration variables */
    l3vpn_net_cfg_t l3vpn_net_1 = {
        egr_net_port_2,           /* Network port */
        egr_net_gport_2,          /* Network gport */
        12,                       /* Network VLAN */
        "00:00:00:00:00:02",      /* INTF MAC */
        "00:00:00:00:13:03",      /* Outer SRC MAC */
        "00:00:00:00:00:03",      /* Outer DST MAC */
        0x0A0A0A01,               /* Destination IP */
        0xffffff00,               /* Destination IP mask */
        0,                        /* L3 ingress interface */
        0,                        /* L3 egress object */
        0,                        /* L3 UL egress object */
        0x65,                     /* Tunnel init label */
        0xC9,                     /* VC init label */
		0x301,                    /* Tunnel termination label */
		0x302,                    /* VC termination label */
        33                        /* VRF ID */
    };

	/* Network port 2 configuration variables */
    l3vpn_net_cfg_t l3vpn_net_2 = {
        egr_net_port_3,           /* Network port */
        egr_net_gport_3,          /* Network gport */
        15,                       /* Network VLAN */
        "00:00:00:00:00:03",      /* INTF MAC */
        "00:00:00:00:13:04",      /* Outer SRC MAC */
        "00:00:00:00:00:04",      /* Outer DST MAC */
        0x0A0A0A02,               /* Destination IP */
        0xffffff00,               /* Destination IP mask */
        0,                        /* L3 ingress interface */
        0,                        /* L3 egress object */
        0,                        /* L3 UL egress object */
        0x66,                     /* Tunnel init label */
        0xCA,                     /* VC init label */
		0x401,                    /* Tunnel termination label */
		0x402,                    /* VC termination label */
        33                        /* VRF ID */
    };

    rv = config_l3_mpls_access (unit, &l3vpn_acc);
    if (BCM_FAILURE(rv)) {
        printf("config_l3_mpls_access return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_l3_mpls_network (unit, &l3vpn_acc, &l3vpn_net_1);
    if (BCM_FAILURE(rv)) {
        printf("config_l3_mpls_network return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = config_l3_mpls_network (unit, &l3vpn_acc, &l3vpn_net_2);
    if (BCM_FAILURE(rv)) {
        printf("config_l3_mpls_network return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\nSetup Hash for ECMP");
    rv = config_FlexDigest(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in Flex digest config: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Create Overlay network L3 ECMP object for Overlay.\n");
    net_egr_obj_ol[0] = l3vpn_net_1.l3_egr_obj;
    net_egr_obj_ol[1] = l3vpn_net_2.l3_egr_obj;

    net_egr_obj_ul[0] = l3vpn_net_1.l3_ul_egr_obj;
    net_egr_obj_ul[1] = l3vpn_net_2.l3_ul_egr_obj;

    rv = mpls_create_ecmp_obj(unit, BCM_L3_ECMP_OVERLAY, NO_OF_ECMP_MEMBERS,
                              net_egr_obj_ol, net_egr_obj_ul,
                              &net_ecmp_obj_id_ol);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring ECMP obj: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Adding IP address to host table */
    bcm_l3_host_t host;
    bcm_l3_host_t_init(&host);
    host.l3a_flags = BCM_L3_MULTIPATH;
    host.l3a_intf = net_ecmp_obj_id_ol;
    host.l3a_ip_addr = l3vpn_net_1.dip;
    host.l3a_vrf = l3vpn_net_1.vrf_id;
    BCM_IF_ERROR_RETURN(bcm_l3_host_add(unit, &host));

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_l3_mpls())
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

    print "~~~ #2) config_l3_mpls(): ** start **";
    if (BCM_FAILURE((rv = config_l3_mpls(unit)))) {
        printf("config_l3_mpls() failed.\n");
        return -1;
    }
    print "~~~ #2) config_l3_mpls(): ** end **";

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
