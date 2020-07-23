
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIFODMA_ACCESS_H__
#define __DNX_FIFODMA_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_fifodma_types.h>
#include <include/bcm_int/dnx/l2/l2_learn.h>
#include <include/bcm_int/dnx/sat/sat.h>
#include <include/soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <src/bcm/dnx/oam/oam_oamp.h>



typedef int (*dnx_fifodma_info_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_fifodma_info_init_cb)(
    int unit);


typedef int (*dnx_fifodma_info_entry_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnx_fifodma_info_entry_max_entries_set_cb)(
    int unit, uint32 entry_idx_0, uint32 max_entries);


typedef int (*dnx_fifodma_info_entry_max_entries_get_cb)(
    int unit, uint32 entry_idx_0, uint32 *max_entries);


typedef int (*dnx_fifodma_info_entry_is_mem_set_cb)(
    int unit, uint32 entry_idx_0, int is_mem);


typedef int (*dnx_fifodma_info_entry_is_mem_get_cb)(
    int unit, uint32 entry_idx_0, int *is_mem);


typedef int (*dnx_fifodma_info_entry_mem_set_cb)(
    int unit, uint32 entry_idx_0, int mem);


typedef int (*dnx_fifodma_info_entry_mem_get_cb)(
    int unit, uint32 entry_idx_0, int *mem);


typedef int (*dnx_fifodma_info_entry_reg_set_cb)(
    int unit, uint32 entry_idx_0, int reg);


typedef int (*dnx_fifodma_info_entry_reg_get_cb)(
    int unit, uint32 entry_idx_0, int *reg);


typedef int (*dnx_fifodma_info_entry_array_index_set_cb)(
    int unit, uint32 entry_idx_0, int array_index);


typedef int (*dnx_fifodma_info_entry_array_index_get_cb)(
    int unit, uint32 entry_idx_0, int *array_index);


typedef int (*dnx_fifodma_info_entry_copyno_set_cb)(
    int unit, uint32 entry_idx_0, int copyno);


typedef int (*dnx_fifodma_info_entry_copyno_get_cb)(
    int unit, uint32 entry_idx_0, int *copyno);


typedef int (*dnx_fifodma_info_entry_force_entry_size_set_cb)(
    int unit, uint32 entry_idx_0, int force_entry_size);


typedef int (*dnx_fifodma_info_entry_force_entry_size_get_cb)(
    int unit, uint32 entry_idx_0, int *force_entry_size);


typedef int (*dnx_fifodma_info_entry_threshold_set_cb)(
    int unit, uint32 entry_idx_0, int threshold);


typedef int (*dnx_fifodma_info_entry_threshold_get_cb)(
    int unit, uint32 entry_idx_0, int *threshold);


typedef int (*dnx_fifodma_info_entry_timeout_set_cb)(
    int unit, uint32 entry_idx_0, int timeout);


typedef int (*dnx_fifodma_info_entry_timeout_get_cb)(
    int unit, uint32 entry_idx_0, int *timeout);


typedef int (*dnx_fifodma_info_entry_is_poll_set_cb)(
    int unit, uint32 entry_idx_0, int is_poll);


typedef int (*dnx_fifodma_info_entry_is_poll_get_cb)(
    int unit, uint32 entry_idx_0, int *is_poll);


typedef int (*dnx_fifodma_info_entry_entry_size_set_cb)(
    int unit, uint32 entry_idx_0, uint32 entry_size);


typedef int (*dnx_fifodma_info_entry_entry_size_get_cb)(
    int unit, uint32 entry_idx_0, uint32 *entry_size);


typedef int (*dnx_fifodma_info_entry_is_alloced_set_cb)(
    int unit, uint32 entry_idx_0, int is_alloced);


typedef int (*dnx_fifodma_info_entry_is_alloced_get_cb)(
    int unit, uint32 entry_idx_0, int *is_alloced);


typedef int (*dnx_fifodma_info_entry_is_started_set_cb)(
    int unit, uint32 entry_idx_0, int is_started);


typedef int (*dnx_fifodma_info_entry_is_started_get_cb)(
    int unit, uint32 entry_idx_0, int *is_started);


typedef int (*dnx_fifodma_info_entry_is_paused_set_cb)(
    int unit, uint32 entry_idx_0, int is_paused);


typedef int (*dnx_fifodma_info_entry_is_paused_get_cb)(
    int unit, uint32 entry_idx_0, int *is_paused);


typedef int (*dnx_fifodma_info_entry_handler_get_cb_cb)(
    int unit, uint32 entry_idx_0, soc_dnx_fifodma_interrupt_handler* cb);


typedef int (*dnx_fifodma_info_entry_handler_register_cb_cb)(
    int unit, uint32 entry_idx_0, char *name);


typedef int (*dnx_fifodma_info_entry_handler_unregister_cb_cb)(
    int unit, uint32 entry_idx_0);


typedef int (*dnx_fifodma_info_entry_last_entry_id_set_cb)(
    int unit, uint32 entry_idx_0, int last_entry_id);


typedef int (*dnx_fifodma_info_entry_last_entry_id_get_cb)(
    int unit, uint32 entry_idx_0, int *last_entry_id);


typedef int (*dnx_fifodma_info_entry_host_buff_lo_set_cb)(
    int unit, uint32 entry_idx_0, uint32 host_buff_lo);


typedef int (*dnx_fifodma_info_entry_host_buff_lo_get_cb)(
    int unit, uint32 entry_idx_0, uint32 *host_buff_lo);


typedef int (*dnx_fifodma_info_entry_host_buff_hi_set_cb)(
    int unit, uint32 entry_idx_0, uint32 host_buff_hi);


typedef int (*dnx_fifodma_info_entry_host_buff_hi_get_cb)(
    int unit, uint32 entry_idx_0, uint32 *host_buff_hi);




typedef struct {
    dnx_fifodma_info_entry_max_entries_set_cb set;
    dnx_fifodma_info_entry_max_entries_get_cb get;
} dnx_fifodma_info_entry_max_entries_cbs;


typedef struct {
    dnx_fifodma_info_entry_is_mem_set_cb set;
    dnx_fifodma_info_entry_is_mem_get_cb get;
} dnx_fifodma_info_entry_is_mem_cbs;


typedef struct {
    dnx_fifodma_info_entry_mem_set_cb set;
    dnx_fifodma_info_entry_mem_get_cb get;
} dnx_fifodma_info_entry_mem_cbs;


typedef struct {
    dnx_fifodma_info_entry_reg_set_cb set;
    dnx_fifodma_info_entry_reg_get_cb get;
} dnx_fifodma_info_entry_reg_cbs;


typedef struct {
    dnx_fifodma_info_entry_array_index_set_cb set;
    dnx_fifodma_info_entry_array_index_get_cb get;
} dnx_fifodma_info_entry_array_index_cbs;


typedef struct {
    dnx_fifodma_info_entry_copyno_set_cb set;
    dnx_fifodma_info_entry_copyno_get_cb get;
} dnx_fifodma_info_entry_copyno_cbs;


typedef struct {
    dnx_fifodma_info_entry_force_entry_size_set_cb set;
    dnx_fifodma_info_entry_force_entry_size_get_cb get;
} dnx_fifodma_info_entry_force_entry_size_cbs;


typedef struct {
    dnx_fifodma_info_entry_threshold_set_cb set;
    dnx_fifodma_info_entry_threshold_get_cb get;
} dnx_fifodma_info_entry_threshold_cbs;


typedef struct {
    dnx_fifodma_info_entry_timeout_set_cb set;
    dnx_fifodma_info_entry_timeout_get_cb get;
} dnx_fifodma_info_entry_timeout_cbs;


typedef struct {
    dnx_fifodma_info_entry_is_poll_set_cb set;
    dnx_fifodma_info_entry_is_poll_get_cb get;
} dnx_fifodma_info_entry_is_poll_cbs;


typedef struct {
    dnx_fifodma_info_entry_entry_size_set_cb set;
    dnx_fifodma_info_entry_entry_size_get_cb get;
} dnx_fifodma_info_entry_entry_size_cbs;


typedef struct {
    dnx_fifodma_info_entry_is_alloced_set_cb set;
    dnx_fifodma_info_entry_is_alloced_get_cb get;
} dnx_fifodma_info_entry_is_alloced_cbs;


typedef struct {
    dnx_fifodma_info_entry_is_started_set_cb set;
    dnx_fifodma_info_entry_is_started_get_cb get;
} dnx_fifodma_info_entry_is_started_cbs;


typedef struct {
    dnx_fifodma_info_entry_is_paused_set_cb set;
    dnx_fifodma_info_entry_is_paused_get_cb get;
} dnx_fifodma_info_entry_is_paused_cbs;


typedef struct {
    dnx_fifodma_info_entry_handler_get_cb_cb get_cb;
    dnx_fifodma_info_entry_handler_register_cb_cb register_cb;
    dnx_fifodma_info_entry_handler_unregister_cb_cb unregister_cb;
} dnx_fifodma_info_entry_handler_cbs;


typedef struct {
    dnx_fifodma_info_entry_last_entry_id_set_cb set;
    dnx_fifodma_info_entry_last_entry_id_get_cb get;
} dnx_fifodma_info_entry_last_entry_id_cbs;


typedef struct {
    dnx_fifodma_info_entry_host_buff_lo_set_cb set;
    dnx_fifodma_info_entry_host_buff_lo_get_cb get;
} dnx_fifodma_info_entry_host_buff_lo_cbs;


typedef struct {
    dnx_fifodma_info_entry_host_buff_hi_set_cb set;
    dnx_fifodma_info_entry_host_buff_hi_get_cb get;
} dnx_fifodma_info_entry_host_buff_hi_cbs;


typedef struct {
    dnx_fifodma_info_entry_alloc_cb alloc;
    
    dnx_fifodma_info_entry_max_entries_cbs max_entries;
    
    dnx_fifodma_info_entry_is_mem_cbs is_mem;
    
    dnx_fifodma_info_entry_mem_cbs mem;
    
    dnx_fifodma_info_entry_reg_cbs reg;
    
    dnx_fifodma_info_entry_array_index_cbs array_index;
    
    dnx_fifodma_info_entry_copyno_cbs copyno;
    
    dnx_fifodma_info_entry_force_entry_size_cbs force_entry_size;
    
    dnx_fifodma_info_entry_threshold_cbs threshold;
    
    dnx_fifodma_info_entry_timeout_cbs timeout;
    
    dnx_fifodma_info_entry_is_poll_cbs is_poll;
    
    dnx_fifodma_info_entry_entry_size_cbs entry_size;
    
    dnx_fifodma_info_entry_is_alloced_cbs is_alloced;
    
    dnx_fifodma_info_entry_is_started_cbs is_started;
    
    dnx_fifodma_info_entry_is_paused_cbs is_paused;
    
    dnx_fifodma_info_entry_handler_cbs handler;
    
    dnx_fifodma_info_entry_last_entry_id_cbs last_entry_id;
    
    dnx_fifodma_info_entry_host_buff_lo_cbs host_buff_lo;
    
    dnx_fifodma_info_entry_host_buff_hi_cbs host_buff_hi;
} dnx_fifodma_info_entry_cbs;


typedef struct {
    dnx_fifodma_info_is_init_cb is_init;
    dnx_fifodma_info_init_cb init;
    
    dnx_fifodma_info_entry_cbs entry;
} dnx_fifodma_info_cbs;






int
dnx_fifodma_info_entry_handler_get_cb(int unit, uint32 entry_idx_0, soc_dnx_fifodma_interrupt_handler* cb);



shr_error_e
soc_dnx_fifodma_interrupt_handler_get_cb(sw_state_cb_t * cb_db, uint8 dryRun, soc_dnx_fifodma_interrupt_handler * cb);




extern dnx_fifodma_info_cbs dnx_fifodma_info;

#endif 
