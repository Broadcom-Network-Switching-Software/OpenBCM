/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Feature  : L3 IPv4 Host/Entry scaling/filling of the entire table.
 *
 * Usage    : BCM.0> cint exhaust_ipv4.c
 *
 * config   : exhaust_ipv4_config.bcm
 *            'config add l3_mem_entries=114688'
 *
 * Log file : exhaust_ipv4_log.txt
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
 *   This Cint example to show configuration of the IPv4 host scaling scenario
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
 *   2) Step2 - Configuration (Done in config_exhaust_ipv4_host()):
 *   =========================================================
 *     a) Configure a basic IPv4 host/entry functional scenario along with
 *        scaling/filling the entire table. This adds the hosts in l3 table[host]
 *        and does the necessary configurations of vlan, interface and next hop.
 *
 *     b) Before BCMINIT, Makesure to set this config variable 'config add
 *        l3_mem_entries=212992' in config.bcm. so this setting enables to
 *        use UFT mode 3 for maximum L3 HOST entries.
 *
 *   3) Step3 - Verification (Done in verify()):
 *   ===========================================
 *     a) display_hash_config - Displays the hash configuration.
 *     b) display_l3_info - Displays the configured L3 information.
 *
 *     c) Expected Result:
 *     ===================
 *       We can see that, maximum entries/interfaces/nexthops related to L3 host
 *       info and entries used/added and utilisation/remaining percentage and so on.
 */

/* Reset C interpreter*/
cint_reset();

bcm_port_t ingress_port;
bcm_port_t egress_port;

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

/* Steps to configure IPv4 host */
int
configure_host(int unit , bcm_if_t *egr_if)
{
    bcm_error_t rv;
    bcm_if_t egr_l3_if;
    bcm_mac_t src_mac = {0x00, 0x00, 0x00, 0x0, 0x00, 0x05};
    bcm_mac_t dst_mac = {0x00, 0x00, 0x11, 0x11, 0x22, 0x25};
    bcm_port_t egr_port = egress_port;
    bcm_gport_t egr_gport;
    bcm_vlan_t egr_vlan = 5;

    BCM_IF_ERROR_RETURN(bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 0x1));

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

/* Adding IP address to host[L3] table */
int
add_host(int unit, bcm_ip_t ip_address, bcm_if_t egr_if)
{
    bcm_l3_host_t host;

    bcm_l3_host_t_init(&host);

    host.l3a_intf = egr_if;
    host.l3a_ip_addr = ip_address;


    BCM_IF_ERROR_RETURN(bcm_l3_host_add(unit, &host));

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

int
add_IPv4_host_entries(int unit, int num_host_entries, bcm_if_t egr_if,
                        bcm_ip_t base_ip_addr, int keep_trying)
{
    bcm_error_t rv;
    bcm_ip_t ip_addr;
    int i;
    int missed = 0;
    int first_miss = -1;
    int added;

    printf("Add %d IPv4 host entries \n", num_host_entries );
    for (i = 0; i < num_host_entries; i++) {
        ip_addr = base_ip_addr + i;

        rv = add_host(unit, ip_addr, egr_if);
        if (BCM_FAILURE(rv)) {
            if (keep_trying && (rv == BCM_E_FULL)) {
                /* Keep trying to add new entries even after first table full */
                if (first_miss < 0) {
                    first_miss = i + 1;
                }
                missed++;
            } else {
                printf("    bcm_l3_host_add() failed at iteration %d: %s\n", i + 1,
                       bcm_errmsg(rv));
                missed = num_host_entries - i;
                break;
            }
        }
    }
    added = num_host_entries - missed;
    printf("    Added %d IPv4 host entries;", added);
    if (keep_trying) {
        printf(" %d missed;", missed);
        if (missed) {
            printf(" first miss: %d;\n", first_miss);
        }
    }
    printf("\n");
    return ((rv != BCM_E_NONE) && (rv != BCM_E_FULL)) ? rv :
      (keep_trying && (added > 0)) ||
      (!keep_trying && (added == num_host_entries)) ? BCM_E_NONE : BCM_E_FULL;
}

int
configure_hash_multi_move_depth(int unit, int depth)
{
    bcm_error_t rv;
    int count = 2;
    int i;
    bcm_switch_control_t controls[count] = {
        bcmSwitchHashMultiMoveDepth,
        bcmSwitchHashMultiMoveDepthL3
    };
    char *control_names[count] = {
        "HashMultiMoveDepth",
        "HashMultiMoveDepthL3"
    };

    for (i = 0; i <count; i++) {
        if (BCM_FAILURE(rv = bcm_switch_control_set(unit, controls[i], depth))) {
            if (rv == BCM_E_UNAVAIL) {
                printf("Cannot set multi-move depth, %s not available\n",
                       control_names[i]);
            } else {
                return rv;
            }
        }
    }
    return BCM_E_NONE;
}

int
display_hash_config(int unit)
{
    bcm_error_t rv;
    int count = 2;
    int i;
    int arg;
    uint32 bank_count;

    bcm_switch_control_t controls[count] = {
        bcmSwitchHashMultiMoveDepth,
        bcmSwitchHashMultiMoveDepthL3,
    };

    char *control_names[count] = {
        "HashMultiMoveDepth",
        "HashMultiMoveDepthL3",
    };

    for (i = 0; i < count; i++) {
        if (BCM_FAILURE(rv = bcm_switch_control_get(unit, controls[i], &arg))) {
            if (rv == BCM_E_UNAVAIL) {
                printf("%s not available\n", control_names[i]);
            } else {
                return rv;
            }
        } else {
            printf("%s = %d\n", control_names[i], arg);
        }
    }
    if (BCM_SUCCESS(bcm_switch_hash_banks_max_get(unit, bcmHashTableL3, &bank_count))) {
        uint32 bank;
        int hash_type;
        uint32 hash_offset;

        printf("Hash Bank Configuration\n");
        for (bank = 0; bank < bank_count; bank++) {
            BCM_IF_ERROR_RETURN(bcm_switch_hash_banks_config_get(unit, bcmHashTableL3,
                                                                 bank, &hash_type,
                                                                 &hash_offset));
            switch (hash_type) {
              case BCM_HASH_LSB:
                  printf("  Bank %u: LSB\n", bank);
                  break;
              case BCM_HASH_CRC16U:
                  printf("  Bank %u: Upper CRC16\n", bank);
                  break;
              case BCM_HASH_CRC16L:
                  printf("  Bank %u: Lower CRC16\n", bank);
                  break;
              case BCM_HASH_CRC32U:
                  printf("  Bank %u: Upper CRC32\n", bank);
                  break;
              case BCM_HASH_CRC32L:
                  printf("  Bank %u: Lower CRC32\n", bank);
                  break;
              case BCM_HASH_OFFSET:
                  printf("  Bank %u: Hash Offset; Offset=%u\n", bank, hash_offset);
                  break;
              default:
                  printf("  Bank %u: Hash Type = %d; Offset=%u\n", bank, hash_type,
                         hash_offset);
                  break;
            }
        }
    }
    return BCM_E_NONE;
}

int
display_l3_info(int unit)
{
    bcm_l3_info_t l3info;

    BCM_IF_ERROR_RETURN(bcm_l3_info(unit, &l3info));

    printf("L3 INFO:\n");
    printf("  L3 host table size (unit is IPv4 unicast): %d\n", l3info.l3info_max_host);
    printf("  L3 host entries used: %d (%d remaining, %d%% utilization)\n",
           l3info.l3info_used_host, l3info.l3info_max_host - l3info.l3info_used_host,
           ((l3info.l3info_used_host * 100) +
            (l3info.l3info_used_host / 2)) / l3info.l3info_max_host);
    printf("  NextHops used: %d\n", l3info.l3info_used_nexthop);
    printf("  L3 interfaces used: %d\n", l3info.l3info_used_intf);
    printf("  Maximum L3 interface groups the chip supports: %d\n",
           l3info.l3info_max_intf_group);
    printf("  Maximum L3 interfaces the chip supports: %d\n", l3info.l3info_max_intf);
    printf("  Maximum NextHops: %d\n", l3info.l3info_max_nexthop);

    return BCM_E_NONE;
}

int
configure_hash_banks(int unit)

{

    int no_of_banks = 7;

    uint32 config[7] = {0|BCM_HASH_OFFSET, 27|BCM_HASH_OFFSET, 32|BCM_HASH_OFFSET,
                       43|BCM_HASH_OFFSET, 40|BCM_HASH_OFFSET, 50|BCM_HASH_OFFSET,
                       1|BCM_HASH_OFFSET};

    uint32 bank;



    for (bank = 0; bank < no_of_banks; bank++) {

        int hash_type;

        uint32 hash_offset;



        if (config[bank] & BCM_HASH_OFFSET) {

            hash_type = BCM_HASH_OFFSET;

            hash_offset = config[bank] & ~BCM_HASH_OFFSET;

        } else {

            hash_type = config[bank];

            hash_offset = 0;

        }



        BCM_IF_ERROR_RETURN(bcm_switch_hash_banks_config_set(unit, bcmHashTableL3,

                                                             bank, hash_type,

                                                             hash_offset));

    }


   return BCM_E_NONE;

}

bcm_error_t
config_exhaust_ipv4_host(int unit)
{
    bcm_error_t rv;
    bcm_mac_t router_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    bcm_vlan_t ing_vlan = 2;
    bcm_port_t ing_port = ingress_port;
    bcm_if_t egr_if;
    bcm_ip_t ip_addr = 0xC0A80114;
    int hash_move_depth = 3; /* Specific recursion depth for hash multi-move, set to 0 to disable hash multi move */
    int keep_trying = 1;  /* In "keep_trying" mode, keep trying to add entries after the first "table full" */

    /*
     * Number of entries in host table depends on the UFT Mode selected. In the
     * below example Mode 3 is selected. This has to be changed depending on the
     * l3_mem_entries given in the configuration file
     */
    int host_ipv4_entries = 114688;

    rv = configure_hash_multi_move_depth(unit, hash_move_depth);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring hash multi move : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_hash_banks(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in configuring hash banks : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = display_hash_config(unit);
    if (BCM_FAILURE(rv)) {
        printf("\nError in displaying hash bank config : %s.\n", bcm_errmsg(rv));
        return rv;
    }

    rv = configure_host(unit, &egr_if);
    if(BCM_FAILURE(rv)) {
        printf("\nError in configuring host: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = add_router_mac(unit, router_mac, ing_vlan, ing_port);
    if(BCM_FAILURE(rv)) {
        printf("\nError in adding router mac: %s.\n",bcm_errmsg(rv));
        return rv;
    }
   /* Create IPv4 host entries */
    rv = add_IPv4_host_entries(unit, host_ipv4_entries, egr_if, ip_addr,
                               keep_trying);
    if(BCM_FAILURE(rv)) {
        printf("\nadd_IPv4_host_entries : failed : rv == %d\n",rv);

        return rv;
    }

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
 *  b) actual configuration (Done in config_exhaust_ipv4_host())
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

    print "~~~ #2) config_exhaust_ipv4_host(): ** start **";
    if (BCM_FAILURE((rv = config_exhaust_ipv4_host(unit)))) {
        printf("config_exhaust_ipv4_host() failed.\n");
        return -1;
    }
    print "~~~ #2) config_exhaust_ipv4_host(): ** end **";

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
