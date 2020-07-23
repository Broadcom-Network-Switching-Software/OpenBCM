/*~~~~~~~~~~~~~~~~~~~~~~~~~~~Link Monitoring~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_linkscan.c
 * Purpose:     Example to enable linkscan on a given set of ports.
 * 
 * The link scan feature enables you to monitor the status of ports and links.
 * If the link status changes, the driver calls your callback routine.
 *
 * Cint usage:
 *  *   Create linkscan callback routine that will be called during a port state change event. 
 *      In the cint example, the routine prints the link change event.
 *      linkscan_cb(unit, port, *port_info)
 *
 *  *   Start SW or HW linkscan on a given set of ports. The linkscan thread is started.  
 *      Callback routine is registered called at each port link change event.
 *      linkscan_enable(unit)
 *
 *  *   Disable linkscan. The linkscan thread is stopped. Callback routine is unregistered.  
 *      Port linkscan is removed (set to NONE).
 *      linkscan_disable(unit)
 */



/*
 * Callback routine called during a port state change event.
 */
void
linkscan_cb(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    char messege[70] = {'\0'};
    sprintf(messege, "Link change event on unit %d, port %d, status: %d \n", unit, port,info->linkstatus);
    diag_printk(messege);

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
    int  result;

    /* Start linkscan */
    result = bcm_linkscan_enable_set(unit, usecs);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_linkscan_enable_set unit %d, usecs %d\n", unit, usecs);
        return result;
    }

    /* Register callback routine */
    result = bcm_linkscan_register(unit, linkscan_cb);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_linkscan_register unit %d\n", unit);
        return result;
    }

    return result;
}

/*
 * This routine starts the linkscan thread with
 * the specified time interval.
 */
int
linkscan_stop(int unit)
{
    int         result;

    /* Stop linkscan thread */
    result = bcm_linkscan_enable_set(unit, 0);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_linkscan_enable_set unit %d, usecs 0\n", unit);
    }

    /* Unregister callback routine */
    result = bcm_linkscan_unregister(unit, linkscan_cb);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_linkscan_unregister unit %d\n", unit);
        return result;
    }

    return result;
}

/*
 * Main routine to start SW linkscan on a given set of ports.
 * Linkscan thread is started.
 * Callback routine is registered, called at each port link change event.
 */
int
linkscan_enable(int unit)
{
    int         result;
    bcm_port_t  port;
    bcm_pbmp_t  pbmp;
    

    /* Start linkscan at 250000 usecs */
    result = linkscan_start(unit, 250000);
    if (BCM_FAILURE(result)) {
        return result;
    }

    /* Linkscan can be set per port or per-bitmap */
    /* Set SW linkscan on port 6 */
    port = 6;
    result = bcm_linkscan_mode_set(unit, 6, BCM_LINKSCAN_MODE_SW);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_linkscan_mode_set unit %d, port %d\n", unit, port);
        return result;
    }

    /* Set SW linkscan on given port bitmap (ports 4, 5) */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_SET(pbmp, 4);
    BCM_PBMP_PORT_ADD(pbmp, 5);
    result = bcm_linkscan_mode_set_pbm(unit, pbmp, BCM_LINKSCAN_MODE_SW);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_linkscan_mode_set_pbm unit %d\n", unit);
    }

    return result;
}

/*
 * Main routine to disable linkscan.
 * Linkscan thread is stopped.
 * Callback routine is unregistered.
 * Port linkscan is removed (set to NONE).
 */
int
linkscan_disable(int unit)
{
    int         result;
    bcm_pbmp_t  pbmp;

    /* Stop linkscan thread */
    result = bcm_linkscan_enable_set(unit, 0);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_linkscan_enable_set unit %d, usecs 0\n", unit);
    }

    /* Unregister callback routine */
    result = bcm_linkscan_unregister(unit, linkscan_cb);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_linkscan_unregister unit %d\n", unit);
        return result;
    }

    /* Set linkscan mode to NONE on ports */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_SET(pbmp, 4);
    BCM_PBMP_PORT_ADD(pbmp, 5);
    BCM_PBMP_PORT_ADD(pbmp, 6);
    result = bcm_linkscan_mode_set_pbm(unit, pbmp, BCM_LINKSCAN_MODE_NONE);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_linkscan_mode_set_pbm unit %d\n", unit);
    }

    return result;
}

int 
linkscan_port_get_cb(int unit, bcm_port_t port, int *state)
{
    int result;
    uint32 fault;
    bcm_port_link_state_t link_state;

    result = bcm_port_link_state_get(unit, port, 0, &link_state);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_port_link_status_get unit %d port %d\n", unit, port);
        return result;
    }


    result = bcm_port_fault_get(unit, port, &fault);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_petra_port_fault_get unit %d port %d\n", unit, port);
        return result;
    }

    if((link_state.status == BCM_PORT_LINK_STATUS_DOWN)
       || (fault & BCM_PORT_FAULT_REMOTE)
       || (fault & BCM_PORT_FAULT_LOCAL)) 
    {
        (*state) = 0;
    } else {
        (*state) = 1;
    }

    /*clear indications*/
    result = bcm_port_control_set(unit, port, bcmPortControlLinkFaultLocal, 0);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_port_control_set unit %d port %d\n", unit, port);
        return result;
    }
    result = bcm_port_control_set(unit, port, bcmPortControlLinkFaultRemote, 0);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_port_control_set unit %d port %d\n", unit, port);
        return result;
    }

    return result;
}

int
linkscan_link_func_register(int unit, bcm_port_t port) 
{
    int result;

    result = bcm_linkscan_port_register(unit, port, linkscan_port_get_cb);
    if (BCM_FAILURE(result)) {
        printf("Error, bcm_linkscan_port_register unit %d port %d\n", unit, port);
    }

    return result;
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
        remote_port = remote_port + 256;
        /*printf("Error, unexpected connectivity to FAP, unit %d port %d\n", unit, port);*/
    }
    if (remote_port == BCM_FABRIC_LINK_NO_CONNECTIVITY) {
        remote_port = -1;
    }

    printf("%d\n",remote_port);
    return BCM_E_NONE;

   

}



/*  Checks if link is up
 */

int cint_check_linkscan(int unit ,bcm_port_t port, int enable) {
    
    int rv = BCM_E_NONE;
    

    rv = bcm_port_loopback_set(unit, port, enable);

    if (BCM_FAILURE(rv)) {
        printf("Error, bcm_port_loopback_set unit %d port %d\n", unit, port);
    }

    sal_sleep(1);

    sal_sleep(1);

    sal_sleep(1);

    return BCM_E_NONE;

}

