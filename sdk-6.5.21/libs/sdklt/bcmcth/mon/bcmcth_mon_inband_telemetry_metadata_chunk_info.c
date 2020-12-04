/*! \file bcmcth_mon_inband_telemetry_metadata_chunk_info.c
 *
 * Inband telemetry metadata chunk information.
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
#include <bcmcth/bcmcth_mon_inband_telemetry_metadata_chunk_info.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/******************************************************************************
 * Local definitions
 */


/*******************************************************************************
 * Public functions
 */


int
bcmcth_mon_inband_telemetry_metadata_chunk_info_validate(
    int unit,
    bcmlt_opcode_t opcode,
    const bcmltd_fields_t *in,
    const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    if (opcode != BCMLT_OPCODE_LOOKUP) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_inband_telemetry_metadata_chunk_info_insert(
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
bcmcth_mon_inband_telemetry_metadata_chunk_info_lookup(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg)
{
    size_t count = 0;
    bcmcth_mon_drv_t *mon_drv = bcmcth_mon_drv_get(unit);
    bcmcth_mon_inband_telemetry_metadata_attrs_t attrs_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out, SHR_E_PARAM);
    sal_memset(&attrs_info, 0,
        sizeof(bcmcth_mon_inband_telemetry_metadata_attrs_t));
    if (mon_drv && mon_drv->telemetry_attrs_info) {
        SHR_IF_ERR_EXIT(mon_drv->telemetry_attrs_info(unit, &attrs_info));
    }

    out->field[count]->id =
        MON_INBAND_TELEMETRY_METADATA_CHUNK_INFOt_WIDE_SIZEf;
    out->field[count]->data = attrs_info.wide_size;
    out->field[count]->idx = 0;
    count++;

    out->field[count]->id =
        MON_INBAND_TELEMETRY_METADATA_CHUNK_INFOt_NARROW_SIZEf;
    out->field[count]->data = attrs_info.narrow_size;
    out->field[count]->idx = 0;
    count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_inband_telemetry_metadata_chunk_info_delete(
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
bcmcth_mon_inband_telemetry_metadata_chunk_info_update(
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
bcmcth_mon_inband_telemetry_metadata_chunk_info_first(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_mon_inband_telemetry_metadata_chunk_info_lookup(
            unit, op_arg, out, out, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_inband_telemetry_metadata_chunk_info_next(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}
