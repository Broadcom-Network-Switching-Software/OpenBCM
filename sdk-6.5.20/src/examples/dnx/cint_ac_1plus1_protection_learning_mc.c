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
 * cint cint_ac_1plus1_protection_learning_mc.c                                                                                                        *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * ac_1plus1_protection_learning_mc__start_run(int unit,  ac_1plus1_protection_learning_mc_s *param) - param = NULL for default params                    *
 *                                                                                                                                                  *
 * Sent and expected Packets:                                                                                                                       *
 *             +-----------------------------------------------------------+                                                                        *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI |                                                                        *
 *             |-----------------------------------------------------------|                                                                        *
 *             |    d_mac  |   s_mac   |   0x9100   |  6/7/8/9   |   any   |                                                                        *
 *             +-----------------------------------------------------------+                                                                        *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_ac_1plus1_protection_learning_mc', before calling the main    *
 * function. In order to re-write only part of the values, call 'ac_1plus1_protection_learning_mc_struct_get(1plus1_protection_learning_mc_s)'            *
 * function and re-write the values prior to calling the main function.                                                                             *
 *                                                                                                                                                  *
 * Test name: AT_Dnx_Cint_vswitch_ac_1plus1_protection_learning_mc                                                                                   *
 */


/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
int FAILOVER_PRIMARY=0;
int FAILOVER_SECONDARY=1;
int NOF_FAILOVER_IDS=2;

struct protection_ac_s {
    bcm_gport_t port;
    bcm_vlan_t  vlan;
    bcm_gport_t vlan_port_id;
};

/*  Main Struct  */
struct ac_1plus1_protection_learning_mc_s {
    protection_ac_s in_ac[NOF_FAILOVER_IDS];
    protection_ac_s out_ac;
    protection_ac_s non_protected_in_ac;
    bcm_mac_t mac_address_out;
    bcm_mac_t mac_address_in;
    bcm_mac_t mac_address_in_non_protected;
    bcm_multicast_t failover_mc_group;
    bcm_vlan_t vsi;
    bcm_failover_t ingress_failover_id;
};


/* Initialization of a global struct*/
ac_1plus1_protection_learning_mc_s g_ac_1plus1_protection_learning_mc = {

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
            2};                                     /* Ingress failover_id */



/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT:
 *   params: new values for g_ac_1plus1_protection_learning_mc
 */
int ac_1plus1_protection_learning_mc_init(int unit, ac_1plus1_protection_learning_mc_s *params) {

    int rv, flags;
    bcm_multicast_t mc_group;
    uint32 *nof_rifs;

    if (params != NULL) {
       sal_memcpy(&g_ac_1plus1_protection_learning_mc, params, sizeof(g_ac_1plus1_protection_learning_mc));
    }

    /* LIF idx must be greater than nof RIFs */
    nof_rifs=dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL);
    g_ac_1plus1_protection_learning_mc.in_ac[FAILOVER_PRIMARY].vlan_port_id += *nof_rifs;
    g_ac_1plus1_protection_learning_mc.in_ac[FAILOVER_SECONDARY].vlan_port_id += *nof_rifs;


    /* Create an Ingress failover ID */
    flags = BCM_FAILOVER_WITH_ID | BCM_FAILOVER_INGRESS;
    rv = bcm_failover_create(unit, flags, &g_ac_1plus1_protection_learning_mc.ingress_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, ac_1plus1_protection_learning_mc_init failed during Ingress bcm_failover_create, rv - %d\n", rv);
        return rv;

    }

    /* Create a vswitch */
    rv = bcm_vswitch_create_with_id(unit, g_ac_1plus1_protection_learning_mc.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create for vsi %d, rv - %d\n", g_ac_1plus1_protection_learning_mc.vsi, rv);
        return rv;
    }

    /* Create a flooding MC Group */
    mc_group = g_ac_1plus1_protection_learning_mc.vsi;
    rv = multicast__open_mc_group(unit, &mc_group, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, rv - %d\n");
        return rv;
    }

    /* Create a failover MC Group for the In-Ports */
    mc_group = g_ac_1plus1_protection_learning_mc.failover_mc_group;
    rv = multicast__open_mc_group(unit, &mc_group, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, rv - %d\n");
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
int ac_1plus1_protection_learning_mc__set_non_protected_ac(int unit, protection_ac_s *ac_info) {

    bcm_vlan_port_t vlan_port;
    int rv, mc_group;
    int is_local=1, local_port;


    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_info->port;
    vlan_port.match_vlan = ac_info->vlan;

    vlan_port.vsi = 0;
    BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port.vlan_port_id, 0, ac_info->vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);

    rv = system_port_is_local(unit, vlan_port.port, &local_port, &is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in gport_is_local for JR2, rv = %d\n", rv);
        return rv;
    }
    printf("vlan_port.port[%d]-local_port[%d]-is_local[%d]\n", vlan_port.port, local_port, is_local);
    if (!is_local) {
        printf("Skip non local vlan_port create\n");
        return BCM_E_NONE;
    }


    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error failed during bcm_vlan_port_create, rv - %d\n", rv);
        return rv;
    }
    ac_info->vlan_port_id = vlan_port.vlan_port_id;

    /*In jr2, vlan membership check is enabled by default. So add port to the vlan.
     */
    rv = bcm_vlan_gport_add(unit, ac_info->vlan, ac_info->port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, adding bcm_vlan_gport_add for vlan %d, port %d, rv - %d\n", ac_info->vlan, ac_info->port, rv);
        return rv;
    }


    /* Attach the Out-AC to the vswitch */
    rv = bcm_vswitch_port_add(unit, g_ac_1plus1_protection_learning_mc.vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_vswitch_port_add of gport 0x%08x to vsi %d, rv - %d\n", vlan_port.vlan_port_id, vlan_port.vsi, rv);
        return rv;
    }

    /* Attach to the flooding MC group */
    mc_group = g_ac_1plus1_protection_learning_mc.vsi;
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

    /* Set the VLAN-Domain for the port */
    rv = bcm_port_class_set(unit, ac_info->port, bcmPortClassId, local_port);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_class_set for port %d, rv - %d\n", ac_info->port, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Configuration of the protection In-ACs.
 * Define the Primary & Secondary In-ACs and attach them to a vswitch and a MC group.
 * The ACs are defined with the appropriate failover_ids Ingress Protection.
 * INPUT:
 *   protection_ac_s: Configuration info for a couple of protection In-ACs.
 */
int ac_1plus1_protection_learning_mc__set_protected_in_acs(int unit, ac_1plus1_protection_learning_mc_s *ac_1plus1_protection_learning_mc_info) {

    bcm_vlan_port_t in_ac;
    bcm_vlan_port_t in_ac_info;
    int rv, mc_group;
    int work_is_local=1, protect_is_local=1, local_port;

    /* Create the Protecting In-AC */
    bcm_vlan_port_t_init(&in_ac);
    in_ac.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_ac.flags = BCM_VLAN_PORT_WITH_ID;
    in_ac.port = ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].port;
    in_ac.match_vlan = ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].vlan;
    in_ac.vsi = 0;
    BCM_GPORT_SUB_TYPE_LIF_SET(in_ac.vlan_port_id, 0, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_ac.vlan_port_id, in_ac.vlan_port_id);

    in_ac.ingress_failover_id = ac_1plus1_protection_learning_mc_info->ingress_failover_id;
    in_ac.failover_mc_group = ac_1plus1_protection_learning_mc_info->failover_mc_group;

    rv = system_port_is_local(unit, in_ac.port, &local_port, &protect_is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in gport_is_local for JR2, rv - %d\n", rv);
        return rv;
    }
    printf("Protect:in_ac.port[%d]-local_port[%d]-is_local[%d]\n", in_ac.port, local_port, protect_is_local);

    rv = bcm_vlan_gport_add(unit, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].vlan, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add for vlan - %d and port %d, rv - %d\n",
               ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].vlan, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].port, rv);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].vlan, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add for vlan - %d and port %d, rv - %d\n",
               ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].vlan, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].port, rv);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].vlan, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add for vlan - %d and port %d, rv - %d\n",
               ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].vlan, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].port, rv);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].vlan, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add for vlan - %d and port %d, rv - %d\n",
               ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].vlan, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].port, rv);
        return rv;
    }

    /* Open up the ports for outgoing and ingoing traffic */
    rv = bcm_port_control_set(unit, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_control_set for port %d, rv - %d\n",
               ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].port, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_control_set for port %d, rv - %d\n",
               ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].port, rv);
        return rv;
    }
    /* Set the VLAN-Domain for the ports */
    rv = bcm_port_class_set(unit, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].port, bcmPortClassId,
                            local_port);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_class_set for port %d, rv - %d\n",
               ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].port, rv);
        return rv;
    }


    if (!protect_is_local) {
        printf("Port is remote system port, skip create protect AC\n");
    } else {
        rv = bcm_vlan_port_create(unit, &in_ac);
        if (rv != BCM_E_NONE) {
            printf("Error during Secondary In-AC creation, rv - %d\n", rv);
            return rv;
        }

        ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].vlan_port_id = in_ac.vlan_port_id;
        printf("in_ac[FAILOVER_SECONDARY].vlan_port_id: %#x\n", ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].vlan_port_id);
        printf("in_ac[FAILOVER_PRIMARY].vlan_port_id: %#x\n", ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].vlan_port_id);

        /* Attach the Secondary In-AC to the vswitch */
        rv = bcm_vswitch_port_add(unit, ac_1plus1_protection_learning_mc_info->vsi, in_ac.vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error during Secondary bcm_vswitch_port_add to vsi %d, rv - %d\n", in_ac.vsi, rv);
            return rv;
        }

        /* Attach to a flooding group */
        mc_group = ac_1plus1_protection_learning_mc_info->vsi;
        rv = multicast__vlan_port_add(unit, mc_group, in_ac.port, in_ac.vlan_port_id, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, flooding multicast__vlan_port_add, rv - %d\n", rv);
            return rv;
        }

        /* Attach to a failover MC group */
        mc_group = ac_1plus1_protection_learning_mc_info->failover_mc_group;
        rv = multicast__vlan_port_add(unit, mc_group, in_ac.port, in_ac.vlan_port_id, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, failover multicast__vlan_port_add, rv - %d\n", rv);
            return rv;
        }

        printf("In-AC Protecting = 0x%08x\n", in_ac.vlan_port_id);
    }

    /* Create the Primary In-AC */
    bcm_vlan_port_t_init(&in_ac);
    in_ac.flags = BCM_VLAN_PORT_WITH_ID;
    in_ac.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_ac.port = ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].port;
    in_ac.match_vlan = ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].vlan;
    in_ac.vsi = 0;
    in_ac.failover_id = 0;
    BCM_GPORT_SUB_TYPE_LIF_SET(in_ac.vlan_port_id, 0, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(in_ac.vlan_port_id, in_ac.vlan_port_id);

    in_ac.failover_mc_group = ac_1plus1_protection_learning_mc_info->failover_mc_group;
    in_ac.ingress_failover_id = ac_1plus1_protection_learning_mc_info->ingress_failover_id;
    in_ac.ingress_failover_port_id = ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].vlan_port_id;

    rv = system_port_is_local(unit, in_ac.port, &local_port, &work_is_local);
    if (rv != BCM_E_NONE) {
        printf("Error in gport_is_local for JR2, rv - %d\n", rv);
        return rv;
    }
    printf("Working: in_ac.port[%d]-local_port[%d]-is_local[%d]\n", in_ac.port, local_port, work_is_local);


    rv = bcm_port_class_set(unit, ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].port, bcmPortClassId,
                            local_port);
    if (rv != BCM_E_NONE) {
        printf("Error during bcm_port_class_set for port %d, rv - %d\n",
                ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].port, rv);
        return rv;
    }

    if (!work_is_local) {
        printf("Port is remote system port, skip creating Primary In-AC\n");
    } else {
        rv = bcm_vlan_port_create(unit, &in_ac);
        if (rv != BCM_E_NONE) {
            printf("Error during Primary In-AC creation, rv - %d\n", rv);
            return rv;
        }
    }

    if (!work_is_local) {
        printf("Skip adding Primary In-AC to vswitch and MC group\n");
    } else {
        ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].vlan_port_id = in_ac.vlan_port_id;

        printf("in_ac[FAILOVER_PRIMARY].vlan_port_id: %#x\n", ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_PRIMARY].vlan_port_id);
        printf("in_ac[FAILOVER_SECONDARY].vlan_port_id: %#x\n", ac_1plus1_protection_learning_mc_info->in_ac[FAILOVER_SECONDARY].vlan_port_id);

        /* Attach the Primary In-AC to the vswitch */
        rv = bcm_vswitch_port_add(unit, ac_1plus1_protection_learning_mc_info->vsi, in_ac.vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error during Primary bcm_vswitch_port_add for vsi %d, rv - %d\n", in_ac.vsi, rv);
            return rv;
        }

        /* Attach to a flooding group */
        mc_group = ac_1plus1_protection_learning_mc_info->vsi;
        rv = multicast__vlan_port_add(unit, mc_group, in_ac.port, in_ac.vlan_port_id, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, flooding multicast__vlan_port_add, rv - %d\n", rv);
            return rv;
        }

        /* Attach to a failover MC group */
        mc_group = ac_1plus1_protection_learning_mc_info->failover_mc_group;
        rv = multicast__vlan_port_add(unit, mc_group, in_ac.port, in_ac.vlan_port_id, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, failover multicast__vlan_port_add, rv - %d\n", rv);
            return rv;
        }

        printf("In-AC Working = 0x%08x\n", in_ac.vlan_port_id);
    }

    return BCM_E_NONE;
}


/* Retrieve the initial confgiuration for the example.
 *
 * INPUT:
 *   protection_ac_s: Configuration info for a single In-AC.
 */
void ac_1plus1_protection_learning_mc__struct_get(ac_1plus1_protection_learning_mc_s *param) {

    sal_memcpy(param, &g_ac_1plus1_protection_learning_mc, sizeof(*param));
    return;
}


/* Main function that performs all the configurations for the example.
 * Performs init configuration before configuring the ACs
 * INPUT:
 *   params: Configuration info for running the example.
 */
int ac_1plus1_protection_learning_mc__start_run(int *unit_ids, int nof_units, ac_1plus1_protection_learning_mc_s *params) {

    int rv,  unit, i;
    int is_local, local_port;

    for (i = 0 ; i < nof_units ; i++) {
        unit = unit_ids[i];
        printf("unit[%d]......\n", unit);
        /* Initialize the test parameters */
        rv = ac_1plus1_protection_learning_mc_init(unit, params);
        if (rv != BCM_E_NONE) {
            printf("Error in ac_1plus1_protection_learning_mc_init, rv - %d\n", rv);
            return rv;
        }
    }

    for (i = 0 ; i < nof_units ; i++) {
        unit = unit_ids[i];

        /* Configure the Out-AC */
        rv = ac_1plus1_protection_learning_mc__set_non_protected_ac(unit, &g_ac_1plus1_protection_learning_mc.out_ac);
        if (rv != BCM_E_NONE) {
            printf("Error, failed to create an Out-AC, rv - %d\n", rv);
            return rv;
        }

        /* Configure the Non-Protected In-AC */
        rv = ac_1plus1_protection_learning_mc__set_non_protected_ac(unit, &g_ac_1plus1_protection_learning_mc.non_protected_in_ac);
        if (rv != BCM_E_NONE) {
            printf("Error, failed to create a non-protected In-AC, rv - %d\n", rv);
            return rv;
        }

        /* Configure the protection In-ACs */
        rv = ac_1plus1_protection_learning_mc__set_protected_in_acs(unit, &g_ac_1plus1_protection_learning_mc);
        if (rv != BCM_E_NONE) {
            printf("Error, failed to create the protected In-ACs, rv - %d\n", rv);
            return rv;
        }
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
int ac_1plus1_protection_learning_mc__with_ports__start_run(int *unit_ids, int nof_units, int in_port_primary, int in_port_secondary, int out_port, int non_protected_in_port) {
    int rv;

    ac_1plus1_protection_learning_mc_s param;

    ac_1plus1_protection_learning_mc__struct_get(&param);

    param.in_ac[FAILOVER_PRIMARY].port = in_port_primary;
    param.in_ac[FAILOVER_SECONDARY].port = in_port_secondary;
    param.out_ac.port = out_port;
    param.non_protected_in_ac.port = non_protected_in_port;

    return ac_1plus1_protection_learning_mc__start_run(unit_ids, nof_units, &param);
}


/* Performs failover switchover
 */

int ac_1plus1_protection_learning_mc__failover_set(int *unit_ids, int nof_units) {

    int rv, i, unit, enable;

    for (i = 0 ; i < nof_units ; i++) {

        unit = unit_ids[i];
        print unit;

        rv = bcm_failover_get(unit, g_ac_1plus1_protection_learning_mc.ingress_failover_id, &enable);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_failover_get gport \n");
            return rv;
        }

        rv = bcm_failover_set(unit, g_ac_1plus1_protection_learning_mc.ingress_failover_id,!enable);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_failover_set gport\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}

/* Performs failover mc setting for the two protection In-ACs to modify the learning object
 *
 * INPUT: failover_mc_group  - The MC group to set as the learning object
 */
int ac_1plus1_protection_learning_mc__failover_mc_set(int *unit_ids, int nof_units, int failover_mc_group) {

    int rv, i, unit;
    bcm_vlan_port_t in_ac;

    for (i = 0 ; i < nof_units ; i++) {
        unit = unit_ids[i];

        /* Retrieve the Secondary In-AC and Replace the failover_mc_group value */
        bcm_vlan_port_t_init(&in_ac);
        in_ac.vlan_port_id = g_ac_1plus1_protection_learning_mc.in_ac[FAILOVER_SECONDARY].vlan_port_id;
        rv = bcm_vlan_port_find(unit, &in_ac);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_find for Secondary In-AC gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
            return rv;
        }

        in_ac.failover_mc_group = failover_mc_group;
        in_ac.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
        rv = bcm_vlan_port_create(unit, &in_ac);
        if (rv != BCM_E_NONE) {
            printf("Error, setting failover_mc_group in bcm_vlan_port_create for gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
            return rv;
        }

        /* Retrieve the Primary In-AC and Replace the failover_mc_group value */
        bcm_vlan_port_t_init(&in_ac);
        in_ac.vlan_port_id = g_ac_1plus1_protection_learning_mc.in_ac[FAILOVER_PRIMARY].vlan_port_id;
        rv = bcm_vlan_port_find(unit, &in_ac);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_find for the Primary In-AC gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
            return rv;
        }

        in_ac.failover_mc_group = failover_mc_group;
        in_ac.flags |= BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
        rv = bcm_vlan_port_create(unit, &in_ac);
        if (rv != BCM_E_NONE) {
            printf("Error, setting failover_mc_group in bcm_vlan_port_create for gport 0x%08x, rv - %d\n", in_ac.vlan_port_id, rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}

int ac_1plus1_protection_learning_mc__cleanup(int *unit_ids, int nof_units) {

    int rv, i, unit;

    for (i = 0 ; i < nof_units ; i++) {

        unit = unit_ids[i];

        rv = bcm_vlan_port_destroy(unit, g_ac_1plus1_protection_learning_mc.in_ac[FAILOVER_SECONDARY].vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_ac_1plus1_protection_learning_mc.in_ac[FAILOVER_SECONDARY].vlan_port_id);
            return rv;
        }

        rv = bcm_vlan_port_destroy(unit, g_ac_1plus1_protection_learning_mc.in_ac[FAILOVER_PRIMARY].vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_ac_1plus1_protection_learning_mc.in_ac[FAILOVER_PRIMARY].vlan_port_id);
            return rv;
        }

        rv = bcm_vlan_port_destroy(unit, g_ac_1plus1_protection_learning_mc.non_protected_in_ac.vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_ac_1plus1_protection_learning_mc.non_protected_in_ac.vlan_port_id);
            return rv;
        }

        rv = bcm_vlan_port_destroy(unit, g_ac_1plus1_protection_learning_mc.out_ac.vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning_mc.out_ac.vlan_port_id);
            return rv;
        }

        rv = bcm_failover_destroy(unit, g_ac_1plus1_protection_learning_mc.ingress_failover_id);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
            return rv;
        }

        rv = bcm_multicast_destroy(unit, g_ac_1plus1_protection_learning_mc.failover_mc_group);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
            return rv;
        }

        rv = bcm_multicast_destroy(unit, g_ac_1plus1_protection_learning_mc.vsi);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_destroy gport: 0x%x\n", g_ac_1plus1_protection_learning.out_ac.vlan_port_id);
            return rv;
        }
    }

    return BCM_E_NONE;
}

