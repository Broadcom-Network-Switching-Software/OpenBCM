/*! \file bcmltm_db_util.h
 *
 * Logical Table Manager - Utility interfaces.
 *
 * This file contains basic utility interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_DB_UTIL_H
#define BCMLTM_DB_UTIL_H

#include <shr/shr_types.h>

/*!
 * \brief Allocate array of uint64 values.
 *
 * This routine allocates an array of uint64 values.
 *
 * \param [in] count Number of elements to allocate.
 * \param [out] values_ptr Returning pointer to values array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_db_array_uint64_alloc(uint32_t count, uint64_t **values_ptr);

#endif /* BCMLTM_DB_UTIL_H */
