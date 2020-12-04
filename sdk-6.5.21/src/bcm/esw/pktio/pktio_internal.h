/*! \file pktio_internal.h
 *
 * PKTIO internal.
 *
 * This file contains functions shared across pktio
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PKTIO_INTERNAL_H_
#define _PKTIO_INTERNAL_H_

#include <bcm/pktio.h>

/*!
 * \brief Get HG2 pktio field support bitmap.
 *
 * \param [in] unit Unit Number.
 * \param [out] support Field ID supported status bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_esw_pktio_hg2_fid_support_get(
    int unit,
    bcm_pktio_bitmap_t *support);

/*!
 * \brief Get HG2 field.
 *
 * \param [in] unit Unit Number.
 * \param [in] hghdr HG2 header.
 * \param [in] fid Field ID.
 * \param [out] val Field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_esw_pktio_hg2_field_get(
    int unit,
    uint32_t *hghdr,
    int fid,
    uint32_t *val);


/*!
 * \brief Set HG2 field.
 *
 * \param [in] unit Unit Number.
 * \param [in] hghdr HG2 header.
 * \param [in] fid Field ID.
 * \param [in] val Field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_esw_pktio_hg2_field_set(
    int unit,
    uint32_t *hghdr,
    int fid,
    uint32_t val);

#endif /* _PKTIO_INTERNAL_H_ */
