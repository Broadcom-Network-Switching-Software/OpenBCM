/*! \file bcmcth_tnl_util.h
 *
 * TNL component utility function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TNL_UTIL_H
#define BCMCTH_TNL_UTIL_H

/*******************************************************************************
 * Includes
 */
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Generic hw mem/reg table read function for TNL component.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_tnl_hw_read(int unit, const bcmltd_op_arg_t *op_arg,
                   bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                   int index, void *entry_data);

/*!
 * \brief Generic hw mem/reg table write function for TNL component.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg The operation arguments.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_tnl_hw_write(int unit, const bcmltd_op_arg_t *op_arg,
                    bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                    int index, void *entry_data);

#endif /* BCMCTH_TNL_UTIL_H */
