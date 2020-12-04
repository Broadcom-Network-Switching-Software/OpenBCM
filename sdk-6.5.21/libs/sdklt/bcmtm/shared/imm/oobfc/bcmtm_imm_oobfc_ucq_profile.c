/*! \file bcmtm_imm_oobfc_ucq_profile.c
 *
 * In-memory call back function for unicast profile mapping
 * configurations for OOBFC.
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
#include <bcmtm/oobfc/bcmtm_oobfc_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief  Populates OOBFC unicast queue map profile configuration
 * Busing user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] ucq_profile Unicast queue map profile configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static void
bcmtm_oobfc_ucq_profile_parse(int unit,
                              const bcmltd_field_t *data,
                              bcmtm_oobfc_q_profile_t *ucq_profile)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;

    while (cur_data) {
        switch (cur_data->id) {
            case TM_OOBFC_UC_Q_MAP_PROFILEt_CONGESTION_NOTIFYf:
                ucq_profile->cng_notify = (uint8_t)cur_data->data;
                break;
            case TM_OOBFC_UC_Q_MAP_PROFILEt_OOB_BIT_OFFSETf:
                ucq_profile->oob_bit_offset = (uint8_t)cur_data->data;
                break;
            case TM_OOBFC_UC_Q_MAP_PROFILEt_OPERATIONAL_STATEf:
                ucq_profile->opcode = (uint8_t)cur_data->data;
                break;
            default:
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief  OOBFC unicast queue profile map physical table configuration
 * based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] profile_id Profile ID.
 * \param [in] ucq_id Unicast Queue ID.
 * \param [in] ucq_profile Unicast queue map profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_oobfc_ucq_profile_pt_set(int unit,
                               bcmltd_sid_t ltid,
                               uint8_t profile_id,
                               uint8_t ucq_id,
                               bcmtm_oobfc_q_profile_t *ucq_profile)
{
    bcmtm_drv_t *drv;
    bcmtm_oobfc_drv_t oobfc_drv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&oobfc_drv, 0, sizeof(bcmtm_oobfc_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->oobfc_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->oobfc_drv_get(unit, &oobfc_drv));
    /* update physical table. */
    if (oobfc_drv.ucq_profile_set) {
        SHR_IF_ERR_EXIT
            (oobfc_drv.ucq_profile_set(unit, ltid, profile_id,
                                       ucq_id, ucq_profile));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_OOBFC_UC_Q_MAP_PROFILE imm lookup.
 *
 * Performs IMM lookup and populates OOBFC configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] profile_id Profile ID.
 * \param [in] ucq_id Unicast queue ID.
 * \param [out] ucq_profile Unicast queue map profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_oobfc_ucq_profile_imm_lkup(int unit,
                                 bcmltd_sid_t ltid,
                                 uint8_t profile_id,
                                 uint8_t ucq_id,
                                 bcmtm_oobfc_q_profile_t *ucq_profile)
{
    bcmltd_fields_t out, in;
    bcmltd_fid_t fid;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));
    in.count = 0;

    /* TM_OOBFC_UC_Q_MAP_PROFILEt_TM_OOBFC_Q_MAP_PROFILE_ID */
    fid = TM_OOBFC_UC_Q_MAP_PROFILEt_TM_OOBFC_Q_MAP_PROFILE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, profile_id));

    /* TM_OOBFC_UC_Q_MAP_PROFILEt_TM_UC_Q_IDf */
    fid = TM_OOBFC_UC_Q_MAP_PROFILEt_TM_UC_Q_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, ucq_id));

    if (SHR_FAILURE(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        ucq_profile->opcode = ENTRY_INVALID;
    } else {
        bcmtm_oobfc_ucq_profile_parse(unit, *(out.field), ucq_profile);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}


/*!
 * \brief  Resets the physical table to default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] profile_id Profile ID.
 * \param [in] ucq_id Unicast Queue ID.
 * \param [in] ucq_profile Unicast queue map profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_oobfc_ucq_profile_reset(int unit,
                              bcmltd_sid_t ltid,
                              uint8_t profile_id,
                              uint8_t ucq_id,
                              bcmtm_oobfc_q_profile_t *ucq_profile)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_oobfc_ucq_profile_imm_lkup(unit, ltid, profile_id,
                                          ucq_id, ucq_profile));

    if (ucq_profile->opcode == VALID) {
        sal_memset(ucq_profile, 0, sizeof(bcmtm_oobfc_q_profile_t));
        /* Physical table update */
        SHR_IF_ERR_EXIT
            (bcmtm_oobfc_ucq_profile_pt_set(unit, ltid, profile_id,
                                            ucq_id, ucq_profile));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] profile_id Profile ID.
 * \param [in] ucq_id Unicast Queue ID.
 * \param [in] ucq_profile Unicast queue map profile.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_oobfc_ucq_profile_set(int unit,
                            bcmltd_sid_t ltid,
                            uint8_t profile_id,
                            uint8_t ucq_id,
                            bcmtm_oobfc_q_profile_t *ucq_profile)
{
    int num_ucq = 0;
    bcmtm_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));

    num_ucq = drv->num_uc_q_cfg_get(unit);

    /* Validating for number of unicast queues for the system. */
    if (ucq_id >= (uint32_t)num_ucq) {
        ucq_profile->opcode = UC_Q_INVALID;
        SHR_EXIT();
    }

    /* PT update */
    SHR_IF_ERR_EXIT
        (bcmtm_oobfc_ucq_profile_pt_set(unit, ltid, profile_id,
                                        ucq_id, ucq_profile));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief TM_OOBFC_UC_Q_MAP_PROFILE logical table callback function for staging.
 *
 * Handle TM_OOBFC_UC_Q_MAP_PROFILE IMM table change events.
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
bcmtm_oobfc_ucq_profile_stage(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t trans_id,
                              bcmimm_entry_event_t event_reason,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              void *context,
                              bcmltd_fields_t *output_fields)
{
    bcmtm_oobfc_q_profile_t ucq_profile;
    bcmltd_fid_t fid;
    uint8_t ucq_id, profile_id;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ucq_profile, 0, sizeof(bcmtm_oobfc_q_profile_t));

    fid = TM_OOBFC_UC_Q_MAP_PROFILEt_TM_OOBFC_Q_MAP_PROFILE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    profile_id = (uint8_t)fval;

    fid = TM_OOBFC_UC_Q_MAP_PROFILEt_TM_UC_Q_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    ucq_id = (uint8_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_oobfc_ucq_profile_imm_lkup(unit, ltid, profile_id,
                                            ucq_id, &ucq_profile));

            /* Fall through */
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            bcmtm_oobfc_ucq_profile_parse(unit, data, &ucq_profile);
            SHR_IF_ERR_EXIT
                (bcmtm_oobfc_ucq_profile_set(unit, ltid, profile_id,
                                             ucq_id, &ucq_profile));

            /* Operation state update */
            fid = TM_OOBFC_UC_Q_MAP_PROFILEt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields, fid, 0,
                                      ucq_profile.opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_oobfc_ucq_profile_reset(unit, ltid, profile_id,
                                            ucq_id, &ucq_profile));
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
bcmtm_oobfc_ucq_profile_imm_register(int unit)
{
    /*!
     * Inmemory callback functions for
     * TM_OOBFC_UC_Q_MAP_PROFILE logical table.
     */
    bcmimm_lt_cb_t bcmtm_oobfc_ucq_profile_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_oobfc_ucq_profile_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_OOBFC_UC_Q_MAP_PROFILEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for OOBFC unicast queue profile . */
    if (SHR_FAILURE
        (bcmimm_lt_event_reg(unit, TM_OOBFC_UC_Q_MAP_PROFILEt,
                            &bcmtm_oobfc_ucq_profile_imm_cb, NULL))) {

        /* Not an imm implementation. */
        SHR_EXIT();
    }
exit:
    SHR_FUNC_EXIT();
}
