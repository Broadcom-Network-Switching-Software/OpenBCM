/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

 /* Feature  : vlan cross-connect for virtual ports
 *
 *  Usage    : BCM.0> cint vlan_cross_connect_vp.c
 *
 *  Config   : BCM56275_A1-PORT.bcm
 *
 *  Log file : vlan_cross_connect_vp_log.txt
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
 *    This cint example configures single vlan cross-connect between two VLAN virtual ports
 *    using BCM API.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Selects one ingress port and one egress port and configure them in Loopback mode.
 *         Install a rule to copy incoming packets to CPU and start packet watcher.
 *
 *      Note: This is done to transmit the required test packets using "tx" command and dump the
 *            contents of the packets received on both ingress and egress ports.
 *
 *    2) Step2 - Configuration (Done in configure_vlan_cross_connect_vp()):
 *    =====================================================================
 *      a) Enable ingress vlan translation on both ingress and egress ports.
 *
 *      b) Creates a VLAN(20) and add ingress port and  egress port as members.
 *
 *      c) Enable single VLAN cross-connect on VLAN 20 and disable learning on VLAN 20.
 *
 *      d) Configure two vlan virtual ports(i.e of type bcm_vlan_port_t), one on the ingress
 *         and the other on egress ports with VLAN 20 as match criteria.
 *
 *      e) Configure single vlan cross-connect with two vlan virtual ports as members.
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Send a packet with VLAN 20 on ingress port.
 *
 *      b) Expected Result:
 *      ===================
 *         The packet egresses out of egress port and no learning happens for this packet.
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

/* Create vlan and add port to vlan */
int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error creating vlan: %d port:%d\n", vlan, port);
        return rv;
    }

    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_port_add: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;

}

/* Enable vlan cross connect */
bcm_error_t
enable_vlan_cross_connect(int unit, bcm_vlan_t vlan, bcm_vlan_control_vlan_t *vlan_ctrl)
{
    bcm_error_t rv = BCM_E_NONE;

    bcm_vlan_control_vlan_t_init(*vlan_ctrl);
    vlan_ctrl->flags = BCM_VLAN_LEARN_DISABLE;
    vlan_ctrl->forwarding_mode = bcmVlanForwardSingleCrossConnect;
    rv = bcm_vlan_control_vlan_set(unit, vlan, *vlan_ctrl);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_control_vlan_set: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Create L2 logical port */
bcm_error_t
create_l2_vp(int unit, bcm_vlan_t vlan, bcm_gport_t gport, bcm_vlan_port_t *vp)
{
    bcm_error_t rv = BCM_E_NONE;

    vp->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp->match_vlan = vlan;
    vp->egress_vlan = vlan;
    vp->port = gport;
    rv = bcm_vlan_port_create(unit, vp);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_port_create: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Configure vlan crosss connect */
bcm_error_t
configure_vlan_cross_connect(int unit, bcm_vlan_t vlan,
                             bcm_vlan_port_t customer_vp,  bcm_vlan_port_t provider_vp)
{
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_vlan_cross_connect_add(unit, vlan, BCM_VLAN_INVALID,
                        customer_vp.vlan_port_id, provider_vp.vlan_port_id);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_cross_connect_add: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/* Setup  vlan cross connect using virtual ports */
bcm_error_t  configure_vlan_cross_connect_vp(int unit)
{
    bcm_port_t cust_port = ingress_port;
    bcm_port_t prov_port = egress_port;
    bcm_vlan_t vlan = 20;
    bcm_error_t rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;
    bcm_gport_t cust_gport, prov_gport;

    /* Enable egress mode */
    rv = bcm_switch_control_set (unit, bcmSwitchL3EgressMode, 1);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_switch_control_set: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    /* Enable ingress vlan translation */
    rv = bcm_vlan_control_port_set(unit, cust_port,
                                                  bcmVlanTranslateIngressEnable, 1);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_control_port_set: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_vlan_control_port_set(unit, prov_port,
                                                  bcmVlanTranslateIngressEnable, 1);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_control_port_set: %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_port_gport_get(unit, cust_port, &cust_gport);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_port_gport_get: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_gport_get(unit, prov_port, &prov_gport);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_port_gport_get: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    /* VLAN Configuration */
    rv = create_vlan_add_port(unit, vlan, cust_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_vlan_add_port(unit, vlan, prov_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enable VLAN cross connect */
    bcm_vlan_control_vlan_t vlan_ctrl;
    rv = enable_vlan_cross_connect(unit, vlan, &vlan_ctrl);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create L2 logical ports */
    bcm_vlan_port_t customer_vp;
    rv = create_l2_vp(unit, vlan, cust_gport, customer_vp);
    if (BCM_FAILURE(rv)) {
        printf("Error creating L2 logical port (customer VP) : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("L2 logical port (customer VP): 0x%x\n",customer_vp.vlan_port_id);

    bcm_vlan_port_t provider_vp;
    rv = create_l2_vp(unit, vlan, prov_gport, provider_vp);
    if (BCM_FAILURE(rv)) {
        printf("Error creating L2 logical port (provider VP) : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    printf("L2 logical port (provider VP): 0x%x\n",provider_vp.vlan_port_id);

    /* Configure vlan crosss connect */
    rv = configure_vlan_cross_connect(unit, vlan, customer_vp, provider_vp);

    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan cross-connect: %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

void verify(int unit)
{
  char   str[512];

  bshell(unit, "hm ieee");
  bshell(unit, "sleep quiet 1");
  printf("Sending tagged packet with Vlan 20 on ingress_port:%d\n", ingress_port);
  snprintf(str, 512, "tx 1 pbm=%d data=0x10111213141500010203040581000014002e00000000000000001234568a1234568b1234568c1234568d1234568e1234568f123456901234569112345692123498f05485; sleep quiet 1", ingress_port);
  bshell(unit, str);

}
/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in configure_vlan_cross_connect_vp())
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

  if (BCM_FAILURE((rv = configure_vlan_cross_connect_vp(unit)))) {
    printf("configure_vlan_cross_connect_vp() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing configure_vlan_cross_connect_vp()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

