/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : COE subport - Scheduler Hierarchy  
 *
 * Usage    : BCM.0> cint subport_sched_hqos.c 
 *
 * config   : coe_config.bcm 
 * 
 * Log file : subport_sched_hqos_log.txt
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
 *      a) Select one one egress port on which subport has to be created
 *    2) Step2 - Configuration (Done in coe_config())
 *    ===============================================
 *      a) Creates COE subport
 *      b) Creates subport scheduler hierarchy 
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Check the scheduler hierarchy created for subport 
 *      c) Expected Result:
 *         ================
 *         hsp pbm=<egress port> subport=<subport no> should show created scheduler hierarchy for subport  
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

bcm_error_t test_setup(int unit)
{
  int port_list[8], i;
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 8)) {
      printf("portNumbersGet() failed\n");
      return -1;
  }

  ing_port = port_list[0];
  egr_port = port_list[6];  /* xe6 -logical port 7 */

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

  int subtag_vlan = 4;
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


bcm_error_t add_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port)
{
  bcm_l2_addr_t l2_addr;

  bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
  l2_addr.flags = BCM_L2_STATIC;
  l2_addr.port = port;
  rv =  bcm_l2_addr_add(unit, &l2_addr);
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

bcm_error_t coe_config(int unit, bcm_port_t egr_port)
{
  int vlan=21;

  if (BCM_FAILURE((rv = create_vlan_and_add_ports(unit, vlan, egr_port, 0)))) {
      printf("create_vlan_and_add_ports() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }
 
  /* COE subport creation */
  if (BCM_FAILURE((rv = create_subport(vlan, egr_port, &subport_gport)))) {
      printf("create_subport() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }
 
  /* Subport Scheduler Hierarchy creation */
  if (BCM_FAILURE((rv = subport_scheduler_config(unit, egr_port, subport_gport)))) {
      printf("subport_scheduler_config() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

void verify(int unit)
{
  char str[512];
  printf("Subport Hierarchy on COE port %d\n", egr_port);
  snprintf(str, 512, "hsp pbm=%d subport=1", egr_port);
  bshell(unit, str);
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;

  bshell(unit, "config show; a ; version");

  /* select port */
  if (BCM_FAILURE((rv = test_setup(unit)))) {
      printf("test_setup() failed.\n");
      return rv;
  }

  /* COE setup */
  if (BCM_FAILURE((rv = coe_config(unit, egr_port)))) {
      printf("coe_config() failed.\n");
      return -1;
  }

  /* verify the result */
  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}
