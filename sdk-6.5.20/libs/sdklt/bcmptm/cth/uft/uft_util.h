/*! \file uft_util.h
 *
 * UFT component utility function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef UFT_UTIL_H
#define UFT_UTIL_H
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_uft_internal.h>

/*!
 * \brief Generic hw mem/reg table read function for UFT component.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] op_arg Operation arguments.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [in] entry_size Size of entry_data.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_uft_lt_hw_read(int unit, bcmltd_sid_t lt_id,
                      const bcmltd_op_arg_t *op_arg,
                      bcmdrd_sid_t pt_id, int index,
                      uint32_t entry_size, void *entry_data);

/*!
 * \brief Generic hw mem/reg table write function for UFT component.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] op_arg Operation arguments.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_uft_lt_hw_write(int unit, bcmltd_sid_t lt_id,
                       const bcmltd_op_arg_t *op_arg,
                       bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic interactive read function for UFT component.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Register identifier.
 * \param [in] index Logcial port identifier.
 * \param [in] entry_size Size of entry_data.
 * \param [in] entry_data New data value to set to register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_uft_lt_ireq_read(int unit, bcmltd_sid_t lt_id,
                        bcmdrd_sid_t pt_id, int index,
                        uint32_t entry_size, void *entry_data);

/*!
 * \brief Generic interactive write function for UFT component.
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
bcmptm_uft_lt_ireq_write(int unit, bcmltd_sid_t lt_id,
                         bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Get device em group info from IMM LT.
 *
 * \param [in] unit Unit number.
 * \param [in/out] grp Device em group pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int uft_lt_grp_get_from_imm(int unit, uft_dev_em_grp_t *grp);


/*!
 * \brief Generic hw mem/reg table write function for UFT Mgr.
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
bcmptm_uft_hw_write_dma(int unit, const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                        int start_index, int entry_cnt, void *entry_buf);

#endif /* UFT_UTIL_H */

