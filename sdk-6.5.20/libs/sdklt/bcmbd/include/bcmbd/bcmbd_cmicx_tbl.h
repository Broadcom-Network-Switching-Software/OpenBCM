/*! \file bcmbd_cmicx_tbl.h
 *
 * Hash table operations to SOC memory on CMICx-based device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_TBL_H
#define BCMBD_CMICX_TBL_H

#include <bcmbd/bcmbd_cmicx.h>

/*!
 * \brief Insert entry to SOC memory hash table.
 *
 * Insert specifed entry \c data to hash table according to the specified
 * entry key.
 *
 * If \c resp_info is not NULL, \c resp_info will return the response
 * information from s-channel message of insert operation.
 *
 * If \c resp_data is not NULL, the replaced entry data will be
 * copied to \c resp_data when return value is SHR_E_NONE.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] data Data entry to insert to hash table.
 * \param [in] wsize Size of data entry.
 * \param [in] bank_ignore_mask Memory-specific bank ignored mask
 *             when inserted to hash table.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Old entry data on table insert if not NULL.
 *
 * \retval SHR_E_NONE New table entry inserted or replaced successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
int
bcmbd_cmicx_tbl_insert(int unit, uint32_t adext, uint32_t addr,
                       uint32_t *data, size_t wsize, uint32_t bank_ignore_mask,
                       bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Delete entry in SOC memory hash table.
 *
 * Delete specifed entry in hash table accroding to entry key
 * set in \c data.
 *
 * If \c resp_info is not NULL, \c resp_info will return the response
 * information from s-channel message of insert operation.
 *
 * If \c resp_data is not NULL, the deleted entry data will be
 * copied to \c resp_data when return value is SHR_E_NONE.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] data Entry key to be deleted in hash table.
 * \param [in] wsize Size of data entry.
 * \param [in] bank_ignore_mask Memory-specific bank ignored mask
 *             when inserted to hash table.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Deleted entry data if not NULL.
 *
 * \retval SHR_E_NONE table entry deleted successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
int
bcmbd_cmicx_tbl_delete(int unit, uint32_t adext, uint32_t addr,
                       uint32_t *data, size_t wsize, uint32_t bank_ignore_mask,
                       bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Lookup specified entry key in SOC memory table.
 *
 * Lookup specifed entry in hash table accroding to entry key
 * set in \c data.
 *
 * If \c resp_info is not NULL, \c resp_info will return the response
 * information from s-channel message of insert operation.
 *
 * If \c resp_data is not NULL, the content of looked up entry found in
 * hash table will be copied to \c resp_data when return value is SHR_E_NONE.

 * \param [in] unit Unit number.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] data Entry key to be searched in hash table.
 * \param [in] wsize Size of data entry.
 * \param [in] bank_ignore_mask Memory-specific bank ignored mask
 *             when inserted to hash table.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Matched data entry if not NULL.
 *
 * \retval SHR_E_NONE table entry found successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
int
bcmbd_cmicx_tbl_lookup(int unit, uint32_t adext, uint32_t addr,
                       uint32_t *data, size_t wsize, uint32_t bank_ignore_mask,
                       bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

#endif /* BCMBD_CMICX_TBL_H */
