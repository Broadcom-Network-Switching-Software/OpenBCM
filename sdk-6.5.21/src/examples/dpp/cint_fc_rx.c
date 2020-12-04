/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Soc_petra-B Example script - Fllow Control reception
 * 
 * How to run
 *      - fc_rx_run(int unit, int is_llfc)
 *        e.g. fc_rx_run(unit, 1)       - LLFC Configuration
 *        e.g. fc_rx_run(unit, 0)       - CBFC Configuration 
 *
 * Environment
 *    - Flow Control configured for Port 1
 *      NOTE
 *        - SOC properties are one based and thus the above will be represented as Port 2/Port3
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
 *    - Testing with an External Tester taht has Dta Center Support features
 *      - External Tester asserts LLFC or CBFC.
 *      - Traffic is then sent to the port and verify that no data is
 *        scheduled out of Soc_petra.
 *      - External tester deasserts LLFC or CBFC
 *      - Traffic is then sent to the port and verify that data is scheduled
 *        out by Soc_petra
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
 *        - Determine number of flow control clases ("n") supported on the interface
 *        - First half of those classes ( 0 - ((n/2) - 1) ) assert flow control to
 *          Egress Queue 0 (BCM_COSQ_HIGH_PRIORITY).
 *        - Second half of those classes ( (n/2) - (n - 1) ) assert flow control to
 *          Egress Queue 1 (BCM_COSQ_LOW_PRIORITY).
 *        - This script does not map Packet-TC to Egress Queues. It relies on the default
 *          setup. If the default setup is different from above it should be accounted for
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
 *        - Determine number of flow control clases ("n") supported on the interface
 *        - First half of those classes ( 0 - ((n/2) - 1) ) assert flow control to
 *          Egress Queue 0 (BCM_COSQ_HIGH_PRIORITY).
 *        - Second half of those classes ( (n/2) - (n - 1) ) assert flow control to
 *          Egress Queue 1 (BCM_COSQ_LOW_PRIORITY).
 *        - This script does not map Packet-TC to Egress Queues. It relies on the default
 *          setup. If the default setup is different from above it should be accounted for
 *          when CBFC is asserted via the tester.
 *     - Example of some of the relevant SOC properties is
 *       - ilkn_num_lanes_0=12
 *       - ucode_port_2=ILKN0.1
 *       - fc_inband_intlkn_mode.port1=0x1
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
 *        - Determine number of flow control clases ("n") supported on the interface
 *        - First half of those classes ( 0 - ((n/2) - 1) ) assert flow control to
 *          Egress Queue 0 (BCM_COSQ_HIGH_PRIORITY).
 *        - Second half of those classes ( (n/2) - (n - 1) ) assert flow control to
 *          Egress Queue 1 (BCM_COSQ_LOW_PRIORITY).
 *        - This script does not map Packet-TC to Egress Queues. It relies on the default
 *          setup. If the default setup is different from above it should be accounted for
 *          when CBFC is asserted via the tester.
 *     - Example of some of the relevant SOC properties is
 *       - ilkn_num_lanes_0=12
 *       - ucode_port_2=ILKN0.1
 *       - fc_inband_intlkn_mode.port1=0x1
 *       - fc_oob_type.port1=2
 *       - fc_oob_mode.port1=0x1
 *       - fc_oob_calender_length.port1=256
 *       - fc_oob_calender_rep_count.port1=1
 */

int CONFIGURE_FC_COSQ_HIGH_PRIORITY = 1; /* BCM_COSQ_HIGH_PRIORITY */
int CONFIGURE_FC_COSQ_LOW_PRIORITY = 0; /* BCM_COSQ_LOW_PRIORITY */


int
configure_fc_packet_mapping(int unit, bcm_gport_t gport)
{
    int                            rc = BCM_E_NONE;
    bcm_gport_t                    ucast_gport;
    int                            tc, dp, queue;

    BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(ucast_gport, gport);

    for (tc = 0; tc < 8; tc++) {
        queue = (tc < 4) ? CONFIGURE_FC_COSQ_HIGH_PRIORITY : CONFIGURE_FC_COSQ_LOW_PRIORITY;
        for (dp = 0; dp < 4; dp++) {
            rc = bcm_cosq_gport_egress_map_set(unit, ucast_gport, tc, dp, queue);
            if (rc != BCM_E_NONE) {
                printf("Error, in uc queue mapping, ucast_gport: 0x%x, tc: %d, dp: %d, queue %d, error: %d\n", ucast_gport, tc, dp, queue, rc);
                return(rc);
            }
        }

        printf("  Mapped Packet TC-(%d) DP-(All) to Egress %s Priority Queue\n", tc,
                           ((queue == CONFIGURE_FC_COSQ_HIGH_PRIORITY) ? "High" : "low"));
    }

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
        printf("bcm_port_congestion_config_set Failed for gport(0x%x), Error (%d, 0x%x)\n", gport, rc, rc);
        return(rc);
    }

    local_port = BCM_GPORT_MODPORT_PORT_GET(gport);
/*
    rc = bcm_port_pause_addr_set(unit, local_port, config->src_mac)

    if (BCM_FAILURE(rc)) {
        printf("bcm_port_pause_addr_set Failed for port(%d), Error (%d, 0x%x)\n", local_port, rc, rc);
        return(rc);
    }
*/

    return(rc);
}


int
configure_fc_rx_non_calender_llfc(int unit, bcm_gport_t dest_gport, bcm_gport_t dest_if_gport)
{
    int                           rc = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;


    rc = configure_fc_intf_transport_parameters(unit, dest_gport);
    if (BCM_FAILURE(rc)) {
        return(rc);
    }

    /* flow control configuration */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    source.port = dest_gport;
    source.cosq = -1;
    target.flags = BCM_COSQ_FC_ENDPOINT_PORT_IMPLICIT;

    rc = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
    if (BCM_FAILURE(rc)) {
        printf("bcm_cosq_fc_path_add failed, LLFC, port_gport(0x%x) err=%d %s\n", dest_gport, rc, bcm_errmsg(rc));
        return(rc);
    }

    return(rc);
}


int
configure_fc_rx_non_calender_cbfc(int unit, bcm_gport_t dest_gport, bcm_gport_t dest_if_gport,
                               int nbr_intf_classes, int nbr_high_intf_classes)
{
    int                           rc = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;
    int                           nbr_classes;


    rc = configure_fc_intf_transport_parameters(unit, dest_gport);
    if (BCM_FAILURE(rc)) {
        return(rc);
    }

    /* flow control configuration */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    source.port = dest_gport;
    target.port = dest_gport;

    for (nbr_classes = 0; nbr_classes < nbr_intf_classes; nbr_classes++) {
        source.cosq = nbr_classes;
        if (nbr_classes < nbr_high_intf_classes) {
            target.cosq = BCM_COSQ_HIGH_PRIORITY;
        }
        else {
            target.cosq = BCM_COSQ_LOW_PRIORITY;
        }
               
        printf("  Mapped Flow Control TC-(%d) to Egress %s Priority Queue\n", source.cosq,
                           ((target.cosq == BCM_COSQ_HIGH_PRIORITY) ? "High" : "low"));

        rc = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
        if (BCM_FAILURE(rc)) {
            printf("bcm_cosq_fc_path_add failed, CBFC(%d), port_gport(0x%x) err=%d %s\n", nbr_classes, dest_gport, rc, bcm_errmsg(rc));
            return(rc);
        }
    }

    return(rc);
}

int
configure_fc_rx_calender_llfc(int unit, bcm_gport_t dest_gport, bcm_gport_t dest_if_gport,
                                        int is_oob, bcm_gport_t oob_gport, int cal_index)
{
    int                           rc = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;


    /* flow control configuration */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    if (is_oob) {
        source.port = oob_gport;
    }
    else {
        source.port = dest_gport;
    }
    source.calender_index = 0;
    target.port = dest_gport;
    target.cosq = -1;

    rc = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
    if (BCM_FAILURE(rc)) {
        printf("bcm_cosq_fc_path_add failed, LLFC, port_gport(0x%x) err=%d %s\n", dest_gport, rc, bcm_errmsg(rc));
        return(rc);
    }

    return(rc);
}

int
configure_fc_rx_calender_cbfc(int unit, bcm_gport_t dest_gport, bcm_gport_t dest_if_gport,
                              int nbr_intf_classes, int nbr_high_intf_classes,
                              int is_oob, bcm_gport_t oob_gport, int cal_index)
{
    int                           rc = BCM_E_NONE;
    bcm_cosq_fc_endpoint_t        source, target;
    bcm_cosq_fc_direction_type_t  fc_direction = bcmCosqFlowControlReception;
    int                           nbr_classes;
    int                           port;


    /* flow control configuration */
    sal_memset(&source, 0, sizeof(source));
    sal_memset(&target, 0, sizeof(target));

    port = BCM_GPORT_MODPORT_PORT_GET(dest_gport);
    port = (port >= 500) ? (port - 500) : port;

    if (is_oob == FALSE) {
        source.port = dest_gport;
    }
    else {
        source.port = oob_gport;
    }
    target.port = dest_gport;

    for (nbr_classes = 0; nbr_classes < nbr_intf_classes; nbr_classes++) {
        source.calender_index = 1 + (port * 8) + nbr_classes;

        /* determine if it needs to be overridden */
        if (cal_index != -1) {
            source.calender_index = cal_index + nbr_classes;
        }

        if (nbr_classes < nbr_high_intf_classes) {
            target.cosq = BCM_COSQ_HIGH_PRIORITY;
        }
        else {
            target.cosq = BCM_COSQ_LOW_PRIORITY;
        }

        rc = bcm_cosq_fc_path_add(unit, fc_direction, &source, &target);
        if (BCM_FAILURE(rc)) {
             printf("bcm_cosq_fc_path_add failed, CBFC(%d), port_gport(0x%x) err=%d %s\n", nbr_classes, dest_gport, rc, bcm_errmsg(rc));
             return(rc);
        }

        printf("  Mapped FC Calender Entry Index-(%d) to Egress %s Priority Queue\n",
               source.calender_index, ((target.cosq == BCM_COSQ_HIGH_PRIORITY) ? "High" : "low"));
    }

    return(rc);
}


int
fc_rx_run(int unit, int is_llfc, int port_nbr, int cal_index, int ilkn_intf)
{
    int            SOC_PETRA_CARD_NUM_PORTS = 1;
    int            SYSPORT_OFF = 100;
    int            rc = BCM_E_NONE;
    int            my_modid;
    int            idx, port, system_port;
    bcm_gport_t    dest_gport[4];
    bcm_gport_t    sys_gport[4];
    bcm_gport_t    if_gport[4];
    int            flags;
    bcm_port_if_t  intf;
    int            is_intf_calender = FALSE, nbr_intf_classes = 2;
    int            nbr_high_intf_classes;
    int            is_oob = FALSE, oob_port, oob_gport, oob_mode, nbr_port;
    int            ilkn_port = 0, ilkn_mode;


    rc = bcm_stk_modid_get(unit, &my_modid);
    if (BCM_FAILURE(rc)) {
        printf("bcm_stk_modid_get failed err=%d %s\n", rc, bcm_errmsg(rc));
        return(rc);
    }

    for (idx = 0; idx < SOC_PETRA_CARD_NUM_PORTS; idx++) {

        port = idx + 1;
        if (port_nbr != -1) { /* determine if it needs to be overridden */
            port = port_nbr + 1;
        }
        system_port = (SYSPORT_OFF * my_modid) + port;

        printf("Flow Control Reception - port %d\n", port);

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
                nbr_high_intf_classes = nbr_intf_classes/2;
                break;

            case BCM_PORT_IF_XAUI:
                is_intf_calender = FALSE;
                nbr_intf_classes = 2;
                nbr_high_intf_classes = nbr_intf_classes/2;
                break;

            case BCM_PORT_IF_SGMII:
                is_intf_calender = FALSE;
                nbr_intf_classes = 2;
                nbr_high_intf_classes = nbr_intf_classes/2;
                break;

            case BCM_PORT_IF_QSGMII:
                is_intf_calender = FALSE;
                nbr_intf_classes = 2;
                nbr_high_intf_classes = nbr_intf_classes/2;
                break;

            case BCM_PORT_IF_ILKN:
                is_intf_calender = TRUE;
                nbr_intf_classes = 2; /* since there are only 2 Queues */
                nbr_high_intf_classes = nbr_intf_classes/2;
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

            /* check if InBand calender is enabled for reception */
            ilkn_mode = soc_property_port_get(unit, ilkn_port, spn_FC_INBAND_INTLKN_MODE, 0);
            if (ilkn_mode & 0x1) {
                is_oob = FALSE;
            }
            else {
                /* determine which OOB interface is enabled for reception */
                /* pickup the first one that is enabled for reception     */
                for (nbr_port = 0; nbr_port < 2; nbr_port++) {
                    oob_mode = soc_property_port_get(unit, nbr_port, spn_FC_OOB_MODE, 0);
                    if (oob_mode & 0x1) {
                        is_oob = TRUE;
                        break;
                    }
                }
                if (is_oob == TRUE) {
                    oob_port = nbr_port;
                    BCM_GPORT_CONGESTION_SET(oob_gport, oob_port);
                }
                else {
                    printf("error, no OOB interface selected for Flow Control reception\n");
                    return(rc = BCM_E_INTERNAL);
                }
            }
        }

        /* configre packet mapping to egress queues */
        rc = configure_fc_packet_mapping(unit, dest_gport[idx]);
        if (BCM_FAILURE(rc)) {
            return(rc);
        }

        /* flow control configuration */
        if (!is_intf_calender) {
            if (is_llfc) {
                printf("  Non-Calender Interface - LLFC\n");

                rc = configure_fc_rx_non_calender_llfc(unit, dest_gport[idx], if_gport[idx]);
                if (BCM_FAILURE(rc)) {
                    return(rc);
                }
            }
            else {
                printf("  Non-Calender Interface - CBFC\n");

                rc = configure_fc_rx_non_calender_cbfc(unit, dest_gport[idx], if_gport[idx], nbr_intf_classes, nbr_high_intf_classes);
                if (BCM_FAILURE(rc)) {
                    return(rc);
                }
            }
        }
        else {
            if (is_llfc) {
                if (is_oob == FALSE) {
                    printf("error, LLFC configured via SOC properties for Inband interface\n");
                    return(rc = BCM_E_INTERNAL);
                }

                printf("  Calender Interface (%s) - LLFC\n", (is_oob) ? "OOB" : "InBand");

                rc = configure_fc_rx_calender_llfc(unit, dest_gport[idx], if_gport[idx],
                                                   is_oob, oob_gport, cal_index);
                if (BCM_FAILURE(rc)) {
                    return(rc);
                }
            }
            else {
                printf("  Calender Interface (%s) - CBFC\n", (is_oob) ? "OOB" : "InBand");

                rc = configure_fc_rx_calender_cbfc(unit, dest_gport[idx], if_gport[idx],
                                                nbr_intf_classes, nbr_high_intf_classes,
                                                is_oob, oob_gport, cal_index);
                if (BCM_FAILURE(rc)) {
                    return(rc);
                }
            }
        }

        if (port_nbr != -1) {
            break;
        }
    }

    printf("cint_fc_rx.c completed with status (%s)\n", bcm_errmsg(rc));

    return(rc);
}

/* LLFC */
/*   - Non-Calender Interface (InBand) */
/*   - Calender Interface (OOB) */
/*   - NOTE: SOC properties configure Calender Interface (InterLaken) */
/* fc_rx_run(unit, 1, -1, -1, -1); */

/* CBFC */
/*   - Non-Calender Interface (InBand) */
/*   - Calender Interface (InterLaken) */
/*   - Calender Interface (OOB) */
fc_rx_run(unit, 0, -1, -1, -1);

/* CBFC - Interoperability with Shadow */
/* fc_rx_run(unit, 0, 2, 0, 0); */


