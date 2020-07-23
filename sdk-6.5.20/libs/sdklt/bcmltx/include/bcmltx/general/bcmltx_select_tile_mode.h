/*! \file bcmltx_select_tile_mode.h
 *
 * Copy a field from input to output.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_SELECT_TILE_MODE_H
#define BCMLTX_SELECT_TILE_MODE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Select tile mode transform
 *
 * Select tile mode from given tile in first transform_arg
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
bcmltx_select_tile_mode_transform(int unit,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_transform_arg_t *arg);

/*!
 * \brief Tile mode reverse transform
 *
 * Select tile mode from given tile in first transform_arg
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
bcmltx_select_tile_mode_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_SELECT_TILE_MODE_H */
