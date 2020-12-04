/*! \file bcmltx_ctr_group_stats.h
 *
 * Ingress and egress drop count event transform utilities.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_CTR_GROUP_STATS_H
#define BCMLTX_CTR_GROUP_STATS_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Drop event group statistic forward transform.
 *
 * \param [in]  unit Unit number.
 * \param [in]  in   Count field.
 * \param [out] out  Hardware statistic fields.
 * \param [in]  arg  Pipe number.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_ctr_group_stats_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg);

/*!
 * \brief Drop event group status reverse transform.
 *
 * \param [in]  unit Unit number.
 * \param [in]  in   Hardware statistic fields.
 * \param [out] out  Count field.
 * \param [in]  arg  Pipe number.
 *
 * Transform direction is physical to logical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_ctr_group_stats_rev_transform(int unit,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_CTR_GROUP_STATS_H */
