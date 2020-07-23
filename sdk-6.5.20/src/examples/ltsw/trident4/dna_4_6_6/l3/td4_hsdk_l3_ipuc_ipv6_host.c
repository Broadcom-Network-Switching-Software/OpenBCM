/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPv6 host entry
 *
 * Usage    : BCM.0> cint td4_hsdk_l3_ipuc_ipv6_host.c 
 *
 * config   : bcm56880_a0-generic-32x400.config.yml 
 *
 * Log file : td4_hsdk_l3_ipuc_ipv6_host_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |  
 *                  |                              | 
 * ingress_vlan 5   |                              |  egress vlan 10   
 * ingress_port cd0 |                              |  egress_port cd1 
 * +----------------+          SWITCH-TD4          +-----------------+   
 * src_mac 0x01     |                              |  src_mac 0x11  
 * dst_mac 0x02     |                              |  dst_mac 0x22 
 * DIP 10.10.10.10  |                              | 
 * SIP 20.20.20.20  |                              |
 *                  |                              |
 *                  +------------------------------+
 *                                                                        
 * Summary:
 * ========
 *   This Cint example to show configuration of the IPv6 host entry using
 *   BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy incoming and outgoing packets to CPU and
 *        start packet watcher to display packet contents.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in configure_ipuc_forwarding()):
 *   =========================================================
 *     a) Configure a basic IPv6 host entry functional scenario. This adds the
 *        host entry in L3 table and does the necessary configurations of vlan, L3_IIF,
 *        egress interface and next hop.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show' and 'l3 host show v6=1'
 *
 *     b) Transmit the known IPv6 unicast packet. The contents of the packet
 *        are printed on screen. The packet used matches the route configured
 *        through script.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac, TTL and vlan are all changed as the packet is
 *       routed through the egress port and 'show c' to check the Tx/Rx packet
 *       stats/counters.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t    ingress_port;
bcm_port_t    egress_port;
bcm_gport_t   ingress_gport;
bcm_gport_t   egress_gport;

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
    bcm_pbmp_t        ports_pbmp;

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
    bcm_field_entry_t        entry;
    bcm_field_group_config_t group_config; 

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.priority = 1;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

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
    bcm_field_entry_t        entry;
    bcm_field_group_config_t group_config; 

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
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

    printf("egress_port_setup configured for egress_port : %d\n", port);

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
    int port_list[2];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port  = port_list[1];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
        printf("egress_port_setup() failed for port %d\n", egress_port);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode +pmd");

    return BCM_E_NONE;
}

/*
 * Verification:
 *   In Packet:
 *   ----------
 *       0000 0000 0002 0000 0000 0001 8100 0005
 *       86dd 6030 0000 0006 3bff 2222 0000 0000
 *       0000 0000 0000 0000 0022 3333 0000 0000
 *       0000 0000 0000 0000 0033 0000 0000 0000
 *       6a10 1c6e
 *
 *  Out Packet:
 *  -----------
 *       0000 0000 0022 0000 0000 0011 8100 000a
 *       86dd 6030 0000 0006 3bfe 2222 0000 0000
 *       0000 0000 0000 0000 0022 3333 0000 0000
 *       0000 0000 0000 0000 0033 0000 0000 0000
 *       6a10 1c6e
 */

void
verify(int unit)
{
    char str[512];

    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "l3 host show v6=1");
    bshell(unit, "clear c");
    printf("\nSending IPv6 unicast packet to ingress_port : %d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000020000000000018100000586dd6030000000063bff22220000000000000000000000000022333300000000000000000000000000330000000000006a101c6e; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "l3 host show v6=1");
    bshell(unit, "show c");

    return;
}

/* Create vlan and add port to vlan */
int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_pbmp_t  pbmp, ubmp;

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

/* Create L3 Ingress Interface */
int 
create_l3_iif(int unit, bcm_vrf_t vrf, bcm_if_t ingress_if)
{
    bcm_error_t      rv = BCM_E_NONE;
    bcm_l3_ingress_t l3_ingress;

    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ingress.vrf   = vrf;

    rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3_IIF : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Config vlan_id to l3_iif mapping. */
int 
configure_l3_iif_binding(int unit, bcm_vlan_t vlan, bcm_if_t ingress_if)
{
    bcm_error_t             rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t vlan_ctrl;

    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;

    rv = bcm_vlan_control_vlan_selective_set(unit, vlan, BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK, &vlan_ctrl);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3_IIF binding : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Create L3 interface */
int
create_l3_intf(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t *l3_intf_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));

    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    *l3_intf_id = l3_intf.l3a_intf_id;

    return BCM_E_NONE;
}

/* Create L3 egress object */
int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport,
               bcm_if_t *egr_if)
{
    bcm_error_t     rv = BCM_E_NONE;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf = l3_if;
    l3_egr.port = gport;
    sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));

    rv = bcm_l3_egress_create(unit, 0, &l3_egr, egr_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 egress object : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("egress object id : %d created\n", *egr_if); 

    return BCM_E_NONE;
}

/* Configure my station */
int 
configure_my_station_tcam(int unit, bcm_mac_t mac)
{
    bcm_error_t      rv = BCM_E_NONE;
    bcm_mac_t        mac_mask={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    bcm_l2_station_t l2_station;
    int              station_id;

    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, mac, sizeof(mac));
    sal_memcpy(l2_station.dst_mac_mask, mac_mask, sizeof(mac_mask));

    rv = bcm_l2_station_add(unit, &station_id, &l2_station);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 station : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Adding subnet address to L3 Host table */
int
configure_host(int unit, bcm_ip6_t ip_address, bcm_if_t egr_if, bcm_vrf_t vrf)
{
    bcm_error_t      rv = BCM_E_NONE;
    bcm_l3_host_t    host;

    bcm_l3_host_t_init(&host);
    host.l3a_ip6_addr = ip_address;
    host.l3a_flags   |= BCM_L3_IP6; /* Needed for IPv6 */
    host.l3a_intf     = egr_if;
    host.l3a_vrf      = vrf;
    rv = bcm_l3_host_add(unit, &host);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 Host table : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Steps to configure IPUC forwarding*/
int
configure_ipuc_forwarding(int unit)
{
    bcm_error_t     rv = BCM_E_NONE;
    bcm_vlan_t      ingress_vlan = 5;
    bcm_vlan_t      egress_vlan = 10;
    bcm_if_t        ingress_if = 100;
    bcm_if_t        egr_if;
    bcm_if_t        egr_l3_if;
    bcm_vrf_t       vrf = 1;
    bcm_mac_t       router_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    bcm_mac_t       src_mac = {0x00, 0x00, 0x00, 0x0, 0x00, 0x11};
    bcm_mac_t       dst_mac = {0x00, 0x00, 0x00, 0x0, 0x00, 0x22};
    bcm_ip6_t       dip6 = {0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33};

    /* Cretae ingress vlan and add ingress port as member */ 
    rv = create_vlan_add_port(unit, ingress_vlan, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("create_vlan_add_port return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Cretae egress vlan and add egress port as member */
    rv = create_vlan_add_port(unit, egress_vlan, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("create_vlan_add_port return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Cretae L3 ingress interface called l3_iif */
    rv = create_l3_iif(unit, vrf, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("create_l3_iif return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* VLAN to L3_IIF binding */
    rv = configure_l3_iif_binding(unit, ingress_vlan, ingress_if);
    if (BCM_FAILURE(rv)) {
        printf("configure_l3_iif_binding return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Cretae L3 egress interface */
    rv = create_l3_intf(unit, src_mac, egress_vlan, &egr_l3_if);
    if (BCM_FAILURE(rv)) {
        printf("create_l3_intf return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port, &egress_gport));

    /* Cretae L3 egress object */
    rv = create_egr_obj(unit, egr_l3_if, dst_mac, egress_gport, egr_if);
    if (BCM_FAILURE(rv)) {
        printf("create_egr_obj return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Configure MY_STATION TCAM for router MAC */
    rv = configure_my_station_tcam(unit, router_mac);
    if (BCM_FAILURE(rv)) {
        printf("configure_my_station_tcam return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Program Host L3 table */
    rv = configure_host(unit, dip6, egr_if, vrf);
    if (BCM_FAILURE(rv)) {
        printf("configure_host return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in configure_ipuc_forwarding())
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

    print "~~~ #2) configure_ipuc_forwarding(): ** start **";
    if (BCM_FAILURE((rv = configure_ipuc_forwarding(unit)))) {
        printf("configure_ipuc_forwarding() failed.\n");
        return -1;
    }
    print "~~~ #2) configure_ipuc_forwarding(): ** end **";

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

