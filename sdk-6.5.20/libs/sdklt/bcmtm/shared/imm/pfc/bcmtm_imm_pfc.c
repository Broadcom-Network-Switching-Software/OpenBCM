/*! \file bcmtm_imm_pfc.c
 *
 * In-memory call back function for PFC configurations.
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
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmtm/pfc/bcmtm_pfc_deadlock_internal.h>
#include <bcmltd/bcmltd_bitop.h>

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief  PFC enable physical table ingress configuration based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port number.
 * \param [in] pfc_enable Enable/disable PFC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_pfc_port_egr_pt_set(int unit,
                          bcmltd_sid_t ltid,
                          bcmtm_lport_t lport,
                          uint32_t pfc_enable,
                          uint32_t profile_id)
{
    bcmtm_drv_t *drv;
    bcmtm_pfc_drv_t *pfc_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    pfc_drv =  drv->pfc_drv_get(unit);

    SHR_NULL_CHECK(pfc_drv, SHR_E_PARAM);

    /* update physical table. */
    if (pfc_drv->egr_port_set) {
        SHR_IF_ERR_EXIT
            (pfc_drv->egr_port_set(unit, ltid, lport,
                                   pfc_enable, profile_id));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Resets the PFC Egress port physical table to default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_pfc_port_egr_reset(int unit,
                         bcmltd_sid_t ltid,
                         bcmtm_lport_t lport)
{
    int mport;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
        SHR_EXIT();
    }
    /* Physical table update. */
    SHR_IF_ERR_EXIT
        (bcmtm_pfc_port_egr_pt_set(unit, ltid, lport, 0, 0));
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Updates IMM for PFC Egress LT.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] opcode Operational state to be updated to IMM.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_pfc_egr_port_imm_update(int unit,
                              bcmltd_sid_t ltid,
                              bcmtm_lport_t lport,
                              uint32_t pfc,
                              uint32_t profile_id,
                              uint8_t opcode)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;
    uint32_t in_alloc = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 4, &in));
    in_alloc = 1;
    in.count = 0;

    /* TM_PFC_EGRt_PORT_IDf */
    fid =TM_PFC_EGRt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_PFC_EGRt_BLOCK_PFC_QUEUE_UPDATESf */
    fid =TM_PFC_EGRt_BLOCK_PFC_QUEUE_UPDATESf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, pfc));

    /* TM_PFC_EGRt_TM_PFC_PRI_PROFILE_IDf */
    fid = TM_PFC_EGRt_TM_PFC_PRI_PROFILE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, profile_id));

    /* TM_PFC_EGRt_OPERATIONAL_STATEf */
    fid = TM_PFC_EGRt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, opcode));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
exit:
    if (in_alloc) {
        bcmtm_field_list_free(&in);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the PFC physical tables with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] pfc_enable PFC enable/disable.
 * \param [in] action Action to be performed.
 * \param [out] opcode Operational status code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_pfc_egr_port_set(int unit,
                       bcmltd_sid_t ltid,
                       bcmtm_lport_t lport,
                       uint32_t pfc_enable,
                       uint32_t profile_id,
                       uint8_t action,
                       uint8_t *opcode)
{
    int mport;
    bcmtm_drv_t *drv;
    bcmtm_pfc_drv_t *pfc_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    pfc_drv =  drv->pfc_drv_get(unit);

    SHR_NULL_CHECK(pfc_drv, SHR_E_PARAM);

    switch (action) {
        case ACTION_INSERT:
            /* TM local port information.
             * Returns error if local port to physical port map is not available.
             * Updates opeartional state.
             */
            if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
                *opcode = PORT_INFO_UNAVAIL;
                SHR_EXIT();
            }
        /* Fall through */
        case ACTION_PORT_UPDATE_INT:
            /* PT update */
            SHR_IF_ERR_EXIT
                (bcmtm_pfc_port_egr_pt_set(unit, ltid, lport, pfc_enable,
                                           profile_id));
            /* opcode success */
            *opcode = VALID;
            break;

        case ACTION_PORT_DELETE_INT:
            SHR_IF_ERR_EXIT
                (bcmtm_pfc_port_egr_reset(unit, ltid, lport));
            *opcode = PORT_INFO_UNAVAIL;
            break;

        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PFC egress LT imm lookup.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [out] pfc_enable PFC enable state.
 * \param [out] profile_id PFC priority profile id.
 * \param [out] opcode operational state code.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_pfc_egr_port_imm_lkup(int unit,
                            bcmltd_sid_t ltid,
                            bcmtm_lport_t lport,
                            uint32_t *pfc_enable,
                            uint32_t *profile_id,
                            uint8_t *opcode)
{
    bcmltd_fields_t out, in;
    bcmltd_fid_t fid;
    size_t num_fid;
    uint64_t fval;
    uint8_t in_alloc = 0, out_alloc = 0;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    *pfc_enable = 0;
    *profile_id = 0;

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));
    out_alloc = 1;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));
    in_alloc = 1;
    in.count = 0;

    fid = TM_PFC_EGRt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    if (SHR_FAILURE(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        *opcode = ENTRY_INVALID;
    } else {
        /* TM_PFC_EGRt_BLOCK_PFC_QUEUE_UPDATESf */
        fid = TM_PFC_EGRt_BLOCK_PFC_QUEUE_UPDATESf;
        if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
            *pfc_enable = (uint32_t)fval;
        }
        /* TM_PFC_EGRt_TM_PFC_PRI_PROFILE_IDf */
        fid = TM_PFC_EGRt_TM_PFC_PRI_PROFILE_IDf;
        if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
            *profile_id = (uint32_t)fval;
        }

        /* TM_PFC_EGRt_OPERATIONAL_STATEf */
        fid = TM_PFC_EGRt_OPERATIONAL_STATEf;
        SHR_IF_ERR_EXIT
            (bcmtm_fval_get_from_field_array(unit, fid, 0, *(out.field), &fval));
        *opcode = (uint8_t)fval;
    }
exit:
    if (in_alloc) {
        bcmtm_field_list_free(&in);
    }
    if (out_alloc) {
        bcmtm_field_list_free(&out);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_PFC_ING logical table callback function for staging.
 *
 * Handle TM_PFC_ING IMM table events.
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
bcmtm_pfc_egr_port_stage(int unit,
                         bcmltd_sid_t ltid,
                         uint32_t trans_id,
                         bcmimm_entry_event_t event_reason,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         void *context,
                         bcmltd_fields_t *output_fields)
{
    bcmtm_lport_t lport;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    uint32_t pfc_enable = 0;
    uint32_t profile_id = 0;
    uint8_t opcode;
    uint8_t action;
    const bcmltd_field_t *data_ptr;
    bcmltd_fields_t out, in;
    bool in_alloc = 0, out_alloc = 0;

    SHR_FUNC_ENTER(unit);

   /* TM_PFC_EGRt_PORT_IDf */
    fid = TM_PFC_EGRt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    if (event_reason == BCMIMM_ENTRY_DELETE) {
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_port_egr_reset(unit, ltid, lport));
        SHR_EXIT();
    } else {
        SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
        if (!data) {
            if (event_reason == BCMIMM_ENTRY_INSERT) {
                /*
                 * Data field not present in the insert command.
                 */
                pfc_enable = 0;
                profile_id = 0;
            } else {
                /*
                 * Data field not present in the update command.
                 * Initialize the entry and return.
                 */
                 output_fields->count = 0;
                 SHR_EXIT();
            }
        } else if (event_reason == BCMIMM_ENTRY_UPDATE) {
            size_t num_fid;
            sal_memset(&in, 0, sizeof(bcmltd_fields_t));
            sal_memset(&out, 0, sizeof(bcmltd_fields_t));
            /* Allocate in field list. */
            SHR_IF_ERR_EXIT
               (bcmtm_field_list_alloc(unit, 1, &in));
            in_alloc = 1;
            /* Allocate out field list. */
            SHR_IF_ERR_EXIT
               (bcmlrd_field_count_get(unit, ltid, &num_fid));
            SHR_IF_ERR_EXIT
               (bcmtm_field_list_alloc(unit, num_fid , &out));
            out_alloc = 1;

            in.field[0]->id   = TM_PFC_EGRt_PORT_IDf;
            in.field[0]->data = lport;

            /* Read the entry if it exists. */
            if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
                size_t idx;
                for (idx = 0; idx < out.count; idx++) {
                    if (out.field[idx]->id == TM_PFC_EGRt_BLOCK_PFC_QUEUE_UPDATESf) {
                        pfc_enable = out.field[idx]->data;
                    } else if (out.field[idx]->id == TM_PFC_EGRt_TM_PFC_PRI_PROFILE_IDf) {
                        profile_id = out.field[idx]->data;
                    }
                }
            }
        }

        SHR_IF_ERR_EXIT
            (bcmtm_field_list_size_check(unit, output_fields, 1));
        output_fields->count = 0;

        /* Parse the data fields. */
        if (data) {
            data_ptr = data;
            while (data_ptr) {
                switch (data_ptr->id) {
                    case TM_PFC_EGRt_BLOCK_PFC_QUEUE_UPDATESf:
                        pfc_enable = data_ptr->data;
                        break;
                    case TM_PFC_EGRt_TM_PFC_PRI_PROFILE_IDf:
                        profile_id = data_ptr->data;
                        break;
                    default:
                        SHR_ERR_EXIT(SHR_E_PARAM);
                }
                data_ptr = data_ptr->next;
            }
        }

        action = ACTION_INSERT;
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_egr_port_set(unit, ltid, lport, pfc_enable,
                               profile_id, action, &opcode));
        fid = TM_PFC_EGRt_OPERATIONAL_STATEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields, fid, 0, opcode));
    }
exit:
    if (in_alloc) {
        bcmtm_field_list_free(&in);
    }
    if (out_alloc) {
        bcmtm_field_list_free(&out);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief  PFC Rx Priority to CoS Map physical table
 *  configuration based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port number.
 * \param [in] pfc_enable Enable/disable PFC.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_pfc_rx_pri_to_cos_map_pt_set(int unit,
                                   int ltid,
                                   int profile_id,
                                   tm_pfc_pri_to_cos_map_config_t map[])
{
    bcmtm_drv_t *drv;
    bcmtm_pfc_drv_t *pfc_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    pfc_drv =  drv->pfc_drv_get(unit);

    SHR_NULL_CHECK(pfc_drv, SHR_E_PARAM);

    /* update physical table. */
    if (pfc_drv->rx_pri_cos_map_set) {
        SHR_IF_ERR_EXIT
            (pfc_drv->rx_pri_cos_map_set(unit,
                                         profile_id, map));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_pfc_rx_map_imm_lkup(int unit, int prof_idx,
                          tm_pfc_pri_to_cos_map_config_t map[])
{
    bcmltd_fields_t out, in;
    bcmltd_sid_t sid = TM_PFC_PRI_PROFILEt;
    tm_pfc_pri_to_cos_map_config_t *map_val;
    int i = 0;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    /* Allocate in field list. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));

    /* Allocate out field list. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, (num_fid + (MAX_NUM_COS - 1)), &out));

    for (i = 0; i < MAX_NUM_PFC_PRI; i++) {
        in.field[0]->id   = TM_PFC_PRI_PROFILEt_TM_PFC_PRI_PROFILE_IDf;
        in.field[0]->data = prof_idx;
        in.field[1]->id   = TM_PFC_PRI_PROFILEt_PFC_PRIf;
        in.field[1]->data = i;
        map_val = &map[i];
        out.count= num_fid + (MAX_NUM_COS - 1);

       /* Read the entry if it exists. */
        if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &in, &out))) {
            size_t idx;
            for (idx = 0; idx < out.count; idx++) {
                if (out.field[idx]->id == TM_PFC_PRI_PROFILEt_PFCf) {
                    map_val->pfc_enable = out.field[idx]->data;
                } else if (out.field[idx]->id == TM_PFC_PRI_PROFILEt_COS_LISTf) {
                    if (out.field[idx]->data == 1) {
                        BCMLTD_BIT_SET(&map_val->cos_list_bmp, out.field[idx]->idx);
                    } else {
                        BCMLTD_BIT_CLR(&map_val->cos_list_bmp, out.field[idx]->idx);
                    }
                }
            }
        }
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

static int
bcmtm_pfc_rx_pri_mapping_update(int unit,
                                bcmltd_sid_t sid,
                                int idx,
                                int pri,
                                const bcmltd_field_t *data_fields)
{
    tm_pfc_pri_to_cos_map_config_t pfc_map[MAX_NUM_PFC_PRI];
    const bcmltd_field_t *gen_field;
    tm_pfc_pri_to_cos_map_config_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(pfc_map, 0,
               sizeof(tm_pfc_pri_to_cos_map_config_t) * MAX_NUM_PFC_PRI);
    sal_memset(&entry, 0,
               sizeof(tm_pfc_pri_to_cos_map_config_t));

    /* Read the current map from imm memory. */
    SHR_IF_ERR_EXIT
        (bcmtm_pfc_rx_map_imm_lkup(unit, idx, pfc_map));

    if (!data_fields) {
        /* Delete requested. */
        pfc_map[pri].pfc_optimized = 0;
        pfc_map[pri].pfc_enable = 0;
        pfc_map[pri].cos_list_bmp = 0;
    } else {
        /* Insert or updated requested, parse the data field. */
        gen_field = data_fields;
        while (gen_field) {
            switch (gen_field->id) {
                case TM_PFC_PRI_PROFILEt_COS_LISTf:
                    if (gen_field->data == 1) {
                        SHR_BITSET(&pfc_map[pri].cos_list_bmp,gen_field->idx);
                    } else {
                        SHR_BITCLR(&pfc_map[pri].cos_list_bmp,gen_field->idx);
                    }
                    break;
                case TM_PFC_PRI_PROFILEt_PFCf:
                    pfc_map[pri].pfc_enable = gen_field->data;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            gen_field = gen_field->next;
        }

    }

    SHR_IF_ERR_EXIT
        (bcmtm_pfc_rx_pri_to_cos_map_pt_set(unit, sid, idx, pfc_map));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_PFC_PRI_PROFILE logical table callback function for staging.
 *
 * Handle TM_PFC_PRI_PROFILE IMM table events.
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
bcmtm_pfc_pri_prof_stage(int unit,
                         bcmltd_sid_t sid,
                         uint32_t trans_id,
                         bcmimm_entry_event_t event_reason,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         void *context,
                         bcmltd_fields_t *out_fields)
{
    const bcmltd_field_t *fld_ptr;
    int profile_idx = 0;
    int pri = 0;

    SHR_FUNC_ENTER(unit);

    fld_ptr = key;
    while (fld_ptr) {
        switch (fld_ptr->id) {
            case TM_PFC_PRI_PROFILEt_TM_PFC_PRI_PROFILE_IDf:
                profile_idx = fld_ptr->data;
                break;
            case TM_PFC_PRI_PROFILEt_PFC_PRIf:
                pri = fld_ptr->data;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        fld_ptr = fld_ptr->next;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            /* Parse data field for input entry. */
            if (!data) {
                /*
                 * Data field not present in the insert command.
                 * Initialize the entry and return.
                 */
                out_fields->count = 0;
                return SHR_E_NONE;
            }
            /* Initialize output_fields. */
            out_fields->count = 0;

            SHR_IF_ERR_EXIT
                (bcmtm_pfc_rx_pri_mapping_update(unit,
                                                  sid,
                                                  profile_idx,
                                                  pri,
                                                  data));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_pfc_rx_pri_mapping_update(unit, sid, profile_idx,
                                                 pri, NULL));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */
int bcmtm_pfc_port_internal_update(int unit,
                                   bcmtm_lport_t lport,
                                   uint8_t action)
{
    bcmltd_sid_t ltid = TM_PFC_EGRt;
    uint32_t pfc_enable = 0, profile_id;
    uint8_t opcode;
    const bcmlrd_map_t *map = NULL;
    int rv = 0;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, ltid, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }


    SHR_IF_ERR_EXIT
        (bcmtm_pfc_egr_port_imm_lkup(unit, ltid, lport, &pfc_enable,
                                     &profile_id, &opcode));

    if (opcode != ENTRY_INVALID) {
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_egr_port_set(unit, ltid, lport, pfc_enable,
                                    profile_id, action, &opcode));
        SHR_IF_ERR_EXIT
            (bcmtm_pfc_egr_port_imm_update(unit, ltid, lport, pfc_enable,
                                           profile_id, opcode));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_pfc_imm_register(int unit)
{
    bcmimm_lt_cb_t tm_cb_handler;
    const bcmlrd_map_t *map = NULL;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * Assign callback functions to IMM event callback handler.
     * commit and abort functions can be added if necessary.
     */

    sal_memset(&tm_cb_handler, 0, sizeof(tm_cb_handler));
    tm_cb_handler.stage    = bcmtm_pfc_pri_prof_stage;

    rv = bcmlrd_map_get(unit, TM_PFC_PRI_PROFILEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_PFC_PRI_PROFILEt, &tm_cb_handler, NULL));

    /*
     * Assign callback functions to IMM event callback handler.
     * commit and abort functions can be added if necessary.
     */
    sal_memset(&tm_cb_handler, 0, sizeof(tm_cb_handler));
    tm_cb_handler.stage = bcmtm_pfc_egr_port_stage;

    rv = bcmlrd_map_get(unit, TM_PFC_EGRt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_PFC_EGRt,
                             &tm_cb_handler, NULL));
exit:
    SHR_FUNC_EXIT();
}
