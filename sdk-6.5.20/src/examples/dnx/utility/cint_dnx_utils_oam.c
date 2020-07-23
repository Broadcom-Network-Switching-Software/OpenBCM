/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_utils_oam.c Purpose: OAM utility functions.
 */

/**
 * Create cross-connected VLAN ports
 *
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


