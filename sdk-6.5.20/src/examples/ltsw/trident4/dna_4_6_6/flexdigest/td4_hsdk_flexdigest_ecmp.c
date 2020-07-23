/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : FlexDigest for ECMP
 *
 *  config   : td4_hsdk_flexdigest.config.yml
 *             (Usage: rack#./bcm.user -y ./td4_hsdk_flexdigest.config.yml)
 *
 *  Usage    : BCM.0> cint td4_hsdk_flexdigest_ecmp.c
 *
 *  Log file : td4_hsdk_flexdigest_ecmp_log.txt
 *
 *  Test Topology :
 *      +-----------------+            +---------+
 *      |         ing-port|<-----------+         |
 *      |                 |            |         |
 *      |         ecmp-p1 +------------>         |
 *      |                 |            |         |
 *      |                 |            |Traffic  |
 *      |     SVK ecmp-p2 +------------>Generator|
 *      |                 |            |         |
 *      |         ecmp-p3 +------------>         |
 *      |                 |            |         |
 *      |         ecmp-p4 +------------>         |
 *      |                 |            |         |
 *      |                 |            |         |
 *      |                 |            |         |
 *      +-----------------+            +---------+
 *
 * Summary:
 * ========
 *   This Cint example to show configuration of the FlexDigest (new Hashing scheme)
 *   for ECMP application using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects five ports and configure them in Loopback mode. Out of these
 *        five ports, one port is used as ingress_port and the rest all as
 *        egress_ports.
 *
 *   2) Step2 - Configuration (Done in config_flexdigest_ecmp()):
 *   =========================================================
 *     a) Configure a basic port, vlan, flexdigest and ECMP functional
 *        scenario and does the necessary configurations.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 intf show', 'l3 egress show'
 *        and 'trunk show'.
 *
 *     b) Transmit the few/many UC packets from ingress-port. The contents of the packet
 *        are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that the packets/traffic is splits in a way that the
 *       traffic load is balanced across multiple egress-ports ( i.e ECMP member-ports)
 *       Also see 'show c' to check the Tx/Rx packet stats/counters.
 */

cint_reset();

bcm_port_t ingress_port;
int i;
int port_count = 6;
int port_count_max = 7;
bcm_port_t port_arr[8] = {0};
bcm_vlan_t vid = 11;
bcm_vlan_t ingress_vlan;
bcm_vrf_t vrf = 2;
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
    /* ECMP hashout selection */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmpOverlay, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, port, bcmSwitchECMPOverlayHashOffset, 48));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, port, bcmSwitchECMPUnderlayHashSet0Offset, 48));

    return BCM_E_NONE;
}

bcm_error_t
configure_port_vlan(int unit, uint16 vid, int port_count, bcm_port_t *port_arr)
{
    int rv = 0;
    int i;
    bcm_pbmp_t pbmp, ubmp;

    for (i = 0; i < port_count; i++) {
        /* Port VLAN configuration */
        BCM_PBMP_CLEAR(ubmp);
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, port_arr[i]);
        BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid + i));
        BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid + i, pbmp, ubmp));
        BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port_arr[i], vid + i));
    }

    return rv;
}

bcm_error_t
configure_FlexDigest(int unit, uint8 L2IifOpaqueCtrlId)
{
    int seed_profile_id, norm_profile_id, hash_profile_id;
    int value;
    int debug = 0;
    uint16 salt_a[16] = {
        0xabcd,
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
        0xeeee
    };
    bcm_flexdigest_qset_t qset;
    bcm_flexdigest_group_t group;
    bcm_flexdigest_entry_t entry;
    bcm_flexdigest_match_id_t match_id;
    int pri = 0;

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_init(unit));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_seed_profile_create(unit, 0, &seed_profile_id));
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
    if (debug) {
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_NORM_PROFILE_SEED traverse -l'");
    }

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_profile_create(unit, 0, &norm_profile_id));

    value = 1;
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_profile_set(unit, norm_profile_id,
                                         bcmFlexDigestNormProfileControlNorm, value));
    value = 0;
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_norm_profile_set(unit, norm_profile_id,
                                         bcmFlexDigestNormProfileControlSeedId, value));

    if (debug) {
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_NORM_PROFILE traverse -l'");
    }

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_profile_create(unit, 0, &hash_profile_id));

    if (debug) {
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_HASH_PROFILE traverse -l'");
    }
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
    if (debug) {
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_HASH_PROFILE traverse -l'");
    }


    if (debug) {
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_HASH_SALT traverse -l'");
    }

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_salt_set(unit, bcmFlexDigestHashBinA, 16, salt_a));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_hash_salt_set(unit, bcmFlexDigestHashBinB, 16, salt_a));

    if (debug) {
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_HASH_SALT traverse -l'");
    }


    if (debug) {
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_LKUP_PRESEL traverse -l'");
    }
    BCM_FLEXDIGEST_QSET_INIT(qset);
    BCM_FLEXDIGEST_QSET_ADD(qset, bcmFlexDigestQualifyMatchId);
    BCM_FLEXDIGEST_QSET_ADD(qset, bcmFlexDigestQualifyL4Valid);
    BCM_FLEXDIGEST_QSET_ADD(qset, bcmFlexDigestQualifyMyStation1Hit);
    BCM_FLEXDIGEST_QSET_ADD(qset, bcmFlexDigestQualifyL2IifOpaqueCtrlId);
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_group_create(unit, qset, pri, &group));
    if (debug) {
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_LKUP_PRESEL traverse -l'");
    }

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_entry_priority_set(unit, entry, 2));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_match_id_create(unit, &match_id));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL2HdrL2));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_qualify_MatchId(unit, entry, match_id));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_qualify_L4Valid(unit, entry, 1, 0x1));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_qualify_MyStation1Hit(unit, entry, 1, 0x1));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_qualify_L2IifOpaqueCtrlId(unit, entry, L2IifOpaqueCtrlId, 0xf));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds0,  BCM_FLEXDIGEST_FIELD_OUTER_MACSA_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds1,  BCM_FLEXDIGEST_FIELD_OUTER_MACSA_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds2,  BCM_FLEXDIGEST_FIELD_OUTER_MACSA_32_47, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds3,  BCM_FLEXDIGEST_FIELD_OUTER_MACDA_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds4,  BCM_FLEXDIGEST_FIELD_OUTER_MACDA_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds5,  BCM_FLEXDIGEST_FIELD_OUTER_MACDA_32_47, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds6,  BCM_FLEXDIGEST_FIELD_OUTER_OTAG_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds7,  BCM_FLEXDIGEST_FIELD_OUTER_OTAG_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds8,  BCM_FLEXDIGEST_FIELD_OUTER_ETHERTYPE, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds9,  BCM_FLEXDIGEST_FIELD_INGRESS_PP_PORT, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds10, BCM_FLEXDIGEST_FIELD_L2_IIF, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds11, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_A_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds12, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_A_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds13, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds14, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds15, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_C_0_15, 0xffff));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds0,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds1,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds2,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_32_47, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds3,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_48_63, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds4,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_64_79, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds5,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_80_95, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds6,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_96_111, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds7,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_112_127, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds8,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds9,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds10, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_32_47, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds11, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_48_63, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds12, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_64_79, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds13, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_80_95, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds14, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_96_111, 0xffff));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds15, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_112_127, 0xffff));

    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds0, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds1, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds2, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds3, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds4, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds5, 1, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds6, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds7, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds8, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds9, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds10, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds11, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds12, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds13, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds14, 0, 0x1));
    BCM_IF_ERROR_RETURN(bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetCCmds15, 0, 0x1));

    if (debug) {
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_LKUP_MASK_PROFILE traverse -l'");
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_LKUP traverse -l'");
    }

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_entry_install(unit, entry));

    if (debug) {
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_LKUP_MASK_PROFILE traverse -l'");
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_LKUP traverse -l'");
    }

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_qualifier_delete(unit, entry, bcmFlexDigestQualifyL4Valid));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_qualifier_delete(unit, entry, bcmFlexDigestQualifyMyStation1Hit));
    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_action_delete(unit, entry, bcmFlexDigestActionExtractBinSetACmds15));

    BCM_IF_ERROR_RETURN
        (bcm_flexdigest_entry_reinstall(unit, entry));

    if (debug) {
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_LKUP_MASK_PROFILE traverse -l'");
        bshell(unit, "bsh -c 'lt FLEX_DIGEST_LKUP traverse -l'");
    }

    return BCM_E_NONE;
}

bcm_error_t
configure_l3_ing_obj(int unit, bcm_port_t ingress_port, bcm_vlan_t ingress_vlan, bcm_vrf_t vrf)
{
    bcm_mac_t router_mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

    /* Create L3 interface */
    bcm_l3_intf_t l3_intf_in;
    bcm_l3_intf_t_init(&l3_intf_in);
    sal_memcpy(l3_intf_in.l3a_mac_addr, router_mac_in, sizeof(router_mac_in));
    l3_intf_in.l3a_vid = ingress_vlan;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf_in));

    /* Create L3 ingress interface */
    bcm_l3_ingress_t l3_ingress;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.flags = BCM_L3_INGRESS_WITH_ID;
    bcm_if_t ingress_if = 0x1b2;
    l3_ingress.vrf = vrf;
    BCM_IF_ERROR_RETURN(bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if));

    /* Config vlan_id to l3_iif mapping */
    bcm_vlan_control_vlan_t vlan_ctrl;
    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, ingress_vlan, &vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, ingress_vlan, vlan_ctrl));

    /* Config my station */
    bcm_l2_station_t l2_station;
    int station_id;
    bcm_l2_station_t_init(&l2_station);
    sal_memcpy(l2_station.dst_mac, router_mac_in, sizeof(router_mac_in));
    l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
    BCM_IF_ERROR_RETURN(bcm_l2_station_add(unit, &station_id, &l2_station));

    return BCM_E_NONE;
}

bcm_error_t
configure_ECMP_setup(int unit, int port_count, bcm_port_t *port_arr, bcm_vlan_t vid, bcm_vrf_t vrf)
{
    int i;
    uint8 router_mac_out[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    uint8 dst_mac[6] = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x02};
    int ecmp_member_count = port_count/2;
    int ecmp_member_count_max = 8;
    bcm_l3_ecmp_member_t ecmp_member_array[8];
    bcm_l3_egress_ecmp_t ecmp_grp;
    int ecmp_id_1, ecmp_id_2;
    bcm_ip_t dip = 0x0a3a4273;
    bcm_ip_t mask = 0xfffff000;

    if (ecmp_member_count >= ecmp_member_count_max) {
        printf("ERROR: ecmp_member_count_max(%d) is not more than ecmp_member_count(%d).\n",
               ecmp_member_count_max, ecmp_member_count);
    }

    for (i = 0; i < port_count; i++) {
        /* Create L3 interface */
        bcm_l3_intf_t l3_intf_out;
        bcm_l3_intf_t_init(&l3_intf_out);
        sal_memcpy(l3_intf_out.l3a_mac_addr, router_mac_out, sizeof(router_mac_out));
        l3_intf_out.l3a_vid = vid + i;
        BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf_out));

        /* Create L3 egress object */
        bcm_l3_egress_t l3_egress;
        bcm_if_t egr_obj_id;
        bcm_l3_egress_t_init(&l3_egress);
        sal_memcpy(l3_egress.mac_addr, dst_mac, sizeof(dst_mac));
        l3_egress.port = port_arr[i];
        l3_egress.intf = l3_intf_out.l3a_intf_id;
        BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_id));
        print egr_obj_id;

        bcm_l3_ecmp_member_t_init(&ecmp_member_array[i]);
        ecmp_member_array[i].egress_if = egr_obj_id;


        /* Added a L2 address to trap loopbacked packet on port_out to CPU */
        bcm_l2_addr_t addr;
        bcm_l2_addr_t_init(&addr, dst_mac, vid + i);
        addr.port = 0;
        addr.flags |= BCM_L2_STATIC;
        BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));

        router_mac_out[5]++;
        dst_mac[5]++;
    }

    /* Create ECMP DLB Object */
    bcm_l3_egress_ecmp_t_init(&ecmp_grp);
    ecmp_grp.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    BCM_IF_ERROR_RETURN(bcm_l3_ecmp_create(unit, 0, &ecmp_grp, ecmp_member_count, ecmp_member_array));
    ecmp_id_1 = ecmp_grp.ecmp_intf;
    print ecmp_id_1;

    /* Create ECMP DLB Object */
    bcm_l3_egress_ecmp_t_init(&ecmp_grp);
    ecmp_grp.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
    BCM_IF_ERROR_RETURN(bcm_l3_ecmp_create(unit, 0, &ecmp_grp, port_count - ecmp_member_count, &ecmp_member_array[ecmp_member_count]));
    ecmp_id_2 = ecmp_grp.ecmp_intf;
    print ecmp_id_2;

    /* Create ECMP Object */
    bcm_if_t ecmp_obj_id;
    bcm_l3_ecmp_member_t_init(&ecmp_member_array[0]);
    ecmp_member_array[0].egress_if = ecmp_id_1;
    bcm_l3_ecmp_member_t_init(&ecmp_member_array[1]);
    ecmp_member_array[1].egress_if = ecmp_id_2;
    bcm_l3_egress_ecmp_t_init(&ecmp_grp);
    int max_paths = 2;
    ecmp_grp.max_paths = max_paths;
    BCM_IF_ERROR_RETURN(bcm_l3_ecmp_create(unit, 0, &ecmp_grp, max_paths, ecmp_member_array));
    ecmp_obj_id = ecmp_grp.ecmp_intf;
    print ecmp_obj_id;

    bcm_l3_route_t route_info;
    /* Install the default route for DIP */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_flags = BCM_L3_MULTIPATH;
    route_info.l3a_intf = ecmp_obj_id;
    route_info.l3a_subnet = 0;
    route_info.l3a_ip_mask = 0;
    route_info.l3a_vrf = vrf;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route_info));

    /* Install the route for DIP */
    bcm_l3_route_t_init(&route_info);
    route_info.l3a_flags = BCM_L3_MULTIPATH;
    route_info.l3a_intf = ecmp_obj_id;
    route_info.l3a_subnet = dip;
    route_info.l3a_ip_mask = mask;
    route_info.l3a_vrf = vrf;
    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route_info));

    return BCM_E_NONE;
}

bcm_error_t
config_flexdigest_ecmp(int unit)
{
    ingress_vlan = vid + port_count;
    BCM_IF_ERROR_RETURN(configure_port_vlan(unit, vid, port_count + 1, port_arr));
    BCM_IF_ERROR_RETURN(configure_FlexDigest(unit, L2IifOpaqueCtrlId));
    BCM_IF_ERROR_RETURN(configure_HashOutSelection(unit, ingress_port));
    BCM_IF_ERROR_RETURN(configure_l3_ing_obj(unit, ingress_port, ingress_vlan, vrf));
    BCM_IF_ERROR_RETURN(configure_ECMP_setup(unit, port_count, port_arr, vid, vrf));

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

bcm_error_t
verify(int unit)
{
    int i;
    int pkt_cnt = 20;
    char   str[512];
    uint64 in_pkt, out_pkt;
    uint32 dip = 0x0a3a4273;
    uint16 checksum = 0xbd76;

    /* Traffic */

    bshell(unit, "pw start");
    bshell(unit, "pw report +all");

    printf("Verify %d UC pkts from port %d routing to ECMP group\n", pkt_cnt, ingress_port);
    for (i = 0; i < pkt_cnt; i++) {
        snprintf(str, 512, "tx 1 pbm=%d data=0x000000000001002a1077770008004500003c67620000ff01%04x0a3a4001%08x0000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869;sleep 1;", ingress_port, checksum - i, dip + i, i);
        printf("%s\n", str);
        bshell(unit, str);
    }

    printf("Executing 'l2 show'\n");
    bshell(unit, "l2 show");

    printf("Executing 'l3 egress show'\n");
    bshell(unit, "l3 egress show");

    printf("Executing 'l3 ecmp show'\n");
    bshell(unit, "l3 ecmp show");

    printf("Executing 'show c'\n");
    bshell(unit, "show c");

    return BCM_E_NONE;
}

/*
 * This functions does the following
 * a)test setup *
 * b)demonstrates the functionality(done in verify()).
 * c)tear down the test environment.
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

    print "~~~ #2) config_flexdigest_ecmp(): ** start **";
    if (BCM_FAILURE((rv = config_flexdigest_ecmp(unit)))) {
        printf("config_flexdigest_ecmp() failed.\n");
        return -1;
    }
    print "~~~ #2) config_flexdigest_ecmp(): ** end **";

    print "~~~ #3) verify(): ** start **";
    verify(unit);
    print "~~~ #3) verify(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}
