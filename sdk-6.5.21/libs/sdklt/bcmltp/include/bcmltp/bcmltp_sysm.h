/*! \file bcmltp_sysm.h
 *
 * BCMLTP System manager interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTP_SYSM_H
#define BCMLTP_SYSM_H

/*!
 * \brief System manager start function.
 *
 * Called by System Manager to start BCMLTP.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmltp_start(void);

#endif /* BCMLTP_SYSM_H */
