/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPMC route RPF mode
 *
 * Usage    : BCM.0> cint td4_hsdk_l3_ipmc_route_rpf_mode.c 
 *
 * config   : bcm56880_a0-generic-32x400.config.yml 
 *
 * Log file : td4_hsdk_l3_ipmc_route_rpf_mode_log.txt
 *
 * Test Topology :
 *
 *
 *                  +------------------------------+
 *                  |                              |  SIP 10.10.10.10 
 *                  |                              |  egress_port_1 cd1
 * DIP 233.1.1.1    |                              +-----------------+   
 * ingress_port cd0 |                              |  VLAN 200 
 * +----------------+          SWITCH-TD4          |   
 * VLAN 20 or 30    |                              |  
 * SIP 10.10.10.10  |                              |  VLAN 300 
 *       or         |                              +-----------------+ 
 * SIP 20.20.20.20  |                              |  egress_port_2 cd2 
 *                  |                              |  SIP 20.20.20.20 
 *                  +------------------------------+
 *
 *                                                                        
 * Summary:
 * ========
 *   This Cint example to show configuration of L3 IPMC route RPF mode using 
 *   BCM APIs.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) Selects three ports and configure them in Loopback mode. Out of these
 *        threee ports, one port is used as ingress_port and the other two ports as
 *        egress_port.
 *
 *     b) Install an IFP rule to copy incoming and outgoing packets to CPU and
 *        start packet watcher to display packet contents.
 *
 *     Note: IFP rule is meant for a testing purpose only (Internal) and it is
 *           nothing to do with an actual functional test.
 *
 *   2) Step2 - Configuration (Done in configure_ipmc_rpf_mode()):
 *   =========================================================
 *      a) Create sg_vlan and add ingress_port as member.
 *
 *      b) Create starg_vlan and add ingress_port as member.
 *
 *      c) Create egress_vlan_1 and add egress_port_1 as member.
 *
 *      d) Create egress_vlan_2 and add egress_port_2 as member.
 *
 *      e) Create l3 ingress interface with l3_iif_lookup key disable for IPMC lookup 
 *         and bind l3 ingress interface with sg_vlan.
 *
 *      f) Create l3 ingress interface with l3_iif_lookup key disable for IPMC lookup 
 *         and bind l3 ingress interface with starg_vlan.
 *
 *      g) Create l3 interface for sg_vlan ingress interface. 
 *
 *      h) Create l3 interface for starg_vlan ingress interface.
 *
 *      i) Create l3 interface for egress interface_1.
 *
 *      j) Create l3 interface for egress interface_2.
 *
 *      k) Create IPMC group for {S, G} destination hit forwading.
 *
 *      l) Create egress interface_1 encapsulation to point from {S, G} IPMC group 
 *
 *      m) Create IPMC group for {*, G} destination hit forwading.
 *
 *      i) Create egress interface_2 encapsulation to point from {*, G} IPMC group
 *
 *      j) Install IPMC entry with {S, G} match to point {S, G} IPMC group with 
 *         EXPECTED_L3_IIF = SG_INGRESS_IF
 *
 *      k) Install IPMC entry with {*, G} match to point {*, G} IPMC group with
 *         EXPECTED_L3_IIF = STARG_INGRESS_IF
 *
 *   3) Step3 - IPMC RPF checks concept: 
 *   =========================================================
 *    IPMC lookup contains {SIP, MIP, VRF and VLAN} as KEY to hit entry in L3_ENTRY_MCAST table.
 *    We will remove VRF KEY configuration here because it doesn't important for our case.
 *    We need to remove VLAN KEY configuration by setting by setting ING_L3_IIF_TABLE.USE_L3_IIF_FOR_IPMC_LOOKUP=0.
 *    L3_ENTRY_MCAST table is programmed with {S, G} and EXPECTED_L3_IIF=SG_INGRESS_IF.
 *    L3_ENTRY_MCAST table is programmed with {*, G} and EXPECTED_L3_IIF=STARG_INGRESS_IF.
 *
 *    Incoming packet on PORT1 with {S, G} with VLAN1
 *      {S, G} entry will hit in IPMC table and incoming L3_IIF (SG_INGRESS_IF) checks against 
 *      l3_expected_iif (SG_INGRESS_IF) and the RPF check PASSES so the packet is forwarded.
 *
 *    Incoming packet on PORT1 with {S, G} with VLAN2
 *      {S, G} entry will hit in IPMC table and incoming L3_IIF (STARG_INGRESS_IF) checks against 
 *      l3_expected_iif (SG_INGRESS_IF) and the RPF check FAILS so the packet is RPF_DROP/RPF_CPU.
 *
 *    Incoming packet on PORT1 with {S', G} with VLAN1
 *      {*, G} entry will hit in IPMC table and incoming L3_IIF (SG_INGRESS_IF) checks against 
 *      l3_expected_iif (STARG_INGRESS_IF) and the RPF check FAILS so the packet is RPF_DROP/RPF_CPU.
 *
 *    Incoming packet on PORT1 with {S', G} with VLAN2
 *      {*, G} entry will hit in IPMC table and incoming L3_IIF (STARG_INGRESS_IF) checks against 
 *      l3_expected_iif (STARG_INGRESS_IF) and the RPF check PASSES so the packet is forwarded.
 *
 *   4) Step4 - Verification (Done in verify()):
 *   ===========================================
 *      a) Send below IPv4 MCAST packet on ingress_port.
 *
 *         DMAC 0x01005e010101
 *         SMAC 0x1
 *         VLAN 20 
 *         DIP 233.1.1.1 
 *         SIP 10.10.10.10
 *         Ingress PORT cd0 
 *
 *         0100 5e01 0101 0000 0000 0001 8100 0014
 *         0800 4502 002e 0000 0000 4000 7cb8 0a0a
 *         0a0a e901 0101 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         4543 dc43
 *
 *      b) Send below IPv4 MCAST packet on ingress_port.
 *
 *         DMAC 0x01005e010101
 *         SMAC 0x2
 *         VLAN 30
 *         DIP 233.1.1.1
 *         SIP 10.10.10.10
 *         Ingress PORT cd0
 *
 *         0100 5e0 10101 0000 0000 0002 8100 001e
 *         0800 4502 002e 0000 0000 4000 7cb8 0a0a
 *         0a0a e901 0101 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         ffa6 93a4
 *
 *      c) Send below IPv4 MCAST packet on ingress_port.
 *
 *         DMAC 0x01005e010101
 *         SMAC 0x3
 *         VLAN 20
 *         DIP 233.1.1.1
 *         SIP 20.20.20.20
 *         Ingress PORT cd0
 *
 *         0100 5e01 0101 0000 0000 0003 8100 0014
 *         0800 4502 002e 0000 0000 4000 68a4 1414
 *         1414 e901 0101 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         bbcb 9360
 *
 *      d) Send below IPv4 MCAST packet on ingress_port.
 *
 *         DMAC 0x01005e010101
 *         SMAC 0x4
 *         VLAN 30
 *         DIP 233.1.1.1
 *         SIP 20.20.20.20
 *         Ingress PORT cd0
 *
 *         01005 e01 0101 0000 0000 0004 8100 001e
 *         0800 4502 002e 0000 0000 4000 68a4 1414
 *         1414 e901 0101 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         44e9 cfa7
 *
 *     e) Expected Result:
 *     ===================
 *        After step 4.a, {S, G} entry will hit with SG_INGRESS_IF == EXPECTED_l3_IIF
 *           Results: RPF check pass so packet forwarded
 *
 *         DMAC 0x01005e010101
 *         SMAC 0x0200
 *         VLAN 200 
 *         DIP 233.1.1.1 
 *         SIP 10.10.10.10
 *         Egress PORT cd1
 *
 *         0100 5e01 0101 0000 0000 0200 8100 00c8
 *         0800 4502 002e 0000 0000 3f00 7db8 0a0a
 *         0a0a e901 0101 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         4543 dc43
 *
 *        After step 4.b, {S, G} entry will hit with SG_INGRESS_IF != EXPECTED_l3_IIF 
 *           Results: RPF check fail so packet drop at ingress 
 *
 *        After step c.c, {*, G} entry will hit with STARG_INGRESS_IF != EXPECTED_l3_IIF 
 *           Results: RPF check fail so packet drop at ingress 
 *
 *        After step 4.d, {*, G} entry will hit with STARG_INGRESS_IF == EXPECTED_l3_IIF
 *           Results: RPF check pass so packet forwarded
 *
 *         DMAC 0x01005e010101
 *         SMAC 0x0300
 *         VLAN 300
 *         DIP 233.1.1.1
 *         SIP 20.20.20.20
 *         Egress PORT cd2
 *
 *         0100 5e01 0101 0000 0000 0300 8100 012c
 *         0800 4502 002e 0000 0000 3f00 69a4 1414
 *         1414 e901 0101 0000 0000 0000 0000 0000
 *         0000 0000 0000 0000 0000 0000 0000 0000
 *         44e9 cfa7
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t    ingress_port;
bcm_port_t    egress_port_1;
bcm_port_t    egress_port_2;
bcm_port_t    ingress_gport;
bcm_port_t    egress_gport_1;
bcm_port_t    egress_gport_2;

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
    int port_list[3];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port   = port_list[0];
    egress_port_1  = port_list[1];
    egress_port_2  = port_list[2];

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
    bshell(unit, "multicast show");
    bshell(unit, "ipmc table show");
    bshell(unit, "clear c");

    printf("\nSend IPMC packet with VLAN=20 | SIP=10.10.10.10 | MIP=233.1.1.1>>\n");
    snprintf(str, 512, "tx 1 pbm=%d data=0x01005e0101010000000000018100001408004502002e0000000040007cb80a0a0a0ae901010100000000000000000000000000000000000000000000000000004543dc43; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    printf("\nSend IPMC packet with VLAN=30 | SIP=10.10.10.10 | MIP=233.1.1.1>>\n");
    snprintf(str, 512, "tx 1 pbm=%d data=0x01005e0101010000000000028100001e08004502002e0000000040007cb80a0a0a0ae90101010000000000000000000000000000000000000000000000000000ffa693a4; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    printf("\nSend IPMC packet with VLAN=20 | SIP=20.20.20.20 | MIP=233.1.1.1>>\n");
    snprintf(str, 512, "tx 1 pbm=%d data=0x01005e0101010000000000038100001408004502002e00000000400068a414141414e90101010000000000000000000000000000000000000000000000000000bbcb9360; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    printf("\nSend IPMC packet with VLAN=30 | SIP=20.20.20.20 | MIP=233.1.1.1>>\n");
    snprintf(str, 512, "tx 1 pbm=%d data=0x01005e0101010000000000048100001e08004502002e00000000400068a414141414e9010101000000000000000000000000000000000000000000000000000044e9cfa7; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    return;
}

/*
 * Configure IP Urpf Strict mode functionality.
 */
int
configure_ipmc_rpf_mode(int unit)
{
  bcm_error_t       rv = BCM_E_NONE;
  bcm_vlan_t        sg_ingress_vlan = 20;
  bcm_vlan_t        starg_ingress_vlan = 30;
  bcm_vlan_t        egress_vlan_1 = 200;
  bcm_vlan_t        egress_vlan_2 = 300;
  bcm_if_t          sg_ingress_if = 1001;
  bcm_if_t          starg_ingress_if = 1002;
  bcm_if_t          ingress_if_1 = 2001;
  bcm_if_t          ingress_if_2 = 2002;
  bcm_if_t          egress_if_1 = 501;
  bcm_if_t          egress_if_2 = 502;
  bcm_vrf_t         vrf = 1;
  bcm_multicast_t   sg_group = 101;
  bcm_multicast_t   starg_group = 102;
  bcm_if_t          encap_id_1;
  bcm_if_t          encap_id_2;

  bcm_mac_t       sg_ingress_if_mac = {0, 0, 0, 0, 0, 0x20};
  bcm_mac_t       starg_ingress_if_mac = {0, 0, 0, 0, 0, 0x30};
  bcm_mac_t       egress_if_1_mac = {0, 0, 0, 0, 0x2, 0};
  bcm_mac_t       egress_if_2_mac = {0, 0, 0, 0, 0x3, 0};

  bcm_ip_t        sg_sip = 0x0a0a0a0a;     /* 10.10.10.10 */
  bcm_ip_t        sg_mip = 0xe9010101;     /* 233.1.1.1 */
  bcm_ip_t        starg_mip = 0xe9010101;  /* 233.1.1.1 */

  bcm_l3_ingress_t         l3_ingress;
  bcm_ipmc_addr_t          ipmc_data;
  bcm_l3_intf_t            l3_egress_intf;
  bcm_multicast_encap_t    ipmc_encap;
  bcm_vlan_control_vlan_t  vlan_ctrl;

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

  /* Create source group ingress vlan */
  rv = bcm_vlan_create(unit, sg_ingress_vlan);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add ingress port member of source group ingress vlan */
  rv = bcm_vlan_gport_add(unit, sg_ingress_vlan, ingress_gport, 0);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create star group ingress vlan */
  rv = bcm_vlan_create(unit, starg_ingress_vlan);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Add ingress port member of star group ingress vlan */
  rv = bcm_vlan_gport_add(unit, starg_ingress_vlan, ingress_gport, 0);
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

  /* Create L3 ingress interface for sg_ingress_vlan ingress interface */
  bcm_l3_ingress_t_init(&l3_ingress);
  l3_ingress.flags     = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_IPMC_DO_VLAN_DISABLE;
  l3_ingress.vrf       = vrf;
  rv = bcm_l3_ingress_create(unit, &l3_ingress, &sg_ingress_if);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Config ingress sg_ingress_vlan to L3_IIF binding */
  bcm_vlan_control_vlan_t_init(&vlan_ctrl);
  vlan_ctrl.ingress_if = sg_ingress_if;
  rv = bcm_vlan_control_vlan_selective_set(unit, sg_ingress_vlan, BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK, &vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_selective_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 ingress interface for starg_ingress_vlan ingress interface */
  bcm_l3_ingress_t_init(&l3_ingress);
  l3_ingress.flags     = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_IPMC_DO_VLAN_DISABLE;
  l3_ingress.vrf       = vrf;
  rv = bcm_l3_ingress_create(unit, &l3_ingress, &starg_ingress_if);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_ingress_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Config ingress starg_ingress_vlan to L3_IIF binding */
  bcm_vlan_control_vlan_t_init(&vlan_ctrl);
  vlan_ctrl.ingress_if = starg_ingress_if;
  rv = bcm_vlan_control_vlan_selective_set(unit, starg_ingress_vlan, BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK, &vlan_ctrl);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_control_vlan_selective_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for sg_vlan ingress interface */
  bcm_l3_intf_t_init(&l3_egress_intf);
  l3_egress_intf.l3a_vid       = sg_ingress_vlan;
  l3_egress_intf.l3a_intf_id   = ingress_if_1;
  l3_egress_intf.l3a_flags     = BCM_L3_WITH_ID;
  sal_memcpy(l3_egress_intf.l3a_mac_addr, sg_ingress_if_mac, sizeof(sg_ingress_if_mac));
  rv = bcm_l3_intf_create(unit, &l3_egress_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for starg_vlan ingress interface */
  bcm_l3_intf_t_init(&l3_egress_intf);
  l3_egress_intf.l3a_vid       = starg_ingress_vlan;
  l3_egress_intf.l3a_intf_id   = ingress_if_2;
  l3_egress_intf.l3a_flags     = BCM_L3_WITH_ID;
  sal_memcpy(l3_egress_intf.l3a_mac_addr, starg_ingress_if_mac, sizeof(starg_ingress_if_mac));
  rv = bcm_l3_intf_create(unit, &l3_egress_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for egress interface_1 */
  bcm_l3_intf_t_init(&l3_egress_intf);
  l3_egress_intf.l3a_vid       = egress_vlan_1;
  l3_egress_intf.l3a_intf_id   = egress_if_1;
  l3_egress_intf.l3a_flags     = BCM_L3_WITH_ID;
  sal_memcpy(l3_egress_intf.l3a_mac_addr, egress_if_1_mac, sizeof(egress_if_1_mac));
  rv = bcm_l3_intf_create(unit, &l3_egress_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create L3 interface for egress interface_2 */
  bcm_l3_intf_t_init(&l3_egress_intf);
  l3_egress_intf.l3a_vid       = egress_vlan_2;
  l3_egress_intf.l3a_intf_id   = egress_if_2;
  l3_egress_intf.l3a_flags     = BCM_L3_WITH_ID;
  sal_memcpy(l3_egress_intf.l3a_mac_addr, egress_if_2_mac, sizeof(egress_if_2_mac));
  rv = bcm_l3_intf_create(unit, &l3_egress_intf);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create {S, G} based musticast IPMC group */
  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &sg_group);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create IPMC encapsulation_id for egress side interface_1 */
  bcm_multicast_encap_t_init(&ipmc_encap);
  ipmc_encap.encap_type = bcmMulticastEncapTypeL3;
  ipmc_encap.l3_intf    = egress_if_1;
  rv = bcm_multicast_encap_create(unit, &ipmc_encap, &encap_id_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_encap_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  print encap_id_1;
  rv = bcm_multicast_egress_add(unit, sg_group, egress_gport_1, encap_id_1);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create {*, G} based musticast IPMC group */
  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &starg_group);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* Create IPMC encapsulation_id for egress side interface_2 */
  bcm_multicast_encap_t_init(&ipmc_encap);
  ipmc_encap.encap_type = bcmMulticastEncapTypeL3;
  ipmc_encap.l3_intf    = egress_if_2;
  rv = bcm_multicast_encap_create(unit, &ipmc_encap, &encap_id_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_encap_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }
  print encap_id_2;
  rv = bcm_multicast_egress_add(unit, starg_group, egress_gport_2, encap_id_2);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_multicast_egress_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* {S, G, SG_INGRESS_IF} triple programming */
  bcm_ipmc_addr_t_init(&ipmc_data);
  ipmc_data.mc_ip_addr = sg_mip;
  ipmc_data.mc_ip_mask = 0xFFFFFFFF;
  ipmc_data.s_ip_addr  = sg_sip;
  ipmc_data.vrf        = vrf;
  ipmc_data.vid        = 0;
  ipmc_data.mtu        = 1500;
  ipmc_data.rp_id      = 0;
  ipmc_data.l3a_intf   = sg_ingress_if;     /* Expected IIF */
  ipmc_data.group      = sg_group;
  ipmc_data.flags      = BCM_IPMC_SOURCE_PORT_NOCHECK | BCM_IPMC_POST_LOOKUP_RPF_CHECK | BCM_IPMC_RPF_FAIL_DROP | BCM_IPMC_RPF_FAIL_TOCPU;
  rv = bcm_ipmc_add(unit, &ipmc_data);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_ipmc_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* {*, G, STARG_INGRESS_IF} triple programming */
  bcm_ipmc_addr_t_init(&ipmc_data);
  ipmc_data.mc_ip_addr = starg_mip;
  ipmc_data.mc_ip_mask = 0xFFFFFFFF;
  ipmc_data.s_ip_addr  = 0;
  ipmc_data.vrf        = vrf;
  ipmc_data.vid        = 0;
  ipmc_data.mtu        = 1500;
  ipmc_data.rp_id      = 0;
  ipmc_data.l3a_intf   = starg_ingress_if;     /* Expected IIF */
  ipmc_data.group      = starg_group;
  ipmc_data.flags      = BCM_IPMC_SOURCE_PORT_NOCHECK | BCM_IPMC_POST_LOOKUP_RPF_CHECK | BCM_IPMC_RPF_FAIL_DROP | BCM_IPMC_RPF_FAIL_TOCPU;
  rv = bcm_ipmc_add(unit, &ipmc_data);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_ipmc_add(): %s.\n", bcm_errmsg(rv));
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

    print "~~~ #2) configure_ipmc_rpf_mode(): ** start **";
    if (BCM_FAILURE((rv = configure_ipmc_rpf_mode(unit)))) {
        printf("configure_ipmc_rpf_mode() failed.\n");
        return -1;
    }
    print "~~~ #2) configure_ipmc_rpf_mode(): ** end **";

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


