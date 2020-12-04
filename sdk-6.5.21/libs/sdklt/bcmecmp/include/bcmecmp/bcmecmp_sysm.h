/*! \file bcmecmp_sysm.h
 *
 * ECMP interface functions to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_SYSM_H
#define BCMECMP_SYSM_H

#include <sal/sal_types.h>

/*!
 * \brief ECMP component init.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmecmp_start(void);

#endif /* BCMECMP_SYSM_H */
