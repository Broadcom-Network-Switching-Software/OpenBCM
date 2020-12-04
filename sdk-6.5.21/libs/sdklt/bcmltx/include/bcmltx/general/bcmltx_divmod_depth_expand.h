/*! \file bcmltx_divmod_depth_expand.h
 *
 * Transforms for depth expanded index tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_DIVMOD_DEPTH_EXPAND_H
#define BCMLTX_DIVMOD_DEPTH_EXPAND_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Divmod and depth expansion transform
 *
 * Index transform for tables that implement depth expansion.
 *
 * The index is divided by the number of tables;
 *   The quotient selects the table
 *   The remainder forms the physical table index.
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
bcmltx_divmod_depth_expand_transform(int unit,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg);
#endif /* BCMLTX_DIVMOD_DEPTH_EXPAND_H */
