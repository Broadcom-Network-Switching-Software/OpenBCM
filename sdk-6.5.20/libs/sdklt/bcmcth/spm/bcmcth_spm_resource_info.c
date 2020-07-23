/*! \file bcmcth_spm_resource_info.c
 *
 * Logical Table Custom Handler for Strenth Profile *_RESOURCE_INFO.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <bcmptm/bcmptm_spm.h>


/*******************************************************************************
 * Public functions
 */

int
bcmcth_spm_resource_info_validate(int unit,
                                  bcmlt_opcode_t opcode,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg)
{
    return bcmptm_spm_resource_info_validate(unit, opcode, in, arg);
}

int
bcmcth_spm_resource_info_insert(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    return SHR_E_FAIL;
}

int
bcmcth_spm_resource_info_delete(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    return SHR_E_FAIL;
}

int
bcmcth_spm_resource_info_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    return SHR_E_FAIL;
}

int
bcmcth_spm_resource_info_lookup(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg)
{
    return bcmptm_spm_resource_info_lookup(unit, op_arg, in, out, arg);
}

int
bcmcth_spm_resource_info_first(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg)
{
    return bcmptm_spm_resource_info_first(unit, op_arg, in, out, arg);
}

int
bcmcth_spm_resource_info_next(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    return bcmptm_spm_resource_info_next(unit, op_arg, in, out, arg);
}

