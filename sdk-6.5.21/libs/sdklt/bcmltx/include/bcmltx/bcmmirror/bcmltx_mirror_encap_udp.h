/*! \file bcmltx_mirror_encap_udp.h
 *
 * Mirror Encap UDP Transform Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_MIRROR_ENCAP_UDP_H
#define BCMLTX_MIRROR_ENCAP_UDP_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Mirror Encap UDP fields transform
 *
 * This function transforms the Mirror Encap UDP Config
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
bcmltx_mirror_encap_udp_rev_transform (int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);

/*!
 * \brief Mirror Encap UDP config fields rev transform
 *
 * This function transforms the HW fields into
 * the Mirror Encap UDP Config.
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
bcmltx_mirror_encap_udp_transform (int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_MIRROR_ENCAP_UDP_H */
