/*! \file bcmcth_ctr_eflex_init.c
 *
 * BCMCTH Enhanced Flex Counter init and cleanup routines
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmbd/bcmbd_ts.h>
#include <bcmptm/bcmptm_cci.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*! Enhanced flex counter control structure. */
static ctr_eflex_control_t *bcmcth_ctr_eflex_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*! Enhanced flex counter ingress pipe mode. */
uint8_t ctr_eflex_ing_pipe_opermode[BCMDRD_CONFIG_MAX_UNITS];

/*! Enhanced flex counter egress pipe mode. */
uint8_t ctr_eflex_egr_pipe_opermode[BCMDRD_CONFIG_MAX_UNITS];

/*! Enhanced flex state control structure. */
static ctr_eflex_control_t *bcmcth_state_eflex_ctrl[BCMDRD_CONFIG_MAX_UNITS];

static bcmlrd_sid_t ctr_lt_sid[] = {/* CTR_ING_EFLEX_ACTION_PROFILEt */
                                    0,
                                    /* CTR_EGR_EFLEX_ACTION_PROFILEt */
                                    0,
                                    /* CTR_ING_EFLEX_OBJ_QUANTIZATIONt */
                                    0,
                                    /* CTR_EGR_EFLEX_OBJ_QUANTIZATIONt */
                                    0,
                                    CTR_ING_EFLEX_STATSt,
                                    CTR_EGR_EFLEX_STATSt,
                                    CTR_ING_EFLEX_OPERAND_PROFILEt,
                                    CTR_EGR_EFLEX_OPERAND_PROFILEt,
                                    CTR_ING_EFLEX_RANGE_CHK_PROFILEt,
                                    CTR_EGR_EFLEX_RANGE_CHK_PROFILEt,
                                    CTR_ING_EFLEX_TRIGGERt,
                                    CTR_EGR_EFLEX_TRIGGERt,
                                    CTR_ING_EFLEX_GROUP_ACTION_PROFILEt,
                                    CTR_EGR_EFLEX_GROUP_ACTION_PROFILEt,
                                    CTR_ING_EFLEX_ERROR_STATSt,
                                    CTR_EGR_EFLEX_ERROR_STATSt,
                                    CTR_ING_EFLEX_HITBIT_CONTROLt,
                                    CTR_EGR_EFLEX_HITBIT_CONTROLt};

static bcmlrd_sid_t state_lt_sid[] = {/* FLEX_STATE_ING_ACTION_PROFILEt */
                                      0,
                                      /* FLEX_STATE_EGR_ACTION_PROFILEt */
                                      0,
                                      /* FLEX_STATE_ING_OBJ_QUANTIZATIONt */
                                      0,
                                      /* FLEX_STATE_EGR_OBJ_QUANTIZATIONt */
                                      0,
                                      FLEX_STATE_ING_STATSt,
                                      FLEX_STATE_EGR_STATSt,
                                      FLEX_STATE_ING_OPERAND_PROFILEt,
                                      FLEX_STATE_EGR_OPERAND_PROFILEt,
                                      FLEX_STATE_ING_RANGE_CHK_PROFILEt,
                                      FLEX_STATE_EGR_RANGE_CHK_PROFILEt,
                                      FLEX_STATE_ING_TRIGGERt,
                                      FLEX_STATE_EGR_TRIGGERt,
                                      FLEX_STATE_ING_GROUP_ACTION_PROFILEt,
                                      FLEX_STATE_EGR_GROUP_ACTION_PROFILEt,
                                      FLEX_STATE_ING_ERROR_STATSt,
                                      FLEX_STATE_EGR_ERROR_STATSt,
                                      FLEX_STATE_ING_TRUTH_TABLEt,
                                      FLEX_STATE_EGR_TRUTH_TABLEt};

/* Get enhanced flex component control structure */
int
bcmcth_eflex_ctrl_get(int unit,
                      eflex_comp_t comp,
                      ctr_eflex_control_t **ctrl)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    switch (comp) {
        case CTR_EFLEX:
            *ctrl = bcmcth_ctr_eflex_ctrl[unit];
            break;
        case STATE_EFLEX:
            *ctrl = bcmcth_state_eflex_ctrl[unit];
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (*ctrl == NULL) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                         "Feature %d not supported\n"), comp));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Get flex component LT sid list */
static void
bcmcth_eflex_lt_sid_get(eflex_comp_t comp,
                        bcmlrd_sid_t **sid_list,
                        uint32_t *num)
{
    if (comp == CTR_EFLEX) {
        *sid_list = ctr_lt_sid;
        *num = COUNTOF(ctr_lt_sid);
    } else if (comp == STATE_EFLEX) {
        *sid_list = state_lt_sid;
        *num = COUNTOF(state_lt_sid);
    }

    return;
}

/*! Initialize field info for all STATE/CTR EFLEX IMM LT */
static int
bcmcth_ctr_eflex_lt_field_info_init(int unit, eflex_comp_t comp)
{
    size_t i, num_fields = 0, count;
    bcmlrd_fid_t   *fid_list = NULL;
    ctr_eflex_control_t *ctrl = NULL;
    bcmlrd_field_def_t  field_info;
    bcmlrd_sid_t *lt_sid = NULL;
    uint32_t num = 0, t;
    const bcmlrd_map_t *ctr_eflex_map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Get EFLEX LT list */
    bcmcth_eflex_lt_sid_get(comp, &lt_sid, &num);

    /* Check number of LTs whose field info is saved */
    if (comp == CTR_EFLEX) {
        assert(CTR_EFLEX_LT_INFO_NUM == num);
    } else if (comp == STATE_EFLEX) {
        assert(FLEX_STATE_LT_INFO_NUM == num);
    }

    /* Populate LT sids */
    lt_sid[0] = ctrl->ing_action_lt_info.lt_sid;
    lt_sid[1] = ctrl->egr_action_lt_info.lt_sid;
    lt_sid[2] = ctrl->ing_obj_quant_lt_info.lt_sid;
    lt_sid[3] = ctrl->egr_obj_quant_lt_info.lt_sid;

    /* Save number of LTs whose field info is saved */
    ctrl->lt_info_num = num;

    for (t = 0; t < ctrl->lt_info_num; t++) {
        /* Check if LT is supported for given device */
        rv = bcmlrd_map_get(unit, lt_sid[t], &ctr_eflex_map);

        if (SHR_SUCCESS(rv) && ctr_eflex_map) {
            /* Get number of fields. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlrd_field_count_get(unit, lt_sid[t], &num_fields));

            SHR_ALLOC(fid_list, sizeof(bcmlrd_fid_t) * num_fields,
                "bcmcthCtrEFlexFields");
            SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
            sal_memset(fid_list, 0, sizeof(bcmlrd_fid_t) * num_fields);

            /* Get list of fields */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlrd_field_list_get(unit,
                                       lt_sid[t],
                                       num_fields,
                                       fid_list,
                                       &count));

            ctrl->ctr_eflex_field_info[t].lt_sid = lt_sid[t];
            ctrl->ctr_eflex_field_info[t].num_key_fields = 0;
            ctrl->ctr_eflex_field_info[t].num_data_fields = 0;
            ctrl->ctr_eflex_field_info[t].num_read_only_fields = 0;

            for (i = 0; i < count; i++) {
                sal_memset(&field_info, 0, sizeof(field_info));

                /* Get field info */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlrd_table_field_def_get(unit,
                                                lt_sid[t],
                                                fid_list[i],
                                                &field_info));

                if (field_info.key) {
                    /* Found one key entry */
                    ctrl->ctr_eflex_field_info[t].num_key_fields++;
                } else {
                    /* Found one data entry */
                    ctrl->ctr_eflex_field_info[t].num_data_fields++;
                    if (field_info.access == BCMLT_FIELD_ACCESS_READ_ONLY) {
                        ctrl->ctr_eflex_field_info[t].num_read_only_fields++;
                    }
                }
            }  /* END inner FOR */

            SHR_FREE(fid_list);
        }
    } /* END outer FOR */

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

 /* Initialize the PT and LT counter field information */
static int
bcmcth_ctr_eflex_field_init(int unit, bool ingress, eflex_comp_t comp)
{
    size_t i, field_num, num;
    bcmdrd_sym_field_info_t finfo = {0};
    bcmdrd_fid_t *fid_list = NULL;
    bcmdrd_sid_t sid;
    uint16_t lt_shift = 0, lt_max = 0, total_bits = 0;
    ctr_eflex_field_t *lt_field = NULL;
    uint32_t *ctr_field_num, *entry_wsize;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    if (ingress) {
        ctr_field_num = &ctrl->ing_ctr_field_num;
        entry_wsize   = &ctrl->ing_entry_wsize;
    } else {
        ctr_field_num = &ctrl->egr_ctr_field_num;
        entry_wsize   = &ctrl->egr_entry_wsize;
    }

    *ctr_field_num = 0;
    *entry_wsize = 0;

    /* Get counter table sid */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_pool_sid_get(unit,
                                   ingress,
                                   comp,
                                   0,
                                   0,
                                   0,
                                   &sid));


    /* Get number of fields */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_fid_list_get(unit, sid, 0, NULL, &field_num));
    SHR_ALLOC(fid_list,
              field_num * sizeof(bcmdrd_fid_t),
              "bcmcthCtreflexFieldId");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);

    /* Get the field list and identify counter fields only */
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_fid_list_get(unit, sid, field_num, fid_list, &field_num));

    for (num = 0, i = 0; i < field_num; i++) {
        if (SHR_FAILURE(
                bcmdrd_pt_field_info_get(unit, sid, fid_list[i], &finfo))) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                 "Error Field Info SID =%d Name = %s\n"),
                                 sid,
                                 bcmdrd_pt_sid_to_name(unit, sid)));
        } else {
            /* Check if this is a counter field */
            if (bcmdrd_pt_field_is_counter(unit, sid, fid_list[i])) {
                lt_field = ingress ? &(ctrl->ing_lt_field[num]) :
                                     &(ctrl->egr_lt_field[num]);

                /*
                 * lt_shift determines the shift in counter lt
                 * bits due to shift
                 * Reset the shift if pt min bit >= lt_max
                 */
                if (finfo.minbit >= lt_max) {
                    lt_shift = 0;
                }

                lt_field->minbit = lt_shift + finfo.minbit;
                /* support 64 bit fields */
                lt_field->maxbit = lt_field->minbit + 63;
                lt_max = lt_field->maxbit;
                /* Increment the shift */
                lt_shift += 64 - (finfo.maxbit - finfo.minbit + 1);

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "SID =%d Name = %s, \
                             lt-minbit =%u, lt-maxbit = %u\n"),
                             sid,
                             bcmdrd_pt_sid_to_name(unit, sid),
                             lt_field->minbit,
                             lt_field->maxbit));

                (*ctr_field_num)++;
                num++;

                /* Each counter field is 64 bit wide */
                total_bits += 64;
            } else {
                /* Non counter fields will be same width as HW */
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                 "Field Info SID =%d Field Name = %s, \
                                  field bits = %d\n"),
                                 sid, finfo.name,
                                 (finfo.maxbit -finfo.minbit + 1)));
            }
        }
    }

    /* store entry size in words */
    *entry_wsize = (total_bits % 32) ? (total_bits / 32) :
                                       ((total_bits / 32) + 1);

exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*! Function to init resource list */
static int
bcmcth_ctr_eflex_resource_list_init(int unit,
                                    uint32_t max_id_num,
                                    ctr_eflex_pool_list_t *list)
{
    uint32_t num_bytes;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(list, SHR_E_PARAM);

    /* Allocate resource list */
    num_bytes = SHR_BITALLOCSIZE(max_id_num);

    /* Allocate inuse bitmap */
    list->inuse_bitmap = sal_alloc(num_bytes,
                                   "bcmcthCtrEflexListBitmap");
    SHR_NULL_CHECK(list->inuse_bitmap, SHR_E_MEMORY);

    sal_memset(list->inuse_bitmap, 0, num_bytes);

    list->max_track = max_id_num;

exit:
    SHR_FUNC_EXIT();
}

/*! Function to free resource list */
static int
bcmcth_ctr_eflex_resource_list_uninit(int unit,
                                      ctr_eflex_pool_list_t *list)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(list, SHR_E_PARAM);

    /* Free resource list */
    if (list->inuse_bitmap) {
        sal_free(list->inuse_bitmap);
    }

    list->inuse_bitmap = NULL;
    list->max_track = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover state during warmboot
 *
 * \param [in] unit    Unit number.
 * \param [in] comp    Enhanced flex component.
 *
 * \retval SHR_E_NONE Success.
 */
static int
bcmcth_ctr_eflex_warm_recover(int unit, eflex_comp_t comp)
{
    const bcmlrd_map_t *ctr_eflex_map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Recover STATE/CTR_ING_EFLEX_ACTION_PROFILE state */
    SHR_IF_ERR_EXIT
        (bcmcth_ctr_eflex_action_profile_state_recover(unit, INGRESS, comp));

    /* Recover STATE/CTR_EGR_EFLEX_ACTION_PROFILE state */
    SHR_IF_ERR_EXIT
        (bcmcth_ctr_eflex_action_profile_state_recover(unit, EGRESS, comp));

    /* Recover STATE/CTR_ING_EFLEX_OPERAND_PROFILE state */
    SHR_IF_ERR_EXIT
        (bcmcth_ctr_eflex_op_profile_state_recover(unit, INGRESS, comp));

    /* Recover STATE/CTR_EGR_EFLEX_OPERAND_PROFILE state */
    SHR_IF_ERR_EXIT
        (bcmcth_ctr_eflex_op_profile_state_recover(unit, EGRESS, comp));

    /* Recover STATE/CTR_ING_EFLEX_RANGE_CHK_PROFILE state */
    SHR_IF_ERR_EXIT
        (bcmcth_ctr_eflex_range_chk_state_recover(unit, INGRESS, comp));

    /* Recover STATE/CTR_EGR_EFLEX_RANGE_CHK_PROFILE state */
    SHR_IF_ERR_EXIT
        (bcmcth_ctr_eflex_range_chk_state_recover(unit, EGRESS, comp));

    if (comp == CTR_EFLEX) {
        /* Check if LT is supported for given device */
        rv = bcmlrd_map_get(unit,
                            CTR_ING_EFLEX_HITBIT_CONTROLt,
                            &ctr_eflex_map);

        if (SHR_SUCCESS(rv) && ctr_eflex_map) {
            /* Recover CTR_ING_EFLEX_HITBIT_CONTROL state */
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_hitbit_ctrl_state_recover(unit,
                                                            INGRESS,
                                                            comp));

            /* Recover CTR_EGR_EFLEX_HITBIT_CONTROL state */
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_hitbit_ctrl_state_recover(unit,
                                                            EGRESS,
                                                            comp));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to get enum values used in EFLEX LTs */
static int
bcmcth_ctr_eflex_lt_enum_get(int unit, eflex_comp_t comp)
{
    bcmlrd_sid_t sid = 0;
    bcmlrd_fid_t fid = 0;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    if (comp == CTR_EFLEX) {
        ctrl = bcmcth_ctr_eflex_ctrl[unit];
        sid = CTR_ING_EFLEX_TRIGGERt;
        fid = CTR_ING_EFLEX_TRIGGERt_OPERATIONAL_STATEf;
    } else if (comp == STATE_EFLEX) {
        ctrl = bcmcth_state_eflex_ctrl[unit];
        sid = FLEX_STATE_ING_TRIGGERt;
        fid = FLEX_STATE_ING_TRIGGERt_OPERATIONAL_STATEf;
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(
            unit,
            sid,
            fid,
            "ACTIVE",
            &ctrl->ctr_eflex_enum.oper_state_active));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(
            unit,
            sid,
            fid,
            "INACTIVE",
            &ctrl->ctr_eflex_enum.oper_state_inactive));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_symbol_to_value(
            unit,
            TABLE_INFOt,
            TABLE_INFOt_TABLE_IDf,
            "INVALID_LT",
            &ctrl->ctr_eflex_enum.invalid_lt_id));
exit:
    SHR_FUNC_EXIT();
}

/*! Initialize enhanced flex counter/state internal structures */
static int
bcmcth_eflex_db_init_common(int unit, bool warm, eflex_comp_t comp)
{
    uint32_t pool_idx, num_ingress_pools, num_egress_pools;
    uint32_t pipe_idx, num_pipes , num_egress_instance;
    uint32_t num_ing_action_id, num_egr_action_id;
    uint32_t num_op_profile_id, num_range_checker;
    bcmdrd_sid_t sid = 0;
    bcmdrd_sym_info_t sinfo = {0};
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info = NULL;
    bool ts_en = false;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    if (device_info->num_ingress_pools > MAX_ING_POOLS_NUM ||
        device_info->num_egress_pools > MAX_EGR_POOLS_NUM ||
        device_info->num_pipes > MAX_PIPES_NUM ||
        device_info->num_objs > MAX_OBJECTS) {

        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_ingress_pools = device_info->num_ingress_pools;
    num_egress_pools = device_info->num_egress_pools;
    num_pipes = device_info->num_pipes;
    num_ing_action_id = device_info->num_ingress_action_id;
    num_egr_action_id = device_info->num_egress_action_id;
    num_op_profile_id = device_info->num_op_profile_id;
    num_range_checker = device_info->num_range_checker;
    num_egress_instance = device_info->state_ext.num_egress_instance;

    /* Get max num counters per ingress pool */
    for (pool_idx = 0; pool_idx < num_ingress_pools; pool_idx++) {
        SHR_IF_ERR_EXIT
            (bcmcth_eflex_pool_sid_get(unit,
                                       INGRESS,
                                       comp,
                                       0,
                                       ING_POST_LKUP_INST,
                                       pool_idx,
                                       &sid));

        SHR_IF_ERR_EXIT(bcmdrd_pt_info_get(unit, sid, &sinfo));
        ctrl->ing_pool_ctr_num[pool_idx] = sinfo.index_max + 1;
    }

    /* Get max num counters per egress pool */
    for (pool_idx = 0; pool_idx < num_egress_pools; pool_idx++) {
        SHR_IF_ERR_EXIT
            (bcmcth_eflex_pool_sid_get(unit,
                                       EGRESS,
                                       comp,
                                       0,
                                       0,
                                       pool_idx,
                                       &sid));


        SHR_IF_ERR_EXIT(bcmdrd_pt_info_get(unit, sid, &sinfo));
        ctrl->egr_pool_ctr_num[pool_idx] = sinfo.index_max + 1;
    }

    if (num_egress_instance == 2) {
       /* Get max num counters per egress pool */
       for (pool_idx = 0; pool_idx < num_egress_pools; pool_idx++) {
            SHR_IF_ERR_EXIT
                (bcmcth_eflex_pool_sid_get(unit,
                                           EGRESS,
                                           comp,
                                           0,
                                           EGR_POST_FWD_INST,
                                           pool_idx,
                                           &sid));


            SHR_IF_ERR_EXIT(bcmdrd_pt_info_get(unit, sid, &sinfo));
            ctrl->state_ext.egr_pool_ctr_num_b[pool_idx] =
                                                sinfo.index_max + 1;
       }
    }

    /* Initialize PT and LT fields */
    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_field_init(unit, INGRESS, comp));
    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_field_init(unit, EGRESS, comp));

    /* Initialize LT field info */
    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_lt_field_info_init(unit, comp));

    /* Initialize internal resource lists */
    for (pipe_idx = 0; pipe_idx < num_pipes; pipe_idx++) {
        /* Init ingress action id list */
        SHR_IF_ERR_EXIT
            (bcmcth_ctr_eflex_resource_list_init(
                unit,
                num_ing_action_id,
                &ctrl->ing_action_id_list[pipe_idx]));

        /* Init egress action id list */
        SHR_IF_ERR_EXIT
            (bcmcth_ctr_eflex_resource_list_init(
                unit,
                num_egr_action_id,
                &ctrl->egr_action_id_list[pipe_idx]));

        /* Init ingress operand profile id list */
        SHR_IF_ERR_EXIT
            (bcmcth_ctr_eflex_resource_list_init(
                unit,
                num_op_profile_id,
                &ctrl->ing_op_id_list[pipe_idx]));

        /* Init egress operand profile id list */
        SHR_IF_ERR_EXIT
            (bcmcth_ctr_eflex_resource_list_init(
                unit,
                num_op_profile_id,
                &ctrl->egr_op_id_list[pipe_idx]));

        /* Init ingress range checker id list */
        SHR_IF_ERR_EXIT
            (bcmcth_ctr_eflex_resource_list_init(
                unit,
                num_range_checker,
                &ctrl->ing_range_chk_id_list[pipe_idx]));

        /* Init egress range checker id list */
        SHR_IF_ERR_EXIT
            (bcmcth_ctr_eflex_resource_list_init(
                unit,
                num_range_checker,
                &ctrl->egr_range_chk_id_list[pipe_idx]));

        /* Init flex state ingress instance lists */
        if (comp == STATE_EFLEX) {
            /* Init ingress action id list */
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_resource_list_init(
                    unit,
                    num_ing_action_id,
                    &ctrl->state_ext.ing_action_id_list_b[pipe_idx]));

            /* Init ingress operand profile id list */
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_resource_list_init(
                    unit,
                    num_op_profile_id,
                    &ctrl->state_ext.ing_op_id_list_b[pipe_idx]));

            /* Init egress range checker id list */
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_resource_list_init(
                    unit,
                    num_range_checker,
                    &ctrl->state_ext.ing_range_chk_id_list_b[pipe_idx]));

            if (num_egress_instance == 2) {
                   /* Init egress action id list */
                   SHR_IF_ERR_EXIT
                      (bcmcth_ctr_eflex_resource_list_init(
                            unit,
                            num_ing_action_id,
                            &ctrl->state_ext.egr_action_id_list_b[pipe_idx]));

                    /* Init egress operand profile id list */
                    SHR_IF_ERR_EXIT
                        (bcmcth_ctr_eflex_resource_list_init(
                            unit,
                            num_op_profile_id,
                            &ctrl->state_ext.egr_op_id_list_b[pipe_idx]));

                    /* Init egress range checker id list */
                       SHR_IF_ERR_EXIT
                       (bcmcth_ctr_eflex_resource_list_init(
                           unit,
                           num_range_checker,
                           &ctrl->state_ext.egr_range_chk_id_list_b[pipe_idx]));
            }
        }
    }

    /* Check if timestamp counter is enabled */
    SHR_IF_ERR_EXIT
        (bcmbd_ts_timestamp_enable_get(unit, BCMEFLEX_TS_INST, &ts_en));
    if (!ts_en) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

    if (!warm) {
        /* Populate ingress OBJ_INFO LT */
        SHR_IF_ERR_EXIT(bcmcth_eflex_obj_info_populate(unit, true, comp));

        /* Populate egress OBJ_INFO LT */
        SHR_IF_ERR_EXIT(bcmcth_eflex_obj_info_populate(unit, false, comp));
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Initialize enhanced flex counter internal structures */
int
bcmcth_ctr_eflex_db_init(int unit, bool warm)
{
    int idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate control structure */
    bcmcth_ctr_eflex_ctrl[unit] = NULL;

    SHR_ALLOC(bcmcth_ctr_eflex_ctrl[unit],
              sizeof(ctr_eflex_control_t),
              "bcmcthCtrEflexControl");
    SHR_NULL_CHECK(bcmcth_ctr_eflex_ctrl[unit], SHR_E_MEMORY);
    sal_memset(bcmcth_ctr_eflex_ctrl[unit], 0, sizeof(ctr_eflex_control_t));

    /* Populate ctr_eflex_device_info structure. */
    SHR_IF_ERR_EXIT
        (bcmcth_ctr_eflex_device_info_get(
            unit,
            &bcmcth_ctr_eflex_ctrl[unit]->ctr_eflex_device_info));

    /*! Initialize info for CTR_xxx_EFLEX LTs with variant support */
    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_lt_info_init(unit));

    /* Populate internal structures. */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_db_init_common(unit, warm, CTR_EFLEX));

    /* Create pool mgr internal structures */
    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_pool_mgr_create(unit, CTR_EFLEX));

    /* Initialize pipe operational mode */
    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_pipe_opermode_cfg_init(
        unit,
        INGRESS,
        CTR_EFLEX,
        &ctr_eflex_ing_pipe_opermode[unit]));

    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_pipe_opermode_cfg_init(
        unit,
        EGRESS,
        CTR_EFLEX,
        &ctr_eflex_egr_pipe_opermode[unit]));

    /* Get the enums used in the EFLEX LTs */
    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_lt_enum_get(unit, CTR_EFLEX));

    /* Populate POOL_INFO LT only during coldboot */
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmcth_ctr_eflex_pool_info_populate(unit, INGRESS, CTR_EFLEX));

        SHR_IF_ERR_EXIT
            (bcmcth_ctr_eflex_pool_info_populate(unit, EGRESS, CTR_EFLEX));
    }

    /* Initialize lt id to invalid value */
    for (idx = 0; idx < HITBIT_LT_NUM; idx++) {
        bcmcth_ctr_eflex_ctrl[unit]->ing_hitbit_lt_info[idx].lt_id =
            bcmcth_ctr_eflex_ctrl[unit]->ctr_eflex_enum.invalid_lt_id;

        bcmcth_ctr_eflex_ctrl[unit]->egr_hitbit_lt_info[idx].lt_id =
            bcmcth_ctr_eflex_ctrl[unit]->ctr_eflex_enum.invalid_lt_id;
    }

    /* Initialize hitbit bank sid to logical id mapping */
    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_hitbit_sid_map_init(unit, warm));

    /* Recover internal state from IMM during warmboot */
    if (warm) {
        SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_warm_recover(unit, CTR_EFLEX));
    }

exit:
    if (SHR_FUNC_ERR()) {
        /* Cleanup internal structures */
        (void)bcmcth_ctr_eflex_db_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*! Initialize enhanced flex state internal structures */
int
bcmcth_state_eflex_db_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Allocate control structure */
    bcmcth_state_eflex_ctrl[unit] = NULL;

    SHR_ALLOC(bcmcth_state_eflex_ctrl[unit],
              sizeof(ctr_eflex_control_t),
              "bcmcthStateEflexControl");
    SHR_NULL_CHECK(bcmcth_state_eflex_ctrl[unit], SHR_E_MEMORY);
    sal_memset(bcmcth_state_eflex_ctrl[unit], 0, sizeof(ctr_eflex_control_t));

    /* Populate ctr_eflex_device_info structure. */
    SHR_IF_ERR_EXIT
        (bcmcth_state_eflex_device_info_get(
            unit,
            &bcmcth_state_eflex_ctrl[unit]->ctr_eflex_device_info));

    /*! Initialize info for FLEX_STATE_xxx LTs with variant support */
    SHR_IF_ERR_EXIT(bcmcth_state_eflex_lt_info_init(unit));

    /* Populate internal structures. */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_db_init_common(unit, warm, STATE_EFLEX));

    /* Create pool mgr internal structures */
    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_pool_mgr_create(unit, STATE_EFLEX));

    /* Populate POOL_INFO LT only during coldboot */
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmcth_ctr_eflex_pool_info_populate(unit, INGRESS, STATE_EFLEX));
        SHR_IF_ERR_EXIT
            (bcmcth_ctr_eflex_pool_info_populate(unit, EGRESS, STATE_EFLEX));
    }

    /* Get the enums used in the EFLEX LTs */
    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_lt_enum_get(unit, STATE_EFLEX));

    /* Recover internal state from IMM during warmboot */
    if (warm) {
        SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_warm_recover(unit, STATE_EFLEX));
    }

exit:
    if (SHR_FUNC_ERR()) {
        /* Cleanup internal structures */
        (void)bcmcth_state_eflex_db_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*! Cleanup enhanced flex counter/state internal structures */
static int
bcmcth_eflex_db_cleanup(int unit, eflex_comp_t comp)
{
    uint32_t num_pipes, pipe_idx;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    int num_egress_instances = 0;
    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    if (ctrl == NULL) {
        return SHR_E_NONE;
    }
    device_info = ctrl->ctr_eflex_device_info;

    if (device_info) {
        num_pipes = device_info->num_pipes;
        num_egress_instances = device_info->state_ext.num_egress_instance;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_destroy(unit, comp));

        /* Free resource lists */
        for (pipe_idx = 0; pipe_idx < num_pipes; pipe_idx++) {
            /* Un-init ingress action id list */
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_resource_list_uninit(
                    unit,
                    &ctrl->ing_action_id_list[pipe_idx]));

            /* Un-init egress action id list */
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_resource_list_uninit(
                    unit,
                    &ctrl->egr_action_id_list[pipe_idx]));

            /* Un-init ingress operand profile id list */
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_resource_list_uninit(
                    unit,
                    &ctrl->ing_op_id_list[pipe_idx]));

            /* Un-init egress operand profile id list */
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_resource_list_uninit(
                    unit,
                    &ctrl->egr_op_id_list[pipe_idx]));

            /* Un-init ingress range checker id list */
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_resource_list_uninit(
                    unit,
                    &ctrl->ing_range_chk_id_list[pipe_idx]));

            /* Un-init egress range checker id list */
            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_resource_list_uninit(
                    unit,
                    &ctrl->egr_range_chk_id_list[pipe_idx]));

            /* Un-init flex state ingress instance lists */
            if (comp == STATE_EFLEX) {
                /* Un-init ingress action id list */
                SHR_IF_ERR_EXIT
                    (bcmcth_ctr_eflex_resource_list_uninit(
                        unit,
                        &ctrl->state_ext.ing_action_id_list_b[pipe_idx]));

                /* Un-init ingress operand profile id list */
                SHR_IF_ERR_EXIT
                    (bcmcth_ctr_eflex_resource_list_uninit(
                        unit,
                        &ctrl->state_ext.ing_op_id_list_b[pipe_idx]));

                    /* Un-init inress range checker id list */
                SHR_IF_ERR_EXIT
                    (bcmcth_ctr_eflex_resource_list_uninit(
                        unit,
                          &ctrl->state_ext.ing_range_chk_id_list_b[pipe_idx]));
                if (num_egress_instances == 2 ) {

                    /* Un-init ingress action id list */
                     SHR_IF_ERR_EXIT
                        (bcmcth_ctr_eflex_resource_list_uninit(
                            unit,
                            &ctrl->state_ext.egr_action_id_list_b[pipe_idx]));

                    /* Un-init ingress operand profile id list */
                    SHR_IF_ERR_EXIT
                        (bcmcth_ctr_eflex_resource_list_uninit(
                            unit,
                            &ctrl->state_ext.egr_op_id_list_b[pipe_idx]));

                    /* Un-init egress range checker id list */
                    SHR_IF_ERR_EXIT
                        (bcmcth_ctr_eflex_resource_list_uninit(
                         unit,
                         &ctrl->state_ext.egr_range_chk_id_list_b[pipe_idx]));
                }
            }
        }
    }
    SHR_FREE(ctrl);

exit:
    SHR_FUNC_EXIT();
}

/*! Cleanup enhanced flex counter internal structures */
int
bcmcth_ctr_eflex_db_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clean-up internal structures. */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_db_cleanup(unit, CTR_EFLEX));
    bcmcth_ctr_eflex_ctrl[unit] = NULL;

    /* De-initialize hitbit bank sid to logical id mapping */
    SHR_IF_ERR_EXIT(bcmcth_ctr_eflex_hitbit_sid_map_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

/*! Cleanup enhanced flex state internal structures */
int
bcmcth_state_eflex_db_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clean-up internal structures. */
    SHR_IF_ERR_EXIT
        (bcmcth_eflex_db_cleanup(unit, STATE_EFLEX));
    bcmcth_state_eflex_ctrl[unit] = NULL;

 exit:
    SHR_FUNC_EXIT();
}
