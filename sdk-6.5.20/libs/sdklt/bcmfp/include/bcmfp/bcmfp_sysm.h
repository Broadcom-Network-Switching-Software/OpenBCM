/*! \file bcmfp_sysm.h
 *
 * BCMFP interface to system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_SYSM_H
#define BCMFP_SYSM_H

/*!
 * \brief Start FP component.
 *
 * Register FP component with System Manager.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int bcmfp_start(void);

#endif /* BCMFP_SYSM_H */
