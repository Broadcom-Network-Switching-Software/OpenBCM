/*! \file bcma_bcmbd_cmicd.h
 *
 * Common functions for bcmbd CMICd commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMBD_CMICD_H
#define BCMA_BCMBD_CMICD_H

#include <bcmbd/bcmbd.h>
#include <bcma/bcmbd/bcma_bcmbd.h>

/*! Prototypes for CMICd hash table insert/delete/lookup functions. */
typedef int (*bcma_bcmbd_cmicd_tblop_f)(int unit, uint32_t adext, uint32_t addr,
                                        uint32_t *data, size_t wsize,
                                        uint32_t bank_ignore_mask,
                                        bcmbd_tbl_resp_info_t *resp_info,
                                        uint32_t *resp_data);

/*!
 * \brief Symbolic register read/write operation for CMICd devices.
 *
 * This function iterates over all specified register blocks, ports, etc in a
 * given sid, performs the request read or write, and displays necessary info.
 *
 * Note that read operation will be performed only if \c and_masks
 * and \c or_maskes are both NULL. Otherwise the operation will
 * be taken as read-modify-write.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] enum_val Symbol enum value.
 * \param [in] sid Operation targets information.
 * \param [in] size Number of 32-bit word size of data to read/write.
 * \param [in] and_masks Data masks to be AND'ed.
 * \param [in] or_masks Data masks to be OR'ed.
 *
 * \retval Always 0.
 */
extern int
bcma_bcmbd_cmicd_regops(int unit,
                        const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                        bcma_bcmbd_id_t *sid, uint32_t size,
                        uint32_t *and_masks, uint32_t *or_masks);

/*!
 * \brief Symbolic Memory read/write operation for CMICd devices.
 *
 * This function iterates over all specified memory indices, blocks, etc in a
 * given sid, performs the request read or write, and displays necessary info.
 *
 * Note that read operation will be performed only if \c and_masks
 * and \c or_maskes are both NULL. Otherwise the operation will
 * be taken as read-modify-write.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] enum_val Symbol enum value.
 * \param [in] sid Operation targets information.
 * \param [in] size Number of 32-bit word size of data to read/write.
 * \param [in] and_masks Data masks to be AND'ed.
 * \param [in] or_masks Data masks to be OR'ed.
 *
 * \retval Always 0.
 */
extern int
bcma_bcmbd_cmicd_memops(int unit,
                        const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                        bcma_bcmbd_id_t *sid, uint32_t size,
                        uint32_t *and_masks, uint32_t *or_masks);

/*!
 * \brief Perform symbolic register and memory operations for CMICd devices.
 *
 * This function will perform symbolic register or memory operation
 * for CMICd units. The information of modification targets is retrieved from
 * \c symbol and BCMBD command id structure \c sid.
 *
 * Note that read operation will be performed only if \c and_masks
 * and \c or_maskes are both NULL. Otherwise the operation will
 * be taken as read-modify-write.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] enum_val Symbol enum value.
 * \param [in] sid Operation targets information.
 * \param [in] and_masks Data masks to be AND'ed.
 * \param [in] or_masks Data masks to be OR'ed.
 *
 * \retval Always 0.
 */
extern int
bcma_bcmbd_cmicd_symop(int unit,
                       const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                       bcma_bcmbd_id_t *sid,
                       uint32_t *and_masks, uint32_t *or_masks);

/*!
 * \brief Perform symbolic hash table insert for CMICd devices.
 *
 * This function will perform symbolic hash table insert
 * operation for CMICd units. The information of modification targets is
 * retrieved from \c symbol and BCMBD command id structure \c sid.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] sid Operation targets information.
 * \param [in] and_masks Data masks to be AND'ed.
 * \param [in] or_masks Data masks to be OR'ed.
 *
 * \retval Always 0.
 */
extern int
bcma_bcmbd_cmicd_tbl_insert(int unit, const bcmdrd_symbol_t *symbol,
                            bcma_bcmbd_id_t *sid,
                            uint32_t *and_masks, uint32_t *or_masks);

/*!
 * \brief Perform symbolic hash table delete for CMICd devices.
 *
 * This function will perform symbolic hash table delete
 * operation for CMICd units. The information of modification targets is
 * retrieved from \c symbol and BCMBD command id structure \c sid.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] sid Operation targets information.
 * \param [in] and_masks Data masks to be AND'ed.
 * \param [in] or_masks Data masks to be OR'ed.
 *
 * \retval Always 0.
 */
extern int
bcma_bcmbd_cmicd_tbl_delete(int unit, const bcmdrd_symbol_t *symbol,
                            bcma_bcmbd_id_t *sid,
                            uint32_t *and_masks, uint32_t *or_masks);

/*!
 * \brief Perform symbolic hash table lookup for CMICd devices.
 *
 * This function will perform symbolic hash table lookup
 * operation for CMICd units. The information of modification targets is
 * retrieved from \c symbol and BCMBD command id structure \c sid.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] sid Operation targets information.
 * \param [in] and_masks Data masks to be AND'ed.
 * \param [in] or_masks Data masks to be OR'ed.
 *
 * \retval Always 0.
 */
extern int
bcma_bcmbd_cmicd_tbl_lookup(int unit, const bcmdrd_symbol_t *symbol,
                            bcma_bcmbd_id_t *sid,
                            uint32_t *and_masks, uint32_t *or_masks);

/*!
 * \brief Perform symbolic FIFO pop operation for CMICd devices.
 *
 * This function will perform symbolic FIFO pop operation
 * for CMICd units. The information of modification targets is retrieved from
 * \c symbol and BCMBD command id structure \c sid.
 *
 * \param [in] unit Unit number.
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] enum_val Symbol enum value.
 * \param [in] sid Operation targets information.
 *
 * \retval 0 on success, -1 on failure.
 */
extern int
bcma_bcmbd_cmicd_fifo_pop(int unit,
                          const bcmdrd_symbol_t *symbol, bcmdrd_sid_t enum_val,
                          bcma_bcmbd_id_t *sid);

#endif /* BCMA_BCMBD_CMICD_H */
