/*! \file bcmcth_mon_inband_telemetry_metadata_field_info.c
 *
 * Inband telemetry metadata field information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmcth/bcmcth_mon_drv.h>
#include <bcmcth/bcmcth_mon_inband_telemetry_metadata_field_info.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/******************************************************************************
 * Local definitions
 */


/*******************************************************************************
 * Public functions
 */


int
bcmcth_mon_inband_telemetry_metadata_field_info_validate(
    int unit,
    bcmlt_opcode_t opcode,
    const bcmltd_fields_t *in,
    const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    if ((opcode != BCMLT_OPCODE_LOOKUP) &&
        (opcode != BCMLT_OPCODE_TRAVERSE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_inband_telemetry_metadata_field_info_insert(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg)
{
    /* Read only table insertion is not allowed */
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_inband_telemetry_metadata_field_info_lookup(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg)
{
    size_t count = 0;
    bcmcth_mon_drv_t *mon_drv = bcmcth_mon_drv_get(unit);
    bcmcth_mon_inband_telemetry_metadata_field_t field_info;
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    sal_memset(&field_info, 0,
        sizeof(bcmcth_mon_inband_telemetry_metadata_field_t));
    /* Set default key value for traverse (first) where key not provided. */
    field_info.fld_enum = 0;
    /* Parse key field */
    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id ==
            MON_INBAND_TELEMETRY_METADATA_FIELD_INFOt_MON_INBAND_TELEMETRY_METADATA_FIELD_INFO_IDf) {
            field_info.fld_enum = in->field[i]->data;
            break;
        }
    }

    if (mon_drv && mon_drv->telemetry_field_info) {
        SHR_IF_ERR_EXIT(mon_drv->telemetry_field_info(unit, &field_info));
    }

    out->field[count]->id =
        MON_INBAND_TELEMETRY_METADATA_FIELD_INFOt_MON_INBAND_TELEMETRY_METADATA_FIELD_INFO_IDf;
    out->field[count]->data = field_info.fld_enum;
    out->field[count]->idx = 0;
    count++;

    out->field[count]->id =
        MON_INBAND_TELEMETRY_METADATA_FIELD_INFOt_SIZEf;
    out->field[count]->data = field_info.fld_size;
    out->field[count]->idx = 0;
    count++;

    out->field[count]->id =
        MON_INBAND_TELEMETRY_METADATA_FIELD_INFOt_WIDE_INDEXf;
    out->field[count]->data = field_info.wide_index;
    out->field[count]->idx = 0;
    count++;

    out->field[count]->id =
        MON_INBAND_TELEMETRY_METADATA_FIELD_INFOt_WIDE_STARTf;
    out->field[count]->data = field_info.wide_start;
    out->field[count]->idx = 0;
    count++;

    out->field[count]->id =
        MON_INBAND_TELEMETRY_METADATA_FIELD_INFOt_NARROW_INDEXf;
    out->field[count]->data = field_info.narrow_index;
    out->field[count]->idx = 0;
    count++;

    out->field[count]->id =
        MON_INBAND_TELEMETRY_METADATA_FIELD_INFOt_NARROW_STARTf;
    out->field[count]->data = field_info.narrow_start;
    out->field[count]->idx = 0;
    count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_inband_telemetry_metadata_field_info_delete(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg)
{
    /* Read only table delete operation not permitted */
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_inband_telemetry_metadata_field_info_update(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg)
{
    /* Read only table update operation not permitted */
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_inband_telemetry_metadata_field_info_first(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_mon_inband_telemetry_metadata_field_info_lookup(
            unit, op_arg, out, out, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_inband_telemetry_metadata_field_info_next(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg)
{
    bcmcth_mon_drv_t *mon_drv = bcmcth_mon_drv_get(unit);
    bcmcth_mon_inband_telemetry_metadata_attrs_t attrs_info;
    uint32_t i = 0;
    uint32_t field_number = 0;
    uint32_t field_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    sal_memset(&attrs_info, 0,
        sizeof(bcmcth_mon_inband_telemetry_metadata_attrs_t));
    if (mon_drv && mon_drv->telemetry_attrs_info) {
        SHR_IF_ERR_EXIT(mon_drv->telemetry_attrs_info(unit, &attrs_info));
    }
    field_number = attrs_info.field_num;

    /* Parse key field */
    for (i = 0; i < in->count; i++) {
        if (in->field[i]->id ==
            MON_INBAND_TELEMETRY_METADATA_FIELD_INFOt_MON_INBAND_TELEMETRY_METADATA_FIELD_INFO_IDf) {
            field_id = in->field[i]->data;
            break;
        }
    }
    if (i == in->count) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if ((field_id >= field_number) || ((field_id + 1) >= field_number)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        out->field[0]->id =
            MON_INBAND_TELEMETRY_METADATA_FIELD_INFOt_MON_INBAND_TELEMETRY_METADATA_FIELD_INFO_IDf;
        out->field[0]->data = field_id + 1;
        out->field[0]->idx = 0;

        SHR_IF_ERR_EXIT
            (bcmcth_mon_inband_telemetry_metadata_field_info_lookup(
                unit, op_arg, out, out, arg));
    }

exit:
    SHR_FUNC_EXIT();
}
