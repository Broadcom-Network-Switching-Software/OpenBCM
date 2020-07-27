/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : COE subport - L3 MC Flow  
 *
 * Usage    : BCM.0> cint coe_subport_l3mc.c
 *
 * config   : coe_config.bcm
 *
 * Log file : coe_subport_l3mc_log.txt
 *
 * Test Topology: Standalone FB6 device
 *
 *  Summary:
 *  ========
 *  This cint example demonstrates creating subport scheduler hieararchy
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one ingress port and one egress port on which subport has to be created
 *    2) Step2 - Configuration (Done in coe_config())
 *    ===============================================
 *      a) Creates COE subport
 *      b) Creates subport scheduler hierarchy
 *      c) Does SVTAG encap configuration
 *      d) Does SVTAG insertion configuration using EFP
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Keep ingress and egress port in loopback 
 *      b) Send L3 multicast packet on ingress port as in verify() function 
 *      c) Check if packet egressing out of egress port has 4 byte subport tag following SA MAC. 
 *      c) Expected Result:
 *         ================
 *         L3 MC packet egressing out of egress channelized port should have 4 bytes Subport tag after SA MAC 
 */

/*****************************************************************************
 *                                                                           *
 *             Configurations Specific For COE in Config File                *
 *                                                                           *
 * #specify port 7 as channelized port                                       *
 * portmap_7=7:25:1:l:1                                                      *
 *                                                                           *
 * #Logical port 7 is used as cascaded/backplace port.                       *
 * pbmp_subport=0x0000000080                                                 *
 *                                                                           *
 * # Number of subport on backplane port 7 are 48                            *
 * num_subports_7=48                                                         *
 *                                                                           *
 * # The subport addition happens through flex encofing. enable flex mode.   *
 * flow_init_mode=1                                                          *
 */

/* Reset c interpreter*/
cint_reset ();
cint_reset();
int rv = 0;
int unit = 0;
bcm_port_t ing_port; /*Ingress port */
bcm_port_t egr_port; /*Egress port */
bcm_gport_t egr_port_2;
bcm_gport_t subport_gport = 0;

bcm_gport_t l0_sched;
bcm_gport_t l1_sched[8];
bcm_gport_t uc_gport[8];
bcm_gport_t mc_gport[8];

bcm_vlan_t vlan1 = 12, vlan2 = 13;

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
  if (BCM_FAILURE(rv)) {
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
  bcm_field_qset_t qset;
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
  bcm_field_qset_t qset;
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
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0));

  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit, entry));

  return BCM_E_NONE;
}

/*
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */

bcm_error_t test_setup(int unit)
{
  int port_list[8], i;
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 8)) {
      printf("portNumbersGet() failed\n");
      return -1;
  }

  ing_port = port_list[0];
  egr_port = port_list[6];  /* xe6 -logical port 7 */
  egr_port_2 = port_list[7]; /* xe7 - logical port 8 */

  if (BCM_E_NONE != ingress_port_setup(unit, ing_port)) {
      printf("ingress_port_setup() failed for port %d\n", ing_port);
      return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, egr_port)) {
      printf("egress_port_setup() failed for port %d\n", egr_port);
      return -1;
  }

  bshell(unit, "pw start report +raw +decode");

  return BCM_E_NONE;
}


bcm_error_t vlan_create_add_port(int unit, int vid, int port)
{   
  bcm_pbmp_t pbmp, upbmp;

  rv = bcm_vlan_create(unit, vid);
  if ((rv < 0) && (rv != -8)) { /* -8 is EEXISTS*/
      return rv;
  }

  if (rv != BCM_E_NONE) {
      printf("bcm_vlan_create returned '%s' for vlan = %d\n", bcm_errmsg(rv), vid);
      return rv;
  }
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(upbmp);
  BCM_PBMP_PORT_ADD(pbmp, port);
  BCM_PBMP_PORT_ADD(pbmp, port+1);
  rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
  if (rv != BCM_E_NONE) {
      printf("bcm_vlan_port_add returned '%s' for vlan = %d\n", bcm_errmsg(rv), vid);
      return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t create_l3_interface(int unit, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
  bcm_l3_intf_t l3_intf;

  bcm_l3_intf_t_init(&l3_intf);
  l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
  sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
  l3_intf.l3a_vid = vid;
  rv = bcm_l3_intf_create(unit, &l3_intf);
  if (rv != BCM_E_NONE) {
      printf("bcm_l3_intf_create returned '%s'\n", bcm_errmsg(rv));
      return rv;
  }
  *intf_id = l3_intf.l3a_intf_id;
  return BCM_E_NONE;
}

/* Create L3 egress object */
bcm_error_t create_l3_egr_obj(int unit, int flags, int l3_if, bcm_mac_t nh_mac,
                              bcm_gport_t gport, bcm_vlan_t p_vlan,
                              bcm_mpls_label_t vc_label_1, bcm_if_t *egr_obj_id)
{   
  bcm_l3_egress_t l3_egress;

  bcm_l3_egress_t_init(&l3_egress);
  l3_egress.flags = flags|BCM_L3_IPMC;
  sal_memcpy(l3_egress.mac_addr, nh_mac, sizeof(nh_mac));
  l3_egress.intf  = l3_if;
  l3_egress.port = gport;
  l3_egress.vlan = p_vlan;
  rv = bcm_l3_egress_create(unit, 0, &l3_egress, egr_obj_id);
  if (rv != BCM_E_NONE) {
      printf("create_l3_egr_obj returned '%s'\n", bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

/* Add IPMC entry */
bcm_error_t add_ipmc(int unit, bcm_vlan_t vid, bcm_gport_t gport,
                     bcm_ip_t mc_ip, bcm_multicast_t ipmcast_group)
{   
  bcm_ipmc_addr_t ipmc_addr;

  bcm_ipmc_addr_t_init(&ipmc_addr);
  ipmc_addr.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;
  ipmc_addr.group = ipmcast_group;
  ipmc_addr.mc_ip_addr = mc_ip;
  ipmc_addr.vid = vid;
  ipmc_addr.port_tgid = gport;
  rv = bcm_ipmc_add(unit, &ipmc_addr);
  if (rv != BCM_E_NONE) {
      printf("add_ipmc returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

/* Create multicast egress object */
bcm_error_t create_mc_egr_obj(int unit, bcm_if_t l3_egr_if, bcm_gport_t gport,
                              bcm_multicast_t mcg)
{
  bcm_if_t encap_id;

  rv = bcm_multicast_egress_object_encap_get(unit, mcg, l3_egr_if, &encap_id);
  if (rv != BCM_E_NONE) {
      printf("bcm_multicast_egress_object_encap_get returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_multicast_egress_add(unit, mcg, gport, encap_id);
  if (rv != BCM_E_NONE) {
      printf("bcm_multicast_egress_add returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  bshell(0,"d chg MMU_REPL_GROUP_INFO_TBL_PIPE0");
  return BCM_E_NONE;
}

bcm_error_t vlan_config(bcm_vlan_t vid_in, bcm_vlan_t tunnel_vid_1)
{
  /* Create VLAN & add member ports */
  rv = vlan_create_add_port(unit, vid_in, ing_port);
  if (rv != BCM_E_NONE) {
      printf("vlan_create_add_port returned '%s' for vlan %d\n",bcm_errmsg(rv), vid_in);
      return rv;
  }
  rv = vlan_create_add_port(unit, tunnel_vid_1, egr_port);
  if (rv != BCM_E_NONE) {
      printf("vlan_create_add_port returned '%s' for vlan %d\n",bcm_errmsg(rv), tunnel_vid_1);
      return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t l3_mc_config(bcm_gport_t subport_gport, bcm_vlan_t vid_in, bcm_vlan_t tunnel_vid_1)
{
  bcm_multicast_t            ipmc_group;
  bcm_ipmc_addr_t            ipmc;
  bcm_gport_t                gport_in;
  bcm_gport_t                gport_tunnel_1;
  bcm_gport_t                gport_tunnel_2;
  bcm_port_t                 port_in = 2;
  bcm_port_t                 port_tunnel_1 = 7;
  bcm_port_t                 port_tunnel_2 = 8;
  bcm_mpls_label_t           vc_label_1 = 201;
  bcm_pbmp_t                 pbmp, upbmp;
  bcm_mac_t                  tunnel_mac  = {0x00,0x00,0x00,0x00,0xBB,0xBB};
  bcm_mac_t                  tunnel_nh_mac_1  = {0x00,0x00,0x00,0x00,0x13,0x13};
  bcm_mac_t                  tunnel_nh_mac_2  = {0x00,0x00,0x00,0x00,0x14,0x14};
  bcm_mac_t                  tunnel_nh_mac_null  = {0x00,0x00,0x00,0x00,0x00,0x00};
  bcm_ip_t                   mc_ip_addr=0xe1010101;
  bcm_if_t                   l3_egr_if_1;
  bcm_if_t                   tunnel_obj_egr_1;

  bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);

  bcm_port_gport_get(unit, port_in, &gport_in);
  bcm_port_gport_get(unit, port_tunnel_1, &gport_tunnel_1);
  bcm_port_gport_get(unit, port_tunnel_2, &gport_tunnel_2);

  /* Tunnel L3 interface */
  rv = create_l3_interface(unit, tunnel_mac, tunnel_vid_1, &l3_egr_if_1);
  if (rv != BCM_E_NONE) {
      printf("create_l3_interface returned '%s' for vlan %d\n",bcm_errmsg(rv), tunnel_vid_1);
      return rv;
  }

  /* Tunnel egress object */
  rv =  create_l3_egr_obj(unit, 0, l3_egr_if_1, tunnel_nh_mac_null, subport_gport,
                          tunnel_vid_1, vc_label_1, &tunnel_obj_egr_1);
  if (rv != BCM_E_NONE) {
      printf("create_l3_egr_obj returned '%s' for vlan %d\n",bcm_errmsg(rv), tunnel_vid_1);
      return rv;
  }

  printf("the egress object created = 0x%x \n", tunnel_obj_egr_1);

  /* Create L3 multicast group */
  rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3, &ipmc_group);
  if (rv != BCM_E_NONE) {
      printf("bcm_multicast_create returned '%s' for vlan %d\n",bcm_errmsg(rv), tunnel_vid_1);
      return rv;
  }
  printf("ipmc group = 0x%x\n", ipmc_group);

  /* Set up IPMC  */
  rv = add_ipmc(unit, vid_in, gport_in, mc_ip_addr, ipmc_group);
  if (rv != BCM_E_NONE) {
      printf("add_ipmc returned '%s' for vlan %d\n",bcm_errmsg(rv), tunnel_vid_1);
      return rv;
  }

  /* L3 encap */
  rv = create_mc_egr_obj(unit, tunnel_obj_egr_1, subport_gport, ipmc_group);
  if (rv != BCM_E_NONE) {
      printf("add_ipmc returned '%s' for vlan %d\n",bcm_errmsg(rv), tunnel_vid_1);
      return rv;
  }
  return BCM_E_NONE; 
}

bcm_error_t config_efp(int unit)
{
  bcm_field_group_config_t group_config;
  uint32 data,mask;
  bcm_field_entry_t entry;
  bcm_field_group_config_t_init(&group_config);
  BCM_FIELD_QSET_INIT(group_config.qset);
  BCM_FIELD_ASET_INIT(group_config.aset);

  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageEgress);
  BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyEgressPortCtrlType);
  group_config.priority = 11;

  /* Creating the group */
  BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

  /* Creating the Entry */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
  printf("efp entry = %d\n", entry);

  /* Qualify the egress port type to add SVTAG. */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_EgressPortCtrlType(unit, entry, bcmFieldEgressPortCtrlTypeSubportSvtagEncrypt));

  /* Action to specify the SVTAG signature in outgoing packet */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionSvtagSignature, 0x11, 0));

  /* specify offset for linecard to know from where to start encryption if packet is to
     go through MACSEC encryption */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionSvtagOffset, 0xE8, 0));

  /* set priority in SVTAG of the outgoing packet */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionSvtagPrio, 0, 0));

  /* Assign subport number - can also assign it through encap configuration per flow */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionSvtagSubportNum, 10, 0));

  /* Add subport tag in the out going packet */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionSvtagEnable, 0, 0));

  /* Installing the Entry */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit,entry));

  return BCM_E_NONE;
}

/* SVTAG Encap Configuration */
bcm_error_t configure_flex_macsec_encoding(int vlan, bcm_gport_t subport_gport)
{
  int unit =0;
  int i = 0;
  bcm_flow_handle_t handle;
  bcm_flow_option_id_t option_id;
  bcm_flow_encap_config_t encap_config;
  bcm_flow_encap_config_t_init(&encap_config);
  bcm_flow_logical_field_t logical_fields;
  bcm_flow_logical_field_t_init(&logical_fields);
  bcm_flow_field_id_t field_id;
  rv = bcm_flow_handle_get(unit, "SUBPORT_SVTAG_ENCRYPT", &handle);
  if (rv != BCM_E_NONE) {
      printf("bcm_flow_handle_get returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_flow_option_id_get(unit, handle, "LOOKUP_DGPP_OVLAN_INTPRI_ASSIGN_CLASS_ID", &option_id);
  if (rv != BCM_E_NONE) {
      printf("bcm_flow_option_id_get returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  encap_config.flow_handle = handle;
  encap_config.flow_option = option_id;
  encap_config.flow_port = subport_gport; /* Destination port number */
  encap_config.vlan = vlan;
  encap_config.pri=i;
  /* Make this a macsec enabled flow. */
  encap_config.flags |= BCM_FLOW_PORT_ENCAP_FLAG_MACSEC_ENCRYPT;
  /* Class id will carry the encap subport number. */
  encap_config.class_id = 20;
  encap_config.criteria = BCM_FLOW_ENCAP_CRITERIA_DGPP_VLAN_INT_PRI;
  encap_config.valid_elements = (BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_CLASS_ID_VALID | BCM_FLOW_ENCAP_VLAN_VALID);

  rv = bcm_flow_encap_add(unit, &encap_config, 0, &logical_fields);
  if (rv != BCM_E_NONE) {
      printf("bcm_flow_encap_add returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  print bshell(0,"d EGR_VLAN_XLATE_2_SINGLE");
  return BCM_E_NONE;
}

bcm_error_t create_subport(int vlan, int phy_port, bcm_gport_t *subport_gport)
{

  int unit = 0;
  bcm_stk_modid_config_t mod_cfg;
  int port = phy_port;  /* Physical port number */
  bcm_gport_t port_gport;
  uint16 subport_tag_tpid = 0x8100;
  bcm_subport_group_config_t  subport_group_cfg;
  bcm_gport_t subport_group_gport;
  bcm_subport_config_t subport_cfg;
  int subtag_vlan = 4; /* Used to decap subtag's subport number. */

  mod_cfg.modid = 5;
  mod_cfg.num_ports = 10;
  mod_cfg.modid_type = bcmStkModidTypeCoe;

  rv =  bcm_stk_modid_config_add(0, &mod_cfg);
  if (rv != BCM_E_NONE) {
      printf("bcm_stk_modid_config_add returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  /* Create Subport Group */

  rv = bcm_port_control_set(0, port, bcmPortControlSubportTagEnable, 1);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_control_set for bcmPortControlSubportTagEnable returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  rv = bcm_port_gport_get(0, port, &port_gport);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_gport_get returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  subport_group_cfg.port  = port_gport;
  subport_group_cfg.flags = BCM_SUBPORT_GROUP_TYPE_SUBPORT_TAG;

  rv = bcm_subport_group_create(unit, &subport_group_cfg, &subport_group_gport);
  if (rv != BCM_E_NONE) {
      printf("bcm_subport_group_create returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  /* Adding subports */
  bcm_subport_config_t_init(&subport_cfg);
  subport_cfg.group = subport_group_gport;

  /* PHB */
  subport_cfg.prop_flags  = BCM_SUBPORT_PROPERTY_PHB;
  subport_cfg.int_pri     = 2;
  subport_cfg.color       = 2;
  subport_cfg.subport_modport = 0;

  subport_cfg.pkt_vlan = subtag_vlan;

  /* Create subport and add subport to subport group */
  rv = bcm_subport_port_add(unit, &subport_cfg, subport_gport);
  if (rv != BCM_E_NONE) {
      printf("bcm_subport_port_add returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  printf("subport gport = 0x%x\n", *subport_gport);

  /* Setting the port vlan membership for subport*/
  rv = bcm_port_vlan_member_set(unit, *subport_gport, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_vlan_member_set returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_vlan_gport_add(unit, vlan, *subport_gport, 0);
  if (rv != BCM_E_NONE) {
      printf("bcm_vlan_gport_add returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

/* Subport Scheduler Hierarchy creation routine */
bcm_error_t subport_scheduler_config(int unit, bcm_port_t port, bcm_gport_t subport_gport)
{
  int i;

  /* Create L0 gport for the subport with 8 input (L1) */
  rv =  bcm_cosq_gport_add(unit, subport_gport, 8, BCM_COSQ_GPORT_SCHEDULER, &l0_sched);
  if (rv != BCM_E_NONE) {
      printf("bcm_cosq_gport_add for L0 returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  /* Attach L0 to Port */
  rv =  bcm_cosq_gport_attach(unit, l0_sched, port, 0);
  if (rv != BCM_E_NONE) {
      printf("bcm_cosq_gport_attach for L0 returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  /* Create 8 L1s, attach them to L0, Create UC and MC per L1 and attach */
  for(i = 0; i < 8; i++) {
     rv =  bcm_cosq_gport_add(unit, subport_gport, 2, BCM_COSQ_GPORT_SCHEDULER, l1_sched[i]);
     if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add for L1[%d] returned '%s'\n",i,bcm_errmsg(rv));
        return rv;
     }

     rv = bcm_cosq_gport_attach(unit, l1_sched[i], l0_sched, i);
     if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach for L1[%d] returned '%s'\n",i,bcm_errmsg(rv));
        return rv;
     }
     /* Create UCQ[i] and attach it to L1[i] as 0th child*/
     rv = bcm_cosq_gport_add(unit, subport_gport, 1, BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, uc_gport[i]);
     if (rv != BCM_E_NONE) {
         printf("bcm_cosq_gport_add for UCQ[%d] returned '%s'\n",i,bcm_errmsg(rv));
         return rv;
     }
     rv = bcm_cosq_gport_attach(unit, uc_gport[i], l1_sched[i], 0);
     if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach for UCQ[%d] returned '%s'\n",i,bcm_errmsg(rv));
        return rv;
     }

     /* Create MCQ[i] and attach it to L1[i] at 1st child */
     rv = bcm_cosq_gport_add(unit, subport_gport, 1, BCM_COSQ_GPORT_MCAST_QUEUE_GROUP, mc_gport[i]);
     if (rv != BCM_E_NONE) {
         printf("bcm_cosq_gport_add for MCQ[%d] returned '%s'\n",i,bcm_errmsg(rv));
         return rv;
     }
     rv = bcm_cosq_gport_attach(unit, mc_gport[i], l1_sched[i], 1);
     if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach for MCQ[%d] returned '%s'\n",i,bcm_errmsg(rv));
        return rv;
     }
  }
    return BCM_E_NONE;
}

bcm_error_t coe_config(bcm_vlan_t vlan)
{
  /* Create COE subport */
  rv = create_subport(vlan, egr_port, &subport_gport);
  if (rv != BCM_E_NONE) {
      printf("create_subport() returned '%s'\n",bcm_errmsg(rv));
      return rv;
   }

  /* Subport Scheduler Hierarchy creation */
  rv = subport_scheduler_config(unit, egr_port, subport_gport);
  if (rv != BCM_E_NONE) {
      printf("subport_scheduler_config() returned '%s'.\n", bcm_errmsg(rv));
      return rv;
  }

  /* SVTAG Encap configuration */
  rv = configure_flex_macsec_encoding(vlan, subport_gport);
  if (rv != BCM_E_NONE) {
      printf("configure_flex_macsec_encoding() returned '%s'\n",bcm_errmsg(rv));
      return rv;
   }

  /* SVTAG insertion config using EFP */
  rv = config_efp(unit);
  if (rv != BCM_E_NONE) {
      printf("config_efp() returned '%s'\n",bcm_errmsg(rv));
      return rv;
   }
  return BCM_E_NONE;
}

void verify(int unit)
{
  char str[512];

  bshell(unit, "hm ieee");
  bshell(unit, "vlan show");
  bshell(unit, "l3 intf show");
  bshell(unit, "l3 egress show");
  bshell(unit, "multicast show");
  bshell(unit, "ipmc table show");
  bshell(unit, "clear c");
  snprintf(str, 512, "tx 1 pbm=%d data=0x01005E0101010000000000118100000C08004500002E0000000040FFC324C0A81402E1010101000102030405060708090A0B0C0D0E0F10111213141516171819F620FE33; sleep quiet 1", ing_port);
  bshell(unit, str);
  bshell(unit, "ipmc table show");
  bshell(unit, "show c");

  return;
}

bcm_error_t execute()
{
    bshell(unit, "config show; a ; version");

    /* select port */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return rv;
    }

  /* vlan config */
  if (BCM_FAILURE((rv = vlan_config(vlan1, vlan2)))) {
      printf("vlan_config() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* COE config */
  if (BCM_FAILURE((rv = coe_config(vlan2)))) {
      printf("coe_config() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* L3 MC config */
  if (BCM_FAILURE((rv = l3_mc_config(subport_gport, vlan1, vlan2)))) {
      printf("l3_mc_config() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* verify the result */
  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
