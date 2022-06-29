/* 
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *  
 * Example walkthrough: 
 *  
 * This example shows how to configure a system of a single FE and multiple faps to
 * replicate multicast packets in the fabric and the fap's egress. 
 * The FE function configs the fabric to replicate data to all given FAPs.
 * The FAP function configs the FAP egress to replicate data to all the given ports. 
 *
 * Usage example : 
 * In the following example we assume a mgmt system which contains
 * 2 FAPs and 1 FE that connects them.
 * The FE will replicate the packets to the FAPs, and the egress of
 * the FAPs will replicate them to the ports.
 *  
 * cint src/examples/dnxc/cint_fabric_egress_mc.c
 * cint
 *
 * int fap_0_unit = 0;
 * int fe_unit = 1;
 * int fap_1_unit = 2;
 *
 * bcm_module_t modids_array[] = {0, 2};
 *
 * bcm_port_t fap_0_ports[] = {15, 16};
 * bcm_port_t fap_1_ports[] = {13, 14};
 *
 * bcm_mac_t mac = {1, 0, 0, 0, 0, 1};
 * bcm_vlan_t vlan = 113;
 *
 * bcm_multicast_t mc_id = 5000;
 *
 * print cint_fap_l2_addr_add_mc(fap_0_unit, mac, vlan, mc_id);
 * print cint_fe_fabric_multicast_set(fe_unit, mc_id, 4, modids_array);
 * print cint_fap_egress_multicast_set(fap_0_unit, mc_id, 2, fap_0_ports);
 * print cint_fap_egress_multicast_set(fap_1_unit, mc_id, 2, fap_1_ports);
 */

/*
 * Function:
 *      cint_fap_l2_addr_add_mc
 * Purpose:
 *      Add entry to l2 table, which set incomming packets with mac
 *      address and vlan id to a multicast group.
 * Parameters:
 *      fap_unit - unit of the configured fap.
 *      mac      - mac address.
 *      vlan     - vlan id.
 *      mc_id    - multicast group id.
 */
int cint_fap_l2_addr_add_mc(int fap_unit, bcm_mac_t mac, bcm_vlan_t vlan, bcm_multicast_t mc_id)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vlan);
    l2_addr.flags = BCM_L2_MCAST;
    l2_addr.l2mc_group = mc_id;

    rv = bcm_l2_addr_add(fap_unit, &l2_addr);
    if (BCM_FAILURE(rv))
    {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}

/*
 * Function:
 *      cint_fe_fabric_multicast_set
 * Purpose:
 *      Create multicast group, and associate module ids with it.
 *      The module ids array should contain only 1 modid of the destination FAP.
 *      If destination FAP has several cores, its egress is responsible to replicate
 *      to the appropriate modids.
 * Parameters:
 *      fe_unit      - unit of the configured fe.
 *      mc_id        - multicast group id.
 *      nof_modids   - number of module ids in modids_array.
 *      modids_array - array of module ids.
 */
int cint_fe_fabric_multicast_set(int fe_unit, bcm_multicast_t mc_id, int nof_modids, bcm_module_t * modids_array)
{
    int rv = BCM_E_NONE;

    rv = bcm_multicast_create(fe_unit, BCM_MULTICAST_WITH_ID, &mc_id);
    if (BCM_FAILURE(rv))
    {
        printf("Error, in bcm_multicast_create, mc_id=%d, \n", mc_id);
        return rv;
    }

    rv = bcm_fabric_multicast_set(fe_unit, mc_id, 0, nof_modids, modids_array);
    if (BCM_FAILURE(rv))
    {
        printf("Error, in bcm_fabric_multicast_set, mc_id=%d, \n", mc_id);
        return rv;
    }

    return rv;
}

int cint_fap_egress_multicast_is_multicast_v2_supported(int fap_unit, int *egress_multicast_v2_supported)
{
    int rv = BCM_E_NONE;
    int is_dnx = 0;

    rv = bcm_device_member_get(fap_unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv))
    {
        printf("Error, in bcm_device_member_get, unit %d.\n", fap_unit);
        return rv;
    }
    if(is_dnx)
    {
        /* DNX data will fail on DPP devices in mixed DNX/DPP systems */
        *egress_multicast_v2_supported = *(dnxc_data_get(fap_unit, "multicast", "params", "mcdb_formats_v2", NULL));
    }
    else
    {
        *egress_multicast_v2_supported = 0;
    }
    return rv;
}

/* 
 * Create multicast id, and set egress of unit to it.
 */
/*
 * Function:
 *      cint_fap_egress_multicast_set
 * Purpose:
 *      Create egress multicast group, and associate ports with it.
 * Parameters:
 *      fap_unit      - unit of the configured fap.
 *      mc_id         - multicast group id.
 *      nof_ports     - number of ports in ports_array.
 *      ports_array   - array of ports.
 */
int cint_fap_egress_multicast_set(int fap_unit, bcm_multicast_t mc_id, int nof_ports, bcm_port_t * ports_array)
{
    int rv = BCM_E_NONE;
    bcm_multicast_replication_t rep[nof_ports];
    int port_index;
    int egress_multicast_v2_supported = 0;
    int multicast_create_flags = 0;

    rv = cint_fap_egress_multicast_is_multicast_v2_supported(fap_unit, &egress_multicast_v2_supported);
    if (BCM_FAILURE(rv))
    {
        printf("Error, in cint_fap_egress_multicast_is_multicast_v2_supported.\n");
        return rv;
    }

    if (egress_multicast_v2_supported)
    {
        multicast_create_flags = (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC);
        
    }
    else
    {
        multicast_create_flags = BCM_MULTICAST_EGRESS_GROUP;
    }

    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        BCM_GPORT_LOCAL_SET(rep[port_index].port, ports_array[port_index]);
        rep[port_index].encap1 = -1;
    }

    rv = bcm_multicast_create(fap_unit, (multicast_create_flags | BCM_MULTICAST_WITH_ID), &mc_id);
    if (BCM_FAILURE(rv))
    {
        printf("Error, in bcm_multicast_create, mc_id=%d\n", mc_id);
        return rv;
    }

    rv = bcm_multicast_set(fap_unit, mc_id, multicast_create_flags, nof_ports, rep);
    if (BCM_FAILURE(rv))
    {
        printf("Error, in bcm_multicast_set, mc_id=%d\n", mc_id);
        return rv;
    }

    return rv;
}

/* 
 * Delete multicast id, and set egress of unit to it.
 */
/*
 * Function:
 *      cint_fap_egress_multicast_delete
 * Purpose:
 *      Delete egress multicast group, and associate ports with it.
 * Parameters:
 *      fap_unit      - unit of the configured fap.
 *      mc_id         - multicast group id.
 *      nof_ports     - number of ports in ports_array.
 *      ports_array   - array of ports.
 */
int cint_fap_egress_multicast_delete(int fap_unit, bcm_multicast_t mc_id, int nof_ports, bcm_port_t * ports_array)
{
    int rv = BCM_E_NONE;
    bcm_multicast_replication_t rep[nof_ports];
    int port_index;
    int egress_multicast_v2_supported = 0;
    int multicast_delete_flags = 0;

    rv = cint_fap_egress_multicast_is_multicast_v2_supported(fap_unit, &egress_multicast_v2_supported);
    if (BCM_FAILURE(rv))
    {
        printf("Error, in cint_fap_egress_multicast_is_multicast_v2_supported.\n");
        return rv;
    }

    if (egress_multicast_v2_supported)
    {
        multicast_delete_flags = (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC);
        
    }
    else
    {
        multicast_delete_flags = BCM_MULTICAST_EGRESS_GROUP;
    }

    for (port_index = 0; port_index < nof_ports; ++port_index)
    {
        BCM_GPORT_LOCAL_SET(rep[port_index].port, ports_array[port_index]);
        rep[port_index].encap1 = -1;
    }

    rv = bcm_multicast_delete(fap_unit, mc_id, multicast_delete_flags, nof_ports, rep);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_multicast_delete (%d)\n",fap_unit);
        return rv;
    }

    return rv;
}