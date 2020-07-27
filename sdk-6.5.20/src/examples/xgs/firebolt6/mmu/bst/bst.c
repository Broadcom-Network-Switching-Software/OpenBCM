/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : BST
 *
 *  Usage    : BCM.0> cint bst.c
 *
 *  config   : bst_config.bcm
 *
 *  Log file : bst_log.txt
 *
 *  Test Topology : Single Firebolt6 device
 *
 *  Summary:
 *  ========
 *  This cint example illustrates configuration of Buffer Statistics Tracking (BST) mechanism
 *  to aid in resource monitoring and buffer allocation tuning.
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one egress port on which bst stat has to be tested.
 *    2) Step2 - Configuration (Done in bst_config())
 *    ===============================================
 *      a) Enable device level BST, BST tracking mode and snapshot view for
 *         THDO, THDI and CFAP using switch controls - bst_global_setup
 *      b) Set BST profile for specified mmu resource - bcmBstStatIdUcast in this
 *         example
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Check the Bst stats for the specified BST object - bcmBstStatIdUcast 
 *      b) Expected Result:
 *         ================
 *         If profile threshold is reached, snapshot is triggered. We see non-zero stats in value1 and value2
 *         (It is assumed that traffic path setup between ingress and egress port and egress port is
 *         congested so that shared buffer usage exceeds the profile threshold in this example)
 */

cint_reset();

uint32 unit = 0;
int rv=0;
bcm_port_t ing_port; /*Ingress port */
bcm_port_t egr_port; /*Egress port */
bcm_gport_t port_gport;	
uint64 value1 = "0x00000000:00000000";
uint64 value2 = "0x00000000:00000000";
int vlan=10;
int rate_kbps=100;
int burst_kbits=100;

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
 * Configures the port in loopback mode 
 */

bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));
  return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and apply port shaper
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));
  BCM_IF_ERROR_RETURN(bcm_port_rate_egress_set(unit, port, rate_kbps, burst_kbits));  
  return BCM_E_NONE;
}

/*
 * This functions gets the port numbers and sets up ingress and
 * egress ports.
 */
bcm_error_t test_setup(int unit)
{
  int port_list[2], i;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
      printf("portNumbersGet() failed\n");
      return -1;
  }

  /* Get port gport */
  ing_port = port_list[0];
  egr_port = port_list[1];

  if (BCM_E_NONE != ingress_port_setup(unit, ing_port)) {
      printf("ingress_port_setup() failed for port %d\n", ing_port);
      return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, egr_port)) {
      printf("egress_port_setup() failed for port %d\n", egr_port);
      return -1;
  }

  if (BCM_E_NONE != bcm_port_gport_get(unit, egr_port, &port_gport)) {
      printf("bcm_port_gport_get() failed.\n");
      return -1;
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
      printf("bcm_l2_addr_add returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

bcm_error_t l2_datapath_config(int unit, int vlan, bcm_port_t port)
{
  unsigned char src_mac[] = {0x00,0x00,0x00,0x00,0xaa,0xaa};
  bcm_mac_t mac_s;

  sal_memcpy(mac_s, src_mac, 6);

  rv = create_vlan_and_add_ports(unit, vlan, ing_port, 0);
  if (rv != BCM_E_NONE) {
      printf("create_vlan_and_add_ports() returned '%s' for vlan %d\n",bcm_errmsg(rv), vlan1);
      return rv;
  }

  rv = create_vlan_and_add_ports(unit, vlan, egr_port, 0);
  if (rv != BCM_E_NONE) {
      printf("create_vlan_and_add_ports() returned '%s' for vlan %d\n",bcm_errmsg(rv), vlan1);
      return rv;
  }

  rv = add_to_l2_table(unit, mac_s, vlan, egr_port); 
  if (rv != BCM_E_NONE) {
      printf("add_to_l2_table() returned '%s'\n",bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

/* Enable BST, Set BST tracking mode, Enable snapshot view for 
 * THDO, THDI and CFAP
 */
bcm_error_t bst_global_setup(int unit)
{
   BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchBstEnable, TRUE));
   BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchBstTrackingMode , 0));
   BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchBstSnapshotEnable, 0x7)); /* CFAP,THDI,THDO */
   return BCM_E_NONE;
}

/* Set BST profile for object bcmBstStatIdUcast */
bcm_error_t bst_profile_set(int unit, bcm_gport_t port_gport)
{
 bcm_cosq_bst_profile_t profile;
 profile.byte=200;

 /* Set BST profile for object bcmBstStatIdUcast for specified port */
 rv = bcm_cosq_bst_profile_set(unit, port_gport,0, bcmBstStatIdUcast, profile); 
 if (rv != BCM_E_NONE) {
      printf("Error in bcm_cosq_bst_profile_set:%s.\n", bcm_errmsg(rv));
      return rv;
 }

 return BCM_E_NONE;
}

bcm_error_t bst_stat_get(int unit, bcm_gport_t port_gport)
{
   rv = bcm_cosq_bst_stat_get(unit, port_gport, 0, bcmBstStatIdUcast, 0, &value1);
   if (rv != BCM_E_NONE) {
       printf("Error in bcm_cosq_bst_stat_get:%s.\n", bcm_errmsg(rv));
       return rv;
 }

 rv = bcm_cosq_bst_stat_sync(unit, bcmBstStatIdUcast);
 if (rv != BCM_E_NONE) {
     printf("Error in bcm_cosq_bst_stat_sync:%s.\n", bcm_errmsg(rv));
     return rv;
 }
 rv = bcm_cosq_bst_stat_get(unit, port_gport, 0, bcmBstStatIdUcast, 0, &value2);
 if (rv != BCM_E_NONE) {
     printf("Error in bcm_cosq_bst_stat_sync:%s.\n", bcm_errmsg(rv));
     return rv;
 }
 printf("BST stat - bcmBstStatIdUcast:value1 = 0x%x:0x%x\n", COMPILER_64_HI(value1),COMPILER_64_LO(value1));
 printf("BST stat - bcmBstStatIdUcast:value2 = 0x%x:0x%x\n", COMPILER_64_HI(value2),COMPILER_64_LO(value2)); 

 return BCM_E_NONE;
}

bcm_error_t bst_config(int unit, bcm_gport_t port_gport)
{
  /* BST setup */
  if (BCM_FAILURE((rv = bst_global_setup(unit)))) {
      printf("bst_global_setup() failed. : %s. \n", bcm_errmsg(rv)); 
      return rv;
  }

  if (BCM_FAILURE((rv = bst_profile_set(unit, port_gport)))) {
      printf("bst_stat_get_on_port() failed. : %s. \n", bcm_errmsg(rv));
      return rv;
  }
  return BCM_E_NONE;
}

/* Function to verify the result */
void verify(int unit)
{
  char   str[512];
  bshell(unit, "hm ieee");
  bshell(unit, "clear c");
  snprintf(str, 512, "tx 100000 len=500 pbm=%d data=0x00000000AAAA00000000BBBB8100000A08004500002E0000000040FFAFCB0A000001C0000005000102030405060708090A0B0C0D0E0F10111213141516171819", ing_port);
  bshell(unit, str);
  bst_stat_get(unit, port_gport);
  printf("Executing 'show c'\n");
  bshell(unit, "show c");
  bshell(unit, "l2 show");
  bst_stat_get(unit, port_gport);
}

bcm_error_t execute()
{
  bcm_error_t rv;
  int unit =0;

  bshell(unit, "config show; a ; version");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
      printf("test_setup() failed.\n");
      return rv;
  }

  /* l2 Data path setup for traffic test */
  if (BCM_FAILURE((rv = l2_datapath_config(unit, vlan, egr_port)))) {
      printf("vlan_config() failed.: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /* BST setup */
  if (BCM_FAILURE((rv = bst_config(unit, port_gport)))) {
      printf("bst_config() failed.\n");
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
