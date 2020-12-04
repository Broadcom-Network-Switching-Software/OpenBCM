/*! \file bcmcth_ldh_sysm.h
 *
 * Latency Distribution Histogram interface to system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_LDH_SYSM_H
#define BCMCTH_LDH_SYSM_H

/*!
 * \brief Start BCMCTH LDH component.
 *
 * Register BCMCTH LDH component with System Manager.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int bcmcth_ldh_start(void);

#endif /* BCMCTH_LDH_SYSM_H */
