/*! \file bcmcth_port_sysm.h
 *
 * BCMCTH PORT component interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_PORT_SYSM_H
#define BCMCTH_PORT_SYSM_H

/*!
 * \brief Start PORT component.
 *
 * Register PORT component with System Manager.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int
bcmcth_port_start(void);

#endif /* BCMCTH_PORT_SYSM_H */
