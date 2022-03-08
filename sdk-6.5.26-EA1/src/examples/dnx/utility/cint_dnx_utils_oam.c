/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_dnx_utils_oam.c Purpose: OAM utility functions.
 */

/**
 * Create cross-connected VLAN ports
 *
 * @param unit
 * @param port1
 * @param port2
 * @param vid1
 * @param vid2
 * @param vlan_port_id_1
 * @param vlan_port_id_2
 *
 * @return int
 */
int dnx_oam_cross_connected_vlan_ports_create(
    int unit,
    bcm_port_t port1,
    bcm_port_t port2,
    bcm_vlan_t vid1,
    bcm_vlan_t vid2,
    bcm_gport_t* vlan_port_id_1,
    bcm_gport_t* vlan_port_id_2)
{
    bcm_error_t rv;
    bcm_vlan_port_t vp1;
    bcm_vlan_port_t vp2;
    bcm_vswitch_cross_connect_t cross_connect;

    if(nof_mep == 0)
    {
        /* Set port classification ID */
        rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_port_class_set.\n", rv);
            return rv;
        }

        /* Set port classification ID */
        rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_port_class_set.\n", rv);
            return rv;
        }
    }
    /* Create VLAN ports */
    bcm_vlan_port_t_init(&vp1);
    vp1.flags = 0;
    vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vp1.port = port1;
    vp1.match_vlan = vid1;
    rv = bcm_vlan_port_create(unit,&vp1);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_vlan_port_create.\n", rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vid1, port1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_vlan_port_create.\n", rv);
        return rv;
    }

    printf("port=%d, vid=%d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vp1.port, vp1.match_vlan,
           vp1.vlan_port_id, vp1.encap_id);

    *vlan_port_id_1 = vp1.vlan_port_id;

    bcm_vlan_port_t_init(&vp2);
    vp2.flags = 0;
    vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vp2.port = port2;
    vp2.match_vlan = vid2;
    rv = bcm_vlan_port_create(unit,&vp2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, vid2, port2, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_vlan_gport_add.\n", rv);
        return rv;
    }

    printf("port=%d, vid=%d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vp2.port, vp2.match_vlan,
           vp2.vlan_port_id, vp2.encap_id);

    *vlan_port_id_2 = vp2.vlan_port_id;

    /**
     * Cross-connect the ports
     */
    bcm_vswitch_cross_connect_t_init(&cross_connect);
    cross_connect.port1 = vp1.vlan_port_id;
    cross_connect.port2 = vp2.vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect);
    if (rv != BCM_E_NONE) {
        printf("Error no %d, in bcm_vswitch_cross_connect_add.\n", rv);
        return rv;
    }

    return rv;
}

/*
 * For MPLS OAM in egress
 * Update counter in egress profile
 *
 * For egress counter for mpls oam injected packet,
 * 1) Counter should always be enabled in non acc egress profile by calling this function
 * 2) Users can control whether the packet should be counted, by setting reserved egress acc profile.
 *    Please refer to appl_dnx_oam_hlm_egress_with_update_counter_in_egr_acc_profile
 *
 */
int
dnx_oam_egress_with_update_counter_in_egr_profile (
    int unit,
    int profile_id,
    int opcode,
    int counter_enable)
{
    bcm_error_t rv;
    uint32 flags = 0;
    bcm_oam_action_key_t    oam_action_key;
    bcm_oam_action_result_t oam_action_result;
    oam_action_key.dest_mac_type = bcmOAMDestMacTypeUknownUcast;
    oam_action_key.endpoint_type = bcmOAMMatchTypePassive;
    oam_action_key.inject = 1;
    oam_action_key.opcode = opcode;

    /*
     * Get original profile configuration
     */
    rv = bcm_oam_profile_action_get(unit, flags, profile_id, &oam_action_key, &oam_action_result);
    if (rv != BCM_E_NONE) {
        printf("Error rv, in bcm_oam_profile_action_get\n");
        return rv;
    }

    oam_action_result.counter_increment = counter_enable;
    oam_action_result.destination = BCM_GPORT_INVALID;

    /*
     * Set new profile configuration
     */
    rv = bcm_oam_profile_action_set(unit, flags, profile_id, &oam_action_key, &oam_action_result);
    if (rv != BCM_E_NONE) {
        printf("Error rv, in bcm_oam_profile_action_set\n");
        return rv;
    }


    printf("set counter enable(%d) for profile(%d)\n", counter_enable, profile_id);

    return rv;
}

/*
 * For MPLS OAM in egress
 * Speculate CW protocol after the MPLS stack
 * This is needed to identify y1731 oam over pwe in egress parser
 */
int
dnx_oam_identify_y1731_oam_over_pwe_egress (
    int unit)
{
    bcm_error_t rv;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    /*
     * Speculate CW protocol aftre the MPLS stack
     */
    key.type = bcmSwitchMplsSpeculativeNibbleMap;
    key.index = 1;
    value.value = bcmSwitchMplsNextProtocolControlWord;
    rv = bcm_switch_control_indexed_set (unit, key, value);
    if (rv != BCM_E_NONE) {
        printf("Error rv, in bcm_switch_control_indexed_set\n");
        return rv;
    }

    return rv;
}

/*
 * For native OAM, in order to avoid it's trapped by MPLS-TP in egress
 * User should update destination in egress Non acc profile for oam match type Passive and LowMDL for MPLS-TP MEP
 *
 * Following function provide an example of how to set destination. 
 *
 * Native oam may be trapped by passive component of MPLS-TP MEP, if below requirements are meet
 * 1) Native OAM are sent to outlif on which MPLS-TP MEP is created
 * 2) MDL of native oam is lower than MDL of MPLS-TP MEP
 * 3) Destination in egress profile for MPLS-TP MEP is a trap
 *
 */
int
dnx_oam_native_oam_to_mpls_oam_fwd_dest_in_egr_profile_set (
    int unit,
    int profile_id)
{
    bcm_error_t rv;
    uint32 flags = 0;

    bcm_oam_action_key_t    oam_action_key;
    bcm_oam_action_result_t oam_action_result;

    bcm_oam_dest_mac_type_t dest_mac_types[] = {bcmOAMDestMacTypeMcast, bcmOAMDestMacTypeMyCfmMac, bcmOAMDestMacTypeUknownUcast};
    bcm_oam_match_type_t endpoint_types[] = {bcmOAMMatchTypePassive, bcmOAMMatchTypeLowMDL};
    bcm_oam_opcode_t supported_opcodes[] = {
        bcmOamOpcodeCCM,
        bcmOamOpcodeLBR,
        bcmOamOpcodeLBM,
        bcmOamOpcodeLTR,
        bcmOamOpcodeLTM,
        bcmOamOpcodeAIS,
        bcmOamOpcodeLCK,
        bcmOamOpcodeLinearAPS,
        bcmOamOpcodeLMR,
        bcmOamOpcodeLMM,
        bcmOamOpcode1DM,
        bcmOamOpcodeDMR,
        bcmOamOpcodeDMM,
        bcmOamOpcodeSLR,
        bcmOamOpcodeSLM
    };

    int dest_mac_type_idx = 0;
    int nof_dest_mac_types;
    bcm_oam_dest_mac_type_t dest_mac_type_tmp;
    int opcode_idx = 0;
    int nof_supported_opcodes;
    bcm_oam_opcode_t opcode_tmp;
    int endpoint_type_idx=0;
    int nof_endpoint_types;
    bcm_oam_match_type_t endpoint_type_tmp;

    nof_dest_mac_types = sizeof(dest_mac_types) / sizeof(dest_mac_type_tmp);
    nof_supported_opcodes = sizeof(supported_opcodes) / sizeof(opcode_tmp);
    nof_endpoint_types = sizeof(endpoint_types) / sizeof(endpoint_type_tmp);

    for (dest_mac_type_idx = 0; dest_mac_type_idx < nof_dest_mac_types; dest_mac_type_idx++)
    {
        oam_action_key.dest_mac_type = dest_mac_types[dest_mac_type_idx];
        for (opcode_idx = 0; opcode_idx < nof_supported_opcodes; opcode_idx++)
        {
            oam_action_key.opcode = supported_opcodes[opcode_idx];
            for (endpoint_type_idx = 0; endpoint_type_idx < nof_endpoint_types; endpoint_type_idx++)
            {
                oam_action_key.endpoint_type = endpoint_types[endpoint_type_idx];
                oam_action_key.inject = 0;

                /*
                 * Get original profile configuration
                 */
                rv = bcm_oam_profile_action_get(unit, flags, profile_id, &oam_action_key, &oam_action_result);
                if (rv != BCM_E_NONE) {
                    printf("Error rv, in bcm_oam_profile_action_get\n");
                    return rv;
                }

                /* Update destination */
                oam_action_result.destination = BCM_GPORT_INVALID;

                /*
                 * Set new profile configuration
                 */
                rv = bcm_oam_profile_action_set(unit, flags, profile_id, &oam_action_key, &oam_action_result);
                if (rv != BCM_E_NONE) {
                    printf("Error rv, in bcm_oam_profile_action_set\n");
                    return rv;
                }
            }
        }
    }

    printf("Updated destination for egress profile(%d)\n", profile_id);

    return rv;
}

/**
 * Procedure used to set a port
 * In case CPU/RCY ports are required for OAM handling.
 *
 * @param unit
 * @param port
 * @param port_header_type
 *             indicates the port header type value according to BCM_SWITCH_PORT_HEADER_TYPE_XXX
 * @param direction
 *         '0' - both directions (in/out)
 *         '1' - 'in'
 *         '2' - 'out'
 * @return int
 */
int
dnx_oam__switch_contol_indexed_port_set(int unit, bcm_port_t port, int port_header_type, int direction){

    int rv = BCM_E_NONE;
    bcm_switch_control_key_t port_key_info;
    bcm_switch_control_info_t port_value_info;

    port_key_info.type = bcmSwitchPortHeaderType;
    port_key_info.index = direction;
    port_value_info.value = port_header_type;

    rv = bcm_switch_control_indexed_port_set(unit,port,port_key_info,port_value_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_switch_control_indexed_port_set\n");
        return rv;
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

/*
 * create vlan-ports
 *   vlan-port: is Logical interface identified by (port-vlan-vlan).
 */
int
oam__vswitch_metro_add_port(int unit, int port_num, bcm_gport_t *port_id, int flags){
    int rv;
    int base = port_num * 4;
    bcm_vlan_port_t vp1;
    bcm_vlan_port_t_init(&vp1);

    /*
     * ring protection parameters
     */
    vp1.group = vswitch_metro_mp_info.flush_group_id[port_num];
    vp1.ingress_failover_id = vswitch_metro_mp_info.ingress_failover_id[port_num];
    vp1.failover_port_id = 0;
    vp1.egress_failover_id = vswitch_metro_mp_info.egress_failover_id[port_num];
    vp1.egress_failover_port_id = 0;

    /** In OAM we usually use tpid 0x8100. Since 0x8100 is CVLAN indicate it for application */
    vp1.criteria = single_vlan_tag ? BCM_VLAN_PORT_MATCH_PORT_CVLAN : BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vp1.port = vswitch_metro_mp_info.sysports[port_num];
    vp1.match_vlan = vswitch_metro_mp_info.p_vlans[base + 0];
    vp1.match_inner_vlan = single_vlan_tag ? 0 : vswitch_metro_mp_info.p_vlans[base + 1];
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

    /* Add VLAN membership */
    rv = bcm_vlan_gport_add(unit, vswitch_metro_mp_info.p_vlans[base + 0], vswitch_metro_mp_info.sysports[port_num], 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add \n");
        return rv;
    }

    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    printf("Set VLAN translation\n");
    rv = vlan_translation_vlan_port_create_to_translation(unit, &vp1);
    if (rv != BCM_E_NONE) {
        printf("Error: vlan_translation_vlan_port_create_to_translation\n");
    }
    if(verbose >= 1){
        printf("Add vlan-port-id:0x%08x in-port:0x%08x match_vlan:0x%08x match_inner_vlan:0x%08x in unit %d\n",vp1.vlan_port_id, vp1.port, vp1.match_vlan, vp1.match_inner_vlan, unit);
    }

    return rv;
}

/**
 * Part of vswitch initialization process.
 *
 * @param unit
 * @param known_mac_lsb
 * @param known_vlan
 *
 * @return int
 */
int
oam__vswitch_metro_run(int unit, int known_mac_lsb, int known_vlan)
{
    int rv;
    bcm_mac_t kmac;
    int flags, i, index;
    bcm_switch_tpid_info_t tpid_info_arry[7];
    int tpid_info_arry_count = -1;
    kmac[5] = known_mac_lsb;

    bcm_vlan_t vsi = dnx_oam_endpoint_info_glb.vsi;

    /* 1. Create VLANs */
    for (index = 0; index < 12; index++) {
        if (single_vlan_tag && (((index % 2) != 0))) {
            continue;
        }
        rv = bcm_vlan_create(unit, vswitch_metro_mp_info.p_vlans[index]);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create, vlan=%d, \n", vswitch_metro_mp_info.p_vlans[index]);
            return rv;
        }
    }

    /* 2. Create vswitch + create Multicast for flooding
     * If vsi defined create with id*/
    rv = vswitch_create(unit, &vsi, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_create\n");
        print rv;
        return rv;
    }
    printf("Created vswitch vsi=0x%x\n", vsi);

    /* 3. create first vlan-port */
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

    /* 4. Add vlan-port to the vswitch and multicast */
    rv = vswitch_add_port(unit, vsi, vswitch_metro_mp_info.sysports[0], gport1);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }

    /* 5. Create second vlan-port */
    gport2 = 0;
    rv = oam__vswitch_metro_add_port(unit, 1, &gport2, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, oam__vswitch_metro_add_port gport2\n");
        return rv;
    }
    if(verbose){
        printf("created vlan-port   0x%08x\n\r",gport2);
    }

    /* 6. Add vlan-port to the vswitch and multicast */
    rv = vswitch_add_port(unit, vsi,vswitch_metro_mp_info.sysports[1], gport2);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }

    /* 7. Create third vlan-port */
    gport3 = 0;
    rv = oam__vswitch_metro_add_port(unit, 2, &gport3, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_add_port_2\n");
        return rv;
    }
    if(verbose){
        printf("created vlan=port   0x%08x in unit %d\n",gport3, unit);
    }

    /* 8. add vlan-port to the vswitch and multicast */
    rv = vswitch_add_port(unit, vsi,vswitch_metro_mp_info.sysports[2], gport3);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }

    /* 9. Add MACT entry for known unicast */
    rv = vswitch_add_l2_addr_to_gport(unit, gport3, kmac, known_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_l2_addr_to_gport\n");
        return rv;
    }

    /* 10. Configure ingress/egress VLAN actions */
    rv = vlan_translation_default_mode_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_default_mode_init\n");
        return rv;
    }

    return rv;
}

/**
 * Initialize vswitch for OAM example usage.
 *
 * @param unit
 *
 * @return int
 */
int create_vswitch_and_mac_entries(int unit) {
    int rv;
    int flags;
    bcm_mac_t kmac;
    int known_mac_lsb = 0xce;
    int known_vlan = 3;
    bcm_vlan_t vsi = dnx_oam_endpoint_info_glb.vsi;

    printf("Creating vswitch in unit %d\n", unit);
    bcm_port_class_set(unit, dnx_oam_endpoint_info_glb.port_1, bcmPortClassId, dnx_oam_endpoint_info_glb.port_1);
    bcm_port_class_set(unit, dnx_oam_endpoint_info_glb.port_2, bcmPortClassId, dnx_oam_endpoint_info_glb.port_2);
    bcm_port_class_set(unit, dnx_oam_endpoint_info_glb.port_3, bcmPortClassId, dnx_oam_endpoint_info_glb.port_3);

    vswitch_metro_mp_info_init(dnx_oam_endpoint_info_glb.port_1, dnx_oam_endpoint_info_glb.port_2, dnx_oam_endpoint_info_glb.port_3);

    rv = oam__vswitch_metro_run(unit, known_mac_lsb, known_vlan);
    if (rv != BCM_E_NONE){
        printf("Error, in oam__vswitch_metro_run\n");
        return rv;
    }

    printf("Adding mep MAC addresses to MAC table\n");
    rv = vswitch_add_l2_addr_to_gport(unit, gport1, dnx_oam_endpoint_info_glb.mac_mep_1, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_add_l2_addr_to_gport\n");
        return rv;
    }

    rv = vswitch_add_l2_addr_to_gport(unit, gport2, dnx_oam_endpoint_info_glb.mac_mep_2, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_add_l2_addr_to_gport\n");
        return rv;
    }

    return rv;
}

/*
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

int get_oam_tod(int unit, uint64 *time, int using_1588) {
    bcm_oam_control_type_t control_var = using_1588? bcmOamControl1588ToD : bcmOamControlNtpToD;

    return bcm_oam_control_get(unit,control_var,time);
}

