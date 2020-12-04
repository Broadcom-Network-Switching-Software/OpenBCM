/*! \file bcmcth_agm_sysm.h
 *
 * BCMCTH AGM system manager initialization
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_AGM_SYSM_H
#define BCMCTH_AGM_SYSM_H

/*!
 * \brief Start LB component.
 *
 * Register LB component with System Manager.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_PARAM  Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int
bcmcth_agm_start(void);

#endif /* BCMCTH_AGM_SYSM_H */
