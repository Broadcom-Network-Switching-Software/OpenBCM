/*! \file bcmtm_imm_shaper_node.c
 *
 * In-memory call back function for shapers for scheduling nodes of
 * non cpu ports.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/sched_shaper/bcmtm_shaper_internal.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>
#include <bcmtm/bcmtm_shaper_encoding_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief  Populates shaper configuration using user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] shaper_cfg Shaper configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static void
bcmtm_shaper_node_cfg_fill(int unit,
                           const bcmltd_field_t *data,
                           bcmtm_shaper_cfg_t *shaper_cfg)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;
    shaper_cfg->burst_auto = 1;

    while (cur_data) {
        switch (cur_data->id) {
            case TM_SHAPER_NODEt_BURST_SIZE_AUTOf:
                shaper_cfg->burst_auto = cur_data->data;
                break;
            case TM_SHAPER_NODEt_MAX_BANDWIDTH_KBPSf:
                shaper_cfg->max_bucket_encode.bandwidth = cur_data->data;
                break;
            case TM_SHAPER_NODEt_MIN_BANDWIDTH_KBPSf:
                shaper_cfg->min_bucket_encode.bandwidth = cur_data->data;
                break;
            case TM_SHAPER_NODEt_MAX_BURST_SIZE_KBITSf:
                shaper_cfg->max_bucket_encode.burst = cur_data->data;
                break;
            case TM_SHAPER_NODEt_MIN_BURST_SIZE_KBITSf:
                shaper_cfg->min_bucket_encode.burst = cur_data->data;
                break;
            case TM_SHAPER_NODEt_SHAPING_MODEf:
                shaper_cfg->shaping_mode = cur_data->data;
                break;
            case TM_SHAPER_NODEt_OPERATIONAL_STATEf:
                shaper_cfg->opcode = cur_data->data;
                break;
            default:
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief Perform IMM lookup for the keys and  populates shaper
 *   configuration with earlier configured values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] level Scheduler level.
 * \param [in] node_id Scheduler node ID.
 * \param [out] shaper_cfg Shaper configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_shaper_node_imm_lkup(int unit,
                           bcmltd_sid_t ltid,
                           bcmtm_lport_t lport,
                           uint32_t level,
                           uint32_t node_id,
                           bcmtm_shaper_cfg_t *shaper_cfg)
{
    bcmltd_fields_t in, out;
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

    /* TM_SHAPER_NODEt_PORT_IDf */
    fid = TM_SHAPER_NODEt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_SHAPER_NODEt_TM_SCHEDULER_NODE_IDf */
    fid = TM_SHAPER_NODEt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, node_id));

    if (SHR_E_NONE != bcmimm_entry_lookup(unit, ltid, &in, &out)) {
        shaper_cfg->opcode = ENTRY_INVALID;
        SHR_EXIT();
    }

    /* update shaper node configuration based on imm lookup. */
    bcmtm_shaper_node_cfg_fill(unit, *(out.field), shaper_cfg);

exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Perform IMM update for the keys and  populates shaper
 *   configuration with earlier configured values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] level Scheduler level.
 * \param [in] node_id Scheduler node ID.
 * \param [out] shaper_cfg Shaper configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_shaper_node_imm_update(int unit,
                             bcmltd_sid_t ltid,
                             bcmtm_lport_t lport,
                             uint32_t level,
                             uint32_t node_id,
                             bcmtm_shaper_cfg_t *shaper_cfg)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 8, &in));

    in.count = 0;

    /* TM_SHAPER_NODEt_PORT_IDf */
    fid = TM_SHAPER_NODEt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_SHAPER_NODEt_TM_SCHEDULER_NODE_IDf */
    fid = TM_SHAPER_NODEt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, node_id));

    /* TM_SHAPER_NODEt_SCHED_NODEf */
    fid = TM_SHAPER_NODEt_SCHED_NODEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, level));


    /* TM_SHAPER_NODEt_OPERATIONAL_STATEf */
    fid = TM_SHAPER_NODEt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, shaper_cfg->opcode));

    if ((shaper_cfg->action == ACTION_PROFILE_UPDATE_INT ||
         shaper_cfg->action == ACTION_PORT_UPDATE_INT)) {

        /* TM_SHAPER_NODEt_MAX_BANDWIDTH_KBPS_OPERf */
        fid = TM_SHAPER_NODEt_MAX_BANDWIDTH_KBPS_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in, fid, 0,
                                  shaper_cfg->max_bucket_encode.bandwidth));

        /* TM_SHAPER_NODEt_MAX_BURST_SIZE_KBITS_OPERf */
        fid = TM_SHAPER_NODEt_MAX_BURST_SIZE_KBITS_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in, fid, 0,
                                  shaper_cfg->max_bucket_encode.burst));

        /* TM_SHAPER_NODEt_MIN_BANDWIDTH_KBPS_OPERf */
        fid = TM_SHAPER_NODEt_MIN_BANDWIDTH_KBPS_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in, fid, 0,
                                  shaper_cfg->min_bucket_encode.bandwidth));

        /* TM_SHAPER_NODEt_MIN_BURST_SIZE_KBITS_OPERf */
        fid = TM_SHAPER_NODEt_MIN_BURST_SIZE_KBITS_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in, fid, 0,
                                  shaper_cfg->min_bucket_encode.burst));
    }

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
 * \param [in] shaper_cfg Shaper configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_shaper_node_pt_set(int unit,
                         bcmtm_lport_t lport,
                         uint32_t node_id,
                         bcmltd_sid_t ltid,
                         bcmtm_shaper_cfg_t *shaper_cfg)
{
    bcmtm_drv_t *drv;
    bcmtm_shaper_drv_t shaper_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&shaper_drv, 0, sizeof(bcmtm_shaper_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->shaper_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->shaper_drv_get(unit, &shaper_drv));

    /* convert application defined to hardware values */
    if (shaper_cfg->action != ACTION_DELETE) {
        /* Update is needed only if values are to be programmed to hardware. */
        SHR_NULL_CHECK(shaper_drv.rate_to_bucket, SHR_E_INTERNAL);

        SHR_IF_ERR_EXIT
            (shaper_drv.rate_to_bucket(unit, lport,
                                       shaper_cfg->shaping_mode,
                                       shaper_cfg->burst_auto,
                                       &(shaper_cfg->min_bucket_encode)));
        SHR_IF_ERR_EXIT
            (shaper_drv.rate_to_bucket(unit, lport,
                                       shaper_cfg->shaping_mode,
                                       shaper_cfg->burst_auto,
                                       &(shaper_cfg->max_bucket_encode)));
    }
    /* update physical table. */
    if (shaper_drv.node_set) {
        SHR_IF_ERR_EXIT
            (shaper_drv.node_set(unit, lport, 0, node_id, ltid, shaper_cfg));
    }

    if (shaper_cfg->action != ACTION_DELETE) {
        /* Get the actual configured shaper values. */
        SHR_NULL_CHECK(shaper_drv.bucket_to_rate, SHR_E_INTERNAL);
        SHR_IF_ERR_EXIT
            (shaper_drv.bucket_to_rate(unit, shaper_cfg->shaping_mode,
                                       &(shaper_cfg->max_bucket_encode)));
        SHR_IF_ERR_EXIT
            (shaper_drv.bucket_to_rate(unit, shaper_cfg->shaping_mode,
                                       &(shaper_cfg->min_bucket_encode)));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with reset values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] level Scheduler level.
 * \param [in] node_id Scheduler node ID.
 * \param [in] shaper_cfg Shaper configuration.
 * \param [out] output fields Read-only fields.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_shaper_node_reset(int unit,
                        bcmltd_sid_t ltid,
                        bcmtm_lport_t lport,
                        uint32_t level,
                        uint32_t node_id,
                        bcmtm_shaper_cfg_t *shaper_cfg)
{
    uint8_t profile_id, action;
    SHR_FUNC_ENTER(unit);

    /*
     * Physical table will be updated only if opcode is valid.
     */
    if (shaper_cfg->opcode == VALID) {
        profile_id = shaper_cfg->profile_id;
        action = shaper_cfg->action;
        sal_memset(shaper_cfg, 0, sizeof(bcmtm_shaper_cfg_t));
        shaper_cfg->profile_id = profile_id;
        shaper_cfg->action = action;
        /* PT update */
        SHR_IF_ERR_EXIT
            (bcmtm_shaper_node_pt_set(unit, lport, node_id, ltid, shaper_cfg));
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
 * \param [in] level Scheduler level.
 * \param [in] node_id Scheduler node ID.
 * \param [in] shaper_cfg Shaper configuration.
 * \param [out] output fields Read-only fields.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_shaper_node_set(int unit,
                      bcmltd_sid_t ltid,
                      bcmtm_lport_t lport,
                      uint32_t level,
                      uint32_t node_id,
                      bcmtm_scheduler_profile_t *profile,
                      bcmtm_shaper_cfg_t *shaper_cfg,
                      bcmltd_fields_t *output_fields)
{
    bcmltd_fid_t fid;
    int mport;
    uint8_t prof_id, action;

    SHR_FUNC_ENTER(unit);

    action = shaper_cfg->action;

    switch (action) {

        case ACTION_INSERT:
            /* TM local port information.
             * Returns error if local port to physical port map is not available.
             * Updates opeartional state.
             */
            if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
                shaper_cfg->opcode = PORT_INFO_UNAVAIL;
                goto op_stage;
            }
            /* Fall thorugh */
        case ACTION_PORT_UPDATE_INT:
            if (SHR_FAILURE
                    (bcmtm_scheduler_port_profile_get(unit, lport, &prof_id))) {
                shaper_cfg->opcode = SCHED_PROFILE_INVALID;
                goto op_stage;
            }
            sal_memset(profile, 0,
                sizeof(bcmtm_scheduler_profile_t) * MAX_TM_SCHEDULER_NODE);
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_profile_get(unit, prof_id, profile));
            shaper_cfg->profile_id = prof_id;
            if (SHR_FAILURE
                    (bcmtm_scheduler_node_id_validate(unit, lport, L0_SCHED_COS,
                                                      node_id, profile))) {
                shaper_cfg->opcode = SCHED_NODE_ID_INVALID;
                goto op_stage;
            }
            shaper_cfg->opcode = VALID;
            break;
        case ACTION_PORT_DELETE_INT:
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_node_reset(unit, ltid, lport, level,
                                         node_id, shaper_cfg));
            shaper_cfg->opcode = PORT_INFO_UNAVAIL;
            SHR_EXIT();
            break;
        case ACTION_PROFILE_DELETE_INT:
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_node_reset(unit, ltid, lport, level,
                                         node_id, shaper_cfg));
            shaper_cfg->opcode = SCHED_PROFILE_INVALID;
            SHR_EXIT();
            break;
        case ACTION_PROFILE_UPDATE_INT:
        case ACTION_PROFILE_UPDATE:
            if (shaper_cfg->opcode == SCHED_PROFILE_INVALID) {
                shaper_cfg->opcode = VALID;
            }
            if (SHR_FAILURE
                    (bcmtm_scheduler_node_id_validate(unit, lport, L0_SCHED_COS,
                                                      node_id, profile))) {
                shaper_cfg->opcode = SCHED_NODE_ID_INVALID;
                SHR_EXIT();
            }
            break;
        default:
            break;
    }

    /* PT update */
    SHR_IF_ERR_EXIT
        (bcmtm_shaper_node_pt_set(unit, lport, node_id, ltid, shaper_cfg));

    /* Update read only fields */

    if ((action == ACTION_PROFILE_UPDATE_INT) ||
        (action == ACTION_PORT_UPDATE_INT)) {
        SHR_EXIT();
    }

    if (output_fields) {
        /* update the readonly fields only if output_fields is valid.
         * 1. TM_SHAPER_NODEt_MAX_BANDWIDTH_KBPS_OPERf
         * 2. TM_SHAPER_NODEt_MAX_BURST_KBITS_OPERf
         * 3. TM_SHAPER_NODEt_MIN_BANDWIDTH_KBPS_OPERf
         * 4. TM_SHAPER_NODEt_MIN_BURST_KBITS_OPERf
         * 5. TM_SHAPER_NODEt_OPERATIONAL_STATEf
         */
        fid = TM_SHAPER_NODEt_MAX_BANDWIDTH_KBPS_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields, fid, 0,
                                  shaper_cfg->max_bucket_encode.bandwidth));

        fid = TM_SHAPER_NODEt_MAX_BURST_SIZE_KBITS_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields, fid, 0,
                                  shaper_cfg->max_bucket_encode.burst));

        fid = TM_SHAPER_NODEt_MIN_BANDWIDTH_KBPS_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields, fid, 0,
                                  shaper_cfg->min_bucket_encode.bandwidth));

        fid = TM_SHAPER_NODEt_MIN_BURST_SIZE_KBITS_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields, fid, 0,
                                  shaper_cfg->min_bucket_encode.burst));
    }
op_stage:
    if (output_fields) {
        fid = TM_SHAPER_NODEt_OPERATIONAL_STATEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields, fid, 0, shaper_cfg->opcode));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_SHAPER_NODE logical table callback function for staging.
 *
 * Handle TM_SHAPER_NODE IMM table change events.
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
bcmtm_shaper_node_stage(int unit,
                        bcmltd_sid_t ltid,
                        uint32_t trans_id,
                        bcmimm_entry_event_t event_reason,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        void *context,
                        bcmltd_fields_t *output_fields)
{
    uint64_t fval;
    bcmtm_shaper_cfg_t shaper_cfg;
    bcmltd_fid_t fid;
    bcmtm_lport_t lport;
    uint32_t node_id, level = 0;
    uint8_t profile_id;
    bcmtm_scheduler_profile_t profile[MAX_TM_SCHEDULER_NODE];

    SHR_FUNC_ENTER(unit);
    sal_memset(&shaper_cfg, 0, sizeof(bcmtm_shaper_cfg_t));

    /* TM_SHAPER_NODEt_PORT_IDf */
    fid = TM_SHAPER_NODEt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    /* TM_SHAPER_NODEt_TM_SCHEDULER_NODE_IDf */
    fid = TM_SHAPER_NODEt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    node_id = (uint32_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_node_imm_lkup(unit, ltid, lport, level,
                                            node_id, &shaper_cfg));
        /* Fall through */
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            bcmtm_shaper_node_cfg_fill(unit, data, &shaper_cfg);
            shaper_cfg.action = ACTION_INSERT;
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_node_set(unit, ltid, lport, level,
                                       node_id, profile, &shaper_cfg,
                                       output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_node_imm_lkup(unit, ltid, lport, level,
                                            node_id, &shaper_cfg));
            if (shaper_cfg.opcode == VALID) {
                SHR_IF_ERR_EXIT
                    (bcmtm_scheduler_port_profile_get(unit, lport, &profile_id));
                shaper_cfg.profile_id = profile_id;
                SHR_IF_ERR_EXIT
                    (bcmtm_shaper_node_reset(unit, ltid, lport, level,
                                             node_id, &shaper_cfg));
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
bcmtm_shaper_node_internal_update(int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t profile_id,
                                  bcmtm_scheduler_profile_t *profile,
                                  uint8_t action)
{
    uint32_t node_id, level = L0_SCHED_COS;
    uint64_t node_min, node_max;
    bcmltd_fid_t fid;
    bcmltd_sid_t ltid = TM_SHAPER_NODEt;
    bcmtm_shaper_cfg_t shaper_cfg;

    SHR_FUNC_ENTER(unit);

    sal_memset(&shaper_cfg, 0, sizeof(bcmtm_shaper_cfg_t));

    fid = TM_SHAPER_NODEt_TM_SCHEDULER_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_value_range_get(unit, ltid, fid, &node_min, &node_max));

    shaper_cfg.profile_id = profile_id;
    shaper_cfg.action = action;

    for (node_id = node_min; node_id <= node_max; node_id++) {
        SHR_IF_ERR_EXIT
            (bcmtm_shaper_node_imm_lkup(unit, ltid, lport, level,
                                        node_id, &shaper_cfg));
        if (shaper_cfg.opcode != ENTRY_INVALID) {
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_node_set(unit, ltid, lport, level,
                                       node_id, profile, &shaper_cfg, NULL));
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_node_imm_update(unit, ltid, lport, level,
                                              node_id, &shaper_cfg));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_shaper_node_imm_register(int unit)
{
    /*! Inmemory callback functions for TM_SHAPER_NODEt logical table. */
    bcmimm_lt_cb_t bcmtm_shaper_node_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_shaper_node_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_SHAPER_NODEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for Shaper node. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_SHAPER_NODEt,
                             &bcmtm_shaper_node_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
