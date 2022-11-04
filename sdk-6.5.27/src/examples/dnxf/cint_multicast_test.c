/*
 *  
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Multicast test application
 * Cint shows two examples of multicast applications: direct and indirect
 * Direct: set multicast table.
 * Indirect: set multicast table and create a static topology. An example of such multicast mode will be
 * to work with module ids that higher than 127.
 * 
 * Focal funtions:
 * o    multicast_test__fabric_direct_mode_set()
 * o    multicast_test__fabric_indirect_mode_set()
 */

/* Flag for 3-stage multicast configuration */
int mc_3_stage_cfg = 0;

/* Multicast indirect mode indication. Used by multicast_test__fabric_multicast_3_stage_cfg */
int mc_mode_indirect = 0;

/* 
 * Create multicast id, and set fabric to it.
 */
int
multicast_test__fabric_set(
    int fe_unit,
    int fabric_multicast_id,
    bcm_module_t * modid_array,
    int nof_modules)
{

    /* Parameters for 3-stage multicast configuration */
    int modid_iter;
    int nof_mc_grps = 1;
    int mc_grps[nof_mc_grps] = {fabric_multicast_id};
    int nof_grp_modids[nof_mc_grps] = {nof_modules};
    int nof_links = 192;
    int is_dnxf = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_device_member_get(fe_unit, 0, bcmDeviceMemberDNXF, &is_dnxf), "");
    if(is_dnxf)
    {
        nof_links = *(dnxc_data_get(fe_unit, "port", "general", "nof_links", NULL));
    }

    int modids_array_size = nof_links;
    bcm_module_t modids_array[nof_mc_grps][modids_array_size];

    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(fe_unit, BCM_MULTICAST_WITH_ID, &fabric_multicast_id), "");

    if (mc_3_stage_cfg) {
        /* 3-stage multicast configuration */
        for (modid_iter=0; modid_iter < nof_grp_modids[0]; modid_iter++) {
            modids_array[0][modid_iter] = modid_array[modid_iter];
        }
        BCM_IF_ERROR_RETURN_MSG(multicast_test__fabric_multicast_3_stage_cfg(fe_unit, nof_mc_grps, mc_grps, nof_grp_modids, modids_array, modids_array_size), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(fe_unit, fabric_multicast_id, 0, nof_modules, modid_array), "");
    }

    return BCM_E_NONE;
}

/*
 * Add a copy to a fap with a given fabric MC id
 */
int
multicast_test__fabric_replication_add(
    int fe_unit,
    int fabric_multicast_id,
    bcm_module_t modid_to_add)
{
    uint32 flags = 0;
    int destination_count = 0, destination_idx, real_modid_decoded;
    /**
     * This is practically also the MAX NOF of elements in MC id group
     */
    int nof_links = *(dnxc_data_get(fe_unit, "port", "general", "nof_links", NULL));
    bcm_module_t destid_array[nof_links];

    /* Parameters for 3-stage multicast configuration */
    int modid_iter;
    int nof_mc_grps = 1;
    int mc_grps[nof_mc_grps] = {fabric_multicast_id};
    int nof_grp_modids[nof_mc_grps];
    int modids_array_size = nof_links;
    bcm_module_t modids_array[nof_mc_grps][modids_array_size];

    BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_get(fe_unit, fabric_multicast_id, flags, nof_links, &destination_count, destid_array), "");

    /**
     * Check if destination id already added to MC id
     */
    for (destination_idx = 0; destination_idx < destination_count; destination_idx++) {
        if (BCM_FABRIC_MODID_IS_LOCAL(modid_to_add)) {
            BCM_IF_ERROR_RETURN_MSG(bcm_fabric_modid_local_mapping_get(fe_unit, modid_to_add, &real_modid_decoded), "");
        } else {
            real_modid_decoded = modid_to_add;
        }
        /**
         * bcm_fabric_multicast_get will return REAL_MODID even if a local MODID is provided
         */
        if (real_modid_decoded == destid_array[destination_idx]) {
            printf("Modid %d already added to multicast ID %d \n", modid_to_add, fabric_multicast_id);
            return BCM_E_PARAM;
        }
    }

    /**
     * Add new device id to MC id replication devices
     */
    destid_array[destination_count] = modid_to_add;

    /**
     * Reconfigure MC id recipients
     */
    if (mc_3_stage_cfg) {
        /* 3-stage multicast configuration */
        nof_grp_modids[0] = destination_count + 1;
        for (modid_iter=0; modid_iter < nof_grp_modids[0]; modid_iter++) {
            modids_array[0][modid_iter] = destid_array[modid_iter];
        }
        BCM_IF_ERROR_RETURN_MSG(multicast_test__fabric_multicast_3_stage_cfg(fe_unit, nof_mc_grps, mc_grps, nof_grp_modids, modids_array, modids_array_size), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(fe_unit, fabric_multicast_id, 0, destination_count + 1, destid_array), "");
    }

    return BCM_E_NONE;
}


/*
 * Delete a copy to a fap with a given fabric MC id
 */
int
multicast_test__fabric_replication_delete(
    int fe_unit,
    int fabric_multicast_id,
    bcm_module_t modid_to_delete)
{
    uint32 flags = 0;
    int destination_count = 0, destination_idx = 0, new_destination_idx = 0, is_found = 0, real_modid_decoded;
    /**
     * This is practically also the MAX NOF of elements in MC id group
     */
    int nof_links = *(dnxc_data_get(fe_unit, "port", "general", "nof_links", NULL));
    bcm_module_t destid_array[nof_links], new_destid_array[nof_links];

    /* Parameters for 3-stage multicast configuration */
    int modid_iter;
    int nof_mc_grps = 1;
    int mc_grps[nof_mc_grps] = {fabric_multicast_id};
    int nof_grp_modids[nof_mc_grps];
    int modids_array_size = nof_links;
    bcm_module_t modids_array[nof_mc_grps][modids_array_size];

    BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_get(fe_unit, fabric_multicast_id, flags, nof_links, &destination_count, destid_array), "");

    /**
     * Check if destination id exists in MC id group
     */
    new_destination_idx = 0;
    for (destination_idx = 0; destination_idx < destination_count; destination_idx++) {
        if (BCM_FABRIC_MODID_IS_LOCAL(modid_to_delete)) {
            BCM_IF_ERROR_RETURN_MSG(bcm_fabric_modid_local_mapping_get(fe_unit, modid_to_delete, &real_modid_decoded), "");
        } else {
            real_modid_decoded = modid_to_delete;
        }
        /**
         * bcm_fabric_multicast_get will return REAL_MODID even if a local MODID is provided
         */
        if (real_modid_decoded == destid_array[destination_idx]) {
            is_found = 1;
            continue;
        }
        new_destid_array[new_destination_idx] = destid_array[destination_idx];
        new_destination_idx++;
    }

    if (!is_found) {
        printf("Modid %d was not a destination to MC id  %d \n", modid_to_delete, fabric_multicast_id);
        return BCM_E_PARAM;
    }
    /**
     * Add new device id to MC id replication devices
     */

    /**
     * Reconfigure MC id recipients
     */
    if (mc_3_stage_cfg) {
        /* 3-stage multicast configuration */
        nof_grp_modids[0] = destination_count - 1;
        for (modid_iter=0; modid_iter < nof_grp_modids[0]; modid_iter++) {
            modids_array[0][modid_iter] = new_destid_array[modid_iter];
        }
        BCM_IF_ERROR_RETURN_MSG(multicast_test__fabric_multicast_3_stage_cfg(fe_unit, nof_mc_grps, mc_grps, nof_grp_modids, modids_array, modids_array_size), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(fe_unit, fabric_multicast_id, 0, destination_count - 1, new_destid_array), "");
    }

    return BCM_E_NONE;
}

/* 
 * Create multicast id, and set egress of unit to it.
 */
int
multicast_test__egress_set(
    int unit,
    int mc_id,
    bcm_port_t port)
{
    char error_msg[200]={0,};
    bcm_if_t encap = 1;
    bcm_info_t info;

    snprintf(error_msg, sizeof(error_msg), "(%d)", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &mc_id), error_msg);

    if (is_device_or_above(unit, JERICHO2) == 1)
    {
        bcm_multicast_replication_t rep;

        BCM_GPORT_LOCAL_SET(&(rep.port), port);
        rep.encap1 = -1;
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_set(unit, mc_id, BCM_MULTICAST_EGRESS_GROUP, 1, &rep), error_msg);
    }
    else
    {
        bcm_gport_t local_gports;
        int is_jer;
        BCM_IF_ERROR_RETURN_MSG(is_device_jericho_only(unit, &is_jer), error_msg);
        
        if (is_jer)
        {
            encap = -1;
        }

        BCM_GPORT_LOCAL_SET(&(local_gports), port);
        BCM_IF_ERROR_RETURN_MSG(bcm_multicast_egress_set(unit, mc_id, 1, &local_gports, &encap), error_msg);
    }

    return BCM_E_NONE;
}

/* 
 * Set multicast direct application. 
 * Given multicast id, create and replicate multicast id over all requested devices. 
 */
int
multicast_test__fabric_direct_mode_set(
    int fe_unit,
    int fabric_multicast_id,
    bcm_module_t * modid_array,
    int nof_modules)
{

    BCM_IF_ERROR_RETURN_MSG(multicast_test__fabric_set(fe_unit, fabric_multicast_id, modid_array, nof_modules), "");

    return BCM_E_NONE;
}

/* 
 * Function for 3-stage multicast configuration
 * Parameters:
 * - nof_mc_ids                 Number of multicast ids to be configured by 'bcm_fabric_multicast_multi_set'
 * - mc_id_array                The array should be defined as array_name[nof_mc_ids]. Each item indicates a multicast id
 * - nof_modids_per_mc_id_array The array should be defined as array_name[nof_mc_ids]. 
 *                              Each item indicates the number of valid destinations in modid_array[mc_id_index][]
 * - modid_array                The array should be defined as array_name[nof_mc_ids][modid_array_size]
 *                              Items modid_array[mc_id_index][] are destinations of relevant MC ID
 * - modid_array_size           Array size of 'modid_array'
 */
int
multicast_test__fabric_multicast_3_stage_cfg(
    int fe_unit,
    int nof_mc_ids,
    int * mc_id_array,
    int * nof_modids_per_mc_id_array,
    bcm_module_t * modid_array,
    int modid_array_size)
{
    int grp_iter, modid_iter;
    bcm_module_t modid_vct;

    /**
     * This is practically also the MAX NOF of elements in MC id group
     */
    int nof_links = *(dnxc_data_get(fe_unit, "port", "general", "nof_links", NULL));
    bcm_multicast_t group[nof_links];
    bcm_fabric_module_vector_t dests[nof_links];
    printf("#### Starting 3-stage multicast configuration. %d MC ID(s) will be added #####\n", nof_mc_ids);

    for (grp_iter = 0; grp_iter < nof_mc_ids; grp_iter++) {
        group[grp_iter] = mc_id_array[grp_iter];

        /* Set destination vector for the MC group */
        for (modid_iter = 0; modid_iter < nof_modids_per_mc_id_array[grp_iter]; modid_iter++) {
            modid_vct =  modid_array[grp_iter * modid_array_size + modid_iter];

            if (BCM_FABRIC_MODID_IS_GROUP(modid_vct)) {
                /* FAP-Group: use FAP-Group ID(without prefix) to set the vector */
                modid_vct = BCM_FABRIC_GROUP_MODID_UNSET(modid_vct);
            } else if (BCM_FABRIC_MODID_IS_LOCAL(modid_vct)) {
                /* Local Modid */
                if (mc_mode_indirect) {
                    /* Indirect mode: use local modid(without prefix) to set the vector */
                    modid_vct = BCM_FABRIC_LOCAL_MODID_UNSET(modid_vct);
                } else {
                    /* Direct mode: use its real modid to set the vector */
                    BCM_IF_ERROR_RETURN_MSG(bcm_fabric_modid_local_mapping_get(fe_unit, modid_array[grp_iter * modid_array_size + modid_iter], &modid_vct), "");
                }
            } else {
                /* Real Modid */
                if (mc_mode_indirect) {
                    /* Use its local modid to set the vector */
                    /* User should know the mapping between real-modid and local-modid(configured via 'bcm_fabric_modid_local_mapping_set')
                       Use hardcode here for test purpose */
                    modid_vct =   modid_array[grp_iter * modid_array_size + modid_iter] == 160 ? 70 : 90;
                }
            }
            BCM_FABRIC_MODULE_VEC_SET(dests[grp_iter], modid_vct);
        }
    }

    /* multicast_multi_set with 3 steps */
    BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_multi_set(fe_unit, BCM_FABRIC_MULTICAST_SET_ONLY, nof_mc_ids, group, dests), "(SET_ONLY)");
    BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_multi_set(fe_unit, BCM_FABRIC_MULTICAST_COMMIT_ONLY, nof_mc_ids, group, dests), "(COMMIT_ONLY)");
    BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_multi_set(fe_unit, BCM_FABRIC_MULTICAST_STATUS_ONLY, nof_mc_ids, group, dests), "(STATUS_ONLY)");
    printf("#### End of 3-stage multicast configuration. %d MC IDs added #####\n", nof_mc_ids);

    return BCM_E_NONE;
}

