/*! \file bcmtm_imm_scheduler_port.c
 *
 * In-memory call back function for schedulers for scheduling ports of
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
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>
#include <bcmtm/sched_shaper/bcmtm_shaper_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
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
bcmtm_scheduler_port_cfg_fill(int unit,
                              const bcmltd_field_t *data,
                              bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;

    while (cur_data) {
        switch (cur_data->id) {
            case TM_SCHEDULER_PORT_PROFILEt_TM_SCHEDULER_PROFILE_IDf:
                scheduler_cfg->profile_id = cur_data->data;
                break;
            case TM_SCHEDULER_PORT_PROFILEt_WRRf:
                scheduler_cfg->wrr_enable = cur_data->data;
                break;
            case TM_SCHEDULER_PORT_PROFILEt_OPERATIONAL_STATEf:
                scheduler_cfg->opcode = cur_data->data;
                break;
            default:
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief  Get the application configured values in IMM for port scheduler
 * profiles.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * modified entry.
 * \param [out] sched_cfg Shaper configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_port_profile_imm_lkup(int unit,
                                      bcmtm_lport_t lport,
                                      bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmltd_fields_t out, in;
    bcmlrd_sid_t ltid = TM_SCHEDULER_PORT_PROFILEt;
    bcmlrd_fid_t fid;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));
    sal_memset(scheduler_cfg, 0, sizeof(bcmtm_scheduler_cfg_t));

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));

    in.count = 0;
    fid = TM_SCHEDULER_PORT_PROFILEt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    if (SHR_E_NONE != bcmimm_entry_lookup(unit, ltid, &in, &out)) {
        scheduler_cfg->opcode = ENTRY_INVALID;
        SHR_EXIT();
    }
    bcmtm_scheduler_port_cfg_fill(unit, *(out.field), scheduler_cfg);
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Update the operational state values in IMM.
 * profiles.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * \param [in] opcode Operational code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_port_profile_imm_update(int unit,
                                        bcmtm_lport_t lport,
                                        bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmltd_fields_t in;
    bcmlrd_fid_t fid;
    bcmlrd_sid_t ltid = TM_SCHEDULER_PORT_PROFILEt;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));

    in.count = 0;
    fid = TM_SCHEDULER_PORT_PROFILEt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* update the operational state for the profile. */
    fid = TM_SCHEDULER_PORT_PROFILEt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, scheduler_cfg->opcode));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Shaper port physical table configuration based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port number.
 * \param [in] profile Scheduler profile.
 * \param [in] sched_cfg Shaper configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_port_pt_set(int unit,
                            bcmltd_sid_t ltid,
                            bcmtm_lport_t lport,
                            bcmtm_scheduler_profile_t *profile,
                            bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmtm_drv_t *drv;
    bcmtm_scheduler_drv_t sched_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&sched_drv, 0, sizeof(bcmtm_scheduler_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->scheduler_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->scheduler_drv_get(unit, &sched_drv));
    /* update physical table. */
    if (sched_drv.port_set) {
        SHR_IF_ERR_EXIT
            (sched_drv.port_set(unit, ltid, lport, profile, scheduler_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Resets the physical table to default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] port Logical Port ID.
 * \param [in] scheduler_cfg Scheduler configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_port_reset(int unit,
                           bcmltd_sid_t ltid,
                           bcmtm_lport_t lport,
                           bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    SHR_FUNC_ENTER(unit);
    /*
     * Physical table update.
     * Get the opcode. The hardware entry will be updated
     * only if the entry is valid.
     */
    if (scheduler_cfg->opcode ==  VALID) {
        sal_memset(scheduler_cfg, 0, sizeof(bcmtm_scheduler_cfg_t));
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_port_pt_set(unit, ltid, lport,
                                         NULL, scheduler_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.

 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] scheduler_cfg Scheduler configurations.
 * \param [out] output_fields This is linked list of extra added data fields
 * (mostly read only fields.)
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_port_set(int unit,
                         bcmltd_sid_t ltid,
                         bcmtm_lport_t lport,
                         bcmtm_scheduler_profile_t *profile,
                         bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    int mport;
    uint8_t status = 0, profile_id, action;

    SHR_FUNC_ENTER(unit);

    action = scheduler_cfg->action;

    /*
     * In case of internal update profile validation is not needed. As calling
     * function has information about the profile status.
     */
    switch (action) {
        case ACTION_INSERT:
        case ACTION_PROFILE_UPDATE:
            /*
             * TM local port information.
             * Returns error if local port to physical port map is not available.
             * Updates opeartional state.
             */
            if (SHR_FAILURE
                    (bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
                scheduler_cfg->opcode = PORT_INFO_UNAVAIL;
                SHR_EXIT();
            }
            /*For internal port updates profile needs to be verified. */
        case ACTION_PORT_UPDATE_INT:
            sal_memset(profile, 0,
                    (sizeof(bcmtm_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE));
            profile_id = scheduler_cfg->profile_id;
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_profile_get(unit, profile_id, profile));

            /* check for profile is OK to configure to hardware. */
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_profile_validate(unit, -1, profile_id,
                                                  profile, &status));
            if (status) {
                /* Profile is valid */
                scheduler_cfg->opcode = VALID;
            } else {
                /* Invalid scheduler profile. */
                scheduler_cfg->opcode = SCHED_PROFILE_INVALID;
            }
            /*
             * Update in profile ID will lead to update in port hierarchy
             * and hence need to update all the scheduler and shaper
             * configurations.
             */
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_node_internal_update(unit, lport, profile_id,
                                                      profile, action));
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_node_internal_update(unit, lport, profile_id,
                                                   profile, action));
            break;

        case ACTION_PROFILE_UPDATE_INT:
            /*
             * Update invalid scheduler profile. Port should be valid
             * in this case. Since we do port validation before
             * profile validation we donot need port validation. */
            if (scheduler_cfg->opcode == SCHED_PROFILE_INVALID) {
                scheduler_cfg->opcode = VALID;
            }
            break;

        case ACTION_PROFILE_DELETE_INT:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_port_reset(unit, ltid, lport, scheduler_cfg));
            scheduler_cfg->opcode = SCHED_PROFILE_INVALID;
            break;

        case ACTION_PORT_DELETE_INT:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_port_reset(unit, ltid, lport, scheduler_cfg));
            scheduler_cfg->opcode = PORT_INFO_UNAVAIL;
            break;
    }

    /* PT update */
    if (scheduler_cfg->opcode == VALID) {
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_port_pt_set(unit, ltid, lport,
                                         profile, scheduler_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_SCHEDULER_NODE logical table callback function for staging.
 *
 * Handle TM_SCHEDULER_NODE IMM table change events.
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
bcmtm_scheduler_port_stage(int unit,
                           bcmltd_sid_t ltid,
                           uint32_t trans_id,
                           bcmimm_entry_event_t event_reason,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           void *context,
                           bcmltd_fields_t *output_fields)
{
    bcmtm_scheduler_cfg_t scheduler_cfg;
    bcmltd_fid_t fid;
    uint64_t fval;
    bcmtm_lport_t lport;
    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE];
    int prev_profile = -1; /* Initializing to invalid */
    SHR_FUNC_ENTER(unit);

    sal_memset(&scheduler_cfg, 0, sizeof(bcmtm_scheduler_cfg_t));

    /* TM_SCHEDULER_PORT_PROFILEt_PORT_IDf */
    fid = TM_SCHEDULER_PORT_PROFILEt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_port_profile_imm_lkup(unit, lport,
                                                       &scheduler_cfg));
            prev_profile = scheduler_cfg.profile_id;
        /* Fall through */
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            bcmtm_scheduler_port_cfg_fill(unit, data, &scheduler_cfg);
            if (scheduler_cfg.profile_id != prev_profile && prev_profile != -1) {
                scheduler_cfg.action = ACTION_PROFILE_UPDATE;
            } else {
                scheduler_cfg.action = ACTION_INSERT;
            }
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_port_set(unit, ltid, lport,
                                          profile, &scheduler_cfg));
            /* update the operational state for external updates. */
            fid = TM_SCHEDULER_PORT_PROFILEt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields, fid,
                                      0, scheduler_cfg.opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_port_profile_imm_lkup(unit, lport,
                                                       &scheduler_cfg));
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_port_reset(unit, ltid, lport, &scheduler_cfg));
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
bcmtm_scheduler_port_update(int unit,
                            bcmtm_lport_t lport,
                            uint8_t profile_id,
                            bcmtm_scheduler_profile_t *profile,
                            uint8_t action)
{
    bcmtm_scheduler_cfg_t scheduler_cfg;
    bcmltd_sid_t ltid = TM_SCHEDULER_PORT_PROFILEt;

    SHR_FUNC_ENTER(unit);

    /* Shaper port updates */
    if ((action == ACTION_PORT_DELETE_INT) ||
            (action == ACTION_PORT_UPDATE_INT)) {
        /* call for shaper port update. */
        SHR_IF_ERR_EXIT
            (bcmtm_shaper_port_internal_update(unit, lport, action));
    }

    /* Get the scheduler port profile from IMM. */
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_port_profile_imm_lkup(unit, lport, &scheduler_cfg));

    scheduler_cfg.action = action;

    switch (scheduler_cfg.opcode) {
        case ENTRY_INVALID:
            /* Entry not available in IMM. */
            SHR_EXIT();
        case PORT_INFO_UNAVAIL:
            /* Port information in unavailable. */
            if (action != ACTION_PORT_UPDATE_INT) {
                SHR_EXIT();
            }
            break;
        case SCHED_PROFILE_INVALID:
            /* Scheduler profile is invalid. */
            if (action != ACTION_PROFILE_UPDATE_INT) {
                SHR_EXIT();
            }
            break;
        default:
            break;
    }

    /* Get profile from imm in case of port update */
    if ((action == ACTION_PORT_DELETE_INT) ||
        (action == ACTION_PORT_UPDATE_INT)) {
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_profile_get(unit, scheduler_cfg.profile_id,
                                         profile));
    }

    if ((profile_id == scheduler_cfg.profile_id) ||
            (profile_id == PORT_UPDATE)) {
        /* Saving the profile id as in case of delete the scheduler config may get memset. */
        profile_id = scheduler_cfg.profile_id;
        /* call profile update. */
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_port_set(unit, ltid, lport,
                                      profile, &scheduler_cfg));

        /* update the opcode for the entry in imm. */
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_port_profile_imm_update(unit, lport,
                                                     &scheduler_cfg));

        /* call for scheduler node update. */
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_node_internal_update(unit, lport,
                                                  profile_id,
                                                  profile, action));
        /* call for shaper node update */
        SHR_IF_ERR_EXIT
            (bcmtm_shaper_node_internal_update(unit, lport,
                                               profile_id,
                                               profile, action));
    }


exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_port_profile_update(int unit,
                                    uint8_t profile_id,
                                    bcmtm_scheduler_profile_t *profile,
                                    uint8_t action)
{
    bcmtm_lport_t lport;
    bcmlrd_sid_t ltid= TM_SCHEDULER_PORT_PROFILEt;
    uint64_t min, max;
    bcmlrd_fid_t fid = TM_SCHEDULER_PORT_PROFILEt_PORT_IDf;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid, &min, &max));

    for (lport = min; lport <= max; lport++) {
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_port_update(unit, lport, profile_id,
                                         profile, action));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_port_profile_get(int unit,
                                 bcmtm_lport_t lport,
                                 uint8_t *profile_id)
{
    bcmtm_scheduler_cfg_t scheduler_cfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_port_profile_imm_lkup(unit, lport, &scheduler_cfg));

    if (scheduler_cfg.opcode != VALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    *profile_id = scheduler_cfg.profile_id;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_port_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    /*! Inmemory callback functions for TM_SCHEDULER_PORT_PROFILE table. */
    static bcmimm_lt_cb_t bcmtm_scheduler_port_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_scheduler_port_stage,
    };
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_SCHEDULER_PORT_PROFILEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /*! Registers callback functions for Scheduler port. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_SCHEDULER_PORT_PROFILEt,
                             &bcmtm_scheduler_port_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
