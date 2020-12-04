/*! \file bcmltx_port_pt_suppress.h
 *
 * TM table's array to bitmap field Transform Handler
 *
 * This file contains field transform information for TM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PORT_PT_SUPPRESS_H
#define BCMLTX_PORT_PT_SUPPRESS_H

#include <bcmltd/bcmltd_handler.h>

 /*!
 * \brief TM logical port number to MMU port number transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_port_pt_suppress_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg);


#endif /* BCMLTX_PORT_PT_SUPPRESS_H */
