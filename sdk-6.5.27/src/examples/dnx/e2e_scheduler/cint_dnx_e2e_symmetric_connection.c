/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~E2E symmetric connection~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_e2e_symmetric_connection.c
 * Purpose: Demonstrate how to create a simple E2E scheme of 2 symmetric connections
 *
 * You can modify the script to do the setup and tear down for various scheduling elements.
 * The example illustrates how to install create a simple scheme when the remotre device has 2 symmetric cores
 *
 * CINT usage:
 *  Create a voq connector (bcm_cosq_voq_connector_gport_add) that supports 2 cores, then attach the connector to a voq (bcm_cosq_connection_set()).
 *
 * Make sure that dtm_flow_nof_remote_cores_region_<id>=2 for desired regions, otherwise bcm_cosq_voq_connector_gport_add will fail since it won't
 * be able to allocate a voq connector that suits 2 symmetric connections
 *
 *  Assumed soc configuration:
 *  dtm_flow_nof_remote_cores_region_<id>=2
 *  diag_cosq_disable=1
 *  port_priorities=8
 *
 */


int
cint_e2e_symmetric_connection_set(int unit, soc_port_t port, uint32 nof_remote_cores)
{
    bcm_gport_t  local_gport, voq_connector_gport, e2e_gport;
    bcm_cosq_voq_connector_gport_t voq_connector;
    bcm_module_t my_modid;
    int cos;
    uint32 dummy_flags;
    bcm_cosq_gport_info_t gport_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    int modid_count, actual_modid_count;
    bcm_cosq_scheduler_gport_params_t params;

    BCM_GPORT_LOCAL_SET(local_gport, port); /* local */
    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, port);  /* e2e */

    /* change HRs mode */
    gport_info.in_gport = e2e_gport;
    gport_info.cosq = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info), "");
    e2e_gport = gport_info.out_gport;
    params.type = bcmCosqSchedulerGportTypeHrEnhanced;
    params.mode = bcmCosqSchedulerGportModeSimple;
    params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;
    params.core = mapping_info.core;
    params.shared_order = bcmCosqSchedulerGportSharedOrderNone;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_add(unit, BCM_COSQ_SCHEDULER_GPORT_REPLACE, params, &e2e_gport), "");

    /* create VOQ connectors */

    BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_count(unit, &modid_count), "");
    bcm_stk_modid_config_t modid_array[modid_count];

    BCM_IF_ERROR_RETURN_MSG(bcm_stk_modid_config_get_all(unit, modid_count, modid_array, &actual_modid_count), "");
    my_modid = modid_array[0].modid;

    voq_connector.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR;
    voq_connector.nof_remote_cores = nof_remote_cores;
    voq_connector.numq = 8;
    voq_connector.port = local_gport;
    voq_connector.remote_modid = my_modid;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_voq_connector_gport_add(unit, voq_connector, &voq_connector_gport), "");

    for (cos = 0; cos < 8; cos++) {
        /* Each VOQ connector attach suitable HR */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(unit, voq_connector_gport, cos, BCM_COSQ_SP0,0), "");

        BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_info.in_gport, local_gport);
        gport_info.cosq = cos;

        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info), "");

        /* attach HR SPi to connecter */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(unit, gport_info.out_gport, voq_connector_gport, cos), "");
    }

    return BCM_E_NONE;
}

