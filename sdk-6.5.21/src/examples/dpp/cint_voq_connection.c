/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  This cint contains basic functions to create and connect voq / voq connector for a single port 
 */

/* -------------------------------------------------------------------------------------------------------------------------------*/
/*                                              
   Global Variables

   if a different value is required, can be modified before calling the cint function

*/

int nof_remote_connections = 1; 
int default_delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED;

/* -------------------------------------------------------------------------------------------------------------------------------*/
/*!
 * \brief
 *  Create VOQ connector for a given port.
 *
 * \par DIRECT INPUT:
 *  \param [in] unit -
 *      The unit id.
 *  \param [in] remote_modid -
 *      modid of ingress device
 *  \param [in] port -
 *      local port number on (local) egress  device
 *  \param [in] numq -
 *      Number of queues in the voq bundle
 *  \param [in] voq_connector_id - 
 *      id of voq connector to be created
 *  \param [out] voq_connector_gport - 
 *      gport of the created voq connector
 */
int create_voq_connector(int unit, int remote_modid, int port, int numq, int voq_connector_id, bcm_gport_t* voq_connector_gport)
{

    int core_id;
    int rv = BCM_E_NONE;
    uint32      flags, dummy_flags;

    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;

    bcm_cosq_voq_connector_gport_t config;
    bcm_cosq_gport_connection_t connection;


    printf ("voq_connector_id = %d\n", voq_connector_id);
    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_get\n");
        return rv;
    }

    /* create voq connector */
    flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | BCM_COSQ_GPORT_WITH_ID;
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(*voq_connector_gport, voq_connector_id, mapping_info.core);
    
    config.flags = flags;
    config.nof_remote_cores = nof_remote_connections;
    config.numq = numq;
    config.remote_modid = remote_modid;
    BCM_COSQ_GPORT_E2E_PORT_SET(config.port, port);    
    
    printf ("Call bcm_cosq_voq_connector_gport_add for port %d\n", port);
    rv = bcm_cosq_voq_connector_gport_add(unit, &config, voq_connector_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_voq_connector_gport_add\n");
        return rv;
    }

    return rv;
}


/*!
 * \brief
 *  Connect VOQ connector to VOQ
 *
 * \par DIRECT INPUT:
 *  \param [in] unit -
 *      The unit id.
 *  \param [in] remote_modid -
 *      modid of ingress device
 *  \param [in] voq_id - 
 *      id of voq  to be connected to
 *  \param [in] voq_connector_gport - 
 *      gport of voq connector to be connected
 */
int connect_voq_connector(int unit, int remote_modid, int ingress_core, int voq_id, bcm_gport_t voq_connector_gport)
{

    int rv;
    bcm_cosq_gport_connection_t connection;
    int core_id;

    /* Egress: connect voq connectors to voqs */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.voq_connector = voq_connector_gport;
    
    connection.remote_modid = (ingress_core == BCM_CORE_ALL ? 0 : ingress_core);
    connection.remote_modid += remote_modid;
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(connection.voq, ingress_core, voq_id);

    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_connection_set\n");
        return rv;
    }

    return 0;

}

int connect_voq_connector_to_sched_scheme(int unit, int num_cos, bcm_gport_t sch_elem, bcm_gport_t voq_connector_gport)
{

    int rv;
    int cos;

    /* Attach VOQ connector to sched scheme*/
    for(cos = 0; cos < num_cos; cos++) {
        rv = bcm_cosq_gport_sched_set(unit, voq_connector_gport, cos, BCM_COSQ_SP0, 0);
        if (BCM_FAILURE(rv)) {
            printf("bcm_cosq_gport_sched_set failed, voq_connector = 0x%x, cosq = %d\n", voq_connector_gport, cos);
            return rv;
        }
        /* attach HR SPi to connecter */
        
        rv = bcm_cosq_gport_attach(unit, sch_elem, voq_connector_gport, cos);
        if (BCM_FAILURE(rv)) {
            printf("bcm_cosq_gport_attach failed, sch_root = 0x%x, voq_connector = 0x%x, cosq = %d\n", sch_root, voq_con, cos);
            return rv;
        }
    }

    return 0;

}

int create_and_connect_voq_connector(int unit, int remote_modid, int ingress_core, int port, int numq, int voq_id, int voq_connector_id)
{

    int rv;
    bcm_gport_t voq_connector_gport;

    bcm_gport_t e2e_port;

    rv = create_voq_connector(unit, remote_modid, port, numq, voq_connector_id, &voq_connector_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, create_voq_connector\n");
        return rv;
    }

    rv = connect_voq_connector(unit, remote_modid, ingress_core, voq_id, voq_connector_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, connect_voq_connector\n");
        return rv;
    }

    /* use existing HR for the port */
    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_port, port);
    rv = connect_voq_connector_to_sched_scheme( unit, numq /* num_cos ? */, e2e_port, voq_connector_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, connect_voq_connector_to_sched_scheme\n");
        return rv;
    }

    return BCM_E_NONE;

}

/*!
 * \brief
 *  Create VOQ for a given port.
 *  VOQ is always created symmetrically on both cores.
 *
 * \par DIRECT INPUT:
 *  \param [in] unit -
 *      The unit id.
 *  \param [in] remote_base_modid -
 *      base modid of egress device
 *  \param [in] port -
 *      local port number on egress  device
 *  \param [in] numq -
 *      Number of queues in the voq bundle
 *  \param [in] additional_flags - 
 *      additional flags  to be provided to bcm_cosq_ingress_queue_bundle_gport_add
 *  \param [in] voq_id - 
 *      id of voq to be created
 *  \param [out] voq_gport - 
 *      gport of the created voq
 */
int create_voq(int unit,  int remote_base_modid, int port, int numq, uint32 additional_flags, bcm_gport_t* voq_gport)
{
    int core_id;
    int rv;
    int nof_cores;
    int cosq;
    uint32                    flags, dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;
    bcm_cosq_ingress_queue_bundle_gport_config_t ingress_config;
    int tm_port;

    bcm_gport_t sysport_gport;
    bcm_gport_t modport_gport;

    /* assume remote port is configured the same way as local port with the same number */
    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_get\n");
        return rv;
    }

    tm_port =  mapping_info.tm_port;
    BCM_GPORT_MODPORT_SET(modport_gport, remote_base_modid + mapping_info.core, tm_port);
    rv = bcm_stk_gport_sysport_get(unit, modport_gport, &sysport_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_gport_sysport_get\n");
        return rv;
    }

    flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | additional_flags;
    ingress_config.flags = flags;
    ingress_config.port = modport_gport;
    ingress_config.local_core_id = BCM_CORE_ALL;
    ingress_config.numq = numq;
    for (cosq = 0 ; cosq < BCM_COS_COUNT; cosq++) {
        /* setting all of the credit request profiles to default slow enabled mode */
        ingress_config.queue_atrributes[cosq].delay_tolerance_level = default_delay_tolerance_level;
        /* setting all of the rate classes zero */
        ingress_config.queue_atrributes[cosq].rate_class = 0;
    }

    /* Map the port with the credit request profile */
    rv = bcm_cosq_ingress_queue_bundle_gport_add(unit, &ingress_config, voq_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_ingress_queue_bundle_gport_add\n");
        return rv;
    }

    return BCM_E_NONE;
}


/*!
 * \brief
 *  Connect VOQ to VOQ connector.
 *
 * \par DIRECT INPUT:
 *  \param [in] unit -
 *      The unit id.
 *  \param [in] egress_core -
 *      core id on egress device
 *  \param [in] voq_gport - 
 *      gport of voq to be connected
 *  \param [in] voq_connector_id - 
 *      id of voq connector to connect to
 */
int connect_voq(int unit, int egress_core, bcm_gport_t voq_gport, int voq_connector_id)
{
    int rv;
    bcm_cosq_gport_connection_t connection;

    /* Ingress: connect voqs to voq connectors */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    
    connection.voq = voq_gport;
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connection.voq_connector, voq_connector_id, egress_core);
    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_connection_set\n");
        return rv;
    }
    return BCM_E_NONE;
}

/*!
 * \brief
 *  Create VOQ for a given port and connect it to VOQ connector.
 *  VOQ is always created and connected symmetrically on both cores.
 *
 * \par DIRECT INPUT:
 *  \param [in] unit -
 *      The unit id.
 *  \param [in] remote_base_modid -
 *      base modid of egress device
 *  \param [in] port -
 *      local port number on egress  device
 *  \param [in] numq -
 *      Number of queues in the voq bundle
 *  \param [in] voq_id - 
 *      id of voq to be created
 *  \param [in] voq_connector_id - 
 *      id of voq connector to connect to
 */
int create_and_connect_voq(int unit, int remote_base_modid, int port, int numq, bcm_gport_t *voq_gport, int voq_connector_id, uint32 additional_flags)
{
    int rv;
    uint32                    dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t   mapping_info;

    /* assume remote port is configured the same way as local port with the same number */
    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_get\n");
        return rv;
    }

    rv = create_voq(unit, remote_base_modid, port, numq, additional_flags, voq_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, create_voq\n");
        return rv;
    }
    
    rv = connect_voq(unit,  mapping_info.core, *voq_gport, voq_connector_id);
    if (rv != BCM_E_NONE) {
        printf("Error, connect_voq\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*!
 * \brief
 *  example which creates and connects voq and voq connector for the given local port 
 *  both on ingress and egress side 
 *
 * \par DIRECT INPUT:
 *  \param [in] unit -
 *      The unit id.
 *  \param [in] port -
 *      local port number 
 *  \param [in] numq -
 *      Number of queues in the voq bundle
 *  \param [in] voq_id - 
 *      id of voq to be created
 *  \param [in] voq_connector_id - 
 *      id of voq connector to be created
 */
int create_and_connect_local_voq_and_connector_example(int unit, int port, int numq, bcm_gport_t *voq_gport, int voq_connector_id, uint32 additional_flags) 
{

    int rv = BCM_E_NONE;
    int my_modid = 0, nof_cores = 0;
    int voq_id = 0;

    rv = bcm_stk_my_modid_get(unit, &my_modid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_my_modid_get\n");
        return rv;
    }

    rv = bcm_stk_modid_count(unit, &nof_cores);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_modid_count\n");
        return rv;
    }
    nof_remote_connections = 1;

    rv = create_and_connect_voq(unit, my_modid, port, numq, voq_gport, voq_connector_id, additional_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, create_and_connect_voq\n");
        return rv;
    }

    voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(voq_gport);
    rv = create_and_connect_voq_connector(unit, my_modid, BCM_CORE_ALL, port, numq, voq_id, voq_connector_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create_and_connect_voq_connector\n");
        return rv;
    }

    return rv;

}

