/*! \file bcmltm_db_internal.h
 *
 * Logical Table Manager - Logical Table Database.
 *
 * This file contains the top level routines to initialize the
 * database for logical tables and provide information used
 * during metadata construction.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_INTERNAL_H
#define BCMLTM_DB_INTERNAL_H

#include <shr/shr_types.h>
#include <shr/shr_pb_local.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmptm/bcmptm_types.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_db_types_internal.h>


/*!
 * \brief Initialize logical table database.
 *
 * This routine initializes the database for logical tables and
 * creates internal data structures to maintain the information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of logical table IDs on unit.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_init(int unit, uint32_t sid_max_count);

/*!
 * \brief Cleanup logical table database.
 *
 * This routine frees any memory allocated during the initialization
 * and creation of the internal database for logical tables.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmltm_db_cleanup(int unit);

/*!
 * \brief Display database information for given logical table ID.
 *
 * This routine displays the database information content for
 * given logical table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in,out] pb Print buffer to populate output data.
 */
extern void
bcmltm_db_info_dump_by_sid(int unit, bcmlrd_sid_t sid,
                           shr_pb_t *pb);

/*!
 * \brief Display database information for given logical table name.
 *
 * This routine displays the database information content for
 * given logical table name.
 *
 * \param [in] unit Unit number.
 * \param [in] name Logical table name.
 * \param [in,out] pb Print buffer to populate output data.
 */
extern void
bcmltm_db_info_dump_by_name(int unit, const char *name,
                            shr_pb_t *pb);

/*!
 * \brief Display database information for all logical tables in given unit.
 *
 * This routine displays the database information content for
 * all logical tables in given unit.
 *
 * \param [in] unit Unit number.
 * \param [in,out] pb Print buffer to populate output data.
 */
extern void
bcmltm_db_info_dump(int unit,
                    shr_pb_t *pb);

/*!
 * \brief Get the table attributes for given table ID.
 *
 * This routine gets the table attributes for given table ID as
 * defined in the logical table definition map file.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] attr_ptr Returning pointer to table attributes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_attr_get(int unit, bcmlrd_sid_t sid,
                         const bcmltm_table_attr_t **attr_ptr);

/*!
 * \brief Get the table type for the given logical table ID.
 *
 * This function gets the table type for given table ID as specified in
 * the logical table definition.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] type Table type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_type_get(int unit, bcmlrd_sid_t sid,
                         bcmltm_table_type_t *type);

/*!
 * \brief Check that logical opcode is supported.
 *
 * This routine checks if a logical opcode is supported in the
 * given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode LT opcode.
 * \param [out] is_supported TRUE if given opcode is supported, FALSE otherwise.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_opcode_is_supported(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmlt_opcode_t opcode,
                                    bool *is_supported);

/*!
 * \brief Get the maximum tracking index for given Index LT.
 *
 * This function gets the maximum value of the tracking index
 * for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] track_index_max Maximum tracking index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_index_max_get(int unit, bcmlrd_sid_t sid,
                                 uint32_t *track_index_max);

/*!
 * \brief Check if track index is mapped in the table map.
 *
 * This routine checks if the track index on a index table
 * is a destination field map.
 *
 * The track index uniquely identifies an entry in a index table
 * and can be supplied by either:
 * - A field map
 *   The current supported case is when a field transform
 *   provides the track index from the source API keys (forward)
 *   and vice versa (reverse).
 * - LTM internal
 *   LTM composes the track index from a set of track index
 *   field elements (PT key params, i.e. index, port, instance, etc.)
 *
 * Applicable only for direct map index tables.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] is_mapped TRUE, if track index is provided by a map.
 *                        FALSE, if track index is not provided by a map,
 *                        or if track index is not applicable for table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_index_is_mapped(int unit, bcmlrd_sid_t sid,
                                   bool *is_mapped);

/*!
 * \brief Get the Track Index metadata for given table.
 *
 * This routine gets the FA Track Index metadata for given table.
 * This is used as the FA node cookie in the FA stage for LTM index
 * resource operations on Index LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] index_ptr Returning pointer to FA Track Index metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_index_get(int unit, bcmlrd_sid_t sid,
                             const bcmltm_track_index_t **index_ptr);


/*!
 * \brief Get the EE LT Index metadata for given table.
 *
 * This routine gets the EE LT Index metadata for given table.
 * This is used as the EE node cookie in the EE stage for LTM index
 * resource operations on Index LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] index_ptr Returning pointer to EE Index metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_ee_lt_index_get(int unit, bcmlrd_sid_t sid,
                             const bcmltm_ee_index_info_t **index_ptr);


/*!
 * \brief Get the Track Index to PT WB copy list metadata.
 *
 * This routine gets the Track Index elements to PT WB copy list metadata
 * for given table ID.
 * This is used as the FA node cookie in the FA stage to copy
 * the next index into the corresponding PTM WB during a TRAVERSE
 * operation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] list_ptr Returning pointer to Track to PT copy list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_to_pt_copy_get(int unit, bcmlrd_sid_t sid,
                                  const bcmltm_wb_copy_list_t **list_ptr);

/*!
 * \brief Get track index fields to API fields mapping metadata.
 *
 * This routine gets the direct field mapping metadata to
 * convert the track index field elements to API cache fields
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] fields_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_to_api_fields_get(int unit,
                                     bcmlrd_sid_t sid,
                        const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get number of reverse field transforms for track index.
 *
 * This routine gets the number of reverse field transforms used
 * to convert the track index field elements to API cache fields
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] num Number of field transforms.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_rev_xfrm_num_get(int unit,
                                    bcmlrd_sid_t sid,
                                    uint32_t *num);
/*!
 * \brief Get reverse track index transform parameters metadata.
 *
 * This routine gets the reverse track index transform parameters
 * metadata for the specified transform index for a given table.
 * This is used to convert the track index field elements in a transform
 * to API cache fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] params_ptr Returning pointer to field transform params.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_rev_xfrm_params_get(int unit,
                                       bcmlrd_sid_t sid,
                                       uint32_t xfrm_idx,
                          const bcmltm_lta_transform_params_t **params_ptr);

/*!
 * \brief Get reverse track index transform input field list metadata.
 *
 * This routine gets the reverse track index transform input field list
 * metadata for the specified transform index for a given table.
 * This is used to convert the track index field elements in a transform
 * to API cache fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] fields_ptr Returning pointer to transform input field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_rev_xfrm_in_fields_get(int unit,
                                          bcmlrd_sid_t sid,
                                          uint32_t xfrm_idx,
                         const bcmltm_lta_select_field_list_t **fields_ptr);

/*!
 * \brief Get reverse track index transform output field list metadata.
 *
 * This routine gets the reverse track index transform output field list
 * metadata for the specified transform index for a given table.
 * This is used to convert the track index field elements in a transform
 * to API cache fields.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] fields_ptr Returning pointer to transform output field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_track_rev_xfrm_out_fields_get(int unit,
                                           bcmlrd_sid_t sid,
                                           uint32_t xfrm_idx,
                          const bcmltm_lta_select_field_list_t **fields_ptr);

/*!
 * \brief Get the PT index list metadata for given table.
 *
 * This routine gets the PT index list metadata for given table.
 * This is used as the EE node cookie in the EE stage for
 * index PTM operations on LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] pt_list_ptr Returning pointer to PT list metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_pt_list_get(int unit, bcmlrd_sid_t sid,
                         const bcmltm_pt_list_t **pt_list_ptr);


/*!
 * \brief Get the PT keyed information metadata for given table.
 *
 * This routine gets the PT keyed operations information metadata
 * for given table and PTM operation.
 *
 * This is used as the EE node cookie in the EE stage for
 * keyed PTM operations on LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] ptm_op PTM operation.
 * \param [in] pt_keyed_ptr Returning pointer for PT keyed op info metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_pt_keyed_get(int unit, bcmlrd_sid_t sid,
                          bcmptm_op_type_t ptm_op,
                          const bcmltm_pt_keyed_op_info_t **pt_keyed_ptr);

/*!
 * \brief Get the direct field mapping metadata.
 *
 * This routine gets the direct field mapping metadata for given table ID.
 * This is used as the FA node cookie in the FA stage for PTM operations
 * on LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type Indicates key or value field.
 * \param [in] field_dir Indicates input or output field direction.
 * \param [out] field_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_field_mapping_get(int unit,
                               bcmlrd_sid_t sid,
                               bcmltm_field_type_t field_type,
                               bcmltm_field_dir_t field_dir,
                               const bcmltm_field_select_mapping_t
                               **field_mapping_ptr);


/*!
 * \brief Get the required key field direct mapping metadata for IwA LTs.
 *
 * This routine gets the direct field mapping metadata for required keys
 * on given IwA LT.
 *
 * This is used as the FA node cookie in the FA stage for INSERT operations
 * on Index with Allocation LTs.
 *
 * Required keys are mandatory during INSERT on IwA LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] field_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_req_key_field_mapping_get(int unit,
                                       bcmlrd_sid_t sid,
                                       const bcmltm_field_select_mapping_t
                                       **field_mapping_ptr);

/*!
 * \brief Get the allocatable key field direct mapping metadata for IwA LTs.
 *
 * This routine gets the direct field mapping metadata for allocatable keys
 * on given IwA LT.
 *
 * This is used as the FA node cookie in the FA stage for INSERT operations
 * on Index with Allocation LTs.
 *
 * Allocatable keys are optional during INSERT on IwA LTs.  When absent,
 * these keys are allocated and returned.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] field_mapping_ptr Returning pointer to field mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_alloc_key_field_mapping_get(int unit,
                                         bcmlrd_sid_t sid,
                                         const bcmltm_field_select_mapping_t
                                         **field_mapping_ptr);

/*!
 * \brief Get the fixed key fields metadata.
 *
 * This routine gets the fixed key field list mapping metadata for
 * given table ID.
 * This is used as the FA node cookie in the FA stage for PTM operations
 * on LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fixed_fields_ptr Returning pointer to fixed fields metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_fixed_keys_get(int unit,
                            bcmlrd_sid_t sid,
                            const bcmltm_fixed_field_list_t
                            **fixed_fields_ptr);

/*!
 * \brief Get the fixed value fields metadata.
 *
 * This routine gets the fixed value field list mapping metadata for
 * given table ID.
 * This is used as the FA node cookie in the FA stage for PTM operations
 * on LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fixed_fields_ptr Returning pointer to fixed fields metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_fixed_values_get(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_fixed_field_list_t
                              **fixed_fields_ptr);

/*!
 * \brief Get the read-only value fields metadata.
 *
 * This routine gets the read-only value field list mapping metadata for
 * given table ID.
 * This is used as the FA node cookie in the FA stage for PTM operations
 * on LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] read_only_list_ptr Returning pointer to read-only
 *                                 fields metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_read_only_list_get(int unit,
                             bcmlrd_sid_t sid,
                             const bcmltm_ro_field_list_t
                             **read_only_list_ptr);

/*!
 * \brief Get the wide value fields metadata.
 *
 * This routine gets the wide value field list mapping metadata for
 * given table ID.
 * This is used as the FA node cookie in the FA stage for PTM operations
 * on LT with Physical table mappings.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] wide_field_list_ptr Returning pointer to wide
 *                                  fields metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_wide_field_list_get(int unit,
                              bcmlrd_sid_t sid,
                              const bcmltm_wide_field_list_t
                              **wide_field_list_ptr);

/*!
 * \brief Get the number of Field Transforms.
 *
 * This routine gets the number of field transforms for the
 * specified transform type in given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field transform type BCMLTM_DB_XFRM_TYPE_xxx.
 * \param [out] num Number of field transforms.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_xfrm_num_get(int unit, bcmlrd_sid_t sid, int xfrm_type,
                       uint32_t *num);

/*!
 * \brief Get the Field Transform parameters.
 *
 * This routine gets the field transform parameters for
 * the specified transform type and index on given table.
 * This is used as the FA node cookie in the FA stage for the
 * LTA interface on LT with Field Transform Handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field transform type BCMLTM_DB_XFRM_TYPE_xxx.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] params_ptr Returning pointer to field transform params.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_xfrm_params_get(int unit, bcmlrd_sid_t sid,
                          int xfrm_type,
                          uint32_t xfrm_idx,
                          const bcmltm_lta_transform_params_t **params_ptr);

/*!
 * \brief Get the Field Transform input field list.
 *
 * This routine gets the field transform input field list for
 * the specified transform type and index on given table.
 * In forward transform case, this in field list is src field.
 * In reverse transform case, this in field list is dst field.
 * This is used as the FA node cookie in the FA stage.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field transform type BCMLTM_DB_XFRM_TYPE_xxx.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] fields_ptr Returning pointer to transform input field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_xfrm_in_fields_get(int unit, bcmlrd_sid_t sid,
                             int xfrm_type,
                             uint32_t xfrm_idx,
                             const bcmltm_lta_field_list_t **fields_ptr);

/*!
 * \brief Get the Field Transform src key input field list.
 *
 * This routine gets the field transform source key input field list for
 * the specified transform type and index on given table.
 * In forward transform case, this in field list is src_key field.
 * In reverse transform case, this in field list is src_key field.
 * This is used as the FA node cookie in the FA stage.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field transform type BCMLTM_DB_XFRM_TYPE_xxx.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] fields_ptr Returning pointer to transform key input field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_xfrm_in_key_fields_get(int unit, bcmlrd_sid_t sid,
                                 int xfrm_type,
                                 uint32_t xfrm_idx,
                                 const bcmltm_lta_field_list_t **fields_ptr);

/*!
 * \brief Get the Field Transform output field list.
 *
 * This routine gets the field transform output field list for
 * the specified transform type and index on given table.
 * In forward transform case, this out field list is dst field.
 * In reverse transform case, this out field list is src field.
 * This is used as the FA node cookie in the FA stage.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field transform type BCMLTM_DB_XFRM_TYPE_xxx.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] fields_ptr Returning pointer to transform output field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_xfrm_out_fields_get(int unit, bcmlrd_sid_t sid,
                              int xfrm_type,
                              uint32_t xfrm_idx,
                              const bcmltm_lta_field_list_t **fields_ptr);

/*!
 * \brief Get the Field Transform Copy field list.
 *
 * This routine gets the field transform copy field list for
 * the specified transform type and index on given table.
 * The current use cases are:
 * - From reverse key transform output to forward key transform input.
 * - From reverse value transform output to forward value transform input.
 * This is used as the FA node cookie in the FA stage.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] xfrm_type Field transform type BCMLTM_DB_XFRM_TYPE_xxx.
 * \param [in] xfrm_idx Field transform index.
 * \param [in] from_xfrm_type From list field transform type.
 * \param [in] from_xfrm_idx From list index of field transform.
 * \param [out] fields_ptr Returning pointer to transform copy field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_xfrm_copy_fields_get(int unit, bcmlrd_sid_t sid,
                               int xfrm_type,
                               uint32_t xfrm_idx,
                               int from_xfrm_type,
                               uint32_t from_xfrm_idx,
                               const bcmltm_lta_field_list_t **fields_ptr);

/*!
 * \brief Get the number of Field Validations.
 *
 * This routine gets the number of field validations for the
 * specified field type, key or value, in given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type The type of field KEY or VALUE.
 * \param [out] num Number of field validations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_fv_num_get(int unit, bcmlrd_sid_t sid,
                     bcmltm_field_type_t field_type,
                     uint32_t *num);

/*!
 * \brief Get the Field Validation parameters.
 *
 * This routine gets the field validation parameters for
 * the specified field type and validation index on given table.
 * This is used as the FA node cookie in the FA stage for the
 * Field Validation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type The type of field KEY or VALUE.
 * \param [in] fld_idx Field validation index.
 * \param [out] params_ptr Returning pointer to field validation params.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_fv_validate_params_get(int unit, bcmlrd_sid_t sid,
                                 bcmltm_field_type_t field_type,
                                 uint32_t fld_idx,
                                 const bcmltm_lta_validate_params_t
                                 **params_ptr);

/*!
 * \brief Get the Field Validation source field list.
 *
 * This routine gets the field validation input field list for
 * the specified field type and validation index on given table.
 * This is used as the FA node cookie in the FA stage.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type The type of field KEY or VALUE.
 * \param [in] fld_idx Field validation index.
 * \param [out] fields_ptr Returning pointer to validation source field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_fv_src_fields_get(int unit, bcmlrd_sid_t sid,
                            bcmltm_field_type_t field_type,
                            uint32_t fld_idx,
                            const bcmltm_lta_field_list_t **fields_ptr);

/*!
 * \brief Get the Field Validation copy field list.
 *
 * This routine gets the input field list for Field Validation copy from
 * a given LTA reverse transform.  This is required on UPDATE to
 * use the existing field values when combined with any changed
 * values to validate the fields.
 * This is used as the FA node cookie in the FA stage.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_type Field Type.
 * \param [in] validate_idx Field validation index.
 * \param [in] from_xfrm_type From list field transform type.
 * \param [in] from_xfrm_idx From list index of field transform.
 * \param [out] fields_ptr Returning pointer to validation copy field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_fv_copy_fields_get(int unit, bcmlrd_sid_t sid,
                             bcmltm_field_type_t field_type,
                             uint32_t validate_idx,
                             int from_xfrm_type,
                             uint32_t from_xfrm_idx,
                             const bcmltm_lta_field_list_t **fields_ptr);

/*!
 * \brief Get field selection core information.
 *
 * This routine gets the field selection core information
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] info Returning field selection information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_fs_core_info_get(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_db_fs_core_info_t *info);

/*!
 * \brief Get field select map metadata.
 *
 * This routine gets the field select map metadata
 * for the specified field selection ID for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [out] fsm_ptr Returning pointer to field select map metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_fsm_get(int unit,
                     bcmlrd_sid_t sid,
                     bcmltm_field_selection_id_t selection_id,
                     const bcmltm_field_select_maps_t **fsm_ptr);

/*!
 * \brief Get key fields mapping metadata for API <-> API Cache.
 *
 * This routine gets the key fields mapping metadata for
 * copying key fields between the API input/output field list
 * and the API cache for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] field_dir Indicates input or output field direction.
 * \param [out] fields_ptr Returning pointer to fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_apic_key_fields_get(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_field_dir_t field_dir,
                           const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get required key fields mapping metadata for API <-> API Cache.
 *
 * This routine gets the key fields mapping metadata for
 * copying required keys between the API input/output field list
 * and the API cache for a given IwA table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] field_dir Indicates input or output field direction.
 * \param [out] fields_ptr Returning pointer to fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_apic_req_key_fields_get(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_field_dir_t field_dir,
                        const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get allocatable key fields mapping metadata for API <-> API Cache.
 *
 * This routine gets the key fields mapping metadata for
 * copying allocatable keys between the API input/output field list
 * and the API cache for a given IwA table.
 *
 * Allocatable keys are optional during INSERT on IwA LTs.  When absent,
 * these keys are allocated and returned.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] field_dir Indicates input or output field direction.
 * \param [out] fields_ptr Returning pointer to fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_apic_alloc_key_fields_get(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmltm_field_dir_t field_dir,
                          const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get value fields mapping metadata for API <-> API Cache.
 *
 * This routine gets the value fields mapping metadata for
 * copying value fields between the API input/output field list
 * and the API cache for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] field_dir Indicates input or output field direction.
 * \param [out] fields_ptr Returning pointer to fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_apic_value_fields_get(int unit,
                                   bcmlrd_sid_t sid,
                                   bcmltm_field_selection_id_t selection_id,
                                   bcmltm_field_dir_t field_dir,
                             const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get API Cache fields mapping metadata for all value fields.
 *
 * This routine gets the fields mapping metadata for all value fields
 * for a given table.
 *
 * This field map metadata is used for situations where it is desirable
 * to have the API cache field maps for all value fields, regardless
 * of the field selection group.
 * For instance, this metadata is used to set the defaults
 * in the API cache for all value fields during a DELETE or UPDATE
 * operation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] field_dir Indicates input or output field direction.
 * \param [out] fields_ptr Returning pointer to fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_apic_all_value_fields_get(int unit,
                                       bcmlrd_sid_t sid,
                                       bcmltm_field_dir_t field_dir,
                          const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get key direct fields mapping metadata.
 *
 * This routine gets the key direct fields mapping metadata
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] field_dir Indicates input or output field direction.
 * \param [out] fields_ptr Returning pointer to fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_key_fields_get(int unit,
                            bcmlrd_sid_t sid,
                            bcmltm_field_dir_t field_dir,
                            const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get required key direct fields mapping metadata for IwA LT.
 *
 * This routine gets the direct fields mapping metadata
 * for required keys for a given IwA table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] field_dir Indicates input or output field direction.
 * \param [out] fields_ptr Returning pointer to fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_req_key_fields_get(int unit,
                                bcmlrd_sid_t sid,
                                bcmltm_field_dir_t field_dir,
                          const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get allocatable key direct fields mapping metadata for IwA LT.
 *
 * This routine gets the direct fields mapping metadata
 * for allocatable keys for a given IwA table.
 *
 * Allocatable keys are optional during INSERT on IwA LTs.  When absent,
 * these keys are allocated and returned.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] field_dir Indicates input or output field direction.
 * \param [out] fields_ptr Returning pointer to fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_alloc_key_fields_get(int unit,
                                  bcmlrd_sid_t sid,
                                  bcmltm_field_dir_t field_dir,
                            const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get value direct fields mapping metadata.
 *
 * This routine gets the value direct fields mapping metadata
 * for the specified field selection ID for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] field_dir Indicates input or output field direction.
 * \param [out] fields_ptr Returning pointer to fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_value_fields_get(int unit,
                              bcmlrd_sid_t sid,
                              bcmltm_field_selection_id_t selection_id,
                              bcmltm_field_dir_t field_dir,
                              const bcmltm_field_select_mapping_t **fields_ptr);

/*!
 * \brief Get fixed key fields mapping metadata.
 *
 * This routine gets the fixed key fields mapping metadata
 * for the specified field selection ID for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [out] fields_ptr Returning pointer to fixed fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_fixed_key_fields_get(int unit,
                                  bcmlrd_sid_t sid,
                            const bcmltm_fixed_field_mapping_t **fields_ptr);

/*!
 * \brief Get fixed value fields mapping metadata.
 *
 * This routine gets the fixed value fields mapping metadata
 * for the specified field selection ID for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [out] fields_ptr Returning pointer to fixed fields mapping metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_fixed_value_fields_get(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmltm_field_selection_id_t selection_id,
                              const bcmltm_fixed_field_mapping_t **fields_ptr);

/*!
 * \brief Get number of field transforms.
 *
 * This routine gets the number of field transforms
 * for the specified field selection ID and transform type
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] xfrm_type Field transform type.
 * \param [out] num Number of field transforms.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_xfrm_num_get(int unit,
                          bcmlrd_sid_t sid,
                          bcmltm_field_selection_id_t selection_id,
                          bcmltm_field_xfrm_type_t xfrm_type,
                          uint32_t *num);

/*!
 * \brief Get transform parameters metadata.
 *
 * This routine gets the field transform parameters metadata
 * for the specified field selection ID, transform type, and index,
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] xfrm_type Field transform type.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] params_ptr Returning pointer to field transform params.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_xfrm_params_get(int unit,
                             bcmlrd_sid_t sid,
                             bcmltm_field_selection_id_t selection_id,
                             bcmltm_field_xfrm_type_t xfrm_type,
                             uint32_t xfrm_idx,
                             const bcmltm_lta_transform_params_t **params_ptr);

/*!
 * \brief Get transform input field list metadata.
 *
 * This routine gets the transform input field list metadata
 * for the specified field selection ID, transform type, and index,
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] xfrm_type Field transform type.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] fields_ptr Returning pointer to transform input field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_xfrm_in_fields_get(int unit,
                                bcmlrd_sid_t sid,
                                bcmltm_field_selection_id_t selection_id,
                                bcmltm_field_xfrm_type_t xfrm_type,
                                uint32_t xfrm_idx,
                   const bcmltm_lta_select_field_list_t **fields_ptr);

/*!
 * \brief Get transform input key field list metadata.
 *
 * This routine gets the transform input key field list metadata
 * for the specified field selection ID, transform type, and index,
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] xfrm_type Field transform type.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] fields_ptr Returning pointer to transform input key field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_xfrm_in_key_fields_get(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmltm_field_selection_id_t selection_id,
                                    bcmltm_field_xfrm_type_t xfrm_type,
                                    uint32_t xfrm_idx,
                       const bcmltm_lta_select_field_list_t **fields_ptr);

/*!
 * \brief Get transform output field list metadata.
 *
 * This routine gets the transform output field list metadata
 * for the specified field selection ID, transform type, and index,
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] xfrm_type Field transform type.
 * \param [in] xfrm_idx Field transform index.
 * \param [out] fields_ptr Returning pointer to transform output field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_dm_xfrm_out_fields_get(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_field_selection_id_t selection_id,
                                 bcmltm_field_xfrm_type_t xfrm_type,
                                 uint32_t xfrm_idx,
                    const bcmltm_lta_select_field_list_t **fields_ptr);

/*!
 * \brief Get number of field validations.
 *
 * This routine gets the number of field validations
 * for the specified field selection ID and validation type
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] val_type Field validation type.
 * \param [out] num Number of field validations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_validate_num_get(int unit,
                           bcmlrd_sid_t sid,
                           bcmltm_field_selection_id_t selection_id,
                           bcmltm_field_validate_type_t val_type,
                           uint32_t *num);

/*!
 * \brief Get field validation parameters metadata.
 *
 * This routine gets the field validation parameters metadata
 * for the specified field selection ID, validation type, and index,
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] val_type Field validation type.
 * \param [in] val_idx Field validation index.
 * \param [out] params_ptr Returning pointer to field validation params.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_validate_params_get(int unit,
                              bcmlrd_sid_t sid,
                              bcmltm_field_selection_id_t selection_id,
                              bcmltm_field_validate_type_t val_type,
                              uint32_t val_idx,
                              const bcmltm_lta_validate_params_t **params_ptr);

/*!
 * \brief Get field validation input field list metadata.
 *
 * This routine gets the field validation input field list metadata
 * for the specified field selection ID, validation type, and index,
 * for a given table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] selection_id Field selection ID.
 * \param [in] val_type Field validation type.
 * \param [in] val_idx Field validation index.
 * \param [out] fields_ptr Returning pointer to validation input field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_validate_in_fields_get(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmltm_field_selection_id_t selection_id,
                                 bcmltm_field_validate_type_t val_type,
                                 uint32_t val_idx,
                           const bcmltm_lta_select_field_list_t **fields_ptr);

/*!
 * \brief Get the Table Commit handler list.
 *
 * This routine gets the Table Commit Handler list for the given
 * logical table.  If the logical table does not have any Table Commit
 * handlers, the returned pointer is NULL.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] tc_list_ptr Returning pointer to Table Commit handler list,
 *                         or NULL if table does not have any.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_commit_list_get(int unit, bcmlrd_sid_t sid,
                                const bcmltm_table_commit_list_t **tc_list_ptr);

/*!
 * \brief Get the LTA Table Entry Limit handler.
 *
 * This routine gets table entry limit handler for the given
 * logical table.  If the logical table does not have a handler,
 * the returned pointer is NULL.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] handler_ptr Returning pointer to LTA table entry limit handler,
 *                          or NULL if LTA handler is not defined.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_entry_limit_handler_get(int unit,
                                        bcmlrd_sid_t sid,
               const bcmltd_table_entry_limit_handler_t  **handler_ptr);

/*!
 * \brief Get the LTA Table Entry Usage handler.
 *
 * This routine gets table entry usage handler for the given
 * logical table.  If the logical table does not have a handler,
 * the returned pointer is NULL.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] handler_ptr Returning pointer to LTA table entry usage handler,
 *                          or NULL if LTA handler is not defined.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_table_entry_usage_handler_get(int unit,
                                        bcmlrd_sid_t sid,
               const bcmltd_table_entry_usage_handler_t  **handler_ptr);

/*!
 * \brief Get the CTH table handler.
 *
 * This routine gets the Custom Table Handler interface for the given
 * logical table.  If the logical table does not have CTH implementation,
 * the returned pointer is NULL.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] handler_ptr Returning pointer to CTH handler, or NULL if
 *                          table is not CTH.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_cth_handler_get(int unit, bcmlrd_sid_t sid,
                          const bcmltd_table_handler_t **handler_ptr);

/*!
 * \brief Get the CTH table handler interface type.
 *
 * This routine gets the custom table handler interface type.
 * This information indicates LTM which flow to apply for the specified
 * custom table handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] intf Custom table handler interface type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_cth_handler_intf_type_get(int unit,
                                    bcmlrd_sid_t sid,
                                    bcmltm_cth_handler_intf_type_t *intf);

/*!
 * \brief Get the CTH table parameters.
 *
 * This routine gets the CTH table parameters for the given logical table.
 * This is used as the EE node cookie in the EE stage for the
 * LTA interface on LT with Custom Table Handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] table_params_ptr Returning pointer to CTH table parameters.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_cth_table_params_get(int unit, bcmlrd_sid_t sid,
                               const bcmltm_lta_table_params_t
                               **table_params_ptr);

/*!
 * \brief Get the CTH input field list.
 *
 * This routine gets the CTH input field list for the given logical table.
 * This is used as the FA node cookie in the FA stage for the
 * LTA interface on LT with Custom Table Handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fields_ptr Returning pointer to CTH input field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_cth_in_fields_get(int unit, bcmlrd_sid_t sid,
                            const bcmltm_lta_field_list_t **fields_ptr);

/*!
 * \brief Get the CTH input key field list.
 *
 * This routine gets the CTH input key field list for the given
 * logical table. This is used as the FA node cookie in the FA stage for the
 * LTA interface on LT with Custom Table Handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fields_ptr Returning pointer to CTH input key field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_cth_in_key_fields_get(int unit, bcmlrd_sid_t sid,
                                const bcmltm_lta_field_list_t **fields_ptr);

/*!
 * \brief Get the CTH output field list.
 *
 * This routine gets the CTH output field list for the given logical table.
 * This is used as the FA node cookie in the FA stage for the
 * LTA interface on LT with Custom Table Handler.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fields_ptr Returning pointer to CTH output field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_cth_out_fields_get(int unit, bcmlrd_sid_t sid,
                             const bcmltm_lta_field_list_t **fields_ptr);

/*!
 * \brief Get the LTM group FA LTM internal metadata for given table.
 *
 * This routine gets the LTM FA LTM internal metadata for given table.
 * This is used as the FA node cookie in the FA stage for LTM based
 * LTs (such as config or stats) to access the LT index.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] ltm_ptr Returning pointer to FA LTM metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_ltm_fa_ltm_get(int unit, bcmlrd_sid_t sid,
                         const bcmltm_fa_ltm_t **ltm_ptr);

/*!
 * \brief Get the LTM group key mapping.
 *
 * This routine gets the LTM group key mapping for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] ltm_key_ptr Returning pointer to LTM group key.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_ltm_key_get(int unit, bcmlrd_sid_t sid,
                      const bcmltm_field_select_mapping_t **ltm_key_ptr);

/*!
 * \brief Get the LTM group field spec list.
 *
 * This routine gets the LTM field spec list for the given logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] fs_list_ptr Returning pointer to field spec list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_ltm_fs_list_get(int unit, bcmlrd_sid_t sid,
                          const bcmltm_field_spec_list_t **fs_list_ptr);

#endif /* BCMLTM_DB_INTERNAL_H */
