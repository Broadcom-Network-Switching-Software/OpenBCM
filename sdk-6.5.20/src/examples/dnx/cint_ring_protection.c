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
 * cint examples/sand/utility/cint_sand_utils_global.c;
 * cint examples/sand/utility/cint_sand_utils_multicast.c;
 * cint examples/sand/utility/cint_sand_utils_vlan.c;
 * cint examples/sand/cint_sand_l2_learning.c;
 * cint examples/sand/cint_sand_multi_device_utils.c
 * cint examples/sand/cint_advanced_vlan_translation_mode.c
 * cint examples/sand/cint_vswitch_metro_mp.c;
 * cint examples/dnx/cint_ring_protection.c;
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
    units_array_make_local_first(units_ids, nof_units, vswitch_metro_mp_info.sysports[0]);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        if (is_system_port_is_local(unit, vswitch_metro_mp_info.sysports[0])) {
            rv = vswitch_metro_add_port(unit, 0, &outlifs[3], flags);
            if (rv != BCM_E_NONE) {
                printf("Error, vswitch_metro_add_port_1\n");
                print rv;
                return rv;
            }
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
            if (is_system_port_is_local(unit, vswitch_metro_mp_info.sysports[gport_num])) {
                rv = bcm_vlan_port_destroy(unit, outlifs[gport_num]);
                if (rv != BCM_E_NONE) {
                    printf("Error in bcm_vlan_port_destroy with gport (0x%x) - %s\n", outlifs[gport_num], bcm_errmsg(rv));
                    return rv;
                }
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
            printf("ring_protection_erp_failover_create for unit %d\n", unit);
            rv = ring_protection_erp_failover_create(unit, failover_flags, erp_idx);
            if (rv != BCM_E_NONE) {
                printf("ring_protection_create failure at ring_protection_erp_failover_create: %s\n", bcm_errmsg(rv));
                return rv;
            }
            failover_flags |= BCM_FAILOVER_WITH_ID;
        }
    }

    /* Call a function to create the vlan-ports */
    printf("ring_protection_vlan_port_create \n");
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

/*
 * Perform Fast Flush on a failover group by supplying a gport that is
 * associated with the failover group.
 */
int ring_protect_fast_flush_by_dest_only(int unit, bcm_gport_t gport)
{
    int rv;
    bcm_l2_addr_t l2_replace_addr;
    int l2_replace_flags;
    bcm_l2_addr_t match_mask_addr;

    /* Set the gport address and the action flags */
    l2_replace_flags =  BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_NO_CALLBACKS | 
         BCM_L2_REPLACE_IGNORE_DES_HIT | BCM_L2_REPLACE_MATCH_STATIC | 
         BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_MATCH_GROUP;
    l2_replace_addr.port = gport;
    match_mask_addr.group = 0;

    /* Perform the entry deletion */
    rv = bcm_l2_replace_match(unit, l2_replace_flags, &l2_replace_addr, &match_mask_addr, NULL, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_l2_replace with gport(%ld) - %s\n", l2_replace_addr.port, bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * To support this function at jr1 system mode, the fast flush filter will be dest(fec value). 
 *  1. Create a fec
 *  2. Binding fec to AC's learning information
 *  3. Send packet as below
 *     learn FEC 1:
 *       tx 1 PSRC=13 DATA=0x0000000000050000000044819100000a000000000fff
 *       tx 1 PSRC=13 DATA=0x0000000000050000000044829100000b000000000fff
 *     learn FEC 2:
 *       tx 1 PSRC=13 DATA=0x0000000000050000000044829100000b000000000fff
 *     learn port
 *       tx 1 PSRC=13 DATA=0x00000000000500000000448391000001000000000fff
 *  4. Flush L2 DB by dest_only(FEC ID) - call ring_protect_fast_flush_by_dest_only() 
 *     to delete L2 entries which learned dest is FEC.
 *     FEC 1:
 *       print ring_protect_fast_flush_by_dest_only(unit, g_fast_flush_vlan_port_id[0]);
 *     FEC 2
 *       print ring_protect_fast_flush_by_dest_only(unit, g_fast_flush_vlan_port_id[2]);
 */
int g_fast_flush_vlan_port_id[3];
int g_fast_flush_fec_gport[2];
int g_fast_flush_vsi;
int ring_protect_with_jr1_mode_create(int unit, int inP, int outP)
{
    int fec_id = 0;
    bcm_l3_egress_t l3_egr;
    int rv = 0;
    bcm_vlan_t vsi_id = 0;
    int match_vid[3] = {10, 11, 20};
    int vid_idx = 0;

    bcm_port_class_set(unit, inP, bcmPortClassId, inP);
    bcm_port_class_set(unit, outP, bcmPortClassId, outP);

    rv = bcm_vswitch_create(unit, &vsi_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create_with_id - %s\n", bcm_errmsg(rv));
        return rv;
    }
    g_fast_flush_vsi = vsi_id;

    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.flags = 0;
    l3_egr.port = inP;
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_egr, &fec_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create 1st FEC - %s\n", bcm_errmsg(rv));
        return rv;
    }
    g_fast_flush_fec_gport[0] = fec_id;

    /*
     * Create 2 vlan port learned destination first FEC ID.
     */

    int fec_gport_id = 0;
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(fec_gport_id, fec_id);
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = inP;
    vlan_port.match_vlan = match_vid[vid_idx];
    vlan_port.group = 1; /* dest+out_lif */
    vlan_port.failover_port_id = fec_gport_id;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create vid:%d - %s\n", match_vid[vid_idx], bcm_errmsg(rv));
        return rv;
    }
    g_fast_flush_vlan_port_id[vid_idx] = vlan_port.vlan_port_id;

    rv = bcm_vswitch_port_add(unit, vsi_id, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add vlan_port_1 - %s\n", bcm_errmsg(rv));
        return rv;
    }

    vid_idx++;
    bcm_vlan_port_t vlan_port_2;
    bcm_vlan_port_t_init(&vlan_port_2);
    vlan_port_2.port = inP;
    vlan_port_2.match_vlan = match_vid[vid_idx];
    vlan_port_2.group = 1; /* dest+out_lif */
    vlan_port_2.failover_port_id = fec_gport_id;
    vlan_port_2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    rv = bcm_vlan_port_create(unit, vlan_port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create vid:%d - %s\n", match_vid[vid_idx], bcm_errmsg(rv));
        return rv;
    }
    g_fast_flush_vlan_port_id[vid_idx] = vlan_port_2.vlan_port_id;

    rv = bcm_vswitch_port_add(unit, vsi_id, vlan_port_2.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add vlan_port_2 - %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create vlan port which leaned destination is FEC_2 */
    int fec_id_2 = 0;
    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.flags = 0;
    l3_egr.port = inP;
    rv =  bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_egr, &fec_id_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create 2nd FEC - %s\n", bcm_errmsg(rv));
        return rv;
    }
    g_fast_flush_fec_gport[1] = fec_id_2;

    int fec_gport_id_2 = 0;
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(fec_gport_id_2, fec_id_2);

    vid_idx++;
    bcm_vlan_port_t vlan_port_3;
    bcm_vlan_port_t_init(&vlan_port_3);
    vlan_port_3.port = inP;
    vlan_port_3.match_vlan = match_vid[vid_idx];
    vlan_port_3.group = 1; /* dest+out_lif */
    vlan_port_3.failover_port_id = fec_gport_id_2;
    vlan_port_3.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    rv = bcm_vlan_port_create(unit, vlan_port_3);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create vlan_port_3 - %s\n", bcm_errmsg(rv));
        return rv;
    }
    g_fast_flush_vlan_port_id[vid_idx] = vlan_port_3.vlan_port_id;

    rv = bcm_vswitch_port_add(unit, vsi_id, vlan_port_3.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add vlan_port_3 - %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;

}

int ring_protect_with_jr1_mode_destroy(int unit)
{
    int rv = 0;
    int idx = 0;
    int vsi_id = g_fast_flush_vsi;

    rv = bcm_l3_egress_destroy(unit, g_fast_flush_fec_gport[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_destroy 1st FEC - %s\n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_l3_egress_destroy(unit, g_fast_flush_fec_gport[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_destroy 2nd FEC - %s\n", bcm_errmsg(rv));
        return rv;
    }

    for (idx = 0; idx < 3; idx++)
    {
        rv = bcm_vswitch_port_delete(unit, vsi_id, g_fast_flush_vlan_port_id[idx]);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add vlan_port:%d - %s\n", idx, bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_vlan_port_destroy(unit, g_fast_flush_vlan_port_id[idx]);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_destroy vlan_port:%d - %s\n", idx, bcm_errmsg(rv));
            return rv;
        }
    }

    rv = bcm_vswitch_destroy(unit, vsi_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_destroy - %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;

}


