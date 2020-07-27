/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* Feature        : WRED-ECN
 *
 * Usage          : BCM.0> cint wred_ecn.c
 *
 * config         : BCM56275_A1-PORT.bcm
 *
 * Log file       : wred_ecn_log.txt
 *
 * Test Topology  :
 *
 * DMAC=0x1            +------------+         DMAC=0x1
 * SMAC=0x2   +-------->   56275    +-------> SMAC=0x2
 * VLAN=2,Pri=4  ge0(1)|            |ge1(2)   VLAN=2,Pri=4
 *                     | l2:mac=0x1 |
 * TEST-1:             |    vlan=2  |        TEST-1:
 * IP.Protocol=1(ICMP) |    port=2  |        IF congested:
 * ECN bits=01         |            |          ECN bits=11
 *                     +------------+        If not congested:
 * TEST-2:                                     ECN bits=10
 * IP.Protocol=2(IGMP)
 * ECN bits=01                               TEST-2:
 *                                           If congested:
 *                                            WRED drops
 *                                           If not congested:
 *                                            No drops or marking
 *
 *  Summary:
 *  ========
 *  This cint example illustrates configuration of WRED and ECN feature
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup()) 
 *    ============================================
 *      a) Select one ingress and one egress port
 *    2) Step2 - Configuration (Done in wred_ecn_config())
 *    ====================================================
 *      a) data_path_setup - sets up required data path for traffic
 *      b) set_responsive_protocol_indication - sets IP Protocol's Responsive indication setting
 *      c) ip_to_int_cn_mapping - maps IP header ECN bits to INT_CN value
 *      d) int_cn_to_mmuif_mapping - INT_CN value to WRED_RESPONSIVE and MARK_ELIGIBLE configuration
 *      e) mmu_wred_ecn_setup - MMU wred and ecn profile Settings
 *      f) egr_int_cn_update - configure outgoing INT_CN value based on congestion status in MMU
 *      g) egr_ecn_mark - Mark outgoing IP header ECN bits based on final INT_CN value
 *    3) Step3 - Verification (Done in verify()) 
 *    ==================================
 *      a) Transmit ICMP packets at line rate through Ixia connected to cd0
 *      b) Transmit IGMP packets at line rate through Ixia connected to cd0
 *      c) Expected Result:
 *         ================
 *         For ICMP packets - In case of congestion we should see ECN marking (ECN bits=11)
 *         for packets egressing ge1. If no congestion, ECN marking will not happen (ECN bits=10)
 *         FOR IGMP packets - In case of congestion, WRED drops will be seen on ge1.
 *                            This can be verified by 'dump chg CTR_WRED_DROP' output.
 *                            If no congestion, No wred drops will be seen.
 */

cint_reset();
int rv = 0;
int unit = 0;
int check = 1;  /*Make it '1' to dump tables that are configured by each API*/
bcm_port_t ing_port = 1; /*Ingress port */
bcm_port_t egr_port = 2; /*Egress port */
bcm_cos_t cos = 4; /*COS Queue of Egress port on which WRED is configured */

uint8 iphdr_ecn = 1; /*Incoming packet IP headers ECN field(2 bits). 0b01 taken as example for this test.*/
uint8 resp_mapped_int_cn = 2; /*INT_CN value to which Responsive protocol is mapped to. 0b10 taken as example for this test */
uint8 nonresp_mapped_int_cn = 0; /*INT_CN value to which Non-Responsive protocol is mapped to. 0b00 taken as example for this test */

uint8 non_cong_int_cn = 2; /*INT_CN value at Dequeue from MMU when there is no congestion. 0b10 taken as example for this test */
uint8 cong_int_cn = 3; /*INT_CN value at Dequeue from MMU when there is congestion. 0b11 taken as example for this test  */

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

bcm_error_t test_setup(int unit)
{
  int port_list[2], i;
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
      printf("portNumbersGet() failed\n");
      return -1;
  }

  ing_port = port_list[0];
  egr_port = port_list[1];

  return BCM_E_NONE;
}

bcm_error_t data_path_setup(int unit) 
{
 bcm_vlan_t vid = 2;
 bcm_mac_t dest_mac = "00:00:00:00:00:01"; /*Destination MAC */
 bcm_pbmp_t pbmp,upbmp;
 bcm_l2_addr_t l2addr;
 
 rv = bcm_vlan_create(unit, vid);
 if (rv != BCM_E_NONE) {
     printf("bcm_vlan_create returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }

 BCM_PBMP_CLEAR(pbmp);
 BCM_PBMP_CLEAR(upbmp);
 BCM_PBMP_PORT_ADD(pbmp, ing_port);
 BCM_PBMP_PORT_ADD(pbmp, egr_port);
 rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
 if (rv != BCM_E_NONE) {
     printf("bcm_vlan_port_add returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }

 bcm_l2_addr_t_init(l2addr, dest_mac, vid);
 l2addr.port = egr_port;
 l2addr.flags |= BCM_L2_STATIC;
 /*WRED works ONLY on unicast. So, l2 entry with DEST-MAC and Dest-port should be present*/
 rv = bcm_l2_addr_add(unit, &l2addr);
 if (rv != BCM_E_NONE) {
     printf("bcm_l2_addr_add returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }
 return BCM_E_NONE;
}

bcm_error_t set_responsive_protocol_indication(int unit)
{ 
 uint8 ip_protocol_resp = 1; /*Protocol field in IP header that gets programmed as Responsive (generally TCP.) 1 = ICMP */
 uint8 ip_protocol_nonresp = 2; /*Protocol field in IP header that gets programmed as Responsive (generally UDP.) 2 = IGMP*/
 int responsive_indicator = 1;

 rv = bcm_ecn_responsive_protocol_set(unit, ip_protocol_resp, responsive_indicator);
 if (rv != BCM_E_NONE) { 
     printf("bcm_ecn_responsive_protocol_set~Resp-protocol returned '%s'\n",bcm_errmsg(rv));
 }

 responsive_indicator = 0;
 rv = bcm_ecn_responsive_protocol_set(unit, ip_protocol_nonresp, responsive_indicator);
 if (rv != BCM_E_NONE) {
     printf("bcm_ecn_responsive_protocol_set~NonResp-protocol returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }
 return BCM_E_NONE;
}

bcm_error_t ip_to_int_cn_mapping(int unit)
{
 bcm_ecn_traffic_map_info_t map;
 bcm_ecn_traffic_action_config_t config;
 bcm_cosq_gport_discard_t discard_cfg;

 bcm_ecn_traffic_map_info_t_init(&map);
 map.ecn    = iphdr_ecn;
 map.flags  = BCM_ECN_TRAFFIC_MAP_RESPONSIVE;
 map.int_cn = resp_mapped_int_cn; /*Responsive*/
 rv = bcm_ecn_traffic_map_set(unit, &map);
 if (rv != BCM_E_NONE) {
     printf("bcm_ecn_traffic_map_set~Resp-protocol returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }

 bcm_ecn_traffic_map_info_t_init(&map);
 map.ecn    = iphdr_ecn;
 map.flags  = 0;
 map.int_cn = nonresp_mapped_int_cn; /*Non-Responsive */
 rv = bcm_ecn_traffic_map_set(unit, &map);
 if (rv != BCM_E_NONE) {
     printf("bcm_ecn_traffic_map_set~NonResp-protocol returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }
 return BCM_E_NONE;
}

bcm_error_t int_cn_to_mmuif_mapping(int unit)
{
 bcm_ecn_traffic_action_config_t config;

 bcm_ecn_traffic_action_config_t_init(&config);
 config.action_type = BCM_ECN_TRAFFIC_ACTION_TYPE_ENQUEUE;
 config.int_cn = resp_mapped_int_cn;  /*Responsive*/
 config.action_flags = BCM_ECN_TRAFFIC_ACTION_ENQUEUE_WRED_RESPONSIVE|BCM_ECN_TRAFFIC_ACTION_ENQUEUE_MARK_ELIGIBLE;
 rv = bcm_ecn_traffic_action_config_set(unit, &config); /*For the above INT_CN value, enable both Responsive drop and ECN Marking.*/
 if (rv != BCM_E_NONE) {
     printf("bcm_ecn_traffic_action_config_set:Enqueue~Resp-protocol returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }

 bcm_ecn_traffic_action_config_t_init(&config);
 config.action_type = BCM_ECN_TRAFFIC_ACTION_TYPE_ENQUEUE;
 config.int_cn = nonresp_mapped_int_cn;  /*Non-Responsive */
 config.action_flags = 0;
 rv = bcm_ecn_traffic_action_config_set(unit, &config);/*For the above INT_CN value, enable Non-Responsive drop, but not ECN marking*/
 if (rv != BCM_E_NONE) {
     printf("bcm_ecn_traffic_action_config_set:Enqueue~NonResp-protocol returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }
 return BCM_E_NONE;
}

bcm_error_t mmu_wred_ecn_setup(int unit) 
{
 bcm_cosq_gport_discard_t discard_cfg; 

 bcm_cosq_gport_discard_t_init(&discard_cfg);
 discard_cfg.flags = BCM_COSQ_DISCARD_ENABLE|  /*WRED_EN=1 */
                     BCM_COSQ_DISCARD_MARK_CONGESTION| /*ECN_MARKING_EN=1 */
                     BCM_COSQ_DISCARD_BYTES|
                     BCM_COSQ_DISCARD_COLOR_GREEN| /*Modify only GREEN Drop curve Profiles-0 or 3 or 6*/
                     BCM_COSQ_DISCARD_RESPONSIVE_DROP| /*Responsive drop profile - 0 */
                     BCM_COSQ_DISCARD_ECT_MARKED; /*Marking profile - 6 */
 discard_cfg.min_thresh = 1*256; /*Queue depth in bytes to begin dropping.(256 bytes = 1 cell in MV2)*/
 discard_cfg.max_thresh = 4*256; /*Queue depth in bytes to drop all packets(256 bytes = 1 cell in MV2)*/
 discard_cfg.drop_probability = 90; /*Drop probability at max threshold*/
 rv = bcm_cosq_gport_discard_set(unit, egr_port, cos, &discard_cfg);
 if (rv != BCM_E_NONE) {
     printf("bcm_cosq_gport_discard_set~Resp-protocol returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }
 
 bcm_cosq_gport_discard_t_init(&discard_cfg);
 discard_cfg.flags = BCM_COSQ_DISCARD_ENABLE|  /*WRED_EN=1 */
                     BCM_COSQ_DISCARD_MARK_CONGESTION| /*ECN_MARKING_EN=1. */
                     BCM_COSQ_DISCARD_BYTES|
                     BCM_COSQ_DISCARD_COLOR_GREEN| /*Modify only GREEN Drop curve Profiles-0 or 3 or 6*/
                     BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP; /*Non-Responsive drop profile - 3 */
 discard_cfg.min_thresh = 3*256; /*Queue depth in bytes to begin dropping.(256 bytes = 1 cell)*/
 discard_cfg.max_thresh = 5*256; /*Queue depth in bytes to drop all packets(256 bytes = 1 cell*/
 discard_cfg.drop_probability = 90; /*Drop probability at max threshold*/
 rv = bcm_cosq_gport_discard_set(unit, egr_port, cos, &discard_cfg);
 if (rv != BCM_E_NONE) {
     printf("bcm_cosq_gport_discard_set~NonResp-protocol returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }
 return BCM_E_NONE;
}

bcm_error_t bcm_egr_int_cn_update(int unit)
{
 bcm_ecn_traffic_action_config_t config;

 bcm_ecn_traffic_action_config_t_init(&config);
 config.action_type  = BCM_ECN_TRAFFIC_ACTION_TYPE_DEQUEUE;
 config.int_cn       = resp_mapped_int_cn; /*Responsive */
 config.color        = bcmColorGreen;
 config.action_flags = BCM_ECN_TRAFFIC_ACTION_DEQUEUE_CONGESTION_INT_CN_UPDATE|BCM_ECN_TRAFFIC_ACTION_DEQUEUE_NON_CONGESTION_INT_CN_UPDATE;
 config.congested_int_cn     = cong_int_cn; /*New INT_CN value if congestion is indicated by MMU */
 config.non_congested_int_cn = non_cong_int_cn; /*New INT_CN value if congestion is not indicated by MMU */
 rv = bcm_ecn_traffic_action_config_set(unit, &config);
 if (rv != BCM_E_NONE) { 
     printf("[bcm_ecn_traffic_action_config_set:Dequeue]~Resp-protocol returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }

 bcm_ecn_traffic_action_config_t_init(&config);
 config.action_type  = BCM_ECN_TRAFFIC_ACTION_TYPE_DEQUEUE;
 config.int_cn       = nonresp_mapped_int_cn; /*Non-Responsive. This is not actually needed as the non-responsive protocols are NOT made 'mark eligible' previously */
 config.color        = bcmColorGreen;
 config.action_flags = BCM_ECN_TRAFFIC_ACTION_DEQUEUE_CONGESTION_INT_CN_UPDATE|BCM_ECN_TRAFFIC_ACTION_DEQUEUE_NON_CONGESTION_INT_CN_UPDATE;
 config.congested_int_cn     = cong_int_cn;
 config.non_congested_int_cn = non_cong_int_cn;
 rv = bcm_ecn_traffic_action_config_set(unit, &config);
 if (rv != BCM_E_NONE) {
     printf("bcm_ecn_traffic_action_config_set:Dequeue~NonResp-protocol returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
 return BCM_E_NONE;
}

bcm_error_t bcm_mark_ecn(int unit)
{
 bcm_ecn_traffic_action_config_t config;
 uint8 new_outgoing_noncong_ecn = 2; /*ECN bits marked in outgoing packets when there is no congestion. 0b10 taken as example */
 uint8 new_outgoing_cong_ecn = 3; /*ECN bits marked in outgoing packets when there is congestion. 0b11 taken as example  */

 bcm_ecn_traffic_action_config_t_init(&config);
 config.action_type  = BCM_ECN_TRAFFIC_ACTION_TYPE_EGRESS;
 config.int_cn       = cong_int_cn; /*INT_CN corresponding to congestion */
 config.ecn          = iphdr_ecn;
 config.action_flags = BCM_ECN_TRAFFIC_ACTION_EGRESS_ECN_MARKING;
 config.new_ecn      = new_outgoing_cong_ecn; /*ECN bits in outgoing packets if congestion is present*/
 rv = bcm_ecn_traffic_action_config_set(unit, &config);
 if (rv != BCM_E_NONE) { 
     printf("bcm_ecn_traffic_action_config_set:Egress~Congession returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }

 bcm_ecn_traffic_action_config_t_init(&config);
 config.action_type  = BCM_ECN_TRAFFIC_ACTION_TYPE_EGRESS;
 config.int_cn       = non_cong_int_cn; /*INT_CN corresponding to 'no congestion' */
 config.ecn          = iphdr_ecn;
 config.action_flags = BCM_ECN_TRAFFIC_ACTION_EGRESS_ECN_MARKING;
 config.new_ecn      = new_outgoing_noncong_ecn; /*ECN bits in outgoing packets if congestion is NOT present. (may retain original ECN bits here too for simplicity)*/
 rv = bcm_ecn_traffic_action_config_set(unit, &config);
 if (rv != BCM_E_NONE) {
     printf("bcm_ecn_traffic_action_config_set:Egress~NonCongession returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }
 return BCM_E_NONE;
}

bcm_error_t wred_ecn_config(int unit)
{
  bcm_error_t rv;

  if (BCM_FAILURE((rv = data_path_setup(unit)))) {
      printf("data_path_setup failed. : %s.\n", bcm_errmsg(rv));
      return rv;
  }

  if (BCM_FAILURE((rv = set_responsive_protocol_indication(unit)))) {
      printf("set_responsive_protocol_indication() failed. : %s.\n", bcm_errmsg(rv));
      return rv;
  }

  if (BCM_FAILURE((rv = ip_to_int_cn_mapping(unit)))) {
      printf("ip_to_int_cn_mapping() failed. : %s.\n", bcm_errmsg(rv));
      return rv;
  }

  if (BCM_FAILURE((rv = int_cn_to_mmuif_mapping(unit)))) {
      printf("int_cn_to_mmuif_mapping() failed. : %s\n", bcm_errmsg(rv));
      return rv;
  }

  if (BCM_FAILURE((rv = mmu_wred_ecn_setup(unit)))) {
      printf("wred_ecn_setup() failed. : %s\n", bcm_errmsg(rv));
      return rv;
  }

  if (BCM_FAILURE((rv = bcm_egr_int_cn_update(unit)))) {
      printf("bcm_egr_int_cn_update() failed. : %s\n", bcm_errmsg(rv));
      return rv;
  }

  if (BCM_FAILURE((rv = bcm_mark_ecn(unit)))) {
      printf("bcm_mark_ecn() failed. : %s\n", bcm_errmsg(rv));
      return rv; 
  }
  return BCM_E_NONE;
}

void verify(int unit)
{
  /* Test 1: ICMP pkt (responsive) - check ECN marking in packet egressing on cd1.
   * If congestion ECN bits = 11
   * If no congestion ECN bits = 10
   *
   * Test 2: IGMP packet (non responsive)
   * If congestion - WRED drops seen in 'show c' output for egress port cd1.
   * If no congestion - No wred drops or marking
   */
   
 if(check){
    bshell(unit,"vlan show");
    bshell(unit,"l2 show");
    bshell(unit,"d RESPONSIVE_PROTOCOL_MATCH");
    bshell(unit,"d PKT_ECN_TO_INT_CN_MAPPING");
    bshell(unit,"d INT_CN_TO_MMUIF_MAPPING");
    bshell(unit,"d chg MMU_WRED_CONFIG_PIPE0");
    bshell(unit,"d chg MMU_WRED_DROP_CURVE_PROFILE_0");
    bshell(unit,"d chg MMU_WRED_DROP_CURVE_PROFILE_3");
    bshell(unit,"d chg MMU_WRED_DROP_CURVE_PROFILE_5");
    bshell(unit,"d EGR_INT_CN_UPDATE");
    bshell(unit,"d EGR_INT_CN_TO_PKT_ECN_MAPPING");
 }
 bshell(unit, "show c");
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit =0;
  bshell(unit, "config show; a ; version ; cancun stat");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
      printf("test_setup() failed.\n");
      return -1;
  }
 
  if (BCM_FAILURE((rv = wred_ecn_config(unit)))) {
      printf("wred_ecn_config() failed.\n");
      return -1;
  }

  verify(unit);
 
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

