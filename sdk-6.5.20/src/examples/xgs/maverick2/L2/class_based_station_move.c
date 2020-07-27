/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Class Based Station Movement
 *
 *  Usage    : BCM.0> cint class_based_station_move.c
 *
 *  config   : l2_config.bcm
 *
 *  Log file : class_based_station_move_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *       Port1       |                              |     Port2
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *
 *  Summary:
 *  ========
 *    This CINT demonstrates Class Based Station Movement using BCM APIs.
 *
 *  Detailed steps done in the CINT script
 *  ======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select two ports and configure them in Loopback mode. These ports will be
 *         alternatively used for packet ingress
 *
 *    2) Step2 - Configuration (Done in configure_class_based_station_move())
 *    =======================================================================
 *      a) Create a VLAN(5) and add port_1 and port_2 as members.
 *      b) Set the class attribute, associate the class with a priority and enable class
 *         based learning for both the ports. The class attribute for port_1 is set to 1
 *         and for port_2 it is set to 3. The priority for class attribute 1 is set to 1
 *         and for class attribute 3 it is set to 3.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Transmit a packet with source MAC as A and vlan ID 5 on port_1.
 *      b) Transmit the same packet on port_2.
 *      c) Transmit the same packet on port_1.
 *
 *      d) Expected Result:
 *      ===================
 *         After Step 3.a, MAC address A should be learnt on port_1.
 *         After Step 3.b, MAC address A should be learnt on port_2 as the priority for
 *         port_2 is higher than port_1.
 *         After Step 3.c, MAC address A should still be learnt on port_2 as the priority
 *         for port_2 is higher than port_1.
 *
 *         This can be observed using "l2 show".
 */
cint_reset();

bcm_port_t port_1;
bcm_port_t port_2;

/* This function populates the port_list with the required
   number of ports
*/
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = BCM_E_NONE;
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

    return rv;
}

/*
 * Configures the ports in MAC loopback mode. The ingress port is configured to
 * allow the packets whereas the egress port is configured to discard frames at
 * the ingress
 */
bcm_error_t port_setup(int unit, bcm_port_t ingress_port, bcm_port_t egress_port)
{
    bcm_error_t rv = BCM_E_NONE;

    //Ingress Port
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, ingress_port, BCM_PORT_DISCARD_NONE));

    //Egress Port
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, egress_port, BCM_PORT_DISCARD_ALL));

    return rv;
}

/*
 * This function gets the port numbers and sets them up. Check port_setup().
 */

bcm_error_t test_setup(int unit)
{
    int port_list[2];
    bcm_error_t rv = BCM_E_NONE;

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 2)) {
      printf("portNumbersGet() failed\n");
      return -1;
    }

    port_1 = port_list[0];
    port_2 = port_list[1];

    if (BCM_E_NONE != port_setup(unit, port_1, port_2)) {
      printf("port_setup() failed for port");
      return -1;
    }

    return rv;
}


/* Create vlan and add ports to vlan */
bcm_error_t vlan_create_add_port(int unit, int vid, int port_1, int port_2)
{
    bcm_pbmp_t pbmp, upbmp;
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_vlan_create(unit, vid);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_create() for vlan:%d %s.\n", vid, bcm_errmsg (rv));
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_1);
    BCM_PBMP_PORT_ADD(pbmp, port_2);
    rv = bcm_vlan_port_add(unit, vid, pbmp, upbmp);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_port_add() for vlan:%d %s.\n", vid, bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

/* Settings for class based station move */
bcm_error_t _configure_class_based_station_move(int unit, bcm_port_t port, int class,
                                    int priority, int flag)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t gport = 0;
    bcm_port_control_t port_type = bcmPortControlLearnClassEnable;
    int enable = 1;

    rv = bcm_port_gport_get(unit, port, &gport);
    if (BCM_FAILURE(rv)) {
        printf("Error in getting gport : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setting the class attribute for the port */
    rv = bcm_l2_learn_port_class_set(unit, gport, class);
    if (BCM_FAILURE(rv)) {
        printf("Error in l2 learn port class set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Setting the priority for the class */
    rv = bcm_l2_learn_class_set(unit, class, priority, flag);
    if (BCM_FAILURE(rv)) {
        printf("Error in l2 learn class set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enabling class based learning on the port */
    rv = bcm_port_control_set(unit, gport, port_type, enable);
    if (BCM_FAILURE(rv)) {
        printf("Error in port control set : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t configure_class_based_station_move(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_vlan_t vid = 5;
    int i = 0;
    bcm_port_t port[2] = {port_1, port_2};
    int class[2] = {1, 3}; /* class 1 for port_1 and 3 for port_2 */
    int priority[2] = {1, 3}; /* priority 1 for class 1 and prority 3 for class 3 */
    int flag = 0;

    /* Create the vlan and add ports */
    rv = vlan_create_add_port (unit, vid, port_1, port_2);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Settings for class based station move.
     * If the flag is set to "BCM_L2_LEARN_CLASS_MOVE", then station move
     * happens if the priority of the incoming port is same as the priority
     * of the port already associated with packet's MACSA
     */
    for (i = 0; i < 2; i++) {
        rv = _configure_class_based_station_move(unit, port[i], class[i],
                                                 priority[i], flag);
        if (BCM_FAILURE(rv)) {
            printf("Error in configuring class based station move : %s.\n",
                    bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}

void verify(int unit)
{
    char   str[512];

    bshell(unit, "hm ieee");

    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000000500000000001081000005002E000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D6EFC72B0; sleep 1", port_1);
    bshell(unit, str);
    bshell(unit, "show counters");
    bshell(unit, "l2 show");
    bshell(unit, "clear counters");

    if (BCM_E_NONE != port_setup(unit, port_2, port_1)) {
      printf("port_setup() failed for port");
    }

    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000000500000000001081000005002E000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D6EFC72B0; sleep 1", port_2);
    bshell(unit, str);
    bshell(unit, "show counters");
    bshell(unit, "l2 show");
    bshell(unit, "clear counters");

    if (BCM_E_NONE != port_setup(unit, port_1, port_2)) {
      printf("port_setup() failed for port");
    }

    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000000500000000001081000005002E000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D6EFC72B0; sleep 1", port_1);
    bshell(unit, str);
    bshell(unit, "show counters");
    bshell(unit, "l2 show");
    bshell(unit, "clear counters");
}


/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in configure_class_based_station_move())
 * c)demonstrates the functionality(done in verify()).
 */
bcm_error_t execute()
{
    bcm_error_t rv = BCM_E_NONE;
    int unit =0;

    bshell(unit, "config show; a ; version");

    if (BCM_FAILURE((rv = test_setup(unit)))) {
      printf("test_setup() failed.\n");
      return -1;
    }
    printf("Completed test setup successfully.\n");

    if (BCM_FAILURE((rv = configure_class_based_station_move(unit)))) {
      printf("configure_class_based_station_move() failed.\n");
      return -1;
    }
    printf("Completed configuration(i.e executing configure_class_based_station_move() successfully.\n");

    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

