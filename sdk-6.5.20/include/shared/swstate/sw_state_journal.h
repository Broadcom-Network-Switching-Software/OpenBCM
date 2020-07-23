/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: this module implement a Roll-Back Journal for the SW State infrastructure layer,
 *       the roll-back is needed in order to implement Crash Recovery (and generic Error Recovery)
 */

#ifndef _SHR_SW_STATE_JOURNAL_H
#define _SHR_SW_STATE_JOURNAL_H

#include <soc/types.h>

#ifdef CRASH_RECOVERY_SUPPORT
int sw_state_journal_init(int unit, uint32 size);
int sw_state_journal_insert(int unit, uint32 offset, uint32 length, uint8 *data, uint8 is_ptr);
int sw_state_journal_roll_back(int unit);
int sw_state_journal_clear(int unit);
int sw_state_journal_update_ptr_offset(int unit, uint32 ptr_offset);
int sw_state_journal_is_after_roll_back_get(int unit, uint8* result);
int sw_state_journal_is_after_roll_back_set(int unit, uint8 value);

/* 
Header:
---------------------------------------------
| size | size left | offset_to_nodeN (last) | 
---------------------------------------------
Journal: 
------------------------------------------------------ 
| data1 | node1 | .... | dataN | nodeN | 00000000.... |
------------------------------------------------------ 
 
nodeN-1 location = nodeN location - nodeN.length   
*/ 
typedef struct sw_state_journal_node_s {
    uint32 offset;
    uint32 length;
    uint8  is_ptr;
    /* followed by data */
} sw_state_journal_node_t;

typedef struct sw_state_journal_s {
    uint32 max_size;
    uint32 size_left;
    uint32 last_node_offset;
    uint32 nof_log_entries;
    uint32 ptr_offset;
} sw_state_journal_t;

#endif

#endif  /* _SHR_SW_STATE_JOURNAL_H */

