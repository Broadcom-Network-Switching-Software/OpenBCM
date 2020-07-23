/*! \file uft_imm.c
 *
 * Uft interface to in-memory table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <sal/sal_assert.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_table_constants.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_cth_uft_be_internal.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmptm/bcmptm_spm_internal.h>

#include "uft_device.h"
#include "uft_internal.h"
#include "uft_util.h"
#include "sbr_internal.h"

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_UFT


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Check whether the tile's configuration is changed.
 *
 * \param [in] unit Unit number.
 * \param [in] tile New parsed tile configuration.
 * \param [out] chg Returned true if configuration is changed.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
dev_em_tile_chg_check(int unit, uft_dev_em_tile_t *tile, bool *chg)
{
    uint32_t tile_mode, seed;
    bool robust;

    SHR_FUNC_ENTER(unit);

    *chg = FALSE;
    SHR_IF_ERR_EXIT
        (bcmptm_uft_tile_cfg_get(unit, tile->tile_id,
                                 &tile_mode, &robust, &seed));
    if (SHR_BITGET(tile->fbmp, DEVICE_EM_TILEt_MODEf)) {
        if (tile->mode != tile_mode) {
            *chg = TRUE;
            SHR_EXIT();
        } else {
            SHR_BITCLR(tile->fbmp, DEVICE_EM_TILEt_MODEf);
        }
    }

    if (SHR_BITGET(tile->fbmp, DEVICE_EM_TILEt_ROBUSTf)) {
        if (tile->robust != robust) {
            *chg = TRUE;
            SHR_EXIT();
        } else {
            SHR_BITCLR(tile->fbmp, DEVICE_EM_TILEt_ROBUSTf);
        }
    }

    if (SHR_BITGET(tile->fbmp, DEVICE_EM_TILEt_SEEDf)) {
        if (tile->seed != seed) {
            *chg = TRUE;
            SHR_EXIT();
        } else {
            SHR_BITCLR(tile->fbmp, DEVICE_EM_TILEt_SEEDf);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether an EM_GROUP is set.
 * When any EM_GROUP is set, UFT mgr rejects changing UFT mode.
 *
 * \param [in] unit Unit number.
 * \param [in] grp_id Group ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_BUSY DEVICE_EM_GROUP was set.
 */
static int
dev_em_grp_set_check(int unit, uint32_t grp_id)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    uint32_t idx;
    uint32_t v_bank_cnt = 0;

    SHR_FUNC_ENTER(unit);

    in_flds.count  = 1;
    out_flds.count = UFT_FIELD_COUNT_MAX;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &in_flds));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &out_flds));

    in_flds.field[0]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
    in_flds.field[0]->data = grp_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, DEVICE_EM_GROUPt, &in_flds, &out_flds));
    for (idx = 0; idx < out_flds.count; idx++) {
        if (out_flds.field[idx]->id == DEVICE_EM_GROUPt_NUM_BANKSf) {
            v_bank_cnt = out_flds.field[idx]->data;
            if (v_bank_cnt > 0) {
                SHR_ERR_EXIT(SHR_E_BUSY);
            }
        }
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &in_flds);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &out_flds);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether an EM_GROUP is pre-assigned with fixed banks.
 *
 * \param [in] unit Unit number.
 * \param [in] grp_id Group ID.
 * \param [out] assigned Returen true if fixed banks are pre-assigned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_BUSY DEVICE_EM_GROUP was set.
 */
static int
dev_em_grp_fixed_bank_assigned(int unit, uint32_t grp_id, bool *assigned)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    uint32_t idx;
    uint32_t f_bank_cnt = 0;

    SHR_FUNC_ENTER(unit);

    in_flds.count  = 1;
    out_flds.count = UFT_FIELD_COUNT_MAX;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &in_flds));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &out_flds));

    in_flds.field[0]->id = DEVICE_EM_GROUP_INFOt_DEVICE_EM_GROUP_IDf;
    in_flds.field[0]->data = grp_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, DEVICE_EM_GROUP_INFOt, &in_flds, &out_flds));
    for (idx = 0; idx < out_flds.count; idx++) {
        if (out_flds.field[idx]->id ==
            DEVICE_EM_GROUP_INFOt_NUM_FIXED_BANKSf) {
            f_bank_cnt = out_flds.field[idx]->data;
            if (f_bank_cnt > 0) {
                *assigned = TRUE;
                SHR_EXIT();
            }
        }
    }

    *assigned = FALSE;
exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &in_flds);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &out_flds);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether an EM_GROUP's mapped LTs already have entries inserted.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] chg_grp EM_GROUP id to be checked.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_BUSY EM_GROUP is in used.
 */
static int
dev_em_grp_lt_inserted(int unit, const bcmltd_op_arg_t *op_arg,
                       uint32_t chg_grp)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    size_t i;
    uint32_t idx;
    uint32_t fid, fidx;
    uint64_t fval;
    uint32_t num_lookup0_lt = 0, num_lookup1_lt = 0;
    uint32_t lookup0_lt[MAX_MAPPED_LT] = {0};
    uint32_t lookup1_lt[MAX_MAPPED_LT] = {0};
    uint32_t inuse_cnt = 0;

    SHR_FUNC_ENTER(unit);

    in_flds.count = 1;
    out_flds.count = UFT_FIELD_COUNT_MAX;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &in_flds));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &out_flds));

    in_flds.field[0]->id = DEVICE_EM_GROUP_INFOt_DEVICE_EM_GROUP_IDf;
    in_flds.field[0]->data = chg_grp;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_lookup(unit, DEVICE_EM_GROUP_INFOt, &in_flds, &out_flds));

    for (i = 0; i < out_flds.count; i++) {
        fid = out_flds.field[i]->id;
        fval = out_flds.field[i]->data;
        fidx = out_flds.field[i]->idx;
        if (fidx >= TABLE_EM_GROUPS_MAX) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        switch (fid) {
        case DEVICE_EM_GROUP_INFOt_NUM_LOOKUP0_LTf:
            num_lookup0_lt = fval;
            break;
        case DEVICE_EM_GROUP_INFOt_NUM_LOOKUP1_LTf:
            num_lookup1_lt = fval;
            break;
        case DEVICE_EM_GROUP_INFOt_LOOKUP0_LTf:
            lookup0_lt[fidx] = fval;
            break;
        case DEVICE_EM_GROUP_INFOt_LOOKUP1_LTf:
            lookup1_lt[fidx] = fval;
            break;
        default:
            break;
        }
    }

    for (idx = 0; idx < num_lookup0_lt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_inuse_count_get(unit, op_arg->trans_id,
                                          lookup0_lt[idx], &inuse_cnt));
        if (inuse_cnt > 0) {
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    }
    for (idx = 0; idx < num_lookup1_lt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_inuse_count_get(unit, op_arg->trans_id,
                                          lookup1_lt[idx], &inuse_cnt));
        if (inuse_cnt > 0) {
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &in_flds);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &out_flds);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether an EM_GROUP already has entries inserted.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] chg_grp EM_GROUP id to be checked.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_BUSY EM_GROUP is in used.
 */
static int
dev_em_grp_used_check(int unit, const bcmltd_op_arg_t *op_arg,
                      uint32_t chg_grp)
{
    int rv = SHR_E_NONE;
    bool fixed_bank_assigned = FALSE;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (dev_em_grp_fixed_bank_assigned(unit, chg_grp, &fixed_bank_assigned));
    /* Group is not set and without pre-assigned banks, return directly. */
    rv = dev_em_grp_set_check(unit, chg_grp);
    if ((rv != SHR_E_BUSY) && !fixed_bank_assigned) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (dev_em_grp_lt_inserted(unit, op_arg, chg_grp));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether the tile's are set with banks under a certain mode.
 *
 * \param [in] unit Unit number.
 * \param [in] tile_id Tile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_BUSY EM_GROUP is in used.
 */
static int
dev_em_tile_set_check(int unit, uint32_t tile_id)
{
    uint32_t grps[TABLE_EM_GROUPS_MAX] = {0};
    uint32_t r_grp_cnt = 0;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_tile_grp_get(unit, tile_id,
                                 grps, TABLE_EM_GROUPS_MAX, &r_grp_cnt));
    for (idx = 0; idx < r_grp_cnt; idx++) {
        SHR_IF_ERR_EXIT
            (dev_em_grp_set_check(unit, grps[idx]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether a tile is in use.
 *
 * \param [in] unit Unit number.
 * \param [in] tile_id Tile ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_BUSY EM_GROUP is in used.
 */
static int
dev_em_tile_used_check(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       uint32_t tile_id)
{
    uint32_t grps[TABLE_EM_GROUPS_MAX] = {0};
    uint32_t r_grp_cnt = 0;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_tile_grp_get(unit, tile_id,
                                 grps, TABLE_EM_GROUPS_MAX, &r_grp_cnt));
    for (idx = 0; idx < r_grp_cnt; idx++) {
        SHR_IF_ERR_EXIT
            (dev_em_grp_used_check(unit, op_arg, grps[idx]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check whether an EM_BANK is in use.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg LT operation arguments.
 * \param [in] chg_bank Bank id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_BUSY Bank is in use.
 */
static int
dev_em_bank_used_check(int unit, const bcmltd_op_arg_t *op_arg,
                       uint32_t chg_bank)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    uint32_t i, b_idx;
    int rv = SHR_E_NONE;
    uint32_t fid, fidx;
    uint64_t fval;
    uint32_t grp_id = 0;
    uint32_t fix_banks[UFT_VAL_BANKS_MAX], var_banks[UFT_VAL_BANKS_MAX];
    uint32_t f_bank_cnt = 0, v_bank_cnt = 0;
    /* The change bank is member of the specific group. */
    bool is_member = FALSE;

    SHR_FUNC_ENTER(unit);

    in_flds.count  = 1;
    out_flds.count = UFT_FIELD_COUNT_MAX;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &in_flds));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_lt_fields_buff_alloc(unit, &out_flds));

    sal_memset(fix_banks, 0, sizeof(fix_banks));
    sal_memset(var_banks, 0, sizeof(var_banks));
    rv = bcmimm_entry_get_first(unit, DEVICE_EM_GROUPt, &out_flds);
    while (rv == SHR_E_NONE) {
        is_member = FALSE;
        for (i = 0; i < out_flds.count; i++) {
            fid = out_flds.field[i]->id;
            fval = out_flds.field[i]->data;
            fidx = out_flds.field[i]->idx;
            if (fidx >= UFT_VAL_BANKS_MAX) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            switch (fid) {
            case DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf:
                grp_id = fval;
                break;
            case DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf:
                var_banks[fidx] = fval;
                break;
            case DEVICE_EM_GROUPt_NUM_BANKSf:
                v_bank_cnt = fval;
                break;
            default:
                break;
            }
        }

        SHR_IF_ERR_EXIT
            (bcmptm_uft_grp_fixed_bank_get(unit, grp_id,
                                           fix_banks, UFT_VAL_BANKS_MAX,
                                           &f_bank_cnt));

        for (b_idx = 0; b_idx < f_bank_cnt; b_idx++) {
            if (chg_bank == fix_banks[b_idx]) {
                is_member = TRUE;
                break;
            }
        }

        if (!is_member) {
            for (b_idx = 0; b_idx < v_bank_cnt; b_idx++) {
                if (chg_bank == var_banks[b_idx]) {
                    is_member = TRUE;
                    break;
                }
            }
        }

        if (is_member) {
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_used_check(unit, op_arg, grp_id));
        }

        in_flds.count = 1;
        in_flds.field[0]->id = DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf;
        in_flds.field[0]->data = grp_id;
        out_flds.count = UFT_FIELD_COUNT_MAX;
        rv = bcmimm_entry_get_next(unit, DEVICE_EM_GROUPt, &in_flds, &out_flds);
    }

exit:
    (void) bcmptm_uft_lt_fields_buff_free(unit, &in_flds);
    (void) bcmptm_uft_lt_fields_buff_free(unit, &out_flds);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TABLE_EM_CONTROLt input fields parsing.
 *
 * Parse IMM TABLE_EM_CONTROLt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] ctrl Table exact match control data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to parse.
 */
static int
table_em_ctrl_fields_parse(int unit,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           uft_tbl_em_ctrl_t *ctrl)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key. */
    if (key->id != TABLE_EM_CONTROLt_TABLE_IDf) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    ctrl->lt_id = key->data;

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        switch (fid) {
        case TABLE_EM_CONTROLt_MOVE_DEPTHf:
            ctrl->move_depth = fval;
            SHR_BITSET(ctrl->fbmp, TABLE_EM_CONTROLt_MOVE_DEPTHf);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief TABLE_EM_CONTROL logical table validate callback function.
 *
 * Validate the field values of TABLE_EM_CONTROLt logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
table_em_ctrl_imm_validate(int unit,
                           bcmltd_sid_t sid,
                           bcmimm_entry_event_t event,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           void *context)
{
    uft_tbl_em_ctrl_t tbl_ctrl;
    uint32_t num_actual;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);

    sal_memset(&tbl_ctrl, 0, sizeof(uft_tbl_em_ctrl_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (table_em_ctrl_fields_parse(unit, key, data, &tbl_ctrl));

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            /* For insert opeartaion, default value is set if not given. */
            if (!SHR_BITGET(tbl_ctrl.fbmp, TABLE_EM_CONTROLt_MOVE_DEPTHf)) {
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmlrd_field_default_get(
                        unit, TABLE_EM_CONTROLt,
                        TABLE_EM_CONTROLt_MOVE_DEPTHf,
                        1, &def_val, &num_actual));
                tbl_ctrl.move_depth = def_val;
                SHR_BITSET(tbl_ctrl.fbmp, TABLE_EM_CONTROLt_MOVE_DEPTHf);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_uft_tbl_em_ctrl_validate(unit, &tbl_ctrl));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_uft_tbl_em_ctrl_validate(unit, &tbl_ctrl));
            break;
        case BCMIMM_ENTRY_DELETE:
        case BCMIMM_ENTRY_LOOKUP:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief TABLE_EM_CONTROL IMM table change callback function for staging.
 *
 * Handle TABLE_EM_CONTROL IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
table_em_ctrl_imm_stage(int unit,
                        bcmltd_sid_t sid,
                        const bcmltd_op_arg_t *op_arg,
                        bcmimm_entry_event_t event,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        void *context,
                        bcmltd_fields_t *output_fields)
{
    uft_tbl_em_ctrl_t ctrl;
    uint32_t num_actual;
    uint64_t def_val;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ctrl, 0, sizeof(uft_tbl_em_ctrl_t));
    if (output_fields) {
        output_fields->count = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (table_em_ctrl_fields_parse(unit, key, data, &ctrl));
    SHR_IF_ERR_VERBOSE_EXIT(
    bcmlrd_field_default_get(unit, TABLE_EM_CONTROLt,
                             TABLE_EM_CONTROLt_MOVE_DEPTHf,
                             1, &def_val, &num_actual));
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_ERR_EXIT(SHR_E_FAIL);
        case BCMIMM_ENTRY_UPDATE:
            if (SHR_BITGET(ctrl.fbmp, TABLE_EM_CONTROLt_MOVE_DEPTHf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_rm_hash_lt_move_depth_update(unit,
                                                         ctrl.lt_id,
                                                         ctrl.move_depth));
            }
            break;
        case BCMIMM_ENTRY_LOOKUP:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
table_em_ctrl_imm_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context)
{
    SHR_FUNC_ENTER(unit);

    bcmevm_publish_event_notify(unit, BCMPTM_EVENT_UFT_LT_MOVEDEPTH_COMMIT, 0);

    SHR_FUNC_EXIT();
}

/*!
 * \brief TABLE_EM_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to TABLE_EM_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t table_em_ctrl_imm_callback = {

    /*! Validate function. */
    .validate = table_em_ctrl_imm_validate,

    /*! Extended staging function. */
    .op_stage = table_em_ctrl_imm_stage,

    /*! Commit function. */
    .commit   = table_em_ctrl_imm_commit,

    /*! Abort function. */
    .abort    = NULL
};

static bool
dev_em_grp_is_same(int unit,
                   uft_dev_em_grp_t *old_grp,
                   uft_dev_em_grp_t *new_grp)
{
    uint32_t o_idx = 0, n_idx = 0;

    if (old_grp->vector_type != new_grp->vector_type) {
        return FALSE;
    }
    if (old_grp->robust != new_grp->robust) {
        return FALSE;
    }
    if (old_grp->bank_cnt != new_grp->bank_cnt) {
        return FALSE;
    }

    for (o_idx = 0; o_idx < old_grp->bank_cnt; o_idx++) {
        for (n_idx = 0; n_idx < new_grp->bank_cnt; n_idx++) {
            if (old_grp->bank[o_idx] == new_grp->bank[n_idx]) {
                break;
            }
        }
        if (n_idx == new_grp->bank_cnt) {
            return FALSE;
        }
    }

    return TRUE;
}

/*!
 * \brief DEVICE_EM_GROUPt input fields parsing.
 *
 * Parse IMM DEVICE_EM_GROUPt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] grp Device exact match group data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to parse.
 */
static int
dev_em_grp_fields_parse(int unit,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        uft_dev_em_grp_t *grp)
{
    const bcmltd_field_t *gen_field = NULL;
    uint32_t fid, fidx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key. */
    if (key->id != DEVICE_EM_GROUPt_DEVICE_EM_GROUP_IDf) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    grp->grp_id = key->data;

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        fidx = gen_field->idx;
        switch (fid) {
        case DEVICE_EM_GROUPt_ROBUSTf:
            grp->robust = fval;
            SHR_BITSET(grp->fbmp, DEVICE_EM_GROUPt_ROBUSTf);
            break;
        case DEVICE_EM_GROUPt_VECTOR_TYPEf:
            grp->vector_type = fval;
            SHR_BITSET(grp->fbmp, DEVICE_EM_GROUPt_VECTOR_TYPEf);
            break;
        case DEVICE_EM_GROUPt_DEVICE_EM_BANK_IDf:
            if (fidx >= UFT_VAL_BANKS_MAX) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            grp->bank[fidx] = fval;
            SHR_BITSET(grp->bankbmp, fidx);
            break;
        case DEVICE_EM_GROUPt_NUM_BANKSf:
            grp->bank_cnt = fval;
            SHR_BITSET(grp->fbmp, DEVICE_EM_GROUPt_NUM_BANKSf);
            break;
        default:
            break;
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE_EM_GROUP logical table validate callback function.
 *
 * Validate the field values of DEVICE_EM_GROUPt logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
dev_em_grp_imm_validate(int unit,
                        bcmltd_sid_t sid,
                        bcmimm_entry_event_t event,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        void *context)
{
    uft_dev_em_grp_t grp, old_grp;
    uint32_t idx;
    bool is_del = FALSE;
    uint32_t i, j, bank;
    const em_grp_info_t *grp_info = NULL;
    bool tile_mode_en = FALSE;
    uint32_t cur_tile_mode;
    bcmltd_op_arg_t op_arg = {0};

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&grp, 0, sizeof(uft_dev_em_grp_t));
            sal_memset(&old_grp, 0, sizeof(uft_dev_em_grp_t));
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_fields_parse(unit, key, data, &grp));
            old_grp.grp_id = grp.grp_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (uft_lt_grp_get_from_imm(unit, &old_grp));
            if (!SHR_BITGET(grp.fbmp, DEVICE_EM_GROUPt_VECTOR_TYPEf)) {
                grp.vector_type = old_grp.vector_type;
            }
            if (!SHR_BITGET(grp.fbmp, DEVICE_EM_GROUPt_ROBUSTf)) {
                grp.robust = old_grp.robust;
            }
            if (!SHR_BITGET(grp.fbmp, DEVICE_EM_GROUPt_NUM_BANKSf)) {
                grp.bank_cnt = old_grp.bank_cnt;
            }
            for (idx = 0; idx < grp.bank_cnt; idx++) {
                if (!SHR_BITGET(grp.bankbmp, idx)) {
                    grp.bank[idx] = old_grp.bank[idx];
                }
            }
            /* There is no change for group, do nothing. */
            if (dev_em_grp_is_same(unit, &old_grp, &grp)) {
                SHR_EXIT();
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_uft_tile_mode_enabled(unit, &tile_mode_en));
            if (tile_mode_en) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_uft_grp_info_get(unit, grp.grp_id, &grp_info));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_uft_tile_cfg_get(unit, grp_info->tile,
                                             &cur_tile_mode, NULL, NULL));
                if (grp_info->tile_mode != cur_tile_mode) {
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }

                if (grp_info->attri != EM_GROUP_ATTRI_EM) {
                    /*
                     * All groups except hash group can't be changed
                     * when related LTs are in use.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (dev_em_grp_lt_inserted(unit, &op_arg, grp.grp_id));
                }
            }

            /*
             * A bank can't be removed from group when this bank is in use.
             * Calculate all old bank to check whether it can be removed.
             */
            for (i = 0; i < old_grp.bank_cnt; i++) {
                bank = old_grp.bank[i];
                is_del = TRUE;
                for (j = 0; j < grp.bank_cnt; j++) {
                    if (bank == grp.bank[j]) {
                        is_del = FALSE;
                        break;
                    }
                }
                if (is_del) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (dev_em_bank_used_check(unit, &op_arg, bank));
                }
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_uft_dev_em_grp_validate(unit, &grp));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE_EM_GROUP IMM table change callback function for staging.
 *
 * Handle DEVICE_EM_GROUP IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
dev_em_grp_imm_stage(int unit,
                     bcmltd_sid_t sid,
                     const bcmltd_op_arg_t *op_arg,
                     bcmimm_entry_event_t event,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     void *context,
                     bcmltd_fields_t *output_fields)
{
    uft_dev_em_grp_t grp, old_grp;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&grp, 0, sizeof(uft_dev_em_grp_t));
            sal_memset(&old_grp, 0, sizeof(uft_dev_em_grp_t));
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_grp_fields_parse(unit, key, data, &grp));
            old_grp.grp_id = grp.grp_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (uft_lt_grp_get_from_imm(unit, &old_grp));
            if (!SHR_BITGET(grp.fbmp, DEVICE_EM_GROUPt_VECTOR_TYPEf)) {
                grp.vector_type = old_grp.vector_type;
            }
            if (!SHR_BITGET(grp.fbmp, DEVICE_EM_GROUPt_ROBUSTf)) {
                grp.robust = old_grp.robust;
            }
            if (!SHR_BITGET(grp.fbmp, DEVICE_EM_GROUPt_NUM_BANKSf)) {
                grp.bank_cnt = old_grp.bank_cnt;
            }
            for (idx = 0; idx < grp.bank_cnt; idx++) {
                if (!SHR_BITGET(grp.bankbmp, idx)) {
                    grp.bank[idx] = old_grp.bank[idx];
                }
            }
            /* There is no change for group, do nothing. */
            if (dev_em_grp_is_same(unit, &old_grp, &grp)) {
                SHR_EXIT();
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_uft_dev_em_grp_set(unit, sid, op_arg, &grp));
            break;
         case BCMIMM_ENTRY_LOOKUP:
            SHR_EXIT();
         default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
dev_em_grp_imm_commit(int unit, bcmltd_sid_t sid,
                      uint32_t trans_id, void *context)
{
    uint64_t ev_data;

    SHR_FUNC_ENTER(unit);

    ev_data = BCMPTM_UFT_EV_INFO_CONSTRUCT(sid, trans_id);
    bcmevm_publish_event_notify(unit, BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                ev_data);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_uft_dev_em_grp_commit(unit));

exit:
    SHR_FUNC_EXIT();
}

static void
dev_em_grp_imm_abort(int unit, bcmltd_sid_t sid,
                     uint32_t trans_id, void *context)
{
    (void) bcmptm_uft_dev_em_grp_abort(unit);
}

/*!
 * \brief DEVICE_EM_GROUP In-memory event callback structure.
 *
 * This structure contains callback functions that conrrespond
 * to DEVICE_EM_GROUP logical table entry commit stages.
 */
static bcmimm_lt_cb_t dev_em_grp_imm_callback = {

    /*! Validate function. */
    .validate = dev_em_grp_imm_validate,

    /*! Extended staging function. */
    .op_stage = dev_em_grp_imm_stage,

    /*! Commit function. */
    .commit   = dev_em_grp_imm_commit,

    /*! Abort function. */
    .abort    = dev_em_grp_imm_abort
};

/*!
 * \brief DEVICE_EM_BANKt input fields parsing.
 *
 * Parse IMM DEVICE_EM_BANKt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] grp Device exact match group data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to parse.
 */
static int
dev_em_bank_fields_parse(int unit,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         uft_dev_em_bank_t *bank)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key. */
    if (key->id != DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Do the transform from DEVICE_EM_BANKt_DEVICE_EM_BANK_IDf to
     * DEVICE_EM_BANK_INFOt_DEVICE_EM_BANK_IDf under the (also correct)
     * assumption that the enum symbols (but not necessarily values) are
     * the same across the two types.
     */
    SHR_IF_ERR_EXIT
        (bcmptm_uft_bank_hash_to_global(unit, key->data, &(bank->bank_id)));

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        switch (fid) {
            case DEVICE_EM_BANKt_OFFSETf:
                bank->offset = fval;
                SHR_BITSET(bank->fbmp, DEVICE_EM_BANKt_OFFSETf);
                break;
            default:
                break;
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE_EM_BANK logical table validate callback function.
 *
 * Validate the field values of DEVICE_EM_BANKt logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
dev_em_bank_imm_validate(int unit,
                         bcmltd_sid_t sid,
                         bcmimm_entry_event_t event,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         void *context)
{
    uft_dev_em_bank_t bank;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&bank, 0, sizeof(uft_dev_em_bank_t));
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_bank_fields_parse(unit, key, data, &bank));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_uft_dev_em_bank_validate(unit, &bank));
            break;
        case BCMIMM_ENTRY_LOOKUP:
            break;
         default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE_EM_BANK IMM table change callback function for staging.
 *
 * Handle DEVICE_EM_BANK IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
dev_em_bank_imm_stage(int unit,
                      bcmltd_sid_t sid,
                      const bcmltd_op_arg_t *op_arg,
                      bcmimm_entry_event_t event,
                      const bcmltd_field_t *key,
                      const bcmltd_field_t *data,
                      void *context,
                      bcmltd_fields_t *output_fields)
{
    uft_dev_em_bank_t bank;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&bank, 0, sizeof(uft_dev_em_bank_t));
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_bank_fields_parse(unit, key, data, &bank));
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_bank_used_check(unit, op_arg, bank.bank_id));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_uft_dev_em_bank_set(unit, sid, op_arg, &bank));
            break;
        case BCMIMM_ENTRY_LOOKUP:
            SHR_EXIT();
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
dev_em_bank_imm_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context)
{
    SHR_FUNC_ENTER(unit);

    bcmevm_publish_event_notify(unit, BCMPTM_EVENT_UFT_BANK_CHANGE_COMMIT, 0);

    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE_EM_BANK in-memory event callback structure.
 *
 * This structure contains callback functions that conrrespond
 * to DEVICE_EM_BANK logical table entry commit stages.
 */
static bcmimm_lt_cb_t dev_em_bank_imm_callback = {

    /*! Validate function. */
    .validate = dev_em_bank_imm_validate,

    /*! Extended staging function. */
    .op_stage = dev_em_bank_imm_stage,

    /*! Commit function. */
    .commit   = dev_em_bank_imm_commit,

    /*! Abort function. */
    .abort    = NULL
};

/*!
 * \brief DEVICE_EM_BANK_PAIR logical table validate callback function.
 *
 * Validate the field values of DEVICE_EM_BANKt logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
dev_em_bank_pair_imm_validate(int unit,
                              bcmltd_sid_t sid,
                              bcmimm_entry_event_t event,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              void *context)
{
    uint32_t global_bank_id = 0;
    bcmltd_op_arg_t op_arg = {0};

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_uft_bank_pair_to_global(unit,
                                                key->data,
                                                &global_bank_id));
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_bank_used_check(unit, &op_arg, global_bank_id));
            break;
        case BCMIMM_ENTRY_LOOKUP:
            break;
         default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
dev_em_bank_pair_imm_commit(int unit,
                            bcmltd_sid_t sid,
                            uint32_t trans_id,
                            void *context)
{
    uint64_t ev_data;

    SHR_FUNC_ENTER(unit);

    ev_data = BCMPTM_UFT_EV_INFO_CONSTRUCT(sid, trans_id);
    /* Pair configuration affects banks of EM groups. */
    bcmevm_publish_event_notify(unit, BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                ev_data);

    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE_EM_BANK_PAIR in-memory event callback structure.
 *
 * This structure contains callback functions that conrrespond
 * to DEVICE_EM_BANK logical table entry commit stages.
 */
static bcmimm_lt_cb_t dev_em_bank_pair_imm_callback = {

    /*! Validate function. */
    .validate = dev_em_bank_pair_imm_validate,

    /*! Extended staging function. */
    .op_stage = NULL,

    /*! Commit function. */
    .commit   = dev_em_bank_pair_imm_commit,

    /*! Abort function. */
    .abort    = NULL
};


/*!
 * \brief DEVICE_EM_TILEt input fields parsing.
 *
 * Parse IMM DEVICE_EM_TILEt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] tile Tile info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to parse.
 */
static int
dev_em_tile_fields_parse(int unit,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         uft_dev_em_tile_t *tile)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key. */
    if (key->id != DEVICE_EM_TILEt_DEVICE_EM_TILE_IDf) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    tile->tile_id = key->data;

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        switch (fid) {
            case DEVICE_EM_TILEt_ROBUSTf:
                tile->robust = fval;
                SHR_BITSET(tile->fbmp, DEVICE_EM_TILEt_ROBUSTf);
                break;
            case DEVICE_EM_TILEt_SEEDf:
                tile->seed = fval;
                SHR_BITSET(tile->fbmp, DEVICE_EM_TILEt_SEEDf);
                break;
            case DEVICE_EM_TILEt_MODEf:
                tile->mode = fval;
                SHR_BITSET(tile->fbmp, DEVICE_EM_TILEt_MODEf);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE_EM_TILE logical table validate callback function.
 *
 * Reject change when this tile is being used.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] event Entry event.
 * \param [in] key Not used.
 * \param [in] data Not used.
 * \param [in] context Not used.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
dev_em_tile_imm_validate(int unit,
                         bcmltd_sid_t sid,
                         bcmimm_entry_event_t event,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         void *context)
{
    uft_dev_em_tile_t tile;
    uint32_t cur_mode = 0, disabled_mode = 0;
    bool spm_blocked = FALSE;

    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&tile, 0, sizeof(uft_dev_em_tile_t));
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_tile_fields_parse(unit, key, data, &tile));

            if (SHR_BITGET(tile.fbmp, DEVICE_EM_TILEt_MODEf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_uft_tile_cfg_get(unit,
                                             tile.tile_id,
                                             &cur_mode, NULL, NULL));
                if (tile.mode == cur_mode) {
                    SHR_EXIT();
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_spm_tile_mode_change_blocked(unit,
                                                         tile.tile_id,
                                                         cur_mode,
                                                         tile.mode,
                                                         &spm_blocked));
                if (spm_blocked) {
                    SHR_ERR_EXIT(SHR_E_BUSY);
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_field_symbol_to_value(unit,
                                                  DEVICE_EM_TILEt,
                                                  DEVICE_EM_TILEt_MODEf,
                                                  "TILE_MODE_DISABLED",
                                                  &disabled_mode));
                if (tile.mode != disabled_mode) {
                    /* Only do the mode validation for non-TILE_MODE_DISABLED. */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmptm_uft_tile_mode_validate(unit,
                                                       tile.tile_id,
                                                       tile.mode));
                }
            }
            break;
        case BCMIMM_ENTRY_LOOKUP:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DEVICE_EM_TILE IMM table change callback function for staging.
 *
 * Handle DEVICE_EM_TILE IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
dev_em_tile_imm_stage(int unit,
                      bcmltd_sid_t sid,
                      const bcmltd_op_arg_t *op_arg,
                      bcmimm_entry_event_t event,
                      const bcmltd_field_t *key,
                      const bcmltd_field_t *data,
                      void *context,
                      bcmltd_fields_t *output_fields)
{
    uft_dev_em_tile_t tile;
    bool chg = FALSE;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&tile, 0, sizeof(uft_dev_em_tile_t));
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_tile_fields_parse(unit, key, data, &tile));
            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_tile_chg_check(unit, &tile, &chg));
            if (!chg) {
                SHR_EXIT();
            }
            if (SHR_BITGET(tile.fbmp, DEVICE_EM_TILEt_MODEf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (dev_em_tile_set_check(unit, tile.tile_id));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (dev_em_tile_used_check(unit, op_arg, tile.tile_id));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_uft_dev_em_tile_set(unit, sid, op_arg, &tile));
            break;
        case BCMIMM_ENTRY_LOOKUP:
            SHR_EXIT();
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
dev_em_tile_imm_commit(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       void *context)
{
    uint64_t ev_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_sbr_tile_mode_commit(unit));

    ev_data = BCMPTM_UFT_EV_INFO_CONSTRUCT(sid, trans_id);
    bcmevm_publish_event_notify(unit,
                                BCMPTM_EVENT_UFT_TILE_CHANGE_COMMIT,
                                ev_data);

exit:
    SHR_FUNC_EXIT();
}

static void
dev_em_tile_imm_abort(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context)
{
    bcmptm_sbr_tile_mode_abort(unit);
}

/*!
 * \brief DEVICE_EM_TILE in-memory event callback structure.
 *
 * This structure contains callback functions that conrrespond
 * to DEVICE_EM_TILE logical table entry commit stages.
 */
static bcmimm_lt_cb_t dev_em_tile_imm_callback = {

    /*! Validate function. */
    .validate = dev_em_tile_imm_validate,

    /*! Extended staging function. */
    .op_stage = dev_em_tile_imm_stage,

    /*! Commit function. */
    .commit   = dev_em_tile_imm_commit,

    /*! Abort function. */
    .abort    = dev_em_tile_imm_abort
};

static void
dev_em_tile_event_handler(int unit,
                          const char *event,
                          uint64_t ev_data)
{
    (void)bcmptm_rm_hash_robust_hash_update(unit,
                                     BCMPTM_UFT_EV_INFO_LTID_GET(ev_data),
                                     BCMPTM_UFT_EV_INFO_TRANS_ID_GET(ev_data),
                                            bcmptm_uft_lt_hw_write);
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_uft_imm_register_init(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for TABLE_EM_CONTROL LT here.
     */
    rv = bcmlrd_map_get(unit, TABLE_EM_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 TABLE_EM_CONTROLt,
                                 &table_em_ctrl_imm_callback,
                                 NULL));
    }

    /*
     * To register callback for DEVICE_EM_GROUP LT here.
     */
    rv = bcmlrd_map_get(unit, DEVICE_EM_GROUPt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 DEVICE_EM_GROUPt,
                                 &dev_em_grp_imm_callback,
                                 NULL));
    }

    /*
     * To register callback for DEVICE_EM_BANK LT here.
     */
    rv = bcmlrd_map_get(unit, DEVICE_EM_BANKt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 DEVICE_EM_BANKt,
                                 &dev_em_bank_imm_callback,
                                 NULL));
    }

    /*
     * To register callback for DEVICE_EM_BANK_PAIR LT here.
     */
    rv = bcmlrd_map_get(unit, DEVICE_EM_BANK_PAIRt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 DEVICE_EM_BANK_PAIRt,
                                 &dev_em_bank_pair_imm_callback,
                                 NULL));
    }


    /*
     * To register callback for DEVICE_EM_TILE LT here.
     */
    rv = bcmlrd_map_get(unit, DEVICE_EM_TILEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 DEVICE_EM_TILEt,
                                 &dev_em_tile_imm_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_em_tile_event_hdl_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmevm_register_published_event(unit,
                                        BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                        dev_em_tile_event_handler));
    SHR_IF_ERR_EXIT(
        bcmevm_register_published_event(unit,
                                        BCMPTM_EVENT_UFT_TILE_CHANGE_COMMIT,
                                        dev_em_tile_event_handler));

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_uft_em_tile_event_hdl_stop(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmevm_unregister_published_event(unit,
                                          BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                          dev_em_tile_event_handler));

    SHR_IF_ERR_EXIT(
        bcmevm_unregister_published_event(unit,
                                          BCMPTM_EVENT_UFT_TILE_CHANGE_COMMIT,
                                          dev_em_tile_event_handler));

exit:
    SHR_FUNC_EXIT();
}
