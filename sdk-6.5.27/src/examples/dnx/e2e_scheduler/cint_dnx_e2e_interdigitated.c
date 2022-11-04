/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This cint configures simple scheduling scheme using interdigitated mode.
 * Interdigitated mode is configured by the flag BCM_COSQ_GPORT_NON_CONTIGUOUS_VOQ_CONNECTOR.
 *
 * Test Scenario:
 * 1) SoC property:
 *      diag_cosq_disable=1
 * 2) Configure 8 streams with different TCs.
 * 3) Build simple interdigitated scheduling scheme.
 * 4) Configure snake from the port to itself, and expect all traffic to get back.
 */
/*
 * Params:
 *  unit -
 *  dest_port   - destination port to build the scheduling scheme.
 *  q_num       - VOQ number for the destination port.
 *  e2e_bw      - bandwidth of the port (in kbps).
 */

int cint_e2e_interdigitated_start_run(int unit, bcm_port_t dest_port, int q_num, int e2e_bw) {
    int cos, num_cos = 8;
    bcm_gport_t queue;
    bcm_gport_t sys_port;
    bcm_cosq_ingress_queue_bundle_gport_config_t conf;

    bcm_gport_t cl_element;
    bcm_gport_t voq_con;
    bcm_cosq_voq_connector_gport_t config;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_resource_t resource;
    uint32 port_flags;
    int delay_tolerance_level, rate_class;
    bcm_cosq_scheduler_gport_params_t params;
    char error_msg[200]={0,};


    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(unit, dest_port, &port_flags, &interface_info, &mapping_info), "");

    uint32 max_burst = 3000;
    int credit_worth = *(dnxc_data_get(unit, "iqs", "credit", "worth", NULL));
    int legacy_credit_worth = 1024;
    int multiplier = (credit_worth / legacy_credit_worth);
    max_burst *= multiplier;

    /***********************************/
    /*          VOQ                    */
    /***********************************/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_get(unit, dest_port, &resource), "");

    BCM_IF_ERROR_RETURN_MSG(appl_dnx_e2e_voq_profiles_get(unit, resource.speed, &delay_tolerance_level, &rate_class), "");
    /* Create E2E port */
    bcm_gport_t e2e_port;
    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_port, dest_port);

    /* Set E2E BW */
    snprintf(error_msg, sizeof(error_msg), "bw = %d", e2e_bw);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, e2e_port, 0, 0, e2e_bw, 0), error_msg);

    /* Create dest gport */
    bcm_gport_t dest_gport;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_port, dest_port);
    snprintf(error_msg, sizeof(error_msg), "sys_port = %d", sys_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_sysport_gport_get(unit, sys_port, &dest_gport), error_msg); /*sysport to modport */
    /* Create interdigitated queue */
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(queue, BCM_CORE_ALL, q_num);

    conf.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_NON_CONTIGUOUS_VOQ_CONNECTOR | BCM_COSQ_GPORT_WITH_ID;
    conf.port = sys_port;
    conf.numq = num_cos;
    conf.local_core_id = BCM_CORE_ALL;
    for (cos = 0; cos < num_cos; cos++) {
        conf.queue_atrributes[cos].delay_tolerance_level = delay_tolerance_level;
        conf.queue_atrributes[cos].rate_class = rate_class;
    }
    snprintf(error_msg, sizeof(error_msg), "VOQ = 0x%x", queue);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_queue_bundle_gport_add(unit, &conf, &queue), error_msg);

    /***********************************/
    /*          VOQ CONNECTOR          */
    /***********************************/

    /* Create scheduler and connect it to e2e_port */
    params.type = bcmCosqSchedulerGportTypeClass4Sp;
    params.mode = bcmCosqSchedulerGportModeSimple;
    params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;
    params.core = mapping_info.core;
    params.shared_order = bcmCosqSchedulerGportSharedOrderNone;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_add(unit, 0, params, &cl_element), "");
    snprintf(error_msg, sizeof(error_msg), "cl_element = 0x%x", cl_element);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit, cl_element, 0, BCM_COSQ_SP1, 0), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(unit, e2e_port, cl_element, 0), error_msg);
    snprintf(error_msg, sizeof(error_msg), "cl_element = 0x%x, e2e_bw = %d", cl_element, e2e_bw);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(unit, cl_element, 0, 0, e2e_bw, 0), error_msg);

    snprintf(error_msg, sizeof(error_msg), "bcmCosqControlBandwidthBurstMax failed, cl_element = 0x%x, max_burst = %d", cl_element, max_burst);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(unit, cl_element, 0, bcmCosqControlBandwidthBurstMax, max_burst), error_msg);

    /* Get modid to use in VOQ connector creation */
    int modid;
    int modid_count, actual_modid_count;
    BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_count(unit, &modid_count), "");
    bcm_stk_modid_config_t modid_array[modid_count];

    BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_config_get_all(unit, modid_count, modid_array, &actual_modid_count), "");
    modid = modid_array[0].modid;


    /* Create VOQ connector */
    config.port = e2e_port;
    config.flags = BCM_COSQ_GPORT_NON_CONTIGUOUS_VOQ_CONNECTOR;
    config.numq = num_cos;
    config.remote_modid = modid + mapping_info.core;
    config.nof_remote_cores = 1;
    snprintf(error_msg, sizeof(error_msg), "unit = %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_voq_connector_gport_add(unit, config, &voq_con), error_msg);

    /* Attach VOQ connector to sched scheme*/
    for(cos = 0; cos < num_cos; cos++) {
        snprintf(error_msg, sizeof(error_msg), "voq_connector = 0x%x, cosq = %d", voq_con, cos);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit, voq_con, cos, BCM_COSQ_SP0, 0), error_msg);
        snprintf(error_msg, sizeof(error_msg), "cl_element = 0x%x, voq_connector = 0x%x, cosq = %d", cl_element, voq_con, cos);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(unit, cl_element, voq_con, cos), error_msg);
    }

    /*bcm_cosq_ingress_queue_bundle_gport_add will a voq gport with core - BCM_CORE_ALL.
      In order to be consistent with the asymmetric scheme to be created
      the core is overwritten.*/
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(queue, mapping_info.core, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(queue));

    /* Connect between VOQs and VOQ-Connectors */
    bcm_cosq_gport_connection_t connection;
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = modid + mapping_info.core;
    connection.voq = queue;
    connection.voq_connector = voq_con;
    snprintf(error_msg, sizeof(error_msg), "BCM_COSQ_GPORT_CONNECTION_INGRESS failed, unit = %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, &connection), error_msg);

    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    snprintf(error_msg, sizeof(error_msg), "BCM_COSQ_GPORT_CONNECTION_EGRESS failed, unit = %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(unit, &connection), error_msg);

    printf("cint_e2e_interdigitated_start_run: PASS\n\n");
    return BCM_E_NONE;
}

