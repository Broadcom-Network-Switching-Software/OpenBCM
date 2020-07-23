/*! \file bcmcth_meter_util.h
 *
 * This file contains FP Meter Custom handler
 * data structures and macros.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_UTIL_H
#define BCMCTH_METER_UTIL_H

#include <shr/shr_bitop.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmltd/bcmltd_types.h>

/*! Ingress FP stage. */
#define METER_FP_STAGE_ID_INGRESS           0
/*! Egress FP stage. */
#define METER_FP_STAGE_ID_EGRESS            1

/*! FP mode global. */
#define METER_FP_OPER_MODE_GLOBAL        0
/*! FP mode pipe unique. */
#define METER_FP_OPER_MODE_PIPE_UNIQUE   1


/*! Get the default value of an LT field. */
#define METER_FP_FIELD_DEF(fld)   ((fld.dtag == BCMLT_FIELD_DATA_TAG_U8) ?       \
                                      (fld.def.u8) :                                \
                                      ((fld.dtag == BCMLT_FIELD_DATA_TAG_U16) ?     \
                                       (fld.def.u16) :                              \
                                       ((fld.dtag == BCMLT_FIELD_DATA_TAG_U32) ?    \
                                        (fld.def.u32) :                             \
                                        ((fld.dtag == BCMLT_FIELD_DATA_TAG_U64) ?   \
                                         (fld.def.u64) :                            \
                                         (fld.def.is_true)))))

/*! Get the minimum supported value of an LT field. */
#define METER_FP_FIELD_MIN(fld)   ((fld.dtag == BCMLT_FIELD_DATA_TAG_U8) ?       \
                                      (fld.min.u8) :                                \
                                      ((fld.dtag == BCMLT_FIELD_DATA_TAG_U16) ?     \
                                       (fld.min.u16) :                              \
                                       ((fld.dtag == BCMLT_FIELD_DATA_TAG_U32) ?    \
                                        (fld.min.u32) :                             \
                                        ((fld.dtag == BCMLT_FIELD_DATA_TAG_U64) ?   \
                                         (fld.min.u64) :                            \
                                         (fld.min.is_true)))))

/*! Get the maximum supported value of an LT field. */
#define METER_FP_FIELD_MAX(fld)   ((fld.dtag == BCMLT_FIELD_DATA_TAG_U8) ?       \
                                      (fld.max.u8) :                                \
                                      ((fld.dtag == BCMLT_FIELD_DATA_TAG_U16) ?     \
                                       (fld.max.u16) :                              \
                                       ((fld.dtag == BCMLT_FIELD_DATA_TAG_U32) ?    \
                                        (fld.max.u32) :                             \
                                        ((fld.dtag == BCMLT_FIELD_DATA_TAG_U64) ?   \
                                         (fld.max.u64) :                            \
                                         (fld.max.is_true)))))

/*!
 * \brief FP Meter hardware parameters structure.
 *
 * LT fields of FP meters are converted to hardware table fields
 * and stored in this structure.
 */
typedef struct bcmcth_meter_fp_policy_fields_s {
    /*! Meter pair index. */
    uint32_t meter_pair_index;
    /*! Meter mode. */
    uint8_t meter_pair_mode;
    /*! Meter mode modifier. */
    uint8_t mode_modifier;
    /*! Meter update odd. */
    uint8_t meter_update_odd;
    /*! Meter test odd. */
    uint8_t meter_test_odd;
    /*! Meter update even. */
    uint8_t meter_update_even;
    /*! Meter test even. */
    uint8_t meter_test_even;
    /*! Meter pipe ID. */
    uint8_t pipe;
} bcmcth_meter_fp_policy_fields_t;

/*!
 * \brief FP Meter event data structure.
 *
 * Data structure used to carry information
 * pertaining to the event.
 */
typedef struct bcmcth_meter_fp_event_info_s {
    /*! Meter index. */
    uint32_t meter_id;
    /*! Transaction id. */
    uint32_t trans_id;
} bcmcth_meter_fp_event_info_t;

/*!
 * \brief Get meter field value.
 *
 * Parse user input and get the field corresponding to
 * a particular fid.
 *
 * \param [in] unit Unit number.
 * \param [in] in Input LT field list.
 * \param [in] fid Lt field id
 * \param [out] rval Pointer to return field value
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
int
bcmcth_meter_entry_field_value_get(int unit,
                                   const bcmltd_fields_t *in,
                                   uint32_t fid,
                                   uint32_t *rval);

/*!
 * \brief Get min value of an LT field.
 *
 * \param [in] fld_def LT field attributes.
 * \param [out] data Output data
 *
 * \retval SHR_E_NONE No errors.
 */
void
meter_field_min_val_get(bcmlrd_field_def_t *fld_def,
                        uint64_t *data);
/*!
 * \brief Get max value of an LT field.
 *
 * \param [in] fld_def LT field attributes.
 * \param [out] data Output data
 *
 * \retval SHR_E_NONE No errors.
 */
void
meter_field_max_val_get(bcmlrd_field_def_t *fld_def,
                        uint64_t *data);
/*!
 * \brief Get default value of an LT field.
 *
 * \param [in] fld_def LT field attributes.
 * \param [out] data Output data
 *
 * \retval SHR_E_NONE No errors.
 */
void
meter_field_def_val_get(bcmlrd_field_def_t *fld_def,
                        uint64_t *data);

#endif /* BCMCTH_METER_UTIL_H */
