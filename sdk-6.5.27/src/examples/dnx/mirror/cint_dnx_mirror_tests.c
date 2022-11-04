/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_mirror_tests.c
 * Purpose: Examples of Mirror tests and usage
 */

/*
 * Usage:
 *
 * BCM> cint examples/dnx/cint_dnx_utils_l3.c
 * BCM> cint examples/dnx/cint_dnx_mirror_tests.c
 *
 */


/* Mirroring destinations traverse callback, returns non zero value on error
 *
 *   The Mirror destination traverse callback might be used to traverse all
 *   created mirror destinations and add them to an array.
 */
int
sand_mirror_destination_list_id(
    int unit,
    bcm_mirror_destination_t * mirror_dest,     /* we get this information from the traverse function */
    void *user_data)
{       /* user data, in our case bcm_gport_t** - a ponter to the pointer with the current location in the output array 
         */
    bcm_gport_t **cur_pointer = user_data;
    if (!mirror_dest || !cur_pointer)
    {
        printf("Callback received a null pointer\n");
        return BCM_E_PARAM;
    }
    **cur_pointer = mirror_dest->mirror_dest_id;
    /*
     * printf("callback handling destination %u\n", BCM_GPORT_MIRROR_GET(**cur_pointer)); 
     */
    ++*cur_pointer;
    return BCM_E_NONE;
}

/* This function just wraps bcm_mirror_destination_traverse.
 * It is needed due to a cint implementation : When returning from bcm_mirror_destination_traverse,
 * the calling function also exists immediately, if the callback was called at least once.
 */
int
sand_mirror_destination_traverse_wrap(
    int unit,
    bcm_mirror_destination_traverse_cb cb,
    void *user_data)
{
    return bcm_mirror_destination_traverse(unit, cb, user_data);
    /*
     * this code is not reached in case a callback was called, due to a cint implementation 
     */
}

/* Delete all existing mirror destinations */
int
sand_delete_all_mirror_destinations(
    int unit)
{
    int i, size;
    bcm_gport_t mirr_dest_ids[1024];    /* This should be big enough to hold all possible destinations, which is 15 in
                                         * Arad */
    bcm_gport_t *mirr_dest_ids_current_location = mirr_dest_ids;
    char error_msg[200]={0,};

    /*
     * Get the created destinations by traversing over the destinations. This could have been done by attempting
     * bcm_mirror_destination_traverse on all possible destinations 
     */
    BCM_IF_ERROR_RETURN_MSG(sand_mirror_destination_traverse_wrap(unit,
                                                   sand_mirror_destination_list_id, &mirr_dest_ids_current_location),
                                                   "could not delete all mirror destinations");
    size = mirr_dest_ids_current_location - mirr_dest_ids;      /* number of destinations traversed */

    /*
     * Destroy each found destination 
     */
    for (i = 0; i < size; ++i)
    {
        snprintf(error_msg, sizeof(error_msg), "could not delete all mirror destinations - failed to destroy mirror destination %u",
            BCM_GPORT_MIRROR_GET(mirr_dest_ids[i]));
        BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_destroy(unit, mirr_dest_ids[i]), error_msg);
    }
    return BCM_E_NONE;
}

/* get the (sorted) ids of up to port_num ports, return 0 on success */
int
sand_get_n_ports(
    int unit,
    int port_num,
    int *out_ports,
    int *out_port_num)
{
    char error_msg[200]={0,};
    bcm_port_config_t c;
    bcm_port_t p;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    if (!out_ports || !out_port_num || port_num <= 0)
    {
        printf("Error, illegal arguments\n");
        return BCM_E_PARAM;
    }
    snprintf(error_msg, sizeof(error_msg), "failed to get the ports of unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_config_get(unit, &c), error_msg);
    *out_port_num = 0;
    BCM_PBMP_ITER(c.nif, p)
    {
        if (p >= 0 && p < 512)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, p, &flags, &interface_info, &mapping_info), error_msg);
            if (flags & BCM_PORT_ADD_KBP_PORT)
            {
                continue;
            }
            out_ports[*out_port_num] = p;
            ++*out_port_num;
            if (*out_port_num >= port_num)
            {
                break;
            }
        }
    }
    return BCM_E_NONE;
}

/* this function is creating recycle mirror port and mapping it to forward port */
int
sand_mirror_port_to_rcy_port_map(
    int unit,
    int forward_port,
    int add_rcy_port,
    int rcy_mirror_port,
    uint32 tm_port,
    uint32 channel)
{
    int core, tmp;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;

    bcm_port_mapping_info_t_init(&mapping_info);
    
    /*
     * retrieve core from forward port
     */
    BCM_IF_ERROR_RETURN_MSG(get_core_and_tm_port_from_port(unit, forward_port, &core, &tmp), "");

    /*
     * setting relevant parameters for mirror port 
     */
    interface_info.interface = BCM_PORT_IF_RCY_MIRROR;  /* interface of port. */
    interface_info.interface_id = -1;   /* should be indicated in case the id can't be derived from phy_port. */
    mapping_info.channel = channel;     /* logical port channel. */
    mapping_info.core = core;   /* core id */
    mapping_info.tm_port = tm_port;     /* tm port id */
    mapping_info.pp_port = -1;     /* set to invalid pp port */
    mapping_info.base_hr = -1;
    rcy_map_info.rcy_mirror_port = rcy_mirror_port;     /* Mirror port */
    rcy_map_info.priority_bitmap =
        BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_NORMAL | BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_HIGH;
    /*
     * Used to specify which priorities to map. Possible values will be named BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_ 
     */

    if (add_rcy_port)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_add(unit, rcy_mirror_port, BCM_PORT_ADD_CONFIG_CHANNELIZED, &interface_info, &mapping_info),
            "failed to add recycle mirror port");
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_to_rcy_port_map_set(unit, 0, forward_port, &rcy_map_info),
        "failed to map port to recycle mirror port");

    return BCM_E_NONE;
}

/*
 * Internal function for sand_mirror_destinations_arad_test_1
 * This function returns the relevant parameters for DNX devices,
 * including the mirr_dests and mirr_dests_expected.
 */
int
sand_mirror_destinations_dnx_params_get(
    int unit,
    bcm_multicast_t *mc_id,
    int *found_ports,
    uint16 *packet_copy_size,
    unsigned int *prob_shift,
    unsigned int *egr_prob_shift,
    unsigned int *egress_sample_rate_dividend,
    bcm_gport_t *mirr_dests,
    bcm_gport_t *mirr_dests_expected)
{
    char error_msg[200]={0,};
    int max_dest_id=15;
    uint32 *max_val = dnxc_data_get(unit, "snif", "ingress", "prob_max_val", NULL);

    *prob_shift = *max_val;
    max_val = dnxc_data_get(unit, "snif", "egress", "prob_max_val", NULL);
    *egr_prob_shift = *max_val;
    *egress_sample_rate_dividend = *egr_prob_shift;
    *packet_copy_size = 256;

    /** Create mirr_dests array for jericho2 use system ports only */
    snprintf(error_msg, sizeof(error_msg), "failed to retrieve system port for port %d", found_ports[1]);
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(unit, found_ports[1], mirr_dests[0]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "failed to retrieve system port for port %d", found_ports[3]);
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(unit, found_ports[3], mirr_dests[1]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "failed to retrieve system port for port %d", found_ports[2]);
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(unit, found_ports[2], mirr_dests[2]), error_msg);
    BCM_GPORT_MCAST_SET(mirr_dests[3], mc_id[0]);
    BCM_GPORT_MCAST_SET(mirr_dests[4], mc_id[1]);
    BCM_GPORT_MCAST_SET(mirr_dests[5], mc_id[2]);
    BCM_GPORT_MCAST_SET(mirr_dests[6], mc_id[3]);
    BCM_GPORT_MCAST_SET(mirr_dests[7], mc_id[4]);
    BCM_GPORT_MCAST_SET(mirr_dests[8], mc_id[5]);
    BCM_GPORT_MCAST_SET(mirr_dests[9], mc_id[1]);
    BCM_GPORT_MCAST_SET(mirr_dests[10], mc_id[0]);
    snprintf(error_msg, sizeof(error_msg), "failed to retrieve system port for port %d", found_ports[1]);
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(unit, found_ports[1], mirr_dests[11]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "failed to retrieve system port for port %d", found_ports[1]);
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(unit, found_ports[1], mirr_dests[12]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "failed to retrieve system port for port %d", found_ports[2]);
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(unit, found_ports[2], mirr_dests[13]), error_msg);
    snprintf(error_msg, sizeof(error_msg), "failed to retrieve system port for port %d", found_ports[3]);
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_gport_sysport_get(unit, found_ports[3], mirr_dests[14]), error_msg);

    /** Create mirr_dests_expected array, for jericho2 we expect the same port types to be returned */
    sal_memcpy(mirr_dests_expected, mirr_dests, sizeof(mirr_dests[0])*(max_dest_id));

    return BCM_E_NONE;
}


/* This function tests all the mirror destination handling APIs (bcm_mirror_destination_*).
 * It assumes the device is Arad, in the mirror IDs available.
 */
int
sand_mirror_destinations_arad_test_1(
    int unit)
{
    int i, size;
    int min_dest_id = 1, max_dest_id = 15, max_egr_dest_id = 7;
    int found_ports_num, found_ports[8], dividend;
    bcm_gport_t dest_id;
    bcm_mirror_destination_t dest = { 0 };
    bcm_gport_t mirr_dest_ids[max_dest_id - min_dest_id + 1];   /* will contain gport representation of mirror
                                                                 * destinations */
    bcm_gport_t mirr_dests_expected[max_dest_id - min_dest_id + 1];
    int dest_ids[max_dest_id - min_dest_id + 1];        /* will contain mirror destination IDs for processing traversal 
                                                         * output */
    bcm_gport_t mirr_dests[max_dest_id - min_dest_id + 1];
    bcm_gport_t *mirr_dest_ids_current_location = mirr_dest_ids;
    int default_nof_mc_groups =  64 * 1024;
    int nof_mc_groups = *(dnxc_data_get(unit, "multicast", "params", "max_egr_mc_groups", NULL));
    unsigned int prob_shift, egr_prob_shift, egress_sample_rate_dividend;;
    uint16 egress_packet_copy_size = 128;
    uint16 packet_copy_size;
    bcm_multicast_t mc_id[6];
    mc_id[0] = 4096;
    mc_id[1] = 8123;
    mc_id[2] = 64000 % nof_mc_groups;
    mc_id[3] = 23123 % nof_mc_groups;
    mc_id[4] = 35777 % nof_mc_groups;
    mc_id[5] = 5000;
    char error_msg[200]={0,};

    BCM_IF_ERROR_RETURN_MSG(sand_get_n_ports(unit, 8, found_ports, &found_ports_num), "failed to get ports for the test");
    if (found_ports_num < 4)
    {
        printf("found only %d ports for the test which is not enough\n", found_ports_num);
        return BCM_E_FAIL;
    }

    /*
     * Get the created destinations by traversing over the destinations. This could have been done by attempting
     * bcm_mirror_destination_traverse on all possible destinations 
     */
    BCM_IF_ERROR_RETURN_MSG(sand_mirror_destination_traverse_wrap(unit, sand_mirror_destination_list_id, &mirr_dest_ids_current_location), "");
    size = mirr_dest_ids_current_location - mirr_dest_ids;      /* number of destinations traversed */

    /*
     * Destroy each found destination 
     */
    for (i = 0; i < size; ++i)
    {
        printf("destroying previously created mirror destination %u\n", BCM_GPORT_MIRROR_GET(mirr_dest_ids[i]));
        BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_destroy(unit, mirr_dest_ids[i]), "");
    }

    /* Call the appropriate function to get the parameters, and mirror destinations (both for set and expected) */
    BCM_IF_ERROR_RETURN_MSG(sand_mirror_destinations_dnx_params_get(unit, mc_id, found_ports, &packet_copy_size, &prob_shift, &egr_prob_shift, &egress_sample_rate_dividend, mirr_dests, mirr_dests_expected), "");


    /*
     * create destinations without selected the destination id 
     */
    for (i = min_dest_id; i <= max_dest_id; ++i)
    {   /* init dest_ids to 0 */
        dest_ids[i - min_dest_id] = -1;
    }
    for (i = min_dest_id; i <= max_dest_id; ++i)
    {
        dividend = egress_sample_rate_dividend;
        bcm_mirror_destination_t_init(&dest);
        dest.gport = mirr_dests[i - min_dest_id];
        dest.flags = 0;
        dest.sample_rate_dividend = (prob_shift / max_dest_id) * i;
        dest.sample_rate_divisor = prob_shift;
        dest.egress_sample_rate_divisor = egr_prob_shift;
        dest.egress_sample_rate_dividend = dividend;
        dest.packet_copy_size = packet_copy_size;
        dest.egress_packet_copy_size = egress_packet_copy_size;
        snprintf(error_msg, sizeof(error_msg), "failed to create mirror destination that should have been number %d", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_create(unit, &dest), error_msg);
        /*
         * check that destinations are not allocated twice, and store mapping to the creation loop index 
         */
        unsigned id = BCM_GPORT_MIRROR_GET(dest.mirror_dest_id);
        if (id < min_dest_id || id > max_dest_id)
        {
            printf("created mirror destination ID %u, is out of range\n", id);
            return BCM_E_FAIL;
        }
        else if (dest_ids[id - min_dest_id] != -1)
        {
            printf("mirror destination created destination a second time with ID %u\n", id);
            return BCM_E_FAIL;
        }
        dest_ids[id - min_dest_id] = i;

    }

    /*
     * get destinations information, compare to the expected information, and destroy the destinations 
     */
    for (i = min_dest_id; i <= max_dest_id; ++i)
    {
        BCM_GPORT_MIRROR_SET(dest_id, i);
        bcm_mirror_destination_t_init(&dest);
        snprintf(error_msg, sizeof(error_msg), "failed to get mirror destination information, destination id %d", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_get(unit, dest_id, &dest), error_msg);
        /*
         * check that destinations are allocated in order. This is Arad implementation specific 
         */
        if (BCM_GPORT_MIRROR_GET(dest.mirror_dest_id) != i)
        {
            printf("Retrieved mirror destination id %d, should have been %d\n",
                   BCM_GPORT_MIRROR_GET(dest.mirror_dest_id), i);
            return BCM_E_FAIL;
        }

        bcm_gport_t expected = mirr_dests_expected[dest_ids[i - min_dest_id] - min_dest_id];
        if (dest.gport != expected)
        {
            printf("In mirror destination %d, retrieved gport 0x%lx, should have been 0x%lx\n",
                   i, dest.gport, expected);
            return BCM_E_FAIL;
        }
        if (dest.sample_rate_dividend != (prob_shift / max_dest_id) * i || dest.sample_rate_divisor != prob_shift)
        {
            printf("Wrong mirror probability in %d:  %u / %u\n", i,
                   dest.sample_rate_dividend, dest.sample_rate_divisor);
            return BCM_E_FAIL;
        }

        if (i <= max_egr_dest_id && dest.egress_packet_copy_size != egress_packet_copy_size)
        {
            printf("Wrong egress mirror crop value in %d:  %u / %u\n", i,
                   dest.egress_packet_copy_size, egress_packet_copy_size);
            return BCM_E_FAIL;
        }
        if (dest.packet_copy_size != packet_copy_size)
        {
            printf("Wrong mirror crop value in %d:  received %u but expected %u\n", i,
                   dest.packet_copy_size, packet_copy_size);
            return BCM_E_FAIL;
        }

        snprintf(error_msg, sizeof(error_msg), "Failed to destroy created mirror destination with ID %d", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_destroy(unit, dest_id), error_msg);
    }
    bcm_mirror_destination_t_init(&dest);

    /*
     * traverse and check that no destinations exist 
     */
    mirr_dest_ids_current_location = mirr_dest_ids;
    BCM_IF_ERROR_RETURN_MSG(sand_mirror_destination_traverse_wrap(unit, sand_mirror_destination_list_id, &mirr_dest_ids_current_location), "");
    if (mirr_dest_ids_current_location != mirr_dest_ids)
    {
        printf("Destinations were found when they should not have. The first one found is %u\n",
               BCM_GPORT_MIRROR_GET(mirr_dest_ids[0]));
        return BCM_E_FAIL;
    }

    /*
     * create destinations with selected id 
     */
    for (i = max_dest_id; i >= min_dest_id; --i)
    {
        dest.gport = mirr_dests[i - min_dest_id];
        dest.flags = BCM_MIRROR_DEST_WITH_ID;
        BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, i);
        snprintf(error_msg, sizeof(error_msg), "failed to create mirror destination number %d", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_create(unit, &dest), error_msg);
        /*
         * check that destinations are allocated in order. This is Arad implementation specific 
         */
        if (BCM_GPORT_MIRROR_GET(dest.mirror_dest_id) != i)
        {
            printf("mirror destination created destination number %d, and not the given %d\n",
                   BCM_GPORT_MIRROR_GET(dest.mirror_dest_id), i);
            return BCM_E_FAIL;
        }
    }
    /*
     * get destinations information, compare to the expected information 
     */
    for (i = min_dest_id; i <= max_dest_id; ++i)
    {
        BCM_GPORT_MIRROR_SET(dest_id, i);
        snprintf(error_msg, sizeof(error_msg), "failed to get mirror destination information, destination %d", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_get(unit, dest_id, &dest), error_msg);
        if (BCM_GPORT_MIRROR_GET(dest.mirror_dest_id) != i)
        {
            printf("Retrieved mirror destination %d, should have been %d\n",
                   BCM_GPORT_MIRROR_GET(dest.mirror_dest_id), i);
            return BCM_E_FAIL;
        }
        bcm_gport_t expected = mirr_dests_expected[i - min_dest_id];
        if (dest.gport != expected)
        {
            printf("In mirror destination %d, gport 0x%lx, should have been 0x%lx\n", i, dest.gport, expected);
            return BCM_E_FAIL;
        }
    }

    bcm_mirror_destination_t_init(&dest);
    /*
     * Recreate destinations with selected id, and different information 
     */
    for (i = max_dest_id; i >= min_dest_id; --i)
    {
        dest.gport = mirr_dests[max_dest_id - i];
        dest.flags = BCM_MIRROR_DEST_WITH_ID | BCM_MIRROR_DEST_REPLACE;
        BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, i);
        snprintf(error_msg, sizeof(error_msg), "failed to recreate mirror destination number %d", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_create(unit, &dest), error_msg);
        /*
         * check that destinations are allocated in order. This is Arad implementation specific 
         */
        if (BCM_GPORT_MIRROR_GET(dest.mirror_dest_id) != i)
        {
            printf("mirror destination recreated destination number %d, and not the given %d\n",
                   BCM_GPORT_MIRROR_GET(dest.mirror_dest_id), i);
            return BCM_E_FAIL;
        }
    }

    /*
     * Get the destinations by traversing over the destinations. 
     */
    mirr_dest_ids_current_location = mirr_dest_ids;
    BCM_IF_ERROR_RETURN_MSG(sand_mirror_destination_traverse_wrap(unit,
                                                   sand_mirror_destination_list_id, &mirr_dest_ids_current_location),
                                                   "Failed traversing all the destinations");
    size = mirr_dest_ids_current_location - mirr_dest_ids;      /* number of destinations traversed */
    if (size != max_dest_id - min_dest_id + 1)
    {
        printf("Expected to traverse %d destinations, but traversed only %d\n", max_dest_id - min_dest_id + 1, size);
        return BCM_E_FAIL;
    }
    for (i = min_dest_id; i <= max_dest_id; ++i)
    {   /* init dest_ids to 0 */
        dest_ids[i - min_dest_id] = -1;
    }
    for (i = min_dest_id; i <= max_dest_id; ++i)
    {   /* test that each destination appears once */
        unsigned id = BCM_GPORT_MIRROR_GET(mirr_dest_ids[i - min_dest_id]);
        if (id < min_dest_id || id > max_dest_id)
        {
            printf("mirror destination ID %u retrieved in traverse, is out of range\n", id);
            return BCM_E_FAIL;
        }
        else if (dest_ids[id - min_dest_id] != -1)
        {
            printf("mirror destination created destination a second time with ID %u\n", id);
            return BCM_E_FAIL;
        }
        dest_ids[id - min_dest_id] = i;
    }

    /*
     * get destinations information, compare to the expected information, and destroy the destinations 
     */
    for (i = min_dest_id; i <= max_dest_id; ++i)
    {
        BCM_GPORT_MIRROR_SET(dest_id, i);
        snprintf(error_msg, sizeof(error_msg), "failed to get recreated mirror destination information, destination id %d", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_get(unit, dest_id, &dest), error_msg);
        /*
         * check that destinations are allocated in order. This is Arad implementation specific 
         */
        if (BCM_GPORT_MIRROR_GET(dest.mirror_dest_id) != i)
        {
            printf("Retrieved recreated mirror destination id %d, should have been %d\n",
                   BCM_GPORT_MIRROR_GET(dest.mirror_dest_id), i);
            return BCM_E_FAIL;
        }

        bcm_gport_t expected = mirr_dests_expected[max_dest_id - i];
        if (dest.gport != expected)
        {
            printf("In recreated mirror destination %d, retrieved gport 0x%lx, should have been 0x%lx\n",
                   i, dest.gport, expected);
            return BCM_E_FAIL;
        }
        snprintf(error_msg, sizeof(error_msg), "Failed to destroy recreated mirror destination with ID %d", i);
        BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_destroy(unit, dest_id), error_msg);
    }

    /*
     * test Jericho2 mirroring attributes
     */
    unsigned char prio = sal_rand() % 8, ecn_value = 1, cnm_cancel = 1;
    bcm_color_t color = bcmColorYellow;
    unsigned int trunk_hash_result = sal_rand() % 65536;
    unsigned short vsq = 1;

    bcm_mirror_destination_t_init(&dest);
    dest.flags = 0;
    dest.gport = mirr_dests[0];
    dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_COLOR
        | BCM_MIRROR_PKT_HEADER_UPDATE_PRIO
        | BCM_MIRROR_PKT_HEADER_UPDATE_ECN_VALUE
        | BCM_MIRROR_PKT_HEADER_UPDATE_ECN_CNM_CANCEL
        | BCM_MIRROR_PKT_HEADER_UPDATE_TRUNK_HASH_RESULT | BCM_MIRROR_PKT_HEADER_UPDATE_VSQ;
    dest.packet_control_updates.cnm_cancel = cnm_cancel;
    dest.packet_control_updates.color = color;
    dest.packet_control_updates.prio = prio;
    dest.packet_control_updates.vsq = vsq;
    dest.packet_control_updates.ecn_value = ecn_value;
    dest.packet_control_updates.trunk_hash_result = trunk_hash_result;

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_create(unit, &dest), "");

    dest_id = BCM_GPORT_MIRROR_GET(dest.mirror_dest_id);
    bcm_mirror_destination_t_init(&dest);
    BCM_GPORT_MIRROR_SET(dest_id, dest_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_get(unit, dest_id, &dest), "");

    if (dest.packet_control_updates.color != color)
        return BCM_E_FAIL;
    if (dest.packet_control_updates.prio != prio)
        return BCM_E_FAIL;
    if (dest.packet_control_updates.vsq != vsq)
        return BCM_E_FAIL;
    if (dest.packet_control_updates.trunk_hash_result != trunk_hash_result)
        return BCM_E_FAIL;

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_destroy(unit, dest_id), "");

    printf("==> Test finished successfully\n");
    /*
     * return with no existing mirror destinations 
     */
    return BCM_E_NONE;
}

/* This function tests all the Arad inbound & outbound mirror port and port+vlan apis:
 * bcm_mirror_port_dest_add
 * bcm_mirror_port_dest_delete
 * bcm_mirror_port_dest_delete_all
 * bcm_mirror_port_dest_get
 * bcm_mirror_port_vlan_dest_add
 * bcm_mirror_port_vlan_dest_delete
 * bcm_mirror_port_vlan_dest_delete_all
 * bcm_mirror_port_vlan_dest_get
 *
 * In arad only one destination may be added to a port and to a port+vlan.
 */
int
sand_mirror_ports_arad_test_1(
    int unit)
{
    int rv, i;
    int found_ports_num, found_ports[8];
    bcm_gport_t mirr_dest1, mirr_dest2;
    bcm_mirror_destination_t dest = { 0 };
    bcm_gport_t port1, port2, port3, out_gport;
    unsigned int u_out_count;
    int out_count;
    bcm_vlan_t vlan1 = 100, vlan2 = 999, vlan3 = 432, rcy_mirr_port = 150;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;
    char error_msg[200]={0,};

    BCM_IF_ERROR_RETURN_MSG(sand_get_n_ports(unit, 8, found_ports, &found_ports_num), "failed to get ports for the test");
    if (found_ports_num < 4)
    {
        printf("found only %d ports for the test which is not enough\n", found_ports_num);
        return BCM_E_FAIL;
    }

    BCM_GPORT_LOCAL_SET(port1, found_ports[0]);
    BCM_GPORT_SYSTEM_PORT_ID_SET(port2, found_ports[2]);
    if (bcm_stk_sysport_gport_get(unit, port2, &port2) != BCM_E_NONE)
    {
        printf("Failed to get gport\n");
        return BCM_E_FAIL;
    }

    BCM_GPORT_LOCAL_SET(port3, found_ports[3]);

    BCM_IF_ERROR_RETURN_MSG(sand_delete_all_mirror_destinations(unit), "");
    /*
     * remove previous mirrors on the ports & vlans we use, if they exist. The APIs will gail if these were not added 
     */
    bcm_mirror_port_dest_delete_all(unit, port1, BCM_MIRROR_PORT_INGRESS);
    bcm_mirror_port_dest_delete_all(unit, port2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS);
    bcm_mirror_port_dest_delete_all(unit, port3, BCM_MIRROR_PORT_EGRESS);
    bcm_mirror_port_vlan_dest_delete_all(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS);
    bcm_mirror_port_vlan_dest_delete_all(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS);
    bcm_mirror_port_vlan_dest_delete_all(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS);
    printf
        ("==> Above (up to 6) failures of bcm_mirror_port_dest_delete_all and bcm_mirror_port_vlan_dest_delete_all are expected\n");

    /*
     * create two mirror destinations 
     */
    BCM_GPORT_LOCAL_SET(dest.gport, found_ports[1]);
    dest.flags = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_create(unit, &dest), "failed to create mirror destination 1");
    mirr_dest1 = dest.mirror_dest_id;

    BCM_GPORT_LOCAL_SET(dest.gport, found_ports[found_ports_num > 4 ? 4 : 2]);
    dest.flags = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_create(unit, &dest), "failed to create mirror destination 2");
    mirr_dest2 = dest.mirror_dest_id;

    /*
     * mirror inbound/outbound ports and ports+vlans to destinations 
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_add(unit, port1, BCM_MIRROR_PORT_INGRESS, mirr_dest1),
        "failed to add inbound port1 to be mirrored to mirr_dest1");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_add(unit, port2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest2),
        "failed to add inbound+outbound port2 to be mirrored to mirr_dest2");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_add(unit, port3, BCM_MIRROR_PORT_EGRESS, mirr_dest1),
        "failed to add outbound port2 to be mirrored to mirr_dest1");

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_add(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, mirr_dest2),
        "failed to add inbound port1+vlan1 to be mirrored to mirr_dest2");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_add(unit, port2, vlan2,
                                           BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest1),
                                           "failed to add inbound+outbound port1+vlan1 to be mirrored to mirr_dest1");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_add(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, mirr_dest2),
        "failed to add outbound port1+vlan1 to be mirrored to mirr_dest2");

    /*
     * Get and verify the information that we added 
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count),
        "failed to get inbound port1 mirror destination");
    if (out_count != 1)
    {
        printf("Output destination count of inbound port1 was %d instead of 1\n", out_count);
        return BCM_E_FAIL;
    }
    else if (out_gport != mirr_dest1)
    {
        printf("mirror destination of inbound port1 was %lx and not the expected %lx\n", out_gport, mirr_dest1);
        return BCM_E_FAIL;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count),
        "failed to get inbound port2 mirror destination");
    if (out_count != 1)
    {
        printf("Output destination count of inbound port2 was %d instead of 1\n", out_count);
        return BCM_E_FAIL;
    }
    else if (out_gport != mirr_dest2)
    {
        printf("mirror destination of inbound port2 was %lx and not the expected %lx\n", out_gport, mirr_dest2);
        return BCM_E_FAIL;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count),
        "failed to get outbound port2 mirror destination");
    if (out_count != 1)
    {
        printf("Output destination count of outbound port2 was %d instead of 1\n", out_count);
        return BCM_E_FAIL;
    }
    else if (out_gport != mirr_dest2)
    {
        printf("mirror destination of outbound port2 was %lx and not the expected %lx\n", out_gport, mirr_dest2);
        return BCM_E_FAIL;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count),
        "failed to get outbound port3 mirror destination");
    if (out_count != 1)
    {
        printf("Output destination count of outbound port2 was %d instead of 1\n", out_count);
        return BCM_E_FAIL;
    }
    else if (out_gport != mirr_dest1)
    {
        printf("mirror destination of outbound port3 was %lx and not the expected %lx\n", out_gport, mirr_dest1);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count),
        "failed to get inbound port1+vlan1 mirror destination");
    if (u_out_count != 1)
    {
        printf("Output destination count of inbound port1+vlan1 was %d instead of 1\n", u_out_count);
        return BCM_E_FAIL;
    }
    else if (out_gport != mirr_dest2)
    {
        printf("mirror destination of inbound port1+vlan1 was %lx and not the expected %lx\n", out_gport, mirr_dest2);
        return BCM_E_FAIL;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count),
        "failed to get inbound port2+vlan2 mirror destination");
    if (u_out_count != 1)
    {
        printf("Output destination count of inbound port2+vlan2 was %d instead of 1\n", u_out_count);
        return BCM_E_FAIL;
    }
    else if (out_gport != mirr_dest1)
    {
        printf("mirror destination of inbound port2+vlan2 was %lx and not the expected %lx\n", out_gport, mirr_dest1);
        return BCM_E_FAIL;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count),
        "failed to get outbound port2+vlan2 mirror destination");
    if (u_out_count != 1)
    {
        printf("Output destination count of outbound port2+vlan2 was %d instead of 1\n", u_out_count);
        return BCM_E_FAIL;
    }
    else if (out_gport != mirr_dest1)
    {
        printf("mirror destination of outbound port2+vlan2 was %lx and not the expected %lx\n", out_gport, mirr_dest1);
        return BCM_E_FAIL;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count),
        "failed to get outbound port3+vlan3 mirror destination");
    if (u_out_count != 1)
    {
        printf("Output destination count of outbound port3+vlan3 was %d instead of 1\n", u_out_count);
        return BCM_E_FAIL;
    }
    else if (out_gport != mirr_dest2)
    {
        printf("mirror destination of outbound port3+vlan3 was %lx and not the expected %lx\n", out_gport, mirr_dest2);
        return BCM_E_FAIL;
    }

    /*
     * test deletions for a specific destination 
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_delete(unit, port1, BCM_MIRROR_PORT_INGRESS, mirr_dest1),
        "failed to delete inbound port1 mirror to mirr_dest1");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_delete(unit, port2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest2),
        "failed to delete inbound+outbound port2 mirror to mirr_dest2");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_delete(unit, port3, BCM_MIRROR_PORT_EGRESS, mirr_dest1),
        "failed to delete outbound port3 mirror to mirr_dest1");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_delete(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, mirr_dest2),
        "failed to delete inbound port1+vlan1 mirror to mirr_dest2");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_delete(unit, port2, vlan2,
                                              BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest1),
                                              "failed to delete inbound+outbound port2+vlan2 mirror to mirr_dest1");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_delete(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, mirr_dest2),
        "failed to delete outbound port3+vlan3 mirror to mirr_dest2");

    /*
     * Verify that they do not exist 
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count),
        "Getting inbound port1 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting inbound port1 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count),
        "Getting inbound port2 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting inbound port2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count),
        "Getting outbound port2 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting outbound port2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count),
        "Getting outbound port3 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting outbound port3 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count),
        "Getting inbound port1+vlan1 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting inbound port1+vlan1 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count),
        "Getting inbound port2+vlan2 mirror destination return value");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting inbound port2+vlan2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count),
        "Getting outbound port2+vlan2 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting outbound port2+vlan2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count),
        "Getting outbound port3+vlan3 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting outbound port3+vlan3 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    /*
     * mirror inbound/outbound ports and ports+vlans to destinations again so that they can be deleted again 
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_add(unit, port1, BCM_MIRROR_PORT_INGRESS, mirr_dest1),
        "failed to add inbound port1 to be mirrored to mirr_dest1");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_add(unit, port2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest2),
        "failed to add inbound+outbound port2 to be mirrored to mirr_dest2");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_add(unit, port3, BCM_MIRROR_PORT_EGRESS, mirr_dest1),
        "failed to add outbound port2 to be mirrored to mirr_dest1");

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_add(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, mirr_dest2),
        "failed to add inbound port1+vlan1 to be mirrored to mirr_dest2");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_add(unit, port2, vlan2,
                                           BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest1),
                                           "failed to add inbound+outbound port1+vlan1 to be mirrored to mirr_dest1");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_add(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, mirr_dest2),
        "failed to add outbound port1+vlan1 to be mirrored to mirr_dest2");

    /*
     * test deletions for all destination 
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_delete_all(unit, port1, BCM_MIRROR_PORT_INGRESS),
        "failed to delete inbound port1 mirror");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_delete_all(unit, port2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS),
        "failed to delete inbound port2 mirror");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_delete_all(unit, port3, BCM_MIRROR_PORT_EGRESS),
        "failed to delete inbound port3 mirror");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_delete_all(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS),
        "failed to delete inbound port1+vlan1 mirror");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_delete_all(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS),
        "failed to delete inbound port2+vlan2 mirror");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_delete_all(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS),
        "failed to delete inbound port3+vlan3 mirror");

    /*
     * Verify that they do not exist 
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count),
        "Getting inbound port1 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting inbound port1 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count),
        "Getting inbound port2 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting inbound port2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count),
        "Getting outbound port2 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting outbound port2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_dest_get(unit, port3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count),
        "Getting outbound port3 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting outbound port3 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count),
        "Getting inbound port1+vlan1 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting inbound port1+vlan1 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count),
        "Getting inbound port2+vlan2 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting inbound port2+vlan2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count),
        "Getting outbound port2+vlan2 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting outbound port2+vlan2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_vlan_dest_get(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count),
        "Getting outbound port3+vlan3 mirror destination");

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE)
    {
        printf
            ("Getting outbound port3+vlan3 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n",
             out_gport);
        return BCM_E_FAIL;
    }

    /*
     * cleanup - delete the created mirror destinations 
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_destroy(unit, mirr_dest1), "could not destroy mirr_dest1");
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_destroy(unit, mirr_dest2), "could not destroy mirr_dest2");

    /*
     *  test reserving mirroring resources (reassembly context and recycle channel) for ports
     *  different sequence for Jericho1 and Jericho2 */
    int pp_tm_context_en = *(dnxc_data_get(unit, "ingr_reassembly", "context", "pp_tm_context", NULL));
    if (!pp_tm_context_en)
    {
        int start_channel = 20; /* first channel to start adding mirror recycle ports */
        for (i = 0; i < found_ports_num; ++i)
        {
            /** using local and system port gport alternatively */
            if (i & 1)
            {
                BCM_GPORT_SYSTEM_PORT_ID_SET(port1, found_ports[i]);
                if ((rv = bcm_stk_sysport_gport_get(unit, port1, &port1)) != BCM_E_NONE)
                {
                    printf("Failed to get system port\n");
                    return BCM_E_FAIL;
                }
            }
            else
            {
                BCM_GPORT_LOCAL_SET(port1, found_ports[i]);
            }

            /** making sure that mirroring resources for port are not initially reserved */
            rcy_map_info.rcy_mirror_port = BCM_PORT_INVALID;
            bcm_mirror_port_to_rcy_port_map_set(unit, 0, port1, &rcy_map_info); /* do not check failure */
            BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_to_rcy_port_map_get(unit, 0, port1, &rcy_map_info), "");
            if (rcy_map_info.rcy_mirror_port != BCM_PORT_INVALID)
            {
                printf("failed making sure that mirroring resources for port %d are not initially reserved\n",
                       found_ports[i]);
                return BCM_E_FAIL;
            }

            /** reserving mirroring resources for port */
            snprintf(error_msg, sizeof(error_msg), "failed reserving mirroring resources for port %d", found_ports[i]);
            BCM_IF_ERROR_RETURN_MSG(sand_mirror_port_to_rcy_port_map(unit, port1, TRUE,
                                              rcy_mirr_port + i, rcy_mirr_port + i, start_channel + i),
                                              error_msg);

            /** making sure that mirroring resources for port are reserved */
            BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_to_rcy_port_map_get(unit, 0, port1, &rcy_map_info), "");
            if (rcy_map_info.rcy_mirror_port == BCM_PORT_INVALID)
            {
                printf("failed making sure that mirroring resources for port %d are reserved\n", found_ports[i]);
                return BCM_E_FAIL;
            }

            /** un-reserving mirroring resources for port */
            rcy_map_info.rcy_mirror_port = BCM_PORT_INVALID;
            snprintf(error_msg, sizeof(error_msg), "failed un-reserving mirroring resources for port %d", found_ports[i]);
            BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_to_rcy_port_map_set(unit, 0, port1, &rcy_map_info), error_msg);

            /** making sure that mirroring resources for port were un-reserved */
            BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_to_rcy_port_map_get(unit, 0, port1, &rcy_map_info), "");
            if (rcy_map_info.rcy_mirror_port != BCM_PORT_INVALID)
            {
                printf("failed making sure that mirroring resources for port %d were un-reserved\n", found_ports[i]);
                return BCM_E_FAIL;
            }

            /** reserving mirroring resources for port again */
            snprintf(error_msg, sizeof(error_msg), "failed reserving (again) mirroring resources for port %d", found_ports[i]);
            BCM_IF_ERROR_RETURN_MSG(sand_mirror_port_to_rcy_port_map(unit, port1, FALSE,
                                                rcy_mirr_port + i, rcy_mirr_port + i, start_channel+ i), error_msg);

            /** making sure that mirroring resources for port are reserved again */
            BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_to_rcy_port_map_get(unit, 0, port1, &rcy_map_info), "");
            if (rcy_map_info.rcy_mirror_port == BCM_PORT_INVALID)
            {
                printf("failed making sure that mirroring resources for port %d are reserved\n", found_ports[i]);
                return BCM_E_FAIL;
            }

            /** un-reserving mirroring resources for port again */
            rcy_map_info.rcy_mirror_port = BCM_PORT_INVALID;
            snprintf(error_msg, sizeof(error_msg), "failed un-reserving (again) mirroring resources for port %d", found_ports[i]);
            BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_to_rcy_port_map_set(unit, 0, port1, &rcy_map_info), error_msg);

            /** making sure that mirroring resources for port were un-reserved again */
            BCM_IF_ERROR_RETURN_MSG(bcm_mirror_port_to_rcy_port_map_get(unit, 0, port1, &rcy_map_info), "");
            if (rcy_map_info.rcy_mirror_port != BCM_PORT_INVALID)
            {
                printf("failed making sure (again) that mirroring resources for port %d were un-reserved\n",
                       found_ports[i]);
                return BCM_E_FAIL;
            }

            /** remove the created mirror rcy port */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_remove(unit, rcy_mirr_port + i, 0), "");
        }
    }

    return BCM_E_NONE;
}

/**
 *
 * @author sinai (04/05/2014)
 *
 * @param unit
 * @param seed
 *
 * @return int
 */
int sand_mirror_destinations_arad_test_rspan(int unit, int seed) {
    bcm_mirror_destination_t dest = {0};
    int found_ports_num, found_ports[4];
    int intf_id;
    bcm_gport_t tunnel_gport;
    int rv;
    bcm_tunnel_initiator_t rspan_tunnel;
    bcm_mac_t dummy_mac_address  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int span_id;
    int mirror_dest_id;
    int interf;
    int mirror_gport, tunnel_id;
    int new_vlan;
    bcm_l3_intf_t intf;
    int tpid, pkt_pri;
    int station_id;
    bcm_l2_station_t station;
    l3_ingress_intf ingress_itf;
    char error_msg[200]={0,};

    sal_srand(seed);
    printf("Randomizing with seed %d\n",seed);

    BCM_IF_ERROR_RETURN_MSG(sand_get_n_ports(unit, 4, found_ports, &found_ports_num), "failed to get ports for the test");
    if (found_ports_num < 2) {
        printf("found only %d ports for the test which is not enough\n", found_ports_num);
        return BCM_E_FAIL;
    }

    /* open in_vlan and attach in_port to it */
    int in_vlan = sal_rand() % 4095 +1;
    rv = in_port_intf_set(unit, found_ports[0], in_vlan);
    if (rv == BCM_E_EXISTS) {
        rv = BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN_MSG(rv, "intf_out");

    rv = out_port_set(unit, found_ports[1]);
    if (rv == BCM_E_EXISTS) {
        rv = BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN_MSG(rv, "intf_out");

    new_vlan = sal_rand() % 4095 +1;
    rv = out_port_set(unit, found_ports[3]);
    if (rv == BCM_E_EXISTS) {
        rv = BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN_MSG(rv, "intf_out");

    if (!is_device_or_above(unit, JERICHO2)) {
        bcm_l2_station_t_init(&station);
        sal_memcpy(station.dst_mac, dummy_mac_address, 6);
        sal_memset(station.dst_mac_mask, 0xff, 6);
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_station_add(unit, &station_id, &station), "");
    }

    rv = intf_eth_rif_create(unit, new_vlan, dummy_mac_address);
    if (rv == BCM_E_EXISTS) {
        rv = BCM_E_NONE;
    }
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", new_vlan);
    BCM_IF_ERROR_RETURN_MSG(rv, error_msg);

    ingress_itf.vrf = new_vlan;
    ingress_itf.intf_id = new_vlan;
    rv = intf_ingress_rif_set(unit, &ingress_itf);
    if (rv == BCM_E_EXISTS) {
        rv = BCM_E_NONE;
    }
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", vlan);
    BCM_IF_ERROR_RETURN_MSG(rv, error_msg);

    if (!is_device_or_above(unit, JERICHO2)) {
        bcm_tunnel_initiator_t_init(&rspan_tunnel);

        rspan_tunnel.type       = bcmTunnelTypeRspan;
        tpid = sal_rand() % 0xff;
        pkt_pri = sal_rand() % 0x7;
        rspan_tunnel.vlan = new_vlan;
        rspan_tunnel.tpid = tpid;
        rspan_tunnel.pkt_pri = pkt_pri;

        /* set tunnel_id to control which outlif to set the span tunnel
           BCM_GPORT_TUNNEL_ID_SET(rspan_tunnel.tunnel_id,0x5000);
         */
        BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &rspan_tunnel), "failed to open RIF");

        tunnel_id = rspan_tunnel.tunnel_id;

        /*
         * if mirror destination is Unicast, set encap-id
         */
        dest.encap_id = BCM_GPORT_TUNNEL_ID_GET(rspan_tunnel.tunnel_id);
     }

     dest.flags = BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;

     /* 2. set Mirror destination */
     dest.gport = found_ports[2]; /* dest port*/

     /* create the destination */
     BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_create(unit, &dest), "failed to create mirror destination 1");
     mirror_dest_id = dest.mirror_dest_id;

     if (!is_device_or_above(unit, JERICHO2)) {
         bcm_tunnel_initiator_t_init(&rspan_tunnel);
         bcm_l3_intf_t_init(&intf);
         /* span info */
         intf.l3a_tunnel_idx = dest.encap_id;
         BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_get(unit,&intf,&rspan_tunnel), "");

         if (rspan_tunnel.type != bcmTunnelTypeRspan) {
             printf("bcm_tunnel_initiator_get did not return correct type\n");
             return -1;
         }

         if (rspan_tunnel.vlan != new_vlan) {
             printf("bcm_tunnel_initiator_get did not return correct vlan. Got: %d, expected %d\n", rspan_tunnel.vlan ,new_vlan);
             return -1;
         }

         if (rspan_tunnel.tpid != tpid) {
             printf("bcm_tunnel_initiator_get did not return correct tpid. Got: %d, expected %d\n", rspan_tunnel.tpid, tpid);
             return -1;
         }

         if (rspan_tunnel.pkt_pri != pkt_pri) {
             printf("bcm_tunnel_initiator_get did not return correct pkt_pri. Got: %d, expected %d\n", rspan_tunnel.pkt_pri, pkt_pri);
             return -1;
         }

         /* 3. Update the tunnel, mirror*/
         bcm_tunnel_initiator_t_init(&rspan_tunnel);
         bcm_l3_intf_t_init(&intf);
         rspan_tunnel.type       = bcmTunnelTypeRspan;
         rspan_tunnel.flags = BCM_TUNNEL_REPLACE | BCM_TUNNEL_WITH_ID;
         rspan_tunnel.tunnel_id = tunnel_id;
         tpid = sal_rand() % 0xff;
         pkt_pri = sal_rand() % 0xe;
         rspan_tunnel.vlan = new_vlan;
         rspan_tunnel.tpid = tpid;
         rspan_tunnel.pkt_pri = pkt_pri;

         /* Update the tunnel*/
         BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &rspan_tunnel), "");

         dest.encap_id = BCM_GPORT_TUNNEL_ID_GET(rspan_tunnel.tunnel_id);
    }

    /* Now the mirror*/
    dest.flags = BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID | BCM_MIRROR_DEST_REPLACE | BCM_MIRROR_DEST_WITH_ID;
    mirror_gport = dest.gport;
    BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, mirror_dest_id);
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_create(unit, &dest), "failed to create mirror destination 1");

    /* Delete the mirror*/
    BCM_IF_ERROR_RETURN_MSG(bcm_mirror_destination_destroy(unit, mirror_dest_id), "");

    if (!is_device_or_above(unit, JERICHO2)) {
        intf.l3a_tunnel_idx =  rspan_tunnel.tunnel_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_clear(unit,&intf), "could not destroy mirror");

        /* Create teh tunnel again to verify that it was correctly freed*/
        bcm_tunnel_initiator_t_init(&rspan_tunnel);
        bcm_l3_intf_t_init(&intf);
        /* span info */
        rspan_tunnel.type       = bcmTunnelTypeRspan;
        rspan_tunnel.flags = BCM_TUNNEL_WITH_ID;
        rspan_tunnel.tunnel_id = tunnel_id;
        rspan_tunnel.vlan = new_vlan;
        rspan_tunnel.tpid = tpid;
        rspan_tunnel.pkt_pri = pkt_pri;

        BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &rspan_tunnel), "");

        /* clear and go home!*/
        intf.l3a_tunnel_idx = rspan_tunnel.tunnel_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_clear(unit,&intf), "could not destroy mirror");
    }

    return BCM_E_NONE;
}

