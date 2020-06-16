/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~E2E symmetric connection~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_e2e_symmetric_connection.c
 * Purpose: Demonstrate how to create a simple E2E scheme of 2 symmetric connections 
 *
 * You can modify the script to do the setup and tear down for various scheduling elements. 
 * The example illustrates how to install create a simple scheme when the remotre device has 2 symmetric cores (IE Jericho) 
 *
 * CINT usage:
 *  Create a voq connector (bcm_cosq_voq_connector_gport_add) that supports 2 cores, then attach the connector to a voq (bcm_cosq_connection_set()).
 *
 * Make sure that dtm_flow_nof_remote_cores_region_<id>=2 for desired regions, otherwise bcm_cosq_voq_connector_gport_add will fail since it won't
 * be able to allocate a voq connector that suits 2 symmetric connections
 * 
 * 
 *  Assumed soc configuration:
 *  dtm_flow_nof_remote_cores_region_<id>=2
 *  diag_cosq_disable=1
 *  port_priorities=8
 */


int
cint_e2e_symmetric_connection_set(int unit, soc_port_t port, uint32 nof_remote_cores)
{
    bcm_gport_t  local_gport, voq_connector_gport, e2e_gport;
    bcm_cosq_voq_connector_gport_t voq_connector;
    bcm_module_t my_modid;
    int rv, cos;
    uint32 flags, dummy_flags;
    bcm_cosq_gport_info_t gport_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    int modid_count, actual_modid_count;
    int is_dnx;


    flags = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED | BCM_COSQ_GPORT_REPLACE);
    BCM_GPORT_LOCAL_SET(local_gport, port); /* local */
    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, port);  /* e2e */     

    /* change HRs mode */     
    rv = bcm_cosq_gport_add(unit, local_gport, 1, flags, &e2e_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_cosq_gport_add\n");
        return rv;
    }

    /* create VOQ connectors */
    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv)) {
        printf("bcm_device_member_get failed");
        return(rv);
    }

    if (is_dnx) {
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
        my_modid = modid_array[0].modid;
    }
    else {
        rv = bcm_stk_my_modid_get(unit, &my_modid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_petra_stk_my_modid_get\n");
            return rv;
        }
    }

    voq_connector.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR;
    voq_connector.nof_remote_cores = nof_remote_cores;
    voq_connector.numq = 8;
    voq_connector.port = local_gport;
    voq_connector.remote_modid = my_modid;

    rv = bcm_cosq_voq_connector_gport_add(unit, voq_connector, &voq_connector_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_cosq_voq_connector_gport_add\n");
        return rv;
    }

    for (cos = 0; cos < 8; cos++) {
        /* Each VOQ connector attach suitable HR */
        rv = bcm_cosq_gport_sched_set(unit, voq_connector_gport, cos, BCM_COSQ_SP0,0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_cosq_gport_sched_set\n");
            return rv;
        }

        BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_info.in_gport, local_gport);
        gport_info.cosq = cos;

        rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);     
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_cosq_gport_handle_get\n");
            return rv;
        }
          
        /* attach HR SPi to connecter */
        rv = bcm_cosq_gport_attach(unit, gport_info.out_gport, voq_connector_gport, cos);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_cosq_gport_attach\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}



