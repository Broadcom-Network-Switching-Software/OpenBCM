/*! \file bcma_testutil_pt_common.h
 *
 * Physical table common utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_PT_COMMON_H
#define BCMA_TESTUTIL_PT_COMMON_H

#include <bcmbd/bcmbd.h>

/*!
 * \brief Perform logical AND on a data array of unsigned integers.
 *
 * \param [out] _dst Destination array.
 * \param [in] _src Source array.
 * \param [in] _len Data array length.
 */
#define BCMA_TESTUTIL_AND(_dst, _src, _len) \
    do { \
        int _idx; \
        for (_idx = 0; _idx < _len; _idx++) { \
            _dst[_idx] &= _src[_idx]; \
        } \
    } while(0)

/*!
 * \brief Perform logical OR on a data array of unsigned integers.
 *
 * \param [out] _dst Destination array.
 * \param [in] _src Source array.
 * \param [in] _len Data array length.
 */
#define BCMA_TESTUTIL_OR(_dst, _src, _len) \
    do { \
        int _idx; \
        for (_idx = 0; _idx < _len; _idx++) { \
            _dst[_idx] |= _src[_idx]; \
        } \
    } while(0)

/*!
 * \brief Perform logical XOR on a data array of unsigned integers.
 *
 * \param [out] _dst Destination array.
 * \param [in] _src Source array.
 * \param [in] _len Data array length.
 */
#define BCMA_TESTUTIL_XOR(_dst, _src, _len) \
    do { \
        int _idx; \
        for (_idx = 0; _idx < _len; _idx++) { \
            _dst[_idx] ^= _src[_idx]; \
        } \
    } while(0)

/*!
 * \brief Perform logical NOT on a data array of unsigned integers.
 *
 * \param [inout] _dst Destination array.
 * \param [in] _len Data array length.
 */
#define BCMA_TESTUTIL_NOT(_dst, _len) \
    do { \
        int _idx; \
        for (_idx = 0; _idx < _len; _idx++) { \
            _dst[_idx] = ~_dst[_idx]; \
        } \
    } while(0)

/*!
 * \brief Perform assignment operation on a data array of unsigned integers.
 *
 * \param [out] _dst Destination array.
 * \param [in] _src Source array.
 * \param [in] _len Data array length.
 */
#define BCMA_TESTUTIL_SET(_dst, _src, _len) \
    do { \
        int _idx; \
        for (_idx = 0; _idx < _len; _idx++) { \
            _dst[_idx] = _src[_idx]; \
        } \
    } while(0)

/*! The max length of physical table name. */
#define BCMA_TESTUTIL_STR_MAX_SIZE 256

/*!
 * \brief Get physical table mask.
 *
 * \c mask buffer must be large enough to hold a full table entry of the given \c sid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] index Primary index if the table has more than a single entry.
 * \param [out] mask Mask of testable bits in table entry.
 * \param [in] wsize Number of 32-bit words in table entry.
 * \param [in] skip_eccp Skip ECC and PARITY field.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_pt_mask_get(int unit, bcmdrd_sid_t sid, int index,
                          uint32_t *mask, int wsize, bool skip_eccp);


/*!
 * \brief Dump physical table data.
 *
 * \param [in] data Data of the physical table.
 * \param [in] wsize Size of the physical table.
 */
extern void
bcma_testutil_pt_data_dump(const uint32_t *data, int wsize);

/*!
 * \brief Compare physical table data.
 *
 * \param [in] data0, data1 Two data that will be compared.
 * \param [in] mask Mask of the physical table.
 * \param [in] wsize Size of the physical table.
 *
 * \return 0 on success and -1 on error.
 */
extern int
bcma_testutil_pt_data_comp(const uint32_t *data0, const uint32_t *data1,
                           const uint32_t *mask, int wsize);

/*!
 * \brief Write value into a field of the physical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] fid Field ID.
 * \param [in] idx Index of table entry.
 * \param [in] tbl_inst Table instance.
 * \param [in] field_value Field value.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_pt_field_set(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                           int idx, int tbl_inst, uint32_t field_value);

/*!
 * \brief Read value from a field of the physical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] fid Field ID.
 * \param [in] idx Index of table entry.
 * \param [in] tbl_inst Table instance.
 * \param [out] field_value Field value.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_pt_field_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                           int idx, int tbl_inst, uint32_t *field_value);

/*!
 * \brief Set zero to all fields of the physical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_pt_clear_all(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Verify the table instance of the physical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [inout] inst_start Start instance index.
 * \param [inout] inst_end End instance index.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_pt_inst_adjust(int unit, bcmdrd_sid_t sid, int *inst_start,
                             int *inst_end);

/*!
 * \brief Enable and disable SER.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] en Set to false to disable SER.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_pt_ser_ctrl(int unit, int sid, bool en);

/*!
 * \brief Get valid index range for physical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] tbl_inst Table instance.
 * \param [in] idx_min Min table index.
 * \param [in] idx_max Max table index.
 * \param [out] idx_start Valid start index.
 * \param [out] idx_end Valid end index.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_pt_valid_index_range_get(int unit, bcmdrd_sid_t sid, int tbl_inst,
                                       int idx_min, int idx_max, int *idx_start,
                                       int *idx_end);

#endif /* BCMA_TESTUTIL_PT_COMMON_H */
