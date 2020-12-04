/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <sal/core/alloc.h>
#include <shared/swstate/sw_state.h>
#include <shared/swstate/no_wb_alloc_list.h>

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
/* { */
#if !defined(BCM_WARM_BOOT_SUPPORT)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

static shr_sw_state_no_wb_alloc_list_t *no_wb_alloc_list[BCM_MAX_NUM_UNITS];


int shr_sw_state_no_wb_alloc_list_create(int unit) {

    SOC_INIT_FUNC_DEFS;

    no_wb_alloc_list[unit] = (shr_sw_state_no_wb_alloc_list_t*)sal_alloc(sizeof(shr_sw_state_no_wb_alloc_list_t), "no wb allocation list");
    SW_STATE_ALLOC_LIST_ALLOCATION_CHECK(unit, no_wb_alloc_list[unit]);
    no_wb_alloc_list[unit]->tail = NULL;

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_no_wb_alloc_list_insert(int unit, void *ptr) {

    shr_sw_state_no_wb_alloc_list_node_t *link = (shr_sw_state_no_wb_alloc_list_node_t*)
        sal_alloc(sizeof(shr_sw_state_no_wb_alloc_list_node_t), "no wb allocation node");

    SOC_INIT_FUNC_DEFS;

    SW_STATE_ALLOC_LIST_ALLOCATION_CHECK(unit, link);
    link->ptr = ptr;
    link->prev = no_wb_alloc_list[unit]->tail;
    no_wb_alloc_list[unit]->tail = link;

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_no_wb_alloc_list_delete(int unit, void *ptr) {

    shr_sw_state_no_wb_alloc_list_node_t *current = no_wb_alloc_list[unit]->tail;
    shr_sw_state_no_wb_alloc_list_node_t *next = current;

    SOC_INIT_FUNC_DEFS;

    if (no_wb_alloc_list[unit]->tail == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_EMPTY,
                       (BSL_META_U(unit,
                          "Unit:%d allocation list is empty \n"), unit));
    }

    /* navigate through the list */
    while (current->ptr != ptr) {
        if (current->prev == NULL) {
            _SOC_EXIT_WITH_ERR(SOC_E_NOT_FOUND,
               (BSL_META_U(unit,
                  "Unit:%d ptr not found \n"), unit));
        } else {
            next = current;
            current = current->prev;
        }
    }

    if (current == no_wb_alloc_list[unit]->tail) {
        no_wb_alloc_list[unit]->tail = current->prev;
    } else {
        /* bypass */
        next->prev = current->prev;
    }

    sal_free(current);

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_no_wb_alloc_list_destroy(int unit) {

    shr_sw_state_no_wb_alloc_list_node_t *current = no_wb_alloc_list[unit]->tail;
    shr_sw_state_no_wb_alloc_list_node_t *prev;

    SOC_INIT_FUNC_DEFS;

    while (current != NULL) {
        prev = current->prev;
        sal_free(current->ptr);
        sal_free(current);
        current = prev;
    }

    sal_free(no_wb_alloc_list[unit]);
    no_wb_alloc_list[unit] = NULL;

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

#endif /* BCM_WARM_BOOT_SUPPORT */
/* } */
#endif
