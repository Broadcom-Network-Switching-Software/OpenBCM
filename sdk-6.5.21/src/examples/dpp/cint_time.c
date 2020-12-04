/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~TIME~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_time.c
 * Purpose: bcm_time_capture_get BCM API use example
 *
 * Before using examples in this CINT, one need to:
 * -----------------------------------------------:
 * 1) Enable TS_PLL clock by enabling following soc property:
 *      phy_1588_dpll_frequency_lock.BCM88650=1
 *
 * 2) Setup a broad sync (BS) system (requires dedication BS physical configuration). 
 *
 * 3) Initialize ARM ukernel firmware:
 *
 *      BS master configuration :
 *      ---------------------  
 *      mcsload 0 /firmware/BCM88650_A0_0.srec initmcs=1
 *      mcsload 1 /firmware/BCM88650_A0_1bs.srec
 *      bs config master bc=10000000 hb=4000
 *      bs debug on;sleep 5;bs debug off
 *      bs status
 *
 *      BS slave configuration :
 *      ---------------------  
 *      mcsload 0 /firmware/BCM88650_A0_0.srec initmcs=1
 *      mcsload 1 /firmware/BCM88650_A0_1bs.srec
 *      bs config bc=10000000 hb=4000
 *      bs debug on;sleep 5;bs debug off
 *      bs status
 *
 *
 * Provides the following functionalities:
 * - get ARAD BS time of day 
 * - Time interface traverse/delete/heartbeat_enable_get. 
 */
/* 
 * how to run:
 *
 *
cint ../../../../src/examples/dpp/cint_time.c
cint
int rv;
rv = get_time(0, 0); 
exit;
sleep 1
cint
rv = get_time(0, 0); 
exit;
sleep 1
cint
rv = get_time(0, 0); 
exit;
sleep 1
cint
rv = get_time(0, 0); 
exit;
sleep 1
cint
rv = get_time(0, 0); 
exit;

cint
rv = interface_traverse(0); 
print rv;
rv = heartbeat_enable_get(0, 0); 
print rv;
rv = interface_delete(0, 0); 
print rv;
exit;

 */ 

/* 
   interface_id - the BS time interface id that is internally created when calling following BCM shell command :
      "bs config [master] bc=10000000 hb=4000" ("bs config" internally calls bcm_time_interface_add )
    
   unit - unit id of SLAVE device ! (in case 'unit' is master device, 'bcm_time_capture_get' initializes 'time' to zero)
*/


int get_time(int unit, int interface_id)
{
    int    rv = BCM_E_NONE;
    bcm_time_capture_t time = {0};
    bcm_time_interface_t interface;

    interface.id = interface_id;

    rv = bcm_time_capture_get(unit, interface.id, &time);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_time_capture_get rv = %d\n", rv);
        return rv;
    }


    printf("BS ToD:   %d.%09d \n", COMPILER_64_LO(time.received.seconds),   time.received.nanoseconds);

    return rv;
}

/*****************************************
 * cint $SDK/src/examples/dpp/cint_time.c
 * cint
 * int rv;
 * rv = get_broadsync_time (0);
 * print rv;
 */
int get_broadsync_time(int unit)
{
    int    rv = BCM_E_NONE;
    bcm_time_spec_t time = {0};

    rv = bcm_time_bs_time_get(unit, &time);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_time_bs_time_get rv = %d\n", rv);
        return rv;
    }
    printf("BS ToD:   %d.%09d \n", COMPILER_64_LO(time.seconds), time.nanoseconds);
    return rv;
}

int traverse_cb(int unit, 
                bcm_time_interface_t *intf, 
                void *user_data)
{
    int *data= user_data;

    printf("traverse_cb: unit %d intf_id %d flags 0x%08x user_data %d\n", 
           unit, intf->id, intf->flags, *data);

}

int interface_traverse(int unit)
{
    int  rv = BCM_E_NONE;
    int user_data = 3;

    rv = bcm_time_interface_traverse(unit, traverse_cb, &user_data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_time_interface_traverse rv = %d\n", rv);
        return rv;
    }

    return rv;
}


int heartbeat_enable_get(int unit, int interface_id)
{
    int  rv = BCM_E_NONE;
    int enable;

    rv = bcm_time_heartbeat_enable_get(unit, interface_id, &enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_time_heartbeat_enable_get rv = %d\n", rv);
        return rv;
    }

    printf("enable = %d\n", enable);

    return rv;
}


int interface_delete(int unit, int interface_id)
{
    int  rv = BCM_E_NONE;
    int enable;

    rv = bcm_time_interface_delete(unit, interface_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_time_interface_delete rv = %d\n", rv);
        return rv;
    }

    printf("interface %d deleted successfully\n", interface_id);

    return rv;
}
