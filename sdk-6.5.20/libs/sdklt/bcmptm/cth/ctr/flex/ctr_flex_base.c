/*! \file ctr_flex_base.c
 *
 * Flex counter memory base index managment routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include "ctr_flex_internal.h"

/*!
 * Calls the pool manager to create a pool_list with the specified number of counters.
 */
static int
create_shr_counters (int unit, bool ingress, uint32_t pool, uint32_t max_num_counters)
{
    uint32_t pipe;
    pool_list_t *pool_list;
    ctr_flex_control_t *ctrl = NULL;
    ctr_flex_device_info_t *device_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    device_info = ctrl->ctr_flex_device_info;
    if (device_info == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (pipe = 0; pipe < device_info->num_pipes; pipe++) {
        pool_list = ingress ? &ctrl->ing_pool_list[pool][pipe] :
                              &ctrl->egr_pool_list[pool][pipe] ;

        SHR_IF_ERR_EXIT
            (bcmptm_ctr_flex_pool_mgr_create(
                    pool_list,
                    max_num_counters));
        /*
         * Reserve the first two counters of each pool,
         * since counter_base_index cannot be 0.
         * Refer: Flex Arch spec section 4.1.
         */
         SHR_IF_ERR_EXIT
             (bcmptm_ctr_flex_pool_mgr_reserve(
                     pool_list,
                     0,
                     2));
    }

    SHR_FUNC_EXIT();

exit:
    if (device_info) {
        for (pipe = 0; pipe < device_info->num_pipes; pipe++) {
            pool_list = ingress ? &ctrl->ing_pool_list[pool][pipe] :
                                  &ctrl->egr_pool_list[pool][pipe] ;

            bcmptm_ctr_flex_pool_mgr_destroy(pool_list);
        }
    }
    SHR_FUNC_EXIT();
}

static int
get_num_instances (int unit, active_entry_t *active_entry, uint32_t *num_inst)
{
    bool ingress;
    uint32_t num_instances;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    /*
     * In non-auto mode, use the max_instances supplied by user.
     * In auto-mode, get the actual number of instances from
     * attribute_template_id.
     * Allocate maximum if no instances yet.
     */
    ingress = active_entry->ingress;

    num_instances = ingress ?
        ctrl->ing_attributes[active_entry->entry_data->attr_id].num_instances:
        ctrl->egr_attributes[active_entry->entry_data->attr_id].num_instances;

    if (active_entry->entry_data->max_instances_mode_auto == 0) {
        num_instances = active_entry->entry_data->max_instances;
    }
    else if (num_instances == 0) {
        num_instances = MAX_CTR_INSTANCES;
    }
    *num_inst = num_instances;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Allocate the specified number of counters from the pool_list.
 */
static int
allocate_shr_counters(int unit,
                      active_entry_t *active_entry,
                      uint32_t *base_index,
                      uint32_t num_instances)
{
    int rv;
    size_t i;
    pool_list_t (*pool_base)[MAX_NUM_PIPES];
    pool_list_t *pool_list = NULL;
    ctr_flex_control_t *ctrl = NULL;
    ctr_flex_device_info_t *device_info = NULL;

    int ingress = active_entry->ingress;
    bool fp_mode = active_entry->entry_data->pipe_unique;
    uint32_t pool_num = active_entry->entry_data->pool_id;
    uint32_t pipe_num = active_entry->entry_data->pipe_num;
    bool base_index_mode_auto = active_entry->entry_data->base_index_mode_auto;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    device_info = ctrl->ctr_flex_device_info;
    if (device_info == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pool_base = ingress ? ctrl->ing_pool_list : ctrl->egr_pool_list;
    pool_list = &pool_base[pool_num][pipe_num];

    if (base_index_mode_auto) {
        rv = bcmptm_ctr_flex_pool_mgr_allocate(
                        pool_list,
                        num_instances,
                        base_index);
    } else {
        rv = bcmptm_ctr_flex_pool_mgr_reserve(
                        pool_list,
                        *base_index,
                        num_instances);
    }

    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_RESOURCE) {
            active_entry->entry_data->op_state =
                ctrl->ctr_flex_enum.awaiting_counter_resource;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (fp_mode == 0) {
        for (i = 0; i < device_info->num_pipes; i++) {
            if (i == pipe_num) {
                continue;
            }

            pool_list = &pool_base[pool_num][i];

            SHR_IF_ERR_EXIT
                (bcmptm_ctr_flex_pool_mgr_reserve(
                            pool_list,
                            *base_index,
                            num_instances));
         }
     }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Release the counters from the pool list.
 */
static int
free_shr_counters(int unit,
              bool ingress,
              bool fp_mode,
              uint8_t pipe_num,
              uint32_t pool_num,
              uint8_t offset_mode,
              uint32_t base_index,
              uint32_t num_counters)
{
    size_t i;
    pool_list_t (*pool_base)[MAX_NUM_PIPES];
    pool_list_t *pool_list = NULL;
    int error = 0;
    ctr_flex_control_t *ctrl = NULL;
    ctr_flex_device_info_t *device_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    device_info = ctrl->ctr_flex_device_info;
    if (device_info == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pool_base = ingress ? ctrl->ing_pool_list : ctrl->egr_pool_list;
    pool_list = &pool_base[pool_num][pipe_num];

    error += bcmptm_ctr_flex_pool_mgr_free(
                    pool_list,
                    base_index,
                    num_counters);

    if (fp_mode == 0) {
        /* FP mode pipe global */
        for (i = 0; i < device_info->num_pipes; i++) {
            if (i == pipe_num) continue;

            pool_list = &pool_base[pool_num][i];

            error += bcmptm_ctr_flex_pool_mgr_free(
                            pool_list,
                            base_index,
                            num_counters);
         }
     }

    if (error != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Allocate specified number of counters.
 * Use supplied base_index or allocate a base_index depending on mode_auto.
 */
int
allocate_counters (int unit, active_entry_t *active_entry)
{
    bool ingress;
    uint8_t pipe_num;
    uint32_t pool_id;
    uint32_t pool_size;
    uint32_t base_index;
    bool base_index_mode_auto;
    uint32_t num_instances = 0;
    pool_list_t *pool_list = NULL;
    ctr_flex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ctr_flex_ctrl_get(unit, &ctrl));

    ingress = active_entry->ingress;
    pool_id = active_entry->entry_data->pool_id;
    pipe_num = active_entry->entry_data->pipe_num;
    base_index_mode_auto = active_entry->entry_data->base_index_mode_auto;
    base_index = active_entry->entry_data->base_index;

    if (ingress) {
        pool_size = ctrl->ctr_ing_flex_pool[pool_id].num_counters;
    }
    else {
        pool_size = ctrl->ctr_egr_flex_pool[pool_id].num_counters;
    }

    pool_list = ingress ? &ctrl->ing_pool_list[pool_id][pipe_num] :
                          &ctrl->egr_pool_list[pool_id][pipe_num] ;

    if (pool_list->inuse_bitmap == NULL) {
        /* first time this pool is being requested */
        SHR_IF_ERR_EXIT
            (create_shr_counters (unit,
                                  ingress,
                                  pool_id,
                                  pool_size));
        /*
         * Enable CCI and counter pool.
         * This call blocks for a while.
         */
        SHR_IF_ERR_EXIT
            (enable_counter_pool(unit,
                             ingress,
                             0,
                             pool_id,
                             CTR_FLEX_POOL_ENABLE));
    }

    SHR_IF_ERR_EXIT
        (get_num_instances (unit, active_entry, &num_instances));
    active_entry->entry_data->max_instances = num_instances;

    /* Allocate shared resource */
    SHR_IF_ERR_VERBOSE_EXIT
        (allocate_shr_counters (unit,
                                active_entry,
                                &base_index,
                                num_instances));

    if (base_index_mode_auto) {
        active_entry->entry_data->base_index = base_index;
    }

    exit:
    SHR_FUNC_EXIT();
}

/*!
 * Free specified number of counters, and release the base_index.
 */
int
free_counters (int unit, active_entry_t *active_entry) {

    bool ingress;
    bool pipe_unique;
    uint8_t pipe_num;
    uint32_t pool_id;
    uint32_t base_index;
    uint32_t max_instances;
    uint8_t offset_mode;

    SHR_FUNC_ENTER(unit);

    /* Decode config information from active entry */
    ingress = active_entry->ingress;
    offset_mode = active_entry->entry_data->offset_mode;
    pool_id = active_entry->entry_data->pool_id;
    base_index = active_entry->entry_data->base_index;
    pipe_unique = active_entry->entry_data->pipe_unique;
    pipe_num = active_entry->entry_data->pipe_num;
    max_instances = active_entry->entry_data->max_instances;

    /* Free counters */
        SHR_IF_ERR_EXIT
            (free_shr_counters(unit,
                          ingress,
                          pipe_unique,
                          pipe_num,
                          pool_id,
                          offset_mode,
                          base_index,
                          max_instances));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * Resize the counters to the specified number.
 */
int
resize_counters (int unit, active_entry_t *active_entry, uint32_t current_instances) {
    bool ingress;
    bool pipe_unique;
    uint8_t pipe_num;
    uint8_t offset_mode;
    uint32_t pool_id;
    uint32_t base_index;
    uint32_t num_instances = 0;
    uint32_t excess_instances;
    uint32_t more_instances;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Decode config information from active entry */
    ingress = active_entry->ingress;
    offset_mode = active_entry->entry_data->offset_mode;
    pool_id = active_entry->entry_data->pool_id;
    base_index = active_entry->entry_data->base_index;
    pipe_unique = active_entry->entry_data->pipe_unique;
    pipe_num = active_entry->entry_data->pipe_num;

    /* 1. find number of instances needed */
    SHR_IF_ERR_EXIT
        (get_num_instances (unit, active_entry, &num_instances));
    active_entry->entry_data->max_instances = num_instances;

    /* 2. compare with current number of instances */
    if (current_instances >= num_instances) {
        excess_instances = current_instances - num_instances;
        base_index += num_instances;
        /* Free any excess counters */
        SHR_IF_ERR_EXIT
            (free_shr_counters(unit,
                          ingress,
                          pipe_unique,
                          pipe_num,
                          pool_id,
                          offset_mode,
                          base_index,
                          excess_instances));
    }
    else {
        more_instances = num_instances - current_instances;
        base_index += current_instances;
        active_entry->entry_data->base_index_mode_auto = false;
        /* Allocate more counters */
        rv = allocate_shr_counters (unit,
                                active_entry,
                                &base_index,
                                more_instances);

        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_RESOURCE);

        if (rv == SHR_E_RESOURCE) {
            /*
             * Not enough space; entry is waitlisted for retry.
             * Release current counters.
             */
            SHR_IF_ERR_EXIT
                (free_shr_counters(unit,
                              ingress,
                              pipe_unique,
                              pipe_num,
                              pool_id,
                              offset_mode,
                              active_entry->entry_data->base_index,
                              current_instances));

            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

exit:
    SHR_FUNC_EXIT();
}
