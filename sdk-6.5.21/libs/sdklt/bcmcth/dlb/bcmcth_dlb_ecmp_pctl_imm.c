/*! \file bcmcth_dlb_ecmp_pctl_imm.c
 *
 * DLB ECMP port control interface to in-memory table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_ha.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_dlb_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_DLB

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief imm DLB_ECMP_PORT_CONTROLt notification input fields parsing.
 *
 * Parse imm DLB_ECMP_PORT_CONTROLt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] DLB ECMP LT infomation data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c lt_info.
 */
static int
dlb_ecmp_pctl_lt_fields_parse(int unit,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data,
                              bcmcth_dlb_ecmp_pctl_t *lt_entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case DLB_ECMP_PORT_CONTROLt_PORT_IDf:
            BCMCTH_DLB_LT_FIELD_SET(
                lt_entry->fbmp, BCMCTH_DLB_ECMP_PCTL_LT_FIELD_PORT_ID);
            lt_entry->port_id = (bcmcth_dlb_port_id_t)fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case DLB_ECMP_PORT_CONTROLt_OVERRIDEf:
            BCMCTH_DLB_LT_FIELD_SET(
                lt_entry->fbmp, BCMCTH_DLB_ECMP_PCTL_LT_FIELD_OVERRIDE);
            lt_entry->override = (bool) fval;
            break;
        case DLB_ECMP_PORT_CONTROLt_OVERRIDE_LINK_STATEf:
            BCMCTH_DLB_LT_FIELD_SET(
                lt_entry->fbmp, BCMCTH_DLB_ECMP_PCTL_LT_FIELD_FORCE_LINK_STATUS);
            lt_entry->force_link_status = (bool) fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Init all fields value as SW default value.
 *
 * This initialization is only used for Insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry DLB_ECMP_PORT_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
dlb_ecmp_pctl_lt_default_values_init(int unit,
                                     bcmcth_dlb_ecmp_pctl_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    if (!BCMCTH_DLB_LT_FIELD_GET(
            lt_entry->fbmp, BCMCTH_DLB_ECMP_PCTL_LT_FIELD_OVERRIDE)) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit,
                                      DLB_ECMP_PORT_CONTROLt,
                                      DLB_ECMP_PORT_CONTROLt_OVERRIDEf,
                                      1, &def_val, &num));
        lt_entry->override = (bool)def_val;
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp, BCMCTH_DLB_ECMP_PCTL_LT_FIELD_OVERRIDE);
    }

    if (!BCMCTH_DLB_LT_FIELD_GET(
            lt_entry->fbmp, BCMCTH_DLB_ECMP_PCTL_LT_FIELD_FORCE_LINK_STATUS)) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit,
                                      DLB_ECMP_PORT_CONTROLt,
                                      DLB_ECMP_PORT_CONTROLt_OVERRIDE_LINK_STATEf,
                                      1, &def_val, &num));
        lt_entry->force_link_status = (bool)def_val;
        BCMCTH_DLB_LT_FIELD_SET(lt_entry->fbmp, BCMCTH_DLB_ECMP_PCTL_LT_FIELD_FORCE_LINK_STATUS);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert or update the LT entry.
 *
 * Parse Insert or update the DLB_ECMP_PORT_CONTROL Lt entry
 *
 * \param [in]  unit Unit number.
 * \param [in]  op_arg Operation arguments.
 * \param [in]  sid LTD Symbol ID.
 * \param [in]  key field list.
 * \param [in]  data field list.
 * \param [out] event_reason reason code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to insert or update the LT entry.
 */
static int
dlb_ecmp_pctl_update(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     bcmltd_sid_t sid,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     bcmimm_entry_event_t event_reason)
{
    bcmcth_dlb_ecmp_pctl_t lt_entry;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    if (sid != DLB_ECMP_PORT_CONTROLt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.op_arg = op_arg;
    lt_entry.glt_sid = sid;
    SHR_IF_ERR_EXIT
        (dlb_ecmp_pctl_lt_fields_parse(unit, key, data, &lt_entry));
    if (event_reason == BCMIMM_ENTRY_INSERT) {
        SHR_IF_ERR_EXIT
            (dlb_ecmp_pctl_lt_default_values_init(unit, &lt_entry));
    }

    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_pctl_set(unit, &lt_entry));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_ECMP_PORT_CONTROL LT entry delete
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] sid LTD Symbol ID.
 * \param [in] key List of the key fields.
 * \param [in] data List of the data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
dlb_ecmp_pctl_delete(int unit,
                     const bcmltd_op_arg_t *op_arg,
                     bcmltd_sid_t sid,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data)
{
    bcmcth_dlb_ecmp_pctl_t lt_entry;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(key, SHR_E_PARAM);

    sal_memset(&lt_entry, 0, sizeof(lt_entry));
    lt_entry.op_arg = op_arg;
    lt_entry.glt_sid = sid;

    SHR_IF_ERR_EXIT
        (dlb_ecmp_pctl_lt_fields_parse(unit, key, data, &lt_entry));
    SHR_IF_ERR_EXIT
        (bcmcth_dlb_ecmp_pctl_del(unit, &lt_entry));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of IMM event handler.
 *
 * \param [in] unit Device unit number.
 * \param [in] sid Logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event_reason Reason for the entry event.
 * \param [in] key_flds Linked list of the key fields identifying the entry.
 * \param [in] data_flds Linked list of the data fields in the modified entry.
 * \param [in] context Pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [out] output_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the output_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
dlb_ecmp_pctl_stage(int unit,
                    bcmltd_sid_t sid,
                    const bcmltd_op_arg_t *op_arg,
                    bcmimm_entry_event_t event_reason,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (data != NULL) {
            SHR_IF_ERR_EXIT
                (dlb_ecmp_pctl_update(unit,
                                      op_arg,
                                      sid,
                                      key,
                                      data,
                                      event_reason));
        }
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_EXIT
            (dlb_ecmp_pctl_delete(unit,
                                  op_arg,
                                  sid,
                                  key,
                                  data));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_ECMP_PORT_CONTROLt In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to DLB_ECMP_PORT_CONTROLt logical table entry commit stages.
 */
static bcmimm_lt_cb_t bcmcth_dlb_ecmp_pctl_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = dlb_ecmp_pctl_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*******************************************************************************
 * Public functions
 */

int
bcmcth_dlb_ecmp_pctl_imm_init(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for DLB LTs here.
     */
    rv = bcmlrd_map_get(unit, DLB_ECMP_PORT_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 DLB_ECMP_PORT_CONTROLt,
                                 &bcmcth_dlb_ecmp_pctl_imm_callback,
                                 NULL));
    }
exit:
    SHR_FUNC_EXIT();
}
