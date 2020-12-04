/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE configure FIFOs example:
 *  
 * This cint demonstrates how to use the CPU2CPU packets mechanism available in the fe3200:
 * start function will register a callback function , and start the main thread for rx reception
 * stop function will stop the thread, and unregister the callback function
 */

/* Callback function - prints packet information */
bcm_rx_t cint_cpu_pkts_callback_function(int unit, bcm_pkt_t *pkt, void *cookie)
{
    int i;
    printf("\nPacket details:\n");
    printf("Source device: %d\n", pkt->src_mod);
    printf("Packet unit: %d\n", pkt->unit);
    printf("Packet pipe index : %d\n", pkt->cos);
    printf("Packet length : %d\n", pkt->pkt_data->len);
    printf("Packet payload : \n");

    for (i=((pkt->pkt_data->len-1)/4)*4 ; i>=0 ; i -= 4)
    {
        printf(" 0x%02x%02x%02x%02x",
               i+3 < pkt->pkt_data->len ? pkt->pkt_data->data[i+3] & 0xFF:0,
               i+2 < pkt->pkt_data->len ? pkt->pkt_data->data[i+2] & 0xFF :0,
               i+1 < pkt->pkt_data->len ? pkt->pkt_data->data[i+1] & 0xFF :0,
               pkt->pkt_data->data[i] & 0xFF);
    }

    return BCM_RX_HANDLED;
}

int cint_cpu_pkts_config_all_reachable_value(int unit, int max_all_reachable_value)
{
    int rv;

    /* Configuring AlrcMaxBaseIndex */
    rv = bcm_stk_module_max_set(unit, BCM_STK_MODULE_MAX_ALL_REACHABLE, max_all_reachable_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_stk_module_max_set\n");
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

int cint_cpu_pkts_reception_start(int unit, int fe_id, int is_default_callback_function, bcm_rx_cb_f callback_function)
{
    int rv;
    int cpu_address[1] ;
    int  update_base_index;
    bcm_rx_cfg_t rx_configuration;

    cpu_address[0] = fe_id;

    /* Config max all reachable id , should be lower than the fe id*/
    if (fe_id > 0)
    {
        rv = cint_cpu_pkts_config_all_reachable_value(unit, (fe_id/32)*32 /*Should be divisible by 32*/);
        if (rv != BCM_E_NONE)
        {
            printf("Error cint_dfe_cpu_packets - cint_cpu_pkts_config_all_reachable_value");
            return BCM_E_FAIL;
        }
    }
    if (is_default_callback_function) /* using a callback function which prints the packet info */
    {
        callback_function = &cint_cpu_pkts_callback_function;
    }
    else
    {
        /*if (callback_function) == NULL)
        {
            printf("Error in cint_dfe_cpu_packets - when using a custom callback function, it must be defined");
            return BCM_E_FAIL;
        }*/
    }
    /* Register a callback function */
    rv = bcm_rx_register(unit, "cint_dfe_cpu_packets_callback_function", callback_function, 100, NULL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_rx_register \n");
        return BCM_E_FAIL;
    }

    /* Number of mod ids to be configured, should be between 0 and 1 */
    rx_configuration.num_of_cpu_addresses = 1;
    /* Mod ids to be assigned to the cpu buffers , should be in the interval [AlrcMaxBaseIndex, UpdateBaseIndex]*/ 
    rx_configuration.cpu_address = cpu_address; 

    /*bcm_rx_start/stop should not be called when traffic is enabled, because it writes to global ECI registers, which can cause undefined behavior under traffic*/
    rv = bcm_fabric_control_set(unit, bcmFabricIsolate, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_fabric_control_set \n");
        return BCM_E_FAIL;
    }

    /* Start the packets reception thread */
    rv = bcm_rx_start(unit, &rx_configuration);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_rx_start \n");
        return BCM_E_FAIL;
    }

    rv = bcm_fabric_control_set(unit, bcmFabricIsolate, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_fabric_control_set \n");
        return BCM_E_FAIL;
    }

    printf("cint_dfe_cpu_packets - cint_cpu_pkts_reception_start finished successfully \n");
    return BCM_E_NONE;
}

int cint_cpu_pkts_reception_stop(int unit)
{
    int rv;
    bcm_rx_cfg_t rx_configuration;

    /* Configuring the rx configuration structure */

    /* Configuring num of cpu addresses = 0 , in order to un-assign the mod ids assigned to the cpu buffers */
    rx_configuration.num_of_cpu_addresses = 0; 

    /*bcm_rx_start/stop should not be called when traffic is enabled, because it writes to global ECI registers, which can cause undefined behavior under traffic*/
    rv = bcm_fabric_control_set(unit, bcmFabricIsolate, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_fabric_control_set \n");
        return BCM_E_FAIL;
    }

    /* Stop the packets reception thread, and un-assign the cpu buffer ids */
    rv = bcm_rx_stop(unit, &rx_configuration);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_rx_stop \n");
        return BCM_E_FAIL;
    }

    /*bcm_rx_start/stop should not be called when traffic is enabled, because it writes to global ECI registers, which can cause undefined behavior under traffic*/
    rv = bcm_fabric_control_set(unit, bcmFabricIsolate, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_fabric_control_set \n");
        return BCM_E_FAIL;
    }

    /* Unregister the callback function */
    rv = bcm_rx_unregister(unit, NULL, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error cint_dfe_cpu_packets - bcm_rx_unregister \n");
        return BCM_E_FAIL;
    }

    printf("cint_dfe_cpu_packets - cint_cpu_pkts_reception_stop finished successfully \n");
    return BCM_E_NONE;
}

int cint_cpu_pkts_config_fap(int unit, int fe_id)
{
    int rv = BCM_E_NONE;
    int is_dnx = 0;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv))
    {
        printf("Error, in bcm_device_member_get, unit %d.\n", unit);
        return rv;
    }

    if (is_dnx)
    {
        rv = cint_dnx_cpu_pkts_config_fap(unit, fe_id);
    }
    else
    {
        rv = cint_dpp_cpu_pkts_config_fap(unit, fe_id);
    }

    return rv;
}

int cint_dpp_cpu_pkts_config_fap(int unit, int fe_id)
{
    bcm_gport_t modport_gport, voq_gport;
    int queue_id, rv = BCM_E_NONE, i;

    /* MAP sysport */
    BCM_GPORT_MODPORT_SET(modport_gport, fe_id /*fe id - should match the cpu_address sent to rx_start */, 0);
    rv = bcm_stk_sysport_gport_set(unit, 1000 /* sysport_gport */, modport_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_dpp_cpu_pkts_config_fap - bcm_stk_sysport_gport_set failed\n");
        return BCM_E_FAIL;
    }

    /* Create VOQ */
    rv = bcm_cosq_gport_add(unit, modport_gport, 8 /* num_cos */, BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &voq_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_dpp_cpu_pkts_config_fap - bcm_cosq_gport_add failed\n");
        return BCM_E_FAIL;
    }

    /* Convert the VOQ to a push-Q */
    for (i=0; i < 8 /*priorities*/; i++)
    {
        rv = bcm_cosq_gport_sched_set(unit, voq_gport, i, BCM_COSQ_DELAY_TOLERANCE_15, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error in cint_dpp_cpu_pkts_config_fap - bcm_cosq_gport_sched_set failed\n");
            return BCM_E_FAIL;
        }
    }

    printf("cint_dfe_cpu_packets - cint_dpp_cpu_pkts_config_fap finished successfully\n");

    return BCM_E_NONE;

}

int cint_dnx_cpu_pkts_config_fap(int unit, int fe_id)
{
    bcm_gport_t modport_gport, voq_gport;
    bcm_gport_t sysport_gport;
    int queue_id, rv = BCM_E_NONE, i;
    int mode;
    bcm_cosq_ingress_queue_bundle_gport_config_t config;
    int cosq;

    /* MAP sysport */
    BCM_GPORT_MODPORT_SET(modport_gport, fe_id /*fe id - should match the cpu_address sent to rx_start */, 0);
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, 1000);
    rv = bcm_stk_sysport_gport_set(unit, sysport_gport, modport_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_dnx_cpu_pkts_config_fap - bcm_stk_sysport_gport_set failed\n");
        return BCM_E_FAIL;
    }

    /*
     * Convert the VOQ to a push-Q
     */

    /*
     * Create VOQ
     */
    config.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
    config.port = sysport_gport;
    config.local_core_id = BCM_CORE_ALL;
    config.numq = 8;
    for (cosq = 0; cosq < config.numq; ++cosq)
    {
        /* profile 31 is configured as a push queue (queue that doesn't need credits) */
        config.queue_atrributes[cosq].delay_tolerance_level = 31;
        config.queue_atrributes[cosq].rate_class = 0;
    }
    rv = bcm_cosq_ingress_queue_bundle_gport_add(unit, &config, &voq_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_dnx_cpu_pkts_config_fap - bcm_cosq_ingress_queue_bundle_gport_add failed\n");
        return BCM_E_FAIL;
    }

    printf("cint_dfe_cpu_packets - cint_dnx_cpu_pkts_config_fap finished successfully\n");

    return BCM_E_NONE;
}


