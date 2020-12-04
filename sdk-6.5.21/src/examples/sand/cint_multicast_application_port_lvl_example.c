/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*~~~~~~~~~~~~~~~~Multicast Options: Port-Level Scheduled (Fully Scheduled)~~~~~~~~~~~~~~~~~~*/

/*
 *
 * $Copyright: (c) 2010 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File: cint_multicast_application_port_lvl_example.c
 * Purpose: Example of port level multicast configuration.
 * 
 * The configuration needed is defined in the cint_multicast_applications.c file.
 * The configuration includes the following:
 *   o  Ingress FAPs: Open MC group defined by a set of {Destination System Ports, Copy-Unique-Data (CUD)}.
 *   o  Egress FAPs: None.
 */

/* Need to also load  <cint_multicast_applications.c> */

/* 
 * 2 examples of port level scheduled multicast
 * one example based on 2 system ports gport.
 * one example based on 2 VOQs gport.
 */

/*
 * The script Assumes Queues and Ports related are already defined (also ERP).
 * In the example we will assume diag_init application was executed.
 * User can specify different parameters but need to make sure
 * Queues and related physical ports are created.
 * The script can run either PP disabled or enabled, changing the traffic setup.
 */

/*
 * Parameters defined
 * Ports: Destination Port to transmit multicast replication packet
 * CUD packet for destination ports
 * Specify mode: is tm: assumes packets being transmit by ITMH. Run ITMH with destination: multicast_id.
 *               If not, creating mac address to send packets to related destinations.
 * Multicast ID: Multicast ID for example1
 */
int is_tm = 0;
uint8 incoming_mac_mc[6] = { 0, 0, 0, 0, 0, 0x1 };      /* 00:00:00:00:00:01 */
int incoming_vlan_mc = 1;

bcm_port_t ports_example[2];
int cud = BCM_IF_INVALID;
int multicast_id_1 = 10;

/* Example of system ports */
int
run_port_lvl_multicast_example1(
    int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t system_ports[2];
    bcm_gport_t local_gport;
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
     * Convert local port to system port 
     */
    BCM_GPORT_LOCAL_SET(local_gport, ports_example[0]);
    rv = bcm_stk_gport_sysport_get(unit, local_gport, system_ports);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_petra_stk_gport_sysport_get failed: (%s) \n", bcm_errmsg(rv));
        return rv;
    }
    BCM_GPORT_LOCAL_SET(local_gport, ports_example[1]);
    rv = bcm_stk_gport_sysport_get(unit, local_gport, &system_ports[1]);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_petra_stk_gport_sysport_get failed: (%s) \n", bcm_errmsg(rv));
        return rv;
    }
    cuds[0] = cud;
    cuds[1] = cud;

    rv = port_level_scheduled_multicast(unit, multicast_id_1, system_ports, cuds, 2);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }

    return rv;
}

/* Example of VOQs */
uint8 incoming_mac_mc_2[6] = { 0, 0, 0, 0, 0, 0x2 };    /* 00:00:00:00:00:02 */
int incoming_vlan_mc_2 = 1;

int VOQ[2];
int cud2 = BCM_IF_INVALID;
int multicast_id_2 = 11;
int
run_port_lvl_multicast_example2(
    int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t voqs[2];
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

    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voqs[0], VOQ[0]);
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voqs[1], VOQ[1]);

    cuds[0] = cud2;
    cuds[1] = cud2;

    rv = port_level_scheduled_multicast(unit, multicast_id_2, voqs, cuds, 2);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }

    return rv;
}

/*util function to set relevent param for arad and PetraB*/
int
set_params_port_lvl_multicast(
    int unit,
    int is_arad)
{
    return set_params_port_lvl_multicast_with_ports(unit, 1, (is_arad ? 14 : 4), 1, BCM_IF_INVALID);
}

/*Utility function to set relevent param for Arad and PetraB*/
int
set_params_port_lvl_multicast_with_ports(
    int unit,
    int port0,
    int port1,
    int is_arad,
    int cud0)
{

    int rv = BCM_E_NONE;
    int i = 0;
    bcm_gport_t sys_gport, voq_gport;

    ports_example[0] = port0;
    ports_example[1] = port1;

    if (is_device_or_above(unit, JERICHO2))
    {
        for (i = 0; i < 2; i++)
        {
            rv = bcm_stk_gport_sysport_get(unit, ports_example[i], &sys_gport);
            if (BCM_E_NONE != rv)
            {
                printf("bcm_stk_gport_sysport_get failed \n");
                return rv;
            }

            rv = bcm_cosq_sysport_ingress_queue_map_get(unit, 0, sys_gport, &voq_gport);
            if (BCM_E_NONE != rv)
            {
                printf("bcm_cosq_sysport_ingress_queue_map_get failed \n");
                return rv;
            }
            VOQ[i] = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(voq_gport);
        }
    }
    else
    {
        VOQ[0] = is_arad ? 32 + ((ports_example[0] - 1) * 8) : 8;
        VOQ[1] = is_arad ? 32 + ((ports_example[1] - 1) * 8) : 32;
    }

    cud = is_arad ? cud0 : 1;
    cud2 = is_arad ? cud0 : 2;
    return BCM_E_NONE;
}

int
set_is_tm(
    int val)
{
    is_tm = val;
    return BCM_E_NONE;
}
