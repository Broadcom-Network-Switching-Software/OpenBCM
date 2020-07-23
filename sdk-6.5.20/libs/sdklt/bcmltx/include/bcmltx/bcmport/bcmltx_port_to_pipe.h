/*! \file bcmltx_port_to_pipe.h
 *
 * Port table's port to port pipe field Transform Handler
 *
 * This file contains field transform information for PORT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PORT_TO_PIPE_H
#define BCMLTX_PORT_TO_PIPE_H

#include <bcmltd/bcmltd_handler.h>

 /*!
 * \brief Port ID to pipe transform function.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in_key        Key input field values.
 * \param [in]  in_value      Value input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_port_to_pipe_rev_transform(int unit,
                                  const bcmltd_fields_t *in_key,
                                  const bcmltd_fields_t *in_value,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_PORT_TO_PIPE_H */
