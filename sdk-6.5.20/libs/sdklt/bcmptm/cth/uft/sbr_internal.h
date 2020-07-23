/*! \file sbr_internal.h
 *
 * SBR sub-component interface functions to UFT-MGR.
 *
 * This file contains function declarations internal to SBR and UFT-MGR.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SBR_INTERNAL_H
#define SBR_INTERNAL_H

#include <shr/shr_error.h>
#include <shr/shr_debug.h>

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Commit device tile mode set changes to HA.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_sbr_tile_mode_commit(int unit);

/*!
 * \brief Abort device tile mode set changes.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_sbr_tile_mode_abort(int unit);

#endif /* SBR_INTERNAL_H */
