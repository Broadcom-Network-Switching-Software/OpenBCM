/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_linkscan.c
 * Purpose:     Example to enable linkscan on a given set of ports.
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
            return -1;
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
    int rv;

    /* Start linkscan */
    rv = bcm_linkscan_enable_set(unit, usecs);
    if (rv<0) {
        printf("Error, bcm_linkscan_enable_set unit %d, usecs %d\n",
               unit, usecs);
        return rv;
    }

    /* Register callback routine */
    rv = bcm_linkscan_register(unit, linkscan_cb);
    if (rv<0) {
        printf("Error, bcm_linkscan_register unit %d\n", unit);
        return rv;
    }

    return rv;
}
/*
 * This routine starts the linkscan thread with
 * the specified time interval.
 */
int
linkscan_stop(int unit)
{
    int rv;

    /* Stop linkscan thread */
    rv = bcm_linkscan_enable_set(unit, 0);
    if (rv<0) {
        printf("Error, bcm_linkscan_enable_set unit %d, usecs 0\n", unit);
    }

    /* Unregister callback routine */
    rv = bcm_linkscan_unregister(unit, linkscan_cb);
    if (rv<0) {
        printf("Error, bcm_linkscan_unregister unit %d\n", unit);
        return rv;
    }

    return rv;
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
    int rv;
    bcm_port_t  port;
    bcm_pbmp_t  pbmp;
    

    /* Start linkscan at 250000 usecs */
    rv = linkscan_start(unit, 250000);
    if (rv<0) {
        return rv;
    }

    /* Linkscan can be set per port or per-bitmap */
    /* Set SW linkscan on port1 */
    rv = bcm_linkscan_mode_set(unit, port1, mode);
    if (rv<0) {
        printf("Error, bcm_linkscan_mode_set unit %d, port %d\n", unit, port1);
        return rv;
    }

    /* Set SW linkscan on given port bitmap (port2, port3) */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_SET(pbmp, port2);
    BCM_PBMP_PORT_ADD(pbmp, port3);
    rv = bcm_linkscan_mode_set_pbm(unit, pbmp, mode);
    if (rv<0) {
        printf("Error, bcm_linkscan_mode_set_pbm unit %d\n", unit);
    }

    return rv;
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
    int rv;
    bcm_pbmp_t  pbmp;

    /* Stop linkscan thread */
    rv = bcm_linkscan_enable_set(unit, 0);
    if (rv<0) {
        printf("Error, bcm_linkscan_enable_set unit %d, usecs 0\n", unit);
    }

    /* Unregister callback routine */
    rv = bcm_linkscan_unregister(unit, linkscan_cb);
    if (rv<0) {
        printf("Error, bcm_linkscan_unregister unit %d\n", unit);
        return rv;
    }

    /* Set linkscan mode to NONE on ports */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_SET(pbmp, port1);
    BCM_PBMP_PORT_ADD(pbmp, port2);
    BCM_PBMP_PORT_ADD(pbmp, port3);
    rv = bcm_linkscan_mode_set_pbm(unit, pbmp, BCM_LINKSCAN_MODE_NONE);
    if (rv<0) {
        printf("Error, bcm_linkscan_mode_set_pbm unit %d\n", unit);
    }

    return rv;
}




/*  Checks if link is up
 */

int cint_check_link_up(int unit ,bcm_port_t port) {
    bcm_fabric_link_connectivity_t link_partner_info;
    int remote_port;
    int rv;
    

    rv = bcm_fabric_link_connectivity_status_single_get(unit, port, link_partner_info);
    if (BCM_FAILURE(rv)) {
        printf("Error, bcm_fabric_link_connectivity_status_single_get unit %d port %d\n", unit, port);
    }

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
    return;
}

/*
 * This routine starts the linkscan performance for
 * the specified port.
 */
int
linkscan_perf_start(int unit, bcm_port_t port)
{
    int  rv;
    int i;
    /* Register callback routine */
    rv = bcm_linkscan_register(unit, linkscan_cb_perf_test);
    if (rv<0) {
        printf("Error, bcm_linkscan_register unit %d\n", unit);
        return rv;
    }
    rv = bcm_port_enable_set(unit, port, 0);
    if (rv<0) {
        printf("Error, bcm_port_loopback_set port %d\n", port);
        return rv;
    }
    sal_usleep(1000000);
    lkscan_perf_cb_port = port;

    /*test link up*/
    lkscan_perf_cb_time2 = 0;
    lkscan_perf_cb_time1 = sal_time_usecs();
    rv = bcm_port_enable_set(unit, port, 1);
    if (rv<0) {
        printf("Error, bcm_port_loopback_set port %d\n", port);
        return rv;
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
    rv = bcm_port_enable_set(unit, port, 0);
    if (rv<0) {
        printf("Error, bcm_port_loopback_set port %d\n", port);
        return rv;
    }
    for (i=0; i<50; i++) {
        sal_usleep(100000);
        if (lkscan_perf_cb_time2 != 0) {
            down_callback_timer = lkscan_perf_cb_time2 - lkscan_perf_cb_time1;
            break;
        } else {
            down_callback_timer = -1;
        }
    }
    return rv;
}

/*
 * This routine starts the linkscan performance for multiple device.
 * Port(in unit) and peer port(in peer_unit) are connected.
 * You can get the port linkscan callback performace time by enable/disable peer_port.
 */
int
linkscan_perf_start2(int unit, bcm_port_t port, int peer_unit, bcm_port_t peer_port)
{
    int  rv;
    int i;
    /* Register callback routine */
    rv = bcm_linkscan_register(unit, linkscan_cb_perf_test);
    if (rv<0) {
        printf("Error, bcm_linkscan_register unit %d\n", unit);
        return rv;
    }
    rv = bcm_port_enable_set(peer_unit, peer_port, 0);
    if (rv<0) {
        printf("Error, bcm_port_loopback_set port %d\n", port);
        return rv;
    }
    sal_usleep(1000000);
    lkscan_perf_cb_port = port;

    /*test link up*/
    lkscan_perf_cb_time2 = 0;
    lkscan_perf_cb_time1 = sal_time_usecs();
    rv = bcm_port_enable_set(peer_unit, peer_port, 1);
    if (rv<0) {
        printf("Error, bcm_port_loopback_set port %d\n", port);
        return rv;
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
    rv = bcm_port_enable_set(peer_unit, peer_port, 0);
    if (rv<0) {
        printf("Error, bcm_port_loopback_set port %d\n", port);
        return rv;
    }
    for (i=0; i<50; i++) {
        sal_usleep(100000);
        if (lkscan_perf_cb_time2 != 0) {
            down_callback_timer = lkscan_perf_cb_time2 - lkscan_perf_cb_time1;
            break;
        } else {
            down_callback_timer = -1;
        }
    }
    return rv;
}

/*
 * This routine unregister callback routine
 * stop the linkscan performace test.
 */
int
linkscan_perf_stop(int unit)
{
    int rv;
    /* Unregister callback routine */
    rv = bcm_linkscan_unregister(unit, linkscan_cb_perf_test);
    if (rv<0) {
        printf("Error, bcm_linkscan_unregister unit %d\n", unit);
        return rv;
    }

    return rv;
}

/*below code to monitor port status change for all ports
record link status change into array */
int link_status_monitor[2048][10];
/*
 * Callback routine called for port status monitor test
 */
void
linkscan_cb_port_monitor_test(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int i;
    for (i=0; i<10; i++) {
        if (link_status_monitor[port][i] == -1) {
            link_status_monitor[port][i] = info->linkstatus;
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
int
linkscan_port_monitor_clear_data ()
{
    int i,j;
    for (i=0; i<2048; i++) {
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
    int  rv;
    int i;
    /* Register callback routine */
    rv = bcm_linkscan_register(unit, linkscan_cb_port_monitor_test);
    if (rv<0) {
        printf("Error, bcm_linkscan_register unit %d\n", unit);
        return rv;
    }
    linkscan_port_monitor_clear_data();
    return rv;
}
/*
 * This routine unregister callback routine
 * stop the port status monitor test.
 */
int
linkscan_port_monitor_stop(int unit)
{
    int rv=0;
    rv = bcm_linkscan_unregister(unit, linkscan_cb_port_monitor_test);
    if (rv<0) {
        printf("Error, bcm_linkscan_unregister unit %d\n", unit);
        return rv;
    }

    return rv;
}

