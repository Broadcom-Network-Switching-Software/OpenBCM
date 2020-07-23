/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 /* Feature  : L2 QoS
 *
 *  Usage    : BCM.0> cint l2_qos.c
 *
 *  config   : vlan_config.bcm
 *
 *  Log file : l2_qos_log.txt
 *  
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *  ingress_port     |                              |  egress_port
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *  Summary:
 *  ========
 *    This cint example configures mapping between {L2 packet Priority + CFI bit} and
 *    {internal priority and CNG} using BCM APIs. It also shows how internal priority
 *    and CNG can be used to remark out going packet's priority and CFI fields.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Selects one ingress port and one egress port and configure them in Loopback mode.
 *         Install a rule to copy incoming packets to CPU and start packet watcher.
 *
 *    2) Step2 - Configuration (Done in l2_qos()):
 *    ============================================
 *      a) Creates a VLAN(20) and add ingress port and  egress port as members.
 *
 *      b) Configure ingress port's default VLAN as 20, default packet priority as 4 and CFI as 1.
 *
 *      c) Configure ingress QoS mapping as below and attach it to ingress_port.
 *               {packet priority(4) + CFI(1)} is mapped to {internal_priority(10) + CNG(Yellow)}.
 *
 *      d) Configure egress QoS mapping as below and attach it to egress_port.
 *               {{internal_priority(10) + CNG(Yellow)} is mapped {packet priority(2) + CFI(0)}.
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Send the below untagged packet on ingress port.
 *        
 *        Packet:
 *        =======
 *        Ethernet header: DA=00:00:00:00:00:02, SA=00:00:00:00:00:01
 *
 *        0000 0000 0002 0000 0000 0001 0001 0203
 *        0405 0607 0809 0A0B 0C0D 0E0F 1011 1213
 *        1415 1617 1819 1A1B 1C1D 1E1F 2021 2223
 *        2425 2627 2829 2A2B 2C2D 2E2F DF52 E538
 *
 *      b) Expected Result:
 *      ===================
 *        The packet going out of egress_port has packet priority as "2" and CFI as "0".
 *        Below is the packet that egresses out of egress_port.
 *
 *        Packet:
 *        =======
 *        Ethernet II, Src: 00:00:00_00:00:01 (00:00:00:00:00:01), Dst: 00:00:00_00:00:02 (00:00:00:00:00:02)
 *        802.1Q Virtual LAN, PRI: 2, CFI: 0, ID: 20
 *        010. .... .... .... = Priority: Spare (2)
 *          ...0 .... .... .... = CFI: Canonical (0)
 *            .... 0000 0001 0100 = ID: 20
 *
 *        0000 0000 0002 0000 0000 0001 8100 4014
 *        0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
 *        1011 1213 1415 1617 1819 1a1b 1c1d 1e1f
 *        2021 2223 2425 2627 2829 2a2b 2c2d 2e2f
 *        df52 e538
 */
/* Reset c interpreter*/
cint_reset();
bcm_port_t ingress_port, egress_port;

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
 * Configures the port in loopback mode and installs
 * an IFP rule. This IFP rule copies the packets ingressing
 * on the specified port to CPU.
 */
bcm_error_t ingress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
  bcm_field_group_t group;
  bcm_field_entry_t entry;

  BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
  
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
 * on the specified port to CPU and drop the packets. This is
 * to avoid continuous loopback of the packet.
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
  bcm_field_qset_t  qset;
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
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
  int port_list[2];

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  ingress_port = port_list[0];
  egress_port = port_list[1];

  if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
    printf("ingress_port_setup() failed for port %d\n", ingress_port);
    return -1;
  }

  if (BCM_E_NONE != egress_port_setup(unit, egress_port)) {
      printf("egress_port_setup() failed for port %d\n", egress_port);
      return -1;
  }

  bshell(unit, "pw start report +raw +decode");
  return BCM_E_NONE;
}

/* Setup port configuration w.r.t untagged vlan id, port priority and drop precedence*/
bcm_error_t port_config(int unit, bcm_port_t port, int vlan, uint8 priority, uint8 CFI)
{
    bcm_error_t rv;   
    bcm_vlan_action_set_t action;

    rv = bcm_port_untagged_vlan_set(unit,port,vlan);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_port_untagged_vlan_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* set default packet priority value in port table */
    rv = bcm_port_untagged_priority_set(unit, port, priority);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_port_untagged_priority_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_port_default_action_get(unit, port, &action);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_port_default_action_get(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_port_default_action_delete(unit,port);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_port_default_action_delete(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    action.new_outer_cfi = CFI;
    action.priority      = priority;
    rv = bcm_vlan_port_default_action_set(unit,port,action);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_port_default_action_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}
/* Setup L2 qos mapping */
bcm_error_t
qos_port_setup(int unit, bcm_port_t port, unsigned int flags, uint8 internal_pri, bcm_color_t clr, uint8 priority, uint8 CFI)
{
    int qMapId = 0;
    bcm_error_t rv;   
    bcm_qos_map_t qMap;
    bcm_gport_t gport;
    BCM_GPORT_MODPORT_SET(gport, unit, port);
    /* Reserve hardware resources for QoS mapping */
    rv = bcm_qos_map_create(unit, flags, &qMapId);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_qos_map_create(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    qMap.int_pri = internal_pri;
    qMap.color   = clr;
    qMap.pkt_pri = priority;
    qMap.pkt_cfi = CFI;
    rv = bcm_qos_map_add(unit, flags, &qMap, qMapId);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_qos_map_add(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    if(flags & BCM_QOS_MAP_INGRESS) {
        rv = bcm_qos_port_map_set(unit, gport, qMapId, -1);
        if (BCM_FAILURE(rv)) {
          printf("Error executing bcm_qos_port_map_set(): %s.\n", bcm_errmsg(rv));
          return rv;
        }
    } else {
        rv = bcm_qos_port_map_set(unit, gport, -1, qMapId);
        if (BCM_FAILURE(rv)) {
          printf("Error executing bcm_qos_port_map_set(): %s.\n", bcm_errmsg(rv));
          return rv;
        }
    }
    return BCM_E_NONE;
}
/* Creates a VLAN(20) and add ingress port and  egress port as members.
 * 
 * Configure ingress port's default VLAN as 20, default packet priority as 4 and CFI as 1.
 *
 * Configure ingress QoS mapping as below and attach it to ingress_port.
 *             {packet priority(4) + CFI(1)} is mapped to {internal_priority(10) + CNG(Yellow)}.
 *
 * Configure egress QoS mapping as below and attach it to egress_port.
 *             {{internal_priority(10) + CNG(Yellow)} is mapped {packet priority(2) + CFI(0)}.
 */

bcm_error_t l2_qos(int unit)
{
  bcm_error_t rv;
  bcm_pbmp_t  pbmp, ubmp;
  bcm_vlan_t vlanId = 20;
  uint8 priority = 4, internal_priority = 10, CFI;
  /* Create the vlan with specific id if it is already present then error will be returned */
  rv = bcm_vlan_create(unit, vlanId);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /*Add ingress_port and egress_port as members of vlanId*/
  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, ingress_port);
  BCM_PBMP_PORT_ADD(pbmp, egress_port);

  rv = bcm_vlan_port_add(unit, vlanId, pbmp, ubmp);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

/* set CFI and default packet priority values in port table for untagged packets */
  CFI = 1;
  rv = port_config(unit, ingress_port, vlanId, priority, CFI);
  if (BCM_FAILURE(rv)) {
      printf("Error executing port_config(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

/* Configure ingress priority mapping on port "ingress_port"
 * for mapping {packet priority 4 + CFI 1} to internal priority 10
 */
  rv = qos_port_setup(unit, ingress_port, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_L2, internal_priority, bcmColorYellow, priority, CFI);
  if (BCM_FAILURE(rv)) {
      printf("Error executing qos_port_setup() for ingress mapping: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  /*Required for CFI remarking on egress port. Sets EGR_VLAN_CONTROL_1r.CFI_AS_CNGf = 1*/
  rv = bcm_switch_control_port_set(unit, egress_port, bcmSwitchColorSelect, BCM_COLOR_OUTER_CFI);
  if (BCM_FAILURE(rv)) {
      printf("Error executing bcm_switch_control_port_set(): %s.\n", bcm_errmsg(rv));
      return rv;
  }

  CFI = 0;
  /*
   * Configure egress QoS mapping as below and attach it to egress_port.
   * {{internal_priority(10) + CNG(Yellow)} to {packet priority(2) + CFI(0)}.
  */
  rv = qos_port_setup(unit, egress_port, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_L2, internal_priority, bcmColorYellow, 2, CFI);
  if (BCM_FAILURE(rv)) {
      printf("Error executing qos_port_setup() for egress mapping: %s.\n", bcm_errmsg(rv));
      return rv;
  }

  return BCM_E_NONE;
}
/*     This function does the following.
 *     ==============================
 *     Send the below untagged packet on ingress port.
 *      
 *      Packet:
 *      ======
 *      Ethernet header: DA=00:00:00:00:00:02, SA=00:00:00:00:00:01
 *
 *      0000 0000 0002 0000 0000 0001 0001 0203
 *      0405 0607 0809 0A0B 0C0D 0E0F 1011 1213
 *      1415 1617 1819 1A1B 1C1D 1E1F 2021 2223
 *      2425 2627 2829 2A2B 2C2D 2E2F DF52 E538
 *
 *      The below packet egresses on egress_port with packet priority 2 and CFI 0.
 *
 *      Packet:
 *      ======
 *      Ethernet II, Src: 00:00:00_00:00:01 (00:00:00:00:00:01), Dst: 00:00:00_00:00:02 (00:00:00:00:00:02)
 *      802.1Q Virtual LAN, PRI: 2, CFI: 0, ID: 20
 *      010. .... .... .... = Priority: Spare (2)
 *        ...0 .... .... .... = CFI: Canonical (0)
 *          .... 0000 0001 0100 = ID: 20
 *
 *      0000 0000 0002 0000 0000 0001 8100 4014
 *      0001 0203 0405 0607 0809 0a0b 0c0d 0e0f
 *      1011 1213 1415 1617 1819 1a1b 1c1d 1e1f
 *      2021 2223 2425 2627 2829 2a2b 2c2d 2e2f
 *      df52 e538
 */

void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");

  printf("Sending untagged packet on ingress_port:%d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x000000000002000000000001000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D2E2FDF52E538; sleep 1", ingress_port);
  bshell(unit, str);

}
/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in l2_qos())
 * c)demonstrates the functionality (Done in verify()).
 */
bcm_error_t execute()
{
  bcm_error_t rv;
  int unit =0;

  bshell(unit, "config show; a ; version");

  if (BCM_FAILURE((rv = test_setup(unit)))) {
    printf("test_setup() failed.\n");
    return -1;
  }
  printf("Completed test setup successfully.\n");

  if (BCM_FAILURE((rv = l2_qos(unit)))) {
    printf("l2_qos() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing l2_qos()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

