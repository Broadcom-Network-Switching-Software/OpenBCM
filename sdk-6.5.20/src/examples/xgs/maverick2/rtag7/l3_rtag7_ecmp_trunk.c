/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : L3 RTAG7 ECMP Trunk
 *
 *  Usage    : BCM.0> cint l3_rtag7_trunk.c
 *
 *  config   : rtag7_config.bcm
 *
 *  Log file : l3_rtag7_ecmp_tunk_log.txt
 *  
 *  Test Topology :
 * 
 * 
 *              +-------+        VID=13   |--port 1
 *              |       |        +--ECMP1-+--port 2
 *              |       |Egress  |        |--port 3
 *    ingress-->|  L3   |--ECMP--+
 *     VID=12   |       |        |        |--port 4
 *              |       |        +--ECMP2-+--port 5
 *              +-------+         VID=14  |--port 6
 * 
 * 
 * 
 *    Summary:
 *    ========
 *    CINT script to show L3 routing using both ECMP and Trunk,
 *    there are two ECMP paths, each of which is on a trunk of three physical ports:
 *    RTAG7 is used as hashing for ECMP and trunking.
 *    this example demonstrates how to configure the RTAG7 to support both
 *    ECMP and trunk simultaneously.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1)Step1 - Test Setup (Done in test_setup())
 *    =========================================
 *    a) Selects Seven ports and configure them in Loopback mode. Out of these Seven ports,
 *       one port is used as Ingress port and the other as Egress port. Install a rule
 *       to copy incoming packets to CPU and start packet watcher. 
 *
 *    2)Step2 - Configuration (Done in config_l3_trunk_ecmp_rtag7())
 *    ===========================================================
 *    a) Create the trunk using Three port and Configure RTAG7 setting on the trunk. 
 *       Here we use three physical ports each to configure two trunks.
 *       Which in turn configured as two ECMP paths. RTAG7 is used as hashing for ECMP and trunking.
 *       This is done in config_l3_trunk_ecmp_rtag7()
 *       
 *    3)Step3 - Verification (Done in verify())
 *    =======================================
 *    a) For checking RTAG7 setting on the trunk and ECMP. send the below packet on ingress port. 
 *
 *       SAMPLE-1
 *       =========
 *       DA 0x00000000AAAA
 *       SA 0x000000002222
 *       VLAN 12
 *       DIP=192.168.10.1 
 *       SIP =10.10.10.1
 *
 *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C
 *       08 00 45 00 00 2E 00 00 00 00 40 FF 9B 1D 0A 0A 
 *       0A 01 C0 A8 0A 01 43 03 1F E5 B2 9E CF 90 C1 81 
 *       3A 56 99 BD 78 02 A6 9E FB 49 9E 95 BA 5A BF 76 
 *       C3 48 74 7B 
 *
 *
 *       SAMPLE-2
 *       =========
 *       DA 0x00000000AAAA
 *       SA 0x000000002222
 *       VLAN 12
 *       DIP=192.168.10.24 
 *       SIP =10.10.10.24
 *
 *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
 *       08 00 45 00 00 2E 00 00 00 00 40 FF 9A EF 0A 0A 
 *       0A 18 C0 A8 0A 18 3A 10 1D 7B 40 01 84 D9 B1 55 
 *       D1 50 6C FE 82 7B 76 83 4F 3C F7 F6 55 50 85 26 
 *       FA 5E 7E 81 
 *
 *
 *       SAMPLE-3
 *       =========
 *       DA 0x00000000AAAA
 *       SA 0x000000002222
 *       VLAN 12
 *       DIP=192.168.10.33 
 *       SIP =10.10.10.33
 *
 *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C
 *       08 00 45 00 00 2E 00 00 00 00 40 FF 9A DD 0A 0A 
 *       0A 21 C0 A8 0A 21 C4 65 87 43 14 45 B9 76 63 DB 
 *       AB C3 4F DF 83 CD BC 36 E5 B5 1A 20 5D EA E7 B3 
 *       B8 B4 B1 75 
 *
 *
 *       SAMPLE-4
 *       =========
 *       DA 0x00000000AAAA
 *       SA 0x000000002222
 *       VLAN 12
 *       DIP=192.168.10.51 
 *       SIP =10.10.10.51
 *
 *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
 *       08 00 45 00 00 2E 00 00 00 00 40 FF 9A B9 0A 0A 
 *       0A 33 C0 A8 0A 33 B8 94 D7 BC 72 CE 32 C9 2D 38 
 *       60 3C 1D C9 CA E9 9C 03 3B 7C 6F 25 44 54 4B A1 
 *       32 1E 69 5D 
 *
 *
 *       SAMPLE-5
 *       =========
 *       DA 0x00000000AAAA
 *       SA 0x000000002222
 *       VLAN 12
 *       DIP=192.168.10.66 
 *       SIP =10.10.10.66
 *       
 *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
 *       08 00 45 00 00 2E 00 00 00 00 40 FF 9A 9B 0A 0A 
 *       0A 42 C0 A8 0A 42 57 62 6D F6 D1 8E 57 2A 98 49 
 *       37 9B 54 2A 94 C3 64 A5 65 C9 22 3A 79 55 68 EC 
 *       CD 72 66 8F 
 *
 *
 *       SAMPLE-6
 *       =========
 *       DA 0x00000000AAAA
 *       SA 0x000000002222
 *       VLAN 12
 *       DIP=192.168.10.87 
 *       SIP =10.10.10.87
 *
 *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
 *       08 00 45 00 00 2E 00 00 00 00 40 FF 9A 71 0A 0A 
 *       0A 57 C0 A8 0A 57 EE E4 EF B9 8A BC A1 34 A4 08 
 *       32 69 93 D1 52 D3 34 92 63 E0 8C 4B 38 A5 AF D1 
 *       E5 1F A4 DC 
 *
 *    b)Expected Result:
 *      ================
 *     NOTE: Below is the snipet of show c output to illustrate packets are loadbalanced
 *           Across all egress ports
 *     For validating the RTAG7 setting on the ecmp and trunk. Below is the counters output
 *       CDMIB_RPKT.cd0                 6         +6         1/s
 *       CDMIB_RPKT.cd1                 1         +1
 *       CDMIB_RPKT.cd2                 1         +1
 *       CDMIB_RPKT.cd3                 1         +1
 *       CDMIB_RPKT.cd4                 1         +1         1/s
 *       CDMIB_RPKT.cd5                 1         +1
 *       CDMIB_RPKT.cd6                 1         +1
 *       CDMIB_TPKT.cd0                 6         +6         1/s
 *       CDMIB_TPKT.cd1                 1         +1
 *       CDMIB_TPKT.cd2                 1         +1
 *       CDMIB_TPKT.cd3                 1         +1
 *       CDMIB_TPKT.cd4                 1         +1         1/s
 *       CDMIB_TPKT.cd5                 1         +1
 *       CDMIB_TPKT.cd6                 1         +1
 */


cint_reset();
bcm_port_t      Ingress_port = 1;
bcm_port_t      Egress_port_1_1 = 2,Egress_port_1_2 = 3,Egress_port_1_3 = 4;
bcm_port_t      Egress_port_2_1 = 5,Egress_port_2_2 = 6,Egress_port_2_3 = 7;
bcm_field_group_t group = -1;

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
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

  if (group == -1)
  {
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, 0, &group));
  }

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
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

  if (group == -1)
  {
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, 0, &group));
  }
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
  int port_list[7];

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 7)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  Ingress_port = port_list[0];
  Egress_port_1_1 = port_list[1];
  Egress_port_1_2 = port_list[2];
  Egress_port_1_3 = port_list[3];
  Egress_port_2_1 = port_list[4];
  Egress_port_2_2 = port_list[5];
  Egress_port_2_3 = port_list[6];

  if (BCM_E_NONE != ingress_port_setup(unit, Ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", Ingress_port);
    return -1;
  }


  if (BCM_E_NONE != egress_port_setup(unit, Egress_port_1_1)) {
    printf("egress_port_setup() failed for port %d\n", Egress_port_1_1);
    return -1;
  }


  if (BCM_E_NONE != egress_port_setup(unit, Egress_port_1_2)) {
    printf("egress_port_setup() failed for port %d\n", Egress_port_1_2);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, Egress_port_1_3)) {
    printf("egress_port_setup() failed for port %d\n", Egress_port_1_3);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, Egress_port_2_1)) {
    printf("egress_port_setup() failed for port %d\n", Egress_port_2_1);
    return -1;
  }


  if (BCM_E_NONE != egress_port_setup(unit, Egress_port_2_2)) {
    printf("egress_port_setup() failed for port %d\n", Egress_port_2_2);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, Egress_port_2_3)) {
    printf("egress_port_setup() failed for port %d\n", Egress_port_2_3);
    return -1;
  }


  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}

/* This functions Create vlan and add port to vlan */
bcm_error_t vlan_create_add_port(int unit, int vid, int port)
{
  bcm_pbmp_t pbmp, upbmp;
  bcm_error_t rv;

  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(upbmp);
  BCM_PBMP_PORT_ADD(pbmp, port);
  rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

/* This functions Create vlan and add array of ports to vlan */
bcm_error_t vlan_create_add_ports(int unit, int vid, int count, bcm_port_t *ports)
{
  bcm_pbmp_t pbmp, upbmp;
  bcm_error_t rv;
  int i;

  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(upbmp);

  for (i = 0; i < count; i++) {
    BCM_PBMP_PORT_ADD(pbmp, ports[i]);
  }

  rv= bcm_vlan_port_add(unit, vid, pbmp, upbmp);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
  /*
     bcm_vlan_create(unit, egr_vid);
     print bcm_vlan_gport_add(unit, vid, out_gport_1, 0);
     print bcm_vlan_gport_add(unit, vid, out_gport_2, 0);
     print bcm_vlan_gport_add(unit, vid, out_gport_3, 0);
     */
}

/* This functions Create L3 interface */
bcm_error_t create_l3_interface(int unit, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
  bcm_l3_intf_t l3_intf;
  bcm_error_t rv = BCM_E_NONE;

  /* L3 interface and egress object for access */
  bcm_l3_intf_t_init(&l3_intf);
  l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
  sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
  l3_intf.l3a_vid = vid;
  rv = bcm_l3_intf_create(unit, &l3_intf);
  *intf_id = l3_intf.l3a_intf_id;

  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_intf_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

/* This functions Create L3 egress object */
bcm_error_t create_egr_obj(int unit, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
    bcm_if_t *egr_obj_id)
{
  bcm_l3_egress_t l3_egress;
  bcm_error_t rv = BCM_E_NONE;

  bcm_l3_egress_t_init(&l3_egress);
  sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
  /* l3_egress.vlan = l3_if; vid; */
  l3_egress.intf  = l3_if;
  l3_egress.port = gport;
  rv= bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

/* This functions Adding router mac */
bcm_error_t config_router_mac(int unit, bcm_mac_t router_mac, bcm_vlan_t ing_vlan)
{
  bcm_l2_addr_t l2_addr;
  bcm_error_t rv = BCM_E_NONE;

  bcm_l2_addr_t_init(&l2_addr, router_mac, ing_vlan);
  l2_addr.flags |= (BCM_L2_L3LOOKUP | BCM_L2_STATIC);
  rv= bcm_l2_addr_add(unit, &l2_addr);

  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l2_addr_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

/*
 * Create a trunk, given array of member ports
 * OUT: trunk ID
 */
bcm_error_t create_trunk(int unit, bcm_trunk_t *tid, int count, bcm_port_t *member_port)
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
 * Enable RTAG7 hashing for ECMP
 */
bcm_error_t global_rtag7_configure(int unit)
{
  int flags;
  bcm_error_t rv = BCM_E_NONE;

  /* HASH_CONTROL */
  rv= bcm_switch_control_get(unit, bcmSwitchHashControl, &flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  flags |= (BCM_HASH_CONTROL_ECMP_ENHANCE |      /* ECMP_HASH_USE_RTAG7 */
      BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE | /* NON_UC_TRUNK_HASH_USE_RTAG7 */
      BCM_HASH_CONTROL_TRUNK_NUC_DST |     /* NON_UC_TRUNK_HASH_DST_ENABLE */
      BCM_HASH_CONTROL_TRUNK_NUC_SRC |     /* NON_UC_TRUNK_HASH_SRC_ENABLE */
      BCM_HASH_CONTROL_MULTIPATH_DIP |     /* ECMP_HASH_USE_DIP */
      BCM_HASH_CONTROL_TRUNK_NUC_MODPORT); /* NON_UC_TRUNK_HASH_MOD_PORT_ENABLE */

  rv= bcm_switch_control_set(unit, bcmSwitchHashControl, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  return rv;
}

/*
 * Configure RTAG7 hashing for trunking, use hash_A
 */
bcm_error_t trunk_rtag7_config(int unit)
{
  int flags;
  bcm_error_t rv = BCM_E_NONE;

  /* Use port based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_TRUNK_UC)*/
  rv=bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelTrunkUc, 0);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  flags = BCM_HASH_FIELD_SRCMOD |
    BCM_HASH_FIELD_SRCPORT |
    BCM_HASH_FIELD_PROTOCOL |
    BCM_HASH_FIELD_DSTL4 |
    BCM_HASH_FIELD_SRCL4 |
    BCM_HASH_FIELD_IP4DST_LO |
    BCM_HASH_FIELD_IP4DST_HI |
    BCM_HASH_FIELD_IP4SRC_LO |
    BCM_HASH_FIELD_IP4SRC_HI;

  /* Block A - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
  rv=bcm_switch_control_set(unit, bcmSwitchHashIP4Field0, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField0, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Configure and HASH A algorithm */
  rv=bcm_switch_control_set(unit, bcmSwitchHashField0Config,
      BCM_HASH_FIELD_CONFIG_CRC32HI);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_switch_control_set(unit, bcmSwitchHashField0Config1,
      BCM_HASH_FIELD_CONFIG_CRC32HI);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Enable preprocess */
  rv=bcm_switch_control_set(unit, bcmSwitchHashField0PreProcessEnable, TRUE);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Configure Seed */
  rv=bcm_switch_control_set(unit, bcmSwitchHashSeed0, 0x33333333);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Trunking Use HASH_A0 (RTAG7_PORT_BASED_HASH->SUB_SEL_TRUNK_UC / OFFSET_TRUNK_UC */
  rv=bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0UnicastOffset, 0 + 0);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_switch_control_set(unit, bcmSwitchTrunkHashSet0NonUnicastOffset, 0 + 0);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  return BCM_E_NONE;
}

/*
 * Configure RTAG7 hashing for ECMP, use hash_B
 */
bcm_error_t ecmp_rtag7_config(int unit)
{
  int flags;
  bcm_error_t rv = BCM_E_NONE;

  /* Use port based hash selection (RTAG7_HASH_SEL->USE_FLOW_SEL_ECMP) */
  rv=bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmp, 0);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  flags = BCM_HASH_FIELD_SRCMOD |
    BCM_HASH_FIELD_SRCPORT |
    BCM_HASH_FIELD_PROTOCOL |
    BCM_HASH_FIELD_DSTL4 |
    BCM_HASH_FIELD_SRCL4 |
    BCM_HASH_FIELD_IP4DST_LO |
    BCM_HASH_FIELD_IP4DST_HI |
    BCM_HASH_FIELD_IP4SRC_LO |
    BCM_HASH_FIELD_IP4SRC_HI;

  /* Block B - L3 packet field selection (RTAG7_HASH_FIELD_BMAP_1) */
  rv=bcm_switch_control_set(unit, bcmSwitchHashIP4Field1, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_switch_control_set(unit, bcmSwitchHashIP4TcpUdpField1, flags);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Configure and HASH B algorithm */
  rv=bcm_switch_control_set(unit, bcmSwitchHashField1Config,
      BCM_HASH_FIELD_CONFIG_CRC32LO);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=bcm_switch_control_set(unit, bcmSwitchHashField1Config1,
      BCM_HASH_FIELD_CONFIG_CRC32LO);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Enable preprocess */
  rv=bcm_switch_control_set(unit, bcmSwitchHashField1PreProcessEnable, TRUE);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Configure Seed */
  rv=bcm_switch_control_set(unit, bcmSwitchHashSeed1, 0x55555555);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Use Hash_B0 for ECMP (RTAG7_PORT_BASED_HASH <SUB_SEL_ECMP=1,OFFSET_ECMP=5) */
  rv=bcm_switch_control_set(unit, bcmSwitchECMPHashSet0Offset, 16 + 5);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  return rv;
}



bcm_error_t config_l3_trunk_ecmp_rtag7(int unit)
{

  int flags;
  bcm_error_t rv = BCM_E_NONE;
  bcm_vlan_t vid_in = 12;
  bcm_gport_t gport_in;

  bcm_vlan_t egr_vid_1 = 13;
  bcm_gport_t egr_gport_1_1, egr_gport_1_2, egr_gport_1_3;
  bcm_gport_t egr_trunk_gp_1;
  bcm_trunk_t trunk_id_1 = 1;
  bcm_if_t egr_l3_if_1;
  bcm_if_t egr_obj_1;

  bcm_vlan_t egr_vid_2 = 14;
  bcm_gport_t egr_gport_2_1, egr_gport_2_2, egr_gport_2_3;
  bcm_gport_t egr_trunk_gp_2;
  bcm_trunk_t trunk_id_2 = 2;
  bcm_if_t egr_l3_if_2;
  bcm_if_t egr_obj_2;

  bcm_port_t trunk_ports[3];

  bcm_l3_route_t route_info;
  bcm_l2_addr_t l2addr;

  bcm_mac_t egr_nxt_hop_1  = {0x00,0x00,0x00,0x00,0x11,0x11};
  bcm_mac_t egr_router_mac_1  = {0x00,0x00,0x00,0x00,0xBB,0x1B};

  bcm_mac_t egr_nxt_hop_2  = {0x00,0x00,0x00,0x00,0x11,0x21};
  bcm_mac_t egr_router_mac_2  = {0x00,0x00,0x00,0x00,0xBB,0x2B};

  bcm_mac_t nxt_hop_in  = {0x00,0x00,0x00,0x00,0x22,0x22};
  bcm_mac_t router_mac_in  = {0x00,0x00,0x00,0x00,0xAA,0xAA};

  bcm_ip_t dip = 0xC0A80A01;  /* 192.168.10.1 */
  bcm_ip_t mask = 0xffffff00;  /* 255.255.255.0 */

  rv= bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_switch_control_set(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= global_rtag7_configure(unit);
  if (BCM_FAILURE(rv)) {
    printf("Error executing global_rtag7_configure(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= trunk_rtag7_config(unit);
  if (BCM_FAILURE(rv)) {
    printf("Error executing trunk_rtag7_config(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= ecmp_rtag7_config(unit);
  if (BCM_FAILURE(rv)) {
    printf("Error executing ecmp_rtag7_config(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_port_gport_get(unit, Ingress_port, &gport_in);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_port_gport_get(unit, Egress_port_1_1, &egr_gport_1_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_port_gport_get(unit, Egress_port_1_2, &egr_gport_1_2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_port_gport_get(unit, Egress_port_1_3, &egr_gport_1_3);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_port_gport_get(unit, Egress_port_2_1, &egr_gport_2_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_port_gport_get(unit, Egress_port_2_2, &egr_gport_2_2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= bcm_port_gport_get(unit, Egress_port_2_3, &egr_gport_2_3);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_port_gport_get(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* 
   * Ingress side of the VLAN
   */ 
  rv=vlan_create_add_port(unit, vid_in, Ingress_port);
  if (BCM_FAILURE(rv)) {
    printf("Error executing vlan_create_add_port(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* Enable ingress L3 lookup */
  rv= config_router_mac(unit, router_mac_in, vid_in);
  if (BCM_FAILURE(rv)) {
    printf("Error executing config_router_mac(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /*
   * Egress side of the VLAN
   */
  /* Create trunk */
  trunk_ports[0] = Egress_port_1_1;
  trunk_ports[1] = Egress_port_1_2;
  trunk_ports[2] = Egress_port_1_3;

  rv= vlan_create_add_ports(unit, egr_vid_1, 3, &trunk_ports);
  if (BCM_FAILURE(rv)) {
    printf("Error executing vlan_create_add_ports(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= create_trunk(unit, &trunk_id_1, 3, &trunk_ports);
  if (BCM_FAILURE(rv)) {
    printf("Error executing create_trunk(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  BCM_GPORT_TRUNK_SET(egr_trunk_gp_1, trunk_id_1);
  print egr_trunk_gp_1;

  /* Create trunk */
  trunk_ports[0] = Egress_port_2_1;
  trunk_ports[1] = Egress_port_2_2;
  trunk_ports[2] = Egress_port_2_3;

  rv= vlan_create_add_ports(unit, egr_vid_2, 3, &trunk_ports);
  if (BCM_FAILURE(rv)) {
    printf("Error executing vlan_create_add_ports(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= create_trunk(unit, &trunk_id_2, 3, &trunk_ports);
  if (BCM_FAILURE(rv)) {
    printf("Error executing create_trunk(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  BCM_GPORT_TRUNK_SET(egr_trunk_gp_2, trunk_id_2);
  print egr_trunk_gp_2;

  /* 
   * Create egress L3 interface
   */
  rv=create_l3_interface(unit, egr_router_mac_1, egr_vid_1, &egr_l3_if_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing create_l3_interface(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv=create_l3_interface(unit, egr_router_mac_2, egr_vid_2, &egr_l3_if_2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing create_l3_interface(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* 
   * Egress side of the next hop object
   */
  rv= create_egr_obj(unit, egr_l3_if_1, egr_nxt_hop_1, egr_trunk_gp_1, &egr_obj_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing create_egr_obj(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv= create_egr_obj(unit, egr_l3_if_2, egr_nxt_hop_2, egr_trunk_gp_2, &egr_obj_2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing create_egr_obj(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  print egr_obj_1;
  print egr_obj_2;

  /* ECMP group */
  bcm_if_t ecmp_group[2];
  bcm_if_t ecmp_group_id;
  ecmp_group[0] = egr_obj_1;
  ecmp_group[1] = egr_obj_2;

  rv= bcm_l3_egress_multipath_create(unit, 0, 2, ecmp_group, &ecmp_group_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_egress_multipath_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  /* 
   * Install the route for DIP/mask
   */
  bcm_l3_route_t_init(&route_info);
  route_info.l3a_flags = BCM_L3_MULTIPATH;
  route_info.l3a_intf = ecmp_group_id;
  route_info.l3a_subnet = dip;
  route_info.l3a_ip_mask = mask;

  rv= bcm_l3_route_add(unit, &route_info);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
    return rv;
  }


  return BCM_E_NONE;
}


void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");

  /*
   * For checking RTAG7 setting on the trunk and ECMP. send the below packet on ingress port. 
   * 
   *       SAMPLE-1
   *       =========
   *       DA 0x00000000AAAA
   *       SA 0x000000002222
   *       VLAN 12
   *       DIP=192.168.10.1 
   *       SIP =10.10.10.1
   *
   *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C
   *       08 00 45 00 00 2E 00 00 00 00 40 FF 9B 1D 0A 0A 
   *       0A 01 C0 A8 0A 01 43 03 1F E5 B2 9E CF 90 C1 81 
   *       3A 56 99 BD 78 02 A6 9E FB 49 9E 95 BA 5A BF 76 
   *       C3 48 74 7B 
   *
   *
   *
   *       SAMPLE-2
   *       =========
   *       DA 0x00000000AAAA
   *       SA 0x000000002222
   *       VLAN 12
   *       DIP=192.168.10.24 
   *       SIP =10.10.10.24
   *
   *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
   *       08 00 45 00 00 2E 00 00 00 00 40 FF 9A EF 0A 0A 
   *       0A 18 C0 A8 0A 18 3A 10 1D 7B 40 01 84 D9 B1 55 
   *       D1 50 6C FE 82 7B 76 83 4F 3C F7 F6 55 50 85 26 
   *       FA 5E 7E 81 
   *
   * 
   *       SAMPLE-3
   *       =========
   *       DA 0x00000000AAAA
   *       SA 0x000000002222
   *       VLAN 12
   *       DIP=192.168.10.33 
   *       SIP =10.10.10.33
   *
   *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C
   *       08 00 45 00 00 2E 00 00 00 00 40 FF 9A DD 0A 0A 
   *       0A 21 C0 A8 0A 21 C4 65 87 43 14 45 B9 76 63 DB 
   *       AB C3 4F DF 83 CD BC 36 E5 B5 1A 20 5D EA E7 B3 
   *       B8 B4 B1 75 
   *
   *
   *       SAMPLE-4
   *       =========
   *       DA 0x00000000AAAA
   *       SA 0x000000002222
   *       VLAN 12
   *       DIP=192.168.10.51 
   *       SIP =10.10.10.51
   *
   *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
   *       08 00 45 00 00 2E 00 00 00 00 40 FF 9A B9 0A 0A 
   *       0A 33 C0 A8 0A 33 B8 94 D7 BC 72 CE 32 C9 2D 38 
   *       60 3C 1D C9 CA E9 9C 03 3B 7C 6F 25 44 54 4B A1 
   *       32 1E 69 5D 
   *
   *
   *
   *       SAMPLE-5
   *       =========
   *       DA 0x00000000AAAA
   *       SA 0x000000002222
   *       VLAN 12
   *       DIP=192.168.10.66 
   *       SIP =10.10.10.66
   *       
   *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
   *       08 00 45 00 00 2E 00 00 00 00 40 FF 9A 9B 0A 0A 
   *       0A 42 C0 A8 0A 42 57 62 6D F6 D1 8E 57 2A 98 49 
   *       37 9B 54 2A 94 C3 64 A5 65 C9 22 3A 79 55 68 EC 
   *       CD 72 66 8F 
   *
   *
   *       SAMPLE-6
   *       =========
   *       DA 0x00000000AAAA
   *       SA 0x000000002222
   *       VLAN 12
   *       DIP=192.168.10.87 
   *       SIP =10.10.10.87
   *
   *       00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C 
   *       08 00 45 00 00 2E 00 00 00 00 40 FF 9A 71 0A 0A 
   *       0A 57 C0 A8 0A 57 EE E4 EF B9 8A BC A1 34 A4 08 
   *       32 69 93 D1 52 D3 34 92 63 E0 8C 4B 38 A5 AF D1 
   *       E5 1F A4 DC 
   * 
   * NOTE: Use above traffic to get loadbalance on all egress port and 
   *       show c out after each packet shows every egress port getting 
   *       one packet each 
   */


  bshell(unit, "clear c"); 

  printf("\nSending packet to Ingress_port:%d\n", Ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500002E0000000040FF9B1D0A0A0A01C0A80A0143031FE5B29ECF90C1813A5699BD7802A69EFB499E95BA5ABF76C348747B; sleep quiet 1", Ingress_port);
  bshell(unit, str);
  bshell(unit, "show c");
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500002E0000000040FF9AEF0A0A0A18C0A80A183A101D7B400184D9B155D1506CFE827B76834F3CF7F655508526FA5E7E81; sleep quiet 1", Ingress_port);
  bshell(unit, str);
  bshell(unit, "show c");
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500002E0000000040FF9ADD0A0A0A21C0A80A21C46587431445B97663DBABC34FDF83CDBC36E5B51A205DEAE7B3B8B4B175; sleep quiet 1", Ingress_port);
  bshell(unit, str);
  bshell(unit, "show c");
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500002E0000000040FF9AB90A0A0A33C0A80A33B894D7BC72CE32C92D38603C1DC9CAE99C033B7C6F2544544BA1321E695D; sleep quiet 1", Ingress_port);
  bshell(unit, str);
  bshell(unit, "show c");
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500002E0000000040FF9A9B0A0A0A42C0A80A4257626DF6D18E572A9849379B542A94C364A565C9223A795568ECCD72668F; sleep quiet 1", Ingress_port);
  bshell(unit, str);
  bshell(unit, "show c");
  snprintf(str, 512, "tx 1 pbm=%d data=0x00000000AAAA0000000022228100000C08004500002E0000000040FF9A710A0A0A57C0A80A57EEE4EFB98ABCA134A408326993D152D3349263E08C4B38A5AFD1E51FA4DC; sleep quiet 1", Ingress_port);
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

  if (BCM_FAILURE((rv = config_l3_trunk_ecmp_rtag7(unit)))) {
    printf("config_ecmp_rtag7() failed.\n");
    return -1;
  }

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

