/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Multicast Options~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_mutlicast_applications.c
 * Purpose: Example of multicast application for TM users.
 *
 * The Configuration enables creating two types of multicast groups:
 *      o   Port level
 *      o   Device level
 * 
 * In port-level scheduled MC, the configuration results in a fully scheduled MC scheme.
 * Its drawback is that it consumes the most resources
 * (that is, ingress memory MC linked-lists, VoQs, fabric utilization, and scheduling elements at the egress).
 * See cint_multicast_application_port_lvl_example.c for application-based example.
 * 
 * In the Device-level scheduled MC, configuration results in a partially scheduled MC scheme.
 * It consumes relatively few resources. It is a viable compromise between port-level scheduled MC
 * and Fabric MC. See cint_multicast_application_device_lvl_example.c for application-based example.
 * 
 * The MC configurations above require opening MC groups in the ingress and egress FAPs.
 * The following MC configuration types can be specified:
 *      o    Ingress Port-Level Scheduled: Sets the ingress FAP configuration for Port-Level Scheduled MC.
 *      o    Ingress Device-Level Scheduled: Sets the ingress FAP configuration for Device-Level Scheduled MC. 
 *
 * The Configuration assumes only one Soc_petra-B device and ERP port (for device level) is enabled.
 */

/*
 * General frame to create multicast group
 */
int
create_multicast_group(
    int unit,
    int is_ingress,
    int is_egress,
    int is_bitmap,
    bcm_multicast_t * multicast_group)
{
    bcm_error_t rv;
    uint32 flags;
    flags = BCM_MULTICAST_WITH_ID;

    if (is_ingress)
    {
        flags |= BCM_MULTICAST_INGRESS_GROUP;
    }

    if (is_egress)
    {
        flags |= BCM_MULTICAST_EGRESS_GROUP;
    }

    if (is_bitmap && (!is_ingress) && is_device_or_above(unit, JERICHO2))
    {
        /*
         * In Jericho 2 bitmap groups are not opened by default. To open a bitmap group an additional flg is required.
         * Ingress bitmap groups are not supported. 
         */
        flags |= BCM_MULTICAST_TYPE_PORTS_GROUP;
    }

    rv = bcm_multicast_create(unit, flags, multicast_group);
    return rv;
}

/*
 * This application creates Ingress multicast with a set of {system ports or VODs + cuds),
 * The configuration is a fully scheduled MC Scheme. 
 * MC traffic is replicated at the ingress FAP to each destination member port. 
 * After the replication, each copy is placed in a VoQ destined to the target member port. 
 * Each copy is dequeued from the VoQ upon reception of a credit from the target port. 
 * destinations parameter can be gport destination: system port, VOQ, FMQ
 */
int
port_level_scheduled_multicast(
    int unit,
    int multicast_id,
    bcm_gport_t * destinations,
    int *cuds,
    int nof_destinations)
{
    bcm_error_t rv = BCM_E_NONE;
    int is_ingress = 1;
    int is_egress = 0;
    bcm_multicast_t multicast_group = multicast_id;
    int index;
    bcm_multicast_replication_t rep_array;

    bcm_multicast_replication_t_init(&rep_array);

    printf("Creating group 0x%x:\n", multicast_id);
    rv = create_multicast_group(unit, is_ingress, is_egress, FALSE, &multicast_group);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    printf("Adding group entries:\n");
    for (index = 0; index < nof_destinations; index++)
    {
        printf("Adding destination 0x%x CUD 0x%x\n", destinations[index], cuds[index]);

        if (is_device_or_above(unit, JERICHO2))
        {
            rep_array.port = destinations[index];
            rep_array.encap1 = cuds[index];

            rv = bcm_multicast_add(unit, multicast_group, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
        }
        else
        {
            rv = bcm_multicast_ingress_add(unit, multicast_group, destinations[index], cuds[index]);
        }

        if (rv != BCM_E_NONE)
        {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

/*    
 * This application creates Ingress+Egress multicast that replicates in ingress a single
 * replication to each destination FAP (holding a member port in that MC group). 
 * Those FAP-level replicated copies are sent to the Egress Replication Port (ERP) 
 * of the appropriate FAP. At the Egress the MC traffic is further replicated to 
 * each member port using egress multicast.
 * The configuration is a partially scheduled MC Scheme.   
 * parameters destinations defined as system-ports/modports/local ports
 *
 * This function does not support a multiple device system.
 */
int
device_level_scheduled_multicast(
    int unit,
    int multicast_id,
    bcm_gport_t * destinations,
    int *cuds,
    int nof_destinations)
{
    bcm_error_t rv = BCM_E_NONE;
    int is_ingress = 1;
    int is_egress = 1;
    int is_bitmap = FALSE;
    bcm_multicast_t multicast_group_egr_ll = multicast_id;
    bcm_multicast_t multicast_group_egr_bmp = multicast_id;
    bcm_multicast_t multicast_group_ing = multicast_id;
    int index, dest;
    int port_max = nof_destinations;
    bcm_gport_t dest_erp;

    /*
     * hashing of module IDs is used to check if a replication was already added to ingress multicast for the module ID 
     */
    int sizeof_hash = 16, hash_empty_val = -1;
    int modid, modid_hash[sizeof_hash];
    bcm_gport_t temp[2], modport;
    int port_erp;
    int count_erp;
    int port, tm_port;
    int cud;
    int num_cores = is_device_or_above(unit, JERICHO) ? 2 : 1;
    bcm_multicast_replication_t rep_array;

    bcm_multicast_replication_t_init(&rep_array);

    if ((multicast_id < 4096))
    {
        /*
         * On Jericho 2 the bitmap groups are not opened by default.
         */
        if (is_device_or_above(unit, JERICHO2))
        {
            is_bitmap = TRUE;
        }
        else
        {
            /*
             * First 4K multicast entries are opened by default
             */
            is_egress = 0;
            rv = bcm_multicast_egress_set(unit, multicast_id, 0, 0, 0); /* If we assume that the egress group exists,
                                                                         * remove any existing replication */
            if (rv != BCM_E_NONE)
            {
                printf("Failed to configure egress MC group %d which is expected to exist to no replications. (%s) \n",
                       multicast_id, bcm_errmsg(rv));
                return rv;
            }
        }
    }
    printf("Step 1: create_multicast_group \n");
    if ((is_bitmap == TRUE) && (is_device_or_above(unit, JERICHO2)))
    {
        /*
         * When creaing bitmap groups in Jericho 2, they need to be added to a linked list group that will point to
         * them.
         *
         * Creating the primary egress linked list group that will point to the egress bitmap group,
         * together with the ingress multicast group with the same ID.
         */
        rv = create_multicast_group(unit, is_ingress, is_egress, FALSE, &multicast_group_egr_ll);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        multicast_group_ing = multicast_group_egr_ll;

        /*
         * Creating the egress bitmap group 
         */
        rv = create_multicast_group(unit, FALSE, is_egress, is_bitmap, &multicast_group_egr_bmp);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        /*
         * Adding the egress bitmap group as a replication in the egress linked list group 
         */
        BCM_MCAST_GPORT_BITMAP_ID_SET(dest, multicast_group_egr_bmp);
        rep_array.port = dest;
        rep_array.encap1 = BCM_IF_INVALID;
        rv = bcm_multicast_add(unit, multicast_group_egr_ll, BCM_MULTICAST_EGRESS_GROUP, 1, &rep_array);
    }
    else
    {
        /*
         * For legacy devices, or for the case for linked list groups in Jericho 2 the multicast group is the same for
         * the ingress and egress groups and they can be creates together. 
         */
        rv = create_multicast_group(unit, is_ingress, is_egress, is_bitmap, &multicast_group_ing);
        if (rv != BCM_E_NONE)
        {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        multicast_group_egr_bmp = multicast_group_egr_ll = multicast_group_ing;
    }
    /*
     * Init hash of module IDs used to check if we configured (added ingress multicast replication to) a module ID
     */
    for (index = 0; index < sizeof_hash; index++)
        modid_hash[index] = hash_empty_val;

    printf("Step 2: For each port, find his module id, and send destination accordingly \n");

    /*
     * CUD on multicast ingress must be the multicast egress group id 
     */
    cud = multicast_id;

    /*
     * Multicast ingress - FAP level destination (ERP)
     */
    /*
     * Call ERP system ports to set ingress group replications with the FAP destination ERP port
     */
    if (is_device_or_above(unit, JERICHO2))
    {
        num_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
    }
    else
    {
        num_cores = is_device_or_above(unit, JERICHO) ? 2 : 1;
    }

    rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_EGRESS_REPLICATION, 1 * num_cores, temp, &count_erp);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    if (count_erp == 0)
    {
        printf("ERP is not enabled, Multicast creation failed \n");
        bcm_multicast_destroy(unit, multicast_group_ing);
        return BCM_E_PARAM;
    }

    /*
     * For each port, find its module id, and send destination accordingly
     */
    for (index = 0; index < nof_destinations; index++)
    {
        if (is_device_or_above(unit, JERICHO2))
        {
            get_core_and_tm_port_from_port(unit, destinations[index], &modid, &tm_port);
            rv = bcm_stk_gport_sysport_get(unit, temp[modid], &dest_erp);
            if (rv != BCM_E_NONE)
            {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
            rep_array.port = dest_erp;
            rep_array.encap1 = multicast_id;
            rv = bcm_multicast_add(unit, multicast_group_ing, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
        }
        else
        {
            /*
             * get a modport gport for the port, for getting the module ID of the port
             */
            rv = bcm_stk_sysport_gport_get(unit, destinations[index], &modport);
            if (rv != BCM_E_NONE)
            {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
            modid = BCM_GPORT_MODPORT_MODID_GET(modport);

            /*
             * If we already configured this modid, skip adding it's ERP to the ingress multicast group
             */
            if (modid_hash[modid % sizeof_hash] != hash_empty_val)
            {
                if (modid_hash[modid % sizeof_hash] != modid)
                {
                    printf("ModID hash conflict between modids %d and %d\n", modid, modid_hash[modid % sizeof_hash]);
                    return -111;
                }
                continue;
            }
            modid_hash[modid % sizeof_hash] = modid;

            /*
             * In Jericho temp hold two ERP-ports - one of each core, use the ERP port relevant for the destination-port's
             * core.
             */
            rv = bcm_stk_gport_sysport_get(unit, temp[modid % num_cores], &dest_erp);
            if (rv != BCM_E_NONE)
            {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }

            rv = bcm_multicast_ingress_add(unit, multicast_group_ing, dest_erp, cud);
        }

        if (rv != BCM_E_NONE)
        {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }

    printf("step 3: Multicast egress, for each port add destination on egress device \n");
    /*
     * Multicast egress, for each port add destination on egress device 
     */
    for (index = 0; index < nof_destinations; index++)
    {
        /*
         * CUD on multicast ingress must be the multicast egress group id 
         */
        cud = cuds[index];
        if (is_device_or_above(unit, JERICHO2))
        {
            if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL))
            {
                bcm_core_t core;
                bcm_module_t mreps[num_cores];
                for (core = 0; core < num_cores; core++)
                {
                    mreps[core] = core;
                }
                rv = bcm_fabric_multicast_set(unit, multicast_id, 0, num_cores, mreps);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, bcm_fabric_multicast_set, multicast_group multicast_group \n");
                    return rv;
                }
            }

            rep_array.port = destinations[index];
            rep_array.encap1 = cud;

            rv = bcm_multicast_add(unit, multicast_group_egr_bmp, BCM_MULTICAST_EGRESS_GROUP, 1, &rep_array);
        }
        else
        {
            rv = bcm_multicast_egress_add(unit, multicast_group_egr_ll, destinations[index], cud);
        }
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
        {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}
