/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: Configure SER trap
 * 
 * how to run the test: 
  cint ../../../../src/examples/dpp/utility/cint_utils_rx.c
  cint ../../../../src/examples/dpp/cint_ser.c
  cint
  ser_ingress__start_run(0);
  exit;
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */


int ser_ingress_trap_id=0;



/*****************************************************************************
* Function:  ser_ingress_trap_destroy
* Purpose:   Destroy the created trap
* Params:
* unit (IN)     - Device Number 
* Return:    (int)
 */
int ser_ingress_trap_destroy(int unit) 
{
    int rv = BCM_E_NONE;

    /*delete trap*/
    rv = rx__trap_destroy(unit, ser_ingress_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in : ser_ingress_trap_destroy, returned %d\n", rv);
        return rv;
    }

    return rv;
}


/*Main Function*/
/*****************************************************************************
* Function:  ser_ingress__start_run
* Purpose:   Main function run default configuration for setting all needed values for SER Ingress Trap
* Params:
* unit (IN) - Device Number
* Return:    (int)
 */
int ser_ingress__start_run(int unit)
{
    int rv = BCM_E_NONE;

    rv = rx__ingress_trap_to_cpu(unit, bcmRxTrapSer, 4, &ser_ingress_trap_id);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, in rx__predefined_trap_drop\n");
        return rv;
    }
    else
    {
        printf("Trap bcmRxTrapSer was created and set \n");
    }

    return rv;
    
}

 
