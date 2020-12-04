/*! \file bcmtm_imm_scheduler_profile.c
 *
 * In-memory call back function for schedulers for scheduling profiles of
 * non cpu ports.
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
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

/*******************************************************************************
 * Local definitions
 */
bcmtm_scheduler_cos_l0_map_t
bcmtm_scheduler_cos_l0_map[BCMDRD_CONFIG_MAX_UNITS][MAX_TM_SCHEDULER_PROFILE];

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM
/*******************************************************************************
 * Private functions
 */

/*! \brief TM scheduler profile imm update.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Scheduler profile ID.
 * \param [in] node_id Scheduler node ID.
 * \param [in] profile Scheduler profile.
 *
 * \retval SHR_E_NONE No Error.
 * \retval !SHR_E_NONE Corresponding error code.
 */
static int
bcmtm_scheduler_profile_imm_update(int unit,
                                   uint8_t profile_id,
                                   uint8_t node_id,
                                   bcmtm_scheduler_profile_t *profile)
{
    int idx;
    bcmltd_fields_t in;
    bcmltd_fid_t fid;
    bcmltd_sid_t ltid = TM_SCHEDULER_PROFILEt;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, 3, &in));

    in.count = 0;
    fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_PROFILE_IDf;

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, profile_id));

    for (idx = 0; idx < MAX_TM_SCHEDULER_NODE; idx++) {
        /*
         * Check opcode status in the profile and update imm only if entry
         * is present.
         */
        if ((idx != node_id) && (profile[idx].opcode != ENTRY_INVALID)) {
            in.count = 1;
            fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_NODE_IDf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in, fid, 0, idx));
            fval = profile[idx].opcode;
            /* operational status update */
            fid = TM_SCHEDULER_PROFILEt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &in, fid, 0, fval));

            /* update IMM entry. */
            SHR_IF_ERR_EXIT
                (bcmimm_entry_update(unit, 0, ltid, &in));
        }
    }
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Populates scheduler configuration using user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] sched_cfg Shaper configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static void
bcmtm_scheduler_profile_fill(int unit,
                             const bcmltd_field_t *data,
                             bcmtm_scheduler_profile_t *scheduler_profile)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;

    while (cur_data) {
        switch (cur_data->id) {
            case TM_SCHEDULER_PROFILEt_NUM_UC_Qf:
                scheduler_profile->num_ucq = cur_data->data;
                break;
            case TM_SCHEDULER_PROFILEt_NUM_MC_Qf:
                scheduler_profile->num_mcq = cur_data->data;
                break;
            case TM_SCHEDULER_PROFILEt_FLOW_CTRL_UCf:
                scheduler_profile->fc_uc = cur_data->data;
                break;
            case TM_SCHEDULER_PROFILEt_OPERATIONAL_STATEf:
                scheduler_profile->opcode = cur_data->data;
                break;
            default:
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief  Scheduler profile physical table configuration based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] profile_id Scheduler profile ID.
 * \param [in] profile Scheduler profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_profile_pt_set(int unit,
                               bcmltd_sid_t ltid,
                               uint8_t profile_id,
                               bcmtm_scheduler_profile_t *profile)
{
    bcmtm_drv_t *drv;
    bcmtm_scheduler_drv_t sched_drv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&sched_drv, 0, sizeof(bcmtm_scheduler_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->scheduler_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->scheduler_drv_get(unit, &sched_drv));
    /* update physical table. */
    if (sched_drv.profile_set) {
        SHR_IF_ERR_EXIT
            (sched_drv.profile_set(unit, ltid, profile_id, profile));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Scheduler profile initialization.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Scheduler profile ID.
 * \param [in] profile Scheduler profile.
 */
static int
bcmtm_scheduler_profile_dev_init(int unit,
                                 uint8_t profile_id,
                                 bcmtm_scheduler_profile_t *profile)
{
    bcmtm_drv_t *drv;
    bcmtm_scheduler_drv_t sched_drv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&sched_drv, 0, sizeof(bcmtm_scheduler_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->scheduler_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->scheduler_drv_get(unit, &sched_drv));
    /* update physical table. */
    if (sched_drv.profile_init) {
        SHR_IF_ERR_EXIT
            (sched_drv.profile_init(unit, profile_id, profile));
    }
exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] profile_id Scheduler  profile ID.
 * \param [in] node_id Scheduler node ID.
 * \param [in] profile Scheduler profile.
 * \param [out] opcode Operational status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_profile_set(int unit,
                            bcmltd_sid_t ltid,
                            uint8_t profile_id,
                            uint8_t profile_state,
                            uint8_t node_id,
                            bcmtm_scheduler_profile_t *profile,
                            uint64_t *opcode)
{
    uint8_t status = 0;
    uint8_t action;

    SHR_FUNC_ENTER(unit);

    /* Validate the new profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_scheduler_profile_validate(unit, (int)node_id,
                                          profile_id, profile, &status));

    /*
     * Hardware update only if state changes
     * from valid to invalid or vice-versa.
     */
    if (status != profile_state) {
        if (status) {
            action = ACTION_PROFILE_UPDATE_INT;
            /* New profile is valid profile */
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_profile_pt_set(unit, ltid, profile_id, profile));
            /* This updates port configuration dependent on scheduler profile. */
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_port_profile_update(unit, profile_id,
                                                     profile, action));
        } else {
            action = ACTION_PROFILE_DELETE_INT;
            /* This updates port configuration dependent on scheduler profile. */
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_port_profile_update(unit, profile_id,
                                                     profile, action));
            /*
             * Profile to be deleted from hardware for invalid profile.
             */
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_profile_pt_set(unit, ltid, profile_id, NULL));
        }
    }
    *opcode = profile[node_id].opcode;

    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_profile_imm_update(unit, profile_id, node_id, profile));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_SCHEDULER_PROFILE logical table callback function for staging.
 *
 * Handle TM_SCHEDULER_PROFILE IMM table change events.
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
bcmtm_scheduler_profile_stage(int unit,
        bcmltd_sid_t ltid,
        uint32_t trans_id,
        bcmimm_entry_event_t event_reason,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    uint8_t profile_id, node_id;
    uint64_t fval;
    bcmltd_fid_t fid;
    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE];
    uint8_t profile_valid;

    SHR_FUNC_ENTER(unit);

    sal_memset(profile, 0,
            (sizeof(bcmtm_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE));

    /* TM_SCHEDULER_PROFILEt_TM_SCHEDULER_PROFILE_IDf */
    fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_PROFILE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    profile_id = (uint8_t)fval;

    /* TM_SCHEDULER_PROFILEt_TM_SCHEDULER_NODE_IDf */
    fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    node_id = (uint8_t)fval;

    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_profile_get(unit, profile_id, profile));

    /* checks for the existing profile status. */
    profile_valid = bcmtm_scheduler_profile_is_valid(unit, profile);

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            /* Add the newly added node in profile. */
            bcmtm_scheduler_profile_fill(unit, data, &profile[node_id]);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmtm_scheduler_profile_set(unit, ltid,
                                            profile_id, profile_valid,
                                            node_id, profile, &fval));
            fid = TM_SCHEDULER_PROFILEt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields, fid, 0, fval));
            break;
        case BCMIMM_ENTRY_DELETE:
            /* Delete the entry from profile */
            sal_memset(&profile[node_id], 0, sizeof(bcmtm_scheduler_profile_t));
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_profile_set(unit, ltid,
                                             profile_id, profile_valid,
                                             node_id, profile, &fval));
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
bcmtm_scheduler_profile_is_valid(int unit,
                                 bcmtm_scheduler_profile_t *profile)
{
    int idx = 0;

    /* if the opdcode is valid it return true. */
    for (idx = 0; idx < MAX_TM_SCHEDULER_NODE; idx++) {
        if (profile[idx].opcode == VALID) {
            return 1;
        }
    }
    return 0;
}

int
bcmtm_scheduler_profile_validate(int unit,
                                 int node_id,
                                 uint8_t profile_id,
                                 bcmtm_scheduler_profile_t *profile,
                                 uint8_t *status)
{
    bcmtm_drv_t *drv;
    bcmtm_scheduler_drv_t sched_drv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&sched_drv, 0, sizeof(bcmtm_scheduler_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->scheduler_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->scheduler_drv_get(unit, &sched_drv));

    if ((node_id != -1) &&
       (sched_drv.node_profile_validate)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            sched_drv.node_profile_validate(unit, node_id, &profile[node_id]));

    }
    if (sched_drv.profile_validate) {
        SHR_IF_ERR_EXIT(
            sched_drv.profile_validate(unit, profile_id, node_id, profile));
    }
    *status = bcmtm_scheduler_profile_is_valid(unit, profile);
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_profile_get(int unit,
                            uint8_t profile_id,
                            bcmtm_scheduler_profile_t *profile)
{
    int node_id, rv;
    bcmltd_fields_t out, in;
    size_t num_fid, count;
    bcmltd_fid_t fid;
    bcmltd_sid_t ltid = TM_SCHEDULER_PROFILEt;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));

    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, num_fid, &out));
    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, 2, &in));

    in.count = 0;
    fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_PROFILE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, profile_id));

    for (node_id = 0; node_id < MAX_TM_SCHEDULER_NODE; node_id++) {
        in.count = 1; /* as profile id is already populated. */
        fid = TM_SCHEDULER_PROFILEt_TM_SCHEDULER_NODE_IDf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in, fid, 0, node_id));

        out.count = num_fid;
        for (count = 0; count < num_fid; count++) {
            out.field[count]->data = 0;
            out.field[count]->id = 0;
            out.field[count]->idx = 0;
        }
        rv = bcmimm_entry_lookup(unit, ltid, &in, &out);
        if (rv == SHR_E_NONE) {
            bcmtm_scheduler_profile_fill(unit, *(out.field), &profile[node_id]);
        } else {
            /* Entry is not in IMM. Marking the opcode as invalid. */
            profile[node_id].opcode = ENTRY_INVALID;
        }
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_profile_imm_init(int unit, bool warm)
{
    int profile_id = 0, rv;
    uint8_t status;
    const bcmlrd_map_t *map = NULL;

    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE];

    SHR_FUNC_ENTER(unit);
    if (warm) {
        rv = bcmlrd_map_get(unit, TM_SCHEDULER_PROFILEt, &map);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

        if ((rv == SHR_E_UNAVAIL) || (!map)) {
            SHR_EXIT();
        }

        for ( ; profile_id < MAX_TM_SCHEDULER_PROFILE; profile_id++) {
            sal_memset(profile, 0,
                    sizeof(bcmtm_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE);
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_profile_get(unit, profile_id, profile));

            /*
             * passing node_id == -1 just to validate the profile.
             * Node validation for the profile will be skipped.
             */
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_profile_validate(unit, -1, profile_id,
                                                  profile, &status));
            if (status) {
                SHR_IF_ERR_EXIT
                    (bcmtm_scheduler_profile_dev_init(unit, profile_id, profile));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_profile_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;

    /*! Inmemory callback functions for TM_SCHEDULER_PROFILE table. */
    static bcmimm_lt_cb_t bcmtm_scheduler_profile_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_scheduler_profile_stage,
    };
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_SCHEDULER_PROFILEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /*! Registers callback functions for Scheduler profile. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_SCHEDULER_PROFILEt,
                             &bcmtm_scheduler_profile_imm_cb, NULL));

exit:
    SHR_FUNC_EXIT();
}
