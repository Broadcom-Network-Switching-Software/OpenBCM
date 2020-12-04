/*! \file field_destination_int.h
 *
 * Field Destination internal header file.
 * This file contains Field Destination internal to
 * the Field Destination module itself.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef BCMINT_LTSW_FIELD_DESTINATION_INT_H
#define BCMINT_LTSW_FIELD_DESTINATION_INT_H

#include <sal/sal_types.h>

/*!
 * \brief Get scalar value from symbol.
 *
 * \param [in] unit Unit Number.
 * \param [in] symbol symbol String
 * \param [in] val scalar value
 */
typedef int (*field_destination_symbol_to_scalar_f)(
    int unit,
    const char *symbol,
    uint64_t *val);

/*!
 * \brief Get scalar value from symbol.
 *
 * \param [in] unit Unit Number.
 * \param [in] val scalar value
 * \param [in] symbol symbol String
 */
typedef int (*field_destination_scalar_to_symbol_f)(
    int unit,
    uint64_t val,
    const char **symbol);

/*!
 * \brief Field destination LT field description.
 */
typedef struct bcmint_field_destination_fld_s {

    /*! LT field name. */
    const char *name;

    /*! Callback to get scalar value from symbol. */
    field_destination_symbol_to_scalar_f symbol_to_scalar;

    /*! Callback to get symbol from scalar value. */
    field_destination_scalar_to_symbol_f scalar_to_symbol;

} bcmint_field_destination_fld_t;

/*!
 * \brief DESTINATION_FP_TABLE LT fields.
 */
typedef enum bcmint_field_destination_fld_destination_fp_table_s {

    /*! DROP_CODE. */
    BCMINT_LTSW_FIELD_DEST_FLD_DFP_TBL_DROP_CODE = 0,

    /*! DROP_CODE_MASK. */
    BCMINT_LTSW_FIELD_DEST_FLD_DFP_TBL_DROP_CODE_MASK = 1,

    /*! DESTINATION_FP_TABLE field count. */
    BCMINT_LTSW_FIELD_DEST_FLD_DFP_TBL_FLD_CNT = 2

} bcmint_field_destination_fld_destination_fp_table_t;

/*!
 * \brief Field Destination LT description.
 */
typedef struct bcmint_field_destination_lt_s {

    /*! LT name. */
    const char *name;

    /*! Bitmap of valid fields. */
    uint64_t fld_bmp;

    /*! LT fields. */
    const bcmint_field_destination_fld_t *flds;

} bcmint_field_destination_lt_t;

/*!
 * \brief Field Destination LTs.
 */
typedef enum bcmint_field_destination_lt_id_s {

    /*! LT DESTINATION_FP_TABLE. */
    BCMINT_LTSW_FIELD_DEST_LT_DESTINATION_FP_TABLE = 0,

    /*! Field Destination LT count. */
    BCMINT_LTSW_FIELD_DEST_LT_CNT = 1

} bcmint_field_destination_lt_id_t;

/*!
 * \brief Field Destination LT field description.
 */
typedef struct bcmint_field_destination_lt_db_s {

    /*! LT bitmap. */
    uint32_t lt_bmp;

    /*! LT array. */
    const bcmint_field_destination_lt_t *lts;

} bcmint_field_destination_lt_db_t;

/*!
 * \brief Field Destination map.
 */
typedef struct bcmint_field_destination_map_s {

    /*! Scalar value. */
    int value;

    /*! Enumeration. */
    int type;

} bcmint_field_destination_map_t;

/*!
 * \brief Get Field Destination LT info with the given LT ID.
 *
 * \param [in] unit Unit Number.
 * \param [in] lt_id LT ID.
 * \param [out] lt_info LT info.
 */
extern int
bcmint_field_destination_lt_get(
    int unit,
    bcmint_field_destination_lt_id_t lt_id,
    const bcmint_field_destination_lt_t **lt_info);

/*!
 * \brief Transform BCM drop reason to hardware drop code.
 *
 * \param [in] unit Unit number.
 * \param [in] reason BCM drop reason.
 * \param [out] code Hardware drop code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmint_field_destination_drop_reason_to_code(
    int unit,
    bcm_field_drop_reason_t reason,
    int *code);

/*!
 * \brief Transform hardware drop code to BCM drop reason.
 *
 * \param [in] unit Unit number.
 * \param [in] code Hardware drop code.
 * \param [out] reason BCM drop reason.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmint_field_destination_drop_code_to_reason(
    int unit,
    int code,
    bcm_field_drop_reason_t *reason);

#endif /* BCMINT_LTSW_FIELD_DESTINATION_INT_H */
