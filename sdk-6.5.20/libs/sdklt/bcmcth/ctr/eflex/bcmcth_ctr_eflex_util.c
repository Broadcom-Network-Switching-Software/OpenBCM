/*! \file bcmcth_ctr_eflex_util.c
 *
 * BCMCTH Enhanced Flex Counter internal utility routines
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_fmm.h>

#include <bcmcfg/bcmcfg_lt.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

int
bcmcth_eflex_action_profile_debug_print(int unit,
                                        bool insert,
                                        ctr_eflex_action_profile_data_t *entry)
{
    uint8_t pipe_unique;

    SHR_FUNC_ENTER(unit);

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "%s %s Entry:\n"
            "action_profile_id %d action_id %d, zone %s, pool_idx %d\n"
            "pipe_unique %d, pipe_idx %d\n"
            "base_idx_auto %d, base_idx %d base_idx_oper %d\n"
            "num_counters %d, num_update_mode %d shadow %d \n"
            "ctr_mode %d, ctr_update_mode_a %d, ctr_update_mode_b %d\n"
            "obj[0] %d, obj[1] %d, obj[2] %d, obj[3] %d\n"
            "profile_id_idx %d, profile_id_a %d, profile_id_b %d\n"
            "ctr_src_a %d, ctr_src_b %d\n"
            "cnt_on_drp %d, cnt_on_hw_drop %d cnt_on_mirror(egr) %d \n"
            "val_data %d, evict_comp %d, evict_reset %d\n"),
            insert ? "Insert" : "Update",
            entry->ingress ? "Ingress" : "Egress",
            entry->action_profile_id, entry->action_id,
            entry->zone ? "FLEX" : "FIXED",
            entry->pool_idx, pipe_unique,
            entry->pipe_idx,
            entry->base_idx_auto, entry->base_idx, entry->base_idx_oper,
            entry->num_ctrs, (uint32_t)entry->num_update_mode, entry->shadow,
            entry->ctr_mode, entry->ctr_update_mode[UPDATE_MODE_A_IDX],
            entry->ctr_update_mode[UPDATE_MODE_B_IDX],
            entry->obj[0], entry->obj[1], entry->obj[2], entry->obj[3],
            entry->op_profile_id_ctr_index, entry->op_profile_id_ctr_a,
            entry->op_profile_id_ctr_b, entry->ctr_src_a, entry->ctr_src_b,
            entry->count_on_rule_drop, entry->count_on_hw_drop,
            entry->count_on_mirror_egress,
            entry->ctr_val_data, entry->evict_comp, entry->evict_reset));

    if (entry->comp == STATE_EFLEX) {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "%s %s Entry:\n"
                "instance %d\n"
                "upd_comp_a %d, upd_comp_b %d\n"
                "upd_true_mode_a %d, upd_true_mode_b %d\n"
                "upd_false_mode_a %d, upd_false_mode_b %d\n"
                "op_id_comp_a %d, op_id_comp_b %d\n"
                "op_id_upd_true_a %d, op_id_upd_true_b %d\n"
                "op_id_upd_false_a %d, op_id_upd_false_b %d\n"
                "ctr_src_comp_a %d, ctr_src_comp_b %d\n"
                "ctr_src_upd_true_a %d, ctr_src_upd_true_b %d\n"
                "ctr_src_upd_false_a %d, ctr_src_upd_false_b %d\n"),
                insert ? "Insert" : "Update",
                entry->ingress ? "Ingress" : "Egress",
                entry->ingress ? entry->state_ext.inst :
                                 entry->state_ext.egr_inst,
                entry->state_ext.upd_compare_op[UPDATE_MODE_A_IDX],
                entry->state_ext.upd_compare_op[UPDATE_MODE_B_IDX],
                entry->state_ext.upd_true_mode[UPDATE_MODE_A_IDX],
                entry->state_ext.upd_true_mode[UPDATE_MODE_B_IDX],
                entry->state_ext.upd_false_mode[UPDATE_MODE_A_IDX],
                entry->state_ext.upd_false_mode[UPDATE_MODE_B_IDX],
                entry->state_ext.op_profile_id_comp[UPDATE_MODE_A_IDX],
                entry->state_ext.op_profile_id_comp[UPDATE_MODE_B_IDX],
                entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_A_IDX],
                entry->state_ext.op_profile_id_upd_true[UPDATE_MODE_B_IDX],
                entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_A_IDX],
                entry->state_ext.op_profile_id_upd_false[UPDATE_MODE_B_IDX],
                entry->state_ext.ctr_src_comp[UPDATE_MODE_A_IDX],
                entry->state_ext.ctr_src_comp[UPDATE_MODE_B_IDX],
                entry->state_ext.ctr_src_upd_true[UPDATE_MODE_A_IDX],
                entry->state_ext.ctr_src_upd_true[UPDATE_MODE_B_IDX],
                entry->state_ext.ctr_src_upd_false[UPDATE_MODE_A_IDX],
                entry->state_ext.ctr_src_upd_false[UPDATE_MODE_B_IDX]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex counter pool SID
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in] pool    Pool number of table.
 * \param [in] inst    Pipe instance.
 * \param [out] table_sid Counter table SID for this pool.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Illegal pool number.
 */
static int
bcmcth_ctr_eflex_pool_sid_get(int unit,
                              bool ingress,
                              uint32_t pool,
                              uint32_t inst,
                              bcmdrd_sid_t *table_sid)
{
    bcmdrd_sid_t *counter_table_list = NULL;
    uint32_t num_counter_table = 0;
    uint8_t num_pipe_instance = 0;
    uint8_t num_pool = 0;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(table_sid, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_pipe_instance = device_info->num_pipe_instance;

    /*
     * For device has no instance concept, the value may be initialized as 0,
     * Adjusted to 1 for unified process.
     */
    if (!num_pipe_instance) {
        num_pipe_instance = 1;
    }

    num_pool = ingress ? device_info->num_ingress_pools :
                         device_info->num_egress_pools;

    if (pool >= num_pool) {
        return SHR_E_PARAM;
    }

    if (inst >= num_pipe_instance) {
        return SHR_E_PARAM;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_ctr_table_get(unit,
                                        ingress,
                                        &counter_table_list,
                                        &num_counter_table));

    if ((num_pool * num_pipe_instance) != num_counter_table) {
        return SHR_E_PARAM;
    }

    SHR_NULL_CHECK(counter_table_list, SHR_E_INTERNAL);

    *table_sid = counter_table_list[pool + num_pool * inst];

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex state counter pool SID
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in] pool    Pool number of table.
 * \param [in] inst    Ingress instance.
 * \param [out] table_sid Counter table SID for this pool.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Illegal pool number.
 */
static int
bcmcth_state_eflex_pool_sid_get(int unit,
                                bool ingress,
                                uint32_t pool,
                                state_eflex_inst_t inst,
                                bcmdrd_sid_t *table_sid)
{
    bcmdrd_sid_t *counter_table_list = NULL;
    uint32_t num_counter_table = 0;
    uint8_t num_pool = 0;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(table_sid, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, STATE_EFLEX, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    num_pool = ingress ? device_info->num_ingress_pools :
                         device_info->num_egress_pools;

    if (pool >= num_pool) {
        return SHR_E_PARAM;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_state_eflex_ctr_table_get(unit,
                                        ingress,
                                        inst,
                                        &counter_table_list,
                                        &num_counter_table));

    SHR_NULL_CHECK(counter_table_list, SHR_E_INTERNAL);

    *table_sid = counter_table_list[pool];

exit:
    SHR_FUNC_EXIT();
}

/* Function to get pool sid */
int
bcmcth_eflex_pool_sid_get(int unit,
                          bool ingress,
                          eflex_comp_t comp,
                          uint32_t ctr_inst,
                          uint32_t state_inst,
                          uint32_t pool_idx,
                          bcmdrd_sid_t *ctr_table_sid)
{
    SHR_FUNC_ENTER(unit);

    /* Get counter pool sid */
    if (comp == CTR_EFLEX) {
        SHR_IF_ERR_EXIT
            (bcmcth_ctr_eflex_pool_sid_get(unit,
                                           ingress,
                                           pool_idx,
                                           ctr_inst,
                                           ctr_table_sid));
    } else if (comp == STATE_EFLEX) {
        SHR_IF_ERR_EXIT
            (bcmcth_state_eflex_pool_sid_get(unit,
                                             ingress,
                                             pool_idx,
                                             state_inst,
                                             ctr_table_sid));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_fields_free(int unit, bcmltd_fields_t* in)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    if (in->field) {
        for (i = 0; i < in->count; i++) {
            if (in->field[i]) {
                shr_fmm_free(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_fields_alloc(int unit, bcmltd_fields_t* in, size_t num_fields)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);

    /* Allocate fields buffers */
    SHR_ALLOC(in->field,
              sizeof(bcmltd_field_t *) * num_fields,
              "bcmcthCtrEflexFields");

    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);

    sal_memset(in->field, 0, sizeof(bcmltd_field_t *) * num_fields);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        in->field[i] = shr_fmm_alloc();
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

    SHR_FUNC_EXIT();

exit:
    bcmcth_ctr_eflex_fields_free(unit, in);
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_num_pipes_get(int unit,
                               bool ingress,
                               eflex_comp_t comp,
                               uint32_t pipe_idx,
                               uint32_t *start_pipe_idx,
                               uint32_t *end_pipe_idx)
{
    int num_pipes;
    uint8_t pipe_unique;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    bool valid = true;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;
    num_pipes = device_info->num_pipes;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            ingress,
                                            comp,
                                            &pipe_unique));

    /* Determine first and last pipe number */
    if (!pipe_unique) {
        *start_pipe_idx = 0;
        *end_pipe_idx = num_pipes;
    } else {
        *start_pipe_idx = pipe_idx;
        *end_pipe_idx = pipe_idx + 1;
    }

    /* Check pipe_idx is valid in pipe unique mode */
    SHR_IF_ERR_EXIT
        (bcmcth_ctr_eflex_pipe_valid_check(unit,
                                           ingress,
                                           pipe_idx,
                                           &valid));

    if (pipe_unique && (valid == false)) {
        *start_pipe_idx = 0;
        *end_pipe_idx = 0;
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_pipe_opermode_get(int unit,
                                   bool ingress,
                                   eflex_comp_t comp,
                                   uint8_t *pipe_unique)
{
    SHR_FUNC_ENTER(unit);

    switch (comp) {
        case CTR_EFLEX:
            *pipe_unique = ingress ? ctr_eflex_ing_pipe_opermode[unit] :
                                     ctr_eflex_egr_pipe_opermode[unit];
            break;
        case STATE_EFLEX:
            /* Flex state always operates in pipe unique mode */
            *pipe_unique = 1;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_pipe_opermode_cfg_init(int unit,
                                        bool ingress,
                                        eflex_comp_t comp,
                                        uint8_t *mode)
{
    int rv = SHR_E_NONE;
    uint64_t val = 0;
    bcmlrd_sid_t sid = 0;
    bcmltd_fid_t fid = 0;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    *mode = PIPE_OPER_MODE_GLOBAL;

    if (comp == CTR_EFLEX) {
        sid = CTR_EFLEX_CONFIGt;
        if (ingress) {
            fid = CTR_EFLEX_CONFIGt_CTR_ING_EFLEX_OPERMODE_PIPEUNIQUEf;
        } else {
            fid = CTR_EFLEX_CONFIGt_CTR_EGR_EFLEX_OPERMODE_PIPEUNIQUEf;
        }
    }

    rv = bcmlrd_map_get(unit, CTR_EFLEX_CONFIGt, &map);
    if (SHR_SUCCESS(rv) && map) {
        rv = bcmcfg_field_get(unit, sid, fid, &val);
    }

    if (rv == SHR_E_NONE) {
        *mode = val;
    }

    SHR_FUNC_EXIT();
}

/* Function to check if given id is already in use */
int
bcmcth_ctr_eflex_id_validate(int unit,
                             uint32_t id,
                             ctr_eflex_pool_list_t *list)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(list, SHR_E_PARAM);

    /* Check if given id is within valid range */
    if (id > list->max_track) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check if given id is already in use */
    if (list->inuse_bitmap) {
        if (SHR_BITGET(list->inuse_bitmap, id)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "id %d already in use\n"),id));

            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to set/clear given id in internal resource list */
int
bcmcth_ctr_eflex_id_set(int unit,
                        bool val,
                        uint32_t id,
                        ctr_eflex_pool_list_t *list)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(list, SHR_E_PARAM);

    /* Check if given id is within valid range */
    if (id > list->max_track) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Set/clear given id */
    if (list->inuse_bitmap) {
        if (val) {
            SHR_BITSET(list->inuse_bitmap, id);
        } else {
            SHR_BITCLR(list->inuse_bitmap, id);
        }
    }

exit:
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "Setting id %d to val %d\n"),id, val));

    SHR_FUNC_EXIT();
}

/*! Function to set operational state */
int
bcmcth_ctr_eflex_oper_state_set(int unit,
                                bcmlrd_fid_t oper_state_fid,
                                uint32_t oper_state,
                                bcmltd_fields_t *out)
{
    size_t i;

    SHR_FUNC_ENTER(unit);

    /* Set OPERATIONAL_STATE */
    for (i = 0; i < out->count; i++) {
        if (out->field[i]->id == oper_state_fid) {
            out->field[i]->data = oper_state;
            out->field[i]->flags = 0;
            out->field[i]->next = 0;
            out->field[i]->idx = 0;
            break;
        }
    }

    SHR_FUNC_EXIT();
}


/*! FLEX_STATE_xxx/CTR_xxx_EFLEX read only LTs validate callback functon */
int
bcmcth_ctr_eflex_read_only_lt_validate_cb(
    int unit,
    bcmltd_sid_t sid,
    bcmimm_entry_event_t event_reason,
    const bcmltd_field_t *key_fields,
    const bcmltd_field_t *data_fields,
    void *context)
{
    SHR_FUNC_ENTER(unit);

    /* Insert, Update and Delete not supported for read-only logical tables */
    if ((event_reason == BCMIMM_ENTRY_INSERT) ||
        (event_reason == BCMIMM_ENTRY_DELETE) ||
        (event_reason == BCMIMM_ENTRY_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to check if action profile id is configured for hitbit */
bool
bcmcth_ctr_eflex_is_hitbit(int unit,
                           ctr_eflex_action_profile_data_t *entry)
{
    return (entry->comp == CTR_EFLEX && entry->ctr_mode == WIDE_MODE &&
            entry->ctr_update_mode[UPDATE_MODE_A_IDX] == SETBIT);
}

/*! Function to calculate absolute counter index */
int
bcmcth_ctr_eflex_abs_idx_get(int unit,
                             uint32_t index,
                             ctr_eflex_action_profile_data_t *entry,
                             uint32_t *abs_ctr_idx)
{
    uint32_t max_ctr_num, num_ctr_per_section, num_slim_ctrs = 1;
    uint32_t base_idx;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    /* Number of counters per section */
    num_ctr_per_section = device_info->num_ctr_section;

    /* Get max number of counters per pool */
    max_ctr_num = entry->ingress ? ctrl->ing_pool_ctr_num[entry->pool_idx] :
                                   ctrl->egr_pool_ctr_num[entry->pool_idx];

    /* Get correct base index */
    base_idx = entry->base_idx_auto ? entry->base_idx_oper : entry->base_idx;

    if (entry->ctr_mode == SLIM_MODE) {
        if (device_info->slim_ctr_row == FALSE) {
        /* For SLIM mode, base index is always 0 */
             base_idx = 0;
     }

        /* Get number of slim counters per pool entry */
        num_slim_ctrs = device_info->num_slim_ctrs;
    }

    if (entry->ctr_mode == NORMAL_DOUBLE_MODE) {
        /* In normal double mode we can 2x counters*/
        num_slim_ctrs =2;
    }

    /* Calculate absolute stats counter index */
    *abs_ctr_idx = (entry->pool_idx * max_ctr_num * num_slim_ctrs ) +
                   (base_idx * num_ctr_per_section) +
                   index;

exit:
    SHR_FUNC_EXIT();
}
