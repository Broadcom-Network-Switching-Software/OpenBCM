/*! \file bcmcfg_sysm.h
 *
 * BCMCFG system manager interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_SYSM_H
#define BCMCFG_SYSM_H

/*!
 * \brief System manager start function.
 *
 * Called by System Manager to start BCMCFG.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_start(void);

#endif /* BCMCFG_SYSM_H */
