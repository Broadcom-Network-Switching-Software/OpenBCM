/*! \file bcmimm_sysm.h
 *
 * In-Memory manager interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMIMM_SYSM_H
#define BCMIMM_SYSM_H

/*!
 * \brief Starts the in-memory front-end component.
 *
 * This function starts the in-memory front-end component by registering
 * its initialization functions with the system manager.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int bcmimm_start(void);

#endif

