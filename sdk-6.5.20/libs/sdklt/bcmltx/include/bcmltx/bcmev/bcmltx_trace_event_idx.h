/*! \file bcmltx_trace_event_idx.h
 *
 * IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COUNTER_0.__TABLE_SEL transform handler
 *
 * This file contains field transform information for
 * IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COUNTER_0.__TABLE_SEL.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_TRACE_EVENT_IDX_H
#define BCMLTX_TRACE_EVENT_IDX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COUNTER_0.__TABLE_SEL forward transform
 *
 * \param [in]  unit   Unit Number.
 * \param [in]  in     IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COUNTER_0.CTR_TRACE_EVENT_ID.
 * \param [out] out    IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COUNTER_0.__TABLE_SEL.
 * \param [in]  arg    Transform arguments.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltx_trace_event_idx_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg);

/*!
 * \brief IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COUNTER_0.__TABLE_SEL reverse transform
 *
 * \param [in]  unit   Unit Number.
 * \param [in]  in     IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COUNTER_0.__TABLE_SEL.
 * \param [out] out    IPOST_TRACE_DROP_EVENT_TRACE_EVENT_COUNTER_0.CTR_TRACE_EVENT_ID.
 * \param [in]  arg    Transform arguments.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmltx_trace_event_idx_rev_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_TRACE_EVENT_IDX_H */
