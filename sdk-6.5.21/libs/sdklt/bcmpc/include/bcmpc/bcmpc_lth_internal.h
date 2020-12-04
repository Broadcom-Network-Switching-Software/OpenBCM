/*! \file bcmpc_lth_internal.h
 *
 * BCMPC Logical Table Handler (LTH) Utilities.
 *
 * Declaration of the LTH utilities which are used by BCMPC internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_LTH_INTERNAL_H
#define BCMPC_LTH_INTERNAL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Add a field to the given field array.
 *
 * \param [in] _flds Field array, refer to bcmltd_fields_t.
 * \param [in] _fid Field ID, refer to bcmltd_field_t.
 * \param [in] _fidx Field Index, refer to bcmltd_field_t.
 * \param [in] _fdata Field data, refer to bcmltd_field_t.
 * \param [in,out] _fcnt Fields count. The \c _fcnt will increase 1 after the
 *                       field is added.
 */
#define BCMPC_LTH_FIELD_ADD(_flds, _fid, _fidx, _fdata, _fcnt) \
            do { \
                _flds->field[_fcnt]->id = _fid; \
                _flds->field[_fcnt]->idx = _fidx; \
                _flds->field[_fcnt]->data = (uint64_t)_fdata; \
                _fcnt++; \
            } while (0);

/*!
 * \brief Get the value of a field from input array.
 *
 * \param [in] unit Unit number.
 * \param [in] in LTA input field array.
 * \param [in] fid Field ID.
 * \param [out] fval Field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
extern int
bcmpc_lth_field_get(int unit, const bcmltd_fields_t *in, uint32_t fid,
                    uint64_t *fval);

/*!
 * \brief Get the minimum and maximum value of the given field.
 *
 * This function will get the minimum and maximum value of a LT field
 * from the LRD field definition.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fid Table Field.
 * \param [out] min Minimum field value.
 * \param [out] max Maximum field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the field definition.
 */
int
bcmpc_lth_field_value_range_get(int unit, uint32_t sid, uint32_t fid,
                                uint64_t *min, uint64_t *max);

/*!
 * \brief Validate the value of the given field.
 *
 * This function uses the LRD field definition to validate the input field
 * value.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field Table Field.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Some or all field values are invalid.
 */
extern int
bcmpc_lth_field_validate(int unit, uint32_t sid, const bcmltd_field_t *field);

/*!
 * \brief Validate the values of the input field array.
 *
 * This function uses the LRD field definition to validate the input field
 * values.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] in LTA input field array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Some or all field values are invalid.
 */
extern int
bcmpc_lth_fields_validate(int unit, uint32_t sid, const bcmltd_fields_t *in);

#endif /* BCMPC_LTH_INTERNAL_H */
