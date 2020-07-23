/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : FlexDigest for different traffic types
 *
 *  config   : td4_hsdk_flexdigest.config.yml
 *             (Usage: rack#./bcm.user -y ./td4_hsdk_flexdigest.config.yml)
 *
 *  Usage    : BCM.0> cinttd4_hsdk_flexdigest_diff_traffic_types.c
 *
 *  Log file : td4_hsdk_flexdigest_diff_traffic_types_log.txt
 *
 *  Test Topology :
 *      +-----------------+            +---------+
 *      |         ing-port|<-----------+         |
 *      |                 |            |         |
 *      |         lag-p1  +------------>         |
 *      |                 |            |         |
 *      |                 |            |Traffic  |
 *      |     SVK lag-p2  +------------>Generator|
 *      |                 |            |         |
 *      |         lag-p3  +------------>         |
 *      |                 |            |         |
 *      |         lag-p4  +------------>         |
 *      |                 |            |         |
 *      |                 |            |         |
 *      |                 |            |         |
 *      +-----------------+            +---------+
 *
 * Summary:
 * ========
 *   This Cint example to show configuration of the FlexDigest (new Hashing scheme)
 *   for different traffic types configuration and funcationality testing using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects five ports and configure them in Loopback mode. Out of these
 *        five ports, one port is used as ingress_port and the rest all as
 *        egress_ports.
 *
 *   2) Step2 - Configuration (Done in config_flexdigest_different_traffic()):
 *   =========================================================
 *     a) Configure a basic port, vlan, flexdigest with different traffic types
 *        hash bin selections test scenario and does the necessary configurations.
 *     b) Configuring 3 different scenarios
 *        - L2 Plain with SMAC increment,
 *        - L4 UDP Plain with Src Port increment and
 *        - L3 packet with unknown protocol and SIP increment
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l2 show', and 'trunk show'.
 *
 *     b) Transmit the few/many UC packets from ingress-port. The contents of the packet
 *        are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that the packets/traffic is splits in a way that the
 *       traffic load is balanced across multiple egress-ports ( i.e LAG ports)
 *       Also see 'show c' to check the Tx/Rx packet stats/counters.
 */

cint_reset();

bcm_port_t ingress_port;
int i;
int port_count = 5;
int port_count_max = 7;
bcm_port_t port_arr[8] = {0};
bcm_vlan_t vid = 11;
uint8 L2IifOpaqueCtrlId = 1;

/*
 * This function checked if given port/index is already present
 * in the list so that same ports number is not generated again
*/
int
checkPortAssigned(int *port_index_list,int no_entries, int index)
{
  int i=0;

  for (i= 0; i < no_entries; i++) {
    if (port_index_list[i] == index)
      return 1;
  }

  /* no entry found */
  return 0;
}

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
*/
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0,rv = 0, index = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;
    int tempports[BCM_PBMP_PORT_MAX];
    int port_index_list[num_ports];

    rv = bcm_port_config_get(unit,&configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i)) {
            tempports[port] = i;
            port++;
        }
    }

    if (( port == 0 ) || ( port < num_ports )) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    /* generating random ports */
    for (i= 0; i < num_ports; i++) {
        index = sal_rand()% port;
        if (checkPortAssigned(port_index_list, i, index) == 0) {
            port_list[i] = tempports[index];
            port_index_list[i] = index;
        } else {
            i = i - 1;
        }
    }

    return BCM_E_NONE;
}

bcm_error_t
setup_port(int unit, int L2IifOpaqueCtrlId, int port_count, bcm_port_t *port_arr)
{
    int i;
    for (i = 0; i < port_count; i++) {
        BCM_IF_ERROR_RETURN(bcm_port_class_set(unit, port_arr[i], bcmPortClassOpaqueCtrlId, L2IifOpaqueCtrlId));
        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_arr[i], BCM_PORT_LOOPBACK_MAC));
    }
    return BCM_E_NONE;
}

bcm_error_t
configure_HashOutSelection(int unit, bcm_port_t port)
{
    /* Trunk UC and Non-UC hashout selection */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelTrunkUc, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, port, bcmSwitchTrunkUnicastHashOffset, 1));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelTrunkNonUnicast, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, port, bcmSwitchTrunkNonUnicastHashOffset, 65));
    return BCM_E_NONE;
}

bcm_error_t
configure_port_vlan(int unit, uint16 vid, int port_count, bcm_port_t *port_arr)
{
    int rv = 0;
    int i;
    bcm_pbmp_t pbmp, ubmp;

    /* Port VLAN configuration */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    for (i = 0; i < port_count; i++) {
        BCM_PBMP_PORT_ADD(pbmp, port_arr[i]);
    }
    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, ubmp));

    return rv;
}

bcm_error_t
configure_FlexDigest(int unit)
{
    printf("Flex Digest Configuration:\n");
    int seed_profile_id, norm_profile_id, hash_profile_id;
    int norm_enable;
    int seed;
    int debug = 0;

    print bcm_flexdigest_init(unit);

    printf("Flex Digest Normalization setup:\n");
    seed_profile_id = 1;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_norm_seed_profile_create(unit, BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &seed_profile_id));

    seed = 0xbabaface;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id, bcmFlexDigestNormSeedControlBinASeed, seed));
    seed = 0xa5a5a5a5;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id, bcmFlexDigestNormSeedControlBinBSeed, seed));
    seed = 0x5a5a5a5a;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id, bcmFlexDigestNormSeedControlBinCSeed, seed));


    norm_profile_id = 0;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_norm_profile_create(unit, BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &norm_profile_id));

    norm_enable = TRUE;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_norm_profile_set(unit, norm_profile_id, bcmFlexDigestNormProfileControlNorm, norm_enable));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_norm_profile_set(unit, norm_profile_id, bcmFlexDigestNormProfileControlSeedId, seed_profile_id));

    /**** LOOKUP SETUP ****/
    printf("Flex Digest Lookup setup:\n");
    bcm_flexdigest_qset_t qset;

    BCM_FLEXDIGEST_QSET_INIT(qset);
    BCM_FLEXDIGEST_QSET_ADD(qset, bcmFlexDigestQualifyMatchId);

    bcm_flexdigest_group_t group;
    bcm_flexdigest_entry_t entry;
    bcm_flexdigest_match_id_t match_id;
    int pri = 0, mask_1, mask_2;

    mask_1 = 0xFFFF; /*General Mask for all full fields */
    mask_2 = 0x0FFF; /*For VLAN ID from VLAN tag */

    int use_a_bin = 1; /*C_BIN[x] = A_BIN[x]*/
    int not_use_a_bin = 0; /*C_BIN[x] = 16 bits of all zeros */
    int c_mask = 1;

    /*** GROUP 0 (Pri = 0) for L2 packets ***/
    printf("Flex Digest Lookup Group 0:\n");
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
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds1, BCM_FLEXDIGEST_FIELD_OUTER_OTAG_0_15, mask_2));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds2, BCM_FLEXDIGEST_FIELD_OUTER_ETHERTYPE, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds3, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_0_15, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds4, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_16_31, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds5, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_32_47, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds6, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_0_15, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds7, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_16_31, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds8, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_32_47, mask_1));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds14, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_A_0_15, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds15, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_C_0_15, mask_1));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds0, use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds1, use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds2, use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds3, use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds4, use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds5, use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds6, not_use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds7, not_use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds8, not_use_a_bin, c_mask));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds14, not_use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds15, use_a_bin, c_mask));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_entry_install(unit, entry));

    /*** GROUP 1 (Pri = 1) for IPV4 and IPv4 UDP packets ***/
    printf("Flex Digest Lookup Group 1 Entry 0:\n");
    pri = 1;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_group_create(unit, qset, pri, &group));

    /** Entry 0 : IPv4 UDP **/
    BCM_IF_ERROR_RETURN(bcm_flexdigest_entry_create(unit, group, &entry));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_match_id_create(unit, &match_id));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL3L4HdrUdp));

    /* Qualifiers */
    BCM_IF_ERROR_RETURN(bcm_flexdigest_qualify_MatchId(unit, entry, match_id));

    /* Actions : Bin field population */
    /* Set A */
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds3, BCM_FLEXDIGEST_FIELD_OUTER_L4_DST_PORT, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds4, BCM_FLEXDIGEST_FIELD_OUTER_L4_SRC_PORT, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds5, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds6, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds7, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds8, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31, mask_1));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds3, use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds4, not_use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds5, not_use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds6, not_use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds7, use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds8, use_a_bin, c_mask));


    BCM_IF_ERROR_RETURN(bcm_flexdigest_entry_install(unit, entry));

    /** Entry 1 : IPv4 Unknown Protocol Type **/
    printf("Flex Digest Lookup Group 1 Entry 1:\n");
    BCM_IF_ERROR_RETURN(bcm_flexdigest_entry_create(unit, group, &entry));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_match_id_create(unit, &match_id));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL3L4HdrUnknownL4));

    /* Qualifiers */
    BCM_IF_ERROR_RETURN(bcm_flexdigest_qualify_MatchId(unit, entry, match_id));

    /* Actions : Bin field population */
    /* Set A */
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds3, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_PROTOCOL, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds4, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_TTL, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds5, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds6, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds7, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15, mask_1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds8, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31, mask_1));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds3, use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds4, use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds5, not_use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds6, not_use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds7, use_a_bin, c_mask));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds8, use_a_bin, c_mask));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_entry_install(unit, entry));

    /*** HASH SETUP ***/
    printf("Flex Digest Hash setup:\n");
    hash_profile_id = 0;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_create(unit,
            BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &hash_profile_id));

    uint16 salt[16] = {
        0xabcd, 0xbbcb, 0xbabe, 0x0123, 0x7654, 0xabcd, 0xaaaa, 0x5555,
        0xffff, 0x0000, 0xa5a5, 0xface, 0xbbbb, 0xcccc, 0xdddd, 0xeeee
    };

    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_salt_set(unit, bcmFlexDigestHashBinA, sizeof(salt)/sizeof(salt[0]), salt));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_salt_set(unit, bcmFlexDigestHashBinC, sizeof(salt)/sizeof(salt[0]), salt));

    int enable = 1;
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlPreProcessBinA, enable));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlPreProcessBinC, enable));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlXorSaltBinA, enable));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlXorSaltBinC, enable));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlBinA0FunctionSelection, BCM_FLEXDIGEST_HASH_FUNCTION_CRC32HI));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlBinA1FunctionSelection, BCM_FLEXDIGEST_HASH_FUNCTION_CRC32LO));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlBinC0FunctionSelection, BCM_FLEXDIGEST_HASH_FUNCTION_CRC32HI));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlBinC1FunctionSelection, BCM_FLEXDIGEST_HASH_FUNCTION_CRC32LO));

    return BCM_E_NONE;
}

int
configure_trunk_setup(int unit, int port_count, bcm_port_t *port_arr, bcm_vlan_t vid)
{
    int i;
    int tid = 0;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t member_array[8];

    BCM_IF_ERROR_RETURN(bcm_trunk_create(unit,BCM_TRUNK_FLAG_WITH_ID,&tid));
    bcm_trunk_info_t_init(&trunk_info);
    trunk_info.flags = BCM_TRUNK_FLAG_FAILOVER_ALL_LOCAL;
    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;
    trunk_info.dlf_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;

    for (i = 0; i < port_count; i++) {
        bcm_trunk_member_t_init(&member_array[i]);
        BCM_GPORT_MODPORT_SET(member_array[i].gport, 0, port_arr[i]);
        }

    BCM_IF_ERROR_RETURN(bcm_trunk_set(unit,tid,&trunk_info,port_count,member_array));

    for (i = 0; i < port_count; i++) {
        BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, port_arr[i], 0));
    }

    uint8 mac[6] = {0x00, 0x00, 0x00, 0xbb, 0xbb, 0xbb};
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.port = tid;
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.flags |= BCM_L2_TRUNK_MEMBER;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

    return BCM_E_NONE;
}


bcm_error_t
config_flexdigest_different_traffic(int unit)
{
    BCM_IF_ERROR_RETURN(configure_port_vlan(unit, vid, port_count + 1, port_arr));
    BCM_IF_ERROR_RETURN(configure_FlexDigest(unit));
    BCM_IF_ERROR_RETURN(configure_HashOutSelection(unit, ingress_port));
    BCM_IF_ERROR_RETURN(configure_trunk_setup(unit, port_count, port_arr, vid));
    return BCM_E_NONE;
}


bcm_error_t
test_setup(int unit)
{
    if (port_count > port_count_max) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(portNumbersGet(unit, port_arr, port_count + 1));
    ingress_port = port_arr[port_count];
    BCM_IF_ERROR_RETURN(setup_port(unit, L2IifOpaqueCtrlId, port_count + 1, port_arr));

    return BCM_E_NONE;
}

void
verify(int unit)
{
    int i;
    int pkt_cnt = 4;
    char   str[512];
    uint64 in_pkt, out_pkt;

    print "Scenario-1 - Plain L2 packets with incrementing SMAC";

    bshell(unit, "sleep 1");
    for (i = 0; i < pkt_cnt; i++) {
        snprintf(str, 512, "tx 1 pbm=%d sm=00:00:00:cc:cc:%02d dm=00:00:00:bb:bb:bb vlan=11;sleep 1;", ingress_port, i);
        printf("%s\n", str);
        bshell(unit, str);
    }

    printf("Executing 'l2 show'\n");
    bshell(unit, "l2 show");

    printf("Executing 'show c'\n");
    bshell(unit, "sleep 2");
    bshell(unit, "show c");

    bshell(unit, "sleep 2");
    bshell(unit, "clear c");

    print "Scenario-2 - IPv4 UDP packets with incrementing L4 Src Port";
    bshell(unit, "sleep 1");
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000BBBBBB000000AAAAAA8100000B08004500002E0000000040113E84141414140A0A0A0A15B30019001A6561000102030405060708090A0B0C0D0E0F1011D4E6E27E", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000BBBBBB000000AAAAAA8100000B08004500002E0000000040113E84141414140A0A0A0A15B40019001A6560000102030405060708090A0B0C0D0E0F1011249A5067", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000BBBBBB000000AAAAAA8100000B08004500002E0000000040113E84141414140A0A0A0A15B50019001A655F000102030405060708090A0B0C0D0E0F1011FAD182B4", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000BBBBBB000000AAAAAA8100000B08004500002E0000000040113E84141414140A0A0A0A15B60019001A655E000102030405060708090A0B0C0D0E0F1011F1E78D56", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);

    printf("Executing 'show c'\n");
    bshell(unit, "sleep 2");
    bshell(unit, "show c");

    print "Scenario-3 - Plain IPv4 packets(Unknown IP protocol=255) with incrementing SIP";
    bshell(unit, "sleep 1");
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000BBBBBB000000AAAAAA8100000B08004500002E0000000040FF3D96141414140A0A0A0A000102030405060708090A0B0C0D0E0F10111213141516171819BF08835D", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000BBBBBB000000AAAAAA8100000B08004500002E0000000040FF3D94141414160A0A0A0A000102030405060708090A0B0C0D0E0F10111213141516171819CCF00104", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000BBBBBB000000AAAAAA8100000B08004500002E0000000040FF3D92141414180A0A0A0A000102030405060708090A0B0C0D0E0F10111213141516171819D1BEB999", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);
    snprintf(str, 512, "tx 1 pbm=%d data=0x000000BBBBBB000000AAAAAA8100000B08004500002E0000000040FF3D91141414190A0A0A0A000102030405060708090A0B0C0D0E0F1011121314151617181919688D1C", ingress_port);
    printf("%s\n", str);
    bshell(unit, str);

    printf("Executing 'show c'\n");
    bshell(unit, "sleep 2");
    bshell(unit, "show c");

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_flexdigest_different_traffic())
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

    print "~~~ #2) config_flexdigest_different_traffic(): ** start **";
    if (BCM_FAILURE((rv = config_flexdigest_different_traffic(unit)))) {
        printf("config_flexdigest_different_traffic() failed.\n");
        return -1;
    }
    print "~~~ #2) config_flexdigest_different_traffic(): ** end **";

    print "~~~ #3) verify(): ** start **";
    verify(unit);
    print "~~~ #3) verify(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}
