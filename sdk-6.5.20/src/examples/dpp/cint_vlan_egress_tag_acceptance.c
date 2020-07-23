/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Example of employing egress vlan membership
 *
 * Tests scenario:
 *
 * step 1:  Egress tag acceptance disabled
 *          no vlan discard for all packets
 *
 * step 2:  Add vlan_i to to discard list of port_i
 *          vlan_i tag will be removed from the header
 *
 * Only in advanced mode:
 * step 3:  Set packet is untagged after EVE
 *          A packet tagged with vlan_i will be treated as untagged and its vlan won't be discarded
 *
 * Example:
 *
 * 1. Egress tag acceptance disabled
 *
 *                INPUT        ________________________         OUTPUT
 *                            |                        |
 *                   port 200 |                        | port 201
 *                   vlan 100 |                        | vlan 100
 *      --------------------->|                        |----------------------->
 *                            |________________________|
 *
 * 2. Vlan 100 in discard list
 *
 *                INPUT        ________________________         OUTPUT
 *                            |                        |
 *                   port 200 |   vlan 100 tag         | port 201
 *                   vlan 100 |   is removed           | untagged
 *      --------------------->|                        | ----------------------->
 *                            |________________________|
 *
 * 3. Vlan 100 in discard list and packet is untagged flag is set
 *
 *                INPUT        ________________________         OUTPUT
 *                            |                        |
 *                   port 200 |                        | port 201
 *                   vlan 100 |                        | vlan 100
 *      --------------------->|                        | ----------------------->
 *                            |________________________|
 *
 *
 * SOC properties:
 * custom_feature_outer_vid_existence_class_mode = 1
 * for step 3 only (Advanced Mode):
 * bcm886xx_vlan_translate_mode = 1
 *
 * CINT files:
 * cd ../../../../src/examples/dpp/
 * utility/cint_utils_l2.c
 * utility/cint_utils_vlan.c
 * utility/cint_utils_port.c
 * cint_port_tpid.c
 * cint_vlan_egress_tag_acceptance.c
 *
 * vlan_egress_tag_acceptance__start_run(int unit, vlan_egress_membership_s *test_data) - test_data = NULL for default params
 */

/*
 * Global Variables Definition and Initialization  START
 */

/*  Main Struct VLAN membership */
struct vlan_egress_tag_acceptance_s {
    bcm_vlan_t  vsi;
    bcm_vlan_t  vlan[2];
    int         ports[2];
    bcm_gport_t gport[3][2];
    bcm_mac_t   mac1;
    bcm_mac_t   mac2;
    int         dt_action_id;
};

/* Initialization of global struct*/
vlan_egress_tag_acceptance_s g_vlan_egress_tag_acceptance = {
    16,                                     /* vsi */
    {100, 200},                             /* vlan */
    {200, 201},                             /* ports */
    {{0, 0}, {0, 0},{0,0}},                 /* gport */
    {0x00, 0x11, 0x22, 0x33, 0x44, 0x55},   /* mac1 */
    {0x00, 0x55, 0x44, 0x33, 0x22, 0x11},   /* mac2 */
    0                                       /* dt_action_id */
};

/*
 * Global Variables Definition and Initialization  END
 */

void
vlan_egress_tag_acceptance__get_struct(vlan_egress_tag_acceptance_s *test_data)
{
    sal_memcpy(test_data, &g_vlan_egress_tag_acceptance, sizeof(*test_data));
    return;
}

int
vlan_egress_tag_acceptance_init(int unit, vlan_egress_tag_acceptance_s *test_data)
{
    if (test_data != NULL) {
       sal_memcpy(&g_vlan_egress_tag_acceptance, test_data, sizeof(g_vlan_egress_tag_acceptance));
    }

    if (!soc_property_get(unit, "custom_feature_outer_vid_existence_class_mode", 1)) {
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

/* configure eve*/
int
vlan_egress_tag_acceptance_eve(int unit)
{
    bcm_vlan_action_set_t action;
    int rv, i;
    int tag_format;
    tag_format = is_device_or_above(unit, JERICHO2) ? 16 : 6;

    if (soc_property_get(unit ,"bcm886xx_vlan_translate_mode", 0) == 1 || is_device_or_above(unit, JERICHO2)) {

        /***** AVT *****/

        bcm_vlan_action_set_t_init(&action);
        action.outer_tpid = 0x9100;
        action.inner_tpid = 0x8100;

        bcm_vlan_translate_action_class_t action_class;
        bcm_vlan_port_translation_t port_trans;
        int action_id_1;
        int action_id;

        for (i = 0; i < 2; i++) {
            /*
            Set outer/innner TPID to 0x9100/0x8100 to work on JR2 too
            by doing so, need to add a IVE on JR1 to pass the TPID profile to egress, JR2 doesn't have TPID profile
            */
            rv = port__tag_classification__set(unit, g_vlan_egress_tag_acceptance.ports[i], /*tag_format*/ tag_format, 0x9100, 0x8100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", g_vlan_egress_tag_acceptance.ports[i], rv);
                return rv;
            }
        }

        /* On JR2, set IVE to do nothing, pass TPID profile to egress */
        if (!is_device_or_above(unit, JERICHO2)) {
            /* set edit profile for ingress LIF */
            bcm_vlan_port_translation_t_init(&port_trans);
            port_trans.gport = g_vlan_egress_tag_acceptance.gport[2][0];
            port_trans.vlan_edit_class_id = 4;
            port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
            rv = bcm_vlan_port_translation_set(unit, &port_trans);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_port_translation_set\n");
                return rv;
            }
            
            /* Create action ID */
            rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_INGRESS, &action_id);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_id_create\n");
                return rv;
            }
            
            /* Set translation action to do nothing */
            action.dt_outer = bcmVlanActionNone;
            action.dt_inner = bcmVlanActionNone;
            rv = bcm_vlan_translate_action_id_set(unit,
                                                  BCM_VLAN_ACTION_SET_INGRESS,
                                                  action_id,
                                                  &action);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_id_set\n");
                return rv;
            }
            
            /* Set translation action class for ingress*/
            bcm_vlan_translate_action_class_t_init(&action_class);
            action_class.vlan_edit_class_id = 4;
            action_class.tag_format_class_id = tag_format;
            action_class.vlan_translation_action_id = action_id;
            action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
            rv = bcm_vlan_translate_action_class_set(unit, &action_class);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_action_class_set\n");
                return rv;
            }
        }

        /* set edit profile for egress LIF */
        bcm_vlan_port_translation_t_init(&port_trans);
        port_trans.gport = g_vlan_egress_tag_acceptance.gport[1][1];
        port_trans.vlan_edit_class_id = 4;
        port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_port_translation_set(unit, &port_trans);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_translation_set\n");
            return rv;
        }

        /* Create action ID */
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }
        g_vlan_egress_tag_acceptance.dt_action_id = action_id_1;

        /* Set translation action for double tagged packets - remove outer tag*/
        action.dt_outer = bcmVlanActionDelete;
        rv = bcm_vlan_translate_action_id_set(unit,
                                              BCM_VLAN_ACTION_SET_EGRESS,
                                              action_id_1,
                                              &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set\n");
            return rv;
        }

        /* Set translation action class */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = 4;
        action_class.tag_format_class_id = tag_format;
        action_class.vlan_translation_action_id = action_id_1;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }

    } else {

        /***** SVT *****/
        /* Set IVE to do nothing, pass TPID profile to egress */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionNone;
        action.dt_inner = bcmVlanActionNone;
        rv = bcm_vlan_translate_action_create(unit, g_vlan_egress_tag_acceptance.ports[0], bcmVlanTranslateKeyPortDouble,
                                              g_vlan_egress_tag_acceptance.vlan[1], g_vlan_egress_tag_acceptance.vlan[0], &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_create for port - %d, rv - %d\n", g_vlan_egress_tag_acceptance.ports[0], rv);
            return rv;
        }

        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionDelete;
        for (i = 0; i < 2; i++) {
            rv =  bcm_vlan_translate_egress_action_add(unit, g_vlan_egress_tag_acceptance.gport[1][1], BCM_VLAN_NONE ,BCM_VLAN_NONE, &action);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_translate_egress_action_add for port - %d, rv - %d\n", g_vlan_egress_tag_acceptance.gport[1][1], rv);
                return rv;
            }
        }
    }

    return rv;
}

/* configure ports */
int
vlan_egress_tag_acceptance_config_ports(int unit)
{
    int i, rv;
    bcm_vlan_port_t vlan_port;

    /* Create VSI */
    rv = bcm_vlan_create(unit, g_vlan_egress_tag_acceptance.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_create\n");
        return rv;
    }

    for (i = 0; i < 2; i++) {
        /* Configure the TPIDs for the port and their classification */
        /* JR2: use default application reference to configure port TPID setting */
        if (!is_device_or_above(unit, JERICHO2)){
            rv = port__tpids__set(unit, g_vlan_egress_tag_acceptance.ports[i], 0x9100, 0x8100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tpids__set for port - %d, rv - %d\n", g_vlan_egress_tag_acceptance.ports[i], rv);
                return rv;
            }
        }

        if (soc_property_get(unit ,"bcm886xx_vlan_translate_mode", 0) == 1 || is_device_or_above(unit, JERICHO2)) {
            rv = port__tag_classification__set(unit, g_vlan_egress_tag_acceptance.ports[i], /*tag_format*/ 4, 0x8100, 0xFFFFFFFF);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", g_vlan_egress_tag_acceptance.ports[i], rv);
                return rv;
            }

            /* Set translation action 0 */
            bcm_vlan_action_set_t action;
            bcm_vlan_action_set_t_init(&action);
            rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, 0, &action);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_vlan_translate_action_id_set, rv - %d\n", rv);
                return rv;
            }
        }

        /* Create an AC LIF for vlan1 */
        bcm_vlan_port_t_init(&vlan_port);

        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.port = g_vlan_egress_tag_acceptance.ports[i];
        vlan_port.match_vlan = g_vlan_egress_tag_acceptance.vlan[0];
        if (!is_device_or_above(unit, JERICHO2)){
            vlan_port.vsi = g_vlan_egress_tag_acceptance.vsi;
            vlan_port.egress_vlan = g_vlan_egress_tag_acceptance.vlan[0];
        }

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create for port - %d, vlan - %d, rv - %d\n",
                   g_vlan_egress_tag_acceptance.ports[i], g_vlan_egress_tag_acceptance.vlan[0], rv);
            return rv;
        }

        g_vlan_egress_tag_acceptance.gport[i][0] = vlan_port.vlan_port_id;
        printf("\n>>> Port %d VLAN %d created: gport = 0x%x)\n",
               g_vlan_egress_tag_acceptance.ports[i], vlan_port.match_vlan, vlan_port.vlan_port_id);

        /* Attach the AC LIF1 to the Bridge VSI */
        rv = bcm_vswitch_port_add(unit, g_vlan_egress_tag_acceptance.vsi, vlan_port.vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vswitch_port_add, vsi - %d, vlan_port_id - %d, rv - %d\n",
                   g_vlan_egress_tag_acceptance.vsi, g_vlan_egress_tag_acceptance.gport[i][0], rv);
            return rv;
        }

        /* Create an AC LIF for vlan2 */
        vlan_port.match_vlan = g_vlan_egress_tag_acceptance.vlan[1];
        vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : g_vlan_egress_tag_acceptance.vlan[1];

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create for port - %d, vlan - %d, rv - %d\n",
                   g_vlan_egress_tag_acceptance.ports[i], g_vlan_egress_tag_acceptance.vlan[1], rv);
            return rv;
        }
        rv = bcm_vlan_gport_add(unit, g_vlan_egress_tag_acceptance.vlan[0], g_vlan_egress_tag_acceptance.ports[i], 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_gport_add for port - %d, vlan - %d, rv - %d\n",
                    g_vlan_egress_tag_acceptance.ports[i], g_vlan_egress_tag_acceptance.vlan[0], rv);
            return rv;
        }
        rv = bcm_vlan_gport_add(unit, g_vlan_egress_tag_acceptance.vlan[1], g_vlan_egress_tag_acceptance.ports[i], 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_gport_add for port - %d, vlan - %d, rv - %d\n",
                    g_vlan_egress_tag_acceptance.ports[i], g_vlan_egress_tag_acceptance.vlan[1], rv);
            return rv;
        }

        g_vlan_egress_tag_acceptance.gport[i][1] = vlan_port.vlan_port_id;
        printf("\n>>> Port %d VLAN %d created: gport = 0x%x)\n",
               g_vlan_egress_tag_acceptance.ports[i], vlan_port.match_vlan, vlan_port.vlan_port_id);

        /* Attach the AC LIF2 to the Bridge VSI */
        rv = bcm_vswitch_port_add(unit, g_vlan_egress_tag_acceptance.vsi, vlan_port.vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vswitch_port_add, vsi - %d, vlan_port_id - %d, rv - %d\n",
                   g_vlan_egress_tag_acceptance.vsi, g_vlan_egress_tag_acceptance.gport[i][1], rv);
            return rv;
        }
    }

    /* Create AC LIF for double tagged traffic */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port.port = g_vlan_egress_tag_acceptance.ports[0];
    vlan_port.match_vlan = g_vlan_egress_tag_acceptance.vlan[1];
    vlan_port.match_inner_vlan = g_vlan_egress_tag_acceptance.vlan[0];
    if (!is_device_or_above(unit, JERICHO2)){
        vlan_port.vsi = g_vlan_egress_tag_acceptance.vsi;
    }

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create for port - %d, vlan - %d, rv - %d\n", g_vlan_egress_tag_acceptance.ports[0], test_data->vlan[0], rv);
        return rv;
    }

    g_vlan_egress_tag_acceptance.gport[2][0] = vlan_port.vlan_port_id;
    printf("\n>>> Port %d VLANs %d and %d  created: gport = 0x%x)\n",
           g_vlan_egress_tag_acceptance.ports[0], vlan_port.match_vlan,
           vlan_port.match_inner_vlan, g_vlan_egress_tag_acceptance.gport[2][0]);

    if (is_device_or_above(unit, JERICHO2)){
        rv = bcm_vswitch_port_add(unit, g_vlan_egress_tag_acceptance.vsi, g_vlan_egress_tag_acceptance.gport[2][0]);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vswitch_port_add, vsi - %d, vlan_port_id - %d, rv - %d\n",
                   g_vlan_egress_tag_acceptance.vsi, g_vlan_egress_tag_acceptance.gport[2][0], rv);
            return rv;
        }
    }

    /* Configure a mac address for dst mac. */
    l2__mact_properties_s mact_entry = {
        g_vlan_egress_tag_acceptance.gport[1][0],
        g_vlan_egress_tag_acceptance.mac1,
        g_vlan_egress_tag_acceptance.vsi
    };

    l2__mact_properties_s mact_entry2 = {
        g_vlan_egress_tag_acceptance.gport[1][1],
        g_vlan_egress_tag_acceptance.mac2,
        g_vlan_egress_tag_acceptance.vsi
    };

    rv = l2__mact_entry_create(unit, &mact_entry);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create, rv - %d\n", rv);
        return rv;
    }

    rv = l2__mact_entry_create(unit, &mact_entry2);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create, rv - %d\n", rv);
        return rv;
    }

    return rv;
}

/* Main function tcl interface */
int
vlan_egress_tag_acceptance__start_run_with_ports(int unit, int port1, int port2, bcm_vlan_t vsi, bcm_vlan_t vlan1, bcm_vlan_t vlan2)
{
    int rv;
    vlan_egress_tag_acceptance_s temp_s;

    sal_memcpy(&temp_s, &g_vlan_egress_tag_acceptance, sizeof(temp_s));

    temp_s.ports[0] = port1;
    temp_s.ports[1] = port2;
    temp_s.vsi      = vsi;
    temp_s.vlan[0]  = vlan1;
    temp_s.vlan[1]  = vlan2;

    rv = vlan_egress_tag_acceptance__start_run(unit, &temp_s);

    return rv;
}

/* Main function of Outer-VID Tag discard test */
int
vlan_egress_tag_acceptance__start_run(int unit, vlan_egress_tag_acceptance_s *test_data)
{
    int rv;
    bcm_vlan_port_t vlan_port;

    rv = vlan_egress_tag_acceptance_init(unit, test_data);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_egress_tag_acceptance_init, rv - %d\n", rv);
        return rv;
    }

    rv = vlan_egress_tag_acceptance_config_ports(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_egress_tag_acceptance_config_ports, rv - %d\n", rv);
        return rv;
    }

    /* Configure Egress VLAN Editing */
    rv = vlan_egress_tag_acceptance_eve(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_egress_tag_acceptance_eve  rv - %d\n",  rv);
        return rv;
    }

    return rv;
}

/* Set Outer-VID Tag discard */
int
vlan_egress_tag_acceptance__outer_tag_discard_set(int unit, bcm_vlan_t vlan)
{
    bcm_gport_t gport = ((vlan == g_vlan_egress_tag_acceptance.vlan[0]) ? g_vlan_egress_tag_acceptance.gport[1][0] : g_vlan_egress_tag_acceptance.gport[1][1]);

    /* Outer-VID Tag discard */
    printf("Port %d (gport - %x) added to untagged list of VLAN %d\n",
           g_vlan_egress_tag_acceptance.ports[1], gport, vlan);

    /* For JR2,Vlan gport is invalid input to the API */
    if (!is_device_or_above(unit, JERICHO2)){
        return bcm_vlan_gport_add(unit, vlan, gport,
                                  (BCM_VLAN_GPORT_ADD_EGRESS_ONLY | BCM_VLAN_PORT_UNTAGGED | BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE));
    } else {
        return bcm_vlan_gport_add(unit, vlan, g_vlan_egress_tag_acceptance.ports[1],
                                  (BCM_VLAN_GPORT_ADD_EGRESS_ONLY | BCM_VLAN_PORT_UNTAGGED | BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE));
    }
}

/* Set packet-is-UNtagged-after-EVE for double tagged packets */
int
vlan_egress_tag_acceptance__dt_untagged_set(int unit)
{
    bcm_vlan_action_set_t action;
    int rv;

    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionDelete;
    action.flags = BCM_VLAN_ACTION_SET_OUTER_VID_UNTAGGED;

    return bcm_vlan_translate_action_id_set(unit,
                                            BCM_VLAN_ACTION_SET_EGRESS,
                                            g_vlan_egress_tag_acceptance.dt_action_id,
                                            &action);
}

/* Remove port from VLAN's memberset */
int
vlan_egress_tag_acceptance__port_delete(int unit, bcm_vlan_t vlan)
{
    bcm_gport_t gport = (vlan == g_vlan_egress_tag_acceptance.vlan[0] ? g_vlan_egress_tag_acceptance.gport[1][0] : g_vlan_egress_tag_acceptance.gport[1][1]);

    /* VLAN membership */
    printf("Port %d (gport - %x) removed from VLAN %d\n",
           g_vlan_egress_tag_acceptance.ports[1], gport, vlan);

    /* For JR2,Vlan gport is invalid input to the API */
    if (!is_device_or_above(unit, JERICHO2)){
        return bcm_vlan_gport_delete(unit, vlan, gport);
    } else {
        return bcm_vlan_gport_delete(unit, vlan, g_vlan_egress_tag_acceptance.ports[1]);
    }
}
