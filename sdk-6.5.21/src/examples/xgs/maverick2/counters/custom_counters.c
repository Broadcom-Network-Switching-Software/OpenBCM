/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Custom Counters
 *                            
 *  Usage    : BCM.0> cint custom_counters.c
 *                                          
 *  config   : counters_config.bcm       
 *                                          
 *  Log file : custom_counters_log.txt      
 *                                          
 *  Test Topology :                         
 *                                          
 *                   +------------------------------+  ingress_port
 *                   |                              +-----------------+
 *                   |                              |                  
 *                   |                              |                  
 *  cpu_port         |                              |                  
 *  +----------------+          SWITCH              +                  
 *                   |                              |                  
 *                   |                              |                  
 *                   |                              |  egress_port     
 *                   |                              +-----------------+
 *                   |                              |                  
 *                   +------------------------------+                  
 *  Summary:
 *  ========
 *    This CINT script demonstrates how to use custom counters to account for
 *    certain type of packets ingressing/egressing the switch.
 *
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one ingress, one egress port and configure them in loopback mode.
 *      b) Modify DOS_CONTROL register to set MACSA_EQUALS_MACDA_DROP=1.
 *
 *    2) Step2 - Configuration (Done in custom_counters_setup())
 *    ==========================================================
 *      a) Delete any custom counter associated with snmpBcmCustomReceive0
 *      b) Add bcmDbgCntMACSAEQUALMACDA custom counter to snmpBcmCustomReceive0   
 *
 *    3) Step3 - Verification(Done in verify())
 *    ==========================================
 *      a) Transmit 10 packets with same source and destination mac address
 *      b) Read the custom counter using bcm_stat_get() API and print.
 *      c) Expected Result:
 *      ===================
 *         The custom counter value should be 10. 
*/

cint_reset();
bcm_port_t egress_port, ingress_port;

/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports
*/
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{

    int i=0,port=0;
    bcm_error_t rv = BCM_E_NONE;
    bcm_port_config_t configP;
    bcm_pbmp_t ports_pbmp;

    rv = bcm_port_config_get(unit,&configP);
    if(BCM_FAILURE(rv)) {
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    ports_pbmp = configP.e;
    for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
        if (BCM_PBMP_MEMBER(&ports_pbmp,i)&& (port < num_ports)) {
            port_list[port]=i;
            port++;
        }
    }

    if (( port == 0 ) || ( port != num_ports )) {
        printf("portNumbersGet() failed \n");
        return -1;
    }

    return rv;

}

bcm_error_t test_setup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Pick arbitrary dest MAC Address */
    int port_list[2];

    rv = portNumbersGet(unit, port_list, 2);

    if (BCM_FAILURE(rv)) {
        printf("portNumbersGet() failed\n");
        return rv;
    }

    egress_port = port_list[0];
    ingress_port = port_list[1];

    /* Put all ports into loopback */
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, egress_port, BCM_PORT_LOOPBACK_MAC));
    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, ingress_port, BCM_PORT_LOOPBACK_MAC));

    /* setp MACSAEQUALMAC custom counter */
    bshell(unit, "modreg DOS_CONTROL MACSA_EQUALS_MACDA_DROP=1");
    
    return rv;
}


bcm_error_t
custom_counters_setup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;

    rv = bcm_stat_custom_delete_all(unit, egress_port, snmpBcmCustomReceive8);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_stat_custom_delete_all(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_stat_custom_add(unit, egress_port, snmpBcmCustomReceive8, bcmDbgCntMACSAEQUALMACDA);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_stat_custom_add(): %s.\n",bcm_errmsg(rv));
        return rv;
    }
    return rv;
}    

bcm_error_t verify(int unit)
{
    uint64 val;
    char        command[128];   /* Buffer for diagnostic shell commands */
    bcm_error_t rv = BCM_E_NONE; 
   
    /* send packets with SourceMac=DestMac */
    sprintf(command, "tx 10 pbm=%d SourceMac=00:00:00:00:00:10 DestMac=00:00:00:00:00:10 length=128" , egress_port);
    bshell(unit, command);
    bshell(unit, "sleep quiet 2");
    
    /* get the counter stat */
    rv = bcm_stat_get(unit, egress_port, snmpBcmCustomReceive8, &val);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_stat_get(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    printf("Custom Counter :");
    print val;
    
    return rv;
}


bcm_error_t execute()
{
    bcm_error_t rv;
    int unit =0;
    bshell(unit, "config show; a ; version");
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = custom_counters_setup(unit)))) {
        printf("Custom Counters Setup Failed\n");
        return -1;
    }

    rv = verify(unit);
    return rv;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

