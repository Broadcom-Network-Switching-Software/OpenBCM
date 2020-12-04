/*! \file bcmcth_ctr_eflex_sysm.h
 *
 * BCMCTH CTR EFLEX system manager initialization
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_CTR_EFLEX_SYSM_H
#define BCMCTH_CTR_EFLEX_SYSM_H

/*!
 * \brief Start CTR EFLEX component.
 *
 * Register CTR EFLEX component with System Manager.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_PARAM  Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int
bcmcth_ctr_eflex_start(void);

/*!
 * \brief Start STATE EFLEX component.
 *
 * Register STATE EFLEX component with System Manager.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_PARAM  Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int
bcmcth_state_eflex_start(void);

#endif /* BCMCTH_CTR_EFLEX_SYSM_H */