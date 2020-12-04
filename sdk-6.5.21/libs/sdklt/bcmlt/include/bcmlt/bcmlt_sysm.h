/*! \file bcmlt_sysm.h
 *
 * BCMLT interface to system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLT_SYSM_H
#define BCMLT_SYSM_H

/*!
 * \brief Start LT component.
 *
 * Register LT component with System Manager.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int bcmlt_start(void);

#endif

