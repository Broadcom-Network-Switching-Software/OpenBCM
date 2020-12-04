/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: Confgure ETPP LIF MTU trap and see packet terminated
 *
 * soc properties:
 *trap_lif_mtu_enable =1;    Jericho B0 and above
 * 
 * how to run the test: 
 * cint ../../../../src/examples/dpp/utility/cint_utils_rx.c
    cint ../../../../src/examples/dpp/utility/cint_utils_switch.c
    cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
    cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
    cint ../../../../src/examples/dpp/cint_etpp_lif_mtu.c
 * cint
 * etpp_lif_mtu__start_run(0,NULL);
 * exit;
 * 
 * 
 * Traffic: 
 
 * Packet flow:
 
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
int ETPP_LIF_MTU_SYS_PORT_1 = 200;
int ETPP_LIF_MTU_SYS_PORT_2 = 201;
int ETPP_LIF_MTU_NUM_OF_HEADER_CODES = bcmForwardingTypeCount;
/*Note both can have local in-LIF and out=LIF*/
int ETPP_LIF_MTU_GLOBAL_LIF_IN_PORT = 0x2000;
int ETPP_LIF_MTU_GLOBAL_LIF_OUT_PORT = 0x2001;

/*VLAN PORT type*/
int ETPP_LIF_MTU_GPORT_GLOBAL_LIF_IN_PORT = 1;
int ETPP_LIF_MTU_GPORT_GLOBAL_LIF_OUT_PORT = ETPP_LIF_MTU_GLOBAL_LIF_OUT_PORT | (17 << 26) /*Vlan port*/| (2 <<22) /*Subtype LIF*/ ; 
int ETPP_LIF_MTU_NUM_OF_USED_PORTS = 2;
int ETPP_LIF_MTU_VSI_FOR_PORTS = 10;

int etpp_lif_mtu_trap_id=0;


struct etpp_lif_mtu_vlan_create_s{
     bcm_vlan_t vsi;
     bcm_port_t port;
     int global_lif_id;
};
struct etpp_lif_mtu_header_trigger_s{
    int header;
    int enable; /*TRUE for enable FALSE for disable*/
};


/*Main Struct*/
struct etpp_lif_mtu_s{
    rx_etpp_trap_set_utils_s trap_utils;
    etpp_lif_mtu_header_trigger_s header_codes[ETPP_LIF_MTU_NUM_OF_HEADER_CODES];
    rx_lif_mtu_set_utils lif_mtu;
    etpp_lif_mtu_vlan_create_s lif_creation_info[ETPP_LIF_MTU_NUM_OF_USED_PORTS];
    l2__mact_properties_s l2_entry;
};

etpp_lif_mtu_s g_etpp_lif_mtu ={
    
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
/**********************************************  Per LIF MTU values Definition ************************************************************/ 
    {
        /*|  gport     Note will be overwritten                 |   MTU val   */
              ETPP_LIF_MTU_GPORT_GLOBAL_LIF_OUT_PORT ,   150
    },
/**********************************************  LIF creation values Definition ************************************************************/ 
    {
         /*VSI                                     |  System Port  as GPORT      |  Global LIF*/
        { ETPP_LIF_MTU_VSI_FOR_PORTS ,ETPP_LIF_MTU_SYS_PORT_1 ,ETPP_LIF_MTU_GLOBAL_LIF_IN_PORT},
        { ETPP_LIF_MTU_VSI_FOR_PORTS ,ETPP_LIF_MTU_SYS_PORT_2 ,ETPP_LIF_MTU_GLOBAL_LIF_OUT_PORT}
    },
/**********************************************  L2 entry Definition ************************************************************/ 
    {
        /*Gport       Note will be overwritten             | mac                                                  |                             vlan                       */
        ETPP_LIF_MTU_GPORT_GLOBAL_LIF_OUT_PORT, {0x00, 0x11, 0x11, 0x11, 0x11, 0x11},ETPP_LIF_MTU_VSI_FOR_PORTS
    }
};


/********** 
  functions
 */
/*****************************************************************************
* Function:  etpp_lif_mtu_init
* Purpose:   Init needed SOC properties
* Params:
* unit (IN)  - Device Number
* Return:    (int)
 */
int etpp_lif_mtu_init(int unit)
{
    
    if (soc_property_get(unit, spn_TRAP_LIF_MTU_ENABLE, 0) == FALSE)
    {
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
    
}

/*****************************************************************************
* Function:  etpp_lif_mtu_struct_get
* Purpose:   Cope the global pre-defined values to param
* Params:
* unit  (IN)     - Device Number
* param (OUT) - the pointer to which predefined values are copied
 */
void etpp_lif_mtu_struct_get(int unit, etpp_lif_mtu_s *param)
{
     sal_memcpy(param,&g_etpp_lif_mtu, sizeof(g_etpp_lif_mtu));
     return;
}

/*****************************************************************************
* Function:  etpp_lif_mtu_trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number 
* Return:    (int)
 */
int etpp_lif_mtu_trap_destroy(int unit) 
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = bcm_rx_trap_type_destroy(unit, etpp_lif_mtu_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : bcm_rx_trap_type_destroy, returned %d\n", rv);
        return rv;
    }

    return rv;
}

/*****************************************************************************
* Function:  etpp_lif_mtu__start_run_with_params
* Purpose:  Run the cint with params 
* Params:
* unit    (IN)     - Device Number 
* inPort   (IN)   - Tx to this port
* outPort   (IN)   -  Rx Port
* mtu     (IN)     - MTU val for trap, packet which MTU exceeds this values will be traped
* vsi       (IN)  - To careate vlan between ports
* mirror_cmd       (IN)  - mirror_id, hold the id to data for mirroring 
* action      (IN) - Action which will be done after trap activation 
*                        0 - forward + mirror
*                        1 - drop (no forward + no mirror)
*                        2 - only mirror (no forward)
* Return:    (int)
 */
int etpp_lif_mtu__start_run_with_params(int unit,bcm_port_t inPort,bcm_port_t outPort, uint32 mtu,bcm_vlan_t vsi,int mirror_cmd, int action)
{
    etpp_lif_mtu_s etpp_lif_mtu_info;
    etpp_lif_mtu_struct_get(unit,&etpp_lif_mtu_info);
    etpp_lif_mtu_info.lif_creation_info[0].port = inPort;
    etpp_lif_mtu_info.lif_creation_info[1].port = outPort;
    etpp_lif_mtu_info.lif_creation_info[0].vsi = vsi;
    etpp_lif_mtu_info.lif_creation_info[1].vsi = vsi;
    etpp_lif_mtu_info.lif_mtu.mtu = mtu;

    /* action:
        * 0 - forward + mirror
        * 1 - drop (no forward + no mirror)
        * 2 - only mirror (no forward)
        */
    if (action == 2)
    {
        etpp_lif_mtu_info.trap_utils.fwd_enable = FALSE;
    }
    else if(action == 1)
    {
        etpp_lif_mtu_info.trap_utils.fwd_enable = FALSE;
        etpp_lif_mtu_info.trap_utils.mirror_enable = 0;
    }
    else if (action == 0) 
    {
        etpp_lif_mtu_info.trap_utils.mirror_cmd = mirror_cmd;
    }
    
    return etpp_lif_mtu__start_run(unit,&etpp_lif_mtu_info);
    
}


/*Main Function*/
/*****************************************************************************
* Function:  etpp_lif_mtu__start_run
* Purpose:   Main function run default configuration for setting all needed values for ETPP LIF MTU
* Params:
* unit (IN) - Device Number
* param (IN) - overrides the default configuration
* Return:    (int)
 */
int etpp_lif_mtu__start_run(int unit, etpp_lif_mtu_s *param)
{
    int rv = BCM_E_NONE;
    etpp_lif_mtu_s etpp_lif_mtu_info;
    int i = 0;
    int gport_subtype;
    bcm_gport_t gport_id;

    rv = etpp_lif_mtu_init(unit);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in etpp_lif_mtu_init\n");
        return rv;
    }

    /*Enables the call etpp_lif_mtu__start_run(0,NULL)*/
    if(param == NULL)
    {
        etpp_lif_mtu_struct_get(unit,&etpp_lif_mtu_info);
    }
    else
    {
        sal_memcpy(&etpp_lif_mtu_info, param, sizeof(g_etpp_lif_mtu));
    }
    
   

    /*Set Header*/
    for(i = 0 ; i < ETPP_LIF_MTU_NUM_OF_HEADER_CODES; i++)
    {
        rv = switch__link_layer_mtu_header_trigger(unit,etpp_lif_mtu_info.header_codes[i].header,
                                                        etpp_lif_mtu_info.header_codes[i].enable);
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

    /*Create LIF*/
    for(i = 0 ; i < ETPP_LIF_MTU_NUM_OF_USED_PORTS ; i++)
    {
        rv = vlan__port_vsi_egress_and_igress__create(unit,etpp_lif_mtu_info.lif_creation_info[i].vsi,
                                                          etpp_lif_mtu_info.lif_creation_info[i].port,
                                                         &gport_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vlan__port_vsi_egress_and_igress_with_id__create index $i \n");
            return rv;
        }
    }

    /*Create port return the gport of global LIF*/
    etpp_lif_mtu_info.lif_mtu.gport = gport_id;
    etpp_lif_mtu_info.l2_entry.gport_id = gport_id;
    
    /*Set MTU val for the outlif*/
    rv = rx__lif_mtu__set(unit,&(etpp_lif_mtu_info.lif_mtu));
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in rx__lif_mtu__set\n");
        return rv;
    }
    else
    {
       printf("MTU %d for GPORT 0x%x was set\n",etpp_lif_mtu_info.lif_mtu.mtu,etpp_lif_mtu_info.lif_mtu.gport); 
    }

    /*Add entry to MAC Table*/
    rv = l2__mact_entry_create(unit,etpp_lif_mtu_info.l2_entry);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in l2__mact_entry_create\n");
        return rv;
    }
    else
    {
        printf("L2 entry set \n");
    }

    /*Create trap*/
    rv = rx__etpp_trap__set(unit,bcmRxTrapEgTxMtuFilter,&(etpp_lif_mtu_info.trap_utils),&etpp_lif_mtu_trap_id);
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


 
