/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : VFI Switching
 *
 *  Usage    : BCM.0> cint vfi_switching.c
 *
 *  config   : l2_config.bcm
 *
 *  Log file : vfi_switching_log.txt
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
 *  Summary:
 *  ========
 *    This CINT example demonstrates VFI switching using BCM APIs.
 *
 *  Detailed steps done in the CINT script
 *  ======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Selects two ports and configure them in Loopback mode.
 *
 *    2) Step2 - Configuration (Done in configure_vfi_switching())
 *    ============================================================
 *      a) Enables ingress vlan translation on both the ports.
 *      b) Creates vlans 2 and 5 and add a port to each of them.
 *      c) Creates a broadcast group for the VPLS flow.
 *      d) Creates a VPN/VFI of type VPLS and associate the broadcast group.
 *      e) Creates MPLS type virtual ports on both the physical ports and add them to the
 *         VPN and the broadcast group.
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Transmit a packet with SrcMac A and DstMac B with vlan 5 on port_1.
 *      b) Transmit a packet with SrcMac B and DstMac A with vlan 7 on port_2.
 *
 *      c) Expected Result:
 *      ===================
 *        After step 3.a, MAC address A should be learnt on virtual port (for port_1)
 *        and VFI. The packet should be flooded to the broadcast group associated with
 *        this VPN.
 *        After step 3.b, MAC address B should be learnt on virtual port (for port_2)
 *        and VFI. The packet should be forwarded based on the L2 table
 *
 *        This can be observed through "l2 show"
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


/* Enable vlan translation on the port */
bcm_error_t
enable_ing_vlan_xlate(int unit, bcm_port_t port, int enable)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_vlan_control_port_t vlan_type = bcmVlanTranslateIngressEnable;
    bcm_port_control_t port_type = bcmPortControlDoNotCheckVlan;

    rv = bcm_vlan_control_port_set(unit, port, vlan_type, enable);
    if (BCM_FAILURE(rv)) {
        printf ("Error executing bcm_vlan_control_port_set() for port:%d %s.\n", port, bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_port_control_set(unit, port, port_type, enable);
    if (BCM_FAILURE(rv)) {
        printf ("Error executing bcm_port_control_set() for port:%d %s.\n", port, bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

/* Create vlan and add ports to vlan */
bcm_error_t
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if (BCM_FAILURE(rv)) {
        printf ("Error executing bcm_vlan_create() for vlan:%d %s.\n", vlan, bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
    if (BCM_FAILURE(rv)) {
        printf ("Error executing bcm_vlan_port_add() for vlan:%d %s.\n", vlan, bcm_errmsg (rv));
        return rv;
    }
    return BCM_E_NONE;
}

/* Create the VPN */
bcm_error_t
create_vpls_vpn(int unit, bcm_multicast_t broadcast_group, bcm_vpn_t *vpn)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_mpls_vpn_config_t vpn_info;

    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.flags = BCM_MPLS_VPN_VPLS;
    vpn_info.broadcast_group = broadcast_group;
    vpn_info.unknown_unicast_group = broadcast_group;
    vpn_info.unknown_multicast_group = broadcast_group;
    rv = bcm_mpls_vpn_id_create(unit, &vpn_info);
    if (BCM_FAILURE(rv)) {
        printf ("Error executing bcm_mpls_vpn_id_create() %s.\n", bcm_errmsg (rv));
        return rv;
    }
    *vpn = vpn_info.vpn;

    return BCM_E_NONE;
}

/* Add ports to the VPN */
bcm_error_t
add_customer_ports_to_vpn(int unit, bcm_port_t port, bcm_vlan_t vlan, bcm_vpn_t vpn, 
                                                        bcm_multicast_t bcast_group)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_c_port;
    bcm_gport_t gport;
    bcm_if_t encap_id = 0;

    rv = bcm_port_gport_get(unit, port, &gport);
    if (BCM_FAILURE(rv)) {
        printf("Error in getting gport for port:%d %s.\n", port, bcm_errmsg(rv));
        return rv;
    }

    bcm_mpls_port_t_init(&mpls_c_port);
    mpls_c_port.port = gport;
    mpls_c_port.criteria = BCM_MPLS_PORT_MATCH_PORT_VLAN;
    mpls_c_port.match_vlan = vlan;
    rv = bcm_mpls_port_add(unit, vpn, &mpls_c_port);
    if (BCM_FAILURE(rv)) {
        printf ("Error executing bcm_mpls_port_add() for port:%d %s.\n", port, bcm_errmsg (rv));
        return rv;
    }

    /* Add the port to the broadcast group */
    rv = bcm_multicast_vpls_encap_get(unit, bcast_group, gport, mpls_c_port.mpls_port_id, &encap_id);
    if (BCM_FAILURE(rv)) {
        printf ("Error executing bcm_multicast_vpls_encap_get() for port:%d %s.\n", port, bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_multicast_egress_add(unit, bcast_group, gport, encap_id);
    if (BCM_FAILURE(rv)) {
        printf ("Error executing bcm_multicast_egress_add() for port:%d %s.\n", port, bcm_errmsg (rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Steps to configure vfi */
bcm_error_t
configure_vfi_switching(int unit)
{
    bcm_error_t rv =  BCM_E_NONE;
    int i = 0;
    bcm_port_t customer_port[2] = {port_1, port_2};
    bcm_vlan_t customer_vlan[2] = {5, 7};
    int enable = 1;
    bcm_multicast_t bcast_group;
    bcm_vpn_t vpn;

    /* Enable egress mode */
    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, enable));

    for(i = 0; i < 2; i++) {
        /* Enable Ingress VLAN Translation on customer ports */
        rv = enable_ing_vlan_xlate(unit, customer_port[i], enable);
        if (BCM_FAILURE(rv)) {
            printf("Error in enabling ingress vlan translation: %s.\n",
                    bcm_errmsg(rv));
            return rv;
        }

        /* Customer side VLAN Configuration */
        rv = create_vlan_add_port(unit, customer_vlan[i], customer_port[i]);
        if (BCM_FAILURE(rv)) {
            printf ("Error executing create_vlan_add_port() for vlan:%d %s.\n", customer_vlan[i], bcm_errmsg (rv));
            return rv;
        }
    }
    /* Create a broadcast group for VPLS */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VPLS, &bcast_group);
    if (BCM_FAILURE(rv)) {
        printf("Error in creating VPLS broadacst group : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create a VPLS VPN,to bind the VPLS customer ports */
    rv = create_vpls_vpn(unit, bcast_group, &vpn);
    if (BCM_FAILURE(rv)) {
        printf("Error in creating VPLS vpn : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    for(i = 0; i < 2; i++) {
        /* Create and bind the customer side virtual ports to VPLS VPN */
        rv = add_customer_ports_to_vpn(unit, customer_port[i], customer_vlan[i],
             vpn, bcast_group);
        if (BCM_FAILURE(rv)) {
            printf("Error in adding customer ports to vpn : %s.\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
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

    snprintf(str, 512, "tx 1 pbm=%d data=0x00000000000100000000005081000007002E000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425262728292A2B2C2D6EFC72B0; sleep 1", port_2);
    bshell(unit, str);
    bshell(unit, "show counters");
    bshell(unit, "l2 show");
    bshell(unit, "clear counters");
}

/*
 * This functions does the following
 * a)test setup
 * b)actual configuration (Done in configure_vfi_switching())
 * c)demonstrates the functionality(done in verify()).
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

  if (BCM_FAILURE((rv = configure_vfi_switching(unit)))) {
    printf("configure_vfi_switching() failed.\n");
    return -1;
  }
  printf("Completed configuration(i.e executing configure_vfi_switching()) successfully.\n");

  verify(unit);
  return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

