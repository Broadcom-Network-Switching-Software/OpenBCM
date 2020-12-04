/*~~~~~~~~~~~~~~~~~~Multicast Options: Device-Level Scheduled~~~~~~~~~~~~~~~~~~~~*/
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_multicast_application_device_lvl_example.c
 * Purpose: Example of device level multicast configuration.
 * 
 * The configuration needed is defined in the cint_multicast_applications.c file.
 * The configuration includes the following: 
 *  o   Ingress FAPs: Open MC group defined by a set of destination FAPs. 
 *  o   Egress FAPs: 
 *      For each MC group holding a member port on this FAP, open a group using one of two methods:
 *          o   LAN Membership - the MC group is defined by the set of destination (local) ports
 *          o   General  - the MC group is defined by the set of {destination local ports, CUD}
 */

/* Need to also load  <cint_multicast_applications.c> */

/* 2 Examples of device level partially scheduled multicast 
 * In first example destination based on 2 local ports 
 * and cud is not important (different ports) thus 
 * we use egress multicast type vlan membership.
 * In second example destionation basded on 1 local port
 * but with two different cuds. Thus we use egress multicast
 * type linked list.
 */

/* 
 * The script Assumes Queues and Ports related are already defined. 
 * In the example we will assume diag_init application was executed. 
 * User can specify different parameters but need to make sure 
 * Queues and related physical ports are created. 
 * Also pay attention multicast group, use L2 reserved group (for vlan membership).
 */

/* 
 * Parameters defined 
 * Ports: Destination Port to transmit multicast replication packet 
 * CUD packet for destination ports
 * Specify mode: is tm: assumes packets being transmit by ITMH. Run ITMH with destinatino: multicast_id. 
 *               If not, creating mac address to send packets to related destinations.
 * Multicast ID: Multicast ID for example1
 */
int is_tm = 0;
uint8 incoming_mac_mc[6] = { 0, 0, 0, 0, 0, 0x3 };      /* 00:00:00:00:00:03 */
int incoming_vlan_mc = 2;

bcm_port_t ports_example[2] = { 1, 4 };
int cud = 0;
int multicast_id_1 = 12;

/* Example of system ports */
int
run_device_lvl_multicast_example1(
    int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t local_ports[2];
    int cuds[2];

    if (!is_tm)
    {
        bcm_l2_addr_t l2_addr;

        bcm_l2_addr_t_init(&l2_addr, incoming_mac_mc, incoming_vlan_mc);
        l2_addr.flags = 0x00000020 | BCM_L2_MCAST;
        l2_addr.l2mc_group = multicast_id_1;

        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in setup_mac_forwarding\n");
        }
    }

    /*
     * Convert local port to gport 
     */
    BCM_GPORT_LOCAL_SET(local_ports[0], ports_example[0]);
    BCM_GPORT_LOCAL_SET(local_ports[1], ports_example[1]);

    cuds[0] = cud;
    cuds[1] = cud;

    rv = device_level_scheduled_multicast(unit, multicast_id_1, local_ports, cuds, 2);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }

    return rv;
}

/* Example of VOQs */
uint8 incoming_mac_mc_2[6] = { 0, 0, 0, 0, 0, 0x4 };    /* 00:00:00:00:00:04 */
int incoming_vlan_mc_2 = 3;

bcm_port_t port = 1;
int cud_2[2] = { 1, 2 };
/* Multicast id must be higher than 4K for linked list type */
/* duplicate the packtes for both cud 1 and 2 */
int multicast_id_2 = 5005;

int
run_device_lvl_multicast_example2(
    int unit)
{
    bcm_error_t rv;
    bcm_gport_t local_ports[2];
    int cuds[2];

    if (!is_tm)
    {
        bcm_l2_addr_t l2_addr;

        bcm_l2_addr_t_init(&l2_addr, incoming_mac_mc_2, incoming_vlan_mc_2);
        l2_addr.flags = 0x00000020 | BCM_L2_MCAST;
        l2_addr.l2mc_group = multicast_id_2;

        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in setup_mac_forwarding\n");
        }
    }

    /*
     * Convert local port to gport. Destination based on 1 local port but with two different cuds. Thus we use egress multicast type linked list.
     */
    BCM_GPORT_LOCAL_SET(local_ports[0], ports_example[0]);
    BCM_GPORT_LOCAL_SET(local_ports[1], ports_example[1]);

    cuds[0] = cud_2[0];
    cuds[1] = cud_2[1];

    rv = device_level_scheduled_multicast(unit, multicast_id_2, local_ports, cuds, 2);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }

    return rv;
}

/*util function to set relevent param for arad and PetraB*/
int
set_params_device_lvl_multicast(
    int is_arad)
{
    return set_params_device_lvl_multicast_with_ports(1, (is_arad ? 14 : 4), 1);
}

/*util function to set relevent param for arad and PetraB*/
int
set_params_device_lvl_multicast_with_ports(
    int port0,
    int port1,
    int is_arad)
{
    ports_example[0] = port0;
    ports_example[1] = port1;

    cud = is_arad ? BCM_IF_INVALID : 1;
    cud_2[0] = is_arad ? BCM_IF_INVALID : 1;
    cud_2[1] = is_arad ? BCM_IF_INVALID : 2;
    return BCM_E_NONE;
}


