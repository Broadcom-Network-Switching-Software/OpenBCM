/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPv4 Unicast Route pointing H-ECMP
 *
 * Usage    : BCM.0> cint td4_hsdk_l3_ip_hecmp_route.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : td4_hsdk_l3_ip_hecmp_route_log.txt
 *
 * Test Topology :
 *
 *                   +------------------------------+              Level-2 ECMP   +-----------------------+
 *                   |                              |               Underlay      | NHI_1 egress port_1 cd1 
 *                   |                              |           +-----------------+
 *                   |                              |  Level-1  |     232769      | NHI_2 egress port_2 cd2
 *  ingress_port cd0 |                              |    ECMP   |                 +-----------------------+
 *  +----------------+          SWITCH-TD4          +-----------+
 *                   |                              |  Overlay  |                 +-----------------------+
 *                   |                              |  200001   |     232770      | NHI_3 egress port_3 cd3
 *                   |                              |           +-----------------+
 *                   |                              |               Underlay      | NHI_4 egress port_3 cd4
 *                   |                              |              Level-2 ECMP   +-----------------------+
 *                   +------------------------------+
 *
 *
 * Summary:
 * ========
 *   This Cint example to show configuration of the IPv4 route entry pointing H-ECMP scenario
 *   using BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Select one ingress and three egress ports and configure them in
 *        Loopback mode.
 *
 *     b) Install an IFP rule to copy incoming packets to CPU and start
 *        packet watcher.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in config_ip_ecmp()):
 *   ====================================================
 *      a) Create ingress_vlan and add ingress_port as member.
 *
 *      b) Create egress_vlan_1 and add egress_port_1 as member.
 *
 *      c) Create egress_vlan_2 and add egress_port_2 as member.
 *
 *      d) Create egress_vlan_3 and add egress_port_3 as member.
 *
 *      e) Create egress_vlan_4 and add egress_port_4 as member.
 *
 *      e) Create L3 ingress interface and L3 egress interface for ingress side 
 *         and bind L3 ingress interface with ingress vlan and cretae L3 egr obj for ingress side.
 *
 *      g) Create L3 egress interface and L3 egress object for egress side_1.
 *
 *      h) Create L3 egress interface and L3 egress object for egress side_2.
 *
 *      i) Create L3 egress interface and L3 egress object for egress side_3.
 *
 *      j) Create L3 egress interface and L3 egress object for egress side_4.
 *
 *      k) Create two underlay ECMP object with above two L3 egress objects as members in each group.
 *
 *      l) Create overlay ECMP object with above two under ECMP objects as member. 
 *
 *      m) Add Route entry for DIP by pointing Overlay ECMP object.
 *
 *      n) Configure flex digest for ECMP object hashing configuration.
 *
 *      o) Create station MAC entry for L3 forwarding.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) Check the configurations by 'vlan show', 'l3 ing_intf show',
 *        'l3 intf show', 'l3 egress show', 'l3 route show', 'l3 ecmp show'
 *        and 'l3 route show v6=1'
 *
 *     b) Send different SMAC and SIP packets ingress port cd0 as below
 *
 *     b1) Transmit the known IPv4 packet on ingress port cd0
 *
 *         DMAC 0x1
 *         SMAC 0xa1
 *         VLAN 0xb
 *         DIP 10.10.10.10
 *         SIP 20.20.20.21
 *
 *         0000 0000 0001 0000 0000 00a1 8100 000b
 *         0800 4500 002e 0000 0000 40ff 3d95 1414
 *         1415 0a0a 0a0a 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         9951 bab9 
 *
 *     b2) Transmit the known IPv4 packet on ingress port cd0
 *
 *         DMAC 0x1
 *         SMAC 0xa2
 *         VLAN 0xb
 *         DIP 10.10.10.10
 *         SIP 20.20.20.101
 * 
 *         0000 0000 0001 0000 0000 00a2 8100 000b
 *         0800 4500 002e 0000 0000 40ff 3d45 1414
 *         1465 0a0a 0a0a 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         5453 1a65
 *
 *     b3) Transmit the known IPv4 packet on ingress port cd0
 *
 *         DMAC 0x1
 *         SMAC 0xa3
 *         VLAN 0xb
 *         DIP 10.10.10.10
 *         SIP 20.20.20.23
 *
 *         0000 0000 0001 0000 0000 00a3 8100 000b
 *         0800 4500 002e 0000 0000 40ff 3d93 1414
 *         1417 0a0a 0a0a 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         ca9a 1395
 *
 *     b4) Transmit the known IPv4 packet on ingress port cd0
 *
 *         DMAC 0x1
 *         SMAC 0xa4
 *         VLAN 0xb
 *         DIP 10.10.10.10
 *         SIP 20.20.20.72
 *
 *         0000 0000 0001 0000 0000 00a4 8100 000b
 *         0800 4500 002e 0000 0000 40ff 3d62 1414
 *         1448 0a0a 0a0a 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         c200 ce61
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that dmac, smac and vlan are changed as the packet is routed
 *       through the L3 forwarding logic and different H-ECMP member is selected for
 *       each egress packet as below
 *
 *     c1) After step 3.b1, verify that the below packet egress out of cd1
 *
 *         DMAC 0x22
 *         SMAC 0x02
 *         VLAN 0xc
 *         DIP 10.10.10.10
 *         SIP 20.20.20.21
 *
 *         0000 0000 0022 0000 0000 0002 8100 000c
 *         0800 4500 002e 0000 0000 3fff 3e95 1414
 *         1415 0a0a 0a0a 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         9951 bab9
 * 
 *     c2) After step 3.b2, verify that the below packet egress out of cd3
 * 
 *         DMAC 0x44
 *         SMAC 0x04
 *         VLAN 0xe
 *         DIP 10.10.10.10
 *         SIP 20.20.20.101
 * 
 *         0000 0000 0044 0000 0000 0004 8100 000e
 *         0800 4500 002e 0000 0000 3fff 3e45 1414
 *         1465 0a0a 0a0a 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         5453 1a65
 *
 *     c3) After step 3.b3, verify that the below packet egress out of cd1
 *
 *         DMAC 0x22
 *         SMAC 0x02
 *         VLAN 0xc
 *         DIP 10.10.10.10
 *         SIP 20.20.20.23
 *
 *         0000 0000 0022 0000 0000 0002 8100 000c
 *         0800 4500 002e 0000 0000 3fff 3e95 1414
 *         1417 0a0a 0a0a 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         ca9a 1395
 *
 *     c4) After step 3.b4, verify that the below packet egress out of cd3
 *
 *         DMAC 0x44
 *         SMAC 0x04
 *         VLAN 0xe
 *         DIP 10.10.10.10
 *         SIP 20.20.20.72
 *
 *         0000 0000 0044 0000 0000 0004 8100 000e
 *         0800 4500 002e 0000 0000 40ff 3d62 1414 
 *         1448 0a0a 0a0a 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         c200 ce61 
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t    ingress_port;
bcm_port_t    egress_port_1;
bcm_port_t    egress_port_2;
bcm_port_t    egress_port_3;
bcm_port_t    egress_port_4;
bcm_gport_t   ingress_gport;
bcm_gport_t   egress_gport_1;
bcm_gport_t   egress_gport_2;
bcm_gport_t   egress_gport_3;
bcm_gport_t   egress_gport_4;

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
    group_config.priority = port;
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
    group_config.priority = port;
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
    int port_list[5];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 5)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port  = port_list[0];
    egress_port_1 = port_list[1];
    egress_port_2 = port_list[2];
    egress_port_3 = port_list[3];
    egress_port_4 = port_list[4];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed for port %d\n", ingress_port);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_1)) {
        printf("egress_port_setup() failed for port %d\n", egress_port_1);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_2)) {
        printf("egress_port_setup() failed for port %d\n", egress_port_2);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_3)) {
        printf("egress_port_setup() failed for port %d\n", egress_port_3);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_4)) {
        printf("egress_port_setup() failed for port %d\n", egress_port_4);
        return -1;
    }

    bshell(unit, "pw start report +raw +decode +pmd");

    return BCM_E_NONE;
}

void
verify(int unit)
{
    char str[512];

    bshell(unit, "vlan show");
    bshell(unit, "l3 ing_intf show");
    bshell(unit, "l3 intf show");
    bshell(unit, "l3 egress show");
    bshell(unit, "l3 ecmp show");
    bshell(unit, "l3 route show");
    bshell(unit, "clear c");

    printf("\nIP PKT SMAC=0xa1 SIP=20.20.20.21\n");
    printf("\nSending IPv4 unicast packet to ingress_port : %d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000010000000000a18100000b08004500002e0000000040ff3d95141414150a0a0a0a00000000000000000000000000000000000000000000000000009951bab9; sleep quiet 2", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    printf("\nIP PKT SMAC=0xa2 SIP=20.20.20.101\n");
    printf("\nSending IPv4 unicast packet to ingress_port : %d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000010000000000a28100000b08004500002e0000000040ff3d45141414650a0a0a0a000000000000000000000000000000000000000000000000000054531a65; sleep quiet 2", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    printf("\nIP PKT SMAC=0xa3 SIP=20.20.20.23\n");
    printf("\nSending IPv4 unicast packet to ingress_port : %d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000010000000000a38100000b08004500002e0000000040ff3d93141414170a0a0a0a0000000000000000000000000000000000000000000000000000ca9a1395; sleep quiet 2", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    printf("\nIP PKT SMAC=0xa4 SIP=20.20.20.72\n");
    printf("\nSending IPv4 unicast packet to ingress_port : %d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000010000000000a48100000b08004500002e0000000040ff3d62141414480a0a0a0a0000000000000000000000000000000000000000000000000000c200ce61; sleep quiet 2", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    return;
}

/* Flex digest configuration for H-ECMP hash selection */
bcm_error_t config_flex_digest(int unit)
{
  bcm_error_t     rv = BCM_E_NONE;
  int             seed_profile_id;
  int             norm_profile_id;
  int             hash_profile_id;
  int             norm_enable;
  int             seed;

  print bcm_flexdigest_init(unit);

  /**** NORMALIZATION and SEED SETUP ****/
  seed_profile_id = 1;
  rv = bcm_flexdigest_norm_seed_profile_create(unit, BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &seed_profile_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_norm_seed_profile_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  seed = 0xbabaface;
  rv = bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id, bcmFlexDigestNormSeedControlBinASeed, seed);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_norm_seed_profile_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  seed = 0xa5a5a5a5;
  print bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id, bcmFlexDigestNormSeedControlBinBSeed, seed);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_norm_seed_profile_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  seed = 0x5a5a5a5a;
  print bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id, bcmFlexDigestNormSeedControlBinCSeed, seed);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_norm_seed_profile_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  norm_profile_id = 0;
  rv = bcm_flexdigest_norm_profile_create(unit, BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &norm_profile_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_norm_profile_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  norm_enable = TRUE;
  rv = bcm_flexdigest_norm_profile_set(unit, norm_profile_id, bcmFlexDigestNormProfileControlNorm, norm_enable);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_norm_profile_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_norm_profile_set(unit, norm_profile_id, bcmFlexDigestNormProfileControlSeedId, seed_profile_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_norm_profile_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /**** LOOKUP SETUP ****/
  bcm_flexdigest_qset_t qset;

  BCM_FLEXDIGEST_QSET_INIT(qset);
  BCM_FLEXDIGEST_QSET_ADD(qset, bcmFlexDigestQualifyMatchId);

  bcm_flexdigest_group_t group;
  bcm_flexdigest_entry_t entry;
  bcm_flexdigest_match_id_t match_id;
  int pri = 0, mask_1, mask_2;

  mask_1 = 0xFFFF; /*General Mask for all full fields */
  mask_2 = 0x0FFF; /*For VLAN ID from VLAN tag */

  /*** GROUP 0 (Pri = 0) for L2 packets ***/
  pri = 0;
  rv = bcm_flexdigest_group_create(unit, qset, pri, &group);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_group_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_entry_create(unit, group, &entry);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_entry_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_match_id_create(unit, &match_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_match_id_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL2HdrL2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_match_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Qualifiers */
  rv = bcm_flexdigest_qualify_MatchId(unit, entry, match_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_qualify_MatchId(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Actions : Bin field population */
  /* Set B */
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds0, BCM_FLEXDIGEST_FIELD_INGRESS_PP_PORT, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds1, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds2, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds3, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_32_47, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds4, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds5, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds6, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_32_47, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds7, BCM_FLEXDIGEST_FIELD_OUTER_OTAG_0_15, mask_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds14, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds15, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_C_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_entry_install(unit, entry);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_entry_install(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /*** GROUP 1 (Pri = 1) for IPV4 and IPv4 UDP packets ***/
  pri = 1;
  rv = bcm_flexdigest_group_create(unit, qset, pri, &group);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_group_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /** Entry 0 : IPv4 UDP **/
  rv = bcm_flexdigest_entry_create(unit, group, &entry);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_entry_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_match_id_create(unit, &match_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_match_id_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL3L4HdrUdp);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_match_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Qualifiers */
  rv = bcm_flexdigest_qualify_MatchId(unit, entry, match_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_qualify_MatchId(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Actions : Bin field population */
  /* Set A */
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds8, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds9, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds10, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds11, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds12, BCM_FLEXDIGEST_FIELD_OUTER_L4_SRC_PORT, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds13, BCM_FLEXDIGEST_FIELD_OUTER_L4_DST_PORT, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Set B */
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds8, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds9, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds10, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds11, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds12, BCM_FLEXDIGEST_FIELD_OUTER_L4_SRC_PORT, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds13, BCM_FLEXDIGEST_FIELD_OUTER_L4_DST_PORT, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_entry_install(unit, entry);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_entry_install(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /** Entry 1 : IPv4 Unknown Protocol Type **/
  rv = bcm_flexdigest_entry_create(unit, group, &entry);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_entry_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_match_id_create(unit, &match_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_match_id_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL3L4HdrUnknownL4);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_match_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Qualifiers */
  rv = bcm_flexdigest_qualify_MatchId(unit, entry, match_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_qualify_MatchId(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Actions : Bin field population */
  /* Set B */
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds8,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds9,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds10,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds11,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds12,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_TTL, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds13,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_PROTOCOL, mask_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_action_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_entry_install(unit, entry);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_entry_install(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /*** HASH SETUP ***/
  hash_profile_id = 0;
  rv = bcm_flexdigest_hash_profile_create(unit, BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &hash_profile_id);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_hash_profile_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  uint16 salt[16] = {
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

  rv = bcm_flexdigest_hash_salt_set(unit, bcmFlexDigestHashBinB, sizeof(salt)/sizeof(salt[0]), salt);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_hash_salt_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  int enable = 1;
  rv = bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlPreProcessBinB, enable);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_hash_profile_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlXorSaltBinB, enable);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_hash_profile_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlBinB0FunctionSelection, BCM_FLEXDIGEST_HASH_FUNCTION_CRC32HI);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_hash_profile_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlBinB1FunctionSelection, BCM_FLEXDIGEST_HASH_FUNCTION_CRC32_ETH_LO);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_flexdigest_hash_profile_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /**** HASH BITS SELECTION SETUP ****/
  int ecmp_level1_seed = 0x7a7a; /* ECMP level1 random seed value */
  int ecmp_level2_seed = 0x3435; /* ECMP level2 random seed value */
  int ecmp_flow_select = 5;          /* Use flow based with macro selection block : B0 Low */
  int ecmp_concat_mode = 0;          /* Don't use concat mode */
  int ecmp_min_offset  = 32;         /* from B0 Offset 0 */
  int ecmp_max_offset  = 63;         /* to B1 Offset 15 */
  int ecmp_offset_stride = 1;        /* Increment by 1 */

  rv = bcm_switch_control_set(unit, bcmSwitchECMPLevel1RandomSeed, ecmp_level1_seed);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchECMPLevel2RandomSeed, ecmp_level2_seed);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmpOverlay, ecmp_flow_select);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_switch_control_set(unit, bcmSwitchMacroFlowEcmpHashOverlayConcatEnable, ecmp_concat_mode);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_switch_control_set(unit, bcmSwitchMacroFlowHashOverlayMinOffset, ecmp_min_offset);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_switch_control_set(unit, bcmSwitchMacroFlowHashOverlayMaxOffset, ecmp_max_offset);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_switch_control_set(unit, bcmSwitchMacroFlowHashOverlayStrideOffset, ecmp_offset_stride);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  return BCM_E_NONE;
}

/*
 * IP H-ECMP pointing route configuration.
 */
bcm_error_t config_ip_ecmp(int unit)
{
  bcm_error_t     rv = BCM_E_NONE;
  bcm_vlan_t      ingress_vlan = 11;
  bcm_vlan_t      egress_vlan_1 = 12;
  bcm_vlan_t      egress_vlan_2 = 13;
  bcm_vlan_t      egress_vlan_3 = 14;
  bcm_vlan_t      egress_vlan_4 = 15;
  bcm_vrf_t       vrf = 1;

  bcm_mac_t       router_mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
  bcm_mac_t       router_mac_out_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
  bcm_mac_t       router_mac_out_2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x03};
  bcm_mac_t       router_mac_out_3 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x04};
  bcm_mac_t       router_mac_out_4 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x05};
  bcm_mac_t       mac_in = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
  bcm_mac_t       mac_out_1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
  bcm_mac_t       mac_out_2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
  bcm_mac_t       mac_out_3 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
  bcm_mac_t       mac_out_4 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x55};
  bcm_mac_t       mac_mask = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

  bcm_ip_t        dip = 0x0a0a0a0a;  /* 10.10.10.10 */

  bcm_if_t        l3_egr_if_in = 100;
  bcm_if_t        l3_egr_if_out_1 = 200;
  bcm_if_t        l3_egr_if_out_2 = 300;
  bcm_if_t        l3_egr_if_out_3 = 400;
  bcm_if_t        l3_egr_if_out_4 = 500;

  bcm_if_t        ingress_if_in = 10;

  bcm_l3_intf_t    l3_intf_in;
  bcm_l3_intf_t    l3_intf_out;
  bcm_l3_ingress_t l3_ingress;
  bcm_l3_route_t   route;
  bcm_l3_route_t   route_d;
  bcm_l2_station_t l2_station;
  int              station_id;

  bcm_vlan_control_vlan_t vlan_ctrl;
  bcm_l3_egress_t         l3_egress;
  bcm_l3_egress_t         l3_egr_def;
  bcm_if_t                egr_obj_in;
  bcm_if_t                egr_obj_out_1;
  bcm_if_t                egr_obj_out_2;
  bcm_if_t                egr_obj_out_3;
  bcm_if_t                egr_obj_out_4;
  bcm_if_t                l3_egr_def_obj_id;

  bcm_l3_ecmp_member_t    ecmp_member_array[4];
  bcm_l3_egress_ecmp_t    ecmp_grp;
  int                     ecmp_member_count = 2;
  bcm_if_t                ecmp_obj_out_ul_1;
  bcm_if_t                ecmp_obj_out_ul_2;
  bcm_if_t                ecmp_obj_out_ol;

  rv = bcm_port_gport_get(unit, ingress_port, &ingress_gport);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port_1, &egress_gport_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port_2, &egress_gport_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port_3, &egress_gport_3);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(unit, egress_port_4, &egress_gport_4);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create ingress vlan */
  rv = bcm_vlan_create(unit, ingress_vlan);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add ingress port member of ingress vlan */
  rv = bcm_vlan_gport_add(unit, ingress_vlan, ingress_gport, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egress vlan_1 */
  rv = bcm_vlan_create(unit, egress_vlan_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress port_1 member of egress vlan_1 */
  rv = bcm_vlan_gport_add(unit, egress_vlan_1, egress_gport_1, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egress vlan_2 */
  rv = bcm_vlan_create(unit, egress_vlan_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress port_2 member of egress vlan_2 */
  rv = bcm_vlan_gport_add(unit, egress_vlan_2, egress_gport_2, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egress vlan_3 */
  rv = bcm_vlan_create(unit, egress_vlan_3);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress port_3 member of egress vlan_3 */
  rv = bcm_vlan_gport_add(unit, egress_vlan_3, egress_gport_3, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create egress vlan_4 */
  rv = bcm_vlan_create(unit, egress_vlan_4);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add egress port_3 member of egress vlan_4 */
  rv = bcm_vlan_gport_add(unit, egress_vlan_4, egress_gport_4, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 ingress interface for ingress interface */
  bcm_l3_ingress_t_init(&l3_ingress);
  l3_ingress.flags     = BCM_L3_INGRESS_WITH_ID;
  l3_ingress.vrf       = vrf;
  rv = bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if_in);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Config ingress vlan to L3_IIF binding */
  bcm_vlan_control_vlan_t_init(&vlan_ctrl);
  vlan_ctrl.ingress_if = ingress_if_in;
  rv = bcm_vlan_control_vlan_selective_set(unit, ingress_vlan, BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK, &vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_selective_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for ingress interface */
  bcm_l3_intf_t_init(&l3_intf_in);
  l3_intf_in.l3a_flags   = BCM_L3_WITH_ID;
  l3_intf_in.l3a_intf_id = l3_egr_if_in;
  l3_intf_in.l3a_vid     = ingress_vlan;
  sal_memcpy(l3_intf_in.l3a_mac_addr, router_mac_in, sizeof(router_mac_in));
  rv = bcm_l3_intf_create(unit, &l3_intf_in);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for egress interface_1 */
  bcm_l3_intf_t_init(&l3_intf_out);
  l3_intf_out.l3a_flags   = BCM_L3_WITH_ID;
  l3_intf_out.l3a_intf_id = l3_egr_if_out_1;
  l3_intf_out.l3a_vid     = egress_vlan_1;
  sal_memcpy(l3_intf_out.l3a_mac_addr, router_mac_out_1, sizeof(router_mac_out_1));
  rv = bcm_l3_intf_create(unit, &l3_intf_out);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for egress interface_2 */
  bcm_l3_intf_t_init(&l3_intf_out);
  l3_intf_out.l3a_flags   = BCM_L3_WITH_ID;
  l3_intf_out.l3a_intf_id = l3_egr_if_out_2;
  l3_intf_out.l3a_vid     = egress_vlan_2;
  sal_memcpy(l3_intf_out.l3a_mac_addr, router_mac_out_2, sizeof(router_mac_out_2));
  rv = bcm_l3_intf_create(unit, &l3_intf_out);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for egress interface_3 */
  bcm_l3_intf_t_init(&l3_intf_out);
  l3_intf_out.l3a_flags   = BCM_L3_WITH_ID;
  l3_intf_out.l3a_intf_id = l3_egr_if_out_3;
  l3_intf_out.l3a_vid     = egress_vlan_3;
  sal_memcpy(l3_intf_out.l3a_mac_addr, router_mac_out_3, sizeof(router_mac_out_3));
  rv = bcm_l3_intf_create(unit, &l3_intf_out);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for egress interface_4 */
  bcm_l3_intf_t_init(&l3_intf_out);
  l3_intf_out.l3a_flags   = BCM_L3_WITH_ID;
  l3_intf_out.l3a_intf_id = l3_egr_if_out_4;
  l3_intf_out.l3a_vid     = egress_vlan_4;
  sal_memcpy(l3_intf_out.l3a_mac_addr, router_mac_out_4, sizeof(router_mac_out_4));
  rv = bcm_l3_intf_create(unit, &l3_intf_out);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress object for ingress interface */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.port  = ingress_gport;
  l3_egress.intf  = l3_egr_if_in;
  l3_egress.mtu   = 1500;
  sal_memcpy(l3_egress.mac_addr, mac_in, sizeof(mac_in));
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_in);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress object for egress interface_1 */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.port  = egress_gport_1;
  l3_egress.intf  = l3_egr_if_out_1;
  l3_egress.mtu   = 1500;
  sal_memcpy(l3_egress.mac_addr, mac_out_1, sizeof(mac_out_1));
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_out_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress object for egress interface_2 */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.port  = egress_gport_2;
  l3_egress.intf  = l3_egr_if_out_2;
  l3_egress.mtu   = 1500;
  sal_memcpy(l3_egress.mac_addr, mac_out_2, sizeof(mac_out_2));
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_out_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress object for egress interface_3 */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.port  = egress_gport_3;
  l3_egress.intf  = l3_egr_if_out_3;
  l3_egress.mtu   = 1500;
  sal_memcpy(l3_egress.mac_addr, mac_out_3, sizeof(mac_out_3));
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_out_3);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 egress object for egress interface_4 */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.port  = egress_gport_4;
  l3_egress.intf  = l3_egr_if_out_4;
  l3_egress.mtu   = 1500;
  sal_memcpy(l3_egress.mac_addr, mac_out_4, sizeof(mac_out_4));
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_out_4);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create Level-2 ECMP Egress Objects first */
  bcm_l3_ecmp_member_t_init(&ecmp_member_array[0]);
  ecmp_member_array[0].egress_if = egr_obj_out_1;
  bcm_l3_ecmp_member_t_init(&ecmp_member_array[1]);
  ecmp_member_array[1].egress_if = egr_obj_out_2;

  bcm_l3_egress_ecmp_t_init(&ecmp_grp);
  ecmp_grp.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
  ecmp_grp.max_paths        = 64;  /* MAX members allowed for this group */
  rv = bcm_l3_ecmp_create(unit, 0, &ecmp_grp, ecmp_member_count, ecmp_member_array);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ecmp_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  ecmp_obj_out_ul_1 = ecmp_grp.ecmp_intf;
  print ecmp_obj_out_ul_1;

  bcm_l3_ecmp_member_t_init(&ecmp_member_array[0]);
  ecmp_member_array[0].egress_if = egr_obj_out_3;
  bcm_l3_ecmp_member_t_init(&ecmp_member_array[1]);
  ecmp_member_array[1].egress_if = egr_obj_out_4;

  bcm_l3_egress_ecmp_t_init(&ecmp_grp);
  ecmp_grp.ecmp_group_flags = BCM_L3_ECMP_UNDERLAY;
  ecmp_grp.max_paths        = 64;  /* MAX members allowed for this group */
  rv = bcm_l3_ecmp_create(unit, 0, &ecmp_grp, ecmp_member_count, ecmp_member_array);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ecmp_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  ecmp_obj_out_ul_2 = ecmp_grp.ecmp_intf;
  print ecmp_obj_out_ul_2;

  /* Create Level-1 ECMP Egress Object */
  bcm_l3_ecmp_member_t_init(&ecmp_member_array[0]);
  ecmp_member_array[0].egress_if = ecmp_obj_out_ul_1;
  bcm_l3_ecmp_member_t_init(&ecmp_member_array[1]);
  ecmp_member_array[1].egress_if = ecmp_obj_out_ul_2;

  bcm_l3_egress_ecmp_t_init(&ecmp_grp);
  ecmp_grp.ecmp_group_flags = BCM_L3_ECMP_OVERLAY;
  ecmp_grp.max_paths        = 64;  /* MAX members allowed for this group */
  rv = bcm_l3_ecmp_create(unit, 0, &ecmp_grp, ecmp_member_count, ecmp_member_array);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ecmp_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  ecmp_obj_out_ol = ecmp_grp.ecmp_intf;
  print ecmp_obj_out_ol;

  /* Create default egress object */
  bcm_l3_egress_t_init(&l3_egr_def);
  l3_egr_def.intf  = l3_egr_if_out_1;
  l3_egr_def.flags = BCM_L3_COPY_TO_CPU | BCM_L3_DST_DISCARD;
  rv = bcm_l3_egress_create(unit, 0, &l3_egr_def, &l3_egr_def_obj_id);
  if (BCM_FAILURE(rv)) {
      printf("Error in configuring def L3 egress object : %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add L3 default Route in the system fisrt */
  bcm_l3_route_t_init(&route_d);
  route_d.l3a_subnet  = 0x00000000;  /* 0.0.0.0 */
  route_d.l3a_ip_mask = 0x00000000;  /* 0.0.0.0 */
  route_d.l3a_intf    = l3_egr_def_obj_id;
  route_d.l3a_vrf     = vrf;
  rv = bcm_l3_route_add(unit, &route_d);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add Route entry with ECMP object */
  bcm_l3_route_t_init(&route);
  route.l3a_flags  |= BCM_L3_MULTIPATH;
  route.l3a_subnet  = dip;
  route.l3a_ip_mask = bcm_ip_mask_create(24);
  route.l3a_intf    = ecmp_obj_out_ol;
  route.l3a_vrf     = vrf;
  rv = bcm_l3_route_add(unit, &route);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Configure flex digest for ECMP hashing */
  rv = config_flex_digest(unit);
  if (BCM_FAILURE(rv)) {
      printf("Error executing config_flex_digest(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Config my station TCAM */
  bcm_l2_station_t_init(&l2_station);
  sal_memcpy(l2_station.dst_mac, router_mac_in, sizeof(router_mac_in));
  sal_memcpy(l2_station.dst_mac_mask, mac_mask, sizeof(mac_mask));
  rv = bcm_l2_station_add(unit, &station_id, &l2_station);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l2_station_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  
  return BCM_E_NONE;
}

/*
 * execute:
 *  This functions does the following
 *  a)test setup
 *  b)actual configuration (Done in config_ip_ecmp())
 *  c)demonstrates the functionality(done in verify()).
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

    print "~~~ #2) config_ip_ecmp(): ** start **";
    if (BCM_FAILURE((rv = config_ip_ecmp(unit)))) {
        printf("config_ip_ecmp() failed.\n");
        return -1;
    }
    print "~~~ #2) config_ip_ecmp(): ** end **";

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

