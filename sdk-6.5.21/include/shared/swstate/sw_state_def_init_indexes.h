/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: sw_state_def_init_indexes.h
 */
#ifndef _SHR_SW_STATE_DEFS_INIT_INDEXES_H
#define _SHR_SW_STATE_DEFS_INIT_INDEXES_H

#define INIT_INDEXES() 		uint8 is_param_allocated = 0; \
		int temp_size = 0; \
        uint32 allocated_size = 0; \
        uint8 is_allocated = 0; \
		int idx0 = 0; \
        int idx1 = 0; \
        int idx2 = 0; \
        int idx3 = 0; \
		uint32 *ptr0 = NULL; \
        uint32 *ptr1 = NULL; \
        uint32 *ptr2 = NULL; \
        uint32 *ptr3 = NULL; \
		int toIdx0 = 0; \
        int toIdx1 = 0; \
        int toIdx2 = 0; \
        int toIdx3 = 0

#define INIT_INDEXES_ASSIGNMENT() 		is_param_allocated  = is_param_allocated + 0; \
        is_allocated = is_allocated + 0;\
        temp_size = temp_size + 0; \
		toIdx0 = toIdx0 + 0; \
        toIdx1 = toIdx1 + 0; \
        toIdx2 = toIdx2 + 0; \
        toIdx3 = toIdx3 + 0; \
        idx0 = idx0 + 0; \
        idx1 = idx1 + 0; \
        idx2 = idx2 + 0; \
        idx3 = idx3 + 0; \
		ptr0 = ptr0 + 0; \
        ptr1 = ptr1 + 0; \
        ptr2 = ptr2 + 0; \
        ptr3 = ptr3 + 0; \
        allocated_size = allocated_size + 0
#endif /* _SHR_SW_STATE_DEFS_INIT_INDEXES_H */


