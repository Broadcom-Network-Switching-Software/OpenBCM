/*! \file bcmltx_mirror_encap_vlan_tag.h
 *
 * Mirror Encap VLAN TAG Transform Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_MIRROR_ENCAP_VLAN_TAG_H
#define BCMLTX_MIRROR_ENCAP_VLAN_TAG_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Mirror Encap VLAN TAG fields transform
 *
 * This function transforms the Mirror Encap VLAN TAG
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
bcmltx_mirror_encap_vlan_tag_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);

/*!
 * \brief Mirror Encap VLAN TAG fields rev transform
 *
 * This function transforms the HW fields into
 * the Mirror Encap VLAN TAG.
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
bcmltx_mirror_encap_vlan_tag_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_MIRROR_ENCAP_VLAN_TAG_H */
