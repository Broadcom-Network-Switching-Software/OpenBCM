/*! \file bcmcth_ts_sysm.h
 *
 * BCMCTH TS system manager initialization
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TS_SYSM_H
#define BCMCTH_TS_SYSM_H

/*!
 * \brief Start TS component.
 *
 * Register TS component with System Manager.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_PARAM  Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int
bcmcth_ts_start(void);

#endif /* BCMCTH_TS_SYSM_H */
