/*! \file bcmfp_egr_action_cont_info.c
 *
 * APIs for FP_EGR_ACTION_CONT_INFO template LTs custom handler.
 *
 * This file contains functions to insert, update,
 * delete, lookup or validate group config provided
 * using group template LTs.
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
#include <shr/shr_util.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_tbl_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/bcmfp_cth_pdd.h>
#include <bcmfp/bcmfp_grp_selcode.h>
#include <bcmfp/bcmfp_cth_filter.h>
#include <bcmltd/chip/bcmltd_fp_constants.h>
#include <bcmfp/bcmfp_cth_info.h>
#include <bcmfp/bcmfp_egr_action_cont_info.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_egr_action_cont_info_lookup(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_generic_arg_t *arg)
{
    bcmfp_stage_t *stage = NULL;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, BCMFP_STAGE_ID_EGRESS , &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_action_cont_info_lookup(unit,
                                       op_arg,
                                       stage,
                                       in,
                                       out));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_egr_action_cont_info_insert(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_generic_arg_t *arg)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    return SHR_E_FAIL;
}

int
bcmfp_egr_action_cont_info_validate(int unit,
                                    bcmlt_opcode_t opcode,
                                    const bcmltd_fields_t *in,
                                    const bcmltd_generic_arg_t *arg)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(opcode);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    return SHR_E_NONE;
}


int
bcmfp_egr_action_cont_info_delete(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_generic_arg_t *arg)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    return SHR_E_FAIL;
}

int
bcmfp_egr_action_cont_info_update(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_generic_arg_t *arg)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(op_arg);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(out);
    COMPILER_REFERENCE(arg);

    return SHR_E_FAIL;
}

int
bcmfp_egr_action_cont_info_first(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(in);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_action_cont_info_first_search(unit,
                                             op_arg,
                                             BCMFP_STAGE_ID_EGRESS,
                                             out,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_egr_action_cont_info_next(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_action_cont_info_next_search(unit,
                                            op_arg,
                                            BCMFP_STAGE_ID_EGRESS,
                                            in,
                                            out,
                                            arg));
exit:
    SHR_FUNC_EXIT();
}

