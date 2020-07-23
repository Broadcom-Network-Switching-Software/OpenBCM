/*! \file bcmltx_wred_pipe_sp_field.h
 *
 * APIs, definitions for WRED service pool functions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_WRED_PIPE_SP_FIELD_H
#define BCMLTX_WRED_PIPE_SP_FIELD_H

/*******************************************************************************
 INCLUDES
 */
#include <bcmltd/bcmltd_handler.h>


/*!
 * \brief WRED PORT SP field forward transform function
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
bcmltx_wred_pipe_sp_field_transform (int unit,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_transform_arg_t *arg);

/*!
 * \brief WRED PORT SP field reverse transform function
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
bcmltx_wred_pipe_sp_field_rev_transform (int unit,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_transform_arg_t *arg);


#endif /* BCMLTX_WRED_PIPE_SP_FIELD_H */
