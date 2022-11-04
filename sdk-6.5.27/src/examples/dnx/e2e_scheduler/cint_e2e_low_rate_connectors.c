/*
  *
  * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
  * 
  * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
    int cos, num_cos = 8;
    bcm_gport_t queue;

    bcm_gport_t voq_con;
    bcm_cosq_voq_connector_gport_t config;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 port_flags;
    int profile_is_rate_class = *dnxc_data_get(unit, "iqs", "credit", "profile_is_rate_class", NULL);
    int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
    int nof_remote_cores = nof_cores > 2 ? nof_cores : 2;

    int credit_request_profile_id = 20;
    int low_rate_region_start = 20;
    int low_rate_region_end = 30;
    char error_msg[200]={0,};

    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, dest_port, &port_flags, &interface_info, &mapping_info), "");

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
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_count(unit, &modid_count), "");
    bcm_stk_modid_config_t modid_array[modid_count];
    
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_config_get_all(unit, modid_count, modid_array, &actual_modid_count), "");
    modid = modid_array[0].modid;

    bcm_gport_t sys_port;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, dest_port);

    uint32 credit_size;
    bcm_cosq_dest_credit_size_get(unit, modid, &credit_size);

    if (profile_is_rate_class)
    {
        bcm_cosq_rate_class_create_info_t create_info;
        bcm_gport_t rate_class_gport;
        uint32 flags = 0;
        create_info.attributes = 0;
        create_info.credit_size = credit_size;
        create_info.rate = 10;
        BCM_GPORT_PROFILE_SET(rate_class_gport, credit_request_profile_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_rate_class_create(unit, rate_class_gport, flags, &create_info), "");
    }
    else
    {
        bcm_cosq_delay_tolerance_t credit_request_profile;
        bcm_cosq_delay_tolerance_preset_attr_t credit_request_attr;
        bcm_cosq_delay_tolerance_preset_attr_t_init(&credit_request_attr);

        credit_request_attr.credit_size = credit_size;
        credit_request_attr.flags = 0;
        credit_request_attr.rate = 10; /* 10G */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_delay_tolerance_preset_get(unit, &credit_request_attr, &credit_request_profile), "");

        /** set the threshold */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_delay_tolerance_level_set(unit, credit_request_profile_id, &credit_request_profile), "");
    }

    /* Create VOQ */
    bcm_cosq_ingress_queue_bundle_gport_config_t conf;
    conf.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
    conf.port = sys_port;
    conf.numq = num_cos;
    conf.local_core_id = BCM_CORE_ALL;
    for (cos = 0; cos < num_cos; ++cos) {
        if (profile_is_rate_class)
        {
            conf.queue_atrributes[cos].delay_tolerance_level = -1;
        }
        else
        {
            conf.queue_atrributes[cos].delay_tolerance_level = credit_request_profile_id;
        }
        conf.queue_atrributes[cos].rate_class = 0;
    }
    snprintf(error_msg, sizeof(error_msg), "VOQ = 0x%x", queue);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_queue_bundle_gport_add(unit, &conf, &queue), error_msg);

 
    /***********************************/
    /*          VOQ CONNECTOR          */
    /***********************************/

    bcm_cosq_range_t range;
    range.range_start = low_rate_region_start;
    range.range_end = low_rate_region_end;
    range.is_enabled = TRUE;
    int is_multi_low_rate_ranges = *dnxc_data_get(unit, "sch", "flow", "multi_low_rate_ranges", NULL);

    /* Create VOQ connector */
    config.port = e2e_port;
    config.numq = num_cos;
    config.remote_modid = modid;
    config.nof_remote_cores = nof_remote_cores;

    if (is_multi_low_rate_ranges == 0)
    {
        snprintf(error_msg, sizeof(error_msg), "unit = %d", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_range_set(unit, 0, 0, bcmCosqRangeLowRateVoqConnector, &range), error_msg);
        config.flags = BCM_COSQ_VOQ_CONNECTOR_GPORT_F_LOW_RATE;
    }
    else
    {
        int key = 0;
        int pattern = 0xffff;
        int region_size = *dnxc_data_get(unit, "sch", "flow", "region_size", NULL);
        int flow_id = low_rate_region_start * region_size;

        snprintf(error_msg, sizeof(error_msg), "unit = %d", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_generic_control_set(unit, 0, 0, key, bcmCosqGenericControlLowRatePattern, pattern), error_msg);
        snprintf(error_msg, sizeof(error_msg), "unit = %d", unit);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_range_set(unit, 0, 0, bcmCosqRangeLowRate_0, &range), error_msg);
        config.flags = BCM_COSQ_VOQ_CONNECTOR_GPORT_F_WITH_ID;
        BCM_COSQ_GPORT_VOQ_CONNECTOR_SET(voq_con, flow_id);
    }

    snprintf(error_msg, sizeof(error_msg), "unit = %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_voq_connector_gport_add(unit, config, &voq_con), error_msg);
    
    /* Attach VOQ connector to sched scheme*/
    for(cos = 0; cos < num_cos; cos++) {
        snprintf(error_msg, sizeof(error_msg), "voq_connector = 0x%x, cosq = %d", voq_con, cos);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit, voq_con, cos, BCM_COSQ_SP0, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "e2e_port = 0x%x, voq_connector = 0x%x, cosq = %d", e2e_port, voq_con, cos);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(unit, e2e_port, voq_con, cos), error_msg);
        /* Set connector BW */
        snprintf(error_msg, sizeof(error_msg), "bw = %d", flow_bw);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, voq_con, cos, 0, flow_bw, 0), error_msg);

        snprintf(error_msg, sizeof(error_msg), "bcmCosqControlBandwidthBurstMax failed, voq_con = 0x%x", voq_con);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(unit, voq_con, cos, bcmCosqControlBandwidthBurstMax, max_burst), error_msg);
    }


    /* Connect between VOQs and VOQ-Connectors */
    bcm_cosq_gport_connection_t connection;
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = modid;
    connection.voq = queue;
    connection.voq_connector = voq_con;
    snprintf(error_msg, sizeof(error_msg), "BCM_COSQ_GPORT_CONNECTION_INGRESS failed, unit = %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, &connection), error_msg);

    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    snprintf(error_msg, sizeof(error_msg), "BCM_COSQ_GPORT_CONNECTION_EGRESS failed, unit = %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, &connection), error_msg);

    printf("cint_e2e_low_rate_start_run: PASS\n\n");
    return BCM_E_NONE;
}

