/*! \file bcmtm_imm_scheduler_sp_profile.c
 *
 * In-memory call back function for scheduler SP profile. This is for
 * selecting COS by the port and not the scheduling descipline between two
 * queues of a COS.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief TM scheduler EB SP profile set.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] profile_id Profile ID.
 * \param [in] data Data field list.
 * \param [in] output_fields Read only field list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Corresponding error code.
 */
static int
bcmtm_scheduler_sp_profile_set(int unit,
                               bcmltd_sid_t ltid,
                               uint8_t profile_id,
                               const bcmltd_field_t *data,
                               bcmltd_fields_t *output_fields)
{
    uint32_t sp_cos = 255, idx;
    uint64_t fval;
    bcmltd_fid_t fid;
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;
    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, MAX_TM_SCHEDULER_NODE));

    output_fields->count = 0;
    /* get the lowest cos for which SP is enabled. */
    while (cur_data) {
        if (cur_data->id == TM_SCHEDULER_SP_PROFILEt_STRICT_PRIORITYf) {
            if ((sp_cos > cur_data->idx) && (cur_data->data)) {
                sp_cos = cur_data->idx;
            }
        }
        cur_data = cur_data->next;
    }
    /* Get the scheduler profile */
    sal_memset(profile, 0,
               (sizeof(bcmtm_scheduler_profile_t)* MAX_TM_SCHEDULER_NODE));
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_profile_get(unit, profile_id, profile));

    fid = TM_SCHEDULER_SP_PROFILEt_STRICT_PRIORITY_OPERf;

    for (idx = 0; idx < MAX_TM_SCHEDULER_NODE; idx++) {
        /* Enable SP mode for all the higher valid cos. */
        if ((idx >= sp_cos) && (profile[idx].opcode == VALID)) {
           fval = 1;
        } else {
            fval = 0;
        }

        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields, fid, idx, fval));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM scheduler EB SP profile update.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] profile_id Profile ID.
 * \param [in] data Data field list.
 * \param [in] output_fields Read only field list.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Corresponding error code.
 */
static int
bcmtm_scheduler_sp_profile_update(int unit,
                                  bcmltd_sid_t ltid,
                                  uint8_t profile_id,
                                  const bcmltd_field_t *data,
                                  bcmltd_fields_t *output_fields)
{
    bcmltd_fields_t out, in;
    int int_update = 0;
    size_t num_fid;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    num_fid = 2 * MAX_TM_SCHEDULER_NODE + 1;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));

    in.count = 0;
    fid = TM_SCHEDULER_SP_PROFILEt_TM_SCHEDULER_PROFILE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, profile_id));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit, ltid, &in, &out));

    /* Updating with updated entry */
    while (data) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &out, data->id,
                                  data->idx, data->data));
        data = data->next;
    }
    /*! Internal updates. */
    if (!output_fields) {
        output_fields = &out;
        int_update = 1;
    }
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_sp_profile_set(unit, ltid, profile_id,
                                        *(out.field), output_fields));

    if (int_update) {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, 0, ltid, &out));
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_SCHEDULER_SP_PROFILE logical table callback function for staging.
 *
 * Handle TM_SCHEDULER_SP_PROFILE IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] data. This is a linked list of extra added data fields (mostly
 * read only fields).
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmtm_scheduler_sp_profile_stage(int unit,
                                 bcmltd_sid_t ltid,
                                 uint32_t trans_id,
                                 bcmimm_entry_event_t event_reason,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 void *context,
                                 bcmltd_fields_t *output_fields)
{
    uint64_t fval;
    uint8_t profile_id;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    /* TM_SCHEDULER_SP_PROFILEt_TM_SCHEDULER_PROFILE_IDf */
    fid = TM_SCHEDULER_SP_PROFILEt_TM_SCHEDULER_PROFILE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    profile_id = (uint8_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_sp_profile_update(unit, ltid, profile_id,
                                                   data, output_fields));
            break;
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_sp_profile_set(unit, ltid, profile_id,
                                                data, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            /* Nothing to do. */
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_scheduler_sp_profile_internal_update(int unit,
                                           uint8_t profile_id)
{
    bcmltd_sid_t ltid = TM_SCHEDULER_SP_PROFILEt;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_sp_profile_update(unit, ltid, profile_id, NULL, NULL));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_sp_profile_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    /*! Inmemory callback functions for TM_SCHEDULER_SP_PROFILE logical table. */
    static bcmimm_lt_cb_t bcmtm_scheduler_sp_profile_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_scheduler_sp_profile_stage,
    };
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_SCHEDULER_SP_PROFILEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /*! Registers callback functions for Scheduler sp profile. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_SCHEDULER_SP_PROFILEt,
                             &bcmtm_scheduler_sp_profile_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}

