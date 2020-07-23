/*! \file bcmcth_ldh_device_info.c
 *
 * LDH device information.
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
#include <bcmcth/bcmcth_ldh_drv.h>
#include <bcmcth/bcmcth_ldh_device_info.h>

#include "bcmcth_ldh_internal.h"

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_LDH

/******************************************************************************
 * Local definitions
 */

/* BCMCTH LDH device information */
static bcmcth_ldh_device_info_t bcmcth_ldh_device_info[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcmcth_ldh_device_info_init(int unit)
{
    /* Get device-specific data */
    return bcmcth_ldh_device_info_get(unit, &bcmcth_ldh_device_info[unit]);
}

int
bcmcth_ldh_device_info_validate(int unit,
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
bcmcth_ldh_device_info_insert(int unit,
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
bcmcth_ldh_device_info_lookup(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    size_t count = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->field[count]->id = MON_LDH_DEVICE_INFOt_LATENCY_LIMIT_QUANTAf;
    out->field[count]->data =
        bcmcth_ldh_device_info[unit].latency_limit_quanta;
    out->field[count]->idx = 0;
    count++;

    out->field[count]->id = MON_LDH_DEVICE_INFOt_COUNTER_INCREMENT_MULTIPLIERf;
    out->field[count]->data =
        bcmcth_ldh_device_info[unit].histo_counter_incr_multiplier;
    out->field[count]->idx = 0;
    count++;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_device_info_delete(int unit,
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
bcmcth_ldh_device_info_update(int unit,
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
bcmcth_ldh_device_info_first(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_ldh_device_info_lookup(unit, op_arg, out, out, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ldh_device_info_next(int unit,
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
