
/*
 * $id cint_field_reflector_1pass.c $
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RFC2544 1-Pass Reflector:
 * This is an example of how to use a single pass reflector with PMF programmable
 * selection. This feature uses ingress PMF to identify and trap the reflected
 * packets, egress PMF to identify these trapped packets and enable the DA/SA
 * swapping before the packet leaves back to the port from which it arrived.
 *
 * This feature requires the soc property
 * custom_feature_rfc2544_reflector_mac_swap_1pass=1
 *
 * In order to use this example, call:
 * > cint utility/cint_utils_field.c
 * > cint cint_field_programmable_reflector.c
 * > cint
 * > reflector_1pass_example(0, port, vlan);
 *
 * Packets sent to the egress device with Outer-VLAN=123 would be echoed
 */


/* Create a trap with no action. Used by the ingress PMF to
   signal to the egress PMF that the packet is reflected */
int signaling_trap_create(int unit, int *trap_id) {

    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;

    /* create trap */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_type_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* configure trap attribute */
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_TRAP;
    trap_config.trap_strength = 0;

    rv = bcm_rx_trap_set(unit, *trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    printf("Trap created with id: 0x%02x\n",*trap_id);

    return BCM_E_NONE;
}

/*
 * Create Egress field processor rule
 * Key: Port+Trap-code
 * Action: Enable DA/SA swap editing
 */
int reflector_1pass_egress_rule_create(int unit, bcm_port_t port, int trap_id){
    int rv = BCM_E_NONE;
    bcm_field_group_t grp_tcam = 1;
    int group_priority = 10;
    int qual_id = 1;

    /* Create field group for OAM EndTLV */
    rv = field__prge_action_create_field_group(unit, group_priority, grp_tcam, qual_id);
    if (rv != BCM_E_NONE) {
        printf("Error in field__prge_action_create_field_group (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Set action code for the created group */
    printf("Adding entry to mark packets for reflection\n");
    rv = field__prge_action_code_add_entry(unit, grp_tcam, trap_id, port,
                                    qual_id, prge_action_code_reflector_1pass);
    if (rv != BCM_E_NONE) {
        printf("Error in field__prge_action_code_add_entry (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * Create Ingress field processor rule
 * Key: SrcPort+VLAN
 * Action: Redirect back to SrcPort and trap
 */
int reflector_1pass_ingress_rule_create(int unit, bcm_port_t port, int vlan, int trap_code){

    int rv = BCM_E_NONE;

    /* Create ingress rule to trap and redirect packets on (port,vlan) */
    rv = field__redirect_trap_ingress_rule_create(unit,
                                                  port/* SrcPort */, vlan,
                                                  port/* DstPort */, trap_code, 7, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in field__redirect_trap_ingress_rule_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * reflector_1pass_example
 *
 * main for this example. Sets up a 1-pass reflector for packets
 * arriving on <port,vlan>.
 */
int reflector_1pass_example(int unit, bcm_port_t port, int vlan) {

    int rv = BCM_E_NONE;
    int trap_id = -1;

    /* Disable same-interface filter */
    rv = bcm_port_control_set(unit, port, bcmPortControlBridge, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_control_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* in case of ceragon RH, need to set port with ceragon pp profile */
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ch_enable", 0)) 
    {
        rv = bcm_port_control_set(unit,port,bcmPortControlReserved280,1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_control_set $rv\n");
            return rv;
        }
    }

    /* Create a trap for the ingress to use and the egress to identify */
    rv = signaling_trap_create(unit, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in signaling_trap_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Create egress rule to enable DA/SA swapping */
    rv = reflector_1pass_ingress_rule_create(unit, port, vlan, trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in reflector_1pass_ingress_rule_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Create egress rule to enable DA/SA swapping */
    rv = reflector_1pass_egress_rule_create(unit, port, trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in reflector_1pass_egress_rule_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}
