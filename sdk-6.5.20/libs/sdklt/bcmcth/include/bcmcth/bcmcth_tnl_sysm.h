/*! \file bcmcth_tnl_sysm.h
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

#ifndef BCMCTH_TNL_SYSM_H
#define BCMCTH_TNL_SYSM_H

/*!
 * \brief Tunnel component init.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_tnl_start(void);

#endif /* BCMCTH_TNL_SYSM_H */
