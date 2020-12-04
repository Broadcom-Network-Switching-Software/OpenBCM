/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Example of outbound mirroring and BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED flag
 *
 * Tests scenario:
 *
 * step 1:  Mirroring is disabled
 *          packet enters from port A and exits from port B
 *
 * step 2:  Mirroring is enabled and BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED is not set
 *          packet enters from port A and exits from port B
 *          in addition, mirrored packet exits from port C
 *
 * step 3:  Mirroring is enabled and BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED is set
 *          packet enter from port A and exit from port B
 *          NO mirrored packet exits from port C
 *
 * Example:
 *
 * 1. Traffic is configured from port 200 to port 201
 *
 *                INPUT        ________________________         OUTPUT
 *                            |                        |
 *         port 200, vlan 100 |                        | port 201, vlan 100
 *                            |                        |
 *      --------------------->|                        |----------------------->
 *                            |________________________|
 *
 * 2. Traffic is configured from port 200 to port 201 and port 201 vlan 100 mirrored to port 203
 *    BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED is not set
 *
 *                INPUT        ________________________         OUTPUT
 *                            |                        | port 201, vlan 100
 *         port 200, vlan 100 |  packet is mirrored    |----------------------->
 *                            |  to port 203           |
 *      --------------------->|                        |port 203, vlan 100
 *                            |________________________|----------------------->
 *
 * 3. Traffic is configured from port 200 to port 201 and port 201 vlan 100 mirrored to port 203
 *    BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED is set for port 201
 *
 *                INPUT        ________________________         OUTPUT
 *                            |                        | port 201, vlan 100
 *                   port 200 | packet is not mirrored |----------------------->
 *                   vlan 100 | to port 203            |
 *      --------------------->|                        |
 *                            |________________________|
 *
 *
 * SOC properties:
 * custom_feature_outer_vid_existence_class_mode = 1
 * bcm886xx_vlan_translate_mode = 1
 *
 * CINT files:
 * cd ../../../../src/examples/dpp/
 * cint_utils_l2.c
 * cint_mirror.c
 * cint_utils_vlan.c
 * cint_utils_port.c
 * cint_vlan_mirror_outbound_untagged.c
 *
 * vlan_mirror_outbound_untagged__start_run(int unit, vlan_mirror_outbound_untagged_s *test_data) - test_data = NULL for default params
 */

/*
 * Global Variables Definition and Initialization  START
 */

/*  Main Struct VLAN membership */
struct vlan_mirror_outbound_untagged_s {
    bcm_vlan_t  vsi;
    bcm_vlan_t  vlan;
    int         ports[2];
    bcm_gport_t gport[2];
    bcm_mac_t   dmac;
    int         dt_action_id;
    int         outer_vid_untagged_set;
};

/* Initialization of global struct*/
vlan_mirror_outbound_untagged_s g_vlan_mirror_outbound_untagged = {
    16,                                     /* vsi */
    100,                                    /* vlan */
    {200, 201},                             /* ports */
    {0, 0},                                 /* gport */
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55},   /* dmac */
    0,                                      /* dt_action_id */
    0                                       /* outer_vid_untagged_set */
};

/*
 * Global Variables Definition and Initialization  END
 */

void
vlan_mirror_outbound_untagged__get_struct(vlan_mirror_outbound_untagged_s *test_data)
{
    sal_memcpy(test_data, &g_vlan_mirror_outbound_untagged, sizeof(*test_data));
    return;
}

int
vlan_mirror_outbound_untagged_init(int unit, vlan_mirror_outbound_untagged_s *test_data)
{
    if (test_data != NULL) {
       sal_memcpy(&g_vlan_mirror_outbound_untagged, test_data, sizeof(g_vlan_mirror_outbound_untagged));
    }

    if (soc_property_get(unit, "custom_feature_outer_vid_existence_class_mode", 1) != 1) {
        return BCM_E_CONFIG;
    }

    if (soc_property_get(unit, "bcm886xx_vlan_translate_mode", 1) != 1) {
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

/* Configure Egress VLAN Editing */
int
vlan_mirror_outbound_untagged_eve(int unit)
{
    bcm_vlan_action_set_t action;
    int rv, i;

    bcm_vlan_action_set_t_init(&action);

    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;
    int action_id;

    /* set edit profile for egress LIF */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.gport = g_vlan_mirror_outbound_untagged.gport[1];
    port_trans.vlan_edit_class_id = 4;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /* Create action ID */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }
    g_vlan_mirror_outbound_untagged.dt_action_id = action_id;

    /* Set translation action - do nothing */
    if (g_vlan_mirror_outbound_untagged.outer_vid_untagged_set) {
        action.flags = action.flags | BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED;
    }
    rv = bcm_vlan_translate_action_id_set(unit,
                                          BCM_VLAN_ACTION_SET_EGRESS,
                                          action_id,
                                          &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    /* Set translation action class */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = 4;
    action_class.tag_format_class_id = 5;
    action_class.vlan_translation_action_id = action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;

    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    return rv;
}

/* Configure Ports */
int
vlan_mirror_outbound_untagged_config_ports(int unit)
{
    int i, rv;
    bcm_vlan_port_t vlan_port;

    for (i = 0; i < 2; i++) {
        rv = port__tag_classification__set(unit, g_vlan_mirror_outbound_untagged.ports[i], /*tag_format*/ 5, 0x8100, 0xFFFFFFFF);
        if (rv != BCM_E_NONE) {
            printf("Error, port__tag_classification__set for port - %d, rv - %d\n", g_vlan_mirror_outbound_untagged.ports[i], rv);
            return rv;
        }

        /* Create an AC LIF */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.port = g_vlan_mirror_outbound_untagged.ports[i];
        vlan_port.match_vlan = g_vlan_mirror_outbound_untagged.vlan;
        vlan_port.egress_vlan = g_vlan_mirror_outbound_untagged.vlan;
        vlan_port.vsi = g_vlan_mirror_outbound_untagged.vsi;

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create for port - %d, vlan - %d, rv - %d\n",
                   g_vlan_mirror_outbound_untagged.ports[i], g_vlan_mirror_outbound_untagged.vlan, rv);
            return rv;
        }
        g_vlan_mirror_outbound_untagged.gport[i] = vlan_port.vlan_port_id;
        printf("\n>>> Port %d VLAN %d created: gport = 0x%x)\n",
               g_vlan_mirror_outbound_untagged.ports[i], vlan_port.match_vlan, vlan_port.vlan_port_id);

        /* Attach the AC LIF to the Bridge VSI */
        rv = bcm_vswitch_port_add(unit, g_vlan_mirror_outbound_untagged.vsi, vlan_port.vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vswitch_port_add, vsi - %d, vlan_port_id - %d, rv - %d\n",
                   g_vlan_mirror_outbound_untagged.vsi, g_vlan_mirror_outbound_untagged.gport[i], rv);
            return rv;
        }
    }

    /* Configure a mac address for dst mac. */
    l2__mact_properties_s mact_entry = {
        g_vlan_mirror_outbound_untagged.gport[1],
        g_vlan_mirror_outbound_untagged.dmac,
        g_vlan_mirror_outbound_untagged.vsi
    };

    rv = l2__mact_entry_create(unit, &mact_entry);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create, rv - %d\n", rv);
        return rv;
    }

    return rv;
}

/* Main function, tcl interface */
int
vlan_mirror_outbound_untagged__start_run_with_ports(int unit, int port1, int port2, bcm_vlan_t vsi, bcm_vlan_t vlan)
{
    int rv;
    vlan_mirror_outbound_untagged_s temp_s;

    sal_memcpy(&temp_s, &g_vlan_mirror_outbound_untagged, sizeof(temp_s));

    temp_s.ports[0] = port1;
    temp_s.ports[1] = port2;
    temp_s.vsi      = vsi;
    temp_s.vlan     = vlan;

    rv = vlan_mirror_outbound_untagged__start_run(unit, &temp_s);

    return rv;
}

/* Main function of outbound mirroring BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED flag test */
int
vlan_mirror_outbound_untagged__start_run(int unit, vlan_mirror_outbound_untagged_s *test_data)
{
    int rv;
    bcm_vlan_port_t vlan_port;

    rv = vlan_mirror_outbound_untagged_init(unit, test_data);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_mirror_outbound_untagged_init, rv - %d\n", rv);
        return rv;
    }

    /* Configure Ports */
    rv = vlan_mirror_outbound_untagged_config_ports(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_mirror_outbound_untagged_config_ports, rv - %d\n", rv);
        return rv;
    }

    /* Configure Egress VLAN Editing*/
    rv = vlan_mirror_outbound_untagged_eve(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_mirror_outbound_untagged_eve  rv - %d\n",  rv);
        return rv;
    }

    return rv;
}

/* Set packet-is-UNtagged-after-EVE for tag_format 5 and edit_profile 4 */
int
vlan_mirror_outbound_untagged__dt_untagged_set(int unit)
{
    bcm_vlan_action_set_t action;
    int rv;

    bcm_vlan_action_set_t_init(&action);
    action.flags = BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED;

    return bcm_vlan_translate_action_id_set(unit,
                                            BCM_VLAN_ACTION_SET_EGRESS,
                                            g_vlan_mirror_outbound_untagged.dt_action_id,
                                            &action);
}
