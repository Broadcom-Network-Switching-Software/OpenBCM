/*  Feature  : Flexport
 *                   
 *  Usage    : BCM.0> cint flexport_4x25G_to_2xHG21.c
 *                                 
 *  config   : BCM56275_A1-PORT.bcm   
 *                                 
 *  Log file : flexport_4x25G_to_2xHG21_log.txt      
 *                                 
 *  Test Topology :                
 *                                 
 *                   +------------------------------+  
 *                   |                              +
 *                   |                              |                  
 *                   |                              |                  
 *                   |                              |  port49 ~ port52    
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |                  
 *                   |                              |                  
 *                   |                              |      
 *                   |                              +
 *                   |                              |                  
 *                   +------------------------------+                  
 *  Summary:                                                           
 *  ========                                                           
 *    This CINT script demonstrates how to flex four 25G ports into two HG[21] ports from application 
 *    using bcm APIs.
 *
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Select the TSCF ports.
 *
 *    2) Step3 - Verification(Done in verify())
 *    ==========================================
 *      a) Dispaly port information before flex operation.
 *      b) Disable linkscan for the selected ports.
 *      c) Disable the selected ports.
 *      d) Flex the selected ports. 
 *      e) Enable linkscan for the new flexed ports.
 *      f) Enable newly flexed ports.
 *      g) Expected Result:
 *      ===================
 *         The selected ports will get flexed to two HG[21] ports. ps command output will show 
 *         new 2 HG[21] ports.
 */

cint_reset();

/* The loggical to physical port mapping for our config.
   Customer Application maintains their mapping. */
   
/* Logical port number */
const int tscf_port [4]    
            =  { 49, 50, 51, 52}; // TSC4: TSCF-16       
/* Physical port number */
const int tscf_physical_port [4]    
            =  {57, 58, 59, 60}; // TSC4: TSCF-16

// for 1-lane port only
enum encap_action_e {
    FLEXPORT_ENCAP_IEEE_10G  = 0, 
    FLEXPORT_ENCAP_IEEE_25G  = 1,
    FLEXPORT_ENCAP_HIGIG2_11G  = 2,  
    FLEXPORT_ENCAP_HIGIG2_27G  = 3  
};

bcm_error_t 
flexport_encap_set(int unit, int port, int encap_action) 
{ 
    int speed, encap; 
    bcm_port_resource_t resource;
    bcm_port_resource_t del_resource;

    switch (encap_action)
    {
        case FLEXPORT_ENCAP_IEEE_10G: 
            speed=10000;
            encap=BCM_PORT_ENCAP_IEEE;
            break;
        case FLEXPORT_ENCAP_IEEE_25G: 
            speed=25000;
            encap=BCM_PORT_ENCAP_IEEE;
            break;
        case FLEXPORT_ENCAP_HIGIG2_11G: 
            speed=11000;
            encap=BCM_PORT_ENCAP_HIGIG2;
            break;    
        case FLEXPORT_ENCAP_HIGIG2_27G: 
            speed=27000;
            encap=BCM_PORT_ENCAP_HIGIG2;
            break;                
        default:
            printf("This port encap action is not supported: %d\n", encap_action);
    }

    BCM_IF_ERROR_RETURN(bcm_port_resource_get(unit, port, &resource));
    BCM_IF_ERROR_RETURN(bcm_port_enable_set(unit, port, 0));     
    BCM_IF_ERROR_RETURN(bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_NONE));
    bcm_port_resource_t_init(&del_resource); 
    del_resource.port = resource.port; 
    del_resource.physical_port = -1; 
    BCM_IF_ERROR_RETURN(bcm_port_resource_set(unit, port, &del_resource));
    resource.speed = speed;
    resource.encap = encap;
    BCM_IF_ERROR_RETURN(bcm_port_resource_set(unit, port, &resource));
    BCM_IF_ERROR_RETURN(bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_SW)); 
    BCM_IF_ERROR_RETURN(bcm_port_enable_set(unit, port, 1)); 
    return BCM_E_NONE;
}

int is_tscf_port(int unit, int port)
{
    int len = 50;
    char name[len];

    if (BCM_E_NONE == bcm_port_phy_drv_name_get(unit, port, name, len)) {
        if (0 == sal_strcmp(name, "Tscf")) {
            return 1;
        }
    }
    return 0;
}

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

    if (BCM_PBMP_NOT_NULL(configP.xe)) {
        ports_pbmp = configP.xe;
        for (i= 1; i < BCM_PBMP_PORT_MAX; i++) {
            if (BCM_PBMP_MEMBER(&ports_pbmp,i) && is_tscf_port(unit, i)) {                                    
                port++;
            }
        }
    }
    if ((port < num_ports) && BCM_PBMP_NOT_NULL(configP.hg)) {
        ports_pbmp = configP.hg;
        for (i= 1; (i < BCM_PBMP_PORT_MAX) && (port < num_ports); i++) {
            if (BCM_PBMP_MEMBER(&ports_pbmp,i)) {
                rv = flexport_encap_set(unit, i, FLEXPORT_ENCAP_IEEE_25G);     
                if(BCM_FAILURE(rv)) {                   
                    printf("\nError in changing port encap: %s.\n",bcm_errmsg(rv));
                    return rv;                                                           
                }
                if (is_tscf_port(unit, i)) {                                            
                    port++; 
                }                                                              
            }
        }   
    }                                                         
    port = 0;
    rv = bcm_port_config_get(unit,&configP);
    if(BCM_FAILURE(rv)) {                   
        printf("\nError in retrieving port configuration: %s.\n",bcm_errmsg(rv));
        return rv;                                                               
    }   
    ports_pbmp = configP.xe;
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
    int i;
    int port_list[4];
    bcm_port_resource_t resource;
    char *flex_conf_str;

    flex_conf_str = sal_config_get("port_flex_enable");
    if (1 == sal_strcmp(flex_conf_str, "1")) {
        printf(">> Please add config variable - port_flex_enable=1");
        return -1;
    }
    if (BCM_E_NONE != portNumbersGet(unit, port_list, 4)) {
        printf("portNumbersGet() failed\n");
        return -1;
    }

    for (i = 0; i < 4; i++) {
        tscf_port[i] = port_list[i];

        if (BCM_E_NONE != bcm_port_resource_get(unit, tscf_port[i], &resource)) {
            printf("bcm_port_resource_get() failed\n");
            return -1;
        }
        tscf_physical_port[i] = resource.physical_port;
    }

    return BCM_E_NONE;

}

enum action_e {
    FLEXPORT_4x25_TO_2x21  = 0, 
    FLEXPORT_2x21_TO_4x11  = 1,
    FLEXPORT_4x10_TO_4x11  = 2    
};

bcm_error_t 
flexport_set(int unit, int *logical_port, int *physical_port, int flexport_action) 
{ 
  /* Need up to 8 resources, 4 old and 4 new */ 
  bcm_port_resource_t reconfig[8]; 
  int idx; 
  int old_port_lane_num;
  int new_port_lane_num;
  int old_port_num;
  int new_port_num;
  int new_speed;
  int flag = 0;
  
  /* Clear original port config */ 
  switch (flexport_action)
  {
  case FLEXPORT_4x25_TO_2x21: // 4 * 25G to 2 * HG[21]
      old_port_lane_num=1;
      old_port_num=4;
      new_port_lane_num=2;
      new_port_num=2;
      new_speed=21000;
      break;
  case FLEXPORT_2x21_TO_4x11: // 2 * HG[21] to 4 * HG[11]
      old_port_lane_num=2;
      old_port_num=2;
      new_port_lane_num=1;
      new_port_num=4;
      new_speed=11000;
      break;            
  case FLEXPORT_4x10_TO_4x11: // 4 * 10G to 4 * HG[11]
      old_port_lane_num=1;
      old_port_num=4;
      new_port_lane_num=1;
      new_port_num=4;
      new_speed=11000;
      break;        
  default:
      printf("This Flexport action is not supported: %d\n", flexport_action);
  }
  
  if (old_port_lane_num == new_port_lane_num) {
      flag = BCM_PORT_RESOURCE_SPEED_ONLY;
  }
    
  for (idx = 0; idx < old_port_num; idx++) { 
    bcm_port_resource_t *reconfig_ptr = &reconfig[idx]; 
    int offset = (old_port_num == 2)? 2 : 1;

    bcm_port_resource_t_init(reconfig_ptr); 
    reconfig_ptr->port = logical_port[idx * offset]; 
    reconfig_ptr->physical_port = -1; 
    reconfig_ptr->flags = flag;
    //print *reconfig_ptr;
  } 
  
  /* Configure new ports */ 
  for (idx = 0; idx < new_port_num; idx++) { 
    //printf("update new config for logical_port:%d\n", logical_port[idx]);
    bcm_port_resource_t *reconfig_ptr = &reconfig[idx + old_port_num]; 
    int offset = (new_port_num == 2)? 2 : 1;

    bcm_port_resource_t_init(reconfig_ptr); 
    reconfig_ptr->port = logical_port[idx * offset]; 
    reconfig_ptr->physical_port = physical_port[idx * offset]; 
    reconfig_ptr->speed = new_speed; 
    reconfig_ptr->lanes = new_port_lane_num; 
    reconfig_ptr->encap = BCM_PORT_ENCAP_HIGIG2; 
    reconfig_ptr->flags = flag;    
    //print *reconfig_ptr;
  } 
  
  /* Disable old ports and remove from linkscan */ 
  for (idx = 0; idx < old_port_num; idx++) { 
    bcm_port_t port = reconfig[idx].port; 
    
    printf("Disable port:%d\n", port);
    BCM_IF_ERROR_RETURN(bcm_port_enable_set(unit, port, 0)); 
    printf("Set Linkscan None for port:%d\n", port);
    BCM_IF_ERROR_RETURN(bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_NONE)); 
  } 
  
  /* Flex ports */ 
  BCM_IF_ERROR_RETURN(bcm_port_resource_multi_set(unit, old_port_num+new_port_num, reconfig)); 
  
  /* Add new ports back into linkscan and then enable */ 
  for (idx = old_port_num; idx < (old_port_num + new_port_num); idx++) { 
    bcm_port_t port = reconfig[idx].port; 
    printf("Set Linkscan SW for port:%d\n", port);
    BCM_IF_ERROR_RETURN(bcm_linkscan_mode_set(unit, port, BCM_LINKSCAN_MODE_SW)); 
    printf("Enable port:%d\n", port);
    BCM_IF_ERROR_RETURN(bcm_port_enable_set(unit, port, 1)); 
  } 
  return BCM_E_NONE; 
}


bcm_error_t verify(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    char str[512];    
    int i =0;

    print "======================================";
    print "  4 * 25G after booting";
    print "======================================";
    snprintf(str, 512, "ps %d-%d", tscf_port[0], tscf_port[3]);
    bshell(unit, str);

    print "======================================";
    print "  4 * 25G   ->   2 * HG[21]";
    print "======================================";
    rv = flexport_set(unit, tscf_port[0], tscf_physical_port[0], FLEXPORT_4x25_TO_2x21);
    if(BCM_FAILURE(rv)) {
        printf("\nError in Flex operation: %s.\n",bcm_errmsg(rv));
        return rv;
    }
    snprintf(str, 512, "ps %d-%d", tscf_port[0], tscf_port[2]);
    bshell(unit, str);

    return BCM_E_NONE;
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit =0;
    bshell(unit, "config show; a ; version  ; cancun stat");
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
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
