/*! \file bcmcth_l2_sysm.h
 *
 * Header file of BCMCTH L2 interface to system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_L2_SYSM_H
#define BCMCTH_L2_SYSM_H

/*!
 * \brief Start L2 component.
 *
 * Register L2 component with System Manager.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int bcmcth_l2_start(void);

#endif /* BCMCTH_L2_SYSM_H */
