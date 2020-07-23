
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MULTITHREAD_ANALYZER_TYPES_H__
#define __MULTITHREAD_ANALYZER_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/callbacks/sw_state_htb_callbacks.h>
#include <soc/dnxc/swstate/types/sw_state_hash_table.h>


#define MTA_MAX_NOF_MUTEXES 4



typedef const char * mta_func_name_ptr;

typedef const char * mta_mutex_name_ptr;



typedef struct {
    mta_mutex_name_ptr mutex_name[MTA_MAX_NOF_MUTEXES];
    mta_func_name_ptr function_name;
    uint32 resource_id;
    uint8 thread_id;
    uint8 resource_type;
    uint8 is_write;
} thread_access_log_entry_t;

typedef struct {
    
    sw_state_htbl_t htb;
} multithread_analyzer_t;


#endif 
