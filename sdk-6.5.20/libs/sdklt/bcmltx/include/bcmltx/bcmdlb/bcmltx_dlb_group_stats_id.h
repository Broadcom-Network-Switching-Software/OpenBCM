/*! \file bcmltx_dlb_group_stats_id.h
 *
 * DLB_ECMP/LAG_GROUP_STATS transform utilities for DLB_ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_DLB_GROUP_STATS_ID_H
#define BCMLTX_DLB_GROUP_STATS_ID_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief DLB group statistic id transform.
 *
 * \param [in]  unit Unit number.
 * \param [in]  in   DLB_ID field.
 * \param [out] out  Hardware entry index.
 * \param [in]  arg  Handler arguments.
 *
 * Transform direction is logical to physical.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_dlb_group_stats_id_transform(int unit,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_DLB_GROUP_STATS_ID_H */
