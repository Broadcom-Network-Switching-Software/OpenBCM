/*! \file bcmltx_ldh_mon_latency_limit_select.h
 *
 * Latency monitor limit select handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_LDH_MON_LATENCY_LIMIT_SELECT_H
#define BCMLTX_LDH_MON_LATENCY_LIMIT_SELECT_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief MON_LDH_LATENCY_LIMIT.MON_LDH_INSTANCE/MON_LDH_QUEUE_INSTANCE/
 *                              MON_LDH_BUCKET_INSTANCE transform.
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
bcmltx_ldh_mon_latency_limit_select_transform(int unit,
                                              const bcmltd_fields_t *in,
                                              bcmltd_fields_t *out,
                                              const bcmltd_transform_arg_t *arg);

/*!
 * \brief MON_LDH_LATENCY_LIMIT.MON_LDH_INSTANCE/MON_LDH_QUEUE_INSTANCE/
 *                              MON_LDH_BUCKET_INSTANCE rev transform.
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
bcmltx_ldh_mon_latency_limit_select_rev_transform(int unit,
                                                  const bcmltd_fields_t *in,
                                                  bcmltd_fields_t *out,
                                                  const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_LDH_MON_LATENCY_LIMIT_SELECT_H */
