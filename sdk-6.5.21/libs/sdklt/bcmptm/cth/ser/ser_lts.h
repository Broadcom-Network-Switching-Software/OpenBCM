/*! \file ser_lts.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_LTS_H
#define SER_LTS_H

#include <sal/sal_types.h>
#include <shr/shr_fmm.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <sal/sal_libc.h>

/*! Invalid value of field */
#define  BCMPTM_SER_INVALID_FIELD_VAL   0xFFFFFFFF

/*! Get value of LT field */
#define  BCMPTM_SER_FLD_DEF_VAL_GET(_field, _val) \
    do { \
        if (_field.dtag == BCMLT_FIELD_DATA_TAG_BOOL) { \
            _val = _field.def.is_true; \
        } else if (_field.dtag == BCMLT_FIELD_DATA_TAG_U8) { \
            _val = _field.def.u8; \
        } else if (_field.dtag == BCMLT_FIELD_DATA_TAG_U16) { \
            _val = _field.def.u16; \
        } else if (_field.dtag == BCMLT_FIELD_DATA_TAG_U32) { \
            _val = _field.def.u32; \
        } else if (_field.dtag == BCMLT_FIELD_DATA_TAG_U64){ \
            _val = _field.def.u64; \
        } else { \
            _val = 0; \
        } \
    } while (0)

/*! Used to save field id and field data of IMM LTs */
typedef struct lt_ser_field_info_s {
    uint32_t  fld_data[BCMDRD_CONFIG_MAX_UNITS];
} bcmptm_ser_lt_field_data_t;

/*! Fields id and fields value which need to be updated */
typedef struct lt_ser_fld_s {
    uint32_t  lt_fid;
    uint32_t  data;
} bcmptm_ser_lt_flds_update_t;

/*!
 * \brief Initialize data structure 'bcmltd_fields_t'.
 *
 * \param [in] lt_flds                LTD fields.
 * \param [in] index                 index of LTD fields.
 * \param [in] fid_lt                  LT field id.
 * \param [in] array_fld_idx      index of array field.
 * \param [in] data                   data of field.
 *
 * \retval
 */
extern void
bcmptm_ser_imm_fld_set(bcmltd_fields_t lt_flds, int index, uint32_t fid_lt,
                       int array_fld_idx, uint32_t data);

/*!
 * \brief Allocate memory space used by 'shr_fmm_t'.
 *
 * \param [in] unit                   Unit number.
 * \param [in] fld_num             Depth of LT field array.
 *
 * \retval  NULL or array fields address handler
 */
extern shr_fmm_t **
bcmptm_ser_imm_flds_allocate(int unit, uint32_t fld_num);

/*!
 * \brief Free memory space used by 'shr_fmm_t'.
 *
 * \param [in] unit                   Unit number.
 * \param [in] fld_array            Array fields address handler.
 * \param [in] fld_num             Depth of LT field array.
 *
 * \retval
 */
extern void
bcmptm_ser_imm_flds_free(int unit, shr_fmm_t **fld_array, uint32_t fld_num);

/*!
 * \brief Check whether SER function is enabled or not.
 *
 * \param [in] unit                   Unit number.
 *
 * \retval 1: enabled, 0: disabled.
 */
bool
bcmptm_ser_checking_enable(int unit);

#endif /* SER_LTS_H */
