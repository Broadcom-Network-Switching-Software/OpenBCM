/*! \file bcmlm_sysm.h
 *
 * Link Manager interface to system manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLM_SYSM_H
#define BCMLM_SYSM_H

/*!
 * \brief Start LM component.
 *
 * Register LM component with System Manager.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
extern int bcmlm_start(void);

#endif /* BCMLM_SYSM_H */
