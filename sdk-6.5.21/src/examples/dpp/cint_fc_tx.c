/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Soc_petra-B Example script - Flow Control transmission
 * 
 * How to run
 *      - fc_tx_run(int unit, int is_llfc, int nbr_queues_per_port)
 *        e.g. fc_tx_run(unit, 1, 4)       /* LLFC Configuration */
 *        e.g. fc_tx_run(unit, 0, 4)       /* CBFC Configuration */
 *
 * Environment
 *    - Flow Control configured for Port 1
 *      NOTE
 *        - SOC properties are one based and thus the above will be represented as Port 2
 *    - SOC properties are read by this application to determine the type of system configuration
 *      (e.g. inband - non calender based, inband - calender based, oob - calender based).
 *      It is the responsibility of some other entity to configure SOC properties
 *
 *      Due to limitation of bcm_port_interface_get() API (bcm_petra_port_interface_get()) it
 *      is not possible for application to determine if the selected interface is ILKN-0 or ILKN-1
 *      Thus it is assumed the port is on ILKN-0. If required additional SOC properties can be read
 *      to determine this. Currently this is not done.
 *    - Based on SOC properties the setup is done by this script.In addition the following
 *      arguments should be passed to this script
 *         arg1 => is_llfc (0 => LLFC, 1 => CBFC)
 *         arg2 => nbr_queues_per_port (4, Has to match the application "src/appl/diag/dpp/init.c")
 *    - Testing with an External Tester that has Data Center Support features
 *      - External Tester asserts LLFC or CBFC to a port.
 *      - Traffic is then sent to the port and verify that no data is
 *        scheduled out of Soc_petra.
 *      - resource buildup occurs and threshold get crossed to assert flow control towards tester
 *      - capture flow control at tester (LLFC or CBFC) to determine if device generated
 *        flow control
 *      - Configuration of remote device (for calender based interfaces) will be done via
 *        another script. The calender configuration has to be symmetrical.
 *        NOTE:
 *          - Configuration script for remote device is not covered here.
 *
 * Configuration
 *    - Non-Calender Based (InBand)
 *      - LLFC
 *        Normal Operation
 *      - CBFC
 *        - Determine number of flow control clases ("nbrIntfTCs") supported on the interface
 *        - determine number of flow control clases to be configured ("nbrCBFC")
 *              nbrCBFC = Min (nbrIntfTCs, nbrQueues)
 *              nbrQueuesTc = nbrQueues/nbrCBFC
 *        - assign VSQ TC to the queues based on the above
 *        - First half of those classes ( 0 - ((n/2) - 1) ) assert flow control to
 *          Egress Queue 0 (BCM_COSQ_HIGH_PRIORITY).
 *        - Second half of those classes ( (n/2) - (n - 1) ) assert flow control to
 *          Egress Queue 1 (BCM_COSQ_LOW_PRIORITY).
 *        - This script does not map incoming priority (e.g. vlan priority) to Traffic ClassTC.
 *          It relies on the default setup (reference "src/appl/diag/dpp/init.c"). If the default
 *          setup is different from above it should be accounted for
 *          when CBFC is asserted via the tester.
 *      - Example of some of the relevant SOC properties is
 *         - config add ucode_port_2=RXAUI6
 *
 *   - Calender Based Interface (InBand)
 *      - LLFC
 *        - Calender Entry 0. Configured via SOC properties and not via APIs
 *      - CBFC
 *        - Calender Entry 9 - 16 (for port 1) 
 *          (1 + (port * 8)) - (1 + (port * 8) + (8 - 1)) 
 *        - Determine number of flow control clases ("nbrIntfTCs") supported on the interface
 *        - determine number of flow control clases to be configured ("nbrCBFC")
 *              nbrCBFC = Min (nbrIntfTCs, nbrQueues)
 *              nbrQueuesTc = nbrQueues/nbrCBFC
 *        - assign VSQ TC to the queues based on the above
 *        - This script does not map incoming priority (e.g. vlan priority) to Traffic ClassTC.
 *          It relies on the default setup (reference "src/appl/diag/dpp/init.c"). If the default
 *          setup is different from above it should be accounted for
 *          when CBFC is asserted via the tester.
 *     - Example of some of the relevant SOC properties is
 *       - ilkn_num_lanes_0=12
 *       - ucode_port_2=ILKN0.1
 *       - fc_inband_intlkn_mode.port1=0x3
 *       - fc_inband_intlkn_calender_length.port1=256
 *       - fc_inband_intlkn_calender_rep_count.port1=1
 *       - fc_inband_intlkn_calender_llfc_mode=1
 *       - fc_inband_intlkn_llfc_mub_enable_mask=0
 *
 *   - Calender Based Interface (OOB)
 *      - LLFC
 *        - Supported
 *      - CBFC
 *        - Calender Entry 9 - 16 (for port 1) 
 *          (1 + (port * 8)) - (1 + (port * 8) + (8 - 1)) 
 *        - Determine number of flow control clases ("nbrIntfTCs") supported on the interface
 *        - determine number of flow control clases to be configured ("nbrCBFC")
 *              nbrCBFC = Min (nbrIntfTCs, nbrQueues)
 *              nbrQueuesTc = nbrQueues/nbrCBFC
 *        - assign VSQ TC to the queues based on the above
 *        - This script does not map incoming priority (e.g. vlan priority) to Traffic ClassTC.
 *          It relies on the default setup (reference "src/appl/diag/dpp/init.c"). If the default
 *          setup is different from above it should be accounted for
 *          when CBFC is asserted via the tester.
 *     - Example of some of the relevant SOC properties is
 *       - ilkn_num_lanes_0=12
 *       - ucode_port_2=ILKN0.1
 *       - fc_oob_type.port1=2
 *       - fc_oob_mode.port1=0x2
 *       - fc_oob_calender_length.port1=256
 *       - fc_oob_calender_rep_count.port1=1
 */


/* Global definitions, dependiencies on the application pprogram */
int   SYSPORT_OFF_START = 100;
int   VOQ_OFF_START = 4;
int   NBR_COS = 4;
int   DEFAULT_UCAST_QUEUE_CT = 2;

int   LLFC_XOFF_THRESHOLD = 10;
int   LLFC_XON_THRESHOLD  = 5;
int   LLFC_XOFF_THRESHOLD_BD = 10;
int   LLFC_XON_THRESHOLD_BD = 5;

int   CBFC_XOFF_THRESHOLD = 10;
int   CBFC_XON_THRESHOLD = 5;
int   CBFC_XOFF_THRESHOLD_BD =  10;
int   CBFC_XON_THRESHOLD_BD = 5;

/* Global definitions, local state. To help in cleanup */
bcm_gport_t    ct_vsq[4];
bcm_gport_t    cttc_vsq[32];
bcm_gport_t    llfc_gport[64];
bcm_gport_t    cbfc_gport[512];


int
create_ct_cttc_vsqs(int unit)
{
    int                  rc = BCM_E_NONE;
    bcm_cosq_vsq_info_t  vsq_info;
    int                  nbr_ct_vsq, nbr_cttc_vsq, ct, tc;
    bcm_gport_t          dummy_gport;


    sal_memset(&ct_vsq, 0, (sizeof(dummy_gport) * 4));
    sal_memset(&cttc_vsq, 0, (sizeof(dummy_gport) * 32));

    /* create category vsqs */
    sal_memset(&vsq_info, 0, sizeof(vsq_info));
    vsq_info.flags = BCM_COSQ_VSQ_CT;

    for (nbr_ct_vsq = 0; nbr_ct_vsq < 4; nbr_ct_vsq++) {
        vsq_info.ct_id = nbr_ct_vsq;

        rc = bcm_cosq_gport_vsq_create( unit, &vsq_info, &ct_vsq[nbr_ct_vsq]);
        if (BCM_FAILURE(rc)) {
            printf("create_ct_cttc_vsqs Failed in creating ct_vsq, Error (%d, %s)\n", rc, bcm_errmsg(rc));
            return(rc);
        }
    }

    /* create category-tc vsq's */
    sal_memset(&vsq_info, 0, sizeof(vsq_info));
    vsq_info.flags = BCM_COSQ_VSQ_CTTC;

    for (nbr_cttc_vsq = 0; nbr_cttc_vsq < 32; nbr_cttc_vsq++) {
        ct = nbr_cttc_vsq / 8;
        tc = nbr_cttc_vsq % 8;

         vsq_info.ct_id = ct;
         vsq_info.traffic_class = tc;

        rc = bcm_cosq_gport_vsq_create( unit, &vsq_info, &cttc_vsq[nbr_cttc_vsq]);
        if (BCM_FAILURE(rc)) {
            printf("create_ct_cttc_vsqs Failed in creating cttc_vsq, Error (%d, %s)\n", rc, bcm_errmsg(rc));
            return(rc);
        }
    }

    return(rc);
}


int
cttc_vsq_get(int unit, int ct, int tc,  bcm_gport_t *cttc_vsq_gport)
{
    int  rc = BCM_E_NONE;
    int  nbr_cttc_vsq;


    nbr_cttc_vsq = (ct * 8) + tc;
    (*cttc_vsq_gport) = cttc_vsq[nbr_cttc_vsq]; 

    return(rc);
}

int
add_queue_to_cttc_vsq(int unit, bcm_gport_t queue_gport, int cosq, int ct, int tc)
{
    int          rc = BCM_E_NONE;
    bcm_gport_t  cttc_vsq_gport;
    int          queue;


    rc = cttc_vsq_get(unit, ct, tc, &cttc_vsq_gport);
    if (BCM_FAILURE(rc)) {
        printf("add_queue_to_cttc_vsq Failed in retreiving cttc_vsq, Error (%d, %s)\n", rc, bcm_errmsg(rc));
        return(rc);
    }

    queue = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(queue_gport); 
    printf("  queue(bQid: 0x%x, cos %d, gport: 0x%x) added to cttcVsq (ct: %d, tc: %d, gport :0x%x)\n", queue, cosq, queue_gport, ct, tc, cttc_vsq_gport); /* DEBUG */

    rc = bcm_cosq_gport_vsq_add(unit, cttc_vsq_gport, queue_gport, cosq);
    if (BCM_FAILURE(rc)) {
        printf("add_queue_to_cttc_vsq Failed to add queue to cttc_vsq, Error (%d, %s)\n", rc, bcm_errmsg(rc));
        return(rc);
    }

    return(rc);
}

/* reference application "src/appl/diag/dpp/init.c" for the following definitions */
int
port_queue_gport_get(int unit, bcm_gport_t modport_gport, int nbr_queues_per_port,
                                                          bcm_gport_t *queue_gport)
{
    int   rc = BCM_E_NONE;
    int   mod, port;
    int   sysport, base_queue;


    if (!BCM_GPORT_IS_MODPORT(modport_gport)) {
        printf("error, port_queue_gport_get() invalid modport_gport(0x%x)\n", modport_gport);
        return(rc = BCM_E_PARAM);
    }

    mod = BCM_GPORT_MODPORT_MODID_GET(modport_gport);
    port = BCM_GPORT_MODPORT_PORT_GET(modport_gport);

    sysport = (SYSPORT_OFF_START * mod) + port;
    base_queue = VOQ_OFF_START + ((sysport - 1) * nbr_queues_per_port);

    BCM_GPORT_UNICAST_QUEUE_GROUP_SET((*queue_gport), base_queue);

    printf("  ModPort (mod: %d, port: %d, gport: 0x%x) has Queue (bQid 0x%x, gport: 0x%x)\n", mod, port, modport_gport, base_queue, (*queue_gport)); /* DEBUG */

    return(rc);
} 

int
configure_fc_intf_transport_parameters(int unit, bcm_gport_t gport)
{
    int                            rc = BCM_E_NONE;
    bcm_port_congestion_config_t   congestion_info;
    bcm_port_t                     local_port;

    sal_memset(&congestion_info, 0, sizeof(congestion_info));
    congestion_info.src_mac = "AA:AA:AA:AA:AA:AA";

    rc = bcm_port_congestion_config_set(unit, gport, &congestion_info);
    if (BCM_FAILURE(rc)) {
        printf("bcm_port_congestion_config_set Failed for gport(0x%x), Error (%d, 0x%x)\n", gport, rv, rv);
        return(rc);
    }

    local_port = BCM_GPORT_MODPORT_PORT_GET(gport);
/*
    rc = bcm_port_pause_addr_set(unit, local_port, config->src_mac)

    if (BCM_FAILURE(rc)) {
        printf("bcm_port_pause_addr_set Failed for port(%d), Error (%d, 0x%x)\n", local_port, rv, rv);
        return(rc);
    }
*/

    return(rc);
}

int
create_configure_llfc_vsq(int unit, bcm_gport_t dest_gport, bcm_gport_t dest_if_gport,
                                                                   bcm_gport_t *vsq_gport)
{
    int                    rc = BCM_E_NONE;
    bcm_cosq_vsq_info_t    vsq_info;
    bcm_cosq_pfc_config_t  config;


    /* create llfc vsq */
    sal_memset(&vsq_info, 0, sizeof(vsq_info));
    vsq_info.flags = BCM_COSQ_VSQ_LLFC;
/*
    vsq_info.src_port = dest_gport;
*/
    vsq_info.src_port = BCM_GPORT_MODPORT_PORT_GET(dest_gport);

    rc = bcm_cosq_gport_vsq_create( unit, &vsq_info, vsq_gport);
    if (BCM_FAILURE(rc)) {
        printf("bcm_cosq_gport_vsq_create Failed (LLFC, gport(0x%x)), Error (%d, %s)\n", dest_gport, rc, bcm_errmsg(rc));
        return(rc);
    }

    /* Setup vsq threshold triggers */
    sal_memset(&config, 0, sizeof(config));
    config.xoff_threshold = LLFC_XOFF_THRESHOLD;
    config.xon_threshold = LLFC_XON_THRESHOLD;
    config.xoff_threshold_bd = LLFC_XOFF_THRESHOLD_BD;
    config.xon_threshold_bd = LLFC_XON_THRESHOLD_BD;

    rc = bcm_cosq_pfc_config_set(unit, (*vsq_gport), 0, 0, &config);
    if (BCM_FAILURE(rc)) {
        printf("bcm_cosq_gport_vsq_create Failed setting VSQ Thresholds (LLFC, destGport(0x%x), vsqGport(0x%x)), Error (%d, %s)\n", dest_gport, (*vsq_gport), rc, bcm_errmsg(rc));
        return(rc);
    }
    
    return(rc);
}

int
create_configure_cbfc_vsq(int unit, bcm_gport_t dest_gport, bcm_gport_t dest_if_gport,
                                                            int tc, bcm_gport_t *vsq_gport)
{
    int                    rc = BCM_E_NONE;
    bcm_cosq_vsq_info_t    vsq_info;
    bcm_cosq_pfc_config_t  config;


    /* create cbfc vsq */
    sal_memset(&vsq_info, 0, sizeof(vsq_info));
    vsq_info.flags = BCM_COSQ_VSQ_PFC;
/*
    vsq_info.src_port = dest_gport;
*/
    vsq_info.src_port = BCM_GPORT_MODPORT_PORT_GET(dest_gport);

    vsq_info.traffic_class = tc;

    rc = bcm_cosq_gport_vsq_create( unit, &vsq_info, vsq_gport);
    if (BCM_FAILURE(rc)) {
        printf("bcm_cosq_gport_vsq_create Failed (CBFC, gport(0x%x), tc(%d)), Error (%d, %s)\n", dest_gport, tc, rc, bcm_errmsg(rc));
        return(rc);
    }

    /* Setup vsq threshold triggers */
    sal_memset(&config, 0, sizeof(config));
    config.xoff_threshold = CBFC_XOFF_THRESHOLD;
    config.xon_threshold = CBFC_XON_THRESHOLD;
    config.xoff_threshold_bd = CBFC_XOFF_THRESHOLD_BD;
    config.xon_threshold_bd = CBFC_XON_THRESHOLD_BD;

    rc = bcm_cosq_pfc_config_set(unit, (*vsq_gport), 0, 0, &config);
    if (BCM_FAILURE(rc)) {
        printf("bcm_cosq_gport_vsq_create Failed setting VSQ Thresholds (CBFC, destGport(0x%x), vsqGport(0x%x) for class(%d)), Error (%d, %s)\n", dest_gport, (*vsq_gport), tc, rc, bcm_errmsg(rc));
        return(rc);
    }

    return(rc);
}

int
fc_port_cbfc_queue_info_get(int unit, int nbr_queues_per_port, int nbr_intf_classes,
                                    int *nbr_cbfc_classes_configured, int *nbr_queues_tc)
{
    int                           rc = BCM_E_NONE;


    if (nbr_queues_per_port > nbr_intf_classes) {
        (*nbr_cbfc_classes_configured) = nbr_intf_classes;
    }
    else {
        (*nbr_cbfc_classes_configured) = nbr_queues_per_port;
    }

    (*nbr_queues_tc) = nbr_queues_per_port / (*nbr_cbfc_classes_configured);

    return(rc);
}

int
configure_fc_tx_non_calender_llfc(int unit, bcm_gport_t dest_gport, bcm_gport_t dest_if_gport)
{
    int                           rc = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlGeneration;
    bcm_gport_t                   vsq_gport;
    int                           port, llfc_index;
    int                           mod;


    rc = configure_fc_intf_transport_parameters(unit, dest_gport);
    if (BCM_FAILURE(rc)) {
        return(rc);
    }

    /* create llfc vsq object */
    rc = create_configure_llfc_vsq(unit, dest_gport, dest_if_gport, &vsq_gport);
    if (BCM_FAILURE(rc)) {
        return(rc);
    }

    /* update local data structure */
    mod = BCM_GPORT_MODPORT_MODID_GET(dest_gport); /* DEBUG */
    port = BCM_GPORT_MODPORT_PORT_GET(dest_gport);
    port = (port >= 500) ? (port - 500) : port;
    llfc_index = port;
    llfc_gport[llfc_index] = vsq_gport;

    printf("  Non-Calender InBand LLFC, ModPort (mod: %d, port: %d, gport: 0x%x), vsq_gport: 0x%x\n", mod, port, dest_gport, llfc_gport[llfc_index]); /* DEBUG */

    /* flow control configuration */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    source.port = vsq_gport;
    target.flags = BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    rc = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
    if (BCM_FAILURE(rc)) {
        printf("bcm_cosq_fc_path_add failed, LLFC, port_gport(0x%x) err=%d %s\n", dest_gport, rc, bcm_errmsg(rc));
        return(rc);
    }

    return(rc);
}


int
configure_fc_tx_non_calender_cbfc(int unit, bcm_gport_t dest_gport, bcm_gport_t dest_if_gport,
                               int nbr_intf_classes, int nbr_queues_per_port)
{
    int                           rc = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlGeneration;
    int                           nbr_cbfc_classes_configured, nbr_queues_tc;
    int                           nbr_classes;
    int                           port, cbfc_index; 
    int                           mod;


    rc = configure_fc_intf_transport_parameters(unit, dest_gport);
    if (BCM_FAILURE(rc)) {
        return(rc);
    }

    /* create cbfc vsq objects */
    mod = BCM_GPORT_MODPORT_MODID_GET(dest_gport); /* DEBUG */
    port = BCM_GPORT_MODPORT_PORT_GET(dest_gport);
    port = (port >= 500) ? (port - 500) : port;
    cbfc_index = port * 8;

    rc = fc_port_cbfc_queue_info_get(unit, nbr_queues_per_port, nbr_intf_classes,
                                    &nbr_cbfc_classes_configured, &nbr_queues_tc);
    if (BCM_FAILURE(rc)) {
        return(rc);
    }

    printf("  Non-Calender InBand CBFC, ModPort (mod: %d, port: %d, gport: 0x%x), cbfcClasses: %d, nbrQueuesInTc: %d\n", mod, port, dest_gport, nbr_cbfc_classes_configured, nbr_queues_tc); /* DEBUG */

    for (nbr_classes = 0; nbr_classes < nbr_cbfc_classes_configured; nbr_classes++) {
        /* also update local data structure */
        rc = create_configure_cbfc_vsq(unit, dest_gport, dest_if_gport,
                                           nbr_classes, &cbfc_gport[(cbfc_index + nbr_classes)]);
        if (BCM_FAILURE(rc)) {
            return(rc);
        }

        printf("  Non-Calender InBand CBFC, ModPort (mod: %d, port: %d tc: %d, gport: 0x%x), vsq_gport: 0x%x\n", mod, port, nbr_classes, dest_gport, cbfc_gport[(cbfc_index + nbr_classes)]); /* DEBUG */
    }

    /* flow control configuration */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));
    target.flags = BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    for (nbr_classes = 0; nbr_classes < nbr_cbfc_classes_configured; nbr_classes++) {
         source.port = cbfc_gport[(cbfc_index + nbr_classes)];

         rc = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
         if (BCM_FAILURE(rc)) {
             printf("bcm_cosq_fc_path_add failed, CBFC(%d), port_gport(0x%x) err=%d %s\n", nbr_classes, dest_gport, rc, bcm_errmsg(rc));
             return(rc);
         }
    }

    return(rc);
}

int
configure_fc_tx_calender_llfc(int unit, bcm_gport_t dest_gport, bcm_gport_t dest_if_gport,
                                        int is_oob, bcm_gport_t oob_gport, int cal_index)
{
    int                           rc = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlGeneration;
    bcm_gport_t                   vsq_gport;
    int                           port, llfc_index;
    int                           mod;


    /* create llfc vsq object */
    rc = create_configure_llfc_vsq(unit, dest_gport, dest_if_gport, &vsq_gport);
    if (BCM_FAILURE(rc)) {
        return(rc);
    }

    /* update local data structure */
    mod = BCM_GPORT_MODPORT_MODID_GET(dest_gport); /* DEBUG */
    port = BCM_GPORT_MODPORT_PORT_GET(dest_gport);
    port = (port >= 500) ? (port - 500) : port;
    llfc_index = port;
    llfc_gport[llfc_index] = vsq_gport;

    /* flow control configuration */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    source.port = vsq_gport;
    if (is_oob) {
        target.port = oob_gport;
    }
    else {
        target.port = dest_gport;
    }
    target.calender_index = 0;

    printf("  Calender InBand LLFC, ModPort (mod: %d, port: %d, gport: 0x%x), Calender: %d\n", mod, port, dest_gport, target.calender_index); /* DEBUG */

    rc = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
    if (BCM_FAILURE(rc)) {
        printf("bcm_cosq_fc_path_add failed, LLFC, port_gport(0x%x) err=%d %s\n", dest_gport, rc, bcm_errmsg(rc));
        return(rc);
    }

    return(rc);
}

int
configure_fc_tx_calender_cbfc(int unit, bcm_gport_t dest_gport, bcm_gport_t dest_if_gport,
                              int nbr_intf_classes, int is_oob, bcm_gport_t oob_gport,
                              int nbr_queues_per_port, int cal_index)
{
    int                           rc = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlGeneration;
    int                           nbr_cbfc_classes_configured, nbr_queues_tc;
    int                           nbr_classes;
    int                           port, cbfc_index; 
    int                           mod;


    /* create cbfc vsq objects */
    mod = BCM_GPORT_MODPORT_MODID_GET(dest_gport); /* DEBUG */
    port = BCM_GPORT_MODPORT_PORT_GET(dest_gport);
    port = (port >= 500) ? (port - 500) : port;
    cbfc_index = port * 8;

    rc = fc_port_cbfc_queue_info_get(unit, nbr_queues_per_port, nbr_intf_classes,
                                    &nbr_cbfc_classes_configured, &nbr_queues_tc);
    if (BCM_FAILURE(rc)) {
        return(rc);
    }

    for (nbr_classes = 0; nbr_classes < nbr_cbfc_classes_configured; nbr_classes++) {
        /* also update local data structure */
        rc = create_configure_cbfc_vsq(unit, dest_gport, dest_if_gport,
                                           nbr_classes, &cbfc_gport[(cbfc_index + nbr_classes)]);
        if (BCM_FAILURE(rc)) {
            return(rc);
        }
    }

    /* flow control configuration */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    if (is_oob) {
        target.port = oob_gport;
    }
    else {
        target.port = dest_gport;
    }

    for (nbr_classes = 0; nbr_classes < nbr_cbfc_classes_configured; nbr_classes++) {
         source.port = cbfc_gport[(cbfc_index + nbr_classes)];
         target.calender_index = 1 + (port * 8) + nbr_classes;
        
         /* determine if calendar index needs to be overridden */
         if (cal_index != -1) {
             target.calender_index = cal_index + nbr_classes;
         }

         printf("  Calender InBand CBFC, ModPort (mod: %d, port: %d tc: %d, gport: 0x%x), Calender: %d\n", mod, port, nbr_classes, dest_gport, target.calender_index); /* DEBUG */

         rc = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
         if (BCM_FAILURE(rc)) {
              printf("bcm_cosq_fc_path_add failed, CBFC(%d), port_gport(0x%x) err=%d %s\n", nbr_classes, dest_gport, rc, bcm_errmsg(rc));
              return(rc);
         }
    }

    return(rc);
}


int
fc_tx_run(int unit, int is_llfc, int nbr_queues_per_port,
                                           int port_nbr, int cal_index, int ilkn_intf)
{
    int            SOC_PETRA_CARD_NUM_PORTS = 1;
    int            rc = BCM_E_NONE;
    int            my_modid;
    int            idx, port, system_port;
    bcm_gport_t    dest_gport[4];
    bcm_gport_t    sys_gport[4];
    bcm_gport_t    if_gport[4];
    int            flags;
    bcm_port_if_t  intf;
    int            is_intf_calender = FALSE, nbr_intf_classes = 2, nbr_cbfc_classes_configured;
    int            is_oob = FALSE, oob_port, oob_gport, oob_mode, nbr_port;
    int            ilkn_port = 0, ilkn_mode;
    int            nbr_queues_tc, queue_cos, queue_tc, nbr_qtc;
    bcm_gport_t    queue_gport;


    rc = bcm_stk_modid_get(unit, &my_modid);
    if (BCM_FAILURE(rc)) {
        printf("bcm_stk_modid_get failed err=%d %s\n", rc, bcm_errmsg(rc));
        return(rc);
    }

    /* to be removed if the application program can handle other cos levels as default */
    if (nbr_queues_per_port != NBR_COS) {
        printf("fc_tx_run invalid configuraion nbr_queues_per_port (%d)\n", nbr_queues_per_port);
        return(rc = BCM_E_INTERNAL);
    }

    /* global Configuration */
    rc = create_ct_cttc_vsqs(unit);
    if (BCM_FAILURE(rc)) {
        return(rc);
    }

    switch (nbr_queues_per_port) {
        case 4:
            break;

        case 8:
            break;

        default:
            printf("fc_tx_run invalid configuraion nbr_queues_per_port (%d)\n", nbr_queues_per_port);
            return(rc = BCM_E_INTERNAL);
            break;
    }

    for (idx = 0; idx < SOC_PETRA_CARD_NUM_PORTS; idx++) {

        port = idx + 1;
        if (port_nbr != -1) { /* determine if it needs to be overridden */
            port = port_nbr + 1;
        }
        system_port = (SYSPORT_OFF_START * my_modid) + port;

        /* update various handles */
        BCM_GPORT_MODPORT_SET(dest_gport[idx], my_modid, port);
        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport[idx], system_port);

        flags = 0;
        rc = bcm_fabric_port_get(unit, dest_gport[idx], flags, &if_gport[idx]);
        if (BCM_FAILURE(rc)) {
            printf("bcm_fabric_port_get failed, port_gport(0x%x) err=%d %s\n", dest_gport[idx], rc, bcm_errmsg(rc));
            return(rc);
        }

        /* determine type of interface (calender versus non-calender) */
        rc = bcm_port_interface_get(unit, port, &intf);
        if (BCM_FAILURE(rc)) {
            printf("bcm_port_interface_get failed, port(0x%x) err=%d %s\n", port, rc, bcm_errmsg(rc));
            return(rc);
        }

        /* NOTE: CINT is not able to interpret case statements that are bunched together. */
        /*       Thus the code is duplicated below.                                       */     
        switch (intf) {
            case BCM_PORT_IF_RXAUI:
                is_intf_calender = FALSE;
                nbr_intf_classes = 2;
                break;

            case BCM_PORT_IF_XAUI:
                is_intf_calender = FALSE;
                nbr_intf_classes = 2;
                break;

            case BCM_PORT_IF_SGMII:
                is_intf_calender = FALSE;
                nbr_intf_classes = 2;
                break;

            case BCM_PORT_IF_QSGMII:
                is_intf_calender = FALSE;
                nbr_intf_classes = 2;
                break;

            case BCM_PORT_IF_ILKN:
                is_intf_calender = TRUE;
                nbr_intf_classes = 8;
                ilkn_port = 0;
                if (ilkn_intf != -1) { /* determine if it needs to be overridden */
                    ilkn_port = ilkn_intf;
                }
                break;

            default:
                printf("Unsuported interface type (%d) for port(%d) (%d)\n", intf, port, BCM_PORT_IF_RXAUI);
                return(rc = BCM_E_INTERNAL);
                break;
        }

        /* for calender based interface determine if InBand or OOB    */
        /* NOTE:                                                      */
        /*   - correct board needs to be present for testing OOB      */
        /*   - calender based interface will also require configuring */
        /*     remote device. Routines for configuration of remote    */
        /*     devices will have to be added                          */
        if (is_intf_calender) {
            is_oob = FALSE;

            /* check if InBand calender is enabled for transmission */
            ilkn_mode = soc_property_port_get(unit, ilkn_port, spn_FC_INBAND_INTLKN_MODE, 0);
            if (ilkn_mode & 0x2) {
                is_oob = FALSE;
            }
            else {
                /* determine which OOB interface is enabled for transmission */
                /* pickup the first one that is enabled for transmission     */
                for (nbr_port = 0; nbr_port < 2; nbr_port++) {
                    oob_mode = soc_property_port_get(unit, nbr_port, spn_FC_OOB_MODE, 0);
                    if (oob_mode & 0x2) {
                        is_oob = TRUE;
                        break;
                    }
                }
                if (is_oob == TRUE) {
                    oob_port = nbr_port;
                    BCM_GPORT_CONGESTION_SET(oob_gport, oob_port);
                }
                else {
                    printf("error, no OOB interface selected for Flow Control transmission\n");
                    return(rc = BCM_E_INTERNAL);
                }
            }
        }

        /* port configuration that can be done for all cases */
        /* associate queues to a VSQ TC */
        rc = fc_port_cbfc_queue_info_get(unit, nbr_queues_per_port, nbr_intf_classes,
                                    &nbr_cbfc_classes_configured, &nbr_queues_tc);
        if (BCM_FAILURE(rc)) {
            return(rc);
        }

        rc = port_queue_gport_get(unit, dest_gport[idx], nbr_queues_per_port, &queue_gport);
        if (BCM_FAILURE(rc)) {
            return(rc);
        }

        for (queue_cos = 0, queue_tc = 0; queue_cos < nbr_queues_per_port; queue_tc++) {
            for (nbr_qtc = 0; nbr_qtc < nbr_queues_tc; nbr_qtc++, queue_cos++) {
                rc = add_queue_to_cttc_vsq(unit, queue_gport, queue_cos, DEFAULT_UCAST_QUEUE_CT, queue_tc);
                if (BCM_FAILURE(rc)) {
                    return(rc);
                }
            }
        } 

        /* flow control configuration */
        if (!is_intf_calender) {
            if (is_llfc) {
                printf("Non-Calender Interface - LLFC\n");

                rc = configure_fc_tx_non_calender_llfc(unit, dest_gport[idx], if_gport[idx]);
                if (BCM_FAILURE(rc)) {
                    return(rc);
                }
            }
            else {
                printf("Non-Calender Interface - CBFC\n");

                rc = configure_fc_tx_non_calender_cbfc(unit, dest_gport[idx], if_gport[idx], nbr_intf_classes, nbr_queues_per_port);
                if (BCM_FAILURE(rc)) {
                    return(rc);
                }
            }
        }
        else {
            if (is_llfc) {
                if (is_oob == FALSE) {
                    printf("error, LLFC configured via SOC properties for inBand interface\n");
                    return(rc = BCM_E_INTERNAL);
                }

                printf("Calender Interface (%s) - LLFC\n", (is_oob) ? "OOB" : "InBand");

                rc = configure_fc_tx_calender_llfc(unit, dest_gport[idx], if_gport[idx], is_oob, oob_gport, cal_index);
                if (BCM_FAILURE(rc)) {
                    return(rc);
                }
            }
            else {
                printf("Calender Interface (%s) - CBFC\n", (is_oob) ? "OOB" : "InBand");

                rc = configure_fc_tx_calender_cbfc(unit, dest_gport[idx], if_gport[idx],
                              nbr_intf_classes, is_oob, oob_gport, nbr_queues_per_port, cal_index);
                if (BCM_FAILURE(rc)) {
                    return(rc);
                }
            }
        }

        if (port_nbr != -1) {
            break;
        }

    }

    printf("cint_fc_tx.c completed with status (%s)\n", bcm_errmsg(rc));

    return(rc);
}

/* LLFC */
/*   - Non-Calender Interface (InBand) */
/*   - Calender Interface (OOB) */
/*   - NOTE: SOC properties configure Calender Interface (InterLaken) */
/* fc_tx_run(unit, 1, 4, -1, -1, -1); */

/* CBFC */
/*   - Non-Calender Interface (InBand) */
/*   - Calender Interface (InterLaken) */
/*   - Calender Interface (OOB) */
fc_tx_run(0, 0, 4, -1, -1, -1);

/* CBFC - Interoperability with Shadow */
/* fc_tx_run(0, 0, 4, 0, 0, 0); */


