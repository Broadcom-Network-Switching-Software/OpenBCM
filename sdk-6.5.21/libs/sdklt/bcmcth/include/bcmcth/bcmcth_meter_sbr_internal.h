/*! \file bcmcth_meter_sbr_internal.h
 *
 * This file contains FP Meter SBR Custom handler
 * data structures and macros.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_SBR_INTERNAL_H
#define BCMCTH_METER_SBR_INTERNAL_H

#include <shr/shr_bitop.h>

/*!
 * \brief FP meter SBR insert
 *
 * Insert an FP Meter SBR in the hardware SBR table.
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
bcmcth_meter_sbr_entry_insert(int unit,
                              int stage,
                              uint32_t trans_id,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data);

/*!
 * \brief FP meter SBR delete
 *
 * Delete an FP Meter SBR from the hardware SBR table.
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
bcmcth_meter_sbr_entry_delete(int unit,
                              int stage,
                              uint32_t trans_id,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data);

#endif /* BCMCTH_METER_SBR_INTERNAL_H */
