/*! \file bcmtm_imm_mirror_on_drop.c
 *
 * In-memory call back function for MoD configurations.
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
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmtm/mod/bcmtm_mirror_on_drop_internal.h>
#include <bcmltd/bcmltd_bitop.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>

/*******************************************************************************
 * Local definitions
 */
/* TM_MIRROR_DROP_CONTROL_ENTRY_STATE_T */

#define MOD_VALID                           BCMLTD_COMMON_TM_MIRROR_DROP_CONTROL_ENTRY_STATE_T_T##_VALID
#define MOD_INCORRECT_RESERVED_CELLS_LIMIT  BCMLTD_COMMON_TM_MIRROR_DROP_CONTROL_ENTRY_STATE_T_T##_INCORRECT_RESERVED_CELLS_LIMIT

/* TM_MIRROR_DROP_DESTINATION_ENTRY_STATE_T */
#define MOD_MRDP_VALID                      BCMLTD_COMMON_TM_MIRROR_DROP_DESTINATION_ENTRY_STATE_T_T##_VALID
#define MOD_MRDP_PORT_INFO_UNAVAILABLE      BCMLTD_COMMON_TM_MIRROR_DROP_DESTINATION_ENTRY_STATE_T_T##_PORT_INFO_UNAVAILABLE
#define MOD_MRDP_INCORRECT_QUEUE            BCMLTD_COMMON_TM_MIRROR_DROP_DESTINATION_ENTRY_STATE_T_T##_INVALID_Q_NUM

#define MRDP_UC_QUEUE_DEFAULT               0
#define MRDP_MC_QUEUE_DEFAULT               0
#define MRDP_PORT_DEFAULT                   1
#define MRDP_UC_DEFAULT                     0
#define MRDP_ENCAP_ID_DEFAULT               0
#define MRDP_INSTANCE_ID_DEFAULT            0

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM
/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Mirror on drop IMM table change callback function for staging.
 *
 * Handle Mirror on drop IMM table change.
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
 * \param [out] out_fields This is a linked list of the status data fields
 * updated by this call back function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL MoD fails to handle LT change events.
 */
static int
bcmtm_mirror_on_drop_stage(int unit,
                           bcmltd_sid_t sid,
                           uint32_t trans_id,
                           bcmimm_entry_event_t event_reason,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           void *context,
                           bcmltd_fields_t *out_fields)
{
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    uint32_t enable, enable_opr = 0;
    uint32_t buf_limit, buf_limit_opr = 0;
    uint32_t opcode = 0;
    bcmtm_drv_t *drv;
    bcmtm_mod_drv_t mod_drv;
    uint32_t hwm_cor, hwm_cor_opr = 0;
    uint32_t aging_time, aging_time_opr = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    SHR_IF_ERR_EXIT(drv->mod_drv_get(unit, &mod_drv));

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:

            SHR_NULL_CHECK(out_fields, SHR_E_PARAM);
            /* Parse data field for input entry. */
            if (!data) {
                /*
                 * Data field not present in the insert command.
                 * Initialize the entry and return.
                 */
                out_fields->count = 0;
                return SHR_E_NONE;
            }
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_size_check(unit, out_fields, 3));
            out_fields->count = 0;

            if (data) {
                fid = TM_MIRROR_ON_DROP_CONTROLt_MIRROR_ON_DROPf;
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        data, &fval))) {
                    enable = (uint32_t)fval;
                    if (mod_drv.enable_set) {
                        SHR_IF_ERR_EXIT
                            (mod_drv.enable_set(unit, sid, enable,
                                                 &enable_opr));
                    } else {
                        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
                    }
                }

                fid = TM_MIRROR_ON_DROP_CONTROLt_RESERVED_LIMIT_CELLSf;
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        data, &fval))) {
                    buf_limit = (uint32_t)fval;
                    if (mod_drv.buffer_set) {
                        SHR_IF_ERR_EXIT
                            (mod_drv.buffer_set(unit, sid, buf_limit,
                                                 &buf_limit_opr));
                        SHR_IF_ERR_EXIT
                            (bcmtm_field_list_set(unit, out_fields,
                                              TM_MIRROR_ON_DROP_CONTROLt_RESERVED_LIMIT_CELLS_OPERf,
                                              0, buf_limit_opr));
                        if (buf_limit != buf_limit_opr) {
                            opcode = MOD_INCORRECT_RESERVED_CELLS_LIMIT;
                        }
                    }
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, out_fields,
                                              TM_MIRROR_ON_DROP_CONTROLt_OPERATIONAL_STATEf,
                                              0, opcode));
                }

                fid = TM_MIRROR_ON_DROP_CONTROLt_HIGH_WATERMARK_CLEAR_ON_READf;
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        data, &fval))) {
                    hwm_cor = (uint32_t)fval;
                    if (mod_drv.hwm_cor_set) {
                        SHR_IF_ERR_EXIT
                            (mod_drv.hwm_cor_set(unit, sid, hwm_cor,
                                                 &hwm_cor_opr));
                    } else {
                        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
                    }
                }

                fid = TM_MIRROR_ON_DROP_CONTROLt_ENQUEUE_TIME_OUTf;
                if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        data, &fval))) {
                    aging_time = (uint32_t)fval;
                    if (mod_drv.aging_time_set) {
                        SHR_IF_ERR_EXIT
                            (mod_drv.aging_time_set(unit, sid, aging_time,
                                                 &aging_time_opr));
                    } else {
                        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
                    }
                }

            }
            break;
        case BCMIMM_ENTRY_DELETE:
            if (mod_drv.enable_set) {
                SHR_IF_ERR_EXIT
                    (mod_drv.enable_set(unit, sid, 0, 0));
            }
            if (mod_drv.buffer_set) {
                SHR_IF_ERR_EXIT
                    (mod_drv.buffer_set(unit, sid, 0, 0));
            }
            if (mod_drv.hwm_cor_set) {
                SHR_IF_ERR_EXIT
                    (mod_drv.hwm_cor_set(unit, sid, 0, 0));
            }
            if (mod_drv.aging_time_set) {
                SHR_IF_ERR_EXIT
                    (mod_drv.aging_time_set(unit, sid, 0, 0));
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_MIRROR_ON_DROP_DESTINATION logical table callback function for staging.
 *
 * Handle TM_MIRROR_ON_DROP_DESTINATION table events.
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
bcmtm_mod_dest_stage(int unit,
                     bcmltd_sid_t ltid,
                     uint32_t trans_id,
                     bcmimm_entry_event_t event_reason,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     void *context,
                     bcmltd_fields_t *out_fields)
{

    const bcmltd_field_t *data_ptr;
    int32_t port_in = MRDP_PORT_DEFAULT;
    uint32_t mq_in = MRDP_MC_QUEUE_DEFAULT;
    int32_t ucast_in = MRDP_UC_DEFAULT;
    uint32_t uq_in = MRDP_UC_QUEUE_DEFAULT;
    uint32_t opr = 0;
    uint32_t opr_old = 0;
    bcmltd_fields_t out, in;
    bool in_alloc = 0, out_alloc = 0;
    uint32_t id;
    uint8_t action = 0;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    bcmtm_mod_dest_entry_info_t dest_info;
    bcmtm_drv_t *drv;
    bcmtm_mod_drv_t mod_drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    SHR_IF_ERR_EXIT(drv->mod_drv_get(unit, &mod_drv));

    /* Key. */
    fid = TM_MIRROR_ON_DROP_DESTINATIONt_TM_MIRROR_ON_DROP_DESTINATION_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    id  = (uint32_t)fval;

    sal_memset(&dest_info, 0, sizeof(dest_info));
    if (event_reason == BCMIMM_ENTRY_DELETE) {
        if (mod_drv.dest_set) {
            SHR_IF_ERR_EXIT
                (mod_drv.dest_set(unit, ltid, id, dest_info,
                                   ACTION_DELETE, 0, 0));
        }
        SHR_EXIT();
    } else {
        SHR_NULL_CHECK(out_fields, SHR_E_PARAM);
        if (!data) {
            /*
             * Data field not present in the insert command.
             * Initialize the entry and return.
             */
             out_fields->count = 0;
             SHR_EXIT();
        }
        if (event_reason == BCMIMM_ENTRY_UPDATE) {
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

            in.field[0]->id   = TM_MIRROR_ON_DROP_DESTINATIONt_TM_MIRROR_ON_DROP_DESTINATION_IDf;
            in.field[0]->data = id;

            /* Read the entry if it exists. */
            if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
                size_t idx;
                for (idx = 0; idx < out.count; idx++) {
                    if (out.field[idx]->id == TM_MIRROR_ON_DROP_DESTINATIONt_OPERATIONAL_STATEf) {
                        opr_old = out.field[idx]->data;
                    } else if (out.field[idx]->id == TM_MIRROR_ON_DROP_DESTINATIONt_PORT_IDf) {
                        port_in = out.field[idx]->data;
                    } else if (out.field[idx]->id == TM_MIRROR_ON_DROP_DESTINATIONt_TM_UC_Q_IDf) {
                        uq_in = out.field[idx]->data;
                    } else if (out.field[idx]->id == TM_MIRROR_ON_DROP_DESTINATIONt_TM_MC_Q_IDf) {
                        mq_in = out.field[idx]->data;
                    } else if (out.field[idx]->id == TM_MIRROR_ON_DROP_DESTINATIONt_UC_Qf) {
                        ucast_in = out.field[idx]->data;
                    }
                }
            }
        }

        SHR_IF_ERR_EXIT
            (bcmtm_field_list_size_check(unit, out_fields, 1));
        out_fields->count = 0;

        /* Parse the data fields. */
        data_ptr = data;
        while (data_ptr) {
            switch (data_ptr->id) {
                case TM_MIRROR_ON_DROP_DESTINATIONt_PORT_IDf:
                    port_in = data_ptr->data;
                    break;
                case TM_MIRROR_ON_DROP_DESTINATIONt_TM_UC_Q_IDf:
                    uq_in = data_ptr->data;
                    break;
                case TM_MIRROR_ON_DROP_DESTINATIONt_TM_MC_Q_IDf:
                    mq_in = data_ptr->data;
                    break;
                case TM_MIRROR_ON_DROP_DESTINATIONt_UC_Qf:
                    ucast_in = data_ptr->data;
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            data_ptr = data_ptr->next;
        }

        if (event_reason == BCMIMM_ENTRY_UPDATE) {
            action = ACTION_UPDATE;
        } else if (event_reason == BCMIMM_ENTRY_INSERT) {
            action = ACTION_INSERT;
        }

        if (mod_drv.dest_set) {
            dest_info.lport = port_in;
            dest_info.uc_q_id = uq_in;
            dest_info.mc_q_id = mq_in;
            dest_info.ucast = ucast_in;
            SHR_IF_ERR_EXIT
                (mod_drv.dest_set(unit, ltid, id, dest_info,
                                   action, &opr, opr_old));
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, out_fields,
                    TM_MIRROR_ON_DROP_DESTINATIONt_OPERATIONAL_STATEf,
                    0, opr));
        }
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
 * \brief TM_MIRROR_ON_DROP_ENCAP_PROFILE logical table callback function for
 * staging.
 *
 * Handle TM_MIRROR_ON_DROP_ENCAP_PROFILE table events.
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
bcmtm_mod_encap_profile_stage(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t trans_id,
                              bcmimm_entry_event_t event_reason,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              void *context,
                              bcmltd_fields_t *out_fields)
{

    const bcmltd_field_t *data_ptr;
    uint32_t instance_id = MRDP_INSTANCE_ID_DEFAULT;
    bcmltd_fields_t out, in;
    bool in_alloc = 0, out_alloc = 0;
    uint32_t id;
    uint8_t action = 0;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    bcmtm_mod_encap_prof_entry_info_t encap_prof_info;
    bcmtm_drv_t *drv;
    bcmtm_mod_drv_t mod_drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    SHR_IF_ERR_EXIT(drv->mod_drv_get(unit, &mod_drv));

    /* Key. */
    fid = TM_MIRROR_ON_DROP_ENCAP_PROFILEt_TM_MIRROR_ON_DROP_ENCAP_PROFILE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    id  = (uint32_t)fval;

    sal_memset(&encap_prof_info, 0, sizeof(encap_prof_info));
    if (event_reason == BCMIMM_ENTRY_DELETE) {
        if (mod_drv.encap_set) {
            SHR_IF_ERR_EXIT
                (mod_drv.encap_set(unit, ltid, id, encap_prof_info,
                                    ACTION_DELETE));
        }
        SHR_EXIT();
    } else {
        SHR_NULL_CHECK(out_fields, SHR_E_PARAM);
        if (!data) {
            /*
             * Data field not present in the insert command.
             * Initialize the entry and return.
             */
             out_fields->count = 0;
             SHR_EXIT();
        }
        if (event_reason == BCMIMM_ENTRY_UPDATE) {
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

            in.field[0]->id = TM_MIRROR_ON_DROP_ENCAP_PROFILEt_TM_MIRROR_ON_DROP_ENCAP_PROFILE_IDf;
            in.field[0]->data = id;

            /* Read the entry if it exists. */
            if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
                size_t idx;
                for (idx = 0; idx < out.count; idx++) {
                    if ((out.field[idx]->id == TM_MIRROR_ON_DROP_ENCAP_PROFILEt_MIRROR_ENCAP_IDf) ||
                         (out.field[idx]->id == TM_MIRROR_ON_DROP_ENCAP_PROFILEt_MIRROR_INSTANCE_IDf)) {
                        instance_id = out.field[idx]->data;
                    } else {
                      /* Do nothing */
                    }
                }
            }
        }

        SHR_IF_ERR_EXIT
            (bcmtm_field_list_size_check(unit, out_fields, 1));
        out_fields->count = 0;

        /* Parse the data fields. */
        data_ptr = data;
        while (data_ptr) {
            switch (data_ptr->id) {
                case TM_MIRROR_ON_DROP_ENCAP_PROFILEt_MIRROR_INSTANCE_IDf:
                case TM_MIRROR_ON_DROP_ENCAP_PROFILEt_MIRROR_ENCAP_IDf:
                    instance_id = data_ptr->data;
                    break;
                case TM_MIRROR_ON_DROP_ENCAP_PROFILEt_MIRROR_MEMBER_IDf:
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
            data_ptr = data_ptr->next;
        }

        if (event_reason == BCMIMM_ENTRY_UPDATE) {
            action = ACTION_UPDATE;
        } else if (event_reason == BCMIMM_ENTRY_INSERT) {
            action = ACTION_INSERT;
        }

        if (mod_drv.encap_set) {
            encap_prof_info.mirror_instance_id  = instance_id;
            SHR_IF_ERR_EXIT
                (mod_drv.encap_set(unit, ltid, id, encap_prof_info, action));
        }
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
 * \brief MoD destination LT imm lookup.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [out] dest_id MoD Destination index.
 * \param [out] mc_q_id Multicast queue id.
 * \param [out] uc_q_id Unicast queue id.
 * \param [out] unicast qualifies uc_q_id/mc_q_id.
 * \param [out] opcode Operational State code.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mod_dest_imm_lkup(int unit,
                        bcmltd_sid_t ltid,
                        bcmtm_lport_t lport,
                        uint32_t *dest_id,
                        bcmtm_mod_dest_entry_info_t *dest_info,
                        uint32_t *opcode)

{
    bcmltd_fields_t out, in;
    bcmltd_fid_t fid;
    size_t num_fid;
    uint64_t fval;
    bool in_alloc = 0, out_alloc = 0;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));
    out_alloc = 1;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));
    in_alloc = 1;
    in.count = 0;

    fid = TM_MIRROR_ON_DROP_DESTINATIONt_TM_MIRROR_ON_DROP_DESTINATION_IDf;
    *dest_id = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, *dest_id));

    if (SHR_FAILURE(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        *opcode = ENTRY_INVALID;
    } else {
        fid = TM_MIRROR_ON_DROP_DESTINATIONt_PORT_IDf;
        if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
            if ((uint32_t)fval != lport) {
                *opcode = ENTRY_INVALID;
            } else {
                dest_info->lport = lport;
            }
        } else {
            size_t idx;
            for (idx = 0; idx < out.count; idx++) {
                if (out.field[idx]->id == TM_MIRROR_ON_DROP_DESTINATIONt_OPERATIONAL_STATEf) {
                    *opcode = out.field[idx]->data;
                } else if (out.field[idx]->id == TM_MIRROR_ON_DROP_DESTINATIONt_TM_UC_Q_IDf) {
                    dest_info->uc_q_id = out.field[idx]->data;
                } else if (out.field[idx]->id == TM_MIRROR_ON_DROP_DESTINATIONt_TM_MC_Q_IDf) {
                    dest_info->mc_q_id = out.field[idx]->data;
                } else if (out.field[idx]->id == TM_MIRROR_ON_DROP_DESTINATIONt_UC_Qf) {
                    dest_info->ucast = out.field[idx]->data;
                }
           }
       }
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

/*
 * \brief Updates IMM for MoD Destination LT.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] dest_id MoD Destination ID.
 * \param [in] opcode Operational state to be updated to IMM.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_mod_dest_imm_update(int unit,
                          bcmltd_sid_t ltid,
                          uint32_t dest_id,
                          uint8_t opcode)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));

    in.count = 0;

    /* TM_MIRROR_ON_DROP_DESTINATIONt_TM_MIRROR_ON_DROP_DESTINATION_IDf */
    fid = TM_MIRROR_ON_DROP_DESTINATIONt_TM_MIRROR_ON_DROP_DESTINATION_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, dest_id));

    /* TM_MIRROR_ON_DROP_DESTINATIONt_OPERATIONAL_STATEf */
    fid = TM_MIRROR_ON_DROP_DESTINATIONt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, opcode));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_mod_imm_register(int unit)
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
    tm_cb_handler.stage    = bcmtm_mirror_on_drop_stage;

    rv = bcmlrd_map_get(unit, TM_MIRROR_ON_DROP_CONTROLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_MIRROR_ON_DROP_CONTROLt, &tm_cb_handler, NULL));

    /*
     * Assign callback functions to IMM event callback handler.
     * commit and abort functions can be added if necessary.
     */
    sal_memset(&tm_cb_handler, 0, sizeof(tm_cb_handler));
    tm_cb_handler.stage = bcmtm_mod_dest_stage;

    rv = bcmlrd_map_get(unit, TM_MIRROR_ON_DROP_DESTINATIONt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_MIRROR_ON_DROP_DESTINATIONt,
                             &tm_cb_handler, NULL));

    /* Register callback handler for TM_MIRROR_ON_DROP_ENCAP_PROFILE. */
    rv = bcmlrd_map_get(unit, TM_MIRROR_ON_DROP_ENCAP_PROFILEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    sal_memset(&tm_cb_handler, 0, sizeof(tm_cb_handler));
    tm_cb_handler.stage = bcmtm_mod_encap_profile_stage;
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_MIRROR_ON_DROP_ENCAP_PROFILEt,
                             &tm_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int bcmtm_mod_port_internal_update(int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t action)
{
    bcmltd_sid_t ltid = TM_MIRROR_ON_DROP_DESTINATIONt;
    uint32_t opcode = 0;
    uint32_t opcode_new;
    uint32_t dest_id;
    bcmtm_mod_dest_entry_info_t dest_info;
    const bcmlrd_map_t *map = NULL;
    int rv = 0;
    bcmtm_drv_t *drv;
    bcmtm_mod_drv_t mod_drv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, ltid, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    SHR_IF_ERR_EXIT(drv->mod_drv_get(unit, &mod_drv));

    sal_memset(&dest_info, 0, sizeof(dest_info));
    SHR_IF_ERR_EXIT
        (bcmtm_mod_dest_imm_lkup(unit, ltid, lport, &dest_id, &dest_info,
                                 &opcode));

    if (opcode != ENTRY_INVALID) {
        if (mod_drv.dest_set) {
            dest_info.lport = lport;
            SHR_IF_ERR_EXIT
                (mod_drv.dest_set(unit, ltid, dest_id, dest_info,
                                   action, &opcode_new, opcode));
            SHR_IF_ERR_EXIT
                (bcmtm_mod_dest_imm_update(unit, ltid, dest_id, opcode_new));
       }
    }
exit:
    SHR_FUNC_EXIT();
}
