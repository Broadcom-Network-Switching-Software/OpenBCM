/*! \file bcma_testutil_cmicx.h
 *
 * CMICX utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_CMICX_H
#define BCMA_TESTUTIL_CMICX_H

/*!
 * \brief Write memory using DMA mode.
 *
 * Write data from DMA buffer into a range of table entries.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] index_start Index of the first table entry of the write operation.
 * \param [in] index_end Index of the last table entry of the write operation.
 * \param [in] tbl_inst Table instance.
 * \param [in] buf DMA buffer for table entries to be written.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_cmicx_mem_range_write(int unit, bcmdrd_sid_t sid, int index_start,
                                    int index_end, int tbl_inst, uint64_t buf);

/*!
 * \brief Read memory using DMA mode.
 *
 * Read data from a range of table entries into a DMA buffer.
 * The buffer size is index count * byte size of table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] index_start Index of the first table entry of the read operation.
 * \param [in] index_end Index of the last table entry of the read operation.
 * \param [in] tbl_inst Table instance.
 * \param [out] buf DMA buffer large enough to hold the specified range of table entries.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_cmicx_mem_range_read(int unit, bcmdrd_sid_t sid, int index_start,
                                   int index_end, int tbl_inst, uint64_t buf);

#endif /* BCMA_TESTUTIL_CMICX_H */
