/*! \file bcmltm_lta_cth_internal.h
 *
 * Interfaces to call initialize and cleanup routines for custom table
 * handlers that have requested such service.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_LTA_CTH_INTERNAL_H
#define BCMLTM_LTA_CTH_INTERNAL_H

/*!
 * \brief Call custom table handler initializer functions.
 *
 * Call custom table handler initializer functions for all custom
 * tables that have requested initialization service that are mapped
 * to the given unit.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
extern int
bcmltm_lta_cth_init(int unit);

/*!
 * \brief Call custom table handler cleanup functions.
 *
 * Call custom table handler cleanup functions for all custom tables
 * that have requested initialization service that are mapped to the
 * given unit. Initialization and cleanup are handled as a pair, so if
 * initialization was requested, then cleanup is called also.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
extern int
bcmltm_lta_cth_cleanup(int unit);

#endif /* BCMLTM_LTA_CTH_INTERNAL_H */
