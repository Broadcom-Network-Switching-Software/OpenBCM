/*! \file bcmcth_tnl_decap.c
 *
 * TNL_MPLS_DECAP/TNL_MPLS_DECAP_TRUNK Custom Handler
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
  Includes
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmcth/bcmcth_tnl_decap_internal.h>

#include <bcmcth/bcmcth_tnl_decap.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_TNL

/*******************************************************************************
 * Public functions
 */

int
bcmcth_tnl_decap_validate(int unit,
                          bcmlt_opcode_t opcode,
                          const bcmltd_fields_t *in,
                          const bcmltd_generic_arg_t *arg)
{
    int rv = SHR_E_NONE;
    bcmcth_tnl_decap_drv_t  *drv = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmcth_tnl_decap_drv_get(unit, &drv);
    if (SHR_FAILURE(rv) || (drv == NULL)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_tnl_decap_mpls_validate(unit, opcode, arg->sid, in));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_tnl_decap_mpls_insert(unit, op_arg, arg->sid, in));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_lookup(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_tnl_decap_mpls_lookup(unit, op_arg,
                                     in, arg->sid, out));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_delete(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_tnl_decap_mpls_delete(unit, op_arg,
                                     arg->sid, in));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(out);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_tnl_decap_mpls_update(unit, op_arg,
                                     arg->sid, in));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_first(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg)
{
    int rv = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    rv = bcmcth_tnl_decap_mpls_traverse(unit, op_arg,
                                        NULL, arg->sid, out);

    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_tnl_decap_next(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg)
{
    int rv = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(arg);

    rv = bcmcth_tnl_decap_mpls_traverse(unit, op_arg,
                                        in, arg->sid, out);

    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT();
}
