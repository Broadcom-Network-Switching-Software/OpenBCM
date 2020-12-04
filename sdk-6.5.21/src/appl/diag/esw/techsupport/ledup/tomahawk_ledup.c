/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tomahawk_ledup.c
 * Purpose: Maintains all the debug information for ledup
 *          feature for Tomahawk.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_ledup_diag_cmdlist[];
extern char * techsupport_ledup_sw_dump_cmdlist[];

/* "ledup" feature's diag command list valid only for Tomahawk */
char * techsupport_ledup_tomahawk_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "ledup" feature's software dump command list valid only for Tomahawk */
char * techsupport_ledup_tomahawk_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "ledup" feature for Tomahawk chipset. */
static soc_mem_t techsupport_ledup_tomahawk_memory_table_list[] = {
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "ledup" feature for Tomahawk chipset. */
techsupport_reg_t techsupport_ledup_tomahawk_reg_list[] = {
    {CMIC_LEDUP0_CTRLr, register_type_global },
    {CMIC_LEDUP0_DATA_RAMr, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r, register_type_global },
    {CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, register_type_global },
    {CMIC_LEDUP0_PROGRAM_RAMr, register_type_global },
    {CMIC_LEDUP0_SCANCHAIN_ASSEMBLY_ST_ADDRr, register_type_global },
    {CMIC_LEDUP0_SCANOUT_COUNT_UPPERr, register_type_global },
    {CMIC_LEDUP0_STATUSr, register_type_global },
    {CMIC_LEDUP0_TM_CONTROLr, register_type_global },
    {CMIC_LEDUP1_CTRLr, register_type_global },
    {CMIC_LEDUP1_DATA_RAMr, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_36_39r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_40_43r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_44_47r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_48_51r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_52_55r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_56_59r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_60_63r, register_type_global },
    {CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r, register_type_global },
    {CMIC_LEDUP1_PROGRAM_RAMr, register_type_global },
    {CMIC_LEDUP1_SCANCHAIN_ASSEMBLY_ST_ADDRr, register_type_global },
    {CMIC_LEDUP1_SCANOUT_COUNT_UPPERr, register_type_global },
    {CMIC_LEDUP1_STATUSr, register_type_global },
    {CMIC_LEDUP1_TM_CONTROLr, register_type_global },
    {CMIC_LEDUP2_CTRLr, register_type_global },
    {CMIC_LEDUP2_DATA_RAMr, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_0_3r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_12_15r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_16_19r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_20_23r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_24_27r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_28_31r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_32_35r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_36_39r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_4_7r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_40_43r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_44_47r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_48_51r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_52_55r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_56_59r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_60_63r, register_type_global },
    {CMIC_LEDUP2_PORT_ORDER_REMAP_8_11r, register_type_global },
    {CMIC_LEDUP2_PROGRAM_RAMr, register_type_global },
    {CMIC_LEDUP2_SCANCHAIN_ASSEMBLY_ST_ADDRr, register_type_global },
    {CMIC_LEDUP2_SCANOUT_COUNT_UPPERr, register_type_global },
    {CMIC_LEDUP2_STATUSr, register_type_global },
    {CMIC_LEDUP2_TM_CONTROLr, register_type_global },
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "ledup" feature for Tomahawk chipset. */
techsupport_data_t techsupport_ledup_tomahawk_data = {
    techsupport_ledup_diag_cmdlist,
    techsupport_ledup_tomahawk_reg_list,
    techsupport_ledup_tomahawk_memory_table_list,
    techsupport_ledup_tomahawk_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_ledup_sw_dump_cmdlist,
    techsupport_ledup_tomahawk_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

