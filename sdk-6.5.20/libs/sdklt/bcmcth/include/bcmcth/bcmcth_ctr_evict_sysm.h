/*! \file bcmcth_ctr_evict_sysm.h
 *
 * BCMCTH CTR EVICT system manager initialization
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_CTR_EVICT_SYSM_H
#define BCMCTH_CTR_EVICT_SYSM_H

/*!
 * \brief Start CTR EVICT component.
 *
 * Register CTR EVICT component with System Manager.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_PARAM  Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int
bcmcth_ctr_evict_start(void);

#endif /* BCMCTH_CTR_EVICT_SYSM_H */
