/*! \file bcmptm_walr_local.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_WALR_LOCAL_H
#define BCMPTM_WALR_LOCAL_H

/*******************************************************************************
 * Includes
 */


/*******************************************************************************
 * Defines
 */


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * External global vars
 */


/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Check if WAL reader thread is in RUN state.
 *
 * \param [in] unit Logical device ID.
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_walr_run_check(int unit);

/*!
 * \brief Wake up WAL reader thread.
 *
 * \param [in] unit Logical device ID.
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_walr_wake(int unit);

#endif /* BCMPTM_WALR_LOCAL_H */
