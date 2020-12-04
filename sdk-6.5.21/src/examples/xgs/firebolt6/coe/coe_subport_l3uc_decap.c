/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : COE subport - L3 UC Decapsulation flow  
 *
 * Usage    : BCM.0> cint coe_subport_l3uc_decap.c
 *
 * config   : coe_config.bcm
 *
 * Log file : coe_subport_l3uc_decap_log.txt
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
 *      a) Inject packet with SVTAG from cpu destined to egress channelized port   
 *      b) Check the Received looped back packet on egress channelized port 
 *      c) Expected Result:
 *         ================
 *         Same packet should come back on channelized port without any additional SVTAG 
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

cint_reset();
int rv = 0;
int unit = 0;
bcm_port_t ing_port; /*Ingress port */
bcm_port_t egr_port; /*Egress port */
bcm_gport_t subport_gport = 0;

bcm_gport_t l0_sched;
bcm_gport_t l1_sched[8];
bcm_gport_t uc_gport[8];
bcm_gport_t mc_gport[8];

bcm_if_t nh_index[2];
int vlan1 = 21, vlan2 = 22;

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

bcm_error_t configure_v4_defroute(int unit, int vrf,  uint32 ip, uint32 subnet_mask, bcm_if_t nh_index)
{

  bcm_l3_route_t defroute;
  int status;
  int i = 0;

  bcm_l3_route_t_init(&defroute);
  defroute.l3a_subnet = ip & subnet_mask;
  defroute.l3a_ip_mask = subnet_mask;
  defroute.l3a_intf = nh_index;
  defroute.l3a_vrf = vrf;
  status =  bcm_l3_route_add(unit, &defroute);
  if (status < 0) {
      printf("bcm_l3_route_add failed for 0x%x !!! status = %d\n", ip, status);
      return status;
  }
  return BCM_E_NONE;
}

bcm_error_t configure_nexthop(int unit, int dest_port, int vid, unsigned char* smac, unsigned char* dmac, 
                              bcm_if_t *nh_index, int flags)
{
  bcm_l3_intf_t intf;
  bcm_l3_egress_t   egress_object;   
  int status;

  bcm_l3_intf_t_init(&intf);  

  sal_memcpy(intf.l3a_mac_addr, smac, 6);
  intf.l3a_vid =  vid;
  intf.l3a_mtu = 4096;
  intf.l3a_flags = BCM_L3_ADD_TO_ARL;
  intf.l3a_vrf = 10;
  status = bcm_l3_intf_create(unit, &intf);
  if (status < 0) {
      printf("bcm_l3_intf_create failed!!! status = %d\n",status);
      return status;
  }

  printf("bcm_l3_intf_create intf - %d\n", intf.l3a_intf_id);

  /* Now create the l3_egress object which gives the index to l3 interface during lookup */
  bcm_l3_egress_t_init(&egress_object);

  /* Now copy the nexthop destmac, set dest port and index of L3_INTF table which is created above */
  sal_memcpy(egress_object.mac_addr, dmac,6);
  egress_object.intf = intf.l3a_intf_id; 
  egress_object.port = dest_port;  
  egress_object.flags = flags; 
  /* go ahead and create the egress object */ 
  status = bcm_l3_egress_create(unit, 0, &egress_object, nh_index); 
  if (status < 0) {
      printf("1 bcm_l3_egress_create failed !!! status = %d\n",status);
      return status;    
  }
  return BCM_E_NONE;
}

bcm_gport_t create_vlan_and_add_ports(int unit, int vid, int tport, int uport)
{
  bcm_pbmp_t pbmp;
  bcm_pbmp_t upbmp;
  int status;

  status = bcm_vlan_create(unit, vid);
  if (status < 0 && status != -8) { /* -8 is EEXISTS*/
      return status;
  }

  BCM_PBMP_PORT_ADD(pbmp, tport);
  BCM_PBMP_PORT_ADD(upbmp, uport);
  status = bcm_vlan_port_add(unit, vid, pbmp,upbmp);
  if (status < 0) {
      printf("Failed to add ports to vlan %d with status = %d\n",vid, status);
      return status;
  }
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

  printf("flow handle = %d\n", handle); 
  rv = bcm_flow_option_id_get(unit, handle, "LOOKUP_DGPP_OVLAN_INTPRI_ASSIGN_CLASS_ID", &option_id);
  if (rv != BCM_E_NONE) {
      printf("bcm_flow_option_id_get returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  printf("flow option_id = %d\n", option_id); 

  encap_config.flow_handle = handle;
  encap_config.flow_option = option_id;
  encap_config.flow_port = subport_gport; /* Destination port number */
  encap_config.vlan = vlan;
  encap_config.pri=i;
  /* Make this a macsec enabled flow. */
  encap_config.flags |= BCM_FLOW_PORT_ENCAP_FLAG_MACSEC_ENCRYPT;
  /* Class id will carry the encap subport number. */
  encap_config.class_id = 15;
  encap_config.criteria = BCM_FLOW_ENCAP_CRITERIA_DGPP_VLAN_INT_PRI;
  encap_config.valid_elements = (BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_CLASS_ID_VALID | BCM_FLOW_ENCAP_VLAN_VALID);

  rv = bcm_flow_encap_add(unit, &encap_config, 0, &logical_fields);
  if (rv != BCM_E_NONE) {
      printf("bcm_flow_encap_add returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  bshell(0,"d EGR_VLAN_XLATE_2_SINGLE");
  return BCM_E_NONE;
}

bcm_error_t delete_subport(int unit, bcm_port_t subport_gport)
{

  bcm_subport_config_t subport_cfg;

  /* add subports to subport group */
  bcm_subport_config_t_init(&subport_cfg);

  /* Get the subport from port */
  rv = bcm_subport_port_get(unit, subport_gport, &subport_cfg);
  if (rv != BCM_E_NONE) {
      printf("bcm_subport_port_get returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  /* Delete the subport */ 
  rv = bcm_subport_port_delete(unit, subport_gport);
  if (rv != BCM_E_NONE) {
      printf("bcm_subport_port_delete returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

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
  int subtag_vlan = 15; /* Used to decap subtag's subport number. */

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
  printf("subport_gport created is 0x%x\n", *subport_gport);  

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

/* SVTAG insertion configuration using EFP */
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
  /* Create FP group */
  BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &group_config));

  /* Create FP Entry */
  BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, group_config.group, &entry));
  printf("efp entry =%d\n", entry);

  /* Qualify the egress port type as COE port to add SVTAG. */
  BCM_IF_ERROR_RETURN(bcm_field_qualify_EgressPortCtrlType(unit, entry, bcmFieldEgressPortCtrlTypeSubportSvtagEncrypt));

  /* Action to specify the SVTAG signature in outgoing packet */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionSvtagSignature, 0x11, 0));

  /* specify offset for linecard to know from where to start encryption if packet is to
     go through MACSEC encryption */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionSvtagOffset, 0xE8, 0));

  /* set priority in SVTAG of the outgoing packet */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionSvtagPrio, 0, 0));

  /* Assign subport number - can assign it through encap configuration per flow */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionSvtagSubportNum, 10, 0));

  /* Add subport tag in the out going packet */
  BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, entry, bcmFieldActionSvtagEnable, 0, 1));

  /* Installing the Entry */
  BCM_IF_ERROR_RETURN(bcm_field_entry_install(unit,entry));

  return BCM_E_NONE;
}

bcm_error_t vlan_config(int unit, int vlan1, int vlan2)
{
  rv = create_vlan_and_add_ports(unit, vlan1, ing_port, 0);
  if (rv != BCM_E_NONE) {
      printf("create_vlan_and_add_ports() returned '%s' for vlan %d\n",bcm_errmsg(rv), vlan1);
      return rv;
  }
  rv = create_vlan_and_add_ports(unit, vlan2, egr_port, 0);
  if (rv != BCM_E_NONE) {
      printf("create_vlan_and_add_ports() returned '%s' for vlan %d\n",bcm_errmsg(rv), vlan2);
      return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t configure_l3(bcm_gport_t subport_gport, int vlan1, int vlan2)
{
  unsigned char src_mac[] = {0x00,0x00,0x00,0x00,0xaa,0xaa};
  unsigned char dst_mac[] = {0x00,0x00,0x00,0x00,0xbb,0xbb};
  int i,x, vrf = 0;

  bcm_switch_control_set(unit,bcmSwitchL3EgressMode,1);

  rv = configure_nexthop(unit, ing_port, vlan1,  src_mac, dst_mac, &nh_index[0], 0);
  if (rv != BCM_E_NONE) {
      printf("configure_nexthop() returned '%s' for ing_port: %d\n",bcm_errmsg(rv), ing_port);
      return rv;
  }
  printf("nexthop created with index  =  0x%x\n", nh_index[0]);

  /* Create egress object with the subport gport */
  rv = configure_nexthop(unit, subport_gport, vlan2,  src_mac, dst_mac, &nh_index[1], 0);
  if (rv != BCM_E_NONE) {
      printf("configure_nexthop() returned '%s' for subport_gport: %d\n",bcm_errmsg(rv), subport_gport);
      return rv;
  }

  rv = configure_v4_defroute(unit, 10,  0x0a000001, 0xfffffff0, nh_index[1]);
  if (rv != BCM_E_NONE) {
      printf("configure_v4_defroute():1 returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  rv = configure_v4_defroute(unit, 10,  0xc0000005, 0xfffffff0, nh_index[1]);
  if (rv != BCM_E_NONE) {
      printf("configure_v4_defroute():2 returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  return BCM_E_NONE;
}

bcm_error_t coe_config(int vlan)
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

bcm_error_t tx_ipv4_packet_from_cpu(int unit, int vlan_tag,
                                bcm_mac_t src_mac, bcm_mac_t dst_mac,
                                bcm_ip_t src_ip, bcm_ip_t dst_ip, bcm_port_t dest_port, bcm_gport_t subport)
{   
  bcm_pkt_t *pkt;
  int pkt_size = 72; /* including VLAN Tag if BCM_PKT_F_NO_VTAG = 0 */
  int flags = BCM_TX_CRC_APPEND;

  /* EtherType(2B) Version+IHL+TOS(2B) Total Length (2B) + Identification (2B)
     Flags+Fragment offset(2B) TTL(1B: TTL=0x10) Protocol(1B: UDP=0x11) Header Checksum(2B) */
  char ipv4_ether_type[2] = {0x08, 0x00};
  char ether_type[2] = {0x81, 0x00};
  char vlan[2] = {0x00, 0x16};
  /* ipv4.total_length = 68 - 12 - 2 - 4 - 4 = 46; */
  char ipv4_hdr[24]    = {0x45, 0x00, 0x00, 0x2E,
                          0x00, 0x00, 0x00, 0x00,
                          0x40, 0xFF, 0xDA, 0x7E,
                          0xC0, 0xA8, 0x0A, 0x01,
                          0xC0, 0xA8, 0x14, 0x01};

  rv = bcm_pkt_alloc(unit, pkt_size, flags, &pkt);
  if (rv != BCM_E_NONE) { 
      printf("TX: Failed to allocate packets\n");
      return rv;
  }

  printf("Start setting pkt data ... \n");
  /* packet init */
  sal_memset(pkt->_pkt_data.data, 0x00, pkt_size);

  /* Start setting pkt data DMAC, SMAC */ 
  BCM_PKT_HDR_DMAC_SET(&pkt, dst_mac);
  BCM_PKT_HDR_SMAC_SET(&pkt, src_mac);
  /* Set svtag */;
  BCM_PKT_HDR_TPID_SET(&pkt, 0x8847);
  BCM_PKT_HDR_VTAG_CONTROL_SET(&pkt, 0xf);
  /* Set vlan and ip header */
  sal_memcpy(pkt->pkt_data->data+16, ether_type, 2);
  sal_memcpy(pkt->pkt_data->data+18, vlan, 2);
  sal_memcpy(pkt->pkt_data->data+20, ipv4_ether_type, 2);
  sal_memcpy(pkt->pkt_data->data+22, ipv4_hdr, 24);
  BCM_PKT_PORT_SET(&pkt, dest_port, 0, 0);
  pkt->dst_gport = subport;

  pkt->call_back = NULL;

  rv = bcm_tx(unit, pkt, NULL);
  if (rv != BCM_E_NONE) {
      printf("bcm tx error\n");
      bcm_pkt_free(unit, pkt);
      return rv;
  }
  bcm_pkt_free(unit, pkt);
  return BCM_E_NONE;
}

void verify(int unit)
{
  unsigned char src_mac[] = {0x00,0x00,0x00,0x00,0xaa,0xaa};
  unsigned char dst_mac[] = {0x00,0x00,0x00,0x00,0xbb,0xbb};
  char str[512];

  bshell(unit, "vlan show");
  bshell(unit, "l3 intf show");
  bshell(unit, "l3 egress show");
  bshell(unit, "l3 defip show");
  bshell(unit, "clear c");
  tx_ipv4_packet_from_cpu(unit, vlan2, src_mac, dst_mac, 0x010aa8c0, 0x0114a8c0, egr_port, subport_gport);
  bshell(unit, "l3 defip show");
  bshell(unit, "sleep 1");
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
  if (BCM_FAILURE((rv = vlan_config(unit, vlan1, vlan2)))) {
      printf("vlan_config() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* COE config */
  if (BCM_FAILURE((rv = coe_config(vlan2)))) {
      printf("coe_config() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* L3 config */
  if (BCM_FAILURE((rv = configure_l3(subport_gport, vlan1, vlan2)))) {
      printf("configure_l3() failed.: %s.\n", bcm_errmsg(rv));
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
