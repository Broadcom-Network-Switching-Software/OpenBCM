
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __FLUSH_ACCESS_H__
#define __FLUSH_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/flush_types.h>
#include <bcm/switch.h>



typedef int (*flush_db_context_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*flush_db_context_init_cb)(
    int unit);


typedef int (*flush_db_context_nof_valid_rules_set_cb)(
    int unit, uint32 nof_valid_rules);


typedef int (*flush_db_context_nof_valid_rules_get_cb)(
    int unit, uint32 *nof_valid_rules);


typedef int (*flush_db_context_flush_in_bulk_set_cb)(
    int unit, uint8 flush_in_bulk);


typedef int (*flush_db_context_flush_in_bulk_get_cb)(
    int unit, uint8 *flush_in_bulk);




typedef struct {
    flush_db_context_nof_valid_rules_set_cb set;
    flush_db_context_nof_valid_rules_get_cb get;
} flush_db_context_nof_valid_rules_cbs;


typedef struct {
    flush_db_context_flush_in_bulk_set_cb set;
    flush_db_context_flush_in_bulk_get_cb get;
} flush_db_context_flush_in_bulk_cbs;


typedef struct {
    flush_db_context_is_init_cb is_init;
    flush_db_context_init_cb init;
    
    flush_db_context_nof_valid_rules_cbs nof_valid_rules;
    
    flush_db_context_flush_in_bulk_cbs flush_in_bulk;
} flush_db_context_cbs;





extern flush_db_context_cbs flush_db_context;

#endif 
