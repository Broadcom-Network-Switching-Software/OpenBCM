/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SHR_SW_STATE_ALLOC_LIST_H
#define _SHR_SW_STATE_ALLOC_LIST_H

#ifdef SW_STATE_DIFF_DEBUG

#define SW_STATE_ALLOC_LIST_ALLOCATION_CHECK(unit, ptr) \
    do { \
      if((ptr) == NULL) { \
        _SOC_EXIT_WITH_ERR(SOC_E_MEMORY, (BSL_META_U((unit), \
                          "Unit:%d Allocation list creation failed!\n"), (unit))); \
      } \
    } while (0)

/* allocated elements list for monitoring sw_state_dump diffs more informative */

typedef struct shr_sw_state_alloc_list_node_s {
    struct    shr_sw_state_alloc_list_node_s *prev;
    char      name[100];
    uint32    start_offset;
    uint32    length;
}   shr_sw_state_alloc_list_node_t;

int shr_sw_state_create_alloc_list(int unit);
int shr_sw_state_alloc_list_insert(int unit, char* name, uint32 start_offset, uint32 length);
int shr_sw_state_alloc_list_search(int unit, uint32 offset, char* name, uint32* start_offset);
int shr_sw_state_alloc_list_destroy(int unit);
uint32 sw_state_str_to_hex(const char * s);

#endif /* SW_STATE_DIFF_DEBUG */
#endif /* _SHR_SW_STATE_ALLOC_LIST_H */
