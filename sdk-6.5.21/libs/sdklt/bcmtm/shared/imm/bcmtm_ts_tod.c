/*! \file bcmtm_ts_tod.c
 *
 * BCMTM Timestamp TOD handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/id/bcmltd_common_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_ts_tod.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmimm/bcmimm_int_comp.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMTM_IMM

/******************************************************************************
 * Private functions
 */
/*! \brief Parse and populate the tod values from the list.
 *
 * \param [in] unit Logical unit number.
 * \param [in] data This is a linked list of the data fields in the entry.
 * \param [out] tod TOD values.
 */
 static void
 bcmtm_ts_tod_parse(int unit,
                    const bcmltd_field_t *data,
                    bcmtm_ts_tod_entry_t *tod)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;
    while (cur_data) {
        switch (cur_data->id) {
            case TM_TS_TODt_HW_UPDATEf:
                tod->hw_update = cur_data->data;
                break;
            case TM_TS_TODt_ADJUSTf:
                tod->adjust = cur_data->data;
                break;
            case TM_TS_TODt_TOD_NSECf:
                tod->nsec = cur_data->data;
                break;
            case TM_TS_TODt_TOD_SECf:
                tod->sec = cur_data->data;
                break;
            default:
                break;
        }
        cur_data = cur_data->next;
    }
}


/*! \brief Get TM_TS_TOD default field values.
 *
 * \param [in] unit Logical unit number.
 * \param [out] tod TOD default values programmed.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_MEMORY No memory available.
 */
static int
bcmtm_ts_tod_default_get(int unit,
                         bcmtm_ts_tod_entry_t *tod)
{
    bcmlrd_client_field_info_t *f_info = NULL;
    const bcmlrd_table_rep_t *lt_info;
    int fid;
    size_t num_fid = TM_TS_TODt_FIELD_COUNT, count;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(f_info, sizeof(bcmlrd_client_field_info_t) * num_fid,
              "bcmtmTsTodDefaultFields");
    SHR_NULL_CHECK(f_info, SHR_E_MEMORY);

    lt_info = bcmlrd_table_get(TM_TS_TODt);

    SHR_NULL_CHECK(lt_info, SHR_E_INTERNAL);
    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, lt_info->name,
                                     num_fid, f_info, &num_fid));
    for (count = 0; count < num_fid; count++) {
        fid = f_info[count].id;
        fval = bcmtm_lt_field_data_to_u64_xfrm(&(f_info[count].def),
                                          f_info[count].width);
        switch (fid) {
            case TM_TS_TODt_HW_UPDATEf:
                tod->hw_update = fval;
                break;
            case TM_TS_TODt_TOD_SECf:
                tod->sec = fval;
                break;
            case TM_TS_TODt_TOD_NSECf:
                tod->nsec = fval;
                break;
            case TM_TS_TODt_ADJUSTf:
                tod->adjust = fval;
                break;
            break;

        }
    }
exit:
    SHR_FREE(f_info);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_TS_TOD IMM lookup.
 *
 * \param [in] unit Logical unit number.
 * \param [out] tod TOD values from IMM lookup.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_MEMORY Error in memory allocation.
 */
static int
bcmtm_ts_tod_imm_lookup(int unit,
                        bcmtm_ts_tod_entry_t *tod)
{
    bcmltd_fields_t out, in;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    /*! Out list for the imm lookup. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, TM_TS_TODt_FIELD_COUNT, &out));
    /*! Dummy entry as there are no keys. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));

    in.count = 0;

    if (SHR_E_NONE == bcmimm_entry_lookup(unit, TM_TS_TODt, &in, &out)) {
        /* update the TS_ToD */
        bcmtm_ts_tod_parse(unit, *(out.field), tod);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}


/*!
 * \brief BCMTM TS TOD update.
 *
 * \param [in] unit Unit number.
 * \param [in] tod TOD parameters/
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_ts_tod_pt_update(int unit,
                       bcmltd_sid_t ltid,
                       bcmtm_ts_tod_entry_t *tod)
{
    bcmtm_drv_t *drv;
    bcmtm_ts_tod_drv_t tod_drv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&tod_drv, 0, sizeof(bcmtm_ts_tod_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->ts_tod_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->ts_tod_drv_get(unit, &tod_drv));

    if (tod->hw_update && tod_drv.tod_auto) {
        SHR_IF_ERR_EXIT(tod_drv.tod_auto(unit, ltid, tod));
    } else {
        if (tod_drv.tod_auto) {
            tod_drv.tod_auto(unit, ltid, tod);
        }
        if (tod_drv.tod_update) {
            tod_drv.tod_update(unit, ltid, tod);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief BCMTM TS TOD stage function.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] output_fields This is a linked list of extra added data fields (mostly
 * read only fields).
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmtm_ts_tod_stage(int unit,
                   bcmltd_sid_t sid,
                   uint32_t trans_id,
                   bcmimm_entry_event_t event,
                   const bcmltd_field_t *key,
                   const bcmltd_field_t *data,
                   void *context,
                   bcmltd_fields_t *output_fields)
{
    bcmtm_ts_tod_entry_t tod;

    SHR_FUNC_ENTER(unit);
    sal_memset (&tod, 0, sizeof(bcmtm_ts_tod_entry_t));

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_ts_tod_imm_lookup(unit, &tod));
            tod.nsec = tod.sec = 0xffffffff;
            bcmtm_ts_tod_parse(unit, data, &tod);
            SHR_IF_ERR_EXIT
                (bcmtm_ts_tod_pt_update(unit, sid, &tod));
            break;
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_EXIT
                (bcmtm_ts_tod_default_get(unit, &tod));
            tod.nsec = tod.sec = 0xffffffff;
            bcmtm_ts_tod_parse(unit, data, &tod);
            SHR_IF_ERR_EXIT
                (bcmtm_ts_tod_pt_update(unit, sid, &tod));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_ts_tod_pt_update(unit, sid, &tod));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief TM_TS_TOD imm lookup handler.
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
bcmtm_ts_tod_lookup(int unit,
                    bcmltd_sid_t ltid,
                    uint32_t trans_id,
                    void *context,
                    bcmimm_lookup_type_t lkup_type,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out)
{
    bcmtm_ts_tod_entry_t entry;
    bcmtm_drv_t *drv;
    bcmtm_ts_tod_drv_t ts_tod_drv;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ts_tod_drv, 0, sizeof(bcmtm_ts_tod_drv_t));
    sal_memset(&entry, 0, sizeof(bcmtm_ts_tod_entry_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));

    SHR_NULL_CHECK(drv->ts_tod_drv_get, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(drv->ts_tod_drv_get(unit, &ts_tod_drv));

    if (ts_tod_drv.tod_get) {
        SHR_IF_ERR_EXIT
            (ts_tod_drv.tod_get(unit, ltid, &entry));

        /* populate the out fields. */
        fid = TM_TS_TODt_HW_UPDATEf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, out, fid, 0, entry.hw_update));

        fid = TM_TS_TODt_TOD_SEC_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, out, fid, 0, entry.sec));

        fid = TM_TS_TODt_TOD_NSEC_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, out, fid, 0, entry.nsec));

        fid = TM_TS_TODt_ADJUST_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, out, fid, 0, entry.adjust));
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
 int
 bcmtm_ts_tod_imm_register(int unit)
 {
    const bcmlrd_map_t *map = NULL;
    int rv;
    bcmimm_lt_cb_t bcmtm_ts_tod_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_ts_tod_stage,
        /*! Lookup function. */
        .lookup = bcmtm_ts_tod_lookup,
    };

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_TS_TODt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if (rv == SHR_E_UNAVAIL || !map) {
        SHR_EXIT();
    }

    /*! Registers callback functions for TS_TOD. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_TS_TODt, &bcmtm_ts_tod_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
 }
