/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * $Copyright: (c) 2014 Broadcom Corporation All Rights Reserved.$
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  All rights reserved
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: define enumerators  
 *----------------------------------------------------------------------*/
#ifndef tscomod_dpll_sc_lkup_table_H_
#define tscomod_dpll_sc_lkup_table_H_ 

#include "tsco/tier1/tscomod.h"
#include <phymod/phymod.h>

#define TSCOMOD_DPLL_HW_SPEED_ID_TABLE_SIZE   64
#define TSCOMOD_DPLL_HW_AM_TABLE_SIZE    64
#define TSCOMOD_DPLL_HW_UM_TABLE_SIZE    64


#define TSCOMOD_DPLL_SPEED_ID_TABLE_SIZE  97
#define TSCOMOD_DPLL_SPEED_ID_ENTRY_SIZE  6
#define TSCOMOD_DPLL_AM_TABLE_SIZE  64
#define TSCOMOD_DPLL_AM_ENTRY_SIZE  3
#define TSCOMOD_DPLL_UM_TABLE_SIZE  64
#define TSCOMOD_DPLL_UM_ENTRY_SIZE  2

#define TSCOMOD_DPLL_SPEED_PRIORITY_MAPPING_TABLE_SIZE 1
#define TSCOMOD_DPLL_SPEED_PRIORITY_MAPPING_ENTRY_SIZE 9


typedef struct tscomod_dpll_sc_pmd_entry_t {
    int t_pma_os_mode;
    int pll_mode;
} tscomod_dpll_sc_pmd_entry_st;

extern const tscomod_dpll_sc_pmd_entry_st tscomod_dpll_sc_pmd_entry[];

extern int tscomod_get_mapped_speed(tscomod_spd_intfc_type_t spd_intf, int *speed);
extern int tscomod_mapped_speed_get_osmode(int mapped_speed_id);
extern uint32_t* tsco_dpll_spd_id_entry_53_get(void);
extern uint32_t* tsco_dpll_spd_id_entry_51_get(void);
extern uint32_t* tsco_dpll_spd_id_entry_41_get(void);
extern uint32_t* tsco_dpll_spd_id_entry_100g_4lane_no_fec_53_get(void);
extern uint32_t* tsco_dpll_spd_id_entry_100g_4lane_no_fec_51_get(void);
extern uint32_t* tsco_dpll_am_table_entry_get(void);
extern uint32_t* tsco_dpll_um_table_entry_get(void);
extern uint32_t* tsco_dpll_speed_priority_mapping_table_get(void);
extern uint32_t* tsco_dpll_spd_id_entry_53_gsh_get(void);
extern uint32_t* tsco_dpll_spd_id_entry_51_gsh_get(void);
extern uint32_t* tsco_dpll_spd_id_entry_41_gsh_get(void);

#endif
