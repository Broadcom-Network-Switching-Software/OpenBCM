/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* Configure an entry in ESEM: outlif x VSI -> native AC-LIF
   outlif must be a L2LIF
   Should be used by PWE tagged mode */
int native_egress_vlan_editing_lif_vsi_lookup(
   int         unit,
   bcm_gport_t l2lif_port,
   int         vsi,
   bcm_gport_t native_vlan_port
   ) {

    int rv = BCM_E_NONE;

    bcm_port_match_info_t port_match_info;
    bcm_port_match_info_t_init(&port_match_info);

    port_match_info.flags = BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY; /* indicate that it's for native VE */
    if (is_device_or_above(unit, JERICHO2)) {
        /* Only BCM_PORT_MATCH_PORT is supported in Jericho2 */
        port_match_info.match = BCM_PORT_MATCH_PORT;
    } else {
        port_match_info.match = BCM_PORT_MATCH_PORT_VPN; /* indicate that lookup fields are port and VPN */
        port_match_info.vpn = vsi;
    }

    port_match_info.port = l2lif_port;

    rv = bcm_port_match_add(unit, native_vlan_port, &port_match_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_match_add \n ");
    }

    printf("entry in ESEM: local outlif (global outlif is: 0x%x) x VSI (%d) -> native AC-LIF (0x%x) \n", l2lif_port, vsi, native_vlan_port);

    return rv;

}

/* add a native AC LIF, then configure the ESEM lookup outlif x VSI -> native AC-LIF */
int native_egress_vlan_editing_lif(
   int         unit,
   bcm_gport_t l2lif_port,
   int         vsi
   ) {

    int rv = BCM_E_NONE;

    /* create native out AC LIF */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_NATIVE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    if (is_device_or_above(unit, JERICHO2))
    {
        vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
        vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
    }
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    printf("Native AC created: 0x%x \n", vlan_port.vlan_port_id);

    uint32 ve_profile = 3;
    uint32 outer_vlan = 89;
    /* Inner vlan must be 0 in Jericho2. */
    uint32 inner_vlan = is_device_or_above(unit, JERICHO2) ? 0 : 123;
    /* configure AC-lif.vlans and AC-lif.VE-profile */
    rv = vlan__port_translation__set(unit,
                                     outer_vlan /* outer vlan */,
                                     inner_vlan /* inner vlan */,
                                     vlan_port.vlan_port_id /* ac-lif */,
                                     ve_profile,
                                     0 /* is ingress */);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__port_translation__set\n");
        return rv;
    }

    printf("Native AC (0x%x) configured: outer-vlan: %d, ve profile: %d \n", vlan_port.vlan_port_id, outer_vlan, ve_profile);

    bcm_vlan_action_t outer_action, inner_action;

    /* create vlan edit command */
    outer_action = bcmVlanActionAdd;
    inner_action = bcmVlanActionAdd;
    int action_id; /* Vlan edit command id */
    rv = vlan__avt_vid_action__set(unit,
                                   0 /* is ingress */,
                                   0x8100 /* outer TPID */,
                                   0x9100 /* inner TPID */,
                                   outer_action, inner_action, &action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__port_translation__set\n");
        return rv;
    }

    printf("create vlan edit command (%d) for native AC (0x%x) \n", action_id, vlan_port.vlan_port_id);

    /* map VE profile and tag format to vlan edit command */
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = ve_profile; /* vlan edit profile */
    action_class.vlan_translation_action_id = action_id; /* vlan edit command */
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    bcm_vlan_translate_action_class_set(unit, action_class);

    printf("map ve profile and tag format to vlan edit command (%d) for native AC (0x%x) \n", action_id, vlan_port.vlan_port_id);

    /* configure ESEM lookup: outlif x VSI -> native AC-LIF */
    rv = native_egress_vlan_editing_lif_vsi_lookup(unit, l2lif_port, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, native_egress_vlan_editing_lif_vsi_lookup\n");
        return rv;
    }

    return rv;
}

/* add a native vlan classification */
int native_egress_vlan_tpid_class_set(
   int         unit,
   bcm_gport_t port, uint32 tpid1, uint32 tpid2)
{

    int rv = BCM_E_NONE;
    bcm_port_tpid_class_t tpid_class;
    bcm_switch_tpid_class_t tpid_class_info;

    /*Get the tpid class from port*/
    bcm_switch_tpid_class_t_init(&tpid_class_info);
    if (!is_device_or_above(unit, JERICHO2)) {
        tpid_class_info.match = bcmSwitchTpidClassMatchPort;
        tpid_class_info.port= port;
        rv = bcm_switch_tpid_class_get(unit, &tpid_class_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_tpid_class_get\n");
            return rv;
        }

        printf("\nThe TPID class ID gotten from port: %d is %d\n", port, tpid_class_info.tpid_class_id);
    }

    /*Create native vlan classification*/
    bcm_port_tpid_class_t_init(&tpid_class);
    tpid_class.tpid1 = tpid1;
    tpid_class.tpid2 = tpid2;
    tpid_class.tag_format_class_id = is_device_or_above(unit, JERICHO2) ? 4 : 1;
    tpid_class.tpid_class_id = is_device_or_above(unit, JERICHO2) ? 0 : tpid_class_info.tpid_class_id;
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_class_set\n");
        return rv;
    }

    return rv;
}

