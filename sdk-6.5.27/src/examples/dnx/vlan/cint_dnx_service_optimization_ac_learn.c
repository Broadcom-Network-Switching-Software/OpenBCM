/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The CINT provides an example for learning based on service optimization AC.                                                                      *
 *                                                                                                                                                  *
 * Describes basic steps of MP VLAN-Port configuration that enable learning for the participating ACs.                                              *
 * The setup consists of two service optimization ACs and one service AC, with various configurations that provide different result-types.          *
 * That in turn represent different learn Info. All the In-ACs are associated with the same VSI.                                                    *
 * If sending the packet matched with the service optimization In-AC, the vsi should be the value in in_lif table plus the pkt outer vlan           *
 * For service optimization In-AC, the learn enable is true by default.                                                                             *
 * In cases where no learning has occured for the destination AC, flooding is expected towards all the ACs.                                         *
 *                                                                                                                                                  *
 *                                                  ___________________________________                                                             *
 *                                                 |            ____________           |                                                            *
 *                                                 |           |            |          |                                                            *
 *               Service Optimization-AC-1         |           |    MACT    |          |                                                            *
 *                 ----------------------------    |<--|       |____________|          |                                                            *
 *                                                 |   |                               |                                                            *
 *               Service Optimization-AC-2         |   |                               |                                                            *
 *                 ----------------------------    |<--|                               |      Out-AC                                                *
 *                                                 |   |                           |-->| ------------                                               *
 *                                                 |   |        ____________       |   |                                                            *
 *                                                 |   |       |            |      |   |                                                            *
 *                         Service In-AC-N         |   |-------|  Flooding  |------    |                                                            *
 *                 ----------------------------    |<--|       |  MC Group  |          |                                                            *
 *                                                 |           |____________|          |                                                            *
 *                                                 |___________________________________|                                                            *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * Test Scenario - start
  cd ../../../../src/examples/dnx                                                                                                                  
  cint ../sand/utility/cint_utils_multicast.c                                                                                                      
  cint ../dnx/utility/cint_dnx_utils_vlan_port.c
  cint ../dnx/vlan/cint_dnx_service_optimization_ac_learn.c                                                                                             
  cint                                                                                                                                             
  int unit = 0;                                                                                                                                    
  dnx_service_optimization_ac_learn_with_ports_start_run(int unit, int port1, int port2, port3)                                                    
 * Test Scenario - end
 *                                                                                                                                                  *
 * Sent and expected Packets:                                                                                                                       *
 *             +-----------------------------------------------------------+                                                                        *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI |                                                                        *
 *             |-----------------------------------------------------------|                                                                        *
 *             |    d_mac  |   s_mac   |   0x9100   |    10      |   any   |                                                                        *
 *             +-----------------------------------------------------------+                                                                        *
 *                                                                                                                                                  *
 * Test name: AT_Dnx_Cint_vswitch_service_optimization_ac_learn                                                                                     *
 *                                                                                                                                                  *
 ************************************************************************************************************************************************** */


/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */
struct ac_vswitch_info_s {
    bcm_port_t port;
    bcm_vlan_t vlan;
    bcm_gport_t gport;
    bcm_mac_t mac_address;
    uint32 ac_type_flags;
};

/*  Main Struct  */
struct dnx_service_optimization_ac_learn_s {
    bcm_vlan_t vsi;
    ac_vswitch_info_s ac_vswitch_info[3];
};

/* Initialization of a global struct */
dnx_service_optimization_ac_learn_s g_dnx_service_optimization_ac_learn = {
    /* Additional parameters */
    4096,    /* Base-VSI for symmetric optimization AC */
    /*  Vswitch In-ACs & Out-AC configurations
    Port   VLAN gport  MAC Address                           ac_type_flags                             */
    {{200, 0,   0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x01}, vlan_port__ac_type_flag_set(ac_type_symmetric_optimized)}, /* Service-optimization-AC-1 */
     {201, 0,   0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x02}, vlan_port__ac_type_flag_set(ac_type_symmetric_optimized)}, /* Service-optimization-AC-2 */
     {202, 100, 0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x03}, 0}}, /* Service-AC */
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */

/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT:
 *   params: new values for g_dnx_service_optimization_ac_learn
 */
int dnx_service_optimization_ac_learn_init(int unit, dnx_service_optimization_ac_learn_s *params) {
    char error_msg[200]={0,};
    bcm_vlan_t fwd_domain = g_dnx_service_optimization_ac_learn.vsi + 10;
    bcm_multicast_t mc_group = 6500;
    bcm_vlan_control_vlan_t control_vlan;

    if (params != NULL) {
       sal_memcpy(&g_dnx_service_optimization_ac_learn, params, sizeof(g_dnx_service_optimization_ac_learn));
    }

    /* Create a vswitch for the In-ACs */
    snprintf(error_msg, sizeof(error_msg), "for fwd_domain %d", fwd_domain);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_create_with_id(unit, fwd_domain), error_msg);

    /* Create a flooding MC Group */
    BCM_IF_ERROR_RETURN_MSG(multicast__open_mc_group(unit, &mc_group, 0), "");

    /* Set vlan control, to flood */
    snprintf(error_msg, sizeof(error_msg), "fwd_domain 0x%08x", fwd_domain);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_get(unit, fwd_domain, &control_vlan), error_msg);

    control_vlan.unknown_unicast_group = control_vlan.unknown_multicast_group = control_vlan.broadcast_group = mc_group;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_control_vlan_set(unit, fwd_domain, control_vlan), error_msg);

    /* Set the match_vlan for service AC */
    g_dnx_service_optimization_ac_learn.ac_vswitch_info[2].vlan = DEFAULT_VLAN;

    return BCM_E_NONE;
}

/*
 * Return g_dnx_service_optimization_ac_learn information
 */
void dnx_service_optimization_ac_learn_struct_get(dnx_service_optimization_ac_learn_s *params) {
    sal_memcpy(params, &g_dnx_service_optimization_ac_learn, sizeof(g_dnx_service_optimization_ac_learn));
    return;
}

/* Configuration of a Vswitch AC
 *
 * INPUT:
 *   vsi - The vsi to associate with the AC
 *   ac_vswitch_info_s: Configuration info for a single vswitch AC.
 */
int dnx_service_optimization_ac_learn_create_ac(int unit, bcm_vlan_t vsi, ac_vswitch_info_s *ac_params) {
    bcm_vlan_port_t vlan_port;
    bcm_vlan_t fwd_domain = g_dnx_service_optimization_ac_learn.vsi + 10;
    bcm_multicast_t mc_group = 6500;
    char error_msg[200]={0,};

    snprintf(error_msg, sizeof(error_msg), "for ac_type_flags - %d", ac_params->ac_type_flags);
    BCM_IF_ERROR_RETURN_MSG(vlan_port__create(unit, ac_params->port, ac_params->vlan, vsi, 0, ac_params->ac_type_flags, NULL, &vlan_port), error_msg);
    ac_params->gport = vlan_port.vlan_port_id;

    snprintf(error_msg, sizeof(error_msg), "add of gport 0x%08x to mc_group %d", ac_params->port, mc_group);
    BCM_IF_ERROR_RETURN_MSG(multicast__gport_encap_add(unit, mc_group, ac_params->port, ac_params->gport, 0), error_msg);

    /* For service AC, using vswitch API to add it to vswitch*/
    if (ac_params->ac_type_flags == 0) {
        snprintf(error_msg, sizeof(error_msg), "add of gport 0x%08x to fwd_domain %d", ac_params->gport, fwd_domain);
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, fwd_domain, ac_params->gport), error_msg);
    }

    /* Set EVE action for each out_lif */
    ++g_vlan_translate_with_eve.eve_action_id;
    g_vlan_translate_with_eve.outer_action = bcmVlanActionMappedAdd;
    g_vlan_translate_with_eve.outer_tpid = 0x88a8;
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_eve_translation_set(unit,ac_params->gport,0), "");

    return BCM_E_NONE;
}

/* Main function that performs all the configurations for the example.
 * Performs init configuration before configuring the ACs
 * INPUT:
 *   params: Configuration info for running the example.
 */
int dnx_service_optimization_ac_learn_start_run(int unit, dnx_service_optimization_ac_learn_s *params) {
    int in_ac_idx;
    char error_msg[200]={0,};

    /* Perform initializations */
    BCM_IF_ERROR_RETURN_MSG(dnx_service_optimization_ac_learn_init(unit, params), "");

    /* Configure the In-ACs that participate in the vswitch */
    for (in_ac_idx = 0; in_ac_idx < 3; in_ac_idx++)
    {
        snprintf(error_msg, sizeof(error_msg), "for In-AC-%d", in_ac_idx + 1);
        BCM_IF_ERROR_RETURN_MSG(dnx_service_optimization_ac_learn_create_ac(unit, params->vsi, &(params->ac_vswitch_info[in_ac_idx])), error_msg);
        g_dnx_service_optimization_ac_learn.ac_vswitch_info[in_ac_idx].gport = params->ac_vswitch_info[in_ac_idx].gport;
        printf("In-AC-%d created. gport - %d\n", in_ac_idx + 1, params->ac_vswitch_info[in_ac_idx].gport);
    }

    return BCM_E_NONE;
}

/* This function runs the main test function with specified ports
 * INPUT: port1  -  Port for the Service-optimization-AC-1 that share the same vswitch
 *        port2  -  Port for the Service-optimization-AC-2 that share the same vswitch
 *        port3  -  Port for the Service-AC that share the same vswitch
 */
int dnx_service_optimization_ac_learn_with_ports_start_run(int unit, int port1, int port2, int port3) {
    dnx_service_optimization_ac_learn_s params;
    dnx_service_optimization_ac_learn_struct_get(&params);

    /* Set the given ports to the AC info structures */
    params.ac_vswitch_info[0].port = port1;
    params.ac_vswitch_info[1].port = port2;
    params.ac_vswitch_info[2].port = port3;

    return dnx_service_optimization_ac_learn_start_run(unit, &params);
}

/* The function performs any required cleanup.
 */
int dnx_service_optimization_ac_learn_cleanup(int unit) {
    int in_ac_idx;
    char error_msg[200]={0,};
    bcm_vlan_t fwd_domain = g_dnx_service_optimization_ac_learn.vsi + 10;

    /* Delete all the In-ACs and the Out-AC that is last in the array */
    for (in_ac_idx = 0; in_ac_idx < 3; in_ac_idx++)
    {
        snprintf(error_msg, sizeof(error_msg), "for AC-%d", in_ac_idx);
        BCM_IF_ERROR_RETURN_MSG(vlan_port__delete(unit, g_dnx_service_optimization_ac_learn.ac_vswitch_info[in_ac_idx].ac_type_flags,
                               NULL, g_dnx_service_optimization_ac_learn.ac_vswitch_info[in_ac_idx].gport), error_msg);
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_destroy(unit, fwd_domain), "");

    return BCM_E_NONE;
}
