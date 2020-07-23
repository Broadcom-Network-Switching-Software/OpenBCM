/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Feature  : Flex Counter 2.0
 *
 * Usage    : BCM.0> cint td4_hsdk_flexctr_vxlan_decap.c
 *
 * config   : bcm56880_a0-generic-32x400.config.yml
 *
 * Log file : td4_hsdk_flexctr_vxlan_decap_log.txt
 *
 * Test Topology :
 *                      +-------------------+
 *                      |                   |
 *                      |        TD4        |
 *        network port  |                   |  access_port
 *       ---->----------+                   +----->-----
 *                      |                   |  
 *                      |                   |  
 *                      |                   |
 *                      |                   |
 *                      +-------------------+
 *
 *
 * Summary:
 * ========
 * cint script to demonstrate flex counter implementation for VxLAN decap
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in testSetup()):
 *   =============================================
 *     a) Select access port and network port configure them in Loopback mode. 
 *     b) Do the necessary VxLAN configurations
 *     c) Configure Flex counters with appropriate paremeters and attach the
 *        counter to the network flow port
 *
 *   2) Step2 - Verification (Done in testVerify()):
 *   ===========================================
 *     a) Check the output of bcm_flexctr_stat_get 
 *
 *     b) Expected Result:
 *     ===================
 *     The output of bcm_flexctr_stat_get should match the number of transmitted 
 *     packets
 *
 *   3) Step3 - Clean up (Done in testCleanup()):
 *   ===========================================
 *     a) The scripts detaches the flex counter of the module and destroys it. 
 */

cint_reset();
uint32_t flexctr_action_id;
bcm_vlan_t vid_acc = 0x776;
bcm_vlan_t vid_network = 0x92;
bcm_pbmp_t pbmp, ubmp;

int cpu_port = 0;

bcm_port_t port_acc;
bcm_port_t port_network;
bcm_gport_t gport_acc = BCM_GPORT_INVALID;
bcm_gport_t gport_network = BCM_GPORT_INVALID;
bcm_gport_t gport_cpu_port = BCM_GPORT_INVALID;

bcm_mac_t dst_mac = "E8:A2:D9:3A:7E:95";
bcm_mac_t src_mac = "2C:EB:D7:92:C7:6A";
bcm_mac_t payload_dst_mac = "8A:77:BA:B3:22:C6";
bcm_mac_t payload_src_mac = "18:BF:A3:C4:12:44";
bcm_l2_addr_t l2addr;

bcm_l2_station_t l2_station;
int station_id;

bcm_vpn_t vpn = 0x8064;
bcm_flow_vpn_config_t vpn_info;
bcm_multicast_t bcast_group;
uint32 flags;

int acc_flow_port;
int net_flow_port;
bcm_flow_port_t flow_port;

bcm_ip_t tnl_local_ip = 0x76c66ade;
bcm_ip_t tnl_remote_ip = 0x495fb3db;
uint16 udp_dst_port = 0x12b5;
uint16 udp_src_port = 0; 
int vnid = 0x339d1;

bcm_flow_tunnel_terminator_t ttinfo;
bcm_flow_match_config_t info;

uint32 flow_handle;

/*
 * This function checked if given port/index is already present
 * in the list so that same ports number is not generated again
 */
int checkPortAssigned(int *port_index_list,int no_entries, int index)
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
int portNumbersGet(int unit, int *port_list, int num_ports)
{
  int i = 0, port = 0,rv = 0, index = 0;
  bcm_port_config_t configP;
  bcm_pbmp_t ports_pbmp;
  int tempports[BCM_PBMP_PORT_MAX];
  int port_index_list[num_ports];

  rv = bcm_port_config_get(unit,&configP);
  if(BCM_FAILURE(rv)) {
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
    if (checkPortAssigned(port_index_list, i, index) == 0)
    {
      port_list[i] = tempports[index];
      port_index_list[i] = index;
    } else {
      i = i - 1;
    }
  }

  return BCM_E_NONE;
}

int vxlan_config(int unit)
{
  BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_acc, &gport_acc));
  BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, port_network, &gport_network));
  BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, cpu_port, &gport_cpu_port));

  BCM_IF_ERROR_RETURN(bcm_flow_handle_get(0,"CLASSIC_VXLAN",&flow_handle));

  /* Port VLAN configuration  */
  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, port_acc);
  /* switch payload to CPU for check */ 
  BCM_PBMP_PORT_ADD(pbmp, cpu_port);
  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_acc));
  BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_acc, pbmp, ubmp));

  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, port_network);
  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid_network));
  BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid_network, pbmp, ubmp));


  /* Set port default vlan id */
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port_acc, vid_acc));
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, port_network, vid_network));

  /* Set port learning */
  BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, port_acc, BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD));
  BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, port_network, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));


  /* VxLAN enable & VFI assignment KEY selection */
  BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_network, bcmPortControlVxlanEnable, 1));
  BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, port_network, bcmPortControlVxlanTunnelbasedVnId, 0));

  /* Set default inner tpid per port */
  BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit, cpu_port, 0x9100));
  BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit, port_acc, 0x9100));
  BCM_IF_ERROR_RETURN(bcm_port_inner_tpid_set(unit, port_network, 0x9100));

  /* Set default outer tpid per port */
  BCM_IF_ERROR_RETURN(bcm_port_tpid_set(unit, cpu_port, 0x8100));
  BCM_IF_ERROR_RETURN(bcm_port_tpid_set(unit, port_acc, 0x8100));
  BCM_IF_ERROR_RETURN(bcm_port_tpid_set(unit, port_network, 0x8100));

  /* Create VPN */
  flags  = BCM_MULTICAST_TYPE_FLOW;
  BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, flags, &bcast_group));

  bcm_flow_vpn_config_t_init(&vpn_info);
  vpn_info.flags = BCM_FLOW_VPN_ELAN | BCM_FLOW_VPN_WITH_ID;
  vpn_info.broadcast_group = bcast_group;
  vpn_info.unknown_multicast_group = bcast_group;
  vpn_info.unknown_unicast_group = bcast_group;
  BCM_IF_ERROR_RETURN(bcm_flow_vpn_create(unit, &vpn, &vpn_info));
  printf("vpn: 0x%x\n",vpn);

  /* Port must be added to VPN to pass membership check */
  BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vpn, gport_acc, 0));
  BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vpn, gport_network, 0));
  BCM_IF_ERROR_RETURN(bcm_vlan_gport_add(unit, vpn, gport_cpu_port, 0));

  /* Access VP creation*/
  bcm_flow_port_t_init(&flow_port);  
  BCM_IF_ERROR_RETURN(bcm_flow_port_create(unit, vpn, &flow_port));
  acc_flow_port = flow_port.flow_port_id;
  printf("acc_flow_port: 0x%x\n",acc_flow_port);

  /* Network VP creation */
  bcm_flow_port_t_init(&flow_port);
  flow_port.flags = BCM_FLOW_PORT_NETWORK;
  BCM_IF_ERROR_RETURN(bcm_flow_port_create(unit, vpn, &flow_port));
  net_flow_port = flow_port.flow_port_id;
  printf("net_flow_port: 0x%x\n",net_flow_port);

  bcm_l2_station_t_init(&l2_station);
  sal_memcpy(l2_station.dst_mac, dst_mac, sizeof(dst_mac));
  l2_station.dst_mac_mask = "ff:ff:ff:ff:ff:ff";
  BCM_IF_ERROR_RETURN(bcm_l2_station_add(unit, &station_id, &l2_station));

  /* Tunnel terminator */
  bcm_flow_tunnel_terminator_t_init(&ttinfo);
  ttinfo.flow_handle = flow_handle;
  ttinfo.valid_elements = BCM_FLOW_TUNNEL_TERM_DIP_VALID;
  ttinfo.dip = tnl_local_ip;
  ttinfo.type = bcmTunnelTypeVxlan;
  BCM_IF_ERROR_RETURN(bcm_flow_tunnel_terminator_create(0,&ttinfo,0,NULL));

  /* SVP assignment */
  bcm_flow_match_config_t_init(&info);
  info.flow_handle = flow_handle;
  info.criteria = BCM_FLOW_MATCH_CRITERIA_SIP;
  info.flow_port = net_flow_port;
  info.sip = tnl_remote_ip;
  info.valid_elements = BCM_FLOW_MATCH_SIP_VALID |
                      BCM_FLOW_MATCH_FLOW_PORT_VALID;
  BCM_IF_ERROR_RETURN(bcm_flow_match_add(0,&info,0,NULL));

  /* VPN assignment */
  bcm_flow_match_config_t_init(&info);
  info.flow_handle = flow_handle;
  info.criteria = BCM_FLOW_MATCH_CRITERIA_VN_ID;
  info.vpn = vpn;
  info.vnid = vnid;
  info.valid_elements = BCM_FLOW_MATCH_VPN_VALID |
                      BCM_FLOW_MATCH_VNID_VALID;
  BCM_IF_ERROR_RETURN(bcm_flow_match_add(0,&info,0,NULL));

  /* Payload switch */
  bcm_l2_addr_t_init(&l2addr, payload_dst_mac, vpn);
  l2addr.port = port_acc;
  l2addr.flags |= BCM_L2_STATIC;
  BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));

  /* Egress VID assignment */
  bcm_vlan_control_vlan_t vlan_ctrl;
  bcm_vlan_control_vlan_t_init(&vlan_ctrl);
  BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_get(unit, vpn, &vlan_ctrl));
  vlan_ctrl.egress_action.outer_tag = bcmVlanActionAdd;
  vlan_ctrl.egress_action.inner_tag = bcmVlanActionNone;
  vlan_ctrl.egress_action.new_outer_vlan = vid_acc;
  BCM_IF_ERROR_RETURN(bcm_vlan_control_vlan_set(unit, vpn, vlan_ctrl));

  /* Payload loopbacked then switch to CPU */
  bcm_l2_addr_t_init(&l2addr, payload_dst_mac, vid_acc);
  l2addr.port = cpu_port;
  l2addr.flags |= BCM_L2_STATIC;
  BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));


  /* Helper configuration for network side MAC learning */

  bcm_l3_intf_t l3_intf;
  bcm_if_t egr_obj_network1 = 10 + 0x186A0 + 0x8000;
  bcm_l3_egress_t l3_egress;
  bcm_if_t intf_id_network = 20 + 0x4000;
  bcm_flow_port_encap_t peinfo;

  /* Create egress L3 interface on network side */
  bcm_l3_intf_t_init(&l3_intf);
  sal_memcpy(l3_intf.l3a_mac_addr, src_mac, sizeof(src_mac));
  l3_intf.l3a_vid = vid_network;
  l3_intf.l3a_intf_flags = BCM_L3_INTF_UNDERLAY;
  BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
  intf_id_network = l3_intf.l3a_intf_id;
  printf("network egr L3 intf: 0x%x\n",intf_id_network);	

  /* Egress object for network port 1 */
  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.flags2 = BCM_L3_FLAGS2_UNDERLAY;
  l3_egress.intf = intf_id_network;
  sal_memcpy(l3_egress.mac_addr,  dst_mac, sizeof(dst_mac));
  l3_egress.vlan = vid_network;
  l3_egress.port = port_network;
  BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egress, &egr_obj_network1));
  printf("network egr object: 0x%x\n",egr_obj_network1);	

  /* 
   * Binding nexthop to network VP,
   * Learning process will get nexthop based on VP.
   */
  bcm_flow_port_encap_t_init(&peinfo);
  peinfo.flow_handle = flow_handle;
  peinfo.flow_port = net_flow_port;
  peinfo.valid_elements = BCM_FLOW_PORT_ENCAP_EGRESS_IF_VALID;
  peinfo.egress_if = egr_obj_network1;
  BCM_IF_ERROR_RETURN(bcm_flow_port_encap_set(0,&peinfo,0,NULL));
   
  return BCM_E_NONE;
}

/*
 * Flex counter configuration for VxLAN decap 
 */
int vxlan_decap_stat_create(int unit)                 
{
    bcm_error_t rv;
    bcm_flexctr_action_t flexctr_action;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    bcm_flexctr_value_operation_t *value_b_op = NULL;
    int options = 0;
    
    bcm_flexctr_action_t_init(&flexctr_action);
    
    /* Configure FLex Counter to count on ING DVP Table using bcmFlexctrSourceDVP */
    flexctr_action.source = bcmFlexctrSourceSvp;
    flexctr_action.mode = bcmFlexctrCounterModeNormal;
    /* Mode can be to count dropped packets or Non drop packets or count all packets */
    flexctr_action.drop_count_mode = bcmFlexctrDropCountModeAll; 
    /* Total number of counters */
    flexctr_action.index_num = 1;  
    
    index_op = &flexctr_action.index_operation;
    index_op->object[0] = bcmFlexctrObjectConstZero;
    index_op->mask_size[0] = 16;
    index_op->mask_size[1] = 1;


    /* Increase counter per packet. */
    value_a_op = &flexctr_action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectConstOne;
    value_a_op->mask_size[0] = 16;
    value_a_op->mask_size[1] = 1;
    value_a_op->type = bcmFlexctrValueOperationTypeInc;

    /* Increase counter per packet bytes. */
    value_b_op = &flexctr_action.operation_b;
    value_b_op->select = bcmFlexctrValueSelectPacketLength;
    value_b_op->mask_size[0] = 16;
    value_b_op->mask_size[1] = 1;
    value_b_op->type = bcmFlexctrValueOperationTypeInc;
   
    rv = bcm_flexctr_action_create(unit, options, &flexctr_action, &flexctr_action_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in flex counter action create: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    printf("Stat Counter Id %d\n", flexctr_action_id);
    
    return BCM_E_NONE;
}

/*
 * This function is test setup.
 */
int testSetup(int unit)
{
    bcm_error_t rv;
    bcm_flow_stat_info_t flow_stat_info;
    
    int port_list[2];
    bcm_pbmp_t pbmp;
    int lb_mode = BCM_PORT_LOOPBACK_MAC;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
      printf("portNumbersGet() failed\n");
      return -1;
    }
  
    port_acc = port_list[0];
    printf("Choosing port %d as access port\n", port_acc);
    
    port_network = port_list[1];
    printf("Choosing port %d as network port\n", port_network);
    
    /* Set port mac loopback mode */
      BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_acc, lb_mode));
      BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port_network, lb_mode));
    
    rv = vxlan_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in vxlan config %s\n", bcm_errmsg(rv));
        return rv;
    }    
    
    rv = vxlan_decap_stat_create(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in vxlan encap stat create %s\n", bcm_errmsg(rv));
        return rv;
    }
    
    bcm_flow_stat_info_t_init(&flow_stat_info);
    flow_stat_info.valid_elements = BCM_FLOW_STAT_FLOW_PORT_VALID;
    flow_stat_info.flow_port = net_flow_port;
    /* Attach created Flex Counter to VP for counting */
      BCM_IF_ERROR_RETURN(bcm_flow_stat_attach(unit, &flow_stat_info, 0, NULL, flexctr_action_id));
         
    return BCM_E_NONE;
}

/*
 * This function does the following.
 * Transmit the below unicast packet on ingress_port and verify
 */
int testVerify(int unit)
{
  char str[512];
  int rv;
  bcm_flexctr_counter_value_t counter_value;
  uint32 num_entries = 1;
  uint32 counter_idx = 0;

  /* Send a packet to ingress port*/
  printf("\n******************************\n");
  printf("******Traffic test start******\n");
  printf("Packet comes into network port %d\n", port_network);
  snprintf(str, 512, "tx 1 pbm=%d data=0xE8A2D93A7E952CEBD792C76A81002092080045E800A0000000002211B886495FB3DB76C66ADE000012B5008CAAE0080000000339D1008A77BAB322C618BFA3C4124408004500006E00000000B5064D59C2F433C69B412635006F00DEEDAC9872229431E4500000001D1300004B75E96BE516F4E596D17C37310FA37001B0622F8355815A174E471EC33F2443C869F9809CDBADDE241B8E0E69080A425988F62608F150F3EAA952356A35D3C71BB4E751000135F29D6D ;sleep 5", port_network);
  printf("%s\n", str);
  bshell(unit, str);
  bshell(unit, "show c");

  /* Get counter value. */
  sal_memset(&counter_value, 0, sizeof(counter_value));      
  rv = bcm_flexctr_stat_get(unit, flexctr_action_id, num_entries, &counter_idx,
                            &counter_value);
  if (BCM_FAILURE(rv)) {
      printf("flexctr stat get failed: [%s]\n", bcm_errmsg(rv));
      return rv;
      } else {
          printf("Flex Counters collected on ING VPN %d  Packets / Bytes : 0x%08x / 0x%08x \n",
                  vpn, COMPILER_64_LO(counter_value.value[0]),
                  COMPILER_64_LO(counter_value.value[1]));
      }
  
  return BCM_E_NONE;
}

/*
 * Cleanup test setup
 */
int testCleanup(int unit)
{
  bcm_error_t rv;
  int i;
  bcm_flow_stat_info_t flow_stat_info;

  /* Detach counter action. */
  bcm_flow_stat_info_t_init(&flow_stat_info);
  flow_stat_info.valid_elements = BCM_FLOW_STAT_FLOW_PORT_VALID;
  flow_stat_info.flow_port = net_flow_port;
  rv = bcm_flow_stat_detach(unit, &flow_stat_info, 0, NULL, flexctr_action_id);
  if (BCM_FAILURE(rv)) {
      printf("bcm_flow_stat_detach %s\n", bcm_errmsg(rv));
      return rv;
  }
  
  /* Destroy counter action. */
  rv = bcm_flexctr_action_destroy(unit, flexctr_action_id);
  if (BCM_FAILURE(rv)) {
    printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
    return rv;
  }

  return BCM_E_NONE;
}

/*
 * This functions does the following
 * a)test setup *
 * b)demonstrates the functionality(done in testVerify()).
 * c)clean up
 */
int testExecute()
{
  bcm_error_t rv;
  int unit = 0;

  bshell(unit, "config show; a ; version");

  rv = testSetup(unit);
  if (BCM_FAILURE(rv)) {
    printf("testSetup() failed, %s.\n", bcm_errmsg(rv));
    return -1;
  }
  printf("Completed test setup successfully.\n");

  rv = testVerify(unit);
  if (BCM_FAILURE(rv)) {
    printf("testVerify() failed\n");
    return -1;
  }
  printf("Completed test verify successfully.\n");

  rv = testCleanup(unit);
  if (BCM_FAILURE(rv)) {
    printf("testCleanup() failed, return %s.\n", bcm_errmsg(rv));
    return -1;
  }

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print testExecute();
}
