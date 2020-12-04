/*! \file bcmltx_l2_eif_dest.h
 *
 * L2_EIF_SYSTEM_DESTINATION.IS_TRUNK/SYSTEM_PORT/TRUNK_ID Transform Handler
 *
 * This file contains field transform information for
 * L2_EIF_SYSTEM_DESTINATION.IS_TRUNK/SYSTEM_PORT/TRUNK_ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_L2_EIF_DEST_H
#define BCMLTX_L2_EIF_DEST_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief L2_EIF_SYSTEM_DESTINATION.IS_TRUNK/SYSTEM_PORT/TRUNK_ID transform.
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
bcmltx_l2_eif_dest_transform(int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg);

/*!
 * \brief L2_EIF_SYSTEM_DESTINATION.IS_TRUNK/SYSTEM_PORT/TRUNK_ID rev transform.
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
bcmltx_l2_eif_dest_rev_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_L2_EIF_DEST_H */
