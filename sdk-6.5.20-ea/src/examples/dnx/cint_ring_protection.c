/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~L2 Ring Protection Fast Flush test~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_l2_fast_flush.c
 * Purpose: Example for using L2 Ring protection Fast Flush APIs with an
 *          optional ERP configuration.
 *
 * Usage:
 * cint;
 * int rv;
 * rv = ring_protection_create(int *units_ids, int nof_units, int outSysport, int inSysport);
 * 
 * This cint uses cint_vswitch_metro_mp.c to build the following vswitch
 * The first 3 vlan-ports configured on the outSysport and the last one on inSysport.
 * at multi-unit setup, outSysport is defined on unit#1 and inSysport defined on unit#0
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *  |                                                                       |
 *  |                   o  \                         o                      |
 *  |                    \  \<4,8>        <10,20>/  /                       |
 *  |                  /\ \  \   -----------    /  /                        |
 *  |                   \  \ \/ /  \   /\   \  /  / /\                      |
 *  |             <40,80>\  \  |    \  /     |\/ /  /                       |
 *  |                     \  \ |     \/      |  /  /<30,60>                 |
 *  |                       p3\|     /\      | /  /                         |
 *  |                          |    /  \     |/p1                           |             
 *  |                         /|   \/   \    |                              |
 *  |                        /  \  VSwitch  / \p4                           |
 *  |                   /\  /p2  -----------   \                            |
 *  |              <5,2>/  /  /              /\ \ \                         |
 *  |                  /  /  /<3,6> <100,150> \  \ \ <200,250>              |
 *  |                 /  /  /                  \  \ \                       |
 *  |                   /  \/                   \  \\/                      |
 *  |                  o                            o                       |
 *  |                                                                       | 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * The functions support ERP Blocking/Unblocking operations as well as fast flushing of the mac table per ERP group
 *
 *
 *  Step 1:
 *      Create Ring Protection groups - ring_protection_create()
 *      The default setting will be create the following two
 *      Ring Protection groups:
 *          1. ring_group_1 - Includes p1 & p2.
 *          2. ring_group_2 - Includes p3 only.
 * 
 *      Calling sequence:
 *          1. Create an ERP failover - ring_protection_erp_failover_create (bcm_failover_create)
 *          2. Create vswitch with ring protection vlan-ports as described in cint_vswitch_metro_mp.c.
 *
 *      Result: After the Ring groups are configured a user can send packets
 *              through the relevant ports (with different SAs):
 *              1. P1 - OuterVlan = 10 (TPID - 0x8100); InnerVlan = 20 (TPID - 0x9100); 
 *              2. P2 - OuterVlan = 5; InnerVlan = 2;
 *              3. P3 - OuterVlan = 4; InnerVlan = 8;
 * 
 *              Then, the MAC table can be watched by applying "l2 show" or counted with "L2EntryCountGet"
 *          Example:
 *              L2EntryCountGet <unit number>
 * 
 *              from the BCM shell. The forwarding will show:
 *              1. P1 & P2 entries will have the same flush group as ring#1
 *              2. P3 entries will have the same flush group as ring#2
 * 
 *  Step 2:
 *      Move a port for from one group to another - ring_protection_outlif_to_erp_associate();
 *
 *      Calling sequence:
 *          1. update vlan port calling bcm_vlan_port_create() with BCM_VLAN_PORT_REPLACE
 *
 *      Result: Moving a port from one group will effect the MAC table if we will
 *              move P2 and resend the packet for the moved vlan port,
 *              When applying "l2 show", the forwarding will show:
 *              1. P1 entries will have the same flush group as ring#1
 *              2. P2 & P3 entries will have the same flush group as ring#2
 * 
 * 
 *  Step 3:
 *      Perform Fast Flush for a Ring group - ring_protection_fast_flush();

 *      Calling sequence:
 *          1. Block the LIFs using bcm_failover_set().
 *          2. Call bcm_l2_replace() with the flush group id as a parameter
 *          3. Unblock the LIFs using bcm_failover_set().
 *
 *      Result: The Fast Flush should remove the the entries from all the
 *              ports that are associated with the flush group.
 * 
 *  Step 4:
 *      Remove all ports and failover groups -
 *          ring_protection_destroy();
 *
 *      Calling sequence:
 *          1. bcm_failover_destroy().
 *          2. bcm_vlan_port_destroy().
 */ 


  int NOF_RING_VP_GPORTS = 3; /** The number of protected vlan-ports created at this test*/
  int NOF_NO_RING_VP_GPORTS = 1; /** The number of not-protected vlan-ports created at this test*/
  int NOF_VP_GPORTS = NOF_RING_VP_GPORTS + NOF_NO_RING_VP_GPORTS; /** The total number of vlan-ports */
  int NOF_ERP_GROUPS = 2; /** The number of erp groups created at this test */

  /*
   * holds the gports representing the vlan-ports created at this test
   */
  bcm_gport_t outlifs[NOF_VP_GPORTS];

  /*
   * holds the flush group and rx, tx failover information per ERP group
   */
  uint32 flush_group_ids[NOF_ERP_GROUPS]= {1, 2};;
  bcm_failover_t rx_failovers[NOF_ERP_GROUPS];
  bcm_failover_t tx_failovers[NOF_ERP_GROUPS];

/*
 * Create the vlan-ports as explained above
 */
int ring_protection_vlan_port_create(int *units_ids, int nof_units, int out_sysport, int in_sysport)
{
    int port_1 = out_sysport;
    int port_2 = out_sysport;
    int port_3 = out_sysport;
    int gport_num;
    int rv;
    int i, j;
    int flags;
    int unit;

    /* Initialize the vlan-ports as defined in cint_vswitch_metro_mp.c */
    vswitch_metro_mp_info_init(out_sysport, out_sysport, out_sysport);

    /* set vlan port ring protection parameters to be used with bcm_vlan_port_create
     * P0 & P1 associated to ring0 and p2 associated to ring1 */
    vswitch_metro_mp_ring_protection_info_init(0, flush_group_ids[0], rx_failovers[0], tx_failovers[0]);
    vswitch_metro_mp_ring_protection_info_init(1, flush_group_ids[0], rx_failovers[0], tx_failovers[0]);
    vswitch_metro_mp_ring_protection_info_init(2, flush_group_ids[1], rx_failovers[1], tx_failovers[1]);

    /* Create the vlan - ports as defined in cint_vswitch_metro_mp.c */
    if (nof_units > 1){
        rv = l2_learning_run_with_defaults(units_ids[0], units_ids[1]);
        if (rv != BCM_E_NONE){
            printf("Error, in l2_learning_run_with_defaults\n");
            return rv;
        }
    }
    rv = vswitch_metro_mp_multi_device_arad_run(units_ids, nof_units, 0xce, 3);
    if (rv != BCM_E_NONE){
        printf("Error, in vswitch_metro_mp_multi_device_arad_run\n");
        return rv;
    }

    /* Read the the allocated gports */
    for (gport_num = 0; gport_num < NOF_RING_VP_GPORTS; gport_num++) {
        units_array_make_local_first(units_ids, nof_units, out_sysport);
        rv = vswitch_metro_find_port(units_ids[0], gport_num, &outlifs[gport_num]);
        if (rv != BCM_E_NONE) {
            printf("Error in vswitch_metro_find_port for #%ld - %s\n", gport_num, bcm_errmsg(rv));
            return rv;
        }

        /* Print the allocated gports */
        printf("Allocated gport[%ld] = 0x%x on unit #%ld\n", gport_num, outlifs[gport_num], units_ids[0]);
    }

    /*****************************************************/
    /** Create a fourth vlan port, without protection. **/
    /***************************************************/

    units_array_make_local_first(units_ids, nof_units, in_sysport);
    /* We can overwrite those fields since they're not in use anymore. */
    vswitch_metro_mp_info.sysports[0] = in_sysport; 
    vswitch_metro_mp_info.p_vlans[0] = 100;
    vswitch_metro_mp_info.p_vlans[1] = 150;
    vswitch_metro_mp_info.p_vlans[2] = 200;
    vswitch_metro_mp_info.p_vlans[3] = 250;
    vswitch_metro_mp_info.flush_group_id[0] = 0;
    vswitch_metro_mp_info.ingress_failover_id[0] = 0;
    vswitch_metro_mp_info.egress_failover_id[0] = 0;

    /* Init the new vlans */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        for (j = 0 ; j < 4 ; j++) {
            rv = vlan__init_vlan(unit,vswitch_metro_mp_info.p_vlans[j]);
            if (rv != BCM_E_NONE) {
                printf("Error, vlan__init_vlan\n");
                print rv;
                return rv;
            }
        }
    }

    /* Create the gport */
    flags = 0;
    outlifs[3] = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = vswitch_metro_add_port(unit, 0, &outlifs[3], flags);
        if (rv != BCM_E_NONE) {
            printf("Error, vswitch_metro_add_port_1\n");
            print rv;
            return rv;
        }
        if(verbose){
            printf("created vlan-port   0x%08x in unit %d\n",outlifs[3], unit);
        }
        flags |= BCM_VLAN_PORT_WITH_ID;
    }

    /* add vlan-port to the vswitch and multicast */
    /* Local unit for sysport1 is already first */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = vswitch_add_port(unit, vswitch_metro_mp_info.vsi, in_sysport, outlifs[3]);
        if (rv != BCM_E_NONE) {
            printf("Error, vswitch_add_port\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Perform Fast Flush on a failover group by supplying a gport that is 
 * associated with the failover group. 
 */
int ring_protection_fast_flush(int unit, int flush_group_id)
{
    int rv;
    bcm_l2_addr_t l2_replace_addr;
    int l2_replace_flags;
    uint8 mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_l2_addr_t_init(&l2_replace_addr, mac_address, 0);

    /* Set the gport address and the action flags */
    l2_replace_flags = BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_NO_CALLBACKS |
                       BCM_L2_REPLACE_IGNORE_DES_HIT | BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_MATCH_GROUP;
    l2_replace_addr.group = flush_group_id;

    /* Perform the entry deletion */
    rv = bcm_l2_replace(unit, l2_replace_flags, &l2_replace_addr, 0, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_l2_replace with flush group (%ld) - %s\n", l2_replace_addr.group, bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * create the ERP rx & tx failover to be used for blocking/unblocking the ERP
 */
int ring_protection_erp_failover_create(int unit, uint32 flags, int erp_idx)
{
    int rv;

    rv = bcm_failover_create(unit, flags | BCM_FAILOVER_INGRESS, &rx_failovers[erp_idx]);
    if (rv != BCM_E_NONE)
    {
        printf("ring_protection_erp_failover_create failure in rx bcm_failover_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("rx ERP group created with failover_id 0x%x\n", rx_failovers[erp_idx]);

    /* Set the default state */
    rv = bcm_failover_set(unit, rx_failovers[erp_idx], 0);
    if (rv != BCM_E_NONE)
    {
        printf("ring_protection_erp_failover_create failure in rx bcm_failover_set with erp_failover_id - 0x%x to init state - 0 %s\n",
                rx_failovers[erp_idx], bcm_errmsg(rv));
        return rv;
    }
    printf("rx ERP group failover_id 0x%x initiated to state 0\n", rx_failovers[erp_idx]);

    rv = bcm_failover_create(unit, flags | BCM_FAILOVER_ENCAP, &tx_failovers[erp_idx]);
    if (rv != BCM_E_NONE)
    {
        printf("ring_protection_erp_failover_create failure in tx bcm_failover_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("tx ERP group created with failover_id 0x%x\n", tx_failovers[erp_idx]);

    /* Set the default state */
    rv = bcm_failover_set(unit, tx_failovers[erp_idx], 0);
    if (rv != BCM_E_NONE)
    {
        printf("ring_protection_erp_failover_create failure in tx bcm_failover_set with erp_failover_id - 0x%x to init state - 0 %s\n",
                tx_failovers[erp_idx], bcm_errmsg(rv));
        return rv;
    }
    printf("tx ERP group failover_id 0x%x initiated to state 0\n", tx_failovers[erp_idx]);

    return BCM_E_NONE;
}

/*
 * Destroy an ERP group rx & tx failover
 */
int ring_protection_erp_failover_destroy(int unit, int erp_idx)
{
    int rv;

    rv = bcm_failover_destroy(unit, rx_failovers[erp_idx]);
    if (rv != BCM_E_NONE) {
        printf("ring_protection_erp_failover_destroy failure in rx bcm_failover_destroy with gport - 0x%x, %s\n", gport, bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_failover_destroy(unit, tx_failovers[erp_idx]);
    if (rv != BCM_E_NONE) {
        printf("ring_protection_erp_failover_destroy failure in tx bcm_failover_destroy with gport - 0x%x, %s\n", gport, bcm_errmsg(rv));
        return rv;
    }

    printf("ERP group rx 0x%x and tx 0x%x failover_ids are destroyed\n", rx_failovers[erp_idx], tx_failovers[erp_idx]);
    return rv;
}

/*
 * Block/Unblock an ERP port.
 * Block/Unblock both the Ingress and Egress using protection mechanism
 */
int ring_protection_erp_block(int unit,
              int failover_id_idx,
              int block_state)
{
    int rv;

    /* Block/Unblock the Ingress */
    rv = bcm_failover_set(unit, rx_failovers[failover_id_idx], block_state);
    if (rv != BCM_E_NONE) {
        printf("ring_protection_erp_block failure in bcm_failover_set with failover_id - 0x%x to state - %ld: %s\n",
                rx_failovers[failover_id_idx], block_state, bcm_errmsg(rv));
        return rv;
    }
    printf("ERP Ingress block state for failover_id 0x%x set to %ld\n", rx_failovers[failover_id_idx], block_state);

    /* Block/Unblock the Ingress */
    rv = bcm_failover_set(unit, tx_failovers[failover_id_idx], block_state);
    if (rv != BCM_E_NONE) {
        printf("ring_protection_erp_block failure in bcm_failover_set with erp_failover_id - 0x%x to state - %ld: %s\n",
                tx_failovers[failover_id_idx], block_state, bcm_errmsg(rv));
        return rv;
    }
    printf("ERP Egress block state for failover_id 0x%x set to %ld\n", tx_failovers[failover_id_idx], block_state);

    return BCM_E_NONE;
}

/*
 * Associate/De-assosiate outlif with ring flush group and blocking information.
 * The operation updates the existing VLAN-Port info with a flush group and failover parameters
 */
int ring_protection_outlif_to_erp_associate(int unit,
                                            int outlif_idx,
                                            uint32 erp_idx,
                                            int enable)
{
    int rv;
    bcm_vlan_port_t vp;
    bcm_gport_t gport = outlifs[outlif_idx];
    uint32 flush_group_id = flush_group_ids[erp_idx];
    bcm_failover_t rx_failover_id = rx_failovers[erp_idx];
    bcm_failover_t tx_failover_id = tx_failovers[erp_idx];

    /* Get the vlan-port info */
    bcm_vlan_port_t_init(&vp);
    vp.vlan_port_id = gport;
    rv = bcm_vlan_port_find(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("ring_protection_outlif_to_erp_associate failure in bcm_vlan_port_find with gport - 0x%x, %s\n", gport, bcm_errmsg(rv));
        return rv;
    }

    /* Attach the vlan-port to the ingress failover group */
    vp.group = enable ? flush_group_id : 0;
    vp.ingress_failover_id = enable ? rx_failover_id : 0;
    vp.failover_port_id = 0;
    vp.egress_failover_id = enable ? tx_failover_id : 0;
    vp.egress_failover_port_id = 0;
    vp.flags |= BCM_VLAN_PORT_REPLACE | BCM_VLAN_PORT_WITH_ID;

    /* Update the vlan-port info */
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("ring_protection_outlif_to_erp_associate failure in bcm_vlan_port_create with gport - 0x%x, %s\n", gport, bcm_errmsg(rv));
        return rv;
    }

    printf("LIF 0x%x associated to erp %ld\n", gport, flush_group_id);

    return BCM_E_NONE;
}

/*
 * destroy the ports and remove the failover groups.
 */
int ring_protection_destroy(int *units_ids, int nof_units)
{
    int gport_num, erp_idx;
    int i, unit;
    int rv;

    /* Remove the erp failover groups */
    for (erp_idx = 0; erp_idx < NOF_ERP_GROUPS; erp_idx++) {
        for (i = 0 ; i < nof_units ; i++) {
            unit = units_ids[i];

            rv = ring_protection_erp_failover_destroy(unit, erp_idx);
            if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
                printf("ring_protectection_remove_erps failure in ring_protection_erp_failover_destroy for erp_idx - %ld: %s\n",
                       erp_idx, bcm_errmsg(rv));
                return rv;
            }
        }
    }

    /* Remove the gport from their failover groups */
    for (gport_num = 0; gport_num < NOF_RING_VP_GPORTS; gport_num++) {
        for (i = 0 ; i < nof_units ; i++) {
            unit = units_ids[i];
            rv = bcm_vlan_port_destroy(unit, outlifs[gport_num]);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_vlan_port_destroy with gport (0x%x) - %s\n", outlifs[gport_num], bcm_errmsg(rv));
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Create NOF_ERP_GROUPS Ring protection groups and NOF_VP_GPORTS vlan-ports.
 * Associate the vlan-ports to the Ring protection groups according to the model explained at the file header.
 */
int ring_protection_create(int *units_ids, int nof_units, int outSysport, int inSysport)
{
    uint32 failover_flags;
    uint32 erp_idx;
    int unit, i;
    int rv;

    /* Create default Ring Protection groups */
    for (erp_idx = 0; erp_idx < NOF_ERP_GROUPS; erp_idx++)
    {
        failover_flags = 0;
        units_array_make_local_last(units_ids, nof_units, outSysport);   /* In multi device, we expect the mac table to be on one device, and
                                                                        the out ACs to be on a different device. We want to create the
                                                                        failover FECs on the unit without the out ACs.*/
        /* Create an ERP ingress and egress groups */
        units_array_make_local_first(units_ids, nof_units, outSysport); /* This time, the erp needs to be on the local unit. */
        for (i = 0 ; i < nof_units ; i++) {
            unit = units_ids[i];
            rv = ring_protection_erp_failover_create(unit, failover_flags, erp_idx);
            if (rv != BCM_E_NONE) {
                printf("ring_protection_create failure at ring_protection_erp_failover_create: %s\n", bcm_errmsg(rv));
                return rv;
            }
            failover_flags |= BCM_FAILOVER_WITH_ID;
        }
    }

    /* Call a function to create the vlan-ports */
    rv = ring_protection_vlan_port_create(units_ids, nof_units, outSysport, inSysport);
    if (rv != BCM_E_NONE) {
        printf("Error, ring_protection_vlan_port_create - %s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Tests removal of all mac table entries from ring_group #erp_idx
 * Create entries by sending packets from ports belong to the ring using <ovid,ivid>
 * Use "L2EntryCountGet" to count entries before and after flush
 * All entries added to port which belong to the ring should be removed successfully
 */
int ring_protection_fast_flush_all(int out_unit, int in_unit, int erp_idx)
{
    int rv;

    rv = ring_protection_fast_flush(in_unit, erp_idx);
    if (rv != BCM_E_NONE) {
        printf("ring_protection_fast_flush_all failure in ring_protection_fast_flush for flush_group_id - %ld: %s\n",
                erp_idx, bcm_errmsg(rv));
        return rv;
    }

    rv = ring_protection_fast_flush(out_unit, erp_idx);
    if (rv != BCM_E_NONE) {
        printf("ring_protection_fast_flush_all failure in ring_protection_fast_flush for flush_group_id - %ld: %s\n",
                erp_idx, bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}
