/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The CINT provides an example for different modifying an AC from P2P to MP, while retaining the original learn information.                       *
 *                                                                                                                                                  *
 * Describes basic steps of MP VLAN-Port configuration that enable learning for the participating ACs.                                              *
 * The setup consists of several In-ACs, with various configurations that provide different result-types, that in turn represent different learn    *
 * Info. All the In-ACs are associated with the same vswitch, while an Out-AC provides a P2P destination.                                           * 
 * In cases where no learning has occured for the destination AC, flooding is expected towards all the In-ACs.                                      *
 * The CINT also verifies that the an MP that transitioned from P2P retains all the configuration that are responsible for the original MP          *
 * result type and the learn info.                                                                                                                  *
 *                                                                                                                                                  * 
 *                                                  ___________________________________                                                             *
 *                                                 |            ____________           |                                                            *
 *                                                 |           |            |          |                                                            *
 *                                 In-AC-1         |           |    MACT    |          |                                                            *
 *                                 --------------  |<--|       |____________|          |                                                            *
 *                                                 |   |                               |                                                            *
 *                                 In-AC-2         |   |                               |                                                            *
 *                                 --------------  |<--|                               |      Out-AC                                                * 
 *                                                 |   |                           |-->| ------------                                               * 
 *                                                 |   |        ____________       |                                                                * 
 *                                                 |   |       |            |      |   |                                                            *
 *                                 In-AC-N         |   |-------|  Flooding  |------    |                                                            *
 *                                 --------------  |<--|       |  MC Group  |          |                                                            *
 *                                                 |           |____________|          |                                                            *
 *                                                 |___________________________________|                                                            *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/dnx                                                                                                                  *
 * cint ../sand/utility/cint_utils_multicast.c                                                                                                      *
 * cint ../sand/utility/cint_sand_utils_vlan_port.c                                                                                                 *
 * cint cint_dnx_ac_learn_p2p_to_mp.c                                                                                                               *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * dnx_l2_learn_ac_with_ports__start_run(int unit, int in_port, int out_port)                                                                       *
 *                                                                                                                                                  *
 * Sent and expected Packets:                                                                                                                       *
 *             +-----------------------------------------------------------+                                                                        *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI |                                                                        *
 *             |-----------------------------------------------------------|                                                                        *
 *             |    d_mac  |   s_mac   |   0x9100   |    100+    |   any   |                                                                        *
 *             +-----------------------------------------------------------+                                                                        *
 *                                                                                                                                                  *                                                                            *
 *                                                                                                                                                  *
 * Test name: AT_Dnx_Cint_ac_learn_p2p_to_mp                                                                                                        *
 */
  
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

bcm_vlan_t IN_ACS_VSWITCH=30;

struct ac_vswitch_info_s {
    bcm_port_t port;
    bcm_vlan_t vlan;
    bcm_gport_t gport;
    bcm_mac_t mac_address;
    uint32 ac_type_flags;
};

/*  Main Struct  */
struct dnx_ac_learn_p2p_to_mp_s {
    bcm_vlan_t vsi;
    ac_vswitch_info_s ac_vswitch_info[5];
};


/* Initialization of a global struct */
dnx_ac_learn_p2p_to_mp_s g_dnx_ac_learn_p2p_to_mp = {
     /* Additional parameters */
     IN_ACS_VSWITCH,    /* VSI for the In-ACs vswitch */
    /*  Vswitch In-ACs & Out-AC configurations
    Phy Port    VLAN    gport       MAC Address                          ac_type_flags                             */
    {{ 200,      100,       0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x01}, 0},                            /* In-AC-1 */
     { 200,      101,       0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x02}, AC_TYPE_FEC_PROTECTION},       /* In-AC-2 */
     { 200,      102,       0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x03}, AC_TYPE_INGRESS_PROTECTION},   /* In-AC-3 */
     { 200,      103,       0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x04}, AC_TYPE_WIDE_DATA},            /* In-AC-4 */
     { 201,      99,        0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x00}, 0}},                           /* Out-AC  */
};

int NOF_IN_ACS=sizeof(g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info)/sizeof(g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[0]) - 1;


/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT: 
 *   params: new values for g_dnx_ac_learn_p2p_to_mp
 */
int dnx_ac_learn_p2p_to_mp_init(int unit, dnx_ac_learn_p2p_to_mp_s *params) {

    int in_ac_idx, rv;
    bcm_multicast_t mc_group;

    if (params != NULL) {
       sal_memcpy(&g_dnx_ac_learn_p2p_to_mp, params, sizeof(g_dnx_ac_learn_p2p_to_mp));
    }

    /* Create a vswitch for the In-ACs */
    rv = bcm_vswitch_create_with_id(unit, params->vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, dnx_ac_learn_p2p_to_mp_init failed during bcm_vswitch_create for vsi %d, rv - %d\n", params->vsi, rv);
        return rv;
    }

    /* Create a flooding MC Group */
    mc_group = params->vsi;
    rv = multicast__open_mc_group(unit, &mc_group, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group failed, rv - %d\n");
        return rv;
    }

    /* Set VID values for each AC, based on the DEFAULT_VLAN value */
    for (in_ac_idx = 0; in_ac_idx < NOF_IN_ACS; in_ac_idx++)
    {
        g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[in_ac_idx].vlan = DEFAULT_VLAN + in_ac_idx;
    }
    g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[NOF_IN_ACS].vlan = DEFAULT_VLAN - 1;

    return BCM_E_NONE;
}


/*
 * Return g_dnx_ac_learn_p2p_to_mp information
 */
void
dnx_ac_learn_p2p_to_mp__struct_get(dnx_ac_learn_p2p_to_mp_s *params)
{
    sal_memcpy(params, &g_dnx_ac_learn_p2p_to_mp, sizeof(g_dnx_ac_learn_p2p_to_mp));

    return;
}


/* Configuration of a Vswitch AC
 *
 * INPUT:
 *   vsi - The vsi to associate with the AC
 *   ac_vswitch_info_s: Configuration info for a single vswitch AC.
 */
int dnx_ac_learn_p2p_to_mp__create_ac(int unit, bcm_vlan_t vsi, ac_vswitch_info_s *ac_params) {

    bcm_vlan_port_t vlan_port;
    bcm_multicast_t mc_group;
    bcm_vlan_t vlan_port_vsi = (vsi == IN_ACS_VSWITCH) ? 0 : vsi;
    int rv;

    rv = vlan_port__create(unit, ac_params->port, ac_params->vlan, vlan_port_vsi, 0, ac_params->ac_type_flags, NULL, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in vlan_port__create for ac_type_flags - %d, rv - %d\n",
               proc_name, ac_params->ac_type_flags, rv);
        return rv;
    }
    ac_params->gport = vlan_port.vlan_port_id;

    /* Enable VLAN-Membership filtering */
    rv = bcm_vlan_gport_add(unit, ac_params->vlan, ac_params->port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, adding bcm_vlan_gport_add for vlan %d, port %d, rv - %d\n",
               proc_name, ac_params->vlan, ac_params->port, rv);
        return rv;
    }

    /* Open up the port for outgoing and ingoing traffic */
    rv = bcm_port_control_set(unit, ac_params->port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_port_control_set for port %d, rv - %d\n",
               proc_name, ac_params->port, rv);
        return rv;
    }

    /* In case it's an In-AC with the Vswitch VSI - Add it to the Vswitch and the flooding group. */
    if (vsi == IN_ACS_VSWITCH)
    {
        /* Attach the AC to a vswitch */
        rv = bcm_vswitch_port_add(unit, vsi, ac_params->gport);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error during bcm_vswitch_port_add of gport 0x%08x to vsi %d, rv - %d\n",
                   proc_name, ac_params->gport, vsi, rv);
            return rv;
        }

        /* Attach to a flooding MC group */
        mc_group = vsi;
        rv = multicast__gport_encap_add(unit, mc_group, ac_params->port, ac_params->gport, 0);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error during multicast__vlan_port_add of gport 0x%08x to mc_group %d, rv - %d\n",
                   proc_name, ac_params->port, mc_group, rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}


/* Main function that performs all the configurations for the example.
 * Performs init configuration before configuring the ACs
 * INPUT: 
 *   params: Configuration info for running the example.
 */
int dnx_ac_learn_p2p_to_mp__start_run(int unit, dnx_ac_learn_p2p_to_mp_s *params) {

    int in_ac_idx, rv;

    /* Perform initializations */
    rv = dnx_ac_learn_p2p_to_mp_init(unit, params);
    if (rv != BCM_E_NONE) {
        printf("Error in dnx_ac_learn_p2p_to_mp_init, rv - %d\n", rv);
        return rv;
    }

    /* Configure the In-ACs that participate in the vswitch */
    for (in_ac_idx = 0; in_ac_idx < NOF_IN_ACS; in_ac_idx++)
    {
        rv = dnx_ac_learn_p2p_to_mp__create_ac(unit, params->vsi, &(params->ac_vswitch_info[in_ac_idx]));
        if (rv != BCM_E_NONE) {
            printf("%s(): Error during dnx_ac_learn_p2p_to_mp__create_ac for In-AC-%d, rv - %d\n",
                   proc_name, in_ac_idx, rv);
            return rv;
        }
        g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[in_ac_idx].gport = params->ac_vswitch_info[in_ac_idx].gport;
        printf("In-AC-%d created. gport - %d\n", in_ac_idx, params->ac_vswitch_info[in_ac_idx].gport);
    }

    /* Configure the Out-AC that doesn't participate in the vswitch */
    rv = dnx_ac_learn_p2p_to_mp__create_ac(unit, params->vsi + 1, &(params->ac_vswitch_info[NOF_IN_ACS]));
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during dnx_ac_learn_p2p_to_mp__create_ac for Out-AC, rv - %d\n",
               proc_name, rv);
        return rv;
    }
    g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[NOF_IN_ACS].gport = params->ac_vswitch_info[NOF_IN_ACS].gport;
    printf("Out-AC created. gport - %d\n", params->ac_vswitch_info[NOF_IN_ACS].gport);

    return BCM_E_NONE;
}


/* This function runs the main test function with specified ports
 *  
 * INPUT: in_port - Port for the In-ACs that share the same vswitch
 *        out_port -  Port for Out-AC that isn't associated with the In-ACs vswitch
 */
int dnx_ac_learn_p2p_to_mp_with_ports__start_run(int unit, int in_port, int out_port) {

    int in_ac_idx;
    dnx_ac_learn_p2p_to_mp_s params;

    dnx_ac_learn_p2p_to_mp__struct_get(&params);

    /* Set the given ports to the AC info structures */
    for (in_ac_idx = 0; in_ac_idx < NOF_IN_ACS; in_ac_idx++)
    {
        params.ac_vswitch_info[in_ac_idx].port = in_port;
    }
    params.ac_vswitch_info[NOF_IN_ACS].port = out_port;

    return dnx_ac_learn_p2p_to_mp__start_run(unit, &params);
}


/* The function modifies a specified indexed In-AC to be a P2P In-LIF destined to the Out-AC
 *  
 * INPUT: in_ac_idx - The index of the In-AC in the CINT's In-ACs array
 */
int dnx_ac_learn_p2p_to_mp__replace_in_ac_to_p2p(int unit, int in_ac_idx) {

    bcm_vswitch_cross_connect_t vswitch_cross_connect;
    int rv;

    /* Set a P2P connection from an In-AC to the Out-AC */
    bcm_vswitch_cross_connect_t_init(&vswitch_cross_connect);
    vswitch_cross_connect.port1 = g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[in_ac_idx].gport;
    vswitch_cross_connect.port2 = g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[NOF_IN_ACS].gport;
    vswitch_cross_connect.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

    rv = bcm_vswitch_cross_connect_add(unit, &vswitch_cross_connect);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vswitch_cross_connect_add from In-AC-%d to Out-AC, rv - %d\n",
               proc_name, in_ac_idx, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* The function modifies a specified indexed In-AC to be a MP In-LIF by removing an existing
 * P2P cross-connect to the Out-AC. 
 * This step should be complemented by either an attachment to a vswitch for Servive ACs 
 * or setting the VSI using bcm_vlan_port_create Replace for Non-Service ACs.
 *  
 * INPUT: in_ac_idx - The index of the In-AC in the CINT's In-ACs array
 */
int dnx_ac_learn_p2p_to_mp__replace_in_ac_to_mp(int unit, int in_ac_idx) {

    bcm_vswitch_cross_connect_t vswitch_cross_connect;
    uint32 ac_type_flags;
    int rv;

    /* Set a P2P connection from an In-AC to the Out-AC */
    bcm_vswitch_cross_connect_t_init(&vswitch_cross_connect);
    vswitch_cross_connect.port1 = g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[in_ac_idx].gport;
    vswitch_cross_connect.port2 = g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[NOF_IN_ACS].gport;
    vswitch_cross_connect.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;

    rv = bcm_vswitch_cross_connect_delete(unit, &vswitch_cross_connect);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vswitch_cross_connect_delete from In-AC-%d to Out-AC, rv - %d\n",
               proc_name, in_ac_idx, rv);
        return rv;
    }

    /* Validate that the newly selected MP In-LIF has the information of the original MP LIF */
    rv = vlan_port__ac_type_get(unit, g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[in_ac_idx].gport, &ac_type_flags);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during vlan_port__ac_type_get for In-AC-%d, rv - %d\n",
               proc_name, in_ac_idx, rv);
        return rv;
    }

    if (ac_type_flags != g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[in_ac_idx].ac_type_flags)
    {
        printf("Error: Comparison failed for In-AC-%d replace to MP, previous ac_type - %d, current ac_type - %d\n",
               in_ac_idx, g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[in_ac_idx].ac_type_flags, ac_type_flags);
        return BCM_E_FAIL;
    }



    return BCM_E_NONE;
}


/* The function attaches a MP In-AC to a vswitch, following a replace from P2P to MP.
 *  
 * INPUT: in_ac_idx - The index of the In-AC in the CINT's In-ACs array
 */
int dnx_ac_learn_p2p_to_mp__in_ac_vswitch_add(int unit, int in_ac_idx) {

    int rv;

    /* Attach the LIF to its previous vswitch */
    rv = bcm_vswitch_port_add(unit, g_dnx_ac_learn_p2p_to_mp.vsi, g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[in_ac_idx].gport);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vswitch_port_add for In-AC-%d, rv - %d\n",
               proc_name, in_ac_idx, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* The function performs any required cleanup.
 */
int dnx_ac_learn_p2p_to_mp__cleanup(int unit) {

    int in_ac_idx, rv;

    /* Delete all the In-ACs and the Out-AC that is last in the array */
    for (in_ac_idx = 0; in_ac_idx <= NOF_IN_ACS; in_ac_idx++)
    {
        rv = bcm_vlan_port_destroy(unit, g_dnx_ac_learn_p2p_to_mp.ac_vswitch_info[in_ac_idx].gport);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error during bcm_vlan_port_destroy for AC-%d, rv - %d\n",
                   proc_name, ac_idx, rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}



