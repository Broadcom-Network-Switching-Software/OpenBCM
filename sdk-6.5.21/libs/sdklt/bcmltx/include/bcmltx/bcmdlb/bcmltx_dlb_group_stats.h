/*! \file bcmltx_dlb_group_stats.h
 *
 * DLB_ECMP/LAG_GROUP_STATS transform utilities.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_DLB_GROUP_STATS_H
#define BCMLTX_DLB_GROUP_STATS_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief DLB group statistic forward transform.
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
bcmltx_dlb_group_stats_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg);

/*!
 * \brief DLB group status reverse transform.
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
bcmltx_dlb_group_stats_rev_transform(int unit,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_DLB_GROUP_STATS_H */
