/*
 * $Id: linkscan.c$
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenSDK/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * cint script for linkscan functionality
 */

/* Feature  : linkscan
 *
 * Usage    : BCM.0>c linkscan.c
 * Config   : linkscan_config.bcm
 *
 * Log file : linkscan_log.txt
 *
 * Test Topology : Nothing in specific
 *
 * Summary:
 * ========
 * The link scan feature enables to monitor the status of ports and links.
 * If the link status changes, the driver calls the callback routine.
 *
 * Detailed steps done in the CINT script:
 * =======================================
 * 1) Step1 - Test Setup(Done in test_setup())
 * ===========================================
 *  a) Select 3 ports on the switch  
 *
 * 2) Step2 - Configuration(Done in linkscan_config())
 * ===================================================
 *  a) Start linkscan
 *  b) Register linkscan callback
 *  c) Add one port in software linkscan
 *  d) Add two ports in a bitmap and enable software linkscan on this bitmap
 *
 * 3) Step3 - Verification (Done in verify())
 * ==========================================
 *  a) Enable loopback on port1
 *  b) Enable loopback on port2 and register port callback on port2
 *  b) Expected Result:
 *  ====================
 *  Check that linkscan callback is seen on the console
 */


cint_reset();
int port1, port2, port3;


/*
 * This function is written so that hardcoding of port
 * numbers in cint scripts is removed. This function gives
 * required number of ports
 */
bcm_error_t
port_numbers_get(int unit, int *port_list, int num_ports)
{
    int i = 0, port = 0, rv = 0;
    bcm_port_config_t config_p;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit, &config_p);
    if(BCM_FAILURE(rv)){
        printf("Error in retrieving port configuration on unit %d, %s\n", unit, bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = config_p.e;
    for(i = 1; i < BCM_PBMP_PORT_MAX; i++){
        if(BCM_PBMP_MEMBER(&ports_pbmp,i) && (port < num_ports)){
            port_list[port]=i;
            port++;
        }
    }

    if((port == 0) || (port != num_ports)){
        printf("portNumbersGet() failed \n");
        return -1;
    }

    return BCM_E_NONE;
}


/*
 * This functions gets the port numbers to enable linkscan
 */
bcm_error_t
test_setup(int unit)
{
    int port_list[3], i;

    if(BCM_E_NONE != port_numbers_get(unit, port_list, 3)){
        printf("portNumbersGet() failed\n");
        return -1;
    }

    port1 = port_list[0];
    port2 = port_list[1];
    port3 = port_list[2];

    return BCM_E_NONE;
}


/*
 * Callback routine called during a port state change event.
 */
void
linkscan_cb(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    printf("Link change event on unit %d, port %d, status: %d \n", unit, port, info->linkstatus);

    return;
}


/*
 * Callback routine called during a port state change event.
 */
int
linkscan_port_get_cb(int unit, bcm_port_t port, int *state)
{
    int rv;
    uint32 fault;
    int status;

    printf("linkscan_port_get_cb on unit %d, port %d\n", unit, port);

    rv = bcm_port_fault_get(unit, port, &fault);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_fault_get on unit %d, port %d\n", unit, port, bcm_errmsg(rv));
        return rv;
    }

    if((status == BCM_PORT_LINK_STATUS_DOWN)
       || (fault & BCM_PORT_FAULT_REMOTE)
       || (fault & BCM_PORT_FAULT_LOCAL)){
        (*state) = 0;
    } else{
        (*state) = 1;
    }

    return rv;
}


/*
 * Register callback for a port link change event
 */
int
linkscan_link_func_register(int unit, bcm_port_t port)
{
    int rv;

    rv = bcm_linkscan_port_register(unit, port, linkscan_port_get_cb);
    if(BCM_FAILURE(rv)){
        printf("Error, bcm_linkscan_port_register unit %d port %d\n", unit, port);
    }

    return rv;
}


/*
 * Main routine to start SW linkscan on a given set of ports.
 * Linkscan thread is started.
 * Callback routine is registered, called at each port link change event.
 */
int
linkscan_config(int unit)
{
    int rv;
    bcm_pbmp_t  pbmp;

    /* Start linkscan at 250000 usecs */
    rv = bcm_linkscan_enable_set(unit, 250000);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_linkscan_enable_set on unit %d, %s\n", unit, bcm_errmsg(rv));
        return rv;
    }

    /* Register callback routine */
    rv = bcm_linkscan_register(unit, linkscan_cb);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_linkscan_register on unit %d, %s\n", unit, bcm_errmsg(rv));
        return rv;
    }

    /* Linkscan can be set per port or per-bitmap */
    /* Set SW linkscan on one of the ports */
    rv = bcm_linkscan_mode_set(unit, port1, BCM_LINKSCAN_MODE_SW);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_linkscan_mode_set on unit %d, port1 %d, %s\n", unit, port1, bcm_errmsg(rv));
        return rv;
    }

    /* Set SW linkscan on given port bitmap (ports 2, 3) */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_SET(pbmp, port2);
    BCM_PBMP_PORT_ADD(pbmp, port3);
    rv = bcm_linkscan_mode_set_pbm(unit, pbmp, BCM_LINKSCAN_MODE_SW);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_linkscan_mode_set_pbm on unit %d, %s\n", unit, bcm_errmsg(rv));
    }

    linkscan_link_func_register(0, port2);

    return rv;
}


/*
 * This function sets the port in loopback and checks if callback prints are seen on console
 */
int
verify(int unit)
{
    int rv = BCM_E_NONE;
    int enable = 1;
    int disable = 0;
    int status;

    rv = bcm_port_loopback_set(unit, port1, enable);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_loopback_set on unit %d, port %d %s\n", unit, port, bcm_errmsg(rv));
        return rv;
    }
    sal_sleep(1);

    rv = bcm_port_link_status_get(unit, port1, &status);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_link_status_get port %d %s\n", port1, bcm_errmsg(rv));
    }
    printf("port %d status on unit %d, is %d\n", port1, unit, status);

    rv = bcm_port_loopback_set(unit, port1, disable);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_loopback_set on unit %d, port %d %s\n", unit, port1, bcm_errmsg(rv));
        return rv;
    }
    sal_sleep(1);

    rv = bcm_port_link_status_get(unit, port1, &status);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_link_status_get on unit %d, port %d %s\n", unit, port1, bcm_errmsg(rv));
    }
    printf("port %d status on unit %d, is %d\n", port1, unit, status);

    rv = bcm_port_loopback_set(unit, port2, enable);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_loopback_set on unit %d, port %d %s\n", unit, port2, bcm_errmsg(rv));
        return rv;
    }
    sal_sleep(1);

    rv = bcm_port_link_status_get(unit, port2, &status);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_link_status_get on unit %d, port %d %s\n", unit, port2, bcm_errmsg(rv));
    }
    printf("port %d status on unit %d, is %d\n", port2, unit, status);

    rv = bcm_port_loopback_set(unit, port2, disable);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_loopback_set on unit %d, port %d %s\n", unit, port2, bcm_errmsg(rv));
        return rv;
    }
    sal_sleep(1);

    rv = bcm_port_link_status_get(unit, port2, &status);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_port_link_status_get on unit %d, port %d %s\n", unit, port2, bcm_errmsg(rv));
    }
    printf("port %d status on unit %d, is %d\n", port2, unit, status);

    return BCM_E_NONE;
}


/*
 * Main routine to disale linkscan.
 * Linkscan thread is stopped.
 * Callback routine is unregistered.
 * Port linkscan is removed (set to NONE).
 */
int
linkscan_disable(int unit)
{
    int rv;
    bcm_pbmp_t pbmp;

    /* Stop linkscan thread */
    rv = bcm_linkscan_enable_set(unit, 0);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_linkscan_enable_set on unit %d, %s\n", unit, bcm_errmsg(rv));
    }

    /* Unregister callback routine */
    rv = bcm_linkscan_unregister(unit, linkscan_cb);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_linkscan_unregister on unit %d, %s\n", unit, bcm_errmsg(rv));
        return rv;
    }

     /* Unregister port callback routine */
     rv = bcm_linkscan_port_unregister(unit, port2, linkscan_port_get_cb);
     if(BCM_FAILURE(rv)){
         printf("Error bcm_linkscan_port_unregister on unit %d, %s\n", unit, bcm_errmsg(rv));
         return rv;
     }

    /* Set linkscan mode to NONE on ports */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_SET(pbmp, port1);
    BCM_PBMP_PORT_ADD(pbmp, port2);
    BCM_PBMP_PORT_ADD(pbmp, port3);
    rv = bcm_linkscan_mode_set_pbm(unit, pbmp, BCM_LINKSCAN_MODE_NONE);
    if(BCM_FAILURE(rv)){
        printf("Error bcm_linkscan_mode_set_pbm on unit %d, %s\n", unit, bcm_errmsg(rv));
    }

    return rv;
}

/* Main function to run the cint script and check the results */
bcm_error_t
execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; a ; version");
    if(BCM_FAILURE((rv = test_setup(unit)))){
        printf("test_setup() failed\n");
        return -1;
    }

    if(BCM_FAILURE((rv = linkscan_config(unit)))){
        printf("linkscan_config() failed\n");
        return -1;
    }

    if(BCM_FAILURE(rv = verify(unit))){
        printf("verify() failed\n");
        return -1;
    }

    linkscan_disable(unit);
    return BCM_E_NONE;
}


const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

