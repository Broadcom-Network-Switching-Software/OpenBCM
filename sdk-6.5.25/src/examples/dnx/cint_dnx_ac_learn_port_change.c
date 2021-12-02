/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The CINT provides an example for VLAN-Port learn of a physical port that differs from the packet's src-sys-port.                                 *
 *                                                                                                                                                  *
 * Describes basic steps of MP VLAN-Port configuration that enable learning to a different destination.                                             *
 * The setup consists of a Main-ACs, that is configured to learn the destination of another physical port, where Learnt-AC is defined.              *
 * In addition to the Main-AC and Learnt-AC, an Out-AC is defined on a third port and packets are sent and received from this port to above ACs.    *
 *                                                                                                                                                  *
 * All the ACs are associated with the same vswitch.                                                                                                * 
 * In cases where no learning has occured for the destination AC, flooding is expected towards all the rest of the ACs.                             *
 *                                                                                                                                                  * 
 *                                                  ___________________________________                                                             *
 *                                                 |            ____________           |                                                            *
 *                                                 |           |            |          |                                                            *
 *                                 Main-AC         |           |    MACT    |          |                                                            *
 *                                 --------------  |<--|       |____________|          |                                                            *
 *                                                 |   |                               |                                                            *
 *                                 Learnt-AC       |   |                               |                                                            *
 *                                 --------------  |<--|                               |      Out-AC                                                * 
 *                                                 |   |                           |-->| ------------                                               * 
 *                                                 |   |        ____________       |                                                                * 
 *                                                 |   |       |            |      |   |                                                            *
 *                                                 |   |-------|  Flooding  |------    |                                                            *
 *                                                 |           |  MC Group  |          |                                                            *
 *                                                 |           |____________|          |                                                            *
 *                                                 |___________________________________|                                                            *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/dnx                                                                                                                  *
 * cint ../sand/utility/cint_utils_multicast.c                                                                                                      *
 * cint ../sand/utility/cint_sand_utils_vlan_port.c                                                                                                 *
 * cint cint_dnx_ac_learn_port_change.c                                                                                                             *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * ddnx_ac_learn_port_change_with_ports__start_run(int unit, int in_port, int out_port)                                                             *
 *                                                                                                                                                  *
 * Sent and expected Packets:                                                                                                                       *
 *             +-----------------------------------------------------------+                                                                        *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI |                                                                        *
 *             |-----------------------------------------------------------|                                                                        *
 *             |    d_mac  |   s_mac   |   0x9100   |    100     |   any   |                                                                        *
 *             +-----------------------------------------------------------+                                                                        *
 *                                                                                                                                                  *                                                                            *
 *                                                                                                                                                  *
 * Test name: AT_Dnx_Cint_ac_learn_port_change                                                                                                      *
 *                                                                                                                                                  *
 ************************************************************************************************************************************************** */
  
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */

bcm_vlan_t IN_ACS_VSWITCH=50;

struct ac_vswitch_info_s {
    bcm_port_t port;
    bcm_vlan_t vlan;
    bcm_gport_t gport;
    bcm_mac_t mac_address;
    uint32 ac_type_flags;
};

/*  Main Struct  */
struct dnx_ac_learn_port_change_s {
    bcm_vlan_t vsi;
    ac_vswitch_info_s ac_vswitch_info[3];
};


/* Initialization of a global struct */
dnx_ac_learn_port_change_s g_dnx_ac_learn_port_change = {
     /* Additional parameters */
     IN_ACS_VSWITCH,    /* VSI for the In-ACs vswitch */
    /*  Vswitch In-ACs & Out-AC configurations
    Phy Port    VLAN    gport   MAC Address                          ac_type_flags */
    {{ 200,     100,    0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x01}, vlan_port__ac_type_flag_set(ac_type_port_learn_change)},   /* Main-AC */
     { 201,     100,    0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x02}, 0},        /* Learnt-AC */
     { 202,     100,    0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x00}, 0}},       /* Out-AC  */
};

int NOF_ACS=sizeof(g_dnx_ac_learn_port_change.ac_vswitch_info)/sizeof(g_dnx_ac_learn_port_change.ac_vswitch_info[0]);

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 **************************************************************************************************** */

/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation.
 *
 * INPUT: 
 *   params: new values for g_dnx_ac_learn_port_change
 */
int dnx_ac_learn_port_change_init(int unit, dnx_ac_learn_port_change_s *params) {

    int rv;
    bcm_multicast_t mc_group;
    char *proc_name = "dnx_ac_learn_port_change__start_run";

    if (params != NULL) {
       sal_memcpy(&g_dnx_ac_learn_port_change, params, sizeof(g_dnx_ac_learn_port_change));
    }

    /* Create a vswitch for the In-ACs */
    rv = bcm_vswitch_create_with_id(unit, params->vsi);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, dnx_ac_learn_port_change_init failed during bcm_vswitch_create for vsi %d, rv - %d\n", proc_name, params->vsi, rv);
        return rv;
    }

    /* Create a flooding MC Group */
    mc_group = params->vsi;
    rv = multicast__open_mc_group(unit, &mc_group, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, multicast__open_mc_group failed, rv - %d\n", proc_name, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * Return g_dnx_ac_learn_port_change information
 */
void
dnx_ac_learn_port_change__struct_get(dnx_ac_learn_port_change_s *params)
{
    sal_memcpy(params, &g_dnx_ac_learn_port_change, sizeof(g_dnx_ac_learn_port_change));

    return;
}


/* Configuration of a Vswitch AC
 *
 * INPUT:
 *   vsi - The vsi to associate with the AC
 *   ac_vswitch_info_s: Configuration info for a single vswitch AC.
 */
int dnx_ac_learn_port_change__create_ac(int unit, ac_vswitch_info_s *ac_params, int port_init, bcm_gport_t learn_gport) {

    bcm_vlan_port_t vlan_port;
    bcm_multicast_t mc_group;
    ac_type_data_port_learn_change data_port_learn_change, *p_data_port_learn_change;
    ac_types_data_s ac_types_data;
    uint32 ret_ac_type_flags;
    bcm_vlan_t vsi = g_dnx_ac_learn_port_change.vsi;
    char *proc_name = "dnx_ac_learn_port_change__create_ac";
    int rv;

    sal_memset(&ac_types_data, 0, sizeof(ac_types_data));

    if (ac_params->ac_type_flags & vlan_port__ac_type_flag_set(ac_type_port_learn_change)) {
        data_port_learn_change.learnt_port = learn_gport;
        p_data_port_learn_change = &data_port_learn_change;
        sal_memcpy(&(ac_types_data.type_data[ac_type_port_learn_change]), &p_data_port_learn_change,
                   sizeof(ac_types_data.type_data[ac_type_port_learn_change]));
        ac_types_data.is_alloc_additional_objects[ac_type_port_learn_change] = FALSE;
    }

    rv = vlan_port__create(unit, ac_params->port, ac_params->vlan, vsi, 0, ac_params->ac_type_flags, &ac_types_data, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in vlan_port__create for ac_type_flags - %d, rv - %d\n",
               proc_name, ac_params->ac_type_flags, rv);
        return rv;
    }

    ac_params->gport = vlan_port.vlan_port_id;

    if (port_init) {
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
    }

    /* Attach to a flooding MC group */
    mc_group = vsi;
    printf("%s(): Before multicast__gport_encap_add with mc_group %d, port %d for gport 0x%08x\n", proc_name, mc_group, ac_params->port, ac_params->gport);

    rv = multicast__gport_encap_add(unit, mc_group, ac_params->port, ac_params->gport, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during multicast__vlan_port_add of gport 0x%08x to mc_group %d, rv - %d\n",
               proc_name, ac_params->gport, mc_group, rv);
        return rv;
    }

    /* Attach the AC to a vswitch */
    rv = bcm_vswitch_port_add(unit, vsi, ac_params->gport);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vswitch_port_add of gport 0x%08x to vsi %d, rv - %d\n",
               proc_name, ac_params->gport, vsi, rv);
        return rv;
    }

    /* Validate that the retrieved In-LIF has the information of the configured LIF.
       Avoid validation when the learn_port is the same as src-sys-port as failover_port_id==0 in this case */
    if ((learn_gport != ac_params->port) || (learn_gport == 0)) {
  
        rv = vlan_port__ac_type_get(unit, ac_params->gport, &ret_ac_type_flags);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error during vlan_port__ac_type_get for gport 0x%08x, rv - %d\n",
                   proc_name, ac_params->gport, rv);
            return rv;
        }

        if (ac_params->ac_type_flags != ret_ac_type_flags)
        {
            printf("%s(): Error in Comparison with configured of gport 0x%08x, configured ac_type flags - %d, retrieved ac_type flags - %d\n",
                   proc_name, ac_params->gport, ac_params->ac_type_flags, ret_ac_type_flags);
            return BCM_E_FAIL;
        }
    }

    return BCM_E_NONE;
}


/* Main function that performs all the configurations for the example.
 * Performs init configuration before configuring the ACs
 * INPUT: 
 *   params: Configuration info for running the example.
 */
int dnx_ac_learn_port_change__start_run(int unit, dnx_ac_learn_port_change_s *params) {

    int ac_idx, rv;
    bcm_gport_t learn_gport;
    char *proc_name = "dnx_ac_learn_port_change__start_run";

    /* Perform initializations */
    rv = dnx_ac_learn_port_change_init(unit, params);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in dnx_ac_learn_port_change_init, rv - %d\n", proc_name, rv);
        return rv;
    }

    /* Configure the ACs that participate in the vswitch */
    for (ac_idx = 0; ac_idx < NOF_ACS; ac_idx++)
    {
        learn_gport = (ac_idx == 0) ? params->ac_vswitch_info[1].port : 0;

        rv = dnx_ac_learn_port_change__create_ac(unit, &(params->ac_vswitch_info[ac_idx]), 1, learn_gport);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error during dnx_ac_learn_port_change__create_ac for AC-%d, rv - %d\n",
                   proc_name, ac_idx + 1, rv);
            return rv;
        }
        g_dnx_ac_learn_port_change.ac_vswitch_info[ac_idx].gport = params->ac_vswitch_info[ac_idx].gport;
        printf("%s(): AC-%d created. gport - %d\n", proc_name, ac_idx + 1, params->ac_vswitch_info[ac_idx].gport);

    }

    return BCM_E_NONE;
}


/* This function runs the main test function with specified ports
 *  
 * INPUT: main_port - Port for the Main-AC that has learning-Info dest of learn-info port 
 *        learnt_port - Port for the learning-Info dest of the Main-AC
 *        out_port -  Port for Out-AC to which the learning packets are sent
 */
int dnx_ac_learn_port_change_with_ports__start_run(int unit, int main_port, int learnt_port, int out_port) {

    dnx_ac_learn_port_change_s params;

    dnx_ac_learn_port_change__struct_get(&params);

    /* Set the given ports to the AC info structures */
    params.ac_vswitch_info[0].port = main_port;
    params.ac_vswitch_info[1].port = learnt_port;
    params.ac_vswitch_info[2].port = out_port;

    return dnx_ac_learn_port_change__start_run(unit, &params);
}


/* The function modifies a specified indexed AC to learn a physical port
 *  
 * INPUT: ac_idx - The index of the AC in the CINT's ACs array 
 *        learn_gport - The gport of the pysical port to learn
 */
int dnx_ac_learn_port_change__modify_learn_port(int unit, int ac_idx, bcm_gport_t learn_gport) {

    char *proc_name = "dnx_ac_learn_port_change__modify_learn_port";
    ac_vswitch_info_s *ac_params = &g_dnx_ac_learn_port_change.ac_vswitch_info[ac_idx];
    bcm_multicast_t mc_group = g_dnx_ac_learn_port_change.vsi;
    int rv;

    /* Detach from a flooding MC group */
    rv = multicast__gport_encap_delete(unit, mc_group, ac_params->port, ac_params->gport, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during multicast__gport_encap_delete of gport 0x%08x from mc_group %d, rv - %d\n",
               proc_name, ac_params->gport, mc_group, rv);
        return rv;
    }

    /* Delete the previos configuration */
    rv = vlan_port__delete(unit, ac_params->ac_type_flags, NULL, ac_params->gport);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during vlan_port__delete for AC-%d, rv - %d\n",
               proc_name, ac_idx + 1, rv);
        return rv;
    }

    ac_params->ac_type_flags = (learn_gport) ? vlan_port__ac_type_flag_set(ac_type_port_learn_change) : 0;
    rv = dnx_ac_learn_port_change__create_ac(unit, ac_params, 0, learn_gport);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during dnx_ac_learn_port_change__create_ac for AC-%d, rv - %d\n",
               proc_name, ac_idx + 1, rv);
        return rv;
    }

    return BCM_E_NONE;
}

/* 
 * The function performs any required cleanup.
 */
int dnx_ac_learn_port_change__cleanup(int unit) {

    char *proc_name = "dnx_ac_learn_port_change__cleanup";
    ac_vswitch_info_s *ac_params;
    bcm_multicast_t mc_group;
    int ac_idx, rv;

    /* Delete all the ACs */
    for (ac_idx = 0; ac_idx < NOF_ACS; ac_idx++)
    {
        ac_params = &g_dnx_ac_learn_port_change.ac_vswitch_info[ac_idx];

        /* Detach from a flooding MC group */
        mc_group = g_dnx_ac_learn_port_change.vsi;
        rv = multicast__gport_encap_delete(unit, mc_group, ac_params->port, ac_params->gport, 0);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error during multicast__gport_encap_delete of gport 0x%08x from mc_group %d, rv - %d\n",
                   proc_name, ac_params->gport, mc_group, rv);
            return rv;
        }

        rv = vlan_port__delete(unit, ac_params->ac_type_flags, NULL, ac_params->gport);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error during vlan_port__delete for AC-%d, rv - %d\n",
                   proc_name, ac_idx + 1, rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}



