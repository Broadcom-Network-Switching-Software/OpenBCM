/*! \file bcmltx_egr_encap_ctrl_entry_type.h
 *
 * Egress Encap Control Entry Type Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_EGR_ENCAP_CTRL_ENTRY_TYPE_H
#define BCMLTX_EGR_ENCAP_CTRL_ENTRY_TYPE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Egress Encap Control Entry Type fields transform
 *
 * This function transforms the Egress Encap Control Entry Type
 * into individual HW fields.
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
bcmltx_egr_encap_ctrl_entry_type_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg);

/*!
 * \brief Egress Encap Control Entry rev transform
 *
 * This function transforms the HW fields into
 * the Egress Encap Control Entry.
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
bcmltx_egr_encap_ctrl_entry_type_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_EGR_ENCAP_CTRL_ENTRY_TYPE_H */
