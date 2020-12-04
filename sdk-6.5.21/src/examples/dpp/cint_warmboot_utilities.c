/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Warmboot~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_warmboot_utilities.c
 * Purpose: Example of how to save and restore Software Databases
 *
 * Provides the following functionalities
 *
 * - Sync device state
 *   Application can sync the state. Useful for providing software upgrade functionality.
 *   Entire device state is synched.
 *   
 * - Setup device in a mode to sync relevant state only on configuration changes.
 *   This mode will provide the path to recovery from system crashes.
 *
 * - Release all driver resources (device continues to operate)
 *   Primarily useful for managed software upgrades.
 *   Could also be used by test scripts that do not cleanup the previous state
 *   (although this is not the primariy purpose of this API. It just aids in
 *   overcoming test limitation on cleanup).
 *
 * The following SOC propreties need to be setup warmboot/coldboot
 *  - stable_location=3
 *  - stable_filename=./warmboot_data
 *  - stable_size=1000000
 *  - stable_flags=0
 * 
 * The following environment variable needs to be setup for warmboot
 *  - setenv SOC_BOOT_FLAGS 0x200000
 * 
 * The following environment variable needs to be setup for coldboot
 *  - setenv SOC_BOOT_FLAGS 0x000000
 */


int
wb_sync_initiate(int unit)
{
    int                           rc = BCM_E_NONE;


    rc = bcm_switch_control_set(unit, bcmSwitchControlSync, 1);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_control_set-bcmSwitchControlSync, failed to sync, rc= %d.\n", rc);
        return(rc);
    }

    return(rc);
}

int
wb_immediate_sync_mode_set(int unit, int is_enable)
{
    int                           rc = BCM_E_NONE;


    if ( (is_enable != TRUE) && (is_enable != FALSE) )  {
        return(BCM_E_PARAM);
    }

    rc = bcm_switch_control_set(unit, bcmSwitchControlAutoSync, is_enable);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_control_set-bcmSwitchControlAutoSync, failed to setup sync mode, rc= %d.\n", rc);
        return(rc);
    }

    return(rc);
}

int
wb_immediate_sync_mode_get(int unit, int *is_enable)
{
    int                           rc = BCM_E_NONE;


    if (is_enable == NULL) {
        return(BCM_E_PARAM);
    }

    rc = bcm_switch_control_get(unit, bcmSwitchControlAutoSync, is_enable);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_control_get-bcmSwitchControlAutoSync, failed to retreive sync mode, rc= %d.\n", rc);
        return(rc);
    }

    return(rc);
}

int
wb_deinit_init(int unit, int sync)
{
    int                           rc = BCM_E_NONE;

    if (sync) {
        bshell(unit, "tr 141 Warmboot=1");
    }
    else {

        bshell(unit, "tr 141 NoSync Warmboot=1");

    }
    return(rc);
}

/*
 * NOTE: The following routine is not complete as all symbols are not exported to CINT.
 *       It is best to execute the "EXIT" command from the Broadcom CLI.
 */
int
driver_graceful_resource_release(int unit)
{
    int                           rc = BCM_E_NONE;


    /* make sure unit is attached */
    rc = bcm_attach_check(unit);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in bcm_attach_check failure, rc= %d.\n", rc);
        return(rc);
    }

/*
    rc = _bcm_shutdown(unit);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in _bcm_shutdown, rc= %d.\n", rc);
        return(rc);
    }

    rc = soc_shutdown(unit);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in _soc_shutdown, rc= %d.\n", rc);
        return(rc);
    }
*/

    /* detach device */
    /* Incomplete, added since the above symbols are not exposed to CINT */
    rc = bcm_detach(unit);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in bcm_detach, rc= %d.\n", rc);
        return(rc);
    }

    return(rc);
}

