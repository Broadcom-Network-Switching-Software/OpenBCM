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
 * cint examples/dpp/utility/cint_utils_vlan.c;
 * cint examples/dpp/cint_port_tpid.c;
 * cint examples/dpp/cint_l2_mact.c;
 * cint examples/dpp/cint_multi_device_utils.c
 * cint examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint examples/dpp/cint_vswitch_metro_mp.c;
 * cint examples/dpp/cint_l2_fast_flush.c;
 * cint examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint;
 * int rv;
 * rv = create_ring_protection_groups(<unit>, <port>, <erp_mode>); 
 * 
 * This cint uses cint_vswitch_metro_mp.c to build the following vswitch
 * with all three ports are configured on the same port - <port>.
 * The functions support an ERP mode in which relevant ERP configurations
 * and ERP Blocking/Unblocking operations. The ERP mode is set when the erp_mode parameter should be
 * erp_mode parameter is set to '1'.
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
 *  |                        /  \  VSwitch  /                               | 
 *  |                   /\  /p2  -----------                                |
 *  |              <5,2>/  /  /                                             |
 *  |                  /  /  /<3,6>                                         |
 *  |                 /  /  /                                               |
 *  |                   /  \/                                               |
 *  |                  o                                                    |
 *  |                                                                       | 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Step 1:
 *      Create Ring Protection groups - create_ring_protection_groups()
 *      The default setting will be create the following two
 *      Ring Protection groups:
 *          1. ring_group_1 - Includes p1 & p2.
 *          2. ring_group_2 - Includes p3 only.
 *      Example:
 *          create_ring_protection_groups(<unit number>, <local port number>, <erp_mode>);
 * 
 *      Calling sequence:
 *          1. Create vswitch with vlan-ports as described in cint_vswitch_metro_mp.c.
 *          2. Call bcm_failover_create() in order to create each Ring Protection group.
 *          3. Call bcm_port_class_set() to associate a vlan-port to a Ring group.
 *          4. In ERP mode: Create an ERP group - erp_group_create (bcm_failover_create)
 *          5. In ERP mode: Associate the vlan-ports to the ERP groups (bcm_vlan_port_create update).
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
 *              1. P1 & P2 entries - GPORT (FEC) with the same MSBs as ring_group_1 
 *              2. P3 entries - GPORT (FEC) with the same MSBs as ring_group_2
 * 
 *      Note: A user may configure any of the configured ports to any ring group
 *            using the function set_port_to_ring(unit);
 *
 *
 *  Step 2:
 *      Move a port for from one group to another - set_port_to_ring();
 *      The default setting will be to move p2 from ring_group_1 to ring_group_2:
 *          set_port_to_ring(unit, g_gport[1], g_failover_id[1], erp_mode);
 *      The example function could be used:
 *          set_port_to_ring_example_1(<unit number erp_mode>);
 *
 *      Calling sequence:
 *          1. Call bcm_port_class_set() to associate a vlan-port to a Ring group.
 *          2. In ERP mode: Associate the vlan-ports to an ERP group (bcm_vlan_port_create).
 * 
 *      Result: Moving a port from one group will effect the MAC table if we
 *              resend the packet for the moved gport - g_gport[1] in the
 *              default case. When applying "l2 show", the forwarding will show:
 *              1. P1 entries - GPORT (FEC) with the same MSBs as ring_group_1 
 *              2. P2 & P3 entries - GPORT (FEC) with the same MSBs as ring_group_2
 * 
 * 
 *  Step 3:
 *      Perform Fast Flush for a Ring group - perform_fast_flush();
 *      The default setting will be to perform the Fast Flush on Ring group
 *      ring_group_2 by supplying a gport that is associated with the group:
 *          perform_fast_flush(unit, g_gport[2]);
 *      Calling sequence:
 *          1. In ERP mode: Block the LIFs using erp_block
 *              (bcm_failover_set & bcm_port_discard_set).
 *          2. Call bcm_l2_replace() with the selected gport as a parameter
 *          3. In ERP mode: Unblock the LIFs using erp_block
 *
 *      Result: The Fast Flush should remove the the entries from all the
 *              gports that are associated with failover group that the
 *              supplied gport belongs to.
 *              In the default sequence, when applying "l2 show", the
 *              forwarding will show:
 *              1. Only P1 entries - GPORT (FEC) with the same MSBs as ring_group_1 
 * 
 * 
 *  Step 4:
 *      Remove all ports from the Ring groups and removing the Ring groups -
 *          remove_rings();
 *      The default setting will be to remove p1 from ring_group_1 and
 *      remove p2 & p3 from ring_group_2, before removing both Ring groups.
 *
 *      Calling sequence:
 *          1. Call bcm_port_class_set() with the class id 0xFFFFFFFF in order
 *             to de associate a gport from his failover group.
 *          2. Call bcm_failover_destroy() to destroy a failover group.
 *          3. In ERP mode: bcm_failover_destroy to destroy the ERP group
 */ 


  /* Globals - gports & failover_ids */
  int MAX_RING_GPORTS = 3;
  int MAX_NO_RING_GPORTS = 1;
  int MAX_GPORTS = MAX_RING_GPORTS + MAX_NO_RING_GPORTS;
  int MAX_FAILOVER_GROUPS = 2;

  struct fast_flush_gport_s {
      bcm_gport_t gport_id;
      bcm_failover_t erp_failover_id;
  };

  fast_flush_gport_s g_gport[MAX_GPORTS];
  bcm_failover_t g_failover_id[MAX_FAILOVER_GROUPS], g_erp_failover_id[MAX_FAILOVER_GROUPS];

/*
 * Create the vlan-ports as defined in cint_vswitch_metro_mp.c 
 * Create them all on port 13 
 */
int create_ring_protection_ports(int *units_ids, int nof_units, int out_sysport, int in_sysport)
{
    int port_1 = out_sysport;
    int port_2 = out_sysport;
    int port_3 = out_sysport;
    int gport_num;
    int rv;
    int i, j;
    int unit;

    /* Create the vlan - ports as defined in cint_vswitch_metro_mp.c */
    rv = vswitch_metro_mp_run_with_defaults_multi_device(units_ids, nof_units, 1, port_1, port_2, port_3);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
    	return rv;
    }

    /* Read the the allocated gports */
    for (gport_num = 0; gport_num < MAX_RING_GPORTS; gport_num++) {
        units_array_make_local_first(units_ids, nof_units, out_sysport);
        rv = vswitch_metro_find_port(units_ids[0], gport_num, &g_gport[gport_num].gport_id);
        if (rv != BCM_E_NONE) {
            printf("Error in vswitch_metro_find_port for #%ld - %s\n", gport_num, bcm_errmsg(rv));
            return rv;
        }
        g_gport[gport_num].erp_failover_id = -1;

        /* Print the allocated gports */
        printf("Allocated gport[%ld] = %ld on unit #%ld\n", gport_num, g_gport[gport_num].gport_id, units_ids[0]);
    }

    /*****************************************************/
    /** Create a fourth vlan port, without protection. **/
    /***************************************************/
    int flags;
    int gport4;

    /* Init tpid in port */
    port_tpid_init(in_sysport,1,1);
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set, in unit %d\n", unit);
            print rv;
            return rv;
        }
    }

    units_array_make_local_first(units_ids, nof_units, in_sysport);
    /* We can overwrite those fields since they're not in use anymore. */
    vswitch_metro_mp_info.sysports[0] = in_sysport; 
    vswitch_metro_mp_info.p_vlans[0] = 100;
    vswitch_metro_mp_info.p_vlans[1] = 150;
    vswitch_metro_mp_info.p_vlans[2] = 200;
    vswitch_metro_mp_info.p_vlans[3] = 250;

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
    gport4 = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = vswitch_metro_add_port(unit, 0, &gport4, flags);
        if (rv != BCM_E_NONE) {
            printf("Error, vswitch_metro_add_port_1\n");
            print rv;
            return rv;
        }
        if(verbose){
            printf("created vlan-port   0x%08x in unit %d\n",gport4, unit);
        }
        flags |= BCM_VLAN_PORT_WITH_ID;
    }

    /* add vlan-port to the vswitch and multicast */
    /* Local unit for sysport1 is already first */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = vswitch_add_port(unit, vswitch_metro_mp_info.vsi, in_sysport, gport4);
        if (rv != BCM_E_NONE) {
            printf("Error, vswitch_add_port\n");
            return rv;
        }
    }

    g_gport[3].gport_id = gport4;    
    return BCM_E_NONE;
}


/*
 * Move a vlan-port to a failover group 
 */
int set_port_to_ring(int unit,
                     bcm_gport_t gport,
                     bcm_failover_t failover_id)
{
    uint32 class_id;
    int rv;
    bcm_vlan_port_t vp_get_1;
    bcm_vlan_port_t vp_get_2;

    bcm_vlan_port_t_init(&vp_get_1);
    vp_get_1.vlan_port_id = gport;
    rv = bcm_vlan_port_find(unit, &vp_get_1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_find of gport(%ld) before associating - %s\n", gport, bcm_errmsg(rv));
        return rv;
    }

    /* Associate a gport to the first group */
    rv = bcm_port_class_set(unit, gport, bcmPortClassL2Lookup, failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_class_set of gport(%ld) to failover_id(%ld) - %s\n",
               gport, failover_id, bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_class_get(unit, gport, bcmPortClassL2Lookup, &class_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_class_get for %ld - %s\n", gport, bcm_errmsg(rv));
        return rv;
    }

    printf("gport %ld is now associated with failover id - %ld\n", gport, class_id);
    if (BCM_GPORT_IS_LOCAL(vp_get_1.port))
    {
        bcm_vlan_port_t_init(&vp_get_2);
        vp_get_2.criteria = vp_get_1.criteria;
        vp_get_2.match_vlan = vp_get_1.match_vlan;
        vp_get_2.match_inner_vlan = vp_get_1.match_inner_vlan;
        vp_get_2.match_pcp = vp_get_1.match_pcp;
        vp_get_2.match_ethertype  = vp_get_1.match_ethertype;
        vp_get_2.port = vp_get_1.port;
        rv = bcm_vlan_port_find(unit, &vp_get_2);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vlan_port_find of gport(%ld) after associating - %s\n", gport, bcm_errmsg(rv));
            return rv;
        }

        if ((vp_get_1.vlan_port_id != vp_get_2.vlan_port_id) ||
            (vp_get_1.encap_id != vp_get_2.encap_id)){
            rv = BCM_E_INTERNAL;
            printf("Error vlan port paras are not same after associating failover and vlan port  - %s\n",
                    bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}


/*
 * Create the default Ring protection groups and vlan-ports. 
 * Associate the vlan-ports to the Ring protection groups 
 */
int create_ring_protection_groups(int *units_ids, int nof_units, int outSysport, int inSysport, int erp_mode)
{
    int failover_flags = BCM_FAILOVER_L2_LOOKUP;
    uint32 class_id;
    int failover_num;
    int unit, i;
    int rv;

    /* Create two default Ring Protection groups */
    for (failover_num = 0; failover_num < MAX_FAILOVER_GROUPS; failover_num++) {
        failover_flags = BCM_FAILOVER_L2_LOOKUP;
        units_array_make_local_last(units_ids, nof_units, outSysport);   /* In multi device, we expect the mac table to be on one device, and 
                                                                        the out ACs to be on a different device. We want to create the 
                                                                        failover FECs on the unit without the out ACs.*/
        for (i = 0 ; i < nof_units ; i++) {
            unit = units_ids[i];
            rv = bcm_failover_create(unit, failover_flags, &g_failover_id[failover_num]);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_failover_create #%ld - %s\n", failover_num, bcm_errmsg(rv));
                return rv;
            }
            failover_flags |= BCM_FAILOVER_WITH_ID;
        }

        /* If in ERP mode. */
        if (erp_mode) {
            /* Create an ERP group */
            failover_flags = 0;
            units_array_make_local_first(units_ids, nof_units, outSysport); /* This time, the erp needs to be on the local unit. */
            for (i = 0 ; i < nof_units ; i++) {
                unit = units_ids[i];
                rv = erp_group_create(unit, failover_flags, &g_erp_failover_id[failover_num]);
                if (rv != BCM_E_NONE) {
                    printf("create_ring_protection_ports failure in erp_group_create: %s\n", bcm_errmsg(rv));
                    return rv;
                }
                failover_flags |= BCM_FAILOVER_WITH_ID;
            }
        }

        /* Print the allocated failover_ids */
        printf ("Allocated g_failover_id[%ld] = %ld\n", failover_num, g_failover_id[failover_num]);
    }

    /* Call a function to create the vlan-ports */
    rv = create_ring_protection_ports(units_ids, nof_units, outSysport, inSysport);
    if (rv != BCM_E_NONE) {
        printf("Error, create_ring_protection_ports - %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create default association of the vlan-ports to the fast flush failover groups */
    for (i = 0 ; i < nof_units ; i++) {
        unit = units_ids[i];
        rv = set_port_to_ring(unit, g_gport[0].gport_id, g_failover_id[0]);
        if (rv != BCM_E_NONE) {
            printf("Error in set_port_to_ring with g_gport[0].gport_id, g_failover_id[0] - %s\n", bcm_errmsg(rv));
            return rv;
        }

        rv = set_port_to_ring(unit, g_gport[1].gport_id, g_failover_id[0]);
        if (rv != BCM_E_NONE) {
            printf("Error in set_port_to_ring with g_gport[1].gport_id, g_failover_id[0] - %s\n", bcm_errmsg(rv));
            return rv;
        }

        rv = set_port_to_ring(unit, g_gport[2].gport_id, g_failover_id[1]);
        if (rv != BCM_E_NONE) {
            printf("Error in set_port_to_ring with g_gport[2].gport_id, g_failover_id[1] - %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* If in ERP mode. */
    if (erp_mode) {
        /* Associate the LIFs to ERP failover groups */
        /*for (i = 0 ; i < nof_units ; i++) {*/
        units_array_make_local_first(units_ids, nof_units, outSysport); /* This time, the erp needs to be on the local unit. */

            unit = units_ids[0];

            rv = set_port_to_erp(unit, 0, 0);
            if (rv != BCM_E_NONE) {
                printf("create_ring_protection_ports failure in set_port_to_erp for LIF - %ld, erp_failover_id - %ld: %s\n",
                       g_gport[0].gport_id, g_erp_failover_id[0], bcm_errmsg(rv));
                return rv;
            }
            rv = set_port_to_erp(unit, 1, 0);
            if (rv != BCM_E_NONE) {
                printf("create_ring_protection_ports failure in set_port_to_erp for LIF - %ld, erp_failover_id - %ld: %s\n",
                       g_gport[1].gport_id, g_erp_failover_id[0], bcm_errmsg(rv));
                return rv;
            }
            rv = set_port_to_erp(unit, 2, 1);
            if (rv != BCM_E_NONE) {
                printf("create_ring_protection_ports failure in set_port_to_erp for LIF - %ld, erp_failover_id - %ld: %s\n",
                       g_gport[2].gport_id, g_erp_failover_id[1], bcm_errmsg(rv));
                return rv;
            }
        /*}*/
    }

    return BCM_E_NONE;
}


/*
 * Perform Fast Flush on a failover group by supplying a gport that is 
 * associated with the failover group. 
 */
int perform_fast_flush(int unit, bcm_gport_t gport)
{
    int rv;
    bcm_l2_addr_t l2_replace_addr;
    int l2_replace_flags;

    /* Set the gport address and the action flags */
    l2_replace_flags = BCM_L2_REPLACE_PROTECTION_RING | BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_NO_CALLBACKS | 
        BCM_L2_REPLACE_IGNORE_DISCARD_SRC | BCM_L2_REPLACE_IGNORE_DES_HIT | BCM_L2_REPLACE_MATCH_STATIC;
    l2_replace_addr.port = gport;

    /* Perform the entry deletion */
    rv = bcm_l2_replace(unit, l2_replace_flags, &l2_replace_addr, 0, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_l2_replace with gport(%ld) - %s\n", l2_replace_addr.port, bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Remove the ports association to the failover groups, destroy them, and remove the 
 * failover groups.  
 */
int remove_rings(int *units_ids, int nof_units, int erp_mode)
{
    int gport_num, failover_num;
    int i, unit;
    int rv;

    /* Remove the gport from their failover groups */
    for (gport_num = 0; gport_num < MAX_RING_GPORTS; gport_num++) {
        for (i = 0 ; i < nof_units ; i++) {
            unit = units_ids[i];
            rv = bcm_port_class_set(unit, g_gport[gport_num].gport_id, bcmPortClassL2Lookup, 0xFFFFFFFF);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_port_class_set deletion with gport(%ld) - %s\n", g_gport[gport_num].gport_id, bcm_errmsg(rv));
                return rv;
            }
            printf("gport %ld de associated\n", g_gport[gport_num].gport_id);

            rv = bcm_vlan_port_destroy(unit, g_gport[gport_num].gport_id);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_vlan_portdestroy with gport(%ld) - %s\n", g_gport[gport_num].gport_id, bcm_errmsg(rv));
                return rv;
            }
        }
    }

    /* Remove the failover groups */
    for (failover_num = 0; failover_num < MAX_FAILOVER_GROUPS; failover_num++) {
        for (i = 0 ; i < nof_units ; i++) {
            unit = units_ids[i];
            printf("======================unit:%d, g_failover_id[%d]:%d\n", unit, failover_num, g_failover_id[failover_num]);
            rv = bcm_failover_destroy(unit, g_failover_id[failover_num]);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_failover_destroy with failover_id(%ld) - %s\n", g_failover_id[failover_num], bcm_errmsg(rv));
                return rv;
            }
            printf("failover_id group - %ld destroyed\n", g_failover_id[failover_num]);
        
            /* If in ERP mode */
            if (erp_mode) {
                rv = erp_group_destroy(unit, g_erp_failover_id[failover_num]);
                if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) { 
                    printf("remove_rings failure in erp_group_destroy for erp_failover_id - %ld: %s\n",
                           g_erp_failover_id[failover_num], bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Tests removal of all mac table entries from ring_group_2 => p3 (see diagram) 
 * Create entries by sending packets from p3 using <ovid,ivid> = <4,8> 
 * Use "L2EntryCountGet" to count entries before and after flush 
 * All entries added to p3 should be removed successfully 
 */
int perform_fast_flush_example_1(int out_unit, int in_unit, int erp_mode)
{
    int rv, erp_failover_id_idx, block_state;

    /* If in ERP mode - First block. */
    if (erp_mode) {
        erp_failover_id_idx = 1;
        block_state = 1;
        rv = erp_block(out_unit, erp_failover_id_idx, block_state);
        if (rv != BCM_E_NONE) {
            printf("perform_fast_flush_example_1 failure in erp_block for erp_failover_id_idx - %ld to state - %ld: %s\n",
                   erp_failover_id_idx, block_state, bcm_errmsg(rv));
            return rv;
        }
    }

    rv = perform_fast_flush(in_unit, g_gport[2].gport_id);
    if (rv != BCM_E_NONE) {
        printf("perform_fast_flush_example_1 failure in perform_fast_flush for LIF - %ld: %s\n",
               g_gport[2].gport_id, bcm_errmsg(rv));
        return rv;
    }

    rv = perform_fast_flush(out_unit, g_gport[2].gport_id);
    if (rv != BCM_E_NONE) {
        printf("perform_fast_flush_example_1 failure in perform_fast_flush for LIF - %ld: %s\n",
               g_gport[2].gport_id, bcm_errmsg(rv));
        return rv;
    }

    /* If in ERP mode - Release after the flush (just for the test to succeed) */
    if (erp_mode) {
        erp_failover_id_idx = 1;
        block_state = 0;
        rv = erp_block(out_unit, erp_failover_id_idx, block_state);
        if (rv != BCM_E_NONE) {
            printf("perform_fast_flush_example_1 failure in erp_block for erp_failover_id_idx - %ld to state - %ld: %s\n",
                   erp_failover_id_idx, block_state, bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Tests removal of all mac table entries from ring_group_1 => p1, p2 (see diagram) 
 * Create entries by sending packets from p3 using <ovid,ivid> = <10,20> or <5, 2> 
 * Use "L2EntryCountGet" to count entries before and after flush 
 * All entries added to p1, p2 should be removed successfully  
 */
int perform_fast_flush_example_2(int out_unit, int in_unit, int erp_mode)
{
    int rv, erp_failover_id_idx, block_state;

    /* If in ERP mode - First block */
    if (erp_mode) {
        erp_failover_id_idx = 0;
        block_state = 1;
        rv = erp_block(out_unit, erp_failover_id_idx, block_state);
        if (rv != BCM_E_NONE) {
            printf("perform_fast_flush_example_2 failure in erp_block for erp_failover_id - %ld to state - %ld: %s\n",
                   erp_failover_id_idx, block_state, bcm_errmsg(rv));
            return rv;
        }
    }

    rv = perform_fast_flush(in_unit, g_gport[0].gport_id);
    if (rv != BCM_E_NONE) {
        printf("perform_fast_flush_example_1 failure in perform_fast_flush for LIF - %ld: %s\n",
               g_gport[0].gport_id, bcm_errmsg(rv));
        return rv;
    }

    rv = perform_fast_flush(out_unit, g_gport[0].gport_id);
    if (rv != BCM_E_NONE) {
        printf("perform_fast_flush_example_1 failure in perform_fast_flush for LIF - %ld: %s\n",
               g_gport[0].gport_id, bcm_errmsg(rv));
        return rv;
    }

    /* If in ERP mode - Release after the flush (just for the test to succeed) */
    if (erp_mode) {
        erp_failover_id_idx = 0;
        block_state = 0;
        rv = erp_block(out_unit, erp_failover_id_idx, block_state);
        if (rv != BCM_E_NONE) {
            printf("perform_fast_flush_example_2 failure in erp_block for erp_failover_id - %ld to state - %ld: %s\n",
                   erp_failover_id_idx, block_state, bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Tests moving p2 from ring_group_1 to ring_group_2 
 * Any entries added to p2 prior to running this function could be removed with "perform_fast_flush_example_2" 
 * Any entries created after running this function can be removed with "perform_fast_flush_example_1"
 */
int set_port_to_ring_example_1(int unit, int erp_mode)
{
    int rv;
    rv = set_port_to_ring(unit, g_gport[1].gport_id, g_failover_id[1]);
    if (rv != BCM_E_NONE) {
        printf("set_port_to_ring_example_1 failure in set_port_to_ring for LIF - %ld, failover_id - %ld: %s\n",
               g_gport[1].gport_id, g_failover_id[1], bcm_errmsg(rv));
        return rv;
    }

    if (erp_mode) {
        /* Associate the LIFs to an ERP failover group */
        rv = set_port_to_erp(unit, 1, 1);
        if (rv != BCM_E_NONE) {
            printf("set_port_to_ring_example_1 failure in set_port_to_erp for LIF - %ld, erp_failover_id - %ld: %s\n",
                   g_gport[1].gport_id, g_erp_failover_id[1], bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}


/* 
 * Create an ERP group by creating a failover group
 */
int erp_group_create(int unit,
                     int flags,
                     bcm_failover_t *erp_failover_id)
{
    int rv, enable_state;

    rv = bcm_failover_create(unit, BCM_FAILOVER_INGRESS, erp_failover_id);
    if (rv != BCM_E_NONE) {
        printf("erp_group_create failure in bcm_failover_create: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf("ERP group created with failover_id %ld\n", *erp_failover_id);

    /* Set the default state */
    enable_state = 0;
    rv = bcm_failover_set(unit, *erp_failover_id, enable_state);
    if (rv != BCM_E_NONE) {
        printf("erp_group_create failure in bcm_failover_set with erp_failover_id - %ld to init state - %ld: %s\n",
               *erp_failover_id, enable_state, bcm_errmsg(rv));
        return rv;
    }
    printf("ERP group failover_id %ld initiated to state %ld\n", *erp_failover_id, enable_state);

    return BCM_E_NONE;
}


/* 
 * Destroy an ERP group by destroying a failover group
 */
int erp_group_destroy(int unit,
                      bcm_failover_t erp_failover_id)
{
    int rv;

    rv = bcm_failover_destroy(unit, erp_failover_id);

    printf("ERP group failover_id %ld destroyed\n", erp_failover_id);
    return rv;
}


/* 
 * Set a port to an ERP group. 
 * The operation updates the existing VLAN-Port info with a created ERP failover 
 * group ID.  
 */
int set_port_to_erp(int unit,
                    int gport_idx,
                    int erp_failover_id_idx)
{
    int rv;
    bcm_vlan_port_t vp;
    bcm_gport_t gport = g_gport[gport_idx].gport_id;
    bcm_failover_t erp_failover_id = g_erp_failover_id[erp_failover_id_idx];

    /* Get the vlan-port info */
    bcm_vlan_port_t_init(&vp);
    vp.vlan_port_id = gport;
    rv = bcm_vlan_port_find(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("set_port_to_erp failure in bcm_vlan_port_find with gport - %ld, erp_failover_id - %ld: %s\n",
               gport, erp_failover_id, bcm_errmsg(rv));
        return rv;
    }

    /* Attach the vlan-port to the ingress failover group */
    vp.ingress_failover_id = erp_failover_id;
    vp.failover_port_id = 0;
    vp.flags |= BCM_VLAN_PORT_REPLACE | BCM_VLAN_PORT_WITH_ID;

    /* Update the vlan-port info */
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("set_port_to_erp failure in bcm_vlan_port_create with gport - %ld, erp_failover_id - %ld: %s\n",
               gport, erp_failover_id, bcm_errmsg(rv));
        return rv;
    }

    g_gport[gport_idx].erp_failover_id = erp_failover_id;
    printf("ERP group failover_id %ld was associated with LIF %ld\n", erp_failover_id, gport);

    return BCM_E_NONE;
}


/* 
 * Remove a port from an ERP group. 
 * The operation updates the existing VLAN-Port info with a nullified ERP failover 
 * group ID.  
 */
int remove_port_from_erp(int unit,
                         int gport_idx)
{
    int rv;
    bcm_vlan_port_t vp;
    bcm_gport_t gport = g_gport[gport_idx].gport_id;

    /* Get the vlan-port info */
    bcm_vlan_port_t_init(&vp);
    vp.vlan_port_id = gport;
    rv = bcm_vlan_port_find(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("remove_port_from_erp failure in bcm_vlan_port_find with gport - %ld: %s\n",
               gport, bcm_errmsg(rv));
        return rv;
    }

    /* Reset the ingress failover group for the VLAN Port */
    vp.ingress_failover_id = 0;
    vp.failover_port_id = 1;
    vp.flags |= BCM_VLAN_PORT_REPLACE | BCM_VLAN_PORT_WITH_ID;

    /* Update the vlan-port info */
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("remove_port_from_erp failure in bcm_vlan_port_create with gport - %ld: %s\n",
               gport, bcm_errmsg(rv));
        return rv;
    }

    g_gport[gport_idx].erp_failover_id = -1;
    printf("LIF %ld was deassociated from an ERP group\n", gport);

    return BCM_E_NONE;
}


/* 
 * Block/Unblock an ERP port. 
 * Block/Unblock both the Ingress and Egress for LIFs that are associated with the failover group
 */
int erp_block(int unit,
              int erp_failover_id_idx,
              int block_state)
{
    int rv, discard_mode, idx;

    /* Block/Unblock the Ingress */
    rv = bcm_failover_set(unit, g_erp_failover_id[erp_failover_id_idx], block_state);
    if (rv != BCM_E_NONE) {
        printf("erp_block failure in bcm_failover_set with erp_failover_id - %ld to state - %ld: %s\n",
               g_erp_failover_id[erp_failover_id_idx], block_state, bcm_errmsg(rv));
        return rv;
    }
    printf("ERP Ingress block state for failover_id %ld set to %ld\n", g_erp_failover_id[erp_failover_id_idx], block_state);

    /* Block/Unblock the Egress - each LIF at a time */
    if (block_state == 0) {
        discard_mode = BCM_PORT_DISCARD_NONE;
    } else {
        discard_mode = BCM_PORT_DISCARD_EGRESS;
    }

    for (idx = 0; idx < MAX_RING_GPORTS; idx++) { 
        /* Check that the LIF is associated with the ERP failover group */
        if (g_gport[idx].erp_failover_id == g_erp_failover_id[erp_failover_id_idx]) {
            rv = bcm_port_discard_set(unit, g_gport[idx].gport_id, discard_mode);
            if (rv != BCM_E_NONE) {
                printf("erp_block failure in bcm_port_discard_set with gport - %ld to state - %ld: %s\n",
                       g_gport[idx].gport_id, discard_mode, bcm_errmsg(rv));
                return rv;
            }
            printf("ERP block state for Egress LIF %ld set to %ld\n", g_gport[idx].gport_id, block_state);
        }
    } 

    return BCM_E_NONE;
}



/* 
 * funstion:non_blocking_flush
 * Checking non blocking flush machine mode. 
 * 1. Add two groups of MAC addresses 
 * 2. Configure a non blocking delete in the flush machine 
 * 3. Poll the flush machine until the flush is done 
*/
int non_blocking_flush(int unit,
					 int local_port) {
	int rv;
	int a = 0;
	int flags = 0;

	bcm_l2_addr_t l2_addr;
	bcm_gport_t dest_port;
	bcm_mac_t mac1;
	bcm_vlan_t vid;
	int grp1, grp2;
	bcm_l2_addr_t match_addr;

	int i = 0;
	uint32 my_limit = 10000;
	int grp1_num_entries = 20;
	int grp2_num_entries = 10;

	/* init data */
	vid = 10;
	mac1[5] = 0x32;
	grp1 = 3; /* first group of entries will be flushed*/
	grp2 = 4; /* second group of entries will not be flushed*/	
	bcm_l2_addr_t_init(&l2_addr, mac1, vid);
	l2_addr.port = dest_port;
	BCM_GPORT_LOCAL_SET(dest_port, local_port);

	/* add L2 entries to group grp1=3*/
	l2_addr.group = grp1;
	for (i = 0; i < grp1_num_entries; i++) {
		rv = bcm_l2_addr_add(unit, &l2_addr);
		if (rv != BCM_E_NONE) {
			printf("Error, bcm_l2_addr_add\n");
		}
		l2_addr.vid++;
	}

	/* add L2 entries to group grp2=4*/
	l2_addr.group = grp2;
	for (i = 0; i < grp2_num_entries; i++) {
		rv =  bcm_l2_addr_add(unit, &l2_addr);
		if (rv != BCM_E_NONE) {
			printf("Error, bcm_l2_addr_add\n");
		}
		l2_addr.vid++;
	}

	/* configure flush machine to delete all the entries pertaining to grp1 , dont wait for it to finish passing on the MACT*/
	flags = BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_NO_CALLBACKS | BCM_L2_REPLACE_IGNORE_DISCARD_SRC | BCM_L2_REPLACE_IGNORE_DES_HIT;
	bcm_l2_addr_t_init(&match_addr, mac1, vid);
	match_addr.group = grp1;
	rv = bcm_l2_replace(unit, flags, &match_addr, 0, 0, 0);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_l2_replace\n");
	}

	/* poll on flush machine till it's finished passing on MACT, make sure flush machine is done,*/
	i=0; /* for limiting number of iterations */
	int flush_finished = 0;
	while (!flush_finished) {
		rv = bcm_switch_control_get(unit, bcmSwitchL2AgeDelete, &flush_finished); /*if flush is finished invalidate all flush table, finished=1 done , finished=0 still not done */
		if (rv != BCM_E_NONE) {
			printf("Error, bcm_switch_control_get\n");
			break;
		}
		if ((my_limit--) == 0) {
			printf("ERROR, reached limit!, bcm_switch_control_get wait is never happening");
			rv = BCM_E_TIMEOUT;
			break;
		}
	}

	return rv;
}
