/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_mirror_tests.c
 * Purpose: Examples of Mirror tests and usage
 */

/*
 * Usage:
 *
 * BCM> cint examples/dpp/utility/cint_utils_l3.c
 * BCM> cint examples/dpp/utility/cint_mirror_tests.c
 */

  /* Enum signifiyng the device type. Should be ordered oldest device to newest*/
 enum device_type_t { 
      device_type_arad_a0=0,
      device_type_arad_plus=1,
      device_type_jericho=2,
      device_type_qax=3
  } ;

int get_device_type(int unit, device_type_t *device_type)
{
  bcm_info_t info;

  int rv = bcm_info_get(unit, &info);
  if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
  }

  
  *device_type = (info.device == 0x8650 && info.revision == 0) ? device_type_arad_a0 :
                            (info.device == 0x8660)? device_type_arad_plus :
                            ((info.device == 0x8375) || (info.device == 0x8675)) ? device_type_jericho:
                            ((info.device == 0x8470) || (info.device == 0x8270)) ? device_type_qax :            
                            -1;
  return rv;
}




/* Mirroring destinations traverse callback, returns non zero value on error
 *
 *   The Mirror destination traverse callback might be used to traverse all
 *   created mirror destinations and add them to an array.
 */
int bcm_mirror_destination_list_id(
    int unit, 
    bcm_mirror_destination_t *mirror_dest, /* we get this information from the traverse function */
    void *user_data) { /* user data, in our case bcm_gport_t** - a ponter to the pointer with the current location in the output array */
    bcm_gport_t** cur_pointer = user_data;
    if (!mirror_dest || !cur_pointer) {
        printf("Callback received a null pointer\n");
        return BCM_E_PARAM;
    }
    **cur_pointer = mirror_dest->mirror_dest_id;
    /* printf("callback handling destination %u\n", BCM_GPORT_MIRROR_GET(**cur_pointer)); */
    ++*cur_pointer;
    return BCM_E_NONE;
}

/* This function just wraps bcm_mirror_destination_traverse.
 * It is needed due to a cint implementation : When returning from bcm_mirror_destination_traverse,
 * the calling function also exists immediately, if the callback was called at least once.
 */
int bcm_mirror_destination_traverse_wrap(int unit,
                                         bcm_mirror_destination_traverse_cb cb,
                                         void *user_data) {
  int res;
  res = bcm_mirror_destination_traverse(unit, cb, user_data);
  /* this code is not reached in case a callback was called, due to a cint issue */
  return res;
}

/* Delete all existing mirror destinations */
int delete_all_mirror_destinations(int unit) {
    int rv, i, size;
    bcm_gport_t mirr_dest_ids[1024]; /* This should be big enouph to hold all possible destinations, which is 15 in Arad */
    bcm_gport_t* mirr_dest_ids_current_location = mirr_dest_ids;

    /* Get the created destinations by traversing over the destinations.
       This could have been done by attempting bcm_mirror_destination_traverse on all possible destinations */
    if (rv = bcm_mirror_destination_traverse_wrap(unit, bcm_mirror_destination_list_id, &mirr_dest_ids_current_location)) {
        printf("could not delete all mirror destinations - traversal failed with value %d\n", rv);
        return rv;
    }
    size = mirr_dest_ids_current_location - mirr_dest_ids; /* number of destinations traversed */
 
    /* Destroy each found destination */
    for (i = 0; i < size; ++i) {
        if (rv = bcm_mirror_destination_destroy(unit, mirr_dest_ids[i])) {
            printf("could not delete all mirror destinations - failed to destroy mirror destination %u, return value %d\n",
                   BCM_GPORT_MIRROR_GET(mirr_dest_ids[i]), rv);
            return rv;
        }
    }
    return BCM_E_NONE;
}

/* get the (sorted) ids of up to port_num ports, return 0 on success */
int get_n_ports(int unit, int port_num, int *out_ports, int *out_port_num) {
    bcm_error_t rv;
    bcm_port_config_t c;
    bcm_port_t p;

    if (!out_ports || !out_port_num || port_num <= 0) {
        printf("Error, illegal arguments\n");
        return BCM_E_PARAM;
    }
    rv = bcm_port_config_get(unit, &c);
    if (rv != BCM_E_NONE) {
        printf("Error, failed to get the ports of unit %d bcm_port_config_get\n", unit);
        return rv;
    }
    *out_port_num = 0;
    BCM_PBMP_ITER(c.all, p) {
        if (p >= 0 && p < 512) {
            out_ports[*out_port_num] = p;
            ++*out_port_num;
            if (*out_port_num >= port_num) { break; }
        }
    }
    return 0;
}

/* get the (sorted) ids of up to port_num ethernet ports, return 0 on success */
int get_n_ethernet_ports(int unit, int port_num, int *out_ports, int *out_port_num) {
    bcm_error_t rv;
    bcm_port_config_t c;
    bcm_port_t p;

    if (!out_ports || !out_port_num || port_num <= 0) {
        printf("Error, illegal arguments\n");
        return BCM_E_PARAM;
    }
    rv = bcm_port_config_get(unit, &c);
    if (rv != BCM_E_NONE) {
        printf("Error, failed to get the ports of unit %d bcm_port_config_get\n", unit);
        return rv;
    }
    *out_port_num = 0;
    BCM_PBMP_ITER(c.e, p) {
        if (p > 0 && p < 512) {
            out_ports[*out_port_num] = p;
            ++*out_port_num;
            if (*out_port_num >= port_num) { break; }
        }
    }
    return 0;
}

int SOC_IS_JERICHO = 1;
int SOC_IS_ARAD = 0;

/* This function tests all the mirror destination handling APIs (bcm_mirror_destination_*).
 * It assumes the device is Arad, in the mirror IDs available.
 */
int mirror_destinations_arad_test_1(int unit, int device_type) {
    int rv, i, size, modid;
    int min_dest_id = 1, max_dest_id = 15;
    int found_ports_num, found_ports[8];
    bcm_gport_t dest_id;
    bcm_mirror_destination_t dest = {0};
    bcm_gport_t mirr_dest_ids[max_dest_id - min_dest_id + 1]; /* will contain gport represantation of mirror destinations */
    int dest_ids[max_dest_id - min_dest_id + 1]; /* will contain mirror destination IDs for processing traversal output */
    bcm_gport_t mirr_dests[max_dest_id - min_dest_id + 1];
    bcm_gport_t* mirr_dest_ids_current_location = mirr_dest_ids;
    bcm_multicast_t mc_id1 = 4096;
    bcm_multicast_t mc_id2 = 8123;
    device_type_t dev_type;
    get_device_type(unit, &dev_type);
    int default_nof_mc_groups = (dev_type ==  device_type_qax ? (16*1024) : (64*1024));
    int nof_mc_groups = soc_property_get(unit, "multicast_egress_group_id_range_max", default_nof_mc_groups);
    bcm_multicast_t mc_id3 = 64000 % nof_mc_groups;
    bcm_multicast_t mc_id4 = 23123 % nof_mc_groups;
    bcm_multicast_t mc_id5 = 35777 % nof_mc_groups;
    bcm_multicast_t mc_id6 = 5000;
    int prob_shift=(1 << SOC_DPP_IMP_DEFS_GET_MIRROR_PROBABILITY_BITS(unit));
    int core, tm_port;

 
    rv = get_n_ports(unit, 8, found_ports, &found_ports_num);
    if (rv) {
        printf("failed to get ports for the test\n");
        return rv;
    } else if (found_ports_num < 4) {
        printf("found only %d ports for the test which is not enough\n", found_ports_num);
        return BCM_E_FAIL;
    }

    rv = bcm_stk_modid_get(unit, &modid);
    if (rv) {
        printf("failed to get modid\n");
        return rv;
    }

    /* Get the created destinations by traversing over the destinations.
       This could have been done by attempting bcm_mirror_destination_traverse on all possible destinations */
    rv = bcm_mirror_destination_traverse_wrap(unit, bcm_mirror_destination_list_id, &mirr_dest_ids_current_location);
    if (rv) {
        printf("bcm_mirror_destination_traverse failed with value %d\n", rv);
        return rv;
    }
    size = mirr_dest_ids_current_location - mirr_dest_ids; /* number of destinations traversed */
 
    /* Destroy each found destination */
    for (i = 0; i < size; ++i) {
        printf("destroying previously created mirror destination %u\n", BCM_GPORT_MIRROR_GET(mirr_dest_ids[i]));
        if (rv = bcm_mirror_destination_destroy(unit, mirr_dest_ids[i])) {
            printf("initial bcm_mirror_destination_destroy failed with value %d\n", rv);
            return rv;
        }
    }

    BCM_GPORT_LOCAL_SET(mirr_dests[0], found_ports[1]);
    BCM_GPORT_LOCAL_SET(mirr_dests[1], found_ports[3]);
    BCM_GPORT_LOCAL_SET(mirr_dests[2], found_ports[2]);
    BCM_GPORT_MCAST_SET(mirr_dests[3], mc_id1);
    BCM_GPORT_MCAST_SET(mirr_dests[4], mc_id2);
    BCM_GPORT_MCAST_SET(mirr_dests[5], mc_id3);
    BCM_GPORT_MCAST_SET(mirr_dests[6], mc_id4);
    BCM_GPORT_MCAST_SET(mirr_dests[7], mc_id5);
    BCM_GPORT_MCAST_SET(mirr_dests[8], mc_id6);
    BCM_GPORT_MCAST_SET(mirr_dests[9], mc_id2);
    BCM_GPORT_MCAST_SET(mirr_dests[10], mc_id1);
    BCM_GPORT_LOCAL_SET(mirr_dests[11], mirr_dests[0]);
    BCM_GPORT_LOCAL_SET(mirr_dests[12], found_ports[1]);

    rv = get_core_and_tm_port_from_port(unit, found_ports[2], &core, &tm_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in get_core_and_tm_port_from_port\n");
        return rv;
    }
    BCM_GPORT_MODPORT_SET(mirr_dests[13], modid + core, tm_port);
    BCM_GPORT_LOCAL_SET(mirr_dests[14], found_ports[3]);
    /* create destinations without selected the destination id */

    for (i = min_dest_id ; i <= max_dest_id; ++i) { /* init dest_ids to 0 */
        dest_ids[i - min_dest_id] = -1;
    }
    for (i = min_dest_id ; i <= max_dest_id; ++i) {
        bcm_mirror_destination_t_init(&dest);
        dest.gport = mirr_dests[i - min_dest_id];
        dest.flags = 0;
        dest.sample_rate_dividend = (prob_shift * i) / max_dest_id;
        dest.sample_rate_divisor = prob_shift;
        if (rv = bcm_mirror_destination_create(unit, &dest)) {
            printf("failed to create mirror destination that should have been number %d, return value %d\n", i, rv);
            return rv;
        }
        /* check that destinations are not allocated twice, and store mapping to the creation loop index */
        unsigned id = BCM_GPORT_MIRROR_GET(dest.mirror_dest_id);
        if (id < min_dest_id || id > max_dest_id ) {
            printf("created mirror destination ID %u, is out of range\n", id);
            return BCM_E_FAIL;
        } else if (dest_ids[id - min_dest_id] != -1) {
            printf("mirror destination created destination a second time with ID %u\n", id);
            return BCM_E_FAIL;
        }
        dest_ids[id - min_dest_id] = i;

    }
    /* get destinations information, compare to the expected information, and destroy the destinations */
    for (i = min_dest_id ; i <= max_dest_id; ++i) {
        BCM_GPORT_MIRROR_SET(dest_id, i);
        bcm_mirror_destination_t_init(&dest);
        if (rv = bcm_mirror_destination_get(unit, dest_id, &dest)) {
            printf("failed to get mirror destination information, destination id %d, return value %d\n", i, rv);
            return rv;
        }
        /* check that destinations are allocated in order. This is Arad implementation specific */
        if (BCM_GPORT_MIRROR_GET(dest.mirror_dest_id) != i) {
            printf("Retrieved mirror destination id %d, should have been %d\n", BCM_GPORT_MIRROR_GET(dest.mirror_dest_id), i);
            return BCM_E_FAIL;
        }
        bcm_gport_t expected = mirr_dests[dest_ids[i - min_dest_id] - min_dest_id];

        if (BCM_GPORT_IS_LOCAL(expected)) {
            rv = get_core_and_tm_port_from_port(unit, expected, &core, &tm_port);
            if (rv != BCM_E_NONE) {
                printf("Error, in get_core_and_tm_port_from_port\n");
                return rv;
            }

            BCM_GPORT_MODPORT_SET(expected, modid + core, tm_port);

        }

        if (dest.gport != expected) {
            printf("In mirror destination %d, retrieved gport 0x%lx, should have been 0x%lx\n", i, dest.gport, expected);
            return BCM_E_FAIL;
        }
        if (dest.sample_rate_dividend != (prob_shift * i) / max_dest_id || dest.sample_rate_divisor != prob_shift) {
            printf("Wrong mirror probability in %d:  %u / %u\n", i, dest.sample_rate_dividend, dest.sample_rate_divisor);
            return BCM_E_FAIL;
        }
        dest.sample_rate_dividend = (prob_shift * i) / max_dest_id;
        dest.sample_rate_divisor = prob_shift;
        if (rv = bcm_mirror_destination_destroy(unit, dest_id)) {
            printf("Failed to destroy created mirror destination with ID %d, return value %d\n", i, rv);
            return rv;
        }
    }
    bcm_mirror_destination_t_init(&dest);

    /* traverse and check that no destinations exist */
    mirr_dest_ids_current_location = mirr_dest_ids;
    if (rv = bcm_mirror_destination_traverse_wrap(unit, bcm_mirror_destination_list_id, &mirr_dest_ids_current_location)) {
        printf("Traversal of no destinations failed with value %d\n", rv);
        return rv;
    }
    if (mirr_dest_ids_current_location != mirr_dest_ids) {
        printf("Destinations were found when they should not have. The first one found is %u\n", BCM_GPORT_MIRROR_GET(mirr_dest_ids[0]));
        return BCM_E_FAIL;
    }

    /* create destinations with selected id */
    for (i = max_dest_id ; i >= min_dest_id; --i) {
        dest.gport = mirr_dests[i - min_dest_id];
        dest.flags = BCM_MIRROR_DEST_WITH_ID;
        BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, i);
        if (rv = bcm_mirror_destination_create(unit, &dest)) {
            printf("failed to create mirror destination number %d, return value %d\n", i, rv);
            return rv;
        }
        /* check that destinations are allocated in order. This is Arad implementation specific */
        if (BCM_GPORT_MIRROR_GET(dest.mirror_dest_id) != i) {
            printf("mirror destination created destination number %d, and not the given %d\n", BCM_GPORT_MIRROR_GET(dest.mirror_dest_id), i);
            return BCM_E_FAIL;
        }
    }
    /* get destinations information, compare to the expected information */
    for (i = min_dest_id ; i <= max_dest_id; ++i) {
        BCM_GPORT_MIRROR_SET(dest_id, i);
        if (rv = bcm_mirror_destination_get(unit, dest_id, &dest)) {
            printf("failed to get mirror destination information, destination %d, return value %d\n", i, rv);
            return rv;
        }
        if (BCM_GPORT_MIRROR_GET(dest.mirror_dest_id) != i) {
            printf("Retrieved mirror destination %d, should have been %d\n", BCM_GPORT_MIRROR_GET(dest.mirror_dest_id), i);
            return BCM_E_FAIL;
        }
        bcm_gport_t expected = mirr_dests[i - min_dest_id];
        printf("--i %d, port %d  \n", i, expected );

        if (BCM_GPORT_IS_LOCAL(expected)) {
            rv = get_core_and_tm_port_from_port(unit, expected, &core, &tm_port);
            if (rv != BCM_E_NONE) {
                printf("Error, in get_core_and_tm_port_from_port\n");
                return rv;
            }
            printf("--expect %d, expected 0x%lx, tm_port 0x%lx, rcv 0x%lx \n", i, expected, tm_port, dest.gport );
            BCM_GPORT_MODPORT_SET(expected, modid + core, tm_port);

        }
        if (dest.gport != expected) {
            printf("In mirror destination %d, gport 0x%lx, should have been 0x%lx\n", i, dest.gport, expected);
            return BCM_E_FAIL;
        }
    }

    /* Recreate destinations with selected id, and different information */
    for (i = max_dest_id ; i >= min_dest_id; --i) {
        dest.gport = mirr_dests[max_dest_id - i];
        dest.flags = BCM_MIRROR_DEST_WITH_ID | BCM_MIRROR_DEST_REPLACE;
        BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, i);
        if (rv = bcm_mirror_destination_create(unit, &dest)) {
            printf("failed to recreate mirror destination number %d, return value %d\n", i, rv);
            return rv;
        }
        /* check that destinations are allocated in order. This is Arad implementation specific */
        if (BCM_GPORT_MIRROR_GET(dest.mirror_dest_id) != i) {
            printf("mirror destination recreated destination number %d, and not the given %d\n", BCM_GPORT_MIRROR_GET(dest.mirror_dest_id), i);
            return BCM_E_FAIL;
        }
    }
    /* Get the destinations by traversing over the destinations. */
    mirr_dest_ids_current_location = mirr_dest_ids;
    if (rv = bcm_mirror_destination_traverse_wrap(unit, bcm_mirror_destination_list_id, &mirr_dest_ids_current_location)) {
        printf("Failed traversing all the destinations, return value %d\n", rv);
        return rv;
    }
    size = mirr_dest_ids_current_location - mirr_dest_ids; /* number of destinations traversed */
    if (size != max_dest_id - min_dest_id + 1) {
        printf("Expected to traverse %d destinations, but traversed only %d\n", max_dest_id - min_dest_id + 1, size);
        return BCM_E_FAIL;
    }
    for (i = min_dest_id ; i <= max_dest_id; ++i) { /* init dest_ids to 0 */
        dest_ids[i - min_dest_id] = -1;
    }
    for (i = min_dest_id ; i <= max_dest_id; ++i) { /* test that each destination appears once */
        unsigned id = BCM_GPORT_MIRROR_GET(mirr_dest_ids[i - min_dest_id]);
        if (id < min_dest_id || id > max_dest_id ) {
            printf("mirror destination ID %u retrieved in traverse, is out of range\n", id);
            return BCM_E_FAIL;
        } else if (dest_ids[id - min_dest_id] != -1) {
            printf("mirror destination created destination a second time with ID %u\n", id);
            return BCM_E_FAIL;
        }
        dest_ids[id - min_dest_id] = i;
    }

    /* get destinations information, compare to the expected information, and destroy the destinations */
    for (i = min_dest_id ; i <= max_dest_id; ++i) {
        BCM_GPORT_MIRROR_SET(dest_id, i);
        if (rv = bcm_mirror_destination_get(unit, dest_id, &dest)) {
            printf("failed to get recreated mirror destination information, destination id %d, return value %d\n", i, rv);
            return rv;
        }
        /* check that destinations are allocated in order. This is Arad implementation specific */
        if (BCM_GPORT_MIRROR_GET(dest.mirror_dest_id) != i) {
            printf("Retrieved recreated mirror destination id %d, should have been %d\n", BCM_GPORT_MIRROR_GET(dest.mirror_dest_id), i);
            return BCM_E_FAIL;
        }
        bcm_gport_t expected = mirr_dests[max_dest_id - i];
        if (BCM_GPORT_IS_LOCAL(expected)) {
            rv = get_core_and_tm_port_from_port(unit, expected, &core, &tm_port);
            if (rv != BCM_E_NONE) {
                printf("Error, in get_core_and_tm_port_from_port\n");
                return rv;
            }

            BCM_GPORT_MODPORT_SET(expected, modid + core, tm_port);
        }
        if (dest.gport != expected) {
            printf("In recreated mirror destination %d, retrieved gport 0x%lx, should have been 0x%lx\n", i, dest.gport, expected);
            return BCM_E_FAIL;
        }
        if (rv = bcm_mirror_destination_destroy(unit, dest_id)) {
            printf("Failed to destroy recreated mirror destination with ID %d, return value %d\n", i, rv);
            return rv;
        }
    }

    /*test Jericho new mirroring attributes*/
    if (device_type == SOC_IS_JERICHO) {
        unsigned char prio = sal_rand() % 8, ecn_value = 1, cnm_cancel = 1;
        bcm_color_t color = bcmColorYellow;
        unsigned int trunk_hash_result = sal_rand() % 65536; 
        unsigned short vsq = 1;

        bcm_mirror_destination_t_init(&dest);
        dest.flags = 0;
        dest.gport = mirr_dests[0];
        dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_COLOR | BCM_MIRROR_PKT_HEADER_UPDATE_PRIO | BCM_MIRROR_PKT_HEADER_UPDATE_ECN_VALUE | 
                                            BCM_MIRROR_PKT_HEADER_UPDATE_ECN_CNM_CANCEL | BCM_MIRROR_PKT_HEADER_UPDATE_TRUNK_HASH_RESULT | BCM_MIRROR_PKT_HEADER_UPDATE_VSQ;
        dest.packet_control_updates.cnm_cancel = cnm_cancel;
        dest.packet_control_updates.color = color;
        dest.packet_control_updates.prio = prio;
        dest.packet_control_updates.vsq = vsq;
        dest.packet_control_updates.ecn_value = ecn_value;
        dest.packet_control_updates.trunk_hash_result = trunk_hash_result;

        if (rv = bcm_mirror_destination_create(unit, &dest)) {
            printf("failed to create mirror destination, return value %d\n", rv);
            return rv;
        }

        dest_id = BCM_GPORT_MIRROR_GET(dest.mirror_dest_id);
        bcm_mirror_destination_t_init(&dest);
        BCM_GPORT_MIRROR_SET(dest_id, dest_id);

        if (rv = bcm_mirror_destination_get(unit, dest_id, &dest)) {
            printf("failed to get mirror destination information, return value %d\n", rv);
            return rv;
        }
        
        if (dest.packet_control_updates.color != color) return BCM_E_FAIL;
        if (dest.packet_control_updates.prio != prio) return BCM_E_FAIL;
        if (dest.packet_control_updates.vsq != vsq) return BCM_E_FAIL;
        if (dest.packet_control_updates.trunk_hash_result != trunk_hash_result) return BCM_E_FAIL;

        device_type_t dev_type;
        get_device_type(unit, &dev_type);
        if(dev_type ==  device_type_jericho){
            if (dest.packet_control_updates.cnm_cancel != cnm_cancel) return BCM_E_FAIL;
            if (dest.packet_control_updates.ecn_value != ecn_value) return BCM_E_FAIL;
        }
        if (rv = bcm_mirror_destination_destroy(unit, dest_id)) {
            printf("Failed to destroy mirror destination, return value %d\n", rv);
            return rv;
        }
    }

    printf ("==> Test finished successfully\n");
    /* return with no existing mirror destinations */
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
int mirror_ports_arad_test_1(int unit, int device_type) {
    int rv, i, modid;
    int found_ports_num, found_ports[8];
    bcm_gport_t mirr_dest1, mirr_dest2;
    bcm_mirror_destination_t dest = {0};
    bcm_gport_t port1, port2, port3, out_gport;
    unsigned int u_out_count;
    int out_count;
    bcm_vlan_t vlan1 = 100, vlan2 = 999, vlan3 = 432;

    rv = get_n_ports(unit, 8, found_ports, &found_ports_num);
    if (rv) {
        printf("failed to get ports for the test\n");
        return rv;
    } else if (found_ports_num < 4) {
        printf("found only %d ports for the test which is not enough\n", found_ports_num);
        return BCM_E_FAIL;
    }

    rv = bcm_stk_modid_get(unit, &modid);
    if (rv) {
        printf("failed to get modid\n");
        return rv;
    }
    
    BCM_GPORT_LOCAL_SET(port1, found_ports[0]);
    BCM_GPORT_LOCAL_SET(port2, found_ports[2]);
    if (bcm_stk_sysport_gport_get(unit, port2, &port2) != BCM_E_NONE) {
        printf("Failed to get gport\n");
    }

    BCM_GPORT_LOCAL_SET(port3, found_ports[3]);

    if (rv = delete_all_mirror_destinations(unit)) {
        return rv;
    }
    /* remove previous mirrors on the ports & vlans we use, if they exist. The APIs will gail if these were not added */
    bcm_mirror_port_dest_delete_all(unit, port1, BCM_MIRROR_PORT_INGRESS);
    bcm_mirror_port_dest_delete_all(unit, port2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS);
    bcm_mirror_port_dest_delete_all(unit, port3, BCM_MIRROR_PORT_EGRESS);
    bcm_mirror_port_vlan_dest_delete_all(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS);
    bcm_mirror_port_vlan_dest_delete_all(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS);
    bcm_mirror_port_vlan_dest_delete_all(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS);
    printf ("==> Above (up to 6) failures of bcm_mirror_port_dest_delete_all and bcm_mirror_port_vlan_dest_delete_all are expected\n");

    /* create two mirror destinations */
    BCM_GPORT_LOCAL_SET(dest.gport, found_ports[1]);
    dest.flags = 0;
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination 1, return value %d\n", rv);
        return rv;
    }
    mirr_dest1 = dest.mirror_dest_id;

    BCM_GPORT_LOCAL_SET(dest.gport, found_ports[found_ports_num > 4 ? 4 : 2]);
    dest.flags = 0;
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination 2, return value %d\n", rv);
        return rv;
    }
    mirr_dest2 = dest.mirror_dest_id;

    /* mirror inbound/outbound ports and ports+vlans to destinations */
    if (rv = bcm_mirror_port_dest_add(unit, port1, BCM_MIRROR_PORT_INGRESS, mirr_dest1)) {
        printf("failed to add inbound port1 to be mirrored to mirr_dest1, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_dest_add(unit, port2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest2)) {
        printf("failed to add inbound+outbound port2 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_dest_add(unit, port3, BCM_MIRROR_PORT_EGRESS, mirr_dest1)) {
        printf("failed to add outbound port2 to be mirrored to mirr_dest1, return value %d\n", rv);
        return rv;
    }

    if (rv = bcm_mirror_port_vlan_dest_add(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, mirr_dest2)) {
        printf("failed to add inbound port1+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_vlan_dest_add(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest1)) {
        printf("failed to add inbound+outbound port1+vlan1 to be mirrored to mirr_dest1, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_vlan_dest_add(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, mirr_dest2)) {
        printf("failed to add outbound port1+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    /* Get and verify the information that we added */
    if (rv = bcm_mirror_port_dest_get(unit, port1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count)) {
        printf("failed to get inbound port1 mirror destination, return value %d\n", rv);
        return rv;
    } else if (out_count != 1) {
        printf("Output destination count of inbound port1 was %d instead of 1\n", out_count);
        return BCM_E_FAIL;
    } else if (out_gport != mirr_dest1) {
        printf("mirror destination of inbound port1 was %lx and not the expected %lx\n", out_gport, mirr_dest1);
        return BCM_E_FAIL;
    }
    if (rv = bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count)) {
        printf("failed to get inbound port2 mirror destination, return value %d\n", rv);
        return rv;
    } else if (out_count != 1) {
        printf("Output destination count of inbound port2 was %d instead of 1\n", out_count);
        return BCM_E_FAIL;
    } else if (out_gport != mirr_dest2) {
        printf("mirror destination of inbound port2 was %lx and not the expected %lx\n", out_gport, mirr_dest2);
        return BCM_E_FAIL;
    }
    if (rv = bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count)) {
        printf("failed to get outbound port2 mirror destination, return value %d\n", rv);
        return rv;
    } else if (out_count != 1) {
        printf("Output destination count of outbound port2 was %d instead of 1\n", out_count);
        return BCM_E_FAIL;
    } else if (out_gport != mirr_dest2) {
        printf("mirror destination of outbound port2 was %lx and not the expected %lx\n", out_gport, mirr_dest2);
        return BCM_E_FAIL;
    }
    if (rv = bcm_mirror_port_dest_get(unit, port3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count)) {
        printf("failed to get outbound port3 mirror destination, return value %d\n", rv);
        return rv;
    } else if (out_count != 1) {
        printf("Output destination count of outbound port2 was %d instead of 1\n", out_count);
        return BCM_E_FAIL;
    } else if (out_gport != mirr_dest1) {
        printf("mirror destination of outbound port3 was %lx and not the expected %lx\n", out_gport, mirr_dest1);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_vlan_dest_get(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count)) {
        printf("failed to get inbound port1+vlan1 mirror destination, return value %d\n", rv);
        return rv;
    } else if (u_out_count != 1) {
        printf("Output destination count of inbound port1+vlan1 was %d instead of 1\n", u_out_count);
        return BCM_E_FAIL;
    } else if (out_gport != mirr_dest2) {
        printf("mirror destination of inbound port1+vlan1 was %lx and not the expected %lx\n", out_gport, mirr_dest2);
        return BCM_E_FAIL;
    }
    if (rv = bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count)) {
        printf("failed to get inbound port2+vlan2 mirror destination, return value %d\n", rv);
        return rv;
    } else if (u_out_count != 1) {
        printf("Output destination count of inbound port2+vlan2 was %d instead of 1\n", u_out_count);
        return BCM_E_FAIL;
    } else if (out_gport != mirr_dest1) {
        printf("mirror destination of inbound port2+vlan2 was %lx and not the expected %lx\n", out_gport, mirr_dest1);
        return BCM_E_FAIL;
    }
    if (rv = bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count)) {
        printf("failed to get outbound port2+vlan2 mirror destination, return value %d\n", rv);
        return rv;
    } else if (u_out_count != 1) {
        printf("Output destination count of outbound port2+vlan2 was %d instead of 1\n", u_out_count);
        return BCM_E_FAIL;
    } else if (out_gport != mirr_dest1) {
        printf("mirror destination of outbound port2+vlan2 was %lx and not the expected %lx\n", out_gport, mirr_dest1);
        return BCM_E_FAIL;
    }
    if (rv = bcm_mirror_port_vlan_dest_get(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count)) {
        printf("failed to get outbound port3+vlan3 mirror destination, return value %d\n", rv);
        return rv;
    } else if (u_out_count != 1) {
        printf("Output destination count of outbound port3+vlan3 was %d instead of 1\n", u_out_count);
        return BCM_E_FAIL;
    } else if (out_gport != mirr_dest2) {
        printf("mirror destination of outbound port3+vlan3 was %lx and not the expected %lx\n", out_gport, mirr_dest2);
        return BCM_E_FAIL;
    }


    /* test deletions for a specific destination */
    if (rv = bcm_mirror_port_dest_delete(unit, port1, BCM_MIRROR_PORT_INGRESS, mirr_dest1)) {
        printf("failed to delete inbound port1 mirror to mirr_dest1, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_dest_delete(unit, port2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest2)) {
        printf("failed to delete inbound+outbound port2 mirror to mirr_dest2, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_dest_delete(unit, port3, BCM_MIRROR_PORT_EGRESS, mirr_dest1)) {
        printf("failed to delete outbound port3 mirror to mirr_dest1, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_vlan_dest_delete(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, mirr_dest2)) {
        printf("failed to delete inbound port1+vlan1 mirror to mirr_dest2, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_vlan_dest_delete(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest1)) {
        printf("failed to delete inbound+outbound port2+vlan2 mirror to mirr_dest1, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_vlan_dest_delete(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, mirr_dest2)) {
        printf("failed to delete outbound port3+vlan3 mirror to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    /* Verify that they do not exist */
    if (rv = bcm_mirror_port_dest_get(unit, port1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count)) {
        printf("Getting inbound port1 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting inbound port1 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count)) {
        printf("Getting inbound port2 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting inbound port2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count)) {
        printf("Getting outbound port2 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting outbound port2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_dest_get(unit, port3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count)) {
        printf("Getting outbound port3 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting outbound port3 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_vlan_dest_get(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count)) {
        printf("Getting inbound port1+vlan1 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting inbound port1+vlan1 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count)) {
        printf("Getting inbound port2+vlan2 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting inbound port2+vlan2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count)) {
        printf("Getting outbound port2+vlan2 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting outbound port2+vlan2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_vlan_dest_get(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count)) {
        printf("Getting outbound port3+vlan3 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting outbound port3+vlan3 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    /* mirror inbound/outbound ports and ports+vlans to destinations again so that they can be deleted again */
    if (rv = bcm_mirror_port_dest_add(unit, port1, BCM_MIRROR_PORT_INGRESS, mirr_dest1)) {
        printf("failed to add inbound port1 to be mirrored to mirr_dest1, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_dest_add(unit, port2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest2)) {
        printf("failed to add inbound+outbound port2 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_dest_add(unit, port3, BCM_MIRROR_PORT_EGRESS, mirr_dest1)) {
        printf("failed to add outbound port2 to be mirrored to mirr_dest1, return value %d\n", rv);
        return rv;
    }

    if (rv = bcm_mirror_port_vlan_dest_add(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, mirr_dest2)) {
        printf("failed to add inbound port1+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_vlan_dest_add(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS, mirr_dest1)) {
        printf("failed to add inbound+outbound port1+vlan1 to be mirrored to mirr_dest1, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_vlan_dest_add(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, mirr_dest2)) {
        printf("failed to add outbound port1+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }


    /* test deletions for all destination */
    if (rv = bcm_mirror_port_dest_delete_all(unit, port1, BCM_MIRROR_PORT_INGRESS)) {
        printf("failed to delete inbound port1 mirror, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_dest_delete_all(unit, port2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) {
        printf("failed to delete inbound port2 mirror, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_dest_delete_all(unit, port3, BCM_MIRROR_PORT_EGRESS)) {
        printf("failed to delete inbound port3 mirror, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_vlan_dest_delete_all(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS)) {
        printf("failed to delete inbound port1+vlan1 mirror, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_vlan_dest_delete_all(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) {
        printf("failed to delete inbound port2+vlan2 mirror, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_port_vlan_dest_delete_all(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS)) {
        printf("failed to delete inbound port3+vlan3 mirror, return value %d\n", rv);
        return rv;
    }

    /* Verify that they do not exist */
    if (rv = bcm_mirror_port_dest_get(unit, port1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count)) {
        printf("Getting inbound port1 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting inbound port1 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &out_count)) {
        printf("Getting inbound port2 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting inbound port2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_dest_get(unit, port2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count)) {
        printf("Getting outbound port2 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting outbound port2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_dest_get(unit, port3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count)) {
        printf("Getting outbound port3 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting outbound port3 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_vlan_dest_get(unit, port1, vlan1, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count)) {
        printf("Getting inbound port1+vlan1 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting inbound port1+vlan1 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_INGRESS, 1, &out_gport, &u_out_count)) {
        printf("Getting inbound port2+vlan2 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting inbound port2+vlan2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_vlan_dest_get(unit, port2, vlan2, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count)) {
        printf("Getting outbound port2+vlan2 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting outbound port2+vlan2 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }

    if (rv = bcm_mirror_port_vlan_dest_get(unit, port3, vlan3, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count)) {
        printf("Getting outbound port3+vlan3 mirror destination return value %d\n", rv);
        return rv;
    }

    if (BCM_GPORT_MIRROR_GET(out_gport) != BCM_MIRROR_INVALID_PROFILE) {
        printf("Getting outbound port3+vlan3 mirror destination should have returned BCM_MIRROR_INVALID_PROFILE, but returned value is %d\n", out_gport);
        return BCM_E_FAIL;
    }


    /* cleanup - delete the created mirror destinations */
    if (rv = bcm_mirror_destination_destroy(unit, mirr_dest1)) {
        printf("could not destroy mirr_dest1, return value %d\n", rv);
        return rv;
    }
    if (rv = bcm_mirror_destination_destroy(unit, mirr_dest2)) {
        printf("could not destroy mirr_dest2, return value %d\n", rv);
        return rv;
    }

    /* test reserving mirroring resources (reassembly context and recycle channel) for ports */

    BCM_GPORT_MIRROR_SET(mirr_dest1, 0);
    for (i = 0; i < found_ports_num; ++i ) {
        BCM_GPORT_LOCAL_SET(port1, found_ports[i]);
        if (i & 1 ) {
            if ((rv = bcm_stk_sysport_gport_get(unit, port1, &port1)) != BCM_E_NONE) {
                printf("Failed to get system port\n");
            }
        }
        bcm_mirror_port_dest_delete_all(unit, port1, BCM_MIRROR_PORT_EGRESS_ACL); /* do not check failure */
        if ((rv = bcm_mirror_port_dest_get(unit, port1, BCM_MIRROR_PORT_EGRESS_ACL, 1, &out_gport, &out_count)) != BCM_E_NONE || out_count) {
            printf("failed making sure that mirroring resources for port %d are not initially reserved\n", found_ports[i]);
            return BCM_E_FAIL;
        }
    }
    for (i = 0; i < found_ports_num; ++i ) {
        BCM_GPORT_LOCAL_SET(port1, found_ports[i]);
        if (i & 1 == 0) {
            if ((rv = bcm_stk_sysport_gport_get(unit, port1, &port1)) != BCM_E_NONE) {
                printf("Failed to get system port\n");
            }
        }
        if ((rv = bcm_mirror_port_dest_add(unit, port1, BCM_MIRROR_PORT_EGRESS_ACL, i)) != BCM_E_NONE) {
            printf("failed reserving mirroring resources for port %d\n", found_ports[i]);
            return rv;
        } else if ((rv = bcm_mirror_port_dest_get(unit, port1, BCM_MIRROR_PORT_EGRESS_ACL, 1, &out_gport, &out_count))
                   != BCM_E_NONE || out_count != 1 || out_gport != mirr_dest1) {
            printf("failed making sure that mirroring resources for port %d are reserved\n", found_ports[i]);
            return BCM_E_FAIL;
        }
    }
    for (i = 0; i < found_ports_num; ++i ) {
        BCM_GPORT_LOCAL_SET(port1, found_ports[i]);
        if (i & 1) {
            if ((rv = bcm_stk_sysport_gport_get(unit, port1, &port1)) != BCM_E_NONE) {
                printf("Failed to get system port\n");
            }
        }
        if ((rv = bcm_mirror_port_dest_delete(unit, port1, BCM_MIRROR_PORT_EGRESS_ACL, found_ports_num-i)) != BCM_E_NONE) {
            printf("failed un-reserving mirroring resources for port %d\n", found_ports[i]);
            return rv;
        } else if ((rv = bcm_mirror_port_dest_get(unit, port1, BCM_MIRROR_PORT_EGRESS_ACL, 1, &out_gport, &out_count))
                   != BCM_E_NONE || out_count) {
            printf("failed making sure that mirroring resources for port %d were un-reserved\n", found_ports[i]);
            return BCM_E_FAIL;
        } else if ((rv = bcm_mirror_port_dest_add(unit, port1, BCM_MIRROR_PORT_EGRESS_ACL, port1+i)) != BCM_E_NONE) {
            printf("failed reserving (again) mirroring resources for port %d\n", found_ports[i]);
            return rv;
        } else if ((rv = bcm_mirror_port_dest_get(unit, port1, BCM_MIRROR_PORT_EGRESS_ACL, 1, &out_gport, &out_count))
                   != BCM_E_NONE || out_count != 1 || out_gport != mirr_dest1) {
            printf("failed making sure (again) that mirroring resources for port %d are reserved\n", found_ports[i]);
            return BCM_E_FAIL;
        } else if ((rv = bcm_mirror_port_dest_delete_all(unit, port1, BCM_MIRROR_PORT_EGRESS_ACL)) != BCM_E_NONE) {
            printf("failed un-reserving (again) mirroring resources for port %d\n", found_ports[i]);
            return rv;
        } else if ((rv = bcm_mirror_port_dest_get(unit, port1, BCM_MIRROR_PORT_EGRESS_ACL, 1, &out_gport, &out_count))
                   != BCM_E_NONE || out_count) {
            printf("failed making sure that mirroring resources for port %d were un-reserved\n", found_ports[i]);
            return BCM_E_FAIL;
        }
    }

    /* Jericho only - Test mirroring/forwaring strength and enable */

    if (device_type == SOC_IS_JERICHO) {
        BCM_GPORT_LOCAL_SET(port1, found_ports[0]);
        /*create mirror destination*/
        bcm_mirror_destination_t_init(&dest);
        BCM_GPORT_LOCAL_SET(dest.gport, found_ports[1]);
        dest.flags = 0;
        if (rv = bcm_mirror_destination_create(unit, &dest)) {
            printf("failed to create mirror destination 1, return value %d\n", rv);
            return rv;
        }
        mirr_dest1 = dest.mirror_dest_id;
        /*choose random options*/
        bcm_mirror_options_t selected_options, returned_options;
        selected_options.copy_strength = sal_rand() % 4;
        selected_options.forward_strength = sal_rand() % 4;

 /*add port to mirror_destination*/
        if (rv = bcm_mirror_port_destination_add(unit, port1, BCM_MIRROR_PORT_EGRESS, mirr_dest1, selected_options)) {
            printf("failed to add outbound port1 to be mirrored to mirr_dest1, return value %d\n", rv);
            return rv;
        }
        /*get port mirror_destination options and compare*/
        if (rv = bcm_mirror_port_destination_get(unit, port1, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &out_count, &returned_options)) {
            printf("failed to get outbound port1 mirror destination, return value %d\n", rv);
            return rv;
        }

        if (returned_options.copy_strength != selected_options.copy_strength) {
            printf("failed to get outbound mirroring mirror strength, expected %d but got %d\n", selected_options.copy_strength, returned_options.copy_strength);
            return BCM_E_FAIL;
        } else if (returned_options.forward_strength != selected_options.forward_strength) {
            printf("failed to get outbound mirroring forward strength, expected %d but got %d\n", selected_options.forward_strength, returned_options.forward_strength);
            return BCM_E_FAIL;
        }
        /*cleanup*/
        if ((rv = bcm_mirror_port_dest_delete(unit, port1, BCM_MIRROR_PORT_EGRESS, mirr_dest1)) != BCM_E_NONE) {
            printf("failed deleting mirroring resources for port %d\n", found_ports[1]);
            return rv;
        }

        /*choose new random options*/
        selected_options.copy_strength = sal_rand() % 4;
        selected_options.forward_strength = sal_rand() % 4;

 /*add port to vlan mirror_destination*/
        if (rv = bcm_mirror_port_vlan_destination_add(unit, port1, vlan1, BCM_MIRROR_PORT_EGRESS, mirr_dest1, selected_options)) {
            printf("failed to add outbound port1+vlan1 to be mirrored to mirr_dest1, return value %d\n", rv);
            return rv;
        }
        /*get port mirror_destination options and compare*/
        if (rv = bcm_mirror_port_vlan_destination_get(unit, port1, vlan1, BCM_MIRROR_PORT_EGRESS, 1, &out_gport, &u_out_count, &returned_options)) {
            printf("failed to get outbound port1+vlan1 mirror destination, return value %d\n", rv);
            return rv;
        }

        if (returned_options.copy_strength != selected_options.copy_strength) {
            printf("failed to get vlan1 outbound mirroring mirror strength, expected %d but got %d\n", selected_options.copy_strength, returned_options.copy_strength);
            return BCM_E_FAIL;
        } else if (returned_options.forward_strength != selected_options.forward_strength) {
            printf("failed to get vlan1 outbound mirroring forward strength, expected %d but got %d\n", selected_options.forward_strength, returned_options.forward_strength);
            return BCM_E_FAIL;
        }
        /*cleanup*/
        if ((rv = bcm_mirror_port_vlan_dest_delete(unit, port1, vlan1, BCM_MIRROR_PORT_EGRESS, mirr_dest1)) != BCM_E_NONE) {
            printf("failed deleting mirroring resources for port %d\n", found_ports[1]);
            return rv;
        }

        /* cleanup - delete the created mirror destination */
        if (rv = bcm_mirror_destination_destroy(unit, mirr_dest1)) {
            printf("could not destroy mirr_dest1, return value %d\n", rv);
            return rv;
        }
    }
    return BCM_E_NONE;
}



/**
 * 
 * 
 * @author sinai (04/05/2014)
 * 
 * @param unit 
 * @param seed - used for randomization 
 * 
 * @return int 
 */
int mirror_destinations_arad_test_erspan(int unit, int seed) {
    bcm_mirror_destination_t dest = {0};
    int found_ports_num, found_ports[4];
    int intf_id;
    bcm_gport_t tunnel_gport;
    int rv;
    bcm_tunnel_initiator_t erspan_tunnel;
    bcm_l3_intf_t intf;
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int span_id;
    int mirror_dest_id;
    int mirror_gport;
    int tunnel_id;


    sal_srand(seed);
    printf("Randomizing with seed %d\n",seed);

    rv = get_n_ports(unit, 4, found_ports, &found_ports_num);
    if (rv) {
        printf("failed to get ports for the test\n");
        return rv;
    } else if (found_ports_num < 2) {
        printf("found only %d ports for the test which is not enough\n", found_ports_num);
        return BCM_E_FAIL;
    }

    rv = ipv4_tunnel_build_tunnels_erspan(unit, found_ports[2], &intf_id, &tunnel_gport, next_hop_mac);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

    int in_vlan = sal_rand() % 4095 +1;
    rv = l2_open_vlan_with_port(unit, in_vlan, found_ports[0]);
    if(rv != BCM_E_NONE){
            printf("failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
       return rv;
   }


    rv = l2_open_vlan_with_port(unit, in_vlan,  found_ports[1]);
    if(rv != BCM_E_NONE){
            printf("failed to add inbound out_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
       return rv;
   }  


    if (is_device_or_above(unit,JERICHO)) {
        /* Step 1: build ERSPAN tunnel */
        bcm_tunnel_initiator_t_init(&erspan_tunnel);
        bcm_l3_intf_t_init(&intf);
        /* span info */
        erspan_tunnel.type       = bcmTunnelTypeErspan;
        span_id = sal_rand() % 0x400; /*span ID is a 10 bit field */
        erspan_tunnel.span_id    = span_id;
        erspan_tunnel.l3_intf_id = intf_id;/* points to ip tunnel */
        /* set tunnel_idto control which outlif to set the span tunnel
           BCM_GPORT_TUNNEL_ID_SET(erspan_tunnel.tunnel_id,0x5000);
        */
        rv = bcm_tunnel_initiator_create(unit, &intf, &erspan_tunnel);
        if(rv != BCM_E_NONE) {
            printf("Error, create tunnel initiator \n");
            return rv;
        }


        /* if mirror desination is Multicast:
         * then set dest.gport using BCM_GPORT_MCAST_SET 
         * and that's all. 
         *  
         * if mirror destination is Unicast, set encap-id 
         */
        /*  */
        dest.flags = BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
        tunnel_id = erspan_tunnel.tunnel_id;
        dest.encap_id = BCM_GPORT_TUNNEL_ID_GET(erspan_tunnel.tunnel_id); 

        /* Step 2: set Mirror destination */
        dest.gport = found_ports[2]; /* dest port*/

         if (rv = bcm_mirror_destination_create(unit, &dest)) {
            printf("failed to create mirror destination 1, return value %d\n", rv);
            return rv;
        }

         /* Step 3: Get the tunnel*/
         bcm_tunnel_initiator_t_init(&erspan_tunnel);
         bcm_l3_intf_t_init(&intf);
         /* span info */
         intf.l3a_tunnel_idx = dest.encap_id;
         rv = bcm_tunnel_initiator_get(unit,&intf,&erspan_tunnel);
         if (rv != BCM_E_NONE) {
             printf("failed bcm_tunnel_initiator_get %d\n", rv);
             return rv;
         }

         if (erspan_tunnel.type != bcmTunnelTypeErspan) {
             printf("bcm_tunnel_initiator_get did not return correct type");
             return 1;
         }

         if (erspan_tunnel.span_id != span_id) {
             printf("bcm_tunnel_initiator_get did not return correct span_id. Got: %d, expected %d\n", erspan_tunnel.span_id , span_id);
             return 1;
         }

         mirror_dest_id = dest.mirror_dest_id;
         mirror_gport = dest.gport;
         bcm_mirror_destination_t_init(&dest);
         if (rv = bcm_mirror_destination_get(unit, mirror_dest_id, &dest)) {
             printf("failed to get mirror destination information, destination id %d, return value %d\n", i, rv);
             return rv;
         }

         /* Step 4: update*/
         bcm_tunnel_initiator_t_init(&erspan_tunnel);
         bcm_l3_intf_t_init(&intf);
         erspan_tunnel.type       = bcmTunnelTypeErspan;
         span_id = sal_rand() % 0x400; /*span ID is a 10 bit field */
         erspan_tunnel.span_id    = span_id; /*presumably this gives us a different ID */
         erspan_tunnel.l3_intf_id = intf_id;/* points to ip tunnel */
         erspan_tunnel.flags = BCM_TUNNEL_REPLACE |BCM_TUNNEL_WITH_ID;
         erspan_tunnel.tunnel_id = tunnel_id;
         /* Update the tunnel*/
         rv = bcm_tunnel_initiator_create(unit, &intf, &erspan_tunnel);
         if(rv != BCM_E_NONE) {
             printf("Error, create tunnel initiator \n");
             return rv;
         }

         /* Now the mirror*/
        dest.flags = BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID | BCM_MIRROR_DEST_REPLACE | BCM_MIRROR_DEST_WITH_ID;
        dest.encap_id = BCM_GPORT_TUNNEL_ID_GET(erspan_tunnel.tunnel_id); 
        mirror_gport = dest.gport;
        BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, mirror_dest_id);
        if (rv = bcm_mirror_destination_create(unit, &dest)) {
           printf("failed to create mirror destination 1, return value %d\n", rv);
           return rv;
        }


         /* Step 5: destroy the mirror then the tunnel*/
        if (rv = bcm_mirror_destination_destroy(unit, mirror_dest_id)) {
            printf("could not destroy mirror, return value %d\n", rv);
            return rv;
        }


        intf.l3a_tunnel_idx =  erspan_tunnel.tunnel_id;
        if (rv = bcm_tunnel_initiator_clear(unit,&intf)) {
            printf("could not destroy mirror, return value %d\n", rv);
            return rv;
        }


        /* Step 6: Create again WITH_ID to verify that the tunnel has been freed.*/
        bcm_tunnel_initiator_t_init(&erspan_tunnel);
        bcm_l3_intf_t_init(&intf);
        /* span info */
        erspan_tunnel.type       = bcmTunnelTypeErspan;
        span_id = sal_rand() % 0x400; /*span ID is a 10 bit field*/
        erspan_tunnel.span_id    = span_id;
        erspan_tunnel.l3_intf_id = intf_id;/* points to ip tunnel */
        /* set tunnel_idto control which outlif to set the span tunnel
           BCM_GPORT_TUNNEL_ID_SET(erspan_tunnel.tunnel_id,0x5000);
        */
        erspan_tunnel.flags = BCM_TUNNEL_WITH_ID;
        erspan_tunnel.tunnel_id = tunnel_id;
        rv = bcm_tunnel_initiator_create(unit, &intf, &erspan_tunnel);
        if(rv != BCM_E_NONE) {
            printf("Error, create tunnel initiator \n");
            return rv;
        }
    } else {
        int yesno;
        uint32 tmp_value;
        is_arad_plus_only(unit, &yesno);
        
        /* Erspan for Arad+ is reverted to Arad */
        bcm_mac_t ll_src_mac  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  
        bcm_mac_t ll_dst_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};

        bcm_if_t dest_encap_id;
        
        /* ports distribution
         * found_ports[0] <-----L2 switch------> found_ports[1]
         *
         * found_ports[0] <---ingress mirror---> found_ports[2]
         */
        if (yesno == 1) {
            
            /* 1: enable erspan on port */
            rv = bcm_port_control_set(unit, found_ports[0], bcmPortControlErspanEnable, 1);
            if ((rv != BCM_E_NONE)) {
                printf("Error, bcm_port_control_set failed: %d\n", rv);
                return rv;
            }
            printf(" ERSPAN(Arad+): enable erspan on port[%d] successfully!\n", found_ports[0]);

            /* 2: set Mirror destination */
            dest.flags |= BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
            dest.tunnel_id = intf_id;
            
            /* SPAN Header */
            dest.flags |= BCM_MIRROR_DEST_TUNNEL_IP_GRE | BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID;
            tmp_value = sal_rand() % 0x400;
            dest.span_id  = tmp_value;
            
            /* L2 */
            dest.flags |= BCM_MIRROR_DEST_TUNNEL_L2;
            /* vlan TAG */
            tmp_value = sal_rand() % 0x10000;
            dest.tpid     = tmp_value; /* tpid=0 means no tagging */
            tmp_value = sal_rand() % 4095 + 1;
            dest.vlan_id  = tmp_value;
            tmp_value = sal_rand() % 0x8;
            dest.pkt_prio = tmp_value;
            /* MACs */
            dest.dst_mac  = ll_dst_mac;
            dest.src_mac  = ll_src_mac;
            
            /* the API forces encap_ip=tunnel_id[3:0] */
            dest.encap_id  = dest.tunnel_id & 0xf;
            dest_encap_id = dest.encap_id;
                
            /* set Mirror destination */
            dest.gport = found_ports[2]; /* dest port*/
            if (rv = bcm_mirror_destination_create(unit, &dest)) {
                printf("failed to create mirror destination 1, return value %d\n", rv);
                return rv;
            }
            printf(" ERSPAN(Arad+): Create mirror destination successfully!\n");

            /* 3: Get the mirror dest */
            mirror_dest_id = dest.mirror_dest_id;
            bcm_mirror_destination_t_init(&dest);
            if (rv = bcm_mirror_destination_get(unit, mirror_dest_id, &dest)) {
                printf("failed to get mirror destination information, destination id %d, return value %d\n", i, rv);
                return rv;
            }
            printf(" ERSPAN(Arad+): Get mirror destination successfully!\n");

            /* 4: update mirror dest*/
            dest.flags = BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID | BCM_MIRROR_DEST_REPLACE | BCM_MIRROR_DEST_WITH_ID;
            dest.encap_id = dest_encap_id;
            BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, mirror_dest_id);
            if (rv = bcm_mirror_destination_create(unit, &dest)) {
               printf("failed to create mirror destination 1, return value %d\n", rv);
               return rv;
            }
            printf(" ERSPAN(Arad+): Update mirror destination successfully!\n");
            
            /* 5: destroy the mirror then the tunnel*/
            if (rv = bcm_mirror_destination_destroy(unit, mirror_dest_id)) {
                printf("could not destroy mirror, return value %d\n", rv);
                return rv;
            }
            printf(" ERSPAN(Arad+): Destroy mirror destination successfully!\n");
        }
    }
    
    return BCM_E_NONE;
}

int check_EPNI_MIRROR_VID_REG_state(int unit, int value){
    reg_val val = {0};
    diag_reg_get(unit, "EPNI_MIRROR_VID_REG.EPNI0", val);
    if (val[0] != value) {
        printf("EPNI_MIRROR_VID_REG isn't configured properly on core 0, expected %d but got %d\n", value, val[0]);
        return BCM_E_FAIL;
    } 
    diag_reg_get(unit, "EPNI_MIRROR_VID_REG.EPNI1", val);
    if (val[0] != value) {
        printf("EPNI_MIRROR_VID_REG isn't configured properly on core 1, expected %d but got %d\n", value, val[0]);
        return BCM_E_FAIL;
    }
    return BCM_E_NONE;
}

/*
* This test adds core 0 and core 1 port+vlan egress mirroring, verifing that EPNI_MIRROR_VID_REG is configured correctly.
*/
int mirror_egress_vlan_id(int unit){
    int vlan, rv; 
    int found_ports_num, found_ports[10], found_port_cores[2] = {0}, found_mirror_dest_port = 0;
    bcm_gport_t gport0, gport1;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;
    unsigned int flags;
    int i, port, mirror_dest;
    bcm_mirror_destination_t mdst;

    rv = get_n_ports(unit, 10, found_ports, &found_ports_num);
    if (rv) {
        printf("failed to get ports for the test\n");
        return rv;
    }

    for (i = 0; i < found_ports_num; i++) {
        rv = bcm_port_get(unit, found_ports[i], &flags, &interface_info, &mapping_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_get: rv(%d), unit(%d), failed to get port(%d) info \n",rv, unit, found_ports[i]);
            return rv;
        }
        
        if (mapping_info.core == 0 && !found_port_cores[0]){
            BCM_GPORT_LOCAL_SET(gport0, found_ports[i]);
            found_port_cores[0] = 1;
        } else if(mapping_info.core == 1 && !found_port_cores[1]){
            BCM_GPORT_LOCAL_SET(gport1, found_ports[i]);
            found_port_cores[1] = 1;
        } else if(!found_mirror_dest_port){
            mirror_dest = found_ports[i];
            found_mirror_dest_port = 1;
        }

        if (found_port_cores[0] && found_port_cores[1] && found_mirror_dest_port) {
            break;
        }
    }

    if(!found_port_cores[0] || !found_port_cores[1] || !found_mirror_dest_port){
        printf("Couldn't find enough ports\n");
        return BCM_E_PARAM;
    }

    rv = check_EPNI_MIRROR_VID_REG_state(unit, 0x0);
    if (rv) {
        return rv;;
    }

    bcm_mirror_destination_t_init(mdst);
    BCM_GPORT_LOCAL_SET(mdst.gport, mirror_dest);
    mdst.flags = 0;
    rv = bcm_mirror_destination_create(unit, &mdst);
    if (rv != BCM_E_NONE) {
        printf("bcm_mirror_destination_create: rv(%d), unit(%d), failed to create mirror destionation.\n",rv, unit);
        return rv;
    }

    /*add core 0 port and then core 1 port*/
    vlan = 100;
    if (found_port_cores[0]) {
        rv = bcm_mirror_port_vlan_dest_add(unit, gport0, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_add: rv(%d), unit(%d), port(%d), vlan(%d), failed to add port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport0), vlan);
            return rv;
        }
    }
    if (found_port_cores[1]) {
        rv = bcm_mirror_port_vlan_dest_add(unit, gport1, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_add: rv(%d), unit(%d), port(%d), vlan(%d), failed to add port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport1), vlan);
            return rv;
        }
    }

    rv = check_EPNI_MIRROR_VID_REG_state(unit, 100);
    if (rv) {
        return rv;
    }
    
    vlan = 200;
    if (found_port_cores[0]) {
        rv = bcm_mirror_port_vlan_dest_add(unit, gport0, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_add: rv(%d), unit(%d), port(%d), failed to add port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport0));
            return rv;
        }
    }
    if (found_port_cores[1]) {
        rv = bcm_mirror_port_vlan_dest_add(unit, gport1, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_add: rv(%d), unit(%d), port(%d), failed to add port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport1));
            return rv;
        }
    }

    rv = check_EPNI_MIRROR_VID_REG_state(unit, 0xc8064);
    if (rv) {
        return rv;
    }

    vlan = 100;
    if (found_port_cores[0]) {
        rv = bcm_mirror_port_vlan_dest_delete(unit, gport0, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_delete: rv(%d), unit(%d), port(%d), failed to delete port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport0));
            return rv;
        }
    }

    rv = check_EPNI_MIRROR_VID_REG_state(unit, 0xc8064);
    if (rv) {
        return rv;
    }

    if (found_port_cores[1]) {
        rv = bcm_mirror_port_vlan_dest_delete(unit, gport1, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_delete: rv(%d), unit(%d), port(%d), failed to delete port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport1));
            return rv;
        }
    }

    rv = check_EPNI_MIRROR_VID_REG_state(unit, 0xc8000);
    if (rv) {
        return rv;
    }

    vlan =200;
    if (found_port_cores[1]) {
        rv = bcm_mirror_port_vlan_dest_delete(unit, gport1, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_delete: rv(%d), unit(%d), port(%d), failed to delete port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport1));
            return rv;
        }
    }

    rv = check_EPNI_MIRROR_VID_REG_state(unit, 0xc8000);
    if (rv) {
        return rv;
    }

    if (found_port_cores[0]) {
        rv = bcm_mirror_port_vlan_dest_delete(unit, gport0, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_delete: rv(%d), unit(%d), port(%d), failed to delete port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport0));
            return rv;
        }
    }

    rv = check_EPNI_MIRROR_VID_REG_state(unit, 0x0);
    if (rv) {
        return rv;
    }

    /*add core 1 port and then core 0 port*/
    vlan = 100;
    if (found_port_cores[1]) {
        rv = bcm_mirror_port_vlan_dest_add(unit, gport1, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_add: rv(%d), unit(%d), port(%d), vlan(%d), failed to add port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport1), vlan);
            return rv;
        }
    }

    if (found_port_cores[0]) {
        rv = bcm_mirror_port_vlan_dest_add(unit, gport0, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_add: rv(%d), unit(%d), port(%d), vlan(%d), failed to add port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport0), vlan);
            return rv;
        }
    }

    rv = check_EPNI_MIRROR_VID_REG_state(unit, 100);
    if (rv) {
        return rv;
    }

    vlan = 200;
    if (found_port_cores[1]) {
        rv = bcm_mirror_port_vlan_dest_add(unit, gport1, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_add: rv(%d), unit(%d), port(%d), failed to add port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport1));
            return rv;
        }
    }
    if (found_port_cores[0]) {
        rv = bcm_mirror_port_vlan_dest_add(unit, gport0, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv != BCM_E_NONE) {
            printf("bcm_mirror_port_vlan_dest_add: rv(%d), unit(%d), port(%d), failed to add port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport0));
            return rv;
        }
    }

    rv = check_EPNI_MIRROR_VID_REG_state(unit, 0xc8064);
    if (rv) {
        return rv;
    }

    vlan = 100;
    if (found_port_cores[0]) {
        rv = bcm_mirror_port_vlan_dest_delete(unit, gport0, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id);
        if (rv) {
            printf("bcm_mirror_port_vlan_dest_delete: rv(%d), unit(%d), port(%d), vlan(%d), failed to delete port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport0), vlan);
            return rv;
        }
    }
    if (found_port_cores[1]) {
        rv = bcm_mirror_port_vlan_dest_delete(unit, gport1, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id); 
        if (rv) {
            printf("bcm_mirror_port_vlan_dest_delete: rv(%d), unit(%d), port(%d), vlan(%d), failed to delete port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport1), vlan);
            return rv;
        }
    }
    vlan = 200;
    if (found_port_cores[0]) {
        rv = bcm_mirror_port_vlan_dest_delete(unit, gport0, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id);
        if (rv) {
            printf("bcm_mirror_port_vlan_dest_delete: rv(%d), unit(%d), port(%d), vlan(%d), failed to delete port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport0), vlan);
            return rv;
        }
    }
    if (found_port_cores[1]) {
        rv = bcm_mirror_port_vlan_dest_delete(unit, gport1, vlan, BCM_MIRROR_PORT_EGRESS, mdst.mirror_dest_id);
        if (rv) {
            printf("bcm_mirror_port_vlan_dest_delete: rv(%d), unit(%d), port(%d), vlan(%d), failed to delete port mirroring.\n",rv, unit, BCM_GPORT_LOCAL_GET(gport1), vlan);
            return rv;
        }
    }
    rv = bcm_mirror_destination_destroy(unit, mdst.mirror_dest_id);
    if (rv) {
        printf("bcm_mirror_destination_destroy: rv(%d), unit(%d), delete mirror destination.\n",rv, unit);
        return rv;
    }
    return BCM_E_NONE;
}

int run_snoop_basic_operation(int unit, int destination_id, int encap_id) {
    uint32 set_trap_code = 0x10;
    uint32 rv = BCM_E_NONE;
    char cmd[300] = {0};
    bcm_gport_t mirror_dest_id;
    bcm_mirror_destination_t snoop_dest;

    bcm_mirror_destination_t_init(&snoop_dest);

    BCM_GPORT_MIRROR_SNOOP_SET(snoop_dest.mirror_dest_id, destination_id);
    snoop_dest.flags = BCM_MIRROR_DEST_IS_SNOOP | BCM_MIRROR_DEST_WITH_ID | BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;

    snoop_dest.packet_copy_size = 128;
    snoop_dest.encap_id = encap_id;

    snoop_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING | BCM_MIRROR_PKT_HEADER_UPDATE_PRIO | BCM_MIRROR_PKT_HEADER_UPDATE_COLOR;
    snoop_dest.packet_control_updates.prio = 1;
    snoop_dest.packet_control_updates.header_fabric.internal_valid = 1;
    snoop_dest.packet_control_updates.header_fabric.internal.trap_id = -1;

    rv = bcm_mirror_destination_create(unit, &snoop_dest);
    if (rv != BCM_E_NONE) {
         printf("bcm_mirror_destination_create failed:%s\n", bcm_errmsg(rv));
        return rv;
    }

    sprintf(cmd, "mod IPT_ARAD_SNP_MIR_STAMP_VALUES %d 1 PPH_FHEI_EXT=%d", (destination_id + 16), set_trap_code);
    bshell(unit,cmd);

    mirror_dest_id = snoop_dest.mirror_dest_id;
    bcm_mirror_destination_t_init(&snoop_dest);
    rv = bcm_mirror_destination_get(unit, mirror_dest_id, &snoop_dest);
    if (rv != BCM_E_NONE) {
        printf("bcm_mirror_destination_get failed:%s\n", bcm_errmsg(rv));
        return rv;
    }

    if (set_trap_code == snoop_dest.packet_control_updates.header_fabric.internal.trap_id) {
        printf("There is error IPT_ARAD_SNP_MIR_STAMP_EN when use API bcm_mirror_destination_create.\n");
        return BCM_E_FAIL;
    }

    rv = bcm_mirror_destination_destroy(unit, mirror_dest_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_mirror_destination_destroy failed:%s\n", bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}
