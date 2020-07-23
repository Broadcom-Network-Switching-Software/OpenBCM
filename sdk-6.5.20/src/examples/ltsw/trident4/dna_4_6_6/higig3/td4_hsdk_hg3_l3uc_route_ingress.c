/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPv4 routing over HiGig3 at ingress node
 *
 * Usage    : BCM.0> cint td4_hsdk_hg3_l3uc_route_ingress.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml 
 *
 * Log file : td4_hsdk_hg3_l3uc_route_ingress_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |  S
 *                  |                              | 
 * ingress_vlan 5   |                              |  egress vlan 10   
 * ingress_port cd0 |                              |  egress_port hg0
 * +----------------+          SWITCH-TD4      HG3 +-----------------+  
 * src_mac 0x01     |                              |  src_mac 0x11  
 * dst_mac 0x02     |                              |  dst_mac 0x22 
 * DIP 10.10.10.10  |                              |  HG_HDR 
 * SIP 20.20.20.20  |                              |    - SYSTEM_SOURCE = 0x401
 *                  |                              |    - SYSTEM_DESTINATION=0x214
 *                  +------------------------------+
 *
 *                                                                        
 * Summary:
 * ---------
 *   This Cint example to show configuration of the IPv4 route over HiGig3
 *   at ingress node using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * ---------------------------------------
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   ---------------------------------------------
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 * 
 *     b) egress port is configured as Higig3.
 *
 *     c) Install an IFP rule to copy incoming and outgoing packets to CPU and
 *        start packet watcher to display packet contents.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 * 
 *   2) Step2 - Higig3 forwarding setup (Done in hg3_forwarding_setup()):
 *   ---------------------------------------------------------------
 *      a) my_modid configuration
 *      b) Enable forwarding ports for remote module
 *      c) Map remote system ports to local HiGig port
 *
 *   3) Step3 - Configuration (Done in configure_ipuc_forwarding()):
 *   ---------------------------------------------------------------
 *     a) Configure a basic IPv4 route/entry functional scenario. This adds the
 *        route in L3 LPM/ALPM table and does the necessary configurations of vlan, L3_IIF,
 *        egress interface and next hop.
 *
 *   4) Step4 - Verification (Done in verify()):
 *   -------------------------------------------
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show' and 'l3 route show'
 *
 *     b) Transmit the known IPv4 unicast packet. The contents of the packet
 *        are printed on screen. The packet used matches the route configured
 *        through script.
 *
 *     c) Expected Result:
 *     -------------------
 *       We can see that dmac, smac, TTL and vlan are all changed as the packet is
 *       routed through the egress port with HG3 header and 'show c' to check the 
 *       Tx/Rx packet stats/counters.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t    ingress_port;
bcm_port_t    egress_port;
bcm_gport_t   ingress_gport;
bcm_gport_t   egress_gport;

int             my_modid = 4;
int             dest_modid = 2;
bcm_port_t         dest_port = 20;

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
 * egress_hg3_port_setup() is to replace egress_port_setup() in hg3 tests.
 * It uses IFP rule to qualify egress loopback packets with HG3 header and
 * copy to cpu.
 * Note that: the ingress port of HG3 loopback packets are the same as
 * ingress packet, match rule ID is added to distinguish ingress and egress
 * packets captured by IFP.
 */
bcm_error_t
egress_hg3_port_setup(int unit, bcm_port_t port)
{
    bcm_field_entry_t           entry;
    bcm_field_group_config_t    group_config; 
    bcm_field_presel_config_t   presel_config;
    bcm_field_presel_t          presel_id;
    bcm_field_presel_group_config_t gcfg_pre;
    bcm_port_encap_config_t encap_config;
    bcm_gport_t                 gport;
    bcm_error_t                 rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

    /* HiGig3 Port encap */
    printf ("Config egress port %d as HiGig3 port.\n",  port);
    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port, &gport));
    encap_config.encap = BCM_PORT_ENCAP_HIGIG3;
    rv = bcm_port_encap_config_set (unit, gport, &encap_config);
    if (BCM_FAILURE(rv)) {
        printf("bcm_port_encap_config_set return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_field_presel_group_config_t_init(&gcfg_pre);
    BCM_FIELD_QSET_INIT(gcfg_pre.qset);
    BCM_FIELD_QSET_ADD(gcfg_pre.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(gcfg_pre.qset, bcmFieldQualifyL2PktType);
    BCM_IF_ERROR_RETURN( bcm_field_presel_group_config_create(unit, &gcfg_pre) );

    bcm_field_presel_config_t_init(&presel_config);
    presel_config.stage = bcmFieldStageIngress;
    BCM_IF_ERROR_RETURN( bcm_field_presel_config_create(unit, &presel_config) );
    presel_id = presel_config.presel_id;
    printf ("presel_config created, presel_id = %d\n", presel_id);

    BCM_IF_ERROR_RETURN(bcm_field_qualify_L2PktType(unit, BCM_FIELD_QUALIFY_PRESEL | presel_id, bcmFieldL2PktTypeHG3Base));

    presel_id = presel_id ^ BCM_FIELD_IFP_ID_BASE;
    bcm_field_group_config_t_init(&group_config);
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESEL_GROUP;
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_ASET;
    BCM_FIELD_PRESEL_ADD(group_config.preselset, presel_id);
    group_config.presel_group = gcfg_pre.presel_group;

    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifySrcPort);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = 2;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    /* Copy to CPU with MATCHED_RULE set as the egress port */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 1, port)); 
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    printf("egress_hg3_port_setup configured for egress_port : %d\n", port);

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

    if (BCM_E_NONE != egress_hg3_port_setup(unit, egress_port)) {
        printf("egress_hg3_port_setup() failed for port %d\n", egress_port);
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
 *       0800 4502 002e 0000 0000 40ff 3d94 1414
 *       1414 0a0a 0a0a 0000 0000 0000 0000 0000
 *       0000 0000 0000 0000 0000 0000 0000 0000
 *       f3b4 2e69
 *
 *  Out Packet: 
 *  -----------
 *       0000 0000 0022 0000 0000 0011 8100 000a
 *       0800 4502 002e 0000 0000 3fff 3e94 1414
 *       1414 0a0a 0a0a 0000 0000 0000 0000 0000
 *       0000 0000 0000 0000 0000 0000 0000 0000
 *       f3b4 2e69
 *  Note: HG header is removed when packet is copied to CPU port. 
 *  HG3 fields can be verified by print out the PMD information of the packets
 *  [RX metadata information]
 *       PKT_LENGTH                      :0x48(72)
 *       SRC_PORT_NUM                    :0x2(2)
 *       SWITCH                          :0x1(1)
 *       MATCH_ID_LO                     :0x500a26(5245478)
 *       MPB_FLEX_DATA_TYPE              :0x2(2)
 *       EGR_ZONE_REMAP_CTRL             :0x1(1)
 *       COPY_TO_CPU                     :0x1(1)
 *  [FLEX fields]
 *       EVENT_TRACE_VECTOR_31_16        :0x4
 *       DROP_CODE_15_0                  :0x909
 *       PARSER_VHLEN_0_15_0             :0x8a
 *       VFI_15_0                        :0xa
 *       L2_IIF_10_0                     :0x1
 *       SYSTEM_SOURCE_15_0              :0x401
 *       L2_OIF_10_0                     :0xb4
 *       INGRESS_PP_PORT_7_0             :0x1
 *       SYSTEM_DESTINATION_15_0         :0x214
 *       EFFECTIVE_TTL_7_0               :0x3f
 *       VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0:0x2
 *       SYSTEM_OPCODE_3_0               :0x1
 *       PKT_MISC_CTRL_0_3_0             :0x1
 *  [RX reasons]
 *       IFP
 *       MATCHED_RULE_BIT_1
 */

void
verify(int unit)
{
    char str[512];

    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "l3 route show");
    bshell(unit, "clear c");
    printf("\nSending IPv4 unicast packet to ingress_port : %d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000020000000000018100000508004502002e0000000040ff3d94141414140a0a0a0a0000000000000000000000000000000000000000000000000000f3b42e69; sleep quiet 1", ingress_port);
    bshell(unit, str);
    sal_sleep(2);
    bshell(unit, "l3 route show");
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

/* Create default L3 egress object */
int
create_def_egr_obj(int unit, bcm_if_t l3_if, bcm_if_t *egr_if, int my_modid) 
{
    bcm_error_t     rv = BCM_E_NONE;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf  = l3_if;
    l3_egr.flags = BCM_L3_COPY_TO_CPU | BCM_L3_DST_DISCARD;
    l3_egr.module = my_modid;

    rv = bcm_l3_egress_create(unit, 0, &l3_egr, egr_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring def L3 egress object : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    printf("egress object id : %d created\n", *egr_if);

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

/* Adding subnet address to LPM/ALPM table */
int
configure_route(int unit, bcm_ip_t subnet_addr, bcm_ip_t subnet_mask, bcm_if_t egr_if,
                bcm_vrf_t vrf)
{
    bcm_error_t      rv = BCM_E_NONE;
    bcm_l3_route_t   route;

    bcm_l3_route_t_init(&route);
    route.l3a_subnet  = subnet_addr;
    route.l3a_ip_mask = subnet_mask;
    route.l3a_intf    = egr_if;
    route.l3a_vrf     = vrf;
    rv = bcm_l3_route_add(unit, &route);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring L3 LPM/ALPM table : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}



/* Configure HiGig3 local/remote forwarding */
int
hg3_forwarding_setup(int unit) 
{
    
    bcm_error_t     rv = BCM_E_NONE;

    bcm_pbmp_t        pbmp;
    int             port; 

    
    /* Stacking Configurations (1/3): my modid */
    rv = bcm_stk_my_modid_set(unit, my_modid); 
    if (BCM_FAILURE(rv)) {
        printf("bcm_stk_my_modid_set return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    
    /* Stacking Configurations (2/3): Enable forwarding ports for remote module */
    BCM_PBMP_CLEAR(pbmp);
    for (port = 1; port <= 32; port++) {
        BCM_PBMP_PORT_ADD(pbmp, port);
    }
    
    rv = bcm_stk_module_remote_port_enable_set(unit, dest_modid, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stk_module_remote_port_enable_set return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    /* Stacking Configurations (3/3): Map remote system ports to local HiGig port  */
    rv = bcm_stk_modport_set(unit, dest_modid, egress_port);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stk_modport_set return error : %s.\n", bcm_errmsg(rv));
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
    bcm_if_t        def_egr_if;
    bcm_if_t        egr_if;
    bcm_if_t        egr_l3_if;
    bcm_vrf_t       vrf = 1;
    bcm_mac_t       router_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    bcm_mac_t       src_mac = {0x00, 0x00, 0x00, 0x0, 0x00, 0x11};
    bcm_mac_t       dst_mac = {0x00, 0x00, 0x00, 0x0, 0x00, 0x22};
    bcm_ip_t        dip = 0x0a0a0a0a;
    bcm_ip_t        dip_mask = 0xFFFFFF00;
    bcm_ip_t        def_dip = 0x00000000;
    bcm_ip_t        def_dip_mask = 0x00000000;
    
    

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

    /* Cretae Default L3 egress object for default route */
    rv = create_def_egr_obj(unit, egr_l3_if, def_egr_if, my_modid);
    if (BCM_FAILURE(rv)) {
        printf("create_def_egr_obj return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* egress_gport is resolved from modid + port in stacking system */
    BCM_GPORT_MODPORT_SET(egress_gport, dest_modid, dest_port);
    printf ( "egress_gport = %d \n", egress_gport);

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

    /* Program default ROUTE in LPM/ALPM table */
    rv = configure_route(unit, def_dip, def_dip_mask, def_egr_if, vrf);
    if (BCM_FAILURE(rv)) {
        printf("configure_route return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Program ROUTE in LPM/ALPM table */
    rv = configure_route(unit, dip, dip_mask, egr_if, vrf);
    if (BCM_FAILURE(rv)) {
        printf("configure_route return error : %s.\n", bcm_errmsg(rv));
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
    
    print "~~~ #2) hg3_forwarding_setup(): ** start **";
    if (BCM_FAILURE((rv = hg3_forwarding_setup(unit)))) {
        printf("hg3_forwarding_setup() failed.\n");
        return -1;
    }
    print "~~~ #2) hg3_forwarding_setup(): ** end **";
    
    print "~~~ #3) configure_ipuc_forwarding(): ** start **";
    if (BCM_FAILURE((rv = configure_ipuc_forwarding(unit)))) {
        printf("configure_ipuc_forwarding() failed.\n");
        return -1;
    }
    print "~~~ #3) configure_ipuc_forwarding(): ** end **";

    print "~~~ #4) verify(): ** start **";
    verify(unit);
    print "~~~ #4) verify(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print "execute(): Start";
  print execute();
  print "execute(): End";
}
