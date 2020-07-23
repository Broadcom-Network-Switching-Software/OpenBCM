/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : Tx and Rx Pause
 *
 *  Usage    : BCM.0>cint pause.c
 *
 *  config   : pause_config.bcm
 *
 *  Log file : pause_log.txt
 *
 *  Test Topology :
 *
 *  DMAC=0x1            +------------+         DMAC=0x1
 *  SMAC=0x2   +-------->   56980    +-------> SMAC=0x2
 *  VLAN=2        cd0(1)|            |cd1(2)   VLAN=2
 *                      | l2:mac=0x1 |
 *                      |    vlan=2  |
 *       <-------+      |    port=2  |     <-------+
 *    TX PAUSE FRAMES   |            |  RX PAUSE FRAMES
 *                      +------------+
 *  Summary:
 *  ========
 *  This cint example demonstrates pause feature on TH3
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one ingress and one egress port
 *    2) Step2 - Configuration (Done in pause_config())
 *    =================================================
 *      a) Sets up required data path for traffic and sets up Tx and Rx Pause on 
 *         ports
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Transmit l2 packet with dmac=0x1, vlan=1 at linerate to from Ixia
 *         to cd0(Port-1)
 *      b) Along with a), Transmit Pause frame(dmac=0x0180C2000001 smac=0x1 type=0x8808 opcode=0x0001)
 *         at linerate to cd1 (Port-2)
 *         (Pause frame: dmac=0x0180C2000001, smac=0x1, type=0x8808, opcode=0x0001)
 *      c) Expected Result:
 *         ================
 *         For test case mentioned in a)
 *         On cd0 (Port-1): verify that Pause frames should be transmitted at regular intervals as
 *         egress port is congested.
 *         In 'show c' output Pause frame Tx should be seen on cd0 (port-1)
 *
 *         On cd1 (Port-2): verify that packets egress at configured rate
 *
 *         For test case mentioned in b)
 *         On cd1(Port-2): verify that Port does not egress any packets as pause frames are being
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
 
bcm_error_t pause_tx_rx_config(int unit, bcm_port_t ing_port, bcm_port_t egr_port)
{
  uint32 limit_kbits_sec = 100000;
  uint32 limit_kbits_burst = 1000;
  int enable = 1;
  int disable = 0;
  bcm_mac_t pause_smac = "00:11:22:33:44:55";

  /****set egress port rate limit  *****/
  rv = bcm_port_rate_egress_set(unit, egr_port, limit_kbits_sec, limit_kbits_burst);
  if (rv != BCM_E_NONE) {
      printf("bcm_port_rate_egress_set returned '%s'\n",bcm_errmsg(rv)); 
      return rv;
  }

 /**** Set the pause frame's Source MAC ****/
 rv = bcm_port_pause_addr_set(unit,ing_port, pause_smac);
 if (rv != BCM_E_NONE) {
     printf("bcm_port_pause_addr_set returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }

 /**** Enable TX-Pause on ingress port ****/
 rv = bcm_port_pause_set(unit,ing_port,enable,disable);
 if (rv != BCM_E_NONE) {
     printf("bcm_port_pause_set-TX returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }

 /**** Enable RX-Pause on egress port ****/
 rv = bcm_port_pause_set(unit,egr_port,disable,enable);
 if (rv != BCM_E_NONE) {
     printf("bcm_port_pause_set-RX returned '%s'\n",bcm_errmsg(rv));
     return rv;
 }
 return BCM_E_NONE;
}

bcm_error_t pause_config(int unit, bcm_port_t ing_port, bcm_port_t egr_port)
{
  if (BCM_FAILURE((rv = common_data_path_init(unit)))) {
      printf("common_data_path_init() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  if (BCM_FAILURE((rv = pause_tx_rx_config(unit, ing_port, egr_port)))) {
      printf("pause_tx_rx_config() failed. : %s. \n", bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
} 

void verify(int unit)
{
  /* ing_port should send Pause frame towards Ixia
     egr_port should not egress anything as it receives pause frames from Ixia.
  */
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

  if (BCM_FAILURE((rv = pause_config(unit, ing_port, egr_port)))) {
      printf("pause_config() failed.\n");
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

