/*! \file bcmtm_imm_scheduler_node.c
 *
 * In-memory call back function for schedulers for scheduling nodes of
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
#include <bcmtm/bcmtm_utils.h>
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
 * \param [out] scheduler_cfg Shaper configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static void
bcmtm_scheduler_node_cfg_fill(int unit,
                              const bcmltd_field_t *data,
                              bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;
    while (cur_data) {
        switch (cur_data->id) {
            case TM_SCHEDULER_NODEt_SCHED_MODEf:
                scheduler_cfg->sched_mode = cur_data->data;
                break;
            case TM_SCHEDULER_NODEt_WEIGHTf:
                scheduler_cfg->weight = cur_data->data;
                break;
            case TM_SCHEDULER_NODEt_OPERATIONAL_STATEf:
                scheduler_cfg->opcode = cur_data->data;
                break;
            default:
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief Perform IMM lookup for the keys and  populates scheduler
 *   configuration with earlier configured values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] level Scheduler level.
 * \param [in] node_id Scheduler node ID.
 * \param [out] scheduler_cfg Scheduler configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_node_imm_lkup(int unit,
                              bcmltd_sid_t ltid,
                              bcmtm_lport_t lport,
                              uint32_t level,
                              uint32_t node_id,
                              bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmltd_fields_t out, in;
    size_t num_fid;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 3, &in));

    in.count = 0;

    /* TM_SCHEDULER_NODEt_PORT_IDf */
    fid = TM_SCHEDULER_NODEt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_SCHEDULER_NODEt_TM_SCHEDULER_NODE_IDf */
    fid = TM_SCHEDULER_NODEt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, node_id));

    /*! TM_SCHEDULER_NODEt_SCHED_NODEf */
    fid = TM_SCHEDULER_NODEt_SCHED_NODEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, level));


    if (SHR_E_NONE != bcmimm_entry_lookup(unit, ltid, &in, &out)) {
        scheduler_cfg->opcode = ENTRY_INVALID;
        SHR_EXIT();
    }

    /* update scheduler_cfg based on imm data. */
    bcmtm_scheduler_node_cfg_fill(unit, *(out.field), scheduler_cfg);
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Perform IMM update for the operational state of the scheduler node.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] level Scheduler level.
 * \param [in] node_id Scheduler node ID.
 * \param [in] opcode Operational state of the node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_node_imm_update(int unit,
                                bcmltd_sid_t ltid,
                                bcmtm_lport_t lport,
                                uint32_t level,
                                uint32_t node_id,
                                bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 4, &in));

    in.count = 0;

    /* TM_SCHEDULER_NODEt_PORT_IDf */
    fid = TM_SCHEDULER_NODEt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_SCHEDULER_NODEt_TM_SCHEDULER_NODE_IDf */
    fid = TM_SCHEDULER_NODEt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, node_id));

    /* TM_SCHEDULER_NODEt_SCHED_NODEf */
    fid = TM_SCHEDULER_NODEt_SCHED_NODEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, level));


    /* TM_SCHEDULER_NODEt_OPERATIONAL_STATEf */
    fid = TM_SCHEDULER_NODEt_OPERATIONAL_STATEf;
    fval = scheduler_cfg->opcode;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0,  fval));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Shaper node physical table configuration based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] lport Logical port number.
 * \param [in] node_id Scheduler node ID.
 * \param [in] ltid Logical table ID.
 * \param [in] scheduler_cfg Shaper configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_node_pt_set(int unit,
                            bcmltd_sid_t ltid,
                            bcmtm_lport_t lport,
                            uint32_t node_id,
                            uint32_t level,
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
    if (sched_drv.node_set) {
        SHR_IF_ERR_EXIT
            (sched_drv.node_set(unit, lport, node_id,
                                level, ltid, scheduler_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] node_id Scheduler node ID.
 * \param [in] level Scheduler level.
 * \param [in] scheduler_cfg Scheduler configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_node_reset(int unit,
                           bcmltd_sid_t ltid,
                           bcmtm_lport_t lport,
                           uint32_t node_id,
                           uint32_t level,
                           bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    uint8_t profile_id;
    SHR_FUNC_ENTER(unit);

    /*
     * Physical table will be updated only if opcode is valid
     */
    if (scheduler_cfg->opcode == VALID) {
        profile_id = scheduler_cfg->profile_id;
        sal_memset(scheduler_cfg, 0, sizeof(bcmtm_scheduler_cfg_t));
        scheduler_cfg->profile_id = profile_id;
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_node_pt_set(unit, ltid, lport,
                                         node_id, level,
                                         scheduler_cfg));
    }
exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] node_id Scheduler node ID.
 * \param [in] level Scheduler level.
 * \param [in] scheduler_cfg Scheduler configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_node_set(int unit,
                         bcmltd_sid_t ltid,
                         bcmtm_lport_t lport,
                         uint32_t node_id,
                         uint32_t level,
                         bcmtm_scheduler_profile_t *profile,
                         bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    int mport;
    uint8_t prof_id;
    uint8_t action;

    SHR_FUNC_ENTER(unit);

    action = scheduler_cfg->action;
    switch (action) {

        case ACTION_INSERT:
            /*
             * TM local port information.
             * Returns error if local port to physical port map is not available.
             * Updates opeartional state.
             */
            if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
                scheduler_cfg->opcode = PORT_INFO_UNAVAIL;
                SHR_EXIT();
            }
            /* Fall through */
        case ACTION_PORT_UPDATE_INT:
            /* Port update needs profile verification. */
            if (SHR_FAILURE
                    (bcmtm_scheduler_port_profile_get(unit, lport, &prof_id))) {
                scheduler_cfg->opcode = SCHED_PROFILE_INVALID;
                SHR_EXIT();
            }
            sal_memset(profile, 0,
                    sizeof(bcmtm_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE);
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_profile_get(unit, prof_id, profile));

            if (SHR_FAILURE
                    (bcmtm_scheduler_node_id_validate(unit, lport, level,
                                                      node_id, profile))) {
                scheduler_cfg->opcode = SCHED_NODE_ID_INVALID;
                SHR_EXIT();
            }
            scheduler_cfg->profile_id = prof_id;
            scheduler_cfg->opcode = VALID;
            break;
        case ACTION_PORT_DELETE_INT:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_node_reset(unit, ltid, lport, node_id,
                                            level, scheduler_cfg));
            scheduler_cfg->opcode = PORT_INFO_UNAVAIL;
            break;
        case ACTION_PROFILE_DELETE_INT:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_node_reset(unit, ltid, lport, node_id,
                                            level, scheduler_cfg));
            scheduler_cfg->opcode = SCHED_PROFILE_INVALID;
            break;
        case ACTION_PROFILE_UPDATE_INT:
        case ACTION_PROFILE_UPDATE:
            /*
             * Port validation done before profile validation.
             * If opcode == SCHED_PROFILE_INVALID then entry becomes valid.
             */
            if (scheduler_cfg->opcode == SCHED_PROFILE_INVALID) {
                scheduler_cfg->opcode = VALID;
            }
            if (SHR_FAILURE
                    (bcmtm_scheduler_node_id_validate(unit, lport, level,
                                                      node_id, profile))) {
                scheduler_cfg->opcode = SCHED_NODE_ID_INVALID;
                SHR_EXIT();
            }
            break;
        default:
            break;
    }

    if (scheduler_cfg->opcode == VALID) {
        SHR_IF_ERR_EXIT
            (bcmtm_scheduler_node_pt_set(unit, ltid, lport, node_id,
                                         level, scheduler_cfg));
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
bcmtm_scheduler_node_stage(int unit,
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
    bcmtm_lport_t lport;
    uint64_t fval;
    uint32_t node_id, level;
    uint8_t profile_id;
    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE];

    SHR_FUNC_ENTER(unit);

    sal_memset(&scheduler_cfg, 0, sizeof(bcmtm_scheduler_cfg_t));

    /* TM_SCHEDULER_NODEt_PORT_IDf */
    fid = TM_SCHEDULER_NODEt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    /* TM_SCHEDULER_NODEt_TM_SCHEDULER_NODE_IDf */
    fid = TM_SCHEDULER_NODEt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    node_id = (uint32_t)fval;

    /*! TM_SCHEDULER_NODEt_SCHED_NODEf */
    fid = TM_SCHEDULER_NODEt_SCHED_NODEf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    level = (uint32_t)fval;

    if (event_reason != BCMIMM_ENTRY_DELETE) {
        scheduler_cfg.sched_mode = DEFAULT_SCHED_MODE;
        scheduler_cfg.weight = DEFAULT_WEIGHT;
    }
    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_node_imm_lkup(unit, ltid, lport, level,
                                               node_id, &scheduler_cfg));
            /* Fall through */
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            bcmtm_scheduler_node_cfg_fill(unit, data, &scheduler_cfg);
            scheduler_cfg.action = ACTION_INSERT;
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_node_set(unit, ltid, lport,
                                          node_id, level, profile,
                                          &scheduler_cfg));

            /* update the operational state. */
            fid = TM_SCHEDULER_NODEt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields, fid,
                                      0, scheduler_cfg.opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_node_imm_lkup(unit, ltid, lport, level,
                                               node_id, &scheduler_cfg));
            if (scheduler_cfg.opcode == VALID) {
                SHR_IF_ERR_EXIT
                    (bcmtm_scheduler_port_profile_get(unit, lport, &profile_id));
                scheduler_cfg.profile_id = profile_id;
                SHR_IF_ERR_EXIT
                    (bcmtm_scheduler_node_reset(unit, ltid, lport, node_id,
                                                level, &scheduler_cfg));
            }
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
bcmtm_scheduler_node_internal_update(int unit,
                                    bcmtm_lport_t lport,
                                    uint8_t profile_id,
                                    bcmtm_scheduler_profile_t *profile,
                                    uint8_t action)
{
    uint32_t node_id, level;
    uint64_t node_min, node_max;
    uint64_t level_min, level_max;
    int num_ucq, num_mcq;
    bcmltd_sid_t ltid = TM_SCHEDULER_NODEt;
    bcmltd_fid_t fid;
    bcmtm_scheduler_cfg_t scheduler_cfg;

    SHR_FUNC_ENTER(unit);

    sal_memset(&scheduler_cfg, 0, sizeof(bcmtm_scheduler_cfg_t));

    fid = TM_SCHEDULER_NODEt_SCHED_NODEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid, &level_min, &level_max));

    fid = TM_SCHEDULER_NODEt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid, &node_min, &node_max));

    SHR_IF_ERR_EXIT
        (bcmtm_lport_num_ucq_get(unit, lport, &num_ucq));

    SHR_IF_ERR_EXIT
        (bcmtm_lport_num_mcq_get(unit, lport, &num_mcq));

    for (level = level_min; level <= level_max; level++) {
        if (level != L0_SCHED_COS) {
            node_min = 0;
            node_max = (level == L1_SCHED_UNICAST_QUEUE)? num_ucq: num_mcq;
        }
        for (node_id = node_min; node_id <= node_max; node_id++) {
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_node_imm_lkup(unit, ltid, lport,
                                               level, node_id, &scheduler_cfg));

            scheduler_cfg.action = action;
            scheduler_cfg.profile_id = profile_id;
            if (scheduler_cfg.opcode != ENTRY_INVALID) {
                SHR_IF_ERR_EXIT
                    (bcmtm_scheduler_node_set(unit, ltid, lport,
                                              node_id, level, profile,
                                              &scheduler_cfg));
                SHR_IF_ERR_EXIT
                    (bcmtm_scheduler_node_imm_update(unit, ltid, lport,
                                                     level, node_id,
                                                     &scheduler_cfg));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_node_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    /*! Inmemory callback functions for TM_SCHEDULER_NODEt logical table. */
    static bcmimm_lt_cb_t bcmtm_scheduler_node_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_scheduler_node_stage,
    };
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_SCHEDULER_NODEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /*! Registers callback functions for Scheduler node. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_SCHEDULER_NODEt,
                             &bcmtm_scheduler_node_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_scheduler_node_id_validate(int unit,
                                 bcmtm_lport_t lport,
                                 uint32_t level,
                                 uint32_t node_id,
                                 bcmtm_scheduler_profile_t *profile)
{
    int num_ucq, num_mcq;
    SHR_FUNC_ENTER(unit);

    switch (level) {
        case L0_SCHED_COS:
            if ((profile[node_id].opcode == ENTRY_INVALID) ||
                (profile[node_id].num_ucq + profile[node_id].num_mcq == 0)) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case L1_SCHED_UNICAST_QUEUE:
            SHR_IF_ERR_EXIT
                (bcmtm_lport_num_ucq_get(unit, lport, &num_ucq));
            if (node_id >= (uint32_t)num_ucq) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        case L1_SCHED_MULTICAST_QUEUE:
            SHR_IF_ERR_EXIT
                (bcmtm_lport_num_mcq_get(unit, lport, &num_mcq));
            if (node_id >= (uint32_t)num_mcq) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}
