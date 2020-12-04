/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: Confgure ETPP RIF MTU trap and see packet terminated
 *
 * soc properties:
 *trap_lif_mtu_enable =1;    Jericho B0 and above
 * 
 * how to run the test: 
 * cint ../../../../src/examples/dpp/utility/cint_utils_rx.c
    cint ../../../../src/examples/dpp/utility/cint_utils_switch.c
    cint ../../../../src/examples/dpp/cint_etpp_rif_mtu.c
   cint
   etpp_rif_mtu__start_run(0,NULL);
   exit;
 * 
 * 
 * Traffic: 
 
 * Packet flow:
 
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */

int ETPP_RIF_MTU_NUM_OF_HEADER_CODES = bcmForwardingTypeCount;

int etpp_rif_mtu_trap_id=0;



struct etpp_rif_mtu_header_trigger_s{
    int header;
    int enable; /*TRUE for enable FALSE for disable*/
};


/*Main Struct*/
struct etpp_rif_mtu_s{
    rx_etpp_trap_set_utils_s trap_utils;
    etpp_rif_mtu_header_trigger_s header_codes[ETPP_RIF_MTU_NUM_OF_HEADER_CODES];
    rx_rif_mtu_set_utils rif_mtu;
};

etpp_rif_mtu_s g_etpp_rif_mtu ={
    
/**********************************************  Trap Definition ************************************************************/
        /*mirror command                    | mirror strength | mirror enable  | forward strength | forward enable*/
     {       1               ,        1       ,     1      ,      2        ,   1        },
        
/**********************************************  Header enable for MTU Definition ************************************************************/    
    {
         /*                    Header Code NUM|               Enable Header*/
        {   bcmForwardingTypeL2,                  TRUE},/* L2 switching forwarding. */
        {   bcmForwardingTypeIp4Ucast,            TRUE},/* IPv4 Unicast Routing forwarding. */
        {   bcmForwardingTypeIp4Mcast,            TRUE},/* IPv4 Multicast Routing forwarding. */
        {   bcmForwardingTypeIp6Ucast,            TRUE},/* IPv6 Unicast Routing forwarding. */
        {   bcmForwardingTypeIp6Mcast,            TRUE},/* IPv6 Multicast Routing forwarding. */
        {   bcmForwardingTypeMpls,                FALSE},/* MPLS Switching forwarding. */
        {   bcmForwardingTypeTrill,               FALSE},/* Trill forwarding. */
        {   bcmForwardingTypeRxReason,            FALSE},/* Forwarding according to a RxReason. */
        {   bcmForwardingTypeTrafficManagement,   FALSE},/* Traffic Management forwarding, when an external Packet Processor sets the forwarding decision. */
        {   bcmForwardingTypeSnoop,               FALSE},/* Snooped packet. */
        {   bcmForwardingTypeFCoE,                FALSE} /* Fiber Channel over Ethernet forwarding. */
    },
/**********************************************  Per RIF MTU values Definition ************************************************************/ 
    {
        /*| RIF Interface          |   MTU val   */
              0x00000064 ,             150
    }
};


/********** 
  functions
 */
/*****************************************************************************
* Function:  etpp_rif_mtu_init
* Purpose:   Init needed SOC properties
* Params:
* unit (IN)  - Device Number
* Return:    (int)
 */
int etpp_rif_mtu_init(int unit)
{
    
    if (soc_property_get(unit, spn_TRAP_LIF_MTU_ENABLE, 0) == FALSE)
    {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
    
}

/*****************************************************************************
* Function:  etpp_rif_mtu_struct_get
* Purpose:   Cope the global pre-defined values to param
* Params:
* unit  (IN)     - Device Number
* param (OUT) - the pointer to which predefined values are copied
 */
void etpp_rif_mtu_struct_get(int unit, etpp_rif_mtu_s *param)
{
     sal_memcpy(param,&g_etpp_rif_mtu, sizeof(g_etpp_rif_mtu));
     return;
}

/*****************************************************************************
* Function:  etpp_rif_mtu_trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number 
* Return:    (int)
 */
int etpp_rif_mtu_trap_destroy(int unit) 
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = bcm_rx_trap_type_destroy(unit, etpp_rif_mtu_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : bcm_rx_trap_type_destroy, returned %d\n", rv);
        return rv;
    }

    return rv;
}

/*****************************************************************************
* Function:  etpp_rif_mtu__start_run_with_params
* Purpose:  Run the cint with params 
* Params:
* unit    (IN)     - Device Number 
* intf   (IN)   - RIF encoded as interface
* mtu     (IN)     - MTU val for trap, packet which MTU exceeds this values will be traped
* mirror_cmd       (IN)  - mirror_id, hold the id to data for mirroring
* action      (IN) - Action which will be done after trap activation 
*                        0 - forward + mirror
*                        1 - drop (no forward + no mirror)
*                        2 - only mirror (no forward)
* Return:    (int)
 */
int etpp_rif_mtu__start_run_with_params(int unit,bcm_if_t intf, uint32 mtu,int mirror_cmd, int action)
{
    etpp_rif_mtu_s etpp_rif_mtu_info;
    etpp_rif_mtu_struct_get(unit,&etpp_rif_mtu_info);
    etpp_rif_mtu_info.rif_mtu.intf = intf;
    etpp_rif_mtu_info.rif_mtu.mtu = mtu;
    
    /* action:
        * 0 - forward + mirror
        * 1 - drop (no forward + no mirror)
        * 2 - only mirror (no forward)
        */
    if (action == 2)
    {
        etpp_rif_mtu_info.trap_utils.fwd_enable = FALSE;
    }
    else if(action == 1)
    {
        etpp_rif_mtu_info.trap_utils.fwd_enable = FALSE;
        etpp_rif_mtu_info.trap_utils.mirror_enable = 0;
    }
    else if (action == 0) 
    {
        etpp_rif_mtu_info.trap_utils.mirror_cmd = mirror_cmd;
    }
    
    return etpp_rif_mtu__start_run(unit,&etpp_rif_mtu_info);
    
}
/*Main Function*/
/*****************************************************************************
* Function:  etpp_rif_mtu__start_run
* Purpose:   Main function run default configuration for setting all needed values for ETPP RIF MTU
* Params:
* unit (IN) - Device Number
* param (IN) - overrides the default configuration
* Return:    (int)
 */
int etpp_rif_mtu__start_run(int unit, etpp_rif_mtu_s *param)
{
    int rv = BCM_E_NONE;
    etpp_rif_mtu_s etpp_rif_mtu_info;
    int i = 0;

    rv = etpp_rif_mtu_init(unit);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in etpp_rif_mtu_init\n");
        return rv;
    }

    /*Enables the call etpp_rif_mtu__start_run(0,NULL)*/
    if(param == NULL)
    {
        etpp_rif_mtu_struct_get(unit,&etpp_rif_mtu_info);
    }
    else
    {
        sal_memcpy(&etpp_rif_mtu_info, param, sizeof(g_etpp_rif_mtu));
    }
    

    /*Set Header*/
    for(i = 0 ; i < ETPP_RIF_MTU_NUM_OF_HEADER_CODES; i++)
    {
        rv = switch__link_layer_mtu_header_trigger(unit,etpp_rif_mtu_info.header_codes[i].header,
                                                        etpp_rif_mtu_info.header_codes[i].enable);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in switch__link_layer_mtu_header_trigger index $ i\n");
            return rv;
        }
        else
        {
            printf("Header set, index: %d \n",i);
        }
    
    }

    
    /*Set MTU val for the outrif*/
    rv = rx__rif_mtu__set(unit,&(etpp_rif_mtu_info.rif_mtu));
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in rx__rif_mtu__set\n");
        return rv;
    }

    bcm_l3_intf_t intf;
    bcm_l3_intf_t_init(&intf);
    intf.l3a_intf_id = etpp_rif_mtu_info.rif_mtu.intf;
    print bcm_l3_intf_get(unit, &intf);

    intf.l3a_ttl = 64;
    intf.l3a_flags = BCM_L3_WITH_ID | BCM_L3_REPLACE;
    print bcm_l3_intf_create(unit, &intf);

    bcm_rx_mtu_config_t config;
    config.flags = BCM_RX_MTU_RIF;
    config.intf = etpp_rif_mtu_info.rif_mtu.intf;
    print bcm_rx_mtu_get(unit,&config);

    /*Create trap*/
    rv = rx__etpp_trap__set(unit,bcmRxTrapEgTxMtuFilter,&(etpp_rif_mtu_info.trap_utils),&etpp_rif_mtu_trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in rx__etpp_trap__set\n");
        return rv;
    }
    else
    {
        printf("Trap bcmRxTrapEgTxMtuFilter was created and set \n");
    }

    return rv;
    
}


 
