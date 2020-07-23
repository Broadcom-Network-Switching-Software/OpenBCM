/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_oam_basic.c Purpose: basic examples for OAM. 
 */

int oam_user_defined_trap_id;
int oam_egress_snoop_trap_id;

/* pcp based counting on MPLS LIFs*/
int is_pcp_based_counting = 0;

/* 
 * Simple procedure that validate if the endpoint type is MplsLmDm 
 *  
 * @param mep_type - indicates endpoint type
 *
 * @return int
 */
int 
mep_type_is_mpls_lm_dm(int mep_type)
{
    if((mep_type == bcmOAMEndpointTypeMplsLmDmLsp) || (mep_type == bcmOAMEndpointTypeMplsLmDmPw) || (mep_type == bcmOAMEndpointTypeMplsLmDmSection))
    {
        return TRUE;
    }

    return FALSE;
}

/* 
 * Exmaple cint for OAM over MPLS 
 *  - Create 3 MPLS lifs
 *  - Create oam-action-profiles - oam_set_of_action_profiles_create cint should be loaded
 *  - Link oam_profile_mpls_single_ended profile to the MPLS lifs
 *  - Create a LM-MEP on each lif
 */
int
oam_create_3_meps_on_3_lifs(
    int unit,
    int port1,
    int port2,
    int mep_type)
{
    int rv;
    int flags;
    bcm_oam_endpoint_type_t type;
    bcm_oam_profile_type_t profile_type;
    bcm_oam_profile_t ingress_profile_id;
    bcm_oam_profile_t egress_profile_id;
    int raw_lif_action_profile_id;
    uint32 flags_get;
    uint64  arg;
    bcm_gport_t gport_1;
    bcm_gport_t gport_2;
    bcm_gport_t gport_3;
    bcm_gport_t gport_stack[4];
    bcm_oam_group_info_t group_info;
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_action_key_t oam_action_key;
    bcm_oam_action_result_t oam_action_result;
    bcm_oam_profile_t acc_profile_id;
    int counter_base_0;
    int counter_base_1;
    int counter_base_2;
    int counter_if_0 = 0;
    int counter_if_1 = 1;
    int counter_if_2 = 2;
    int count_lm_dm = 0;

    rv = mpls_termination_basic_main(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in mpls_termination_basic_main.\n", rv);
        return rv;
    }

    rv = mpls_termination_basic_tunnel_id_stack_get(unit, 3, gport_stack);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in mpls_termination_basic_tunnel_id_stack_get.\n", rv);
        return rv;
    }

    if (!mep_type_is_mpls_lm_dm(mep_type))
    {
        /*
         * Create MEG
         */
        bcm_oam_group_info_t_init(group_info);

        group_info.id = 7;
        group_info.flags = BCM_OAM_GROUP_WITH_ID;
        rv = bcm_oam_group_create(unit, group_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_group_create.\n", rv);
            return rv;
        }

        group_info_short_ma.id = group_info.id;


        /*
         * Create OAM profiles
         */
        rv = oam_set_of_action_profiles_create(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in oam_set_of_action_profiles_create.\n", rv);
            return rv;
        }

        raw_lif_action_profile_id = oam_lif_profiles.oam_profile_mpls_single_ended;
        rv = bcm_oam_profile_id_get_by_type(unit, raw_lif_action_profile_id, bcmOAMProfileIngressLIF, &flags_get, &ingress_profile_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_oam_profile_id_get_by_type.\n", rv);
            return rv;
        }
    }
    else
    { 
        rv = mpls_lm_dm_action_profiles_config(unit, mep_type, is_ntp, count_lm_dm, &ingress_profile_id, &acc_profile_id);
        if (rv != BCM_E_NONE)
        {
           printf("Error in bcm_oam_lif_profile_set: (%s)\n", bcm_errmsg(rv));
           return rv;
        }
    }

    egress_profile_id = BCM_OAM_PROFILE_INVALID;

    flags = 0;
    rv = bcm_oam_lif_profile_set(unit, flags, gport_stack[0], ingress_profile_id, egress_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
        return rv;
    }
    rv = bcm_oam_lif_profile_set(unit, flags, gport_stack[1], ingress_profile_id, egress_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
        return rv;
    }
    rv = bcm_oam_lif_profile_set(unit, flags, gport_stack[2], ingress_profile_id, egress_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
        return rv;
    }

    if (is_pcp_based_counting) {

       /*
        * Set Counter Range for PCP based counters.
        * As default all counter range defined as non-PCP.
        * To use PCP based counters, limited counter range
        * should be set. 
        * To set all counters for PCP, 
        * range_min = range_max = 0 should be used. 
        */

       /* Config Range to : Min=0, Max=0 . That allows the counters to be set */
       COMPILER_64_ZERO(arg);
       rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMin, arg);
       if (rv != BCM_E_NONE)
       {
           printf("Error no %d, in bcm_oam_control_set.\n", rv);
           return rv;
       }
       rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMax, arg);
       if (rv != BCM_E_NONE)
       {
           printf("Error no %d, in bcm_oam_control_set.\n", rv);
           return rv;
       }
    }

    /*
     * Allocate counter engines, and get counters
     */
    rv = set_counter_resource(unit, port1, counter_if_0, 1, &counter_base_0);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in set_counter_resource.\n", rv);
        return rv;
    }
    rv = set_counter_resource(unit, port1, counter_if_1, 1, &counter_base_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in set_counter_resource.\n", rv);
        return rv;
    }
    rv = set_counter_resource(unit, port1, counter_if_2, 1, &counter_base_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in set_counter_resource.\n", rv);
        return rv;
    }
 
    /*
     * Create MEP on each lif
     */

    bcm_oam_endpoint_info_t_init(endpoint_info);

    endpoint_info.type = mep_type;
    endpoint_info.gport = gport_stack[0];
    endpoint_info.mpls_out_gport = BCM_GPORT_INVALID;
    endpoint_info.id = 1;
    endpoint_info.lm_counter_base_id = counter_base_0;
    endpoint_info.lm_counter_if = counter_if_0;
    if (!mep_type_is_mpls_lm_dm(mep_type))
    {
        endpoint_info.group = group_info.id;
        endpoint_info.level = 1;
        endpoint_info.lm_flags |=  BCM_OAM_LM_PCP*is_pcp_based_counting;
    }
    rv = bcm_oam_endpoint_create(unit, endpoint_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_dnx_oam_endpoint_create.\n", rv);
        return rv;
    }

    ep1.id = endpoint_info.id;
    ep1.gport = endpoint_info.gport;
    ep1.lm_counter_base_id = endpoint_info.lm_counter_base_id;

    endpoint_info.type = mep_type;
    endpoint_info.gport = gport_stack[1];
    endpoint_info.mpls_out_gport = BCM_GPORT_INVALID;
    endpoint_info.id = 2;
    endpoint_info.lm_counter_base_id = counter_base_1;
    endpoint_info.lm_counter_if = counter_if_1;
    if (!mep_type_is_mpls_lm_dm(mep_type))
    {
        endpoint_info.group = group_info.id;
        endpoint_info.level = 1;
        endpoint_info.lm_flags |=  BCM_OAM_LM_PCP*is_pcp_based_counting;
    }
    rv = bcm_oam_endpoint_create(unit, endpoint_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_dnx_oam_endpoint_create.\n", rv);
        return rv;
    }

    ep2.id = endpoint_info.id;
    ep2.gport = endpoint_info.gport;
    ep2.lm_counter_base_id = endpoint_info.lm_counter_base_id;

    endpoint_info.type = mep_type;
    endpoint_info.gport = gport_stack[2];
    endpoint_info.mpls_out_gport = BCM_GPORT_INVALID;
    endpoint_info.id = 3;
    endpoint_info.lm_counter_base_id = counter_base_2;
    endpoint_info.lm_counter_if = counter_if_2;
    if (!mep_type_is_mpls_lm_dm(mep_type))
    {
        endpoint_info.group = group_info.id;
        endpoint_info.level = 1;
        endpoint_info.lm_flags |=  BCM_OAM_LM_PCP*is_pcp_based_counting;
    }
    rv = bcm_oam_endpoint_create(unit, endpoint_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_dnx_oam_endpoint_create.\n", rv);
        return rv;
    }

    ep3.id = endpoint_info.id;
    ep3.gport = endpoint_info.gport;
    ep3.lm_counter_base_id = endpoint_info.lm_counter_base_id;

    return rv;
}

/* 
 * Exmaple cint for ETH-OAM MEP create
 */
int
oam_create_eth_mep(
    int unit,
    int gport,
    int group_id,
    int mdl,
    int counter_idx,
    int counter_if,
    int is_up,
    int is_pcp)
{
   bcm_oam_endpoint_info_t endpoint_info;
    int rv;
    int flags;

    bcm_oam_endpoint_info_t_init(endpoint_info);

    endpoint_info.type = bcmOAMEndpointTypeEthernet;
    endpoint_info.gport = gport;
    endpoint_info.group = group_id;
    endpoint_info.level = mdl;
    endpoint_info.lm_counter_base_id = counter_idx;
    endpoint_info.lm_counter_if = counter_if;
    if (is_up) 
    {
       endpoint_info.flags |=  BCM_OAM_ENDPOINT_UP_FACING;
    }
    if (is_pcp) 
    {
       endpoint_info.lm_flags |=  BCM_OAM_LM_PCP;
    }
    rv = bcm_oam_endpoint_create(unit, endpoint_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_dnx_oam_endpoint_create.\n", rv);
        return rv;
    }
    return rv;
}

/* 
 * Example for down-MEP with pcp based counting
 */
int
oam_pcp_based_counting(
    int unit,
    int gport,
    int group_id,
    int mdl,
    int counter_base,
    int counter_if)
{
   int rv;
   uint64  arg;
   int is_pcp;
   int is_up;

   /*
    * Set Counter Range for PCP based counters.
    * As default all counter range defined as non-PCP.
    * To use PCP based counters, limited counter range
    * should be set. 
    * To set all counters for PCP, 
    * range_min = range_max = 0 should be used. 
    */

   /* Config Range to : Min=0, Max=0 . That allows the counters to be set */
   COMPILER_64_ZERO(arg);
   rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMin, arg);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in bcm_oam_control_set.\n", rv);
       return rv;
   }
   rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMax, arg);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in bcm_oam_control_set.\n", rv);
       return rv;
   }

    /*
     * Create MEP with counter_base out of the range. 
     * Counter base is advised to be aligned to 8. 
     * If counter base is not aligned to 8 it will be truncated.
     * Counter index to be used for a specific pcp value:
     * counter_idx = ((counter_base>>3)<<3) + pcp
     */
   is_pcp = 1;
   is_up = 0;
   rv = oam_create_eth_mep(unit,gport,group_id,mdl,counter_base,counter_if,is_up,is_pcp);
   if (rv != BCM_E_NONE)
   {
     printf("Error no %d, in oam_create_eth_mep.\n", rv);
     return rv;
   }
   return rv;
}

/* 
 * Example for MPLS down-MEP with pcp based counting
 */
int
oam_mpls_pcp_based_counting(
    int unit,
    int gport,
    int gport2,
    int mdl,
    int counter_base,
    int counter_if)
{
   int rv;
   uint64  arg;
   uint32 flags;
   int is_ingress;
   int raw_ingress_profile_id;
   bcm_oam_profile_type_t profile_type;
   bcm_oam_profile_t ingress_profile_id;
   bcm_oam_profile_t egress_profile_id;
   bcm_gport_t gport_stack[4];
   bcm_oam_group_info_t group_info;
   bcm_oam_endpoint_info_t endpoint_info;
   bcm_oam_action_key_t oam_action_key;
   bcm_oam_action_result_t oam_action_result;
   
   /*
    * Set Counter Range for PCP based counters.
    * As default all counter range defined as non-PCP.
    * To use PCP based counters, limited counter range
    * should be set.
    * To set all counters for PCP, 
    * range_min = range_max = 0 should be used. 
    */

   /* Config Range to : Min=0, Max=0 . That allows the counters to be set */
   COMPILER_64_ZERO(arg);
   rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMin, arg);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in bcm_oam_control_set.\n", rv);
       return rv;
   }
   rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMax, arg);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in bcm_oam_control_set.\n", rv);
       return rv;
   }

    rv = mpls_termination_basic_main(unit, gport, gport2);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in mpls_termination_basic_main.\n", rv);
        return rv;
    }

    rv = mpls_termination_basic_tunnel_id_stack_get(unit, 3, gport_stack);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in mpls_termination_basic_tunnel_id_stack_get.\n", rv);
        return rv;
    }

    /*
     * Create MEG
     */
    bcm_oam_group_info_t_init(group_info);

    group_info.id = 7;
    group_info.flags = BCM_OAM_GROUP_WITH_ID;
    rv = bcm_oam_group_create(unit, group_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_group_create.\n", rv);
        return rv;
    }

    /*
     * Configure action profile for MPLS (profile 5)
     */
    is_ingress = 1;
    rv = oam_set_of_action_profiles_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in oam_set_of_action_profiles_create.\n", rv);
        return rv;
    }
    /*
     * Use default MPLS ingress action profile
     */
    egress_profile_id = BCM_OAM_PROFILE_INVALID;

    flags = 0;
    rv = bcm_oam_lif_profile_set(unit, flags, gport_stack[2], oam_lif_profiles.oam_profile_mpls_single_ended, egress_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
        return rv;
    }

    /*
     * Create MPLS down MEP
     */
    bcm_oam_endpoint_info_t_init(endpoint_info);

    endpoint_info.type = bcmOAMEndpointTypeBHHMPLS;
    endpoint_info.gport = gport_stack[2];
    endpoint_info.mpls_out_gport = BCM_GPORT_INVALID;
    endpoint_info.group = group_info.id;
    endpoint_info.id = 1;
    endpoint_info.level = mdl;
    endpoint_info.lm_counter_base_id = counter_base;
    endpoint_info.lm_counter_if = counter_if;
    endpoint_info.lm_flags = BCM_OAM_LM_PCP;
    rv = bcm_oam_endpoint_create(unit, endpoint_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_dnx_oam_endpoint_create.\n", rv);
        return rv;
    }
    return rv;
}

/*
* Example of pcp based counting on egress 
* For downmeps predefined 1-1 tc - oam_pcp mapping is used.
* For upmeps the mapping is defined by OAM_priority_profile. 
*  
* Inputs: 
*  in_lif - The out-lif on which the MEP resides.
*  OAM_priority_profile - Defines thethe tc - oam_pcp mapping
*                         4 priority profiles are available in Jericho2
*                         Each profile defines a specific tc - oam_pcp mapping.
*                         In this example 1-1 mapping is defined.
*  group_id - The MEG on which the MEP resides.
*  up_mep_lm_counter_base - base index for counters (counter for the pcp=0)
*  up_mep_lm_counter_if   - interface of the counters (0-2)
 */
int oam_upmep_pcp_example(int unit, int in_lif, int OAM_priority_profile, int *group_id, int up_mep_lm_counter_base,int up_mep_lm_counter_if) {
    bcm_oam_endpoint_info_t ep;
    bcm_qos_map_t oam_pcp_qos_map;
    int     rv;
    int     oam_pcp_qos_map_id;
    int     egress_tc;
    uint64  arg;
    int raw_lif_action_profile_id;
    bcm_oam_group_info_t group_info;
    int ing_profile;
    int eg_profile;
    uint32 flags_get;
    int ac_to_ac;
    bcm_mac_t mac_upmep = {0x00, 0x00, 0x00, 0xff, 0xff, 0xff};

    bcm_oam_endpoint_info_t_init(&ep);

    /*
     * Set Counter Range for PCP based counters.
     * As default all counter range defined as non-PCP.
     * To use PCP based counters, limited counter range
     * should be set. 
     * To set all counters for PCP, 
     * range_min = range_max = 0 should be used. 
     */

    /* Config Range to : Min=0, Max=0 . That allows the counters to be set */
    COMPILER_64_ZERO(arg);
    rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMin, arg);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_control_set.\n", rv);
        return rv;
    }
    rv = bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMax, arg);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_control_set.\n", rv);
        return rv;
    }

    bcm_qos_map_t_init(&oam_pcp_qos_map);

    oam_pcp_qos_map_id = OAM_priority_profile; /* (map_id 0-3 are available)*/

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_OAM_PCP | BCM_QOS_MAP_WITH_ID, &oam_pcp_qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_qos_map_create.\n", rv);
        return rv;
    }

    for (egress_tc = 0; egress_tc < 8; egress_tc++) 
    {

        oam_pcp_qos_map.int_pri = egress_tc;
        oam_pcp_qos_map.pkt_pri = egress_tc; /* OAM-PCP taken from this value. Use a 1:1 mapping in this case from the TC to the OAM-PCP.*/

        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_OAM_PCP, &oam_pcp_qos_map, oam_pcp_qos_map_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in bcm_qos_map_add.\n", rv);
            return rv;
        }
    }

    /* Set the OAM-outlif-profile on the up MEP's gport.*/
    rv = bcm_qos_port_map_set(unit, in_lif, -1 /* ingress map */, oam_pcp_qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /* Get ingress profile id */
    raw_lif_action_profile_id = 1;
    rv = bcm_oam_profile_id_get_by_type(unit, raw_lif_action_profile_id, bcmOAMProfileIngressLIF, &flags_get, &ing_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_profile_id_get_by_type.\n", rv);
        return rv;
    }

    /* Get egress profile id */
    raw_lif_action_profile_id = 1;
    rv = bcm_oam_profile_id_get_by_type(unit, raw_lif_action_profile_id, bcmOAMProfileEgressLIF, &flags_get, &eg_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_profile_id_get_by_type.\n", rv);
        return rv;
    }

    /* Bind profile ids to lif */
    rv = bcm_oam_lif_profile_set(unit, 0, in_lif, ing_profile, eg_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_profile_id_get_by_type.\n", rv);
        return rv;
    }

    /*
     * Create MEG
     */
    bcm_oam_group_info_t_init(group_info);

    group_info.id = 7;
    group_info.flags = BCM_OAM_GROUP_WITH_ID;
    rv = bcm_oam_group_create(unit, group_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error no %d, in bcm_oam_group_create.\n", rv);
        return rv;
    }
    *group_id = group_info.id;

    /* Create up MEP.*/
    bcm_oam_endpoint_info_t_init(&ep);
    ep.gport = in_lif;
    ep.type = bcmOAMEndpointTypeEthernet;
    ep.level = 3;
    ep.group = group_info.id;
    ep.lm_counter_base_id = up_mep_lm_counter_base;
    ep.lm_counter_if = up_mep_lm_counter_if;

    ep.lm_flags = BCM_OAM_LM_PCP;
    ep.flags = BCM_OAM_ENDPOINT_UP_FACING;

    rv = bcm_oam_endpoint_create(unit, &ep);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    return 0;
}

/* 
 * Counter Engine Background:
 * Jericho2 CRPS consist of counter engines with different number counters: 
 *  
 * 8 engines of  8K counters 
 * 8 engines of 16K counters 
 * 6 engines of 32K counters 
 *  
 * For OAM up to 6 counter databases are needed: 3 for Ingress interfaces, and 3 for Egress. 
 * Each database may use several engines to form a database with the required number of counters. 
 *  
 *  
 * Function name: set_counter_resource 
 * ----------------------------------- 
 *  
 * Whenever creating an endpoint with counter this function 
 * should be called. The value returned in counter_base should
 * go into the field lm_counter_base_id and the counter_if into
 * the lm_counter_if field in bcm_oam_endpoint_create().
 * 
 * This function returns a counter or a batch of 8 counters for PCP based counting.
 *  
 * In this example databases are created and engines are added to databases on the fly. When 
 * a counter database is full, another engine is added. 
 *  
 * Inputs: 
 * port - the port on which the endpoint resides 
 * counter_if - the interface on which the counter is requested 
 * is_pcp - pcp counters request indication. If set, a batch of 8 counters will be set
 *  
 * Output: 
 * counter_base: pointer to index of the counter or the first counter out of 8 counters for pcp
 */
/** Next engine to be added to database */
int next_engine = 0;

/* Next counter_base to be set for counter_if per core
 * 6 indexes for core 0, 6 indexes for core 1
 */
int counter_id[12]={0,0,0,0,0,0,0,0,0,0,0,0};
int nof_counters[12]={0,0,0,0,0,0,0,0,0,0,0,0};

int set_counter_resource(int unit, int port, int counter_if, int is_pcp, int *counter_base)
{
    /*
     * CRPS - General
     */
    bcm_stat_counter_interface_type_t oam_source[3] = {bcmStatCounterInterfaceIngressOam, bcmStatCounterInterfaceEgressOam, bcmStatCounterInterfaceOamp};
    int ingress_command_id[3] = {7,8,9};
    int egress_command_id[3] = {0,1,2};
    /*
     * Counter Engines List in requested allocation order
     */
    int engine_array[22] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21};
    /*
     * General variables
     */
    int added_nof_counters;
    int database_id[2];
    int command_id[2];
    int core_id,tm_port;
    int i,j;
    int nof_engines;
    int dnx_data_nof_engines;
    int total_nof_counters_to_compare = 0;
    uint32 total_nof_counters_get;
    uint32 *engine_size;
    int nof_requested_counters;
	int offset_to_next_counter_base;
	
    int rv;

    nof_requested_counters = 1;
    if(is_pcp)
    {
      /* In case of pcp we will allocate 8 counters starting at the nearest 8 aligned counter_base */
	  offset_to_next_counter_base = (8 - (counter_id[database_id[0]] & 7) & 7);
      nof_requested_counters = 8 + offset_to_next_counter_base;
    }

    dnx_data_nof_engines = *(dnxc_data_get(unit,  "crps", "engine", "nof_engines", NULL)); 
    if (next_engine >= dnx_data_nof_engines) 
    {
       /** No availabe engines */
       printf("Currently %d engines are in use and no available counter engines left.\n", next_engine);
       return BCM_E_FULL;
    }

    rv = get_core_and_tm_port_from_port(unit, port, &core_id, &tm_port);
    if (rv != BCM_E_NONE) {
       printf("Error, in get_core_and_tm_port_from_port\n");
       return rv;
    }

   /*
    * Input verification
    */
   if ((core_id > 1) || (core_id<0))
   {
      printf("Error. Illegal Core \n");
      return (-4);
   }
   if ((counter_if > 2) || (counter_if < 0))
   {
      printf("Error. Illegal counter interface \n");
      return (-4);
   }

   /*
    * Using database_id 0-2 for ingress, 3-5 for egress
    */
   database_id[0] = core_id*6 + counter_if;
   database_id[1] = core_id*6 + counter_if + 3;
   command_id[0]=ingress_command_id[counter_if];
   command_id[1]=egress_command_id[counter_if];

   /** Check if initial database configuration is required */
   if (counter_id[database_id[0]] == 0)
   {
       /*
        * Initial configuration for ingress and egress
        */
       nof_engines = 1;
       for (i=0 ; i<2 ; i++) 
       {
           total_nof_counters_to_compare = 0;
           /** create OAM CRPS database*/
           rv = crps_oam_database_set(unit, core_id, oam_source[i], command_id[i], nof_engines, &(engine_array[next_engine]), /*counter_base*/ 0, database_id[i], &total_nof_counters_get);
           if (rv != BCM_E_NONE)
           {
               printf("Error in crps_oam_database_set \n");
               return rv;
           }
           
           /** calculate the size of the engines in double entry; done
            *  only for comparison */
           for (j = 0; j < nof_engines; j++)
           {
                engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", engine_array[next_engine]);
                total_nof_counters_to_compare = total_nof_counters_to_compare + 2*(*engine_size); /** since in double entry mode*/
           }
           if (total_nof_counters_get != total_nof_counters_to_compare)
           {
               printf("Error in crps_oam_database_set, wrong actual_nof_counters=%d received, expected=%d \n",total_nof_counters_get,total_nof_counters_to_compare);
               return rv;
           }
           next_engine++;
           /** Number of available counters */
           nof_counters[database_id[i]] += total_nof_counters_get;
           /** Don't use counter_base = 0 */
           counter_id[database_id[i]] = nof_requested_counters;
           nof_counters[database_id[i]] -=nof_requested_counters;
       }
   }

   /*
    * Engines with same number of counters are used for ingress and egress 
    * Therefore only ingress database maybe checked for fullness. 
    *  
    * We need to check that there are enough counters available for PCP based 
    * counting. 
    */
   if (counter_id[database_id[0]] + nof_requested_counters >= nof_counters[database_id[0]])
   {
       /*
        * No more counters available. Add an engine to ingress and databases.
        */
       for (i=0 ; i<2 ; i++) 
       {
          if (next_engine >= 22) 
          {
             /** No availabe engines */
             printf("Error in crps_oam_database_add. No available counter engines left.\n");
             return BCM_E_FULL;
          }
           rv = crps_oam_database_engines_add(unit, core_id, database_id[i], 1, engine_array[next_engine++], &total_nof_counters_get);
           if (rv != BCM_E_NONE)
           {
               printf("Error in crps_oam_database_engines_add \n");
               return rv;
           }
           nof_counters[database_id[i]] = total_nof_counters_get;
       }
   }
   
   /* Assign new counter_base. Allocate 8 counters to support PCP based counting if requested. */
   *counter_base = counter_id[database_id[0]] + offset_to_next_counter_base;
   counter_id[database_id[0]] += nof_requested_counters;

   return 0;
}

/* Helper function for counter resource allocation without pcp */
int set_counter_resource_no_pcp(int unit, int port, int counter_if, int *counter_base)
{
int rv;
   rv = set_counter_resource(unit, port, counter_if, 0, counter_base);
   if (rv != BCM_E_NONE)
   {
	   printf("Error in set_counter_resource \n");
	   return rv;
   }
}

/* Helper function for counter resource allocation with pcp */
int set_counter_resource_pcp(int unit, int port, int counter_if, int *counter_base)
{
int rv;
   rv = set_counter_resource(unit, port, counter_if, 1, counter_base);
   if (rv != BCM_E_NONE)
   {
	   printf("Error in set_counter_resource \n");
	   return rv;
   }
}

/**
 * Create a snoop and a trap associated with that snoop.
 * Changes the action for the given profiles based on
 * opcode and MAC type to snoop OAM packets
 * on both directions to a given destination.
 *
 *
 * @param unit
 * @param dest_snoop_port
 * @param opcode
 * @param mac_type
 * @param ing_profile_id
 * @param egr_profile_id
 *
 * @return int
 */
int mip_create_additional_snoop(
    int unit,
    bcm_gport_t dest_snoop_port,
    bcm_oam_opcode_t opcode,
    bcm_oam_dest_mac_type_t mac_type,
    bcm_oam_profile_t ing_profile_id,
    bcm_oam_profile_t egr_profile_id)
{
    bcm_error_t rv;
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_destination_t mirror_dest;
    bcm_rx_trap_config_t trap_config_snoop;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    int trap_gport;
    int is_snoop;
    int snoop_cmnd;
    int trap_code;
    int flags = 0;
    int trap_strength = 0;
    int snoop_strength = 3;

    /* Create a new snoop command for ingress to be associated with the new trap */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.gport = dest_snoop_port;
    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP;

    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_destination_create.\n");
        return rv;
    }

    /* Check if snoop was created */
    is_snoop = BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest.mirror_dest_id);
    if (!is_snoop)
    {
        printf("Error. Could not create snoop.\n");
        return -1;
    }

    /* Get the snoop command */
    snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);

    /* Create a new trap */
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_rx_trap_type_create.\n");
       return rv;
    }

    oam_user_defined_trap_id = trap_code;

    /* Setup the new trap */
    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
    trap_config_snoop.trap_strength = 0;
    trap_config_snoop.snoop_cmnd = snoop_cmnd;

    rv = bcm_rx_trap_set(unit, trap_code, trap_config_snoop);
    if (rv != BCM_E_NONE)
    {
       printf("Error in bcm_rx_trap_set.\n");
       return rv;
    }

    /* Setup an action that uses the new trap */
    bcm_oam_action_key_t_init(&action_key);
    bcm_oam_action_result_t_init(&action_result);

    /* Encode trap, trap strength and snoop strength as a gport */
    BCM_GPORT_TRAP_SET(trap_gport, trap_code, trap_strength, snoop_strength);
    action_key.flags = 0;
    action_key.inject = 0;
    action_key.opcode = opcode;
    action_key.endpoint_type = bcmOAMMatchTypeMIP;
    action_key.dest_mac_type = mac_type;

    action_result.destination = trap_gport;
    action_result.counter_increment = 0;
    action_result.flags = 0;
    action_result.meter_enable = 1;

    /* Update the action for the Ingress profile */
    rv = bcm_oam_profile_action_set(unit, flags, ing_profile_id, &action_key, &action_result);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_oam_profile_action_set.\n");
        return rv;
    }

    /* Create a new snoop command for egress */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.gport = dest_snoop_port;
    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP | BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER;
    mirror_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;

    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_destination_create.\n");
        return rv;
    }

    /* Check if snoop was created */
    is_snoop = BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest.mirror_dest_id);
    if (!is_snoop)
    {
        printf("Error. Could not create snoop.\n");
        return -1;
    }

    /* Get the snoop command */
    snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);

    /*
     * Call mirror_header_info_set in addition to ensure
     * that snooped packet will come with an FHEI
     */
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.src_sysport = 13; /* packets can be recognized according to this field */
    rv = bcm_mirror_header_info_set(unit, BCM_MIRROR_DEST_IS_SNOOP, mirror_dest.mirror_dest_id, &mirror_header_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_header_info_set.\n");
        return rv;
    }

    /* For egress, use the snoop command as destination */
    BCM_GPORT_TRAP_SET(trap_gport, snoop_cmnd, trap_strength, snoop_strength);
    action_result.destination = trap_gport;
    /* Update the action for the Egress profile */
    rv = bcm_oam_profile_action_set(unit, flags, egr_profile_id, &action_key, &action_result);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_oam_profile_action_set.\n");
        return rv;
    }

    /**
      * MIP snoop packets will come with a sniff/trap FHEI and
      * the trap code being that associated with
      * (bcmRxTrapEgTxFieldSnoop0+ mirror_dest.mirror_dest_id).
      *
      * in other words the trap_code returned from
      * bcm_rx_trap_type_get(unit,0,bcmRxTrapEgTxFieldSnoop0+mirror_dest.mirror_dest_id,&oam_egress_snoop_trap_id);
      *
      * After the first set of system headers, egress snooped packets
      * will have exactly 64 bytes of the original packet,
      * including system headers from the egress.
      * This is due to the flag
      * BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER.
      * After those 64 bytes the snooped packet will include the
      * full original network header.
      */
    rv = bcm_rx_trap_type_get(unit, flags, bcmRxTrapEgTxFieldSnoop0 + snoop_cmnd, &oam_egress_snoop_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_type_get.\n");
        return rv;
    }

    return rv;
}

/**
 * Create a snoop and a trap associated with that snoop.
 * Changes the action for the given profile based on
 * opcode and MAC type to snoop OAM packets
 * to a given destination.
 *
 *
 * @param unit
 * @param dest_snoop_port
 * @param opcode
 * @param mac_type
 * @param counter_increment
 * @param profile_id - profile created with bcm_oam_profile_create()
 * of type:
 * - bcmOAMProfileIngressLIF for non-accelerated Down-MEPs.
 * - bcmOAMProfileEgressLIF for non-accelerated Up-MEPs.
 * - bcmOAMProfileIngressAcceleratedEndpoint for accelerated Down-MEPs.
 * - bcmOAMProfileEgressAcceleratedEndpoint for accelerated Up-MEPs.
 * or invalid profile in which case no action will be configured.
 * @param is_ingress
 * @return int
 */
int dnx_oam_change_mep_destination_to_snoop(
    int unit,
    bcm_gport_t dest_snoop_port,
    bcm_oam_opcode_t opcode,
    bcm_oam_dest_mac_type_t mac_type,
    int counter_increment,
    bcm_oam_profile_t profile_id,
    int is_ingress)
{
    bcm_error_t rv;
    bcm_oam_profile_type_t profile_type;
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_destination_t mirror_dest;
    bcm_rx_trap_config_t trap_config_snoop;
    bcm_oam_action_key_t action_key;
    bcm_oam_action_result_t action_result;
    int trap_gport;
    int is_snoop;
    int snoop_cmnd;
    int trap_code;
    int flags = 0;
    int trap_strength = 0;
    int snoop_strength = 3;

    /* Create a new snoop command to be associated with the new trap */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.gport = dest_snoop_port;
    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP;

    if (!is_ingress)
    {
        mirror_dest.flags |= BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER;
        mirror_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
    }

    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_destination_create.\n");
        return rv;
    }

    /* Check if snoop was created */
    is_snoop = BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest.mirror_dest_id);
    if (!is_snoop)
    {
        printf("Error. Could not create snoop.\n");
        return -1;
    }

    /* Get the snoop command */
    snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);

    if (is_ingress)
    {
        /* Create a new trap */
        rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_code);
        if (rv != BCM_E_NONE)
        {
           printf("Error in bcm_rx_trap_type_create.\n");
           return rv;
        }

        bcm_rx_trap_config_t_init(&trap_config_snoop);
        trap_config_snoop.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
        trap_config_snoop.trap_strength = 0;
        trap_config_snoop.snoop_cmnd = snoop_cmnd;

        rv = bcm_rx_trap_set(unit, trap_code, trap_config_snoop);
        if (rv != BCM_E_NONE)
        {
           printf("Error in bcm_rx_trap_set.\n");
           return rv;
        }

        oam_user_defined_trap_id = trap_code;
    }
    else
    {
        /*
         * Call mirror_header_info_set in addition to ensure
         * that snooped packet will come with an FHEI
         */
        bcm_mirror_header_info_t_init(&mirror_header_info);
        mirror_header_info.tm.src_sysport = 13; /* packets can be recognized according to this field */
        rv = bcm_mirror_header_info_set(unit, BCM_MIRROR_DEST_IS_SNOOP, mirror_dest.mirror_dest_id, &mirror_header_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_mirror_header_info_set.\n");
            return rv;
        }

        /**
          * Up-MEP snoop packets will come with a sniff/trap FHEI and
          * the trap code being that associated with
          * (bcmRxTrapEgTxFieldSnoop0+ mirror_dest.mirror_dest_id).
          *
          * in other words the trap_code returned from
          * bcm_rx_trap_type_get(unit,0,bcmRxTrapEgTxFieldSnoop0+mirror_dest.mirror_dest_id,&oam_egress_snoop_trap_id);
          *
          * After the first set of system headers, egress snooped packets
          * will have exactly 64 bytes of the original packet,
          * including system headers from the egress.
          * This is due to the flag
          * BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER.
          * After those 64 bytes the snooped packet will include the
          * full original network header.
          */
        rv = bcm_rx_trap_type_get(unit, flags, bcmRxTrapEgTxFieldSnoop0 + snoop_cmnd, &oam_egress_snoop_trap_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_rx_trap_type_get.\n");
            return rv;
        }
    }

    if (profile_id != BCM_OAM_PROFILE_INVALID)
    {
        /* Setup an action that uses the new trap */
        bcm_oam_action_key_t_init(&action_key);
        bcm_oam_action_result_t_init(&action_result);
        if (is_ingress)
        {
            /* Encode user-defined trap, trap strength and snoop strength as a gport */
            BCM_GPORT_TRAP_SET(trap_gport, trap_code, trap_strength, snoop_strength);
        }
        else
        {
            /* Encode snoop command, trap strength and snoop strength as a gport */
            BCM_GPORT_TRAP_SET(trap_gport, snoop_cmnd, trap_strength, snoop_strength);
        }
        action_key.flags = 0;
        action_key.inject = 0;
        action_key.opcode = opcode;
        action_key.endpoint_type = bcmOAMMatchTypeMEP;
        action_key.dest_mac_type = mac_type;

        action_result.destination = trap_gport;
        action_result.counter_increment =  counter_increment;
        action_result.flags = 0;
        action_result.meter_enable = 1;

        /* Update the action for the given profile */
        rv = bcm_oam_profile_action_set(unit, flags, profile_id, &action_key, &action_result);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_oam_profile_action_set.\n");
            return rv;
        }
    }

    return rv;
}

/**
 * Create Ethernet Upmep connected to VPLS tunnel 
 * ---------------------------------------------- 
 *  
 * Endpoint on egress will be created on the inner ETH 
 * LIF. 
 * Packets CCM o ETH o PWE o ETH will be trapped and and counted 
 * by this endpoint. 
 * Packets on outer ETH and on PWE will be terminated. 
 * Tested with:
 * CCMoETH(vlan=5)oPWE(label=3456)oMPLS(label=1111)oETH(mac=00:0c:00:02:00:01) 
 *
 * @param unit
 * @param port - AC port
 * @param port2 - PWE port
 *
 * @return int
 */ 
int oam_up_mep_over_vpls_run_with_defaults_dnx(int unit, int port, int port2,int is_acc) {

    int rv;
    int counter_if;
    int lm_counter_base_id;
    bcm_oam_group_info_t group_info;
    bcm_oam_endpoint_info_t ep;
    bcm_oam_profile_t ingress_profile_id;
    bcm_oam_profile_t egress_profile_id;
    int group_id;
    uint8 raw_lif_action_profile_id;
    bcm_oam_profile_t acc_profile_id;
    uint32 flags;
    unsigned int flags_get;
    uint8 group_name[] =  { 0x1, 0x3, 0x2, 0xd, 0xe };
    bcm_oam_endpoint_info_t remote_ep;

   /* 
    * Create LIFs
    */ 
   rv = vpls_mp_basic_main(unit,port,port2);
   if (rv != BCM_E_NONE) {
       printf("Error,vpls_mp_basic_main(%s) \n",bcm_errmsg(rv));
       return rv;
   }

   /*
   * 1. Set port tpid class:
   * 2. Set a vlan translation command
   */ 
   rv = vpls_mp_basic_ve_swap(unit,port,1);
   if (rv != BCM_E_NONE) {
       printf("Error,vpls_mp_basic_ve_swap(%s) \n",bcm_errmsg(rv));
       return rv;
   }

   /*
    * Allocate counter resource for the Endpoint
    */
   counter_if = 0;
   rv = set_counter_resource(unit, port, counter_if, 1, &lm_counter_base_id);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in set_counter_resource.\n", rv);
       return rv;
   }

   /*
    * Create MEG
    */
   bcm_oam_group_info_t_init(group_info);

   group_info.id = 7;
   group_info.flags = BCM_OAM_GROUP_WITH_ID;
   if (is_acc)
   {
     sal_memcpy(group_info.name, &group_name, sizeof(group_name));
   }

   rv = bcm_oam_group_create(unit, group_info);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in bcm_oam_group_create.\n", rv);
       return rv;
   }
   group_id = group_info.id;

   /*
    * Create OAM profiles
    */
   rv = oam_set_of_action_profiles_create(unit);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in oam_set_of_action_profiles_create.\n", rv);
       return rv;
   }

   /*
    * Set ingress profile
    */
   raw_lif_action_profile_id = oam_lif_profiles.oam_profile_eth_single_ended;
   rv = bcm_oam_profile_id_get_by_type(unit, raw_lif_action_profile_id, bcmOAMProfileIngressLIF, &flags_get, &ingress_profile_id);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in bcm_oam_profile_id_get_by_type.\n", rv);
       return rv;
   }

   /*
    * Set egress profile
    */
   raw_lif_action_profile_id = oam_lif_profiles.oam_profile_eth_single_ended;
   rv = bcm_oam_profile_id_get_by_type(unit, raw_lif_action_profile_id, bcmOAMProfileEgressLIF, &flags_get, &egress_profile_id);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in bcm_oam_profile_id_get_by_type.\n", rv);
       return rv;
   }

   /*
    * Link profiles to LIF
    */
   flags = 0;
   rv = bcm_oam_lif_profile_set(unit, flags, cint_vpls_basic_info.vlan_port_id, ingress_profile_id, egress_profile_id);
   if (rv != BCM_E_NONE)
   {
       printf("Error no %d, in bcm_oam_lif_profile_set.\n", rv);
       return rv;
   }

   if (is_acc)
   {
   /*
    * Set ingress profile
    */
     raw_lif_action_profile_id = oam_acc_lif_profiles.oam_acc_profile_eth_single_ended;
     rv = bcm_oam_profile_id_get_by_type(unit, raw_lif_action_profile_id, bcmOAMProfileEgressAcceleratedEndpoint, &flags_get, &acc_profile_id);
     if (rv != BCM_E_NONE)
     {
       printf("Error no %d, in bcm_oam_profile_id_get_by_type.\n", rv);
       return rv;
     }

     oam_set_opcode_destination_to_oamp(unit,0,acc_profile_id,bcmOamOpcodeCCM,bcmOAMEndpointTypeEthernet);
     oam_set_opcode_destination_to_oamp(unit,0,acc_profile_id,bcmOamOpcodeLMM,bcmOAMEndpointTypeEthernet);
   }

   /* 
    *  Create upMEP
    */
   bcm_oam_endpoint_info_t_init(&ep);
   ep.gport = cint_vpls_basic_info.vlan_port_id;
   ep.type = bcmOAMEndpointTypeEthernet;
   ep.level = 3;
   ep.group = group_info.id;
   ep.lm_counter_base_id = lm_counter_base_id;
   ep.lm_counter_if = counter_if;
   ep.flags = BCM_OAM_ENDPOINT_UP_FACING;
   if (is_acc)
   {
     ep.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
     ep.acc_profile_id = acc_profile_id;
   }

   rv = bcm_oam_endpoint_create(unit, &ep);
   if (rv != BCM_E_NONE) {
       printf("Error no %d, in bcm_oam_endpoint_create.\n", rv);
       return rv;
   }
   printf("Created endpoint id=%d\n",ep.id);

   /* Create RMEP */
   if (is_acc)
   {
       bcm_oam_endpoint_info_t_init(&remote_ep);
       remote_ep.name = 0xff;
       remote_ep.local_id = ep.id;
       remote_ep.type = bcmOAMEndpointTypeEthernet;
       remote_ep.ccm_period = 0;
       remote_ep.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
       remote_ep.loc_clear_threshold = 1;
       remote_ep.id = ep.id;

       printf("Creating remote endpoint\n");
       rv = bcm_oam_endpoint_create(unit, &remote_ep);
       if (rv != BCM_E_NONE)
       {
           printf("(%s) \n", bcm_errmsg(rv));
           return rv;
       }
       printf("Created RMEP: ID %d\n", remote_ep.id);

       rv = register_events(unit);
       if (rv != BCM_E_NONE)
       {
           printf("(%s) \n", bcm_errmsg(rv));
           return rv;
       }

   }
   return rv;
}

/**
 * Get OAMP gport
 *
 * @param unit
 * @return int
 */
int dnx_oamp_gport_get(int unit, bcm_gport_t *oamp_gports) {
    bcm_error_t rv;
    int count;

    rv = bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 2, oamp_gports, &count);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    if (count < 1) {
        printf("OAMP GPort not found.\n");
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/**
 * Configure snoop command to OAMP BFD trap.
 *
 *
 * @param unit
 * @param dest_snoop_port
 * @param is_ipv6
 * @return int
 */
int dnx_oamp_bfd_trap_snoop_config(int unit, bcm_gport_t dest_snoop_port, int is_ipv6) {
    bcm_error_t rv;
    bcm_mirror_destination_t mirror_dest;
    bcm_rx_trap_config_t trap_config_snoop;
    int is_snoop;
    int snoop_cmnd;
    int trap_code;
    int flags = 0;

    /* Create a new snoop command to be associated with the new trap */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.gport = dest_snoop_port;
    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP;

    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_destination_create.\n");
        return rv;
    }

    /* Check if snoop was created */
    is_snoop = BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest.mirror_dest_id);
    if (!is_snoop)
    {
        printf("Error. Could not create snoop.\n");
        return -1;
    }

    /* Get the snoop command */
    snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);

    /* Configure trap */
    if (is_ipv6)
    {
        rv = bcm_rx_trap_type_get(unit, flags, bcmRxTrapOamBfdIpv6, &trap_code);
    }
    else
    {
        rv = bcm_rx_trap_type_get(unit, flags, bcmRxTrapOamBfdIpv4, &trap_code);
    }

    if (rv != BCM_E_NONE) {
       printf("Error in bcm_rx_trap_type_create.\n");
       return rv;
    }

    bcm_gport_t oamp_gports[2];
    rv = dnx_oamp_gport_get(unit, &oamp_gports);
    if (rv != BCM_E_NONE) {
        printf("Error in OAMP gport get.\n");
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP);
    trap_config_snoop.trap_strength = 0;
    trap_config_snoop.snoop_cmnd = snoop_cmnd;
    trap_config_snoop.dest_port = oamp_gports[0];

    rv = bcm_rx_trap_set(unit, trap_code, trap_config_snoop);
    if (rv != BCM_E_NONE) {
       printf("Error in bcm_rx_trap_set.\n");
       return rv;
    }

    return rv;
}

/**
 * Change the action for default profiles
 * based on opcode and MAC type to snoop
 * OAM packets to a given destination.
 *
 *
 * @param unit
 * @param dest_snoop_port
 * @param opcode
 * @param mac_type
 *
 * @return int
 */
int mip_egress_snooping(
    int unit,
    bcm_gport_t dest_snoop_port,
    bcm_oam_opcode_t opcode,
    bcm_oam_dest_mac_type_t mac_type)
{
    bcm_error_t rv;
    uint32 flags = 0;
    bcm_oam_profile_t ingress_profile_id = 1;
    bcm_oam_profile_t egress_profile_id = 1;

    /**
     * Get default profile encoded values.
     * Default profile is 1 and configured
     * on init(application) stage.
     */
    rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile_id, bcmOAMProfileIngressLIF, &flags, &ingress_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_oam_profile_id_get_by_type(ingress)\n");
        return rv;
    }

    rv = bcm_oam_profile_id_get_by_type(unit, egress_profile_id, bcmOAMProfileEgressLIF, &flags, &egress_profile_id);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_oam_profile_id_get_by_type(egress)\n");
        return rv;
    }

    /* Configure egress snoop command */
    rv = mip_create_additional_snoop(unit, dest_snoop_port, opcode, mac_type, ingress_profile_id, egress_profile_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error in mip_create_additional_snoop.\n");
       return rv;
    }

    return rv;
}

/**
 * Disabled same interface filter on destination Port of egress snoop packet, in interop mode
 * In interop mode, same interface filter is enabled for snoop packet. In JR2 mode, the filter is disabled by default. 
 * In case destination port of snoop packet is same as source system port in 2nd pass and same interface filter is enabled, snoop packet will be dropped.
 * Note, The source system port of snoop packet can be set using API bcm_mirror_header_info_set.
 *
 * @param unit
 * @param dest_snoop_port
 *
 * @return int
 */
int oam_egress_snoop_iop_dis_same_interface (
    int unit,
    bcm_gport_t dest_snoop_port)
{
    bcm_error_t rv = BCM_E_NONE;

    /** Disabled same interface filter on destination Port of egress snoop packet */
    rv = bcm_port_control_set(unit, dest_snoop_port, bcmPortControlBridge, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv(%d), bcm_port_control_set with bcmPortControlBridge\n");
        return rv;
    }

    return rv;
}


/**
 * Configure time-of-day (NTP and IEEE1588) to a given value
 * @param unit
 * @param dest_snoop_port
 *
 * @return int
 */
int oam_time_of_day_set (int unit, uint32 time_upper, uint32 time_lower)
{
    bcm_error_t rv = BCM_E_NONE;
	uint64 tmp;

    COMPILER_64_SET(tmp,time_upper,time_lower);

    rv = bcm_oam_control_set(unit,bcmOamControlNtpToD,tmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv(%d), bcm_oam_control_set for ntp\n");
        return rv;
    }
    rv = bcm_oam_control_set(unit,bcmOamControl1588ToD,tmp);
    if (rv != BCM_E_NONE)
    {
        printf("Error rv(%d), bcm_oam_control_set for ieee 1588\n");
        return rv;
    }

    return rv;
}

