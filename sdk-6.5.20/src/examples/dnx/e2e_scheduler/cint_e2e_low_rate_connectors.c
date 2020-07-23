/*
  *
  * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
  * 
  * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This cint configures simple scheduling scheme using low rate connectors
 * 
 * Test Scenario:
 * 1) Configure 8 streams with different TCs.
 * 1) Build simple scheduling scheme.
 * 2) Configure snake from the port to itself, and expect traffic to get back according to the shaper.
 */
/*
 * Params:
 *  unit - 
 *  dest_port   - destination port to build the scheduling scheme.
 *  flow_bw      - bandwidth of the connector (in kbps).
 */

int cint_e2e_low_rate_start_run(int unit, bcm_port_t dest_port, int flow_bw) {
    int rv = BCM_E_NONE; 
    int cos, num_cos = 8;
    bcm_gport_t queue;

    bcm_gport_t voq_con;
    bcm_cosq_voq_connector_gport_t config;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 port_flags;

    int credit_request_profile_id = 20;
    int low_rate_region_start = 20;
    int low_rate_region_end = 30;

    rv = bcm_port_get(unit, dest_port, &port_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_get, $rv \n");
        return rv;
    }

    int credit_worth = *(dnxc_data_get(unit, "iqs", "credit", "worth", NULL));
    uint32 max_burst = 4*credit_worth;

    /***********************************/
    /*          VOQ                    */
    /***********************************/

    /* Create E2E port */
    bcm_gport_t e2e_port;
    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_port, dest_port);
    
    /* Get modid to use in VOQ connector creation */
    int modid;

    int modid_count, actual_modid_count;
    rv = bcm_stk_modid_count(unit, &modid_count);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stk_modid_count\n");
        return rv;
    }
    bcm_stk_modid_config_t modid_array[modid_count];
    
    rv = bcm_stk_modid_config_get_all(unit, modid_count, modid_array, &actual_modid_count);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stk_modid_config_get_all\n");
        return rv;
    }
    modid = modid_array[0].modid;

    bcm_gport_t sys_port;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, dest_port);

    bcm_cosq_delay_tolerance_t credit_request_profile;
    bcm_cosq_delay_tolerance_preset_attr_t credit_request_attr;
    bcm_cosq_ingress_queue_bundle_gport_config_t conf;
    uint32 credit_size;

    bcm_cosq_delay_tolerance_preset_attr_t_init(&credit_request_attr);
    bcm_cosq_dest_credit_size_get(unit, modid, &credit_size);
    credit_request_attr.credit_size = credit_size;
    credit_request_attr.flags = 0;
    credit_request_attr.rate = 10; /* 10G */
    rv = bcm_cosq_delay_tolerance_preset_get(unit, &credit_request_attr, &credit_request_profile);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_delay_tolerance_preset_get failed\n");
        return rv;
    }

    /** set the threshold */
    rv = bcm_cosq_delay_tolerance_level_set(unit, credit_request_profile_id, &credit_request_profile);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_delay_tolerance_level_set failed\n");
        return rv;
    }

    /* Create VOQ */
    conf.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
    conf.port = sys_port;
    conf.numq = num_cos;
    conf.local_core_id = BCM_CORE_ALL;
    for (cos = 0; cos < num_cos; ++cos) {
        conf.queue_atrributes[cos].delay_tolerance_level = credit_request_profile_id;
        conf.queue_atrributes[cos].rate_class = 0;
    }
    rv = bcm_cosq_ingress_queue_bundle_gport_add(unit, &conf, &queue);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_ingress_queue_bundle_gport_add failed, VOQ = 0x%x", queue);
        return rv;
    }

 
    /***********************************/
    /*          VOQ CONNECTOR          */
    /***********************************/

    bcm_cosq_range_t range;
    range.range_start = low_rate_region_start;
    range.range_end = low_rate_region_end;
    range.is_enabled = TRUE;

    rv = bcm_cosq_control_range_set(unit, 0, 0, bcmCosqRangeLowRateVoqConnector, &range);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_control_range_set failed, unit = %d\n", unit);
        return rv;
    }


    /* Create VOQ connector */
    config.port = e2e_port;
    config.flags = BCM_COSQ_VOQ_CONNECTOR_GPORT_F_LOW_RATE;
    config.numq = num_cos;
    config.remote_modid = modid;
    config.nof_remote_cores = 2;
    rv = bcm_cosq_voq_connector_gport_add(unit, config, &voq_con);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_voq_connector_gport_add failed, unit = %d\n", unit);
        return rv;
    }
    
    /* Attach VOQ connector to sched scheme*/
    for(cos = 0; cos < num_cos; cos++) {
        rv = bcm_cosq_gport_sched_set(unit, voq_con, cos, BCM_COSQ_SP0, 0);
        if (BCM_FAILURE(rv)) {
            printf("bcm_cosq_gport_sched_set failed, voq_connector = 0x%x, cosq = %d\n", voq_con, cos);
            return rv;
        }
        rv = bcm_cosq_gport_attach(unit, e2e_port, voq_con, cos);
        if (BCM_FAILURE(rv)) {
            printf("bcm_cosq_gport_attach failed, cl_element = 0x%x, voq_connector = 0x%x, cosq = %d\n", cl_element, voq_con, cos);
            return rv;
        }
        /* Set connector BW */
        rv = bcm_cosq_gport_bandwidth_set(unit, voq_con, cos, 0, flow_bw, 0);
        if (BCM_FAILURE(rv)) {
            printf("bcm_cosq_gport_bandwidth_set failed, bw = %d\n", flow_bw);
            return rv;
        }

        rv = bcm_cosq_control_set(unit, voq_con, cos, bcmCosqControlBandwidthBurstMax, max_burst);
        if (BCM_FAILURE(rv)) {
            printf("bcm_cosq_control_set bcmCosqControlBandwidthBurstMax failed, voq_con = 0x%x\n", voq_con);
            return rv;
        }
    }


    /* Connect between VOQs and VOQ-Connectors */
    bcm_cosq_gport_connection_t connection;
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = modid;
    connection.voq = queue;
    connection.voq_connector = voq_con;
    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_gport_connection_set BCM_COSQ_GPORT_CONNECTION_INGRESS failed, unit = %d\n", unit);
        return rv;
    }

    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (BCM_FAILURE(rv)) {
        printf("bcm_cosq_gport_connection_set BCM_COSQ_GPORT_CONNECTION_EGRESS failed, unit = %d\n", unit);
        return rv;
    }

    printf("cint_e2e_low_rate_start_run: PASS\n\n");
    return rv;
}

