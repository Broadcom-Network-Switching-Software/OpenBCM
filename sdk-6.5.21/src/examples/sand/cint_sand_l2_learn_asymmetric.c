/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The CINT provides an example for Ingress only learning                                                                                           *
 *                                                                                                                                                  *
 * Describes basic steps of asymmetric Ingress-only ACs configuration that enables port learning for those ACs.                                     *
 * One type of Ingress-only AC is an Optimized In-AC, while the second is a VLAN-Translation.                                                       *
 * The packets are sent between a third out-ac and one of the mentioned Ingress-only ACs. In cases where no learning has occured for                *
 * the destination AC, flooding is expected.                                                                                                        *
 *                                                                                                                                                  *
 *                                                  ___________________________________                                                             *
 *                                                 |            ____________           |                                                            *
 *                                                 |           |            |          |                                                            *
 *                           Optimized In-AC       |           |    MACT    |          |                                                            *
 *                           --------------------  |<--|       |____________|          |                                                            *
 *                                                 |   |                               |                                                            *
 *                                                     |                               |      Out-AC                                                *
 *                                                 |   |        ____________           | ------------                                               *
 *                                                 |   |       |            |          |                                                            *
 *                           Translation In-AC     |   |-------|  Flooding  |          |                                                            *
 *                           --------------------  |<--|       |  MC Group  |          |                                                            *
 *                                                 |           |____________|          |                                                            *
 *                                                 |___________________________________|                                                            *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/sand                                                                                                                 *
 * cint utility/cint_utils_multicast.c                                                                                                              *
 * cint cint_sand_l2_learn_asymmetric.c                                                                                                             *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * sand_l2_learn_asymmetric_with_ports__start_run(int unit, int in_port_opt, int in_port_vlan_translation, int out_port)                            *
 *                                                                                                                                                  *
 * Sent and expected Packets:                                                                                                                       *
 *             +-----------------------------------------------------------+                                                                        *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI |                                                                        *
 *             |-----------------------------------------------------------|                                                                        *
 *             |    d_mac  |   s_mac   |   0x9100   |   3/6/7    |   any   |                                                                        *
 *             +-----------------------------------------------------------+                                                                        *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_sand_l2_learn_asymmetric', before calling the main         *
 * function. In order to re-write only part of the values, call 'sand_l2_learn_asymmetric__struct_get(sand_l2_learn_asymmetric_s)'                  *
 * function and re-write the values prior to calling the main function.                                                                             *
 *                                                                                                                                                  *
 * Test name: AT_Sand_Cint_l2_learn_asymmetric                                                                                                      *
 */
  
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
struct ac_info_s {
    bcm_port_t port;
    bcm_vlan_t vlan;
    bcm_gport_t gport;
    bcm_mac_t mac_address;
};

/*  Main Struct  */
struct sand_l2_learn_asymmetric_s {
    bcm_vlan_t vsi;
    ac_info_s in_ac_opt;
    ac_info_s in_ac_translation;
    ac_info_s out_ac;
};

/* Initialization of a global struct */
sand_l2_learn_asymmetric_s g_sand_l2_learn_asymmetric = {
     /* Additional parameters */
     3, /* VSI */   
     /*  AC configurations
    Phy Port    VLAN    gport   MAC Address    */
     { 200,      3,      0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x13} }, /* In-AC Optimization */
     { 201,      6,      0,     {0x00, 0x33, 0x00, 0x00, 0x00, 0x33} }, /* In-AC translation */
     { 202,      7,      0,     {0x00, 0x55, 0x00, 0x00, 0x00, 0x66} }, /* Out-AC */
};


/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT: 
 *   params: new values for g_sand_l2_learn_asymmetric
 */
int sand_l2_learn_asymmetric_init(int unit, sand_l2_learn_asymmetric_s *params) {

    int rv;
    bcm_multicast_t mc_group;

    if (params != NULL) {
       sal_memcpy(&g_sand_l2_learn_asymmetric, params, sizeof(g_sand_l2_learn_asymmetric));
    }

    /* Create a vswitch */
    rv = bcm_vswitch_create_with_id(unit, g_sand_l2_learn_asymmetric.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, sand_l2_learn_asymmetric_init failed during bcm_vswitch_create for vsi %d, rv - %d\n", g_sand_l2_learn_asymmetric.vsi, rv);
        return rv;
    }

    /* Create a flooding MC Group */
    mc_group = g_sand_l2_learn_asymmetric.vsi;
    rv = multicast__open_mc_group(unit, &mc_group, (is_device_or_above(unit, JERICHO2)?0:BCM_MULTICAST_TYPE_L2));
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, rv - %d\n");
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * Return g_sand_l2_learn_asymmetric information
 */
void
sand_l2_learn_asymmetric__struct_get(sand_l2_learn_asymmetric_s *param)
{

    sal_memcpy(param, &g_sand_l2_learn_asymmetric, sizeof(g_sand_l2_learn_asymmetric));

    return;
}


/* Configuration of the participating ACs:
 * In-AC1 - Ingress-Only Optimization AC
 * In-AC2 - Ingress-Only VLAN-Translation AC
 * Out-AC - Symmetric AC
 *
 * INPUT: 
 *   protection_ac_s: Configuration info for a single non-protected AC.
 */
int sand_l2_learn_asymmetric__create_acs(int unit, sand_l2_learn_asymmetric_s *param) {

    bcm_vlan_port_t vlan_port;
    bcm_multicast_t mc_group;
    int rv;

    /* Configure an Ingress-Only Optimization AC */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_VSI_BASE_VID;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.port = param->in_ac_opt.port;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error failed during bcm_vlan_port_create of an Ingress-Only Optimization AC, rv - %d\n", proc_name, rv);
        return rv;
    }
    param->in_ac_opt.gport = vlan_port.vlan_port_id;
    printf("Created Optimization AC on port %d, gport = %d\n", param->in_ac_opt.port, param->in_ac_opt.gport);

    rv = bcm_vlan_gport_add(unit, param->in_ac_opt.vlan, param->in_ac_opt.port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, adding bcm_vlan_gport_add for vlan %d, port %d, rv - %d\n", proc_name, param->vsi,
               param->in_ac_opt.port, rv);
        return rv;
    }

    /* Attach to the flooding MC group */
    mc_group = param->vsi;
    rv = multicast__vlan_port_add(unit, mc_group, param->in_ac_opt.port, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during multicast__vlan_port_add of gport 0x%08x to mc_group %d, rv - %d\n", proc_name, param->in_ac_opt.port, mc_group, rv);
        return rv;
    }

    /* Open up the port for outgoing and ingoing traffic */
    rv = bcm_port_control_set(unit, param->in_ac_opt.port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_port_control_set for port %d, rv - %d\n", proc_name,
               param->in_ac_opt.port, rv);
        return rv;
    }

    /* Configure an Ingress-Only VLAN-Translation AC */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = param->in_ac_translation.port;
    vlan_port.match_vlan = param->in_ac_translation.vlan;
    vlan_port.vsi = param->vsi;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error failed during bcm_vlan_port_create of an Ingress-Only VLAN-Translation AC, rv - %d\n", proc_name, rv);
        return rv;
    }
    param->in_ac_translation.gport = vlan_port.vlan_port_id;
    printf("Created Translation AC on port %d, gport = %d\n", param->in_ac_translation.port, param->in_ac_translation.gport);

    /* Open up the port for outgoing and ingoing traffic */
    rv = bcm_port_control_set(unit, param->in_ac_translation.port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_port_control_set for port %d, rv - %d\n", proc_name,
               param->in_ac_translation.port, rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, param->in_ac_translation.vlan, param->in_ac_translation.port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, adding bcm_vlan_gport_add for vlan %d, port %d, rv - %d\n", proc_name, param->vsi,
               param->in_ac_translation.port, rv);
        return rv;
    }

    /* Attach to the flooding MC group */
    rv = multicast__vlan_port_add(unit, mc_group, param->in_ac_translation.port, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during multicast__vlan_port_add of port 0x%08x to mc_group %d, rv - %d\n", proc_name, param->in_ac_translation.port, mc_group, rv);
        return rv;
    }

    /* Configure a symmetric Out-AC */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = param->out_ac.port;
    vlan_port.match_vlan = param->out_ac.vlan;
    vlan_port.vsi = 0;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error failed during bcm_vlan_port_create of an Ingress-Only VLAN-Translation AC, rv - %d\n", proc_name, rv);
        return rv;
    }
    param->out_ac.gport = vlan_port.vlan_port_id;
    printf("Created Out AC on port %d, gport = %d\n", param->out_ac.port, param->out_ac.gport);

    /* Attach the Out-AC to the vswitch */
    rv = bcm_vswitch_port_add(unit, param->vsi, param->out_ac.gport);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vswitch_port_add of gport 0x%08x to vsi %d, rv - %d\n", proc_name,
               param->out_ac.gport, param->vsi, rv);
        return rv;
    }

    /* Open up the port for outgoing and ingoing traffic */
    rv = bcm_port_control_set(unit, param->out_ac.port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_port_control_set for port %d, rv - %d\n", proc_name,
               param->out_ac.port, rv);
        return rv;
    }

    rv = bcm_vlan_gport_add(unit, param->out_ac.vlan, param->out_ac.port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, adding bcm_vlan_gport_add for vlan %d, port %d, rv - %d\n", proc_name, param->vsi,
               param->out_ac.port, rv);
        return rv;
    }

    /* Attach to the flooding MC group */
    rv = multicast__vlan_port_add(unit, mc_group, param->out_ac.port, param->out_ac.gport, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during multicast__vlan_port_add of port 0x%08x to mc_group %d, rv - %d\n", proc_name, param->out_ac.port, mc_group, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Main function that performs all the configurations for the example.
 * Performs init configuration before configuring the ACs
 * INPUT: 
 *   params: Configuration info for running the example.
 */
int sand_l2_learn_asymmetric__start_run(int unit, sand_l2_learn_asymmetric_s *params) {

    int rv;

    /* Perform initializations */
    rv = sand_l2_learn_asymmetric_init(unit, params);
    if (rv != BCM_E_NONE) {
        printf("Error in sand_l2_learn_asymmetric_init, rv - %d\n", rv);
        return rv;
    }

    /* Configure the participating ACs */
    rv = sand_l2_learn_asymmetric__create_acs(unit, &g_sand_l2_learn_asymmetric);
    if (rv != BCM_E_NONE) {
        printf("Error in sand_l2_learn_asymmetric_init, rv - %d\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* This function runs the main test function with specified ports
 *  
 * INPUT: in_port_opt - Port for Optimization In-AC 
 *        in_port_vlan_translation - Port for VLAN Translation In-AC 
 *        out_port - Port for symmetric AC
 */
int sand_l2_learn_asymmetric_with_ports__start_run(int unit, int in_port_opt, int in_port_vlan_translation, int out_port) {

    sand_l2_learn_asymmetric_s param;

    sand_l2_learn_asymmetric__struct_get(&param);

    param.in_ac_opt.port = in_port_opt;
    param.in_ac_translation.port = in_port_vlan_translation;
    param.out_ac.port = out_port;

    return sand_l2_learn_asymmetric__start_run(unit, &param);
}


int sand_l2_learn_asymmetric_with_ports__cleanup(int unit) {

    int rv;

    rv = bcm_vlan_port_destroy(unit, g_sand_l2_learn_asymmetric.in_ac_opt.gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_sand_l2_learn_asymmetric.in_ac_opt.gport);
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit, g_sand_l2_learn_asymmetric.in_ac_translation.gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_sand_l2_learn_asymmetric.in_ac_translation.gport);
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit, g_sand_l2_learn_asymmetric.out_ac.gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_sand_l2_learn_asymmetric.out_ac.gport);
        return rv;
    }

    return BCM_E_NONE;
}


