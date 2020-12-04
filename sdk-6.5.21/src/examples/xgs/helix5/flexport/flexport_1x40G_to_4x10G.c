/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*  Feature  : Flexport
 *                   
 *  Usage    : BCM.0> cint flexport_1x40G_to_4x10G.c
 *                                 
 *  config   : flexport_config.bcm   
 *                                 
 *  Log file : flexport_1x40G_to_4x10G_log.txt      
 *                                 
 *  Test Topology :                
 *                                 
 *                   +------------------------------+  
 *                   |                              +
 *                   |                              |                  
 *                   |                              |                  
 *                   |                              |  port1    
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |                  
 *                   |                              |                  
 *                   |                              |      
 *                   |                              +
 *                   |                              |                  
 *                   +------------------------------+                  
 *  Summary:                                                           
 *  ========                                                           
 *    This CINT script demonstrates how to flex one 400G port into four 100G ports from application 
 *    using bcm APIs.
 *
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select a 40G port.
 *
 *    2) Step2 - Configuration (Done in flexport_setup())
 *    ============================================
 *      a) Disable linkscan for the selected 40G port.
 *      b) Disable the selected 40G port. 
 *
 *    3) Step3 - Verification(Done in verify())
 *    ==========================================
 *      a) Dispaly port information before flex operation.
 *      b) Flex the selected 40G port. 
 *      c) Enable linkscan for the new 10G flexed ports.
 *      d) Enable newly 10G flexed ports.
 *      d) Expected Result:
 *      ===================
 *         The 40G port will get flexed to four 10G ports. ps command output will show 
 *         new 4 10G ports.
 */

cint_reset();
int logical_port = 24;

/* The loggical to physical port mapping for our config.
   Customer Application maintains their mapping. */

int logical_to_phy=61; 
          
/* This function is written so that hardcoding of port
   numbers in Cint scripts is removed. This function gives
   required number of ports                               
*/                                                        
bcm_error_t portNumbersGet(int unit, int *port_list, int num_ports)
{                                                                  

  int i=0,port=0,rv=0;
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

  return BCM_E_NONE;

}

bcm_error_t test_setup(int unit)
{                               

    int port_list[1];

    if (BCM_E_NONE != portNumbersGet(unit, port_list, 1)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    logical_port = 24; //hardcoded as per config

    return BCM_E_NONE;

}

int flex_1x40g_to_4x10g(int unit , bcm_gport_t port, int phy_port)
{
    int i = 0;
    int lanes = 1; /* 1 lane for each 10G port*/
    bcm_port_resource_t res[5];

    /* delete old port */
    bcm_port_resource_t_init(&res[0]);
    res[0].port = port;
    res[0].physical_port = -1;

    for(i=0; i<4; i++) {
        bcm_port_resource_t *res_ptr = &res[i+1];
        bcm_port_resource_t_init(res_ptr);
        res_ptr->port = port+i;
        res_ptr->physical_port = phy_port + (i * lanes);
        res_ptr->speed = 10000;
        res_ptr->lanes = lanes;
        res_ptr->encap = BCM_PORT_ENCAP_IEEE;
    }

    return bcm_port_resource_multi_set(unit, 5 , &res[0]);
 
}

/* Flexport Setup */
bcm_error_t flexport_setup(int unit)
{
    int rv = 0;

    /* remove port from linkscan */
    rv = bcm_port_linkscan_set(unit, logical_port, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_linkscan_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* disable the port */
    rv = bcm_port_enable_set(unit, logical_port, 0);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_port_enable_set(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}

bcm_error_t verify(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int i =0;
    
    bshell(unit, "ps");
    
    /* Flex 100G port to 4x25G ports */
    rv = flex_1x40g_to_4x10g(0,logical_port,logical_to_phy); 
    if(BCM_FAILURE(rv)) {
        printf("\nError in Flex operation: %s.\n",bcm_errmsg(rv));
        return rv;
    }

    for(i=0; i<4; i++) {
        /* Add flexed ports to linkscan */
        rv = bcm_port_linkscan_set(unit, logical_port+i, 1);
        if(BCM_FAILURE(rv)) {
            printf("Error in bcm_port_linkscan_set() post flex operation:
                   %s.\n",bcm_errmsg(rv));
            return rv;
        }

        /* Enable flexed ports */
        rv = bcm_port_enable_set(unit, logical_port+i, 1);
        if(BCM_FAILURE(rv)) {
            printf("Error in bcm_port_enable_set() post flex operation:
                   %s.\n",bcm_errmsg(rv));
            return rv;
        }
    }

    bshell(unit, "ps");
    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit =0;
    bshell(unit, "config show; a ; version");
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return rv;
    }

    if (BCM_FAILURE((rv = flexport_setup(unit)))) {
        printf("Flexport Setup Failed\n");
        return rv;
    }

    if (BCM_FAILURE((rv= verify(unit)))){
        printf("Flexport verify failed\n");
        return rv;
    }
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
} 
