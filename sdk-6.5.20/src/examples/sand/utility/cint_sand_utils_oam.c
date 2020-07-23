/* $Id: cint_sand_utils_oam.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * OAM utility functions
 */

/* The cint  cint_queue_tests.c  must be included: used to get core from port*/

/* Enum signifying the device type. Should be ordered oldest device to newest*/
enum device_type_t {
    device_type_arad_a0 = 0,
    device_type_arad_plus = 1,
    device_type_jericho = 2,
    device_type_jericho_b = 3,
    device_type_jericho_plus = 4,
    device_type_qax = 5,
    device_type_qux = 6,
    device_type_jericho2 = 7,
    device_type_jericho2c = 8,
    device_type_q2a = 9,
    device_type_jericho2p = 10,
    device_type_q2u = 11
};

device_type_t device_type;

/* Endpoint info */
struct oam__ep_info_s {
    int id;
    int rmep_id;
    bcm_gport_t gport;
    bcm_gport_t mpls_out_gport;
    int out_lif;
    int label;
    int lm_counter_base_id;
    int lm_counter_if;
};

int NUMBER_OF_TPID = 2;
uint16 jr2_tpid[NUMBER_OF_TPID] = {0x8100, 0x9100};

int oam__device_type_get(int unit, device_type_t *device_type) {
    bcm_info_t info;
    int device_id_masked;
    int device_id_masked_j2c;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        print rv;
        return rv;
    }

    device_id_masked = (info.device & 0xfff0);
    device_id_masked_j2c = (info.device & 0xffc0);

    *device_type =
            (info.device == 0x8650 && info.revision == 0) ? device_type_arad_a0 :
            (info.device == 0x8660) ? device_type_arad_plus :
            (((info.device == 0x8375) || (info.device == 0x8675)) && info.revision == 0x11) ? device_type_jericho_b :
            ((info.device == 0x8375) || (info.device == 0x8675)) ? device_type_jericho :
            (info.device == 0x8470) ? device_type_qax : (info.device == 0x8680) ? device_type_jericho_plus :
            (info.device == 0x8270) ? device_type_qux : (device_id_masked == 0x8690) ? device_type_jericho2 :
            (device_id_masked_j2c == 0x8800) ? device_type_jericho2c : (device_id_masked == 0x8480) ? device_type_q2a :
            (device_id_masked == 0x8850) ? device_type_jericho2p : ((device_id_masked == 0x8280) ? device_type_q2u : -1);
            

    return rv;
}

/*
 * create vlan-ports
 *   vlan-port: is Logical interface identified by (port-vlan-vlan).
 */
int
oam__vswitch_metro_add_port(int unit, int port_num,  bcm_gport_t *port_id, int flags){
    int rv;
    int base = port_num * 4;
    bcm_vlan_port_t vp1;
    int is_jericho2 = 0;
    bcm_vlan_port_t_init(&vp1);

    if (is_device_or_above(unit, JERICHO2))
    {
        is_jericho2 = 1;
    }

    /*
     * ring protection parameters
     */
    vp1.group = vswitch_metro_mp_info.flush_group_id[port_num];
    vp1.ingress_failover_id = vswitch_metro_mp_info.ingress_failover_id[port_num];
    vp1.failover_port_id = 0;
    vp1.egress_failover_id = vswitch_metro_mp_info.egress_failover_id[port_num];
    vp1.egress_failover_port_id = 0;

    /* the match criteria is port:1, out-vlan:10,  in-vlan:20 */
    vp1.criteria = single_vlan_tag ? BCM_VLAN_PORT_MATCH_PORT_VLAN : BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    if (is_jericho2) {
        /** In OAM we usually use tpid 0x8100. Since 0x8100 is CVLAN indicate it for application */
        vp1.criteria = single_vlan_tag ? BCM_VLAN_PORT_MATCH_PORT_CVLAN : vp1.criteria;
    }
    vp1.port = vswitch_metro_mp_info.sysports[port_num];
    vp1.match_vlan = vswitch_metro_mp_info.p_vlans[base + 0];
    vp1.match_inner_vlan = single_vlan_tag ? 0 : vswitch_metro_mp_info.p_vlans[base + 1];
    /* when packet forwarded to this port then it will be set with out-vlan:100,  in-vlan:200 */
    if (is_device_or_above(unit, JERICHO2))
    {
        vp1.egress_vlan = 0;
        vp1.egress_inner_vlan = 0;
    }
    else
    {
        vp1.egress_vlan = vswitch_metro_mp_info.p_vlans[base + 2];
        vp1.egress_inner_vlan = single_vlan_tag ? 0 : vswitch_metro_mp_info.p_vlans[base + 3];
    }

    vp1.flags = flags;
    vp1.vlan_port_id = *port_id;
    /* this is relevant only when get the gport, not relevant for creation */
    /* will be pupulated when the gport is added to service, using vswitch_port_add */
    vp1.vsi = 0;
    rv = bcm_vlan_port_create(unit,&vp1);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }
    /* egress_vlan and egress_inner_vlan are used at eve */
    vp1.egress_vlan = vswitch_metro_mp_info.p_vlans[base + 2];
    vp1.egress_inner_vlan = single_vlan_tag ? 0 : vswitch_metro_mp_info.p_vlans[base + 3];

    /* save the created gport */
    *port_id = vp1.vlan_port_id;

    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode || is_jericho2) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vp1);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    }
    if(verbose >= 1){
        printf("Add vlan-port-id:0x%08x in-port:0x%08x match_vlan:0x%08x match_inner_vlan:0x%08x in unit %d\n",vp1.vlan_port_id, vp1.port, vp1.match_vlan, vp1.match_inner_vlan, unit);
    }


  return rv;
}


/**
 * Part of vswitch initialization process.
 *
 *
 * @param unit
 * @param known_mac_lsb
 * @param known_vlan
 *
 * @return int
 */
int
oam__vswitch_metro_run(int unit, int known_mac_lsb, int known_vlan){
    int rv;
    /*bcm_vlan_t  vsi*/;
    bcm_mac_t kmac;
    int index, index2;
    bcm_vlan_t kvlan;
    int flags, i, found = 0, found_no = 0;
    bcm_switch_tpid_info_t tpid_info_arry[7];
    int tpid_info_arry_count = -1;

    kmac[5] = known_mac_lsb;
    kvlan = known_vlan;
    device_type_t device_type;

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error oam__device_type_get.\n");
        print rv;
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        /* get all configured tpids */
        rv = bcm_switch_tpid_get_all(unit, 7, &tpid_info_arry, &tpid_info_arry_count);
        if (rv != BCM_E_NONE) {
            printf("ERROR! bcm_switch_tpid_get_all: return code is %d. Expected return code is %d \n", rv, BCM_E_NONE);
        }

        /* check if all required tpid are configured */
        for (index = 0; index < NUMBER_OF_TPID; index++)
        {
            found = 0;
            for (index2 = 0; index2 < tpid_info_arry_count; index2++) {
                if (jr2_tpid[index] == tpid_info_arry[index2].tpid_value) {
                    found = 1;
                    found_no++;
                    break;
                }
            }
            if (found == 0){
                break;
            }
        }

        /* configure new tpid if required */
        if (found_no != NUMBER_OF_TPID) {
            /* Jr2 - use "new" sand Cints for TPID*/
            rv = tpid__tpids_clear_all(unit);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in tpid__tpids_clear_all\n");
                return rv;
            }

            for (index = 0; index < NUMBER_OF_TPID; index++)
            {
                rv = tpid__tpid_add(unit, jr2_tpid[index]);

                if (rv != BCM_E_NONE)
                {
                    printf("Error, in tpid__tpid_add(tpid_value=0x%04x)\n", jr2_tpid[index]);
                    return rv;
                }
            }
        }
    } else
    {
        /* Jer1: Use existing DPP cints.
           Set ports to identify double tags packets */
        port_tpid_init(vswitch_metro_mp_info.sysports[0], 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, port_tpid_set, in unit %d\n", unit);
            print rv;
            return rv;
        }

        port_tpid_init(vswitch_metro_mp_info.sysports[1], 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, port_tpid_set\n");
            print rv;
            return rv;
        }

        port_tpid_init(vswitch_metro_mp_info.sysports[2], 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, port_tpid_set\n");
            print rv;
            return rv;
        }
    }



    /* When using new vlan translation mode, tpid and vlan actions and mapping must be configured manually */
    if (advanced_vlan_translation_mode || (device_type >= device_type_jericho2)) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    for (index = 0; index < 12; index++) {
        if (single_vlan_tag && (((index % 2) != 0))) {
            continue;
        }
        rv = vlan__init_vlan(unit,vswitch_metro_mp_info.p_vlans[index]);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("Error, vlan__init_vlan\n");
            print rv;
            return rv;
        }
    }

    /* 1. create vswitch + create Multicast for flooding
     * If vsi defined create with id*/
    rv = vswitch_create(unit, &vsi, vsi!=0 ? 1 : 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_create\n");
        print rv;
        return rv;
    }
    printf("Created vswitch vsi=0x%x\n", vsi);

    /* 2. create first vlan-port */

    flags = 0;
    gport1 = 0;
    rv = oam__vswitch_metro_add_port(unit, 0, &gport1, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, oam__vswitch_metro_add_port gport1\n");
        print rv;
        return rv;
    }
    if(verbose){
        printf("created vlan-port   0x%08x in unit %d\n",gport1, unit);
    }

    /* 3. add vlan-port to the vswitch and multicast */
    rv = vswitch_add_port(unit, vsi,vswitch_metro_mp_info.sysports[0], gport1);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }
    /* add another port to the vswitch */

    /* 4. create second vlan-port */
    flags = 0;
    gport2 = 0;
    rv = oam__vswitch_metro_add_port(unit, 1, &gport2, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, oam__vswitch_metro_add_port gport2\n");
        return rv;
    }
    if(verbose){
        printf("created vlan-port   0x%08x\n\r",gport2);
    }

    /* 5. add vlan-port to the vswitch and multicast */

    /* Local unit for sysport2 is already first */
    rv = vswitch_add_port(unit, vsi,vswitch_metro_mp_info.sysports[1], gport2);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }

    /* add a third port to the vswitch */

    /* 6. create third vlan-port */
    flags = 0;
    gport3 = 0;
    rv = oam__vswitch_metro_add_port(unit, 2, &gport3, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_add_port_2\n");
        return rv;
    }
    if(verbose){
        printf("created vlan=port   0x%08x in unit %d\n",gport3, unit);
    }

    /* 7. add vlan-port to the vswitch and multicast */
    rv = vswitch_add_port(unit, vsi,vswitch_metro_mp_info.sysports[2], gport3);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }

    rv = vswitch_add_l2_addr_to_gport(unit, gport3, kmac, kvlan);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_l2_addr_to_gport\n");
        return rv;
    }

    return rv;
}

/**
 * Create vlan-port and add it
 * to vswitch and multicast group
 *
 * @param unit
 * @param port
 * @param vlan
 *
 * @return int
 */
int oam_vswitch_gport_add(int unit, bcm_port_t port, int vlan)
{
    bcm_error_t rv;
    device_type_t device_type;
    gport4 = 0;
    bcm_vlan_port_t vp;
    bcm_vlan_port_t_init(&vp);

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0)
    {
        printf("Error oam__device_type_get.\n");
        print rv;
        return rv;
    }

    /* Create vlan-port */
    if (device_type >= device_type_jericho2)
    {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    }
    else
    {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }
    vp.port = port;
    vp.match_vlan = vlan;
    vp.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : vlan;
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }

    gport4 = vp.vlan_port_id;
    if(verbose)
    {
        printf("Created vlan-port 0x%08x in unit %d\n", gport4, unit);
    }

    /* Add vlan-port to vswitch */
    rv = bcm_vswitch_port_add(unit, vsi, gport4);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    if(verbose)
    {
        printf("Added port 0x%08x to vswitch 0x%08x\n", gport4, vsi);
    }

    /* Add vlan-port to multicast group */
    rv = multicast__gport_encap_add(unit, vsi, port, gport4, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multicast__gport_encap_add\n");
        return rv;
    }

    if(verbose)
    {
        printf("Added port 0x%08x to multicast group 0x%08x in unit %d\n", gport4, vsi, unit);
    }

    return rv;
}

/**
 * Function that demonstrate setting of my cfm mac
 *
 * @param unit
 * @param src_port
 * @param my_cfm_mac
 *
 * @return int
 */
int oam__my_cfm_mac_set(int unit, bcm_gport_t src_port, bcm_mac_t my_cfm_mac)
{
     bcm_l2_station_t station;
     int station_id;
     int rv;
     /* Intitialize an L2 station structure */
     bcm_l2_station_t_init(&station);

     station.flags = BCM_L2_STATION_OAM;
     sal_memcpy(station.dst_mac, my_cfm_mac, 6);
     sal_memset(station.dst_mac_mask, 0xff, 6);
     station.src_port = src_port;
     station.src_port_mask = -1;

     rv = bcm_l2_station_add(unit, &station_id, &station);
     if (rv != BCM_E_NONE) {
        printf("bcm_l2_station_add  %d \n", station_id);
        return rv;
    }

    return rv;
}

/**
 * Procedure used to set a port as CPU port.
 * In case additional CPU ports are required for OAM handling.
 *
 *
 * @param unit
 * @param port
 *
 * @return int
 */
int
oam__switch_contol_indexed_port_set_to_cpu(int unit, bcm_port_t port){

    int rv = BCM_E_NONE;
    bcm_switch_control_key_t port_key_info;
    bcm_switch_control_info_t port_value_info;

    port_key_info.type = bcmSwitchPortHeaderType;
    /* CPU destination is supported on outgoing direction only */
    port_key_info.index = 2; 
    /* Set the port as CPU to allow system headers */
    port_value_info.value = BCM_SWITCH_PORT_HEADER_TYPE_CPU; 

    rv = bcm_switch_control_indexed_port_set(unit,port,port_key_info,port_value_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_switch_control_indexed_port_set\n");
        return rv;
    }

    return rv;
}

/**
 * Initialize vswitch for OAM example usage.
 *
 *
 * @param unit
 *
 * @return int
 */
int create_vswitch_and_mac_entries(int unit) {
    bcm_error_t rv;
    int flags;

    printf("Creating vswitch in unit %d\n", unit);
    bcm_port_class_set(unit, port_1, bcmPortClassId, port_1);
    bcm_port_class_set(unit, port_2, bcmPortClassId, port_2);
    bcm_port_class_set(unit, port_3, bcmPortClassId, port_3);

    vswitch_metro_mp_info_init(port_1, port_2, port_3);

    rv = oam__vswitch_metro_run(unit, 0xce, 3);
    if (rv != BCM_E_NONE){
      printf("Error, in oam__vswitch_metro_run\n");
      return rv;
    }

    printf("Adding mep MAC addresses to MAC table\n");
    /* int l2_entry_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan, int dest_type, int dest_gport, int dest_mc_id){*/
    rv = l2_entry_add(unit, mac_mep_1, vsi, 0, gport1, 0);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = l2_entry_add(unit, mac_mep_2, vsi, 0, gport2, 0);
    if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }

    printf("Creating multicast group\n");
    flags =  BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID;
    rv = bcm_multicast_create(unit, flags, mc_group);
    if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }

    if (!is_device_or_above(unit, JERICHO2))
    {
        rv = bcm_multicast_ingress_add(unit, mc_group, port_1, 0);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    else
    {
        bcm_multicast_replication_t rep_array;
        int flags;

        flags = BCM_MULTICAST_INGRESS_GROUP;
        bcm_multicast_replication_t_init(&rep_array);

       rep_array.port = port_1;
       rep_array.encap1 = 0;
       rv = bcm_multicast_add(unit, mc_group, flags, 1, &rep_array);

    }

    if (!is_device_or_above(unit, JERICHO2))
    {
        /* Adding MC mac address of mep 2 to the multicast group */
        rv = l2_entry_add(unit, mac_mep_2_mc, vsi, 1, 0, mc_group);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

/**
 *
 *
 * @author sinai (22/07/2015)
 *
 * @param unit
 * @param seconds
 * @param frac_seconds
 * @param using_1588
 *
 * @return int
 */
int set_oam_tod(int unit, int seconds, int frac_seconds, int using_1588) {
    uint64 time;
    bcm_oam_control_type_t control_var = using_1588? bcmOamControl1588ToD : bcmOamControlNtpToD;

    COMPILER_64_SET(time, seconds, frac_seconds);
    return bcm_oam_control_set(unit,control_var,time );
}

