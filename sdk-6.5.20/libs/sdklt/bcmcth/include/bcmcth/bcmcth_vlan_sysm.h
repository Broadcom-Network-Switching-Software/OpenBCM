/*! \file bcmcth_vlan_sysm.h
 *
 * BCMCTH VLAN component interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_VLAN_SYSM_H
#define BCMCTH_VLAN_SYSM_H

/*!
 * \brief Start VLAN component.
 *
 * Register VLAN component with System Manager.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int
bcmcth_vlan_start(void);

#endif /* BCMCTH_VLAN_SYSM_H */
