/*! \file bcmcth_trunk_util.h
 *
 * TRUNK component utility function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_UTIL_H
#define BCMCTH_TRUNK_UTIL_H

/*******************************************************************************
 * Includes
 */
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmcth/bcmcth_trunk_types.h>


/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Generic hw mem/reg table read function for TRUNK component.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] start_index Start index of pt to read.
 * \param [in] entry_cnt Entry count to read.
 * \param [out] entry_buf Returned buffer of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_hw_read_dma(int unit, const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                         int start_index, int entry_cnt, void *entry_buf);

/*!
 * \brief Generic hw mem/reg table write function for TRUNK component.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] start_index Start index of pt to write.
 * \param [in] entry_cnt Entry count to read
 * \param [out] entry_buf New buffer of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_hw_write_dma(int unit, const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                          int start_index, int entry_cnt, void *entry_buf);

/*!
 * \brief Generic hw mem/reg table read function for TRUNK component.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_hw_read(int unit, const bcmltd_op_arg_t *op_arg,
                     bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                     int index, void *entry_data);

/*!
 * \brief Generic hw mem/reg table write function for TRUNK component.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_hw_write(int unit, const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                      int index, void *entry_data);

/*!
 * \brief Generic port type register read function for TRUNK component.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Register identifier.
 * \param [in] index Logcial port identifier.
 * \param [out] entry_data Returned data value of register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_pt_hw_read(int unit, const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                        int index, void *entry_data);

/*!
 * \brief Generic port type register write function for TRUNK component.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Register identifier.
 * \param [in] index Logcial port identifier.
 * \param [in] entry_data New data value to set to register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_pt_hw_write(int unit, const bcmltd_op_arg_t *op_arg,
                         bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                         int index, void *entry_data);

/*!
 * \brief Generic interactive read function for TRUNK component.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Register identifier.
 * \param [in] index Logcial port identifier.
 * \param [in] entry_data New data value to set to register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_ireq_read(int unit, bcmltd_sid_t lt_id,
                       bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic interactive write function for TRUNK component.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Register identifier.
 * \param [in] index Logcial port identifier.
 * \param [in] entry_data New data value to set to register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_trunk_ireq_write(int unit, bcmltd_sid_t lt_id,
                        bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Update trunk group parameter struct based on current grp info.
 *
 * \param [in] unit Unit number.
 * \param [in] param Group param struct to be updated.
 * \param [in] grp Current group info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_param_update(int unit, bcmcth_trunk_group_param_t *param,
                          bcmcth_trunk_group_t *grp);

/*!
 * \brief Update TRUNK_FAST group parameter struct based on current grp info.
 *
 * \param [in] unit Unit number.
 * \param [in] param Fast group param struct to be updated.
 * \param [in] grp Current fast group info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_fast_param_update(int unit,
                               bcmcth_trunk_fast_group_param_t *param,
                               bcmcth_trunk_fast_group_t *grp);

/*!
 * \brief Update TRUNK_FAILOVER entry parameter struct based on current grp info.
 *
 * \param [in] unit Unit number.
 * \param [in] param Fast group param struct to be updated.
 * \param [in] grp Current fast group info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_failover_param_update(int unit,
                                   bcmcth_trunk_failover_param_t *param,
                                   bcmcth_trunk_failover_t *grp);


/*!
 * \brief Get the minimum and maximum value of the given field.
 *
 * This function will get the minimum and maximum value of a LT field
 * from the LRD field definition.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fid Table Field.
 * \param [out] min Minimum field value.
 * \param [out] max Maximum field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the field definition.
 */
int
bcmcth_trunk_field_value_range_get(int unit, uint32_t sid, uint32_t fid,
                                   uint64_t *min, uint64_t *max);

/*!
 * \brief Get the value of a field from input array.
 *
 * \param [in] unit Unit number.
 * \param [in] in LTA input field array.
 * \param [in] fid Field ID.
 * \param [out] fval Field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
extern int
bcmcth_trunk_field_get(int unit, const bcmltd_fields_t *in, uint32_t fid,
                       uint64_t *fval);

/*!
 * \brief Free TRUNK_MEMBER entries resource.
 *
 * \param [in] unit Unit number.
 * \param [in] bmp TRUNK_MEMBER occupancy bitmap pointer.
 * \param [in] base Base of bitmap.
 * \param [in] num_entries Numober of entries.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_member_free(int unit, SHR_BITDCL *bmp, uint32_t base,
                         int num_entries);

/*!
 * \brief Get base pointer of TRUNK_MEMBER entries.
 *
 * \param [in] unit Unit number.
 * \param [in] bmp TRUNK_MEMBER occupancy bitmap pointer.
 * \param [in] tbl_size Size of TRUNK_MEMBER table.
 * \param [in] num_entries Numober of entries.
 * \param [out] base Returned base pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_member_base_get(int unit, SHR_BITDCL *bmp,
                             int tbl_size, int num_entries, uint32_t *base);

/*!
 * \brief Update max members of a trunk group, it may return a new base_ptr.
 *
 * \param [in] unit Unit number.
 * \param [in] bmp TRUNK_MEMBER occupancy bitmap pointer.
 * \param [in] tbl_size Size of TRUNK_MEMBER table.
 * \param [in] old_max Old max members of a trunk group.
 * \param [in] old_base Old base pointer of TRUNK_MEMBER.
 * \param [in] new_max New max members of a trunk group.
 * \param [out] new_base_ptr Returned base pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_trunk_max_members_update(int unit, SHR_BITDCL *bmp,
                                uint32_t tbl_size,
                                uint32_t old_max, uint32_t old_base,
                                uint32_t new_max, uint32_t *new_base_ptr);

/*!
 * \brief Get the field value, given the imm field list and field ID.
 *
 * \param [in]  unit Unit number.
 * \param [in]  in  List of imm fields.
 * \param [in]  fid Field ID.
 * \param [out] fval Field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_trunk_imm_field_get(int unit,
                           const bcmltd_field_t *in,
                           uint32_t fid,
                           uint64_t *fval);

/*!
 * \brief Validate the if the arrray index is less than the count.
 *
 * Parse  Checks if the member port index is in range
 *        of unicast or non-unicast member count.
 *
 * \param [in] unit Unit number.
 * \param [in] cnt  Count.
 * \param [in] idx  Array Index.
 *
 * \retval SHR_E_NONE if validation is successful.
 * \retval SHR_E_FAIL if validation failed.
 */
extern int
bcmcth_trunk_validate_member_idx(int unit,
                                 int cnt,
                                 int idx);
#endif /* BCMCTH_TRUNK_UTIL_H */

