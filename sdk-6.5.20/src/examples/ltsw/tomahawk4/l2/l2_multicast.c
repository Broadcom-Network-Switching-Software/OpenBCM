/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/******************************************************************************
#
# Purpose: Demonstrate L2 multicast switching.
#
# Description: Packet with Destination MAC as 0x010000BBBBBB ingresses on port 1
# and gets multicast to port 2 - 3.
#
# Setup diagram:
#                +---------------------------+
#                |                           |
#                |                           |
#                |                           +------------------>
#                |   BCM56990                |      2
#                |                           +------------------>
#+-------------> |                           |      3
#           1    |   VLAN 2: Ports 1 - 4     |
#                |                           + 4
#                |                           |
#                |                           |
#                +---------------------------+
#
# Ingress port(s): 1.
#
# Egress ports(s): 2,3.
#
# Egress CoS queue: Not relevant.
#
# Device: bcm56990_a0.
#
# NGSDK Version: 1.1
#
# Notes:  Script is self contained i.e it configures all logical tables
# required to make L2 switching work.
#
# Verification method: Check port counters of Ports 1-4.
# Port 4 should not receive any packet, packet should be L2 multicast to port 2 - 3 only.
 */
cint_reset();
bcm_port_t ingress_port, egress_port1, egress_port2, egress_port3;
bcm_vlan_t vid = 2;
bcm_multicast_t l2mc_group = 0xf;
uint8 mac[6] = {0x01, 0x00, 0x00, 0xbb, 0xbb, 0xbb};

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
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
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

/*
 * This function is pre-test setup.
 */
int testSetup(int unit)
{
  int port_list[4];
  bcm_pbmp_t pbmp;

  bcm_l2_addr_t addr;
  int lb_mode = BCM_PORT_LOOPBACK_MAC;

  uint32_t flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID;
  bcm_gport_t gegress_port1, gegress_port2;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  ingress_port = port_list[0];
  egress_port1 = port_list[1];
  egress_port2 = port_list[2];
  egress_port3 = port_list[3];
  printf("Ingress port: %d\n",ingress_port);
  printf("Egress port1: %d\n",egress_port1);
  printf("Egress port2: %d\n",egress_port2);
  printf("Egress port3: %d\n",egress_port3);

  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, ingress_port);
  BCM_PBMP_PORT_ADD(pbmp, egress_port1);
  BCM_PBMP_PORT_ADD(pbmp, egress_port2);
  BCM_PBMP_PORT_ADD(pbmp, egress_port3);

  /* Create a vlan */
  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));

  /* Add vlan member ports */
  BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, pbmp));

  /* Set port default vlan id */
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vid));
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, vid));
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port2, vid));
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port3, vid));

  /* Set port mac loopback mode */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, lb_mode));
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port1, lb_mode));
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port2, lb_mode));
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port3, lb_mode));

  /* Add a L2 address on front panel port */
  bcm_l2_addr_t_init(&addr, mac, vid);
  addr.l2mc_group = l2mc_group;
  BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));

  BCM_IF_ERROR_RETURN(bcm_multicast_create(unit, flags, &l2mc_group));
  BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port1, &gegress_port1));
  BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port2, &gegress_port2));
  BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, l2mc_group, gegress_port1, -1));
  BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, l2mc_group, gegress_port2, -1));

  /* Drop P2,P3,P4 received packet to avoid packet being forwarded again */
  BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port1, 0));
  BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port2, 0));
  BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port3, 0));

  return BCM_E_NONE;
}

/*
 * Clean up pre-test setup.
 */
int testCleanup(int unit)
{
  bcm_pbmp_t pbmp;
  bcm_gport_t gegress_port1, gegress_port2;

  /* Set default vlan. */
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, 1));
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, 1));
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port2, 1));
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port3, 1));

  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, ingress_port);
  BCM_PBMP_PORT_ADD(pbmp, egress_port1);
  BCM_PBMP_PORT_ADD(pbmp, egress_port2);
  /* Delete vlan member ports. */
  BCM_IF_ERROR_RETURN(bcm_vlan_port_remove(unit, vid, pbmp));

  /* Delete a vlan. */
  BCM_IF_ERROR_RETURN(bcm_vlan_destroy(unit, vid));

  /* Traffic test is completed - recover ports as none loopback mode. */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_NONE));
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port1, BCM_PORT_LOOPBACK_NONE));
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port2, BCM_PORT_LOOPBACK_NONE));
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port3, BCM_PORT_LOOPBACK_NONE));

  /* Delete a L2 address on front panel port. */
  BCM_IF_ERROR_RETURN(bcm_l2_addr_delete(unit, mac, vid));

  BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port1, &gegress_port1));
  BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port2, &gegress_port2));
  BCM_IF_ERROR_RETURN(bcm_multicast_egress_delete(unit, l2mc_group, gegress_port1, -1));
  BCM_IF_ERROR_RETURN(bcm_multicast_egress_delete(unit, l2mc_group, gegress_port2, -1));
  BCM_IF_ERROR_RETURN(bcm_multicast_destroy(unit, l2mc_group));

  BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port1, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
  BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port2, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));
  BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port3, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD));

  BCM_IF_ERROR_RETURN(bcm_stat_clear(unit, ingress_port));
  BCM_IF_ERROR_RETURN(bcm_stat_clear(unit, egress_port1));
  BCM_IF_ERROR_RETURN(bcm_stat_clear(unit, egress_port2));
  BCM_IF_ERROR_RETURN(bcm_stat_clear(unit, egress_port3));

  return BCM_E_NONE;
}
/*
 * This function does the following.
 * Transmit the below packet on ingress_port and verify
 */
bcm_error_t testVerify(int unit)
{
  char   str[512];
  uint64 in_pkt, out_pkt;

  printf("Verify MC pkt from %d flooding to port %d, %d\n", ingress_port, egress_port1, egress_port2);

  snprintf(str, 512, "tx 1 pbm=%d data=0x010000bbbbbb000000aaaaaa08004500003c67620000ff01bd760a3a40010a3a42730000550a000100516162636465666768696a6b6c6d6e6f7071727374757677616263646566676869;sleep 1", ingress_port);
  bshell(unit, str);

  printf("Executing 'show c'\n");
  bshell(unit, "show c");

  printf("\n******Port stats check******\n");
  printf("--------Ingress port (%d) stats--------\n", ingress_port);
  COMPILER_64_ZERO(in_pkt);
  BCM_IF_ERROR_RETURN(bcm_stat_get(unit, ingress_port, snmpDot1dTpPortInFrames, &in_pkt));
  printf("Rx : %d packets\n", COMPILER_64_LO(in_pkt));

  printf("--------Egress port1 (%d) stats--------\n", egress_port1);
  COMPILER_64_ZERO(out_pkt);
  BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egress_port1, snmpDot1dTpPortOutFrames, &out_pkt));
  printf("Tx : %d packets\n", COMPILER_64_LO(out_pkt));
  if (COMPILER_64_LO(out_pkt) != 1) {
    printf("Packet verify failed. Expected out packet 1 but get %d\n", COMPILER_64_LO(out_pkt));
    return -1;
  }

  printf("--------Egress port2 (%d) stats--------\n", egress_port2);
  COMPILER_64_ZERO(out_pkt);
  BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egress_port2, snmpDot1dTpPortOutFrames, &out_pkt));
  printf("Tx : %d packets\n", COMPILER_64_LO(out_pkt));
  if (COMPILER_64_LO(out_pkt) != 1) {
    printf("Packet verify failed. Expected out packet 1 but get %d\n", COMPILER_64_LO(out_pkt));
    return -1;
  }

  printf("--------Egress port3 (%d) stats--------\n", egress_port3);
  COMPILER_64_ZERO(out_pkt);
  BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egress_port3, snmpDot1dTpPortOutFrames, &out_pkt));
  printf("Tx : %d packets\n", COMPILER_64_LO(out_pkt));
  if (COMPILER_64_LO(out_pkt) != 0) {
    printf("Packet verify failed. Expected out packet 0 but get %d\n", COMPILER_64_LO(out_pkt));
    return -1;
  }

  return BCM_E_NONE;
}

/*
 * This functions does the following
 * a)test setup *
 * b)demonstrates the functionality(done in testVerify()).
 */
bcm_error_t testExecute()
{
  bcm_error_t rv;
  int unit =0;

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
  rv = testCleanup(unit);
  if (BCM_FAILURE(rv)) {
    printf("testCleanup() failed\n");
    return -1;
  }
  printf("Completed test verify successfully.\n");

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print testExecute();
}
