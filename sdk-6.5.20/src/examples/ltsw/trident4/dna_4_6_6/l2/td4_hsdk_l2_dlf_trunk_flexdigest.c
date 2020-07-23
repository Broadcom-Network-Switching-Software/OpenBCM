/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Feature  : VLAN/VFI flooding for unicast DLF packets on trunk ports using flexdigest.
 *
 * Usage    : BCM.0> cint td4_hsdk_l2_dlf_trunk_flexdigest.c
 *
 * config   : bcm56880_a0-generic-l2.config.yml
 *
 * Log file : td4_hsdk_l2_dlf_trunk_flexdigest_log.txt
 *
 * Test Topology :

 *
 *                +-----------+
 *                |           |        +--EgressPort1--->
 *                |           |        |
 *  IngressPort-->|     TD4   |--trunk-|
 *                |           |        |
 *                |           |        +--EgressPort2--->
 *                +-----------+
 *
 * Summary:
 * ========
 * cint script to demonstrate L2 forwarding for DLF packets on trunk ports.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. Out of these
 *        three ports, one port is used as ingress_port and the other two as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy ingress & egress packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a verification purpose and is not part of the feature configuration.
 *
 *   2) Step2 - Configuration (Done in):
 *   =========================================================
 *     a) Ports are configured in the specific vlan/vfi
 *     b) Enable outer tag to vfi mapping at port VLAN_TO_VFI_MAPPING_ENABLE
 *     c) L2 static entry is setup for unicast forwarding on trunk group.
 *     d) L2 learn mode is setup on ingress and egress ports
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'vlan show' 'l2 show' 'show c' 'trunk show'
 *     b) Transmit the packet on the ingress port side
 *
 *     c) Expected Result:
 *     ===================
 *     The packet capture shows the egress packet transmitted on outgoing ports as per the forwarding path setup
 *
 *   Ingress Packet:
 *
 *   Egress Packet:
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port_1;
bcm_port_t egress_port_2;
bcm_gport_t ingress_phy_gport;
bcm_gport_t egress_phy_gport_1;
bcm_gport_t egress_phy_gport_2;

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
    int i = 0, port = 0;
    bcm_error_t rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

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
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t
ingress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_entry_t entry;
    bcm_port_t     port;
    bcm_field_group_config_t group_config;

    bcm_field_group_config_t_init(&group_config);
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionCopyToCpu);
    group_config.priority = 1;
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

    BCM_PBMP_ITER(pbm, port) {
        printf("ingress_port_multi_setup port: %d setup\n", port);
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

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
 * loopback of the packet from the egress port.
 */
bcm_error_t
egress_port_multi_setup(int unit, bcm_pbmp_t pbm)
{
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_port_t     port;

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

        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));

        BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));

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
    int num_ports = 3;
    int port_list[num_ports];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port_1 = port_list[1];
    egress_port_2 = port_list[2];

    if (BCM_E_NONE !=bcm_port_gport_get(unit, ingress_port, &(ingress_phy_gport))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != bcm_port_gport_get(unit, egress_port_1, &(egress_phy_gport_1))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", egress_port_1);
        return -1;
    }
    if (BCM_E_NONE != bcm_port_gport_get(unit, egress_port_2, &(egress_phy_gport_2))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", egress_port_2);
        return -1;
    }


    bcm_pbmp_t  pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, pbmp)) {
        printf("ingress_port_multi_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port_1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port_2);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for egress ports \n");
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

/*Verify function call*/
void
verify(int unit)
{
    char str[512];

    print "vlan show";
    bshell(unit, "vlan show");
    print "l2 show";
    bshell(unit, "l2 show");
    print "trunk show";
    bshell(unit, "trunk show");
    bshell(unit, "clear c");
    printf("\nSending 10 unicast packets with increment source mac on ingress port:%d\n",ingress_port);
    snprintf(str, 512, "tx 10 pbm=%d DestMac=0x11 SourceMac=0x3214 SourceMacInc=1 vlan=20 len=80; sleep quiet 1", ingress_port);
    bshell(unit, str);
    print "show c";
    bshell(unit, "show c");

    printf("\nverify l2 learning:\n");
    print "l2 show";
    bshell(unit, "l2 show");

    return;
}

/* Configure port to vlan as tagged or untagged*/
bcm_error_t
vlan_add_port(int unit, int vid, bcm_port_t t_port, bcm_port_t ut_port)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, t_port);
    BCM_PBMP_PORT_ADD(upbmp, ut_port);
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_port_add: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

/* Static L2 entry for forwarding */
bcm_error_t
add_l2_static_entry(int unit,uint32 flag,bcm_mac_t mac,bcm_vpn_t vpn_id,bcm_trunk_t tgid)
{
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC | flag;
    l2_addr.tgid = tgid;
    return bcm_l2_addr_add(unit, &l2_addr);
}

/*Setup learning on port*/
bcm_error_t
port_learn_mode_set(int unit, bcm_port_t port, uint32 flag)
{
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_port_learn_set(unit, port, flag);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_port_learn_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

bcm_error_t
flexdigest_trunk_config(int unit)
{
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

    int trunk_nonuc_flow_select = 0; /*Use Port based hash */
    int trunk_nonuc_offset = 1; /*A0 block with Offset 1*/

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelTrunkNonUnicast, trunk_nonuc_flow_select));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchTrunkNonUnicastHashOffset, trunk_nonuc_offset));

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
  /* trunk_info.psc = BCM_TRUNK_PSC_RANDOMIZE; */
  trunk_info.dlf_index= BCM_TRUNK_UNSPEC_INDEX;
  trunk_info.mc_index= BCM_TRUNK_UNSPEC_INDEX;
  trunk_info.ipmc_index= BCM_TRUNK_UNSPEC_INDEX;

  rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, tid);
  printf("Trunk Create, tid=%d, rv=0x%x \n", *tid, rv);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_trunk_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_trunk_set(unit, *tid, &trunk_info, count, member_info);
  printf("Trunk Set 0x%x \n", rv);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_trunk_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  return rv;
}

bcm_error_t config_l2_trunk_flexdigest(int unit, bcm_trunk_t trunk_id)
{

  int flags;
  int rv;

  bcm_port_t trunk_ports[2];
  bcm_l2_addr_t l2addr;
  trunk_ports[0] = egress_port_1;
  trunk_ports[1] = egress_port_2;

  rv = create_trunk(unit, &trunk_id, 2, &trunk_ports);
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


/* L2 switch configuration function */
bcm_error_t
setup_l2_forwarding(int unit)
{
    bcm_vlan_t vlanId=20, default_vlan=20;
    bcm_error_t rv = BCM_E_NONE;
    bcm_multicast_t l2mc_group = 20;
    uint32_t flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID;
    bcm_vlan_control_vlan_t vlan_control;


    /*Create the vlan with specific id*/
    printf("create vlan %d\n", vlanId);
    rv = bcm_vlan_create(unit, vlanId);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_create: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*Add ports to vlan*/
    rv = vlan_add_port(unit, vlanId, ingress_port, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vlan_add_port(unit, vlanId, egress_port_1, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vlan_add_port(unit, vlanId, egress_port_2, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_multicast_create(unit, flags, &l2mc_group);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_multicast_egress_add(unit, l2mc_group, ingress_phy_gport, -1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_egress_add: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_multicast_egress_add(unit, l2mc_group, egress_phy_gport_1, -1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_egress_add: %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_multicast_egress_add(unit, l2mc_group, egress_phy_gport_2, -1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_multicast_egress_add: %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_vlan_control_vlan_t_init(&vlan_control);
    flags = BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK |
            BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK |
            BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK;
    vlan_control.broadcast_group = l2mc_group;
    vlan_control.unknown_multicast_group = l2mc_group;
    vlan_control.unknown_unicast_group = l2mc_group;

    rv = bcm_vlan_control_vlan_selective_set(unit, vlanId, flags, &vlan_control);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_vlan_control_vlan_selective_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*Setup identity mapping of VLAN_ID to VFI at ingress port*/
    /*it is enabled by default in the switch*/
    rv = bcm_port_control_set(unit, ingress_port, bcmPortControlTrustIncomingVlan,1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_control_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*Learn mode port config*/
    rv = port_learn_mode_set(unit, ingress_port, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*Drop egress_port received packet to avoid looping back*/
    rv = port_learn_mode_set(unit, egress_port_1, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }
    /*Drop egress_port received packet to avoid looping back*/
    rv = port_learn_mode_set(unit, egress_port_2, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Trunk ports configuration\n");
    rv = config_l2_trunk_flexdigest(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_l2_trunk_flexdigest: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in setup_l2_forwarding())
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

    print "~~~ #2) setup_l2_forwarding(): ** start **";
    if (BCM_FAILURE((rv = setup_l2_forwarding(unit)))) {
        printf("setup_l2_forwarding() failed.\n");
        return -1;
    }
    print "~~~ #2) setup_l2_forwarding(): ** end **";

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
