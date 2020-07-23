/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    monterey_rsvd4.c
 * Purpose: Maintains all the debug information for rsvd4
 *          feature for Monterey.
*/

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_rsvd4_diag_cmdlist[];
extern char * techsupport_rsvd4_sw_dump_cmdlist[];

/* "rsvd4" feature's diag command list valid only for Monterey */
char * techsupport_rsvd4_monterey_diag_cmdlist[] = {
     NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "rsvd4" feature's software dump command list valid only for Monterey */
char * techsupport_rsvd4_monterey_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "rsvd4" feature for Monterey chipset. */
static soc_mem_t techsupport_rsvd4_monterey_memory_table_list[] = {
    CPRI_DECAP_ULAW_LUT_0m,
    BRCM_RESERVED_CPM_18m ,
    BRCM_RESERVED_CPM_19m ,
    BRCM_RESERVED_CPM_20m ,
    BRCM_RESERVED_CPM_21m ,
    BRCM_RESERVED_CPM_23m ,
    BRCM_RESERVED_CPM_24m ,
    BRCM_RESERVED_CPM_25m ,
    BRCM_RESERVED_CPM_26m ,
    BRCM_RESERVED_CPM_27m ,
    BRCM_RESERVED_CPM_28m ,
    BRCM_RESERVED_CPM_29m ,
    BRCM_RESERVED_CPM_30m ,
    BRCM_RESERVED_CPM_31m ,
    BRCM_RESERVED_CPM_32m ,
    BRCM_RESERVED_CPM_33m ,
    BRCM_RESERVED_CPM_34m ,
    BRCM_RESERVED_CPM_35m ,
    BRCM_RESERVED_CPM_36m ,
    BRCM_RESERVED_CPM_37m ,
    BRCM_RESERVED_CPM_38m ,
    BRCM_RESERVED_CPM_39m ,
    BRCM_RESERVED_CPM_40m ,
    BRCM_RESERVED_CPM_41m ,
    BRCM_RESERVED_CPM_42m ,
    BRCM_RESERVED_CPM_60m ,
    BRCM_RESERVED_CPM_61m ,
    BRCM_RESERVED_CPM_62m ,
    BRCM_RESERVED_CPM_63m ,
    BRCM_RESERVED_CPM_44m ,
    BRCM_RESERVED_CPM_45m ,
    BRCM_RESERVED_CPM_46m ,
    BRCM_RESERVED_CPM_47m ,
    BRCM_RESERVED_CPM_48m ,
    BRCM_RESERVED_CPM_49m ,
    BRCM_RESERVED_CPM_50m ,
    BRCM_RESERVED_CPM_51m ,
    BRCM_RESERVED_CPM_52m ,
    BRCM_RESERVED_CPM_53m ,
    BRCM_RESERVED_CPM_54m ,
    BRCM_RESERVED_CPM_55m ,
    BRCM_RESERVED_CPM_56m ,
    BRCM_RESERVED_CPM_57m ,
    BRCM_RESERVED_CPM_58m ,
    BRCM_RESERVED_CPM_59m ,
    BRCM_RESERVED_CPM_335m,
    BRCM_RESERVED_CPM_336m,
    BRCM_RESERVED_CPM_337m,
    BRCM_RESERVED_CPM_338m,
    BRCM_RESERVED_CPM_339m,
    BRCM_RESERVED_CPM_340m,
    BRCM_RESERVED_CPM_341m,
    BRCM_RESERVED_CPM_342m,
   INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "rsvd4" feature for Monterey chipset. */
techsupport_reg_t techsupport_rsvd4_monterey_reg_list[] = {
    { BRCM_RESERVED_CPM_0r  , register_type_per_port },
    { BRCM_RESERVED_CPM_1r  , register_type_per_port },
    { BRCM_RESERVED_CPM_2r, register_type_global },
    { BRCM_RESERVED_CPM_3r, register_type_global },
    { BRCM_RESERVED_CPM_4r  , register_type_per_port },
    { BRCM_RESERVED_CPM_5r  , register_type_per_port },
    { BRCM_RESERVED_CPM_6r  , register_type_per_port },
    { BRCM_RESERVED_CPM_7r  , register_type_per_port },
    { BRCM_RESERVED_CPM_8r  , register_type_per_port },
    { BRCM_RESERVED_CPM_9r  , register_type_per_port },
    { BRCM_RESERVED_CPM_12r , register_type_per_port },
    { BRCM_RESERVED_CPM_13r , register_type_per_port },
    { BRCM_RESERVED_CPM_14r , register_type_per_port },
    { BRCM_RESERVED_CPM_15r , register_type_per_port },
    { BRCM_RESERVED_CPM_16r , register_type_per_port },
    { BRCM_RESERVED_CPM_300r, register_type_per_port },
    { BRCM_RESERVED_CPM_302r, register_type_per_port },
    { BRCM_RESERVED_CPM_303r, register_type_per_port },
    { BRCM_RESERVED_CPM_304r, register_type_per_port },
    { BRCM_RESERVED_CPM_305r, register_type_per_port },
    { BRCM_RESERVED_CPM_307r, register_type_per_port },
    { BRCM_RESERVED_CPM_308r, register_type_per_port },
    { BRCM_RESERVED_CPM_309r, register_type_per_port },
    { BRCM_RESERVED_CPM_310r, register_type_per_port },
    { BRCM_RESERVED_CPM_311r, register_type_per_port },
    { BRCM_RESERVED_CPM_312r, register_type_per_port },
    { BRCM_RESERVED_CPM_313r, register_type_per_port },
    { BRCM_RESERVED_CPM_314r, register_type_per_port },
    { BRCM_RESERVED_CPM_315r, register_type_per_port },
    { BRCM_RESERVED_CPM_316r, register_type_per_port },
    { BRCM_RESERVED_CPM_317r, register_type_per_port },
    { BRCM_RESERVED_CPM_318r, register_type_per_port },
    { BRCM_RESERVED_CPM_319r, register_type_per_port },
    { BRCM_RESERVED_CPM_320r, register_type_per_port },
    { BRCM_RESERVED_CPM_321r, register_type_per_port },
    { BRCM_RESERVED_CPM_322r, register_type_per_port },
    { BRCM_RESERVED_CPM_323r, register_type_per_port },
    { BRCM_RESERVED_CPM_324r, register_type_per_port },
    { BRCM_RESERVED_CPM_325r, register_type_per_port },
    { BRCM_RESERVED_CPM_326r, register_type_per_port },
    { BRCM_RESERVED_CPM_327r, register_type_per_port },
    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains  diag cmdlist, reg_list, mem_list  for
 * "rsvd4" feature for Monterey chipset. */
techsupport_data_t techsupport_rsvd4_monterey_data = {
    techsupport_rsvd4_diag_cmdlist,
    techsupport_rsvd4_monterey_reg_list,
    techsupport_rsvd4_monterey_memory_table_list,
    techsupport_rsvd4_monterey_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_rsvd4_sw_dump_cmdlist,
    techsupport_rsvd4_monterey_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};

