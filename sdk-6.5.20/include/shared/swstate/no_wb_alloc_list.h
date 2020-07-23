/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SHR_SW_STATE_NO_WB_ALLOC_LIST_H
#define _SHR_SW_STATE_NO_WB_ALLOC_LIST_H


#define SW_STATE_ALLOC_LIST_ALLOCATION_CHECK(unit, ptr) \
    do { \
      if((ptr) == NULL) { \
        _SOC_EXIT_WITH_ERR(SOC_E_MEMORY, (BSL_META_U((unit), \
                          "Unit:%d Allocation list - allocation failed!\n"), (unit))); \
      } \
    } while (0)

typedef struct shr_sw_state_no_wb_alloc_list_node_t {
    struct    shr_sw_state_no_wb_alloc_list_node_t *prev;
    void      *ptr;
}   shr_sw_state_no_wb_alloc_list_node_t;

typedef struct shr_sw_state_no_wb_alloc_list_s {
    shr_sw_state_no_wb_alloc_list_node_t *tail;
} shr_sw_state_no_wb_alloc_list_t;

int shr_sw_state_no_wb_alloc_list_create(int unit);
int shr_sw_state_no_wb_alloc_list_insert(int unit, void *ptr);
int shr_sw_state_no_wb_alloc_list_delete(int unit, void *ptr);
int shr_sw_state_no_wb_alloc_list_destroy(int unit);

#endif /* _SHR_SW_STATE_NO_WB_ALLOC_LIST_H */
