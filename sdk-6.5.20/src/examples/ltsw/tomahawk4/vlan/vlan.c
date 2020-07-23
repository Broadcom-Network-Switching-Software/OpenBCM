/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Purpose: Demonstrate basic vlan function.
 *
 * Description:
 *     Description: Packet with Destination MAC as 00:00:03:00:01:FF ingresses
 *     on ingress port and gets switched to port egress port.
 *
 * Usage : BCM.0> cint vlan.c
 *
 * Config : bcm56990_a0-generic-64x400.config.yml &
 *     bcm56990_a0-generic-system_port.config.yml
 */
cint_reset();
bcm_port_t port_1, port_2;

/*
 * This function checked if given port/index is already present
 * in the list so that same ports number is not generated again
*/
int checkPortAssigned(int *port_index_list,int no_entries, int index)
{
  int i = 0;

  for (i = 0; i < no_entries; i++) {
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
  int i = 0, port = 0, rv = 0, index = 0;
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
      printf("	() failed \n");
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
  bcm_vlan_t vid = 2;
  bcm_vlan_t pkt_outer_vlan = 2;

  bcm_pbmp_t pbmp;
  bcm_pbmp_t ubmp;
  int port_list[2];

  bcm_l2_addr_t addr;
  bcm_mac_t dst_mac = "00:00:03:00:01:FF";
  bcm_mac_t src_mac = "00:00:03:00:00:FF";
  int lb_mode = BCM_PORT_LOOPBACK_MAC;
  bcm_vlan_action_set_t action;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }
  port_1 = port_list[0];
  port_2 = port_list[1];

  /* Create a VLAN forwarding domain */
  print bcm_vlan_create(unit, vid);

  /* Add vlan member ports */
  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, port_1);
  BCM_PBMP_PORT_ADD(pbmp, port_2);
  print bcm_vlan_port_add(unit, vid, pbmp, ubmp);

  /* Set default outer tpid per port */
  print bcm_port_tpid_set(unit, port_1, 0x8100);
  print bcm_port_tpid_set(unit, port_2, 0x8100);

  /* Set port mac loopback mode */
  print bcm_port_loopback_set(unit, port_1, lb_mode);
  print bcm_port_loopback_set(unit, port_2, lb_mode);

  /* Add a L2 address on front panel port */
  bcm_l2_addr_t_init(&addr, dst_mac, vid);
  addr.port = port_2;
  addr.flags |= BCM_L2_STATIC;
  print bcm_l2_addr_add(unit, &addr);

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

  /* Start traffic. */
  bshell(unit, "pw start");
  bshell(unit, "pw report all");

  /* Send pacekt to ingress_port */
  printf("\n******************************\n");
  printf("******Traffic test start******\n");
  printf("Packet comes into ingress port %d, expect to forward to port %d\n", port_1, port_2);
  snprintf(str, 512, "tx 1 pbm=%d data=0x0000030001FF0000030000FF81002002910000145555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555;sleep 5", port_1);
  printf("%s\n", str);
  bshell(unit, str);
  bshell(unit, "show c");
  bshell(unit, "pw stop");
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
  printf("Completed test verify successfully.\n");

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print testExecute();
}
