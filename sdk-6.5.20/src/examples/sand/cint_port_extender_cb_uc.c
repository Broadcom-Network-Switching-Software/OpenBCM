/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * Purpose: The CINT demonstrates a Controlling bridge configuration as part of a Port Extender (802.BR) application.
 *
 * 1. The basic example handles UC traffic with no protection.
 *    Packets can be sent from/to a cascaded port or an End station.
 * 2. A new match can be added to an existing extender port using port_extender_cb_uc_match_add.
 *    A new Extender-Channel registration entry, corresponding to the new match, may be added by setting add_e_channel_reg_entry = 1.
 * 3. The previously created new match can be removed using port_extender_cb_uc_match_delete
 *    An existing Extender-Channel registration entry, corresponding to the removed match, may be added by setting delete_e_channel_reg_entry = 1.
 *
 *                                                                         Network
 *                                                                       | Port
 *                                                                       |
 *                                                    ___________________|__________________
 *                 __________                        |                                      |                           __________
 *        LIF 1 __|          |                       |         Controlling Bridge           |                          |          |__  LIF 3
 *        LIF 2 __|   Port   | --------------------->|------------------------------------> |------------------------->|  Port    |__
 *              __|          |      In-Cascaded-Port |                    |    /\           | Out-Cascaded-Port        |          |__
 *              __| Extender | <---------------------|<-------------------|    |            |                          | Extender |__
 *                |__________|                       |                    |    |            |                          |__________|
 *                                                   |                    |    |            |
 *                             _____________         |                    |    |            |
 *                            |             |<------ |<-------------------     |            |
 *                  LIF4(AC)  | End Station |        |                         |            |
 *                            |_____________|------->|-------------------------             |
 *                                                   |                                      |
 *                                                   |______________________________________|
 *
 * SOC properties:
 * 1. Configure the device to be a Control Bridge: extender_control_bridge_enable. 1: Enable, 0: Disable.
 * 2. Set the number of bytes to prepend to the start of the packet: prepend_tag_bytes = 8B
 * 3. Set the offset in bytes from the DA/SA in the packet from which to prepend the tag: prepend_tag_offset = 0
 *
 * CINT files:
 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c
 * cint cint_utils_port.c
 * cint cint_utils_l2.c
 * cint cint_utils_vlan.c
 * cint cint_port_extender_cb_uc.c
 * cint
 *
 * port_extender_cb_uc__start_run(int unit, port_extender_cb_uc_s *param) - param = NULL for default params
 *
 * All default values could be re-written with initialization of the global structure 'g_port_extender_cb_uc', before calling the main function.
 * In order to re-write only part of the values, call 'port_extender_cb_uc_struct_get(port_extender_cb_uc_s)' function and re-write the values
 * prior to calling the main function.
 */



/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
int NOF_EXTENDER_PORTS = 3;
uint8 adv_xlate_mode = 0;
int saved_field_group_8021br_pmf = 1;

struct port_extender_cascaded_port_s {
    bcm_gport_t port;
    bcm_port_tag_format_class_t tag_format;     /* Tag format used for Tag classification */
};

struct port_extender_end_station_s {
    bcm_gport_t port;
    bcm_vlan_t tag_vid;
    bcm_vlan_t tag_format;                      /* Tag format used for Tag classification */
    bcm_gport_t vlan_port_id;                   /* The LIF ID */
};

/*  Main Struct  */
struct port_extender_cb_uc_s {
    port_extender_cascaded_port_s cascaded_port[NOF_EXTENDER_PORTS];
    extender__port_extender_port_s extender_port[NOF_EXTENDER_PORTS];
    port_extender_end_station_s end_station;
    bcm_vlan_t vsi;
    uint32 extender_tpid;
    uint32 tag_tpid;
    uint32 ext_vlan_edit_profile;               /* A VLAN-Edit profile that will be used only by Extender Ports */
    bcm_vlan_t new_vid;
};

/* Initialization of global struct*/
port_extender_cb_uc_s g_port_extender_cb_uc = { /* Cascaded Ports configuration
                                                Phy Port    Tag Format                  */
                                                {{ 13,       5   },     /* In-Cascaded-Port  */
                                                 { 13,       5   },     /* In-Cascaded-Port (Out) */
                                                 { 14,       5   }},    /* Out-Cascaded-Port */

                                                /* Extender Ports configuration
                                                Phy Port    Tag VID     Extender VID    Extender gport ID */
                                                {{ 13,       100,       201,            0 },    /* LIF 1 */
                                                 { 13,       100,       202,            0 },    /* LIF 2 */
                                                 { 14,       100,       203,            0 }},   /* LIF 3 */

                                                /* End Station configuration
                                                Phy Port    Tag VID     Tag format      vlan_port_id*/
                                                 { 15,       100,       4,              0 },

                                                /* Additional parameters */
                                                12,         /* VSI */
                                                0x893F,     /* Extender TPID */
                                                0x8100,     /* Tag TPID */
                                                3};         /* VLAN-Edit Profile */


/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */



/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT:
 *   params: new values for g_port_extender_cb_uc
 */
int port_extender_cb_uc_init(int unit, port_extender_cb_uc_s *params) {

    int rv, cb_enable, prepend_bytes, prepend_offset, avt_mode;

    if (params != NULL) {
       sal_memcpy(&g_port_extender_cb_uc, params, sizeof(g_port_extender_cb_uc));
    }

    /* Save VLAN Edit mode (advanced/standard) */
    if (is_device_or_above(unit, JERICHO2)) {
        adv_xlate_mode = 1;
    } else {
        adv_xlate_mode = soc_property_get(unit ,"bcm886xx_vlan_translate_mode", 0);
    }

    /* Define a VSI for the switch */
    rv = bcm_vswitch_create_with_id(unit, g_port_extender_cb_uc.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create failed for vsi - %d, rv - %d\n", g_port_extender_cb_uc.vsi, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Configuration of a single cascaded port.
 * Various per physical port configuration that determine the prepend tag,
 * the Extender device type and filter configuration.
 * In addition, the function handles the port's TPIDs - The profile and the
 * tag format classification.
 *
 * INPUT:
 *   cascaded_port_info: Configuration info for a single cascaded port.
 */
int port_extender_cb_uc__set_cascaded_port(int unit, port_extender_cascaded_port_s *cascaded_port_info) {

    int rv;
    uint32 vlan_domain, enable, flags;

    /* Attach the port to a VLAN-Domain - The same as the port in this case (but doesn't have to) */
    vlan_domain = cascaded_port_info->port;
    rv = bcm_port_class_set(unit, cascaded_port_info->port, bcmPortClassId, vlan_domain);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set for port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    /* Disable VLAN-Membership filter */
    flags = 0;
    rv = bcm_port_vlan_member_set(unit, cascaded_port_info->port, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_member_set to disable for port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    /* Disable same-interface filter */
    enable = 0;
    rv = bcm_port_control_set(unit, cascaded_port_info->port, bcmPortControlBridge, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set for bcmPortControlBridge to Disable, port - %d, rv - %d\n", cascaded_port_info->port, rv);
        return rv;
    }

    if (!is_device_or_above(unit, JERICHO2)) {
        /* Configure the TPIDs for the port and their classification */
        rv = port__tpids__set(unit, cascaded_port_info->port, g_port_extender_cb_uc.extender_tpid, g_port_extender_cb_uc.tag_tpid);
        if (rv != BCM_E_NONE) {
            printf("Error, port__tpids__set for port - %d, rv - %d\n", cascaded_port_info->port, rv);
            return rv;
        }

        if (adv_xlate_mode) {
            rv = port__tag_classification__set(unit, cascaded_port_info->port, cascaded_port_info->tag_format, g_port_extender_cb_uc.extender_tpid, 0xFFFFFFFF);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", cascaded_port_info->port, rv);
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}


/* Configuration of Egress VLAN Editing for a single Extender LIF
 *
 * All cascaded Ports: Add ETAG, VID no change
 * g_port_extender_cb_uc.cascaded_port[2]: Add ETAG + Modify VID
 */
int
port_extender_cb_uc__eve_translation_set(int unit,
                    bcm_gport_t lif,
                    int outer_tpid,
                    int inner_tpid,
                    bcm_vlan_action_t outer_action,
                    bcm_vlan_action_t inner_action,
                    bcm_vlan_t new_outer_vid,
                    bcm_vlan_t new_inner_vid,
                    uint32 vlan_edit_profile,
                    port_extender_cascaded_port_s * cascaded_port_info)
{
    bcm_vlan_action_set_t action;
    int rv;

    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;

    if (adv_xlate_mode)
    {
        bcm_vlan_translate_action_class_t action_class;
        bcm_vlan_port_translation_t port_trans;
        int action_id_1, action_id_2;

        /* set edit profile for egress LIF */
        bcm_vlan_port_translation_t_init(&port_trans);
        port_trans.new_outer_vlan = new_outer_vid;
        port_trans.new_inner_vlan = new_inner_vid;
        port_trans.gport = lif;
        port_trans.vlan_edit_class_id = vlan_edit_profile;
        port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_port_translation_set(unit, &port_trans);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_translation_set\n");
            return rv;
        }

        /* Create action ID*/
        rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        /* Set translation action */
        action.dt_outer = outer_action;
        action.dt_inner = inner_action;
        action.dt_inner_pkt_prio = bcmVlanActionCopy;
        rv = bcm_vlan_translate_action_id_set( unit,
                                               BCM_VLAN_ACTION_SET_EGRESS,
                                               action_id_1,
                                               &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set\n");
            return rv;
        }

        /* Set translation action class */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = vlan_edit_profile;
        action_class.tag_format_class_id = cascaded_port_info->tag_format;
        action_class.vlan_translation_action_id = action_id_1;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }
        /* Action for untagged packets */

        /* Create action ID*/
        rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        bcm_vlan_action_set_t_init(&action);
        action.outer_tpid = g_port_extender_cb_uc.extender_tpid;
        action.inner_tpid = 0x8100;
        action.dt_outer = bcmVlanActionAdd;
        action.dt_inner = bcmVlanActionAdd;
        rv = bcm_vlan_translate_action_id_set( unit,
                                               BCM_VLAN_ACTION_SET_EGRESS,
                                               action_id_2,
                                               &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set\n");
            return rv;
        }

        /* Set translation action class */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = vlan_edit_profile;
        action_class.tag_format_class_id = 0;
        action_class.vlan_translation_action_id = action_id_2;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }
    }
    else if (cascaded_port_info->port == g_port_extender_cb_uc.cascaded_port[2].port)
    {
        action.ot_outer = outer_action;
        action.ot_inner = inner_action;
        action.ut_outer = bcmVlanActionAdd;
        action.ut_inner = bcmVlanActionAdd;
        action.new_outer_vlan = new_outer_vid;
        action.new_inner_vlan = new_inner_vid;
        action.outer_tpid_action = bcmVlanTpidActionModify;
        action.ot_inner_pkt_prio = bcmVlanActionCopy;
        rv =  bcm_vlan_translate_egress_action_add(unit, lif, BCM_VLAN_NONE ,BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_egress_action_add for port - %d, rv - %d\n", cascaded_port_info->port, rv);
            return rv;
        }
    }
    return rv;
}


/* Configuration of a Port Extender single Extender LIF
 * Create the object(LIF), associate it with a VSI and define an EVE operation.
 *
 * INPUT:
 *   extender_port_info: Configuration info for a single Extender LIF.
 */
int port_extender_cb_uc__set_extender_lif(int unit, extender__port_extender_port_s *extender_port_info, port_extender_cascaded_port_s *cascaded_port_info) {

    int rv;

    /* Attach the Extender-Port to a specific VSI */
    rv = bcm_vswitch_port_add(unit, g_port_extender_cb_uc.vsi, extender_port_info->extender_gport_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_port_add, vsi - %d, extender_port_id - %d, rv - %d\n", g_port_extender_cb_uc.vsi, extender_port_info->extender_gport_id, rv);
    }
    printf("Allocated extender_port_id - %x\n", extender_port_info->extender_gport_id);


    if (!is_device_or_above(unit, JERICHO2)) {
        /* Configure an EVE action that Adds the E-TAG of the destination VM (out cascaded port: Add ETAG + replace VID) */
        rv = port_extender_cb_uc__eve_translation_set(unit,
                                                      extender_port_info->extender_gport_id,
                                                      g_port_extender_cb_uc.extender_tpid,
                                                      g_port_extender_cb_uc.tag_tpid,
                                                      bcmVlanActionReplace,
                                                      bcmVlanActionAdd,
                                                      extender_port_info->extender_vid,
                                                      ((cascaded_port_info->port == g_port_extender_cb_uc.cascaded_port[2].port) ? g_port_extender_cb_uc.new_vid : extender_port_info->tag_vid),
                                                      g_port_extender_cb_uc.ext_vlan_edit_profile,
                                                      cascaded_port_info);
        if (rv != BCM_E_NONE) {
            printf("Error in port_extender_cb_uc__eve_translation_set, extender_port_id - %d, rv - %d\n", extender_port_info->extender_gport_id, rv);
        }

    } else {
        /*
         * Nwk_QoS for outer tag
         */
        rv = bcm_switch_control_port_set(unit, extender_port_info->port, bcmSwitchTagPcpDeiSrc, 7);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_switch_control_port_set\n");
        }

    }


    return BCM_E_NONE;
}


/* Configuration of a Port Extender End station
 * Handles the port's TPIDs - The profile and the tag format classification,
 * creates the AC LIF and associates it with a VSI.
 * No EVE is defined as the C-VID in this example is the same across all the
 * interfaces.
 *
 * INPUT:
 *   end_station_info: Configuration info for a single End station
 */
int port_extender_cb_uc__set_end_station(int unit, port_extender_end_station_s *end_station_info) {

    int rv;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_translation_t vp_translation;

    /* Configure the TPIDs for the port and their classification */
    if (!is_device_or_above(unit, JERICHO2)) {
        rv = port__tpids__set(unit, end_station_info->port, g_port_extender_cb_uc.tag_tpid, 0x0);
        if (rv != BCM_E_NONE) {
            printf("Error, port__tpids__set for port - %d, rv - %d\n", end_station_info->port, rv);
            return rv;
        }

        if (adv_xlate_mode) {
            rv = port__tag_classification__set(unit, end_station_info->port, end_station_info->tag_format, g_port_extender_cb_uc.tag_tpid, 0);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", end_station_info->port, rv);
                return rv;
            }
        }
    }
    
    /* Create an AC LIF */
    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = end_station_info->port;
    vlan_port.match_vlan = end_station_info->tag_vid;
    vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : end_station_info->tag_vid;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create for port - %d, vlan - %d, rv - %d\n", end_station_info->port, end_station_info->tag_vid, rv);
        return rv;
    }
    end_station_info->vlan_port_id = vlan_port.vlan_port_id;

    /* Attach the AC LIF to the Controller Bridge VSI */
    rv = bcm_vswitch_port_add(unit, g_port_extender_cb_uc.vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_port_add, vsi - %d, vlan_port_id - %d, rv - %d\n", g_port_extender_cb_uc.vsi, vlan_port.vlan_port_id, rv);
        return rv;
    }

    /* No IVE/EVE is configured as it is assumed that the End Station provides packets with the same C-TAG value.
       In other cases, the IVE/EVE should reflect the C-TAG manipulation. */
    if (adv_xlate_mode) {

        bcm_vlan_action_set_t action;
        bcm_vlan_action_set_t_init(&action);
        rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, 0, &action);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_vlan_translate_action_id_set, vsi - %d, vlan_port_id - %d, rv - %d\n", g_port_extender_cb_uc.vsi, vlan_port.vlan_port_id, rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}


/*
 * Main function to run the Port Extender Control Bridge UC example
 *
 * Main steps of configuration:
 *    1. Initialize test parameters
 *    2. Set Cascaded Ports
 *    3. Set Port-Extender LIFs
 *    4. Set an End Station
 *
 * INPUT: unit  - unit
 *        param - NULL for default params, or new values.
 */
int port_extender_cb_uc__start_run(int unit,  port_extender_cb_uc_s *params) {

    int rv, idx;
    bcm_extender_forward_t fwd_entry;

    /* Initialize the test parameters */

    rv = port_extender_cb_uc_init(unit, params);
    if (rv != BCM_E_NONE) {
        printf("Error in port_extender_cb_uc_init, rv - %d\n", rv);
        return rv;
    }

    rv = extender__init(unit, g_port_extender_cb_uc.extender_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error in extender__init, rv - %d\n", rv);
        return rv;
    }
    

    /* Configure the cascaded ports */
    for (idx = 1; idx < NOF_EXTENDER_PORTS; idx++) {
        rv = extender__cascaded_port_configure(unit, g_port_extender_cb_uc.cascaded_port[idx].port);
        if (rv != BCM_E_NONE) {
            printf("Error in extender__cascaded_port_configure for port #%d, rv - %d\n", idx, rv);
            return rv;
        }

        rv = port_extender_cb_uc__set_cascaded_port(unit, &g_port_extender_cb_uc.cascaded_port[idx]);
        if (rv != BCM_E_NONE) {
            printf("Error in port_extender_cb_uc__set_cascaded_port for port #%d, rv - %d\n", idx, rv);
            return rv;
        }
    }


    for (idx = 0; idx < NOF_EXTENDER_PORTS; idx++) {

        /* Configure Port-Extender LIFs */

        rv = extender__extender_port_configure(unit, &g_port_extender_cb_uc.extender_port[idx]);
        if (rv != BCM_E_NONE) {
            printf("Error in extender__extender_port_configure for LIF #%d, rv - %d\n", idx, rv);
            return rv;
        }

        rv = port_extender_cb_uc__set_extender_lif(unit, &g_port_extender_cb_uc.extender_port[idx], &g_port_extender_cb_uc.cascaded_port[idx]);
        if (rv != BCM_E_NONE) {
            printf("Error in port_extender_cb_uc__set_extender_lif for LIF #%d, rv - %d\n", idx, rv);
            return rv;
        }   
    }


    /* Configure an End Station */
    rv = port_extender_cb_uc__set_end_station(unit, &g_port_extender_cb_uc.end_station);
    if (rv != BCM_E_NONE) {
        printf("Error in port_extender_cb_uc__set_end_station, rv - %d\n", rv);
        return rv;
    } 

    /* Configure a mac address for the endpoint. */
    l2__mact_properties_s endpoint_mact_entry = { g_port_extender_cb_uc.end_station.vlan_port_id,
                                        {0x00, 0x00, 0x12, 0x34, 0x56, 0x78},
                                        g_port_extender_cb_uc.vsi
    };

    rv = l2__mact_entry_create(unit, &endpoint_mact_entry);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create, rv - %d\n", rv);
        return rv;
    } 

    /* Configure a mac address for the out cascade port. */
    l2__mact_properties_s out_cascade_mact_entry = { g_port_extender_cb_uc.extender_port[2].extender_gport_id,
                                        {0x00, 0x00, 0x9a, 0xbc, 0xde, 0xf0},
                                        g_port_extender_cb_uc.vsi
    };

    rv = l2__mact_entry_create(unit, &out_cascade_mact_entry);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create, rv - %d\n", rv);
        return rv;
    }

    /* Configure a mac address for the in cascade port. */
    l2__mact_properties_s in_cascade_mact_entry = { g_port_extender_cb_uc.extender_port[1].extender_gport_id,
                                        {0x00, 0x00, 0x00, 0x11, 0x22, 0x33},
                                        g_port_extender_cb_uc.vsi
    };

    rv = l2__mact_entry_create(unit, &in_cascade_mact_entry);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create, rv - %d\n", rv);
        return rv;
    }

    printf("Done configuring\n");

    return BCM_E_NONE;
}

/*
 * You can use this function to change the ports the cint is using from the defaults (13, 14 and 15).
 *
 * INPUT: unit              - unit
 *        in_cascaded_port  - cascaded port holding lifs 1 and 2
 *        out_cascaded_port - cascaded port holding lif 3
 *        end_station_port  - ethernet port holding lif 4
 */
int port_extender_cb_uc__start_run_dvapi(int unit, int in_cascaded_port, int out_cascaded_port, int end_station_port, bcm_vlan_t new_vid){
    port_extender_cb_uc_s params;

    sal_memcpy(&params, &g_port_extender_cb_uc, sizeof (g_port_extender_cb_uc));

    params.cascaded_port[0].port = in_cascaded_port;
    params.cascaded_port[1].port = in_cascaded_port;
    params.cascaded_port[2].port = out_cascaded_port;

    params.end_station.port = end_station_port;

    params.extender_port[0].port = in_cascaded_port;
    params.extender_port[1].port = in_cascaded_port;
    params.extender_port[2].port = out_cascaded_port;

    params.new_vid = new_vid;

    return port_extender_cb_uc__start_run(unit, &params);
}

/*
 * add match to an existing Extender LIF
 * INPUT: unit              - unit
 *        extender_index    - index of the extender port LIF, choose 0 or 1
 *        extended_port_vid - ECID of the new match to add
 *        add_e_channel_reg_entry  - add E-Channel registration entry flag
 */
int port_extender_cb_uc_match_add(int unit, int extender_index, int extended_port_vid, int add_e_channel_reg_entry){
    int rv;
    bcm_extender_forward_t fwd_entry;

    bcm_port_match_info_t extender_port_match;
    bcm_port_match_info_t_init(&extender_port_match);
    extender_port_match.match = BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_VLAN;
    extender_port_match.port = 0;
    extender_port_match.extended_port_vid = extended_port_vid;
    extender_port_match.match_vlan = 100;
    extender_port_match.flags = BCM_PORT_MATCH_INGRESS_ONLY;

    rv = bcm_port_match_add(unit, g_port_extender_cb_uc.extender_port[extender_index].extender_gport_id, &extender_port_match);
    if (rv != BCM_E_NONE) {
          printf("Error in bcm_port_match_add\n");
          return rv;
    }

    if (add_e_channel_reg_entry) {
        bcm_extender_forward_t_init(&fwd_entry);
        fwd_entry.flags = BCM_EXTENDER_FORWARD_UPSTREAM_ONLY;
        fwd_entry.name_space = g_port_extender_cb_uc.cascaded_port[extender_index].port;    /* vlan_domain */
        fwd_entry.extended_port_vid = extended_port_vid;                                    /* ecid */
        rv = bcm_extender_forward_add(unit, &fwd_entry);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_extender_forward_add for port - %d, ecid - 0x%x rv - %d\n", cascaded_port_info->port, extended_port_vid, rv);
            return rv;
        }
    }

    return rv;
}

/*
 * add/delete match for untagged packets to an existing Extender LIF
 * INPUT: unit              - unit
 *        extender_index    - index of the extender port LIF, choose 0 or 1
 *        add               - 1:add 0:delete
 */
int port_extender_cb_uc_ut_match_add_delete(int unit, int extender_index, int initial_vid, int add){

    int rv;
    bcm_port_match_info_t extender_port_match;

    bcm_port_match_info_t_init(&extender_port_match);
    extender_port_match.match = BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_INITIAL_VLAN;
    extender_port_match.port = g_port_extender_cb_uc.extender_port[extender_index].port;
    extender_port_match.extended_port_vid = g_port_extender_cb_uc.extender_port[extender_index].extender_vid;
    extender_port_match.match_vlan = initial_vid;
    extender_port_match.flags = BCM_PORT_MATCH_INGRESS_ONLY;

    if (add) {
        rv = bcm_port_match_add(unit, g_port_extender_cb_uc.extender_port[extender_index].extender_gport_id, &extender_port_match);
        if (rv != BCM_E_NONE) {
              printf("Error in bcm_port_match_add\n");
              return rv;
        }
    } else {
        rv = bcm_port_match_delete(unit, g_port_extender_cb_uc.extender_port[extender_index].extender_gport_id, &extender_port_match);
        if (rv != BCM_E_NONE) {
              printf("Error in bcm_port_match_add\n");
              return rv;
        }
    }

    return rv;
}

/*
 * Remove match from an existing Extender LIF (that was previously created with bcm_port_match_add)
 * INPUT:   unit                        - unit
 *          extender_index              - index of the extender port LIF, choose 0 or 1
 *          extended_port_vid           - ECID of the match to delete
 *          delete_e_channel_reg_entry  - delete E-Channel registration entry flag
 */
int port_extender_cb_uc_match_delete(int unit, int extender_index, int extended_port_vid, int delete_e_channel_reg_entry){
    int rv;
    bcm_extender_forward_t fwd_entry;

    bcm_port_match_info_t extender_port_match;
    bcm_port_match_info_t_init(&extender_port_match);
    extender_port_match.match = BCM_PORT_MATCH_PORT_EXTENDED_PORT_VID_VLAN;
    extender_port_match.port = 0;
    extender_port_match.extended_port_vid = extended_port_vid;
    extender_port_match.match_vlan = 100;
    extender_port_match.flags = BCM_PORT_MATCH_INGRESS_ONLY;

    rv = bcm_port_match_delete(unit, g_port_extender_cb_uc.extender_port[extender_index].extender_gport_id, &extender_port_match);
    if (rv != BCM_E_NONE) {
          printf("Error in bcm_port_match_delete\n");
          return rv;
    }

    if (delete_e_channel_reg_entry) {
        bcm_extender_forward_t_init(&fwd_entry);
        fwd_entry.flags = BCM_EXTENDER_FORWARD_UPSTREAM_ONLY;
        fwd_entry.name_space = g_port_extender_cb_uc.cascaded_port[extender_index].port;    /* vlan_domain */
        fwd_entry.extended_port_vid = extended_port_vid;                                    /* ecid */
        rv = bcm_extender_forward_delete(unit, &fwd_entry);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_extender_forward_add for port - %d, ecid - 0x%x rv - %d\n", cascaded_port_info->port, extended_port_vid, rv);
            return rv;
        }
    }

    return rv;
}

int port_extender_cb_uc_ut_extender_port_add(int unit, int initial_vid, int ecid)
{
    int rv;
    extender__port_extender_port_s ext_port;

    ext_port.port = 0;
    ext_port.tag_vid = initial_vid;
    ext_port.extender_vid = ecid;

    /* Configure untagged Port-Extender */
    rv = extender__extender_port_untagged_configure(unit, &ext_port);
    if (rv != BCM_E_NONE) {
        printf("Error in extender__extender_port_untagged_configure for LIF #%d, rv - %d\n", idx, rv);
        return rv;
    }

    /* Attach the Extender-Port to a specific VSI */
    rv = bcm_vswitch_port_add(unit, g_port_extender_cb_uc.vsi, ext_port.extender_gport_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_port_add, vsi - %d, extender_port_id - %d, rv - %d\n", g_port_extender_cb_uc.vsi,ext_port.extender_gport_id, rv);
    }
    printf("Allocated extender_port_id - %x\n", ext_port.extender_gport_id);

    return BCM_E_NONE;

}

/*
 * You can use this function to change the ports the cint is using from the defaults (13, 14 and 15).
 *
 * INPUT: unit              - unit
 *        in_cascaded_port  - cascaded port holding lifs 1 and 2
 *        out_cascaded_port - cascaded port holding lif 3
 *        end_station_port  - ethernet port holding lif 4
 */
int port_extender_cb_uc__start_run_wide_lif(int unit, int in_cascaded_port, int out_cascaded_port, int end_station_port, bcm_vlan_t new_vid){
    port_extender_cb_uc_s params;

    sal_memcpy(&params, &g_port_extender_cb_uc, sizeof (g_port_extender_cb_uc));

    params.cascaded_port[0].port = in_cascaded_port;
    params.cascaded_port[1].port = in_cascaded_port;
    params.cascaded_port[2].port = out_cascaded_port;

    params.end_station.port = end_station_port;

    params.extender_port[0].port = in_cascaded_port;
    params.extender_port[1].port = in_cascaded_port;
    params.extender_port[2].port = out_cascaded_port;

    params.new_vid = new_vid;
    is_extender_port_wide_lif = 1;

    return port_extender_cb_uc__start_run(unit, &params);
}

int
set_port_extender_lif_wide_data (int unit, int wide_data) {
    uint64 wd_data;
    int rv = BCM_E_NONE;
    COMPILER_64_SET(wd_data, 0, wide_data);

    int idx;
    for (idx = 0; idx < NOF_EXTENDER_PORTS; idx++) {
        rv = bcm_port_wide_data_set(unit, g_port_extender_cb_uc.extender_port[idx].extender_gport_id, BCM_PORT_WIDE_DATA_INGRESS, wd_data);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_wide_data_set\n");
            return rv;
        }
    }
    return rv;
}


