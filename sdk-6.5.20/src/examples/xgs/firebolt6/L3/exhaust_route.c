/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPv4 Route scaling/filling of the entire table.
 *
 * Usage    : BCM.0> cint exhaust_route.c
 *
 * config   : exhaust_route_config.bcm
 *            'config add l3_mem_entries=32768'
 *
 * Log file : exhaust_route_log.txt
 *
 * Test Topology :
 *
 *                  +------------------------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *     ingress_port |                              |  egress_port
 * +----------------+          SWITCH              +-----------------+
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  |                              |
 *                  +------------------------------+
 *
 * Summary:
 * ========
 *   This Cint example to show configuration of the IPv4 route scaling scenario
 *   using BCM APIs. Note that, number of banks varies based on the chipset.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 *   1) Step1 - Test Setup (Done in test_setup()):
 *   =============================================
 *     a) This test_setup is not applicable for scaling scenario test as
 *        it's a configuration related test rather than functionality. However
 *        we have left as is just for completeness.
 *
 *   2) Step2 - Configuration (Done in config_l3_exhaust_route()):
 *   =========================================================
 *     a) Configure a basic IPv4 route entry functional scenario along with
 *        scaling/filling the entire table. This adds the routes in l3 table[LPM]
 *        and does the necessary configurations of vlan, interface and next hop.
 *
 *     b) Before BCMINIT, Makesure to set this config variable 'config add
 *        l3_mem_entries=32768' in config.bcm
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) display_hash_config - Displays the hash configuration.
 *     b) display_l3_info - Displays the configured L3 information.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that, maximum entries/interfaces/nexthops related to L3 route
 *       info and entries used/added and utilisation/remaining percentage and so on.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port;
int unit = 0;

/*
 * This function is written so that hardcoding of port
 * numbers in Cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
portNumbersGet(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &configP);
    if (BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp, i) && (port < num_ports)) {
            port_list[port] = i;
            port++;
        }
    }

    if ((0 == port) || (port != num_ports)) {
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
bcm_error_t
ingress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_qset_t qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;

    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_discard_set(unit, port, BCM_PORT_DISCARD_NONE));

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
 * on the specified port to CPU. Port is also configured
 * to discard all packets. This is to avoid continuous
 * loopback of the packet.
 */
bcm_error_t
egress_port_setup(int unit, bcm_port_t port)
{
    bcm_field_qset_t qset;
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
 * Test Setup:
 * This functions gets the port numbers and sets up ingress and
 * egress ports. Check ingress_port_setup() and egress_port_setup().
 */
bcm_error_t
test_setup(int unit)
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

void
verify(int unit)
{
    char str[512];

    bshell(unit, "hm ieee");
    bshell(unit, "clear c");
    printf("\nSending IPv4 unicast packet to ingress_port:%d\n", ingress_port);
    snprintf(str, 512, "tx 1 pbm=%d data=0x0000000000020000000000018100000208004500002A0000000040FFA3E50A0A0A2AC0A8011400010203000000000000000000000000000000000000024D7BD6; sleep quiet 1", ingress_port);
    bshell(unit, str);
    bshell(unit, "show c");

    return;
}

int added_ip4_routes =0;
int added_ip6_routes =0;

/* Create vlan and add port to vlan */
int
create_vlan_add_port(int unit, bcm_vlan_t vlan, bcm_port_t port)
{
    bcm_error_t rv;
    bcm_pbmp_t pbmp, ubmp;

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_vlan_create(unit, vlan);
    if ((BCM_FAILURE(rv)) & (rv != BCM_E_EXISTS)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vlan, pbmp, ubmp));

    return BCM_E_NONE;
}

/* Create L3 interface */
int
create_l3_intf(int unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_if_t *l3_intf_id)
{
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    sal_memcpy(l3_intf.l3a_mac_addr, mac, sizeof(mac));

    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    *l3_intf_id = l3_intf.l3a_intf_id;

    return BCM_E_NONE;
}

/* Create L3 egress object */
int
create_egr_obj(int unit, bcm_if_t l3_if, bcm_mac_t mac, bcm_gport_t gport,
               bcm_if_t *egr_if)
{
    bcm_error_t rv;
    bcm_l3_egress_t l3_egr;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.intf = l3_if;
    l3_egr.port = gport;
    sal_memcpy(l3_egr.mac_addr, mac, sizeof(mac));
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0, &l3_egr, egr_if));

    return BCM_E_NONE;
}

/* Steps to configure IPv4 Route */
int
configure_route(int unit , bcm_if_t *egr_if)
{
    bcm_error_t rv;
    bcm_if_t egr_l3_if;
    bcm_mac_t src_mac = {0x00, 0x00, 0x00, 0x0, 0x00, 0x05};
    bcm_mac_t dst_mac = {0x00, 0x00, 0x11, 0x11, 0x22, 0x25};
    bcm_port_t egr_port = egress_port;
    bcm_gport_t egr_gport;
    bcm_vlan_t egr_vlan = 5;

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode,
                        0x1));

    rv = create_vlan_add_port(unit, egr_vlan, egr_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = create_l3_intf(unit, src_mac, egr_vlan, &egr_l3_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 interface : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_IF_ERROR_RETURN(bcm_port_gport_get(unit, egr_port, &egr_gport));
    rv = create_egr_obj(unit, egr_l3_if, dst_mac, egr_gport, egr_if);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring l3 egress object : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/* Adding IPV4 subnet address to defip table */
int
add_route(int unit, bcm_ip_t subnet_addr, bcm_ip_t subnet_mask, bcm_if_t egr_if)
{
    bcm_l3_route_t route;

    bcm_l3_route_t_init(&route);

    route.l3a_intf = egr_if;

    route.l3a_subnet = subnet_addr;
    route.l3a_ip_mask = subnet_mask;

    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route));

    return BCM_E_NONE;
}

/* Adding IPV6 subnet address to defip table */
int
add_ip6_route(int unit, bcm_ip6_t subnet_addr, bcm_ip6_t subnet_mask,
              bcm_if_t egr_if)
{
    bcm_l3_route_t route;

    bcm_l3_route_t_init(&route);
    route.l3a_flags |= BCM_L3_IP6; /* Needed for IPv6 */

    route.l3a_intf = egr_if;

    route.l3a_ip6_net = subnet_addr;
    route.l3a_ip6_mask = subnet_mask;

    BCM_IF_ERROR_RETURN(bcm_l3_route_add(unit, &route));

    return BCM_E_NONE;
}

/* Adding router mac */
int
add_router_mac(int unit, bcm_mac_t router_mac, bcm_vlan_t ing_vlan,
               bcm_port_t ing_port)
{
    bcm_error_t rv;
    bcm_l2_addr_t l2_addr;

    rv = create_vlan_add_port(unit, ing_vlan, ing_port);
    if (BCM_FAILURE(rv)) {
        printf("Error in configuring vlan : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_l2_addr_t_init(l2_addr, router_mac, ing_vlan);

    l2_addr.flags =  BCM_L2_L3LOOKUP;

    BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2_addr));

    return BCM_E_NONE;
}

/* Counting number of set bits in an integer */
int
num_bits(int num)
{
    int count = 0;

    while(num) {
        num &= (num-1);
        count++;
    }

    return count;
}

int
add_ipv6_route_entries(int unit, bcm_if_t egr_if, bcm_ip6_t base_ip_addr,
                       bcm_ip6_t base_ip_addr_mask, int iteration, int subnet)
{
    bcm_error_t rv;
    bcm_ip6_t ip_addr;
    int num_of_set_bytes;
    int i;

    /* This logic is to change the ip address mask for different subnets */
    num_of_set_bytes = subnet/8 ;
    if ((subnet % 8) == 0) {
         for( i=num_of_set_bytes; i<16; i++)
           base_ip_addr_mask[i] &= 0;
    } else {
        base_ip_addr_mask[num_of_set_bytes] &= 0xF0;
        for( i=(num_of_set_bytes+1); i<16; i++)
           base_ip_addr_mask[i] &= 0;
    }

    /*
     * This logic is to provide a unique ipv6 address each time it is added to
     * route table and it is not the essential logic if each time a new address
     * is provided to be added to the route table.
     */

    if ((subnet % 8) == 0) {
        if (iteration >= 1021)
            base_ip_addr[(num_of_set_bytes-5)] += iteration-1021+1;
        else if (iteration >= 766)
            base_ip_addr[(num_of_set_bytes-4)] += iteration-766+1;
        else if (iteration >= 511)
            base_ip_addr[(num_of_set_bytes-3)] += iteration-511+1;
        else if (iteration >= 255)
            base_ip_addr[(num_of_set_bytes-2)] += iteration-255+1;
        else
            base_ip_addr[(num_of_set_bytes-1)] += iteration+1;
    }

    else {
          if (iteration >= 1021)
            base_ip_addr[(num_of_set_bytes-7)] += iteration-1021+1;
        else if (iteration >= 766)
            base_ip_addr[(num_of_set_bytes-6)] += iteration-766+1;
        else if (iteration >= 511)
            base_ip_addr[(num_of_set_bytes-5)] += iteration-511+1;
        else if (iteration >= 255)
            base_ip_addr[(num_of_set_bytes-4)] += iteration-255+1;
        else
            base_ip_addr[(num_of_set_bytes-3)] += iteration+1;
    }
    ip_addr = base_ip_addr;
    rv = add_ip6_route(unit, ip_addr, base_ip_addr_mask, egr_if);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_route_add() failed at iteration %d in %d subnet: %s\n",
                iteration , subnet, bcm_errmsg(rv));
        return rv;
    }

    added_ip6_routes++;
    return BCM_E_NONE;
}

int
add_ipv4_route_entries(int unit, bcm_if_t egr_if, bcm_ip_t base_ip_addr,
                        bcm_ip_t ip_addr_mask, int iteration, int subnet)
{
    bcm_error_t rv;
    bcm_ip_t ip_addr;

    /* This logic is to change the ip address mask for different subnets */
    ip_addr_mask = (ip_addr_mask & (ip_addr_mask << (32 - subnet)));

    /*
     * This logic is to provide a unique ipv4 address each time it is added to
     * route table and it is not the essential logic if each time a new address
     * is provided to be added to the route table.
     */

    ip_addr = (base_ip_addr ) + (iteration <<(32 - num_bits(ip_addr_mask)));
    rv = add_route(unit, ip_addr, ip_addr_mask, egr_if);
    if (BCM_FAILURE(rv)) {
        printf("bcm_l3_route_add() failed at iteration %d in /%d subnet: %s\n",
                iteration, subnet, bcm_errmsg(rv));
        return rv;
    }

    added_ip4_routes++;
    return BCM_E_NONE;
}

/* Displays the L3 route information added through this script */
int
display_l3_info(int unit)
{
    bcm_l3_info_t l3info;

    BCM_IF_ERROR_RETURN(bcm_l3_info(unit, &l3info));

    printf("L3 INFO:\n");
    printf("  L3 route table size (unit is IPv4 unicast): %d\n",
           l3info.l3info_max_route);
    printf("  L3 IPv4 route entries used: %d (%d remaining, %d%% utilization)\n",
           l3info.l3info_used_route,
           l3info.l3info_max_route - l3info.l3info_used_route,
           ((l3info.l3info_used_route * 100) +
           (l3info.l3info_used_route / 2)) / l3info.l3info_max_route);
    printf("  NextHops used: %d\n", l3info.l3info_used_nexthop);
    printf("  L3 interfaces used: %d\n", l3info.l3info_used_intf);
    printf("  Maximum L3 interface groups the chip supports: %d\n",
           l3info.l3info_max_intf_group);
    printf("  Maximum L3 interfaces the chip supports: %d\n",
           l3info.l3info_max_intf);
    printf("  Maximum NextHops: %d\n", l3info.l3info_max_nexthop);

    return BCM_E_NONE;
}

bcm_error_t
config_l3_exhaust_route(int unit)
{
    bcm_error_t rv;
    bcm_mac_t router_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    bcm_vlan_t ing_vlan = 2;
    bcm_port_t ing_port = ingress_port;
    bcm_if_t egr_if;
    bcm_ip_t base_subnet_addr = 0xC0A80110;
    bcm_ip_t base_addr_msk = 0xFFFFFFFF;
    int route_entries = 1024;
    bcm_ip6_t base_ip6_subnet_addr = {0x3F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    bcm_ip6_t base_ip6_addr_msk = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    int i;

    rv = configure_route(unit, &egr_if);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring route: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = add_router_mac(unit, router_mac, ing_vlan, ing_port);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding router mac: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    for(i=0; i<route_entries; i++) {

        rv = add_ipv6_route_entries(unit, egr_if, base_ip6_subnet_addr,
                                    base_ip6_addr_msk, i, 120);
        if(BCM_FAILURE(rv)) {
            printf("\nAdd_IPv6_host_entries failed in %d entry insertion in /120
                   subnet: %s\n",i, bcm_errmsg(rv));
            break;
        }

        rv = add_ipv4_route_entries(unit, egr_if, base_subnet_addr,
                                    base_addr_msk, i, 28);
        if(BCM_FAILURE(rv)) {
            printf("\nAdd_IPv4_host_entries failed in %d entry insertion in /28
                   subnet: %s\n", i, bcm_errmsg(rv));
            break;
        }

        rv = add_ipv6_route_entries(unit, egr_if, base_ip6_subnet_addr,
                                    base_ip6_addr_msk, i, 60);
        if(BCM_FAILURE(rv)) {
            printf("\nAdd_IPv6_host_entries failed in %d entry insertion in /60
                   subnet: %s\n",i, bcm_errmsg(rv));
            break;
        }

        rv = add_ipv4_route_entries(unit, egr_if, base_subnet_addr,
                                    base_addr_msk, i, 24);
        if(BCM_FAILURE(rv)) {
            printf("\nAdd_IPv4_host_entries failed in %d entry insertion in /24
                   subnet: %s\n", i, bcm_errmsg(rv));
            break;
        }

        rv = add_ipv6_route_entries(unit, egr_if, base_ip6_subnet_addr,
                                    base_ip6_addr_msk, i, 68);
        if(BCM_FAILURE(rv)) {
            printf("\nAdd_IPv6_host_entries failed in %d entry insertion in /68
                   subnet: %s\n",i, bcm_errmsg(rv));
            break;
        }

        rv = add_ipv6_route_entries(unit, egr_if, base_ip6_subnet_addr,
                                    base_ip6_addr_msk, i, 56);
        if(BCM_FAILURE(rv)) {
            printf("\nAdd_IPv6_host_entries failed in %d entry insertion in /56
                   subnet: %s\n",i, bcm_errmsg(rv));
            break;
        }

        rv = add_ipv4_route_entries(unit, egr_if, base_subnet_addr,
                                    base_addr_msk, i, 32);
        if(BCM_FAILURE(rv)) {
            printf("\nAdd_IPv4_host_entries failed in %d entry insertion in /32
                   subnet: %s\n", i, bcm_errmsg(rv));
            break;
        }

        rv = add_ipv6_route_entries(unit, egr_if, base_ip6_subnet_addr,
                                    base_ip6_addr_msk, i, 128);
        if(BCM_FAILURE(rv)) {
            printf("\nAdd_IPv6_host_entries failed in %d entry insertion in /128
                   subnet: %s\n",i, bcm_errmsg(rv));
            break;
        }

        rv = add_ipv4_route_entries(unit, egr_if, base_subnet_addr,
                                    base_addr_msk, i, 30);
        if(BCM_FAILURE(rv)) {
            printf("\nAdd_IPv4_host_entries failed in %d entry insertion in /30
                   subnet: %s\n", i, bcm_errmsg(rv));
            break;
        }

        rv = add_ipv6_route_entries(unit, egr_if, base_ip6_subnet_addr,
                                    base_ip6_addr_msk, i, 64);
        if(BCM_FAILURE(rv)) {
            printf("\nAdd_IPv6_host_entries failed in %d entry insertion in /64
                   subnet: %s\n",i, bcm_errmsg(rv));
            break;
        }
    }

    printf(" Added %d ipv4 routes and %d ipv6 routes\n", added_ip4_routes,
           added_ip6_routes);

    rv = display_l3_info(unit);
    if (BCM_FAILURE(rv)) {
        printf("Error in displaying l3 info : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * execute:
 *  This function does the following
 *  a) test setup
 *  b) actual configuration (Done in config_l3_exhaust_route())
 *  c) demonstrates the functionality(done in verify()).
 */
bcm_error_t
execute(void)
{
    bcm_error_t rv;
    int unit = 0;
    print "config show; attach; cancun stat; version";
    bshell(unit, "config show; a ; cancun stat; version");

    print "~~~ #1) test_setup(): ** start **";
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }
    print "~~~ #1) test_setup(): ** end **";

    print "~~~ #2) config_l3_exhaust_route(): ** start **";
    if (BCM_FAILURE((rv = config_l3_exhaust_route(unit)))) {
        printf("config_l3_exhaust_route() failed.\n");
        return -1;
    }
    print "~~~ #2) config_l3_exhaust_route(): ** end **";

    print "~~~ #3) verify(): ** start **";
    verify(unit);
    print "~~~ #3) verify(): ** end **";

    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print "execute(): Start";
  print execute();
  print "execute(): End";
}
