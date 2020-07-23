/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Feature  : L3 MPLS LSR Label Swap with trunk group functionality
 *
 * Usage    : BCM.0> cint td4_hsdk_l3mpls_lsr_swap_trunk.c
 *
 * config   : bcm56880_a0-generic-32x400.mpls-config.yml
 *
 * Log file : td4_hsdk_l3mpls_lsr_swap_trunk_log.txt
 *
 * Test Topology :
 *
 *                +---------------+
 *                |               |        +--EgressPort1--->
 *                |               |        |
 *  IngressPort-->|      TD4      |--trunk-|--EgressPort2--->
 *                |               |        |
 *                |               |        +--EgressPort3--->
 *                +---------------+
 *
 * Summary:
 * ========
 *   This Cint example to show the configuration of L3 MPLS lsr label swap
 *   with trunk group and flexdigest hashing functionality
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects two ports and configure them in Loopback mode. Out of these
 *        two ports, one port is used as ingress_port and the other as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_l3_mpls_lsr()):
 *   =========================================================
 *     a) Configure a basic L3 MPLS functional scenario and
 *        does the necessary configurations of vlan, interface, next hop and
 *        label action.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show',
 *        'l3 egress show' and 'l3 defip show'
 *
 *     b) Transmit the MPLS packet with IP unicast payload. The contents of the
 *        packet are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *      We can see that L3 mpls packet out on egress port with configured
 *      LSR functionality.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_gport_t ingress_gport;

bcm_port_t egress_port;
bcm_port_t egress_port_1;
bcm_port_t egress_port_2;

bcm_trunk_t trunk_id = 1;
bcm_gport_t trunk_gport;

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
        printf("\nError in retrieving port configuration:%s\n",bcm_errmsg(rv));
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
egress_port_setup(int unit, bcm_port_t port, int prio)
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
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionDrop);
    group_config.priority = prio;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group,
                                                               &entry));

    BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port,
                                                 BCM_FIELD_EXACT_MATCH_MASK));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                             bcmFieldActionCopyToCpu, 0, 0));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry,
                                             bcmFieldActionDrop, 0, 0));

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
    int port_list[4];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port = port_list[1];
    egress_port_1 = port_list[2];
    egress_port_2 = port_list[3];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port, 2)) {
        printf("egress_port_setup() failed for port %d\n", egress_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_1, 3)) {
        printf("egress_port_setup() failed for port %d\n", egress_port_1);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_2, 4)) {
        printf("egress_port_setup() failed for port %d\n", egress_port_1);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode +pmd");

    return BCM_E_NONE;
}

bcm_error_t
flexdigest_trunk_config(int unit)
{
    printf("flexdigest configuration for trunk hashing\n");

    int pri = 0, mask_1, mask_2;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_init(unit));

    /**** LOOKUP SETUP ****/
    bcm_flexdigest_qset_t qset;
    bcm_flexdigest_qset_t qset_get;

    bcm_flexdigest_group_t group;
    bcm_flexdigest_entry_t entry;
    bcm_flexdigest_match_id_t match_id;

    BCM_FLEXDIGEST_QSET_INIT(qset);
    BCM_FLEXDIGEST_QSET_ADD(qset, bcmFlexDigestQualifyMatchId);

    mask_1 = 0xFFFF; /*General Mask for all full fields */
    mask_2 = 0x0FFF; /*For VLAN ID from VLAN tag */

    /*** GROUP 0 (Pri = 0) for L2 packets ***/
    pri = 0;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_group_create(unit, qset, pri, &group));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_match_id_create(unit, &match_id));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL2HdrL2));

    /* Qualifiers */
    BCM_IF_ERROR_RETURN(bcm_flexdigest_qualify_MatchId(unit, entry, match_id));

    /* Actions : Bin field population */
    /* Set A */
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds0, BCM_FLEXDIGEST_FIELD_INGRESS_PP_PORT, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds1, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_0_15, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds2, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_16_31, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds3, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_32_47, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds4, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_0_15, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds5, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_16_31, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds6, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_32_47, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds7, BCM_FLEXDIGEST_FIELD_OUTER_OTAG_0_15, mask_2));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_entry_install(unit, entry));

    /**** HASH COMPUTATION SETUP ****/
    int hash_profile_id = 0;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_create(unit, BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &hash_profile_id));

    int enable = 1;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlPreProcessBinA, enable));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlPreProcessBinB, enable));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlPreProcessBinC, enable));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlBinA0FunctionSelection, BCM_FLEXDIGEST_HASH_FUNCTION_CRC16));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlBinA1FunctionSelection, BCM_FLEXDIGEST_HASH_FUNCTION_CRC32LO));

    /**** HASH BITS SELECTION SETUP ****/
    int trunk_uc_flow_select = 0; /*Use Port based hash */
    int trunk_uc_offset = 1; /*A0 block with Offset 1*/

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelTrunkUc, trunk_uc_flow_select));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchTrunkUnicastHashOffset, trunk_uc_offset));

    return BCM_E_NONE;
}

bcm_error_t
create_trunk(int unit, bcm_trunk_t *tid, int count, bcm_port_t *member_port)
{
    int rv, i;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t *member_info;
    bcm_trunk_member_t info;
    int mem_size;

    bcm_trunk_info_t_init(&trunk_info);

    mem_size = sizeof(info) * count;
    member_info = sal_alloc(mem_size, "trunk members");

    for (i = 0 ; i < count ; i++) {
      bcm_trunk_member_t_init(&member_info[i]);
      BCM_GPORT_MODPORT_SET(member_info[i].gport, 0, member_port[i]);
    }

    trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW;
    trunk_info.dlf_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index= BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index= BCM_TRUNK_UNSPEC_INDEX;

    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, tid);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_trunk_create(): %s.\n", bcm_errmsg(rv));
      return rv;
    }
    printf("Trunk Create with tid = %d\n", *tid);

    rv = bcm_trunk_set(unit, *tid, &trunk_info, count, member_info);
    if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_trunk_set(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    return rv;
}

bcm_error_t config_mpls_trunk_flexdigest(int unit, bcm_trunk_t trunk_id)
{
    int flags;
    int rv;

    bcm_port_t trunk_ports[3];
    bcm_l2_addr_t l2addr;
    trunk_ports[0] = egress_port;
    trunk_ports[1] = egress_port_1;
    trunk_ports[2] = egress_port_2;

    rv = create_trunk(unit, &trunk_id, 3, &trunk_ports);
    if (BCM_FAILURE(rv)) {
      printf("Error executing create_trunk(): %s.\n", bcm_errmsg(rv));
      return rv;
    }

    rv = flexdigest_trunk_config(unit);
    if (BCM_FAILURE(rv)) {
      printf("Error executing flexdigest_trunk_config: %s.\n", bcm_errmsg(rv));
      return rv;
    }

    return BCM_E_NONE;

}

void
verify(int unit)
{
    char str[512];

    bshell(unit, "vlan show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "clear c");
    printf("\nSending 10 MPLS packets with source mac increment on ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 10 pbm=%d SourceMacInc=1 data=0x0000000013030000000000AA8100000C884700301140450000400000000040FF9B0B0A0A0A01C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2BE3128EDB; sleep quiet 3", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    return;
}

bcm_error_t
config_l3_mpls_lsr(int unit)
{
    bcm_error_t rv;
    bcm_vlan_t in_vlan = 12;
    bcm_vlan_t out_vlan = 22;
    bcm_pbmp_t pbmp, ubmp;
    bcm_mac_t router_mac = {0x00, 0x00, 0x00, 0x00, 0x13, 0x03};
    bcm_mac_t local_mac = {0x00, 0x00, 0x00, 0x00, 0x11, 0x11};
    bcm_mac_t nh_mac    = {0x00, 0x00, 0x00, 0x00, 0x22, 0x22};
    uint32 tunnel_label_term = 0x301;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egress;
    bcm_if_t egr_obj;

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, ingress_port, &ingress_gport));
    printf("ingress_gport=0x%x\n", ingress_gport);

    /* Create vlan and add port to vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, in_vlan));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, in_vlan, pbmp, ubmp));

    /* Create vlan and add port to vlan */
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, out_vlan));
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port);
    BCM_PBMP_PORT_ADD(pbmp, egress_port_1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port_2);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, out_vlan, pbmp, ubmp));

    printf("MPLS LSR Trunk ports configuration\n");
    rv = config_mpls_trunk_flexdigest(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_mpls_trunk_flexdigest: %s\n", bcm_errmsg(rv));
        return rv;
    }
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);

    /* Create L3 interface  */
    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, 6);
    l3_intf.l3a_vid = out_vlan;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    printf("l3 intf = %d\n", l3_intf.l3a_intf_id);

    /* Create Egress object for label swap */
    bcm_l3_egress_t_init(&l3_egress);
    sal_memcpy(l3_egress.mac_addr, nh_mac, 6);
    l3_egress.intf     = l3_intf.l3a_intf_id;
    l3_egress.vlan     = out_vlan;
    l3_egress.port     = trunk_gport;    /* trunk gport id */
    l3_egress.mpls_label = 0x888;
    l3_egress.mpls_exp = 0x2;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj));

    /* Add MPLS table entry for incoming label matching */
    bcm_mpls_tunnel_switch_t_init (&tunnel_switch);
    tunnel_switch.label   = tunnel_label_term;
    tunnel_switch.port    = BCM_GPORT_INVALID;
    tunnel_switch.action  = BCM_MPLS_SWITCH_ACTION_SWAP;
    tunnel_switch.egress_if = egr_obj;
    rv = bcm_mpls_tunnel_switch_add(unit, &tunnel_switch);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_mpls_tunnel_switch_add: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Enable MPLS Lookup for incoming packet */
    BCM_IF_ERROR_RETURN(bcm_l2_tunnel_add(unit, router_mac, in_vlan));

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_l3_mpls_lsr())
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

    print "~~~ #2) config_l3_mpls_lsr(): ** start **";
    if (BCM_FAILURE((rv = config_l3_mpls_lsr(unit)))) {
        printf("config_l3_mpls_lsr() failed.\n");
        return -1;
    }
    print "~~~ #2) config_l3_mpls_lsr(): ** end **";

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
