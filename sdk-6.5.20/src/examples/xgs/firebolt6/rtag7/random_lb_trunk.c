/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Randomized Load-Balancing - LAG
 *
 *  Usage    : BCM.0> cint random_lb_trunk.c
 *
 *  config   : rtag7_config.bcm
 *
 *  Log file : random_lb_trunk_log.txt
 *  
 *  Test Topology :
 *
 *                +-----------+
 *                |           |        +--EgressPort1--->
 *                |           |        |
 *  IngressPort-->|   SWITCH  |--trunk-+--EgressPort2--->
 *                |           |        |
 *                |           |        +--EgressPort3--->
 *                +-----------+
 *
 *   Summary:
 *   ========
 *     This cint example configures randomized load balancing on the egress trunk using
 *     BCM APIs
 *
 *   Detailed steps done in the CINT script
 *   ======================================
 *     1) Step1 - Test Setup (Done in test_setup())
 *     ============================================
 *       a) Select one ingress port and three egress ports and configure them in Loopback mode.
 *
 *     2) Step2 - Configuration (Done in random_lb_trunk())
 *     ==================================================== 
 *       a) Create a VLAN(12) and add ingress port and  egress port as members.
 *       b) Create a trunk with id 1 and add all the egress ports to this trunk. Set the port
 *          selection criteria to BCM_TRUNK_PSC_RANDOMIZED (randomized load balancing).
 *       c) Configure the destination MAC address of the test packet statically in the L2 table
 *          in order to  make the test packet a unicast packet.
 *       d) Set the load-balancing randomizer value to 3 for the ingress port.
 *
 *     3) Step3 - Verification (Done in verify())
 *     ==========================================
 *       a) Transmit 50 copies of the below packet on the ingress port and verify that packets
 *          are load balanced across the egress ports using "show counters"
 *          Packet
 *          ======
 *          DA 0x00000000AAAA
 *          SA 0x000000002222
 *          VLAN 12
 *          DIP 192.168.10.1
 *          SIP 10.10.10.1
 *          00 00 00 00 AA AA 00 00 00 00 22 22 81 00 00 0C
 *          08 00 45 00 00 3C 00 00 00 00 40 FF 9B 0F 0A 0A
 *          0A 01 C0 A8 0A 01 00 01 02 03 04 05 06 07 08 09
 *          0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19
 *          1A 1B 1C 1D 1E 1F 20 21 22 23 24 25 26 27 2E CB
 *          F6 84
 *       b) Expected Result
 *          ===============
 *          The packets should be load balanced across all the trunk member ports. This can be
 *          checked using "show counters"
 */

/* Reset c interpreter*/
cint_reset();
bcm_port_t ingress_port;
bcm_port_t egress_port_1;
bcm_port_t egress_port_2;
bcm_port_t egress_port_3;

/* This function populates the port_list_arr with the required
   number of ports
*/
bcm_error_t portNumbersGet(int unit, bcm_port_t *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit,&configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i = 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
            port_list[port] = i;
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
    return BCM_E_NONE;
}

/*
 * Configures the port in loopback mode and sets the discard bit
 * in the PORT table
 */
bcm_error_t egress_port_setup(int unit, bcm_port_t port)
{
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_ALL));
    return BCM_E_NONE;
}
/*
 * This functions gets the port numbers and sets up ingress and 
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t test_setup(int unit)
{
    int port_list[4];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    ingress_port = port_list[0];
    egress_port_1 = port_list[1];
    egress_port_2 = port_list[2];
    egress_port_3 = port_list[3];

    if (BCM_E_NONE != ingress_port_setup(unit, ingress_port)) {
        printf("ingress_port_setup() failed");
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_1)) {
        printf("egress_port_setup() failed for port:%d", egress_port_1);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_2)) {
        printf("egress_port_setup() failed for port:%d", egress_port_2);
        return -1;
    }

    if (BCM_E_NONE != egress_port_setup(unit, egress_port_3)) {
        printf("egress_port_setup() failed for port:%d", egress_port_3);
        return -1;
    }

    return BCM_E_NONE;
}

/*
 * This function creates a trunk and adds the member ports
 */
bcm_error_t create_trunk(int unit, bcm_trunk_t *tid, int count, bcm_port_t *member_ports)
{
    bcm_error_t rv = BCM_E_NONE;
    int i = 0;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t *member_info = NULL;
    bcm_trunk_member_t info;

    bcm_trunk_info_t_init(&trunk_info);

    member_info = sal_alloc((sizeof(info) * count), "trunk members");

    for (i = 0 ; i < count ; i++) {
        bcm_trunk_member_t_init(&member_info[i]);
        BCM_GPORT_MODPORT_SET(member_info[i].gport, 0, member_ports[i]);
    }

    /* Use randomized load balancing */
    trunk_info.psc = BCM_TRUNK_PSC_RANDOMIZED;

    trunk_info.dlf_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.mc_index = BCM_TRUNK_UNSPEC_INDEX;
    trunk_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;

    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, tid);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_trunk_create %s.\n", bcm_errmsg (rv));
        return rv;
    }

    rv = bcm_trunk_set(unit, *tid, &trunk_info, count, member_info);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_trunk_set %s.\n", bcm_errmsg (rv));
        return rv;
    }
    
    return rv;
}

bcm_error_t random_lb_trunk(int unit)
{
    bcm_vlan_t vid = 12;
    bcm_pbmp_t pbmp, ubmp;
    bcm_gport_t trunk_gp_id;
    bcm_trunk_t trunk_id = 1;
    bcm_port_t trunk_ports[3];
    bcm_l2_addr_t l2addr;
    bcm_mac_t da = {0x00, 0x00, 0x00, 0x00, 0xAA, 0xAA};
    int i = 0;
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_port_control_set (unit, ingress_port,
                      bcmPortControlTrunkLoadBalancingRandomizer, 3);
    if (BCM_FAILURE (rv))
    {
        printf ("Error setting TrunkLoadBalancingRandomizer %s.\n", bcm_errmsg (rv));
        return rv;
    }

    /* Create the vlan with specific id */
    rv = bcm_vlan_create (unit, vid);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_create(): %s.\n", bcm_errmsg (rv));
        return rv;
    }

    BCM_PBMP_CLEAR (pbmp);
    BCM_PBMP_CLEAR (ubmp);

    BCM_PBMP_PORT_ADD (pbmp, ingress_port);
    BCM_PBMP_PORT_ADD (pbmp, egress_port_1);
    BCM_PBMP_PORT_ADD (pbmp, egress_port_2);
    BCM_PBMP_PORT_ADD (pbmp, egress_port_3);

    rv = bcm_vlan_port_add (unit, vid, pbmp, ubmp);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_vlan_port_add(): %s.\n", bcm_errmsg (rv));
        return rv;
    }

    trunk_ports[0] = egress_port_1;
    trunk_ports[1] = egress_port_2;
    trunk_ports[2] = egress_port_3;

    rv = create_trunk(unit, &trunk_id, 3, trunk_ports);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing create_trunk(): %s.\n", bcm_errmsg (rv));
        return rv;
    }
    BCM_GPORT_TRUNK_SET (trunk_gp_id, trunk_id);

    bcm_l2_addr_t_init (&l2addr, da, vid);
    l2addr.port = trunk_gp_id;
    rv = bcm_l2_addr_add (unit, &l2addr);
    if (BCM_FAILURE (rv))
    {
        printf ("Error executing bcm_l2_addr_add(): %s.\n", bcm_errmsg (rv));
        return rv;
    }

    return rv;
}

void verify(int unit)
{
    char   str[512];

    bshell(unit, "hm ieee");

    snprintf(str, 512, "tx 50 pbm=%d data=0x00000000AAAA0000000022228100000C08004500003C0000000040FF9B0F0A0A0A01C0A80A01000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20212223242526272ECBF684; sleep 1", ingress_port);
    bshell(unit, str);
    snprintf(str, 512, "show counters");
    bshell(unit, str); 
}

bcm_error_t execute()
{
    int unit = 0;
    int rv = 0;

    bshell(unit, "config show; a ; version");
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = random_lb_trunk(unit)))) {
        printf("random_lb_trunk() failed.\n");
        return -1;
    }

    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

