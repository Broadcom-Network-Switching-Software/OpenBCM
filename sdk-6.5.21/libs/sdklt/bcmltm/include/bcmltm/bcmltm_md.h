/*! \file bcmltm_md.h
 *
 * Logical Table Manager Metadata.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_H
#define BCMLTM_MD_H

#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_pt.h>

/*!
 * \brief Get a count of all field IDs for given physical table.
 *
 * Get a count of all distinct fields for a given physical table ID in the
 * specified unit.
 *
 * The count includes only unique field IDs regardless of the field width.
 * For example, if a field occupies 128 bits (i.e. 2 field elements),
 * the returned count for this is 1.
 *
 * Unlike the corresponding DRD function, the list count includes
 * the special key fields, such as the index key field (for indexed memory
 * tables), or the port key field (for port-based register).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [out] num_fid Number of field IDs in table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pt_field_count_get(int unit, bcmdrd_sid_t sid, size_t *num_fid);


/*!
 * \brief  Get list of fields IDs for given physical table ID.
 *
 * Get the list of distinct fields IDs for a given physical table ID in the
 * specified unit.
 *
 * The list includes only unique field IDs regardless of the field width.
 * For example, if a field occupies 128 bits (i.e. 2 field elements),
 * the returned list contains 1 field.
 *
 * Unlike the corresponding DRD function, the list includes
 * the special key fields, such as the index key field (for indexed memory
 * tables), or the port key field (for port-based register).
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] list_max Number of entries allocated in field ID list.
 * \param [out] fid_list Field ID list.
 * \param [out] num_fid Total number of field IDs returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pt_field_list_get(int unit, bcmdrd_sid_t sid, size_t list_max,
                            bcmdrd_fid_t *fid_list, size_t *num_fid);


/*!
 * \brief Get field information for register/memory field.
 *
 * Obtain field information (name, start bit, end bit, etc.) for
 * specified symbol ID and field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical table ID.
 * \param [in] fid Field ID.
 * \param [out] finfo Field information structure to fill.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pt_field_info_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                            bcmdrd_sym_field_info_t *finfo);

/*!
 * \brief Find given field in transform entry.
 *
 * This routine checks if the given field ID is part of the transform
 * field list.  Depending on the input argument, it looks into
 * either the source field list (API facing field) or
 * the destination field list (PT).
 *
 * \param [in] unit Unit number.
 * \param [in] entry LRD transform map entry.
 * \param [in] src Indicates to look into the source or destination fields.
 * \param [out] num_fields Number of fields.
 * \param [out] flist Array of fields xfrm information.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_PARAM on NULL entry.
 * \retval SHR_E_INIT on sanity check failure.
 */
extern int
bcmltm_md_xfrm_field_get(int unit, const bcmlrd_map_entry_t *entry,
                         bool src, uint32_t *num_fields,
                         const bcmltd_field_list_t **flist);

#endif /* BCMLTM_MD_H */
