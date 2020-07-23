/*! \file bcmcth_device_info.c
 *
 * Logical Table Custom Handler for DEVICE_INFO.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmcth/bcmcth_device_info.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_DEV


/*******************************************************************************
 * Public functions
 */

int
bcmcth_device_info_validate(int unit,
                            bcmlt_opcode_t opcode,
                            const bcmltd_fields_t *in,
                            const bcmltd_generic_arg_t *arg)
{
    if (opcode != BCMLT_OPCODE_LOOKUP) {
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

int
bcmcth_device_info_insert(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    return SHR_E_FAIL;
}

int
bcmcth_device_info_delete(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    return SHR_E_FAIL;
}

int
bcmcth_device_info_update(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    return SHR_E_FAIL;
}

int
bcmcth_device_info_lookup(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg)
{
    int fcnt;
    bcmdrd_dev_id_t id;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(arg);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_dev_id_get(unit, &id));

    fcnt = 0;
    out->field[fcnt]->id = DEVICE_INFOt_VENDOR_IDf;
    out->field[fcnt]->data = id.vendor_id;
    fcnt++;
    out->field[fcnt]->id = DEVICE_INFOt_DEV_IDf;
    out->field[fcnt]->data = id.device_id;
    fcnt++;
    out->field[fcnt]->id = DEVICE_INFOt_REV_IDf;
    out->field[fcnt]->data = id.revision;
    fcnt++;
    out->field[fcnt]->id = DEVICE_INFOt_MODELf;
    out->field[fcnt]->data = id.model;
    fcnt++;

    out->count = fcnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_device_info_first(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg)
{
    return bcmcth_device_info_lookup(unit, op_arg, NULL, out, arg);
}

int
bcmcth_device_info_next(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    return SHR_E_NOT_FOUND;
}

