/*! \file bcmltx_port_pg_pipe_idx.h
 *
 * TM local port, priority group to pipe and index calculation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PORT_PG_PIPE_IDX_H
#define BCMLTX_PORT_PG_PIPE_IDX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Port and priority group to pipe and table index transform.
 *
 * \param [in]  unit          Unit Number
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_port_pg_pipe_idx_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg);

/*!
 * \brief Port and priority group to pipe and table index reverse transform.
 *
 * \param [in]  unit          Unit Number
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_port_pg_pipe_idx_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg);


#endif /* BCMLTX_PORT_PG_PIPE_IDX_H */
