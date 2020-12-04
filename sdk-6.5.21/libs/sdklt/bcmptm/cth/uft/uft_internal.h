/*! \file uft_internal.h
 *
 * defines internal to UFT
 *
 * This file defines data structures of Unified Format Table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef UFT_INTERNAL_H
#define UFT_INTERNAL_H

#include <shr/shr_error.h>
#include <shr/shr_debug.h>

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Initialize imm callback function.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_uft_imm_register_init(int unit);

/*!
 * \brief Commit handler for DEVICE_EM_GROUP LT.
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_uft_dev_em_grp_commit(int unit);

/*!
 * \brief Abort handler for DEVICE_EM_GROUP LT.
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_uft_dev_em_grp_abort(int unit);

#endif /* UFT_INTERNAL_H */
