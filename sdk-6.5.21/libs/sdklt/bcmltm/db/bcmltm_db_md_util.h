/*! \file bcmltm_db_md_util.h
 *
 * Logical Table Manager - Metadata utility functions.
 *
 * This file contains utility routines used to construct the LTM
 * database information which are related to the LTM metadata structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_MD_UTIL_H
#define BCMLTM_DB_MD_UTIL_H

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_md_internal.h>

/*!
 * \brief Destroy the list of field maps metadata.
 *
 * Destroy the list of field maps metadata.
 *
 * \param [in] num_fields Number of fields in field map list.
 * \param [in] field_map_list Field maps metadata to destroy.
 */
extern void
bcmltm_db_field_maps_destroy(uint32_t num_fields,
                             bcmltm_field_map_t *field_map_list);

/*!
 * \brief Destroy the field list metadata.
 *
 * Destroy the field list metadata.
 *
 * \param [in] field_list Field list metadata to destroy.
 */
extern void
bcmltm_db_field_map_list_destroy(bcmltm_field_map_list_t *field_list);

/*!
 * \brief Destroy the field mapping metadata.
 *
 * Destroy the field mapping metadata.
 *
 * \param [in] field_mapping Field mapping metadata to destroy.
 */
extern void
bcmltm_db_field_mapping_destroy(bcmltm_field_select_mapping_t *field_mapping);

/*!
 * \brief Destroy the fixed field list metadata.
 *
 * Destroy the fixed field list metadata.
 *
 * \param [in] field_list Fixed field list metadata to destroy.
 */
extern void
bcmltm_db_fixed_field_list_destroy(bcmltm_fixed_field_list_t *field_list);

/*!
 * \brief Destroy the fixed field mapping metadata.
 *
 * Destroy the fixed field mapping metadata.
 *
 * \param [in] mapping Fixed field mapping metadata to destroy.
 */
extern void
bcmltm_db_fixed_field_mapping_destroy(bcmltm_fixed_field_mapping_t *mapping);

/*!
 * \brief Destroy the given LTA field list.
 *
 * Destroy the given LTA field list.
 *
 * \param [in] list LTA field list to destroy.
 */
extern void
bcmltm_db_lta_field_list_destroy(bcmltm_lta_field_list_t *list);

/*!
 * \brief Destroy the given LTA select field list.
 *
 * Destroy the given LTA select field list.
 *
 * \param [in] list LTA select field list to destroy.
 */
extern void
bcmltm_db_lta_select_field_list_destroy(bcmltm_lta_select_field_list_t *list);

/*!
 * \brief Create the LTA copy field list metadata for given table.
 *
 * Create the LTA Copy field list metadata for given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] from_list Origin LTA field list from which to copy.
 * \param [in] to_list Target LTA field list to which to copy.
 * \param [out] copy_list_ptr Returning pointer for LTA field list
 *              copy metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_copy_lta_field_list_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lta_field_list_t *from_list,
                                     bcmltm_lta_field_list_t *to_list,
                                     bcmltm_lta_field_list_t **copy_list_ptr);

/*!
 * \brief Get the working buffer offset for the given PT param field ID.
 *
 * This routine gets the PTM working buffer offset for the specified
 * field ID (parameter section).
 *
 * This function returns offset only on PT op 0.
 * This is a helper routine intended to be used
 * for building the track index elements and the corresponding
 * PT params copy list (used to propagate the params to other PTs).
 * This is intended to be used only in Index LTs where there is only
 * 1 PT op entry per PT ID.
 *
 * LTs with multiple PT ops per PT ID require a transform supplied
 * track index (where track field elements do not apply).
 *
 * \param [in] unit Unit number.
 * \param [in] pt_list PT list.
 * \param [in] ptid PT ID of field.
 * \param [in] fid Field ID to get offset for.
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_pt_param_offset_get(int unit,
                              const bcmltm_pt_list_t *pt_list,
                              bcmdrd_sid_t ptid,
                              uint32_t fid,
                              uint32_t *offset);

/*!
 * \brief Get property flags for given logical field.
 *
 * This routine gets the metadata field property flags for
 * the given logical field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] fid Logical field ID.
 * \param [out] flags Field property flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_field_properties_get(int unit,
                               bcmlrd_sid_t sid,
                               bcmlrd_fid_t fid,
                               uint32_t *flags);

#endif /* BCMLTM_DB_MD_UTIL_H */
