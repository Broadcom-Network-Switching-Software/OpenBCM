/* $Id: cint_dnx_l2_learn_asymmetric.c
 *
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
 * Test Scenario - start
  cd ../../../../src/examples/dnx                                                                                                                  
  cint utility/cint_dnx_utils_multicast.c                                                                                                          
  cint cint_dnx_l2_learn_asymmetric.c                                                                                                              
  cint                                                                                                                                             
  int unit = 0;                                                                                                                                    
  dnx_l2_learn_asymmetric_with_ports__start_run(int unit, int in_port_opt, int in_port_vlan_translation, int out_port)                             
 * Test Scenario - end
 *                                                                                                                                                  *
 * Sent and expected Packets:                                                                                                                       *
 *             +-----------------------------------------------------------+                                                                        *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI |                                                                        *
 *             |-----------------------------------------------------------|                                                                        *
 *             |    d_mac  |   s_mac   |   0x9100   |   3/6/7    |   any   |                                                                        *
 *             +-----------------------------------------------------------+                                                                        *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_dnx_l2_learn_asymmetric', before calling the main          *
 * function. In order to re-write only part of the values, call 'dnx_l2_learn_asymmetric__struct_get(dnx_l2_learn_asymmetric_s)'                    *
 * function and re-write the values prior to calling the main function.                                                                             *
 *
 */




/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */
struct ac_info_s {
    bcm_port_t port;
    bcm_vlan_t vlan;
    bcm_gport_t gport;
    bcm_mac_t mac_address;
};

/*  Main Struct  */
struct dnx_l2_learn_asymmetric_s {
    bcm_vlan_t vsi;
    ac_info_s in_ac_opt;
    ac_info_s in_ac_translation;
    ac_info_s out_ac;
};

/* Initialization of a global struct */
dnx_l2_learn_asymmetric_s g_dnx_l2_learn_asymmetric = {
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
 **************************************************************************************************** */

/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT: 
 *   params: new values for g_dnx_l2_learn_asymmetric
 */
int dnx_l2_learn_asymmetric_init(int unit, dnx_l2_learn_asymmetric_s *params) {

    char error_msg[200]={0,};
    bcm_multicast_t mc_group;

    if (params != NULL) {
       sal_memcpy(&g_dnx_l2_learn_asymmetric, params, sizeof(g_dnx_l2_learn_asymmetric));
    }

    /* Create a vswitch */
    snprintf(error_msg, sizeof(error_msg), "vsi %d", g_dnx_l2_learn_asymmetric.vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_create_with_id(unit, g_dnx_l2_learn_asymmetric.vsi), error_msg);

    /* Create a flooding MC Group */
    mc_group = g_dnx_l2_learn_asymmetric.vsi;
    BCM_IF_ERROR_RETURN_MSG(multicast__open_mc_group(unit, &mc_group, 0), "");

    return BCM_E_NONE;
}


/*
 * Return g_dnx_l2_learn_asymmetric information
 */
void
dnx_l2_learn_asymmetric__struct_get(dnx_l2_learn_asymmetric_s *param)
{

    sal_memcpy(param, &g_dnx_l2_learn_asymmetric, sizeof(g_dnx_l2_learn_asymmetric));

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
int dnx_l2_learn_asymmetric__create_acs(int unit, dnx_l2_learn_asymmetric_s *param) {

    bcm_vlan_port_t vlan_port;
    bcm_multicast_t mc_group;
    char error_msg[200]={0,};

    /* Configure an Ingress-Only Optimization AC */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_VSI_BASE_VID;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.port = param->in_ac_opt.port;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "Ingress-Only Optimization AC");
    param->in_ac_opt.gport = vlan_port.vlan_port_id;
    printf("Created Optimization AC on port %d, gport = %d\n", param->in_ac_opt.port, param->in_ac_opt.gport);

    snprintf(error_msg, sizeof(error_msg), "vlan %d, port %d", param->vsi, param->in_ac_opt.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, param->in_ac_opt.vlan, param->in_ac_opt.port, 0), error_msg);

    /* Attach to the flooding MC group */
    mc_group = param->vsi;
    snprintf(error_msg, sizeof(error_msg), "gport 0x%08x to mc_group %d", param->in_ac_opt.port, mc_group);
    BCM_IF_ERROR_RETURN_MSG(multicast__gport_encap_add(unit, mc_group, param->in_ac_opt.port, 0, 0), error_msg);

    /* Open up the port for outgoing and ingoing traffic */
    snprintf(error_msg, sizeof(error_msg), "port %d", param->in_ac_opt.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, param->in_ac_opt.port, bcmPortControlBridge, 1), error_msg);

    /* Configure an Ingress-Only VLAN-Translation AC */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = param->in_ac_translation.port;
    vlan_port.match_vlan = param->in_ac_translation.vlan;
    vlan_port.vsi = param->vsi;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "Ingress-Only VLAN-Translation AC");
    param->in_ac_translation.gport = vlan_port.vlan_port_id;
    printf("Created Translation AC on port %d, gport = %d\n", param->in_ac_translation.port, param->in_ac_translation.gport);

    /* Open up the port for outgoing and ingoing traffic */
    snprintf(error_msg, sizeof(error_msg), "port %d", param->in_ac_translation.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, param->in_ac_translation.port, bcmPortControlBridge, 1), error_msg);

    snprintf(error_msg, sizeof(error_msg), "vlan %d, port %d", param->vsi, param->in_ac_translation.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, param->in_ac_translation.vlan, param->in_ac_translation.port, 0), error_msg);

    /* Attach to the flooding MC group */
    snprintf(error_msg, sizeof(error_msg), "port 0x%08x to mc_group %d", param->in_ac_translation.port, mc_group);
    BCM_IF_ERROR_RETURN_MSG(multicast__gport_encap_add(unit, mc_group, param->in_ac_translation.port, 0, 0), error_msg);

    /* Configure a symmetric Out-AC */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = param->out_ac.port;
    vlan_port.match_vlan = param->out_ac.vlan;
    vlan_port.vsi = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "Ingress-Only VLAN-Translation AC");
    param->out_ac.gport = vlan_port.vlan_port_id;
    printf("Created Out AC on port %d, gport = %d\n", param->out_ac.port, param->out_ac.gport);

    /* Attach the Out-AC to the vswitch */
    snprintf(error_msg, sizeof(error_msg), "gport 0x%08x to vsi %d", param->out_ac.gport, param->vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, param->vsi, param->out_ac.gport), error_msg);

    /* Open up the port for outgoing and ingoing traffic */
    snprintf(error_msg, sizeof(error_msg), "port %d", param->out_ac.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, param->out_ac.port, bcmPortControlBridge, 1), error_msg);

    snprintf(error_msg, sizeof(error_msg), "vlan %d, port %d", param->vsi, param->out_ac.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, param->out_ac.vlan, param->out_ac.port, 0), error_msg);

    /* Attach to the flooding MC group */
    snprintf(error_msg, sizeof(error_msg), "port 0x%08x to mc_group %d", param->out_ac.port, mc_group);
    BCM_IF_ERROR_RETURN_MSG(multicast__gport_encap_add(unit, mc_group, param->out_ac.port, param->out_ac.gport, 0), error_msg);

    return BCM_E_NONE;
}


/* Main function that performs all the configurations for the example.
 * Performs init configuration before configuring the ACs
 * INPUT: 
 *   params: Configuration info for running the example.
 */
int dnx_l2_learn_asymmetric__start_run(int unit, dnx_l2_learn_asymmetric_s *params) {


    /* Perform initializations */
    BCM_IF_ERROR_RETURN_MSG(dnx_l2_learn_asymmetric_init(unit, params), "");

    /* Configure the participating ACs */
    BCM_IF_ERROR_RETURN_MSG(dnx_l2_learn_asymmetric__create_acs(unit, &g_dnx_l2_learn_asymmetric), "");

    return BCM_E_NONE;
}


/* This function runs the main test function with specified ports
 *  
 * INPUT: in_port_opt - Port for Optimization In-AC 
 *        in_port_vlan_translation - Port for VLAN Translation In-AC 
 *        out_port - Port for symmetric AC
 */
int dnx_l2_learn_asymmetric_with_ports__start_run(int unit, int in_port_opt, int in_port_vlan_translation, int out_port) {

    dnx_l2_learn_asymmetric_s param;

    dnx_l2_learn_asymmetric__struct_get(&param);

    param.in_ac_opt.port = in_port_opt;
    param.in_ac_translation.port = in_port_vlan_translation;
    param.out_ac.port = out_port;

    return dnx_l2_learn_asymmetric__start_run(unit, &param);
}


int dnx_l2_learn_asymmetric_with_ports__cleanup(int unit) {

    char error_msg[200]={0,};

    snprintf(error_msg, sizeof(error_msg), "gport: 0x%08x",  g_dnx_l2_learn_asymmetric.in_ac_opt.gport);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, g_dnx_l2_learn_asymmetric.in_ac_opt.gport), error_msg);

    snprintf(error_msg, sizeof(error_msg), "gport: 0x%08x",  g_dnx_l2_learn_asymmetric.in_ac_translation.gport);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, g_dnx_l2_learn_asymmetric.in_ac_translation.gport), error_msg);

    snprintf(error_msg, sizeof(error_msg), "gport: 0x%08x",  g_dnx_l2_learn_asymmetric.out_ac.gport);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, g_dnx_l2_learn_asymmetric.out_ac.gport), error_msg);

    return BCM_E_NONE;
}


