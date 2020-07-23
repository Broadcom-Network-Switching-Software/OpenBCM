/*! \file bcmptm_ser_testutil.h
 *
 * SER interfaces which can be used by test cases in bcma.
 *
 * APIs defines for SER test cases in bcma, cannot be called by bcmptm.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_SER_TESTUTIL_H
#define BCMPTM_SER_TESTUTIL_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <sal/sal_time.h>
/*!
 * \brief Write data to a H/W entry of a given PT.
 *
 * Only can be used by SER component.
 *
 *
 * \param [in] unit         Unit number
 * \param [in] mem_sid      PT ID.
 * \param [in] index        Table index.
 * \param [in] tbl_inst     If tbl_copy < 0, Table instance ID, otherwise, base index.
 * \param [in] tbl_copy     Some PTs have two dimensions, used to index appointed instance.
 * \param [in] data         New data for entry
 * \param [in] hw_only    Only write H/W, not update PTcache.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_testutil_pt_hw_write(int unit, bcmdrd_sid_t  mem_sid,
                                int index, int tbl_inst, int tbl_copy,
                                uint32_t *data, bool hw_only);

/*!
 * \brief Read data from a H/W entry of a given PT.
 *
 * Only can be used by SER component.
 *
 *
 * \param [in] unit         Unit number
 * \param [in] sid          PT ID.
 * \param [in] index        Table index.
 * \param [in] tbl_inst     If tbl_copy < 0, Table instance, otherwise, base index.
 * \param [in] tbl_copy     Some PTs have two dimensions,
 *                          used to index appointed instance.
 * \param [out] data        Data buffer, saved data from SID
 * \param [in] wsize        Data buffer size
 * \param [in] hw            Read data from H/W or PTcache.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_testutil_pt_read(int unit, bcmdrd_sid_t  sid,
                            int index, int tbl_inst, int tbl_copy,
                            uint32_t *data, size_t wsize, bool hw);

/*!
 * \brief Map a non-existing instance to a existing instance.
 *
 * \param [in] unit          Unit number.
 * \param [in] sid           PT ID.
 * \param [in/out] tbl_inst  Table instance number.
 * \param [in/out] tbl_copy  Table copy number.
 *
 * \return FALSE on instance doesn't exist, True exist.
 */
extern bool
bcmptm_ser_testutil_pt_inst_remap(int unit, bcmdrd_sid_t sid,
                                  int *tbl_inst, int *tbl_copy);

/*!
 * \brief  Map raw SID/index to SID/index, SER error is injected into it
 * when parameter 'inject == 1';
 * Map raw SID/index to SID/index, SER error is injected into it, after then,
 * Remap injected SID/index to SID/index used to correct SER error,
 * when parameter 'inject == 1'.
 *
 * \param [in] unit Unit number.
 * \param [in] sid SER testing SID.
 * \param [in] index SER testing index.
 * \param [in] inject 1: Map testing SID/index to SER error injected SID/index.
 *                    0: Map testing SID/index to SER error corrected SID/index.
 * \param [out] remap_sid Remapped SID, SER error is injected or corrected by this view.
 * \param [out] remap_index Remapped index, SER error is injected or corrected by this index.
 * \param [out] shadow_table 1: is shadow table.
 *
 * \return NONE.
 */
void
bcmptm_ser_testutil_mem_remap(int unit, bcmdrd_sid_t sid, int index, int inject,
                              bcmdrd_sid_t *remap_sid, int *remap_index,
                              bool *shadow_table);

/*!
 * \brief Check whether table instance and base index is valid
 * or not for a given PT.
 *
 * \param [in] unit       Unit number
 * \param [in] sid        PT ID
 * \param [in] tbl_inst   If copy_no < 0, table instance number,
 *                        otherwise, table base index.
 * \param [in] copy_no    Table instance number.
 *
 * \retval SHR_E_NONE for Success
 */
extern bool
bcmptm_ser_testutil_pt_copy_no_valid(int unit, bcmdrd_sid_t sid,
                                     int tbl_inst, int copy_no);

/*!
 * \brief Check whether index is valid or not.
 *
 * For UFT tables, their max index can be changed
 * during running time.
 *
 * \param [in] unit         Unit number.
 * \param [in] sid          PT ID.
 * \param [in] index        Index needed to check
 * \param [in] max_index    Max index of SID, only meaningful when routine returns TRUE.
 *
 * \retval TRUE for valid index, FALSE for invalid index
 */
extern int
bcmptm_ser_testutil_index_valid(int unit, bcmdrd_sid_t sid,
                                int index, int *max_index);

/*!
 * \brief Get SER logical block types according to block types(per chip).
 *
 * For some PTs, H/W can reports wrong block type, so when SER logic find
 * block type reported by H/W is wrong, this routine will tell SER logic which
 * block type the PTs belonged to.
 *
 * \param [in] unit               Unit number.
 * \param [in] blk_type           Block type (per chip) defined by BD.
 * \param [out] blk_type_overlay  Block type used to replace block type reported by H/W.
 * \param [out] ser_blk_type      Block type defined by SER LT.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_testutil_blk_type_map(int unit, int blk_type,
                                 int *blk_type_overlay, int *ser_blk_type);

/*!
 * \brief  Check whether control registers of all blocks (including sub-blocks) is enabled.
 * If not enable properly, there will be error messages.
 *
 * \param [in] unit        Unit number.
 *
 * \return NONE.
 */
extern void
bcmptm_ser_testutil_blk_parity_ctrl_validate(int unit);

/*!
 * \brief  Check whether a field of a control register is configured as '1'.
 *
 * \param [in] unit       Unit number.
 * \param [in] sid        PT ID.
 * \param [in] fid        Field ID.
 *
 * \return FALSE: control register is not enabled.
 * \return TRUE: control register is enabled.
 */
extern int
bcmptm_ser_testutil_ctrl_reg_validate(int unit, bcmdrd_sid_t sid,
                                      bcmdrd_fid_t fid);
/*!
 * \brief  Disable or enable "H/W refresh".
 *
 * During SER injection and validation, "H/W refresh" should be disabled.
 * Otherwise, the injected SER error will be triggered by H/W.
 *
 * \param [in] unit      Unit number.
 * \param [in] disable Disable H/W refresh.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_testutil_refresh_regs_disable(int unit, int disable);

/*!
 * \brief  Check whether access type of a PT is ADDR_SPLIT.
 *
 * \param [in] unit       Unit number.
 * \param [in] sid        PT ID.
 *
 * \return FALSE: is not ADDR_SPLIT.
 * \return TRUE: is ADDR_SPLIT.
 */
extern bool
bcmptm_ser_testutil_pt_acctype_is_addr_split(int unit, bcmdrd_sid_t pt_id);

/*!
 * \brief  Check whether access type of a PT is DATA_SPLIT.
 *
 * \param [in] unit       Unit number.
 * \param [in] sid        PT ID.
 *
 * \return FALSE: is not DATA_SPLIT.
 * \return TRUE: is DATA_SPLIT.
 */
extern bool
bcmptm_ser_testutil_pt_acctype_is_data_split(int unit, bcmdrd_sid_t pt_id);

/*!
 * \brief  Check whether access type of a PT is ADDR_SPLIT_SPLIT.
 *
 * \param [in] unit       Unit number.
 * \param [in] sid        PT ID.
 *
 * \return FALSE: is not ADDR_SPLIT_SPLIT.
 * \return TRUE: is ADDR_SPLIT_SPLIT.
 */
extern bool
bcmptm_ser_testutil_pt_acctype_is_addr_split_split(int unit, bcmdrd_sid_t pt_id);

/*!
 * \brief  Disable or enable SER check (including 1bit error reporting) of all PTs.
 *
 *
 * \param [in] unit      Unit number.
 * \param [in] Enable  Enable ECC or parity check, and 1bit error reporting.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmptm_ser_testutil_all_pts_enable(int unit, int enable);

/*!
 * \brief  For some legacy chips, TCAMs are protected by CMIC SER engine,
 * the engine can support either global mode or unique mode for one PT.
 *
 * \param [in] unit  Unit number.
 * \param [in] tcam Check whether SER protection mode is unique.
 * \param [out] unique_mode Unique mode.
 *
 * \return NONE.
 */
extern void
bcmptm_ser_testutil_is_unique_mode(int unit, bcmdrd_sid_t tcam, bool *unique_mode);

/*!
 * \brief Clear fields which value can be updated by H/W.
 *
 * \param [in] unit Unit number.
 * \param [in] sid SID.
 * \param [in/out] entry_data Entry data.
 *
 * \retval NONE.
 */
extern void
bcmptm_ser_testutil_parity_field_clear(int unit, bcmdrd_sid_t sid,
                                       uint32_t *entry_data);

#endif /* BCMPTM_SER_TESTUTIL_H */
