/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        cint_linkscan.c
 * Purpose:     Example to enable linkscan on a given set of ports.
 *
 */



/*
 * Callback routine called during a port state change event.
 */
int link_status[2048];
int if_print_link_change=1;
int if_check_soc_feature_linkscan_remove_port_info=0;
void
linkscan_cb(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    char test_print[100];
    link_status[port] = info->linkstatus;
    /**
     * DONT Change these prints!
     * The prints are expected strings in the linkscan tests
     */
    if (if_print_link_change) {
        sprintf(test_print, "Link change event on unit %d, port %d, status: %d \n", unit, port, info->linkstatus);
        diag_printk(test_print);
    }
    /* Once this feature enabled, you can expect that info will be all zeroed, except of action_mask field.*/
    if (if_check_soc_feature_linkscan_remove_port_info) {
        printf("check info structure for soc_feature_linkscan_remove_port_info \n");
        int action_mask = info->action_mask;
        if (action_mask != BCM_PORT_ATTR_LINKSTAT_MASK) {
            sprintf(test_print, "Error, info->action_mask in linkscan callback is not 2\n");
            diag_printk(test_print);
            return -1;
        }
        int speed = info->speed;
        int loopback = info->loopback;
        if (speed != 0 && loopback !=0) {
            sprintf(test_print, "Error, info->speed/loopback in linkscan callback is not 0\n");
            diag_printk(test_print);
            return;
        }
    }
    /* Application to process the link change event .... */

    return;
}

/*
 * This routine starts the linkscan thread with
 * the specified time interval.
 */
int
linkscan_start(int unit, int usecs)
{
    char error_msg[200]={0,};

    /* Start linkscan */
    snprintf(error_msg, sizeof(error_msg), "unit %d, usecs %d", unit, usecs);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_enable_set(unit, usecs), error_msg);

    /* Register callback routine */
    snprintf(error_msg, sizeof(error_msg), "unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_register(unit, linkscan_cb), error_msg);

    return BCM_E_NONE;
}
/*
 * This routine starts the linkscan thread with
 * the specified time interval.
 */
int
linkscan_stop(int unit)
{
    char error_msg[200]={0,};

    /* Stop linkscan thread */
    snprintf(error_msg, sizeof(error_msg), "unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_enable_set(unit, 0), error_msg);

    /*Make sure linkscan callback is done*/
    sal_sleep(2);
    /* Unregister callback routine */
    snprintf(error_msg, sizeof(error_msg), "unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_unregister(unit, linkscan_cb), error_msg);

    /*Make sure linkscan task is done*/
    sal_sleep(2);
    return BCM_E_NONE;
}

/*
 * Main routine to start SW linkscan on a given set of ports.
 * Linkscan thread is started.
 * Callback routine is registered, called at each port link change event. 
 *  ##input:
 *    modes:
 *        1. BCM_LINKSCAN_MODE_SW
 *        2. BCM_LINKSCAN_MODE_HW                                                                     .
 *  port1 , port2, port3 to enable linkscan in the above mode.                                                                .
 */
int
linkscan_enable(int unit, int mode, bcm_port_t port1, bcm_port_t port2, bcm_port_t port3)
{
    bcm_port_t  port;
    bcm_pbmp_t  pbmp;
    char error_msg[200]={0,};
    

    /* Start linkscan at 250000 usecs */
    BCM_IF_ERROR_RETURN_MSG(linkscan_start(unit, 250000), "");

    /* Linkscan can be set per port or per-bitmap */
    /* Set SW linkscan on port1 */
    snprintf(error_msg, sizeof(error_msg), "unit %d, port %d", unit, port1);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_mode_set(unit, port1, mode), error_msg);

    /* Set SW linkscan on given port bitmap (port2, port3) */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_SET(pbmp, port2);
    BCM_PBMP_PORT_ADD(pbmp, port3);
    snprintf(error_msg, sizeof(error_msg), "unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_mode_set_pbm(unit, pbmp, mode), error_msg);

    return BCM_E_NONE;
}

/*
 * Main routine to disable linkscan.
 * Linkscan thread is stopped.
 * Callback routine is unregistered.
 * Port linkscan is removed (set to NONE). 
 * port1 , port2, port3 to diable linkscan in the above mode.                                      .
 */
int
linkscan_disable(int unit, bcm_port_t port1, bcm_port_t port2, bcm_port_t port3)
{
    bcm_pbmp_t  pbmp;
    char error_msg[200]={0,};

    /* Stop linkscan thread */
    snprintf(error_msg, sizeof(error_msg), "unit %d, usecs 0", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_enable_set(unit, 0), error_msg);

    /*Make sure linkscan callback is done*/
    sal_sleep(2);
    /* Unregister callback routine */
    snprintf(error_msg, sizeof(error_msg), "unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_unregister(unit, linkscan_cb), error_msg);
    /*Make sure linkscan task is done*/
    sal_sleep(2);

    /* Set linkscan mode to NONE on ports */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_SET(pbmp, port1);
    BCM_PBMP_PORT_ADD(pbmp, port2);
    BCM_PBMP_PORT_ADD(pbmp, port3);
    snprintf(error_msg, sizeof(error_msg), "unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_mode_set_pbm(unit, pbmp, BCM_LINKSCAN_MODE_NONE), error_msg);

    return BCM_E_NONE;
}




/*  Checks if link is up
 */

int cint_check_link_up(int unit ,bcm_port_t port) {
    bcm_fabric_link_connectivity_t link_partner_info;
    int remote_port;
    char error_msg[200]={0,};
    

    snprintf(error_msg, sizeof(error_msg), "unit %d port %d", unit, port);
    BCM_IF_ERROR_RETURN_MSG(bcm_fabric_link_connectivity_status_single_get(unit, port, link_partner_info), error_msg);

    remote_port = link_partner_info.link_id;

    if (link_partner_info.device_type == bcmFabricDeviceTypeFAP) {
        remote_port = -1;
        printf("Error, unexpected connectivity to FAP, unit %d port %d\n", unit, port);
    }
    if (remote_port == BCM_FABRIC_LINK_NO_CONNECTIVITY) {
        remote_port = -1;
    }

    printf("%d\n",remote_port);
    return BCM_E_NONE;
}


/*below code for linkscan performance test
 record time to lkscan_perf_cb_time1 before link status change, then change link status
 record time to lkscan_perf_cb_time1 in callback function, time2-time1 is the linkscan performance*/
unsigned int lkscan_perf_cb_time1, lkscan_perf_cb_time2;
int lkscan_perf_cb_port;
unsigned int up_callback_timer,down_callback_timer;

/*
 * Callback routine called for performance test
 */
void
linkscan_cb_perf_test(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    /*only record time for specified port status change*/
    if (lkscan_perf_cb_port == port) {
        lkscan_perf_cb_time2 =sal_time_usecs();
    }
    printf("Link change event on unit %d, port %d, status: %d \n", unit, port, info->linkstatus);
}

/*
 * This routine starts the linkscan performance for
 * the specified port.
 */
int
linkscan_perf_start(int unit, bcm_port_t port)
{
    int i;
    bcm_port_phy_tx_t tx;
    char error_msg[200]={0,};
    /* Register callback routine */
    snprintf(error_msg, sizeof(error_msg), "unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_register(unit, linkscan_cb_perf_test), error_msg);
    printf("registered cb done\n");
    snprintf(error_msg, sizeof(error_msg), "port %d - unit %d", port, unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_phy_tx_get(unit, port, &tx), error_msg);
    snprintf(error_msg, sizeof(error_msg), "port %d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, port, 0), error_msg);
    printf("Finished, bcm_port_enable_set 0\n");
    sal_sleep(2);
    lkscan_perf_cb_port = port;

    /*test link up*/
    lkscan_perf_cb_time2 = 0;
    lkscan_perf_cb_time1 = sal_time_usecs();
    snprintf(error_msg, sizeof(error_msg), "port %d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, port, 1), error_msg);
    printf("Finished, bcm_port_enable_set 1\n");
    if (tx.signalling_mode == bcmPortPhySignallingModePAM4) {
        sal_sleep(6);
    } else {
        sal_sleep(3);
    }
    for (i=0; i<50; i++) {
        sal_usleep(100000);
        if (lkscan_perf_cb_time2 != 0) {
            up_callback_timer = lkscan_perf_cb_time2 - lkscan_perf_cb_time1;
            break;
        } else {
            up_callback_timer = -1;
        }
    }

    /*test link down*/
    lkscan_perf_cb_time2 = 0;
    lkscan_perf_cb_time1 = sal_time_usecs();
    snprintf(error_msg, sizeof(error_msg), "port %d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(unit, port, 0), error_msg);
    printf("Finished, bcm_port_enable_set 0\n");
    sal_sleep(1);
    for (i=0; i<50; i++) {
        sal_usleep(100000);
        if (lkscan_perf_cb_time2 != 0) {
            down_callback_timer = lkscan_perf_cb_time2 - lkscan_perf_cb_time1;
            break;
        } else {
            down_callback_timer = -1;
        }
    }
    return BCM_E_NONE;
}

/*
 * This routine starts the linkscan performance for multiple device.
 * Port(in unit) and peer port(in peer_unit) are connected.
 * You can get the port linkscan callback performace time by enable/disable peer_port.
 */
int
linkscan_perf_start2(int unit, bcm_port_t port, int peer_unit, bcm_port_t peer_port)
{
    int i;
    char error_msg[200]={0,};
    /* Register callback routine */
    snprintf(error_msg, sizeof(error_msg), "unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_register(unit, linkscan_cb_perf_test), error_msg);
    sal_sleep(2);
    printf("Finished, bcm_linkscan_register unit %d\n", unit);
    snprintf(error_msg, sizeof(error_msg), "port %d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(peer_unit, peer_port, 0), error_msg);
    printf("Finished, bcm_port_enable_set 0\n");
    sal_sleep(1);
    sal_usleep(1000000);
    lkscan_perf_cb_port = port;

    /*test link up*/
    lkscan_perf_cb_time2 = 0;
    lkscan_perf_cb_time1 = sal_time_usecs();
    snprintf(error_msg, sizeof(error_msg), "port %d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(peer_unit, peer_port, 1), error_msg);
    printf("Finished, bcm_port_enable_set 1\n");
    sal_sleep(3);
    for (i=0; i<50; i++) {
        sal_usleep(100000);
        if (lkscan_perf_cb_time2 != 0) {
            up_callback_timer = lkscan_perf_cb_time2 - lkscan_perf_cb_time1;
            break;
        } else {
            up_callback_timer = -1;
        }
    }

    /*test link down*/
    lkscan_perf_cb_time2 = 0;
    lkscan_perf_cb_time1 = sal_time_usecs();
    snprintf(error_msg, sizeof(error_msg), "port %d", port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_enable_set(peer_unit, peer_port, 0), error_msg);
    printf("Finished, bcm_port_enable_set 0\n");
    sal_sleep(1);
    for (i=0; i<50; i++) {
        sal_usleep(100000);
        if (lkscan_perf_cb_time2 != 0) {
            down_callback_timer = lkscan_perf_cb_time2 - lkscan_perf_cb_time1;
            break;
        } else {
            down_callback_timer = -1;
        }
    }
    return BCM_E_NONE;
}

/*
 * This routine unregister callback routine
 * stop the linkscan performace test.
 */
int
linkscan_perf_stop(int unit)
{
    char error_msg[200]={0,};
    /*Make sure linkscan callback is done*/
    sal_sleep(2);
    /* Unregister callback routine */
    snprintf(error_msg, sizeof(error_msg), "unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_unregister(unit, linkscan_cb_perf_test), error_msg);
    printf("unregistered cb done\n");
    /*Make sure linkscan task is done*/
    sal_sleep(2);
    return BCM_E_NONE;
}

/*below code to monitor port status change for all ports
record link status change into array */
int link_status_monitor[2048][10];
int linkscan_on_flexe_phy = 0;

/*
 * Callback routine called for port status monitor test
 */
void
linkscan_cb_port_monitor_test(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int i;
    /* 'offset' indicates unit id when linkscan_on_flexe_phy set */
    int offset = linkscan_on_flexe_phy ? unit : 0;

    for (i=0; i<10; i++) {
        if (link_status_monitor[port + offset][i] == -1) {
            link_status_monitor[port + offset][i] = info->linkstatus;
            break;
        }
    }
    printf("Link change event on unit %d, port %d, status: %d \n", unit, port, info->linkstatus);
    return;
}

/*
 * This routine clear the port status data for
 * all ports.
 */
void
linkscan_port_monitor_clear_data ()
{
    int i;
    for (i=0; i<2048; i++) {
        int j;
        for (j=0; j<10; j++) {
            link_status_monitor[i][j] = -1;
        }
    }
}


/*
 * This routine starts the port status monitor for
 * all ports.
 */
int
linkscan_port_monitor_start(int unit)
{
    int i;
    char error_msg[200]={0,};
    /* Register callback routine */
    snprintf(error_msg, sizeof(error_msg), "unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_register(unit, linkscan_cb_port_monitor_test), error_msg);
    linkscan_port_monitor_clear_data();
    return BCM_E_NONE;
}
/*
 * This routine unregister callback routine
 * stop the port status monitor test.
 */
int
linkscan_port_monitor_stop(int unit)
{
    char error_msg[200]={0,};
    /*Make sure linkscan callback is done*/
    sal_sleep(2);
    snprintf(error_msg, sizeof(error_msg), "unit %d", unit);
    BCM_IF_ERROR_RETURN_MSG(bcm_linkscan_unregister(unit, linkscan_cb_port_monitor_test), error_msg);

    /*Make sure linkscan task is done*/
    sal_sleep(2);
    return BCM_E_NONE;
}

