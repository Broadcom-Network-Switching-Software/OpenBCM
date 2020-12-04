/*! \file bcmbd_sysm.h
 *
 * BCMBD interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_SYSM_H
#define BCMBD_SYSM_H

/*!
 * \brief Start BD component.
 *
 * Register BD component with System Manager.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int bcmbd_start(void);

#endif /* BCMBD_SYSM_H */
