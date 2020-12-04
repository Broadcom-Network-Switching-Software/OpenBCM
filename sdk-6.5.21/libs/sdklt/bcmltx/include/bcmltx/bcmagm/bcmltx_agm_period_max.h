/*! \file bcmltx_agm_period_max.h
 *
 * Egress Encap Control Entry Type Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_AGM_PERIOD_MAX_H
#define BCMLTX_AGM_PERIOD_MAX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief AGM_MONITOR_TABLE.PERIOD_MAX reverse transform
 *
 * \param [in]  unit    Unit number
 * \param [in]  in      AGM_MONITOR_TABLE.PERIOD_MAX
 * \param [out] out     MON_AGM.PERIOD_MAX
 * \param [in]  arg     Transform arguments.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmltx_agm_period_max_rev_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg);

/*!
 * \brief AGM_MONITOR_TABLE.PERIOD_MAX forward transform
 *
 * \param [in]  unit    Unit number
 * \param [in]  in      MON_AGM.PERIOD_MAX
 * \param [out] out     AGM_MONITOR_TABLE.PERIOD_MAX
 * \param [in]  arg     Transform arguments.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmltx_agm_period_max_transform(
    int unit,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_AGM_PERIOD_MAX_H */
