/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#ifndef tbhmod_gen3_sc_lkup_table_H_
#define tbhmod_gen3_sc_lkup_table_H_ 

#include "tbhmod_gen3.h"
#include <phymod/phymod.h>

#define TSCBH_GEN3_HW_SPEED_ID_TABLE_SIZE   64
#define TSCBH_GEN3_HW_AM_TABLE_SIZE    64
#define TSCBH_GEN3_HW_UM_TABLE_SIZE    64


#define TSCBH_GEN3_SPEED_ID_TABLE_SIZE  73
#define TSCBH_GEN3_SPEED_ID_ENTRY_SIZE  5
#define TSCBH_GEN3_AM_TABLE_SIZE  64
#define TSCBH_GEN3_AM_ENTRY_SIZE  3
#define TSCBH_GEN3_UM_TABLE_SIZE  64
#define TSCBH_GEN3_UM_ENTRY_SIZE  2

#define TSCBH_GEN3_SPEED_PRIORITY_MAPPING_TABLE_SIZE 1
#define TSCBH_GEN3_SPEED_PRIORITY_MAPPING_ENTRY_SIZE 9

typedef struct tscbh_gen3_sc_pmd_entry_t {
    int t_pma_os_mode;
    int pll_mode;
} tscbh_gen3_sc_pmd_entry_st;

extern const tscbh_gen3_sc_pmd_entry_st tscbh_gen3_sc_pmd_entry[];
extern const tscbh_gen3_sc_pmd_entry_st tscbh_gen3_sc_pmd_entry_312M_ref[];

extern int tbhmod_gen3_get_mapped_speed(tbhmod_gen3_spd_intfc_type_t spd_intf, int *speed);


#endif
