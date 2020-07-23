/*! \file bcmcth_meter_pdd_internal.h
 *
 * This file contains FP Meter PDD Custom handler
 * data structures and macros.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_PDD_INTERNAL_H
#define BCMCTH_METER_PDD_INTERNAL_H

#include <shr/shr_bitop.h>

/*!
 * \brief FP meter PDD insert
 *
 * Insert an FP Meter PDD in the hardware PDD table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  sid           Logical table ID.
 * \param [in]  stage         FP stage.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  reason        IMM event reason.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_pdd_entry_insert(int unit,
                              bcmltd_sid_t sid,
                              int stage,
                              uint32_t trans_id,
                              bcmimm_entry_event_t reason,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data);

/*!
 * \brief FP meter PDD delete
 *
 * Delete an FP Meter PDD from the hardware PDD table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  stage         FP stage.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_meter_pdd_entry_delete(int unit,
                              int stage,
                              uint32_t trans_id,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data);

#endif /* BCMCTH_METER_PDD_INTERNAL_H */
