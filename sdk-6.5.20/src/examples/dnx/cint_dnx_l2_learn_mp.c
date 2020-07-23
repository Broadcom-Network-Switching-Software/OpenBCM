/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The CINT provides an example MP learning configuration.                                                                                          *
 *                                                                                                                                                  *
 * Describes basic steps of MP VLAN-Port configuration that enables learning for the participating ACs.                                             *
 * The packets are sent between an In-AC-1 and Out-AC, while an additional In-AC-2 participates in their vswitch.                                   *
 * In cases where no learning has occured for the destination AC, flooding is expected.                                                             *
 * The sequence will enable replacing of the In-LIF formats that in turn make use of various learn-info types.                                      *
 *                                                  ___________________________________                                                             *
 *                                                 |            ____________           |                                                            *
 *                                                 |           |            |          |                                                            *
 *                                 In-AC-1         |           |    MACT    |          |                                                            *
 *                                 --------------  |<--|       |____________|          |                                                            *
 *                                                 |   |                               |                                                            *
 *                                                     |                               |      Out-AC                                                *
 *                                                 |   |        ____________       |-->| ------------                                               *
 *                                                 |   |       |            |      |   |                                                            *
 *                                 In-AC-2         |   |-------|  Flooding  |------    |                                                            *
 *                                 --------------  |<--|       |  MC Group  |          |                                                            *
 *                                                 |           |____________|          |                                                            *
 *                                                 |___________________________________|                                                            *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/sand                                                                                                                 *
 * cint utility/cint_utils_multicast.c                                                                                                              *
 * cint cint_dnx_l2_learn_mp.c                                                                                                                      *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * dnx_l2_learn_mp_with_ports__start_run(int unit, int in_port_1, int in_port_2, int out_port)                                                      *
 *                                                                                                                                                  *
 * Sent and expected Packets:                                                                                                                       *
 *             +-----------------------------------------------------------+                                                                        *
 *        eth: |    DA     |     SA    | Outer-TPID | Outer-VLAN | PCP-DEI |                                                                        *
 *             |-----------------------------------------------------------|                                                                        *
 *             |    d_mac  |   s_mac   |   0x9100   |   5/6/7    |   any   |                                                                        *
 *             +-----------------------------------------------------------+                                                                        *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_dnx_l2_learn_mp', before calling the main                  *
 * function. In order to re-write only part of the values, call 'dnx_l2_learn_mp__struct_get(dnx_l2_learn_mp_s)'                                    *
 * function and re-write the values prior to calling the main function.                                                                             *
 *                                                                                                                                                  *
 * Test name: AT_Dnx_Cint_l2_learn_modify                                                                                                           *
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
struct dnx_l2_learn_mp_s {
    bcm_vlan_t vsi;
    ac_info_s in_ac_1;
    ac_info_s in_ac_2;
    ac_info_s out_ac;
};

/* Initialization of a global struct */
dnx_l2_learn_mp_s g_dnx_l2_learn_mp = {
     /* Additional parameters */
     3, /* VSI */   
     /*  AC configurations
    Phy Port    VLAN    gport   MAC Address    */
     { 200,      5,      0,     {0x00, 0x11, 0x00, 0x00, 0x00, 0x13} }, /* In-AC-1 */
     { 201,      6,      0,     {0x00, 0x33, 0x00, 0x00, 0x00, 0x33} }, /* In-AC-2 */
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
 *   params: new values for g_dnx_l2_learn_mp
 */
int dnx_l2_learn_mp_init(int unit, dnx_l2_learn_mp_s *params) {

    int rv;
    bcm_multicast_t mc_group;

    if (params != NULL) {
       sal_memcpy(&g_dnx_l2_learn_mp, params, sizeof(g_dnx_l2_learn_mp));
    }

    /* Create a vswitch */
    rv = bcm_vswitch_create_with_id(unit, g_dnx_l2_learn_mp.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, dnx_l2_learn_mp_init failed during bcm_vswitch_create for vsi %d, rv - %d\n", g_dnx_l2_learn_mp.vsi, rv);
        return rv;
    }

    /* Create a flooding MC Group */
    mc_group = g_dnx_l2_learn_mp.vsi;
    rv = multicast__open_mc_group(unit, &mc_group, 0);
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
dnx_l2_learn_mp__struct_get(dnx_l2_learn_mp_s *param)
{

    sal_memcpy(param, &g_dnx_l2_learn_mp, sizeof(g_dnx_l2_learn_mp));

    return;
}


/* Configuration of the vswitch participating AC
 *
 * INPUT: 
 *   ac_info_s: Configuration info for a single AC.
 */
int dnx_l2_learn_mp__create_ac(int unit, bcm_vlan_t vsi, ac_info_s *ac_param) {

    bcm_vlan_port_t vlan_port;
    bcm_multicast_t mc_group;
    int rv;

    /* Configure a standard MP VLAN-Port AC */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_param->port;
    vlan_port.match_vlan = ac_param->vlan;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, failed during bcm_vlan_port_create for port %d, vlan %d, rv - %d\n",
               proc_name, ac_param->port, ac_param->vlan, rv);
        return rv;
    }
    ac_param->gport = vlan_port.vlan_port_id;
    printf("Created MP AC on port %d, vlan %d, gport = %d\n", ac_param->port, ac_param->vlan, ac_param->gport);

    /* Attach the AC to the vswitch */
    rv = bcm_vswitch_port_add(unit, vsi, ac_param->gport);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vswitch_port_add of gport 0x%08x to vsi %d, rv - %d\n",
               proc_name, ac_param->gport, vsi, rv);
        return rv;
    }

    /* Attach to the flooding MC group */
    mc_group = vsi;
    rv = multicast__vlan_port_add(unit, mc_group, ac_param->port, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during multicast__vlan_port_add of gport 0x%08x to mc_group %d, rv - %d\n",
               proc_name, ac_param->port, mc_group, rv);
        return rv;
    }

    /* Enable VLAN-Membership filtering */
    rv = bcm_vlan_gport_add(unit, ac_param->vlan, ac_param->port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, adding bcm_vlan_gport_add for vlan %d, port %d, rv - %d\n",
               proc_name, ac_param->vlan, ac_param->port, rv);
        return rv;
    }

    /* Open up the port for outgoing and ingoing traffic */
    rv = bcm_port_control_set(unit, ac_param->port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_port_control_set for port %d, rv - %d\n",
               proc_name, ac_param->port, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Main function that performs all the configurations for the example.
 * Performs init configuration before configuring the ACs
 * INPUT: 
 *   params: Configuration info for running the example.
 */
int dnx_l2_learn_mp__start_run(int unit, dnx_l2_learn_mp_s *params) {

    int rv;

    /* Perform initializations */
    rv = dnx_l2_learn_mp_init(unit, params);
    if (rv != BCM_E_NONE) {
        printf("Error in dnx_l2_learn_mp_init, rv - %d\n", rv);
        return rv;
    }

    /* Configure the participating ACs */
    rv = dnx_l2_learn_mp__create_ac(unit, g_dnx_l2_learn_mp.vsi, &g_dnx_l2_learn_mp.in_ac_1);
    if (rv != BCM_E_NONE) {
        printf("Error in dnx_l2_learn_mp__create_ac for in_ac_1, rv - %d\n", rv);
        return rv;
    }
    rv = dnx_l2_learn_mp__create_ac(unit, g_dnx_l2_learn_mp.vsi, &g_dnx_l2_learn_mp.in_ac_2);
    if (rv != BCM_E_NONE) {
        printf("Error in dnx_l2_learn_mp__create_ac for in_ac_2, rv - %d\n", rv);
        return rv;
    }
    rv = dnx_l2_learn_mp__create_ac(unit, g_dnx_l2_learn_mp.vsi, &g_dnx_l2_learn_mp.out_ac);
    if (rv != BCM_E_NONE) {
        printf("Error in dnx_l2_learn_mp__create_ac for out_ac, rv - %d\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* This function runs the main test function with specified ports
 *  
 * INPUT: in_port_1 - Port for the In-AC-1 that sends packets to Out-AC
 *        in_port_2 - Port for the Additional AC in the vswitch
 *        out_port -  Port for Out-AC that sends packets to In-AC-1
 */
int dnx_l2_learn_mp_with_ports__start_run(int unit, int in_port_1, int in_port_2, int out_port) {

    dnx_l2_learn_mp_s param;

    dnx_l2_learn_mp__struct_get(&param);

    param.in_ac_1.port = in_port_1;
    param.in_ac_2.port = in_port_2;
    param.out_ac.port = out_port;

    return dnx_l2_learn_mp__start_run(unit, &param);
}


int dnx_l2_learn_mp__cleanup(int unit) {

    int rv;

    rv = bcm_vlan_port_destroy(unit, g_dnx_l2_learn_mp.in_ac_1.gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_dnx_l2_learn_mp.in_ac_1.gport);
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit, g_dnx_l2_learn_mp.in_ac_2.gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_dnx_l2_learn_mp.in_ac_2.gport);
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit, g_dnx_l2_learn_mp.out_ac.gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy gport: 0x%08x\n", g_dnx_l2_learn_mp.out_ac.gport);
        return rv;
    }

    return BCM_E_NONE;
}


/* This function modifies In-AC-1 with a different parameters that may affect the learning format 
 *  
 * INPUT: is_wide_data - Ingress Wide data flag
 */
int dnx_l2_learn_mp__modify_ac(int unit, int is_wide_data) {

    bcm_vlan_port_t vlan_port;
    int rv;

    /* Remove the AC from the vswitch */
    rv = bcm_vswitch_port_delete(unit, g_dnx_l2_learn_mp.vsi, g_dnx_l2_learn_mp.in_ac_1.gport);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vswitch_port_delete of gport 0x%08x from vsi %d, rv - %d\n",
               proc_name, g_dnx_l2_learn_mp.in_ac_1.gport, g_dnx_l2_learn_mp.vsi, rv);
        return rv;
    }

    /* Configure a standard MP VLAN-Port AC */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE;
    if (is_wide_data) {
        vlan_port.flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = g_dnx_l2_learn_mp.in_ac_1.port;
    vlan_port.match_vlan = g_dnx_l2_learn_mp.in_ac_1.vlan;
    vlan_port.vlan_port_id = g_dnx_l2_learn_mp.in_ac_1.gport;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, during bcm_vlan_port_create failed while modifying gport - 0x%08x, rv - %d\n",
               proc_name, ac_param->port, ac_param->vlan, rv);
        return rv;
    }

    /* Attach the AC to the vswitch */
    rv = bcm_vswitch_port_add(unit, g_dnx_l2_learn_mp.vsi, g_dnx_l2_learn_mp.in_ac_1.gport);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during bcm_vswitch_port_add of gport 0x%08x to vsi %d, rv - %d\n",
               proc_name, g_dnx_l2_learn_mp.in_ac_1.gport, g_dnx_l2_learn_mp.vsi, rv);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Verify kinds of the default destinations in VSI configured by bcm_vlan_control_valn_set
 * 5 VSIs are created. Packets that match these VSIs should be transmitted.
 */
int dnx_l2_vlan_control_default_dest_main(int unit, int port1, int port2, int port3, int port4)
{
    int rv;
    int nof_vsis = 5; /** 0: drop, 1: fec, 2: sys-port, 3: trap, 4: flow-id*/
    int vsis[nof_vsis] = {200, 202, 203, 204, 205};
    int gports[nof_vsis] = { 0 };
    int index;

    /*
     * 0: prepare a drop-destination.
     */
    gports[0] = BCM_GPORT_BLACK_HOLE;


    /*
     * 1: prepare a forward gport.
     */
    bcm_l3_egress_t l3_egr;
    bcm_if_t if_id;
    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.port = port2;
    l3_egr.vlan = vsis[2];

    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_egr, &if_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_egress_create\n");
        return rv;
    }

    BCM_GPORT_FORWARD_PORT_SET(gports[1], if_id);

    /*
     * 2: prepare a system gport.
     */
    BCM_GPORT_LOCAL_SET(gports[2], port2);
    rv = bcm_stk_gport_sysport_get(unit, gports[2], gports+2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_stk_gport_sysport_get\n");
        return rv;
    }

    /*
     * 3: prepare a trap gport.
     */
    int trap_id;
    bcm_rx_trap_config_t trap_config;

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_type_create\n");
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = BCM_GPORT_LOCAL_CPU;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_rx_trap_set\n");
        return rv;
    }
    BCM_GPORT_TRAP_SET(gports[3], trap_id, 4, 4);

    /*
     * 4: prepare a flow gport with an invalid flow id.
     */
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gports[4], 0x7FFFF);


    /*
     * create and confifure the VSIs.
     */
    bcm_vlan_control_vlan_t vlan_control;

    for (index = 0; index < nof_vsis; index++) {
        /** Create the VSI*/
        rv = bcm_vswitch_create_with_id(unit, vsis[index]);
        if (rv != BCM_E_NONE) {
            printf("Error %d, bcm_vswitch_create_with_id vsi = %d\n", rv, vsis[index]);
            return rv;
        }

        /** Add port to VSIs, port1 is used as the in-port*/
        rv = bcm_vlan_gport_add(unit, vsis[index], port1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error %d, bcm_vlan_gport_add vsi = %d\n", rv, vsis[index]);
            return rv;
        }
        
        /** Configure the VSIs */
        bcm_vlan_control_vlan_t_init(&vlan_control);
        rv = bcm_vlan_control_vlan_get(unit, vsis[index], &vlan_control);
        if (rv != BCM_E_NONE) {
            printf("Error %d, bcm_vlan_control_vlan_get vsi = %d\n", rv, vsis[index]);
            return rv;
        }

        vlan_control.flags2 |= BCM_VLAN_FLAGS2_UNKNOWN_DEST;
        vlan_control.unknown_unicast_group= 0;
        vlan_control.unknown_multicast_group = 0;
        vlan_control.broadcast_group = 0;
        vlan_control.unknown_dest = gports[index];

        rv = bcm_vlan_control_vlan_set(unit, vsis[index], vlan_control);
        if (rv != BCM_E_NONE) {
            printf("Error %d, bcm_vlan_control_vlan_set vsi = %d\n", rv, vsis[index]);
            return rv;
        }
    }

    return rv;
}



