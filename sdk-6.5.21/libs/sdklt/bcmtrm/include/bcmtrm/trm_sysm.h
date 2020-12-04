/*! \file trm_sysm.h
 *
 * Transaction manager interface to system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TRM_SYSM_H
#define TRM_SYSM_H

/*!
 * \brief BCMTRM Start function.
 *
 * This function must be called before any BCMTRM function. It register the TRM
 * with the system manager.
 *
 * \retval SHR_E_NONE Success, error code otherwise.
 */
extern int bcmtrm_start(void);

#endif
