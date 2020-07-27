/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 *  Feature  : VLAN
 *
 *  Usage    : BCM.0> cint general_vlan.c
 *
 *  config   : BCM56275_A1-PORT.bcm
 *
 *  Log file : general_vlan_log.txt
 *
 *  Test Topology :
 *                  +------------------------+
 *                  |                        |
 *    ingress_port1 |                        |
 *    +-------------+                        |
 *                  |      SWITCH            |
 *    ingress_port2 |                        |
 *    +-------------+                        |
 *                  |                        |
 *                  |                        |
 *                  +------------------------+
 *
 *  Summary:
 *  ========
 *    This cint example is to demonstrate L2 VLAN management activities like
 *    add/create/delete vlan using BCM APIs. This script also demonstrates how
 *    to set up default VLAN on the device.
 *
 *  Detailed steps done in the CINT script:
 *  =======================================
 *    1) Step1 - Test Setup (Done in test_setup()):
 *    =============================================
 *      a) Selects two ingress ports.
 *
 *    2) Step2 - Configuration (Done in config_vlan()):
 *    =================================================
 *      a) Create a vlan 20 and add two ports to the vlan.
 *
 *      b) Create a vlan 10 and make it new default vlan.
 *
 *    3) Step3 - Verification (Done in verify()):
 *    ===========================================
 *      a) Display the vlan configuration using "vlan show"
 *         and display default vlan using "vlan default".
 *      b) Expected Result:
 *      ===================
 *         Default vlan should be 10.
 *         Default Vlan 10 should have all ports as it members.
 *         Vlan 20 should have ingress_port1 and ingressport2 as it members.
 *
 *         "vlan show" output
 *         ==================
 *         vlan 10 ports cpu,cd,xe (0x0000000000000000000000000000f0000f4000f0000f0000f0000f4000f0001f), untagged cd,xe (0x0000000000000000000000000000f0000f4000f0000f0000f0000f4000f0001e) MCAST_FLOOD_UNKNOWN
 *         vlan 20 ports cd0-cd1 (0x0000000000000000000000000000000000000000000000000000000000000006), untagged none (0x0000000000000000000000000000000000000000000000000000000000000000) MCAST_FLOOD_UNKNOWN
 *
 *         "vlan default" output
 *         =====================
 *         Default VLAN ID is 10
 */
/* Reset c interpreter*/
cint_reset();
bcm_port_t ingress_port1, ingress_port2;

uint16 defVlan=1;

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
 * This functions gets the port numbers.
 */
bcm_error_t test_setup(int unit)
{
  int port_list[2], i;

  if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
    printf("portNumbersGet() failed\n");
    return -1;
  }

  ingress_port1 = port_list[0];
  ingress_port2 = port_list[1];

  return BCM_E_NONE;
}

/* Add a port to tagged and/or untagged vlan bitmap */
bcm_error_t vlan_add_port(int unit, int vid, bcm_port_t tport, bcm_port_t utport)
{
    bcm_pbmp_t pbmp, upbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, tport);
    BCM_PBMP_PORT_ADD(upbmp, utport);
    return bcm_vlan_port_add(unit, vid, pbmp, upbmp);
}
/* Add a gport to vlan */
bcm_error_t vlan_add_gport(int unit, int vid, bcm_port_t port, unsigned int flag)
{
    bcm_gport_t gport;
    BCM_GPORT_MODPORT_SET(gport, unit, port);
    return bcm_vlan_gport_add(unit, vid, gport, flag);
}
/* Make VLAN vlanId1 as a default vlan and delete older default vlan */
bcm_error_t new_default_vlan_setup(int unit, int vid)
{
    bcm_pbmp_t pbmp;
    bcm_pbmp_t upbmp;
    int rv;
    /* Get the port members for default VLAN */
    rv = bcm_vlan_port_get(unit,defVlan,pbmp,upbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_port_get(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create a vlan with all the valid port bitmap */
    rv = bcm_vlan_create(unit, vid);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add ports into VLAN bitmap*/
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Make VLAN 10 as a default vlan and delete older default vlan*/
    rv = bcm_vlan_default_set(unit,vid);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_default_set(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_vlan_destroy(unit,defVlan);
    if (BCM_FAILURE(rv)) {
        printf("Error executing bcm_vlan_destroy(): %s.\n", bcm_errmsg(rv));
        return rv;
    }
    return BCM_E_NONE;
}

void verify(int unit)
{
  bshell(unit, "vlan show");
  bshell(unit, "vlan default");
}

bcm_error_t config_vlan(int unit)
{
    uint16 vlanId1=10, vlanId2=20;
   /* Create the vlan with specific id if it is already present then error will be returned */
    int rv = bcm_vlan_create(unit, vlanId2);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in creating VLAN %d : %s.\n", vlanId2, bcm_errmsg(rv));
    }

    rv = vlan_add_gport(unit, vlanId2, ingress_port2, 0);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in function vlan_add_gport: %s.\n", bcm_errmsg(rv));
    }

    rv = vlan_add_port(unit, vlanId2, ingress_port1, 0);
    if(BCM_FAILURE(rv))
    {
        printf("\nError in function vlan_add_port: %s.\n", bcm_errmsg(rv));
    }
    /*Setup vlanId1(10) as default VLAN*/
    rv =  new_default_vlan_setup(unit, vlanId1);
    if(BCM_FAILURE(rv))
    {
        printf("\nError while setting new default vlan: %s.\n", bcm_errmsg(rv));
    }

    return BCM_E_NONE;
}
/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in config_vlan())
 * c)Displays the configuration (Done in verify()).
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

  if (BCM_FAILURE((rv = config_vlan(unit)))) {
    printf("vlan_port_flex_ctr() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing config_vlan()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

