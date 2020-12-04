/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/**************************************************************************************************************************************************
 *
 * Purposes of this cint is demonstrate creating ETH-OAM endpoint's with VSI in order to count CCM packets together with data packets.
 *
 *
 * set following soc properties
 *   bcm886xx_vlan_translate_mode=1
 *   set of soc_properties that enable oam.
 *
 * This cint is creating 2 endpoints with VSI with two different OutLifs.
 * Each Lif have different Egress Vlan Editing profile(egress vlan editing is performed on such packets).
 * The cint demonstrate creating two diferent egress vlan editing profiles(one for each lif).
 * Both endpoints inject untagged packet and on egress side one or two tags added according vlan editing profile
 *
 * run:                                                                                                                                             *
 * cd ../../../../src/examples/sand                                                                                                                  *
 * cint ../sand/utility/cint_sand_utils_global.c                                                                                                                 *
 * cint utility/cint_utils_vlan.c                                                                                                                   *
 * cint utility/cint_utils_port.c                                                                                                                   *
 * cint utility/cint_utils_l2.c                                                                                                                     *
 * cint utility/cint_utils_oam.c                                                                                                                    *
 * cint cint_oam_egress_injection.c                                                                                                                 *
 * cint                                                                                                                                             *
 * int unit = 0;                                                                                                                                    *
 * egress_injection__start_run(int unit,  egress_injection_s *param) - param could be NULL if you want to use default parameters                    *
 * OR                                                                                                                                               *
 * egress_injection_with_ports__start_run(int unit,  int in_port, int out_port) - if you want to change only ports(ingress/egress)                  *
 *                                                                                                                                                  *
 * This will create two endpoint that will start to send CCM packets.                                                                               *
 *                                                                                                                                                  *
 * All default values could be re-written with initialization of the global structure 'g_egress_injection', before calling the main  function       *
 * In order to re-write only part of values, call 'egress_injection_struct_get(egress_injection_s)' function and re-write values,                   *
 * prior calling the main function                                                                                                                  *
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
struct egress_injection_eve_s{
    vlan_edit_utils_s    egress_vlan_profile;
    vlan_action_utils_s  egress_vlan_action;
    bcm_vlan_t           outer_vlan;
    bcm_vlan_t           inner_vlan;
};

struct egress_injection_ep_s{
    bcm_oam_endpoint_t id;
    uint16 name;
    bcm_oam_endpoint_t remote_id;
    uint16 remote_name;
    int lm_counter;
    bcm_mac_t ep_dmac;
    bcm_mac_t ep_smac;
};

/* Main struct */
struct egress_injection_s{
    int sys_port[NUMBER_OF_PORTS];
    bcm_gport_t lif[2];
    int vsi;
    egress_injection_eve_s egress_vlan_edit[2];
    uint8 group_name[BCM_OAM_GROUP_NAME_LENGTH];
    uint8 group_name_48b[BCM_OAM_GROUP_NAME_LENGTH];
    int extra_group_data_index;
    egress_injection_ep_s  endpoint[2];
    int tpid_flag;
};

egress_injection_s g_egress_injection = {
/*************     Ports     **********************/
        /* PORT1 | PORT2 */
        { 200    , 201 },

/*************     Lif's    **********************/
        {0x20000 ,  0x20001},
/*************     VSI      **********************/
        20,

/**********  Egress VLAN Editing   ***************/
        {
                {
                        { 10          ,     0       ,     2}, /* Edit Profile, Tag format, action_id */
                        { 2      ,   0x8100   , bcmVlanActionAdd   ,   0x8100   , bcmVlanActionAdd   },/* action */
                        100, /* Outer vlan*/
                        101  /* Inner vlan*/
                },
                {
                        {     11          ,     0       ,     3      },  /* Edit Profile, Tag format, action_id */
                        {   3      ,   0x8100   , bcmVlanActionAdd   ,   0        , bcmVlanActionNone   }, /* action */
                        202,  /* Outer vlan*/
                        0     /* Inner vlan*/
                }
        },
/**********        Endpoint         ***************/
        {1, 3, 2, 0xab, 0xcd},
        {0xa9, 0x23, 0x52, 0x8c, 0xd4, 0xb0, 0x18, 0x37,
         0xf4, 0x77, 0x41, 0x22, 0xe3, 0x70, 0xa4, 0x92,
         0x0f, 0x87, 0xff, 0x00, 0x9d, 0x42, 0x3b, 0x24,
         0x15, 0x3c, 0xd8, 0xa1, 0x43, 0x80, 0xb2, 0x62,
         0x5d, 0xa7, 0x66, 0xea, 0x27, 0x41, 0x93, 0x36,
         0x0d, 0x45, 0x9c, 0xab, 0x21, 0x79, 0x30, 0x4d},
         0x2014, /* extra_group_data_index */
        {
            /* id | name | remote_id | remote_name | lm_counter |   Destination MAC                   |    Source MAC */
            {-1   , 0x10 ,   -1      ,   0xfd      , -1         , {0x00, 0x00, 0x00, 0x01, 0x01, 0x01}, {0x00, 0x00, 0x00, 0x11, 0x11, 0x11}},
            {-1   , 0x11 ,   -1      ,   0xfd      , -1         , {0x00, 0x00, 0x00, 0x02, 0x02, 0x02}, {0x00, 0x00, 0x00, 0x22, 0x22, 0x22}}
        }
};

/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of main struct
 * Function allow to re-write default values
 *
 * INPUT:
 *   params: new values for egress_injection_s
 */
int egress_injection_init(int unit, egress_injection_s *param){

    if (param != NULL) {
       sal_memcpy(&g_egress_injection, param, sizeof(g_egress_injection));
    }

    if (!is_device_or_above(unit, JERICHO2))
    {
        advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

        if (!advanced_vlan_translation_mode ) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Return egress_injection_s information
 */
void egress_injection__struct_get(int unit, egress_injection_s *param){

    sal_memcpy( param, &g_egress_injection, sizeof(g_egress_injection));

    return;
}

/* This function runs the main test function with given ports
 *
 * INPUT: unit     - unit
 *        in_port  - ingress port
 *        out_port - egress port
 *        maid48 - mention is it maid48 or not
 *        bcm_oam_endpoint_memory_type_t - memory type (self-contained/ short) - relevant for JR2 only)
 */
int egress_injection_with_ports__start_run(int unit,  int port1, int port2, int maid48, bcm_oam_endpoint_memory_type_t  endpoint_memory_type){
    int rv;

    egress_injection_s param;

    egress_injection__struct_get(unit, &param);

    param.sys_port[0] = port1;
    param.sys_port[1] = port2;

    return egress_injection__start_run(unit, &param, maid48, endpoint_memory_type);
}


/*****************************************************************************/
/*                OAM  Egress Injection(QAX)                                 */
/*****************************************************************************/

/*
 * Main function runs the egress injection example
 *
 * Main steps of configuration:
 *    1. Creating VSI
 *    2. Setting VLAN domain for ports.
 *    3. Create LIF's for each port
 *    4. Call function that define Egress VLAN Translation
 *    5. Call function that creates enpoints
 *
 * INPUT: unit  - unit
 *        param - new values for egress_injection(in case it's NULL default values will be used).
 *        maid8 - mention if it's maid48 or not
 *        bcm_oam_endpoint_memory_type_t - memory type(delf-contained /short) - relevant for JR2 only
 */
int egress_injection__start_run(int unit, egress_injection_s *param, int maid_48b, bcm_oam_endpoint_memory_type_t endpoint_memory_type)
{
    int rv;
    int i;
    int gport;

    rv = egress_injection_init(unit, param);
    if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        printf("Registering OAM events\n");
        rv = register_events(unit);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    /* Create VSI */
    rv  = bcm_vlan_create(unit, g_egress_injection.vsi);
    if (rv != BCM_E_NONE ) {
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, g_egress_injection.vsi, rv);
        return rv;
    }

    /* set VLAN domain to each port */
    for(i=0; i<NUMBER_OF_PORTS; i++) {
        rv = port__vlan_domain__set(unit, g_egress_injection.sys_port[i], g_egress_injection.sys_port[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, in port__vlan_domain__set, port=%d, \n",  g_egress_injection.sys_port[i]);
            return rv;
        }

        if (!is_device_or_above(unit, JERICHO2))
        {
            /* Set outer and inner tpid */
            rv = port__tpid__set(unit, g_egress_injection.sys_port[i], g_egress_injection.egress_vlan_edit[i].outer_vlan, g_egress_injection.egress_vlan_edit[i].inner_vlan);
            if (rv != BCM_E_NONE) {
                printf("Error, in port__tpid__set, port=%d, \n",   g_egress_injection.sys_port[i]);
                return rv;
            }
        }

        /* Define Tag Format for in_port
         * Untagged packet get tag format - '0'
         * Other packets(one tag) get tag format - '1'
         * Other packets(double-tag) get tag format - '2'
         */
        rv = port__basic_tpid_class__set(unit,g_egress_injection.sys_port[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, in port__basic_tpid_class__set, port=%d, \n",  g_egress_injection.sys_port[i]);
            return rv;
        }

        BCM_GPORT_SUB_TYPE_LIF_SET(gport, 0, g_egress_injection.lif[i]);
        BCM_GPORT_VLAN_PORT_ID_SET(gport,gport);

        /* Create LIF's for <Port,vlan>*/
        rv = l2__port_vlan__create(unit,BCM_VLAN_PORT_WITH_ID,  g_egress_injection.sys_port[i],
                                       0,
                                       gport,g_egress_injection.egress_vlan_edit[i].outer_vlan) ;

        if (rv != BCM_E_NONE) {
            printf("Error, l2__port_vlan__create\n");
            return rv;
        }

        rv = bcm_vswitch_port_add(unit, g_egress_injection.vsi, gport);
        BCM_IF_ERROR_RETURN(rv);
        rv = l2__mact_entry_create( unit, g_egress_injection.endpoint[i].ep_smac, g_egress_injection.vsi, g_egress_injection.sys_port[i]);
        if (rv != BCM_E_NONE) {
            printf("Fail  l2__mact_entry_create ");
            return rv;
        }
    }

    if(g_egress_injection.tpid_flag){
        /* Set ive tpid profile */
        rv = egress_injection_ive_command_set(unit, g_egress_injection.sys_port[0], g_egress_injection.vsi);
        if (rv != BCM_E_NONE) {
            printf("Fail  egress_injection_egress_vlan_translation_set ");
            return rv;
        }
        /* Set Egress VLAN Translation */
        rv = egress_injection_eve_command_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Fail  egress_injection_egress_vlan_translation_set ");
            return rv;
        }
    }
    else {
        /* Set Egress VLAN Translation */
        rv = egress_injection_egress_vlan_translation_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Fail  egress_injection_egress_vlan_translation_set ");
            return rv;
        }
    }
    rv = egress_injection_ep_create(unit, maid_48b, endpoint_memory_type);
    return rv;
}


int egress_injection_ive_command_set(int unit, int port,int vlan)
{
    int rv=0;
    bcm_gport_t vlan_port = 0;
    rv = l2__port__create(unit, 0,  port, vlan, &vlan_port );
    if (rv != BCM_E_NONE) {
        printf("Error, in l2__port__create, port=%d, \n",   port);
        return rv;
    }

    /* Set outer and inner tpid */
    rv = port__tpid__set(unit, port, /*o_tpid*/0x88a8, /*i_tpid*/0);
    if (rv != BCM_E_NONE) {
        printf("Error, in port__tpid__set, port=%d, \n",   port);
        return rv;
    }

    /*1, set tag_format 3 for outer: TPID1, inner: ANY ==> S_tag(3) */
    rv = port__tag_classification__set(unit,port,3,0x88a8,BCM_PORT_TPID_CLASS_TPID_ANY);
    if (rv != BCM_E_NONE) {
        printf("Error, in port__tag_classification__set, port=%d, \n", port);
        return rv;
    }

    return rv;
}

int egress_injection_eve_command_set(int unit)
{
    int rv=0;
    int gport;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport, 0, g_egress_injection.lif[0]);
    BCM_GPORT_VLAN_PORT_ID_SET(gport,gport);
    /* Egress VLAN Translation */
    rv = vlan__port_translation__set(unit, g_egress_injection.egress_vlan_edit[0].outer_vlan,
        g_egress_injection.egress_vlan_edit[0].inner_vlan,
        gport,
        g_egress_injection.egress_vlan_edit[0].egress_vlan_profile.edit_profile,0);
    if (rv != BCM_E_NONE) {
        printf("Fail  vlan__port_translation__set ");
        return rv;
    }

    rv = vlan__translate_action_with_id__set(unit, g_egress_injection.egress_vlan_edit[0].egress_vlan_action.action_id,
        0x8100,bcmVlanActionAdd,0,bcmVlanActionNone,0);
    if (rv != BCM_E_NONE) {
        printf("Fail  vlan__translate_action_with_id__set for action %d ", i);
        return rv;
    }

    rv = vlan__translate_action_class__set(unit, g_egress_injection.egress_vlan_edit[0].egress_vlan_action.action_id,
                g_egress_injection.egress_vlan_edit[0].egress_vlan_profile.edit_profile,3 /* 3 for outer: TPID1, inner: ANY ==> S_tag(3) */,0);
    if (rv != BCM_E_NONE) {
            printf("Fail  vlan__egress_translate_action_class__set vlan edit profile %d  ", i);
            return rv;
    }

    return rv;
}


int egress_injection_eve_command_update(int unit,bcm_vlan_action_t outer_action,bcm_vlan_action_t inner_action,uint16 outer_tpid,uint16  inner_tpid)
{
    int rv=0;
    bcm_vlan_action_set_t action;
    bcm_port_tag_format_class_t tag_format = 3; /*  single tag format */

    /* Set translation action */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = outer_action;
    action.dt_inner = inner_action;
    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;

    rv = bcm_vlan_translate_action_id_set( unit,
                                           BCM_VLAN_ACTION_SET_EGRESS ,
                                           g_egress_injection.egress_vlan_edit[0].egress_vlan_action.action_id,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set 1\n");
        return rv;
    }

    rv = vlan__translate_action_class__set(unit, g_egress_injection.egress_vlan_edit[0].egress_vlan_action.action_id,
                g_egress_injection.egress_vlan_edit[0].egress_vlan_profile.edit_profile,tag_format,0);
    if (rv != BCM_E_NONE) {
            printf("Fail  vlan__egress_translate_action_class__set vlan edit profile %d  ", i);
            return rv;
    }

    return rv;
}


int egress_injection_egress_vlan_translation_set(int unit)
{
    int rv=0;
    int i;
    int gport;

    for(i=0; i<2; i++) {

        BCM_GPORT_SUB_TYPE_LIF_SET(gport, 0, g_egress_injection.lif[i]);
        BCM_GPORT_VLAN_PORT_ID_SET(gport,gport);
        /* Egress VLAN Translation */
        rv = vlan__port_translation__set(unit, g_egress_injection.egress_vlan_edit[i].outer_vlan,
                g_egress_injection.egress_vlan_edit[i].inner_vlan,
                gport,
                g_egress_injection.egress_vlan_edit[i].egress_vlan_profile.edit_profile,
                0);
        if (rv != BCM_E_NONE) {
            printf("Fail  vlan__port_translation__set ");
            return rv;
        }

        rv = vlan__translate_action_with_id__set(unit, g_egress_injection.egress_vlan_edit[i].egress_vlan_action.action_id,
                g_egress_injection.egress_vlan_edit[i].egress_vlan_action.o_tpid,
                g_egress_injection.egress_vlan_edit[i].egress_vlan_action.o_action,
                g_egress_injection.egress_vlan_edit[i].egress_vlan_action.i_tpid,
                g_egress_injection.egress_vlan_edit[i].egress_vlan_action.i_action,
                0);

        if (rv != BCM_E_NONE) {
            printf("Fail  vlan__translate_action_with_id__set for action %d ", i);
            return rv;
        }

        rv = vlan__translate_action_class__set(unit, g_egress_injection.egress_vlan_edit[i].egress_vlan_action.action_id,
                g_egress_injection.egress_vlan_edit[i].egress_vlan_profile.edit_profile,
                g_egress_injection.egress_vlan_edit[i].egress_vlan_profile.tag_format,
                0);


        if (rv != BCM_E_NONE) {
            printf("Fail  vlan__egress_translate_action_class__set vlan edit profile %d  ", i);
            return rv;
        }
    }

    return rv;
}
/* Function creating 2 ETH-OAM endpointss and enabling counting */
int egress_injection_ep_create(int unit, int maid_48b, bcm_oam_endpoint_memory_type_t endpoint_memory_type ) {
    bcm_oam_group_info_t group_info;
    bcm_oam_endpoint_info_t  acc_endpoint;
    bcm_oam_endpoint_info_t  remote_ep;
    bcm_oam_endpoint_action_t action;
    int i, extra_group_data_index;
    int rv;
    int gport;
    uint32 flags;
    bcm_oam_profile_type_t profile_type;
    bcm_oam_profile_t ing_profile_id;
    bcm_oam_profile_t egr_profile_id;
    bcm_oam_profile_t acc_profile_id;
    uint8 ing_lif_profile_action_id;
    uint8 egr_lif_profile_action_id;
    uint8 acc_profile_action_id;


    bcm_oam_group_info_t_init(&group_info);

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    if (maid_48b) {
        group_info.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
        extra_group_data_index = (device_type == device_type_qux) ? 0x870 : g_egress_injection.extra_group_data_index;
        group_info.group_name_index = extra_group_data_index;

        if(is_device_or_above(unit, JERICHO2))
        {
            if((endpoint_memory_type == bcmOamEndpointMemoryTypeShortEntry )||
               (endpoint_memory_type == bcmOamEndpointMemoryTypeLmDmOffloadedEntry ))
            {
                 group_info.group_name_index = 0;
            }
            else
            {
                 group_info.group_name_index = 0xA014;
            }
        }
        sal_memcpy(group_info.name, g_egress_injection.group_name_48b, BCM_OAM_GROUP_NAME_LENGTH);
    }
    else {
        sal_memcpy(group_info.name, g_egress_injection.group_name, BCM_OAM_GROUP_NAME_LENGTH);
    }
    rv = bcm_oam_group_create(unit, &group_info);
    if( rv != BCM_E_NONE) {
        printf("Fail  bcm_oam_group_create");
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        ing_lif_profile_action_id = 1;
        rv = bcm_oam_profile_id_get_by_type(unit, ing_lif_profile_action_id, bcmOAMProfileIngressLIF, &flags, &ing_profile_id);
        printf(" Ingress profile id: %d\n", ing_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_oam_profile_id_get_by_type failed with error: (%s) \n", bcm_errmsg(rv));
            return rv;
        }

        egr_lif_profile_action_id = 1;
        rv = bcm_oam_profile_id_get_by_type(unit, egr_lif_profile_action_id, bcmOAMProfileEgressLIF, &flags, &egr_profile_id);
        printf(" Egress profile id: %d\n", egr_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_oam_profile_id_get_by_type failed with error: (%s) \n", bcm_errmsg(rv));
            return rv;
        }

        acc_profile_action_id = 1;
        rv = bcm_oam_profile_id_get_by_type(unit, acc_profile_action_id, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_oam_profile_id_get_by_type failed with error: (%s) \n", bcm_errmsg(rv));
            return rv;
        }

        /*
         * Bind profiles to oam_lif
         */
        for(i=0; i<2; i++){
            BCM_GPORT_SUB_TYPE_LIF_SET(gport, 0, g_egress_injection.lif[i]);
            BCM_GPORT_VLAN_PORT_ID_SET(gport,gport);

            rv = bcm_oam_lif_profile_set(unit, 0, gport, ing_profile_id, egr_profile_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
                return rv;
            }
        }

    }

    if (is_device_or_above(unit, JERICHO2))
    {
        rv= set_counter_resource(unit, g_egress_injection.sys_port[0], 0, 1, &g_egress_injection.endpoint[1].lm_counter);
    }
    else
    {
        rv = set_counter_source_and_engines(unit, &g_egress_injection.endpoint[1].lm_counter, g_egress_injection.sys_port[0]);
    }
    if (rv != BCM_E_NONE) {
         printf("Fail  set_counter_source_and_engines MEP 0  ");
         return rv;
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        rv= set_counter_resource(unit, g_egress_injection.sys_port[1], 0, 1, &g_egress_injection.endpoint[0].lm_counter);
    }
    else
    {
        rv = set_counter_source_and_engines(unit, &g_egress_injection.endpoint[0].lm_counter, g_egress_injection.sys_port[1]);
    }
    if (rv != BCM_E_NONE) {
         printf("Fail  set_counter_source_and_engines MEP 1  ");
         return rv;
     }

    for(i=0; i<2; i++){

        BCM_GPORT_SUB_TYPE_LIF_SET(gport, 0, g_egress_injection.lif[i]);
        BCM_GPORT_VLAN_PORT_ID_SET(gport,gport);

        /* Endpoint Create */
        bcm_oam_endpoint_info_t_init(&acc_endpoint);
        acc_endpoint.type = bcmOAMEndpointTypeEthernet;
        acc_endpoint.group = group_info.id;
        acc_endpoint.name = g_egress_injection.endpoint[i].name;
        acc_endpoint.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
        acc_endpoint.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
        acc_endpoint.level = 1;
        acc_endpoint.flags2|= BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN;
        acc_endpoint.vpn = g_egress_injection.vsi;

        if (is_device_or_above(unit, JERICHO2))
        {
            acc_endpoint.acc_profile_id = acc_profile_id;
            acc_endpoint.endpoint_memory_type = endpoint_memory_type;
        }

        if(g_egress_injection.tpid_flag){
            acc_endpoint.outer_tpid= 0x88a8;
            acc_endpoint.vlan= 200;
        }

        BCM_GPORT_SYSTEM_PORT_ID_SET(acc_endpoint.tx_gport, g_egress_injection.sys_port[i]);

        acc_endpoint.gport = gport;
        if (!is_device_or_above(unit, JERICHO2))
        {
            sal_memcpy(acc_endpoint.dst_mac_address, g_egress_injection.endpoint[i].ep_dmac, 6);
            acc_endpoint.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
        }
        else
        {
            /* Setting My-CFM-MAC (for Jericho2 Only)*/
            if(device_type >= device_type_jericho2)
            {
                rv = oam__my_cfm_mac_set(unit, g_egress_injection.sys_port[i], g_egress_injection.endpoint[i].ep_dmac);
                if (rv != BCM_E_NONE) {
                    printf("oam__my_cfm_mac_set fail \n");
                    return rv;
                }
            }

        }
        sal_memcpy(acc_endpoint.src_mac_address, g_egress_injection.endpoint[i].ep_smac, 6);
        acc_endpoint.lm_counter_base_id = g_egress_injection.endpoint[i].lm_counter;
        rv = bcm_oam_endpoint_create(unit, &acc_endpoint);

        if (rv != BCM_E_NONE) {
            printf("Fail  bcm_oam_endpoint_create MEP %d  ", i);
            return rv;
        } else
            printf("bcm_oam_endpoint_create MEP %d\n",i);

        g_egress_injection.endpoint[i].id = acc_endpoint.id;

        /* Remote Endpoint Create */
        bcm_oam_endpoint_info_t_init(&remote_ep);
        remote_ep.name = g_egress_injection.endpoint[i].remote_name;
        remote_ep.local_id = acc_endpoint.id;
        remote_ep.type = bcmOAMEndpointTypeEthernet;
        remote_ep.ccm_period = 0;
        remote_ep.flags |= BCM_OAM_ENDPOINT_REMOTE;
        remote_ep.loc_clear_threshold = 1;
        remote_ep.id = acc_endpoint.id;
        remote_ep.flags2 = BCM_OAM_ENDPOINT2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT2_RDI_ON_LOC;

        if((is_device_or_above(unit, JERICHO2)) &&
           (*(dnxc_data_get(unit, "oam", "oamp",
           "rmep_db_non_zero_lifetime_units_limitation", NULL)))) {
            /** Needed to avoid limitation in JR2 A0 */
            remote_ep.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
        }

        rv = bcm_oam_endpoint_create(unit, &remote_ep);
        if (rv != BCM_E_NONE) {
            printf("Fail  bcm_oam_endpoint_create RMEP %d  ", i);
            return rv;
        } else{
            printf("bcm_oam_endpoint_create RMEP %d\n",i);
       }

        g_egress_injection.endpoint[i].remote_id = remote_ep.id;

       if(g_egress_injection.tpid_flag){
           /* Only create one MEP for tpid issue test*/
           break;
       }
    }

    if (!is_device_or_above(unit, JERICHO2))
    {
        /* Counter Enable */
        bcm_oam_endpoint_action_t_init(&action);
        BCM_OAM_OPCODE_SET(action,1);
        BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable);
        bcm_oam_endpoint_action_set(unit, 0, &action);
        if (rv != BCM_E_NONE) {
            printf("Fail  bcm_oam_endpoint_action_set %d  ", i);
            return rv;
        }

        bcm_oam_endpoint_action_set(unit, 1, &action);
        if (rv != BCM_E_NONE) {
            printf("Fail  bcm_oam_endpoint_action_set %d  ", i);
            return rv;
        }
    }
   return 0;
}


/*
*   The example is about how to build egress injected packet in QMX/JER (without OAMP).
*   pkt_ccm packet:
*   0x41000d01|020000|20000000500000|0000010000|0180C2000031000000111111|88a800c8|89022064|0346000000000010010302ABCD000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
*   ITMH     ITMH-Ext     PPH          FHEI      OAM-ccm (use opcode 0x64 ,not 1)
*   ITMH(23:8) : Destination-system-port (in this case send packet to port 13)
*   ITMH-EXT(18:0) : Out-lif   (in this case out_lif is 0x20000)
*   PPH (53:52) : FHEI-Size      (in this case FHEI is 5B)
*   PPH (33:18) : VSI           (in this case VSI is 20)
*   FHEI (21:16): TPID-profile  (in this case out_lif is 1)
*
*   pkt_aps packet:
*   0xc1000d01|020000|000000000000|20000000500000|0000010000|0a01020304050b00001111118902202700000000000000000000000000000000000000000000000000000000
*   ITMH     ITMH-Ext      OAM-TS     PPH          FHEI      APS
*   ITMH(23:8) : Destination-system-port (in this case send packet to port 13)
*   ITMH-EXT(18:0) :  Out-lif   (in this out_lif is 0x20000)
*   OAM-TS (47:0) : Dummy oam-ts   (in this case, it's 000000000000)
*   PPH (53:52) : FHEI-Size      (in this case FHEI is 5B)
*   PPH (33:18) : VSI           (in this case VSI is 20)
*   FHEI (21:16): TPID-profile
*
*   pkt_lm packet:
*   0X81000d00|080000001012|0a01020304050b00001111118902202B000C0000000000000000000000000000000000000000000000000000
*   ITMH         OAM-TS       LMM
*   ITMH (23:8) : Destination-system-port
*   OAM-TS (41:8) : counter-pointer
*   OAM-TS (7:0) : offset to stamp TxFCf
*
*   pkt_lm packet(for COE port):
*   0xc1000d01|000000|200000001012|20000000500000|0000010000|0a01020304050b0000111111|88a800c8|8902202B|000C0000000000000000000000000000000000000000000000000000
*   ITMH     ITMH-Ext      OAM-TS     PPH          FHEI      LMM
*   ITMH(23:8) : Destination-system-port (in this case send packet to port 13)
*   ITMH-EXT(18:0) : Dummy Out-lif   (in this case out_lif is 0)
*   OAM-TS (45:43) : OAM-Sub-Type   (in this case, LM is using subtype 4)
*   OAM-TS (41:8) : counter-pointer
*   OAM-TS (7:0) : offset to stamp TxFCf
*   PPH (53:52) : FHEI-Size      (in this case FHEI is 5B)
*   PPH (33:18) : VSI           (in this case VSI is 20)
*   FHEI (21:16): TPID-profile
*
*   pkt_dm packet:
*   0X81000d00|100000000012|0ab00ef00000000001111118902202f0120000000000000000000000000000000000000000000000000000000000000000000
*   ITMH           OAM-TS       DMM
*   ITMH (23:8) : Destination-system-port (
*   OAM-TS (7:0) : offset to stamp TxTimestampf
*
*   pkt_dm packet(for COE port):
*   0xc1000d01|000000|100000000012|20000000500000|0000010000|0a01020304050b0000111111|88a800c8|8902202F|01200000000000000000000000000000000000000000000000000000
*   ITMH     ITMH-Ext       OAM-TS       PPH          FHEI      DMM
*   ITMH(23:8) : Destination-system-port (in this case send packet to port 13)
*   ITMH-EXT(18:0) : Dummy Out-lif   (in this case out_lif is 0)
*   OAM-TS (45:43) : OAM-Sub-Type   (in this case, DM is using subtype 2)
*   OAM-TS (7:0) : offset to stamp TxFCf
*   PPH (53:52) : FHEI-Size      (in this case FHEI is 5B)
*   PPH (33:18) : VSI           (in this case VSI is 20)
*   FHEI (21:16): TPID-profile
*/
int inject_oam_packets(int unit) {
    bcm_error_t rv;
    char *pkt_ccm, *pkt_lm, *pkt_dm;
    int port1  = 13, port2 =14;
    pkt_ccm = "0X4100c010200002000000050000000000100000180C200003100000011111188a800c8890220640346000000000010010302ABCD000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"; /*ccm down*/
    pkt_lm = "0X81000d000800000010120a01020304050b00001111118902202B000C0000000000000000000000000000000000000000000000000000"; /*LM down*/
    pkt_dm = "0X81000d0010000000001200AB00EF00000000001111118902202F0120000000000000000000000000000000000000000000000000000000000000000000"; /*DM down*/

    rv = egress_injection_with_ports__start_run(unit,port1,port2, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("egress_injection_with_ports__start_run error %d \n ", rv);
        return rv;
    }

    /*Injected ccm*/
    rv = oam_tx(unit,pkt_ccm,0,port1);
    if (rv != BCM_E_NONE) {
        printf("Fail  to send ccm packet with error %d \n ", rv);
        return rv;
    }

    /*Injected lm*/
    rv = oam_tx(unit,pkt_lm,0,port1);
    if (rv != BCM_E_NONE) {
        printf("Fail  to send lmm packet with error  %d \n ", rv);
        return rv;
    }

    /*Injected dm*/
    rv = oam_tx(unit,pkt_dm,0,port1);
    if (rv != BCM_E_NONE) {
        printf("Fail  to send dmm packet with error  %d  \n", rv);
        return rv;
    }
    return rv;

}
