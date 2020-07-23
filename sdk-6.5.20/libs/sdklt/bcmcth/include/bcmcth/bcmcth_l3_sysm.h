/*! \file bcmcth_l3_sysm.h
 *
 * Tunnel component interface to system manager.
 * This file contains tunnel custom handler component interface implementation to
 * System Manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_L3_SYSM_H
#define BCMCTH_L3_SYSM_H

/*!
 * \brief L3 component init.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_l3_start(void);

#endif /* BCMCTH_L3_SYSM_H */
