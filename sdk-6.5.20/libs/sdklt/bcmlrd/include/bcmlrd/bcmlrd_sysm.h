/*! \file bcmlrd_sysm.h
 *
 * BCMLRD interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLRD_SYSM_H
#define BCMLRD_SYSM_H

/*!
 * \brief Starts the BCMLRD component.
 *
 * This function starts the BCMLRD component by registering
 * its initialization functions with the system manager.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int bcmlrd_start(void);

#endif /* BCMLRD_SYSM_H */

