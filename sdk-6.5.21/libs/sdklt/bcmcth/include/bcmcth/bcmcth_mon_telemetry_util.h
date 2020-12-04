/*! \file bcmcth_mon_telemetry_util.h
 *
 * MON telemetry utility function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_TELEMETRY_UTIL_H
#define BCMCTH_MON_TELEMETRY_UTIL_H

/*******************************************************************************
 * Includes
 */
#include <bcmcth/bcmcth_mon_telemetry_drv.h>
#include <bcmcth/bcmcth_mon_telemetry_msg.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmdrd/bcmdrd_feature.h>

/*! Get the minimum supported value of an LT field. */
#define TELEMETRY_FIELD_MIN(fld)   ((fld.dtag == BCMLT_FIELD_DATA_TAG_U8) ?    \
                                      (fld.min.u8) :                           \
                                      ((fld.dtag == BCMLT_FIELD_DATA_TAG_U16) ?\
                                      (fld.min.u16) :                          \
                                      ((fld.dtag == BCMLT_FIELD_DATA_TAG_U32) ?\
                                      (fld.min.u32) :                          \
                                      ((fld.dtag == BCMLT_FIELD_DATA_TAG_U64) ?\
                                      (fld.min.u64) :                          \
                                      (fld.min.is_true)))))

/*! Get the maximum supported value of an LT field. */
#define TELEMETRY_FIELD_MAX(fld)   ((fld.dtag == BCMLT_FIELD_DATA_TAG_U8) ?    \
                                      (fld.max.u8) :                           \
                                      ((fld.dtag == BCMLT_FIELD_DATA_TAG_U16) ?\
                                      (fld.max.u16) :                          \
                                      ((fld.dtag == BCMLT_FIELD_DATA_TAG_U32) ?\
                                      (fld.max.u32) :                          \
                                      ((fld.dtag == BCMLT_FIELD_DATA_TAG_U64) ?\
                                      (fld.max.u64) :                          \
                                      (fld.max.is_true)))))

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Utility function to retrive the telemetry control data.
 *
 * \param [in] unit Unit number.
 * \param [in] data input data fields.
 * \param [out] entry entry with telemetry control data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
mon_telemetry_control_lt_fields_parse(int unit,
    const bcmltd_field_t *data,
    bcmcth_mon_telemetry_control_t *entry);

/*!
 * \brief Utility function to retrive the telemetry object data.
 *
 * \param [in] unit Unit number.
 * \param [in] key input key field.
 * \param [in] data input data fields.
 * \param [out] entry entry with telemetry object data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
mon_telemetry_object_lt_fields_parse(int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    bcmcth_mon_telemetry_object_t *entry);

/*!
 * \brief Utility function to retrive the telemetry instance data.
 *
 * \param [in] unit Unit number.
 * \param [in] key input key field.
 * \param [in] data input data fields.
 * \param [out] entry entry with telemetry instance data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
mon_telemetry_instance_lt_fields_parse(int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    bcmcth_mon_telemetry_instance_t *entry);

/*!
 * \brief Get MON Telemetry field array handle.
 *
 * \param [in] unit Unit number.
 * \param [out] hdl Field array handle.
 */
extern int
bcmcth_mon_telemetry_fld_arr_hdl_get(int unit,
                                        shr_famm_hdl_t *hdl);

/*!
 * \brief Utility function to retrive data fields linked list from array fields.
 *
 * \param [in] unit Unit number.
 * \param [out] data updated data fields linked list pointer
 * \param [in] out_flds fields array pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_telemetry_data_from_outfields(
    int unit,
    bcmltd_field_t *data,
    const bcmltd_fields_t *out_flds);

/*!
 * \brief Utility function to retrive Telemetry Instance LT data fields count.
 *
 * \param [in] unit Unit number.
 * \param [out] count Number of data fields in Telemetry Instance LT
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_telemetry_instance_out_fields_count(
    int unit,
    uint32_t *count);
#endif /* BCMCTH_MON_TELEMETRY_UTIL_H */
