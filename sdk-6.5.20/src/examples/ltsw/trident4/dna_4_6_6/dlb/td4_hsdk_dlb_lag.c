/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : DLB configuration for LAG/Trunk
 *
 *  config   : td4_hsdk_dlb.config.yml
 *             (Usage: rack#./bcm.user -y ./td4_hsdk_dlb.config.yml)
 *
 *  Usage    : BCM.0> cint td4_hsdk_dlb_lag.c
 *
 *  Log file : td4_hsdk_dlb_lag_log.txt
 *
 *  Test Topology :
 *      +-----------------+            +---------+
 *      |         ing-port|<-----------+         |
 *      |                 |            |         |
 *      |    trunk-DLB-p1 +------------>         |
 *      |                 |            |         |
 *      |                 |            |Traffic  |
 *      |    trunk-DLB-p2 +------------>Generator|
 *      |                 |            |         |
 *      |    trunk-DLB-p3 +------------>         |
 *      |                 |            |         |
 *      |    trunk-DLB-p4 +------------>         |
 *      |                 |            |         |
 *      |                 |            |         |
 *      |                 |            |         |
 *      +-----------------+            +---------+
 *
 * Summary:
 * ========
 *   This Cint example to show configuration of the DLB for LAG application
 *   along with FlexDigest hashing using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects five ports and configure them in Loopback mode. Out of these
 *        five ports, one port is used as ingress_port and the rest all as
 *        egress_ports.
 *
 *
 *   2) Step2 - Configuration (Done in config_dlb_lag()):
 *   =========================================================
 *     a) Configure a basic port, vlan, flexdigest and trunk/LAG with DLB
 *        attributes functional scenario and does the necessary configurations.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l2 show' and 'trunk show'
 *
 *     b) Transmit the few/many UC packets from ingress-port. The contents of
 *        the packet are printed on screen.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that the packets/traffic is splits in a way that the
 *       traffic load is balanced across multiple egress-ports ( i.e Trunk-DLB ports)
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
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelTrunkNonUnicast, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, port, bcmSwitchTrunkUnicastHashOffset, 64));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, port, bcmSwitchTrunkNonUnicastHashOffset, 64));
    /* DLB hashout selection */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelTrunkDynamic, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_port_set(unit, port, bcmSwitchTrunkDynamicHashOffset, 64));
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
        (bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL2HdrOtag));
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

int
configure_trunk_dlb_setup(int unit, int port_count, bcm_port_t *port_arr, bcm_vlan_t vid)
{
    int i;
    int tid = 0;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t member_array[8];
    int dlb_valid = 0;
    
    /*** DLB ECMP Global Parameters configuration ****/
    printf("DLB Global parameters:\n");
    /* DLB ECMP Sampling Rate */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchEcmpDynamicSampleRate, 62500));  /* 16 us*/
    /* Create Match ID */
    /* DLB ECMP Quantization parameters */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesExponent, 3));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesDecreaseReset, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesExponent, 3));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesDecreaseReset, 0));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesExponent, 3));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesDecreaseReset, 0));

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesMinThreshold, 1000));   /* 10% of 10Gbs, unit Mbs */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicEgressBytesMaxThreshold, 10000));  /* 10 Gbs, unit Mbs */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesMinThreshold, 0x5D44)); /* in bytes, 0x5E cells x 254 Bytes */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicQueuedBytesMaxThreshold, 0xFE00)); /* 254 x 0x100 cells */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesMinThreshold, 0x2EA2));/* 254 x 0x2F cells */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit,bcmSwitchEcmpDynamicPhysicalQueuedBytesMaxThreshold, 0x7F00)); /* 254 x 0x80 cells */

    /* Set the DLB ECMP random seed */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchEcmpDynamicRandomSeed, 0x5555));

    print "bcm_trunk_create";
    BCM_IF_ERROR_RETURN(bcm_trunk_create(unit,BCM_TRUNK_FLAG_WITH_ID,&tid));
    bcm_trunk_info_t_init(&trunk_info);
    trunk_info.flags = BCM_TRUNK_FLAG_FAILOVER_ALL_LOCAL;
    trunk_info.psc = BCM_TRUNK_PSC_DYNAMIC;
//    trunk_info.psc = BCM_TRUNK_PSC_DYNAMIC_ASSIGNED;
//    trunk_info.psc = BCM_TRUNK_PSC_DYNAMIC_OPTIMAL;
    trunk_info.dlf_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;

    if (trunk_info.psc == BCM_TRUNK_PSC_DYNAMIC ||
        trunk_info.psc == BCM_TRUNK_PSC_DYNAMIC_ASSIGNED ||
        trunk_info.psc == BCM_TRUNK_PSC_DYNAMIC_OPTIMAL) {
        dlb_valid = 1;
    }
    if (dlb_valid) {
        trunk_info.dynamic_size = 512;
        trunk_info.dynamic_age = 32;
    }
    
    for (i = 0; i < port_count; i++) {
        bcm_trunk_member_t_init(&member_array[i]);
        BCM_GPORT_MODPORT_SET(member_array[i].gport, 0, port_arr[i]);
        if (dlb_valid) {
            member_array[i].dynamic_scaling_factor = 400;
            member_array[i].dynamic_load_weight = 50;
            member_array[i].dynamic_queue_size_weight = 40;
        }
    }
   
    print "bcm_trunk_set"; 
    BCM_IF_ERROR_RETURN(bcm_trunk_set(unit,tid,&trunk_info,port_count,member_array));
    
    printf("DLB Trunk Members status set:\n");
    for(i=0; i < port_count; i++) {
        BCM_IF_ERROR_RETURN(bcm_trunk_member_status_set(unit, port_arr[i], BCM_TRUNK_DYNAMIC_MEMBER_HW));
    }

    for (i = 0; i < port_count; i++) {
        BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, port_arr[i], 0));
    }

    uint8 mac[6] = {0x00, 0x00, 0x00, 0xdc, 0xdd, 0xde};
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac, vid);
    l2addr.port = tid;
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.flags |= BCM_L2_TRUNK_MEMBER;
    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

    return BCM_E_NONE;
}


bcm_error_t
config_dlb_lag(int unit)
{
    BCM_IF_ERROR_RETURN(configure_port_vlan(unit, vid, port_count + 1, port_arr));
    BCM_IF_ERROR_RETURN(configure_FlexDigest(unit, L2IifOpaqueCtrlId));
    BCM_IF_ERROR_RETURN(configure_HashOutSelection(unit, ingress_port));
    BCM_IF_ERROR_RETURN(configure_trunk_dlb_setup(unit, port_count, port_arr, vid));

    return BCM_E_NONE;
}


/*
 * This function is pre-test setup.
 */
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
    int pkt_cnt = 20;
    char   str[512];
    uint64 in_pkt, out_pkt;

    printf("Verify %d UC pkts from port %d forwarding to tid 0\n", pkt_cnt, ingress_port);
    for (i = 0; i < pkt_cnt; i++) {
        snprintf(str, 512, "tx 1 pbm=%d sm=00:00:00:cc:cc:%02d dm=00:00:00:dc:dd:de vlan=11;sleep 1;", ingress_port, i);
        printf("%s\n", str);
        bshell(unit, str);
    }

    printf("Executing 'l2 show'\n");
    bshell(unit, "l2 show");

    printf("Executing 'trunk show'\n");
    bshell(unit, "trunk show");

    printf("Executing 'show c'\n");
    bshell(unit, "show c");

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_dlb_lag())
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

    print "~~~ #2) config_dlb_lag(): ** start **";
    if (BCM_FAILURE((rv = config_dlb_lag(unit)))) {
        printf("config_dlb_lag() failed.\n");
        return -1;
    }
    print "~~~ #2) config_dlb_lag(): ** end **";

    print "~~~ #3) verify(): ** start **";
    verify(unit);
    print "~~~ #3) verify(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}
