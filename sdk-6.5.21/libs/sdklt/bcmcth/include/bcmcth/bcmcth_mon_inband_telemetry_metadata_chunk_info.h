/*! \file bcmcth_mon_inband_telemetry_metadata_chunk_info.h
 *
 * This file contains inband telemetry metadata chunk handler
 * function declarations and macro definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_INBAND_TELEMETRY_METADATA_CHUNK_INFO_H
#define BCMCTH_MON_INBAND_TELEMETRY_METADATA_CHUNK_INFO_H

#include <bcmdrd_config.h>
#include <bcmltd/bcmltd_handler.h>


/*!
 * \brief Mon inband telemetry metadata chunk info validation
 *
 * Validate Mon inband telemetry metadata chunk info parameters.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_mon_inband_telemetry_metadata_chunk_info_validate(
    int unit,
    bcmlt_opcode_t opcode,
    const bcmltd_fields_t *in,
    const bcmltd_generic_arg_t *arg);

/*!
 * \brief Mon inband telemetry metadata chunk info insert.
 *
 * Insert into Mon inband telemetry metadata chunk info LT.
 * Invalid operation on read-only LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_mon_inband_telemetry_metadata_chunk_info_insert(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg);

/*!
 * \brief Mon inband telemetry metadata chunk info lookup.
 *
 * Lookup Mon inband telemetry metadata chunk info LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_mon_inband_telemetry_metadata_chunk_info_lookup(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg);

/*!
 * \brief Mon inband telemetry metadata chunk info delete.
 *
 * Delete Mon inband telemetry metadata chunk info.
 * Invalid operation on read-only LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_mon_inband_telemetry_metadata_chunk_info_delete(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg);

/*!
 * \brief Mon inband telemetry metadata chunk info update.
 *
 * Update Mon inband telemetry metadata chunk info.
 * Invalid operaton on read-only LT.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field value (not applicable).
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_mon_inband_telemetry_metadata_chunk_info_update(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg);

/*!
 * \brief Mon inband telemetry metadata chunk info table first.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values (not applicable).
 * \param [out] out           Output field values.
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_mon_inband_telemetry_metadata_chunk_info_first(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg);

/*!
 * \brief Mon inband telemetry metadata chunk info table next.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Custom table handler arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_mon_inband_telemetry_metadata_chunk_info_next(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out,
    const bcmltd_generic_arg_t *arg);

#endif /* BCMCTH_MON_INBAND_TELEMETRY_METADATA_CHUNK_INFO_H */
