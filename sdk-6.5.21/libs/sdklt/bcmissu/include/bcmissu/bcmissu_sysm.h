/*! \file bcmissu_sysm.h
 *
 * ISSU start function decleration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMISSU_SYSM_H
#define BCMISSU_SYSM_H

/*!
 * \brief BCMISSU start function.
 *
 * This function should be called before any other ISSU API function.
 * This function registers the ISSU with the system manager.
 *
 * \retval SHR_E_NONE Success, error code otherwise.
 */
extern int bcmissu_start(void);

#endif /* BCMISSU_SYSM_H */
