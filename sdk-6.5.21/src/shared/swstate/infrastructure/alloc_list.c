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
#include <shared/swstate/alloc_list.h>

#ifdef SW_STATE_DIFF_DEBUG

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE


static shr_sw_state_alloc_list_node_t *shr_sw_state_alloc_list_head[BCM_MAX_NUM_UNITS];
static shr_sw_state_alloc_list_node_t *shr_sw_state_alloc_list_tail[BCM_MAX_NUM_UNITS];

uint32 sw_state_str_to_hex(const char * s) {
    uint32 result = 0;
    int c = 0, i = 0;
    for (i = 0; i < 8; i++) {
        result = result << 4;
        if (c=(*s-'0'),(c>=0 && c <=9)) result|=c;
        else if (c=(*s-'a'),(c>=0 && c <=5)) result|=(c+10);
        ++s;
    }
    return result;
}

int shr_sw_state_create_alloc_list(int unit) {

    shr_sw_state_alloc_list_node_t *ptr;
    SOC_INIT_FUNC_DEFS;

    cli_out("Unit %d, Creating allocation list for diff dump monitoring\n",unit);
    ptr = (shr_sw_state_alloc_list_node_t*)sal_alloc(sizeof(shr_sw_state_alloc_list_node_t), "alloc_list_for_sw_state_diff");
    SW_STATE_ALLOC_LIST_ALLOCATION_CHECK(unit, ptr);
    sal_strncpy(ptr->name,"HEAD",sizeof(ptr->name));
    ptr->prev = NULL;
    ptr->start_offset = 0;
    ptr->length = 0;

    shr_sw_state_alloc_list_head[unit] = shr_sw_state_alloc_list_tail[unit] = ptr;

    cli_out("Unit %d, Allocation list for diff dump monitoring has been created successfully!\n",unit);

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_alloc_list_insert(int unit, char* name, uint32 start_offset, uint32 length) {

    SOC_INIT_FUNC_DEFS;

    if (shr_sw_state_alloc_list_head[unit] == NULL) {
      _SOC_EXIT_WITH_ERR(SOC_E_EMPTY, (BSL_META_U(unit,
                        "Unit:%d Allocation list insertion failed, Uninitiaized list!\n"), unit));
    } else {
          shr_sw_state_alloc_list_node_t *ptr =
              (shr_sw_state_alloc_list_node_t*)sal_alloc(sizeof(shr_sw_state_alloc_list_node_t),
                    "shr_sw_state_alloc_list insertion");
          SW_STATE_ALLOC_LIST_ALLOCATION_CHECK(unit, ptr);

          sal_strncpy(ptr->name, name, sizeof(ptr->name));
          ptr->prev         = shr_sw_state_alloc_list_tail[unit];
          ptr->start_offset = start_offset;
          ptr->length       = length;

          shr_sw_state_alloc_list_tail[unit] = ptr;
    }
    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

/* reversed search */
int shr_sw_state_alloc_list_search(int unit, uint32 offset, char* name, uint32* start_offset) {

    shr_sw_state_alloc_list_node_t *ptr = shr_sw_state_alloc_list_tail[unit];
    SOC_INIT_FUNC_DEFS;

    cli_out("Searching sw_state alloc list for offset [%d]\n",(int)offset);
    while (ptr != shr_sw_state_alloc_list_head[unit]) {
        if(offset >= ptr->start_offset && offset < (ptr->start_offset + ptr->length)) {
            sal_strncpy(name, ptr->name, sizeof(ptr->name));
            *start_offset = ptr->start_offset;
            SOC_EXIT;
        }
        ptr = ptr->prev;
    }
    _SOC_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (BSL_META_U(unit,
              "Unit:%d Allocation list offset not found!\n"), unit));
exit:
    SOC_FUNC_RETURN;
}

int shr_sw_state_alloc_list_destroy(int unit) {
    shr_sw_state_alloc_list_node_t *ptr = shr_sw_state_alloc_list_tail[unit];
    shr_sw_state_alloc_list_node_t *tmp = NULL;
    SOC_INIT_FUNC_DEFS;

    if (shr_sw_state_alloc_list_tail[unit] == NULL) SOC_EXIT;
    cli_out("Destroying sw_state alloc list!\n");

    while (ptr != shr_sw_state_alloc_list_head[unit]) {
        tmp = ptr;
        ptr = ptr->prev;
        sal_free(tmp);
    }
    sal_free(shr_sw_state_alloc_list_head[unit]);
    shr_sw_state_alloc_list_head[unit] = NULL;
    shr_sw_state_alloc_list_tail[unit] = NULL;

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;

}

#endif /* SW_STATE_DIFF_DEBUG */

