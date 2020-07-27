/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : L2 RTAG7 Trunk
 *
 *  Usage    : BCM.0> cint l2_rtag7_trunk.c
 *
 *  config   : rtag7_config.bcm
 *
 *  Log file : l2_rtag7_trunk_log.txt
 *
 *  Test Topology :
 *
 *                   +-------+
 *                   |       |        +--outPort1 (xe1)--->
 *                   |       |        |
 *    -inPort(xe0)-->|   L2  |--trunk-+--OutPort2 (xe2)-->
 *                   |       |        |
 *                   |       |        +--outPort3 (xe3)--->
 *                   +-------+
 *    Summary:
 *    ========
 *    CINT script to show L2 switching using Trunk as egress port,
 *    RTAG7 is used as hashing for trunk selection.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1)Step1 - Test Setup (Done in test_setup())
 *    =========================================
 *    a) Selects Four ports and configure them in Loopback mode. Out of these Four ports,
 *       one port is used as Ingress port and the other as Egress port. Install a rule
 *       to copy incoming packets to CPU and start packet watcher.
 *
 *    2)Step2 - Configuration (Done in config_l2_trunk_rtag7())
 *    =======================================================
 *    a) Create the trunk using Three port and Configure RTAG7 setting on the trunk. 
 *       This is done in config_l2_trunk_rtag7().
 *       
 *    3)Step3 - Verification (Done in verify())
 *    =======================================
 *    a) For checking RTAG7 setting on the trunk. send the below packet on ingress port. 
 *       Send traffic with incrementing SA addresses to the ingress port,
 *       observe the routed traffic distributed among the outgoing trunk ports.
 *
 *       SAMPLE-1
 *       =========
 *       DA 0x00000000AAAA
 *       SA 0x000000002222
 *       VLAN 12
 *
 *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
 *       08 00 45 00 00 3C 00 00 00 00 40 FF 9B 0F 0A 0A 
 *       0A 01 C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09 
 *       0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 
 *       1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 2E CB
 *       F6 84 
 *
 *
 *       SAMPLE-2
 *       =========
 *       DA 0x00000000AAAA
 *       SA 0x000000002224
 *       VLAN 12
 *
 *       00 00 00 00 AA AA 00 00 00 00 22 24 81 00 00 0C 
 *       08 00 45 00 00 3C 00 00 00 00 40 FF 9B 0F 0A 0A 
 *       0A 01 C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09 
 *       0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 
 *       1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 2E CB
 *       F6 84 
 *
 *       SAMPLE-3
 *       =========
 *       DA 0x00000000AAAA
 *       SA 0x000000002226
 *       VLAN 12
 *
 *       00 00 00 00 AA AA 00 00 00 00 22 26 81 00 00 0C 
 *       08 00 45 00 00 3C 00 00 00 00 40 FF 9B 0F 0A 0A 
 *       0A 01 C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09 
 *       0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 
 *       1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 2E CB
 *       F6 84 
 *
 *    b)Expected Result
 *     ================
 *     NOTE: Below is the snipet of show c output to illustrate packets are loadbalanced
 *           Across all egress ports
 *     For validating the RTAG7 setting on the trunk. Below is the counters output
 *       CDMIB_RPKT.cd0                 3         +3         1/s
 *       CDMIB_RPKT.cd1                 1         +1
 *       CDMIB_RPKT.cd2                 1         +1
 *       CDMIB_RPKT.cd3                 1         +1         1/s
 *       CDMIB_TPKT.cd0                 3         +3         1/s
 *       CDMIB_TPKT.cd1                 1         +1
 *       CDMIB_TPKT.cd2                 1         +1
 *       CDMIB_TPKT.cd3                 1         +1         1/s
 */

cint_reset();
bcm_vlan_t vid = 12;
bcm_port_t      Ingress_port = 1;
bcm_port_t      Egress_port_1 = 2,Egress_port_2 = 3,Egress_port_3 = 4;

/* This function is written so that hardcoding of port 
   numbers in Cint scripts is removed. This function gives
   required number of ports
   */
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

  int i=0,port=0,rv=0;
  bcm_port_config_t configP;
  bcm_pbmp_t ports_pbmp;

  rv = bcm_port_config_get(unit,&configP);
  if(BCM_FAILURE(rv)) {
    printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
    return rv;
  }

  ports_pbmp = configP.e;
  for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
    if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
      port_list[port]=i;
      port++;
    }
  }

  if (( port == 0 ) || ( port != num_ports )) {
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
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

  BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));
  return BCM_E_NONE;
}


/* 
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous 
 * loopback of the packet.
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

  BCM_FIELD_QSET_INIT(qset);
  BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);

  BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group));

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

  return BCM_E_NONE;
}

/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
  int port_list[4];

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  Ingress_port = port_list[0];
  Egress_port_1 = port_list[1];
  Egress_port_2 = port_list[2];
  Egress_port_3 = port_list[3];

  if (BCM_E_NONE != ingress_port_setup(unit, Ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", Ingress_port);
    return -1;
  }


  if (BCM_E_NONE != egress_port_setup(unit, Egress_port_1)) {
    printf("egress_port_setup() failed for port %d\n", Egress_port_1);
    return -1;
  }


  if (BCM_E_NONE != egress_port_setup(unit, Egress_port_2)) {
    printf("egress_port_setup() failed for port %d\n", Egress_port_2);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, Egress_port_3)) {
    printf("egress_port_setup() failed for port %d\n", Egress_port_3);
    return -1;
  }


  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}

/* 
 * This functions Create trunk based on the egress port 
 * Check ingress_port_setup() and egress_port_setup().
 */

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

/* 
 * This functions adds new port as part of trunk member 
 * Check ingress_port_setup() and egress_port_setup().
 */


bcm_error_t trunk_add_member(int unit, bcm_trunk_t tid, int port)
{
  int rv;
  bcm_trunk_member_t member_array;
  BCM_GPORT_MODPORT_SET(member_array.gport, 0, port);
  rv =  bcm_trunk_member_add(unit, tid, &member_array);
  printf ("bcm_trunk_member_add: %s\n", bcm_errmsg(BCM_FAILURE(rv)));
  if (BCM_FAILURE(rv)) {
    printf("Error executing trunk_add_member(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}


bcm_error_t config_l2_trunk_rtag7(int unit)
{
  int flags;
  int rv;
  bcm_gport_t gport_in;
  bcm_gport_t out_gport_1;
  bcm_gport_t out_gport_2;
  bcm_gport_t out_gport_3;
  bcm_gport_t trunk_gport;
  bcm_trunk_t trunk_id = 1;
  bcm_port_t trunk_ports[3];
  bcm_l2_addr_t l2addr;
  bcm_mac_t da_1  = {0x00,0x00,0x00,0x00,0xAA,0xAA};
  bcm_mac_t da_2  = {0x00,0x00,0x00,0x00,0xAA,0xBB};

  rv=bcm_port_gport_get(unit, Ingress_port, &gport_in);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_port_gport_get(unit, Egress_port_1, &out_gport_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_port_gport_get(unit, Egress_port_2, &out_gport_2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_port_gport_get(unit, Egress_port_3, &out_gport_3);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_vlan_create(unit, vid);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_vlan_gport_add(unit, vid, gport_in, 0);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_vlan_gport_add(unit, vid, out_gport_1, 0);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_vlan_gport_add(unit, vid, out_gport_2, 0);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_vlan_gport_add(unit, vid, out_gport_3, 0);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_gport_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  trunk_ports[0] = Egress_port_1;
  trunk_ports[1] = Egress_port_2;
  trunk_ports[2] = Egress_port_3;

  rv=create_trunk(unit, &trunk_id, 3, &trunk_ports);
  if (BCM_FAILURE(rv)) {
    printf("Error executing create_trunk(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
  print trunk_gport;

  rv=bcm_switch_control_get(unit, bcmSwitchHashControl, &flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  flags |= (BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE |
      BCM_HASH_CONTROL_TRUNK_NUC_SRC |
      BCM_HASH_CONTROL_TRUNK_NUC_DST |
      BCM_HASH_CONTROL_TRUNK_NUC_MODPORT);

  rv=bcm_switch_control_set(unit, bcmSwitchHashControl, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Only use L2 fields for hashing even for IP packets */
  rv=bcm_switch_control_get(unit,bcmSwitchHashSelectControl, &flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  flags |=(BCM_HASH_FIELD0_DISABLE_IP4|
      BCM_HASH_FIELD1_DISABLE_IP4|
      BCM_HASH_FIELD0_DISABLE_IP6|
      BCM_HASH_FIELD1_DISABLE_IP6);

  rv= bcm_switch_control_set(unit, bcmSwitchHashSelectControl, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Block A - L2 packet field selection (RTAG7_HASH_FIELD_BMAP3) */
  rv = bcm_switch_control_set(unit, bcmSwitchHashL2Field0,
      BCM_HASH_FIELD_SRCMOD |
      BCM_HASH_FIELD_SRCPORT |
      BCM_HASH_FIELD_VLAN |
      BCM_HASH_FIELD_ETHER_TYPE |
      BCM_HASH_FIELD_MACDA_LO |
      BCM_HASH_FIELD_MACDA_MI |
      BCM_HASH_FIELD_MACDA_HI |
      BCM_HASH_FIELD_MACSA_LO |
      BCM_HASH_FIELD_MACSA_MI |
      BCM_HASH_FIELD_MACSA_HI);

  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Block B - L2 packet field selection */
  rv = bcm_switch_control_set(unit, bcmSwitchHashL2Field1,
      BCM_HASH_FIELD_SRCMOD |
      BCM_HASH_FIELD_SRCPORT |
      BCM_HASH_FIELD_VLAN |
      BCM_HASH_FIELD_ETHER_TYPE |
      BCM_HASH_FIELD_MACDA_LO |
      BCM_HASH_FIELD_MACDA_MI |
      BCM_HASH_FIELD_MACDA_HI |
      BCM_HASH_FIELD_MACSA_LO |
      BCM_HASH_FIELD_MACSA_MI |
      BCM_HASH_FIELD_MACSA_HI);

  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Configure HASH A and HASH B functions */
  rv=bcm_switch_control_set(unit, bcmSwitchHashField0Config,
      BCM_HASH_FIELD_CONFIG_CRC32LO); /* BCM_HASH_FIELD_CONFIG_CRC32LO */
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_switch_control_set(unit, bcmSwitchHashField1Config,
      BCM_HASH_FIELD_CONFIG_CRC16); /* BCM_HASH_FIELD_CONFIG_CRC32HI */
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  bcm_l2_addr_t_init(&l2addr, da_1, vid);
  l2addr.port = trunk_gport;

  rv=bcm_l2_addr_add(unit, &l2addr);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l2_addr_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  bcm_l2_addr_t_init(&l2addr, da_2, vid);
  l2addr.port = trunk_gport;

  rv=bcm_l2_addr_add(unit, &l2addr);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l2_addr_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }	

  return BCM_E_NONE;
}

void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");

  /*
   *   For checking RTAG7 setting on the trunk. send the below packet on ingress port. 
   *   Send traffic with incrementing SA addresses to the ingress port,
   *   observe the routed traffic distributed among the outgoing trunk ports.
   *
   *       SAMPLE-1
   *       =========
   *       DA 0x00000000AAAA
   *       SA 0x000000002222
   *       VLAN 12
   *
   *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
   *       08 00 45 00 00 3C 00 00 00 00 40 FF 9B 0F 0A 0A 
   *       0A 01 C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09 
   *       0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 
   *       1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 2E CB
   *       F6 84 
   *
   *       SAMPLE-2
   *       =========
   *       DA 0x00000000AAAA
   *       SA 0x000000002224
   *       VLAN 12
   *
   *       00 00 00 00 AA AA 00 00 00 00 22 24 81 00 00 0C 
   *       08 00 45 00 00 3C 00 00 00 00 40 FF 9B 0F 0A 0A 
   *       0A 01 C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09 
   *       0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 
   *       1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 2E CB
   *       F6 84 
   *
   *       SAMPLE-3
   *       =========
   *       DA 0x00000000AAAA
   *       SA 0x000000002226
   *       VLAN 12
   *
   *       00 00 00 00 AA AA 00 00 00 00 22 26 81 00 00 0C 
   *       08 00 45 00 00 3C 00 00 00 00 40 FF 9B 0F 0A 0A 
   *       0A 01 C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09 
   *       0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 
   *       1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 2E CB
   *       F6 84 
   *
   * NOTE: Use above traffic to get loadbalance on all egress port and 
   *       show c out after each packet shows every egress port getting 
   *       one packet each
   */


  bshell(unit, "clear c"); 

  printf("\nSending packet to Ingress_port:%d\n", Ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500003C0000000040FF9B0F0A0A0A01C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20212223242526272ECBF684; sleep quiet 1", Ingress_port);
  bshell(unit, str);
  bshell(unit, "show c");
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022248100000C08004500003C0000000040FF9B0F0A0A0A01C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20212223242526272ECBF684; sleep quiet 1", Ingress_port);
  bshell(unit, str);
  bshell(unit, "show c");
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022268100000C08004500003C0000000040FF9B0F0A0A0A01C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20212223242526272ECBF684; sleep quiet 1", Ingress_port);
  bshell(unit, str);
  bshell(unit, "show c");

  return BCM_E_NONE;
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;
  bshell(unit, "config show; a ; version");
  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }

  if (BCM_FAILURE((rv = config_l2_trunk_rtag7(unit)))) {
    printf("config_l2_trunk_rtag7() failed.\n");
    return -1;
  }

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

