/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Feature  : L2 callback function demo for trunk
 *
 * Usage    : BCM.0> cint td4_hsdk_l2_callback_register_on_trunk.c
 *
 * config   : bcm56880_a0-generic-l2.config.yml
 *
 * Log file : td4_hsdk_l2_callback_register_on_trunk_log.txt
 *
 * Test Topology :
 *
 *                      +-------------------+
 *                      |                   |
 *                      |        TD4        |
 *       ingress_port   |                   |  egress_port
 *           ---------->+                   +---------->
 *             vfi 20   |                   |  vfi 20
 *                      |                   |  dst_mac 00:00:00:00:00:11
 *                      |                   |
 *                      |                   |
 *                      +-------------------+
 *
 * Summary:
 * ========
 * cint script to demonstrate L2 callback function with setup forwarding ingress untagged
 * packets on switch using default VFI configuration
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects four ports and configure them in Loopback mode. Out of these
 *        two ports, two ports is used as ingress_port and two as
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
 *     b) L2 static entry is setup for unicast forwarding
 *     c) L2 learn mode is setup on ingress and egress ports
 *     d) L2 callback function is setup to verify updates in l2 entry
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the outputs of 'vlan show' 'l2 show' 'show c'
 *     b) Transmit the packet on the ingress port side
 *
 *     c) Expected Result:
 *     ===================
 *     The packet capture shows the egress packet transmitted on outgoing port as per the forwarding path setup
 *     SMAC learning should call registered function
 *
 *   Ingress Packet:
 *   [bcmPWN.0]Packet[1]: data[0000]: {000000000011} {000000000002} 8100 0014
 *   [bcmPWN.0]Packet[1]: data[0010]: 0806 0001 0800 0604 0001 0000 0000 0002
 *   [bcmPWN.0]Packet[1]: data[0020]: 0a0a 0a02 0000 0000 0000 1414 1402 0001
 *   [bcmPWN.0]Packet[1]: data[0030]: 0203 0405 0607 0809 0a0b 0c0d bc94 977f
 *
 *   Egress Packet:
 *   [bcmPWN.0]Packet[2]: data[0000]: {000000000011} {000000000002} 8100 0001
 *   [bcmPWN.0]Packet[2]: data[0010]: 0806 0001 0800 0604 0001 0000 0000 0002
 *   [bcmPWN.0]Packet[2]: data[0020]: 0a0a 0a02 0000 0000 0000 1414 1402 0001
 *   [bcmPWN.0]Packet[2]: data[0030]: 0203 0405 0607 0809 0a0b 0c0d bc94 977f
 *
 *   L2_EVENT! MAC: 00:00:00:00:00:02  VID:20  PORT:1  Operation: 3
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t egress_port_1;
bcm_port_t egress_port_2;
bcm_port_t ingress_port_1;
bcm_port_t ingress_port_2;
bcm_gport_t egress_phy_gport_1;
bcm_gport_t egress_phy_gport_2;
bcm_gport_t ingress_phy_gport_1;
bcm_gport_t ingress_phy_gport_2;
bcm_trunk_t ing_tid = 1;
bcm_trunk_t egr_tid = 2;
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
    int num_ports = 4;
    int port_list[num_ports];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, num_ports)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port_1 = port_list[0];
    ingress_port_2 = port_list[1];
    egress_port_1 = port_list[2];
    egress_port_2 = port_list[3];

    if (BCM_E_NONE !=bcm_port_gport_get(unit, ingress_port_1, &(ingress_phy_gport_1))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", ingress_port_1);
        return -1;
    }

    if (BCM_E_NONE !=bcm_port_gport_get(unit, ingress_port_2, &(ingress_phy_gport_2))){
        printf("bcm_port_gport_get() failed to get gport for port %d\n", ingress_port_2);
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
    BCM_PBMP_PORT_ADD(pbmp, ingress_port_1);
    BCM_PBMP_PORT_ADD(pbmp, ingress_port_2);
    if (BCM_E_NONE != ingress_port_multi_setup(unit, pbmp)) {
        printf("ingress_port_multi_setup() failed for port %d\n", ingress_port_1);
        return -1;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, egress_port_1);
    BCM_PBMP_PORT_ADD(pbmp, egress_port_2);
    if (BCM_E_NONE != egress_port_multi_setup(unit, pbmp)) {
        printf("egress_port_multi_setup() failed for port %d\n", egress_port_2);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode");

    return BCM_E_NONE;
}

/*function callback to display l2 entries*/
bcm_error_t
l2_show_traverse (int unit, bcm_l2_addr_t* l2_entry, void* data)
{
    printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x ",l2_entry->mac[0],
                l2_entry->mac[1],l2_entry->mac[2],l2_entry->mac[3],
                                l2_entry->mac[4],l2_entry->mac[5]);
    printf(" VID:%d ",l2_entry->vid);
    if((l2_entry->flags & BCM_L2_TRUNK_MEMBER) == 0)
    {
        printf(" PORT:%d \n",l2_entry->port);
    }
    else
    {
        printf(" TRUNK:%d \n",l2_entry->tgid);
    }
    return BCM_E_NONE;
}

bcm_error_t
show_l2_entries(int unit)
{
    bcm_l2_traverse(unit,l2_show_traverse,NULL);
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
    bshell(unit, "clear c");
    printf("\nSending unicast packet to ingress port:%d\n",ingress_port_1);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000011000000000002080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port_1);
    bshell(unit, str);
    print "show c";
    bshell(unit, "show c");

    printf("\nverify l2 learning:\n");

    /*l2 traverse routine*/
    show_l2_entries(unit);

    bshell(unit, "clear c");
    printf("\nSending unicast packet to ingress port:%d\n",ingress_port_2);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000000011000000000033080600010800060400010000000000020A0A0A0200000000000014141402000102030405060708090a0b0c0dbc94977f; sleep quiet 1", ingress_port_2);
    bshell(unit, str);
    print "show c";
    bshell(unit, "show c");

    printf("\nverify l2 learning:\n");

    /*l2 traverse routine*/
    show_l2_entries(unit);

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
add_l2_static_entry(int unit,uint32 flag,bcm_mac_t mac,bcm_vpn_t vpn_id,bcm_port_t port)
{
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC | flag;
    l2_addr.port = port;
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

    return BCM_E_NONE;
}

bcm_error_t
create_trunk(int unit, bcm_trunk_t *tid, int count, bcm_port_t *member_port)
{
  bcm_error_t rv;
  int i;
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
  printf("Trunk Create, tid=%d, rv=%s \n", *tid, bcm_errmsg(rv));
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_trunk_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_trunk_set(unit, *tid, &trunk_info, count, member_info);
  printf("Trunk Set %s\n", bcm_errmsg(rv));
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_trunk_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  return BCM_E_NONE;
}

bcm_error_t
config_l2_trunk_rtag7(int unit,int count,bcm_port_t* trunk_ports, bcm_trunk_t trunk_id)
{

  bcm_error_t rv;

  rv = create_trunk(unit, &trunk_id, count,trunk_ports);
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
l2_addr_callback(int unit, bcm_l2_addr_t *l2_entry, int operation, void *userdata)
{
   char *oper[6] = {"BCM_L2_CALLBACK_DELETE",
                    "BCM_L2_CALLBACK_ADD",
                    "BCM_L2_CALLBACK_REPORT",
                    "BCM_L2_CALLBACK_LEARN_EVENT",
                    "BCM_L2_CALLBACK_AGE_EVENT",
                    "BCM_L2_CALLBACK_MOVE_EVENT"};
    printf("\nL2_EVENT! MAC=%02x:%02x:%02x:%02x:%02x:%02x ",l2_entry->mac[0],
                l2_entry->mac[1],l2_entry->mac[2],l2_entry->mac[3],
                                l2_entry->mac[4],l2_entry->mac[5]);
    printf(" VID=%d ",l2_entry->vid);
    if((l2_entry->flags & BCM_L2_TRUNK_MEMBER) == 0)
    {
        printf(" PORT=%d ",l2_entry->port);
    }
    else
    {
        printf(" TRUNK=%d ",l2_entry->tgid);
    }

    printf(" Operation=%s\n",oper[operation]);
}

/* L2 switch configuration function */
bcm_error_t
setup_l2_forwarding(int unit)
{
    bcm_vlan_t vlanId=20, default_vlan=20;
    bcm_error_t rv = BCM_E_NONE;
    bcm_mac_t dst_mac = "00:00:00:00:00:11";

    /*Create the vlan with specific id*/
    printf("create vlan %d\n", vlanId);
    rv = bcm_vlan_create(unit, vlanId);
    if (BCM_FAILURE(rv)) {
        printf("Error in bcm_vlan_create: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*Add ports to vlan*/
    rv = vlan_add_port(unit, vlanId, ingress_port_1, ingress_port_1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vlan_add_port(unit, vlanId, ingress_port_2, ingress_port_2);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vlan_add_port(unit, vlanId, egress_port_1, egress_port_1);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vlan_add_port(unit, vlanId, egress_port_2, egress_port_2);
    if (BCM_FAILURE(rv)) {
        printf("\nError in vlan_add_port: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*Setup default VFI at ingress port for untagged packets*/
    rv = bcm_port_untagged_vlan_set(unit, ingress_port_1, default_vlan);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_untagged_vlan_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*Setup default VFI at ingress port for untagged packets*/
    rv = bcm_port_untagged_vlan_set(unit, ingress_port_2, default_vlan);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_untagged_vlan_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*Learn mode port config*/
    rv = port_learn_mode_set(unit, ingress_port_1, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*Learn mode port config*/
    rv = port_learn_mode_set(unit, ingress_port_2, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
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

    rv = port_learn_mode_set(unit, egress_port_2, 0);
    if (BCM_FAILURE(rv)) {
        printf("\nError in port_learn_mode_set: %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_port_t trunk_ports[2];
    trunk_ports[0] = ingress_port_1;
    trunk_ports[1] = ingress_port_2;

    printf("Ingress trunk ports configuration\n");
    rv = config_l2_trunk_rtag7(unit,2,trunk_ports,ing_tid);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_l2_trunk_rtag7: %s\n", bcm_errmsg(rv));
        return rv;
    }

    trunk_ports[0] = egress_port_1;
    trunk_ports[1] = egress_port_2;

    printf("Egress trunk ports configuration\n");
    rv = config_l2_trunk_rtag7(unit,2,trunk_ports,egr_tid);
    if (BCM_FAILURE(rv)) {
        printf("\nError in config_l2_trunk_rtag7: %s\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_GPORT_TRUNK_SET(trunk_gport, egr_tid);

    /*Configure static l2 entry for forwarding*/
    rv = add_l2_static_entry(unit,0,dst_mac,vlanId,trunk_gport);
    if (BCM_FAILURE(rv)) {
        printf("\nError in add_l2_static_entry: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*register callback function for l2 addr events*/
    printf("setup callback function for l2 event\n");
    rv = bcm_l2_addr_register(unit, l2_addr_callback, NULL);
    if (BCM_FAILURE(rv)) {
        printf("\nError in bcm_l2_addr_register: %s\n", bcm_errmsg(rv));
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
