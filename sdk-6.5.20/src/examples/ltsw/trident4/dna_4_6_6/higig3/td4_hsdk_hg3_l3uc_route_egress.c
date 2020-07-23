/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPv4 routing over HiGig3 at ingress node
 *
 * Usage    : BCM.0> cint td4_hsdk_hg3_l3uc_route_egress.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml 
 *
 * Log file : td4_hsdk_hg3_l3uc_route_egress_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |  
 *                  |                              | 
 * ingress_vlan 10  |                              |  egress vlan 10   
 * ingress_port hg0 |                              |  egress_port cd0 
 * +----------------+          SWITCH-TD4          +-----------------+   
 * src_mac 0x01     |                              |  src_mac 0x11  
 * dst_mac 0x02     |                              |  dst_mac 0x22 
 * HG3_HDR          |                              |  DIP 10.10.10.10
 * DIP 10.10.10.10  |                              |  SIP 20.20.20.20
 * SIP 20.20.20.20  |                              |
 *                  |                              |
 *                  +------------------------------+
 *
 *                                                                        
 * Summary:
 * --------
 *   This Cint example to show configuration of the IPv4 route over HiGig3
 *   at egress node using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * ---------------------------------------
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   ---------------------------------------------
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 * 
 *     b) Ingress port is configured as Higig3.
 *
 *     c) Install an IFP rule to copy incoming and outgoing packets to CPU and
 *        start packet watcher to display packet contents.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Higig3 forwarding setup (Done in hg3_forwarding_setup()):
 *   ------------------------------------------------------------------------    
 *      a) my_modid configuration
 *      b) Enable forwarding ports for remote module
 *      c) Map remote system ports to local HiGig port      
 *
 *   3) Step3 - Verification (Done in verify()):
 *   -------------------------------------------
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show' and 'l3 route show'
 *
 *     b) Transmit the known IPv4 unicast packet with hg3 header. The contents 
 *        of the packet are printed on screen. The packet used matches the route 
 *        configured through script.
 *
 *     c) Expected Result:
 *     -------------------
 *       We can see that  the packet is switched through the egress port and 
 *       'show c' to check the Tx/Rx packet stats/counters.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t    ingress_port;
bcm_port_t    egress_port;
bcm_gport_t   ingress_gport;
bcm_gport_t   egress_gport;
int           transit = 0;

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
 * ingress_hg3_port_setup() is to replace egress_port_setup() in hg3 tests.
 * It uses IFP rule to qualify egress loopback packets with HG3 header and
 * copy to cpu.
 * Note that: the ingress port of HG3 loopback packets are the same as
 * ingress packet, match rule ID is added to distinguish ingress and egress
 * packets captured by IFP.
 */
bcm_error_t
ingress_hg3_port_setup(int unit, bcm_port_t port)
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
    group_config.priority = 1;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
    /* Copy to CPU with MATCHED_RULE set as the ingress port */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 1, port)); 

    BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

    printf("ingress_hg3_port_setup configured for ingress_port : %d\n", port);

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

    if (BCM_E_NONE != ingress_hg3_port_setup(unit, ingress_port)) {
        printf("ingress_hg3_port_setup() failed for port %d\n", ingress_port);
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
 * [RX metadata information]
 *         PKT_LENGTH                      :0x48(72)
 *         SRC_PORT_NUM                    :0x1(1)
 *         SWITCH                          :0x1(1)
 *         MATCH_ID_LO                     :0x500a26(5245478)
 *         MPB_FLEX_DATA_TYPE              :0x2(2)
 *         EGR_ZONE_REMAP_CTRL             :0x1(1)
 *         COPY_TO_CPU                     :0x1(1)
 * [FLEX fields]
 *         EVENT_TRACE_VECTOR_31_16        :0x4
 *         EVENT_TRACE_VECTOR_47_32        :0x1
 *         PARSER_VHLEN_0_15_0             :0x8a
 *         VFI_15_0                        :0xa
 *         L2_IIF_10_0                     :0xa1
 *         SYSTEM_SOURCE_15_0              :0x401
 *         L2_OIF_10_0                     :0x2
 *         INGRESS_PP_PORT_7_0             :0x1
 *         SYSTEM_DESTINATION_15_0         :0x202
 *         EFFECTIVE_TTL_7_0               :0x3f
 *         VLAN_TAG_PRESERVE_CTRL_SVP_MIRROR_ENABLE_1_0:0x2
 *         SYSTEM_OPCODE_3_0               :0x1
 *         PKT_MISC_CTRL_0_3_0             :0x1
 * [RX reasons]
 *         IFP
 *         MATCHED_RULE_BIT_0
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
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000022000000000011af0402%02d840100008100000a08004502002e000000003fff3e94141414140a0a0a0a0000000000000000000000000000000000000000000000000000f3b42e69; sleep quiet 1", ingress_port, egress_port);
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

/* Configure my_modidy and enable stacking forwarding to ports in remote device */
int
hg3_forwarding_setup(int unit, int transit) 
{
    
    bcm_error_t     rv = BCM_E_NONE;
    bcm_pbmp_t  pbmp, ubmp;
    
    bcm_vlan_t      vlan = 0xa;
    int             my_modid = 0xa;
    int             ingress_modid = 0x4;
    int             egress_modid = 0x2;
    int             port;
    
    
    if (!transit) {
        my_modid = egress_modid;
    }
    
    /* Add Higig3 ports to vlan*/
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in bcm_vlan_create : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in bcm_vlan_port_add : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    
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
    
    rv = bcm_stk_module_remote_port_enable_set(unit, ingress_modid, pbmp);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stk_module_remote_port_enable_set return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    
    if (transit) {
        rv = bcm_stk_module_remote_port_enable_set(unit, egress_modid, pbmp);
        if (BCM_FAILURE(rv)) {
            printf("bcm_stk_module_remote_port_enable_set return error : %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }
    
    /* Stacking Configurations (3/3): Map remote system ports to local HiGig port  */
    rv = bcm_stk_modport_set(unit, ingress_modid, ingress_port);
    if (BCM_FAILURE(rv)) {
        printf("bcm_stk_modport_set return error : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    if (transit) {
        rv = bcm_stk_modport_set(unit, egress_modid, egress_port);
        if (BCM_FAILURE(rv)) {
            printf("bcm_stk_modport_set return error : %s.\n", bcm_errmsg(rv));
            return rv;
        }
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
    if (BCM_FAILURE((rv = hg3_forwarding_setup(unit, transit)))) {
        printf("hg3_forwarding_setup() failed.\n");
        return -1;
    }
    print "~~~ #2) hg3_forwarding_setup(): ** end **";

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
