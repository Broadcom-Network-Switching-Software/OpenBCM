/*! \file bcmevm_sysm.h
 *
 * Notify manager interface to system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMEVM_SYSM_H
#define BCMEVM_SYSM_H

/*!
 * \brief Starts the event manager component.
 *
 * This function starts the event manager component by registering
 * its initialization functions with the system manager.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int bcmevm_start(void);

#endif /* BCMEVM_SYSM_H */

