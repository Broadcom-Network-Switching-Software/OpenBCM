/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : L3 FLEX DIGEST ECMP Trunk
 *
 *  Usage    : BCM.0> cint l3_flexdigest_trunk.c
 *
 *  config   : bcm56880_a0-generic.config.yml
 *
 *  Log file : l3_flexdigest_trunk_log.txt
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
 *    flex digest is used as hashing for ECMP and trunking.
 *    this example demonstrates how to configure the flex digest to support both
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
 *    2)Step2 - Configuration (Done in config_l3_trunk_ecmp_flexdigest())
 *    ===================================================================
 *    a) Create the trunk using Three port and Configure flexdigest setting on the trunk. 
 *       Here we use three physical ports each to configure two trunks.
 *       Which in turn configured as two ECMP paths. Flexdigest is used as hashing for ECMP 
 *       and trunking.
 *       This is done in config_l3_trunk_ecmp_flexdigest()
 *       
 *    3)Step3 - Verification (Done in verify())
 *    =======================================
 *    a) For checking Flex digest setting on the trunk and ECMP. send the below packet on 
 *       ingress port. 
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
 */


cint_reset();
bcm_port_t      Ingress_port = 1;
bcm_port_t      Egress_port_1_1 = 2,Egress_port_1_2 = 3,Egress_port_1_3 = 4;
bcm_port_t      Egress_port_2_1 = 5,Egress_port_2_2 = 6,Egress_port_2_3 = 7;
bcm_field_group_t group = -1;
bcm_vrf_t vrf = 1;
bcm_ip_t def_subnet = 0x0;
bcm_ip_t def_mask = 0x0;

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
  /*BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

  if (group == -1)
  {
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, 0, &group));
  }

  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));*/
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
  /*BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

  if (group == -1)
  {
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_IF_ERROR_RETURN(bcm_field_group_create(unit, qset, 0, &group));
  }
  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group, &entry));

  BCM_IF_ERROR_RETURN(bcm_field_qualify_InPort(unit, entry, port, BCM_FIELD_EXACT_MATCH_MASK));
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));*/

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


bcm_error_t 
create_flexdigest(int unit)
{
bcm_error_t rv = BCM_E_NONE;
/*###########################*/
/* FLEX DIGEST CONFIGURATION */
/*###########################*/
int seed_profile_id, norm_profile_id, hash_profile_id;
int norm_enable;
int seed;
int debug = 0;

print bcm_flexdigest_init(unit);

/**** NORMALIZATION and SEED SETUP ****/
seed_profile_id = 1;
 rv =  bcm_flexdigest_norm_seed_profile_create(unit, BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &seed_profile_id);
   if (BCM_FAILURE(rv)) {
    printf("Error executing BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID:1 %s.\n", bcm_errmsg(rv));
    return rv;
   }
seed = 0xbabaface;
rv = bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id, bcmFlexDigestNormSeedControlBinASeed, seed);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestNormSeedControlBinASeed %s.\n", bcm_errmsg(rv));
    return rv;
  }
seed = 0xa5a5a5a5;
rv =  bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id, bcmFlexDigestNormSeedControlBinBSeed, seed);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestNormSeedControlBinBSeed %s.\n", bcm_errmsg(rv));
    return rv;
  }
seed = 0x5a5a5a5a;
rv =  bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id, bcmFlexDigestNormSeedControlBinCSeed, seed);
  if (BCM_FAILURE(rv)) {
    printf("Error bcmFlexDigestNormSeedControlBinCSeed %s.\n", bcm_errmsg(rv));
    return rv;
  }

norm_profile_id = 0;
rv = bcm_flexdigest_norm_profile_create(unit, BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &norm_profile_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID:0 %s.\n", bcm_errmsg(rv));
    return rv;
  }
norm_enable = TRUE;
rv =  bcm_flexdigest_norm_profile_set(unit, norm_profile_id, bcmFlexDigestNormProfileControlNorm, norm_enable);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestNormProfileControlNorm %s.\n", bcm_errmsg(rv));
    return rv;
  }

rv = bcm_flexdigest_norm_profile_set(unit, norm_profile_id, bcmFlexDigestNormProfileControlSeedId, seed_profile_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestNormProfileControlNorm %s.\n", bcm_errmsg(rv));
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
rv =  bcm_flexdigest_group_create(unit, qset, pri, &group);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_group_create %s.\n", bcm_errmsg(rv));
    return rv;
  }

rv =  bcm_flexdigest_entry_create(unit, group, &entry);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_entry_create %s.\n", bcm_errmsg(rv));
    return rv;
  }

rv = bcm_flexdigest_match_id_create(unit, &match_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_match_id_create %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL2HdrL2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestMatchOuterL2HdrL2 %s.\n", bcm_errmsg(rv));
    return rv;
  }

/* Qualifiers */
rv = bcm_flexdigest_qualify_MatchId(unit, entry, match_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_qualify_MatchId %s.\n", bcm_errmsg(rv));
    return rv;
  }

/* Actions : Bin field population */
/* Set B */
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds0, BCM_FLEXDIGEST_FIELD_INGRESS_PP_PORT, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds0 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds1, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds1 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds2, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds2 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds3, BCM_FLEXDIGEST_FIELD_OUTER_MACDA_32_47, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds3 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds4, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds4 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds5, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds5 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds6, BCM_FLEXDIGEST_FIELD_OUTER_MACSA_32_47, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds6 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds7, BCM_FLEXDIGEST_FIELD_OUTER_OTAG_0_15, mask_2);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds7 %s.\n", bcm_errmsg(rv));
    return rv;
  }

rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds14, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds14 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds15, BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_C_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds15 %s.\n", bcm_errmsg(rv));
    return rv;
  }

rv = bcm_flexdigest_entry_install(unit, entry);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_entry_install %s.\n", bcm_errmsg(rv));
    return rv;
  }

/*** GROUP 1 (Pri = 1) for IPV4 and IPv4 UDP packets ***/
pri = 1;
rv =  bcm_flexdigest_group_create(unit, qset, pri, &group);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_group_create %s.\n", bcm_errmsg(rv));
    return rv;
  }

/** Entry 0 : IPv4 UDP **/
rv = bcm_flexdigest_entry_create(unit, group, &entry);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_entry_create %s.\n", bcm_errmsg(rv));
    return rv;
  }

rv =  bcm_flexdigest_match_id_create(unit, &match_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_match_id_create %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL3L4HdrUdp);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_match_add:bcmFlexDigestMatchOuterL3L4HdrUdp %s.\n", bcm_errmsg(rv));
    return rv;
  }

/* Qualifiers */
rv = bcm_flexdigest_qualify_MatchId(unit, entry, match_id);
if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_qualify_MatchId %s.\n", bcm_errmsg(rv));
    return rv;
  }

/* Actions : Bin field population */
/* Set A */
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds8, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetACmds8 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds9, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetACmds9 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds10, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetACmds10 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds11, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetACmds11 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds12, BCM_FLEXDIGEST_FIELD_OUTER_L4_SRC_PORT, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_action_add %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetACmds13, BCM_FLEXDIGEST_FIELD_OUTER_L4_DST_PORT, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_action_add %s.\n", bcm_errmsg(rv));
    return rv;
  }

/* Set B */
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds8, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_action_add %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds9, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_action_add %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds10, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_action_add %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds11, BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_action_add %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds12, BCM_FLEXDIGEST_FIELD_OUTER_L4_SRC_PORT, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_action_add %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds13, BCM_FLEXDIGEST_FIELD_OUTER_L4_DST_PORT, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_action_add %s.\n", bcm_errmsg(rv));
    return rv;
  }

rv = bcm_flexdigest_entry_install(unit, entry);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_entry_install %s.\n", bcm_errmsg(rv));
    return rv;
  }

/** Entry 1 : IPv4 Unknown Protocol Type **/
rv =  bcm_flexdigest_entry_create(unit, group, &entry);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_entry_create %s.\n", bcm_errmsg(rv));
    return rv;
  }

rv = bcm_flexdigest_match_id_create(unit, &match_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_match_id_create %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_match_add(unit, match_id, bcmFlexDigestMatchOuterL3L4HdrUnknownL4);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestMatchOuterL3L4HdrUnknownL4 %s.\n", bcm_errmsg(rv));
    return rv;
  }

/* Qualifiers */
rv =  bcm_flexdigest_qualify_MatchId(unit, entry, match_id);
if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_qualify_MatchId %s.\n", bcm_errmsg(rv));
    return rv;
  }

/* Actions : Bin field population */
/* Set B */
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds8,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds8 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds9,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds9 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds10,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds10 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds11,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds11 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds12,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_TTL, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds12 %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_action_add(unit, entry, bcmFlexDigestActionExtractBinSetBCmds13,  BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_PROTOCOL, mask_1);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestActionExtractBinSetBCmds13 %s.\n", bcm_errmsg(rv));
    return rv;
  }

rv = bcm_flexdigest_entry_install(unit, entry);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_hash_profile_create %s.\n", bcm_errmsg(rv));
    return rv;
  }

/*** HASH SETUP ***/
hash_profile_id = 0;
rv = bcm_flexdigest_hash_profile_create(unit,
    BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &hash_profile_id);
      if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_flexdigest_qualify_MatchId %s.\n", bcm_errmsg(rv));
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

rv =  bcm_flexdigest_hash_salt_set(unit, bcmFlexDigestHashBinB, sizeof(salt)/sizeof(salt[0]), salt);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestHashBinB %s.\n", bcm_errmsg(rv));
    return rv;
  }

int enable = 1;
rv = bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlPreProcessBinB, enable);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestHashProfileControlPreProcessBinB %s.\n", bcm_errmsg(rv));
    return rv;
  }

rv =  bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlXorSaltBinB, enable);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestHashProfileControlXorSaltBinB %s.\n", bcm_errmsg(rv));
    return rv;
  }

rv =  bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlBinB0FunctionSelection, BCM_FLEXDIGEST_HASH_FUNCTION_CRC32HI);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestHashProfileControlBinB0FunctionSelection %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_flexdigest_hash_profile_set(unit, hash_profile_id, bcmFlexDigestHashProfileControlBinB1FunctionSelection, BCM_FLEXDIGEST_HASH_FUNCTION_CRC32_ETH_LO);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmFlexDigestHashProfileControlBinB1FunctionSelection %s.\n", bcm_errmsg(rv));
    return rv;
  }

/**** HASH BITS SELECTION SETUP ****/
int ecmp_flow_select = 5;   /*Use flow based with macro selection block : B0 Low*/
int ecmp_concat_mode = 0;   /*Don't use concat mode */
int ecmp_min_offset = 32;   /*from B0 Offset 0 */
int ecmp_max_offset = 63;   /*to B1 Offset 15 */
int ecmp_offset_stride = 1; /*Increment by 1*/


rv = bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelEcmpOverlay, ecmp_flow_select);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmSwitchHashUseFlowSelEcmpOverlay %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_switch_control_set(unit, bcmSwitchMacroFlowEcmpHashOverlayConcatEnable, ecmp_concat_mode);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmSwitchMacroFlowEcmpHashOverlayConcatEnable %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv =  bcm_switch_control_set(unit, bcmSwitchMacroFlowHashOverlayMinOffset, ecmp_min_offset);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmSwitchMacroFlowHashOverlayMinOffset %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_switch_control_set(unit, bcmSwitchMacroFlowHashOverlayMaxOffset, ecmp_max_offset);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmSwitchMacroFlowHashOverlayMaxOffset %s.\n", bcm_errmsg(rv));
    return rv;
  }
rv = bcm_switch_control_set(unit, bcmSwitchMacroFlowHashOverlayStrideOffset, ecmp_offset_stride);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcmSwitchMacroFlowHashOverlayStrideOffset %s.\n", bcm_errmsg(rv));
    return rv;
  }
  /**** HASH BITS SELECTION SETUP ****/
    printf("Trunk Unicast and Non-unicast Hash bits selection setup:\n");
    int trunk_uc_flow_select = 0;   /*Use Port based hash */
    int trunk_uc_offset = 33;        /*A0 block with Offset 1*/

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelTrunkUc, trunk_uc_flow_select));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchTrunkUnicastHashOffset, trunk_uc_offset));

    int trunk_nonuc_flow_select = 0;    /*Use Port based hash */
    int trunk_nonuc_offset = 33;        /*C1 block with Offset 1*/

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchHashUseFlowSelTrunkNonUnicast, trunk_nonuc_flow_select));
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchTrunkNonUnicastHashOffset, trunk_nonuc_offset));

return BCM_E_NONE;
}


bcm_error_t config_l3_trunk_ecmp_flexdigest(int unit)
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
  bcm_vlan_control_vlan_t vlan_ctrl;

/************/
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
  /*****To change as separate API */
     /**** L3 INGRESS INTERFACE (L3_IIF) CREATE *****/
    /*NOTE : This is optional on older XGS devices, but mandatory on TD4, L3 flow won't work*/
    bcm_l3_ingress_t l3_ingress;
    bcm_l3_intf_t l3_intf;
    bcm_if_t ingress_if;
    bcm_l3_ingress_t_init(&l3_ingress);
    l3_ingress.vrf = vrf;
    rv= bcm_l3_ingress_create(unit, &l3_ingress, &ingress_if);
    if (BCM_FAILURE(rv)) {
    printf("Error executing create_l3_interface_l3_iif:bcm_l3_ingress_create %s.\n", bcm_errmsg(rv));
    return rv;
   }

    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    bcm_vlan_control_vlan_get(unit, vid_in, &vlan_ctrl);
    vlan_ctrl.ingress_if = ingress_if;
    rv= bcm_vlan_control_vlan_set(unit, vid_in, vlan_ctrl);
    if (BCM_FAILURE(rv)) {
    printf("Error executing create_l3_interface_l3_iif:bcm_vlan_control_vlan_set %s.\n", bcm_errmsg(rv));
    return rv;
   } 
   rv = create_flexdigest(unit);
    if (BCM_FAILURE(rv)) {
    printf("Error executing create_flexdigest(): %s.\n", bcm_errmsg(rv));
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
     bcm_if_t ecmp_group_id;
     bcm_l3_egress_ecmp_t ecmp_group; 
    /***** ECMP OBJECT CREATE *****/
    bcm_l3_ecmp_member_t ecmp_member_array[2];
    bcm_l3_ecmp_member_t_init(&ecmp_member_array);
    ecmp_member_array[0].egress_if =egr_obj_1;
    ecmp_member_array[1].egress_if = egr_obj_2;
    

 rv= bcm_l3_ecmp_create(unit, 0, &ecmp_group, 2, ecmp_member_array);
   if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_ecmp_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
ecmp_group_id = ecmp_group.ecmp_intf;

/* ALPM is default mode and default-route should always be first route */
 bcm_l3_route_t_init(& route_info);
 route_info.l3a_flags = 0;
 route_info.l3a_subnet = def_subnet;
 route_info.l3a_ip_mask = def_mask;
 route_info.l3a_flags = BCM_L3_MULTIPATH;
 route_info.l3a_intf = ecmp_group_id; /* using same next-hop interface */
 route_info.l3a_vrf = vrf;
 rv = bcm_l3_route_add(unit, & route_info);
 if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_route_add(): %s.\n", bcm_errmsg(rv));
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
  route_info.l3a_vrf=vrf;

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
   * For checking flex digest setting on the trunk and ECMP. send the below packet on ingress port. 
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
   * NOTE: Use above traffic to get loadbalance on egress port and 
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

  if (BCM_FAILURE((rv = config_l3_trunk_ecmp_flexdigest(unit)))) {
    printf(" config_l3_trunk_ecmp_flexdigest() failed.\n");
    return -1;
  }

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}


