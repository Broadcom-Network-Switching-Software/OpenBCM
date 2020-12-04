/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_etrap.c
 *
 * Purpose:
 *    Maintains all the debug information for etrap feature for tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_etrap_diag_cmdlist[];
extern char * techsupport_etrap_sw_dump_cmdlist[];

/* "etrap" feature's diag command list valid only for tomahawk3 */
char * techsupport_etrap_tomahawk3_diag_cmdlist[] = {
    "ft etrap dump pipe=0 valid=true",
    "ft etrap dump pipe=1 valid=true",
    "ft etrap dump pipe=2 valid=true",
    "ft etrap dump pipe=3 valid=true",
    "ft etrap dump pipe=4 valid=true",
    "ft etrap dump pipe=5 valid=true",
    "ft etrap dump pipe=6 valid=true",
    "ft etrap dump pipe=7 valid=true",
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "etrap" feature's software dump command list valid only for tomahawk3 */
char * techsupport_etrap_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "etrap" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_etrap_tomahawk3_memory_table_list[] = {
    ETRAP_COLOR_EN_EGR_PORT_BMPm,
    ETRAP_INT_PRI_REMAP_TABLEm,
    ETRAP_LKUP_EN_ING_PORTm,
    ETRAP_QUEUE_EN_EGR_PORT_BMPm,
    PORT_COS_MAPm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "etrap" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_etrap_tomahawk3_reg_list[] = {
    { ETRAP_COLOR_EN_INT_PRIr, register_type_global },
    { ETRAP_COLOR_EN_PKT_TYPEr, register_type_global },
    { ETRAP_FILT_CFGr, register_type_global },
    { ETRAP_FILT_EXCEED_CTR_PIPE0r, register_type_global },
    { ETRAP_FILT_EXCEED_CTR_PIPE1r, register_type_global },
    { ETRAP_FILT_EXCEED_CTR_PIPE2r, register_type_global },
    { ETRAP_FILT_EXCEED_CTR_PIPE3r, register_type_global },
    { ETRAP_FILT_EXCEED_CTR_PIPE4r, register_type_global },
    { ETRAP_FILT_EXCEED_CTR_PIPE5r, register_type_global },
    { ETRAP_FILT_EXCEED_CTR_PIPE6r, register_type_global },
    { ETRAP_FILT_EXCEED_CTR_PIPE7r, register_type_global },
    { ETRAP_FILT_THRESHr, register_type_global },
    { ETRAP_FLOW_CFGr, register_type_global },
    { ETRAP_FLOW_DETECT_CTR_PIPE0r, register_type_global },
    { ETRAP_FLOW_DETECT_CTR_PIPE1r, register_type_global },
    { ETRAP_FLOW_DETECT_CTR_PIPE2r, register_type_global },
    { ETRAP_FLOW_DETECT_CTR_PIPE3r, register_type_global },
    { ETRAP_FLOW_DETECT_CTR_PIPE4r, register_type_global },
    { ETRAP_FLOW_DETECT_CTR_PIPE5r, register_type_global },
    { ETRAP_FLOW_DETECT_CTR_PIPE6r, register_type_global },
    { ETRAP_FLOW_DETECT_CTR_PIPE7r, register_type_global },
    { ETRAP_FLOW_ELEPH_THRESHOLDr, register_type_global },
    { ETRAP_FLOW_ELEPH_THR_REDr, register_type_global },
    { ETRAP_FLOW_ELEPH_THR_YELr, register_type_global },
    { ETRAP_FLOW_INS_FAIL_CTR_PIPE0r, register_type_global },
    { ETRAP_FLOW_INS_FAIL_CTR_PIPE1r, register_type_global },
    { ETRAP_FLOW_INS_FAIL_CTR_PIPE2r, register_type_global },
    { ETRAP_FLOW_INS_FAIL_CTR_PIPE3r, register_type_global },
    { ETRAP_FLOW_INS_FAIL_CTR_PIPE4r, register_type_global },
    { ETRAP_FLOW_INS_FAIL_CTR_PIPE5r, register_type_global },
    { ETRAP_FLOW_INS_FAIL_CTR_PIPE6r, register_type_global },
    { ETRAP_FLOW_INS_FAIL_CTR_PIPE7r, register_type_global },
    { ETRAP_FLOW_INS_SUCCESS_CTR_PIPE0r, register_type_global },
    { ETRAP_FLOW_INS_SUCCESS_CTR_PIPE1r, register_type_global },
    { ETRAP_FLOW_INS_SUCCESS_CTR_PIPE2r, register_type_global },
    { ETRAP_FLOW_INS_SUCCESS_CTR_PIPE3r, register_type_global },
    { ETRAP_FLOW_INS_SUCCESS_CTR_PIPE4r, register_type_global },
    { ETRAP_FLOW_INS_SUCCESS_CTR_PIPE5r, register_type_global },
    { ETRAP_FLOW_INS_SUCCESS_CTR_PIPE6r, register_type_global },
    { ETRAP_FLOW_INS_SUCCESS_CTR_PIPE7r, register_type_global },
    { ETRAP_FLOW_RESET_THRESHOLDr, register_type_global },
    { ETRAP_LKUP_EN_INT_PRIr, register_type_global },
    { ETRAP_LKUP_EN_PKT_TYPEr, register_type_global },
    { ETRAP_MONITOR_CONFIGr, register_type_global },
    { ETRAP_MONITOR_MIRROR_CONFIGr, register_type_global },
    { ETRAP_PROC_EN_2r, register_type_global },
    { ETRAP_PROC_ENr, register_type_global },
    { ETRAP_QUEUE_EN_INT_PRIr, register_type_global },
    { ETRAP_QUEUE_EN_PKT_TYPEr, register_type_global },
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "etrap" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_etrap_tomahawk3_data = {
    techsupport_etrap_diag_cmdlist,
    techsupport_etrap_tomahawk3_reg_list,
    techsupport_etrap_tomahawk3_memory_table_list,
    techsupport_etrap_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_etrap_sw_dump_cmdlist,
    techsupport_etrap_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
