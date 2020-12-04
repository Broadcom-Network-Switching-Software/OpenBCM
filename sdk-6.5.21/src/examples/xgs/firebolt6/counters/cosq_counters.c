/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Cosq Counters
 *                            
 *  Usage    : BCM.0> cint cosq_counters.c
 *                                          
 *  config   : counters_config.bcm       
 *                                          
 *  Log file : cosq_counters_log.txt      
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
 *    This CINT script demonstrates how to read Cosq counters
 *    associated with a particular port.
 *
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select one ingress, one egress port and configure them in loopback mode.
 *
 *    2) Step2 - Configuration (Done in cosq_counters_setup())
 *    ==========================================================
 *      a) Noting to be done in cosq_counters_setup() 
 *
 *    3) Step3 - Verification(Done in verify())
 *    ==========================================
 *      a) Transmit total 6 packets packets, 2 packets for each cos queue 0,1,2. 
 *      b) Read the cosq counters using bcm_cosq_stat_get32() API and print.
 *      c) Expected Result:
 *      ===================
 *         cosq counters for queue 0,1,2 should show 2 packets each.   
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

    return rv;
}


bcm_error_t
cosq_counters_setup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    return rv;
}    

bcm_error_t verify(int unit)
{
    char        command[128];   /* Buffer for diagnostic shell commands */
    bcm_error_t rv = BCM_E_NONE; 
    uint32      cosqOutPkts[3];
    uint32      cosqOutBytes[3];
    int         i=0;
   
    /* send 6 packets */
    sprintf(command, "tx 2 CosQueue=0 SourceMac=00:00:00:00:10:10 DestMac=00:00:00:00:00:10 length=128");
    bshell(unit, command);
    
    sprintf(command, "tx 2 CosQueue=1 SourceMac=00:00:00:00:10:10 DestMac=00:00:00:00:00:10 length=128");
    bshell(unit, command);
    
    sprintf(command, "tx 2 Cosqueue=2 SourceMac=00:00:00:00:10:10 DestMac=00:00:00:00:00:10 length=128");
    bshell(unit, command);
   
    bshell(unit, "sleep quiet 2");
 
    /* get the cosq stats */
    for (i=0; i<3; i++) { 
        rv = bcm_cosq_stat_get32(unit, egress_port,i,bcmCosqStatOutPackets, &cosqOutPkts[i]);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_cosq_stat_get32(): %s.\n",bcm_errmsg(rv));
            return rv;
        }
        
        rv = bcm_cosq_stat_get32(unit, egress_port,i,bcmCosqStatOutBytes, &cosqOutBytes[i]);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_cosq_stat_get32(): %s.\n",bcm_errmsg(rv));
            return rv;
        }
    }

    for (i=0; i<3; i++) { 
        printf("Port:%2d COS:%2d Out Packets: %d  Out Bytes: %d \n", egress_port,
               i, cosqOutPkts[i] , cosqOutBytes[i]);
    } 
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

    if (BCM_FAILURE((rv = cosq_counters_setup(unit)))) {
        printf("Cosq Counters Setup Failed\n");
        return -1;
    }

    rv = verify(unit);
    return rv;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

