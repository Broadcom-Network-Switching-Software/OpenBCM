/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr_utils.h
 * Purpose:     Utils for resource allocation. Especially concerning multi core devices.
 */

#ifndef  INCLUDE_ALLOC_MNGR_UTILS_H
#define  INCLUDE_ALLOC_MNGR_UTILS_H

#include <shared/swstate/sw_state_resmgr.h>

#define BCM_DPP_AM_DEFAULT_NOF_POOLS    1
#define BCM_DPP_AM_DEFAULT_POOL_IDX     0
#define BCM_DPP_AM_INVALID_POOL_IDX     (-1)


/* SW State for alloc manager utility engine */
typedef struct {
    PARSER_HINT_ARR int                         *resource_to_pool_map;
    PARSER_HINT_ARR int                         *template_to_pool_map;
} bcm_dpp_alloc_mngr_utils_info_t;

#define DPP_AM_RES_CHECK_SILENT_MODE(_rv,_flags) \
    if ( ( _rv == BCM_E_RESOURCE ) && (_flags & SW_STATE_RES_TAG_BITMAP_ALLOC_CHECK_ONLY) ) { BCM_RETURN_VAL_EXIT(_rv); } \
    else { BCMDNX_IF_ERR_EXIT(_rv); }

/*
 * GENERAL CORE HANDLING UTILS - START
 */

int 
bcm_dpp_am_resource_to_nof_pools(int unit, int res_type, uint8 *nof_pools);

int 
bcm_dpp_am_template_to_nof_pools(int unit, int res_type, uint8 *nof_pools);

int
bcm_dpp_am_resource_id_to_pool_id_get(int unit, int pool_idx, int resource_id, int *pool_id);

int
bcm_dpp_am_template_id_to_pool_id_get(int unit, int pool_idx, int template_id, int *pool_id);

int 
bcm_dpp_am_pool_id_to_resource_id_get(int unit, int pool_id, int *res_id, int *pool_idx);

int 
bcm_dpp_am_pool_id_to_template_id_get(int unit, int pool_id, int *template_id, int *pool_idx);


/*
 * GENERAL CORE HANDLING UTILS - END
 */


/*
 * RESOURCE MANAGER ENCAPSULATION - START
 */

int 
dpp_am_res_free(int unit, int pool_idx, int res_id, int count, int elem);

int 
dpp_am_res_free_and_status(int unit, int pool_idx, int res_id, int count, int elem, uint32 *flags);

int 
dpp_am_res_alloc(int unit, int pool_idx, int res_id, uint32 flags, int count, int *elem);

int 
dpp_am_res_silent_alloc(int unit, int pool_idx, int res_id, uint32 flags, int count, int *elem);

int 
dpp_am_res_alloc_align(int unit, int pool_idx, int res_id, uint32 flags, int align, int offset, int count, int *elem);

int 
dpp_am_res_silent_alloc_align(int unit, int pool_idx, int res_id, uint32 flags, int align, int offset, int count, int *elem);

int
dpp_am_res_alloc_align_tag(int unit, int pool_idx, int res_id, uint32 flags, int align, int offset, const void *tag, int count, int *elem);

int
dpp_am_res_alloc_tag(int unit, int pool_idx, int res_id, uint32 flags, const void *tag, int count, int *elem);

int
dpp_am_res_tag_set(int unit, int pool_idx, int res_id, uint32 flags, int offset, int count, const void *tag);

int
dpp_am_res_tag_get(int unit, int pool_idx, int res_id, uint32 flags, int elem, const void *tag);

int
dpp_am_res_check(int unit, int pool_idx, int res_id, int count, int elem);

/* Return the amount of used elements in res_id. */
int
dpp_am_res_used_count_get(int unit, int pool_idx, int res_id, int *used_count);

int
dpp_am_res_check_all(int unit, int pool_idx, int res_id, int count, int elem);

int
dpp_am_res_set_pool(int unit, int pool_idx, int res_id, sw_state_res_allocator_t manager, 
                                 int low_id, int count, const void *extras, const char *name);

int
dpp_am_res_unset_pool(int unit, int pool_idx, int res_id);

/*
 * RESOURCE MANAGER ENCAPSULATION - END
 */


/*
 * TEMPLATE MANAGER ENCAPSULATION - START
 */


int 
dpp_am_template_allocate(int unit, int pool_idx, int template_id, uint32 flags, const void *data, int *is_allocated, int *template);

int 
dpp_am_template_allocate_group(int unit, int pool_idx, int template_id, uint32 flags, const void *data, int nof_additions, int *is_allocated, int *template);

int
dpp_am_template_exchange(int unit, int pool_idx, int template_id, uint32 flags, const void *data, int old_template,
                                      int *is_last, int *template, int *is_allocated);

int
dpp_am_template_exchange_test(int unit, int pool_idx, int template_id, uint32 flags, const void *data, int old_template,
                                      int *is_last, int *template, int *is_allocated);

int
dpp_am_template_free(int unit, int pool_idx, int template_id, int template, int *is_last);

int
dpp_am_template_free_group(int unit, int pool_idx, int template_id, int template, int nof_deductions, int *is_last);

int
dpp_am_template_free_all(int unit, int pool_idx, int template_id, int template, int *nof_deductions);

int 
dpp_am_template_clear(int unit, int pool_idx, int template_id);

int
dpp_am_template_data_get(int unit, int pool_idx, int template_id, int template, void *data);

int
dpp_am_template_template_get(int unit, int pool_idx, int template_id, const void *data, int *template);

int
dpp_am_template_ref_count_get(int unit, int pool_idx, int template_id, int template, uint32 *ref_count);

int
dpp_am_template_template_count_get(int unit, int pool_idx, int template_id, int *template_count);

/*
 * TEMPLATE MANAGER ENCAPSULATION - END
 */

#endif /*INCLUDE_ALLOC_MNGR_UTILS_H*/

