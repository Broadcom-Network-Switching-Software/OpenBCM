/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The CINT provides an example for 1+1 Ingress Protection learning                                                                                 *
 *                                                                                                                                                  *
 * Packets can be sent from the In-ACs towards the Out-AC to create learning entries for them.                                                      *
 * Packets from the Protection In-ACs (Primary & Secondary) are expected to invoke MC learning of both ACs.                                         *
 * Packets send from the Out-AC with the Protection In-AC MAC DA are expected to be forwarded to the ACs that participate in the learnt MC group,   *
 *   while if no learning was perfomed, flooding can address any of the defined ACs.                                                                *
 * A learning MC Group may be modified (on both Protection In-ACs) and take effect once a packet is sent from either of them.                       *
 *                                                                                                                                                  *                                                                                                                                                                                                 *
 *                                                                                                                                                  *                                                                                                                                                                                                 *
 *                                                  ____________________________________                                                            *
 *                                                 |              ____________          |                                                           *
 *                           Primary In-AC         |             |            |         |                                                           *
 *                           --------------------  | <--|        |  Learning  |         |                                                           *
 *                                                 |    |------- |  MC Group  |         |                                                           *
 *                           Secondary In-AC       |    | <-|    |____________|         |                                                           *
 *                           --------------------- | <--|   |                           |                                                           *
 *                                                 |        |     ____________      |-->|       Out-AC                                              *
 *                                                 |        |    |            |     |   | ------------                                              *
 *                                                 |        |    |  Flooding  |     |   |                                                           *
 *                           Non-Protected In-AC   | <---------- |  MC Group  |-----|   |                                                           *
 *                           --------------------- |             |____________|         |                                                           *
 *                                                 |                                    |                                                           *
 *                                                 |____________________________________|                                                           *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/dnx                                                                                                                  *
 * cint ../sand/utility/cint_sand_utils_global.c                                                                                                                 *
 * cint utility/cint_utils_l2.c                                                                                                                     *
 * cint utility/cint_utils_multicast.c                                                                                                              *
 * cint utility/cint_multi_device_utils.c                                                                                                           *
 * cint cint_ac_1plus1_protection_learning.c                                                                                                        *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * ac_1plus1_protection_learning__start_run(int unit,  ac_1plus1_protection_learning_s *param) - param = NULL for default params                    *
 *                                                                                                                                                  *
 * Sent and expected Packets:                                                                                                                       *
 *             +-----------------------------------------------------------+                                                                        *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI |                                                                        *
 *             |-----------------------------------------------------------|                                                                        *
 *             |    d_mac  |   s_mac   |   0x9100   |  6/7/8/9   |   any   |                                                                        *
 *             +-----------------------------------------------------------+                                                                        *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_ac_1plus1_protection_learning', before calling the main    *
 * function. In order to re-write only part of the values, call 'ac_1plus1_protection_learning_struct_get(1plus1_protection_learning_s)'            *
 * function and re-write the values prior to calling the main function.                                                                             *
 *                                                                                                                                                  *
 * Test name: AT_Sand_Cint_failover_ac_1plus1_protection_learning                                                                                   *
 */


/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
int FAILOVER_PRIMARY=0;
int FAILOVER_SECONDARY=1;
int NOF_FAILOVER_IDS=2;
int outer_tpid =0x9100;

struct protection_ac_s {
    bcm_gport_t port;
    bcm_vlan_t  vlan;
    bcm_gport_t vlan_port_id;
    bcm_if_t encap_id;
};

/*  Main Struct  */
struct ac_1plus1_protection_learning_s {
    protection_ac_s in_ac[NOF_FAILOVER_IDS];
    protection_ac_s out_ac;
    protection_ac_s non_protected_in_ac;
    bcm_mac_t mac_address_out;
    bcm_mac_t mac_address_in;
    bcm_mac_t mac_address_in_non_protected;
    bcm_multicast_t failover_mc_group;
    bcm_vlan_t vsi;
    bcm_failover_t ingress_failover_id;
    bcm_if_t fec_id;
};


/* Initialization of a global struct*/
ac_1plus1_protection_learning_s g_ac_1plus1_protection_learning = {

        /*  AC configurations
            Phy Port    VLAN    vlan_port_id    */
           {{ 200,      9,         0x44800010 }, /* In-ACs */
            { 201,      6,         0x44001001 }},
            { 202,      7,         0x44001002 }, /* Out-AC */
            { 203,      8,         0x44001020 }, /* Non-Protected-AC */

            /* Additional parameters */
            {0x00, 0x11, 0x00, 0x00, 0x00, 0x13},   /* MAC Address Out */
            {0x00, 0x33, 0x00, 0x00, 0x00, 0x33},   /* MAC Address In */
            {0x00, 0x55, 0x00, 0x00, 0x00, 0x66},   /* MAC Address Non-Protected */
            50,                                     /* faliover mc_group */
            20,                                     /* VSI */
            2,                                      /* Ingress failover_id */
            0xCCCE};                                /* FEC ID*/



/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT:
 *   params: new values for g_ac_1plus1_protection_learning
 */
int ac_1plus1_protection_learning_init(int unit,ac_1plus1_protection_learning_s *params) {

    int rv, flags,device_is_jericho2;
    bcm_multicast_t mc_group;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2, rv - %d\n", rv);
        return rv;
    }


    /* Create an Ingress failover ID */
    flags = BCM_FAILOVER_WITH_ID | BCM_FAILOVER_INGRESS;
    rv = bcm_failover_create(unit, flags, &g_ac_1plus1_protection_learning.ingress_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_1plus1_protection_learning_init failed during Ingress bcm_failover_create, rv - %d\n", rv);
        return rv;
    }

    /* Create a vswitch */
    rv = bcm_vswitch_create_with_id(unit, g_ac_1plus1_protection_learning.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_1plus1_protection_learning_init failed during bcm_vswitch_create for vsi %d, rv - %d\n", g_ac_1plus1_protection_learning.vsi, rv);
        return rv;
    }

    /* Create a flooding MC Group */
    mc_group = g_ac_1plus1_protection_learning.vsi;
    rv = multicast__open_mc_group(unit, &mc_group, (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2));
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, rv - %d\n");
        return rv;
    }

    /* Create a failover MC Group for the In-Ports */
    mc_group = g_ac_1plus1_protection_learning.failover_mc_group;

    rv = multicast__open_mc_group(unit, &mc_group, (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2));
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, rv - %d\n");
        return rv;
    }

    if ((soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0)&&
         is_device_or_above(unit, JERICHO2)) {
        /**
         *For JR2:
         *JR1 mode:, FEC is used as learning, FEC is created explicitly
         *JR2 mode: MC Group is used as learning,
         */

        create_l3_egress_s l3_egress;
        sal_memset(&l3_egress, 0, sizeof(l3_egress));
        l3_egress.allocation_flags = BCM_L3_WITH_ID;
        l3_egress.fec_id = device_is_jericho2 ? g_ac_1plus1_protection_learning.fec_id:(g_ac_1plus1_protection_learning.fec_id & 0x7FFF);
        BCM_GPORT_MCAST_SET(l3_egress.out_gport, g_ac_1plus1_protection_learning.failover_mc_group);
        rv = l3__egress_only_fec__create(unit, &l3_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress_only_fec__create for FEC\n");
            return rv;
        }
        g_ac_1plus1_protection_learning.fec_id = l3_egress.fec_id;
    }
    else
    {
       BCM_L3_ITF_SET(g_ac_1plus1_protection_learning.fec_id,  BCM_L3_ITF_TYPE_FEC, 0x1200);
    }

    return BCM_E_NONE;
}

/* JR1 vlan tranlation in avt mode.
*/
int ac_1plus1_protection_learning__set_ive_eve_translation(int unit,
                        bcm_gport_t lif,
                        int outer_tpid,
                        int inner_tpid,
                        bcm_vlan_tpid_action_t outer_tpid_action,
                        bcm_vlan_tpid_action_t inner_tpid_action,
                        bcm_vlan_action_t outer_action,
                        bcm_vlan_action_t inner_action,
                        bcm_vlan_t new_outer_vid,
                        bcm_vlan_t new_inner_vid,
                        uint32 vlan_edit_profile,
                        uint16 tag_format,
                        uint8 is_ive)
{
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;
    int action_id_1, rv;

    /* set edit profile for  ingress/egress LIF */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = new_outer_vid;
    port_trans.new_inner_vlan = new_inner_vid;
    port_trans.gport = lif;
    port_trans.vlan_edit_class_id = vlan_edit_profile;
    port_trans.flags = is_ive ? BCM_VLAN_ACTION_SET_INGRESS:BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /* Create action ID*/
    rv = bcm_vlan_translate_action_id_create(unit, (is_ive ? BCM_VLAN_ACTION_SET_INGRESS:BCM_VLAN_ACTION_SET_EGRESS), &action_id_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /* Set translation action */
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;

    action.outer_tpid_action = outer_tpid_action;
    action.inner_tpid_action = inner_tpid_action;
    action.dt_outer = outer_action;
    action.dt_inner = inner_action;
    rv = bcm_vlan_translate_action_id_set( unit,
                                           (is_ive ? BCM_VLAN_ACTION_SET_INGRESS:BCM_VLAN_ACTION_SET_EGRESS),
                                           action_id_1,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    /* Set translation action class (map edit_profile & tag_format to action_id) */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = vlan_edit_profile;
    action_class.tag_format_class_id = tag_format;
    action_class.vlan_translation_action_id	= action_id_1;
    action_class.flags = (is_ive ? BCM_VLAN_ACTION_SET_INGRESS:BCM_VLAN_ACTION_SET_EGRESS);
    rv = bcm_vlan_translate_action_class_set( unit,&action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }


    return BCM_E_NONE;
}

/* init JR1 port tpid 0x9100 as outer.
*/
int ac_1plus1_protection_learning__set_port_tpid(int unit,
                        bcm_port_t port,
                        int outer_tpid,
                        int inner_tpid,
                        int tag_format)
{
    bcm_port_tpid_class_t tpid_class;
    int rv;

    rv = bcm_port_tpid_delete_all(unit,port);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_tpid_delete_all, rv - %d\n", rv);
        return rv;
    }
    rv = bcm_port_tpid_add(unit,port,outer_tpid,0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_port_tpid_set, rv - %d\n", rv);
        return rv;
    }
    if (inner_tpid) {
        rv = bcm_port_tpid_add(unit,port,inner_tpid,0);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_port_tpid_set, rv - %d\n", rv);
            return rv;
        }
    }

    /* set tpid class ingress */
    bcm_port_tpid_class_t_init(&tpid_class);
    tpid_class.tpid1 = outer_tpid;
    if (inner_tpid) {
        tpid_class.tpid2 = inner_tpid;
    } else {
        tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    }
    tpid_class.port = port;
    tpid_class.tag_format_class_id = tag_format;
    tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }
    /* set tpid class egress */
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    return BCM_E_NONE;
}

/* Configuration of a non-protected AC.
 * Define the AC and attach it to a vswitch.
 *
 * INPUT:
 *   protection_ac_s: Configuration info for a single non-protected AC.
 */
int ac_1plus1_protection_learning__set_non_protected_ac(int unit, protection_ac_s *ac_info) {

    bcm_vlan_port_t vlan_port;
    int rv, mc_group;
    int device_is_jericho2;
    int is_local=1, local_port;

    rv = is_device_jericho2(unit, &device_is_jericho2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in is_device_jericho2, rv - %d\n", rv);
        return rv;
    }

    rv = system_port_is_local(unit, ac_info->port, &local_port, &is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in gport_is_local for JR2, rv = %d\n", rv);
        return rv;
    }
    printf("system port[%d]-local_port[%d]-is_local[%d]\n", ac_info->port, local_port, is_local);

    if (!device_is_jericho2) {
        rv = ac_1plus1_protection_learning__set_port_tpid(unit,ac_info->port,outer_tpid,0,2);
        if (rv != BCM_E_NONE) {
            printf("Error ac_1plus1_protection_learning__set_port_tpid, rv = %d\n", rv);
            return rv;
        }
    }


    /* Set the VLAN-Domain for the port */
    rv = bcm_port_class_set(unit, ac_info->port, bcmPortClassId,local_port);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_class_set for port %d, rv - %d\n", ac_info->port, rv);
        return rv;
    }


    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_info->port;
    vlan_port.match_vlan = ac_info->vlan;
    vlan_port.flags = BCM_VLAN_PORT_WITH_ID;
    vlan_port.vsi = 0;
    BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port.vlan_port_id, 0, ac_info->vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);


    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error failed during bcm_vlan_port_create, rv - %d\n", rv);
        return rv;
    }
    ac_info->vlan_port_id = vlan_port.vlan_port_id;
    ac_info->encap_id = vlan_port.encap_id;

    if (!device_is_jericho2) {
        rv = ac_1plus1_protection_learning__set_ive_eve_translation(unit,
                        ac_info->vlan_port_id,
                        outer_tpid,
                        0,
                        bcmVlanTpidActionModify,
                        0,
                        0,
                        0,
                        0,
                        0,
                        1,
                        2,
                        1);
        if (rv != BCM_E_NONE) {
            printf("Error in ac_1plus1_protection_learning__set_ive_eve_translation, rv - %d\n", rv);
            return rv;
        }
    }

    if (device_is_jericho2) {
        /*In jr2, vlan membership check is enabled by default. So add port to the vlan.
         *In jr1, the port can be added to the vlan too but needs the related egress mc-group to be open.
         */
        rv = bcm_vlan_gport_add(unit, ac_info->vlan, ac_info->port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, adding bcm_vlan_gport_add for vlan %d, port %d, rv - %d\n", ac_info->vlan, ac_info->port, rv);
            return rv;
        }
    }

    /* Attach the Out-AC to the vswitch */
    rv = bcm_vswitch_port_add(unit, g_ac_1plus1_protection_learning.vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_vswitch_port_add of gport 0x%08x to vsi %d, rv - %d\n", vlan_port.vlan_port_id, vlan_port.vsi, rv);
        return rv;
    }

    /* Attach to the flooding MC group */
    mc_group = g_ac_1plus1_protection_learning.vsi;
    rv = multicast__vlan_port_add(unit, mc_group, vlan_port.port, vlan_port.vlan_port_id, 0);
    if (rv != BCM_E_NONE) {
        printf("Error during multicast__vlan_port_add of gport 0x%08x to mc_group %d, rv - %d\n", vlan_port.vlan_port_id, mc_group, rv);
        return rv;
    }

    /* Open up the port for outgoing and ingoing traffic */
    rv = bcm_port_control_set(unit, ac_info->port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_control_set for port %d, rv - %d\n", ac_info->port, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Configuration of the Secondary In-AC.
 * Define the Secondary In-AC and attach them to a vswitch and a MC group.
 * The AC is defined with the appropriate failover_ids Ingress Protection.
 * INPUT:
 *   ac_1plus1_protection_learning_s: Configuration info for a Secondary In-AC.
 */
int ac_1plus1_protection_learning__set_secondary_in_ac(int unit1, int unit2,ac_1plus1_protection_learning_s *ac_1plus1_protection_learning_info) {

    bcm_vlan_port_t in_ac;
    bcm_vlan_port_t in_ac_info;
    int rv, mc_group;
    int unit1_is_jericho2,unit2_is_jericho2;
    int work_is_local=1, protect_is_local=1, local_port;
    bcm_gport_t fec_gport_id = BCM_GPORT_INVALID;

    rv = is_device_jericho2(unit1, &unit1_is_jericho2);
    if (rv != BCM_E_NONE){
        printf("Error in is_device_jericho2, rv - %d\n", rv);
        return rv;
    }
    rv = is_device_jericho2(unit2, &unit2_is_jericho2);
    if (rv != BCM_E_NONE){
        printf("Error in is_device_jericho2, rv - %d\n", rv);
        return rv;
    }

    rv = system_port_is_local(unit2, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, &local_port, &protect_is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in gport_is_local for JR2, rv - %d\n", rv);
        return rv;
    }
    printf("Protect:in_ac.port[%d]-local_port[%d]-is_local[%d]\n", ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, local_port, protect_is_local);

    if (!unit2_is_jericho2) {
        rv = ac_1plus1_protection_learning__set_port_tpid(unit2,ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port,outer_tpid,0,2);
        if (rv != BCM_E_NONE) {
            printf("Error ac_1plus1_protection_learning__set_port_tpid, rv = %d\n", rv);
            return rv;
        }
    }


    /* Set the VLAN-Domain for the ports */
    rv = bcm_port_class_set(unit2, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, bcmPortClassId,
                            local_port);
    if (rv != BCM_E_NONE) {
        printf("%Error during bcm_port_class_set for port %d, rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, rv);
        return rv;
    }

    rv = bcm_vlan_create(unit2, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_create for vlan - %d , rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan,  rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit2, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add for vlan - %d and port %d, rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, rv);
        return rv;
    }

    /* Open up the ports for outgoing and ingoing traffic */
    rv = bcm_port_control_set(unit2, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_control_set for port %d, rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port, rv);
        return rv;
    }

    /* Create the Protecting In-AC */
    bcm_vlan_port_t_init(&in_ac);
    in_ac.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_ac.flags = BCM_VLAN_PORT_WITH_ID;
    in_ac.port = ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port;
    in_ac.match_vlan = ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan;
    in_ac.vsi = 0;

    BCM_GPORT_SUB_TYPE_LIF_SET(in_ac.vlan_port_id, 0, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_ac.vlan_port_id, in_ac.vlan_port_id);

    in_ac.ingress_failover_id = ac_1plus1_protection_learning_info->ingress_failover_id;
    int failover_mc_group;
    BCM_MULTICAST_L2_SET(failover_mc_group,ac_1plus1_protection_learning_info->failover_mc_group);

    if (!is_device_or_above(unit2, JERICHO2))
    {
        in_ac.failover_port_id = ac_1plus1_protection_learning_info->fec_id;
        in_ac.failover_mc_group = failover_mc_group;
    }
    else
    {
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(fec_gport_id, ac_1plus1_protection_learning_info->fec_id);
        in_ac.failover_port_id = soc_property_get(unit2, spn_SYSTEM_HEADERS_MODE, 1)?0:fec_gport_id;
        in_ac.failover_mc_group = soc_property_get(unit2, spn_SYSTEM_HEADERS_MODE, 1)?failover_mc_group:0;
    }

    rv = bcm_vlan_port_create(unit2, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_create during Secondary In-AC creation, rv - %d\n", rv);
        return rv;
    }

    ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan_port_id = in_ac.vlan_port_id;
    ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].encap_id = in_ac.encap_id;
    printf("in_ac[FAILOVER_SECONDARY].vlan_port_id: %#x\n", ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan_port_id);

    /* Attach the Secondary In-AC to the vswitch */
    rv = bcm_vswitch_port_add(unit2, ac_1plus1_protection_learning_info->vsi, in_ac.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vswitch_port_add during Secondary bcm_vswitch_port_add to vsi %d, rv - %d\n", ac_1plus1_protection_learning_info->vsi, rv);
        return rv;
    }

    if (!unit2_is_jericho2) {
        rv = ac_1plus1_protection_learning__set_ive_eve_translation(unit2,
                        in_ac.vlan_port_id,
                        outer_tpid,
                        0,
                        bcmVlanTpidActionModify,
                        0,
                        0,
                        0,
                        0,
                        0,
                        1,
                        2,
                        1);
        if (rv != BCM_E_NONE) {
            printf("Error in ac_1plus1_protection_learning__set_ive_eve_translation, rv - %d\n", rv);
            return rv;
        }
    }

    /* Attach to a flooding group */
    mc_group = ac_1plus1_protection_learning_info->vsi;
    rv = multicast__vlan_port_add(unit2, mc_group, in_ac.port, in_ac.vlan_port_id, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, flooding multicast__vlan_port_add, rv - %d\n", rv);
        return rv;
    }

    /* Attach to a failover MC group */
    mc_group = ac_1plus1_protection_learning_info->failover_mc_group;
    rv = multicast__vlan_port_add(unit2, mc_group, in_ac.port, in_ac.vlan_port_id, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, failover multicast__vlan_port_add, rv - %d\n", rv);
        return rv;
    }

    if (unit1 != unit2) {

        rv = bcm_vlan_create(unit1, ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_create for vlan - %d , rv - %d\n",
                   ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan,  rv);
            return rv;
        }

        if (!unit1_is_jericho2) {

            /* Create the Protecting In-AC on remote devices */
            bcm_vlan_port_t_init(&in_ac);

            in_ac.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            in_ac.flags = BCM_VLAN_PORT_WITH_ID;
            in_ac.port = ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].port;
            in_ac.match_vlan = ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan;
            in_ac.vsi = 0;
            in_ac.vlan_port_id = ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan_port_id;

            in_ac.ingress_failover_id = ac_1plus1_protection_learning_info->ingress_failover_id;
            int failover_mc_group;
            BCM_MULTICAST_L2_SET(failover_mc_group,ac_1plus1_protection_learning_info->failover_mc_group);
            in_ac.failover_mc_group = failover_mc_group;
            in_ac.failover_port_id = unit1_is_jericho2 ? 0: ac_1plus1_protection_learning_info->fec_id;

            rv = bcm_vlan_port_create(unit1, &in_ac);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_vlan_port_create during Secondary In-AC creation, rv - %d\n", rv);
                return rv;
            }

            /* Attach the Secondary In-AC to the vswitch */
            rv = bcm_vswitch_port_add(unit1, g_ac_1plus1_protection_learning.vsi, in_ac.vlan_port_id);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_vswitch_port_add during Secondary bcm_vswitch_port_add to vsi %d, rv - %d\n", ac_1plus1_protection_learning_info->vsi, rv);
                return rv;
            }
        }
        /* Attach to the flooding MC group */
        mc_group = g_ac_1plus1_protection_learning.vsi;
        rv = multicast__add_multicast_entry(unit1, mc_group, g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].port, &g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, flooding multicast__vlan_port_add, rv - %d\n", rv);
            return rv;
        }

        /* Attach to a failover MC group */
        mc_group = g_ac_1plus1_protection_learning.failover_mc_group;
        rv = multicast__add_multicast_entry(unit1, mc_group, g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].port, &g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, failover multicast__vlan_port_add, rv - %d\n", rv);
            return rv;
        }

    }

    return BCM_E_NONE;
}

/* Configuration of the Primary In-AC.
 * Define the Primary In-AC and attach them to a vswitch and a MC group.
 * The AC is defined with the appropriate failover_ids Ingress Protection.
 * INPUT:
 *   ac_1plus1_protection_learning_s: Configuration info for Primary In-AC.
 */
int ac_1plus1_protection_learning__set_primary_in_ac(int unit1, int unit2,ac_1plus1_protection_learning_s *ac_1plus1_protection_learning_info) {

    bcm_vlan_port_t in_ac;
    bcm_vlan_port_t in_ac_info;
    int rv, mc_group;
    int unit1_is_jericho2,unit2_is_jericho2;
    int work_is_local=1, protect_is_local=1, local_port;
    bcm_gport_t fec_gport_id = BCM_GPORT_INVALID;



    rv = is_device_jericho2(unit1, &unit1_is_jericho2);
    if (rv != BCM_E_NONE){
        printf("Error in is_device_jericho2, rv - %d\n", rv);
        return rv;
    }
    rv = is_device_jericho2(unit2, &unit2_is_jericho2);
    if (rv != BCM_E_NONE){
        printf("Error in is_device_jericho2, rv - %d\n", rv);
        return rv;
    }


    rv = system_port_is_local(unit1, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, &local_port, &work_is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in system_port_is_local for JR2, rv - %d\n", rv);
        return rv;
    }
    printf("Working: in_ac.port[%d]-local_port[%d]-is_local[%d]\n", ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, local_port, work_is_local);

    if (!unit1_is_jericho2) {
        rv = ac_1plus1_protection_learning__set_port_tpid(unit1,ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port,outer_tpid,0,2);
        if (rv != BCM_E_NONE) {
            printf("Error ac_1plus1_protection_learning__set_port_tpid, rv = %d\n", rv);
            return rv;
        }
    }

    rv = bcm_port_class_set(unit1, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, bcmPortClassId,
                            local_port);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_class_set for port %d, rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, rv);
        return rv;
    }


    rv = bcm_vlan_gport_add(unit1, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add for vlan - %d and port %d, rv - %d\n",
            ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, rv);
        return rv;
    }


    rv = bcm_port_control_set(unit1, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_control_set for port %d, rv - %d\n",
               ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port, rv);
        return rv;
    }



    /* Create the Primary In-AC */
    bcm_vlan_port_t_init(&in_ac);
    in_ac.flags = BCM_VLAN_PORT_WITH_ID;
    in_ac.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_ac.port = ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].port;
    in_ac.match_vlan = ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan;
    in_ac.vsi = 0;

    BCM_GPORT_SUB_TYPE_LIF_SET(in_ac.vlan_port_id, 0, ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_ac.vlan_port_id, in_ac.vlan_port_id);

    int failover_mc_group;
    BCM_MULTICAST_L2_SET(failover_mc_group,ac_1plus1_protection_learning_info->failover_mc_group);
    in_ac.ingress_failover_id = ac_1plus1_protection_learning_info->ingress_failover_id;
    in_ac.ingress_failover_port_id = ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan_port_id ;

    if (!is_device_or_above(unit1, JERICHO2))
    {
        in_ac.failover_port_id = ac_1plus1_protection_learning_info->in_ac[FAILOVER_SECONDARY].vlan_port_id;
        in_ac.failover_mc_group = failover_mc_group;
    }
    else
    {
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(fec_gport_id, ac_1plus1_protection_learning_info->fec_id);
        in_ac.failover_port_id = soc_property_get(unit1, spn_SYSTEM_HEADERS_MODE, 1)?0:fec_gport_id;
        in_ac.failover_mc_group = soc_property_get(unit1, spn_SYSTEM_HEADERS_MODE, 1)?failover_mc_group:0;
    }

    rv = bcm_vlan_port_create(unit1, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_vlan_port_create during Primary In-AC creation, rv - %d\n", rv);
        return rv;
    }

    ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan_port_id = in_ac.vlan_port_id;
    ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].encap_id = in_ac.encap_id;
    printf("in_ac[FAILOVER_PRIMARY].vlan_port_id: %#x\n", ac_1plus1_protection_learning_info->in_ac[FAILOVER_PRIMARY].vlan_port_id);

    if (!unit1_is_jericho2) {
            rv = ac_1plus1_protection_learning__set_ive_eve_translation(unit1,
                            in_ac.vlan_port_id,
                            outer_tpid,
                            0,
                            bcmVlanTpidActionModify,
                            0,
                            0,
                            0,
                            0,
                            0,
                            1,
                            2,
                            1);
            if (rv != BCM_E_NONE) {
                printf("Error in ac_1plus1_protection_learning__set_ive_eve_translation, rv - %d\n", rv);
                return rv;
            }
        }


    /* Attach the Primary In-AC to the vswitch */
    rv = bcm_vswitch_port_add(unit1, ac_1plus1_protection_learning_info->vsi, in_ac.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error during Primary bcm_vswitch_port_add for vsi %d, rv - %d\n", ac_1plus1_protection_learning_info->vsi, rv);
        return rv;
    }

    /* Attach to a flooding group */
    mc_group = ac_1plus1_protection_learning_info->vsi;
    rv = multicast__vlan_port_add(unit1, mc_group, in_ac.port, in_ac.vlan_port_id, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, flooding multicast__vlan_port_add, rv - %d\n", rv);
        return rv;
    }

    /* Attach to a failover MC group */
    mc_group = ac_1plus1_protection_learning_info->failover_mc_group;
    rv = multicast__vlan_port_add(unit1, mc_group, in_ac.port, in_ac.vlan_port_id, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, failover multicast__vlan_port_add, rv - %d\n", rv);
        return rv;
    }

    if (unit2 != unit1) {

        if (!unit2_is_jericho2) {
            /* Create the Protecting In-AC on remote devices */
            rv = bcm_vlan_port_create(unit2, &in_ac);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_vlan_port_create during Secondary In-AC creation, rv - %d\n", rv);
                return rv;
            }

            printf("local.vlan_port_id: %#x\n", in_ac.vlan_port_id);
            printf("local.encap_id: %#x\n", in_ac.encap_id);

            /* Attach the Secondary In-AC to the vswitch */
            rv = bcm_vswitch_port_add(unit2, g_ac_1plus1_protection_learning.vsi, in_ac.vlan_port_id);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_vswitch_port_add during Secondary bcm_vswitch_port_add to vsi %d, rv - %d\n", in_ac.vsi, rv);
                return rv;
            }
        }

        /* Attach to the flooding MC group */
        mc_group = g_ac_1plus1_protection_learning.vsi;
        rv = multicast__add_multicast_entry(unit2, mc_group, g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].port, &g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, flooding multicast__vlan_port_add, rv - %d\n", rv);
            return rv;
        }

        /* Attach to a failover MC group */
        mc_group = g_ac_1plus1_protection_learning.failover_mc_group;
        rv = multicast__add_multicast_entry(unit2, mc_group, g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].port, &g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, failover multicast__vlan_port_add, rv - %d\n", rv);
            return rv;
        }

    }
    /* FEC ID in learning payload is 15 bits,whereas the FEC ID in JR2 is 18 bits wide.
     * this will tranlsate the global FEC ID from learning
     * payload to local FEC ID used for local FEC access.*/
    rv = fec_id_alignment_field_wa_add(unit1, local_port, BCM_L3_ITF_VAL_GET(g_ac_1plus1_protection_learning.fec_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error, fec_id_alignment_field_wa_entry_add\n");
        return rv;
    }

    return BCM_E_NONE;
}

/* Retrieve the initial confgiuration for the example.
 *
 * INPUT:
 *   protection_ac_s: Configuration info for a single In-AC.
 */
void ac_1plus1_protection_learning__struct_get(ac_1plus1_protection_learning_s *param) {

    sal_memcpy(param, &g_ac_1plus1_protection_learning, sizeof(*param));
    return;
}


/* Main function that performs all the configurations for the example.
 * Performs init configuration before configuring the ACs
 * INPUT:
 *   unit1: out_ac, primary_ac locating
 *   unit2: non_protected_in_ac, secondary_ac locating
 *   params: Configuration info for running the example.
 */
int ac_1plus1_protection_learning__start_run(int unit1, int unit2, ac_1plus1_protection_learning_s *params) {

    int rv, unit, mc_group;
    int is_local, local_port;
    int unit1_is_jericho2, unit2_is_jericho2 ;
    uint32 *nof_rifs;
    char *proc_name;


    if (params != NULL) {
       sal_memcpy(&g_ac_1plus1_protection_learning, params, sizeof(g_ac_1plus1_protection_learning));
    }

    rv = is_device_jericho2(unit1, &unit1_is_jericho2);
    if (rv != BCM_E_NONE){
        printf("Error in is_device_jericho2, rv - %d\n", rv);
        return rv;
    }
    rv = is_device_jericho2(unit2, &unit2_is_jericho2);
    if (rv != BCM_E_NONE){
        printf("Error in is_device_jericho2, rv - %d\n", rv);
        return rv;
    }

    /* LIF idx must be greater than nof RIFs */
     if (!unit1_is_jericho2) {
         g_ac_1plus1_protection_learning.out_ac.vlan_port_id += SOC_DPP_DEFS_MAX_NOF_RIFS();
         g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].vlan_port_id += SOC_DPP_DEFS_MAX_NOF_RIFS();
     } else {
         nof_rifs=dnxc_data_get(unit1, "l3", "rif", "nof_rifs", NULL);
         g_ac_1plus1_protection_learning.out_ac.vlan_port_id += *nof_rifs;
         g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].vlan_port_id += *nof_rifs;
     }

     if (!unit2_is_jericho2) {
         g_ac_1plus1_protection_learning.non_protected_in_ac.vlan_port_id += SOC_DPP_DEFS_MAX_NOF_RIFS();
         g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].vlan_port_id += SOC_DPP_DEFS_MAX_NOF_RIFS();
     } else {
         nof_rifs=dnxc_data_get(unit2, "l3", "rif", "nof_rifs", NULL);
         g_ac_1plus1_protection_learning.non_protected_in_ac.vlan_port_id += *nof_rifs;
         g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].vlan_port_id += *nof_rifs;
     }


    /* Initialize the test parameters */
    rv = ac_1plus1_protection_learning_init(unit1,params);
    if (rv != BCM_E_NONE) {
        printf("Error in ac_1plus1_protection_learning_init, rv - %d\n", rv);
        return rv;
    }
    if (unit2 != unit1) {
        rv = ac_1plus1_protection_learning_init(unit2,params);
        if (rv != BCM_E_NONE) {
            printf("Error in ac_1plus1_protection_learning_init, rv - %d\n", rv);
            return rv;
        }
    }

    /* Configure the Out-AC */
    proc_name = "ac_1plus1_protection_learning__set_non_protected_ac";
    rv = ac_1plus1_protection_learning__set_non_protected_ac(unit1, &g_ac_1plus1_protection_learning.out_ac);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, failed to create an Out-AC, rv - %d\n", proc_name, rv);
        return rv;
    }
    printf("out_ac.vlan_port_id: %#x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);

    if (unit2 != unit1) {
        /* Attach to the flooding MC group */
        mc_group = g_ac_1plus1_protection_learning.vsi;
        rv = multicast__add_multicast_entry(unit2, mc_group, g_ac_1plus1_protection_learning.out_ac.port, &g_ac_1plus1_protection_learning.out_ac.encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error during multicast__vlan_port_add of gport 0x%08x to mc_group %d, rv - %d\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id, mc_group, rv);
            return rv;
        }

    }


    /* Configure the Non-Protected In-AC */
    rv = ac_1plus1_protection_learning__set_non_protected_ac(unit2, &g_ac_1plus1_protection_learning.non_protected_in_ac);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, failed to create a non-protected In-AC, rv - %d\n", proc_name, rv);
        return rv;
    }
    printf("non_protected_in_ac.vlan_port_id: %#x\n", g_ac_1plus1_protection_learning.non_protected_in_ac.vlan_port_id);

    if (unit2 !=unit1) {
        /* Attach to the flooding MC group */
        mc_group = g_ac_1plus1_protection_learning.vsi;
        rv = multicast__add_multicast_entry(unit1, mc_group, g_ac_1plus1_protection_learning.non_protected_in_ac.port, &g_ac_1plus1_protection_learning.non_protected_in_ac.encap_id, 1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error during multicast__vlan_port_add of gport 0x%08x to mc_group %d, rv - %d\n", g_ac_1plus1_protection_learning.non_protected_in_ac.vlan_port_id, mc_group, rv);
            return rv;
        }
    }

    /* Configure the secondary In-ACs */
    proc_name = "ac_1plus1_protection_learning__set_secondary_in_ac";
    rv = ac_1plus1_protection_learning__set_secondary_in_ac(unit1,unit2, &g_ac_1plus1_protection_learning);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, failed to create the protected In-ACs, rv - %d\n", proc_name, rv);
        return rv;
    }

    /* Configure the primary In-ACs */
    proc_name = "ac_1plus1_protection_learning__set_primary_in_ac";
    rv = ac_1plus1_protection_learning__set_primary_in_ac(unit1,unit2, &g_ac_1plus1_protection_learning);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, failed to create the protecting In-ACs, rv - %d\n", proc_name, rv);
        return rv;
    }


    return BCM_E_NONE;
}


/* This function runs the main test function with specified ports
 *
 * INPUT: in_port_primary - ingress primary port
 *        in_port_secondary - ingress secondary port
 *        out_port - egress port
 *        non_protected_in_port - Non-Protected In-Port
 */
int ac_1plus1_protection__learning_with_ports__start_run(int unit1, int unit2, int in_port_primary, int in_port_secondary, int out_port, int non_protected_in_port) {
    int rv;

    ac_1plus1_protection_learning_s param;

    ac_1plus1_protection_learning__struct_get(&param);

    param.in_ac[FAILOVER_PRIMARY].port = in_port_primary;
    param.in_ac[FAILOVER_SECONDARY].port = in_port_secondary;
    param.out_ac.port = out_port;
    param.non_protected_in_ac.port = non_protected_in_port;

    return ac_1plus1_protection_learning__start_run(unit1, unit2, &param);
}


/* Performs failover mc setting for the two protection In-ACs to modify the learning object
 *
 * INPUT: failover_mc_group  - The MC group to set as the learning object
 */
int ac_1plus1_protection_learning__failover_mc_set(int unit1, int unit2, int failover_mc_group) {

    int rv,unit1_is_jericho2;
    bcm_vlan_port_t in_ac;

    rv = is_device_jericho2(unit1, &unit1_is_jericho2);
    if (rv != BCM_E_NONE){
        printf("Error in is_device_jericho2, rv - %d\n", rv);
        return rv;
    }



    /* Retrieve the Secondary In-AC and Replace the failover_mc_group value */
    bcm_vlan_port_t_init(&in_ac);
    in_ac.vlan_port_id = g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].vlan_port_id;
    rv = bcm_vlan_port_find(unit2, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_find for Secondary In-AC gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
        return rv;
    }

    in_ac.failover_mc_group = failover_mc_group;
    in_ac.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
    rv = bcm_vlan_port_create(unit2, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, replacing failover_mc_group Secondary In-AC gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
        return rv;
    }

    /* Retrieve the Primary In-AC and Replace the failover_mc_group value */
    bcm_vlan_port_t_init(&in_ac);
    in_ac.vlan_port_id = g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].vlan_port_id;
    rv = bcm_vlan_port_find(unit1, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_find for the Primary In-AC gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
        return rv;
    }

    in_ac.failover_mc_group = unit1_is_jericho2 ? 0 : failover_mc_group;
    in_ac.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
    rv = bcm_vlan_port_create(unit1, &in_ac);
    if (rv != BCM_E_NONE) {
        printf("Error, replacing failover_mc_group for Primary In-AC gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
        return rv;
    }

    return BCM_E_NONE;
}

int ac_1plus1_protection_learning__cleanup(int unit1, int unit2) {

    int rv;

    rv = bcm_vlan_port_destroy(unit2, g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_ac_1plus1_protection_learning.in_ac[FAILOVER_SECONDARY].vlan_port_id);
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit1, g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_ac_1plus1_protection_learning.in_ac[FAILOVER_PRIMARY].vlan_port_id);
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit2, g_ac_1plus1_protection_learning.non_protected_in_ac.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_ac_1plus1_protection_learning.non_protected_in_ac.vlan_port_id);
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit1, g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
        return rv;
    }



    rv = bcm_failover_destroy(unit1, g_ac_1plus1_protection_learning.ingress_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
        return rv;
    }

    rv = bcm_multicast_destroy(unit1, g_ac_1plus1_protection_learning.failover_mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
        return rv;
    }

    rv = bcm_multicast_destroy(unit1, g_ac_1plus1_protection_learning.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
        return rv;
    }

    if (unit2 != unit1) {

        rv = bcm_failover_destroy(unit2, g_ac_1plus1_protection_learning.ingress_failover_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
            return rv;
        }

        rv = bcm_multicast_destroy(unit2, g_ac_1plus1_protection_learning.failover_mc_group);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
            return rv;
        }

        rv = bcm_multicast_destroy(unit2, g_ac_1plus1_protection_learning.vsi);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
            return rv;
        }
    }
    return BCM_E_NONE;
}
