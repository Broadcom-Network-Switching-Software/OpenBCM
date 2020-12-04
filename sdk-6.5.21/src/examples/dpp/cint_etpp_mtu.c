/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: Confgure ETPP MTU trap and see packet terminated, 
    Note addition to this cint a RIF infterface with mtu should be created
 *
 * soc properties:
 *trap_lif_mtu_enable =1;    Jericho B0 and above
 * 
 * how to run the test: 
    cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
    cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
    cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
    cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
    cint ../../../../src/examples/dpp/utility/cint_utils_rx.c
    cint ../../../../src/examples/dpp/utility/cint_utils_switch.c
    cint ../../../../src/examples/dpp/cint_multi_device_utils.c
    cint ../../../../src/examples/dpp/cint_ip_route.c
    cint ../../../../src/examples/dpp/cint_etpp_mtu.c
    cint
    etpp_mtu__start_run(0,NULL);
    exit;
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
int ETPP_MTU_NUM_OF_HEADER_CODES = bcmForwardingTypeCount;

int etpp_mtu_trap_id=0;


struct etpp_mtu_header_trigger_s{
    int header;
    int enable; /*TRUE for enable FALSE for disable*/
};

/*Main Struct*/
struct etpp_mtu_s{
    rx_etpp_trap_set_utils_s        trap_utils;
    etpp_mtu_header_trigger_s       header_codes[ETPP_MTU_NUM_OF_HEADER_CODES];
    int                             mtu_profile;
    int                             mtu_val;
};

etpp_mtu_s g_etpp_mtu ={
    
/**********************************************  Trap Definition ************************************************************/
        /*mirror command      | mirror strength     | mirror enable     | forward strength    | forward enable*/
     {           1      ,        1       ,     1      ,      2        ,           1        },

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
/**********************************************  MTU Value set ************************************************************/    
    1,      /*MTU Profile*/
    150     /*MTU Value*/
};


/********** 
  functions
 */
/*****************************************************************************
* Function:  etpp_mtu_init
* Purpose:   Supported form jericho and above
* Params:
* unit (IN)  - Device Number
* Return:    (int)
 */
int etpp_mtu_init(int unit)
{
    
    if (is_device_or_above(unit, JERICHO) == FALSE)
    {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
    
}

/*****************************************************************************
* Function:  etpp_mtu_struct_get
* Purpose:   Cope the global pre-defined values to param
* Params:
* unit  (IN)     - Device Number
* param (OUT) - the pointer to which predefined values are copied
 */
void etpp_mtu_struct_get(int unit, etpp_mtu_s *param)
{
     sal_memcpy(param,&g_etpp_mtu, sizeof(g_etpp_mtu));
     return;
}

/*****************************************************************************
* Function:  etpp_mtu_trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number 
* Return:    (int)
 */
int etpp_mtu_trap_destroy(int unit) 
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = bcm_rx_trap_type_destroy(unit, etpp_mtu_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : bcm_rx_trap_type_destroy, returned %d\n", rv);
        return rv;
    }

    return rv;
}

/*****************************************************************************
* Function:  etpp_mtu__start_run_with_params
* Purpose:  Run the cint with params 
* Params:
* unit    (IN)     - Device Number 
* mirror_cmd   (IN)   - mirror_id, hold the id to data for mirroring
* mtu     (IN)     - MTU val for trap, packet which MTU exceeds this values will be traped
* Return:    (int)
 */
int etpp_mtu__start_run_with_params(int unit, int mtu,int mirror_cmd)
{
    etpp_mtu_s etpp_mtu_info;
    etpp_mtu_struct_get(unit,&etpp_mtu_info);
    etpp_mtu_info.mtu_val = mtu;
    etpp_mtu_info.trap_utils.mirror_cmd = mirror_cmd;
    return etpp_mtu__start_run(unit,&etpp_mtu_info);
    
}
/*Main Function*/
/*****************************************************************************
* Function:  etpp_mtu__start_run
* Purpose:   Main function run default configuration for setting all needed values for ETPP MTU
* Params:
* unit (IN) - Device Number
* param (IN) - overrides the default configuration
* Return:    (int)
 */
int etpp_mtu__start_run(int unit, etpp_mtu_s *param)
{
    int rv = BCM_E_NONE;
    etpp_mtu_s etpp_mtu_info;
    int i = 0;
    int gport_subtype;
    bcm_gport_t gport_id;

     rv = etpp_mtu_init(unit);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in etpp_mtu_init\n");
        return rv;
    }

    /*Enables the call etpp_mtu__start_run(0,NULL)*/
    if(param == NULL)
    {
        etpp_mtu_struct_get(unit,&etpp_mtu_info);
    }
    else
    {
        sal_memcpy(&etpp_mtu_info, param, sizeof(g_etpp_mtu));
    }
    
    /*Set Header*/
    for(i = 0 ; i < ETPP_MTU_NUM_OF_HEADER_CODES; i++)
    {
        rv = switch__link_layer_mtu_header_trigger(unit,etpp_mtu_info.header_codes[i].header,
                                                        etpp_mtu_info.header_codes[i].enable);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in switch__link_layer_mtu_header_trigger index $ i\n");
            return rv;
        }
    }

    /*Set MTU value*/
    rv = switch__link_layer_mtu_value_per_profile_set(unit,etpp_mtu_info.mtu_profile,etpp_mtu_info.mtu_val);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in setting the MTU value \n");
        return rv;
    }

    
    /*Create trap*/
    rv = rx__etpp_trap__set(unit,bcmRxTrapEgTxMtuFilter,&(etpp_mtu_info.trap_utils),&etpp_mtu_trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in rx__etpp_trap__set\n");
        return rv;
    }


    return rv;
    
}


 
