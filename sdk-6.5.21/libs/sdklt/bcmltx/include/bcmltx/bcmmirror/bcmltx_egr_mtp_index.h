/*! \file bcmltx_egr_mtp_index.h
 *
 * Egress Mirror-to-Port(MTP) Index Transform Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_EGR_MTP_INDEX_H
#define BCMLTX_EGR_MTP_INDEX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Egress MTP Index fields transform
 *
 * This function transforms the Egress MTP Index
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
bcmltx_egr_mtp_index_rev_transform (int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);

/*!
 * \brief Egress MTP Index fields rev transform
 *
 * This function transforms the HW fields into
 * the Egress MTP Index.
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
bcmltx_egr_mtp_index_transform (int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_EGR_MTP_INDEX_H */
