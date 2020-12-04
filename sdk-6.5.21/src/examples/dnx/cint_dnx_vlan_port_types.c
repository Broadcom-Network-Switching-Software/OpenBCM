/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 *                                                                                                                                                  *
 * The CINT provides an example for different AC type configurations.                                                                               * 
 * It can provide a base for many types of for various tests that can utilize specific VLAN-Port configuration variants or traverse a large         * 
 * range of combinations, for example: Traverse all configuration combinations and verify that the learning works correctly or the smallest         * 
 * result type gets selected.                                                                                                                       * 
 *                                                                                                                                                  *
 *                                                  ___________________________________                                                             *
 *                                                 |                                   |                                                            *
 *                              Conf-Match --------|---         ____________           |                                                            *
 *                                                 |  |        |            |          |                                                            *
 *                                 Conf-AC         |  \/       |    MACT    |          |                                                            *
 *                                 --------------  |<--|       |____________|          |                                                            *
 *                                                 |   |                               |                                                            *
 *                                                 |   |                               |                                                            *
 *                                                 |   |                               |                                                            *
 *                   Ingress-Protection AC         |   |                               |                                                            *
 *                                 --------------  |<--|                               |      Out-AC                                                * 
 *                                 --------------  |<--|                           |-->| ------------                                               * 
 *                       FEC-Protection AC         |   |        ____________       |   |                                                            * 
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
 * cint cint_dnx_vlan_port_types.c                                                                                                                  *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * dnx_vlan_port_types_with_ports__start_run(int unit, int in_port, int out_port, int protection_port)                                              *
 *                                                                                                                                                  *                                                                                                                              *
 * Test name: AT_Dnx_Vlan_port_types                                                                                                                *
 */
  
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     ----------------
 */

/* Enum for the ACs that are supported by this test */
enum ac_e {
    ac_conf = 0,
    ac_out = 1,
    ac_ing_protection = 2,
    ac_fec_protection = 3,

    ac_last = 4
};

bcm_vlan_t IN_ACS_VSWITCH=30;
int NOF_ACS=ac_last;

struct ac_vswitch_info_s {
    bcm_port_t port;
    bcm_vlan_t vlan;
    bcm_gport_t gport;
    bcm_mac_t mac_address;
    uint32 ac_type_flags;
};

/*  Main Struct  */
struct dnx_vlan_port_types_s {
    bcm_vlan_t vsi;
    bcm_gport_t gport;
    bcm_if_t primary_fec_id;
    bcm_failover_t fec_failover_id;
    ac_vswitch_info_s ac_vswitch_info[NOF_ACS];
};


dnx_vlan_port_types_s g_dnx_vlan_port_types = {
     /* Additional parameters */
     IN_ACS_VSWITCH,                /* VSI for the In-ACs vswitch */
     0,                             /* gport */
     0,                             /* Primary FEC-ID */
     DEFAULT_FEC_FAILOVER_ID,       /* FEC Failover ID */
    /*  Vswitch In-ACs & Out-AC configurations
    Phy Port    VLAN            gport   MAC Address                           ac_type_flags                             */
    {{ 0,       DEFAULT_VLAN,   0,      {0x00, 0x11, 0x00, 0x00, 0x00, 0x01}, 0},                                                         /* ac_conf */
     { 0,       DEFAULT_VLAN,   0,      {0x00, 0x11, 0x00, 0x00, 0x00, 0x02}, 0},                                                         /* ac_out */
     { 0,       20,             0,      {0x00, 0x11, 0x00, 0x00, 0x00, 0x03}, vlan_port__ac_type_flag_set(ac_type_ingress_protection)},   /* ac_ing_protection */
     { 0,       30,             0,      {0x00, 0x11, 0x00, 0x00, 0x00, 0x04}, vlan_port__ac_type_flag_set(ac_type_fec_protection)}},      /* ac_fec_protection */
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of the main struct
 * Function allows the re-write of default values, SOC Property validation and
 * other general operation such as VSI creation, multicast group creation for flooding and
 * port initialization to bridging for all participating ports.
 *
 * INPUT: 
 *   params: new values for g_dnx_vlan_port_types
 */
int dnx_vlan_port_types_init(int unit, dnx_vlan_port_types_s *params) {

    int ac_idx, rv;
    bcm_multicast_t mc_group;
    char *proc_name = "dnx_vlan_port_types_init";

    if (params != NULL) {
       sal_memcpy(&g_dnx_vlan_port_types, params, sizeof(g_dnx_vlan_port_types));
    }

    /* Create a vswitch for the In-ACs */
    rv = bcm_vswitch_create_with_id(unit, params->vsi);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, dnx_vlan_port_types_init failed during bcm_vswitch_create for vsi %d, rv - %d\n", proc_name, params->vsi, rv);
        return rv;
    }

    /* Create a flooding MC Group */
    mc_group = params->vsi;
    rv = multicast__open_mc_group(unit, &mc_group, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, multicast__open_mc_group failed, rv - %d\n", proc_name, rv);
        return rv;
    }

    /* Enable the relevant ports for bridging */
    for (ac_idx = 0; ac_idx < NOF_ACS; ac_idx++)
    {
        rv = dnx_vlan_port_types__port_bridge_init(unit, params->ac_vswitch_info[ac_idx].port, params->ac_vswitch_info[ac_idx].vlan);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, dnx_vlan_port_types__port_bridge_init failed for port %d, vid - %d, rv - %d\n",
                   proc_name, port, vid, rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Return g_dnx_vlan_port_types information
 */
void
dnx_vlan_port_types__struct_get(dnx_vlan_port_types_s *params)
{
    sal_memcpy(params, &g_dnx_vlan_port_types, sizeof(g_dnx_vlan_port_types));

    return;
}


/* Configuration of a Vswitch AC
 * The function uses the utility function vlan_port__create, and requests additional
 * resource allocations via the specific structs per AC-Type.
 * The function also validates that the created AC has a result-type that matches all
 * the requested AC-Types.
 *
 * INPUT:
 *   ac_port - The phy port enumeration on which to create the AC
 *   vsi - The vsi to associate with the AC
 *   ac_type_flags - The requested attributes for the created VLAN-Port.
 */
int dnx_vlan_port_types__create_ac(int unit, ac_e ac_port, bcm_vlan_t vsi, uint32 ac_type_flags) {
    bcm_vlan_port_t vlan_port;
    bcm_multicast_t mc_group;
    bcm_vlan_t vlan_port_vsi = (vsi == IN_ACS_VSWITCH) ? 0 : vsi;
    bcm_vlan_t vid = g_dnx_vlan_port_types.ac_vswitch_info[ac_port].vlan;
    ac_types_data_s ac_types_data;
    ac_type_data_ingress_protection data_ingress_protection, *p_data_ingress_protection;
    ac_type_data_fec_protection data_fec_protection, *p_data_fec_protection;
    bcm_port_t port = g_dnx_vlan_port_types.ac_vswitch_info[ac_port].port, gport;
    uint32 ret_ac_type_flags;
    int rv;
    char *proc_name = "dnx_vlan_port_types__create_ac";

    sal_memset(&ac_types_data, 0, sizeof(ac_types_data));

    /* Fill specific data per ac_type that is required for the configuration
       The use of a pointer is required in order to achieve a pointer to a void struct in CINT */
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_ingress_protection)) {
        data_ingress_protection.is_primary = (g_dnx_vlan_port_types.ac_vswitch_info[ac_conf].port == port) ? TRUE : FALSE;
        data_ingress_protection.ingress_failover_id = DEFAULT_INGRESS_FAILOVER_ID;
        p_data_ingress_protection = &data_ingress_protection;
        sal_memcpy(&(ac_types_data.type_data[ac_type_ingress_protection]), &p_data_ingress_protection,
                   sizeof(ac_types_data.type_data[ac_type_ingress_protection]));
        ac_types_data.is_alloc_additional_objects[ac_type_ingress_protection] = FALSE;
    }
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_fec_protection)) {
        data_fec_protection.is_primary = (g_dnx_vlan_port_types.ac_vswitch_info[ac_conf].port == port) ? TRUE : FALSE;
        data_fec_protection.fec_failover_id = DEFAULT_FEC_FAILOVER_ID;
        data_fec_protection.fec_id = (data_fec_protection.is_primary) ? g_dnx_vlan_port_types.primary_fec_id : 0;
        p_data_fec_protection = &data_fec_protection;
        sal_memcpy(&(ac_types_data.type_data[ac_type_fec_protection]), &p_data_fec_protection,
                   sizeof(ac_types_data.type_data[ac_type_fec_protection]));
        ac_types_data.is_alloc_additional_objects[ac_type_fec_protection] = (data_fec_protection.is_primary) ? FALSE : TRUE;
    }

    /* Perform the VLAN-Port creation and store the returned gport */
    rv = vlan_port__create(unit, port, vid, vlan_port_vsi, 0, ac_type_flags, &ac_types_data, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in vlan_port__create for ac_type_flags - 0x%08x, rv - %d\n",
               proc_name, ac_type_flags, rv);
        return rv;
    }
    gport = vlan_port.vlan_port_id;
    g_dnx_vlan_port_types.ac_vswitch_info[ac_port].gport = gport;

    /* Return any updated info on the specific AC type structure */
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_fec_protection) && ac_types_data.is_alloc_additional_objects[ac_type_fec_protection]) {
        g_dnx_vlan_port_types.primary_fec_id = p_data_fec_protection->fec_id;
        g_dnx_vlan_port_types.fec_failover_id = p_data_fec_protection->fec_failover_id;
    }

    /* In case it's an AC with the Vswitch VSI - Add it to the Vswitch and the flooding group. */
    if (vsi == IN_ACS_VSWITCH)
    {
        /* Attach the AC to a vswitch */
        rv = bcm_vswitch_port_add(unit, vsi, gport);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in bcm_vswitch_port_add of gport 0x%08x to vsi %d, rv - %d\n",
                   proc_name, gport, vsi, rv);
            return rv;
        }

        /* Attach to a flooding MC group */
        mc_group = vsi;
        rv = multicast__gport_encap_add(unit, mc_group, port, gport, 0);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error in multicast__gport_encap_add of gport 0x%08x to mc_group %d, rv - %d\n",
                   proc_name, port, mc_group, rv);
            return rv;
        }
    }

    /* Validate that the selected MP In-LIF has all the attributes that were requested */
    rv = vlan_port__ac_type_get(unit, gport, &ret_ac_type_flags);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during vlan_port__ac_type_get for gport 0x%08x, rv - %d\n",
               proc_name, gport, rv);
        return rv;
    }
    if (ac_type_flags != ret_ac_type_flags)
    {
        printf("%s(): Comparison failed for gport 0x%08x, expected ac_type - 0x%08x, received ac_type - 0x%08x\n",
               proc_name, gport, ac_type_flags, ret_ac_type_flags);
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}


/* Configuration of a Vswitch AC of dnx_vlan_port_types__create_ac for calls
 * from the TCL layer.
 *
 * INPUT:
 *   ac_port - The phy port enumeration on which to create the AC
 *   ac_type_flags - The requested attributes for the created VLAN-Port.
 */
int dnx_vlan_port_types__configure_ac(int unit, ac_e ac_port, uint32 ac_type_flags) {

    char *proc_name = "dnx_vlan_port_types__configure_ac";
    int rv;

    rv = dnx_vlan_port_types__create_ac(unit, ac_port, IN_ACS_VSWITCH, ac_type_flags);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during dnx_vlan_port_types__create_ac for ac_type_flags - %d, rv - %d\n",
               proc_name, ac_type_flags, rv);
        return rv;
    }

    return BCM_E_NONE;
}

/* Deletion of a Vswitch AC, including any allocated additional resources as defined
 * by the AC-Type specific structures.
 *
 * INPUT:
 *   ac_port - The phy port enumeration on which to create the AC
 *   ac_type_flags - The requested attributes for the created VLAN-Port.
 */
int dnx_vlan_port_types__delete_ac(int unit, ac_e ac_port, uint32 ac_type_flags) {

    int rv;
    bcm_gport_t gport = g_dnx_vlan_port_types.ac_vswitch_info[ac_port].gport;
    bcm_port_t port = g_dnx_vlan_port_types.ac_vswitch_info[ac_port].port;
    bcm_multicast_t mc_group = IN_ACS_VSWITCH;
    ac_types_data_s ac_types_data;
    ac_type_data_ingress_protection data_ingress_protection, *p_data_ingress_protection;
    ac_type_data_fec_protection data_fec_protection, *p_data_fec_protection;
    char *proc_name = "dnx_vlan_port_types__delete_ac";

    /* Detach from a flooding MC group */
    rv = multicast__gport_encap_delete(unit, mc_group, port, gport, 0);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error during multicast__gport_encap_delete of gport 0x%08x from mc_group %d, rv - %d\n",
               proc_name, port, mc_group, rv);
        return rv;
    }

    sal_memset(&ac_types_data, 0, sizeof(ac_types_data));

    /* Fill specific data per ac_type that is required for the configuration */
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_ingress_protection)) {
        ac_type_data_ingress_protection *p_data_ingress_protection;

        data_ingress_protection.is_primary = (g_dnx_vlan_port_types.ac_vswitch_info[ac_conf].port == port) ? TRUE : FALSE;
        data_ingress_protection.ingress_failover_id = DEFAULT_INGRESS_FAILOVER_ID;
        p_data_ingress_protection = &data_ingress_protection;
        sal_memcpy(&(ac_types_data.type_data[ac_type_ingress_protection]), &p_data_ingress_protection,
                   sizeof(ac_types_data.type_data[ac_type_ingress_protection]));
        ac_types_data.is_alloc_additional_objects[ac_type_ingress_protection] = FALSE;
    }
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_fec_protection)) {

        data_fec_protection.is_primary = (g_dnx_vlan_port_types.ac_vswitch_info[ac_conf].port == port) ? TRUE : FALSE;
        data_fec_protection.fec_failover_id = DEFAULT_FEC_FAILOVER_ID;
        data_fec_protection.fec_id = (data_fec_protection.is_primary) ? g_dnx_vlan_port_types.primary_fec_id : 0;
        p_data_fec_protection = &data_fec_protection;
        sal_memcpy(&(ac_types_data.type_data[ac_type_fec_protection]), &p_data_fec_protection,
                   sizeof(ac_types_data.type_data[ac_type_fec_protection]));
        ac_types_data.is_alloc_additional_objects[ac_type_fec_protection] = (data_fec_protection.is_primary) ? FALSE : TRUE;
    }

    rv = vlan_port__delete(unit, ac_type_flags, &ac_types_data, gport);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in vlan_port__delete for gport - %d, rv - %d\n",
               proc_name, gport, rv);
        return rv;
    }

    return BCM_E_NONE;
}

/* Perform port sprcific configurations in order to enable each participating phy port for bridging
 *
 * INPUT:
 *   port: phy port
 *   vlan: Outer VID
 */
int dnx_vlan_port_types__port_bridge_init(int unit, bcm_port_t port, bcm_vlan_t vlan) {

    char *proc_name = "dnx_vlan_port_types__port_bridge_init";
    int rv;

    /* Enable VLAN-Membership filtering */
    rv = bcm_vlan_gport_add(unit, vlan, port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, adding bcm_vlan_gport_add for Port %d and VID %d, rv - %d\n",
               proc_name, port, vlan, rv);
        return rv;
    }

    /* Open up the port for outgoing and ingoing traffic */
    rv = bcm_port_control_set(unit, port, bcmPortControlBridge, 1);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in bcm_port_control_set for port %d, rv - %d\n",
               proc_name, port, rv);
        return rv;
    }

    return BCM_E_NONE;
}

/* Enables another match configuration for the main AC-Conf.
 *
 * INPUT:
 *   match_port: phy port
 *   match_vlan: Outer VID
 */
int dnx_vlan_port_types__match_ac(int unit, bcm_port_t match_port, bcm_vlan_t match_vlan) {

    char *proc_name = "dnx_vlan_port_types__match_ac";
    bcm_port_match_info_t match_info;
    int rv;

    printf("%s(): Add a match port with Port %d and VID %d, rv - %d\n",
               proc_name, match_port, match_vlan, rv);

    bcm_port_match_info_t_init(&match_info);
    match_info.match = BCM_PORT_MATCH_PORT_VLAN;
    match_info.port = match_port;
    match_info.match_vlan = match_vlan;
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;

    rv = bcm_port_match_add(unit, g_dnx_vlan_port_types.ac_vswitch_info[ac_conf].gport, &match_info);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in bcm_port_match_add for Port %d and VID %d, rv - %d\n",
               proc_name, match_port, match_vlan, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Removes an additional match configuration from the main AC-Conf.
 *
 * INPUT:
 *   match_port: phy port
 *   match_vlan: Outer VID
 */
int dnx_vlan_port_types__unmatch_ac(int unit, bcm_port_t match_port, bcm_vlan_t match_vlan) {

    char *proc_name = "dnx_vlan_port_types__unmatch_ac";
    bcm_port_match_info_t match_info;
    int rv;

    bcm_port_match_info_t_init(&match_info);
    match_info.match = BCM_PORT_MATCH_PORT_VLAN;
    match_info.port = match_port;
    match_info.match_vlan = match_vlan;
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;

    rv = bcm_port_match_delete(unit, g_dnx_vlan_port_types.ac_vswitch_info[ac_conf].gport, &match_info);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in bcm_port_match_delete for Port %d and VID %d, rv - %d\n",
               proc_name, match_port, match_vlan, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Main function that performs all the configurations for the example.
 * Performs init configuration before configuring the ACs
 * INPUT: 
 *   params: Configuration info for running the example.
 */
int dnx_vlan_port_types__start_run(int unit, dnx_vlan_port_types_s *params) {

    char *proc_name = "dnx_vlan_port_types__start_run";
    int in_ac_idx, rv;

    /* Perform initializations */
    rv = dnx_vlan_port_types_init(unit, params);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in dnx_vlan_port_types_init, rv - %d\n", proc_name, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* This function runs the main test function with specified ports
 *  
 * INPUT: in_port - Port for the In-ACs that share the same vswitch
 *        out_port -  Port for Out-AC that isn't associated with the In-ACs vswitch
 *        protection_port - Port for protection scenarios
 */
int dnx_vlan_port_types_with_ports__start_run(int unit, int conf_port, int out_port, int protection_port) {

    int in_ac_idx;
    dnx_vlan_port_types_s params;

    dnx_vlan_port_types__struct_get(&params);

    params.ac_vswitch_info[ac_conf].port = conf_port;
    params.ac_vswitch_info[ac_out].port = out_port;
    params.ac_vswitch_info[ac_ing_protection].port = protection_port;
    params.ac_vswitch_info[ac_fec_protection].port = protection_port;

    return dnx_vlan_port_types__start_run(unit, &params);
}

/* The function performs any required cleanup.
 */
int dnx_vlan_port_types__cleanup(int unit) {

    char *proc_name = "dnx_vlan_port_types__cleanup";
    int ac_idx, rv;

    /* Delete all the ACs except for the Conf-AC that is deleted by the TCL */
    for (ac_idx = ac_out; ac_idx < NOF_ACS; ac_idx++)
    {
        rv = dnx_vlan_port_types__delete_ac(unit, ac_idx, g_dnx_vlan_port_types.ac_vswitch_info[ac_idx].ac_type_flags);
        if (rv != BCM_E_NONE) {
            printf("%s(): Error during bcm_vlan_port_destroy for AC-%d, rv - %d\n",
                   proc_name, ac_idx, rv);
            return rv;
        }
    }

    return BCM_E_NONE;
}



