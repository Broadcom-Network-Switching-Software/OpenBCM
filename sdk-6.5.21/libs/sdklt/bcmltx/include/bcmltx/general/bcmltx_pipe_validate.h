/*! \file bcmltx_pipe_validate.h
 *
 * Pipe Validation Handler
 *
 * This file contains table handler for pipe validation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PIPE_VALIDATE_H
#define BCMLTX_PIPE_VALIDATE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Pipe validation.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid pipe.
 * \retval SHR_E_INTERNAL     Inernal errors.
 */
extern int
bcmltx_pipe_validate(int unit,
                     bcmlt_opcode_t opcode,
                     const bcmltd_fields_t *in,
                     const bcmltd_field_val_arg_t *arg);

#endif /* BCMLTX_PIPE_VALIDATE_H */
