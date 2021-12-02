/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
#ifndef tbhmod_fe_gen2_sc_lkup_table_H_
#define tbhmod_fe_gen2_sc_lkup_table_H_ 

#include "tbhmod_fe_gen2.h"
#include <phymod/phymod.h>

#define TSCBH_FE_GEN2_HW_SPEED_ID_TABLE_SIZE   64
#define TSCBH_FE_GEN2_HW_AM_TABLE_SIZE    64
#define TSCBH_FE_GEN2_HW_UM_TABLE_SIZE    64


#define TSCBH_FE_GEN2_SPEED_ID_TABLE_SIZE  76
#define TSCBH_FE_GEN2_SPEED_ID_ENTRY_SIZE  5
#define TSCBH_FE_GEN2_AM_TABLE_SIZE  64
#define TSCBH_FE_GEN2_AM_ENTRY_SIZE  3
#define TSCBH_FE_GEN2_UM_TABLE_SIZE  64
#define TSCBH_FE_GEN2_UM_ENTRY_SIZE  2

#define TSCBH_FE_GEN2_SPEED_PRIORITY_MAPPING_TABLE_SIZE 1
#define TSCBH_FE_GEN2_SPEED_PRIORITY_MAPPING_ENTRY_SIZE 9

typedef struct tscbh_fe_gen2_sc_pmd_entry_t {
    int t_pma_os_mode;
    int pll_mode;
} tscbh_fe_gen2_sc_pmd_entry_st;

extern const tscbh_fe_gen2_sc_pmd_entry_st tscbh_fe_gen2_sc_pmd_entry[];
extern const tscbh_fe_gen2_sc_pmd_entry_st tscbh_fe_gen2_sc_pmd_entry_312M_ref[];

extern int tbhmod_fe_gen2_get_mapped_speed(tbhmod_fe_gen2_spd_intfc_type_t spd_intf, int *speed);
extern uint32_t* tscbh_fe_gen2_spd_id_entry_get(void);
extern uint32_t* tscbh_fe_gen2_am_table_entry_get(void);
extern uint32_t* tscbh_fe_gen2_um_table_entry_get(void);
extern uint32_t* tscbh_fe_gen2_spd_id_entry_100g_4lane_no_fec_get(void);
extern uint32_t* tscbh_fe_gen2_speed_priority_mapping_table_get(void);


#endif
