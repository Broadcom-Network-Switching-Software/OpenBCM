/*! \file bcmltx_l2_opaque_tag_id.h
 *
 * L2_OPAQUE_TAG Logic table Handler Header file.
 * This file contains field transform information for
 * opaque tag id in L2_OPAQUE_TAG Logic table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_L2_OPAQUE_TAG_ID_H
#define BCMLTX_L2_OPAQUE_TAG_ID_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief opaque tag id transform
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
bcmltx_l2_opaque_tag_id_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg);

/*!
 * \brief opaque tag id reverse transform
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
bcmltx_l2_opaque_tag_id_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_L2_OPAQUE_TAG_ID_H */
