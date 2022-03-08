/* $Id: cint_end_to_end_aqm_latency.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

 /* Explanation:
 *  This cint includes functions that are required to activate end to end AQM latency measurements.
 *
 *  Three egress latency based AQM modes are supported:
 *  - classic AQM -ecn mark/drop by average egress latency
 *  - L4S AQM (Low Losses Low Latency Scalable) - ecn mark/drop by instantaneous egress latency
 *  - coupled-queue L4S AQM - mixture of L4S and classic modes - default is L4S has priority over Classic,
 *  with coupled mode configuration can be made that compensated the classic stream over the L4S;
 *  two voqs are maintained - L4S-queue(LQ) and Classic-queue(CQ)
 * 
 *  Calling sequence - configure TYPICAL mode
 * 1. create L4Q flow id gport - bcm_cosq_gport_add
 * 2. map (port+tc) to flow id gport - bcm_cosq_control_set - bcmCosqControlLatencyEgressAqmFlowId
 * 3. create AQM latency profile - bcm_cosq_latency_profile_create - flag BCM_COSQ_LATENCY_END_TO_END_AQM
 * 4. map L4Q flow id gport to latency profie - bcm_cosq_control_set - bcmCosqControlLatencyEgressAqmFlowIdToProfile
 * 5. configure profile properties
 * 
 * Calling sequence - configure COUPLED mode
 * 1.-4. - follow steps for L4S port
 * 5. set gpot latency profile to be Coupled - bcm_cosq_control_set - bcmCosqControlLatencyEgressCoupledAqmMode
 * 6. configure profile properties
 * 
 * Valid cosq  controls for END_TO_END_AQM latency: 
 * 
 * Profile configuration: 
 * 
 * bcmCosqControlLatencyDropEnable 
 * bcmCosqControlLatencyDropThreshold 
 * 
 * bcmCosqControlLatencyEcnEnable 
 * bcmCosqControlLatencyEcnProbEnable
 * bcmCosqControlLatencyEcnThreshold
 * bcmCosqControlLatencyEcnProbBaseThreshold
 * 
 *  bcmCosqControlLatencyEgressAqmFlowId
 *  bcmCosqControlLatencyEgressAqmFlowIdToProfile
  
 *  bcmCosqControlLatencyEgressCoupledAqmMode
  
 *  bcmCosqControlLatencyEgressAqmL4sEcnClassificationThresholdMax
  
 * bcmCosqControlLatencyEgressAqmAverageWeight
  
 *  General configuration: 
 * 
 * bcmCosqControlLatencyEcnProbConvertExponent
 * bcmCosqControlLatencyEcnProbConvertCoeff 
 */

/** latency profile gport */
bcm_gport_t aqm_flow_id_gport, aqm_latency_gport;

/**
* \brief - function to create aqm flow id gport, map flow id to 
*        pot and tc, create aqm latency profile and map it to
*        the flow id
*/
int end_to_end_aqm_latency_profile_create(
    int unit,
    int tc,
    int out_port)
{
    int rv = BCM_E_NONE;
    int bin_idx, profile_idx;
    bcm_gport_t port;
    int feature_end_to_end_aqm_support = *dnxc_data_get(unit, "latency", "features", "valid_end_to_end_aqm_profile", NULL);
    
    if (!feature_end_to_end_aqm_support)
    {  
        printf("Error in end_to_end_aqm_latency_l4s_port_example configuration - E2E AQM profile not supported on device.\n");
        return BCM_E_FAIL;

    }
    
    /** create L4Q flow id gport - if user want to set id use
     *  macro BCM_GPORT_LATENCY_AQM_FLOW_ID_SET(flow_id) */
    /** for bcm_cosq_gport_add with flag
     *  BCM_COSQ_GPORT_LATENCY_AQM_FLOW_ID - parameter port is
     *  not relevant */
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_add(unit, port, 0, BCM_COSQ_GPORT_LATENCY_AQM_FLOW_ID, &aqm_flow_id_gport));
    /** map (egress port + tc) to aqm flow id */
    BCM_GPORT_LOCAL_SET(port,out_port);
    BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,port,tc,bcmCosqControlLatencyEgressAqmFlowId, BCM_GPORT_LATENCY_AQM_FLOW_ID_GET(aqm_flow_id_gport)));
    /** create a latency profile */
    BCM_IF_ERROR_RETURN(bcm_cosq_latency_profile_create(unit, BCM_COSQ_LATENCY_END_TO_END_AQM, &aqm_latency_gport));
    /** map L4Q flow id gport to latency profile */
    BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,aqm_flow_id_gport,-1,bcmCosqControlLatencyEgressAqmFlowIdToProfile, BCM_GPORT_LATENCY_END_TO_END_AQM_LATENCY_PROFILE_GET(aqm_latency_gport)));
  
    return rv;
}

/**
* \brief - function used to configure the latency profile
*        attributes
*/
int end_to_end_aqm_latency_profile_configuration(
    int unit,
    int is_coupled,
    int l4s_ecn_classification_threshold_max,
    int ecn_prob_en,
    int ecn_en,
    int ecn_prob_base_threshold,
    int ecn_prob_max_threshold,
    int average_weight,
    int drop_en,
    int drop_thresh)
{
    int rv = BCM_E_NONE;
    int bin_idx, profile_idx;
    bcm_gport_t port;
    int feature_end_to_end_aqm_support = 
        *dnxc_data_get(unit, "latency", "features", "valid_end_to_end_aqm_profile", NULL);
    int feature_coupled_mode_support = *dnxc_data_get(unit, "latency", "features", "coupled_mode_support", NULL);
    
    if (!feature_end_to_end_aqm_support)
    {  
        printf
        ("Error in end_to_end_aqm_latency_l4s_port_example configuration - E2E AQM profile not supported on device.\n");
        return BCM_E_FAIL;
    }
    
    if(!feature_coupled_mode_support && is_coupled)
    {
        printf("Error in coupled_mode configuration - coupled mode is not supported.\n");
        return BCM_E_FAIL;
    }

    if(feature_coupled_mode_support)
    {
        /** configure coupled or single mode */
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set
        (unit,aqm_latency_gport,0,bcmCosqControlLatencyEgressCoupledAqmMode, is_coupled));
    }

    if (is_coupled)
    {

        /** configure ECN L4S max threshold -  in which packet will be
         *  defined as L4S - relevant only for coupled mode */
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set
        (unit,aqm_latency_gport,0,bcmCosqControlLatencyEgressAqmL4sEcnClassificationThresholdMax, 
        l4s_ecn_classification_threshold_max));
    }

    /** probabilistic mechanism configuration - ecn mark */
    BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,aqm_latency_gport,0,bcmCosqControlLatencyEcnEnable, ecn_en));

    BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,aqm_latency_gport,0,bcmCosqControlLatencyEcnProbEnable, ecn_prob_en));

    if (ecn_prob_en)
    {

        BCM_IF_ERROR_RETURN(bcm_cosq_control_set
        (unit,aqm_latency_gport,0,bcmCosqControlLatencyEcnProbBaseThreshold, ecn_prob_base_threshold));
    }
    BCM_IF_ERROR_RETURN(bcm_cosq_control_set
    (unit,aqm_latency_gport,0,bcmCosqControlLatencyEcnThreshold, ecn_prob_max_threshold));

    /** define the weight of the current latency sample, in order
     *  to calculate average latency */
    BCM_IF_ERROR_RETURN(bcm_cosq_control_set
    (unit,aqm_latency_gport,0,bcmCosqControlLatencyEgressAqmAverageWeight, average_weight));

    /** legacy mode - drop enable and threshold */
    BCM_IF_ERROR_RETURN(bcm_cosq_control_set(unit,aqm_latency_gport,0,bcmCosqControlLatencyDropEnable, drop_en));
    if (drop_en)
    {
        BCM_IF_ERROR_RETURN(bcm_cosq_control_set
        (unit,aqm_latency_gport,0,bcmCosqControlLatencyDropThreshold, drop_thresh));
    }
  
    return rv;
}

bcm_field_group_t lat_fg_id;
bcm_field_entry_t lat_ent_id;

int
ftmh_aqm_profile_acl_rule_set(
    int unit,
    bcm_field_context_t context,
    bcm_field_qualify_t qual_type,
    int qual_mask,
    int qual_data,
    int is_packet_L4S /** Low Latency Low Loss Scalable */
    )
{
    int rv  = BCM_E_NONE;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_gport_t gport;
    void *dest_char;
            
    /** Create and attach first group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /** Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = qual_type;
    /** Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionEcnNew;
    rv = bcm_field_group_add(unit, 0, &fg_info, &lat_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }        

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    rv = bcm_field_group_context_attach(unit, 0, lat_fg_id, context, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }
    
    /* Add entry */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = qual_data;
    ent_info.entry_qual[0].mask[0] = qual_mask;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    /** {aqm-profile[1](is_packet_l4S),ecn-value,aqm-profile[0](ecn-en)} */
    /** ecn-value should be zero in this cint, */
    /** because we want that latency mechanism will mark cni and not ingress pipe */
    ent_info.entry_action[0].value[0] = is_packet_L4S << 2;

    rv = bcm_field_entry_add(unit, 0, lat_fg_id, &ent_info, &lat_ent_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    printf("Entry add: id:(0x%x) ingress latency: flow decode (=%d)\n", lat_fg_id ,ent_info.entry_action[0].value[0]);

    return rv;
}


