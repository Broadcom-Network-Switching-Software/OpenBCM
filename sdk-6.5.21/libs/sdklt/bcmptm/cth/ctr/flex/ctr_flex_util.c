/*! \file ctr_flex_util.c
 *
 * Flex counter utility routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "ctr_flex_internal.h"
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm_ctr_flex.h>

int
bcmptm_ctr_flex_flds_allocate(int unit, bcmltd_fields_t* in, size_t num_fields)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    /* Allocate fields buffers */
    SHR_ALLOC(in->field, sizeof(bcmltd_field_t *) * num_fields, CTR_FLEX_FIELDS);

    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);

    sal_memset(in->field, 0, sizeof(bcmltd_field_t *) * num_fields);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        SHR_ALLOC(in->field[i], sizeof(bcmltd_field_t), CTR_FLEX_FIELDS);
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

    SHR_FUNC_EXIT();

exit:
    bcmptm_ctr_flex_flds_free(unit, in);

    SHR_FUNC_EXIT();
}

int
bcmptm_ctr_flex_flds_free(int unit, bcmltd_fields_t* in)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if (in->field) {
        for (i = 0; i < in->count; i++) {
            if (in->field[i]) {
                SHR_FREE(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }

    SHR_FUNC_EXIT();
}

int
bcmptm_ctr_flex_ireq_read(int unit,
                          bcmltd_sid_t lt_id,
                          bcmdrd_sid_t pt_id,
                          int index,
                          void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    void *pt_ptr_ovr_info = NULL; /* Physical table overrid info pointer */
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = -1;

    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);

    SHR_IF_ERR_EXIT(bcmptm_ltm_ireq_read(unit,
                                         BCMLT_ENT_ATTR_GET_FROM_HW,
                                         pt_id,
                                         &pt_info,
                                         pt_ptr_ovr_info,
                                         rsp_entry_wsize,
                                         lt_id,
                                         entry_data,
                                         &rsp_ltid,
                                         &rsp_flags));


exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ctr_flex_ireq_write(int unit,
                          bcmltd_sid_t lt_id,
                          bcmdrd_sid_t pt_id,
                          int index,
                          void *entry_data)
{
    bcmbd_pt_dyn_info_t pt_info;
    void *pt_ptr_ovr_info = NULL; /* Physical table overrid info pointer */
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    pt_info.index = index;
    pt_info.tbl_inst = -1;

    SHR_IF_ERR_EXIT(bcmptm_ltm_ireq_write(unit,
                                          0,
                                          pt_id,
                                          &pt_info,
                                          pt_ptr_ovr_info,
                                          entry_data,
                                          lt_id,
                                          &rsp_ltid,
                                          &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ctr_flex_pool_mgr_create (pool_list_t *pool_list,
                                 uint32_t max_counters)
{
    uint32_t num_bytes;

    if (pool_list->inuse_bitmap != NULL ||
        max_counters == 0) {
        return SHR_E_INTERNAL;
    }

    num_bytes = SHR_BITALLOCSIZE(max_counters);

    /* Allocate inuse bitmap */
    pool_list->inuse_bitmap = sal_alloc(num_bytes, "bcmptmCtrFlexPoolListBitmap");

    if (pool_list->inuse_bitmap == NULL) {
        return SHR_E_MEMORY;
    }
    sal_memset(pool_list->inuse_bitmap, 0, num_bytes);

    pool_list->max_track = max_counters;
    return SHR_E_NONE;
}

int
bcmptm_ctr_flex_pool_mgr_allocate (pool_list_t *pool_list,
                                   uint32_t num_counters,
                                   uint32_t *base_index)
{
    uint32_t i;
    uint32_t num;
    uint32_t limit;

    num = num_counters;

    if (num > pool_list->max_track) {
        return SHR_E_PARAM;
    }
/*
    if (*base_index + num_counters > pool_list->max_track) {
        return SHR_E_PARAM;
    }
*/
    limit = pool_list->max_track - num;

    for (i = 0; i <= limit; i++) {
        if (SHR_BITNULL_RANGE(pool_list->inuse_bitmap, i, num)) {
            break;
        }
    }

    if (i > limit) {
        return SHR_E_RESOURCE;
    }

    SHR_BITSET_RANGE(pool_list->inuse_bitmap, i, num);
    *base_index = i;
    return SHR_E_NONE;
}

int
bcmptm_ctr_flex_pool_mgr_reserve (pool_list_t *pool_list,
                                  uint32_t base_index,
                                  uint32_t num_counters)
{
    if (base_index + num_counters > pool_list->max_track) {
        return SHR_E_PARAM;
    }

    if (SHR_BITNULL_RANGE(pool_list->inuse_bitmap, base_index, num_counters)==0) {
        return SHR_E_RESOURCE;
    }

    SHR_BITSET_RANGE(pool_list->inuse_bitmap, base_index, num_counters);
    return SHR_E_NONE;
}

int
bcmptm_ctr_flex_pool_mgr_is_base_free (int unit,
                                   bool ingress,
                                   uint32_t pipe,
                                   uint32_t pool,
                                   uint32_t num_counters,
                                   uint32_t base_index)
{
    uint32_t num;
    uint32_t limit;
    pool_list_t *pool_list;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    num = num_counters;

    pool_list = ingress ? &ctrl->ing_pool_list[pool][pipe] :
                          &ctrl->egr_pool_list[pool][pipe] ;

    if (num > pool_list->max_track) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    limit = pool_list->max_track - num;

    if (base_index > limit) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (SHR_BITNULL_RANGE(pool_list->inuse_bitmap, base_index, num)) {
        SHR_EXIT();
    }
    SHR_ERR_EXIT(SHR_E_EXISTS);
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ctr_flex_pool_mgr_is_base_full (int unit,
                                   bool ingress,
                                   uint32_t pipe,
                                   uint32_t pool,
                                   uint32_t num_counters,
                                   uint32_t base_index)
{
    uint32_t num;
    uint32_t limit;
    int count = 0;
    pool_list_t *pool_list;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    num = num_counters;

    pool_list = ingress ? &ctrl->ing_pool_list[pool][pipe] :
                          &ctrl->egr_pool_list[pool][pipe] ;

    if (num > pool_list->max_track) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    limit = pool_list->max_track - num;

    if (base_index > limit) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_BITCOUNT_RANGE(pool_list->inuse_bitmap, count, base_index, num);

    if (num == (uint32_t)count) {
        SHR_EXIT();
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

bool
bcmptm_ctr_flex_pool_mgr_is_pool_full (int unit,
                                       bool ingress,
                                       uint32_t pipe,
                                       uint32_t pool)
{
    uint32_t i;
    ctr_flex_control_t *ctrl = NULL;
    pool_list_t *pool_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    if (ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pool_list = ingress ? &ctrl->ing_pool_list[pool][pipe] :
                          &ctrl->egr_pool_list[pool][pipe] ;

    for (i = 0; i < pool_list->max_track; i++) {
        if (SHR_BITNULL_RANGE(pool_list->inuse_bitmap, i, 1)) {
            SHR_ERR_EXIT(FALSE);
        }
    }
exit:
    if (SHR_FAILURE(_func_rv)) {
        SHR_ERR_EXIT(TRUE);
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ctr_flex_pool_mgr_free (pool_list_t *pool_list,
                               uint32_t base_index,
                               uint32_t num_counters)
{
    if (base_index + num_counters > pool_list->max_track) {
        return SHR_E_PARAM;
    }

    SHR_BITCLR_RANGE(pool_list->inuse_bitmap, base_index, num_counters);
    return SHR_E_NONE;
}

void
bcmptm_ctr_flex_pool_mgr_destroy (pool_list_t *pool_list)
{
    if (pool_list->inuse_bitmap != NULL) {
        sal_free(pool_list->inuse_bitmap);
    }
    pool_list->inuse_bitmap = NULL;
    pool_list->max_track = 0;
}

/*!
 * \brief Program OFFSET_TABLE to direct map.
 *
 * The routine enables all offsets in the OFFSET_MODE 3
 * region of the OFFSET_TABLE to direct map (all zeros).
 * The corresponding count enables are enabled.
 *
 * \param [in] unit     Unit number.
 * \param [in] ingress  Ingress or Egress direction.
 * \param [in] pool     Pool number of offset table to update.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Illegal pool number.
 * \retval SHR_E_MEMORY Unable to alloc memory.
 * \retval SHR_E_UNAVAIL Device Driver unavailable.
 */
int
bcmptm_ctr_flex_enable_direct_map(int unit,
                                  bool ingress,
                                  uint32_t pool_num)
{
    int index;
    uint8_t offset_mode = 3;
    offset_table_entry_t* offset_table_map = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(offset_table_map,
              sizeof(offset_table_entry_t)*MAX_CTR_INSTANCES,
              "bcmptmCtrFlexOffsetTableMap");
    SHR_NULL_CHECK(offset_table_map, SHR_E_MEMORY);
    sal_memset(offset_table_map, 0,
              sizeof(offset_table_entry_t)*MAX_CTR_INSTANCES);

    for (index = 0; index < MAX_CTR_INSTANCES; index++) {
        offset_table_map[index].count_enable = 1;
    }

    SHR_IF_ERR_EXIT
        (update_offset_table(unit,
                        ingress,
                        pool_num,
                        offset_mode,
                        offset_table_map));

exit:
    if (offset_table_map) {
        sal_free(offset_table_map);
    }
   SHR_FUNC_EXIT();
}
/*!
 * \brief Program OFFSET_TABLE to direct map.
 *
 * The routine disables all offsets in the OFFSET_MODE 3
 * region of the OFFSET_TABLE.
 *
 * \param [in] unit     Unit number.
 * \param [in] ingress  Ingress or Egress direction.
 * \param [in] pool     Pool number of offset table to update.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Illegal pool number.
 * \retval SHR_E_MEMORY Unable to alloc memory.
 * \retval SHR_E_UNAVAIL Device Driver unavailable.
 */
int
bcmptm_ctr_flex_disable_direct_map(int unit,
                                   bool ingress,
                                   uint32_t pool_num)
{
    uint8_t offset_mode = 3;
    offset_table_entry_t* offset_table_map = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(offset_table_map,
              sizeof(offset_table_entry_t)*MAX_CTR_INSTANCES,
              "bcmptmCtrFlexOffsetTableMap");
    SHR_NULL_CHECK(offset_table_map, SHR_E_MEMORY);
    sal_memset(offset_table_map, 0,
              sizeof(offset_table_entry_t)*MAX_CTR_INSTANCES);

    SHR_IF_ERR_EXIT
        (update_offset_table(unit,
                        ingress,
                        pool_num,
                        offset_mode,
                        offset_table_map));
exit:
    if (offset_table_map) {
        sal_free(offset_table_map);
    }
   SHR_FUNC_EXIT();

}
