/*! \file bcmltx_null.c
 *
 * NULL CTH handler
 *
 * Built-in Custom Table Handler which does nothing but return
 * the unavailable error code.  This handler is a default for
 * testing purposes.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmltx_null.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

int
bcmltx_null_validate(int unit,
                      bcmlt_opcode_t opcode,
                      const bcmltd_fields_t *in,
                      const bcmltd_generic_arg_t *arg)
{
    return SHR_E_UNAVAIL;
}

int
bcmltx_null_insert(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   const bcmltd_fields_t *in,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg)
{
    return SHR_E_UNAVAIL;
}

int
bcmltx_null_lookup(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   const bcmltd_fields_t *in,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg)
{
    return SHR_E_UNAVAIL;
}

int
bcmltx_null_delete(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   const bcmltd_fields_t *in,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg)
{
    return SHR_E_UNAVAIL;
}

int
bcmltx_null_update(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   const bcmltd_fields_t *in,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg)
{
    return SHR_E_UNAVAIL;
}

int
bcmltx_null_first(int unit,
                  const bcmltd_op_arg_t *op_arg,
                  const bcmltd_fields_t *in,
                  bcmltd_fields_t *out,
                  const bcmltd_generic_arg_t *arg)
{
    return SHR_E_UNAVAIL;
}

int
bcmltx_null_next(int unit,
                 const bcmltd_op_arg_t *op_arg,
                 const bcmltd_fields_t *in,
                 bcmltd_fields_t *out,
                 const bcmltd_generic_arg_t *arg)
{
    return SHR_E_UNAVAIL;
}

