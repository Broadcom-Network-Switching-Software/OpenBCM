/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*##############################################################################
#
# Purpose: Demonstrate ingress mirroring.
#
# Description: Packet with destination MAC as 0x00BBBBBBBBBB ingresses on
#              port 2 and gets switched to port 61.
#              Enabled ingress mirroring to VxLAN tunnel on port 2 and configured 
#              MTP port as port 140.
#              VxLAN packet will be sent to the MTP port.
# Setup diagram:
#                +---------------------------+
#                |                           |
#                |                           |
#                |   BCM56880                |
#                |                           +------------------>
#+-------------> |                           | Port 61
#     Port 2     |   VLAN 2: Ports 1 - 3     |
#                |                           |------------------>
#                |                           | Port 140 - Vxlan tunnel port(MTP port)
#                |                           | tunnel header + Vxlan header + payload
#                +---------------------------+
#
# Ingress port(s): 2.
#
# Egress ports(s): 61.
#
# Egress CoS queue: Not relevant.
#
# Device: bcm56880_a0.
#
# SDK: sdk-6.5.18-EA6.
#
# Configuration File:
#     bcm56880_a0-generic-32x400.config.yml
#
##############################################################################*/
cint_reset();
int ingress_port, egress_port1, egress_port2, cpu_port = 0;
bcm_gport_t mirror_dest_id;
uint32 flags = BCM_MIRROR_PORT_INGRESS;

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

bcm_vlan_t vxlan_vid = 200;

bcm_error_t vxlan_mirror_destination_create(int unit, bcm_gport_t mirror_dest_gport,
		                                     bcm_gport_t *mirror_dest_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_mirror_destination_t mirror_dest_info;

    /* VxLAN related parameters */
    bcm_mac_t vxlan_src_mac = {0x00, 0x00, 0x00, 0x00, 0x06, 0x66};
    bcm_mac_t vxlan_dst_mac = {0x00, 0x00, 0x00, 0x00, 0x09, 0x99};
    uint16 vxlan_tpid = 0x8100;
    bcm_ip_t vxlan_src_ip = 0x06060606;
    bcm_ip_t vxlan_dst_ip = 0x09090909;
    uint16 vxlan_udp_src_port = 0xffff;
    uint16 vxlan_udp_dst_port = 4789;
    //uint16 vxlan_udp_dst_port = 8472;
    uint32 vxlan_vni = 0x654321;

    bcm_mirror_destination_t_init(mirror_dest_info);
    mirror_dest_info.gport = mirror_dest_gport;
    mirror_dest_info.flags2 = BCM_MIRROR_DEST_FLAGS2_TUNNEL_VXLAN;
    sal_memcpy(mirror_dest_info.src_mac, vxlan_src_mac, 6);
    sal_memcpy(mirror_dest_info.dst_mac, vxlan_dst_mac, 6);
    mirror_dest_info.vlan_id = vxlan_vid;
    mirror_dest_info.tpid = vxlan_tpid;
    mirror_dest_info.version = 4;
    mirror_dest_info.src_addr = vxlan_src_ip;
    mirror_dest_info.dst_addr = vxlan_dst_ip;
    mirror_dest_info.ttl = 16;
    mirror_dest_info.udp_src_port = vxlan_udp_src_port;
    mirror_dest_info.udp_dst_port = vxlan_udp_dst_port;
    mirror_dest_info.vni = vxlan_vni;

    BCM_IF_ERROR_RETURN(bcm_mirror_destination_create(unit, mirror_dest_info));
    *mirror_dest_id = mirror_dest_info.mirror_dest_id;
    print *mirror_dest_id;

    return rv;
}

/*
 * This function is pre-test setup.
 */
int testSetup(int unit)
{
  bcm_vlan_t vid = 2;
  uint8 dest_mac[6] = {0x00, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB};
  bcm_pbmp_t pbmp;
  bcm_pbmp_t ubmp;
  bcm_l2_addr_t addr;
  int lb_mode = BCM_PORT_LOOPBACK_MAC;
  bcm_gport_t gport;
  bcm_switch_trace_event_mon_t trace;

  int port_list[3];
  if (BCM_E_NONE != portNumbersGet(unit, port_list, 3)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }
  ingress_port = port_list[0];
  egress_port1 = port_list[1];
  egress_port2 = port_list[2];
  printf("Ingress port: %d\n",ingress_port);
  printf("Egress  port: %d\n",egress_port1);
  printf("Mirrored port: %d\n",egress_port2);

  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_PORT_ADD(pbmp, egress_port1);
  BCM_PBMP_PORT_ADD(pbmp, ingress_port);

  /* Create a vlan */
  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
  /* Add vlan member ports */
  BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, ubmp));
  
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, cpu_port);
  BCM_PBMP_PORT_ADD(pbmp, egress_port2);
  BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vxlan_vid));
  BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vxlan_vid, pbmp, ubmp));

  /* Set port default vlan id */
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port1, vid));
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, ingress_port, vid));
  BCM_IF_ERROR_RETURN(bcm_port_untagged_vlan_set(unit, egress_port2, vxlan_vid));

  /* Set port mac loopback mode */
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port1, lb_mode));
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, lb_mode));
  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port2, lb_mode));

  /* Add a L2 address on front panel port */
  bcm_l2_addr_t_init(&addr, dest_mac, vid);
  addr.port=egress_port1;
  addr.flags |= BCM_L2_STATIC;
  BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &addr));

  /* Drop packet. */
  BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port1, 0));
  BCM_IF_ERROR_RETURN(bcm_port_learn_set(unit, egress_port2, BCM_PORT_LEARN_CPU));

  /* Set COPY_TO_CPU action for CML trace event. */
  bcm_switch_trace_event_mon_t_init(&trace);
  trace.trace_event = bcmPktTraceEventIngressCmlFlags;
  trace.actions = BCM_SWITCH_MON_ACTION_COPY_TO_CPU;
  BCM_IF_ERROR_RETURN(bcm_switch_trace_event_mon_set(unit, &trace));

  BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egress_port2, &gport));

  /* Mirror Port */
  BCM_IF_ERROR_RETURN(vxlan_mirror_destination_create(unit, gport, &mirror_dest_id));
  
  /* Add a mirror destination to a source port. */
  BCM_IF_ERROR_RETURN(bcm_mirror_port_dest_add(unit, ingress_port, flags, mirror_dest_id));

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
  printf("Packet comes into ingress port %d, export forward to %d and mirror to %d\n", ingress_port, egress_port1, egress_port2);
  snprintf(str, 512, "tx 1 pbm=%d data=0x00BBBBBBBBBB0011223344558100000208004500001C000000004002F95DC0A8001EC0A800141164EE9B00000000000102030405060708090A0B0C0D0E0F1011E544D79B;sleep 5", ingress_port);
  printf("%s\n", str);
  bshell(unit, str);

  bshell(unit, "show c");
  bshell(unit, "pw stop");

  printf("\n******Port stats check******\n");
  printf("--------Ingress port (%d) stats--------\n", ingress_port);
  COMPILER_64_ZERO(in_pkt);
  BCM_IF_ERROR_RETURN(bcm_stat_get(unit, ingress_port, snmpDot1dTpPortInFrames, &in_pkt));
  printf("Rx : %d packets\n", COMPILER_64_LO(in_pkt));

  printf("--------Egress port (%d) stats--------\n", egress_port1);
  COMPILER_64_ZERO(out_pkt);
  BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egress_port1, snmpDot1dTpPortOutFrames, &out_pkt));
  printf("Tx : %d packets\n", COMPILER_64_LO(out_pkt));
  if (COMPILER_64_LO(out_pkt) != 1) {
    printf("Packet verify failed. Expected out packet 1 but get %d\n", COMPILER_64_LO(out_pkt));
    return -1;
  }

  printf("--------Mirrored port (%d) stats--------\n", egress_port2);
  COMPILER_64_ZERO(out_pkt);
  BCM_IF_ERROR_RETURN(bcm_stat_get(unit, egress_port2, snmpDot1dTpPortOutFrames, &out_pkt));
  printf("Tx : %d packets\n", COMPILER_64_LO(out_pkt));
  if (COMPILER_64_LO(out_pkt) != 1) {
    printf("Packet verify failed. Expected out packet 1 but get %d\n", COMPILER_64_LO(out_pkt));
    return -1;
  }

  return BCM_E_NONE;
}

/*
 * Cleanup test setup
 */
bcm_error_t testCleanup(int unit)
{
  int rv;

  /* Delete mirror dest. */
  rv = bcm_mirror_port_dest_delete(unit, ingress_port, flags, mirror_dest_id);
  if (BCM_FAILURE(rv)) {
    printf("bcm_vlan_stat_detach %s\n", bcm_errmsg(rv));
    return rv;
  }

  /* Destroy mirror destination. */
  rv = bcm_mirror_destination_destroy(unit, mirror_dest_id);
  if (BCM_FAILURE(rv)) {
    printf("bcm_flexctr_action_destroy %s\n", bcm_errmsg(rv));
    return rv;
  }

  return rv;
}

/*
 * This functions does the following
 * a)test setup *
 * b)demonstrates the functionality(done in testVerify()).
 * c)clen up
 */
bcm_error_t testExecute()
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
    printf("testCleanup() failed, return %d.\n", rv);
    return -1;
  }

  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print testExecute();
}
