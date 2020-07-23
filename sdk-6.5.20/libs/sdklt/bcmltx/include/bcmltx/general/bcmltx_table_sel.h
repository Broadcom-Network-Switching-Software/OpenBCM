/*! \file bcmltx_table_sel.h
 *
 * Select a table based on input.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_TABLE_SEL_H
#define BCMLTX_TABLE_SEL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Table select transform
 *
 * Select the table (or any one to one PT field) based on input.
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
bcmltx_table_sel_transform(int unit,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_transform_arg_t *arg);

/*!
 * \brief Table select reverse transform
 *
 * Select the LT field from the PT input.
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
bcmltx_table_sel_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_TABLE_SEL_H */
