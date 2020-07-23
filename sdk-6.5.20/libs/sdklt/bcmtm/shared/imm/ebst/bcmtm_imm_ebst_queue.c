/*! \file bcmtm_imm_ebst_queue.c
 *
 * BCMTM EBST interface to in-memory table for queues.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/ebst/bcmtm_ebst_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>


/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Get the EBST mappings for TM_EBST_DATA table for queues.
 *
 * This returns the queue map for queue types used for populating data table.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] q_id Queue ID. (unicast queue ID or multicast queue ID).
 * \param [in] q_type Queue type.
 * \param [out] q_map Queue mappings for the port and queue ID.
 *
 * \retval SHR_E_INIT EBST driver function not initialized.
 * \retval SHR_E_NOT_FOUND Queue mapping not found for the port and queue ID.
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_ebst_q_map_get(int unit,
                     uint16_t lport,
                     uint16_t q_id,
                     bcmtm_q_type_t q_type,
                     bcmtm_ebst_entity_map_t **q_map)
{
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_drv_t ebst_drv;
    bcmtm_drv_t *drv;
    uint16_t  map_idx;

    SHR_FUNC_ENTER(unit);

    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->ebst_drv_get, SHR_E_INIT);
    SHR_IF_ERR_EXIT(drv->ebst_drv_get(unit, &ebst_drv));

    SHR_IF_ERR_EXIT
        (ebst_drv.ebst_q_map_index_get(unit, lport, q_id, q_type, &map_idx));

    SHR_NULL_CHECK(ebst_info->map_info.q_map, SHR_E_NOT_FOUND);
    *q_map = &(ebst_info->map_info.q_map[map_idx]);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief parsing EBST unicast queue configuration.
 *
 * \param [in] unit Logical unit number.
 * \param [in] data Field list data from application.
 * \param [out] q_cfg Queue configuration.
 */
static void
bcmtm_ebst_ucq_parse(int unit,
                     const bcmltd_field_t *data,
                     bcmtm_ebst_q_cfg_t *q_cfg)
{
    const bcmltd_field_t *cur_data = data;
    uint64_t fval;

    q_cfg->q_type = UC_Q;
    while (cur_data) {
        fval = cur_data->data;
        switch (cur_data->id) {
            case TM_EBST_UC_Qt_MONITORf:
                q_cfg->enable = fval;
                break;
            case TM_EBST_UC_Qt_BASE_INDEXf:
                q_cfg->q_map.base_id = fval;
                break;
            case TM_EBST_UC_Qt_NUM_ENTRIESf:
                q_cfg->q_map.num_entry = fval;
                break;
            case TM_EBST_UC_Qt_OPERATIONAL_STATEf:
                q_cfg->opcode = fval;
                break;
            default:
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief TM_EBST_UCQ imm lookup and parsing the data.
 *
 * Gets the value programmed by application from IMM and parse the data to
 * populate q_cfg.
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] ucq_id Unicast queue ID.
 * \param [in] q_cfg Queue configuration.
 *
 * \retval SHR_E_PARAM Error in parameters.
 * \retval SHR_E_NOT_FOUND Field not found in the list.
 * \retval SHR_E_MEMORY Error in allocation of field list.
 * \retval SHR_E_NONE No error.
 */
static int
bcmtm_ebst_ucq_imm_lkup(int unit,
                        bcmltd_sid_t ltid,
                        bcmtm_lport_t lport,
                        uint16_t ucq_id,
                        bcmtm_ebst_q_cfg_t *q_cfg)
{
    bcmltd_fields_t out, in;
    bcmltd_fid_t fid;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    /* Get the number of buffer pool supported for the device. */
    fid = TM_EBST_UC_Qt_TM_EBST_DATA_IDf;
    num_fid = TM_EBST_UC_Qt_FIELD_COUNT +
              bcmlrd_field_idx_count_get(unit, ltid, fid) - 1;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    /* Number of keys : 2*/
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));
    in.count = 0;

    /* TM_EBST_UC_Qt_PORT_IDf */
    fid = TM_EBST_UC_Qt_PORT_IDf;
    SHR_IF_ERR_EXIT(bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_EBST_UC_Qt_TM_UC_Q_IDf */
    fid = TM_EBST_UC_Qt_TM_UC_Q_IDf;
    SHR_IF_ERR_EXIT(bcmtm_field_list_set(unit, &in, fid, 0, ucq_id));

    /* IMM lookup. */
    if (SHR_FAILURE(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        q_cfg->opcode = ENTRY_INVALID;
    } else {
        bcmtm_ebst_ucq_parse(unit, *(out.field), q_cfg);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief EBST queue configuration.
 * Validates the queue configuration to update the operation status of the
 * configuration and configures the physical table.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port.
 * \param [in] queue_id Queue ID.
 * \param [in] queue_cfg Queue configuration.
 *
 * \retval SHR_E_INIT EBST driver not initialized.
 * \retval SHR_E_NONE No error.
 */
static int
bcmtm_ebst_q_config(int unit,
                        bcmltd_sid_t ltid,
                        bcmtm_lport_t lport,
                        uint16_t q_id,
                        bcmtm_ebst_q_cfg_t *q_cfg)
{
    int pport, num_q;
    bcmtm_ebst_drv_t ebst_drv;
    bcmtm_drv_t *drv;
    bcmtm_ebst_dev_info_t *ebst_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->ebst_drv_get, SHR_E_INIT);
    SHR_IF_ERR_EXIT(drv->ebst_drv_get(unit, &ebst_drv));

    bcmtm_ebst_dev_info_get(unit, &ebst_info);

    switch (q_cfg->action) {
        case ACTION_INSERT:
        case ACTION_DELETE:
            /* check for logical to physical port mapping */
            if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
                q_cfg->opcode = EBST_OPER_PORT_INFO_UNAVAILABLE;
                SHR_EXIT();
            }
            /* Validate queue ID for the port. */
            if (q_cfg->q_type == UC_Q) {
                /* unicast queue */
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_num_ucq_get(unit, lport, &num_q));
            } else if (q_cfg->q_type == MC_Q) {
                /* multicast queue */
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_num_mcq_get(unit, lport, &num_q));
            }
            if (q_id >= (uint32_t)num_q) {
                q_cfg->opcode = EBST_OPER_Q_ID_INVALID;
                SHR_EXIT();
            }
            break;
        case ACTION_PORT_UPDATE_INT:
            q_cfg->opcode = EBST_OPER_VALID;
            break;
        case ACTION_PORT_DELETE_INT:
            q_cfg->opcode = EBST_OPER_PORT_INFO_UNAVAILABLE;
            q_cfg->enable = 0;
            break;
    }
    /* Update queue configuration in physical tables. */
    SHR_IF_ERR_EXIT
        (ebst_drv.ebst_q_config_set(unit, ltid, lport, q_id, q_cfg));
    q_cfg->enable ? (ebst_info->monitor++): (ebst_info->monitor--);
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief EBST UC queue IMM table change callback function for staging.
 *
 * Handle EBST UC queue IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid This is the logical table ID.
 * \param [in] trans_id Transaction ID.
 * \param [in] event_reason This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] output_fields Read only fields update to imm.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL BST fails to handle LT change events.
 */
static int
bcmtm_ebst_ucq_imm_stage_cb(int unit,
                            bcmltd_sid_t ltid,
                            uint32_t trans_id,
                            bcmimm_entry_event_t event_reason,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            void *context,
                            bcmltd_fields_t *output_fields)
{
    bcmltd_fid_t fid;
    uint64_t fval;
    uint16_t lport, q_id;
    bcmtm_ebst_q_cfg_t q_cfg;
    bcmtm_ebst_drv_t ebst_drv;
    bcmtm_drv_t *drv;
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_entity_map_t *q_map;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->ebst_drv_get, SHR_E_INIT);
    SHR_IF_ERR_EXIT(drv->ebst_drv_get(unit, &ebst_drv));

    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    sal_memset(&q_cfg, 0, sizeof(bcmtm_ebst_q_cfg_t));

    /* TM_EBST_UC_Qt_PORT_IDf */
    fid = TM_EBST_UC_Qt_PORT_IDf;
    SHR_IF_ERR_EXIT(bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (uint16_t) fval;

    /* TM_EBST_UC_Qt_TM_UC_Q_IDf */
    fid = TM_EBST_UC_Qt_TM_UC_Q_IDf;
    SHR_IF_ERR_EXIT(bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    q_id = (uint16_t) fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_ucq_imm_lkup(unit, ltid, lport, q_id, &q_cfg));
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_INTERNAL);
            output_fields->count = 0;
            q_cfg.action = ACTION_INSERT;
            bcmtm_ebst_ucq_parse(unit, data, &q_cfg);
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_q_config(unit, ltid, lport, q_id, &q_cfg));
            fid = TM_EBST_UC_Qt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields, fid, 0, q_cfg.opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            q_cfg.action = ACTION_DELETE;
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_q_config(unit, ltid, lport, q_id, &q_cfg));
            break;
        default:
            break;
    }
    /* Update the queue map for the port and queue ID. */
    if (SHR_SUCCESS(bcmtm_ebst_q_map_get(unit, lport, q_id, UC_Q, &q_map))) {
        *q_map = q_cfg.q_map;
        sal_memset(q_map->write_ptr, -1, sizeof(int) * MAX_BUFFER_POOL);
        q_map->max_id = q_map->base_id + q_map->num_entry - 1;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_EBST_UC_Q imm lookup handler.
 *
 * This populates fields manipulated by hardware and software not having a
 * updated copy in in-memory.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] trans_id Transaction ID.
 * \param [in] context Is a pointer that was given during registration.
 * \param [in] lkup_type Lookup type.
 * \param [in] in This is a linked list of the in fields in the
 * modified entry.
 * \param [out] out This is a linked list of the out fields in the
 * modified entry.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_ebst_ucq_imm_lookup(int unit,
                          bcmltd_sid_t ltid,
                          uint32_t trans_id,
                          void *context,
                          bcmimm_lookup_type_t lkup_type,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out)
{
    bcmltd_fid_t fid;
    uint64_t fval;
    bcmtm_lport_t lport;
    uint8_t q_id, idx;
    const bcmltd_fields_t *key_fields;
    bcmtm_ebst_entity_map_t *q_map;
    uint16_t num_buffer_pool;

    SHR_FUNC_ENTER(unit);

    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* TM_EBST_UC_Qt_PORT_IDf */
    fid = TM_EBST_UC_Qt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, (bcmltd_fields_t *)key_fields,
                              fid, 0, &fval));
    lport = (bcmtm_lport_t)fval;

    /* TM_EBST_UC_Qt_TM_UC_Q_IDf */
    fid = TM_EBST_UC_Qt_TM_UC_Q_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, (bcmltd_fields_t *)key_fields,
                              fid, 0, &fval));
    q_id = (uint8_t)fval;

    /* Get the queue map from local mapping. */
    if (SHR_SUCCESS(bcmtm_ebst_q_map_get(unit, lport, q_id, UC_Q, &q_map))) {
        fid = TM_EBST_UC_Qt_TM_EBST_DATA_IDf;
        num_buffer_pool = bcmlrd_field_idx_count_get(unit, ltid, fid);
        for (idx = 0; idx < num_buffer_pool; idx++) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, out, fid, idx,
                                      q_map->write_ptr[idx]));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_EBST_UC_Q in-memory update.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logiacl table ID.
 * \param [in] lport Logical port.
 * \param [in] q_id Unicast queue ID.
 * \param [in] q_cfg Queue configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Error in parameters.
 * \retval SHR_E_NOT_FOUND Field not found.
 */
static int
bcmtm_ebst_ucq_imm_update(int unit,
                          bcmltd_sid_t ltid,
                          bcmtm_lport_t lport,
                          uint16_t qid,
                          bcmtm_ebst_q_cfg_t *q_cfg)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 3, &in));
    in.count = 0;

    /* TM_EBST_UC_Qt_PORT_IDf */
    fid = TM_EBST_UC_Qt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_EBST_UC_Qt_TM_UC_Q_IDf */
    fid = TM_EBST_UC_Qt_TM_UC_Q_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, qid));

    /* TM_EBST_UC_Qt_OPERATIONAL_STATEf */
    fid = TM_EBST_UC_Qt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, q_cfg->opcode));

    /* updating the entry for TM_EBST_UC_Q */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief parsing EBST unicast queue configuration.
 *
 * \param [in] unit Logical unit number.
 * \param [in] data Field list data from application.
 * \param [out] q_cfg Queue configuration.
 */
static void
bcmtm_ebst_mcq_parse(int unit,
                     const bcmltd_field_t *data,
                     bcmtm_ebst_q_cfg_t *q_cfg)
{
    const bcmltd_field_t *cur_data = data;
    uint64_t fval;

    q_cfg->q_type = MC_Q;
    while (cur_data) {
        fval = cur_data->data;
        switch (cur_data->id) {
            case TM_EBST_MC_Qt_MONITORf:
                q_cfg->enable = fval;
                break;
            case TM_EBST_MC_Qt_BASE_INDEXf:
                q_cfg->q_map.base_id = fval;
                break;
            case TM_EBST_MC_Qt_NUM_ENTRIESf:
                q_cfg->q_map.num_entry = fval;
                break;
            case TM_EBST_MC_Qt_OPERATIONAL_STATEf:
                q_cfg->opcode = fval;
                break;
            default:
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief TM_EBST_MC_Q in-memory lookup.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] q_id Multicast queue ID.
 * \param [out] q_cfg Queue configuration from in-memory lookup.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_ebst_mcq_imm_lkup(int unit,
                        bcmltd_sid_t ltid,
                        bcmtm_lport_t lport,
                        uint16_t mcq_id,
                        bcmtm_ebst_q_cfg_t *q_cfg)
{
    bcmltd_fields_t out, in;
    bcmltd_fid_t fid;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    fid = TM_EBST_MC_Qt_TM_EBST_DATA_IDf;
    /* Get the number of buffer pool supported for the device. */
    num_fid = TM_EBST_MC_Qt_FIELD_COUNT +
              bcmlrd_field_idx_count_get(unit, ltid, fid);
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));
    in.count = 0;

    /*! TM_EBST_MC_Qt_PORT_IDf */
    fid = TM_EBST_MC_Qt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /*! TM_EBST_MC_Qt_TM_MC_Q_IDf */
    fid = TM_EBST_MC_Qt_TM_MC_Q_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, mcq_id));

    if (SHR_FAILURE(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        q_cfg->opcode = ENTRY_INVALID;
    } else {
        bcmtm_ebst_mcq_parse(unit, *(out.field), q_cfg);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_EBST_MC_Q in-memory update.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logiacl table ID.
 * \param [in] lport Logical port.
 * \param [in] q_id Queue ID.
 * \param [in] q_cfg Queue configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Error in parameters.
 * \retval SHR_E_NOT_FOUND Field not found.
 */
static int
bcmtm_ebst_mcq_imm_update(int unit,
                          bcmltd_sid_t ltid,
                          bcmtm_lport_t lport,
                          uint16_t qid,
                          bcmtm_ebst_q_cfg_t *q_cfg)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 3, &in));
    in.count = 0;

    /* TM_EBST_MC_Qt_PORT_IDf */
    fid = TM_EBST_MC_Qt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_EBST_MC_Qt_TM_MC_Q_IDf */
    fid = TM_EBST_MC_Qt_TM_MC_Q_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, qid));

    /* TM_EBST_MC_Qt_OPERATIONAL_STATEf */
    fid = TM_EBST_MC_Qt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, q_cfg->opcode));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EBST MC queue IMM table change callback function for staging.
 *
 * Handle EBST MC queue IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid This is the logical table ID.
 * \param [in] trans_id Transaction ID.
 * \param [in] event_reason This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] output_fields Read only fields update to imm.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL EBST fails to handle LT change events.
 */
static int
bcmtm_ebst_mcq_imm_stage_cb(int unit,
                            bcmltd_sid_t ltid,
                            uint32_t trans_id,
                            bcmimm_entry_event_t event_reason,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            void *context,
                            bcmltd_fields_t *output_fields)
{
    bcmltd_fid_t fid;
    uint64_t fval;
    uint16_t lport, q_id;
    bcmtm_ebst_q_cfg_t q_cfg;
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_entity_map_t *q_map;

    SHR_FUNC_ENTER(unit);


    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    sal_memset(&q_cfg, 0, sizeof(bcmtm_ebst_q_cfg_t));

    /* TM_EBST_MC_Qt_PORT_IDf */
    fid = TM_EBST_MC_Qt_PORT_IDf;
    SHR_IF_ERR_EXIT(bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (uint16_t) fval;

    /* TM_EBST_MC_Qt_TM_MC_Q_IDf */
    fid = TM_EBST_MC_Qt_TM_MC_Q_IDf;
    SHR_IF_ERR_EXIT(bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    q_id = (uint16_t) fval;


    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_mcq_imm_lkup(unit, ltid, lport, q_id, &q_cfg));
        case BCMIMM_ENTRY_INSERT:
            q_cfg.action = ACTION_INSERT;
            SHR_NULL_CHECK(output_fields, SHR_E_INTERNAL);
            output_fields->count = 0;
            bcmtm_ebst_mcq_parse(unit, data, &q_cfg);
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_q_config(unit, ltid, lport, q_id, &q_cfg));
            fid = TM_EBST_MC_Qt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields, fid, 0, q_cfg.opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            q_cfg.action = ACTION_DELETE;
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_q_config(unit, ltid, lport, q_id, &q_cfg));
            break;
        default:
            break;
    }
    if (SHR_SUCCESS(bcmtm_ebst_q_map_get(unit, lport, q_id, MC_Q, &q_map))) {
        *q_map = q_cfg.q_map;
        sal_memset(q_map->write_ptr, -1, sizeof(int) * MAX_BUFFER_POOL);
        q_map->max_id = q_map->base_id + q_map->num_entry - 1;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_EBST_MC_Q imm stage lookup handler.
 *
 * This populates fields manipulated by hardware and software not having a
 * updated copy in in-memory.
 *
 * \param [in] unit Logical unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id Transaction ID.
 * \param [in] context Is a pointer that was given during registration.
 * \param [in] lkup_type Lookup type.
 * \param [in] in This is a linked list of the in fields in the
 * modified entry.
 * \param [out] out This is a linked list of the out fields in the
 * modified entry.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_ebst_mcq_imm_lookup(int unit,
                          bcmltd_sid_t ltid,
                          uint32_t trans_id,
                          void *context,
                          bcmimm_lookup_type_t lkup_type,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out)
{
    bcmltd_fid_t fid;
    uint64_t fval;
    bcmtm_drv_info_t *drv_info;
    bcmtm_lport_t lport;
    uint8_t q_id, idx;
    const bcmltd_fields_t *key_fields;
    bcmtm_ebst_entity_map_t *q_map;
    uint16_t num_buffer_pool;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* TM_EBST_MC_Qt_PORT_IDf */
    fid = TM_EBST_MC_Qt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, (bcmltd_fields_t *)key_fields,
                              fid, 0, &fval));
    lport = (bcmtm_lport_t)fval;

    /* TM_EBST_MC_Qt_TM_MC_Q_IDf */
    fid = TM_EBST_MC_Qt_TM_MC_Q_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, (bcmltd_fields_t *)key_fields,
                              fid, 0, &fval));
    q_id = (uint8_t)fval;

    if (SHR_SUCCESS(bcmtm_ebst_q_map_get(unit, lport, q_id, MC_Q, &q_map))) {
        fid = TM_EBST_MC_Qt_TM_EBST_DATA_IDf;
        num_buffer_pool = bcmlrd_field_idx_count_get(unit, ltid, fid);
        for (idx = 0; idx < num_buffer_pool ; idx++) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, out, fid, idx,
                                      q_map->write_ptr[idx]));
        }
    }
exit:
    SHR_FUNC_EXIT();
}



int
bcmtm_imm_ebst_q_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;
    bcmltd_sid_t ltid;
    static bcmimm_lt_cb_t bcmtm_ebst_ucq_imm_cb = {
        /* staging function. */
        .stage = bcmtm_ebst_ucq_imm_stage_cb,
        /* lookup function. */
        .lookup = bcmtm_ebst_ucq_imm_lookup
    };
    static bcmimm_lt_cb_t bcmtm_ebst_mcq_imm_cb = {
        /* staging function. */
        .stage = bcmtm_ebst_mcq_imm_stage_cb,
        /* llokup function. */
        .lookup = bcmtm_ebst_mcq_imm_lookup
    };

    SHR_FUNC_ENTER(unit);

    /* TM_EBST_UC_Qt */
    ltid = TM_EBST_UC_Qt;
    rv = bcmlrd_map_get(unit, ltid, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if (rv == SHR_E_UNAVAIL || (!map)) {
        SHR_EXIT();
    }
    /* Register callback for TM_EBST_UC_Qt */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, ltid, &bcmtm_ebst_ucq_imm_cb, NULL));

    /* TM_EBST_MC_Qt */
    ltid = TM_EBST_MC_Qt;
    rv = bcmlrd_map_get(unit, ltid, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if (rv == SHR_E_UNAVAIL || (!map)) {
        SHR_EXIT();
    }
    /* Register callback for TM_EBST_MC_Qt */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, ltid, &bcmtm_ebst_mcq_imm_cb, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_ebst_q_map_populate(int unit, bcmltd_sid_t ltid)
{
    const bcmlrd_map_t *map = NULL;
    bcmltd_fid_t fid;
    size_t num_fid;
    bcmltd_fields_t in, out;
    int rv;
    bcmtm_lport_t lport = 0;
    uint64_t fval;
    uint16_t q_id = 0;
    bcmtm_ebst_q_cfg_t q_cfg;
    bcmtm_ebst_entity_map_t *q_map;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    sal_memset(&q_cfg, 0, sizeof(bcmtm_ebst_q_cfg_t));
    rv = bcmlrd_map_get(unit, ltid, &map);
    /* Skip if LT doesnot exist */
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));

    /* Number of keys: 2*/
    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, 2, &in));
    SHR_IF_ERR_EXIT(bcmtm_field_list_alloc(unit, num_fid, &out));

    in.count = 0;
    rv = bcmimm_entry_get_first(unit, ltid, &out);
    while (SHR_SUCCESS(rv)) {

        if (ltid == TM_EBST_UC_Qt) {
            fid = TM_EBST_UC_Qt_PORT_IDf;
            SHR_IF_ERR_EXIT(bcmtm_field_list_get(unit, &out, fid, 0, &fval));
            SHR_IF_ERR_EXIT(bcmtm_field_list_set(unit, &in, fid, 0, fval));
            lport = (bcmtm_lport_t)fval;

            fid = TM_EBST_UC_Qt_TM_UC_Q_IDf;
            SHR_IF_ERR_EXIT(bcmtm_field_list_get(unit, &out, fid, 0, &fval));
            SHR_IF_ERR_EXIT(bcmtm_field_list_set(unit, &in, fid, 0, fval));
            q_id = (uint16_t) fval;

            bcmtm_ebst_ucq_parse(unit, *(out.field), &q_cfg);
        } else if (ltid == TM_EBST_MC_Qt) {
            fid = TM_EBST_MC_Qt_PORT_IDf;
            SHR_IF_ERR_EXIT(bcmtm_field_list_get(unit, &out, fid, 0, &fval));
            SHR_IF_ERR_EXIT(bcmtm_field_list_set(unit, &in, fid, 0, fval));
            lport = (bcmtm_lport_t)fval;

            fid = TM_EBST_MC_Qt_TM_MC_Q_IDf;
            SHR_IF_ERR_EXIT(bcmtm_field_list_get(unit, &out, fid, 0, &fval));
            SHR_IF_ERR_EXIT(bcmtm_field_list_set(unit, &in, fid, 0, fval));
            q_id = (uint16_t) fval;

            bcmtm_ebst_mcq_parse(unit, *(out.field), &q_cfg);
        }
        if (SHR_SUCCESS
            (bcmtm_ebst_q_map_get(unit, lport, q_id, q_cfg.q_type, &q_map))) {
            *q_map = q_cfg.q_map;
            sal_memset(q_map->write_ptr, -1, sizeof(int) * MAX_BUFFER_POOL);
            q_map->max_id = q_map->base_id + q_map->num_entry - 1;
        }
        out.count = num_fid;
        rv = bcmimm_entry_get_next(unit, ltid, &in, &out);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}


int
bcmtm_ebst_port_q_internal_update(int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t action)
{
    int num_q, q_id;
    bcmtm_ebst_q_cfg_t q_cfg;
    bcmltd_sid_t ltid;
    int rv;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    /* Unicast queue map. */
    ltid = TM_EBST_UC_Qt;
    rv = bcmlrd_map_get(unit, ltid, &map);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    sal_memset(&q_cfg, 0 , sizeof(bcmtm_ebst_q_cfg_t));
    SHR_IF_ERR_EXIT
        (bcmtm_lport_num_ucq_get(unit, lport, &num_q));

    q_cfg.action = action;
    for (q_id = 0; q_id < num_q; q_id++) {
        SHR_IF_ERR_EXIT
            (bcmtm_ebst_ucq_imm_lkup(unit, ltid, lport, q_id, &q_cfg));
        if (q_cfg.opcode != ENTRY_INVALID) {
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_q_config(unit, ltid, lport, q_id, &q_cfg));
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_ucq_imm_update(unit, ltid, lport, q_id, &q_cfg));
        }
    }

    /* Multicast queue map.*/
    ltid = TM_EBST_MC_Qt;
    rv = bcmlrd_map_get(unit, ltid, &map);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    sal_memset(&q_cfg, 0 , sizeof(bcmtm_ebst_q_cfg_t));
    SHR_IF_ERR_EXIT
        (bcmtm_lport_num_mcq_get(unit, lport, &num_q));

    q_cfg.action = action;
    for (q_id = 0; q_id < num_q; q_id++) {
        SHR_IF_ERR_EXIT
            (bcmtm_ebst_mcq_imm_lkup(unit, ltid, lport, q_id, &q_cfg));
        if (q_cfg.opcode != ENTRY_INVALID) {
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_q_config(unit, ltid, lport, q_id, &q_cfg));
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_mcq_imm_update(unit, ltid, lport, q_id, &q_cfg));
        }
    }
exit:
    SHR_FUNC_EXIT();
}
