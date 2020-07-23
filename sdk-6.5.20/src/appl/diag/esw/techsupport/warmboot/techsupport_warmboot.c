/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    techsupport_warmboot.c
 * Purpose: This file implements techsupport utility for warmboot info.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

#ifdef BCM_WARM_BOOT_SUPPORT
/* "warmboot" feature's diag command list for all devices */
char * techsupport_warmboot_diag_cmdlist[] = {
    "warmboot scache",
    "warmboot storage",
    "warmboot usage",
    "warmboot show",
    NULL /* Must be the last element in this structure */
};

/* Structure that maintains register list for "warmboot" feature */
techsupport_reg_t techsupport_warmboot_reg_list[] = {
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains memory list for "warmboot" feature */
static soc_mem_t techsupport_warmboot_memory_table_list[] = {
    INVALIDm /* Must be the last element in this structure */
};

/* "warmboot" feature's diag command list for specific device, currently NULL */
char * techsupport_warmboot_device_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "warmboot" feature's software dump command list for all devices */
char * techsupport_warmboot_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

/* "warmboot" feature's software dump command list for specific device, currently NULL */
char * techsupport_warmboot_device_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Warmboot data structure */
techsupport_data_t techsupport_warmboot_data = {
    techsupport_warmboot_diag_cmdlist,
    techsupport_warmboot_reg_list,
    techsupport_warmboot_memory_table_list,
    techsupport_warmboot_device_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_warmboot_sw_dump_cmdlist,
    techsupport_warmboot_device_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

/* Function:   techsupport_warmboot
 * Purpose :   Implements techsupport utility for warmboot feature.
 * Parameters: unit - Unit number
 *             a - pointer to argument.
 *             techsupport_data_t -structure that maintains per chip per feature debug info.
 * Returns:    CMD_OK :done
 *             CMD_FAIL : INVALID INPUT
 * Notes:      This function is a place holder to implement feature specific objects.
 */
int techsupport_warmboot(int unit, args_t *a, techsupport_data_t *techsupport_data)
{
    return techsupport_feature_process(unit, a, techsupport_data);
}
#endif /* BCM_WARM_BOOT_SUPPORT */
