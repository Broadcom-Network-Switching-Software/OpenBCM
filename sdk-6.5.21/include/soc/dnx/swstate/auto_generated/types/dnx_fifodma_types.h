
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIFODMA_TYPES_H__
#define __DNX_FIFODMA_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm_int/dnx/l2/l2_learn.h>
#include <include/bcm_int/dnx/sat/sat.h>
#include <include/soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <src/bcm/dnx/oam/oam_oamp.h>





typedef struct {
    
    uint32 max_entries;
    
    int is_mem;
    
    int mem;
    
    int reg;
    
    int array_index;
    
    int copyno;
    
    int force_entry_size;
    
    int threshold;
    
    int timeout;
    
    int is_poll;
    
    uint32 entry_size;
    
    int is_alloced;
    
    int is_started;
    
    int is_paused;
    
    sw_state_cb_t handler;
    
    int last_entry_id;
    
    uint32 host_buff_lo;
    
    uint32 host_buff_hi;
} fifodma_info_t;


typedef struct {
    
    fifodma_info_t* entry;
} fifodma_db_t;


#endif 
