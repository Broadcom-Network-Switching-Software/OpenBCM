/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    trident3_cancun.c
 * Purpose: Maintains all the debug information for cancun
 *          feature for trident3.
*/

#include <soc/drv.h>
#include <appl/diag/techsupport.h>

#ifdef BCM_TRIDENT3_SUPPORT

extern char * techsupport_cancun_diag_cmdlist[];
extern char * techsupport_cancun_sw_dump_cmdlist[];

extern techsupport_reg_t techsupport_cancun_trident3_reg_list[];
extern soc_mem_t techsupport_cancun_trident3_memory_table_list[];

char *techsupport_cancun_trident3_diag_cmdlist[] = {
    NULL
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
char *techsupport_cancun_trident3_sw_dump_cmdlist[] = {
    NULL
};
#endif

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "cancun" feature for Trident3 chipset. */
techsupport_data_t techsupport_cancun_trident3_data = {
    techsupport_cancun_diag_cmdlist,
    techsupport_cancun_trident3_reg_list,
    techsupport_cancun_trident3_memory_table_list,
    techsupport_cancun_trident3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_cancun_sw_dump_cmdlist,
    techsupport_cancun_trident3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
#endif /* BCM_TRIDENT3_SUPPORT */
