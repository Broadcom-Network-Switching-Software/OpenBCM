/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Packet trace
 *
 *  Usage    : BCM.0> cint ptrace_lag_ecmp.c
 *
 *  config   : pkt_trace_config.bcm
 *
 *  Log file : ptrace_lag_ecmp_log.txt
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
 *  Summary:
 *  ========
 *    This CINT example shows how to trace a packet using BCM APIs.
 *
 *    The BCM API to trace a packet is bcm_switch_pkt_trace_info_get().
 *    This API returns the captured traced information. This example
 *    parses the traced information, maps returned values to corresponding
 *    software objects, and prints the traced information in readable form.
 *
 *    The packet trace results are determined by how the switch processes the
 *    packet specified in the API call. As such, for a desired behavior,
 *    the switch should be configured how to forward the packet before
 *    tracing the packet. In this example, L3 routing flow with destination
 *    as ecmp and trunk is configured and the packet is sent to match the
 *    configured L3 routing flow.
 *
 *    Note: 
 *    =====
 *    Visibility cannot be used with IPEP clock gating enabled.Use the below
 *    config variable.
 *      ipep_clock_gating_disable=1   
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Select one ingress and six egress ports and configure them in 
 *         Loopback mode.
 *
 *      b) Configure RTAG7 settings for ECMP and trunk application.
 *
 *      c) Create two trunks each having three members.
 *
 *      d) Create an ecmp groups with two ecmp members where each ecmp member
 *         uses one of the two trunks created in step 1.c.
 *
 *    2) Step2 - Configuration (None):
 *    ================================
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Transmit the below trace packet on ingress_port.
 *         Packet:
 *         =======
 *         DA 0x00000000AAAA
 *         SA 0x000000002222
 *         VLAN 12
 *         DIP=192.168.10.33
 *         SIP =10.10.10.33
 *
 *         00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C
 *         08 00 45 00 00 2E 00 00 00 00 40 FF 9A DD 0A 0A
 *         0A 21 C0 A8 0A 21 C4 65 87 43 14 45 B9 76 63 DB
 *         AB C3 4F DF 83 CD BC 36 E5 B5 1A 20 5D EA E7 B3
 *         B8 B4 B1 75
 *      b) Expected Result:
 *         ================
 *         Verify that lookup status results, packet resolution result, ecmp resolution result
 *         and trunk resolution result are displayed.
 */
cint_reset();
bcm_port_t      ingress_port = 1;
bcm_port_t      egress_port_1_1 = 2,egress_port_1_2 = 3,egress_port_1_3 = 4;
bcm_port_t      egress_port_2_1 = 5,egress_port_2_2 = 6,egress_port_2_3 = 7;
/*
 * Given a GPORT, get the GPORT readable name. Not all possible
 * GPORT names are supported.
 *
 * The name will have up to str_size characters - 1
 */
char *
*gport_to_str(bcm_gport_t gp, char *str, int str_size)
{
    if (str == NULL) return ("NULL");

    if (BCM_GPORT_IS_SET(gp)) {
        if (BCM_GPORT_IS_LOCAL(gp))
            snprintf(str, str_size, "local(%d)", BCM_GPORT_LOCAL_GET(gp));
        else if (BCM_GPORT_IS_MODPORT(gp))
            snprintf(str, str_size, "modport(%d, %d)",
                     BCM_GPORT_MODPORT_MODID_GET(gp),
                     BCM_GPORT_MODPORT_PORT_GET(gp));
        else if (BCM_GPORT_IS_TRUNK(gp))
            snprintf(str, str_size, "trunk(%d)", BCM_GPORT_TRUNK_GET(gp));
        else if (BCM_GPORT_IS_BLACK_HOLE(gp))
            snprintf(str, str_size, "blackhole");
        else if (BCM_GPORT_IS_LOCAL_CPU(gp))
            snprintf(str, str_size, "cpu");
        else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gp))
            snprintf(str, str_size, "ucast(%d, %d)",
                     BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gp),
                     BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gp));
        else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gp))
            snprintf(str, str_size, "ucast_sub(%d)",
                     BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gp));
        else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gp))
            snprintf(str, str_size, "mcast(%d, %d)",
                     BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(gp),
                     BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gp));
        else if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gp))
            snprintf(str, str_size, "mcast_sub(%d)",
                     BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gp));
        else if (BCM_GPORT_IS_SUBPORT_PORT(gp))
            snprintf(str, str_size, "subport(%d)", BCM_GPORT_SUBPORT_PORT_GET(gp));
        else if (BCM_GPORT_IS_SUBPORT_GROUP(gp))
            snprintf(str, str_size, "subport_g(%d)", BCM_GPORT_SUBPORT_GROUP_GET(gp));
        else if (BCM_GPORT_IS_MPLS_PORT(gp))
            snprintf(str, str_size, "mpls(%d)", BCM_GPORT_MPLS_PORT_ID_GET(gp));
        else if (BCM_GPORT_IS_MIM_PORT(gp))
            snprintf(str, str_size, "mim(%d)", BCM_GPORT_MIM_PORT_ID_GET(gp));
        else if (BCM_GPORT_IS_WLAN_PORT(gp))
            snprintf(str, str_size, "wlan(%d)", BCM_GPORT_WLAN_PORT_ID_GET(gp));
        else if (BCM_GPORT_IS_VLAN_PORT(gp))
            snprintf(str, str_size, "vlan(%d)", BCM_GPORT_VLAN_PORT_ID_GET(gp));
        else if (BCM_GPORT_IS_TRILL_PORT(gp))
            snprintf(str, str_size, "trill(%d)", BCM_GPORT_TRILL_PORT_ID_GET(gp));
        else if (BCM_GPORT_IS_NIV_PORT(gp))
            snprintf(str, str_size, "niv(%d)", BCM_GPORT_NIV_PORT_ID_GET(gp));
        else if (BCM_GPORT_IS_MIRROR(gp))
            snprintf(str, str_size, "mirror(%d)", BCM_GPORT_MIRROR_GET(gp));
        else if (BCM_GPORT_IS_SCHEDULER(gp))
            snprintf(str, str_size, "scheduler(%d)", BCM_GPORT_SCHEDULER_GET(gp));
        else if (BCM_GPORT_IS_DEVPORT(gp))
            snprintf(str, str_size, "devport(%d, %d)",
                     BCM_GPORT_DEVPORT_DEVID_GET(gp),
                     BCM_GPORT_DEVPORT_PORT_GET(gp));
        else if (BCM_GPORT_IS_TUNNEL(gp))
            snprintf(str, str_size, "tunnel(%d)", BCM_GPORT_TUNNEL_ID_GET(gp));
        else
            snprintf(str, str_size, "UNKNOWN TYPE(%d)", (gp >> 26) & 0x3f);
    } else {
        if (gp == BCM_GPORT_INVALID) snprintf(str, str_size, "Invalid port");
        else snprintf(str, str_size, "port(%d)", gp);
    }

    return str;
}



/*
 * Given a GPORT, print its name.
 */
void
gport_print(bcm_port_t gp)
{
    char gp_name[100];

    printf("%s", gport_to_str(gp, gp_name, 100));
}


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
 * Configures the port in loopback mode.
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  return BCM_E_NONE;
}

/* 
 * Configures the port in loopback mode.
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));

  return BCM_E_NONE;
}

/* This function creates vlan and add port to vlan */
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

/* This function creates vlan and add array of ports to vlan */
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
}

/* This function creates L3 interface */
bcm_error_t create_l3_interface(int unit, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
  bcm_l3_intf_t l3_intf;
  bcm_error_t rv = BCM_E_NONE;

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

/* This function creates L3 egress object */
bcm_error_t create_egr_obj(int unit, int l3_if, bcm_mac_t nh_mac, bcm_gport_t gport,
    bcm_if_t *egr_obj_id)
{
  bcm_l3_egress_t l3_egress;
  bcm_error_t rv = BCM_E_NONE;

  bcm_l3_egress_t_init(&l3_egress);
  sal_memcpy(l3_egress.mac_addr,  nh_mac, sizeof(nh_mac));
  l3_egress.intf  = l3_if;
  l3_egress.port = gport;
  rv= bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_l3_egress_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }
  return rv;
}

/* This function adds router mac */
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
  if (BCM_FAILURE(rv)) {
    printf("Error executing bcm_trunk_create(): %s.\n", bcm_errmsg(rv));
    return rv;
  }

  rv = bcm_trunk_set(unit, *tid, &trunk_info, count, member_info);
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

  bcm_vlan_t egr_vid_1 = 13;
  bcm_gport_t egr_trunk_gp_1;
  bcm_trunk_t trunk_id_1 = 1;
  bcm_if_t egr_l3_if_1;
  bcm_if_t egr_obj_1;

  bcm_vlan_t egr_vid_2 = 14;
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

  /* 
   * Ingress side of the VLAN
   */ 
  rv=vlan_create_add_port(unit, vid_in, ingress_port);
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
  trunk_ports[0] = egress_port_1_1;
  trunk_ports[1] = egress_port_1_2;
  trunk_ports[2] = egress_port_1_3;

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

  /* Create trunk */
  trunk_ports[0] = egress_port_2_1;
  trunk_ports[1] = egress_port_2_2;
  trunk_ports[2] = egress_port_2_3;

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

/* Default configuration */
struct ptrace_cfg_t {
    uint32      options;        /* BCM_PKT_TRACE_XXX: LEARN, NO_IFP, FORWARD */
    uint8      *pkt_data;       /* pointer to packet */
    uint32      pkt_len;        /* length of the packet */
    bcm_port_t  src_port;       /* front panel masquerade source port */
};

/*       DA 0x00000000AAAA$
 *       SA 0x000000002222$
 *       VLAN 12$
 *       DIP=192.168.10.33 $
 *       SIP =10.10.10.33$
*/

uint8 pkt_tagged_l4[] = {
  0x00,0x00,0x00,0x00,0xAA,0xAA,0x00,0x00,0x00,0x00,0x22,0x22,0x81,0x00,0x00,0x0C,
  0x08,0x00,0x45,0x00,0x00,0x2E,0x00,0x00,0x00,0x00,0x40,0xFF,0x9A,0xDD,0x0A,0x0A,
  0x0A,0x21,0xC0,0xA8,0x0A,0x21,0xC4,0x65,0x87,0x43,0x14,0x45,0xB9,0x76,0x63,0xDB,
  0xAB,0xC3,0x4F,0xDF,0x83,0xCD,0xBC,0x36,0xE5,0xB5,0x1A,0x20,0x5D,0xEA,0xE7,0xB3,
  0xB8,0xB4,0xB1,0x75
};

/* Change default configuration here */
ptrace_cfg_t ptrace_cfg = {
    BCM_PKT_TRACE_LEARN | BCM_PKT_TRACE_FORWARD,
    pkt_tagged_l4,
    sizeof(pkt_tagged_l4),
    ingress_port
};

/* Map resolution enums to readable names */
struct ptrace_abs_resolution_t {
        bcm_switch_pkt_trace_resolution_t resolution;
        char *name;
};

/* Map lookup results enums to readable names */
struct ptrace_abs_lookup_results_t {
    bcm_switch_pkt_trace_lookup_t bit;
    char *name;
};

int
ptrace(int unit, ptrace_cfg_t *cfg)
{
	int rv;
	bcm_switch_pkt_trace_info_t pkt_trace_info;

    /*
     * BCM API to trace a packet
     *    options       BCM_PKT_TRACE_XXX (LEARN, NO_IFP, FORWARD)
     *    src_port      the front panel source port
     *    pkt_len       length of the packet data
     *    pkt_data      pointer to the packet data
     *    pkt_trace_info buffer to store the trace results
     */
    rv = bcm_switch_pkt_trace_info_get(unit, cfg->options, cfg->src_port,
                                       cfg->pkt_len, cfg->pkt_data,
                                       &pkt_trace_info);
    if (BCM_SUCCESS(rv)) {
        printf("Displaying trace results for packet sent on masquerade source port: %d\n", cfg->src_port);
        rv = ptrace_info_decode(unit, &pkt_trace_info);
    }

    return rv;
}

/*
 *  Print information for the traced packet
 */
int
ptrace_info_decode(int unit, bcm_switch_pkt_trace_info_t *pkt_trace_info)

{
    printf("Ingress pipeline: %d\n", pkt_trace_info->dest_pipe_num);
    BCM_IF_ERROR_RETURN(ptrace_abs_info_print(unit, pkt_trace_info));


    return BCM_E_NONE;
}

int
ptrace_abs_info_print(int unit, bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    BCM_IF_ERROR_RETURN(
        ptrace_abs_lookup_results_print(unit, pkt_trace_info->pkt_trace_lookup_status));
    BCM_IF_ERROR_RETURN(
        ptrace_abs_resolution_print(unit, pkt_trace_info->pkt_trace_resolution));
    BCM_IF_ERROR_RETURN(
        ptrace_stp_state_print(pkt_trace_info->pkt_trace_stp_state));
    BCM_IF_ERROR_RETURN(
        ptrace_abs_hashing_info_print(unit, &pkt_trace_info->pkt_trace_hash_info));

    return BCM_E_NONE;
}
/*
 * Print the lookup string corresponding to the given the lookup result
 */
int
ptrace_abs_lookup_results_print(int unit, bcm_switch_pkt_trace_lookup_result_t *lr)
{
    int i;
    int found = 0;

    ptrace_abs_lookup_results_t abs_lookup_results_table[] = {
        { bcmSwitchPktTraceLookupFirstVlanTranslationHit,
          "FirstVlanTranslationHit" },
        { bcmSwitchPktTraceLookupSecondVlanTranslationHit,
          "SecondVlanTranslationHit" },
        { bcmSwitchPktTraceLookupForwardingVlanValid,
          "ForwardingVlanValid" },
        { bcmSwitchPktTraceLookupL2SrcHit,
          "L2SrcHit" },
        { bcmSwitchPktTraceLookupL2SrcStatic,
          "L2SrcStatic" },
        { bcmSwitchPktTraceLookupL2DstHit,
          "L2DstHit" },
        { bcmSwitchPktTraceLookupL2CacheHit,
          "L2CacheHit" },
        { bcmSwitchPktTraceLookupL3SrcHostHit,
          "L3SrcHostHit" },
        { bcmSwitchPktTraceLookupL3DestHostHit,
          "L3DestHostHit" },
        { bcmSwitchPktTraceLookupL3DestRouteHit,
          "L3DestRouteHit" },
        { bcmSwitchPktTraceLookupL2SrcMiss,
          "L2SrcMiss" },
        { bcmSwitchPktTraceLookupDosAttack,
          "DosAttack" },
        { bcmSwitchPktTraceLookupIpTunnelHit,
          "IpTunnelHit" },
        { bcmSwitchPktTraceLookupMplsLabel1Hit,
          "MplsLabel1Hit" },
        { bcmSwitchPktTraceLookupMplsLabel2Hit,
          "MplsLabel2Hit" },
        { bcmSwitchPktTraceLookupMplsTerminated,
          "MplsTerminated" },
        { bcmSwitchPktTraceLookupMystationHit,
          "MystationHit" },
        { 0, NULL }
    };

    ptrace_abs_lookup_results_t *alrtp = &(abs_lookup_results_table[0]);

    printf("Lookup Status Results:\n");
    while (1) {
        if (! alrtp->name) {
            break;
        }

        /* Assumes only one bitmap element */
        if (lr->pkt_trace_status_bitmap[0] & (1 << alrtp->bit )) {
            found++;
            printf("   %s\n", alrtp->name);
        }
        alrtp++;
    }

    return found ? BCM_E_NONE : BCM_E_FAIL;
}

/*
 * Print the string corresponding to the given packet resolution value,
 */
int
ptrace_abs_resolution_print(int unit, bcm_switch_pkt_trace_resolution_t resolution)
{
    int i;
    int found = 0;

    /* Lookup table to map resolution to string  */
    ptrace_abs_resolution_t  abs_resolution_table[] = {
        { bcmSwitchPktTraceResolutionUnkown, "UnknownPkt" },
        { bcmSwitchPktTraceResolutionControlPkt, "ControlPkt" },
        { bcmSwitchPktTraceResolutionOamPkt, "OamPkt" },
        { bcmSwitchPktTraceResolutionBfdPkt, "BfdPkt" },
        { bcmSwitchPktTraceResolutionBpduPkt, "BpduPkt" },
        { bcmSwitchPktTraceResolution1588Pkt, "1588Pkt" },
        { bcmSwitchPktTraceResolutionKnownL2UcPkt, "KnownL2UcPkt" },
        { bcmSwitchPktTraceResolutionUnknownL2UcPkt, "UnknownL2UcPkt" },
        { bcmSwitchPktTraceResolutionKnownL2McPkt, "KnownL2McPkt" },
        { bcmSwitchPktTraceResolutionUnknownL2McPkt, "UnknownL2McPkt" },
        { bcmSwitchPktTraceResolutionL2BcPkt, "L2BcPkt" },
        { bcmSwitchPktTraceResolutionKnownL3UcPkt, "KnownL3UcPkt" },
        { bcmSwitchPktTraceResolutionUnknownL3UcPkt, "UnknownL3UcPkt" },
        { bcmSwitchPktTraceResolutionKnownIpmcPkt, "KnownIpmcPkt" },
        { bcmSwitchPktTraceResolutionUnknownIpmcPkt, "UnknownIpmcPkt" },
        { bcmSwitchPktTraceResolutionKnownMplsL2Pkt, "KnownMplsL2Pkt" },
        { bcmSwitchPktTraceResolutionUnknownMplsPkt, "UnknownMplsPkt" },
        { bcmSwitchPktTraceResolutionKnownMplsL3Pkt, "KnownMplsL3Pkt" },
        { bcmSwitchPktTraceResolutionKnownMplsPkt, "KnownMplsPkt" },
        { bcmSwitchPktTraceResolutionKnownMimPkt, "KnownMimPkt" },
        { bcmSwitchPktTraceResolutionUnknownMimPkt, "UnknownMimPkt" },
        { bcmSwitchPktTraceResolutionKnownTrillPkt, "KnownTrillPkt" },
        { bcmSwitchPktTraceResolutionUnknownTrillPkt, "UnknownTrillPkt" },
        { bcmSwitchPktTraceResolutionKnownNivPkt, "KnownNivPkt" },
        { bcmSwitchPktTraceResolutionUnknownNivPkt, "UnknownNivPkt" },
        { bcmSwitchPktTraceResolutionKnownL2GrePkt, "KnownL2GrePkt" },
        { bcmSwitchPktTraceResolutionKnownVxlanPkt, "KnownVxlanPkt" },
        { 0, NULL }
    };
    ptrace_abs_resolution_t *artp = &(abs_resolution_table[0]);

    printf("Packet Resolution Result:\n");
    /* Search for resolution entry in the lookup table */
    while (1) {
        if (! artp->name) {
            break;
        }
        if (resolution == artp->resolution) {
            printf("   %s\n", artp->name);
            found++;
        }
        artp++;
    }

    return found ? BCM_E_NONE : BCM_E_FAIL;
}

/*
 * Print STP state
 */
int
ptrace_stp_state_print(bcm_stg_stp_t stp_state)
{
    int si = (auto) stp_state;
    char *stp_state_names[] =  {
        /* In 56960, learn==forward */
        "Disable", "Block", "Listen", "Forward", "Forward"
    };

    printf("Source port STP state: ");
    if (stp_state > BCM_STG_STP_FORWARD) {
        printf(" Unexpected STP state=%d\n", stp_state);
        return BCM_E_FAIL;
    } else {
        printf("%s\n", stp_state_names[stp_state]);
    }

    return BCM_E_NONE;
}

/*
 * Print hashing results
 */
int
ptrace_abs_hashing_info_print(int unit, bcm_switch_pkt_trace_hashing_info_t *hi)
{
    /* Hashing resolution? */
    if (hi->flags == 0) {
        return BCM_E_NONE;
    }

    /*
     * Print the hashing resolution for ECMP and trunk
 */
    if (hi->flags & BCM_SWITCH_PKT_TRACE_ECMP_1) {
        printf("Hash resolution: ECMP1\n");
        BCM_IF_ERROR_RETURN(
            ptrace_l3_ecmp_print(unit, hi->ecmp_1_group));
        BCM_IF_ERROR_RETURN(
            ptrace_l3_egress_print(unit, hi->ecmp_1_egress));
    }

    if (hi->flags & BCM_SWITCH_PKT_TRACE_ECMP_2) {
        printf("Hash resolution: ECMP2\n");
        BCM_IF_ERROR_RETURN(
            ptrace_l3_ecmp_print(unit,  hi->ecmp_2_group));
        BCM_IF_ERROR_RETURN(
            ptrace_l3_egress_print(unit, hi->ecmp_2_egress));
    }

    if (hi->flags & BCM_SWITCH_PKT_TRACE_TRUNK) {
        printf("Hash resolution: Trunk\n");
        ptrace_trunk_print(hi->trunk);
        printf("     Trunk member selected for the packet: ");
        gport_print(hi->trunk_member);
        printf("\n");
    }

    if (hi->flags & BCM_SWITCH_PKT_TRACE_FABRIC_TRUNK) {
        printf("Hash resolution: Fabric trunk\n");
        printf("   Fabric trunk: ");
        gport_print(hi->fabric_trunk);
        printf("   Fabric trunk member selected for packet: ");
        gport_print(hi->fabric_trunk_member);
        printf("\n");
    }

    return BCM_E_NONE;
}

/*
 * Print the egress interfaces corresponding to the given
 * ECMP egress object
 */
int
ptrace_l3_ecmp_print(int unit, bcm_if_t ecmp_egr)
{
    int rv;
    int idx;
    int intf_count = 0;
    bcm_if_t *intf_array;
    bcm_l3_egress_ecmp_t ecmp;

    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.ecmp_intf = ecmp_egr;

    /* First, determine how many interfaces in this ECMP egress object */
    rv = bcm_l3_egress_ecmp_get(unit, &ecmp, 0, NULL, &intf_count);
    if (BCM_FAILURE(rv)) {
        printf("Egress ecmp get failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * Allocate an interface array to hold the interfaces for this
     * ECMP egress object and get the egress interfaces
     */
    intf_array = sal_alloc(intf_count * sizeof(ecmp_egr),
                           "ecmp egress interface array");
    if (intf_array == NULL) {
        rv = BCM_E_MEMORY;
        printf("Failed allocating ecmp egress interface array: \n",
               bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_l3_egress_ecmp_get(unit, &ecmp, intf_count, intf_array,
                                &intf_count);
    if (BCM_FAILURE(rv)) {
        printf("Egress ecmp get failed: %s\n", bcm_errmsg(rv));
        sal_free(intf_array);
        return rv;
    }

    printf("   EMCP egress object=%d\n", ecmp_egr);
    printf("      Number of egress objects in ecmp group:%d \n", intf_count);
    for (idx = 0; idx < intf_count; idx++) {
    printf("         Egress Object %d: %d\n", idx, intf_array[idx]);
        if (idx && (!(idx % 8))) {
            printf("\n           ");
        }
    }
    printf("\n");

    sal_free(intf_array);

    return BCM_E_NONE;
}

/*
 * Print the matching programmed egress object corresponding to
 * the given a egress interface
 */
int
ptrace_l3_egress_print(int unit, bcm_if_t egr_intf)
{
    int rv;
    bcm_mac_t mac;
    bcm_l3_egress_t egr_obj;

    sal_memset(&egr_obj, 0, sizeof(egr_obj));
    rv = bcm_l3_egress_get(unit, egr_intf, &egr_obj);
    if (BCM_FAILURE(rv)) {
        printf("Egress get failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Print the egress object used to egress the traced packet */
    printf("   Egress object selected for the packet is %d\n", egr_intf);
    if (egr_obj.flags & BCM_L3_TGID == 0) {
      printf("      port=");
      gport_print(egr_obj.port);
    } else {
      printf("      trunk=%d", egr_obj.trunk);
    }
    printf(", L3 interface=%d, ", egr_obj.intf);

    mac = egr_obj.mac_addr;
    printf("mac=$mac");  /* cint extension */
    printf(", vlan=%d\n", egr_obj.vlan);

    return BCM_E_NONE;
}

/*
 * Print the matching programmed trunk information corresponding
 * to the given a trunk id
 */
int
ptrace_trunk_print(bcm_trunk_t tid)
{
    int rv;
    int i;
    int member_count;
    bcm_trunk_info_t tinfo;
    bcm_trunk_member_t member_array[BCM_TRUNK_MAX_PORTCNT];

    rv = bcm_trunk_get(0, tid, &tinfo, BCM_TRUNK_MAX_PORTCNT, &member_array,
                       &member_count);
    if (BCM_FAILURE(rv)) {
        printf("bcm_trunk_get failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("   Trunk: %d\n", tid);
    printf("     Rtag: %d\n", tinfo.psc);

    /* Print all members in this trunk */
    printf("     Members: %d ports\n", member_count);
    for (i = 0; i < member_count; i++) {
        printf("       ");
        gport_print(member_array[i].gport);
        printf("\n");
    }

    return BCM_E_NONE;
}

/* 
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
  int port_list[7], i, rv;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 7)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  ingress_port = port_list[0];
  egress_port_1_1 = port_list[1];
  egress_port_1_2 = port_list[2];
  egress_port_1_3 = port_list[3];
  egress_port_2_1 = port_list[4];
  egress_port_2_2 = port_list[5];
  egress_port_2_3 = port_list[6];

  if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", ingress_port);
    return -1;
  }

  for (i = 1; i <= 6; i++) {
    if (BCM_E_NONE != egress_port_setup(unit, port_list[i])) {
      printf("egress_port_setup() failed for port %d\n", port_list[i]);
      return -1;
    }
  }
  if (BCM_FAILURE((rv = config_l3_trunk_ecmp_rtag7(unit)))) {
    printf("config_l3_trunk_ecmp_rtag7() failed.\n");
    return -1;
  }
  printf("Completed configuration of L3 routing flow with ECMP group as destination(i.e executing config_l3_trunk_ecmp_rtag7()) successfully.\n");

  return BCM_E_NONE;
}
void verify(int unit)
{

  ptrace_cfg.src_port = ingress_port;
  printf("Sending trace packet on masquerade source port: %d\n", ingress_port);
  ptrace(0, &ptrace_cfg);
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit =0;

  bshell(unit, "config show; a ; version");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }
  printf("Completed test setup successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

