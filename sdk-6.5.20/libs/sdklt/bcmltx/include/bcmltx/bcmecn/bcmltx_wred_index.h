/*! \file bcmltx_wred_index.h
 *
 * ECN wred table's index Transform Handler
 *
 * This file contains field transform information for ECN.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_WRED_INDEX_H
#define BCMLTX_WRED_INDEX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * Local Defines.
 */

/*! Packet having green color. No drop */
#define BCM_XGS_COLOR_GREEN   0

/*! Yellow colored packets. probable drop*/
#define BCM_XGS_COLOR_YELLOW  3

/*! Red colored packets, Drop */
#define BCM_XGS_COLOR_RED     1

/*! No color for packet */
#define BCM_XGS_COLOR_NONE    0

/*!
 * \brief ecn index transform
 *
 * This function transforms table key into
 * table index.
 * Table key is designed in a way that if key is organised
 * in a particular sequence then index of table can be generated.
 * The inputs to key are ECN_CNG_ID, TM_CONGESTION and PKT_CNG.
 * All the inputs are packed in (in)put parameter of below API.
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
bcmltx_wred_index_transform(int unit,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_transform_arg_t *arg);

/*!
 * \brief ecn index reverse transform
 *
 * This function transforms table index back into table
 * key values.
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
bcmltx_wred_index_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_WRED_INDEX_H */
