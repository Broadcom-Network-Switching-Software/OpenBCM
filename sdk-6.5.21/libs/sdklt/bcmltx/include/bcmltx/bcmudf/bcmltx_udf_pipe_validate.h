/*! \file bcmltx_udf_pipe_validate.h
 *
 * UDF validator.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_UDF_PIPE_VALIDATE_H
#define BCMLTX_UDF_PIPE_VALIDATE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief UDF PIPE Field Validation Function.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          Table opcode.
 * \param [in]  in              LTD input field set.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_udf_pipe_validate(int unit,
                         bcmlt_opcode_t opcode,
                         const bcmltd_fields_t *in,
                         const bcmltd_field_val_arg_t *arg);

#endif /* BCMLTX_UDF_PIPE_VALIDATE_H */
