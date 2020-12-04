/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: Configure different destination per core
 * 
 * how to run the test: 
  cint ../../../../src/examples/dpp/utility/cint_utils_rx.c
  cint ../../../../src/examples/dpp/cint_config_dest_per_core.c
  cint
  dest_per_core__start_run_TrapSaEqualsDa(0);
  exit;
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */


int _trap_id=0;



/*****************************************************************************
* Function:  _trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number 
* Return:    (int)
 */
int _trap_destroy(int unit) 
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = rx__trap_destroy(unit, _trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : ser_ingress_trap_destroy, returned %d\n", rv);
        return rv;
    }

    return rv;
}

/*****************************************************************************
* Function:  dest_per_core__start_run_TrapSaEqualsDa
* Purpose:   Packet that arrive on core 0 will be dropped and packet on core 1 will be forwarded to CPU
* Params:
* unit - Device Number
* Return:    (int)
 */
int dest_per_core__start_run_TrapSaEqualsDa(int unit)
{
    int rv = BCM_E_NONE;
    /*Packet that arrive on core 0 will be dropped and packet on core 1 will be forwarded to CPU*/
    rv = dest_per_core__start_run(unit,bcmRxTrapSaEqualsDa,4,BCM_GPORT_BLACK_HOLE,BCM_GPORT_LOCAL_CPU);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in dest_per_core__start_run\n");
        return rv;
    }
}
    

/*Main Function*/

/*****************************************************************************
* Function:  dest_per_core__start_run
* Purpose:   
* Params:
* unit      - Device Number
* trap_type - Type to create
* trap_strength - strength of trap
* dest0  - Destination for core0
* dest1  - Destination for core1
* Return:    (int)
 */
int dest_per_core__start_run(int unit,bcm_rx_trap_t trap_type,int trap_strength,bcm_gport_t dest0,bcm_gport_t dest1)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_core_config_t core_config_arr[2];
    int trap_create_flags = 0;

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST);
    trap_config.trap_strength = trap_strength;
    trap_config.core_config_arr_len = 2;
    core_config_arr[0].dest_port = dest0;
    core_config_arr[1].dest_port = dest1;
    trap_config.core_config_arr = &core_config_arr;
    


    rv = rx__trap_create_and_set(unit, trap_type, trap_create_flags, &trap_config, &_trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in rx__trap_create_and_set\n");
        return rv;
    }

    return rv;
    
}

 
