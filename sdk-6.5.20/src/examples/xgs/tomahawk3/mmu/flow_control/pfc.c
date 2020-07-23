/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/* Feature Name   : PFC(Priority Flow control) Receive & Trasmit
 *
 * Usage          : BCM.0>cint pfc.c
 *
 * Config         : pfc_config.bcm
 *
 * Log file       : pfc_log.txt
 *
 * Test Topology  :
 *  
 * DMAC=0x1            +------------+         DMAC=0x1
 * SMAC=0x2   +-------->   56980    +-------> SMAC=0x2
 * VLAN=2,Pri=2  cd0(1)|            |cd1(2)   VLAN=2,Pri=2
 *                     | l2:mac=0x1 |
 *                     |    vlan=2  |
 *      <-------+      |    port=2  |     <-------+
 *   TX PFC FRAMES     |            |  RX PFC FRAMES
 *                     +------------+ 
 *
 *  Summary:
 *  ========
 *  This cint example demonstrates PFC feature on TH3
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one ingress and one egress port
 *    2) Step2 - Configuration (Done in pfc_config())
 *    ===============================================
 *      a) Sets up required data path for traffic and enables PFC Tx and Rx on
 *         ports
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) send at linerate to cd0 L2 learnt packets (vlan=1 prio=2 dmac=0x1 smac=0x2)
 *      b) Along with a), send at linerate to cd1 PFC frames with pause-control class 2 enabled
 *      c) Expected Result:
 *         ================
 *         For test case mentioned in a)
 *         On cd0 (Port-1): verify that PFC frames should be transmitted at regular intervals as
 *         egress port is congested.
 *         In 'show c' output, PFC frame Tx should be seen on cd0 (port-1)
 *
 *         On cd1 (Port-2): packets should egress at configured rate
 *
 *         For test case mentioned in b)
 *         Port cd1 should not egress any packets as pfc pasue frames are being
 *         received. In 'show c' output, there should not be any Tx on on cd1 (port-2)
 */
 
cint_reset();
int rv = 0;
int unit = 0;
bcm_port_t ing_port; /*Ingress port */
bcm_port_t egr_port; /*Egress port */

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

bcm_error_t common_data_path_init(int unit)
{
  bcm_vlan_t vid = 2;
  bcm_mac_t dest_mac = "00:00:00:00:00:01"; /*Destination MAC */
  bcm_pbmp_t pbmp,upbmp;
  bcm_l2_addr_t l2addr;

  rv = bcm_vlan_create(unit, vid);
  if (rv != BCM_E_NONE) {
      printf("[bcm_vlan_create] returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(upbmp);
  BCM_PBMP_PORT_ADD(pbmp, ing_port);
  BCM_PBMP_PORT_ADD(pbmp, egr_port);
  rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
  if (rv != BCM_E_NONE) {
      printf("bcm_vlan_port_add returned :%s.\n", bcm_errmsg(rv));
      return rv;
  }

  bcm_l2_addr_t_init(l2addr, dest_mac, vid);
  l2addr.port = egr_port;
  l2addr.flags |= BCM_L2_STATIC;
  rv = bcm_l2_addr_add(unit, &l2addr);
  if (rv != BCM_E_NONE) {
      printf("bcm_l2_addr_add returned :'%s'\n",bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t pfc_setup(int unit, bcm_port_t ing_port, bcm_port_t egr_port)
{
 uint32 limit_kbits_sec = 100000; /*Bandwidth limit on egress queue - 100 Mbps*/
 int enable = 1;
 int disable = 0;
 bcm_mac_t pause_smac = "00:11:22:33:44:55"; /*Source MAC on all PFC frames transmitted from a port */
 int num_cos = 8; /* Number of COS levels */
 int cos = 2; /*Priority of incoming packet on which testing is done */
 int pri; /* Used for iterations */

 /* pfc tx priotiry to pg mapping array */
 int pri_to_pg_array[8] = {0, 1, 2, 3, 4, 5, 6, 7}; 
 int profile_id = 1;
  
/* Configure num of COS levels in the system */
  rv = bcm_cosq_config_set(unit, num_cos);
  if (rv != BCM_E_NONE) {
      printf("bcm_cosq_config_set returned '%s'\n",bcm_errmsg(rv)); 
      return rv;
  }

/**** Egress queue rate limit settings *****
 **** Set the bandwidth limit on particular queue of egr port 
*/
  rv = bcm_cosq_gport_bandwidth_set(unit, egr_port, cos, limit_kbits_sec, limit_kbits_sec, 0);
  if (rv != BCM_E_NONE) {
      printf("bcm_cosq_gport_bandwidth_set returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

/**** Pause Settings ****/
  /* Set the source MAC for PFC Transmit frames */
  rv = bcm_port_pause_addr_set(unit, ing_port, pause_smac);
  if (rv != BCM_E_NONE) { 
      printf("bcm_port_pause_addr_set returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  /* Disable normal pause on ingress port */
  rv = bcm_port_pause_set(unit, ing_port, disable, disable);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_pause_set~ingress returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }

  /* Disable normal pause on egress port */
  rv = bcm_port_pause_set(unit, egr_port, disable, disable);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_pause_set~egress returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }


  /* set PFC-tx priority to PG mapping 1:1 mapping*/
  rv = bcm_cosq_priority_group_pfc_priority_mapping_profile_set(0, profile_id, 8, pri_to_pg_array);
  if (rv != BCM_E_NONE) {
     printf("bcm_cosq_priority_group_pfc_priority_mapping_profile_set returned '%s'\n",bcm_errmsg(rv));
     return rv;
  }
 
  /* Enable PFC Tx on ingress port and RX on egress port */
  rv = bcm_port_control_set(unit,ing_port, bcmPortControlPFCTransmit, 1);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_control_set~Tx returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  rv = bcm_port_control_set(unit,egr_port, bcmPortControlPFCReceive, 1);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_control_set~Rx returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t pfc_config(int unit, bcm_port_t ing_port, bcm_port_t egr_port)
{
  /* common data path setup */
  if (BCM_FAILURE((rv = common_data_path_init(unit)))) {
      printf("common_data_path_init() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* pfc config */
  if (BCM_FAILURE((rv = pfc_setup(unit, ing_port, egr_port)))) {
      printf("pfc_setup() failed. : %s. \n", bcm_errmsg(rv)); 
      return rv;
  }
  return BCM_E_NONE;
}

/* ing_port should send PFC frame towards Ixia
   egr_port should not egress anything as it receives PFC pause frames from Ixia.
 */
void verify(int unit)
{
  bshell(unit, "show c");
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit = 0;

  bshell(unit, "config show; a ; version");

  /* select port */
  if (BCM_FAILURE((rv = test_setup(unit)))) {
      printf("test_setup() failed.\n");
      return -1;
  }

  /* pfc setup */
  if (BCM_FAILURE((rv = pfc_config(unit, ing_port, egr_port)))) {
      printf("pfc_config() failed.\n");
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

